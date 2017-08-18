/**
 *
 * @ingroup swiril
 *
 * @file 
 * Provides voice related QMI based Sierra functions
 *
 * @author
 * Copyright: © 2012 Sierra Wireless, Inc.
 *            All rights reserved
 *
 */

#include <stdio.h>
#include <string.h>
#include <telephony/ril.h>
#include <assert.h>
#include <stdbool.h>
#include "swiril_main.h"
#include "swiril_callhandling.h"
#include "swiril_misc.h"
#include <cutils/properties.h>
#include "swiril_device_qmi.h"
#include "swiril_network_common_qmi.h"

#define LOG_TAG "RIL"
#include "swiril_log.h"

/* For QMI */
#include "qmerrno.h"
#include "SWIWWANCMAPI.h"
#include "swiril_oem.h"

/* QMI related CLIR defines for SLQS */
#define CLIR_NOT_SET     0x00
#define CLIR_SUPPRESSION 0x01
#define CLIR_INVOCATION  0x02

#define CLIR_INACTIVE 0x00
#define CLIR_ACTIVE   0x01

#define CLIR_NOT_PROVISIONED      0x00
#define CLIR_PROVISIONED_PERM     0x01
#define CLIR_TEMP_RESTRICTED      0x02
#define CLIR_PRESENTATION_ALLOWED 0x03

/* 3GPP related CLIR defines for RIL */
#define GPP_CLIR_NOCHANGE    0x00
#define GPP_CLIR_INVOCATION  0x01
#define GPP_CLIR_SUPPRESSION 0x02

#define GPP_CLIR_NOT_PROVISIONED      0x00
#define GPP_CLIR_PROVISIONED_PERM     0x01
#define GPP_CLIR_UNKNOWN              0x02
#define GPP_CLIR_TEMP_RESTRICTED      0x03
#define GPP_CLIR_PRESENTATION_ALLOWED 0x04

/** Emergency call Reporting Macros */
#define EMERGENCY_CALL_TYPE       0x09
#define VOICE_CALL_TYPE           0x00
#define OTASP_CALL_TYPE           0x08
#define EMERGENCY_CATEGORY_MANUAL 0x20

/* QMI call end reason for 3GPP TS 24.008 Annex H */
#define QMI_FAILURE_CAUSE_UNASSIGNED_NUMBER         141
#define QMI_FAILURE_CAUSE_INTERWORKING_UNSPECIFIED  188
/* Call end reason of 3GPP TS 24.008 Annex H extending RIL_LastCallFailCause in ril.h */
#define CALL_FAIL_OPERATOR_DETERMINED_BARRING       8

BYTE storedCLIRVal = CLIR_NOT_SET;
static const struct timeval TIMEVAL_NOTIFY_CALL_WAITING_CHANGE = { 10, 0 };
static const struct timeval TIMEVAL_1S_DELAY= { 1, 0 };

enum eQMI_CALL_STATES {
    CALL_STATE_ORIGINATION      = 0x01,
    CALL_STATE_INCOMING         = 0x02,
    CALL_STATE_CONVERSATION     = 0x03,
    CALL_STATE_CC_IN_PROGRESS   = 0x04,
    CALL_STATE_ALERTING         = 0x05,
    CALL_STATE_HOLD             = 0x06,
    CALL_STATE_WAITING          = 0x07,
    CALL_STATE_DISCONNECTING    = 0x08,
    CALL_STATE_END              = 0x09,
    CALL_STATE_SETUP            = 0x0A,
    CALL_STATE_UNAVAILABLE      = 0xFF,
};

enum eQMI_CALL_DIRECTION {
    CALL_DIRECTION_MO = 0x01,
    CALL_DIRECTION_MT = 0x02,
};

enum eQMI_CALL_MODE {
    CALL_MODE_CDMA       = 0x01,
    CALL_MODE_GSM        = 0x02,
    CALL_MODE_UMTS       = 0x03,
    CALL_MODE_LTE        = 0x04,
    CALL_MODE_TDS        = 0x05,
    CALL_MODE_UNAVALABLE = 0xFF,
};

enum eQMI_VOICE_PRIVACY {
    VOICE_PRIVACY_STANDARD    = 0x01,
    VOICE_PRIVACY_ENHANCED    = 0x02,
    VOICE_PRIVACY_UNAVAILABLE = 0xFF,
};

enum eQMI_SUPS_TYPE {
    SUPS_TYPE_RELEASE_HELD_OR_WAITING               = 0x01,
    SUPS_TYPE_RELEASE_ACTIVE_ACCEPT_HELD_OR_WAITING = 0x02,
    SUPS_TYPE_HOLD_ACTIVE_ACCEPT_WAITING_OR_HELD    = 0x03,
    SUPS_TYPE_HOLD_ALL_EXCEPT_SPECIFIED_CALL        = 0x04,
    SUPS_TYPE_MAKE_CONFERENCE_CALL                  = 0x05,
    SUPS_TYPE_EXPLICIT_CALL_TRANSFER                = 0x06,
    SUPS_TYPE_CCBS_ACTIVATION                       = 0x07,
    SUPS_TYPE_END_ALL_CALLS                         = 0x08,
    SUPS_TYPE_RELEASE_SPECIFIED_CALL                = 0x09,
};

enum eQMI_AUDIO_GENERATOR {
     AUDIO_GENERATOR_VOICE    = 0x00,
     AUDIO_GENERATOR_KEY_BEEP = 0x01,
     AUDIO_GENERATOR_MIDI     = 0x02,
};

static const int s_ril_call_end_reason[] = {
    1,      /* CALL_END_CAUSE_UNASSIGNED_NUMBER */
    3,      /* CALL_END_CAUSE_NO_ROUTE_TO_DESTINATION */
    6,      /* CALL_END_CAUSE_CHANNEL_UNACCEPTABLE */
    8,      /* CALL_END_CAUSE_OPERATOR_DETERMINED_BARRING */
    16,     /* CALL_END_CAUSE_NORMAL_CALL_CLEARING */
    17,     /* CALL_END_CAUSE_USER_BUSY */
    18,     /* CALL_END_CAUSE_NO_USER_RESPONDING */
    19,     /* CALL_END_CAUSE_USER_ALERTING_NO_ANSWER */
    21,     /* CALL_END_CAUSE_CALL_REJECTED */
    22,     /* CALL_END_CAUSE_NUMBER_CHANGED */
    25,     /* CALL_END_CAUSE_PREEMPTION */
    27,     /* CALL_END_CAUSE_DESTINATION_OUT_OF_ORDER */
    28,     /* CALL_END_CAUSE_INVALID_NUMBER_FORMAT */
    29,     /* CALL_END_CAUSE_FACILITY_REJECTED */
    30,     /* CALL_END_CAUSE_RESP_TO_STATUS_ENQUIRY */
    31,     /* CALL_END_CAUSE_NORMAL_UNSPECIFIED */
    34,     /* CALL_END_CAUSE_NO_CIRCUIT_OR_CHANNEL_AVAILABLE */
    38,     /* CALL_END_CAUSE_NETWORK_OUT_OF_ORDER */
    41,     /* CALL_END_CAUSE_TEMPORARY_FAILURE */
    42,     /* CALL_END_CAUSE_SWITCHING_EQUIPMENT_CONGESTION */
    43,     /* CALL_END_CAUSE_ACCESS_INFORMATION_DISCARDED */
    44,     /* CALL_END_CAUSE_REQUESTED_CIRCUIT_OR_CHANNEL_NOT_AVAILABLE */
    47,     /* CALL_END_CAUSE_RESOURCES_UNAVAILABLE_OR_UNSPECIFIED */
    49,     /* CALL_END_CAUSE_QOS_UNAVAILABLE */
    50,     /* CALL_END_CAUSE_REQUESTED_FACILITY_NOT_SUBSCRIBED */
    55,     /* CALL_END_CAUSE_INCOMING_CALLS_BARRED_WITHIN_CUG */
    57,     /* CALL_END_CAUSE_BEARER_CAPABILITY_NOT_AUTH */
    58,     /* CALL_END_CAUSE_BEARER_CAPABILITY_UNAVAILABLE */
    63,     /* CALL_END_CAUSE_SERVICE_OPTION_NOT_AVAILABLE */
    68,     /* CALL_END_CAUSE_ACM_LIMIT_EXCEEDED */
    65,     /* CALL_END_CAUSE_BEARER_SERVICE_NOT_IMPLEMENTED */
    69,     /* CALL_END_CAUSE_REQUESTED_FACILITY_NOT_IMPLEMENTED */
    70,     /* CALL_END_CAUSE_ONLY_DIGITAL_INFORMATION_BEARER_AVAILABLE */
    79,     /* CALL_END_CAUSE_SERVICE_OR_OPTION_NOT_IMPLEMENTED */
    81,     /* CALL_END_CAUSE_INVALID_TRANSACTION_IDENTIFIER */
    87,     /* CALL_END_CAUSE_USER_NOT_MEMBER_OF_CUG */
    88,     /* CALL_END_CAUSE_INCOMPATIBLE_DESTINATIONB */
    91,     /* CALL_END_CAUSE_INVALID_TRANSIT_NW_SELECTION */
    95,     /* CALL_END_CAUSE_SEMANTICALLY_INCORRECT_MESSAGE */
    96,     /* CALL_END_CAUSE_INVALID_MANDATORY_INFORMATION */
    97,     /* CALL_END_CAUSE_MESSAGE_TYPE_NON_IMPLEMENTED */
    98,     /* CALL_END_CAUSE_MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE */
    99,     /* CALL_END_CAUSE_INFORMATION_ELEMENT_NON_EXISTENT */
    100,    /* CALL_END_CAUSE_CONDITONAL_IE_ERROR */
    101,    /* CALL_END_CAUSE_MESSAGE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE */
    102,    /* CALL_END_CAUSE_RECOVERY_ON_TIMER_EXPIRED */
    111,    /* CALL_END_CAUSE_PROTOCOL_ERROR_UNSPECIFIED */
    127     /* CALL_END_CAUSE_INTERWORKING_UNSPECIFIED */
};

