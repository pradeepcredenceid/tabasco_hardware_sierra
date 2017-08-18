/**
 * \ingroup ims
 *
 * \file    qaGobiApiIms.h
 *
 * \brief   IMS Service API function prototypes
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */
#ifndef __GOBI_API_IMS_H__
#define __GOBI_API_IMS_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  This structure contains the SLQSSetSIPConfig request parameters.
 *
 *  \param  pSIPLocalPort
 *          - Primary call session control function SIP port number
 *
 *  \param  pTimerSIPReg
 *          - Initial SIP registration duration from the User equipment,
 *            in seconds
 *
 *  \param  pSubscribeTimer
 *          - Duration of the subscription by the UE for IMS registration
 *            notifications, in seconds
 *
 *  \param  pTimerT1
 *          - RTT estimate, in milliseconds
 *
 *  \param  pTimerT2
 *          - The maximum retransmit interval for non-invite requests and
 *            invite responses, in milliseconds
 *
 *  \param  pTimerTf
 *          - Non-invite transaction timeout timer, in milliseconds
 *
 *  \param  pSigCompEnabled
 *          - Sig Comp Status
 *              - TRUE - Enable
 *              - FALSE - Disable
 *
 */
typedef struct
{
    WORD  *pSIPLocalPort;
    ULONG *pTimerSIPReg;
    ULONG *pSubscribeTimer;
    ULONG *pTimerT1;
    ULONG *pTimerT2;
    ULONG *pTimerTf;
    BYTE  *pSigCompEnabled;
} SetSIPConfigReq;

/**
 *  This structure contains the SLQSSetSIPConfig response parameters.
 *
 *  \param  pSettingResp
 *          - Settings standard response type. A settings specific error
 *            code is returned when the standard response error type is
 *            QMI_ERR_CAUSE_CODE
 *
 */
typedef struct
{
    BYTE *pSettingResp;
} SetSIPConfigResp;

/**
 *  This API sets the IMS Session Initiation Protocol(SIP) configuration
 *  parameters for the requesting control point.
 *
 *  \param  pSetSIPConfigReq[IN]
 *          - See \ref SetSIPConfigReq for more information
 *
 *  \param  pSetSIPConfigResp[OUT]
 *          - See \ref SetSIPConfigResp for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Technology Supported: NA\n
 *          Device Supported: MC7800\n
 *          Timeout: 5 seconds\n
 *
 */
ULONG SLQSSetSIPConfig (
    SetSIPConfigReq  *pSetSIPConfigReq,
    SetSIPConfigResp *pSetSIPConfigResp );

/**
 *  This structure contains the SLQSSetRegMgrConfig request parameters.
 *
 *  \param  pPriCSCFPort
 *          - Primary call session control function port
 *
 *  \param  pCSCFPortNameLen
 *          - Length of the CSCF Port name parameter to follow
 *
 *  \param  pCSCFPortName
 *          - Call Session control port, fully qualified domain name
 *          - Length of this string must be specified in
 *            pCSCFPortNameLen parameter
 *
 *  \param  pIMSTestMode
 *          - IMS Test mode Enabled.
 *              - TRUE - Enable, no IMS registration
 *              - FALSE - Disable, IMS registration is initiated
 *
 */
typedef struct
{
    WORD *pPriCSCFPort;
    BYTE *pCSCFPortNameLen;
    BYTE *pCSCFPortName;
    BYTE *pIMSTestMode;
} SetRegMgrConfigReq;

/**
 *  This structure contains the SLQSSetRegMgrConfig response parameters.
 *
 *  \param  pSettingResp
 *          - Settings standard response type. A settings specific error
 *            code is returned when the standard response error type is
 *            QMI_ERR_CAUSE_CODE
 *
 */
typedef struct
{
    BYTE *pSettingResp;
} SetRegMgrConfigResp;

/**
 *  This API sets the IMS registration manager configuration
 *  parameters for the requesting control point.
 *
 *  \param  pSetRegMgrConfigReq[IN]
 *          - See \ref SetRegMgrConfigReq for more information
 *
 *  \param  pSetRegMgrConfigResp[OUT]
 *          - See \ref SetRegMgrConfigResp for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Technology Supported: NA\n
 *          Device Supported: MC7800\n
 *          Timeout: 5 seconds\n
 *
 */
