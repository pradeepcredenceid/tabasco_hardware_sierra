/**
 *    \if CvsId 
 *    $Id: SwiApiGsmNetwork.c,v 1.25 2010/06/10 00:18:31 epasheva Exp $
 *    \endif
 *    \ingroup gsmnetwork
 *    \file SwiApiGsmNetwork.c
 * 
 *    \brief  Contains source code for GSM APIs related to network
 *            and data.  
 *
 *    Copyright: © 2008 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "SwiApiGsmNetwork.h"
#include "ap/apidefs.h"


/**
 *
 * This function is used to fetch the current received signal strength
 * indication (RSSI) and bit error rate (BER) from the modem.  
 * 
 * The RSSI values are measured in dBm.  A return value approaching -110
 * indicates that the modem is outside of coverage or the signal is weak.
 *
 * Use of this function should be reserved to application startup. There is a 
 * notification available in the GPRS API (SWI_NOTIFY_Rssi) that also includes 
 * a measure of the Block Error Rate (BER).  Use of notifications is preferred 
 * over direct polling.
 *
 * This function is only supported on GSM devices.
 *            
 * \param    sRssiBer: [OUT]     
 *            RSSI and BER values
 *
 * \param    timeout: [IN]        
 *            Response timeout value in milliseconds.
 *
 * \return    SWI_RCODE_OK:
 *            The request has been processed and values returned are valid.
 *
 * \sa    See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_Rssi
 * \sa SWI_NOTIFY_Rssi
 *
 */
SWI_API SWI_RCODE SwiGetRssiBer(struct SWI_STRUCT_Rssi *sRssiBer, 
                                swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* no outgoing parameters */
    parmlength = 0;
            
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_RSSI, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUpkRssiGsm( inparamp, sRssiBer );
    }
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function maps RSSI values to the number of RSSI bars that should
 * be shown to the user.  It will output 0 to 5 RSSI bars.  
 *
 * This function is only supported on GSM devices.  
 *            
 * \param    iRssi: [IN]    
 *            RSSI value in dBm to be converted to number of RSSI bars.
 *
 * \param    pNumBars: [OUT]    
 *            Number of RSSI bars to be displayed.
 *
 * \return    SWI_RCODE_OK:
 *            The request has been processed and values returned are valid.
 * 
 * \return SWI_RCODE_INVALID_PAR:
 *            Invalid RRSI value was supplied.
 *
 * \sa    See SWI_RCODE for other error return codes.
 * 
 * \sa To get the RSSI value in dBm, call SwiGetRssiBer. 
 *
 */
SWI_API SWI_RCODE SwiGetRssiBars(const swi_int32 iRssi, 
                                 swi_uint32 *pNumBars)
{
    if( iRssi > 0 )
        return SWI_RCODE_INVALID_PAR;

    if( iRssi <= RSSI_LEVEL0 )
        *pNumBars = 0;              /* Display no bars */

    else if( iRssi < RSSI_LEVEL1 )
        *pNumBars = 1;

    else if( iRssi < RSSI_LEVEL2 )
        *pNumBars = 2;

    else if( iRssi < RSSI_LEVEL3 )
        *pNumBars = 3;

    else if( iRssi < RSSI_LEVEL4 )
        *pNumBars = 4;

    else
        *pNumBars = 5;

    return SWI_RCODE_OK;
}

/**
 *
 * This function is called to retrieve network traffic statistics 
 * (the number of bytes transmitted and received).
 *
 * The traffic counters are reset to zero when the connection is dropped. 
 * While disconnected, this will report zeros.  The traffic count is user 
 * payload data only and does not include network housekeeping traffic or
 * SMS messages.
 *            
 * \param    nSent: [OUT]        
 *            Number of bytes sent
 *
 * \param    nReceived: [OUT]    
 *            Number of bytes received
 *
 * \param    timeout: [IN]        
 *            Response timeout value in milliseconds
 *
 * \sa    See SWI_RCODE for additional information.
 *
 */
SWI_API SWI_RCODE SwiGetTrafficStatistics(swi_uint32 *nSent, 
                                          swi_uint32 *nReceived, 
                                          swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* no outgoing parameters */
    parmlength = 0;
            
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_CALL_BYTE_CNT, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUpkByteCounters( inparamp, nSent, nReceived );
    }
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function is used to reset the modem's byte data counters.   
 * 
 * If not explicitly reset, byte counter information is persistent across
 * PDP context activations.
 *
 * This function is only supported on GSM devices.
 *            
 * \param    timeout: [IN]        
 *            Response timeout value in milliseconds.
 *
 * \return    SWI_RCODE_OK:
 *            The request has been processed.
 *
 * \sa    See SWI_RCODE for other error return codes.
 *
 */
