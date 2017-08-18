/* 
 * This source code is "Not a Contribution" under Apache license
 *
 * Sierra Wireless RIL
 *
 * Based on reference-ril by The Android Open Source Project
 * and U300 RIL by ST-Ericsson.
 * Modified by Sierra Wireless, Inc.
 *
 * Copyright (C) 2011 Sierra Wireless, Inc.
 * Copyright (C) ST-Ericsson AB 2008-2009
 * Copyright 2006, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Based on reference-ril by The Android Open Source Project.
 *
 * Heavily modified for ST-Ericsson U300 modems.
 * Author: Christian Bejram <christian.bejram@stericsson.com>
 */

#include <telephony/ril.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <alloca.h>
#include <getopt.h>
#include <sys/socket.h>
#include <cutils/sockets.h>
#include <termios.h>
#include <cutils/properties.h>
#include <signal.h>
#include <sys/wait.h>
#include "at_channel.h"
#include "at_tok.h"
#include "at_misc.h"
#include "SWIWWANCMAPI.h"
#include "swiril_main.h"
#include "swiril_config.h"
#include "swiril_gps.h"
#include "swiril_network.h"
#include "swiril_sim.h"
#include "swiril_requestdatahandler.h"
#include "swiril_cache.h"
#include "swims_ossdkuproto.h"
#include "swiril_oem.h"
#include "swiril_misc.h"
#include "swiril_sim_qmi.h"
#include "swiril_misc_qmi.h"
#include "swiril_misc_qmi.h"
#include "swiril_network_qmi.h"
#include "swiril_pdp_qmi.h"
/* For QMI */
#include "swiril_main_qmi.h"
#include "swiril_sms_qmi.h"
#include "swi_osapi.h"
#include "qmerrno.h"
#define LOG_TAG "RIL"
#include "swiril_log.h"

#ifdef HW_TI_OMAP
#include <hsmim.h>
#include <hardware/hardware.h>
static struct hw_module_t       *mod = NULL;
static struct hw_device_t       *dev = NULL;
static struct hsmim_device_t    *hsmim_dev = NULL;
#endif // HW_TI_OMAP

#define RIL_VERSION_STRING  "Sierra Ril V7.1.4 QMI"

#define MAX_AT_RESPONSE 0x1000

#define timespec_cmp(a, b, op)         \
        ((a).tv_sec == (b).tv_sec    \
        ? (a).tv_nsec op (b).tv_nsec \
        : (a).tv_sec op (b).tv_sec)

#define IMEISV_MC77XX_STRING  "IMEI SV: "
#define IMEISV_MC83XX_STRING  "SVN: "
#define IMEI_SV_STRING_LENGTH 20

/* RIL solicited command start from 1 */
#ifdef  SWI_RIL_VERSION_10
#define RIL_SOL_NUMBER_MAX  (RIL_REQUEST_SHUTDOWN + 1)
#else
#define RIL_SOL_NUMBER_MAX  (RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING + 1)
#define RIL_REQUEST_SHUTDOWN 129
#endif

/* AT Port Closing timeout (sec). The AT port is opened once at the start
 * of a transaction and remains open until this timeout elapses. The value 
 * is passed to the AT handler when at_open() is called during queueRunner
 * initialization. Units are Seconds
 */
#define AT_PORT_CLOSE_TIMEOUT 12

#ifdef ECC_LIST_INIT
#warning "ECC list enabled"
/** ECC Read list Macros */
#define ECC_LIST_SIM_READ_START_PTR 22
#define ECC_LIST_BUFFER_SIZE 100
#define ECC_LIST_SIM_READ_START_MAX_LIST 15
#define ECC_LIST_READ_SIM_DUMMY 0x46
#define ECC_LIST_ADD_COMMA 0x2C
#else
#warning "ECC list disabled"
#endif

/*** Declarations ***/
static void onRequest(int request, void *data, size_t datalen,
                      RIL_Token t);


static int isRadioOn();
static void signalCloseQueues(void);
extern const char *requestToString(int request);

extern ULONG getFileAttributesQMI(RIL_SIM_IO *pIoArg, RIL_SIM_IO_Response *pSr);
extern ULONG readRecordQMI( RIL_SIM_IO *pIoArg,RIL_SIM_IO_Response *pSr);
extern ULONG readBinaryQMI( RIL_SIM_IO *pIoArg,RIL_SIM_IO_Response *pSr);

/*** Static Variables ***/
static const RIL_RadioFunctions s_callbacks = {
    RIL_VERSION,
    onRequest,
    currentState,
    onSupports,
    onCancel,
    getVersion
};

static RIL_RadioState sState = RADIO_STATE_UNAVAILABLE;

static pthread_mutex_t s_state_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t s_screen_state_mutex = PTHREAD_MUTEX_INITIALIZER;
int s_screenState = 1;

static RIL_RequestHandlerFunc s_rilsolreqhandler[RIL_SOL_NUMBER_MAX];
static RIL_RequestHandlerFunc s_rilsolreqhandlercdma[RIL_SOL_NUMBER_MAX];


const char * default_properties[] = 
{

	// NON-AUDIO
    #ifdef RIL_GCF_TEST_FLAG	
	"persist.radio.ensen", "0",
	"persist.radio.gcfen", "1",
	"persist.radio.cben",  "01",
    #else
	"persist.radio.ensen", "1",
	"persist.radio.gcfen", "0",
	"persist.radio.cben",  "00",
    #endif
	/*
	"persist.ril.ecc1", "\"999\"", 	// Vodafone
	"persist.ril.ecc2", "\"000\"", 	// AU/NZ
	"persist.ril.ecc3", "\"???\"", 	// Unused
	*/
    #ifdef AUDIO_PROVISIONING
    #warning "Audio Provisioning enabled"
	// BLUETOOTH
	"persist.radio.volumelevel2","3",
	"persist.radio.txvolumegain2","ffff",
	"persist.radio.txagc2","0",
	"persist.radio.rxagc2","2",
	"persist.radio.codecstg2","0",
	"persist.radio.txnoofstages2","0",
	"persist.radio.rxnoofstages2","3",
	"persist.radio.rxstg1parameter2","00000000,00000000,5A6703DF,00000000,00000000",
	"persist.radio.rxstg2parameter2","00000000,00000000,5A6703DF,00000000,00000000",
	"persist.radio.rxstg3parameter2","00000000,00000000,5A6703DF,00000000,00000000",
	"persist.radio.avns2","0",
	"persist.radio.avec2","2",
	"persist.radio.rxvolumegain21","4b0",
	"persist.radio.rxvolumegain22","4b0",
	"persist.radio.rxvolumegain23","4b0",
	"persist.radio.rxvolumegain24","4b0",
	"persist.radio.rxvolumegain25","4b0",
	"persist.radio.rxvolumegain26","4b0",
	"persist.radio.rxvolumegain27","4b0",
	// CARKIT
	"persist.radio.volumelevel5","7",
	"persist.radio.txvolumegain5","ffff",
	"persist.radio.txagc5","0",
	"persist.radio.rxagc5","2",
	"persist.radio.codecstg5","0",
	"persist.radio.txnoofstages5","5",
	"persist.radio.txstg1parameter5","b5d92915,24df82c0,4c2e3d52,c7a4007a,1c0a2cc6",
	"persist.radio.txstg2parameter5","fb0fabe7,12674f20,40000000,088cae65,05c359f8",
	"persist.radio.txstg3parameter5","30747bad,0a83dbcc,40000000,053bcecc,eb4104a9",
	"persist.radio.txstg4parameter5","00000000,00000000,78bf2228,00000000,00000000",
	"persist.radio.txstg5parameter5","00000000,00000000,78bf2228,00000000,00000000",
	"persist.radio.rxnoofstages5","5",
	"persist.radio.rxstg1parameter5","babc2a8f,25bb2efd,3a3c9b2c,b276bbd5,24d73e52",
	"persist.radio.rxstg2parameter5","29a8c467,24461169,40000000,2a5f874b,1ffa200a",
	"persist.radio.rxstg3parameter5","df91e7c1,1a041b63,40000000,0c6ffe66,1172c048",
	"persist.radio.rxstg4parameter5","00000000,00000000,65c28f5c,00000000,00000000",
	"persist.radio.rxstg5parameter5","00000000,00000000,65c28f5c,00000000,00000000",
	"persist.radio.avns5","0", 
	"persist.radio.avec5","3",
	"persist.radio.rxvolumegain51","320",
	"persist.radio.rxvolumegain52","320",
	"persist.radio.rxvolumegain53","320",
	"persist.radio.rxvolumegain54","320",
	"persist.radio.rxvolumegain55","320",
	"persist.radio.rxvolumegain56","320",
	"persist.radio.rxvolumegain57","320",
	// HEADSET
	"persist.radio.volumelevel1","3",
	"persist.radio.txvolumegain1","ffff",
	"persist.radio.txagc1","0",
	"persist.radio.rxagc1","2",
	"persist.radio.codecstg1","800",
	"persist.radio.txnoofstages1","0",
	"persist.radio.rxnoofstages1","0",
	"persist.radio.avns1","0",
	"persist.radio.avec1","2",
	"persist.radio.rxvolumegain11","fa56",
	"persist.radio.rxvolumegain12","fcae",
	"persist.radio.rxvolumegain13","ff06",
	"persist.radio.rxvolumegain14","96",
	"persist.radio.rxvolumegain15","226",
	"persist.radio.rxvolumegain16","3b6",
	"persist.radio.rxvolumegain17","3b6",
	// SPEAKERPHONE (AAC)
	"persist.radio.volumelevel3a","3",
	"persist.radio.txvolumegain3a","ffff",
	"persist.radio.txagc3a","0",
	"persist.radio.rxagc3a","2",
	"persist.radio.codecstg3a","0",
	"persist.radio.txnoofstages3a","5",
	"persist.radio.txstg1parameter3a","f152fe6d,31d00cb6,696e0794,e754a4f2,51f92c60",
	"persist.radio.txstg2parameter3a","b311d37e,268ee6f7,40000000,aa408b5b,26b63ce1",
	"persist.radio.txstg3parameter3a","cb342de5,25e3a0dc,40000000,d5792785,22eeb091",
	"persist.radio.txstg4parameter3a","00000000,00000000,6570a3d7,00000000,00000000",
	"persist.radio.txstg5parameter3a","00000000,00000000,5a7ef9db,00000000,00000000",
	"persist.radio.rxnoofstages3a","5",
	"persist.radio.rxstg1parameter3a","d43b263f,27dfab98,3a946104,d221c1cf,224024fa",
	"persist.radio.rxstg2parameter3a","aa313a2f,21a24fa3,40000000,15b14b7f,1d9b084c",
	"persist.radio.rxstg3parameter3a","f47d61d2,1f8a7c8c,40000000,bc96657a,0bfe1683",
	"persist.radio.rxstg4parameter3a","00000000,00000000,40000000,00000000,00000000",
	"persist.radio.rxstg5parameter3a","00000000,00000000,40000000,00000000,00000000",
	"persist.radio.avns3a","0",
	"persist.radio.avec3a","3",
	"persist.radio.rxvolumegain3a1","f9c0",
	"persist.radio.rxvolumegain3a2","fc18",
	"persist.radio.rxvolumegain3a3","fe70",
	"persist.radio.rxvolumegain3a4","0",
	"persist.radio.rxvolumegain3a5","190",
	"persist.radio.rxvolumegain3a6","320",
	"persist.radio.rxvolumegain3a7","320",
	// SPEAKERPHONE (Traneat)
	"persist.radio.volumelevel3b","3",
	"persist.radio.txvolumegain3b","ffff",
	"persist.radio.txagc3b","0",
	"persist.radio.rxagc3b","2",
	"persist.radio.codecstg3b","0",
	"persist.radio.txnoofstages3b","5",
	"persist.radio.txstg1parameter3b","f152fe6d,31d00cb6,696e0794,e754a4f2,51f92c60",
	"persist.radio.txstg2parameter3b","b311d37e,268ee6f7,40000000,aa408b5b,26b63ce1",
	"persist.radio.txstg3parameter3b","cb342de5,25e3a0dc,40000000,d5792785,22eeb091",
	"persist.radio.txstg4parameter3b","00000000,00000000,6570a3d7,00000000,00000000",
	"persist.radio.txstg5parameter3b","00000000,00000000,5a7ef9db,00000000,00000000",
	"persist.radio.rxnoofstages3b","5",
	"persist.radio.rxstg1parameter3b","d43b263f,27dfab98,3a946104,d221c1cf,224024fa",
	"persist.radio.rxstg2parameter3b","aa313a2f,21a24fa3,40000000,15b14b7f,1d9b084c",
	"persist.radio.rxstg3parameter3b","f47d61d2,1f8a7c8c,40000000,bc96657a,0bfe1683",
	"persist.radio.rxstg4parameter3b","00000000,00000000,47ced916,00000000,00000000",
	"persist.radio.rxstg5parameter3b","00000000,00000000,40000000,00000000,00000000",
	"persist.radio.avns3b","0",
	"persist.radio.avec3b","3",
	"persist.radio.rxvolumegain3b1","f9c0",
	"persist.radio.rxvolumegain3b2","fc18",
	"persist.radio.rxvolumegain3b3","fe70",
	"persist.radio.rxvolumegain3b4","0",
	"persist.radio.rxvolumegain3b5","190",
	"persist.radio.rxvolumegain3b6","320",
	"persist.radio.rxvolumegain3b7","320",
	// HANDSET (AAC)
	"persist.radio.volumelevel0a","3",
	"persist.radio.txvolumegain0a","ffff",
	"persist.radio.txagc0a","0",
	"persist.radio.rxagc0a","1",
	"persist.radio.codecstg0a","100",
	"persist.radio.txnoofstages0a","3",
	"persist.radio.txstg1parameter0a","8f52a909,33dade48,20f9ce6e,c4f37f23,1be6ccf6",
	"persist.radio.txstg2parameter0a","4fc8b7df,1c1441c1,7eb851ec,02baef1d,37ff2f2d",
	"persist.radio.txstg3parameter0a","073a4ed1,18046db6,37a30627,3b0c80dd,0e94f440",
	"persist.radio.rxnoofstages0a","0",
	"persist.radio.avns0a","1",
	"persist.radio.avec0a","1",
	"persist.radio.rxvolumegain0a1","f98e",
	"persist.radio.rxvolumegain0a2","fbe6",
	"persist.radio.rxvolumegain0a3","fe3e",
	"persist.radio.rxvolumegain0a4","ffce",
	"persist.radio.rxvolumegain0a5","15e",
	"persist.radio.rxvolumegain0a6","2ee",
	"persist.radio.rxvolumegain0a7","2ee",
	// HANDSET (Traneat)
	"persist.radio.volumelevel0b","3",
	"persist.radio.txvolumegain0b","ffff",
	"persist.radio.txagc0b","0",
	"persist.radio.rxagc0b","1",
	"persist.radio.codecstg0b","100",
	"persist.radio.txnoofstages0b","3",
	"persist.radio.txstg1parameter0b","8f52a909,33dade48,20f9ce6e,c4f37f23,1be6ccf6 ",
	"persist.radio.txstg2parameter0b","4fc8b7df,1c1441c1,7eb851ec,02baef1d,37ff2f2d ",
	"persist.radio.txstg3parameter0b","073a4ed1,18046db6,37a30627,3b0c80dd,0e94f440 ",
	"persist.radio.rxnoofstages0b","5",
	"persist.radio.rxstg1parameter0b","F265DBA3,2935E95F,4DF76BFD,FAC9CF6D,33828B36",
	"persist.radio.rxstg2parameter0b","157E12B7,17761B83,40000000,0F4D3468,1D9BD35E",
	"persist.radio.rxstg3parameter0b","2EE9CE59,04BD4341,40000000,33EC7E94,04DC1DC7",
	"persist.radio.rxstg4parameter0b","00000000,00000000,47CF267D,00000000,00000000",
	"persist.radio.rxstg5parameter0b","00000000,00000000,40000000,00000000,00000000",
	"persist.radio.avns0b","1",
	"persist.radio.avec0b","1",
	"persist.radio.rxvolumegain0b1","f98e",
	"persist.radio.rxvolumegain0b2","fbe6",
	"persist.radio.rxvolumegain0b3","fe3e",
	"persist.radio.rxvolumegain0b4","ffce",
	"persist.radio.rxvolumegain0b5","15e",
	"persist.radio.rxvolumegain0b6","2ee",
	"persist.radio.rxvolumegain0b7","2ee",
    #else
    #warning "Audio Provisioning disabled"
    #endif // AUDIO_PROVISIONING
};

