/**
 *
 * @ingroup swiril
 *
 * @file 
 * Provides support for AT-based GPS commands
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
#include <cutils/sockets.h>
#include <cutils/properties.h>

#include "swiril_main.h"
#include "at_channel.h"

//#include "qmerrno.h"
#include "swiril_gps.h"

/* To capture logs from this facility use:
 *     'adb logcat -v time | grep gps_'
 */
#define LOG_TAG "gps_at"
#include "swiril_log.h"

#define  GPS_DEBUG  1

#define  DFR(...)   LOGD(__VA_ARGS__)

#if GPS_DEBUG
#  define  D(...)   LOGD(__VA_ARGS__)
#else
#  define  D(...)   ((void)0)
#endif

/* Local definitions */

#define DISABLE_TRACKING 0
#define ENABLE_TRACKING 1

#define UTC 1
#define FORCE 1

#define START_GPS_IN_DRIVER 1

/**
 *
 * GPS AT agent which handles the request to get default settings
 * from an AT modem. 
 * 
 * @param [in] paramp
 *     Indicates which file handle to use when replying to the sgi side
 *     once the default settings have been received from the modem
 *
 * @return
 *     none
 *
 * @par Abort:<br> 
 *      none
 *
 * @note
 *      Runs in the context of the RIL daemon
 *
 */
void rilgpsGetDef( void *paramp )
{
    unsigned char *bufp;

    bufp = (unsigned char *) paramp;

    /* Need to free the parameter array */
    free ( bufp );

    LOGE("%s: not implemented for AT RIL", __func__);
}

/**
 *
 * GPS AT Agent to process a request to set the default values
 * in an AT modem. 
 * 
 * @param [in] paramp
 *     A pointer to the arguments sent by the swigps daemon. NOTE:
 *     This points to a block of allocated memory and must be freed
 *     before returning from this handler
 *
 * @return
 *     none
 *
 * @par Abort:<br> 
 *      none
 *
 * @note
 *      Runs in the context of the RIL daemon
 * 
 *      The incoming arguments are stored in network byte order in
 *      a buffer pointed to by paramp. The order of arguments is:
 * 
 *      fd        - File descriptor for socket connection to swigps
 *      args      - Packet bytes which follow the format of the 
 *                  Defaults Request packet, documented in the
 *                  "Software Design Specification for Android GPS
 *                  Vendor Specific Location"
 * 
 *      This function checks the tracking status byte in the 
 *      parameters list. The tracking status at the end of this 
 *      function must be the same as it was at the beginning
 *
 *      Setting defaults should not also start a tracking session
 *      but in an AT device, if tracking is already enabled then
 *      it must be stopped and restarted with the new defaults. 
 *
 */