ULONG SLQSSetRegMgrConfig (
    SetRegMgrConfigReq  *pSetRegMgrConfigReq,
    SetRegMgrConfigResp *pSetRegMgrConfigResp );

/**
 *  This structure contains the SLQSSetIMSSMSConfig request parameters.
 *
 *  \param  pSMSFormat
 *          - SMS format
 *              - 0 - 3GPP
 *              - 1 - 3GPP2
 *
 *  \param  pSMSOverIPNwInd
 *          - SMS over IP Network Indication Flag
 *              - TRUE - Turn on mobile-originated SMS
 *              - FALSE - Turn off mobile-originated SMS
 *
 *  \param  pPhoneCtxtURILen
 *          - Length of Phone context Universal Resource Identifier to follow
 *
 *  \param  pPhoneCtxtURI
 *          - Phone context universal resource identifier
 *          - Length of this string must be specified in
 *            pPhoneCtxtURILen parameter
 *
 */
typedef struct
{
    BYTE *pSMSFormat;
    BYTE *pSMSOverIPNwInd;
    BYTE *pPhoneCtxtURILen;
    BYTE *pPhoneCtxtURI;
} SetIMSSMSConfigReq;

/**
 *  This structure contains the SLQSSetIMSSMSConfig response parameters.
 *
 *  \param  pSettingResp
 *          - Settings standard response type. A settings specific error
 *            code is returned when the standard response error type is
 *            QMI_ERR_CAUSE_CODE
 *
 */
typedef struct
{
    BYTE *pSettingResp;
} SetIMSSMSConfigResp;

/**
 *  This API sets the IMS SMS configuration
 *  parameters for the requesting control point.
 *
 *  \param  pSetIMSSMSConfigReq[IN]
 *          - See \ref SetIMSSMSConfigReq for more information
 *
 *  \param  pSetIMSSMSConfigResp[OUT]
 *          - See \ref SetIMSSMSConfigResp for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Technology Supported: NA\n
 *          Device Supported: MC7800\n
 *          Timeout: 5 seconds\n
 *
 */
ULONG SLQSSetIMSSMSConfig (
    SetIMSSMSConfigReq  *pSetIMSSMSConfigReq,
    SetIMSSMSConfigResp *pSetIMSSMSConfigResp );

/**
 *  This structure contains the SLQSSetIMSUserConfig request parameters.
 *
 *  \param  pIMSDomainLen
 *          - Length of IMS Domain Name to follow
 *
 *  \param  pIMSDomain
 *          - IMS domain name
 *
 */
typedef struct
{
    BYTE *pIMSDomainLen;
    BYTE *pIMSDomain;
} SetIMSUserConfigReq;

/**
 *  This structure contains the SLQSSetIMSUserConfig response parameters.
 *
 *  \param  pSettingResp
 *          - Settings standard response type. A settings specific error
 *            code is returned when the standard response error type is
 *            QMI_ERR_CAUSE_CODE
 *
 */
typedef struct
{
    BYTE *pSettingResp;
} SetIMSUserConfigResp;

/**
 *  This API sets the IMS user configuration
 *  parameters for the requesting control point.
 *
 *  \param  pSetIMSUserConfigReq[IN]
 *          - See \ref SetIMSUserConfigReq for more information
 *
 *  \param  pSetIMSUserConfigResp[OUT]
 *          - See \ref SetIMSUserConfigResp for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Technology Supported: NA\n
 *          Device Supported: MC7800\n
 *          Timeout: 5 seconds\n
 *
 */
ULONG SLQSSetIMSUserConfig (
    SetIMSUserConfigReq  *pSetIMSUserConfigReq,
    SetIMSUserConfigResp *pSetIMSUserConfigResp );

