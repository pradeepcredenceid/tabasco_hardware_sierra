/* 
** This source code is "Not a Contribution" under Apache license
**
** Sierra Wireless RIL
**
** Based on reference-ril by The Android Open Source Project
** and U300 RIL by ST-Ericsson.
** Modified by Sierra Wireless, Inc.
**
** Copyright (C) 2011 Sierra Wireless, Inc.
** Copyright (C) ST-Ericsson AB 2008-2009
** Copyright 2006, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** Based on reference-ril by The Android Open Source Project.
**
** Heavily modified for ST-Ericsson U300 modems.
** Author: Christian Bejram <christian.bejram@stericsson.com>
*/

#include <telephony/ril.h>
#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <cutils/properties.h>

#include "at_channel.h"
#include "at_tok.h"
#include "fcp_parser.h"
#include "at_misc.h"
#include "swiril_main.h"
#include "swiril_sim.h"
#include "swiril_misc.h"
#include "SWIWWANCMAPI.h"

#define LOG_TAG "RIL"
#include "swiril_log.h"

extern unsigned long GetFirmwareRevision(unsigned char stringSize, char *pString );
extern unsigned long GetModelID(unsigned char stringSize, char *pString);
extern unsigned long GetVoiceNumber(unsigned char voiceNumberSize,char *pVoiceNumber,unsigned char minSize, char *pMIN);
extern void setCLIRVal(int clirVal);
extern bool isModemNetworkRegistered();
extern bool getFirmwareInfoCarrierQMI(enum eGobiImageCarrier *image);

typedef enum {
    SIM_ABSENT = 0,
    SIM_NOT_READY = 1,
    SIM_READY = 2,    /* SIM_READY means radio state = RADIO_STATE_SIM_READY */
    SIM_PIN = 3,
    SIM_PUK = 4,
    SIM_NETWORK_PERSONALIZATION = 5,
    SIM_FAILURE = 6
} SIM_Status; 

#define SIM_POLL_RETRY 10 
static const struct timeval TIMEVAL_SIMPOLL = { 1, 0 };
static const struct timeval TIMEVAL_SIMRESET = { 60, 0 };

typedef enum {
    SIM_PIN_TYPE,
    SIM_PUK_TYPE,
    SIM_PIN2_TYPE,
    SIM_PUK2_TYPE
} SIM_PinPukType; 

typedef struct {
    int simPIN;
    int simPUK;
    int simPIN2;
    int simPUK2;
}PINRetryCount_s;

extern void setPowerOff(void);
bool s_AR75_Modem = false;

#define PROPERTY_VERSION_BASEBAND "gsm.version.baseband"

/* Workaround for SL9090 CRSM failure BZ48549 */ 
#define BASEBAND_SL9090_UMTS "SWI6600U"
#define BASEBAND_MC73xx_VOICE "SWI9X15C_06"
#define BASEBAND_SL9090_03 "SWI6600C"
#define BASEBAND_SL9090_GENERIC_CDMA "SWI6600H"
#define FAKE_GET_RESPONSE_MSISDN_SL9090 "0000008C6F40040011F0EE0102011C"
#define FAKE_ICCID_QUERY_RESPONSE "0000000A2FE204000FF0EE01020000"
#define FAKE_ICCID_RESPONSE "98198800002035584433"

/* as per 3GPP TS11.11 EF (MSISDN) format for CRSM response
- First 28 chars in hex string (14 bytes) bytes are Tag identifier - Fix
- next 2 char (1 byte) tells BCD length of MSISDN and TON/NPI field - Modify
- next 2 char (1 byte) tells TON and NPI- Fix
- next 20 char (10 bytes) MSISDN number - Modify
- other info in remaining 4 char - Fix  */
 
static char msisdn_crsm_response [57] = 
        {/* alpha tag */'8','0', '2','0','0','0','2','C','7','2','0','0','0','2','F','F',
        'F','F','F','F','F','F','F','F','F','F','F','F',/* BCD length */'F','F',/*TON/NPI */'8','1',
        /* MSISDN */'F','F','F','F','F','F','F','F','F','F','F','F','F','F','F','F','F','F','F','F',
        /* other info */'F','F','F','F', '\0'};                                       

/**
 * Get the corresponsing PIN or PUK type from the incoming request
 *
 * @param [in] request
 *      the following are the requests which are supported
 *          RIL_REQUEST_ENTER_SIM_PIN
 *          RIL_REQUEST_ENTER_SIM_PUK
 *          RIL_REQUEST_ENTER_SIM_PIN2
 *          RIL_REQUEST_ENTER_SIM_PUK2
 *
 * @return
 *     enumeration SIM_PinPukType is returned
 *
 * @note
 *     none
 */
static SIM_PinPukType getSIMPinPukType( int request )
{
    SIM_PinPukType simPinPukType = SIM_PIN_TYPE;

    switch(request) {
        case RIL_REQUEST_ENTER_SIM_PIN:
            simPinPukType = SIM_PIN_TYPE;
            break;
        case RIL_REQUEST_ENTER_SIM_PUK:
            simPinPukType = SIM_PUK_TYPE;
            break;
        case RIL_REQUEST_ENTER_SIM_PIN2:
            simPinPukType = SIM_PIN2_TYPE;
            break;
        case RIL_REQUEST_ENTER_SIM_PUK2:
            simPinPukType = SIM_PUK2_TYPE;
            break;
    }
    return simPinPukType;
}

/**
 * Get the PIN retry count for the associated PIN PUK type requested
 *
 * @param [in] simPinPukType
 *      the associated PIN retry count 
 *
 * @return
 *     no of retries left for the PIN or PUK type requested, -1 for unknown
 *
 * @note
 *     none
 */
static int getPINRetryCount(SIM_PinPukType simPinPukType)
{
    ATResponse *atresponse = NULL;
    int err;
    char *cpincLine;
    int num_retries = -1;
    PINRetryCount_s retryCount;

    /* Set to default values : retry count is defaulted to -1[unknown] */
    retryCount.simPIN  = -1;
    retryCount.simPUK  = -1;
    retryCount.simPIN2 = -1;
    retryCount.simPUK2 = -1;

    #ifdef SWI_HL_RIL
    err = at_send_command_singleline("AT+XPINCNT", "+XPINCNT:", &atresponse);
    #else
    err = at_send_command_singleline("AT+CPINC?", "+CPINC:", &atresponse);
    #endif
    if ( err < 0 || atresponse->success ==0 ) {
        goto done;
    }

    cpincLine = atresponse->p_intermediates->line;
    err = at_tok_start(&cpincLine);
    if (err < 0) {
        goto done;
    }
    err = at_tok_nextint(&cpincLine, &retryCount.simPIN);
    if (err < 0) {
        goto done;
    }
    err = at_tok_nextint(&cpincLine, &retryCount.simPIN2);
    if (err < 0) {
        goto done;
    }
    err = at_tok_nextint(&cpincLine, &retryCount.simPUK);
    if (err < 0) {
        goto done;
    }
    at_tok_nextint(&cpincLine, &retryCount.simPUK2);

done:
    at_response_free(atresponse);
    switch(simPinPukType) {
        case SIM_PIN_TYPE:
            num_retries = retryCount.simPIN;
            break;
        case SIM_PIN2_TYPE:
            num_retries = retryCount.simPIN2;
            break;
        case SIM_PUK_TYPE:
            num_retries = retryCount.simPUK;
            break;
        case SIM_PUK2_TYPE:
            num_retries = retryCount.simPUK2;
            break;
    }
    return num_retries;
}

