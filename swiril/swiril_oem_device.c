/**
 *
 * @ingroup swiril
 *
 * @file 
 * Provides OEM device specific Sierra functions
 *
 * @author
 * Copyright: (C) 2012 Sierra Wireless, Inc.
 *            All rights reserved
 *
 */

#include <telephony/ril.h>
#include <stdbool.h>
#include <string.h>
#include "swiril_main.h"
#include "swiril_misc_qmi.h"
#include "swiril_sim_qmi.h"
#include "swiril_network_qmi.h"
#include "at_channel.h"
#include "at_tok.h"

/* For QMI */
#include "qmerrno.h"
#include "SWIWWANCMAPI.h"

#define LOG_TAG "RIL"
#include "swiril_log.h"

/**< MIP profile index set to 1 now */
#define SWI_MIP_PROFILE_INDEX 1
#define MAX_NAME_LEN_IN_WORDS 256

/* Both Get and Set parameters use string array and the length in comments below excluding '\0' at the end of string */
typedef enum {
    OEM_PPDATAP_USER_NAI = 0,           /**< NAI string of MIP profile, ASCII string */
    OEM_PPDATAP_REV_TUNNELING_SETTING,  /**< Reverse Tunneling of MIP profile, one byte long decimal string */
    OEM_PPDATAP_HA_SPI,                 /**< HA SPI of MIP profile, up to 4 bytes long hexadecimal value string */
    OEM_PPDATAP_AAA_SPI,                /**< AAA SPI of MIP profile, up to 4 bytes long hexadecimal value string */
    OEM_PPDATAP_HOME_IP,                /**< Home address(IPv4) of MIP profile, up to 4 bytes long hexadecimal value string */
    OEM_PPDATAP_PRIMARY_HA_IP,          /**< HA address(IPv4) of MIP profile, up to 4 bytes long hexadecimal value string */
    OEM_PPDATAP_SECONDARY_HA_IP,        /**< AAA address(IPv4) of MIP profile, up to 4 bytes long hexadecimal value string */
    OEM_PPDATAP_MEID,                   /**< MEID, ASCII string */
    OEM_PPDATAP_P_ESN,                  /**< ESN, ASCII string */
    OEM_PPDATAP_STATION_CLASS_MARK,     /**< Station Class Mark, one byte long decimal string */
    OEM_PPDATAP_SLOTTED_MODE_INDEX,     /**< Slot Cycle Index, one byte long decimal string */
    OEM_PPDATAP_HOME_NID,               /**< Home NID, one byte long decimal string */
    OEM_PPDATAP_HOME_SID,               /**< Home SID, one byte long decimal string */
    OEM_PPDATAP_ACCOLC,                 /**< ACCOLC, one byte long decimal string */
    OEM_PPDATAP_HOME_SYS_REG,           /**< Register on home system, one byte long decimal string */
    OEM_PPDATAP_FSID,                   /**< Register on foreign system, one byte long decimal string */
    OEM_PPDATAP_FNID,                   /**< Register on foreign network, one byte long decimal string */
    OEM_PPDATAP_1X_CHANNEL,             /**< 1xrtt channel number, up to 4 bytes long decimal string */
    OEM_PPDATAP_1X_BAND_CLASS,          /**< 1xrtt band class, up to 4 bytes long decimal string */
    OEM_PPDATAP_1X_SYSTEM_ID,           /**< 1xrtt system ID, up to 4 bytes long decimal string */
    OEM_PPDATAP_1X_NETWORK_ID,          /**< 1xrtt network ID, up to 4 bytes long decimal string */
    OEM_PPDATAP_1X_BASE_ID,             /**< 1xrtt base ID, up to 4 bytes long decimal string */
    OEM_PPDATAP_1X_LATITUDE,            /**< 1xrtt base station latitude, up to 4 bytes long hexadecimal value string */
    OEM_PPDATAP_1X_LONGTITUDE,          /**< 1xrtt base station longtitude, up to 4 bytes long hexadecimal value string */
    OEM_PPDATAP_1X_RX_PWR,              /**< 1xrtt RSSI, up to 4 bytes long decimal string */
    OEM_PPDATAP_1X_RX_ECIO,             /**< 1xrtt ECIO, up to 4 bytes long decimal string */
    OEM_PPDATAP_1X_RX_ERROR_RATE,       /**< 1xrtt error rate, up to 4 bytes long decimal string */
    OEM_PPDATAP_EVDO_CHANNEL,           /**< EVDO channel number, up to 4 bytes long decimal string */
    OEM_PPDATAP_EVDO_RX_PWR,            /**< EVDO RSSI, up to 4 bytes long decimal string */
    OEM_PPDATAP_EVDO_RX_PER,            /**< EVDO Packet Error Rate, up to 4 bytes long decimal string */
    OEM_PPDATAP_EVDO_RX_SINR,           /**< EVDO SINR, up to 4 bytes long decimal string */
    OEM_PPDATAP_EVDO_AN_AAA_STATUS,     /**< EVDO AN-AAA status, up to 4 bytes long decimal string */
    OEM_PPDATAP_EVDO_PRL,               /**< PRL version, up to 4 bytes long decimal string */
    OEM_PPDATAP_AMSS_VERSION,           /**< Firmware version, ASCII string */
    OEM_PPDATAP_P_REV,                  /**< Protocol version, up to 4 bytes long decimal string */
    OEM_PPDATAP_PACKET_DATA_PROFILE,    /**< Active MIP profile number, one byte long decimal string */
    OEM_PPDATAP_AT_DEBUG1,              /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG2,              /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG3,              /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG4,              /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG5,              /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG6,              /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG7,              /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG8,              /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG9,              /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG10,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG11,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG12,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG13,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG14,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG15,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG16,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG17,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG18,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG19,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG20,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG21,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG22,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG23,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG24,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG25,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG26,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG27,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG28,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG29,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG30,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG31,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG32,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG33,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG34,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG35,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG36,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG37,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG38,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG39,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG40,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG41,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG42,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG43,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG44,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG45,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG46,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG47,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG48,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG49,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG50,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG51,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG52,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG53,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG54,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG55,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG56,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_AT_DEBUG57,             /**< AT$DEBUG? response string <n> */
    OEM_PPDATAP_MCC,                    /** Mobile Country Code */
    OEM_PPDATAP_MNC,                    /** Mobile Network Code */
    OEM_PPDATAP_MAX
}OEM_HOOK_STRINGS_GET_PPDATAP;

