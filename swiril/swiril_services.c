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
#include <assert.h>
#include <telephony/ril.h>
#include "swiril_main.h"

#define LOG_TAG "RIL"
#include "swiril_log.h"

/* For QMI */
#include "qmerrno.h"
#include "SWIWWANCMAPI.h"
#include "swiril_misc_qmi.h"
#include "swiril_services.h"

BYTE convertCLIRActiveStatus(BYTE clirActiveStatus);
BYTE convertCLIRProvStatus(BYTE clirProvisionStatus);
void setCLIRVal(int clirVal);

USSDSessionState ussdSessionState = USSD_SESSION_TERMINATED;

#define QMI_CALL_SUPS_ACTIVATE   0x01
#define QMI_CALL_SUPS_DEACTIVATE 0x02
#define QMI_CALL_SUPS_REGISTER   0x03
#define QMI_CALL_SUPS_ERASURE    0x04

#define GPP_CALL_SUPS_DEACTIVATE 0x00
#define GPP_CALL_SUPS_ACTIVATE   0x01
#define GPP_CALL_SUPS_REGISTER   0x03
#define GPP_CALL_SUPS_ERASURE    0x04

#define CALL_WAITING_REASON  0x0F

/* PTCRB 31.2.1.1.2 & 31.2.1.2.2 doesn't allow retry on call forward request */
#ifdef RIL_GCF_TEST_FLAG
#define MAX_RETRY_CALL_FORWARD 0
#else
#define MAX_RETRY_CALL_FORWARD 10
#endif

static const struct timeval TIMEVAL_RETRY_CALL_FORWARD = { 2, 0 };

/* QMI call forwarding reasons */
#define QMI_FWDREASON_UNCONDITIONAL  0x01
#define QMI_FWDREASON_MOBILEBUSY     0x02
#define QMI_FWDREASON_NOREPLY        0x03
#define QMI_FWDREASON_UNREACHABLE    0x04
#define QMI_FWDREASON_ALLFORWARDING  0x05
#define QMI_FWDREASON_ALLCONDITIONAL 0x06

/* 3GPP call forwarding reasons */
#define GPP_FWDREASON_UNCONDITIONAL  0x00
#define GPP_FWDREASON_MOBILEBUSY     0x01
#define GPP_FWDREASON_NOREPLY        0x02
#define GPP_FWDREASON_UNREACHABLE    0x03
#define GPP_FWDREASON_ALLFORWARDING  0x04
#define GPP_FWDREASON_ALLCONDITIONAL 0x05

/* Call Barring reason macros */
#define BARR_ALLOUTGOING             0x07
#define BARR_OUTGOINGINT             0x08
#define BARR_OUTGOINGINTEXTOHOME     0x09
#define BARR_ALLINCOMING             0x0A
#define BARR_INCOMINGROAMING         0x0B
#define BARR_ALLBARRING              0x0C
#define BARR_ALLOUTGOINGBARRING      0x0D
#define BARR_ALLINCOMINGBARRING      0x0E

/**
 * Convert 3GPP call-forwarding modes to QMI ones
 *
 * @param [in] gppCallForwReason
 *     3GPP call forwarding modes
 *
 * @return
 *     QMI call forwarding modes
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 */
BYTE convertCFModeFrom3GPPtoQMI (BYTE gppCallForwMode)
{
    BYTE QmiCallForwMode;
    switch (gppCallForwMode) {
        case GPP_CALL_SUPS_ACTIVATE:
            QmiCallForwMode = QMI_CALL_SUPS_ACTIVATE;
            break;
        case GPP_CALL_SUPS_DEACTIVATE:
            QmiCallForwMode = QMI_CALL_SUPS_DEACTIVATE;
            break;
        case GPP_CALL_SUPS_REGISTER:
            QmiCallForwMode = QMI_CALL_SUPS_REGISTER;
            break;
        case GPP_CALL_SUPS_ERASURE:
            QmiCallForwMode = QMI_CALL_SUPS_ERASURE;
            break;
        default :
            QmiCallForwMode = QMI_CALL_SUPS_ACTIVATE;
            break;
    }
    return QmiCallForwMode;
}

