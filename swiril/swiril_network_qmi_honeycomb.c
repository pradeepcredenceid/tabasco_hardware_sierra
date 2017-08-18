/**
 *
 * @ingroup swiril
 *
 * @file 
 * Provides network related QMI functions for RIL version 6
 *
 * @author
 * Copyright: © 2011 Sierra Wireless, Inc.
 *            All rights reserved
 *
 */

#include <telephony/ril.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <cutils/properties.h>

#include "swiril_main.h"
#include "swiril_cache.h"
#include "swiril_misc_qmi.h"
#include "swiril_network.h"
#include "swiril_network_qmi.h"
#include "swiril_device_qmi.h"
#include "swiril_pdp_qmi.h"

#include "SWIWWANCMAPI.h"
#include "qmerrno.h"
#include "qmudefs.h"

#define LOG_TAG "RIL"
#include "swiril_log.h"

#define BASEBAND_SL9090_UMTS "SWI6600U"

static int getRadioTechCDMA(void)
{
    int networkType = RIL_RAT_UNKNOWN;
    struct getServingNetworkPara servingNetworkParaOld;
    BYTE cap;
    BYTE radioIface;
    BYTE count;
    
    /* check eHRPD */
    networkType = getRatCDMA();

    if (networkType == RIL_RAT_UNKNOWN) {
        servingNetworkParaOld = getServingNetworkParameters();
        if (getServingDataCapability(&cap, true)) {
            LOGD("%s find serving data capability=%d", __func__, cap);
            switch(cap) {
                case QMI_SERVING_NW_CAPABILITY_CDMA_1XRTT:
                    networkType = RIL_RAT_1xRTT;
                    break;
                    
                case QMI_SERVING_NW_CAPABILITY_CDMA_1XEVDO_REV0:
                    networkType = RIL_RAT_EvDo_REV0;
                    break;
                    
                case QMI_SERVING_NW_CAPABILITY_CDMA_1XEVDO_REVA:
                    networkType = RIL_RAT_EvDo_REVA;
                    break;
                    
                case QMI_SERVING_NW_CAPABILITY_CDMA_1XEVDO_REVB:
                    networkType = RIL_RAT_EvDo_REVB;
                    break;
    
                case QMI_SERVING_NW_CAPABILITY_GPRS:
                case QMI_SERVING_NW_CAPABILITY_EDGE:
                case QMI_SERVING_NW_CAPABILITY_WCDMA:
                case QMI_SERVING_NW_CAPABILITY_HSDPA:
                case QMI_SERVING_NW_CAPABILITY_HSUPA:
                case QMI_SERVING_NW_CAPABILITY_LTE:
                case QMI_SERVING_NW_CAPABILITY_HSPA_PLUS:
                case QMI_SERVING_NW_CAPABILITY_DUAL_CARRIER_HSPA_PLUS:
                case QMI_SERVING_NW_CAPABILITY_GSM:
                default:
                    break;
            }        
        }
        else {
            radioIface = getServingRadioIface();
            LOGD("%s radioIface=%d", __func__, radioIface);
            switch(radioIface) {
                case RAT_CDMA_1XRTT:
                    networkType = RIL_RAT_1xRTT;
                    break;
                    
                case RAT_CDMA_1XEVDO:
                    networkType = RIL_RAT_EvDo_REVA;
                    break;
                                
                case RAT_GSM:
                case RAT_UMTS:
                case RAT_NO_SERVICE:
                case RAT_AMPS:
                default:
                    break;
            }
        }
    }
    LOGD("getRadioTechCDMA networkType=%d", networkType);
    return networkType;
}

/* SWI_TBD It is better to maintain different handler for voice and data,
 * because even some common fields between voice and data have slight different defintions. 
 * We may need to handle difference later */
