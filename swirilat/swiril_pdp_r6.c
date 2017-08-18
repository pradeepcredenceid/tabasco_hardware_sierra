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
#include <termios.h>

#define LOG_TAG "RIL"
#include "swiril_log.h"

#include <fcntl.h>
#include <cutils/sockets.h>

#include "swiril_main.h"
#include "swiril_pdp_common.h"
#include "swiril_pdp.h"
#include "swiril_misc.h"
#include "swiril_network.h"

/* Last pdp fail cause, obtained by +CEER */
static int s_lastPdpFailCause = PDP_FAIL_ERROR_UNSPECIFIED;

#ifdef SWI_RIL_VERSION_12
static RIL_Data_Call_Response_v11 s_rilPDPContextListRes = { PDP_FAIL_NONE,
                                                            RIL_ACTIVATE_DATA_CALL_RETRY_NO_SUGGESTION,
                                                            0,
                                                            RIL_STATE_INACTIVE,
                                                            NULL,
                                                            NULL,
                                                            NULL,
                                                            NULL,
                                                            NULL,
                                                            NULL,
                                                            0 };

#else
static RIL_Data_Call_Response_v6 s_rilPDPContextListRes = { PDP_FAIL_NONE,
                                                            RIL_ACTIVATE_DATA_CALL_RETRY_NO_SUGGESTION,
                                                            0,
                                                            RIL_STATE_INACTIVE,
                                                            NULL,
                                                            NULL,
                                                            NULL,
                                                            NULL,
                                                            NULL };
#endif

struct dhcpPollParams {
    RIL_Token t;
    int loopcount;
};

struct pppPollParams {
    RIL_Token t;
    int loopcount;
};
static const struct timeval TIMEVAL_PPP_POLL = { 4, 0 };
int g_ppp_fd = -1;
#define REPOLL_PPP 8
extern int g_ppp_enabled;
void PPPStopService();
static const struct timeval TIMEVAL_DHCP_POLL = { 3, 0 };
bool s_StopRadioTechPoll = false;
extern bool s_AR75_Modem;
#define REPOLL_DHCP 10
int PollDHCPnsetDNS(void);

/** 
  * Enabled by default, macro is disabled to overcome the delay in response of AT command 
  * after network deregistration. After network is unregsitered at modem, data session 
  * de-activate command response is delayed by modem for around 40 seconds.
  */   
#define DEACTIVATE_DATA_SESSION_AFTER_MODEM_UNREG

/**
 *
 * Initialize RIL_Data_Call_Response(_v6) structure
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
void initDataCallResponseList(void)
{
    s_rilPDPContextListRes.status = PDP_FAIL_RADIO_POWER_OFF;

    /* Suggested retry times, default to -1 which means no value is suggested */
    s_rilPDPContextListRes.suggestedRetryTime = RIL_ACTIVATE_DATA_CALL_RETRY_NO_SUGGESTION;
    /* Context ID */
    s_rilPDPContextListRes.cid = 0;
    s_rilPDPContextListRes.active = RIL_STATE_INACTIVE;
    /* One of the PDP_type values in TS 27.007 section 10.1.1. */
    if (s_rilPDPContextListRes.type != NULL) {
        free(s_rilPDPContextListRes.type);
        s_rilPDPContextListRes.type = NULL;
    }
    /* This is done due to bug in Java layer. 
     * because it does not handle NULL for interface name, even when there is no active session 
     */

    if(g_ppp_enabled)
        s_rilPDPContextListRes.ifname = "ppp0";
    else
        s_rilPDPContextListRes.ifname = ril_iface;

    /* IP Addresses */
    if (s_rilPDPContextListRes.addresses != NULL) {
        free(s_rilPDPContextListRes.addresses);
        s_rilPDPContextListRes.addresses = NULL;
    }
    /* DNS */
    if (s_rilPDPContextListRes.dnses != NULL) {
        free(s_rilPDPContextListRes.dnses);
        s_rilPDPContextListRes.dnses = NULL;
    }
    /* Gateways */
    if (s_rilPDPContextListRes.gateways != NULL) {
        free(s_rilPDPContextListRes.gateways);
        s_rilPDPContextListRes.gateways = NULL;
    }
}


/**
 *
 * Get DNS and Gateway info from system property
 * 
 * @param[out] dnspp 
 *     Pointer to the DNS address pointer
 * @param[out] gatewaypp 
 *     Pointer to the gateway address pointer
 * 
 * @return
 *     None
 *
 * @par Abort:<br> 
 *     none
 *
 * @note
 *     No space allowed at the end of DNS string(s)
 */
static void getDNSnGW(char **dnspp, char **gatewaypp)
{
    char prop_result[PROPERTY_VALUE_MAX];
    int i;
    char *property = NULL;
    char *strp[SWI_MAX_DNS];
    int strcount = 0;
    int slen = 0;
    char *outp, *origp;

    /* get DNS array */
    for (i = 1; i <= SWI_MAX_DNS; i++) {
        asprintf(&property, "dhcp.%s.dns%d", ril_iface, i);
        property_get(property, prop_result, "");
        free(property);
        if (strcmp(prop_result, "") != 0) {
            strp[i-1] = strdup(prop_result);
            strcount += strlen(prop_result);
            if (i != SWI_MAX_DNS) {
                /* leave a space for next DNS, the strcount number could be one byte more than required */
                strcount ++;
            }
        }
        else {
            strp[i-1] = NULL;
        }
    }
    if (strcount > 0) {
        outp = swimalloc((strcount+1),
                         "getDNSnGW allocate memory failed");
        origp = outp;
        memset(outp, 0, strcount);
        for (i = 0; i < SWI_MAX_DNS; i++) {
            if (strp[i] != NULL) {
                if (origp != outp) {
                    /* Append a space and point to next, no space on string start */
                    *outp = ' ';
                    outp++;
                }
                slen = strlen(strp[i]);
                memcpy(outp, strp[i], slen);
                free(strp[i]);
                /* move pointer to the next DNS */
                outp += slen;
            }
        }
        *dnspp = origp;
    }
   
    /* get gateway */
    asprintf(&property, "dhcp.%s.gateway", ril_iface);
    property_get(property, prop_result, "");
    free(property);
    *gatewaypp = strdup(prop_result);
}

static void resetDTR(void) 
{
    int iFlags;

    /* turn off DTR */
    iFlags = TIOCM_DTR;
    ioctl(g_ppp_fd, TIOCMBIC, &iFlags);
    
    sleep(1);

    /* turn on DTR */
    iFlags = TIOCM_DTR;
    ioctl(g_ppp_fd, TIOCMBIS, &iFlags);
}

