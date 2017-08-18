/**
 *
 * @ingroup swiril
 *
 * @file 
 * Provides device related QMI based Sierra functions
 *
 * @author
 * Copyright: © 2011 Sierra Wireless, Inc.
 *            All rights reserved
 *
 */

#include <telephony/ril.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <alloca.h>
#include <getopt.h>
#include <sys/socket.h>
#include <cutils/sockets.h>
#include <termios.h>
#include <cutils/properties.h>

#include "swiril_main.h"
#include "swiril_cache.h"
#include "swiril_misc_qmi.h"
#include "swiril_device_qmi.h"
#include "swiril_network_qmi.h"
#include "swiril_misc.h"
#include "at_channel.h"
#include "swiril_pdp_qmi.h"

/* For QMI */
#include "aa/aaglobal.h"
#include "qmerrno.h"
#include "SWIWWANCMAPI.h"

/* Internal Constants */

#define LOG_TAG "RIL"
#include "swiril_log.h"
#include "swiril_oem.h"

typedef enum {
    QMI_OTASP_STATE_SERVICE_NOT_ACTIVATED = 0,
    QMI_OTASP_STATE_SERVICE_ACTIVATED,
    QMI_OTASP_STATE_ACTIVATION_CONNECTING,
    QMI_OTASP_STATE_ACTIVATION_CONNECTED,
    QMI_OTASP_STATE_SECURITY_AUTHENTICATED,
    QMI_OTASP_STATE_NAM_DOWNLOADED,
    QMI_OTASP_STATE_MDN_DOWNLOADED,
    QMI_OTASP_STATE_IMSI_DOWNLOADED,
    QMI_OTASP_STATE_PRL_DOWNLOADED,
    QMI_OTASP_STATE_SPC_DOWNLOADED,
    QMI_OTASP_STATE_SETTING_COMMITTED
}QMI_OTASP_STATE;

typedef enum {
    QMI_OTASP_START_STATUS_STARTED = 1,
    QMI_OTASP_START_STATUS_ABORTED
}QMI_OTASP_START_STATUS;

typedef enum {
    QMI_OMADM_STATE_COMPLETE_INFO_UPDATED = 0,
    QMI_OMADM_STATE_COMPLETE_UPDATE_INFO_UNAVAILABLE,
    QMI_OMADM_STATE_FAILED,
    QMI_OMADM_STATE_RETRYING,
    QMI_OMADM_STATE_CONNECTING,
    QMI_OMADM_STATE_CONNECTED,
    QMI_OMADM_STATE_AUTHENTICATED,
    QMI_OMADM_STATE_MDN_DOWNLOADED,
    QMI_OMADM_STATE_MSID_DOWNLOADED,
    QMI_OMADM_STATE_PRL_DOWNLOADED,
    QMI_OMADM_STATE_MIP_PROFILE_DOWNLOADED
}QMI_OMADM_STATE;

typedef enum {
    QMI_OMADM_SESSION_TYPE_CLIENT_INIT_DEVICE_CONFIG = 0,
    QMI_OMADM_SESSION_TYPE_CLIENT_INIT_PRL_UPDATE,
    QMI_OMADM_SESSION_TYPE_CLIENT_INIT_HANDSFREE_ACT
}QMI_OMADM_SESSION_TYPE;

typedef enum {
    QMI_OMADM_FAILURE_UNKNOWN = 0,
    QMI_OMADM_FAILURE_NETWORK_UNAVAILABLE,
    QMI_OMADM_FAILURE_SERVER_UNAVAILABLE,
    QMI_OMADM_FAILURE_AUTHENTICATED_FAILED,
    QMI_OMADM_FAILURE_MAX_RETRY_EXCEEDED,
    QMI_OMADM_FAILURE_SESSION_CANCELED
}QMI_OMADM_FAILURE_REASONS;

typedef struct {
    SWI_FW_INFO_TYPE   firmwareType;
    struct qmifwinfo_s firmwareInfo;
}firmwareInfo_s;

static firmwareInfo_s sFirmwareInfo;

/* Flag to keep track of whether a Sprint OMADM session is active or not.
 * NOTE: this applies to client-initiated sessions only.
 */
static bool s_OMASessionActive = false;

/* Flag to determine whether there is a pending OMADM session, as determined by
 * the OMADM session state returned from the modem.  This flag applies to both
 * client-initiated and network-initiated sessions.
 */
static bool s_pendingOMADMSession = false;

struct OMADMSessionPollParams {
    QMI_OMADM_STATE state;
};

void clearFirmwareInfoQMI(void)
{
    memset(&sFirmwareInfo, 0, sizeof(sFirmwareInfo));
}

/**
 *
 * To identify if the device is a Sierra device
 *
 * @param
 *     none
 *
 * @return
 *     true  : if the device is a Sierra device
 *     false : if the device is not a Sierra device
 *
 * @note
 *     none
 *
 */
bool IsSierraDevice(void)
{
    BYTE  stringsize = 50;
    CHAR  ModelId[stringsize];
    ULONG resultCode;
    CHAR  *pstr = NULL;

    resultCode = GetModelID( stringsize, ModelId );
    pstr = strstr (ModelId, "MC83");
    if (pstr == NULL) {
        return true;
    }
    else {
        return false;
    }
}

/*
 * Read an Android property
 *
 * @param propNamep [in]
 *     - The name of the property to read
 * @param propBufferp [out]
 *     - The string value of the property, if it is set
 *
 * @return
 *     true  : if the property is set
 *     false : if the property is not set
 *
 * @note
 *     SWI_TBD: This function could be moved into a common file, as there
 *              are several other cases where a property is read.
 */
bool readProperty(char *propNamep, char *propBufferp)
{
    /* The property is set if the return string has length > 0 */
    if (property_get(propNamep, propBufferp, NULL) > 0) {
        LOGI("%s=%s", propNamep, propBufferp); 
        return true;
    }

    LOGI("%s is not set", propNamep); 
    return false;
}

/**
 *
 * Init properties telephony.lteOnGsmDevice and telephony.lteOnCdmaDevice
 * if not already set.
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
void initLTEProperties(void)
{
    CHAR propValue[PROPERTY_VALUE_MAX];
    bool gsmSet;
    bool cdmaSet;
    char *lteOnGsm = "0";   /* default to false */
    char *lteOnCdma = "0";  /* default to false */
    
    /* If either property is set, then do nothing further */
    gsmSet = readProperty("telephony.lteOnGsmDevice", propValue);
    cdmaSet = readProperty("telephony.lteOnCdmaDevice", propValue);
    if ( gsmSet || cdmaSet )
         return;
         
    /* Overwrite the default if the appropriate modem is found */
    if (sFirmwareInfo.firmwareType == SWI_FW_INFO_TYPE_SWI_UMTS_ONLY) {
        lteOnGsm = "1";
    } else if (sFirmwareInfo.firmwareType == SWI_FW_INFO_TYPE_SWI_DUAL_MODE) {
        lteOnCdma = "1";
    }

    /* Init the properties */
    property_set( "telephony.lteOnGsmDevice", lteOnGsm );
    LOGI("Setting %s=%s", "telephony.lteOnGsmDevice", lteOnGsm);
    property_set( "telephony.lteOnCdmaDevice", lteOnCdma );
    LOGI("Setting %s=%s", "telephony.lteOnCdmaDevice", lteOnCdma); 
}