static void sendVoiceRegistStateCDMA(RIL_Token t)
{
    BYTE  count;
    char *responseStr[VOICE_REGISTRATION_STATE_PARAMS];
    int networkType = RIL_RAT_UNKNOWN;
    ULONG nRet;
    qaQmiServingSystemParam tqaQmiServingSystemParam;
    struct getServingNetworkPara servingNetworkParaOld;
    int RegState = RIL_REG_STATE_UNKNOWN;
    
    servingNetworkParaOld = getServingNetworkParameters();    
    memset(responseStr, 0, sizeof(responseStr));

    RegState = getRegistrationState();
    if((RegState == RIL_REG_STATE_NOT_REGISTERED || 
        RegState == RIL_REG_STATE_SEARCHING_NOT_REGISTERED ||
        RegState == RIL_REG_STATE_REGISTRATION_DENIED ||
        RegState == RIL_REG_STATE_UNKNOWN ) && 
        (currentState() == RADIO_STATE_SIM_LOCKED_OR_ABSENT)) {
         LOGD("%s update status from %d to %d ", __func__, RegState, RegState + 10);
         RegState += 10;
    }
    asprintf(&responseStr[0], "%d", RegState);

    /* CDMA leaves LAC and CID as NULL as defined in ril.h */
        
    /* Radio technology */
    networkType = getRadioTechCDMA();
    asprintf(&responseStr[3], "%d", networkType);
    
    if (servingNetworkParaOld.registrationState == QMI_REG_STATE_REGISTERED) {
        nRet = SLQSGetServingSystem(&tqaQmiServingSystemParam);
        printSLQSGetServingSystem((void *)&tqaQmiServingSystemParam);

        /* Base Station ID */
        asprintf(&responseStr[4], "%d", tqaQmiServingSystemParam.BasestationID);

        /* Base Station latitude */
        asprintf(&responseStr[5], "%d", (int)tqaQmiServingSystemParam.BasestationLatitude);

        /* Base Station longitude */
        asprintf(&responseStr[6], "%d", (int)tqaQmiServingSystemParam.BasestationLongitude);

        /* SWI_TBD hardcode for now 
         * concurrent services support indicator */
        asprintf(&responseStr[7], "%d", 0);

        /* System ID if registered on a CDMA system */
        asprintf(&responseStr[8], "%d", tqaQmiServingSystemParam.SystemID);

        /* Network ID if registered on a CDMA system */
        asprintf(&responseStr[9], "%d", tqaQmiServingSystemParam.NetworkID);

        /* TSB-58 Roaming Indicator */
        asprintf(&responseStr[10], "%d", (int)servingNetworkParaOld.roamingInd);

        /* SWI_TBD hardcode for now
         * indicates whether the current system is in the
         * PRL if registered on a CDMA or EVDO system */
        asprintf(&responseStr[11], "%d", 1);
        
        /* SWI_TBD hardcode for now 
         * default Roaming Indicator from the PRL */ 
        asprintf(&responseStr[12], "%d", 1);
        
        /* SWI_TBD doesn't care - if registration state is 3 (Registration denied) */
        
        /* SWI_TBD leave as NULL for Primary Scrambling Code */
    }
    
    if (t != NULL) {
        /* Return the successful response and cache the value for next time */
        swiril_cache_RIL_onRequestComplete(t, RIL_E_SUCCESS, responseStr,
                                           VOICE_REGISTRATION_STATE_PARAMS * sizeof(char *), 
                                           RIL_REQUEST_VOICE_REGISTRATION_STATE);
    }
    else {
        /* Notify state change and cache the value for next time */
        swiril_cache_set(RIL_REQUEST_VOICE_REGISTRATION_STATE,
                         responseStr, VOICE_REGISTRATION_STATE_PARAMS * sizeof(char *));
        RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED,
                                  NULL, 0);
    }
                                       
    for (count = 0; count < VOICE_REGISTRATION_STATE_PARAMS; count++) {
        if (responseStr[count])
            free(responseStr[count]);
    }
}

static void sendDataRegistStateCDMA(RIL_Token t)
{
    BYTE  count;
    char *responseStr[DATA_REGISTRATION_STATE_PARAMS];
    int networkType = RIL_RAT_UNKNOWN;
    ULONG nRet;
    qaQmiServingSystemParam tqaQmiServingSystemParam;
    struct getServingNetworkPara servingNetworkParaOld;
    
    servingNetworkParaOld = getServingNetworkParameters();    
    memset(responseStr, 0, sizeof(responseStr));
    asprintf(&responseStr[0], "%d", getRegistrationState());

    /* CDMA leaves LAC and CID as NULL */

    /* Radio technology */
    networkType = getRadioTechCDMA();
    asprintf(&responseStr[3], "%d", networkType);
        
    /* registration state is 3 (Registration denied) 
     * this is an enumerated reason why registration was denied 
     * doesn't care for now */
    
    /* The maximum number of simultaneous Data Calls that can be
     * established using RIL_REQUEST_SETUP_DATA_CALL */
    /* SWI_TBD hardcode to 1 now*/
    asprintf(&responseStr[5], "%d", 1);

    if (t != NULL) {
        /* Return the successful response and cache the value for next time */
        swiril_cache_RIL_onRequestComplete(t, RIL_E_SUCCESS, responseStr,
                                           DATA_REGISTRATION_STATE_PARAMS * sizeof(char *), 
                                           RIL_REQUEST_DATA_REGISTRATION_STATE);
    }
    else {
        /* Notify state change and cache value for next time */
        swiril_cache_set(RIL_REQUEST_DATA_REGISTRATION_STATE,
                         responseStr, DATA_REGISTRATION_STATE_PARAMS * sizeof(char *));
        /* SWI_TBD not sure which unsolicited message to use for data registration state change, 
         * use RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED  for now */
        RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED,
                                  NULL, 0);
    }
                                       
    for (count = 0; count < DATA_REGISTRATION_STATE_PARAMS; count++) {
        if (responseStr[count] != NULL)
            free(responseStr[count]);
    }
}

