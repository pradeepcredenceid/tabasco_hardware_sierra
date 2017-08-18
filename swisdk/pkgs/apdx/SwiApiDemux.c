/**
 *    \if CvsId 
 *    $Id: SwiApiDemux.c,v 1.10 2010/06/09 23:30:37 epasheva Exp $
 *    \endif
 *    \ingroup demux
 *    \file SwiApiDemux.c
 * 
 *    \brief  Contains source code for Basic Demux API functions for the
 *            the SDK. 
 *
 *    Copyright: © 2008 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "apdxidefs.h"


/* Defines */

/* Global storage */
/* This package's global control block */
struct apdxcb apdxcbk;

/* Buffer to build and send  Demux Request */
swi_uint8 apdxmsgarray[APDXBUFSIZE];

/* Buffer to receive demux indication */
swi_uint8  inbuf[APDXBUFSIZE];          /* IPC buffer for messages received 
                                         * on a device service channel 
                                         */
/**
 *
 * Return a pointer to the APDX package's control block 
 *
 * \return  pointer to apdxcb structure
 *
 */
package struct apdxcb *SwiApDxGetCbp(void)
{
    return &apdxcbk;
}

/**
 *
 * Return a pointer to the APDX package's IPC control block for a particular
 * device service
 *
 * \param  deviceservice: [IN]  device service - diagnostic or NMEA
 * \return  pointer to apdxipccb structure
 *
 */
package struct apdxipccb *SwiApDxGetIPCCbp( swi_uint16 deviceservice )
{
        return &(apdxcbk.apdxchancb[deviceservice]);  
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
package swi_bool SwiApDxRunning( void )
{
    struct apdxcb *apdxcbp = SwiApDxGetCbp();

    if( apdxcbp->apdxopened == APDXINITIALIZED )
        return TRUE;

    return FALSE;
}

/**
 *
 * Determine whether the device is present
 *
 * \return  TRUE  - air server present
 * \return  FALSE - air server is not loaded onto the system
 *
 */
package swi_bool SwiApDxAirServerPresent( void )
{
    struct apdxcb *apdxcbp = SwiApDxGetCbp();
   
    if( apdxcbp->apdxairstate.apdxairserverpresent == SWI_ONLINE )
        return TRUE;
    return FALSE;
}

/**
 *
 * Maps a deviceservice to a predefined channel id
 *
 * \param   deviceservice: [IN] device service - diagnostics, NMEA, etc...
 * 
 * \return  the channel id if successful mapping 
 * \return  CIMAXIPCCHAN if the device service is not supported
 *
 */
package swi_uint32 SwiApDxDevToChan( swi_uint16 deviceservice)
{
    switch (deviceservice)
    {
        case  DRDIAG:
            return  (cigetindex ((swi_uint8 *)CIIPCDIAG));    /* Diagnostic */  
            break;
            
        case DRNMEA:
            return (cigetindex ((swi_uint8 *)CIIPCNMEA));    /* NMEA */    
            break;
            
        default:
            return CIMAXIPCCHAN;
            break;
    }
}

/**
 *
 * Return a pointer to identification string for a  device service 
 *
 * \param  deviceservice: [IN]  device service - diagnostics, NMEA, etc..
 * \return pointer to string structure
 *
 */
package swi_uint8 *SwiApDxDevToName( swi_uint16 deviceservice )
{
    swi_uint16 channel;
    
    channel = SwiApDxDevToChan(deviceservice);
    if (channel < CIMAXIPCCHAN)
    {
        return cigetchannelname( channel );
    }
    else
    {
        return (swi_uint8 *)"Unknown";        
    }
}
  
/**
 *
 * This function is used to start the API Demux sub-system.  
 *
 * If an error is returned or to re-initialize the API, 
 * SwiApiDxShutdown must be called before calling this function 
 * again.
 *
 * Looking in the system log after a failure may provide more 
 * details about the cause
 *
 * \param   numchannels: [IN] the number of channels the Application intends 
 *                           to use (for verification purposes)
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
SWI_API SWI_RCODE SwiApiDxStartup(swi_uint16 numchannels, 
    struct SWI_STRUCT_ApiStartup *pOpenParams)
{

    struct apdxcb *apdxcbp;
    SWI_RCODE      retcode;
    swi_uint16     maxnumchan; /* Max number of IPC channels supported by SDK 
                                * This corresponds to the maximum number of 
                                * device services supported 
                                */
    swi_uint8 *pathnamep;      /* Resolves to the path to the executable */
    
    /* Verify the number of channels requested for use does not exceed the
     * maximum number supported by the current version of SDK.
     */
     retcode = dsgetmaxnumchan( &maxnumchan );
     /* Ignore retcode because it is always OK */
     if (numchannels > maxnumchan)
     {
          /* SDK cannot support that number of channels; return with
           * SWI_RCODE_NOT_COMPATIBLE
           */  
          retcode = SWI_RCODE_NOT_COMPATIBLE;
          return retcode;      
     }

    /* Get the package global control block pointer */
    apdxcbp = SwiApDxGetCbp();

    /* Not opened yet */
    apdxcbp->apdxopened = 0;
    
    /* Initialize Air Server status structure */
    apdxcbp->apdxairstate.apdxairserverpresent = SWI_OFFLINE;
    apdxcbp->apdxairstate.apdxairchgera        = 0;

    /* Initialize maximum number of channels that are supported by the SDK */
    apdxcbp->apdxmaxnumchan    = maxnumchan; 
   
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
    
    /* Initialize the channel information manager package */
    ciinit();
    
    if( swi_osapisdkprocesscreate( (const char *)pathnamep ) )
    {
        /* So far things are fine, update the return code */
        apdxcbp->apdxopened = APDXINITIALIZED;
        retcode = SWI_RCODE_OK;
    }
    else
    {
        retcode = SWI_RCODE_SYSTEM_ERROR;
        return retcode;
    }
    
    return retcode;
}