void rilgpsSetDef( void *paramp )
{
    int written, fd;
    char *errp;
    unsigned int operation;
    unsigned int interval;
    unsigned int accuracy;
    unsigned int tracking;
    unsigned char timeout;
    int nret;
    ATResponse *atresponse = NULL;
    unsigned char *tmpbufp, *bufp;
    char *pCmd = NULL;
    char errmsgbuf[ERRMSGSIZE];
    /* No variable part, just the header is returned */
    unsigned char respbuf[SWIGPS_MSGSIZE_HEADER];

    /* First, unpack the incoming arguments */
    bufp = (unsigned char *) paramp;
    tmpbufp = bufp;

    /* Get the fd from paramp */
    fd = ril_piget32( &bufp );

    /* Mode - 1 byte */
    operation = (unsigned int) *bufp++;

    /* Timeout - 1 byte */
    timeout = (unsigned int) *bufp++;

    /* Interval - 4 bytes */
    interval = ril_piget32( &bufp );

    /* Accuracy - 4 bytes */
    accuracy = ril_piget32( &bufp );

    /* Tracking status - 1 byte */
    tracking = (unsigned int ) *bufp++;

    /* If tracking session is underway, need to stop it */
    if( tracking )
    {
        /* Use AT!GPSEND=0 to stop the session */
        asprintf(&pCmd, "AT!GPSEND=0");
        nret = at_send_command(pCmd, &atresponse);
        free(pCmd);
        if (nret < 0  || atresponse->success == 0) {
            LOGE("%s session end err=%d, atresponse->success=%d\n",
                 __func__, nret,atresponse->success);
        }
    }

    /*** No tracking in progress at this point ***/
    
    /* The AT command for tracking is 
     * !GPSTRACK: <fixtype>,<maxtime>,<maxdist>,<fixcount>,<fixrate>
     * <fixtype>: 1-Standalone, 2-MS-Based, 3-MS-Assisted
     * <maxtime>: 1-255 seconds
     * <maxdist>: 1-4294967280 meters
     * <fixcount>: 1-1000,1000=continuous
     * <fixrate>: 1-65535 seconds
     *
     * Need to convert incoming args into ASCII 
     */
    asprintf(&pCmd, "AT!GPSTRACK=%u,%u,%u,1000,%u",
             operation, timeout, accuracy, interval );

    nret = at_send_command(pCmd, &atresponse);
    free(pCmd);
    if (nret < 0  || atresponse->success == 0) {
        LOGE("%s track err=%d, atresponse->success=%d\n",
             __func__, nret,atresponse->success);
    }

    /* Clear the response buffer prior to use */
    memset( respbuf, 0, sizeof(respbuf) );

    /* Make the header */
    errp = swigps_makhdr( SWIGPS_MSGTYPE_RESP_SETDEF, 
                          SWIGPS_MSGSIZE_RESP_SETDEF, 
                          nret,
                          respbuf, sizeof(respbuf), &written );

    /* Ensure the header got built */
    if(errp) {
        LOGE("%s makhdr error: %s",__func__, errp);
        LOGE("Called with type %d, buflength %d", 
             SWIGPS_MSGTYPE_RESP_SETDEF, sizeof(respbuf) );
		return;
	}

    /* Send it to the swigps daemon */
    if ( socksend( fd, respbuf, written, errmsgbuf ) ) {
        LOGE("%s", errmsgbuf);
        return;
    }

    free(tmpbufp);
	at_response_free(atresponse);

    D("%s: sent %d bytes to swigps", __func__,written);
}

/**
 *
 * GPS AT agent for getting AGPS information from the modem and
 * sending it back to the swigps daemon
 * 
 * @param [in] paramp
 *     Indicates which file handle to use when replying to the sgi side
 *
 * @return
 *     none
 *
 * @par Abort:<br> 
 *      none
 *
 * @note
 *      Runs in the context of the RIL daemon
 * 
 *      Not used at the moment. Placeholder in case it is needed 
 * 
 */
void rilgpsGetAgps( void *paramp )
{
    unsigned char *bufp;

    bufp = (unsigned char *) paramp;

    /* Need to free the parameter array */
    free ( bufp );

    LOGE("%s: not implemented for AT RIL", __func__);
}

/**
 *
 * GPS AT agent for setting AGPS information on the modem
 * 
 * @param [in] 
 *     Pointer to an array of memory containing the values passed
 *     from the swigps package. Needs to be unpacked into local
 *     variables
 * @return
 *     none
 *
 * @par Abort:<br> 
 *      none
 *
 * @note
 *      Runs in the context of the RIL daemon
 *
 *      This function is not re-entrant because the urlbuf is statically
 *      declared to save putting too much storage on the stack
 *
 *      This function has been tested on a modem with a hard-coded 
 *      CnD password. The default password has been used in the
 *      code below but this may not work on all modules.
 * 
 */