SWI_API SWI_RCODE SwiResetTrafficStatistics(swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* no outgoing parameters */
    parmlength = 0;
            
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_BYTE_CNT_RESET, 
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
 * This function returns a summary of the status of all account profiles, 
 * and identifies which one is the default profile for auto connection.
 * 
 * Use this function with 3G and newer devices.  SwiGetProfileSummary should
 * be used with older devices.
 * 
 * The summary reports the status of each profile along with its context 
 * number.  The summary structure reports two profile context identities: 
 * one for the default profile used for automatic activation, and another to 
 * identify the current profile that is actively connected to the network 
 * (activated). If the modem is not currently connected, the current profile 
 * context is reported as zero.
 *            
 * \param    pProfileSummary: [OUT]    
 *            Profile summary information
 *
 * \param    timeout: [IN]        
 *            Response timeout value in milliseconds
 *
 * \return    SWI_RCODE_OK: 
 *            The request has been processed and values returned are valid.
 *
 * \sa    See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_GsmProfileSummary
 * \sa SwiGetProfileSummary
 *
 */
SWI_API SWI_RCODE SwiGetGsmProfileSummary(
    struct SWI_STRUCT_GsmProfileSummary* pProfileSummary, 
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* no outgoing parameters */
    parmlength = 0;

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PROFILE_SUMMARY, 
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
        SwiUpkGsProfileSummary( inparamp, pProfileSummary );
    }
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function retrieves the basic details of a connection profile.
 *
 * This function is used with 3G and newer devices.  For older devices,
 * use SwiGetProfile.
 * 
 * Calling this function on profile that is not present (deleted) returns
 * default profile settings.
 * 
 * SWI_NOTIFY_GsmProfileChange can be used to monitor for changes made to
 * profile information.
 *
 * \param    pProfile: [OUT]        
 *            Basic profile information
 *
 * \param    timeout: [IN]        
 *            Response timeout value in milliseconds
 *
 * \return    SWI_RCODE_OK: 
 *            The request has been processed and values returned are valid.
 *
 * \sa    See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_GsmProfileBasic
 * \sa SWI_NOTIFY_GsmProfileChange
 * \sa SwiGetGsmProfileQos
 * \sa SwiGetGsmProfileDns
 * \sa SwiGetGsmProfileTft
 * \sa SwiSetGsmProfileBasic
 * \sa SwiSetGsmProfileQos
 * \sa SwiSetGsmProfileDns
 * \sa SwiSetGsmProfileTft
 * \sa SWI_STRUCT_Profile
 *
 */
SWI_API SWI_RCODE SwiGetGsmProfileBasic(
    struct SWI_STRUCT_GsmProfileBasic* pProfile, 
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *outparamp; /* ptr to outbound param field */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */
    swi_bool           bUpkRcode = TRUE; /* Unpack result */
    swi_uint8 infoarray[LENGTH_CNS_PROFILE_READ_PARAM];
    
    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* pack outgoing parameters */
    parmlength = LENGTH_CNS_PROFILE_READ_PARAM;

     /* Format the parameters for the set request */
    infoarray[0] = pProfile->nProfileId;
    infoarray[1] = SWI_PROFILEINFO_Basic;
    infoarray[2] = 0; /* this field makes sense only for a Tft profile */
    piputmulti ( &(infoarray[0]), &outparamp, LENGTH_CNS_PROFILE_READ_PARAM );
          
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PROFILE_READ, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        /* Copy to the caller's buffer */
        bUpkRcode = SwiUpkGsProfileBasic( inparamp, pProfile );
        if (bUpkRcode == FALSE)
        {
            resultcode = SWI_RCODE_CNS_FAILED;
        }
        else
        {

            /* Override username/password values using CNS_PROFILE_AUTH (0x700E) */
            /* Initialize the pointer to the outgoing request buffer pointer */
            reqbufferp = cngetreqbkp();

            /* Get a pointer to the start of the outbound CnS Parameter field */
            outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );
            parmlength = LENGTH_CNS_PROFILE_AUTH_GET;
            piput16(APOBJVER1, &outparamp);
            *outparamp = pProfile->nProfileId;
            /* Prepare and send the blocking call */
             resultcode = SwiIntSendnWait( reqbufferp,
                                           CNS_PROFILE_AUTH,
                                           parmlength,
                                           timeout,
                                           CNS_GETREQ,
                                           SWI_AMGSM,
                                           &inparamp,
                                           &parmlength );
            if( resultcode == SWI_RCODE_OK )
            {
                bUpkRcode = SwiUpkGsProfileAuth( inparamp, pProfile );
                if (bUpkRcode == FALSE)
                    resultcode = SWI_RCODE_CNS_FAILED;
            }
            /* If the result code is REQUEST REJECTED, then the 
             * modem must be an older one which doesn't support
             * the CNS_PROFILE_AUTH object. Ignore this call and
             * return as though we never made it
             */
            else if( resultcode == SWI_RCODE_REQUEST_REJECTED )
            {
                resultcode = SWI_RCODE_OK;
            }
            /* else just return the erroneous error code */
        }
    }
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function retrieves the QoS details of a connection profile.
 *
 * This function is used with 3G and newer devices.  For older devices,
 * use SwiGetProfile.
 * 
 * Calling this function on profile that is not present (deleted) returns
 * default profile settings.
 * 
 * SWI_NOTIFY_GsmProfileChange can be used to monitor for changes made to
 * profile information.
 *
 * \param    pProfile: [OUT]        
 *            QoS profile information
 *
 * \param    timeout: [IN]        
 *            Response timeout value in milliseconds
 *
 * \return    SWI_RCODE_OK: 
 *            The request has been processed and values returned are valid.
 *
 * \sa    See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_GsmProfileQos
 * \sa SWI_NOTIFY_GsmProfileChange
 * \sa SwiGetGsmProfileBasic
 * \sa SwiGetGsmProfileDns
 * \sa SwiGetGsmProfileTft
 * \sa SwiSetGsmProfileBasic
 * \sa SwiSetGsmProfileQos
 * \sa SwiSetGsmProfileDns
 * \sa SwiSetGsmProfileTft
 * \sa SWI_STRUCT_Profile
 *
 */
SWI_API SWI_RCODE SwiGetGsmProfileQos(
    struct SWI_STRUCT_GsmProfileQos* pProfile, 
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *outparamp; /* ptr to outbound param field */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */
    swi_bool           bUpkRcode = TRUE; /* Unpack result */
    swi_uint8 infoarray[LENGTH_CNS_PROFILE_READ_PARAM];
    
    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* pack outgoing parameters */
    parmlength = LENGTH_CNS_PROFILE_READ_PARAM;

     /* Format the parameters for the set request */
    infoarray[0] = pProfile->nProfileId;
    infoarray[1] = SWI_PROFILEINFO_Qos;
    infoarray[2] = 0; /* this field makes sense only for a Tft profile */
    piputmulti ( &(infoarray[0]), &outparamp, LENGTH_CNS_PROFILE_READ_PARAM );
          
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PROFILE_READ, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        /* Copy to the caller's buffer */
        bUpkRcode = SwiUpkGsProfileQos( inparamp, pProfile );
        if (bUpkRcode == FALSE)
            resultcode = SWI_RCODE_CNS_FAILED;
    }
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function retrieves the DNS details of a connection profile.
 *
 * This function is used with 3G and newer devices.  For older devices,
 * use SwiGetProfile.
 * 
 * Calling this function on profile that is not present (deleted) returns
 * default profile settings.
 * 
 * SWI_NOTIFY_GsmProfileChange can be used to monitor for changes made to
 * profile information.
 *
 * \param    pProfile: [OUT]        
 *            DNS profile information
 *
 * \param    timeout: [IN]        
 *            Response timeout value in milliseconds
 *
 * \return    SWI_RCODE_OK: 
 *            The request has been processed and values returned are valid.
 *
 * \sa    See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_GsmProfileDns
 * \sa SWI_NOTIFY_GsmProfileChange
 * \sa SwiGetGsmProfileBasic
 * \sa SwiGetGsmProfileQos
 * \sa SwiGetGsmProfileTft
 * \sa SwiSetGsmProfileBasic
 * \sa SwiSetGsmProfileQos
 * \sa SwiSetGsmProfileDns
 * \sa SwiSetGsmProfileTft
 * \sa SWI_STRUCT_Profile
 *
 */