bool Is9x07_9x50_Device(void)
{
    #define EM75XX         "EM75"
    #define WP76XX         "WP76"

    if ((strstr(sFirmwareInfo.firmwareInfo.dev.s.modelid_str, EM75XX)) ||
        (strstr(sFirmwareInfo.firmwareInfo.dev.s.modelid_str, WP76XX)))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void initFirmwareInfoQMI(void)
{
    ULONG nRet;
    
    clearFirmwareInfoQMI();

    sFirmwareInfo.firmwareType = SWI_FW_INFO_TYPE_UNKNOWN;
    nRet = SLQSGetFirmwareInfo(&(sFirmwareInfo.firmwareInfo));

    if ( nRet == eQCWWAN_ERR_NONE ) {
        if( IsGobiDevice()  &&  !Is9x15Device() && !Is9x30Device() && !Is9x07_9x50_Device()) {
            LOGD("GOBI Device: Info");
            LOGD("Firmware id: %lx\n",sFirmwareInfo.firmwareInfo.dev.g.FirmwareID);
            LOGD("technology: %lx\n",sFirmwareInfo.firmwareInfo.dev.g.Technology);
            LOGD("Carrier : %lx\n",sFirmwareInfo.firmwareInfo.dev.g.Carrier);
            LOGD("region: %lx\n",sFirmwareInfo.firmwareInfo.dev.g.Region);
            LOGD("gPSCapability: %lx\n",sFirmwareInfo.firmwareInfo.dev.g.GPSCapability);

            switch ((enum eGobiImageTech)sFirmwareInfo.firmwareInfo.dev.g.Technology) {
                case eGOBI_IMG_TECH_CDMA:
                    sFirmwareInfo.firmwareType = SWI_FW_INFO_TYPE_GOBI_CDMA;
                    break;
                case eGOBI_IMG_TECH_UMTS:
                    sFirmwareInfo.firmwareType = SWI_FW_INFO_TYPE_GOBI_UMTS;
                    break;
                default:
                    sFirmwareInfo.firmwareType = SWI_FW_INFO_TYPE_UNKNOWN;
                    break;
            }
            LOGD("firmwareType: %lx\n",sFirmwareInfo.firmwareType);
            
        }
        else {
            LOGD("SWI Device: Info");
            LOGD("Device model identifier: %s\n",sFirmwareInfo.firmwareInfo.dev.s.modelid_str);
            LOGD("Firmware boot version: %s\n",sFirmwareInfo.firmwareInfo.dev.s.bootversion_str);
            LOGD("Firmware application version: %s\n",sFirmwareInfo.firmwareInfo.dev.s.appversion_str);
            LOGD("SKU(PRI) string: %s\n",sFirmwareInfo.firmwareInfo.dev.s.sku_str);
            LOGD("Package identifier: %s\n",sFirmwareInfo.firmwareInfo.dev.s.packageid_str);
            LOGD("Carrier string: %s\n",sFirmwareInfo.firmwareInfo.dev.s.carrier_str);
            LOGD("PRI version: %s\n",sFirmwareInfo.firmwareInfo.dev.s.priversion_str);
            /* SWI_TBD:
             * Check for "MC77xx" instead of "MC77x0", it should be good enough.
             */
            if (((strlen(sFirmwareInfo.firmwareInfo.dev.s.modelid_str) == strlen("MC77x0")) && 
                (strncmp(sFirmwareInfo.firmwareInfo.dev.s.modelid_str, "MC77x0", 3) == 0))
                ||((strlen(sFirmwareInfo.firmwareInfo.dev.s.modelid_str) == strlen("EM73x0")) && 
                (strncmp(sFirmwareInfo.firmwareInfo.dev.s.modelid_str, "EM73x0", 3) == 0))
                ||((strlen(sFirmwareInfo.firmwareInfo.dev.s.modelid_str) == strlen("WP7xxx")) &&
                (strncmp(sFirmwareInfo.firmwareInfo.dev.s.modelid_str, "WP7xxx", 3) == 0))
                ||((strlen(sFirmwareInfo.firmwareInfo.dev.s.modelid_str) == strlen("WP7xxx-x")) &&
                (strncmp(sFirmwareInfo.firmwareInfo.dev.s.modelid_str, "WP7xxx-x", 3) == 0)))
             {
                if (strcmp(sFirmwareInfo.firmwareInfo.dev.s.modelid_str, "MC7750") == 0)
                {
                    sFirmwareInfo.firmwareType = SWI_FW_INFO_TYPE_SWI_DUAL_MODE;
                }
                /* Same handling for MC7355 MC7354 MC7350 MC7304 EM/MC74XX */
                else if ((strncmp(sFirmwareInfo.firmwareInfo.dev.s.modelid_str, "MC73XX",4) == 0) ||
                         (strncmp(sFirmwareInfo.firmwareInfo.dev.s.modelid_str, "EM73XX",4) == 0) ||
                         (strncmp(sFirmwareInfo.firmwareInfo.dev.s.modelid_str, "MC74XX",4) == 0) ||
                         (strncmp(sFirmwareInfo.firmwareInfo.dev.s.modelid_str, "EM74XX",4) == 0) ||
                         (strncmp(sFirmwareInfo.firmwareInfo.dev.s.modelid_str, "WP75XX",4) == 0) ||
                         (strncmp(sFirmwareInfo.firmwareInfo.dev.s.modelid_str, "WP76XX",4) == 0))
                {
                    /* Verizon with carrier string 5 or carrier id 101*/
                    if( (strcmp(sFirmwareInfo.firmwareInfo.dev.s.carrier_str, "5") == 0) ||
                        (strcmp(sFirmwareInfo.firmwareInfo.dev.s.carrier_str, "101") == 0) )
                        sFirmwareInfo.firmwareType = SWI_FW_INFO_TYPE_SWI_DUAL_MODE;
                    /* Sprint with carrrier string 11 or carrier id 102 */
                    else if( (strcmp(sFirmwareInfo.firmwareInfo.dev.s.carrier_str, "11") == 0) || 
                        (strcmp(sFirmwareInfo.firmwareInfo.dev.s.carrier_str, "102") == 0) )
                        sFirmwareInfo.firmwareType = SWI_FW_INFO_TYPE_SWI_DUAL_MODE;
                    else
                        sFirmwareInfo.firmwareType = SWI_FW_INFO_TYPE_SWI_UMTS_ONLY;
                }
                else {
                    sFirmwareInfo.firmwareType = SWI_FW_INFO_TYPE_SWI_UMTS_ONLY;
                }
            }
            else if ((strlen(sFirmwareInfo.firmwareInfo.dev.s.modelid_str) == strlen("EM88x0")) && 
                     (strncmp(sFirmwareInfo.firmwareInfo.dev.s.modelid_str, "EM88x0", 4) == 0))
            {
            
                /* Verizon with carrier string 5 or carrier id 101*/
                if( (strcmp(sFirmwareInfo.firmwareInfo.dev.s.carrier_str, "5") == 0) ||
                    (strcmp(sFirmwareInfo.firmwareInfo.dev.s.carrier_str, "101") == 0) )
                        sFirmwareInfo.firmwareType = SWI_FW_INFO_TYPE_GOBI_CDMA;
                /* Sprint with carrrier string 11 or carrier id 102 */
                 else if( (strcmp(sFirmwareInfo.firmwareInfo.dev.s.carrier_str, "11") == 0) || 
                    (strcmp(sFirmwareInfo.firmwareInfo.dev.s.carrier_str, "102") == 0) )
                    sFirmwareInfo.firmwareType = SWI_FW_INFO_TYPE_GOBI_CDMA;
                else
                    sFirmwareInfo.firmwareType = SWI_FW_INFO_TYPE_GOBI_UMTS;
            }
        }

    }
    else if (Is8092RespinDevice())
    {
        sFirmwareInfo.firmwareType = SWI_FW_INFO_TYPE_SWI_UMTS_ONLY;
    }
    else {
            LOGD("%s: SLQSGetFirmwareInfo() failed %lx\n", __func__, nRet);
    }

    /* Set LTE related properties, if applicable */
    initLTEProperties();
}

bool getFirmwareInfoTechQMI(SWI_FW_INFO_TYPE *tech)
{
    bool iRet = true;
    
    if (sFirmwareInfo.firmwareType == SWI_FW_INFO_TYPE_UNKNOWN) {
        LOGE("%s: info invalid, initialize again", __func__);
        /* initialize again */
        initFirmwareInfoQMI();
    }

    if (sFirmwareInfo.firmwareType != SWI_FW_INFO_TYPE_UNKNOWN) {
        *tech = sFirmwareInfo.firmwareType;
    } else {
        iRet = false;
        LOGE("%s: Info invalid after initFirmwareInfoQMI()", __func__);
    }
    return iRet;
}

bool isDualModeModule(void)
{
    SWI_FW_INFO_TYPE technology;
    
    if (getFirmwareInfoTechQMI(&technology)) {
        if (technology == SWI_FW_INFO_TYPE_SWI_DUAL_MODE)
            return true;
    }
    return false;
}

bool isDualModeRunningCDMA(void)
{
    if (isDualModeModule() && 
        (getRealtimeTechnology() == SWI_RUNNING_TECH_CDMA)) {
        return true;
    }
    return false;
}

bool isDualModeRunningLTE(void)
{
    if (isDualModeModule() && 
        /* Check for both LTE and GSM, so that this function is the opposite 
           of the CDMA function above.  Normally GSM should never happen for
           Verizon, but could happen if the MC7750 is running in Global mode.
         */
        ( (getRealtimeTechnology() == SWI_RUNNING_TECH_LTE) ||
          (getRealtimeTechnology() == SWI_RUNNING_TECH_GSM) ) ) {
        return true;
    }
    return false;
}

bool isDualModeCDMA(void)
{
    if ((strcmp(sFirmwareInfo.firmwareInfo.dev.s.modelid_str, "MC7350") == 0) ||
        (strcmp(sFirmwareInfo.firmwareInfo.dev.s.modelid_str, "EM7350") == 0) ||
        (strcmp(sFirmwareInfo.firmwareInfo.dev.s.modelid_str, "MC7750") == 0))
        return true;
    else
        return false;
}

bool isFW9x15C(void)
{
    if (strncmp(sFirmwareInfo.firmwareInfo.dev.s.bootversion_str, "SWI9X15C", 7) == 0)
        return true;
    else
        return false;
}

bool getFirmwareInfoCarrierQMI(enum eGobiImageCarrier *image)
{
    int iRet = false;
    SWI_FW_INFO_TYPE type;
    
    if (getFirmwareInfoTechQMI(&type)) {
        if ((type == SWI_FW_INFO_TYPE_GOBI_UMTS) || (type == SWI_FW_INFO_TYPE_GOBI_CDMA)) {
            *image = (enum eGobiImageCarrier)sFirmwareInfo.firmwareInfo.dev.g.Carrier;
            iRet = true;
        }
        else if (type == SWI_FW_INFO_TYPE_SWI_DUAL_MODE) {
            if( (strcmp(sFirmwareInfo.firmwareInfo.dev.s.carrier_str, "11") == 0) || 
	            (strcmp(sFirmwareInfo.firmwareInfo.dev.s.carrier_str, "102") == 0) )
            {
                *image = eGOBI_IMG_CAR_SPRINT;
                iRet = true;
            }
            else if ((strcmp(sFirmwareInfo.firmwareInfo.dev.s.carrier_str, "5") == 0) ||
                     (strcmp(sFirmwareInfo.firmwareInfo.dev.s.carrier_str, "101") == 0) )
            {
                *image = eGOBI_IMG_CAR_VERIZON;
                iRet = true;
            }
        }
        
    }
    return iRet;
}

bool isFirmwareRunningCDMA() 
{
    SWI_FW_INFO_TYPE tech;
    bool bRet = false;
    if (getFirmwareInfoTechQMI(&tech) && (tech == SWI_FW_INFO_TYPE_GOBI_CDMA))
        bRet = true;

   return bRet;
}

/**
 *
 * Set power off by using QMI API
 *
 * @return
 *     true  - the modem switches to airplane mode
 *     false - otherwise
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 *
 */
bool setPowerOffQMI(void)
{
    if (SetPower(QMI_POWER_PERSISTENT_LOW_POWER) != eQCWWAN_ERR_NONE) {
        LOGE("%s can't set to QMI_POWER_PERSISTENT_LOW_POWER", __func__);
        return false;
    }
    /* Stop the DHCP client if already running */
    checkDHCPnStopService();

    setRadioState(RADIO_STATE_OFF);
    return true;
}

/**
 * RIL_REQUEST_RADIO_POWER
 *
 * Toggle radio on and off (for "airplane" mode).
*/
void requestRadioPowerCDMA(void *data, size_t datalen, RIL_Token t)
{
    int onOff;
    ULONG nRet;

    assert(datalen >= sizeof(int *));
    onOff = ((int *) data)[0];

    LOGD("%s %d", __func__, onOff);

    if ((onOff == 0) && (currentState() != RADIO_STATE_OFF)) {
        if (!setPowerOffQMI()) {
            goto error;
        }
    } else if ((onOff > 0) && (currentState() == RADIO_STATE_OFF)) {
        nRet = SetPower(QMI_POWER_ONLINE);
        if (nRet != eQCWWAN_ERR_NONE) {
            LOGE("SetPower can't set to QMI_POWER_ONLINE, error: %lu", nRet);
            goto error;
        }
        if (isDualModeModule()) {
            LOGD("%s dual mode module set to RADIO_STATE_SIM_NOT_READY", __func__);
            setRadioState(RADIO_STATE_SIM_NOT_READY);
        }
        else {
            LOGD("%s set to RADIO_STATE_NV_NOT_READY", __func__);
            setRadioState(RADIO_STATE_NV_NOT_READY);
        }
        /* clean up data call list when radio is turned off */
#ifdef SWI_RIL_VERSION_12
        initDataCallResponseList_r12();
#else
        initDataCallResponseList();
#endif
    } else {
        LOGE("Erroneous input to requestRadioPowerCDMA(): onOff=%i, sState=%i", onOff, currentState());
        goto error;
    }
    
    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

char initializeChannelCDMA(void)
{
    ULONG nRet;
    ULONG powerMode = 0;
    int count;
    
    LOGI("initializeChannelCDMA()");

    /* The modem has just been detected so must have
     * rebooted (or this is the first time after 
     * startup). No OMADM Session underway. CDMA only
     */

 
    /* The radio should initially be off.
     * Simulate a RIL RadioPower off command
     */
    setRadioState(RADIO_STATE_OFF);

    s_OMASessionActive = false;

    /* Set phone functionality.
     *    1 - Low power (airplane) mode
     */
    if (SetPower(QMI_POWER_PERSISTENT_LOW_POWER) != eQCWWAN_ERR_NONE) {
        LOGE("SetPower can't set to QMI_POWER_PERSISTENT_LOW_POWER");
        goto error;
    }

    /* It may take a few seconds for the modem to go into QMI_POWER_LOW mode,
     * so keep polling until the mode changes.  If the modem hasn't changed
     * after a reasonable time, assume that it never will and just continue.
     * A time of 30 seconds is somewhat arbitrary, but should be more than
     * long enough in most normal cases.
     *
     * SWI_TBD: may need to adjust the time, based on testing.  Also, this
     *          might be better accomplished with a call-back, but it is
     *          sufficient for now.
     */
    for (count=30; count>0; count--) {
        nRet = GetPower( &powerMode );
        LOGI("%s: powerMode=%li\n", __func__, powerMode);
        if ((nRet == eQCWWAN_ERR_NONE) && (powerMode == QMI_POWER_PERSISTENT_LOW_POWER)) {
            /* Got desired result, so return */
            return 0;
        }    
        sleep(1);
        
        /* Add retry to set phone functionality.*/
        if(count%10 == 0) {
            if (SetPower(QMI_POWER_PERSISTENT_LOW_POWER) != eQCWWAN_ERR_NONE) {
                LOGE("SetPower can't set to QMI_POWER_PERSISTENT_LOW_POWER");
                goto error;
            }
        }    
    }

    /* The previous polling time expired.
     * SWI_TBD: I'm not sure if this could really happen.
     */

    LOGE("%s: unexpected powerMode=%li\n", __func__, powerMode);

    /* If the modem is on, then just proceed with the normal power on process */
    if ((nRet == eQCWWAN_ERR_NONE) && (powerMode == QMI_POWER_ONLINE)) {
        if (isDualModeModule()) {
            LOGD("%s dual mode module set to RADIO_STATE_SIM_NOT_READY", __func__);
            setRadioState(RADIO_STATE_SIM_NOT_READY);
        }
        else {
            LOGD("%s set to RADIO_STATE_NV_NOT_READY", __func__);
            setRadioState(RADIO_STATE_NV_NOT_READY);
        }
    }

error:
    return 1;
}

void requestRadioPowerUMTS(void *data, size_t datalen, RIL_Token t)
{
    int onOff;
    ULONG nRet;

    assert(datalen >= sizeof(int *));
    onOff = ((int *) data)[0];

    LOGD("requestRadioPowerUMTS %d", onOff);
    
    if ((onOff == 0) && (currentState() != RADIO_STATE_OFF)) {
        if (!setPowerOffQMI()) {
            goto error;
        }
    } else if ((onOff > 0) && (currentState() == RADIO_STATE_OFF)) {
        nRet = SetPower(QMI_POWER_ONLINE);
        if (nRet != eQCWWAN_ERR_NONE) {
            LOGE("SetPower can't set to QMI_POWER_ONLINE, error: %lu", nRet);
            goto error;
        }
        setRadioState(RADIO_STATE_SIM_NOT_READY);
        /* clean up data call list when radio is turned off */
#ifdef SWI_RIL_VERSION_12
        initDataCallResponseList_r12();
#else
        initDataCallResponseList();
#endif

    } else {
        LOGE("Erroneous input to requestRadioPowerUMTS(): onOff=%i, sState=%i", onOff, currentState());
        goto error;
    }
    
    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

char initializeChannelUMTS(void)
{
    ULONG nRet;
    ULONG powerMode = 0;
    int count;
    
    LOGI("initializeChannelUMTS()");
    
    /* The radio should initially be off.
     * Simulate a RIL RadioPower off command
     */
    setRadioState(RADIO_STATE_OFF);

    /* Set phone functionality.
     *    1 - Low power (airplane) mode
     */
    if (SetPower(QMI_POWER_PERSISTENT_LOW_POWER) != eQCWWAN_ERR_NONE) {
        LOGE("SetPower can't set to QMI_POWER_PERSISTENT_LOW_POWER");
        goto error;
    }

    /* It may take a few seconds for the modem to go into QMI_POWER_LOW mode,
     * so keep polling until the mode changes.  If the modem hasn't changed
     * after a reasonable time, assume that it never will and just continue.
     * A time of 30 seconds is somewhat arbitrary, but should be more than
     * long enough in most normal cases.
     *
     * SWI_TBD: may need to adjust the time, based on testing.  Also, this
     *          might be better accomplished with a call-back, but it is
     *          sufficient for now.
     */
    for (count=30; count>0; count--) {
        nRet = GetPower( &powerMode );
        LOGI("%s: powerMode=%li\n", __func__, powerMode);
        if ((nRet == eQCWWAN_ERR_NONE) && (powerMode == QMI_POWER_PERSISTENT_LOW_POWER)) {
            /* Got desired result, so return */
            return 0;
        }    
        sleep(1);
    }

    /* The previous polling time expired.
     * SWI_TBD: I'm not sure if this could really happen.
     */

    LOGE("%s: unexpected powerMode=%li\n", __func__, powerMode);

    /* If the modem is on, then just proceed with the normal power on process */
    if ((nRet == eQCWWAN_ERR_NONE) && (powerMode == QMI_POWER_ONLINE)) {
        setRadioState(RADIO_STATE_SIM_NOT_READY);
    }

error:
    return 1;
}

/**
 * RIL_REQUEST_BASEBAND_VERSION
 *
 * Return string value indicating baseband version
 * 
*/
void requestBasebandVersionQMI(void *data, size_t datalen, RIL_Token t)
{
    ULONG nRet;
    CHAR fmrString[NUM_MAX_STR_LEN];

    nRet = GetFirmwareRevision(sizeof(fmrString),
                               &fmrString[0]);
    if (nRet == eQCWWAN_ERR_NONE) {
        LOGD("Firmware Revision : %s\n", fmrString);
        /* Return the successful response and cache the value for next time */
        swiril_cache_RIL_onRequestComplete(t, RIL_E_SUCCESS, fmrString, sizeof(char *),
                                           RIL_REQUEST_BASEBAND_VERSION);
    }
    else {
        LOGE("requestBasebandVersion error: %lu", nRet);
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    }
}

/**
 * Set to the corresponding power down mode 
 *
 * @param [in] state
 *     The power state to which the modem has to switch to
 *
 * @return
 *     None
 *
 * @note
 *     None 
 *
 */
void changePowerModeQMI(QMI_POWER_STATE state)
{
    ULONG nRet;
    
    nRet = SetPower(state);
    if (nRet != eQCWWAN_ERR_NONE) {
        LOGE("changePowerModeQMI: SetPower can't set to %d with error %lu",
             state, nRet);
    } else if ((state == QMI_POWER_OFF) ||
               (state == QMI_POWER_OFFLINE)||
               (state == QMI_POWER_RESET)) {
        /* If the state is power off, set the radio state accordingly */
        setRadioState(RADIO_STATE_UNAVAILABLE);
    }
}

/**
 *
 * Is there a pending/active OMADM session, so that an attempt to start a new
 * data session should not be allowed.
 *
 * @param
 *     none
 *
 * @return
 *     true  : pending OMADM session; do not start new data session
 *     false : no pending OMADM session; okay to try starting new data session
 *
 * @note
 *     none
 *
 */
bool IsOMADMSessionPending(void)
{
    return s_pendingOMADMSession;
}

/**
 * 
 * Enqueued function to handle potential pending OMADM session.
 *
 * @param params/sessionstate [In]
 *     - The session status being reported by the modem through its callback
 * 
 * @return
 *     - None
 *
 * @note
 *     - Runs in the context of the Sierra RIL 
 *
 */
static void handlePendingOMADMSession(void *params)
{
    ULONG sessionState=0xff;   /* default to invalid value */

    /* Timer to clear the pending flag, if it hasn't already been cleared */
    const struct timeval TIMEVAL_CLEARPENDING = { 20, 0 };

    /* Did the timer expire, or did we get a new session state */
    if (params == NULL ) {
        if ( s_pendingOMADMSession ) {
            s_pendingOMADMSession = false;
            LOGI("%s: timer expired, set pending false", __func__);
        }
    } else {
        /* Get the parameter value, and ensure the memory is freed */
        sessionState = *((ULONG *)params);
        free(params);

        /* Is there a new OMADM session starting.  If there was an existing
         * data session that had to be dropped, then the new session starts
         * with RETRYING state; otherwise it starts with CONNECTING state.
         */
        if ( (sessionState >= QMI_OMADM_STATE_RETRYING) &&
             (sessionState <= QMI_OMADM_STATE_CONNECTING) ) {
            if ( !s_pendingOMADMSession ) {
                s_pendingOMADMSession = true;
                LOGI("%s: state is %lu, set pending true", __func__, sessionState);

                /* As a backup, clear pending flag after a bit, if still pending */
                enqueueRILEvent(RIL_EVENT_QUEUE_NORMAL,
                                handlePendingOMADMSession,
                                NULL,
                                &TIMEVAL_CLEARPENDING);
            }
        } else if (sessionState == QMI_OMADM_STATE_CONNECTED) {
            /* Once CONNECTED state is reached, then the OMADM session is
             * active, and thus no longer pending.
             */
            if ( s_pendingOMADMSession ) {
                s_pendingOMADMSession = false;
                LOGI("%s: state is %lu, set pending false", __func__, sessionState);
            }
        } else 
            LOGE("%s: unexpected session state %lu", __func__, sessionState);

    }

    LOGI("%s: state is %lu, pending is %d", __func__, sessionState, s_pendingOMADMSession);
}

/**
 * 
 * Sub-function to handle the actual processing required in response to an
 * OMADM Session state notification event. This function is enqueued by the
 * actual callback handler, setOMADMAStateCallback(). 
 *
 * @param sessionstate [In]
 *     - The session status being reported by the modem through its callback
 * @param failurereason
 *     - Indicates the reason why the session failed if "sessionstate"
 *       indicates a failure
 *
 * @return
 *     - None
 *
 * @note
 *     - Runs in the context of the Sierra RIL 
 *
 */
static void pollOMADMSessionInfoCDMA(void *params)
{
    int response = CDMA_OTA_PROVISION_STATUS_OTAPA_ABORTED;
    ULONG nRet;
    ULONG sessionStateCheck;
    ULONG sessionType;
    ULONG failureReasonCheck;
    BYTE  retryCount;
    WORD  sessionPause;
    WORD  timeRemaining;
    struct OMADMSessionPollParams *poll_params;
    QMI_OMADM_STATE sessionState;

    assert(params != NULL);
    poll_params = (struct OMADMSessionPollParams *) params;
    sessionState = poll_params->state;

    /* Proceed with updating the upper layers */
    nRet = OMADMGetSessionInfo(&sessionStateCheck,
                               &sessionType,
                               &failureReasonCheck,
                               &retryCount,
                               &sessionPause,
                               &timeRemaining);
    LOGD("pollOMADMSessionInfoCDMA: sessionStateCheck %lu\n",sessionStateCheck);
    if (nRet == eQCWWAN_ERR_NONE) {
        if ((sessionState == QMI_OMADM_STATE_COMPLETE_INFO_UPDATED) 
         && (sessionStateCheck == QMI_OMADM_STATE_COMPLETE_INFO_UPDATED)) {
            response = CDMA_OTA_PROVISION_STATUS_COMMITTED;
        }
        else if ((sessionState == QMI_OMADM_STATE_RETRYING) 
              && (sessionStateCheck == QMI_OMADM_STATE_RETRYING)) {
            /* Seems like a FW issue, no any notification after 
             * QMI_OMADM_STATE_RETRYING Following Windows watcher, send session 
             * failed call OMADMCancelSession() making possible for retry, 
             * seems like MC8355 always return error 1065 
             */
            nRet = OMADMCancelSession();
            if (nRet != eQCWWAN_ERR_NONE) {
                LOGE("pollOMADMSessionInfo: OMADMCancelSession failed %lu\n", nRet);
            }
        }
    }
    /* Inform final result */
    RIL_onUnsolicitedResponse(RIL_UNSOL_CDMA_OTA_PROVISION_STATUS,
                              &response, sizeof(int *));
    free(params);
}

/**
 * 
 * Callback function for OMADM State Changes. This function is installed
 * at startup to register the callback. OMADM sessions can be client-
 * initiated or network-initiated. Once a session is underway, this callback
 * communications the session status. 
 *
 * @param sessionstate [In]
 *     - The session status being reported by the modem through its callback
 * @param failurereason
 *     - Indicates the reason why the session failed if "sessionstate"
 *       indicates a failure
 *
 * @return
 *     - None
 *
 * @note
 *     - Runs in the context of the SLQS package's notification handler
 *
 */
void setOMADMAStateCallback(ULONG sessionState, ULONG failureReason)
{
    int response = CDMA_OTA_PROVISION_STATUS_COMMITTED;
    struct OMADMSessionPollParams *poll_params = NULL;
    unsigned char sessionstate_response[3];
    
    LOGD("CDMA OMADMAStateCallback Status: %lu %lu\n", sessionState, failureReason);

    /* Send UNSOL notification with raw session state.  It is up to the 
     * telephony framework to interpret these values properly.
     */
    sessionstate_response[0] = SWI_IND_OMADM_STATE;
    sessionstate_response[1] = 1;
    sessionstate_response[2] = sessionState;
    RIL_onUnsolicitedResponse(RIL_UNSOL_OEM_HOOK_RAW,
        sessionstate_response, sizeof(sessionstate_response));

    /* Check if there is a potential pending OMADM session.  If so, then
     * enqueue function to handle it in the queueRunner thread.
     */
    if ( (sessionState >= QMI_OMADM_STATE_RETRYING) &&
         (sessionState <= QMI_OMADM_STATE_CONNECTED) ) {

        ULONG *saved_sessionStatep;
        saved_sessionStatep = malloc(sizeof(*saved_sessionStatep));
        *saved_sessionStatep = sessionState;

        enqueueRILEvent(RIL_EVENT_QUEUE_NORMAL,
                        handlePendingOMADMSession,
                        saved_sessionStatep,
                        NULL);
    }

    /* Only send a report if there's an active session underway */
    if( s_OMASessionActive ) {

        /* It may be time to clear the session active flag. Check
         * for one of the telltale session state values which
         * indicate the session is done
         */
        switch( sessionState ) {

            case QMI_OMADM_STATE_COMPLETE_INFO_UPDATED:
            case QMI_OMADM_STATE_COMPLETE_UPDATE_INFO_UNAVAILABLE:
            case QMI_OMADM_STATE_FAILED:
            case QMI_OMADM_STATE_RETRYING:
            case QMI_OMADM_STATE_PRL_DOWNLOADED:
                s_OMASessionActive = false;
                break;

            default: 
                /* Session is still active, but the session 
                 * state value is not of interest yet. Don't
                 * change the active flag
                 */
                break;
        }

        if ((sessionState == QMI_OMADM_STATE_COMPLETE_INFO_UPDATED) || 
            (sessionState == QMI_OMADM_STATE_RETRYING)) {
            /* Confirm OMA-DM state again
             * sounds like no way to call QMI API in callback 
             */
            poll_params = malloc(sizeof(struct OMADMSessionPollParams));
            poll_params->state = sessionState;
            enqueueRILEvent(RIL_EVENT_QUEUE_NORMAL, pollOMADMSessionInfoCDMA,
                            poll_params, NULL);
        }
        else if (sessionState == QMI_OMADM_STATE_FAILED) {
            /* Inform activation failed */
            response = CDMA_OTA_PROVISION_STATUS_OTAPA_ABORTED;
            RIL_onUnsolicitedResponse(RIL_UNSOL_CDMA_OTA_PROVISION_STATUS,
                              &response, sizeof(int *));
        }
    }
}

void omaDmCallback(ULONG sessionType, BYTE* psessionTypeFields);
void SLQSOMADMAlertCallback(ULONG sessionType, BYTE* psessionTypeFields);
void registerSetOMADMStateCallbackCDMA(void)
{
    ULONG nRet;
    CHAR modelType[100];

    memset(modelType,0,sizeof(modelType));  
    if (property_get("persist.radio.module", modelType, NULL) > 0) {
        LOGI("Read device type from property persist.radio.module \n");
    }   
    else {           
         GetModelID(sizeof(modelType),(CHAR*)&modelType);
         property_set("persist.radio.module",modelType);
    }
 
    if (strstr(modelType,"9090") != NULL)
    {
        LOGD("Model type is SL9090 or SL9090RD");
        nRet = SetSLQSOMADMAlertCallbackExt( &omaDmCallback );
    }
    else if ((strstr(modelType,"MC73") != NULL) ||
             (strstr(modelType,"EM73") != NULL) ||
             (strstr(modelType,"MC74") != NULL) ||
             (strstr(modelType,"EM74") != NULL))
    {
       LOGD("Model type is MC73xx/MC74xx or EM73xx/EM74xx");
       nRet = SetSLQSOMADMAlertCallback( &SLQSOMADMAlertCallback );
    }
    else
    {
        nRet = SetOMADMStateCallback( &setOMADMAStateCallback );
    }
    if ( nRet != eQCWWAN_ERR_NONE ) {
	        LOGE("registerSetOMADMStateCallback: failed to enable callback %lu\n", nRet );
	}
}

static void otaSpCallback(
    ULONG activationStatus )
{
    unsigned char  activationstart_response[2];
    struct timeval timeval_reset_delay = { 6, 0 };
        
    LOGD("CDMA Activation Status: %lu\n", activationStatus );
    
    activationstart_response[0] = SWI_IND_OTASP_ACTIVATION_STATUS;
    activationstart_response[1] = (unsigned char) activationStatus;
		   
    if (activationStatus == QMI_OTASP_STATE_SERVICE_ACTIVATED) {
        /* Inform successfully activated */
        RIL_onUnsolicitedResponse(RIL_UNSOL_OEM_HOOK_RAW,
                          &activationstart_response, sizeof(activationstart_response));
        /* power cycle the module, it seems working ok with SLQS00.06.00 */
        /* modem will peform RF reset after OTASP, need to delay power cycle, otherwise it will fail VZW test case */
        enqueueRILEvent(RIL_EVENT_QUEUE_PRIO, setPowerOff, NULL,&timeval_reset_delay);
    }
    else if (activationStatus == QMI_OTASP_STATE_SERVICE_NOT_ACTIVATED) {
        /* Inform activation failed */
        RIL_onUnsolicitedResponse(RIL_UNSOL_OEM_HOOK_RAW,
                          &activationstart_response, sizeof(activationstart_response));
    }
}

static void setActivationStatusCB(
    ULONG activationStatus )
{
    int response = CDMA_OTA_PROVISION_STATUS_COMMITTED;
    ULONG nRet;
    
    LOGD("CDMA Activation Status: %lu\n", activationStatus );
   
    if (activationStatus == QMI_OTASP_STATE_SERVICE_ACTIVATED) {
        /* Inform successfully activated */
        RIL_onUnsolicitedResponse(RIL_UNSOL_CDMA_OTA_PROVISION_STATUS,
                          &response, sizeof(int *));
        /* power cycle the module, it seems working ok with SLQS00.06.00 */
        setPowerOff();
    }
    else if (activationStatus == QMI_OTASP_STATE_SERVICE_NOT_ACTIVATED) {
        /* Inform activation failed */
        response = CDMA_OTA_PROVISION_STATUS_OTAPA_ABORTED;
        RIL_onUnsolicitedResponse(RIL_UNSOL_CDMA_OTA_PROVISION_STATUS,
                          &response, sizeof(int *));
    }
}

void registerActivationStatusCallbackCDMA(void)
{
    ULONG nRet;
    CHAR modelType[100];

    memset(modelType,0,sizeof(modelType));  
    if (property_get("persist.radio.module", modelType, NULL) > 0) {
        LOGI("Read device type from property persist.radio.module \n");
    }   
    else {         
         GetModelID(sizeof(modelType),(CHAR*)&modelType);
         property_set("persist.radio.module",modelType);
    }
        
    if (strstr(modelType,"9090") != NULL) {
        LOGD("Model type is SL9090 or SL9090RD");
	nRet = SetActivationStatusCallback(&otaSpCallback);	
    }
    else {
        nRet = SetActivationStatusCallback(&setActivationStatusCB);
    }    

    if ( nRet != eQCWWAN_ERR_NONE ) {
        LOGE("registerActivationStatusCallbackCDMA: failed to enable callback %lu\n", nRet );
    }
}

void activateServiceCDMA(void)
{
    ULONG nRet;
    ULONG activationState;
    int response = CDMA_OTA_PROVISION_STATUS_OTAPA_ABORTED;
    
    if (sFirmwareInfo.firmwareInfo.dev.g.Carrier == eGOBI_IMG_CAR_VERIZON) {
        /*
           ActivateAutomatic() function requests the device to perform 
           automatic service activation. Currently, automatic activation is 
           not complete until the device has been reset. For this reason, the 
           recommended approach when using ActivateAutomatic is for the application 
           to perform the following steps:
            1. Enable the activation status callback via SetActivationStatusCallback().
            2. Call ActivateAutomatic().
            3. Monitor activation status (via callback) for a value of 1 
               (service activated).
            4. When this value is received, call SetPower( 5 ).
            5. Call QCWWANDisconnect().
            6. Reconnect after the device power cycles.
         */
        /* hardcode activation code now, Windows RIL does the same */
        nRet = ActivateAutomatic("*22899");
        
        if (nRet == eQCWWAN_ERR_NONE) {
            LOGD("activateServiceCDMA OTASP starting");
            response = CDMA_OTA_PROVISION_STATUS_OTAPA_STARTED;
            RIL_onUnsolicitedResponse(RIL_UNSOL_CDMA_OTA_PROVISION_STATUS,
                              &response, sizeof(int *));
        }
        else {
            LOGE("activateServiceCDMA OTASP error: %lu", nRet);
            RIL_onUnsolicitedResponse(RIL_UNSOL_CDMA_OTA_PROVISION_STATUS,
                              &response, sizeof(int *));
        }    
    }
    else if (sFirmwareInfo.firmwareInfo.dev.g.Carrier == eGOBI_IMG_CAR_SPRINT) {
        nRet = OMADMStartSession(QMI_OMADM_SESSION_TYPE_CLIENT_INIT_DEVICE_CONFIG);
        if (nRet == eQCWWAN_ERR_NONE) {
            LOGD("activateServiceCDMA OMADM starting");

            /* Make note of the now-active session */
            s_OMASessionActive = true;

            response = CDMA_OTA_PROVISION_STATUS_OTAPA_STARTED;
            RIL_onUnsolicitedResponse(RIL_UNSOL_CDMA_OTA_PROVISION_STATUS,
                              &response, sizeof(int *));
        }
        else {
            LOGE("activateServiceCDMA OMADM error: %lu", nRet);
            RIL_onUnsolicitedResponse(RIL_UNSOL_CDMA_OTA_PROVISION_STATUS,
                              &response, sizeof(int *));
        } 
    }
    else {
        LOGE("activateServiceCDMA image carrier %lu not supported",
             sFirmwareInfo.firmwareInfo.dev.g.Carrier);
        RIL_onUnsolicitedResponse(RIL_UNSOL_CDMA_OTA_PROVISION_STATUS,
                          &response, sizeof(int *));    
    }
}

/**
 *  Initiate a PRL Update request
 *
 *  @param  data [in]
 *          - Pointer to the string pointer array, unused
 *  @param  count [in]
 *          - count of the number of strings pointed to by the data array,
 *            unused
 *  @param  t [in]
 *          - A RIL token structure pointer
 *
 *  @return None
 *
 *  @note   Support: CDMA
 *                   Sprint only
 *
 */
void requestOEMHookStringsPRLUpdate(void *data, size_t count, RIL_Token t)
{
    ULONG nRet;
    
    LOGD("Entered %s", __func__ );

    /* Supported only for Sprint devices at this time */
    if (sFirmwareInfo.firmwareInfo.dev.g.Carrier == eGOBI_IMG_CAR_SPRINT) {

        LOGD("%s, carrier is Sprint", __func__ );

        /* Request a client-initiated PRL Update */
        nRet = OMADMStartSession(QMI_OMADM_SESSION_TYPE_CLIENT_INIT_PRL_UPDATE);

        if (nRet == eQCWWAN_ERR_NONE) {
            LOGD("%s PRL Update started", __func__ );

            /* Make a note of the now-active session */
            s_OMASessionActive = true;

            /* Send a successful completion response */
            RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
        }
        else {
            LOGE("%s PRL Update request error: %lu", __func__, nRet);
            /* Error completion response */
            RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        } 
    }
    else {
        LOGD("%s, carrier is %d. PRL update cancelled", 
             __func__, (int) sFirmwareInfo.firmwareInfo.dev.g.Carrier );
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    }
}

/**
 *  Cancel the current OMADM session
 *
 *  @param  data [in]
 *          - Pointer to the string pointer array, unused
 *  @param  count [in]
 *          - count of the number of strings pointed to by the data array,
 *            unused
 *  @param  t [in]
 *          - A RIL token structure pointer
 *
 *  @return
 *      none
 *
 *  @note
 *      none
 */
void requestOEMHookStringsCancelOmaDm(void *data, size_t count, RIL_Token t)
{
    ULONG nRet;
    
    LOGD("Entered %s", __func__ );

    /* Supported only for Sprint devices at this time */
    if (sFirmwareInfo.firmwareInfo.dev.g.Carrier == eGOBI_IMG_CAR_SPRINT) {
        /* Request a OMA cancel session*/
        nRet = OMADMCancelSession();

        if (nRet == eQCWWAN_ERR_NONE) {
            /* Send a successful completion response */
            RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
            return;
        }
        LOGD("%s :: %ld", __func__, nRet);
    }
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

/**
 *  Initiate a Factory Reset on the attached modem
 *
 *  @param  data [in]
 *          - Pointer to the string pointer array
 *  @param  count [in]
 *          - count of the number of strings pointed to by the data array
 *  @param  t [in]
 *          - A RIL token structure pointer
 *
 *  @return None
 *
 *  @note   Support: CDMA
 *                   UMTS
 * 
 *          Assumes the string pointed to by spc argument is valid, does 
 *          no internal validation of its own
 * 
 */
void requestOEMHookStringsFactoryReset(void *data, size_t count, RIL_Token t)
{
    ULONG nRet;
    const char **args = data;
    char *spcp;
    char *prop = NULL;

    /* Advance to the next argument */
    args++;
    count--;

    /* just one argument is the only acceptable choice */
    if( count != 1 ) {
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        LOGE("Factory reset - wrong number of args: %d", count+1);
        return;
    }

    /* The next argument in the list is the SPC string pointer */
    spcp = (char *) *args;
  
    LOGD("Factory reset requested with spc: %s", spcp );

    /* Launch the request to reset to factory defaults */
    nRet = ResetToFactoryDefaults( spcp );

    /* Reply to the upper layer depending on the outcome */
    if (nRet == eQCWWAN_ERR_NONE) {
        LOGD("%s Factory reset started", __func__ );

        //Will cause HFA fail after ##SCRTN#
        /* To change the value of force flag to 1 to update all the required audio parameters */
        //asprintf(&prop, "%s", "persist.radio.forceflag");
        //property_set(prop, "1");
        //free(prop);
        /* tell the upper layer the result is success */

        RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

        /* The modem must be power-cycled after a factory reset */
        /* SWI_TBD
           What should we do if starting the power cycle fails, since sending a
           failure response to the RIL command is not really correct.
         */
        setPowerOff();
    }
    else {
        LOGE("%s Factory reset request error: %lu", __func__, nRet);
        /* Error completion response */
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    } 
}

/**
 *  AT version of RF SAR status query request
 *
 *  @param[out]  nRfSarState 
 *     - Pointer to the SAR state retrieved from module
 *
 * @return
 *     true  - successfully retrieved SAR state
 *     false - otherwise
 *
 * @par Abort:<br> 
 *     none
 *
 * @note
 *     none
 */
static bool ATGetRfSarState(ULONG *nRfSarState)
{
    int err = -1;
    ATResponse *atresponse = NULL;
    bool nRet = true;

    err = at_send_command_numeric("AT!SARSTATE?", &atresponse);
    if (err < 0 || atresponse->success == 0) {
        nRet = false;
    } else {
        *nRfSarState = (ULONG) atoi(atresponse->p_intermediates->line);
    }
    at_response_free(atresponse);
    return nRet;
}


/**
 *  AT version of RF SAR status set request
 *
 *  @param nRfSarState 
 *     - SAR state to be set
 *
 * @return
 *     true  - successfully set SAR state
 *     false - otherwise
 *
 * @par Abort:<br> 
 *     none
 *
 * @note
 *     This function returns 'true' for "!SARSTATE: NOT IMPLEMENTED \n OK" response
 */
static bool ATSetRfSarState(ULONG nRfSarState)
{
    int err = -1;
    ATResponse *atresponse = NULL;
    char *cmd = NULL;
    bool nRet = true;

    asprintf(&cmd, "AT!SARSTATE=%lu", nRfSarState);
    err = at_send_command(cmd, &atresponse);
    free(cmd);

    if (err < 0 || atresponse->success == 0) {
        nRet = false;
    }
    at_response_free(atresponse);
    return nRet;
}

/**
 *  Initiate a RF SAR status query request
 *
 *  @param  data [in]
 *          - Pointer to the string pointer array
 *  @param  count [in]
 *          - count of the number of strings pointed to by the data array
 *  @param  t [in]
 *          - A RIL token structure pointer
 *
 *  @return None
 *
 *  @note   Support: CDMA
 *                   UMTS
 * 
 */
void requestOEMHookStringsGetRfSarState(const char **data, size_t count, RIL_Token t)
{
    ULONG nRet = 0 ;
    ULONG nRfSarState = 0 ;
    char  *response[2];
    char  *strp = NULL;
    char  *subcmd = NULL;
    bool result = true;
    
    asprintf(&subcmd, "%s", *data);
    response[0] = subcmd;
    
    LOGD("Entered %s", __func__ );

    /* Request a client-initiated SAR query function */
    nRet = SLQSGetRfSarState(&nRfSarState);
    if (nRet != eQCWWAN_ERR_NONE) {
        LOGD("%s SLQS GetRfSarState failed: %lu", __func__, nRet);
        /* Run AT command for MC77xx modules */
        if(!ATGetRfSarState(&nRfSarState)) {
            LOGE("%s AT GetRfSarState failed!", __func__);
            result = false;
        }
    }

    if (result) {
        LOGD("%s  GetRfSarState succeed, nRfSarState: %lu. ", __func__, nRfSarState );
        /* Send a successful completion response */
        asprintf(&strp, "%lu", nRfSarState);
        response[1] = strp;
        RIL_onRequestComplete(t, RIL_E_SUCCESS, response,  2 * sizeof(char *));
    }
    else {
        /* Error completion response */
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    } 

    if (subcmd != NULL)
        free(subcmd);

    if (strp != NULL)
        free(strp);
}

/**
 *  Initiate a RF SAR status setting request
 *
 *  @param  data [in]
 *          - Pointer to the string pointer array
 *  @param  count [in]
 *          - count of the number of strings pointed to by the data array
 *  @param  t [in]
 *          - A RIL token structure pointer
 *
 *  @return None
 *
 *  @note   Support: CDMA
 *                   UMTS
 * 
 */
void requestOEMHookStringsSetRfSarState(const char **data, size_t count, RIL_Token t)
{
    ULONG nRet = 0;
    ULONG nRfSarState = 0;
    const char **args = data;
    bool result = true;

    /* Advance to the next argument */
    args++;
    count--;

    /* just one argument is the only acceptable choice */
    if( count != 1 ) {
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        LOGE("RF SAR setting - wrong number of args: %d", count+1);
        return;
    }

    /* The next argument in the list is the RF SAR state to be set */
    nRfSarState = (ULONG)atoi((char *) *args);
    LOGD("%s SAR setting with status %lu ", __func__, nRfSarState);

    if(nRfSarState > 8) {
        LOGE("%s SetRfSarState invalid argument nRfSarState: %lu", __func__, nRfSarState);
        /* Error completion response */
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    /* Launch the request to set SAR status */
    nRet = SLQSSetRfSarState(nRfSarState);
    /* Reply to the upper layer depending on the outcome */
    if (nRet != eQCWWAN_ERR_NONE) {
        LOGD("%s SLQS SetRfSarState failed: %lu", __func__, nRet);
        if (!ATSetRfSarState(nRfSarState)) {
            LOGE("%s AT SetRfSarState failed!", __func__);
            result = false;
        }
    }

    /* Reply to the upper layer depending on the outcome */
    if (result) {
        LOGD("%s  SetRfSarState succeed. ", __func__);
        /* tell the upper layer the result is success */
        RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    }
    else {
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    } 
}

/**
 *  Set SMS wake request to enable WoWW (Wake on Wireless Wide Area Network)
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
void requestOEMHookStringsSetSMSWake(const char **data, size_t count, RIL_Token t)
{
    ULONG nRet;
    ULONG bEnable;
    ULONG wakeMask;
    const char **args = data;
    bool result = true;

    /* Advance to the next argument */
    args++;
    count--;

    /* Two argument is the only acceptable choice */
    if( count != 2 ) {
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        LOGE("%s - wrong number of args: %d", __func__, count+1);
        return;
    }

    /* The next argument in the list is whether SMS WoWW is enabled */
    bEnable = (ULONG)atoi((char *) *args);
    /* SMS wake mask to search for incoming message */
    wakeMask = strtoul((char *) *(args + 1), NULL, 16);
    LOGD("%s bEnable %lu wakeMask %lx", __func__, bEnable, wakeMask);

    nRet = SetSMSWake(bEnable, wakeMask);
    /* Reply to the upper layer depending on the outcome */
    if (nRet != eQCWWAN_ERR_NONE) {
        LOGE("%s SetSMSWake failed: %lu", __func__, nRet);
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    }
    else {
        /* Get back the value */
        bEnable = 0;
        wakeMask = 0;
        nRet = GetSMSWake(&bEnable, &wakeMask);
        if (nRet != eQCWWAN_ERR_NONE) {
            LOGE("%s GetSMSWake failed: %lu", __func__, nRet);
        }
        else
            LOGD("%s GetSMSWake bEnable %lu wakeMask %lx", __func__, bEnable, wakeMask);
        RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    }
}

/**
 *
 * Set power off
 *
 * @return
 *     None
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 *
 */
void setPowerOff(void)
{
    changePowerModeQMI(QMI_POWER_OFFLINE);
    changePowerModeQMI(QMI_POWER_RESET);
}

/**
 *  Reset power of attached modem for RIL_REQUEST_RESET_RADIO
 *
 *  @param  data [in]
 *          - Pointer to the string pointer array
 *  @param  count [in]
 *          - count of the number of strings pointed to by the data array
 *  @param  t [in]
 *          - A RIL token structure pointer
 *
 *  @return None
 *
 *  @note   Support: CDMA
 *                   UMTS
 *          Always return success
 * 
 */
void requestResetRadioQMI(void **data, size_t count, RIL_Token t)
{
    setPowerOff();
    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
}

/**
 * RIL_REQUEST_SHUTDOWN
 *
 * Device is shutting down. All further commands are ignored
 * and RADIO_NOT_AVAILABLE must be returned.
 *
 *  @param  data [in]
 *          - Pointer to the string pointer array
 *  @param  count [in]
 *          - count of the number of strings pointed to by the data array
 *  @param  t [in]
 *          - A RIL token structure pointer
 *
 *  @return None
 *
 *  @note   Support: CDMA
 *                   UMTS
 *          Always return success

 */
void requestShutdownQMI(void *data, size_t datalen, RIL_Token t)
{
    if (currentState() != RADIO_STATE_OFF) {
        changePowerModeQMI(QMI_POWER_OFFLINE);
    }    
    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
}

/**
 *  Initiate a Start Session for OMADM updates
 *
 *  @param  data [in]
 *          - Pointer to the string pointer array, unused
 *  @param  count [in]
 *          - count of the number of strings pointed to by the data array,
 *            unused
 *  @param  t [in]
 *          - A RIL token structure pointer
 *
 *  @return None
 *
 *  @note   Support: CDMA
 *                   Sprint only
 *
 */
void requestOEMHookStringsOMADMStartSession(const char **data, size_t count, RIL_Token t)
{

    char *str = (char*)alloca(3 * sizeof(char));
    ULONG reqType = 0;
    ULONG nRet = 0;
    char modelType[100];
    ULONG startSessionResponse;

    strcpy(str, data[1]);
    reqType = atoi(str);

    LOGD("Entered %s", __func__ );

    memset(modelType,0,sizeof(modelType));  
    if (property_get("persist.radio.module", modelType, NULL) > 0) {
        LOGI("Read device type from property persist.radio.module \n");
    }   
    else {         
         GetModelID(sizeof(modelType),(CHAR*)&modelType);
         property_set("persist.radio.module",modelType);
    }
        
    if (strstr(modelType,"9090") != NULL) {
        /* Supported only for Sprint devices at this time */
        if (sFirmwareInfo.firmwareInfo.dev.g.Carrier == eGOBI_IMG_CAR_SPRINT) {
            LOGD("%s, carrier is Sprint, request code is %lu", __func__, reqType );

            /* Request a client-initiated Profile Update */
            nRet = SLQSOMADMStartSessionExt(reqType);

            if (nRet == eQCWWAN_ERR_NONE) {
                LOGD("%s OMA session started", __func__ );

                /* Make a note of the now-active session */
                s_OMASessionActive = true;

                /* Send a successful completion response */
                RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
            }
            else {
                LOGE("%s Profile Update request error: %lu", __func__, nRet);
                /* Error completion response */
	            RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);   
           }
        }
        else {
            LOGD("%s, Invalid carrier for OMADM %s\n",__func__, sFirmwareInfo.firmwareInfo.dev.s.carrier_str );
            RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        }
    }
    else if ((strstr(modelType,"MC73") != NULL) ||
             (strstr(modelType,"EM73") != NULL) ||
             (strstr(modelType,"MC74") != NULL) ||
             (strstr(modelType,"EM74") != NULL)) {

        /* Supported only for Sprint devices at this time */
        if( (strcmp(sFirmwareInfo.firmwareInfo.dev.s.carrier_str, "11") == 0) || 
            (strcmp(sFirmwareInfo.firmwareInfo.dev.s.carrier_str, "102") == 0) ) {
            LOGD("%s, carrier is Sprint, request code is %lu", __func__, reqType );

            /* Request a client-initiated Profile Update */
            nRet = SLQSOMADMStartSession2(reqType, &startSessionResponse);

            if (nRet == eQCWWAN_ERR_NONE) {
                LOGD("%s OMA session started, firmware availiability: %lu\n",__func__, startSessionResponse );

                /* Make a note of the now-active session */
                s_OMASessionActive = true;

                /* Send a successful completion response */
                RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
            }
            else {
                LOGE("%s Profile Update request error: %lu", __func__, nRet);
                /* Error completion response */
	            RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);   
            }
        }
        else {
            LOGD("%s, Invalid carrier for OMADM %s\n",__func__, sFirmwareInfo.firmwareInfo.dev.s.carrier_str );
            RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        }
    }
    else {
            LOGD("%s, OMADM not supported for this module", __func__ );
            RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    }
}

/**
 *  Cancel the current OMADM session
 *
 *  @param  data [in]
 *          - Pointer to the string pointer array, unused
 *  @param  count [in]
 *          - count of the number of strings pointed to by the data array,
 *            unused
 *  @param  t [in]
 *          - A RIL token structure pointer
 *
 *  @return
 *      none
 *
 *  @note
 *      none
 */
void requestOEMHookStringsOMADMCancelSession(const char **data, size_t count, RIL_Token t)
{
    char *str = (char*)alloca(5 * sizeof(char));
    ULONG reqType = 0;
    ULONG nRet = 0;
    char modelType[100];

    strcpy(str, data[1]);
    reqType = atoi(str);

    LOGD("Entered %s ReqType %d", __func__,reqType );

    memset(modelType,0,sizeof(modelType));  
    if (property_get("persist.radio.module", modelType, NULL) > 0) {
        LOGI("Read device type from property persist.radio.module \n");
    }   
    else {         
         GetModelID(sizeof(modelType),(CHAR*)&modelType);
         property_set("persist.radio.module",modelType);
    }
        
    if (strstr(modelType,"9090") != NULL) {

        /* Supported only for Sprint devices at this time */
        if (sFirmwareInfo.firmwareInfo.dev.g.Carrier == eGOBI_IMG_CAR_SPRINT) {

            /* Request a OMA cancel session*/
            nRet = SLQSOMADMCancelSessionExt(reqType);

            if (nRet == eQCWWAN_ERR_NONE) {

                /* Send a successful completion response */
                RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
                return;
            }
            LOGD("%s :: %ld", __func__, nRet);
        }
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    }
    else if ((strstr(modelType,"MC73") != NULL) ||
             (strstr(modelType,"EM73") != NULL) ||
             (strstr(modelType,"MC74") != NULL) ||
             (strstr(modelType,"EM74") != NULL)) {

        /* Supported only for Sprint devices at this time */
        if( (strcmp(sFirmwareInfo.firmwareInfo.dev.s.carrier_str, "11") == 0) || 
            (strcmp(sFirmwareInfo.firmwareInfo.dev.s.carrier_str, "102") == 0) ) {
            LOGD("%s, carrier is Sprint, request code is %lu", __func__, reqType );

            /* Request a client-initiated Profile cancel */
            nRet = SLQSOMADMCancelSession(reqType);

            if (nRet == eQCWWAN_ERR_NONE) {
                LOGD("%s OMA session cancelled,\n",__func__ );

                /* Send a successful completion response */
                RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
            }
            else {
                LOGE("%s Profile update cancel error: %lu", __func__, nRet);

                /* Error completion response */
	            RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);   
           }
        }
        else {
            LOGD("%s, Invalid carrier for OMADM %s\n",__func__, sFirmwareInfo.firmwareInfo.dev.s.carrier_str );
            RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        }
    }
    else {
            LOGD("%s, OMADM not supported for this module", __func__ );
            RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    }
}

void requestOEMHookStringsOMADMSetSelection(const char **data, size_t count, RIL_Token t)
{
    char *str = (char*)alloca(3 * sizeof(char));
    ULONG reqType = 0;
    ULONG nRet = 0;
    char modelType[100];

    strcpy(str, data[1]);
    reqType = atoi(str);

    LOGD("Entered %s, option %lu", __func__, reqType);

    memset(modelType,0,sizeof(modelType));  
    if (property_get("persist.radio.module", modelType, NULL) > 0) {
        LOGI("Read device type from property persist.radio.module \n");
    }   
    else {         
         GetModelID(sizeof(modelType),(CHAR*)&modelType);
         property_set("persist.radio.module",modelType);
    }
        
    if (strstr(modelType,"9090") != NULL) {

        /* Supported only for Sprint devices at this time */
        if (sFirmwareInfo.firmwareInfo.dev.g.Carrier == eGOBI_IMG_CAR_SPRINT) {

            /* Request a OMA selection session*/
            nRet = SLQSOMADMSendSelectionExt(reqType);

            if (nRet == eQCWWAN_ERR_NONE) {

                /* Send a successful completion response */
                RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
                return;
            }
            LOGD("%s :: %ld", __func__, nRet);
        }
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    }
    else if ((strstr(modelType,"MC73") != NULL) ||
             (strstr(modelType,"EM73") != NULL) ||
             (strstr(modelType,"MC74") != NULL) ||
             (strstr(modelType,"EM74") != NULL)){
        /* Supported only for Sprint devices at this time */
        if( (strcmp(sFirmwareInfo.firmwareInfo.dev.s.carrier_str, "11") == 0) || 
            (strcmp(sFirmwareInfo.firmwareInfo.dev.s.carrier_str, "102") == 0) ) {
            LOGD("%s, carrier is Sprint, request code is %lu", __func__, reqType );

            /* Defer by 30 minutes by default */
            if (reqType == 3)
            {
                ULONG deferTime = 30;
                nRet = SLQSOMADMSendSelection2(reqType, &deferTime, NULL );
            }
            else
            {
                nRet = SLQSOMADMSendSelection2(reqType, NULL, NULL );
            }

            if (nRet == eQCWWAN_ERR_NONE) {
                LOGD("%s OMA session selected,\n",__func__ );

                /* Send a successful completion response */
                RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
            }
            else {
                LOGE("%s Profile update selection error: %lu", __func__, nRet);

                /* Error completion response */
	            RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);   
           }
        }
        else {
            LOGD("%s, Invalid carrier for OMADM %s. ",__func__, sFirmwareInfo.firmwareInfo.dev.s.carrier_str );
            RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        }
    }
    else {
            LOGD("%s, OMADM not supported for this module", __func__ );
            RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    }
}

