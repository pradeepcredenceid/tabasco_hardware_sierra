/*
 *  $Id: racommon.c,v 1.4 2009/11/03 01:08:50 epasheva Exp $
 *
 *  Filename: racommon.c
 *
 *  Purpose:  This package contains functions that are common to the relay
 *            agent service components.
 *
 * Copyright: © 2009 Sierra Wireless, Inc., all rights reserved
 *
 */

/* Linux definitions */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <getopt.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* include files */
#include "SwiDataTypes.h"
#include "SwiRcodes.h"
#include "SwiApiDemux.h"
#include "SwiSerial.h"

#include "ra.h"

extern swi_bool verbose ;
extern pthread_t   RaThreads[RA_MAX_THREADS];

/* Global data */
global struct ractrlblk racb;

char * rasvcname[] =
{
/* SWI_DIAG */ "DIAG",
/* SWI_NMEA */ "NMEA",
/* SWI_AT   */ "AT",
                ""
};
/* Used for sending data to Modem */
swi_uint8 scratchbuf[MAXBUFSIZE];

/* for printing error messages */
char    msg[512];
size_t  start = 0;


/**************
 *
 * Name:     ragetcbp
 *
 * Purpose:  Return pointer to global control block
 *
 * Parms:    None.
 *
 * Return:   None.
 *
 * Notes:    None.
 *
 **************/
 struct ractrlblk *ragetcbp ()
 {
    return (&racb);
 }

/**************
 *
 * Name:     ragetservicecbp
 *
 * Purpose:  Return pointer to service control block
 *
 * Parms:    service     - device service - diagnostics, NMEA, AT
 *
 * Return:   Pointer to the service control block for that service.
 *
 * Notes:    None.
 *
 **************/
 struct raservice *ragetservicecbp (enum raservicetype service)
 {
    struct ractrlblk *racbp = ragetcbp ();
    
    return ((struct raservice *)&racbp->rasvc[service]);
 }

/**************
 *
 * Name:     raserviceinit
 *
 * Purpose:  Initialize the service's control block
 *
 * Parms:    service - enum for service
 *
 * Return:   None.
 *
 * Notes:    None.
 *
 **************/
void raserviceinit(enum raservicetype service)
{
    struct raservice    *rasvcp    = ragetservicecbp( service );

    rasvcp->rasvctype      = service;
    rasvcp->braSerialPort  = FALSE;
    rasvcp->braSocket      = FALSE;
    rasvcp->ratcphostnamep = NULL;
    rasvcp->rasocketp      = NULL;
    rasvcp->rasocket       = BASE_TCP_SOCKET + service;
    rasvcp->rasphostp       = NULL;
    rasvcp->raspmdmp       = NULL;
    rasvcp->rasphostfd      = -1;
    rasvcp->raspmdmfd      = -1;
    rasvcp->rasocketfd     = -1;
    rasvcp->rasocketld     = -1;

    /* Buffer to build a Demux Data Indication Message */
    memset (&rasvcp->ramsg[0], 0, MAXBUFSIZE);
    
    switch (service)
    {
        case RA_DIAG:
            rasvcp->rarcvthrd        = RADIAGRECEIVETHREAD; 
            rasvcp->racomporth2mthrd = RADIAGCOMPORTHOST2MODEMTHREAD;
            rasvcp->racomportm2hthrd = RADIAGCOMPORTMODEM2HOSTTHREAD;
            rasvcp->ratcpsvrh2mthrd  = RADIAGTCPSERVERMODEHOST2MODEMTHREAD;
            rasvcp->ratcpclnth2mthrd = RADIAGTCPCLIENTMODEHOST2MODEMTHREAD;
        break;
        case RA_NMEA:
            rasvcp->rarcvthrd        = RANMEARECEIVETHREAD; 
            rasvcp->racomporth2mthrd = RANMEACOMPORTHOST2MODEMTHREAD;
            rasvcp->racomportm2hthrd = RANMEACOMPORTMODEM2HOSTTHREAD;
            rasvcp->ratcpsvrh2mthrd  = RANMEATCPSERVERMODEHOST2MODEMTHREAD;
            rasvcp->ratcpclnth2mthrd = RANMEATCPCLIENTMODEHOST2MODEMTHREAD;
        break;
        case RA_AT:
            rasvcp->rarcvthrd        = RAATRECEIVETHREAD; 
            rasvcp->racomporth2mthrd = RAATCOMPORTHOST2MODEMTHREAD;
            rasvcp->racomportm2hthrd = RAATCOMPORTMODEM2HOSTTHREAD;
            rasvcp->ratcpsvrh2mthrd  = RAATTCPSERVERMODEHOST2MODEMTHREAD;
            rasvcp->ratcpclnth2mthrd = RAATTCPCLIENTMODEHOST2MODEMTHREAD;
        break;
        default:
        break;
    }
}