typedef struct RILRequest {
    int request;
    void *data;
    size_t datalen;
    RIL_Token token;
    struct RILRequest *next;
} RILRequest;

typedef struct RILEvent {
    void (*eventCallback) (void *param);
    void *param;
    struct timespec abstime;
    struct RILEvent *next;
    struct RILEvent *prev;
} RILEvent;

typedef struct RequestQueue {
    pthread_mutex_t queueMutex;
    pthread_cond_t cond;
    RILRequest *requestList;
    RILEvent *eventList;
    char enabled;
    char closed;
} RequestQueue;

static RequestQueue s_requestQueue = {
    .queueMutex = PTHREAD_MUTEX_INITIALIZER,
    .cond = PTHREAD_COND_INITIALIZER,
    .requestList = NULL,
    .eventList = NULL,
    .enabled = 1,
    .closed = 1
};

static RequestQueue s_requestQueuePrio = {
    .queueMutex = PTHREAD_MUTEX_INITIALIZER,
    .cond = PTHREAD_COND_INITIALIZER,
    .requestList = NULL,
    .eventList = NULL,
    .enabled = 0,
    .closed = 1
};

static RequestQueue *s_requestQueues[] = {
    &s_requestQueue,
    &s_requestQueuePrio
};

static const struct timeval TIMEVAL_0 = { 0, 0 };
static const struct timeval TIMEVAL_RADIO_POWER_POLL = { 2, 0 };
static const struct timeval TIMEVAL_GET_SMS_LIST = { 15, 0 };
static const struct timeval TIMEVAL_MODEM_POLL_LONG_INTERVAL = {60, 0 };
static const struct timeval TIMEVAL_MODEM_POLL_SHORT_INTERVAL = {30, 0 };
static const struct timeval TIMEVAL_2 = { 2, 0 };

/* To keep track of modem polling */
static int s_mdmrsp_fail_retry = 0;

/* Need to keep track of whether the modem supports Circuit Switched (CS)
   or is only Packet Switched (PS)
 */
static int s_modem_ps_only = 0;

#ifdef ECC_LIST_INIT
/** Read Ecc list variables */
static char extractedNums[ECC_LIST_BUFFER_SIZE];
static int numdigits;   
static char* listdecodeptr = NULL;
#endif

// store the netork selection mode in modem. 0=auto, 1=manual
extern int net_sel_mode_qmi;

/**
 *
 * Determine which technology is using by module
 *
 * @return
 *     enum SWI_RunningTech of technology
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     this function will also return false if the current or realtime 
 *     technology is not known, because the modem is not registered yet.  
 *     Thus, the default is GSM until the modem is registered.
 *
 */
static SWI_RunningTech getRunningTechnology(void)
{
    SWI_FW_INFO_TYPE technology;
    int runningtech = SWI_RUNNING_TECH_UNKNOWN;
    
    if (getFirmwareInfoTechQMI(&technology)) {
        switch(technology) {
            case SWI_FW_INFO_TYPE_GOBI_CDMA:
                LOGV("getRunningTechnology: CDMA\n");
                runningtech = SWI_RUNNING_TECH_CDMA;
                break;
                
            case SWI_FW_INFO_TYPE_GOBI_UMTS:
            case SWI_FW_INFO_TYPE_SWI_UMTS_ONLY:
                LOGV("getRunningTechnology: GSM/UMTS\n");
                runningtech = SWI_RUNNING_TECH_GSM;
                break;
                
            /* SWI_TBD the current decision is using RIL handler 
             * matching with the module running technology for MC7750 
             * liked modules, althoug it might cause mismatch in case of 
             * active handoff between LTE and eHRPD */
            case SWI_FW_INFO_TYPE_SWI_DUAL_MODE:
                if (isDualModeRunningLTE()) {
                    runningtech = SWI_RUNNING_TECH_GSM;
                }
                else {
                    runningtech = SWI_RUNNING_TECH_CDMA;
                }
                break;
                
            default:
                LOGE("getRunningTechnology: unexpected handler technology: %d!!!", runningtech);
                runningtech = SWI_RUNNING_TECH_UNKNOWN;
                break;
        }
    }
    
    return runningtech;
}

/**
 *
 * Determine whether modem is set for PS only
 *
 * @return
 *     TRUE: modem is PS only
 *     FALSE: modem is configured for PS and CS.
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 *
 */
int is_modem_ps_only(void)
{
    return s_modem_ps_only;
}


/**
 * Enqueue a RILEvent to the request queue. isPrio specifies in what queue
 * the request will end up.
 *
 * 0 = the "normal" queue, 1 = prio queue and 2 = both. If only one queue
 * is present, then the event will be inserted into that queue.
 */
void enqueueRILEvent(int isPrio, void (*callback) (void *param), 
                     void *param, const struct timeval *relativeTime)
{
    struct timespec tv;
    char done = 0;
    RequestQueue *q = NULL;

    RILEvent *e = malloc(sizeof(RILEvent));
    memset(e, 0, sizeof(RILEvent));

    e->eventCallback = callback;
    e->param = param;

    if (relativeTime == NULL) {
        relativeTime = alloca(sizeof(struct timeval));
        memset((struct timeval *) relativeTime, 0, sizeof(struct timeval));
    }
    
    clock_gettime(CLOCK_MONOTONIC, &tv);
    
    e->abstime.tv_sec = tv.tv_sec + relativeTime->tv_sec;
    e->abstime.tv_nsec = tv.tv_nsec + (relativeTime->tv_usec * 1000);

    if (e->abstime.tv_nsec > 1000000000) {
        e->abstime.tv_sec++;
        e->abstime.tv_nsec -= 1000000000;
    }

    if (!s_requestQueuePrio.enabled || 
        (isPrio == RIL_EVENT_QUEUE_NORMAL || isPrio == RIL_EVENT_QUEUE_ALL)) {
        q = &s_requestQueue;
    } else if (isPrio == RIL_EVENT_QUEUE_PRIO) {
        q = &s_requestQueuePrio;
    }

again:
    pthread_mutex_lock(&q->queueMutex);

    if (q->eventList == NULL) {
        q->eventList = e;
    } else {
        if (timespec_cmp(q->eventList->abstime, e->abstime, > )) {
            e->next = q->eventList;
            q->eventList->prev = e;
            q->eventList = e;
        } else {
            RILEvent *tmp = q->eventList;
            do {
                if (timespec_cmp(tmp->abstime, e->abstime, > )) {
                    tmp->prev->next = e;
                    e->prev = tmp->prev;
                    tmp->prev = e;
                    e->next = tmp;
                    break;
                } else if (tmp->next == NULL) {
                    tmp->next = e;
                    e->prev = tmp;
                    break;
                }
                tmp = tmp->next;
            } while (tmp);
        }
    }
    
    pthread_cond_broadcast(&q->cond);
    pthread_mutex_unlock(&q->queueMutex);

    if (s_requestQueuePrio.enabled && isPrio == RIL_EVENT_QUEUE_ALL && !done) {
        RILEvent *e2 = malloc(sizeof(RILEvent));
        memcpy(e2, e, sizeof(RILEvent));
        e = e2;
        done = 1;
        q = &s_requestQueuePrio;

        goto again;
    }

    return;
}

/**
 *
 * Turn on/off AT unsolicited notifications controlled by RIL ScreenState command.
 *
 * @param on_off
 *     if TRUE, screen is on and notifications should be enabled.
 *
 * @return
 *     zero     : success
 *     non-zero : failure
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 *
 */
static int setATUnsolNotifState(int on_off)
{
/* SWI_TBD in transition to QMI API */
#if 0
    int err=0;

    if (on_off) {
        /* Screen is on - enable all unsolicited notifications */

        /* Subscribe to network registration events. 
         *  n = 2 - Enable network registration and location information 
         *          unsolicited result code +CREG: <stat>[,<lac>,<ci>] 
         */
        err = at_send_command("AT+CREG=2", NULL);
        if (err < 0)
            return err;

        /* Configure Packet Domain Network Registration Status events
         *    2 = Enable network registration and location information
         *        unsolicited result code
         */
        err = at_send_command("AT+CGREG=2", NULL);
        if (err < 0)
            return err;

        /* Subscribe to Packet Domain Event Reporting.
         *  mode = 1 - Discard unsolicited result codes when ME-TE link is reserved
         *             (e.g. in on-line data mode); otherwise forward them directly
         *             to the TE.
         *   bfr = 0 - MT buffer of unsolicited result codes defined within this
         *             command is cleared when <mode> 1 is entered.
         */
        err = at_send_command("AT+CGEREP=2,0", NULL);
        if (err < 0)
            return err;

        /* Configure Mobile Equipment Event Reporting.
         *  mode = 1 - discard unsolicited result codes when TA-TE link is reserved
         *             (e.g. in on-line data mode); otherwise forward them directly
         *             to the TE.
         *  keyp = 0 - no keypad event reporting.
         *  disp = 0 - no display event reporting.
         *   ind = 1 - indicator event reporting using result code +CIEV: <ind>,<value>.
         *             <ind> indicates the indicator order number (as specified for
         *             +CIND) and <value> is the new value of indicator. Only those
         *             indicator events, which are not caused by +CIND shall be
         *             indicated by the TA to the TE.
         *   bfr = 0 - TA buffer of unsolicited result codes defined within this
         *             command is cleared when <mode> 1...3 is entered.
         */
        err = at_send_command("AT+CMER=1,0,0,1,0", NULL);
        if (err < 0)
            return err;

    } else {
        /* Screen is off - disable all unsolicited notifications. */

        /* SWI_TBD
           According to the comment in ril.h, it is only cell updates that
           should be disabled, in which case +CREG and +CGREG should be set
           to 1 instead of 0.  This would also correspond better with the
           location updates command. Need to think about this some more.
         */
        err = at_send_command("AT+CREG=0", NULL);
        if (err < 0)
            return err;
        err = at_send_command("AT+CGREG=0", NULL);
        if (err < 0)
            return err;
        err = at_send_command("AT+CGEREP=0,0", NULL);
        if (err < 0)
            return err;
        err = at_send_command("AT+CMER=0,0,0,0,0", NULL);
        if (err < 0)
            return err;
    }
#endif
    /* everything was okay, as indicated by a return value of zero */
    return 0;
}

#ifdef ECC_LIST_INIT
static void readNVForEmergencyNumbers()
{
    bool bRet = false;
    enum eGobiImageCarrier image;

    LOGD("%s: Entered",__func__);

    bRet = getFirmwareInfoCarrierQMI(&image);

    /* Set the android property with required emergency numbers */
    if (bRet && (image == eGOBI_IMG_CAR_ATT)) {
        property_set(ECCLIST_PROPERTY, "911,112,08,000,110,118,119,999");
    }
    else if (bRet && ((image == eGOBI_IMG_CAR_SPRINT) || (image == eGOBI_IMG_CAR_VERIZON))) {
         property_set(ECCLIST_PROPERTY, "911,*911,#911");
    }
    else if (bRet && ((image == eGOBI_IMG_CAR_CHINA_TELECOM) ||
        (image == eGOBI_IMG_CAR_GENERIC_CDMA))) { 
        property_set(ECCLIST_PROPERTY, "110,112,120,122,119");
    }
    ecclistReadModemNVSet();
}
#endif

/** Do post-AT+CFUN=1 initialization. */
static void onRadioPowerOn()
{
    resetpollSIMRetry();
    enqueueRILEvent(RIL_EVENT_QUEUE_PRIO, pollSIMState, NULL, NULL);
}

/** Do post- SIM ready initialization. */
static void onSIMReady()
{
    int err = 0;

    /* Enable automatic time zone update via NITZ */
    err = at_send_command("AT+CTZU=1", NULL);
    if (err < 0)
        LOGE("%s AT+CTZU failed", __func__);

#if 0
    ATResponse *atresponse = NULL;
    int err = 0;

    /* If the screen is currently on, then turn on all AT unsolicited notifications
       related to the ScreenState command. We don't need locking here, since the
       return value fits into a single word.

       IMPORTANT: This relies on the initial value of the screen to be on, so that
       these get enabled during system initialization.
     */
    if (getScreenState()) {
        setATUnsolNotifState(1);
    }

    /* SWI_TBD
       Should TZ reporting be turned off when the screen is off.  If it is, then
       a fake NITZ event should be generated when the screen is turned back on.
     */
    /* Configure time zone change event reporting
     *    1 = Enable time zone change event reporting.
     */
    at_send_command("AT+CTZR=1", NULL);
#endif

#ifdef ECC_LIST_INIT
    /** Read and Set Emergency call List */
    if (isVoiceEnabled()) {
        ecclistReadSimCardSet(); 
    }
#endif
    smsServiceInit();
}

/**
 * RIL_REQUEST_GET_IMSI
*/
static void requestGetIMSI(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err;

    err = at_send_command_numeric("AT+CIMI", &atresponse);

    if (err < 0 || atresponse->success == 0) {
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    } else {
        /* Return the successful response and cache the value for next time */
        swiril_cache_RIL_onRequestComplete(t, RIL_E_SUCCESS, 
                                           atresponse->p_intermediates->line,
                                           sizeof(char *),
                                           RIL_REQUEST_GET_IMSI);
    }
    at_response_free(atresponse);
    return;
}

/**
 * Get IMEISV
*/
int getIMEISV(char **datap)
{
    ATResponse *atresponse = NULL;
    char *bufp = NULL;
    int  err;
    char *imeiSVString = NULL;
    SWI_FW_INFO_TYPE technology;

    if(IsSierraDevice()) {
        imeiSVString = IMEISV_MC77XX_STRING;
    } else {
        imeiSVString = IMEISV_MC83XX_STRING;
    }

    /* ATI response after IMEI SV will go to onUnsolicited() */
    err = at_send_command_singleline("ATI", imeiSVString, &atresponse);
    if (err < 0 || atresponse->success == 0) 
    {
        at_response_free(atresponse);
        return -1;
    }

    bufp = strstr(atresponse->p_intermediates->line, imeiSVString);
    if (bufp != NULL)
    {
        asprintf(datap, "%s", bufp + strlen(imeiSVString));
    }
    else
    {
        datap = NULL;
    }
    at_response_free(atresponse);

    return err;
}

/* RIL_REQUEST_DEVICE_IDENTITY
 *
 * Request the device ESN / MEID / IMEI / IMEISV.
 *
 */
