/**
 *
 * @ingroup swiril
 *
 * @file 
 * Provides support for QMI Location Service (LOC) functions.
 *
 * @author
 * Copyright: © 2015 Sierra Wireless, Inc.
 *            All rights reserved
 *
 */
 
 /* To capture logs from this facility use:
 *     'adb logcat -v time | grep gps_'
 */
#define LOG_TAG "gps_qmi"
#include "swiril_log.h"
#include "swiril_gps_qmi_loc.h"
#include "swiril_gps.h"
#include "swiril_main.h"
#include "swiril_misc_qmi.h"
#include "swiril_network_qmi.h"
#include <string.h>

#define  GPS_DEBUG  0

#define  DFR(...)   LOGD(__VA_ARGS__)

#if GPS_DEBUG
#  define  D(...)   LOGD(__VA_ARGS__)
#else
#  define  D(...)   ((void)0)
#endif

#define LOC_SERVERTYPE_UMTS 0x00000003
#define LOC_SERVERTYPE_CDMA 0x00000001

static int gLOC_RegInfo[5];


void cbkGetOpMode(QmiCbkLocGetOpModeInd *pGetOpModeNotification)
{
    ULONG status = eQCWWAN_ERR_GENERAL;
    ULONG mode_loc = 0;
    ULONG mode_pds = 0;
    
    status = pGetOpModeNotification->status;
    D("%s::status = %lu",__func__, status);
    disableLocCallback(LOC_GET_OPMODE);
    
    if (eQCWWAN_ERR_NONE == status)
    {
        if (pGetOpModeNotification->pMode)
            mode_loc = *pGetOpModeNotification->pMode;

        switch (mode_loc)
        {
            case 2:
                setLocReqInfo(LOC_GET_OPMODE, MODE_MSB);
                break;
            case 3:
                setLocReqInfo(LOC_GET_OPMODE, MODE_MSA);
                break;
            default:
                setLocReqInfo(LOC_GET_OPMODE, MODE_STANDALONE);
                break;
        
        }
        
        /* Get Fix Criteria info before response */
        rilLocGetFixCriteria();
    }    
    else {
        int written, fd;
        char *errp;
        BYTE *bufp;
        BYTE respbuf[SWIGPS_MSGSIZE_HEADER+SWIGPS_MSGSIZE_RESP_GETDEF];
        char errmsgbuf[ERRMSGSIZE];
        ULONG acc_defautl = 50;
        ULONG interval_default = 1000;
        BYTE  timeout_default = 255;
        /* Clear the response buffer prior to use */
        memset( respbuf, 0, sizeof(respbuf) );
        /* Copy of the pointer to the buffer */
        bufp = respbuf;

        /* Make the header */
        errp = swigps_makhdr( SWIGPS_MSGTYPE_RESP_GETDEF, 
                       SWIGPS_MSGSIZE_RESP_GETDEF, 
                       status,
                       bufp, sizeof(respbuf), &written );

        /* Ensure the header got built */
        if(errp) {
            LOGE("%s makhdr error: %s",__func__, errp);
            LOGE("Called with type %d, buflength %d", 
                 SWIGPS_MSGTYPE_RESP_GETDEF, sizeof(respbuf) );
            return;
        }

        /* Adjust bufp to the first byte past the header */
        bufp = respbuf + SWIGPS_MSGSIZE_HEADER;

        /* Mode - 1 byte */
        *bufp++ = (BYTE) MODE_STANDALONE;

        /* Timeout - 1 byte */
        *bufp++ = timeout_default;

        /* Interval - 4 bytes */
        ril_piput32( interval_default, &bufp );

        /* Accuracy - 4 bytes */
        ril_piput32( acc_defautl, &bufp );

        /* Get filedesc which store in LOC_GET_FIX_CRT*/
        fd = getLocReqInfo(LOC_GET_FIX_CRT);
        
        /* Send it to the swigps daemon */
        if ( socksend( fd, respbuf, (bufp - respbuf), errmsgbuf ) ) {
            LOGE("%s", errmsgbuf);
            return;
        }

        D("Sent %d bytes to swigps", (bufp-respbuf));
    }
 
}

