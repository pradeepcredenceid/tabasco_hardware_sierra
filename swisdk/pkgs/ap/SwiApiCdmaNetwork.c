/**
 *    \if CvsId 
 *    $Id: SwiApiCdmaNetwork.c,v 1.3 2010/06/10 00:10:23 epasheva Exp $
 *    \endif
 *    \ingroup cdmanetwork
 *    \file SwiApiCdmaNetwork.c
 * 
 *    \brief SWI CDMA specific network related APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#include "SwiApiCdmaNetwork.h"
#include "SwiIntPkCdmaNetwork.h"
#include "SwiIntUpkCdmaNetwork.h"
#include "ap/apidefs.h"

/**
 *
 * This function reports the Received Signal Strength Indicator (RSSI)
 * A request is sent to the modem.  The modem's response contains the
 * RSSI. The caller must specify the amount of time to wait for the 
 * modem to respond to the request.    
 *            
 * CDMA does not use an RSSI per se. The modem uses an algorithm 
 * incorporating signal quality elements to produce the numbers 
 * delivered by this function.
 *
 * The interpretation is described in the following table:
 *
 *            -------------------------------------------------
 *            |        Value        |            Meaning      |
 *            -------------------------------------------------
 *            |    -137             |    No signal detected   |
 *            |    -136 to -91      |    Very poor            |
 *            |    -90 to -86       |    Poor                 |
 *            |    -85 to -81       |    Fair                 |
 *            |    -80 to -76       |    Good                 |
 *            |    > -76            |    Excellent            |
 *            -------------------------------------------------
 *
 * Use of this function should be reserved to application startup. 
 * There is a notification available in the CDMA2000 1X API 
 * (SWI_NOTIFY_Rssi). Use of notifications is preferred over 
 * direct polling.
 *
 * \param [OUT] iRssi:   (positive) RSSI value.  Negate the value
 *                       received to interpret it correctly.
 * \param [IN] timeout:  response timeout value in milliseconds
 *
 * \return SWI_RCODE_OK - the request has been received by the modem
 * \return SWI_RCODE_REQUEST_TIMEOUT  - the  request has timed out
 * \return SWI_RCODE_UNEXPECTED_RESP - the value returned was outside of the 
 *             expected range. The returned data is not likely correct.
 * \return See SWI_RCODE for other error return codes
 *
 *
 */
SWI_API SWI_RCODE SwiGetRssi(swi_int32 *iRssi, swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
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
                                  CNS_RSSI, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &paramlength );
                     
                     
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackRssiCdma( inparamp, iRssi );
    }
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function returns the current channel number.
 *
 * \param nChannelNumber[OUT] channel number.
 * \param timeout[IN]         Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetChannelNumber(swi_uint32 *nChannelNumber, 
                                      swi_uint32 timeout)
{
    SWI_RCODE resultcode; /* Result of call to cnSendWait() */
    swi_uint16 nValue;
    
    resultcode = SwiIntGetU16(CNS_CHANNEL_NUMBER, &nValue, SWI_AMCDMA, 
            timeout);
    if(SWI_RCODE_OK == resultcode)
    {
        *nChannelNumber = nValue;
    }
    
    return resultcode;
}

/**
 *
 * This function reports whether or not there is service in the current 
 * location.
 *
 * The presence of a CDMA service does not guarantee that connection is 
 * possible. The modem may fail to authenticate with the local service 
 * provider. E911 service should still be available, even if authentication 
 * fails.
 *
 * There is a notification equivalent (SWI_NOTIFY_ServiceIndication) to 
 * advise the application when entering or leaving coverage. This is 
 * preferred over the polling command.
 *
 * \param lServiceIndicator[OUT] The current service:
 *                               0 = No service
 *                               1 = Analog (not supported)
 *                               2 = Digital service
 * \param timeout[IN]            Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetServiceIndication(swi_int32* lServiceIndicator, 
                                          swi_uint32 timeout)
{
    SWI_RCODE resultcode; /* Result of call to cnSendWait() */
    swi_uint16 nValue;
    
    resultcode = SwiIntGetU16(CNS_SERVICE_INDICATION, &nValue, SWI_AMCDMA, 
            timeout);
    if(SWI_RCODE_OK == resultcode)
    {
        *lServiceIndicator = (swi_int32)nValue;
    }
    
    return resultcode;
}

/**
 *
 * This function retrieves the protocol revision (service type) of the base 
 * station currently serving the modem.
 *
 * This command is used to determine if making 1xRTT calls is possible.
 *
 * Protocol revision 1 is not supported.
 *
 * \param P_PRV[OUT]       The destination in which the base station protocol 
 *                         revision is returned:
 *                           1 = JSTD-008 (PCS Japan)
 *                           2 = IS-95
 *                           3 = IS-95A (Cellular)
 *                           4 = IS-95B minimum req.
 *                           5 = IS-95B full requirements
 *                           6 = CDMA2000 Revision 0
 *                           7 = CDMA2000 Revision 1
 *                         Values of 6 or above indicate 1xRTT coverage. 
 *                         Values below 6 indicate IS-95 coverage.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetCurrentBaseStation(swi_uint16 *P_PRV, 
                                           swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_PREV, P_PRV, SWI_AMCDMA, timeout);
}

/**
 *
 * This function reports whether the device has acquired service from a 
 * cellular service provider other than that to which the user has subscribed.
 *
 * Results when called while not in coverage are undefined. The modem cannot 
 * verify that you are roaming until it can detect coverage 
 * (SwiGetServiceIndication) and identify the service provider(s).
 *
 * Use of this function should be limited. There is a notification available 
 * in the CDMA2000 1X API (SWI_NOTIFY_RoamingStatus) that reports the same 
 * data. Use of notifications is preferred over direct polling.
 *
 * \param lRoamingStatus[OUT] A pointer to return the status:
 *                              0 = Not roaming
 *                              1 = Roaming
 *                              2 = Non preferred roaming
 *                              >2 = Service provider dependent
 * \param timeout[IN]         Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetRoamingStatus(swi_uint32* lRoamingStatus, 
                                      swi_uint32 timeout)
{
    SWI_RCODE resultcode; /* Result of call to cnSendWait() */
    swi_uint16 nValue;
    
    resultcode = SwiIntGetU16(CNS_ROAMING_STATUS, &nValue, SWI_AMCDMA, 
            timeout);
    if(SWI_RCODE_OK == resultcode)
    {
        *lRoamingStatus = nValue;
    }
    
    return resultcode;
}

