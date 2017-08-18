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

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <telephony/ril.h>
#include <sys/types.h>
#include <stdbool.h>
#include "at_channel.h"
#include "at_tok.h"
#include "swiril_main.h"

#define LOG_TAG "RIL"
#include "swiril_log.h"

#define USSD_RESP_MAX_LEN_UCS2   640
#define SLUTF8CHARSIZE1          (unsigned short)0x80
#define SLUTF8CHARSIZE2          (unsigned short)0x800
#define EOS                      '\0'
#define SL2BYTEUTF8FIRSTBYTEMSK  0x1F
#define SL3BYTEUTF8FIRSTBYTEMSK  0x0F
#define SLSECONDARYUTF8BYTEMSK   0x3F
#define SL2BYTEUTF8FIRSTBYTEMRK  0xC0
#define SL3BYTEUTF8FIRSTBYTEMRK  0xE0
#define SL4BYTEUTF8FIRSTBYTEMRK  0xF0
#define SLSECONDARYUTF8BYTEMRK   0x80

#ifdef RIL_GCF_TEST_FLAG
#define MAX_RETRY_CALL_FORWARD 0
#else
#define MAX_RETRY_CALL_FORWARD 10
#endif

static const struct timeval TIMEVAL_RETRY_CALL_FORWARD = { 2, 0 };
/* function prototypes */
static int convHexStrToBin(char *pStrUCS2, unsigned short *pUCS2);
static bool slUcs2ToUtf8(
    unsigned short   *ucs2stringp,
    int numucs2chars,
    char *utf8stringp,
    int maxbytes);
static void convertUTF8ToUCS2Str(const char* src, char* dest);

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
    /* AT+CLIP? */
    char *cmd = NULL;
    char *line = NULL;
    int err = 0;
    int response = 2;
    ATResponse *atresponse = NULL;

    err = at_send_command_singleline("AT+CLIP?", "+CLIP:", &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;

    line = atresponse->p_intermediates->line;

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    /* Read the first int and ignore it, we just want to know if
       CLIP is provisioned. */
    err = at_tok_nextint(&line, &response);
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
void requestCancelUSSD(void *data, size_t datalen, RIL_Token t)
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
void requestSendUSSD(void *data, size_t datalen, RIL_Token t)
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
    if (err < 0 || response->success == 0)
         goto error;
    at_response_free(response);
    response = NULL;

    ussdRequest = (char *) (data);

    /* allocate memory for USSDReqUCS2 */
    USSDReqUCS2 = malloc(strlen(ussdRequest)*4 +1);
    memset(USSDReqUCS2,0,strlen(ussdRequest)*4 +1);

    /* convert UTF8 to UCS2 */
    convertUTF8ToUCS2Str(ussdRequest,USSDReqUCS2);

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
 * RIL_UNSOL_ON_USSD
 * Called when a new USSD message is received
 *
 * @param [in] s
 *     unsolicited USSD message that has been received
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
void onUSSDReceived(const char *s)
{
    char **response;
    char *line;
    int err = -1;
    int i = 0;
    int n = 0;
    unsigned short  ucs2arryLen, ucs2StrLen;
    char   pUcs2InUtf8Str[ USSD_RESP_MAX_LEN_UCS2 ];
    unsigned short  pDecodedUCS[ USSD_RESP_MAX_LEN_UCS2/2 ];

    LOGD("%s: USSD Response from Network: %s\n", __func__, s);

    line = alloca(strlen(s) + 1);
    strcpy(line, s);
    line[strlen(s)] = 0;

    response = alloca(2 * sizeof(char *));
    response[0] = NULL;

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &i);
    if (err < 0)
        goto error;

    if (i < 0 || i > 5)
        goto error;

    response[0] = alloca(2);
    sprintf(response[0], "%d", i);

    n = 1;

    if (i < 2) {
        n = 2;

        err = at_tok_nextstr(&line, &response[1]);
        if (err < 0)
            goto error;
    }

#ifdef HANDLE_USSD_RESP_UTF8
    RIL_onUnsolicitedResponse( RIL_UNSOL_ON_USSD, response, n * sizeof(char *) );
#else
   /*
    *  USSD response will be received in UCS2 format always as send USSD is also in UCS2 always.
    *  So USSD response is in UCS2, need to covert utf8 string and send to Android UI
    */
    ucs2StrLen = strlen( response[1] );

    /* convert ucs2 string to utf8 string */
    ucs2arryLen = convHexStrToBin( response[1], pDecodedUCS );
    slUcs2ToUtf8( pDecodedUCS, ucs2arryLen, pUcs2InUtf8Str, ucs2StrLen );

    response[1] = pUcs2InUtf8Str;

    LOGD("%s:: UCS2 Length:%d Str: %s\n", __func__, strlen(pUcs2InUtf8Str), pUcs2InUtf8Str );

    RIL_onUnsolicitedResponse( RIL_UNSOL_ON_USSD, response,  n * sizeof(char *) );

    return;
#endif

error:
    LOGE("%s:: Error in decode USSD response\n", __func__);

    return;
}

