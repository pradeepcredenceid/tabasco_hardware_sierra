/*
 * \ingroup audio
 *
 * \file   qaAudioSLQSSetAudioPath.c
 *
 * \brief  Contains Packing and UnPacking routines for the
 *         QMI_AUDIO_SET_AUDIO_PATH_MSG message.
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaQmiBasic.h"
#include "qaGobiApiAudio.h"
#include "qaAudioSLQSSetAudioPathConfig.h"
#include "qaAudioSLQSGetAudioPathConfig.h"

/*****************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the SLQSSetAudioPathConfig Profile Number field to
 * the QMI message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvAudioProfileNum(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiAudioSLQSSetAudioPathConfigReq *pReq =
                        (struct QmiAudioSLQSSetAudioPathConfigReq *)pParam;
    SetAudioPathConfigReq *tReq = pReq->pSetAudioPathConfigReq;

    /* Insert the profile number into the QMI message */
    return PutByte( pBuf, tReq->Profile );
}

/*
 * This function packs the SLQSSetAudioPathConfig Echo Cancellation field to
 * the QMI message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvEC(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiAudioSLQSSetAudioPathConfigReq *pReq =
                        (struct QmiAudioSLQSSetAudioPathConfigReq *)pParam;
    SetAudioPathConfigReq *tReq = pReq->pSetAudioPathConfigReq;

    if( NULL == tReq->pECMode )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert echo cancellation into the QMI message */
    return PutByte( pBuf, *(tReq->pECMode) );
}

/*
 * This function packs the SLQSSetAudioPathConfig Noise suppression field to
 * the QMI message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvNS(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiAudioSLQSSetAudioPathConfigReq *pReq =
                        (struct QmiAudioSLQSSetAudioPathConfigReq *)pParam;
    SetAudioPathConfigReq *tReq = pReq->pSetAudioPathConfigReq;

    if( NULL == tReq->pNSEnable )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert noise suppression into the QMI message */
    return PutByte( pBuf, *(tReq->pNSEnable) );
}

/*
 * This function packs the SLQSSetAudioPathConfig TX Voice volume field to
 * the QMI message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvTXVol(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiAudioSLQSSetAudioPathConfigReq *pReq =
                        (struct QmiAudioSLQSSetAudioPathConfigReq *)pParam;
    SetAudioPathConfigReq *tReq = pReq->pSetAudioPathConfigReq;

    if( NULL == tReq->pTXGain )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert TX Voice volume gain into the QMI message */
    return PutWord( pBuf, *(tReq->pTXGain) );
}

/*
 * This function packs the SLQSSetAudioPathConfig DTMF TX Gain field to the QMI
 * message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvDTMFTXG(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiAudioSLQSSetAudioPathConfigReq *pReq =
                        (struct QmiAudioSLQSSetAudioPathConfigReq *)pParam;
    SetAudioPathConfigReq *tReq = pReq->pSetAudioPathConfigReq;

    if( NULL == tReq->pDTMFTXGain )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert DTMF TX Voice volume gain into the QMI message */
    return PutWord( pBuf, *(tReq->pDTMFTXGain) );
}

/*
 * This function packs the SLQSSetAudioPathConfig Codec ST Gain field to the
 * QMI message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvCodecSTG(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiAudioSLQSSetAudioPathConfigReq *pReq =
                        (struct QmiAudioSLQSSetAudioPathConfigReq *)pParam;
    SetAudioPathConfigReq *tReq = pReq->pSetAudioPathConfigReq;

    if( NULL == tReq->pCodecSTGain )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert Codec ST Gain into the QMI message */
    return PutWord( pBuf, *(tReq->pCodecSTGain) );
}