/**
 *
 * This function is used to register for device service with the API Demux 
 * sub-system.  
 *
 * If an error is returned or to re-initialize the API, 
 * SwiApiDxShutdown must be called before calling this function 
 * again.
 *
 * Looking in the system log after a failure may provide more 
 * details about the cause
 *
 * \param  deviceservice : device service requested - diagnostic or NMEA
 *                            
 * \param  datacbfp : pointer to user callback function for Demux Data
 * \param  datauserp : user parameter for data callback
 * \param  ctlcbfp : pointer to user callback for control indication
 * \param  ctluserp : User parameter for control indication callback
 *   
 * \return    SWI_RCODE_OK           - API registered successfully
 * \return    SWI_RCODE_FAILED       - The API sub-system was not registered 
 *                                     successfully.
 * \return    SWI_RCODE_SYSTEM_ERROR - This may be due to limited system 
 *                                     resources when the API requested 
 *                                     memory from the OS.
 * \return    See SWI_RCODE for other error return codes
 *
 */
SWI_API SWI_RCODE SwiApiDxRegister(swi_uint16 deviceservice, 
    void (*datacbfp)(swi_uint16 deviceservice, swi_uint8 *datap, 
        swi_uint32 datalen, void *userp), 
    void *datauserp, 
    void (*ctlcbfp)(swi_uint16 deviceservice, swi_uint8 *ctldatap, 
        void *userp), 
    void *ctluserp )
{
    struct apdxipccb *ipccntrlbkp;  /* pointer to IPC structure within the 
                                     * the package control block
                                     */
    struct apdxcb    *pkgcntrlbkp;  /* pointer to apdx package control block */
    SWI_RCODE        retcode;
    swi_uint32 channel = 0;  /* channel id */
    
    /* Do the standard checks */
    if(!SwiApDxRunning())
    {
        return SWI_RCODE_API_NOT_OPEN;
    }
    
    /* Get package control block pointer */
    pkgcntrlbkp = SwiApDxGetCbp();
    
    /* Get IPC channel control block pointer */
    ipccntrlbkp = SwiApDxGetIPCCbp( deviceservice );

    /* Install the callbacks and store user parameters */
    ipccntrlbkp->apdxdevsrv    = deviceservice;
    ipccntrlbkp->apdxnamep     = SwiApDxDevToName(deviceservice); 
    ipccntrlbkp->apdxrcvcbckp  = pkgcntrlbkp->apdxrcvhandlerp;
    ipccntrlbkp->apdxdatacbfp  = datacbfp;
    ipccntrlbkp->apdxdatauserp = datauserp;
    ipccntrlbkp->apdxctlcbfp   = ctlcbfp;
    ipccntrlbkp->apdxctluserp  = ctluserp;
    
    /* Obtain assigned channel for that device service */
    channel = SwiApDxDevToChan( deviceservice ); 
    if (channel < CIMAXIPCCHAN)
    {
        /* Open IPC channel */    
        retcode = dsopen( ipccntrlbkp->apdxnamep, channel ); 
    }
    else
    {
       retcode =  SWI_RCODE_INVALID_PAR;       
    }
    return retcode;
}

