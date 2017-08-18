/**
 *
 * @ingroup swiril
 *
 * @file
 * Contains functions related to processing OEMHookString requests
 * for the AT RIL
 *
 * @author
 * Copyright: © 2011 Sierra Wireless, Inc.
 *            All rights reserved
 *
 */

#include <stdio.h>
#include <string.h>
#include <telephony/ril.h>
#include "swiril_main.h"
#include "at_channel.h"
#include "at_tok.h"
#include "swiril_misc.h"

#define LOG_TAG "RIL"
#include "swiril_log.h"

#include "swiril_oem.h"

typedef enum {
     AUDIO_GENERATOR_VOICE    = 0x00,
     AUDIO_GENERATOR_KEY_BEEP = 0x01,
     AUDIO_GENERATOR_MIDI     = 0x02,
} AudioGenerator;

typedef enum {
     AUDIO_PROFILE_HANDSET          = 0x00,
     AUDIO_PROFILE_HEADSET          = 0x01,
     AUDIO_PROFILE_CAR_KIT          = 0x02,
     AUDIO_PROFILE_SPEAKER_PHONE    = 0x03,
     AUDIO_PROFILE_AUX              = 0x04,
     AUDIO_PROFILE_TTY              = 0x05,
     AUDIO_PROFILE_AUX_EXT_PCM      = 0x06,
     AUDIO_PROFILE_PRIM_EXT_PCM     = 0x07,
} AudioProfiles;

/**
 * OEM-specific subcommand RIL getModemVol() handler
 *
 * @param[in] data
 *     pointer to the string pointer array
 * @param count [in]
 *          - count of the number of strings pointed to by the data array, unused
 * @param t
 *     token passed to the RIL command request
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 */
void requestOEMHookStringsGetModemVol(const char **data, size_t count, RIL_Token t)
{
    LOGD("%s Entered", __func__);
    ATResponse *atresponse = NULL;
    ATLine *atlinep = NULL;
    char *line = NULL;
    char *strp = NULL;
    char *subcmd = NULL;
    char *token = NULL;
    int profile = AUDIO_PROFILE_HANDSET;
    int modemVol = 0;
    int generator = AUDIO_GENERATOR_VOICE;
    char **response;
    int i;
    int j = 0;
    int err = 0;
    response = (char**)alloca(2* sizeof(char*));
    LOGD("%s Entered", __func__);

    for (i = 0; i < 2; i++) {
        response[i] = (char*)alloca(4* sizeof(char));
    }
    err = at_send_command_multivalue("AT!AVSETPROFILE?generator","", &atresponse);

    if (err < 0 || atresponse->success == 0) {
        goto error;
    }

    atlinep = atresponse->p_intermediates;
    line = atlinep->line;
    /* split the string on these delimiters into "tokens" */
    token = strtok (line,",");
    while (token != NULL) {
         j++;
         if (j==4) {
             modemVol = atoi(token);
         }
         token = strtok (NULL,",");
    }
    asprintf(&subcmd, "%s", *data);
    strcpy(response[0],subcmd);
    asprintf(&strp, "%d", modemVol);
    strcpy(response[1], strp);
    if (subcmd != NULL)
        free(subcmd);
    if (strp != NULL)
        free(strp);
finally:
    RIL_onRequestComplete(t, RIL_E_SUCCESS, response, 2 * 4 * sizeof(char));
    at_response_free(atresponse);
    return;
error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    at_response_free(atresponse);

}

/**
 * OEM-specific subcommand RIL setModemVol() handler
 *
 * @param[in] data
 *     pointer to the string pointer array
 * @param count [in]
 *          - count of the number of strings pointed to by the data array, unused
 * @param t
 *     token passed to the RIL command request
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 */
void requestOEMHookStringsSetModemVol(const char **data, size_t count, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    ATLine *atlinep = NULL;
    char *line = NULL;
    char *token = NULL;
    int profile = AUDIO_PROFILE_HANDSET;
    int generator = AUDIO_GENERATOR_VOICE;
    int earmute=0,micmute=0;
    int i;
    int j = 0;
    int err = 0;
    char *cmd = NULL;
    char *str = (char*)alloca(1 * sizeof(char));
    strcpy(str, data[1]);
    int modemVol = atoi(str);
    LOGD("%s Entered", __func__);

    /* To get the values of earmute and micmute */
    err = at_send_command_multivalue("AT!AVSETPROFILE?generator","", &atresponse);

    if (err < 0 || atresponse->success == 0) {
        goto error;
    }

    atlinep = atresponse->p_intermediates;
    line = atlinep->line;
    /* split the string on these delimiters into "tokens" */
    token = strtok (line,",");
    while (token != NULL) {
         j++;
         if (j==2) {
             earmute = atoi(token);
         }
         if (j==3) {
             micmute = atoi(token);
         }
         token = strtok (NULL,",");
    }

    asprintf(&cmd, "AT!AVSETPROFILE=%d,%d,%d,%d,%d", profile, earmute,micmute,generator,modemVol);
    err = at_send_command(cmd, &atresponse);
    free(cmd);
    if (err < 0 || atresponse->success == 0) {
        goto error;
    }
    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

finally:
    at_response_free(atresponse);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;

}