/*
 * This function packs the SLQSSetAudioPathConfig TX PCM Filter fields to the
 * QMI message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvTXPCMFLTR(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiAudioSLQSSetAudioPathConfigReq *pReq =
                        (struct QmiAudioSLQSSetAudioPathConfigReq *)pParam;
    SetAudioPathConfigReq *tReq          = pReq->pSetAudioPathConfigReq;
    TXPCMIIRFltr          *pTXPCMIIRFltr = tReq->pTXPCMIIRFltr;
    enum eQCWWANError     eRCode         = eQCWWAN_ERR_NONE;
    BYTE                  lCtr           = 0;

    if( NULL == pTXPCMIIRFltr )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert Flag into the QMI message */
    eRCode = PutWord( pBuf, *(pTXPCMIIRFltr->pFlag) );
    if( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    /* Insert Stage Count into the QMI message */
    eRCode = PutWord( pBuf, *(pTXPCMIIRFltr->pStageCnt) );
    if( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    /* Insert Stage 0 Value into the QMI message */
    for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
    {
        eRCode = PutByte( pBuf, pTXPCMIIRFltr->pStage0Val[lCtr] );
        if( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
    }

    /* Insert Stage 1 Value into the QMI message */
    for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
    {
        eRCode = PutByte( pBuf, pTXPCMIIRFltr->pStage1Val[lCtr] );
        if( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
    }

    /* Insert Stage 2 Value into the QMI message */
    for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
    {
        eRCode = PutByte( pBuf, pTXPCMIIRFltr->pStage2Val[lCtr] );
        if( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
    }

    /* Insert Stage 3 Value into the QMI message */
    for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
    {
        eRCode = PutByte( pBuf, pTXPCMIIRFltr->pStage3Val[lCtr] );
        if( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
    }

    /* Insert Stage 4 Value into the QMI message */
    for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
    {
        eRCode = PutByte( pBuf, pTXPCMIIRFltr->pStage4Val[lCtr] );
        if( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
    }

    return eRCode;
}

/*
 * This function packs the SLQSSetAudioPathConfig RX PCM Filter fields to
 * the QMI message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvRXPCMFLTR(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiAudioSLQSSetAudioPathConfigReq *pReq =
                        (struct QmiAudioSLQSSetAudioPathConfigReq *)pParam;
    SetAudioPathConfigReq *tReq          = pReq->pSetAudioPathConfigReq;
    RXPCMIIRFltr          *pRXPCMIIRFltr = tReq->pRXPCMIIRFltr;
    enum eQCWWANError     eRCode         = eQCWWAN_ERR_NONE;
    BYTE                  lCtr           = 0;

    if( NULL == pRXPCMIIRFltr )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert Flag into the QMI message */
    eRCode = PutWord( pBuf, *(pRXPCMIIRFltr->pFlag) );
    if( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    /* Insert Stage Count into the QMI message */
    eRCode = PutWord( pBuf, *(pRXPCMIIRFltr->pStageCnt) );
    if( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    /* Insert Stage 0 Value into the QMI message */
    for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
    {
        eRCode = PutByte( pBuf, pRXPCMIIRFltr->pStage0Val[lCtr] );
        if( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
    }

    /* Insert Stage 1 Value into the QMI message */
    for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
    {
        eRCode = PutByte( pBuf, pRXPCMIIRFltr->pStage1Val[lCtr] );
        if( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
    }

    /* Insert Stage 2 Value into the QMI message */
    for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
    {
        eRCode = PutByte( pBuf, pRXPCMIIRFltr->pStage2Val[lCtr] );
        if( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
    }

    /* Insert Stage 3 Value into the QMI message */
    for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
    {
        eRCode = PutByte( pBuf, pRXPCMIIRFltr->pStage3Val[lCtr] );
        if( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
    }

    /* Insert Stage 4 Value into the QMI message */
    for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
    {
        eRCode = PutByte( pBuf, pRXPCMIIRFltr->pStage4Val[lCtr] );
        if( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
    }

    return eRCode;
}

/*
 * This function packs the SLQSSetAudioPathConfig RX AVC/AGC Switch field to
 * the QMI message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvRXAVCAGCSW(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiAudioSLQSSetAudioPathConfigReq *pReq =
                        (struct QmiAudioSLQSSetAudioPathConfigReq *)pParam;
    SetAudioPathConfigReq *tReq = pReq->pSetAudioPathConfigReq;

    if( NULL == tReq->pRXAVCAGCSwitch )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert RX AVC AGC Switch into the QMI message */
    return PutByte( pBuf, *(tReq->pRXAVCAGCSwitch) );
}

/*
 * This function packs the SLQSSetAudioPathConfig TX AVC Switch field to
 * the QMI message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvTXAVCSW(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiAudioSLQSSetAudioPathConfigReq *pReq =
                        (struct QmiAudioSLQSSetAudioPathConfigReq *)pParam;
    SetAudioPathConfigReq *tReq = pReq->pSetAudioPathConfigReq;

    if( NULL == tReq->pTXAVCSwitch )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert TX AVC Switch into the QMI message */
    return PutByte( pBuf, *(tReq->pTXAVCSwitch) );
}

/*
 * This function packs the SLQSSetAudioPathConfig RX AGC List fields to the QMI
 * message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will be
 *                         placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvRXAGCList(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiAudioSLQSSetAudioPathConfigReq *pReq =
                        (struct QmiAudioSLQSSetAudioPathConfigReq *)pParam;
    SetAudioPathConfigReq *tReq       = pReq->pSetAudioPathConfigReq;
    RXAGCList             *pRXAGCList = tReq->pRXAGCList;
    enum eQCWWANError     eRCode      = eQCWWAN_ERR_NONE;

    if( NULL == pRXAGCList )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert RX AGC Static gain into the QMI message */
    eRCode = PutWord( pBuf, *(pRXAGCList->pRXStaticGain) );
    if( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    /* Insert RX gain selection flag into the QMI message */
    eRCode = PutWord( pBuf, *(pRXAGCList->pRXAIG) );
    if( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    /* Insert RX expansion threshold into the QMI message */
    eRCode = PutWord( pBuf, *(pRXAGCList->pRXExpThres) );
    if( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    /* Insert RX expansion slope into the QMI message */
    eRCode = PutWord( pBuf, *(pRXAGCList->pRXExpSlope) );
    if( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    /* Insert RX compression threshold into the QMI message */
    eRCode = PutWord( pBuf, *(pRXAGCList->pRXComprThres) );
    if( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    /* Insert RX compression slope into the QMI message */
    eRCode = PutWord( pBuf, *(pRXAGCList->pRXComprSlope) );
    if( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }
    return eRCode;
}

/*
 * This function packs the SLQSSetAudioPathConfig RX AVC List fields to the QMI
 * message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will be
 *                         placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvRXAVCList(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiAudioSLQSSetAudioPathConfigReq *pReq =
                        (struct QmiAudioSLQSSetAudioPathConfigReq *)pParam;
    SetAudioPathConfigReq *tReq       = pReq->pSetAudioPathConfigReq;
    RXAVCList             *pRXAVCList = tReq->pRXAVCList;
    enum eQCWWANError     eRCode      = eQCWWAN_ERR_NONE;

    if( NULL == pRXAVCList )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert RX AVC sensitivity into the QMI message */
    eRCode = PutWord( pBuf, *(pRXAVCList->pAVRXAVCSens) );
    if( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    /* Insert RX AVC headroom into the QMI message */
    eRCode = PutWord( pBuf, *(pRXAVCList->pAVRXAVCHeadroom) );
    if( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    return eRCode;
}

/*
 * This function packs the SLQSSetAudioPathConfig TX AGC List fields to the QMI
 * message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvTXAGCList(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiAudioSLQSSetAudioPathConfigReq *pReq =
                        (struct QmiAudioSLQSSetAudioPathConfigReq *)pParam;
    SetAudioPathConfigReq *tReq       = pReq->pSetAudioPathConfigReq;
    TXAGCList             *pTXAGCList = tReq->pTXAGCList;
    enum eQCWWANError     eRCode      = eQCWWAN_ERR_NONE;

    if( NULL == pTXAGCList )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert TX AGC Static gain into the QMI message */
    eRCode = PutWord( pBuf, *(pTXAGCList->pTXStaticGain) );
    if( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    /* Insert TX gain selection flag into the QMI message */
    eRCode = PutWord( pBuf, *(pTXAGCList->pTXAIG) );
    if( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    /* Insert TX expansion threshold into the QMI message */
    eRCode = PutWord( pBuf, *(pTXAGCList->pTXExpThres) );
    if( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    /* Insert TX expansion slope into the QMI message */
    eRCode = PutWord( pBuf, *(pTXAGCList->pTXExpSlope) );
    if( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    /* Insert TX compression threshold into the QMI message */
    eRCode = PutWord( pBuf, *(pTXAGCList->pTXComprThres) );
    if( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    /* Insert TX compression slope into the QMI message */
    eRCode = PutWord( pBuf, *(pTXAGCList->pTXComprSlope) );
    if( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }
    return eRCode;
}

/*
 * This function packs the SLQSSetAudioPathConfig parameters to the QMI message
 * SDU
 *
 * \param  pParamField [OUT] - Pointer to storage into which the packed
 *                             data will be placed by this function.
 *
 * \param  pMlength    [OUT] - Total length of built message.
 *
 * \param  pSetAudioPathReq [IN] - Request parameters for packing.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError PkQmiAudioSLQSSetAudioPathConfig(
    WORD                  *pMlength,
    BYTE                  *pParamField,
    SetAudioPathConfigReq *pSetAudioPathConfigReq )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_AUDIO_PATH_PROF,   &BuildTlvAudioProfileNum },
        { eTLV_AV_EC,             &BuildTlvEC },
        { eTLV_AV_NS,             &BuildTlvNS },
        { eTLV_AV_TXVOL,          &BuildTlvTXVol },
        { eTLV_AV_DTMFTXG,        &BuildTlvDTMFTXG },
        { eTLV_AV_CODECSTG,       &BuildTlvCodecSTG },
        { eTLV_AV_TXPCMIIRFLTR,   &BuildTlvTXPCMFLTR },
        { eTLV_AV_RXPCMIIRFLTR,   &BuildTlvRXPCMFLTR },
        { eTLV_RX_AVC_AGC_SWITCH, &BuildTlvRXAVCAGCSW },
        { eTLV_TX_AVC_SWITCH,     &BuildTlvTXAVCSW },
        { eTLV_RX_AGC_LIST,       &BuildTlvRXAGCList },
        { eTLV_RX_AVC_LIST,       &BuildTlvRXAVCList },
        { eTLV_TX_AGC_LIST,       &BuildTlvTXAGCList },
        { eTLV_TYPE_INVALID,      NULL } /* Important. Sentinel.
                                          * Signifies last item in map
                                          */
    };

    struct QmiAudioSLQSSetAudioPathConfigReq req;
    enum   eQCWWANError                      eRCode;

    /* Intialize the request structure */
    slmemset( (char *)&req,
              0,
              sizeof(struct QmiAudioSLQSSetAudioPathConfigReq) );
    req.pSetAudioPathConfigReq = pSetAudioPathConfigReq;

    eRCode = qmbuild( pParamField,
                      (BYTE *)&req,
                      map,
                      eQMI_AUDIO_SET_AUDIO_PATH_CONFIG,
                      pMlength );
    return eRCode;
}

/*****************************************************************************
 * Response handling
 ******************************************************************************/

/*
 * This function unpacks the SLQSSetAudioPathConfig response message to a
 * user-provided response structure.
 *
 * \param     MdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UpkQmiAudioSLQSSetAudioPathConfig(
    BYTE                                      *pMdmResp,
    struct QmiAudioSLQSSetAudioPathConfigResp *pApiResp)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,  &qmUnpackTlvResultCode },
        { eTLV_TYPE_INVALID, NULL } /* Important. Sentinel.
                                     * Signifies last item in map.
                                     */
    };
    eRCode = qmunpackresp( pMdmResp,
                           (BYTE*)pApiResp,
                           map,
                           eQMI_AUDIO_SET_AUDIO_PATH_CONFIG );
    return eRCode;
}
