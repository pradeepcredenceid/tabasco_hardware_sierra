/*
 * \ingroup wds
 *
 * \file qaGobiApiWds.c
 *
 * \brief  Entry points for Gobi APIs for the Wireless Data Service (WDS)
 *
 * Copyright: © 2011-2013 Sierra Wireless, Inc. all rights reserved
 *
 */
#include <string.h> 
#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "amudefs.h"
#include "sludefs.h"
#include "qaQmiBasic.h"
#include "qaGobiApiWds.h"
#include "qaGobiApiFms.h"
#include "qaWdsStartNetworkInterface.h"
#include "qaWdsStopNetworkInterface.h"
#include "qaWdsSetMipMode.h"
#include "qaWdsSetMipParams.h"
#include "qaWdsGetAutoconnectSetting.h"
#include "qaWdsGetDefaultSettings.h"
#include "qaWdsGetPktSrvcStatus.h"
#include "qaWdsGetPktStatistics.h"
#include "qaWdsGetDormancyState.h"
#include "qaWdsGetDataBearerTechnology.h"
#include "qaWdsGetCallDuration.h"
#include "qaWdsGetRuntimeSettings.h"
#include "qaWdsGetCurrentChannelRate.h"
#include "qaWdsModifyProfile.h"
#include "qaWdsGetMipMode.h"
#include "qaWdsReadMipProfile.h"
#include "qaWdsGetLastMipStatus.h"
#include "qaWdsSLQSGetRuntimeSettings.h"
#include "qaWdsSLQSGetProfileSettings.h"
#include "qaWdsSLQSSetIPFamilyPreference.h"
#include "qaWdsSLQSDeleteProfile.h"
#include "qaWdsSLQSCreateProfile.h"
#include "qaWdsSLQSACSettings.h"
#include "qaGobiApiDms.h"
#include "qaWdsGetCurDataBearerTechnology.h"
#include "qaWdsSLQSModifyProfile.h"
#include "os/swi_osapi.h"
#include "qaWdsSLQSSet3GPPConfigItem.h"
#include "qaWdsSLQSGet3GPPConfigItem.h"
#include "qaWdsModifyMipProfile.h"
#include "qaWdsSLQSWdsSetEventReport.h"
#include "qaWdsSLQSWdsSwiPDPRuntimeSettings.h"
#include "qaWdsSLQSWdsGoDormant.h"

#define PKT_STAT_STAT_MASK 0X0000003F
#define BYT_STAT_STAT_MASK 0X000000C0
#define MIN_PROFILE_ID     1
#define MAX_PROFILE_ID     16
#define START_DATA_SESSION 1
#define STOP_DATA_SESSION  0
#define SET_AUTO_CONNECT   1
#define GET_AUTO_CONNECT   0

/* DEBUG defines */
#define DBG_WDS
#ifdef DBG_WDS
#include <syslog.h>
#endif

/* enumerations */
enum pdpidx{
    WDSPDP1IDX,
    WDSPDP2IDX,
    WDSPDP3IDX,
    WDSNUMPDPS
};

/* For an IPv4v6 session, we abstract the added complexity of managing two data
 * sessions, a v4 and a v6, from the application by storing both session handles
 * internally. When the session is subsequently terminated, the user will pass in
 * the IPv6 session handle which will be matched up with the correct IPv4 session
 * handle. We do not register for callback notifications when the session state
 * changes. The application, if registered will receive these; if not, the attempt
 * to stop the data session will simply return a relevant error.
 */
struct qaWdsPDPSessionData{
    enum pdpidx pdpIdx;         /* get to know myself */
    ULONG v4sessionId;          /* IPv4 QMI client session handle */
    ULONG v6sessionId;          /* IPv6 QMI client session handle */

    /* critical section mutex */
    struct swi_osmutex pdpmutex;

    /* private interface */
    void (*pdplock)(enum pdpidx);
    void (*pdpunlock)(enum pdpidx);
};

local struct wdsSessionMgr{
    /* private data */
    struct qaWdsPDPSessionData pdp[WDSNUMPDPS];
    int v4fs;   /* next free v4 slot */
    int v6fs;   /* next free v6 slot */
    int v4v6fs; /* next free v4v6 slot */

    /* critical section mutex */
    struct swi_osmutex smmutex;

    /* private interface */
    void (*smlock)(void);
    void (*smunlock)(void);
    void (*updateslots)(void);
    int  (*getnextslot)(int);
    int  (*sid2slot)(ULONG);
    void (*print)(struct ssdatasession_params *, void *);
}wdsSessionManager;
/*-----------------------------------------------
    WDS Data Session Manager - public methods
  ----------------------------------------------*/
#ifdef DBG_WDS
local void wdsSessionDataLog(
    struct qaWdsPDPSessionData  *ps,
    struct ssdatasession_params *pd,
    void *prc )
{
    UNUSEDPARAM(pd);

    syslog( LOG_DEBUG,
            "%s/%d: PDP%d: v4SH: %lu, v6SH: %lu",
            __func__,
            __LINE__,
            ps->pdpIdx,
            ps->v4sessionId,
            ps->v6sessionId );

    if( NULL != prc )
    {
        syslog( LOG_DEBUG,
                "%s/%d: Error Code: %lu (0x%lX)\n",
                __func__,
                __LINE__,
                *(ULONG *)prc,
                *(ULONG*)prc );
    }
}

local void qaWdsSMprintSlots()
{
    syslog( LOG_DEBUG,
            "%s/%d: V4 FSLOT: %d, V6 FSLOT: %d, V4V6 FSLOT: %d",
            __func__, __LINE__,
            wdsSessionManager.v4fs,
            wdsSessionManager.v6fs,
            wdsSessionManager.v4v6fs );
}

local void wdsSMprint(
    struct ssdatasession_params *pd,
    void *prc )
{
    if( NULL != pd )
    {
        syslog( LOG_DEBUG,
                "%s/%d: action: %s, sid: %lu, v4sid: %lu, v6sid: %lu",
                __func__,
                __LINE__,
                pd->action == 0 ? "STOP" : "START",
                pd->sessionId,
                pd->v4sessionId,
                pd->v6sessionId );
    }

    qaWdsSMprintSlots();

    int i;
    for( i = 0 ; i < WDSNUMPDPS ; i++ )
    {
        wdsSessionDataLog( &wdsSessionManager.pdp[i], pd, prc );
    }
    syslog( LOG_DEBUG, "%s/%d: - - - - - - - - - - - - - - - - - - - - - - - - - - -",
            __func__,
            __LINE__ );
}
#endif

/*-----------------------------------------------
    WDS Data Session Manager - private methods
  ----------------------------------------------*/
local void qaWdsSMlock(void)
{
    swi_osapi_mutexlock(&wdsSessionManager.smmutex);
}

local void qaWdsSMunlock(void)
{
    swi_osapi_mutexunlock(&wdsSessionManager.smmutex);
}

local void qaWdsPDPSessionDataLock(enum pdpidx idx)
{
    swi_osapi_mutexlock(&wdsSessionManager.pdp[idx].pdpmutex);
}

local void qaWdsPDPSessionDataUnLock(enum pdpidx idx)
{
    swi_osapi_mutexunlock(&wdsSessionManager.pdp[idx].pdpmutex);
}

local int qaWdsSMSessionIDtoPDPInstance(ULONG sessionId)
{
    struct qaWdsPDPSessionData *pd= wdsSessionManager.pdp;

    int i;
    for( i = 0; i < WDSNUMPDPS ; i++ )
    {
        if( sessionId == pd[i].v4sessionId )
        {
           /* PDP instance found */
            return i;
        }

        if( sessionId == pd[i].v6sessionId )
        {
           /* PDP instance found */
            return i;
        }
    }

    /* no match found */
    return -1;
}

local void qaWdsSMUpdateAvailablePDPIdx()
{
    struct qaWdsPDPSessionData *pd= wdsSessionManager.pdp;
    BOOL searchv4 = TRUE;
    BOOL searchv6 = TRUE;
    BOOL searchv4v6 = TRUE;

    wdsSessionManager.v4fs = -1;
    wdsSessionManager.v6fs = -1;
    wdsSessionManager.v4v6fs = -1;

    int i;
    for( i = 0; i < WDSNUMPDPS ; i++ )
    {
        /* udpate available v4v6 slot */
        if( searchv4v6 && 0 == pd[i].v4sessionId && 0 == pd[i].v6sessionId )
        {
            /* available v4v6 slot found */
            wdsSessionManager.v4v6fs = i;
            wdsSessionManager.v4fs = i;
            wdsSessionManager.v6fs = i;
            searchv4v6 = FALSE;
            continue;
        }

        /* update available v4 slot */
        if( searchv4 && 0 == pd[i].v4sessionId && 0 != pd[i].v6sessionId )
        {
             wdsSessionManager.v4fs = i;
            searchv4 = FALSE;
        }

        /* udpate available v6 slot */
        if( searchv6 && 0 == pd[i].v6sessionId && 0 != pd[i].v4sessionId )
        {
            wdsSessionManager.v6fs = i;
            searchv6 = FALSE;
        }
    }
}

local int qaWdsSMGetAvailablePDPIdx(int iptype)
{
    return IPV4 == iptype ? wdsSessionManager.v4fs :
           IPV6 == iptype ? wdsSessionManager.v6fs :
           IPV4V6 == iptype ? wdsSessionManager.v4v6fs : -1;
}

/*-----------------------------
    Generic Helper Functions
 -----------------------------*/
package BOOL WDS_IsGobiDevice()
{
    BYTE stringsize = 128;
    CHAR ModelId[stringsize];
    CHAR *pstr = NULL;

    GetModelID( stringsize,
                ModelId );

    /* TODO: check if update is needed similar to qaGobiApiFms.c */
    pstr = strstr (ModelId, "MC83");
    if( NULL != pstr )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/**
 *  Retrieves the session information for the corresponding PDP index
 *
 *  \param  pdpIndex
 *          - The PDP index for which the session information needs to be
 *            fetched
 *
 *  \param  pV4sessionId[OUT]
 *          - IPV4 session index
 *
 *  \param  pV6sessionId[OUT]
 *          - IPV6 session index
 *
 *  \return TRUE on success, FALSE otherwise
 *
 *  \note   This function is implemented to retrieve the session information for
 *          the WDS notifications. The function is execute in the notification
 *          thread context and hence the session manager information needs to be
 *          protected
 *
 */
global BOOL qaRetrieveWDSSessionIDs(
    BYTE  pdpIndex,
    ULONG *pV4sessionId,
    ULONG *pV6sessionId )
{
    if(pdpIndex >= WDSNUMPDPS)
    {
        return FALSE;
    }
    /**
     * Data access from the notification thread, hence protecting the memory
     * for the corresponding information
     */
    qaWdsPDPSessionDataLock(pdpIndex);

    *pV4sessionId = wdsSessionManager.pdp[pdpIndex].v4sessionId;
    *pV6sessionId = wdsSessionManager.pdp[pdpIndex].v6sessionId;

    qaWdsPDPSessionDataUnLock(pdpIndex);
    return TRUE;
}

ULONG SetMobileIP( ULONG mode )
{
    ULONG  resultcode;  /* result code */
    BYTE  *pInParam;   /* ptr to param field rx'd from modem */
    BYTE  *pOutParam;  /* ptr to outbound param field */
    BYTE  *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT ParamLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiWdsSetMipModeResp response;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultcode = PkQmiWdsSetMipMode( &ParamLength,
                                     pOutParam,
                                     mode );

    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_WDS,
                                  ParamLength,
                                  eQMI_TIMEOUT_2_S, /* 2 seconds */
                                  &pInParam,
                                  &ParamLength );

    /* Only parse out the response data if we got a positive return */
    if (resultcode == eQCWWAN_ERR_NONE)
    {
        /* Copy to the caller's buffer */
        resultcode = UpkQmiWdsSetMipMode( pInParam, &response );
    }

    qmrelreqbkp();
    return resultcode;
}
ULONG SetMobileIPParameters(
    CHAR  *pSPC,
    ULONG *pMode,
    BYTE  *pRetryLimit,
    BYTE  *pRetryInterval,
    BYTE  *pReRegPeriod,
    BYTE  *pReRegTraffic,
    BYTE  *pHAAuthenticator,
    BYTE  *pHA2002bis )
{
    ULONG             resultcode;  /* result code */
    BYTE              *pInParam;   /* ptr to param field rx'd from modem */
    BYTE              *pOutParam;  /* ptr to outbound param field */
    BYTE              *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT            ParamLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiWdsSetMipParamsResp response;

    /* SPC is Mandatory Parameter */
    if( !pSPC )
        return eQCWWAN_ERR_INVALID_ARG;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultcode = PkQmiWdsSetMipParams( &ParamLength,
                                   pOutParam,
                                   pSPC,
                                   pMode,
                                   pRetryLimit,
                                   pRetryInterval,
                                   pReRegPeriod,
                                   pReRegTraffic,
                                   pHAAuthenticator,
                                   pHA2002bis );

    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_WDS,
                                  ParamLength,
                                  eQMI_TIMEOUT_2_S, /* 2 seconds */
                                  &pInParam,
                                  &ParamLength );

    /* Only parse out the response data if we got a positive return */
    if (resultcode == eQCWWAN_ERR_NONE)
        /* Copy to the caller's buffer */
        resultcode = UpkQmiWdsSetMipParams(pInParam, &response);

    qmrelreqbkp();
    return resultcode;
}

