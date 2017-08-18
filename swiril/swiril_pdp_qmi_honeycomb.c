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
#include <telephony/ril.h>
#include <cutils/properties.h>
#include <string.h>

#define LOG_TAG "RIL"
#include "swiril_log.h"

#include "swiril_misc.h"
#include "swiril_main.h"
#include "swiril_misc_qmi.h"
#include "swiril_device_qmi.h"
#include "swiril_pdp_common.h"
#include "swiril_pdp_qmi.h"
#include "swiril_network_qmi.h"
#include "swiril_main_qmi.h"

/* For QMI */
#include "qmerrno.h"
#include "sludefs.h"
#include "SWIWWANCMAPI.h"

extern BOOL Is9x15Device();
static ULONG s_sessionIDQMI = 0;

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
local struct ssdatasession_params s46;

struct dhcpPollParams {
    RIL_Token t;
    int loopcount;
    int pdp_active;
};
static const struct timeval TIMEVAL_DHCP_POLL = { 3, 0 };
#define REPOLL_DHCP 10
#define IPSTRSIZE_MAXIMUM 17
int PollDHCPnsetDNS(void);

/**
 *
 * Map the IP family preference string to SLQS enum value
 *
 * @param[in] IPPrefp 
 *     Pointer to the IP family preference string
 *
 * @return
 *     enum QMI_IPFamilyPreference value
 *
 * @par Abort:<br> 
 *     none
 *
 * @note
 *     This function performs no pointer validation
 */
static QMI_IPFamilyPreference getIPFamilyPreference(char *IPPrefp)
{
    QMI_IPFamilyPreference IPFamilyPreference = QMI_IP_ADDRESS_UNSPEC;
    
    if(IPPrefp)
    {
        if ((0 == strcmp(IPPrefp, "IP")) || (0 == strcmp(IPPrefp, "IPV4"))) {
            IPFamilyPreference = QMI_IPV4_ADDRESS;
        }
        else if (0 == strcmp(IPPrefp, "IPV6")) {
            IPFamilyPreference = QMI_IPV6_ADDRESS;
        }
        else if (0 == strcmp(IPPrefp, "IPV4V6")) {
            IPFamilyPreference = QMI_IPV4V6_ADDRESS;
        }
    }
    return IPFamilyPreference;
}

/**
 *
 * Get the list of PCSCF IP addresses for the current connection and convert
 * it into a string in proper dotted-decimal notation. 
 *
 * @param[IN] AddrList 
 *     List of Addresses in unsigned long format to convert to ASCII
 * @param [out]
 *     ipaddrStrp pointer to pointer to storage for the allocated string 
 *
 * @return
 *     none
 *
 * @par Abort:<br> 
 *     none
 *
 * @note
 *     The caller function is responsible to free the memory
 */
static void getPCSCFAddresses2Str( struct PCSCFIPv4ServerAddressList AddrList, char **ipaddrStrp )
{
    char *strp = NULL;
    char *strtmp = NULL;
    int i;

    if (AddrList.numInstances == 0) {
        *ipaddrStrp = NULL;
        return;
    }

    /* Allocate memory for the string. Caller must remember to free it */
    strp = swimalloc((IPSTRSIZE_MAXIMUM * (AddrList.numInstances)) + 1, "getPCSCFAddresses2Str");

    for (i = 0; i < AddrList.numInstances; i++) {
        getIPAddressStr(AddrList.pscsfIPv4Addr[i], &strtmp);
        if ( i == 0)    
            strncpy(strp, strtmp, IPSTRSIZE_MAXIMUM);
        else
            strcat(strp, strtmp);    
        free(strtmp);
        /* append a space */
        strcat(strp, " ");
    }
    /* Assign the return value */
    *ipaddrStrp = strp;
}

bool isAPNProfileIdentical(CHAR *pAPNName, ULONG *pAuthentication, CHAR  *pUsername, CHAR  *pPassword)
{
    BYTE  nStringSize = 0xff;
    ULONG profileType = 0;
    ULONG PDPType;
    ULONG IPAddress;
    ULONG PrimaryDNS;
    ULONG SecondaryDNS;
    ULONG oldAuthentication;
    BYTE  nameSize = nStringSize-1;
    CHAR  oldName[nStringSize];
    BYTE  apnSize = nStringSize-1;
    CHAR  oldAPNName[nStringSize];
    BYTE  userSize = nStringSize-1;
    CHAR  oldUsername[nStringSize];
    BYTE  pwdSize = nStringSize-1;
    CHAR  oldPwd[nStringSize];
    ULONG rc;
    
    if(!pAPNName || !pAuthentication || !pUsername || !pPassword)
    {
        LOGE("%s invalid parameters", __func__);
        return false;
    }
    
    rc = GetDefaultProfile(profileType,
                            &PDPType,
                            &IPAddress,
                            &PrimaryDNS,
                            &SecondaryDNS,
                            &oldAuthentication,
                            nameSize,
                            &oldName[0],
                            apnSize,
                            &oldAPNName[0],
                            userSize,
                            &oldUsername[0],
                            pwdSize,
                            &oldPwd[0]);                             
    if (rc != eQCWWAN_ERR_NONE) {
        LOGE("%s failed to get default profile with error=%lu", __func__, rc);
        return false;
    }
    else {
                
        if(strcmp(oldAPNName,pAPNName)) {
            LOGI("%s profile APN updated from %s to %s", __func__, oldAPNName, pAPNName);
            return false;
        }
            
        if(strcmp(oldUsername, pUsername)) {
            LOGI("%s profile Username updated from %s to %s", __func__, oldUsername, pUsername);
            return false;
        }
            
        if(strcmp(oldPwd, pPassword)) {
            LOGI("%s profile Username updated from %s to %s", __func__, oldPwd, pPassword);
            return false;
        }
            
        if(oldAuthentication != *pAuthentication) {
            LOGI("%s profile Authentication updated from %d to %d", __func__, oldAuthentication, *pAuthentication);
            return false;
        }
        
        return true;
    }    
}

#ifndef SLQS_2X
/* forced to use instance 0 (multi-pdn not supported) */
static ULONG  StartDataSession2(
    ULONG *pTechnology,
    ULONG *pPrimaryDNS,
    ULONG *pSecondaryDNS,
    ULONG *pPrimaryNBNS,
    ULONG *pSecondaryNBNS,
    CHAR  *pAPNName,
    ULONG *pIPAddress,
    ULONG *pAuthentication,
    CHAR  *pUsername,
    CHAR  *pPassword,
    ULONG *pSessionIdV4,
    ULONG *pSessionIdV6,
    ULONG *pFailureReason,
    CHAR IPFamilyPreference)
{
    /* For set default profile */
    ULONG rc;               /* result code */
    ULONG profileType = 0;  /* 0:3GPP 1:3GPP2*/
    ULONG PDPType = 0;      /* 0:IPV4 */

    /* For start data session */
    BYTE ipfaimly = IPFamilyPreference;      /* 4:IPV4, 6:IPV6 or 7:IPV4V6*/
    ULONG tech = 1;         /* 1:UMTS */
    /*
     * Profile ID will be determined on the basis of device type
     * MC7750 device - Default Profile ID is 3
     * Other devices - Default Profile ID is 1
     */
    ULONG prof = 1;         /* default profile  */
    BOOL action = 1;        /* 1:start 0:stop */


    if(QMI_TECH_UMTS == *pTechnology)
    {
        BYTE  nStringSize = 0xff;
        CHAR modelType[nStringSize];
        
        memset(modelType, 0, nStringSize);
        rc = GetModelID(nStringSize, (CHAR*)&modelType);
        if((rc == eQCWWAN_ERR_NONE) && isDualModeModule())
        {
            prof = 3;
            LOGD("%s - use profile %lu", __func__, prof );
        }
#ifndef SWI_RIL_VERSION_9
        /*Tested ok for MC8355 UMTS, SL9090 UMTS, MC7710 */
        if(!isDualModeModule() || (strcmp(modelType,"MC7750") == 0))
        {
            if (!isAPNProfileIdentical(pAPNName, pAuthentication,pUsername,pPassword)) 
            {
                rc = SetDefaultProfile(profileType,
                             &PDPType,
                             pIPAddress,
                             pPrimaryDNS,
                             pSecondaryDNS,
                             pAuthentication,
                             "android_prof",
                             pAPNName,
                             pUsername,
                             pPassword);

                if (rc != eQCWWAN_ERR_NONE)
                {
                    LOGE("%s failed to set default profile with error=%lu", __func__, rc);
                }
            }      
        }
#endif        
        s46.action = action;
        s46.instanceId = 0;/* InstanceId  will be same as  sessionNum*/
        s46.pTechnology = &tech;
        s46.pProfileId3GPP = &prof;
        s46.pProfileId3GPP2 = NULL;
        s46.ipfamily = ipfaimly;
    }
    else
    {
        /*Tested ok for MC8355 CDMA, SL9090 CDMA, MC7750 */
        tech = 2;   /* 2:3GPP2 */
        prof = 0;   /* 0 is ok on callbox, need to check with VZW */

        BYTE  nStringSize = 0xff;
        CHAR modelType[nStringSize];
        
        memset(modelType, 0, nStringSize);
        rc = GetModelID(nStringSize, (CHAR*)&modelType);
        if((rc == eQCWWAN_ERR_NONE) && (strcmp(modelType,"MC7750") == 0))
        {
            prof = 3;
            LOGD("%s - use profile %lu", __func__, prof );
        }

        s46.action = action;
        s46.instanceId = 0;/* InstanceId  will be same as  sessionNum*/
        s46.pTechnology = &tech;
        s46.pProfileId3GPP = NULL;

        /*fixme, IsGobiDevice() need to check MC73 MC78 WP71*/
        if (IsGobiDevice()  &&  !Is9x15Device() && !Is9x30Device()) {
            s46.pProfileId3GPP2 = NULL;   /* for SL9090, MC8355 only , force to use default */
        }    
        else
        {
            bool bRet = false;
            enum eGobiImageCarrier image;
            bRet = getFirmwareInfoCarrierQMI(&image);
            if (bRet && (image == eGOBI_IMG_CAR_SPRINT))
                s46.pProfileId3GPP2 = NULL;   /* for Sprint*/
            else
                s46.pProfileId3GPP2 = &prof;    /* for MC7750, MC7350, MC7355, use (default profile + 100) */
        }    
        s46.ipfamily = ipfaimly;
    }

    /* Starts the data session */
    rc = SLQSStartStopDataSession(&s46);
    
    if ((rc != eQCWWAN_ERR_NONE)&&
        (rc != eQCWWAN_ERR_SWICM_V4UP_V6UP) &&
        (rc != eQCWWAN_ERR_SWICM_V4UP_V6DWN) &&
        (rc != eQCWWAN_ERR_SWICM_V4DWN_V6UP))
    {
        /* An error occurred */
        LOGD("%s connection result: nRet: %d, Reason: %d\n",
             __func__, (int) rc, (int) s46.failureReason);
    }
    else
    {
        if(ipfaimly == 4)
        {
            LOGD("%s connection result: nRet: %d, v4sessionId: %d\n",
                __func__, (int) rc, (int) s46.v4sessionId);
            *pSessionIdV4 = s46.v4sessionId;
        } 
        else if(ipfaimly == 6) {
            LOGD("%s connection result: nRet: %d, v6sessionId: %d\n",
                __func__, (int) rc, (int) s46.v6sessionId);
            *pSessionIdV6 = s46.v6sessionId;
        }
        else if (ipfaimly == 7) {
           LOGD("%s connection result: nRet: %d, v4sessionId: %d, v6sessionId: %d\n",
                __func__, (int) rc, (int) s46.v4sessionId, (int) s46.v6sessionId);
            *pSessionIdV4 = s46.v4sessionId;    
            *pSessionIdV6 = s46.v6sessionId;
            rc = eQCWWAN_ERR_NONE;
        }
    }    
    return rc;
}

