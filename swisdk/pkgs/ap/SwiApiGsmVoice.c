/**
 *
 * \if CvsId
 * $Id: SwiApiGsmVoice.c,v 1.9 2009/04/24 23:52:20 bsiu Exp $
 * \endif
 * \ingroup gsmvoice
 * \file SwiApiGsmVoice.c
 *
 * \brief  Contains source code for GSM APIs related to Voice
 *
 * Copyright: © 2009 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "SwiApiGsmVoice.h"
#include "ap/apidefs.h"
#include "sl/sludefs.h"

/**
 *
 * This function is used to retrieve the speaker volume.  The speaker volume
 * setting is stored on the modem.
 *
 * The value returned is the percentage of full volume. This 
 * function should be used every time the host application is started 
 * to restore the previous setting.
 *
 * \param [OUT] sSpeakerVolume:        
 *        Speaker volume
 *
 * \param [IN] timeout:              
 *        Response timeout value in milliseconds.
 *
 * \return          SWI_RCODE_OK: 
 *         The request is being processed.
 *
 * \return SWI_RCODE_REQUEST_TIMEOUT
 *         The request has timed out
 *
 * \return See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_GSM_SpeakerVolume
 * \sa SwiSetGsmSpeakerVolume
 *
 */
SWI_API SWI_RCODE SwiGetGsmSpeakerVolume(
    struct SWI_STRUCT_GSM_SpeakerVolume *sSpeakerVolume, swi_uint32 timeout)
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
                                  CNS_SPKR_VOL, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        sSpeakerVolume->nVolume = (swi_uint32)piget16_noinc(&inparamp);
    }

    /* All done, send the result to the caller */
    return resultcode;
}


/**
 *
 * This function is used to set the speaker volume.  The speaker volume 
 * is stored on the modem.
 *
 * The value used in this function is the percentage of full volume. 
 * This is stored in NVRAM.  The host application may present a few 
 * volume levels that are mapped to specific percentage levels. For 
 * example, the application might have six volume levels where: 
 * 1 = 17%, 2 = 33%, 3 = 50%, 4 = 66%, 5 = 83%, and 6 = 100%.
 *          
 * \param [IN] sSpeakerVolume:            
 *        Speaker volume
 *
 * \param [IN] timeout:              
 *        Response timeout value in milliseconds.
 *
 * \return          SWI_RCODE_OK: 
 *         The request is being processed.
 *
 * \return SWI_RCODE_REQUEST_TIMEOUT
 *         The request has timed out
 *
 * \return See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_GSM_SpeakerVolume
 * \sa SwiGetGsmSpeakerVolume
 *
 */
