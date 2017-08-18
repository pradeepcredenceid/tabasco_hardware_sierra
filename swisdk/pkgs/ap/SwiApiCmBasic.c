/**
 *    \if CvsId 
 *    $Id: SwiApiCmBasic.c,v 1.44 2010/06/10 00:13:35 epasheva Exp $
 *    \endif
 *    \ingroup cmbasic
 *    \file SwiApiCmBasic.c
 * 
 *    \brief  Contains source code for Common API functions for the
 *            the SDK. Common means common to all wireless data
 *            technologies, CDMA, GSM, etc. 
 *
 *    Copyright: © 2008 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "SwiApiCmBasic.h"
#include "am/amudefs.h"
#include "ap/apidefs.h"

/* Defines */

/**
 *
 * This function is used to start the API sub-system.  
 *
 * If an error is returned or to re-initialize the API, 
 * SwiApiShutdown must be called before calling this function 
 * again.
 *
 * After calling this function, SwiGetAvailAirServers can be 
 * called to retrieve a list of air servers (devices).  
 * SwiSelectAirServer must be used to commit to a specific 
 * device before calling any control and status API functions.
 *
 * Looking in the system log after a failure may provide more 
 * details about the cause
 *
 * \note SwiSelectAirServer is not implemented in LinuxSDKV2
 *
 * \param     pOpenParams: [IN] parameters to initialize the API in 
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
 * \return    See SWI_RCODE for other error return codes
 *
 */
SWI_API SWI_RCODE SwiApiStartup(struct SWI_STRUCT_ApiStartup *pOpenParams)
{
    swi_uint8 *pathnamep;  /* Resolves to the path to the executable */
    struct apicb *apicbp;
    SWI_RCODE retcode;

    /* Initialize the package global control block pointer */
    apicbp = SwiApiGetCbp();

    /* Not opened yet */
    apicbp->apiopened = 0;
    
    /* Initialize Air Server status structure */
    apicbp->apairstate.apairserverpresent = FALSE;
    apicbp->apairstate.apairchgera        = 0;
    apicbp->apairstate.apairdevice        = SWI_DEVICE_None;
    apicbp->apapprunning                  = TRUE;
    apicbp->apairstate.apairtechtype      = SWI_WIRELESSTECH_Unknown;
    /* Clear product string name buffer */
    slmemset( (char *)apicbp->apairstate.apairprodstr, 0, 
                sizeof (apicbp->apairstate.apairprodstr));

    /* Initialize user callback  pointer */
    apicbp->apcallbackp = NULL;

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
         * issue an error and return
         */
        retcode = SWI_RCODE_SYSTEM_ERROR;
        return retcode;
    }
    
    if( swi_osapisdkprocesscreate( (const char *)pathnamep ) )
    {
        /* So far things are fine, update the return code */
        apicbp->apiopened = APINITIALIZED;
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
    if (!aminitapi())
    {
        /* Failed this last check, so we must remember we're not open */
        apicbp->apiopened = 0;
        retcode = SWI_RCODE_BUSY;
    }
    /* Set Wait notification pointer to NULL, non-NULL means that the 
     * notification thread has already been started. 
     */
    apicbp->apwaitnotiffnp = NULL;
    return retcode;
}
/* RILSTART */
/**
 *
 * Gracefully terminate the SDK process
 *
 * The SDK process is designed to run forever once it is started, behaving
 * as a daemon under Linux in the truest sense of the word. However in some
 * applications there may be a requirement to shut down the daemon when a 
 * particular task has completed and this API allows an application to 
 * gracefully shut down the daemon
 *
 * \param   None
 *
 * \return  None
 *
 * \return  SWI_RCODE_OK
 *
 */
SWI_API SWI_RCODE SwiTerminateSDK( void )
{
    swi_osapistopsdk();

    return SWI_RCODE_OK;
}
/* RILSTOP */
/**
 *
 * This function is used to retrieve a list of available air servers.
 *
 * Air servers are deemed available if the appropriate driver interface, is
 * found.  These interfaces must be capable of supporting a control and status
 * command path to the air server.  SwiSelectAirServer must be called to bind
 * to the air server before any command and status functions are called.
 *
 * Air servers locked for exclusive use by an application are also included.
 *
 * \param   [OUT] sList: 
 *          A structure of type SWI_STRUCT_AirServerList which contains a
 *          list of available air servers.
 *
 * \return  SWI_RCODE_OK:       
 *          List of available servers is successfully retrieved.
 *
 * \return  SWI_RCODE_FAILED:   
 *          Failed to retrieve the list of available air servers.
 *
 * \sa SWI_STRUCT_AirServerList
 * \sa SwiSelectAirServer
 * \sa See sample code, ConnectApi
 */
SWI_API SWI_RCODE SwiGetAvailAirServers(
    struct SWI_STRUCT_AirServerList *sList)
{
    SWI_RCODE retcode;
    swi_bool  present;
    
    /* Clears caller's buff so that the unused portion will be NULLed */
    slmemset( (char *)sList, 0, sizeof (struct SWI_STRUCT_AirServerList));
    
    /* Initialize to none present */
    present = FALSE;

    /* send request to SDK */
    retcode =  SwiIntSendnWaitAirServerRequest(AMAIRSERVERAVAIL, 
                                               &present,
                                               &sList->AirServers[0]);
    if(present)
    {
        sList->nNumber = 1;
    }
    
    return(retcode);
}

/**
 *
 * This function is used to bind the API to a specific air server.
 *
 * Use SwiGetAvailAirServers to retrieve a list of available air servers.
 * Modem control and status API calls can not be invoked until this call is
 * completed.  The API can only bind to one air server at a time.  
 *
 * If the API is already bound to an air server, calling this function will 
 * cancel all pending requests and re-bind to the new air server.  
 *
 * \param  [IN] airServer: 
 *         Air server you are connecting to. 
 *         This tells the API what services are supported.
 *
 * \return SWI_RCODE_OK:
 *         API successfully bound to air server
 *
 * \return SWI_RCODE_FAILED:
 *         API failed to bind to new air server.  
 *
 * \return See SWI_RCODE for other error return codes.
 * 
 * \sa SWI_STRUCT_AirServer
 *
 */
