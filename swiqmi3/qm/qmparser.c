/*************
 *
 * Filename: qmparser.c
 *
 * Purpose:  qmi message parser
 *
 * Copyright: © 2011 Sierra Wireless Inc., all rights reserved
 *
 **************/
#include "qmidefs.h"
#include "pi/piudefs.h"

#define QMPSR_LIST_LEN 10
#define DBG_QMPARSER
#define IPV4Family 4
#define IPV6Family 6
#ifdef DBG_QMPARSER
#include <syslog.h>
#endif

struct qmimsgnode{
    swi_uint16 msgid;
    void *(*msgparser)(swi_uint8* pmsg);
};

struct qmpsrentry{
    enum eQMIService svc;
    void (*parser)(swi_uint8 *pmsg);
    struct qmimsgnode *list[QMPSR_LIST_LEN];
};
/* Common Data */
local swi_uint8 instanceid;

/* WDS */
local void* psrwds_getsettings(swi_uint8*);
local void* psrwds_stopnet(swi_uint8*);
local void* psrwds_starnet(swi_uint8*);
local void* psrwds_setipfamily(swi_uint8*);
local void qmparserwds(swi_uint8 *pmsg);

local struct qmimsgnode wdsn4 =
{
    eQMI_WDS_GET_SETTINGS,
    psrwds_getsettings
};

/* DMS */
local void* psrwds_setfwPref(swi_uint8*);
local void qmparserdms(swi_uint8 *pmsg);

local struct qmimsgnode wdsn3 =
{
    eQMI_WDS_STOP_NET,
    psrwds_stopnet
};

local struct qmimsgnode wdsn2 =
{
    eQMI_WDS_START_NET,
    psrwds_starnet
};

local struct qmimsgnode wdsn1 =
{
    eQMI_WDS_SET_IP_FAMILY,
    psrwds_setipfamily
};

local struct qmpsrentry wdspsr = {
    eQMI_SVC_WDS,
    qmparserwds,
    { &wdsn1, &wdsn2, &wdsn3, &wdsn4, NULL,
      NULL, NULL, NULL, NULL, NULL }
};

local struct qmimsgnode dmsn1 =
{
    eQMI_DMS_SET_FIRMWARE_PREF,
    psrwds_setfwPref
};

local struct qmimsgnode dmsn2 =
{
    eQMI_DMS_SET_FIRMWARE_ID,
    psrwds_setfwPref
};

local struct qmpsrentry dmspsr = {
    eQMI_SVC_DMS,
    qmparserdms,
    { &dmsn1, &dmsn2, NULL, NULL, NULL,
      NULL, NULL, NULL, NULL, NULL }
};

local struct qmpsrentry *qmparsers[] = {
    &wdspsr,
    &dmspsr
};

local void* psrwds_getsettings(swi_uint8* pmsg)
{
#ifdef DBG_QMPARSER
    syslog( LOG_DEBUG, "%s:", __func__ );
#endif
   return pmsg;
}

local void* psrwds_stopnet(swi_uint8* pmsg)
{
#ifdef DBG_QMPARSER
    syslog( LOG_DEBUG, "%s:", __func__ );
#endif
   return pmsg; }

local void* psrwds_starnet(swi_uint8* pmsg)
{
#ifdef DBG_QMPARSER
    syslog( LOG_DEBUG, "%s:", __func__ );
#endif
   return pmsg;
}

local void* psrwds_setipfamily(swi_uint8* pmsg)
{
    /* extract ip family preference */
    swi_uint8 ipfp = *( pmsg + QMISVC_MSGLENSZ + QMITLV_TYPE_SZ +
                        QMITLV_LENGTH_SZ);

    if( IPv4_FAMILY_PREFERENCE == ipfp )
    {
        qmwdsclientset( instanceid == 0 ? QMIWDSCLNT :
                        instanceid == 1 ? QMIWDSCLNT2:
                        instanceid == 2 ? QMIWDSCLNT3:
                        instanceid == 3 ? QMIWDSCLNT4:
                        instanceid == 4 ? QMIWDSCLNT5:
                        instanceid == 5 ? QMIWDSCLNT6:
                        instanceid == 6 ? QMIWDSCLNT7: QMIWDSV6CLNT8  );
        qmSetActiveIPFamily(IPV4Family);
    }
    else if( IPv6_FAMILY_PREFERENCE == ipfp )
    {
        qmwdsclientset( instanceid == 0 ? QMIWDSV6CLNT :
                        instanceid == 1 ? QMIWDSV6CLNT2:
                        instanceid == 2 ? QMIWDSV6CLNT3:
                        instanceid == 3 ? QMIWDSV6CLNT4:
                        instanceid == 4 ? QMIWDSV6CLNT5:
                        instanceid == 5 ? QMIWDSV6CLNT6:
                        instanceid == 6 ? QMIWDSV6CLNT7: QMIWDSV6CLNT8 );
        qmSetActiveIPFamily(IPV6Family);
    }
#ifdef DBG_QMPARSER
    enum qmisupportedclients active =  qmwdsclientget();
    syslog( LOG_DEBUG,
            "%s: instance: %d IP family: V%d Active WDS Client: %s",
            __func__,
            instanceid,
            ipfp,
            QMIWDSCLNT == active || QMIWDSCLNT2 == active
                                 || QMIWDSCLNT3 == active
                                 || QMIWDSCLNT4 == active
                                 || QMIWDSCLNT5 == active
                                 || QMIWDSCLNT6 == active
                                 || QMIWDSCLNT7 == active
                                 || QMIWDSCLNT8 == active ? "V4" : "V6" );
#endif
    return NULL;
}

