/**
 *    \if CvsId 
 *    $Id: SwiApiCmLbs.c,v 1.6 2010/06/10 00:15:38 epasheva Exp $
 *    \endif
 *    \ingroup cmlbs
 *    \file SwiApiCmLbs.c
 * 
 *    \brief  Contains source code for common APIs related to the 
 *            Location Based Services.
 *
 *    Copyright: © 2008 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "SwiApiCmLbs.h"
#include "SwiIntPkCdmaLbs.h"
#include "SwiIntUpkCdmaLbs.h"
#include "SwiIntUpkCmLbs.h"
#include "SwiIntPkCmLbs.h"
#include "ap/apidefs.h"
#include "sl/sludefs.h"

/**
 *
 * This function is used to initiate a location fix.
 * 
 * If the location fix initiation is successful, the following notifications
 * will inform the host of the location fix progress and termination:
 *
 *        - SWI_NOTIFY_LbsPdDone
 *        - SWI_NOTIFY_LbsPdData
 *        - SWI_NOTIFY_LbsPdEnd
 *        - SWI_NOTIFY_LbsPdBegin
 *        - SWI_NOTIFY_LbsPdUpdateFailure
 *        - SWI_NOTIFY_LbsSatInfo
 *                
 * \param      psLbsPdGetPos:    [IN]     
 *                     A pointer to a SWI_STRUCT_LbsPdGetPos structure which 
 *                     contains the location fix configuration.
 *
 * \param      timeout:    [IN]         
 *                     Function timeout in milliseconds.
 *
 * \return SWI_RCODE_OK is returned if the modem is able to initiate a 
 *                     location fix.  
 *
 * \return     If the modem is unable to initiate the location fix, an error
 *             code will be returned.  See SwiRcodes.h for additional 
 *             information.    
 *
 */
package SWI_API SWI_RCODE SwiSetLbsPdGetPos(
    const struct SWI_STRUCT_LbsPdGetPos *psLbsPdGetPos, 
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
    parmlength = SwiPackLbsPdGetPos( outparamp, psLbsPdGetPos );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_LBS_PD_GET_POS, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    if(SWI_RCODE_OK == resultcode)
    {
        swi_uint16 nErrType;

        piget16(&inparamp);    /* Discard obj_ver */
        nErrType = piget16(&inparamp);
        if (nErrType != SWI_LBSPDERR_NoError)
        {
            resultcode = (SWI_RCODE)(SWI_RCODE_LBS_PD_NO_ERROR + nErrType);
        }
    }

    /* All done, send the result to the caller */
    return resultcode;


}

/**
 *
 * This function requests that the modem provide the current status of the 
 * LBS.
 *                
 * \param      psLbsPdGetStatus    [OUT]     
 *                     A pointer to a SWI_STRUCT_LbsPdGetStatus structure 
 *                     which contains the current status of the LBS.
 *
 * \param      timeout:    [IN]             
 *                     Function timeout in milliseconds.
 *
 * \return See SWI_RCODE for additional information.
 *
 */
