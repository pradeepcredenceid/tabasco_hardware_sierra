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
#include <stdbool.h>

#include "at_channel.h"
#include "at_tok.h"
#include "at_misc.h"

#include "swiril_main.h"
#include "swiril_config.h"
#include "swiril_gps.h"
#include "swiril_network.h"
#include "swiril_network_at.h"
#include "swiril_oem_at.h"
#include "swiril_pdp.h"
#include "swiril_sim.h"
#include "swiril_messaging.h"
#include "swiril_callhandling.h"
#include "swiril_requestdatahandler.h"
#include "swiril_cache.h"
#include "swiril_services.h"
#include "swiril_misc.h"
#include "swims_ossdkuproto.h"
#include "swiril_oem.h"

#define LOG_TAG "RIL"
#include "swiril_log.h"

#ifdef HW_TI_OMAP
#include <hsmim.h>
#include <hardware/hardware.h>
static struct hw_module_t       *mod = NULL;
static struct hw_device_t       *dev = NULL;
static struct hsmim_device_t    *hsmim_dev = NULL;
#endif // HW_TI_OMAP

#define RIL_VERSION_STRING  "Sierra Ril V7.1.4 AT"

#define MAX_AT_RESPONSE 0x1000

#define timespec_cmp(a, b, op)         \
        ((a).tv_sec == (b).tv_sec    \
        ? (a).tv_nsec op (b).tv_nsec \
        : (a).tv_sec op (b).tv_sec)

#define IMEISV_STRING "IMEI SV: "

/* RIL solicited command start from 1 */
#ifdef  SWI_RIL_VERSION_10
#define RIL_SOL_NUMBER_MAX  (RIL_REQUEST_SHUTDOWN + 1)
#else
#define RIL_SOL_NUMBER_MAX  (RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING + 1)
#define RIL_REQUEST_SHUTDOWN 129
#endif

#ifdef ECC_LIST_INIT
#warning "ECC list enabled"
/** ECC Read list Macros */
#define ECC_LIST_SIM_READ_START_PTR 22
#define ECC_LIST_BUFFER_SIZE 100
#define ECC_LIST_SIM_READ_START_MAX_LIST 15
#define ECC_LIST_READ_SIM_DUMMY 0x46
#define ECC_LIST_ADD_COMMA 0x2C
#define ECC_LIST_MAX 64
#else
#warning "ECC list disabled"
#endif

/**
 * RIL_Request Handler Function pointer
 *
 * @param data is pointer to data defined for that RIL_REQUEST_*
 *        data is owned by caller, and should not be modified or freed by callee
 * @param t should be used in subsequent call to RIL_onResponse
 * @param datalen the length of data
 *
 */
typedef void (*RIL_RequestHandlerFunc) (void *data, size_t datalen, RIL_Token t);

/*** Declarations ***/
static int isRadioOn();
static void signalCloseQueues(void);
extern const char *requestToString(int request);
static bool checkForEndofUSSD(const char* data);
static void initATRILFunctionHandler(void);
static bool isModemAR75(void);
extern bool s_AR75_Modem;
int g_ppp_enabled;
extern PPPStopService();

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
static pthread_mutex_t s_screen_state_mutex = PTHREAD_MUTEX_INITIALIZER;

static int s_screenState = 1;
static RIL_RequestHandlerFunc s_atrilsolreqhandler[RIL_SOL_NUMBER_MAX];

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


/* Need to keep track of whether the modem supports Circuit Switched (CS)
   or is only Packet Switched (PS)
 */
static int s_modem_ps_only = 0;

#ifdef ECC_LIST_INIT
/* Read Ecc list variables */
static char extractedNums[ECC_LIST_BUFFER_SIZE];
static int numdigits;   
static char* listdecodeptr = NULL;
static int readFromMODEM_NV(void);
#endif

// store the netork selection mode in modem. 0=auto, 1=manual
extern int net_sel_mode;


const char * default_properties[] = 
{
    //NON-AUDIO
    #ifdef RIL_GCF_TEST_FLAG
	"persist.radio.gcfen", "1",
    #else	
	"persist.radio.gcfen", "0",
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
	"persist.radio.txvolumegain2","8f27",
	"persist.radio.txagc2","0",
	"persist.radio.rxagc2","2",
	"persist.radio.codecstg2","0",
	"persist.radio.txnoofstages2","0",
	"persist.radio.rxnoofstages2","0",
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
	"persist.radio.txstg4parameter5","00000000,00000000,47ced916,00000000,00000000",
	"persist.radio.txstg5parameter5","00000000,00000000,65c28f5c,00000000,00000000",
	"persist.radio.rxnoofstages5","5",
	"persist.radio.rxstg1parameter5","babc2a8f,25bb2efd,3a3c9b2c,b276bbd5,24d73e52",
	"persist.radio.rxstg2parameter5","29a8c467,24461169,40000000,2a5f874b,1ffa200a",
	"persist.radio.rxstg3parameter5","df91e7c1,1a041b63,40000000,0c6ffe66,1172c048",
	"persist.radio.rxstg4parameter5","00000000,00000000,65c28f5c,00000000,00000000",
	"persist.radio.rxstg5parameter5","00000000,00000000,65c28f5c,00000000,00000000",
	"persist.radio.avns5","0", 
	"persist.radio.avec5","3",
	"persist.radio.rxvolumegain51","0c8",
	"persist.radio.rxvolumegain52","0c8",
	"persist.radio.rxvolumegain53","0c8",
	"persist.radio.rxvolumegain54","0c8",
	"persist.radio.rxvolumegain55","0c8",
	"persist.radio.rxvolumegain56","0c8",
	"persist.radio.rxvolumegain57","0c8",
	// HEADSET
	"persist.radio.volumelevel1","3",
	"persist.radio.txvolumegain1","8f27",
	"persist.radio.txagc1","0",
	"persist.radio.rxagc1","2",
	"persist.radio.codecstg1","800",
	"persist.radio.txnoofstages1","0",
	"persist.radio.rxnoofstages1","0",
	"persist.radio.avns1","0",
	"persist.radio.avec1","2",
	"persist.radio.rxvolumegain11","f79a",
	"persist.radio.rxvolumegain12","f9f2",
	"persist.radio.rxvolumegain13","fc4a",
	"persist.radio.rxvolumegain14","fdda",
	"persist.radio.rxvolumegain15","ff6a",
	"persist.radio.rxvolumegain16","fa",
	"persist.radio.rxvolumegain17","fa",
	// SPEAKERPHONE (AAC)
	"persist.radio.volumelevel3a","7",
	"persist.radio.txvolumegain3a","ffff",
	"persist.radio.txagc3a","0",
	"persist.radio.rxagc3a","2",
	"persist.radio.codecstg3a","0",
	"persist.radio.txnoofstages3a","5",
	"persist.radio.txstg1parameter3a","f152fe6d,31d00cb6,696e0794,e754a4f2,51f92c60",
	"persist.radio.txstg2parameter3a","b311d37e,268ee6f7,40000000,aa408b5b,26b63ce1",
	"persist.radio.txstg3parameter3a","cb342de5,25e3a0dc,40000000,d5792785,22eeb091",
	"persist.radio.txstg4parameter3a","00000000,00000000,5083126e,00000000,00000000",
	"persist.radio.txstg5parameter3a","00000000,00000000,40000000,00000000,00000000",
	"persist.radio.rxnoofstages3a","5",
	"persist.radio.rxstg1parameter3a","d43b263f,27dfab98,3a946104,d221c1cf,224024fa",
	"persist.radio.rxstg2parameter3a","aa313a2f,21a24fa3,40000000,15b14b7f,1d9b084c",
	"persist.radio.rxstg3parameter3a","f47d61d2,1f8a7c8c,40000000,bc96657a,bfe1683",
	"persist.radio.rxstg4parameter3a","00000000,00000000,40000000,00000000,00000000",
	"persist.radio.rxstg5parameter3a","00000000,00000000,40000000,00000000,00000000",
	"persist.radio.avns3a","0",
	"persist.radio.avec3a","3",
	"persist.radio.rxvolumegain3a1","f768",
	"persist.radio.rxvolumegain3a2","f9c0",
	"persist.radio.rxvolumegain3a3","fc18",
	"persist.radio.rxvolumegain3a4","fda8",
	"persist.radio.rxvolumegain3a5","ff38",
	"persist.radio.rxvolumegain3a6","c8",
	"persist.radio.rxvolumegain3a7","c8",
	// SPEAKERPHONE (Traneat)
	"persist.radio.volumelevel3b","7",
	"persist.radio.txvolumegain3b","ffff",
	"persist.radio.txagc3b","0",
	"persist.radio.rxagc3b","2",
	"persist.radio.codecstg3b","0",
	"persist.radio.txnoofstages3b","5",
	"persist.radio.txstg1parameter3b","f152fe6d,31d00cb6,696e0794,e754a4f2,51f92c60",
	"persist.radio.txstg2parameter3b","b311d37e,268ee6f7,40000000,aa408b5b,26b63ce1",
	"persist.radio.txstg3parameter3b","cb342de5,25e3a0dc,40000000,d5792785,22eeb091",
	"persist.radio.txstg4parameter3b","00000000,00000000,5083126e,00000000,00000000",
	"persist.radio.txstg5parameter3b","00000000,00000000,40000000,00000000,00000000",
	"persist.radio.rxnoofstages3b","5",
	"persist.radio.rxstg1parameter3b","d43b263f,27dfab98,3a946104,d221c1cf,224024fa",
	"persist.radio.rxstg2parameter3b","aa313a2f,21a24fa3,40000000,15b14b7f,1d9b084c",
	"persist.radio.rxstg3parameter3b","f47d61d2,1f8a7c8c,40000000,bc96657a,0bfe1683",
	"persist.radio.rxstg4parameter3b","00000000,00000000,47ced916,00000000,00000000",
	"persist.radio.rxstg5parameter3b","00000000,00000000,40000000,00000000,00000000",
	"persist.radio.avns3b","0",
	"persist.radio.avec3b","3",
	"persist.radio.rxvolumegain3b1","f768",
	"persist.radio.rxvolumegain3b2","f9c0",
	"persist.radio.rxvolumegain3b3","fc18",
	"persist.radio.rxvolumegain3b4","fda8",
	"persist.radio.rxvolumegain3b5","ff38",
	"persist.radio.rxvolumegain3b6","c8",
	"persist.radio.rxvolumegain3b7","c8",
	// HANDSET (AAC)
	"persist.radio.volumelevel0a","3",
	"persist.radio.txvolumegain0a","5400",
	"persist.radio.txagc0a","0",
	"persist.radio.rxagc0a","1",
	"persist.radio.codecstg0a","100",
	"persist.radio.txnoofstages0a","3",
	"persist.radio.txstg1parameter0a","ed88f90d,27db3918,48bb8f24,ea7ec8a0,2db79d61",
	"persist.radio.txstg2parameter0a","592491b0,20fa71f7,40000000,592863a6,1fc36fd0",
	"persist.radio.txstg3parameter0a","2e07d41c,1b9667e1,40000000,23764896,168ad43a",
	"persist.radio.rxnoofstages0a","0",
	"persist.radio.avns0a","1",
	"persist.radio.avec0a","1",
	"persist.radio.rxvolumegain0a1","f736",
	"persist.radio.rxvolumegain0a2","f98e",
	"persist.radio.rxvolumegain0a3","fbe6",
	"persist.radio.rxvolumegain0a4","fd76",
	"persist.radio.rxvolumegain0a5","ff06",
	"persist.radio.rxvolumegain0a6","96",
	"persist.radio.rxvolumegain0a7","96",
	// HANDSET (Traneat)
	"persist.radio.volumelevel0b","3",
	"persist.radio.txvolumegain0b","5400",
	"persist.radio.txagc0b","0",
	"persist.radio.rxagc0b","1",
	"persist.radio.codecstg0b","100",
	"persist.radio.txnoofstages0b","3",
	"persist.radio.txstg1parameter0b","ed88f90d,27db3918,48bb8f24,ea7ec8a0,2db79d61",
	"persist.radio.txstg2parameter0b","592491b0,20fa71f7,40000000,592863a6,1fc36fd0",
	"persist.radio.txstg3parameter0b","2e07d41c,1b9667e1,40000000,23764896,168ad43a",
	"persist.radio.rxnoofstages0b","5",
	"persist.radio.rxstg1parameter0b","F265DBA3,2935E95F,4DF76BFD,FAC9CF6D,33828B36",
	"persist.radio.rxstg2parameter0b","157E12B7,17761B83,40000000,0F4D3468,1D9BD35E",
	"persist.radio.rxstg3parameter0b","2EE9CE59,04BD4341,40000000,33EC7E94,04DC1DC7",
	"persist.radio.rxstg4parameter0b","00000000,00000000,47CF267D,00000000,00000000",
	"persist.radio.rxstg5parameter0b","00000000,00000000,40000000,00000000,00000000",
	"persist.radio.avns0b","1",
	"persist.radio.avec0b","1",
	"persist.radio.rxvolumegain0b1","f736",
	"persist.radio.rxvolumegain0b2","f98e",
	"persist.radio.rxvolumegain0b3","fbe6",
	"persist.radio.rxvolumegain0b4","fd76",
	"persist.radio.rxvolumegain0b5","ff06",
	"persist.radio.rxvolumegain0b6","96",
	"persist.radio.rxvolumegain0b7","96",
    #else
    #warning "Audio Provisioning disabled"
    #endif // AUDIO_PROVISIONING
};

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

    /* everything was okay, as indicated by a return value of zero */
    return 0;
}

