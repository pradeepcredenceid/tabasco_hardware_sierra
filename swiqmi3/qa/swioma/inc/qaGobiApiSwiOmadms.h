/**
 * \ingroup swioma
 *
 * \file    qaGobiApiSwiOmadms.h
 *
 * \brief   SWI Open Mobile Alliance Device Management Service API function
 *          prototypes
 *
 * Copyright: © 2011-2013 Sierra Wireless, Inc. all rights reserved
 *
 */
#ifndef __GOBI_API_SWIOMADMS_H__
#define __GOBI_API_SWIOMADMS_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  Starts an OMA-DM session.
 *
 *  \param  sessionType[IN]
 *          - Session type
 *              - 0x01 - FOTA, to check availability of FW Update
 *              - 0x02 - DM, to check availability of DM Update
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 20 seconds
 */
ULONG SLQSOMADMStartSession(
    ULONG sessionType );

/**
 *  Cancels an ongoing OMA-DM session.
 *
 *  \param  session[IN]
 *          - Session
 *              - 0x01 - FOTA, to check availability of FW Update
 *              - 0x02 - DM, to check availability of DM Update
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 20 seconds
 */
ULONG SLQSOMADMCancelSession(
    ULONG session );

/**
 *  Structure containing the OMA DM Session Info returned by the device.
 *  Also used as input parameter to specify the size of variable parameters.
 *  (ref. notes)
 *
 *  \param  pStatus
 *          - 1 Byte parameter indicating status
 *              - 0x01 - No Firmware available
 *              - 0x02 - Query Firmware Download
 *              - 0x03 - Firmware Downloading
 *              - 0x04 - Firmware Downloaded
 *              - 0x05 - Query Firmware Update
 *              - 0x06 - Firmware Updating
 *              - 0x07 - Firmware Update
 *
 *  \param  pUpdateCompleteStatus
 *          - 2 byte parameter indicating Update Complete Status
 *              - See qaGobiApiTableSwiOMADMUpdateCompleteStatus.h
 *                Update Complete Status
 *
 *  \param  pSeverity
 *          - 1 byte parameter indicating severity
 *              - 0x01 - Mandatory
 *              - 0x02 - Optional
 *
 *  \param  pSourceLength
 *          - 2 byte parameter indicating Length of Vendor Name String
 *            in Bytes.
 *
 *  \param  pSource
 *          - Variable length parameter indicating Vendor Name in ASCII
 *
 *  \param  pPkgNameLength
 *          - 2 byte parameter indicating Length of Package Name String
 *            in Bytes.
 *
 *  \param  pPkgName
 *          - Variable length parameter indicating Package Name in ASCII
 *
 *  \param  pPkgDescLength
 *          - 2 byte parameter indicating Length of Package Description String
 *            in Bytes.
 *
 *  \param  pPkgDescription
 *          - Variable length parameter indicating Package Description in ASCII
 *
 *  \param  pDateLength
 *          - 2 byte parameter indicating Length of Package Description String
 *            in Bytes.
 *
 *  \param  pDate
 *          - Variable length parameter indicating Package Description in ASCII
 *
 *  \param  pTimeLength
 *          - 2 byte parameter indicating Length of Time String in Bytes.
 *
 *  \param  pTime
 *          - Variable length parameter indicating Time String in ASCII
 *
 *  \param  pSessionType
 *          - 1 byte parameter reflects the last session started for Sprint
 *              - 0x00 - No session since boot
 *              - 0x01 - Sprint CI-DC Session
 *              - 0x02 - Sprint CI-PRL Session
 *              - 0x03 - Sprint CI-FUMO Session
 *              - 0x04 - Sprint HFA-DC Session
 *              - 0x05 - Sprint HFA-PRL Session
 *              - 0x06 - Sprint HFA-FUMO Session
 *              - 0x07 - Sprint NI Session
 *
 *  \param  pSessionState
 *          - 1 byte parameter indicating session state
 *              - 0x01 - idle
 *              - 0x02 - active
 *              - 0x03 - pending
 *
 *  \param  pRetryCount
 *          - 1 byte parameter indicating retries left count
 *              - valid values 0 to 6
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   As input parameter the members pSourceLength, pPkgNameLength
 *          pPkgDescLength, pDateLength, pTimeLength have to be specified.
 *          These should contain the initialized size of pSource, pPkgName,
 *          pPkgDescription, pDate, pTime respectively.
 *
 */
typedef struct _SLQSOMADMSessionInfo
{
    BYTE  *pStatus;
    WORD  *pUpdateCompleteStatus;
    BYTE  *pSeverity;
    WORD  *pSourceLength;
    BYTE  *pSource;
    WORD  *pPkgNameLength;
    BYTE  *pPkgName;
    WORD  *pPkgDescLength;
    BYTE  *pPkgDescription;
    WORD  *pDateLength;
    BYTE  *pDate;
    WORD  *pTimeLength;
    BYTE  *pTime;
    BYTE  *pSessionType;
    BYTE  *pSessionState;
    BYTE  *pRetryCount;
} SLQSOMADMSessionInfo;

