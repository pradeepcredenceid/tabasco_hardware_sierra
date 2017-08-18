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
#include "at_channel.h"
#include "at_tok.h"
#include "at_misc.h"
#include <telephony/ril.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include <linux/route.h>
#include <cutils/properties.h>
#include <stdbool.h>

#define LOG_TAG "RIL"
#include "swiril_log.h"

#include <fcntl.h>
#include <cutils/sockets.h>

#include "swiril_main.h"
#include "swiril_pdp_common.h"
#include "swiril_pdp.h"
#include "swiril_misc.h"

/* Last pdp fail cause, obtained by +CEER */
static int s_lastPdpFailCause = PDP_FAIL_ERROR_UNSPECIFIED;


/**
 *
 * Initialize RIL_Data_Call_Response_v4 structure
 *
 * @return
 *     none
 *
 * @par Abort:<br> 
 *     none
 *
 * @note
 *     do nothing since RIL_Data_Call_Response_v4 uses 
 *     dynamically allocated memory
 */
void initDataCallResponseList(void)
{
}

/**
 *
 * Sends PDP context information to the RIL based on a solicited
 * request or an unsolicited event
 *
 * @param[in] t 
 *     Pointer to theRIL token
 * @return
 *     none
 *
 * @par Abort:<br> 
 *     none
 *
 * @note
 *     This function supports multiple active data calls.
 *     It is verified working ok, so leave as is.
 */
void requestOrSendPDPContextList(RIL_Token *t)
{
    ATResponse *atresponse;
    RIL_Data_Call_Response *responses;
    RIL_Data_Call_Response *response;
    ATLine *cursor;
    int err;
    int n = 0;
    int i = 0;
    char *out;

    err = at_send_command_multiline("AT+CGACT?", "+CGACT:", &atresponse);
    if (err != 0 || atresponse->success == 0) {
        if (t != NULL)
            RIL_onRequestComplete(*t, RIL_E_GENERIC_FAILURE, NULL, 0);
        else
            RIL_onUnsolicitedResponse(RIL_UNSOL_DATA_CALL_LIST_CHANGED,
                                      NULL, 0);
        at_response_free(atresponse);
        return;
    }

    for (cursor = atresponse->p_intermediates; cursor != NULL;
         cursor = cursor->p_next)
        n++;

    responses = alloca(n * sizeof(RIL_Data_Call_Response));

    for (i = 0; i < n; i++) {
        responses[i].cid = -1;
        responses[i].active = -1;
        responses[i].type = "";
        responses[i].apn = "";
        responses[i].address = "";
    }

    response = responses;
    for (cursor = atresponse->p_intermediates; cursor != NULL;
         cursor = cursor->p_next) {
        char *line = cursor->line;

        err = at_tok_start(&line);
        if (err < 0)
            goto error;

        err = at_tok_nextint(&line, &response->cid);
        if (err < 0)
            goto error;

        err = at_tok_nextint(&line, &response->active);
        if (err < 0)
            goto error;

        if (response->active == 1)
            response->active = 2;

        response++;
    }

    at_response_free(atresponse);

    err = at_send_command_multiline("AT+CGDCONT?", "+CGDCONT:",
                                    &atresponse);
    if (err != 0 || atresponse->success == 0) {
        if (t != NULL)
            RIL_onRequestComplete(*t, RIL_E_GENERIC_FAILURE, NULL, 0);
        else
            RIL_onUnsolicitedResponse(RIL_UNSOL_DATA_CALL_LIST_CHANGED,
                                      NULL, 0);
        return;
    }

    for (cursor = atresponse->p_intermediates; cursor != NULL;
         cursor = cursor->p_next) {
        char *line = cursor->line;
        int cid;
        char *type;
        char *apn;
        char *address;

        err = at_tok_start(&line);
        if (err < 0)
            goto error;

        err = at_tok_nextint(&line, &cid);
        if (err < 0)
            goto error;

        for (i = 0; i < n; i++) {
            if (responses[i].cid == cid)
                break;
        }

        if (i >= n) {
            /* Details for a context we didn't hear about in the last request. */
            continue;
        }

        err = at_tok_nextstr(&line, &out);
        if (err < 0)
            goto error;

        responses[i].type = alloca(strlen(out) + 1);
        strcpy(responses[i].type, out);

        err = at_tok_nextstr(&line, &out);
        if (err < 0)
            goto error;

        responses[i].apn = alloca(strlen(out) + 1);
        strcpy(responses[i].apn, out);

        err = at_tok_nextstr(&line, &out);
        if (err < 0)
            goto error;

        responses[i].address = alloca(strlen(out) + 1);
        strcpy(responses[i].address, out);
    }

    at_response_free(atresponse);

    if (t != NULL)
        RIL_onRequestComplete(*t, RIL_E_SUCCESS, responses,
                              n * sizeof(RIL_Data_Call_Response));
    else
        RIL_onUnsolicitedResponse(RIL_UNSOL_DATA_CALL_LIST_CHANGED,
                                  responses,
                                  n * sizeof(RIL_Data_Call_Response));

    return;

error:
    if (t != NULL)
        RIL_onRequestComplete(*t, RIL_E_GENERIC_FAILURE, NULL, 0);
    else
        RIL_onUnsolicitedResponse(RIL_UNSOL_DATA_CALL_LIST_CHANGED,
                                  NULL, 0);

    at_response_free(atresponse);
}