SWI_API SWI_RCODE SwiSelectAirServer(
    struct SWI_STRUCT_AirServer *airServer)
{
    return SWI_RCODE_OK;
}

/**
 *
 * This function is used to retrieve additional information about a
 * particular air server.
 *
 * See SWI_STRUCT_AirServerExtended for list of additional information that 
 * can be retrieved.
 *
 * \param  [IN] sAirServer:    
 *         A structure of type SWI_STRUCT_AirServer which specifies the
 *         the air server of interest.
 *
 * \param  [OUT]sInfo:         
 *         A structure of type SWI_STRUCT_AirServerExtended which contains
 *         additional information about the requested air server.
 *
 * \return SWI_RCODE_OK: 
 *         Additional information successfully retrieved.
 *
 * \return See SWI_RCODE for other error return codes.
 * 
 * \sa SWI_STRUCT_AirServer
 * \sa SWI_STRUCT_AirServerExtended
 *
 */
SWI_API SWI_RCODE SwiGetAirServerInfo(
    struct SWI_STRUCT_AirServer sAirServer, 
    struct SWI_STRUCT_AirServerExtended *sInfo)
{
    return SWI_RCODE_OK;
}

/**
 *
 * This function is used to retrieve the version of the SDK.
 *
 * \param  sdkversionpp : [OUT]
 *         pointer to pointer to returned string.
 *
 * \return SWI_RCODE_OK: 
 *         Information successfully retrieved.
 *
 * \return See SWI_RCODE for other error return codes.
 *
 */
SWI_API SWI_RCODE SwiGetSdkVersion(
    swi_charp *sdkversionpp)
{
    *sdkversionpp = (swi_charp)amgettagname();
    return SWI_RCODE_OK; /* always */
}

/**************** Common APIs ********************/

/**
 *
 * This function is used to retrieve the last error that occurred
 * during a CnS transaction.
 *
 * Recommended length for the CnS buffer is 256 bytes.  
 *
 * \param      szError: [OUT]    CnS transaction error
 * \param      sizeBuffer: [IN]  length of buffer provided for the CnS
 *                               transaction error
 *
 * \return     None
 *
 */
SWI_API void SwiGetLastError(swi_char *szError, swi_uint32 sizeBuffer)
{
    /* Get the AP package control block pointer */
    struct apicb *apicbp = SwiApiGetCbp();

    /* Fill the destination buffer with NULL so that no matter
     * how long the error string is, the result will always be    
     * NULL-terminated
     */
    slmemset( (char *)szError, 0x0, sizeBuffer );

    /* Put the string into the buffer provided by the caller */
    slstrncpy( szError, apicbp->aplasterror, sizeBuffer-1 );
}

/**
 *
 * This function returns the device's firmware version in a string.  
 *
 * The firmware version string varies in length. It is recommended that a 
 * character string of 80 characters be allocated to store this return 
 * value
 *         
 * \param      szFWVersion: [OUT]  
 *                     Firmware version (NULL terminated)
 *
 * \param      sizeBuffer: [IN] 
 *                     Size of buffer provided for the firmware version.  
 *
 * \param      timeout: [IN] 
 *                     Response timeout value in milliseconds
 *
 * \return     See SWI_RCODE for additional information.
 *
 */
SWI_API SWI_RCODE SwiGetFirmwareVersion(
    swi_charp szFWVersion, 
    swi_uint32 sizeBuffer, 
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */
    
    if((szFWVersion == NULL) || (sizeBuffer == 0))
        return(SWI_RCODE_SMALL_BUF);

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Normally a Pack function call would go here, but this 
     * request has no CnS Parameter field to pack
     */

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_FIRMWARE_VERSION, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength );
                     
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        /* Clears caller's buff so that the unused portion will be NULLed */
        slmemset( (char *)szFWVersion, 0, sizeBuffer);

        /* Truncate the size, if necessary */
        if( parmlength > sizeBuffer )
            parmlength = sizeBuffer;

        /* Copy to the caller's buffer */
        slmemcpy( (char *)szFWVersion, inparamp, parmlength );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function returns the date the device's firmware was created.
 *         
 * \param      szBuildDate: [OUT] 
 *                     Firmware build date.  The firmware build date 
 *                     string is in the form of MM/DD/YY.
 *
 * \param      sizeBuffer: [IN] 
 *                     Size of buffer provided for the firmware build date
 *
 * \param      timeout: [IN] 
 *                     Response timeout value in milliseconds
 *
 * \return     See SWI_RCODE for additional information.   
 *
 */
SWI_API SWI_RCODE SwiGetFirmwareBuildDate(
    swi_charp szBuildDate, 
    swi_uint32 sizeBuffer, 
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */
    
    if((szBuildDate == NULL) || (sizeBuffer < 8))
        return(SWI_RCODE_SMALL_BUF);

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Normally a Pack function call would go here, but this 
     * request has no CnS Parameter field to pack
     */

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_FIRMWARE_BUILD_DATE, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength );
                     
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        /* Clears caller's buff so that the unused portion will be NULLed */
        slmemset( (char *)szBuildDate, 0, sizeBuffer);

        /* Truncate the size, if necessary */
        if( parmlength > sizeBuffer )
            parmlength = sizeBuffer;

        /* Copy to the caller's buffer */
        slmemcpy( (char *)szBuildDate, inparamp, parmlength );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function returns the device's hardware version in a string.
 *         
 * \param      szHWVersion: [OUT]  
 *                     Hardware version (NULL terminated).  The firmware 
 *                     version string varies in length. It is recommended 
 *                     that a character string of 80 characters be allocated 
 *                     to store this return value.  The returned string is 
 *                     null terminated.
 *
 * \param      sizeBuffer: [IN]   
 *                     Size of buffer provided for the hardware version
 *
 * \param      timeout: [IN]    
 *                     Response timeout value in milliseconds
 *
 * \return     See SWI_RCODE for additional information.   
 *
 */