typedef enum {
    OEM_SET_PPDATAP_SPC = 0,                /**< Service Programming Code, expecting up to 6 bytes long decimal value string */
    OEM_SET_PPDATAP_USER_NAI,               /**< NAI string of MIP profile, expecting ASCII string */
    OEM_SET_PPDATAP_REV_TUNNELING_SETTING,  /**< Reverse Tunneling of MIP profile, expecting one byte long decimal string */
    OEM_SET_PPDATAP_HA_SPI,                 /**< HA SPI of MIP profile, expecting up to 4 bytes long hexadecimal value string */
    OEM_SET_PPDATAP_HA_PASSWORD,            /**< HA password of MIP profile, expecting ASCII string */
    OEM_SET_PPDATAP_AAA_SPI,                /**< AAA SPI of MIP profile, expecting up to 4 bytes long hexadecimal value string */
    OEM_SET_PPDATAP_AAA_PASSWORD,           /**< AAA password of MIP profile, expecting ASCII string */
    OEM_SET_PPDATAP_HOME_IP,                /**< Home address(IPv4) of MIP profile, expecting up to 4 bytes long hexadecimal value string */
    OEM_SET_PPDATAP_PRIMARY_HA_IP,          /**< Primary HA address(IPv4) of MIP profile, expecting up to 4 bytes long hexadecimal value string */
    OEM_SET_PPDATAP_SECONDARY_HA_IP,        /**< Secondary HA address(IPv4) of MIP profile, expecting up to 4 bytes long hexadecimal value string */
    OEM_SET_PPDATAP_ACCOLC,                 /**< ACCOLC, expecting one byte long decimal string */
    OEM_SET_PPDATAP_MAX
}OEM_HOOK_STRINGS_SET_PPDATAP;

/* function prototypes */
extern void getPrevDDTMPreference(struct DDTMSettings* pPrevDDTMPref );
extern bool getFirmwareInfoCarrierQMI(enum eGobiImageCarrier *image);

/**
 *  Device programming command ##DATA# get handler
 *
 *  @param  data [in]
 *          - Pointer to the string pointer array
 *  @param  count [in]
 *          - count of the number of strings pointed to by the data array
 *  @param  t [in]
 *          - A RIL token structure pointer
 *
 * @return
 *     none
 *
 * @par Abort:<br> 
 *     none
 *
 * @note
 *     none
 *
 */
