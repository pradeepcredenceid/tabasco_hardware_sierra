/* 
** This source code is "Not a Contribution" under Apache license
**
** Sierra Wireless RIL
**
** Based on reference-ril by The Android Open Source Project
** and U300 RIL by ST-Ericsson.
** Modified by Sierra Wireless, Inc.
**
** Copyright (C) 2012 Sierra Wireless, Inc.
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

#include "at_channel.h"
#include "at_tok.h"
#include <telephony/ril.h>
#include <cutils/properties.h>
#include <stdbool.h>
#include <string.h>

#define LOG_TAG "RIL"
#include "swiril_log.h"

#include "swiril_pdp_common.h"
#include "swiril_pdp.h"

/**
 * +CEER lookup structure
 */
typedef struct ceer_error_lookup
{
  char     *text;   /**< error string defined by QCT */
  int      code;    /**< error code defined by 3GPP */
} ceer_error_lookup_type;

/** 
 * network reject cause lookup table 
 */
static const ceer_error_lookup_type ceer_sm_net_cause_codes[] =
{
    {"Operator determined barring",             (int)PDP_FAIL_OPERATOR_BARRED},
    {"Insufficient resources",                  (int)PDP_FAIL_INSUFFICIENT_RESOURCES},
#ifdef ANDROID_6PLUS    
    {"Missing or unknown APN",                  (int)PDP_FAIL_MISSING_UNKNOWN_APN},
#else    
    {"Missing or unknown APN",                  (int)PDP_FAIL_MISSING_UKNOWN_APN},
#endif
    {"Unknown PDP address or PDP type",         (int)PDP_FAIL_UNKNOWN_PDP_ADDRESS_TYPE},
    {"User Authentication failed",              (int)PDP_FAIL_USER_AUTHENTICATION},
    {"User Aauthentication failed",             (int)PDP_FAIL_USER_AUTHENTICATION}, /* this comes from QCT AT return although there is a typo */
    {"Activation rejected by GGSN",             (int)PDP_FAIL_ACTIVATION_REJECT_GGSN},
    {"Activation rejected, unspecified",        (int)PDP_FAIL_ACTIVATION_REJECT_UNSPECIFIED},
    {"Service option not supported",            (int)PDP_FAIL_SERVICE_OPTION_NOT_SUPPORTED},
    {"Requested service option not subscribed", (int)PDP_FAIL_SERVICE_OPTION_NOT_SUBSCRIBED},
    {"Service option temporarily out of order", (int)PDP_FAIL_SERVICE_OPTION_OUT_OF_ORDER},
    {"NSAPI already used (not sent)",           (int)PDP_FAIL_NSAPI_IN_USE},
    {"Protocol error, unspecified",             (int)PDP_FAIL_PROTOCOL_ERRORS},
    {NULL,                                      (int)PDP_FAIL_ERROR_UNSPECIFIED},
};

extern int g_ppp_enabled;

/**
 *
 * Retrive the reject case of last PDP context activation failure
 *
 * @param[out] causep 
 *     Pointer to the failure cause
 *
 * @return
 *     0  - successfully retrived error code
 *     -1 - otherwise
 *
 * @par Abort:<br> 
 *     none
 *
 * @note
 *     none
 */