package SWI_API SWI_RCODE SwiGetLbsPdStatus(
    struct SWI_STRUCT_LbsPdGetStatus *psLbsPdGetStatus, 
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         paramlength; /* Returned length of the CnS parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Normally a Pack function call would go here, but this 
     * request has no CnS Parameter field to pack
     */

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_LBS_PD_GET_STATUS, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &paramlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackLbsPdStatus( inparamp, psLbsPdGetStatus );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function requests that the modem start a tracking session.
 * 
 * If the track session was initiated successfully, the following 
 * notifications will inform the host of the location fix progress and 
 * termination:
 *
 *    - SWI_NOTIFY_LbsPdDone
 *    - SWI_NOTIFY_LbsPdData
 *    - SWI_NOTIFY_LbsPdEnd
 *    - SWI_NOTIFY_LbsPdBegin
 *    - SWI_NOTIFY_LbsPdUpdateFailure
 *    - SWI_NOTIFY_LbsSatInfo
 *
 *    This command should be used when an application requires multiple
 * location fixes over a small period of time (ex. a fix between 
 * every 1 to 240 seconds).
 *
 *    Due to the potential need for first updating the ephemeris, almanac 
 * and/or location data, the "time to first fix" may require more time than 
 * the subsequent fixes.  Tracking applications need to consider this possible
 * initial delay and may want to behave "pro actively" by initiating a Data
 * Download session and/or a single location fix as part of their startup 
 * sequence.  As a guideline, almanac data is valid for 3 to 4 days, 
 * ephemeris data is valid for 30 to 120 minutes, coarse location data is 
 * valid for 4 minutes.
 *                
 * \param      psLbsPdTrack: [IN]     
 *                     A pointer to a SWI_STRUCT_LbsPdTrack structure which 
 *                     contains the track session configuration.
 *
 * \param      timeout: [IN]    
 *                     Function timeout in milliseconds.
 *
 * \return SWI_RCODE_OK is returned if the modem is able to initiate a 
 *                     location fix.  
 *
 * \return If the modem is unable to initiate the location fix, an error
 *                     code will be returned.  See SwiRcodes.h for additional 
 *                     information.    
 *
 */
package SWI_API SWI_RCODE SwiSetLbsPdTrack(
    const struct SWI_STRUCT_LbsPdTrack *psLbsPdTrack, 
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
    parmlength = SwiPackLbsPdTrack( outparamp, psLbsPdTrack );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_LBS_PD_TRACK, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    if(SWI_RCODE_OK == resultcode)
    {
        swi_uint16 nErrType;

        piget16(&inparamp);    /* Discard obj_ver */
        nErrType = piget16(&inparamp);
        if (nErrType != SWI_LBSPDERR_NoError)
        {
            resultcode = (SWI_RCODE)(SWI_RCODE_LBS_PD_NO_ERROR + nErrType);
        }
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function is used to end current LBS tracking session.
 * 
 * This API will respond with a SWI_NOTIFY_LbsPdDone/SWI_NOTIFY_LbsPdEnd 
 * notification that includes the End status.
 *
 * There may be one of each type of session active simultaneously.
 *                
 * \param      psLbsPdEndSession: [IN]     
 *                     A pointer to a SWI_STRUCT_LbsPdEndSession structure 
 *                     which contains the current tracking session to 
 *                     terminate.
 *
 * \param      timeout:    [IN]             
 *                     Function timeout in milliseconds.
 *
 * \return See SWI_RCODE for additional information.    
 *
 */
package SWI_API SWI_RCODE SwiSetLbsPdEndSession(
    const struct SWI_STRUCT_LbsPdEndSession *psLbsPdEndSession, 
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
    parmlength = SwiPackLbsPdEndSession( outparamp, psLbsPdEndSession );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_LBS_PD_END_SESSION, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    if(SWI_RCODE_OK == resultcode)
    {
        swi_uint16 nErrType;

        piget16(&inparamp);    /* Discard obj_ver */
        nErrType = piget16(&inparamp);
        if (nErrType != SWI_LBSPDERR_NoError)
        {
            resultcode = (SWI_RCODE)(SWI_RCODE_LBS_PD_NO_ERROR + nErrType);
        }
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function requests the modem's default parameter settings.
 *                
 * \param    psLbsPdGetParam:  [OUT]     
 *             A pointer to a SWI_STRUCT_LbsPaGetParam structure which contains
 *             the position determination parameters.
 *
 * \param    timeout: [IN]     
 *                     Function timeout in milliseconds.
 *
 * \return See SWI_RCODE for additional information.    
 *
 */
package SWI_API SWI_RCODE SwiGetLbsPaParam(
    struct SWI_STRUCT_LbsPaGetParam *psLbsPdGetParam, 
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         paramlength; /* Returned length of the CnS parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Normally a Pack function call would go here, but this 
     * request has no CnS Parameter field to pack
     */

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_LBS_PA_GET_PARAM, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &paramlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackLbsPaParam( inparamp, psLbsPdGetParam );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function gets the results of the position fix.
 *                
 * \param      psLbsPdData: [OUT]        
 *                     A pointer to a SWI_STRUCT_LbsPdData structure which 
 *                     contains the position fix results.
 *
 * \param      timeout: [IN]            
 *                     Function timeout in milliseconds.
 *
 * \return     An error will be returned if there is no position fix 
 *                     information to report.  See SWI_RCODE for additional 
 *                     information.
 *
 */
package SWI_API SWI_RCODE SwiGetLbsPdData(
    struct SWI_STRUCT_LbsPdData *psLbsPdData, 
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         paramlength; /* Returned length of the CnS parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Normally a Pack function call would go here, but this 
     * request has no CnS Parameter field to pack
     */

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_LBS_PD_DATA, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &paramlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackLbsPdData( inparamp, psLbsPdData );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function is used to get GPS IP Address settings from the modem.
 * 
 * The PDE IP Address is a write only field and is not to be displayed to
 * users.  This API will return:
 *    - 0.0.0.0         : The IP address is 0.0.0.0 or PDE NV Item is inactive
 *               (GPS not supported).
 *    - 255.255.255.255 : The device has valid PDE IP address. 
 *    - IP Address      : The device is internally unlocked and will return
 *                        the true address.
 *                
 * \param      psPaIpAddr: [OUT]        
 *                     A pointer to a SWI_STRUCT_LbsPaIpAddr structure 
 *                     which contains the TCP/IP address information.
 *
 * \param      timeout: [IN]    
 *                     Function timeout in milliseconds.
 *
 *\return See SWI_RCODE for additional information.    
 *
 */
package SWI_API SWI_RCODE SwiGetLbsPaIpAddr(
    struct SWI_STRUCT_LbsPaIpAddr *psPaIpAddr,
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         paramlength; /* Returned length of the CnS parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Normally a Pack function call would go here, but this 
     * request has no CnS Parameter field to pack
     */

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_LBS_PA_IP_ADDR, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &paramlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        piget16(&inparamp);    /* Discard obj_ver */
        psPaIpAddr->nIpAddress = piget32(&inparamp);

    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function is used to set current GPS IP Address.
 *                
 * \param      psPaIpAddr: [IN]        
 *                     A pointer to a SWI_STRUCT_LbsPaIpAddr structure which 
 *                     contains the TCP/IP address information.
 *
 *\param       timeout: [IN]        
 *                     Function timeout in milliseconds.
 *
 * \return See SWI_RCODE for additional information.
 *
 */
package SWI_API SWI_RCODE SwiSetLbsPaIpAddr(
    const struct SWI_STRUCT_LbsPaIpAddr *psPaIpAddr, 
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
    parmlength = SwiPackLbsPaIpAddr( outparamp, psPaIpAddr );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_LBS_PA_IP_ADDR, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    if(SWI_RCODE_OK == resultcode)
    {
        swi_uint16 nErrType;

        piget16(&inparamp);    /* Discard obj_ver */
        nErrType = piget16(&inparamp);
        if (nErrType != SWI_LBSPAERR_NoError)
        {
            resultcode = (SWI_RCODE)(SWI_RCODE_LBS_PA_NO_ERROR + nErrType);
        }
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function is used to get the GPS port ID used in LBS sessions.
 *                
 * \param      psPaPortId: [OUT]        
 *                     A pointer to a SWI_STRUCT_LbsPaPortId structure which 
 *                     contains the port ID.
 *
 * \param      timeout: [IN]           
 *                     Function timeout in milliseconds.
 *
 * \return See SWI_RCODE for additional information.
 *
 */
package SWI_API SWI_RCODE SwiGetLbsPaPortId(
    struct SWI_STRUCT_LbsPaPortId *psPaPortId, 
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         paramlength; /* Returned length of the CnS parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Normally a Pack function call would go here, but this 
     * request has no CnS Parameter field to pack
     */

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_LBS_PA_PORT_ID, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &paramlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        piget16(&inparamp);    /* Discard obj_ver */
        psPaPortId->nPortId = piget32(&inparamp);

    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function is used to set the GPS port ID used in LBS sessions.
 *                
 * \param      psPAPortID: [IN]        
 *                     A pointer to a SWI_STRUCT_LbsPaPortId structure which 
 *                     contains the port ID.
 *
 * \param      timeout: [IN]    
 *                     Function timeout in milliseconds.
 *
 * \return See SWI_RCODE for additional information.
 *
 */
package SWI_API SWI_RCODE SwiSetLbsPaPortId(
    const struct SWI_STRUCT_LbsPaPortId *psPAPortID, 
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
    parmlength = SwiPackLbsPaPortId( outparamp, psPAPortID );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_LBS_PA_PORT_ID, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    if(SWI_RCODE_OK == resultcode)
    {
        swi_uint16 nErrType;

        piget16(&inparamp);    /* Discard obj_ver */
        nErrType = piget16(&inparamp);
        if (nErrType != SWI_LBSPAERR_NoError)
        {
            resultcode = (SWI_RCODE)(SWI_RCODE_LBS_PA_NO_ERROR + nErrType);
        }
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * Use this function to get current GPS satellite information.
 *                
 * \param      psSatInfo: [OUT]        
 *                     A pointer to a SWI_STRUCT_LbsSatInfo structure which 
 *                     contains satellite information.
 *
 * \param      timeout: [IN]        
 *                     Function timeout in milliseconds.
 *
 * \return See SWI_RCODE for additional information.
 *
 */
SWI_API SWI_RCODE SwiGetLbsSatInfo(
    struct SWI_STRUCT_LbsSatInfo *psSatInfo, 
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         paramlength; /* Returned length of the CnS parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Normally a Pack function call would go here, but this 
     * request has no CnS Parameter field to pack
     */

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_LBS_SAT_INFO, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &paramlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackLbsSatInfo( inparamp, psSatInfo );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function requests that the modem clear various location parameters to 
 * simulate a cold start.
 *                
 * \param psLbsClearAssistance: [IN]        
 *            A pointer to a SWI_STRUCT_LbsClearAssistance structure which
 *            contains parameters to clear.
 *
 *\param timeout: [IN]        
 *            Function timeout in milliseconds.
 *
 * \return See SWI_RCODE for additional information.
 *
 */
package SWI_API SWI_RCODE SwiSetLbsClearAssistance(
    const struct SWI_STRUCT_LbsClearAssistance *psLbsClearAssistance, 
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
    parmlength = SwiPackLbsClearAssistance( outparamp, psLbsClearAssistance );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_LBS_CLEAR_ASSISTANCE, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* All done, send the result to the caller */
    return resultcode;

}

/* 
 * $Log: SwiApiCmLbs.c,v $
 */