void requestQueryCallForwardStatus(void *data, size_t datalen, RIL_Token t);
void requestSetCallForward(void *data, size_t datalen, RIL_Token t);

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
    char *cmd;
    int err;
    ATResponse *atresponse = NULL;
    ATLine *cursor = NULL;
    const RIL_CallForwardInfo *pCallForwardInfo = &gCallForwardInfo;

    int mode = 2;               /* Query status */
    int n;
    int i = 0;
    int serviceClass;
    RIL_CallForwardInfo *rilResponse;
    RIL_CallForwardInfo **rilResponseArray;

    memcpy(&gCallForwardInfo, data, sizeof(RIL_CallForwardInfo));

    /* If service Class is 0, use 0xFF */
    serviceClass = pCallForwardInfo->serviceClass;
    if (serviceClass == 0)
       serviceClass = 0xFF;

    /* AT+CCFC=<reason>,<mode>[,<number>[,<type>[,<class> [,<subaddr> [,<satype> [,<time>]]]]]] */
    asprintf(&cmd, "AT+CCFC=%d,%d,,,%d", pCallForwardInfo->reason, 
             mode,serviceClass);

    err = at_send_command_multiline(cmd, "+CCFC:", &atresponse);

    if (err < 0 || atresponse->success == 0) {
        goto error;
    }

    n = 0;

    for (cursor = atresponse->p_intermediates; cursor != NULL;
         cursor = cursor->p_next)
        n++;

    rilResponse = alloca(n * sizeof(RIL_CallForwardInfo));
    rilResponseArray = alloca(n * sizeof(RIL_CallForwardInfo *));
    memset(rilResponse, 0, sizeof(RIL_CallForwardInfo) * n);

    for (i = 0; i < n; i++) {
        rilResponseArray[i] = &(rilResponse[i]);
    }

    /* When <mode>=2 and command successful:
     * +CCFC: <status>,<class1>[,<number>,<type>
     * [,<subaddr>,<satype>[,<time>]]][
     * <CR><LF>
     * +CCFC: <status>,<class2>[,<number>,<type>
     * [,<subaddr>,<satype>[,<time>]]]
     * [...]]
     */
    for (i = 0, cursor = atresponse->p_intermediates; cursor != NULL && i < n;
         cursor = cursor->p_next, ++i) {
        char *line = NULL;
        line = cursor->line;

        err = at_tok_start(&line);
        if (err < 0)
            goto error;

        err = at_tok_nextint(&line, &rilResponse[i].status);
        if (err < 0)
            goto error;

        err = at_tok_nextint(&line, &rilResponse[i].serviceClass);
        if (err < 0)
            goto error;

        if (at_tok_hasmore(&line)) {
            err = at_tok_nextstr(&line, &rilResponse[i].number);
            if (err < 0)
                goto error;

            err = at_tok_nextint(&line, &rilResponse[i].toa);
            if (err < 0)
                goto error;
        }
    }
    counter_retry_call_forward = 0;
    RIL_onRequestComplete(t, RIL_E_SUCCESS, rilResponseArray,
                          n * sizeof(RIL_CallForwardInfo *));