#pragma pack(1)

// Used for SL9090 series modems
union _retreivedData
{
    struct omaDmFotaTlvExt fotaTlv;
    struct omaDmConfigTlvExt configTlv;
};

typedef struct _omaResultInfo
{
    BYTE session;
    union _retreivedData retrievedData;
    
}omaResultInfo;

// Used for 9x15 series modems (MC73xx/EM73xx)
union swiOmaIndInfo
{
    struct omaDmFotaTlv fotaTlv;
    struct omaDmConfigTlv configTlv;
    struct omaDmNotificationsTlv omaDmNotifications;
};

typedef struct _swiOmaResultInfo
{
    BYTE session;
    union swiOmaIndInfo indInfo;
    
}swiOmaResultInfo;

void swapBytes(BYTE* source, int length, int chunk)
{
    int i = 0;
    switch(chunk)
    {
        case 2:
            for (i = 0; i< length;i+=2)
            {
                BYTE temp = source[i];
                source [i] = source[i+1];
                source[i+1] = temp;
            }
            break;
        case 4:
            for (i = 0; i<length;i+=4)
            {
                BYTE first = source[i];
                BYTE second = source[i+1];
                source[i] = source[i+3];
                source[i+1] = source[i+2];
                source[i+2] = second;
                source[i+3] = first;
            }
            break;
    }
}

