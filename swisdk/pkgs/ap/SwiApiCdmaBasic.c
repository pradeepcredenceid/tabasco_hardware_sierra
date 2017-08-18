/**
 *    \if CvsId 
 *    $Id: SwiApiCdmaBasic.c,v 1.7 2011/02/11 22:09:02 ygao Exp $
 *    \endif
 *    \ingroup cdmabasic
 *    \file SwiApiCdmaBasic.c
 *    
 *    \brief SWI CDMA specific basic APIs.
 *
 *    Copyright (c) 2011 Sierra Wireless, Inc.  All rights reserved
 *
 */

#include "SwiApiCdmaBasic.h"
#include "SwiIntPkCdmaBasic.h"
#include "SwiIntUpkCdmaBasic.h"
#include "ap/apidefs.h"

/**
 *
 * This function reports whether the modem is in a state where it is ready 
 * for normal operation.  This function should be called after successfully 
 * calling SwiRegisterCallback, and before any other API function call.
 *
 * This function should always be called after successfully calling 
 * SwiRegisterCallback (which is always called after SwiApiOpen, 
 * and before any other API function call.
 *
 * When the modem undergoes a power cycle (typically after inserting 
 * the modem or resuming from a reset or host suspend), the modem may 
 * take some time to initialize.  During this period, the modem 
 * may not respond to any API modem functions, in spite a successful 
 * call with SwiApiOpen. This function is used to test for 
 * this condition, and should be repeatedly called until it succeeds.
 *
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiIsModemReady(swi_uint32 timeout)
{
    swi_int32 lEsn;
    
    return SwiGetESN(&lEsn, timeout);
}

/**
 *
 * This function returns the device's unique identity as a string. The 
 * returned value is the ESN (Electronic Serial Number) of the modem.
 *
 * Managing display of the ESN as a string is up to you. In the CDMA 
 * marketplace, the ESN is usually presented as the single 8-character string, 
 * in the form of a hexadecimal number; for example: CE0271C8.
 *
 * \param lEsn[OUT]        ESN returned from modem.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetESN(swi_int32* lEsn, swi_uint32 timeout)
{
    return SwiIntGetU32(CNS_ESN, (swi_uint32 *)lEsn, SWI_AMCDMA, timeout);
}

/**
 *
 * This function returns the device's unique identity
 * The returned value is the MEID (Mobile Equipment Identifier) and
 * ESN (Electronic Serial Number) of the modem.
 *
 * Managing display of the MEID and ESN is up to the caller
 *
 * \param timeout[IN]   Function timeout in milliseconds
 * \param pMeidEns[OUT] The destination in which the MEID and ESN
 *                      is returned.
 *
 * \return  See SwiRcodes.h
 *
 *
 */
SWI_API SWI_RCODE SwiGetMEIDESN(struct SWI_STRUCT_MeidEsn *pMeidEsn, 
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
                                  CNS_MEID_ESN, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &paramlength );
                     
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackMeidEsn( inparamp, pMeidEsn );
    }
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function gets PRL version
 *
 * \param pPrlVersion[OUT] Version number of the current PRL.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetPRLVersion(swi_uint16* pPrlVersion, swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_PRL_VERSION, pPrlVersion, SWI_AMCDMA, timeout);
}