void requestOEMHookStringsGetppData(const char **data, size_t count, RIL_Token t)
{
    ULONG nRet;
    int i;
    char *responseStrp[OEM_PPDATAP_MAX];
    
    BYTE Enabled;
    ULONG Address;
    ULONG PrimaryHA;
    ULONG SecondaryHA;
    BYTE RevTunneling;
    CHAR NAI[NUM_MAX_STR_LEN];
    ULONG HASPI;
    ULONG AAASPI;
    ULONG HAState;
    ULONG AAAState;
    
    CHAR  ESNString[ESN_STRING_LENGTH];
    CHAR  IMEIString[IMEI_STRING_LENGTH];
    CHAR  MEIDString[MEID_STRING_LENGTH];
    
    int prefType = 0;

    WORD  pMCC;
    WORD  pMNC;
    CHAR  pName[NUM_MAX_STR_LEN];
    WORD  pHomeSID;
    WORD  pHomeNID;

    BYTE  ACCOLC;

    BYTE SCI;
    BYTE SCM;
    BYTE RegHomeSID;
    BYTE RegForeignSID;
    BYTE RegForeignNID;
    BYTE ForceRev0;
    BYTE CustomSCP;
    ULONG Protocol;
    ULONG Broadcast;
    ULONG Application;
    ULONG Roaming;
    
    ATResponse *atresponse = NULL;
    int err;
    char *line;
    int profile;

    qaQmiServingSystemParam tqaQmiServingSystemParam;

    BYTE  instancesSize = RAT_MAX;
    struct RFBandInfoElements instances[RAT_MAX];

    struct slqsSignalStrengthInfo signalInfo;
    
    ULONG AAAStatus;
    
    WORD  prlVersion;
    
    CHAR fmrString[NUM_MAX_STR_LEN];
    
    /* Subtract OEM hook sub-command */
    count--;

    /* No argument is the only acceptable choice */
    if( count != 0 ) {
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        LOGE("%s - wrong number of args: %d", __func__, count+1);
        return;
    }

    memset(responseStrp, 0, sizeof(responseStrp));
    
    /* Mobile IP profile */
    nRet = GetMobileIPProfile(SWI_MIP_PROFILE_INDEX,
                              &Enabled,
                              &Address,
                              &PrimaryHA,
                              &SecondaryHA,
                              &RevTunneling,
                              sizeof(NAI),
                              &NAI[0],
                              &HASPI,
                              &AAASPI,
                              &HAState,
                              &AAAState);
    if (nRet == eQCWWAN_ERR_NONE) {
        asprintf(&responseStrp[OEM_PPDATAP_USER_NAI], "%s", NAI);
        asprintf(&responseStrp[OEM_PPDATAP_REV_TUNNELING_SETTING], "%d", RevTunneling);
        asprintf(&responseStrp[OEM_PPDATAP_HA_SPI], "%lx", HASPI);
        asprintf(&responseStrp[OEM_PPDATAP_AAA_SPI], "%lx", AAASPI);
        asprintf(&responseStrp[OEM_PPDATAP_HOME_IP], "%lx", Address);
        asprintf(&responseStrp[OEM_PPDATAP_PRIMARY_HA_IP], "%lx", PrimaryHA);
        asprintf(&responseStrp[OEM_PPDATAP_SECONDARY_HA_IP], "%lx", SecondaryHA);
    }
    else {
        LOGE("%s GetMobileIPProfile error  %lu", __func__, nRet);
        //goto error;
    }
    
    /* MEID, ESN */
    nRet = GetSerialNumbers(sizeof(ESNString),
                            &ESNString[0],
                            sizeof(IMEIString),
                            &IMEIString[0],
                            sizeof(MEIDString),
                            &MEIDString[0]); 
    if (nRet == eQCWWAN_ERR_NONE) {
        asprintf(&responseStrp[OEM_PPDATAP_MEID], "%s", MEIDString);
        asprintf(&responseStrp[OEM_PPDATAP_P_ESN], "%s", ESNString);
    }
    else {
        LOGE("%s GetSerialNumbers error  %lu", __func__, nRet);
        goto error;
    }

    /* Home NID/SID */
    nRet = GetHomeNetwork(&pMCC,
                          &pMNC,
                          sizeof(pName),
                          &pName[0],
                          &pHomeSID,
                          &pHomeNID);
    if (nRet == eQCWWAN_ERR_NONE) {
        asprintf(&responseStrp[OEM_PPDATAP_HOME_NID], "%d", pHomeNID);
        asprintf(&responseStrp[OEM_PPDATAP_HOME_SID], "%d", pHomeSID);
        asprintf(&responseStrp[OEM_PPDATAP_MCC], "%d", pMCC);
        asprintf(&responseStrp[OEM_PPDATAP_MNC], "%d", pMNC);
    }
    else {
        LOGE("%s GetHomeNetwork error  %lu", __func__, nRet);
        goto error;
    }
    
    /* ACCOLC */
    nRet = GetACCOLC(&ACCOLC);
    if (nRet == eQCWWAN_ERR_NONE) {
        asprintf(&responseStrp[OEM_PPDATAP_ACCOLC], "%d", ACCOLC);
    }
    else {
        LOGE("%s GetACCOLC error  %lu", __func__, nRet);
        goto error;
    }
    
    /* SCM, SCI, Foreign SID/NID */
    nRet = GetCDMANetworkParameters(&SCI, 
                                    &SCM, 
                                    &RegHomeSID, 
                                    &RegForeignSID, 
                                    &RegForeignNID, 
                                    &ForceRev0, 
                                    &CustomSCP, 
                                    &Protocol, 
                                    &Broadcast, 
                                    &Application, 
                                    &Roaming );

    if (nRet == eQCWWAN_ERR_NONE) {
        asprintf(&responseStrp[OEM_PPDATAP_STATION_CLASS_MARK], "%d", SCM);
        asprintf(&responseStrp[OEM_PPDATAP_SLOTTED_MODE_INDEX], "%d", SCI);
        asprintf(&responseStrp[OEM_PPDATAP_HOME_SYS_REG], "%d", RegHomeSID);
        asprintf(&responseStrp[OEM_PPDATAP_FSID], "%d", RegForeignSID);
        asprintf(&responseStrp[OEM_PPDATAP_FNID], "%d", RegForeignNID);
    }
    else {
        LOGE("%s GetCDMANetworkParameters error  %lu", __func__, nRet);
        goto error;
    }

    /* SID, NID, base ID, latitude and longitude */
    nRet = SLQSGetServingSystem(&tqaQmiServingSystemParam);
    if (nRet == eQCWWAN_ERR_NONE) {
        asprintf(&responseStrp[OEM_PPDATAP_1X_SYSTEM_ID], "%d", tqaQmiServingSystemParam.SystemID);
        asprintf(&responseStrp[OEM_PPDATAP_1X_NETWORK_ID], "%d", tqaQmiServingSystemParam.NetworkID);
        asprintf(&responseStrp[OEM_PPDATAP_1X_BASE_ID], "%d", tqaQmiServingSystemParam.BasestationID);

        double latitude;
        if(tqaQmiServingSystemParam.BasestationLatitude < 2592000) {
            latitude = ((double)tqaQmiServingSystemParam.BasestationLatitude) / (3600 * 4);
            asprintf(&responseStrp[OEM_PPDATAP_1X_LATITUDE], "%f", latitude);
        }

        double longitude;
        if(tqaQmiServingSystemParam.BasestationLongitude < 2592000) {
            longitude = ((double)tqaQmiServingSystemParam.BasestationLongitude) / (3600 * 4);
            asprintf(&responseStrp[OEM_PPDATAP_1X_LONGTITUDE], "%f", longitude);
        }
    }
    else {
        LOGE("%s SLQSGetServingSystem error  %lu", __func__, nRet);
        goto error;
    }

    /* channel number, band class */
    nRet = GetRFInfo(&instancesSize, (BYTE *)&instances[0]);
    if (nRet == eQCWWAN_ERR_NONE) {
        for (i = 0; i < instancesSize; i++) {
            if (instances[i].radioInterface == RAT_CDMA_1XRTT) {
                asprintf(&responseStrp[OEM_PPDATAP_1X_CHANNEL], "%lu", instances[i].activeChannel);
                asprintf(&responseStrp[OEM_PPDATAP_1X_BAND_CLASS], "%lu", instances[i].activeBandClass);
            }
            else if (instances[i].radioInterface == RAT_CDMA_1XEVDO) {
                asprintf(&responseStrp[OEM_PPDATAP_EVDO_CHANNEL], "%lu", instances[i].activeChannel);
            }
        }
    }
    else if (nRet == eQCWWAN_ERR_QMI_INFO_UNAVAILABLE) {
        LOGD("%s GetRFInfo return %lu - not register yet", __func__, nRet);
    }
    else {
        LOGE("%s GetRFInfo error  %lu", __func__, nRet);
        goto error;
    }
    
    /* RSSI, ECIO, error rate, and SINR */
    signalInfo.signalStrengthReqMask = 0xFF; /* Mask all the bits of "Request Mask" to retrieve complete info */
    nRet = SLQSGetSignalStrength( &signalInfo );
    if (nRet == eQCWWAN_ERR_NONE) {
        for (i = 0; i < signalInfo.rxSignalStrengthListLen; i++) {
            if (signalInfo.rxSignalStrengthList[i].radioIf == RAT_CDMA_1XRTT) {
                asprintf(&responseStrp[OEM_PPDATAP_1X_RX_PWR], "%d", signalInfo.rxSignalStrengthList[i].rxSignalStrength);
            }
            else if (signalInfo.rxSignalStrengthList[i].radioIf == RAT_CDMA_1XEVDO) {
                asprintf(&responseStrp[OEM_PPDATAP_EVDO_RX_PWR], "%d", signalInfo.rxSignalStrengthList[i].rxSignalStrength);
            }
        }
        for (i = 0; i < signalInfo.ecioListLen; i++) {
            if (signalInfo.ecioList[i].radioIf == RAT_CDMA_1XRTT) {
                asprintf(&responseStrp[OEM_PPDATAP_1X_RX_ECIO], "%d", signalInfo.ecioList[i].ecio);
                break;
            }
        }
        for (i = 0; i < signalInfo.errorRateListLen; i++) {
            if (signalInfo.errorRateList[i].radioIf == RAT_CDMA_1XRTT) {
                asprintf(&responseStrp[OEM_PPDATAP_1X_RX_ERROR_RATE], "%d", signalInfo.errorRateList[i].errorRate);
            }
            else if (signalInfo.errorRateList[i].radioIf == RAT_CDMA_1XEVDO) {
                asprintf(&responseStrp[OEM_PPDATAP_EVDO_RX_PER], "%d", signalInfo.errorRateList[i].errorRate);
            }
        }
        asprintf(&responseStrp[OEM_PPDATAP_EVDO_RX_SINR], "%d", signalInfo.sinr);
    }
    else {
        LOGE("%s SLQSGetSignalStrength error  %lu", __func__, nRet);
        goto error;
    }
    
    /* AN-AAA */
    nRet = GetANAAAAuthenticationStatus(&AAAStatus);
    if (nRet == eQCWWAN_ERR_NONE) {
        asprintf(&responseStrp[OEM_PPDATAP_EVDO_AN_AAA_STATUS], "%lu", AAAStatus);
    }
    else {
        LOGE("%s GetANAAAAuthenticationStatus error  %lu", __func__, nRet);
        goto error;
    }
    
    /* PRL */
    nRet = GetPRLVersion(&prlVersion);
    if (nRet == eQCWWAN_ERR_NONE) {
        asprintf(&responseStrp[OEM_PPDATAP_EVDO_PRL], "%d", prlVersion);
    }
    else {
        LOGE("%s GetPRLVersion error  %lu", __func__, nRet);
        //goto error;
    }
    
    /* Firmware version */
    nRet = GetFirmwareRevision(sizeof(fmrString), &fmrString[0]);
    if (nRet == eQCWWAN_ERR_NONE) {
        asprintf(&responseStrp[OEM_PPDATAP_AMSS_VERSION], "%s", fmrString);
    }
    else {
        LOGE("%s GetFirmwareRevision error  %lu", __func__, nRet);
        goto error;
    }

    /* Get remaining parameters using AT$DEBUG */
    err = at_send_command_multivalue("AT$DEBUG?", "1x",&atresponse);

    if (err < 0 || atresponse->success == 0) {
        LOGE("%s AT$DEBUG? failed with %d", __func__, err);
    } else {
        ATLine *p_line = atresponse->p_intermediates;
        i = 0;
        while (p_line != NULL) {
            asprintf(&responseStrp[OEM_PPDATAP_AT_DEBUG1 + i++], "%s", p_line->line);
            p_line = p_line->p_next;
        }
    }
    at_response_free(atresponse);

    /* Protocol version */
    err = at_send_command_numeric("AT$QCPREV", &atresponse);

    if (err < 0 || atresponse->success == 0) {
        LOGE("%s AT$QCPREV failed with %d", __func__, err);
        at_response_free(atresponse);
        goto error;
    }
    asprintf(&responseStrp[OEM_PPDATAP_P_REV], "%d", atoi(atresponse->p_intermediates->line));
    at_response_free(atresponse);

    /* Active MIP profile */
    err = at_send_command_singleline("AT$QCMIPP?", "$QCMIPP:", &atresponse);

    if (err < 0 || atresponse->success == 0) {
        LOGE("%s AT$QCMIPP? failed with %d", __func__, err);
        at_response_free(atresponse);
        goto error;
    }
    line = atresponse->p_intermediates->line;

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &profile);
    if (err < 0)
        goto error;
    asprintf(&responseStrp[OEM_PPDATAP_PACKET_DATA_PROFILE], "%d", profile);
    at_response_free(atresponse);
    
    LOGD("%s print response", __func__);
    for (i = 0; i < OEM_PPDATAP_MAX; i++) {
        LOGD("%d '%s' ", i, responseStrp[i]);
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, responseStrp, OEM_PPDATAP_MAX * sizeof(char *));
    