/**
 *
 * Query the SIM status
 *
 * @param [out] substate
 *     personalization substat
 *
 * @return
 *     One of enum SIM_Status
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     Returns one of SIM_*. Returns SIM_NOT_READY on error.
 *
 */
static SIM_Status getSIMStatus(RIL_PersoSubstate *substate)
{
    ATResponse *atresponse = NULL;
    int err;
    int ret = SIM_READY;
    char *cpinLine;
    char *cpinResult;

    *substate = RIL_PERSOSUBSTATE_UNKNOWN;
    if (currentState() == RADIO_STATE_OFF ||
        currentState() == RADIO_STATE_UNAVAILABLE) {
        ret = SIM_NOT_READY;
        goto done;
    }

    err = at_send_command_singleline("AT+CPIN?", "+CPIN:", &atresponse);

    if (err != 0) {
        ret = SIM_NOT_READY;
        goto done;
    }

    switch (at_get_cme_error(atresponse)) {
        case CME_SUCCESS:
            break;

        case CME_SIM_NOT_INSERTED:
            ret = SIM_ABSENT;
            goto done;

        case CME_SIM_FAILURE:
            ret = SIM_FAILURE;
            goto done;

        default:
            ret = SIM_NOT_READY;
            goto done;
    }

    /* CPIN? has succeeded, now look at the result. */

    cpinLine = atresponse->p_intermediates->line;
    err = at_tok_start(&cpinLine);

    if (err < 0) {
        ret = SIM_NOT_READY;
        goto done;
    }

    err = at_tok_nextstr(&cpinLine, &cpinResult);

    if (err < 0) {
        ret = SIM_NOT_READY;
        goto done;
    }
    if (0 == strcmp(cpinResult, "SIM PIN")) {
        ret = SIM_PIN;
        goto done;
    } else if (0 == strcmp(cpinResult, "SIM PUK")) {
        ret = SIM_PUK;
        goto done;
    } else if (0 == strcmp(cpinResult, "PH-NET PIN")) {
        *substate = RIL_PERSOSUBSTATE_SIM_NETWORK;
        return SIM_NETWORK_PERSONALIZATION;
    } else if (0 == strcmp(cpinResult, "PH-NETSUB PIN")) {
        *substate = RIL_PERSOSUBSTATE_SIM_NETWORK_SUBSET;
        return SIM_NETWORK_PERSONALIZATION;
    } else if (0 == strcmp(cpinResult, "PH-SP PIN")) {
        *substate = RIL_PERSOSUBSTATE_SIM_SERVICE_PROVIDER;
        return SIM_NETWORK_PERSONALIZATION;
    } else if (0 == strcmp(cpinResult, "PH-CORP PIN")) {
        *substate = RIL_PERSOSUBSTATE_SIM_CORPORATE;
        return SIM_NETWORK_PERSONALIZATION;
    } else if (0 != strcmp(cpinResult, "READY")) {
        /* We're treating unsupported lock types as "sim absent". */
        ret = SIM_ABSENT;
        goto done;
    }

done:
    at_response_free(atresponse);
    return ret;
}

/**
 * Get the current card status.
 *
 * This must be freed using freeCardStatus.
 * @return: On success returns RIL_E_SUCCESS.
 */