/**
 *  Returns information related to the current (or previous if no session is
 *  active) OMA-DM session.
 *
 *  \param  SessionType[IN]
 *          - Session type
 *              - 0x01 - FOTA
 *
 *  \param  pResp[IN/OUT]
 *          - See \ref SLQSOMADMSessionInfo for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 20 seconds
 */
ULONG SLQSOMADMGetSessionInfo(
    ULONG                *pSessionType,
    SLQSOMADMSessionInfo *pResp );

/**
 *  Sends the specified OMA-DM selection for the current network initiated
 *  session.
 *
 *  \param  selection[IN]
 *          - OMA-DM NIA Selection
 *              - 0x01 - Accept
 *              - 0x02 - RejectSession
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 20 seconds
 */
ULONG SLQSOMADMSendSelection(
    ULONG selection );

/**
 *  Returns the OMA-DM settings.
 *
 *  \param  pbOMADMEnabled[OUT]
 *          - Device OMADM service enabled
 *              - 0x00000001 - Client-initiated device configuration
 *              - 0x00000002 - Network-initiated device configuration
 *              - 0x00000010 - Client-initiated FUMO
 *              - 0x00000020 - Network-initiated FUMO
 *
 *  \param  pbFOTAdownload[OUT]
 *          - Firmware AutoDownload
 *              - 0x00 - Firmware autodownload FALSE
 *              - 0x01 - Firmware autodownload TRUE
 *
 *  \param  pbFOTAUpdate[OUT]
 *          - Firmware AutoUpdate
 *              - 0x00 - Firmware autoupdate FALSE
 *              - 0x01 - Firmware autoupdate TRUE
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 20 seconds
 */
ULONG SLQSOMADMGetSettings(
    ULONG *pbOMADMEnabled,
    ULONG *pbFOTAdownload,
    ULONG *pbFOTAUpdate );

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
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 20 seconds
 */
ULONG SLQSOMADMSetSettings(
    ULONG bFOTAdownload,
    ULONG bFOTAUpdate );

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
 *  \param  pAutosdm[IN]
 *          - Optional 1 byte parameter indicating OMA Automatic UI
 *            Alert Response
 *              - 0x00 - Disabled
 *              - 0x01 - Enabled Accept
 *              - 0x02 - Enabled Reject
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
typedef struct _SLQSOMADMSettingsReqParams
{
    BYTE  FOTAdownload;
    BYTE  FOTAUpdate;
    BYTE  *pAutosdm;
} SLQSOMADMSettingsReqParams;

/**
 * Sets the settings related to OMADM. These settings are saved on the modem
 * across power cycles.
 *
 *  \param pSLQSOMADMSettingsReqParams[IN]
 *         - See \ref SLQSOMADMSettingsReqParams for more information
 *
 *  \note   Timeout: 20 seconds
 */
ULONG SLQSOMADMSetSettings2(
    SLQSOMADMSettingsReqParams *pSLQSOMADMSettingsReqParams);

/**
 *  Structure containing the OMA DM settings retrieved from the device
 *
 *  \param  pOMADMEnabled[OUT]
 *          - Optional 4 byte parameter indicating OMADM service enabled
 *              - 0x00000001 - Client-initiated device configuration
 *              - 0x00000002 - Network-initiated device configuration
 *              - 0x00000010 - Client-initiated FUMO
 *              - 0x00000020 - Network-initiated FUMO
 *          - function SLQSOMADMGetSettings2() returns a default value
 *            0xFFFFFFFF in case this parameter is not returned by the modem.
 *
 *  \param  pFOTAdownload[OUT]
 *          - Optional 1 Byte parameter indicating support for FOTA Automatic
 *            download
 *              - 0x00 - Firmware autodownload FALSE
 *              - 0x01 - Firmware autodownload TRUE
 *          - function SLQSOMADMGetSettings2() returns a default value 0xFF
 *            in case this parameter is not returned by the modem.
 *
 *  \param  pFOTAUpdate[OUT]
 *          - Optional 1 byte parameter indicating FOTA Automatic update
 *              - 0x00 - Firmware autoupdate FALSE
 *              - 0x01 - Firmware autoupdate TRUE
 *          - function SLQSOMADMGetSettings2() returns a default value 0xFF
 *            in case this parameter is not returned by the modem.
 *
 *  \param  pAutosdm[OUT]
 *          - Optional 1 byte parameter indicating OMA Automatic UI Alert
 *            Response
 *              - 0x00 - Disabled
 *              - 0x01 - Enabled Accept
 *              - 0x02 - Enabled Reject
 *          - function SLQSOMADMGetSettings2() returns a default value 0xFF
 *            in case this parameter is not returned by the modem.
 *
 *  \param  pFwAutoCheck[OUT]
 *          - Optional 1 byte parameter indicating OMA Automatic Check for
 *            Firmware Update on Power-Up
 *            Response
 *              - 0x00 - Disabled
 *              - 0x01 - Enabled
 *          - function SLQSOMADMGetSettings2() returns a default value 0xFF
 *            in case this parameter is not returned by the modem.
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
typedef struct _SLQSOMADMSettings
{
    ULONG *pOMADMEnabled;
    BYTE  *pFOTAdownload;
    BYTE  *pFOTAUpdate;
    BYTE  *pAutosdm;
    BYTE  *pFwAutoCheck;
} SLQSOMADMSettings;

/**
 *  Retrieves the OMA-DM settings from the device.
 *
 *  \param  SLQSOMADMSettingsReqParams
 *          - See \ref SLQSOMADMSettings for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 20 seconds
 */