ULONG GetAutoconnect( ULONG *pSetting )
{
    ULONG               resultcode;  /* result code */
    BYTE                *pInParam;   /* ptr to param field rx'd from modem */
    BYTE                *pOutParam;  /* ptr to outbound param field */
    BYTE                *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT              ParamLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiWdsGetAutoconnectSettingResp response;

    /* pSetting is an OUT parameter and hence should not be NULL */
    if ( !pSetting )
        return eQCWWAN_ERR_INVALID_ARG;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultcode = PkQmiWdsGetAutoconnectSetting( &ParamLength, pOutParam );

    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_WDS,
                                  ParamLength,
                                  eQMI_TIMEOUT_2_S, /* 2 seconds */
                                  &pInParam,
                                  &ParamLength );

    /* Only parse out the response data if we got a positive return */
    if (resultcode == eQCWWAN_ERR_NONE)
    {
        /* Pass the pointer for the OUT parameters */
        response.pSetting = pSetting;

        /* Copy to the caller's buffer */
        resultcode = UpkQmiWdsGetAutoconnectSetting( pInParam, &response );
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG GetDefaultProfile(
    ULONG profileType,
    ULONG *pPDPType,
    ULONG *pIPAddress,
    ULONG *pPrimaryDNS,
    ULONG *pSecondaryDNS,
    ULONG *pAuthentication,
    BYTE  nameSize,
    CHAR  *pName,
    BYTE  apnSize,
    CHAR  *pAPNName,
    BYTE  userSize,
    CHAR  *pUsername,
    BYTE  pwdSize,
    CHAR  *pPassword)
{
    ULONG             resultcode;  /* result code */
    BYTE              *pInParam;   /* ptr to param field rx'd from modem */
    BYTE              *pOutParam;  /* ptr to outbound param field */
    BYTE              *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT            ParamLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiWdsGetDefaultSettingsResp response;

    /* Check against NULL for out params */
    if ( !pPDPType          ||
         !pIPAddress        ||
         !pPrimaryDNS       ||
         !pSecondaryDNS     ||
         !pAuthentication   ||
         !pName             ||
         !pAPNName          ||
         !pUsername         ||
         !pPassword)
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultcode = PkQmiWdsGetDefaultSettings( &ParamLength,
                                         pOutParam,
                                         profileType );

    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_WDS,
                                  ParamLength,
                                  eQMI_TIMEOUT_2_S, /* 2 seconds */
                                  &pInParam,
                                  &ParamLength );

    /* Only parse out the response data if we got a positive return */
    if (resultcode == eQCWWAN_ERR_NONE)
    {
        /* Initialisation of Buffer */
        slmemset((char *)pName,     0, nameSize);
        slmemset((char *)pAPNName,  0, apnSize);
        slmemset((char *)pUsername, 0, userSize);
        slmemset((char *)pPassword, 0, pwdSize);
        /* Pass the pointer for the OUT parameters */
        response.pPDPType        = pPDPType;
        response.pIPAddress      = pIPAddress;
        response.pPrimaryDNS     = pPrimaryDNS;
        response.pSecondaryDNS   = pSecondaryDNS;
        response.pIPAddressv6    = NULL;
        response.pPrimaryDNSv6   = NULL;
        response.pSecondaryDNSv6 = NULL;
        response.pAuthentication = pAuthentication;
        response.pName           = pName;
        response.pAPNName        = pAPNName;
        response.pUserName       = pUsername;
        response.pPassword       = pPassword;
        response.nameSize        = nameSize;
        response.apnSize         = apnSize;
        response.userSize        = userSize;
        response.pwdSize         = pwdSize;

        /* Copy to the caller's buffer */
        resultcode = UpkQmiWdsGetDefaultSettings( pInParam, &response );
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG GetDefaultProfileLTE(
    ULONG  profileType,
    ULONG  *pPDPType,
    ULONG  *pIPAddressv4,
    ULONG  *pPrimaryDNSv4,
    ULONG  *pSecondaryDNSv4,
    USHORT *pIPAddressv6,
    USHORT *pPrimaryDNSv6,
    USHORT *pSecondaryDNSv6,
    ULONG  *pAuthentication,
    BYTE   nameSize,
    CHAR   *pName,
    BYTE   apnSize,
    CHAR   *pAPNName,
    BYTE   userSize,
    CHAR   *pUsername)
{
    ULONG             resultcode;  /* result code */
    BYTE              *pInParam;   /* ptr to param field rx'd from modem */
    BYTE              *pOutParam;  /* ptr to outbound param field */
    BYTE              *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT            ParamLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiWdsGetDefaultSettingsResp response;

    /* Check against NULL for out params */
    if ( !pPDPType          ||
         !pIPAddressv4      ||
         !pPrimaryDNSv4     ||
         !pSecondaryDNSv4   ||
         !pIPAddressv6      ||
         !pPrimaryDNSv6     ||
         !pSecondaryDNSv6   ||
         !pAuthentication   ||
         !pName             ||
         !pAPNName          ||
         !pUsername )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultcode = PkQmiWdsGetDefaultSettings( &ParamLength,
                                             pOutParam,
                                             profileType );

    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_WDS,
                                  ParamLength,
                                  eQMI_TIMEOUT_2_S, /* 2 seconds */
                                  &pInParam,
                                  &ParamLength );

    /* Only parse out the response data if we got a positive return */
    if (resultcode == eQCWWAN_ERR_NONE)
    {
        /* Initialisation of Buffer */
        slmemset((char *)pName,     0, nameSize);
        slmemset((char *)pAPNName,  0, apnSize);
        slmemset((char *)pUsername, 0, userSize);
        /* Pass the pointer for the OUT parameters */
        response.pPDPType        = pPDPType;
        response.pIPAddress      = pIPAddressv4;
        response.pPrimaryDNS     = pPrimaryDNSv4;
        response.pSecondaryDNS   = pSecondaryDNSv4;
        response.pIPAddressv6    = pIPAddressv6;
        response.pPrimaryDNSv6   = pPrimaryDNSv6;
        response.pSecondaryDNSv6 = pSecondaryDNSv6;
        response.pAuthentication = pAuthentication;
        response.pName           = pName;
        response.pAPNName        = pAPNName;
        response.pUserName       = pUsername;
        response.nameSize        = nameSize;
        response.apnSize         = apnSize;
        response.userSize        = userSize;

        /* Copy to the caller's buffer */
        resultcode = UpkQmiWdsGetDefaultSettings( pInParam, &response );
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG  GetSessionState(
    ULONG    *pState,
    BYTE     instance )
{
    ULONG              resultcode;  /* result code */
    BYTE               *pInParam;    /* ptr to param field rx'd from modem */
    BYTE               *pOutParam;   /* ptr to outbound param field */
    BYTE               *pReqBuf;     /* Pointer to outgoing request buffer */
    USHORT             ParamLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiWdsGetPktSrvcStatusResp response;

    /* pState is OUT parameter and hence should not be NULL.*/
    if ( !pState )
    {
        return eQCWWAN_ERR_INVALID_ARG; /* QCWWAN_ERR_INVALID_ARGUMENT; */
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultcode = PkQmiWdsGetPktSrvcStatus(
                &ParamLength,
                pOutParam);

    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiMISendnWait( pReqBuf,
                                   eQMI_SVC_WDS,
                                   ParamLength,
                                   eQMI_TIMEOUT_2_S,
                                   &pInParam,
                                   &ParamLength,
                                   instance );
    /* Only parse out the response data if we got a positive return */
    if (resultcode == eQCWWAN_ERR_NONE)
    {
        /* Pass the pointer for the OUT parameters */
        response.pConnectionStatus = pState;

        /* Copy to the caller's buffer */
        resultcode = UpkQmiWdsGetPktSrvcStatus(pInParam, &response);
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG  GetPacketStatus(
    ULONG    *pTXPacketSuccesses,
    ULONG    *pRXPacketSuccesses,
    ULONG    *pTXPacketErrors,
    ULONG    *pRXPacketErrors,
    ULONG    *pTXPacketOverflows,
    ULONG    *pRXPacketOverflows,
    BYTE     instance )
{
    ULONG              resultcode;  /* result code */
    BYTE               *pInParam;   /* ptr to param field rx'd from modem */
    BYTE               *pOutParam;  /* ptr to outbound param field */
    BYTE               *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT             ParamLength; /* Ret'd length of the QMI Param field */
    ULONG              StatMask;

    /* Storage for results and response variable */
    struct QmiWdsGetPktStatisticsResp response;

    /* stat_mask to be set according to following settings
     * Requested statistic bit mask
     *    0x00000001  Tx packets OK
     *    0x00000002  Rx packets OK
     *    0x00000004  Tx packet errors
     *    0x00000008  Rx packet errors
     *    0x00000010  Tx overflows
     *    0x00000020  Rx overflows
     */
    StatMask = PKT_STAT_STAT_MASK;

    /*
     * pTXPacketSuccesses, pRXPacketSuccesses
     * pTXPacketErrors,    pRXPacketErrors
     * pTXPacketOverflows, pRXPacketOverflows
     * are OUT parameters and hence should not be NULL.
     */
    if ( !pTXPacketSuccesses   ||
         !pRXPacketSuccesses   ||
         !pTXPacketErrors      ||
         !pRXPacketErrors      ||
         !pTXPacketOverflows   ||
         !pRXPacketOverflows )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }
    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultcode = PkQmiWdsGetPktStatistics( &ParamLength,
                                           pOutParam,
                                           StatMask );

    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
     resultcode = SwiQmiMISendnWait( pReqBuf,
                                     eQMI_SVC_WDS,
                                     ParamLength,
                                     eQMI_TIMEOUT_2_S,
                                     &pInParam,
                                     &ParamLength,
                                     instance );
    /* Only parse out the response data if we got a positive return */
    if (resultcode == eQCWWAN_ERR_NONE)
    {
        /* Pass the pointer for the OUT parameters */
        response.pTXPacketSuccesses = pTXPacketSuccesses;
        response.pRXPacketSuccesses = pRXPacketSuccesses;
        response.pTXPacketErrors    = pTXPacketErrors;
        response.pRXPacketErrors    = pRXPacketErrors;
        response.pTXPacketOverflows = pTXPacketOverflows;
        response.pRXPacketOverflows = pRXPacketOverflows;

        *(response.pTXPacketSuccesses) = 0;
        *(response.pRXPacketSuccesses) = 0;
        *(response.pTXPacketErrors)    = 0;
        *(response.pRXPacketErrors)    = 0;
        *(response.pTXPacketOverflows) = 0;
        *(response.pRXPacketOverflows) = 0;
        /* Copy to the caller's buffer */
        resultcode = UpkQmiWdsGetPktStatistics(pInParam, &response);
    }

    qmrelreqbkp();
    return resultcode;

}

ULONG GetByteTotals(
    ULONGLONG    *pTXTotalBytes,
    ULONGLONG    *pRXTotalBytes,
    BYTE         instance )
{
    ULONG              resultcode;  /* result code */
    BYTE               *pInParam;   /* ptr to param field rx'd from modem */
    BYTE               *pOutParam;  /* ptr to outbound param field */
    BYTE               *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT             ParamLength; /* Ret'd length of the QMI Param field */
    ULONG              StatMask;

    /* Storage for results and response variable */
    struct QmiWdsGetPktStatisticsResp response;

    /* stat_mask to be set according to following settings
     * Requested statistic bit mask
     *    0x00000040  Tx bytes OK
     *    0x00000080  Rx bytes OK
     */
    StatMask = BYT_STAT_STAT_MASK;

    /*
     * pTXTotalBytes, pRXTotalBytes
     * are OUT parameters and hence should not be NULL.
     */
    if ( !pTXTotalBytes ||
         !pRXTotalBytes )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }
    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultcode = PkQmiWdsGetPktStatistics(
                &ParamLength,
                pOutParam,
                StatMask);

    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
     resultcode = SwiQmiMISendnWait( pReqBuf,
                                   eQMI_SVC_WDS,
                                   ParamLength,
                                   eQMI_TIMEOUT_2_S,
                                   &pInParam,
                                   &ParamLength,
                                   instance );
    /* Only parse out the response data if we got a positive return */
    if (resultcode == eQCWWAN_ERR_NONE)
    {
        /* Pass the pointer for the OUT parameters */
        response.pTXOkBytesCount = pTXTotalBytes;
        response.pRXOkBytesCount = pRXTotalBytes;

        /* Initialise the Values */
        *(response.pTXOkBytesCount) = 0;
        *(response.pRXOkBytesCount) = 0;

        /* Copy to the caller's buffer */
        resultcode = UpkQmiWdsGetPktStatistics(pInParam, &response);
    }

    qmrelreqbkp();
    return resultcode;

}

ULONG  GetDormancyState(
   ULONG *pDormancyState,
   BYTE  instance )
{
    ULONG              resultcode;  /* result code */
    BYTE               *pInParam;   /* ptr to param field rx'd from modem */
    BYTE               *pOutParam;  /* ptr to outbound param field */
    BYTE               *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT             ParamLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiWdsGetDormancyResp response;

    /* pDormancyState is OUT parameter and hence should not be NULL.*/
    if ( !pDormancyState )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }
    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultcode = PkQmiWdsGetDormancyState(
                &ParamLength,
                pOutParam);

    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiMISendnWait( pReqBuf,
                                  eQMI_SVC_WDS,
                                  ParamLength,
                                  eQMI_TIMEOUT_2_S,
                                  &pInParam,
                                  &ParamLength,
                                  instance );
    /* Only parse out the response data if we got a positive return */
    if (resultcode == eQCWWAN_ERR_NONE)
    {
        /* Pass the pointer for the OUT parameters */
        response.pDormancyState  = pDormancyState;

        /* Copy to the caller's buffer */
        resultcode = UpkQmiWdsGetDormancyState(pInParam, &response);
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG  GetDataBearerTechnology(
    ULONG *pDataBearer,
    BYTE  instance )
{
    ULONG              resultcode;   /* result code */
    BYTE               *pInParam;     /* ptr to param field rx'd from modem */
    BYTE               *pOutParam;    /* ptr to outbound param field */
    BYTE               *pReqBuf;      /* Pointer to outgoing request buffer */
    USHORT             ParamLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiWdsGetDataBearerResp response;

    /* pDataBearer is OUT parameter and hence should not be NULL.*/
    if ( !pDataBearer )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultcode = PkQmiWdsGetDataBearerTechnology(
                &ParamLength,
                pOutParam);

    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
     resultcode = SwiQmiMISendnWait( pReqBuf,
                                   eQMI_SVC_WDS,
                                   ParamLength,
                                   eQMI_TIMEOUT_2_S,
                                   &pInParam,
                                   &ParamLength,
                                   instance );
    /* Only parse out the response data if we got a positive return */
    if (resultcode == eQCWWAN_ERR_NONE)
    {
        /* Pass the pointer for the OUT parameters */
        response.pCurrentDataBearer = pDataBearer;

        /* Copy to the caller's buffer */
        resultcode = UpkQmiWdsGetDataBearerTechnology(pInParam, &response);
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSGetDataBearerTechnology(
     QmiWDSDataBearers *pDataBearers,
     BYTE instance )
{
    ULONG  resultcode;  /* result code */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT ParamLength; /* Ret'd length of the Param field */

    /* Storage for results and response variable */
    struct QmiWdsGetCurDataBearerResp response;

    /* OUT parameters should not be NULL.*/
    if (!pDataBearers)
        return eQCWWAN_ERR_INVALID_ARG;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultcode = PkQmiWdsGetCurDataBearerTechnology(&ParamLength, pOutParam);

    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiMISendnWait( pReqBuf,
                                    eQMI_SVC_WDS,
                                    ParamLength,
                                    eQMI_TIMEOUT_2_S,
                                    &pInParam,
                                    &ParamLength,
                                    instance );

    /* Only parse out the response data if we got a positive return */
    if (resultcode == eQCWWAN_ERR_NONE)
    {
        /* Reset the data bearer mask, this will be set while unpacking */
        pDataBearers->dataBearerMask = 0x00;
        response.pDataBearers        = pDataBearers;

        /* Copy to the caller's buffer */
        resultcode = UpkQmiWdsGetCurDataBearerTechnology(pInParam, &response);
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG  GetSessionDuration(
    ULONGLONG *pDuration,
    BYTE      instance )
{
    ULONG              resultcode;  /* result code */
    BYTE               *pInParam;   /* ptr to param field rx'd from modem */
    BYTE               *pOutParam;  /* ptr to outbound param field */
    BYTE               *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT             ParamLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiWdsGetDurationResp response;

    /* pDataBearer is OUT parameter and hence should not be NULL.*/
    if ( !pDuration )
    {
        return eQCWWAN_ERR_INVALID_ARG; /* QCWWAN_ERR_INVALID_ARGUMENT; */
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultcode = PkQmiWdsGetCallDuration( &ParamLength,
                                      pOutParam );

    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
     resultcode = SwiQmiMISendnWait( pReqBuf,
                                     eQMI_SVC_WDS,
                                     ParamLength,
                                     eQMI_TIMEOUT_2_S,
                                     &pInParam,
                                     &ParamLength,
                                     instance );
    /* Only parse out the response data if we got a positive return */
    if (resultcode == eQCWWAN_ERR_NONE)
    {
        /* Pass the pointer for the OUT parameters */
        response.pCallDuration    = pDuration;
        *(response.pCallDuration) = 0;

        /* Copy to the caller's buffer */
        resultcode = UpkQmiWdsGetCallDuration(pInParam, &response);
    }

    qmrelreqbkp();
    return resultcode;

}

ULONG GetConnectionRate(
    ULONG     *pCurrentChannelTXRate,
    ULONG     *pCurrentChannelRXRate,
    ULONG     *pMaxChannelTXRate,
    ULONG     *pMaxChannelRXRate,
    BYTE      instance )
{
    ULONG             resultcode;  /* result code */
    BYTE              *pInParam;   /* ptr to param field rx'd from modem */
    BYTE              *pOutParam;  /* ptr to outbound param field */
    BYTE              *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT            ParamLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiWdsGetCurrentChannelRateResp response;

    if ( !pCurrentChannelTXRate ||
         !pCurrentChannelRXRate ||
         !pMaxChannelTXRate     ||
         !pMaxChannelRXRate)
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Index points to first QMI header*/
    ParamLength = 0;

    resultcode = PkQmiWdsGetCurrentChannelRate( &ParamLength , pOutParam );
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiMISendnWait( pReqBuf,
                                  eQMI_SVC_WDS,
                                  ParamLength,
                                  eQMI_TIMEOUT_2_S, /* 2 Seconds */
                                  &pInParam,
                                  &ParamLength,
                                  instance );

    /* Only parse out the response data if we got a positive return */
    if (resultcode == eQCWWAN_ERR_NONE)
    {
        response.pCurrentChannelTXRate  = pCurrentChannelTXRate;
        response.pCurrentChannelRXRate  = pCurrentChannelRXRate;
        response.pMaxChannelTXRate      = pMaxChannelTXRate;
        response.pMaxChannelRXRate      = pMaxChannelRXRate;

        /* Copy to the caller's buffer */
        resultcode = UpkQmiWdsGetCurrentChannelRate(pInParam, &response);
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG  SetAutoconnect(
    ULONG    setting )
{
    ULONG              resultcode;  /* result code */

    /* Storage for request parameters */
    struct slqsautoconnect Req;

    /* Initialize the request structure*/
    Req.action = SET_AUTO_CONNECT;
    Req.acsetting = ( BYTE ) setting;
    Req.acroamsetting = 0x00;

    /* Call Api SLQSAutoConnect */
    resultcode = SLQSAutoConnect( &Req );

    return resultcode;
}

ULONG  SetDefaultProfile(
    ULONG profileType,
    ULONG *pPDPType,
    ULONG *pIPAddress,
    ULONG *pPrimaryDNS,
    ULONG *pSecondaryDNS,
    ULONG *pAuthentication,
    CHAR  *pName,
    CHAR  *pAPNName,
    CHAR  *pUsername,
    CHAR  *pPassword )
{
    ULONG  resultcode;            /* result code */
    BYTE  *pInParam;              /* ptr to param field rx'd from modem */
    BYTE  *pOutParam;             /* ptr to outbound param field */
    BYTE  *pReqBuf;               /* Pointer to outgoing request buffer */
    USHORT ParamLength;           /* Ret'd length of the QMI Param field */
    /* Storage for results and response variable */
    struct QmiWdsModifyProfileResp response;
    BYTE ProfileID = 1;
    char modelType[128];

    /*
     * Profile ID will be determined on the basis of device type
     * MC7750 device - Default Profile ID is 0
     * Other devices - Default Profile ID is 1
     */
    if( eQCWWAN_ERR_NONE == GetModelID( sizeof(modelType), &modelType[0] ) )
    {
        if( 0 == slstrncmp( "MC7750", modelType, 6) )
            ProfileID = 0;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Index points to first QMI header. */
    ParamLength = 0;

    resultcode = PkQmiWdsModifyProfile( &ParamLength,
                                        pOutParam,
                                        profileType,
                                        ProfileID,
                                        pPDPType,
                                        pIPAddress,
                                        pPrimaryDNS,
                                        pSecondaryDNS,
                                        NULL,
                                        NULL,
                                        NULL,
                                        pAuthentication,
                                        pName,
                                        pAPNName,
                                        pUsername,
                                        pPassword );
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
     resultcode = SwiQmiSendnWait( pReqBuf,
                                   eQMI_SVC_WDS,
                                   ParamLength,
                                   eQMI_TIMEOUT_2_S,
                                   &pInParam,
                                   &ParamLength );
    /* Only parse out the response data if we got a positive return */
    if (resultcode == eQCWWAN_ERR_NONE)
        /* Copy to the caller's buffer */
        resultcode = UpkQmiWdsModifyProfile(pInParam, &response);

    qmrelreqbkp();
    return resultcode;
}

ULONG  SetDefaultProfileLTE(
    ULONG  profileType,
    ULONG  *pPDPType,
    ULONG  *pIPAddressv4,
    ULONG  *pPrimaryDNSv4,
    ULONG  *pSecondaryDNSv4,
    USHORT *pIPAddressv6,
    USHORT *PrimaryDNSv6,
    USHORT *pSecondaryDNSv6,
    ULONG  *pAuthentication,
    CHAR   *pName,
    CHAR   *pAPNName,
    CHAR   *pUsername,
    CHAR   *pPassword )
{
    ULONG     resultcode; /* result code */
    BYTE      *pInParam;  /* ptr to param field rx'd from modem */
    BYTE      *pOutParam; /* ptr to outbound param field */
    BYTE      *pReqBuf;   /* Pointer to outgoing request buffer */
    USHORT    ParamLength;/* Ret'd length of the QMI Param field */
    BYTE      ProfileID = 1;  /* Profile ID */
    char      modelType[128];

    /* Storage for results and response variable */
    struct QmiWdsModifyProfileResp response;

    /*
     * Profile ID will be determined on the basis of device type
     * MC7750 device - Default Profile ID is 0
     * Other devices - Default Profile ID is 1
     */
    if( eQCWWAN_ERR_NONE == GetModelID( sizeof(modelType), &modelType[0] ) )
    {
        if( 0 == slstrncmp( "MC7750", modelType, 6) )
            ProfileID = 0;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Index points to first QMI header. */
    ParamLength = 0;

    resultcode = PkQmiWdsModifyProfile( &ParamLength,
                                        pOutParam,
                                        profileType,
                                        ProfileID,
                                        pPDPType,
                                        pIPAddressv4,
                                        pPrimaryDNSv4,
                                        pSecondaryDNSv4,
                                        pIPAddressv6,
                                        PrimaryDNSv6,
                                        pSecondaryDNSv6,
                                        pAuthentication,
                                        pName,
                                        pAPNName,
                                        pUsername,
                                        pPassword );

    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
     resultcode = SwiQmiSendnWait( pReqBuf,
                                   eQMI_SVC_WDS,
                                   ParamLength,
                                   eQMI_TIMEOUT_2_S,
                                   &pInParam,
                                   &ParamLength );

    /* Only parse out the response data if we got a positive return */
    if (resultcode == eQCWWAN_ERR_NONE)
        /* Copy to the caller's buffer */
        resultcode = UpkQmiWdsModifyProfile(pInParam, &response);

    qmrelreqbkp();
    return resultcode;
}

ULONG  GetMobileIP(
   ULONG    *pMode )
{
    ULONG  resultcode;  /* result code */
    BYTE  *pInParam;   /* ptr to param field rx'd from modem */
    BYTE  *pOutParam;  /* ptr to outbound param field */
    BYTE  *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT ParamLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiWdsGetMipModeResp response;

    /* pMode is OUT parameter and hence should not be NULL.*/
    if ( !pMode )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }
    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultcode = PkQmiWdsGetMipMode( &ParamLength,
                                     pOutParam );
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_WDS,
                                  ParamLength,
                                  eQMI_TIMEOUT_2_S,
                                  &pInParam,
                                  &ParamLength );
    /* Only parse out the response data if we got a positive return */
    if (resultcode == eQCWWAN_ERR_NONE)
    {
        /* Pass the pointer for the OUT parameters */
        response.pMipMode = pMode;

        /* Copy to the caller's buffer */
        resultcode = UpkQmiWdsGetMipMode(pInParam, &response);
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG GetMobileIPProfile(
      BYTE   index,
      BYTE  *pEnabled,
      ULONG *pAddress,
      ULONG *pPrimaryHA,
      ULONG *pSecondaryHA,
      BYTE  *pRevTunneling,
      BYTE  naiSize,
      CHAR  *pNAI,
      ULONG *pHASPI,
      ULONG *pAAASPI,
      ULONG *pHAState,
      ULONG *pAAAState)
{
        ULONG resultcode;   /* result code */
        BYTE  *pInParam;    /* ptr to param field rx'd from modem */
        BYTE  *pOutParam;   /* ptr to outbound param field */
        BYTE  *pReqBuf;     /* Pointer to outgoing request buffer */
        USHORT ParamLength; /* Ret'd length of the QMI Param field */

        /* Storage for results and response variable */
        struct QmiWdsReadMipProfileResp response;

        if ( !pEnabled      ||
             !pAddress      ||
             !pPrimaryHA    ||
             !pSecondaryHA  ||
             !pRevTunneling ||
             !pNAI          ||
             !pHASPI        ||
             !pAAASPI       ||
             !pHAState      ||
             !pAAAState )
            return eQCWWAN_ERR_INVALID_ARG;

        /* Initialize the pointer to the outgoing request buffer pointer */
        pReqBuf = qmgetreqbkp();

        /* Get a pointer to the start of the outbound QMI Parameter field */
        pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

        resultcode = PkQmiWdsReadMipProfile( &ParamLength,
                                             pOutParam,
                                             index );
        if (resultcode != eQCWWAN_ERR_NONE)
        {
            qmrelreqbkp();
            return resultcode;
        }


        /* Prepare and send the blocking call */
         resultcode = SwiQmiSendnWait( pReqBuf,
                                       eQMI_SVC_WDS,
                                       ParamLength,
                                       eQMI_TIMEOUT_2_S,/* 2 Seconds */
                                       &pInParam,
                                       &ParamLength );

        /* Only parse out the response data if we got a positive return */
        if (resultcode == eQCWWAN_ERR_NONE)
        {
            response.pEnabled       = pEnabled;
            response.pAddress       = pAddress;
            response.pPrimaryHA     = pPrimaryHA;
            response.pSecondaryHA   = pSecondaryHA;
            response.pRevTunneling  = pRevTunneling;
            response.naiSize        = naiSize;
            response.pNAI           = pNAI;
            response.pHASPI         = pHASPI;
            response.pAAASPI        = pAAASPI;
            response.pHAState       = pHAState;
            response.pAAAState      = pAAAState;

            /* Copy to the caller's buffer */
            resultcode = UpkQmiWdsReadMipProfile( pInParam, &response );
        }

        qmrelreqbkp();
        return resultcode;
}

ULONG GetLastMobileIPError(
    ULONG *pError)
{
    ULONG resultcode;   /* result code */
    BYTE  *pInParam;    /* ptr to param field rx'd from modem */
    BYTE  *pOutParam;   /* ptr to outbound param field */
    BYTE  *pReqBuf;     /* Pointer to outgoing request buffer */
    USHORT ParamLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiWdsGetLastMipStatusResp response;

    /* pError is OUT parameter and
     * hence should not be NULL */
    if ( !pError )
        return eQCWWAN_ERR_INVALID_ARG;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    resultcode = PkQmiWdsGetLastMipStatus( &ParamLength, pOutParam );
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
     resultcode = SwiQmiSendnWait( pReqBuf,
                                   eQMI_SVC_WDS,
                                   ParamLength,
                                   eQMI_TIMEOUT_2_S,/* 2 Seconds */
                                   &pInParam,
                                   &ParamLength );

    /* Only parse out the response data if we got a positive return */
    if (resultcode == eQCWWAN_ERR_NONE)
    {
        /* Pass the pointer for the OUT parameters */
        response.pError = pError;

        /* Copy to the caller's buffer */
        resultcode = UpkQmiWdsGetLastMipStatus( pInParam, &response );
    }
    qmrelreqbkp();
    return resultcode;
}

ULONG iSLQSMISetIPFamilyPreference(
    BYTE IPFamilyPreference,
    BYTE instance )
{
    /* Initialize the pointer to the outgoing request buffer pointer */
    BYTE *pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    BYTE *pOutParam;
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    USHORT ParamLength;
    ULONG rc = PkQmiWdsSLQSSetIPFamilyPreference(   &ParamLength,
                                                    pOutParam,
                                                    IPFamilyPreference );

    if ( eQCWWAN_ERR_NONE != rc )
    {
        qmrelreqbkp();
        return rc;
    }

    /* send QMI request */
    BYTE *pInParam; /* ptr to param field rx'd from modem */
    rc = SwiQmiMISendnWait( pReqBuf,
                            eQMI_SVC_WDS,
                            ParamLength,
                            eQMI_TIMEOUT_2_S, /* 2 seconds */
                            &pInParam,
                            &ParamLength,
                            instance );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == rc )
    {
        /* Storage for results and response variable */
        struct QmiWdsSLQSSetIPFamilyPreferenceResp response;
        rc = UpkQmiWdsSLQSSetIPFamilyPreference( pInParam,
                                                 &response );
    }

    qmrelreqbkp();
    return rc;
}

ULONG iSLQSGetRuntimeSettings (
    struct WdsRunTimeSettings *pRunTimeSettings )
{
    ULONG  resultcode;  /* result code */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT ParamLength; /* Ret'd length of the QMI Param field */
    ULONG  *pRequestedSettings;
    int    pdpidx = -1;

    if ( !pRunTimeSettings )
    {
         return eQCWWAN_ERR_INVALID_ARG;
    }

    if( NULL != pRunTimeSettings->v4sessionId )
    {
        if( 0 == *pRunTimeSettings->v4sessionId )
        {
            return eQCWWAN_ERR_SWICM_INVALID_V4_SESSION_ID;
        }

        pdpidx = wdsSessionManager.sid2slot(*pRunTimeSettings->v4sessionId);
        if( -1 == pdpidx )
        {
            return eQCWWAN_ERR_SWICM_INVALID_V4_SESSION_ID;
        }

        ULONG rc;
        if( !WDS_IsGobiDevice() &&
            eQCWWAN_ERR_NONE != (rc = iSLQSMISetIPFamilyPreference(IPV4, pdpidx )) )
        {
            return rc;
        }
    }
    else if( NULL != pRunTimeSettings->v6sessionId )
    {
        if( 0 == *pRunTimeSettings->v6sessionId )
        {
            return eQCWWAN_ERR_SWICM_INVALID_V6_SESSION_ID;
        }

        pdpidx = wdsSessionManager.sid2slot(*pRunTimeSettings->v6sessionId);
        if( -1 == pdpidx )
        {
            return eQCWWAN_ERR_SWICM_INVALID_V6_SESSION_ID;
        }

        ULONG rc;
        if( !WDS_IsGobiDevice() &&
            eQCWWAN_ERR_NONE != (rc = iSLQSMISetIPFamilyPreference(IPV6, pdpidx)) )
        {
            return rc;
        }
    }

    /* Set all bits to request all settings */
    ULONG settings = 0xFFFF;
    pRequestedSettings = &settings;

    /* Storage for results and response variable */
    struct QmiWdsSLQSGetRuntimeSettingsResp response;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Index points to first QMI header. */
    ParamLength = 0;
    resultcode = PkQmiWdsSLQSGetRuntimeSettings( &ParamLength,
                                                 pOutParam,
                                                 pRequestedSettings );

    if ( resultcode != eQCWWAN_ERR_NONE )
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
     resultcode = SwiQmiMISendnWait( pReqBuf,
                                   eQMI_SVC_WDS,
                                   ParamLength,
                                   eQMI_TIMEOUT_2_S,
                                   &pInParam,
                                   &ParamLength,
                                   pdpidx );

    /* Only parse out the response data if we got a positive return */
    if ( resultcode == eQCWWAN_ERR_NONE )
    {
        memcpy( (void *)&response.WdsSlqsRunTimeSettings,
                (void *)&pRunTimeSettings->rts,
                sizeof(response.WdsSlqsRunTimeSettings) );

        /* Unpack the parameters */
        resultcode = UpkQmiWdsSLQSGetRuntimeSettings( pInParam, &response );
    }

    qmrelreqbkp();
    return resultcode;
}

local void InitRunTimeSettings(
    struct qmiWdsRunTimeSettings *pRunTimeSettings,
    struct qmiWdsRunTimeSettings *pRunTimeSettingsV4,
    struct qmiWdsRunTimeSettings *pRunTimeSettingsV6 )
{
    struct UMTSQoS *pUMTSQoS;
    struct GPRSQoS *pGPRSQoS;
    BYTE           idx        = 0;

    slmemset( (char*) pRunTimeSettingsV4, 0, sizeof( struct qmiWdsRunTimeSettings ) );
    slmemset( (char*) pRunTimeSettingsV6, 0, sizeof( struct qmiWdsRunTimeSettings ) );

    /* Initialise Profile Name - IPv4 info */
    if( pRunTimeSettings->pProfileName )
    {
        *pRunTimeSettings->pProfileName    = EOS;
        pRunTimeSettingsV4->pProfileName   = pRunTimeSettings->pProfileName;
    }
    /* Initialise PDP Type - IPv4 info*/
    if( pRunTimeSettings->pPDPType )
    {
        *pRunTimeSettings->pPDPType        = ~0;
        pRunTimeSettingsV4->pPDPType       = pRunTimeSettings->pPDPType;
    }
    /* Initialise APN Name - IPv4 info*/
    if( pRunTimeSettings->pAPNName )
    {
        *pRunTimeSettings->pAPNName        = EOS;
        pRunTimeSettingsV4->pAPNName       = pRunTimeSettings->pAPNName;
    }
    /* Initialise Primary DNS V4 - IPv4 info*/
    if( pRunTimeSettings->pPrimaryDNSV4 )
    {
        *pRunTimeSettings->pPrimaryDNSV4   = ~0;
        pRunTimeSettingsV4->pPrimaryDNSV4  = pRunTimeSettings->pPrimaryDNSV4;
    }
    /* Initialise Secondary DNS V4 - IPv4 info*/
    if( pRunTimeSettings->pSecondaryDNSV4 )
    {
        *pRunTimeSettings->pSecondaryDNSV4  = ~0;
        pRunTimeSettingsV4->pSecondaryDNSV4 = pRunTimeSettings->pSecondaryDNSV4;
    }
    /* Initialise UMTS Req QoS - IPv4 info*/
    if( pRunTimeSettings->pUMTSGrantedQoS )
    {
        pUMTSQoS = pRunTimeSettings->pUMTSGrantedQoS;

        pUMTSQoS->trafficClass        = ~0;
        pUMTSQoS->maxUplinkBitrate    = ~0;
        pUMTSQoS->maxDownlinkBitrate  = ~0;
        pUMTSQoS->grntUplinkBitrate   = ~0;
        pUMTSQoS->grntDownlinkBitrate = ~0;
        pUMTSQoS->qosDeliveryOrder    = ~0;
        pUMTSQoS->maxSDUSize          = ~0;
        pUMTSQoS->sduErrorRatio       = ~0;
        pUMTSQoS->resBerRatio         = ~0;
        pUMTSQoS->deliveryErrSDU      = ~0;
        pUMTSQoS->transferDelay       = ~0;
        pUMTSQoS->trafficPriority     = ~0;

        pRunTimeSettingsV4->pUMTSGrantedQoS = pRunTimeSettings->pUMTSGrantedQoS;
    }
    /* Initialise GPRS Req QoS - IPv4 info*/
    if( pRunTimeSettings->pGPRSGrantedQoS )
    {
        pGPRSQoS = pRunTimeSettings->pGPRSGrantedQoS;

        pGPRSQoS->precedenceClass     = ~0;
        pGPRSQoS->delayClass          = ~0;
        pGPRSQoS->reliabilityClass    = ~0;
        pGPRSQoS->peakThroughputClass = ~0;
        pGPRSQoS->meanThroughputClass = ~0;

        pRunTimeSettingsV4->pGPRSGrantedQoS = pRunTimeSettings->pGPRSGrantedQoS;
    }
    /* Initialise User Name - IPv4 info*/
    if( pRunTimeSettings->pUsername )
    {
        pRunTimeSettings->pUsername   = EOS;
        pRunTimeSettingsV4->pUsername = pRunTimeSettings->pUsername;
    }
    /* Initialise Authentication - IPv4 info*/
    if( pRunTimeSettings->pAuthentication )
    {
        *pRunTimeSettings->pAuthentication  = ~0;
        pRunTimeSettingsV4->pAuthentication = pRunTimeSettings->pAuthentication;
    }
    /* Initialise IPAddress IPv4 - IPv4 info*/
    if( pRunTimeSettings->pIPAddressV4 )
    {
        *pRunTimeSettings->pIPAddressV4  = ~0;
        pRunTimeSettingsV4->pIPAddressV4 = pRunTimeSettings->pIPAddressV4;
    }
    /* Initialise ProfileID - IPv4 info*/
    if( pRunTimeSettings->pProfileID )
    {
        pRunTimeSettings->pProfileID->profileType  = ~0;
        pRunTimeSettings->pProfileID->profileIndex = ~0;

        pRunTimeSettingsV4->pProfileID = pRunTimeSettings->pProfileID;
    }

    /* Initialise IPv4 Gateway Address - IPv4 info*/
    if( pRunTimeSettings->pGWAddressV4 )
    {
        *pRunTimeSettings->pGWAddressV4  = ~0;
        pRunTimeSettingsV4->pGWAddressV4 = pRunTimeSettings->pGWAddressV4;
    }
    /* Initialise IPv4 subnet mask - IPv4 info*/
    if( pRunTimeSettings->pSubnetMaskV4 )
    {
        *pRunTimeSettings->pSubnetMaskV4  = ~0;
        pRunTimeSettingsV4->pSubnetMaskV4 = pRunTimeSettings->pSubnetMaskV4;
    }
    /* Initialise PCSCF Address using PCO Flag - IPv4 info*/
    if( pRunTimeSettings->pPCSCFAddrPCO )
    {
        *pRunTimeSettings->pPCSCFAddrPCO  = ~0;
        pRunTimeSettingsV4->pPCSCFAddrPCO = pRunTimeSettings->pPCSCFAddrPCO;
    }
    /* Initialise PCSCF Server Address List - IPv4 info*/
    if( pRunTimeSettings->pServerAddrList )
    {
        pRunTimeSettingsV4->pServerAddrList = pRunTimeSettings->pServerAddrList;
    }
    /* Initialise PCSCF FQDN List - IPv4 info */
    if( pRunTimeSettings->pPCSCFFQDNAddrList )
    {
        pRunTimeSettingsV4->pPCSCFFQDNAddrList =
            pRunTimeSettings->pPCSCFFQDNAddrList;
    }
    /* Initialise Primary DNS IPV6 - IPv6 info */
    if( pRunTimeSettings->pPrimaryDNSV6 )
    {
        for( idx = 0; idx < 8; idx++ )
        {
            pRunTimeSettings->pPrimaryDNSV6[idx] = ~0;
        }
        pRunTimeSettingsV6->pPrimaryDNSV6 = pRunTimeSettings->pPrimaryDNSV6;
    }
    /* Initialise Secondary DNS IPV6 - IPv6 info */
    if( pRunTimeSettings->pSecondaryDNSV6 )
    {
        for( idx = 0; idx < 8; idx++ )
        {
            pRunTimeSettings->pPrimaryDNSV6[idx] = ~0;
        }
        pRunTimeSettingsV6->pSecondaryDNSV6 = pRunTimeSettings->pSecondaryDNSV6;
    }
    /* Initialise Mtu - IPv4 info */
    if( pRunTimeSettings->pMtu )
    {
        *(pRunTimeSettings->pMtu) = ~0;
        pRunTimeSettingsV4->pMtu = pRunTimeSettings->pMtu;
    }
    /* Initialise Domain List - IPv4 info */
    if( pRunTimeSettings->pDomainList )
    {
        pRunTimeSettingsV4->pDomainList = pRunTimeSettings->pDomainList;
    }
    /* Initialise IP Family Preference - IPv4 info */
    if( pRunTimeSettings->pIPFamilyPreference )
    {
        *(pRunTimeSettings->pIPFamilyPreference) = ~0;
        pRunTimeSettingsV4->pIPFamilyPreference =
            pRunTimeSettings->pIPFamilyPreference;
    }
    /* Initialise IM CN flag - IPv4 info */
    if( pRunTimeSettings->pIMCNflag )
    {
        *(pRunTimeSettings->pIMCNflag) = ~0;
        pRunTimeSettingsV4->pIMCNflag = pRunTimeSettings->pIMCNflag;
    }
    /* Initialise Technology - IPv4 info */
    if( pRunTimeSettings->pTechnology )
    {
        *(pRunTimeSettings->pTechnology) = ~0;
        pRunTimeSettingsV4->pTechnology = pRunTimeSettings->pTechnology;
    }
    /* Initialise IPV6 address info - IPv6 info */
    if( pRunTimeSettings->pIPV6AddrInfo )
    {
        struct IPV6AddressInfo *pIPV6AddrInfo;
        pIPV6AddrInfo = pRunTimeSettings->pIPV6AddrInfo;
        pIPV6AddrInfo->IPV6PrefixLen = ~0;
        for( idx = 0; idx < 8; idx++ )
        {
            pIPV6AddrInfo->IPAddressV6[idx] = ~0;
        }
        pRunTimeSettingsV6->pIPV6AddrInfo = pRunTimeSettings->pIPV6AddrInfo;
    }
    /* Initialise IPV6 Gateway address info - IPv6 info */
    if( pRunTimeSettings->pIPV6GWAddrInfo )
    {
        struct IPV6GWAddressInfo *pIPV6GWAddrInfo;
        pIPV6GWAddrInfo = pRunTimeSettings->pIPV6GWAddrInfo;
        pIPV6GWAddrInfo->gwV6PrefixLen = ~0;
        for( idx = 0; idx < 8; idx++ )
        {
            pIPV6GWAddrInfo->gwAddressV6[idx] = ~0;
        }
        pRunTimeSettingsV6->pIPV6GWAddrInfo = pRunTimeSettings->pIPV6GWAddrInfo;
    }
}

ULONG SLQSGetRuntimeSettings (
    struct WdsRunTimeSettings *pRunTimeSettings )
{
    if( NULL == pRunTimeSettings->v4sessionId &&
        NULL == pRunTimeSettings->v6sessionId )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* set i/p parameters to default values and initialize response structures */
    struct WdsRunTimeSettings RunTimeSettingsV4;
    struct WdsRunTimeSettings RunTimeSettingsV6;

    InitRunTimeSettings( &pRunTimeSettings->rts,
                         &RunTimeSettingsV4.rts,
                         &RunTimeSettingsV6.rts );

    int iptype = 0;

    enum eQCWWANError rcv4 = eQCWWAN_ERR_NONE;
    if( NULL != pRunTimeSettings->v4sessionId )
    {
        iptype = IPV4;

        /* Get Runtime settings related to IPv4 */
        RunTimeSettingsV4.v4sessionId = pRunTimeSettings->v4sessionId;
        RunTimeSettingsV4.v6sessionId = NULL;
        rcv4 = iSLQSGetRuntimeSettings( &RunTimeSettingsV4 );
    }

    enum eQCWWANError rcv6 = eQCWWAN_ERR_NONE;
    if( NULL != pRunTimeSettings->v6sessionId )
    {
        iptype = IPV4 == iptype ? IPV4V6 : IPV6;

        /* Get Runtime settings related to IPv6 */
        RunTimeSettingsV6.v6sessionId = pRunTimeSettings->v6sessionId;
        RunTimeSettingsV6.v4sessionId = NULL;
        rcv6 = iSLQSGetRuntimeSettings( &RunTimeSettingsV6 );
    }

    switch( iptype )
    {
        case IPV4:
            return rcv4;
            break;

        case IPV6:
            return rcv6;
            break;

        case IPV4V6:
            /* set the family preference to IPV4V6; error logic handled below */
            if( NULL != pRunTimeSettings->rts.pIPFamilyPreference )
            {
                *pRunTimeSettings->rts.pIPFamilyPreference = IPV4V6;
            }
            break;
    }

    /* IPV4V6 - Determine return codes to pass back to the caller */
    if( eQCWWAN_ERR_NONE == rcv4 &&
        eQCWWAN_ERR_NONE == rcv6 )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* v6 settings available */
    if( eQCWWAN_ERR_NONE == rcv6 )
    {
        if( eQCWWAN_ERR_QMI_OUT_OF_CALL == rcv4 )
        {
            /* v4 connection down */
            return eQCWWAN_ERR_SWICM_V4DWN_V6UP;
        }
        else
        {
            /* Error retrieving v4 settings */
            return rcv4;
        }
    }

    /* v4 settings available */
    if( eQCWWAN_ERR_NONE == rcv4 )
    {
        if( eQCWWAN_ERR_QMI_OUT_OF_CALL == rcv6 )
        {
            /* v6 connection down */
            return eQCWWAN_ERR_SWICM_V4UP_V6DWN;
        }
        else
        {
            /* Error retrieving v6 settings */
            return rcv6;
        }
    }

    /* v4 connection down */
    if( eQCWWAN_ERR_QMI_OUT_OF_CALL == rcv4 )
    {
        /* v6 connection down */
        if( eQCWWAN_ERR_QMI_OUT_OF_CALL == rcv6 )
        {
            return eQCWWAN_ERR_QMI_OUT_OF_CALL;
        }
        else
        {
            /* Error retrieving v6 settings */
            return rcv6;
        }
    }

    /* v6 connection down */
    if( eQCWWAN_ERR_QMI_OUT_OF_CALL == rcv6 )
    {
        /* v4 connection down */
        if( eQCWWAN_ERR_QMI_OUT_OF_CALL == rcv4 )
        {
            return eQCWWAN_ERR_QMI_OUT_OF_CALL;
        }
        else
        {
            /* Error retrieving v4 settings */
            return rcv4;
        }
    }

    /* Error retrieving both IP addresses & both sessions appear to be up.
     * Return first point of failure
     */
    return rcv4;
}

ULONG SLQSSetProfile(
    ULONG   profileType,
    BYTE    profileId,
    ULONG   *pPDPType,
    ULONG   *pIPAddress,
    ULONG   *pPrimaryDNS,
    ULONG   *pSecondaryDNS,
    ULONG   *pAuthentication,
    CHAR    *pName,
    CHAR    *pAPNName,
    CHAR    *pUsername,
    CHAR    *pPassword )
{
    ULONG  resultcode;            /* result code */
    BYTE   *pInParam;              /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;             /* ptr to outbound param field */
    BYTE   *pReqBuf;               /* Pointer to outgoing request buffer */
    USHORT ParamLength;           /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiWdsModifyProfileResp response;

    /* profileId should be between 1 - 16 */
    if( MIN_PROFILE_ID > profileId || MAX_PROFILE_ID < profileId )
        return eQCWWAN_ERR_INVALID_ARG;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Index points to first QMI header. */
    ParamLength = 0;

    /* pack QMI request */
    resultcode = PkQmiWdsModifyProfile( &ParamLength,
                                        pOutParam,
                                        profileType,
                                        profileId,
                                        pPDPType,
                                        pIPAddress,
                                        pPrimaryDNS,
                                        pSecondaryDNS,
                                        NULL,
                                        NULL,
                                        NULL,
                                        pAuthentication,
                                        pName,
                                        pAPNName,
                                        pUsername,
                                        pPassword );
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* send QMI request */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_WDS,
                                  ParamLength,
                                  eQMI_TIMEOUT_2_S,
                                  &pInParam,
                                  &ParamLength );

    if (resultcode == eQCWWAN_ERR_NONE)
        resultcode = UpkQmiWdsModifyProfile(pInParam, &response);

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSGetProfile(
    ULONG profileType,
    BYTE  profileId,
    ULONG *pPDPType,
    ULONG *pIPAddress,
    ULONG *pPrimaryDNS,
    ULONG *pSecondaryDNS,
    ULONG *pAuthentication,
    BYTE  nameSize,
    CHAR  *pName,
    BYTE  apnSize,
    CHAR  *pAPNName,
    BYTE  userSize,
    CHAR  *pUsername,
    WORD  *pExtendedErrorCode )
{
    ULONG  resultcode;  /* result code */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT ParamLength; /* Ret'd length of the QMI Param field */
    WORD   passSize = 0; /* Set to 0 for compatibility */
    BYTE   lPDPType;    /* To retrieve PDP Type - for compatibility */
    BYTE   lAuth;       /* To retrieve Authentication - for compatibility */

    /* Storage for results and response variable */
    struct QmiWdsSlqsGetProfileSettingsResp response;
    struct Profile3GPP                      *pProfile3GPP;
    GetProfileSettingIn                     ProfileSettingReqParams;
    GetProfileSettingOut                    ProfileSettings;

    /* profileId should be between 1 - 16 */
    if( MIN_PROFILE_ID > profileId || MAX_PROFILE_ID < profileId )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Check against NULL for out params */
    if ( !pPDPType          ||
         !pIPAddress        ||
         !pPrimaryDNS       ||
         !pSecondaryDNS     ||
         !pAuthentication   ||
         !pName             ||
         !pAPNName          ||
         !pUsername )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Initialize request structure */
    ProfileSettingReqParams.ProfileID = profileId;
    ProfileSettingReqParams.ProfileType = profileType;

    /* pack QMI request */
    resultcode = PkQmiWdsSlqsGetProfileSettings( &ParamLength,
                                                 pOutParam,
                                                 &ProfileSettingReqParams );

    if ( eQCWWAN_ERR_NONE != resultcode )
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* send QMI request */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_WDS,
                                  ParamLength,
                                  eQMI_TIMEOUT_2_S, /* 2 seconds */
                                  &pInParam,
                                  &ParamLength );

    if ( eQCWWAN_ERR_NONE == resultcode )
    {
        slmemset( (char *)&ProfileSettings,
                  0,
                  sizeof(GetProfileSettingOut) );

        /* initialize response structure */
        response.pProfileSettings = &ProfileSettings;

        pProfile3GPP = &(ProfileSettings.curProfile.SlqsProfile3GPP);

        pProfile3GPP->pPDPtype            = &lPDPType;
        pProfile3GPP->pIPv4AddrPref       = pIPAddress;
        pProfile3GPP->pPriDNSIPv4AddPref  = pPrimaryDNS;
        pProfile3GPP->pSecDNSIPv4AddPref  = pSecondaryDNS;
        pProfile3GPP->pAuthenticationPref = &lAuth;
        pProfile3GPP->pProfilename        = pName;
        pProfile3GPP->pAPNName            = pAPNName;
        pProfile3GPP->pUsername           = pUsername;
        pProfile3GPP->pProfilenameSize    = (WORD*) &nameSize;
        pProfile3GPP->pAPNnameSize        = (WORD*) &apnSize;
        pProfile3GPP->pUsernameSize       = (WORD*) &userSize;

        /* Set password size to zero as it isn't returned to user
         * This is done to make it compatible with SLQSGetProfileSettings()
         */
        pProfile3GPP->pPasswordSize       = &passSize;
        ProfileSettings.pExtErrCode     = pExtendedErrorCode;

        /* Copy to the caller's buffer */
        resultcode = UpkQmiWdsSlqsGetProfileSettings( pInParam,
                                                      (BYTE*) &profileType,
                                                      &response );

        /* The below code has been added to maintain compatibility between
         * existing API interface to the user and the current unpacking
         * routines for QMI_WDS_GET_PROFILE_SETTINGS
         */
        if ( eQCWWAN_ERR_NONE == resultcode )
        {
            *pPDPType        = lPDPType;
            *pAuthentication = lAuth;
        }
    }

    qmrelreqbkp();
    return resultcode;
}

/* APIs Not Implemented */
ULONG SetActiveMobileIPProfile(
    CHAR *pSPC,
    BYTE index )
{
    UNUSEDPARAM( pSPC );
    UNUSEDPARAM( index );

    return eQCWWAN_ERR_SWICM_NOT_IMPLEMENTED;
}

ULONG GetActiveMobileIPProfile(
    BYTE *pIndex )
{
    UNUSEDPARAM( pIndex );

    return eQCWWAN_ERR_SWICM_NOT_IMPLEMENTED;
}

ULONG GetMobileIPProfile2(
    BYTE  index,
    BYTE  *pEnabled,
    ULONG *pAddress,
    ULONG *pPrimaryHA,
    ULONG *pSecondaryHA,
    BYTE  *pRevTunneling,
    BYTE  naiSize,
    CHAR  *pNAI,
    ULONG *pHASPI,
    ULONG *pAAASPI,
    ULONG *pHAState,
    ULONG *pAAAState )
{
    UNUSEDPARAM( index );
    UNUSEDPARAM( pEnabled );
    UNUSEDPARAM( pAddress );
    UNUSEDPARAM( pPrimaryHA );
    UNUSEDPARAM( pSecondaryHA );
    UNUSEDPARAM( pRevTunneling );
    UNUSEDPARAM( naiSize );
    UNUSEDPARAM( pNAI );
    UNUSEDPARAM( pHASPI );
    UNUSEDPARAM( pAAASPI );
    UNUSEDPARAM( pHAState );
    UNUSEDPARAM( pAAAState );

    return eQCWWAN_ERR_SWICM_NOT_IMPLEMENTED;
}

ULONG GetMobileIPParameters(
   ULONG *pMode,
   BYTE  *pRetryLimit,
   BYTE  *pRetryInterval,
   BYTE  *pReRegPeriod,
   BYTE  *pReRegTraffic,
   BYTE  *pHAAuthenticator,
   BYTE  *pHA2002bis )
{
    UNUSEDPARAM( pMode );
    UNUSEDPARAM( pRetryLimit );
    UNUSEDPARAM( pRetryInterval );
    UNUSEDPARAM( pReRegPeriod );
    UNUSEDPARAM( pReRegTraffic );
    UNUSEDPARAM( pHAAuthenticator );
    UNUSEDPARAM( pHA2002bis );

    return eQCWWAN_ERR_SWICM_NOT_IMPLEMENTED;
}

local ULONG iSLQSStartStopDataSession(
    struct ssdatasession_params *pSessionParams,
    BYTE instance )
{
    ULONG   rc;          /* result code */
    BYTE    *pInParam;   /* ptr to param field rx'd from modem */
    BYTE    *pOutParam;  /* ptr to outbound param field */
    BYTE    *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT  ParamLength; /* Ret'd length of the QMI Param field */
    ULONG   timeout;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Start Data Session */
    if( START_DATA_SESSION == pSessionParams->action )
    {
        /* Invoke the function which packs QMI message */
        rc = PkQmiWdsStartNetworkInterface2( &ParamLength,
                                             pOutParam,
                                             pSessionParams->pTechnology,
                                             pSessionParams->pProfileId3GPP,
                                             pSessionParams->pProfileId3GPP2,
                                             pSessionParams->pAuthentication,
                                             pSessionParams->pUsername,
                                             pSessionParams->pPassword );

        timeout = eQMI_TIMEOUT_300_S; /* 5 minutes */
    }
    else /* Stop data session */
    {
        rc = PkQmiWdsStopNetworkInterface( &ParamLength,
                                           pOutParam,
                                           pSessionParams->sessionId,
                                           eWDS_STOP_NETWORK_INTERFACE_AUTOCONNECT_SETTING_UNCHANGED );

        timeout = eQMI_TIMEOUT_60_S; /* 1 minute */
    }

    if ( eQCWWAN_ERR_NONE != rc )
    {
        qmrelreqbkp();
        return rc;
    }

    /* send QMI request */
    rc = SwiQmiMISendnWait( pReqBuf,
                            eQMI_SVC_WDS,
                            ParamLength,
                            timeout,
                            &pInParam,
                            &ParamLength,
                            instance );

    if ( eQCWWAN_ERR_NONE == rc )
    {
        if( START_DATA_SESSION == pSessionParams->action )
        {
            struct QmiWdsStartNetworkInterfaceResp response;
            response.pPktDataHandle = &pSessionParams->sessionId;
            response.pFailureReason = &pSessionParams->failureReason;
            response.pVerboseFailReasonType =
                &pSessionParams->verbFailReasonType;
            response.pVerboseFailureReason = &pSessionParams->verbFailReason;

            /* Copy to the caller's buffer */
            rc = UpkQmiWdsStartNetworkInterface2( pInParam, &response );
#ifdef DBG_WDS
            syslog( LOG_DEBUG,
                    "%s/%d: WDS start interface verbose error: %lu\n",
                    __func__,
                    __LINE__,
                    *response.pVerboseFailureReason );
#endif
        }
        else
        {
            struct QmiWdsStopNetworkInterfaceResp response;
            rc = UpkQmiWdsStopNetworkInterface( pInParam, &response );
        }
    }

    qmrelreqbkp();
    return rc;
}

enum eQCWWANError iSLQSStartDataSessionV4(
    struct qaWdsPDPSessionData *pd,
    struct ssdatasession_params *ps,
    BOOL *pSessionActive )
{
    enum eQCWWANError rc;
    *pSessionActive = FALSE;

    /* select v4 ip family */
    if( !WDS_IsGobiDevice() &&
        eQCWWAN_ERR_NONE != (rc = iSLQSMISetIPFamilyPreference(IPV4, pd->pdpIdx) ) )
    {
        /* set v4 error code */
        ps->rcv4 = rc;
        /* clear failure reason */
        ps->failureReasonv4 = 0;
#ifdef DBG_WDS
        wdsSessionDataLog(pd, ps, &rc);
#endif
        return rc;
    }

    /* start IPv4 session */
    if( eQCWWAN_ERR_NONE !=
        (rc = iSLQSStartStopDataSession(ps, pd->pdpIdx) ) )
    {
        /* store v4 failure reason */
        ps->failureReasonv4 = ps->failureReason;

        if( eQCWWAN_ERR_QMI_NO_EFFECT == rc )
        {
            /* if starting a session has no effect assume the session is active */
            ps->sessionId = ps->v4sessionId = pd->v4sessionId;
            *pSessionActive = TRUE;
        }
    }
    else /* IPv4 successful */
    {
        /* mark session as active */
        *pSessionActive = TRUE;
        /* clear failures */
        ps->failureReason = ps->failureReasonv4 = 0;
        /* store the IPv4 Session Handle */
        ps->v4sessionId = ps->sessionId;
        pd->v4sessionId = ps->sessionId;
    }
#ifdef DBG_WDS
    wdsSessionDataLog(pd, ps, &rc);
#endif
    /* set v4 error code */
    ps->rcv4 = rc;

    return rc;
}

enum eQCWWANError iSLQSStartDataSessionV6(
    struct qaWdsPDPSessionData *pd,
    struct ssdatasession_params *ps,
    BOOL *pSessionActive )
{
    enum eQCWWANError rc;
#ifdef DBG_WDS
    wdsSessionDataLog(pd, ps, NULL);
#endif
    *pSessionActive = FALSE;

    /* select v6 ip family */
    if( eQCWWAN_ERR_NONE !=
        (rc = iSLQSMISetIPFamilyPreference(IPV6, pd->pdpIdx) ) )
    {
#ifdef DBG_WDS
    wdsSessionDataLog(pd, NULL, &rc);
#endif
        return rc;
    }

    /* start IPv6 session */
    if( eQCWWAN_ERR_NONE !=
        (rc = iSLQSStartStopDataSession(ps, pd->pdpIdx) ) )
    {
        ps->failureReasonv6 = ps->failureReason; /* store v6 failure reason */

        if( eQCWWAN_ERR_QMI_NO_EFFECT == rc )
        {
            /* if starting a session has no effect assume the session is active */
            ps->sessionId = ps->v6sessionId = pd->v6sessionId;
            *pSessionActive = TRUE;
        }
    }
    else /* IPv6 successful */
    {
        /* mark session as active */
        *pSessionActive = TRUE;
        /* clear failures */
        ps->failureReason = ps->failureReasonv6 = 0;
        /* store the IPv6 Session Handle */
        ps->v6sessionId = ps->sessionId;
        pd->v6sessionId = ps->sessionId;
    }
#ifdef DBG_WDS
    wdsSessionDataLog(pd, ps, &rc);
#endif
    ps->rcv6 = rc;
    return rc;
}

local enum eQCWWANError iSLQSStartDataSession(
    struct ssdatasession_params *ps )
{
    /* active session flags */
    BOOL v4SA = FALSE;
    BOOL v6SA = FALSE;
    enum eQCWWANError rc;

    /* lock critical section */
    struct wdsSessionMgr *pd = &wdsSessionManager;
    pd->smlock();
#ifdef DBG_WDS
    pd->print(ps, NULL);
#endif

    /* Get InstanceId for which session to be started */
    int idx = ps->instanceId;

    if( -1 == idx )
    {
        ps->sessionId = 0;

        if( IPV4 == ps->ipfamily || IPV4V6 == ps->ipfamily )
        {
            ps->rcv4 = eQCWWAN_ERR_SWICM_SM_NO_AVAILABLE_SESSIONS;
        }
        if( IPV6 == ps->ipfamily || IPV4V6 == ps->ipfamily )
        {
            ps->rcv6 = eQCWWAN_ERR_SWICM_SM_NO_AVAILABLE_SESSIONS;
        }

        /* unlock critical section */
        pd->smunlock();

        return eQCWWAN_ERR_SWICM_SM_NO_AVAILABLE_SESSIONS;
    }

    if( IPV4V6 == ps->ipfamily || IPV4  == ps->ipfamily )
    {
        /*----------------*/
        /*-- V4 Handler --*/
        /*----------------*/
        rc = iSLQSStartDataSessionV4( &pd->pdp[idx],
                                      ps,
                                      &v4SA );
        if ( eQCWWAN_ERR_NONE != rc )
        {
            /* unlock critical section */
            pd->smunlock();
            return rc;
        }
    }

    if( IPV4V6 == ps->ipfamily || IPV6  == ps->ipfamily )
    {
        /*----------------*/
        /*-- V6 Handler --*/
        /*----------------*/
        rc = iSLQSStartDataSessionV6(  &pd->pdp[idx],
                                       ps,
                                       &v6SA);
        if ( eQCWWAN_ERR_NONE != rc )
        {
            /* unlock critical section */
            pd->smunlock();
            return rc;
        }
    }

    /* unlock critical section */
    pd->smunlock();

#ifdef DBG_WDS
    pd->print(ps, NULL);
#endif
    switch( ps->ipfamily )
    {
        case IPV4:
            return ps->rcv4;

        case IPV6:
            return ps->rcv6;

        case IPV4V6:
            /* Handle IPV4V6 logic below switch */
            break;
        default:
            break;
    }

    /* IPV4V6 - Determine return codes to pass back to the caller */
    if( v4SA && v6SA ) /* v4 and v6 up */
    {
        rc = eQCWWAN_ERR_SWICM_V4UP_V6UP;
    }

    else if(v4SA) /* v4 up, v6 failed */
    {
        rc = eQCWWAN_ERR_SWICM_V4UP_V6DWN;
    }
    else if(v6SA) /* v4 fail, v6 up */
    {
        rc = eQCWWAN_ERR_SWICM_V4DWN_V6UP;
    }
    else /* v4 fail, v6 fail */
    {
        /* return first point of failure */
        rc = eQCWWAN_ERR_SWICM_V4DWN_V6DWN;
    }
    return rc;
}

local enum eQCWWANError iSLQSStopDataSessionV4(
    struct qaWdsPDPSessionData *pd,
    struct ssdatasession_params *ps,
    BOOL *pSessionInactive )
{
    enum eQCWWANError rc;
    *pSessionInactive = FALSE;

    if( pd->v4sessionId )
    {
        /* select v4 ip family */
        if( !WDS_IsGobiDevice() &&
            eQCWWAN_ERR_NONE != (rc = iSLQSMISetIPFamilyPreference(IPV4,pd->pdpIdx)) )
        {
            /* set v4 error code */
            ps->rcv4 = rc;
            /* clear failure reason */
            ps->failureReasonv4 = 0;
#ifdef DBG_WDS
    wdsSessionDataLog(pd, ps, &rc);
#endif
            return rc;
        }

        /* cache caller's session id */
        const ULONG tempsid = ps->sessionId;

        /* stop session */
        ps->sessionId = pd->v4sessionId;
        if( eQCWWAN_ERR_NONE !=
            (rc = iSLQSStartStopDataSession(ps, pd->pdpIdx)) )
        {
            /* store v4 failure reason */
            ps->failureReasonv4 = ps->failureReason;

            if( eQCWWAN_ERR_QMI_NO_EFFECT != rc )
            {
                /* restore caller's session id */
                ps->sessionId = tempsid;
            }
            else
            {
                /* if stopping a session has no effect, assume the session is
                 * inactive.
                 */
                *pSessionInactive = TRUE;
            }
        }
        else
        {
            /* mark session as inactive */
            *pSessionInactive = TRUE;
        }
    }
    else
    {
        /* mark session as inactive */
        *pSessionInactive = TRUE;
        rc = eQCWWAN_ERR_QMI_NO_EFFECT;
    }
    if( *pSessionInactive )
    {
        /* clear caller's session id */
        ps->sessionId = 0;
        /* clear caller's IPv4 Session id */
        ps->v4sessionId = 0;
        /* clear session manager IPv4 Session id */
        pd->v4sessionId = 0;
    }

    /* set v4 error code */
    ps->rcv4 = rc;
    return rc;
}

local enum eQCWWANError iSLQSStopDataSessionV6(
    struct qaWdsPDPSessionData *pd,
    struct ssdatasession_params *ps,
    BOOL *pSessionInactive )
{
    enum eQCWWANError rc;
    *pSessionInactive = FALSE;

    if( pd->v6sessionId )
    {
        /* select v6 ip family */
        if( eQCWWAN_ERR_NONE !=
            (rc = iSLQSMISetIPFamilyPreference(IPV6,pd->pdpIdx)) )
        {
            /* set v6 error code */
            ps->rcv6 = rc;
            /* clear failure reason */
            ps->failureReasonv6 = 0;
#ifdef DBG_WDS
    wdsSessionDataLog(pd, ps, &rc);
#endif
            return rc;
        }

        /* cache caller's session id */
        const ULONG tempsid = ps->sessionId;

        /* stop session */
        ps->sessionId = pd->v6sessionId;
        if( eQCWWAN_ERR_NONE !=
            (rc = iSLQSStartStopDataSession(ps, pd->pdpIdx)) )
        {
            /* store v6 failure reason */
            ps->failureReasonv6 = ps->failureReason;

            if( eQCWWAN_ERR_QMI_NO_EFFECT != rc )
            {
                /* restore caller's session id */
                ps->sessionId = tempsid;
            }
            else
            {
                /* if stopping a session has no effect, assume the session is
                 * inactive.
                 */
                *pSessionInactive = TRUE;
            }
        }
        else
        {
            /* mark session as inactive */
            *pSessionInactive = TRUE;
        }
    }
    else
    {
        /* mark session as inactive */
        *pSessionInactive = TRUE;
        rc = eQCWWAN_ERR_QMI_NO_EFFECT;
    }
    if( *pSessionInactive )
    {
        /* clear caller's session id */
        ps->sessionId = 0;
        /* clear caller's IPv6 Session id */
        ps->v6sessionId = 0;
        /* clear session manager IPv6 Session id */
        pd->v6sessionId = 0;
    }

    /* set v6 error code */
    ps->rcv6 = rc;
    return rc;
}

local enum eQCWWANError iSLQSStopDataSession(
    struct ssdatasession_params *ps )
{
    enum eQCWWANError rc;

    /* inactive session flags */
    BOOL v4SI = FALSE;
    BOOL v6SI = FALSE;
    BYTE iptype = 0;

    /* lock critical section */
    wdsSessionManager.smlock();
#ifdef DBG_WDS
    wdsSessionManager.print(ps, NULL);
#endif
    if( ps->v4sessionId != 0 )
    {
        /* determine PDP instance by matching the session handle */
        int pdpidx = wdsSessionManager.sid2slot(ps->v4sessionId);

        if( -1 != pdpidx )
        {
            /* note that a valid v4 session id was provided */
            iptype = IPV4;

            /*----------------*/
            /*-- V4 Handler --*/
            /*----------------*/
            rc = iSLQSStopDataSessionV4( &wdsSessionManager.pdp[pdpidx],
                                    ps,
                                    &v4SI );

#ifdef DBG_WDS
            syslog( LOG_DEBUG,
                    "%s/%d: iSLQSStopDataSessionV4 returns %d\n",
                    __func__,
                    __LINE__,
                    rc );
#endif
        }
        else
        {
            ps->rcv4 = eQCWWAN_ERR_SWICM_INVALID_V4_SESSION_ID;
            ps->rcv6 = eQCWWAN_ERR_NONE;
        }
    }

    if( ps->v6sessionId != 0 )
    {

        /* determine PDP instance by matching the session handle */
        int pdpidx = wdsSessionManager.sid2slot(ps->v6sessionId);

        if( -1 != pdpidx )
        {
            /* note that a valid v6 session id was provided */
            iptype = IPV4 == iptype ? IPV4V6 : IPV6;

            /*----------------*/
            /*-- V6 Handler --*/
            /*----------------*/
            rc = iSLQSStopDataSessionV6(  &wdsSessionManager.pdp[pdpidx],
                                     ps,
                                     &v6SI );
#ifdef DBG_WDS
            syslog( LOG_DEBUG,
                    "%s/%d: iSLQSStopDataSessionV6 returns %d\n",
                    __func__,
                    __LINE__,
                    rc );
#endif
        }
        else
        {
            ps->rcv6 = eQCWWAN_ERR_SWICM_INVALID_V6_SESSION_ID;
            ps->rcv4 = eQCWWAN_ERR_NONE;
        }
    }
#ifdef DBG_WDS
    wdsSessionManager.print(ps, NULL);
#endif
    /* unlock critical section */
    wdsSessionManager.smunlock();

    switch( iptype )
    {
        case 0:
            return eQCWWAN_ERR_SWICM_INVALID_SESSION_ID;
            break;

        case IPV4:
            return ps->rcv4;
            break;

        case IPV6:
            return ps->rcv6;
            break;

        case IPV4V6:
            /* Handle IPV4V6 logic below switch */
            break;
        default:
            break;
    }

    /* IPV4V6 - Determine return codes to pass back to the caller */
    if( v4SI && v6SI ) /* v4 and v6 down */
    {
        rc = eQCWWAN_ERR_NONE;
    }
    else if(v4SI) /* v4 down, v6 up */
    {
        rc = eQCWWAN_ERR_SWICM_V4DWN_V6UP;
    }
    else if(v6SI) /* v4 up, v6 down */
    {
        rc = eQCWWAN_ERR_SWICM_V4UP_V6DWN;
    }
    else /* v4 up, v6 up */
    {
        rc = eQCWWAN_ERR_SWICM_V4UP_V6UP;
    }
    return rc;
}

ULONG SLQSStartStopDataSession(
    struct ssdatasession_params *pSSDataSession )
{
    ULONG rc;  /* result code */

    /* input parameter validation - start */
    if ( NULL == pSSDataSession )
    {
         return eQCWWAN_ERR_INVALID_ARG;
    }

    if( START_DATA_SESSION != pSSDataSession->action &&
        STOP_DATA_SESSION  != pSSDataSession->action )
    {
         return eQCWWAN_ERR_INVALID_ARG;
    }

    /* perform requested action */
    switch(pSSDataSession->action)
    {
        case START_DATA_SESSION:
            rc = iSLQSStartDataSession(pSSDataSession);
            break;

        case STOP_DATA_SESSION:
            rc = iSLQSStopDataSession(pSSDataSession);
            break;

        default:
            break;
    }
    return rc;
}

ULONG SLQSDeleteProfile(
    struct SLQSDeleteProfileParams *pProfileToDelete,
    WORD                           *pExtendedErrorCode )
{
    ULONG  resultCode;  /* result code */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT ParamLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiWdsSlqsDeleteProfileResp response;

    /* validate input parameters  */
    if( !pProfileToDelete )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* pack QMI request */
    resultCode = PkQmiWdsSlqsDeleteProfile( &ParamLength,
                                            pOutParam,
                                            pProfileToDelete->profileType,
                                            pProfileToDelete->profileIndex );

    if ( eQCWWAN_ERR_NONE != resultCode )
    {
        qmrelreqbkp();
        return resultCode;
    }

    /* send QMI request */
    resultCode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_WDS,
                                  ParamLength,
                                  eQMI_TIMEOUT_2_S, /* 2 seconds */
                                  &pInParam,
                                  &ParamLength );

    if ( eQCWWAN_ERR_NONE == resultCode )
    {
        /* Copy to the caller's buffer */
        response.pExtendedErrorCode = pExtendedErrorCode;
        resultCode = UpkQmiWdsSlqsDeleteProfile( pInParam, &response );
    }

    qmrelreqbkp();
    return resultCode;
}

ULONG SLQSCreateProfile(
    struct CreateProfileIn  *pCreateProfileIn,
    struct CreateProfileOut *pCreateProfileOut )
{
    ULONG  resultcode;  /* result code */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT ParamLength; /* Ret'd length of the CnS Param field */

    /* NULL check of IN and OUT Parameters */
    if ( !pCreateProfileIn ||
         !pCreateProfileOut )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Storage for results and response variable */
    struct QmiWdsSLQSCreateProfileResp response;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Index points to first QMI header. */
    ParamLength = 0;

    /* pack QMI request */
    resultcode = PkQmiWdsSLQSCreateProfile(
                     &ParamLength,
                     pOutParam,
                     pCreateProfileIn );

    if ( eQCWWAN_ERR_NONE != resultcode )
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_WDS,
                                  ParamLength,
                                  eQMI_TIMEOUT_2_S,
                                  &pInParam,
                                  &ParamLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultcode )
    {
        response.pSlqsCreateProfileOut = pCreateProfileOut;

        /* Unpack the parameters */
        resultcode = UpkQmiWdsSLQSCreateProfile( pInParam,
                                                 &response,
                                                 pCreateProfileIn->pProfileID );
    }
    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSAutoConnect( struct slqsautoconnect *pAcreq )
{
    ULONG  resultcode;  /* result code */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT ParamLength; /* Ret'd length of the Param field */
    ULONG  timeout;

    /* NULL check for OUT Parameters */
    if ( !pAcreq )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* validate action value passed by the user */
    if( ( SET_AUTO_CONNECT != pAcreq->action ) &&
        ( GET_AUTO_CONNECT != pAcreq->action ) )
    {
         return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Index points to first QMI header. */
    ParamLength = 0;

    /* Set Autoconnect */
    if( SET_AUTO_CONNECT == pAcreq->action )
    {
        /* pack QMI request */
        resultcode = PkQmiWdsSLQSSetAutoConnect( &ParamLength,
                                                 pOutParam,
                                                 pAcreq->acsetting,
                                                 pAcreq->acroamsetting );

        timeout = eQMI_TIMEOUT_300_S; /* 5 minutes */
    }
    else
    {
        /* pack QMI request */
        resultcode = PkQmiWdsSLQSGetAutoConnect( &ParamLength,
                                                 pOutParam );

        timeout = eQMI_TIMEOUT_2_S; /* 2 second */
    }

    if ( eQCWWAN_ERR_NONE != resultcode )
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_WDS,
                                  ParamLength,
                                  timeout,
                                  &pInParam,
                                  &ParamLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultcode )
    {
        if( SET_AUTO_CONNECT == pAcreq->action )
        {
            /* Unpack the parameters */
            struct QmiWdsSLQSSetAutoConnectResp response;
            resultcode = UpkQmiWdsSLQSSetAutoConnect( pInParam, &response );
        }
        else
        {
            struct QmiWdsSLQSGetAutoConnectResp response;
            response.slqsautoconnectResp.acroamsetting = 0;

            /* Unpack the parameters */
            resultcode = UpkQmiWdsSLQSGetAutoConnect( pInParam, &response );

            /* Assign fectched parameters to structure */
            pAcreq->acsetting     = response.slqsautoconnectResp.acsetting;
            pAcreq->acroamsetting = response.slqsautoconnectResp.acroamsetting;
        }
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSModifyProfile(
    struct ModifyProfileIn  *pModifyProfileIn,
    struct ModifyProfileOut *pModifyProfileOut )
{
    ULONG  resultcode;  /* Result of SwiQmiSendnWait() */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT ParamLength; /* Ret'd length of the CnS Param field */

    /* NULL check of IN and OUT Parameters */
    if ( !pModifyProfileIn ||
         !pModifyProfileIn->pProfileID ||
         !pModifyProfileIn->pProfileType ||
         !pModifyProfileOut )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Storage for results and response variable */
    struct QmiWdsSLQSModifyProfileResp response;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Index points to first QMI header. */
    ParamLength = 0;

    /* pack QMI request */
    resultcode = PkQmiWdsSLQSModifyProfile( &ParamLength,
                                            pOutParam,
                                            pModifyProfileIn );

    if ( eQCWWAN_ERR_NONE != resultcode )
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_WDS,
                                  ParamLength,
                                  eQMI_TIMEOUT_2_S,
                                  &pInParam,
                                  &ParamLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultcode )
    {
        response.pSlqsModifyProfileOut = pModifyProfileOut;

        /* Unpack the parameters */
        resultcode = UpkQmiWdsSLQSModifyProfile( pInParam,
                                                 &response );
    }
    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSSet3GPPConfigItem( slqs3GPPConfigItem *pSLQS3GPPConfigItem )
{
    ULONG  resultcode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiWdsSLQSSet3GPPConfigItemResp response;

    /* Check against NULL for in params */
    if ( !pSLQS3GPPConfigItem )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Atleast one IN param should not be NULL */
    if( !pSLQS3GPPConfigItem->pLTEAttachProfile  &&
        !pSLQS3GPPConfigItem->pProfileList       &&
        !pSLQS3GPPConfigItem->pDefaultPDNEnabled &&
        !pSLQS3GPPConfigItem->p3gppRelease )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* pack QMI request */
    resultcode = PkQmiWdsSLQSSet3GPPConfigItem( &paramLength,
                                                pOutParam,
                                                pSLQS3GPPConfigItem );

    if ( eQCWWAN_ERR_NONE != resultcode )
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* send QMI request and receive the response */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_WDS,
                                  paramLength,
                                  eQMI_TIMEOUT_2_S, /* 2 seconds */
                                  &pInParam,
                                  &paramLength );

    if ( eQCWWAN_ERR_NONE == resultcode )
    {
        /* Copy to the caller's buffer */
        resultcode = UpkQmiWdsSLQSSet3GPPConfigItem( pInParam, &response );
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSGet3GPPConfigItem( slqs3GPPConfigItem *pSLQS3GPPConfigItem )
{
    ULONG  resultcode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */
    BYTE   counter = 0; /* counter for loop used to initialize profileList */

    /* Storage for results and response variable */
    struct QmiWdsSLQSGet3GPPConfigItemResp response;

    /* Check against NULL for out params */
    if ( !pSLQS3GPPConfigItem )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Atleast one OUT param should not be NULL */
    if( !pSLQS3GPPConfigItem->pLTEAttachProfile  &&
        !pSLQS3GPPConfigItem->pProfileList       &&
        !pSLQS3GPPConfigItem->pDefaultPDNEnabled &&
        !pSLQS3GPPConfigItem->p3gppRelease )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* pack QMI request */
    resultcode = PkQmiWdsSLQSGet3GPPConfigItem( &paramLength, pOutParam );

    if ( eQCWWAN_ERR_NONE != resultcode )
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* send QMI request and receive the response */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_WDS,
                                  paramLength,
                                  eQMI_TIMEOUT_2_S, /* 2 seconds */
                                  &pInParam,
                                  &paramLength );

    if ( eQCWWAN_ERR_NONE == resultcode )
    {
        /* initialize response structure */
        response.pSLQS3GPPConfigItem = pSLQS3GPPConfigItem;
        if( pSLQS3GPPConfigItem->pProfileList )
        {
            for( counter = 0; counter <= 4; counter++ )
            {
                pSLQS3GPPConfigItem->pProfileList[counter] = 0xFF;
            }
        }
        if( pSLQS3GPPConfigItem->pLTEAttachProfile )
            *(response.pSLQS3GPPConfigItem->pLTEAttachProfile) = 0xFF;
        if( pSLQS3GPPConfigItem->pDefaultPDNEnabled )
            *(response.pSLQS3GPPConfigItem->pDefaultPDNEnabled) = 0xFF;
        if( pSLQS3GPPConfigItem->p3gppRelease )
            *(response.pSLQS3GPPConfigItem->p3gppRelease) = 0xFF;

        /* Copy to the caller's buffer */
        resultcode = UpkQmiWdsSLQSGet3GPPConfigItem( pInParam, &response );
    }

    qmrelreqbkp();
    return resultcode;
}

package void qaWdsInit(void)
{
    swi_osapi_mutexinit(&wdsSessionManager.smmutex);
    wdsSessionManager.smlock = qaWdsSMlock;
    wdsSessionManager.smunlock = qaWdsSMunlock;
    wdsSessionManager.updateslots = qaWdsSMUpdateAvailablePDPIdx;
    wdsSessionManager.getnextslot = qaWdsSMGetAvailablePDPIdx;
    wdsSessionManager.sid2slot = qaWdsSMSessionIDtoPDPInstance;
    wdsSessionManager.print = wdsSMprint;
    int i = WDSPDP3IDX;
    do{
        swi_osapi_mutexinit(&wdsSessionManager.pdp[i].pdpmutex);
        wdsSessionManager.pdp[i].pdplock = qaWdsPDPSessionDataLock;
        wdsSessionManager.pdp[i].pdpunlock = qaWdsPDPSessionDataUnLock;
        wdsSessionManager.pdp[i].pdpIdx = i;
    }while( i--);
}

local void initProf3GPP( struct Profile3GPP *pProfile3GPP )
{
    BYTE idx = 0;
    if( pProfile3GPP->pProfilename )
    {
        *(pProfile3GPP->pProfilename) = EOS;
    }
    if( pProfile3GPP->pPDPtype )
    {
        *(pProfile3GPP->pPDPtype) = ~0;
    }
    if( pProfile3GPP->pPdpHdrCompType )
    {
        *(pProfile3GPP->pPdpHdrCompType) = ~0;
    }
    if( pProfile3GPP->pPdpDataCompType )
    {
        *(pProfile3GPP->pPdpDataCompType) = ~0;
    }
    if( pProfile3GPP->pAPNName )
    {
        *(pProfile3GPP->pAPNName) = EOS;
    }
    if( pProfile3GPP->pPriDNSIPv4AddPref )
    {
        *(pProfile3GPP->pPriDNSIPv4AddPref) = ~0;
    }
    if( pProfile3GPP->pSecDNSIPv4AddPref )
    {
        *(pProfile3GPP->pSecDNSIPv4AddPref) = ~0;
    }
    if( pProfile3GPP->pUMTSReqQoS )
    {
        struct UMTSQoS *pUMTSQoS = pProfile3GPP->pUMTSReqQoS;
        pUMTSQoS->trafficClass        = ~0;
        pUMTSQoS->maxUplinkBitrate    = ~0;
        pUMTSQoS->maxDownlinkBitrate  = ~0;
        pUMTSQoS->grntUplinkBitrate   = ~0;
        pUMTSQoS->grntDownlinkBitrate = ~0;
        pUMTSQoS->qosDeliveryOrder    = ~0;
        pUMTSQoS->maxSDUSize          = ~0;
        pUMTSQoS->sduErrorRatio       = ~0;
        pUMTSQoS->resBerRatio         = ~0;
        pUMTSQoS->deliveryErrSDU      = ~0;
        pUMTSQoS->transferDelay       = ~0;
        pUMTSQoS->trafficPriority     = ~0;
    }
    if( pProfile3GPP->pUMTSMinQoS )
    {
        struct UMTSQoS *pUMTSQoS = pProfile3GPP->pUMTSMinQoS;
        pUMTSQoS->trafficClass        = ~0;
        pUMTSQoS->maxUplinkBitrate    = ~0;
        pUMTSQoS->maxDownlinkBitrate  = ~0;
        pUMTSQoS->grntUplinkBitrate   = ~0;
        pUMTSQoS->grntDownlinkBitrate = ~0;
        pUMTSQoS->qosDeliveryOrder    = ~0;
        pUMTSQoS->maxSDUSize          = ~0;
        pUMTSQoS->sduErrorRatio       = ~0;
        pUMTSQoS->resBerRatio         = ~0;
        pUMTSQoS->deliveryErrSDU      = ~0;
        pUMTSQoS->transferDelay       = ~0;
        pUMTSQoS->trafficPriority     = ~0;
    }
    if( pProfile3GPP->pGPRSRequestedQos )
    {
        struct GPRSRequestedQoS *pGPRSReqQoS = pProfile3GPP->pGPRSRequestedQos;
        pGPRSReqQoS->precedenceClass     = ~0;
        pGPRSReqQoS->delayClass          = ~0;
        pGPRSReqQoS->reliabilityClass    = ~0;
        pGPRSReqQoS->peakThroughputClass = ~0;
        pGPRSReqQoS->meanThroughputClass = ~0;
    }
    if( pProfile3GPP->pGPRSMinimumQoS )
    {
        struct GPRSRequestedQoS *pGPRSReqQoS = pProfile3GPP->pGPRSMinimumQoS;
        pGPRSReqQoS->precedenceClass     = ~0;
        pGPRSReqQoS->delayClass          = ~0;
        pGPRSReqQoS->reliabilityClass    = ~0;
        pGPRSReqQoS->peakThroughputClass = ~0;
        pGPRSReqQoS->meanThroughputClass = ~0;
    }
    if( pProfile3GPP->pUsername )
    {
        *(pProfile3GPP->pUsername) = EOS;
    }
    if( pProfile3GPP->pPassword )
    {
        *(pProfile3GPP->pPassword) = EOS;
    }
    if( pProfile3GPP->pAuthenticationPref )
    {
        *(pProfile3GPP->pAuthenticationPref) = ~0;
    }
    if( pProfile3GPP->pIPv4AddrPref )
    {
        *(pProfile3GPP->pIPv4AddrPref) = ~0;
    }
    if( pProfile3GPP->pPcscfAddrUsingPCO )
    {
        *(pProfile3GPP->pPcscfAddrUsingPCO) = ~0;
    }
    if( pProfile3GPP->pPdpAccessConFlag )
    {
        *(pProfile3GPP->pPdpAccessConFlag) = ~0;
    }
    if( pProfile3GPP->pPcscfAddrUsingDhcp )
    {
        *(pProfile3GPP->pPcscfAddrUsingDhcp) = ~0;
    }
    if( pProfile3GPP->pImCnFlag )
    {
        *(pProfile3GPP->pImCnFlag) = ~0;
    }
    if( pProfile3GPP->pTFTID1Params )
    {
        struct TFTIDParams *pTFTIDPar = pProfile3GPP->pTFTID1Params;

        pTFTIDPar->filterId           = ~0;
        pTFTIDPar->eValid             = ~0;
        pTFTIDPar->ipVersion          = ~0;
        for( idx = 0; idx < 8; idx++ )
        {
            pTFTIDPar->pSourceIP[idx] = ~0;
        }
        pTFTIDPar->sourceIPMask       = ~0;
        pTFTIDPar->nextHeader         = ~0;
        pTFTIDPar->destPortRangeStart = ~0;
        pTFTIDPar->destPortRangeEnd   = ~0;
        pTFTIDPar->srcPortRangeStart  = ~0;
        pTFTIDPar->srcPortRangeEnd    = ~0;
        pTFTIDPar->IPSECSPI           = ~0;
        pTFTIDPar->tosMask            = ~0;
        pTFTIDPar->flowLabel          = ~0;
    }
    if( pProfile3GPP->pTFTID2Params )
    {
        struct TFTIDParams *pTFTIDPar = pProfile3GPP->pTFTID2Params;

        pTFTIDPar->filterId           = ~0;
        pTFTIDPar->eValid             = ~0;
        pTFTIDPar->ipVersion          = ~0;
        for( idx = 0; idx < 8; idx++ )
        {
            pTFTIDPar->pSourceIP[idx] = ~0;
        }
        pTFTIDPar->sourceIPMask       = ~0;
        pTFTIDPar->nextHeader         = ~0;
        pTFTIDPar->destPortRangeStart = ~0;
        pTFTIDPar->destPortRangeEnd   = ~0;
        pTFTIDPar->srcPortRangeStart  = ~0;
        pTFTIDPar->srcPortRangeEnd    = ~0;
        pTFTIDPar->IPSECSPI           = ~0;
        pTFTIDPar->tosMask            = ~0;
        pTFTIDPar->flowLabel          = ~0;
    }
    if( pProfile3GPP->pPdpContext )
    {
        *(pProfile3GPP->pPdpContext) = ~0;
    }
    if( pProfile3GPP->pSecondaryFlag )
    {
        *(pProfile3GPP->pSecondaryFlag) = ~0;
    }
    if( pProfile3GPP->pPrimaryID )
    {
        *(pProfile3GPP->pPrimaryID) = ~0;
    }
    if( pProfile3GPP->pIPv6AddPref )
    {
        for( idx = 0; idx < 8; idx++ )
        {
            pProfile3GPP->pIPv6AddPref[idx] = ~0;
        }
    }
    if( pProfile3GPP->pUMTSReqQoSSigInd )
    {
        struct UMTSReqQoSSigInd *pUMTSReqQoSSigInd =
            pProfile3GPP->pUMTSReqQoSSigInd;

        struct UMTSQoS *pUMTSQoS = &(pUMTSReqQoSSigInd->UMTSReqQoS);

        pUMTSQoS->trafficClass        = ~0;
        pUMTSQoS->maxUplinkBitrate    = ~0;
        pUMTSQoS->maxDownlinkBitrate  = ~0;
        pUMTSQoS->grntUplinkBitrate   = ~0;
        pUMTSQoS->grntDownlinkBitrate = ~0;
        pUMTSQoS->qosDeliveryOrder    = ~0;
        pUMTSQoS->maxSDUSize          = ~0;
        pUMTSQoS->sduErrorRatio       = ~0;
        pUMTSQoS->resBerRatio         = ~0;
        pUMTSQoS->deliveryErrSDU      = ~0;
        pUMTSQoS->transferDelay       = ~0;
        pUMTSQoS->trafficPriority     = ~0;
        pUMTSReqQoSSigInd->SigInd     = ~0;
    }
    if( pProfile3GPP->pUMTSMinQosSigInd )
    {
        struct UMTSReqQoSSigInd *pUMTSReqQoSSigInd =
            pProfile3GPP->pUMTSMinQosSigInd;

        struct UMTSQoS *pUMTSQoS =  &(pUMTSReqQoSSigInd->UMTSReqQoS);

        pUMTSQoS->trafficClass        = ~0;
        pUMTSQoS->maxUplinkBitrate    = ~0;
        pUMTSQoS->maxDownlinkBitrate  = ~0;
        pUMTSQoS->grntUplinkBitrate   = ~0;
        pUMTSQoS->grntDownlinkBitrate = ~0;
        pUMTSQoS->qosDeliveryOrder    = ~0;
        pUMTSQoS->maxSDUSize          = ~0;
        pUMTSQoS->sduErrorRatio       = ~0;
        pUMTSQoS->resBerRatio         = ~0;
        pUMTSQoS->deliveryErrSDU      = ~0;
        pUMTSQoS->transferDelay       = ~0;
        pUMTSQoS->trafficPriority     = ~0;
        pUMTSReqQoSSigInd->SigInd     = ~0;
    }
    if( pProfile3GPP->pPriDNSIPv6addpref )
    {
        for( idx = 0; idx < 8; idx++ )
        {
            pProfile3GPP->pPriDNSIPv6addpref[idx] = ~0;
        }
    }
    if( pProfile3GPP->pSecDNSIPv6addpref )
    {
        for( idx = 0; idx < 8; idx++ )
        {
            pProfile3GPP->pSecDNSIPv6addpref[idx] = ~0;
        }
    }
    if( pProfile3GPP->pAddrAllocPref )
    {
        *(pProfile3GPP->pAddrAllocPref) = ~0;
    }
    if( pProfile3GPP->pQosClassID )
    {
        struct QosClassID *pQosClsID = pProfile3GPP->pQosClassID;
        pQosClsID->QCI          = ~0;
        pQosClsID->gDlBitRate   = ~0;
        pQosClsID->maxDlBitRate = ~0;
        pQosClsID->gUlBitRate   = ~0;
        pQosClsID->maxUlBitRate = ~0;
    }
    if( pProfile3GPP->pAPNDisabledFlag )
    {
        *(pProfile3GPP->pAPNDisabledFlag) = ~0;
    }
    if( pProfile3GPP->pPDNInactivTimeout )
    {
        *(pProfile3GPP->pPDNInactivTimeout) = ~0;
    }
    if( pProfile3GPP->pAPNClass )
    {
        *(pProfile3GPP->pAPNClass) = ~0;
    }
}

local void initProf3GPP2( struct Profile3GPP2 *pProfile3GPP2 )
{
    BYTE idx = 0;
    if( pProfile3GPP2->pNegoDnsSrvrPref )
    {
        *(pProfile3GPP2->pNegoDnsSrvrPref) = ~0;
    }
    if( pProfile3GPP2->pPppSessCloseTimerDO )
    {
        *(pProfile3GPP2->pPppSessCloseTimerDO) = ~0;
    }
    if( pProfile3GPP2->pPppSessCloseTimer1x )
    {
        *(pProfile3GPP2->pPppSessCloseTimer1x) = ~0;
    }
    if( pProfile3GPP2->pAllowLinger )
    {
        *(pProfile3GPP2->pAllowLinger) = ~0;
    }
    if( pProfile3GPP2->pLcpAckTimeout )
    {
        *(pProfile3GPP2->pLcpAckTimeout) = ~0;
    }
    if( pProfile3GPP2->pIpcpAckTimeout )
    {
        *(pProfile3GPP2->pIpcpAckTimeout) = ~0;
    }
    if( pProfile3GPP2->pAuthTimeout )
    {
        *(pProfile3GPP2->pAuthTimeout) = ~0;
    }
    if( pProfile3GPP2->pLcpCreqRetryCount )
    {
        *(pProfile3GPP2->pLcpCreqRetryCount) = ~0;
    }
    if( pProfile3GPP2->pIpcpCreqRetryCount )
    {
        *(pProfile3GPP2->pIpcpCreqRetryCount) = ~0;
    }
    if( pProfile3GPP2->pAuthRetryCount )
    {
        *(pProfile3GPP2->pAuthRetryCount) = ~0;
    }
    if( pProfile3GPP2->pAuthProtocol )
    {
        *(pProfile3GPP2->pAuthProtocol) = ~0;
    }
    if( pProfile3GPP2->pUserId )
    {
        *(pProfile3GPP2->pUserId) = EOS;
    }
    if( pProfile3GPP2->pAuthPassword )
    {
        *(pProfile3GPP2->pAuthPassword) = EOS;
    }
    if( pProfile3GPP2->pDataRate )
    {
        *(pProfile3GPP2->pDataRate) = ~0;
    }
    if( pProfile3GPP2->pAppType )
    {
        *(pProfile3GPP2->pAppType) = ~0;
    }
    if( pProfile3GPP2->pDataMode )
    {
        *(pProfile3GPP2->pDataMode) = ~0;
    }
    if( pProfile3GPP2->pAppPriority )
    {
        *(pProfile3GPP2->pAppPriority) = ~0;
    }
    if( pProfile3GPP2->pApnString )
    {
        *(pProfile3GPP2->pApnString) = EOS;
    }
    if( pProfile3GPP2->pPdnType )
    {
        *(pProfile3GPP2->pPdnType) = ~0;
    }
    if( pProfile3GPP2->pIsPcscfAddressNedded )
    {
        *(pProfile3GPP2->pIsPcscfAddressNedded) = ~0;
    }
    if( pProfile3GPP2->pPrimaryV4DnsAddress )
    {
        *(pProfile3GPP2->pPrimaryV4DnsAddress) = ~0;
    }
    if( pProfile3GPP2->pSecondaryV4DnsAddress )
    {
        *(pProfile3GPP2->pSecondaryV4DnsAddress) = ~0;
    }
    if( pProfile3GPP2->pPriV6DnsAddress )
    {
        for( idx = 0; idx < 8; idx++ )
        {
            pProfile3GPP2->pPriV6DnsAddress[idx] = ~0;
        }
    }
    if( pProfile3GPP2->pSecV6DnsAddress )
    {
        for( idx = 0; idx < 8; idx++ )
        {
            pProfile3GPP2->pSecV6DnsAddress[idx] = ~0;
        }
    }
    if( pProfile3GPP2->pRATType )
    {
        *(pProfile3GPP2->pRATType) = ~0;
    }
    if( pProfile3GPP2->pAPNEnabled3GPP2 )
    {
        *(pProfile3GPP2->pAPNEnabled3GPP2) = ~0;
    }
    if( pProfile3GPP2->pPDNInactivTimeout3GPP2 )
    {
        *(pProfile3GPP2->pPDNInactivTimeout3GPP2) = ~0;
    }
    if( pProfile3GPP2->pAPNEnabled3GPP2 )
    {
        *(pProfile3GPP2->pAPNEnabled3GPP2) = ~0;
    }
}

ULONG SLQSGetProfileSettings(
    GetProfileSettingIn  *pProfileSettingIn,
    GetProfileSettingOut *pProfileSettingsOut )
{
    ULONG  resultcode;  /* result code */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT ParamLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiWdsSlqsGetProfileSettingsResp response;
    struct Profile3GPP                      *pProfile3GPP;
    struct Profile3GPP2                     *pProfile3GPP2;
    BYTE                                    profileType;

    /* Check against NULL for params */
    if ( !pProfileSettingIn ||
         !pProfileSettingsOut )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Store the profile type */
    profileType = pProfileSettingIn->ProfileType;

    /* pack QMI request */
    resultcode = PkQmiWdsSlqsGetProfileSettings( &ParamLength,
                                                 pOutParam,
                                                 pProfileSettingIn );

    if ( eQCWWAN_ERR_NONE != resultcode )
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* send QMI request */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_WDS,
                                  ParamLength,
                                  eQMI_TIMEOUT_2_S, /* 2 seconds */
                                  &pInParam,
                                  &ParamLength );

    if ( eQCWWAN_ERR_NONE == resultcode )
    {
        /* initialize response structure */
        response.pProfileSettings = pProfileSettingsOut;

        /* Initialize all optional out params to default values */
        if( PROFILE_3GPP == profileType )
        {
            pProfile3GPP = &(pProfileSettingsOut->curProfile.SlqsProfile3GPP);
            initProf3GPP( pProfile3GPP );
        }
        else
        {
            pProfile3GPP2 = &(pProfileSettingsOut->curProfile.SlqsProfile3GPP2);
            initProf3GPP2( pProfile3GPP2 );
        }

        /* Copy to the caller's buffer */
        resultcode = UpkQmiWdsSlqsGetProfileSettings( pInParam,
                                                      &profileType,
                                                      &response );
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG SetMobileIPProfile(
    CHAR  *pSPC,
    BYTE  index,
    BYTE  *pEnabled,
    ULONG *pAddress,
    ULONG *pPrimaryHA,
    ULONG *pSecondaryHA,
    BYTE  *pRevTunneling,
    CHAR  *pNAI,
    ULONG *pHASPI,
    ULONG *pAAASPI,
    CHAR  *pMNHA,
    CHAR  *pMNAAA )
{
    ULONG  resultCode;  /* result code */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT ParamLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiWdsModifyMipProfileResp response;

    /* Check against NULL for mandatory parameter */
    if (!pSPC)
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Index points to first QMI header. */
    ParamLength = 0;

    /* pack QMI request */
    resultCode = PkQmiWdsModifyMipProfile( &ParamLength,
                                           pOutParam,
                                           pSPC,
                                           index,
                                           pEnabled,
                                           pAddress,
                                           pPrimaryHA,
                                           pSecondaryHA,
                                           pRevTunneling,
                                           pNAI,
                                           pHASPI,
                                           pAAASPI,
                                           pMNHA,
                                           pMNAAA );
    if (resultCode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultCode;
    }

    /* send QMI request */
    resultCode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_WDS,
                                  ParamLength,
                                  eQMI_TIMEOUT_2_S,
                                  &pInParam,
                                  &ParamLength );

    if (resultCode == eQCWWAN_ERR_NONE)
    {
        resultCode = UpkQmiWdsModifyMipProfile(pInParam, &response);
    }

    qmrelreqbkp();
    return resultCode;
}

ULONG SLQSWdsSetEventReport( wdsSetEventReportReq *pSetEventReportReq )
{
    ULONG  resultcode;  /* result code */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT ParamLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiWdsSLQSSetEventReportResp response;

    /* Checking for Invalid Parameter */
    if ( NULL == pSetEventReportReq )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultcode = PkQmiWdsSLQSSetEventReport( &ParamLength,
                                             pOutParam,
                                             pSetEventReportReq );

    if ( eQCWWAN_ERR_NONE != resultcode )
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_WDS,
                                  ParamLength,
                                  eQMI_TIMEOUT_2_S,
                                  &pInParam,
                                  &ParamLength );

    /* Only parse out the response data if we get a positive return */
    if ( eQCWWAN_ERR_NONE == resultcode )
    {
        /* Copy to the caller's buffer */
        resultcode = UpkQmiWdsSLQSSetEventReport( pInParam, &response );
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSWdsSwiPDPRuntimeSettings(
    swiPDPRuntimeSettingsReq  *pPDPRuntimeSettingsReq,
    swiPDPRuntimeSettingsResp *pPDPRuntimeSettingsResp )
{
    ULONG  resultCode;  /* result code */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT ParamLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiWdsSLQSSwiPDPRuntimeSettingsResp response;

    /* Check against NULL for mandatory parameter */
    if ( NULL == pPDPRuntimeSettingsReq || NULL == pPDPRuntimeSettingsResp )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* pack QMI request */
    resultCode = PkQmiWdsSLQSSwiPDPRuntimeSettings( &ParamLength,
                                                    pOutParam,
                                                    pPDPRuntimeSettingsReq );

    if (resultCode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultCode;
    }

    /* Send QMI request */
    resultCode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_WDS,
                                  ParamLength,
                                  eQMI_TIMEOUT_2_S,
                                  &pInParam,
                                  &ParamLength );

    if (resultCode == eQCWWAN_ERR_NONE)
    {
        /* Pass the pointer for the OUT parameters */
        response.pPDPRuntimeSettingsResp = pPDPRuntimeSettingsResp;

        /* Initializing the response */
        if ( pPDPRuntimeSettingsResp->pContextId )
        {
            *(pPDPRuntimeSettingsResp->pContextId) = ~0;
        }
        if ( pPDPRuntimeSettingsResp->pBearerId )
        {
            *(pPDPRuntimeSettingsResp->pBearerId) = ~0;
        }
        if ( pPDPRuntimeSettingsResp->pAPNName )
        {
            *(pPDPRuntimeSettingsResp->pAPNName) = EOS;
        }
        if ( pPDPRuntimeSettingsResp->pIPv4Address )
        {
            *(pPDPRuntimeSettingsResp->pIPv4Address) = ~0;
        }
        if ( pPDPRuntimeSettingsResp->pIPv4GWAddress )
        {
            *(pPDPRuntimeSettingsResp->pIPv4GWAddress) = ~0;
        }
        if ( pPDPRuntimeSettingsResp->pPrDNSIPv4Address )
        {
            *(pPDPRuntimeSettingsResp->pPrDNSIPv4Address) = ~0;
        }
        if ( pPDPRuntimeSettingsResp->pSeDNSIPv4Address )
        {
            *(pPDPRuntimeSettingsResp->pSeDNSIPv4Address) = ~0;
        }
        if ( pPDPRuntimeSettingsResp->pIPv6Address )
        {
            pPDPRuntimeSettingsResp->pIPv6Address->IPV6PrefixLen = ~0;
        }
        if ( pPDPRuntimeSettingsResp->pIPv6GWAddress )
        {
            pPDPRuntimeSettingsResp->pIPv6GWAddress->IPV6PrefixLen = ~0;
        }
        if ( pPDPRuntimeSettingsResp->pPrPCSCFIPv4Address )
        {
            *(pPDPRuntimeSettingsResp->pPrPCSCFIPv4Address) = ~0;
        }
        if ( pPDPRuntimeSettingsResp->pSePCSCFIPv4Address )
        {
            *(pPDPRuntimeSettingsResp->pSePCSCFIPv4Address) = ~0;
        }

        resultCode = UpkQmiWdsSLQSSwiPDPRuntimeSettings( pInParam, &response );
    }

    qmrelreqbkp();
    return resultCode;
}

ULONG iGetConnectionRate (
        ULONG *pV4sessionId,
        ULONG *pV6sessionId,
        struct WdsConnectionRateElmnts *pConnectionRateElmnt )
{
    int    pdpidx = -1;

    /* Check for invalid parameters */
    if ( NULL == pConnectionRateElmnt )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /*Set Ipfamily accordig to the IP channel*/
    if( NULL != pV4sessionId )
    {
        if( 0 == *pV4sessionId )
        {
            return eQCWWAN_ERR_SWICM_INVALID_V4_SESSION_ID;
        }

        pdpidx = wdsSessionManager.sid2slot(*pV4sessionId);
        if( -1 == pdpidx )
        {
            return eQCWWAN_ERR_SWICM_INVALID_V4_SESSION_ID;
        }

        ULONG rc;
        if( !WDS_IsGobiDevice() &&
            eQCWWAN_ERR_NONE != (rc = iSLQSMISetIPFamilyPreference(IPV4, pdpidx )) )
        {
            return rc;
        }
    }
    else if( NULL != pV6sessionId )
    {
        if( 0 == *pV6sessionId )
        {
            return eQCWWAN_ERR_SWICM_INVALID_V6_SESSION_ID;
        }

        pdpidx = wdsSessionManager.sid2slot(*pV6sessionId);
        if( -1 == pdpidx )
        {
            return eQCWWAN_ERR_SWICM_INVALID_V6_SESSION_ID;
        }

        ULONG rc;
        if( !WDS_IsGobiDevice() &&
            eQCWWAN_ERR_NONE != (rc = iSLQSMISetIPFamilyPreference(IPV6, pdpidx)) )
        {
            return rc;
        }
    }

    /*Call GetConnectionrate*/
return    GetConnectionRate( pConnectionRateElmnt->pCurrentChannelTXRate,
                             pConnectionRateElmnt->pCurrentChannelRXRate,
                             pConnectionRateElmnt->pMaxChannelTXRate,
                             pConnectionRateElmnt->pMaxChannelRXRate,
                             pdpidx );

}

ULONG SLQSGetConnectionRate (
    struct WdsConnectionRate *pConnectionRate )
{

	/*Check for invalid parameter*/
    if( NULL == pConnectionRate->pV4sessionId &&
        NULL == pConnectionRate->pV6sessionId )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Local variables */
    struct WdsConnectionRateElmnts *pConnRateElmnt;
    int iptype = 0;
    enum eQCWWANError rcv4 = eQCWWAN_ERR_NONE;

    if( NULL != pConnectionRate->pV4sessionId )
    {
        iptype = IPV4;

        /* Get Connection rate related to IPv4 */
        pConnRateElmnt             = &pConnectionRate->ConnRateElmntsV4;
        rcv4 = iGetConnectionRate( pConnectionRate->pV4sessionId,
                                   NULL,
                                   pConnRateElmnt );
    }

    enum eQCWWANError rcv6 = eQCWWAN_ERR_NONE;
    if( NULL != pConnectionRate->pV6sessionId )
    {
        iptype = IPV4 == iptype ? IPV4V6 : IPV6;

        /* Get Connection rate related to IPv6 */
        pConnRateElmnt             = &pConnectionRate->ConnRateElmntsV6;
        rcv6 = iGetConnectionRate( NULL,
                                   pConnectionRate->pV6sessionId,
                                   pConnRateElmnt );
    }

    switch( iptype )
    {
        case IPV4:
            pConnectionRate->ConnRateElmntsV6.pCurrentChannelRXRate = NULL;
            pConnectionRate->ConnRateElmntsV6.pCurrentChannelTXRate = NULL;
            pConnectionRate->ConnRateElmntsV6.pMaxChannelRXRate = NULL;
            pConnectionRate->ConnRateElmntsV6.pMaxChannelTXRate = NULL;
            return rcv4;
            break;

        case IPV6:
            pConnectionRate->ConnRateElmntsV4.pCurrentChannelRXRate = NULL;
            pConnectionRate->ConnRateElmntsV4.pCurrentChannelTXRate = NULL;
            pConnectionRate->ConnRateElmntsV4.pMaxChannelRXRate = NULL;
            pConnectionRate->ConnRateElmntsV4.pMaxChannelTXRate = NULL;
            return rcv6;
            break;

        case IPV4V6:
            return rcv6;

    }

    /* IPV4V6 - Determine return codes to pass back to the caller */
    if( eQCWWAN_ERR_NONE == rcv4 &&
        eQCWWAN_ERR_NONE == rcv6 )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* v6 settings available */
    if( eQCWWAN_ERR_NONE == rcv6 )
    {
        if( eQCWWAN_ERR_QMI_OUT_OF_CALL == rcv4 )
        {
            /* v4 connection down */
            return eQCWWAN_ERR_SWICM_V4DWN_V6UP;
        }
        else
        {
            /* Error retrieving v4 settings */
            return rcv4;
        }
    }

    /* v4 settings available */
    if( eQCWWAN_ERR_NONE == rcv4 )
    {
        if( eQCWWAN_ERR_QMI_OUT_OF_CALL == rcv6 )
        {
            /* v6 connection down */
            return eQCWWAN_ERR_SWICM_V4UP_V6DWN;
        }
        else
        {
            /* Error retrieving v6 settings */
            return rcv6;
        }
    }

    /* v4 connection down */
    if( eQCWWAN_ERR_QMI_OUT_OF_CALL == rcv4 )
    {
        /* v6 connection down */
        if( eQCWWAN_ERR_QMI_OUT_OF_CALL == rcv6 )
        {
            return eQCWWAN_ERR_QMI_OUT_OF_CALL;
        }
        else
        {
            /* Error retrieving v6 settings */
            return rcv6;
        }
    }

    /* v6 connection down */
    if( eQCWWAN_ERR_QMI_OUT_OF_CALL == rcv6 )
    {
        /* v4 connection down */
        if( eQCWWAN_ERR_QMI_OUT_OF_CALL == rcv4 )
        {
            return eQCWWAN_ERR_QMI_OUT_OF_CALL;
        }
        else
        {
            /* Error retrieving v4 settings */
            return rcv4;
        }
    }

    /* Error retrieving both IP addresses & both sessions appear to be up.
     * Return first point of failure
     */
    return rcv4;
}


ULONG iGetByteTotals (
        ULONG                      *pV4sessionId,
        ULONG                      *pV6sessionId,
        struct WdsByteTotalsElmnts *pByteTotalsElmnt )
{
    int    pdpidx = -1;

    /* Check for invalid parameters */
    if ( NULL == pByteTotalsElmnt )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /*Set Ipfamily according to the IP channel*/
    if( NULL != pV4sessionId )
    {
        if( 0 == *pV4sessionId )
        {
            return eQCWWAN_ERR_SWICM_INVALID_V4_SESSION_ID;
        }

        pdpidx = wdsSessionManager.sid2slot(*pV4sessionId);
        if( -1 == pdpidx )
        {
            return eQCWWAN_ERR_SWICM_INVALID_V4_SESSION_ID;
        }

        ULONG rc;
        if( !WDS_IsGobiDevice() &&
            eQCWWAN_ERR_NONE != (rc = iSLQSMISetIPFamilyPreference(IPV4, pdpidx )) )
        {
            return rc;
        }
    }
    else if( NULL != pV6sessionId )
    {
        if( 0 == *pV6sessionId )
        {
            return eQCWWAN_ERR_SWICM_INVALID_V6_SESSION_ID;
        }

        pdpidx = wdsSessionManager.sid2slot(*pV6sessionId);
        if( -1 == pdpidx )
        {
            return eQCWWAN_ERR_SWICM_INVALID_V6_SESSION_ID;
        }

        ULONG rc;
        if( !WDS_IsGobiDevice() &&
            eQCWWAN_ERR_NONE != (rc = iSLQSMISetIPFamilyPreference(IPV6, pdpidx)) )
        {
            return rc;
        }
    }

    /*Call GetByteTotals*/
return    GetByteTotals( pByteTotalsElmnt->pTXTotalBytes,
                         pByteTotalsElmnt->pRXTotalBytes,
                         pdpidx );

}

ULONG SLQSGetByteTotals (
    struct WdsByteTotals *pByteTotals )
{

    /*Check for invalid parameter*/
    if( NULL == pByteTotals->pV4sessionId &&
        NULL == pByteTotals->pV6sessionId )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Local variables */
    struct WdsByteTotalsElmnts *pByteTotalsElmnt;
    int iptype = 0;
    enum eQCWWANError rcv4 = eQCWWAN_ERR_NONE;

    if( NULL != pByteTotals->pV4sessionId )
    {
        iptype = IPV4;

        /* Get Byte totals related to IPv4 */
        pByteTotalsElmnt  = &pByteTotals->ByteTotalsElmntsV4;
        rcv4 = iGetByteTotals( pByteTotals->pV4sessionId,
                               NULL,
                               pByteTotalsElmnt );
    }

    enum eQCWWANError rcv6 = eQCWWAN_ERR_NONE;
    if( NULL != pByteTotals->pV6sessionId )
    {
        iptype = IPV4 == iptype ? IPV4V6 : IPV6;

        /* Get Byte totals related to IPv6 */
        pByteTotalsElmnt = &pByteTotals->ByteTotalsElmntsV6;
        rcv6 = iGetByteTotals( NULL,
                               pByteTotals->pV6sessionId,
                               pByteTotalsElmnt );
    }

    switch( iptype )
    {
        case IPV4:
            pByteTotals->ByteTotalsElmntsV6.pRXTotalBytes = NULL;
            pByteTotals->ByteTotalsElmntsV6.pTXTotalBytes = NULL;
            return rcv4;
            break;

        case IPV6:
            pByteTotals->ByteTotalsElmntsV4.pRXTotalBytes = NULL;
            pByteTotals->ByteTotalsElmntsV4.pTXTotalBytes = NULL;
            return rcv6;
            break;

        case IPV4V6:
            break;

    }

    /* IPV4V6 - Determine return codes to pass back to the caller */
    if( eQCWWAN_ERR_NONE == rcv4 &&
        eQCWWAN_ERR_NONE == rcv6 )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* v6 settings available */
    if( eQCWWAN_ERR_NONE == rcv6 )
    {
        if( eQCWWAN_ERR_QMI_OUT_OF_CALL == rcv4 )
        {
            /* v4 connection down */
            return eQCWWAN_ERR_SWICM_V4DWN_V6UP;
        }
        else
        {
            /* Error retrieving v4 settings */
            return rcv4;
        }
    }

    /* v4 settings available */
    if( eQCWWAN_ERR_NONE == rcv4 )
    {
        if( eQCWWAN_ERR_QMI_OUT_OF_CALL == rcv6 )
        {
            /* v6 connection down */
            return eQCWWAN_ERR_SWICM_V4UP_V6DWN;
        }
        else
        {
            /* Error retrieving v6 settings */
            return rcv6;
        }
    }

    /* v4 connection down */
    if( eQCWWAN_ERR_QMI_OUT_OF_CALL == rcv4 )
    {
        /* v6 connection down */
        if( eQCWWAN_ERR_QMI_OUT_OF_CALL == rcv6 )
        {
            return eQCWWAN_ERR_QMI_OUT_OF_CALL;
        }
        else
        {
            /* Error retrieving v6 settings */
            return rcv6;
        }
    }

    /* v6 connection down */
    if( eQCWWAN_ERR_QMI_OUT_OF_CALL == rcv6 )
    {
        /* v4 connection down */
        if( eQCWWAN_ERR_QMI_OUT_OF_CALL == rcv4 )
        {
            return eQCWWAN_ERR_QMI_OUT_OF_CALL;
        }
        else
        {
            /* Error retrieving v4 settings */
            return rcv4;
        }
    }

    /* Error retrieving both IP addresses & both sessions appear to be up.
     * Return first point of failure
     */
    return rcv4;
}

ULONG  SLQSWdsGoDormant( void )
{
    ULONG              resultcode;  /* result code */
    BYTE               *pInParam;   /* ptr to param field rx'd from modem */
    BYTE               *pOutParam;  /* ptr to outbound param field */
   BYTE               *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT             ParamLength; /* Ret'd length of the QMI Param field */

   /* Storage for results and response variable */
    struct QmiWdsGoDormantResp response;
    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
   pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultcode = PkQmiWdsGoDormant( &ParamLength,
                                    pOutParam );

    if (resultcode != eQCWWAN_ERR_NONE) {
       qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
   resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_WDS,
                                  ParamLength,
                                  eQMI_TIMEOUT_5_S,
                                  &pInParam,
                                  &ParamLength);

    /* Only parse out the response data if we got a positive return */
    if (resultcode == eQCWWAN_ERR_NONE) {
        /* Copy to the caller's buffer */
        resultcode = UpkQmiWdsGoDormant(pInParam, &response);
    }

    qmrelreqbkp();
    return resultcode;
}