SWI_API SWI_RCODE SwiGetGsmProfileDns(
    struct SWI_STRUCT_GsmProfileDns* pProfile, 
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *outparamp; /* ptr to outbound param field */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */
    swi_bool           bUpkRcode = TRUE; /* Unpack result */
    swi_uint8 infoarray[LENGTH_CNS_PROFILE_READ_PARAM];
    
    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* pack outgoing parameters */
    parmlength = LENGTH_CNS_PROFILE_READ_PARAM;

     /* Format the parameters for the set request */
    infoarray[0] = pProfile->nProfileId;
    infoarray[1] = SWI_PROFILEINFO_Dns;
    infoarray[2] = 0; /* this field makes sense only for a Tft profile */
    piputmulti ( &(infoarray[0]), &outparamp, LENGTH_CNS_PROFILE_READ_PARAM );
          
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PROFILE_READ, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );
    
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        /* Copy to the caller's buffer */
        bUpkRcode = SwiUpkGsProfileDns( inparamp, pProfile );
        if (bUpkRcode == FALSE)
            resultcode = SWI_RCODE_CNS_FAILED;
    }
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function retrieves the TFT details of a connection profile.
 *
 * This function is used with 3G and newer devices.  Older devices do not
 * support TFT configurations.
 * 
 * Calling this function on profile that is not present (deleted) returns
 * default profile settings.
 * 
 * SWI_NOTIFY_GsmProfileChange can be used to monitor for changes made to
 * profile information.
 *
 * \param    pProfile: [OUT]        
 *            TFT profile information
 *
 * \param    timeout: [IN]        
 *            Response timeout value in milliseconds
 *
 * \return    SWI_RCODE_OK: 
 *            The request has been processed and values returned are valid.
 *
 * \sa    See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_GsmProfileTft
 * \sa SWI_NOTIFY_GsmProfileChange
 * \sa SwiGetGsmProfileBasic
 * \sa SwiGetGsmProfileDns
 * \sa SwiGetGsmProfileQos
 * \sa SwiSetGsmProfileBasic
 * \sa SwiSetGsmProfileQos
 * \sa SwiSetGsmProfileDns
 * \sa SwiSetGsmProfileTft
 * \sa SWI_STRUCT_Profile
 *
 */
SWI_API SWI_RCODE SwiGetGsmProfileTft(
    struct SWI_STRUCT_GsmProfileTft* pProfile, 
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *outparamp; /* ptr to outbound param field */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */
    swi_bool           bUpkRcode = TRUE; /* Unpack result */
    swi_uint8 infoarray[LENGTH_CNS_PROFILE_READ_PARAM];
    
    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* pack outgoing parameters */
    parmlength = LENGTH_CNS_PROFILE_READ_PARAM;

     /* Format the parameters for the set request */
    infoarray[0] = pProfile->nProfileId;
    infoarray[1] = SWI_PROFILEINFO_Tft;
    infoarray[2] = (swi_uint8)pProfile->nFilterId; /* this field makes sense only for a Tft profile */
    piputmulti ( &(infoarray[0]), &outparamp, LENGTH_CNS_PROFILE_READ_PARAM );
          
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PROFILE_READ, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        /* Copy to the caller's buffer */
        bUpkRcode = SwiUpkGsProfileTft( inparamp, pProfile );
        if (bUpkRcode == FALSE)
            resultcode = SWI_RCODE_CNS_FAILED;
    }
    /* All done, send the result to the caller */
    return resultcode;

}

/**
 *
 * This function stores the basic user account information in a profile slot, 
 * making it available for connection.
 *            
 * This function is used with 3G and newer devices.  For older devices,
 * use SwiSetProfile.
 *
 * \param    pProfile: [IN]        
 *            Basic profile information to be updated on the modem.  The 
 *            profile ID and size of the structure must be provided by 
 *            the caller.
 *
 * \param    timeout: [IN]        
 *            Response timeout value in milliseconds
 *
 * \return    SWI_RCODE_OK: 
 *            The request has been processed.
 * 
 * \return SWI_RCODE_FAILED:
 *            The modem will verify the validity of the data in the profile 
 *            structure.   If it fails to validate, the function will return 
 *            SWI_RCODE_FAILED.  You must use the validation process provided in 
 *            the sample code to ensure the profile data can be written.  If the
 *            profile is active when you attempt to write a new setting, the 
 *            function will also return SWI_RCODE_FAILED.
 *
 * \sa    See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_GsmProfileBasic
 * \sa SWI_NOTIFY_GsmProfileChange
 *
 */
SWI_API SWI_RCODE SwiSetGsmProfileBasic(
    struct SWI_STRUCT_GsmProfileBasic pProfile, 
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

    /* pack outgoing parameters *
     * parmlength should be = LENGTH_CNS_PROFILE_WRITE_MAX
     * Note: The valid flag in the basic profile info has to be set to enable
     *       the profile. When the valid flag is cleared, all  profile data 
     *       are set to default. 
     */
    parmlength = SwiPkGsProfileBasic( pProfile, outparamp );
    
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PROFILE_WRITE, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        /* Nothing to do, we're SETTING, there is notification 
         * SWI_NOTIFY_GsmProfileChange 
         */
        /* Check results using "at+cgdcont?" command           */
        /* Override username/password values using CNS_PROFILE_AUTH (0x700E) */
        /* Initialize the pointer to the outgoing request buffer pointer */
        reqbufferp = cngetreqbkp();

        /* Get a pointer to the start of the outbound CnS Parameter field */
        outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

        parmlength = SwiPkGsProfileAuth( &pProfile, outparamp );

        /* Prepare and send the blocking call */
         resultcode = SwiIntSendnWait( reqbufferp,
                                       CNS_PROFILE_AUTH,
                                       parmlength,
                                       timeout,
                                       CNS_SETREQ,
                                       SWI_AMGSM,
                                       &inparamp,
                                       &parmlength );
        /* If the result code is REQUEST REJECTED, then the 
         * modem must be an older one which doesn't support
         * the CNS_PROFILE_AUTH object. Ignore this call and
         * return as though we never made it
         */
        if( resultcode == SWI_RCODE_REQUEST_REJECTED )
            resultcode = SWI_RCODE_OK;
        
    }
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 * 
 * This function stores the QoS user account information in a profile slot.
 *            
 * This function is used with 3G and newer devices.  For older devices,
 * use SwiSetProfile.
 *
 * \param    pProfile: [IN]        
 *            QoS profile information to be updated on the modem.  The profile 
 *            ID and size of the structure must be provided by the caller.
 *
 * \param    timeout: [IN]        
 *            Response timeout value in milliseconds
 *
 * \return    SWI_RCODE_OK: 
 *            The request has been processed.
 * 
 * \return SWI_RCODE_FAILED:
 *            The modem will verify the validity of the data in the profile 
 *            structure.   If it fails to validate, the function will return 
 *            SWI_RCODE_FAILED.  You must use the validation process provided 
 *            in the sample code to ensure the profile data can be written.
 *            If the profile is active when you attempt to write a new setting
 *            the function will also return SWI_RCODE_FAILED.
 *
 * \sa    See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_GsmProfileQos
 * \sa SWI_NOTIFY_GsmProfileChange
 *
 */