#ifndef SWI_RIL_VERSION_6
static int getCardStatus(RIL_CardStatus **pp_card_status) {
    static RIL_AppStatus app_status_array[] = {
        /* SIM_ABSENT = 0 */
        { RIL_APPTYPE_UNKNOWN, RIL_APPSTATE_UNKNOWN, RIL_PERSOSUBSTATE_UNKNOWN,
          NULL, NULL, 0, RIL_PINSTATE_UNKNOWN, RIL_PINSTATE_UNKNOWN },
        /* SIM_NOT_READY = 1 */
        { RIL_APPTYPE_SIM, RIL_APPSTATE_DETECTED, RIL_PERSOSUBSTATE_UNKNOWN,
          NULL, NULL, 0, RIL_PINSTATE_UNKNOWN, RIL_PINSTATE_UNKNOWN },
        /* SIM_READY = 2 */
        { RIL_APPTYPE_SIM, RIL_APPSTATE_READY, RIL_PERSOSUBSTATE_READY,
          NULL, NULL, 0, RIL_PINSTATE_UNKNOWN, RIL_PINSTATE_UNKNOWN },
        /* SIM_PIN = 3 */
        { RIL_APPTYPE_SIM, RIL_APPSTATE_PIN, RIL_PERSOSUBSTATE_UNKNOWN,
          NULL, NULL, 0, RIL_PINSTATE_ENABLED_NOT_VERIFIED, RIL_PINSTATE_UNKNOWN },
        /* SIM_PUK = 4 */
        { RIL_APPTYPE_SIM, RIL_APPSTATE_PUK, RIL_PERSOSUBSTATE_UNKNOWN,
          NULL, NULL, 0, RIL_PINSTATE_ENABLED_BLOCKED, RIL_PINSTATE_UNKNOWN },
        /* SIM_NETWORK_PERSONALIZATION = 5 */
        { RIL_APPTYPE_SIM, RIL_APPSTATE_SUBSCRIPTION_PERSO, RIL_PERSOSUBSTATE_SIM_NETWORK,
          NULL, NULL, 0, RIL_PINSTATE_ENABLED_NOT_VERIFIED, RIL_PINSTATE_UNKNOWN }
    };
    RIL_CardState card_state;
    int num_apps;
    RIL_PersoSubstate substate;
    unsigned long nRet;
    char fmrString[0xff];
    enum eGobiImageCarrier image;

    SIM_Status sim_status = getSIMStatus(&substate);
    if ((sim_status == SIM_ABSENT) || (sim_status == SIM_FAILURE)) {
        card_state = RIL_CARDSTATE_ABSENT;
        num_apps = 0;
    } else {
        card_state = RIL_CARDSTATE_PRESENT;
        num_apps = 1;
    }

    /* Allocate and initialize base card status. */
    RIL_CardStatus *p_card_status = malloc(sizeof(RIL_CardStatus));
    p_card_status->card_state = card_state;
    p_card_status->universal_pin_state = RIL_PINSTATE_UNKNOWN;
    p_card_status->gsm_umts_subscription_app_index = RIL_CARD_MAX_APPS;
    p_card_status->cdma_subscription_app_index = RIL_CARD_MAX_APPS;
    p_card_status->num_applications = num_apps;

    /* Initialize application status. */
    int i;
    for (i = 0; i < RIL_CARD_MAX_APPS; i++) {
        p_card_status->applications[i] = app_status_array[SIM_ABSENT];
    }

    /* Pickup the appropriate application status
       that reflects sim_status for gsm. */
    if (num_apps != 0) {
        /* Only support one app, gsm. */
        p_card_status->num_applications = 1;
        p_card_status->gsm_umts_subscription_app_index = 0;

        /* Get the correct app status. */
        p_card_status->applications[0] = app_status_array[sim_status];
        /* update the substate of personalization */
        if (p_card_status->applications[0].app_state == RIL_APPSTATE_SUBSCRIPTION_PERSO) {
            p_card_status->applications[0].perso_substate = substate;
        }
        /* MC7750-like module uses CDMA as default */
        if (isDualModeModule()) {
            if ((isModemNetworkRegistered()) && (!isDualModeRunningLTE())) {
                LOGD("getCardStatus: CSIM");
                p_card_status->applications[0].app_type = RIL_APPTYPE_CSIM;
                p_card_status->cdma_subscription_app_index = 0;
            }
        }
        /* China telcom modem firmware or generic CDMA modem firmware
           with  Chinatelcom/genericCDMA carrier image */
        nRet = GetFirmwareRevision(sizeof(fmrString), &fmrString[0]);
        if (nRet == 0) {
            if ((0 != strstr(fmrString,BASEBAND_SL9090_03))||
               ((0 != strstr(fmrString,BASEBAND_SL9090_GENERIC_CDMA)) &&
               ((getFirmwareInfoCarrierQMI(&image)) && 
               ((image == eGOBI_IMG_CAR_CHINA_TELECOM) || 
               (image == eGOBI_IMG_CAR_GENERIC_CDMA))))) {
                LOGD("getCardStatusRIL: CSIM");
                p_card_status->applications[0].app_type = RIL_APPTYPE_CSIM;
                p_card_status->cdma_subscription_app_index = 0;
            }
        }
    }

    *pp_card_status = p_card_status;
    return RIL_E_SUCCESS;
}
#else
static int getCardStatus(RIL_CardStatus_v6 **pp_card_status) {
    static RIL_AppStatus app_status_array[] = {
        /* SIM_ABSENT = 0 */
        { RIL_APPTYPE_UNKNOWN, RIL_APPSTATE_UNKNOWN, RIL_PERSOSUBSTATE_UNKNOWN,
          NULL, NULL, 0, RIL_PINSTATE_UNKNOWN, RIL_PINSTATE_UNKNOWN },
        /* SIM_NOT_READY = 1 */
        { RIL_APPTYPE_SIM, RIL_APPSTATE_DETECTED, RIL_PERSOSUBSTATE_UNKNOWN,
          NULL, NULL, 0, RIL_PINSTATE_UNKNOWN, RIL_PINSTATE_UNKNOWN },
        /* SIM_READY = 2 */
        { RIL_APPTYPE_SIM, RIL_APPSTATE_READY, RIL_PERSOSUBSTATE_READY,
          NULL, NULL, 0, RIL_PINSTATE_UNKNOWN, RIL_PINSTATE_UNKNOWN },
        /* SIM_PIN = 3 */
        { RIL_APPTYPE_SIM, RIL_APPSTATE_PIN, RIL_PERSOSUBSTATE_UNKNOWN,
          NULL, NULL, 0, RIL_PINSTATE_ENABLED_NOT_VERIFIED, RIL_PINSTATE_UNKNOWN },
        /* SIM_PUK = 4 */
        { RIL_APPTYPE_SIM, RIL_APPSTATE_PUK, RIL_PERSOSUBSTATE_UNKNOWN,
          NULL, NULL, 0, RIL_PINSTATE_ENABLED_BLOCKED, RIL_PINSTATE_UNKNOWN },
        /* SIM_NETWORK_PERSONALIZATION = 5 */
        { RIL_APPTYPE_SIM, RIL_APPSTATE_SUBSCRIPTION_PERSO, RIL_PERSOSUBSTATE_SIM_NETWORK,
          NULL, NULL, 0, RIL_PINSTATE_ENABLED_NOT_VERIFIED, RIL_PINSTATE_UNKNOWN }
    };
    RIL_CardState card_state;
    int num_apps;
    RIL_PersoSubstate substate;
    unsigned long nRet;
    char fmrString[0xff];
    enum eGobiImageCarrier image;

    SIM_Status sim_status = getSIMStatus(&substate);
    if ((sim_status == SIM_ABSENT) || (sim_status == SIM_FAILURE)) {
        card_state = RIL_CARDSTATE_ABSENT;
        num_apps = 0;
    } else {
        card_state = RIL_CARDSTATE_PRESENT;
        num_apps = 1;
    }

    /* Allocate and initialize base card status. */
    RIL_CardStatus_v6 *p_card_status = malloc(sizeof(RIL_CardStatus_v6));
    p_card_status->card_state = card_state;
    p_card_status->universal_pin_state = RIL_PINSTATE_UNKNOWN;
    p_card_status->gsm_umts_subscription_app_index = RIL_CARD_MAX_APPS;
    p_card_status->cdma_subscription_app_index = RIL_CARD_MAX_APPS;
    p_card_status->ims_subscription_app_index = RIL_CARD_MAX_APPS;
    p_card_status->num_applications = num_apps;

    /* Initialize application status. */
    int i;
    for (i = 0; i < RIL_CARD_MAX_APPS; i++) {
        p_card_status->applications[i] = app_status_array[SIM_ABSENT];
    }

    /* Pickup the appropriate application status
       that reflects sim_status for gsm. */
    if (num_apps != 0) {
        /* Only support one app, gsm. */
        p_card_status->num_applications = 1;
        p_card_status->gsm_umts_subscription_app_index = 0;

        /* Get the correct app status. */
        p_card_status->applications[0] = app_status_array[sim_status];
        /* update the substate of personalization */
        if (p_card_status->applications[0].app_state == RIL_APPSTATE_SUBSCRIPTION_PERSO) {
            p_card_status->applications[0].perso_substate = substate;
        }
        /* MC7750-like module uses CDMA as default */
        if (isDualModeModule()) {
            if ((isModemNetworkRegistered()) && (!isDualModeRunningLTE())) {

                /*Android 5.0 or above*/
                #ifdef SWI_RIL_VERSION_10
                if(Is9x15Device() || Is9x30Device())
                {
                    LOGD("getCardStatus: SIM");
                    p_card_status->applications[0].app_type = RIL_APPTYPE_SIM;
                }
                else
                #endif
                {
                    LOGD("getCardStatus: CSIM");
                    p_card_status->applications[0].app_type = RIL_APPTYPE_CSIM;
                }
                p_card_status->cdma_subscription_app_index = 0;
            }
        }

        /* China telcom modem firmware or generic CDMA modem firmware
           with  Chinatelcom/genericCDMA carrier image */
        nRet = GetFirmwareRevision(sizeof(fmrString), &fmrString[0]);
        if (nRet == 0) {
            if ((0 != strstr(fmrString,BASEBAND_SL9090_03)) ||
               ((0 != strstr(fmrString,BASEBAND_SL9090_GENERIC_CDMA)) &&
               ((getFirmwareInfoCarrierQMI(&image)) && 
               ((image == eGOBI_IMG_CAR_CHINA_TELECOM) || 
               (image == eGOBI_IMG_CAR_GENERIC_CDMA))))) {
                LOGD("getCardStatusRIL: CSIM");
                p_card_status->applications[0].app_type = RIL_APPTYPE_CSIM;
                p_card_status->cdma_subscription_app_index = 0;
            }
        }
    }
    *pp_card_status = p_card_status;
    return RIL_E_SUCCESS;
}
#endif
/**
 * Free the card status returned by getCardStatus.
 */