void formatconfigTlvEndianNess(struct omaDmConfigTlvExt* tlvData)
{
    swapBytes((BYTE*)&tlvData->userInputTimeout,sizeof(tlvData->userInputTimeout),2);
    swapBytes((BYTE*)&tlvData->alertmsglength,sizeof(tlvData->alertmsglength),2);
    swapBytes((BYTE*)&tlvData->alertmsg,sizeof(tlvData->alertmsg),2);
}

void formatFUMOtlvEndianNess ( struct omaDmFotaTlvExt* tlvData)
{
    swapBytes((BYTE*)&tlvData->userInputTimeout,sizeof(tlvData->userInputTimeout),2);
    swapBytes((BYTE*)&tlvData->packageSize,sizeof(tlvData->packageSize),4);
    swapBytes((BYTE*)&tlvData->receivedBytes,sizeof(tlvData->receivedBytes),4);
    swapBytes((BYTE*)&tlvData->fumoResultCode,sizeof(tlvData->fumoResultCode),2);
    swapBytes((BYTE*)&tlvData->versionlength,sizeof(tlvData->versionlength),2);
    swapBytes((BYTE*)&tlvData->version,sizeof(tlvData->version),2);
    swapBytes((BYTE*)&tlvData->namelength,sizeof(tlvData->namelength),2);
    swapBytes((BYTE*)&tlvData->package_name,sizeof(tlvData->package_name),2);
    swapBytes((BYTE*)&tlvData->descriptionlength,sizeof(tlvData->descriptionlength),2);
    swapBytes((BYTE*)&tlvData->description,sizeof(tlvData->description),2);
}