SWI_API SWI_RCODE SwiSetGsmProfileQos(
    struct SWI_STRUCT_GsmProfileQos pProfile, 
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

    /* pack outgoing parameters *
     * parmlength should be = LENGTH_CNS_PROFILE_WRITE_MAX
     * Note: The valid flag in the basic profile info has to be set to enable
     *       the profile. When the valid flag is cleared, all  profile data 
     *       are set to default. 
     */
    parmlength = SwiPkGsProfileQos( pProfile, outparamp ); 
    
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PROFILE_WRITE, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        /* Nothing to do, we're SETTING, there is notification 
         * SWI_NOTIFY_GsmProfileChange 
         */
        /* Check results using "at+cgdcont?" command           */
    }
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function stores the DNS user account information in a profile slot.
 *            
 * This function is used with 3G and newer devices.  For older devices,
 * use SwiSetProfile.
 *
 * \param    pProfile: [IN]        
 *            DNS profile information to be updated on the modem.  The profile 
 *            ID and size of the structure must be provided by the caller.
 *
 * \param    timeout: [IN]        
 *            Response timeout value in milliseconds
 *
 * \return    SWI_RCODE_OK: 
 *            The request has been processed.
 * 
 * \return SWI_RCODE_FAILED:
 *            The modem will verify the validity of the data in the profile 
 *            structure.   If it fails to validate, the function will return 
 *            SWI_RCODE_FAILED.  You must use the validation process provided in 
 *            the sample code to ensure the profile data can be written.  If the
 *            profile is active when you attempt to write a new setting, the 
 *            function will also return SWI_RCODE_FAILED.
 *
 * \sa        See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_GsmProfileDns
 * \sa SWI_NOTIFY_GsmProfileChange
 *
 */
SWI_API SWI_RCODE SwiSetGsmProfileDns(
    struct SWI_STRUCT_GsmProfileDns pProfile, 
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

    /* pack outgoing parameters *
     * parmlength should be = LENGTH_CNS_PROFILE_WRITE_MAX
     * Note: The valid flag in the basic profile info has to be set to enable
     *       the profile. When the valid flag is cleared, all  profile data 
     *       are set to default. 
     */
    parmlength = SwiPkGsProfileDns( pProfile, outparamp ); 
    
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PROFILE_WRITE, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        /* Nothing to do, we're SETTING, there is notification 
         * SWI_NOTIFY_GsmProfileChange 
         */
        /* Check results using "at+cgdcont?" command           */
    }
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function stores the TFT user account information in a profile slot.
 *            
 * This function is used with 3G and newer devices.  Older devices do not 
 * support TFT configurations.
 *
 * \param    pProfile: [IN]        
 *            TFT profile information to be updated on the modem.  The profile 
 *            ID and size of the structure must be provided by the caller.
 *
 * \param    timeout: [IN]        
 *            Response timeout value in milliseconds
 *
 * \return    SWI_RCODE_OK: 
 *            The request has been processed.
 * 
 * \return SWI_RCODE_FAILED:
 *            The modem will verify the validity of the data in the profile 
 *            structure.   If it fails to validate, the function will return 
 *            SWI_RCODE_FAILED.  You must use the validation process provided in 
 *            the sample code to ensure the profile data can be written.  If the
 *            profile is active when you attempt to write a new setting, the 
 *            function will also return SWI_RCODE_FAILED.
 *
 * \sa    See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_GsmProfileTft
 * \sa SWI_NOTIFY_GsmProfileChange
 *
 */
SWI_API SWI_RCODE SwiSetGsmProfileTft(
    struct SWI_STRUCT_GsmProfileTft pProfile, 
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

    /* pack outgoing parameters *
     * parmlength should be = LENGTH_CNS_PROFILE_WRITE_MAX
     * Note: The valid flag in the basic profile info has to be set to enable
     *       the profile. When the valid flag is cleared, all  profile data 
     *       are set to default. 
     */
    parmlength = SwiPkGsProfileTft( pProfile, outparamp ); 
    
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PROFILE_WRITE, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        /* Nothing to do, we're SETTING, there is notification 
         * SWI_NOTIFY_GsmProfileChange 
         */
        /* Check results using "at+cgdcont?" command           */
    }
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function will erase the account information from a profile and 
 * restore to factory defaults.  The profile will then report a status of 
 * "deleted".  
 *            
 * \param    nProfileID: [IN]        
 *            Profile ID to be erased.  Profile ID ranges from 1-16 for 3G and
 *            newer devices and 1-3 for older (AC7xx and MP7xx) devices. 
 *
 * \param    timeout: [IN]        
 *            Response timeout value in milliseconds
 *
 * \return    SWI_RCODE_OK: 
 *            The request has been processed.
 * 
 * \return SWI_RCODE_FAILED:
 *            If the profile is active when you attempt to write a new setting, 
 *            the function will return SWI_RCODE_FAILED.
 *
 * \return    See SWI_RCODE for other error return codes.
 *
 * \sa SWI_NOTIFY_GsmProfileChange
 *
 */
SWI_API SWI_RCODE SwiEraseProfile(
    const swi_uint32 nProfileID, 
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *outparamp; /* ptr to outbound param field */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */
    swi_uint8          infoarray[LENGTH_CNS_PROFILE_WRITE_MAX];

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* pack outgoing parameters */
    parmlength = LENGTH_CNS_PROFILE_WRITE_MAX;
    
    slmemset((char *)&infoarray[0], 0, LENGTH_CNS_PROFILE_WRITE_MAX);
    /* Format the parameters for the set request */
    infoarray[0] = nProfileID;
    infoarray[1] = SWI_PROFILEINFO_Basic; 
    infoarray[2] = 0; /* valid flag need to be set to 0 */
    piputmulti ( &(infoarray[0]), &outparamp, LENGTH_CNS_PROFILE_WRITE_MAX );
   
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PROFILE_WRITE, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );
                     
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        /* Nothing to do, we're SETTING, there is notification 
         * SWI_NOTIFY_GsmProfileChange 
         */
        /* Check results using "at+cgdcont?" command           */
    }
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function sets which profile is to be used for possible automatic 
 * connection.  If the profile that is set to be the default has the auto 
 * connection flag set to true, then that profile will be used to connect to 
 * the network automatically when the modem is started or reset.
 * 
 * The modem does not verify that the profile contains a valid (non-blank) 
 * account. You can set the default to a "deleted" profile; the autoconnect
 * flag will be false so this is equivalent to disabling autoconnection.
 *            
 * \param    nProfileID: [IN]        
 *           Profile ID to be made the default. Profile ID ranges from 1-16 for
 *           3G and newer devices and 1-3 for older (AC7xx and MP7xx) devices. 
 *
 * \param    timeout: [IN]        
 *           Response timeout value in milliseconds.
 *
 * \return   SWI_RCODE_OK: 
 *           The request has been processed.
 *
 * \sa    See SWI_RCODE for other error return codes.
 * 
 * \sa SwiGetGsmProfileSummary
 * \sa SwiGetProfileSummary
 *
 */