finally:
    if (cmd != NULL)
        free(cmd);
    if (atresponse != NULL)
        at_response_free(atresponse);
    return;

error:
    if (counter_retry_call_forward >= MAX_RETRY_CALL_FORWARD) {
        counter_retry_call_forward = 0;
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    } else {
        callForwardToken = t;
        LOGD("requestQueryCallForwardStatus Failed!, Re-trying...#%d\n\n", counter_retry_call_forward);
        counter_retry_call_forward++;
        enqueueRILEvent(RIL_EVENT_QUEUE_PRIO, queryCallFwd,
                    (void*)data, &TIMEVAL_RETRY_CALL_FORWARD);
    }
    goto finally;
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
    char *cmd = NULL;
    int err;
    ATResponse *atresponse = NULL;
    int serviceClass;
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

    /* If service Class is 0, use 0xFF */
    serviceClass = callForwardInfo->serviceClass;
    if (serviceClass == 0)
        serviceClass = 0xFF;

    /* AT+CCFC=<reason>,<mode>[,<number>[,<type>[,<class> [,<subaddr> [,<satype> [,<time>]]]]]] */
    if (callForwardInfo->number == NULL) {
        if (callForwardInfo->timeSeconds) {
            asprintf(&cmd, "AT+CCFC=%d,%d,,,%d,,,%d",
                     callForwardInfo->reason,
                     callForwardInfo->status,
                     serviceClass,
                     callForwardInfo->timeSeconds);
        }
        else {
            asprintf(&cmd, "AT+CCFC=%d,%d,,,%d",
                     callForwardInfo->reason,
                     callForwardInfo->status,
                     serviceClass); 
        }       
     } else {
        if (callForwardInfo->timeSeconds) {
            asprintf(&cmd, "AT+CCFC=%d,%d,\"%s\",%d,%d,,,%d",
                     callForwardInfo->reason,
                     callForwardInfo->status,
                     callForwardInfo->number,
                     callForwardInfo->toa,
                     serviceClass,
                     callForwardInfo->timeSeconds);
        }
        else {
            asprintf(&cmd, "AT+CCFC=%d,%d,\"%s\",%d,%d",
                     callForwardInfo->reason,
                     callForwardInfo->status,
                     callForwardInfo->number,
                     callForwardInfo->toa,
                     serviceClass);
       }            
    }

    err = at_send_command(cmd, &atresponse);

    if (err == 0 && atresponse->success == 0) {
        if (atresponse->finalResponse != NULL &&
            strstr(atresponse->finalResponse,"ERROR: 257") == NULL){
            counter_retry_call_forward = MAX_RETRY_CALL_FORWARD;
            LOGD("%s, not retry\n\n",atresponse->finalResponse);
        }
        goto error;
    }

    if (err < 0 || atresponse->success == 0) {
        goto error;
    }
    counter_retry_call_forward = 0;
    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

finally:
    free(cmd);
    at_response_free(atresponse);
    return;

