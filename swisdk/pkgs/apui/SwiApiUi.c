/*************
 *
 * $Id: SwiApiUi.c,v 1.3 2010/06/09 23:31:38 epasheva Exp $
 *
 * Filename: SwiApiUi.c
 *
 * Purpose:  Contains source code for UI API functions for the
 *           the LinuxSDK. 
 *
 * Copyright:  2009 Sierra Wireless, Inc. all rights reserved
 *
 **************/

/* include files */
#include "apuiidefs.h"

/* Defines */

/* Global storage */
/* This package's global control block */
global struct apuicb apuicbk;
                                        
/**
 *
 * Return a pointer to the APUI package's control block 
 *
 * \return  pointer to apuicb structure
 *
 */
package struct apuicb *SwiApUiGetCbp(void)
{
    return &apuicbk;
}

/**
 *
 * Return a pointer to the APUI package's IC ipc control block 
 *
 * \return  pointer to icipccb structure
 *
 */
package struct icipccb *SwiApUiGetIcIpccbp(void)
{
    return &(apuicbk.apuiipccb);
}

/*************
 *
 * Name: SwiAPiUiGetReqBufp
 *
 * Purpose: Return a pointer to sufficient storage for holding a 
 *          AM request for transmission to the SDK side. 
 *             
 * Parms:   none
 *
 * Return:  Pointer to an array of swi_uint8 large enough to contain
 *          the biggest possible AM request
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
package swi_uint8 *SwiAPiUiGetReqBufp( void )
{
    return &(apuicbk.apuirequestblock[0]);
}
/*************
 *
 * Name: SwiAPiUiGetRespBufp
 *
 * Purpose: Return a pointer to sufficient storage for holding a 
 *          AM response received from the SDK side. 
 *             
 * Parms:   none
 *
 * Return:  Pointer to an array of swi_uint8 large enough to contain
 *          the biggest possible AM response
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
package swi_uint8 *SwiAPiUiGetRespBufp( void )
{
    return &(apuicbk.apuiresponseblock[0]); 
}
/*************
 *
 * Name:    SwiAPiUiGetRespBufsz
 *
 * Purpose: Return the size of the response block, in bytes
 *             
 * Parms:   none
 *
 * Return:  size of the apuiresponseblock array, in bytes
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
package swi_uint32 SwiAPiUiGetRespBufsz( void )
{
    return sizeof( apuicbk.apuiresponseblock );
}

/**
 *
 * Determine whether the API has been previously started
 *
 * \return  TRUE  - API is initialized and running
 * \return  FALSE - API was not properly started. Go back and
 *                  start it up
 *
 */
package swi_bool SwiApUiRunning( void )
{
    struct apuicb *apuicbp = SwiApUiGetCbp();

    if( apuicbp->apuiopened == APUIINITIALIZED )
        return TRUE;

    return FALSE;
}

/**
 *
 * This function is used to start the API UI sub-system.  
 *
 * \param   pOpenParams: [IN] parameters to initialize the API in 
 *                         different modes
 *
 * \return    SWI_RCODE_OK           - API initialized successfully
 * \return    SWI_RCODE_FAILED       - The API sub-system was not initialized 
 *                                     successfully.
 * \return    SWI_RCODE_SYSTEM_ERROR - This may be due to limited system 
 *                                     resources when the API requested 
 *                                     memory from the OS.
 * \return    SWI_RCODE_BUSY         - If another application has the SDK
 *                                     open. Only 1 application at a time
 *                                     is allowed
 * \return    SWI_RCODE_NOT_COMPATIBLE - the SDK and the Application cannot
 *                                       work together
 * \return    See SWI_RCODE for other error return codes
 *
 */
SWI_API SWI_RCODE SwiApiUiStartup(
    struct SWI_STRUCT_ApiStartup *pOpenParams)
{

    struct apuicb *apuicbp;
    struct icipccb *apuicipccbp;
    SWI_RCODE      retcode;

    swi_uint8 *pathnamep;      /* Resolves to the path to the executable */
    swi_bool retval;   

    /* Get the package global control block pointer */
    apuicbp = SwiApUiGetCbp();

    /* Not opened yet */
    apuicbp->apuiopened = 0;
   
    /* Provide a method for reading the path to the SDK process 
     * executable. If not provided, then use the default path
     */
    if( pOpenParams->sSdkPath.bEnable )
    {
        /* Use the pathname specified by the caller */
        pathnamep = (swi_uint8 *)pOpenParams->sSdkPath.szSdkPath;
    }
    else
    {
        /* Since we got no SDK path from the caller, we'll 
         * return an error
         */
        retcode = SWI_RCODE_SYSTEM_ERROR;
        return retcode;
    }
    
    if( swi_osapisdkprocesscreate( (const char *)pathnamep ) )
    {
        /* So far things are fine, update the return code */
        apuicbp->apuiopened = APUIINITIALIZED;
        retcode = SWI_RCODE_OK;
    }
    else
    {
        retcode = SWI_RCODE_SYSTEM_ERROR;
        return retcode;
    }
        
   /* Initialize the IPC channels. If the result 
     * is FALSE, then another application is already
     * attached to the SDK, so this one should end
     */
    