SWI_API SWI_RCODE SwiSetDefaultProfile(
    const swi_uint32 nProfileID, 
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *outparamp; /* ptr to outbound param field */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */
    swi_uint8          profileId;  /* profile id */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* pack outgoing parameters */
    parmlength = sizeof(swi_uint8);
    profileId = (swi_uint8)nProfileID;
    piputmulti (&(profileId), &outparamp, parmlength);
            
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PROFILE_SUMMARY, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );
     
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        /* Nothing to do, we're SETTING, there is notification 
         * SWI_NOTIFY_GsmProfileChange 
         */
        /* Check results using "at+cgdcont?" command           */
    }
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function initiates or destroys a packet session.
 *            
 * \param    nProfileID: [IN]        
 *            Profile ID to acted on.  Profile ID ranges from 1-16 for
 *            3G and newer devices and 1-3 for older (AC7xx and MP7xx) devices. 
 *
 * \param    Activate: [IN]        
 *            True if the profile should be activated.  False if the profile 
 *            should be deactivated.
 *
 * \param    timeout: [IN]        
 *            Response timeout value in milliseconds.
 *
 * \return    SWI_RCODE_OK: 
 *            The request has been processed.  A SWI_NOTIFY_PktSessionStatus 
 *            notification will indicate the result of the request.
 * 
 * \return SWI_RCODE_REQUEST_REJECTED:
 *            Activation requests MAY be rejected if FDN is enabled.  This 
 *            behaviour is customizable by the carrier.  SwiGetLastError would
 *            return "FDN Mismatch".
 *
 * \sa    See SWI_RCODE for other error return codes.
 *
 * \sa SWI_NOTIFY_PktSessionStatus
 *
 */
SWI_API SWI_RCODE SwiActivateProfile(
    const swi_uint32 nProfileID, swi_bool Activate, 
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint32         modemprofile; /* Returned profile ID from modem */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *outparamp; /* active ptr to outbound param field */
    swi_uint8          *startparamp; /* ptr to start of outbound param field */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */
    swi_uint8          status;     /* Returned status of request from modem */
    swi_uint8          errorstrp[32]; /* Returned error string from modem */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    startparamp = amgetparamp( AMCNSREQUEST, reqbufferp );
    outparamp = startparamp;

    /* pack outgoing parameters */
    *outparamp++ = (swi_uint8) nProfileID;  /* First byte of set request */
    *outparamp++ = (swi_uint8) Activate;    /* Second byte */
            
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PKT_SESSION_CALL, 
                                  (swi_uint16) (outparamp-startparamp),      
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );
     
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        /* Parse out the response fields. Most of this will be 
         * discarded since there is no way to inform the caller
         * with the API defined as it is
         */
        modemprofile = *inparamp++;/* Ignored for now */
        Activate = *inparamp++;    /* Ignored for now */

        status = *inparamp++;      /* Used to generate the return code */

        /* If modem rejects the action */
        if( status )
        {
            /* Copy the returned error string, if there was one, ignored */
            slmemcpy( errorstrp, inparamp, slstrlen( (const char *)inparamp )+1 );
        
            /* Alter the return code to reflect the failure */
            resultcode = SWI_RCODE_FAILED;
        }
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function controls the radio power state.
 * 
 * This function is similar to SwiSetModemDisable.  When called to "shutdown"
 * the radio, the modem will detach from the network and power down the radio.
 * Unlike SwiSetModemDisable, however, this powered down state is NOT 
 * persistent across modem power cycles.
 * 
 * When called to "startup" the radio, the modem will power on the radio and
 * attempt to attach to a network (if available).
 * 
 * It is recommended that this function be called with the "shutdown" 
 * parameter when the host application shuts down or when the PC enters a 
 * suspend state.
 * 
 * It is recommended that this function be called with the "startup" parameter
 * when the host application initially starts up and after resuming from
 * a PC suspend state.
 *
 * A change in the radio's power state will be signaled with a
 * SWI_NOTIFY_ModemDisabled indication.
 *
 * This function is only supported on GSM devices.
 *            
 * \param    bStartup: [IN]        
 *            Set to true to power up the radio and attempt to attach to a 
 *            network (if available).  Set to false to detach from the network
 *            and power down the radio.
 *
 * \param    timeout: [IN]        
 *            Response timeout value in milliseconds.
 *
 * \return SWI_RCODE_OK:
 *            The request was received by the modem.  The result of the request
 *            is indicated in a SWI_NOTIFY_ModemDisabled notification.
 *
 * \return See SWI_RCODE for other error return codes.
 * 
 * \sa SWI_NOTIFY_ModemDisabled
 *
 */
SWI_API SWI_RCODE SwiSetHostStartup(swi_bool bStartup, 
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

    /* Negate the incoming parameter to match the CnS Object Definition */
    if( bStartup )
        bStartup = 0; /* Host starting up */
    else
        bStartup = 1; /* Host shutting down */

    /* pack outgoing parameters */
    parmlength = sizeof(swi_uint8); 
    piputmulti ((const swi_uint8 *)&(bStartup), &outparamp, parmlength);
    
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_HOST_SW_STATE, 
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
 * This function is used to retrieve the RSCP and Ec/Io measurements.
 *            
 * Use of this function should be limited.  SWI_NOTIFY_RscpEcIo 
 * notification is generated when there is a change in the information.  
 * Use of notifications is preferred over direct polling.
 *
 * This function is only supported on GSM devices.
 *
 * \param    pRscpEcIo: [OUT]    
 *            RSCP and Ec/Io measurements.
 *
 * \param    timeout: [IN]        
 *            Response timeout value in milliseconds
 *
 * \return    SWI_RCODE_OK:
 *            The request has been processed and values returned are valid.
 *
 * \return    See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_RscpEcIo
 * \sa SWI_NOTIFY_RscpEcIo
 *
 */
SWI_API SWI_RCODE SwiGetRscpEcIo(struct SWI_STRUCT_RscpEcIo* pRscpEcIo, 
                                 swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* no outgoing parameters */
    parmlength = 0;
            
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_RSCP_ECIO, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUpkGsRscpEcIo( inparamp, pRscpEcIo );
    }
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function retrieves the IP address of the specified profile.  
 *
 * This function is only supported on GSM devices. 
 *
 * \param    pIp: [OUT]    
 *            IP address information. ProfileId of the IP address structure 
 *            must be specified before calling this function.
 *
 * \param    timeout: [IN]        
 *            Response timeout value in milliseconds.
 *
 * \return    SWI_RCODE_OK:
 *            The request has been processed and values returned are valid.
 *
 * \sa    See SWI_RCODE for other error return codes. 
 * 
 * \sa SWI_STRUCT_IpAddress
 * \sa SwiGetIPAddress
 *
 */
SWI_API SWI_RCODE SwiGetIPAddressEx(struct SWI_STRUCT_IpAddress* pIp, 
                                    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *outparamp; /* ptr to outbound param field */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */
    swi_uint8          profileId;  /* profile id */
    swi_uint8          *endoutparamp;  /* local copy of outparamp */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );
    endoutparamp = outparamp;
    
    /* Format the parameters for the set request */
    profileId               = pIp->nProfileId;
    piputmulti (&(profileId), &endoutparamp, 1);

    /* outgoing parameter size in bytes*/
    parmlength = endoutparamp - outparamp;
           
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_IP_ADDRESS_EXP, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        /* Copy to the caller's buffer */
        SwiUpkGsIPAddress( inparamp, pIp );
    }
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function is obsolete and is replaced with SwiGetGsmSystemStatus.  
 * 
 * This function is used to fetch the network status of the modem.
 * 
 * SWI_NOTIFY_NetworkStatus indication is generated when there is a state
 * change.  This is the preferred method of monitoring status instead of 
 * polling with this function. 
 *
 * This function is only supported on GSM devices.
 *
 * \param    sNetworkStatus: [OUT]        
 *            Network status
 * 
 * \param    timeout: [IN]        
 *            Response timeout value in milliseconds.
 *
 * \return    SWI_RCODE_OK:
 *            The request has been processed and values returned are valid.
 *
 * \sa    See SWI_RCODE for other error return codes. 
 *
 * \sa SwiGetGsmSystemStatus
 * \sa SWI_STRUCT_GSM_NetworkStatus
 * \sa SWI_NOTIFY_NetworkStatus
 *
 */