static int getRadioTechUMTS(void)
{
    int networkType = RIL_RAT_UNKNOWN;
    BYTE cap;
    struct getServingNetworkPara servingNetworkParaOld;
    
    servingNetworkParaOld = getServingNetworkParameters();    
    if (getServingDataCapability(&cap, false)) {
        LOGD("getRadioTechUMTS capability=%d", cap);
        switch(cap) {
            case QMI_SERVING_NW_CAPABILITY_GPRS:
            case QMI_SERVING_NW_CAPABILITY_GSM:
                networkType = RIL_RAT_GPRS;
                break;

            case QMI_SERVING_NW_CAPABILITY_EDGE:
                networkType = RIL_RAT_EDGE;
                break;
                                
            case QMI_SERVING_NW_CAPABILITY_WCDMA:
                networkType = RIL_RAT_UMTS;
                break;

            case QMI_SERVING_NW_CAPABILITY_HSDPA:
                networkType = RIL_RAT_HSDPA;
                break;

            case QMI_SERVING_NW_CAPABILITY_HSUPA:
                networkType = RIL_RAT_HSUPA;
                break;

            case QMI_SERVING_NW_CAPABILITY_LTE:
                networkType = RIL_RAT_LTE;
                break;
                          
            case QMI_SERVING_NW_CAPABILITY_HSPA_PLUS:
                networkType = RIL_RAT_HSPAP;
                break;
                
            case QMI_SERVING_NW_CAPABILITY_DUAL_CARRIER_HSPA_PLUS:
                /* Choose the close one */
                networkType = RIL_RAT_HSPAP;
                break;
                                               
            case QMI_SERVING_NW_CAPABILITY_CDMA_1XRTT:
            case QMI_SERVING_NW_CAPABILITY_CDMA_1XEVDO_REV0:
            case QMI_SERVING_NW_CAPABILITY_CDMA_1XEVDO_REVA:
            case QMI_SERVING_NW_CAPABILITY_CDMA_1XEVDO_REVB:
            default:
                break;
        }        
    }
    else {
        LOGD("getRadioTechUMTS radioIfaces=%d", servingNetworkParaOld.radioIfaces[0]);
        switch(servingNetworkParaOld.radioIfaces[0]) {
            case RAT_GSM:
                networkType = RIL_RAT_GPRS;
                break;
                
            case RAT_UMTS:
                networkType = RIL_RAT_UMTS;
                break;

            case RAT_LTE:
                networkType = RIL_RAT_LTE;
                break;
                                
            case RAT_CDMA_1XRTT:
            case RAT_CDMA_1XEVDO:
            case RAT_NO_SERVICE:
            case RAT_AMPS:
            default:
                break;
        }
    }

    /* MC8355 modems in UMTS mode don't always report technology above UMTS
     * using one of the above methods, and so we must rely on the data bearer.
     * networkType will only be updated if the necessary conditions are met.
     */
   updateWithDataBearer(&networkType);


    LOGD("getRadioTechUMTS networkType=%d", networkType);
    return networkType;
}

