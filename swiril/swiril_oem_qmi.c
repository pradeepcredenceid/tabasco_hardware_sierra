/**
 *
 * @ingroup swiril
 *
 * @file
 * Prototypes for QMI based OEM related Sierra functions
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
#include "swiril_misc_qmi.h"
#include "swiril_device_qmi.h"
#include "swiril_oem_qmi.h"

#define LOG_TAG "RIL"
#include "swiril_log.h"

#include "swiril_oem.h"

#include "qmerrno.h"
#include "SWIWWANCMAPI.h"

#define QMI_ITEM_NOISE_SUPPRESSION  0x01

enum eQMI_AUDIO_GENERATOR {
     AUDIO_GENERATOR_VOICE    = 0x00,
     AUDIO_GENERATOR_KEY_BEEP = 0x01,
     AUDIO_GENERATOR_MIDI     = 0x02,
};

enum eQMI_AUDIO_PROFILE {
     AUDIO_PROFILE_HANDSET          = 0x00,
     AUDIO_PROFILE_HEADSET          = 0x01,
     AUDIO_PROFILE_CAR_KIT          = 0x02,
     AUDIO_PROFILE_SPEAKER_PHONE    = 0x03,
     AUDIO_PROFILE_AUX              = 0x04,
     AUDIO_PROFILE_TTY              = 0x05,
     AUDIO_PROFILE_AUX_EXT_PCM      = 0x06,
     AUDIO_PROFILE_PRIM_EXT_PCM     = 0x07,
     AUDIO_PROFILE_EXT_SLAVE_PCM    = 0x08,
     AUDIO_PROFILE_I2S              = 0x09,
};

enum eQMI_AUDIO_ITEM {
    AUDIO_ITEM_EC,
    AUDIO_ITEM_NS,
    AUDIO_ITEM_TXVOL,
    AUDIO_ITEM_DTMFTXG,
    AUDIO_ITEM_CODECSTG,
    AUDIO_ITEM_TXPCMIIRFLTR,
    AUDIO_ITEM_RXPCMIIRFLTR,
    AUDIO_ITEM_MICGAIN,
    AUDIO_ITEM_RXAGC,
    AUDIO_ITEM_TXAGC,
    AUDIO_ITEM_RXAGCLIST,
    AUDIO_ITEM_RXAVCLIST,
    AUDIO_ITEM_TXAGCLIST
};
/**
 *  Initiate CDMA activation on the attached device
 *
 *  @param  data [in]
 *          - Pointer to the string pointer array, unused
 *  @param  datalen [in]
 *          - Count of pointers in the array, unused
 *  @param  t [in]
 *          - A RIL token structure pointer
 *
 *  @return None
 *
 *  @note   Support: CDMA
 *
 */
void requestOEMHookStringsCMDAActQMI(void *data, size_t datalen, RIL_Token t)
{
    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    activateServiceCDMA();
}


/**
 *  Initiate a SIM PB read request
 *
 *  @param  data [in]
 *          - Pointer to the string pointer array, unused
 *  @param  count [in]
 *          - count of the number of strings pointed to by the data array, unused
 *  @param  t [in]
 *          - A RIL token structure pointer
 *
 *  @return None
 *
 *  @note   Support: UMTS
 *
 */