/**
 *
 * RIL_REQUEST_SETUP_DATA_CALL
 * Configure and activate PDP context for default IP connection.
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
void requestSetupDefaultPDP(void *data, size_t datalen, RIL_Token t)
{
    const char *APN = NULL;
    char *cmd = NULL;
    char *property = NULL;
    char *ipAddrStr = NULL;
    char **rilResponse = NULL;
    int err = 0;
    ATResponse *atresponse = NULL;
    int tech;
    int pidtype;
    const char *username = NULL;
    const char *password = NULL;
    const char *authtype;
    int authentype;
    char *linep = NULL;
    char *outp = NULL;
    int pid;
    int cme_err;
    int pdp_active = 0;
    bool isUNPresent = false;
    bool isPWPresent = false;

    /* Assign parameters. */
    tech = atoi(((const char **)data)[RIL_PDP_PARAM_RADIO_TECH]);
    pidtype = atoi(((const char **)data)[RIL_PDP_PARAM_DATA_PROFILE]);
    APN = ((const char **) data)[RIL_PDP_PARAM_APN];
    username = (char *)(((const char **)data)[RIL_PDP_PARAM_APN_USERNAME]);
    password = (char *)(((const char **)data)[RIL_PDP_PARAM_APN_PASSWORD]);
    authtype = ((const char **)data)[RIL_PDP_PARAM_APN_AUTH_TYPE];

    if (tech == 0) {
        LOGE("requestSetupDefaultPDP CDMA not supported by the modem\n");
        RIL_onRequestComplete(t, RIL_E_MODE_NOT_SUPPORTED, NULL, 0);
        return;
    }
    
    if (pidtype != RIL_DATA_PROFILE_DEFAULT) {
        LOGE("requestSetupDefaultPDP CID type %d not supported by the modem\n", pidtype);
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }    
    
    /* Stop the DHCP client if already running */
    checkDHCPnStopService();

    LOGD("requesting data connection to APN '%s'", APN);

    rilResponse = swimalloc((3 * sizeof(char *)), "requestSetupDefaultPDP allocate memory failed");
      
    asprintf(&rilResponse[0], "%d", SWI_DEFAULT_CID);
    rilResponse[1] = ril_iface;    
    
    /* Define PDP Context. NULL indicates no APN overrride */
    if (APN != NULL) {
        asprintf(&cmd, "AT+CGDCONT=%d,\"IP\",\"%s\",\"\",0,0", SWI_DEFAULT_CID, APN);
        
        err = at_send_command(cmd, &atresponse);
        free(cmd);
        if (err < 0 || atresponse->success == 0) {
            goto error;
        }
        at_response_free(atresponse);
        atresponse = NULL;
    }

    /* Define QCT PDP Authentication. */
    if (authtype != NULL) {
        authentype = atoi(authtype);
        /* check authentication type, RIL supports 0, 1, 2, 3 */
        if ((authentype < 0) || (authentype > 3)) {
            LOGE("requestSetupDefaultPDP authentication type %d is not supported by modem", authentype);
            goto error;
        } else if (authentype == 3) {
            /* we decide to do no authentication for type 3 */
            LOGI("requestSetupDefaultPDP authentication type %d will do no authentication at all", authentype);
        } else {
            /* If authentication is none, do not require UN and PWD */
            /* Username/Password could be NULL or an empty string - ignore if any */
            if (authentype != 0) {
                isUNPresent = ( (username != NULL) && (username[0] != '\0') );
                isPWPresent = ( (password != NULL) && (password[0] != '\0') );
            }
            if (isUNPresent && isPWPresent) {
                asprintf(&cmd, "AT$QCPDPP=%d,%d,\"%s\",\"%s\"", SWI_DEFAULT_CID, authentype, password, username);
            }
            else if (isPWPresent) {
                asprintf(&cmd, "AT$QCPDPP=%d,%d,\"%s\"", SWI_DEFAULT_CID, authentype, password);
            } else {
                asprintf(&cmd, "AT$QCPDPP=%d,%d", SWI_DEFAULT_CID, authentype);
            }
                    
            err = at_send_command(cmd, &atresponse);
            free(cmd);
            if (err < 0 || atresponse->success == 0) {
                goto error;
            }
            at_response_free(atresponse);
            atresponse = NULL;
        }
    }

    asprintf(&cmd, "AT+XCEDATA%d,0", SWI_DEFAULT_CID);
    err = at_send_command(cmd, &atresponse);
    free(cmd);
    /* After activated PDP successfully, the last data call fail cause doesn't clear */
    if (err < 0 || atresponse->success == 0) {
        /* Due to initialization code AT+CMEE=1, AT command returns "+CME ERROR:" instead of "ERROR".
         * get handle it separately */
        cme_err = at_get_cme_error(atresponse);
        if (cme_err != CME_SUCCESS && cme_err != CME_ERROR_NON_CME) {
            err = retrievePDPRejectCause(&s_lastPdpFailCause);
            if (err < 0) {
                LOGE("Setup data call fail: CME %d and can't get last fail cause",cme_err);
            }
            else {
                LOGE("Setup data call fail: CME %d cause %d",cme_err, s_lastPdpFailCause);
            }
        }
        goto error;
    }
    at_response_free(atresponse);
    atresponse = NULL; 
    pdp_active = 1;  
        
    /* get IP address */
    asprintf(&cmd, "AT!SCPADDR=%d", SWI_DEFAULT_CID);
    err = at_send_command_singleline(cmd, "!SCPADDR:", &atresponse);
    free(cmd);
    if (err < 0 || atresponse->success == 0) {
        goto error;
    }

    linep = atresponse->p_intermediates->line;
    err = at_tok_start(&linep);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&linep, &pid);
    if (err < 0 || pid != SWI_DEFAULT_CID)
        goto error;

    err = at_tok_nextstr(&linep, &outp);
    if (err < 0)
        goto error;
                    
    ipAddrStr = strdup(outp);
    swicheckp(ipAddrStr, "Duplicate IP address out of memory");

    rilResponse[2] = ipAddrStr;
    LOGI("IP Address: %s\n", ipAddrStr);
        
    at_response_free(atresponse);
    atresponse = NULL; 

    /*
     * Have to set properties net.xxx.dns1 and net.xxx.dns2 before send call
     * RIL_onRequestComplete; otherwise MobileDataStateTracker.java can't pick
     * up this information.
     *
     * Note that this code only makes sense on a real device.  On the emulator
     * we don't use the modem for the data path.
     *
     */

    /* Start dhcp client, so that we get assigned an IP address and DNS */
    if (property_set("ctl.start", "sierra_dhcpcd")) {
        LOGE("requestSetupDefaultPDP FAILED to set ctl.start sierra_dhcpcd property!");
        goto error;
    }

    if (waitDHCPnsetDNS() < 0) goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, rilResponse, 3 * sizeof(char *));
    /* Data Session started successully, now start polling to get current RAT */
    startPollingForRadioTechnology();