void formatSwiconfigTlvEndianNess(struct omaDmConfigTlv* tlvData)
{
    swapBytes((BYTE*)&tlvData->userInputTimeout,sizeof(tlvData->userInputTimeout),2);
    swapBytes((BYTE*)&tlvData->alertmsglength,sizeof(tlvData->alertmsglength),2);
    swapBytes((BYTE*)&tlvData->alertmsg,sizeof(tlvData->alertmsg),2);
}

void formatSwinotificationTlvEndianNess(struct omaDmNotificationsTlv* tlvData)
{
    swapBytes((BYTE*)&tlvData->sessionStatus,sizeof(tlvData->sessionStatus),2);
}

void formatSwiFUMOtlvEndianNess ( struct omaDmFotaTlv* tlvData)
{
    swapBytes((BYTE*)&tlvData->userInputTimeout,sizeof(tlvData->userInputTimeout),2);
    swapBytes((BYTE*)&tlvData->fwdloadsize,sizeof(tlvData->fwdloadsize),4);
    swapBytes((BYTE*)&tlvData->fwloadComplete,sizeof(tlvData->fwloadComplete),4);
    swapBytes((BYTE*)&tlvData->updateCompleteStatus,sizeof(tlvData->updateCompleteStatus),2);
    swapBytes((BYTE*)&tlvData->versionlength,sizeof(tlvData->versionlength),2);
    swapBytes((BYTE*)&tlvData->version,sizeof(tlvData->version),2);
    swapBytes((BYTE*)&tlvData->namelength,sizeof(tlvData->namelength),2);
    swapBytes((BYTE*)&tlvData->package_name,sizeof(tlvData->package_name),2);
    swapBytes((BYTE*)&tlvData->descriptionlength,sizeof(tlvData->descriptionlength),2);
    swapBytes((BYTE*)&tlvData->description,sizeof(tlvData->description),2);
}