void requestOEMHookStringsReadSIMPB(const char **data, size_t count, RIL_Token t)
{
    char **responsepp = NULL;
    int responsesize=0;
    ATResponse *atresponse = NULL;
    ATLine *atlinep = NULL;
    int err = 0;
    char *cmd;
    char *line;
    char *tmpstr;
    int pbused;
    int pbsize;
    int index;


    /* Ensure the correct character set is used */
    err = at_send_command("AT+CSCS=\"IRA\"", &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;
    at_response_free(atresponse);
    atresponse=NULL;


    /* Ensure all reads are from the SIM phonebook */
    err = at_send_command("AT+CPBS=\"SM\"", &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;
    at_response_free(atresponse);
    atresponse=NULL;


    /* Determine how many records are in the SIM phonebook */
    err = at_send_command_singleline("AT+CPBS?", "+CPBS:", &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;

    line = atresponse->p_intermediates->line;

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    err = at_tok_nextstr(&line, &tmpstr);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &pbused);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &pbsize);
    if (err < 0)
        goto error;

    at_response_free(atresponse);
    atresponse=NULL;
    LOGI("%s: pbused=%d, pbsize=%d", __func__, pbused, pbsize);


    /* Make sure there are phone book entries to read */
    if (pbused > 0) {

        /* Allocate memory for response */
        responsepp = swimalloc(pbused * sizeof(char *),
            "requestOEMHookStringsReadSIMPB: no memory");


        /* Read the phonebook, and fill in the response */
        asprintf(&cmd, "AT+CPBR=1,%d", pbsize);
        err = at_send_command_multiline(cmd, "+CPBR:", &atresponse);
        free(cmd);
        if (err < 0 || atresponse->success == 0)
            goto error;

        atlinep = atresponse->p_intermediates;

        for (index=0; (index < pbused) && (atlinep != NULL); index++) {
            line = atlinep->line;

            err = at_tok_start(&line);
            if (err < 0)
                goto error;

            responsepp[index] = line;

            atlinep = atlinep->p_next;
        }

        /* LOG a message if we don't get as many as we expect */
        if ( index != pbused ) {
            LOGE("%s: mismatch pbused=%d, index=%d", __func__, pbused, index);
        }

        responsesize = index * sizeof(char *);

    } else {
        responsepp = NULL;
        responsesize = 0;
    }

    /* Return the PB records or empty list */
    RIL_onRequestComplete(t, RIL_E_SUCCESS, responsepp, responsesize);


finally:
    /* Free memory for response */
    free(responsepp);

    at_response_free(atresponse);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

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
    int i;
    char **response;
    char *strp = NULL;
    char *subcmd = NULL;
    response = (char**)alloca(2* sizeof(char*));
    for (i = 0; i < 2; i++) {
       response[i] = (char*)alloca(4* sizeof(char));
    }
    ULONG nRet = eQCWWAN_ERR_NONE;
    GetAudioProfileReq Req;
    GetAudioProfileResp Res;

    LOGD("%s Entered function", __func__);
    memset( &Req, 0, sizeof(Req));
    memset( &Res, 0, sizeof(Res));
    Req.Generator = AUDIO_GENERATOR_VOICE;

    nRet = SLQSGetAudioProfile (&Req, &Res);

    if ( nRet != eQCWWAN_ERR_NONE ) {
        LOGD("%s: SLQSGetAudioProfile() failed %lu", __func__, nRet);
        goto error;
    }

    asprintf(&subcmd, "%s", *data);
    strcpy(response[0],subcmd);
    asprintf(&strp, "%d", Res.Volume);
    strcpy(response[1], strp);
    if (subcmd != NULL)
        free(subcmd);

    if (strp != NULL)
        free(strp);

finally:
    RIL_onRequestComplete(t, RIL_E_SUCCESS, response, 2 * 4 * sizeof(char));
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);

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
    char *cmd = NULL;
    int err = 0;
    char *str = (char*)alloca(1 * sizeof(char));
    strcpy(str, data[1]);
    BYTE volume = atoi(str);

    LOGD("%s: Entered", __func__);

    asprintf(&cmd, "AT+CLVL=%d", volume);
    at_send_command(cmd,&atresponse);
    free(cmd);
    if (err < 0 || atresponse->success == 0) {
        goto error;
    }
    at_response_free(atresponse);
    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    return;

error:
    at_response_free(atresponse);
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);

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
    int i;
    char **response;
    response = (char**)alloca(2* sizeof(char*));
    for (i = 0; i < 2; i++) {
        response[i] = (char*)alloca(4* sizeof(char));
    }
    ULONG nRet = eQCWWAN_ERR_NONE;
    GetAudioPathConfigReq Req;
    GetAudioPathConfigResp Res;
    BYTE *pNSEnable, nsEnable;
    char *strp = NULL;
    char *subcmd = NULL;

    LOGD("%s Entered function", __func__);
    memset( &Req, 0, sizeof(Req));
    memset( &Res, 0, sizeof(Res));
    memset( &nsEnable, 0, sizeof(nsEnable));

    Res.pNSEnable = &nsEnable;
    Req.Item = AUDIO_ITEM_NS;
    Req.Profile = AUDIO_PROFILE_HANDSET;

    nRet = SLQSGetAudioPathConfig (&Req, &Res);
    if ( nRet != eQCWWAN_ERR_NONE ) {
        LOGD("%s: SLQSGetAudioPathConfig() failed %lu", __func__, nRet);
        goto error;
    }

    asprintf(&subcmd, "%s", *data);
    strcpy(response[0],subcmd);
    asprintf(&strp, "%d",*(Res.pNSEnable));
    strcpy(response[1], strp);
    if (subcmd != NULL)
        free(subcmd);

    if (strp != NULL)
        free(strp);