/**
 *
 * This function is used to format and send a Demux Request to
 * Start a specific device service. After this command SDK starts to send data
 * for that particular device service
 *
 * If an error is returned or to re-initialize the API, 
 * SwiApiDxShutdown must be called before calling this function 
 * again.
 *
 * Looking in the system log after a failure may provide more 
 * details about the cause
 *
 * \param  deviceservice : device service requested - diagnostic or NMEA
 *                            
 * \return    SWI_RCODE_OK  - API control indication send  successfully
 * \return    See SWI_RCODE for other error return codes
 *
 */
SWI_API SWI_RCODE SwiApiDxBegin(swi_uint16 deviceservice)
{
    SWI_RCODE retcode;
    swi_uint32 channel;
    swi_uint32 bytesbuilt;
    swi_uint8  *msgarrayp;
    
    msgarrayp = &apdxmsgarray[0];
    
    /* Do the standard checks */
    if(!SwiApDxRunning())
    {
        return SWI_RCODE_API_NOT_OPEN;
    }
    
    /* Format AMDEMUXREQUEST with deviceservice and operation Start */   
    bytesbuilt =  ambuilddemuxrequest( &msgarrayp, deviceservice, SWI_DxStart); 
    channel = SwiApDxDevToChan(deviceservice);
    if (channel >= CIMAXIPCCHAN)
    {
        return SWI_RCODE_INVALID_PAR;  /* invalid device service */         
    }
    
    /* Send the request over that device service IPC channel */ 
    retcode = dsappsend( channel, &apdxmsgarray[0], bytesbuilt);   
    
    return retcode;  
}

/**
 *
 * This function is used to format and send a Demux Request to
 * Stop a specific device service. After this command SDK stops to send data
 * for that particular device service
 *
 * If an error is returned or to re-initialize the API, 
 * SwiApiDxShutdown must be called before calling this function 
 * again.
 *
 * Looking in the system log after a failure may provide more 
 * details about the cause
 *
 * \param  deviceservice : device service requested
 *                            
 * \return    SWI_RCODE_OK           - API control indication send  successfully
 * \return    See SWI_RCODE for other error return codes
 *
 */

SWI_API SWI_RCODE SwiApiDxEnd(swi_uint16 deviceservice)
{
    SWI_RCODE retcode;
    swi_uint32 channel;
    swi_uint16 bytesbuilt;
    swi_uint8  *msgarrayp;
    
    msgarrayp = &apdxmsgarray[0];
    
    /* Do the standard checks */
    if(!SwiApDxRunning())
    {
        return SWI_RCODE_API_NOT_OPEN;
    }
    
   /* Format AMDEMUXREQUEST with deviceservice and operation Stop */       
   bytesbuilt =  ambuilddemuxrequest( &msgarrayp, deviceservice, SWI_DxStop); 
   
   /* Send the request over that device service IPC channel */ 
    channel = SwiApDxDevToChan(deviceservice);
    if (channel >= CIMAXIPCCHAN)
    {
        return SWI_RCODE_INVALID_PAR;  /* invalid device service */         
    }

    /* Send the request over that device service IPC channel */ 
    retcode = dsappsend( channel, &apdxmsgarray[0], bytesbuilt); 
    return retcode;     
}
 
/**
 *
 * This function is used to send a Demux Data Indication to a 
 * specific device service. 
 *
 * If an error is returned or to re-initialize the API, 
 * SwiApiDxShutdown must be called before calling this function 
 * again.
 *
 * Looking in the system log after a failure may provide more 
 * details about the cause
 *
 * \param  deviceservice: [IN]  device service requested
 * \param  datap:   [IN]  pointer to the beginning of the buffer to be send
 * \param  datalen: [IN]  length of total data to be send 
 *                            
 * \return    SWI_RCODE_OK             - API data indication send  successfully
 * \return    SWI_RCODE_INVALID_PAR    - device service does not have open 
 *                                       channel
 * \return    SWI_RCODE_DEVICE_UNAVAIL - air server is not loaded onto the 
 *                                       system
 * \return    SWI_RCODE_NOT_INIT       - this is not initialized data packet
 * \sa \return    See SWI_RCODE for other error return codes
 *
 */
