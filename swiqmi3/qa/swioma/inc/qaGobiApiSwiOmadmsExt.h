/**
 * \ingroup swioma
 *
 * \file    qaGobiApiSwiOmadms.h
 *
 * \brief   SWI Open Mobile Alliance Device Management Service API function
 *          prototypes
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */
#ifndef __GOBI_API_SWIOMADMSEXT_H__
#define __GOBI_API_SWIOMADMSEXT_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  Starts an OMA-DM session.
 *
 *  \param  sessionType[IN]
 *          - Session type
 *          0: Client Initiated Device Configuration (CIDC)
 *          1: Client initiated FUMO
 *          2: Device initiated FUMO
 *          3: Device Initiated DM
 *          4. Client Initiated PRL
 *          5. User Initiated HFA 
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Technology Supported: UMTS/CDMA\n
 *          Device Supported: MC7750\n
 *          Timeout: 20 seconds
 */
ULONG SLQSOMADMStartSessionExt(
    ULONG sessionType );

/**
 *  Cancels an ongoing OMA-DM session.
 *
 *  \param  session[IN]
 *          - Session
 *          0: Client Initiated Device Configuration (CIDC)
 *          1: Client initiated FUMO
 *          2: Device initiated FUMO
 *          3: Device Initiated DM
 *          4. Client Initiated PRL
 *          5. User Initiated HFA 
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Technology Supported: UMTS/CDMA\n
 *          Device Supported: MC7750\n
 *          Timeout: 20 seconds
 */
ULONG SLQSOMADMCancelSessionExt(
    ULONG session );
    
    
/**
 *  Sends the specified OMA-DM selection for the current network initiated
 *  session.
 *
 *  \param  selection[IN]
 *          - OMA-DM NIA Selection
 *              0 - User selects NO
 *              1 - User selects "YES/OK"
 *              2 - User selects "Delay"
 *              3 - User selects "Delay one hour"
 *              4 - User selects "Delay four hours"
 *              5 - User selects "Delay until 2:00 AM"
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Technology Supported: UMTS/CDMA\n
 *          Device Supported: MC7750\n
 *          Timeout: 20 seconds
 */
ULONG SLQSOMADMSendSelectionExt(
    ULONG selection );
    

/**
 *  Sets the OMA-DM settings requested.
 *
 *  \param  bFOTAdownload[IN]
 *          - Firmware AutoDownload
 *              - 0x00 - Firmware autodownload FALSE
 *              - 0x01 - Firmware autodownload TRUE
 *
 *  \param  bFOTAUpdate[IN]
 *          - Firmware AutoUpdate
 *              - 0x00 - Firmware autoupdate FALSE
 *              - 0x01 - Firmware autoupdate TRUE
 * 
 *  \param  bOMADMEnable[IN]
 *          - OMADM Enable
 *          - 0x00 - OMADM disabled
 *          - 0x01 - OMADM Enabled
 *
 *  \param bOMADMLOGEnable[IN]
 *          - OMADM Logs enabled
 *          - 0x00 - OMADM Logs disabled
 *          - 0x01 - OMADM Logs enabled
 *
 *  \param bFUMOEnable[IN]
 *          - FUMO Enabled
 *          - 0x00 - Fumo disabled
 *          - 0x01 - Fumo enabled
 *
 *  \param bPRLEnable[IN]
 *          - PRL Enabled
 *              - 0x00 - PRL Disabled
 *              - 0x01 - PRL Enabled
 *              - 0x02 - Launch a CI PRL
 *              - 0x03 - PRL Enabled and PRL automatic update every 45 days
 *              - 0x04 - PRL Enabled and PRL update ever 90 days 
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Technology Supported: UMTS/CDMA\n
 *          Device Supported: MC7750\n
 *          Timeout: 20 seconds
 */
ULONG SLQSOMADMSetSettingsExt(
    ULONG bFOTAdownload,
    ULONG bFOTAUpdate,
    ULONG bOMADMEnable,
    ULONG bOMALOGEnable,
    ULONG bFUMOEnable,
    ULONG bPRLEnable );

