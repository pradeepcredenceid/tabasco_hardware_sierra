/**
 *  $Id: aptestvoice.c,v 1.8 2010/06/10 00:05:32 epasheva Exp $
 *
 *  File: aptestvoice.c
 *
 *  Purpose:  Contains functions for testing GSM Voice APIs.
 *
 * Copyright: © 2008 Sierra Wireless, Inc., all rights reserved
 *
 */
 
/* Linux definitions */
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* include files */
#include "SwiApiGsmVoice.h"
#include "SwiDataTypes.h"
#include "sl/sludefs.h"
#include "apudefs.h"
#include "aptestdefs.h"

/* Variables */
static swi_bool waiting = TRUE;

/* Functions */
/**************
 *
 * Name:     doSwiGetSpeakerVolume
 *
 * Purpose:  Test retrieve the modem's speaker volume and prints it out
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiGetSpeakerVolume (unsigned int timeout)
{
    SWI_RCODE retcode;
    struct SWI_STRUCT_GSM_SpeakerVolume gVoice;
    swi_char lasterror[80];

    /* Request Speaker Volume */
    gVoice.sizeStruct = sizeof( struct SWI_STRUCT_GSM_SpeakerVolume);
    retcode = SwiGetGsmSpeakerVolume(&gVoice, timeout );

    printf ("\n");

    if( retcode == SWI_RCODE_OK )
    {
        printf("Speaker Volume: %d\n", (int)gVoice.nVolume);
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("Speaker Volume request timed out\n");
    }
    else 
    {
        printf ("Speaker Volume request command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
}

/**************
 *
 * Name:     doSwiSetSpeakerVolume
 *
 * Purpose:  Test sets the modem's speaker volume level
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiSetSpeakerVolume (unsigned int timeout)
{
    struct SWI_STRUCT_GSM_SpeakerVolume sVoice;
    SWI_RCODE       retcode;
    swi_char   lasterror[80];

    /* Set default volume */
    sVoice.nVolume = 50;

    /* Set speaker volume */
    sVoice.sizeStruct = sizeof( struct SWI_STRUCT_GSM_SpeakerVolume);
    retcode = SwiSetGsmSpeakerVolume(&sVoice, timeout );
    
    if( retcode == SWI_RCODE_OK )
    {
        printf ("SwiSetGsmSpeakerVolume command is being processed\n");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SwiSetGsmSpeakerVolume command timed out\n");
    }
    else
    {
        printf ("SwiSetGsmSpeakerVolume command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");
}


/**************
 *
 * Name:     doSwiGetMicMute
 *
 * Purpose:  Test retrieves the headset microphone
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiGetMicMute (unsigned int timeout)
{
    SWI_RCODE retcode;
    struct SWI_STRUCT_MicMute gMic;
    swi_char lasterror[80];

    /* Request Speaker Volume */
    gMic.sizeStruct = sizeof( struct SWI_STRUCT_MicMute);
    retcode = SwiGetMicMute(&gMic, timeout );

    printf ("\n");

    if( retcode == SWI_RCODE_OK )
    {
        printf("Mic Mute Status: %s\n", gMic.bMute ? "Muted":"Unmuted");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("Mic Mute Status request timed out\n");
    }
    else 
    {
        printf ("Mic Mute Status request command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("Mdm reports CnS Error: %s\n", lasterror );
        }
    }
}

/**************
 *
 * Name:     doSwiSetMicMute
 *
 * Purpose:  Test mutes the headset microphone
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiSetMicMute (unsigned int timeout)
{
    struct SWI_STRUCT_MicMute sMic;
    SWI_RCODE       retcode;
    swi_char   lasterror[80];

    /* Set default volume */
    sMic.bMute = (swi_uint16)TRUE;

    /* Set speaker volume */
    sMic.sizeStruct = sizeof( struct SWI_STRUCT_MicMute);
    retcode = SwiSetMicMute(&sMic, timeout );
    
    if( retcode == SWI_RCODE_OK )
    {
        printf ("Set Mic Mute Status command is being processed\n");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("Set Mic Mute Status command timed out\n");
    }
    else
    {
        printf ("Set Mic Mute Status command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");
}


/**************
 *
 * Name:     doSwiGetSpeakerMute
 *
 * Purpose:  Test retrieves the headset speaker
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiGetSpeakerMute (unsigned int timeout)
{
    SWI_RCODE retcode;
    struct SWI_STRUCT_GSM_SpeakerMute gSpkr;
    swi_char lasterror[80];

    /* Request Speaker Volume */
    gSpkr.sizeStruct = sizeof( struct SWI_STRUCT_GSM_SpeakerMute);
    retcode = SwiGetSpeakerMute(&gSpkr, timeout );

    printf ("\n");

    if( retcode == SWI_RCODE_OK )
    {
        printf("Speaker Mute Status: %s\n", gSpkr.bMute ? "Muted":"Unmuted");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("Speaker Mute Status request timed out\n");
    }
    else 
    {
        printf ("Speaker Mute Status request command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("Mdm reports CnS Error: %s\n", lasterror );
        }
    }
}