/**
 *
 * This function reports the temperature of the radio within the modem.
 *
 * The modem firmware monitors the radio temperature to protect electronics 
 * and prevent drift from specification. Consult the product specification
 * for the specific type of unit you are using to determine the way the
 * modem handles high temperatures.
 *
 * \param iTemperature[OUT] Temperature in degrees C.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetTemperature(swi_int32  *iTemperature, 
                                    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint16 value;
    
    resultcode = SwiIntGetU16(CNS_TECH_TEMP, &value, SWI_AMCDMA, timeout);
    if( resultcode == SWI_RCODE_OK )
    {
        *iTemperature = (swi_int32)value;
    }

    return resultcode;
}

/**
 *
 * This function reports the activation status of the modem.
 *
 * This function can be used to determine if the modem has been activated 
 * with at least one NAM account profile. If not, the application should only 
 * offer the user E911 service and a mechanism to activate the modem with the
 * service provider, either by OTASP or direct data entry.
 *
 * \param pActivationStatus[OUT] The destination in which the status is 
 *                               returned: 0 = False; 1 = True.
 * \param timeout[IN]            Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetActivationStatus(swi_uint16* pActivationStatus, 
                                         swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_ACTIVATION_STATUS, pActivationStatus, SWI_AMCDMA, 
            timeout);
}

/**
 *
 * This function returns the phone number from the currently active NAM 
 * account profile.
 *
 * The phone number is returned as an null-termi-nated ASCII string of 10-15 
 * numerals.
 *
 * The current NAM account profile is set with SwiPstSetCurrentNam.
 *
 * \param PhoneNumber[OUT] The destination in which the current account phone 
 *                         number is returned.
 * \param BufferSize[IN]   The maximum size of the output string(PhoneNumber).
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetPhoneNumber(swi_charp PhoneNumber, 
                                    swi_uint32 BufferSize, 
                                    swi_uint32 timeout)
{
    return SwiIntGetString(CNS_PHONE_NUMBER, PhoneNumber, BufferSize, 
            LENGTH_PhoneNumberMin, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function reports the modem's voice/data capabilities
 *
 * \param voiceDataMask[OUT] The destination in which the data is returned. 
 *                           Possible mask bit values are defined in 
 *                           SWI_TYPE_PRI_VoiceDataMask (See SwiStructs.h for 
 *                           details)
 * \param timeout[IN]        Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPriGetVoiceDataMask(swi_uint16* voiceDataMask, 
                                         swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_SWINV, SWI_NV_PRI_WA_VOICEDATA, 
            voiceDataMask, SWI_AMCDMA, timeout);
}

/**
 *
 * This function reports the ID of the carrier the modem is configured for.
 *
 * \param pnCarrierID[OUT] Carrier ID. Possible enumerated values are defined 
 *                         in SWI_TYPE_CarrierID.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetCarrierID(swi_int16* pnCarrierID, swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_SWINV, SWI_NV_CARRIER_ID,
                          (swi_uint16 *)pnCarrierID, SWI_AMCDMA, timeout);
}

/**
 *
 * This function reports the ID of the Distributor the modem is configured for.
 *
 * \param pnDistributorID[OUT] Distributor ID. Possible enumerated values are 
 *                             defined in SWI_TYPE_DistributorID
 * \param timeout[IN]          Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetDistributorID(swi_int16* pnDistributorID, 
                                      swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_SWINV, SWI_NV_WATCHER_TITLE, 
            (swi_uint16 *)pnDistributorID, SWI_AMCDMA, timeout);
}

/**
 *
 * This function returns the date and time from the CDMA network.
 *
 * \param year[OUT]    Year as a binary number; such as 0x7D0 for year 2000.
 * \param month[OUT]   Month in binary
 * \param day[OUT]     Day date in the month in binary
 * \param weekDay[OUT] Day of the week as a numeric: 0x0001 = Mon, 
 *                     0x0001 = Mon, 0x0007 = Sun
 * \param hour[OUT]    Hour in binary (24-hour clock). 
 *                     For example: 0x0016 = 10 PM
 * \param minute[OUT]  Minutes in binary
 * \param second[OUT]  Seconds in binary
 * \param timeout[IN]  Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetSystemTime(swi_uint16 *year, 
                                   swi_uint16 *month, 
                                   swi_uint16 *day, 
                                   swi_uint16 *weekDay, 
                                   swi_uint16 *hour, 
                                   swi_uint16 *minute, 
                                   swi_uint16 *second, 
                                   swi_uint32 timeout)
{
    swi_int16 array[7];
    SWI_RCODE rc;

    rc = SwiIntGetU16s(CNS_SYSTEM_TIME, (swi_uint16 *)array, 7, SWI_AMCDMA,
                       timeout);
    if(SWI_RCODE_OK == rc)
    {
        *year = array[0];
        *month = array[1];
        *day = array[2];
        *weekDay = array[3];
        *hour = array[4];
        *minute = array[5];
        *second = array[6];
    }

    return rc;
}

/**
 *
 * This function This function reports the current E911 mode of the modem. 
 * Modem features may be disabled in E911 mode, based on service provider 
 * settings.
 *
 * The modem enters E911 mode when a voice call is originated using one of the 
 * pre-programmed emergency numbers. The mode is cleared after five minutes or 
 * manually via SwiSetE911Mode.
 *
 * There is a notification (SWI_NOTIFY_E911Mode) to report the modem’s entry 
 * and exit from E911 mode.
 *
 * \param e911Mode[OUT]    The destination in which the mode is returned:
 *                         0 = Off; 1 = On
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetE911Mode(swi_uint16* e911Mode, swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_E911_MODE, e911Mode, SWI_AMCDMA, timeout);
}

/**
 *
 * This function turns off the E911 mode of the modem.
 *
 * The modem enters E911 mode when a voice call is originated using one of 
 * the pre-programmed emergency numbers. The mode is cleared after five 
 * minutes, or this function can be used to force the modem back to normal 
 * operations.
 *
 * There is a notification (SWI_NOTIFY_E911Mode) to report the modem’s entry 
 * and exit from E911 mode.
 *
 * \param e911Mode[IN]     Must be zero (0).
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetE911Mode(swi_uint16 e911Mode, swi_uint32 timeout)
{
    return SwiIntSetU16(CNS_E911_MODE,  e911Mode, NULL, SWI_AMCDMA, timeout);
}

/**
 *
 * This function reports the current EC/IO value
 *
 * The EC/IO value returned is positive, and should be negated to be 
 * interpreted correctly.
 *
 * \param pEcio[OUT]     The destination in which the EC/IO value is returned.
 * \param timeout[IN]    Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetECIO(swi_uint16* pEcio, swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_ECIO, pEcio, SWI_AMCDMA, timeout);
}

/**
 *
 * This function reports information about the modem’s NAM profile table: the 
 * number of profiles supported, and the next empty table entry.
 *
 * \param cntNamProfile[OUT]  The total number of NAM profiles the modem can 
 *                            support.
 * \param nextNamProfile[OUT] The index number (base 0) of the next empty 
 *                            profile. 0xFFFF indicates no empty profiles.
 * \param timeout[IN]         Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetNamInfo(swi_uint16 *cntNamProfile, 
                                swi_uint16 *nextNamProfile, 
                                swi_uint32  timeout)
{
    swi_int16 array[2];
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */

    resultcode = SwiIntGetU16s(CNS_NAM_INFO, (swi_uint16 *)array, 2,
                               SWI_AMCDMA, timeout);
    if(SWI_RCODE_OK == resultcode)
    {
        *cntNamProfile = array[0];
        *nextNamProfile = array[1];
    }

    return resultcode;
}