static void sendVoiceRegistStateUMTS(RIL_Token t)
{
    BYTE  count;
    char *responseStr[VOICE_REGISTRATION_STATE_PARAMS];
    int networkType = RIL_RAT_UNKNOWN;
    ULONG nRet;
    qaQmiServingSystemParam tqaQmiServingSystemParam;
    struct getServingNetworkPara servingNetworkParaOld;
    int lac;
    ULONG cellID;
    int RegState = RIL_REG_STATE_UNKNOWN;
        
    servingNetworkParaOld = getServingNetworkParameters();    
    memset(responseStr, 0, sizeof(responseStr));

    RegState = getRegistrationState();
    if((RegState == RIL_REG_STATE_NOT_REGISTERED || 
        RegState == RIL_REG_STATE_SEARCHING_NOT_REGISTERED ||
        RegState == RIL_REG_STATE_REGISTRATION_DENIED ||
        RegState == RIL_REG_STATE_UNKNOWN ) && 
        (currentState() == RADIO_STATE_SIM_LOCKED_OR_ABSENT)) {
         LOGD("%s update status from %d to %d ", __func__, RegState, RegState + 10);
         RegState += 10;
    }
    asprintf(&responseStr[0], "%d", RegState);
    
    if (servingNetworkParaOld.registrationState == QMI_REG_STATE_REGISTERED) {
        if (getLacCellID(&lac, &cellID)) {
            asprintf(&responseStr[1], "%04x", lac);
            asprintf(&responseStr[2], "%08lx", cellID);
        }
    }
        
    /* Radio technology */
    networkType = getRadioTechUMTS();
    asprintf(&responseStr[3], "%d", networkType);
    
    if (servingNetworkParaOld.registrationState == QMI_REG_STATE_REGISTERED) {
        nRet = SLQSGetServingSystem(&tqaQmiServingSystemParam);
        printSLQSGetServingSystem((void *)&tqaQmiServingSystemParam);

        /* Base Station ID - CDMA only */

        /* Base Station latitude - CDMA only  */

        /* Base Station longitude - CDMA only  */

        /* concurrent services support indicator - CDMA only  */

        /* System ID if registered on a CDMA system - CDMA only  */

        /* Network ID if registered on a CDMA system - CDMA only  */

        /* TSB-58 Roaming Indicator - CDMA only */

        /* indicates whether the current system is in the
         * PRL if registered on a CDMA or EVDO system - CDMA only  */
        
        /* default Roaming Indicator from the PRL - CDMA only  */
        
        /* SWI_TBD doesn't care - if registration state is 3 (Registration denied) */
        
        /* SWI_TBD leave as NULL for Primary Scrambling Code */
    }
    
    if (t != NULL) {
        /* Return the successful response and cache the value for next time */
        swiril_cache_RIL_onRequestComplete(t, RIL_E_SUCCESS, responseStr,
                                           VOICE_REGISTRATION_STATE_PARAMS * sizeof(char *), 
                                           RIL_REQUEST_VOICE_REGISTRATION_STATE);
    }
    else {
        /* Notify state change and cache the value for next time */
        swiril_cache_set(RIL_REQUEST_VOICE_REGISTRATION_STATE,
                         responseStr, VOICE_REGISTRATION_STATE_PARAMS * sizeof(char *));
        RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED,
                                  NULL, 0);
    }
                                       
    for (count = 0; count < VOICE_REGISTRATION_STATE_PARAMS; count++) {
        if (responseStr[count])
            free(responseStr[count]);
    }
}

static void sendDataRegistStateUMTS(RIL_Token t)
{
    BYTE  count;
    char *responseStr[DATA_REGISTRATION_STATE_PARAMS];
    int networkType = RIL_RAT_UNKNOWN;
    BYTE  pDataCapsSize = RAT_MAX;
    BYTE  pDataCaps[RAT_MAX];
    struct getServingNetworkPara servingNetworkParaOld;
    int lac;
    ULONG cellID;
    
    servingNetworkParaOld = getServingNetworkParameters();   
    memset(responseStr, 0, sizeof(responseStr));
    asprintf(&responseStr[0], "%d", (int)servingNetworkParaOld.PSDomain);

    if (servingNetworkParaOld.registrationState == QMI_REG_STATE_REGISTERED) {
        if (getLacCellID(&lac, &cellID)) {
            asprintf(&responseStr[1], "%04x", lac);
            asprintf(&responseStr[2], "%08lx", cellID);
        }
    }
    
    /* Radio technology */
    networkType = getRadioTechUMTS();
    asprintf(&responseStr[3], "%d", networkType);
        
    /* registration state is 3 (Registration denied) 
     * this is an enumerated reason why registration was denied 
     * doesn't care for now */
    
    /* The maximum number of simultaneous Data Calls that can be
     * established using RIL_REQUEST_SETUP_DATA_CALL */
    /* SWI_TBD hardcode to 1 now*/
    asprintf(&responseStr[5], "%d", 1);

    if (t != NULL) {
        /* Return the successful response and cache the value for next time */
        swiril_cache_RIL_onRequestComplete(t, RIL_E_SUCCESS, responseStr,
                                           DATA_REGISTRATION_STATE_PARAMS * sizeof(char *), 
                                           RIL_REQUEST_DATA_REGISTRATION_STATE);
    }
    else {
        /* Notify state change and cache value for next time */
        swiril_cache_set(RIL_REQUEST_DATA_REGISTRATION_STATE,
                         responseStr, DATA_REGISTRATION_STATE_PARAMS * sizeof(char *));
        /* SWI_TBD not sure which unsolicited message to use */
        RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED,
                                  NULL, 0);
    }
                                       
    for (count = 0; count < DATA_REGISTRATION_STATE_PARAMS; count++) {
        if (responseStr[count] != NULL)
            free(responseStr[count]);
    }
}