/**
 *
 * send dial command to change port to PPP mode
 * 
 * @param[in] dialcmd 
 *     dial string for PPP
 *
 * @param[in] len
 *     length of dial string
 *
 * @param[out] rbuf
 *     buffer to collect dial cmd response
 *
 * @param[in] rlen
 *     length of read buffer
 * 
 * @return
 *     0 on success and -1 on error
 *
 * @par Abort:<br> 
 *     none
 */
static int sendPppDial(char *dialcmd, int len, char *rbuf, int rlen)
{
    int ret = -1;

    ret = write(g_ppp_fd, dialcmd, len);
    if (ret == -1) {
        LOGE("error on PPP dialing!\n");
        return ret;
    }    
    sleep(3);
   
    read(g_ppp_fd, rbuf, rlen);
    LOGI("PPP dial response %s\n", rbuf);
    if (strstr(rbuf,"CONNECT")) {
        ret = 0;
    }
    return ret;
}        

/**
 *
 * open port for PPP serial data communication
 * 
 * @param[in] tty_name 
 *     Pointer to the tty port
 * 
 * @return
 *     none
 *
 * @par Abort:<br> 
 *     none
 */
static void openPppPort(char *tty_name)
{
    struct termios ios;
    if (tty_name != NULL) {
        g_ppp_fd = open(tty_name, O_RDWR);
        if (g_ppp_fd >= 0) {
            LOGI("PPP port opened successfully\n");
            tcgetattr(g_ppp_fd, &ios);
            cfmakeraw(&ios);
            cfsetospeed(&ios, B115200);
            cfsetispeed(&ios, B115200);
            ios.c_cflag |= CREAD | CLOCAL;
            tcflush(g_ppp_fd, TCIOFLUSH);
            tcsetattr(g_ppp_fd, TCSANOW, &ios);
        }
        else
            LOGE("PPP port not opened, error!!\n");
    }
}        


/**
 *
 * Get DNS and for PPP from system property
 * 
 * @param[out] dnspp 
 *     Pointer to the DNS address pointerr
 * 
 * @return
 *     None
 *
 * @par Abort:<br> 
 *     none
 *
 * @note
 *     No space allowed at the end of DNS string(s)
 */
static void getDNSForPPP(char **dnspp)
{
    char prop_result[PROPERTY_VALUE_MAX];
    int i;
    char *property = NULL;
    char *strp[2];
    int strcount = 0;
    int slen = 0;
    char *outp, *origp;

    /* get DNS array */
    for (i = 1; i <= 2; i++) {
        asprintf(&property, "net.ppp0.dns%d",i);
        property_get(property, prop_result, "");
        free(property);
        if (strcmp(prop_result, "") != 0) {
            strp[i-1] = strdup(prop_result);
            strcount += strlen(prop_result);
            if (i != 2) {
                /* leave a space for next DNS, the strcount number could be one byte more than required */
                strcount ++;
            }
        }
        else {
            strp[i-1] = NULL;
        }
    }
    if (strcount > 0) {
        outp = swimalloc((strcount+1),
                         "getDNSForPPP allocate memory failed");
        origp = outp;
        memset(outp, 0, strcount);
        for (i = 0; i < 2; i++) {
            if (strp[i] != NULL) {
                if (origp != outp) {
                    /* Append a space and point to next, no space on string start */
                    *outp = ' ';
                    outp++;
                }
                slen = strlen(strp[i]);
                memcpy(outp, strp[i], slen);
                free(strp[i]);
                /* move pointer to the next DNS */
                outp += slen;
            }
        }
        *dnspp = origp;
    }
}

void clearPppProps()
{
    property_set("net.ppp.status","disconnected");
    property_set("net.interfaces.defaultroute","");
    property_set("net.ppp0.dns1","");
    property_set("net.ppp0.dns2","");
    property_set("net.ppp0.local-ip","");
    property_set("net.ppp0.remote-ip","");
}

/**
 * Stop the PPPD service if already running
 *
 * @return
 *      none
 *
 * @par Abort:<br> 
 *     none
 *
 * @note
 *     None
 */
