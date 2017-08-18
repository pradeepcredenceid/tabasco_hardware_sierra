/**
 *
 * @ingroup swiril
 *
 * @file 
 * Definitions and function prototype for common OEM Hookstrings 
 * handler functions. 
 *
 * @author
 * Copyright: © 2011 Sierra Wireless, Inc.
 *            All rights reserved
 *
 */

/**
 * list of supported RIL_REQUEST_OEM_HOOK_STRINGS subcommand
 * Note: the negative value subcommand(s) reserved for Sierra 
 * internal test purpose only 
 */
typedef enum {
    SWI_AT_COMMAND_OPT      = -2, /**< Test support for AT cmd w/ sending option */
    SWI_CURRENT_STATE       = 1,  /**< invoke RIL currentState() */
    SWI_ON_SUPPORTS         = 2,  /**< invoke RIL onSupports() */
    SWI_ON_CANCEL           = 3,  /**< invoke RIL onCancel() */
    SWI_GET_VERSION         = 4,  /**< invoke RIL getVersion() */
    SWI_CDMA_ACTIVATION     = 5,  /**< CDMA Activation procedure */
    SWI_DO_FACTORY_RESET    = 6,  /**< Reset modem to factory defaults */
    SWI_PRL_UPDATE          = 7,  /**< Client-initiated PRL update request */
    SWI_GET_SAR_STATE       = 8,  /**< Client-initiated Get RF SAR status */
    SWI_SET_SAR_STATE       = 9,  /**< Client-initiated Set RF SAR status */
    SWI_SET_SMS_WAKE        = 10, /**< Call SetSMSWake() */
    SWI_GET_ppDATAp         = 11, /**< Query ##DATA# */
    SWI_SET_ppDATAp         = 12, /**< Set ##DATA# */
    SWI_VALIDATE_SPC        = 13,  /**< Validate SPC */
    SWI_READ_SIM_PB         = 14,  /**< Read the complete PB from SIM */
    SWI_PRL_CANCEL          = 15,  /**< Cancel the current OMADM session */
    SWI_GET_NOISE_SP        = 16,
    SWI_SET_NOISE_SP        = 17,
    SWI_GET_MODEM_VOL       = 18,
    SWI_SET_MODEM_VOL       = 19,
    SWI_GET_COLP            = 20, /**< Get COLP Status */
    SWI_GET_COLR            = 21, /**< Get COLR Status */
    SWI_SET_COLP            = 22, /**< Enable COLP */
    SWI_CLEAR_COLP          = 23, /**< Disable COLP */
    SWI_SET_COLR            = 24, /**< Enable COLR */
    SWI_CLEAR_COLR          = 25, /**< Disable COLR */
    SWI_OMA_DM_START_SESS   = 30,
    SWI_OMA_DM_CANCEL_SESS  = 31,
    SWI_OMA_DM_SET_SELECTION = 32,
    SWI_OMA_DM_GET_DESCRIPTION=33,
    SWI_CDMA_ACTIVATION_VERIZON_SL9090= 34,  /**< CDMA Activation procedure for Verizon on SL9090 */
    SWI_VOICE_GET_CONFIG    = 35, /** invoke requestOEMHookStringsGetAGPSConfig() */
    SWI_SET_DDTM_PREF       = 36, /** invoke requestOEMHookStringsSetDDTMPref() */
    SWI_GET_AGPS_CONFIG     = 37, /** invoke requestOEMHookStringsGetAGPSConfig() */
    SWI_SET_AGPS_CONFIG     = 38, /** invoke requestOEMHookStringsSetAGPSConfig() */
    SWI_GET_3GPP2_INFO      = 39, /** invoke requestOEMHookStringsGet3GPP2SubscriptionInfo() */
    SWI_SET_3GPP2_INFO      = 40, /** invoke requestOEMHookStringsSet3GPP2SubscriptionInfo() */
    SWI_GET_CDMA_PROVISION_STATUS= 41, /** invoke requestOEMHookStringsGetCdmaProvisionStatus() */
    SWI_RESET_CDMA_RADIO_STATE= 42, /** invoke requestOEMHookStringsResetCdmaRadioState() */
    SWI_GET_CDMA_CARRIER    = 43, /** invoke requestOEMHookStringsGetCdmaCarrier() */
    SWI_SET_AUDIO_PROFILE   = 44, /** invoke requestOEMHookStringsSetAudioProfile */
    SWI_CANCEL_QUERY_AVAILABLE_NETWORKS   = 45, /* Cancel QUERY Network */

} SWI_OEMHookType;

/**
 * list of supported RIL_UNSOL_OEM_HOOK_RAW subcommand
 * Note: the negative value subcommand(s) reserved for Sierra 
 * internal test purpose only 
 */
typedef enum {
    SWI_IND_OMADM_STATE             = 1, /**< invoke RIL_UNSOL_OEM_HOOK_RAW for OMADM (backward compatiblity) */
    SWI_IND_OMADM_STATE_FUMO        = 0,
    SWI_IND_OMADM_STATE_CONFIG      = 1, /**< invoke RIL_UNSOL_OEM_HOOK_RAW for OMADM */
    SWI_IND_MSG_WAIT_INFO           = 2, /**< invoke RIL_UNSOL_OEM_HOOK_RAW for WMS MSG_WAIT_IND */
    SWI_IND_OTASP_START_STATUS      = 3, /** <Start status of OTASP> */
    SWI_IND_OTASP_ACTIVATION_STATUS = 4, /** <Activation status of OTASP> */
    SWI_IND_DDTM_PREF_INFO          = 5, /** <DDTM Indication> */
    SWI_IND_REJ_CAUSE_INFO          = 6, /** <Rejct cause Indication> */
    SWI_IND_HLFOTA_STATE            = 7,
    SWI_IND_OMADM_STATE_NOTIF       = 8, /**< invoke RIL_UNSOL_OEM_HOOK_RAW for OMADM */
} SWI_UNSOL_OEMHookType;

/**
 * list of supported AT command sending options
 */
typedef enum {
    SWI_AT_CMD_OPT_0   = 0, /**< AT command with sending option withoutline */
    SWI_AT_CMD_OPT_1   = 1, /**< AT command with sending option single line */
    SWI_AT_CMD_OPT_2   = 2, /**< AT command with sending option multi line */
    SWI_AT_CMD_OPT_3   = 3, /**< AT command with sending option multi value */
    SWI_AT_CMD_OPT_4   = 4, /**< AT command with sending option numeric value */
} SWI_ATSendOption;

/* External Definitions */
extern void swicommon_requestOEMHookStrings(void *data,size_t datalen,RIL_Token t);