ULONG StopDataSession(ULONG sessionId)
{
    ULONG rc;
    BOOL action = 0;        /* 1:start 0:stop */

    /* Data session values are internally managed by SLQS - session ID
     * value passed here has no impact. Error codes returned provide the
     * result of IPv4v6 session stop */
    s46.action = action;
    rc = SLQSStartStopDataSession(&s46);

    if (rc != eQCWWAN_ERR_NONE)
    {
        /* An error occurred */
        LOGD("%s disconnection fail: nRet: %d, Reason: %d \n",
             __func__, (int) rc, (int) s46.failureReason);
    }
    else
    {
        LOGD("%s disconnection succeed: nRet: %d\n",
             __func__, (int) rc);
    }
    return rc;
}
#endif


/**
 * RIL_REQUEST_ALLOW_DATA
 * 
 * Requests to allow or disallow data
 *
 * TODO: Request is introduced in Android 6,
 * Response is alwaus success for now, may be handled later in detail
 *  
 */
void requestAllowData(void *data, size_t datalen, RIL_Token t)
{
    const int allowData = ((int *) data)[0];
    LOGD("Requesting allow or disallow data: %d\n", allowData);
    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL,0);
}

#ifndef SWI_RIL_VERSION_12

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
void requestOrSendPDPContextListQMI(RIL_Token *t)
{
    /* Network interface name, never change right now */
    s_rilPDPContextListRes.ifname = ril_iface;
    
    /* START MOTO 172,174*/
    if (isDataSessionActive()) {
        s_rilPDPContextListRes.active = RIL_STATE_ACTIVE_LINKUP;
    }
    else {
        /* Clear data call Response List, this will set 
         * s_rilPDPContextListRes.active = RIL_STATE_INACTIVE
         */
            initDataCallResponseList();
            s_rilPDPContextListRes.status = PDP_FAIL_NONE;
        }
    /* Android CDMA onDataStateChanged() will process either NULL or data call list.
     * GSM onDataStateChanged() will only process data call list. For compatibility
     * with both, always return s_rilPDPContextListRes even when session
     * is inactive.
     */
    if (t != NULL) {
            RIL_onRequestComplete(*t, RIL_E_SUCCESS, &s_rilPDPContextListRes,
                                  sizeof(RIL_Data_Call_Response_v6));
        }
    else {
        /* RIL_UNSOL_DATA_CALL_LIST_CHANGED is not expected to be
         * issued because of an RIL_REQUEST_DEACTIVATE_DATA_CALL.
         * And make sure no RIL_UNSOL_DATA_CALL_LIST_CHANGED for power off.
         */
        if (s_rilPDPContextListRes.status != PDP_FAIL_RADIO_POWER_OFF) {
                RIL_onUnsolicitedResponse(RIL_UNSOL_DATA_CALL_LIST_CHANGED,
                                          &s_rilPDPContextListRes,
                                          sizeof(RIL_Data_Call_Response_v6));
            }
        }
    /* STOP MOTO 172,174 */
    return;
            
}