/**
 *  This structure contains the SLQSSetIMSVoIPConfig request parameters.
 *
 *  \param  pSessionExpiryTimer
 *          - Session duration, in seconds
 *
 *  \param  pMinSessionExpiryTimer
 *          - Minimum allowed value for session expiry timer,
 *            in seconds
 *
 *  \param  pAmrWbEnable
 *          - Flag to enable/disable Adaptive Multirate Codec(AMR)
 *            WideBand(WB) audio
 *          - Values:
 *              - True  - Enable
 *              - False - Disable
 *
 *  \param  pScrAmrEnable
 *          - Flag to enable/disable Source Control Rate(SCR)
 *            for AMR NarrowBand (NB)
 *          - Values:
 *              - True  - Enable
 *              - False - Disable
 *
 *  \param  pScrAmrWbEnable
 *          - Flag to enable/disable SCR for AMR WB Audio
 *          - Values:
 *              - True  - Enable
 *              - False - Disable
 *
 *  \param  pAmrMode
 *          - BitMask for AMR NB modes allowed
 *          - Values:
 *              - 0x1 - 4.75 kbps
 *              - 0x2 - 5.15 kbps
 *              - 0x4 - 5.9 kbps
 *              - 0x8 - 6.17 kbps
 *              - 0x10 - 7.4 kbps
 *              - 0x20 - 7.95 kbps
 *              - 0x40 - 10.2 kbps
 *              - 0x80 - 12.2 kbps
 *
 *  \param  pAmrWBMode
 *          - BitMask for AMR WB modes allowed
 *          - Values:
 *              - 0x1 - 6.60 kbps
 *              - 0x2 - 8.85 kbps
 *              - 0x4 - 12.65 kbps
 *              - 0x8 - 14.25 kbps
 *              - 0x10 - 15.85 kbps
 *              - 0x20 - 18.25 kbps
 *              - 0x40 - 19.85 kbps
 *              - 0x80 - 23.05 kbps
 *              - 0x100 - 23.85 kbps
 *
 *  \param  pAmrOctetAligned
 *          - Flag to indicate if the octet is aligned for AMR NB Audio
 *          - Values:
 *              - True  - Aligned
 *              - False - Not aligned, Bandwidth Efficient mode
 *
 *  \param  pAmrWBOctetAligned
 *          - Flag to indicate if the octet is aligned for AMR WB Audio
 *          - Values:
 *              - True  - Aligned
 *              - False - Not aligned, Bandwidth Efficient mode
 *
 *  \param  pRingingTimer
 *          - Duration of ringing timer, in seconds. The ringing timer starts
 *            on the ringing event. If the call is not answered within the
 *            duration of this timer, the call is disconnected.
 *
 *  \param  pRingBackTimer
 *          - Duration of ringback timer, in seconds. The ringback timer starts
 *            on the ringback event. If the call is not answered within the
 *            duration of this timer, the call is disconnected.
 *
 *  \param  pRTPRTCPInactTimer
 *          - Duration of RTP/RTCP inactivity timer, in seconds. If no
 *            RTP/RTCP packet is received prior to the expiry
 *            of this timer, the call is disconnected.
 */
typedef struct
{
    WORD *pSessionExpiryTimer;
    WORD *pMinSessionExpiryTimer;
    BYTE *pAmrWbEnable;
    BYTE *pScrAmrEnable;
    BYTE *pScrAmrWbEnable;
    BYTE *pAmrMode;
    WORD *pAmrWBMode;
    BYTE *pAmrOctetAligned;
    BYTE *pAmrWBOctetAligned;
    WORD *pRingingTimer;
    WORD *pRingBackTimer;
    WORD *pRTPRTCPInactTimer;
} SetIMSVoIPConfigReq;

/**
 *  This structure contains the SLQSSetIMSVoIPConfig response parameters.
 *
 *  \param  pSettingResp
 *          - Settings standard response type. A settings specific error
 *            code is returned when the standard response error type is
 *            QMI_ERR_CAUSE_CODE
 *
 */
typedef struct
{
    BYTE *pSettingResp;
} SetIMSVoIPConfigResp;

/**
 *  This API sets the IMS Voice over Internet Protocol (VoIP) configuration
 *  parameters for the requesting control point.
 *
 *  \param  pSetIMSVoIPConfigReq[IN]
 *          - See \ref SetIMSVoIPConfigReq for more information
 *
 *  \param  pSetIMSVoIPConfigResp[OUT]
 *          - See \ref SetIMSVoIPConfigResp for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Technology Supported: NA\n
 *          Device Supported: MC7800\n
 *          Timeout: 5 seconds\n
 *
 */
ULONG SLQSSetIMSVoIPConfig (
    SetIMSVoIPConfigReq  *pSetIMSVoIPConfigReq,
    SetIMSVoIPConfigResp *pSetIMSVoIPConfigResp );

