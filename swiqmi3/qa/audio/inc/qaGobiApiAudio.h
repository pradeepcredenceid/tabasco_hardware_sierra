/**
 * \ingroup audio
 *
 * \file    qaGobiApiAudio.h
 *
 * \brief   Audio Service API function prototypes
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */
#ifndef __GOBI_API_AUDIO_H__
#define __GOBI_API_AUDIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This structure contains the SLQSGetAudioProfile request parameters
 *
 *  \param  Generator
 *          - Audio Generator
 *              - 0 - Voice
 *              - 1 - Key Beep
 *              - 2 - MIDI
 *
 */
typedef struct
{
    BYTE Generator;
} GetAudioProfileReq;

/**
 *  This structure contains the SLQSGetAudioProfile response parameters.
 *
 *  \param  Profile
 *          - Audio Profile
 *              - 0 - Handset
 *              - 1 - Headset
 *              - 2 - Car Kit
 *              - 3 - Speaker phone
 *              - 4 - Auxiliary
 *              - 5 - TTY
 *              - 6 - Auxiliary external PCM
 *              - 7 - Primary external PCM
 *              - 8 - External slave PCM
 *              - 9 - I2S
 *
 *  \param  EarMute
 *          - Ear Mute Setting
 *              - 0 - unmuted
 *              - 1 - muted
 *
 *  \param  MicMute
 *          - MIC Mute Setting
 *              - 0 - unmuted
 *              - 1 - muted
 *
 *  \param  Volume
 *          - Audio Volume Level
 *              - 0 to 7
 *
 */
typedef struct
{
    BYTE Profile;
    BYTE EarMute;
    BYTE MicMute;
    BYTE Volume;
} GetAudioProfileResp;

/**
 *  This API get the profile content of the requested audio generator.
 *
 *  \param  pGetAudioProfileReq[IN]
 *          - See \ref GetAudioProfileReq for more information
 *
 *  \param  pGetAudioProfileResp[OUT]
 *          - See \ref GetAudioProfileResp for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Device Supported: SL9090\n
 *          Timeout: 5 seconds\n
 *
 */
ULONG SLQSGetAudioProfile (
    GetAudioProfileReq  *pGetAudioProfileReq,
    GetAudioProfileResp *pGetAudioProfileResp );

/**
 *  This structure contains the SLQSSetAudioProfile request parameters.
 *
 *  \param  Profile
 *          - Audio Profile
 *              - 0 - Handset
 *              - 1 - Headset
 *              - 2 - Car Kit
 *              - 3 - Speaker phone
 *              - 4 - Auxiliary
 *              - 5 - TTY
 *              - 6 - Auxiliary external PCM
 *              - 7 - Primary external PCM
 *              - 8 - External slave PCM
 *              - 9 - I2S
 *
 *  \param  EarMute
 *          - Ear Mute Setting
 *              - 0 - unmuted
 *              - 1 - muted
 *
 *  \param  MicMute
 *          - MIC Mute Setting
 *              - 0 - unmuted
 *              - 1 - muted
 *
 *  \param  Generator
 *          - Audio Generator
 *              - 0 - Voice
 *              - 1 - Key Beep
 *              - 2 - MIDI
 *
 *  \param  Volume
 *          - Audio Volume Level
 *              - 0 to 7
 *
 */
typedef struct
{
    BYTE Profile;
    BYTE EarMute;
    BYTE MicMute;
    BYTE Generator;
    BYTE Volume;
} SetAudioProfileReq;

/**
 *  This API sets an audio profile.
 *
 *  \param  pSetAudioProfileReq[IN]
 *          - See \ref SetAudioProfileReq for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Device Supported: SL9090\n
 *          Timeout: 5 seconds\n
 *
 */
ULONG SLQSSetAudioProfile (
    SetAudioProfileReq  *pSetAudioProfileReq );

