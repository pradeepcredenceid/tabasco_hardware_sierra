/*
 *    $Id: usscan_sdk.c,v 1.27 2010/06/09 23:57:01 epasheva Exp $
 *
 *    Filename: us.c
 *
 *    Purpose:  USB Serial Interface package
 *
 *    Copyright Sierra Wireless Inc., 2008 All rights reserved
 *
 *    Notes: none
 */
#include "aa/aaglobal.h"
#include "sdk/sdkudefs.h"
#include "su/suudefs.h"
#include "us/usidefs.h"

/* Internal Definitions */


/* Local Storage */
struct usscb usscbk;

/*
 *    Name: usgetscbkp
 *
 *    Purpose: Returns a pointer to the US task's control block
 *             structure
 *
 *    Param:   None
 *
 *    Returns: Pointer to usscbk
 *
 *    Abort:   None
 *
 *    Notes:   none
 */
package struct usscb *usgetscbkp( void )
{
    return &usscbk;
}

/*
 *    Name: usgetepname
 *
 *    Purpose: Return name of the connected device given the endpoint number
 *
 *    Param:   endpoint  - endpoint we are interested in (read or write)
 *             namep     - pointer to storage for endpoint name
 *             namlen    - maximum length of storage for name
 *             inout     - direction, TRUE = IN , FALSE = OUT
 *
 *    Returns: TRUE if get name successful, FALSE otherwise
 *
 *    Abort:   None
 *
 *    Notes:   None
 *
 */
global swi_bool usgetepname(swi_uint32 endpoint, 
                            swi_uint8* namep, 
                            swi_uint8 namelen,
                            swi_bool  inout)
{
    struct usscb *usscbkp;  /* US package control block pointer */
    
    /* Get a pointer to the US package control block */
    usscbkp = usgetscbkp();

    /* Get the name, given the direction */
    if (!swi_ossdkusbendpoint(&(usscbkp->uscusbctl), endpoint,
                              inout, (char *)namep, namelen))
        return FALSE;

    return TRUE;
                         
}

/* 
 *    Name: usreadterms
 *
 *    Purpose: Configure the USB read termination parameters. These 
 *             parameters will improve the response of USB reads
 *             by indicating when it is OK for the read routine to 
 *             return what it has so far
 *
 *    Params:  endpoint       - The endpoint being configured by this request
 *             readterminator - Return from a read when the last character
 *                              read matches this one. Use
 *
 *                              USNOTERMINATION 
 *
 *                              to cause the usb read to stop matching
 *                              characters
 *
 *             readthreshold  - Return from a read when the number of
 *                              characters read is equal to or greater
 *                              than this value. Use 
 *
 *                              USNOTHRESHOLD
 *
 *                              to cause the usb read operation to stop
 *                              returning based on a threshold count of 
 *                              characters being received
 *
 *    Abort:   None
 *
 *    Return:  None
 *
 *    Note:    None
 *
 */
global void usreadterms( swi_uint32 endpoint, 
                         swi_uint8 readtermination, 
                         swi_uint32 readthreshold )
{
    struct usscb *usscbkp;  /* US package control block pointer */

    /* Get a pointer to the US package control block */
    usscbkp = usgetscbkp();

    /* Pass this along to the OS wrapper layer */
    swi_ossdkreadterms( &usscbkp->uscusbctl, 
                        endpoint, 
                        readtermination, 
                        readthreshold );
}

/*
 *    Name: usgetinfo
 *
 *    Purpose: Return information about the connected device
 *
 *    Param:   vendidp   - Pointer to storage for the vendor ID
 *             prodidp   - Pointer to storage for the product ID
 *
 *    Returns: TRUE if product exists, FALSE otherwise
 *
 *    Abort:   None
 *
 *    Notes:   None
 *
 */
global swi_bool usgetinfo( swi_uint32 *vendidp, swi_uint32 *prodidp)
{
    struct usscb *usscbkp;  /* US package control block pointer */

    /* Get a pointer to the US package control block */
    usscbkp = usgetscbkp();
    
    /* Get the information */
    return swi_ossdkusbproduct( &usscbkp->uscusbctl, vendidp, prodidp );
}

/*
 *    Name: usifacetoep
 *
 *    Purpose: Find usb endpoint number based on direction and usb interface 
 *             number
 *
 *    Param:   iface - usb interface number
 *             dir   - endpoint direction (SWI_EP_IN|SWI_EP_OUT)
 *
 *    Returns: endpoint number or USEPINVALID
 *
 *    Abort:   None
 *
 *    Notes:   None
 *
 */
package int usifacetoep( swi_uint32 iface, enum swi_endpointdirection dir )
{
    int epnum;

    /* Get the information */
    epnum = swi_ossdkusmapifacetoep( &usgetscbkp()->uscusbctl, iface, dir );
    if( epnum < 0 )
    {
        return USEPINVALID;
    }
    return epnum;
}

