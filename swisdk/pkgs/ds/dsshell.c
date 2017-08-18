/*************
 *
 * $Id: dsshell.c,v 1.14 2010/06/09 23:39:03 epasheva Exp $
 *
 * Filename: dsshell.c
 *
 * Purpose:  DS package shell task and associated functions
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* include files */
#include "ds/dsidefs.h"


/* Local storage */
/* Array of task control blocks one for each channel */
struct dsipccb dsshellcb[CIMAXIPCCHAN];
struct dsuscb dsepcbk[CIMAXIPCCHAN];

/*************
 *
 * Name: dsgetepcbp
 *
 * Purpose: Function to get a pointer to the Endpoint
 *          control block structure.
 *             
 * Parms:   index       - entry in the configuration structure
 *
 * Return:  pointer to struct dsuscb
 *
 * Abort:   None
 *
 * Notes:   None
 *
 **************/
package struct dsuscb *dsgetepcbp( swi_uint16  index )
{
    return &(dsepcbk[index]);
}

/*************
 *
 * Name: dsgetipccbp
 *
 * Purpose: Function to get a pointer to  IPC channel
 *          control block structure.
 *             
 * Parms:   chan - channel index
 *
 * Return:  pointer to struct dsipctcb
 *
 * Abort:   none
 *
 * Notes:   None
 *
 **************/
package struct dsipccb *dsgetipccbp( swi_uint16  chan )
{
    return &(dsshellcb[chan]);
}

/*************
 *
 * Name: dsinit
 *
 * Purpose: Initialize internal variables within the DS package
 *          at SDK startup
 *             
 * Parms:   none
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
global void dsinit( void )
{
    /* Nothing for now */
}

/*************
 *
 * Name: dsrxinit
 *
 * Purpose: To create a memory pool and logging name for this endpoint channel
 *             
 * Parms:   index       - entry in the configuration structure
 *
 * Return:  None
 *
 * Abort:   None
 *
 * Notes:   None
 *
 **************/
package void dsrxinit( swi_uint16 index)
{
    struct dsuscb *dsuscbp = dsgetepcbp(index);

    /* create a memory pool for receive buffer allocation */
    mmpcreate( &dsuscbp->dsbufpool, "dsRxPool", DSRXPOOLSZ, DSEPBUFSZ );

    /* Register with the logger process */
    dlregister((char *)cigetchannelname(index) , &dsuscbp->dsusdldcb, FALSE);

    dsuscbp->dsrxpktsdropped = 0;
}

/*************
 *
 * Name: dsshell_ipc
 *
 * Purpose: Implements the DS shell instance for communicating with IPC channels
 *          This DS shell instance initializes and opens a particular 
 *          channel as specified by the calling parameters,
 *          blocks waiting for incoming IPC messages from that channel
 *          and invokes a receive handler for each message received.
 *             
 * Parms:   index      - entry in the configuration structure
 *          owneridp   - string with the device service name 
 *          devicetype - IPC channel
 *          chanid     - IC defined channel
 *          dsrcvcbckp - callback on message receive function pointer with the
 *                       following input parameters:
 *              inipcmsgp - pointer to beginning of received message 
 *              inmsglen  - length of received message
 *              memfreep  - pointer to memory to be released by callback
 *              ipcchannel  -IPC channel used for this service
 *
 * Return:  never returns
 *
 * Abort:   none
 *
 * Notes:   This task blocks waiting for inbound Packets on 
 *          the IPC channel from the APIs. This task is a parameterized version 
 *          of a DS thread and is called by a wrapper function from the package 
 *          that calls it during the task creation process.
 **************/