/**************
 *
 * Name:     rasettcphost
 *
 * Purpose:  Set TCP hostname and socket in service's control block
 *
 * Parms:    service        - enum of service type
 *           tcphostnamep   - pointer to hostname
 *           socketp        - pointer to socket number string
 *
 * Return:   None.
 *
 * Notes:    None.
 *
 **************/
void rasettcphost(enum raservicetype service, char *tcphostnamep, char *socketp)
{
    struct raservice    *rasvcp    = ragetservicecbp( service );

    rasvcp->ratcphostnamep  = tcphostnamep;
    rasvcp->rasocketp       = socketp;
}

/**************
 *
 * Name:     raservicecleanup
 *
 * Purpose:  Re-Initialize the service's control block and close descriptors
 *
 * Parms:    service - enum of serivce type
 *
 * Return:   None.
 *
 * Notes:    None.
 *
 **************/
void raservicecleanup(enum raservicetype service)
{
    struct raservice    *rasvcp    = ragetservicecbp( service );

    rasvcp->rasvctype      = service;
    rasvcp->braSerialPort  = FALSE;
    rasvcp->braSocket      = FALSE;
    rasvcp->rasphostp      = NULL;
    rasvcp->raspmdmp       = NULL;
    rasvcp->ratcphostnamep = NULL;
    rasvcp->rasocketp      = NULL;

    if (rasvcp->rasphostfd != -1)
        close_dev(rasvcp->rasphostfd, &rasvcp->raoldtioh);

    if (rasvcp->raspmdmfd != -1)
        close_dev(rasvcp->raspmdmfd, &rasvcp->raoldtiom);

    if (rasvcp->rasocketfd != -1)
        close(rasvcp->rasocketfd);
    if (rasvcp->rasocketld != -1)
        close(rasvcp->rasocketld);

    /* Buffer to build a Demux Data Indication Message */
    memset (&rasvcp->ramsg[0], 0, MAXBUFSIZE);
}

/**************
 *
 * Name:     raservicestart
 *
 * Purpose:  Initializes the IPC channel for selected services from the device
 *           and waits for the device to be ready
 *
 * Parms:    service    - service type
 *
 * Return:   None.
 *
 * Notes:    None.
 *
 **************/
void raservicestart(enum raservicetype service)
{
    struct      ractrlblk    *racbp      = ragetcbp ();
    SWI_RCODE   retcode;                        /* Return code from API calls */

    /* Start DIAG device service */
    retcode = SwiApiDxBegin( service );
    if( retcode == SWI_RCODE_OK )
    {
        printf("\n%s service started\n", rasvcname[service]);
    }
    else
    {
        printf("\nError %d, cannot begin the service %s; test aborted\n",
            retcode, rasvcname[service]);
        exit(1);
    }

    /* Wait for modem detection */
    if ( verbose )
    printf("\nWaiting for Air Server - SLEEPING! \n");
    while(racbp->devicepresent  == SWI_OFFLINE)
    {
        sleep(DIAGPAUSE);
    }

}

/**************
 *
 * Name:     raservicestop
 *
 * Purpose:  Stops the given service
 *
 * Parms:    service - enum of service type
 *
 * Return:   None.
 *
 * Notes:    None.
 *
 **************/
void raservicestop(enum raservicetype service)
{

    SWI_RCODE retcode;   /* Return code from API calls */

    /* Give the modem a chance to reply */
    sleep(DIAGPAUSE);

    /* Disable the communication pipe */
    retcode =  SwiApiDxEnd( service );

    if (retcode == SWI_RCODE_OK)
        printf("\n\n%s service stopped\n", rasvcname[service]);


    /* Give the modem a chance to reply */
    sleep(DIAGPAUSE);

}