/**************
 *
 * Name:     doSwiSetSpeakerMute
 *
 * Purpose:  Test mutes the headset speaker
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiSetSpeakerMute (unsigned int timeout)
{
    struct SWI_STRUCT_GSM_SpeakerMute sSpkr;
    SWI_RCODE       retcode;
    swi_char   lasterror[80];

    /* Set default volume */
    sSpkr.bMute = (swi_uint16)TRUE;

    /* Set speaker volume */
    sSpkr.sizeStruct = sizeof( struct SWI_STRUCT_GSM_SpeakerMute);
    retcode = SwiSetSpeakerMute(&sSpkr, timeout );
    
    if( retcode == SWI_RCODE_OK )
    {
        printf ("Set Mic Mute Status command is being processed\n");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("Set Mic Mute Status command timed out\n");
    }
    else
    {
        printf ("Set Mic Mute Status command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");
}

/**************
 *
 * Name:     doSwiGetNVToneStore
 *
 * Purpose:  Test retrieves the tone configuration
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiGetNVToneStore (unsigned int timeout)
{
    SWI_RCODE retcode;
    struct SWI_STRUCT_NVToneStore gToneLength;
    swi_char lasterror[80];

    /* Request Speaker Volume */
    gToneLength.sizeStruct = sizeof( struct SWI_STRUCT_NVToneStore);
    retcode = SwiGetNVToneStore(&gToneLength, timeout );

    printf ("\n");

    if( retcode == SWI_RCODE_OK )
    {
        printf("Tone Length Type: %d\n", gToneLength.eToneLength);
        printf("User-defined Tone On Duration: %d\n", 
            (int)gToneLength.nUserDefinedToneOnDuration);
        printf("User-defined Tone Off Duration: %d\n", 
            (int)gToneLength.nUserDefinedToneOffDuration);
        printf("Comfort Tone On Duration: %d\n", 
            (int)gToneLength.nComfortToneOnDuration);
        printf("Comfort Tone Off Duration: %d\n", 
            (int)gToneLength.nComfortToneOffDuration);
        printf("Earpiece Feedback: %s\n", 
            gToneLength.bEarpieceFeedback ? "True":"False");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("Tone configuration request timed out\n");
    }
    else 
    {
        printf ("Tone configuration request command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("Mdm reports CnS Error: %s\n", lasterror );
        }
    }
}

/**************
 *
 * Name:     doSwiSetNVToneStore
 *
 * Purpose:  Test sets the tone configuration
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiSetNVToneStore (unsigned int timeout)
{
    struct SWI_STRUCT_NVToneStore sToneLength;
    SWI_RCODE       retcode;
    swi_char   lasterror[80];

    /* Assign dummy values for test */
    sToneLength.eToneLength = SWI_TONELENGTH_Medium;
    sToneLength.nUserDefinedToneOnDuration = (swi_uint32)888;
    sToneLength.nUserDefinedToneOffDuration = (swi_uint32)2225;
    sToneLength.nComfortToneOnDuration = (swi_uint32)235;
    sToneLength.nComfortToneOffDuration = (swi_uint32)444;
    sToneLength.bEarpieceFeedback = TRUE;
    
    /* Send tone information */
    sToneLength.sizeStruct = sizeof( struct SWI_STRUCT_NVToneStore);
    retcode = SwiSetNVToneStore(&sToneLength, timeout );
    
    if( retcode == SWI_RCODE_OK )
    {
        printf ("Tone configuration command is being processed\n");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("Tone configuration command timed out\n");
    }
    else
    {
        printf ("Tone configuration command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");
}

/**************
 *
 * Name:     doSwiGetAudioProfile
 *
 * Purpose:  Test retrieves the curent audio profile
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiGetAudioProfile (unsigned int timeout)
{
    SWI_RCODE retcode;
    struct SWI_STRUCT_AudioProfile gAudioProfile;
    swi_char lasterror[80];

    /* Request Speaker Volume */
    gAudioProfile.sizeStruct = sizeof( struct SWI_STRUCT_AudioProfile);
    retcode = SwiGetAudioProfile(&gAudioProfile, timeout );

    printf ("\n");

    if( retcode == SWI_RCODE_OK )
    {
        printf("Current audio profile setting: %d\n", gAudioProfile.eCurrent);
        printf("Ear Piece Mute Status: %s\n", 
            gAudioProfile.bEarPieceMute ? "Muted":"Unmuted");
        printf("Mic Mute Status: %s\n", 
            gAudioProfile.bMicMute ? "Muted":"Unmuted");
        printf("Volume level: %d\n", gAudioProfile.eVolLevel);
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("Audio profile request timed out\n");
    }
    else 
    {
        printf ("Audio profile request command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("Mdm reports CnS Error: %s\n", lasterror );
        }
    }
}