/**
 *
 * This function returns the currently active NAM index number.
 *
 * This account profile is used for all call connections. Use 
 * SwiPstSetCurrentNam to change the active account.
 *
 * \param CurrentNam[OUT]  The index (base 0) of the currently active NAM 
 *                         account profile.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetCurrentNam(swi_uint16 *CurrentNam, 
                                      swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_NV, NV_CURR_NAM, CurrentNam, SWI_AMCDMA, 
            timeout);
}

/**
 *
 * This function returns the NAM valid status
 *
 * \param pNamValidStatus[OUT] Returns the NAM valid status.
 *                             0x0000 – Incomplete NAM, Service Required
 *                             0x0001 – Complete NAM
 *                             0x0002 – Incomplete NAM, exceptional Service 
 *                                      Required
 * \param timeout[IN]          Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetNamValidStatus(swi_uint16* pNamValidStatus, 
                                       swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_NAM_VALID_STATUS, pNamValidStatus, SWI_AMCDMA, 
            timeout);
}

/**
 *
 * This function sets the index to a NAM account profile to make the active 
 * account for connec-tions and other queries.
 *
 * The modem must be reset via SwiResetModem to have the change take effect.
 *
 * The current setting can be read using SwiPstGetCurrentNam.
 *
 * \param CurrentNam[IN]   Index (base 0) to set as the currently active NAM 
 *                         account profile. See SwiGetNamInfo.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetCurrentNam(swi_uint16 CurrentNam, 
                                      swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_NV, NV_CURR_NAM, CurrentNam, SWI_AMCDMA, 
            timeout);
}

/**
 *
 * This function returns the date on which a specified NAM index was 
 * activated (written).
 *
 * The provisioning date is set whenever the NAM profile is written. The date 
 * comes from the CDMA network at the time the account profile is written, or 
 * as soon as CDMA coverage is available.
 *
 * \param namIndex[IN]     The (base 0) index number of the NAM account 
 *                         profile to query. See SwiGetNamInfo.
 * \param pszProvisioningData[OUT] Date string in the form YYYYMMDD. An empty 
 *                                 account index will report all zeroes.
 * \param length[IN]       The maximum size of the output string.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetProvisioningDate(swi_uint16 namIndex, 
                                         swi_charp pszProvisioningData, 
                                         swi_uint32 length, 
                                         swi_uint32 timeout)
{
    return SwiIntGetNamString(CNS_PROVISIONING_DATE, namIndex, 
            pszProvisioningData, length, 0, LENGTH_ProvisioningDate, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function returns the MIN (Mobile Identifi-cation Number) string of 
 * the specified NAM account profile.
 *
 * This function returns the NMSI digits of the CDMA IMSI (refer to IS-95B).
 * The length of the MIN string can be up to 10 bytes.
 *
 * \param namIndex[IN]     Index (base 0) of the NAM account to query. 
 *                         See SwiGetNamInfo.
 * \param pszMIN[OUT]      The MIN string of the selected NAM profile.
 * \param length[IN]       Size of the buffer to receive the result.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetCdmaIMSI(swi_uint16 namIndex, 
                                 swi_charp pszMIN, 
                                 swi_uint32 length, 
                                 swi_uint32 timeout)
{
    return SwiIntGetNamU16LenString(CNS_IMSI_CDMA, namIndex, pszMIN, length, 
            0, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function uses the subsidy lock value (OTSL, MSL, or SPC) to unlock 
 * access to activate (write) a NAM account profile.  
 *
 * \param SubsidyLock[IN]  Either the OTSL, or the MSL (also known as the 
 *                         SPC). This number is provided by the service 
 *                         provider.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstUnlock(swi_uint32 SubsidyLock, swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    swi_uint32 iRandom;
    swi_uint32 iScd;
    swi_uint8 nTemp[LENGTH_SecureChallenge];
    swi_char szTemp[32];
    swi_charp pszTemp;
    swi_uint32 nNum; 

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Parameter Field Construction Zone - get a pointer to the 
     * location to fill in the CnS parameter field 
     */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* accomodate 6 zeros as a MSL */
    if (SubsidyLock == 000000)
    {
        /* do the calculations with this subsidy lock */
        SubsidyLock = 1000000;
    }

    /* Generate a random number between 10 -20 */
    iRandom = 10;

    /* encrypt the Master Subsidy Lock */
    iScd = iRandom * SubsidyLock;
    pszTemp = (swi_charp)sluint64toasciidec(szTemp, 0, iScd); /* 8 digits max*/
    nNum = pszTemp-szTemp;
    if(nNum > LENGTH_SecureChallenge)
    {
        return SWI_RCODE_INVALID_PAR;
    }
    slmemset((char *)nTemp, '0', LENGTH_SecureChallenge-nNum);
    slmemcpy(nTemp+LENGTH_SecureChallenge-nNum, szTemp, nNum);

    /* Pack the CnS Parameter field with the input parameters */
    parmlength = SwiPackU8s( outparamp, (const swi_uint8 *)nTemp,
                             LENGTH_SecureChallenge );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_SECURE_CHALLENGE, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */
                     
    if (SWI_RCODE_OK != resultcode)
    {
        return resultcode;
    }
    
    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Parameter Field Construction Zone - get a pointer to the 
     * location to fill in the CnS parameter field 
     */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    swi_uint64 iScr;

    iScr = slatol64((const char *)inparamp);

    if (0 == iScr)
        return SWI_RCODE_FAILED;

    swi_uint64 T1 = ((iScr/iScd)-1);
    swi_uint64 iSrd = T1*iRandom;

    pszTemp = (swi_charp)sluint64toasciidec(szTemp, (swi_uint32)(iSrd >> 32), 
            (swi_uint32)(iSrd & 0xffffffff)); /* 4 digits max*/
    
    nNum = pszTemp-szTemp;
    if(nNum > LENGTH_SecureRebuttal)
    {
        return SWI_RCODE_INVALID_PAR;
    }

    slmemset((char *)nTemp, '0', LENGTH_SecureRebuttal-nNum);
    slmemcpy(nTemp+LENGTH_SecureRebuttal-nNum, szTemp, nNum);

    /* Pack the CnS Parameter field with the input parameters */
    parmlength = SwiPackU8s( outparamp, nTemp, LENGTH_SecureRebuttal );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_SECURE_REBUTTAL, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    if (SWI_RCODE_OK == resultcode)  /* Success */
    {
        swi_uint16 result;
        
        result = piget16(&inparamp);

        switch(result)
        {
        case 1:
        case 2:
        case 3:
            break;  /* Do nothing. */
            
        default:
            resultcode = SWI_RCODE_FAILED;
            break;
        }
    }

    return resultcode;
}

/**
 *
 * This function sets the MIN string for the specified NAM account profile.
 *
 * The MIN is a string of the NMSI digits of the CDMA IMSI (refer to IS-95B). 
 * The MIN must be from 1 to 10 bytes long. The modem will pad with zeroes 
 * for MINs less than 10 bytes. The balance of the IMSI is pre-programmed at 
 * the factory for the particular service provider.
 *
 * The data is not committed until SwiPstLock is called. 
 * See "Activation process" in documentation for a description of the 
 * activation process.
 *
 * \param namIndex[IN]     Index (base 0) of the NAM account to set. 
 *                         See SwiGetNamInfo.
 * \param pszMIN[IN]       MIN to store.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetIMSI(swi_uint16 namIndex, 
                             const swi_charp pszMIN, 
                             swi_uint32 timeout)
{
    return SwiIntSetU16sU16LenString(CNS_IMSI_CDMA, &namIndex, 1, pszMIN, 
            LENGTH_Min, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function closes the activation process, either cancelling the changes 
 * or committing them to storage. 
 *
 * The modem must have been unlocked via SwiPstUnlock prior to this command.
 *
 * In the event of a failure, the entire process of activation should be 
 * repeated. The failure mask reports all known reasons why a commit failed. 
 * See Activation process in the API Reference for a description of the 
 * complete activation process.
 *
 * \param Type[IN]          0 = cancel changes; 1 = commit changes
 * \param Nam[IN]           Index (base 0) of the NAM account profile to 
 *                          commit or cancel. See SwiGetNamInfo.
 * \param commitResult[OUT] 0 = Fail - see remarks; 1 = Success
 * \param failureMask[OUT]  Bit-mapped failure reason, 
 *                          see SWI_TYPE_Commit_ErrorMask for possible values
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstLock(swi_uint16 Type, 
                             swi_uint16 Nam, 
                             swi_uint16* commitResult, 
                             swi_uint16* failureMask, 
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
    parmlength = SwiPackU16U16( outparamp, Type, Nam );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_SECURE_COMMIT_UPDATE, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        *commitResult = piget16(&inparamp);
        *failureMask = piget16(&inparamp);
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function resets the modem.
 *
 * This is typically done after modifying particular modem settings that 
 * require a reset for the new settings to take effect.
 *
 * During the period in which the modem resets and re-initializes, the modem 
 * will not respond to any API requests.  
 *
 * After the reset, any previously enabled notifications must be re-enabled, 
 * and any previous modem states and/or information may no longer be valid.
 *
 * Typical scenarios requiring a modem reset include:
 *    - Modem activation
 *    - Modifying system select setting (SwiPstSetSystemPref)
 *    - Changing the current active NAM (SwiPstSetCurrentNam)
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiResetModem()
{
    return SwiIntSet(CNS_RESET_DEVICE_LEGACY, SWI_AMCDMA, 
            APTIMEOUT);
}

/**
 *
 * This function This function set the modem to offline mode.
 *
 * This is typically done before saving data to the modem and then reset the 
 * modem with SwiResetModem. The benefit to bring the modem to offline mode 
 * is to limit modem's operation during this mode.
 *
 * Typical scenarios requiring a modem reset include:
 *     - Modem activation
 *     - Modifying system select setting (SwiPstSetSystemPref)
 *     - Changing the current active NAM (SwiPstSetCurrentNam)
 *
 * During this mode, the modem might error out on certain operations like, 
 * voice/data/SMS related call.
 *
 * SWI_NOTIFY_Power[_Power.State == SWI_Type_PowerOffline] notification will 
 * be sent when the modem completes the offline. See SWI_NOTIFY_Power for 
 * details.
 *
 * To exit this mode, application needs to call SwiResetModem.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetModemOffline()
{
    /* This is not implemented by either EVDO SDK or SDK7, 
        but is called by SwiSetPower(). Just return OK as place holder. */
    return SWI_RCODE_NOT_SUP;
}