SWI_API SWI_RCODE SwiGetHardwareVersion(
    swi_charp szHWVersion, 
    swi_uint32 sizeBuffer, 
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */

    if((szHWVersion == NULL) || (sizeBuffer < 3))
        return(SWI_RCODE_SMALL_BUF);
        
    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Normally a Pack function call would go here, but this 
     * request has no CnS Parameter field to pack
     */

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_HARDWARE_VERSION, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength );
                     
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        /* Clears caller's buff so that the unused portion will be NULLed */
        slmemset( (char *)szHWVersion, 0, sizeBuffer);

        /* Truncate the size, if necessary */
        if( parmlength > sizeBuffer )
            parmlength = sizeBuffer;

        /* Copy to the caller's buffer */
        slmemcpy( (char *)szHWVersion, inparamp, parmlength );
    }

    /* All done, send the result to the caller */
    return resultcode;


}
/**
 *
 * This function returns the device's firmware loader version in a string. 
 *         
 * \param      szBVVersion: [OUT]  
 *                     Bootloader version (NULL terminated).  The version 
 *                     string varies in length. It is recommended that a 
 *                     character string of 80 characters be allocated to 
 *                     store this return value.
 *
 * \param      sizeBuffer: [IN]    
 *                     Size of buffer provided for the bootloader version
 *
 * \param      timeout: [IN]    
 *                     Response timeout value in milliseconds
 *
 * \return     See SWI_RCODE for additional information.
 *
 */
SWI_API SWI_RCODE SwiGetBootVersion(
    swi_charp szBVVersion, 
    swi_uint32 sizeBuffer, 
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */
    
    /* validate the Boot Version Buffer */
    if((szBVVersion == NULL) ||
       (sizeBuffer == 0))
    {
        return(SWI_RCODE_SMALL_BUF);
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Normally a Pack function call would go here, but this 
     * request has no CnS Parameter field to pack
     */

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_BOOT_VERSION, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength );
                     
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        /* Clears caller's buff so that the unused portion will be NULLed */
        slmemset( (char *)szBVVersion, 0, sizeBuffer);

        /* Truncate the size, if necessary */
        if( parmlength > sizeBuffer )
            parmlength = sizeBuffer;

        /* Copy to the caller's buffer */
        slmemcpy( (char *)szBVVersion, inparamp, parmlength );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function is used to retrieve the boot loader build date.
 *         
 * \param      szBLBldDate: [OUT]    
 *                     Pointer to store the build date information
 *
 * \param      nLength: [IN/OUT]    
 *                     Size of buffer available to store the build date 
 *                     information/required buffer space
 *
 * \param      Timeout: [IN]       
 *                     Response timeout value in milliseconds
 *
 * \return     SWI_RCODE_SMALL_BUF is returned if the specified buff 
 *                     isn't large enough to store the entire build date 
 *                     information.  Check nLength to see the required 
 *                     buffer size.  
 *
 * \return See SWI_RCODE for additional information.
 *
 */
SWI_API SWI_RCODE SwiGetBootloaderBuildDate(
    swi_charp szBLBldDate, 
    swi_uint32 *nLength,
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */
    swi_uint16         objver;     /* CnS Object Version */


    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Normally a Pack function call would go here, but this 
     * request has no CnS Parameter field to pack
     */

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_BOOT_BUILD_DATE,
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength );
                     
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        /* Skip over the object version field in the parameter section */
        objver = piget16( &inparamp );
        parmlength -= sizeof( objver );

        /* Clears caller's buff so that the unused portion will be NULLed */
        slmemset( (char *)szBLBldDate, 0, *nLength);

        /* if buffer is too small */
        if( parmlength > *nLength )
        { 
            *nLength = parmlength;
            return(SWI_RCODE_SMALL_BUF); 
        }

        /* Copy to the caller's buffer */
        slmemcpy( (char *)szBLBldDate, inparamp, parmlength );

        /* TBD - Add version number check */
    }

    /* All done, send the result to the caller */
    return resultcode;
}

#if 0
/**
 *          This function is used to retrieve the roaming state.
 *          The modem is either roaming or on the home network.
 *
 * \param   eRoaming[OUT]:        roaming state
 * \param   timeout[IN]:        response timeout value in milliseconds
 *
 * \return  See SWI_RCODE for additional information.
 *
 * \note    If this function is called while the modem is not in coverage, it 
 *          will report being at home. The modem cannot verify that you are
 *          roaming until it can detect coverage and identify the carrier(s). 
 *          Use of this function should be limited. There is a notification 
 *          available in the GPRS API (SWI_NOTIFY_NetworkStatus) that reports
 *          this data along with other useful service parameters. Use of 
 *          notifications is preferred over direct polling.
 *
*/
SWI_API SWI_RCODE SwiGetRoamingState(SWI_TYPE_RoamingState *eRoaming, 
                                     swi_uint32 timeout)
{

}
#endif

/**
 *
 * This function is used to retrieve the USB descriptor build 
 * information.
 *            
 * \param   pUsbd: [OUT] structure to store USB descriptor build information
 *
 * \return:       See SWI_RCODE for additional information.
 *
 */