local void dsshell_ipc (
    swi_uint8 *owneridp,
    void *dsrcvcbckp( swi_uint8 *inipcmsgp, swi_uint32 inmsglen,
        swi_uint8 *memfreep, swi_uint16 ipcchannel ))
{
    swi_uint32 loop;                  /* Helps force log messages to print */
    struct dsipccb *dsipccbp = NULL;  /* local pointer of task control block */
    struct icipccb *ipccbp;           /* pointer to initialized channel
                                       * structure 
                                       */
    swi_uint32 ipcbuflen;             /* contains length of the received data */
    swi_bool result = FALSE;
    swi_uint8 *inipcmsgp;             /* Storage for incoming IPC packet buffer
                                       * pointer 
                                       */
    swi_uint8 *ipcmemfreep;           /* pointer to start of incoming IPC
                                       * packet pointer 
                                       */
    swi_uint16 chanid;
    
    /* Initialize the IPC Handle needed by this DS shell instance
     * Note: Initialization for IC package IPC component 
     * is done by am package;
     */
    chanid = cigetchannelnum((char *)owneridp);
    
    /* Define and allocate the task control block */
    dsipccbp = dsgetipccbp(chanid);
    
    /* Initialize this package's ability to generate logs, initially enabled */
    dlregister( (char *)owneridp, &dsipccbp->dsdlcb, FALSE );
 
    /* Install receive handler for that particular channel */
    dsipccbp->dsrcvcbckp = (void *)dsrcvcbckp;
    
    /* Set the IPC channel pointer */
    ipccbp =  cigetipcp(chanid);
    
    /* Create the IPC Pool used for exchanging traffic with the 
     * IPC layer 
     */
    mmpcreate( &dsipccbp->dsbufpool, 
                dsipccbp->dsdlcb.dlpkgnamep, DSIPCPOOLSZ, DSIPCBUFSZ );

    /* Tell the world we've started up */
    dlLog(  &dsipccbp->dsdlcb, DSLOG_CLASSC, 
            "IPC Task started\n", 
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL);
    
    /* Main line of processing begins next */
    for(loop=0;;loop++)
    {
        /* Every pass through here the buffer length argument
         * and the message pointer need to be reset because 
         * they are modified during processing, below
         */
        ipcbuflen = DSIPCBUFSZ;

        /* allocate a buffer to store the next incoming IPC messages */
        ipcmemfreep = (swi_uint8 *) mmbufget( &dsipccbp->dsbufpool );

        /* Ensure we actually got one or abort */
        if( !ipcmemfreep )
        {
            /* Abort the SDK - this should never happen */
            erAbort("dsshell instance : No event blk memory", 0 );
        }

        /* initially the two pointers are the same */
        inipcmsgp = ipcmemfreep;
        
        /* Wait for incoming requests from the application via the IPC channel. 
         * Note: ipcbuflng contains the number of bytes in the ipc message
         */
        result = icsdk_rcv( ipccbp, 
                            inipcmsgp, 
                            &ipcbuflen,  
                            DSIPCTIMEOUT );
        /* Invoke the installed receive handler  on succesful receive */
        if(result && (inipcmsgp))
        {
            /* On data receive call callback on receive */
            /* Note: The buffer is released by the callback */
            dsipccbp->dsrcvcbckp(inipcmsgp, ipcbuflen, ipcmemfreep, chanid);
        }
        else
        {
            /* Release the buffer and return to the top */
            mmbufrel( ipcmemfreep );
        }
    }
}

/*************
 *
 * Name: dsshell_ep
 *
 * Purpose: Implements the DS Shell instance for communicating with an Endpoint.
 *          This DS shell instance initializes and opens a particular 
 *          endpoint as specified by the calling parameters, and
 *          blocks waiting for incoming modem messages from that endpoint.
 *             
 * Parms:   index      - entry in the configuration structure
 *          chanid     - IC defined channel or endpoint number
 *          dsrcvcbckp - callback on message receive function pointer with the
 *                       following input parameters:
 *              inipcmsgp - pointer to beginning of received message 
 *              inmsglen  - length of received message
 *              memfreep  - pointer to memory to be released by callback
 *              ipcchannel- IPC channel used for this service - not used here
 *          
 * Return:  never returns
 *
 * Abort:   none
 *
 * Notes:   This task blocks waiting for inbound Packets on the Endpoint from 
 *          the Modem. This task is a parameterized version of a DS thread and
 *          is called by a wrapper function from the package that calls it
 *          during the task creation process.
 *
 **************/
local void dsshell_ep (
    swi_uint8 *owneridp,
    void *dsrcvcbckp( swi_uint8 *inipcmsgp, swi_uint32 inmsglen,
        swi_uint8 *memfreep, swi_uint16 ipcchannel))
{
    struct usbep endpoint = {0, 0}; 
    swi_uint8 *inbufp;                 /* receive buffer pointer */
    swi_size_t resplength;            /* receive buffer length */
    swi_bool   result        = FALSE;  /* return status variable */
    swi_uint32 timeout       = 300;    /* Periodically wake up */
    swi_uint16 chanid;
    
    chanid = cigetchannelnum((char *)owneridp);

    /* Define and allocate the task control block */
    struct dsuscb *dsuscbp = dsgetepcbp(chanid);
    
    /* Initialize task structures */
    dsrxinit(chanid);

    /* Initialize this task's waiter structure */
    swi_ossdkwaiterinit( &dsuscbp->dsusdwaiter );
    
    /* Get an input buffer from the pool */
    inbufp = (swi_uint8 *)mmbufget( &dsuscbp->dsbufpool );
    
    /* Should never run out of memory */
    if( !inbufp )
    {
        erAbort("dsshell_ep: no memory", 0 );
    }
    
    /* Tell the world we've started up */
    dlLog(  &dsuscbp->dsusdldcb, DSLOG_CLASSC, 
            "IPC Task started\n", 
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL);
    
    for(;;)
    {
        /* If the USB system is not alive yet */
        if( !usgetep (cigetifusetype( chanid ), &endpoint) )
        {
            /* Invoke our waiter */
            swi_ossdkwaiterwait( &dsuscbp->dsusdwaiter, DSRXSCANDELAY ); 

            /* restart from the top of the loop */
            continue;
        }
        /* Reset this every time through the loop */
        resplength = DSEPBUFSZ;
        slmemset( (char *) inbufp, 0, DSEPBUFSZ );

        /* Wait for incoming messages from the Diag Receive Endpoint */
        result = usrcvtty( endpoint.usreadep, inbufp, &resplength, timeout);
        
        /* When message received, check if received sucessfully
         */
        if (result == TRUE)
        {
            /* If not a timeout */
            if( resplength )
            {
                /* It is safe to reuse the same buffer (see Note above),
                 * so we do not recycle it.
                 */
                
                /* Call function to add buffer to the DR queue*/
                dsrcvcbckp( inbufp, (swi_uint32)resplength, NULL, chanid );
            }
        }
        else
        {
            /* Note that if the result was false, it may well be a hardware 
             * failure here or the modem was removed while we were attempting 
             * to read from it.
             * Update dropped packets counter. No logging is attempted here as
             * removing and inserting a modem could result in a blast of dropped
             * packets from the modem
             */
            dsuscbp->dsrxpktsdropped++;
            
            /* Log that packet was dropped */
            dlLog(  &dsuscbp->dsusdldcb, DSLOG_CLASSB, 
                    "dsshell_ep: packet dropped: %d", 
                    (swi_uint32)dsuscbp->dsrxpktsdropped,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL);
        }
    } /* end for loop */
}