#ifndef SWI_RIL_VERSION_6
static void freeCardStatus(RIL_CardStatus *p_card_status) {
#else
static void freeCardStatus(RIL_CardStatus_v6 *p_card_status) {
#endif
    free(p_card_status);
}

static int poll_retry_count = SIM_POLL_RETRY;

void resetpollSIMRetry(void)
{
	poll_retry_count = SIM_POLL_RETRY;
}


/**
 * SIM ready means any commands that access the SIM will work, including:
 *  AT+CPIN, AT+CSMS, AT+CNMI, AT+CRSM
 *  (all SMS-related commands).
 */
void pollSIMState(void *param)
{
    RIL_PersoSubstate substate;
    char propValue[PROPERTY_VALUE_MAX];
    int intpropvalue=0;
    struct timeval timeval_simpoll = { 0, 0 };
    unsigned long nRet;
    char fmrString[0xff];
    enum eGobiImageCarrier image;
   
    if (((long) param) != 1 &&
        currentState() != RADIO_STATE_SIM_NOT_READY &&
        currentState() != RADIO_STATE_SIM_LOCKED_OR_ABSENT &&
        currentState() != RADIO_STATE_RUIM_NOT_READY &&
        currentState() != RADIO_STATE_RUIM_LOCKED_OR_ABSENT &&
        currentState() != RADIO_STATE_NV_READY) {
        /* No longer valid to poll. */
        return;
    }
    nRet = GetFirmwareRevision(sizeof(fmrString), &fmrString[0]);
    switch (getSIMStatus(&substate)) {
        case SIM_ABSENT:      
        case SIM_FAILURE:
            if (property_get("persist.sierra.sim_poll_delay", propValue, NULL) > 0) {
            intpropvalue = atoi(propValue);
            LOGI("%s: poll pindelay=%i retry = %i", __func__, intpropvalue,poll_retry_count);
            timeval_simpoll.tv_sec = intpropvalue;
            if(intpropvalue > 0 && poll_retry_count-- > 0 )
                enqueueRILEvent(RIL_EVENT_QUEUE_PRIO, pollSIMState, NULL,&timeval_simpoll);
            else if (isFirmwareRunningCDMA())
                setRadioState(RADIO_STATE_RUIM_LOCKED_OR_ABSENT);
            else
                setRadioState(RADIO_STATE_SIM_LOCKED_OR_ABSENT);
            }
            else if (isFirmwareRunningCDMA()) 
                setRadioState(RADIO_STATE_RUIM_LOCKED_OR_ABSENT);
            else
                setRadioState(RADIO_STATE_SIM_LOCKED_OR_ABSENT);

            break;
            
        case SIM_PIN:
        case SIM_PUK:
        case SIM_NETWORK_PERSONALIZATION:

        default:
            /* MC7750-like module uses CDMA as default */
            if (isDualModeModule() && (!isDualModeRunningLTE())) {
                LOGD("%s: set RADIO_STATE_SIM_LOCKED_OR_ABSENT", __func__);
                setRadioState(RADIO_STATE_SIM_LOCKED_OR_ABSENT);
            }
            else if (isFirmwareRunningCDMA()) {
                setRadioState(RADIO_STATE_RUIM_LOCKED_OR_ABSENT);
            }     
            else {
                setRadioState(RADIO_STATE_SIM_LOCKED_OR_ABSENT);
            }
            return;
    
        case SIM_NOT_READY:
            enqueueRILEvent(RIL_EVENT_QUEUE_PRIO, pollSIMState, NULL,
                            &TIMEVAL_SIMPOLL);
            return;
    
        case SIM_READY:
            if ( ! useDelayedSIMReady() ) {
                /* MC7750-like module uses CDMA as default */
                if (isDualModeModule() && (!isDualModeRunningLTE())) {
                    LOGD("%s: set RADIO_STATE_SIM_READY", __func__);
                    setRadioState(RADIO_STATE_SIM_READY);
                }
                /* China telcom modem firmware or generic CDMA modem firmware
                   with  Chinatelcom/genericCDMA carrier image */
                else if ((nRet == 0) && 
                        ((0 != strstr(fmrString,BASEBAND_SL9090_03)) ||
                        ((0 != strstr(fmrString,BASEBAND_SL9090_GENERIC_CDMA)) &&
                        ((getFirmwareInfoCarrierQMI(&image)) && 
                        ((image == eGOBI_IMG_CAR_CHINA_TELECOM) || 
                        (image == eGOBI_IMG_CAR_GENERIC_CDMA)))))) {
                    setRadioState(RADIO_STATE_RUIM_READY);
                }
                else if (isFirmwareRunningCDMA()) {
                    setRadioState(RADIO_STATE_RUIM_READY);
                } 
                else {
                    setRadioState(RADIO_STATE_SIM_READY);
                }
            }
            return;
    }
}

/** 
 * RIL_REQUEST_GET_SIM_STATUS
 *
 * Requests status of the SIM interface and the SIM card.
 * 
 * Valid errors:
 *  Must never fail.
 */
void requestGetSimStatus(void *data, size_t datalen, RIL_Token t)
{
#ifndef SWI_RIL_VERSION_6
    RIL_CardStatus* p_card_status = NULL;
#else
    RIL_CardStatus_v6* p_card_status = NULL;
#endif

    if (getCardStatus(&p_card_status) != RIL_E_SUCCESS)
        goto error;
    
    RIL_onRequestComplete(t, RIL_E_SUCCESS, (char*)p_card_status, sizeof(*p_card_status));

finally:
    if (p_card_status != NULL) {
        freeCardStatus(p_card_status);
    }
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * convertSimIoFcp
 *
 * Convert the incoming SIM response to the "GET RESPONSE"
 * command into a 2G FCP image, if applicable. The upper
 * layers of the Android stack, notably in a file called
 * IccFileHandler.java, expect the SIM File Header info
 * to be packed by a 2G SIM. If the incoming data appears
 * to be from a 3G SIM, this function converts it to 2G
 * format before proceeding. 
 */
int convertSimIoFcp(RIL_SIM_IO_Response *sr, char **cvt)
{
    int err;
    size_t pos;
    size_t fcplen;
    struct ts_51011_921_resp resp;
    void *cvt_buf = NULL;

    if (!sr->simResponse || !cvt) {
        err = -EINVAL;
        goto error;
    }

    fcplen = strlen(sr->simResponse);
    if ((fcplen == 0) || (fcplen & 1)) {
        err = -EINVAL;
        goto error;
    }

    err = fcp_to_ts_51011(sr->simResponse, fcplen, &resp);
    if (err < 0) {
        LOGE("%s: fcp parser returned %d", __func__, err );
        goto error;
    }

    cvt_buf = malloc(sizeof(resp) * 2 + 1);
    if (!cvt_buf) {
        err = -ENOMEM;
        goto error;
    }

    err = binaryToString((unsigned char*)(&resp),
                   sizeof(resp), cvt_buf);
    if (err < 0)
        goto error;

    /* cvt_buf ownership is moved to the caller */
    *cvt = cvt_buf;
    cvt_buf = NULL;

finally:
    return err;

error:
    free(cvt_buf);
    goto finally;
}

void manageSavedState(int request, char* inputPin)
{
	static char savedPin[10];
	if (request == 0)
	{
		strcpy(savedPin,inputPin);
	}
	else if (request ==1)
	{
		strcpy(inputPin,savedPin);
	}
}

/**
 * Authenticate SIM PIN2
 *
 * @param [in] pSIMPIN2
 *      the associated PIN2
 * @param t
 *     the RIL token identifier
 *
 * @return
 *     nothing
 *
 * @note
 *     none
 */
int authenticateSIMPIN2(char *pSIMPIN2,RIL_Token t,int n)
{
   char* cmd = NULL;
   char *cpin2line;
   static char savedSimPin[10];
   int count=3;
   int num_retries = 0;
   int err;
   ATResponse *atresponse = NULL;
   asprintf(&cmd, "AT+CPIN2?");
   err = at_send_command(cmd, &atresponse);
   if (strcmp(atresponse->finalResponse,"OK")!=0) {
      free(cmd);
      at_response_free(atresponse);
      n = -1;
      RIL_onRequestComplete(t,RIL_E_SIM_PUK2,NULL, 0);
      return n; 
   }
   free(cmd);
   at_response_free(atresponse);
   asprintf(&cmd, "AT+CPIN2?");
   (void)at_send_command_singleline_with_timeout(cmd,"+CPIN",&atresponse,30000);
   cpin2line = atresponse->p_intermediates->line;
   free(cmd);
   if (0 == strncmp(cpin2line,"+CPIN2: SIM PIN2",strlen("+CPIN2: SIM PIN2"))) {
      at_response_free(atresponse);
      asprintf(&cmd, "AT+CPIN2=%s",pSIMPIN2);
      err = at_send_command(cmd, &atresponse);
      cpin2line = atresponse->finalResponse;
      if (strcmp(cpin2line,"OK") == 0) {
         manageSavedState(0,pSIMPIN2);
         LOGD("%s SavedSimPin2:%s", __func__,savedSimPin);
         free(cmd);
         at_response_free(atresponse);
      }
      else {
         num_retries = getPINRetryCount( SIM_PIN2_TYPE );
         if((num_retries >=1 )) {
            n=-1;
            RIL_onRequestComplete(t,RIL_E_PASSWORD_INCORRECT,NULL, 0);
            free(cmd);
            at_response_free(atresponse);
            return n;
         }
         else {
            n=-1;
            RIL_onRequestComplete(t,RIL_E_SIM_PUK2,NULL, 0);
            free(cmd);
            at_response_free(atresponse);
            return n;
         }
      }

   }
   else if (0 == strncmp(cpin2line, "+CPIN2: READY",strlen("+CPIN2: READY"))) {
      num_retries = 3;
      at_response_free(atresponse);
      manageSavedState(1,&savedSimPin[0]);
      if ( (savedSimPin[0] !=0) && ( strcmp(savedSimPin,pSIMPIN2) != 0) ) {
         n=-1;
         RIL_onRequestComplete(t,RIL_E_PASSWORD_INCORRECT,NULL, 0);
         return n;
      }
   }
  return n;
}

/**
 * writeBCDInHex_ToMSISDNRes
 *
 * Write a BCD byte as 2 hexadecimal chars values in MSISDN response array 
 */
static void writeBCDInHex_ToMSISDNRes(unsigned char c)
{
    if ((c >> 4) <= 9)
        msisdn_crsm_response[28] = (c >> 4) + 0x30;
    else if ((c >> 4) >= 0x0A)
        msisdn_crsm_response[28] = (c >> 4) - 0x0A + 0x41;
        
    if ((c & 0x0F) <= 9)
        msisdn_crsm_response[29] = (c &  0x0F) + 0x30;
    else if ((c & 0x0F) >= 0x0A)
        msisdn_crsm_response[29] = (c & 0x0F) - 0x0A + 0x41;
}


/**
 * convertMSISDN_QMIresponse_To_CRSMformat
 *
 * Convert a digit string to BCD string (CRSM format)
 * As per 3GPP TS11.11, a phone number in a digit string is converted
 * in BCD string. First digit string is converted in BCD string and then
 * BCD string into hex string.
 * It later on modify the other fields in MSISDN response formatted for CRSM
 * responne command
 */
static int convertMSISDN_QMIresponse_To_CRSMformat(char *pVoiceNumber) 
{
    size_t i, pos, len;
    unsigned char *binaryData = NULL;
    char *extVoiceNumber;
    size_t binaryDataLen; 
    size_t extVoiceLen;
    unsigned char c;

    i = 0;
    pos = 0;
    len = strlen(pVoiceNumber);

    if (len % 2)
        binaryDataLen = len/2 + 1;
    else
        binaryDataLen = len/2;

    extVoiceLen = binaryDataLen * 2;
    extVoiceNumber = malloc(extVoiceLen + 1);
    if (!extVoiceNumber) {
        LOGE("%s - Memory allocation failure", __func__);
        return -1;
    }
    extVoiceNumber[extVoiceLen] = '\0';
    binaryData = malloc(binaryDataLen + 1);
    if (!binaryData) {
        free (extVoiceNumber);
        LOGE("%s - Memory allocation failure", __func__);
        return -1;
    }    
    memset(binaryData,0,binaryDataLen);
    binaryData[binaryDataLen] = '\0';

    digitStringToBCDString (pVoiceNumber,len, binaryData);    
    binaryToString(binaryData, binaryDataLen, extVoiceNumber);

    /* Modify the required fields in MSISDN response */
    c = charToBCD((unsigned char)(binaryDataLen + 1));
    writeBCDInHex_ToMSISDNRes(c);
    strncpy(&msisdn_crsm_response[32], extVoiceNumber, extVoiceLen);
    free (binaryData);
    free (extVoiceNumber);   
    return 0;
}

int readSimEccFileStatus(bool *pb3GSIM) 
{
    ATResponse *atresponse = NULL;
    ATLine *atlinep = NULL;
    char *line;
    int err = 0;
    int retValue = 0;
    int nRet;
    char* simResponse = NULL;;
    int sw1,sw2;

    LOGD("%s : Entered",__func__);
    
    /* Get SIM status */
    err = at_send_command_singleline("AT+CRSM=242,28599", "+CRSM:", &atresponse);

    if (err < 0 || atresponse->success == 0) {
        goto error;
    }
    line = atresponse->p_intermediates->line;

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &sw1);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &sw2);
    if (err < 0)
        goto error;

    if (at_tok_hasmore(&line)) {
        err = at_tok_nextstr(&line, &simResponse);
        if (err < 0)
            goto error;        
        *pb3GSIM = is3GSim(simResponse);
    }    
 
finally:    
    at_response_free(atresponse);
    LOGD("%s : finally :%d",__func__,retValue);
    return retValue;

error:
    retValue =  -1;
    goto finally;
}