void pollRegistration(void *params)
{
    BYTE updated = 0;
    ULONG nRet;
    struct getServingNetworkPara servingNetworkParaOld;
    SWI_FW_INFO_TYPE tech; 
    CHAR fmrString[NUM_MAX_STR_LEN];
    
    if (getServingNetworkQMI(&updated) == eQCWWAN_ERR_NONE) {
        servingNetworkParaOld = getServingNetworkParameters();
        /* notifiy only when something changed */
        if (updated != SWI_NO_UPDATE) {
            if ((servingNetworkParaOld.registrationState == QMI_REG_STATE_REGISTERED)
                && ((servingNetworkParaOld.RAN == QMI_REGISTERED_RAN_UMTS) 
                /* special condition for LTE */
                   || ((servingNetworkParaOld.radioIfaces[0] == RAT_LTE) 
                       && (servingNetworkParaOld.RAN == QMI_REGISTERED_RAN_UNKNOWN)))) {
                /* send both */
                sendVoiceRegistStateUMTS(NULL);
                sendDataRegistStateUMTS(NULL);
                /* Get Firmware version */
                nRet = GetFirmwareRevision(sizeof(fmrString), &fmrString[0]);
                if (nRet == eQCWWAN_ERR_NONE) {
                    /* This function should not be invoked for SL9090/FW9x15C as QMI_NAS_NETWORK_TIME_IND
                     * is used to get the date and time for the same */
                    if ((!strstr(fmrString,BASEBAND_SL9090_UMTS)) && (!isFW9x15C())) {
                        if (updated == SWI_REGISTRATION_STATUS_UPDATED){
                            if(IsSierraDevice()) {
                                sendNITZtimeATMC77xx(true);
                            } 
                            else {
                                sendNITZtimeAT(true);
                            }
			 }
		    }
                }
                else
		    LOGE("%s GetFirmwareRevision error %lu",__func__,nRet);
            }
            else {
                /* START MOTO 167 */
                /* default as CDMA response.This will also take care of case
                 * when network is lost. Note: here RAN will not be known */
                sendVoiceRegistStateCDMA(NULL);
                sendDataRegistStateCDMA(NULL);
                /* registered with unknown RAN, assume CDMA and update NITZ */
                if ((servingNetworkParaOld.registrationState == QMI_REG_STATE_REGISTERED) &&
                    (updated == SWI_REGISTRATION_STATUS_UPDATED))
                    sendNITZtimeQMI(QMI_REGISTERED_RAN_CDMA2000);
                /* STOP MOTO 167 */
            }
        }
        if ((servingNetworkParaOld.registrationState == QMI_REG_STATE_REGISTERED)
            && ((servingNetworkParaOld.RAN == QMI_REGISTERED_RAN_UMTS) 
            /* special condition for LTE */
               || ((servingNetworkParaOld.radioIfaces[0] == RAT_LTE) 
                       && (servingNetworkParaOld.RAN == QMI_REGISTERED_RAN_UNKNOWN)))){
            /* Get Firmware version */
	    nRet = GetFirmwareRevision(sizeof(fmrString), &fmrString[0]);
	    if (nRet == eQCWWAN_ERR_NONE) {
                /* This function should not be invoked for SL9090/FW9x15C as QMI_NAS_NETWORK_TIME_IND
                 * is used to get the date and time for the same */
                if ((!strstr(fmrString,BASEBAND_SL9090_UMTS)) && (!isFW9x15C())) {
                    if(IsSierraDevice()) {
                        sendNITZtimeATMC77xx(false);
                    } else {
                        sendNITZtimeAT(false);
                    }
                }
	    }
	    else
	        LOGE("%s GetFirmwareRevision error %lu",__func__,nRet);
        }
    }
}