void omaDmCallback(ULONG sessionType, BYTE* psessionTypeFields)
{
    ATResponse *atresponse = NULL;

    /* enable OMADM log store in EFS */
    /* at_send_command_numeric("AT+OMALOG=1", &atresponse);*/

    if (sessionType == 0x00) //FUMO
    {
        omaResultInfo  result;
        memset((void*)&result,0,sizeof(result));
        LOGD("omaDMCallback invoked with FUMO event");
        result.session = SWI_IND_OMADM_STATE_FUMO;
        result.retrievedData.fotaTlv = *(struct omaDmFotaTlvExt*)psessionTypeFields;
        formatFUMOtlvEndianNess(&result.retrievedData.fotaTlv);
        RIL_onUnsolicitedResponse(RIL_UNSOL_OEM_HOOK_RAW,(void*)&result, sizeof(omaResultInfo));
    }
    else if (sessionType == 0x01) //DMconfig
    {
        omaResultInfo  result;
        memset((void*)&result,0,sizeof(result));
        LOGD("omaDMCallback invoked with CONFIG event");
        result.session = SWI_IND_OMADM_STATE_CONFIG;
        result.retrievedData.configTlv = *(struct omaDmConfigTlvExt*)psessionTypeFields;
        formatconfigTlvEndianNess(&result.retrievedData.configTlv);
        RIL_onUnsolicitedResponse(RIL_UNSOL_OEM_HOOK_RAW,(void*)&result, sizeof(omaResultInfo));
    }
}