/**
 * convertIccId_To_CRSMformat
 *
 * Convert a digit string to BCD string (CRSM format)
 * As per 3GPP TS11.11, a phone number in a digit string is converted
 * in BCD string. First digit string is converted in BCD string and then
 * BCD string into hex string.
 */
static void convertIccId_To_CRSMformat(char *pIccid, char *pIccid_Crsm) 
{
    size_t i, pos, len;
    unsigned char *binaryData = NULL;
    char *extNumber;
    size_t binaryDataLen; 
    size_t extLen;

    i = 0;
    pos = 0;
    len = strlen(pIccid);

    if (len % 2)
        binaryDataLen = len/2 + 1;
    else
        binaryDataLen = len/2;

    extLen = binaryDataLen * 2;
    extNumber = malloc(extLen + 1);
    if (!extNumber) {
        LOGE("%s - Memory allocation failure", __func__);
        return;
    }
    extNumber[extLen] = '\0';
    binaryData = malloc(binaryDataLen + 1);
    if (!binaryData) {
        free (extNumber);
        LOGE("%s - Memory allocation failure", __func__);
        return;
    }    
    memset(binaryData,0,binaryDataLen);
    binaryData[binaryDataLen] = '\0';

    digitStringToBCDString (pIccid,len, binaryData);    
    binaryToString(binaryData, binaryDataLen, extNumber);

    strncpy(pIccid_Crsm, extNumber, extLen);
    pIccid_Crsm[extLen] ='\0';   
    free (binaryData);
    free (extNumber);   
    return;
}