/**************
 *
 * Name:     apClearFlag
 *
 * Purpose:  This function clears the wait flag
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apClearFlag ( struct SwiNotifyVariant *sVariantp)
{
    waiting = FALSE;
}

/**************
 *
 * Name:     doSwiSetAudioProfile
 *
 * Purpose:  Test sets the audio profile. The waiting flag is to ensure there
 *           is sufficient time for the test to print out the notification
 *           content
 *
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiSetAudioProfile (unsigned int timeout)
{
    struct SWI_STRUCT_AudioProfile sAudioProfile;
    SWI_RCODE       retcode;
    swi_char   lasterror[80];

    /* Assign dummy values for test */
    sAudioProfile.eCurrent = SWI_AUDIOPROFILE_Headset;
    sAudioProfile.bEarPieceMute = FALSE;
    sAudioProfile.bMicMute = FALSE;
    sAudioProfile.eAudioGen = SWI_AUDIOGEN_Voice;
    sAudioProfile.eVolLevel = SWI_AUDIOVOLLEVEL_4;

    apaddcbkfp((void *)apClearFlag);

    retcode = SwiNotify(SWI_NOTIFY_AudioProfile, timeout);

    /* Send audio profile information */
    sAudioProfile.sizeStruct = sizeof( struct SWI_STRUCT_AudioProfile);
    retcode = SwiSetAudioProfile(&sAudioProfile, timeout );
    
    if( retcode == SWI_RCODE_OK )
    {
        printf ("Audio profile command is being processed\n");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("Audio profile command timed out\n");
    }
    else
    {
        printf ("Audio profile command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    
    printf("Action Result: %d\n", sAudioProfile.eActionResult);

    while(waiting) 
    {
        sleep(1);
    }

    printf("\n");
}

/**************
 *
 * Name:     doSwiGetCLIRSetting
 *
 * Purpose:  Test retrieves the caller line id restriction setting
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiGetCLIRSetting (unsigned int timeout)
{
    SWI_RCODE retcode;
    struct SWI_STRUCT_CLIR_Setting gCLIR;
    swi_char lasterror[80];

    /* Request Speaker Volume */
    gCLIR.sizeStruct = sizeof( struct SWI_STRUCT_CLIR_Setting);
    retcode = SwiGetCLIRSetting(&gCLIR, timeout );

    printf ("\n");

    if( retcode == SWI_RCODE_OK )
    {
        printf("Caller Line ID Restriction: %d\n", gCLIR.eCallRestrict);
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("Caller Line ID Restriction request timed out\n");
    }
    else 
    {
        printf ("Caller Line ID Restriction request command is rejected: %d\n", 
            retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("Mdm reports CnS Error: %s\n", lasterror );
        }
    }
}

/**************
 *
 * Name:     doSwiSetCLIRSetting
 *
 * Purpose:  Test set the caller line id restriction setting
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiSetCLIRSetting (unsigned int timeout)
{
    struct SWI_STRUCT_CLIR_Setting sCLIR;
    SWI_RCODE       retcode;
    swi_char   lasterror[80];

    /* Set default volume */
    sCLIR.eCallRestrict = SWI_CALL_RESTRICT_Restricted;

    /* Set speaker volume */
    sCLIR.sizeStruct = sizeof( struct SWI_STRUCT_CLIR_Setting);
    retcode = SwiSetCLIRSetting(&sCLIR, timeout );
    
    if( retcode == SWI_RCODE_OK )
    {
        printf ("Set Caller Line ID Restriction command is being processed\n");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("Set Caller Line ID Restriction command timed out\n");
    }
    else
    {
        printf ("Set Caller Line ID Restriction command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");
}


/**************
 *
 * Name:     doSwiGetVoiceMessage
 *
 * Purpose:  Test retrieves the Voice Message Waiting Flags
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiGetVoiceMessage (unsigned int timeout)
{
    SWI_RCODE retcode;
    struct SWI_STRUCT_VoiceMessage gVoiceMessage;
    swi_char lasterror[80];

    /* set up for notification */
    retcode = SwiNotify(SWI_NOTIFY_VoiceMessage, timeout);

    /* Request Speaker Volume */
    gVoiceMessage.sizeStruct = sizeof( struct SWI_STRUCT_VoiceMessage);
    retcode = SwiGetVoiceMessage(&gVoiceMessage, timeout );

    printf ("\n");

    if( retcode == SWI_RCODE_OK )
    {
        printf("SIM Support Flag: %d\n", 
            gVoiceMessage.bVoiceMailWaitingFeature);
        printf("Message on Line 1 Flag: %d\n", 
            gVoiceMessage.bVoiceMailWaitingLine1);
        printf("Message on Line 2 Flag: %d\n", 
            gVoiceMessage.bVoiceMailWaitingLine2);
        printf("Fax Message Waiting Flag : %d\n", 
            gVoiceMessage.bFaxMssageWaiting1);
        printf("Data Message Waiting Flag : %d\n", 
            gVoiceMessage.bFaxMssageWaiting2);
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("Voice Message Flag Status request timed out\n");
    }
    else 
    {
        printf ("Voice Message Flag Status request command is rejected: %d\n", 
            retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("Mdm reports CnS Error: %s\n", lasterror );
        }
    }
}