/* Last call fail cause */
static int s_lastCallFailCause = CALL_FAIL_ERROR_UNSPECIFIED;
static int s_currentMuteValue = 0;

/* Override current Call info for COLP */
#define QMI_SL9090_OVERRIDE_CALLID_BZ48538

#define MAX_NUMBER_LEN  81
typedef struct 
{
    bool    bOverrideRemoteNum;
    BYTE    call_id;
    char    number[MAX_NUMBER_LEN];
    BYTE    numLen;
} voiceOverrideRemoteNumInfo;

voiceOverrideRemoteNumInfo s_OverriddenCallId;

static BYTE s_CID=1;

/* function prototypes */
void getPrevDDTMPreference(struct DDTMSettings* pPrevDDTMPref );

/**
 * Converts the call state information from QMI to RIL
 *
 * @param [in] state
 *     QMI call state information
 * @param [out] p_state
 *     converted RIL call state information
 *
 * @return
 *     true, if the call information is retrieved; false, otherwise
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 */
bool qmiCallStateToRILState(
    enum eQMI_CALL_STATES state,
    RIL_CallState *p_state)
{
    bool stateStatus = true;
    static RIL_CallState sLastCallstatus = 0xff;
    
    LOGD("%s: current QMI state :%d, last call RIL status: %d", __func__, state, sLastCallstatus);
    switch (state) {
        /**
         * Disconnect is parsed with last call status. And End is
         * grouped with active call, as there is no 
         * corresponding RIL state. RIL would disconnect the next time
         * the call state changes (call ID would not be present)
         */
        case CALL_STATE_DISCONNECTING:
            if(sLastCallstatus != 0xff)
                *p_state = sLastCallstatus;
            break;

        case CALL_STATE_CONVERSATION:
        case CALL_STATE_END:
            *p_state = RIL_CALL_ACTIVE;
            break;

        case CALL_STATE_HOLD:
            *p_state = RIL_CALL_HOLDING;
            break;

        case CALL_STATE_ORIGINATION:
        case CALL_STATE_CC_IN_PROGRESS:
            *p_state = RIL_CALL_DIALING;
            break;

        case CALL_STATE_ALERTING:
            *p_state = RIL_CALL_ALERTING;
            break;

        case CALL_STATE_INCOMING:
        case CALL_STATE_SETUP:
            *p_state = RIL_CALL_INCOMING;
            break;

        case CALL_STATE_WAITING:
            *p_state = RIL_CALL_WAITING;
            break;

        case CALL_STATE_UNAVAILABLE:
        default:
            stateStatus = false;
            break;
    }
    sLastCallstatus = *p_state;
    return stateStatus;
}

/**
 * Converts the call information from SLQS to the RIL_Call type
 *
 * @param [in] pAllCallInfo
 *     call information received from SLQS
 * @param [out] pCalls
 *     call information which needs to be filled to be sent to application layer
 *
 * @return
 *     true, if the call information is retrieved; false, otherwise
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 */
bool getCallInformation (voiceGetAllCallInfo *pAllCallInfo, RIL_Call *pCalls)
{
    int countCalls = 0, countNumbers = 0, countNames = 0, length = 0;
    bool callInfoStatus = true;
    int i = 0, j = 0, k = 0;

    LOGD("%s: Entered", __func__);

    if (pAllCallInfo->pArrCallInfo != NULL) {
        countCalls = pAllCallInfo->pArrCallInfo->numInstances;
    } else {
        callInfoStatus = false;
    }

    LOGD("%s: countCalls : %d", __func__, countCalls);

    for (i = 0; i < countCalls; i++) {
        getAllCallInformation *pCurCallInfo =
                    &pAllCallInfo->pArrCallInfo->getAllCallInfo[i];
        RIL_Call *pCurCall = pCalls + i;

        pCurCall->index = pCurCallInfo->Callinfo.callID;

        /* Get the call information, if error return to the request routine */
        callInfoStatus = qmiCallStateToRILState(
                            pCurCallInfo->Callinfo.callState, &pCurCall->state);
        if (!callInfoStatus)
            break;

        /* Is call mobile terminted */
        pCurCall->isMT = 
            (pCurCallInfo->Callinfo.direction == CALL_DIRECTION_MO)?0:1;

        if (pCurCallInfo->Callinfo.mode == 0x01) {
            LOGD("%s: Call mode is CDMA ", __func__);
        }
        if (pCurCall->state == RIL_CALL_ACTIVE) {
            LOGD("%s: Call state is active ", __func__);
        }  

        /* Is Multiparty enabled */
        pCurCall->isMpty = pCurCallInfo->isEmpty;

        /* Alternate Line Service Indicator */
        pCurCall->als = pCurCallInfo->ALS;

        /* Is the current call a voice call */
        pCurCall->isVoice = (pCurCallInfo->Callinfo.callType == 0)?1:0;

        /* Is CDMA voice privacy enabled */
        if (*(pAllCallInfo->pVoicePrivacy) != VOICE_PRIVACY_UNAVAILABLE) {
            pCurCall->isVoicePrivacy = 1;
        }

        /* Type of address */
        pCurCall->toa = pCurCallInfo->Callinfo.callType;

        /* Update the number and number presentation */
        countNumbers = pAllCallInfo->pArrRemotePartyNum->numInstances;
        for (j = 0; j < countNumbers; j++) {
            getAllCallRmtPtyNum *pCurNumInfo =
                    &pAllCallInfo->pArrRemotePartyNum->RmtPtyNum[j];   

            /* Copy the number information if the CallID matches */
            if (pCurNumInfo->callID == pCurCallInfo->Callinfo.callID){
                if ((s_OverriddenCallId.bOverrideRemoteNum) &&
                    (pCurCallInfo->Callinfo.callID == s_OverriddenCallId.call_id)) {
                    LOGD("%s: Overriding RemotePartyNum \n", __func__);                
                    length = s_OverriddenCallId.numLen;
                    pCurNumInfo->RemotePartyNum.remPartyNumber[length] = 0;
                    memcpy(pCurNumInfo->RemotePartyNum.remPartyNumber, 
                           s_OverriddenCallId.number,
                           length
                           );
                    pCurCall->number = (char *)pCurNumInfo->RemotePartyNum.remPartyNumber;
                    pCurCall->numberPresentation = 
                            pCurNumInfo->RemotePartyNum.presentationInd;
                }
                else {                    
                    length = pCurNumInfo->RemotePartyNum.numLen;
                    pCurNumInfo->RemotePartyNum.remPartyNumber[length] = 0;
                    pCurCall->number = 
                           (char *)pCurNumInfo->RemotePartyNum.remPartyNumber;
                    pCurCall->numberPresentation = 
                            pCurNumInfo->RemotePartyNum.presentationInd;
                }
            }
        }

        /* Update the name and name presentation */
        countNames = pAllCallInfo->pArrRemotePartyName->numInstances;
        for (k = 0; k < countNames; k++) {
            getAllCallRmtPtyName *pCurNameInfo =
                    &pAllCallInfo->pArrRemotePartyName->GetAllCallRmtPtyName[k];

            /* Copy the name information if the CallID matches */
            if (pCurNameInfo->callID == pCurCallInfo->Callinfo.callID){
                length = pCurNameInfo->RemotePartyName.nameLen;
                pCurNameInfo->RemotePartyName.callerName[length] = 0;
                pCurCall->name =
                        (char *) pCurNameInfo->RemotePartyName.callerName;
                pCurCall->namePresentation =
                         pCurNameInfo->RemotePartyName.namePI;
            }
        }
    }
    return callInfoStatus;
}