/**
 * RIL_REQUEST_SIM_IO
 *
 * Request SIM I/O operation.
 * This is similar to the TS 27.007 "restricted SIM" operation
 * where it assumes all of the EF selection will be done by the
 * callee.
 */
void requestSIM_IO(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    RIL_SIM_IO_Response sr;
    int len, cvt_done, err;
    char *cmd = NULL;
    RIL_SIM_IO *ioargs;
    int num_retries = 0;
    char *line;
    char propValue[PROPERTY_VALUE_MAX];
    unsigned long nRet;
    int n=0;
    int ret = 0;
    char pVoiceNumber[21];
    char pMIN[21];
    char  stringsize = 50;
    char  ModelId[stringsize];
    char fmrString[0xff];
    char iccid[64];

    cvt_done = 0;
    memset(&sr, 0, sizeof(sr));

    ioargs = (RIL_SIM_IO *) data;

    /* Handling of pin2. */
    if (ioargs->pin2 != NULL) {
        ret = authenticateSIMPIN2(ioargs->pin2,t,n);
        if(ret==-1){
            LOGD("%s - return from here", __func__ );
            return;
        }
    }

    /* Workaround for SL9090 for SWI6600U_02.00.00.02 : BZ48549 */
    if(ioargs->fileid == EF_MSISDN)
    {
        nRet = GetFirmwareRevision(sizeof(fmrString), &fmrString[0]);
        if (nRet == 0)
        {
            if (0 != strstr(fmrString,BASEBAND_SL9090_UMTS))
            {
                if (ioargs->command == SIM_GET_RESPONSE) {
                    sr.sw1 = 0x90;
                    sr.sw2 = 0;
                    sr.simResponse = FAKE_GET_RESPONSE_MSISDN_SL9090;
                    RIL_onRequestComplete(t, RIL_E_SUCCESS, &sr, sizeof(sr));
                    return;
                }
                if (ioargs->command == SIM_GET_RECORD) {
                    nRet = GetVoiceNumber( sizeof(pVoiceNumber),
                               pVoiceNumber,
                               sizeof(pMIN),
                               pMIN );
                    LOGD("%s - GetVoiceNumber returned %ld", __func__ , nRet);
                    if (nRet == 0) {
                        LOGD("%s - successfully completed %s", __func__, pVoiceNumber);
                        if (!(convertMSISDN_QMIresponse_To_CRSMformat(pVoiceNumber))) {
                            sr.sw1 = 0x90;
                            sr.sw2 = 0;
                            sr.simResponse = msisdn_crsm_response;
                            RIL_onRequestComplete(t, RIL_E_SUCCESS, &sr, sizeof(sr));
                        }
                        else {
                              RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
                        }
                    }
                    else {
                         RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
                    }
                    return;
                }
            }
        }
    }

    /* If the command is SIM_GET_RESPONSE increase the #
     * of requested bytes to ensure we get the whole
     * header 
     */
    if(ioargs->command == SIM_GET_RESPONSE) {
        /* SWI_TBD: workaround for SL9090 - SIM header request size need to be set to 0 */
        if ((GetModelID( stringsize, ModelId) == 0) && (0 != strstr(ModelId,"9090")))
        {
            LOGD("%s - for SL9090", __func__ );
            ioargs->p3 = SIM_HEADER_REQ_SIZE_QMI;
        }
        else {
            ioargs->p3 = SIM_HEADER_REQ_SIZE;
        }
    }

    if (ioargs->data == NULL) {
        asprintf(&cmd, "AT+CRSM=%d,%d,%d,%d,%d",
                 ioargs->command, ioargs->fileid,
                 ioargs->p1, ioargs->p2, ioargs->p3);
    } else {
        len = asprintf(&cmd, "AT+CRSM=%d,%d,%d,%d,%d,\"%s\"",
                 ioargs->command, ioargs->fileid,
                 ioargs->p1, ioargs->p2, ioargs->p3, ioargs->data);
        if( len < 0 ) {
            err = len;
            goto error;
        }
        /* Convert the data to uppercase hex, if applicable */
        hexToUpper(cmd, len);
    }

    /* SWI_TBD: try timing out after 30s instead of the usual 3min */
    err = at_send_command_singleline_with_timeout(cmd, "+CRSM:", &atresponse, 30000);

    if (err < 0 || atresponse->success == 0) {
        goto error;
    }

    line = atresponse->p_intermediates->line;

    #ifdef SWI_HL_RIL
    #warning "HL RIL SIM handler"
    if(line == NULL)
        goto error;
    #endif

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &(sr.sw1));
    if (err < 0)
        goto error;

    /* not for HL */
    #ifndef SWI_HL_RIL
    /* 
    || Work around for issue CUS72958, retrying SIM IO for Voice mail related
    || EFs
     */
    if ((sr.sw1 != 0x90) && 
       ((ioargs->fileid == 28615) || (ioargs->fileid == 28617) || (ioargs->fileid == 28439))) {
        at_response_free(atresponse);
        LOGD("%s - retrying SIMIO command", __func__ );
        err = at_send_command_singleline_with_timeout(cmd, "+CRSM:", &atresponse, 30000);
        if (err < 0 || atresponse->success == 0) {
            goto error;
        }
        line = atresponse->p_intermediates->line;
        err = at_tok_start(&line);
        if (err < 0)
            goto error;

        err = at_tok_nextint(&line, &(sr.sw1));
        if (err < 0)
            goto error;
    }
    #endif

    if ((sr.sw1 != 0x90) && (ioargs->fileid == EF_ICCID) && s_AR75_Modem) 
        goto iccid_resp;
    
    err = at_tok_nextint(&line, &(sr.sw2));
    if (err < 0)
        goto error;

    #ifdef SWI_HL_RIL
    if(line == NULL)
    	goto error;
    #endif

    if (at_tok_hasmore(&line)) {
        err = at_tok_nextstr(&line, &(sr.simResponse));
        if (err < 0)
            goto error;
    }

    /*
     * Now that the modem has responded, re-check the command and 
     * check the first byte of the response to see if it is an FCP 
     * Template. If so we parse it here, otherwise it's probably a 
     * 2G header so just let the java layer handle it
     */
    if (ioargs->command == SIM_GET_RESPONSE && is3GSim( sr.simResponse ) ) {
        LOGD("FCP Template  detected");
        err = convertSimIoFcp(&sr, &sr.simResponse);
        if (err < 0)
            goto error;
        cvt_done = 1; /* sr.simResponse needs to be freed */
    }
    LOGD("%s - successfully completed", __func__ );
    RIL_onRequestComplete(t, RIL_E_SUCCESS, &sr, sizeof(sr));