/**
 *
 * This function reports the state of the user security lock.
 *
 * \param pLockStatus[OUT] The status of the user security lock:
 *                         0 = not locked; 1 = locked
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetLockStatus(swi_uint16* pLockStatus, swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_LOCK_STATUS, pLockStatus, SWI_AMCDMA, timeout);
}

/**
 *
 * This function reports the state of the autolock feature.
 *
 * The setting of autolock is controlled with SwiSetAutoLock.
 *
 * \param autoLock[OUT]    The status of the autolock feature:
 *                         0 = disabled; 1 = enabled
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetAutoLock(swi_uint16* autoLock, swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_AUTO_LOCK, autoLock, SWI_AMCDMA, timeout);
}

/**
 *
 * This function This function sets the state of the automatic user security 
 * lock on the modem.
 *
 * If you enable the autolock, the lock will be on the next time the modem is 
 * started (reset or power cycled). The state of the current autolock setting 
 * can be read using SwiGetAutoLock.
 *
 * A locked modem is unlocked with SwiLockModem.
 *
 * \param autoLock[IN]     The autolock state setting:
 *                         0 = Off; !0 = Enabled
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetAutoLock(swi_uint16 autoLock, swi_uint32 timeout)
{
    return SwiIntSetU16(CNS_AUTO_LOCK,  autoLock, NULL, SWI_AMCDMA, timeout);
}

/**
 *
 * This function toggles the state of the user security lock.
 *
 * \param Password[IN]     The user’s passcode to contol the modem lock.
 * \param pResponse[OUT]   The modem’s response to the password provided:
 *                         0 = passcode rejected; 1 = accepted - state toggled
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiLockModem(const swi_charp Password, 
                               swi_uint16* pResponse, 
                               swi_uint32 timeout)
{
    return SwiIntSetString(CNS_LOCK, Password, pResponse, 0, 
            LENGTH_PinPassword, SWI_AMCDMA, timeout);
}

/**
 *
 * This function changes the user’s passcode (PIN).
 *
 * The return value: SWI_RCODE_REQUEST_TIMEOUT requires special care. If the 
 * OldPassword was accepted, the new setting will be made.  A subsequent 
 * retry will report an invalid password (OldPassword) because the new 
 * password is in effect. On the other hand, if the OldPassword was rejected, 
 * the settings are unchanged. A retry will report an invalid password, 
 * because the user failed to match. Both cases give exactly the same result 
 * on a retry!  The recommended course of action is to have the application 
 * silently retry the command using the NewPassword value in both the old and 
 * new parameters. If the modem accepts this, then the change is complete. 
 * If the modem rejects it, then OldPassword was likely rejected in the 
 * orginal call and the user will have to start the process over.
 *
 * \param OldPassword[IN]  The original passcode; to permit the user to make 
 *                         the change.
 * \param NewPassword[IN]  The new setting for the passcode.
 * \param pResponse[OUT]   The result of the requested change:
 *                         0 = change rejected; 1 = change accepted
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiChangeLockPIN(const swi_charp OldPassword, 
                                   const swi_charp NewPassword, 
                                   swi_uint16* pResponse, 
                                   swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    if(slstrlen (OldPassword) != LENGTH_PinPassword || 
            slstrlen (NewPassword) != LENGTH_PinPassword)
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
    parmlength = SwiPackChangeLockPIN( outparamp, OldPassword, NewPassword );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_CHANGE_LOCK_PIN, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */
                     
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        *pResponse = piget16( &inparamp );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function gets the ERI version stored in the modem.
 *
 * \param pszERIFilename[IN] ERI Filename stored in modem.
 * \param pnERIVer[IN]       ERI File Version.
 * \param timeout[IN]        Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetERIVer(const swi_charp pszERIFilename, 
                               swi_uint16* pnERIVer, 
                               swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    SWI_STRUCT_EFileInfo sEf;
    swi_uint8           Buffer[MAX_EFileBlock];
    swi_uint32          nBlockSize;
    
    sEf.sizeStruct = sizeof(SWI_STRUCT_EFileInfo);

    resultcode = SwiGetEFileInfo(pszERIFilename, &sEf, timeout);
    if( resultcode == SWI_RCODE_OK )
    {
        if (1 != sEf.nResult) 
        {
            resultcode = SWI_RCODE_NOT_INIT;
        }
        else if(0 == sEf.nBlocks)
        {
            resultcode = SWI_RCODE_NOT_INIT;
        }
        else
        {
            resultcode = SwiGetEFileBlock(pszERIFilename, 0, Buffer, 
                    MAX_EFileBlock, &nBlockSize, timeout);
            /* Only parse out the response data if we got a positive return */
            if( resultcode == SWI_RCODE_OK )
            {
                *pnERIVer = ((swi_uint16)(((swi_uint8)(Buffer[1] & 0xff)) | 
                        ((swi_uint16)((swi_uint8)(Buffer[0] & 0xff))) << 8));
            }
        }
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function forces modem out of CDMA deep sleep.
 *
 * Typically prior to dialing a number using the dialpad, using the phone 
 * book or when RAS dialing, this function should be called to wake up the 
 * modem if it is in CDMA power save mode.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPowerSaveExit()
{
    return SwiIntSet(CNS_POWERSAVE_EXIT, SWI_AMCDMA, APTIMEOUT);
}

/**
 *
 * This function gets the Access Overload Class setting.
 *
 * \param Nam[IN]          NAM index.
 * \param Accolc[OUT]      Accolc setting
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetAccolc(swi_uint16 Nam, 
                                  swi_uint16 *Accolc, 
                                  swi_uint32 timeout)
{
    return SwiIntGetNvNamU16(CNS_PST_NV, NV_ACCOLC, Nam, Accolc, SWI_AMCDMA, 
            timeout);
}

/**
 *
 * This function Sets the Access Overload Class value.
 *
 * \param Nam[IN]          NAM index.
 * \param Accolc[IN]       Accolc setting
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetAccolc(swi_uint16 Nam, 
                                  swi_uint16 Accolc, 
                                  swi_uint32 timeout)
{
    return SwiIntSetNvNamU16(CNS_PST_NV, NV_ACCOLC, Nam, Accolc, SWI_AMCDMA, 
            timeout);
}

/**
 *
 * This function gets the Slot Cycle Index value.
 *
 * \param Sci[OUT]         The Slot cycle index setting.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetSci(swi_uint16 *Sci, swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_NV, NV_SCI, Sci, SWI_AMCDMA, timeout);
}

/**
 *
 * This function upload the PRL binary data to the modem.
 *
 * If this API returns error then call it one more time with the same 
 * parameters. Before calling this API, make sure that the modem is SPC 
 * (or MSL) unlocked. After calling this API, whether the operation is 
 * successfull or not, should call SwiResetModem
 *
 * \param NamIndex[IN]     NAM index.
 * \param pPRLBuffer[IN]   Pointer to the entire binary data of PRL to upload.
 * \param nPRLBufLen[IN]   Size of the binary data.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetPRL(swi_uint16 NamIndex, 
                            swi_uint8* pPRLBuffer, 
                            swi_uint16 nPRLBufLen, 
                            swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    int                 iCNSBlockSize;

    /* Create a file with the specified file name and size of file does not 
    exist. Otherwise, will update the file size of the existing file to the 
    specified size. 
    */
    resultcode = SwiSetPRLSize(NamIndex, nPRLBufLen, timeout);
    if (SWI_RCODE_OK != resultcode)
    {
        return resultcode;
    }
    
    iCNSBlockSize = CNS_MAX_DATA_BLOCK_SIZE;
    
    if ( nPRLBufLen > iCNSBlockSize )
    {
        int cntBlocks = nPRLBufLen/iCNSBlockSize;

        /* send intermediate blocks */
        int i;
        for ( i = 0; i < cntBlocks; i++)
        {
            SwiIntSendPRLBlock(NamIndex, (swi_uint8)(i), 
                    (swi_uint16) iCNSBlockSize, pPRLBuffer, timeout);
            pPRLBuffer += iCNSBlockSize;
            
        }
        /* Write the last block */
        resultcode = SwiIntSendPRLBlock(NamIndex, (swi_uint8)(i), 
                (swi_uint16) (nPRLBufLen - (i)*iCNSBlockSize), 
                pPRLBuffer, timeout);
    }
    else
    {
        /* send the only one data block */
        resultcode = SwiIntSendPRLBlock(NamIndex, (swi_uint8)(0), nPRLBufLen, 
                pPRLBuffer, timeout);
    }

    return resultcode;
}

/**
 *
 * This function upload the PRL binary data to the modem.
 * If this API returns error then call it one more time with the same 
 * parameters. This API differs from SwiSetPRL in that this call does not 
 * need modem to be SPC (or MSL) unlocked. After calling this API, whether 
 * the operation is successfull or not, should call SwiResetModem
 *
 * \param NamIndex[IN]     NAM index.
 * \param pPRLBuffer[IN]   Pointer to the entire binary data of PRL to upload.
 * \param nPRLBufLen[IN]   Size of the binary data.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetPRLEx(swi_uint16 NamIndex, 
                              swi_uint8* pPRLBuffer, 
                              swi_uint16 nPRLBufLen, 
                              swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    SWI_STRUCT_ApiUnlock sUnlockParms;
    
    slmemset((char *)&sUnlockParms, 0, sizeof(SWI_STRUCT_ApiUnlock));
    sUnlockParms.sizeStruct = sizeof(SWI_STRUCT_ApiUnlock);
    
    sUnlockParms.nLevel = API_UNLOCK_OEM;  
    sUnlockParms.nPasswordLen = SWI_API_UNLOCK_PWD_LEN;
    sUnlockParms.szPassword[0] = 0x53;  /* 'S' */
    sUnlockParms.szPassword[1] = 0x57;  /* 'W' */
    sUnlockParms.szPassword[2] = 0x49;  /* 'I' */
    sUnlockParms.szPassword[3] = 0x20;  /* ' ' */
    sUnlockParms.szPassword[4] = 0x4C;  /* 'L' */
    sUnlockParms.szPassword[5] = 0x4F;  /* 'O' */
    sUnlockParms.szPassword[6] = 0x57;  /* 'W' */

    resultcode = SwiApiUnlock(&sUnlockParms, timeout);
    if( resultcode == SWI_RCODE_OK )
    {
        if(sUnlockParms.nResult == API_UNLOCK_SUCCESS)
        {
            resultcode = SwiSetPRL(NamIndex, pPRLBuffer, nPRLBufLen, timeout);
        }
        else
        {
            resultcode = SWI_RCODE_FAILED;
        }
    }

    return resultcode;
}