/**
 * OEM-specific subcommand RIL getNoiseSP() handler
 *
 * @param[in] data
 *     pointer to the string pointer array
 * @param count [in]
 *          - count of the number of strings pointed to by the data array, unused
 * @param t
 *     token passed to the RIL command request
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 */
void requestOEMHookStringsGetNoiseSP(const char **data, size_t count, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    ATLine *atlinep = NULL;
    char *strp = NULL;
    char *subcmd = NULL;
    char *line = NULL;
    int profile = AUDIO_PROFILE_HANDSET;
    int noiseSP;
    char **response;
    int i;
    int err = 0;
    response = (char**)alloca(2* sizeof(char*));
    LOGD("%s Entered", __func__);

    for (i = 0; i < 2; i++) {
        response[i] = (char*)alloca(4* sizeof(char));
    }
    err = at_send_command_multivalue("AT!AVNS?profile","", &atresponse);

    if (err < 0 || atresponse->success == 0) {
        goto error;
    }
    atlinep=atresponse->p_intermediates;
    line = atlinep->line;
    noiseSP = atoi(line);
    asprintf(&subcmd, "%s", *data);
    strcpy(response[0],subcmd);
    asprintf(&strp, "%d", noiseSP);
    strcpy(response[1], strp);
    if (subcmd != NULL)
        free(subcmd);
    if (strp != NULL)
        free(strp);
finally:
    RIL_onRequestComplete(t, RIL_E_SUCCESS, response, 2 * 4 * sizeof(char));
    at_response_free(atresponse);
    return;
error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    at_response_free(atresponse);

}

