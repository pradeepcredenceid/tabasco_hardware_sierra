/*
 *  $Id: ra.h,v 1.4 2009/11/03 01:09:17 epasheva Exp $
 *
 *  Filename: ra.h
 *
 *  Purpose:  This file contains defines, data structures and forward
 *            declarations of the relay agent utility.
 *
 * Copyright: © 2009 Sierra Wireless, Inc., all rights reserved
 *
 */
#include <termios.h>
#include "SwiApiDemux.h"

/* Internal Definitions */

/* Maximum buffer size for data indication message */
#define MAXBUFSIZE          LENGTH_DxData

/* Default timeout value */
#define DIAGDEFAULTTIMEOUT  3000

/* Pause value */
/* Note: This delay is needed to be able to log properly the modem responses
 *       to all commands
 */
#define DIAGPAUSE           1

/* Default port base, service number will be added for the final port number  */
#define BASE_TCP_SOCKET     33600

#define SWI_SOCKET_REAP_MS  30000        /* socket reap time after exit */
#define SWI_RETRY_MS        100          /* milliseconds */

/* global data */

/*
 * Name:    rathread
 *
 * Purpose: enum for threads - names are self-explanatory
 *
 * Members:     RADIAGRECEIVETHREAD
 *              RADIAGCOMPORTHOST2MODEMTHREAD
 *              RADIAGCOMPORTMODEM2HOSTTHREAD
 *              RADIAGTCPSERVERMODEHOST2MODEMTHREAD
 *              RADIAGTCPCLIENTMODEHOST2MODEMTHREAD
 *              RANMEARECEIVETHREAD
 *              RANMEACOMPORTHOST2MODEMTHREAD
 *              RANMEACOMPORTMODEM2HOSTTHREAD
 *              RANMEATCPCLIENTMODEHOST2MODEMTHREAD
 *              RANMEATCPSERVERMODEHOST2MODEMTHREAD
 *              RAATRECEIVETHREAD
 *              RAATCOMPORTHOST2MODEMTHREAD      
 *              RAATCOMPORTMODEM2HOSTTHREAD     
 *              RAATTCPSERVERMODEHOST2MODEMTHREAD
 *              RAATTCPCLIENTMODEHOST2MODEMTHREAD  
 * 
 * Note:    None.
 */
enum rathread
{
   RA_MIN_THREADS = 0,
   RADIAGRECEIVETHREAD                  = RA_MIN_THREADS,              
   RADIAGCOMPORTHOST2MODEMTHREAD,       
   RADIAGCOMPORTMODEM2HOSTTHREAD,       
   RADIAGTCPSERVERMODEHOST2MODEMTHREAD, 
   RADIAGTCPCLIENTMODEHOST2MODEMTHREAD, 
   RANMEARECEIVETHREAD,               
   RANMEACOMPORTHOST2MODEMTHREAD ,  
   RANMEACOMPORTMODEM2HOSTTHREAD,      
   RANMEATCPCLIENTMODEHOST2MODEMTHREAD,
   RANMEATCPSERVERMODEHOST2MODEMTHREAD, 
   RAATRECEIVETHREAD, 
   RAATCOMPORTHOST2MODEMTHREAD,       
   RAATCOMPORTMODEM2HOSTTHREAD,       
   RAATTCPSERVERMODEHOST2MODEMTHREAD, 
   RAATTCPCLIENTMODEHOST2MODEMTHREAD, 
   RA_MAX_THREADS
};

/*
 * Name:    raservicetype
 *
 * Purpose: enum for services
 *
 * Members: RA_DIAG - diagnostic
 *          RA_NMEA - NMEA
 *          RA_AT   - AT command -sevice not implemted in DR/DS yet
 *
 * Note:    None.
 */
enum raservicetype
{
    RA_SVC_MIN = SWI_DIAG,
    RA_DIAG = SWI_DIAG,
    RA_NMEA = SWI_NMEA,
    RA_AT,
    RA_SVC_MAX
};

