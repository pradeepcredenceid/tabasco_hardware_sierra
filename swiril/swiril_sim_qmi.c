/**
 *
 * @ingroup swiril
 *
 * @file 
 * Provides miscellaneous Sierra specific functions
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
#include <stdbool.h>
#include <cutils/properties.h>

#include "swiril_main.h"
#include "swiril_cache.h"
#include "swiril_sim_qmi.h"
#include "swiril_sim.h"
#include "swiril_misc_qmi.h"
#include "swiril_device_qmi.h"
#include "swiril_network_qmi.h"

#include "at_channel.h"
#include "at_tok.h"

#include "aa/aaglobal.h"
#include "qmerrno.h"
#include "qmudefs.h"
#include "SWIWWANCMAPI.h"

#define LOG_TAG "RIL"
#include "swiril_log.h"

#define IMSI_STRING_LENGTH 16       /**< IMSI string length including NULL */
#define IMEI_SV_STRING_LENGTH 2     /**< IMEI SV string length including NULL */

/* SIM related defines */
#define SIM_PIN1 1
#define SIM_PIN2 2
#define SIM_PUK1 1
#define SIM_PUK2 2
#define BASEBAND_SL9090_UMTS "SWI6600U"

extern unsigned char nib2char(unsigned char c);
extern int binaryToString(/*in*/ const unsigned char *binary,
                   /*in*/ size_t len,
                   /*out*/ char *string);
extern int stringToShort(/*in*/ const char *string,
                  /*in*/ size_t len,
                  /*out*/ unsigned short *pshort);
extern int stringToBinary(/*in*/ const char *string,
                   /*in*/ size_t len,
                   /*out*/ unsigned char *binary);
extern int convertSimIoFcp(RIL_SIM_IO_Response *sr, char **cvt);
extern bool is3GSim( /* in */ const char *stream );
extern bool isFirmwareRunningCDMA();
static const struct timeval TIMEVAL_SIMPOLL = { 1, 0 };

/* Should SIM_READY be sent.  This is used as part of the logic to delay
 * sending SIM_READY so that it does not conflict with registration.
 */
static bool s_pendingSIMReady = false;

typedef enum {
    FDN_NOT_AVAILABLE,
    FDN_AVAILABLE_DISABLED,
    FDN_AVAILABLE_ENABLED
} FDN_Status;

typedef enum {
    SIM_FILE_TRANSPARENT,
    SIM_FILE_CYCLIC,
    SIM_FILE_LINEAR_FIXED,    
    SIM_FILE_DEDICATED,
    SIM_FILE_MASTER
} Sim_file_type;
/**
 * Enter the SIM PIN.
 *
 * @param simPINId
 *     the SIM PIN Id carrying value of either 1 or 2
 * @param [in] pSIMPIN
 *     the SIM PIN to be entered
 * @param t
 *     the RIL token identifier 
 *
 * @return
 *     none
 */
static void enterSIMPIN( ULONG simPINId, CHAR *pSIMPIN, RIL_Token t )
{
    ULONG     nRet;
    ULONG     verifyRetriesLeft = 0;
    ULONG     unblockRetriesLeft = 0;
    UIMVerifyPinReq req;
    UIMPinResp      resp;
    remainingRetries retryCnt;
    RIL_Errno errNumber = RIL_E_GENERIC_FAILURE;

    /* Default is 1 second, same as TIMEVAL_SIMPOLL */
    struct timeval timeval_simpoll = { 1, 0 };

    /* If the property exists, overwrite the default */
    CHAR propValue[PROPERTY_VALUE_MAX];
    int intpropvalue;

    if (property_get("persist.sierra.pin_delay", propValue, NULL) > 0) {
    	intpropvalue = atoi(propValue);
        LOGI("%s: pindelay=%i", __func__, intpropvalue);
        timeval_simpoll.tv_sec = intpropvalue;
    }

    memset(&req, 0, sizeof(UIMVerifyPinReq));
    memset(&resp, 0, sizeof(UIMPinResp));
    if(isFirmwareRunningCDMA())    
        req.sessionInfo.sessionType=1;
    else
        req.sessionInfo.sessionType=0;
    req.sessionInfo.aidLength=0;
    req.verifyPIN.pinID = simPINId;
    req.verifyPIN.pinLen = strlen(pSIMPIN);
    memcpy(req.verifyPIN.pinVal, pSIMPIN, req.verifyPIN.pinLen);
    resp.pRemainingRetries = &retryCnt;
    
    nRet = SLQSUIMVerifyPin(&req, &resp);

    if(retryCnt.verifyLeft != 0xFF)
        verifyRetriesLeft = retryCnt.verifyLeft;

    /* Success */
    if (nRet == eQCWWAN_ERR_NONE) {
        LOGD( "enterSIMPIN Success" );

        /* Got OK, so return success and start SIM polling process so that the
           RADIO state can be updated once the SIM is ready. While not actually
           required, wait a short time before starting the polling process,
           since the SIM won't be ready right away anyways, and thus we avoid
           unnecessary polling.
         */
        resetpollSIMRetry();
        enqueueRILEvent( RIL_EVENT_QUEUE_PRIO,
                         pollSIMState,
                         NULL,
                         &timeval_simpoll );

        /* Return the successful response with number of retries left */
        RIL_onRequestComplete( t,
                               RIL_E_SUCCESS,
                               (int *)&verifyRetriesLeft,
                               sizeof(int *) );
    }
    /* Error */
    else {
        LOGE("enterSIMPIN error: %lu", nRet);

        /* Set the appropriate error code */
        if (nRet ==  eQCWWAN_ERR_QMI_INCORRECT_PIN){
            errNumber = RIL_E_PASSWORD_INCORRECT;
            RIL_onRequestComplete( t, errNumber, &verifyRetriesLeft, sizeof(int *));
        }
        else
            RIL_onRequestComplete( t, errNumber, NULL, 0);
    }
}

/**
 * Authenticate SIM PIN2. *
 * 
 * @param [in] pSIMPIN2
 *     the SIM PIN2 to be verified
 *
 * @return
 *     none
 */
ULONG authenticateSIMPIN2QMI(char *pSIMPIN2)
{
    ULONG verifyRetriesLeft = 0;
    ULONG unblockRetriesLeft = 0;
    ULONG nRet;
    nRet = UIMVerifyPIN ( 2,
                          pSIMPIN2, /* PIN2 value */
                          &verifyRetriesLeft,
                          &unblockRetriesLeft );
    return nRet;
}

/**
 * Enter the SIM PUK.
 *
 * @param simPUKId
 *     the SIM PUK Id carrying value of either 1 or 2
 * @param [in] pSIMPUK
 *     the SIM PUK to be entered
 * @param [in] pSIMPIN
 *     the SIM PIN to be entered
 * @param t
 *     the RIL token identifier 
 *
 * @return
 *     none
 */