void rilgpsSetAgps( void *paramp )
{
    int written, fd;
    int nret;
    char *errp;
    char *pCmd = NULL;
    unsigned int ipaddr;
    unsigned int port;
    unsigned int urllength;
    unsigned char *tmpbufp, *bufp;
    ATResponse *atresponse = NULL;
    char errmsgbuf[ERRMSGSIZE];

    /* Temporary storage for the URL */
    static unsigned char urlbuf[MAXURL];

    /* Response buffer - sent back to swigps after modem responds */
    char respbuf[SWIGPS_MSGSIZE_HEADER];

    /* First, unpack the incoming arguments 
     * and free the incoming buffer pointer
     * before we make any decisions about 
     * whether to return early or not
     */
    bufp = (unsigned char *) paramp;
    tmpbufp = bufp;

    /* Get the fd from paramp */
    fd = ril_piget32( &bufp );

    /* ip address - 4 bytes */
    ipaddr = ril_piget32( &bufp );

    /* server port - 4 bytes */
    port = ril_piget32( &bufp );

    /* Length of the URL, not including NULL termination */
    urllength = (unsigned int) *bufp++;

    /* Server URL - length variable */
    ril_pigetmulti( &bufp, urlbuf, (unsigned short) urllength );

    /* NULL Terminate the URL */
    urlbuf[urllength] = 0;

    D("%s: ipaddr: %ul, port: %ul", __func__, ipaddr, port);
    D("%s: hostname: %s", __func__, urlbuf );

    /* First need to unlock the modem to CND mode */
    asprintf(&pCmd, "AT!ENTERCND=\"A710\"");

    nret = at_send_command(pCmd, &atresponse);
    free(pCmd);
    if (nret < 0  || atresponse->success == 0) {
        LOGE("%s unlock err=%d, atresponse->success=%d\n",
             __func__, nret,atresponse->success);
    }

    /* If there is a port number defined, 
     * combine it with the host name URL. 
     */
    if( port ) {
        asprintf(&pCmd, "AT!GPSSUPLURL=\"%s:%d\"", urlbuf, port);
    }
    else {
        asprintf(&pCmd, "AT!GPSSUPLURL=\"%s\"", urlbuf );
    }

    nret = at_send_command(pCmd, &atresponse);
    free(pCmd);
    if (nret < 0  || atresponse->success == 0) {
        LOGE("%s err=%d, atresponse->success=%d\n",
             __func__, nret,atresponse->success);
    }

    /* Just send nret back along with the values received. If there
     * was an error the recipient can deal with it
     */

    /* Make the header */
    errp = swigps_makhdr( SWIGPS_MSGTYPE_RESP_SETAGPS, 
                          SWIGPS_MSGSIZE_RESP_SETAGPS, 
                          nret,
                          (unsigned char *) respbuf, sizeof(respbuf), &written );

    /* Ensure the header got built */
    if(errp) {
        LOGE("%s makhdr error: %s",__func__, errp);
        LOGE("Called with type %d, buflength %d", 
             SWIGPS_MSGTYPE_RESP_SETAGPS, sizeof(respbuf) );
		return;
	}

    /* Send it to the swigps daemon */
    if ( socksend( fd, respbuf, written, errmsgbuf ) ) {
        LOGE("%s", errmsgbuf);
        return;
    }

    D("%s sent %d bytes to swigps", __func__,(written));

    free(tmpbufp);
	at_response_free(atresponse);
}

/**
 *
 * GPS AT agent for handling requests to inject time into the 
 * modem
 * 
 * @param [in] 
 *     Pointer to an array of memory containing the values passed
 *     from the swigps package. Needs to be unpacked into local
 *     variables
 * @return
 *     none
 *
 * @par Abort:<br> 
 *      none
 *
 * @note
 *      Runs in the context of the RIL daemon
 *
 *      This function is not re-entrant because the urlbuf is statically
 *      declared to save putting too much storage on the stack
 * 
 */
void rilgpsInjTime( void *paramp )
{
    unsigned char *bufp;

    bufp = (unsigned char *) paramp;

    /* Need to free the parameter array */
    free ( bufp );

    LOGE("%s: not implemented for AT RIL", __func__);
}

/**
 *
 * GPS AT agent which handles the request to stop a tracking session
 * on the modem
 * 
 * @param [in] paramp
 *     Indicates which file handle to use when replying to the sgi side
 *
 * @return
 *     none
 *
 * @par Abort:<br> 
 *      none
 *
 * @note
 *      Runs in the context of the RIL daemon
 *
 */