local void qmparserwds(swi_uint8* pmsg)
{
    swi_uint16 msgid = piget16(&pmsg);
    int nnodes = sizeof(wdspsr.list)/sizeof(wdspsr.list[0]) ;
    int i;
    swi_bool handler_exists = FALSE;

    for( i = 0 ; i < nnodes && wdspsr.list[i] != NULL ; i++ )
    {
        if( msgid == wdspsr.list[i]->msgid )
        {
            /* QMI message parser found */
            wdspsr.list[i]->msgparser(pmsg);
            handler_exists = TRUE;
            break;
        }
    }

    if( !handler_exists )
    {
        if(qmGetActiveIPFamily() == IPV6Family )
        /*
         * The WDS client is lookup by QMI instance as below.
         * Please note that we always reset to IPv4 WDS client
         * If we need to access the WDS IPv6 client, we need to call 
         * iSLQSSetIPFamilyPreference before the QMI request of interest.
         * See iSLQSGetRuntimeSettings for example.
         */
            qmwdsclientset( instanceid == 0 ? QMIWDSV6CLNT :
                            instanceid == 1 ? QMIWDSV6CLNT2:
                            instanceid == 2 ? QMIWDSV6CLNT3:
                            instanceid == 3 ? QMIWDSV6CLNT4:
                            instanceid == 4 ? QMIWDSV6CLNT5:
                            instanceid == 5 ? QMIWDSV6CLNT6:
                            instanceid == 6 ? QMIWDSV6CLNT7: QMIWDSV6CLNT8 );

        else
        qmwdsclientset( instanceid == 0 ? QMIWDSCLNT :
                        instanceid == 1 ? QMIWDSCLNT2:
                        instanceid == 2 ? QMIWDSCLNT3:
                        instanceid == 3 ? QMIWDSCLNT4:
                        instanceid == 4 ? QMIWDSCLNT5:
                        instanceid == 5 ? QMIWDSCLNT6:
                        instanceid == 6 ? QMIWDSCLNT7: QMIWDSCLNT8  );
    }
}

local void* psrwds_setfwPref(swi_uint8* pmsg)
{
    UNUSEDPARAM( pmsg );
    /* Get the QMI RR control block pointer */
    struct qmtcb *qmcbp = qmgetcbp();

    qmcbp->fms_data.fw_download_requested = TRUE;
#ifdef DBG_QMPARSER
    syslog( LOG_DEBUG,
            "%s: Firmware download Requested",
            __func__ );
#endif
    return NULL;
}

local void qmparserdms(swi_uint8* pmsg)
{
    swi_uint16 msgid = piget16(&pmsg);
    int nnodes = sizeof(dmspsr.list)/sizeof(dmspsr.list[0]) ;
    int i;

    for( i = 0 ; i < nnodes && dmspsr.list[i] != NULL ; i++ )
    {
        if( msgid == dmspsr.list[i]->msgid )
        {
            /* QMI message parser found */
            dmspsr.list[i]->msgparser(pmsg);
            break;
        }
    }

}

package void qmparser(  swi_uint8 qmiinstanceid,
                        enum eQMIService svc,
                        swi_uint8* pmsg )
{
    instanceid = qmiinstanceid;

    int numpsrs = sizeof(qmparsers)/sizeof(qmparsers[0]) ;
    int i;
    for( i = 0 ; i < numpsrs ; i++ )
    {
        if( svc == qmparsers[i]->svc )
        {
            /* QMI service parser found */
            qmparsers[i]->parser(pmsg);
            break;
        }
    }
}