/*
 *    Name: ususbwaitmodem
 *
 *    Purpose: Wait until the modem is available
 *
 *    Param:   None
 *
 *    Returns: None
 *
 *    Abort:   None
 *
 *    Notes:   None
 *
 */
global void ususbwaitmodem( void )
{
    struct usscb *usscbkp;  /* US package control block pointer */

    /* Get a pointer to the US package control block */
    usscbkp = usgetscbkp();

    /* Wait for the specified endpoint */
    swi_ossdkusbwaitmodem( &usscbkp->uscusbctl );
}

/*
 *    Name: usrcvtty
 *
 *    Purpose: Receive some data from a specified endpoint
 *
 *    Param:   endpoint - The endpoint to receive from
 *             datap    - Pointer to a buffer to store the received data
 *             dlengthp - Pointer to the number of bytes available in datap
 *                        On return, contains the number of bytes read or 0
 *                        if timed out
 *             timeout  - The length of time (ms) to wait for a 
 *                        response. 0 means wait forever
 *
 *    Returns: TRUE  - Data received
 *             FALSE - The device has disappeared (cable unplugged, modem
 *                     reset, etc)
 *
 *    Abort:   None
 *
 *    Notes:   This function runs in a different thread from the main
 *             US scan task.
 *
 */
global swi_bool usrcvtty(  swi_uint16 endpoint, 
                           swi_uint8 *datap, 
                           swi_size_t *dlengthp, 
                           swi_uint32 timeout )
{
    swi_bool retval;          /* Returned by this function */
    swi_size_t readlength;
    struct usscb *usscbkp;  /* US package control block pointer */

    /* Get a pointer to the US package control block */
    usscbkp = usgetscbkp();

    /* Make sure the caller isn't using a bad endpoint */
    if( endpoint == USEPINVALID )
        return FALSE;

    /* Initialize the return value to show a successful read */
    retval = TRUE;

    /* Need to reset this every time through the loop */
    readlength = *dlengthp;
 
    dlLog(  &usscbkp->usdlscb, USLOG_CLASSB, 
            "reading ep: %d, timeout %d\n", 
            (swi_uint32)endpoint, 
            (swi_uint32)timeout,
            (swi_uint32)NULL,
            (swi_uint32)NULL);

    /* Send the data to the endpoint */
    retval = swi_ossdkusbread( &usscbkp->uscusbctl, 
                                   endpoint, 
                                   datap, 
                                   &readlength, 
                                   timeout );
                       
    /* Update the caller's length */
    *dlengthp = readlength;

    return retval;
}

/*
 *    Name: ussendtty
 *
 *    Purpose: Send some data to a tty. The caller specifies the 
 *             endpoint and this entry determines which tty to use
 *             and sends the traffic to it.
 *
 *    Param:   endpoint - The destination endpoint
 *             datap    - Pointer to a buffer of data to send
 *             dlength  - The number of bytes pointed-to by datap
 *
 *    Returns: TRUE if write succeeded, 
 *             FALSE if the modem has disappeared (crashed, unplugged
 *             reset, etc.)
 *
 *    Abort:   None
 *
 *    Notes:   If the modem disappears the caller must take steps to
 *             determine when it has returned and what endpoint to
 *             use for the type of traffic desired. 
 *
 *             This function runs in a different thread from the main
 *             US scan task.
 *
 */

global swi_bool ussendtty( swi_uint16 endpoint, 
                           swi_uint8 *datap, 
                           swi_size_t dlength )
{
    swi_bool retval;
    struct usscb *usscbkp;

    /* Get a pointer to the US package control block */
    usscbkp = usgetscbkp();

    /* Make sure the caller isn't using a bad endpoint */
    if( endpoint == USEPINVALID )
        return FALSE;

    /* Log the write, if enabled */
    dlLog(  &usscbkp->usdlscb, USLOG_CLASSC, 
            "writing ep: %d\n", 
            (swi_uint32)endpoint,
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL);

    /* Send the data to the endpoint. Watch the return 
     * code to determine if there's been a problem
     */
    retval = swi_ossdkusbwrite( &usscbkp->uscusbctl, 
                                endpoint, 
                                datap, 
                                dlength );

    return retval;
 
}

/*
 *    Name: usclosetty
 *
 *    Purpose: Close the tty associated with the specified endpoint
 *
 *    Param:   ep - Pointer to a US package endpoint type containing
 *                  valid numbers for the read and write endpoint
 *
 *    Returns: None
 *
 *    Abort:   None
 *
 *    Notes:   The endpoints may or may not be open when this function
 *             is called. 
 *
 *             Package use only for now
 *
 */
package void usclosetty( struct usbep *ep )
{
    struct usscb *usscbkp;

    /* Get a pointer to the US package control block */
    usscbkp = usgetscbkp();

    /* Close the specified write endpoint */
    swi_ossdkusbclose( &usscbkp->uscusbctl, ep->uswriteep, TRUE );
    swi_ossdkusbclose( &usscbkp->uscusbctl, ep->usreadep, FALSE );
}