SWI_API SWI_RCODE SwiGetNetworkStatus(
    struct SWI_STRUCT_GSM_NetworkStatus *sNetworkStatus, 
    swi_uint32 timeout)
{
    /* OBSOLETE */
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* no outgoing parameters */
    parmlength = 0;
           
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_NETWORK_STATE, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );
    
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUpkGsNetworkStatus( inparamp, sNetworkStatus );
    }

    /* All done, send the result to the caller */
    return resultcode;

}

/**
 *
 * This function is used to retrieve the GSM system status.
 * 
 * This function replaces SwiGetNetworkStatus.
 * 
 * Use of this function should be limited.  SWI_NOTIFY_GsmSystemStatus 
 * notification is generated when there is a change in the system status
 * information.  Use of notifications is preferred over direct polling.
 *
 * This function is only supported on GSM devices.
 *            
 * \param    pSysStatus: [OUT]        
 *            System status
 *
 * \param    timeout: [IN]    
 *            Response timeout value in milliseconds.
 *
 * \return    SWI_RCODE_OK:
 *            The request has been processed and values returned are valid.
 *
 * \sa    See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_GsmSystemStatus
 * \sa SWI_NOTIFY_GsmSystemStatus
 * \sa SwiGetNetworkStatus
 *
 */
SWI_API SWI_RCODE SwiGetGsmSystemStatus(
    struct SWI_STRUCT_GsmSystemStatus* pSysStatus, 
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* no outgoing parameters */
    parmlength = 0;
       
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_SYSTEM_STATUS, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUpkGsSystemStatus( inparamp, pSysStatus );
    }
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function is used to fetch the packet session status of the modem.
 *
 * This function has been deprecated and SwiGetPktSessionStatusEx() should
 * be used instead.
 *
 * \sa SwiGetPktSessionStatusEx()
 * 
 * \param    sSessionStatus: [OUT]        
 *            Packet session status
 * 
 * \param    timeout: [IN]        
 *            Response timeout value in milliseconds
 *
 * \return    SWI_RCODE_OK:
 *            The request has been processed and values returned are valid.
 *
 * \sa    See SWI_RCODE for other error return codes. 
 *
 * \sa SWI_STRUCT_PktSessionStatus
 * \sa SWI_NOTIFY_PktSessionStatus
 *
 */
SWI_API SWI_RCODE SwiGetPktSessionStatus(
    struct SWI_STRUCT_PktSessionStatus *sSessionStatus, 
    swi_uint32 timeout)
{
    /* This function has been deprecated. Callers should use
     * SwiGetPktSessionStatusEx() instead
     */
    return SWI_RCODE_NOT_SUP;
}

/**
 *
 * This API is used to fetch the packet session status of the modem.
 *
 * This API replaces the old API SwiGetPktSessionStatus()  
 *
 * This API is only supported on GSM devices.
 * 
 * \param    sSessionStatus: [IN/OUT]        
 *            Packet session status containing profileID
 * 
 * \param    timeout: [IN]        
 *            Response timeout value in milliseconds
 *
 * \return    SWI_RCODE_OK:
 *            The request has been processed and values returned are valid.
 *
 * \sa    See SWI_RCODE for other error return codes. 
 *
 * \sa SWI_STRUCT_PktSessionCall
 * 
 *
 */
SWI_API SWI_RCODE SwiGetPktSessionStatusEx(
    struct SWI_STRUCT_PktSessionCall *sPktSessionCall, 
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *outparamp; /* ptr to outbound param field */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */
    swi_uint8          profileId;  /* 8-bit field profile id */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* pack outgoing parameters */
    parmlength = 1;
    /* Note: For now we assume profileId 1. This is because this API is obsolete
     *       One should use  SwiGetPktSessionStatusEx instead 
     */
    /* pack input data */
    profileId = (swi_uint8)sPktSessionCall->nProfileId;
    piputmulti (&(profileId), &outparamp, 1);
           
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PKT_SESSION_STATUS_EXP, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );
     
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        /* Copy to the caller's buffer */
        SwiUpkGsPktSessionStatusExp( inparamp, sPktSessionCall );
    }
    /* All done, send the result to the caller */
    return resultcode; 
}

/**
 *
 * This function is used to fetch the PLMN selection mode of the modem.  
 * 
 * The modem PLMN selection mode will be either Automatic or Manual.
 *
 * This function is only supported on GSM devices.
 *
 * \param    eMode: [OUT]        
 *            PLMN selection mode
 * 
 * \param    timeout: [OUT]        
 *            Response timeout value in milliseconds
 *
 * \return    SWI_RCODE_OK:
 *            The request has been processed and values returned are valid.
 *
 * \sa    See SWI_RCODE for other error return codes. 
 *
 * \sa SWI_TYPE_PLMN_Mode
 * \sa SwiSetPLMNMode
 *
 */