/**
 * This structure contains the SLQSGetAudioPathConfig request parameters
 *
 *  \param  Profile
 *          - Audio Profile
 *              - 0-9
 *
 *  \param  Item
 *          - Item
 *              - 0 - AV_EC
 *              - 1 - AV_NS
 *              - 2 - AV_TXVOL
 *              - 3 - AV_DTMFTXG
 *              - 4 - AV_CODECSTG
 *              - 5 - AV_TXPCMIIRFLTR
 *              - 6 - AV_RXPCMIIRFLTR
 *              - 7 - AV_MICGAIN
 *              - 8 - AV_RXAGC
 *              - 9 - AV_TXAGC
 *              - 10 - AV_RXAGCLIST
 *              - 11 - AV_RXAVCLIST
 *              - 12 - AV_TXAGCLIST
 */
typedef struct
{
    BYTE Profile;
    BYTE Item;
} GetAudioPathConfigReq;

/**
 * This structure contains the SLQSGetAudioPathConfig parameters related to
 * AV_TXPCMIIRFLTR.
 *
 *  \param  pFlag
 *          - Flag
 *              - 0x0000 - IIR filter disable
 *              - 0xffff - IIR filter enable
 *
 *  \param  pStageCnt
 *          - Stage Count
 *              - 0-4
 *
 *  \param  pStage0Val
 *          - A 20 BYTE sized parameter indicating Stage 0 value
 *              - A1
 *              - A2
 *              - B0
 *              - B1
 *              - B2
 *
 *  \param  pStage1Val
 *          - A 20 BYTE sized parameter indicating Stage 1 value
 *              - A1
 *              - A2
 *              - B0
 *              - B1
 *              - B2
 *
 *  \param  pStage2Val
 *          - A 20 BYTE sized parameter indicating Stage 2 value
 *              - A1
 *              - A2
 *              - B0
 *              - B1
 *              - B2
 *
 *  \param  pStage3Val
 *          - A 20 BYTE sized parameter indicating Stage 3 value
 *              - A1
 *              - A2
 *              - B0
 *              - B1
 *              - B2
 *
 *  \param  pStage4Val
 *          - A 20 BYTE sized parameter indicating Stage 4 value
 *              - A1
 *              - A2
 *              - B0
 *              - B1
 *              - B2
 */
typedef struct
{
    WORD *pFlag;
    WORD *pStageCnt;
    BYTE *pStage0Val;
    BYTE *pStage1Val;
    BYTE *pStage2Val;
    BYTE *pStage3Val;
    BYTE *pStage4Val;
} TXPCMIIRFltr;

/**
 * This structure contains the SLQSGetAudioPathConfig parameters related to
 * AV_RXPCMIIRFLTR.
 *
 *  \param  pFlag
 *          - Flag
 *              - 0x0000 - IIR filter disable
 *              - 0xffff - IIR filter enable
 *
 *  \param  pStageCnt
 *          - Stage Count
 *              - 0-4
 *
 *  \param  pStage0Val
 *          - A 20 BYTE sized parameter indicating Stage 0 value
 *              - A1
 *              - A2
 *              - B0
 *              - B1
 *              - B2
 *
 *  \param  pStage1Val
 *          - A 20 BYTE sized parameter indicating Stage 1 value
 *              - A1
 *              - A2
 *              - B0
 *              - B1
 *              - B2
 *
 *  \param  pStage2Val
 *          - A 20 BYTE sized parameter indicating Stage 2 value
 *              - A1
 *              - A2
 *              - B0
 *              - B1
 *              - B2
 *
 *  \param  pStage3Val
 *          - A 20 BYTE sized parameter indicating Stage 3 value
 *              - A1
 *              - A2
 *              - B0
 *              - B1
 *              - B2
 *
 *  \param  pStage4Val
 *          - A 20 BYTE sized parameter indicating Stage 4 value
 *              - A1
 *              - A2
 *              - B0
 *              - B1
 *              - B2
 */