void SLQSOMADMAlertCallback(ULONG sessionType, BYTE* psessionTypeFields)
{
    swiOmaResultInfo  result;

    memset((void*)&result,0,sizeof(result));
    
    // Session type FOTA
    if (sessionType == 0x00)
    {
        LOGD("SLQSOMADMAlertCallback invoked with FOTA event");
        result.session = SWI_IND_OMADM_STATE_FUMO;
        result.indInfo.fotaTlv = *(struct omaDmFotaTlv*)psessionTypeFields;
        formatSwiFUMOtlvEndianNess(&result.indInfo.fotaTlv);
        RIL_onUnsolicitedResponse(RIL_UNSOL_OEM_HOOK_RAW,(void*)&result, sizeof(swiOmaResultInfo));
    }
    
    // Session type CONFIG
    else if (sessionType == 0x01)
    {
        LOGD("SLQSOMADMAlertCallback invoked with CONFIG event");
        result.session = SWI_IND_OMADM_STATE_CONFIG;
        result.indInfo.configTlv = *(struct omaDmConfigTlv*)psessionTypeFields;
        formatSwiconfigTlvEndianNess(&result.indInfo.configTlv);
        RIL_onUnsolicitedResponse(RIL_UNSOL_OEM_HOOK_RAW,(void*)&result, sizeof(swiOmaResultInfo));
    }

    // Session type  NOTIFICATION
    else if (sessionType == 0x02)
    {
        LOGD("SLQSOMADMAlertCallback invoked with NOTIFICATION event");
        result.session = SWI_IND_OMADM_STATE_NOTIF;
        result.indInfo.omaDmNotifications = *(struct omaDmNotificationsTlv*)psessionTypeFields;
        formatSwinotificationTlvEndianNess(&result.indInfo.omaDmNotifications);
        RIL_onUnsolicitedResponse(RIL_UNSOL_OEM_HOOK_RAW,(void*)&result, sizeof(swiOmaResultInfo));
    }
}