static void enterSIMPUK(
    ULONG     simPUKId,
    CHAR      *pSIMPUK,
    CHAR      *pSIMPIN,
    RIL_Token t )
{
    ULONG     nRet;
    ULONG     verifyRetriesLeft = 0;
    ULONG     unblockRetriesLeft = 0;
    UIMUnblockPinReq req;
    UIMPinResp       resp;
    remainingRetries retryCnt;
    RIL_Errno errNumber = RIL_E_GENERIC_FAILURE;

    /* Verify the requested PUK value */
    memset(&req, 0, sizeof(UIMUnblockPinReq));
    memset(&resp, 0, sizeof(UIMPinResp));
    
    if(isFirmwareRunningCDMA())    
        req.sessionInfo.sessionType=1;
    else
        req.sessionInfo.sessionType=0;
    req.sessionInfo.aidLength=0;
    req.unblockPIN.pinID = simPUKId;
    req.unblockPIN.pukLen = strlen(pSIMPUK);
    memcpy(req.unblockPIN.pukVal, pSIMPUK, req.unblockPIN.pukLen);
    req.unblockPIN.newPINLen = strlen(pSIMPIN);
    memcpy(req.unblockPIN.newPINVal, pSIMPIN, req.unblockPIN.newPINLen);
    resp.pRemainingRetries = &retryCnt;
    
    nRet = SLQSUIMUnblockPin(&req, &resp);
    
    if(retryCnt.unblockLeft != 0xFF)
        unblockRetriesLeft = retryCnt.unblockLeft;
                           
    /* Success */
    if (nRet == eQCWWAN_ERR_NONE) {
        LOGD( "enterSIMPUK Success" );

        /* Got OK, so return success and start SIM polling process so that the
           RADIO state can be updated once the SIM is ready. While not actually
           required, wait a short time before starting the polling process,
           since the SIM won't be ready right away anyways, and thus we avoid
           unnecessary polling.
         */
        resetpollSIMRetry(); 
        enqueueRILEvent( RIL_EVENT_QUEUE_PRIO,
                         pollSIMState,
                         NULL,
                         &TIMEVAL_SIMPOLL );

        /* Return the successful response with number of retries left */
        RIL_onRequestComplete( t,
                               RIL_E_SUCCESS,
                               (int *)&unblockRetriesLeft,
                               sizeof(int *) );
    }
    /* Error */
    else {
        LOGE("enterSIMPUK error: %lu", nRet);

        /* Set the appropriate error code */
        if (nRet == eQCWWAN_ERR_QMI_INCORRECT_PIN){
            errNumber = RIL_E_PASSWORD_INCORRECT;
            RIL_onRequestComplete( t, errNumber, &unblockRetriesLeft, sizeof(int *));
        }
        else
            RIL_onRequestComplete( t, errNumber, NULL, 0);
    }
}

/**
 * To get the status of card (session type, pin status etc).
 *
 * @param pSessionType
 *     session type
 * @param pPin1Status
 *     status of pin 1 i.e. whether enabled or disaled.
 *
 * @return
 *     none
 */
ULONG getCardStatusQMI( int *pSessionType, int *pPin1Status )
{
    local cardStatus CardStatus;
    local hotSwapStatus HotSwapStatus;
    CardStatus.indexGwPri = 0;
    CardStatus.indexGwSec =0;
    BYTE tc = 0;
    ULONG nRet=0;
    UIMGetCardStatusResp resp;
    WORD lcount,lIcount;
    UIMEventRegisterReqResp reqresp;

    memset(&reqresp, 0, sizeof(UIMEventRegisterReqResp));
    memset(&CardStatus, 0, sizeof(cardStatus));
    memset(&HotSwapStatus, 0, sizeof(hotSwapStatus));
    reqresp.eventMask =0x00000001; /* bit 0 needs to be set to get the card status */
    LOGD("Entered %s",__func__);

    /* Event needs to be registered before getting the status of card */
    nRet = SLQSUIMEventRegister(&reqresp);
    if ( eQCWWAN_ERR_NONE == nRet || nRet == eQCWWAN_ERR_QMI_NO_EFFECT ) {
        resp.pCardStatus = &CardStatus;
        resp.pHotSwapStatus = &HotSwapStatus;
        nRet = SLQSUIMGetCardStatus( &resp );
        if ( eQCWWAN_ERR_NONE == nRet ) {
            LOGD("SLQSUIMGetCardStatus Successful\n");
            if ( NULL != resp.pCardStatus ) {
                /* If any session is active then value other than 0xFFFF is returned depending on app type*/
                if (resp.pCardStatus->indexGwPri != 0xFFFF)
                    *pSessionType = 0;
                else if (resp.pCardStatus->index1xPri != 0xFFFF)
                    *pSessionType = 1;
                else if (resp.pCardStatus->indexGwSec != 0xFFFF)
                    *pSessionType = 2;
                else if (resp.pCardStatus->index1xSec != 0xFFFF)
                    *pSessionType = 3;
                else
                    LOGD("%s: Error in getting session information",__func__);
                for ( lcount=0 ; lcount < resp.pCardStatus->numSlot; lcount++ ) {
                    slotInfo *temp = &resp.pCardStatus->SlotInfo[lcount];
                    for ( lIcount=0 ; lIcount < temp->numApp; lIcount++ ) {
                        appStatus *lresp = &temp->AppStatus[lIcount];
                        /* status of pin 1 whether enabled or disabled */
			*pPin1Status = lresp->pin1State; 
                    }
                }
            }
        }
	else
	    LOGD("%s: SLQSUIMGetCardStatus failed",__func__);
    }
    else
        LOGD("%s: SLQSUIMEventRegister failed",__func__);
    return nRet;
           
}

/**
 * Set the FDN service.
 *
 * @param [in] sessionType
 *     session type
 * @param [in] fdnStatus
 *     status of fdn i.e. whether to enable or disable.
 *
 * @return
 *     none
 */
ULONG setFDNServiceQMI(int sessionType, int fdnStatus)
{   
    ULONG nRet;  
    UIMSetServiceStatusReq req;
    memset(&req, 0, sizeof(req));
    req.sessionInfo.sessionType = sessionType;
    req.pFDNStatus = (BYTE*)&fdnStatus;
    /* Set the status of FDN */
    nRet = SLQSUIMSetServiceStatus(&req);
    return nRet;
}

/**
 * Get the FDN status.
 *
 * @param [in] sessionType
 *     session type
 * @param [out] pFDNStatus
 *     status of fdn i.e. whether enabled or disabled.
 *
 * @return
 *     none
 */
ULONG getFDNStatusQMI(int sessionType, int *pFDNStatus) 
{
    ULONG nRet;
    UIMGetServiceStatusReq req;
    UIMGetServiceStatusResp resp;
    BYTE fdnStatus;
    memset(&req, 0, sizeof(req));
    memset(&resp, 0, sizeof(resp));
    
    resp.pFDNStatus = &fdnStatus;
    req.sessionInfo.sessionType = sessionType;
    /* Bit 0 needs to be set to get the status of FDN, can be modified later if status of any 
       other service is required.
     */
    req.capMask = 0x00000001; 
    /* Get the status of FDN */
    nRet = SLQSUIMGetServiceStatus( &req,&resp );
    if (nRet == eQCWWAN_ERR_NONE) {
        *pFDNStatus = *(resp.pFDNStatus);
    }
    return nRet;
}