/*
 *    Name: usscantask
 *
 *    Purpose: The USB Scanning task's main loop. 
 *
 *    Parms:   paramp - pointer to data that may be passed in when this task
 *                      is started.
 *
 *    Returns: None
 *
 *    Abort:   None
 *
 *    Notes:   This task uses a Waiter with a fixed timeout to 
 *             generate timing events to kick off a scan
 *  
 *             There are two threads in this package, this one does most
 *             of the work of initialization for the package. The other 
 *             thread usdetecttask(), is documented in usdetect_sdk.c
 */
global void usscantask( void *paramp)
{
    swi_uint32 logfilter, logmask;
    struct usscb *usscbkp;

    /* Get a pointer to the US package control block */
    usscbkp = usgetscbkp();

    /* Register with DL for logging, initially disabled */
    dlregister( "USST", &usscbkp->usdlscb, FALSE);

    /* Log our start */
    dlLog(  &usscbkp->usdlscb, USLOG_CLASSA,
            "Scan task startup\n",
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL);

    /* Initialize the logging mask for the "for" loop below. 
     * One day this mask can be read from a configuration file
     * to override this hard-coded setting if need be
     */
    logmask = USFILTERMASK;

    /* Initialize the task's waiter structure */
    swi_ossdkwaiterinit( &usscbkp->uscwaiterscan );

    /* Initialize the task's USB structure */
    swi_ossdkusbinit( &usscbkp->uscusbctl );

    /* Call the SU callback now that this task is running and 
     * let the next task start up
     */
    sutaskinitcb( SDK_USSPKG );

    /* Main loop for USB scanning. Note, this task must always run periodically
     * independently of the device detection task because it is the scan below
     * that ensures devices will be detected when they appear. Therefore, do 
     * not make any changes to this task that would cause it to block except
     * where it already blocks, in the call to swi_ossdkwaiterwait()
     */
    for( logfilter = 0; ; logfilter++ )
    {
        /* Sleep and then force a scan of the USB subsystem */
        swi_ossdkwaiterwait( &usscbkp->uscwaiterscan, USSLEEPSCAN );

        /* Only log every nth pass through here to keep the logfile
         * from becoming cluttered if dl Logging is turned on for 
         * this package
         */
        if( (logfilter & logmask) == logmask )
        {
            /* Log that we are running the next scan */
            dlLog(  &usscbkp->usdlscb, USLOG_CLASSA, 
                    "usscan: scanning\n",
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL);
        }

        /* Run a scan for USB devices */
        swi_ossdkusbscan( &usscbkp->uscusbctl );
    }
}

/*
 *    Name: usinit
 *
 *    Purpose: To initialize the USB serial interface package including
 *             variables for both tasks
 *
 *    Param:   None
 *
 *    Returns: None
 *
 *    Abort:   None
 *
 *    Notes:   None
 */
global void usinit( void )
{
    struct usdcb *usdcbkp;

    /* Get a pointer to the US package control block */
    usdcbkp = usgetdcbkp();

    /* Initially no valid read or write endpoints */
    usdcbkp->useptab.ushipep.usreadep     = USEPINVALID;
    usdcbkp->useptab.ushipep.uswriteep    = USEPINVALID;
    usdcbkp->useptab.usdiagep.usreadep    = USEPINVALID;
    usdcbkp->useptab.usdiagep.uswriteep   = USEPINVALID;
    usdcbkp->useptab.usnmeaep.usreadep    = USEPINVALID;
    usdcbkp->useptab.usnmeaep.uswriteep   = USEPINVALID;
    usdcbkp->useptab.usatcmdep.usreadep   = USEPINVALID;
    usdcbkp->useptab.usatcmdep.uswriteep  = USEPINVALID;
    usdcbkp->useptab.usdata1ep.usreadep   = USEPINVALID;
    usdcbkp->useptab.usdata1ep.uswriteep  = USEPINVALID;
    usdcbkp->useptab.usdata2ep.usreadep   = USEPINVALID;
    usdcbkp->useptab.usdata2ep.uswriteep  = USEPINVALID;
    usdcbkp->useptab.usdata3ep.usreadep   = USEPINVALID;
    usdcbkp->useptab.usdata3ep.uswriteep  = USEPINVALID;
    usdcbkp->useptab.usmassstorageep.usreadep   = USEPINVALID;
    usdcbkp->useptab.usmassstorageep.uswriteep  = USEPINVALID;

    /* Loopback test result parameters */
    usdcbkp->usdlbresult = HPINVALID;
}

/*
 *    Name: ustaskinit
 *
 *    Purpose: To create the USB interface thread. This thread's job
 *             it to periodically assess scan the USB subsystem to determine
 *             whether there has been a change of device and update the 
 *             endpoint to tty mapping
 *
 *    Param:   None
 *
 *    Returns: None
 *
 *    Abort:   None
 *
 *    Notes:   None
 */
global void usstaskinit( void )
{
    swi_ossdkthreadcreate( usscantask, NULL );
}

/*
 * $Log: usscan_sdk.c,v $
 */
