/**
 *    \if CvsId 
 *    $Id: SwiApiGsmLbs.c,v 1.4 2009/02/10 01:35:25 bsiu Exp $
 *    \endif
 *    \ingroup gsmlbs
 *    \file SwiApiGsmLbs.c
 * 
 *    \brief  Contains source code for GSM APIs related to the 
 *            Location Based Services.
 *
 *    Copyright: © 2008 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "SwiApiCmLbs.h"
#include "SwiApiGsmLbs.h"
#include "ap/apidefs.h"
#include "sl/sludefs.h"

/**
 *
 * Use this function to start Keep Warm processing.
 * 
 * This downloads GPS assistance data over the network.  The status of the 
 * process can be monitored using:
 *     - SWI_NOTIFY_LbsPaWarmBegin
 *     - SWI_NOTIFY_LbsPaWarmStatus
 *     - SWI_NOTIFY_LbsPaWarmDone  
 *             
 * \param      timeout: [IN]   
 *                     Function timeout in milliseconds.
 *
 * \return See SWI_RCODE for additional information.
 *
 */
package SWI_API SWI_RCODE SwiSetLbsPaKeepWarmStart(
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode;  /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;   /* ptr to param field rx'd from modem */
    swi_uint8          *outparamp;  /* ptr to outbound param field */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength;  /* Ret'd length of the CnS Parm field */
    swi_uint8          *endoutparamp;  /* ptr to outbound param field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );
    endoutparamp = outparamp;
    
    /* Pack outgoing parameter field */
    piput16(APOBJVER1, &endoutparamp);

    /* pack outgoing parameters */
    parmlength = endoutparamp - outparamp;

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_LBS_PA_KEEP_WARM_START, 
                                  parmlength,
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * Use this function to get the Keep Warm processing status.
 *             
 * \param      psLbsPaWarmStatus: [OUT]        
 *                     A pointer to a SWI_STRUCT_LbsPaWarmStatus structure 
 *                     which contains warm status information.
 *
 * \param      timeout: [IN]               
 *                     Function timeout in milliseconds.
 *
 * \return See SWI_RCODE for additional information.               
 *
 */
package SWI_API SWI_RCODE SwiGetLbsPaWarmStatus(
    struct SWI_STRUCT_LbsPaWarmStatus *psLbsPaWarmStatus, 
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
                                  CNS_LBS_PA_WARM_STATUS, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

   /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUpkLbsPaWarmStatus( inparamp, psLbsPaWarmStatus);
    }

    /* All done, send the result to the caller */
    return resultcode;

}

/**
 *
 * Use this function to stop the Keep Warm processing.
 * 
 * SWI_NOTIFY_LbsPaWarmDone is generated when the process completes.
 *             
 * \param      timeout: [IN]   
 *                     Function timeout in milliseconds.
 *
 * \return SWI_RCODE_OK indicates that the modem has successfully received
 *                     the stop request.  A SWI_NOTIFY_LbsPaWarmDone 
 *                     notification is sent the the actually process 
 *                     terminates.  
 *
 * \return See SWI_RCODE for additional information.
 *
 * \sa SWI_NOTIFY_LbsPaWarmDone        
 *
 */
package SWI_API SWI_RCODE SwiSetLbsPaKeepWarmStop(
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode;  /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;   /* ptr to param field rx'd from modem */
    swi_uint8          *outparamp;  /* ptr to outbound param field */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength;  /* Ret'd length of the CnS Parm field */
    swi_uint8          *endoutparamp;  /* ptr to outbound param field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );
    endoutparamp = outparamp;
    
    /* Pack outgoing parameter field */
    piput16(APOBJVER1, &endoutparamp);

    /* pack outgoing parameters */
    parmlength = endoutparamp - outparamp;

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_LBS_PA_KEEP_WARM_STOP, 
                                  parmlength,
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );
 
    /* All done, send the result to the caller */
    return resultcode;
}
/**
 *
 * Use this function to retrieve location fix settings stored on the device.
 *             
 * \param      psLbsSettings: [OUT]    
 *                     A pointer to a SWI_STRUCT_LbsFixSettings structure 
 *                     which contains location fix settings.
 *
 * \param      timeout: [IN]       
 *                     Function timeout in milliseconds.
 *
 * \return SWI_RCODE_OK indicates that the modem has received the request.
 *
 * \return     See SWI_STRUCT_LbsFixSettings for actual results of the 
 *                     transaction.
 *
 * \return     See SWI_RCODE for additional information.
 *
 */
SWI_API SWI_RCODE SwiGetLbsFixSettings(
    struct SWI_STRUCT_LbsFixSettings *psLbsFixSettings, 
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
                                  CNS_LBS_SETTINGS, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

   /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUpkLbsFixSettingsGetResp( inparamp, psLbsFixSettings);
    }

    /* All done, send the result to the caller */
    return resultcode;
}
/**
 *
 * Use this function to change location fix settings stored on the device.
 *             
 * \param psLbsSettings: [IN]       
 *             A pointer to a SWI_STRUCT_LbsFixSettings structure which 
 *             contains location fix settings.
 *
 * \param timeout: [IN]         
 *                     Function timeout in milliseconds.
 *
 * \return     SWI_RCODE_OK indicates that the modem has received the request.
 *
 * \return     See SWI_STRUCT_LbsFixSettings for actual results of the 
 *                     transaction.
 *
 * \return     See SWI_RCODE for additional information.
 *
 */
package SWI_API SWI_RCODE SwiSetLbsFixSettings(
    struct SWI_STRUCT_LbsFixSettings *psLbsFixSettings, 
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode;  /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;   /* ptr to param field rx'd from modem */
    swi_uint8          *outparamp;  /* ptr to outbound param field */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength;  /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* pack outgoing parameters */
    parmlength = SwiPkLbsFixSettings( outparamp, APOBJVER1, psLbsFixSettings);

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_LBS_SETTINGS, 
                                  parmlength,
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

   /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUpkLbsFixSettingsSetResp( inparamp, psLbsFixSettings);
    }

    /* All done, send the result to the caller */
    return resultcode;
}
                    
/**
 *
 * Use this function to respond to a network initiated location fix request.
 * 
 * The host is notified of a network initiated location fix request via
 * SWI_NOTIFY_LbsNiReq.  
 *             
 * \param      pLbsNiReq: [IN/OUT]     
 *                     A pointer to a SWI_STRUCT_LbsNiReq structure which 
 *                     contains network initiated request information.
 *
 * \param      timeout: [IN]           
 *                     Function timeout in milliseconds.
 *
 * \return     SWI_RCODE_OK indicates that the modem has received the request.
 *
 * \return     See SWI_STRUCT_LbsNiReq::eErrCode for actual results of the 
 *                     transaction.
 *
 * \return     See SWI_RCODE for additional information.
 * 
 * \sa SWI_STRUCT_LbsNiReq
 * \sa SWI_NOTIFY_LbsNiReq
 *
 */
package SWI_API SWI_RCODE SwiSetLbsNiReq(
    struct SWI_STRUCT_LbsNiReq *pLbsNiReq,
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode;  /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;   /* ptr to param field rx'd from modem */
    swi_uint8          *outparamp;  /* ptr to outbound param field */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength;  /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* pack outgoing parameters */
    parmlength = SwiPkLbsNiRequest( outparamp, APOBJVER1, pLbsNiReq);

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_LBS_NI_REQUEST, 
                                  parmlength,
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUpkLbsNiRequest( inparamp, pLbsNiReq);
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/* 
 * $Log: SwiApiGsmLbs.c,v $
 */