    apuicipccbp = SwiApUiGetIcIpccbp ();
    /* Initialize the IC handle for Request/Response channel */
    ciinit();
    retval = ciinitapiipcchan  (apuicipccbp, (swi_uint8 *)CIIPCUI);
    if (!retval)
    {
        /* Failed this last check, so we must remember we're not open */
        apuicbp->apuiopened  = 0;
        retcode = SWI_RCODE_BUSY;
    }
    return retcode;
}

/**
 *
 * This function is used to send any UI command to the SDK
 *
 * \param  datap     : data to be passed to the UI task in SDK
 * \param  datalen   : length of data to be passed to the UI task in SDK
 *                            
 * \return    SWI_RCODE_OK  - API control indication sent  successfully
 * \return    See SWI_RCODE for other error return codes
 *
 */
SWI_API SWI_RCODE SwiApiUiSend( 
    swi_uint8 *datap, 
    swi_uint16 datalen)
{
    struct icipccb  *apuicipccbp;
    swi_uint8       *hdrp;
    swi_uint16      amsize;           /* Returned by pkt building routine */
   
     /* Do the standard checks */
    if(!SwiApUiRunning())
    {
        return SWI_RCODE_API_NOT_OPEN;
    }
    
    /* Get IC ipc control block pointer */
    apuicipccbp = SwiApUiGetIcIpccbp ();
    
    /* initialize the local copy */
    hdrp = SwiAPiUiGetReqBufp();

    /* Build the AM packet first */
    amsize = ambuilduirequest( &hdrp, datalen );
    slmemcpy ( (hdrp+amsize), datap, datalen);

    /* Note: This is a non-blocking call. Simply return SUCCESS to the caller */
    /*       The application needs to wait for messages from the other side 
     *       in a separate thread and output them to the console 
     */
    /* Send the packet to the SDK side */
    icapi_send(apuicipccbp, hdrp, amsize+datalen);

    /* Always return success */
    return SWI_RCODE_OK;  
}


/**
 *
 * The UI receive API main entry point
 *             
 * This function waits to receive UI messages fro the SDK 
 *
 * \param  respp   : pointer to pointer to storage for returning data
 * \param  timeout : timeout to wait for data from SDK, 0 means wait forever,
 *                   ie, until there are messages from the SDK
 *
 * \return  SWI_RCODE_OK              - The function executed properly.
 * \return  SWI_RCODE_FAILED          - The function failed to execute 
 *                                      correctly. 
 * \return  SWI_RCODE_API_NOT_OPEN    - SDK is not running.
 * \sa  See SWI_RCODE for additional information. 
 *
 */
SWI_API SWI_RCODE SwiApiUiReceive(
    swi_uint8 **respp, 
    swi_uint32 timeout) 
{
    SWI_RCODE       retcode = SWI_RCODE_OK;
    struct icipccb  *apuicipccbp;
    swi_uint16      uidatalen;

    /* variables used in parsing received packet */
    swi_uint16 amtype;
    swi_uint16 amlength;
    swi_uint16 amheaderversion;
    swi_uint8  amresultcode;
    swi_uint8  *inbufp;          /* local pointer to global receive buffer
                                  * structure 
                                  */
    swi_bool    result;
    swi_uint32  buflength;
 
     /* Do the standard checks */
    if(!SwiApUiRunning())
    {
        return SWI_RCODE_API_NOT_OPEN;
    }
    
    /* Get IC ipc control block pointer */
    apuicipccbp = SwiApUiGetIcIpccbp ();

    /* Loop until a UI message is received or the SDK terminates 
     * unsuccessfully 
     */
    
    /* Make a local copy of the incoming packet pointer */
    inbufp = (swi_uint8*)SwiAPiUiGetRespBufp();
    /* 
     * Determine the maximum size of response that can be 
     * accommodated in the local buffer 
     */
    buflength = SwiAPiUiGetRespBufsz();
    slmemset ((char *)inbufp, 0, buflength);
              
   /* if a non-zero timeout is specified */
    if(timeout)
    {
        /* 
         * adjust the timeout value so to ensure that the API side
         * times out before the SDK side.
         */
        timeout = (1000 + timeout) << 2;
    }
    /* Now, block waiting for a response from the modem or a timeout */
    result = icapi_rcv( apuicipccbp, 
                        inbufp,
                        &buflength,
                        timeout); 
    
     /* Determine if there was a timeout. Note, it's possible this 
     * is a real timeout or that the SDK task has disappeared
     */
    if( !buflength )
    {
        /* Timeout */
        return(SWI_RCODE_GET_TIMEOUT);
    }
  
    /* Parse the header of the received AM packet */
    amparseamheader(
        &amtype, 
        &amheaderversion, 
        &amresultcode, 
        &amlength, 
        &inbufp);
    
    /* if the SDK header version is different from what the API supports */
    if((amheaderversion != AMHDRVERSION ) ||  
       (amresultcode == SWI_AMVERSERROR))
    {
        return(SWI_RCODE_NOT_COMPATIBLE);
    }
    
    /* if this is not a short response from the SDK */
    if(amlength)
    {
        /* Parse AM response packet payload */
        amparseuiresponse(&inbufp, &uidatalen);
        retcode = SWI_RCODE_OK; 
    }
    else /* bad result */
    {
        /* error in receiving on notification channel */
        retcode = SWI_RCODE_FAILED;
    }

    /* return pointer to the end of the parsed fields */
    *respp = inbufp;

    return retcode;
}

/* 
 * $Log: SwiApiUi.c,v $
 */