/**
 *  This structure contains the SLQSGetSIPConfig response parameters.
 *
 *  \param  pSettingResp
 *          - Settings Response
 *
 *  \param  pSIPLocalPort
 *          - Primary call session control function SIP port number
 *
 *  \param  pTimerSIPReg
 *          - Initial SIP registration duration from the User equipment,
 *            in seconds
 *
 *  \param  pSubscribeTimer
 *          - Duration of the subscription by the UE for IMS registration
 *            notifications, in seconds
 *
 *  \param  pTimerT1
 *          - RTT estimate, in milliseconds
 *
 *  \param  pTimerT2
 *          - The maximum retransmit interval for non-invite requests and
 *            invite responses, in milliseconds
 *
 *  \param  pTimerTf
 *          - Non-invite transaction timeout timer, in milliseconds
 *
 *  \param  pSigCompEnabled
 *          - Sig Comp Status
 *              - TRUE - Sig Comp Enabled
 *              - FALSE - Sig Comp Disabled
 *
 */
typedef struct
{
    BYTE  *pSettingResp;
    WORD  *pSIPLocalPort;
    ULONG *pTimerSIPReg;
    ULONG *pSubscribeTimer;
    ULONG *pTimerT1;
    ULONG *pTimerT2;
    ULONG *pTimerTf;
    BYTE  *pSigCompEnabled;
} GetSIPConfigResp;

/**
 *  This API retrieves the Session Initiation Protocol(SIP) configuration
 *  parameters.
 *
 *  \param  pGetSIPConfigResp[OUT]
 *          - See \ref GetSIPConfigResp for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Technology Supported: NA\n
 *          Device Supported: MC7800\n
 *          Timeout: 5 seconds\n
 *
 */
ULONG SLQSGetSIPConfig (
    GetSIPConfigResp *pGetSIPConfigResp );

/**
 *  This structure contains the SLQSGetRegMgrConfig response parameters.
 *
 *  \param  pSettingResp
 *          - Settings Response
 *
 *  \param  pPCSCFPort
 *          - Proxy call session control function port
 *
 *  \param  pPriCSCFPortNameLen(IN/OUT)
 *          - Size in bytes assigned to the primary CSCF Port name
 *            parameter to follow
 *
 *  \param  pPriCSCFPortName
 *          - Call Session control port, fully qualified domain name
 *          - Length of this string must be specified in
 *            pPriCSCFPortNameLen parameter
 *
 *  \param  pIMSTestMode
 *          - IMS Test mode Enabled.
 *              - TRUE - Enabled
 *              - FALSE - Disabled
 *
 *  \note   pPriCSCFPortNameLen must be set to a valid value during API call
 *          to retrieve pPriCSCFPortName.
 */
typedef struct
{
    BYTE *pSettingResp;
    WORD *pPCSCFPort;
    BYTE *pPriCSCFPortNameLen;
    BYTE *pPriCSCFPortName;
    BYTE *pIMSTestMode;
} GetRegMgrConfigParams;

/**
 *  This API retrieves the registration manager configuration
 *  parameters.
 *
 *  \param  pGetRegMgrConfigParams[IN/OUT]
 *          - See \ref GetRegMgrConfigResp for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Technology Supported: NA\n
 *          Device Supported: MC7800\n
 *          Timeout: 5 seconds\n
 *
 */
ULONG SLQSGetRegMgrConfig (
    GetRegMgrConfigParams *pGetRegMgrConfigParams );

/**
 *  This structure contains the SLQSGetIMSSMSConfig response parameters.
 *
 *  \param  pSettingResp
 *          - Settings Response
 *
 *  \param  pSMSFormat
 *          - SMS format
 *              - 0 - 3GPP
 *              - 1 - 3GPP2
 *
 *  \param  pSMSOverIPNwInd
 *          - SMS over IP Network Indication Flag
 *              - TRUE - Turn on mobile-originated SMS
 *              - FALSE - Turn off mobile-originated SMS
 *
 *  \param  pPhoneCtxtURILen
 *          - Size in bytes assigned to the Phone context Universal Resource
 *            Identifier to follow
 *
 *  \param  pPhoneCtxtURI
 *          - Phone context universal resource identifier
 *          - Length of this string must be specified in
 *            pPhoneCtxtURILen parameter
 *
 */