finally:
    for (i = 0; i < OEM_PPDATAP_MAX; i++) {
        if (responseStrp[i] != NULL)
            free(responseStrp[i]);
    }
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 *  Device programming command ##DATA# set handler
 *
 *  @param  data [in]
 *          - Pointer to the string pointer array
 *  @param  count [in]
 *          - count of the number of strings pointed to by the data array
 *  @param  t [in]
 *          - A RIL token structure pointer
 *
 * @return
 *     none
 *
 * @par Abort:<br> 
 *     none
 *
 * @note
 *     none
 *
 */
void requestOEMHookStringsSetppData(const char **data, size_t count, RIL_Token t)
{
    const char **args = data;
    int i;
    ULONG nRet;
    CHAR  *SPCp;
    CHAR  *NAIp = NULL;
    ULONG homeIP;
    ULONG *homeIPp = NULL;
    ULONG primaryHA;
    ULONG *primaryHAp = NULL;
    ULONG secondaryHA;
    ULONG *secondaryHAp = NULL;
    BYTE  revTunneling;
    BYTE  *revTunnelingp = NULL;
    ULONG HASPI;
    ULONG *HASPIp = NULL;
    ULONG AAASPI;
    ULONG *AAASPIp = NULL;
    CHAR  *HAPasswordp = NULL;
    CHAR  *AAAPasswordp = NULL;
    bool hasACCOLC = false;
    BYTE  ACCOLC;
    
    /* Advance to the next argument */
    args++;
    count--;

    /* Argument number has to match required set list number */
    if( count != OEM_SET_PPDATAP_MAX ) {
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        LOGE("%s - wrong number of args: %d", __func__, count+1);
        return;
    }
    
    LOGD("%s print input", __func__);
    for (i = 0; i < OEM_SET_PPDATAP_MAX; i++) {
        LOGD("%d '%s' ", i, args[i]);
    }
    
    /* Check and assign parameters. */
    if (args[OEM_SET_PPDATAP_SPC] == NULL) {
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        LOGE("%s - SPC not given!!", __func__);
        return;
    }
    SPCp = (char *)args[OEM_SET_PPDATAP_SPC];
    if (args[OEM_SET_PPDATAP_USER_NAI] != NULL)
        NAIp = (char *)args[OEM_SET_PPDATAP_USER_NAI];
    if (args[OEM_SET_PPDATAP_REV_TUNNELING_SETTING] != NULL) {
        revTunneling = atoi(args[OEM_SET_PPDATAP_REV_TUNNELING_SETTING]);
        revTunnelingp = &revTunneling;
    }
    if (args[OEM_SET_PPDATAP_HA_SPI] != NULL) {
        HASPI = strtoul(args[OEM_SET_PPDATAP_HA_SPI], NULL, 16);
        HASPIp = &HASPI;
    }
    if (args[OEM_SET_PPDATAP_HA_PASSWORD] != NULL) {
        HAPasswordp = (char *)args[OEM_SET_PPDATAP_HA_PASSWORD];
    }
    if (args[OEM_SET_PPDATAP_AAA_SPI] != NULL) {
        AAASPI = strtoul(args[OEM_SET_PPDATAP_AAA_SPI], NULL, 16);
        AAASPIp = &AAASPI;
    }

    if (args[OEM_SET_PPDATAP_AAA_PASSWORD] != NULL) {
        AAAPasswordp = (char *)args[OEM_SET_PPDATAP_AAA_PASSWORD];
    }
    if (args[OEM_SET_PPDATAP_HOME_IP] != NULL) {
        homeIP = strtoul(args[OEM_SET_PPDATAP_HOME_IP], NULL, 16);
        homeIPp = &homeIP;
    }
    if (args[OEM_SET_PPDATAP_PRIMARY_HA_IP] != NULL) {
        primaryHA = strtoul(args[OEM_SET_PPDATAP_PRIMARY_HA_IP], NULL, 16);
        primaryHAp = &primaryHA;
    }
    if (args[OEM_SET_PPDATAP_SECONDARY_HA_IP] != NULL) {
        secondaryHA = strtoul(args[OEM_SET_PPDATAP_SECONDARY_HA_IP], NULL, 16);
        secondaryHAp = &secondaryHA;
    }
    if (args[OEM_SET_PPDATAP_ACCOLC] != NULL) {
        ACCOLC = atoi(args[OEM_SET_PPDATAP_ACCOLC]);
        hasACCOLC = true;
    }

    /* Mobile IP profile */
    nRet = SetMobileIPProfile(SPCp,
                              SWI_MIP_PROFILE_INDEX,
                              NULL,
                              homeIPp,
                              primaryHAp,
                              secondaryHAp,
                              revTunnelingp,
                              NAIp,
                              HASPIp,
                              AAASPIp,
                              HAPasswordp,
                              AAAPasswordp);
    if( (nRet != eQCWWAN_ERR_NONE) &&
        (nRet != eQCWWAN_ERR_QMI_NO_EFFECT) ) {
        LOGE("%s SetMobileIPProfile error %lu", __func__, nRet);
        goto error;
    }
    
    /* ACCOLC */
    if (hasACCOLC) {
        nRet = SetACCOLC(SPCp, ACCOLC);
        if( (nRet != eQCWWAN_ERR_NONE) &&
            (nRet != eQCWWAN_ERR_QMI_NO_EFFECT) ) {
            LOGE("%s SetACCOLC error %lu", __func__, nRet);
            goto error;
        }
    }
    
    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

/**
 *  Device programming command get provision status handler
 *
 *  @param  data [in]
 *          - Pointer to the string pointer array
 *  @param  count [in]
 *          - count of the number of strings pointed to by the data array
 *  @param  t [in]
 *          - A RIL token structure pointer
 *
 * @return
 *     none
 *
 * @par Abort:<br> 
 *     none
 *
 * @note
 *     none
 *
 */
void requestOEMHookStringsGetCdmaProvisionStatus(const char **data, size_t count, RIL_Token t)
{
    ULONG nRet;
    char *responseStr[3];
    CHAR fmrString[NUM_MAX_STR_LEN];
    enum eGobiImageCarrier image;
    
    /* Subtract OEM hook sub-command */
    count--;
    
    /* No argument is the only acceptable choice */
    if( count != 0 ) {
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        LOGE("%s - wrong number of args: %d", __func__, count+1);
        return;
    } 
    
    memset(responseStr, 0, sizeof(responseStr));  

    /* Firmware version */
    nRet = GetFirmwareRevision(sizeof(fmrString), &fmrString[0]);
    if (nRet != eQCWWAN_ERR_NONE) {       
        LOGE("%s GetFirmwareRevision error  %lu", __func__, nRet);
        goto error;
    }   

    /* Get provision status: 
       SLQSOMADMGetSessionInfoExt is applicable only for SPRINT */
    if ((0 != strstr(fmrString,"SWI6600S")) ||
       ((0 != strstr(fmrString,"SWI6600H")) &&
       ((getFirmwareInfoCarrierQMI(&image)) && (image == eGOBI_IMG_CAR_SPRINT)))) {
        SLQSOMADMSessionInfoExt sessionInfo;
        BYTE  OMAModInfoResult = 0xFF;
        BYTE  SessionState;
        BYTE  HFAStatus;
        WORD  HFAMaxretryTime;
        WORD  RetryInterval;
        WORD  RetryingTimes;
        BYTE  FUMOState;
        ULONG FUMOResultCode;
        WORD  PkgVendorNameLength = MAX_NAME_LEN_IN_WORDS;
        BYTE  PkgVendorName[2 * MAX_NAME_LEN_IN_WORDS];
        ULONG PkgSize;
        WORD  PkgVersionLength = MAX_NAME_LEN_IN_WORDS;
        BYTE  PkgVersion[2 * MAX_NAME_LEN_IN_WORDS];
        WORD  PkgNameLength = MAX_NAME_LEN_IN_WORDS;
        BYTE  PkgName[2 * MAX_NAME_LEN_IN_WORDS];
        WORD  PkgDescLength = MAX_NAME_LEN_IN_WORDS;
        BYTE  PkgDescription[2 * MAX_NAME_LEN_IN_WORDS];
        WORD  PkgDateLength = MAX_NAME_LEN_IN_WORDS;
        BYTE  PkgDate[2 * MAX_NAME_LEN_IN_WORDS];
        WORD  PkgInstallTimeLength = MAX_NAME_LEN_IN_WORDS;
        BYTE  PkgInstallTime[2 * MAX_NAME_LEN_IN_WORDS];

        memset(&sessionInfo,0,sizeof(sessionInfo));
        memset(PkgVendorName,0,sizeof(PkgVendorName));
        memset(PkgVersion,0,sizeof(PkgVersion));
        memset(PkgName,0,sizeof(PkgName));
        memset(PkgDescription,0,sizeof(PkgDescription));
        memset(PkgDate,0,sizeof(PkgDate));
        memset(PkgInstallTime,0,sizeof(PkgInstallTime));

        sessionInfo.pOMAModInfoResult = &OMAModInfoResult;
        sessionInfo.pSessionState = &SessionState;
        sessionInfo.pHFAStatus = &HFAStatus;
        sessionInfo.pHFAMaxretryTime = &HFAMaxretryTime;
        sessionInfo.pRetryInterval = &RetryInterval;
        sessionInfo.pRetryingTimes = &RetryingTimes;
        sessionInfo.pFUMOState = &FUMOState;
        sessionInfo.pFUMOResultCode = &FUMOResultCode;
        sessionInfo.pPkgVendorNameLength = &PkgVendorNameLength;
        sessionInfo.pPkgVendorName = PkgVendorName;
        sessionInfo.pPkgSize = &PkgSize;
        sessionInfo.pPkgVersionLength = &PkgVersionLength;
        sessionInfo.pPkgVersion = PkgVersion;
        sessionInfo.pPkgNameLength = &PkgNameLength;
        sessionInfo.pPkgName = PkgName;
        sessionInfo.pPkgDescLength = &PkgDescLength;
        sessionInfo.pPkgDescription = PkgDescription;
        sessionInfo.pPkgDateLength = &PkgDateLength;
        sessionInfo.pPkgDate = PkgDate;
        sessionInfo.pPkgInstallTimeLength = &PkgInstallTimeLength;
        sessionInfo.pPkgInstallTime = PkgInstallTime;

        nRet = SLQSOMADMGetSessionInfoExt( &sessionInfo);
        if (nRet == eQCWWAN_ERR_NONE) {
            LOGE("%s SLQSOMADMGetSessionInfoExt: OMADMInfoResult %d", __func__,*(sessionInfo.pOMAModInfoResult));

            /* Deciding provision status based on pOMAModInfoResult, pHFAStatus and pFUMOState
             *
             *  OMAModInfoResult:
             *    0 – OMA module initiation not completed.
             *    1 – OMA module initiation completed but no UI is registered.
             *    128 – OMA initiation completed.
             *    255 – OMA initiation error.
             *
             *  HFAStatus:
             *    0 – HFA completed
             *    1 – HFA not start
             *    2 – HFA DC completed, but PRL is not completed
             *    3 – HFA PRL completed, but FUMO is not completed
             *
             *  FUMOState:
             *    0 – FUMO state READY
             *    1 – FUMO state DISCOVERY
             *    2 –  FUMO state DISCOCONF
             *    3 –  FUMO state DOWNLOAD
             *    4 – FUMO state RECEIVED
             *    5 – FUMO state CONFIRM
             *    6 – FUMO state CONFIRMED
             *    7 – FUMO state UPDATE
             *    8 – FUMO state UPDATING
             *    9 – FUMO state UNUSABLE
             *    10 – FUMO state REJECTED
             *    11 – FUMO state REJECTCONF
             *    12 – FUMO state FAILED
             *    13 – FUMO state DENIED
             *    14 – FUMO state UPDATED
             *    15 – FUMO state FINCONF
             *    16 – FUMO state FINISHED
             */                  
            if(((*(sessionInfo.pOMAModInfoResult)) == 128) || (*(sessionInfo.pOMAModInfoResult)) == 1) {
                if ((HFAStatus == 0) || (HFAStatus == 2)|| (HFAStatus == 3))
                {
                    asprintf(&responseStr[0], "%s", "Provisioned");
                }
                else {
                    asprintf(&responseStr[0], "%s", "Not provisioned");
                }
            }
            else {
                asprintf(&responseStr[0], "%s", "Not provisioned");
            }
            asprintf(&responseStr[1], "%d", SessionState);
            asprintf(&responseStr[2], "%d", RetryingTimes);
            LOGE("%s Get OMAModInfoResult  %d"      , __func__, OMAModInfoResult );
            LOGE("%s Get HFAStatus  %d"      , __func__, HFAStatus );
            LOGE("%s Get SessionState  %d"      , __func__, SessionState );
            LOGE("%s Get RetryingTimes  %d"      , __func__, RetryingTimes );
        }
        else {
            LOGE("%s Get provision Status error  %lu", __func__, nRet); 
            goto error;
        }
    }
    /* Use GetActivationState() for Verizon */
    else if ((0 != strstr(fmrString,"SWI6600V")) ||
            ((0 != strstr(fmrString,"SWI6600H")) &&
            ((getFirmwareInfoCarrierQMI(&image)) && (image == eGOBI_IMG_CAR_VERIZON)))) {
        ULONG activationState;
        nRet = GetActivationState(&activationState);
        if (nRet == eQCWWAN_ERR_NONE) {
            switch (activationState) {
                /* OTASP Service Activation Code:
                   0 - Service not activated
                   1 - Service activated
                   2 - Activation connecting
                   3 - Activation connected
                   4 - OTASP security authenticated
                   5 - OTASP NAM downloaded
                   6 - OTASP MDN downloaded
                   7 - OTASP IMSI downloaded
                   8 - OTASP PRL downloaded
                   9 - OTASP SPC downloaded
                  10 - OTASP settings committed
                  */ 
                case 0x01:
                case 0x0A:
                    asprintf(&responseStr[0], "%s", "Provisioned");
                    break;
                default:
                    asprintf(&responseStr[0], "%s", "Not provisioned");
                    break;
            }
        }
        else {
            LOGE("%s Get provision Status error  %lu", __func__, nRet);
            goto error;
        }
    }
    else { 
        asprintf(&responseStr[0], "%s", "N/A");
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, responseStr, 3*sizeof(char *));

finally:
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}   
  

/**
 *  Device programming command to validate SPC code
 *
 *  @param  data [in]
 *          - Pointer to the string pointer array
 *  @param  count [in]
 *          - count of the number of strings pointed to by the data array
 *  @param  t [in]
 *          - A RIL token structure pointer
 *
 * @return
 *     none
 *
 * @par Abort:<br> 
 *     none
 *
 * @note
 *     none
 *
 */
void requestOEMHookStringsValidateSPC(const char **data, size_t count, RIL_Token t)
{
    const char **args = data;
    char *responseStrp[1];
    ULONG nRet;
    CHAR  *SPCp;
    
    /* Advance to the next argument */
    args++;
    count--;

    /* Argument number has to be 1 */
    if( count != 1 ) {
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        LOGE("%s - wrong number of args: %d", __func__, count+1);
        return;
    }
    
    SPCp = (char *)args[0];
    nRet = ValidateSPC(SPCp);
    if (nRet == eQCWWAN_ERR_NONE) {
        LOGD("%s SPC ok", __func__);
        responseStrp[0] = "1";
    }
    else {
        LOGE("%s ValidateSPC error  %lu", __func__, nRet);
        responseStrp[0] = "0";
    }
    RIL_onRequestComplete(t, RIL_E_SUCCESS, responseStrp, sizeof(char *));
}

/**
 *  Device programming command to set DDTM preference
 *
 *  @param  data [in]
 *          - Pointer to the string pointer array
 *  @param  count [in]
 *          - count of the number of strings pointed to by the data array
 *  @param  t [in]
 *          - A RIL token structure pointer
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 *
 */
void requestOEMHookStringsSetDDTMPref(const char **data, size_t count, RIL_Token t)
{
const char **args = data;
    ULONG nRet = eQCWWAN_ERR_NONE;
    BYTE i;
    struct DDTMPreference *pDDTMPref;
    struct DDTMSettings	    PrevDDTMSettings;

    /* Advance to the next argument */
    args++;
    count--;

    LOGD("%s:: Entered", __func__);

    /* Get previous DDTM preferences */
    getPrevDDTMPreference(&PrevDDTMSettings);
    pDDTMPref = &PrevDDTMSettings.DDTMPref;

    /* copy the user preference */
    pDDTMPref->DDTMPrefSetting = (BYTE)atoi(args[0]);

    LOGD("%s::ddtmPref: %x", __func__, pDDTMPref->DDTMPrefSetting );
    LOGD("%s::ddtmAction: %d", __func__, pDDTMPref->DDTMAction );
    LOGD("%s::SOListAction: %x", __func__, pDDTMPref->SOListAction );
    LOGD("%s::SOLen: %d", __func__, pDDTMPref->SOLen  );
    for (i=0; i < pDDTMPref->SOLen ; i++ )
    {
        LOGD("SO[%d]: %04X", i, pDDTMPref->pServiceOption[i] );
    }

    nRet = SLQSSetDDTMPref(pDDTMPref);
    if (nRet != eQCWWAN_ERR_NONE) {
        LOGE("%s::SLQSSetDDTMPref error  %lu", __func__, nRet);
        goto err;
    }
    LOGD("%s:: SLQSSetDDTMPref Success ", __func__);
    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    goto finally;

err:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);

finally:
    free(pDDTMPref->pServiceOption);
}