/**
 *  Initiate the CDMA activation process for verizon on SL9090
 *
 *  @param  data [in]
 *          - Pointer to the string pointer array, unused
 *  @param  count [in]
 *          - count of the number of strings pointed to by the data array,
 *            unused
 *  @param  t [in]
 *          - A RIL token structure pointer
 *
 *  @return None
 *
 *  @note   Support: CDMA
 *                   Verizon only
 *
 */
void requestOEMHookStringsCMDAActVerizonSL9090(const char **data, size_t count, RIL_Token t)
{
    ULONG nRet = 0;
    unsigned char activationstart_response[2];
   
    
    LOGD("Entered %s", __func__ );  
    if (sFirmwareInfo.firmwareInfo.dev.g.Carrier == eGOBI_IMG_CAR_VERIZON) {
        LOGD("%s, carrier is Verizon, ", __func__);
        RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
        /*
           ActivateAutomatic() function requests the device to perform 
           automatic service activation. Currently, automatic activation is 
           not complete until the device has been reset. For this reason, the 
           recommended approach when using ActivateAutomatic is for the application 
           to perform the following steps:
            1. Enable the activation status callback via SetActivationStatusCallback().
            2. Call ActivateAutomatic().
            3. Monitor activation status (via callback) for a value of 1 
               (service activated).
            4. When this value is received, call SetPower( 5 ).
            5. Call QCWWANDisconnect().
            6. Reconnect after the device power cycles.
         */
        /* hardcode activation code now, Windows RIL does the same */
        nRet = ActivateAutomatic("*22899");
        
        if (nRet == eQCWWAN_ERR_NONE) {
            LOGD("activateServiceCDMA OTASP starting");

            activationstart_response[0] = SWI_IND_OTASP_START_STATUS;
            activationstart_response[1] = QMI_OTASP_START_STATUS_STARTED;
            
            RIL_onUnsolicitedResponse(RIL_UNSOL_OEM_HOOK_RAW,
                              activationstart_response, sizeof(activationstart_response));
        }
        else {
            LOGE("activateServiceCDMA OTASP error: %lu", nRet);
            activationstart_response[0] = SWI_IND_OTASP_START_STATUS;
            activationstart_response[1] = QMI_OTASP_START_STATUS_ABORTED;
            
            RIL_onUnsolicitedResponse(RIL_UNSOL_OEM_HOOK_RAW,
                              activationstart_response, sizeof(activationstart_response));
        }    
    }
    else {
        LOGD("%s, carrier is %d. Activation cancelled",
             __func__, (int) sFirmwareInfo.firmwareInfo.dev.g.Carrier );
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    }
}


static void VoiceOtaStatusCb(voiceOTASPStatusInfo *pOtaStatusInfo)
{
    int response;

    if(pOtaStatusInfo == NULL) {
        LOGE("%s:: pOtaStatusInfo is NULL\n", __func__);
        return;
    }
    LOGD("%s:: OTASPStatus is %d\n", __func__, pOtaStatusInfo->OTASPStatus);

    /** 
     * RIL_CDMA_OTA_ProvisionStatus defined in ril.h has the mapping of 
     * status codes returned in this indication 
     */  
    if (pOtaStatusInfo->OTASPStatus == 0x0C) {
        response = CDMA_OTA_PROVISION_STATUS_COMMITTED;
    }
    else {
        response = (int)pOtaStatusInfo->OTASPStatus;
    }    
    RIL_onUnsolicitedResponse(RIL_UNSOL_CDMA_OTA_PROVISION_STATUS,
                              &response, sizeof(int *));  
}


/**
 * Register Voice OTA status notification call back function,
 * registered call back retrives the notification for OTASP.
 * This is required when OTASP is initiated by *228 dialing
 *
 * @param 
 *     none
 *
 * @return
 *     none
 */
void registerVoiceOtaStatusCallback(void)
{
    LOGD("%s:: entered\n", __func__);
    ULONG nRet = 0;

    /* register callback function */
    nRet = SLQSVoiceSetOTASPStatusCallBack(VoiceOtaStatusCb);
    if ( nRet != eQCWWAN_ERR_NONE ) {
        LOGE("%s:: SLQSWmsMessageWaitingCallBack failed, nRet: %lu\n", __func__, nRet );
    }
}