typedef struct
{
    WORD *pFlag;
    WORD *pStageCnt;
    BYTE *pStage0Val;
    BYTE *pStage1Val;
    BYTE *pStage2Val;
    BYTE *pStage3Val;
    BYTE *pStage4Val;
} RXPCMIIRFltr;

/**
 * This structure contains the SLQSGetAudioPathConfig parameters related to
 * AV_RXAGCLIST.
 *
 *  \param  pRXStaticGain
 *          - RX pre-compressor static gain
 *
 *  \param  pRXAIG
 *          - RX pre-compressor gain selection flag
 *
 *  \param  pRXExpThres
 *          - RX expansion threshold
 *
 *  \param  pRXExpSlope
 *          - RX expansion slope
 *
 *  \param  pRXComprThres
 *          - RX compression threshold
 *
 *  \param  pRXComprSlope
 *          - RX compression slope
 */
typedef struct
{
    WORD *pRXStaticGain;
    WORD *pRXAIG;
    WORD *pRXExpThres;
    WORD *pRXExpSlope;
    WORD *pRXComprThres;
    WORD *pRXComprSlope;
} RXAGCList;

/**
 * This structure contains the SLQSGetAudioPathConfig parameters related to
 * AV_RXAVCLIST.
 *
 *  \param  pAVRXAVCSens
 *          - AVC variation from nominal sensitivity
 *
 *  \param  pAVRXAVCHeadroom
 *          - AVC headroom
 *
 */
typedef struct
{
    WORD *pAVRXAVCSens;
    WORD *pAVRXAVCHeadroom;
} RXAVCList;

/**
 * This structure contains the SLQSGetAudioPathConfig parameters related to
 * AV_TXAGCLIST.
 *
 *  \param  pTXStaticGain
 *          - TX pre-compressor static gain
 *
 *  \param  pTXAIG
 *          - TX pre-compressor gain selection flag
 *
 *  \param  pTXExpThres
 *          - TX expansion threshold
 *
 *  \param  pTXExpSlope
 *          - TX expansion slope
 *
 *  \param  pTXComprThres
 *          - TX compression threshold
 *
 *  \param  pTXComprSlope
 *          - TX compression slope
 */
typedef struct
{
    WORD *pTXStaticGain;
    WORD *pTXAIG;
    WORD *pTXExpThres;
    WORD *pTXExpSlope;
    WORD *pTXComprThres;
    WORD *pTXComprSlope;
} TXAGCList;

/**
 *  This structure contains the SLQSGetAudioPathConfig response parameters.
 *
 *  \param  pECMode [Optional]
 *          - AV_EC
 *              - 0 - Echo cancellation off
 *              - 1 - Handset mode
 *              - 2 - Headset mode
 *              - 3 - Car kit mode
 *              - 4 - Speaker Mode
 *
 *  \param  pNSEnable [Optional]
 *          - AV_NS
 *              - 0 - Noise suppression off
 *              - 1 - Noise suppression on
 *
 *  \param  pTXGain [Optional]
 *          - AV_TXVOL
 *              - 0x0000 - 0xffff
 *
 *  \param  pDTMFTXGain [Optional]
 *          - AV_DTMFTXG
 *              - 0x0000 - 0xffff
 *
 *  \param  pCodecSTGain [Optional]
 *          - AV_CODECSTG
 *              - 0x0000 - 0xffff
 *
 *  \param  pTXPCMIIRFltr [Optional]
 *          - See \ref TXPCMIIRFltr for more information
 *
 *  \param  pRXPCMIIRFltr [Optional]
 *          - See \ref RXPCMIIRFltr for more information
 *
 *  \param  pMICGainSelect [Optional]
 *          - AV_MICGAIN
 *
 *  \param  pRXAVCAGCSwitch [Optional]
 *          - RX AVC/AGC Switch
 *
 *  \param  pTXAVCSwitch [Optional]
 *          - TX AVC Switch
 *
 *  \param  pRXAGCList [Optional]
 *          - See \ref RXAGCList for more information
 *
 *  \param  pRXAVCList [Optional]
 *          - See \ref RXAVCList for more information
 *
 *  \param  pTXAGCList [Optional]
 *          - See \ref TXAGCList for more information
 */