/**
 *  Retrieves various configuration parameters that control the modem behaviour
 *  related to circuit switched services
 *
 *  @param  data [in]
 *          - Pointer to the string pointer array
 *  @param  count [in]
 *          - count of the number of strings pointed to by the data array
 *  @param  t [in]
 *          - A RIL token structure pointer
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 */
void requestOEMHookStringsVoiceGetConfig(const char **data, size_t count, RIL_Token t)
{
    const char **args = data;
    ULONG nRet = eQCWWAN_ERR_NONE;
    voiceGetConfigReq voiceGetConfigReqParam;
    voiceGetConfigResp voiceGetConfigRespParam;
    BYTE CurrentPreferedVoiceSO = 0x01;
    prefVoiceSO respPrefVoiceSO;
    char *resp[5];
    int i;

    /* Advance to the next argument */
    args++;
    count--;
    LOGD("%s:: Entered\n", __func__);
    /* Argument number has to be 1 */
    if( count != 0 ) {
        LOGE("%s - wrong number of args: %d", __func__, count+1);
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }
    /* Initialize memory for the argurments */
    voiceGetConfigReqParam.pPrefVoiceSO = &CurrentPreferedVoiceSO;
    voiceGetConfigReqParam.pAMRStatus = NULL;
    voiceGetConfigReqParam.pAirTimer = NULL;
    voiceGetConfigReqParam.pAutoAnswer = NULL;
    voiceGetConfigReqParam.pNamID = NULL;
    voiceGetConfigReqParam.pPrefVoicePrivacy = NULL;
    voiceGetConfigReqParam.pRoamTimer = NULL;
    voiceGetConfigReqParam.pTTYMode = NULL;
    voiceGetConfigReqParam.pVoiceDomainPref = NULL;

    voiceGetConfigRespParam.pAirTimerCnt = NULL;
    voiceGetConfigRespParam.pAutoAnswerStat = NULL;
    voiceGetConfigRespParam.pCurAMRConfig = NULL;
    voiceGetConfigRespParam.pCurPrefVoiceSO = &respPrefVoiceSO;
    voiceGetConfigRespParam.pCurVoiceDomainPref = NULL;
    voiceGetConfigRespParam.pCurVoicePrivacyPref = NULL;
    voiceGetConfigRespParam.pCurrTTYMode = NULL;
    voiceGetConfigRespParam.pRoamTimerCnt = NULL;

    nRet = SLQSVoiceGetConfig(&voiceGetConfigReqParam, &voiceGetConfigRespParam);
    if ( nRet != eQCWWAN_ERR_NONE ) {
        LOGE("%s:: SLQSVoiceGetConfig failed, error: %lu\n", __func__, nRet);
        goto err;
    }
    LOGD("%s:: SLQSVoiceGetConfig success\n", __func__);
    asprintf( &resp[0], "%d", voiceGetConfigRespParam.pCurPrefVoiceSO->namID );
    asprintf( &resp[1], "%02X", voiceGetConfigRespParam.pCurPrefVoiceSO->evrcCapability);
    asprintf( &resp[2], "%04X", voiceGetConfigRespParam.pCurPrefVoiceSO->homePageVoiceSO);
    asprintf( &resp[3], "%04X", voiceGetConfigRespParam.pCurPrefVoiceSO->homeOrigVoiceSO);
    asprintf( &resp[4], "%04X", voiceGetConfigRespParam.pCurPrefVoiceSO->roamOrigVoiceSO);
    LOGD("%s:: %s", __func__, resp[0]);
    LOGD("%s:: %s", __func__, resp[1]);
    LOGD("%s:: %s", __func__, resp[2]);
    LOGD("%s:: %s", __func__, resp[3]);
    LOGD("%s:: %s", __func__, resp[4]);

    RIL_onRequestComplete(t, RIL_E_SUCCESS, resp, 5*sizeof(char *) );
    
    /* Free allocated memory */
    for (i = 0; i < 5; i++) {
        if (resp[i] != NULL)
            free(resp[i]);
    }  
    return;

err:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}