/**
 * RIL_REQUEST_GET_CURRENT_CALLS
 * Requests current call list
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
void requestGetCurrentCalls(void *data, size_t datalen, RIL_Token t)
{
    int countCalls, i;
    RIL_Call *calls;
    RIL_Call **response = NULL;
    ULONG nRet = eQCWWAN_ERR_NONE;
    /* Parameters required for getting the voice information */
    voiceGetAllCallInfo allCallInfo;
    arrCallInfo         callInformation;
    arrRemotePartyNum   remotePartyNum;
    arrRemotePartyName  remotePartyName;
    BYTE                voicePrivacy;

    LOGD("%s: Entered", __func__);

    /** 
     * Need to assign memory only for the parameters which needs to be
     * fetched
     */
    memset( &allCallInfo, 0, sizeof(allCallInfo));
    memset( &callInformation, 0, sizeof(arrCallInfo));
    memset( &remotePartyNum, 0, sizeof(arrRemotePartyNum));
    memset( &remotePartyName, 0, sizeof(arrRemotePartyName));
    memset( &voicePrivacy, 0, sizeof(BYTE));

    allCallInfo.pArrCallInfo        = &callInformation;
    allCallInfo.pArrRemotePartyNum  = &remotePartyNum;
    allCallInfo.pArrRemotePartyName = &remotePartyName;
    allCallInfo.pVoicePrivacy       = &voicePrivacy;

    nRet = SLQSVoiceGetAllCallInfo (&allCallInfo);

    if ( nRet != eQCWWAN_ERR_NONE ) {
        LOGD("%s: SLQSVoiceGetAllCallInfo() failed %lu", __func__, nRet);
        goto error;
    }

    countCalls = allCallInfo.pArrCallInfo->numInstances;
    if ( countCalls == 0 ) {
        LOGD("%s: No Active/Waiting/Held Call", __func__);
        goto finally;
    }


    /* There's an array of pointers and then an array of structures. */
    response = (RIL_Call **) alloca(countCalls * sizeof(RIL_Call *));
    calls = (RIL_Call *) alloca(countCalls * sizeof(RIL_Call));
    memset(calls, 0, countCalls * sizeof(RIL_Call));

    /* Init the pointer array. */
    for (i = 0; i < countCalls; i++) {
        response[i] = &(calls[i]);
    }

    if(!getCallInformation(&allCallInfo, calls))
        goto error;

finally:
    RIL_onRequestComplete(t, RIL_E_SUCCESS, response,
                          countCalls * sizeof(RIL_Call *));
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

/**
 * Enqueued function to send the call state change unsolicited response
 *
 * @param [in] params
 *     NULL in this case
 *
 * @return
 *     none
 */
static void onCallStateChange (void *params)
{
    RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED,
                              NULL,
                              0);
}

/**
 * It Displays call information of all call in progress
 *
 * @param [in] pCallBackInfo
 *     All call state information
 *
 * @return
 *     none
 */
void VoiceCallInfoCallback( voiceSetAllCallStatusCbkInfo *pCallBackInfo )
{
    int i = 0;
    int iQmiCallEndReason = 0;
    BYTE callState;
    BYTE callType;
    BYTE direction;

    LOGD("%s: Enqueue the state change indication", __func__);
    if (!pCallBackInfo)
       return;
 
    callState = pCallBackInfo->arrCallInfomation.getAllCallInfo[0].Callinfo.callState;
    callType = pCallBackInfo->arrCallInfomation.getAllCallInfo[0].Callinfo.callType;
    direction = pCallBackInfo->arrCallInfomation.getAllCallInfo[0].Callinfo.direction;

    /* If OTAPA call, don't send it up to Android app/framework */
    if ((callType == 0x06) && (direction == 0x02)) {
        LOGD("%s: OTAPA MT call, CallState %d,return!", __func__, callState);
        return;
    }

    if (pCallBackInfo->pArrCallEndReason != NULL) {
        arrCallEndReason *pCallEndReason = pCallBackInfo->pArrCallEndReason;
        i = pCallEndReason->numInstances - 1; 
        iQmiCallEndReason = pCallEndReason->callEndReason[i];
        if( iQmiCallEndReason >= QMI_FAILURE_CAUSE_UNASSIGNED_NUMBER && 
            iQmiCallEndReason <= QMI_FAILURE_CAUSE_INTERWORKING_UNSPECIFIED )
            /* Map the call end faild cause from QMI definition to RIL(3GPP) definition */
            s_lastCallFailCause = s_ril_call_end_reason[iQmiCallEndReason - QMI_FAILURE_CAUSE_UNASSIGNED_NUMBER];
        else
            s_lastCallFailCause = iQmiCallEndReason & 0x7F;
    }

    /* Workaround for PTCRB test 31.8.7 */
    if(s_lastCallFailCause == CALL_FAIL_OPERATOR_DETERMINED_BARRING) {
        LOGD("%s:: change s_lastCallFailCause from %d to %d", __func__, s_lastCallFailCause, CALL_FAIL_CALL_BARRED);
        s_lastCallFailCause = CALL_FAIL_CALL_BARRED;
    }
    LOGD("%s: s_lastCallFailCause = %d", __func__, s_lastCallFailCause);

    /**
     * Enqueue the function to send the usolicited response for the call state
     * change. We do not require to send anything else as the GET_CURRENT_CALLS
     * would be invoked subsequently to get the call information
     * TBD - Need to decide whether we need to send the RING indication, as of
     * now seems to be redundant as the CALL_STATE_CHANGED has to be sent anyway
     */ 
    enqueueRILEvent(RIL_EVENT_QUEUE_PRIO, onCallStateChange, NULL, NULL);
}

#ifdef QMI_SL9090_OVERRIDE_CALLID_BZ48538
/**
 * It Displays call information record available from network
 *
 * @param [in] pVoiceInfoRec
 *     call information record
 *
 * @return
 *     none
 */