/**
 *
 * This function write the ERI data to the modem.
 *
 * Before calling this API, make sure that the modem is SPC (or MSL) unlocked. 
 * After calling this API, whether the operation is successfull or not, should 
 * call SwiResetModem
 *
 * \param pERIBuffer[IN]   Pointer to the entire ERI data buffer.
 * \param nERIBufLen[IN]   Size of the ERI data.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetERI(swi_uint8* pERIBuffer, 
                            swi_uint16 nERIBufLen, 
                            swi_uint32 timeout)
{
    return SwiSetEFile(G_SZ_FNERINAM1, pERIBuffer, nERIBufLen, timeout);
}

/**
 *
 * This function returns the Devices Life Timer value
 *
 * \param pTotalUpTime[OUT] Device Life Timer value.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetTotalUpTime(swi_uint32* pTotalUpTime, 
                                    swi_uint32 timeout)
{
    return SwiIntGetU32(CNS_TOTAL_UP_TIME, pTotalUpTime, SWI_AMCDMA, timeout);
}

/**
 *
 * This function retrieves 1st part of IMSI
 *
 * Use SwiPstGetImsiMS2 to retrieve 2nd part. Both IMSI parts must be 
 * retrieved and decoded for complete IMSI display.
 *
 * \param Nam[IN]          NAM index.
 * \param IMSI[OUT]        Encoded IMSI value.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetImsiMS1(swi_uint16 Nam, 
                                   swi_uint32 *IMSI, 
                                   swi_uint32 timeout)
{
    return SwiIntGetNvNamU32(CNS_PST_NV, NV_MIN1, Nam, IMSI, SWI_AMCDMA, 
            timeout);
}

/**
 *
 * This function sets 1st part of IMSI.
 *
 * IMSI must be encoded before writing to modem.
 *
 * \param Nam[IN]          NAM index.
 * \param Imsi_MS1[IN]     encoded IMSI value.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetImsiMS1(swi_uint16 Nam, 
                                   swi_uint32 Imsi_MS1, 
                                   swi_uint32 timeout)
{
    return SwiIntSetNvNamU32(CNS_PST_NV, NV_MIN1, Nam, Imsi_MS1, SWI_AMCDMA, 
            timeout);
}

/**
 *
 * This function retrieves 2st part of IMSI.
 *
 * Use SwiPstGetImsiMS1 to retrieve 1st part. Both IMSI parts must be 
 * retrieved and decoded for complete IMSI display.
 *
 * \param Nam[IN]          NAM index.
 * \param Imsi_MS2[OUT]    Encoded IMSI value.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetImsiMS2(swi_uint16 Nam, 
                                   swi_uint16 *Imsi_MS2, 
                                   swi_uint32 timeout)
{
    return SwiIntGetNvNamU16(CNS_PST_NV, NV_MIN2, Nam, Imsi_MS2, SWI_AMCDMA, 
            timeout);
}

/**
 *
 * This function sets 2nd part of IMSI.
 *
 * IMSI must be encoded before writing to modem.
 *
 * \param Nam[IN]          NAM index.
 * \param Imsi_MS2[IN]     Encoded IMSI value.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetImsiMS2(swi_uint16 Nam, 
                                   swi_uint16 Imsi_MS2, 
                                   swi_uint32 timeout)
{
    return SwiIntSetNvNamU16(CNS_PST_NV, NV_MIN2, Nam, Imsi_MS2, SWI_AMCDMA, 
            timeout);
}

/**
 *
 * This function retrieves NAM name.
 *
 * \param Nam[IN]          NAM index.
 * \param pszNamName[OUT]  Nam Name String.
 * \param length[IN]       Length of string buffer passed in.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetNamName(swi_uint16 Nam, 
                                   swi_charp pszNamName, 
                                   swi_uint32 length, 
                                   swi_uint32 timeout)
{
    return SwiIntGetNvNamString(CNS_PST_NV, NV_NAME_NAM_I, Nam, pszNamName, 
            length, 0, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets the NAM name
 *
 * \param Nam[IN]          NAM index.
 * \param pszNamName[IN]   Nam Name String.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetNamName(swi_uint16 Nam, 
                                   const swi_charp pszNamName, 
                                   swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint32          szLength;  /* Length of string */ 
    
    szLength = slstrlen(pszNamName);
    if(szLength < LENGTH_NvNameNam)
    {
        swi_char              NamName[LENGTH_NvNameNam+1];
        
        /* Pad the string with spaces */
        slstrncpy(NamName, pszNamName, szLength);
        slmemset((char *)&NamName[szLength], ' ', LENGTH_NvNameNam-szLength);
        NamName[LENGTH_NvNameNam] = EOS;
        resultcode =  SwiIntSetNvNamString(CNS_PST_NV, NV_NAME_NAM_I, Nam, 
                NamName, 0, LENGTH_NvNameNam, SWI_AMCDMA, timeout);
    }
    else
    {
        resultcode =  SwiIntSetNvNamString(CNS_PST_NV, NV_NAME_NAM_I, Nam, 
                pszNamName, 0, LENGTH_NvNameNam, SWI_AMCDMA, timeout);
    }

    return resultcode;
    
}

