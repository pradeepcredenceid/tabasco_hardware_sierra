/**
 *    \if CvsId 
 *    $Id: SwiApiCdmaVoice.c,v 1.3 2010/06/10 00:12:30 epasheva Exp $
 *    \endif
 *    \ingroup cdmavoice
 *    \file SwiApiCdmaVoice.c
 * 
 *    \brief SWI CDMA specific Voice related APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#include "SwiApiCdmaVoice.h"
#include "SwiIntPkCdmaVoice.h"
#include "ap/apidefs.h"

/**
 *
 * This function reports the mode the modem will use to answer
 * the next incoming call.
 *
 * \param AnswerMode[OUT]  The destination in which the mode is returned.
 *                         See SWI_TYPE_AnswerState.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetAnswerState(swi_uint16* AnswerMode, swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_ANSWER_STATE, AnswerMode, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets the mode the modem will use to answer the next
 * incoming call.
 *
 * \param AnswerMode[IN]   The mode setting to make. See SWI_TYPE_AnswerState.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetAnswerState(swi_uint16 AnswerMode)
{
    return SwiIntSetU16(CNS_ANSWER_STATE,  AnswerMode, NULL, SWI_AMCDMA,
                        APTIMEOUT);
}

/**
 *
 * This function returns the standard caller ID for
 * the current incoming call (if supported by the service provider)
 *
 * \param szCallerID[OUT]  The destination in which the callerID string
                           is returned.
 * \param iLength[IN]      The maximum size of the output
 *                         string (pointed to by szCallerID)
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetCallerID(swi_charp szCallerID, swi_uint32 iLength,
                                 swi_uint32 timeout)
{
    return SwiIntGetString(CNS_CALL_CALLER_ID, szCallerID, iLength,
                           0, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function is is used to answer an incoming call.
 *
 * \return  See SwiRcodes.h
 *          SWI_RCODE_OK    The function executed normally. This does 
 *                          not mean that the call was successfully 
 *                          answered. Use call notifications
 *                          for the status of the call connection.
 *                          See also SwiGetCallConnectionState.    
 *
 * \Notes      Don’t call this unless you have detected an incoming call 
 *             request (ring) via SWI_NOTIFY_CallIncoming.
 *
 *             The mode the modem will use to answer the call
 *             is set by SwiSetAnswerState that must have been
 *             set prior to receiving the incoming call notification.    
 *
 */
SWI_API SWI_RCODE SwiSetCallAnswerCmd()
{
    return SwiIntSet(CNS_CALL_ANSWER_CMD, SWI_AMCDMA, APTIMEOUT);
}