SWI_API SWI_RCODE SwiGetUsbdInfo(struct SWI_STRUCT_UsbdInfo* pUsbdInfo, 
                                 swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Normally a Pack function call would go here, but this 
     * request has no CnS Parameter field to pack
     */

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_USBD_BUILD_INFO, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength );
                     
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        /* Clears caller's buff so that the unused portion will be NULLed */
        slmemset( (char *)pUsbdInfo, 0, sizeof (struct SWI_STRUCT_UsbdInfo));

        /* Copy to the caller's buffer - no error checking here */
        SwiUpkUsbdInfo (inparamp, pUsbdInfo );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function is used to retrieve PRI information of the device
 *            
 * \param    pPriInfo: [OUT] PRI information
 *           timeout: [IN]  response timeout value is 1000 milliseconds
 *
 * \return   SWI_RCODE_OK - the request has been received.
 * \return   See SWI_RCODE for other error return codes.
 *
 */
SWI_API SWI_RCODE SwiGetPriInfo(struct SWI_STRUCT_PriInfo* pPriInfo, 
                                swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Normally a Pack function call would go here, but this 
     * request has no CnS Parameter field to pack
     */

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PRI_INFO, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength );
                     
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        /* Clears caller's buff so that the unused portion will be NULLed */
        slmemset( (char *)pPriInfo, 0, sizeof (struct SWI_STRUCT_PriInfo));

        /* Copy to the caller's buffer */
        SwiUpkPriInfo (inparamp, pPriInfo);
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function is used to retrieve modem type information.
 *
 * \param  pSwiModemType: [IN/OUT]     
 *         Removable media configuration
 *
 * \param  timeout: [IN]    
 *         Response timeout value in milliseconds.
 *
 * \return SWI_RCODE_OK:
 *         The request has been processed and values returned are valid. 
 *
 * \return See SWI_RCODE for other error return codes.
 *
 */
SWI_API SWI_RCODE SwiGetModemType(struct SWI_STRUCT_ModemType* pSwiModemType, swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Normally a Pack function call would go here, but this 
     * request has no CnS Parameter field to pack
     */

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_MODEM_TYPE, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength );
                     
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        /* Clears caller's buff so that the unused portion will be NULLed */
        slmemset( (char *)pSwiModemType, 0, sizeof (struct SWI_STRUCT_ModemType));

       /* Copy to the caller's buffer */
        SwiUpkModemType(inparamp, pSwiModemType);
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function is used to retrieve the system time on the device.
 *         
 * \param  pDeviceTime: [OUT]      
 *         Pointer to a SWI_STRUCT_DeviceTime structure to store the system
 *         time from the device.
 *
 * \param  timeout: [IN]       
 *         Response timeout value in milliseconds.
 *
 * \return SWI_RCODE_OK:
 *         The request has been received.
 * 
 * \return See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_DeviceTime
 *
 */
SWI_API SWI_RCODE SwiGetDeviceTime(struct SWI_STRUCT_DeviceTime* pDeviceTime, swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Normally a Pack function call would go here, but this 
     * request has no CnS Parameter field to pack
     */

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_DEVICE_TIME,
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength );
                     
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        /* Clears caller's buff so that the unused portion will be NULLed */
        slmemset( (char *)pDeviceTime, 0, sizeof (struct SWI_STRUCT_DeviceTime));

        /* Copy to the caller's buffer */
        SwiUpkDeviceTime (inparamp, pDeviceTime);
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function returns the device's unique identity as a string. The 
 * returned value is the ESN (Electronic Serial Number) or EID 
 * (Electronic Identity) of the modem.
 *
 * The ESN is in the form of an 14-character string of decimal numerals.  
 * The string is null terminated (adding a 15th byte to the length).  The 
 * content of the ESN includes several concatenated components, as specified
 * for GPRS.
 *
 * \param  szDeviceID: [OUT]     
 *         A string representing the Device ID (NULL terminated).
 *
 * \param  sizeBuffer: [IN]     
 *         Size of buffer provided for the device ID.  A buffer of 15 bytes
 *         is recommended.
 *
 * \param  timeout: [IN]     
 *         Response timeout value in milliseconds
 *
 * \return See SWI_RCODE for additional information.   
 *
 */