int retrievePDPRejectCause(int *causep)
{
    int err = 0;
    int i;
    int ret = -1;
    ATResponse *atresponse = NULL;
    char *linep = NULL;
    char *outp = NULL;
    
    /* set last reject cause to PDP_FAIL_ERROR_UNSPECIFIED here in case of any failure later */
    *causep = PDP_FAIL_ERROR_UNSPECIFIED;
    
    /* retrive reject cause */
    err = at_send_command_singleline("AT+CEER", "+CEER", &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;
    
    linep = atresponse->p_intermediates->line;
    err = at_tok_start(&linep);
    if (err < 0)
        goto error;
    
    /* skip the leading space */
    err = at_tok_nextstr(&linep, &outp);
    if (err < 0)
        goto error;
  
    i = 0;
    while (ceer_sm_net_cause_codes[i].text != NULL) {
        if (0 == strcmp(outp, ceer_sm_net_cause_codes[i].text)) {
            *causep = ceer_sm_net_cause_codes[i].code;
            break;
        }
        i++;
    }
    /* Return ok even it didn't find error code from lookup table, 
     * because it has been set to PDP_FAIL_ERROR_UNSPECIFIED at the begining of this function
     */
    ret = 0;

finally:
    at_response_free(atresponse);
    return ret;

error:
    LOGE("Retrive reject cause error");
    goto finally;
}

/**
 *
 * RIL_UNSOL_PDP_CONTEXT_LIST_CHANGED
 * Indicate a PDP context state has changed, or a context
 * has been deactivated from network side.
 *
 * @param[in] param 
 *     Pointer to the callback parameter
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
void onPDPContextListChanged(void *param)
{
#ifdef SWI_RIL_VERSION_12
    requestOrSendPDPContextList_r12(NULL);
#else
    requestOrSendPDPContextList(NULL);
#endif
}

#ifndef SWI_RIL_VERSION_12

/**
 *
 * RIL_REQUEST_PDP_CONTEXT_LIST
 * Queries the status of PDP contexts.
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
void requestPDPContextList(void *data, size_t datalen, RIL_Token t)
{
    requestOrSendPDPContextList(&t);
}

#else

/**
 *
 * RIL_REQUEST_PDP_CONTEXT_LIST
 * Queries the status of PDP contexts.
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
void requestPDPContextList_r12(void *data, size_t datalen, RIL_Token t)
{
    requestOrSendPDPContextList_r12(&t);
}
#endif

/**
 *
 * Check if there is an activae data session
 *
 * @return
 *      true  - there is an activae data session
 *      false - otherwise
 *
 * @par Abort:<br> 
 *     none
 *
 * @note
 *     AT!SCACT could return an error if the SIM is PIN locked. 
 *     In this case, assume that there is no active PDP context.
 */
bool isDataSessionActive(void)
{
    ATResponse *atresponsep;
    char *linep;
    char *cmdp;
    int err;
    int pid = 0;
    int active = 0;
    bool bRet = false; 
    char propValue[PROPERTY_VALUE_MAX];

    if (g_ppp_enabled) {
        property_get("net.ppp.status", propValue,"");
        if(strcmp(propValue,"connected") == 0)
            bRet = true;
        return bRet;
    }   

    /* Get the PID and PDP status */
    asprintf(&cmdp, "AT!SCACT?%d", SWI_DEFAULT_CID);
    err = at_send_command_singleline(cmdp, "!SCACT:", &atresponsep);
    if (err < 0 || atresponsep->success == 0) {
        at_response_free(atresponsep);
        /* It's important, retry once */
        sleep(1);
        err = at_send_command_singleline(cmdp, "!SCACT:", &atresponsep);
        if (err < 0 || atresponsep->success == 0) {
            LOGE("%s %s failed twice", __func__, cmdp);
            free(cmdp);
            at_response_free(atresponsep);
            goto error;
        }
    } 
    free(cmdp);
    
    linep = atresponsep->p_intermediates->line;
    err = at_tok_start(&linep);
    if (err < 0) {
        LOGE("%s can't get AT line err %d", __func__, err);
    } 
    else {
        err = at_tok_nextint(&linep, &pid);
        if (err < 0 || pid != SWI_DEFAULT_CID) {
            LOGE("%s can't get pid or pid not match err %d pid %d", 
                 __func__, err, pid);
        }
        else {
            err = at_tok_nextint(&linep, &active);
            if (err < 0) {
                LOGE("%s can't get data call state err %d", __func__, err);
            }
        }
    }

    at_response_free(atresponsep);
    atresponsep = NULL;

    LOGI("%s session state: %d\n", __func__, active );
    if (active == 1) {
        bRet = true;
    }
    
error:
    return bRet;
}

/**
 * De-activate PDP if already active
 *
 * @return
 *      none
 *
 * @par 
 *     none
 *
 * @note
 *     None
 */
void deactivatePDPContext(void)
{
    int  err;
    char *cmdp;
    ATResponse *atresponsep = NULL;

    /* De-activate the PDP if already active */
    if(isDataSessionActive()) {
        if(g_ppp_enabled) {
            initDataCallResponseList();
            return;
        }
        asprintf(&cmdp, "AT!SCACT=0,%d", SWI_DEFAULT_CID);
        err = at_send_command(cmdp, &atresponsep);
        if (err < 0) {
            LOGE("%s %s err %d", __func__, cmdp, err);
        } 
        free(cmdp);
        at_response_free(atresponsep);
        initDataCallResponseList();
    }
}