/**
 *
 * This function sends a flash with information to the base station.
 *
 * \param SOValue[IN]       Service option to request from base station.
 *                          See SWI_TYPE_FlashContext.
 * \param pszCallNumber[IN] Phone number or feature code; can be null 
 *                          if a call number is not required.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetCallFlashCmd(swi_uint16 SOValue,
                                     const swi_charp pszCallNumber,
                                     swi_uint32 timeout)
{
    return SwiIntSetU16String(CNS_CALL_FLASH_CMD, SOValue, pszCallNumber,
                              LENGTH_CallNumber, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function reports whether or not the headset is attached.
 *
 * \param headsetState[OUT] The destination in which the state is returned:
 *                          0 = unplugged (removed)
 *                          1 = attached (inserted)
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetHeadsetState(swi_uint16 *headsetState, 
                                     swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_HEADSET_ICON_LEGACY, headsetState, SWI_AMCDMA,
                        timeout);
}

/**
 *
 * This function reports the volume level of the speaker (headset output).
 *
 * \param pSpeakerVolume[OUT]   The destination in which the
 *                              volume level is returned:
 *                              0 = muted (see remarks)
 *                              1 = quietest
 *                              6 = loudest
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetCdmaSpeakerVolume(swi_uint16* pSpeakerVolume,
                                          swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_SPEAKER_VOL, pSpeakerVolume, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets the volume level of the speaker (headset output).
 *
 * \param SpeakerVolume[IN] The volume level to set:
 *                          0 = mute (see remarks)
 *                          1 = quietest
 *                          6 = loudest
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetCdmaSpeakerVolume(swi_uint16 SpeakerVolume,
                                          swi_uint32 timeout)
{
    return SwiIntSetU16(CNS_SPEAKER_VOL,  SpeakerVolume, NULL,
                        SWI_AMCDMA, timeout);
}

/**
 *
 * This function reports the status of the speaker mute setting.
 *
 * \param pSpeakerMuteStatus[OUT]  The destination in which the mute state
 *                                 is returned:
 *                                 0 = speaker on
 *                                 1 = muted (speaker off)
 * \param timeout[IN]              Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetSpeakerMuteStatus(swi_uint16* pSpeakerMuteStatus,
                                          swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_SPEAKER_MUTE, pSpeakerMuteStatus, SWI_AMCDMA,
                        timeout);
}

/**
 *
 * This function sets the mute status of the speaker (headset output).
 *
 * \param SpeakerMuteStatus[IN] The mute state to set:
 *                              0 = speaker on
 *                              1 = mute (speaker off)
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetSpeakerMuteStatus(swi_uint16 SpeakerMuteStatus,
                                          swi_uint32 timeout)
{
    return SwiIntSetU16(CNS_SPEAKER_MUTE,  SpeakerMuteStatus, NULL, 
                        SWI_AMCDMA, timeout);
}

/**
 *
 * This function reports the volume level of DTMF tones sent to the 
 * speaker (headset output).
 *
 * \param pToneLevel[OUT]  The destination in which the tone level is returned:
 *
 * \param timeout[IN]       Function timeout in milliseconds.
 *                          0 = muted (see remarks)
 *                          1 = quietest
 *                          6 = loudest
 * \return  See SwiRcodes.h
 *
 * \Notes       The level can be set using SwiSetToneLevel. The
 *              level setting is retained across resets and power cycles.
 *
 *              There are separate tone (SwiGetDTMFMuteStatus) and speaker 
 *              (SwiGetSpeakerMuteStatus) mute settings that do not alter the 
 *              level reported here.
 *
 *              If the speaker mute is on, DTMF tones are not
 *              heard regardless of the tone level or DTMF mute status.
 *
 *              If the tone level reported is 0, no DTMF tones are heard on the 
 *              speaker regardless of the settings of the mute states.
 *
 */
SWI_API SWI_RCODE SwiGetToneLevel(swi_uint16* pToneLevel, swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_TONE_LEVEL, pToneLevel, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets the volume level of DTMF tones sent to the
 * speaker (headset output).
 *
 * \param ToneLevel[IN]     The volume level to set:
 *                          0 = mute (see remarks)
 *                          1 = quietest
 *                          6 = loudest
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 * \Notes       The level setting is retained across resets and power cycles.
 *
 *              A normal practice is to step the volume up or down, one step 
 *              at a time. The current tone level setting can be read using 
 *              SwiGetToneLevel.
 *              There are separate tone (SwiSetDTMFMuteStatus) and speaker 
 *              (SwiGetSpeakerMuteStatus) mute settings that do not alter 
 *              the level reported here.
 *
 *              If the speaker mute is on, DTMF tones are not heard 
 *              regardless of the tone level or DTMF mute status.
 *
 *              Use the DTMF mute state to enable or disable
 *              DTMF tones on the speaker.    
 *
 *
 */
SWI_API SWI_RCODE SwiSetToneLevel(swi_uint16 ToneLevel, swi_uint32 timeout)
{
    return SwiIntSetU16(CNS_TONE_LEVEL, ToneLevel, NULL, SWI_AMCDMA, timeout);
}

/**
 *
 * This function reports the status of the DTMF tone mute setting.
 *
 * \param pDTMFMuteStatus[OUT]    The destination in which the
 *                                mute state is returned:
 *                                0 = DTMF level enabled
 *                                1 = muted
 * \param timeout[IN]             Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 * \Notes       This mute status is distinct from both the tone level of zero 
 *              (muted), and the state of the speaker mute. The volume level 
 *              set with SwiSetToneLevel is retained regardless of the
 *              setting of the mute status.
 *
 *              The mute state is controlled with SwiSetDTMFMuteStatus. 
 *              When muted, tones are not sent to the speaker. When not muted, 
 *              tones may still not be heard if the speaker mute is on.
 *
 *              The mute setting is not retained across resets and
 *              power cycles. The modem does retain the setting
 *              of DTMF mute during the session (across multiple calls).    
 */
SWI_API SWI_RCODE SwiGetDTMFMuteStatus(swi_uint16* pDTMFMuteStatus,
                                       swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_DTMF_MUTE, pDTMFMuteStatus, SWI_AMCDMA, timeout);
}