void cbkGetFixCriteria(QmiCbkLocGetFixCriteriaInd *pGetFixCriteriaNotification)
{
    int written, fd;
    char *errp;
    BYTE *bufp;
    BYTE respbuf[SWIGPS_MSGSIZE_HEADER+SWIGPS_MSGSIZE_RESP_GETDEF];
    char errmsgbuf[ERRMSGSIZE];
    ULONG status = eQCWWAN_ERR_GENERAL;
    ULONG acc_level = 0;
    ULONG acc_meter[] = {50, 500, 100, 25};
    ULONG interval = 1000;
    BYTE timeout = 255;
    
    status = pGetFixCriteriaNotification->status;
    D("%s::status = %lu",__func__, status);
    if(pGetFixCriteriaNotification->pAccuracy)
        acc_level = *pGetFixCriteriaNotification->pAccuracy;
    
    if(pGetFixCriteriaNotification->pMinInterval)
        interval = *pGetFixCriteriaNotification->pMinInterval;
    
    disableLocCallback(LOC_GET_FIX_CRT); 
    fd = getLocReqInfo(LOC_GET_FIX_CRT);
    
    /* Send respose of rilgpsGetDef */
    
    /* Clear the response buffer prior to use */
    memset( respbuf, 0, sizeof(respbuf) );
    /* Copy of the pointer to the buffer */
    bufp = respbuf;

    /* Make the header */
    errp = swigps_makhdr( SWIGPS_MSGTYPE_RESP_GETDEF, 
                   SWIGPS_MSGSIZE_RESP_GETDEF, 
                   status,
                   bufp, sizeof(respbuf), &written );

    /* Ensure the header got built */
    if(errp) {
        LOGE("%s makhdr error: %s",__func__, errp);
        LOGE("Called with type %d, buflength %d", 
             SWIGPS_MSGTYPE_RESP_GETDEF, sizeof(respbuf) );
		return;
	}

    /* Adjust bufp to the first byte past the header */
    bufp = respbuf + SWIGPS_MSGSIZE_HEADER;

    /* Mode - 1 byte */
    *bufp++ = (BYTE) getLocReqInfo(LOC_GET_OPMODE);

    /* Timeout - 1 byte */
    *bufp++ = timeout;

    /* Interval - 4 bytes */
    ril_piput32( interval, &bufp );

    /* Accuracy - 4 bytes */
    ril_piput32( acc_meter[acc_level], &bufp );

    /* Send it to the swigps daemon */
    if ( socksend( fd, respbuf, (bufp - respbuf), errmsgbuf ) ) {
        LOGE("%s", errmsgbuf);
        return;
    }

    D("Sent %d bytes to swigps", (bufp-respbuf));
}


