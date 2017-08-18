/**
 *    \if CvsId
 *    $Id: SwiApiCdmaLbs.c,v 1.5 2011/02/11 22:09:02 ygao Exp $
 *    \endif
 *    \ingroup cdmalbs
 *    \file SwiApiCdmaLbs.c
 * 
 *    \brief SWI CDMA specific LBS related APIs.
 *
 *    Copyright (c) 2011 Sierra Wireless, Inc.  All rights reserved
 *
 */

#include "SwiApiCmLbs.h"
#include "SwiApiCdmaLbs.h"
#include "SwiIntPkCdmaLbs.h"
#include "SwiIntUpkCdmaLbs.h"
#include "ap/apidefs.h"

/**
 *
 * This function reports the LBS notification status mask.
 *
 * Use this function to check the current notification state of the following 
 * group of LBS notifications:
 *
 *     SWI_NOTIFY_LbsPdDone
 *     SWI_NOTIFY_LbsPdData
 *     SWI_NOTIFY_LbsPdEnd
 *     SWI_NOTIFY_LbsPdBegin
 *     SWI_NOTIFY_LbsPdUpdateFailure
 *
 * \param psNotifyStatus[OUT] Current notification state.
 * \param timeout[IN]         Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetLbsLocNotifyStatus(
        SWI_STRUCT_LbsLocNotifyStatus *psNotifyStatus, 
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
                                  CNS_LBS_LOC_NOTIFY_STATUS, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &paramlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        piget16(&inparamp);    /* Discard obj_ver */
        psNotifyStatus->eNotifyFlag = piget16(&inparamp);
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function sets the LBS notification status mask.
 *
 * Use this function to set the current notification state of the following 
 * group of LBS notifications:
 *
 *     SWI_NOTIFY_LbsPdDone
 *     SWI_NOTIFY_LbsPdData
 *     SWI_NOTIFY_LbsPdEnd
 *     SWI_NOTIFY_LbsPdBegin
 *     SWI_NOTIFY_LbsPdUpdateFailure
 *
 * \param psNotifyStatus[IN] New notification state.
 * \param timeout[IN]        Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetLbsLocNotifyStatus(
        const SWI_STRUCT_LbsLocNotifyStatus *psNotifyStatus, 
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
    parmlength = SwiPackLbsLocNotifyStatus( outparamp, psNotifyStatus );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_LBS_LOC_NOTIFY_STATUS, 
                                  parmlength, 
                                  timeout, 
                                  ((SWI_LBSNOTIFYSTATE_On == 
                                      psNotifyStatus->eNotifyFlag) ?  
                                      CNS_NOTREQ : CNS_STOPREQ),
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */
                     
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function reports LBS download notification status mask.
 *
 * Use this function to check the current notification state of the following 
 * group of LBS notifications:
 *
 *     SWI_NOTIFY_LbsPdDloadBegin
 *     SWI_NOTIFY_LbsPdDloadData
 *     SWI_NOTIFY_LbsPdDloadDone
 *     SWI_NOTIFY_LbsPdDloadEnd
 *
 * \param psNotifyStatus[OUT] Current notification state.
 * \param timeout[IN]         Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetLbsDloadNotifyStatus(
        SWI_STRUCT_LbsDloadNotifyStatus *psNotifyStatus, 
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
                                  CNS_LBS_DLOAD_NOTIFY_STATUS, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &paramlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        piget16(&inparamp);    /* Discard obj_ver */
        psNotifyStatus->eNotifyFlag = piget16(&inparamp);
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function sets LBS download notification status mask.
 *
 * Use this function to check the current notification state of the following 
 * group of LBS notifications:
 *
 *     SWI_NOTIFY_LbsPdDloadBegin
 *     SWI_NOTIFY_LbsPdDloadData
 *     SWI_NOTIFY_LbsPdDloadDone
 *     SWI_NOTIFY_LbsPdDloadEnd
 *
 * \param psNotifyStatus[IN] New notification state.
 * \param timeout[IN]        Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetLbsDloadNotifyStatus(
        const SWI_STRUCT_LbsDloadNotifyStatus *psNotifyStatus, 
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
    parmlength = SwiPackLbsDloadNotifyStatus( outparamp, psNotifyStatus );
    
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_LBS_DLOAD_NOTIFY_STATUS, 
                                  parmlength, 
                                  timeout, 
                                  ((SWI_LBSNOTIFYSTATE_On == 
                                      psNotifyStatus->eNotifyFlag) ?  
                                      CNS_NOTREQ : CNS_STOPREQ),
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */
                     
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function reports LBS parameter notification status mask.
 *
 * Use this function to check the current notification state of the following 
 * group of CnS notifications:
 *
 *     SWI_NOTIFY_LbsPaIpAddr
 *     SWI_NOTIFY_LbsPaGpsLock
 *     SWI_NOTIFY_LbsPaPtlmMode
 *     SWI_NOTIFY_LbsPaPortId
 *     SWI_NOTIFY_LbsPaPrivacy
 *     SWI_NOTIFY_LbsPaNetAccess
 *
 * \param psNotifyStatus[OUT] Current notification state.
 * \param timeout[IN]         Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetLbsParamNotifyStatus(
        SWI_STRUCT_LbsParamNotifyStatus *psNotifyStatus, 
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
                                  CNS_LBS_PARAM_NOTIFY_STATUS, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &paramlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        piget16(&inparamp);    /* Discard obj_ver */
        psNotifyStatus->eNotifyFlag = piget16(&inparamp);
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function sets LBS parameter notification status mask.
 *
 * Use this function to check the current notification state of the following 
 * group of CnS notifications:
 *
 *     SWI_NOTIFY_LbsPaIpAddr
 *     SWI_NOTIFY_LbsPaGpsLock
 *     SWI_NOTIFY_LbsPaPtlmMode
 *     SWI_NOTIFY_LbsPaPortId
 *     SWI_NOTIFY_LbsPaPrivacy
 *     SWI_NOTIFY_LbsPaNetAccess
 *
 * \param psNotifyStatus[IN] New notification state.
 * \param timeout[IN]        Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetLbsParamNotifyStatus(
        const SWI_STRUCT_LbsParamNotifyStatus *psNotifyStatus, 
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
    parmlength = SwiPackLbsParamNotifyStatus( outparamp, psNotifyStatus);
    
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_LBS_PARAM_NOTIFY_STATUS, 
                                  parmlength, 
                                  timeout, 
                                  ((SWI_LBSNOTIFYSTATE_On == 
                                      psNotifyStatus->eNotifyFlag) ?  
                                      CNS_NOTREQ : CNS_STOPREQ),
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */
                     
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function requests a data download of ephemeris and almanac data.
 *
 * The host can use the download option specified in SWI_STRUCT_LbsPdDownload 
 * to indicate whether the data download should occur just once or 
 * periodically, in an effort to keep the data "warm".  If the application 
 * that will be requesting this data will be requiring the use of the data 
 * over a period of time, the application should indicate that the download 
 * should be periodic.
 *
 * \param psLbsPdDownload[IN] Download configuration.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetLbsPdDownload(
        const SWI_STRUCT_LbsPdDownload *psLbsPdDownload, 
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
    parmlength = SwiPackLbsPdDownload( outparamp, psLbsPdDownload );
    
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_LBS_PD_DOWNLOAD, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCDMA, 
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
 * This function gets GPS Lock settings from the modem.
 *
 * \param psPaGpsLock[OUT] GPS lock settings.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetLbsPaGpsLock(SWI_STRUCT_LbsPaGpsLock *psPaGpsLock, 
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
                                  CNS_LBS_PA_GPS_LOCK, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &paramlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        piget16(&inparamp);    /* Discard obj_ver */
        psPaGpsLock->eGpsLock = piget16(&inparamp);
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function sets GPS Lock Setting.
 *
 * \param psPaGpsLock[IN]  GPS lock settings.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetLbsPaGpsLock(
        const SWI_STRUCT_LbsPaGpsLock *psPaGpsLock, 
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
    parmlength = SwiPackLbsPaGpsLock( outparamp, psPaGpsLock );
    
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_LBS_PA_GPS_LOCK, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCDMA, 
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
 * This function gets the mechanism used to transport LBS messages.
 *
 * \param psPaPtlmMode[OUT] Transport mechanism.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetLbsPaPtlmMode(SWI_STRUCT_LbsPaPtlmMode *psPaPtlmMode, 
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
                                  CNS_LBS_PA_PTLM_MODE, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &paramlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        piget16(&inparamp);    /* Discard obj_ver */
        psPaPtlmMode->eTransportMech = piget16(&inparamp);
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function sets the mechanism used to transport LBS messages.
 *
 * \param psPaPtlmMode[IN] Transport mechanism.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetLbsPaPtlmMode(
        const SWI_STRUCT_LbsPaPtlmMode *psPaPtlmMode, 
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
    parmlength = SwiPackLbsPaPtlmMode( outparamp, psPaPtlmMode );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_LBS_PA_PTLM_MODE, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCDMA, 
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
 * This function gets GPS privacy setting used for LBS sessions.
 *
 * \param psPaPrivacy[OUT] Privacy settings.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetLbsPaPrivacy(SWI_STRUCT_LbsPaPrivacy *psPaPrivacy, 
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
                                  CNS_LBS_PA_PRIVACY, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &paramlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        piget16(&inparamp);    /* Discard obj_ver */
        psPaPrivacy->ePrivacy = piget16(&inparamp);
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function sets GPS privacy settings used for LBS sessions.
 *
 * \param psPaPrivacy[IN]  Privacy settings.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetLbsPaPrivacy(
        const SWI_STRUCT_LbsPaPrivacy *psPaPrivacy, 
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
    parmlength = SwiPackLbsPaPrivacy( outparamp, psPaPrivacy );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_LBS_PA_PRIVACY, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCDMA, 
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
 * This function gets GPS network access permissions used during LBS sessions.
 *
 * \param psPaNetAccess[OUT] Network access permissions.
 * \param timeout[IN]        Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetLbsPaNetAccess(
        SWI_STRUCT_LbsPaNetAccess *psPaNetAccess, 
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
                                  CNS_LBS_PA_NET_ACCESS, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &paramlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        piget16(&inparamp);    /* Discard obj_ver */
        psPaNetAccess->eNetAccess = piget16(&inparamp);
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function sets GPS network access permissions used during LBS sessions.
 *
 * \param psPaNetAccess[IN] Network access permissions.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetLbsPaNetAccess(
        const SWI_STRUCT_LbsPaNetAccess *psPaNetAccess, 
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
    parmlength = SwiPackLbsPaNetAccess( outparamp, psPaNetAccess );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_LBS_PA_NET_ACCESS, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCDMA, 
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
 * This function gets current GPS base station information.
 *
 * \param psBsInfo[OUT]    Base station information.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetLbsPaBsInfo(SWI_STRUCT_LbsPaBsInfo *psBsInfo, 
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
                                  CNS_LBS_PA_BS_INFO, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &paramlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackLbsPaBsInfo( inparamp, psBsInfo );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function gets Qualcomm smart mode control.
 *
 * \param pnMask[OUT]  Pointer save Smart Mode control value. One 8-Bit Data 
 *                     Field with two bits assigned to two different smart 
 *                     modes and ORd Together. Smart MS-Based Mode [Valid 
 *                     choice is Enable (0x01) or Disable (0x00)]. Smart 
 *                     Standalone Mode [Valid choice is Enable (0x02) or 
 *                     Disable (0x00)].
 * \param timeout[IN]  Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetLbsSmartModeControl(swi_uint8 *pnMask, 
                                            swi_uint32 timeout)
{
    return SwiIntGetNvU8(CNS_PST_NV, NV_GPS1_DYNAMIC_MODE_I, pnMask, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets Qualcomm smart mode control.
 *
 * \param nMask[IN]    Smart Mode control value to set. One 8-Bit Data 
 *                     Field with two bits assigned to two different smart 
 *                     modes and ORd Together. Smart MS-Based Mode [Valid 
 *                     choice is Enable (0x01) or Disable (0x00)]. Smart 
 *                     Standalone Mode [Valid choice is Enable (0x02) or 
 *                     Disable (0x00)].
 * \param timeout[IN]  Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetLbsSmartModeControl(swi_uint8 nMask, 
                                            swi_uint32 timeout)
{
    return SwiIntSetNvU8(CNS_PST_NV, NV_GPS1_DYNAMIC_MODE_I, nMask, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function get Sierra Wireless smart mode control.
 *
 * \param pnMask[OUT]  pointer save Smart Mode control value. One 8-Bit Field 
 *                     to allow Enable/Disable of Sierra MS-Based Fallback.
 * \param timeout[IN]  Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetLbsSWSmartModeControl(swi_uint8 *pnMask, 
                                              swi_uint32 timeout)
{
    return SwiIntGetNvU8(CNS_PST_NV, NV_SWI_GPS_MSB_FALLBACK_I, pnMask, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets Sierra Wireless smart mode control.
 *
 * \param nMask[IN]    Smart Mode control value to set. One 8-Bit Field to 
 *                     allow Enable/Disable of Sierra MS-Based Fallback.
 * \param timeout[IN]  Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetLbsSWSmartModeControl(swi_uint8 nMask, 
                                              swi_uint32 timeout)
{
    return SwiIntSetNvU8(CNS_PST_NV, NV_SWI_GPS_MSB_FALLBACK_I, nMask, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function get LBS fix type control.
 *
 * \param pFixTypeControl[OUT] Pointer save fix type control value. 
 *                             See SWI_STRUCT_LBS_FixTypeControl.
 * \param timeout[IN]          Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetLbsFixTypeControl(
        SWI_STRUCT_LBS_FixTypeControl *pFixTypeControl, 
        swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         paramlength; /* Returned length of the CnS parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Parameter Field Construction Zone - get a pointer to the 
     * location to fill in the CnS parameter field 
     */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Pack the CnS Parameter field with the input parameters */
    paramlength = SwiPackNv( outparamp, NV_SWI_GPS_MODE_SUPPORT_I );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PST_NV, 
                                  paramlength,
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &paramlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackLbsFixTypeControl( inparamp, pFixTypeControl );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function sets GPS fix type control.
 *
 * \param pFixTypeControl[IN] Pointer point to fix type control value to set.
 *                            See SWI_STRUCT_LBS_FixTypeControl.
 * \param timeout[IN]         Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetLbsFixTypeControl(
        const SWI_STRUCT_LBS_FixTypeControl *pFixTypeControl, 
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
    parmlength = SwiPackLbsFixTypeControl( outparamp, pFixTypeControl );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PST_NV, 
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
 * This function get LBS Coarse Location Message (CLM) Control.
 *
 * \param pnMask[OUT]  Pointer save CLM control value. One 8-Bit Data Field 
 *                     to Enable/Disable the CLM Request Message Support.
 * \param timeout[IN]  Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetLbsCLMControl(swi_uint8 *pnMask, swi_uint32 timeout)
{
    return SwiIntGetNvU8(CNS_PST_NV, NV_GPS1_CAPABILITIES_I, pnMask, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets GPS Coarse Location Message (CLM) Control.
 *
 * \param nMask[IN]    CLM control value to set. One 8-Bit Data Field to 
 *                     Enable/Disable the CLM Request Message Support.
 * \param timeout[IN]  Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetLbsCLMControl(swi_uint8 nMask, swi_uint32 timeout)
{
    return SwiIntSetNvU8(CNS_PST_NV, NV_GPS1_CAPABILITIES_I, nMask, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets NMEA Output Message Control
 *
 * \param pnMask[OUT]  Pointer save NMEA Output Message control value. 
 *                     One 8-Bit Data Field with 5 bits defined and OR'd 
 *                     together to control the different NMEA message types 
 *                     being output.
 *       GPGGA Message Type [Valid choice is Enable (0x01) or Disable (0x00)]
 *       GPGSA Message Type [Valid choice is Enable (0x02) or Disable (0x00)]
 *       GPGSV Message Type [Valid choice is Enable (0x04) or Disable (0x00)]
 *       GPVTG Message Type [Valid choice is Enable (0x08) or Disable (0x00)]
 *       GPRMC Message Type [Valid choice is Enable (0x10) or Disable (0x00)]
 *
 * \param timeout[IN]  Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetLbsNemaOutputMessageControl(swi_uint8 *pnMask, 
                                                    swi_uint32 timeout)
{
    return SwiIntGetNvU8(CNS_PST_NV, NV_GPS1_NMEA_OUTPUT_I, pnMask, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets NMEA Output Message Control.
 *
 * \param nMask[IN]    NMEA Output Message control value to set.
 *                     One 8-Bit Data Field with 5 bits defined and OR'd 
 *                     together to control the different NMEA message types 
 *                     being output.
 *       GPGGA Message Type [Valid choice is Enable (0x01) or Disable (0x00)]
 *       GPGSA Message Type [Valid choice is Enable (0x02) or Disable (0x00)]
 *       GPGSV Message Type [Valid choice is Enable (0x04) or Disable (0x00)]
 *       GPVTG Message Type [Valid choice is Enable (0x08) or Disable (0x00)]
 *       GPRMC Message Type [Valid choice is Enable (0x10) or Disable (0x00)]
 *
 * \param timeout[IN]  Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetLbsNemaOutputMessageControl(swi_uint8 nMask, 
                                                    swi_uint32 timeout)
{
    return SwiIntSetNvU8(CNS_PST_NV, NV_GPS1_NMEA_OUTPUT_I, nMask, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets GPS AutoStart settings
 *
 * \param psLbsAutoStart[OUT] AutoStart Settings. See SWI_STRUCT_LbsAutoStart.
 * \param timeout[IN]         Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetLbsAutoStart(SWI_STRUCT_LbsAutoStart *psLbsAutoStart, 
                                     swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         paramlength; /* Returned length of the CnS parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Parameter Field Construction Zone - get a pointer to the 
     * location to fill in the CnS parameter field 
     */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Pack the CnS Parameter field with the input parameters */
    paramlength = SwiPackNv( outparamp, NV_SWI_GPS_AUTO_TRACK_I );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PST_NV, 
                                  paramlength,
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &paramlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        if(NV_SWI_GPS_AUTO_TRACK_I == piget16(&inparamp))
        {
            SwiUnpackLbsAutoStart( inparamp, psLbsAutoStart );
        }
        else
        {
            resultcode = SWI_RCODE_UNEXPECTED_RESP;
        }
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function sets GPS AutoStart Settings
 *
 * \param psLbsAutoStart[IN] AutoStart Settings. See SWI_STRUCT_LbsAutoStart.
 * \param timeout[IN]        Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetLbsAutoStart(
        const SWI_STRUCT_LbsAutoStart *psLbsAutoStart, 
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
    parmlength = SwiPackLbsAutoStart( outparamp, NV_SWI_GPS_AUTO_TRACK_I, 
            psLbsAutoStart );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PST_NV, 
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
 * This function gets LBS fix type support status.
 *
 * \param pStatus[OUT]     LBS fix type support status.
 * \param nLen[IN]         Length of output buffer.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetLbsFixTypeSupportStatus(swi_uint8 *pStatus, 
                                                swi_uint32 nLen, 
                                                swi_uint32 timeout)
{
    return SwiIntGetNvU8s(CNS_PST_NV, NV_SWI_GPS_MODE_SUPPORT_I, pStatus, 
            nLen, SWI_AMCDMA, timeout);
}

/* GPS One Xtra (begin) */

/**
 *
 * This function gets current almanac data status.
 *
 * \param data[OUT]     Contains the returned almanac data.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetLbsXtraAssistData(
                                SWI_STRUCT_LBS_XTRA_ASSIST_DATA_GET *data,
                                swi_uint32 timeout )
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

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp,
            CNS_LBS_XTRA_ASSIST_DATA,
            0,
            timeout,
            CNS_GETREQ,
            SWI_AMCDMA,
            &inparamp,
            &parmlength ); /* Re-used for OUT arg */

    if(SWI_RCODE_OK == resultcode)
    {
        /* Throw away the object version */
        piget16( &inparamp );

        data->assistanceDataStatus = *inparamp++;

        /* Use cast from "swi_char" to "swi_uint8" to avoid gcc warnings */
        pigetmulti( &inparamp, (swi_uint8 *)&data->assistanceDataTimeStamp, 16 );
        pigetmulti( &inparamp, (swi_uint8 *)&data->assistanceDataTimeStampExpiry, 16 );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function sets GPS One Xtra almanac data.
 *
 * \param data[IN]      Point to GPS One Xtra almanac data to be set.
 * \param response[OUT] Contain results for setting GPS One Xtra data.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetLbsXtraAssistData(
                                SWI_STRUCT_LBS_XTRA_ASSIST_DATA_SET *data,
                                LBS_XTRA_ASSIST_DATA_RESULT *response,
                                swi_uint32 timeout )
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

    swi_uint8  *localfieldp = outparamp;  /* Local copy of paramfieldp */

    piput16( 1, &localfieldp );        /* objVer */
    *localfieldp++ = data->operation;

    /* Use cast from "swi_char" to "swi_uint8" to avoid gcc warnings */
    piputmulti( (swi_uint8 *)&data->assistanceDataFilename, &localfieldp, 8 );

    /* Report how many bytes we've just written */
    parmlength = (localfieldp - outparamp);

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp,
            CNS_LBS_XTRA_ASSIST_DATA,
            parmlength,
            timeout,
            CNS_SETREQ,
            SWI_AMCDMA,
            &inparamp,
            &parmlength ); /* Re-used for OUT arg */

    if( SWI_RCODE_OK == resultcode )
    {
        piget16( &inparamp );
        *response = *inparamp;
    }
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function sets GPS One Xtra time injection data.
 *
 * \param data[IN]      Point to GPS One Xtra time injection date to be set.
 * \param response[OUT] Contain results for setting the time information.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetLbsXtraTimeInfo(
                                SWI_STRUCT_LBS_XTRA_TIME_INFO_SET *data,
                                LBS_XTRA_ASSIST_DATA_RESULT *response,
                                swi_uint32 timeout )
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

    swi_uint8  *localfieldp = outparamp;  /* Local copy of paramfieldp */

    piput16( 1, &localfieldp );        /* objVer */
    *localfieldp++ = data->operation;
    piput64( data->time, &localfieldp );
    piput32( data->timeUncertainty, &localfieldp );
    *localfieldp++ = data->timeReference;
    piput16( data->timeForce, &localfieldp );

    /* Report how many bytes we've just written */
    parmlength = (localfieldp - outparamp);

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp,
            CNS_LBS_XTRA_TIME_INFO,
            parmlength,
            timeout,
            CNS_SETREQ,
            SWI_AMCDMA,
            &inparamp,
            &parmlength ); /* Re-used for OUT arg */

    /* Initialize the response to some failure */
    *response = LBS_XTRA_ASSIST_DATA_RESULT_NOT_SUPPORTED;
    if( SWI_RCODE_OK == resultcode )
    {
        piget16( &inparamp );
        *response = *inparamp;
    }
    /* All done, send the result to the caller */
    return resultcode;
}

static SWI_RCODE SwiGetLbsXtraServerURL(
                                swi_uint16 /* CNS_TYPE */ theCNSType,
                                SWI_STRUCT_LBS_XTRA_SERVER_URL *data,
                                swi_uint32 timeout )
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

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp,
            theCNSType,
            0,
            timeout,
            CNS_GETREQ,
            SWI_AMCDMA,
            &inparamp,
            &parmlength ); /* Re-used for OUT arg */

    if(SWI_RCODE_OK == resultcode)
    {
        /* throw away the object version */
        piget16( &inparamp );

        data->numberOfRetries = *inparamp++;

        /* Use cast from "swi_char" to "swi_uint8" to avoid gcc warnings */
        pigetmulti( &inparamp, (swi_uint8 *)&data->serverURL, 128 );

        data->moreURLs = *inparamp++;
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function gets GPS One Xtra server URL to acquire almanac injection data
 *
 * \param data[OUT]     Contain GPS One Xtra data server URL returned.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetLbsXtraDataServerURL(
                                SWI_STRUCT_LBS_XTRA_SERVER_URL *data,
                                swi_uint32 timeout )
{
    SWI_RCODE swiErr = SWI_RCODE_NOT_SUP;

    swiErr = SwiGetLbsXtraServerURL( CNS_LBS_XTRA_DATA_URL, data, timeout );

    return swiErr;
}

/**
 *
 * This function gets GPS One Xtra server URL to acquire time injection data.
 *
 * \param data[OUT]     Contain GPS One Xtra time server URL returned.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetLbsXtraTimeServerURL(
                                SWI_STRUCT_LBS_XTRA_SERVER_URL *data,
                                swi_uint32 timeout )
{
    SWI_RCODE swiErr = SWI_RCODE_NOT_SUP;

    swiErr = SwiGetLbsXtraServerURL( CNS_LBS_XTRA_TIME_URL, data, timeout );

    return swiErr;
}

/* GPS One Xtra (end) */
/* 
 * $Log: SwiApiCdmaLbs.c,v $
 */