/**************
 *
 * Name:     doSwiAcknowledgeMissedCall
 *
 * Purpose:  Test reset the missed call counter
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiAcknowledgeMissedCall (unsigned int timeout)
{
    struct SWI_STRUCT_MissedCall pMissedCall;
    SWI_RCODE       retcode;
    swi_char   lasterror[80];

    /* set up for notification */
    retcode = SwiNotify(SWI_NOTIFY_MissedCall, timeout);

    /* Set speaker volume */
    pMissedCall.sizeStruct = sizeof( struct SWI_STRUCT_MissedCall);
    retcode = SwiAcknowledgeMissedCall(&pMissedCall, timeout );
    
    if( retcode == SWI_RCODE_OK )
    {
        printf ("Missed Call Indication command is being processed\n");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("Missed Call Indication command timed out\n");
    }
    else
    {
        printf ("Missed Call Indication command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");
}
/**************
 *
 * Name:     doSwiSetCallOriginate
 *
 * Purpose:  Test attemps to make a voice call
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiSetCallOriginate (unsigned int timeout)
{
    struct SWI_STRUCT_CALL_Create pCall;
    SWI_RCODE       retcode;
    swi_char   lasterror[80];

    /* Set call information */
    pCall.eCallRestrict = SWI_CALL_RESTRICT_Allowed;
    pCall.bInternational = FALSE;
    pCall.nNumberLength = 10;
    pCall.bcdPhone[0] = 6;
    pCall.bcdPhone[1] = 0;
    pCall.bcdPhone[2] = 4;
    pCall.bcdPhone[3] = 2;
    pCall.bcdPhone[4] = 3;
    pCall.bcdPhone[5] = 3;
    pCall.bcdPhone[6] = 8;
    pCall.bcdPhone[7] = 0;
    pCall.bcdPhone[8] = 1;
    pCall.bcdPhone[9] = 9;
    
    retcode = SwiNotify(SWI_NOTIFY_CallProgress, timeout);
    
    /* Attempt voice call */
    pCall.sizeStruct = sizeof( struct SWI_STRUCT_CALL_Create);
    retcode = SwiSetCallOriginate(&pCall, timeout );
    
    if( retcode == SWI_RCODE_OK )
    {
        printf ("Call Create command is being processed\n");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("Call Create command timed out\n");
    }
    else
    {
        printf ("Call Create command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");
}

/**************
 *
 * Name:     doSwiSetDTMFSend
 *
 * Purpose:  Test request the card to send DTMF overdial digits
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiSetDTMFSend (unsigned int timeout)
{
    struct SWI_STRUCT_DTMFSend pDTMFSend;
    SWI_RCODE       retcode;
    swi_char        lasterror[80];


    pDTMFSend.nNumberOfDigits = 5; 
    /*! a buffer of DTMF digits in ASCII (',' '*' 
    *  '#' '0'-'9' 'A'-'D'). "," indicates a 3 second pause */
    pDTMFSend.byDigits[0] = '2';
    pDTMFSend.byDigits[1] = 'B';
    pDTMFSend.byDigits[2] = '*';
    pDTMFSend.byDigits[3] = ',';
    pDTMFSend.byDigits[4] = '#';
    pDTMFSend.nToneOnDuration = 2569;     
    pDTMFSend.nToneOffDuration = 300;    
    pDTMFSend.bEarpieceFeedback = TRUE;

    /* Set dummy values */
    pDTMFSend.sizeStruct = sizeof( struct SWI_STRUCT_DTMFSend);
    retcode = SwiSetDTMFSend(&pDTMFSend, timeout );
    
    if( retcode == SWI_RCODE_OK )
    {
        printf ("DTMF Overdial command is being processed\n");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("DTMF Overdial command timed out\n");
    }
    else
    {
        printf ("DTMF Overdial command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");
}

/**************
 *
 * Name:     doSwiSetCallControlUMTS
 *
 * Purpose:  Test performs various acts on voice calls in progress
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiSetCallControlUMTS (unsigned int timeout)
{
    struct SWI_STRUCT_CALL_UMTS_Control pCall;
    SWI_RCODE       retcode;
    swi_char        lasterror[80];

    /* dummy variables */
    pCall.nVersion = 1;
    pCall.nCallID = 2;
    pCall.eCallAction = SWI_CALL_CONTROL_UMTS_DisconnectAll;

    /* Set dummy values */
    pCall.sizeStruct = sizeof( struct SWI_STRUCT_CALL_UMTS_Control);
    retcode = SwiSetCallControlUMTS(&pCall, timeout );
    
    if( retcode == SWI_RCODE_OK )
    {
        printf ("Call Control command is being processed\n");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("Call Control command timed out\n");
    }
    else
    {
        printf ("Call Control command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");
}

