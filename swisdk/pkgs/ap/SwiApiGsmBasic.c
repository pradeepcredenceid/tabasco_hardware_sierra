/**
 *    \if CvsId 
 *    $Id: SwiApiGsmBasic.c,v 1.21 2010/06/10 00:16:53 epasheva Exp $
 *    \endif
 *    \ingroup gsmbasic
 *    \file SwiApiGsmBasic.c
 * 
 *    \brief  Contains source code for GSM API functions for the
 *            the SDK.  
 *
 *    Copyright: © 2008 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "SwiApiGsmBasic.h"     
#include "ap/apidefs.h"


/**
 * This function returns the modem power setting.    
 *
 * The modem disable setting will persist though the modem's power may be
 * cycled.
 *
 * \param   pModemDisable[OUT]: pointer to location to store the modem disable 
 *                              setting.  true - modem is disabled
 *                                        false - modem is enabled
 *
 * \param   timeout[IN]:        response timeout value in milliseconds
 *
 * \return  SWI_RCODE_OK - the disable modem setting has been received from 
 *                         the modem.
 * \return  SWI_RCODE_REQUEST_TIMEOUT  - the disable modem request has timed out
 * \sa      See SWI_RCODE for other error return codes
 *
 */
SWI_API SWI_RCODE SwiGetModemDisable(swi_bool* pModemDisable, 
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
    parmlength = 0;

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_MODEM_DISABLE, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        /* Copy to the caller's buffer */
        *pModemDisable = (swi_bool) *inparamp;
    }
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function can be used to change the modem disable setting on the 
 * modem.  A request is sent to the modem.  The return code will indicate
 * if the request was received by the modem.  The result of the request
 * is indicated in the SWI_NOTIFY_ModemDisabled notification.  The caller 
 * must specify the amount of time to wait for the modem to respond to the 
 * request.  
 *          
 * The modem disable setting will persist though the modem's power may be
 * cycled.
 *
 * Setting the modem to the on state causes a modem reset.
 *
 * \param   bModemDisable[IN]:  modem disable flag
 *                              1 - disable modem
 *                              0 - enable modem
 *
 * \param   timeout[IN]:        response timeout value in milliseconds
 *
 * \return  SWI_RCODE_OK -  modem disable has been set
 * \return  SWI_RCODE_REQUEST_TIMEOUT  - the modem disable request has timed out
 * \return  See SWI_RCODE for other error return codes
 *
 */
SWI_API SWI_RCODE SwiSetModemDisable(const swi_bool bModemDisable, 
                                     swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *outparamp; /* ptr to outbound param field */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */
    swi_uint8          nModemDisabled;

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* pack outgoing parameters */
    parmlength = 1;
    nModemDisabled = (swi_uint8)bModemDisable;
    piputmulti (&nModemDisabled, &outparamp, 1);

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_MODEM_DISABLE, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        /* Do nothing, we set the value , so need to use Get to read back
         * the value we set and check notification
         */
    }
    /* All done, send the result to the caller */
    return resultcode;
}


/**
 *
 * This function is used to fetch the IMEI from the modem.  The IMEI is
 * consists of: Type Approval Code (TAC), Final Assembly Code (FAC), 
 *              Serial Number (SNR)
 *
 * A request is sent to the modem.  The modem's response contains the IMEI
 * plus check byte.
 *
 * The caller must specify the amount of time to wait for the modem
 * to respond to the request.    
 *          
 * \param   pImei: [OUT] pointer to SWI_STRUCT_Imei storage
 * \param   timeout[IN]: response timeout value in milliseconds
 *
 * \return  SWI_RCODE_OK - the request has been received by the modem
 * \return  SWI_RCODE_REQUEST_TIMEOUT  - the  request has timed out
 * \return  See SWI_RCODE for other error return codes
 *
 */