/**************
 *
 * Name:     rapassthru
 *
 * Purpose:  Pass data received in either direction given the descriptor for the
 *           port
 *
 * Parms:    fd                - file descriptor of write destination
 *           datap             - pointer to data received
 *           datalen           - length of data packet in number of bytes
 *           userp             - user parameter supplied during registartion
 *
 * Return:   number of bytes written, -1 if failed.
 *
 * Notes:    None.
 *
 **************/
 package int rapassthru(
    int fd,
    swi_uint8 *datap,
    swi_uint32 datalen,
    void *usrstrp)
{
    int i, wr;

    /* write to selected port */
    wr=write(fd, (char *)datap, datalen);
    if (verbose)
    {
        if (wr == -1)
        {
            printf ("write failed\n");
        }
        else
        {
            printf ("%s ", (char*)usrstrp);
            for (i=0; i < datalen; i++)
            {
                printf ("0x%02X ", datap[i]);
            }
            printf ("\n");
        }
    }
    return wr;
}

 /**************
 *
 * Name:     rainit
 *
 * Purpose:  Initialize global control block variables.
 *
 * Parms:    None.
 *
 * Return:   None.
 *
 * Notes:    None.
 *
 **************/
 void rainit ()
 {
    struct ractrlblk *racbp = ragetcbp ();

    racbp->devicepresent = SWI_OFFLINE;
 }


/**************
 *
 * Name:     racommpairinit
 *
 * Purpose:  Initialize comm port(s) for either modem and/or host side.
 *
 * Parms:    service - enum of service type
 *           modemp  - pointer to modem serial port name
 *           pcp     - pointer to host serial port name
 *
 * Return:   None.
 *
 * Notes:    None.
 *
 **************/
void racommpairinit (enum raservicetype service, char *modemp, char *pcp)
{
    struct raservice      *rasvcp    = ragetservicecbp(service);

    if (modemp)
    {
        /* Open serial port if enabled. Caller should check for serial port
         * availability
         */
        rasvcp->raspmdmp  = modemp;

        rasvcp->raspmdmfd = open_dev((char *)rasvcp->raspmdmp,
                                &rasvcp->raoldtiom,
                                &rasvcp->ranewtiom);
    }

    if (pcp)
    {
        /* Open serial port if enabled. Caller should check for serial port
         * availability
         */
        rasvcp->braSerialPort = TRUE;
        rasvcp->rasphostp    = pcp;

        rasvcp->rasphostfd = open_dev((char *)rasvcp->rasphostp,
                                &rasvcp->raoldtioh,
                                &rasvcp->ranewtioh);
    }
}

/**************
 *
 * Name:     ractlcallback
 *
 * Purpose:  Callback invoked when Control Indication is received for specific
 *           device service
 *
 * Parms:    deviceservice     - device service - diagnostic or NMEA
 *           ctlindp           - pointer to control data received
 *           userp             - user parameter supplied during registartion
 *
 * Return:   None.
 *
 * Notes:    None.
 *
 **************/
package void ractlcallback(
    swi_uint16 deviceservice,
    swi_uint8 *ctldatap,
    void *userp)
{
    struct ractrlblk *racbp = ragetcbp ();

    /* Get control data */
    racbp->devicepresent = *ctldatap;
    if (racbp->devicepresent == SWI_ONLINE)
        printf("\n\nDemux Ctl Ind callback: device On-Line\n");
    else
        printf("\n\nDemux Ctl Ind callback: device Off-Line\n");
}

/**************
 *
 * Name:     radatacallback
 *
 * Purpose:  Callback invoked when there are data received for specific
 *           device service
 *
 * Parms:    deviceservice     - device service - diagnostic or NMEA
 *           datap             - pointer to data received
 *           datalen           - length of data packet in number of bytes
 *           userp             - user parameter supplied during registartion
 *
 * Return:   None.
 *
 * Notes:    None.
 *
 **************/
package void radatacallback(
    swi_uint16 deviceservice,
    swi_uint8 *datap,
    swi_uint32 datalen,
    void *userp )
{
    struct raservice    *rasvcp    = ragetservicecbp(deviceservice);

    /* Forward if enabled */
    if (rasvcp->braSerialPort)
        rapassthru ( rasvcp->rasphostfd, datap, datalen, userp );

    if (rasvcp->braSocket)
        rapassthru ( rasvcp->rasocketfd, datap, datalen, userp );
}

