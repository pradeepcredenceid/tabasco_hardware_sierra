/**
 * \ingroup swiaudio
 *
 * \file    qaGobiApiSwiAudio.h
 *
 * \brief   M2M Audio Service API function prototypes
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */
#ifndef __GOBI_API_SWI_AUDIO_H__
#define __GOBI_API_SWI_AUDIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_LEN_IFACE_TABLE 255

/**
 *  This structure contains the SLQSGetM2MAudioProfile request parameters.
 *
 *  \param  pGenerator[optional]
 *          - Generator
 *              - 0 - Voice
 *
 */
typedef struct
{
    BYTE *pGenerator;
} GetM2MAudioProfileReq;

/**
 *  This structure contains the SLQSGetM2MAudioProfile response parameters.
 *
 *  \param  Profile
 *          - Audio Profile
 *              - 0-5
 *
 *  \param  EarMute
 *          - Ear Mute
 *              - 0 - Mute
 *              - 1 - Unmute
 *
 *  \param  MicMute
 *          - MIC Mute
 *              - 0 - Mute
 *              - 1 - Unmute
 *
 *  \param  Generator
 *          - Generator
 *              - 0 - Voice
 *
 *  \param  Volume
 *          - RX volume level
 *              - 0-5
 *
 *  \param  CwtMute
 *          - Call waiting tone Mute
 *              - 0 - Mute
 *              - 1 - Unmute
 *
 */
typedef struct
{
    BYTE Profile;
    BYTE EarMute;
    BYTE MicMute;
    BYTE Generator;
    BYTE Volume;
    BYTE CwtMute;
} GetM2MAudioProfileResp;

/**
 *  This API gets the profile content.
 *
 *  \param  pGetM2MAudioProfileReq[OUT]
 *          - See \ref GetM2MAudioProfileReq for more information
 *
 *  \param  pGetM2MAudioProfileResp[OUT]
 *          - See \ref GetM2MAudioProfileResp for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 5 seconds\n
 *
 */
ULONG SLQSGetM2MAudioProfile (
    GetM2MAudioProfileReq  *pGetM2MAudioProfileReq,
    GetM2MAudioProfileResp *pGetM2MAudioProfileResp );

/**
 *  This structure contains the SLQSSetM2MAudioProfile request parameters.
 *
 *  \param  Profile
 *          - Audio Profile Number
 *              - 0-5
 *
 *  \param  pEarMute
 *          - Ear Mute
 *              - 0 - mute
 *              - 1 - unmute
 *
 *  \param  pMicMute
 *          - Mic Mute
 *              - 0 - mute
 *              - 1 - unmute
 *
 *  \param  pGenerator
 *          - Generator
 *              - 0 - voice
 *
 *  \param  pVolume
 *          - Set RX Volume level
 *              - 0-5
 *
 *  \param  pCwtMute
 *          - Call Waiting Tone Mute
 *              - 0 - Mute
 *              - 1 - UnMute
 */
typedef struct
{
    BYTE Profile;
    BYTE *pEarMute;
    BYTE *pMicMute;
    BYTE *pGenerator;
    BYTE *pVolume;
    BYTE *pCwtMute;
} SetM2MAudioProfileReq;

/**
 *  This API sets the profile content.
 *
 *  \param  pSetM2MAudioProfileReq[IN]
 *          - See \ref SetM2MAudioProfileReq for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 5 seconds\n
 *
 */
ULONG SLQSSetM2MAudioProfile (
    SetM2MAudioProfileReq *pSetM2MAudioProfileReq );

/**
 *  This structure contains the SLQSGetM2MAudioVolume request parameters.
 *
 *  \param  Profile
 *          - Audio Profile
 *              - 0-5
 *
 *  \param  Generator
 *          - Generator
 *              - 0 - Voice
 */
typedef struct
{
    BYTE Profile;
    BYTE Generator;
} GetM2MAudioVolumeReq;

/**
 *  This structure contains the SLQSGetM2MAudioVolume response parameters.
 *
 *  \param  Level
 *          - The RX Volume Level
 *              - 0-5
 *
 */