typedef struct
{
    BYTE         *pECMode;
    BYTE         *pNSEnable;
    WORD         *pTXGain;
    WORD         *pDTMFTXGain;
    WORD         *pCodecSTGain;
    TXPCMIIRFltr *pTXPCMIIRFltr;
    RXPCMIIRFltr *pRXPCMIIRFltr;
    BYTE         *pMICGainSelect;
    BYTE         *pRXAVCAGCSwitch;
    BYTE         *pTXAVCSwitch;
    RXAGCList    *pRXAGCList;
    RXAVCList    *pRXAVCList;
    TXAGCList    *pTXAGCList;
} GetAudioPathConfigResp;

/**
 *  This API gets the audio path configuration parameters.
 *
 *  \param  pGetAudioPathConfigReq[IN]
 *          - See \ref GetAudioPathConfigReq for more information
 *
 *  \param  pGetAudioPathConfigResp[OUT]
 *          - See \ref GetAudioPathConfigResp for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Device Supported: SL9090\n
 *          Timeout: 5 seconds\n
 *
 */
ULONG SLQSGetAudioPathConfig (
    GetAudioPathConfigReq  *pGetAudioPathConfigReq,
    GetAudioPathConfigResp *pGetAudioPathConfigResp );

/**
 *  This structure contains the SLQSSetAudioPathConfig request parameters.
 *
 *  \param  Profile [Mandatory]
 *          - Audio Profile
 *              - 0-9
 *
 *  \param  pECMode [Optional]
 *          - AV_EC
 *              - 0 - Echo cancellation off
 *              - 1 - Handset echo mode
 *              - 2 - Headset mode
 *              - 3 - Car kit mode
 *              - 4 - Speaker Mode
 *
 *  \param  pNSEnable [Optional]
 *          - Noise Suppression
 *              - 0 - Noise suppression off
 *              - 1 - Noise suppression on
 *
 *  \param  pTXGain [Optional]
 *          - TX Voice volume
 *              - 0x0000 - 0xffff
 *
 *  \param  pDTMFTXGain [Optional]
 *          - AV_DTMFTXG
 *              - 0x0000 - 0xffff
 *
 *  \param  pCodecSTGain [Optional]
 *          - AV_CODECSTG
 *              - 0x0000 - 0xffff
 *
 *  \param  pTXPCMIIRFltr [Optional]
 *          - See \ref TXPCMIIRFltr for more information
 *
 *  \param  pRXPCMIIRFltr [Optional]
 *          - See \ref RXPCMIIRFltr for more information
 *
 *  \param  pRXAVCAGCSwitch [Optional]
 *          - RX AVC/AGC Switch
 *
 *  \param  pTXAVCSwitch [Optional]
 *          - TX AVC Switch
 *
 *  \param  pRXAGCList [Optional]
 *          - See \ref RXAGCList for more information
 *
 *  \param  pRXAVCList [Optional]
 *          - See \ref RXAVCList for more information
 *
 *  \param  pTXAGCList [Optional]
 *          - See \ref TXAGCList for more information
 */
typedef struct
{
    BYTE         Profile;
    BYTE         *pECMode;
    BYTE         *pNSEnable;
    WORD         *pTXGain;
    WORD         *pDTMFTXGain;
    WORD         *pCodecSTGain;
    TXPCMIIRFltr *pTXPCMIIRFltr;
    RXPCMIIRFltr *pRXPCMIIRFltr;
    BYTE         *pRXAVCAGCSwitch;
    BYTE         *pTXAVCSwitch;
    RXAGCList    *pRXAGCList;
    RXAVCList    *pRXAVCList;
    TXAGCList    *pTXAGCList;
} SetAudioPathConfigReq;