static void polldhcptask(void *params)
{
    struct dhcpPollParams *poll_params;
    RIL_Token t;
    ULONG nRet;
    int pdp_active;    
    enum   eGobiImageCarrier GobiImageCarrier;
    
    LOGD("%s::Entered ", __func__);

    poll_params = (struct dhcpPollParams *) params;
    t = poll_params->t;
    pdp_active = poll_params->pdp_active;

    if (poll_params->loopcount++ >= REPOLL_DHCP)
        goto error;

    if (PollDHCPnsetDNS() < 0) {
        LOGE("%s::PollDHCPnsetDNS retry  %d out of %d ", __func__, poll_params->loopcount,REPOLL_DHCP);
        enqueueRILEvent(RIL_EVENT_QUEUE_NORMAL, polldhcptask,
                        poll_params, &TIMEVAL_DHCP_POLL);
        return;
    }

    s_rilPDPContextListRes.status = PDP_FAIL_NONE;
    /* CID */
    s_rilPDPContextListRes.cid = SWI_DEFAULT_CID;
    /* Data call state */
    s_rilPDPContextListRes.active = RIL_STATE_ACTIVE_LINKUP;
   
    /* Fast Dormancy supported only for SPRINT carrier devices at this time */
    if (getFirmwareInfoCarrierQMI(&GobiImageCarrier) && (GobiImageCarrier == eGOBI_IMG_CAR_SPRINT))
    {
        nRet = SLQSWdsGoDormant();
        if (eQCWWAN_ERR_NONE != nRet) {
            LOGE("%s::SLQSWdsGoDormant failed, nRet: %lu\n",__func__, nRet );
            /* not support in MC73xx, not error */
            goto finally;
        }
        LOGD("%s::SLQSWdsGoDormant Success\n",__func__ );
    }   
 
finally:
   
    /* Always return successful response */
    RIL_onRequestComplete(t, RIL_E_SUCCESS, &s_rilPDPContextListRes, sizeof(RIL_Data_Call_Response_v6));
    free (params);
    LOGD("requestSetupDefaultPDP done");
    return;

error:
    LOGD("requestSetupDefaultPDP error handling");
    /* This is work around to get eHRPD from data bearer */
    if (!getPropertyRadioNotifiction() && isDualModeRunningCDMA()) {
        registerDormancyStatusCallbackQMI(false);
    }
    if ((pdp_active == 1) && (s_sessionIDQMI != 0)) {
        /* CUS90903: Workaround for data connnection lost during idleness:
         * Always send ERROR_UNSPECIFIED, because PROTOCOL_ERRORS is treated
         * as permanenet failure at framework
         * PDP_FAIL_ERROR_UNSPECIFIED will kick off retrying of data call
         */
        setLastPdpFailCauseQMI(PDP_FAIL_ERROR_UNSPECIFIED);

        isDataSessionCloseByClient(true);
        /* deactivate PDP context */
        nRet = StopDataSession ( s_sessionIDQMI );

        if ((nRet == eQCWWAN_ERR_NONE) || (nRet == eQCWWAN_ERR_QMI_NO_EFFECT)) {
            LOGI("requestSetupDefaultPDPQMI error handle: Stopped the session %lu\n", s_sessionIDQMI);
            s_sessionIDQMI = 0;
            /* stop DHCP client */
            checkDHCPnStopService();
            if (nRet == eQCWWAN_ERR_QMI_NO_EFFECT) {
                isDataSessionCloseByClient(false);
            }
        }
        else
        {
            LOGE("%s:: error handle: Stopped the session %lu failed with %lu \n", __func__, s_sessionIDQMI, nRet);
            isDataSessionCloseByClient(false);
        }
    }
    s_rilPDPContextListRes.status = getLastPdpFailCauseQMI();

    /* de-activate network ifc, ril_iface */
    activateNetworkInterface(false);
    goto finally;
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
void requestSetupDefaultPDPQMI(void *data, size_t datalen, RIL_Token t)
{
    const char *APN = NULL;
    char *property = NULL;
    int tech_ril;
    ULONG tech_qmi;
    int pidtype;
    char *username = NULL;
    char *password = NULL;
    const char *authtype;
    int authentype;
    int *authentypep;
    int pid;
    int pdp_active = 0;
    
    ULONG nRet;
    ULONG IPAddress = 0;
    ULONG sessionIDv4 = 0;
    ULONG sessionIDv6 = 0;
    ULONG FailureReason = 0;
    ULONG PDPType = GOBI_DEFAULT_PDP_TYPE;
    char *pdptype = NULL;
    int i;
    char *ipAddrStrp = NULL;
    USHORT IPAddressV6[8];
    BYTE IPv6prefixlen = 0;
    BYTE IPFamilyPreference = QMI_IPV4_ADDRESS;

    ULONG  v4sessionId;
    ULONG  v6sessionId;
    CHAR ProfileName[NUM_MAX_STR_LEN];
    ULONG PdpType;
    CHAR APNName[NUM_MAX_STR_LEN];
    ULONG PrimaryDNSV4;
    ULONG SecondaryDNSV4;
    struct UMTSQoS UMTSGrantedQos;
    struct GPRSQoS GPRSGrantedQos;
    CHAR Username[NUM_MAX_STR_LEN];
    ULONG Authentication;
    ULONG IPAddressV4;
    struct ProfileIdentifier ProfileID;
    ULONG GWAddressV4;
    ULONG SubnetMaskV4;
    BYTE PCSCFAddrPCO;
    struct PCSCFIPv4ServerAddressList ServerAddrList;
    struct PCSCFFQDNAddressList PCSCFFQDNAddrList;
    USHORT PrimDNSV6[8];
    USHORT SecondDNSV6[8];
    ULONG Mtu;
    struct DomainNameList DomainList;
    BYTE IPFamilyPref;
    BYTE IMCNflag;
    WORD Technology;
    struct IPV6AddressInfo IPV6AddrInfo;
    struct IPV6GWAddressInfo IPV6GWAddrInfo;
    struct WdsRunTimeSettings runTimeSettings;
    enum   eGobiImageCarrier GobiImageCarrier;
    struct dhcpPollParams *poll_params=NULL;

    runTimeSettings.v4sessionId = &v4sessionId;
    runTimeSettings.v6sessionId = &v6sessionId;
    runTimeSettings.rts.pProfileName = &ProfileName[0];
    runTimeSettings.rts.pPDPType = &PdpType;
    runTimeSettings.rts.pAPNName = &APNName[0];
    runTimeSettings.rts.pPrimaryDNSV4 =  &PrimaryDNSV4;
    runTimeSettings.rts.pSecondaryDNSV4 = &SecondaryDNSV4;
    runTimeSettings.rts.pUMTSGrantedQoS = &UMTSGrantedQos;
    runTimeSettings.rts.pGPRSGrantedQoS = &GPRSGrantedQos;
    runTimeSettings.rts.pUsername = &Username[0];
    runTimeSettings.rts.pAuthentication = &Authentication;
    runTimeSettings.rts.pIPAddressV4 = &IPAddressV4;
    runTimeSettings.rts.pProfileID = &ProfileID;
    runTimeSettings.rts.pGWAddressV4 = &GWAddressV4;
    runTimeSettings.rts.pSubnetMaskV4 = &SubnetMaskV4;
    runTimeSettings.rts.pPCSCFAddrPCO = &PCSCFAddrPCO;
    runTimeSettings.rts.pServerAddrList = &ServerAddrList;
    runTimeSettings.rts.pPCSCFFQDNAddrList = &PCSCFFQDNAddrList;
    runTimeSettings.rts.pPrimaryDNSV6 = &PrimDNSV6[0];
    runTimeSettings.rts.pSecondaryDNSV6 = &SecondDNSV6[0];
    runTimeSettings.rts.pMtu = &Mtu;
    runTimeSettings.rts.pDomainList = &DomainList;
    runTimeSettings.rts.pIPFamilyPreference = &IPFamilyPref;
    runTimeSettings.rts.pIMCNflag = &IMCNflag;
    runTimeSettings.rts.pTechnology = &Technology;
    runTimeSettings.rts.pIPV6AddrInfo = &IPV6AddrInfo;
    runTimeSettings.rts.pIPV6GWAddrInfo = &IPV6GWAddrInfo;

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

    /* Network interface name, never change right now */
    s_rilPDPContextListRes.ifname = ril_iface;

    /* activate network ifc, ril_iface */
    activateNetworkInterface(true);
    
    /* Check radio state first */
    if ((currentState() == RADIO_STATE_OFF) || (currentState() == RADIO_STATE_UNAVAILABLE)) {
        LOGE("requestSetupDefaultPDPQMI radio is not avaliable\n");
        setLastPdpFailCauseQMI(PDP_FAIL_RADIO_POWER_OFF);
        goto error;
    }
    
    IPFamilyPreference = getIPFamilyPreference(pdptype);
    if (IPFamilyPreference == QMI_IP_ADDRESS_UNSPEC) {
        LOGE("%s IPFamilyPreference unspecified\n", __func__);
        /* update status field for failure cause */
        s_rilPDPContextListRes.status = PDP_FAIL_UNKNOWN_PDP_ADDRESS_TYPE;       
        goto error;
    }

    /* Suggested retry times, default to -1 which means no value is suggested */
    LOGD("%s:: Radio Technology to use: %d\n", __func__, tech_ril);
    s_rilPDPContextListRes.suggestedRetryTime = RIL_ACTIVATE_DATA_CALL_RETRY_NO_SUGGESTION;

    /* SWI_TBD temprory disable this check for handoff */
#if 0
    /* Only support 1 data connection for now */
    if ((s_rilPDPContextListRes.active == RIL_STATE_ACTIVE_LINKUP) && (s_sessionIDQMI != 0)) {
        LOGE("requestSetupDefaultPDPQMI only 1 data connection supported by the modem\n");
        /* update status field for failure cause */
        s_rilPDPContextListRes.status = PDP_FAIL_ONLY_SINGLE_BEARER_ALLOWED;
        goto finally;
    }
#endif
    /* Gobi device only support IPV4 now */
    if (IsGobiDevice()  &&  !Is9x15Device() &&  !Is9x30Device()) {
        if (IPFamilyPreference != QMI_IPV4_ADDRESS) {
            LOGE("%s Gobi device only supports IPV4\n", __func__);
            /* update status field for failure cause */
            s_rilPDPContextListRes.status = PDP_FAIL_ONLY_IPV4_ALLOWED;           
            goto error;
        }
    }

    /* Clean up some of the cache */
    /* active */
    s_rilPDPContextListRes.active = RIL_STATE_INACTIVE;
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

    /* Stop the DHCP client if already running */
    if (!isDataSessionActive()) {
        checkDHCPnStopService();
    }

    LOGD("requesting data connection to APN '%s' IP preference '%s'", APN, pdptype);

    /* This is work around to get eHRPD from data bearer */
    if (!getPropertyRadioNotifiction() && isDualModeRunningCDMA()) {
        registerDormancyStatusCallbackQMI(true);
    }
    
    /* Start the data session for either UMTS or CDMA */
    if ((tech_ril == RIL_TECH_UMTS) ||
        (tech_ril -2 == RIL_RAT_GPRS) ||
        (tech_ril -2 == RIL_RAT_EDGE) ||
        (tech_ril -2 == RIL_RAT_UMTS) ||
        (tech_ril -2 == RIL_RAT_HSDPA) ||
        (tech_ril -2 == RIL_RAT_HSUPA) ||
        (tech_ril -2 == RIL_RAT_HSPA) ||
        (tech_ril -2 == RIL_RAT_LTE) ||
        (tech_ril -2 == RIL_RAT_HSPAP))
    {
        tech_qmi = QMI_TECH_UMTS;

        /* Determine PDP authentication type, if it was specified */
        authentypep = NULL;
        if (authtype != NULL) {
            authentype = atoi(authtype);
            authentypep = &authentype;

            /* check authentication type, RIL supports 0, 1, 2, 3 */
            if ((authentype < 0) || (authentype > 3)) {
                LOGE("requestSetupDefaultPDPQMI authentication type %d is not supported by modem", authentype);
                goto error;
            }
        }
        /* Start data session for UMTS.
           This relies on there being at least one defined profile on the modem.
           For Sierra devices, this should always be the case for a PRIed modem.
           For Gobi devices, a default profile is created during startup if there
           are no profiles currently defined. 

           SWI_TBD:
           SetDefaultProfile() cannot be used to create a profile if there are
           no currently defined profiles on the modem, because it will return
           an error in this case. Is this a bug in SetDefaultProfile() ?
         */
        if (IsGobiDevice()  &&  !Is9x15Device() && !Is9x30Device())
        {
            nRet = StartDataSession2 (
                        &tech_qmi,
                        NULL,
                        NULL,
                        NULL,
                        NULL,
                        (char *)APN,
                        NULL,
                        (ULONG *)authentypep,
                        username,
                        password,
                        &sessionIDv4,
                        &sessionIDv6,
                        &FailureReason,
                        IPFamilyPreference);
        }
        else
        {
            #ifdef SLQS_2X
            /* not necessary in SLQS3.X*/
            nRet = SLQSSetIPFamilyPreference (IPFamilyPreference);
            if (nRet != eQCWWAN_ERR_NONE) {
                LOGE("%s failed to set IPFamilyPreference %d with error=%lu", __func__, IPFamilyPreference, nRet);
                goto error;
            }
            nRet = StartDataSessionLTE (
                        &tech_qmi,
                        NULL,
                        NULL,
                        NULL,
                        NULL,
                        NULL,
                        NULL,
                        (char *)APN,
                        NULL,
                        NULL,
                        (ULONG *)authentypep,
                        username,
                        password,
                        &sessionIDv4,
                        &sessionIDv6,
                        &FailureReason, 
                        IPFamilyPreference);
            #else
            nRet = StartDataSession2 (
                        &tech_qmi,
                        NULL,
                        NULL,
                        NULL,
                        NULL,
                        (char *)APN,
                        NULL,
                        (ULONG *)authentypep,
                        username,
                        password,
                        &sessionIDv4,
                        &sessionIDv6,
                        &FailureReason,
                        IPFamilyPreference);
            #endif
        }
    }   /* end of UMTS or LTE */
    else
    {
        /* CDMA2000 */
        tech_qmi = QMI_TECH_CDMA2000;

        /* start data session with IPV4 for CDMA */
        nRet = StartDataSession2 (
                    &tech_qmi,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    &sessionIDv4,
                    &sessionIDv6,
                    &FailureReason,
                    IPFamilyPreference);
    }

    /* nRet is the result from either StartDataSession2() or StartDataSessionLTE() */
    if (nRet == eQCWWAN_ERR_NONE) {
        /* Flag the connection is active */
        pdp_active = 1;
        
        if (IPFamilyPreference == QMI_IPV4_ADDRESS) {
            /* Save the new session ID */
            s_sessionIDQMI = sessionIDv4;
            *runTimeSettings.v4sessionId = sessionIDv4;
            runTimeSettings.v6sessionId = 0;
        }
        
        if (IPFamilyPreference == QMI_IPV6_ADDRESS) {
            /* Save the new session ID */
            s_sessionIDQMI = sessionIDv6;           
            *runTimeSettings.v6sessionId = sessionIDv6;
            runTimeSettings.v4sessionId = 0;
        }    
        
        if (IPFamilyPreference == QMI_IPV4V6_ADDRESS) {
            /* Save the new session ID */
            s_sessionIDQMI = sessionIDv4;        
            *runTimeSettings.v4sessionId = sessionIDv4;
            *runTimeSettings.v6sessionId = sessionIDv6;
        }

        /* call the API, get the address related values */
        nRet = SLQSGetRuntimeSettings( &runTimeSettings );
        if (nRet == eQCWWAN_ERR_NONE) {
            LOGI("get runtime setting ok ");
            if (IPFamilyPreference == QMI_IPV4_ADDRESS) {
                LOGI("get ipv4 addr");
                getIPAddressStr(IPAddressV4, &ipAddrStrp);
                LOGI("get ipv4 dns addr");
                getIPAddresses2Str(PrimaryDNSV4, SecondaryDNSV4, &s_rilPDPContextListRes.dnses);
                LOGI("get ipv4 gw addr");
                getIPAddressStr(GWAddressV4, &s_rilPDPContextListRes.gateways);
                LOGI("%s V4 IP='%s' DNS='%s' gateway='%s'\n", 
                     __func__, ipAddrStrp, s_rilPDPContextListRes.dnses, s_rilPDPContextListRes.gateways);
            }
            else if (IPFamilyPreference == QMI_IPV6_ADDRESS) {
                getIPV6AddressStr(&IPV6AddrInfo.IPAddressV6[0], &ipAddrStrp);
                getIPV6Addresses2Str(&PrimDNSV6[0], &SecondDNSV6[0], &s_rilPDPContextListRes.dnses);
                getIPV6AddressStr(&IPV6GWAddrInfo.gwAddressV6[0], &s_rilPDPContextListRes.gateways);
                LOGI("%s V6 IP='%s' DNS='%s' gateway='%s'\n", 
                     __func__, ipAddrStrp, s_rilPDPContextListRes.dnses, s_rilPDPContextListRes.gateways);
            }
            else {
                getIPV4V6AddressesStr(IPAddressV4, &IPV6AddrInfo.IPAddressV6[0], &ipAddrStrp);
                getIPV4V6AddressesStr2(PrimaryDNSV4, SecondaryDNSV4, 
                                       &PrimDNSV6[0], &SecondDNSV6[0], 
                                       &s_rilPDPContextListRes.dnses);
                getIPV4V6AddressesStr(GWAddressV4, &IPV6GWAddrInfo.gwAddressV6[0], &s_rilPDPContextListRes.gateways);
                LOGI("%s V4V6 IP='%s' DNS='%s' gateway='%s'\n", 
                     __func__, ipAddrStrp, s_rilPDPContextListRes.dnses, s_rilPDPContextListRes.gateways);
            }
        }
        else {
            LOGE("%s SLQSGetRuntimeSettings failed %lu sessionID = %lu ", __func__, nRet,s_sessionIDQMI);
            goto error;
        }
    }
    /* Failure reason can only be translated if 
     * qmerror.h code is eQCWWAN_ERR_QMI_CALL_FAILED 
     */
    else if ( nRet == eQCWWAN_ERR_QMI_CALL_FAILED ) {
        /* An error occurred */
        LOGD("%s connection result: nRet: %d, Reason: %d\n",
             __func__, (int) nRet, (int) FailureReason);

        if ( retrievePDPRejectCauseQMI(FailureReason) < 0) {
            LOGE("Setup data call fail: %d, can't get last fail cause",(int) FailureReason);
        }
        else {
            LOGE("Setup data call fail: FailureReason %d, cause %d",
                 (int) FailureReason, getLastPdpFailCauseQMI());
        }
        goto error;
    }
    else if ( nRet == eQCWWAN_ERR_QMI_NO_EFFECT ) {
        /* check existing data session for handoff */
        if (isDataSessionActive())
        {
            LOGI("%s eQCWWAN_ERR_QMI_NO_EFFECT \n", __func__);
            /* Flag the connection is active */
            pdp_active = 1;
            
            /* compare IP Family Preference */
            if (IPFamilyPreference == getIPFamilyPreference(s_rilPDPContextListRes.type))
            {
                /* call the API, get the address related values */
                nRet = SLQSGetRuntimeSettings( &runTimeSettings );

                if (nRet == eQCWWAN_ERR_NONE)
                {
                    if (IPFamilyPreference == QMI_IPV4_ADDRESS) {
                        getIPAddressStr(IPAddressV4, &ipAddrStrp);
                        LOGI("%s eQCWWAN_ERR_QMI_NO_EFFECT IP V4 Address: %lx, '%s'\n", __func__, IPAddressV4, ipAddrStrp);
                    }
                    else if (IPFamilyPreference == QMI_IPV6_ADDRESS) {
                        getIPV6AddressStr(&IPV6AddrInfo.IPAddressV6[0], &ipAddrStrp);
                        LOGI("%s eQCWWAN_ERR_QMI_NO_EFFECT IP V6 Address: '%s'\n", __func__, ipAddrStrp);
                    }
                    else {
                        getIPV4V6AddressesStr(IPAddressV4, &IPV6AddrInfo.IPAddressV6[0], &ipAddrStrp);
                        LOGI("%s eQCWWAN_ERR_QMI_NO_EFFECT IP V4V6 Address: '%s'\n", __func__, ipAddrStrp);
                    }
                    
                    /* check IP address with cache */
                    if (0 == strcmp(ipAddrStrp, s_rilPDPContextListRes.addresses)) {
                        LOGI("%s eQCWWAN_ERR_QMI_NO_EFFECT assume data seesion persist, return ok!", __func__);
                        goto response;
                    }
                    else
                        LOGE("%s eQCWWAN_ERR_QMI_NO_EFFECT IP address not the same as cache", __func__);
                }
                else
                    LOGE("%s eQCWWAN_ERR_QMI_NO_EFFECT Failed to get IP address\n",  __func__);
            }
            else
                LOGE("%s eQCWWAN_ERR_QMI_NO_EFFECT IPFamilyPreference is not match", __func__);
        }
        else
            LOGE("%s eQCWWAN_ERR_QMI_NO_EFFECT no active data session",__func__);

        goto error;
    }
    else {
        LOGE("QMI SDK (qmerrno.h) reports error %d\n", (int) nRet );
        goto error;
    }

    /* Start dhcp client, IP address and DNS got from SLQSGetRuntimeSettings() already */
    clearDHCPproperty();
    if (property_set("ctl.start", "sierra_dhcpcd")) {
        LOGE("requestSetupDefaultPDP FAILED to set ctl.start sierra_dhcpcd property!");
        goto error;
    }
    
    if (IPFamilyPreference != QMI_IPV6_ADDRESS) {

        poll_params = malloc(sizeof(struct dhcpPollParams));
        poll_params->loopcount = 0;
        poll_params->t = t;
        poll_params->pdp_active = pdp_active;

        /* PDP type */
        if (pdptype != NULL) {
            if (s_rilPDPContextListRes.type != NULL) {
                free(s_rilPDPContextListRes.type);
                s_rilPDPContextListRes.type = NULL;
            }
            s_rilPDPContextListRes.type = strdup(pdptype);
        }
        /* IP Addresses */
        if (s_rilPDPContextListRes.addresses != NULL) {
            free(s_rilPDPContextListRes.addresses);
            s_rilPDPContextListRes.addresses = NULL;
        }
        if (ipAddrStrp != NULL) {
            s_rilPDPContextListRes.addresses = strdup(ipAddrStrp);
            free(ipAddrStrp);
        }
        enqueueRILEvent(RIL_EVENT_QUEUE_PRIO, polldhcptask,
                    poll_params, NULL);
        return;
    } 
    else {
        /* IPV6 - IP address and DNS handled by kernel. 
           DHCP client only help to up the interface */
        property_set("ctl.stop", "sierra_dhcpcd");
    }

response:
    /* update cache of RIL_Data_Call_Response_v6 */
    /* status */    
    s_rilPDPContextListRes.status = PDP_FAIL_NONE;
    /* CID */
    s_rilPDPContextListRes.cid = SWI_DEFAULT_CID;
    /* Data call state */
    s_rilPDPContextListRes.active = RIL_STATE_ACTIVE_LINKUP;
    /* PDP type */
    if (pdptype != NULL) {
        if (s_rilPDPContextListRes.type != NULL) {
            free(s_rilPDPContextListRes.type);
            s_rilPDPContextListRes.type = NULL;
        }
        s_rilPDPContextListRes.type = strdup(pdptype);
    }
    /* IP Addresses */
    if (s_rilPDPContextListRes.addresses != NULL) {
        free(s_rilPDPContextListRes.addresses);
        s_rilPDPContextListRes.addresses = NULL;
    }
    if (ipAddrStrp != NULL)
        s_rilPDPContextListRes.addresses = strdup(ipAddrStrp);
    
    /* Fast Dormancy supported only for SPRINT carrier devices at this time */
    if (getFirmwareInfoCarrierQMI(&GobiImageCarrier) && (GobiImageCarrier == eGOBI_IMG_CAR_SPRINT))
    {
        nRet = SLQSWdsGoDormant();
        if (eQCWWAN_ERR_NONE != nRet) {
            LOGE("%s::SLQSWdsGoDormant failed, nRet: %lu\n",__func__, nRet );
            goto error;
        }
        LOGD("%s::SLQSWdsGoDormant Success\n",__func__ );
    }   
 
finally:

    if (ipAddrStrp != NULL)
        free(ipAddrStrp);
    /* Always return successful response */
    RIL_onRequestComplete(t, RIL_E_SUCCESS, &s_rilPDPContextListRes, sizeof(RIL_Data_Call_Response_v6));
    LOGD("requestSetupDefaultPDP done");
    return;

error:
    LOGD("requestSetupDefaultPDP error handling");
    /* This is work around to get eHRPD from data bearer */
    if (!getPropertyRadioNotifiction() && isDualModeRunningCDMA()) {
        registerDormancyStatusCallbackQMI(false);
    }
    if ((pdp_active == 1) && (s_sessionIDQMI != 0)) {

        /* CUS90903: Workaround for data connnection lost during idleness: 
         * Always send ERROR_UNSPECIFIED, because PROTOCOL_ERRORS is treated 
         * as permanenet failure at framework
         * PDP_FAIL_ERROR_UNSPECIFIED will kick off retrying of data call 
         */
        setLastPdpFailCauseQMI(PDP_FAIL_ERROR_UNSPECIFIED);
        isDataSessionCloseByClient(true);
        /* deactivate PDP context */
        nRet = StopDataSession ( s_sessionIDQMI );

        if ((nRet == eQCWWAN_ERR_NONE) || (nRet == eQCWWAN_ERR_QMI_NO_EFFECT)) {
            LOGI("requestSetupDefaultPDPQMI error handle: Stopped the session %lu\n", s_sessionIDQMI);
            s_sessionIDQMI = 0;
            /* stop DHCP client */
            checkDHCPnStopService();
            if (nRet == eQCWWAN_ERR_QMI_NO_EFFECT) {
                isDataSessionCloseByClient(false);
            }
        }
        else
        {
            LOGE("%s:: error handle: Stopped the session %lu failed with %lu \n", __func__, s_sessionIDQMI, nRet);
            isDataSessionCloseByClient(false);
        }
    }
    s_rilPDPContextListRes.status = getLastPdpFailCauseQMI();

    /* de-activate network ifc, ril_iface */
    activateNetworkInterface(false);
    goto finally;
}

/**
 *
 * RIL_REQUEST_DEACTIVATE_DATA_CALL
 * Deactivate PDP context created by RIL_REQUEST_SETUP_DATA_CALL.
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
void requestDeactivateDefaultPDPQMI(void *data, size_t datalen, RIL_Token t)
{
    const char *pPdpCid = NULL;
    int disconnectReason = RIL_DISCONNECT_REASON_NO_SPECIFIC;
    ULONG nRet;

    if (t != NULL) {   
        pPdpCid = ((const char **) data)[0];
        if ((((datalen/sizeof(char *)) >= RIL_REQUEST_DEACTIVATE_DATA_CALL_REQUEST_PARAMS)) && 
            (((const char **) data)[1] != NULL)) {
            disconnectReason = atoi(((const char **) data)[1]);
        }
        LOGD("requestDeactivateDefaultPDPQMI PdpCid '%s' DisconnectReason '%d'", pPdpCid, disconnectReason);

        /* Regardless of the disconnect reason, stop the data session first, and 
         * then do any further actions below.  This way, we always know that the
         * data session was properly stopped.
         */    
        if ((pPdpCid != NULL) && (s_sessionIDQMI != 0)) {
            if (atoi(pPdpCid) == SWI_DEFAULT_CID) {
                isDataSessionCloseByClient(true);
                nRet = StopDataSession ( s_sessionIDQMI );
                if ((nRet != eQCWWAN_ERR_NONE) && (nRet != eQCWWAN_ERR_QMI_NO_EFFECT)) {
                    LOGE("%s StopDataSession failed %lu\n", __func__, nRet);
                    isDataSessionCloseByClient(false);
                    goto error;
                } else if (nRet == eQCWWAN_ERR_QMI_NO_EFFECT) {
                    isDataSessionCloseByClient(false);
                }
                LOGI("%s successfully stopped the session %lu\n", __func__, s_sessionIDQMI);
                s_sessionIDQMI = 0;
                clearDHCPproperty();
            }
            else {
                LOGE("requestDeactivateDefaultPDP CID is invalid %s\n", pPdpCid);
                goto error;
            }
        }
        else {
            LOGE("requestDeactivateDefaultPDP error: CID: %s, s_sessionID: %d",
                pPdpCid, (int) s_sessionIDQMI);
            goto error;
		}
    }
    else {
        if (s_sessionIDQMI != 0) {
            isDataSessionCloseByClient(true);
            nRet = StopDataSession ( s_sessionIDQMI );
            if ((nRet != eQCWWAN_ERR_NONE) && (nRet != eQCWWAN_ERR_QMI_NO_EFFECT)) {
                LOGE("%s StopDataSession failed %lu\n", __func__, nRet);
                isDataSessionCloseByClient(false);
                goto error;
            }
            else if (nRet == eQCWWAN_ERR_QMI_NO_EFFECT) {
                isDataSessionCloseByClient(false);
            }
            LOGI("%s successfully stopped the session %lu\n", __func__, s_sessionIDQMI);
            s_sessionIDQMI = 0;
            clearDHCPproperty();
        }
        else {
            LOGE("requestDeactivateDefaultPDP error: s_sessionID: %d",(int) s_sessionIDQMI);
            goto error;
		}
    }
    /* START MOTO 172 */
    /* When turning off the RADIO (e.g., entering airplane mode) Android will first attempt to gracefully
     * close the data call (with a timeout of 30 seconds if data call disconnect is not received) and then
     * send a request to enter low power mode. The RIL should not switch to low power mode here as it'll
     * result in two seperate requests to enter LPM mode which can interfear with the user attempting to 
     * quickly exit from airplane mode.
     */