/**
 * Convert 3GPP call-forwarding reasons to QMI ones
 *
 * @param [in] gppCallForwReason
 *     3GPP call forwarding reasons
 *
 * @return
 *     QMI call forwarding reasons
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 */
BYTE convertCFReasonFrom3GPPtoQMI (BYTE gppCallForwReason)
{
    BYTE QmiCallForwReason;
    switch (gppCallForwReason) {
        case GPP_FWDREASON_UNCONDITIONAL:
            QmiCallForwReason = QMI_FWDREASON_UNCONDITIONAL;
            break;
        case GPP_FWDREASON_MOBILEBUSY:
            QmiCallForwReason = QMI_FWDREASON_MOBILEBUSY;
            break;
        case GPP_FWDREASON_NOREPLY:
            QmiCallForwReason = QMI_FWDREASON_NOREPLY;
            break;
        case GPP_FWDREASON_UNREACHABLE:
            QmiCallForwReason = QMI_FWDREASON_UNREACHABLE;
            break;
        case GPP_FWDREASON_ALLFORWARDING:
            QmiCallForwReason = QMI_FWDREASON_ALLFORWARDING;
            break;
        case GPP_FWDREASON_ALLCONDITIONAL:
            QmiCallForwReason = QMI_FWDREASON_ALLCONDITIONAL;
            break;
        default :
            QmiCallForwReason = QMI_FWDREASON_UNCONDITIONAL;
            break;
    }
    return QmiCallForwReason;
}

/**
 * RIL_REQUEST_QUERY_CLIP
 * Queries the status of the CLIP supplementary service.
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
void requestQueryClip(void *data, size_t datalen, RIL_Token t)
{
    CLIPResp clipResp;
    voiceGetCLIPResp resp;
    int response = 2;

    memset( &resp, 0, sizeof(resp) );
    resp.pCLIPResp    = &clipResp;

    ULONG nRet = SLQSVoiceGetCLIP( &resp );
    if (eQCWWAN_ERR_NONE != nRet) {
        LOGE("%s:: SLQSVoiceGetCLIP failed, nRet: %lu\n",__func__, nRet );
        goto error;
    }

    if (NULL != resp.pCLIPResp) {
        response = resp.pCLIPResp->ProvisionStatus;
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, &response, sizeof(int));
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

/**
 * Currently, it is not being used because QMI
 * SDK API for storing CLIR in modem is not available
 * so implementation using AT+CLIR command has been done.
 *
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
void requestGetCLIRQMI(void *data, size_t datalen, RIL_Token t)
{
    CLIRResp clirResp;
    voiceGetCLIRResp resp;
    int response[2] = {0,2};
    ULONG nRet;

    memset( &resp, 0, sizeof(resp) );
    memset( &clirResp, 0, sizeof(clirResp) );
    resp.pCLIRResp    = &clirResp;

    nRet = SLQSVoiceGetCLIR( &resp );
    if (eQCWWAN_ERR_NONE != nRet) {
        LOGE("%s:: SLQSVoiceGetCLIR failed, nRet: %lu\n",__func__, nRet );
        goto error;
    }

    if (NULL != resp.pCLIRResp) {
        response[0] = convertCLIRActiveStatus(resp.pCLIRResp->ActiveStatus);
        response[1] = convertCLIRProvStatus(resp.pCLIRResp->ProvisionStatus);
    }
    /*To be fixed, this is workaround to fix BZ46971 so that CME ERROR: 258 is received from modem before sending the request for call waiting*/
    sleep(2);
    RIL_onRequestComplete(t, RIL_E_SUCCESS, response, sizeof(response));
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

/**
 * Currently, it is not being used because QMI
 * SDK API for storing CLIR in modem is not available
 * so implementation using AT+CLIR command has been done. 
 *
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
 *     CLIR activation is not supported because the 3GPP specification does not
 *     allow CLIR activation; we remember the CLIR type and use the same in
 *     RIL_REQUEST_DIAL
 */
void requestSetCLIRQMI(void *data, size_t datalen, RIL_Token t)
{
    setCLIRVal(((int *) data)[0]);
    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
}

RIL_Token callForwardToken = 0;
RIL_CallForwardInfo gCallForwardInfo;
int counter_retry_call_forward;

void queryCallFwd(void *params) 
{
    requestQueryCallForwardStatus((void*)&gCallForwardInfo, 0, callForwardToken);
}
void setCallFwd(void *params) 
{
    requestSetCallForward((void*)&gCallForwardInfo, 0, callForwardToken);
}