SWI_API SWI_RCODE SwiGetPLMNMode(enum SWI_TYPE_PLMN_Mode* eMode, 
                                 swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* no outgoing parameters */
    parmlength = 0;
           
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PLMN_MODE, 
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
        *eMode = *inparamp;
    }
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function is used to set the PLMN selection mode of the modem. 
 *    
 * The mode can be set to automatic or manual.  The PLMN must be specified
 * for the manual mode.  Use SwiStartPLMNSearch to search for a list of
 * available PLMNs before switching to manual mode. 
 *
 * This function is only supported on GSM devices.
 *
 * \param    sPlmn: [IN]            
 *            PLMN selection mode information
 * 
 * \param    timeout: [IN]        
 *            Response timeout value in milliseconds
 *
 * \return    SWI_RCODE_OK:
 *            The request has been processed and values returned are valid.
 *
 * \sa    See SWI_RCODE for other error return codes. 
 *
 * \sa SWI_STRUCT_PlmnSelection
 * \sa SwiGetPLMNMode
 * \sa SwiStartPLMNSearch
 *
 */
SWI_API SWI_RCODE SwiSetPLMNMode(const struct SWI_STRUCT_PlmnSelection sPlmn, 
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
    parmlength = LENGTH_CNS_PLMN_MODE_SET_PARAM; 
    *outparamp++ = sPlmn.eMode;
    piput16(sPlmn.nMCC, &outparamp);
    piput16(sPlmn.nMNC, &outparamp);
    
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PLMN_MODE, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );
    
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        /* Nothing to do, we're SETTING, there is notification 
         * SWI_NOTIFY_PlmnMode 
         */
    }
    /* All done, send the result to the caller */
    return resultcode;

}

/**
 *
 * This function is used to initiate a PLMN scan.  In general this is done
 * before switching to manual PLMN selection mode.  
 * 
 * This function should not be called until the SWI_NOTIFY_PlmnReady
 * notification has been received.  
 *
 * This function is only supported on GSM devices.
 *
 * \param    timeout: [IN]        
 *            Response timeout value in milliseconds
 *
 * \return    SWI_RCODE_OK:
 *            The request has been processed. The result of the request will 
 *            be indicated in a SWI_NOTIFY_PlmnAvailable notification.  
 *            SwiGetPLMNSelection() should be used to retrieve the list of PLMN 
 *            found in the area as a result of the scan.
 *
 * \sa See SWI_RCODE for other error return codes. 
 *
 * \sa SWI_NOTIFY_PlmnAvailable
 * \sa SWI_NOTIFY_PlmnReady
 * \sa SwiGetPLMNSelection
 * \sa SwiSetPLMNMode
 * \sa SwiGetPLMNMode
 *
 */
SWI_API SWI_RCODE SwiStartPLMNSearch(swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* no outgoing parameters */
    parmlength = 0; 
    
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_MANUAL_SELECT_EXP, 
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
 * The PLMN scan initiated by SwiStartPLMNSearch compiles a list of PLMNs in 
 * the area.  This function is used to retrieve a single PLMN from that list.
 *
 * This function is only supported on GSM devices.
 *            
 * \param    sPlmn: [OUT]    
 *            PLMN information
 *
 * \param    timeout: [IN]    
 *            Response timeout value in milliseconds
 *
 * \return    SWI_RCODE_OK:
 *            The request has been processed and values returned are valid.  The
 *            modem's response contains a single PLMNs information and will 
 *            indicate if there are more PLMN to be retrieved from the list on
 *            the modem.  See SWI_STRUCT_PlmnSelection for additional info.  If
 *            there are additional PLMNs in the list, call SwiGetPLMNSelection 
 *            repeatedly until all PLMNs are read.
 *
 * \sa    See SWI_RCODE for other error return codes. 
 *
 * \sa SWI_NOTIFY_PlmnAvailable
 * \sa SwiStartPLMNSearch
 * \sa SWI_STRUCT_PlmnSelection
 *
 */
SWI_API SWI_RCODE SwiGetPLMNSelection(struct SWI_STRUCT_PlmnSelection* sPlmn, 
                                      swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* no outgoing parameters */
    parmlength = 0;
           
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_MANUAL_SELECT_EXP, 
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
        SwiUpkGsPLMNSelection( inparamp, sPlmn );
    }
    /* All done, send the result to the caller */
    return resultcode;

}

/**
 *
 * This function requests the current service status from the modem.
 * 
 * Use of this function should be limited. There is a notification available 
 * in the GPRS API (SWI_NOTIFY_NetworkStatus or SWI_NOTIFY_GsmSystemStatus) 
 * that reports this data along with other useful service parameters.  Use of
 * notifications is preferred over direct polling.
 *
 * This function is only supported on GSM devices.
 *            
 * \param    eServiceStatus: [OUT]    
 *            Service status
 *
 * \param    timeout: [IN]    
 *            Response timeout value in milliseconds.
 *
 * \return    SWI_RCODE_OK:
 *            The request has been processed and values returned are valid.
 *
 * \sa    See SWI_RCODE for other error return codes.
 *
 * \sa SWI_TYPE_ServiceStatus
 * \sa SWI_STRUCT_GsmSystemStatus
 * \sa SWI_NOTIFY_NetworkStatus
 *
 */
SWI_API SWI_RCODE SwiGetServiceStatusEx(enum SWI_TYPE_ServiceStatus *eServiceStatus, 
                                        swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* no outgoing parameters */
    parmlength = 0;
            
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_NETWORK_STATE, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );
 
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        /* Unpack the incoming service status information */
        *eServiceStatus = piget16(&inparamp);
    }
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function is used to retrieve GSM cell information.
 *            
 * Use of this function should be limited.  SWI_NOTIFY_GsmCellInfo 
 * notification is generated when there is a change in the cell information.  
 * Use of notifications is preferred over direct polling.
 *
 * This function is only supported on GSM devices.
 *
 * \param    pCellInfo: [OUT]        
 *            GSM cell info
 *
 * \param    timeout: [IN]        
 *            Response timeout value in milliseconds
 *
 * \return    SWI_RCODE_OK:
 *            The request has been processed and values returned are valid.
 *
 * \return    See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_GsmCellInfo
 * \sa SWI_NOTIFY_GsmCellInfo
 *
 */
SWI_API SWI_RCODE SwiGetGsmCellInfo(struct SWI_STRUCT_GsmCellInfo *pCellInfo, 
                                    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */
    swi_bool           version = TRUE; /* flag indicationg whether the object 
                                        * version is supported
                                        */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* no outgoing parameters */
    parmlength = 0;
       
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_GSM_CELL_INFO, 
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
        SwiUpkGsCellInfo(inparamp, pCellInfo, &version);
        if (!version)
        {
            resultcode = SWI_RCODE_FAILED;
        }
    }
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function allows the host to get the RAT mode of the device.
 * This function is only supported on GSM devices.
 *
 * \param   psRat: [OUT]
 *                 RAT mode information.
 *
 * \param   timeout: [IN]
 *                 Response timeout value in milliseconds
 *
 * \return     SWI_RCODE_OK:
 *                 The request has been processed and values returned are valid.
 *
 * \return See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_Rat
 * \sa SwiSetRat
 *
 */