SWI_API SWI_RCODE SwiGetDeviceID(swi_charp pszDeviceID, 
                                 swi_uint32 sizeBuffer, 
                                 swi_uint32  timeout)
{
    
    /* Note: This is an exact copy of the SwiGetDeviceID code except for the
     * slmemcpy lines. Any changes in SwiGetDeviceID must be made here as well
     * unless this API needs to be different otherwise
     */
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *outparamp; /* ptr to outbound param field */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */
    
    if((pszDeviceID == NULL) ||
       (sizeBuffer == 0))
    {
        return(SWI_RCODE_SMALL_BUF);
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* pack outgoing parameters */
    parmlength = 0;

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp,
                                  CNS_IMEI_EXP,
                                  parmlength,
                                  timeout,
                                  CNS_GETREQ,
                                  SWI_AMGSM,
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        /* Truncate the size, if necessary */
        if( parmlength > sizeBuffer )
            return(SWI_RCODE_SMALL_BUF);

        /* Clears caller's buffer so that the unused portion will be NULLed */
        slmemset( (char *)pszDeviceID, 0, sizeBuffer );

        /* Copy to the caller's buffer */
        slmemcpy( (char *)pszDeviceID, inparamp, parmlength );
    }
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function is used to retrieve firmware flash information from the
 * device.
 *         
 * \param  [IN/OUT] pFlashImgInfo:
 *         Pointer to a SWI_STRUCT_FlashImgInfo which specifies the flash
 *         image type to retrieve.  This structure also contains the flash
 *         image information returned from the device.  See 
 *         SWI_STRUCT_FlashImgInfo for additional information.
 *
 * \param  timeout: [IN]    
 *         Response timeout value in milliseconds.
 *
 * \return SWI_RCODE_OK:
 *         The device has received the request.  See 
 *         SWI_STRUCT_FlashImgInfo::eResult for actual request result.
 *
 * \return See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_FlashImgInfo
 *
 */
SWI_API SWI_RCODE SwiGetFlashImgInfo(struct SWI_STRUCT_FlashImgInfo* pFlashImgInfo, swi_uint32 timeout)
{    
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *outparamp; /* ptr to outbound param field */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Pack the Flash Image information into the CnS parm field */
    parmlength = SwiPkFlashImgInfo( outparamp, 
                                    APOBJVER1, 
                                    pFlashImgInfo);

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_FLASH_IMAGE_INFO, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength );
                     
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {   
        /* Copy to the caller's buffer */
        SwiUpkFlashImgInfoResp (inparamp, pFlashImgInfo);
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function is used to retrieve the object ID range revision 
 * on the modem and the API.
 *
 * \param  sObjRngRev: [OUT]           
 *         Object range revision information
 *
 * \param  timeout: [IN]    
 *         Response timeout value in milliseconds.
 *
 * \return SWI_RCODE_OK:
 *         The request has been processed and values returned are valid. 
 *
 * \return See SWI_RCODE for other error return codes.
 *
 */
SWI_API SWI_RCODE SwiGetObjRngRev(
    struct SWI_STRUCT_ObjRngRev *ObjRngRevp, 
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */
    swi_bool               bUpkRcode;  /* Return code from unpack function */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Normally a Pack function call would go here, but this 
     * request has no CnS Parameter field to pack
     */

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_OBJECT_RANGE_REVISION, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength );
                     
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {   
        /* Copy to the caller's buffer */
        bUpkRcode = SwiUpkObjRngRev (inparamp, ObjRngRevp);
        if (bUpkRcode == FALSE)
            resultcode = SWI_RCODE_CNS_FAILED;
    }

    /* All done, send the result to the caller */
    return resultcode;

}

/*
 * Radio Configuration APIs
 */

/**
 *
 * This function is used to retrieve the current radio power mode.
 *         
 * \param      pRadioPower: [OUT]      
 *                     Pointer to a SWI_STRUCT_RadioPower structure 
 *                     to store 
 *                     the current radio power setting.
 *
 * \param      timeout: [IN]    
 *                     Response timeout value in milliseconds.
 *
 * \return     See SWI_RCODE for other error return codes.
 * 
 * \sa SWI_STRUCT_RadioPower
 *
 */
SWI_API SWI_RCODE SwiGetRadioPower(
    struct SWI_STRUCT_RadioPower *pRadioPower, 
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Normally a Pack function call would go here, but this 
     * request has no CnS Parameter field to pack
     */

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_RADIO_POWER, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength );
                     
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
       SwiUpkRadioPowerResp(inparamp, pRadioPower);
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function is used to configure the radio power mode.
 *         
 * \param      pRadioPower: [IN]       
 *                     A pointer to a SWI_STRUCT_RadioPower structure which 
 *                     specifies the new radio power setting.
 *
 * \param      timeout: [IN]    
 *                     Response timeout value in milliseconds.
 *
 * \return See SWI_RCODE for other error return codes.
 * 
 * \sa SWI_STRUCT_RadioPower
 *
 */
SWI_API SWI_RCODE SwiSetRadioPower(
    struct SWI_STRUCT_RadioPower* pRadioPower, 
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *outparamp; /* ptr to outbound param field */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* pack outgoing parameters */
    parmlength = SwiPkRadioPower( outparamp, APOBJVER1, pRadioPower);

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_RADIO_POWER, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength );
                     
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {   
       SwiUpkRadioPowerResp(inparamp, pRadioPower);
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function is used to retrieve the current radio temperature state.
 * 
 * UMTS AirCards will always report that the temperature state is normal.
 * Temperature state changes can be monitored with SWI_NOTIFY_RadioTempState.
 *         
 * \param      pRadioTemp: [OUT]       
 *                     Pointer to a SWI_STRUCT_RadioTempState to store the 
 *                     current radio temperature state.
 *
 * \param      timeout: [IN]    
 *                     Response timeout value in milliseconds.
 *
 * \return See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_RadioTempState
 * \sa SWI_NOTIFY_RadioTempState   
 *
 */
SWI_API SWI_RCODE SwiGetRadioTempState(
    struct SWI_STRUCT_RadioTempState *pRadioTemp,
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Normally a Pack function call would go here, but this 
     * request has no CnS Parameter field to pack
     */

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_RADIO_TEMPERATURE, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength );
                     
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
       SwiUpkRadioTemp(inparamp, pRadioTemp);
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function is used to retrieve the current radio battery state.
 *
 * Voltage changes can be monitored with SWI_NOTIFY_RadioBattery.
 *         
 * \param      pRadioBattery: [OUT]    
 *                     A pointer to a SWI_STRUCT_RadioBattery structure to 
 *                     store the current radio battery state.
 *
 * \param      timeout: [IN]    
 *                     Response timeout value in milliseconds.
 *
 * \return See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_RadioBattery
 * \sa SWI_NOTIFY_RadioBattery
 *
 */
SWI_API SWI_RCODE SwiGetRadioBattery(
    struct SWI_STRUCT_RadioBattery *pRadioBattery,
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Normally a Pack function call would go here, but this 
     * request has no CnS Parameter field to pack
     */

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_RADIO_BATTERY, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength );
                     
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
       SwiUpkRadioBattery(inparamp, pRadioBattery);
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function is used to determine the current radio band(s) the device is
 * currently using.
 * 
 * This API replaces SwiGetBandInfo.
 *
 *  For GSM/WCDMA devices and CDMA/EVDO devices operating in non-hybrid
 *  mode operation, only one band is supported at a time.  In these
 *  cases, the band info list will only contain a single entry.
 *         
 * \param      pRadioBand: [OUT]       
 *                     Pointer to a SWI_STRUCT_RadioBandList structure to 
 *                     store the list of current radio bands
 *
 * \param      timeout: [IN]    
 *                     Response timeout value in milliseconds.
 *
 * \return See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_RadioBandList        
 *
 */
SWI_API SWI_RCODE SwiGetCurrentRadioBand(
    struct SWI_STRUCT_RadioBandList *pRadioBand, 
    swi_int32  timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Normally a Pack function call would go here, but this 
     * request has no CnS Parameter field to pack
     */

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_CURRENT_RADIO_BAND, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength );
                     
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
       SwiUpkCurrentRadioBand(inparamp, pRadioBand);
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function is used to retrieve the current radio band setting of the
 * device along with the list of all the device supported radio bands.
 *
 * This API replaces SwiGetBandInfo.
 *         
 * \param  pRadioBand: [OUT]       
 *                 A pointer to a SWI_STRUCT_RadioBandConfig structure 
 *                 to store the current radio band configuration.
 *
 * \param  timeout: [IN]    
 *                 Response timeout value in milliseconds.
 *
 * \return See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_RadioBandConfig      
 *
 */
SWI_API SWI_RCODE SwiGetRadioBandCfg(
    struct SWI_STRUCT_RadioBandConfig *pRadioBand, 
    swi_int32  timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Normally a Pack function call would go here, but this 
     * request has no CnS Parameter field to pack
     */

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_CONFIG_RADIO_BAND, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength );
                     
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
       SwiUpkRadioBandCfg(inparamp, pRadioBand);
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function is used to request a new radio band setting.
 * 
 * This API replaces SwiSetBandInfo.
 *
 * Setting the frequency band may cause a modem reset.  The caller
 * should use SwiGetRadioBandCfg to retrieve the list of supported bands
 * before calling SwiSetRadioBandCfg.
 *         
 * \param      nRadioBandGroup: [IN]       
 *                     A bit mask value which specifies the radio band group 
 *                     to use.  See SWI_RADIOBANDGROUP_xxx definitions.
 *    
 * \param  timeout: [IN]    
 *                     Response timeout value in milliseconds.
 *
 * \return     See SWI_RCODE for other error return codes.
 *
 * \sa SWI_RADIOBANDGROUP_xxx      
 *
 */
SWI_API SWI_RCODE SwiSetRadioBandCfg(
    swi_uint64 nRadioBandGroup, 
    swi_int32  timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *outparamp; /* ptr to outbound param field */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* pack outgoing parameters */
    parmlength = SwiPkRadioBandCfg( outparamp, APOBJVER1, nRadioBandGroup);

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_CONFIG_RADIO_BAND, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength );
                     
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {   
       /* nothing but a object version to unpack - so don't bother */
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/*
 * Other APIs
 */

/**
 *
 * This function is used to retrieve the Port Name for the data port.
 *
 * \param   [OUT] szPortName: 
 *              Port Names of the device.
 *
 * \return  SWI_RCODE_OK:       
 *          List of available port names is successfully retrieved.
 *
 * \return  SWI_RCODE_FAILED:   
 *          Failed to retrieve the list of available port names.
 */
SWI_API SWI_RCODE SwiGetUsbPortName(
    struct SWI_STRUCT_UsbPortName *sPortNamep
    )
{
    SWI_RCODE retcode;
    
    /* Clears caller's buff so that the unused portion will be NULLed */
    slmemset( (char *)sPortNamep, 0, sizeof (struct SWI_STRUCT_UsbPortName) );
    
    /* send request to SDK */
    retcode =  SwiIntSendnWaitUsbPortNameRequest(sPortNamep);
     
    return(retcode);
}

/**
 *
 * Fetch the boot and hold state of the modem, if it's connected. There is
 * a small window in which this information will be inaccurate if the state
 * of the modem changes while this function is being called. However, the 
 * notification called SWI_NOTIFY_AirServerChange will always indicate a
 * change in the execution state of the modem and interested callers should
 * re-issue this Api to learn the modem's new Boot and Hold state, if they
 * are expecting it to change.
 *
 * \param    pbootnhold [OUT]: 
 *              TRUE - Modem is operating in boot-and-hold mode
 *              FALSE - Modem is executing its application
 *
 * \param    timeout [IN]: 
 *              Function timeout in milliseconds
 *
 * \return  SWI_RCODE_OK:       
 *              pbootnhold contains a valid boolean value
 *
 * \return  SWI_RCODE_DEVICE_UNAVAIL:   
 *              The modem is either not connected to the host or
 *              the SDK hasn't detected it yet. 
 *              Wait to receive the notification 
 *              SWI_NOTIFY_AirServerChange before retrying this 
 *              request
 */
SWI_API SWI_RCODE SwiGetBootAndHoldMode( swi_bool *pbootnhold, 
                                         swi_uint32 timeout )
{
    /* Get the AP package control block pointer */
    struct apicb *apicbp = SwiApiGetCbp();

    /* If no modem yet detected, simply return with an error */
    if( !apicbp->apairstate.apairserverpresent )
    {
        return SWI_RCODE_DEVICE_UNAVAIL;
    }

    /* Else, there is a modem connected. Return its boot and hold state */
    *pbootnhold = !(apicbp->apapprunning);

    return SWI_RCODE_OK;
}

/**
 *
 * This function Forces the modem to perform a NV update from an EFS file.
 *
 * Note that this function does not return any data via pNVUpdateFromFile.
 * Caller need to set the sizeStruct and szFileName member of 
 * pNVUpdateFromFile. The other members of SWI_STRUCT_NVUpdateFromFile
 * are for notification use only.
 *
 * \param pNVUpdateFromFile[IN]   NV update settings. 
 *                                See SWI_STRUCT_NVUpdateFromFile.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiNVUpdateFromFile(
            SWI_STRUCT_NVUpdateFromFile *pNVUpdateFromFile, 
            swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    if(slstrlen((const char *)pNVUpdateFromFile->szFileName) > SWI_FILENAME_LEN)
    {
        return SWI_RCODE_INVALID_PAR;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Parameter Field Construction Zone - get a pointer to the 
     * location to fill in the CnS parameter field 
     */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Pack the CnS Parameter field with the input parameters */
    parmlength = SwiPackNVUpdateFromFile( outparamp, pNVUpdateFromFile );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_NVUPDATE_FROM_FILE, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */
    
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function allows the host to unlock access to various 
 * password-protected other SWI APIs. Those APIs can be used to configure 
 * carrier-specific parameters and to enable limited diagnostic capabilities. 
 * Those APIs are required by OEMs and device provisioning applications. 
 * This object is not intended for use by end-users..
 *
 * Typical cases to use this API
 *     - Before upload PRL with SwiSetPRL
 *
 * \param pApiUnlock[IN]   See SWI_STRUCT_ApiUnlock.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiApiUnlock(SWI_STRUCT_ApiUnlock *pApiUnlock, 
                               swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    if(pApiUnlock->nPasswordLen > SWI_API_UNLOCK_PWD_LEN)
    {
        return SWI_RCODE_INVALID_PAR;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Parameter Field Construction Zone - get a pointer to the 
     * location to fill in the CnS parameter field 
     */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Pack the CnS Parameter field with the input parameters */
    parmlength = SwiPackApiUnlock( outparamp, pApiUnlock );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_API_UNLOCK, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCOMMON,
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */
    
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK)
    {
        piget16(&inparamp);     /* discard obj_ver */
        pApiUnlock->nResult = *inparamp;
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function retrieves the modem's Remote Diagnostics Agent support 
 * information.
 *
 * \param psRDA[OUT]       Contains RDA support information. 
 *                         See SWI_STRUCT_RDA_SUPPORT.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetRemoteDiagAgentSupport(SWI_STRUCT_RDA_SUPPORT *psRDA, 
                                               swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_RDA_SUPPORT, 
                                  0, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackRemoteDiagAgentSupport( inparamp, psRDA );
        
        if(psRDA->nNameLength > SWI_RDA_NAME_MAX ||
                psRDA->nVersionLength > SWI_RDA_VERSION_MAX)
        {
            resultcode = SWI_RCODE_UNEXPECTED_RESP;
        }
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function enables/disables the modem's Remote Diagnostics Agent.
 *
 * \param bEnable[IN]      Disable = 0, Enable = 1;
 * \param pStatus[OUT]     0=Success, 1=Failed unspecified,2=Failed security 
 *                         access denied.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetRemoteDiagAgentEnable(swi_uint8 bEnable, 
                                              swi_uint8 *pStatus, 
                                              swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Parameter Field Construction Zone - get a pointer to the 
     * location to fill in the CnS parameter field 
     */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Pack the CnS Parameter field with the input parameters */
    parmlength = SwiPackRemoteDiagAgentEnable( outparamp, bEnable );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_RDA_SUPPORT, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        piget16(&inparamp);     /* Discard obj_ver */
        *pStatus = *inparamp;
    }
    
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function retrieves the modem's Removable Media support information
 *
 * \param psRM[OUT]        Contains RM support information. 
 *                         See SWI_STRUCT_RM_CONFIG.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetRemovableMediaSupport(SWI_STRUCT_RM_CONFIG *psRM, 
                                              swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_RM_CONFIG, 
                                  0, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        piget16(&inparamp);     /* Discard obj_ver */
        psRM->nFeatureSupportMask = piget16(&inparamp);
        psRM->nFeatureEnableMask = piget16(&inparamp);
    }
    
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function sets the modem's Removable Media support information.
 *
 * \param psRM[IN]         Contains RM support config. 
 *                         See SWI_STRUCT_RM_CONFIG_SET.
 * \param pStatus[OUT]     Set status.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetRemovableMediaSupport(
        const SWI_STRUCT_RM_CONFIG_SET *psRM, 
        swi_uint8 *pStatus, 
        swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Parameter Field Construction Zone - get a pointer to the 
     * location to fill in the CnS parameter field 
     */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Pack the CnS Parameter field with the input parameters */
    parmlength = SwiPackRemovableMediaSupport( outparamp, psRM );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_RM_CONFIG, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        piget16(&inparamp);     /* Discard obj_ver */
        *pStatus = *inparamp;
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function causes the modem to undergo a soft reset. 
 * SWI_NOTIFY_AirServerChange is generated after the reset has completed.
 * Note that the driver may also unload and reload when a modem
 * reset occurs.  
 *
 * This API will return very quickly but this does not indicate
 * that the modem has reset, only that the reset command has been
 * sent to the modem. When the modem finally resets a  
 * SWI_NOTIFY_AirServerChange notification will be generated. Callers
 * may want to consider calling SwiGetBootAndHoldMode() to determine the 
 * operating state of the modem, however, modem resets generated by this 
 * API will always bring the modem back into full operation after the 
 * reset is complete
 *
 * If the modem is already in boot and hold operation when this command
 * is issued, there will be no effect. The modem does not respond to 
 * reset requests when operating in boot and hold mode.
 *            
 * \param  None
 *
 * \return      See SWI_RCODE for additional information.
 *
 */
SWI_API SWI_RCODE SwiSoftModemReset(void)
{
    swi_uint8        *inparamp;  /* ptr to param field rx'd from modem */
    SWI_RCODE        resultcode;
    struct amrrparms reqparms;

    /* Indicate the type of request we are making */
    reqparms.amparmtype = SWI_PARM_RESET;

    /* Normal soft reset requested */
    reqparms.amparm.amreset.amresettype = FALSE;

    /* No timeout for soft reset requests */
    reqparms.amtimeout = 0;

    /* Dispatch this packet to the SDK side, waiting for a response */
    resultcode = amsendnwait( amgetreqbufp(), &reqparms, &inparamp );

    /* Nothing worth looking at in the returned response */
    return resultcode;
}

/**
 *
 * This function causes the modem to reset into boot and hold operation.
 * Boot and Hold operation is used for upgrading the modem's firmware.
 * For USB-based devices, the driver will unload and reload on a modem
 * reset.  
 *
 * The caller will receive a response very quickly, but receipt of a 
 * response does not indicate the modem's entry into boot and hold mode,
 * only that the command was sent to the modem. The caller's next step
 * should be to await a SWI_NOTIFY_AirServerChange message and when it's
 * received, call SwiGetBootAndHoldMode() to determine the modem's 
 * operating state 
 *
 * If the modem is already in boot and hold operation when this command
 * is issued, there will be no effect. The modem does not respond to 
 * reset requests when operating in boot and hold mode.
 *            
 * \param  timeout[IN]:  response timeout value in milliseconds
 *
 * Return:    SWI_RCODE_OK - request was successfully sent to the modem
 *            SWI_RCODE_REQUEST_TIMEOUT  - request timed out
 *            See SWI_RCODE for other error return codes
 *
 */
SWI_API SWI_RCODE SwiResetToBootHold( swi_uint32 timeout )
{
    swi_uint8        *inparamp;  /* ptr to param field rx'd from modem */
    SWI_RCODE        resultcode;
    struct amrrparms reqparms;

    /* Indicate the type of request we are making */
    reqparms.amparmtype = SWI_PARM_RESET;

    /* Normal soft reset requested */
    reqparms.amparm.amreset.amresettype = TRUE;

    /* No timeout for soft reset requests */
    reqparms.amtimeout = timeout;

    /* Dispatch this packet to the SDK side, waiting for a response */
    resultcode = amsendnwait( amgetreqbufp(), &reqparms, &inparamp );

    /* Nothing worth looking at in the returned response */
    return resultcode;
}

/**
 *
 * This function is used to get fundamental information from the modem.
 * 
 * The information returned is determined by setting the 
 * eDeviceInformationType field of sDeviceInformation_Request.  
 * 
 *  \param	[OUT] sDeviceInformation_Request:
 *          struct must be filled out to select type of information returned.
 *
 *  \param  [IN] sDeviceInformation_Response:
 *          struct returned will contain the requested information.
 *
 *  \param  [IN] timeout:
 *          Response timeout value in milliseconds.
 *
 * \return  See SWI_RCODE for additional information.
 *
 * \sa  SWI_STRUCT_DeviceInformation_Request
 * \sa  SWI_STRUCT_DeviceInformation_Response
 *
 */
SWI_API SWI_RCODE SwiGetDeviceInformation(
    struct SWI_STRUCT_DeviceInformation_Request  *sDeviceInformation_Requestp, 
    struct SWI_STRUCT_DeviceInformation_Response *sDeviceInformation_Responsep,
    swi_uint32 timeout)
{
    swi_uint8        *inparamp;  /* ptr to param field rx'd from modem */
    SWI_RCODE        resultcode;
    struct amrrparms reqparms;

    slmemset ((char *)&reqparms, 0, sizeof (struct amrrparms));

    /* Indicate the type of request we are making */
    reqparms.amparmtype = SWI_PARM_DEV_INFO;

    /* Normal soft reset requested */
    reqparms.amparm.amdevinfo.amdevversion = 
                            sDeviceInformation_Requestp->Version;
    reqparms.amparm.amdevinfo.amdevinfotype =
                            sDeviceInformation_Requestp->eDeviceInformationType;

    /* Timeout to wait for requests */
    reqparms.amtimeout = timeout;

    /* Dispatch this packet to the SDK side, waiting for a response */
    resultcode = amsendnwait( amgetreqbufp(), &reqparms, &inparamp );

    slmemset (  (char *)sDeviceInformation_Responsep, 
                0, 
                sizeof (struct SWI_STRUCT_DeviceInformation_Response));
                
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUpkDeviceInformationResp (inparamp, sDeviceInformation_Responsep);
    }
    
    /* Nothing worth looking at in the returned response */
    return resultcode;
}

/**
 *
 * This function is used to retrieve Software on Card (SWoC) configuration.
 *
 * Configuration items returned determine if:
 *     -SWoC/TRU-Install feature supported/enabled
 *     -SWoC/TRU-Install media in modem mode supported/enabled
 *     -bypassing of the SWoC/TRU-Install feature after a software reboot is
 *      performed on the device
 *
 * Some devices require that the OEM level is unlocked before some or all of
 * the request options are attempted.
 *
 * In order for the ‘SWoC access in modem mode’ feature to be enabled,
 * general overall ‘SWoC’ TRU-Install support must be both supported by
 * the product and currently enabled.
 *
 * \param  pSwocCfg [IN/OUT]
 *
 * \param  timeout [IN]
 *
 * \return SWI_RCODE_OK:
 *         SWoC configuration successfully retrieved
 *
 * \return See SWI_RCODE for other error return codes.
 *
 */
SWI_API SWI_RCODE SwiGetSwocCfg(
    struct SWI_STRUCT_GetSwocCfg *pSwocCfg,
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* no outgoing parameters */
    parmlength = 0;

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp,
                                  CNS_SWoC_CONFIG,
                                  parmlength,
                                  timeout,
                                  CNS_GETREQ,
                                  SWI_AMCOMMON,
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUpkSwocCfg (inparamp, pSwocCfg);
    }

    /* All done, send the result to the caller */
    return resultcode;
}
/*
 * $Log: SwiApiCmBasic.c,v $
 */