/**
 *  Structure containing the OMA DM settings to be set on the device
 *
 *  \param  FOTAdownload
 *          - 1 Byte parameter indicating support for FOTA Automatic download
 *              - 0x00 - Firmware autodownload FALSE
 *              - 0x01 - Firmware autodownload TRUE
 *
 *  \param  FOTAUpdate
 *          - 1 byte parameter indicating FOTA Automatic update
 *              - 0x00 - Firmware autoupdate FALSE
 *              - 0x01 - Firmware autoupdate TRUE
 *
 *  \param  OMADMEnable[IN]
 *          -  1 byte parameter indicating OMA DM
 *              - 0x00 - OMA DM Disabled
 *              - 0x01 - OMA DM Enabled
 *
 *  \param  OMADMLogEnable[IN]
 *          -  1 byte parameter indicating OMA DM Logs
 *              - 0x00 - OMA DM Logs Disabled
 *              - 0x01 - OMA DM Logs Enabled
 *
 *  \param  FUMOEnable[IN]
 *          -  1 byte parameter indicating FUMO enabled
 *              - 0x00 - FUMO Disabled
 *              - 0x01 - FUMO enabled
 *
 *  \param  PRLEnable[IN]
 *          -  1 byte parameter indicating PRL Enabled
 *              - 0x00 - PRL Disabled
 *              - 0x01 - PRL Enabled
 *              - 0x02 - Launch a CI PRL
 *              - 0x03 - PRL Enabled and PRL automatic update every 45 days
 *              - 0x04 - PRL Enabled and PRL update ever 90 days 
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
typedef struct _SLQSOMADMSettingsReqParamsExt
{
    BYTE  FOTAdownload;
    BYTE  FOTAUpdate;
    BYTE  OMADMEnable;
    BYTE  OMADMLogEnable;
    BYTE  FUMOEnable;
    BYTE  PRLEnable;
} SLQSOMADMSettingsReqParamsExt;


/**
 * Sets the settings related to OMADM. These settings are saved on the modem
 * across power cycles.
 *
 *  \param pSLQSOMADMSettingsReqParams[IN]
 *         - See \ref SLQSOMADMSettingsReqParamsExt for more information
 *
 *  \note   Technology Supported: UMTS/CDMA\n
 *          Device Supported: MC7750\n
 *          Timeout: 20 seconds
 */
ULONG SLQSOMADMSetSettings2Ext(
    SLQSOMADMSettingsReqParamsExt *pSLQSOMADMSettingsReqParams);