#if 0
    /* SWI_TBD only observed RIL_DISCONNECT_REASON_RADIO_SHUTDOWN when device is switching off
     * set to airplane mode for now */
    if (disconnectReason == RIL_DISCONNECT_REASON_RADIO_SHUTDOWN) {
        changePowerModeQMI(QMI_POWER_LOW);
        s_sessionIDQMI = 0;
        setRadioState(RADIO_STATE_OFF);
    }
#endif /* 0 */
    /* STOP MOTO 172 */

    /* clean up data call list when data call deactivated */
    initDataCallResponseList();
    s_rilPDPContextListRes.status = PDP_FAIL_NONE;
    /* stop DHCP client */
    checkDHCPnStopService();

    /* de-activate network ifc, ril_iface */
    activateNetworkInterface(false);
    if (t != NULL)
        RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    else {
        if (s_rilPDPContextListRes.status != PDP_FAIL_RADIO_POWER_OFF) {
            RIL_onUnsolicitedResponse(RIL_UNSOL_DATA_CALL_LIST_CHANGED,
                                      &s_rilPDPContextListRes,
                                      sizeof(RIL_Data_Call_Response_v6));
        }
    }
    return;
    
error:
    if (t != NULL)
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

RIL_Data_Call_Response_v6* getActivatedPDPContext( void )
{
    return &s_rilPDPContextListRes;
}