/**
 * RIL_REQUEST_VOICE_REGISTRATION_STATE
 *
 * Request current voice registration state.
 */
void requestVoiceRegistrationStateCDMA(void *data, size_t datalen, RIL_Token t)
{
    BYTE updated = 0;

    if (getServingNetworkQMI(&updated) != eQCWWAN_ERR_NONE) {
        getServingNetworkErrHandle();
    }
    /* send response */
    sendVoiceRegistStateCDMA(t);
}

void requestDataRegistrationStateCDMA(void *data, size_t datalen, RIL_Token t)
{
    BYTE updated = 0;

    if (getServingNetworkQMI(&updated) != eQCWWAN_ERR_NONE) {
        getServingNetworkErrHandle();
    }
    /* send response */
    sendDataRegistStateCDMA(t);
}

void requestVoiceRegistrationStateUMTS(void *data, size_t datalen, RIL_Token t)
{
    BYTE updated = 0;
    
    if (getServingNetworkQMI(&updated) != eQCWWAN_ERR_NONE) {
        getServingNetworkErrHandle();
    }
    /* send response */
    sendVoiceRegistStateUMTS(t);
}

void requestDataRegistrationStateUMTS(void *data, size_t datalen, RIL_Token t)
{
    BYTE updated = 0;
    
    if (getServingNetworkQMI(&updated) != eQCWWAN_ERR_NONE) {
        getServingNetworkErrHandle();
    }
    /* send response */
    sendDataRegistStateUMTS(t);
}

#ifdef SWI_RIL_VERSION_9
void requestSetInitialAPN(void *data, size_t datalen, RIL_Token t)
{
    RIL_InitialAttachApn    *p_pf;
    int ret;
    ULONG profileType = 0;  /* 0:3GPP 1:3GPP2*/
    ULONG PDPType = 0;      /* 0:IPV4 */
    ULONG authType;
    SWI_FW_INFO_TYPE tech;
    
    p_pf = (RIL_InitialAttachApn *)data;
 
    if (!p_pf->apn) {
        LOGI("%s : apn=%s is invalid.", __func__,p_pf->apn);
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }
    
    /* check authentication type, RIL supports 0, 1, 2, 3 */
    if ((p_pf->authtype < 0) || (p_pf->authtype > 3)) {
       LOGI("%s RIL does not support auth type %d, change to default value", __func__, p_pf->authtype);
        authType = 3 ; //AUTH_PAP_CHAP
    }
    else {
        authType = p_pf->authtype;
    }
    
    if ( isAPNProfileIdentical(p_pf->apn, &authType, p_pf->username, p_pf->password))
    {
        LOGI("%s : Profile identical.", __func__);
        RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
        return;
    }
    
    LOGI("%s APN info: apn=%s, protocol=%s, authtype=%d , username=%s, password=%s",
        __func__,p_pf->apn,p_pf->protocol, authType, p_pf->username,p_pf->password);
    
    if (getFirmwareInfoTechQMI(&tech) && 
        ((tech == SWI_FW_INFO_TYPE_GOBI_UMTS)||(tech == SWI_FW_INFO_TYPE_SWI_UMTS_ONLY))) {
        ret = SetDefaultProfile(profileType,
                             &PDPType,
                             NULL,
                             NULL,
                             NULL,
                             &authType,
                             "android_prof",
                             p_pf->apn,
                             p_pf->username,
                             p_pf->password);

        if (ret != eQCWWAN_ERR_NONE)
        {
            LOGE("%s failed to set default profile with error=%lu", __func__, ret);
            RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        }
        RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    }
    else {
        RIL_onRequestComplete(t, RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0);
    }    
}
#endif

/**
 *
 * Send unsolicited CDMA registration State message.
 *
 * @return
 *     None
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 *
 */
void sendUnsolicitedCDMARegState(void)
{
    sendVoiceRegistStateCDMA(NULL);
    sendDataRegistStateCDMA(NULL);
}

/**
 *
 * Send unsolicited UMTS Data registration State change message.
 *
 * @return
 *     None
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 *
 */
void sendUnsolicitedUMTSRegState(void)
{
    sendVoiceRegistStateUMTS(NULL);
    sendDataRegistStateUMTS(NULL);
}

void updateNetworkSysInfo()
{
    if (getRealtimeTechnology() == SWI_RUNNING_TECH_CDMA)
        sendUnsolicitedCDMARegState();
    else    
        sendUnsolicitedUMTSRegState();
}