/**
 *  Structure containing the OMA DM settings to be set on the device
 *
 *  \param  pOMAModInfoResult
 *          - 1 Byte parameter indicating OMADM result info
 *             0 – OMA module initiation not completed.
 *             1 – OMA module initiation completed but no UI is registered.
 *             128 – OMA initiation completed.
 *             255 – OMA initiation error.
 *
 *  \param  pSessionState
 *          - 1 byte parameter indicating session state
 *             0 - OMA module in CIDC session.
 *             1 - OMA module in CIFUMO session
 *             2 - OMA module in device initiated FUMO session
 *             3 - OMA module in FUMO resume session
 *             4 - OMA module is sending FUMO report
 *             5 - OMA module in device initiated DM session
 *             6 - OMA module in HFA DC session
 *             7 - OMA module in CI PRL session
 *             8 - OMA module in device initiated PRL session
 *             9 - OMA module in HFA PRL session
 *             10 - OMA module in HFA FUMO session
 *             11 - OMA module in NI PRL session
 *             12 - OMA is storing configuration after DC/PRL session
 *             13 - OMA module is blocked by UI
 *             14 - there is a pending session
 *             15 - OMA module in idle state
 *
 *  \param  pHFAStatus
 *          -  1 byte parameter indicating HFA status
 *             0 – HFA completed
 *             1 – HFA not start
 *             2 – HFA DC completed, but PRL is not completed
 *             3 – HFA PRL completed, but FUMO is not completed 
 *
 *  \param  pHFAMaxretryTime
 *          -  2 byte parameter indicating HFA max retry time
 *              - 0x00 - OMA DM Logs Disabled
 *              - 0x01 - OMA DM Logs Enabled
 *
 *  \param  pRetryInterval
 *          -  2 byte parameter indicating retry interval
 *              - 0x00 - FUMO Disabled
 *              - 0x01 - FUMO enabled
 *
 *  \param  pRetryingTimes
 *          -  2 byte parameter indicating retrying times 
 *
 *  \param  pFumoState
 *          -  1 byte parameter indicating FumoState
 *             0 – FUMO state READY
 *             1 – FUMO state DISCOVERY
 *             2 –  FUMO state DISCOCONF
 *             3 –  FUMO state DOWNLOAD
 *             4 – FUMO state RECEIVED
 *             5 – FUMO state CONFIRM
 *             6 – FUMO state CONFIRMED
 *             7 – FUMO state UPDATE
 *             8 – FUMO state UPDATING
 *             9 – FUMO state UNUSABLE
 *             10 – FUMO state REJECTED
 *             11 – FUMO state REJECTCONF
 *             12 – FUMO state FAILED
 *             13 – FUMO state DENIED
 *             14 – FUMO state UPDATED
 *             15 – FUMO state FINCONF
 *             16 – FUMO state FINISHED
 *
 *  \param  pFUMOResultCode
 *          -  4 byte parameter indicating FUMO result code
 *
 *  \param  pPkgVendorNameLength
 *          -  2 byte parameter indicating PkgVendorNameLength
 *
 *  \param  pPkgVendorName
 *          -  array of characters indicating PkgVendorName
 *
 *  \param  pPkgSize
 *          -  4 byte parameter indicating PkgSize
 *
 *  \param  pPkgVersionLength
 *          -  2 byte parameter indicating PkgVersionLength
 *
 *  \param  pPkgVersion
 *          -  array of characters indicating PkgVersion
 *
 *  \param  pPkgDescLength
 *          -  2 byte parameter indicating PkgDescLength
 *
 *  \param  pPkgDescription
 *          -  array of characters indicating PkgDescription
 *
 *  \param  pPkgDateLength
 *          -  2 byte parameter indicating PkgDateLength
 *
 *  \param  pPkgDate
 *          -  array of characters indicating PkgDate
 *
 *  \param  pPkgInstallTimeLength
 *          -  2 byte parameter indicating pPkgInstallTimeLength
 *
 *  \param  pPkgInstallTime
 *          -  array of characters indicating PkgInstallTime
 *
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */

typedef struct _SLQSOMADMSessionInfoExt
{
    BYTE  *pOMAModInfoResult;
    BYTE  *pSessionState;
    BYTE  *pHFAStatus;
    WORD  *pHFAMaxretryTime;
    WORD  *pRetryInterval;
    WORD  *pRetryingTimes;
    BYTE  *pFUMOState;
    ULONG *pFUMOResultCode;
    WORD  *pPkgVendorNameLength;
    BYTE  *pPkgVendorName;
    ULONG *pPkgSize;

    WORD  *pPkgVersionLength;
    BYTE  *pPkgVersion;

    WORD  *pPkgNameLength;
    BYTE  *pPkgName;

    WORD  *pPkgDescLength;
    BYTE  *pPkgDescription;

    WORD  *pPkgDateLength;
    BYTE  *pPkgDate;

    WORD  *pPkgInstallTimeLength;
    BYTE  *pPkgInstallTime;

} SLQSOMADMSessionInfoExt;

ULONG SLQSOMADMGetSessionInfoExt(
    SLQSOMADMSessionInfoExt *pResp );



#endif /* __GOBI_API_SWIOMADMS_H__ */