/**************
 *
 * Name:     rasendtoDevice
 *
 * Purpose:  This function attempts to send user specified data buffer
 *           to the  device.
 *
 * Parms:    dataservice - service whose data needs to be sent
 *           datatosendp - pointer to data to send to device
 *           datalen     - length of data to send
 *
 * Return:  TRUE  - send successful
 *          FALSE - device offline ?
 *
 * Notes:    This function assumes data to send are less than the maximum
 *           bytes defined for the scratch buffer..
 *
 **************/
swi_bool rasendtoDevice(
    swi_uint16 dataservice,
    swi_uint8 *datatosendp,
    swi_uint32 datalen)
{
    SWI_RCODE   retcode = SWI_RCODE_OK; /* Return code from API calls */
    swi_uint8   *datap = NULL;          /* Pointer to test data to send to
                                         * diagnostic agent in modem
                                         */
    swi_uint8   *payloadp = NULL;       /* Pointer to payload data to send to
                                         * diagnostic agent in modem
                                         */
    swi_uint32  payloadlen = 0;         /* Length of payload pointed to by
                                         * payloadp
                                         */
    swi_uint32  bytesoffset = 0;        /* Offset in buffer where can write our
                                         * payload leaving room for header
                                         */
    swi_uint32  i = 0;                  /* Index variable */


    /* Clear the scratch buffer */
    memset((char *)&scratchbuf[0], 0, MAXBUFSIZE );

    /* Obtain pointer to the payload section of data packet */
    retcode = SwiGetDataPldOffset( &scratchbuf[0],
                                   datalen,
                                   &payloadp,
                                   &bytesoffset);

    if (retcode == SWI_RCODE_OK)
    {
        /* Build the message */
        datap = datatosendp;
        for( i=0; i < datalen; i++ )
        {
            /* Copy data */
            *payloadp++ = *datap++;
        }

        /* Adjust the length to include the header */
        payloadlen = datalen+bytesoffset;

        /* Send Data Indication to SDK */
        retcode = SwiApiDxSend(dataservice, &scratchbuf[0], payloadlen);
    }
    else
    {
        /* Indicate that Data Indication packet cannot be built due to
         * insufficient memory allocated by the caller
         */
        if (verbose)
            printf("\nData packet cannot be built - no memory %s\n",
                    rasvcname[dataservice]);
    }
    if (retcode != SWI_RCODE_OK)
        return FALSE;
    
    return TRUE;
}

/**************
 *
 * Name:     doRaRegisterService
 *
 * Purpose:  This function registers our call back functions for control and 
 *           data indication from the modem.
 *
 * Parms:    deviceservice : device service requested - diagnostics, NMEA, AT                        
 *           datacbfp      : pointer to user callback function for Demux Data
 *           ctlcbfp       : pointer to user callback for control indication
 * *
 * Return:   none
 *
 * Notes:    This thread never returns.
 *           Ctrl-C  interrupts the test.
 *           Actions:
 *              register a callback function,
 *              wait for Demux Indications
 *
 *           This thread is NOT allowed to initiate stop-and-wait requests under
 *           any circumstances
 *
 **************/
package void doRaRegisterService(
    swi_uint16 deviceservice,
    void (*datacbfp)(swi_uint16 deviceservice, swi_uint8 *datap,
        swi_uint32 datalen, void *userp),
    void (*ctlcbfp)(swi_uint16 deviceservice, swi_uint8 *ctldatap,
        void *userp)
)
{

    SWI_RCODE retcode;          /* Return code from API calls */
    void *datauserp="DataInd";  /* Optional callback argument for data traffic
                                 * from modem */
    void    *ctluserp="CtlInd"; /* Optional callback argument for status traffic
                                 * from modem */

    /* Register for given device service */
    retcode =  SwiApiDxRegister(deviceservice,
                                datacbfp, datauserp,
                                ctlcbfp, ctluserp );
    if( verbose )
    {
        switch( retcode )
        {
            case SWI_RCODE_OK:
                printf("\n%s: registered the following callbacks:\n",
                        rasvcname[deviceservice]);
                printf("%s: Data Callback: 0x%x, Data User Parameter: %s\n",
                        rasvcname[deviceservice],
                        (unsigned int)datacbfp,
                        (char *)datauserp);
                printf("%s: Ctl  Callback: 0x%x, Ctl  User Parameter: %s\n",
                        rasvcname[deviceservice],
                        (unsigned int)ctlcbfp,
                        (char *)ctluserp);
                break;

            case SWI_RCODE_FAILED:
            case SWI_RCODE_SYSTEM_ERROR:
            case SWI_RCODE_BUSY:
                printf("\n\n%s: registration failed %d\n",
                        rasvcname[deviceservice], retcode );
                exit( 1 );
                break;

            case SWI_RCODE_API_NOT_OPEN:
                printf("\n\n%s: Registration failed, API not opened %d\n",
                        rasvcname[deviceservice],
                        retcode );
                exit( 1 );
                break;

            case SWI_RCODE_DEVICE_INUSE:
                printf("\n\n%s: Registration failed, channel in use %d\n",
                        rasvcname[deviceservice],
                        retcode );
                exit( 1 );
                break;

            default:
                printf("\n\n%s: Unrecognized error: %d\n",
                        rasvcname[deviceservice],
                        retcode );
                exit( 1 );
                break;
        }
    }
}