void VoiceInfoRecCallback( voiceInfoRec *pVoiceInfoRec  )
{    
    voiceInfoRec *pLVACS = pVoiceInfoRec;

    LOGD("%s:: Entered:\n", __func__);
    if (!pLVACS) {
        LOGE("%s:: VoiceInfoRecCallback failed, return\n",__func__);
        return;
    }    
    memset( &s_OverriddenCallId, 0, sizeof(voiceOverrideRemoteNumInfo));
    s_OverriddenCallId.call_id = pLVACS->callID; 
    LOGD("%s:: Call id %d:\n", __func__,pLVACS->callID );  
    if (pLVACS && pLVACS->pConnectNumInfo ) { 
        connectNumInfo *pTemp = pLVACS->pConnectNumInfo;       
        LOGD("%s:: Connected Number Information:\n",__func__ );
        LOGD("PI                    :%d\n",pTemp->numPresInd );
        LOGD("NumLen                :%d\n",pTemp->callerIDLen );
        LOGD("Connected number Info :%s\n",pTemp->callerID); 
        
        s_OverriddenCallId.bOverrideRemoteNum = true;
        s_OverriddenCallId.numLen = pTemp->callerIDLen;
        if (s_OverriddenCallId.numLen >= MAX_NUMBER_LEN) {
            LOGE("%s:: Invalid length of number %d, return\n",__func__,s_OverriddenCallId.numLen);
            memset( &s_OverriddenCallId, 0, sizeof(voiceOverrideRemoteNumInfo));
            return;
        }
        s_OverriddenCallId.number[s_OverriddenCallId.numLen] = '\0';
        memcpy(&s_OverriddenCallId.number[0],pTemp->callerID,s_OverriddenCallId.numLen);        
        RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED,
                                  NULL,
                                  0);          
    } else if (pLVACS && pLVACS->pCallWaitInd) {
        if (*pLVACS->pCallWaitInd == 1) {
            /* New call waiting indication received */
            RIL_CDMA_CallWaiting_v6 callWaitInfo;

            memset(&callWaitInfo,0, sizeof(RIL_CDMA_CallWaiting_v6));
            if (pLVACS->pCallingPartyInfo) {
                callWaitInfo.number = (char*)pLVACS->pCallingPartyInfo->callerID;
                callWaitInfo.numberPresentation = (int)pLVACS->pCallingPartyInfo->numPresInd;
                callWaitInfo.number_type = (int)pLVACS->pCallingPartyInfo->numType;
                callWaitInfo.number_plan = (int)pLVACS->pCallingPartyInfo->numPlan;
            }
            if (pLVACS->pSignalInfo) {
                callWaitInfo.signalInfoRecord.isPresent = true;
                callWaitInfo.signalInfoRecord.signalType = pLVACS->pSignalInfo->signalType;
                callWaitInfo.signalInfoRecord.alertPitch = pLVACS->pSignalInfo->alertPitch;
                callWaitInfo.signalInfoRecord.signal = pLVACS->pSignalInfo->signal;
            }

            LOGD("%s:[line:%d]: received new call waiting indication!\n", __func__, __LINE__);
            LOGD("%s: Calling number Info:%s \n",__func__,pLVACS->pCallingPartyInfo->callerID);
            RIL_onUnsolicitedResponse(RIL_UNSOL_CDMA_CALL_WAITING,
                                      &callWaitInfo,
                                      sizeof(RIL_CDMA_CallWaiting_v6));
        }
    }
}
#endif

/**
 * Register voice notification call back function
 *
 * @param 
 *     none
 *
 * @return
 *     none
 */
void registerVoiceCallbackQMI(void)
{
    LOGD("%s:: entered\n", __func__);
    ULONG nRet = eQCWWAN_ERR_NONE;

    /* register callback function */
    nRet = SLQSVoiceSetAllCallStatusCallBack (&VoiceCallInfoCallback);
    if ( nRet != eQCWWAN_ERR_NONE ) {
        LOGE("%s:: SLQSVoiceSetAllCallStatusCallBack failed, nRet: %lu\n",
             __func__, nRet );
    }
#ifdef QMI_SL9090_OVERRIDE_CALLID_BZ48538
    nRet = SLQSVoiceInfoRecCallback( &VoiceInfoRecCallback );
    if ( nRet != eQCWWAN_ERR_NONE ) {
        LOGE("%s:: SLQSVoiceInfoRecCallback failed, nRet: %lu\n",
             __func__, nRet );
    }
#endif
}

/**
 * RIL_REQUEST_LAST_CALL_FAIL_CAUSE
 * Requests the failure cause code for the most recently terminated call
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
 *     See also: RIL_REQUEST_LAST_PDP_FAIL_CAUSE
 */
void requestLastCallFailCause(void *data, size_t datalen, RIL_Token t)
{
    RIL_onRequestComplete(t, RIL_E_SUCCESS, &s_lastCallFailCause,
                              sizeof(int));
}

/**
 * Store the CLIR value set using RIL_REQUEST_SET_CLIR
 *
 * @param [in] clirVal
 *     CLIR value to be set
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
void setCLIRVal(int clirVal)
{    
    switch (clirVal) {
        case CLIR_INVOCATION:
            storedCLIRVal = GPP_CLIR_INVOCATION;
            break;
        case CLIR_SUPPRESSION:
            storedCLIRVal = GPP_CLIR_SUPPRESSION;
            break;
        default:
            storedCLIRVal = GPP_CLIR_NOCHANGE;
            break;
    }
}

/**
 * Retreive the CLIR value set using RIL_REQUEST_SET_CLIR
 *
 * @param
 *     none
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
static BYTE getStoredCLIRVal()
{
    return storedCLIRVal;
}

/**
 * Convert the CLIR activation status from QMI representation to RIL
 * representation
 *
 * @param [in] clirActiveStatus
 *     activation status in QMI format
 *
 * @return
 *     activation status in 3GPP format
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 */
BYTE convertCLIRActiveStatus(BYTE clirActiveStatus)
{
    BYTE clirActStatus;

    switch (clirActiveStatus) {
        case CLIR_INACTIVE:
            clirActStatus = GPP_CLIR_SUPPRESSION;
            break;
        case CLIR_ACTIVE:
            clirActStatus = GPP_CLIR_INVOCATION;
            break;
        default:
            clirActStatus = GPP_CLIR_NOCHANGE;
            break;
    }
    return clirActStatus;
}

/**
 * Convert the CLIR provision status from QMI representation to RIL
 * representation
 *
 * @param [in] clirProvisionStatus
 *     provision status in QMI format
 *
 * @return
 *     provision status in 3GPP format
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 */
BYTE convertCLIRProvStatus(BYTE clirProvisionStatus)
{
    BYTE clirProvStatus;

    switch (clirProvisionStatus) {
        case CLIR_NOT_PROVISIONED:
            clirProvStatus = GPP_CLIR_NOT_PROVISIONED;
            break;
        case CLIR_PROVISIONED_PERM:
            clirProvStatus = GPP_CLIR_PROVISIONED_PERM;
            break;
        case CLIR_TEMP_RESTRICTED:
            clirProvStatus = GPP_CLIR_TEMP_RESTRICTED;
            break;
        case CLIR_PRESENTATION_ALLOWED:
            clirProvStatus = GPP_CLIR_PRESENTATION_ALLOWED;
            break;
        default:
            clirProvStatus = GPP_CLIR_UNKNOWN;
            break;
    }
    return clirProvStatus;
}