/**
 *
 * This function gets Mobile Country Code (MCC).
 *
 * Value must be decoded for proper display.
 *
 * \param Nam[IN]          NAM index.
 * \param Mcc[OUT]         Encoded MCC
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetImsiMcc(swi_uint16 Nam, 
                                   swi_uint16 *Mcc, 
                                   swi_uint32 timeout)
{
    return SwiIntGetNvNamU16(CNS_PST_NV, NV_IMSI_MCC, Nam, Mcc, SWI_AMCDMA, 
            timeout);
}

/**
 *
 * This function sets the Mobile Country Code (MCC).
 *
 * \param Nam[IN]          NAM index.
 * \param Mcc[IN]          Encoded MCC
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetImsiMcc(swi_uint16 Nam, 
                                   swi_uint16 Mcc, 
                                   swi_uint32 timeout)
{
    return SwiIntSetNvNamU16(CNS_PST_NV, NV_IMSI_MCC, Nam, Mcc, SWI_AMCDMA, 
            timeout);
}

/**
 *
 * This function gets IMSI_1112.
 *
 * Value must be decoded for proper display.
 *
 * \param Nam[IN]          NAM index.
 * \param Digits1112[OUT]  Encoded IMSI_1112
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetImsi1112(swi_uint16 Nam, 
                                    swi_uint16 *Digits1112, 
                                    swi_uint32 timeout)
{
    return SwiIntGetNvNamU16(CNS_PST_NV, NV_IMSI_11_12, Nam, Digits1112, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets IMSI_1112
 *
 * Value must be encoded before written to modem.
 *
 * \param Nam[IN]          NAM index.
 * \param Digits1112[IN]   Encoded IMSI_1112
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetImsi1112(swi_uint16 Nam, 
                                    swi_uint16 Digits1112, 
                                    swi_uint32 timeout)
{
    return SwiIntSetNvNamU16(CNS_PST_NV, NV_IMSI_11_12, Nam, Digits1112, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets the 20 Digit AKey.
 *
 * AKey Checksum is managed internally by the API. Modem must be MSL unlocked 
 * to write this item.  Modem should be relocked after written.
 *
 * \param szAKey[IN]       AKey to be written to modem.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetAKey(swi_charp szAKey, swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp;/* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    swi_uint32         szLength;   /* Length of string */ 
    swi_bool           isValidatingChkSum;
    swi_uint8 AKeyChecksum[LENGTH_AKeyChecksum];

    szLength = slstrlen(szAKey);
    if (szLength == LENGTH_AKey)
    {
        /* old technic: blindly write AKEY + CHKSUM generated by the modem */
        isValidatingChkSum = FALSE;
    }
    else if (szLength == LENGTH_AKeyChecksum)
    {
        /* new technic where CHKSUM is also part of the AKEY */
        isValidatingChkSum = TRUE;
    }
    else
    {
        /* Invalid AkeyLen */
        return SWI_RCODE_INVALID_PAR;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Parameter Field Construction Zone - get a pointer to the 
     * location to fill in the CnS parameter field 
     */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Pack the CnS Parameter field with the input parameters */
    parmlength = SwiPackU8s( outparamp, (const swi_uint8 *)szAKey,
                             LENGTH_AKey );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_AKEY, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    if (SWI_RCODE_OK != resultcode)
    {
        return resultcode;
    }

    if(TRUE == isValidatingChkSum)
    {
        /* amcnslength == */
        if(slmemcmp(inparamp, &szAKey[LENGTH_AKey], 
                LENGTH_AKeyChecksum-LENGTH_AKey) != 0)
        {
            return SWI_RCODE_INVALID_PAR;
        }
        slmemcpy(AKeyChecksum, szAKey, LENGTH_AKeyChecksum);
    }
    else
    {
        slmemcpy(AKeyChecksum, szAKey, LENGTH_AKey);
        slmemcpy(&AKeyChecksum[LENGTH_AKey], inparamp, 
                LENGTH_AKeyChecksum-LENGTH_AKey);
    }

    return SwiIntSetU8s(CNS_AKEY_CHKSUM, AKeyChecksum, LENGTH_AKeyChecksum, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function performs a RTN or PRI reset on the device.
 *
 * This will reset the device to factory state. Note that this action may 
 * take several moments.
 *
 * \param nType[IN]        Type of reset (PRI_RESET = 1,RTN_RESET = 2)
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiRtnReset(swi_uint8 nType, swi_uint32 timeout)
{
    return SwiIntSetU16(CNS_DFLT_NV,  nType, NULL, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets the device HDR MAC index value.
 *
 * \param pTechHdrMacIndex[OUT] Mac Index. (see SWI_STRUCT_Tech_Hdr_Mac_Index)
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetTechHdrMacIndex(
        SWI_STRUCT_Tech_Hdr_Mac_Index *pTechHdrMacIndex, swi_uint32 timeout)
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
                                  CNS_TECH_HDR_MAC_INDEX, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &paramlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackTechHdrMacIndex( inparamp, pTechHdrMacIndex );
    }
    
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function gets the device's LED enable state.
 *
 * This functionality is not support on all devices.  A successful return 
 * indicates the functionality exists and the current setting of the LED.
 *
 * \param pbEnable[OUT]  Current state of LED setting (0=disabled, 1=enabled).
 * \param timeout[IN]    Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetLEDEnable(swi_bool *pbEnable, swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint16 nValue;

    resultcode = SwiIntGetNvU16(CNS_PST_NV, NV_SWI_LED_ENABLE_I, &nValue, 
            SWI_AMCDMA, timeout);
    if(SWI_RCODE_OK == resultcode )
    {
        *pbEnable = (swi_bool)nValue;
    }
    return resultcode;
}

/**
 *
 * This function sets the device's LED enable state.
 *
 * This functionality is not support on all devices.  A successful return 
 * indicates the functionality exists and the LED setting was set.  To 
 * determine if feature is supported, use SwiGetLEDEnable().
 *
 * \param bEnable[IN]      LED setting (0=disabled, 1=enabled)
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetLEDEnable(swi_bool bEnable, swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_NV, NV_SWI_LED_ENABLE_I, bEnable, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets the device's LED enable state.
 *
 * This API extends SwiGetLEDEnable by adding a bit mask for indicating LED 
 * groups (ie. pwr/signal and glowtop).
 *
 * This functionality is not support on all devices. Check 
 * SwiGetGeneralFeatureSupport() for SWI_GEN_FEATURE_ENH_LED to determine if 
 * enhanced LED support is available.  If so, this API must be used in place 
 * of SwiGetLEDEnable().
 *
 * \param pLED[IN]        LED enable state returned from modem.
 * \param timeout[IN]     Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetLEDEnableEx(SWI_STRUCT_LED_CFG_ENABLE *pLED, 
                                    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    SWI_STRUCT_LED_CFG_TBL sLCT;
    swi_uint16 nIdx;

    nIdx = 0;
    while(nIdx < SWI_MAX_LEDS && resultcode == SWI_RCODE_OK)
    {
        resultcode = SwiIntGetNvNamU8s(CNS_PST_NV, NV_SWI_LED_CONFIG_TABLE_I, 
                nIdx, (swi_uint8*)&sLCT, sizeof(sLCT), SWI_AMCDMA, timeout);
        if(resultcode == SWI_RCODE_OK)
        {
            pLED->nType[nIdx] = sLCT.led_type;
        }
        nIdx++;
    }
    
    if(resultcode == SWI_RCODE_OK)
    {
        /* now get ENABLE table */
        swi_uint8 bTable[SWI_MAX_LEDS];
        resultcode = SwiIntGetNvU8s(CNS_PST_NV, NV_SWI_LED_ENABLE_TABLE_I, 
                (swi_uint8*)&bTable, SWI_MAX_LEDS, SWI_AMCDMA, timeout);
      
        if(resultcode == SWI_RCODE_OK)
        {
            for(nIdx = 0; nIdx < SWI_MAX_LEDS; nIdx++)
            {
                pLED->bEnable[nIdx] = bTable[nIdx];
            }
        }
        else
        {
            /* Assume all LED on in this case 
                        (FW NV item may not be properly configured) */
            for(nIdx = 0; nIdx < SWI_MAX_LEDS; nIdx++)
            {
                pLED->bEnable[nIdx] = SWI_LED_ENABLED;
            }
        }
    }
    
    return resultcode;
}

/**
 *
 * This function sets the device's LED enable states.
 *
 * This API extends SwiSetLEDEnable by adding a bit mask to control LED groups.
 *
 * This functionality is not support on all devices.  A successful return 
 * indicates the functionality exists and the LED setting was set.  To 
 * determine if feature is supported, check for SWI_GEN_FEATURE_ENH_LED using 
 * SwiGetGeneralFeatureSupport(). If supported, this API must be used in place 
 * of SwiSetLEDEnable().
 *
 * \param pLED[IN]         Table of LED settings. 0 = disable, 1 = enable.
 *                         Note: Type field is not used.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetLEDEnableEx(const SWI_STRUCT_LED_CFG_ENABLE *pLED, 
                                    swi_uint32 timeout)
{
    swi_uint8 bTable[SWI_MAX_LEDS];
    swi_uint16 nIdx;
    
    for(nIdx = 0; nIdx < SWI_MAX_LEDS; nIdx++)
    {
      bTable[nIdx] = pLED->bEnable[nIdx];
    }

    return SwiIntSetNvU8s(CNS_PST_NV, NV_SWI_LED_ENABLE_TABLE_I, 
            (swi_uint8*)&bTable, SWI_MAX_LEDS, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets the device's General Feature support capabilities.
 *
 * See SWI_TYPE_General_Features for mask definitions.
 *
 * \param pMask[OUT]       Bitmask of capability settings
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetGeneralFeatureSupport(swi_uint32 *pMask, 
                                              swi_uint32 timeout)
{
    return SwiIntGetNvU32(CNS_PST_NV, NV_SWI_GENERAL_FEATURE_SUPPORT_I, pMask, 
            SWI_AMCDMA, timeout);
}

static swi_bool gnDeviceEVDOSettingNew = FALSE;

/**
 *
 * This function gets device CDMA setting.
 *
 * \param pwMask[OUT]  Pointer to save mask retrived for device CDMA setting.
 *                     0: CDMA Rev 0; 1: CDMA Rev A.
 * \param timeout[IN]  Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetDeviceEVDOSetting(swi_uint16 *pwMask, 
                                          swi_uint32 timeout)
{
    SWI_RCODE  resultcode; /* Result of call to cnSendWait() */
    swi_uint16 nValue;

    resultcode = SwiIntGetNvU16(CNS_PST_NV, NV_HDRSCP_FORCE_REL0_CONFIG_I, 
            &nValue, SWI_AMCDMA, timeout);
    if(SWI_RCODE_OK == resultcode)
    {
        *pwMask = nValue;
        gnDeviceEVDOSettingNew = TRUE;
    }
    else
    {
        /* Just need a buffer large enough */
        CNS_HDRSCP_subtype_custom_config_type scp;
        
        resultcode = SwiIntGetNvU8s(CNS_PST_NV, 
                NV_HDRSCP_SUBTYPE_CUSTOM_CONFIG_I, (swi_uint8 *)&scp, 
                sizeof(CNS_HDRSCP_subtype_custom_config_type), SWI_AMCDMA, 
                timeout);
        if(SWI_RCODE_OK == resultcode)
        {
            swi_uint8 *inparamp;  /* ptr to param field rx'd from modem */

            /* Can not use structure member directory, since struct member 
               is not  always byte aligned. 
            */
            inparamp = (swi_uint8 *)&scp;
            
            piget16(&inparamp);    /* Discard custom_config_is_active*/
            piget32(&inparamp);    /* Discard custom_config_is_active*/
            *pwMask = (swi_uint16)((piget32(&inparamp) != 0) ? 
                    SWI_REV_A : SWI_REV_0);
            gnDeviceEVDOSettingNew = FALSE;
        }
    }
    
    return resultcode;
}