/**************
 *
 * Name:     doSwiSetContDtmf
 *
 * Purpose:  Test is used to play DTMF tones continuously over the air and to 
 *           the earpiece only during an active voice call
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiSetContDtmf (unsigned int timeout)
{
    struct SWI_STRUCT_ContDtmf pDtmf;
    SWI_RCODE       retcode;
    swi_char        lasterror[80];

    /* dummy variables */
    pDtmf.nDtmfTone = (swi_uint32)'#';
    pDtmf.bStart = FALSE;

    /* Set dummy values */
    pDtmf.sizeStruct = sizeof( struct SWI_STRUCT_ContDtmf);
    retcode = SwiSetContDtmf(&pDtmf, timeout );
    
    if( retcode == SWI_RCODE_OK )
    {
        printf ("DTMF command is being processed\n");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("DTMF command timed out\n");
    }
    else
    {
        printf ("DTMF command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");
}

/**************
 *
 * Name:     doSwiSetUserdefinedUserBusy
 *
 * Purpose:  Test is used to send call to voicemail
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiSetUserdefinedUserBusy (unsigned int timeout)
{
    SWI_RCODE       retcode;
    swi_char        lasterror[80];

    /* Set Values */
    retcode = SwiSetUserdefinedUserBusy(timeout);
    
    if( retcode == SWI_RCODE_OK )
    {
        printf ("User Busy Signal command is being processed\n");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("User Busy Signal command timed out\n");
    }
    else
    {
        printf ("User Busy Signal command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");
}

/**************
 *
 * Name:     doSwiSetSSRequest
 *
 * Purpose:  Test is used to send Supplementary Service requests
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiSetSSRequest (unsigned int timeout)
{
    SWI_RCODE       retcode;
    swi_char        lasterror[80];
    struct SWI_STRUCT_SS_Request sSSRequest;

    /* dummy variables */
    sSSRequest.nVersion = APOBJVER1;
    sSSRequest.eRequestType = SWI_SSREQUESTTYPE_Activate;
    sSSRequest.eSuppServiceCode = SWI_SERVCODE_CallWaiting;
    sSSRequest.bBasicServGrpInfo = FALSE;
    sSSRequest.eBasicServGrpType = SWI_BASICSERVGRPTYPE_TeleServices;
    sSSRequest.eBasicServGrpCode = (swi_uint8)0;

    /* Send dummy values */
    sSSRequest.sizeStruct = sizeof( struct SWI_STRUCT_SS_Request);
    retcode = SwiSetSSRequest(&sSSRequest, timeout );
    
    if( retcode == SWI_RCODE_OK )
    {
        printf ("SS Request command is being processed\n");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SS Request command timed out\n");
    }
    else
    {
        printf ("SS Request command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");
}

/**************
 *
 * Name:     doSwiAbortRequest
 *
 * Purpose:  Test is used to send Supplementary Service Abort request
 *           once a SS request has been made 
 * 
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    SS request must take at least several seconds to complete
 *           for the SS Abort request to work correctly
 *
 **************/