finally:
    RIL_onRequestComplete(t, RIL_E_SUCCESS, response, 2 * 4 * sizeof(char));
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
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
    char *str = (char*)alloca(1 * sizeof(char));
    strcpy(str, data[1]);
    BYTE noiseSP = atoi(str);
    ULONG nRet = eQCWWAN_ERR_NONE;
    SetAudioPathConfigReq SetReq;
    memset( &SetReq, 0, sizeof(SetReq));

    LOGD("%s Entered function", __func__);

    SetReq.pNSEnable = &noiseSP;
    SetReq.Profile = AUDIO_PROFILE_HANDSET;

    nRet = SLQSSetAudioPathConfig (&SetReq);

    if ( nRet != eQCWWAN_ERR_NONE ) {
        LOGD("%s: SLQSSetAudioPathConfig() failed %lu", __func__, nRet);
        goto error;
    }


finally:
    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}
/**
 * qmirequestGetCOLP
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
void qmiRequestGetCOLP( const char **data, size_t datalen, RIL_Token t)
{

    ATResponse *atresponse = NULL;
    int err;
    int response[2];            // <n> and <m>
    char *line;
    char **responseCOLP;
    int i;
    ULONG nRet;
    voiceGetCOLPResp GetCOLPResp;
    COLPResp COLPInfo;

   responseCOLP = (char**)alloca(2* sizeof(char*));
    for (i = 0; i < 2; i++) {
        responseCOLP[i] = (char*)alloca(4* sizeof(char));
    }
   
    LOGD("%s::Entered\n", __func__);

    GetCOLPResp.pCOLPResp    = &COLPInfo;
    GetCOLPResp.pAlphaIDInfo = NULL;
    GetCOLPResp.pCCResType   = NULL;
    GetCOLPResp.pCallID      = NULL;
    GetCOLPResp.pFailCause   = NULL;
    GetCOLPResp.pCCSUPSType  = NULL;

    nRet = SLQSVoiceGetCOLP( &GetCOLPResp );
    if (eQCWWAN_ERR_NONE != nRet) {
        LOGE("%s:: SLQSVoiceGetCOLR failed, nRet : %lu\n", __func__, nRet);
        goto error;
    }    
    /* Active Status
     * 0x00 - ACTIVE_STATUS_INACTIVE - Inactive
     * 0x01 - ACTIVE_STATUS_ACTIVE - Active
     * 0xFF - Not Available
     */
    LOGD("%s::COLRInfo.ActiveStatus : %d\n", __func__, COLPInfo.ActiveStatus);

    /* ProvisionStatus
     * 0x00 - PROVISION_STATUS_NOT_PROVISIONED - Not provisioned
     * 0x01 - PROVISION_STATUS_PROVISIONED - Provisioned
     * 0xFF - Not Available
     */
    LOGD("%s::COLRInfo.ProvisionStatus : %d\n", __func__, COLPInfo.ProvisionStatus); 
   

   /** Check for successful active status of COLP */
    if(COLPInfo.ActiveStatus == 1)
    responseCOLP[1] = "1";
    else
    responseCOLP[1] = "0";
    /** Check for successful provisioned status of COLP */
    if(COLPInfo.ProvisionStatus == 1)
    responseCOLP[0] = "1";
    else
    responseCOLP[1] = "0";    

    RIL_onRequestComplete(t, RIL_E_SUCCESS, responseCOLP, 2 * sizeof(char *));

finally:
    at_response_free(atresponse);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;     
}