/**
 *
 * This function reports the current powersave status.
 *
 * The modem enters powersave mode when it fails to find service for a 
 * predetermined amount of time. During this state, the modem will 
 * periodically wake up and scan for service. To force the modem to 
 * immediately exit powersave mode and wake up and scan for service, send an 
 * arbitrary DTMF to the modem using the command SwiSetKeyPressed.
 *
 * Use the notifications SWI_NOTIFY_PowerSaveEnter and leaves powersave mode.
 *
 * SWI_NOTIFY_PowerSaveExit to determine when the modem enters and
 * \param powersaveMode[OUT] The destination in which the powersave state is 
 *                           returned:
 *                             0 = powersave mode off
 *                             1 = powersave mode on
 *                           See SWI_TYPE_TechPowersaveMode.
 * \param timeout[IN]        Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetTechPowersaveMode(swi_uint16 *powersaveMode, 
                                          swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_TECH_POWERSAVE_STATE, powersaveMode, SWI_AMCDMA, 
            timeout);
}

/**
 *
 * This function reports the frame error rate value.
 *
 * The frame error rate value is cumulative for the session in which the modem 
 * is powered. Any power cycles to the modem (reset, eject/insert, etc) will 
 * cause the value to reset.
 *
 * \param totalFrames[OUT]  The destination in which the total # of frames is 
 *                          returned.
 * \param badFrames[OUT]    The destination in which the # of bad frames is 
 *                          returned.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetTechFrame(swi_uint32* totalFrames, 
                                  swi_uint32* badFrames, 
                                  swi_uint32 timeout)
{
    swi_int32 array[2];
    SWI_RCODE rc;

    rc = SwiIntGetU32s(CNS_TECH_FER, (swi_uint32 *)array, 2, SWI_AMCDMA,
                       timeout);
    if(SWI_RCODE_OK == rc)
    {
        *totalFrames = array[0];
        *badFrames = array[1];
    }

    return rc;
}

/**
 *
 * This function reports the transmit power value
 *
 * The transmit power value is measured in negative dBm. To interpret the 
 * value correctly, divide the value by 100.
 *
 * \param TxPower[OUT]     The destination in which the transmit power value 
 *                         is returned.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetTransmitPower(swi_int16* TxPower, swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_TECH_TX_PWR, (swi_uint16*)TxPower, SWI_AMCDMA, 
            timeout);
}

/**
 *
 * This function returns the modem's call privacy state.
 *
 * \param privacyMode[OUT] The destination in which the call privacy state is 
 *                         returned:
 *                           0 = Call Privacy off
 *                           1 = Call Privacy on
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetCallPrivacy(swi_uint16* privacyMode, 
                                    swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_CALL_PRIVACY, privacyMode, SWI_AMCDMA, timeout);
}

/**
 *
 * This function returns the number of bytes trans-mitted and received in the 
 * current call
 *
 * Counters contain the total bytes sent and received over the local 
 * (host/modem) interface. The counter resets when the call is disconnected.
 *
 * A notification equivalent is available (SWI_NOTIFY_CallByteCounter) and is 
 * the preferred mechanism for tracking this data.
 *
 * \param pSent[OUT]       The number of bytes tranmitted.
 * \param pReceived[OUT]   The number of bytes received.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetByteCounter(swi_uint32* pSent, 
                                    swi_uint32* pReceived, 
                                    swi_uint32 timeout)
{
    swi_int32 array[2];
    SWI_RCODE rc;

    rc = SwiIntGetU32s(CNS_CALL_BYTE_COUNTER, (swi_uint32 *)array, 2,
                       SWI_AMCDMA, timeout);
    if(SWI_RCODE_OK == rc)
    {
        *pSent = array[0];
        *pReceived = array[1];
    }

    return rc;
}

/**
 *
 * This function reports the geographical coordindates of the base station 
 * the modem is currently using
 *
 * The geographical coordinates may not be necessilary provided by all base 
 * stations.
 *
 * Below is an example of how to convert the longitude geographical seconds 
 * into degrees/minutes/seconds. Latitude is computed identically.
 \verbatim

        int iMin, iSec, iDeg, iGeoSecs;

        iGeoSecs = iLongitude / 4;
        iDeg = iGeoSecs / 3600;
        iMin = (iGeoSecs - iDeg * 3600) / 60;
        iSec = iGeoSecs - iDeg * 3600 - iMin * 60;

        GeoDeg = iDeg;
        GeoMin = abs(iMin);
        GeoSec = abs(iSec);

\endverbatim
 * \param longitude[OUT]   Longitude reported in geographical seconds.
 * \param latitude[OUT]    Latitude reported in geographical seconds.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetTechBSInfo(swi_int32 *longitude, 
                                   swi_int32 *latitude, 
                                   swi_uint32 timeout)
{
    swi_int32 array[2];
    SWI_RCODE rc;

    rc = SwiIntGetU32s(CNS_TECH_BS_INFO, (swi_uint32 *)array, 2, SWI_AMCDMA,
                       timeout);
    if(SWI_RCODE_OK == rc)
    {
        *longitude = array[0];
        *latitude = array[1];
    }

    return rc;
}

/**
 *
 * This function returns the MDN (Mobile Directory Number) of the specified 
 * NAM account profile.
 *
 * The MDN has a maximum length of 15 bytes.
 *
 * See also SwiGetPhoneNumber.
 *
 * \param Nam[IN]          NAM index.
 * \param MDN[OUT]         Mobile Directory Number of the selected NAM profile.
 * \param length[IN]       Length of the string buffer to receive the result.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetMobileDirNum(swi_uint16 Nam, 
                                     swi_charp MDN, 
                                     swi_uint32 length, 
                                     swi_uint32 timeout)
{
    return SwiIntGetNamU16LenString(CNS_MOBILE_DIRECTORY_NUM, Nam, MDN, 
            length, 0, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function returns the table of SID and NID values designated as "home" 
 * (not roaming).
 *
 * These values are usually preset at the factory for the particular service 
 * provider. They can be changed using SwiPstSetHomeSidNid.
 *
 * \param Nam[IN]          NAM index.
 * \param HomeSidNid[OUT]  The destination structure in which the data is 
 *                         returned.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetHomeSidNid(swi_uint16 Nam, 
                                      SWI_STRUCT_HomeSidNid *HomeSidNid, 
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
    paramlength = SwiPackNvNam( outparamp, NV_HOME_SID_NID, Nam );

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
        SwiPstUnpackHomeSidNid( inparamp, HomeSidNid );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function returns the setting of the CDMA "preferred service" for a 
 * specified NAM account profile.
 *
 * The preferred service is set using SwiPstSetSystemPref.
 *
 * \param Nam[IN]          NAM index.
 * \param PrefServ[OUT]    The system preference (determined by the service 
 *                         provider) from the selected NAM account profile:
 *                           0 = System A only
 *                           1 = System B only
 *                           2 = Home only
 *                           3 = Home preferred
 *                           4 = Automatic A
 *                           5 = Automatic B
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetSystemPref(swi_uint16 Nam, 
                                      swi_uint16 *PrefServ, 
                                      swi_uint32 timeout)
{
    return SwiIntGetNvNamU16(CNS_PST_NV, NV_SYSTEM_PREF, Nam, PrefServ, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function returns the Simple IP profile 1 (index 0) user ID string.
 *
 * The user ID is also referred to as the NAI (refer to IS-638B section 3.5.8).
 *
 * The User ID is set using SwiPstSetSipUserId.
 *
 * \param pszUserId[OUT]   The destination in which the user ID string is 
 *                         returned.
 * \param szLength[IN]     The maximum size of the output buffer.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 * \sa SwiPstGetSipPassword.
 */