static void requestDeviceIdentity(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    char* response[4];
    int err;
    int i;

    /* IMEI */ 
    err = at_send_command_numeric("AT+CGSN", &atresponse);

    if (err < 0 || atresponse->success == 0) {
        goto error;
    } else {
        asprintf(&response[0], "%s", atresponse->p_intermediates->line);
    }

    /* IMEISV */
    err = getIMEISV(&response[1]);

    if (err < 0) {
        goto error;
    }

    /* CDMA not supported */
    response[2] = NULL;
    response[3] = NULL;

    /* Return the successful response and cache the value for next time */
    swiril_cache_RIL_onRequestComplete(t, RIL_E_SUCCESS,
                                       &response,
                                       sizeof(response),
                                       RIL_REQUEST_DEVICE_IDENTITY);

finally:
    for (i=0; i<4; i++)
    {
        if (response[i] != NULL)
        {
            free(response[i]);
        }
    }
    at_response_free(atresponse);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/* Deprecated */
/**
 * RIL_REQUEST_GET_IMEI
 *
 * Get the device IMEI, including check digit.
*/
static void requestGetIMEI(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err;

    err = at_send_command_numeric("AT+CGSN", &atresponse);

    if (err < 0 || atresponse->success == 0) {
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    } else {
        /* Return the successful response and cache the value for next time */
        swiril_cache_RIL_onRequestComplete(t, RIL_E_SUCCESS,
                                           atresponse->p_intermediates->line,
                                           sizeof(char *),
                                           RIL_REQUEST_GET_IMEI);
    }
    at_response_free(atresponse);
    return;
}

/* Deprecated */
/**
 * RIL_REQUEST_GET_IMEISV
 *
 * Get the device IMEISV, which should be two decimal digits.
*/
static void requestGetIMEISV(void *data, size_t datalen, RIL_Token t)
{
    int err;
    char *response = NULL;

    err = getIMEISV(&response);

    if (err < 0) 
    {
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE,
                              NULL,
                              0);
    }
    else
    {
        /* Return the successful response and cache the value for next time.
           Note that the response could be a NULL string.
         */
        swiril_cache_RIL_onRequestComplete(t, RIL_E_SUCCESS,
                                           response,
                                           sizeof(char *),
                                           RIL_REQUEST_GET_IMEISV);
    }
    if (response != NULL)
        free(response);
}