finally:

    if (ipAddrStr != NULL)
        free(ipAddrStr);

    at_response_free(atresponse);
    
    if (rilResponse != NULL) {
        /* don't free rilResponse[1], it is a shared string 
         * rilResponse[2] freed already 
         */
        if (rilResponse[0] != NULL) {
            free(rilResponse[0]);
        }
        free(rilResponse);
    }
    return;

error:
    LOGE("requestSetupDefaultPDP err=%d\n",err);
    /* deactivate the active PDP in case of error */
    if (pdp_active == 1)
    {
        /* free at response buffer */
        if (atresponse != NULL)
        {     
            at_response_free(atresponse);
            atresponse = NULL;
        }
        asprintf(&cmd, "AT+CGACT=0,%d", SWI_DEFAULT_CID);
        err = at_send_command(cmd, &atresponse);
        if (err < 0 || atresponse->success == 0)
        {
            /* log the deactivation error */
            LOGE("requestSetupDefaultPDP deactivate PDP err=%d\n",err);
        }
        free(cmd);
        s_lastPdpFailCause = PDP_FAIL_PROTOCOL_ERRORS;
    }
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 *
 * RIL_REQUEST_DEACTIVATE_DEFAULT_PDP
 * Deactivate PDP context created by RIL_REQUEST_SETUP_DEFAULT_PDP.
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
 *     See also: RIL_REQUEST_SETUP_DEFAULT_PDP.
 */