SWI_API SWI_RCODE SwiGetImei(struct SWI_STRUCT_Imei *pImei, 
                             swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *outparamp; /* ptr to outbound param field */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */

   /* Note: This is an exact copy of the SwiGetImei code except for the
     * last slmemcpy line. Any changes in GetImei must be made here as well
     * unless this API needs to be different otherwise
     */

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
        if( parmlength > LENGTH_Imei - 1 )
            parmlength = LENGTH_Imei - 1;

       /* Clears caller's buffer so that the unused portion will be NULLed */
        slmemset( (char *)pImei, 0, sizeof(struct SWI_STRUCT_Imei) );
        
        /* Unused but set it here anyways */
        pImei->sizeStruct = sizeof(struct SWI_STRUCT_Imei);
        
        /* Copy NULL terminated string containing the IMEI. */
        slmemcpy( pImei->szImei, inparamp, parmlength );  
 
        /* Copy 6 character, NULL terminated string representing 
         * the Type Approval Code. 
         */
        slmemcpy( pImei->szTac, pImei->szImei, LENGTH_Tac-1 );  
 
        /* Copy 2 character, NULL terminated string representing 
         * the Assembly Code. 
         */
        slmemcpy( pImei->szFac, 
                  &pImei->szImei[LENGTH_Tac-1], 
                  LENGTH_Fac-1 );  
    
        /* Copy 6 character, NULL terminated string representing 
         * the Serial Number. 
         */
        slmemcpy( pImei->szSnr, 
                  &pImei->szImei[LENGTH_Tac + LENGTH_Fac - 2], 
                  LENGTH_Snr-1 );  
    
        /*! Check byte */
        slmemcpy( pImei->szCheckByte, 
                  &pImei->szImei[LENGTH_Imei-2], 
                  LENGTH_ChkByte-1 );  
    }
    
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function is used to retrieve the test history byte
 *
 * A request is sent to the modem.  The modem's response contains the
 * test history byte.
 *
 * The caller must specify the amount of time to wait for the modem
 * to respond to the request.    
 *          
 * \param   Thbp: [OUT]    test history byte
 * \param timeout: [IN]    response timeout value in milliseconds
 *
 * \return  SWI_RCODE_OK - the request has been received by the modem
 * \return  SWI_RCODE_REQUEST_TIMEOUT  - the  request has timed out
 * \sa  See SWI_RCODE for other error return codes
 *
 */
SWI_API SWI_RCODE SwiGetTestHistoryByte (struct SWI_STRUCT_Thb *Thbp, swi_uint32 timeout)
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
    parmlength = 0;

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_THB, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        /* Copy to the caller's buffer */
        SwiUpkGsThb(inparamp, Thbp);
   }
    /* All done, send the result to the caller */
    return resultcode;

}

/**
 *
 * This function is used to retrieve the FSN
 *
 * A request is sent to the modem.  The modem's response contains the
 * FSN.
 *
 * The caller must specify the amount of time to wait for the modem
 * to respond to the request.    
 *          
 * \param    pFsn: [OUT]    FSN
 * \param  timeout: [IN]    response timeout value in milliseconds
 *
 * \return  SWI_RCODE_OK - the request has been received by the modem
 * \return  SWI_RCODE_REQUEST_TIMEOUT  - the  request has timed out
 * \sa      See SWI_RCODE for other error return codes
 *
 */
SWI_API SWI_RCODE SwiGetSerialNumber(struct SWI_STRUCT_Fsn *sFsn, 
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
    parmlength = 0;

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_FSN, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        /* Copy to the caller's buffer */
        SwiUpkGsSerialNumber(inparamp, sFsn);
    }
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function is used to retrieve the modem's available features.
 *
 * A request is sent to the modem.  The modem's response contains the
 * modem's available features.
 *
 * The caller must specify the amount of time to wait for the modem
 * to respond to the request.    
 *          
 * \param sFeatures: [OUT]  feature capability.  See SWI_STRUCT_AvailFeatures
 *                          for details.
 * \param  timeout: [IN]    response timeout value in milliseconds
 *
 * \return SWI_RCODE_OK - the request has been received by the modem
 * \return SWI_RCODE_REQUEST_TIMEOUT  - the  request has timed out
 * \sa     See SWI_RCODE for other error return codes
 *
 */