typedef struct
{
    BYTE Level;
} GetM2MAudioVolumeResp;

/**
 *  This API gets the Volume content.
 *
 *  \param  pGetM2MAudioVolumeReq[IN]
 *          - See \ref GetM2MAudioVolumeReq for more information
 *
 *  \param  pGetM2MAudioVolumeResp[OUT]
 *          - See \ref GetM2MAudioVolumeResp for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 5 seconds\n
 *
 */
ULONG SLQSGetM2MAudioVolume (
    GetM2MAudioVolumeReq  *pGetM2MAudioVolumeReq,
    GetM2MAudioVolumeResp *pGetM2MAudioVolumeResp );

/**
 *  This structure contains the SLQSSetM2MAudioProfile request parameters.
 *
 *  \param  Profile
 *          - Audio Profile Number
 *              - 0-5
 *
 *  \param  Generator
 *          - Generator
 *              - 0 - voice
 *
 *  \param  Level
 *          - Audio volume level
 *              - 0-5
 */
typedef struct
{
    BYTE Profile;
    BYTE Generator;
    BYTE Level;
} SetM2MAudioVolumeReq;

/**
 *  This API sets the Volume content.
 *
 *  \param  pSetM2MAudioVolumeReq[IN]
 *          - See \ref SetM2MAudioVolumeReq for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 5 seconds\n
 *
 */
ULONG SLQSSetM2MAudioVolume (
    SetM2MAudioVolumeReq *pSetM2MAudioVolumeReq );

/**
 *  This structure contains the PCM parameters.
 *
 *  \param  iFaceTabLen
 *          - Number of sets of iface table
 *
 *  \param  iFaceTab
 *          - Physical Interface Parameters
 *          - See \ref qaGobiApiTableSwiAudio.h for more information on
 *            physical interface parameters
 *
 */
typedef struct
{
    BYTE iFaceTabLen;
    BYTE iFaceTab[MAX_LEN_IFACE_TABLE];
}PCMparams;

/**
 *  This structure contains the SLQSSetM2MAudioAVCFG request parameters.
 *
 *  \param  Profile
 *          - Audio Profile
 *              - 0-5
 *
 *  \param  Device
 *          - ACDB Device
 *          - See \ref qaGobiApiTableSwiAudio.h for more information
 *            on ACDB Device
 *
 *  \param  PIFACEId
 *          - Physical Interface
 *          - See \ref qaGobiApiTableSwiAudio.h for more information
 *            on physical interface
 *
 *  \param  pPCMParams
 *          - PCM parameters
 *          - See \ref PCMparams for more information
 */
typedef struct
{
    BYTE      Profile;
    BYTE      Device;
    BYTE      PIFACEId;
    PCMparams *pPCMParams;
}SetM2MAudioAVCFGReq;

/**
 *  This API sets the AVCFG content.
 *
 *  \param  pSetM2MAudioAVCFGReq[IN]
 *          - See \ref SetM2MAudioAVCFGReq for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 5 seconds\n
 *
 */
ULONG SLQSSetM2MAudioAVCFG (
    SetM2MAudioAVCFGReq *pSetM2MAudioAVCFGReq );

/**
 *  This structure contains the SLQSSetM2MAudioLPBK request parameters.
 *
 *  \param  Enable
 *          - Operation to be performed
 *              - 0 - stop
 *              - 1 - VOCODER loop
 *              - 2 - internal codec loop

 *
 */
typedef struct
{
    BYTE Enable;
} SetM2MAudioLPBKReq;

/**
 *  This API sets the LPBK content.
 *
 *  \param  pSetM2MAudioLPBKReq[IN]
 *          - See \ref SetM2MAudioLPBKReq for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 5 seconds\n
 *
 */
ULONG SLQSSetM2MAudioLPBK (
    SetM2MAudioLPBKReq *pSetM2MAudioLPBKReq );

/**
 *  This API sets the NVDef content.
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 5 seconds\n
 *
 */
ULONG SLQSSetM2MAudioNVDef ( );

/**
 *  This structure contains the SLQSGetM2MSpkrGain request parameters.
 *
 *  \param  pProfile
 *          - Audio Profile Number
 *              - 0-5
 *
 */