void dormancyStatusCallback(ULONG dormancyStatus)
{
    RIL_Data_Call_Response_v6* pActivePDPContext;

    LOGD("dormancyStatusCallback: dormancyStatus=%lx\n", dormancyStatus );
    if ( dormancyStatus == QMI_TRAFFIC_CHANNEL_ACTIVE) {
        /* can't run in callback, queue up */
        enqueueRILEvent(RIL_EVENT_QUEUE_NORMAL, checkEHRPD,
                        NULL, NULL);
    }

    pActivePDPContext = getActivatedPDPContext();
    if (dormancyStatus == QMI_TRAFFIC_CHANNEL_ACTIVE) {
        pActivePDPContext->active = RIL_STATE_ACTIVE_LINKUP;
    }
    else if (dormancyStatus == QMI_TRAFFIC_CHANNEL_DORMANT) {
        pActivePDPContext->active = RIL_STATE_ACTIVE_LINKDOWN;
    }
    RIL_onUnsolicitedResponse( RIL_UNSOL_DATA_CALL_LIST_CHANGED,
                               pActivePDPContext,
                               sizeof(RIL_Data_Call_Response_v6) );
}

#else

/**
 *
 * Initialize RIL_Data_Call_Response(_v11) structure
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
void initDataCallResponseList_r12(void)
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
    /* pcscf */
    if (s_rilPDPContextListRes.pcscf != NULL) {
        free(s_rilPDPContextListRes.pcscf);
        s_rilPDPContextListRes.pcscf = NULL;
    }    
    s_rilPDPContextListRes.mtu = 0;
}


/**
 *
 * Sends PDP context information to the RIL based on a solicited
 * request or an unsolicited event for SWI_RIL_VERSION_12
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
void requestOrSendPDPContextListQMI_r12(RIL_Token *t)
{
    /* Network interface name, never change right now */
    s_rilPDPContextListRes.ifname = ril_iface;
    
    /* START MOTO 172,174*/
    if (isDataSessionActive()) {
        s_rilPDPContextListRes.active = RIL_STATE_ACTIVE_LINKUP;
    }
    else {
        /* Clear data call Response List, this will set 
         * s_rilPDPContextListRes.active = RIL_STATE_INACTIVE
         */
            initDataCallResponseList_r12();
            s_rilPDPContextListRes.status = PDP_FAIL_NONE;
        }
    /* Android CDMA onDataStateChanged() will process either NULL or data call list.
     * GSM onDataStateChanged() will only process data call list. For compatibility
     * with both, always return s_rilPDPContextListRes even when session
     * is inactive.
     */
    if (t != NULL) {
            RIL_onRequestComplete(*t, RIL_E_SUCCESS, &s_rilPDPContextListRes,
                                  sizeof(RIL_Data_Call_Response_v11));
        }
    else {
        /* RIL_UNSOL_DATA_CALL_LIST_CHANGED is not expected to be
         * issued because of an RIL_REQUEST_DEACTIVATE_DATA_CALL.
         * And make sure no RIL_UNSOL_DATA_CALL_LIST_CHANGED for power off.
         */
        if (s_rilPDPContextListRes.status != PDP_FAIL_RADIO_POWER_OFF) {
                RIL_onUnsolicitedResponse(RIL_UNSOL_DATA_CALL_LIST_CHANGED,
                                      &s_rilPDPContextListRes,
                                      sizeof(RIL_Data_Call_Response_v11));
            }
        }
    /* STOP MOTO 172,174 */
    return;            
}



static void polldhcptask_r12(void *params)
{
    struct dhcpPollParams *poll_params;
    RIL_Token t;
    ULONG nRet;
    int pdp_active;    
    enum   eGobiImageCarrier GobiImageCarrier;
    
    LOGD("%s::Entered ", __func__);

    poll_params = (struct dhcpPollParams *) params;
    t = poll_params->t;
    pdp_active = poll_params->pdp_active;

    if (poll_params->loopcount++ >= REPOLL_DHCP)
        goto error;

    if (PollDHCPnsetDNS() < 0) {
        LOGE("%s::PollDHCPnsetDNS retry  %d out of %d ", __func__, poll_params->loopcount,REPOLL_DHCP);
        enqueueRILEvent(RIL_EVENT_QUEUE_NORMAL, polldhcptask_r12,
                        poll_params, &TIMEVAL_DHCP_POLL);
        return;
    }

    s_rilPDPContextListRes.status = PDP_FAIL_NONE;
    /* CID */
    s_rilPDPContextListRes.cid = SWI_DEFAULT_CID;
    /* Data call state */
    s_rilPDPContextListRes.active = RIL_STATE_ACTIVE_LINKUP;
   
    /* Fast Dormancy supported only for SPRINT carrier devices at this time */
    if (getFirmwareInfoCarrierQMI(&GobiImageCarrier) && (GobiImageCarrier == eGOBI_IMG_CAR_SPRINT))
    {
        nRet = SLQSWdsGoDormant();
        if (eQCWWAN_ERR_NONE != nRet) {
            LOGE("%s::SLQSWdsGoDormant failed, nRet: %lu\n",__func__, nRet );
            /* not support in MC73xx, not error */
            goto finally;
        }
        LOGD("%s::SLQSWdsGoDormant Success\n",__func__ );
    }   
 
finally:
   
    /* Always return successful response */
    RIL_onRequestComplete(t, RIL_E_SUCCESS, &s_rilPDPContextListRes, sizeof(RIL_Data_Call_Response_v11));
    free (params);
    LOGD("requestSetupDefaultPDP_r12 done");
    return;

error:
    LOGD("requestSetupDefaultPDP_r12 error handling");
    /* This is work around to get eHRPD from data bearer */
    if (!getPropertyRadioNotifiction() && isDualModeRunningCDMA()) {
        registerDormancyStatusCallbackQMI(false);
    }
    if ((pdp_active == 1) && (s_sessionIDQMI != 0)) {
        /* CUS90903: Workaround for data connnection lost during idleness:
         * Always send ERROR_UNSPECIFIED, because PROTOCOL_ERRORS is treated
         * as permanenet failure at framework
         * PDP_FAIL_ERROR_UNSPECIFIED will kick off retrying of data call
         */
        setLastPdpFailCauseQMI(PDP_FAIL_ERROR_UNSPECIFIED);

        isDataSessionCloseByClient(true);
        /* deactivate PDP context */
        nRet = StopDataSession ( s_sessionIDQMI );

        if ((nRet == eQCWWAN_ERR_NONE) || (nRet == eQCWWAN_ERR_QMI_NO_EFFECT)) {
            LOGI("requestSetupDefaultPDPQMI_r12 error handle: Stopped the session %lu\n", s_sessionIDQMI);
            s_sessionIDQMI = 0;
            /* stop DHCP client */
            checkDHCPnStopService();
            if (nRet == eQCWWAN_ERR_QMI_NO_EFFECT) {
                isDataSessionCloseByClient(false);
            }
        }
        else
        {
            LOGE("%s:: error handle: Stopped the session %lu failed with %lu \n", __func__, s_sessionIDQMI, nRet);
            isDataSessionCloseByClient(false);
        }
    }
    s_rilPDPContextListRes.status = getLastPdpFailCauseQMI();

    /* de-activate network ifc, ril_iface */
    activateNetworkInterface(false);
    goto finally;
}