package void doSwiAbortRequest (unsigned int timeout)
{
    SWI_RCODE       retcode;
    swi_char        lasterror[80];
    struct SWI_STRUCT_SS_Request sSSRequest;
    struct SWI_STRUCT_SS_Abort sSSAbort;
    
    /* dummy variables */
    sSSRequest.nVersion = APOBJVER1;
    sSSRequest.eRequestType = SWI_SSREQUESTTYPE_Activate;
    sSSRequest.eSuppServiceCode = SWI_SERVCODE_CallWaiting;
    sSSRequest.bBasicServGrpInfo = FALSE;
    sSSRequest.eBasicServGrpType = SWI_BASICSERVGRPTYPE_TeleServices;
    sSSRequest.eBasicServGrpCode = (swi_uint8)0;
    
    sSSRequest.bInternationalNumFlag = FALSE;
    sSSRequest.nLengthFwdNum = (swi_uint8)10;
    sSSRequest.byFwdNum[0] = 6;
    sSSRequest.byFwdNum[1] = 0;
    sSSRequest.byFwdNum[2] = 4;
    sSSRequest.byFwdNum[3] = 2;
    sSSRequest.byFwdNum[4] = 3;
    sSSRequest.byFwdNum[5] = 3;
    sSSRequest.byFwdNum[6] = 8;
    sSSRequest.byFwdNum[7] = 0;
    sSSRequest.byFwdNum[8] = 1;
    sSSRequest.byFwdNum[9] = 9;
    sSSRequest.bNoReplyInfoPresent = TRUE;
    sSSRequest.nNoReplyTime = (swi_uint8)3;
    
    sSSAbort.nVersion = (swi_uint16)APOBJVER1;

    /* Send dummy values */
    sSSRequest.sizeStruct = sizeof( struct SWI_STRUCT_SS_Request);
    sSSAbort.sizeStruct = sizeof( struct SWI_STRUCT_SS_Abort);
    retcode = SwiSetSSRequest(&sSSRequest, timeout );
    
    if( retcode == SWI_RCODE_OK )
    {
        printf ("SS Request command is being processed\n");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SS Request command timed out\n");
    }
    else
    {
        printf ("SS Request command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");
    
    sSSAbort.nHandle = (swi_uint32)sSSRequest.nHandle;
    retcode = SwiAbortRequest(&sSSAbort, timeout );
    
    if( retcode == SWI_RCODE_OK )
    {
        printf ("SS Request command is being processed\n");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SS Request command timed out\n");
    }
    else
    {
        printf ("SS Request command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("Mdm reports CnS Error: %s\n", lasterror );
        }
    }
}

/**************
 *
 * Name:     doSwiSetSSRequestErase
 *
 * Purpose:  Test is used to send Supplementary Service to erase a
 *           Supplementary Service
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiSetSSRequestErase (unsigned int timeout)
{
    SWI_RCODE       retcode;
    swi_char        lasterror[80];
    struct SWI_STRUCT_SS_Request sSSRequest;

    /* dummy variables */
    sSSRequest.nVersion = APOBJVER1;
    sSSRequest.eRequestType = SWI_SSREQUESTTYPE_Erase;
    sSSRequest.eSuppServiceCode = SWI_SERVCODE_CallFwdAllServices;
    sSSRequest.bBasicServGrpInfo = FALSE;
    sSSRequest.eBasicServGrpType = SWI_BASICSERVGRPTYPE_TeleServices;
    sSSRequest.eBasicServGrpCode = (swi_uint8)0;
    
    sSSRequest.bInternationalNumFlag = FALSE;
    sSSRequest.nLengthFwdNum = (swi_uint8)10;
    sSSRequest.byFwdNum[0] = 6;
    sSSRequest.byFwdNum[1] = 0;
    sSSRequest.byFwdNum[2] = 4;
    sSSRequest.byFwdNum[3] = 2;
    sSSRequest.byFwdNum[4] = 3;
    sSSRequest.byFwdNum[5] = 3;
    sSSRequest.byFwdNum[6] = 8;
    sSSRequest.byFwdNum[7] = 0;
    sSSRequest.byFwdNum[8] = 1;
    sSSRequest.byFwdNum[9] = 9;
    sSSRequest.bNoReplyInfoPresent = TRUE;
    sSSRequest.nNoReplyTime = (swi_uint8)3;

    /* Send dummy values */
    sSSRequest.sizeStruct = sizeof( struct SWI_STRUCT_SS_Request);
    retcode = SwiSetSSRequest(&sSSRequest, timeout );
    
    if( retcode == SWI_RCODE_OK )
    {
        printf ("SS Request command is being processed\n");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SS Request command timed out\n");
    }
    else
    {
        printf ("SS Request command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");
}

/**************
 *
 * Name:     doSwiSetSSRequestActivate
 *
 * Purpose:  Test is used to send Supplementary Service requests to
 *           activate a Supplementary Service
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiSetSSRequestActivate (unsigned int timeout)
{
    SWI_RCODE       retcode;
    swi_char        lasterror[80];
    struct SWI_STRUCT_SS_Request sSSRequest;

    /* dummy variables */
    sSSRequest.nVersion = APOBJVER1;
    sSSRequest.eRequestType = SWI_SSREQUESTTYPE_Activate;
    sSSRequest.eSuppServiceCode = SWI_SERVCODE_CallFwdAllServices;
    sSSRequest.bBasicServGrpInfo = FALSE;
    sSSRequest.eBasicServGrpType = SWI_BASICSERVGRPTYPE_TeleServices;
    sSSRequest.eBasicServGrpCode = (swi_uint8)0;
        
    sSSRequest.bInternationalNumFlag = FALSE;
    sSSRequest.nLengthFwdNum = (swi_uint8)10;
    sSSRequest.byFwdNum[0] = 6;
    sSSRequest.byFwdNum[1] = 0;
    sSSRequest.byFwdNum[2] = 4;
    sSSRequest.byFwdNum[3] = 2;
    sSSRequest.byFwdNum[4] = 3;
    sSSRequest.byFwdNum[5] = 3;
    sSSRequest.byFwdNum[6] = 8;
    sSSRequest.byFwdNum[7] = 0;
    sSSRequest.byFwdNum[8] = 1;
    sSSRequest.byFwdNum[9] = 9;
    sSSRequest.bNoReplyInfoPresent = TRUE;
    sSSRequest.nNoReplyTime = (swi_uint8)3;

    /* Send dummy values */
    sSSRequest.sizeStruct = sizeof( struct SWI_STRUCT_SS_Request);
    retcode = SwiSetSSRequest(&sSSRequest, timeout );
    
    if( retcode == SWI_RCODE_OK )
    {
        printf ("SS Request command is being processed\n");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SS Request command timed out\n");
    }
    else
    {
        printf ("SS Request command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");
}

/**************
 *
 * Name:     doSwiSetSSRequestDeactivate
 *
 * Purpose:  Test is used to send Supplementary Service requests to
 *           deactivate a Supplementary Service
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiSetSSRequestDeactivate (unsigned int timeout)
{
    SWI_RCODE       retcode;
    swi_char        lasterror[80];
    struct SWI_STRUCT_SS_Request sSSRequest;

    /* dummy variables */
    sSSRequest.nVersion = APOBJVER1;
    sSSRequest.eRequestType = SWI_SSREQUESTTYPE_Deactivate;
    sSSRequest.eSuppServiceCode = SWI_SERVCODE_CallFwdAllServices;
    sSSRequest.bBasicServGrpInfo = FALSE;
    sSSRequest.eBasicServGrpType = SWI_BASICSERVGRPTYPE_TeleServices;
    sSSRequest.eBasicServGrpCode = (swi_uint8)0;
        
    sSSRequest.bInternationalNumFlag = FALSE;
    sSSRequest.nLengthFwdNum = (swi_uint8)10;
    sSSRequest.byFwdNum[0] = 6;
    sSSRequest.byFwdNum[1] = 0;
    sSSRequest.byFwdNum[2] = 4;
    sSSRequest.byFwdNum[3] = 2;
    sSSRequest.byFwdNum[4] = 3;
    sSSRequest.byFwdNum[5] = 3;
    sSSRequest.byFwdNum[6] = 8;
    sSSRequest.byFwdNum[7] = 0;
    sSSRequest.byFwdNum[8] = 1;
    sSSRequest.byFwdNum[9] = 9;
    sSSRequest.bNoReplyInfoPresent = TRUE;
    sSSRequest.nNoReplyTime = (swi_uint8)3;
    
    /* Send dummy values */
    sSSRequest.sizeStruct = sizeof( struct SWI_STRUCT_SS_Request);
    retcode = SwiSetSSRequest(&sSSRequest, timeout );
    
    if( retcode == SWI_RCODE_OK )
    {
        printf ("SS Request command is being processed\n");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SS Request command timed out\n");
    }
    else
    {
        printf ("SS Request command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");
}

/**************
 *
 * Name:     doSwiSetSSRequestInterrogate
 *
 * Purpose:  Test is used to send Supplementary Service requests to
 *           interrogate a Supplementary Service
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiSetSSRequestInterrogate (unsigned int timeout)
{
    SWI_RCODE       retcode;
    swi_char        lasterror[80];
    struct SWI_STRUCT_SS_Request sSSRequest;

    /* dummy variables */
    sSSRequest.nVersion = APOBJVER1;
    sSSRequest.eRequestType = SWI_SSREQUESTTYPE_Interrogate;
    sSSRequest.eSuppServiceCode = SWI_SERVCODE_CallWaiting;
    sSSRequest.bBasicServGrpInfo = FALSE;
    sSSRequest.eBasicServGrpType = SWI_BASICSERVGRPTYPE_TeleServices;
    sSSRequest.eBasicServGrpCode = (swi_uint8)0;
        
    sSSRequest.bInternationalNumFlag = FALSE;
    sSSRequest.nLengthFwdNum = (swi_uint8)10;
    sSSRequest.byFwdNum[0] = 6;
    sSSRequest.byFwdNum[1] = 0;
    sSSRequest.byFwdNum[2] = 4;
    sSSRequest.byFwdNum[3] = 2;
    sSSRequest.byFwdNum[4] = 3;
    sSSRequest.byFwdNum[5] = 3;
    sSSRequest.byFwdNum[6] = 8;
    sSSRequest.byFwdNum[7] = 0;
    sSSRequest.byFwdNum[8] = 1;
    sSSRequest.byFwdNum[9] = 9;
    sSSRequest.bNoReplyInfoPresent = TRUE;
    sSSRequest.nNoReplyTime = (swi_uint8)3;

    /* Send dummy values */
    sSSRequest.sizeStruct = sizeof( struct SWI_STRUCT_SS_Request);
    retcode = SwiSetSSRequest(&sSSRequest, timeout );
    
    if( retcode == SWI_RCODE_OK )
    {
        printf ("SS Request command is being processed\n");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SS Request command timed out\n");
    }
    else
    {
        printf ("SS Request command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");
}

/**************
 *
 * Name:     doSwiSetSSRequestRegister
 *
 * Purpose:  Test is used to send Supplementary Service requests to
 *           register a Supplementary Service
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiSetSSRequestRegister (unsigned int timeout)
{
    SWI_RCODE       retcode;
    swi_char        lasterror[80];
    struct SWI_STRUCT_SS_Request sSSRequest;

    /* dummy variables */
    sSSRequest.nVersion = APOBJVER1;
    sSSRequest.eRequestType = SWI_SSREQUESTTYPE_Register;
    sSSRequest.eSuppServiceCode = SWI_SERVCODE_CallFwdAllServices;
    sSSRequest.bBasicServGrpInfo = FALSE;
    sSSRequest.eBasicServGrpType = SWI_BASICSERVGRPTYPE_TeleServices;
    sSSRequest.eBasicServGrpCode = (swi_uint8)1;
        
    sSSRequest.bInternationalNumFlag = FALSE;
    sSSRequest.nLengthFwdNum = (swi_uint8)10;
    sSSRequest.byFwdNum[0] = 6;
    sSSRequest.byFwdNum[1] = 0;
    sSSRequest.byFwdNum[2] = 4;
    sSSRequest.byFwdNum[3] = 2;
    sSSRequest.byFwdNum[4] = 3;
    sSSRequest.byFwdNum[5] = 3;
    sSSRequest.byFwdNum[6] = 8;
    sSSRequest.byFwdNum[7] = 0;
    sSSRequest.byFwdNum[8] = 1;
    sSSRequest.byFwdNum[9] = 9;
    sSSRequest.bNoReplyInfoPresent = TRUE;
    sSSRequest.nNoReplyTime = (swi_uint8)3;

    /* Send dummy values */
    sSSRequest.sizeStruct = sizeof( struct SWI_STRUCT_SS_Request);
    retcode = SwiSetSSRequest(&sSSRequest, timeout );
    
    if( retcode == SWI_RCODE_OK )
    {
        printf ("SS Request command is being processed\n");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SS Request command timed out\n");
    }
    else
    {
        printf ("SS Request command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");
}

/**************
 *
 * Name:     doSwiSetSSPassword
 *
 * Purpose:  Test is used to send Supplementary Service Password status 
 *           request 
 * 
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiSetSSPassword (unsigned int timeout)
{
    SWI_RCODE       retcode;
    swi_char        lasterror[80];
    struct SWI_STRUCT_SS_PasswordReg sSSPassReg;
    struct SWI_STRUCT_SS_PasswordStatus sSSPassStatus;

    /* dummy variables */
    sSSPassStatus.nVersion = (swi_uint16)APOBJVER1;
    /* old password*/
    sSSPassStatus.nPasswordLength = 4;
    sSSPassStatus.byPassword[0] = 1;
    sSSPassStatus.byPassword[1] = 0;
    sSSPassStatus.byPassword[2] = 1;
    sSSPassStatus.byPassword[3] = 0;

    /* dummy variables */
    sSSPassReg.nVersion = (swi_uint16)APOBJVER1;
    sSSPassReg.eSuppServiceCode = SWI_SERVCODE_AllCallRestriction;

    /* Send dummy values */
    sSSPassReg.sizeStruct = sizeof( struct SWI_STRUCT_SS_PasswordReg);
    retcode = SwiStartSSPassReg(&sSSPassReg, timeout );
    
    if( retcode == SWI_RCODE_OK )
    {
        printf ("SS Password Registration command is being processed\n");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SS Password Registration command timed out\n");
    }
    else
    {
        printf ("SS Password Registration command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");
    
    /* Send old password */
    sSSPassStatus.nHandle = (swi_uint32)sSSPassReg.nHandle;
    
    sSSPassStatus.sizeStruct = sizeof( struct SWI_STRUCT_SS_PasswordStatus);
    retcode = SwiSetSSPassword(&sSSPassStatus, timeout );
    
    if( retcode == SWI_RCODE_OK )
    {
        printf ("SS Password Status command is being processed\n");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SS Password Status command timed out\n");
    }
    else
    {
        printf ("SS Password Status command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    
    /* Send new SS password*/
    sSSPassStatus.nHandle = (swi_uint32)sSSPassReg.nHandle;
    sSSPassStatus.nPasswordLength = 4;
    sSSPassStatus.byPassword[0] = 1;
    sSSPassStatus.byPassword[1] = 1;
    sSSPassStatus.byPassword[2] = 1;
    sSSPassStatus.byPassword[3] = 1;
    
    sSSPassStatus.sizeStruct = sizeof( struct SWI_STRUCT_SS_PasswordStatus);
    retcode = SwiSetSSPassword(&sSSPassStatus, timeout );
    
    if( retcode == SWI_RCODE_OK )
    {
        printf ("SS Password Status command is being processed\n");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SS Password Status command timed out\n");
    }
    else
    {
        printf ("SS Password Status command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    
    /* Repeat new password*/
    sSSPassStatus.nHandle = (swi_uint32)sSSPassReg.nHandle;
    
    sSSPassStatus.sizeStruct = sizeof( struct SWI_STRUCT_SS_PasswordStatus);
    retcode = SwiSetSSPassword(&sSSPassStatus, timeout );
    
    if( retcode == SWI_RCODE_OK )
    {
        printf ("SS Password Status command is being processed\n");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SS Password Status command timed out\n");
    }
    else
    {
        printf ("SS Password Status command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("Mdm reports CnS Error: %s\n", lasterror );
        }
    }
}

/* 
 * $Log: aptestvoice.c,v $
 */