void cbkGetServer(QmiCbkLocGetServerInd *pGetServerNotification)
{
    static BYTE respbuf[SWIGPS_MSGSIZE_HEADER+SWIGPS_MSGSIZE_MAX_AGPS];
    BYTE *bufp;
    char errmsgbuf[ERRMSGSIZE];
    int written, fd;
    char *errp;
    ULONG status;
    ULONG type;
    ULONG ipaddr = 0;
    WORD port = 0;
    CHAR urlString[256];
    BYTE urlStringLen = 0;
    
    status = pGetServerNotification->status;
    type =  pGetServerNotification->serverType;
    memset(urlString,0,sizeof(urlString));
    D("%s::status = %lu, Type = %lu",__func__, status, type); 

    disableLocCallback(LOC_GET_SERVER);    
    fd = getLocReqInfo(LOC_GET_SERVER);
    
    if( pGetServerNotification->pAddressIPV4 ) {
       ipaddr = pGetServerNotification->pAddressIPV4->ipv4Addr; 
       port = pGetServerNotification->pAddressIPV4->port;
    }
    
    if(pGetServerNotification->pAddressURL)
    {
        UrlAddress *lresp = pGetServerNotification->pAddressURL;
        strcpy(urlString, lresp->url);
        urlStringLen = strlen(urlString);
    } 
    
    /* Clear the response buffer prior to use */
    memset( respbuf, 0, sizeof(respbuf) );

    /* Copy of the pointer to the buffer */
    bufp = respbuf;

    /* Make the header */
    errp = swigps_makhdr( SWIGPS_MSGTYPE_RESP_GETAGPS, 
                   SWIGPS_MSGSIZE_MIN_AGPS, 
                   status,
                   bufp, sizeof(respbuf), &written );    

    /* Ensure the header got built */
    if(errp) {
        LOGE("%s makhdr error: %s",__func__, errp);
        LOGE("rilgpsGetAgps: Called with type %d, buflength %d", 
             SWIGPS_MSGTYPE_RESP_GETAGPS, sizeof(respbuf) );
		return;
	}

    /* Adjust bufp to the first byte past the header */
    bufp = respbuf + SWIGPS_MSGSIZE_HEADER;

    /* IPAddress */
    ril_piput32( ipaddr, &bufp );

    /* port - 4 bytes */
    ril_piput32((int)port, &bufp );
    
    /* URL*/
    if(urlStringLen)
    {
        /* Store it into the packet */
        *bufp++ = (unsigned char) urlStringLen;
        /* Finally, copy the URL into the destination */
        ril_piputmulti( (unsigned char *) urlString, &bufp, urlStringLen );
    }

    /* Send it to the swigps daemon */
    if ( socksend( fd, respbuf, (bufp - respbuf), errmsgbuf ) ) {
        LOGE("%s", errmsgbuf);
        return;
    }
    D("Sent %d bytes to swigps", (bufp-respbuf));    
}

void cbkSetServer(QmiCbkLocSetServerInd *pSetServerNotification )
{   
    ULONG status;
    int written, fd;
    char *errp;
    char errmsgbuf[ERRMSGSIZE];
    BYTE respbuf[SWIGPS_MSGSIZE_HEADER]; /* Response buffer - sent back to swigps after modem responds */
    
    status  = pSetServerNotification->status;
    D("%s::status = %lu",__func__, status);
    
    disableLocCallback(LOC_SET_SERVER);
    fd = getLocReqInfo(LOC_SET_SERVER);
   
    /* Clear the response buffer prior to use */
    memset( respbuf, 0, sizeof(respbuf) );
    
    /* Make the header */
    errp = swigps_makhdr( SWIGPS_MSGTYPE_RESP_SETAGPS, 
                          SWIGPS_MSGSIZE_RESP_SETAGPS, 
                          status,
                          respbuf, sizeof(respbuf), &written );

    /* Ensure the header got built */
    if(errp) {
        LOGE("%s makhdr error: %s",__func__, errp);
        LOGE("rilgpsSetAgps: type %d, buflength %d", 
             SWIGPS_MSGTYPE_RESP_SETAGPS, sizeof(respbuf) );
		return;
	}
    
    /* Send it to the swigps daemon */
    if ( socksend( fd, respbuf, written, errmsgbuf ) ) {
        LOGE("%s", errmsgbuf);
        return;
    }
    D("Sent %d bytes to swigps", written);
    
}