/**
 * OEM-specific subcommand RIL setNoiseSP() handler
 *
 * @param[in] data
 *     pointer to the string pointer array
 * @param count [in]
 *          - count of the number of strings pointed to by the data array, unused
 * @param t
 *     token passed to the RIL command request
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 */
void requestOEMHookStringsSetNoiseSP(const char **data, size_t count, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err = 0;
    char *cmd = NULL;
    char *str = (char*)alloca(1 * sizeof(char));
    strcpy(str, data[1]);
    int noiseSP = atoi(str);
    int profile = AUDIO_PROFILE_HANDSET;
    LOGD("%s Entered", __func__);

    asprintf(&cmd, "AT!AVNS=%d,%d", profile, noiseSP);
    err = at_send_command(cmd, &atresponse);
    free(cmd);
    if (err < 0 || atresponse->success == 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

finally:
    at_response_free(atresponse);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}
/**
 * requestGetCOLP
 *
 *
 * @param [in] data
 *
 *
 * @param datalen
 *     length of the data parameter received
 * @param t
 *     token passed to the RIL command request
 *
 * @return
 *     none
 */
void requestGetCOLP( const char **data, size_t datalen, RIL_Token t)
{

    ATResponse *atresponse = NULL;
    int err;
    int response[2];            // <n> and <m>
    char *line;
    char **responseCOLP;
    int i;

   responseCOLP = (char**)alloca(2* sizeof(char*));
    for (i = 0; i < 2; i++) {
        responseCOLP[i] = (char*)alloca(4* sizeof(char));
    }

   LOGD("%s::Entered\n", __func__);
   err = at_send_command_multiline("AT+COLP?", "+COLP:", &atresponse);

    if (err < 0 || atresponse->success == 0) {
        goto error;
    }
    LOGD("%s::AT+COLP? success\n", __func__);     
    line = atresponse->p_intermediates->line;

    LOGD("::RESPONSE VALUES => %s \n", line);

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    /* Parse and store <n> as first repsonse parameter. */
    err = at_tok_nextint(&line, &(response[0]));
    if (err < 0)
        goto error;

    /* Parse and store <m> as second response parameter. */
    err = at_tok_nextint(&line, &(response[1]));
    if (err < 0)
        goto error;

    LOGD("::RESPONSE VALUES => %d %d  \n", response[0],response[1]); 
    LOGD("%s::MESSAGE READ FOR -> AT+COLP success\n", __func__); 
   

   /** Check for successful active status of COLP */
    if(response[0] == 1)
        responseCOLP[1] = "1";
    else
        responseCOLP[1] = "0";

    /** Check for successful provisioned status of COLP */
    if(response[1] == 1)
        responseCOLP[0] = "1";
    else
        responseCOLP[0] = "0";

    RIL_onRequestComplete(t, RIL_E_SUCCESS, responseCOLP, 2 * sizeof(char *));

finally:
    at_response_free(atresponse);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;     
}

/**
 * requestGetCOLR
 *
 *
 * @param [in] data
 *
 *
 * @param datalen
 *     length of the data parameter received
 * @param t
 *     token passed to the RIL command request
 *
 * @return
 *     none
 */
void requestGetCOLR( const char **data, size_t datalen, RIL_Token t)
{

    ATResponse *atresponse = NULL;
    int err;
    int response[2];            // <n> and <m>
    char *line;
    char **responseCOLP;
    int i;

   responseCOLP = (char**)alloca(2* sizeof(char*));
    for (i = 0; i < 2; i++) {
        responseCOLP[i] = (char*)alloca(4* sizeof(char));
    }

   LOGD("%s::Entered\n", __func__);
   err = at_send_command_multiline("AT+COLR?", "+COLR:", &atresponse);

    if (err < 0 || atresponse->success == 0) {
        goto error;
    }
    LOGD("%s::AT+COLR? success\n", __func__);     
    line = atresponse->p_intermediates->line;

    LOGD("::RESPONSE VALUES => %s \n", line);

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    /* Parse and store <m> as second response parameter. */
    err = at_tok_nextint(&line, &(response[1]));
    if (err < 0)
        goto error;

    LOGD("::RESPONSE VALUES => %d \n", response[1]); 
    LOGD("%s::MESSAGE READ FOR -> AT+COLP success\n", __func__); 
   
    if(response[1] == 1)
    responseCOLP[1] = "1";
    else
    responseCOLP[1] = "0";

    responseCOLP[0] = "2";    

    RIL_onRequestComplete(t, RIL_E_SUCCESS, responseCOLP, 2 * sizeof(char *));

finally:
    at_response_free(atresponse);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}
/**
 * requestSetCOLP
 *
 *
 * @param [in] data
 *
 *
 * @param datalen
 *     length of the data parameter received
 * @param t
 *     token passed to the RIL command request
 *
 * @return
 *     none
 */
void requestSetCOLP(const char **data, size_t datalen, RIL_Token t )
{
    char **response;
    char atcmdresponse;
    int i;   
    const char *EnableDisable    = ((const char **)data)[1];
    response = (char**)alloca(2* sizeof(char*));
    for (i = 0; i < 2; i++) {
        response[i] = (char*)alloca(4* sizeof(char));
    }
	int err = 0;
	ATResponse *atresponse = NULL;
	char *cmd = NULL;
       
	LOGD("%s::Entered\n", __func__);
        LOGD("%s Enable Disable is ",EnableDisable);
        if(strcmp(EnableDisable,"4"))
           err = at_send_command( "AT+COLP=1", &atresponse );
        else if(strcmp(EnableDisable,"3"))
           err = at_send_command( "AT+COLP=0", &atresponse );   
	if (err < 0 || atresponse->success == 0) {
		at_response_free(atresponse);
		LOGD("%s::AT+COLP=1 failed\n", __func__);
                response[0] = "0";
		goto error;
	}
	LOGD("%s::AT+COLP=1 success\n", __func__);       
 	response[0] = "1";
	RIL_onRequestComplete(t, RIL_E_SUCCESS, response,sizeof(char *));
	at_response_free(atresponse);
	return;

error:
	RIL_onRequestComplete(t, RIL_E_SUCCESS, response,sizeof(char *));
}
/**
 * requestSetCOLR
 *
 *
 * @param [in] data
 *
 *
 * @param datalen
 *     length of the data parameter received
 * @param t
 *     token passed to the RIL command request
 *
 * @return
 *     none
 */
void requestSetCOLR( const char **data, size_t datalen, RIL_Token t )
{
    char **response;
    char atcmdresponse;
    const char *EnableDisable    = ((const char **)data)[1];
    int i;
    response = (char**)alloca(2* sizeof(char*));
    for (i = 0; i < 2; i++) {
        response[i] = (char*)alloca(4* sizeof(char));
    }
	int err = 0;
	ATResponse *atresponse = NULL;
	char *cmd = NULL;

	LOGD("%s::Entered\n", __func__);
        LOGD("%s Enable Disable is ",EnableDisable);
        if(strcmp(EnableDisable,"6"))
           err = at_send_command( "AT+COLR=1", &atresponse );
        else if(strcmp(EnableDisable,"5"))
           err = at_send_command( "AT+COLR=0", &atresponse );	
	if (err < 0 || atresponse->success == 0) {
		at_response_free(atresponse);
		LOGD("%s::AT+COLR=1 failed\n", __func__);
                response[0] = "0";
		goto error;
	}
	LOGD("%s::AT+COLR=1 success\n", __func__);
        response[0] = "1";
	RIL_onRequestComplete(t, RIL_E_SUCCESS, response,sizeof(char *));
	at_response_free(atresponse);
	return;

error:       
	RIL_onRequestComplete(t, RIL_E_SUCCESS, response,sizeof(char *));
}
/**
 *
 * QMI-Specific Hook Strings Handler. This is the main entry point
 * for handling OEM hook string requests for the QMI RIL. Functions
 * called from in this handler must be QMI-specific.
 *
 * @param[in] data
 *     pointer to the packet data
 * @param datalen
 *     packet data length
 * @param t
 *     RIL token
 *
 * @return
 *     None
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     This command defined as a RIL test command.
 *     The idea is treating the string array sent from
 *     client side as a command structure
 *         data[0] - subcommand
 *         data[1] - parameter 1 of subcommand
 *         data[2] - parameter 2 of subcommand
 *         ... ...
 *
 *     The response packet will be like following
 *         responses[0] - subcommand
 *         responses[1] - raw string of subcommand response line 1
 *         responses[2] - raw string of subcommand response line 2
 *         ... ...
 *
 *      This function has a counterpart function in the AT RIL
 *      and any additions to this may also need to be considered
 *      in that RIL.
 *
 *      This function calls the common OEM Hookstring handler
 *      in file swiril_oem.c located in swicommon directory
 *      if the incoming subcommand doesn't match any of the
 *      QMI-specific cases
 */
void requestOEMHookStrings(void *data, size_t datalen, RIL_Token t)
{
    int i;
    const char **cur;
    SWI_OEMHookType subcommand;

    /* These aren't used but present here as a reminder
     * that some ATRIL-specific handling could be done
     * if necessary. At the moment only the common
     * hookstring handler is of interest so nothing to
     * do but call it. NOTE: Use the swiril_oem_qmi.c
     * example as a template if any ATRIL specific
     * hookstring handlers should ever need to be added
     */
    cur = (const char **) data;
    subcommand = atoi(*cur);

    switch(subcommand) {

        case SWI_GET_NOISE_SP:
            requestOEMHookStringsGetNoiseSP(cur, (datalen / sizeof(char *)), t);
            break;

        case SWI_SET_NOISE_SP:
            requestOEMHookStringsSetNoiseSP(cur, (datalen / sizeof(char *)), t);
            break;

        case SWI_GET_MODEM_VOL:
            requestOEMHookStringsGetModemVol(cur, (datalen / sizeof(char *)), t);
            break;

        case SWI_SET_MODEM_VOL:
            requestOEMHookStringsSetModemVol(cur, (datalen / sizeof(char *)), t);
            break;

       case SWI_GET_COLP:
            requestGetCOLP(cur, (datalen / sizeof(char *)), t);
            break;

       case SWI_GET_COLR:
            requestGetCOLR(cur, (datalen / sizeof(char *)), t);
            break;

       case SWI_SET_COLP:
       case SWI_CLEAR_COLP:
            requestSetCOLP(cur, (datalen / sizeof(char *)), t);
            break;

       case SWI_SET_COLR:
       case SWI_CLEAR_COLR:
            requestSetCOLR(cur, (datalen / sizeof(char *)), t);
            break;

        default:
            /* Not an AT-specific call, check the common possibilities */
            swicommon_requestOEMHookStrings( data, datalen, t );
            break;
    }
}