/**
 *  This API sets the audio path configuration parameters.
 *
 *  \param  pSetAudioPathConfigReq[IN]
 *          - See \ref SetAudioPathConfigReq for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Device Supported: SL9090\n
 *          Timeout: 5 seconds\n
 *
 */
ULONG SLQSSetAudioPathConfig (
    SetAudioPathConfigReq  *pSetAudioPathConfigReq );

/**
 * This structure contains the SLQSGetAudioVolTLBConfig request parameters
 *
 *  \param  Profile
 *          - Audio Profile
 *              - 0-9
 *
 *  \param  Generator
 *          - Audio Generator
 *              - 0-2
 *
 *  \param  Volume
 *          - Audio Volume Level
 *              - 0-7
 *
 *  \param  Item
 *          - Item
 *              - 13 - AV_RXVOLDB
 *              - 14 - AV_DTMFVOLDB
 *              - 15 - AV_PAD
 */
typedef struct
{
    BYTE Profile;
    BYTE Generator;
    BYTE Volume;
    BYTE Item;
} GetAudioVolTLBConfigReq;

/**
 *  This structure contains the SLQSGetAudioVolTLBConfig response parameters.
 *
 *  \param  ResCode
 *          - Result of requested item
 */
typedef struct
{
    WORD ResCode;
} GetAudioVolTLBConfigResp;

/**
 *  This API gets the audio path configuration parameters.
 *
 *  \param  pGetAudioVolTLBCfgReq[IN]
 *          - See \ref GetAudioVolTLBConfigReq for more information
 *
 *  \param  pGetAudioVolTLBCfgResp[OUT]
 *          - See \ref GetAudioVolTLBConfigResp for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Device Supported: SL9090\n
 *          Timeout: 5 seconds\n
 *
 */
ULONG SLQSGetAudioVolTLBConfig (
    GetAudioVolTLBConfigReq  *pGetAudioVolTLBCfgReq,
    GetAudioVolTLBConfigResp *pGetAudioVolTLBCfgResp );

/**
 * This structure contains the SLQSSetAudioVolTLBConfig request parameters
 *
 *  \param  Profile
 *          - Audio Profile
 *              - 0-9
 *
 *  \param  Generator
 *          - Audio Generator
 *              - 0-2
 *
 *  \param  Volume
 *          - Audio Volume Level
 *              - 0-7
 *
 *  \param  Item
 *          - Item
 *              - 13 - AV_RXVOLDB
 *              - 14 - AV_DTMFVOLDB
 *              - 15 - AV_PAD
 *
 *  \param  Value
 *          - Value to be set to the volume table
 */
typedef struct
{
    BYTE Profile;
    BYTE Generator;
    BYTE Volume;
    BYTE Item;
    WORD VolValue;
} SetAudioVolTLBConfigReq;

/**
 *  This structure contains the SLQSSetAudioVolTLBConfig response parameters.
 *
 *  \param  ResCode
 *          - Result of requested item
 */
typedef struct
{
    WORD ResCode;
} SetAudioVolTLBConfigResp;

/**
 *  This API sets the audio path configuration parameters.
 *
 *  \param  pSetAudioVolTLBCfgReq[IN]
 *          - See \ref SetAudioVolTLBConfigReq for more information
 *
 *  \param  pSetAudioVolTLBCfgResp[OUT]
 *          - See \ref SetAudioVolTLBConfigResp for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Device Supported: SL9090\n
 *          Timeout: 5 seconds\n
 *
 */
ULONG SLQSSetAudioVolTLBConfig (
    SetAudioVolTLBConfigReq  *pSetAudioVolTLBCfgReq,
    SetAudioVolTLBConfigResp *pSetAudioVolTLBCfgResp );

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __GOBI_API_AUDIO_H__ */