SWI_API SWI_RCODE SwiGetAvailableFeatures(struct SWI_STRUCT_AvailFeatures * sFeatures,
                                          swi_uint32 sizeBuffer, 
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
    parmlength = 0;

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_AVAILABLE_FEATURES, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        /* Copy to the caller's buffer */
        SwiUpkGsAvailableFeatures(inparamp, sFeatures);
    }
    /* All done, send the result to the caller */
    return resultcode;

}

/**
 *
 * This function is used to retrieve the modem's customized features.
 *
 * A request is sent to the modem.  The modem's response contains the
 * modem's customizations.
 *
 * The caller must specify the amount of time to wait for the modem
 * to respond to the request.    
 *          
 * \param   sCustomizations: [OUT] modem's customizations
 * \param    timeout: [IN]         response timeout value in milliseconds
 *
 * \return  SWI_RCODE_OK - the request has been received by the modem
 * \return  SWI_RCODE_REQUEST_TIMEOUT  - the  request has timed out
 * \sa      See SWI_RCODE for other error return codes
 *
 */
SWI_API SWI_RCODE SwiGetFeatureCustomizations(struct SWI_STRUCT_FeatureCustomizations* sCustomizations, 
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
    parmlength = 0;

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_CUSTOMIZATIONS, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        /* Copy to the caller's buffer */
        SwiUpkGsFeatureCustomizations(inparamp, sCustomizations);
    }
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function is used to fetch the current frequency band and a list
 * of all bands supported by the modem.  
 *
 * A request is sent to the modem.  The modem's response contains the
 * band information.  The modem's response indicates the request has been
 * processed by the modem.
 *
 * The caller must specify the amount of time to wait for the modem
 * to respond to the request.    
 *
 * NOTE: This function is deprecated and replaced with SwiGetRadioBand  
 *
 *          
 * \param   sBandInfo: [OUT] the modem's band information 
 * \param   sizeBuffer: [IN] size of buffer provided for the IMEI
 * \param   timeout: [IN]    response timeout value in milliseconds
 *
 * \return  SWI_RCODE_OK - the request has been processed
 * \return  SWI_RCODE_REQUEST_TIMEOUT  - the  request has timed out
 * \sa      See SWI_RCODE for other error return codes
 *
 */
SWI_API SWI_RCODE SwiGetBandInfo(struct SWI_STRUCT_Band *sBandInfo, 
                                 swi_uint32 sizeBuffer, 
                                 swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *outparamp; /* ptr to outbound param field */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */
    struct CNS_BAND_PAYLOAD* pPayload;
    
    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* pack outgoing parameters */
    parmlength = 0;

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_BAND, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        pPayload = (struct CNS_BAND_PAYLOAD*)inparamp;

        /* Copy to the caller's buffer */
        SwiUpkGsBandInfo(pPayload, sBandInfo);
    }
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function turns the all notifications OFF 
 *
 * After calling this function, the user enabled notifications will 
 * not be forwarded to the client's callback function.
 *
 * The following notifications are enabled by default and can not 
 * be disabled:
 *
 * SWI_NOTIFY_AirServerChange        
 *
 * \param timeout: [IN]    response timeout value in milliseconds    
 *
 * \sa See SWI_RCODE for additional information.
 * \sa GSM only 
 */
SWI_API SWI_RCODE SwiStopAllNotif(swi_uint32 timeout)
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
                                  CNS_DIS_ALL_NOT, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength );

    /* All done, send the result to the caller */
    return resultcode;
}

/*
 * $Log: SwiApiGsmBasic.c,v $
 */
