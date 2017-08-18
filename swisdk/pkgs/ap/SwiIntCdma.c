/**
 *    $Id: SwiIntCdma.c,v 1.4 2010/08/16 18:18:50 mdejong Exp $
 *
 *    Name       : SwiIntCdma.c
 *    
 *    Purpose    : SWI CDMA specific internal APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#include "SwiIntCdma.h"
#include "SwiIntPkCdmaInternal.h"
#include "SwiIntUpkCdmaInternal.h"
#include "ap/apidefs.h"

static swi_bool gbUsesSCP = FALSE;

/**
 *
 * This function establishes a call with certain serive option.
 *
 * \param SOValue[IN]      Call service option. See SWI_TYPE_FlashContext.
 * \param szCallNumber[IN] Call Number.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetCallOriginateSoCmd(swi_uint16 SOValue, 
                                           const swi_charp szCallNumber, 
                                           swi_uint32 timeout)
{
    return SwiIntSetU16String(CNS_CALL_ORIGINATE_SO_CMD, SOValue, 
            szCallNumber, LENGTH_CallNumber, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function retrieves current call notification enable status.
 *
 * \param NotifStatus[OUT]  Current call notificaiton enable status.
 *                          0x0000 – Notification Off
 *                          0x0001 – Notification On
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetCallNotificationStatus(swi_uint16* NotifStatus, 
                                               swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_CALL_NOTIFICATION_STATUS, NotifStatus, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function retrieves extended caller ID of an incoming call
 *
 * \param szExtndedCallerID[OUT]    Extended caller ID retrieved.
 * \param iLength[IN]               Size of the buffer that contains extended 
 *                                  call ID.
 * \param timeout[IN]               Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetCallExtendedCallerID(swi_charp szExtndedCallerID, 
                                             swi_uint32 iLength, 
                                             swi_uint32 timeout)
{
    return SwiIntGetString(CNS_CALL_EXTENDED_CALLER_ID, szExtndedCallerID, 
            iLength, 0, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * Calling this function is the first step to unlock the modem and initiate 
 * a provisioning session.
 *
 * \param pszSCD[IN]        Secure Challenge Digits (SCD), Numeric alphabets.
 *
 * \sa SwiSetSecureRebuttal.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetSecureChallenge(const swi_charp pszSCD)
{
    return SwiIntSetString(CNS_SECURE_CHALLENGE, pszSCD, NULL, 0, 
            LENGTH_SecureChallenge, SWI_AMCDMA, APTIMEOUT);
}

/**
 *
 * Calling this function is the last step to unlock the modem and initiate 
 *
 * \param pszSecureRebuttal[IN] Secure Rebuttal Digits (SRD), Numeric 
 *                              alphabets.
 *
 * \sa SwiSetSecureChallenge.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetSecureRebuttal(const swi_charp pszSecureRebuttal)
{
    return SwiIntSetString(CNS_SECURE_REBUTTAL, pszSecureRebuttal, NULL, 0, 
            LENGTH_SecureRebuttal, SWI_AMCDMA, APTIMEOUT);
}

/**
 *
 * This function closes QPST/provisioning session.
 *
 * SwiSetSecureChallenge() and SwiSetSecureRebuttal() must must precede it. 
 *
 * \param commitType[IN]    CommitType, 
 *                          0 – Non-provisioning secure commit
 *                          1 – Provisioning Secure commit
 * \param namIndex[IN]      NAM index.
 * \param commitResult[OUT] Secure Commit Result.
 *                          0 – Fail 
 *                          1 – Succeed (All non-provisioning Commit return 
 *                              Succeed)
 * \param failureMask[OUT]  Provisioning Commit Failure Cause Mask.
 *                          0x0000 – No Error 
 *                          0x0001 – Mobile Directory Number Not Provisioned
 *                          0x0002 – IMSI Not Provisioned
 *                          0x0004 – No IP Type Network User ID /Password is 
 *                                   Provisioned
 *                          0x0008 - Provisioning date missing.
 *                          0x0010 – NAM profile is not provisioned in right 
 *                                   order.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetSecureCommitUpdate(swi_uint16 commitType, 
                                           swi_uint16 namIndex, 
                                           swi_uint16* commitResult, 
                                           swi_uint16* failureMask)
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
    parmlength = SwiPackU16U16( outparamp, commitType, namIndex );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_SECURE_COMMIT_UPDATE, 
                                  parmlength, 
                                  APTIMEOUT, 
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
 * This function gets the provisioning date.
 *
 * \param namIndex[IN]          NAM index.
 * \param provisoningDate[IN]   Date string with the following format YYYYMMDD.
 *                              Note that provisioning date will be all zeros 
 *                              if the particular NAM has never been 
 *                              provisioned.
 *                              YYYY – Numeric String containing the Year
 *                              MM – Numeric String containing the month
 *                              DD – Numeric String containing the Day.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetProvisioningDate(swi_uint16 namIndex, 
                                         const swi_charp provisoningDate)
{
    return SwiIntSetU16String(CNS_PROVISIONING_DATE, namIndex, 
            provisoningDate, 0, LENGTH_ProvisioningDate, SWI_AMCDMA, 
            APTIMEOUT);
}

/**
 *
 * This function obtains Channel acquisition state
 *
 * \param State[OUT]        Channel State.
 *                          0x0001 - Channel acquired
 *                          0x0005 - Scanning for channel
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetChannelStateEx(swi_uint16* State, swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_CHANNEL_STATE, State, SWI_AMCDMA, timeout);
}

/**
 *
 * This function returns CRC16 of predetermined NV objects grouped into four 
 * categories: normal, optional, MIP and final checksum.
 *
 * \param pPriChksum[OUT]   PRI checksum. 4 swi_uint16 vlaues.
 *                          pPriChksum[0] - Modem Configure Checksum Value 
 *                                          (those parameters that are only 
 *                                          changed in Modem Configuration 
 *                                          menu from Watcher).
 *                          pPriChksum[1] - Modem Options Checksum Value 
 *                                          (those parameters that could be 
 *                                          changed in the Option menu in 
 *                                          Watcher).
 *                          pPriChksum[2] – MIP Checksum Value (MIP related 
 *                                          parameters and profiles).
 *                          pPriChksum[3] – overall checksum of the items 
 *                                          above.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetPriChksum(swi_uint16* pPriChksum, swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_PRI_CHKSUM, pPriChksum, SWI_AMCDMA, timeout);
}

/**
 *
 * This function returns V42BIS Compression enabling status
 *
 * \param pV42BISEnableStatus[OUT]  V42BIS compression enabling status.
 *                                  0 - Enabled; 1 - disabled.
 * \param timeout[IN]               Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetV42BISEnableStatus(swi_uint16* pV42BISEnableStatus, 
                                           swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_V42BIS_ENABLED, pV42BISEnableStatus, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function indicates if authentication data is sent over access channel.
 *
 * \param pAuthenticationStatus[OUT]    Authentication status.
 *                                      0 - Authenticated; 
 *                                      1 - Not authenticated.
 * \param timeout[IN]                   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetAuthenticationStatus(
        swi_uint16* pAuthenticationStatus, 
        swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_AUTH_STATUS, pAuthenticationStatus, 
        SWI_AMCDMA, timeout);
}

/**
 *
 * This function returns the total cumulative call duration of the phone in 
 * seconds.
 *
 * \param pTotalCallDuration[OUT]   Total cumulative call duration of the 
 *                                  phone in seconds.
 * \param timeout[IN]               Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetTotalCallDuration(swi_uint32* pTotalCallDuration, 
                                          swi_uint32 timeout)
{
    return SwiIntGetU32(CNS_TOTAL_CALL_DURATION, pTotalCallDuration, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function returns the total cumulative byte counter of the phone.
 *
 * \param Sent[OUT]         Total cumulative bytes sent.
 * \param Received[OUT]     Total cumulative bytes received.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetCumulativeByteCounter(swi_uint32 *Sent, 
                                              swi_uint32 *Received, 
                                              swi_uint32 timeout)
{
    swi_uint32 array[2];
    SWI_RCODE resultcode;

    resultcode = SwiIntGetU32s(CNS_CALL_MB_COUNTER, array, 2, 
            SWI_AMCDMA, timeout);
    if(SWI_RCODE_OK == resultcode)
    {
        *Sent = array[0];
        *Received = array[1];
    }

    return resultcode;
}

/**
 *
 * This function resets the total cumulative byte counter to 0.
 *
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiResetCumulativeByteCounter(swi_uint32 timeout)
{
    return SwiIntSet(CNS_CALL_MB_COUNTER, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets the mode that whether DIAG or CnS packets through 
 * secondary port (debug port).  
 *
 * This object will only be useful when transmitting data with the debug port.
 *
 * \param pCnsModeStatus[OUT]   DIAG or CnS mode.
 *                              0x0000 – CAIT Mode (DIAG mode)
 *                              0x0001 – CNS Mode
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetCNSModeStatus(swi_uint16* pCnsModeStatus, 
                                      swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_CNS_MODE_STATUS, pCnsModeStatus, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function retrieves device debug status.
 *
 * \param pDebugStatus[OUT] Debug status.
 *                          0x0000 – Debug Off
 *                          0x0001 – Debug On (output of special Qualcomm 
 *  `                                debug messages (e.g. MSG_HIGH….) over 
 *                                   HIP)
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetDebugStatus(swi_uint16* pDebugStatus, 
                                    swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_DEBUG_STATUS, pDebugStatus, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function retrieves device debug message level.
 *
 * \param pDebugMsgLevel[OUT]   The level of debug messages that are sent 
 *                              out to the API.
 *                              0x0000 – Low Level
 *                              0x0001 – Medium Level
 *                              0x0002 – High level
 *                              0x0003 – Error Level
 *                              0x0004 – Fatal Level
 *                              0x00FF – No Message Level
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetDebugMsgLevel(swi_uint16* pDebugMsgLevel, 
                                      swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_DEBUG_MSG_LVL, pDebugMsgLevel, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets Speaker Codec gain.
 *
 * \param pSpeakerLevel[OUT]    Speaker Codec Gain. Range: 0x0000 – 0x0006.
 *                              0x0000           – factory default codec gain.
 *                              0x0001 to 0x0003 - speaker codec gain lower 
 *                                                 then factory default, 1 
 *                                                 being the lowest gain.
 *                              0x0004 to 0x0006 - speaker codec gain higher 
 *                                                 then factory default, 6 
 *                                                 being the highest gain.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetSpeakerLevel(swi_uint16* pSpeakerLevel, 
                                     swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_SPEAKER_LEVEL, pSpeakerLevel, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets microphone level.
 *
 * \param pMICLevel[OUT]    Microphone level. Range: 0x0000 – 0x0006.
 *                          0x0000           – factory default mic codec gain.
 *                          0x0001 to 0x0003 - mic codec gain lower then 
 *                                             factory default, 1 being the 
 *                                             lowest gain
 *                          0x0004 to 0x0006 - mic codec gain higher then 
 *                                             factory default, 6 being the 
 *                                             highest gain
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetMICLevel(swi_uint16* pMICLevel, swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_MIC_LEVEL, pMICLevel, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets DTMF level.
 *
 * \param pDTMFLevel[OUT]   DTMF leve.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetDTMFLevel(swi_uint16* pDTMFLevel, swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_DTMF_DURATION, pDTMFLevel, SWI_AMCDMA, timeout);
}

/**
 *
 * This function retrieves por number.
 *
 * \param pDiagPort[OUT]        Diag port.
 * \param pDataPort[OUT]        Data port.
 * \param pBluetoothPort[OUT]   Bluetooth port.
 * \param pMultimediaPort[OUT]  Multi-media port.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetPort(swi_uint8* pDiagPort, 
                             swi_uint8* pDataPort, 
                             swi_uint8* pBluetoothPort, 
                             swi_uint8* pMultimediaPort, 
                             swi_uint32 timeout)
{
    swi_uint8 array[4];
    SWI_RCODE rc;

    rc = SwiIntGetU8s(CNS_PORT, array, 4, SWI_AMCDMA, timeout);
    if(SWI_RCODE_OK == rc)
    {
        *pDiagPort = array[0];
        *pDataPort = array[1];
        *pBluetoothPort = array[2];
        *pMultimediaPort = array[3];
    }

    return rc;
}

/**
 *
 * This function gets the echo cancellation profile for the audio path during 
 * a voice call.
 *
 * \param pEchoCancelProfile[OUT]   Echo cancellation profile. 
 *                                  Range: 0x0000 to 0x0005
 *                                  0x0000: Factory Default profile (headset)
 *                                  0x0001: No Echo Cancellation
 *                                  0x0002: Handset Echo Cancellation
 *                                  0x0003: Headset Echo Cancellation
 *                                  0x0004: Acoustic Echo Cancellation (AEC)
 *                                  0x0005: Speaker-Phone Cancellation
 * \param timeout[IN]               Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetEchoCancelProfile(swi_uint16* pEchoCancelProfile, 
                                          swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_ECHO_CANCEL, pEchoCancelProfile, 
        SWI_AMCDMA, timeout);
}

/**
 *
 * This function writes Mobile Directory Number to divice.
 *
 * User needs to initiate the provisioning session through security 
 * challenge/rebuttal in order to write the phone number.
 *
 * \param PhoneNumber[IN]   Mobile Directory Number to write.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetPhoneNumber(const swi_charp PhoneNumber, 
                                    swi_uint32 timeout)
{
    return SwiIntSetString(CNS_PHONE_NUMBER, PhoneNumber, NULL, 
            LENGTH_CDMA_PhoneNumber, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets V42BIS Compression Enable or Disable.
 *
 * \param V42BISEnableStatus[IN]    V42BIS compression.
 *                                  0 - Enabled; 1 - Disabled.
 * \param timeout[IN]               Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetV42BISEnableStatus(swi_uint16 V42BISEnableStatus, 
                                           swi_uint32 timeout)
{
    return SwiIntSetU16(CNS_V42BIS_ENABLED, V42BISEnableStatus, NULL, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function resets byte counter to 0.
 *
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiResetByteCounter()
{
    return SwiIntSet(CNS_CALL_BYTE_CNT_RESET, SWI_AMCDMA, 
            APTIMEOUT);
}

/**
 *
 * This function set the mode that whether DIAG or CnS packets through
 * secondary port (debug port). 
 *
 * \param CnsModeStatus[IN] DIAG or CnS mode to set.
 *                          0x0000 – CAIT Mode (DIAG mode)
 *                          0x0001 – CNS Mode
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetCNSModeStatus(swi_uint16 CnsModeStatus, 
                                      swi_uint32 timeout)
{
    return SwiIntSetU16(CNS_CNS_MODE_STATUS, CnsModeStatus, NULL, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets device debug status.
 *
 * \param DebugStatus[IN]   Debug status to set.
 *                          0x0000 – Debug Off
 *                          0x0001 – Debug On (output of special Qualcomm 
 *  `                                debug messages (e.g. MSG_HIGH….) over 
 *                                   HIP)
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetDebugStatus(swi_uint16 DebugStatus, swi_uint32 timeout)
{
    return SwiIntSetU16(CNS_DEBUG_STATUS, DebugStatus, NULL, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets device debug message level.
 *
 * \param DebugMsgLevel[IN] The level of debug messages that are sent 
 *                          out to the API.
 *                          0x0000 – Low Level
 *                          0x0001 – Medium Level
 *                          0x0002 – High level
 *                          0x0003 – Error Level
 *                          0x0004 – Fatal Level
 *                          0x00FF – No Message Level
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetDebugMsgLevel(swi_uint16 DebugMsgLevel, 
                                      swi_uint32 timeout)
{
    return SwiIntSetU16(CNS_DEBUG_MSG_LVL, DebugMsgLevel, NULL, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets Speaker Codec gain.
 *
 * \param SpeakerLevel[IN]  Speaker Codec Gain to set. Range: 0x0000 – 0x0006.
 *                          0x0000           – factory default codec gain.
 *                          0x0001 to 0x0003 - speaker codec gain lower 
 *                                             then factory default, 1 
 *                                             being the lowest gain.
 *                          0x0004 to 0x0006 - speaker codec gain higher 
 *                                             then factory default, 6 
 *                                             being the highest gain.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetSpeakerLevel(swi_uint16 SpeakerLevel, 
                                     swi_uint32 timeout)
{
    return SwiIntSetU16(CNS_SPEAKER_LEVEL, SpeakerLevel, NULL, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets microphone level.
 *
 * \param MICLevel[IN]  Microphone level. Range: 0x0000 – 0x0006.
 *                      0x0000           – factory default mic codec gain.
 *                      0x0001 to 0x0003 - mic codec gain lower then 
 *                                         factory default, 1 being the 
 *                                         lowest gain
 *                      0x0004 to 0x0006 - mic codec gain higher then 
 *                                         factory default, 6 being the 
 *                                         highest gain
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetMICLevel(swi_uint16 MICLevel, swi_uint32 timeout)
{
    return SwiIntSetU16(CNS_MIC_LEVEL, MICLevel, NULL, SWI_AMCDMA, timeout);
}

/**
 *
 * This function Generate a burst of DTMF tones  as specified in an ASCII 
 * string of max length 32.
 *
 * On-duration (digit) and off-duration (interdigit) for the burst are set by 
 * CNS_DTMF_DURATION object. When sending a burst, it is not necessary to 
 * ‘play’ the burst at the earpiece. Instead, a short tone (200ms ?) can be 
 * played to indicate that the burst has been sent.
 *
 * \param DTMFBurst[IN] DTMF burst to set.
 *                      HS_POUND_K = 0x23, '#' key, ASCII '#'
 *                      HS_STAR_K = 0x2A,     '*' key, ASCII '*'
 *                      HS_0_K = 0x30,        '0' key, ASCII '0'
 *                      HS_1_K = 0x31,        '1' key, ASCII '1'
 *                      HS_2_K = 0x32,        '2' key, ASCII '2'
 *                      HS_3_K = 0x33,        '3' key, ASCII '3'
 *                      HS_4_K = 0x34,        '4' key, ASCII '4'
 *                      HS_5_K = 0x35,        '5' key, ASCII '5'
 *                      HS_6_K = 0x36,        '6' key, ASCII '6'
 *                      HS_7_K = 0x37,        '7' key, ASCII '7'
 *                      HS_8_K = 0x38,        '8' key, ASCII '8'
 *                      HS_9_K = 0x39,        '9' key, ASCII '9'
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetDTMFBurst(const swi_charp DTMFBurst, swi_uint32 timeout)
{
    return SwiIntSetString(CNS_DTMF_BURST, DTMFBurst, NULL, 
            LENGTH_DtmfBurstString, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets DTMF level.
 *
 * \param DTMFLevel[IN] DTMF leve to set.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetDTMFLevel(swi_uint16 DTMFLevel, swi_uint32 timeout)
{
    return SwiIntSetU16(CNS_DTMF_LEVEL, DTMFLevel, NULL, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets the echo cancellation profile for the audio path during 
 *
 * \param EchoCancelProfile[IN] Echo cancellation profile to set. 
 *                              Range: 0x0000 to 0x0005
 *                              0x0000: Factory Default profile (headset)
 *                              0x0001: No Echo Cancellation
 *                              0x0002: Handset Echo Cancellation
 *                              0x0003: Headset Echo Cancellation
 *                              0x0004: Acoustic Echo Cancellation (AEC)
 *                              0x0005: Speaker-Phone Cancellation
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetEchoCancelProfile(swi_uint16 EchoCancelProfile, 
                                          swi_uint32 timeout)
{
    return SwiIntSetU16(CNS_ECHO_CANCEL, EchoCancelProfile, NULL, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets port number.
 *
 * \param pDiagPort[IN]         Diag port.
 * \param pDataPort[IN]         Data port.
 * \param pBluetoothPort[IN]    Bluetooth port.
 * \param pMultimediaPort[IN]   Multi-media port.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetPort(swi_uint8 DiagPort, 
                             swi_uint8 DataPort, 
                             swi_uint8 BluetoothPort, 
                             swi_uint8 MultimediaPort, 
                             swi_uint32 timeout)
{
    swi_uint8 array[] = { DiagPort, DataPort, BluetoothPort, MultimediaPort };
    return SwiIntSetU8s(CNS_PORT, array, 4, SWI_AMCDMA, timeout);
}

/**
 *
 * This function selects current NAM
 *
 * \param namIndex[IN]  NAM index to select.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetSelectNam(swi_uint16 namIndex, swi_uint32 timeout)
{
    return SwiIntSetU16(CNS_SELECT_NAM, namIndex, NULL, SWI_AMCDMA, timeout);
}

/**
 *
 * This function read current NAM
 *
 * \param namIndex[OUT]     NAM index.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetSelectNam(swi_uint16* namIndex, swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_SELECT_NAM, namIndex, SWI_AMCDMA, timeout);
}

/**
 *
 * This function retrieves the current NAM 
 *
 * \param namIndex[OUT] NAM index.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetActiveNam(swi_uint16 *namIndex, swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_ACTIVE_NAM, namIndex, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets auto NAM
 *
 * \param bAutoNam[OUT] Auto NAM retrieved.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetAutoNam(swi_bool *bAutoNam, swi_uint32 timeout)
{
    SWI_RCODE resultcode;
    swi_uint16 nValue;
    
    resultcode = SwiIntGetU16(CNS_AUTO_NAM, &nValue, SWI_AMCDMA, timeout);
    if(SWI_RCODE_OK == resultcode)
    {
        *bAutoNam = (swi_bool)nValue;
    }

    return resultcode;
}

/**
 *
 * This function sets auto NAM
 *
 * \param bAutoNam[IN]  Auto NAM to set.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetAutoNam(swi_bool bAutoNam, swi_uint32 timeout)
{
    return SwiIntSetU16(CNS_AUTO_NAM, bAutoNam, NULL, SWI_AMCDMA, timeout);
}

/**
 *
 * This function signals the modem at the startup of the API
 *
 * \param bApiConnected[IN] Flag indicates API connect or disconnect.
 *                          0x0000 - API Disconnected.
 *                          0x0001 – API Connected.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetApiFirmwareHandshake(swi_bool bApiConnected, 
                                             swi_uint32 timeout)
{
    return SwiIntSetU16(CNS_API_FIRMWARE_HANDSHAKE, bApiConnected, NULL, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function obtain flag that indicates whether API has signaled the Modem.
 *
 * \param bApiConnected[OUT]    Flag indicates API connect or disconnect.
 *                              0x0000 - API Disconnected.
 *                              0x0001 – API Connected.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetApiFirmwareHandshake(swi_bool *bApiConnected, 
                                             swi_uint32 timeout)
{
    SWI_RCODE resultcode;
    swi_uint16 nValue;

    resultcode = SwiIntGetU16(CNS_API_FIRMWARE_HANDSHAKE, &nValue, 
            SWI_AMCDMA, timeout);
    if(SWI_RCODE_OK == resultcode)
    {
        *bApiConnected = (swi_bool)nValue;
    }

    return resultcode;
}

/**
 *
 * This function sets modem power down.
 *
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetPowerDown()
{
    return SwiIntSet(CNS_POWERDOWN, SWI_AMCDMA, APTIMEOUT);
}

/**
 *
 * This function turns radio on/off.
 *
 * \param bEnable[IN]      TRUE: turn on radio; FALSE: turn off radio.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetRadioEnable(swi_bool bEnable, swi_uint32 timeout)
{
    return SwiIntSetU16(CNS_RADIO_ENABLE, bEnable, NULL, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets the current band class.
 *
 * \param bandClass[OUT]    Band class, 
 *                          0x0000 – Cellular
 *                          0x0001 - PCS
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetTechBandClass(swi_uint16 *bandClass, 
                                      swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_TECH_BANDCLASS, bandClass, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets the registration zone ID.
 *
 * \param zoneID[OUT]   12- Bit Registration Zone ID.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetTechRegZone(swi_uint16 *zoneID, swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_TECH_REG_ZONE, zoneID, SWI_AMCDMA, timeout);
}

/**
 *
 * This function returns the PCS Block Value when the modem is in PCS coverage.
 *
 * \param pcs[OUT]          PCS Block.
 *                          0x0000 – No PCS Coverage
 *                          0x0001 – Block A
 *                          0x0002 – Block B
 *                          0x0003 – Block C
 *                          0x0004 – Block D
 *                          0x0005 – Block E
 *                          0x0006 – Block F
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetTechPCSBlock(swi_uint16 *pcs, swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_TECH_PCS_BLOCK, pcs, SWI_AMCDMA, timeout);
}

/**
 *
 * This function returns the channel for the current cellular coverage.
 *
 * \param channel[OUT]  Channel for the current cellular coverage.
 *                      0x0000- No Cell. Coverage
 *                      0x0001- Channel A
 *                      0x0002- Channel B 
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetTechCellChannel(swi_uint16 *channel, 
                                        swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_TECH_CELL_CHANNEL, channel, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets network user ID
 *
 * \param profileType[IN]   Profile type.
 * \param profileIndex[IN]  Profile index.
 * \param pszUserID[IN]     User ID to set.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetNetworkUserID(swi_uint16 profileType, 
                                      swi_uint16 profileIndex, 
                                      const swi_charp pszUserID, 
                                      swi_uint32 timeout)
{
    swi_uint16 array[] = { profileType, profileIndex };
    return SwiIntSetU16sU16LenString(CNS_USER_ID, array, 2, pszUserID, 
            LENGTH_UserID, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets network user ID
 *
 * \param profileType[IN]   Profile type.
 * \param profileIndex[IN]  Profile index.
 * \param pszUserID[OUT]    User ID to set.
 * \param length[IN]        Size of the buffer that contains the user ID.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetNetworkUserID(swi_uint16 profileType, 
                                      swi_uint16 profileIndex, 
                                      swi_charp pszUserID, 
                                      swi_uint32 length, 
                                      swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */
    swi_uint16 array[] = { profileType, profileIndex };

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Parameter Field Construction Zone - get a pointer to the 
     * location to fill in the CnS parameter field 
     */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Pack the CnS Parameter field with the input parameters */
    parmlength = SwiPackU16s(outparamp, array, sizeof(array)/sizeof(array[0]));
    
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_USER_ID, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackNetworkUserID( inparamp, pszUserID, length );
    }

    /* All done, send the result to the caller */
    return resultcode;
}
    