/**
 *
 * RIL_REQUEST_SETUP_DATA_CALL for SWI_RIL_VERSION_12
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
void requestSetupDefaultPDPQMI_r12(void *data, size_t datalen, RIL_Token t)
{
    const char *APN = NULL;
    char *property = NULL;
    int tech_ril;
    ULONG tech_qmi;
    int pidtype;
    char *username = NULL;
    char *password = NULL;
    const char *authtype;
    int authentype;
    int *authentypep;
    int pid;
    int pdp_active = 0;
    
    ULONG nRet;
    ULONG IPAddress = 0;
    ULONG sessionIDv4 = 0;
    ULONG sessionIDv6 = 0;
    ULONG FailureReason = 0;
    ULONG PDPType = GOBI_DEFAULT_PDP_TYPE;
    char *pdptype = NULL;
    int i;
    char *ipAddrStrp = NULL;
    USHORT IPAddressV6[8];
    BYTE IPv6prefixlen = 0;
    BYTE IPFamilyPreference = QMI_IPV4_ADDRESS;

    ULONG  v4sessionId;
    ULONG  v6sessionId;
    CHAR ProfileName[NUM_MAX_STR_LEN];
    ULONG PdpType;
    CHAR APNName[NUM_MAX_STR_LEN];
    ULONG PrimaryDNSV4;
    ULONG SecondaryDNSV4;
    struct UMTSQoS UMTSGrantedQos;
    struct GPRSQoS GPRSGrantedQos;
    CHAR Username[NUM_MAX_STR_LEN];
    ULONG Authentication;
    ULONG IPAddressV4;
    struct ProfileIdentifier ProfileID;
    ULONG GWAddressV4;
    ULONG SubnetMaskV4;
    BYTE PCSCFAddrPCO;
    struct PCSCFIPv4ServerAddressList ServerAddrList;
    struct PCSCFFQDNAddressList PCSCFFQDNAddrList;
    USHORT PrimDNSV6[8];
    USHORT SecondDNSV6[8];
    ULONG Mtu = 0;
    struct DomainNameList DomainList;
    BYTE IPFamilyPref;
    BYTE IMCNflag;
    WORD Technology;
    struct IPV6AddressInfo IPV6AddrInfo;
    struct IPV6GWAddressInfo IPV6GWAddrInfo;
    struct WdsRunTimeSettings runTimeSettings;
    enum   eGobiImageCarrier GobiImageCarrier;
    struct dhcpPollParams *poll_params=NULL;

    memset(&ServerAddrList, 0, sizeof(struct PCSCFIPv4ServerAddressList));
    runTimeSettings.v4sessionId = &v4sessionId;
    runTimeSettings.v6sessionId = &v6sessionId;
    runTimeSettings.rts.pProfileName = &ProfileName[0];
    runTimeSettings.rts.pPDPType = &PdpType;
    runTimeSettings.rts.pAPNName = &APNName[0];
    runTimeSettings.rts.pPrimaryDNSV4 =  &PrimaryDNSV4;
    runTimeSettings.rts.pSecondaryDNSV4 = &SecondaryDNSV4;
    runTimeSettings.rts.pUMTSGrantedQoS = &UMTSGrantedQos;
    runTimeSettings.rts.pGPRSGrantedQoS = &GPRSGrantedQos;
    runTimeSettings.rts.pUsername = &Username[0];
    runTimeSettings.rts.pAuthentication = &Authentication;
    runTimeSettings.rts.pIPAddressV4 = &IPAddressV4;
    runTimeSettings.rts.pProfileID = &ProfileID;
    runTimeSettings.rts.pGWAddressV4 = &GWAddressV4;
    runTimeSettings.rts.pSubnetMaskV4 = &SubnetMaskV4;
    runTimeSettings.rts.pPCSCFAddrPCO = &PCSCFAddrPCO;
    runTimeSettings.rts.pServerAddrList = &ServerAddrList;
    runTimeSettings.rts.pPCSCFFQDNAddrList = &PCSCFFQDNAddrList;
    runTimeSettings.rts.pPrimaryDNSV6 = &PrimDNSV6[0];
    runTimeSettings.rts.pSecondaryDNSV6 = &SecondDNSV6[0];
    runTimeSettings.rts.pMtu = &Mtu;
    runTimeSettings.rts.pDomainList = &DomainList;
    runTimeSettings.rts.pIPFamilyPreference = &IPFamilyPref;
    runTimeSettings.rts.pIMCNflag = &IMCNflag;
    runTimeSettings.rts.pTechnology = &Technology;
    runTimeSettings.rts.pIPV6AddrInfo = &IPV6AddrInfo;
    runTimeSettings.rts.pIPV6GWAddrInfo = &IPV6GWAddrInfo;

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

    /* Network interface name, never change right now */
    s_rilPDPContextListRes.ifname = ril_iface;

    /* activate network ifc, ril_iface */
    activateNetworkInterface(true);
    
    /* Check radio state first */
    if ((currentState() == RADIO_STATE_OFF) || (currentState() == RADIO_STATE_UNAVAILABLE)) {
        LOGE("requestSetupDefaultPDPQMI radio is not avaliable\n");
        setLastPdpFailCauseQMI(PDP_FAIL_RADIO_POWER_OFF);
        goto error;
    }
    
    IPFamilyPreference = getIPFamilyPreference(pdptype);
    if (IPFamilyPreference == QMI_IP_ADDRESS_UNSPEC) {
        LOGE("%s IPFamilyPreference unspecified\n", __func__);
        /* update status field for failure cause */
        s_rilPDPContextListRes.status = PDP_FAIL_UNKNOWN_PDP_ADDRESS_TYPE;       
        goto error;
    }

    /* Suggested retry times, default to -1 which means no value is suggested */
    LOGD("%s:: Radio Technology to use: %d\n", __func__, tech_ril);
    s_rilPDPContextListRes.suggestedRetryTime = RIL_ACTIVATE_DATA_CALL_RETRY_NO_SUGGESTION;

    /* SWI_TBD temprory disable this check for handoff */
#if 0
    /* Only support 1 data connection for now */
    if ((s_rilPDPContextListRes.active == RIL_STATE_ACTIVE_LINKUP) && (s_sessionIDQMI != 0)) {
        LOGE("requestSetupDefaultPDPQMI only 1 data connection supported by the modem\n");
        /* update status field for failure cause */
        s_rilPDPContextListRes.status = PDP_FAIL_ONLY_SINGLE_BEARER_ALLOWED;
        goto finally;
    }