/**
 * RIL_REQUEST_DIAL
 * Initiate voice call
 *
 * @param [in] data
 *     data is const RIL_Dial *
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
void requestDial(void *data, size_t datalen, RIL_Token t)
{
    RIL_Dial *dial;    
    voiceCallRequestParams  CallRequestParams;
    voiceCallResponseParams CallResponseParams;
    BYTE clirVal = 0;
    ULONG nRet = eQCWWAN_ERR_NONE;
    char propValue[PROPERTY_VALUE_MAX];
    char *ptmpstr;
    BYTE isEmergency = VOICE_CALL_TYPE;
    BYTE EmergencyCat = EMERGENCY_CATEGORY_MANUAL;
    BYTE isOtasp = VOICE_CALL_TYPE;
    int numberLen;

    dial = (RIL_Dial *) data;
    memset( &CallRequestParams, 0, sizeof(CallRequestParams));
    memset( &CallResponseParams, 0, sizeof(CallResponseParams));
    
    /* Reset struct to override call id for COLP active */
    memset( &s_OverriddenCallId, 0, sizeof(voiceOverrideRemoteNumInfo));

    /* truncate the number if crosses permissible length */
    numberLen = strlen(dial->address);
    if (numberLen >= MAX_CALL_NO_LEN)
        numberLen = MAX_CALL_NO_LEN -1;

    /* Copy the destination address */
    strncpy((char *)CallRequestParams.callNumber, dial->address, numberLen);

    if (!(strncmp(dial->address,"*228", 4))) {
        isOtasp = OTASP_CALL_TYPE;
        CallRequestParams.pCallType = &isOtasp;
        LOGD("%s: OTASP Call",__func__);
    }   

    /* Use the CLIR value which is set using the RIL_REQUEST_SET_CLIR request */
    clirVal = getStoredCLIRVal();
    if (clirVal != CLIR_NOT_SET) {
        CallRequestParams.pCLIRType = &clirVal;
    }

    /* Set the CLIR value, do not set the value if not provided */
    switch (dial->clir) {
        /* Invocation */
        case 1:
            clirVal = CLIR_INVOCATION;
            CallRequestParams.pCLIRType = &clirVal;
            break;
        /* Suppression */
        case 2:
            clirVal = CLIR_SUPPRESSION;
            CallRequestParams.pCLIRType = &clirVal;
            break;
        /* Subscribtion default */
        case 0:
        default:
            break;
    }

    property_get(ECCLIST_PROPERTY, propValue,NULL);

    ptmpstr = strtok (propValue,",");
    while (ptmpstr != NULL)
    {
        #define PROPERTY_VERSION_BASEBAND "gsm.version.baseband"
        #define BASEBAND_SL9090_GENERIC_CDMA "SWI6600H"

        if(!strcmp(ptmpstr, dial->address)) {
           isEmergency = EMERGENCY_CALL_TYPE;
           CallRequestParams.pEmergencyCategory = &EmergencyCat;
           CallRequestParams.pCallType = &isEmergency;
           LOGD("CallRequestParams.pCallType = %d",*(CallRequestParams.pCallType));

           if ((property_get(PROPERTY_VERSION_BASEBAND, propValue, NULL) > 0) &&
               (0 == strstr(propValue, BASEBAND_SL9090_GENERIC_CDMA))) {
               /* need to disable emergency mode status callback and enable afterward, as modem may generate incorrect status callback */
               deregisterSetSysSelectionPreferenceCallbackQMI();
               /* enable emergency mode status callback */
               enqueueRILEvent(RIL_EVENT_QUEUE_PRIO, registerSetSysSelectionPreferenceCallbackQMI, NULL, &TIMEVAL_1S_DELAY);
           }
        }
        ptmpstr = strtok (NULL, ",");
    }

    if(isEmergency!=EMERGENCY_CALL_TYPE)
        ExitEmergencyMode();

    nRet = SLQSVoiceDialCall( &CallRequestParams, &CallResponseParams );

    if ( nRet != eQCWWAN_ERR_NONE ) {
        LOGD("%s: SLQSVoiceDialCall() failed %lu\n", __func__, nRet);
        goto error;
    }

    if(CallResponseParams.pCallID)
    {
        LOGD("%s: callid =  %d\n", __func__, *CallResponseParams.pCallID);
        s_CID = *CallResponseParams.pCallID;
    }    

    /* Success or failure is ignored by the upper layer here,
       it will call GET_CURRENT_CALLS and determine success that way. */
    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

/**
 * Get the call ID for the incoming call state
 *
 * @param callState
 *     The call state for which the call ID inforamtion need to be fetched
 *
 * @return
 *      call ID of the alerting call
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 */
BYTE getCallID (RIL_CallState callState)
{
    BYTE callID = 0;
    RIL_CallState state;
    int countCalls = 0, i = 0;
    bool status;
    ULONG nRet = eQCWWAN_ERR_NONE;
    /* Parameters required for getting the voice information */
    voiceGetAllCallInfo allCallInfo;
    arrCallInfo         callInformation;

    LOGD("%s: Entered", __func__);

    /** 
     * Need to assign memory only for the parameters which needs to be
     * fetched
     */
    memset( &allCallInfo, 0, sizeof(allCallInfo));
    allCallInfo.pArrCallInfo        = &callInformation;

    nRet = SLQSVoiceGetAllCallInfo (&allCallInfo);

    countCalls = allCallInfo.pArrCallInfo->numInstances;

    for (i = 0; i < countCalls; i++) {
        getAllCallInformation *pCurCallInfo =
                    &allCallInfo.pArrCallInfo->getAllCallInfo[i];
        status = qmiCallStateToRILState( pCurCallInfo->Callinfo.callState,
                                         &state);

        if ((status) && (state == callState)) {
            callID = pCurCallInfo->Callinfo.callID;
            break;
        }
    }
    return callID;
}

/**
 * RIL_REQUEST_ANSWER
 * Answer incoming call
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
 *     Will not be called for WAITING calls.
 *     RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE will be used in this 
 *     case instead
 */
void requestAnswer(void *data, size_t datalen, RIL_Token t)
{
    BYTE callID;
    voiceAnswerCall answerCallParams;
    ULONG nRet = eQCWWAN_ERR_NONE;

    callID = getCallID(RIL_CALL_INCOMING);
    answerCallParams.pCallId = &callID;

    nRet = SLQSVoiceAnswerCall( &answerCallParams );

    if ( nRet != eQCWWAN_ERR_NONE ) {
        LOGD("%s: SLQSVoiceAnswerCall() failed %lu\n", __func__, nRet);
        goto error;
    }

    /* Success or failure is ignored by the upper layer here,
       it will call GET_CURRENT_CALLS and determine success that way. */
    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

/**
 * RIL_REQUEST_HANGUP
 * Hang up a specific line
 *
 * @param [in] data
 *     data is an "int *"
 *     (int *)data)[0] contains Connection index
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
void requestHangup(void *data, size_t datalen, RIL_Token t)
{
    BYTE cid;
    ULONG nRet = eQCWWAN_ERR_NONE;

    if(data)
        cid = ((int *) data)[0];
    else
        cid = s_CID;

    LOGD("%s: cid = %d \n", __func__, cid);

    nRet = SLQSVoiceEndCall( &cid );

    if ( nRet != eQCWWAN_ERR_NONE ) {
        LOGD("%s: SLQSVoiceEndCall() failed %lu\n", __func__, nRet);
        goto error;
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    return;

error:
    /* Success or failure is ignored by the upper layer here,
       it will call GET_CURRENT_CALLS and determine success that way. */
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}