/**
 * Change the SIM PIN.
 *
 * @param simPINId
 *     the SIM PIN Id carrying value of either 1 or 2
 * @param [in] pSIMPINOld
 *     the old SIM PIN
 * @param [in] pSIMPINNew
 *     the new SIM PIN
 * @param t
 *     the RIL token identifier 
 *
 * @return
 *     none
 */
static void changeSIMPIN(
    ULONG     simPINId,
    CHAR      *pSIMPINOld,
    CHAR      *pSIMPINNew,
    RIL_Token t )
{
    ULONG     nRet;
    ULONG     verifyRetriesLeft = 0;
    ULONG     unblockRetriesLeft = 0;
    UIMChangePinReq req;
    UIMPinResp      resp;
    remainingRetries retryCnt;

    RIL_Errno errNumber = RIL_E_GENERIC_FAILURE;

    /* Verify the requested PUK value */
    memset(&req, 0, sizeof(UIMChangePinReq));
    memset(&resp, 0, sizeof(UIMPinResp));
    if(isFirmwareRunningCDMA())    
        req.sessionInfo.sessionType=1;
    else
        req.sessionInfo.sessionType=0;
    req.sessionInfo.aidLength=0;    
    req.changePIN.pinID = simPINId;
    req.changePIN.oldPINLen = strlen(pSIMPINOld);
    memcpy(req.changePIN.oldPINVal, pSIMPINOld, req.changePIN.oldPINLen);
    req.changePIN.pinLen = strlen(pSIMPINNew);
    memcpy(req.changePIN.pinVal, pSIMPINNew, req.changePIN.pinLen);
    resp.pRemainingRetries = &retryCnt;
    
    nRet = SLQSUIMChangePin(&req, &resp);
    
    if(retryCnt.verifyLeft != 0xFF)
        verifyRetriesLeft = retryCnt.verifyLeft;

    /* Success */
    if (nRet == eQCWWAN_ERR_NONE) {
        LOGD( "changeSIMPIN Success" );
        /* Return the successful response with number of retries left */
        RIL_onRequestComplete( t,
                               RIL_E_SUCCESS,
                               (int *)&verifyRetriesLeft,
                               sizeof(int *) );
    }
    /* Error */
    else {
        LOGE("changeSIMPIN error: %lu", nRet);

        /* Set the appropriate error code */
        if (nRet ==  eQCWWAN_ERR_QMI_INCORRECT_PIN){
            errNumber = RIL_E_PASSWORD_INCORRECT;
            RIL_onRequestComplete( t, errNumber, &verifyRetriesLeft, sizeof(int *));
        }
	else if (nRet ==  eQCWWAN_ERR_QMI_PIN_BLOCKED){
            errNumber = RIL_E_SIM_PUK2;
            RIL_onRequestComplete( t, errNumber, &verifyRetriesLeft, sizeof(int *));
        }
        else
            RIL_onRequestComplete( t, errNumber, NULL, 0);
    }
}


/**
 * RIL_REQUEST_GET_IMSI
 */
void requestGetIMSIQMI(void *data, size_t datalen, RIL_Token t)
{
    ULONG nRet;
    char imsiString[IMSI_STRING_LENGTH];
    char imsiStringPrint[IMSI_STRING_LENGTH];
        
    memset(imsiStringPrint, 'x', IMSI_STRING_LENGTH - 1);
    imsiStringPrint[IMSI_STRING_LENGTH - 1] = 0;

    nRet = GetIMSI(sizeof(imsiString), &imsiString[0]);
    if (nRet == eQCWWAN_ERR_NONE) {
        /* only print out first 6 digits */
        strncpy(imsiStringPrint, imsiString, 6);
        LOGD("requestGetIMSIQMI IMSI: %s\n", imsiStringPrint);
        /* Return the successful response and cache the value for next time */
        swiril_cache_RIL_onRequestComplete(t, RIL_E_SUCCESS, 
                                           &imsiString[0],
                                           sizeof(char *),
                                           RIL_REQUEST_GET_IMSI);
    }
    else {
        /* retry to get IMSI with AT command */
        ATResponse *atresponse = NULL;
        int err;

        err = at_send_command_numeric("AT+CIMI", &atresponse);

        if (err < 0 || atresponse->success == 0) {
            LOGE("%s GetIMSI error: %lu", __func__, nRet);
            RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
            /* SWI_TBD when GetIMSI failed with eQCWWAN_ERR_QMI_SIM_NOT_INITIALIZED,
               GetIMSI will always failed with same return, just reset modem for now */
            if ((nRet == eQCWWAN_ERR_QMI_SIM_NOT_INITIALIZED)&&(isDualModeModule())) {
                LOGE("%s reset modem", __func__);
                setPowerOff();
             }
        } else {
            /* Return the successful response and cache the value for next time */
            swiril_cache_RIL_onRequestComplete(t, RIL_E_SUCCESS, 
                                               atresponse->p_intermediates->line,
                                               sizeof(char *),
                                               RIL_REQUEST_GET_IMSI);
        }
        at_response_free(atresponse);
   
    }
}

/**
 * RIL_REQUEST_GET_IMEI
 */
void requestGetIMEIQMI(void *data, size_t datalen, RIL_Token t)
{
    ULONG nRet;
    CHAR  ESNString[ESN_STRING_LENGTH];
    CHAR  IMEIString[IMEI_STRING_LENGTH];
    CHAR  MEIDString[MEID_STRING_LENGTH];
        
    nRet = GetSerialNumbers(sizeof(ESNString),
                            &ESNString[0],
                            sizeof(IMEIString),
                            &IMEIString[0],
                            sizeof(MEIDString),
                            &MEIDString[0]); 
    if (nRet == eQCWWAN_ERR_NONE) {
        LOGD("requestGetIMEIQMI ESN: %s, IMEI: %s, MEID: %s\n", ESNString, IMEIString, MEIDString);
        /* Return the successful response and cache the value for next time */
        swiril_cache_RIL_onRequestComplete(t, RIL_E_SUCCESS,
                                           &IMEIString[0],
                                           sizeof(char *),
                                           RIL_REQUEST_GET_IMEI);
    } else {
        LOGE("requestGetIMEIQMI: GetSerialNumbers() error: %lu", nRet);
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    }
}

/**
 * RIL_REQUEST_GET_IMEISV
 */
void requestGetIMEISVQMI(void *data, size_t datalen, RIL_Token t)
{
    int err;
    char *response = NULL;
        
    err = getIMEISV(&response);
    if (err < 0) {
        LOGE("%s:: getIMEISV failed, err number: %d", __func__, err);
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE,
                              NULL,
                              0);
    } else {
        swiril_cache_RIL_onRequestComplete(t, RIL_E_SUCCESS,
                                       response,
                                       sizeof(char *),
                                       RIL_REQUEST_GET_IMEISV);
    }
    if (response != NULL)
        free(response);
}