void cbkInjectUTCTime(QmiCbkLocInjectUTCTimeInd *pInjectUTCTimeNotification )
{
    ULONG status;
    int written, fd;
    char *errp;
    char errmsgbuf[ERRMSGSIZE];
    BYTE respbuf[SWIGPS_MSGSIZE_HEADER]; /* Response buffer - sent back to swigps after modem responds */
    
    status = pInjectUTCTimeNotification->status;
    D("%s::status = %lu",__func__, status);
    
    disableLocCallback(LOC_INJECT_UTC);
    fd = getLocReqInfo(LOC_INJECT_UTC);
    
    /* Clear the response buffer prior to use */
    memset( respbuf, 0, sizeof(respbuf) );
    
    /* Make the header */
    errp = swigps_makhdr( SWIGPS_MSGTYPE_RESP_INJTIME, 
                          SWIGPS_MSGSIZE_RESP_INJTIME, 
                          status,
                          respbuf, sizeof(respbuf), &written );

    /* Ensure the header got built */
    if(errp) {
        LOGE("%s makhdr error: %s",__func__, errp);
        LOGE("rilgpsInjTime: type %d, buflength %d", 
             SWIGPS_MSGTYPE_RESP_INJTIME, sizeof(respbuf) );
		return;
	}

    /* Send it to the swigps daemon */
    if ( socksend( fd, respbuf, written, errmsgbuf ) ) {
        LOGE("%s", errmsgbuf);
        return;
    }
    
    D("Sent %d bytes to swigps", written);
}

ULONG enableLocCallback(LOC_EVENT id)
{
    D("%s:: entered\n", __func__);
    ULONG nRet = eQCWWAN_ERR_NONE;
   
    /* register callback function */
    if (LOC_INJECT_UTC == id)
        nRet = SLQSSetLocInjectUTCTimeCallback( &cbkInjectUTCTime );
    else if (LOC_GET_SERVER == id)
        nRet = SLQSSetLocGetServerCallback( &cbkGetServer );
    else if (LOC_SET_SERVER == id)
        nRet = SLQSSetLocSetServerCallback( &cbkSetServer );
    else if (LOC_GET_OPMODE == id)
        nRet = SLQSSetLocGetOpModeCallback (&cbkGetOpMode);
    else if (LOC_GET_FIX_CRT == id)
        nRet = SLQSSetLocGetFixCriteriaCallback( &cbkGetFixCriteria );
    
    if ( nRet != eQCWWAN_ERR_NONE ) {
        LOGE("%s:: Loc enable callback failed, nRet: %lu, event: %d\n",
             __func__, nRet, id );
    }
    return nRet;
}

ULONG disableLocCallback(LOC_EVENT id)
{
    D("%s:: entered\n", __func__);
    ULONG nRet = eQCWWAN_ERR_NONE;
   
    /* register callback function */
    if (LOC_INJECT_UTC == id)
        nRet = SLQSSetLocInjectUTCTimeCallback( NULL );
    else if (LOC_GET_SERVER == id)
        nRet = SLQSSetLocGetServerCallback( NULL );
    else if (LOC_SET_SERVER == id)
        nRet = SLQSSetLocSetServerCallback( NULL );
    else if (LOC_GET_OPMODE == id)
        nRet = SLQSSetLocGetOpModeCallback (NULL);
    else if (LOC_GET_FIX_CRT == id)
        nRet = SLQSSetLocGetFixCriteriaCallback( NULL );
    
    if ( nRet != eQCWWAN_ERR_NONE ) {
        LOGE("%s:: Loc enable callback failed, nRet: %lu, event: %d\n",
             __func__, nRet, id );
    }
    return nRet;
}

void setLocReqInfo(LOC_EVENT id, int value)
{
    D("%s:: entered, id=%d , value=%d\n", __func__, id ,value);
    gLOC_RegInfo[id] = value;
}

int getLocReqInfo(LOC_EVENT id)
{
    D("%s:: entered, id=%d\n", __func__, id);
    return gLOC_RegInfo[id];
}

ULONG rilLocInjTime(int filedesc,
                    ULONGLONG time,
                    ULONG uncertainty)
{
    ULONG nRet = eQCWWAN_ERR_NONE;
    D("%s:: entered\n", __func__);
    
    nRet = enableLocCallback(LOC_INJECT_UTC);
    if ( nRet != eQCWWAN_ERR_NONE )
        return nRet;
    
    setLocReqInfo(LOC_INJECT_UTC, filedesc);
    
    nRet = SLQSLOCInjectUTCTime(time, uncertainty);
    if ( nRet != eQCWWAN_ERR_NONE )
        LOGE("%s failed",__func__);    
        
    return nRet;   
}