SWI_API SWI_RCODE SwiApiDxSend(
    swi_uint16 deviceservice, 
    swi_uint8 *datap, 
    swi_uint32 datalen)
{
    SWI_RCODE retcode;
    swi_uint32 channel;
    swi_uint32 bytesbuild;
    swi_uint8 *msgarrayp = datap; /* local pointer to caller's start of 
                                   * buffer
                                   */
    
    /* Do the standard checks */
    if(!SwiApDxRunning())
    {
        return SWI_RCODE_API_NOT_OPEN;
    }
    
    /* If there is no Air Server present do not run API */    
    if(!SwiApDxAirServerPresent())
    {
        return SWI_RCODE_DEVICE_UNAVAIL;
    }
    
    /* Make sure this is a new initialized data packet */
    if ( *((swi_uint32 *)datap) != APDXDATAHDRMARK )
    {
        return SWI_RCODE_NOT_INIT;        
    }
   
   /* Format AMDEMUXDATAIND header with deviceservice */       
    bytesbuild = ambuilddemuxdataind( &msgarrayp, deviceservice); 
    
    /* Send the request over that device service IPC channel */ 
    channel = SwiApDxDevToChan(deviceservice);
    if (channel < CIMAXIPCCHAN)
    {
        /* Send the request over that device service IPC channel */ 
        retcode = dsappsend( channel, datap, datalen);
    }
    else
    {
       retcode =  SWI_RCODE_INVALID_PAR;       
    }
    return retcode; 
}

/**
 *
 * The Demux receive API main entry point
 *             
 * When the received packet is wrong type the message "Wrong 
 * packet type" is logged and program terminates.
 *
 * This function waits to receive demux indications
 * and invokes the user callback function if registered. 
 * This function does not return unless there is an error.
 *
 * \return  SWI_RCODE_OK              - The function executed properly.
 * \return  SWI_RCODE_FAILED          - The function failed to execute 
 *                                      correctly. 
 * \return  SWI_RCODE_API_NOT_OPEN    - SDK is not running.
 * \sa  See SWI_RCODE for additional information. 
 *
 */