/**
 * RIL_REQUEST_RADIO_POWER
 *
 * Toggle radio on and off (for "airplane" mode).
*/
static void requestRadioPower(void *data, size_t datalen, RIL_Token t)
{
    int onOff;
    int err;
    ATResponse *atresponse = NULL;

    assert(datalen >= sizeof(int *));
    onOff = ((int *) data)[0];

    if (onOff == 0 && sState != RADIO_STATE_OFF) {
        err = at_send_command("AT+CFUN=0", &atresponse);
        if (err < 0 || atresponse->success == 0)
            goto error;
        setRadioState(RADIO_STATE_OFF);
    } else if (onOff > 0 && sState == RADIO_STATE_OFF) {
        err = at_send_command("AT+CFUN=1", &atresponse);
        if (err < 0 || atresponse->success == 0) {
            goto error;
        }
        setRadioState(RADIO_STATE_SIM_NOT_READY);
    } else {
        LOGE("Erroneous input to requestRadioPower(): onOff=%i, sState=%i", onOff, sState);
        goto error;
    }
    
    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
finally:
    at_response_free(atresponse);
    return;
error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * Will LOCK THE MUTEX! MAKE SURE TO RELEASE IT!
 */
void getScreenStateLock(void)
{
    /* Just make sure we're not changing anything with regards to screen state. */
    pthread_mutex_lock(&s_screen_state_mutex);
}

int getScreenState(void)
{
    return s_screenState;
}

void releaseScreenStateLock(void)
{
    pthread_mutex_unlock(&s_screen_state_mutex);
}

/**
 * RIL_REQUEST_BASEBAND_VERSION
 *
 * Return string value indicating baseband version, eg
 * response from AT+CGMR.
*/
static void requestBasebandVersion(void *data, size_t datalen, RIL_Token t)
{
    int err;
    ATResponse *atresponse = NULL;
    char *line;

    /* The AT command output is a single line with no initial prefix string,
       so use the empty string as the response prefix.
     */
    err = at_send_command_singleline("AT+CGMR", "", &atresponse);

    if (err < 0 || 
        atresponse->success == 0 || 
        atresponse->p_intermediates == NULL) {
        goto error;
    }

    line = atresponse->p_intermediates->line;

    /* Return the successful response and cache the value for next time */
    swiril_cache_RIL_onRequestComplete(t, RIL_E_SUCCESS, line, sizeof(char *),
                                       RIL_REQUEST_BASEBAND_VERSION);

finally:
    at_response_free(atresponse);
    return;

error:
    LOGE("Error in requestBasebandVersion()");
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

static char isPrioRequest(int request)
{
    unsigned int i;
    for (i = 0; i < sizeof(prioRequests) / sizeof(int); i++)
        if (request == prioRequests[i])
            return 1;
    return 0;
}

static void processRequest(int request, void *data, size_t datalen, RIL_Token t)
{
    LOGE("processRequest: %s", requestToString(request));

    /*Android 6 need to report not support instead of radio not available */
    #ifdef SWI_RIL_VERSION_11
    if (request == RIL_REQUEST_GET_RADIO_CAPABILITY) {
        RIL_onRequestComplete(t, RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0);
        return;
    }
    #endif

    /* Ignore all requests except RIL_REQUEST_GET_SIM_STATUS and RIL_REQUEST_SHUTDOWN
     * when RADIO_STATE_UNAVAILABLE.
     */
    if (sState == RADIO_STATE_UNAVAILABLE
        && !(request == RIL_REQUEST_GET_SIM_STATUS ||
             request == RIL_REQUEST_ALLOW_DATA ||
             request == RIL_REQUEST_SHUTDOWN)) {
        RIL_onRequestComplete(t, RIL_E_RADIO_NOT_AVAILABLE, NULL, 0);
        return;
    }

    /* Ignore all non-power requests when RADIO_STATE_OFF
     * (except RIL_REQUEST_GET_SIM_STATUS and a few more).
     */
    if ((sState == RADIO_STATE_OFF || sState == RADIO_STATE_SIM_NOT_READY)
        && !(request == RIL_REQUEST_RADIO_POWER || 
             /*not allow to get SIM status if SIM not ready
               because if we are in SIM ready delay stage,  
               get SIM status will set SIM status to ready */
             /*request == RIL_REQUEST_GET_SIM_STATUS ||*/
             request == RIL_REQUEST_GET_IMEISV ||
             request == RIL_REQUEST_GET_IMEI ||
             request == RIL_REQUEST_SET_BAND_MODE ||
             request == RIL_REQUEST_BASEBAND_VERSION || 
             request == RIL_REQUEST_OEM_HOOK_STRINGS ||
             request == RIL_REQUEST_DEVICE_IDENTITY ||
             request == RIL_REQUEST_SCREEN_STATE ||
             request == RIL_REQUEST_ALLOW_DATA ||
             request == RIL_REQUEST_SHUTDOWN)) {
        RIL_onRequestComplete(t, RIL_E_RADIO_NOT_AVAILABLE, NULL, 0);
        return;
    }
    
    /* 
     * These commands won't accept RADIO_NOT_AVAILABLE, so we just return
     * GENERIC_FAILURE if we're not in SIM_STATE_READY or NV_STATE_READY.
     */
    if ( (sState != RADIO_STATE_SIM_READY && sState != RADIO_STATE_NV_READY && sState != RADIO_STATE_RUIM_READY)
        && (request == RIL_REQUEST_WRITE_SMS_TO_SIM ||
            request == RIL_REQUEST_DELETE_SMS_ON_SIM)) {
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    /* Don't allow radio operations when sim is absent or locked! */
    if (sState == RADIO_STATE_SIM_LOCKED_OR_ABSENT
        && !(request == RIL_REQUEST_ENTER_SIM_PIN ||
             request == RIL_REQUEST_ENTER_SIM_PUK ||
             request == RIL_REQUEST_ENTER_SIM_PIN2 ||
             request == RIL_REQUEST_ENTER_SIM_PUK2 ||
             request == RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION ||
             request == RIL_REQUEST_GET_SIM_STATUS ||
             /*SIM IO is necessary even SIM PIN locked since Android 5.1*/
             request == RIL_REQUEST_SIM_IO ||
             request == RIL_REQUEST_RADIO_POWER ||
             request == RIL_REQUEST_GET_IMEISV ||
             request == RIL_REQUEST_GET_IMEI ||
             request == RIL_REQUEST_BASEBAND_VERSION ||
             request == RIL_REQUEST_OPERATOR||
             request == RIL_REQUEST_VOICE_REGISTRATION_STATE||
             request == RIL_REQUEST_DATA_REGISTRATION_STATE||
             request == RIL_REQUEST_SIGNAL_STRENGTH||
             request == RIL_REQUEST_DIAL||
             request == RIL_REQUEST_HANGUP||
             request == RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND||
             request == RIL_REQUEST_ANSWER||
             request == RIL_REQUEST_GET_MUTE||
             request == RIL_REQUEST_SET_MUTE||
             request == RIL_REQUEST_LAST_CALL_FAIL_CAUSE||
             request == RIL_REQUEST_GET_CURRENT_CALLS||
             request == RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE||
             request == RIL_REQUEST_GET_CURRENT_CALLS||
             request == RIL_REQUEST_QUERY_FACILITY_LOCK ||
             request == RIL_REQUEST_SET_FACILITY_LOCK ||
             request == RIL_REQUEST_OEM_HOOK_STRINGS ||
             request == RIL_REQUEST_DEVICE_IDENTITY ||
             request == RIL_REQUEST_GET_IMSI ||
             request == RIL_REQUEST_SCREEN_STATE ||
             request == RIL_REQUEST_ALLOW_DATA ||
             request == RIL_REQUEST_SHUTDOWN )) {
        RIL_onRequestComplete(t, RIL_E_RADIO_NOT_AVAILABLE, NULL, 0);
        return;
    }

    /* Check if network mode is changed. If not, just return success. BZ48550 */
    if (request == RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC && net_sel_mode_qmi == QMI_REG_TYPE_AUTOMATIC) {
        LOGE("module already in auto mode");
        RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
        return;
    }

    /* If the command could affect network state, clear the network cache data
     */
    if (request == RIL_REQUEST_RADIO_POWER ||
        request == RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC ||
        request == RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL ||
        request == RIL_REQUEST_SET_BAND_MODE ||
        request == RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE) {

        swiril_cache_clearnetworkdata();
    }

    /* If the RIL command supports caching, check the cache first */
    if (request == RIL_REQUEST_REGISTRATION_STATE ||
        request == RIL_REQUEST_GPRS_REGISTRATION_STATE ||
        request == RIL_REQUEST_SIGNAL_STRENGTH ||
        request == RIL_REQUEST_OPERATOR ||
        request == RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE ||
        request == RIL_REQUEST_GET_IMSI ||
        request == RIL_REQUEST_DEVICE_IDENTITY ||
        request == RIL_REQUEST_GET_IMEI ||
        request == RIL_REQUEST_GET_IMEISV ||
        request == RIL_REQUEST_BASEBAND_VERSION) {

        void *responsep;
        int responselen;
        
        swiril_cache_get(request, &responsep, &responselen);
        if (responsep != NULL) {
            RIL_onRequestComplete(t, RIL_E_SUCCESS, responsep, responselen);
            return;
        }
    }

    /* check handler struct first, call handler if existing */
    if ((request > 0) && (request < RIL_SOL_NUMBER_MAX)) {
        if (getRunningTechnology() != SWI_RUNNING_TECH_UNKNOWN) {
            if ((getRunningTechnology() == SWI_RUNNING_TECH_GSM) && (s_rilsolreqhandler[request] != NULL)) {
                s_rilsolreqhandler[request](data, datalen, t);
                return;
            }
            else if (getRunningTechnology() == SWI_RUNNING_TECH_CDMA) {
                if (s_rilsolreqhandlercdma[request] != NULL) {
                    s_rilsolreqhandlercdma[request](data, datalen, t);
                    return;
                }
                /* fall through for RUIM SIM related of AT handle */
            }
        }
        else {
            /* initialize again */
            initFirmwareInfoQMI();
            RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
            return;
        }
    }
    
    switch (request) {
        /* SIM Handling Requests */
        case RIL_REQUEST_SIM_IO:
            requestSIM_IOQMI(data, datalen, t);
            break;
        case RIL_REQUEST_GET_SIM_STATUS:
            requestGetSimStatus(data, datalen, t);
            break;
        case RIL_REQUEST_QUERY_FACILITY_LOCK:
            /* SL9090 PTCRB need to stick on AT for test cases 31.8.3.1 & 31.8.3.1.2 & 31.8.4.1 */
            if (isFirmwareRunningCDMA())
                requestQueryFacilityLockQMI(data, datalen, t);
            else
                requestQueryFacilityLock(data, datalen, t);
            break;
        case RIL_REQUEST_SET_FACILITY_LOCK:
            /* SL9090 PTCRB need to stick on AT for test cases 31.8.3.1 & 31.8.3.1.2 & 31.8.4.1 */
            if (isFirmwareRunningCDMA())
                requestSetFacilityLockQMI(data, datalen, t);
            else
                requestSetFacilityLock(data, datalen, t);
            break;

        /* Call related requests */
        case RIL_REQUEST_GET_CLIR:
            requestGetCLIR(data, datalen, t);
            break;
        case RIL_REQUEST_SET_CLIR:
            requestSetCLIR(data, datalen, t);
            break;

        /* Network Selection */
        case RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION:
            if (isFirmwareRunningCDMA())
                requestEnterNetworkDepersonalizationQMI(data, datalen, t);
            else
                requestEnterNetworkDepersonalization(data, datalen, t);
            break;
#ifdef SWI_RIL_VERSION_9        
        /* SMS on IMS */    
        case RIL_REQUEST_IMS_REGISTRATION_STATE:
            requestQueryIMSRegistrationState(data, datalen, t);
            break;

        case RIL_REQUEST_IMS_SEND_SMS:
            requestIMSSendSMS(data, datalen, t);
            break;

        case RIL_REQUEST_SET_INITIAL_ATTACH_APN:
            requestSetInitialAPN(data, datalen, t);
            break;
#endif
        default:
            LOGW("Unsupported request logged: %s",
                 requestToString(request));
            RIL_onRequestComplete(t, RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0);
            break;
    }
}

/*** Callback methods from the RIL library to us ***/

/**
 * Call from RIL to us to make a RIL_REQUEST.
 *
 * Must be completed with a call to RIL_onRequestComplete().
 */
static void onRequest(int request, void *data, size_t datalen, RIL_Token t)
{
    RILRequest *r;
    char done = 0;
    RequestQueue *q = &s_requestQueue;

    if (s_requestQueuePrio.enabled && isPrioRequest(request))
        q = &s_requestQueuePrio;

    r = malloc(sizeof(RILRequest));  
    memset(r, 0, sizeof(RILRequest));

    /* Formulate a RILRequest and put it in the queue. */
    r->request = request;
    r->data = dupRequestData(request, data, datalen);
    r->datalen = datalen;
    r->token = t;

    pthread_mutex_lock(&q->queueMutex);

    /* Queue empty, just throw r on top. */
    if (q->requestList == NULL) {
        q->requestList = r;
    } else {
        RILRequest *l = q->requestList;
        while (l->next != NULL)
            l = l->next;

        l->next = r;
    }

    pthread_cond_broadcast(&q->cond);
    pthread_mutex_unlock(&q->queueMutex);
}

/**
 * Synchronous call from the RIL to us to return current radio state.
 * RADIO_STATE_UNAVAILABLE should be the initial state.
 */
RIL_RadioState currentState()
{
    return sState;
}

/**
 * Call from RIL to us to find out whether a specific request code
 * is supported by this implementation.
 *
 * Return 1 for "supported" and 0 for "unsupported".
 *
 * Currently just stubbed with the default value of one. This is currently
 * not used by android, and therefore not implemented here. We return
 * RIL_E_REQUEST_NOT_SUPPORTED when we encounter unsupported requests.
 */
int onSupports(int requestCode)
{
    LOGI("onSupports() called!");

    return 1;
}

/** 
 * onCancel() is currently stubbed, because android doesn't use it and
 * our implementation will depend on how a cancellation is handled in 
 * the upper layers.
 */
void onCancel(RIL_Token t)
{
    LOGI("onCancel() called!");
}

const char *getVersion(void)
{
    return RIL_VERSION_STRING;
}

static void pollSimStateCDMA(void *params)
{
    LOGD("%s:Entered",__func__);
    resetpollSIMRetry();
    enqueueRILEvent(RIL_EVENT_QUEUE_PRIO, pollSIMState, NULL, NULL);
}

static void pollRadioPower(void *params)
{
    LOGD("Enter pollRadioPower");
    
    /* Doesn't really poll anything */
    setRadioState(RADIO_STATE_NV_READY);
}

void setRadioState(RIL_RadioState newState)
{
    RIL_RadioState oldState;
    enum eGobiImageCarrier image;

    pthread_mutex_lock(&s_state_mutex);

    oldState = sState;

    if (sState != newState) {
        sState = newState;
    }

    pthread_mutex_unlock(&s_state_mutex);

    /* Do these outside of the mutex. */
    if ((sState != oldState) || 
        (sState == RADIO_STATE_SIM_LOCKED_OR_ABSENT) ||
        (sState == RADIO_STATE_RUIM_LOCKED_OR_ABSENT)) {
        RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED,
                                  NULL, 0);

        /**
         * Get the unread SMS list from the device and notify application side,
         * if any unread SMSs are present
         */
        if (sState == RADIO_STATE_SIM_READY) {
            enqueueRILEvent(RIL_EVENT_QUEUE_PRIO, onSIMReady, NULL, NULL);
            enqueueRILEvent(RIL_EVENT_QUEUE_PRIO, getSMSList, NULL,
                                                 &TIMEVAL_GET_SMS_LIST);
            enqueueRILEvent(RIL_EVENT_QUEUE_NORMAL, getVMInfo, NULL, NULL);
        } else if (sState == RADIO_STATE_RUIM_READY) {
            enqueueRILEvent(RIL_EVENT_QUEUE_PRIO, onSIMReady, NULL, NULL);
            enqueueRILEvent(RIL_EVENT_QUEUE_PRIO, getSMSList, NULL,
                                                 &TIMEVAL_GET_SMS_LIST);
            enqueueRILEvent(RIL_EVENT_QUEUE_NORMAL, getVMInfo, NULL, NULL);
        } else if ((sState == RADIO_STATE_SIM_LOCKED_OR_ABSENT) ||
                   (sState == RADIO_STATE_RUIM_LOCKED_OR_ABSENT)) {
          #ifdef ECC_LIST_INIT
            // For GSM/UMTS and CDMA having SIM/RUIM functionality
            enqueueRILEvent(RIL_EVENT_QUEUE_PRIO,readNVForEmergencyNumbers, NULL, NULL);
          #endif
        } else if (sState == RADIO_STATE_NV_READY) {           
            enqueueRILEvent(RIL_EVENT_QUEUE_PRIO, getSMSList, NULL,
                                                 &TIMEVAL_GET_SMS_LIST);
            enqueueRILEvent(RIL_EVENT_QUEUE_NORMAL, getVMInfo, NULL, NULL);
            if (getFirmwareInfoCarrierQMI(&image) && ((image == eGOBI_IMG_CAR_CHINA_TELECOM) ||
                (image == eGOBI_IMG_CAR_GENERIC_CDMA))) {
                enqueueRILEvent(RIL_EVENT_QUEUE_PRIO, pollSimStateCDMA, NULL, NULL);
            }
            else if (getFirmwareInfoCarrierQMI(&image) && 
                    ((image == eGOBI_IMG_CAR_SPRINT) || (image == eGOBI_IMG_CAR_VERIZON))) {
            #ifdef ECC_LIST_INIT
                // For without SIM functionality CDMA
                enqueueRILEvent(RIL_EVENT_QUEUE_PRIO,readNVForEmergencyNumbers, NULL, NULL);
            #endif
            }
        } else if ((sState == RADIO_STATE_SIM_NOT_READY) ||
                   (sState == RADIO_STATE_RUIM_NOT_READY)) {
            enqueueRILEvent(RIL_EVENT_QUEUE_NORMAL, onRadioPowerOn, NULL, NULL);
        } else if (sState == RADIO_STATE_NV_NOT_READY) {
            /* add a 2 seconds delay to set RADIO_STATE_NV_READY */
            enqueueRILEvent(RIL_EVENT_QUEUE_NORMAL, pollRadioPower,
                            NULL, &TIMEVAL_RADIO_POWER_POLL);
        }
    }
}

/** Returns 1 if on, 0 if off, and -1 on error. */
static int isRadioOn()
{
    ATResponse *atresponse = NULL;
    int err;
    char *line;
    int ret;

    err = at_send_command_singleline("AT+CFUN?", "+CFUN:", &atresponse);
    if (err < 0 || atresponse->success == 0) {
        /* Assume radio is off. */
        goto error;
    }

    line = atresponse->p_intermediates->line;

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &ret);
    if (err < 0)
        goto error;

    switch (ret) {
        case 1:         /* Full functionality (switched on) */
            ret = 1;
            break;

        default:
            ret = 0;
    }

    at_response_free(atresponse);
    return ret;

error:
    at_response_free(atresponse);
    return -1;
}

/** 
 * Common initialization for all AT channels.
 *
 * Note that most of the AT unsolicited result code reporting is only turned
 * on once the SIM is ready, in onSIMReady().
 */
static char initializeCommon(void)
{
    int err = 0;
    ATResponse *atresponse = NULL;
    char *cmd = NULL;

    if (at_handshake() < 0) {
        LOG_FATAL("Handshake failed!");
        goto error;
    }

    /* Configure/set
     *   command echo (E), result code suppression (Q), DCE response format (V)
     *
     *  E0 = DCE does not echo characters during command state and online
     *       command state
     *  Q0 = DCE transmits result codes
     *  V1 = Display verbose result codes
     */
    err = at_send_command("ATE0Q0V1", NULL);
    if (err < 0)
        goto error;

    /* Enable +CME ERROR: <err> result code and use numeric <err> values. */
    err = at_send_command("AT+CMEE=1", NULL);
    if (err < 0)
        goto error;

    if (isVoiceEnabled()) {
        /* clear modem recover count to prevent modem fall into fail safe image*/
        asprintf(&cmd, "AT!UNLOCK=\"A710\"");
        err = at_send_command(cmd, &atresponse);
        free(cmd);
        if (err < 0 || atresponse->success == 0)
            LOGE("%s error by AT!UNLOCK command",__func__);
        at_response_free(atresponse);

        err = at_send_command("AT!BCRECOVERTEST=0", NULL);
        if (err < 0)
            LOGE("%s error to clear recover test count",__func__);
    }

    /* For 9x15 series module, ^MODE indication may corrupt normal AT response
     * Disable it here */
    at_send_command("AT^MODE=0", NULL);

    return 0;
error:
    return 1;
}

/**
 * Initialize everything that can be configured while we're still in
 * AT+CFUN=0.
 *
 * Note that most of the AT unsolicited result code reporting is only turned
 * on once the SIM is ready, in onSIMReady().
 */
static char initializeChannel(void)
{
    int err;

    LOGI("initializeChannel()");

    /* The radio should initially be off.
     * Simulate a RIL RadioPower off command
     */
    setRadioState(RADIO_STATE_OFF);

    /* Set phone functionality.
     *    0 = minimum functionality.
     */
    err = at_send_command("AT+CFUN=0", NULL);
    if (err < 0)
        goto error;

    /* If the AT+CFUN=0 does not work for some reason, but doesn't fail, and
     * the radio is still on, then start the power on process.
     *
     * Assume radio is off if isRadioOn() returns an error.
     */
    if (isRadioOn() > 0) {
        setRadioState(RADIO_STATE_SIM_NOT_READY);
    }

    return 0;

error:
    return 1;
}

/**
 * Initialize everything that can be configured while we're still in
 * AT+CFUN=0.
 *
 * Since there is no priority AT channel, there is nothing to initialize
 * here, but keep the function as a placeholder for now.
 */
static char initializePrioChannel()
{
    LOGI("initializePrioChannel()");
    return 0;
}

/**
 * Called by atchannel when an unsolicited line appears.
 * This is called on atchannel's reader thread. AT commands may
 * not be issued here.
 */
static void onUnsolicited(const char *s, const char *sms_pdu)
{
    /* Do nothing. This is a stub function */
}

static void signalCloseQueues(void)
{
    unsigned int i;
    for (i = 0; i < (sizeof(s_requestQueues) / sizeof(RequestQueue *)); i++) {
        RequestQueue *q = s_requestQueues[i];
        pthread_mutex_lock(&q->queueMutex);
        q->closed = 1;
        pthread_cond_signal(&q->cond);
        pthread_mutex_unlock(&q->queueMutex);
    }
}

/**
 *
 * Deregistration function, called when the AT handler package 
 * encounters a problem. This function used to be responsible 
 * for device detection, but for QMI devices, that responsibility
 * has been handed over to the QMI callback: "SetDeviceStateChangeCbk()"
 *
 * The reader loop (reader thread) portion of the AT package can 
 * still close down the interface if it detects a other type of 
 * error condition. Refer to "readline()" in at_channel.c for cases
 * where a NULL pointer is returned for details of specific error
 * conditions which can cause this callback to be invoked.
 *
 * @param 
 *    none
 * @return
 *    none
 * @note
 *    none
 */
static void onATReaderClosed(void)
{
    LOGI("AT channel closed\n");

    /* We don't know what's happened with the device, so clear all the data */
    swiril_cache_clearalldata();

    /* Don't inform the upper layers yet that the AT port has closed, until we
       get a better idea of why this happened.  However, do any other cleanup
       that should be done.
     */
    signalCloseQueues();
}

/**
 * 
 * Function registered to be called when device is no longer 
 * detected. The QMI callback - SetDeviceStateChangeCbk() - 
 * invokes this entry whenever the device's status, present or
 * disconnected, changes. 
 * 
 * @param [in] device_state
 *    indicates the device's current state: 
 *        DEVICE_STATE_DISCONNECTED, or
 *        DEVICE_STATE_READY
 * @return
 *    none
 * @note
 *    none
 *
 */
static void onDeviceStateChg( eDevState device_state )
{
    if( device_state == DEVICE_STATE_DISCONNECTED ) {
        LOGI("%s: Device has deregistered\n", __func__);

        /* The device has gone away so clear all the data */
        swiril_cache_clearalldata();

        /*
         * Inform the queueRunner queues that they are now closed and need
         * to pass control back to the outer loop
         */
        signalCloseQueues();
     }
     else {
        LOGI("%s: Device is ready\n", __func__);
     }
}

/* Called on command thread. */
static void onATTimeout()
{
    LOGI("AT channel timeout; restarting..\n");
    /* Last resort, throw escape on the line, close the channel
       and hope for the best. */
    at_send_escape();

    setRadioState(RADIO_STATE_UNAVAILABLE);
    signalCloseQueues();

    /* Hopefully QMI interface is still working, so try resetting modem. */
    LOGI("Try resetting the modem..\n");
    setPowerOff();
}

static void chkModemResponsiveness(void *params)
{
    ULONG nRet;
    CHAR modelType[100];
    nRet = GetModelID(sizeof(modelType),(CHAR*)&modelType);
    if (nRet != eQCWWAN_ERR_NONE) {
        LOGE("Failed to get ModelID response, error=%lu!\n", nRet);
        s_mdmrsp_fail_retry++;
        if (s_mdmrsp_fail_retry == 1) {
            enqueueRILEvent(RIL_EVENT_QUEUE_PRIO, chkModemResponsiveness, NULL,
                            &TIMEVAL_MODEM_POLL_SHORT_INTERVAL);
        } else {            
            LOGE("Modem is unresponsive, Resetting.....", nRet);
            /* Reset the modem */            
            setRadioState(RADIO_STATE_UNAVAILABLE);
            signalCloseQueues();
            resetModem();
            s_mdmrsp_fail_retry = 0;
        }        
    }
    else {
        s_mdmrsp_fail_retry = 0;
        enqueueRILEvent(RIL_EVENT_QUEUE_PRIO, chkModemResponsiveness, NULL,
                            &TIMEVAL_MODEM_POLL_LONG_INTERVAL);
    }
}

/* Start polling for EM7305 modem to check its responsiveness */
static void startPollingModemRspAbility()
{
    enqueueRILEvent(RIL_EVENT_QUEUE_PRIO, chkModemResponsiveness, NULL, 
                        &TIMEVAL_MODEM_POLL_SHORT_INTERVAL);
}

static void usage(char *s)
{
    fprintf(stderr, "usage: %s [-z] [-p <tcp port>] [-d /dev/tty_device] [-x /dev/tty_device] [-i <network interface>] [-o <atport open delay ms>]\n", s);
    exit(-1);
}

/** 
 * RIL request queue arguments
 */
struct queueArgs {
    int port;                   /**< port number */
    char * loophost;            /**< local host IP address */
    const char *device_path;    /**< device path */
    char isPrio;                /**< specifies which queue will be used */
    char hasPrio;               /**< has priority channel */
    char isdevicesocket;        /**< device socket flag */
    char isautodetect;          /**< auto detect modem AT port flag */
    int  atopendelay;           /**< Delay before using AT port after opening */
};

/**
 *
 * Initialize the file descriptor and execute the requests from queue
 *
 * @param[in] param 
 *     Pointer to the queueArgs structure
 *
 * @return
 *     Pointer to a void type
 *
 * @par Abort:<br> 
 *     none
 *
 * @note
 *     none
 */
static void *queueRunner(void *param)
{
    int gpsfd, fd;
    int ret;
    int checklength;
    struct queueArgs *queueArgs = (struct queueArgs *) param;
    struct RequestQueue *q = NULL;
    char * tty_name = NULL;
    char * tty_save_name = NULL;
    char *prop = NULL;
    char propValue[PROPERTY_VALUE_MAX];
    bool bForceFlag = false;
    CHAR modelType[100];
    
    LOGI("queueRunner starting!");

    for (;;) {
        /* QMI port check */
        if (qmiGetConnectedDeviceID() < 0) {
            /* disconnect and re-connect again */
            LOGI("QMI port reconnecting\n");
            qmiDeviceDisconnect();
            sleep(2);
            qmiDeviceConnect();
        }
        fd = -1;
        while (fd < 0) {
            if (queueArgs->port > 0) {
                if (queueArgs->loophost) {
                    fd = socket_network_client(queueArgs->loophost, queueArgs->port, SOCK_STREAM);
                } else {
                    fd = socket_loopback_client(queueArgs->port, SOCK_STREAM);
                }

            } else if (queueArgs->isdevicesocket) {
                if (!strcmp(queueArgs->device_path, "/dev/socket/qemud")) {
                    /* Qemu-specific control socket */
                    fd = socket_local_client( "qemud",
                                              ANDROID_SOCKET_NAMESPACE_RESERVED,
                                              SOCK_STREAM );
                    if (fd >= 0 ) {
                        char  answer[2];

                        if ( write(fd, "gsm", 3) != 3 ||
                             read(fd, answer, 2) != 2 ||
                             memcmp(answer, "OK", 2) != 0)
                        {
                            close(fd);
                            fd = -1;
                        }
                    }
                }
                else
                    fd = socket_local_client( queueArgs->device_path,
                                              ANDROID_SOCKET_NAMESPACE_FILESYSTEM,
                                              SOCK_STREAM );

            } else if ((queueArgs->device_path != NULL) || (queueArgs->isautodetect != 0)) {
                if (queueArgs->isautodetect != 0) {
                    /* use modem scan for AT command port */
                    tty_name = swims_ossdkgetatifname();
                    if (tty_name != NULL) {
                        tty_name = strdup(tty_name);
                        tty_save_name = strdup(tty_name);
                        /* open the port */
                        if (tty_name != NULL) {
                            fd = open(tty_name, O_RDWR);
                        }
                    }
                }
                else {
                    if (queueArgs->device_path != NULL) {
                        fd = open(queueArgs->device_path, O_RDWR);
                        /* Make a copy of the name we used 
                         * to open the AT port
                         */
                        tty_save_name = strdup( queueArgs->device_path );
                    }
                }

                /* If the port was successfully opened and the 
                 * path to the device was either a serial port
                 * or a USB serial device that was detected by
                 * the SWIMS package...
                 */
                if ( fd >= 0 && 
                     ((queueArgs->device_path != NULL && 
                     !memcmp(queueArgs->device_path, "/dev/ttyS", 9)) ||
                     (tty_name != NULL && 
                     strstr(tty_name, "/dev/ttyUSB") != NULL))) {

                    LOGI("%s: AT port detected on %s and validated", 
                         __func__, tty_save_name);
                    
                    /* Add port information to Android properties */
                    property_set("ril.sierra.at", swims_ossdkgetatifname());
                    property_set("ril.sierra.dm", swims_ossdkgetdmifname());
                    property_set("ril.sierra.nmea", swims_ossdkgetgpsifname());
                    
                    /* We now know the port is available and 
                     * can be opened. Close it for now and 
                     * save a copy of the port name for later
                     * use
                     */
                    close( fd );
                }
            }

            /* If the AT Cmd port couldn't be opened */
            if (fd < 0) {
                if (queueArgs->isautodetect != 0) {
                    if (tty_name != NULL) {
                        LOGE("FAILED to open AT channel %s (%s), retrying in 2 sec.", 
                            tty_name, strerror(errno));
                    }
                    else {
                        LOGE("FAILED to auto-detect AT channel, retrying in 2 sec.");
                    }

                    /* wait 2 seconds for modem scan */
                    sleep(2);
                }
                else {
                    if (queueArgs->device_path != NULL) {
                        LOGE("FAILED to open AT channel %s (%s), retrying in 10 sec.", 
                            queueArgs->device_path, strerror(errno));
                    }
                    else {
                        LOGE("FAILED to open AT channel, retrying in 10 sec.");
                    }
                    /* wait 10 seconds when modem scan is not enabled */
                    sleep(10);
                }
            }

            if (tty_name != NULL) {
                free(tty_name);
                tty_name = NULL;
            }
        }

        /* Before opening the AT channel, make sure all the cache values
           are cleared.
         */
        swiril_cache_clearalldata();

        /* reset s_pendingSIMReady to false */
        ResetPendingSIMReady();

        /* reset s_ratCDMA to RIL_RAT_UNKNOWN*/
        setRatCDMA(RIL_RAT_UNKNOWN);

        /* Start the AT Command reader thread */
        ret = at_open(tty_save_name, onUnsolicited, 
                      queueArgs->atopendelay, AT_PORT_CLOSE_TIMEOUT);

        if (ret < 0) {
            LOGE("AT error %d on at_open\n", ret);
            at_close();
            continue;
        }

        at_set_on_reader_closed(onATReaderClosed);
        at_set_on_timeout(onATTimeout);
        
        q = &s_requestQueue;

        if(initializeCommon()) {
            LOGE("FAILED to initialize channel!");
            at_close();
            continue;
        }

        /* Install a pointer to the device state change callback */
        SetDeviceStateChangeCbk( onDeviceStateChg );

        if (queueArgs->isPrio == 0) {
            q->closed = 0;
            if (getRunningTechnology() == SWI_RUNNING_TECH_CDMA)  {
                initializeChannelCDMA();
            }
            else {
                initializeChannelUMTS();
            }

            /* Stop the DHCP client if already running */
            checkDHCPnStopService();

            at_make_default_channel();
        } else {
            q = &s_requestQueuePrio;
            q->closed = 0;
            at_set_timeout_msec(1000 * 30); 
        }

        /* If there is no priority queue, then do the priority channel init on
         * the regular channel, otherwise only do it for the priority channel.
         */
        if (queueArgs->hasPrio == 0 || queueArgs->isPrio)
            if (initializePrioChannel()) {
                LOGE("FAILED to initialize channel!");
                at_close();
                continue;
            }

        /* Check if GPS enabled properly on SL9090 */
        GetModelID(sizeof(modelType),(CHAR*)&modelType);
        if (strstr(modelType,"9090") != NULL)
        {
            ATResponse *atresponse = NULL;
            int err = 0;
            char *cmd = NULL;
            char *line = NULL;
            char trimResult[64];
            bool bResetNV452 = false;
            int i = 0;

            asprintf(&cmd, "AT!UNLOCK=\"A710\"");
            err = at_send_command(cmd, &atresponse);
            free(cmd);
            cmd=NULL;
            if (err < 0 || atresponse->success == 0)
                LOGE("%s error by AT!UNLOCK command",__func__);
            at_response_free(atresponse);
            atresponse=NULL;

            asprintf(&cmd, "AT!NV?452");
            err = at_send_command_singleline(cmd,"", &atresponse);
            free(cmd);
            cmd=NULL;
            if (err < 0 || 
                atresponse->success == 0 || 
                atresponse->p_intermediates == NULL) {
                LOGE("%s error in reading NV452",__func__);
            }
            else {
                line = atresponse->p_intermediates->line;
                memset(trimResult, 0, sizeof(trimResult));
                while ((*line != '\0') && (i < sizeof(trimResult))) {
                    if ((*line >= '0') && (*line <= '9'))
                        trimResult[i++] = *line;
                    line++;
                }                    
                if (strcmp(trimResult,"00000000")) {                     
                     bResetNV452 = true;
                }
            }            
            at_response_free(atresponse);
            atresponse=NULL;
            if (bResetNV452) {
                asprintf(&cmd, "AT!CUSTOM=\"gpsenable\",1");
                err = at_send_command(cmd, &atresponse);
                free(cmd);
                cmd=NULL;
                if (err < 0 || atresponse->success == 0)
                    LOGE("%s error in gpsenable command",__func__);
                at_response_free(atresponse);
                atresponse=NULL;

                LOGI("Try resetting the modem..\n");
                setPowerOff();
                sleep(3);
            }
        }

        /* start polling to check that modem is responding correctly */
        startPollingModemRspAbility();           

        /** Get the value of force flag from android property */
        asprintf(&prop, "%s", "persist.radio.forceflag");
        checklength = property_get(prop, propValue,NULL);
        free(prop);
        prop = NULL;
        if(checklength) {
            bForceFlag = strcmp(propValue, RIL_VERSION_STRING);
        }
        if (bForceFlag) {
            ATResponse *atresponse = NULL;
            int err = 0;
            char *cmd = NULL;

            // unlock required for some commands
            asprintf(&cmd, "AT!UNLOCK=\"A710\"");
            err = at_send_command(cmd, &atresponse);
            free(cmd);
            cmd=NULL;
            if (err < 0 || atresponse->success == 0)
                LOGE("%s error by AT!UNLOCK command",__func__);
            at_response_free(atresponse);
            atresponse=NULL;
            

            #ifdef ECC_LIST_INIT
            /* Only applicable to voice enable modem */
            if (isVoiceEnabled()) 
            {
                // NVENUM: emergency call codes
                at_send_command("AT!NVENUM=0", NULL);
                if (PROPERTY_GET("persist.ril.ecc1", propValue)) { 
                    asprintf(&cmd, "AT!NVENUM=1,\"%s\"",propValue);
                    err = at_send_command(cmd, &atresponse);
                    free(cmd);
                    cmd=NULL;
                    if (err < 0 || atresponse->success == 0)
                        LOGE("%s error by AT!NVENUM command",__func__);
                    at_response_free(atresponse);
                    atresponse=NULL;
                }
                if (PROPERTY_GET("persist.ril.ecc2", propValue)) { 
                    asprintf(&cmd, "AT!NVENUM=1,\"%s\"",propValue);
                    err = at_send_command(cmd, &atresponse);
                    free(cmd);
                    cmd=NULL;
                    if (err < 0 || atresponse->success == 0)
                        LOGE("%s error by AT!NVENUM command",__func__);
                    at_response_free(atresponse);
                    atresponse=NULL;
                }
                if (PROPERTY_GET("persist.ril.ecc3", propValue)) { 
                    asprintf(&cmd, "AT!NVENUM=1,\"%s\"",propValue);
                    err = at_send_command(cmd, &atresponse);
                    free(cmd);
                    cmd=NULL;
                    if (err < 0 || atresponse->success == 0)
                        LOGE("%s error by AT!NVENUM command",__func__);
                    at_response_free(atresponse);
                    atresponse=NULL;
                }
            }
            #endif

            // ENSEN: Enable ENS functionality
            if (PROPERTY_GET("persist.radio.ensen", propValue)){ 
                asprintf(&cmd, "at!ensen=%s",propValue);
                err = at_send_command(cmd, &atresponse);
                free(cmd);
                cmd=NULL;
                at_response_free(atresponse);
                atresponse=NULL;
            }

            // GCFEN: Enable GCF test mode
            if (PROPERTY_GET("persist.radio.gcfen", propValue)){ 
                asprintf(&cmd, "at!gcfen=%s",propValue);
                err = at_send_command(cmd, &atresponse);
                free(cmd);
                cmd=NULL;
                at_response_free(atresponse);
                atresponse=NULL;
                // Set the property so that framework/App can access it
                if (property_set("persist.radio.gcfen", propValue) < 0)
                    LOGE("FAILED to set persist.radio.gcfen!");
            }
            // CBEN: Enable Cell Broadcast SMS
            if (PROPERTY_GET("persist.radio.cben", propValue)){
                asprintf(&cmd, "at!nv=1016,%s",propValue);
                err = at_send_command(cmd, &atresponse);
                asprintf(&cmd, "at!nv=1017,%s",propValue);
                err = at_send_command(cmd, &atresponse);
                free(cmd);
                cmd=NULL;
                at_response_free(atresponse);
                atresponse=NULL;
            }
        }

        #ifdef AUDIO_PROVISIONING
        if (isVoiceEnabled()) {
            /** Update audio profile parameters if modified */
            updateAudioProfile(bForceFlag);
        }
        #endif
        

#if 1
        {
            enum eGobiImageCarrier image;
            /*10 for HFA, FUMO */
            struct timeval timeval_omadm_delay = { 10, 0 };
            if (getFirmwareInfoCarrierQMI(&image) && (image == eGOBI_IMG_CAR_SPRINT)) {

                enqueueRILEvent(RIL_EVENT_QUEUE_PRIO, registerSetOMADMStateCallbackCDMA, NULL,
                            &timeval_omadm_delay);
            }
        }
#endif

        LOGE("Looping the requestQueue!");
        for (;;) {
            RILRequest *r;
            RILEvent *e;
            struct timespec ts;

            memset(&ts, 0, sizeof(ts));

            pthread_mutex_lock(&q->queueMutex);

            if (q->closed != 0) {
                LOGW("Device error, attempting to recover..");
                pthread_mutex_unlock(&q->queueMutex);
                break;
            }

            while (q->closed == 0 && q->requestList == NULL && 
                   q->eventList == NULL) {
                pthread_cond_wait(&q->cond,
                                  &q->queueMutex);

            }

            /* eventList is prioritized, smallest abstime first. */
            if (q->closed == 0 && q->requestList == NULL && q->eventList) {
                int err = 0;
                err = pthread_cond_timedwait(&q->cond, 
                                             &q->queueMutex, 
                                             &q->eventList->abstime);
                if (err && err != ETIMEDOUT)
                    LOGE("timedwait returned unexpected error: %s",
                         strerror(err));
            }

            if (q->closed != 0) {
                pthread_mutex_unlock(&q->queueMutex);
                continue; /* Catch the closed bit at the top of the loop. */
            }

            e = NULL;
            r = NULL;

            clock_gettime(CLOCK_MONOTONIC, &ts);

            if (q->eventList != NULL &&
                timespec_cmp(q->eventList->abstime, ts, < )) {
                e = q->eventList;
                q->eventList = e->next;
            }

            if (q->requestList != NULL) {
                r = q->requestList;
                q->requestList = r->next;
            }

            pthread_mutex_unlock(&q->queueMutex);

            if (e) {
                e->eventCallback(e->param);
                free(e);
            }

            if (r) {
               processRequest(r->request, r->data, r->datalen, r->token);
               freeRequestData(r->request, r->data, r->datalen);
               free(r);
            }
        }

        at_close();
    
        /* Free the tty_save_name now */
        if (tty_save_name != NULL) {
            free(tty_save_name);
        }

        /* Inform the application that the modem has gone away */
        setRadioState(RADIO_STATE_UNAVAILABLE);

        /* Wait a bit to give the resetting AT port a chance to come back */
        sleep(2);
        /* clear Device Node and Key*/
        clearDeviceNodeAndKey();
        
        LOGI("Re-opening after close");
    }
    return NULL;
}

pthread_t s_tid_queueRunner;
pthread_t s_tid_queueRunnerPrio;

void dummyFunction(void *args)
{
    LOGE("dummyFunction: %p", args);
}

/**
 *
 * Initialize the RIL command handler function pointer
 *
 * @return
 *     None
 *
 * @par Abort:<br> 
 *     none
 *
 * @note
 *     none
 */
void initRILFunctionHandler(void)
{
    int i;

    /* initialize all handler pointer to NULL */
    for (i=0; i<RIL_SOL_NUMBER_MAX; i++) {
        s_rilsolreqhandler[i] = NULL;
        s_rilsolreqhandlercdma[i] = NULL;
    }
    
    /* only support QMI API handlers for now */
    initRILFunctionHandlerQMI (&s_rilsolreqhandler[0],
                               &s_rilsolreqhandlercdma[0]);
}

/**
 *
 * Install a signal handler for one signal
 *
 * @param[in] signo
 *          signal number
 * @param[in] functionp
 *          signal handler function of type
 *          void (*functionp)(int, siginfo_t *, void *)
 *
 * @return
 *     none
 *
 * @note
 *     none
 */
void RIL_SignalInstall( unsigned int signo,
                        void (*functionp)(int, siginfo_t *, void *) )
{
    struct sigaction sa;

    if (functionp == NULL)
        LOGI("signal handler install failed: NULL signal handler");

    sa.sa_sigaction = functionp;
    sigemptyset(&sa.sa_mask);

    sa.sa_flags = SA_NODEFER | SA_SIGINFO;

    if (sigaction(signo, &sa, NULL) < 0)
        LOGI("signal handler install failed %d", errno);
}

/**
 *
 * Signal handler for SIGTERM
 *
 * @param[in] signo
 *          signal number
 * @param[in] siginfop
 *          pointer to signal info
 * @param[in] contextp
 *          pointer to interrupted user context
 *
 * @return
 *     none
 *
 * @note
 *     none
 */
void RIL_SIGTERMhandler(int signo, siginfo_t *siginfop, void *contextp)
{
    /* Disconnect from SLQS SDK */
    qmiDeviceDisconnect();

    /* simple exit */
    exit(EXIT_SUCCESS);
}

/**
 * Callback for restarting the SDK process
 *
 * @param[in] param
 *     void pointer passed into the callback. In this case NULL.
 *
 * @return
 *     none
 *
 * @note
 *     none
 */
void restartSDK(void *param)
{
    qmiSLQSRestart();

    /**
     * Get the SMS list from the device once the UIM/NV is ready; if UIM/NV not
     * ready now, the SMS list would be fetched on receiving either of
     * RADIO_STATE_SIM_READY, RADIO_STATE_RUIM_READY or RADIO_STATE_NV_READY in
     * setRadioState()
     */
    if ((sState == RADIO_STATE_SIM_READY) ||
        (sState == RADIO_STATE_RUIM_READY) ||
        (sState == RADIO_STATE_NV_READY)) {
        getSMSList(NULL);
    }
}

/**
 * Signal handler for SIGCHLD
 *
 * @param[in] signo
 *     signal number
 * @param[in] siginfop
 *     pointer to signal info
 * @param[in] contextp
 *     pointer to interrupted user context
 *
 * @return
 *     none
 *
 * @note
 *     none
 */
void RIL_SIGCHLDhandler(int signo, siginfo_t *siginfop, void *contextp)
{
    int status;
    pid_t pid;

    /* Allow the SDK process to terminate, only if there is a pid match */
    swi_osapigetprocessID( &pid );
    if( pid == siginfop->si_pid ) {
        wait(&status);
        enqueueRILEvent(RIL_EVENT_QUEUE_PRIO, restartSDK, NULL, NULL);
    }
}

/**
 * Install a signal handler for software termination signal from kill
 *
 * @param
 *     none
 *
 * @return
 *     none
 *
 * @note
 *     none
 */
void RIL_SignalInit()
{
    LOGI( "RIL_Signalinit\n" );
    RIL_SignalInstall(SIGTERM, RIL_SIGTERMhandler);
    RIL_SignalInstall(SIGCHLD, RIL_SIGCHLDhandler);
}

/**
 *
 * Initialize the RIL functions
 *
 * @param[in] env 
 *     Pointer to the RIL environment 
 * @param argc 
 *     argument count
 * @param[in] argv 
 *     Pointer to the argument strings 
 *
 * @return
 *     pointer to the RIL radio functions
 *
 * @par Abort:<br> 
 *     none
 *
 * @note
 *     none
 */
const RIL_RadioFunctions *RIL_Init(const struct RIL_Env *env, int argc,
                                   char **argv)
{
    int ret;
    int opt;
    int port = -1;
    char *loophost = NULL;
    const char *device_path = NULL;
    const char *priodevice_path = NULL;
    const char *handler_method = NULL;
    int opendelay=AT_OPEN_DELAY_MS;
    char isdevicesocket = 0;
    struct queueArgs *queueArgs;
    struct queueArgs *prioQueueArgs;
    pthread_attr_t attr;
    char autodetect = 0;

    s_rilenv = env;

    LOGI("Entering RIL_Init..");

    while (-1 != (opt = getopt(argc, argv, "z:i:p:o:d:s:x:c:a"))) {
        switch (opt) {
            case 'z':
                loophost = optarg;
                LOGI("Using loopback host %s..", loophost);
                break;

            case 'i':
                ril_iface = optarg;
                LOGI("Using network interface %s as primary data channel.",
                     ril_iface);
                break;

            case 'p':
                port = atoi(optarg);
                if (port == 0) {
                    usage(argv[0]);
                    return NULL;
                }
                LOGI("Opening loopback port %d\n", port);
                break;

            case 'o':
                opendelay = atoi(optarg);
                if( opendelay > 750 ) {
                    opendelay = 750;
                }
                else if( opendelay < 0 ) {
                   opendelay = 0;
                }
                LOGI("AT port opening delay=%d\n", opendelay);
                break;

            case 'd':
                device_path = optarg;
                LOGI("Opening tty device %s\n", device_path);
                break;

            case 'a':
                /* '-a' specified for automatic AT port scan */
                autodetect = 1;
                /* initialize and start modem scan */
                swims_ossdkscaninit();
                LOGI("Opening tty device automatically\n");
                break;

            case 'x':
                priodevice_path = optarg;
                LOGI("Opening priority tty device %s\n", priodevice_path);
                break;

            case 's':
                device_path = optarg;
                isdevicesocket=1;
                LOGI("Opening socket %s\n", device_path);
                break;
                
            case 'c':
                LOGI("Client id received %s\n", optarg);
                break;
                
            default:
                LOGE("%c is not a valid calling argument", optopt );
                usage(argv[0]);
                return NULL;
        }
    }

    if (ril_iface == NULL) {
        LOGI("Network interface was not supplied, falling back on usb0!");
        ril_iface = strdup("usb0\0");
    }

    if (port < 0 && device_path == NULL && autodetect == 0) {
        usage(argv[0]);
        return NULL;
    }

    /* Initialize the RIL deamon signal handling */
    RIL_SignalInit();

    /* Initialize the RIL command handler function pointer */
    initRILFunctionHandler();
    /* connect sdk */
    qmiDeviceConnect();

    queueArgs = malloc(sizeof(struct queueArgs));
    memset(queueArgs, 0, sizeof(struct queueArgs));

    queueArgs->device_path = device_path;
    queueArgs->port = port;
    queueArgs->loophost = loophost;
    queueArgs->isdevicesocket = isdevicesocket;
    queueArgs->atopendelay = opendelay;
    if (autodetect != 0) {
        /* set flag of modem scan */
        queueArgs->isautodetect = 1;
    }
    else {
        queueArgs->isautodetect = 0;
    }
    
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    if (priodevice_path != NULL) {
        prioQueueArgs = malloc(sizeof(struct queueArgs));
        memset(prioQueueArgs, 0, sizeof(struct queueArgs));
        prioQueueArgs->device_path = priodevice_path;
        prioQueueArgs->isPrio = 1;
        prioQueueArgs->hasPrio = 1;
        prioQueueArgs->atopendelay = opendelay;
        queueArgs->hasPrio = 1;

        s_requestQueuePrio.enabled = 1;

        pthread_create(&s_tid_queueRunnerPrio, &attr, queueRunner, prioQueueArgs);
    }

    pthread_create(&s_tid_queueRunner, &attr, queueRunner, queueArgs);

    /* Create the swiril_gps thread */
    RILGPS_Init(GPS_RILTYPE_QMI);

    return &s_callbacks;
}

#ifdef ECC_LIST_INIT

/**
 * Emergency Call Number Read from Modem NV
 *
 * Read ModemNV for ECC List 
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 *
 */ 
int readFromMODEM_NV(void)
{     
    ATResponse *atresponse = NULL;
    ATLine *atlinep = NULL;
    int err = 0;
    char* line;
   
    /* Read ECC List from  Modem NV*/ 
    err = at_send_command_multivalue("AT!NVENUM?","!NVENUM:", &atresponse);
    atlinep=atresponse->p_intermediates;
    if (err < 0 || atresponse->success == 0) {
        at_response_free(atresponse);        
        LOGE("ERROR in Sending Command -> AT!NVENUM?");
        return -1;
    }
    else {
        /** Read next line in response */
        line=atlinep->line;
        while(true) {
            atlinep=atlinep->p_next;
            if (atlinep == NULL) {
                break;
            }
            line=atlinep->line; 
            if (line!=NULL) {
                listdecodeptr = NULL;  
                /** Search for quotes to isolate the number */
                listdecodeptr = strchr(line,'"');
                if (listdecodeptr!=NULL) {
                    listdecodeptr++;
                    if(numdigits !=0)
                        extractedNums[numdigits++] = 0x2C;   
                    /** Search for quotes to isolate the number */
                    while (*listdecodeptr!='"') {
                        extractedNums[numdigits++] = *listdecodeptr++;
                        if(numdigits > ECC_LIST_BUFFER_SIZE) {
                           return -1;
                        }
                    }
                }
                else {
                    return -1;
                }
            }
            else {
                return -1;
            }
        }
      
    }
   return 0;
}



/**
 * Emergency Call Number Read from SIMCARD
 *
 * Read SIMCard for ECC List 
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 *
 */ 
int readFromSIMCARD(void)
{
    ATResponse *atresponse = NULL;
    ATLine *atlinep = NULL;
    char *cmd;
    int err = 0;
    char* line;
    int eccCountVal = 0;
    int eccCount; 
    int countinterchange = 0;
    int counttemp = 0; 
    int tempchar = 0;

    LOGD("%s : Entered",__func__);
    
    eccCountVal=ECC_LIST_SIM_READ_START_PTR;
    eccCount = ECC_LIST_SIM_READ_START_MAX_LIST;
      
    while(true) {
        /** Start AT Command Buffer */
        if (getRunningTechnology() == SWI_RUNNING_TECH_CDMA) {
            asprintf(&cmd,"AT+CRSM=176,28487,0,0,%d",eccCount);
        }
        else {
            asprintf(&cmd,"AT+CRSM=176,28599,0,0,%d",eccCount);
        }   
        eccCount-=3;       
        /* Read SIM Card ECC List*/ 
        err = at_send_command_multivalue(cmd,"+CRSM:", &atresponse);
        free(cmd);
        /** Read the next inlist */
        atlinep=atresponse->p_intermediates;
        /** Check for error response */
        if (err < 0 || atresponse->success == 0) {
            at_response_free(atresponse);        
            LOGE("ERROR in response of command -> AT+CRSM");
            return -1;
        }
	else {
            line=atlinep->line;
            /** Check for response not to be null */
            if (line !=NULL) {
                if((strstr(line, "144") != NULL) || (strstr(line,"108") != NULL)) {
                    LOGD("Response for -> AT+CRSM ==> %s",line);
                    /** Search for quotes to isolate the numbers */
                    listdecodeptr = strchr(line,'"');
                    if (listdecodeptr!=NULL) {
                        listdecodeptr++;
                        eccCountVal = 0;
                        countinterchange =0;
                        while (*listdecodeptr!='"') {
                            if(*listdecodeptr != ECC_LIST_READ_SIM_DUMMY) {
                                extractedNums[numdigits++] = *listdecodeptr++;
                                countinterchange++;
                                if(countinterchange > ECC_LIST_BUFFER_SIZE) {
                                   return -1;
                                }
                            }
                            else
                               listdecodeptr++;
                            eccCountVal++;
                            if(eccCountVal == 6 && numdigits > 0) {
                                extractedNums[numdigits++] = ECC_LIST_ADD_COMMA;
                                counttemp = (numdigits - countinterchange -1);
                                /** Decode the ECC List from SIM */
                                while(true) {
                                    tempchar = extractedNums[counttemp];
                                    if(extractedNums[counttemp+1] != ECC_LIST_ADD_COMMA && 
                                    extractedNums[counttemp] != ECC_LIST_ADD_COMMA) {
                                        extractedNums[counttemp] = extractedNums[counttemp+1]; 
                                        counttemp++;
                                        extractedNums[counttemp] =  tempchar;
                                        counttemp++;
                                    }
                                    else {
                                        countinterchange=0;
                                        counttemp =0;
                                        break;
                                    }
                                }
                                eccCountVal = 0;
                            }
                        }
                    }
                    /** Exit in case of no numbers */
                    at_response_free(atresponse);
                    if(numdigits > 0) {
                        numdigits--;
                        extractedNums[numdigits] = 0x00;
                    }
                    break;
                }
                else if (strstr(line, "103") != NULL) {
                    // Donothing, keep continuing the loop
                }
                else  {
                    at_response_free(atresponse);
                    return -1;
                }
            }
            else {
                at_response_free(atresponse); 
                return -1;
            }
        }
        at_response_free(atresponse);
        if(eccCount == 0)
            break;
    }
    LOGD("Emergency Numbers from SIM are %s",extractedNums);
    return 0;
}

int readFromSIMCARDRecord(void)
{
    ATResponse *atresponse = NULL;
    char *cmd;
    ATLine *atlinep = NULL;
    int err = 0;
    char* line;
    int eccCount;
    int countinterchange = 0;

    LOGD("%s : Entered",__func__);
    eccCount = 1;
    while(true)
    {
        asprintf(&cmd,"AT+CRSM=178,28599,%d,4,4",eccCount);
        eccCount+=1;
        /* Read SIM Card ECC List*/
        err = at_send_command_multivalue(cmd,"+CRSM:", &atresponse);
        free(cmd);
        /* Read the next inlist */
        atlinep=atresponse->p_intermediates;
        /* Check for error response */
        if (err < 0 || atresponse->success == 0)
        {
            at_response_free(atresponse);
            LOGE("ERROR in response of command -> AT+CRSM");
            return -1;
        }
        else
        {
            line=atlinep->line;
            /* Check for response not to be null */
            if (line !=NULL)
            {
                if((strstr(line, "144") != NULL) || (strstr(line,"108") != NULL))
                {
                    LOGD("Response for -> AT+CRSM ==> %s",line);
                    /* Search for quotes to isolate the numbers */
                    listdecodeptr = strchr(line,'"');
                    if (listdecodeptr!=NULL)
                    {
                        listdecodeptr++;
                        countinterchange = 0;
                        while (*listdecodeptr!='"')
                        {
                            /* process 2 bytes each time */
                            if((*listdecodeptr == 'F') && (*(listdecodeptr+1) == 'F'))
                            {
                                if(countinterchange)
                                    extractedNums[numdigits++] = ECC_LIST_ADD_COMMA;
                                /* finished one record */
                                break;
                            }
                            else
                            {
                                /* low byte always valid */
                                extractedNums[numdigits++] = *(listdecodeptr+1);
                                countinterchange++;

                                /* high byte */
                                if(*listdecodeptr != 'F')
                                {
                                    extractedNums[numdigits++] = *listdecodeptr;
                                    countinterchange++;
                                }
                                listdecodeptr+=2;
                                if(countinterchange > ECC_LIST_BUFFER_SIZE) {
                                    at_response_free(atresponse);
                                   return -1;
                                }
                            }
                        }  /* end while of reading one record */
                    }
                      
                }
            }
            else {
                at_response_free(atresponse);
                return -1;
            }
        }
        at_response_free(atresponse);
        if(eccCount == 5)
            break;
    } /* end while of reading 5 records */
    LOGD("Emergency Numbers from SIM REC are %s",extractedNums);
    return 0;
}

static int readSIMECCRecordsQMI(unsigned short fileSize, unsigned short recordSize)
{
    unsigned short i;
    unsigned short numRecords;
    unsigned long nRet;
    RIL_SIM_IO ioarg;
    RIL_SIM_IO_Response sr;
    int countinterchange;

    LOGD("%s: fileSize %d, recordSize %d\n",__func__,fileSize, recordSize);

    memset(&ioarg,0,sizeof(RIL_SIM_IO));
    memset(&sr,0,sizeof(RIL_SIM_IO_Response));

    /* Setting up parameters to get SIM ECC file characteristics */
    ioarg.path = "3F007F20";
    ioarg.fileid = 0x6FB7;
    ioarg.p3 = (int)recordSize;
    numRecords = fileSize/recordSize;
    for (i = 1; i <= numRecords; i++) {
        ioarg.p1 = (int) i; 
        nRet = readRecordQMI(&ioarg, &sr);
        LOGD("%s: readRecordQMI returned = %lu, sr.sw1 = %d \n",__func__,nRet,sr.sw1); 
        if ((nRet == eQCWWAN_ERR_NONE) && (sr.sw1 == 0x90)) {
            listdecodeptr = sr.simResponse;
            if (sr.simResponse == NULL )
                return -1;
            if (sr.simResponse == '\0' ) {
                free(sr.simResponse);
                return -1;
            }

            LOGD("%s: simResponse %s \n",__func__,sr.simResponse); 
            countinterchange = 0;
            while (listdecodeptr != '\0') {
                /* process 2 bytes each time */
                if((*listdecodeptr == 'F') && (*(listdecodeptr+1) == 'F')) {
                    if(countinterchange)
                        extractedNums[numdigits++] = ECC_LIST_ADD_COMMA;
                     /* finished one record */
                     break;
                     }
                else {
                     /* low byte always valid */
                     extractedNums[numdigits++] = *(listdecodeptr+1);
                     countinterchange++;
                     
                     /* high byte */
                     if(*listdecodeptr != 'F') {
                         extractedNums[numdigits++] = *listdecodeptr;
                         countinterchange++;
                     }
                     listdecodeptr += 2;
                }    
            }
            if (sr.simResponse)
                free(sr.simResponse);
        }
        else
            return -1;
    }
    LOGD("%s :Emergency Numbers from SIM RECORD are %s",__func__,extractedNums);
    return 0;
}   

static int readSIMECCBinaryQMI(unsigned short fileSize)
{
    unsigned short i;
    unsigned short fileDataLen;
    unsigned short numEcc;
    unsigned long nRet;
    RIL_SIM_IO ioarg;
    RIL_SIM_IO_Response sr;
    char *line;
    char *startIndex;
    int countinterchange;
    int byteTraversed;

    LOGD("%s: fileSize %d\n",__func__,fileSize);

    memset(&ioarg,0,sizeof(RIL_SIM_IO));
    memset(&sr,0,sizeof(RIL_SIM_IO_Response));

    /* Setting up parameters to get SIM ECC file characteristics */
    if (getRunningTechnology() == SWI_RUNNING_TECH_CDMA) {
        ioarg.path = "3F007F25";
        ioarg.fileid = 0x6F47;
    }
    else {
        ioarg.path = "3F007F20";
        ioarg.fileid = 0x6FB7;
    }    
    ioarg.p3 = fileSize;
    
    nRet = readBinaryQMI(&ioarg, &sr);
    LOGD("%s: readBinaryQMI returned = %lu, sr.sw1 = %d \n",__func__,nRet,sr.sw1); 
    if ((nRet == eQCWWAN_ERR_NONE) && (sr.sw1 == 0x90)) {
        listdecodeptr = sr.simResponse;
        if (sr.simResponse == NULL)
            return -1;
        if (sr.simResponse == '\0' ) {
            free(sr.simResponse);
            return -1;
        }

        LOGD("%s: simResponse %s \n",__func__,sr.simResponse);
        fileDataLen =  strlen(sr.simResponse);
        numEcc = fileDataLen/6;        
        while (numEcc) {
            startIndex = listdecodeptr;
            countinterchange = 0;
            byteTraversed = 0; 
            while (byteTraversed != 6) {
                /* process 2 bytes each time */
                if((*listdecodeptr == 'F') && (*(listdecodeptr+1) == 'F')) {
                    if(countinterchange)
                        extractedNums[numdigits++] = ECC_LIST_ADD_COMMA;
                    /* finished one record */
                    break;
                }
                else {
                    /* low byte always valid */
                    extractedNums[numdigits++] = *(listdecodeptr+1);
                    countinterchange++;
                     
                    /* high byte */
                    if(*listdecodeptr != 'F') {
                        extractedNums[numdigits++] = *listdecodeptr;
                        countinterchange++;
                    }
                    listdecodeptr += 2; 
                    byteTraversed += 2;
                }                    
            }
            listdecodeptr = startIndex + 6;
            numEcc--;
        }
        if (sr.simResponse)
            free(sr.simResponse);
    }
    else
        return -1;
    LOGD("%s :Emergency Numbers from SIM BINARY are %s",__func__,extractedNums);
    return 0;
}



/**
 * Emergency Call Number Read from SIMCARD
 *
 * Read SIMCard for ECC List 
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 *
 */ 
int readFromSIMCARDQMI(void)
{
    unsigned long nRet;
    int retValue = 0;
    unsigned short file_size = 0;
    unsigned char file_type = 0;
    unsigned short rec_size;
    unsigned short i;
    RIL_SIM_IO ioarg;
    RIL_SIM_IO_Response sr;

    LOGD("%s: Entered",__func__);

    memset(&ioarg,0,sizeof(RIL_SIM_IO));
    memset(&sr,0,sizeof(RIL_SIM_IO_Response));

    /* Setting up parameters to get SIM ECC file characteristics */
    if (getRunningTechnology() == SWI_RUNNING_TECH_CDMA) {
        ioarg.path = "3F007F25";
        ioarg.fileid = 0x6F47;
    }
    else {
        ioarg.path = "3F007F20";
        ioarg.fileid = 0x6FB7;
    }        
    
    /* Get the SIM ECC file characteristics */
    nRet = getFileAttributesQMI(&ioarg,&sr);
    LOGD("%s: getFileAttributesQMI returned = %lu, sr.sw1 = %d \n",__func__,nRet,sr.sw1); 
    if ((nRet == eQCWWAN_ERR_NONE) && (sr.sw1 == 0x90)) {    
        LOGD("%s: simResponse %s \n",__func__,sr.simResponse); 
        file_size = ((((char2nib(sr.simResponse[4]) << 4) | (char2nib(sr.simResponse[5]))) << 8) |
                      (((char2nib(sr.simResponse[6]) << 4) | (char2nib(sr.simResponse[7]))) & 0x00FF)) ;

        file_type = ((char2nib(sr.simResponse[26]) << 4) | (char2nib(sr.simResponse[27])));
        LOGD("%s: file_size %d, file_type %d\n",__func__,file_size, file_type);
        switch (file_type) {
            /* Transparent EF */
            case 0:
                retValue = readSIMECCBinaryQMI(file_size);
                break;
            /* Linear Fixed/cyclic */
            case 1:
            case 3:
                rec_size = (unsigned short)((char2nib(sr.simResponse[28]) << 4) | (char2nib(sr.simResponse[29])));
                retValue = readSIMECCRecordsQMI(file_size,rec_size);
                break;
            default:
                retValue = -1;
                break;
         }
         if (sr.simResponse)
             free(sr.simResponse);
    }
    else if (nRet != eQCWWAN_ERR_QMI_SIM_FILE_NOT_FOUND) {
        if (getRunningTechnology() == SWI_RUNNING_TECH_CDMA) {
            /* Read the emergency call from SIM Card always as binary */
            retValue = readFromSIMCARD();
            }
        else {
            bool b3GSIM = false;
            /* Figure out by GET RESPONSE command if SIM is 2G or 3G */
            retValue = readSimEccFileStatus(&b3GSIM);
            if (retValue == 0) {
                if (b3GSIM) {
                    retValue = readFromSIMCARDRecord();
                }
                else {
                    retValue = readFromSIMCARD();
                }
            }
            else {
                 LOGE("%s: Could not read from SIM card ECC attributes",__func__);
            }
        }
    }
    else
        return -1;
    LOGD("%s: Exiting with %d",__func__,retValue); 
    return retValue;
}
       
/**
 * Attenuate the Dupliacte Emergency Call Numbers from the list
 *
 * Clears the duplicate numbers in ECC List 
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 *
 */
void ecclistAttenuate(void)
{
    char *emerValue=NULL;
    char *listEGNumbers[32];
    char *elm=NULL;
    int nEGnum = 0;
    int i,j,index; 

    /* get the numbers one by one from list and populate in array of strings */
    LOGD("Emergency Numbers Before Attenuation are: %s",extractedNums);
 
    emerValue = malloc(strlen(extractedNums) + 1);
    strcpy(emerValue,extractedNums);   
    elm = strtok(emerValue, " ,"); 
    while ((elm != NULL) && (nEGnum < 32)) {
        listEGNumbers[nEGnum] = malloc(strlen(elm) + 1);
        LOGD("str: %s\n", elm);
        strcpy(listEGNumbers[nEGnum], elm);
        nEGnum++;
        elm = strtok(NULL," ,");
    }

    for (i =0; i < nEGnum; i++) {
         for (j=i+1; j < nEGnum; j++) {
            if (((listEGNumbers[i]) && (listEGNumbers[j])) && (!strcmp(listEGNumbers[i],listEGNumbers[j]))) {
                free(listEGNumbers[j]);
                listEGNumbers[j] = NULL;
            }
         }
    }
    memset(extractedNums,0,sizeof(extractedNums));
    index =0;
    for (i =0; i < nEGnum; i++) {
        if (listEGNumbers[i] != NULL) {
            memcpy(&extractedNums[index],listEGNumbers[i], strlen(listEGNumbers[i]));
            index += strlen(listEGNumbers[i]);
            free(listEGNumbers[i]);
            listEGNumbers[i] = NULL;
            extractedNums[index] = ',';
            index++;
        }
    }
    if ((index) && (extractedNums[index -1] == ','))
        extractedNums[index-1] ='\0';

    free(emerValue);
            
    LOGD("Emergency Numbers after Attenuation are %s",extractedNums);
}
/**
 * Main function to read and update Modem NV Emergency Call Numbers 
 * to android properties list
 *
 * Adds ECC List to ECCLIST_PROPERTY 
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 *
 */
int ecclistReadModemNVSet(void)
{
    char propValue[PROPERTY_VALUE_MAX];
    int returnValue = 0;
    int  ecclistchecklength;
 
    LOGD("%s: Entered",__func__);
    numdigits = 0;
    listdecodeptr = NULL;
    memset(extractedNums,0,sizeof(extractedNums));

    /** Read the emergency call from Modem NV */
    returnValue = readFromMODEM_NV();
    if (returnValue != 0) {
        LOGE("%s: Could not read Modem NV!! Error",__func__);
    }

    /** Read the Ecc list from persistent ril.ecclist */
    ecclistchecklength = property_get(ECCLIST_PROPERTY, propValue,NULL);
    listdecodeptr = propValue;
    if (listdecodeptr!=NULL && *listdecodeptr!=0) {
        if (numdigits)
            extractedNums[numdigits++] = 0x2C;
        while (*listdecodeptr!=0){
            extractedNums[numdigits++] = *listdecodeptr++;
        }
    }
    /** Clear the Duplicate numbers */
    ecclistAttenuate();
    /** Set the ECCLIST_PROPERTY with update emergency numbers */
    property_set(ECCLIST_PROPERTY, extractedNums);
    ecclistchecklength = property_get(ECCLIST_PROPERTY, propValue,NULL);
    LOGD("Setting %s=%s Length = %d", ECCLIST_PROPERTY, propValue,ecclistchecklength);
    return returnValue;
}

/**
 * Main function to read and update Sim Card Emergency Call Numbers 
 * to android properties list
 *
 * Adds ECC List to ril.ecclist 
 *
 * @return
 *     int status
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 *
 */
int ecclistReadSimCardSet(void)
{
    char propValue[PROPERTY_VALUE_MAX];
    int  returnValue; 
    int  ecclistchecklength;
    enum eGobiImageCarrier image;
    bool bRet= false;    
 
    LOGD("%s: Entered",__func__);

    bRet = getFirmwareInfoCarrierQMI(&image);

    /* Set the android property with required emergency numbers */
    if (bRet && (image == eGOBI_IMG_CAR_ATT)) {
        property_set(ECCLIST_PROPERTY, "911,112");
    }
    else if (bRet && ((image == eGOBI_IMG_CAR_CHINA_TELECOM) ||
        (image == eGOBI_IMG_CAR_GENERIC_CDMA))) {
        property_set(ECCLIST_PROPERTY, "110,112,120,122,119");
    }
    ecclistReadModemNVSet();
    numdigits = 0;
    listdecodeptr = NULL;
    memset(extractedNums,0,sizeof(extractedNums));

    /* Read the emergency call from SIM Card */
    returnValue = readFromSIMCARDQMI();
    if (returnValue != 0) {
        LOGE("%s: Could not read from SIM card Error",__func__);
    }
    /* Read the Ecc list from ril.ecclist */
    ecclistchecklength = property_get(ECCLIST_PROPERTY, propValue,NULL);
    listdecodeptr = propValue;
    if (listdecodeptr!=NULL && *listdecodeptr!=0){
        if (numdigits)
            extractedNums[numdigits++] = 0x2C;
        while (*listdecodeptr!=0){
           extractedNums[numdigits++] = *listdecodeptr++;
        }
    }
   
    /* Clear the Duplicate numbers */
    ecclistAttenuate();
    /* Set the ril.ecclist with update emergency numbers */
    property_set(ECCLIST_PROPERTY, extractedNums);
    ecclistchecklength = property_get(ECCLIST_PROPERTY, propValue,NULL);
    LOGD("Setting %s=%s Length = %d", ECCLIST_PROPERTY, propValue,ecclistchecklength);
    return 0;
}
#endif  // ECC_LIST_INIT



#ifdef HW_TI_OMAP
bool hsmd_open_hsmim()
{
    /*  Open the HSM IM HAL module  */
    if (hw_get_module(HSMIM_MODULE_ID, (const struct hw_module_t **)&mod) == 0) {
        /* Get the hsmim_device_t pointer */
        if (mod->methods->open((const struct hw_module_t *)mod,
                        HSMIM_DEVICE_ID,
                        &dev) == 0) {
            hsmim_dev = (hsmim_device_t *)dev;
        } else {
            return false;
        }
    } else {
        return false;
    }
    return true;
}
void hsmd_close_hsmim()
{
    if (hsmim_dev != NULL) {
        /* Release resource */
        if (hsmim_dev->common.close != NULL) {
            hsmim_dev->common.close((hw_device_t *)hsmim_dev);
        }
    }
}
#endif // HW_TI_OMAP

const char * GetHWAudioVersion(int profile)
{
#ifdef HW_TI_OMAP
    int readLength=0;
    char config_num[64], serial_num[64];
    memset(config_num, 0, sizeof(config_num));
    memset(serial_num, 0, sizeof(serial_num));
    switch (profile)
    {
        case 0 :    // handset
        case 3 :    // speakerphone
            if(hsmd_open_hsmim()) {
                readLength=0;
                hsmim_dev->HsmReadData(hsmim_dev, HSM_CONFIG_NUM, config_num, sizeof(config_num)/sizeof(char), &readLength);
                readLength=0;
                hsmim_dev->HsmReadData(hsmim_dev, HSM_MFG_SERIAL_NUM, serial_num, sizeof(serial_num)/sizeof(char), &readLength);
                hsmd_close_hsmim();
            }
            switch (config_num[14])
            {
            case '2' :
            case 'H' : // Hospital Unit
                return "b";	// IP67 Unit, use Traneat speakerphone settings
            default:             
                serial_num[5] = '\0';
                if (atoi(serial_num) >= 14180)
                    return "b";
                return "a";	// IP54 Unit, using original settings
            }
            break;
    }
#endif
    return "";
}
const char * default_property_get(const char *key)
{
    unsigned i;
    for (i = 0; i < sizeof(default_properties) / sizeof(const char *); i+=2) {
        if (!strcmp(key, default_properties[i]))
            return default_properties[i+1];
    }
    return NULL;
}

/**
 * Audio profile update
 *
 * Get the values of audio parameters form android properties. 
 * Store the values in modem.
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 *
 */

void updateAudioProfile(bool bForceFlag)
{
    char propValue[PROPERTY_VALUE_MAX];
    int checklength;
    ATResponse *atresponse = NULL;
    ATLine *atlinep = NULL;
    int err = 0,stages=0;
    int j=0,value=0;
    char *cmd = NULL;
    int profile=0;
    int generator = 0; /* for voice */
    int numdigits =0; 
    int volumeLevel=0;
    char *prop = NULL;
    int extractedParam;
    static const struct timeval TIMEVAL_RESET_DELAY = { 2, 0 };
     
    asprintf(&cmd, "AT!UNLOCK=\"A710\"");
    err = at_send_command(cmd, &atresponse);
    free(cmd);
    if (err < 0 || atresponse->success == 0)
        LOGE("%s error by AT!UNLOCK command",__func__);
    at_response_free(atresponse);

    if (bForceFlag){
        for(profile=0; profile<8; profile++) {
            /* To store the value of volume level in modem */
            updateVolumeLevel(profile);

            /* To store the value of tx volume gain in modem */
            updateParameters(profile,"txvolumegain","AT!AVTXVOL"); 

            /* To store the value of txagc in modem */  
            updateParameters(profile,"txagc","AT!AVTXAGC");

            /* To store the value of rxagc in modem */
            updateParameters(profile,"rxagc","AT!AVRXAGC");
                
            /* To store the value of codec side tone gain in modem */
            updateParameters(profile,"codecstg","AT!AVCODECSTG");

            /* To store the no of stages for tx pcm IIR filter in modem */
            updateIIRFilterParameters(profile,"tx","AT!AVTXPCMIIRFLTR");

            /* To store the no of stages for rx pcm IIR filter in modem */
            updateIIRFilterParameters(profile,"rx","AT!AVRXPCMIIRFLTR");

            /* To store the value of noise suppression in modem */
            updateParameters(profile,"avns","AT!AVNS");

            /* To store the value of echo cancellation in modem */
            updateParameters(profile,"avec","AT!AVEC");
        }  
        /* To change the value of force flag to 0 after updating all the required audio parameters */
        asprintf(&prop, "%s", "persist.radio.forceflag");
        property_set(prop, RIL_VERSION_STRING);
        free(prop); 
        prop=NULL;
        LOGD("%s restart modem in 2 s !",__func__);
        setRadioState(RADIO_STATE_UNAVAILABLE);
        enqueueRILEvent(RIL_EVENT_QUEUE_PRIO, resetModem, NULL, &TIMEVAL_RESET_DELAY);
        return;
    }
    
   /* 
    * To store the value of volume gains for different volume levels (0-7) and it is  
    * to be done independent of force flag 
    */
    for(profile=0; profile<8; profile++) {
        for (volumeLevel=0; volumeLevel<8; volumeLevel++) {
            asprintf(&prop, "%s%d%s%d", "persist.radio.rxvolumegain",profile,GetHWAudioVersion(profile),volumeLevel);
            checklength = property_get(prop, propValue,default_property_get(prop));
            free(prop);
            prop = NULL;
            if (checklength){  
                asprintf(&cmd, "AT!AVRXVOLDB=%d,%d,%d,%s",profile,generator,volumeLevel,propValue);
                err = at_send_command(cmd, &atresponse);
                free(cmd);
                if (err < 0 || atresponse->success == 0)
                    goto error;
                at_response_free(atresponse);
            }       
        }  
    }
    
    /* Switching of audio profile */
    switchAudioProfile();
    return;	
    error:
        LOGE("%s error by AT command",__func__);
        return; 
   
}

/**
 * Switch Audio profile
 *
 * Get the value of audio profile from Android property. 
 * Set the value in modem.
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 *
 */
void switchAudioProfile(void) 
{
    char propValue[PROPERTY_VALUE_MAX];
    int checklength;
    char *prop = NULL;
    ATResponse *atresponse = NULL;
    ATLine *atlinep = NULL;
    int err = 0;
    int j=0;
    int profile=0,earmute=0,micmute=0,generator=0,volume=0;
    char *line = NULL;
    char *token = NULL;
    char *cmd = NULL;
    
    /* To get the value of audio profile to be made active */
    asprintf(&prop, "%s", "persist.radio.audioprofile");
    checklength = property_get(prop, propValue,NULL);
    free(prop);
    prop = NULL;
    if(checklength) {
        profile = atoi(propValue);

        /* To get the values of earmute,micmute and volume as only profile needs to set here */
        err = at_send_command_multivalue("AT!AVSETPROFILE?generator","", &atresponse);
        if (err < 0 || atresponse->success == 0) {
            goto error;
        }

       atlinep = atresponse->p_intermediates;
       line = atlinep->line;
       /* split the string on these delimiters into "tokens" */
       token = strtok (line,",");
       while (token != NULL) {
           j++;
           if (j==2) {
               earmute = atoi(token);
           }
           if (j==3) {
               micmute = atoi(token);
           }
           if (j==4) {
               volume = atoi(token);
           }
           token = strtok (NULL,",");
        }
        at_response_free(atresponse);

        asprintf(&cmd, "AT!AVSETPROFILE=%d,%d,%d,%d,%d", profile, earmute, micmute, generator, volume);
        err = at_send_command(cmd, &atresponse);
        free(cmd);
        if (err < 0 || atresponse->success == 0)
            goto error;
        at_response_free(atresponse);

        /* Workaround for SL9090 to save volume level in modem */
        asprintf(&cmd, "AT+CLVL=%d", volume);
        at_send_command(cmd,&atresponse);
        free(cmd);
        if (err < 0 || atresponse->success == 0) {
            goto error;
        }
        at_response_free(atresponse); 
        
    }
    return; 
    error:
        at_response_free(atresponse);
        LOGE("%s error by AT command",__func__);
        return;
}

/**
 * Update volume level
 *
 * Get the values of volume for different audio profiles from Android properties. 
 * Set the values in modem.
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 *
 */
void updateVolumeLevel(int profile)
{
    char propValue[PROPERTY_VALUE_MAX];
    int checklength=0;
    ATResponse *atresponse = NULL;
    ATLine *atlinep = NULL;
    char *cmd = NULL;
    int err = 0;
    char *prop = NULL;
    int earmute=0,micmute=0,volume=0;
    int generator=0,j=0;
    char *line = NULL;
    char *token = NULL;
    
    asprintf(&prop, "%s%d%s", "persist.radio.volumelevel",profile, GetHWAudioVersion(profile));	
    checklength = property_get(prop, propValue,default_property_get(prop));
    free(prop);
    prop = NULL;
    if (checklength){
        volume = atoi(propValue);  
        if (volume>=0 && volume<=7) { 
            
            /* To get the values of earmute and micmute as only volume needs to be updated here */
            err = at_send_command_multivalue("AT!AVSETPROFILE?generator","", &atresponse);

            if (err < 0 || atresponse->success == 0) {
                goto error;
            }

            atlinep = atresponse->p_intermediates;
            line = atlinep->line;
            /* split the string on these delimiters into "tokens" */
            token = strtok (line,",");
            while (token != NULL) {
                j++;
                if (j==2) {
                    earmute = atoi(token);
                }
                if (j==3) {
                    micmute = atoi(token);
                }
                token = strtok (NULL,",");
            }
            at_response_free(atresponse);

            asprintf(&cmd, "AT!AVSETPROFILE=%d,%d,%d,%d,%d", profile, earmute, micmute, generator, volume);
            err = at_send_command(cmd, &atresponse);
            free(cmd);
            if (err < 0 || atresponse->success == 0)
                goto error;
            at_response_free(atresponse);

            /* Workaround for SL9090 to save volume level in modem */
            asprintf(&cmd, "AT+CLVL=%d", volume);
            at_send_command(cmd,&atresponse);
            free(cmd);
            if (err < 0 || atresponse->success == 0) {
                goto error;
            }
            at_response_free(atresponse);
        }
        else
            LOGE("%s: Invalid argument (volume) for command AT!AVSETPROFILE",__func__);
    }
    return;
    error:
        at_response_free(atresponse);
        LOGE("%s error by AT command",__func__);
        return;
}

/**
 * Update audio parameters
 *
 * Get the values of audio parameters for different audio profiles from Android properties. 
 * Set the values in modem.
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 *
 */
void updateParameters(int profile,char* property,char* command)
{
    char propValue[PROPERTY_VALUE_MAX];
    int checklength=0;
    ATResponse *atresponse = NULL;
    char *cmd = NULL;
    int err = 0;
    char *prop = NULL;
    asprintf(&prop, "%s%s%d%s", "persist.radio.",property,profile,GetHWAudioVersion(profile));	
    checklength = property_get(prop, propValue,default_property_get(prop));
    free(prop);
    prop = NULL;
    if (checklength){ 
        asprintf(&cmd, "%s=%d,%s",command,profile,propValue);
        err = at_send_command(cmd, &atresponse);
        free(cmd);
        if (err < 0 || atresponse->success == 0)
            goto error;
        at_response_free(atresponse);   
    }    
    return; 
    error:
        LOGE("%s error by AT command",__func__);
        return;
}

/**
 * Update pcm IIR filter parameters
 *
 * Get the values of filter parameters for different audio profiles from Android properties. 
 * Set the values in modem.
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 *
 */
void updateIIRFilterParameters(int profile,char* property,char* command)
{
    int noOfStages=0,stages=0;
    char propValue[PROPERTY_VALUE_MAX];
    int checklength;
    ATResponse *atresponse = NULL;
    char *cmd = NULL;
    char *prop = NULL;
    int err = 0,extractedParam=0;
    asprintf(&prop, "%s%s%s%d%s", "persist.radio.",property,"noofstages",profile,GetHWAudioVersion(profile));
    checklength = property_get(prop, propValue,default_property_get(prop));
    free(prop);
    prop = NULL;
    if (checklength){   
        extractedParam = atoi(propValue);
        if (extractedParam>=0 && extractedParam<=5) {
            stages = extractedParam;
            asprintf(&cmd, "%s=%d,%d,%d",command,profile,0,stages);
            err = at_send_command(cmd, &atresponse);
            free(cmd);
            if (err < 0 || atresponse->success == 0)
                goto error;
            at_response_free(atresponse);

            /* To store the values of stage parameters in modem */
            for (noOfStages = 1; noOfStages <= stages; noOfStages++) {
                asprintf(&prop, "%s%s%s%d%s%d%s", "persist.radio.",property,"stg",noOfStages,"parameter",profile,GetHWAudioVersion(profile));	
                checklength = property_get(prop, propValue,default_property_get(prop));
                free(prop);
                prop = NULL;
                if (checklength){  
                    asprintf(&cmd, "%s=%d,%d,%s",command,profile,noOfStages,propValue);
                    err = at_send_command(cmd, &atresponse);
                    free(cmd);
                    if (err < 0 || atresponse->success == 0)
                        goto error;
                    at_response_free(atresponse);
                }
            }
        }
        else
            LOGE("%s: Invalid argument (no of stages)", __func__);
    }
    return; 
    error:
        LOGE("%s error by AT command",__func__);
        return;
}

bool getDefaultVolumebyProfile(int profile, int *volume)
{
  char propValue[PROPERTY_VALUE_MAX];
  int checklength = 0;
  char *prop = NULL;
  bool result = false;
  int tmp;

  asprintf(&prop, "%s%d%s", "persist.radio.volumelevel",profile, GetHWAudioVersion(profile));
  checklength = property_get(prop, propValue,default_property_get(prop));
  free(prop);
  prop=NULL;

  if (checklength){
     tmp = atoi(propValue);
     if (tmp>=0 && tmp<=7 && volume != NULL) {
       *volume = tmp;
       result = true;
     }
  }
  return result;
}

/* Enable/disable AT timeout */
void EnableATTimeoutHandler(void)
{
    at_set_on_timeout(onATTimeout);
}

void DisableATTimeoutHandler(void)
{
    at_set_on_timeout(NULL);
}

void completeCancelQueryNetworks(void *params){
    RILRequest *r = (RILRequest *)params;
    LOGD("%s:: Complete request OEM cancel query network\n", __func__);
    RIL_onRequestComplete(r->token, RIL_E_SUCCESS, NULL, 0);
    freeRequestData(r->request, r->data, r->datalen);
    free(r);
}

/* FIXME: Required to cancel query network. Can we find a better way?? */
bool isCancelQueryNetworkRequested(void)
{
    RILRequest *r=NULL;
    RILRequest *temp=NULL;
    RILRequest *stacktop=NULL;
    RILRequest *p=NULL;
    bool reqFound = false;
    const char **cur;
    int subcommand;
    RequestQueue *q = &s_requestQueue;

    if (s_requestQueuePrio.enabled)
        q = &s_requestQueuePrio;

    LOGD("%s:: Entered\n", __func__);
    pthread_mutex_lock(&q->queueMutex);
    if (q->closed != 0) {
        pthread_mutex_unlock(&q->queueMutex);
        return false;
    }    
    temp = q->requestList;
    q->requestList = NULL;

    /* Loop through all the pending requests in the queue, if it is 
     * not  a cancel request, push it into a stack to put it 
     * back into queue later in the same order
     * if cancel request, enqueue an event to complete this request 
     * and break the loop 
     */
    while (temp != NULL) {
        r = temp;
        temp = temp->next;
        r->next = NULL;
        if (r->request == RIL_REQUEST_OEM_HOOK_STRINGS) {
             cur = (const char **) r->data;
             subcommand = atoi(*cur);
             if (subcommand == SWI_CANCEL_QUERY_AVAILABLE_NETWORKS) {
                 LOGD("%s:: subcommand OEM cancel query network\n", __func__);
                 /* enqueue event to complete this request */
                 q->requestList = temp;                 
                 pthread_mutex_unlock(&q->queueMutex);
                 enqueueRILEvent(RIL_EVENT_QUEUE_NORMAL, completeCancelQueryNetworks, r, &TIMEVAL_2);
                 pthread_mutex_lock(&q->queueMutex);
                 reqFound = true;                 
                 break;
             }
        }
        /* Push the  out request back onto a stack */
        if (stacktop == NULL)
            stacktop = r;        
        else {
            r->next = stacktop;
            stacktop = r;
        }
    }    
    /* Push at the front of the queue */
    while (stacktop != NULL) {
         p = stacktop;
         stacktop = stacktop->next;
         p->next = NULL;
         if (q->requestList == NULL) {
              q->requestList = p;
          }
          else {
              p->next = q->requestList;
              q->requestList = p;
          }
    }
    pthread_mutex_unlock(&q->queueMutex);
    LOGD("%s:: Exit\n", __func__);
    return reqFound;
}