/**
 *  Retrieves the configuration used by the MSM GPS service.
 *
 *  @param  data [in]
 *          - Pointer to the string pointer array
 *  @param  count [in]
 *          - count of the number of strings pointed to by the data array
 *  @param  t [in]
 *          - A RIL token structure pointer
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 */
void requestOEMHookStringsGetAGPSConfig(const char **data, size_t count, RIL_Token t)
{
    ULONG nRet = eQCWWAN_ERR_NONE;
    ULONG serverAddress;
    ULONG serverPort;
    char *resp[2];
    int i;

    nRet = SLQSGetAGPSConfig( &serverAddress,
                              &serverPort,
                              NULL,
                              NULL,
                              NULL
                              );
    if( nRet != eQCWWAN_ERR_NONE )
    {
        LOGE("%s:: SLQSGetAGPSConfig failed, error: %lu\n", __func__, nRet);
        goto err;
    }
    LOGD("%s:: SLQSGetAGPSConfig Success\n", __func__);
    LOGD("%s:: Server Address: %lu", __func__, serverAddress);
    LOGD("%s:: Server Port: %lu", __func__, serverPort);

    asprintf( &resp[0], "%lu", serverAddress );
    asprintf( &resp[1], "%lu", serverPort );
    RIL_onRequestComplete(t, RIL_E_SUCCESS, resp, 2*sizeof(char *) );
    for (i = 0; i < 2; i++) {
        if (resp[i] != NULL)
            free(resp[i]);
    } 
    return;

err:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

/**
 *  Sets the configuration used by the MSM GPS service.
 *
 *  @param  data [in]
 *          - Pointer to the string pointer array
 *  @param  count [in]
 *          - count of the number of strings pointed to by the data array
 *  @param  t [in]
 *          - A RIL token structure pointer
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 */
void requestOEMHookStringsSetAGPSConfig(const char **data, size_t count, RIL_Token t)
{
    const char **args = data;
    ULONG nRet = eQCWWAN_ERR_NONE;
    ULONG serverAddress;
    ULONG serverPort;

    /* Advance to the next argument */
    args++;
    count--;

    /* Argument number has to be 2 */
    if( count != 2 ) {
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        LOGE("%s - wrong number of args: %d", __func__, count+1);
        goto err;
    }
    serverAddress = (ULONG)atoi(args[0]);
    serverPort    = (ULONG)atoi(args[1]);
    LOGD("%s:: Server Address: %lu", __func__, serverAddress);
    LOGD("%s:: Server Port: %lu", __func__, serverPort);

    nRet = SLQSSetAGPSConfig( &serverAddress,
                              &serverPort,
                              NULL,
                              NULL,
                              NULL );
    if( nRet != eQCWWAN_ERR_NONE )
    {
        LOGE("%s:: SLQSSetAGPSConfig failed, error: %lu\n", __func__, nRet);
        goto err;
    }

    LOGD("%s:: SLQSSetAGPSConfig Success\n", __func__);

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0 );
    return;

err:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}