/**
 * RIL_REQUEST_QUERY_CALL_FORWARD_STATUS
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
void requestQueryCallForwardStatus(void *data, size_t datalen, RIL_Token t)
{
    const RIL_CallForwardInfo *pCallForwardInfo = &gCallForwardInfo;
    int n;
    int i = 0;
    ULONG nRet;
    RIL_CallForwardInfo *rilResponse;
    RIL_CallForwardInfo **rilResponseArray;
    /* SLQS structures */
    voiceGetCallFWReq  getCallFWReq;
    voiceGetCallFWResp getCallFWResp;
    getCallFWInfo GetCallFWInfo, *pGetCallFWInfo;
    BYTE SvcClass = 0;

    memcpy(&gCallForwardInfo, data, sizeof(RIL_CallForwardInfo));

    SvcClass = pCallForwardInfo->serviceClass;

    /* Initialize request and response structures */
    memset (&getCallFWReq, 0, sizeof(getCallFWReq));
    memset (&getCallFWResp, 0, sizeof(getCallFWResp));
    memset (&GetCallFWInfo, 0, sizeof(GetCallFWInfo));

    getCallFWReq.Reason = convertCFReasonFrom3GPPtoQMI (pCallForwardInfo->reason);
    getCallFWReq.pSvcClass= &SvcClass;
    getCallFWResp.pGetCallFWInfo = &GetCallFWInfo;

    LOGD("%s:: SLQSVoiceGetCallForwardingStatus serviceClass = %d Reason = %d \n",
                    __func__, *getCallFWReq.pSvcClass, getCallFWReq.Reason );

    nRet = SLQSVoiceGetCallForwardingStatus (&getCallFWReq, &getCallFWResp);
    if (eQCWWAN_ERR_NONE != nRet) {
        LOGE("%s:: SLQSVoiceGetCallForwardingStatus failed, nRet: %lu\n",
                __func__, nRet );
        goto error;
    }

    /* Get the number of occurances */
    pGetCallFWInfo = getCallFWResp.pGetCallFWInfo;
    n = pGetCallFWInfo->numInstances;

    /* Initialize the RIL response structures */
    rilResponse = alloca(n * sizeof(RIL_CallForwardInfo));
    rilResponseArray = alloca(n * sizeof(RIL_CallForwardInfo *));
    memset(rilResponse, 0, sizeof(RIL_CallForwardInfo) * n);

    for (i = 0; i < n; i++) {
        rilResponseArray[i] = &(rilResponse[i]);
    }

    /* Get the Call Fwd infor from SLQS to RIL response structure */
    for (i = 0; i < n; i++) {
        BYTE numberIndex = pGetCallFWInfo->CallFWInfo[i].numLen;
        rilResponse[i].status = pGetCallFWInfo->CallFWInfo[i].SvcStatus;
        rilResponse[i].serviceClass = pGetCallFWInfo->CallFWInfo[i].SvcClass;
        pGetCallFWInfo->CallFWInfo[i].number[numberIndex] = 0;
        rilResponse[i].number = (char *)pGetCallFWInfo->CallFWInfo[i].number;
        rilResponse[i].toa = 145;
        rilResponse[i].timeSeconds = pGetCallFWInfo->CallFWInfo[i].noReplyTimer;
    }
    counter_retry_call_forward = 0;
    RIL_onRequestComplete(t, RIL_E_SUCCESS, rilResponseArray,
                          n * sizeof(RIL_CallForwardInfo *));
    return;

error:
    if (counter_retry_call_forward >= MAX_RETRY_CALL_FORWARD) {
        counter_retry_call_forward = 0;
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    } else {
        callForwardToken = t;
        LOGD("requestQueryCallForwardStatus Failed!, Re-trying...#%d\n\n", counter_retry_call_forward);
        counter_retry_call_forward++;
        enqueueRILEvent(RIL_EVENT_QUEUE_PRIO, queryCallFwd, data, &TIMEVAL_RETRY_CALL_FORWARD);
    }
}