/*
 * Name:    raservice
 *
 * Purpose: Structure for services of the Relay Agent
 *
 * Members: rasvctype     - Service type
 *          braSerialPort - flag for Serial Port
 *          braSocket     - flag for Socket
 *          ratcphostnamep- pointer to TCP host name string
 *          rasocketp     - pointer to TCP host socket port number string
 *          rasocket      - int value  TCP host socket port number
 *          rasphostfd    - file descriptor for serial host port
 *          raspmdmfd     - file descriptor for serial modem port
 *          rasocketfd    - file descriptor for TCP socket
 *          rasocketld    - file descriptor for TCP listen socket
 *          rasphostp     - pointer to host serial port name string
 *          raoldtioh     - structure to save serial port config info host
 *          ranewtioh     - structure for new serial port config info host
 *          raspmdmp      - pointer to modem serial port name string
 *          raoldtiom     - structure to save serial port config info modem
 *          ranewtiom     - structure for new serial port config info modem
 *          ramsg         - message buffer
 *
 * Note:    None
 */
struct raservice
{
    swi_uint16  rasvctype;
    swi_bool    rasvcon;
    swi_bool    braSerialPort;
    swi_bool    braSocket;
    char        *ratcphostnamep;
    char        *rasocketp;
    int         rasocket;
    int         rasphostfd;
    int         raspmdmfd;
    int         rasocketfd;
    int         rasocketld;
    char        *rasphostp;
    struct      termios raoldtioh;
    struct      termios ranewtioh;
    char        *raspmdmp;
    struct      termios raoldtiom;
    struct      termios ranewtiom;
    swi_uint8   ramsg[MAXBUFSIZE];
    enum rathread rarcvthrd;
    enum rathread racomporth2mthrd;
    enum rathread racomportm2hthrd;
    enum rathread ratcpsvrh2mthrd;
    enum rathread ratcpclnth2mthrd;
};

/*
 * Name:    ractrlblk
 *
 * Purpose: Structure for Control block of the Relay Agent
 *
 * Members: devicepresent - Device on/off flag
 *          rasvc         - array of services supported - DIAG, NMEA & AT.
    -
 *
 * Note:    None
 */
struct ractrlblk
{
    swi_uint8 devicepresent;
    struct raservice rasvc[RA_SVC_MAX];
};

/* forward declarations */

/* racommon.c */
extern struct ractrlblk *ragetcbp ();

extern struct raservice *ragetservicecbp (enum raservicetype service);

extern void rainit (void);
extern void raserviceinit(enum raservicetype service);
extern void raservicestart(enum raservicetype service);
extern void raservicestop(enum raservicetype service);

extern void ractlcallback(
    swi_uint16 deviceservice,
    swi_uint8 *ctldatap,
    void *userp);

extern void radatacallback(
    swi_uint16 deviceservice,
    swi_uint8 *datap,
    swi_uint32 datalen,
    void *userp );

extern swi_bool rasendtoDevice(
    swi_uint16 dataservice,
    swi_uint8 *datatosendp,
    swi_uint32 datalen);

extern void doRaRegisterService(
    swi_uint16 deviceservice,
    void (*datacbfp)(swi_uint16 deviceservice, swi_uint8 *datap,
        swi_uint32 datalen, void *userp),
    void (*ctlcbfp)(swi_uint16 deviceservice, swi_uint8 *ctldatap,
        void *userp) );

extern void * doRaReceiveThread( void *dummyp );

extern int rapassthru (
    int fd,
    swi_uint8 *datap,
    swi_uint32 datalen,
    void *userp);

extern void raservicecleanup(enum raservicetype service);
extern void racommpairinit (
    enum raservicetype service,
    char *modemp,
    char *pcp);

extern void rasettcphost(
    enum raservicetype service,
    char *tcphostnamep,
    char *socketp);

extern void * doRaComPortModem2HostThread(void *svcp);
extern void * doRaComPortHost2ModemThread(void *svcp);
extern void * doRaTCPClientModeHost2ModemThread(void *svcp);
extern void * doRaTCPServerModeHost2ModemThread(void *svcp);

/*
 * $Log: ra.h,v $
 * Revision 1.4  2009/11/03 01:09:17  epasheva
 * BZ19433
 *
 * Revision 1.3  2009/04/30 23:08:43  blee
 * BUG #15847
 * Relay Agent direct interface using serial not working for DIAG service.
 * Re-vamped command-line options. Combine Service thread start into
 * one common function.
 *
 * Revision 1.2  2009/04/21 18:38:33  rfiler
 * Added RADIAGCOMPORTMODEM2HOSTTHREAD
 *
 * Revision 1.1  2009/03/30 21:59:51  blee
 * BUG # 15436
 * Initial check-in of Relay Agent for Diagnostics, NMEA and AT services
 *
 *
 */