#endif
    /* Gobi device only support IPV4 now */
    if (IsGobiDevice()  &&  !Is9x15Device() &&  !Is9x30Device()) {
        if (IPFamilyPreference != QMI_IPV4_ADDRESS) {
            LOGE("%s Gobi device only supports IPV4\n", __func__);
            /* update status field for failure cause */
            s_rilPDPContextListRes.status = PDP_FAIL_ONLY_IPV4_ALLOWED;           
            goto error;
        }
    }

    /* Clean up some of the cache */
    /* active */
    s_rilPDPContextListRes.active = RIL_STATE_INACTIVE;
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

    if (s_rilPDPContextListRes.pcscf != NULL) {
        free(s_rilPDPContextListRes.pcscf);
        s_rilPDPContextListRes.pcscf = NULL;
    }
    s_rilPDPContextListRes.mtu = 0;

    /* Stop the DHCP client if already running */
    if (!isDataSessionActive()) {
        checkDHCPnStopService();
    }

    LOGD("requesting data connection to APN '%s' IP preference '%s'", APN, pdptype);

    /* This is work around to get eHRPD from data bearer */
    if (!getPropertyRadioNotifiction() && isDualModeRunningCDMA()) {
        registerDormancyStatusCallbackQMI(true);
    }
    
    /* Start the data session for either UMTS or CDMA */
    if ((tech_ril == RIL_TECH_UMTS) ||
        (tech_ril -2 == RIL_RAT_GPRS) ||
        (tech_ril -2 == RIL_RAT_EDGE) ||
        (tech_ril -2 == RIL_RAT_UMTS) ||
        (tech_ril -2 == RIL_RAT_HSDPA) ||
        (tech_ril -2 == RIL_RAT_HSUPA) ||
        (tech_ril -2 == RIL_RAT_HSPA) ||
        (tech_ril -2 == RIL_RAT_LTE) ||
        (tech_ril -2 == RIL_RAT_HSPAP))
    {
        tech_qmi = QMI_TECH_UMTS;

        /* Determine PDP authentication type, if it was specified */
        authentypep = NULL;
        if (authtype != NULL) {
            authentype = atoi(authtype);
            authentypep = &authentype;

            /* check authentication type, RIL supports 0, 1, 2, 3 */
            if ((authentype < 0) || (authentype > 3)) {
                LOGE("requestSetupDefaultPDPQMI authentication type %d is not supported by modem", authentype);
                goto error;
            }
        }
        /* Start data session for UMTS.
           This relies on there being at least one defined profile on the modem.
           For Sierra devices, this should always be the case for a PRIed modem.
           For Gobi devices, a default profile is created during startup if there
           are no profiles currently defined. 

           SWI_TBD:
           SetDefaultProfile() cannot be used to create a profile if there are
           no currently defined profiles on the modem, because it will return
           an error in this case. Is this a bug in SetDefaultProfile() ?
         */
        if (IsGobiDevice()  &&  !Is9x15Device() && !Is9x30Device())
        {
            nRet = StartDataSession2 (
                        &tech_qmi,
                        NULL,
                        NULL,
                        NULL,
                        NULL,
                        (char *)APN,
                        NULL,
                        (ULONG *)authentypep,
                        username,
                        password,
                        &sessionIDv4,
                        &sessionIDv6,
                        &FailureReason,
                        IPFamilyPreference);
        }
        else
        {
            #ifdef SLQS_2X
            /* not necessary in SLQS3.X*/
            nRet = SLQSSetIPFamilyPreference (IPFamilyPreference);
            if (nRet != eQCWWAN_ERR_NONE) {
                LOGE("%s failed to set IPFamilyPreference %d with error=%lu", __func__, IPFamilyPreference, nRet);
                goto error;
            }
            nRet = StartDataSessionLTE (
                        &tech_qmi,
                        NULL,
                        NULL,
                        NULL,
                        NULL,
                        NULL,
                        NULL,
                        (char *)APN,
                        NULL,
                        NULL,
                        (ULONG *)authentypep,
                        username,
                        password,
                        &sessionIDv4,
                        &sessionIDv6,
                        &FailureReason, 
                        IPFamilyPreference);
            #else
            nRet = StartDataSession2 (
                        &tech_qmi,
                        NULL,
                        NULL,
                        NULL,
                        NULL,
                        (char *)APN,
                        NULL,
                        (ULONG *)authentypep,
                        username,
                        password,
                        &sessionIDv4,
                        &sessionIDv6,
                        &FailureReason,
                        IPFamilyPreference);
            #endif
        }
    }   /* end of UMTS or LTE */
    else
    {
        /* CDMA2000 */
        tech_qmi = QMI_TECH_CDMA2000;

        /* start data session with IPV4 for CDMA */
        nRet = StartDataSession2 (
                    &tech_qmi,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    &sessionIDv4,
                    &sessionIDv6,
                    &FailureReason,
                    IPFamilyPreference);
    }

    /* nRet is the result from either StartDataSession2() or StartDataSessionLTE() */
    if (nRet == eQCWWAN_ERR_NONE) {
        /* Flag the connection is active */
        pdp_active = 1;
        
        if (IPFamilyPreference == QMI_IPV4_ADDRESS) {
            /* Save the new session ID */
            s_sessionIDQMI = sessionIDv4;
            *runTimeSettings.v4sessionId = sessionIDv4;
            runTimeSettings.v6sessionId = 0;
        }
        
        if (IPFamilyPreference == QMI_IPV6_ADDRESS) {
            /* Save the new session ID */
            s_sessionIDQMI = sessionIDv6;           
            *runTimeSettings.v6sessionId = sessionIDv6;
            runTimeSettings.v4sessionId = 0;
        }    
        
        if (IPFamilyPreference == QMI_IPV4V6_ADDRESS) {
            /* Save the new session ID */
            s_sessionIDQMI = sessionIDv4;        
            *runTimeSettings.v4sessionId = sessionIDv4;
            *runTimeSettings.v6sessionId = sessionIDv6;
        }

        /* call the API, get the address related values */
        nRet = SLQSGetRuntimeSettings( &runTimeSettings );
        if (nRet == eQCWWAN_ERR_NONE) {
            LOGI("get runtime setting ok ");
            s_rilPDPContextListRes.mtu = (int)Mtu;
            LOGI("MTU is %d", (int)Mtu);
            if (IPFamilyPreference == QMI_IPV4_ADDRESS) {
                LOGI("get ipv4 addr");
                getIPAddressStr(IPAddressV4, &ipAddrStrp);
                LOGI("get ipv4 dns addr");
                getIPAddresses2Str(PrimaryDNSV4, SecondaryDNSV4, &s_rilPDPContextListRes.dnses);
                LOGI("get ipv4 gw addr");
                getIPAddressStr(GWAddressV4, &s_rilPDPContextListRes.gateways);
                LOGI("get pcscf ipv4 addr");
                getPCSCFAddresses2Str(ServerAddrList, &s_rilPDPContextListRes.pcscf);
                if (s_rilPDPContextListRes.pcscf)
                    LOGI("%s PCSCF V4 IP='%s'\n",__func__, s_rilPDPContextListRes.pcscf);
                LOGI("%s V4 IP='%s' DNS='%s'\n",__func__,ipAddrStrp, s_rilPDPContextListRes.dnses);
                LOGI("%s gateway='%s'\n",__func__, s_rilPDPContextListRes.gateways);
            }
            else if (IPFamilyPreference == QMI_IPV6_ADDRESS) {
                getIPV6AddressStr(&IPV6AddrInfo.IPAddressV6[0], &ipAddrStrp);
                getIPV6Addresses2Str(&PrimDNSV6[0], &SecondDNSV6[0], &s_rilPDPContextListRes.dnses);
                getIPV6AddressStr(&IPV6GWAddrInfo.gwAddressV6[0], &s_rilPDPContextListRes.gateways);
                LOGI("%s V6 IP='%s' DNS='%s' gateway='%s'\n", 
                     __func__, ipAddrStrp, s_rilPDPContextListRes.dnses, s_rilPDPContextListRes.gateways);
            }
            else {
                getIPV4V6AddressesStr(IPAddressV4, &IPV6AddrInfo.IPAddressV6[0], &ipAddrStrp);
                getIPV4V6AddressesStr2(PrimaryDNSV4, SecondaryDNSV4, 
                                       &PrimDNSV6[0], &SecondDNSV6[0], 
                                       &s_rilPDPContextListRes.dnses);
                getIPV4V6AddressesStr(GWAddressV4, &IPV6GWAddrInfo.gwAddressV6[0], &s_rilPDPContextListRes.gateways);
                LOGI("%s V4V6 IP='%s' DNS='%s' gateway='%s'\n", 
                     __func__, ipAddrStrp, s_rilPDPContextListRes.dnses, s_rilPDPContextListRes.gateways);
            }
        }
        else {
            LOGE("%s SLQSGetRuntimeSettings failed %lu sessionID = %lu ", __func__, nRet,s_sessionIDQMI);
            goto error;
        }
    }
    /* Failure reason can only be translated if 
     * qmerror.h code is eQCWWAN_ERR_QMI_CALL_FAILED 
     */
    else if ( nRet == eQCWWAN_ERR_QMI_CALL_FAILED ) {
        /* An error occurred */
        LOGD("%s connection result: nRet: %d, Reason: %d\n",
             __func__, (int) nRet, (int) FailureReason);

        if ( retrievePDPRejectCauseQMI(FailureReason) < 0) {
            LOGE("Setup data call fail: %d, can't get last fail cause",(int) FailureReason);
        }
        else {
            LOGE("Setup data call fail: FailureReason %d, cause %d",
                 (int) FailureReason, getLastPdpFailCauseQMI());
        }
        goto error;
    }
    else if ( nRet == eQCWWAN_ERR_QMI_NO_EFFECT ) {
        /* check existing data session for handoff */
        if (isDataSessionActive())
        {
            LOGI("%s eQCWWAN_ERR_QMI_NO_EFFECT \n", __func__);
            /* Flag the connection is active */
            pdp_active = 1;
            
            /* compare IP Family Preference */
            if (IPFamilyPreference == getIPFamilyPreference(s_rilPDPContextListRes.type))
            {
                /* call the API, get the address related values */
                nRet = SLQSGetRuntimeSettings( &runTimeSettings );

                if (nRet == eQCWWAN_ERR_NONE)
                {
                    if (IPFamilyPreference == QMI_IPV4_ADDRESS) {
                        getIPAddressStr(IPAddressV4, &ipAddrStrp);
                        LOGI("%s eQCWWAN_ERR_QMI_NO_EFFECT IP V4 Address: %lx, '%s'\n", __func__, IPAddressV4, ipAddrStrp);
                    }
                    else if (IPFamilyPreference == QMI_IPV6_ADDRESS) {
                        getIPV6AddressStr(&IPV6AddrInfo.IPAddressV6[0], &ipAddrStrp);
                        LOGI("%s eQCWWAN_ERR_QMI_NO_EFFECT IP V6 Address: '%s'\n", __func__, ipAddrStrp);
                    }
                    else {
                        getIPV4V6AddressesStr(IPAddressV4, &IPV6AddrInfo.IPAddressV6[0], &ipAddrStrp);
                        LOGI("%s eQCWWAN_ERR_QMI_NO_EFFECT IP V4V6 Address: '%s'\n", __func__, ipAddrStrp);
                    }
                    
                    /* check IP address with cache */
                    if (0 == strcmp(ipAddrStrp, s_rilPDPContextListRes.addresses)) {
                        LOGI("%s eQCWWAN_ERR_QMI_NO_EFFECT assume data seesion persist, return ok!", __func__);
                        goto response;
                    }
                    else
                        LOGE("%s eQCWWAN_ERR_QMI_NO_EFFECT IP address not the same as cache", __func__);
                }
                else
                    LOGE("%s eQCWWAN_ERR_QMI_NO_EFFECT Failed to get IP address\n",  __func__);
            }
            else
                LOGE("%s eQCWWAN_ERR_QMI_NO_EFFECT IPFamilyPreference is not match", __func__);
        }
        else
            LOGE("%s eQCWWAN_ERR_QMI_NO_EFFECT no active data session",__func__);

        goto error;
    }
    else {
        LOGE("QMI SDK (qmerrno.h) reports error %d\n", (int) nRet );
        goto error;
    }

    /* Start dhcp client, IP address and DNS got from SLQSGetRuntimeSettings() already */
    clearDHCPproperty();
    if (property_set("ctl.start", "sierra_dhcpcd")) {
        LOGE("requestSetupDefaultPDP FAILED to set ctl.start sierra_dhcpcd property!");
        goto error;
    }
    
    if (IPFamilyPreference != QMI_IPV6_ADDRESS) {

        poll_params = malloc(sizeof(struct dhcpPollParams));
        poll_params->loopcount = 0;
        poll_params->t = t;
        poll_params->pdp_active = pdp_active;

        /* PDP type */
        if (pdptype != NULL) {
            if (s_rilPDPContextListRes.type != NULL) {
                free(s_rilPDPContextListRes.type);
                s_rilPDPContextListRes.type = NULL;
            }
            s_rilPDPContextListRes.type = strdup(pdptype);
        }
        /* IP Addresses */
        if (s_rilPDPContextListRes.addresses != NULL) {
            free(s_rilPDPContextListRes.addresses);
            s_rilPDPContextListRes.addresses = NULL;
        }
        if (ipAddrStrp != NULL) {
            s_rilPDPContextListRes.addresses = strdup(ipAddrStrp);
            free(ipAddrStrp);
        }
        enqueueRILEvent(RIL_EVENT_QUEUE_PRIO, polldhcptask_r12,
                    poll_params, NULL);
        return;
    } 
    else {
        /* IPV6 - IP address and DNS handled by kernel. 
           DHCP client only help to up the interface */
        property_set("ctl.stop", "sierra_dhcpcd");
    }