void rilgpsStopSession( void *paramp )
{
    int written, fd;
    const char *pPdpCid = NULL;
    char *pCmd = NULL;
    char *tmpbufp, *bufp, *errp;
    char respbuf[SWIGPS_MSGSIZE_HEADER+SWIGPS_MSGSIZE_RESP_STOP];
    char errmsgbuf[ERRMSGSIZE];
    int nret;
    ATResponse *atresponse = NULL;

    bufp = paramp;
    tmpbufp = bufp;

    /* Get the fd from paramp */
    fd = ril_piget32( (unsigned char **) &bufp );

    D("%s: using filedesc: %d", __func__, fd);

#ifndef START_GPS_IN_DRIVER
    /* Use AT!GPSEND=0 to stop the session */
    asprintf(&pCmd, "AT!GPSEND=0");

    nret = at_send_command(pCmd, &atresponse);
    free(pCmd);
    if (nret < 0  || atresponse->success == 0) {
        LOGE("%s err=%d, atresponse->success=%d\n",
             __func__, nret,atresponse->success);
    }
#endif

    /* Just send nret back along with the values received. If there
     * was an error the recipient can deal with it
     */

    bufp = respbuf;

    /* Make the header */
    /*errp = swigps_makhdr( SWIGPS_MSGTYPE_RESP_STOP, 
                   SWIGPS_MSGSIZE_RESP_STOP, 
                   nret,
                   (unsigned char *) bufp, sizeof(respbuf), &written );*/

    /* Ensure the header got built */
    /*if(errp) {
        LOGE("%s makhdr error: %s",__func__, errp);
        LOGE("Called with type %d, buflength %d", 
             SWIGPS_MSGTYPE_RESP_STOP, sizeof(respbuf) );
return;
	}*/

    /* Send it to the swigps daemon */
    /*if ( socksend( fd, respbuf, written, errmsgbuf ) ) {
        LOGE("%s", errmsgbuf);
        return;
    }*/

    D("%s sent %d bytes to swigps", __func__,(bufp-respbuf));

    free(tmpbufp);
	at_response_free(atresponse);
}

void managegpsXtra(void)
{
    char propValue[PROPERTY_VALUE_MAX];
    int enabled = 0;
    ATResponse *atresponse = NULL;
    LOGD("%s::Entered\n",__func__);
    if (property_get("persist.sierra.gpsxtra", propValue, NULL)>0)
    {
        LOGD("Property value exists");
        enabled = atoi(propValue);
        if (enabled)
        {
            char* ptr = NULL;
            at_send_command_singleline("AT!GPSXTRASTATUS?"," Xtra Data status",&atresponse);
            if (atresponse->success ==0)
            {
                LOGD("%s::AT!GPSXTRASTATUS? command failed\n", __func__);
                at_response_free(atresponse);
                goto error;
            }
            ptr = strstr(atresponse->p_intermediates->line,"Valid");
            if (ptr==NULL)
            {
                at_response_free(atresponse);
                at_send_command("AT!entercnd=\"A710\"", &atresponse);
                if (atresponse->success ==0)
                {
                    LOGD("%s::AT!entercnd command failed\n",__func__);
                    at_response_free(atresponse);
                    goto error;
                }
                at_response_free(atresponse);
                at_send_command("AT!GPSXTRADATAENABLE=1", &atresponse);
                if (atresponse->success ==0)
                {
                    LOGD("%s::AT!GPSXTRADATAENABLE command failed\n",__func__);
                    at_response_free(atresponse);
                }
                at_response_free(atresponse);
                at_send_command("AT!GPSXTRAINITDNLD", &atresponse);
                if (atresponse->success ==0)
                {
                    LOGD("%s::AT!GPSXTRAINITDNLD command failed\n",__func__);
                    at_response_free(atresponse);
                    goto error;
                }
                at_response_free(atresponse);
            }
            else
            {
                LOGD("%s::Invalid substring not found",__func__);
            }
        }
        else
        {
            at_send_command("AT!ENTERCND=\"A710\"", &atresponse);
            if (atresponse->success ==0)
            {
                LOGD("%s::AT!entercnd command failed\n",__func__);
                at_response_free(atresponse);
                goto error;
            }
            at_response_free(atresponse);
            at_send_command("AT!GPSXTRADATAENABLE=0", &atresponse);
            if (atresponse->success ==0)
            {
                LOGD("%s::AT!entercnd command failed\n",__func__);
                at_response_free(atresponse);
                goto error;
            }
            at_response_free(atresponse);
        }
    }
    else
    {
        LOGD("%s::Property does not exist",__func__);
    }
    error:
    return;
}

/**
 *
 * GPS AT agent which handles the request to start a tracking session
 * on the modem
 * 
 * @param [in] paramp
 *     Indicates which file handle to use when replying to the sgi side
 *
 * @return
 *     none
 *
 * @par Abort:<br> 
 *      none
 *
 * @note
 *      Runs in the context of the RIL daemon
 *
 *      This agent also hard-codes the AT!GPSNMEACONFIG to enable
 *      NMEA traffic with a 1-second interval. This code could be
 *      moved to its own agent one day, but there isn't much point
 *      since there are no direct controls from Android for these
 *      2 settings
 *
 */