/**
 *
 * This function sets device CDMA setting
 *
 * \param wMask[IN]        Mask to be set for device CDMA setting.
 *                         0: CDMA Rev 0; 1: CDMA Rev A.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetDeviceEVDOSetting(swi_uint16 wMask, swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */

    if(TRUE == gnDeviceEVDOSettingNew)
    {
        resultcode = SwiIntSetNvU16(CNS_PST_NV, NV_HDRSCP_FORCE_REL0_CONFIG_I, 
                wMask, SWI_AMCDMA, timeout);
        if(SWI_RCODE_OK == resultcode)
        {
            return SwiIntSetNvU16(CNS_PST_NV, NV_HDRSCP_SESSION_STATUS_I, 0, 
                    SWI_AMCDMA, timeout);
        }
    }
    else
    {
        swi_uint32 nLength;
        
        /* Just need a buffer large enough */
        CNS_HDRSCP_subtype_custom_config_type scp;
        slmemset((char *)&scp, 0, sizeof(scp));

        nLength = SwiPackDeviceEVDOSetting((swi_uint8 *)&scp, wMask);
        
        return SwiIntSetNvU8s(CNS_PST_NV, NV_HDRSCP_SUBTYPE_CUSTOM_CONFIG_I, 
                (swi_uint8 *)&scp, nLength, SWI_AMCDMA, timeout);
    }
    
    return resultcode;
}