SWI_API SWI_RCODE SwiPstGetSipUserId(swi_charp pszUserId, 
                                     swi_uint32 szLength, 
                                     swi_uint32 timeout)
{
    return SwiIntGetNvU16LenString(CNS_PST_NV, NV_PAP_USER_ID, pszUserId, 
            szLength, LENGTH_NvSipUserID, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function returns the Simple IP profile 1 (index 0) password string.
 *
 * The password is set using SwiPstSetSipPassword.
 *
 * \param pszPassword[OUT] The destination in which the user password is 
 *                         returned.
 * \param szLength[IN]     The maximum size of the output buffer.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 * \sa SwiPstGetSipPassword.
 */
SWI_API SWI_RCODE SwiPstGetSipPassword(swi_charp pszPassword, 
                                       swi_uint32 szLength, 
                                       swi_uint32 timeout)
{
    return SwiIntGetNvU16LenString(CNS_PST_NV, NV_PPP_PASSWORD, pszPassword, 
            szLength, LENGTH_NvSipPassword, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function the Simple IP profile 2 (index 1) user ID string.
 *
 * The user ID is also referred to as the NAI (refer to IS-638B section 3.5.8).
 *
 * The User ID is set using SwiPstSetSip2UserId.
 *
 * \param pszUserId[OUT]   The destination in which the user ID string is 
 *                         returned.
 * \param szLength[IN]     The maximum size of the output buffer.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 * \sa SwiPstGetSipPassword.
 */
SWI_API SWI_RCODE SwiPstGetSip2UserId(swi_charp pszUserId, 
                                      swi_uint32 szLength, 
                                      swi_uint32 timeout)
{
    return SwiIntGetNvU16LenString(CNS_PST_SWINV, SWI_NV_SIP2_USER_ID, 
            pszUserId, szLength, LENGTH_NvSipUserID, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function returns the Simple IP profile 2 (index 1) password string.
 *
 * The password is set using SwiPstSetSip2Password.
 *
 * \param pszPassword[OUT] The destination in which the user password is 
 *                         returned.
 * \param szLength[IN]     The maximum size of the output buffer.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 * \sa SwiPstGetSip2Password.
 */
SWI_API SWI_RCODE SwiPstGetSip2Password(swi_charp pszPassword, 
                                        swi_uint32 szLength, 
                                        swi_uint32 timeout)
{
    return SwiIntGetNvU16LenString(CNS_PST_SWINV, SWI_NV_SIP2_PASSWORD, 
            pszPassword, szLength, LENGTH_NvSipPassword, 0, SWI_AMCDMA, 
            timeout);
}

/**
 *
 * This function configures various Mobile IP profile settings.
 *
 * Supported for only Mobile IP enabled devices. See SwiPstGetDsQcMip for 
 * determining MIP capability.
 *
 * \param index[IN]              Index # of Profile to modify: 0-5.
 * \param naiLength[IN]          Length of NAI field in bytes.
 * \param nai[IN]                NAI (username)
 * \param mnHaSpiEnable[IN]      MN-HA SPI enable
 *                                 0 - disable
 *                                 1 - enable
 * \param mnHaSpi[IN]            MN-HA SPI
 * \param mnAaaSpiEnable[IN]     MN-AAA SPI enable
 *                                 0 - disable
 *                                 1 - enable
 * \param mnAaaSpi[IN]           MN-AAA SPI
 * \param reverseTunneling[IN]   Reverse Tunneling Preferred
 *                                 0 - disable
 *                                 1 - enable
 * \param homeAddress[IN]        Home IP Address
 * \param primaryHaAddress[IN]   Primary HA IP Address (big-endian)
 * \param secondaryHaAddress[IN] Secondary HA IP Address (big-endian)
 * \param timeout[IN]            Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetDsMipGenUser(swi_uint16 index, 
                                        swi_uint16 naiLength, 
                                        swi_charp nai, 
                                        swi_uint16 mnHaSpiEnable, 
                                        swi_uint32 mnHaSpi, 
                                        swi_uint16 mnAaaSpiEnable, 
                                        swi_uint32 mnAaaSpi, 
                                        swi_uint16 reverseTunneling, 
                                        swi_uint32 homeAddress, 
                                        swi_uint32 primaryHaAddress, 
                                        swi_uint32 secondaryHaAddress, 
                                        swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    if(naiLength > LENGTH_NvNai)
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
    parmlength = SwiPstPackDsMipGenUser( outparamp, index, naiLength, nai, 
        mnHaSpiEnable, mnHaSpi, mnAaaSpiEnable, mnAaaSpi, reverseTunneling, 
        homeAddress, primaryHaAddress, secondaryHaAddress);
    
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
 * This function retrieves various Mobile IP profile settings.
 *
 * Supported for only Mobile IP enabled devices. See SwiPstGetDsQcMip for 
 * determining MIP capability.
 *
 * \param index[IN]                Index # of Profile to modify: 0-5.
 * \param naiLength[IN]            Length of NAI field in bytes.
 * \param nai[OUT]                 NAI (username)
 * \param pMnHaSpiEnable[OUT]      MN-HA SPI enable
 *                                   0 - disable
 *                                   1 - enable
 * \param pMnHaSpi[OUT]            MN-HA SPI
 * \param pMnAaaSpiEnable[OUT]     MN-AAA SPI enable
 *                                   0 - disable
 *                                   1 - enable
 * \param pMnAaaSpi[OUT]           MN-AAA SPI
 * \param pReverseTunneling[OUT]   Reverse Tunneling Preferred
 *                                   0 - disable
 *                                   1 - enable
 * \param pHomeAddress[OUT]        Home IP Address
 * \param pPrimaryHaAddress[OUT]   Primary HA IP Address (big-endian)
 * \param pSecondaryHaAddress[OUT] Secondary HA IP Address (big-endian)
 * \param timeout[OUT]             Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetDsMipGenUser(swi_uint16 index, 
                                        swi_uint16 naiLength,
                                        swi_charp nai, 
                                        swi_uint16* pMnHaSpiEnable, 
                                        swi_uint32* pMnHaSpi, 
                                        swi_uint16* pMnAaaSpiEnable, 
                                        swi_uint32* pMnAaaSpi, 
                                        swi_uint16* pReverseTunneling, 
                                        swi_uint32* pHomeAddress, 
                                        swi_uint32* pPrimaryHaAddress, 
                                        swi_uint32* pSecondaryHaAddress, 
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
    paramlength = SwiPackNvU16( outparamp, NV_DS_MIP_GEN_USER_PROF, index );

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
        swi_uint16 nNvIndex;
        swi_uint16 nIndex;
        swi_uint16 nNaiLength;

        nNvIndex = piget16(&inparamp);
        nIndex = piget16(&inparamp);

        if(NV_DS_MIP_GEN_USER_PROF != nNvIndex || nIndex != index)
        {
            resultcode = SWI_RCODE_UNEXPECTED_RESP;
        }
        else
        {
            nNaiLength = piget16(&inparamp);
            if(nNaiLength >= naiLength || nNaiLength >= LENGTH_NvNai)
            {
                resultcode = SWI_RCODE_SMALL_BUF;
            }
            else
            {
                slmemset(nai, 0, nNaiLength);
                pigetmulti(&inparamp, (swi_uint8 *)nai, nNaiLength);
                inparamp += LENGTH_NvNai-nNaiLength;
                *pMnHaSpiEnable = piget16(&inparamp);
                *pMnHaSpi = piget32(&inparamp);
                *pMnAaaSpiEnable = piget16(&inparamp);
                *pMnAaaSpi = piget32(&inparamp);
                *pReverseTunneling = piget16(&inparamp);
                *pHomeAddress = piget32(&inparamp);
                *pPrimaryHaAddress = piget32(&inparamp);
                *pSecondaryHaAddress = piget32(&inparamp);
            }
        }
        
    }
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function reports the modem's Mobile IP call support.
 *
 * \param pBehaviour[OUT] The destination in which the MIP support is returned:
 *                        0 - Not supported.  Simple IP only.
 *                        1 - MIP Prefered.  (Fallback to SIP if MIP fails)
 *                        2 - MIP Only.
 * \param timeout[IN]     Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetDsQcMip(swi_uint16* pBehaviour, swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_NV, NV_DS_QCMIP, pBehaviour, SWI_AMCDMA, 
            timeout);
}

/**
 *
 * This function sets the MDN (Mobile Directory Number) of a specified NAM 
 * account profile.
 *
 * The modem must be unlocked via SwiPstUnlock to execute this command.
 *
 * The MDN has a minimum length of 1 and a maximum length of 15 digits.
 *
 * The data is not committed until SwiPstLock is called.
 *
 * \param Nam[IN]          NAM index.
 * \param MDN[IN]          Mobile Directory Number to store.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetMobileDirNum(swi_uint16 Nam, 
                                     const swi_charp MDN, 
                                     swi_uint32 timeout)
{
    return SwiIntSetNamU16LenString(CNS_MOBILE_DIRECTORY_NUM, Nam, MDN, 
            LENGTH_Mdn, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets a table of SID and NID values
 * to be designated as "home" (not roaming).
 *
 * The modem must be unlocked via SwiPstUnlock
 * to execute this command.
 *
 * Not all service providers require the user to alter
 * these settings. The current table can be read
 * using SwiPstGetHomeSidNid.
 *
 * The data is not committed until SwiPstLock is
 * called. See "Activation process in documentation
 * for a description of the activation process.
 *
 *
 * \param  Nam[IN]                 Index (base 0) of the NAM
 *                                 account to set. See
 *                                 SwiGetNamInfo.
 * \param  HomeSidNidp[IN]         Pointer to structure containing the 
 *                                 data to store. See documentation for
 *                                 structure details.
 * \param  timeout[IN]             Function timeout in milliseconds
 *
 * \return      No special handling.  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetHomeSidNid(
                    const swi_uint16                   Nam, 
                    const struct SWI_STRUCT_HomeSidNid *HomeSidNidp, 
                    swi_uint32                         timeout)
{
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Parameter Field Construction Zone - get a pointer to the 
     * location to fill in the CnS parameter field 
     */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Start packing the field with the caller's data and 
     * update the cns length field when done
     */
    parmlength = SwiPackSidNid( outparamp, HomeSidNidp, Nam );

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
 * This function sets the CDMA preferred service for a specified NAM account 
 * profile. This is the fallback channel scan if the PRL is exhausted without 
 * finding service.
 *
 * This value is usually set at the factory for the specific service provider. 
 * Only change the setting at the service provider’s direction. The current 
 * setting can be read using SwiPstGetSystemPref. The modem must be reset via 
 * SwiResetModem to have the setting take effect.
 *
 * \param Nam[IN]          NAM index.
 * \param PrefServ[IN]     The system preference to set in the selected NAM.
 *                         See SWI_TYPE_NvSystemPref.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetSystemPref(swi_uint16 Nam, 
                                      swi_uint16 PrefServ, 
                                      swi_uint32 timeout)
{
    swi_uint16 prefServ;
    swi_uint16 nModePref, nModeTerm, nRoamPref;
    swi_uint32 nModeDur, nBandPref;

    prefServ = PrefServ;

    if (prefServ == SWI_NV_SYSTEM_PREF_AutoA)
        prefServ = SWI_NV_SYSTEM_PREF_AOnly;
    else if (prefServ == SWI_NV_SYSTEM_PREF_AutoB)
        prefServ = SWI_NV_SYSTEM_PREF_BOnly;
    else if (prefServ > SWI_NV_SYSTEM_PREF_AutoB)
        return SWI_RCODE_INVALID_PAR;


    nModeTerm = CNS_MODE_TERM_PERMANENT;
    nModePref = CM_MODE_PREF_NO_CHANGE;

    nModeDur = 0;   /* Ignored by firmware in the PERMANENT case */

    switch (prefServ)
    {
    case 0:
        nBandPref = CNS_BAND_PREF_BC0_A;
        nRoamPref = CNS_ROAM_PREF_ANY;
        break;
    case 1:
        nBandPref = CNS_BAND_PREF_BC0_B;
        nRoamPref = CNS_ROAM_PREF_ANY;
        break;
    case 2:
        nBandPref = CNS_BAND_PREF_ANY;
        nRoamPref = CNS_ROAM_PREF_HOME;
        break;
    case 3:
        nBandPref = CNS_BAND_PREF_ANY;
        nRoamPref = CNS_ROAM_PREF_ANY;
        break;
    default:
        return SWI_RCODE_INVALID_PAR;
    }

    return SwiSetTempModePref(nModePref,            /* mode_pref */
                               nModeTerm,           /* term */
                               nModeDur,            /* duration */
                               nBandPref,           /* band_pref */
                               nRoamPref,           /* roam_pref */
                               timeout);

}

/**
 *
 * This function sets the Simple IP profile 1 (index 0) user ID string.
 *
 * The user ID is also referred to as the NAI (refer to IS-638B 
 * section 3.5.8). The current setting can be read using SwiPstGetSipUserID.
 *
 * The data is committed and takes effect immediately.
 *
 * \param pszUserId[IN]    The user ID string to set.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 * \sa SwiPstSetSipPassword.
 */
SWI_API SWI_RCODE SwiPstSetSipUserId(const swi_charp pszUserId, 
                                     swi_uint32 timeout)
{
    return SwiIntSetNvU16LenString(CNS_PST_NV, NV_PAP_USER_ID, pszUserId, 
            LENGTH_NvSipUserID, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets the Simple IP profile 1 (index 0) user password string.
 *
 * The current setting can be read using SwiPstGetSipPassword.
 *
 * The data is committed and takes effect immediately.
 * \param pszPassword[IN]  The user password string to store.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 * \sa SwiPstSetSipUserId.
 */
SWI_API SWI_RCODE SwiPstSetSipPassword(const swi_charp pszPassword, 
                                       swi_uint32 timeout)
{
    return SwiIntSetNvU16LenString(CNS_PST_NV, NV_PPP_PASSWORD, pszPassword, 
            LENGTH_NvSipPassword, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets the Simple IP profile 2 (index 1) user ID string.
 *
 * The user ID is also referred to as the NAI (refer to IS-638B 
 * section 3.5.8). The current setting can be read using SwiPstGetSip2UserID.
 *
 * The data is committed and takes effect immediately.
 *
 * \param pszUserId[IN]    The user ID string to set.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 * \sa SwiPstSetSip2Password.
 */
SWI_API SWI_RCODE SwiPstSetSip2UserId(const swi_charp pszUserId, 
                                      swi_uint32 timeout)
{
    return SwiIntSetNvU16LenString(CNS_PST_SWINV, SWI_NV_SIP2_USER_ID, 
            pszUserId, LENGTH_NvSipUserID, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets the Simple IP profile 2 (index 1) user password string.
 *
 * The current setting can be read using SwiPstGetSip2Password.
 *
 * The data is committed and takes effect immediately.
 * \param pszPassword[IN]  The user password string to store.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 * \sa SwiPstSetSip2UserId.
 */
SWI_API SWI_RCODE SwiPstSetSip2Password(const swi_charp pszPassword, 
                                        swi_uint32 timeout)
{
    return SwiIntSetNvU16LenString(CNS_PST_SWINV, SWI_NV_SIP2_PASSWORD, 
            pszPassword, LENGTH_NvSipPassword, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function reports the state of the modem’s current connections.
 *
 * Consult the data type for SWI_TYPE_CDMA_CallState for additional 
 * information on the meaning of the returned bit mask.
 *
 * There is a family of notifications used to indicate any changes in call 
 * states.
 *
 * \param CallState[OUT]   The destination in which the current call states 
 *                         are returned as a single bit-mapped value.
 *                         See SWI_TYPE_CDMA_CallState. 
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetCallConnectionState(swi_uint16* CallState, 
                                            swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_CALL_CONNECTION_STATE, CallState, SWI_AMCDMA, 
            timeout);
}

/**
 *
 * This function reports the state of the modem’s current connections.
 *
 * Consult the data type for SWI_TYPE_CallState for additional information on 
 * the meaning of the returned bit mask.
 *
 * \param CallState[OUT]   The destination in which the current call states 
 *                         are returned as a single bit-mapped value and the 
 *                         SO value of the call. This SO value is ONLY 
 *                         meaningfull when the current call is ACTIVE 
 *                         (CONNECTED).
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetCallConnectionStateEx(
        SWI_STRUCT_CallConnected* CallState, 
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
                                  CNS_CALL_CONNECTED, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &paramlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackCallConnectionStateEx( inparamp, CallState );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function originates a Voice, QNC, or 1xRTT call / connection.
 *
 * \param callType[IN]     The type of call to originate. See 
 *                         SWI_TYPE_CDMA_CallType. The valid options are:
 *                           SWI_CALL_TYPE_Voice
 *                           SWI_CALL_TYPE_AsyncData
 *                           SWI_CALL_TYPE_PacketData
 * \param szCallNumber[IN] The number to dial. You must ensure the string 
 *                         contains only characters: #, *, 0-9.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetCallOriginateCmd(swi_uint16 callType, 
                                         const swi_charp szCallNumber, 
                                         swi_uint32 timeout)
{
    return SwiIntSetU16String(CNS_CALL_ORIGINATE_CMD, callType, szCallNumber, 
            LENGTH_CallNumber, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function disconnects the current call.
 *
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetCallDisconnectCmd()
{
    return SwiIntSet(CNS_CALL_DISCONNECT_CMD, SWI_AMCDMA, 
            APTIMEOUT);
}

/**
 *
 * This function reports the elapsed time of the current call.
 *
 * Applicable to both data and voice calls.
 *
 * \param pCallDuration[IN] Duration of current call in seconds.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetCallDuration(swi_uint32* pCallDuration, 
                                     swi_uint32 timeout)
{
    return SwiIntGetU32(CNS_CALL_DURATION, pCallDuration, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets the active profile in which a Mobile IP call will use.
 *
 * This is supported for only Mobile IP enabled modems.
 *
 * \param pActive[OUT]     Active Profile # returned from modem.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetDsMipActiveProf(swi_uint16* pActive, 
                                           swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_NV, NV_DS_MIP_ACTIVE_PROF, pActive, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets the active profile in which a Mobile IP call will use.
 *
 * This is supported for only Mobile IP enabled modems.
 *
 * \param active[IN]    Profile # to set as active. Supported values are 0-5.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetDsMipActiveProf(swi_uint16 active, 
                                           swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_NV, NV_DS_MIP_ACTIVE_PROF, active, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function returns the last Mobile IP registration error received from 
 * the network.
 *
 * \param nErrCode[OUT]    The destination in which the error code is returned.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 * \sa SwiGetMipErrCodeEx()
 */
SWI_API SWI_RCODE SwiGetMipErrCode(swi_uint16 *nErrCode, swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_MIP_ERR_CODE, nErrCode, SWI_AMCDMA, timeout);
}

/**
 *
 * This function returns the last Mobile IP registration error received from 
 * the network. 
 *
 * This function extends SwiGetMipErrCode by adding support for 
 * carrier-defined errors (nErrCode2).
 *
 * The standard error code definitions can be found in the following public 
 * references: Mobile IP RFC2002.
 *
 * The classification of the error codes used by Watcher are as follows:
\verbatim
        - No Error: 0, 1, 0xffff
        - Registration failure: 67
        - Sign-in failure: 131, 133
        - Session request too long: 69
        - Home Agent failure: 80, 81, 82, 88
        - Unknown Error: 64, 65, 128, 129
        - Unrecognizable Home Agent Addresses: 136
        - Network Error: 66, 68, 97-101, 130, 132, 140, 141
        - System Error: 70-76, 79, 96, 104-106, 134, 135, 137-139
        - Cannot prepare data services: 1012
        - Suspension of Account: 801
        - Carrier Specific: 89
\endverbatim
 * Carrier specific error codes (valid when nErrCode = 89) include the 
 * following:
\verbatim
    Verizon:
        1 - General Failure
        2 - Authentication Failure
        3 - Invalid Service Failure
        4 - Invalid Public Key Failure
\endverbatim
 * \param nErrCode1[OUT]   The destination in which the error code is returned.
 * \param nErrCode2[OUT]   Carrier specific error code.  Valid only when 
 *                         nErrCode = 89.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetMipErrCodeEx(swi_uint16 *nErrCode1, 
                                     swi_uint16 *nErrCode2, 
                                     swi_uint32 timeout)
{
    swi_uint16 array[2];
    SWI_RCODE rc;

    rc = SwiIntGetU16s(CNS_MIP_ERR_CODE, array, 2, SWI_AMCDMA, timeout);
    if(SWI_RCODE_OK == rc)
    {
        *nErrCode1 = array[0];
        *nErrCode2 = array[1];
    }

    return rc;
}

/**
 *
 * This function reports whether the alert tone for changes in service 
 * (in or out of coverage)
 *
 * The setting can be changed using SwiPstSetServiceAreaAlert.
 *
 * Enables/disables playing an alert tone in the headset when there is a 
 * change in the type of coverage. A call to SwiGetServiceIndication will 
 * confirm the nature of the change in service.
 *
 * \param srvAreaAlert[OUT] The destination in which the service alert state 
 *                          is returned:
 *                            0 = disabled
 *                            1 = enabled
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetServiceAreaAlert(swi_uint16* srvAreaAlert, 
                                            swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_NV, NV_SVC_AREA_ALERT, srvAreaAlert, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function enables or disables the playing of a beep tone to alert the 
 * user of changes in service while in a voice call.
 *
 * Enables/disables playing an alert tone in the headset when there is a 
 * change in the type of coverage. A call to SwiGetServiceIndication will 
 * confirm the nature of the change in service.
 *
 * The setting can be read using SwiPstGetServiceAreaAlert.
 *
 * \param srvAreaAlert[IN] The service alert state to set:
 *                           0 = disabled
 *                           1 = enabled
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetServiceAreaAlert(swi_uint16 srvAreaAlert, 
                                            swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_NV, NV_SVC_AREA_ALERT, srvAreaAlert, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets modem preference
 *
 * \param  nModePref[IN]    Modem preference
 * \param  nModeTerm[IN]    Mode preference change termination
 * \param  nModeDur[IN]     Time in secs for temp. mode preference change
 * \param  nBandPref[IN]    Band preference to switch to
 * \param  nRoamPref[IN]    Roam preference to switch to
 *              
 * \return SWI_RCODE_OK            The function executed normally. 
 * \return SWI_RCODE_FAILED        The operation failed in the modem. 
 * \return SWI_RCODE_REQUEST_TIMEOUT The function failed to complete within 
 *                                      the timeout.  
 */
SWI_API SWI_RCODE SwiSetTempModePref(swi_uint16 nModePref, 
                                     swi_uint16 nModeTerm, 
                                     swi_uint32 nModeDur, 
                                     swi_uint32 nBandPref, 
                                     swi_uint16 nRoamPref,
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
    parmlength = SwiPackModePref( outparamp, 
                                  nModePref,
                                  nModeTerm,
                                  nModeDur,
                                  nBandPref,
                                  nRoamPref );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PH_SS_PREF, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */
                     
    /* The response packet for this request contains the same 
     * information we just sent in the request packet. The 
     * 3G SDK "does not care" about payloads in responses to 
     * Set operations and discards the information without 
     * looking at it. Therefore it's OK for us to do the same
     */

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function gets the current RSSI and EcIo values for Signal 
 * Stregth reporting.
 *
 * \param  [OUT] stRssiEcio Struct with RSSI and EcIo values.
 *              
 * \return SWI_RCODE_OK The function executed normally. 
 * \return SWI_RCODE_FAILED The operation failed in the modem. 
 * \return SWI_RCODE_REQUEST_TIMEOUT The function failed to complete within 
 *
 * \sa     See SwiStructs.h for SWI_STRUCT_Rssi_Ecio definition.
 *
 */
SWI_API SWI_RCODE SwiGetRssiEcio(SWI_STRUCT_Rssi_Ecio* stRssiEcio, 
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
                                  CNS_RSSI_ECIO, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &paramlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackRssiEcio( inparamp, stRssiEcio );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function gets the current EVDO RSSI.
 *
 * \param lRssi[OUT]       RSSI value returned from modem.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiEVDOGetRssi(swi_int32* lRssi, swi_uint32 timeout)
{
    SWI_RCODE resultcode; /* Result of call to cnSendWait() */
    swi_uint16 nValue;

    resultcode = SwiIntGetU16(CNS_HDR_RSSI, &nValue, SWI_AMCDMA, timeout);
    if(SWI_RCODE_OK == resultcode)
    {
        *lRssi = (swi_int32)nValue;
    }
    return resultcode;
}

/**
 *
 * This function gets the current roaming status for HDR service.
 *
 * \param iHdrRoamStatus[OUT] HDR roam status. See SWI_TYPE_Hdr_Roam_Status.
 * \param timeout[IN]         Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetHdrRoamStatus(swi_uint16* iHdrRoamStatus, 
                                      swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_HDR_ROAM_STATUS, iHdrRoamStatus, SWI_AMCDMA, 
            timeout);
}

/**
 *
 * This function gets the HDR service state.
 *
 * \param iHdrSrvState[OUT] HDR service state. See SWI_TYPE_Hdr_Srv_State. 
 *                          0 = No Service; 2 = HDR Service
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetHdrSrvState(swi_uint16* iHdrSrvState, 
                                    swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_HDR_SRV_STATE, iHdrSrvState, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets the HDR Hybrid Pref setting.
 *
 * \param iHdrHybrPref[OUT] HDR Hybrid Pref setting. 
 *                          See SWI_TYPE_Hdr_Hybr_Pref.  0 = OFF; 1 = ON
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetHdrHybrPref(swi_uint16* iHdrHybrPref, 
                                    swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_HDR_HYBR_PREF, iHdrHybrPref, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets the HDR Hybrid Pref setting.
 *
 * \param iHdrHybrPref[IN] HDR Hybrid Pref setting. 0 = OFF; 1 = ON
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetHdrHybrPref(swi_uint16 iHdrHybrPref, 
                                    swi_uint32 timeout)
{
    return SwiIntSetU16(CNS_HDR_HYBR_PREF,  iHdrHybrPref, NULL, SWI_AMCDMA, 
            timeout);
}

/**
 *
 * This function request that the modem close the HDR session.
 *
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiHdrCloseSession(swi_uint32 timeout)
{
    return SwiIntSet(CNS_HDR_CLOSE_SESS, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets a mask of which HDR notifications are enabled.
 *
 * \param iHdrNotificationStatus[OUT] Notification Mask.
 * \param timeout[IN]                 Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetHdrNotificationStatus(
        swi_uint16* iHdrNotificationStatus,
        swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_HDR_NOTIFICATION_STATUS, iHdrNotificationStatus, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets a mask of which Tech HDR notifications are enabled.
 *
 * \param iTechHdrNotificationStatus[OUT] Notification Mask.
 * \param timeout[IN]                     Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetTechHdrNotificationStatus(
        swi_uint16* iTechHdrNotificationStatus,
        swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_TECH_HDR_NOTIFICATION_STATUS, 
            iTechHdrNotificationStatus, SWI_AMCDMA, timeout);
}

/**
 *
 * This function the HDR Nai
 *
 * The user ID is also referred to as the NAI (refer to IS-638B section 3.5.8).
 *
 * The User ID is set using SwiPstSetHdrUserId.
 *
 * \param pszUserId[OUT]   The destination in which the HDR user ID string is 
 *                         returned.
 * \param szLength[IN]     The maximum size of the output buffer.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 * \sa SwiPstGetHdrPassword.
 */
SWI_API SWI_RCODE SwiPstGetHdrUserId(swi_charp pszUserId, 
                                     swi_uint32 szLength, 
                                     swi_uint32 timeout)
{
    return SwiIntGetNvU16LenString(CNS_PST_NV, NV_HDR_AN_AUTH_NAI, pszUserId, 
            szLength, LENGTH_NvHdrUserID, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets the Hdr Nai
 *
 * The user ID is also referred to as the NAI (refer to IS-638B section 3.5.8).
 *
 * The data is committed and takes effect immediately.
 *
 * The User ID is get using SwiPstGetHdrUserId.
 *
 * \param pszUserId[IN]    The HDR user ID string to set.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 * \sa SwiPstSetHdrPassword.
 */
SWI_API SWI_RCODE SwiPstSetHdrUserId(const swi_charp pszUserId, 
                                     swi_uint32 timeout)
{
    return SwiIntSetNvU16LenString(CNS_PST_NV, NV_HDR_AN_AUTH_NAI, pszUserId, 
            LENGTH_NvHdrUserID, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function returns the HDR password
 *
 * The HDR passeord is set using SwiPstSetHdrPassword.
 *
 * \param pszPassword[OUT] The destination in which the HDR password string 
 *                         is returned.
 * \param szLength[IN]     The maximum size of the output buffer.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 * \sa SwiPstGetHdrUserId.
 */
SWI_API SWI_RCODE SwiPstGetHdrPassword(swi_charp pszPassword, 
                                       swi_uint32 szLength, 
                                       swi_uint32 timeout)
{
    return SwiIntGetNvU16LenString(CNS_PST_NV, NV_HDR_AN_AUTH_PASSWORD, 
            pszPassword, szLength, LENGTH_NvHdrPassword, 0, SWI_AMCDMA, 
            timeout);
}

/**
 *
 * This function sets the HDR password
 *
 * The current setting can be read using SwiPstGetHdrPassword.
 *
 * \param pszPassword[IN]  The user password string to store.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 * \sa SwiPstSetHdrUserId.
 */
SWI_API SWI_RCODE SwiPstSetHdrPassword(const swi_charp pszPassword, 
                                       swi_uint32 timeout)
{
    return SwiIntSetNvU16LenString(CNS_PST_NV, NV_HDR_AN_AUTH_PASSWORD, 
            pszPassword, LENGTH_NvHdrPassword, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets auto connect flag (for data call) set in modem.
 *
 * \param pnEnable[OUT]    value 1 = enable ; 0 to disable.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetAutoconnect(swi_uint16* pnEnable, swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_AUTOCONNECT, pnEnable, SWI_AMCDMA, timeout);
}

/**
 *
 * This function This function will be used to set auto connect flag 
 * (for data call).
 *
 * \param nEnable[IN]      value 1 = enable ; 0 to disable.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetAutoconnect(swi_uint16 nEnable, swi_uint32 timeout)
{
    return SwiIntSetU16(CNS_AUTOCONNECT,  nEnable, NULL, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets the Mobile IP Mode setting.
 *
 * \param behaviour[IN]    The MIP mode behaviour:
 *                           0 = SIP only
 *                           1 = MIP Preferred (fallback to SIP if MIP fails)
 *                           2 = MIP only 
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetDsQcMip(swi_uint16 behaviour, swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_NV, NV_DS_QCMIP, behaviour, SWI_AMCDMA, 
            timeout);
}

/**
 *
 * This function gets the Preferred Operating Mode setting.
 *
 * \param Nam[IN]          NAM index.
 * \param PrefMode[OUT]    Preferred Mode setting. See SWI_TYPE_NvPrefOpMode.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetPrefMode(swi_uint16 Nam, 
                                    swi_uint16 *PrefMode, 
                                    swi_uint32 timeout)
{
    return SwiIntGetNvNamU16(CNS_PST_NV, NV_PREF_MODE, Nam, PrefMode, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets the Preferred Operating Mode value.
 *
 * \param Nam[IN]          NAM index.
 * \param PrefMode[IN]     This is a parameter.
 * \param timeout[IN]      Preferred Mode setting. See SWI_TYPE_NvPrefOpMode.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetPrefMode(swi_uint16 Nam, 
                                    swi_uint16 PrefMode, 
                                    swi_uint32 timeout)
{
    return SwiIntSetNvNamU16(CNS_PST_NV, NV_PREF_MODE, Nam, PrefMode, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets the Packet Zone ID value.
 *
 * \param packetZoneID[OUT] The Packet Zone ID setting returned from modem.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetTechPacketZone(swi_uint16 *packetZoneID, 
                                       swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_TECH_PACKET_ZONE, packetZoneID, SWI_AMCDMA, 
            timeout);
}

/**
 *
 * This function retrieve the SID
 *
 * \param systemID[OUT]    SID returned from modem.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetTechSID(swi_uint16 *systemID, swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_TECH_SID, systemID, SWI_AMCDMA, timeout);
}

/**
 *
 * This function retrieve the NID.
 *
 * \param networkID[OUT]   NID returned from modem.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetTechNID(swi_uint16 *networkID, swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_TECH_NID, networkID, SWI_AMCDMA, timeout);
}

/**
 *
 * This function retrieves the Foreign SID registration setting.
 *
 * \param Nam[IN]            NAM index.
 * \param TermSidEnable[OUT] Foreign SID registration setting 
 *                           (0 = disabled, 1 = enabled)
 * \param timeout[IN]        Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetTermSid(swi_uint16 Nam, 
                                   swi_uint16 *TermSidEnable, 
                                   swi_uint32 timeout)
{
    return SwiIntGetNvNamU16(CNS_PST_NV, NV_TERM_SID, Nam, TermSidEnable, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets the Foreign SID registration setting.
 *
 * \param Nam[IN]           NAM index.
 * \param TermSidEnable[IN] Foreign SID registration setting 
 *                          (0 = disabled, 1 = enabled)
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetTermSid(swi_uint16 Nam, 
                                   swi_uint16 TermSidEnable, 
                                   swi_uint32 timeout)
{
    return SwiIntSetNvNamU16(CNS_PST_NV, NV_TERM_SID, Nam, TermSidEnable, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function retrieve the Foreign NID registration setting.
 *
 * \param Nam[IN]            NAM index.
 * \param TermNidEnable[OUT] Foreign NID registration setting 
 *                           (0 = disabled, 1 = enabled)
 * \param timeout[IN]        Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetTermNid(swi_uint16 Nam, 
                                   swi_uint16 *TermNidEnable, 
                                   swi_uint32 timeout)
{
    return SwiIntGetNvNamU16(CNS_PST_NV, NV_TERM_NID, Nam, TermNidEnable, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets the Foreign NID registration setting.
 *
 * \param Nam[IN]           NAM index.
 * \param TermNidEnable[IN] Foreign NID registration setting 
 *                          (0 = disabled, 1 = enabled)
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetTermNid(swi_uint16 Nam, 
                                   swi_uint16 TermNidEnable, 
                                   swi_uint32 timeout)
{
    return SwiIntSetNvNamU16(CNS_PST_NV, NV_TERM_NID, Nam, TermNidEnable, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function retrieves the Active Set 
 *
 * \param pTechActSet[OUT] Active set list.  See SWI_STRUCT_TechActiveSet.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetTechActiveSet(SWI_STRUCT_TechActiveSet* pTechActSet, 
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
                                  CNS_TECH_ACTIVE_SET, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &paramlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackTechActiveSet( inparamp, pTechActSet );
        
        if(pTechActSet->nCount > MAX_ActiveSet)
        {
            resultcode = SWI_RCODE_UNEXPECTED_RESP;
        }
    }
    
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function sets Mobile Retry count
 *
 * \param retries[IN]      Retry count.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetDsMipRetries(swi_uint16 retries, swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_NV, NV_DS_MIP_RETRIES, retries, SWI_AMCDMA, 
            timeout);
}

/**
 *
 * This function get Mobile Retry count.
 *
 * \param pRetries[OUT]    Retry count.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetDsMipRetries(swi_uint16* pRetries, 
                                        swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_NV, NV_DS_MIP_RETRIES, pRetries, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function set Mobile Retry Interval (milliseconds).
 *
 * \param interval[IN]     Retry Interval.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetDsMipRetryInt(swi_uint16 interval, 
                                         swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_NV, NV_DS_MIP_RETRY_INT, interval, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets Mobile Retry Interval (milliseconds).
 *
 * \param pInterval[OUT]   Retry Interval
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetDsMipRetryInt(swi_uint16* pInterval, 
                                         swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_NV, NV_DS_MIP_RETRY_INT, pInterval, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets Pre lifetime expiry re-registration (minutes)
 *
 * \param rereg[IN]        Re-reg expiry.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetDsMipPrereRrqTime(swi_uint16 rereg, 
                                             swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_NV, NV_DS_MIP_PRE_RE_RRQ_TIME, rereg, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets Pre lifetime expiry re-registration (minutes)
 *
 * \param pRereg[IN]       Re-reg expiry.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetDsMipPrereRrqTime(swi_uint16* pRereg, 
                                             swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_NV, NV_DS_MIP_PRE_RE_RRQ_TIME, pRereg, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function Set QC optimized dormant handoff enable.
 *
 * \param active[IN]       Enabled = 1, Disabled = 0.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetDsMipQcDrsOpt(swi_uint16 active, swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_NV, NV_DS_MIP_QC_DRS_OPT, active, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function Get QC optimized dormant handoff enable.
 *
 * \param pActive[OUT]     Enabled = 1, Disabled = 0..
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetDsMipQcDrsOpt(swi_uint16* pActive, 
                                         swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_NV, NV_DS_MIP_QC_DRS_OPT, pActive, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets RFC2002bis MN-HA authenticator calculation enable.
 *
 * \param rfc2002[IN]      Enabled = 1, Disabled = 0.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetDsMip2002BisMnHaAuth(swi_uint16 rfc2002, 
                                                swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_NV, NV_DS_MIP_2002BIS_MN_HA_AUTH, rfc2002, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets RFC2002bis MN-HA authenticator calculation enable.
 *
 * \param pRfc2002[IN]     Enable = 1, Disable = 0.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetDsMip2002BisMnHaAuth(swi_uint16* pRfc2002, 
                                                swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_NV, NV_DS_MIP_2002BIS_MN_HA_AUTH, pRfc2002, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets Re-register only if traffic since last RRQ enable.
 *
 * \param reregistration[IN] Enable = 1, Disable = 0.
 * \param timeout[IN]        Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetDsMipRrqIfTfrk(swi_uint16 reregistration, 
                                          swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_NV, NV_DS_MIP_RRQ_IF_TFRK, reregistration, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets Re-register only if traffic since last RRQ enable.
 *
 * \param pReregistration[OUT] Enable = 1, Disable = 0.
 * \param timeout[IN]          Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetDsMipRrqIfTfrk(swi_uint16* pReregistration, 
                                          swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_NV, NV_DS_MIP_RRQ_IF_TFRK, pReregistration, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets MIP Profile Enabled list.
 *
 * This list should be used to manage which MIP profiles are enabled.
 *
 * \param sMipEnabled[OUT] MIP Profile Enabled list. (see SWI_STRUCT_MipEnabled)
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetDsMipEnableProf(SWI_STRUCT_MipEnabled* sMipEnabled, 
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
    paramlength = SwiPackNv( outparamp, NV_DS_MIP_ENABLE_PROF );

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
        swi_uint16 nNvIndex;
        
        nNvIndex = piget16(&inparamp);
        if(NV_DS_MIP_ENABLE_PROF != nNvIndex)
        {
            resultcode = SWI_RCODE_UNEXPECTED_RESP;
        }
        else
        {
            swi_uint16 idx;
            for (idx = 0; idx < MAX_ActiveSet; idx++) 
            {
                sMipEnabled->nMipEnabled[idx] = piget16(&inparamp);
            }       
        }
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function sets MIP Profile Enabled list.
 *
 * This list should be used to manage which MIP profiles are enabled.
 *
 * \param psMipEnabled[IN] MIP Profile Enabled list. 
 *                         (see SWI_STRUCT_MipEnabled)
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetDsMipEnableProf(
        const SWI_STRUCT_MipEnabled *psMipEnabled, 
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
    parmlength = SwiPstPackDsMipEnableProf( outparamp, psMipEnabled );

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

/* 
 * $Log: SwiApiCdmaNetwork.c,v $
 */