SWI_API SWI_RCODE SwiWaitDxIndication(swi_uint16 deviceservice) 
{
    SWI_RCODE retcode            = SWI_RCODE_OK;
    swi_uint32 apdxipcbuflng;      /* length of receive buffer for demux 
                                    * indication message 
                                    */
    struct apdxipccb *ipccntrlbkp; /* pointer to IPC structure within the 
                                    * the package control block
                                    */
    struct apdxcb    *pkgcntrlbkp; /* pointer to apdx package control block */
    swi_uint32 channel;
    swi_uint32 payldoffset;        /* offset of payload within the packet */ 
    swi_uint32 msglen;             /* length of the message to be sent to the 
                                    * registered user 
                                    */
    swi_uint32 eranumber;          /* era number */
    /* variables used in parsing received packet */
    swi_uint16 amtype;
    swi_uint16 amheaderversion;
    swi_uint16 ampacketlength;
    swi_uint8  *inbufp;          /* local pointer to global receive buffer
                                  * structure 
                                  */
    swi_uint8  amresultcode;
    swi_uint8 opcode;            /* operation code */
    swi_uint8 devicepresent;     /* device present */

    swi_uint8 errstring[LENGTH_DxErrorString]; /* error string */
    swi_uint8 *msgp;             /* Local copy of input IPC message pointer 
                                  * to be used in parsing 
                                  */

    /* Do the standard checks */
    if(!SwiApDxRunning())
    {
        return SWI_RCODE_API_NOT_OPEN;
    }
    
    /* Get package control block pointer */
    pkgcntrlbkp = SwiApDxGetCbp();
    /* Get IPC channel control block pointer */
    ipccntrlbkp =  SwiApDxGetIPCCbp( deviceservice );
    
    channel = SwiApDxDevToChan( deviceservice);
    
    /* Loop until a proper demux indication is received or the SDK terminates 
     * unsuccessfully 
     */
    while(1)
    {
        /* Make a local copy of the incoming packet pointer */
        inbufp = &inbuf[0];
        
        /* Initialize ipc buffer length variable */
        apdxipcbuflng = APDXBUFSIZE;
        
        /* This is a blocking call to receive a packet */        
        retcode = dsreceive( channel, inbufp, &apdxipcbuflng, 0 );

        /* if the IPC channel timed out or received something */
        if( retcode == SWI_RCODE_OK)
        {
            /* if any demux indication was received */
            if(apdxipcbuflng != 0)
            {                
                /* Check the type of the packet received */
                msgp = inbufp;
                /* Parse the received message */
                /* First parse the header */
                amparseamheader( &amtype, &amheaderversion, &amresultcode, 
                    &ampacketlength, &msgp);
                /* Note: header parameter check not implemented */
                switch (amtype)
                {
                    case AMDEMUXREQUEST:
                         /* Not Applicable */
                        break;
                       
                    case AMDEMUXCTLIND:
                        /* Parse Demux Control Indication */
                        amparsedemuxctlind( &msgp, &deviceservice, &opcode, 
                            &devicepresent, &eranumber, &errstring[0]);
                        /* If modem status change indication record locally */
                        if( ( opcode == 0 ) &&
                            ( pkgcntrlbkp->apdxairstate.apdxairchgera < 
                                eranumber) )
                        {
                            pkgcntrlbkp->apdxairstate.apdxairserverpresent = 
                                devicepresent;
                            pkgcntrlbkp->apdxairstate.apdxairchgera = eranumber;
                        }
                        /* If there is a registered callback activate 
                         * the callback
                         */
                        if (ipccntrlbkp->apdxctlcbfp)
                        {
                            (ipccntrlbkp->apdxctlcbfp)( 
                                deviceservice, 
                                &devicepresent, 
                                ipccntrlbkp->apdxctluserp);
                        }
                        break;
            
                    case AMDEMUXDATAIND:
                        /* Parse Demux Data Indication */
                        amparsedemuxdataind( &msgp, &deviceservice );
                        if (msgp)
                        {
                            /* calculate payload length */
                            payldoffset =     msgp - inbufp; 
                            msglen = apdxipcbuflng - payldoffset;
                        }
                        
                        /* If there is a registered callback activate 
                         * the callback
                         */
                        if (ipccntrlbkp->apdxdatacbfp)
                        {
                            (ipccntrlbkp->apdxdatacbfp)(
                               deviceservice, 
                               msgp, 
                               msglen, 
                               ipccntrlbkp->apdxdatauserp);                    
                        }
                        break;
            
                    default:
                        /* Do nothing */            
                        break;
                }
                retcode = SWI_RCODE_OK; 
            }
            /* else no data was received */
            else
            {
                /* Timeout condition */
                /* go back to waiting for message */
            }
        }
        else /* bad result */
        {
            /* error in receiving on notification channel */
            retcode = SWI_RCODE_FAILED;
            break;
        }
    }
    return retcode;
}


/**
 *
 * This function is used to obtain the payload pointer within  Demux Data 
 * Indication packet.
 *
 * If an error is returned or to re-initialize the API, 
 * SwiApiDxShutdown must be called before calling this function 
 * again.
 *
 * Looking in the system log after a failure may provide more 
 * details about the cause
 *
 * \param  bufferp: [IN]  pointer to the beginning of caller-allocated buffer 
 *                       which will contain data to send
 * \param  datalen: [IN]  length of data to be send 
 * \param  payloadpp: [OUT] pointer to pointer to beginning of payload
 * \param  bytesbuildp: [OUT]  number of bytes added at the beginning of the
 *                            buffer (i.e. the number of bytes in the header)
 * 
 * \return    SWI_RCODE_OK           - API data indication send  successfully
 * \return    SWI_RCODE_INVALID_PAR  - data length requested is too long
 * \return    See SWI_RCODE for other error return codes
 *
 */       
SWI_API SWI_RCODE SwiGetDataPldOffset(
    swi_uint8 *bufferp, 
    swi_uint32 datalen,
    swi_uint8 **payloadpp,
    swi_uint32 *bytesbuildp)
{
    swi_uint8 *msgarrayp;
    
    msgarrayp = bufferp; /* local pointer to caller's start of  buffer */
    
    /* Check whether we can accomodate data */
    if (datalen > ( APDXBUFSIZE - APDXDATAINDOFF ) )
    {
        return SWI_RCODE_INVALID_PAR;    /* data too long */    
    }
    
    /* Obtain pointer to payload */
    *payloadpp = msgarrayp + APDXDATAINDOFF;
    
    /* Set the number of bytes in the offset */
    *bytesbuildp = APDXDATAINDOFF;    

    /* Mark that the caller has called this function */
    *(swi_uint32 *)msgarrayp = APDXDATAHDRMARK;
    
    return SWI_RCODE_OK;
}

/* 
 * $Log: SwiApiDemux.c,v $
 */