void rilgpsStartSession( void *paramp )
{
    int written, fd;
	char *pCmd = NULL;
    char *errp;
    ATResponse *atresponse = NULL;
    unsigned int operation;
    int nret;
	unsigned int accuracy, interval;
    unsigned char *tmpbufp, *bufp;
    unsigned char respbuf[SWIGPS_MSGSIZE_HEADER+SWIGPS_MSGSIZE_RESP_START];
    char errmsgbuf[ERRMSGSIZE];
    unsigned char timeout;

    managegpsXtra();

    /* No variable part, just the header is returned */

    /* First, unpack the incoming arguments 
     * and free the incoming buffer pointer
     * before we make any decisions about 
     * whether to return early or not
     */
    bufp = (unsigned char *) paramp;
    tmpbufp = bufp;

    /* Get the fd from paramp */
	fd = ril_piget32( &bufp );

    /* Mode - 1 byte */
    operation = (unsigned int) *bufp++;

    /* Timeout - 1 byte */
    timeout = *bufp++;

    /* Interval - 4 bytes */
    interval = ril_piget32( &bufp );

    /* Accuracy - 4 bytes */
    accuracy = ril_piget32( &bufp );

    D("%s: op'n: %d, timeout: %d, interval: %d, accuracy: %d",
      __FUNCTION__, (int) operation, (int) timeout, 
     (int) interval, (int) accuracy);

    D("%s: filedesc: %d",__func__, fd);

    /* NMEA configuration is mandatory and
     * may or may not have been done at the
     * factory. Doing it here has no effect
     * except for a little extra overhead.
     *
     * NOTE: gps_timer_thread() uses the 
     * 'interval' to generate reports to 
     * the GPS service. The modem's interval
     * is hard-coded to 1 second, below
     */
#ifndef START_GPS_IN_DRIVER
    asprintf(&pCmd, "AT!GPSNMEACONFIG=1,1");
    
    nret = at_send_command(pCmd, &atresponse);
    free(pCmd);
    if (nret < 0  || atresponse->success == 0) {
        LOGE("%s nmea config err=%d, atresponse->success=%d\n",
             __func__, nret,atresponse->success);
    }
#endif

    /* The AT command for tracking is 
     * !GPSTRACK: <fixtype>,<maxtime>,<maxdist>,<fixcount>,<fixrate>
     * <fixtype>: 1-Standalone, 2-MS-Based, 3-MS-Assisted
     * <maxtime>: 1-255 seconds
     * <maxdist>: 1-4294967280 meters
     * <fixcount>: 1-1000,1000=continuous
     * <fixrate>: 1-65535 seconds
     *
     * Need to convert incoming args into ASCII 
     */
#ifndef START_GPS_IN_DRIVER
    asprintf(&pCmd, "AT!GPSTRACK=%u,%u,%u,1000,%u",
             operation, timeout, accuracy, interval );

    nret = at_send_command(pCmd, &atresponse);
    free(pCmd);
    if (nret < 0  || atresponse->success == 0) {
        LOGE("%s err=%d, atresponse->success=%d\n",
             __func__, nret,atresponse->success);
    }
#endif

    /* Clear the response buffer prior to use */
    memset( respbuf, 0, sizeof(respbuf) );

    /* Just send nret back along with the values received. 
     * If there was an error swigps can deal with it
     */

    /* Make the header */
    errp = swigps_makhdr( SWIGPS_MSGTYPE_RESP_START, 
                          SWIGPS_MSGSIZE_RESP_START, 
                          nret,
                          respbuf, sizeof(respbuf), &written );

    /* Ensure the header got built */
    if(errp) {
        LOGE("%s makhdr error: %s",__func__, errp);
        LOGE("Called with type %d, buflength %d", 
             SWIGPS_MSGTYPE_RESP_START, sizeof(respbuf) );
		return;
	}

    /* Send it to the swigps daemon */
    if ( socksend( fd, respbuf, written, errmsgbuf ) ) {
        LOGE("%s", errmsgbuf);
        return;
    }
    D("%s: sent %d bytes to swigps", __func__,written);

    free(tmpbufp);
	at_response_free(atresponse);
}