typedef struct
{
    BYTE *pSettingResp;
    BYTE *pSMSFormat;
    BYTE *pSMSOverIPNwInd;
    BYTE *pPhoneCtxtURILen;
    BYTE *pPhoneCtxtURI;
} GetIMSSMSConfigParams;

/**
 *  This API retrieves the SMS configuration parameters.
 *
 *  \param  pGetIMSSMSConfigParams[IN/OUT]
 *          - See \ref GetIMSSMSConfigParams for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Technology Supported: NA\n
 *          Device Supported: MC7800\n
 *          Timeout: 5 seconds\n
 *
 */
ULONG SLQSGetIMSSMSConfig (
    GetIMSSMSConfigParams *pGetIMSSMSConfigParams );

/**
 *  This structure contains the SLQSGetIMSUserConfig response parameters.
 *
 *  \param  pSettingResp
 *          - Settings Response
 *
 *  \param  pIMSDomainLen
 *          - Length of IMS Domain Name to follow
 *
 *  \param  pIMSDomain
 *          - IMS domain name
 *          - Length of this string must be specified in
 *            pIMSDomainLen parameter
 *
 */
typedef struct
{
    BYTE *pSettingResp;
    BYTE *pIMSDomainLen;
    BYTE *pIMSDomain;
} GetIMSUserConfigParams;

/**
 *  This API retrieves the IMS User configuration parameters.
 *
 *  \param  pGetIMSUserConfigParams[IN/OUT]
 *          - See \ref GetIMSUserConfigParams for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Technology Supported: NA\n
 *          Device Supported: MC7800\n
 *          Timeout: 5 seconds\n
 *
 */
ULONG SLQSGetIMSUserConfig (
    GetIMSUserConfigParams *pGetIMSUserConfigParams );

/**
 *  This structure contains the SLQSGetIMSVoIPConfig request parameters.
 *
 *  \param  pSettingResp
 *          - Settings Response. A settings specific error
 *            code is returned when the standard response error type is
 *            QMI_ERR_CAUSE_CODE
 *
 *  \param  pSessionExpiryTimer
 *          - Session duration, in seconds
 *
 *  \param  pMinSessionExpiryTimer
 *          - Minimum allowed value for session expiry timer,
 *            in seconds
 *
 *  \param  pAmrWbEnable
 *          - Flag to enable/disable Adaptive Multirate Codec(AMR)
 *            WideBand(WB) audio
 *          - Values:
 *              - True  - Enable
 *              - False - Disable
 *
 *  \param  pScrAmrEnable
 *          - Flag to enable/disable Source Control Rate(SCR)
 *            for AMR NarrowBand (NB)
 *          - Values:
 *              - True  - Enable
 *              - False - Disable
 *
 *  \param  pScrAmrWbEnable
 *          - Flag to enable/disable SCR for AMR WB Audio
 *          - Values:
 *              - True  - Enable
 *              - False - Disable
 *
 *  \param  pAmrMode
 *          - BitMask for AMR NB modes allowed
 *          - Values:
 *              - 0x1 - 4.75 kbps
 *              - 0x2 - 5.15 kbps
 *              - 0x4 - 5.9 kbps
 *              - 0x8 - 6.17 kbps
 *              - 0x10 - 7.4 kbps
 *              - 0x20 - 7.95 kbps
 *              - 0x40 - 10.2 kbps
 *              - 0x80 - 12.2 kbps
 *
 *  \param  pAmrWBMode
 *          - BitMask for AMR WB modes allowed
 *          - Values:
 *              - 0x1 - 6.60 kbps
 *              - 0x2 - 8.85 kbps
 *              - 0x4 - 12.65 kbps
 *              - 0x8 - 14.25 kbps
 *              - 0x10 - 15.85 kbps
 *              - 0x20 - 18.25 kbps
 *              - 0x40 - 19.85 kbps
 *              - 0x80 - 23.05 kbps
 *              - 0x100 - 23.85 kbps
 *
 *  \param  pAmrOctetAligned
 *          - Flag to indicate if the octet is aligned for AMR NB Audio
 *          - Values:
 *              - True  - Aligned
 *              - False - Not aligned, Bandwidth Efficient mode
 *
 *  \param  pAmrWBOctetAligned
 *          - Flag to indicate if the octet is aligned for AMR WB Audio
 *          - Values:
 *              - True  - Aligned
 *              - False - Not aligned, Bandwidth Efficient mode
 *
 *  \param  pRingingTimer
 *          - Duration of ringing timer, in seconds. The ringing timer starts
 *            on the ringing event. If the call is not answered within the
 *            duration of this timer, the call is disconnected.
 *
 *  \param  pRingBackTimer
 *          - Duration of ringback timer, in seconds. The ringback timer starts
 *            on the ringback event. If the call is not answered within the
 *            duration of this timer, the call is disconnected.
 *
 *  \param  pRTPRTCPInactTimer
 *          - Duration of RTP/RTCP inactivity timer, in seconds. If no
 *            RTP/RTCP packet is received prior to the expiry
 *            of this timer, the call is disconnected.
 */