/**
 * RIL_REQUEST_SET_CALL_FORWARD
 * Configure call forward rule.
 *
 * @param [in] data
 *     "data" is const RIL_CallForwardInfo *
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
void requestSetCallForward(void *data, size_t datalen, RIL_Token t)
{
    BYTE SvcClass;
    BYTE noReplyTimer;
    voiceSetSUPSServiceReq  req;
    voiceSetSUPSServiceResp resp;
    ULONG nRet;
    const RIL_CallForwardInfo *callForwardInfo = &gCallForwardInfo;
    static char callForwardNumber[15];

    memcpy(&gCallForwardInfo, data, sizeof(RIL_CallForwardInfo));
    gCallForwardInfo.number = callForwardNumber;

    if (((RIL_CallForwardInfo *)data)->number != NULL) {
        strcpy(callForwardNumber, ((RIL_CallForwardInfo *)data)->number);
    }
    else {
        memset(callForwardNumber,0, sizeof(char)*15);
    }

    memset (&req, 0, sizeof(req));
    memset (&resp, 0, sizeof(resp));

    req.voiceSvc = convertCFModeFrom3GPPtoQMI (callForwardInfo->status);
    req.reason = convertCFReasonFrom3GPPtoQMI (callForwardInfo->reason);

    /* SL9090 does not like svcClass 0 */
    SvcClass = callForwardInfo->serviceClass;
    if (SvcClass) 
        req.pServiceClass = &SvcClass;
    noReplyTimer = callForwardInfo->timeSeconds;
    if (noReplyTimer) 
        req.pTimerVal = &noReplyTimer;     

    if (callForwardInfo->number != NULL) {
        req.pCallForwardingNumber = (BYTE *)callForwardInfo->number;
    }    

    nRet = SLQSVoiceSetSUPSService( &req , &resp );
    if (eQCWWAN_ERR_NONE != nRet) {
        LOGE("%s:: SLQSVoiceSetSUPSService failed, nRet: %lu\n",
             __func__, nRet );
        if (nRet != eQCWWAN_ERR_QMI_SUPS_FAILURE_CAUSE) {
            counter_retry_call_forward = MAX_RETRY_CALL_FORWARD;
        }
        goto error;
    }
    counter_retry_call_forward = 0;
    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    return;

error:
    if (counter_retry_call_forward >= MAX_RETRY_CALL_FORWARD) {
        counter_retry_call_forward = 0;
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    } else {
        callForwardToken = t;
        LOGD("requestSetCallForward Failed!, Re-trying...#%d\n\n", counter_retry_call_forward);
        counter_retry_call_forward++;
        enqueueRILEvent(RIL_EVENT_QUEUE_PRIO, setCallFwd, data, &TIMEVAL_RETRY_CALL_FORWARD);
    }
}

/**
 * RIL_REQUEST_QUERY_CALL_WAITING
 * Query current call waiting state.
 *
 * @param [in] data
 *     "data" is const int *
 *     ((const int *)data)[0] is the TS 27.007 service class to query.
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
void requestQueryCallWaiting(void *data, size_t datalen, RIL_Token t)
{
    int response[2] = { 0, 0 };
    const int class = ((int *) data)[0];
    ULONG nRet;
    int status = 1;
    voiceGetCallWaitInfo getCallWaitInfo;
    BYTE svcClass = 0;
    ccSUPSType CCSUPSType;

    /* Initialize the request/response structures */
    memset (&CCSUPSType, 0, sizeof(CCSUPSType));
    memset (&getCallWaitInfo, 0, sizeof(getCallWaitInfo));

    /* Sierra modem doesn't like class=0, SDK3.X will treat svcClass=0 as NULL */
    svcClass = class;

    getCallWaitInfo.pSvcClass = &svcClass;
    getCallWaitInfo.pCCSUPSType = &CCSUPSType;

    nRet = SLQSVoiceGetCallWaiting (&getCallWaitInfo);
    if (eQCWWAN_ERR_NONE != nRet) {
        LOGE("%s::SLQSVoiceGetCallWaiting failed, nRet: %lu\n",__func__, nRet );
        goto error;
    }

    LOGD("%s::SLQSVoiceGetCallWaiting  svcType:%d svcClass:%d \n",__func__, getCallWaitInfo.pCCSUPSType->svcType,svcClass );

    /* neglect svcType parameter, it is not return in SL9090 */
    /* Get the status - active or inactive */
    /* if (getCallWaitInfo.pCCSUPSType->svcType == 0x01) {
        status = 1;
    } else if (getCallWaitInfo.pCCSUPSType->svcType == 0x02) {
        status = 0;
    }*/

    /* Fill the service class information in the response structure */
    if (status == 1 && svcClass > 0 && svcClass <= 128) {
        response[1] |= svcClass;
    }

    if (response[1] > 0) {
        response[0] = 1;
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, response, sizeof(int) * 2);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