void requestDeactivateDefaultPDP(void *data, size_t datalen, RIL_Token t)
{
    const char *pPdpCid = NULL;
    char *pCmd = NULL;
    int err;
    ATResponse *atresponse = NULL;

    pPdpCid = ((const char **) data)[0];
    
    if (pPdpCid != NULL) {
        if (atoi(pPdpCid) == SWI_DEFAULT_CID) {
            asprintf(&pCmd, "AT+CGACT=0,%d", SWI_DEFAULT_CID);
        }
        else {
            LOGE("requestDeactivateDefaultPDP CID is invalid %s\n", pPdpCid);
            goto error;
        }        
    }
    else {
        LOGE("requestDeactivateDefaultPDP CID is NULL\n");
        goto error;
    }    

    err = at_send_command(pCmd, &atresponse);
    free(pCmd);
    if (err < 0  || atresponse->success == 0) {
        LOGE("requestDeactivateDefaultPDP err=%d, atresponse->success=%d\n",err,atresponse->success);
        goto error;
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

finally:
    at_response_free(atresponse);
    /* stop DHCP client */
    //HL not good
    //if (property_set("ctl.stop", "sierra_dhcpcd")) {
    //    LOGE("requestDeactivateDefaultPDP FAILED to set ctl.stop sierra_dhcpcd property!");
    }
    return;
    
error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_LAST_PDP_FAIL_CAUSE
 * 
 * Requests the failure cause code for the most recently failed PDP 
 * context activate.
 *
 * See also: RIL_REQUEST_LAST_CALL_FAIL_CAUSE.
 *  
 */
void requestLastPDPFailCause(void *data, size_t datalen, RIL_Token t)
{
    RIL_onRequestComplete(t, RIL_E_SUCCESS, &s_lastPdpFailCause,
                          sizeof(int));
}