/**
 *
 * This function sets the mute status of DTMF tones.
 *
 * \param DTMFMuteStatus[IN]     The mute state to set:
 *                               0 = DTMF level enabled
 *                               1 = muted
 * \param timeout[IN]            Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 * \Notes       This mute status is distinct from a volume level
 *              of zero (muted). The volume level set with
 *              SwiSetToneLevel is retained regardless of the
 *              setting of the mute status.
 *
 *              The mute state can be read with SwiGetDTMFMuteStatus.
 *
 *              The mute setting is not retained across resets and
 *              power cycles. The modem does retain the setting
 *              of DTMF mute during the session (across multiple calls). 
 */
SWI_API SWI_RCODE SwiSetDTMFMuteStatus(swi_uint16 DTMFMuteStatus,
                                       swi_uint32 timeout)
{
    return SwiIntSetU16(CNS_DTMF_MUTE,  DTMFMuteStatus, NULL, SWI_AMCDMA,
                        timeout);
}

/**
 *
 * This function reports the status of the microphone
 * (headset input) mute setting.
 *
 * \param pMICMuteStatus[OUT]    The destination in which the
 *                               mute state is returned:
 *                               0 = MIC enabled
 *                               1 = muted
 * \param timeout[IN]            Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 * \Notes       The mute state is controlled with SwiSetMICMuteStatus.
 *
 *              The mute setting is automatically disabled (MIC
 *              enabled) when a call is disconnected.    
 */
SWI_API SWI_RCODE SwiGetMICMuteStatus(swi_uint16* pMICMuteStatus,
                                      swi_uint32 timeout)
{
    return SwiIntGetU16(CNS_MIC_MUTE_CDMA, pMICMuteStatus, SWI_AMCDMA,
                        timeout);
}

/**
 *
 * This function sets the mute status of the microphone (headset input).
 *
 * \param MICMuteStatus[IN]    The mute state to set:
 *                             0 = MIC enabled
 *                             1 = muted
 * \param timeout[IN]          Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 * \Notes       The mute state can be read with SwiGetMICMuteStatus.
 *
 *              The mute setting is automatically disabled (MIC
 *              enabled) when a call is disconnected.    
 */
SWI_API SWI_RCODE SwiSetMICMuteStatus(swi_uint16 MICMuteStatus,
                                      swi_uint32 timeout)
{
    return SwiIntSetU16(CNS_MIC_MUTE_CDMA,  MICMuteStatus, NULL,
                        SWI_AMCDMA, timeout);
}

/**
 *
 * This function generates a single DTMF key tone.
 *
 * \param DTMFKey[IN]      The enumerated type for the key tone to generate.
 *                         NOTE: Only the first 12 entries in
 *                         the data type are valid for this
 *                         function: #, *, 0-9. See SWI_TYPE_PhoneKey.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 * \Notes       The duration of the tone is determined by SwiSetDTMFDuration.  
 */
SWI_API SWI_RCODE SwiSetDTMFKey(swi_uint16 DTMFKey, swi_uint32 timeout)
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
    parmlength = SwiPackDTMFKey( outparamp, DTMFKey );
    
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_DTMF_KEY, 
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
 * This function reports the settings for the duration
 * of DTMF tone generation for both single key 
 * events and multi-key strings.
 *
 * \param pDTMFKeyOn[OUT]               On duration for single key events.
 *                                      100-3000 ms.
 * \param pDTMFBurstDigitOn[OUT]        On duration for multi-key strings.
 *                                      95-350 ms.
 * \param pDTMFBurstInterdigitOff[OUT]  Off duration between keys in 
 *                                      multi-key strings.60-200 ms.
 * \param timeout[IN]                   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 * \Notes       The values for tone durations are set with SwiSetDTMFDuration. 
 */
SWI_API SWI_RCODE SwiGetDTMFDuration(   swi_uint16* pDTMFKeyOn, 
                                     swi_uint16* pDTMFBurstDigitOn,
                                     swi_uint16* pDTMFBurstInterdigitOff,
                                     swi_uint32 timeout)
{
    swi_uint16 array[3];
    SWI_RCODE rc;

    rc = SwiIntGetU16s(CNS_CONT_DTMF_MODE, array, 3, SWI_AMCDMA, timeout);
    if(SWI_RCODE_OK == rc)
    {
        *pDTMFKeyOn = array[0];
        *pDTMFBurstDigitOn = array[1];
        *pDTMFBurstInterdigitOff = array[2];
    }

    return rc;
}