/**
 *
 * This function sets scp customer configuration.
 *
 * \param pSubtypeCustomConfig[IN]  scp customer configuration to set.
 *                              See SWI_STRUCT_HDRSCP_Subtype_Custom_Config.
 * \param timeout[IN]               Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetHdrScpCustomerConfig(
        const SWI_STRUCT_HDRSCP_Subtype_Custom_Config *pSubtypeCustomConfig, 
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
    parmlength = SwiPackHdrScpCustomerConfig( outparamp, 
            pSubtypeCustomConfig );
    
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
 * This function set network user password.
 *
 * \param profileType[IN]       Profile type.
 * \param profileIndex[IN]      Profile index.
 * \param pszUserPassword[IN]   User password to set.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetNetworkUserPassword(swi_uint16 profileType, 
                                            swi_uint16 profileIndex, 
                                            const swi_charp pszUserPassword, 
                                            swi_uint32 timeout)
{
    swi_uint16 array[] = { profileType, profileIndex };
    return SwiIntSetU16sU16LenString(CNS_USER_PASSWORD, array, 2, 
            pszUserPassword, LENGTH_UserPassword, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets MIP session password.
 *
 * \param profileIndex[IN]      Profile index.
 * \param pszUserPassword[IN]   User session password to set.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetMIPSessionPassword(swi_uint16 profileIndex, 
                                           const swi_charp pszUserPassword, 
                                           swi_uint32 timeout)
{
    return SwiIntSetU16String(CNS_MIP_SESSION_PW, profileIndex, 
            pszUserPassword, LENGTH_MIPSessionPwd, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets next MIP profile index.
 *
 * \param mipProfileIndex[OUT]  Next MIP profile index.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetNextMipProfileIndex(swi_uint16 *mipProfileIndex, 
                                            swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_MIP_PROFILE_INFO, mipProfileIndex, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets next SIP profile index.
 *
 * \param sipProfileIndex[OUT]  Next SIP profile index.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetNextSipProfileIndex(swi_uint16 *sipProfileIndex, 
                                            swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_SIP_PROFILE_INFO, sipProfileIndex, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets active profile.
 *
 * \param profileType[IN]   Profile type.
 * \param profileIndex[IN]  Profile index.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetActiveProfile(swi_uint16 profileType, 
                                      swi_uint16 profileIndex, 
                                      swi_uint32 timeout)
{
    swi_uint16 array[] = { profileType, profileIndex };
    return SwiIntSetU16s(CNS_ACTIVE_PROFILE, array, 2, NULL, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets active profile information.
 *
 * \param profileType[OUT]  Profile type.
 * \param profileIndex[OUT] Profile index.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetActiveProfile(swi_uint16 *profileType, 
                                      swi_uint16 *profileIndex, 
                                      swi_uint32 timeout)
{
    swi_uint16 array[2];
    SWI_RCODE rc;

    rc = SwiIntGetU16s(CNS_ACTIVE_PROFILE, array, 2, SWI_AMCDMA, timeout);
    if(SWI_RCODE_OK == rc)
    {
        *profileType = array[0];
        *profileIndex = array[1];
    }

    return rc;
}

/**
 *
 * This function gets HDR customer configuration information.
 *
 * \param pSubtypeCustomConfig[OUT] HDR customer configuration information.
 *                              See SWI_STRUCT_HDRSCP_Subtype_Custom_Config.
 * \param timeout[IN]               Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetHdrScpCustomerConfig(
        SWI_STRUCT_HDRSCP_Subtype_Custom_Config *pSubtypeCustomConfig, 
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
    parmlength = SwiPackNv( outparamp, NV_HDRSCP_SUBTYPE_CUSTOM_CONFIG_I);
    
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PST_NV, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackHdrScpCustomerConfig( inparamp, pSubtypeCustomConfig );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function gets HDR SCP session status.
 *
 * \param Nam[IN]               NAM index.
 * \param pSessionStatus[OUT]   HDR scp session status.
 *                              See SWI_STRUCT_HDRSCP_Session_Status.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetHdrScpSessionStatus(
        swi_uint16 Nam, 
        SWI_STRUCT_HDRSCP_Session_Status *pSessionStatus, 
        swi_uint32 timeout)
{
    return SwiIntGetNvNamU16(CNS_PST_NV, NV_HDRSCP_SESSION_STATUS_I, Nam, 
            &pSessionStatus->nSessionStatus, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets HDR SCP session status.
 *
 * \param Nam[IN]               NAM index.
 * \param pSessionStatus[IN]    HDR scp session status.
 *                              See SWI_STRUCT_HDRSCP_Session_Status.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetHdrScpSessionStatus(
        swi_uint16 Nam, 
        const SWI_STRUCT_HDRSCP_Session_Status *pSessionStatus, 
        swi_uint32 timeout)
{
    return SwiIntSetNvNamU16(CNS_PST_NV, NV_HDRSCP_SESSION_STATUS_I, Nam,
            pSessionStatus->nSessionStatus, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets HDR Forward Traffic Channel statistics.
 *
 * \param pTechHdrSuFwdStats[IN]    HDR Forward Traffic Channel statistics.
 *                                  See SWI_STRUCT_Tech_Hdr_Su_Fwd_Stats.
 * \param timeout[IN]               Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetTechHdrSuFwdStats(
        const SWI_STRUCT_Tech_Hdr_Su_Fwd_Stats *pTechHdrSuFwdStats, 
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
    parmlength = SwiPackTechHdrSuFwdStats( outparamp, pTechHdrSuFwdStats );
    
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_TECH_HDR_SU_FWD_STATS, 
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
 * This function sets HDR Forward Traffic Channel packet termination slot count 
 * for first ten supported DRCs.
 *
 * \param pTechHdrSuFwdSlotCnt[IN]  HDR Forward Traffic Channel packet 
 *                                  termination slot count information.
 *                                  See SWI_STRUCT_Tech_Hdr_Su_Fwd_Slot_Cnt.
 * \param timeout[IN]               Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetTechHdrSuFwdSlotCnt(
        const SWI_STRUCT_Tech_Hdr_Su_Fwd_Slot_Cnt *pTechHdrSuFwdSlotCnt, 
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
    parmlength = SwiPackTechHdrSuFwdSlotCnt( outparamp, pTechHdrSuFwdSlotCnt );
    
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_TECH_HDR_SU_FWD_SLOT_CNT, 
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
 * This function sets HDR MU Forward Traffic Channel statistics.
 *
 * \param pTechHdrMuFwdStats[IN]    HDR MU Forward Traffic Channel statistics.
 *                                  See SWI_STRUCT_Tech_Hdr_Mu_Fwd_Stats.
 * \param timeout[IN]               Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetTechHdrMuFwdStats(
        const SWI_STRUCT_Tech_Hdr_Mu_Fwd_Stats *pTechHdrMuFwdStats, 
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
    parmlength = SwiPackTechHdrMuFwdStats( outparamp, pTechHdrMuFwdStats );
    
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_TECH_HDR_MU_FWD_STATS, 
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
 * This function gets current HDR personality data.
 *
 * \param pTechHdrPersonality[IN]   Current HDR personality data.
 *                                  See SWI_STRUCT_Tech_Hdr_Personality.
 * \param timeout[IN]               Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetTechHdrPersonality(
        SWI_STRUCT_Tech_Hdr_Personality *pTechHdrPersonality, 
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
                                  CNS_TECH_HDR_PERSONALITY, 
                                  0, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackTechHdrPersonality( inparamp, pTechHdrPersonality );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function gets HDR protocol subtype information.
 *
 * \param pTechHdrProtAppSubtype[OUT]   HDR protocol subtype information.
 *                                  See SWI_STRUCT_Tech_Hdr_Prot_App_Subtype.
 * \param timeout[IN]                   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetTechHdrProtAppSubtype(
        SWI_STRUCT_Tech_Hdr_Prot_App_Subtype *pTechHdrProtAppSubtype, 
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
                                  CNS_TECH_HDR_PROT_APP_SUBTYPE, 
                                  0, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackTechHdrProtAppSubtype( inparamp, pTechHdrProtAppSubtype );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function gets negotiated slot cycle indexes and idle slot cycle 
 * timeout information (system time when sleep period switches to new slot 
 * cycle index).
 *
 * \param pTechHdrSleepParms[OUT]   Slot cycle information retrieved.
 *                                  See SWI_STRUCT_Tech_Hdr_Sleep_Parms.
 * \param timeout[IN]               Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetTechHdrSleepParms(
        SWI_STRUCT_Tech_Hdr_Sleep_Parms *pTechHdrSleepParms, 
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
                                  CNS_TECH_HDR_SLEEP_PARMS, 
                                  0, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackTechHdrSleepParms( inparamp, pTechHdrSleepParms );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function gets display banner name.
 *
 * \param pszDisplayBannerName[OUT] Buffer that contains banner name returned.
 * \param szLength[IN]              Size of the buffer pszDisplayBannerName.
 * \param timeout[IN]               Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetDisplayBannerName(swi_charp pszDisplayBannerName, 
                                             swi_uint32 szLength, 
                                             swi_uint32 timeout)
{
    return SwiIntGetNvString(CNS_PST_NV, NV_BANNER_I, pszDisplayBannerName, 
            szLength, LENGTH_NvBannerText, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets SCI
 *
 * \param Sci[IN]           SCI to be set.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetSci(swi_uint16 Sci, swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_NV, NV_SCI, Sci, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets Mobile CAI revision number.
 *
 * \param MobCaiRev[OUT]    Mobile CAI revision number retrieved.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetMobCaiRev(swi_uint16 *MobCaiRev, swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_NV, NV_MOB_CAI_REV, MobCaiRev, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets Mobile CAI revision number.
 *
 * \param MobCaiRev[IN]     Mobile CAI revision number to be set.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetMobCaiRev(swi_uint16 MobCaiRev, swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_NV, NV_MOB_CAI_REV, MobCaiRev, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function get prefer service
 *
 * \param Nam[IN]           NAM index.
 * \param PrefService[OUT]  Prefer service returned.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetPrefServ(swi_uint16 Nam, swi_uint16 *PrefService, 
                                    swi_uint32 timeout)
{
    return SwiIntGetNvNamU16(CNS_PST_NV, NV_PREF_SERV, Nam, PrefService, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets primary CDMA channel number.
 *
 * \param Nam[IN]           NAM index.
 * \param ChannelA[OUT]     Primary channel A returned.
 * \param ChannelB[OUT]     Primary channel B returned.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetPrimaryChan(swi_uint16 Nam, swi_uint16* ChannelA, 
                                       swi_uint16* ChannelB, 
                                       swi_uint32 timeout)
{
    swi_uint16 array[2];
    SWI_RCODE rc;

    rc = SwiIntGetNvNamU16s(CNS_PST_NV, NV_PCDMACH, Nam, array, 2, 
            SWI_AMCDMA, timeout);
    if(SWI_RCODE_OK == rc)
    {
        *ChannelA = array[0];
        *ChannelB = array[1];
    }

    return rc;
}

/**
 *
 * This function sets primary CDMA channel number.
 *
 * \param Nam[IN]           NAM index.
 * \param ChannelA[IN]      Primary channel A to be set.
 * \param ChannelB[IN]      Primary channel B to be set.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetPrimaryChan(swi_uint16 Nam, 
                                       swi_uint16 ChannelA, 
                                       swi_uint16 ChannelB, 
                                       swi_uint32 timeout)
{
    swi_uint16 array[] = { ChannelA, ChannelB };
    return SwiIntSetNvNamU16s(CNS_PST_NV, NV_PCDMACH, Nam, array, 2, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function get secondary channel number.
 *
 * \param Nam[IN]           NAM index.
 * \param ChannelA[OUT]     Secondary channel A returned.
 * \param ChannelB[OUT]     Secondary channel B returned.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetSecondaryChan(swi_uint16 Nam, 
                                         swi_uint16* ChannelA, 
                                         swi_uint16* ChannelB, 
                                         swi_uint32 timeout)
{
    swi_uint16 array[2];
    SWI_RCODE rc;

    rc = SwiIntGetNvNamU16s(CNS_PST_NV, NV_SCDMACH, Nam, array, 2, 
            SWI_AMCDMA, timeout);
    if(SWI_RCODE_OK == rc)
    {
        *ChannelA = array[0];
        *ChannelB = array[1];
    }

    return rc;
}

/**
 *
 * This function get secondary channel number.
 *
 * \param Nam[IN]           NAM index.
 * \param ChannelA[IN]      Secondary channel A to set.
 * \param ChannelB[IN]      Secondary channel B to set.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetSecondaryChan(swi_uint16 Nam, 
                                         swi_uint16 ChannelA, 
                                         swi_uint16 ChannelB, 
                                         swi_uint32 timeout)
{
    swi_uint16 array[] = { ChannelA, ChannelB };
    return SwiIntSetNvNamU16s(CNS_PST_NV, NV_SCDMACH, Nam, array, 2, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function retrieves CDMA MOB_TERM_HOMEp registration flag. This flag 
 * determines if the phone will register on its home network.  
 *
 * \param Nam[IN]               NAM index.
 * \param TermHomeEnable[OUT]   CDMA MOB_TERM_HOMEp registration flag returned.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetTermHome(swi_uint16 Nam, 
                                    swi_uint16 *TermHomeEnable, 
                                    swi_uint32 timeout)
{
    return SwiIntGetNvNamU16(CNS_PST_NV, NV_TERM_HOME, Nam, TermHomeEnable, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function retrieves CDMA MOB_TERM_HOMEp registration flag. This flag 
 * determines if the phone will register on its home network.  
 *
 * \param Nam[IN]               NAM index.
 * \param TermHomeEnable[IN]    CDMA MOB_TERM_HOMEp registration flag to set.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetTermHome(swi_uint16 Nam, 
                                    swi_uint16 TermHomeEnable, 
                                    swi_uint32 timeout)
{
    return SwiIntSetNvNamU16(CNS_PST_NV, NV_TERM_HOME, Nam, TermHomeEnable, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function get SID/NID.
 *
 * \param Nam[IN]       NAM index.
 * \param Sid[OUT]      SID returned.
 * \param Nid[OUT]      NID returned.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetSidNid(swi_uint16 Nam, 
                                  swi_uint16 *Sid, 
                                  swi_uint16 *Nid, 
                                  swi_uint32 timeout)
{
    swi_uint16 array[2];
    SWI_RCODE rc;

    rc = SwiIntGetNvNamU16s(CNS_PST_NV, NV_SID_NID, Nam, array, 2, 
            SWI_AMCDMA, timeout);
    if(SWI_RCODE_OK == rc)
    {
        *Sid = array[0];
        *Nid = array[1];
    }

    return rc;
}

/**
 *
 * This function sets SID/NID.
 *
 * \param Nam[IN]       NAM index.
 * \param Sid[IN]       SID to be set.
 * \param Nid[IN]       NID to be set.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetSidNid(swi_uint16 Nam, 
                                  swi_uint16 Sid, 
                                  swi_uint16 Nid, 
                                  swi_uint32 timeout)
{
    swi_uint16 array[] = { Sid, Nid };
    return SwiIntSetNvNamU16s(CNS_PST_NV, NV_SID_NID, Nam, array, 2, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function retrieves a flag that indicates whether select NAM 
 * automatically when roaming
 *
 * \param AutoNam[OUT]  0: not select NAM automatically when roaming.
 *                      Otherwise select NAM automatically when roaming.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetAutoNam(swi_uint16 *AutoNam, swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_NV, NV_AUTO_NAM, AutoNam, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets a flag that indicates whether select NAM 
 * automatically when roaming
 *
 * \param AutoNam[IN]   0: not select NAM automatically when roaming.
 *                      Otherwise select NAM automatically when roaming.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetAutoNam(swi_uint16 AutoNam, swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_NV, NV_AUTO_NAM, AutoNam, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets auto-redial setting.
 *
 * \param Enable[OUT]   0: auto-redial is not enabled; otherwise enabled.
 * \param Rings[OUT]    Number of rings after which redial starts if enabled.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetAutoRedial(swi_uint16 *Enable, 
                                      swi_uint16 *Rings, 
                                      swi_uint32 timeout)
{
    swi_uint16 array[2];
    SWI_RCODE rc;

    rc = SwiIntGetNvU16s(CNS_PST_NV, NV_AUTO_REDIAL, array, 2, 
            SWI_AMCDMA, timeout);
    if(SWI_RCODE_OK == rc)
    {
        *Enable = array[0];
        *Rings = array[1];
    }

    return rc;
}

/**
 *
 * This function sets auto-redial setting.
 *
 * \param Enable[IN]    0: disable auto-redial; otherwise enable auto-redial.
 * \param Rings[IN]     Number of rings after which redial starts if enabled.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetAutoRedial(swi_uint16 Enable, 
                                      swi_uint16 Rings, 
                                      swi_uint32 timeout)
{
    swi_uint16 array[] = { Enable, Rings };
    return SwiIntSetNvU16s(CNS_PST_NV, NV_AUTO_REDIAL, array, 2, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function retrieves Master Subsidy Lock code.
 *
 * \param ServProgCode[OUT] Buffer that contains Master Subsidy Lock code 
 *                          returned.
 * \param szLength[IN]      Size of the buffer ServProgCode.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetServProgCode(swi_charp ServProgCode, 
                                        swi_uint32 szLength, 
                                        swi_uint32 timeout)
{
    return SwiIntGetNvString(CNS_PST_NV, NV_SEC_CODE_I, ServProgCode, 
        szLength, 0, LENGTH_NvSecCode, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets Master Subsidy Lock code.
 *
 * \param ServProgCode[IN]  Buffer that contains Master Subsidy Lock code 
 *                          to be set.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetServProgCode(const swi_charp ServProgCode, 
                                        swi_uint32 timeout)
{
    return SwiIntSetNvString(CNS_PST_NV, NV_SEC_CODE_I, ServProgCode, 0, 
            LENGTH_NvSecCode, SWI_AMCDMA, timeout);
}

/**
 *
 * This function retrieves field sevice code.
 *
 * \param pszFsc[OUT]   Buffer that contains field sevice code returned.
 * \param szLength[OUT] Size of the buffer pszFsc.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetFieldServCode(swi_charp pszFsc, 
                                         swi_uint32 szLength, 
                                         swi_uint32 timeout)
{
    return SwiIntGetNvString(CNS_PST_NV, NV_FSC, pszFsc, szLength, 0, 
            LENGTH_NvFscCode, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets field sevice code.
 *
 * \param pszFsc[IN]    Buffer that contains field sevice code to be set.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetFieldServCode(const swi_charp pszFsc, 
                                         swi_uint32 timeout)
{
    return SwiIntSetNvString(CNS_PST_NV, NV_FSC, pszFsc, 0, LENGTH_NvFscCode, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function retrieves Warranty Date Code.
 *
 * \param pszWarranty[OUT]      Buffer that contains Warranty Date Code 
 *                              returned.
 * \param lengthWarranty[IN]    Size of the buffer pszWarranty.
 * \param pszExtra[OUT]         Buffer that contains extra Warranty Date Code 
 *                              data returned.
 * \param lengthExtra[IN]       Size of the buffer pszExtra.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetWarDateCode(swi_charp pszWarranty, 
                                       swi_uint32 lengthWarranty, 
                                       swi_charp pszExtra, 
                                       swi_uint32 lengthExtra, 
                                       swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    if (lengthWarranty < LENGTH_NvWdcDate || 
            lengthExtra < LENGTH_NvWdcExtra)
    {
        return SWI_RCODE_SMALL_BUF;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Parameter Field Construction Zone - get a pointer to the 
     * location to fill in the CnS parameter field 
     */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Pack the CnS Parameter field with the input parameters */
    parmlength = SwiPackNv( outparamp, NV_WDC );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PST_NV, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiPstUnpackWarDateCode( inparamp, pszWarranty, lengthWarranty, 
                pszExtra, lengthExtra );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function sets Warranty Date Code.
 *
 * \param pszWarranty[OUT]  Buffer that contains Warranty Date Code to be set.
 * \param pszExtra[OUT]     Buffer that contains extra Warranty Date Code data 
 *                          to be set.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetWarDateCode(const swi_charp pszWarranty, 
                                       const swi_charp pszExtra, 
                                       swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    if (slstrlen(pszWarranty) != LENGTH_NvWdcDate || 
            slstrlen(pszExtra) != LENGTH_NvWdcExtra)
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
    parmlength = SwiPstPackWarDateCode( outparamp, pszWarranty, pszExtra );
    
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
 * This function gets length of IMSI.
 *
 * \param Nam[IN]       NAM index.
 * \param AddrNum[OUT]  Length of IMSI returned.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetImsiAddrNum(swi_uint16 Nam, 
                                       swi_uint16 *AddrNum, 
                                       swi_uint32 timeout)
{
    return SwiIntGetNvNamU16(CNS_PST_NV, NV_IMSI_ADDR_NUM, Nam, AddrNum, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets length of IMSI.
 *
 * \param Nam[IN]       NAM index.
 * \param AddrNum[IN]   Length of IMSI to be set.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetImsiAddrNum(swi_uint16 Nam, 
                                       swi_uint16 AddrNum, 
                                       swi_uint32 timeout)
{
    return SwiIntSetNvNamU16(CNS_PST_NV, NV_IMSI_ADDR_NUM, Nam, AddrNum, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets Directory number in PCS format.
 *
 * \param Nam[IN]           NAM index.
 * \param pszDirNumPcs[OUT] Buffer that contains Directory number returned 
 *                          in PCS format.
 * \param szLength[IN]      Size of the buffer pszDirNumPcs.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetDirNumPcs(swi_uint16 Nam, 
                                     swi_charp pszDirNumPcs, 
                                     swi_uint32 szLength, 
                                     swi_uint32 timeout)
{
    return SwiIntGetNvNamU16LenString(CNS_PST_NV, NV_DIR_NUMBER_PCS, Nam, 
            pszDirNumPcs, szLength, 0, LENGTH_Mdn, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets Directory number in PCS format.
 *
 * \param Nam[IN]           NAM index.
 * \param pszDirNumPcs[IN]  Buffer that contains Directory number to be set 
 *                          in PCS format.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetDirNumPcs(swi_uint16 Nam, 
                                     const swi_charp pszDirNumPcs, 
                                     swi_uint32 timeout)
{
    return SwiIntSetNvNamU16LenString(CNS_PST_NV, NV_DIR_NUMBER_PCS, Nam, 
            pszDirNumPcs, LENGTH_CDMA_PhoneNumber, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets One-Time Keypad Subsidy Lock code.
 *
 * \param pszOneTimeSubsidyLock[OUT]    One-Time Keypad Subsidy Lock code 
 *                                      returned.
 * \param szLength[IN]                  Size of the buffer 
 *                                      pszOneTimeSubsidyLock.
 * \param timeout[IN]                   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetOneTimeSubsidyLock(swi_charp pszOneTimeSubsidyLock, 
                                              swi_uint32 szLength, 
                                              swi_uint32 timeout)
{
    return SwiIntGetNvString(CNS_PST_NV, NV_OTKSL, pszOneTimeSubsidyLock, 
            szLength, LENGTH_NvOtksl, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets One-Time Keypad Subsidy Lock code.
 *
 * \param pszOneTimeSubsidyLock[IN] One-Time Keypad Subsidy Lock code to be 
 *                                  set. 
 * \param timeout[IN]               Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetOneTimeSubsidyLock(
        const swi_charp pszOneTimeSubsidyLock, 
        swi_uint32 timeout)
{
    return SwiIntSetNvString(CNS_PST_NV, NV_OTKSL, pszOneTimeSubsidyLock, 
            LENGTH_NvOtksl, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets True IMSI – MIN1.
 *
 * \param Nam[IN]       NAM index.
 * \param ImsiTS1[OUT]  True IMSI – MIN1 returned.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetImsiTS1(swi_uint16 Nam, 
                                   swi_uint32 *ImsiTS1, 
                                   swi_uint32 timeout)
{
    return SwiIntGetNvNamU32(CNS_PST_NV, NV_IMSI_T_S1, Nam, ImsiTS1, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets True IMSI – MIN1.
 *
 * \param Nam[IN]       NAM index.
 * \param ImsiTS1[IN]   True IMSI – MIN1 to be set.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetImsiTS1(swi_uint16 Nam, 
                                   swi_uint32 ImsiTS1, 
                                   swi_uint32 timeout)
{
    return SwiIntSetNvNamU32(CNS_PST_NV, NV_IMSI_T_S1, Nam, ImsiTS1, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets True IMSI – MIN2.
 *
 * \param Nam[IN]       NAM index.
 * \param ImsiTS2[OUT]  True IMSI – MIN2 returned.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetImsiTS2(swi_uint16 Nam, 
                                   swi_uint16 *ImsiTS2, 
                                   swi_uint32 timeout)
{
    return SwiIntGetNvNamU16(CNS_PST_NV, NV_IMSI_T_S2, Nam, ImsiTS2, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets True IMSI – MIN2.
 *
 * \param Nam[IN]       NAM index.
 * \param ImsiTS1[IN]   True IMSI – MIN2 to be set.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetImsiTS2(swi_uint16 Nam, swi_uint16 ImsiTS2, 
                                   swi_uint32 timeout)
{
    return SwiIntSetNvNamU16(CNS_PST_NV, NV_IMSI_T_S2, Nam, ImsiTS2, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets True IMSI – Mobile country code.
 *
 * \param Nam[IN]       NAM index.
 * \param ImsiMcc[OUT]  True IMSI – Mobile country code returned.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetTrueImsiMcc(swi_uint16 Nam, 
                                       swi_uint16 *ImsiMcc, 
                                       swi_uint32 timeout)
{
    return SwiIntGetNvNamU16(CNS_PST_NV, NV_IMSI_T_MCC, Nam, ImsiMcc, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets True IMSI – Mobile country code.
 *
 * \param Nam[IN]       NAM index.
 * \param ImsiMcc[IN]   True IMSI – Mobile country code to be set.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetTrueImsiMcc(swi_uint16 Nam, 
                                       swi_uint16 ImsiMcc, 
                                       swi_uint32 timeout)
{
    return SwiIntSetNvNamU16(CNS_PST_NV, NV_IMSI_T_MCC, Nam, ImsiMcc, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets True IMSI – 11th and 12th digits
 *
 * \param Nam[IN]       NAM index.
 * \param Imsi1112[Out] True IMSI – 11th and 12th digits returned.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetTrueImsi1112(swi_uint16 Nam, 
                                        swi_uint16 *Imsi1112, 
                                        swi_uint32 timeout)
{
    return SwiIntGetNvNamU16(CNS_PST_NV, NV_IMSI_T_11_12, Nam, Imsi1112, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets True IMSI – 11th and 12th digits
 *
 * \param Nam[IN]       NAM index.
 * \param Imsi1112[Out] True IMSI – 11th and 12th digits to be set.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetTrueImsi1112(swi_uint16 Nam, 
                                        swi_uint16 Imsi1112, 
                                        swi_uint32 timeout)
{
    return SwiIntSetNvNamU16(CNS_PST_NV, NV_IMSI_T_11_12, Nam, Imsi1112, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets a flag that indicates Enable/disable OTASP SPC change.
 *
 * \param Enable[OUT]   0: OTASP SPC change is disabled; otherwise enabled.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetSpcChgEnabled(swi_uint16 *Enable, 
                                         swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_NV, NV_SPC_CHG_ENABLED, Enable, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function enables/disables OTASP SPC change.
 *
 * \param Enable[IN]    0: Disable OTASP SPC change; 
 *                      otherwise Enable OTASP SPC change.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetSpcChgEnabled(swi_uint16 Enable, swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_NV, NV_SPC_CHG_ENABLED, Enable, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function retrieves PAP user ID.
 *
 * \param pszUserId[OUT]    Buffer that contains PAP user ID returned.
 * \param szLength[IN]      Size of the buffer pszUserId.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetPapUserId(swi_charp pszUserId, 
                                     swi_uint32 szLength, 
                                     swi_uint32 timeout)
{
    return SwiIntGetNvU16LenString(CNS_PST_NV, NV_PPP_USER_ID, pszUserId, 
            szLength, LENGTH_NvPppUserID, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets PAP user ID.
 *
 * \param pszUserId[IN] Buffer that contains PAP user ID to be set.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetPapUserId(const swi_charp pszUserId, 
                                     swi_uint32 timeout)
{
    return SwiIntSetNvU16LenString(CNS_PST_NV, NV_PPP_USER_ID, pszUserId, 
            LENGTH_NvPppUserID, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function retrieves PAP password.
 *
 * \param pszPassword[OUT]  Buffer that contains PAP password returned.
 * \param szLength[IN]      Size of the buffer pszPassword.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetPapPassword(swi_charp pszPassword, 
                                       swi_uint32 szLength, 
                                       swi_uint32 timeout)
{
    return SwiIntGetNvU16LenString(CNS_PST_NV, NV_PAP_PASSWORD, pszPassword, 
            szLength, LENGTH_NvPapPassword, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets PAP password.
 *
 * \param pszPassword[IN]   Buffer that contains PAP password to be set.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetPapPassword(const swi_charp pszPassword, 
                                       swi_uint32 timeout)
{
    return SwiIntSetNvU16LenString(CNS_PST_NV, NV_PAP_PASSWORD, pszPassword, 
            LENGTH_NvPapPassword, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets preferred RC.
 *
 * \param pnRC[OUT]     Proferer RC returned.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetPrefRC(swi_uint16* pnRC, swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_NV, NV_PREF_FOR_RC, pnRC, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets preferred RC.
 *
 * \param nRC[IN]       Proferer RC to be set.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetPrefRC(swi_uint16 nRC, swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_NV, NV_PREF_FOR_RC, nRC, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets primary DNS.
 *
 * \param Dns[OUT]      Primary DNS returned.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetPrimaryDns(swi_uint32 *Dns, swi_uint32 timeout)
{
    return SwiIntGetNvU32(CNS_PST_NV, NV_PRIMARY_DNS, Dns, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets primary DNS.
 *
 * \param Dns[IN]       Primary DNS to be set.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetPrimaryDns(swi_uint32 Dns, swi_uint32 timeout)
{
    return SwiIntSetNvU32(CNS_PST_NV, NV_PRIMARY_DNS, Dns, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets secondary DNS.
 *
 * \param Dns[OUT]      Secondary DNS returned.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetSecondaryDns(swi_uint32 *Dns, swi_uint32 timeout)
{
    return SwiIntGetNvU32(CNS_PST_NV, NV_SECONDARY_DNS, Dns, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets secondary DNS.
 *
 * \param Dns[OUT]      Secondary DNS to be set.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetSecondaryDns(swi_uint32 Dns, 
                                        swi_uint32 timeout)
{
    return SwiIntSetNvU32(CNS_PST_NV, NV_SECONDARY_DNS, Dns, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets the mobile’s ability to SCRM settings.
 *
 * \param Enable[OUT]   The mobile’s ability to SCRM settings returned.
 *                      0: Disables the mobile’s ability to SCRM.
 *                      otherwise enables the mobile’s ability to SCRM.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetScrmEnabled(swi_uint16 *Enable, 
                                       swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_NV, NV_SCRM_ENABLED, Enable, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function enables/disables the mobile’s ability to SCRM.
 *
 * \param Enable[IN]    The mobile’s ability to SCRM settings to be set
 *                      0: Disables the mobile’s ability to SCRM.
 *                      otherwise enables the mobile’s ability to SCRM.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetScrmEnabled(swi_uint16 Enable, 
                                       swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_NV, NV_SCRM_ENABLED, Enable, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function retrieves speed dial number information.
 *
 * \param address[IN]           Location for the particular number in the 
 *                              NV_DIAL_I NV Item.
 * \param status[OUT]           Information retrieve status.
 * \param numberOfDigits[OUT]   Number of phone number digits retrieved.
 * \param pszDigits[OUT]        Buffer that contains phone number retrieved.
 * \param szDigitsLength[IN]    Size of the buffer pszDigits.
 * \param pszName[OUT]          Buffer that contains phone number name 
 *                              retrieved.
 * \param szNameLength[IN]      Size of the buffer pszName.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetDialPhonebook(swi_uint16 address, 
                                         swi_uint16* status, 
                                         swi_uint16* numberOfDigits, 
                                         swi_charp pszDigits, 
                                         swi_uint32 szDigitsLength, 
                                         swi_charp pszName, 
                                         swi_uint32 szNameLength, 
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
    parmlength = SwiPackNvU16( outparamp, NV_DIAL_I, address );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PST_NV, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        resultcode = SwiPstUnpackDialPhonebook( inparamp, status, 
                numberOfDigits, pszDigits, szDigitsLength, pszName, 
                szNameLength );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function sets speed dial number information.
 *
 * \param address[IN]           Location for the particular number in the 
 *                              NV_DIAL_I NV Item.
 * \param status[IN]            Status of the phoenbook.
 * \param numberOfDigits[IN]    Number of phone number digits.
 * \param pszDigits[IN]         Buffer that contains phone number to be set.
 * \param pszName[OUT]          Buffer that contains phone number name 
 *                              to be set.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetDialPhonebook(swi_uint16 address, 
                                         swi_uint16 status, 
                                         swi_uint16 numberOfDigits, 
                                         const swi_charp pszDigits, 
                                         const swi_charp pszName, 
                                         swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    if (numberOfDigits > LENGTH_NvDialDigits || 
            slstrlen(pszName) > LENGTH_NvDialLetters)
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
    parmlength = SwiPstPackDialPhonebook( outparamp, address, status, 
            numberOfDigits, pszDigits, pszName );

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
 * This function gets voice privacy settings.
 *
 * \param voicePrivacy[OUT] Voice privacy setting returned.
 *                          0 - Standard.
 *                          1 - Enhanced.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetVoicePrivacy(swi_uint16* voicePrivacy, 
                                        swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_NV, NV_VOICE_PRIV, voicePrivacy, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets voice privacy settings.
 *
 * \param voicePrivacy[IN]  Voice privacy setting to be set.
 *                          0 - Standard.
 *                          1 - Enhanced.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetVoicePrivacy(swi_uint16 voicePrivacy, 
                                        swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_NV, NV_VOICE_PRIV, voicePrivacy, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets flag that indicates whether QNC call is enabled.
 *
 * \param enableQnc[OUT]    Flag that indicates whether QNC call is enabled.
 *                          0: QNC call is not enabled.
 *                          otherwise QNC call is enabled.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetQncCallEnable(swi_uint16* enableQnc, 
                                         swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_NV, NV_DATA_QNC_ENABLE, enableQnc, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function enables/disables QNC call.
 *
 * \param enableQnc[IN] 0: disable QNC call; otherwise enable QNC call.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetQncCallEnable(swi_uint16 enableQnc, 
                                         swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_NV, NV_DATA_QNC_ENABLE, enableQnc, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets a flag that indicates which data service option set is 
 * in effect.
 *
 * \param dataServiceOption[OUT]    Flag that indicates which data service 
 *                                  option set is in effect.
 *                                  0 - Pre-707 SO numbers, 
 *                                  1 - QC proprietary SO numbers, 
 *                                  2 - IS-707 SO numbers. 
 * \param timeout[IN]               Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetDataCallOption(swi_uint16* dataServiceOption, 
                                          swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_NV, NV_DATA_SO_SET, dataServiceOption, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets which data service option set is in effect.
 *
 * \param dataServiceOption[IN] Flag to be set that indicates which data 
 *                              service option set is in effect.
 *                              0 - Pre-707 SO numbers, 
 *                              1 - QC proprietary SO numbers, 
 *                              2 - IS-707 SO numbers. 
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetDataCallOption(swi_uint16 dataServiceOption, 
                                          swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_NV, NV_DATA_SO_SET, dataServiceOption, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets EVRC Voice service option.
 *
 * \param Nam[IN]               NAM index.
 * \param enable[OUT]           0: EVRC Voice services is disabled.
 *                              Otherwise EVRC Voice services is enabled.
 * \param homePageOption[OUT]   Home Page Voice service option.
 * \param homeOrigOption[OUT]   Home Originated Voice service option.
 * \param roamOrigOption[OUT]   Roam Originated Voice service option.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetEvrcVoiceService(swi_uint16 Nam, 
                                            swi_uint16* enable, 
                                            swi_uint16* homePageOption, 
                                            swi_uint16* homeOrigOption, 
                                            swi_uint16* roamOrigOption, 
                                            swi_uint32 timeout)
{
    swi_uint16 array[4];
    SWI_RCODE rc;

    rc = SwiIntGetNvNamU16s(CNS_PST_NV, NV_PREF_VOICE, Nam, array, 4, 
            SWI_AMCDMA, timeout);
    if(SWI_RCODE_OK == rc)
    {
        *enable = array[0];
        *homePageOption = array[1];
        *homeOrigOption = array[2];
        *roamOrigOption = array[3];
    }

    return rc;
}

/**
 *
 * This function sets EVRC Voice service option.
 *
 * \param Nam[IN]               NAM index.
 * \param enable[IN]            0: EVRC Voice services is disabled.
 *                              Otherwise EVRC Voice services is enabled.
 * \param homePageOption[IN]    Home Page Voice service option.
 * \param homeOrigOption[IN]    Home Originated Voice service option.
 * \param roamOrigOption[IN]    Roam Originated Voice service option.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetEvrcVoiceService(swi_uint16 Nam, 
                                            swi_uint16 enable, 
                                            swi_uint16 homePageOption, 
                                            swi_uint16 homeOrigOption, 
                                            swi_uint16 roamOrigOption, 
                                            swi_uint32 timeout)
{
    swi_uint16 array[] = 
    { 
            enable, homePageOption, homeOrigOption, roamOrigOption 
    };

    return SwiIntSetNvNamU16s(CNS_PST_NV, NV_PREF_VOICE, Nam, array, 4, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets the MDR mode.
 *
 * \param modeDataMdr[OUT]  MDR mode returned.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetMdrDataMode(swi_uint16* modeDataMdr, 
                                       swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_NV, NV_DATA_MDR_MODE, modeDataMdr, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets MDR mode.
 *
 * \param modeDataMdr[IN]   MDR mode to be set.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetMdrDataMode(swi_uint16 modeDataMdr, 
                                       swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_NV, NV_DATA_MDR_MODE, modeDataMdr, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets packet call dial string.
 *
 * \param pszDialString[OUT]    Buffer thar contains packet call dial string
 *                              returned.
 * \param szLength[IN]          Size of the buffer pszDialString.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetPktCallDialString(swi_charp pszDialString, 
                                             swi_uint32 szLength, 
                                             swi_uint32 timeout)
{
    return SwiIntGetNvU16LenString(CNS_PST_NV, NV_PKT_ORIG_STR, pszDialString, 
            szLength, LENGTH_NvDialString, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets packet call dial string.
 *
 * \param pszDialString[IN] Buffer thar contains packet call dial string
 *                          to be set.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetPktCallDialString(const swi_charp pszDialString, 
                                             swi_uint32 timeout)
{
    return SwiIntSetNvU16LenString(CNS_PST_NV, NV_PKT_ORIG_STR, pszDialString, 
            LENGTH_NvDialString, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets continuous keypad DTMF tones settings.
 *
 * \param keyDtmf[OUT]  Continuous keypad DTMF tones settings returned.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetKeyDtmf(swi_uint16* keyDtmf, swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_NV, NV_CONT_KEY_DTMF, keyDtmf, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets continuous keypad DTMF tones settings.
 *
 * \param keyDtmf[IN]  Continuous keypad DTMF tones settings to be set.
 * \param timeout[IN]  Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetKeyDtmf(swi_uint16 keyDtmf, swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_NV, NV_CONT_KEY_DTMF, keyDtmf, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets call fade alert settings
 *
 * \param fadeAlert[OUT]    Call fade alert settings returned.
 *                          0 - Call fade alert is on.
 *                          Otherwise call fade alert is off.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetCallFadeAlert(swi_uint16* fadeAlert, 
                                         swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_NV, NV_CALL_FADE_ALERT, fadeAlert, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function enables/disables call fade alert.
 *
 * \param fadeAlert[IN] Call fade alert settings to be set.
 *                      0 - Call fade alert is on.
 *                      Otherwise call fade alert is off.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetCallFadeAlert(swi_uint16 fadeAlert, 
                                         swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_NV, NV_CALL_FADE_ALERT, fadeAlert, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function get modem access lock state.
 *
 * \param accessLock[OUT]   Modem access lock state.
 *                          0: Not locked; otherwise locked.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetModemAccessLock(swi_uint16* accessLock, 
                                           swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_NV, NV_LOCK_I, accessLock, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function locks/unlocks modem.
 *
 * \param accessLock[IN]    0: Unlock modem; otherwise lock modem.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetModemAccessLock(swi_uint16 accessLock, 
                                           swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_NV, NV_LOCK_I, accessLock, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets modem access lock code.
 *
 * \param pszLockCode[OUT]  Buffer that contains modem access lock code 
 *                          returned..
 * \param szLength[IN]      Size of the buffer pszLockCode.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetModemAccessLockCode(swi_charp pszLockCode, 
                                               swi_uint32 szLength, 
                                               swi_uint32 timeout)
{
    return SwiIntGetNvString(CNS_PST_NV, NV_LOCK_CODE_I, pszLockCode, 
            szLength, 0, LENGTH_NvLockCode, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets modem access lock code.
 *
 * \param pszLockCode[IN]  Buffer that contains modem access lock code 
 *                         to be set.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetModemAccessLockCode(const swi_charp pszLockCode, 
                                               swi_uint32 timeout)
{
    return SwiIntSetNvString(CNS_PST_NV, NV_LOCK_CODE_I, pszLockCode, 0, 
            LENGTH_NvLockCode, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets modem auto power lock state.
 *
 * \param autoPowerLock[OUT]    Auto power lock state returned.
 *                              0 - Not locked; otherwise locked.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetModemAutoPowerLock(swi_uint16* autoPowerLock, 
                                              swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_NV, NV_AUTO_LOCK_I, autoPowerLock, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function locks/unlocks modem auto power.
 *
 * \param autoPowerLock[IN] 0 - Unlocks modem auto power.
 *                          Otherwise locks modem auto power.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetModemAutoPowerLock(swi_uint16 autoPowerLock, 
                                              swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_NV, NV_AUTO_LOCK_I, autoPowerLock, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets message alert volume.
 *
 * \param alertVolume[OUT]  Message alert volume returned.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetMessageAlertVolume(swi_uint16* alertVolume, 
                                              swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_NV, NV_ALERTS_I, alertVolume, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets message alert volume.
 *
 * \param alertVolume[IN]   Message alert volume to be set.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetMessageAlertVolume(swi_uint16 alertVolume, 
                                              swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_NV, NV_ALERTS_I, alertVolume, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets key beep volume.
 *
 * \param beepVolume[OUT]   Key beep volume returned.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetKeyBeepVolume(swi_uint16* beepVolume, 
                                         swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_NV, NV_BEEP_LVL_SHADOW, beepVolume, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets key beep volume.
 *
 * \param beepVolume[IN]    Key beep volume to be set.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetKeyBeepVolume(swi_uint16 beepVolume, 
                                         swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_NV, NV_BEEP_LVL_SHADOW, beepVolume, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets ear piece volume.
 *
 * \param pieceVolume[OUT]  Ear piece volume returned.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetEarPieceVolume(swi_uint16* pieceVolume, 
                                          swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_NV, NV_EAR_LVL_SHADOW, pieceVolume, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets ear piece volume.
 *
 * \param pieceVolume[IN]   Ear piece volume to be set.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetEarPieceVolume(swi_uint16 pieceVolume, 
                                          swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_NV, NV_EAR_LVL_SHADOW, pieceVolume, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets voice privacy alert settings.
 *
 * \param privacyAlert[OUT] Voice privacy alert settings returned.
 *                          0 - Voice privacy alert is disabled.
 *                          Otherwise Voice privacy alert is enabled.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetVoicePrivacyAlert(swi_uint16* privacyAlert, 
                                             swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_NV, NV_VOICE_PRIV_ALERT, privacyAlert, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function enables/disables voice privacy alert.
 *
 * \param privacyAlert[IN]  0 - Disables voice privacy alert.
 *                          Otherwise enables voice privacy alert.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetVoicePrivacyAlert(swi_uint16 privacyAlert, 
                                             swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_NV, NV_VOICE_PRIV_ALERT, privacyAlert, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets PRL settings.
 *
 * \param prlEnable[OUT]    PRL settings returned.
 *                          0 - PRL is disabled; otherwise PRL is enabled.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetPrlEnable(swi_uint16* prlEnable, swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_NV, NV_PRL_ENABLED, prlEnable, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function enables/disables PRL.
 *
 * \param prlEnable[IN] 0 - Disables PRL; otherwise enables PRL.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetPrlEnable(swi_uint16 prlEnable, swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_NV, NV_PRL_ENABLED, prlEnable, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets SMS timeout format.
 *
 * \param smsUtcTime[IN]    SMS timeout format returned.
 *                          0 - UTC Time.
 *                          0 - Adjusted Time.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetSmsTimeFormat(swi_uint16* smsUtcTime, 
                                         swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_NV, NV_SMS_UTC_I, smsUtcTime, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets SMS timeout format.
 *
 * \param smsUtcTime[IN]    SMS timeout format to be set.
 *                          0 - UTC Time.
 *                          0 - Adjusted Time.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetSmsTimeFormat(swi_uint16 smsUtcTime, 
                                         swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_NV, NV_SMS_UTC_I, smsUtcTime, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets roam ringer settings.
 *
 * \param roamRinger[OUT]   Roam ringer settings returned.
 *                          0 - Roam ringer disabled.
 *                          1 - Roam ringer enabled.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetRoamRinger(swi_uint16* roamRinger, 
                                      swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_NV, NV_ROAM_RINGER, roamRinger, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function enables/disables roam ringer.
 *
 * \param roamRinger[IN]    Roam ringer settings to be set.
 *                          0 - Disables roam ringer.
 *                          1 - Enables roam ringer.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetRoamRinger(swi_uint16 roamRinger, 
                                      swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_NV, NV_ROAM_RINGER, roamRinger, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets data packet auto detection settings.
 *
 * \param pktAutoDetect[IN] Data packet auto detection returned.
 *                          0 - Data packet auto detection is disabled.
 *                          Otherwise Data packet auto detection is enabled.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetDataPktAutoDetect(swi_uint16* pktAutoDetect, 
                                             swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_NV, NV_DATA_AUTO_PACKET_DET, pktAutoDetect, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function enables/disables data packet auto detection.
 *
 * \param pktAutoDetect[IN] Data packet auto detection to be set.
 *                          0 - Disables data packet auto detection.
 *                          Otherwise enables data packet auto detection.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetDataPktAutoDetect(swi_uint16 pktAutoDetect, 
                                             swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_NV, NV_DATA_AUTO_PACKET_DET, pktAutoDetect, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets SMS reminder tone.
 *
 * \param reminderTone[OUT] SMS reminder tone returned.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetSmsReminderTone(swi_uint16* reminderTone, 
                                           swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_NV, NV_SMS_REMINDER, reminderTone, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets SMS reminder tone.
 *
 * \param reminderTone[IN]  SMS reminder tone to be set.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetSmsReminderTone(swi_uint16 reminderTone, 
                                           swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_NV, NV_SMS_REMINDER, reminderTone, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets high temperature protection threshold.
 *
 * \param tempThreshold[OUT]    Temperature protection threshold returned.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetTempThreshold(swi_uint16 *tempThreshold, 
                                         swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_SWINV, SWI_NV_TEMP_THRESH, tempThreshold, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets high temperature protection threshold.
 *
 * \param tempThreshold[IN] Temperature protection threshold to be set.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetTempThreshold(swi_uint16 tempThreshold, 
                                         swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_SWINV, SWI_NV_TEMP_THRESH, tempThreshold, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets FSN Number (Manufacturing Trace-ability number).
 *
 * \param pszFsn[OUT]   FSN numcer returned.
 * \param szLength[IN]  Size of the buffer pszFsn.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetFsn(swi_charp pszFsn, 
                               swi_uint32 szLength, 
                               swi_uint32 timeout)
{
    return SwiIntGetNvString(CNS_PST_SWINV, SWI_NV_FSN, pszFsn, szLength, 0, 
            LENGTH_NvFsn, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets FSN Number (Manufacturing Trace-ability number).
 *
 * \param pszFsn[IN]    FSN numcer to be set.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetFsn(const swi_charp pszFsn, swi_uint32 timeout)
{
    return SwiIntSetNvString(CNS_PST_SWINV, SWI_NV_FSN, pszFsn, 0, 
            LENGTH_NvFsn, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets calibration date.
 *
 * \param pszCalDate[OUT]   Calibration date returned.
 * \param szLength[IN]      This is a parameter.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetCalibrationDate(swi_charp pszCalDate, 
                                           swi_uint32 szLength, 
                                           swi_uint32 timeout)
{
    return SwiIntGetNvString(CNS_PST_SWINV, SWI_NV_CAL_DATE, pszCalDate, 
            szLength, 0, LENGTH_NvCalDate, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets calibration date.
 *
 * \param pszCalDate[IN]    Calibration date to be set.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetCalibrationDate(const swi_charp pszCalDate, 
                                           swi_uint32 timeout)
{
    return SwiIntSetNvString(CNS_PST_SWINV, SWI_NV_CAL_DATE, pszCalDate, 0, 
            LENGTH_NvCalDate, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets test history.
 *
 * \param testHistory[OUT]  Test history returned.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetTestHistory(swi_uint16* testHistory, 
                                       swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_SWINV, SWI_NV_TEST_HISTORY, testHistory, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets test history.
 *
 * \param testHistory[IN]   Test history to be set.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetTestHistory(swi_uint16 testHistory, 
                                       swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_SWINV, SWI_NV_TEST_HISTORY, testHistory, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function retrieves provision information.
 *
 * \param nam1ProvFlag[OUT]     Provision flag for NAM1 returned.
 * \param pszNam1ProvDate[OUT]  Buffer that contains provision date for NAM1.
 * \param szLengthNam1[IN]      Size of the buffer pszNam1ProvDate.
 * \param nam2ProvFlag[OUT]     Provision flag for NAM2 returned.
 * \param pszNam2ProvDate[OUT]  Buffer that contains provision date for NAM2.
 * \param szLengthNam2[IN]      Size of the buffer pszNam2ProvDate.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetProvisionInfo(swi_uint32* nam1ProvFlag, 
                                         swi_charp pszNam1ProvDate, 
                                         swi_uint32 szLengthNam1, 
                                         swi_uint32* nam2ProvFlag, 
                                         swi_charp pszNam2ProvDate, 
                                         swi_uint32 szLengthNam2, 
                                         swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    if (szLengthNam1 < LENGTH_NvNam1ProvisionDate ||
            szLengthNam2 < LENGTH_NvNam2ProvisionDate)
    {
        return SWI_RCODE_SMALL_BUF;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Parameter Field Construction Zone - get a pointer to the 
     * location to fill in the CnS parameter field 
     */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Pack the CnS Parameter field with the input parameters */
    parmlength = SwiPackNv( outparamp, SWI_NV_PROVISION_INFO );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PST_SWINV, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        resultcode = SwiPstUnpackProvisionInfo( inparamp, 
                nam1ProvFlag, pszNam1ProvDate, szLengthNam1, 
                nam2ProvFlag, pszNam2ProvDate, szLengthNam2);
        
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function sets provision information.
 *
 * \param nam1ProvFlag[IN]      Provision flag for NAM1 to be set.
 * \param pszNam1ProvDate[IN]   Provision date for NAM1 to be set.
 * \param nam2ProvFlag[IN]      Provision flag for NAM2 to be set.
 * \param pszNam2ProvDate[IN]   Provision date for NAM2 to be set.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetProvisionInfo(swi_uint32 nam1ProvFlag, 
                                         const swi_charp pszNam1ProvDate, 
                                         swi_uint32 nam2ProvFlag, 
                                         const swi_charp pszNam2ProvDate, 
                                         swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    if (slstrlen(pszNam1ProvDate) > LENGTH_NvNam1ProvisionDate ||
            slstrlen(pszNam2ProvDate) > LENGTH_NvNam2ProvisionDate)
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
    parmlength = SwiPstPackProvisionInfo( outparamp, nam1ProvFlag, 
            pszNam1ProvDate, nam2ProvFlag, pszNam2ProvDate );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PST_SWINV, 
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
 * This function unlocks modem using MSL code.
 *
 * \param SubsidyLock[IN]   MSL code used to unlock modem.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstMSLUnlock(swi_uint32 SubsidyLock, swi_uint32 timeout)
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
    parmlength = SwiPackU8s( outparamp, nTemp, LENGTH_SecureChallenge );

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
                                  CNS_SECURE_MSL_REBUTTAL, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    if (SWI_RCODE_OK == resultcode)  /* Success. */
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
 * This function gets Finger set information.
 *
 * \param TechFing[OUT] Finger set information returned.
 *                      See SWI_STRUCT_TechFinger.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetTechFinger(SWI_STRUCT_TechFinger* TechFing, 
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
                                  CNS_TECH_FING, 
                                  0, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackTechFinger( inparamp, TechFing );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function retrieves the address range in the NV Emergency phone book 
 * and the bit mask of the programmed location for a particular E911 Type.
 *
 * \param e911Number[IN]        E911 info type.
 *                              0x0000 – Carrier-defined E911 Number
 *                              0x0001 – User-defined E911 Number
 * \param startLocation[OUT]    Inclusive start location for the particular 
 *                              emergency number in the NV_DIAL_I NV Item.
 * \param endLocation[OUT]      Inclusive end location for the particular 
 *                              emergency number in the NV_DIAL_I NV Item.
 * \param programMask[OUT]      The Least significant bit in the Program Mask 
 *                              corresponds to the phone number in the start 
 *                              location and the Most Significant bit in the 
 *                              program mask corresponds to the phone number 
 *                              in the end location. Each bit is set when that 
 *                              particular location address is written.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetE911Info(swi_uint16 e911Number, 
                                 swi_uint16* startLocation, 
                                 swi_uint16* endLocation, 
                                 swi_uint16* programMask, 
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
    parmlength = SwiPackU16( outparamp, e911Number );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_E911_INFO, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackE911Info( inparamp, startLocation, endLocation, programMask );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function gets audio path.
 *
 * \param audioMode[OUT]    Audio mode returned.
 *                          0x0000 – handset.
 *                          0x0001 – headset.
 *                          0x0002 – speakerphone.
 *                          0x0003 – carkit.
 *                          0x0004 - TTY full.
 *                          0x0005 – TTY talk.
 *                          0x0006 – TTY hear.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetAudioMode(swi_uint16* audioMode, swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_SB_AUDMOD, audioMode, SWI_AMCDMA, timeout);
}

/**
 *
 * This function seta audio path.
 *
 * \param audioMode[IN] Audio path to be set.
 *                      0x0000 – handset.
 *                      0x0001 – headset.
 *                      0x0002 – speakerphone.
 *                      0x0003 – carkit.
 *                      0x0004 - TTY full.
 *                      0x0005 – TTY talk.
 *                      0x0006 – TTY hear.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetAudioMode(swi_uint16 audioMode, swi_uint32 timeout)
{
    return SwiIntSetU16(CNS_SB_AUDMOD, audioMode, NULL, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets RLP statistics data.
 *
 * \param techRlpStat[OUT]  RLP statistics data returned.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetTechRlpStat(swi_uint16 techRlpStat[NUMBER_TechRlpStat], 
                                    swi_uint32 timeout)
{
        return SwiIntGetU16s(CNS_TECH_RLP_STAT, techRlpStat, 
                NUMBER_TechRlpStat, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets RLP statistics data.
 *
 * \param techRlpStat[IN]   RLP statistics data to be set.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetTechRlpStat(
        swi_uint16 techRlpStat[NUMBER_TechRlpStat], 
        swi_uint32 timeout)
{
        return SwiIntSetU16s(CNS_TECH_RLP_STAT, techRlpStat, 
                NUMBER_TechRlpStat, NULL, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets RLP33 statistics data.
 *
 * \param techRlp33Stat[OUT]    RLP33 statistics data returned.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetTechRlp33Stat(
        swi_uint32 techRlp33Stat[NUMBER_TechRlp33Stat], 
        swi_uint32 timeout)
{
        return SwiIntGetU32s(CNS_TECH_RLP33_STAT, techRlp33Stat, 
                NUMBER_TechRlp33Stat, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets RLP33 statistics data.
 *
 * \param techRlp33Stat[IN] RLP33 statistics data to be set.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetTechRlp33Stat(
        swi_uint32 techRlp33Stat[NUMBER_TechRlp33Stat], 
        swi_uint32 timeout)
{
        return SwiIntSetU32s(CNS_TECH_RLP33_STAT, techRlp33Stat, 
                NUMBER_TechRlp33Stat, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets DTMF mode.
 *
 * \param DtmfMode[OUT] DTMF mode returned.
 *                      0 - Continuous.
 *                      Otherwise Burst.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetDtmfMode(swi_uint16* DtmfMode, swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8 array[2];

    resultcode = SwiIntGetU8s(CNS_DTMF_LEVEL, array, 
            sizeof(array)/sizeof(array[0]), SWI_AMCDMA, timeout);
    
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        *DtmfMode = array[1];
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function sets DTMF mode.
 *
 * \param DtmfMode[IN]  DTMF mode to be set.
 *                      0 - Continuous.
 *                      1 - Burst.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetDtmfMode(swi_uint16 DtmfMode, swi_uint32 timeout)
{
    swi_uint8 array[] = { 0, (swi_uint8)DtmfMode };
    return SwiIntSetU8s(CNS_CONT_DTMF_MODE, array, 
            sizeof(array)/sizeof(array[0]), SWI_AMCDMA, timeout);
}

/**
 *
 * This function starts to send continuous DTMF.
 *
 * \param startKeyDtmf[IN]  Key DTMF to start.
 *                          See SWI_TYPE_PhoneKey.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetKeyDtmfStart(swi_uint16 startKeyDtmf, 
                                     swi_uint32 timeout)
{
    return SwiIntSetU16(CNS_CONT_DTMF_START, startKeyDtmf, NULL, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function stops to send continuous DTMF.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetKeyDtmfStop()
{
    return SwiIntSet(CNS_CONT_DTMF_STOP, SWI_AMCDMA, APTIMEOUT);
}

/**
 *
 * This function gets TTY Options
 *
 * \param ttyOption[OUT]    TTY options returned.
 *                          0 - Off.
 *                          1 - On (Full).
 *                          2 - On (Talk Only).
 *                          3 - On (Hear Only).
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetTtyOption(swi_uint16* ttyOption, swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_TTY_OPTION, ttyOption, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets TTY Options
 *
 * \param ttyOption[IN]     TTY options to be set.
 *                          0 - Off.
 *                          1 - On (Full).
 *                          2 - On (Talk Only).
 *                          3 - On (Hear Only).
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetTtyOption(swi_uint16 ttyOption, swi_uint32 timeout)
{
    return SwiIntSetU16(CNS_TTY_OPTION, ttyOption, NULL, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets ICMP statistics data.
 *
 * \param techIcmpStat[OUT] ICMP statistics data returned.
 *                          techIcmpStat[0] - The total number of ICMP 
 *                                            messages received.
 *                          techIcmpStat[1] - The total number of ICMP 
 *                                            messages sent.
 *                          techIcmpStat[2] - The total number of ICMP Echo 
 *                                            (request) messages received.
 *                          techIcmpStat[3] - The total number of ICMP 
 *                                            Timestamp (request) messages 
 *                                            received.
 *                          techIcmpStat[4] - The total number of ICMP Info 
 *                                            Request messages received.
 *                          techIcmpStat[5] - The total number of broadcast  
 *                                            ICMP messages received.
 *                          techIcmpStat[6] - The total number messages with 
 *                                            bad ICMP checksum.
 *                          techIcmpStat[7] - The total number of invalid ICMP 
 *                                            messages.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetTechIcmpStat(
        swi_uint16 techIcmpStat[NUMBER_TechIcmpStat], 
        swi_uint32 timeout)
{
        return SwiIntGetU16s(CNS_TECH_ICMP_STAT, techIcmpStat, 
                NUMBER_TechIcmpStat, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets ICMP statistics data.
 *
 * \param techIcmpStat[IN]  ICMP statistics data returned.
 *                          techIcmpStat[0] - The total number of ICMP 
 *                                            messages received.
 *                          techIcmpStat[1] - The total number of ICMP 
 *                                            messages sent.
 *                          techIcmpStat[2] - The total number of ICMP Echo 
 *                                            (request) messages received.
 *                          techIcmpStat[3] - The total number of ICMP 
 *                                            Timestamp (request) messages 
 *                                            received.
 *                          techIcmpStat[4] - The total number of ICMP Info 
 *                                            Request messages received.
 *                          techIcmpStat[5] - The total number of broadcast  
 *                                            ICMP messages received.
 *                          techIcmpStat[6] - The total number messages with 
 *                                            bad ICMP checksum.
 *                          techIcmpStat[7] - The total number of invalid ICMP 
 *                                            messages.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetTechIcmpStat(
        swi_uint16 techIcmpStat[NUMBER_TechIcmpStat], 
        swi_uint32 timeout)
{
        return SwiIntSetU16s(CNS_TECH_ICMP_STAT, techIcmpStat, 
                NUMBER_TechIcmpStat, NULL, SWI_AMCDMA, timeout);
}

/**
 *
 * This function get UDP statistics data.
 *
 * \param techUdpStat[OUT]  UDP statistics data returned.
 *                          techUdpStat[0] - Total number of UDP datagrams 
 *                                           delivered to UDP users.
 *                          techUdpStat[1] - Total number of UDP datagrams 
 *                                           sent.
 *                          techUdpStat[2] - Number of received UDP datagrams 
 *                                           that could not be delivered for 
 *                                           reasons other than the lack of an 
 *                                           application at the destination 
 *                                           port.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetTechUdpStat(
        swi_uint16 techUdpStat[NUMBER_TechUdpStat], 
        swi_uint32 timeout)
{
        return SwiIntGetU16s(CNS_TECH_UDP_STAT, techUdpStat, 
                NUMBER_TechUdpStat, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets UDP statistics data.
 *
 * \param techUdpStat[IN]   UDP statistics data to be set.
 *                          techUdpStat[0] - Total number of UDP datagrams 
 *                                           delivered to UDP users.
 *                          techUdpStat[1] - Total number of UDP datagrams 
 *                                           sent.
 *                          techUdpStat[2] - Number of received UDP datagrams 
 *                                           that could not be delivered for 
 *                                           reasons other than the lack of an 
 *                                           application at the destination 
 *                                           port.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetTechUdpStat(
        swi_uint16 techUdpStat[NUMBER_TechUdpStat], 
        swi_uint32 timeout)
{
        return SwiIntSetU16s(CNS_TECH_UDP_STAT, techUdpStat, 
                NUMBER_TechUdpStat, NULL, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets TCP statistics data. 
 *
 * \param techTcpStat[OUT]  TCP statistics data returned.
 *                          techTcpStat[0] - The number of times TCP 
 *                                           connections have made a direct 
 *                                           transition to the SYN-SENT state 
 *                                           from the CLOSED state.
 *                          techTcpStat[1] - The number of times TCP 
 *                                           connections have made a direct 
 *                                           transition to the SYN-RCVD state 
 *                                           from the LISTEN state.
 *                          techTcpStat[2] - The number of times TCP 
 *                                           connections have made a direct 
 *                                           transition to the CLOSED state 
 *                                           from either the SYN-SENT state or 
 *                                           th SYN-RCVD state, plus the 
 *                                           number of times TCP connections 
 *                                           have made a direct transition to 
 *                                           the LISTEN state from the 
 *                                           SYN-RCVD state.
 *                          techTcpStat[3] - The number of times TCP 
 *                                           connections have made a direct 
 *                                           transition to the CLOSED state 
 *                                           from either the ESTABLISHED state 
 *                                           or the CLOSE-WAIT state.
 *                          techTcpStat[4] - The number of segments received, 
 *                                           including  those received in 
 *                                           error.
 *                          techTcpStat[5] - The number of segments sent, 
 *                                           including  those on current 
 *                                           connections but excluding those 
 *                                           containing only retransmitted 
 *                                           octets.
 *                          techTcpStat[6] - The number of segments 
 *                                           retransmitted.
 *                          techTcpStat[7] - The number of segments received 
 *                                           in error.
 *                          techTcpStat[8] - The number of TCP segments sent 
 *                                           containing the RST flag.
 *                          techTcpStat[9] - The number of times a segment 
 *                                           was received out of order
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetTechTcpStat(
        swi_uint16 techTcpStat[NUMBER_TechTcpStat], 
        swi_uint32 timeout)
{
        return SwiIntGetU16s(CNS_TECH_TCP_STAT, techTcpStat, 
                NUMBER_TechTcpStat, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets TCP statistics data. 
 *
 * \param techTcpStat[IN]   TCP statistics data to be set.
 *                          techTcpStat[0] - The number of times TCP 
 *                                           connections have made a direct 
 *                                           transition to the SYN-SENT state 
 *                                           from the CLOSED state.
 *                          techTcpStat[1] - The number of times TCP 
 *                                           connections have made a direct 
 *                                           transition to the SYN-RCVD state 
 *                                           from the LISTEN state.
 *                          techTcpStat[2] - The number of times TCP 
 *                                           connections have made a direct 
 *                                           transition to the CLOSED state 
 *                                           from either the SYN-SENT state or 
 *                                           th SYN-RCVD state, plus the 
 *                                           number of times TCP connections 
 *                                           have made a direct transition to 
 *                                           the LISTEN state from the 
 *                                           SYN-RCVD state.
 *                          techTcpStat[3] - The number of times TCP 
 *                                           connections have made a direct 
 *                                           transition to the CLOSED state 
 *                                           from either the ESTABLISHED state 
 *                                           or the CLOSE-WAIT state.
 *                          techTcpStat[4] - The number of segments received, 
 *                                           including  those received in 
 *                                           error.
 *                          techTcpStat[5] - The number of segments sent, 
 *                                           including  those on current 
 *                                           connections but excluding those 
 *                                           containing only retransmitted 
 *                                           octets.
 *                          techTcpStat[6] - The number of segments 
 *                                           retransmitted.
 *                          techTcpStat[7] - The number of segments received 
 *                                           in error.
 *                          techTcpStat[8] - The number of TCP segments sent 
 *                                           containing the RST flag.
 *                          techTcpStat[9] - The number of times a segment 
 *                                           was received out of order
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetTechTcpStat(
        swi_uint16 techTcpStat[NUMBER_TechTcpStat], 
        swi_uint32 timeout)
{
        return SwiIntSetU16s(CNS_TECH_TCP_STAT, techTcpStat, 
                NUMBER_TechTcpStat, NULL, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets IP statistics data.
 *
 * \param techIpStat[OUT]   IP statistics data returned.
 *                          techIpStat[0] - The total number of input 
 *                                          datagrams received from interfaces.
 *                          techIpStat[0] - The total number of input 
 *                                          datagrams discarded due to errors 
 *                                          in their IP headers.
 *                          techIpStat[0] - The total number of locally-
 *                                          addressed datagrams received 
 *                                          successfully but discarded because 
 *                                          of an unknown or unsupported 
 *                                          protocol.
 *                          techIpStat[0] - The total number of input 
 *                                          datagrams successfully delivered 
 *                                          to IP user-protocols.
 *                          techIpStat[0] - The total number of IP datagrams 
 *                                          which local IP user-protocols 
 *                                          supplied to IP in requests for 
 *                                          transmission.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetTechIpStat(swi_uint16 techIpStat[NUMBER_TechIpStat], 
                                   swi_uint32 timeout)
{
        return SwiIntGetU16s(CNS_TECH_IP_STAT, techIpStat, NUMBER_TechIpStat, 
                SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets IP statistics data.
 *
 * \param techIpStat[IN]    IP statistics data to be set.
 *                          techIpStat[0] - The total number of input 
 *                                          datagrams received from interfaces.
 *                          techIpStat[0] - The total number of input 
 *                                          datagrams discarded due to errors 
 *                                          in their IP headers.
 *                          techIpStat[0] - The total number of locally-
 *                                          addressed datagrams received 
 *                                          successfully but discarded because 
 *                                          of an unknown or unsupported 
 *                                          protocol.
 *                          techIpStat[0] - The total number of input 
 *                                          datagrams successfully delivered 
 *                                          to IP user-protocols.
 *                          techIpStat[0] - The total number of IP datagrams 
 *                                          which local IP user-protocols 
 *                                          supplied to IP in requests for 
 *                                          transmission.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetTechIpStat(swi_uint16 techIpStat[NUMBER_TechIpStat], 
                                   swi_uint32 timeout)
{
        return SwiIntSetU16s(CNS_TECH_IP_STAT, techIpStat, NUMBER_TechIpStat, 
                NULL, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets PPP statistics data.
 *
 * \param techPppStat[OUT]  PPP statistics data returned.
 *                          techPppStat[0] - The number of LCP messages 
 *                                           received.
 *                          techPppStat[1] - The number of LCP messages 
 *                                           transmitted.
 *                          techPppStat[2] - The number of IPCP messages 
 *                                           received.
 *                          techPppStat[3] - The number of IPCP messages 
 *                                           transmitted.
 *                          techPppStat[4] - The number of Um PPP packets 
 *                                           that we don't unframe.
 *                          techPppStat[5] - The number of Rm PPP packets 
 *                                           that we don't unframe.
 *                          techPppStat[6] - The number of Um PPP packets 
 *                                           that we are unframed.
 *                          techPppStat[7] - The number of Rm PPP packets 
 *                                           that we are unframed.
 *                          techPppStat[8] - Total Um PPP packets that arrive.
 *                          techPppStat[9] - Total Rm PPP packets that arrive.
 *                          techPppStat[10] - The number of packets received in 
 *                                            internal mode.
 *                          techPppStat[11] - The number of packets tramsmitted 
 *                                            in internal mode.
 *                          techPppStat[12] - The number of incoming packets 
 *                                            discarded due to lack of buffer 
 *                                            space.
 *                          techPppStat[13] - The number of outgoing packets 
 *                                            discarded due to lack of buffer 
 *                                            space.
 *                          techPppStat[14] - The number of packets received 
 *                                            in error.
 *                          techPppStat[15] - The number of outgoing packets 
 *                                            in error.
 *                          techPppStat[16] - The number of incoming packets 
 *                                            with unknown protocol.
 *                          techPppStat[17] - The number of incoming packets 
 *                                            with checksum error.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetTechPppStat(
        swi_uint16 techPppStat[NUMBER_TechPppStat], 
        swi_uint32 timeout)
{
        return SwiIntGetU16s(CNS_TECH_PPP_STAT, techPppStat, 
                NUMBER_TechPppStat, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets PPP statistics data.
 *
 * \param techPppStat[IN]   PPP statistics data returned.
 *                          techPppStat[0] - The number of LCP messages 
 *                                           received.
 *                          techPppStat[1] - The number of LCP messages 
 *                                           transmitted.
 *                          techPppStat[2] - The number of IPCP messages 
 *                                           received.
 *                          techPppStat[3] - The number of IPCP messages 
 *                                           transmitted.
 *                          techPppStat[4] - The number of Um PPP packets 
 *                                           that we don't unframe.
 *                          techPppStat[5] - The number of Rm PPP packets 
 *                                           that we don't unframe.
 *                          techPppStat[6] - The number of Um PPP packets 
 *                                           that we are unframed.
 *                          techPppStat[7] - The number of Rm PPP packets 
 *                                           that we are unframed.
 *                          techPppStat[8] - Total Um PPP packets that arrive.
 *                          techPppStat[9] - Total Rm PPP packets that arrive.
 *                          techPppStat[10] - The number of packets received in 
 *                                            internal mode.
 *                          techPppStat[11] - The number of packets tramsmitted 
 *                                            in internal mode.
 *                          techPppStat[12] - The number of incoming packets 
 *                                            discarded due to lack of buffer 
 *                                            space.
 *                          techPppStat[13] - The number of outgoing packets 
 *                                            discarded due to lack of buffer 
 *                                            space.
 *                          techPppStat[14] - The number of packets received 
 *                                            in error.
 *                          techPppStat[15] - The number of outgoing packets 
 *                                            in error.
 *                          techPppStat[16] - The number of incoming packets 
 *                                            with unknown protocol.
 *                          techPppStat[17] - The number of incoming packets 
 *                                            with checksum error.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetTechPppStat(
        swi_uint16 techPppStat[NUMBER_TechPppStat], 
        swi_uint32 timeout)
{
        return SwiIntSetU16s(CNS_TECH_PPP_STAT, techPppStat, 
                NUMBER_TechPppStat, NULL, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets error log number.
 *
 * \param errorNumber[OUT]  Error log number returned.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetTechErrorNumber(swi_uint16* errorNumber, 
                                        swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_TECH_GET_ERRNUM, errorNumber, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets error statistics data
 *
 * \param errSeqNumber[OUT]     Array index, Range: 0 – 19.
 * \param errFatalFlag[OUT]     Fatal error flag.
 *                              0x00 - other errors.
 *                              0x01 - fatal error.
 * \param errCount[OUT]         Error count.
 * \param errFilename[OUT]      File Name.
 * \param errLineNumber[OUT]    Line number.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetTechErrorStat(swi_uint16 errSeqNumber, 
                                      swi_uint8* errFatalFlag, 
                                      swi_uint8* errCount, 
                                      swi_charp errFilename, 
                                      swi_uint32 length, 
                                      swi_uint16* errLineNumber, 
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
    parmlength = SwiPackU16( outparamp, errSeqNumber );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_TECH_ERR_STAT, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackTechErrorStat( inparamp, errFatalFlag, errCount, 
                errFilename, length, errLineNumber );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function sets error statistics data
 *
 * \param errSeqNumber[IN]  Array index, Range: 0 – 19.
 * \param errFatalFlag[IN]  Fatal error flag.
 *                          0x00 - other errors.
 *                          0x01 - fatal error.
 * \param errCount[IN]      Error count.
 * \param errFilename[IN]   File Name.
 * \param errLineNumber[IN] Line number.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetTechErrorStat(swi_uint16 errSeqNumber, 
                                      swi_uint8 errFatalFlag, 
                                      swi_uint8 errCount, 
                                      const swi_charp errFilename, 
                                      swi_uint16 errLineNumber, 
                                      swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    if (slstrlen(errFilename) > LENGTH_TechErrFileName)
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
    parmlength = SwiPackTechErrorStat( outparamp, errSeqNumber, 
            errFatalFlag, errCount, errFilename, errLineNumber );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_TECH_ERR_STAT, 
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
 * This function gets temperature threshold.
 *
 * \param tempThreshold[OUT]    Temperature threshold returned.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetTemperatureThreshold(swi_uint16* tempThreshold, 
                                             swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_TEMPERATURE_THRESH, tempThreshold, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets temperature threshold.
 *
 * \param tempThreshold[IN] Temperature threshold to be set.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetTemperatureThreshold(swi_uint16 tempThreshold, 
                                             swi_uint32 timeout)
{
    return SwiIntSetU16(CNS_TEMPERATURE_THRESH, tempThreshold, NULL, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets incoming call type
 *
 * \param callType[OUT] Incoming call type returned.
 *                      See SWI_TYPE_CDMA_CallType.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetIncomingCallType(swi_uint16* callType, 
                                         swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_CALL_INCOMING, callType, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets the Rx power in Dbm multiply by 100.
 *
 * \param RxPower[OUT]  The Rx power returned. The Rx power in Dbm 
 *                      multiply by 100
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetReceivePower(swi_int16* RxPower, swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_TECH_RX_PWR, (swi_uint16*)RxPower, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets hardware version.
 *
 * \param pszHardwareVersion[OUT]   Buffer that contains hardware version 
 *                                  returned.
 * \param szLength[IN]              Size of the buffer szLength.
 * \param timeout[IN]               Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetTechHardwareVersion(swi_charp pszHardwareVersion, 
                                            swi_uint32 szLength, 
                                            swi_uint32 timeout)
{
    return SwiIntGetNvString(CNS_PST_NV, NV_HW_REV_ID_I, pszHardwareVersion, 
            szLength, 0, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets sidetone gain level.
 *
 * \param sidetoneGainLvl[OUT]  Sidetone gain level returned.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetSidetoneGainLevel(swi_uint16* sidetoneGainLvl, 
                                          swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_SB_STGLVL, sidetoneGainLvl, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets sidetone gain level.
 *
 * \param sidetoneGainLvl[IN]   Sidetone gain level to be set.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetSidetoneGainLevel(swi_uint16 sidetoneGainLvl, 
                                          swi_uint32 timeout)
{
    return SwiIntSetU16(CNS_SB_STGLVL, sidetoneGainLvl, NULL, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets SMS delivery over Traffic channel only settings.
 *
 * \param trafficOnly[OUT]  SMS delivery over Traffic channel only settings.
 *                          0 - MS delivery over Traffic channel only is 
 *                              disabled. Delivery over access channel is 
 *                              always attempted first.
 *                          1 - MS delivery over Traffic channel only is 
 *                              enabled.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetSmsTrafficOnly(swi_uint16* trafficOnly, 
                                          swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_SWINV, SWI_NV_SMS_TRAFFIC_DLVRY, 
            trafficOnly, SWI_AMCDMA, timeout);
}

/**
 *
 * This function enables/disables SMS delivery over Traffic channel only.
 *
 * \param trafficOnly[IN]   SMS delivery over Traffic channel only settings.
 *                          0 - Disable MS delivery over Traffic channel only. 
 *                              Delivery over access channel is 
 *                              always attempted first.
 *                          1 - Enable SMS delivery over Traffic channel only.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetSmsTrafficOnly(swi_uint16 trafficOnly, 
                                          swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_SWINV, SWI_NV_SMS_TRAFFIC_DLVRY, 
            trafficOnly, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets OTKSL Counter. 
 *
 * OTKSL Counter is the number of times that an OTKSL can be used in to manual 
 * user-initiated service provisioning before it is invalidated.
 *
 * \param OTKSLCounter[OUT] OTKSL Counter returned.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetOTKSLCounter(swi_uint16* OTKSLCounter, 
                                        swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_SWINV, SWI_NV_OTKSL_COUNTER, OTKSLCounter, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets OTKSL Counter. 
 *
 * OTKSL Counter is the number of times that an OTKSL can be used in to manual 
 * user-initiated service provisioning before it is invalidated.
 *
 * \param OTKSLCounter[IN]  OTKSL Counter to be set.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetOTKSLCounter(swi_uint16 OTKSLCounter, 
                                        swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_SWINV, SWI_NV_OTKSL_COUNTER, OTKSLCounter, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets OTKSL Max failures.
 *
 * \param OTKSLMaxFailures[OUT] OTKSL Max failures returned.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetOTKSLMaxFailures(swi_uint16* OTKSLMaxFailures, 
                                            swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_SWINV, SWI_NV_SBSDY_LCK_COUNTER, 
            OTKSLMaxFailures, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets OTKSL Max failures.
 *
 * \param OTKSLMaxFailures[IN]  OTKSL Max failures to be set.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetOTKSLMaxFailures(swi_uint16 MaxFailures, 
                                            swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_SWINV, SWI_NV_SBSDY_LCK_COUNTER, 
            MaxFailures, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets create QNC settings.
 *
 * \param createQnc[OUT]    Create QNC settings returned.
 *                          0 - QNC is disabled.
 *                          1 - QNC is enabled.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPriGetCreateQnc(swi_uint16* createQnc, swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_SWINV, SWI_NV_PRI_CREATE_QNC, createQnc, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets create QNC settings.
 *
 * \param createQnc[IN]    Create QNC settings to be set.
 *                          0 - Disable QNC.
 *                          1 - Enabled QNC.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPriSetCreateQnc(swi_uint16 createQnc, swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_SWINV, SWI_NV_PRI_CREATE_QNC, createQnc, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets data warning mask.
 *
 * \param pnDataWarningMask[OUT]    Data warning mask returned.
 *                                  See SWI_TYPE_PRI_DataWarningMask.
 * \param timeout[IN]               Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPriGetDataWarningMask(swi_uint16* pnDataWarningMask, 
                                           swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_SWINV, SWI_NV_PRI_DATA_WARN_MASK, 
            pnDataWarningMask, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets data warning mask.
 *
 * \param pnDataWarningMask[IN] Data warning mask to be set.
 *                              See SWI_TYPE_PRI_DataWarningMask.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPriSetDataWarningMask(swi_uint16 nDataWarningMask, 
                                           swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_SWINV, SWI_NV_PRI_DATA_WARN_MASK, 
            nDataWarningMask, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets SID request setting for Activation Wizard.
 *
 * This is for backward compatibility. SwiPriGetActivationMask() should be
 * used to decide whether SIP page is displayed.
 *
 * \param nSidActivPrompt[OUT]  SID request setting for Activation Wizard.
 *                              0 - Do not show SID page.
 *                              1 - Show SID page.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPriGetSidActivPrompt(swi_uint16* pnSidActivPrompt, 
                                          swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_SWINV, SWI_NV_PRI_SID_ACTIV_PROMPT, 
            pnSidActivPrompt, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets SID request setting for Activation Wizard.
 *
 * \param nSidActivPrompt[IN]   SID request setting for Activation Wizard.
 *                              0 - Do not show SID page.
 *                              1 - Show SID page.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPriSetSidActivPrompt(swi_uint16 nSidActivPrompt, 
                                          swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_SWINV, SWI_NV_PRI_SID_ACTIV_PROMPT, 
            nSidActivPrompt, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets the Qnc request settings for Activation Wizard.
 *
 * This is for backward compatibility. SwiPriGetActivationMask() should be
 * used to decide whether SIP page is displayed.
 *
 * \param pnQncActivPrompt[OUT] QNC request setting for Activation Wizard 
 *                              returned.
 *                              0 - Do not show QNC page.
 *                              1 - Show QNC page.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPriGetQncActivPrompt(swi_uint16* pnQncActivPrompt, 
                                          swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_SWINV, SWI_NV_PRI_QNC_ACTIV_PROMPT, 
            pnQncActivPrompt, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets the Qnc request settings for Activation Wizard.
 *
 * \param nQncActivPrompt[IN]   QNC request setting for Activation Wizard to 
 *                              be set.
 *                              0 - Do not show QNC page.
 *                              1 - Show QNC page.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPriSetQncActivPrompt(swi_uint16 nQncActivPrompt, 
                                          swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_SWINV, SWI_NV_PRI_QNC_ACTIV_PROMPT, 
            nQncActivPrompt, SWI_AMCDMA, timeout);
}

/**
 *
* This function gets the QNC connectoid name.
 *
 * \param pszQncName[OUT]  Buffer that contains QNC connectoid name returned.
 * \param szLength[IN]     Size of the buffer pszQncName.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPriGetConnectoidNameQnc(swi_charp pszQncName, 
                                             swi_uint32 szLength, 
                                             swi_uint32 timeout)
{
    return SwiIntGetNvU16LenString(CNS_PST_SWINV, 
            SWI_NV_QNC_CONNECTOID_NAME_I, pszQncName, szLength, 
            MAXLEN_ConnectoidName, 0, SWI_AMCDMA, timeout);
}

/**
 *
* This function gets the 1x connectoid name.
 *
 * \param psz1xName[OUT]   1x connectoid name
 * \param szLength[IN]     Size of the buffer psz1xName.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPriGetConnectoidName1x(swi_charp psz1xName, 
                                            swi_uint32 szLength, 
                                            swi_uint32 timeout)
{
    return SwiIntGetNvU16LenString(CNS_PST_SWINV, SWI_NV_1X_CONNECTOID_NAME_I, 
            psz1xName, szLength, MAXLEN_ConnectoidName, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets the Activation mask.
 *
 * \param awMask[OUT]       Activation mask returned.
 *                          See SWI_TYPE_PRI_ActivWizMask.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPriGetActivationMask(swi_uint16* awMask, 
                                          swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_SWINV, SWI_NV_PRI_AW_SETTINGS, awMask, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets the MDN Length.
 *
 * \param minLen[OUT]      Minimum MDN length.
 * \param maxLen[OUT]      Maximum MDN length.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPriGetMDNLength(swi_uint16* minLen, 
                                     swi_uint16* maxLen, 
                                     swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint16  nValue;

    resultcode = SwiIntGetNvU16(CNS_PST_SWINV, SWI_NV_PRI_MDN_LEN, &nValue, 
            SWI_AMCDMA, timeout);
    if( resultcode == SWI_RCODE_OK )
    {
        swi_uint16  nMin;
        swi_uint16  nMax;

        nMin = (nValue & 0xff);
        nMax = (nValue >> 8);
        if (nMax >= nMin    &&
            nMin >= 1 && nMin <= MAXLEN_MDN_Length    &&
            nMax >= 1 && nMax <= MAXLEN_MDN_Length)
        {
            *minLen = nMin;
            *maxLen = nMax;
        }
        else
        {
            resultcode = SWI_RCODE_INVALID_PAR;
        }
    }
    
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function gets the GUI Settings
 *
 * \param guiSettings[OUT]  GUI Settings returned.
 *                          See SWI_TYPE_PRI_GUIMask.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPriGetGUISettings(swi_uint16* guiSettings, 
                                       swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_SWINV, SWI_NV_PRI_GUI_SETTINGS, guiSettings, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets the SIP user ID format.
 *
 * \param sipFormat[OUT]    Buffer that contains SIP user ID format.
 * \param szLength[IN]      Size of the buffer sipFormat.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPriGetSIPUserIDFormat(swi_charp sipFormat, 
                                           swi_uint32 szLength, 
                                           swi_uint32 timeout)
{
    return SwiIntGetNvU16LenString(CNS_PST_SWINV, SWI_NV_PRI_SIP_USER_ID_FMT, 
            sipFormat, szLength, MAXLEN_SIPUserIDFormat, 0, 
            SWI_AMCDMA, timeout);
}

/**
 *
* This function gets the OTASP dial string.
 *
 * \param otaspString[OUT]  Buffer that contains OTASP dial string.
 * \param szLength[IN]      Size of the buffer otaspString.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPriGetOTASPDialstring(swi_charp otaspString, 
                                           swi_uint32 szLength, 
                                           swi_uint32 timeout)
{
    return SwiIntGetNvU16LenString(CNS_PST_SWINV, SWI_NV_PRI_OTASP_DIAL, 
            otaspString, szLength, MAXLEN_OTASP_Dialstring, 0, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets the Voicemail dial string
 *
 * \param vmailString[OUT]  Buffer that contains Voicemail dial string.
 * \param szLength[IN]      Size of the buffer vmailString.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPriGetVoicemailDialstring(swi_charp vmailString, 
                                               swi_uint32 szLength, 
                                               swi_uint32 timeout)
{
    return SwiIntGetNvU16LenString(CNS_PST_SWINV, SWI_NV_PRI_VOICEMAIL, 
            vmailString, szLength, MAXLEN_VMail_Dialstring, 0, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function checks whether DMU is Enabled or not.
 *
 * \param nEnableDmu[OUT]   DMU settings returned.
 *                          0 - DMU is disabled.
 *                          1 - DMU is enabled.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPriGetEnableDmu(swi_uint16* nEnableDmu, 
                                     swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_SWINV, SWI_NV_PRI_ENABLE_DMU, nEnableDmu, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets the number of MIP profile in the modem.
 *
 * \param profiles[IN]  Number of MIP profiles to be set.
 * \param timeout[IN]   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetDsMipNumProf(swi_uint16 profiles, 
                                        swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_NV, NV_DS_MIP_NUM_PROF, profiles, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets the number of MIP profile in the modem.
 *
 * \param pProfiles[OUT]   Number of MIP profiles returned.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetDsMipNumProf(swi_uint16* pProfiles, 
                                        swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_NV, NV_DS_MIP_NUM_PROF, pProfiles, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function is used to set the MIP Gen user profile.
 *
 * \param index[IN]           Profile Index.
 * \param mnHaSsLength[IN]    Length of mnHaSs.
 * \param mnHaSs[IN]          Buffer containing mnHaSs.
 * \param mnAaaSsLength[IN]   Length of mnAaaSs.
 * \param mnAaaSs[IN]         Buffer containing mnAaaSs.
 * \param timeout[IN]         Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetDsMipSsUserProf(swi_uint16 index, 
                                           swi_uint16 mnHaSsLength, 
                                           swi_charp mnHaSs, 
                                           swi_uint16 mnAaaSsLength, 
                                           swi_charp mnAaaSs, 
                                           swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    if (mnHaSsLength > LENGTH_NvMnHaSs || mnAaaSsLength > LENGTH_NvMnAaaSs)
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
    parmlength = SwiPstPackDsMipSsUserProf( outparamp, index, mnHaSsLength,
            mnHaSs, mnAaaSsLength, mnAaaSs);

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
 * This function is used to get the MIP Gen user profile.
 *
 * \param index[IN]                Profile Index.
 * \param mnHaSs[OUT]              mnHaSs.
 * \param pmnHaSsLength[OUT]       Pointer to length of mnHaSs.
 * \param mnHaSsBufLength[IN]      mnHaSs buffer Length.
 * \param mnAaaSs[OUT]             mnAaaSs.
 * \param pmnAaaSsLength[OUT]      Pointer to length of mnAaaSs.
 * \param mnAaaSsBufLength[IN]     mnAaaSs buffer length.
 * \param timeout[IN]              Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetDsMipSsUserProf(swi_uint16 index, 
                                           swi_charp mnHaSs, 
                                           swi_uint16* pmnHaSsLength, 
                                           swi_uint16 mnHaSsBufLength, 
                                           swi_charp mnAaaSs, 
                                           swi_uint16* pmnAaaSsLength, 
                                           swi_uint16 mnAaaSsBufLength, 
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
    parmlength = SwiPackNvU16( outparamp, NV_DS_MIP_SS_USER_PROF, index );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PST_NV, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiPstUnpackDsMipSsUserProf( inparamp, mnHaSs, pmnHaSsLength, 
                mnHaSsBufLength, mnAaaSs, pmnAaaSsLength, mnAaaSsBufLength );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function is used to set the timebase difference between the home 
 * agent and the mobile in second.
 *
 * \param index[IN]         Profile Index.
 * \param mnHaDelta[IN]     Timebase difference between the home agent and 
 *                          the mobile in second.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetDsMipMnHaTimeDelta(swi_uint16 index, 
                                              swi_uint32 mnHaDelta, 
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
    parmlength = SwiPstPackDsMipMnHaTimeDelta( outparamp, index, mnHaDelta );

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
 * This function is used to get the timebase difference between the home 
 * agent and the mobile in second.
 *
 * \param index[IN]         Profile Index.
 * \param pMnHaDelta[OUT]   Timebase difference between the home agent and 
 *                          the mobile in second.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetDsMipMnHaTimeDelta(swi_uint16 index, 
                                              swi_uint32* pMnHaDelta, 
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
    paramlength = SwiPackNvU16( outparamp, NV_DS_MIP_MN_HA_TIME_DELTA, index );

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
        swi_uint16 nNvItem;
        swi_uint16 nIndex;

        nNvItem = piget16(&inparamp);
        nIndex = piget16(&inparamp);
        
        if(nNvItem != NV_DS_MIP_MN_HA_TIME_DELTA && nIndex != index)
        {
            resultcode = SWI_RCODE_UNEXPECTED_RESP;
        }
        else
        {
            *pMnHaDelta = piget32(&inparamp);
        }
    }
    
    /* All done, send the result to the caller */
    return resultcode;
}


/**
 *
 * This function is used to get the copy of the MIP general user profile.
 *
 * \param index[IN]                Profile Index.
 * \param naiLength[IN]            Length of NAI.
 * \param nai[OUT]                 NAI.
 * \param pMnHaSpiEnable[OUT]      Mn HA SPI enabled.
 * \param pMnHaSpi[OUT]            Mn HA SPI.
 * \param pMnAaaSpiEnable[OUT]     Mn AAA SPI enabled.
 * \param pMnAaaSpi[OUT]           Mn AAA SPI.
 * \param pReverseTunneling[OUT]   Reverse tunneling.
 * \param pHomeAddress[OUT]        Home address.
 * \param pPrimaryHaAddress[OUT]   Primary HA address.
 * \param pSecondaryHaAddress[OUT] Second HA address.
 * \param timeout[IN]              Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetDsMipGenUserBak(swi_uint16 index, 
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
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Parameter Field Construction Zone - get a pointer to the 
     * location to fill in the CnS parameter field 
     */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Pack the CnS Parameter field with the input parameters */
    parmlength = SwiPackNvU16( outparamp, SWI_NV_DS_MIP_GEN_USER_PROF_BAK, 
            index );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PST_SWINV, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiPstUnpackDsMipGenUserBak( inparamp, naiLength, nai, pMnHaSpiEnable,
                pMnHaSpi, pMnAaaSpiEnable, pMnAaaSpi, pReverseTunneling, 
                pHomeAddress, pPrimaryHaAddress, pSecondaryHaAddress );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function is used to get the copy of the MIP general shared 
 * secret profile.
 *
 * \param index[IN]         Profile Index.
 * \param mnHaSsLength[IN]  Length of Mn Ha Ss.
 * \param mnHaSs[OUT]       Buffer containing MnHaSs.
 * \param mnAaaSsLength[IN] Length of Mn Aaa Ss.
 * \param mnAaaSs[OUT]      Buffer containing MnAaaSs.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetDsMipSsUserBak(swi_uint16 index, 
                                          swi_uint16 mnHaSsLength, 
                                          swi_charp mnHaSs, 
                                          swi_uint16 mnAaaSsLength, 
                                          swi_charp mnAaaSs, 
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
    parmlength = SwiPackNvU16( outparamp, SWI_NV_DS_MIP_SS_USER_PROF_BAK, 
            index );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PST_SWINV, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiPstUnpackDsMipSsUserBak( inparamp, mnHaSsLength, mnHaSs, 
                mnAaaSsLength, mnAaaSs );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function gets the Qnc DialString.
 *
 * \param pszDialString[OUT] Qnc DialString.
 * \param szLength[IN]       Length of dial string.
 * \param timeout[IN]        Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetQncDialString(swi_charp pszDialString, 
                                         swi_uint32 szLength, 
                                         swi_uint32 timeout)
{
    return SwiIntGetNvU16LenString(CNS_PST_SWINV, SWI_NV_PRI_QNC_ORIG_STR, 
            pszDialString, szLength, LENGTH_NvDialString, 0, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets the Qnc DialString.
 *
 * \param pszDialString[IN] Qnc DialString.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetQncDialString(const swi_charp pszDialString, 
                                         swi_uint32 timeout)
{
    return SwiIntSetNvU16LenString(CNS_PST_SWINV, SWI_NV_PRI_QNC_ORIG_STR, 
            pszDialString, LENGTH_NvDialString, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function check whether PRL is locked or not.
 *
 * \param pnPRLLocked[OUT]  1: PRL Locked; unlocked otherwise.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetPRLLocked(swi_uint16* pnPRLLocked, 
                                     swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_SWINV, SWI_NV_UPGR_PROTECT_OPTION, 
            pnPRLLocked, SWI_AMCDMA, timeout);
}

/**
 * This function gets DMU MN Authenticator value.
 * 
 *
 * \param pnMnAuth[OUT]     DMU MN Authenticator value returned.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetDMUMnAuth(swi_uint32 *pnMnAuth, swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          mn_auth[LENGTH_DMU_MN_AUTH];

    resultcode = SwiIntGetU8s(CNS_DMU_MN_AUTH, mn_auth, LENGTH_DMU_MN_AUTH, 
            SWI_AMCDMA, timeout);

    if(SWI_RCODE_OK == resultcode)
    {
        *pnMnAuth = (mn_auth[0]<<16) + (mn_auth[1]<<8) + (mn_auth[2]);
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function resets the DMU MN authentication.
 *
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiResetDMUMnAuth(swi_uint32 timeout)
{
    return SwiIntSet(CNS_DMU_MN_AUTH_RESET, SWI_AMCDMA, timeout);
}

/**
 *
 * This function is used to get the DMU public key index
 *
 * \param pPublicKeyIndex[OUT] DMU public key index.
 * \param timeout[IN]          Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetDMUPublicKeyIndex(swi_uint16* pPublicKeyIndex, 
                                          swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_DMU_PK_INDEX, pPublicKeyIndex, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function is used to set the DMU public key index
 *
 * \param publicKeyIndex[IN] DMU public key index.
 * \param timeout[IN]        Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetDMUPublicKeyIndex(swi_uint16 publicKeyIndex, 
                                          swi_uint32 timeout)
{
    return SwiIntSetU16(CNS_DMU_PK_INDEX, publicKeyIndex, NULL, 
            SWI_AMCDMA, timeout);
}

/**
 *
This function is used to get the CDMA feature key.
 *
 * \param pnFeatureMask[OUT]    Feature mask.
 *                              See SWI_TYPE_FEATURE_MASK.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetCdmaFeaturesKey(swi_uint32* pnFeatureMask, 
                                        swi_uint32 timeout)
{
    return SwiIntGetU32(CNS_FEATURE_KEY_CDMA, pnFeatureMask, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function is used to enable Host software features.
 *
 * \param pszFeatKey[IN]    Input feature key.
 *                          See SWI_TYPE_FEATURE_MASK.
 * \param pnResult[OUT]     Result of the operation.
 *                          0x0000 – Feature successfully enabled
 *                          0x0001 – Unknown feature
 *                          0x0002 – Feature disabled
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetCdmaFeaturesKey(const swi_charp pszFeatKey, 
                                        swi_uint16* pnResult, 
                                        swi_uint32 timeout)
{
    return SwiIntSetString(CNS_FEATURE_KEY_CDMA, pszFeatKey, pnResult, 
            LENGTH_SetFeaturesKey, 0, SWI_AMCDMA, timeout);
}


/**
 *
 * This function is a used to get EFile information.
 *
 * \param pszEFilename[IN] Buffer that contains EFile name.
 * \param psEFileInfo[OUT] Output info about the EFile.
 *                         See SWI_STRUCT_EFileInfo.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetEFileInfo(const swi_charp pszEFilename, 
                                  SWI_STRUCT_EFileInfo* psEFileInfo, 
                                  swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    if(slstrlen(pszEFilename) > LENGTH_EFilename)
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
    parmlength = SwiPackU8s( outparamp, (const swi_uint8 *)pszEFilename,
                             slstrlen(pszEFilename) );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_FILE_NAME, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackEFileInfo( inparamp, psEFileInfo );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/* This is a help function for SwiGetEFile() */
/**
 *
 * This function is used to read one EFile block from the modem.
 *
 * \param pszEFilename[IN] EFile name.
 * \param nSeq[IN]         Sequence of block number (zero based).
 * \param pBuffer[OUT]     Buffer to store the block.
 * \param nBufLen[IN]      Length of the buffer.
 * \param pnBlockSize[OUT] Size of the block returned.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetEFileBlock(const swi_charp pszEFilename, 
                                   swi_uint16 nSeq, 
                                   swi_uint8* pBuffer, 
                                   swi_uint16 nBufLen, 
                                   swi_uint32 *pnBlockSize, 
                                   swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    if(slstrlen(pszEFilename) > LENGTH_EFilename)
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
    parmlength = SwiPackEFileBlock( outparamp, pszEFilename, nSeq );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_FILE_READ, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        resultcode = SwiUnpackEFileBlock(inparamp, nSeq, pBuffer, nBufLen, 
                pnBlockSize);
    }
    
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function reads an EFile into buffer from EFS.
 *
 * \param pszEFilename[IN]  Specified filename.
 * \param pBuffer[OUT]      Buffer to stored the read file.
 * \param nBufLen[IN]       Size of the buffer.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetEFile(const swi_charp pszEFilename, 
                              swi_uint8* pBuffer, 
                              swi_uint16 nBufLen, 
                              swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint16          nSeq;
    swi_uint32          nDataOfs;
    swi_uint32          nBlockSize;
    
    if(slstrlen(pszEFilename) > LENGTH_EFilename || NULL == pBuffer)
    {
        return SWI_RCODE_INVALID_PAR;
    }

    /* Do the standard checks */
    if(!SwiApiRunning())
    {
        return SWI_RCODE_API_NOT_OPEN;
    }

    SWI_STRUCT_EFileInfo sEF;
    sEF.sizeStruct = sizeof(SWI_STRUCT_EFileInfo);
    resultcode = SwiGetEFileInfo(pszEFilename, &sEF, timeout);
    if (SWI_RCODE_OK != resultcode) {
        return resultcode;
    }
    else if (1 != sEF.nResult) {
        return SWI_RCODE_FAILED;
    }
    else if (nBufLen < sEF.nSize) {
        return SWI_RCODE_SMALL_BUF;
    }

    nDataOfs = 0;

    /* Loop reading each sequence block */
    for (nSeq = 0; nSeq < sEF.nBlocks; nSeq++)
    {
        resultcode = SwiGetEFileBlock(pszEFilename, nSeq, pBuffer+nDataOfs, 
                nBufLen-nDataOfs, &nBlockSize, timeout);
        /* Only parse out the response data if we got a positive return */
        if( resultcode == SWI_RCODE_OK )
        {
            nDataOfs += nBlockSize;
        }
    }
    
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function is used to set EFile info in modem.
 *
 * \param pszEFilename[IN] Filename.
 * \param nSize[IN]        Size of file.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetEFileInfo(const swi_charp pszEFilename, 
                                  swi_uint32 nSize, 
                                  swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */
    swi_uint32          szLength;

    szLength = slstrlen(pszEFilename);
    if(szLength < 1 || szLength > LENGTH_EFilename)
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
    parmlength = SwiPackEFileInfo(outparamp, pszEFilename, nSize);

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_FILE_NAME, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */
                     
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        swi_uint16 nResult, nErrorCode;

        nResult = piget16(&inparamp);
        nErrorCode = piget16(&inparamp);

        if (1 != nResult || 0 != nErrorCode)
        {
            resultcode = SWI_RCODE_FAILED;
        }
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function writes the buffer to EFS with given filename.
 *
 * \param pszEFilename[IN] Specified filename.
 * \param pBuffer[IN]      Buffer to be written to file.
 * \param nBufLen[IN]      Length of the buffer.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetEFile(const swi_charp pszEFilename, 
                              swi_uint8* pBuffer, 
                              swi_uint16 nBufLen, 
                              swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */

    resultcode = SwiSetEFileInfo(pszEFilename, nBufLen, timeout);
    if( resultcode != SWI_RCODE_OK )
    {
        return resultcode;
    }

    if ( nBufLen > MAX_EFileBlock )
    {
        swi_uint16 totalBlocks = 0;
        swi_uint16 cntBlocks;
        swi_uint16 i = 0;

        cntBlocks = nBufLen/MAX_EFileBlock;
        totalBlocks = cntBlocks;

        if (0 != (nBufLen%MAX_EFileBlock))
        {
            totalBlocks = cntBlocks + 1;
        }

        /* send intermediate blocks */
        for ( i = 0; i < cntBlocks; i++)
        {
            resultcode = SwiIntSendFileBody(pszEFilename, i, totalBlocks, 
                    MAX_EFileBlock, pBuffer, timeout);
            pBuffer += MAX_EFileBlock;
        }
        resultcode = SwiIntSendFileBody(pszEFilename, i, totalBlocks, 
                nBufLen - i*MAX_EFileBlock, pBuffer, timeout);
    }
    else
    {
        /* send the only one data block */
        resultcode = SwiIntSendFileBody(pszEFilename, 0, 1, nBufLen, pBuffer, 
                timeout);
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
* This function gets the size of the PRL for the specified NAM.
 *
 * \param Nam[IN]          NAM index.
 * \param NumBlocks[OUT]   Number of blocks in PRL.
 * \param Size[OUT]        Size of PRL.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetPRLSize(swi_uint16 Nam, 
                                swi_uint16 *NumBlocks, 
                                swi_uint32 *Size, 
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
    paramlength = SwiPackU16( outparamp, Nam );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PRL_SIZE, 
                                  paramlength,
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &paramlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackPRLSize( inparamp, NumBlocks, Size );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function sets the size of the PRL for the specified NAM.
 *
 * \param Nam[IN]          NAM index.
 * \param nSize[IN]        specified size.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetPRLSize(swi_uint16 Nam, 
                                swi_uint32 nSize, 
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
    parmlength = SwiPackPRLSize( outparamp, Nam, nSize );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PRL_SIZE, 
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
 * This function checks whether PRL for the given NAM is valid or not.
 *
 * \param Nam[IN]          NAM index.
 * \param Validity[OUT]    if 1 then valid; otherwise invalid
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetPRLValid(swi_uint16 Nam, 
                                 swi_uint16 *Validity, 
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
    paramlength = SwiPackU16( outparamp, Nam );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PRL_VALID, 
                                  paramlength,
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &paramlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        
        *Validity= piget16(&inparamp);    /* Discard NV item */
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function is used to get the PRL from the modem.
 *
 * \param NamIndex[IN]     The NAM index.
 * \param pBuffer[OUT]     The buffer were returned PRL will be stored.
 * \param nBufLen[IN]      The size of the buffer.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetPRL(swi_uint16 NamIndex, 
                            swi_uint8* pBuffer, 
                            swi_uint16 nBufLen, 
                            swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         paramlength; /* Returned length of the CnS parm field */

    swi_uint16 NumBlocks;
    swi_uint32 nSize;
    swi_uint16 nSeq;
    swi_uint16 array[2];
    swi_uint32 nDataOfs;

    swi_uint16 nRecievedBlockSeq;
    swi_uint16 nRecievedBlockSize;

    resultcode = SwiGetPRLSize(NamIndex, &NumBlocks, &nSize, timeout);
    if (SWI_RCODE_OK != resultcode)
    {
        return resultcode;
    }
    else if (nBufLen < nSize) 
    {
        return SWI_RCODE_SMALL_BUF;
    }

    nDataOfs = 0;
    slmemset((char *)pBuffer, 0, nBufLen); 

    /* Loop reading each sequence block */
    for (nSeq = 0; nSeq < NumBlocks; nSeq++)
    {
        /* Initialize the pointer to the outgoing request buffer pointer */
        reqbufferp = cngetreqbkp();
        
        /* Parameter Field Construction Zone - get a pointer to the 
         * location to fill in the CnS parameter field 
         */
        outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );
        
        /* Pack the CnS Parameter field with the input parameters */
        array[0] = NamIndex;
        array[0] = nSeq;
        paramlength = SwiPackU16s( outparamp, array, 
                sizeof(array)/sizeof(array[0]) );
        
        /* Prepare and send the blocking call */
        resultcode = SwiIntSendnWait( reqbufferp, 
                                      CNS_PRL_READ, 
                                      paramlength,
                                      timeout, 
                                      CNS_GETREQ,
                                      SWI_AMCDMA, 
                                      &inparamp,
                                      &paramlength );
        
        /* Only parse out the response data if we got a positive return */
        if( resultcode == SWI_RCODE_OK )
        {
            piget16(&inparamp);     /* Discard NAM index */
            nRecievedBlockSeq= piget16(&inparamp);
            nRecievedBlockSize= piget16(&inparamp);

            if(nSeq != nRecievedBlockSeq || 
                    nBufLen < nRecievedBlockSize+nDataOfs )
            {
                resultcode = SWI_RCODE_FAILED;
                break;
            }
            else
            {
                pigetmulti(&inparamp, pBuffer+nDataOfs, nRecievedBlockSize);
                nDataOfs += nRecievedBlockSize;
            }
        }
    }
    
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function is a force the Data call to be dormant.
 *
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetDormant(swi_uint32 timeout)
{
    return SwiIntSet(CNS_SB_GO_DORMANT, SWI_AMCDMA, timeout);
}

/**
 *
 * This function is used to set TCXOMode.
 *
 * \param nInterfaceMode[IN]  Interface mode.
 *                            See SWI_INTERFACE_MODE.
 * \param timeout[IN]         Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetTCXOMode(SWI_INTERFACE_MODE nInterfaceMode, 
                                 swi_uint32 timeout)
{
    return SwiIntSetU16(CNS_SB_TCXO_CTL_MODE, nInterfaceMode, NULL, 
            SWI_AMCDMA, timeout);
}

/**
 *
 * This function is used to find out Acquired Network.
 *
 * \param lAcquired[OUT]   The Network acquired.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiEVDONetworkAcquired(swi_int32* lAcquired, 
                                         swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8 nAcquired;
    
    resultcode = SwiIntGetU8(CNS_HDR_ACQ, &nAcquired, SWI_AMCDMA, timeout);
    if( SWI_RCODE_OK == resultcode )
    {
        *lAcquired = nAcquired;
    }
    
    return resultcode;
}

/**
 *
 * This function is used to get EVDO Uati.
 *
 * \param lUati[OUT]       Pointer to buffer for result of Get Uati.
 * \param Size[IN]         Size of the buffer.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiEVDOGetUati(swi_uint8* lUati, 
                                 swi_uint32 Size, 
                                 swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         paramlength; /* Returned length of the CnS parm field */

    if(SWI_EVDO_UATI_SIZE > Size)
    {
        return SWI_RCODE_SMALL_BUF;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Normally a Pack function call would go here, but this 
     * request has no CnS Parameter field to pack
     */

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_HDR_UATI, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &paramlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiEVDOUnpackUati( inparamp, lUati, Size);
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function is used to reset the Data statistic in modem.
 *
 * \param eProtocol[IN]     Protocol information.
 *                          See SWI_TYPE_LinkProtocol.
 * \param pResetResult[OUT] Result of data statistic reset.
 *                          See SWI_TYPE_DataStatsResetResult.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiDataStatsReset(swi_uint16 eProtocol, 
                                    SWI_STRUCT_DataStatsReset *pResetResult, 
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
    parmlength = SwiPackDataStatsReset( outparamp, eProtocol );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_DATA_STATS_RESET, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */
                     
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackDataStatsReset( inparamp, pResetResult );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function is used for getting statistics.
 *
 * \param eProtocol[IN]        Protocol information.
 *                             See SWI_TYPE_LinkProtocol.
 * \param ePppSubProtocol[IN]  Sub protocol information.
 *                             see SWI_TYPE_PppSubProtocol
 * \param pDataStats[OUT]      Fetched data statistics information
 *                             see SWI_STRUCT_DataStatsFetch
 * \param timeout[IN]          Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiDataStatsFetch(swi_uint16 eProtocol, 
                                    swi_uint16 ePppSubProtocol, 
                                    SWI_STRUCT_DataStatsFetch *pDataStats, 
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
    parmlength = SwiPackDataStatsFetch( outparamp, eProtocol, ePppSubProtocol);

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_DATA_STATS_FETCH, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */
                     
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackDataStatsFetch( inparamp, pDataStats );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function gets the Aupto Populate info from the modem.
 *
 * \param nIndex[IN]       NAM index.
 * \param pData[OUT]       Auto populate information.
 *                         See nv_auto_populate_info_ap_type.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstGetAutoPopulateInfoAp(
        swi_uint16 nIndex, 
        nv_auto_populate_info_ap_type *pData, 
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
    paramlength = SwiPackNvU16( outparamp, NV_AUTO_POPULATE_INFO_AP_I, nIndex);

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
        SwiPstUnpackAutoPopulateInfoAp( inparamp, pData );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function sets the Aupto Populate info into the modem.
 *
 * \param pData[IN]        Auto populate information.
 *                         See nv_auto_populate_info_ap_type.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPstSetAutoPopulateInfoAp(
        const nv_auto_populate_info_ap_type *pData, 
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
    parmlength = SwiPstPackAutoPopulateInfoAp( outparamp, pData );

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
 * This function gets the device EVDO scp settings.
 *
 * New devices support this feature with NV_HDRSCP_FORCE_REL0_CONFIG_I NV item,
 * while old devices support thie feature with  
 * NV_HDRSCP_SUBTYPE_CUSTOM_CONFIG_I NV item. This function first tries to read
 * NV_HDRSCP_FORCE_REL0_CONFIG_I, if failed then tries to read
 * NV_HDRSCP_SUBTYPE_CUSTOM_CONFIG_I. a global flag is set according to which 
 * NV is valie. Application must call this function once in order to have 
 * SwiSetEVDOSCPSettings function write correct NV.
 *
 * \param pnCustCfgActive[OUT]  Custom configuration active or not.
                                0: not active; otherwise active.
 * \param pnProtMask[OUT]       Custom configuration parameter.
 * \param pnBCMCSMask[OUT]      Custom configuration parameter.
 * \param pnAppMask[OUT]        Custom configuration parameter.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetEVDOSCPSettings(swi_uint16* pnCustCfgActive, 
                                        swi_uint32 *pnProtMask, 
                                        swi_uint32 *pnBCMCSMask, 
                                        swi_uint32 *pnAppMask, 
                                        swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */


    resultcode = SwiIntGetNvU16(CNS_PST_NV, NV_HDRSCP_FORCE_REL0_CONFIG_I, 
            pnCustCfgActive, SWI_AMCDMA, timeout);
    if( resultcode == SWI_RCODE_OK )
    {
        gbUsesSCP = FALSE;
    }
    else
    {
        SWI_STRUCT_HDRSCP_Subtype_Custom_Config scp;
        resultcode = SwiGetHdrScpCustomerConfig(&scp, timeout);
        if( resultcode == SWI_RCODE_OK )
        {
            gbUsesSCP = TRUE;
            *pnCustCfgActive = scp.bIsActive;
            *pnProtMask = scp.nProtSubtypeMask;
            *pnBCMCSMask = scp.nBcmcsSubtypeMask;
            *pnAppMask = scp.nAppSubtypeMask;
        }
    }

    return resultcode;
}

/**
 *
 * This function sets the device EVDO scp settings.
 *
 * New devices support this feature with NV_HDRSCP_FORCE_REL0_CONFIG_I NV item,
 * while old devices support thie feature with  
 * NV_HDRSCP_SUBTYPE_CUSTOM_CONFIG_I NV item. This function set correct NV 
 * according to the global flag set during calling SwiGetEVDOSCPSettings.
 *
 * \param pnCustCfgActive[IN]   Custom configuration active or not.
                                0: not active; otherwise active.
 * \param pnProtMask[IN]        Custom configuration parameter.
 * \param pnBCMCSMask[IN]       Custom configuration parameter.
 * \param pnAppMask[IN]         Custom configuration parameter.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetEVDOSCPSettings(swi_uint16 nCustCfgActive, 
                                        swi_uint32 nProtMask, 
                                        swi_uint32 nBCMCSMask, 
                                        swi_uint32 nAppMask, 
                                        swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */

    if(FALSE == gbUsesSCP)
    {
        resultcode = SwiIntSetNvU16(CNS_PST_NV, NV_HDRSCP_FORCE_REL0_CONFIG_I, 
                nCustCfgActive, SWI_AMCDMA, timeout);
    }
    else
    {
        SWI_STRUCT_HDRSCP_Subtype_Custom_Config scp;

        scp.sizeStruct = sizeof(SWI_STRUCT_HDRSCP_Subtype_Custom_Config);
        scp.bIsActive = nCustCfgActive;
        scp.nProtSubtypeMask = nProtMask;
        scp.nBcmcsSubtypeMask = nBCMCSMask;
        scp.nAppSubtypeMask = nAppMask;
        
        resultcode = SwiSetHdrScpCustomerConfig(&scp, timeout);
    }

    return resultcode;
}


/* 
 * $Log: SwiIntCdma.c,v $
 */