ULONG SLQSOMADMGetSettings2( SLQSOMADMSettings *pSLQSOMADMSettings );

/**
 *  Starts an OMA-DM session.
 *
 *  \param  sessionType[IN]
 *          - Session type
 *              - 0x01 - FOTA, to check availability of FW Update
 *              - 0x02 - DM, to check availability of DM Update
 *              - 0x03 - PRL,  to check availability of PRL Update
 *
 *  \param  pFwAvailability[OUT]
 *          - OMA-DM CHECK FW Available
 *              - 0x00000001 - FW Available. For CIDC and CIPRL, this value
 *                             will be returned by the modem. CIDC and CIPRL
 *                             are asynchronous OMADM sessions.
 *              - 0x00000002 - FW Not Available
 *              - 0x00000003 - FW Check Timed Out
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 20 seconds
 */
ULONG SLQSOMADMStartSession2(
    ULONG sessionType,
    ULONG *pFwAvailability);

/**
 *  Sends the specified OMA-DM selection for the current network initiated
 *  session.
 *
 *  \param  selection[IN]
 *          - OMA-DM NIA Selection
 *              - 0x01 - Accept
 *              - 0x02 - Reject
 *              - 0x03 - Defer
 *
 *  \param  pDeferTime[IN]
 *          - Defer time in minutes. A value of 0 will cause the prompt
 *            to be resent immediately.
 *          - This TLV is mandatory if selection is set to 0x03.
 *
 *  \param  pRejectReason[IN]
 *          - Reject Reason
 *          - This TLV is processed if selection is set to 0x02.
 *            If it is not present, the reject reason 0 is used as default.
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 20 seconds
 */
ULONG SLQSOMADMSendSelection2(
    ULONG selection,
    ULONG *pDeferTime,
    ULONG *pRejectReason);

/**
 *  Structure containing the OMA DM settings to be set on the device
 *
 *  \param  FOTAdownload
 *          - 1 Byte parameter indicating support for FOTA Automatic download
 *              - 0x00 - Firmware auto download FALSE
 *              - 0x01 - Firmware auto download TRUE
 *
 *  \param  FOTAUpdate
 *          - 1 byte parameter indicating FOTA Automatic update
 *              - 0x00 - Firmware auto update FALSE
 *              - 0x01 - Firmware auto update TRUE
 *
 *  \param  pAutosdm[IN]
 *          - Optional 1 byte parameter indicating OMA Automatic UI
 *            Alert Response
 *              - 0x00 - Disabled
 *              - 0x01 - Enabled Accept
 *              - 0x02 - Enabled Reject
 *
 *  \param  pFwAutoCheck[IN]
 *          - Optional 1 byte parameter indicating OMA Automatic Check for
 *            Firmware Update on Power-Up
 *            Response
 *              - 0x00 - Disabled
 *              - 0x01 - Enabled
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
*/
typedef struct _SLQSOMADMSettingsReqParams3
{
    BYTE  FOTAdownload;
    BYTE  FOTAUpdate;
    BYTE  *pAutosdm;
    BYTE  *pFwAutoCheck;
} SLQSOMADMSettingsReqParams3;

/**
 * Sets the settings related to OMADM. These settings are saved on the modem
 * across power cycles.
 *
 *  \param SLQSOMADMSettingsReqParamsExt[IN]
 *         - See \ref SLQSOMADMSettingsReqParamsExt for more information
 *
 *  \note   Timeout: 20 seconds
 */
ULONG SLQSOMADMSetSettings3(
        SLQSOMADMSettingsReqParams3 *pSLQSOMADMSettingsReqParams3);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __GOBI_API_SWIOMADMS_H__ */