/**
 *
 * This function sets the durations for DTMF tone generation.
 *
 * \param DTMFKeyOn[IN]                 On duration for single key events.
 *                                      100-3000 ms.
 * \param DTMFBurstDigitOn[IN]          On duration for multi-key strings.
 *                                      95-350 ms.
 * \param DTMFBurstInterdigitOff[IN]    Off duration between keys in 
 *                                      multi-key strings.60-200 ms.
 * \param timeout[IN]                   Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 * \Notes       The settings can be read using SwiGetDTMFDuration.  
 */
SWI_API SWI_RCODE SwiSetDTMFDuration(   swi_uint16 DTMFKeyOn, 
                                     swi_uint16 DTMFBurstDigitOn,
                                     swi_uint16 DTMFBurstInterdigitOff,
                                     swi_uint32 timeout)
{
    swi_uint16 array[] = { DTMFKeyOn, DTMFBurstDigitOn,
                          DTMFBurstInterdigitOff };
    return SwiIntSetU16s(CNS_DTMF_DURATION, array, 3, NULL,
                         SWI_AMCDMA, timeout);
}

/**
 *
 * This function causes the modem to play audible
 * DTMF key tones to the speaker (headset output).
 *
 * \param phoneKey[IN]     The enumerated type for the key
 *                         tone to generate. See documentation.
 *                         NOTE: Only the first 12 entries in
 *                         the data type are valid for this
 *                         function: #, *, 0-9. See SWI_TYPE_PhoneKey.
 *
 * \return  See SwiRcodes.h
 *
 * \Notes      The tone is sent to the headset but not over the
 *              air. This is a human feedback for button pressing
 *              on a software keypad. The tone continues to
 *              sound until SwiSetKeyReleased is called.
 */
SWI_API SWI_RCODE SwiSetKeyPressed(swi_uint16 phoneKey)
{
    return SwiIntSetU16(CNS_KEY_PRESSED,  phoneKey, NULL, SWI_AMCDMA,
                        APTIMEOUT);
}

/**
 *
 * This function stops a DTMF key tone playback in
 * the headset that was initiated with SwiSetKeyPressed.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetKeyReleased()
{
    return SwiIntSet(CNS_KEY_RELEASED, SWI_AMCDMA,
                     APTIMEOUT);
}

/**
 *
 * This function reports the state of the setting for
 * generating an audible beep shortly before the
 * one minute intervals of a voice call.
 *
 * \param callBeep[OUT]     The destination in which the
 *                          beep setting is returned:
 *                          0 = disabled
 *                          1 = enabled
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 * \Notes       The value is set using SwiPstSetMinuteCallBeep.
 */
SWI_API SWI_RCODE SwiPstGetMinuteCallBeep(swi_uint16* callBeep,
                                          swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_NV, NV_CALL_BEEP, callBeep,
                          SWI_AMCDMA, timeout);
}

/**
 *
 * This function enables or disables the playing of a
 * beep tone just before the one minute intervals of a voice call.
 *
 * \param callBeep[IN]     The beep setting to use:
 *                         0 = disabled
 *                         1 = enabled
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 * \Notes       The setting can be read using SwiPstGetMinuteCallBeep.
 */
SWI_API SWI_RCODE SwiPstSetMinuteCallBeep(swi_uint16 callBeep,
                                          swi_uint32 timeout)
{
    return SwiIntSetNvU16(CNS_PST_NV, NV_CALL_BEEP, callBeep, SWI_AMCDMA,
                          timeout);
}

/**
 *
 * This function sets the DDTM Pref setting.
 *
 * \param iDDTMPref[IN]    HDR Hybrid Pref setting
 *                         0 = OFF
 *                         1 = ON
 *                         2 = No Change
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetDDTMPref(swi_uint16 iDDTMPref, swi_uint32 timeout)
{
    return SwiIntSetU16(CNS_DDTM_PREF,  iDDTMPref, NULL, SWI_AMCDMA, timeout);
}

/**
 *
 * This function is used to delete voicemail SMS on the modem.
 *
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiClearVoicemail(swi_uint32 timeout)
{
    return SwiIntSet(CNS_SMS_VMN_MWI_CLEAR, SWI_AMCDMA, timeout);
}

/* 
 * $Log: SwiApiCdmaVoice.c,v $
 */