/**
 * RIL_REQUEST_CDMA_FLASH
 *
 * Send FLASH
 *
 * "data" is const char *
 * ((const char *)data)[0] is a FLASH string
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  GENERIC_FAILURE
 *
 *
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 */
void requestSendFlash(void *data, size_t datalen, RIL_Token t)
{
    BYTE cid = s_CID;
    ULONG nRet = eQCWWAN_ERR_NONE;
    voiceFlashInfo req;

    memset(&req,0,sizeof(req));

    req.pCallID = &cid;

    if (data != NULL) {
        req.pFlashPayLd = ((BYTE*) data);
    }

    nRet = SLQSVoiceSendFlash( &req );

    if ( nRet != eQCWWAN_ERR_NONE ) {
        LOGD("%s: SLQSVoiceSendFlash() failed %lu\n", __func__, nRet);
        goto error;
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    return;

error:
    /* Success or failure is ignored by the upper layer here,
       it will call GET_CURRENT_CALLS and determine success that way. */
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}



/**
 * RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND
 * Hang up waiting or held
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
void requestHangupWaitingOrBackground(void *data, size_t datalen,
                                      RIL_Token t)
{
    ULONG nRet = eQCWWAN_ERR_NONE;
    voiceManageCallsReq  manageCallsReq;
    voiceManageCallsResp manageCallsResp;
    SWI_FW_INFO_TYPE type;

    /* CDMA use hang up only */
    if (getFirmwareInfoTechQMI(&type))
        if (type == SWI_FW_INFO_TYPE_GOBI_CDMA)
            return requestHangup(data,datalen,t);

    memset( &manageCallsReq, 0, sizeof(manageCallsReq));
    memset( &manageCallsResp, 0, sizeof(manageCallsResp));

    /**
     * Release the held call, and make the backgroud call active; if there are
     * not any background calls no active calls would be present after this
     */ 
    manageCallsReq.SUPSType = SUPS_TYPE_RELEASE_HELD_OR_WAITING;
    nRet = SLQSVoiceManageCalls (&manageCallsReq, &manageCallsResp);

    if ( nRet != eQCWWAN_ERR_NONE ) {
        LOGD("%s: SLQSVoiceManageCalls() failed %lu\n", __func__, nRet);
        goto error;
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);}

/**
 * RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND
 * Hang up waiting or held
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
 *     SLQSVoiceManageCalls is for 3GPP only
 */