error:
    if (counter_retry_call_forward >= MAX_RETRY_CALL_FORWARD) {
        counter_retry_call_forward = 0;
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    } else {
        callForwardToken = t;
        LOGD("requestSetCallForward Failed!, Re-trying...#%d\n\n", counter_retry_call_forward);
        counter_retry_call_forward++;
        enqueueRILEvent(RIL_EVENT_QUEUE_PRIO, setCallFwd,
                    (void*)data, &TIMEVAL_RETRY_CALL_FORWARD);
    }

    goto finally;
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
    char *cmd = NULL;
    char *line = NULL;
    int response[2] = { 0, 0 };
    int err;
    ATResponse *atresponse = NULL;
    ATLine *cursor = NULL;
    const int class = ((int *) data)[0];

    /* Sierra modem doesn't like class=0, pass 255 if class=0 */
    if(0==class)
        asprintf(&cmd, "AT+CCWA=0,2,255");
    else
        /* AT+CCWA=[<n>[,<mode>[,<class>]]]  n=0 (default) mode=2 (query) */
        asprintf(&cmd, "AT+CCWA=0,2,%d", class);

    err = at_send_command_multiline(cmd, "+CCWA:", &atresponse);

    /* When <mode> =2 and command successful:
       +CCWA: <status>,<class1>[<CR><LF>+CCWA: <status>,<class2>[...]]  */

    if (err < 0 || atresponse->success == 0) {
        goto error;
    }

    for (cursor = atresponse->p_intermediates; cursor != NULL;
         cursor = cursor->p_next) {
        int serviceClass = 0;
        int status = 0;
        line = cursor->line;

        err = at_tok_start(&line);
        if (err < 0)
            goto error;

        err = at_tok_nextint(&line, &status);
        if (err < 0)
            goto error;

        err = at_tok_nextint(&line, &serviceClass);
        if (err < 0)
            goto error;

        if (status == 1 && serviceClass > 0 && serviceClass <= 128) {
            response[1] |= serviceClass;
        }
    }

    if (response[1] > 0)
        response[0] = 1;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, response, sizeof(int) * 2);

finally:
    free(cmd);
    at_response_free(atresponse);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
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
    char *pCmd = NULL;
    int err;
    const int mode = ((int *) data)[0];
    int class = ((int *) data)[1];
    ATResponse *atresponse = NULL;

    if( (mode<0) || (mode>1) ) {
        goto error;
    }

    if (class == 0) {
        class = 0xFF;
    }

    /* AT+CCWA=[<n>[,<mode>[,<class>]]]  n=0 (default) */
    asprintf(&pCmd, "AT+CCWA=0,%d,%d", mode, class);

    err = at_send_command(pCmd, &atresponse);
    if (err < 0 || atresponse->success == 0) {
        goto error;
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

finally:
    if(pCmd != NULL) {
        free(pCmd);
    }
    at_response_free(atresponse);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_UNSOL_SUPP_SVC_NOTIFICATION
 * Reports supplementary service related notification from the network
 *
 * @param [in] s
 *      unsolicited supplementary service notification received
 * @param [in] type
 *      type of supplementary service notification received
 *          0 - CSSI
 *          1 - CSSU
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
void onSuppServiceNotification(const char *s, int type)
{
    RIL_SuppSvcNotification ssnResponse;
    char *line;
    char *tok;
    int err;

    line = tok = strdup(s);

    memset(&ssnResponse, 0, sizeof(ssnResponse));
    ssnResponse.notificationType = type;

    err = at_tok_start(&tok);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&tok, &ssnResponse.code);
    if (err < 0)
        goto error;

    if (ssnResponse.code == 16 ||
        (type == 0 && ssnResponse.code == 4) ||
        (type == 1 && ssnResponse.code == 1)) {
        err = at_tok_nextint(&tok, &ssnResponse.index);
        if (err < 0)
            goto error;
    }

    /* RIL_SuppSvcNotification has two more members that we won't
       get from the +CSSI/+CSSU. Where do we get them, if we ever do? */
    RIL_onUnsolicitedResponse(RIL_UNSOL_SUPP_SVC_NOTIFICATION,
                              &ssnResponse, sizeof(ssnResponse));

error:
    free(line);
}

/**
 * RIL_REQUEST_SET_SUPP_SVC_NOTIFICATION
 * Enables/disables supplementary service related notifications from the network
 *
 * @param [in] data
 *     "data" is int *
 *     ((int *)data)[0] is == 1 for notifications enabled
 *     ((int *)data)[0] is == 0 for notifications disabled
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
 *     Notifications are reported via RIL_UNSOL_SUPP_SVC_NOTIFICATION
 *     See also: RIL_UNSOL_SUPP_SVC_NOTIFICATION
 */