/**
 * RIL_REQUEST_SET_CALL_WAITING
 * Configure current call waiting state.
 *
 * @param [in] data
 *     "data" is const int *
 *     ((const int *)data)[0] is 0 for "disabled" and 1 for "enabled"
 *     ((const int *)data)[1] is the TS 27.007 service class bit vector of
 *     services to modify
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
void requestSetCallWaiting(void *data, size_t datalen, RIL_Token t)
{
    voiceSetSUPSServiceReq  req;
    voiceSetSUPSServiceResp resp;
    BYTE svcClass = 0;
    ULONG nRet;
    const int mode = ((int *) data)[0];
    const int class = ((int *) data)[1];

    memset( &req, 0, sizeof(req) );
    memset( &resp, 0, sizeof(resp) );

    if( (mode<0) || (mode>1) ) {
        goto error;
    }

    if (mode == 0) {
        req.voiceSvc = QMI_CALL_SUPS_DEACTIVATE;
    } else {
        req.voiceSvc = QMI_CALL_SUPS_ACTIVATE;
    }

    LOGD("%s:: SLQSVoiceSetSUPSService mode:%d svcClass:%d \n",__func__,mode,class );

    req.reason = CALL_WAITING_REASON;
    
    /* Sierra modem doesn't like class = 0, don't pass svcClass parameter if svcClass=0 */
    svcClass = class;
    if (svcClass)
        req.pServiceClass = &svcClass;

    nRet = SLQSVoiceSetSUPSService( &req , &resp );
    if (eQCWWAN_ERR_NONE != nRet) {
        LOGE("%s:: SLQSVoiceSetSUPSService failed, nRet: %lu\n",
             __func__, nRet );
        goto error;
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}


/**
 * RIL_REQUEST_DTMF_START
 * Starts DTMF tones.
 *
 * @param [in] data
 *     "data" is const int *
 *     ((const int *)data)[0] is 0 for "disabled" and 1 for "enabled"
 *     ((const int *)data)[1] is the TS 27.007 service class bit vector of
 *     services to modify
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
void requestStartDTMF(void *data, size_t datalen, RIL_Token t)
{
    ULONG nRet;
    unsigned char callId = 0xff;
    voiceContDTMFinfo getContDTMFInfo;
   
    memset (&getContDTMFInfo, 0, sizeof(getContDTMFInfo));
    getContDTMFInfo.pCallID = &callId;
    getContDTMFInfo.DTMFdigit= *((char*)data);
    nRet = SLQSVoiceStartContDTMF(&getContDTMFInfo);
    
    if(eQCWWAN_ERR_NONE != nRet)
    {
       LOGE("%s :: SLQSVoiceStartContDTMF failed, nRet: %lu\n",__func__,nRet );
       goto error;
    }
    
    RIL_onRequestComplete(t, RIL_E_SUCCESS,NULL, 0);
       return;

    error:
       RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);

}

/**
 * RIL_REQUEST_DTMF_STOP
 * DTMF tones stop.
 *
 * @param [in] data
 *     "data" is const int *
 *     ((const int *)data)[0] is 0 for "disabled" and 1 for "enabled"
 *     ((const int *)data)[1] is the TS 27.007 service class bit vector of
 *     services to modify
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
void requestStopDTMF(void *data, size_t datalen, RIL_Token t)
{   
    ULONG nRet;
    voiceStopContDTMFinfo getStopContDTMFInfo;

    memset (&getStopContDTMFInfo, 0, sizeof(getStopContDTMFInfo));
    unsigned char callId = 0xff;
    getStopContDTMFInfo.callID = callId;

    nRet = SLQSVoiceStopContDTMF(&getStopContDTMFInfo);

    if(eQCWWAN_ERR_NONE != nRet)
    {
       LOGE("%s :: SLQSVoiceStopContDTMF failed, nRet: %lu\n",__func__,nRet );
       goto error;
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS,NULL, 0);
       return;

    error:
       RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

/**
 * RIL_REQUEST_CDMA_BURST_DTMF
 * Configure Burst DTMF
 *
 *
 * "data" is const char **
 * ((const char **)data)[0] is a DTMF string
 * ((const char **)data)[1] is the DTMF ON length in milliseconds, or 0 to use
 *                          default
 * ((const char **)data)[2] is the DTMF OFF length in milliseconds, or 0 to use
 *                          default
 *
 * "response" is NULL
 *
 *
 * @note
 *     none
 */