void PPPStopService()
{
    /* TODO: Stop of this service does not send LCP
     * terminate request, but it kills the PPP daemon
     * Right now we are relying on this and file descriptor closure
     */
    if (property_set("ctl.stop", "sierra_pppd")) {
        LOGE("FAILED to set ctl.stop sierra_dhcpcd property!");
    }
    if (g_ppp_fd >= 0) {
        close(g_ppp_fd);
        g_ppp_fd = -1;
    }
    clearPppProps();
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
    int disconnectReason = RIL_DISCONNECT_REASON_NO_SPECIFIC;

    pPdpCid = ((const char **) data)[0];
    if ((((datalen/sizeof(char *)) >= RIL_REQUEST_DEACTIVATE_DATA_CALL_REQUEST_PARAMS)) && 
        (((const char **) data)[1] != NULL)) {
        disconnectReason = atoi(((const char **) data)[1]);
    }
    LOGD("%s PdpCid '%s' DisconnectReason '%d'", __func__, pPdpCid, disconnectReason);

    if (g_ppp_enabled)
    {
        /* Stop currentRadioTechnology polling */
        s_StopRadioTechPoll = true;

        /* clean up data call list when data call is deactivated */
        initDataCallResponseList();
        s_rilPDPContextListRes.status = PDP_FAIL_NONE;

        /* stop PPPD */
        //resetDTR();
        PPPStopService();
        RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
        if (disconnectReason == RIL_DISCONNECT_REASON_RADIO_SHUTDOWN) {
            err = at_send_command("AT+CFUN=0", &atresponse);
             if (err < 0 || atresponse->success == 0) {
                 LOGE("%s set low power mode failed err=%d, atresponse->success=%d\n", 
                     __func__, err, atresponse->success);
             }
            at_response_free(atresponse);
            setRadioState(RADIO_STATE_OFF);
        }
        return;
    }

#ifdef DEACTIVATE_DATA_SESSION_AFTER_MODEM_UNREG

    if (pPdpCid != NULL) {
        if (atoi(pPdpCid) == SWI_DEFAULT_CID) {
            asprintf(&pCmd, "AT!SCACT=0,%d", SWI_DEFAULT_CID);
        }
        else {
            LOGE("%s CID is invalid %s\n", __func__, pPdpCid);
            goto error;
        }
    }
    else {
        LOGE("%s CID is NULL\n", __func__);
        goto error;
    }

    err = at_send_command(pCmd, &atresponse);
    free(pCmd);
    if (err < 0  || atresponse->success == 0) {
        LOGE("%s err=%d, atresponse->success=%d\n", __func__, err, atresponse->success);
        goto error;
    }
#else

    /* Send command only if PDP context is active */
    if (s_rilPDPContextListRes.active == RIL_STATE_ACTIVE_LINKUP) {
        if (pPdpCid != NULL) {
            if (atoi(pPdpCid) == SWI_DEFAULT_CID) {
                asprintf(&pCmd, "AT!SCACT=0,%d", SWI_DEFAULT_CID);
            }
            else {
                LOGE("%s CID is invalid %s\n", __func__, pPdpCid);
                goto error;
            }
        }
        else {
            LOGE("%s CID is NULL\n", __func__);
            goto error;
        }

        err = at_send_command(pCmd, &atresponse);
        free(pCmd);
        if (err < 0  || atresponse->success == 0) {
            LOGE("%s err=%d, atresponse->success=%d\n", __func__, err, atresponse->success);
            goto error;
        }
    }
#endif   

    /* Stop currentRadioTechnology polling */
    s_StopRadioTechPoll = true;

    /* clean up data call list when data call is deactivated */
    initDataCallResponseList();
    s_rilPDPContextListRes.status = PDP_FAIL_NONE;
    /* stop DHCP client */
    checkDHCPnStopService();

    /* de-activate network ifc, ril_iface */
    activateNetworkInterface(false);
    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

finally:
    if (atresponse)
        at_response_free(atresponse);
    /* Java layer requests to power down */
    if (disconnectReason == RIL_DISCONNECT_REASON_RADIO_SHUTDOWN) {
        err = at_send_command("AT+CFUN=0", &atresponse);
        if (err < 0 || atresponse->success == 0) {
            LOGE("%s set low power mode failed err=%d, atresponse->success=%d\n", 
                 __func__, err, atresponse->success);
        }
        at_response_free(atresponse);
        setRadioState(RADIO_STATE_OFF);
        /* clean up data call list when radio is turned off */
        initDataCallResponseList();
    }
    return;
    
error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

#ifndef SWI_RIL_VERSION_12 

static void pollppptask(void *params)
{    
    struct pppPollParams *poll_params;
    char propValue[PROPERTY_VALUE_MAX];
    RIL_Token t;
    char *cmd = NULL;

    poll_params = (struct pppPollParams *) params;
    t = poll_params->t;

    if (poll_params->loopcount++ >= REPOLL_PPP)
        goto error;

    property_get("net.ppp.status", propValue,"");    
    if(strcmp(propValue,"connected") != 0) {
        LOGE("%s::polldhcptask retry  %d out of %d ", __func__, poll_params->loopcount,REPOLL_PPP);
        enqueueRILEvent(RIL_EVENT_QUEUE_NORMAL, pollppptask,
                    poll_params, &TIMEVAL_PPP_POLL);
        return;
    }
   
    property_get("net.ppp0.local-ip", propValue, "");
    s_rilPDPContextListRes.addresses = strdup(propValue);
    swicheckp(s_rilPDPContextListRes.addresses, "Duplicate IP address out of memory");
    LOGI("IP Address: %s\n", s_rilPDPContextListRes.addresses);

    property_get("net.ppp0.remote-ip", propValue, "");
    s_rilPDPContextListRes.gateways = strdup(propValue);
   
    /* Get DNSs */
    getDNSForPPP(&s_rilPDPContextListRes.dnses);
    LOGD("dns: '%s' gateway: '%s'\n", s_rilPDPContextListRes.dnses, s_rilPDPContextListRes.gateways);

    /* update cache of RIL_Data_Call_Response_v6 */
    /* status */
    s_rilPDPContextListRes.status = PDP_FAIL_NONE;
    /* CID */
    s_rilPDPContextListRes.cid = SWI_DEFAULT_CID;
    /* Data call state */
    s_rilPDPContextListRes.active = RIL_STATE_ACTIVE_LINKUP;

    s_StopRadioTechPoll = false;
    /* Data Session started successully, now start polling to get current RAT */
    startPollingForRadioTechnology();

finally:
    /* Always return successful response */
    RIL_onRequestComplete(t, RIL_E_SUCCESS, &s_rilPDPContextListRes, sizeof(RIL_Data_Call_Response_v6));
    LOGD("%s done", __func__);
    free(poll_params);
    return;

error:
    /* it will take modem to AT cmd mode from PPP mode */
    //resetDTR();
    PPPStopService();

    /* In case of failure after data call activated, let Java layer retry */
    s_lastPdpFailCause = PDP_FAIL_ERROR_UNSPECIFIED;
    s_rilPDPContextListRes.status = s_lastPdpFailCause;
    goto finally;
}

static void polldhcptask(void *params)
{
    int err = 0;
    int response = 0;
    char *linep = NULL;
    char *outp = NULL;
    int pid;
    ATResponse *atresponse = NULL;
    struct dhcpPollParams *poll_params;
    RIL_Token t;
    char *cmd = NULL;

    poll_params = (struct dhcpPollParams *) params;
    t = poll_params->t;

    if (poll_params->loopcount++ >= REPOLL_DHCP)
        goto error;

    if (PollDHCPnsetDNS() < 0) {
        LOGE("%s::PollDHCPnsetDNS retry  %d out of %d ", __func__, poll_params->loopcount,REPOLL_DHCP);
        enqueueRILEvent(RIL_EVENT_QUEUE_NORMAL, polldhcptask,
                        poll_params, &TIMEVAL_DHCP_POLL);
        return;
    }
    
    /* Sometimes IP address was not fetched in first attempt for AR75, trying again */
    if (!strcmp(s_rilPDPContextListRes.addresses,"")) {    
        if (s_AR75_Modem) {
            asprintf(&cmd, "AT+CGPADDR=%d", SWI_DEFAULT_CID);
            err = at_send_command_singleline(cmd, "+CGPADDR:", &atresponse);
        }
        else {
            asprintf(&cmd, "AT!SCPADDR=%d", SWI_DEFAULT_CID);
            err = at_send_command_singleline(cmd, "!SCPADDR:", &atresponse);
        }

        free(cmd);
        if (err < 0 || atresponse->success == 0) {
            at_response_free(atresponse);
            atresponse = NULL;
            goto error;
        }

        linep = atresponse->p_intermediates->line;
        err = at_tok_start(&linep);
        if (err < 0) {
            at_response_free(atresponse);
            atresponse = NULL;
            goto error;
        }

        err = at_tok_nextint(&linep, &pid);
        if (err < 0 || pid != SWI_DEFAULT_CID) {
            at_response_free(atresponse);
            atresponse = NULL;
            goto error;
        }

        err = at_tok_nextstr(&linep, &outp);
        if (err < 0) {
            at_response_free(atresponse);
            atresponse = NULL;
            goto error;
        }
                    
        s_rilPDPContextListRes.addresses = strdup(outp);
        swicheckp(s_rilPDPContextListRes.addresses, "Duplicate IP address out of memory");
        LOGI("IP Address: %s\n", s_rilPDPContextListRes.addresses);
    }        

    /* Get DNSs and gateways */
    getDNSnGW(&s_rilPDPContextListRes.dnses, &s_rilPDPContextListRes.gateways);
    LOGD("dns: '%s' gateway: '%s'\n", s_rilPDPContextListRes.dnses, s_rilPDPContextListRes.gateways);

    /* update cache of RIL_Data_Call_Response_v6 */
    /* status */
    s_rilPDPContextListRes.status = PDP_FAIL_NONE;
    /* CID */
    s_rilPDPContextListRes.cid = SWI_DEFAULT_CID;
    /* Data call state */
    s_rilPDPContextListRes.active = RIL_STATE_ACTIVE_LINKUP;

    s_StopRadioTechPoll = false;
    /* Data Session started successully, now start polling to get current RAT */
    startPollingForRadioTechnology();

finally:
    /* Always return successful response */
    RIL_onRequestComplete(t, RIL_E_SUCCESS, &s_rilPDPContextListRes, sizeof(RIL_Data_Call_Response_v6));
    LOGD("%s done", __func__);
    free(poll_params);
    return;

error:
    asprintf(&cmd, "AT!SCACT=0,%d", SWI_DEFAULT_CID);
    err = at_send_command(cmd, &atresponse);
    if (err < 0 || atresponse->success == 0)
    {
        /* log the deactivation error */
        LOGE("%s deactivate PDP err=%d\n", __func__, err);
    }
    free(cmd);
    at_response_free(atresponse);

    /* In case of failure after data call activated, let Java layer retry */
    s_lastPdpFailCause = PDP_FAIL_ERROR_UNSPECIFIED;
    s_rilPDPContextListRes.status = s_lastPdpFailCause;

    /* de-activate network ifc, ril_iface */
    activateNetworkInterface(false);
    goto finally;
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
 *     Returns the data call list. An entry is added when a
 *     RIL_REQUEST_SETUP_DATA_CALL is issued and removed on a
 *     RIL_REQUEST_DEACTIVATE_DATA_CALL. The list is emptied
 *     when RIL_REQUEST_RADIO_POWER off/on is issued.
 */
void requestOrSendPDPContextList(RIL_Token *t)
{
    if (isDataSessionActive()) {
        s_rilPDPContextListRes.active = RIL_STATE_ACTIVE_LINKUP;
    }
    else {
        /* no active data call */
        s_rilPDPContextListRes.active = RIL_STATE_INACTIVE;
    }
   
    if (t != NULL)
        if (s_rilPDPContextListRes.active == RIL_STATE_INACTIVE) {
            initDataCallResponseList();
            s_rilPDPContextListRes.status = PDP_FAIL_NONE;
            RIL_onRequestComplete(*t, RIL_E_SUCCESS, NULL, 0);
        }
        else {
            RIL_onRequestComplete(*t, RIL_E_SUCCESS, &s_rilPDPContextListRes,
                                  sizeof(RIL_Data_Call_Response_v6));
        }
    else {
        /* RIL_UNSOL_DATA_CALL_LIST_CHANGED is not expected to be
         * issued because of an RIL_REQUEST_DEACTIVATE_DATA_CALL.
         * And make sure no RIL_UNSOL_DATA_CALL_LIST_CHANGED for power off.
         */
        if (s_rilPDPContextListRes.status != PDP_FAIL_RADIO_POWER_OFF) {
            if (s_rilPDPContextListRes.active == RIL_STATE_INACTIVE) {
                initDataCallResponseList();
                s_rilPDPContextListRes.status = PDP_FAIL_NONE;
                RIL_onUnsolicitedResponse(RIL_UNSOL_DATA_CALL_LIST_CHANGED,
                                          NULL,
                                          0);
            }
            else {
                RIL_onUnsolicitedResponse(RIL_UNSOL_DATA_CALL_LIST_CHANGED,
                                          &s_rilPDPContextListRes,
                                          sizeof(RIL_Data_Call_Response_v6));
            }
        }
    }
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
    int err = 0;
    ATResponse *atresponse = NULL;
    RIL_RAT tech_ril;
    int pidtype;
    const char *username = NULL;
    const char *password = NULL;
    const char *authtype;
    int authentype;
    char *pdptype = NULL;
    char *linep = NULL;
    char *outp = NULL;
    int pid;
    int cme_err;
    int pdp_active = 0;
    bool isUNPresent = false;
    bool isPWPresent = false;
    struct dhcpPollParams *poll_params=NULL;
    struct pppPollParams *ppp_poll_params=NULL;
    char rbuf[512];
    char *dialStr = "ATDT*99***1#\r";

    /* Assign parameters. */
    tech_ril = atoi(((const char **)data)[RIL_PDP_PARAM_RADIO_TECH]);
    pidtype = atoi(((const char **)data)[RIL_PDP_PARAM_DATA_PROFILE]);
    APN = ((const char **) data)[RIL_PDP_PARAM_APN];
    username = (char *)(((const char **)data)[RIL_PDP_PARAM_APN_USERNAME]);
    password = (char *)(((const char **)data)[RIL_PDP_PARAM_APN_PASSWORD]);
    authtype = ((const char **)data)[RIL_PDP_PARAM_APN_AUTH_TYPE];
    if ((datalen >= (RIL_REQUEST_SETUP_DATA_CALL_REQUEST_PARAMS_MIN * sizeof(char *))) && 
        (char *)(((const char **)data)[RIL_PDP_PARAM_PDP_TYPE]) != NULL) {
        pdptype = (char *)(((const char **)data)[RIL_PDP_PARAM_PDP_TYPE]);
    }
    else {
        /* default to IPV4 */
        pdptype = "IP";
    }

    /* Start the data session for GSM, UMTS or LTE */
    if ((tech_ril != RIL_TECH_UMTS) &&
        ((tech_ril - 2) != RIL_RAT_GPRS) &&
        ((tech_ril - 2) != RIL_RAT_EDGE) &&
        ((tech_ril - 2) != RIL_RAT_UMTS) &&
        ((tech_ril - 2) != RIL_RAT_HSDPA) &&
        ((tech_ril - 2) != RIL_RAT_HSUPA) &&
        ((tech_ril - 2) != RIL_RAT_HSPA) &&
        ((tech_ril - 2) != RIL_RAT_LTE) &&
        ((tech_ril - 2) != RIL_RAT_HSPAP)) {    
        LOGE("%s requested technology %d not supported by the modem\n", __func__, tech_ril);
        RIL_onRequestComplete(t, RIL_E_MODE_NOT_SUPPORTED, NULL, 0);
        return;
    }

    if (pidtype != RIL_DATA_PROFILE_DEFAULT) {
        LOGE("%s CID type %d not supported by the modem\n", __func__, pidtype);
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    if(g_ppp_enabled) {
        s_rilPDPContextListRes.ifname = "ppp0";
    }    
    else {
        s_rilPDPContextListRes.ifname = ril_iface;

        /* activate network ifc, ril_iface */    
        activateNetworkInterface(true);
    }
    
    /* Only one data call allowed now */
    if (isDataSessionActive()) {
        /* SWI_TBD temprory disable this check for handoff */
        #if 0
        /* update status field for failure cause */
        s_rilPDPContextListRes.status = PDP_FAIL_ONLY_SINGLE_BEARER_ALLOWED;
        goto finally;
        #endif
    }
    else {
        /* Stop the DHCP or PPP client if already running */
        if(g_ppp_enabled)
            PPPStopService();
        checkDHCPnStopService();
    }
    
    /* Doesn't support IPV6 or IPV4V6 for now */
    if (strcmp(pdptype, "IP") != 0) {
        LOGE("%s only IPV4 supported\n", __func__);
        /* update status field for failure cause */
        s_lastPdpFailCause = PDP_FAIL_ONLY_IPV4_ALLOWED;
        goto error;
    }

    LOGD("%s: Radio Technology to use: %d APN '%s'\n", __func__, tech_ril, APN);    
    /* In case of AT command, let Java layer retry */
    s_lastPdpFailCause = PDP_FAIL_ERROR_UNSPECIFIED;
    
    /* Suggested retry times, default to -1 which means no value is suggested */
    s_rilPDPContextListRes.suggestedRetryTime = RIL_ACTIVATE_DATA_CALL_RETRY_NO_SUGGESTION;
    
    /* Clean up some of the cache */
    /* active */
    s_rilPDPContextListRes.active = RIL_STATE_INACTIVE;
    /* PDP type */
    if (s_rilPDPContextListRes.type != NULL) {
        free(s_rilPDPContextListRes.type);
        s_rilPDPContextListRes.type = NULL;
    }
    /* DNS */
    if (s_rilPDPContextListRes.dnses != NULL) {
        free(s_rilPDPContextListRes.dnses);
        s_rilPDPContextListRes.dnses = NULL;
    }
    /* Gateways */
    if (s_rilPDPContextListRes.gateways != NULL) {
        free(s_rilPDPContextListRes.gateways);
        s_rilPDPContextListRes.gateways = NULL;
    }
    /* IP Addresses */
    if (s_rilPDPContextListRes.addresses != NULL) {
        free(s_rilPDPContextListRes.addresses);
        s_rilPDPContextListRes.addresses = NULL;
    }

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
            LOGE("%s authentication type %d is not supported by modem", __func__, authentype);
            goto error;
        } else if (authentype == 3) {
            /* we decide to do no authentication for type 3 */
            LOGI("%s authentication type %d will do no authentication at all", __func__, authentype);
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

    if (g_ppp_enabled) {
        /* if port not opened already */
        if (g_ppp_fd == -1)
            openPppPort("/dev/ttyUSB3");
        if (g_ppp_fd >= 0) {
            memset(rbuf, 0, sizeof(rbuf));
            write(g_ppp_fd,"ATE0\r",sizeof("ATE0\r"));
            sleep(3);
            read(g_ppp_fd, rbuf, sizeof(rbuf));
            LOGI("ATE0 response on PPP port %s\n", rbuf);
            memset(rbuf, 0, sizeof(rbuf));

            /* Dial the ISP for PPP connect */            
            if(!sendPppDial(dialStr,strlen(dialStr),rbuf, sizeof(rbuf))) {
                /* Start PPP client, so that we get assigned an IP,gateway and DNS */
                if (property_set("ctl.start", "sierra_pppd")) {
                    LOGE("%s FAILED to set ctl.start sierra_pppd property!", __func__);
                    //resetDTR();
                    close(g_ppp_fd);
                    g_ppp_fd = -1; 
                    goto error;       
                } 
            }
            else {
                // TODO: do we need to reset DTR here?
                //resetDTR();
                close(g_ppp_fd);
                g_ppp_fd = -1;
                goto error;
            }
        }
        else {
            goto error;
        }

        ppp_poll_params = malloc(sizeof(struct pppPollParams));
        ppp_poll_params->loopcount = 0;
        ppp_poll_params->t = t;

        /* PDP type */
        if (pdptype != NULL)
            s_rilPDPContextListRes.type = strdup(pdptype);
        /* PPP polling maybe blocking, poll PPP result in another task */
        enqueueRILEvent(RIL_EVENT_QUEUE_NORMAL, pollppptask,
                    ppp_poll_params, &TIMEVAL_PPP_POLL);
        return;
    }

    asprintf(&cmd, "AT!SCACT=1,%d", SWI_DEFAULT_CID);
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
    if (s_AR75_Modem) {
        asprintf(&cmd, "AT+CGPADDR=%d", SWI_DEFAULT_CID);
        err = at_send_command_singleline(cmd, "+CGPADDR:", &atresponse);
    }
    else {
        asprintf(&cmd, "AT!SCPADDR=%d", SWI_DEFAULT_CID);
        err = at_send_command_singleline(cmd, "!SCPADDR:", &atresponse);
    }

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
                    
    s_rilPDPContextListRes.addresses = strdup(outp);
    swicheckp(s_rilPDPContextListRes.addresses, "Duplicate IP address out of memory");
    LOGI("IP Address: %s\n", s_rilPDPContextListRes.addresses);
        
    at_response_free(atresponse);
    atresponse = NULL; 

    /* Start dhcp client, so that we get assigned an gateway IP address and DNS */
    if (property_set("ctl.start", "sierra_dhcpcd")) {
        LOGE("%s FAILED to set ctl.start sierra_dhcpcd property!", __func__);
        goto error;
    }

    if (atresponse != NULL)
        at_response_free(atresponse);

    poll_params = malloc(sizeof(struct dhcpPollParams));
    poll_params->loopcount = 0;
    poll_params->t = t;

    /* PDP type */
    if (pdptype != NULL)
        s_rilPDPContextListRes.type = strdup(pdptype);
    /* DHCP polling maybe blocking, poll DHCP result in another task */
    enqueueRILEvent(RIL_EVENT_QUEUE_NORMAL, polldhcptask,
                    poll_params, &TIMEVAL_DHCP_POLL);
    return;

error:
    LOGE("%s err=%d\n", __func__, err);
    /* deactivate the active PDP in case of error */
    if (pdp_active == 1)
    {
        /* free at response buffer */
        if (atresponse != NULL)
        {     
            at_response_free(atresponse);
            atresponse = NULL;
        }
        asprintf(&cmd, "AT!SCACT=0,%d", SWI_DEFAULT_CID);
        err = at_send_command(cmd, &atresponse);
        if (err < 0 || atresponse->success == 0)
        {
            /* log the deactivation error */
            LOGE("%s deactivate PDP err=%d\n", __func__, err);
        }
        free(cmd);
        /* In case of failure after data call activated, let Java layer retry */
        s_lastPdpFailCause = PDP_FAIL_ERROR_UNSPECIFIED;
    }
    s_rilPDPContextListRes.status = s_lastPdpFailCause;

    if (atresponse != NULL) {
        at_response_free(atresponse);
    }

    if (!g_ppp_enabled) {
        /* de-activate network ifc, ril_iface */
        activateNetworkInterface(false);
    }

    /* Always return successful response */
    RIL_onRequestComplete(t, RIL_E_SUCCESS, &s_rilPDPContextListRes, sizeof(RIL_Data_Call_Response_v6));
    LOGD("%s done", __func__);
    return;
}

#else

static void pollppptask_r12(void *params)
{    
    struct pppPollParams *poll_params;
    char propValue[PROPERTY_VALUE_MAX];
    RIL_Token t;
    char *cmd = NULL;

    poll_params = (struct pppPollParams *) params;
    t = poll_params->t;

    if (poll_params->loopcount++ >= REPOLL_PPP)
        goto error;

    property_get("net.ppp.status", propValue,"");    
    if(strcmp(propValue,"connected") != 0) {
        LOGE("%s::polldhcptask retry  %d out of %d ", __func__, poll_params->loopcount,REPOLL_PPP);
        enqueueRILEvent(RIL_EVENT_QUEUE_NORMAL, pollppptask_r12,
                    poll_params, &TIMEVAL_PPP_POLL);
        return;
    }
   
    property_get("net.ppp0.local-ip", propValue, "");
    s_rilPDPContextListRes.addresses = strdup(propValue);
    swicheckp(s_rilPDPContextListRes.addresses, "Duplicate IP address out of memory");
    LOGI("IP Address: %s\n", s_rilPDPContextListRes.addresses);

    property_get("net.ppp0.remote-ip", propValue, "");
    s_rilPDPContextListRes.gateways = strdup(propValue);
   
    /* Get DNSs */
    getDNSForPPP(&s_rilPDPContextListRes.dnses);
    LOGD("dns: '%s' gateway: '%s'\n", s_rilPDPContextListRes.dnses, s_rilPDPContextListRes.gateways);

    /* update cache of RIL_Data_Call_Response_v11 */
    /* status */
    s_rilPDPContextListRes.status = PDP_FAIL_NONE;
    /* CID */
    s_rilPDPContextListRes.cid = SWI_DEFAULT_CID;
    /* Data call state */
    s_rilPDPContextListRes.active = RIL_STATE_ACTIVE_LINKUP;

    s_StopRadioTechPoll = false;
    /* Data Session started successully, now start polling to get current RAT */
    startPollingForRadioTechnology();

finally:
    /* Always return successful response */
    RIL_onRequestComplete(t, RIL_E_SUCCESS, &s_rilPDPContextListRes, sizeof(RIL_Data_Call_Response_v11));
    LOGD("%s done", __func__);
    free(poll_params);
    return;

error:
    /* it will take modem to AT cmd mode from PPP mode */
    //resetDTR();
    PPPStopService();

    /* In case of failure after data call activated, let Java layer retry */
    s_lastPdpFailCause = PDP_FAIL_ERROR_UNSPECIFIED;
    s_rilPDPContextListRes.status = s_lastPdpFailCause;
    goto finally;
}

static void polldhcptask_r12(void *params)
{
    int err = 0;
    int response = 0;
    char *linep = NULL;
    char *outp = NULL;
    int pid;
    ATResponse *atresponse = NULL;
    struct dhcpPollParams *poll_params;
    RIL_Token t;
    char *cmd = NULL;

    poll_params = (struct dhcpPollParams *) params;
    t = poll_params->t;

    if (poll_params->loopcount++ >= REPOLL_DHCP)
        goto error;

    if (PollDHCPnsetDNS() < 0) {
        LOGE("%s::PollDHCPnsetDNS retry  %d out of %d ", __func__, poll_params->loopcount,REPOLL_DHCP);
        enqueueRILEvent(RIL_EVENT_QUEUE_NORMAL, polldhcptask_r12,
                        poll_params, &TIMEVAL_DHCP_POLL);
        return;
    }
    
    /* Sometimes IP address was not fetched in first attempt for AR75, trying again */
    if (!strcmp(s_rilPDPContextListRes.addresses,"")) {    
        if (s_AR75_Modem) {
            asprintf(&cmd, "AT+CGPADDR=%d", SWI_DEFAULT_CID);
            err = at_send_command_singleline(cmd, "+CGPADDR:", &atresponse);
        }
        else {
            asprintf(&cmd, "AT!SCPADDR=%d", SWI_DEFAULT_CID);
            err = at_send_command_singleline(cmd, "!SCPADDR:", &atresponse);
        }

        free(cmd);
        if (err < 0 || atresponse->success == 0) {
            at_response_free(atresponse);
            atresponse = NULL;
            goto error;
        }

        linep = atresponse->p_intermediates->line;
        err = at_tok_start(&linep);
        if (err < 0) {
            at_response_free(atresponse);
            atresponse = NULL;
            goto error;
        }

        err = at_tok_nextint(&linep, &pid);
        if (err < 0 || pid != SWI_DEFAULT_CID) {
            at_response_free(atresponse);
            atresponse = NULL;
            goto error;
        }

        err = at_tok_nextstr(&linep, &outp);
        if (err < 0) {
            at_response_free(atresponse);
            atresponse = NULL;
            goto error;
        }
                    
        s_rilPDPContextListRes.addresses = strdup(outp);
        swicheckp(s_rilPDPContextListRes.addresses, "Duplicate IP address out of memory");
        LOGI("IP Address: %s\n", s_rilPDPContextListRes.addresses);
    }        

    /* Get DNSs and gateways */
    getDNSnGW(&s_rilPDPContextListRes.dnses, &s_rilPDPContextListRes.gateways);
    LOGD("dns: '%s' gateway: '%s'\n", s_rilPDPContextListRes.dnses, s_rilPDPContextListRes.gateways);

    /* update cache of RIL_Data_Call_Response_v11 */
    /* status */
    s_rilPDPContextListRes.status = PDP_FAIL_NONE;
    /* CID */
    s_rilPDPContextListRes.cid = SWI_DEFAULT_CID;
    /* Data call state */
    s_rilPDPContextListRes.active = RIL_STATE_ACTIVE_LINKUP;

    s_StopRadioTechPoll = false;
    /* Data Session started successully, now start polling to get current RAT */
    startPollingForRadioTechnology();

finally:
    /* Always return successful response */
    RIL_onRequestComplete(t, RIL_E_SUCCESS, &s_rilPDPContextListRes, sizeof(RIL_Data_Call_Response_v11));
    LOGD("%s done", __func__);
    free(poll_params);
    return;

error:
    asprintf(&cmd, "AT!SCACT=0,%d", SWI_DEFAULT_CID);
    err = at_send_command(cmd, &atresponse);
    if (err < 0 || atresponse->success == 0)
    {
        /* log the deactivation error */
        LOGE("%s deactivate PDP err=%d\n", __func__, err);
    }
    free(cmd);
    at_response_free(atresponse);

    /* In case of failure after data call activated, let Java layer retry */
    s_lastPdpFailCause = PDP_FAIL_ERROR_UNSPECIFIED;
    s_rilPDPContextListRes.status = s_lastPdpFailCause;

    /* de-activate network ifc, ril_iface */
    activateNetworkInterface(false);
    goto finally;
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
 *     Returns the data call list. An entry is added when a
 *     RIL_REQUEST_SETUP_DATA_CALL is issued and removed on a
 *     RIL_REQUEST_DEACTIVATE_DATA_CALL. The list is emptied
 *     when RIL_REQUEST_RADIO_POWER off/on is issued.
 */
void requestOrSendPDPContextList_r12(RIL_Token *t)
{
    if (isDataSessionActive()) {
        s_rilPDPContextListRes.active = RIL_STATE_ACTIVE_LINKUP;
    }
    else {
        /* no active data call */
        s_rilPDPContextListRes.active = RIL_STATE_INACTIVE;
    }
   
    if (t != NULL)
        if (s_rilPDPContextListRes.active == RIL_STATE_INACTIVE) {
            initDataCallResponseList();
            s_rilPDPContextListRes.status = PDP_FAIL_NONE;
            RIL_onRequestComplete(*t, RIL_E_SUCCESS, NULL, 0);
        }
        else {
            RIL_onRequestComplete(*t, RIL_E_SUCCESS, &s_rilPDPContextListRes,
                                  sizeof(RIL_Data_Call_Response_v11));
        }
    else {
        /* RIL_UNSOL_DATA_CALL_LIST_CHANGED is not expected to be
         * issued because of an RIL_REQUEST_DEACTIVATE_DATA_CALL.
         * And make sure no RIL_UNSOL_DATA_CALL_LIST_CHANGED for power off.
         */
        if (s_rilPDPContextListRes.status != PDP_FAIL_RADIO_POWER_OFF) {
            if (s_rilPDPContextListRes.active == RIL_STATE_INACTIVE) {
                initDataCallResponseList();
                s_rilPDPContextListRes.status = PDP_FAIL_NONE;
                RIL_onUnsolicitedResponse(RIL_UNSOL_DATA_CALL_LIST_CHANGED,
                                          NULL,
                                          0);
            }
            else {
                RIL_onUnsolicitedResponse(RIL_UNSOL_DATA_CALL_LIST_CHANGED,
                                          &s_rilPDPContextListRes,
                                          sizeof(RIL_Data_Call_Response_v11));
            }
        }
    }
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
void requestSetupDefaultPDP_r12(void *data, size_t datalen, RIL_Token t)
{
    const char *APN = NULL;
    char *cmd = NULL;
    int err = 0;
    ATResponse *atresponse = NULL;
    RIL_RAT tech_ril;
    int pidtype;
    const char *username = NULL;
    const char *password = NULL;
    const char *authtype;
    int authentype;
    char *pdptype = NULL;
    char *linep = NULL;
    char *outp = NULL;
    int pid;
    int cme_err;
    int pdp_active = 0;
    bool isUNPresent = false;
    bool isPWPresent = false;
    struct dhcpPollParams *poll_params=NULL;
    struct pppPollParams *ppp_poll_params=NULL;
    char rbuf[512];
    char *dialStr = "ATDT*99***1#\r";

    /* Assign parameters. */
    tech_ril = atoi(((const char **)data)[RIL_PDP_PARAM_RADIO_TECH]);
    pidtype = atoi(((const char **)data)[RIL_PDP_PARAM_DATA_PROFILE]);
    APN = ((const char **) data)[RIL_PDP_PARAM_APN];
    username = (char *)(((const char **)data)[RIL_PDP_PARAM_APN_USERNAME]);
    password = (char *)(((const char **)data)[RIL_PDP_PARAM_APN_PASSWORD]);
    authtype = ((const char **)data)[RIL_PDP_PARAM_APN_AUTH_TYPE];
    if ((datalen >= (RIL_REQUEST_SETUP_DATA_CALL_REQUEST_PARAMS_MIN * sizeof(char *))) && 
        (char *)(((const char **)data)[RIL_PDP_PARAM_PDP_TYPE]) != NULL) {
        pdptype = (char *)(((const char **)data)[RIL_PDP_PARAM_PDP_TYPE]);
    }
    else {
        /* default to IPV4 */
        pdptype = "IP";
    }

    /* Start the data session for GSM, UMTS or LTE */
    if ((tech_ril != RIL_TECH_UMTS) &&
        ((tech_ril - 2) != RIL_RAT_GPRS) &&
        ((tech_ril - 2) != RIL_RAT_EDGE) &&
        ((tech_ril - 2) != RIL_RAT_UMTS) &&
        ((tech_ril - 2) != RIL_RAT_HSDPA) &&
        ((tech_ril - 2) != RIL_RAT_HSUPA) &&
        ((tech_ril - 2) != RIL_RAT_HSPA) &&
        ((tech_ril - 2) != RIL_RAT_LTE) &&
        ((tech_ril - 2) != RIL_RAT_HSPAP)) {    
        LOGE("%s requested technology %d not supported by the modem\n", __func__, tech_ril);
        RIL_onRequestComplete(t, RIL_E_MODE_NOT_SUPPORTED, NULL, 0);
        return;
    }

    if (pidtype != RIL_DATA_PROFILE_DEFAULT) {
        LOGE("%s CID type %d not supported by the modem\n", __func__, pidtype);
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    if(g_ppp_enabled) {
        s_rilPDPContextListRes.ifname = "ppp0";
    }    
    else {
        s_rilPDPContextListRes.ifname = ril_iface;

        /* activate network ifc, ril_iface */    
        activateNetworkInterface(true);
    }
    
    /* Only one data call allowed now */
    if (isDataSessionActive()) {
        /* SWI_TBD temprory disable this check for handoff */
        #if 0
        /* update status field for failure cause */
        s_rilPDPContextListRes.status = PDP_FAIL_ONLY_SINGLE_BEARER_ALLOWED;
        goto finally;
        #endif
    }
    else {
        /* Stop the DHCP or PPP client if already running */
        if(g_ppp_enabled)
            PPPStopService();
        checkDHCPnStopService();
    }
    
    /* Doesn't support IPV6 or IPV4V6 for now */
    if (strcmp(pdptype, "IP") != 0) {
        LOGE("%s only IPV4 supported\n", __func__);
        /* update status field for failure cause */
        s_lastPdpFailCause = PDP_FAIL_ONLY_IPV4_ALLOWED;
        goto error;
    }

    LOGD("%s: Radio Technology to use: %d APN '%s'\n", __func__, tech_ril, APN);    
    /* In case of AT command, let Java layer retry */
    s_lastPdpFailCause = PDP_FAIL_ERROR_UNSPECIFIED;
    
    /* Suggested retry times, default to -1 which means no value is suggested */
    s_rilPDPContextListRes.suggestedRetryTime = RIL_ACTIVATE_DATA_CALL_RETRY_NO_SUGGESTION;
    
    /* Clean up some of the cache */
    /* active */
    s_rilPDPContextListRes.active = RIL_STATE_INACTIVE;
    /* PDP type */
    if (s_rilPDPContextListRes.type != NULL) {
        free(s_rilPDPContextListRes.type);
        s_rilPDPContextListRes.type = NULL;
    }
    /* DNS */
    if (s_rilPDPContextListRes.dnses != NULL) {
        free(s_rilPDPContextListRes.dnses);
        s_rilPDPContextListRes.dnses = NULL;
    }
    /* Gateways */
    if (s_rilPDPContextListRes.gateways != NULL) {
        free(s_rilPDPContextListRes.gateways);
        s_rilPDPContextListRes.gateways = NULL;
    }
    /* IP Addresses */
    if (s_rilPDPContextListRes.addresses != NULL) {
        free(s_rilPDPContextListRes.addresses);
        s_rilPDPContextListRes.addresses = NULL;
    }

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
            LOGE("%s authentication type %d is not supported by modem", __func__, authentype);
            goto error;
        } else if (authentype == 3) {
            /* we decide to do no authentication for type 3 */
            LOGI("%s authentication type %d will do no authentication at all", __func__, authentype);
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

    if (g_ppp_enabled) {
        /* if port not opened already */
        if (g_ppp_fd == -1)
            openPppPort("/dev/ttyUSB3");
        if (g_ppp_fd >= 0) {
            memset(rbuf, 0, sizeof(rbuf));
            write(g_ppp_fd,"ATE0\r",sizeof("ATE0\r"));
            sleep(3);
            read(g_ppp_fd, rbuf, sizeof(rbuf));
            LOGI("ATE0 response on PPP port %s\n", rbuf);
            memset(rbuf, 0, sizeof(rbuf));

            /* Dial the ISP for PPP connect */            
            if(!sendPppDial(dialStr,strlen(dialStr),rbuf, sizeof(rbuf))) {
                /* Start PPP client, so that we get assigned an IP,gateway and DNS */
                if (property_set("ctl.start", "sierra_pppd")) {
                    LOGE("%s FAILED to set ctl.start sierra_pppd property!", __func__);
                    //resetDTR();
                    close(g_ppp_fd);
                    g_ppp_fd = -1; 
                    goto error;       
                } 
            }
            else {
                // TODO: do we need to reset DTR here?
                //resetDTR();
                close(g_ppp_fd);
                g_ppp_fd = -1;
                goto error;
            }
        }
        else {
            goto error;
        }

        ppp_poll_params = malloc(sizeof(struct pppPollParams));
        ppp_poll_params->loopcount = 0;
        ppp_poll_params->t = t;

        /* PDP type */
        if (pdptype != NULL)
            s_rilPDPContextListRes.type = strdup(pdptype);
        /* PPP polling maybe blocking, poll PPP result in another task */
        enqueueRILEvent(RIL_EVENT_QUEUE_NORMAL, pollppptask_r12,
                    ppp_poll_params, &TIMEVAL_PPP_POLL);
        return;
    }

    asprintf(&cmd, "AT!SCACT=1,%d", SWI_DEFAULT_CID);
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
    if (s_AR75_Modem) {
        asprintf(&cmd, "AT+CGPADDR=%d", SWI_DEFAULT_CID);
        err = at_send_command_singleline(cmd, "+CGPADDR:", &atresponse);
    }
    else {
        asprintf(&cmd, "AT!SCPADDR=%d", SWI_DEFAULT_CID);
        err = at_send_command_singleline(cmd, "!SCPADDR:", &atresponse);
    }

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
                    
    s_rilPDPContextListRes.addresses = strdup(outp);
    swicheckp(s_rilPDPContextListRes.addresses, "Duplicate IP address out of memory");
    LOGI("IP Address: %s\n", s_rilPDPContextListRes.addresses);
        
    at_response_free(atresponse);
    atresponse = NULL; 

    /* Start dhcp client, so that we get assigned an gateway IP address and DNS */
    if (property_set("ctl.start", "sierra_dhcpcd")) {
        LOGE("%s FAILED to set ctl.start sierra_dhcpcd property!", __func__);
        goto error;
    }

    if (atresponse != NULL)
        at_response_free(atresponse);

    poll_params = malloc(sizeof(struct dhcpPollParams));
    poll_params->loopcount = 0;
    poll_params->t = t;

    /* PDP type */
    if (pdptype != NULL)
        s_rilPDPContextListRes.type = strdup(pdptype);
    /* DHCP polling maybe blocking, poll DHCP result in another task */
    enqueueRILEvent(RIL_EVENT_QUEUE_NORMAL, polldhcptask_r12,
                    poll_params, &TIMEVAL_DHCP_POLL);
    return;

error:
    LOGE("%s err=%d\n", __func__, err);
    /* deactivate the active PDP in case of error */
    if (pdp_active == 1)
    {
        /* free at response buffer */
        if (atresponse != NULL)
        {     
            at_response_free(atresponse);
            atresponse = NULL;
        }
        asprintf(&cmd, "AT!SCACT=0,%d", SWI_DEFAULT_CID);
        err = at_send_command(cmd, &atresponse);
        if (err < 0 || atresponse->success == 0)
        {
            /* log the deactivation error */
            LOGE("%s deactivate PDP err=%d\n", __func__, err);
        }
        free(cmd);
        /* In case of failure after data call activated, let Java layer retry */
        s_lastPdpFailCause = PDP_FAIL_ERROR_UNSPECIFIED;
    }
    s_rilPDPContextListRes.status = s_lastPdpFailCause;

    if (atresponse != NULL) {
        at_response_free(atresponse);
    }

    if (!g_ppp_enabled) {
        /* de-activate network ifc, ril_iface */
        activateNetworkInterface(false);
    }

    /* Always return successful response */
    RIL_onRequestComplete(t, RIL_E_SUCCESS, &s_rilPDPContextListRes, sizeof(RIL_Data_Call_Response_v11));
    LOGD("%s done", __func__);
    return;
}
#endif