/**
 * qmiRequestGetCOLR
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
void qmiRequestGetCOLR( const char **data, size_t datalen, RIL_Token t)
{

   ATResponse *atresponse = NULL;
    int err;
    int response[2];            // <n> and <m>
    char *line;
    char **responseCOLR;
    int i;
    ULONG nRet;
    voiceGetCOLRResp GetCOLRResp;
    COLRResp COLRInfo;

   responseCOLR = (char**)alloca(2* sizeof(char*));
    for (i = 0; i < 2; i++) {
        responseCOLR[i] = (char*)alloca(4* sizeof(char));
    }
   
    LOGD("%s::Entered\n", __func__);

    GetCOLRResp.pCOLRResp    = &COLRInfo;
    GetCOLRResp.pAlphaIDInfo = NULL;
    GetCOLRResp.pCCResType   = NULL;
    GetCOLRResp.pCallID      = NULL;
    GetCOLRResp.pFailCause   = NULL;
    GetCOLRResp.pCCSUPSType  = NULL;

    nRet = SLQSVoiceGetCOLR( &GetCOLRResp );
    if (eQCWWAN_ERR_NONE != nRet) {
        LOGE("%s:: SLQSVoiceGetCOLR failed, nRet : %lu\n", __func__, nRet);
        goto error;
    }    
    /* Active Status
     * 0x00 - ACTIVE_STATUS_INACTIVE - Inactive
     * 0x01 - ACTIVE_STATUS_ACTIVE - Active
     * 0xFF - Not Available
     */
    LOGD("%s::COLRInfo.ActiveStatus : %d\n", __func__, COLRInfo.ActiveStatus);

    /* ProvisionStatus
     * 0x00 - PROVISION_STATUS_NOT_PROVISIONED - Not provisioned
     * 0x01 - PROVISION_STATUS_PROVISIONED - Provisioned
     * 0xFF - Not Available
     */
    LOGD("%s::COLRInfo.ProvisionStatus : %d\n", __func__, COLRInfo.ProvisionStatus); 
   

   /** Check for successful active status of COLP */
    if(COLRInfo.ActiveStatus == 1)
    responseCOLR[1] = "1";
    else
    responseCOLR[1] = "0";
    /** Check for successful provisioned status of COLP */
    if(COLRInfo.ProvisionStatus == 1)
    responseCOLR[0] = "1";
    else
    responseCOLR[0] = "0";    

    RIL_onRequestComplete(t, RIL_E_SUCCESS, responseCOLR, 2 * sizeof(char *));

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
 * requestOEMHookStringsSetAudioProfile
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
void requestOEMHookStringsSetAudioProfile(const char **data, size_t datalen, RIL_Token t) 
{
    ULONG nRet = eQCWWAN_ERR_NONE;
    SetAudioProfileReq SetReq;
    GetAudioProfileReq GetReq;
    GetAudioProfileResp GetRes;
    int dftvolume;

    char *str = (char*)alloca(10 * sizeof(char));
    strcpy(str, data[1]);
    BYTE profile = atoi(str);

    LOGD("%s: Entered, new profile %d", __func__, profile);

    if ((profile < AUDIO_PROFILE_HANDSET) || (profile > AUDIO_PROFILE_TTY) || (profile == AUDIO_PROFILE_AUX)) {
        LOGD("%s: Invalid profile selected!", __func__);
        goto error;
    }
 
    memset( &SetReq, 0, sizeof(SetReq));
    memset( &GetReq, 0, sizeof(GetReq));
    memset( &GetRes, 0, sizeof(GetRes));    
 
    GetReq.Generator = AUDIO_GENERATOR_VOICE;

    //Firstly we get the audio profile to get the values of all the parameters
    nRet = SLQSGetAudioProfile (&GetReq,&GetRes);
    if ( nRet != eQCWWAN_ERR_NONE ) {
        LOGD("%s: SLQSGetAudioProfile() failed %lu", __func__, nRet);
        goto error;
    }

    //Update only the mute parameter..other parameters to take their values as such.
    SetReq.Volume = GetRes.Volume;
    if(getDefaultVolumebyProfile(profile, &dftvolume)) {
        SetReq.Volume = dftvolume;
    }
    SetReq.EarMute = GetRes.EarMute;
    SetReq.MicMute = GetRes.MicMute;
    SetReq.Generator = AUDIO_GENERATOR_VOICE;
    SetReq.Profile = profile;
    nRet = SLQSSetAudioProfile (&SetReq);
    if ( nRet != eQCWWAN_ERR_NONE ) {
        LOGD("%s: SLQSSetAudioProfile() failed %lu", __func__, nRet);
        goto error;
    }
    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    return;
 
error:
     RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
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

    cur = (const char **) data;
    subcommand = atoi(*cur);

    switch(subcommand) {

        case SWI_CDMA_ACTIVATION:
            requestOEMHookStringsCMDAActQMI(cur, (datalen / sizeof(char *)), t);
            break;

        case SWI_DO_FACTORY_RESET:
            requestOEMHookStringsFactoryReset(cur, (datalen/sizeof(char *)), t);
            break;

        case SWI_PRL_UPDATE:
            requestOEMHookStringsPRLUpdate(cur, (datalen / sizeof(char *)), t);
            break;

        case SWI_PRL_CANCEL:
            requestOEMHookStringsCancelOmaDm(cur, (datalen / sizeof(char *)), t);
            break;

        case SWI_GET_SAR_STATE:
            requestOEMHookStringsGetRfSarState(cur, (datalen / sizeof(char *)), t);
            break;

        case SWI_SET_SAR_STATE:
            requestOEMHookStringsSetRfSarState(cur, (datalen / sizeof(char *)), t);
            break;

        case SWI_SET_SMS_WAKE:
            requestOEMHookStringsSetSMSWake(cur, (datalen / sizeof(char *)), t);
            break;

        case SWI_GET_ppDATAp:
            requestOEMHookStringsGetppData(cur, (datalen / sizeof(char *)), t);
            break;

        case SWI_SET_ppDATAp:
            requestOEMHookStringsSetppData(cur, (datalen / sizeof(char *)), t);
            break;

        case SWI_GET_CDMA_PROVISION_STATUS:
            requestOEMHookStringsGetCdmaProvisionStatus(cur, (datalen / sizeof(char *)), t);
            break;

        case SWI_VALIDATE_SPC:
            requestOEMHookStringsValidateSPC(cur, (datalen / sizeof(char *)), t);
            break;

        case SWI_READ_SIM_PB:
            requestOEMHookStringsReadSIMPB(cur, (datalen / sizeof(char *)), t);
            break;
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
       case SWI_OMA_DM_START_SESS:
	    requestOEMHookStringsOMADMStartSession(cur, (datalen / sizeof(char *)), t);
	    break;

       case SWI_OMA_DM_CANCEL_SESS:
	    requestOEMHookStringsOMADMCancelSession(cur, (datalen / sizeof(char *)), t);
	    break;
       case SWI_OMA_DM_SET_SELECTION:
	    requestOEMHookStringsOMADMSetSelection(cur, (datalen / sizeof(char *)), t);
	    break;  

       case SWI_CDMA_ACTIVATION_VERIZON_SL9090:
	    requestOEMHookStringsCMDAActVerizonSL9090(cur, (datalen / sizeof(char *)), t);
	    break;

        case SWI_VOICE_GET_CONFIG:
            requestOEMHookStringsVoiceGetConfig(cur, (datalen / sizeof(char *)), t);
        break;

        case SWI_SET_DDTM_PREF:
            requestOEMHookStringsSetDDTMPref(cur, (datalen / sizeof(char *)), t);
        break;

        case SWI_GET_AGPS_CONFIG:
            requestOEMHookStringsGetAGPSConfig(cur, (datalen / sizeof(char *)), t);
        break;

        case SWI_SET_AGPS_CONFIG:
            requestOEMHookStringsSetAGPSConfig(cur, (datalen / sizeof(char *)), t);
        break;

        case SWI_GET_3GPP2_INFO:
            requestOEMHookStringsGet3GPP2SubscriptionInfo(cur, (datalen / sizeof(char *)), t);
        break;

        case SWI_SET_3GPP2_INFO:
            requestOEMHookStringsSet3GPP2SubscriptionInfo(cur, (datalen / sizeof(char *)), t);
        break;

        case SWI_RESET_CDMA_RADIO_STATE:
            requestOEMHookStringsResetCdmaRadioState(cur, (datalen / sizeof(char *)), t);
        break;

        case SWI_GET_CDMA_CARRIER:
            requestOEMHookStringsGetCdmaCarrier(cur, (datalen / sizeof(char *)), t);
            break;

        case SWI_SET_AUDIO_PROFILE:
            requestOEMHookStringsSetAudioProfile(cur, (datalen / sizeof(char *)), t);
            break;

        default:
            /* Not a QMI-specific call, check the common possibilities */
            swicommon_requestOEMHookStrings( data, datalen, t );
        break;
    }
}