ULONG rilLocGetServer(int filedesc)
{
    ULONG nRet = eQCWWAN_ERR_NONE;
    LOCGetServerReq req;
    
    D("%s:: entered\n", __func__);
    
    nRet = enableLocCallback(LOC_GET_SERVER);
    if ( nRet != eQCWWAN_ERR_NONE )
        return nRet;
    
    setLocReqInfo(LOC_GET_SERVER, filedesc);
    
    memset(&req, 0, sizeof(req));

    if(SWI_RUNNING_TECH_CDMA == getRealtimeTechnology())
        req.serverType = LOC_SERVERTYPE_CDMA;
    else        
        req.serverType = LOC_SERVERTYPE_UMTS;
    
    nRet = SLQSLOCGetServer(req);
    if ( nRet != eQCWWAN_ERR_NONE )
        LOGE("%s failed",__func__);    
        
    return nRet;  
}

ULONG rilLocSetServer(int filedesc, ULONG ipaddr, ULONG port, BYTE *urlbuf, BYTE urllen)
{
    ULONG nRet = eQCWWAN_ERR_NONE;
    LOCSetServerReq req;
    IPv4Address     ipv4;
    UrlAddress      url;
    BYTE i;
    
    D("%s:: entered\n", __func__);
    memset(&req, 0, sizeof(LOCSetServerReq));
    
    if(SWI_RUNNING_TECH_CDMA == getRealtimeTechnology())        
        req.serverType = LOC_SERVERTYPE_CDMA; 
    else        
        req.serverType = LOC_SERVERTYPE_UMTS; 
    
    if( ipaddr && port ) {    
        ipv4.ipv4Addr = (unsigned int)ipaddr;
        ipv4.port = (int) port;              
        req.pIPv4Address = &ipv4;
    }   
    
    if (urllen > 0)
    {
        memcpy(url.url, urlbuf, urllen);
        url.url[urllen] = 0;
        req.pURLAddress = &url;
    }
    
    nRet = enableLocCallback(LOC_SET_SERVER);
    if ( nRet != eQCWWAN_ERR_NONE )
        return nRet;
    
    setLocReqInfo(LOC_SET_SERVER, filedesc);
    
    nRet = SLQSLOCSetServer(req);
    if ( nRet != eQCWWAN_ERR_NONE )
        LOGE("%s failed",__func__);    
    return nRet;   
}

ULONG rilLocGetDef(int filedesc)
{
    ULONG nRet = eQCWWAN_ERR_NONE;
    D("%s:: entered\n", __func__);
    
    /* rilgpsGetDef request operation mode and fix criteria info,
     * Get OpMode first. And send response in
     * getFixCriteria callback.
     */
     
    setLocReqInfo(LOC_GET_FIX_CRT, filedesc);        
    nRet = rilLocGetOpMode();

    return nRet;  
}

ULONG rilLocGetOpMode()
{
    ULONG nRet = eQCWWAN_ERR_NONE;
    D("%s:: entered\n", __func__);
    
    nRet = enableLocCallback(LOC_GET_OPMODE);
    if ( nRet != eQCWWAN_ERR_NONE )
        return nRet;
    
    nRet = SLQSLOCGetOpMode();
    if ( nRet != eQCWWAN_ERR_NONE )
        LOGE("%s failed",__func__);    
    
    return nRet;
}

ULONG rilLocGetFixCriteria()
{
    ULONG nRet = eQCWWAN_ERR_NONE;
    D("%s:: entered\n", __func__);
    
    nRet = enableLocCallback(LOC_GET_FIX_CRT);
    if ( nRet != eQCWWAN_ERR_NONE )
        return nRet;
    
    nRet = SLQSLOCGetFixCriteria();
    if ( nRet != eQCWWAN_ERR_NONE )
        LOGE("%s failed",__func__);    
    
    return nRet;  
}