void requestBurstDTMF(void *data, size_t datalen, RIL_Token t)
{ 
    ULONG nRet;
    unsigned char callId = 0xff;
    voiceBurstDTMFInfo getBurstDTMFInfo;
    const char *DTMFString = ((char **)data)[0];
    const char *DTMFOnLen  = ((char **)data)[1];
    const char *DTMFOffLen = ((char **)data)[2];

    memset (&getBurstDTMFInfo, 0, sizeof(getBurstDTMFInfo));
    LOGE("%s :: DTMF string %s DTMF on len %d DTMF off len %d \n",__func__,DTMFString, (int)(*DTMFOnLen),(int)(*DTMFOffLen));

    /* send to the current active call when call_id is set to 0xFF */
    getBurstDTMFInfo.BurstDTMFInfo.pCallID=&callId;
    /* send 1 digit, DTMFon/offLen are optional, omit to use default */
    getBurstDTMFInfo.BurstDTMFInfo.digitCnt = 1;
    getBurstDTMFInfo.BurstDTMFInfo.pDigitBuff[0] = DTMFString[0];

    nRet = SLQSVoiceBurstDTMF(&getBurstDTMFInfo);

    if(eQCWWAN_ERR_NONE != nRet)
    {
       LOGE("%s :: SLQSVoiceBurstDTMF failed, nRet: %lu\n",__func__,nRet );
       goto error;
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS,NULL, 0);
    return;

    error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

/* Workaround for SL9090 for SWI6600U_02.00.00.02 : BZ48558 */
#define BASEBAND_SL9090_UMTS "SWI6600U"
char fmrString[NUM_MAX_STR_LEN];

/**
 * RIL_REQUEST_SEND_USSD
 * Sends USSD command.
 *
 * @param [in] data
 *     "data" is const int *
 *     ((const int *)data)[0] is the TS 27.007 service class to query.
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
void requestSendUSSD(void *data, size_t datalen, RIL_Token t)
{
    ULONG nRet;
    voiceOrigUSSDNoWaitInfo OrigUSSDNoWaitInfo;
    struct USSInfo USSDUserResp;
    const BYTE *uss_data = (BYTE *) data;
    const USHORT data_len = strlen( (char *)uss_data );

    /* Workaround for SL9090 for SWI6600U_02.00.00.02 : BZ48558 */
    nRet = GetFirmwareRevision(sizeof(fmrString), &fmrString[0]);
    if (nRet == 0) {
        if (0 != strstr(fmrString,BASEBAND_SL9090_UMTS)) {
            requestSendUSSD_AT(data, datalen, t);
            return;
        }
    }

    if( (data_len > MAXUSSDLENGTH) || (data_len <= 0) )
    {
        LOGE("%s::Invalid USSD Command\n",__func__);
        goto error;
    }
    LOGD("%s::USSD Command: %s Length : %d\n", __func__, uss_data, data_len );

    if ( USSD_SESSION_TERMINATED == ussdSessionState )
    {
        /* USSD session not initiated, initiate USSD session */
        ussdSessionState = USSD_SESSION_INITIATED;

        /* Fill the OrigUSSDNoWaitInfo to send SDK*/
        OrigUSSDNoWaitInfo.USSInformation.ussDCS = 0x01; /* ASCII coding scheme */
        OrigUSSDNoWaitInfo.USSInformation.ussLen = (BYTE)data_len;
        strcpy( (char *)OrigUSSDNoWaitInfo.USSInformation.ussData, (char *)data );

        nRet = SLQSVoiceOrigUSSDNoWait ( &OrigUSSDNoWaitInfo );
        if (eQCWWAN_ERR_NONE != nRet)
        {
            LOGE("%s::SLQSVoiceOrigUSSDNoWait failed, nRet: %lu\n",__func__, nRet );
            goto error;
        }
        LOGD("%s::SLQSVoiceOrigUSSDNoWait Success\n",__func__ );

        RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    }

    else if ( USSD_SESSION_INITIATED == ussdSessionState)
    {
        /* USSD session already initiated, send response */

        /* Fill the USSDUserResp to send SDK*/
        USSDUserResp.ussDCS = 0x01; /* ASCII coding scheme */
        USSDUserResp.ussLen = data_len;
        strcpy( (char*)USSDUserResp.ussData, (char *)data );

        LOGD("%s::USSD User Response : %s\n",__func__, USSDUserResp.ussData );
        nRet = AnswerUSSD((BYTE*)&USSDUserResp );
        if (eQCWWAN_ERR_NONE != nRet)
        {
            LOGE("%s::AnswerUSSD failed, nRet: %lu\n",__func__, nRet );
            goto error;
        }
        LOGD("%s::AnswerUSSD Success\n",__func__ );

        RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    }

    return;

error:
    ussdSessionState = USSD_SESSION_TERMINATED;
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

/**
 * RIL_REQUEST_CANCEL_USSD
 * Cancels USSD session.
 *
 * @param [in] none
 *
 * @return
 *     none
 */
void requestCancelUSSD(void *data, size_t datalen, RIL_Token t)
{
    ULONG nRet;

    /* Workaround for SL9090 for SWI6600U_02.00.00.02 : BZ48558 */
    nRet = GetFirmwareRevision(sizeof(fmrString), &fmrString[0]);
    if ( eQCWWAN_ERR_NONE == nRet ) {
        if ( strstr(fmrString, BASEBAND_SL9090_UMTS) ) {
            requestCancelUSSD_AT(data, datalen, t);
            return;
        }
    }

    nRet = CancelUSSD();
    if (eQCWWAN_ERR_NONE != nRet) {
        LOGE("%s::CancelUSSD failed, nRet: %lu\n",__func__, nRet );
        goto error;
    }

    LOGD("%s::CancelUSSD  Success\n",__func__ );

    ussdSessionState = USSD_SESSION_TERMINATED;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    return;

    error:
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

/**
 * RIL_REQUEST_CHANGE_BARRING_PASSWORD
 * Changes call barring password.
 *
 * @param [in] data
 *     "data" is const char **
 *     ((const char **)data)[0] = facility string code from TS 27.007 7.4
 *     (eg "AO" for BAOC)
 *     ((const char **)data)[1] = old password
 *     ((const char **)data)[2] = new password
 *
 * @param datalen
 *     length of the data parameter received
 * @param t
 *     token passed to the RIL command request
 *
 * @return
 *     none
 */
void requestChangeBarringPassword(void *data, size_t datalen, RIL_Token t)
{
    ULONG nRet;
    const char *facilityStr = ((char **)data)[0];
    const char *oldPassword = ((char **)data)[1];
    const char *newPassword = ((char **)data)[2];
    voiceSetCallBarringPwdInfo SetCallBarringPwdInfo;
    voiceSetCallBarringPwdResp SetCallBarringPwdResp;
    WORD barrFailureCause;

    LOGD("%s::Entered\n", __func__);
    LOGD("%s::Facility String : %s\n", __func__, facilityStr );
    LOGD("%s::Old Password    : %s\n", __func__, oldPassword );
    LOGD("%s::New Password    : %s\n", __func__, newPassword );

    /* Initialize the request/response structures */
    memset (&SetCallBarringPwdInfo, 0, sizeof(SetCallBarringPwdInfo));
    memset (&SetCallBarringPwdResp, 0, sizeof(SetCallBarringPwdResp));
    SetCallBarringPwdResp.pFailCause    = &barrFailureCause;
    SetCallBarringPwdResp.pAlphaIDInfo  = NULL;
    SetCallBarringPwdResp.pCCResType    = NULL;
    SetCallBarringPwdResp.pCallID       = NULL;
    SetCallBarringPwdResp.pCCSUPSType   = NULL;

    /* Set call barring reason */
    if ( strstr("AO", facilityStr) )
        SetCallBarringPwdInfo.Reason = BARR_ALLOUTGOING;
    else if ( strstr("OI", facilityStr) )
        SetCallBarringPwdInfo.Reason = BARR_OUTGOINGINT;
    else if ( strstr("AI", facilityStr) )
        SetCallBarringPwdInfo.Reason = BARR_ALLINCOMING;
    else if ( strstr("IR", facilityStr) )
        SetCallBarringPwdInfo.Reason = BARR_INCOMINGROAMING;
    else if ( strstr("OX", facilityStr) )
        SetCallBarringPwdInfo.Reason = BARR_OUTGOINGINTEXTOHOME;
    else if ( strstr("AB", facilityStr) )
        SetCallBarringPwdInfo.Reason = BARR_ALLBARRING;
    else if ( strstr("AG", facilityStr) )
        SetCallBarringPwdInfo.Reason = BARR_ALLOUTGOINGBARRING;
    else if ( strstr("AC", facilityStr) )
        SetCallBarringPwdInfo.Reason = BARR_ALLINCOMINGBARRING;
    else {
        LOGE("%s:: Invalid Barring reason : %s\n", __func__, facilityStr );
        goto error;
    }

    memcpy( SetCallBarringPwdInfo.oldPasswd,      oldPassword, PASSWORD_LENGTH );
    memcpy( SetCallBarringPwdInfo.newPasswd,      newPassword, PASSWORD_LENGTH );
    memcpy( SetCallBarringPwdInfo.newPasswdAgain, newPassword, PASSWORD_LENGTH );

    nRet = SLQSVoiceSetCallBarringPassword(&SetCallBarringPwdInfo, &SetCallBarringPwdResp);
    if (eQCWWAN_ERR_NONE != nRet) {
        LOGE("%s::SLQSVoiceSetCallBarringPassword failed, nRet: %lu\n",__func__, nRet );
        LOGE("SetCallBarringPwdResp.pFailCause: %d\n",barrFailureCause );
        goto error;
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

/**
 * convertUTF8ToUCS2Str
 * Converts UTF8 string to UCS2 Hex string.
 *
 * @param [in] src
 *     pointer to UTF8 string to be converted
 * @param [out]dest
 *     pointer to UCS2 hex string where coveted string will be placed.
 *
 * @return
 *     none
 *
 */
static void convertUTF8ToUCS2Str_AT(const char* src, char* dest)
{
    int length = 0;

    length = strlen(src);

    while (length>0)
    {
        sprintf(dest,"%04X",*src++);
        length--;
        dest = dest+4;
    }
}

#include "at_channel.h"
#include "at_tok.h"

/**
 * RIL_REQUEST_SEND_USSD
 * Send a USSD message
 *
 * @param [in] data
 *     "data" is USSD request as a string
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
 *     See also: RIL_REQUEST_CANCEL_USSD, RIL_UNSOL_ON_USSD
 *     requestSendUSSD() sends USSD command in UCS2 format always.
 */
void requestSendUSSD_AT(void *data, size_t datalen, RIL_Token t)
{
    const char *ussdRequest;
    char *cmd = NULL;
    char *newCmd = NULL;
    int err = -1;
    ATResponse *response = NULL;
    char* ptr = NULL;

    char* USSDReqUCS2 = NULL;

    LOGD("%s:: USSD Command : %s\n", __func__, (char *)data);

    /* Change character coding scheme to UCS2 for sendign USSD in UCS2 format */
    err = at_send_command("AT+CSCS=\"UCS2\"", &response);
    at_response_free(response);
    if (err < 0 || response->success == 0)
         goto error;

    ussdRequest = (char *) (data);

    /* allocate memory for USSDReqUCS2 */
    USSDReqUCS2 = malloc(strlen(ussdRequest)*4 +1);
    memset(USSDReqUCS2,0,strlen(ussdRequest)*4 +1);

    /* convert UTF8 to UCS2 */
    convertUTF8ToUCS2Str_AT(ussdRequest,USSDReqUCS2);

    asprintf(&cmd, "AT+CUSD=1,\"%s\",15", USSDReqUCS2);

    LOGD("%s:: USSD command in UCS2 : %s\n", __func__, cmd);

    err = at_send_command(cmd, &response);
    free(cmd);
    free(USSDReqUCS2);

    if (err < 0 || response->success == 0)
            goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

finally:
    if (response != NULL)
        at_response_free(response);

    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_CANCEL_USSD
 * Cancel the current USSD session if one exists
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
void requestCancelUSSD_AT(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err;

    err = at_send_command("AT+CUSD=2", &atresponse);

    if (err < 0 || atresponse->success == 0) {
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    } else {
        RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    }
    at_response_free(atresponse);
    return;
}