void requestOEMHookStringsGet3GPP2SubscriptionInfo(const char **data, size_t count, RIL_Token t)
{
    ULONG nRet = eQCWWAN_ERR_NONE;
    char *resp[4];
    Mdn  mMdn;
    minBasedIMSI mMinBasedIMSI;
    nasGet3GPP2SubscriptionInfoReq mGet3GPP2SubsInfoReq;
    nasGet3GPP2SubscriptionInfoResp mGet3GPP2SubsInfoResp;

    memset( &mGet3GPP2SubsInfoResp, 0,  sizeof(nasGet3GPP2SubscriptionInfoResp));
    memset( &mMdn, 0,  sizeof(Mdn));
    memset( &mMinBasedIMSI, 0,  sizeof(minBasedIMSI));

    mGet3GPP2SubsInfoResp.pMdn = &mMdn;
    mGet3GPP2SubsInfoResp.pMinBasedIMSI = &mMinBasedIMSI;
    mGet3GPP2SubsInfoReq.namID = 255;           //current NAM
    nRet = SLQSNasGet3GPP2Subscription (&mGet3GPP2SubsInfoReq,&mGet3GPP2SubsInfoResp );

    if( nRet != eQCWWAN_ERR_NONE )
    {
        LOGE("%s:: SLQSNasGet3GPP2Subscription failed, error: %lu\n", __func__, nRet);
        goto err;
    }
    else
    {
        if((mGet3GPP2SubsInfoResp.pMdn->mdn) && (mGet3GPP2SubsInfoResp.pMdn->mdnLen < 16))
        {
            asprintf( &resp[0], "%s", mGet3GPP2SubsInfoResp.pMdn->mdn );
            asprintf( &resp[1], "%d", mGet3GPP2SubsInfoResp.pMdn->mdnLen );
            LOGD("%s:: SLQSNasGet3GPP2Subscription Success mdn len = %d \n", __func__, mGet3GPP2SubsInfoResp.pMdn->mdnLen);
            LOGD("%s:: SLQSNasGet3GPP2Subscription Success mdn= %s \n", __func__, resp[0]);
        }
        if((mGet3GPP2SubsInfoResp.pMinBasedIMSI->imsiMS1) && (mGet3GPP2SubsInfoResp.pMinBasedIMSI->imsiMS2))
        {
            asprintf( &resp[2], "%s", mGet3GPP2SubsInfoResp.pMinBasedIMSI->imsiMS1 );
            asprintf( &resp[3], "%s", mGet3GPP2SubsInfoResp.pMinBasedIMSI->imsiMS2 );
            LOGD("%s:: SLQSNasGet3GPP2Subscription Success imsiMS1= %s \n", __func__, resp[2]);
            LOGD("%s:: SLQSNasGet3GPP2Subscription Success imsiMS2= %s \n", __func__, resp[3]);
        }
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, resp, 4*sizeof(char *) );
    return;

err:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}