#ifdef ECC_LIST_INIT
static void readNVForEmergencyNumbers()
{
    LOGD("%s: Entered",__func__);
    /* Set the android property with required emergency numbers */
    property_set(ECCLIST_PROPERTY, "911,112,08,000,110,118,119,999");
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
    ATResponse *atresponse = NULL;
    int err = 0;
    int retStatus = -1;

    /* If the screen is currently on, then turn on all AT unsolicited notifications
       related to the ScreenState command. We don't need locking here, since the
       return value fits into a single word.

       IMPORTANT: This relies on the initial value of the screen to be on, so that
       these get enabled during system initialization.
     */
    if (getScreenState()) {
        setATUnsolNotifState(1);
    }

    /* SWI_TBD: workaround for first wake up ring dropped from full suspend */
    at_send_command("AT+CLIP=1", NULL);

    /* SWI_TBD
       Should TZ reporting be turned off when the screen is off.  If it is, then
       a fake NITZ event should be generated when the screen is turned back on.
     */
    /* Configure time zone change event reporting
     *    1 = Enable time zone change event reporting.
     */
    at_send_command("AT+CTZR=1", NULL);

    /* Select message service */
    at_send_command("AT+CSMS=0", NULL);

   /* Configure new messages indication 
    *  mode = 1 - Discard indication and reject new received message 
    *             unsolicited result codes when TA-TE link is reserved
    *             (e.g. in on-line data mode).
    *             Otherwise forward them directly to the TE.
    *  mode = 2 - Buffer unsolicited result code in TA when TA-TE link is 
    *             reserved(e.g. in on.line data mode) and flush them to the 
    *             TE after reservation. Otherwise forward them directly to 
    *             the TE. 
    *  mt   = 1 - SMS-DELIVERs are rounted using unsolicited code: +CMTI: 
                  "SM", <index>
    *  mt   = 2 - SMS-DELIVERs (except class 2 messages and messages in the 
    *             message waiting indication group (store message)) are 
    *             routed directly to TE using unsolicited result code:
    *             +CMT: [<alpha>],<length><CR><LF><pdu> (PDU mode)
    *             Class 2 messages are handled as if <mt> = 1
    *  bm   = 2 - New CBMs are routed directly to the TE using unsolicited
    *             result code:
    *             +CBM: <length><CR><LF><pdu> (PDU mode)
    *  ds   = 1 - SMS-STATUS-REPORTs are routed to the TE using unsolicited
    *             result code: +CDS: <length><CR><LF><pdu> (PDU mode)
    *  ds   = 2 - SMS-STATUS-REPORTs are stored and routed using unsolicited
    *             result code: +CDSI: "SR", <index>
    *  dfr  = 0 - TA buffer of unsolicited result codes defined within this
    *             command is flushed to the TE when <mode> 1...3 is entered
    *             (OK response is given before flushing the codes).
    */
    /* Mode 1 or Mode 2 support would be carrier dependent and should be 
       definable by the customer */
#ifdef RIL_GCF_TEST_FLAG
	at_send_command("AT+CNMI=2,1,2,2,0", NULL);
#else
    at_send_command("AT+CNMI=1,2,2,1,0", NULL);
    /* at_send_command("AT+CNMI=2,1,2,2,0", NULL); */
#endif

    /* Configure preferred message storage 
     *   mem1 = SM, mem2 = SM, mem3 = SM
     */
    at_send_command("AT+CPMS=\"SM\",\"SM\",\"SM\"", NULL);

    /* Configure Short Message (SMS) Format 
     *  mode = 0 - PDU mode.
     */
    at_send_command("AT+CMGF=0", NULL);

    /* Configure mute control.
     *  n 0 - Mute off
     */
    /* at_send_command("AT+CMUT=0", NULL);
    setCurrentMuteValue(0); */

    /* Subscribe to Unstuctured Supplementary Service Data (USSD) notifications.
     *  n = 1 - Enable result code presentation in the TA.
     */
    at_send_command("AT+CUSD=1", NULL);

    /* enable voicemail indication */
    at_send_command("AT+CPHS=1,1", NULL);

    /* Subscribe to Supplementary Services Notification
     *  n = 1 - Enable the +CSSI result code presentation status.
     *          Intermediaate result codes. When enabled and a supplementary
     *          service notification is received after a mobile originated
     *          call setup.
     *  m = 1 - Enable the +CSSU result code presentation status.
     *          Unsolicited result code. When a supplementary service 
     *          notification is received during a mobile terminated call
     *          setup or during a call, or when a forward check supplementary
     *          service notification is received.
     */
    at_send_command("AT+CSSN=1,1", NULL);



    /* Enabling modem reporting rejected cause,
     * the indication will be pop up once the registered/attached state has been changed:
     * !Rind: <reject_domain>,<reject_cause>
     *   <reject_domain>
     *   1 CS domain
     *   2 PS domain
     *   3 Both CS and PS domain
     *   <reject_cause>
     *   0             No reject cause for corresponding <reject_domain>
     *   254           Unbarred for corresponding <reject_domain>
     *   255           Barred for corresponding <reject_domain>
     *   Other value   See the Annex G and Annex I in 3GPP TS24.008
     */
    /* Reject Cause indication for SL8092, disable by default, set to 1 to enable */
    at_send_command("AT!RIND=0", NULL);

#ifdef ECC_LIST_INIT
    /** Read ECC List and populate android property */
    ecclistReadSimCardSet();  
#endif    
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
static int getIMEISV(char **datap)
{
    ATResponse *atresponse = NULL;
    char *bufp = NULL;
    int err;
    
    /* ATI response after IMEI SV will go to onUnsolicited() */
    err = at_send_command_singleline("ATI", IMEISV_STRING, &atresponse);

    if (err < 0 || atresponse->success == 0) 
    {
        at_response_free(atresponse);
        return -1;
    }

    bufp = strstr(atresponse->p_intermediates->line, IMEISV_STRING);
    if (bufp != NULL)
    {
        asprintf(datap, "%s", bufp + strlen(IMEISV_STRING));
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
 * Toggle radio on and off (for "airplane" mode).
 *
 * @param[in] data 
 *     Pointer to the request data
 * @param datalen 
 *     request data length
 * @param t 
 *     RIL token
 *
 * @return
 *     none
 *
 * @par Abort:<br> 
 *     none
 *
 * @note
 *     none
 */
static void requestRadioPower(void *data, size_t datalen, RIL_Token t)
{
    int onOff;
    int err;
    ATResponse *atresponse = NULL;

    assert(datalen >= sizeof(int *));
    onOff = ((int *) data)[0];

    if (onOff == 0 && sState != RADIO_STATE_OFF) {

        /* De-activate the PDP and stop DHCP if running */
        deactivatePDPContext();
        checkDHCPnStopService();

        if (g_ppp_enabled)
            PPPStopService();

        err = at_send_command("AT+CFUN=0", &atresponse);
        if (err < 0 || atresponse->success == 0)
            goto error;
        setRadioState(RADIO_STATE_OFF);
        /* clean up data call list when radio is turned off */
        initDataCallResponseList();
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
 * RIL_REQUEST_SHUTDOWN
 *
 * Device is shutting down. All further commands are ignored
 * and RADIO_NOT_AVAILABLE must be returned.
 *
 * "data" is null
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  OPERATION_NOT_ALLOWED
 *  GENERIC_FAILURE
 */
static void requestShutdown(void *data, size_t datalen, RIL_Token t)
{
    int err;
    ATResponse *atresponse = NULL;
    
    if (sState != RADIO_STATE_OFF) {
        err = at_send_command("AT+CFUN=0", &atresponse);
        if (err < 0 || atresponse->success == 0) {
            RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        }    
        else {
            RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
            setRadioState(RADIO_STATE_UNAVAILABLE);
        }    
        at_response_free(atresponse);        
    }
    else {
        RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    }      
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

static void requestScreenState(void *data, size_t datalen, RIL_Token t)
{
    int err, screenState;

    assert(datalen >= sizeof(int *));

    pthread_mutex_lock(&s_screen_state_mutex);
    screenState = s_screenState = ((int *) data)[0];

    if ((screenState == 1) || (screenState == 0)) {
#ifdef NO_LCD_OFF_IND
        err = setATUnsolNotifState(screenState);
        if (err < 0)
            goto error;
#endif
    } else {
        /* Not a defined value - error. */
        goto error;
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

finally:
    pthread_mutex_unlock(&s_screen_state_mutex);
    return;

error:
    LOGE("ERROR: requestScreenState failed");
    if (t != (RIL_Token) 0)
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);

    goto finally;
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
             request == RIL_REQUEST_SHUTDOWN)) {
        RIL_onRequestComplete(t, RIL_E_RADIO_NOT_AVAILABLE, NULL, 0);
        return;
    }

    /* Ignore all non-power requests when RADIO_STATE_OFF
     * (except RIL_REQUEST_GET_SIM_STATUS and a few more).
     */
    if ((sState == RADIO_STATE_OFF || sState == RADIO_STATE_SIM_NOT_READY)
        && !(request == RIL_REQUEST_RADIO_POWER || 
             request == RIL_REQUEST_GET_SIM_STATUS ||
             request == RIL_REQUEST_GET_IMEISV ||
             request == RIL_REQUEST_GET_IMEI ||
             request == RIL_REQUEST_BASEBAND_VERSION || 
             request == RIL_REQUEST_OEM_HOOK_STRINGS ||
             request == RIL_REQUEST_SCREEN_STATE ||
             request == RIL_REQUEST_SHUTDOWN)) {
        RIL_onRequestComplete(t, RIL_E_RADIO_NOT_AVAILABLE, NULL, 0);
        return;
    }
    
    /* 
     * These commands won't accept RADIO_NOT_AVAILABLE, so we just return
     * GENERIC_FAILURE if we're not in SIM_STATE_READY.
     */
    if (sState != RADIO_STATE_SIM_READY
        && (request == RIL_REQUEST_WRITE_SMS_TO_SIM ||
            request == RIL_REQUEST_DELETE_SMS_ON_SIM ||
            request == RIL_REQUEST_SCREEN_STATE)) {
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
             request == RIL_REQUEST_SCREEN_STATE ||
             request == RIL_REQUEST_SHUTDOWN )) {
        RIL_onRequestComplete(t, RIL_E_RADIO_NOT_AVAILABLE, NULL, 0);
        return;
    }

    /* Check if network mode is changed. If not, just return success. BZ48550 */
    if (request == RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC && net_sel_mode == 0) {
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

    /* check handler struct, call handler if existing */
    if ((request > 0) && 
        (request < RIL_SOL_NUMBER_MAX) &&
        (s_atrilsolreqhandler[request] != NULL)) { 
        s_atrilsolreqhandler[request](data, datalen, t);
    }
    else {
        LOGW("Unsupported request logged: %s",
             requestToString(request));
        RIL_onRequestComplete(t, RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0);
    }
}

/*** Callback methods from the RIL library to us ***/

/**
 * Call from RIL to us to make a RIL_REQUEST.
 *
 * Must be completed with a call to RIL_onRequestComplete().
 */
void onRequest(int request, void *data, size_t datalen, RIL_Token t)
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

void setRadioState(RIL_RadioState newState)
{
    RIL_RadioState oldState;

    pthread_mutex_lock(&s_state_mutex);

    oldState = sState;

    if (sState != newState) {
        sState = newState;
    }

    pthread_mutex_unlock(&s_state_mutex);

    /* Do these outside of the mutex. */
    if (sState != oldState || sState == RADIO_STATE_SIM_LOCKED_OR_ABSENT) {
        RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED,
                                  NULL, 0);

        if (sState == RADIO_STATE_SIM_READY) {
            enqueueRILEvent(RIL_EVENT_QUEUE_PRIO, onSIMReady, NULL, NULL);
        } else if (sState == RADIO_STATE_SIM_NOT_READY) {
            enqueueRILEvent(RIL_EVENT_QUEUE_NORMAL, onRadioPowerOn, NULL, NULL);
        } else if (sState == RADIO_STATE_SIM_LOCKED_OR_ABSENT) {
#ifdef ECC_LIST_INIT
            enqueueRILEvent(RIL_EVENT_QUEUE_PRIO,readNVForEmergencyNumbers, NULL, NULL);
#endif
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

    /* disable SIM took kit to prevent unpredictable indications */
    at_send_command("AT!UNLOCK=\"A710\"", NULL);
    at_send_command("AT!CUSTOM=\"STKUIEN\",0", NULL);

    /* Config SMS retry to 60s. Default value is 240s.
     * 240s SMS retry will cause AT channel timeout */
    at_send_command("AT!SMSRETRY=60,5", NULL);

    return 0;
error:
    return 1;
}

/**
 * Initialize everything that can be configured while we're still in
 * AT+CFUN=0.
 *
 * @param check_reset_source 
 *     0  - assume a valid reset and continue with normal init.
 *     1  - check the source of the reset and act accordingly
 *
 * @return
 *     none
 *
 * @par Abort:<br> 
 *     none
 *
 * @note
 *     Most of the AT unsolicited result code reporting is only turned
 *     on once the SIM is ready, in onSIMReady().
 */
static char initializeChannel(int check_reset_source)
{
    int err;
    ATResponse *atresponsep = NULL;
    ATLine *atlinep = NULL;
    char *linep;
    char *cmdp;

    int pid;
    bool pdp_status;

    /* In order to determine the source of the init/reset, we need to keep
       track of the previous time values in between function calls.
    */
    static int prev_curr_time=-1;
    static int prev_bootup_time=-1;
    static int prev_elapsed_time=-1;
    int new_curr_time;
    int new_bootup_time;
    int new_elapsed_time;
    int real_boot;
    struct timespec ts;

    int modem_mode;


    LOGI("initializeChannel()");

    /* Get some useful status info about the modem every time the RIL starts up.
       Mainly used for testing and debugging.
     */
    err = at_send_command_multivalue("AT!GCDUMP", "", NULL);
    if (err < 0)
        goto error;
        
    err = at_send_command_multivalue("AT!ERR", "", NULL);
    if (err < 0)
        goto error;


    /* Determine if there is an active PDP context, which would often be
       the case if we had a re-enumeration.  Note that AT!SCACT could return
       an error if the SIM is PIN locked.  In this case, assume that there is
       no active PDP context.
     */
    pdp_status = isDataSessionActive();

    /* Get the current time and bootup time */
    err = at_send_command_multivalue("AT!GSTATUS?", "!GSTATUS:", &atresponsep);
    if (err < 0 || atresponsep->success == 0) {
        goto error;
    }

    /* Get the elapsed time since some point in the past. Don't care about
       the absolute value, but will only use the difference. CLOCK_MONOTONIC
       is used because it will not be affected by changes to the system time.
     */
    if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1) {
        LOGE("------ clock_gettime(CLOCK_MONOTONIC) not supported");
        new_elapsed_time = -1;
    } else {
        /* SWI_TBD -- for testing
        LOGE("------ clock_gettime: tv_sec=%ld, tv_nsec=%ld", ts.tv_sec, ts.tv_nsec);
         */
        new_elapsed_time = ts.tv_sec;
    }

    /* Extract the current time and bootup time, discarding the line containing
       the prefix.

       Note that the time is uint32, whereas at_tok_nextint() returns an int
       which is the same as int32. This is not seen as a problem because it
       would take over 68 years of continuous running to overflow an int32.
       i.e. (2^31-1)/(60*60*24*365)) = 68.096
     */
    atlinep=atresponsep->p_intermediates->p_next;
    linep = atlinep->line;

    err = at_tok_start(&linep);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&linep, &new_curr_time);
    if (err < 0)
        goto error;

    atlinep=atlinep->p_next;
    linep = atlinep->line;

    err = at_tok_start(&linep);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&linep, &new_bootup_time);
    if (err < 0)
        goto error;

    at_response_free(atresponsep);
    atresponsep = NULL;


    /* Check whether the modem was actually reset or power-cycled, or we got a
       false alarm, i.e. just a re-enumeration.
     */

    /* Check if we had a real reset/crash */
    real_boot = (prev_bootup_time != new_bootup_time);
    LOGI("initializeChannel real_boot=%i", real_boot);
    
    /* Check if we had a power cycle.  These checks are necessary because a
       boot up time of 0 is valid for both power-cycles and re-enumerations.
     */
    if (!real_boot && new_bootup_time==0) {

        /* A power-cycle happened if current time has gone backwards */
        real_boot = (new_curr_time < prev_curr_time);
        LOGI("initializeChannel real_boot=%i", real_boot);

        if ( (!real_boot) && (new_curr_time < 60) ) {
            /* Compare the delta in modem time with the delta in system time. If
               the deltas are not the same or close, then a power-cycle happened.
            
               We only need to do this check after a suspected power-cycle, so
               only do it within a short period of time of the modem starting. A
               value of 60 seconds was chosen, since under normal circumstances
               the RIL should definitely be communicating with the modem within
               60 seconds of a modem power-cycle.  Thus, after 60 seconds, we
               can safely conclude that this was a re-enumeration.

               Another reason for only comparing the deltas within the first 60
               seconds is that the deltas will be kept small, and this will reduce
               the effects of any clock drift between the modem and system clocks.

               For comparing the deltas, 3 is a fudge factor for clock sampling 
               time and drift.  It could be larger, but it is better to falsely
               declare a power-cycle, than to falsely declare a re-enumeration.
               On the other hand, a smaller number might cause too many false
               power-cycles to be declared.
             */
            real_boot = ( abs( (new_curr_time-prev_curr_time) -
                               (new_elapsed_time-prev_elapsed_time) ) > 3 );
            LOGI("initializeChannel real_boot=%i", real_boot);
        }
    }

    LOGI("initializeChannel bootup time: prev=%i new=%i", prev_bootup_time, new_bootup_time);
    LOGI("initializeChannel current time: prev=%i new=%i diff=%i",
        prev_curr_time, new_curr_time, new_curr_time-prev_curr_time);
    LOGI("initializeChannel elapsed time: prev=%i new=%i diff=%i",
        prev_elapsed_time, new_elapsed_time, new_elapsed_time-prev_elapsed_time);
    LOGI("initializeChannel real_boot=%i, check_reset_source=%i", real_boot, check_reset_source);

    prev_bootup_time = new_bootup_time;
    prev_curr_time = new_curr_time;
    prev_elapsed_time = new_elapsed_time;

    /* Do we want to act upon the reset source info. It may be that we've already
       notified the application that the modem is down, so no point trying to
       hide the reset now.
     */
    if (check_reset_source) {

        /* If the reset was not valid, then just restart the dhcp client, which
           exited due to the re-enumeration, and skip the rest of the init.
         */
        if ( !real_boot ) {
            
            /* Only restart dhcp client, if the PDP context is active */
            if (pdp_status) {
                if (property_set("ctl.start", "sierra_dhcpcd") < 0) {
                    LOGE("initializeChannel: FAILED to set ctl.start sierra_dhcpcd property!");
                    goto error;
                }
            } else {
                /* Normally, auto-suspend will be enabled when the dhcp client
                   finishes. However, since the dhcp client is not started here,
                   auto-suspend must be explicitly enabled.
                 */
                if (property_set("ril.sierra.enable_autosuspend", "1") < 0) {
                    LOGE("FAILED to set ril.sierra.enable_autosuspend property!");
                }
            }

            /* init was successful */
            return 0;
        }

        /* The reset was valid, so inform the upper layers by falling through
           to the normal init code.  Note that previously the radio state was
           set to UNAVAILABLE in onATReaderClosed() when the AT port was closed.
           However, that step can be skipped here, since the radio state will
           be set to OFF at the start of the normal init code below.
         */

    }

    /* Check whether modem is PS only.  This is a PRI configuration setting and
     * so will not change.  There is no need to check for CS only, since that
     * would never happen for a correctly PRIed modem.
     */
    err = at_send_command_singleline("AT!SELMODE?", "!SELMODE:", &atresponsep);
    if (err < 0 || atresponsep->success == 0)
        goto error;

    linep = atresponsep->p_intermediates->line;
    err = at_tok_start(&linep);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&linep, &modem_mode);
    if (err < 0)
        goto error;

    if (modem_mode == 1) {
        LOGI("initializeChannel: PS only is set");
        s_modem_ps_only = 1;
    } else {
        LOGI("initializeChannel: PS only is NOT set");
        s_modem_ps_only = 0;
    }

    at_response_free(atresponsep);
    atresponsep = NULL;

    /* De-activate the PDP and stop DHCP if running */
    deactivatePDPContext();

    /* Restart DHCP to bring WWAN0 device to "up" state */
    if (property_set("ctl.start", "sierra_dhcpcd")) {
        LOGE("%s FAILED to set ctl.start sierra_dhcpcd property!", __func__);
    }

    checkDHCPnStopService();

    if (g_ppp_enabled)
        PPPStopService();

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
    /* clean up data call list when radio is turned off */
    initDataCallResponseList();

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
    at_response_free(atresponsep);
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
 * function handler for Voicemail unsolicited response.
 * 
 */

void onNewVMIndication(const char *s)
{
    int err;
    int response[2];            // <n> and <m>
    char *line;
    char  msgWaitIndResponse[3];
    line = s ;

    memset(msgWaitIndResponse, 0, sizeof(msgWaitIndResponse));

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    /* Parse and store <n> as first repsonse parameter. */
    err = at_tok_nextint(&line, &(response[0]));
    if (err < 0)
        goto error;

    /* Parse and store <m> as second response parameter. */
    err = at_tok_nextint(&line, &(response[1]));
    if (err < 0)
        goto error;

    LOGD("::WVMI parsed values %d %d  \n", response[0],response[1]);

    if(response[0])
    {
        msgWaitIndResponse[0] = SWI_IND_MSG_WAIT_INFO;
        msgWaitIndResponse[1] = 0; /* MWI_MESSAGE_TYPE_VOICMAIL */
        msgWaitIndResponse[2] = response[1];
        RIL_onUnsolicitedResponse(RIL_UNSOL_OEM_HOOK_RAW,
                msgWaitIndResponse, sizeof(msgWaitIndResponse));
        LOGD("UNSOL OEM sent");
    }

finally:
    free(line);
    return;

error:
    LOGE("Failed to parse +WVMI.");
    goto finally;
}


/**
 * function handler for modem reject cause indication
 *
 */

void onRejectCauseIndication(const char *s)
{
    int err;
    int response[2];            //  <reject_domain> & <reject_cause>
    char *line;
    char  msgRegIndResponse[3];
    line = s ;

    memset(msgRegIndResponse, 0, sizeof(msgRegIndResponse));

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    /* Parse and store <reject_domain> as first repsonse parameter. */
    err = at_tok_nextint(&line, &(response[0]));
    if (err < 0)
        goto error;

    /* Parse and store <reject_cause> as second response parameter. */
    err = at_tok_nextint(&line, &(response[1]));
    if (err < 0)
        goto error;

    LOGD("::modem reject cause parsed values %d %d  \n", response[0],response[1]);


    msgRegIndResponse[0] = SWI_IND_REJ_CAUSE_INFO;
    msgRegIndResponse[1] = response[0];
    msgRegIndResponse[2] = response[1];
    RIL_onUnsolicitedResponse(RIL_UNSOL_OEM_HOOK_RAW,msgRegIndResponse, sizeof(msgRegIndResponse));
    LOGD("UNSOL OEM sent");


finally:
    free(line);
    return;

error:
    LOGE("Failed to parse !Rind");
    goto finally;
}

/**
 * Handle the unsolicited response and received SMS
 *
 * @param[in] s 
 *     Pointer to the unsolicited response
 * @param[in] sms_pdu 
 *     Pointer to the received SMS PDU
 *
 * @return
 *     none
 *
 * @par Abort:<br> 
 *     none
 *
 * @note
 *     Called by atchannel when an unsolicited line appears.
 *     This is called on atchannel's reader thread. AT commands may
 *     not be issued here.
 */
static bool onUnsolicited(const char *s, const char *sms_pdu)
{
    int err;
    static bool ussdStart = FALSE; /* To indicate start of USSD
                                    * TRUE,  USSD response started i.e 1st line of ussd
                                    * FALSE, USSD response completed, programmer need to
                                    *        make it false once USSD receive completed
                                    */
    static char* ussdString = NULL; /* USSD data parsed to this buffer */
    bool bRet = FALSE;

    /* Ignore unsolicited responses until we're initialized.
       This is OK because the RIL library will poll for initial state. */
    if (sState == RADIO_STATE_UNAVAILABLE) {
        return FALSE;
    }

    /* USSD Response parsing:
     * USSD Response have multiple lines of data, starts with "+CUSD" and the
     * USSD data to be sent to Android UI is enclosed in a quotes ("<USSD data>").
     * So do following to parse the ussd response into single buffer
     * 1. check for start of USSD (+CUSD),
     * 2. check for start quotes,
     * 3. parse the data into buffer till end quotes received,
     * 4. process the response and send to Android UI
     */
    if ( TRUE == ussdStart ) {
        char* tmp = NULL;

        /* Take back up of previous line into temp buffer */
        tmp = ussdString;

        /* Allocate memory for the current line received from network */
        ussdString = (char*)malloc(strlen(tmp)+strlen(s)+3);

        /* Restore previous line into ussdString from temp buffer */
        strcpy(ussdString,tmp);

        /* Concatenate carriage return and next line */
        strcat(ussdString,"\r\n");

        /* concatenate the current line with previous ussd data */
        strcat(ussdString,s);

        /* free the temp buffer */
        free(tmp);

        bRet = checkForEndofUSSD(s); /* check for end of USSD response */
        if ( TRUE == bRet ) {
        	/* Indicate USSD response completed */
        	ussdStart = FALSE;

            /* process USSD response */
            onUSSDReceived(ussdString);

            /* free ussd buffer */
            free(ussdString);
            ussdString = NULL;
        }
    }

    if (strStartsWith(s, "+CREG:")
        || strStartsWith(s, "+CGREG:"))
    {
        /* If registration status has changed, clear the network cache data */
         if(onNetworkRegistrationReceived(s))
             swiril_cache_clearnetworkdata();

    } else if (strStartsWith(s, "+CTZV:")) {
        /* time zone changed; update the NITZ time */
        /* SWI_TBD
           If we're in screen off state, we have disabled CREG, but the CTZV may
           catch network related events, so we may want to send network state
           changed as well on NITZ in this case.  Think about it some more.

           RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_NETWORK_STATE_CHANGED,
                                      NULL, 0);
         */
        onNetworkTimeReceived(s);
        
    } else if (strStartsWith(s, "+CGEV:"))
    {
        /* SWI_TBD need to verify "+CGEV: REJECT ..." */
        if ((strstr(s, "NW DEACT") != NULL) ||
            (strstr(s, "NW DETACH") != NULL) ||
            (strstr(s, "ME DETACH") != NULL) ||
            (strstr(s, "ME DEACT") != NULL) ||
            (strstr(s, "NW REACT") != NULL))
        {
            static const struct timeval TIMEVAL_DISCONNECT = { 2, 0 };
            enqueueRILEvent(RIL_EVENT_QUEUE_NORMAL, onPDPContextListChanged, NULL, &TIMEVAL_DISCONNECT);
        }
    } else if (strStartsWith(s, "+CIEV:")) {
        onIndicatorEvent(s);
    } else if (strStartsWith(s, "+CMT:")) {
        onNewSms(sms_pdu);
    } else if (strStartsWith(s, "+CDS:")) {
        onNewStatusReport(sms_pdu);
    } else if (strStartsWith(s, "+CRING:")
               || strStartsWith(s, "RING")) {
        /**
         * Start the call state polling and send RING indication to the
         * application
         */
        if (isVoiceEnabled() && (!s_AR75_Modem)) {
            startCallStatePolling();
            RIL_onUnsolicitedResponse(RIL_UNSOL_CALL_RING, NULL, 0);
        }
    } else if (strStartsWith(s, "+CUSD:")) {
        /* Indicate USSD response start */
    	ussdStart = TRUE;

    	/* Allocate memory to store USSD data 1st line */
        ussdString = (char*)malloc(strlen(s)+1);
        /* Initialize memory with 0 */
        memset(ussdString,0,strlen(s)+1);
        /* copy the USSD data into the buffer ussdString */
        strcpy(ussdString,s);

        bRet = checkForEndofUSSD(s); /* check of the end USSD */
        if ( TRUE == bRet ) {
        	/* Indicate USSD response completed */
            ussdStart = FALSE;

            /* process USSD response */
            onUSSDReceived(s);

            /* free ussd buffer */
            free(ussdString);
            ussdString = NULL;

        }
    } else if (strStartsWith(s, "+CSSI:")) {
        onSuppServiceNotification(s, 0);
    } else if (strStartsWith(s, "+CSSU:")) {
        onSuppServiceNotification(s, 1);
    } else if (strStartsWith(s, "+CMTI:")) {
        onNewSmsOnSIM(s);
    }
    else if (strStartsWith(s, "+WVMI:")) /* check if it is an voicemail indication */
    {
        onNewVMIndication(s);
    }
    else if (strStartsWith(s, "!Rind:")) /* check if it is an Reject Cause indication*/
    {
        onRejectCauseIndication(s);
    }
    else if (strStartsWith(s, "CONNECT"))
    {
        //Do nothing
    }
    else
        /* No known indication found */
        return FALSE;

    /* Found a indication*/
    return TRUE;
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

/* Called on command or reader thread. */
static void onATReaderClosed()
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

/* Called on command thread. */
static void onATTimeout()
{
    LOGI("AT channel timeout; restarting..\n");
    /* Last resort, throw escape on the line, close the channel
       and hope for the best. */
    at_send_escape();

    setRadioState(RADIO_STATE_UNAVAILABLE);
    signalCloseQueues();

    /* TODO We may cause a radio reset here. */
}

static void usage(char *s)
{
    fprintf(stderr, "usage: %s [-z] [-p <tcp port>] [-d /dev/tty_device] [-x /dev/tty_device] [-i <network interface>]\n", s);
    exit(-1);
}

/* To check if modem is AR75xx */
static bool isModemAR75(void)
{
    int err;
    ATResponse *atresponse = NULL;
    bool b_AR75_Modem = false;
    
    err = at_send_command_singleline("AT+CGMM", "", &atresponse);

    if (err < 0 ||
        atresponse->success == 0 ||
        atresponse->p_intermediates == NULL) {
        at_response_free(atresponse);
        return b_AR75_Modem;
    }

    if (strstr(atresponse->p_intermediates->line, "AR75"))
    {
        b_AR75_Modem = true;
        s_AR75_Modem = true;
    }
    at_response_free(atresponse);
    return b_AR75_Modem;
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
    int fd;
    int ret;
    int checklength;
    struct queueArgs *queueArgs = (struct queueArgs *) param;
    struct RequestQueue *q = NULL;
    char * tty_name = NULL;
    char propValue[PROPERTY_VALUE_MAX];
    char *prop = NULL;
    bool bForceFlag = false;    

    /* Counts down the number of modem detect attempts, before concluding that
       the modem has probably gone away.  Used when the AT channel gets closed
       and re-opened, to determine when/if to inform the application. A value
       of zero indicates that the counter is not running.
     */
    int restart_detect_count=0;

    LOGI("queueRunner starting!");

    for (;;) {
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
                        /* open the port */
                        if (tty_name != NULL) {
                            fd = open(tty_name, O_RDWR);
                        }
                    }
                }
                else {
                    if (queueArgs->device_path != NULL) {
                        fd = open(queueArgs->device_path, O_RDWR);
                    }
                }

                if (fd >= 0 && ((queueArgs->device_path != NULL && !memcmp(queueArgs->device_path, "/dev/ttyS", 9)) 
                                || (tty_name != NULL && strstr(tty_name, "/dev/ttyUSB") != NULL))) {

                    /* Disable echo on serial ports. */
                    struct termios ios;
                    tcgetattr(fd, &ios);
                    cfmakeraw(&ios);
                    cfsetospeed(&ios, B115200);
                    cfsetispeed(&ios, B115200);
                    ios.c_cflag |= CREAD | CLOCAL;
                    tcflush(fd, TCIOFLUSH);
                    tcsetattr(fd, TCSANOW, &ios);
                }
            }

            if (fd < 0) {
                if (queueArgs->isautodetect != 0) {
                    if (tty_name != NULL) {
                        LOGE("FAILED to open AT channel %s (%s), retrying in 2 sec.", 
                            tty_name, strerror(errno));
                    }
                    else {
                        LOGE("FAILED to auto-detect AT channel, retrying in 2 sec.");
                    }

                    /* If counting down the modem detect attempts and it expires
                       (i.e. reaches zero), inform the application that the modem
                       has gone away and then continue trying to detect the modem.
                     */
                    if (restart_detect_count > 0) {
                        restart_detect_count--;
                        if (restart_detect_count == 0) {
                            setRadioState(RADIO_STATE_UNAVAILABLE);
                        }
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

        ret = at_open(fd, onUnsolicited);

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

        if (queueArgs->isPrio == 0) {
            q->closed = 0;
            /* If the AT channel was detected before the counter expired, then
               check if this is a real reset, or just a re-enumeration.
             */
            if (initializeChannel(/*restart_detect_count>0*/ 0)) {
                LOGE("FAILED to initialize channel!");
                at_close();
                continue;
            }
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

        /** Get the value of force flag from android property */
        asprintf(&prop, "%s", "persist.radio.forceflag");
        checklength = property_get(prop, propValue,NULL);
        free(prop); 	
        prop=NULL;
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
            #endif

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

            // Enable default GPS settings
            asprintf(&cmd, "AT!GPSLBSSETTINGS = 2,255,1,1000,1");
            err = at_send_command(cmd, NULL);
            free(cmd);
            cmd=NULL;
            if (err < 0)
                LOGE("%s error by AT!GPSLBSSETTINGS command",__func__);

            // unlock fails on legacy devices, switch to entercnd for remaining commands
            asprintf(&cmd, "AT!ENTERCND=\"A710\"");
            err = at_send_command(cmd, &atresponse);
            free(cmd);
            cmd=NULL;
            if (err < 0 || atresponse->success == 0)
                LOGE("%s error by AT!ENTERCND command",__func__);
            at_response_free(atresponse);
            atresponse=NULL;
            at_send_command("AT!HNWANDROIDRIL=1", NULL);
        }

        /* Check if AR75xx series modem, if yes, reinit the RIL handlers.
         * Only Data connection requests are supported for now
         */
        
        if (isModemAR75()) {
            initATRILFunctionHandler();            
        }

        /* PPP is supported only for modem with two AT ports */
        property_get("persist.ril.enable_ppp_data", propValue,"0");
        g_ppp_enabled = atoi(propValue);
        if (g_ppp_enabled && (!s_AR75_Modem)) {
            g_ppp_enabled = 0;
            property_set("persist.ril.enable_ppp_data", "0"); 
        }

        #ifdef AUDIO_PROVISIONING
        /** Update audio profile parameters if modified */
        updateAudioProfile(bForceFlag);
        #endif

        LOGE("Looping the requestQueue!");
        for (;;) {
            RILRequest *r;
            RILEvent *e;
            struct timespec ts;

            memset(&ts, 0, sizeof(ts));

            pthread_mutex_lock(&q->queueMutex);

            if (q->closed != 0) {
                LOGW("AT Channel error, attempting to recover..");
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
                err = pthread_cond_timedwait(&q->cond, &q->queueMutex, &q->eventList->abstime);
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

        /* Wait a bit to give the resetting AT port a chance to come back.
           Init the detect count, allowing up to N attempts (currently 5).
           This means the AT port must come back within 10 sec.  This value
           was determined by observing the timing of many re-enumerations and
           taking a reasonable maximum value.  Note that most re-enumerations
           take less than 5sec.
        */
        sleep(2);
        restart_detect_count=5;
        
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
 * Proxy function of enter SIM PIN
 *
 * @param[in] data 
 *     Pointer to the request data
 * @param datalen 
 *     request data length
 * @param t 
 *     RIL token
 *
 * @return
 *     none
 *
 * @par Abort:<br> 
 *     none
 *
 * @note
 *     This function performs no pointer validation
 */
static void requestEnterSimPinHandle(void *data, size_t datalen, RIL_Token t)
{
    requestEnterSimPin(data, datalen, t, RIL_REQUEST_ENTER_SIM_PIN);
}

/**
 *
 * Proxy function of enter SIM PIN2
 *
 * @param[in] data 
 *     Pointer to the request data
 * @param datalen 
 *     request data length
 * @param t 
 *     RIL token
 *
 * @return
 *     none
 *
 * @par Abort:<br> 
 *     none
 *
 * @note
 *     This function performs no pointer validation
 */
static void requestEnterSimPin2(void *data, size_t datalen, RIL_Token t)
{
    requestEnterSimPin(data, datalen, t, RIL_REQUEST_ENTER_SIM_PIN2);
}

/**
 *
 * Proxy function of enter SIM PUK
 *
 * @param[in] data 
 *     Pointer to the request data
 * @param datalen 
 *     request data length
 * @param t 
 *     RIL token
 *
 * @return
 *     none
 *
 * @par Abort:<br> 
 *     none
 *
 * @note
 *     This function performs no pointer validation
 */
static void requestEnterSimPUK(void *data, size_t datalen, RIL_Token t)
{
    requestEnterSimPin(data, datalen, t, RIL_REQUEST_ENTER_SIM_PUK);
}

/**
 *
 * Proxy function of enter SIM PUK2
 *
 * @param[in] data 
 *     Pointer to the request data
 * @param datalen 
 *     request data length
 * @param t 
 *     RIL token
 *
 * @return
 *     none
 *
 * @par Abort:<br> 
 *     none
 *
 * @note
 *     This function performs no pointer validation
 */
static void requestEnterSimPUK2(void *data, size_t datalen, RIL_Token t)
{
    requestEnterSimPin(data, datalen, t, RIL_REQUEST_ENTER_SIM_PUK2);
}

/**
 *
 * Proxy function of change SIM PIN
 *
 * @param[in] data 
 *     Pointer to the request data
 * @param datalen 
 *     request data length
 * @param t 
 *     RIL token
 *
 * @return
 *     none
 *
 * @par Abort:<br> 
 *     none
 *
 * @note
 *     This function performs no pointer validation
 */
static void requestChangeSIMPIN(void *data, size_t datalen, RIL_Token t)
{
    requestChangePassword(data, datalen, t, "SC");
}

/**
 *
 * Proxy function of change SIM PIN2
 *
 * @param[in] data 
 *     Pointer to the request data
 * @param datalen 
 *     request data length
 * @param t 
 *     RIL token
 *
 * @return
 *     none
 *
 * @par Abort:<br> 
 *     none
 *
 * @note
 *     This function performs no pointer validation
 */
static void requestChangeSIMPIN2(void *data, size_t datalen, RIL_Token t)
{
    requestChangePassword(data, datalen, t, "P2");
}

/**
 *
 * Proxy function of query registration state
 *
 * @param[in] data 
 *     Pointer to the request data
 * @param datalen 
 *     request data length
 * @param t 
 *     RIL token
 *
 * @return
 *     none
 *
 * @par Abort:<br> 
 *     none
 *
 * @note
 *     This function performs no pointer validation
 */
static void requestRegistrationStateHandle(void *data, size_t datalen, RIL_Token t)
{
    requestRegistrationState(RIL_REQUEST_REGISTRATION_STATE, data, datalen, t);
}

/**
 *
 * Proxy function of query GPRS registration state
 *
 * @param[in] data 
 *     Pointer to the request data
 * @param datalen 
 *     request data length
 * @param t 
 *     RIL token
 *
 * @return
 *     none
 *
 * @par Abort:<br> 
 *     none
 *
 * @note
 *     This function performs no pointer validation
 */
static void requestGPRSRegistrationState(void *data, size_t datalen, RIL_Token t)
{
    requestRegistrationState(RIL_REQUEST_GPRS_REGISTRATION_STATE, data, datalen, t);
}

/**
 *
 * Add additional handle for RIL_REQUEST_SCREEN_STATE
 *
 * @param[in] data 
 *     Pointer to the request data
 * @param datalen 
 *     request data length
 * @param t 
 *     RIL token
 *
 * @return
 *     none
 *
 * @par Abort:<br> 
 *     none
 *
 * @note
 *     This function performs no pointer validation
 */
static void requestScreenStateAdditional(void *data, size_t datalen, RIL_Token t)
{
    requestScreenState(data, datalen, t);
    /* Trigger a rehash of network values, just to be sure. */
    if (((int *)data)[0] == 1) {
        RIL_onUnsolicitedResponse(
                           RIL_UNSOL_RESPONSE_NETWORK_STATE_CHANGED,
                           NULL, 0);
    }
}

/**
 *
 * Stub function for RIL_REQUEST_GET_CURRENT_CALLS
 *
 * @param[in] data 
 *     Pointer to the request data
 * @param datalen 
 *     request data length
 * @param t 
 *     RIL token
 *
 * @return
 *     none
 *
 * @par Abort:<br> 
 *     none
 *
 * @note
 *     This function performs no pointer validation
 */
void requestCurrentCallsStub(void *data, size_t datalen, RIL_Token t)
{
    LOGW("RIL_REQUEST_GET_CURRENT_CALLS: returning no current calls");
    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
}

/**
 *
 * Fill in the AT RIL command handler function pointer
 *
 * @param[out] rilsolreqhandlerp 
 *     Pointer to handler structure 
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
static void fillATRILFuncHandler(RIL_RequestHandlerFunc *rilsolreqhandlerp)
{
    /* Data Call Requests */
#ifdef SWI_RIL_VERSION_12
    rilsolreqhandlerp[RIL_REQUEST_SETUP_DATA_CALL] = requestSetupDefaultPDP_r12;
    rilsolreqhandlerp[RIL_REQUEST_DATA_CALL_LIST] = requestPDPContextList_r12;
#else
    rilsolreqhandlerp[RIL_REQUEST_SETUP_DATA_CALL] = requestSetupDefaultPDP;
    rilsolreqhandlerp[RIL_REQUEST_DATA_CALL_LIST] = requestPDPContextList;
#endif
    rilsolreqhandlerp[RIL_REQUEST_DEACTIVATE_DATA_CALL] = requestDeactivateDefaultPDP;
#ifndef SWI_RIL_VERSION_6
    rilsolreqhandlerp[RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE] = requestLastPDPFailCause;
#endif
    
    /* SIM Handling Requests */
    rilsolreqhandlerp[RIL_REQUEST_SIM_IO]   = requestSIM_IO;
    rilsolreqhandlerp[RIL_REQUEST_GET_SIM_STATUS]  = requestGetSimStatus;
    rilsolreqhandlerp[RIL_REQUEST_ENTER_SIM_PIN]   = requestEnterSimPinHandle;
    rilsolreqhandlerp[RIL_REQUEST_ENTER_SIM_PIN2]  = requestEnterSimPin2;
    rilsolreqhandlerp[RIL_REQUEST_ENTER_SIM_PUK]   = requestEnterSimPUK;
    rilsolreqhandlerp[RIL_REQUEST_ENTER_SIM_PUK2]  = requestEnterSimPUK2;
    rilsolreqhandlerp[RIL_REQUEST_CHANGE_SIM_PIN]  = requestChangeSIMPIN;
    rilsolreqhandlerp[RIL_REQUEST_CHANGE_SIM_PIN2] = requestChangeSIMPIN2;
    rilsolreqhandlerp[RIL_REQUEST_QUERY_FACILITY_LOCK]  = requestQueryFacilityLock;
    rilsolreqhandlerp[RIL_REQUEST_SET_FACILITY_LOCK] = requestSetFacilityLock;

    /* Network Selection */
    rilsolreqhandlerp[RIL_REQUEST_SET_BAND_MODE] = requestSetBandMode;
    rilsolreqhandlerp[RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE] = requestQueryAvailableBandMode;
    rilsolreqhandlerp[RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION] = requestEnterNetworkDepersonalization;
    rilsolreqhandlerp[RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE] = requestQueryNetworkSelectionMode;
    rilsolreqhandlerp[RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC] = requestSetNetworkSelectionAutomatic;
    rilsolreqhandlerp[RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL] = requestSetNetworkSelectionManual;
    rilsolreqhandlerp[RIL_REQUEST_QUERY_AVAILABLE_NETWORKS] = requestQueryAvailableNetworks;
    rilsolreqhandlerp[RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE] = requestSetPreferredNetworkType;
    rilsolreqhandlerp[RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE] = requestGetPreferredNetworkType;
    rilsolreqhandlerp[RIL_REQUEST_REGISTRATION_STATE] = requestRegistrationStateHandle;
    rilsolreqhandlerp[RIL_REQUEST_GPRS_REGISTRATION_STATE] = requestGPRSRegistrationState;
    rilsolreqhandlerp[RIL_REQUEST_SET_LOCATION_UPDATES] = requestSetLocationUpdates;
#if defined(SWI_RIL_VERSION_12)
    rilsolreqhandlerp[RIL_REQUEST_SIGNAL_STRENGTH] = requestSignalStrength_r12;
#elif defined(SWI_RIL_VERSION_6)
    rilsolreqhandlerp[RIL_REQUEST_SIGNAL_STRENGTH] = requestSignalStrength_r6;
#else
    rilsolreqhandlerp[RIL_REQUEST_SIGNAL_STRENGTH] = requestSignalStrength;
#endif
    rilsolreqhandlerp[RIL_REQUEST_OPERATOR] = requestOperator;
    /* SWI_TBD it might be a bug of function responseCellList() in ril.cpp. 
     * The definition of RIL_NeighboringCell in ril.h is cid first rssi as following .
     * Need to investigate how this command is used by the application layers, 
     * since there is no way to indicate in the response whether this is 2G based or 3G based information.
     */
    rilsolreqhandlerp[RIL_REQUEST_GET_NEIGHBORING_CELL_IDS] = requestGetNeighboringCellIDS;

    /* USSD Requests */
    rilsolreqhandlerp[RIL_REQUEST_SEND_USSD] = requestSendUSSD;
    rilsolreqhandlerp[RIL_REQUEST_CANCEL_USSD] = requestCancelUSSD;

    /* Misc */
    rilsolreqhandlerp[RIL_REQUEST_RADIO_POWER] = requestRadioPower;
    rilsolreqhandlerp[RIL_REQUEST_GET_IMSI] = requestGetIMSI;
    rilsolreqhandlerp[RIL_REQUEST_GET_IMEI] = requestGetIMEI;       /* Deprecated */
    rilsolreqhandlerp[RIL_REQUEST_GET_IMEISV] = requestGetIMEISV;   /* Deprecated */
    rilsolreqhandlerp[RIL_REQUEST_DEVICE_IDENTITY] = requestDeviceIdentity;
    rilsolreqhandlerp[RIL_REQUEST_BASEBAND_VERSION] = requestBasebandVersion;
    rilsolreqhandlerp[RIL_REQUEST_OEM_HOOK_STRINGS] = requestOEMHookStrings;
    rilsolreqhandlerp[RIL_REQUEST_SET_SUPP_SVC_NOTIFICATION] = requestSetSuppSvcNotification;
    rilsolreqhandlerp[RIL_REQUEST_SCREEN_STATE] = requestScreenStateAdditional;

    /* Short Message Service */
    rilsolreqhandlerp[RIL_REQUEST_SEND_SMS] = requestSendSMS;
    rilsolreqhandlerp[RIL_REQUEST_SMS_ACKNOWLEDGE] = requestSMSAcknowledge;
    rilsolreqhandlerp[RIL_REQUEST_WRITE_SMS_TO_SIM] = requestWriteSmsToSim;
    rilsolreqhandlerp[RIL_REQUEST_DELETE_SMS_ON_SIM] = requestDeleteSmsOnSim;
    rilsolreqhandlerp[RIL_REQUEST_GET_SMSC_ADDRESS] = requestGetSMSCAddress;
    rilsolreqhandlerp[RIL_REQUEST_SET_SMSC_ADDRESS] = requestSetSMSCAddress;
#ifdef   SWI_RIL_VERSION_10
    rilsolreqhandlerp[RIL_REQUEST_SEND_SMS_EXPECT_MORE] = requestSendSMS;
    rilsolreqhandlerp[RIL_REQUEST_SHUTDOWN] = requestShutdown;
#endif 

    /* voice call related */
    if ((isVoiceEnabled ()) && (!s_AR75_Modem)) {
        /* Basic Voice Call Requests */
        rilsolreqhandlerp[RIL_REQUEST_LAST_CALL_FAIL_CAUSE] = requestLastCallFailCause;
        rilsolreqhandlerp[RIL_REQUEST_GET_CURRENT_CALLS] = requestGetCurrentCalls;
        rilsolreqhandlerp[RIL_REQUEST_DIAL] = requestDial;
        rilsolreqhandlerp[RIL_REQUEST_HANGUP] = requestHangup;
        rilsolreqhandlerp[RIL_REQUEST_ANSWER] = requestAnswer;
        rilsolreqhandlerp[RIL_REQUEST_GET_MUTE] = requestGetMute;
        rilsolreqhandlerp[RIL_REQUEST_SET_MUTE] = requestSetMute;

        /* Advanced Voice Call Requests */
        rilsolreqhandlerp[RIL_REQUEST_GET_CLIR] = requestGetCLIR;
        rilsolreqhandlerp[RIL_REQUEST_SET_CLIR] = requestSetCLIR;
        rilsolreqhandlerp[RIL_REQUEST_QUERY_CALL_FORWARD_STATUS] = requestQueryCallForwardStatus;
        rilsolreqhandlerp[RIL_REQUEST_SET_CALL_FORWARD] = requestSetCallForward;
        rilsolreqhandlerp[RIL_REQUEST_QUERY_CALL_WAITING] = requestQueryCallWaiting;
        rilsolreqhandlerp[RIL_REQUEST_SET_CALL_WAITING] = requestSetCallWaiting;
        rilsolreqhandlerp[RIL_REQUEST_UDUB] = requestUDUB;
        rilsolreqhandlerp[RIL_REQUEST_QUERY_CLIP] = requestQueryClip;
        
        /* Call Waiting/Conference */
        rilsolreqhandlerp[RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND] = requestHangupWaitingOrBackground;
        rilsolreqhandlerp[RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND] = requestHangupForegroundResumeBackground;
        rilsolreqhandlerp[RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE] = requestSwitchWaitingOrHoldingAndActive;
        rilsolreqhandlerp[RIL_REQUEST_CONFERENCE] = requestConference;
        rilsolreqhandlerp[RIL_REQUEST_SEPARATE_CONNECTION] = requestSeparateConnection;
        rilsolreqhandlerp[RIL_REQUEST_EXPLICIT_CALL_TRANSFER] = requestExplicitCallTransfer;

        /* DTMF Requests */
	rilsolreqhandlerp[RIL_REQUEST_DTMF_START] = requestStartDTMF;
	rilsolreqhandlerp[RIL_REQUEST_DTMF_STOP] = requestStopDTMF;    

}
    else {
        /* RIL_REQUEST_GET_CURRENT_CALLS has to be supported */
        rilsolreqhandlerp[RIL_REQUEST_GET_CURRENT_CALLS] = requestCurrentCallsStub;
    }
    /* Change barring password */
    rilsolreqhandlerp[RIL_REQUEST_CHANGE_BARRING_PASSWORD] = requestChangeBarringPassword;
}

/**
 *
 * Initialize the AT RIL command handler
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
static void initATRILFunctionHandler(void)
{
    int i;
    
    /* initialize all handler pointer to NULL */
    for (i=0; i<RIL_SOL_NUMBER_MAX; i++) {
        s_atrilsolreqhandler[i] = NULL;
    }

    fillATRILFuncHandler(&s_atrilsolreqhandler[0]);
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
    char isdevicesocket = 0;
    struct queueArgs *queueArgs;
    struct queueArgs *prioQueueArgs;
    pthread_attr_t attr;
    char autodetect = 0;

    s_rilenv = env;

    LOGI("Entering RIL_Init..");

    while (-1 != (opt = getopt(argc, argv, "z:i:p:d:s:x:c:a"))) {
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

    /* initialize function pointers */
    initATRILFunctionHandler();
    
    queueArgs = malloc(sizeof(struct queueArgs));
    memset(queueArgs, 0, sizeof(struct queueArgs));

    queueArgs->device_path = device_path;
    queueArgs->port = port;
    queueArgs->loophost = loophost;
    queueArgs->isdevicesocket = isdevicesocket;
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
        queueArgs->hasPrio = 1;

        s_requestQueuePrio.enabled = 1;

        pthread_create(&s_tid_queueRunnerPrio, &attr, queueRunner, prioQueueArgs);
    }

    pthread_create(&s_tid_queueRunner, &attr, queueRunner, queueArgs);

    /* Create the swiril_gps thread */
    RILGPS_Init(GPS_RILTYPE_AT);

    return &s_callbacks;
}

/**
 * checkForEndofUSSD
 * check for start and end quotes are received and
 * returns true if start and end quotes are received
 * otherwise returns false
 *
 * @param[in] data
 *     Pointer to data to check start and end quotes
 *
 * @return bool
 *     returns true if start and end quotes are received
 *     otherwise returns false *
 */
static bool checkForEndofUSSD(const char* data)
{
    static bool FirstQuotes = FALSE;
    char* ptr = NULL;

    if ( TRUE == FirstQuotes)
    {
        ptr = strchr(data, '\"');
        if ( NULL != ptr )
        {
            FirstQuotes = FALSE;
            return TRUE;
        }
        return FALSE;
    }

    ptr = strchr(data, '\"');
    if ( NULL != ptr )
    {
        FirstQuotes = TRUE;
        ptr = strchr (ptr+1,'\"');
        if ( NULL != ptr )
        {
            return TRUE;
        }
        return FALSE;
    }
    LOGD("%s:: Should not come here. some problem...\n", __func__);
    return TRUE;
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
static int readFromMODEM_NV(void)
{     
    ATResponse *atresponse = NULL;
    ATLine *atlinep = NULL;
    int err = 0;
    char* line;

    LOGD("%s : Entered",__func__); 

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
    char *cmd;
    ATLine *atlinep = NULL;
    int err = 0;
    char* line;
    int eccCountVal = 0;
    int eccCount; 
    int countinterchange = 0;
    int counttemp = 0; 
    int tempchar = 0;  

    LOGD("%s : Entered",__func__);

    eccCountVal = ECC_LIST_SIM_READ_START_PTR;
    eccCount = ECC_LIST_SIM_READ_START_MAX_LIST;
      
    while(true) {
        asprintf(&cmd,"AT+CRSM=176,28599,0,0,%d",eccCount);
        eccCount-=3;       
        /* Read SIM Card ECC List*/ 
        err = at_send_command_multivalue(cmd,"+CRSM:", &atresponse);
        free(cmd);
        /* Read the next inlist */
        atlinep=atresponse->p_intermediates;
        /* Check for error response */
        if (err < 0 || atresponse->success == 0) {
            at_response_free(atresponse);        
            LOGE("ERROR in response of command -> AT+CRSM");
            return -1;
        }
        else {
            line=atlinep->line;
            /** Check for response not to be null */
            if (line !=NULL){           
                if((strstr(line, "144") != NULL) || (strstr(line,"108") != NULL)){
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
int ecclistReadModemNVSet(void)
{
    char propValue[PROPERTY_VALUE_MAX];
    int  returnValue = 0; 
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
    /* Read the Ecc list from persistent ril.ecclist */
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
    bool b3GSIM = false;

    LOGD("%s: Entered",__func__);

    /* set property */
    property_set(ECCLIST_PROPERTY, "911,112");
    
    /* read modem NV */
    ecclistReadModemNVSet();
    numdigits = 0;
    listdecodeptr = NULL;
    memset(extractedNums,0,sizeof(extractedNums));

    /* Figure out by GET RESPONSE command if SIM is 2G or 3G */
    returnValue = readSimEccFileStatus(&b3GSIM);
    if (returnValue == 0) {
        if (b3GSIM)
            returnValue = readFromSIMCARDRecord();
        else
            returnValue = readFromSIMCARD();
    }
    else {
         LOGE("%s: Could not read from SIM card ECC attributes",__func__);
    }
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
#endif

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
#endif

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
#endif // HW_TI_OMAP
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
    
    at_response_free(atresponse);
    asprintf(&cmd, "AT!ENTERCND=\"A710\"");
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
            prop=NULL;
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
    prop=NULL;
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

        /* Workaround for SL8092 to save volume level in modem */
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
    prop=NULL;

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

            /* Workaround for SL8092 to save volume level in modem */
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
    prop=NULL;

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
    prop=NULL;

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
                prop=NULL;
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