/**
 *
 * This function gets device's Station Class Mark
 *
 * \param pbSCM[OUT]       Pointer to data buffer.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetStationClassMark(swi_uint8 *pbSCM, swi_uint32 timeout)
{
    return SwiIntGetNvU8(CNS_PST_NV, NV_SCM_I, pbSCM, SWI_AMCDMA, timeout);
}


/**
 *
 * This function retrieves the modem's PRI version.
 *
 * \param pMajor[OUT]      PRI major version
 * \param pMinor[OUT]      PRI minor version
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetPRIVersion(swi_uint8 *pMajor, 
                                   swi_uint8 *pMinor, 
                                   swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          array[2];

    resultcode = SwiIntGetNvU8s(CNS_PST_NV, NV_PRI_VER, array, 2, SWI_AMCDMA, 
            timeout);
    if( SWI_RCODE_OK == resultcode )
    {
        *pMajor = array[0];
        *pMinor = array[1];
    }

    return resultcode;
}

/**
 *
 * This function gets the current port mapping 
 *
 * \param pnPortMapping[OUT]  Pointer pointing to current port mapping returned
 * \param timeout[IN]         Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetPortMapping(swi_uint8 * pnPortMapping, 
                                      swi_uint32 timeout)
{
    return SwiIntGetNvU8(CNS_PST_NV, NV_SIO_DEV_MAP_MENU_ITEM_I, 
            pnPortMapping, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets the port mapping
 *
 * \param nPortMapping[IN]   Port mapping to set.
 * \param timeout[IN]        Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetPortMapping(swi_uint8 nPortMapping, 
                                      swi_uint32 timeout)
{
    return SwiIntSetNvU8(CNS_PST_NV, NV_SIO_DEV_MAP_MENU_ITEM_I, 
            nPortMapping, SWI_AMCDMA, timeout);
}

/* 
 * $Log: SwiApiCdmaBasic.c,v $
 */