void requestOEMHookStringsSet3GPP2SubscriptionInfo(const char **data, size_t count, RIL_Token t)
{
    ULONG nRet = eQCWWAN_ERR_NONE;
    const char **args = data;
    char *resp[2];
    char *str;
    Mdn  mMdn;
    BYTE i;
    nasSet3GPP2SubscriptionInfoReq Set3GPP2SubsInfoReq;
    nasSet3GPP2SubscriptionInfoResp Set3GPP2SubsInfoResp;
    memset( &Set3GPP2SubsInfoResp, 0,  sizeof(nasSet3GPP2SubscriptionInfoResp));
    memset( &mMdn, 0,  sizeof(Mdn));

    /* Advance to the next argument */
    args++;
    count--;

    /* Argument number has to be 2 */
    if( count != 2 ) {
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        LOGE("%s - wrong number of args: %d", __func__, count+1);
        return;
    }

    str = (char *)args[0];
    mMdn.mdnLen = strtoul(str, NULL, 10);

    str = (char *)args[1];
    for( i=0 ; i<  mMdn.mdnLen; i++)
        mMdn.mdn[i] = str[i];

    LOGD("%s:: SLQSNasSet3GPP2Subscription len =  %d, mdn = %s\n", __func__,  mMdn.mdnLen,  mMdn.mdn );

    Set3GPP2SubsInfoReq.pMdn = &mMdn;
    Set3GPP2SubsInfoReq.namID = 255;       //current NAM

    nRet = SLQSNasSet3GPP2Subscription (&Set3GPP2SubsInfoReq,&Set3GPP2SubsInfoResp );

    if( nRet != eQCWWAN_ERR_NONE )
    {
        LOGE("%s:: SLQSNasSet3GPP2Subscription failed, error: %lu\n", __func__, nRet);
        goto err;
    }
    else
        LOGD("%s:: SLQSNasSet3GPP2Subscription Success \n", __func__);

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    return;

err:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

/**
 *  Reset the CDMA radio state.
 *
 *  @param  data [in]
 *          - Pointer to the string pointer array
 *  @param  count [in]
 *          - count of the number of strings pointed to by the data array
 *  @param  t [in]
 *          - A RIL token structure pointer
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 */

void requestOEMHookStringsResetCdmaRadioState(const char **data, size_t count, RIL_Token t)
{
    /* 
     * Recycle the CDMA radio state. It will trigger framework to fetch 
       ServiceState, DataState and otehr parameters again 
     */
    LOGD("%s:: reset CDMA radiostate\n", __func__);
    setRadioState(RADIO_STATE_NV_NOT_READY);
    
    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    return;
}


/**
 *
 * Get the CDMA carrier returned by UQCM image
 *
 * @param[in] data 
 *     pointer to the string pointer array
 * @param count 
 *     string pointer count
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
 *     none
 */

void requestOEMHookStringsGetCdmaCarrier(const char **data, size_t count, RIL_Token t)
{
    char *resp[2];
    enum eGobiImageCarrier image;
    RIL_RadioState state;
    int i;
    bool bStatus = false;

    bStatus = getFirmwareInfoCarrierQMI(&image);

    if (!bStatus) {
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        LOGE("%s: Could not fetch carrier info, return failure!", __func__);
        return;
    }

    LOGD("%s:: Carrier %d\n", __func__, image);    
    asprintf(&resp[0], "%s", *data);      
    asprintf(&resp[1], "%d", image);    
    
    RIL_onRequestComplete(t, RIL_E_SUCCESS, resp, 2 * sizeof(char *));
    
    for (i = 0; i < 2; i++) {
        if (resp[i] != NULL)
            free(resp[i]);
    } 
    return;
}

   