/*
 * Threads below here
 */

/**************
 *
 * Name:     doRaReceiveThread
 *
 * Purpose:  This is the thread function started by the main program to prepare
 *           Dx Receive from the modem in a separate thread.
 *
 * Parms:    svcp    - this is used to pass in the service type here.
 *
 * Return:   none
 *
 * Notes:    This thread never returns.
 *           Ctrl-C  interrupts the test.
 *           Actions:
 *              register a callback function,
 *              wait for Demux Indications
 *
 *           This thread is NOT allowed to initiate stop-and-wait requests under
 *           any circumstances
 *
 **************/
package void* doRaReceiveThread( void *svcp )
{
    SWI_RCODE   retcode; /* Return code from API calls */
    /* 
     * The pthread_create interface requires a (void *), so we cast it back 
     * to our data type.
     */
    swi_uint16  *servicep = (swi_uint16 *)svcp;

    /* Block waiting for data from the modem */
    while(TRUE)
    {
        /* Invoke receive handler for Diagnostic device service */
        retcode = SwiWaitDxIndication( (swi_uint16)*servicep);
        if( retcode != SWI_RCODE_OK )
        {
            if ( verbose )
                printf("\nSwiWaitDxIndication returned error: %d\n", retcode );
        }
    }
    return (void*)NULL;
}

/**************
 *
 * Name:     doRaTCPServerModeHost2ModemThread
 *
 * Purpose:  This is the thread function started by the main program to handle
 *           Dx Receive from the external host via a TCP socket in a separate
 *           thread in TCP server mode.
 *
 * Parms:    svcp    - pointer to service's control block
 *
 * Return:   none
 *
 * Notes:    This thread never returns.
 *           Ctrl-C  interrupts the test.
 *           Actions:
 *              reads inputs from external port.
 *              forward to the modem
 *
 *           This thread is NOT allowed to initiate stop-and-wait requests under
 *           any circumstances.
 *           Data from other sources will need to be handled in their own thread
 *
 **************/