typedef struct
{
    BYTE *pSettingResp;
    WORD *pSessionExpiryTimer;
    WORD *pMinSessionExpiryTimer;
    BYTE *pAmrWbEnable;
    BYTE *pScrAmrEnable;
    BYTE *pScrAmrWbEnable;
    BYTE *pAmrMode;
    WORD *pAmrWBMode;
    BYTE *pAmrOctetAligned;
    BYTE *pAmrWBOctetAligned;
    WORD *pRingingTimer;
    WORD *pRingBackTimer;
    WORD *pRTPRTCPInactTimer;
} GetIMSVoIPConfigResp;

/**
 *  This API retrieves the IMS VoIP configuration parameters.
 *
 *  \param  GetIMSVoIPConfigResp[OUT]
 *          - See \ref GetIMSVoIPConfigResp for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Technology Supported: NA\n
 *          Device Supported: MC7800\n
 *          Timeout: 5 seconds\n
 *
 */
ULONG SLQSGetIMSVoIPConfig (
    GetIMSVoIPConfigResp *pGetIMSVoIPConfigResp );

/**
 *  This structure contains parameters of IMS Config Indication Register
 *
 *  \param  pSIPConfigEvents(optional)
 *          - Registration Indication For SIP Configuration Events.
 *          - When this registration is enabled, the device learns of SIP
 *            config events via the QMI_IMS_SIP_CONFIG_IND indication.
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  \param  pRegMgrConfigEvents(optional)
 *          - Registration Indication For Registration Manager Configuration
 *            Events.
 *          - When this registration is enabled, the device learns of Reg Mgr
 *            config events via the QMI_IMS_REG_MGR_CONFIG_IND indication.
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  \param  pSMSConfigEvents(optional)
 *          - Registration Indication For SMS Configuration Events.
 *          - When this registration is enabled, the device learns of SMS
 *            config events via the QMI_IMS_SMS_CONFIG_IND indication.
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  \param  pUserConfigEvents(optional)
 *          - Registration Indication For User Configuration Events.
 *          - When this registration is enabled, the device learns of user
 *            config events via the QMI_IMS_USER_CONFIG_IND indication.
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  \param  pVoIPConfigEvents(optional)
 *          - Registration Indication For VoIP Configuration Events.
 *          - When this registration is enabled, the device learns of VOIP
 *            config events via the QMI_IMS_VOIP_CONFIG_IND indication.
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  \note   One of the optional parameter is mandatory to be present in the
 *          request.
 */
typedef struct
{
    BYTE *pSIPConfigEvents;
    BYTE *pRegMgrConfigEvents;
    BYTE *pSMSConfigEvents;
    BYTE *pUserConfigEvents;
    BYTE *pVoIPConfigEvents;
} imsCfgIndRegisterInfo;

/**
 *  Sets the registration state for different QMI_IMS indications for the
 *  requesting control point
 *
 *  \param  pImsCfgIndRegisterInfo[IN]
 *          - Structure containing Indication Register Information.
 *              - See \ref imsCfgIndRegisterInfo for more informtaion.
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Technology Supported: UMTS\n
 *          Device Supported: MC7800\n
 *          Timeout: 10 Secs
 *
 *          This API is used by a device to register/deregister for different
 *          QMI IMS indications.
 *          The device's registration state variables that control registration
 *          for indications will be modified to reflect the settings indicated in the
 *          request message.
 *          At least one optional parameter must be present in the request.
 *
 */
ULONG SLQSImsConfigIndicationRegister(
    imsCfgIndRegisterInfo *pImsCfgIndRegisterInfo );

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __GOBI_API_IMS_H__ */