finally:
    at_response_free(atresponse);
    free(cmd);
    if( cvt_done ) {
        free(sr.simResponse);
    }
    return;

error:
    LOGE("%s - error detected: %d", __func__, err );
    if ((ioargs->fileid == EF_ICCID) && s_AR75_Modem)
        goto iccid_resp;
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;

iccid_resp:
    /* Workaround for AR Modem series*/
    at_response_free(atresponse);
    atresponse = NULL;
    free(cmd);
    if (ioargs->command == SIM_GET_RESPONSE) {             
        sr.simResponse = FAKE_ICCID_QUERY_RESPONSE;
    }
    if (ioargs->command == SIM_GET_BINARY) {
        err = at_send_command_singleline("AT!ICCID?", "ICCID:", &atresponse);
        if (err < 0 || atresponse->success == 0) {
            at_response_free(atresponse);
            sr.simResponse = FAKE_ICCID_RESPONSE;
            goto final_iccid;
        }
        line = atresponse->p_intermediates->line;
        err = at_tok_start(&line);
        if (err < 0) {
            at_response_free(atresponse);
            sr.simResponse = FAKE_ICCID_RESPONSE;
            goto final_iccid;
        }
        err = at_tok_nextstr(&line, &(sr.simResponse));
        if (err < 0) {
            at_response_free(atresponse);        
            sr.simResponse = FAKE_ICCID_RESPONSE;
            goto final_iccid;
        }
        convertIccId_To_CRSMformat(sr.simResponse,iccid);
        at_response_free(atresponse);
        sr.simResponse = iccid;
    }

final_iccid:
    sr.sw1 = 0x90;
    sr.sw2 = 0;
    RIL_onRequestComplete(t, RIL_E_SUCCESS, &sr, sizeof(sr));
    return;
}

/**
 * Enter SIM PIN, might be PIN, PIN2, PUK, PUK2, etc.
 *
 * Data can hold pointers to one or two strings, depending on what we
 * want to enter. (PUK requires new PIN, etc.).
 *
 */
void requestEnterSimPin(void *data, size_t datalen, RIL_Token t, int request)
{
    ATResponse *atresponse = NULL;
    int err;
    int cme_err;
    char *cmd = NULL;
    const char **strings = (const char **) data;
    int num_retries = -1;
    PINRetryCount_s retryCount;
    int count;

    if (datalen == sizeof(char *)) {
        asprintf(&cmd, "AT+CPIN=\"%s\"", strings[0]);
    } else if ((datalen == 2 * sizeof(char *)) ||
               (datalen == 3 * sizeof(char *))) {
        /* SWI_TDB Modem can't access AID(Application Identifier) now, so ignore it */
        if(strings[1] == NULL)
            asprintf(&cmd, "AT+CPIN=\"%s\"", strings[0]);
        else
            asprintf(&cmd, "AT+CPIN=\"%s\",\"%s\"", strings[0], strings[1]);
    } 
    else {
        goto error;
    }

    err = at_send_command(cmd, &atresponse);
    free(cmd);

    cme_err = at_get_cme_error(atresponse);

    if (cme_err != CME_SUCCESS && (err < 0 || atresponse->success == 0)) {
        if (cme_err == CME_INCORRECT_PASSWORD || 
            cme_err == CME_SIM_PIN_REQUIRED) {

            /* Set the number of retries based on the request */
            num_retries = getPINRetryCount( getSIMPinPukType(request) );

            RIL_onRequestComplete( t,
                                   RIL_E_PASSWORD_INCORRECT,
                                   &num_retries,
                                   sizeof(int *) );
        } else
            goto error;

    } else {
        /* Got OK, so return success and start SIM polling process so that the
           RADIO state can be updated once the SIM is ready. While not actually
           required, wait a short time before starting the polling process,
           since the SIM won't be ready right away anyways, and thus we avoid
           unnecessary polling.
         */      
        resetpollSIMRetry(); 
        enqueueRILEvent(RIL_EVENT_QUEUE_PRIO, pollSIMState, NULL, &TIMEVAL_SIMPOLL);

        /* Set the number of retries based on the request */
        for (count = 5; count > 0 && num_retries < 1; count--)
        {
            num_retries = getPINRetryCount( getSIMPinPukType(request) );
            usleep(500000);
        }
        
        if(strings[1] != NULL)
        {
           manageSavedState(0,(char*)strings[1]);
        }
        RIL_onRequestComplete(t, RIL_E_SUCCESS, &num_retries, sizeof(int *));
    }

finally:
    at_response_free(atresponse);
    return;
error:
    /* Set the number of retries based on the request */
    num_retries = getPINRetryCount( getSIMPinPukType(request) );

    RIL_onRequestComplete( t,
                           RIL_E_GENERIC_FAILURE,
                           &num_retries,
                           sizeof(int *) );
    goto finally;
}

void requestChangePassword(void *data, size_t datalen, RIL_Token t,
                           char *facility)
{
    int err = 0;
    char *oldPassword = NULL;
    char *newPassword = NULL;
    char *cmd = NULL;
    ATResponse *atresponse = NULL;
    int num_retries = -1;
    PINRetryCount_s retryCount;

    if( datalen > 3 * sizeof(char *) || 
        datalen < 2 * sizeof(char *) || 
        strlen(facility) != 2 )
        goto error;

    oldPassword = ((char **) data)[0];
    newPassword = ((char **) data)[1];

    asprintf(&cmd, "AT+CPWD=\"%s\",\"%s\",\"%s\"", facility, oldPassword,
             newPassword);

    err = at_send_command(cmd, &atresponse);
    free(cmd);
    if (err < 0 || atresponse->success == 0)
        goto error;

    /* Set the number of retries if the SIM PIN or SIM PIN2 is the facility */
    if( !strcmp(facility, "SC") ) {
        num_retries = getPINRetryCount( SIM_PIN_TYPE );
    }
    else if( !strcmp(facility, "P2") ) {
        num_retries = getPINRetryCount( SIM_PIN2_TYPE );
    }
    manageSavedState(0,newPassword);

    RIL_onRequestComplete(t, RIL_E_SUCCESS, &num_retries, sizeof(int *));

finally:
    at_response_free(atresponse);
    return;

error:
    /* Set the number of retries if the SIM PIN or SIM PIN2 is the facility */
    if( !strcmp(facility, "SC") ) {
        num_retries = getPINRetryCount( SIM_PIN_TYPE );
    }
    else if( !strcmp(facility, "P2") ) {
        num_retries = getPINRetryCount( SIM_PIN2_TYPE );
    }

    if (atresponse != NULL && 
        at_get_cme_error(atresponse) == CME_INCORRECT_PASSWORD) {
        RIL_onRequestComplete( t,
                               RIL_E_PASSWORD_INCORRECT,
                               &num_retries,
                               sizeof(int *) );
    } else {
        RIL_onRequestComplete( t,
                               RIL_E_GENERIC_FAILURE,
                               &num_retries,
                               sizeof(int *) );
    }
    goto finally;
}