typedef struct
{
    BYTE Profile;
} GetM2MSpkrGainReq;

/**
 *  This structure contains the SLQSGetM2MSpkrGain response parameters.
 *
 *  \param  Value
 *          - RX speakerphone gain
 *              - 0x0 - 0x7fff
 *
 */
typedef struct
{
    WORD Value;
} GetM2MSpkrGainResp;

/**
 *  This API Gets the SPKRGAIN content.
 *
 *  \param  pSpkrGainReq[IN]
 *          - See \ref GetM2MSpkrGainReq for more information
 *
 *  \param  pSpkrGainResp[OUT]
 *          - See \ref GetM2MSpkrGainResp for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 5 seconds\n
 *
 */
ULONG SLQSGetM2MSpkrGain (
    GetM2MSpkrGainReq  *pSpkrGainReq,
    GetM2MSpkrGainResp *pSpkrGainResp );

/**
 *  This structure contains the SLQSSetM2MSpkrGain request parameters.
 *
 *  \param  Profile
 *          - Audio Profile Number
 *              - 0-5
 *
 *  \param  Value
 *          - RX speakerphone gain
 *              - 0x0 - 0x7fff
 *
 */
typedef struct
{
    BYTE Profile;
    WORD Value;
} SetM2MSpkrGainReq;

/**
 *  This API Sets the SPKRGAIN content.
 *
 *  \param  pSpkrGainReq[IN]
 *          - See \ref GetM2MSpkrGainReq for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 5 seconds\n
 *
 */
ULONG SLQSSetM2MSpkrGain (
    SetM2MSpkrGainReq  *pSpkrGainReq );

/**
 *  This structure contains the SLQSGetM2MAVMute request parameters.
 *
 *  \param  Profile
 *          - Audio Profile Number
 *              - 0-5
 *
 */
typedef struct
{
    BYTE Profile;
} GetM2MAVMuteReq;

/**
 *  This structure contains the SLQSGetM2MAVMute response parameters.
 *
 *  \param  pEarMute
 *          - Ear Mute
 *              - 0-Mute
 *              - 1-UnMute
 *
 *  \param  pMicMute
 *          - Mic Mute
 *              - 0-Mute
 *              - 1-unmute
 *
 *  \param  CwtMute
 *          - Waiting tone Mute
 *              - 0-5
 */
typedef struct
{
    BYTE EarMute;
    BYTE MicMute;
    BYTE CwtMute;
} GetM2MAVMuteResp;

/**
 *  This API Gets the AV Mute content.
 *
 *  \param  pGetM2MAVMuteReq[IN]
 *          - See \ref GetM2MAVMuteReq for more information
 *
 *  \param  pGetM2MAVMuteResp[OUT]
 *          - See \ref GetM2MAVMuteResp for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 5 seconds\n
 *
 */
ULONG SLQSGetM2MAVMute (
    GetM2MAVMuteReq  *pGetM2MAVMuteReq,
    GetM2MAVMuteResp *pGetM2MAVMuteResp );

/**
 *  This structure contains the SLQSSetM2MAVMute request parameters.
 *
 *  \param  Profile
 *          - Audio Profile Number
 *              - 0-5
 *
 *  \param  EarMute
 *          - Ear Mute
 *              - 0-1
 *
 *  \param  MicMute
 *          - Mic Mute
 *              - 0-1
 *
 *  \param  pCwtMute [ Optional ]
 *          - Call Waiting Tone Mute
 *              - 0-1
 */
typedef struct
{
    BYTE Profile;
    BYTE EarMute;
    BYTE MicMute;
    BYTE *pCwtMute;
} SetM2MAVMuteReq;

/**
 *  This API Sets the AV Mute content.
 *
 *  \param  pSetM2MAVMuteReq[IN]
 *          - See \ref SetM2MAVMuteReq for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 5 seconds\n
 *
 */
ULONG SLQSSetM2MAVMute ( SetM2MAVMuteReq *pSetM2MAVMuteReq );

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __GOBI_API_SWI_AUDIO_H__ */