SWI_API SWI_RCODE SwiSetGsmSpeakerVolume(
    struct SWI_STRUCT_GSM_SpeakerVolume *sSpeakerVolume, swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *outparamp; /* ptr to outbound param field */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */
    swi_uint16         tempVolume;
    
    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    tempVolume = (swi_uint16)sSpeakerVolume->nVolume;
    
    /* pack outgoing parameters */
    parmlength = sizeof(swi_uint16);
    piput16(tempVolume, &outparamp);

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_SPKR_VOL, 
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
 * This function is used to mute the microphone. The microphone mute
 * setting is stored on the modem.
 *
 * A request is sent to the modem. The modem's response indicates that
 * the request has been processed. 
 *
 * This function does not affect the microphone volume setting, it only
 * enables or disables the muting effect. Settings made with this 
 * function are lost when the modem resets. The setting is not lost when 
 * switching between calls. The application should ensure that the 
 * microphone is on when the user switches between calls.
 *
 * \param  pMicMute [IN]:
 *         Microphone Mute setting: 
 *         0 - Mic is not muted
 *         1 - Mic is muted
 *
 * \param  timeout [IN]:
 *         Response timeout value in milliseconds.
 *
 * \return SWI_RCODE_OK: 
 *         The request is being processed
 * \return SWI_RCODE_REQUEST_TIMEOUT
 *         The request has timed out
 *
 * \sa See SWI_RCODE for other error return codes.
 *
 */
SWI_API SWI_RCODE SwiSetMicMute(
    struct SWI_STRUCT_MicMute *pMicMute, swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *outparamp; /* ptr to outbound param field */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */
    swi_uint16         MuteValue; /* temporary holder forthe mute value */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    if (pMicMute->bMute)
    {
        MuteValue = 1;
    }
    else
    {
        MuteValue = 0;
    }

    /* pack outgoing parameters */
    parmlength = sizeof(swi_uint16);
    piput16(MuteValue, &outparamp);

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_MIC_MUTE, 
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
 * This function is used to fetch the current mute status of the microphone. 
 *
 * A request is sent to the modem. The modem's response contains the modem's
 * mute setting.
 *
 * Use this function every time the application is started to determine
 * the state of the microphone's mute setting.
 *
 * \param  pMicMute [OUT]: Microphone Mute setting: 
 *                         0 - Mic is not muted
 *                         1 - Mic is mute
 * \param  timeout [IN]: Response timeout value in milliseconds.
 *
 * \return SWI_RCODE_OK     The mute status has been successfully returned
 * \return SWI_RCODE_REQUEST_TIMEOUT    The request has timed out
 *
 * \sa See SWI_RCODE for other error return codes.
 *
 */
SWI_API SWI_RCODE SwiGetMicMute(
    struct SWI_STRUCT_MicMute *pMicMute, swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */
    swi_uint16         MuteValue; /* temporary holder of the Mute value */
    
    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* no outgoing parameters */
    parmlength = 0;

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_MIC_MUTE, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        MuteValue = piget16(&inparamp);
        if (MuteValue == 1)
        {
            pMicMute->bMute = TRUE;
        }
        else 
        {
            pMicMute->bMute = FALSE;
        }
       
    }

    /* All done, send the result to the caller */
    return resultcode;
}


/**
 *           This function is used to mute the speaker.
 *           The speaker mute setting is stored on the modem.
 *
 *           A request is sent to the modem.  The modem's response indicates that
 *           the request has been processed.
 *
 *           The caller must specify the amount of time to wait for the modem
 *           to respond to the request.    
 * 
 * \param    pSpeakerMute[IN]:   speaker mute setting
 *
 * \param    timeout[IN]:        response timeout value in milliseconds
 *
 * \return   SWI_RCODE_OK - the request has been received by the modem
 * \return   SWI_RCODE_REQUEST_TIMEOUT  - the  request has timed out
 * \return   See SWI_RCODE for other error return codes
 *
 * \note     This function does not impact speaker volume but only enables and 
 *           disables muting. Settings made with this function are lost when the
 *           modem resets. The setting is not lost when switching between calls. 
 *           The application should ensure that the speaker is on when the user
 *           switches between calls.
 *
 */
SWI_API SWI_RCODE SwiSetSpeakerMute(
    struct SWI_STRUCT_GSM_SpeakerMute *pSpeakerMute, swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *outparamp; /* ptr to outbound param field */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */
    swi_uint16         MuteValue; /* temporary holder forthe mute value */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* pack outgoing parameters */
    parmlength = sizeof(swi_uint16);

    if (pSpeakerMute->bMute)
    {
        MuteValue = 1;
    }
    else
    {
        MuteValue = 0;
    }
    piput16(MuteValue, &outparamp);

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_SPKR_MUTE, 
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
 *           This function is used to retrieve the speaker mute setting.  The 
 *           speaker mute setting is stored on the modem.
 *
 *           A request is sent to the modem.  The modem's response contains the
 *           speaker mute setting.
 *
 *           The caller must specify the amount of time to wait for the modem
 *           to respond to the request.    
 * 
 * \param    pSpeakerMute[OUT]:  speaker mute setting
 *
 * \param    timeout[IN]:        response timeout value in milliseconds
 *
 * \return   SWI_RCODE_OK - the request has been received by the modem
 * \return   SWI_RCODE_REQUEST_TIMEOUT  - the  request has timed out
 * \return   See SWI_RCODE for other error return codes
 *
 * \note     Use this function every time the host application is started to 
 *           determine whether the speaker mute is enabled. (In this way, the 
 *           speaker setting is attached to the modem, rather than the host.)
 *
 */
SWI_API SWI_RCODE SwiGetSpeakerMute(
    struct SWI_STRUCT_GSM_SpeakerMute *pSpeakerMute, swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */
    swi_uint16         MuteValue; /* temporary holder of the Mute value */
    
    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* no outgoing parameters */
    parmlength = 0;

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_SPKR_MUTE, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        MuteValue = piget16(&inparamp);
        if (MuteValue == 1)
        {
            pSpeakerMute->bMute = TRUE;
        }
        else 
        {
            pSpeakerMute->bMute = FALSE;
        }
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *         This function is used to set the DTMF tone parameters in the modem.
 *         The parameters are stored on the modem.
 *
 *         A request is sent to the modem.  The modem's response indicates that
 *         the request has been processed.
 *
 *         The caller must specify the amount of time to wait for the modem
 *         to respond to the request.    
 *
 * \param  pToneLength[IN]:  information about the DTMF tones
 *
 * \param  timeout[IN]:      response timeout value in milliseconds
 *
 * \return SWI_RCODE_OK - the request has been received by the modem
 * \return SWI_RCODE_REQUEST_TIMEOUT  - the  request has timed out
 * \return See SWI_RCODE for other error return codes
 *
 * \note   Every parameter in the data structure must be valid and none may 
 *         be omitted.  This function only allows you to store values entered in 
 *         the application. (SwiGetNVToneStore is used to retrieve the values.)
 *         The firmware does not use the tone length values stored by this 
 *         function-its purpose is to allow the application to have a 
 *         user-defined tone configuration attached to the modem (rather
 *         than the host). The earpiece feedback setting stored by this function 
 *         is used by the SwiSetContDtmf function.
 */
SWI_API SWI_RCODE SwiSetNVToneStore(
    struct SWI_STRUCT_NVToneStore *pToneLength, swi_uint32 timeout)
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
    parmlength = SwiPkGsmNVToneStore(outparamp, pToneLength);

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_NV_TONE_STORE, 
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
 *        This function is used to retrieve DTMF tone parameters from the modem.
 *        The tone parameters are used when playing DTMF tones.
 *
 *        A request is sent to the modem.  The modem's response contains the
 *        tone parameters stored in the modem.
 *
 *        The caller must specify the amount of time to wait for the modem
 *        to respond to the request.    
 *
 * \param pToneLength[OUT]:  information about the DTMF tones
 * \param timeout[IN]:       response timeout value in milliseconds
 *
 * \return SWI_RCODE_OK - the request has been received by the modem
 * \return SWI_RCODE_REQUEST_TIMEOUT - the request has timed out
 * \return See SWI_RCODE for other error return codes
 */
SWI_API SWI_RCODE SwiGetNVToneStore(
    struct SWI_STRUCT_NVToneStore *pToneLength, swi_uint32 timeout)
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
                                  CNS_NV_TONE_STORE, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUpkGsmNVToneStore(inparamp, pToneLength);
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *           This function is used to get the currently used audio profile
 *
 * \param    pAudioProfile[OUT]: current audio profile currently used
 * \param    timeout[IN]:        response timeout value in milliseconds
 *
 * \return   SWI_RCODE_OK - the request has been received.
 * \return   See SWI_RCODE for other error return codes.
 *
 * \note     A device may support multiple audio profiles.
 *
 */
SWI_API SWI_RCODE SwiGetAudioProfile(
    struct SWI_STRUCT_AudioProfile* pAudioProfile, swi_uint32 timeout)
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
                                  CNS_AUDIO_PROFILE, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUpkAudioProfile(inparamp, pAudioProfile);
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *         This function is used to configure the currently used audio profile
 *
 * \param  pAudioProfile[IN]:    current audio profile currently used
 * \param  timeout[IN]:          response timeout value in milliseconds
 *
 * \return SWI_RCODE_OK - the request has been received.
 * \return See SWI_RCODE for other error return codes.
 *
 * \note   A device may support multiple audio profiles. SWI_NOTIFY_AudioProfile
 *         notification is sent anytime the audio profile is changed.
 *
 */
SWI_API SWI_RCODE SwiSetAudioProfile(
    struct SWI_STRUCT_AudioProfile* pAudioProfile, swi_uint32 timeout)
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
    parmlength = SwiPkAudioProfile(outparamp, APOBJVER2, pAudioProfile);

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_AUDIO_PROFILE, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    if( resultcode == SWI_RCODE_OK )
    {
        SwiUpkAudioProfileResp( inparamp, pAudioProfile );
    }
   
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *           This function is used to modify the caller line identity 
 *           restriction (CLIR) setting on the modem.  A request is sent to the 
 *           modem specifying the a CLIR setting.  The modem will respond to the
 *           request indicating that the setting has been changed.   
 *
 *           The caller must specify the amount of time to wait for the modem
 *           to respond to the request.    
 *
 * \param    pCLIR[IN]:      CLIR setting
 * \param    timeout[IN]:    response timeout value in milliseconds
 *
 * \return   SWI_RCODE_OK - the request has been processed
 * \return   SWI_RCODE_REQUEST_TIMEOUT  - the  request has timed out
 * \return   See SWI_RCODE for other error return codes
 */
SWI_API SWI_RCODE SwiSetCLIRSetting(
    const struct SWI_STRUCT_CLIR_Setting *pCLIR, swi_uint32 timeout)
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
    parmlength = sizeof(swi_uint8);
    *outparamp++ = pCLIR->eCallRestrict;

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_CLIR_SETTING, 
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
 *           This function is used to fetch the caller line identity restriction 
 *           (CLIR) setting from the modem.  A request is sent to the modem.  
 *           The modem will respond with the current CLIR setting.  
 *
 *           The caller must specify the amount of time to wait for the modem
 *           to respond to the request.    
 *
 * \param    pCLIR[OUT]:     the current CLIR setting
 * \param    timeout[IN]:    response timeout value in milliseconds
 *
 * \return   SWI_RCODE_OK - the current CLIR setting has been retrieved
 * \return   SWI_RCODE_REQUEST_TIMEOUT  - the  request has timed out
 * \return   See SWI_RCODE for other error return codes
 */
SWI_API SWI_RCODE SwiGetCLIRSetting(
    struct SWI_STRUCT_CLIR_Setting  *pCLIR, swi_uint32 timeout)
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
                                  CNS_CLIR_SETTING, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        pCLIR->eCallRestrict = *inparamp++;
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *           This function is used to retrieve voice mail waiting status.  
 *           This indicates if there is new voice mail.
 *
 *           A request is sent to the modem.  The modem's response contains the
 *           voice mail waiting status.
 *
 *           The caller must specify the amount of time to wait for the modem
 *           to respond to the request.    
 *
 * \param    pVoiceMessage[OUT]: voice mail waiting status
 * \param    timeout[IN]:        response timeout value in milliseconds
 *
 * \return   SWI_RCODE_OK - the request has been received by the modem
 * \return   SWI_RCODE_REQUEST_TIMEOUT  - the  request has timed out
 * \return   See SWI_RCODE for other error return codes
 *
 * \note     Use this function to get the initial status (whether or not there are 
 *           voice messages) when launching the application.  Following this initial 
 *           check, the SWI_NOTIFY_VoiceMessage notification informs the application 
 *           as new messages arrive.  A field in the data structure (VoiceMailWaiting-
 *           Feature) is intended to report whether voice messaging is a provisioned 
 *           service, but this is a network dependent feature. The application should 
 *           NOT use this field.
 *
 *           This function should not be called until the application receives
 *           the SWI_NOTIFY_SmsReady notification.  
 */
SWI_API SWI_RCODE SwiGetVoiceMessage(
    struct SWI_STRUCT_VoiceMessage *pVoiceMessage, swi_uint32 timeout)
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
                                  CNS_VMWF, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUpkGsmVoiceMessage(inparamp, pVoiceMessage);
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *           This function is used to acknowledge missed calls.  This causes the 
 *           missed count to be reset.
 *
 *           A request is sent to the modem.  The modem's response indicates 
 *           that the request has been processed.
 *
 *           The caller must specify the amount of time to wait for the modem
 *           to respond to the request.    
 *
 * \param    pMissedCall[OUT]:       missed calls
 * \param    timeout[IN]:            response timeout value in milliseconds
 *
 * \return   SWI_RCODE_OK - the request has been received by the modem
 * \return   SWI_RCODE_REQUEST_TIMEOUT  - the  request has timed out
 * \return   See SWI_RCODE for other error return codes
 */
SWI_API SWI_RCODE SwiAcknowledgeMissedCall(
    struct SWI_STRUCT_MissedCall  *pMissedCall, swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* pack outgoing parameters */
    parmlength = 0;

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_MISSED_CALL, 
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
 *   This function is used to initiate an outgoing voice call.  A request
 *   containing the phone number is sent to the modem.  The modem will
 *   respond to the request indicating that the request is being processed.
 *   The SWI_NOTIFY_CallProgress and SWI_NOTIFY_CallStatus notifications will 
 *   reflect the status of the created call.  
 *
 *   The caller must specify the amount of time to wait for the modem
 *   to respond to the request.    
 *
 * \param pCall[IN]:     information needed to create the call
 * \param timeout[IN]:   response timeout value in milliseconds
 *
 * \return   SWI_RCODE_OK - the request is being processed
 * \return   SWI_RCODE_REQUEST_TIMEOUT  - the  request has timed out
 * \return   See SWI_RCODE for other error return codes
 *
 * \note     A wait period is required between calls of this function. If the 
 *           function is called twice without a sufficient wait between the first 
 *           and second call, it returns a "FAILED" return code. The duration
 *           that must elapse between calls is network dependent and the 
 *           recommendation is to experiment with different durations before 
 *           deciding on the wait period.
 *
 */
SWI_API SWI_RCODE SwiSetCallOriginate(
    const struct SWI_STRUCT_CALL_Create* pCall, swi_uint32 timeout)
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
    parmlength = SwiPkGsmCallCreate(outparamp, pCall);

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_CALL_CREATE, 
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
 *   This function is used to send overdial numbers during calls. This sends 
 *   the digits as well as setting the duration of the DTMF tone and the
 *   duration of the interval between tones. It also enables/disables earpiece 
 *   feedback (which determines whether the tone is audible).
 *
 *   A request is sent to the modem.  The modem's response indicates that
 *   the request has been processed.
 *
 *   The caller must specify the amount of time to wait for the modem
 *   to respond to the request.    
 *
 * \param    pDTMFSend[IN]:      information about the DTMF tones to be sent
 * \param    timeout[IN]:        response timeout value in milliseconds
 *
 * \return   SWI_RCODE_OK - the request has been received by the modem
 *           SWI_RCODE_REQUEST_TIMEOUT  - the  request has timed out
 *           See SWI_RCODE for other error return codes
 *
 * \note Over-dial digits are numbers that are dialed after a circuit-switched 
 *       call has been established to communicate with a voice messaging system 
 *       or a PBX (for example, to enter a credit card number).  If your 
 *       application has a phone book feature, entries in the phone book may 
 *       contain over-dial digits separated from the main calling number
 *       by the "DTMF Control Digit Separator" (a comma). For example, 
 *       there might be a phone book entry, '6045551234,201,8'. This includes
 *       a component, ',201,8' to be over-dialed after the call is established. 
 *       The first comma separates the over-dial digits from the main number. 
 *       The second comma generates a three-second pause between dialing the 
 *       '201' and the '8'. In this example, you would use the function, 
 *       SwiSetCallOriginate to dial the phone number ('6045551234'). Once the 
 *       call is established, use SwiSetDTMFSend to send the '201,8'. (The 
 *       first comma should not be sent by either function.) The maximum number of 
 *       digits that can be sent with this function is 20.
 */
SWI_API SWI_RCODE SwiSetDTMFSend(
    struct SWI_STRUCT_DTMFSend *pDTMFSend, swi_uint32 timeout)
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
    parmlength = SwiPkGsmDTMFSend(outparamp, pDTMFSend);

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_DTMF_SEND, 
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
 *   This function is used to answer calls and manage multiple calls. Use this 
 *   function to: answer an incoming call, put a call on hold, activate a call 
 *   (take it off hold), link calls to create a multi-party call, swap an 
 *   active call and on hold call and disconnect a call. 
 *
 *   A request is sent to the modem indicating the call id and the action 
 *   to take.  The modem will respond to the request indicating that the 
 *   request is being processed.  The SWI_NOTIFY_CallProgress and 
 *   SWI_NOTIFY_CallStatus notifications will reflect the status of the current
 *   calls.  
 *
 *   The caller must specify the amount of time to wait for the modem
 *   to respond to the request.    
 *
 * \param    pCall[IN]:		information needed to create the call
 * \param    timeout[IN]:	response timeout value in milliseconds
 *
 * \return   SWI_RCODE_OK - the request is being processed
 * \return   SWI_RCODE_REQUEST_TIMEOUT  - the  request has timed out
 * \return   See SWI_RCODE for other error return codes
 */
SWI_API SWI_RCODE SwiSetCallControlUMTS(
    const struct SWI_STRUCT_CALL_UMTS_Control* pCall, swi_uint32 timeout)
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
    parmlength = SwiPkGsmCallControl(outparamp, pCall);

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_CALL_CONTROL_UMTS, 
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
 *       This function is used to play a continuous DTMF tone to the earpiece
 *       and over-the-air.
 *
 *       A request is sent to the modem.  The modem's response indicates that
 *       the request has been processed.
 *
 *       The caller must specify the amount of time to wait for the modem
 *       to respond to the request.    
 *
 * \param    pDtmf[IN]:          information about the DTMF tone to be sent
 * \param    timeout[IN]:        response timeout value in milliseconds
 *
 * \return   SWI_RCODE_OK - the request has been received by the modem
 * \return   SWI_RCODE_REQUEST_TIMEOUT  - the  request has timed out
 * \return   See SWI_RCODE for other error return codes
 *
 * \note     For earpiece feedback, the duration of the tone is limited to an
 *           absolute maximum.  However, the over-the-air tones may play
 *           continuously for the duration of the voice call until the stop
 *           request is sent.  Some carriers limit the maximum duration of tones
 *           sent over-the-air.
 */
SWI_API SWI_RCODE SwiSetContDtmf(
    struct SWI_STRUCT_ContDtmf *pDtmf, swi_uint32 timeout)
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
    parmlength = SwiPkGsmSetContDtmf(outparamp, pDtmf);

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_CONT_DTMF, 
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
 *   This function is used to set UDUB to automatically send an incoming call
 *   to voicemail or return the busy signal.
 *
 *   A request is sent to the modem.  The modem's response indicates that
 *   the request has been processed.
 *
 *   The caller must specify the amount of time to wait for the modem
 *   to respond to the request.    
 *
 * \param    timeout[IN]:        response timeout value in milliseconds
 *
 * \return   SWI_RCODE_OK - the request has been received by the modem
 * \return   SWI_RCODE_REQUEST_TIMEOUT  - the  request has timed out
 * \return   See SWI_RCODE for other error return codes
 */
SWI_API SWI_RCODE SwiSetUserdefinedUserBusy(swi_uint32 timeout)
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
                                  CNS_UDUB, 
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
 *   This function provides the main interface for user initiated supplementary 
 *   service operations. There are 5 different request types: erase, activate, 
 *   deactivate, interrogate and register.  
 * 
 *   If no basic service group information is present in this request, it is 
 *   assumed that the requested operation should affect all supplementary 
 *   services.
 * 
 *   The international number flag, length of forwarding number, forwarding 
 *   number and no reply information field are needed only when requested to 
 *   register information for call forwarding.
 * 
 *   To enable call waiting simply send a activate request with the call 
 *   waiting supplemenetary service code
 * 
 *   The first field of the set request and response packets is the current 
 *   version of the packet. The version indicate how the packet should be 
 *   interpreted. The current version of both the set request and response 
 *   packets is 1.
 *
 * \param    sSSRequest[IN]:     information about the SS request to be sent
 * \param    timeout[IN]:        response timeout value in milliseconds
 *
 * \return   SWI_RCODE_OK - the request has been received by the modem
 * \return   SWI_RCODE_REQUEST_TIMEOUT  - the  request has timed out
 * \return   See SWI_RCODE for other error return codes
 */
SWI_API SWI_RCODE SwiSetSSRequest (
    struct SWI_STRUCT_SS_Request* sSSRequest, swi_uint32 timeout)
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
    parmlength = 245;
    SwiPkGsmSetSSRequest( outparamp, sSSRequest);

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_SS_REQ_EXP, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUpkGsmSetSSRequest(inparamp, sSSRequest);
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *   User initiated supplementary service operations can take up to 20 seconds
 *   to complete. This object provides the user with the cancel 
 *   operation functionality
 * 
 *   The first field of the set request and response packets is the current
 *   version of the packet format. These fields are guaranteed to never change
 *   position or size in their packet. The version indicates how the packet
 *   should be interpreted. The current version of both the set request and 
 *   response packet is 1
 *
 * \param    sSSAbort[IN]:     information about the SS Abort request to be sent
 * \param    timeout[IN]:      response timeout value in milliseconds
 *
 * \return   SWI_RCODE_OK - the request has been received by the modem
 * \return   SWI_RCODE_REQUEST_TIMEOUT  - the  request has timed out
 * \return   See SWI_RCODE for other error return codes
 */
SWI_API SWI_RCODE SwiAbortRequest (
    struct SWI_STRUCT_SS_Abort* sSSAbort, swi_uint32 timeout)
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
    parmlength = 245;
    SwiPkGsmAbortRequest(outparamp, sSSAbort);

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_SS_ABORT_REQ, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        sSSAbort->nVersion = piget16( &inparamp );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *   The host can initiate the changing of a supplementary service password by
 *   sending a CNS_SS_PWD_REG set request. The card will return a transaction
 *   handle to the host in the set response.
 * 
 *   The set request initiates a  network password registration procedure. The 
 *   network will prompt the host of the old password, the new password twice
 *   using the transaction handle and the CNS_SS_PWD_STS object notification.
 *   The host will return the passwords to the network using the CNS_SS_PWD_STS
 *   set request
 * 
 *   The first field of the set request and response packet is the current 
 *   version of the packet format. These fields are guaranteed to never change
 *   position or size in their packet. The version indicates how the packet 
 *   should be interpreted. The current version of both the set request and
 *   response packets is 1.
 *
 * \param    sSSPassReg[IN]:   information about the SS password registration
 * \param    timeout[IN]:      response timeout value in milliseconds
 *
 * \return   SWI_RCODE_OK - the request has been received by the modem
 * \return   SWI_RCODE_REQUEST_TIMEOUT  - the  request has timed out
 * \return   See SWI_RCODE for other error return codes
 */
SWI_API SWI_RCODE SwiStartSSPassReg (
    struct SWI_STRUCT_SS_PasswordReg* sSSPassReg, swi_uint32 timeout)
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
    parmlength = 245;
    SwiPkGsmStartSSPassReg(outparamp, sSSPassReg);

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_SS_PWD_REG, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUpkGsmStartSSPassReg(inparamp, sSSPassReg);
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *   The CNS_SS_PWD_STS notification can be received as a result of a 
 *   CNS_SS_PWD_REG get request or can be network initiated
 * 
 *   A CNS_SS_PWD_STS set request should be sent in response to a 
 *   CNS_SS_PWD_STS notification if the transaction status is "Enter Password",
 *   "Enter New Password", "Enter New Password AGain", "Bad Password Try Again"
 *   or "Bad Password Format Try Again. IF the notification indicates "Bad
 *   Password Try Again" or "Bad PAssword Format Try Again" then the user should
 *   be notified.
 * 
 *   The first field of the set request/response and notification packets is the
 *   current version of the packet format. These fields are guaranteed to never
 *   change position or size in their packet. The version indicates how the 
 *   packet should be interpreted. The current version of the set request,
 *   response and notification packets is 1
 *
 * \param    sSSPassStatus[IN]: information about the SS password
 * \param    timeout[IN]:      response timeout value in milliseconds
 *
 * \return   SWI_RCODE_OK - the request has been received by the modem
 * \return   SWI_RCODE_REQUEST_TIMEOUT  - the  request has timed out
 * \return   See SWI_RCODE for other error return codes
 */
SWI_API SWI_RCODE SwiSetSSPassword (
    struct SWI_STRUCT_SS_PasswordStatus* sSSPassStatus, swi_uint32 timeout)
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
    parmlength = 245;
    SwiPkGsmSetSSPassword(outparamp, sSSPassStatus);

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_SS_PWD_STS, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        sSSPassStatus->nVersion = piget16( &inparamp );
    }

    /* All done, send the result to the caller */
    return resultcode;
}
/* 
 * $Log: SwiApiGsmVoice.c,v $
 */