void requestSetSuppSvcNotification(void *data, size_t datalen, RIL_Token t)
{
    int err;
    int ssn = ((int *) data)[0];
    char *cmd;

    if( ssn != 0 && ssn != 1 )
        goto error;

    asprintf(&cmd, "AT+CSSN=%d,%d", ssn, ssn);

    err = at_send_command(cmd, NULL);
    free(cmd);
    if (err < 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

finally:
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
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
    ATResponse *atresponse = NULL;
    char c = ((char *) data)[0];
    char *cmd = NULL;
    int err = 0;

    err = at_send_command("AT+VTD=0", &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;

    at_response_free(atresponse);
    atresponse = NULL;

    /* Start the DTMF tone. */
    asprintf(&cmd, "AT+VTS=%c", (int) c);
    err = at_send_command(cmd, &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

finally:
    if (cmd != NULL)
        free(cmd);

    at_response_free(atresponse);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
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
    int err = 0;
    ATResponse *atresponse = NULL;

    err = at_send_command("AT+VTD=0", &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

finally:
    at_response_free(atresponse);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/*************
 *
 * Name:    slUcs2ToUtf8
 *
 * Purpose: To convert a UCS2 string to a UTF-8 string.
 *          This function will NULL-terminate the UTF-8 string.
 *
 *          An UCS2 character is converted to UTF-8 in the following way:
 *
 *              UCS2                  UTF-8
 *                                    byte1    byte2    byte3
 *              00000000 0xxxxxxx     0xxxxxxx
 *              00000yyy yyxxxxxx     110yyyyy 10xxxxxx
 *              zzzzyyyy yyxxxxxx     1110zzzz 10yyyyyy 10xxxxxx
 *
 * Parms:   ucs2stringp  - pointer to UCS2 string to be converted
 *          numucs2chars - number of characters in the UCS2 string
 *          utf8stringp  - pointer to the empty UTF-8 string buffer where
 *                         the converted string will be placed
 *          maxbytes     - maximum number of bytes supported by the passed in
 *                         UTF-8 string pointer (including NULL terminator).
 *
 * Return:  TRUE  - if the conversion was successful
 *          FALSE - if the string could not be fit in the provided buffer
 *
 * Abort:   none
 *
 * Notes:   It is hoped that the caller will ensure that the string
 *          buffer given by utf8stringp will be large enough to contain all of
 *          the converted characters plus a NULL character.  Which means
 *          the buffer must be at least of size (numucs2chars*4 + 1).  If the
 *          buffer is too small then the string will be truncated.
 *
 **************/
static bool slUcs2ToUtf8(
    unsigned short   *ucs2stringp,
    int numucs2chars,
    char *utf8stringp,
    int maxbytes)
{
    int numconvchars; /* counter of the number of character converted */
    unsigned short   ucs2char;     /* value of the UCS2 character being converted */
    char *endstringp = utf8stringp + maxbytes - 1;

    /* clear the number of converted characters counter */
    numconvchars = 0;

    /* loop through the number of characters to convert */
    while(numconvchars != numucs2chars)
    {
        /* get the next UCS2 character for conversion */
        ucs2char = *ucs2stringp++;

        /* if this UCS2 character is represented in UTF-8 as a single byte */
        if(ucs2char < SLUTF8CHARSIZE1)
        {
            /*
             * verify that there is room in the buffer for this
             * converted character
             */
            if(utf8stringp + 1 > endstringp)
            {
                *utf8stringp = EOS;
                return(FALSE);
            }

            /* copy character over and increment UTF-8 string pointer */
            *utf8stringp++ = (char)ucs2char;
        }
        /* if this UCS2 character is represented in UTF-8 as two bytes */
        else if(ucs2char < SLUTF8CHARSIZE2)
        {
            /*
             * verify that there is room in the buffer for this
             * converted character
             */
            if(utf8stringp + 2 > endstringp)
            {
                *utf8stringp = EOS;
                return(FALSE);
            }

            /* copy the second UTF-8 character into the string buffer */
            *(utf8stringp + 1) =
                (char)(SLSECONDARYUTF8BYTEMRK +
                       (ucs2char & SLSECONDARYUTF8BYTEMSK));

            /* shift ucs2char to its first byte value */
            ucs2char >>= 6;

            /* copy the first UTF-8 character into the string buffer */
            *utf8stringp =
                (char)(SL2BYTEUTF8FIRSTBYTEMRK +
                        (ucs2char & SL2BYTEUTF8FIRSTBYTEMSK));

            /*
             * increment the UTF-8 string pointer pass the
             * representation of this character
             */
            utf8stringp +=2;
        }
        /* this USC2 character is represented in UTF-8 as three bytes */
        else
        {
            /*
             * verify that there is room in the buffer for this
             * converted character
             */
            if(utf8stringp + 3 > endstringp)
            {
                *utf8stringp = EOS;
                return(FALSE);
            }

            /* copy the third UTF-8 character into the string buffer */
            *(utf8stringp + 2) =
                (char)(SLSECONDARYUTF8BYTEMRK +
                    (ucs2char & SLSECONDARYUTF8BYTEMSK));

            /* shift ucs2char to its second byte value */
            ucs2char >>= 6;

            /* copy the second UTF-8 character into the string buffer */
            *(utf8stringp + 1) =
                (char)(SLSECONDARYUTF8BYTEMRK +
                    (ucs2char & SLSECONDARYUTF8BYTEMSK));

            /* shift ucs2char to its first byte value */
            ucs2char >>= 6;

            /* copy the first UTF-8 character into the string buffer */
            *utf8stringp =
                (char)(SL3BYTEUTF8FIRSTBYTEMRK +
                    (ucs2char & SL3BYTEUTF8FIRSTBYTEMSK));

            /*
             * increment the UTF-8 string pointer pass the representation
             * of this character
             */
            utf8stringp +=3;
        }
        /* increment number of characters converted */
        numconvchars++;
    }

    *utf8stringp = EOS;

    return(TRUE);
}

/**
 * convHexStrToBin
 * Converts ucs2 string (in Hex) into binary.
 *
 * @param [in] pStrUCS2
 *     pointer to ucs2 string from network to be converted into binary
 *
 * @param [out] pUCS2
 *     pointer to ucs2 array. converted ucs2 array will be placed here.
 *
 * @return int
 *     returns length of the ucs2 array.
 */
static int convHexStrToBin(char *pStrUCS2, unsigned short *pUCS2)
{
    bool bRet = FALSE;
    char fragment[5];
    int length;
    char* ptr = NULL;
    int decodedLength = 0;

    /* check for valid args */
    if ( NULL == pStrUCS2 ||
         NULL == pUCS2 )
    {
        bRet = FALSE;
        goto err;
    }

    length = strlen(pStrUCS2);
    ptr = pStrUCS2;

    while ( length>0 )
    {
        unsigned short val = 0;
        memset(fragment,0,sizeof(fragment));
        strncpy(fragment,ptr,4);
        sscanf(fragment,"%x",(unsigned int *)&val);
        ptr +=4;
        length -= 4;
        *pUCS2++ = val;
        decodedLength++;
    }

    return decodedLength;

    err:
    return 0;
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
static void convertUTF8ToUCS2Str(const char* src, char* dest)
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
void requestChangeBarringPassword (void *data, size_t datalen, RIL_Token t)
{
    int err = 0;
    ATResponse *atresponse = NULL;
    char *cmd = NULL;   
    const char *facilityString = ((char **)data)[0];
    const char *oldPassword    = ((char **)data)[1];
    const char *newPassword    = ((char **)data)[2];

    LOGD("%s::Entered\n", __func__);
    LOGD("%s::Facility String : %s\n", __func__, facilityString);
    LOGD("%s::Old Password    : %s\n", __func__, oldPassword   );
    LOGD("%s::New Password    : %s\n", __func__, newPassword   );

    asprintf(&cmd, "AT+CPWD=\"%s\",%s,%s", facilityString, oldPassword, newPassword);
    err = at_send_command(cmd, &atresponse);
    free(cmd);
    if (err < 0 || atresponse->success == 0) {
        at_response_free(atresponse);
        LOGE("%s::AT+CPWD failed\n", __func__);
        goto error;
    }
    LOGD("%s::AT+CPWD success\n", __func__);
    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    at_response_free(atresponse);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}