/**
 * RIL_REQUEST_DEVICE_IDENTITY
 */
void requestGetDeviceIdentityQMI(void *data, size_t datalen, RIL_Token t)
{
    ULONG nRet;
    CHAR  ESNString[ESN_STRING_LENGTH];
    CHAR  IMEIString[IMEI_STRING_LENGTH];
    CHAR  MEIDString[MEID_STRING_LENGTH];
    char* response[4];
    int   err;
        
    nRet = GetSerialNumbers(sizeof(ESNString),
                            &ESNString[0],
                            sizeof(IMEIString),
                            &IMEIString[0],
                            sizeof(MEIDString),
                            &MEIDString[0]); 
    if (nRet == eQCWWAN_ERR_NONE) {
        LOGD("requestGetDeviceIdentityQMI ESN: %s, IMEI: %s, MEID: %s\n", ESNString, IMEIString, MEIDString);
        /* Return the successful response and cache the value for next time */
        
        /* UMTS related */
        response[0] = &IMEIString[0];

        response[1] = NULL;
        err = getIMEISV(&response[1]);
        if (err < 0) {
            LOGE("%s:: getIMEISV failed, err number: %d", __func__, err);
        }
        
        /* CDMA related */
        response[2] = &ESNString[0];
        response[3] = &MEIDString[0];
        swiril_cache_RIL_onRequestComplete(t, RIL_E_SUCCESS, 
                                           &response,
                                           sizeof(response),
                                           RIL_REQUEST_DEVICE_IDENTITY);
    } else {
        LOGE("requestGetDeviceIdentityQMI error: %lu", nRet);
        goto error;
    }

finally:
    if (response[1] != NULL)
        free(response[1]);
    return;
error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_ENTER_SIM_PIN
 */
void requestEnterSIMPINQMI(void *data, size_t datalen, RIL_Token t)
{
    enterSIMPIN(SIM_PIN1,
                ((CHAR **)data)[0], /* SIM PIN */
                t);
}

/**
 * RIL_REQUEST_ENTER_SIM_PIN2
 */
void requestEnterSIMPIN2QMI(void *data, size_t datalen, RIL_Token t)
{
    enterSIMPIN( SIM_PIN2,
                 ((CHAR **)data)[0], /* SIM PIN 2 */
                 t );
}

/**
 * RIL_REQUEST_ENTER_SIM_PUK
 */
void requestEnterSIMPUKQMI(void *data, size_t datalen, RIL_Token t)
{
    enterSIMPUK( SIM_PUK1,
                 ((CHAR **)data)[0], /* PUK 1 */
                 ((CHAR **)data)[1], /* PIN 1 */
                 t );
}

/**
 * RIL_REQUEST_ENTER_SIM_PUK2
 */
void requestEnterSIMPUK2QMI(void *data, size_t datalen, RIL_Token t)
{
    enterSIMPUK( SIM_PUK2,
                 ((CHAR **)data)[0], /* PUK 2 */
                 ((CHAR **)data)[1], /* PIN 2 */
                 t );
}

/**
 * RIL_REQUEST_CHANGE_SIM_PIN
 */
void requestChangeSIMPINQMI(void *data, size_t datalen, RIL_Token t)
{
    changeSIMPIN( SIM_PIN1,
                  ((CHAR **)data)[0], /* current SIM PIN */
                  ((CHAR **)data)[1], /* new SIM PIN */
                  t );
}

/**
 * RIL_REQUEST_CHANGE_SIM_PIN2
 */
void requestChangeSIMPIN2QMI(void *data, size_t datalen, RIL_Token t)
{
    changeSIMPIN( SIM_PIN2,
                  ((CHAR **)data)[0], /* current SIM PIN 2 */
                  ((CHAR **)data)[1], /* new SIM PIN 2 */
                  t );
}

/**
 * Fill the bytes to send the response to COMMAND_GET_RESPONSE after converting it
 * to specific format required by framework (bytes to string).
 */
ULONG getFileAttributesQMI(RIL_SIM_IO *pIoArg,
                           RIL_SIM_IO_Response *pSr)
{
    ULONG nRet;
    UIMGetFileAttributesReq req;
    UIMGetFileAttributesResp resp;
    int sessionType=0;
    int pin1Status;
    fileAttributes fileAttri;
    cardResult cardRes;
    char *getResponse;
    unsigned short res = 0;
    unsigned short file_size =0,file_id =0;
    unsigned char file_type =0;
    unsigned short rec_size;
    unsigned char chres = 0;
    char fmrString[0xff];

    LOGD("%s:Entered \n",__func__);
    memset(&cardRes, 0, sizeof(cardRes));
    memset(&fileAttri, 0, sizeof(fileAttri));
    memset(&req, 0, sizeof(UIMGetFileAttributesReq));
    memset(&resp, 0, sizeof(UIMGetFileAttributesResp));
    
    /* To get the session information */
    nRet = getCardStatusQMI(&sessionType,&pin1Status);
    if ( eQCWWAN_ERR_NONE != nRet )
    {        
        LOGD("%s:getCardStatusQMI failed: nRet %lu",__func__,nRet);
        return nRet;
    }
    req.sessionInfo.sessionType = sessionType;
    req.fileIndex.fileID = pIoArg->fileid;  
    /* Divided by 2 because framework sends it in string format */       
    req.fileIndex.pathLen = strlen(pIoArg->path)/2; 
    resp.pFileAttributes = &fileAttri;
    resp.pCardResult = &cardRes;
    stringToShort(pIoArg->path,strlen(pIoArg->path),req.fileIndex.path);
    LOGD("%s:file_id: %d,path: %s",__func__,pIoArg->fileid,pIoArg->path);

    /* get the following values from get file attributes
    file_size : 2 bytes
    file_id: 2 bytes
    file_type : 1 byte
    rec_size : 2 bytes (liner fixed and cyclic only)
    rec_count : 2 bytes (linear fixed only) */
    nRet = SLQSUIMGetFileAttributes(&req,&resp);
    if(nRet != eQCWWAN_ERR_NONE) {
        LOGD("%s:SLQSUIMGetFileAttributes failed, nRet: %lu",__func__,nRet);
        /* 
        || Work around for issue CUS72958, sending dummy response for GET RESPONSE command for Voice mail
        || related EFs
        */
        if ((nRet != eQCWWAN_ERR_QMI_SIM_FILE_NOT_FOUND) &&
            ((pIoArg->fileid == 28615) || (pIoArg->fileid == 28617) || (pIoArg->fileid == 28439))) {
            if (!GetFirmwareRevision(sizeof(fmrString), &fmrString[0])) {
                if (0 != strstr(fmrString,BASEBAND_SL9090_UMTS)) {
                    LOGD("%s: sending dummy response for Voice Mail related EFs",__func__);
                    getResponse = malloc(31);
                    if ((pIoArg->fileid == 28615) || (pIoArg->fileid == 28439))
                        strcpy(getResponse,"000000406FC7040000000001020120");
                    if (pIoArg->fileid == 28617)
                        strcpy(getResponse,"000000086FC9040000000001020104");
                    pSr->sw1 = 0x90;
                    pSr->sw2 = 0x00;
                    pSr->simResponse = getResponse;
                    LOGD("%s:sending dummy simResponse for Voice Mail related EFs: %s",__func__,getResponse);
    	            LOGD("%s:SLQSUIMGetFileAttributes: sw1:%x,sw2:%x",__func__,pSr->sw1,pSr->sw2);
                    return eQCWWAN_ERR_NONE;
                }
            }
            return nRet;
        }
        else
            return nRet;
    }

    /* Response to COMMAND_GET_RESPONSE is of 30 bytes + 1 byte for NULL character */
    getResponse = malloc( 30 + 1);
 
    /* Initialize memory with '0'= 0x30 or 'F' */
    memset(getResponse, '0', 31);
    getResponse[30] = 0; /* NULL char */
   
    /* Fill the values for file_size */
    res  = resp.pFileAttributes->fileSize;
    getResponse[4] = nib2char((char)((res >> 12) & 0x000F)); 
    getResponse[5] = nib2char((char)((res >> 8) & 0x000F));
    getResponse[6] = nib2char((char)((res >> 4) & 0x000F));
    getResponse[7] = nib2char((char)(res & 0x000F));

    /* Fill the values for file_id 2 bytes at index 8,9,10,11 */
    res = resp.pFileAttributes->fileID;
    getResponse[8] = nib2char((char)((res >> 12) & 0x000F)); 
    getResponse[9] = nib2char((char)((res >> 8) & 0x000F));
    getResponse[10] = nib2char((char)((res >> 4) & 0x000F));
    getResponse[11] = nib2char((char)(res & 0x000F)); 
    file_type = resp.pFileAttributes->fileType;

    /* Fill the values for file_type at 12,13 and 
     * data_structure at 26,27 (not needed for MF and DF) 
     */
    switch (file_type) {
        case SIM_FILE_TRANSPARENT:
            chres = 4; /* file_type 4 is for EF, check in IccFileHandler.java */
            getResponse[12] = nib2char((chres >> 4) & 0x0F);
            getResponse[13] = nib2char(chres & 0x0F); 

            chres = 0; /* data struct is 0 for transparent , check in IccfIleHandler.java */
            getResponse[26] = nib2char((chres >> 4) & 0x0F);
            getResponse[27] = nib2char(chres & 0x0F);
            break;

        case SIM_FILE_LINEAR_FIXED:
            chres = 4; /* file_type 4 is for EF, check in IccFileHandler.java */
            getResponse[12] = nib2char((chres >> 4) & 0x0F);
            getResponse[13] = nib2char(chres & 0x0F); 

            chres = 1; /* data struct is 1 for linear fixed , check in IccfIleHandler.java */
            getResponse[26] = nib2char((chres >> 4) & 0x0F);
            getResponse[27] = nib2char(chres & 0x0F);
            break;
        case SIM_FILE_CYCLIC:
            chres = 4; /* file_type 4 is for EF, check in IccFileHandler.java */
            getResponse[12] = nib2char((chres >> 4) & 0x0F);
            getResponse[13] = nib2char(chres & 0x0F); 
            chres = 3; /* data struct is 3 for cyclic , check in IccfIleHandler.java */
            getResponse[26] = nib2char((chres >> 4) & 0x0F);
            getResponse[27] = nib2char(chres & 0x0F);
            break;

        case SIM_FILE_DEDICATED:
            chres = 2; /* file_type 2 is for DF, check in IccFileHandler.java */
            getResponse[12] = nib2char((chres >> 4) & 0x0F);
            getResponse[13] = nib2char(chres & 0x0F); 
            
            break;

        case SIM_FILE_MASTER:
            chres = 1; /* file_type 1 is for MF, check in IccFileHandler.java */
            getResponse[12] = nib2char((chres >> 4) & 0x0F);
            getResponse[13] = nib2char(chres & 0x0F); 
            break;
        default:
            break;
    }
 
    /* If file_type is linear fixed or cyclic fixed, put record_size */
    if ((file_type == SIM_FILE_LINEAR_FIXED) || (file_type == SIM_FILE_CYCLIC)) {
        res  = resp.pFileAttributes->recordSize;
        getResponse[28] = nib2char((char)((res >> 4) & 0x000F));
        getResponse[29] = nib2char((char)(res & 0x000F)); 
    }
    LOGD("%s:simResponse: %s",__func__,getResponse);
    LOGD("%s:SLQSUIMGetFileAttributes: sw1:%x,sw2:%x",__func__,resp.pCardResult->sw1,resp.pCardResult->sw2);
    pSr->sw1 = resp.pCardResult->sw1;
    pSr->sw2 = resp.pCardResult->sw2;
    pSr->simResponse = getResponse;
    return nRet;
}

/**
 * Read the data from LINEAR_FIXED or CYCLIC EF	from SIM and send 
 * it in specific format required by framework (bytes to string).
 */
ULONG readRecordQMI( RIL_SIM_IO *pIoArg,
                     RIL_SIM_IO_Response *pSr )
{
    cardResult cardRes;
    readResult readRes;
    UIMReadRecordReq req;
    UIMReadRecordResp resp;
    ULONG nRet=0;
    int sessionType=0;
    int pin1Status;
    char *simResponse;
    LOGD("%s:Entered \n",__func__);
  
    memset(&req, 0, sizeof(UIMReadRecordReq));
    memset(&resp, 0, sizeof(UIMReadRecordResp));

    /* To get the session information */
    nRet = getCardStatusQMI(&sessionType,&pin1Status);
    if ( eQCWWAN_ERR_NONE != nRet )
    {
        LOGD("%s:getCardStatusQMI failed: nRet %lu",__func__,nRet);
        return nRet;
    }

    req.sessionInfo.sessionType = sessionType;
    req.fileIndex.fileID = pIoArg->fileid;
   
    if (pIoArg->path) 
    {   
        /* Divided by 2 because framework sends it in string format */
        req.fileIndex.pathLen = strlen(pIoArg->path)/2;
        stringToShort(pIoArg->path,strlen(pIoArg->path),req.fileIndex.path);
    }
    req.readRecord.record = pIoArg->p1;
    req.readRecord.length = pIoArg->p3;
    LOGD("%s:file_id: %d,path: %s,record: %d,length: %d",__func__,pIoArg->fileid,pIoArg->path,pIoArg->p1,pIoArg->p3);
    memset(&cardRes, 0, sizeof(cardResult));
    memset(&readRes, 0, sizeof(readResult));
    resp.pCardResult = &cardRes;
    resp.pReadResult = &readRes;

    nRet = SLQSUIMReadRecord (&req, &resp);
    if ( eQCWWAN_ERR_NONE != nRet )
    {
        LOGD("%s:SLQSUIMReadRecord failed: nRet %lu",__func__,nRet);
        return nRet;
    }
    simResponse = malloc((2*(resp.pReadResult->contentLen))+1);
    binaryToString(resp.pReadResult->content,resp.pReadResult->contentLen,simResponse);
    LOGD("%s:simResponse: %s",__func__,simResponse);
    LOGD("%s:SLQSUIMReadRecord sw1: %x sw2: %x\n",__func__,resp.pCardResult->sw1, resp.pCardResult->sw2);
    pSr->sw1 = resp.pCardResult->sw1;
    pSr->sw2 = resp.pCardResult->sw2;
    pSr->simResponse = simResponse;
    return nRet;          
}

/**
 * Update the data in LINEAR_FIXED or CYCLIC EF	in SIM .
 */
ULONG updateRecordQMI( RIL_SIM_IO *pIoArg,
                      RIL_SIM_IO_Response *pSr )
{
    cardResult cardRes;
    UIMWriteRecordReq req;
    UIMWriteRecordResp resp;
    ULONG nRet=0;
    int sessionType=0,i;
    int pin1Status;

    LOGD("%s:Entered \n",__func__);
    memset(&req, 0, sizeof(UIMWriteRecordReq));
    memset(&resp, 0, sizeof(UIMWriteRecordResp));
    memset(&cardRes, 0, sizeof(cardResult));
    resp.pCardResult = &cardRes;
     
    nRet = getCardStatusQMI(&sessionType,&pin1Status);
    if ( eQCWWAN_ERR_NONE != nRet )
    {
        LOGD("%s:getCardStatusQMI failed: nRet %lu",__func__,nRet);
        return nRet;
    }

    req.sessionInfo.sessionType = sessionType;
    req.fileIndex.fileID = pIoArg->fileid;
    if (pIoArg->path) 
    {   
        /* Divided by 2 because framework sends it in string format */
        req.fileIndex.pathLen = strlen(pIoArg->path)/2;
        stringToShort(pIoArg->path,strlen(pIoArg->path),req.fileIndex.path);
    }
   
    if (pIoArg->data) 
    {   
        req.writeRecord.record = pIoArg->p1;
        req.writeRecord.dataLen = strlen(pIoArg->data)/2;
        stringToBinary(pIoArg->data,strlen(pIoArg->data),req.writeRecord.data);
    }
    LOGD("%s:data: %s",__func__,pIoArg->data);
    LOGD("%s:file_id: %d,path: %s,record: %d,length: %d",__func__,pIoArg->fileid,pIoArg->path,pIoArg->p1,pIoArg->p3);
    nRet = SLQSUIMWriteRecord (&req, &resp);
    if ( eQCWWAN_ERR_NONE != nRet )
    {
        LOGD("%s:SLQSUIMWriteRecord failed: nRet %lu",__func__,nRet);
        return nRet;
    }
    
    LOGD("%s:SLQSUIMWriteRecord sw1: %x sw2: %x\n", __func__,resp.pCardResult->sw1, resp.pCardResult->sw2);
    pSr->sw1 = resp.pCardResult->sw1;
    pSr->sw2 = resp.pCardResult->sw2;
    pSr->simResponse = NULL;
    return nRet;    

}

/**
 * Read the data from TRANSPARENT EF from SIM and send 
 * it in specific format required by framework (bytes to string).
 */
ULONG readBinaryQMI( RIL_SIM_IO *pIoArg,
                     RIL_SIM_IO_Response *pSr )
{
    cardResult cardRes;
    readResult readRes;
    ULONG  nRet=0;
    int sessionType=0;
    int pin1Status;
    char *simResponse;
    UIMReadTransparentReq req;
    UIMReadTransparentResp resp;
    LOGD("%s:Entered \n",__func__);
  
    memset(&req, 0, sizeof(UIMReadTransparentReq));
    memset(&resp, 0, sizeof(UIMReadTransparentResp));

    nRet = getCardStatusQMI(&sessionType,&pin1Status);
    if ( eQCWWAN_ERR_NONE != nRet )
    {
        LOGD("%s:getCardStatusQMI failed: nRet %lu",__func__,nRet);
        return nRet;
    }
    req.sessionInfo.sessionType = sessionType;
    req.fileIndex.fileID = pIoArg->fileid;
   
    if (pIoArg->path) 
    {   
        /* Divided by 2 because framework sends it in string format */
        req.fileIndex.pathLen = strlen(pIoArg->path)/2;
        stringToShort(pIoArg->path,strlen(pIoArg->path),req.fileIndex.path);
    }
    req.readTransparent.offset = pIoArg->p1;
    req.readTransparent.length = pIoArg->p3;
    memset(&cardRes, 0, sizeof(cardResult));
    memset(&readRes, 0, sizeof(readResult));
    resp.pCardResult = &cardRes;
    resp.pReadResult = &readRes;

    LOGD("%s:file_id: %d,path: %s,record: %d,length: %d",__func__,pIoArg->fileid,pIoArg->path,pIoArg->p1,pIoArg->p3);
    nRet = SLQSUIMReadTransparent (&req, &resp);
    if ( eQCWWAN_ERR_NONE != nRet )
    {
        LOGD("%s:SLQSUIMReadTransparent failed: nRet %lu",__func__,nRet);
        return nRet;
    }
    simResponse = malloc((2*(resp.pReadResult->contentLen))+1);
    binaryToString(resp.pReadResult->content,resp.pReadResult->contentLen,simResponse);
    LOGD("%s:simResponse: %s",__func__,simResponse);
    LOGD("%s: SLQSUIMReadTransparent sw1: %x sw2: %x\n", __func__,resp.pCardResult->sw1, resp.pCardResult->sw2);
    pSr->sw1 = resp.pCardResult->sw1;
    pSr->sw2 = resp.pCardResult->sw2;
    pSr->simResponse = simResponse;
    return nRet;
}

/**
 * Update the data in TRANSPARENT EF in SIM .
 */
ULONG updateBinaryQMI( RIL_SIM_IO *pIoArg,
                       RIL_SIM_IO_Response *pSr )
{
    cardResult cardRes;
    UIMWriteTransparentReq req;
    UIMWriteTransparentResp resp;
    ULONG nRet=0;
    int sessionType=0,i;
    int pin1Status;

    LOGD("%s:Entered \n",__func__);
    memset(&req, 0, sizeof(UIMWriteTransparentReq));
    memset(&resp, 0, sizeof(UIMWriteTransparentResp));
    memset(&cardRes, 0, sizeof(cardResult));
    resp.pCardResult = &cardRes;
     
    nRet = getCardStatusQMI(&sessionType,&pin1Status);
    if ( eQCWWAN_ERR_NONE != nRet )
    {
        LOGD("%s:getCardStatusQMI failed: nRet %lu",__func__,nRet);
        return nRet;
    }

    req.sessionInfo.sessionType = sessionType;
    req.fileIndex.fileID = pIoArg->fileid;
   
    if (pIoArg->path) 
    {   
        /* Divided by 2 because framework sends it in string format */
        req.fileIndex.pathLen = strlen(pIoArg->path)/2;
        stringToShort(pIoArg->path,strlen(pIoArg->path),req.fileIndex.path);
    }
   
    if (pIoArg->data) 
    {   
        req.writeTransparent.offset = pIoArg->p1;
        req.writeTransparent.dataLen = strlen(pIoArg->data)/2;
        stringToBinary(pIoArg->data,strlen(pIoArg->data),req.writeTransparent.data);
    }
    LOGD("%s:data: %s",__func__,pIoArg->data);
    LOGD("%s:file_id: %d,path: %s,record: %d,length: %d",__func__,pIoArg->fileid,pIoArg->path,pIoArg->p1,pIoArg->p3);
    nRet = SLQSUIMWriteTransparent (&req, &resp);
    if ( eQCWWAN_ERR_NONE != nRet )
    {
        LOGD("%s:SLQSUIMWriteTransparent failed: nRet %lu",__func__,nRet);
        return nRet;
    }
    
    LOGD("%s: SLQSUIMWriteTransparent sw1: %x sw2: %x\n", __func__,resp.pCardResult->sw1, resp.pCardResult->sw2);
    pSr->sw1 = resp.pCardResult->sw1;
    pSr->sw2 = resp.pCardResult->sw2;
    pSr->simResponse = NULL;
    return nRet;    
}
/**
 * Call-back function to send delayed SIM_READY notification.
 */
static void delayedSIMReady(void *param)
{
    if (s_pendingSIMReady) {
        s_pendingSIMReady = false;

        LOGD("%s: set RADIO_STATE_SIM_READY", __func__);
        setRadioState(RADIO_STATE_SIM_READY);
    } else {
        LOGV("%s: SIM_READY already sent", __func__);
    }
}

/**
 * Determine whether a delayed SIM_READY notification should be used
 */
bool useDelayedSIMReady(void)
{
    ULONG nRet = 0;
    struct getServingNetworkPara servingNetworkPara;

    /* Assume there will be no delayed SIM_READY */
    s_pendingSIMReady = false;

    /* Check the current registration state.  If the modem is already
     * registered, SIM_READY can be sent right away.
     */
    memset(&servingNetworkPara, 0, sizeof(servingNetworkPara));
    servingNetworkPara.radioIfacesSize = RAT_MAX;
        
    nRet = GetServingNetwork(&servingNetworkPara.registrationState,
                             &servingNetworkPara.CSDomain,
                             &servingNetworkPara.PSDomain,
                             &servingNetworkPara.RAN,
                             &servingNetworkPara.radioIfacesSize,
                             &servingNetworkPara.radioIfaces[0],
                             &servingNetworkPara.roamingInd,
                             &servingNetworkPara.MCC,
                             &servingNetworkPara.MNC,
                             sizeof(servingNetworkPara.name),
                             &servingNetworkPara.name[0]);


    if (nRet == eQCWWAN_ERR_NONE) {
        LOGD("%s registrationState  : %lu\n", __func__, servingNetworkPara.registrationState);
        if(servingNetworkPara.registrationState == QMI_REG_STATE_REGISTERED)
            return false;
    } else {
        /* Log an error and assume not currently registered */
        LOGE("%s: GetServingNetwork error: %lu", __func__, nRet);
    }


    /* If the property exists, delay the SIM_READY notification
     * by the specified time.
     */
    char propValue[PROPERTY_VALUE_MAX];

    if (property_get("persist.sierra.sim_ready_delay", propValue, NULL) > 0) {
        /* Default is 0 second */
        struct timeval timeval_simready = { 0, 0 };
        int intpropvalue;

        intpropvalue = atoi(propValue);
        LOGI("%s: sim_ready_delay=%i", __func__, intpropvalue);
        timeval_simready.tv_sec = intpropvalue;

        enqueueRILEvent(RIL_EVENT_QUEUE_PRIO, delayedSIMReady, NULL,
                    &timeval_simready);

        s_pendingSIMReady = true;
        return true;
    }

    return false;
}

/*
 * If there is still a pending SIM_READY notification, then sent it now.
 * Normally called when modem becomes registered.
 */
void sendPendingSIMREADY(void)
{
    delayedSIMReady(NULL);
}

/**
 * Try using QMI API to get retry count for PIN.
 *
 * Currently only support PIN1.  May be expanded to other values later.
 */
int getPINRetryCountQMI(ULONG id)
{
    int num_retries = -1;
    ULONG nRet;
    ULONG Status;
    ULONG VerifyRetriesLeft;
    ULONG UnblockRetriesLeft;

    nRet = UIMGetPINStatus ( id, &Status, &VerifyRetriesLeft, &UnblockRetriesLeft);
    if (nRet == eQCWWAN_ERR_NONE) {
        num_retries = VerifyRetriesLeft;
        LOGI("%s: success, retry count = %d", __func__, num_retries);
    } else
        LOGE("%s: error, nRet = %lu", __func__, nRet);

    return num_retries;
}

/** 
 * Reset s_pendingSIMReady
 *
 */
void ResetPendingSIMReady(void)
{
    s_pendingSIMReady = false;
}

/**
 * RIL_REQUEST_SET_FACILITY_LOCK
 *
 * Enable/disable one facility lock.
 */
void requestSetFacilityLockQMI(void *data, size_t datalen, RIL_Token t)
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
    unsigned long nRet;
    int n=0;
    ULONG verifyRetriesLeft = 0;
    ULONG unblockRetriesLeft = 0;
    int sessionType=0;
    int pin1Status;
    RIL_Errno rilerr = RIL_E_GENERIC_FAILURE;

    assert(datalen >= (4 * sizeof(char **)));

    facility_string = ((char **) data)[0];
    facility_mode_str = ((char **) data)[1];
    facility_password = ((char **) data)[2];
    facility_class = ((char **) data)[3];

    assert(*facility_mode_str == '0' || *facility_mode_str == '1');
    facility_mode = atoi(facility_mode_str);
    
    
    if( !strcmp(facility_string, "FD")) {
        /* To authenticate PIN2 */
        nRet = authenticateSIMPIN2QMI(facility_password);
        if (nRet == eQCWWAN_ERR_NONE) {
            /* It is invoked to get the session information */
            nRet = getCardStatusQMI(&sessionType,&pin1Status);
	    if (nRet != eQCWWAN_ERR_NONE) 
	        goto error;

            /* Enable/Disable the FDN service */
	    nRet = setFDNServiceQMI(sessionType,facility_mode);
	    if (nRet != eQCWWAN_ERR_NONE) 
	        goto error;	   
	}
	
        else if (nRet == eQCWWAN_ERR_QMI_INCORRECT_PIN || nRet == eQCWWAN_ERR_QMI_PIN_BLOCKED){
            /* Get the retry count for PIN2 */
	    num_retries = getPINRetryCountQMI( SIM_PIN2 );
	    LOGD("%s: num_retries= %d",
                   __func__, num_retries);
            /* If no of retries for PIN2 is 1 or 2 then pwd incorrect error will be returned and
               if it is 0 then it means PIN2 is blocked so PUK2 required error will be returned.
             */
	    if (num_retries) 
                rilerr = RIL_E_PASSWORD_INCORRECT;
	    else
	        rilerr = RIL_E_SIM_PUK2;
            goto error;
        }
	else
	    goto error; /* If there is some other error then GENERIC_FAILURE should be returned */
    }
    if(!strcmp(facility_string, "SC")) {
        /* Enbale/Disable PIN1 */
        nRet = UIMSetPINProtection(SIM_PIN1,
			  facility_mode, 
			  facility_password, /* PIN1 value */
                          &verifyRetriesLeft,
                          &unblockRetriesLeft);
        
        num_retries = verifyRetriesLeft;
        if (nRet == eQCWWAN_ERR_QMI_INCORRECT_PIN ) {
           LOGD("%s: num_retries= %d\n",
                   __func__, num_retries);
           /* If no of retries for PIN1 is 1 or 2 then pwd incorrect error will be returned */
	   if (num_retries)
               rilerr = RIL_E_PASSWORD_INCORRECT;
           goto error;
        }
     
        if (nRet != eQCWWAN_ERR_NONE) 
	    goto error; /* If there is some other error then GENERIC_FAILURE should be returned */
    }
    
    /* Return the successful response with number of retries left */
    RIL_onRequestComplete(t, RIL_E_SUCCESS, &num_retries, sizeof(int *));
    return;

error:
    RIL_onRequestComplete( t,
                           rilerr,
                           &num_retries,
                           sizeof(int *) );

    /* If number of retry is 0, send out RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED 
        which will trigger up layer to send GET_SIM_STATUS request */
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
void requestQueryFacilityLockQMI(void *data, size_t datalen, RIL_Token t)
{
    int err, response;
    char *facility_string = NULL;
    char *facility_password = NULL;
    char *facility_class = NULL;
    int sessionType=0;
    int pin1Status=0;
    int fdnStatus;
    ULONG nRet;
    RIL_Errno rilerr = RIL_E_GENERIC_FAILURE;

    assert(datalen >= (3 * sizeof(char **)));

    facility_string = ((char **) data)[0];
    facility_password = ((char **) data)[1];
    facility_class = ((char **) data)[2];

   
    if( !strcmp(facility_string, "SC")) {
        /* To get the status of PIN 1 */
        nRet = getCardStatusQMI(&sessionType,&pin1Status);
	if(nRet == eQCWWAN_ERR_NONE) {
	    if (pin1Status == RIL_PINSTATE_DISABLED) 
	        response = 0;
	    else if (pin1Status == RIL_PINSTATE_ENABLED_NOT_VERIFIED || 
                     pin1Status == RIL_PINSTATE_ENABLED_VERIFIED)
	        response = 1;
            else
                goto error;
        }
	else
            goto error;
    }
    
    if( !strcmp(facility_string, "FD")) {
        /* It is invoked to get the session information */
        nRet = getCardStatusQMI(&sessionType,&pin1Status);
	if (nRet != eQCWWAN_ERR_NONE) 
	    goto error;
        /* To get the status of FDN */
        nRet = getFDNStatusQMI(sessionType, &fdnStatus);
	if(nRet == eQCWWAN_ERR_NONE) {
	    if (fdnStatus == FDN_AVAILABLE_DISABLED) 
	        response = 0;
	    else if (fdnStatus == FDN_AVAILABLE_ENABLED)
	        response = 1;
            else
                goto error;
        }
        else
	    goto error;
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, &response, sizeof(int));
    return;

error:
    RIL_onRequestComplete(t, rilerr, NULL, 0);
}

/**
 * RIL_REQUEST_SIM_IO
 *
 * Request SIM I/O operation.
 * This is similar to the TS 27.007 "restricted SIM" operation
 * where it assumes all of the EF selection will be done by the
 * callee.
 */
void requestSIM_IOQMI(void *data, size_t datalen, RIL_Token t)
{
    ULONG nRet;
    RIL_SIM_IO *ioargs;
    RIL_SIM_IO_Response sr;
    RIL_Errno rilerr = RIL_E_GENERIC_FAILURE;
    ioargs = (RIL_SIM_IO *) data;
    int err,len,cvt_done;
    int num_retries = -1;
    char *pSimResponse;
    LOGD("%s:Entered",__func__);
    memset(&sr, 0, sizeof(sr));

    if(ioargs->path == NULL)
    {
        LOGE("SIM IO Path cannot be NULL");
        goto error;
    }

    /* Handling of pin2. */
    if (ioargs->pin2 != NULL) {
        nRet = authenticateSIMPIN2QMI(ioargs->pin2);
        if (nRet == eQCWWAN_ERR_QMI_INCORRECT_PIN || nRet == eQCWWAN_ERR_QMI_PIN_BLOCKED) {
            /* Get the retry count for PIN2 */
            num_retries = getPINRetryCountQMI( SIM_PIN2 );
            LOGD("%s: num_retries= %d",__func__, num_retries);
            if (num_retries)
                rilerr = RIL_E_PASSWORD_INCORRECT;
            else
                rilerr = RIL_E_SIM_PUK2;
            goto error;
        }
        else if (nRet != eQCWWAN_ERR_NONE) 
            goto error;
    }

    switch(ioargs->command) {
        case SIM_GET_RESPONSE:
            nRet = getFileAttributesQMI(ioargs,&sr);
            break;
        case SIM_READ_RECORD:
            nRet = readRecordQMI(ioargs,&sr);
            break;
        case SIM_WRITE_RECORD:
            nRet = updateRecordQMI(ioargs,&sr);
            break;
        case SIM_READ_BINARY:
           nRet = readBinaryQMI(ioargs,&sr);
            break;
        case SIM_WRITE_BINARY:
            nRet = updateBinaryQMI(ioargs,&sr);
            break;
    }

    if(nRet != eQCWWAN_ERR_NONE) {
        if (nRet != eQCWWAN_ERR_QMI_SIM_FILE_NOT_FOUND) { 
            requestSIM_IO(data, datalen, t);
            return;
        }
        else 
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
        pSimResponse = sr.simResponse;
        err = convertSimIoFcp(&sr, &sr.simResponse);
        if(pSimResponse)
            free (pSimResponse);
        if (err < 0)
            goto error;
        cvt_done = 1; /* sr.simResponse needs to be freed */
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, &sr, sizeof(sr));
    if(sr.simResponse) {
        free(sr.simResponse);
    }
    return;

error:
    LOGE("%s - error detected", __func__);
    RIL_onRequestComplete(t, rilerr, NULL, 0);
}