package void * doRaTCPServerModeHost2ModemThread(void *svcp)
{
    struct  raservice      *rasvcp = (struct raservice *)svcp;
    int     i, sd, ld, rc;
    struct  sockaddr_in name;
    struct  sockaddr_in addr;
    struct  sockaddr_in *ptr;
    struct  hostent *hp;
    int     nbytes;
    int     delay = 0;
    unsigned int addrlen;

    memset ((char *)&name, 0, sizeof (name));

    /* Obtain our LISTEN socket */
    if ( (ld = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror ("INET Domain socket");
        exit(1);
    }

    /* Use user provided port number if available */
    if (rasvcp->rasocketp)
        rasvcp->rasocket = atoi (rasvcp->rasocketp);

    name.sin_family = AF_INET;
    name.sin_port = htons(rasvcp->rasocket);
    name.sin_addr.s_addr = INADDR_ANY;

    /* If the program is killed or stopped, it takes a while for the ports to
     * be freed up by the system. Let's loop until it's available or failed
     * completely, in which case, the reap time may need to be longer
     */
    printf ("\nAttempting to bind socket to our local address...\n");
    for (;;)
    {
        if ( bind (ld, (struct sockaddr *)&name, sizeof (name)) >= 0)
            break;

        if (errno != EADDRINUSE)    /* maybe called too many times? */
        {
            close (ld);
            perror ("INET Domain bind");
            exit (2);
        }

        if (delay >= SWI_SOCKET_REAP_MS)
            break;    /* already running */

        /* linux takes a while to reap sockets so retry after SWI_RETRY_MS */

        sleep (1);
        delay += SWI_RETRY_MS*2;
    }

    addrlen = sizeof (name);
    if ( (rc = getsockname (ld, (struct sockaddr *)&name, &addrlen)) < 0)
    {
        close (ld);
        printf ("\ngetsockname return code = %d\n", rc);
        perror ("INET Domain getsockname");
        exit (3);
    }

    ptr = (struct sockaddr_in *) &name;
    printf ("\n\nSocket has port number: %d\n\n", htons(ptr->sin_port));
    fflush (stdout);
    while (TRUE)
    {
        if (listen (ld, 100) < 0)
        {
            close (ld);
            perror ("INET Domain Listen");
            exit (4);
        }
        
        printf ("\n Listening for connection\n");

        /* We wait for a connection to happen */
        memset ((char *)&addr, 0, sizeof (addr));
        addrlen = sizeof (addr);
        if ( (sd = accept (ld, (struct sockaddr *)&addr, &addrlen) ) < 0)
        {
            close (ld);
            perror ("INET Domain accept()");
            exit (5);
        }
        printf ("\n Accepted connection\n");

        rasvcp->braSocket = TRUE;
        rasvcp->rasocketfd = sd;
        rasvcp->rasocketld = ld;

        ptr = (struct sockaddr_in *) &addr;
        printf ("\n\nCalling socket from host %s has port number: %d\n\n",
        inet_ntoa(ptr->sin_addr), ptr->sin_port);
        fflush (stdout);
        hp = gethostbyaddr (&ptr->sin_addr, sizeof (struct in_addr), AF_INET);
        if (hp != NULL)
        {
            printf ("\nFrom host:%s\n\tWith aliases: ", hp->h_name);
            while (*hp->h_aliases)
                printf ("\n\t\t\t%s", *hp->h_aliases++);
            printf ("\n\n");
        }
        else
        {
            perror ("\n\tgethostbyaddr failed");
            printf ("errno is %d \n\n", errno);
        }

        while (TRUE)
        {
            fflush (stdout);
            /* Someone has connected to us, just read and forward anything from
             * them to the modem
             */
            do
            {
                pthread_testcancel();
                if ( (nbytes = read (sd, &rasvcp->ramsg[0], 
                                     sizeof (rasvcp->ramsg))) < 0)
                {
                    snprintf(msg, sizeof(msg), "%d read on sd: %d",nbytes, sd);
                    printf ("%s", msg);
                    close (sd);
                    perror ("INET Domain read");
                    exit (6);
                }
                else
                {
                    if (nbytes != 0)
                    {
                        pthread_testcancel();
                        /* If connection to modem is via serial port use it */
                        if (rasvcp->raspmdmp)
                        {
                            snprintf(msg, sizeof(msg),
                                    "%s TCP Server Host to Modem via com port:",
                                    rasvcname[rasvcp->rasvctype]);
                            rapassthru (rasvcp->raspmdmfd,
                                        (swi_uint8 *)&rasvcp->ramsg[0],
                                        nbytes,
                                        msg);
                        }
                        else
                        {   /* Demux channel route */
                            rasendtoDevice(rasvcp->rasvctype,
                                            (swi_uint8 *)&rasvcp->ramsg[0],
                                            nbytes);
                        }
                        if (verbose)
                        {
                            printf ("%s TCP server from Host: ",
                                    rasvcname[rasvcp->rasvctype]);
                            for(i=0; i < nbytes; i++)
                            {
                                printf ("0x%02X ", rasvcp->ramsg[i] & 0xFF);
                            }
                            printf ("\n");
                        }
                    }
                    else 
                    {
                        /* Most likely the connection was broken by client */
                        break;
                    }
                }
            } while (nbytes != 0);
            /* Assume connection broken, go listen for clients again */
            if (nbytes == 0) 
            {
                printf ("Closing socket, remote side disconnected?\n");
                close (sd);
                break;
            }
        }
    }
}

/**************
 *
 * Name:     doRaTCPClientModeHost2ModemThread
 *
 * Purpose:  This is the thread function started by the main program to handle
 *           Dx Receive from the external host via a TCP socket in a separate
 *           thread in TCP client mode.
 *
 * Parms:    svcp    - pointer to service control block
 *
 * Return:   none
 *
 * Notes:    This thread never returns.
 *           Ctrl-C  interrupts the test.
 *           Actions:
 *              reads inputs from external port.
 *              forward to the modem
 *
 *           This thread is NOT allowed to initiate stop-and-wait requests under
 *           any circumstances.
 *           Data from other sources will need to be handled in their own thread
 *
 **************/
package void * doRaTCPClientModeHost2ModemThread(void *svcp)
{
    struct  raservice  *rasvcp = (struct raservice *)svcp;
    struct  sockaddr_in  name;
    struct  hostent      *hp;
    int     nbytes;
    int     i, sd;
    char    msg[512];

    memset ((char *)&name, 0, sizeof (name));

    /* Obtain a socket */
    if ( (sd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror ("INET Domain socket");
        exit(1);
    }

    /* Use user provided port number if available */
    if (rasvcp->rasocketp)
        rasvcp->rasocket = atoi (rasvcp->rasocketp);

    /* Use user provided host name to connect  */
    name.sin_family = AF_INET;
    name.sin_port = htons(rasvcp->rasocket);

    hp = gethostbyname (rasvcp->ratcphostnamep);
    if (!hp)
    {
        perror ("\n\tgethostbyname failed");
        exit (3);
    }
    bcopy (hp->h_addr, &name.sin_addr.s_addr, hp->h_length);

    if (connect (sd, (struct sockaddr *)&name, sizeof (name)) < 0)
    {
        perror ("INET Server connect");
        exit (2);
    }
    printf ("\nTCP Server: %s, port: %d connected for %s\n",
            rasvcp->ratcphostnamep,
            rasvcp->rasocket,
            rasvcname[rasvcp->rasvctype]);

    rasvcp->braSocket = TRUE;
    rasvcp->rasocketfd = sd;

    while (TRUE)
    {
        do
        {
            /* Read from Host */
            memset (rasvcp->ramsg, 0, sizeof(rasvcp->ramsg));
            pthread_testcancel();
            if ( (nbytes = read (sd, rasvcp->ramsg, sizeof(rasvcp->ramsg))) < 0)
            {
                close (sd);
                perror ("INET Domain read");
                exit (6);
            }
            else
            {   
                if (nbytes != 0)
                {
                    pthread_testcancel();
                    /* Forward to Modem */
                    /* If the connection to the modem is via serial port use it */
                    if (rasvcp->raspmdmp)
                    {
                            snprintf(msg, sizeof(msg),
                                    "%s TCP client Host to Modem via com port:",
                                    rasvcname[rasvcp->rasvctype]);
                            rapassthru (rasvcp->raspmdmfd,
                                        (swi_uint8 *)&rasvcp->ramsg[0],
                                        nbytes,
                                        msg);
                     }
                    else
                    {   /* Demux channel route */
                        if (!rasendtoDevice(rasvcp->rasvctype,
                                    (swi_uint8 *)&rasvcp->ramsg[0],
                                    nbytes))
                        {
                            if (verbose)
                            {
                              printf("rasendtoDevice failed, device offline?\n");
                            }
                        }
                    }
                    if (verbose)
                    {
                        printf ("%s TCP client from Host: ",
                                rasvcname[rasvcp->rasvctype]);
                        for(i=0; i < nbytes; i++)
                        {
                            printf ("0x%02X ", rasvcp->ramsg[i] & 0xFF);
                        }
                        printf ("\n");
                    }
                }
            }
        } while (nbytes != 0);
    }
}

/**************
 *
 * Name:     doRaComPortHost2ModemThread
 *
 * Purpose:  This is the thread function started by the main program to handle
 *           Dx Receive from the external host via a serial port
 *           in a separate thread.
 *
 * Parms:    svcp    - pointer to service control block
 *
 * Return:   none
 *
 * Notes:    This thread never returns.
 *           Ctrl-C  interrupts the test.
 *           Actions:
 *              reads inputs from external port.
 *              forward to the modem
 *
 *           This thread is NOT allowed to initiate stop-and-wait requests under
 *           any circumstances.
 *           Data from other sources will need to be handled in their own thread
 *
 **************/
package void * doRaComPortHost2ModemThread(void *svcp)
{
    struct      raservice      *rasvcp = (struct raservice *)svcp;;
    int         nr;
    int         hostfd, mdmfd;
    swi_uint16  service;
    char        *svcnamep;
    char        msg[512];

    /* Make local copies */
    hostfd   = rasvcp->rasphostfd;
    mdmfd    = rasvcp->raspmdmfd;
    svcnamep = rasvcname[rasvcp->rasvctype];
    service  = rasvcp->rasvctype;

    snprintf(msg, sizeof(msg),
            "\n%s Com Port Host to Modem Thread started on %s, fd: %d\n",
            svcnamep,
            rasvcp->rasphostp,
            rasvcp->rasphostfd);

    printf ("%s", msg);

   /* Read from the host and send to the modem. */

    while (TRUE)
    {
        memset (&rasvcp->ramsg[0], 0, sizeof(rasvcp->ramsg));
        pthread_testcancel();
        nr = read (hostfd, &rasvcp->ramsg[0], sizeof(rasvcp->ramsg));
        if (nr != -1)
        {
            if (nr != 0)
            {
                pthread_testcancel();
                if (rasvcp->raspmdmp)
                {
                        snprintf(msg, 
                                 sizeof(msg), 
                                 "%s Com port Host to Modem:",
                                 svcnamep);
                        rapassthru (mdmfd, (swi_uint8 *)&rasvcp->ramsg[0], nr, msg);
                }
                else
                {
                    rasendtoDevice(service, (swi_uint8 *)&rasvcp->ramsg[0], nr);
                }
            }
        }
        else
        {
            perror ("Reading from host failed");
            exit (2);
        }
    }
    return (void*)NULL;
}

/* For testing only  */
/**************
 *
 * Name:     doRaComPortModem2HostThread
 *
 * Purpose:  This is the thread function started by the main program to handle
 *           Dx Receive from the modem via a serial port to the host
 *           in a separate thread.
 *
 * Parms:    scvp    - pointer to service control block
 *
 * Return:   none
 *
 * Notes:    This thread never returns.
 *           Ctrl-C  interrupts the test.
 *           Actions:
 *              reads inputs from external port.
 *              forward to the modem
 *
 *           This thread is NOT allowed to initiate stop-and-wait requests under
 *           any circumstances.
 *           Data from other sources will need to be handled in their own thread
 *
 **************/
package void * doRaComPortModem2HostThread(void *svcp)
{
    struct  raservice      *rasvcp = (struct raservice *)svcp;;
    int     nr;
    int     hostfd, mdmfd;
    char    *svcnamep;
    char    msg[512];

    /* Make local copies */
    hostfd = rasvcp->rasphostfd; /* Assume host is on another serial port */
    mdmfd  = rasvcp->raspmdmfd;
    svcnamep = rasvcname[rasvcp->rasvctype];

    printf ("\n%s Com Port Modem to Host Thread started on %s, fd: %d\n",
            svcnamep,
            rasvcp->raspmdmp,
            rasvcp->raspmdmfd);

    /* Read from the modem and send to the external port.
     * We check for a host descriptor in the loop in case we have a TCP
     * connection to the host, which may not have happened when we started this
     * thread
     */

    while (TRUE)
    {
        /* If the host is sitting on a socket, we better hook it up */
        if (rasvcp->braSocket)
            hostfd = rasvcp->rasocketfd;

        memset (&rasvcp->ramsg[0], 0, sizeof(rasvcp->ramsg));
        pthread_testcancel();
        nr = read (mdmfd, &rasvcp->ramsg[0], sizeof(rasvcp->ramsg));
        pthread_testcancel();
        if (nr != -1)
        {
            if (nr != 0)
            {
              snprintf(msg, sizeof(msg), "%s Com port Modem to Host:",svcnamep);
              rapassthru (hostfd, (swi_uint8 *)&rasvcp->ramsg[0], nr, msg);
            }
        }
        else
        {
            perror ("Reading from modem failed");
            exit (2);
        }
    }
    return (void*)NULL;
}

/*
 * $Log: racommon.c,v $
 * Revision 1.4  2009/11/03 01:08:50  epasheva
 * BZ 19433 - fixed a typo in function header
 *
 * Revision 1.3  2009/04/30 23:08:43  blee
 * BUG #15847
 * Relay Agent direct interface using serial not working for DIAG service.
 * Re-vamped command-line options. Combine Service thread start into
 * one common function.
 *
 * Revision 1.2  2009/03/30 22:20:35  blee
 * Fixed header description for doRaRegisterService()
 *
 * Revision 1.1  2009/03/30 21:59:51  blee
 * BUG # 15436
 * Initial check-in of Relay Agent for Diagnostics, NMEA and AT services
 *
 *
 */