SWI_API SWI_RCODE SwiGetRat(
    struct SWI_STRUCT_Rat *psRat,
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *outparamp; /* ptr to outbound param field */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */
    swi_bool           version = TRUE; /* flag indicationg whether the object
                                        * version is supported
                                        */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* outgoing parameter */
    piput16(APOBJVER1, &outparamp);
    parmlength = OBJ_LENGTH_GetRat;

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp,
                                  CNS_RAT,
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
        SwiUpkGsGetRat(inparamp, psRat, &version);
        if (!version)
        {
            resultcode = SWI_RCODE_FAILED;
            psRat->bSuccess = FALSE;
        }
        psRat->bSuccess = TRUE;

    }
    /* All done, send the result to the caller */
    return resultcode;
}
/**
 *
 *  This function allows the host to set the RAT mode of the device.
 *  This function is only supported on GSM devices.
 *
 *  \param psRat  [IN/OUT]
 *                RAT information. The structure size and RAT mode type must be
 *                supplied by the caller.  The action result is returned.
 *
 *  \param  timeout [IN]:
 *               Response timeout value in milliseconds
 *
 *  \return     SWI_RCODE_OK:
 *              The request has been processed and values returned are valid.
 *
 *  \return See SWI_RCODE for other error return codes.
 *
 *  \sa SWI_STRUCT_Rat
 *  \sa SwiGetRat
 */
SWI_API SWI_RCODE SwiSetRat(
    struct SWI_STRUCT_Rat *psRat,
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *outparamp; /* ptr to outbound param field */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */
    swi_bool           version = TRUE; /* flag indicationg whether the object
                                        * version is supported
                                        */
    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* outgoing parameter */
    piput16(APOBJVER1, &outparamp);
    *outparamp = psRat->eRatMode;
    parmlength = OBJ_LENGTH_SetRat;

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp,
                                  CNS_RAT,
                                  parmlength,
                                  timeout,
                                  CNS_SETREQ,
                                  SWI_AMGSM,
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        /* Copy to the caller's buffer */
        SwiUpkGsSetRat(inparamp, psRat, &version);
        if (!version || !(~psRat->bSuccess) )
        {
            resultcode = SWI_RCODE_FAILED;
        }
    }
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function allows the host to get the PRL Region of the device.
 * The PRL Region setting determines the scan order and grouping of
 * the GSM and WCDMA bands.
 *
 * \param       sPrl:[OUT]
 *                 PRL region information. The structure size must be populated
 *                 by the caller.
 *
 * \param   timeout:[IN]
 *                 Response timeout value in milliseconds
 *
 * \return     SWI_RCODE_OK:
 *                 The request has been processed and values returned are valid.
 *                 when SWI_RCODE_OK is returned, the caller still has to check
 *                 the ePrlChangeStatu *s for the actual results of the request.
 *
 * \return See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_PrlRegion
 * \sa SwiSetPrlRegion
 *
 */

SWI_API SWI_RCODE SwiGetPrlRegion(
    struct SWI_STRUCT_PrlRegion *psPrl,
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *outparamp; /* ptr to outbound param field */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */
    swi_bool           version = TRUE; /* flag indicationg whether the object
                                        * version is supported
                                        */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* outgoing parameter */
    piput16(APOBJVER1, &outparamp);
    parmlength = OBJ_LENGTH_GetPrlRegion;

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp,
                                  CNS_PRL_REGION,
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
        SwiUpkGsGetPrlRegion(inparamp, psPrl, &version);
        if (!version)
        {
            resultcode = SWI_RCODE_FAILED;
        }
    }
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function allows the host to set the PRL Region of the device.
 * The PRL Region setting determines the scan order and grouping of
 * the GSM and WCDMA bands.
 *
 * \param    sPrl:[IN/OUT]
 *            PRL region information. The structure size, PRL region and the
 *            password to unlock API must be populated by the caller. The action
 *            result is returned.
 *
 * \param   timeout:[IN]
 *            Response timeout value in milliseconds
 *
 * \return  SWI_RCODE_OK:
 *            The request has been processed and values returned are valid.
 *            when SWI_RCODE_OK is returned, the caller still has to check
 *            the ePrlChangeStatus for the actual result of the request.
 *
 *  \return See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_PrlRegion
 * \sa SwiGetPrlRegion
 * \sa SwiApiUnlock
 *
 */
SWI_API SWI_RCODE SwiSetPrlRegion(
    struct SWI_STRUCT_PrlRegion *psPrl,
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *outparamp; /* ptr to outbound param field */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */
    swi_bool           version = TRUE; /* flag indicationg whether the object
                                        * version is supported
                                        */
    SWI_STRUCT_ApiUnlock sUnlock;

    /* We need to unlock the modem first */
    slmemset ((char *)&sUnlock, 0, sizeof (SWI_STRUCT_ApiUnlock));
    slmemcpy (&sUnlock.szPassword[0], psPrl->szUnlockKey, LENGTH_UnlockKey);
    sUnlock.nLevel = API_UNLOCK_OEM;
    sUnlock.nPasswordLen = LENGTH_UnlockKey-1;

    resultcode     = SwiApiUnlock( &sUnlock, timeout);

    if( resultcode == SWI_RCODE_OK )
    {
        if(sUnlock.nResult == API_UNLOCK_PWD_INVALID)
        {
            psPrl->ePrlChangeStatus = SWI_PRLCHANGE_InvalidPassword;
            return resultcode;
        }
        else if (sUnlock.nResult != API_UNLOCK_SUCCESS)
        {
            psPrl->ePrlChangeStatus = SWI_PRLCHANGE_GeneralFailure;
            return resultcode;
        }
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* outgoing parameter */
    piput16(APOBJVER1, &outparamp);
    *outparamp = (swi_uint8)psPrl->ePrlRegion;
    parmlength = OBJ_LENGTH_SetPrlRegion;

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp,
                                  CNS_PRL_REGION,
                                  parmlength,
                                  timeout,
                                  CNS_SETREQ,
                                  SWI_AMGSM,
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        /* Copy to the caller's buffer */
        SwiUpkGsSetPrlRegion(inparamp, psPrl, &version);
        if ( !version && !(~psPrl->ePrlChangeStatus) )
        {
            resultcode = SWI_RCODE_FAILED;
        }
    }
    /* All done, send the result to the caller */
    return resultcode;
}
/*
 * $Log: SwiApiGsmNetwork.c,v $
 */