response:
    /* update cache of RIL_Data_Call_Response_v11 */
    /* status */    
    s_rilPDPContextListRes.status = PDP_FAIL_NONE;
    /* CID */
    s_rilPDPContextListRes.cid = SWI_DEFAULT_CID;
    /* Data call state */
    s_rilPDPContextListRes.active = RIL_STATE_ACTIVE_LINKUP;
    /* PDP type */
    if (pdptype != NULL) {
        if (s_rilPDPContextListRes.type != NULL) {
            free(s_rilPDPContextListRes.type);
            s_rilPDPContextListRes.type = NULL;
        }
        s_rilPDPContextListRes.type = strdup(pdptype);
    }
    /* IP Addresses */
    if (s_rilPDPContextListRes.addresses != NULL) {
        free(s_rilPDPContextListRes.addresses);
        s_rilPDPContextListRes.addresses = NULL;
    }
    if (ipAddrStrp != NULL)
        s_rilPDPContextListRes.addresses = strdup(ipAddrStrp);
    
    /* Fast Dormancy supported only for SPRINT carrier devices at this time */
    if (getFirmwareInfoCarrierQMI(&GobiImageCarrier) && (GobiImageCarrier == eGOBI_IMG_CAR_SPRINT))
    {
        nRet = SLQSWdsGoDormant();
        if (eQCWWAN_ERR_NONE != nRet) {
            LOGE("%s::SLQSWdsGoDormant failed, nRet: %lu\n",__func__, nRet );
            goto error;
        }
        LOGD("%s::SLQSWdsGoDormant Success\n",__func__ );
    }   
 
finally:

    if (ipAddrStrp != NULL)
        free(ipAddrStrp);
    /* Always return successful response */
    RIL_onRequestComplete(t, RIL_E_SUCCESS, &s_rilPDPContextListRes, sizeof(RIL_Data_Call_Response_v11));
    LOGD("requestSetupDefaultPDP done");
    return;

error:
    LOGD("requestSetupDefaultPDP error handling");
    /* This is work around to get eHRPD from data bearer */
    if (!getPropertyRadioNotifiction() && isDualModeRunningCDMA()) {
        registerDormancyStatusCallbackQMI(false);
    }
    if ((pdp_active == 1) && (s_sessionIDQMI != 0)) {

        /* CUS90903: Workaround for data connnection lost during idleness: 
         * Always send ERROR_UNSPECIFIED, because PROTOCOL_ERRORS is treated 
         * as permanenet failure at framework
         * PDP_FAIL_ERROR_UNSPECIFIED will kick off retrying of data call 
         */
        setLastPdpFailCauseQMI(PDP_FAIL_ERROR_UNSPECIFIED);
        isDataSessionCloseByClient(true);
        /* deactivate PDP context */
        nRet = StopDataSession ( s_sessionIDQMI );

        if ((nRet == eQCWWAN_ERR_NONE) || (nRet == eQCWWAN_ERR_QMI_NO_EFFECT)) {
            LOGI("requestSetupDefaultPDPQMI error handle: Stopped the session %lu\n", s_sessionIDQMI);
            s_sessionIDQMI = 0;
            /* stop DHCP client */
            checkDHCPnStopService();
            if (nRet == eQCWWAN_ERR_QMI_NO_EFFECT) {
                isDataSessionCloseByClient(false);
            }
        }
        else
        {
            LOGE("%s:: error handle: Stopped the session %lu failed with %lu \n", __func__, s_sessionIDQMI, nRet);
            isDataSessionCloseByClient(false);
        }
    }
    s_rilPDPContextListRes.status = getLastPdpFailCauseQMI();

    /* de-activate network ifc, ril_iface */
    activateNetworkInterface(false);
    goto finally;
}


/**
 *
 * RIL_REQUEST_DEACTIVATE_DATA_CALL for SWI_RIL_VERSION_12
 * Deactivate PDP context created by RIL_REQUEST_SETUP_DATA_CALL.
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
void requestDeactivateDefaultPDPQMI_r12(void *data, size_t datalen, RIL_Token t)
{
    const char *pPdpCid = NULL;
    int disconnectReason = RIL_DISCONNECT_REASON_NO_SPECIFIC;
    ULONG nRet;

    if (t != NULL) {   
        pPdpCid = ((const char **) data)[0];
        if ((((datalen/sizeof(char *)) >= RIL_REQUEST_DEACTIVATE_DATA_CALL_REQUEST_PARAMS)) && 
            (((const char **) data)[1] != NULL)) {
            disconnectReason = atoi(((const char **) data)[1]);
        }
        LOGD("requestDeactivateDefaultPDPQMI_r12 PdpCid '%s' DisconnectReason '%d'", pPdpCid, disconnectReason);

        /* Regardless of the disconnect reason, stop the data session first, and 
         * then do any further actions below.  This way, we always know that the
         * data session was properly stopped.
         */    
        if ((pPdpCid != NULL) && (s_sessionIDQMI != 0)) {
            if (atoi(pPdpCid) == SWI_DEFAULT_CID) {
                isDataSessionCloseByClient(true);
                nRet = StopDataSession ( s_sessionIDQMI );
                if ((nRet != eQCWWAN_ERR_NONE) && (nRet != eQCWWAN_ERR_QMI_NO_EFFECT)) {
                    LOGE("%s StopDataSession failed %lu\n", __func__, nRet);
                    isDataSessionCloseByClient(false);
                    goto error;
                } else if (nRet == eQCWWAN_ERR_QMI_NO_EFFECT) {
                    isDataSessionCloseByClient(false);
                }
                LOGI("%s successfully stopped the session %lu\n", __func__, s_sessionIDQMI);
                s_sessionIDQMI = 0;
                clearDHCPproperty();
            }
            else {
                LOGE("requestDeactivateDefaultPDP_r12 CID is invalid %s\n", pPdpCid);
                goto error;
            }
        }
        else {
            LOGE("requestDeactivateDefaultPDP_r12 error: CID: %s, s_sessionID: %d",
                pPdpCid, (int) s_sessionIDQMI);
            goto error;
		}
    }
    else {
        if (s_sessionIDQMI != 0) {
            isDataSessionCloseByClient(true);
            nRet = StopDataSession ( s_sessionIDQMI );
            if ((nRet != eQCWWAN_ERR_NONE) && (nRet != eQCWWAN_ERR_QMI_NO_EFFECT)) {
                LOGE("%s StopDataSession failed %lu\n", __func__, nRet);
                isDataSessionCloseByClient(false);
                goto error;
            } else if (nRet == eQCWWAN_ERR_QMI_NO_EFFECT) {
                isDataSessionCloseByClient(false);
            }
            LOGI("%s successfully stopped the session %lu\n", __func__, s_sessionIDQMI);
            s_sessionIDQMI = 0;
            clearDHCPproperty();
        }
        else {
            LOGE("requestDeactivateDefaultPDP_r12 error: s_sessionID: %d",(int) s_sessionIDQMI);
            goto error;
		}
    }
    /* START MOTO 172 */
    /* When turning off the RADIO (e.g., entering airplane mode) Android will first attempt to gracefully
     * close the data call (with a timeout of 30 seconds if data call disconnect is not received) and then
     * send a request to enter low power mode. The RIL should not switch to low power mode here as it'll
     * result in two seperate requests to enter LPM mode which can interfear with the user attempting to 
     * quickly exit from airplane mode.
     */
#if 0
    /* SWI_TBD only observed RIL_DISCONNECT_REASON_RADIO_SHUTDOWN when device is switching off
     * set to airplane mode for now */
    if (disconnectReason == RIL_DISCONNECT_REASON_RADIO_SHUTDOWN) {
        changePowerModeQMI(QMI_POWER_LOW);
        s_sessionIDQMI = 0;
        setRadioState(RADIO_STATE_OFF);
    }
#endif /* 0 */
    /* STOP MOTO 172 */

    /* clean up data call list when data call deactivated */
    initDataCallResponseList_r12();
    s_rilPDPContextListRes.status = PDP_FAIL_NONE;
    /* stop DHCP client */
    checkDHCPnStopService();

    /* de-activate network ifc, ril_iface */
    activateNetworkInterface(false);
    if (t != NULL)
        RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    else {
        if (s_rilPDPContextListRes.status != PDP_FAIL_RADIO_POWER_OFF) {
            RIL_onUnsolicitedResponse(RIL_UNSOL_DATA_CALL_LIST_CHANGED,
                                      &s_rilPDPContextListRes,
                                      sizeof(RIL_Data_Call_Response_v11));
        }
    }
    return;
    
error:
    if (t != NULL)
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

/**
 *
 * getActivatedPDPContext
 * Returns the active PDP context.
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
 *     This function added for updating Dormancy Status to Android UI.
 *     This function called in dormancyStatusCallback().
 */
RIL_Data_Call_Response_v11* getActivatedPDPContext_r12( void )
{
    return &s_rilPDPContextListRes;
}


void dormancyStatusCallback_r12(ULONG dormancyStatus)
{
    RIL_Data_Call_Response_v11* pActivePDPContext;

    LOGD("dormancyStatusCallback_r12: dormancyStatus=%lx\n", dormancyStatus );
    if ( dormancyStatus == QMI_TRAFFIC_CHANNEL_ACTIVE) {
        /* can't run in callback, queue up */
        enqueueRILEvent(RIL_EVENT_QUEUE_NORMAL, checkEHRPD,
                        NULL, NULL);
    }

    pActivePDPContext = getActivatedPDPContext_r12();
    if (dormancyStatus == QMI_TRAFFIC_CHANNEL_ACTIVE) {
        pActivePDPContext->active = RIL_STATE_ACTIVE_LINKUP;
    }
    else if (dormancyStatus == QMI_TRAFFIC_CHANNEL_DORMANT) {
        pActivePDPContext->active = RIL_STATE_ACTIVE_LINKDOWN;
    }
    RIL_onUnsolicitedResponse( RIL_UNSOL_DATA_CALL_LIST_CHANGED,
                               pActivePDPContext,
                               sizeof(RIL_Data_Call_Response_v11) );
}
#endif