/**
 * RIL_REQUEST_SET_FACILITY_LOCK
 *
 * Enable/disable one facility lock.
 */
void requestSetFacilityLock(void *data, size_t datalen, RIL_Token t)
{
    int err;
    ATResponse *atresponse = NULL;
    char *cmd = NULL;
    char *facility_string = NULL;
    int facility_mode = -1;
    char *facility_mode_str = NULL;
    char *facility_password = NULL;
    char *facility_class = NULL;
    int num_retries = -1;
    RIL_Errno rilerr = RIL_E_GENERIC_FAILURE;

    assert(datalen >= (4 * sizeof(char **)));

    facility_string = ((char **) data)[0];
    facility_mode_str = ((char **) data)[1];
    facility_password = ((char **) data)[2];
    facility_class = ((char **) data)[3];

    assert(*facility_mode_str == '0' || *facility_mode_str == '1');
    facility_mode = atoi(facility_mode_str);

    if( '0' == *facility_class)
        asprintf(&cmd, "AT+CLCK=\"%s\",%d,\"%s\",255", facility_string,
             facility_mode, facility_password);
    else
        asprintf(&cmd, "AT+CLCK=\"%s\",%d,\"%s\",%s", facility_string,
             facility_mode, facility_password, facility_class);

    err = at_send_command(cmd, &atresponse);
    free(cmd);
    
    /* Get the retry count only if the SIM PIN is the facility */
    if( !strcmp(facility_string, "FD")) {
        num_retries = getPINRetryCount( SIM_PIN2_TYPE );
        if(num_retries >= 1 && num_retries < 3) {
            rilerr = RIL_E_PASSWORD_INCORRECT;
            goto error;
        }
        else if (num_retries == 0) {
            rilerr = RIL_E_SIM_PUK2;
            goto error;
        }  
    }
    if(!strcmp(facility_string, "SC")) {
        num_retries = getPINRetryCount( SIM_PIN_TYPE );  
        if(num_retries >= 1 && num_retries < 3) {
            rilerr = RIL_E_PASSWORD_INCORRECT;
            goto error;    
        }  
    }
    if (err < 0 || atresponse->success == 0) {
        LOGD("%s:: at fail, num_retries= %d, atresponse->success = %d \n",
                   __func__, num_retries,atresponse->success );
        goto error;
    }
    manageSavedState(0,facility_password);
    RIL_onRequestComplete(t, RIL_E_SUCCESS, &num_retries, sizeof(int *));
    at_response_free(atresponse);
    return;

error:
    at_response_free(atresponse);

    RIL_onRequestComplete( t,
                           rilerr,
                           &num_retries,
                           sizeof(int *) );

    /* If number of retry is 0, send out RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED 
        which will trigger up layer to send GET_SIM_STATUS request, applicable only for PIN1 */
    if(num_retries == 0 && !strcmp(facility_string, "SC")){
        LOGD("%s:: retry number %d, send RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED ",
            __func__, num_retries );
        RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED,
                      NULL, 0);
    }

}

/**
 * RIL_REQUEST_QUERY_FACILITY_LOCK
 *
 * Query the status of a facility lock state.
 */
void requestQueryFacilityLock(void *data, size_t datalen, RIL_Token t)
{
    int err, rat, response;
    ATResponse *atresponse = NULL;
    char *cmd = NULL;
    char *line = NULL;
    char *facility_string = NULL;
    char *facility_password = NULL;
    char *facility_class = NULL;

    assert(datalen >= (3 * sizeof(char **)));

    facility_string = ((char **) data)[0];
    facility_password = ((char **) data)[1];
    facility_class = ((char **) data)[2];

    if (facility_password == NULL) {
        asprintf(&cmd, "AT+CLCK=\"%s\",2", facility_string);      
    } else {
        if ('0' == *facility_class) {
            asprintf(&cmd, "AT+CLCK=\"%s\",2,\"%s\"", facility_string,
                facility_password);
        } else {
            asprintf(&cmd, "AT+CLCK=\"%s\",2,\"%s\",%s", facility_string,
                facility_password, facility_class);
        }
    }
    
    /* Short delay for 9x30 may not response sometimes */
    usleep(500000);

    err = at_send_command_singleline(cmd, "+CLCK:", &atresponse);
    free(cmd);
    if (err < 0 || atresponse->success == 0) {
        goto error;
    }

    line = atresponse->p_intermediates->line;

    err = at_tok_start(&line);

    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &response);

    if (err < 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, &response, sizeof(int));

finally:
    at_response_free(atresponse);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_GET_CLIR
 * Gets current CLIR status.
 *
 * @param [in] data
 *     "data" is NULL
 * @param datalen
 *     length of the data parameter received
 * @param t
 *     token passed to the RIL command request
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
void requestGetCLIR(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err;
    int response[2];            // <n> and <m>
    char *line;

   err = at_send_command_singleline("AT+CLIR?", "+CLIR:", &atresponse);

    if (err < 0 || atresponse->success == 0) {
        goto error;
    }

    line = atresponse->p_intermediates->line;

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

    RIL_onRequestComplete(t, RIL_E_SUCCESS, response, sizeof(response));

finally:
    at_response_free(atresponse);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_SET_CLIR
 * Sets the CLIR status.
 *
 * @param [in] data
 *     "data" is NULL
 * @param datalen
 *     length of the data parameter received
 * @param t
 *     token passed to the RIL command request
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
void requestSetCLIR(void *data, size_t datalen, RIL_Token t)
{
    char *cmd = NULL;
    int err;
    ATResponse *atresponse = NULL;
    char propValue[PROPERTY_VALUE_MAX];

    asprintf(&cmd, "AT+CLIR=%d", ((int *) data)[0]);

    err = at_send_command(cmd, &atresponse);

    if (err < 0 || atresponse->success == 0) {
        goto error;
    }
    if ((property_get(PROPERTY_VERSION_BASEBAND, propValue, NULL) > 0) &&
            ((0 != strstr(propValue,BASEBAND_SL9090_UMTS)) ||
              (0 != strstr(propValue,BASEBAND_MC73xx_VOICE)))) {
            setCLIRVal(((int *) data)[0]);
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
finally:
    free(cmd);
    at_response_free(atresponse);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}