void requestHangupForegroundResumeBackground(void *data, size_t datalen,
                                             RIL_Token t)
{
    ULONG nRet = eQCWWAN_ERR_NONE;
    voiceManageCallsReq  manageCallsReq;
    voiceManageCallsResp manageCallsResp;
    SWI_FW_INFO_TYPE type;

    /* CDMA use hang up only */
    if (getFirmwareInfoTechQMI(&type))
        if (type == SWI_FW_INFO_TYPE_GOBI_CDMA)
            return requestHangup(data,datalen,t);


    memset( &manageCallsReq, 0, sizeof(manageCallsReq));
    memset( &manageCallsResp, 0, sizeof(manageCallsResp));

    /**
     * Release the held call, and make the backgroud call active; if there are
     * not any background calls no active calls would be present after this
     */ 
    manageCallsReq.SUPSType = SUPS_TYPE_RELEASE_ACTIVE_ACCEPT_HELD_OR_WAITING;
    nRet = SLQSVoiceManageCalls (&manageCallsReq, &manageCallsResp);

    if ( nRet != eQCWWAN_ERR_NONE ) {
        LOGD("%s: SLQSVoiceManageCalls() failed %lu\n", __func__, nRet);
        goto error;
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

/**
 * RIL_REQUEST_UDUB
 * Send UDUB (user determined used busy) to ringing or waiting call answer
 * (RIL_BasicRequest r).
 *
 * @param [in] data
 *     data is NULL
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
void requestUDUB(void *data, size_t datalen, RIL_Token t)
{
    ULONG nRet = eQCWWAN_ERR_NONE;
    voiceManageCallsReq  manageCallsReq;
    voiceManageCallsResp manageCallsResp;

    memset( &manageCallsReq, 0, sizeof(manageCallsReq));
    memset( &manageCallsResp, 0, sizeof(manageCallsResp));

    /**
     * Release the held call, and make the backgroud call active; if there are
     * not any background calls no active calls would be present after this
     */ 
    manageCallsReq.SUPSType = SUPS_TYPE_END_ALL_CALLS;
    nRet = SLQSVoiceManageCalls (&manageCallsReq, &manageCallsResp);

    if ( nRet != eQCWWAN_ERR_NONE ) {
        LOGD("%s: SLQSVoiceManageCalls() failed %lu\n", __func__, nRet);
        goto error;
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

void setCurrentMuteValue( int mute )
{
    s_currentMuteValue = mute;
    LOGD("%s: muteValue set to %d\n", __func__, s_currentMuteValue);
}

int getCurrentMuteValue( )
{
    LOGD("%s: muteValue got %d\n", __func__, s_currentMuteValue);
    return s_currentMuteValue;
}

/**
 * RIL_REQUEST_SET_MUTE
 * Turn on or off uplink (microphone) mute.
 *
 * @param [in] data
 *     data is an "int *". (int *)data)[0] is 1 for "enable mute"
 *     and 0 for "disable mute"
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
 *     Will only be sent while voice call is active. Will always be reset to
 *     "disable mute" when a new voice call is initiated.
 */
void requestSetMute(void *data, size_t datalen, RIL_Token t)
{
    int mute = ((int *) data)[0];
    int err = 0;
    ULONG nRet = eQCWWAN_ERR_NONE;
    SetAudioProfileReq SetReq;
    GetAudioProfileReq GetReq;
    GetAudioProfileResp GetRes;

    int countCalls;
    voiceGetAllCallInfo allCallInfo;
    arrCallInfo         callInformation;
    arrRemotePartyNum   remotePartyNum;
    arrRemotePartyName  remotePartyName;
    BYTE                voicePrivacy;

    memset( &SetReq, 0, sizeof(SetReq));
    memset( &GetReq, 0, sizeof(GetReq));
    memset( &GetRes, 0, sizeof(GetRes));

    LOGD("%s: Entered", __func__);

    if (!(mute == 0 || mute == 1))
        goto error;

    /* Let us check how many calls are present */
    memset( &allCallInfo, 0, sizeof(allCallInfo));
    memset( &callInformation, 0, sizeof(arrCallInfo));
    memset( &remotePartyNum, 0, sizeof(arrRemotePartyNum));
    memset( &remotePartyName, 0, sizeof(arrRemotePartyName));
    memset( &voicePrivacy, 0, sizeof(BYTE));

    allCallInfo.pArrCallInfo        = &callInformation;
    allCallInfo.pArrRemotePartyNum  = &remotePartyNum;
    allCallInfo.pArrRemotePartyName = &remotePartyName;
    allCallInfo.pVoicePrivacy       = &voicePrivacy;

    nRet = SLQSVoiceGetAllCallInfo (&allCallInfo);

    if ( nRet != eQCWWAN_ERR_NONE ) {
        LOGD("%s: SLQSVoiceGetAllCallInfo() failed %lu", __func__, nRet);
        goto error;
    }

    countCalls = allCallInfo.pArrCallInfo->numInstances;

    /* If no calls are present, mute the microphone always */
    if ( countCalls == 0 ) {
        mute = 1;
    }

    GetReq.Generator = AUDIO_GENERATOR_VOICE;

    //Firstly we get the audio profile to get the values of all the parameters
    nRet = SLQSGetAudioProfile (&GetReq,&GetRes);
    if ( nRet != eQCWWAN_ERR_NONE ) {
        LOGD("%s: SLQSSetAudioProfile() failed %lu", __func__, nRet);
        goto error;
    }

    //Update only the mute parameter..other parameters to take their values as such.
    SetReq.MicMute = mute;
    SetReq.EarMute = GetRes.EarMute;
    SetReq.Volume = GetRes.Volume;
    SetReq.Generator = AUDIO_GENERATOR_VOICE;
    SetReq.Profile = GetRes.Profile;

    nRet = SLQSSetAudioProfile (&SetReq);
    if ( nRet != eQCWWAN_ERR_NONE ) {
        LOGD("%s: SLQSSetAudioProfile() failed %lu", __func__, nRet);
        goto error;
    }
    setCurrentMuteValue(mute);

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);

}

/**
 * RIL_REQUEST_GET_MUTE
 * Queries the current state of the uplink mute setting
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
void requestGetMute(void *data, size_t datalen, RIL_Token t)
{
    int response = 0;
    LOGD("%s: entered", __func__);
    response = getCurrentMuteValue();
    RIL_onRequestComplete(t, RIL_E_SUCCESS, &response, sizeof(int));
    return;

}

/**
 * RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE
 * Switch waiting or holding call and active call
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
void requestSwitchWaitingOrHoldingAndActive(void *data, size_t datalen,
                                            RIL_Token t)
{
    ULONG nRet = eQCWWAN_ERR_NONE;
    voiceManageCallsReq  manageCallsReq;
    voiceManageCallsResp manageCallsResp;

    memset( &manageCallsReq, 0, sizeof(manageCallsReq));
    memset( &manageCallsResp, 0, sizeof(manageCallsResp));

    /**
     * Release the held call, and make the backgroud call active; if there are
     * not any background calls no active calls would be present after this
     */ 
    manageCallsReq.SUPSType = SUPS_TYPE_HOLD_ACTIVE_ACCEPT_WAITING_OR_HELD;
    nRet = SLQSVoiceManageCalls (&manageCallsReq, &manageCallsResp);

    if ( nRet != eQCWWAN_ERR_NONE ) {
        LOGD("%s: SLQSVoiceManageCalls() failed %lu\n", __func__, nRet);
        goto error;
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

/**
 * RIL_REQUEST_CONFERENCE
 * Conference holding and active (like AT+CHLD=3)
 *
 * @param data
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
void requestConference(void *data, size_t datalen, RIL_Token t)
{
    ULONG nRet = eQCWWAN_ERR_NONE;
    voiceManageCallsReq  manageCallsReq;
    voiceManageCallsResp manageCallsResp;

    memset( &manageCallsReq, 0, sizeof(manageCallsReq));
    memset( &manageCallsResp, 0, sizeof(manageCallsResp));

    /**
     * Release the held call, and make the backgroud call active; if there are
     * not any background calls no active calls would be present after this
     */ 
    manageCallsReq.SUPSType = SUPS_TYPE_MAKE_CONFERENCE_CALL;
    nRet = SLQSVoiceManageCalls (&manageCallsReq, &manageCallsResp);

    if ( nRet != eQCWWAN_ERR_NONE ) {
        LOGD("%s: SLQSVoiceManageCalls() failed %lu\n", __func__, nRet);
        goto error;
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

/**
 * RIL_REQUEST_SEPARATE_CONNECTION
 * Separate a party from a multiparty call placing the multiparty call
 * (less the specified party) on hold and leaving the specified party 
 * as the only other member of the current (active) call
 *
 * @param [in] data
 *     data is an "int *"
 *     (int *)data)[0] contains Connection index (value of 'x' in CHLD above)
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
 *     See TS 22.084 1.3.8.2 (iii)
 *     TS 22.030 6.5.5 "Entering "2X followed by send"
 */
void requestSeparateConnection(void *data, size_t datalen, RIL_Token t)
{
    ULONG nRet = eQCWWAN_ERR_NONE;
    BYTE party = (BYTE)((int *) data)[0];
    voiceManageCallsReq  manageCallsReq;
    voiceManageCallsResp manageCallsResp;

    /* Make sure that party is a single digit. */
    if (party < 1 || party > 9)
        goto error;

    memset( &manageCallsReq, 0, sizeof(manageCallsReq));
    memset( &manageCallsResp, 0, sizeof(manageCallsResp));

    /**
     * Release the held call, and make the backgroud call active; if there are
     * not any background calls no active calls would be present after this
     */ 
    manageCallsReq.SUPSType = SUPS_TYPE_HOLD_ALL_EXCEPT_SPECIFIED_CALL;
    manageCallsReq.pCallID  = &party;
    nRet = SLQSVoiceManageCalls (&manageCallsReq, &manageCallsResp);

    if ( nRet != eQCWWAN_ERR_NONE ) {
        LOGD("%s: SLQSVoiceManageCalls() failed %lu\n", __func__, nRet);
        goto error;
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

/**
 * RIL_REQUEST_EXPLICIT_CALL_TRANSFER
 * Connects the two calls and disconnects the subscriber from both calls
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
void requestExplicitCallTransfer(void *data, size_t datalen, RIL_Token t)
{
    ULONG nRet = eQCWWAN_ERR_NONE;
    voiceManageCallsReq  manageCallsReq;
    voiceManageCallsResp manageCallsResp;

    memset( &manageCallsReq, 0, sizeof(manageCallsReq));
    memset( &manageCallsResp, 0, sizeof(manageCallsResp));

    /**
     * Release the held call, and make the backgroud call active; if there are
     * not any background calls no active calls would be present after this
     */ 
    manageCallsReq.SUPSType = SUPS_TYPE_EXPLICIT_CALL_TRANSFER;
    nRet = SLQSVoiceManageCalls (&manageCallsReq, &manageCallsResp);

    if ( nRet != eQCWWAN_ERR_NONE ) {
        LOGD("%s: SLQSVoiceManageCalls() failed %lu\n", __func__, nRet);
        goto error;
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

/**
 * USSDNotificationCallback
 *    Called when any indication or notification from the Network
 *    [out] USSDResp
 *    Response from the network which need to send back to UI (See ril.h for further information)
 *
 * @param [in] NotificationType
 *    0x1 No further action to be taken
 *    0x2 further action is required
 *
 * @param [in] pNetworkInfo
 *    Response from the network
 *
 * @return
 *     none
 */
void USSDNotificationCallback( ULONG NotificationType, BYTE *pNetworkInfo )
{
    USSDRespFNetwork USSDResp;

    USSDResp.pTypeCode = (char*)malloc(sizeof(char)+1); /* To hold typecode string */

    if( NotificationType == 0x01 )
    {
        /* Further action not required, Send UI as USSD notification*/
        strcpy(USSDResp.pTypeCode, "0");
    }
    else if ( NotificationType == 0x02 )
    {
        /* Further action is required, Send UI as USSD request*/
        strcpy(USSDResp.pTypeCode, "1");
    }
    else
    {
        /* Unsupported */
        strcpy(USSDResp.pTypeCode, "4");
    }

    USSDResp.pRespData = (char *)((struct USSInfo *)pNetworkInfo)->ussData;
    LOGD("%s:: TypeCode:%s Data:%s\n", __func__, USSDResp.pTypeCode, USSDResp.pRespData);

    RIL_onUnsolicitedResponse( RIL_UNSOL_ON_USSD,
                               &USSDResp,
                               sizeof(USSDResp) );
}

/**
 * Register USSD notification call back function
 *
 * @param
 *     none
 *
 * @return
 *     none
 */
void registerUSSDNotificationCallbackQMI(void)
{
    LOGD("%s:: entered\n", __func__);
    ULONG nRet = eQCWWAN_ERR_NONE;

    /* register callback function */
    nRet = SetUSSDNotificationCallback (&USSDNotificationCallback);
    if ( nRet != eQCWWAN_ERR_NONE ) {
        LOGE("%s:: SetUSSDNotificationCallback failed, nRet: %lu\n",
             __func__, nRet );
    }
}

/*
 * USSDReleaseCallback
 *    Called when USSD session terminated by network
 *    [out] USSDResp
 *    Holds data need to send back to UI (See ril.h for further information)
 *
 * @param None
 *
 * @return
 *     none
 */
void USSDReleaseCallback( void )
{
    USSDRespFNetwork USSDResp = { "2", "" }; /* "2" - terminated by network */
                                             /* ""  - No data to UI */
    ussdSessionState = USSD_SESSION_TERMINATED;
    RIL_onUnsolicitedResponse( RIL_UNSOL_ON_USSD,
                               &USSDResp,
                               sizeof(USSDResp) );
}

/**
 * Register USSD release call back function
 *
 * @param
 *     none
 *
 * @return
 *     none
 */
void registerUSSDReleaseCallbackQMI(void)
{
    LOGD("%s:: entered\n", __func__);
    ULONG nRet = eQCWWAN_ERR_NONE;

    /* register callback function */
    nRet = SetUSSDReleaseCallback (&USSDReleaseCallback);
    if ( nRet != eQCWWAN_ERR_NONE ) {
        LOGE("%s:: SetUSSDReleaseCallback failed, nRet: %lu\n",
             __func__, nRet );
    }
}

/**
 * QMI_VOICE_ORIG_USSD_NO_WAIT_IND
 * USSDNoWaitIndicationCallback
 *    Called when USSD No Wait indication received from the Network
 *    [out] USSDResp
 *    Response from the network which need to send back to UI (See ril.h for further information)
 *
 * @param [in] NotificationType
 *    0x1 No further action to be taken
 *    0x2 further action is required
 *
 * @param [in] pNetworkInfo
 *    - pointer to response from the network
 *    - See \ref USSDNoWaitIndicationInfo for more details
 *
 * @return
 *     none
 */
void USSDNoWaitIndicationCallback( USSDNoWaitIndicationInfo *pNetworkInfo )
{
    USSDRespFNetwork USSDResp;

    USSDResp.pTypeCode = (char*)malloc(sizeof(char)+1); /* To hold typecode string */

    LOGD("%s:: entered\n", __func__);

    /* Further action not required, Send UI as USSD notification*/
    strcpy(USSDResp.pTypeCode, "0");

    if ( NULL != pNetworkInfo )
    {
        if( NULL != pNetworkInfo->pUSSDData )
        {
            USSDResp.pRespData = (char *)((pNetworkInfo->pUSSDData)->ussData);

            LOGD("%s:: TypeCode:%s USSD Data:%s\n", __func__, USSDResp.pTypeCode, USSDResp.pRespData);
            RIL_onUnsolicitedResponse( RIL_UNSOL_ON_USSD,
                                       &USSDResp,
                                       sizeof(USSDResp)
                                     );
        }
    }
}

/**
 * Register Voice originate USSD Indication (No Wait) call back function
 *
 * @param
 *     none
 *
 * @return
 *     none
 */
void registerVoiceOrigUSSDNoWaitIndCallbackQMI(void)
{
    LOGD("%s:: entered\n", __func__);
    ULONG nRet = eQCWWAN_ERR_NONE;

    /* register callback function */
    nRet = SetUSSDNoWaitIndicationCallback (&USSDNoWaitIndicationCallback);
    if ( nRet != eQCWWAN_ERR_NONE ) {
        LOGE("%s:: SetUSSDNoWaitIndicationCallback failed, nRet: %lu\n",
             __func__, nRet );
    }
    LOGD("%s:: SetUSSDNoWaitIndicationCallback Success\n",__func__);
}

/*
 * To hold the DDTM preference settings, which will be
 *  used later while setting the DDTM preferences
 */
static struct DDTMSettings prevDDTMSettings;

/**
 * QMI_NAS_DDTM_IND
 * DDTMSettingsIndicationCallback
 *    Called when DDTM settings indication received from the Network
 * @param  [Out] pDDTMIndInfo
 *    - Response from the network. which need to send back to UI (See ril.h for further information)
 *    - See \ref DDTMSettings for further details
 *
 * @return
 *     none
 */
void DDTMSettingsIndicationCallback(struct DDTMSettings  *pDDTMIndInfo)
{
    BYTE i;
    BYTE DDTMIndicationResp[2];

    LOGD("%s:: Entered\n", __func__);

    if( NULL == pDDTMIndInfo ) {
        LOGE("%s:: DDTMSettings is NULL", __func__);
        goto err;
    }

    LOGD("CurrentDDTMStatus : %02X", pDDTMIndInfo->CurrentDDTMStatus);
    LOGD("DDTMPrefSetting: %02X", pDDTMIndInfo->DDTMPref.DDTMPrefSetting);
    LOGD("DDTMAction: %02X", pDDTMIndInfo->DDTMPref.DDTMAction);
    LOGD("SOListAction: %02X", pDDTMIndInfo->DDTMPref.SOListAction);
    LOGD("SOLen : %d", pDDTMIndInfo->DDTMPref.SOLen);
    for(i=0; i<pDDTMIndInfo->DDTMPref.SOLen; i++) {
        LOGD("SO[%d] %04X", i, pDDTMIndInfo->DDTMPref.pServiceOption[i]);
    }
    // copy the present settings into a prevDDTMSettings structure
    prevDDTMSettings.CurrentDDTMStatus = pDDTMIndInfo->CurrentDDTMStatus;
    prevDDTMSettings.DDTMPref.DDTMPrefSetting = pDDTMIndInfo->DDTMPref.DDTMPrefSetting;
    prevDDTMSettings.DDTMPref.DDTMAction = pDDTMIndInfo->DDTMPref.DDTMAction;
    prevDDTMSettings.DDTMPref.SOListAction = pDDTMIndInfo->DDTMPref.SOListAction;
    prevDDTMSettings.DDTMPref.SOLen = pDDTMIndInfo->DDTMPref.SOLen;
    prevDDTMSettings.DDTMPref.pServiceOption = (USHORT *)malloc( pDDTMIndInfo->DDTMPref.SOLen * sizeof(USHORT) );
    for(i=0; i<pDDTMIndInfo->DDTMPref.SOLen; i++) {
        prevDDTMSettings.DDTMPref.pServiceOption[i] = pDDTMIndInfo->DDTMPref.pServiceOption[i];
    }

    DDTMIndicationResp[0] = SWI_IND_DDTM_PREF_INFO;
    DDTMIndicationResp[1] = pDDTMIndInfo->DDTMPref.DDTMPrefSetting;

    RIL_onUnsolicitedResponse( RIL_UNSOL_OEM_HOOK_RAW,
                               DDTMIndicationResp,
                               sizeof(DDTMIndicationResp) );
    err:
    return;
}

/**
 * Register DDTM Indication call back function
 *
 * @param
 *     none
 *
 * @return
 *     none
 */
void registerQmiNasDDTMIndication (void)
{
    LOGD("%s:: entered\n", __func__);
    ULONG nRet = eQCWWAN_ERR_NONE;

    /* register callback function */
    nRet = SetDDTMPrefIndicationCallback ( &DDTMSettingsIndicationCallback );
    if ( nRet != eQCWWAN_ERR_NONE ) {
        LOGE("%s:: SetDDTMPrefIndicationCallback failed, nRet: %lu\n",
             __func__, nRet );
        return;
    }
    LOGD("%s:: SetDDTMPrefIndicationCallback Success\n",__func__);
}

/**
 * Gives the previous settings of DDTM
 *
 * @param [Out] pPrevDDDTPref
 *     - Pointer to the structure where the previous DDTM preferences need to be copied.
 *
 * @return
 *     none
 *
 *  Note: Memory will be allocated dynamically for pServiceOption
 *        (pPrevDDDTPref.DDTMPref.pServiceOption) here according to the SO Length.
 *        Need to be free by the caller after the use.
 */
void getPrevDDTMPreference(struct DDTMSettings* pPrevDDTMPref )
{
    BYTE i;

    /* check for invalid arguments */
    if( NULL == pPrevDDTMPref )
    {
        return;
    }

    // copy the present settings into the given structure pointer
    pPrevDDTMPref->CurrentDDTMStatus = prevDDTMSettings.CurrentDDTMStatus;
    pPrevDDTMPref->DDTMPref.DDTMPrefSetting = prevDDTMSettings.DDTMPref.DDTMPrefSetting;
    pPrevDDTMPref->DDTMPref.DDTMAction = prevDDTMSettings.DDTMPref.DDTMAction;
    pPrevDDTMPref->DDTMPref.SOListAction = prevDDTMSettings.DDTMPref.SOListAction;
    pPrevDDTMPref->DDTMPref.SOLen = prevDDTMSettings.DDTMPref.SOLen;
    /* Allocate memory for ServiceOption, need to be free later after the use. */
    pPrevDDTMPref->DDTMPref.pServiceOption = (USHORT *)malloc( pPrevDDTMPref->DDTMPref.SOLen * sizeof (USHORT) );
    for (i=0; i < pPrevDDTMPref->DDTMPref.SOLen; i++) {
        pPrevDDTMPref->DDTMPref.pServiceOption[i] = prevDDTMSettings.DDTMPref.pServiceOption[i];
    }
}