/*************
 *
 * Name: dsshell
 *
 * Purpose: Implements the main part of the DS receive task. 
 *          This DS shell instance initializes and opens a particular 
 *          channel or an endpoint as specified by the calling parameters,
 *          blocks waiting for incoming IPC messages from that channel
 *          and invokes a receive handler for each message received.
 *             
 * Parms:   index      - entry in the configuration structure
 *          owneridp   - string with the device service name 
 *          devicetype - IPC channel or endpoint channel
 *          chanid     - IC defined channel or endpoint number
 *          dsrcvcbckp - callback on message receive function pointer with the
 *                       following input parameters:
 *              inipcmsgp - pointer to beginning of received message 
 *              inmsglen  - length of received message
 *              memfreep  - pointer to memory to be released by callback
 *              ipcchannel- IPC channel used for this service
 *
 * Return:  never returns
 *
 * Abort:   none
 *
 * Notes:   This task blocks waiting for inbound Packets on 
 *          the IPC channel from the APIs or the Endpoint from the Modem
 *          This task is a parameterized version of a DS thread and
 *          is called by a wrapper function from the package that calls it
 *          during the task creation process.
 *
 **************/
global void dsshell (    
    swi_uint8 *shnamep,
    void *dsrcvcbckp( swi_uint8 *inipcmsgp, swi_uint32 inmsglen,
        swi_uint8 *memfreep, swi_uint16 ipcchannel))
{
    swi_uint16 devicetype;
    
    /* Check if this shell was created on for the
     * IPC side or the Endpoint side 
     */
    ciinitsdkipcchan (shnamep);
    devicetype = cigetifacetype(shnamep);
    switch (devicetype)
    {
        case CIIPC:
            dsshell_ipc(shnamep, dsrcvcbckp);
            break;
        case CIUIF:
            dsshell_ep(shnamep, dsrcvcbckp);
            break;
        default:
            erAbort("dsshell: invalid device type", 0 );
            break;
    }
}

 /*************
 *
 * Name:    dslaunchshell
 *
 * Purpose: Invokes an instance of DS shell
 *             
 * Parms:   shellparmp - pointer to parameters for starting the DS shell
 *                       instance
 *
 * Return:  never returns
 *
 * Abort:   none
 *
 * Notes:   None 
 *
 **************/
global void dslaunchshell ( void *shellparmp )
{   
    struct dsipcshell *ipcshellp = (struct dsipcshell *)shellparmp;
    
    /* 
     * Initiate DS task instance with IPC channel name and callback function
     */
    dsshell( ipcshellp->dsipcshellnamep, (void *)ipcshellp->dsipccbfcnp );
}

/*************
 *
 * Name: dsep_send
 *
 * Purpose: Sends packet to the modem through an endpoint
 *             
 * Parms:   epid         - Endpoint to send packet through
 *          packetp      - Pointer to packet for modem
 *          paylength    - Length of packet
 *
 * Return:  None
 *
 * Abort:   Abort if endpoint is not available
 *
 * Notes:   None
 *
 **************/
global void dsep_send(
    enum useptype epid,
    swi_uint8 *packetp,
    swi_uint16 paylength)
{
    struct usbep endpoint = {0, 0}; 

    if (usgetep(epid, &endpoint))
    {
        /* Send data to the endpoint*/
        ussendtty( endpoint.uswriteep, packetp, paylength );
    }
    /* NULL Block */
    else
    {
        /* This should never happen */
        erAbort("dsEpPacket: Endpoint not available", 0 );
    }
}

/*
 * $Log: dsshell.c,v $
 */
  
