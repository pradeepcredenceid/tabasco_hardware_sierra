/*
 * \ingroup audio
 *
 * \file   qaAudioSLQSGetAudioPathConfig.c
 *
 * \brief  Contains Packing and UnPacking routines for the
 *         QMI_AUDIO_GET_AUDIO_PATH_CONFIG_MSG message.
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
#include "qaAudioSLQSGetAudioPathConfig.h"

/*****************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the SLQSGetAudioPathConfig profile field to the QMI
 * message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will be
 *                         placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvProfile(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiAudioSLQSGetAudioPathConfigReq *pReq =
                        (struct QmiAudioSLQSGetAudioPathConfigReq *)pParam;
    GetAudioPathConfigReq *tReq = pReq->pGetAudioPathConfigReq;

    /* Insert the Profile into the QMI message */
    return PutByte( pBuf, tReq->Profile);
}

/*
 * This function packs the SLQSGetAudioConfigPath item field to the QMI
 * message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will be
 *                         placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvAudioItem(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiAudioSLQSGetAudioPathConfigReq *pReq =
                        (struct QmiAudioSLQSGetAudioPathConfigReq *)pParam;
    GetAudioPathConfigReq *tReq = pReq->pGetAudioPathConfigReq;

    /* Insert the item into the QMI message */
    return PutByte( pBuf, tReq->Item);
}
/*
 * This function packs the SLQSGetAudioConfigPath parameters to the QMI message
 * SDU
 *
 * \param  pParamField [OUT] - Pointer to storage into which the packed
 *                             data will be placed by this function.
 *
 * \param  pMlength    [OUT] - Total length of built message.
 *
 * \param  pGetAudioPathReq [IN] - Request parameters for packing.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError PkQmiAudioSLQSGetAudioPathConfig(
    WORD                  *pMlength,
    BYTE                  *pParamField,
    GetAudioPathConfigReq *pGetAudioPathConfigReq )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_AUDIO_PATH_PROFILE, &BuildTlvProfile },
        { eTLV_AUDIO_ITEM,         &BuildTlvAudioItem },
        { eTLV_TYPE_INVALID,       NULL } /* Important. Sentinel.
                                           * Signifies last item in map
                                           */
    };

    struct QmiAudioSLQSGetAudioPathConfigReq req;
    enum   eQCWWANError           eRCode;

    /* Intialize the request structure */
    slmemset( (char *)&req,
              0,
              sizeof(struct QmiAudioSLQSGetAudioPathConfigReq) );
    req.pGetAudioPathConfigReq   = pGetAudioPathConfigReq;

    eRCode = qmbuild( pParamField,
                      (BYTE *)&req,
                      map,
                      eQMI_AUDIO_GET_AUDIO_PATH_CONFIG,
                      pMlength );
    return eRCode;
}

/*****************************************************************************
 * Response handling
 ******************************************************************************/

/*
 * This function unpacks the SLQSGetAudioPathConfig EC mode from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvEC(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiAudioSLQSGetAudioPathConfigResp *lResp =
        (struct QmiAudioSLQSGetAudioPathConfigResp *)pResp;
    GetAudioPathConfigResp *tResp = lResp->pGetAudioPathConfigResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              bTemp;

    if( NULL == tResp->pECMode )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get EC Mode in local variable */
    eRCode = GetByte( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    *(tResp->pECMode) = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetAudioPathConfig NS enable parameter from
 * the QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvNS(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiAudioSLQSGetAudioPathConfigResp *lResp =
        (struct QmiAudioSLQSGetAudioPathConfigResp *)pResp;
    GetAudioPathConfigResp *tResp = lResp->pGetAudioPathConfigResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              bTemp;

    if( NULL == tResp->pNSEnable )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get NS Enable in local variable */
    eRCode = GetByte( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    *(tResp->pNSEnable) = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetAudioPathConfig TX gain from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvTXVol(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiAudioSLQSGetAudioPathConfigResp *lResp =
        (struct QmiAudioSLQSGetAudioPathConfigResp *)pResp;
    GetAudioPathConfigResp *tResp = lResp->pGetAudioPathConfigResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    WORD              bTemp;

    if( NULL == tResp->pTXGain )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get TX Gain in local variable */
    eRCode = GetWord( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    *(tResp->pTXGain) = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetAudioPathConfig DTMF TX gain from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvDTMFXG(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiAudioSLQSGetAudioPathConfigResp *lResp =
        (struct QmiAudioSLQSGetAudioPathConfigResp *)pResp;
    GetAudioPathConfigResp *tResp = lResp->pGetAudioPathConfigResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    WORD              bTemp;

    if( NULL == tResp->pDTMFTXGain )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get DTMF TX Gain in local variable */
    eRCode = GetWord( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    *(tResp->pDTMFTXGain) = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetAudioPathConfig Codec ST gain from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvCodecSTG(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiAudioSLQSGetAudioPathConfigResp *lResp =
        (struct QmiAudioSLQSGetAudioPathConfigResp *)pResp;
    GetAudioPathConfigResp *tResp = lResp->pGetAudioPathConfigResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    WORD              bTemp;

    if( NULL == tResp->pCodecSTGain )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get Codec ST Gain in local variable */
    eRCode = GetWord( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    *(tResp->pCodecSTGain) = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetAudioPathConfig TX PCM Fltr parameters from
 * the QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvTXPCMfltr(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiAudioSLQSGetAudioPathConfigResp *lResp =
        (struct QmiAudioSLQSGetAudioPathConfigResp *)pResp;
    GetAudioPathConfigResp *tResp = lResp->pGetAudioPathConfigResp;
    TXPCMIIRFltr           *fResp = tResp->pTXPCMIIRFltr;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    WORD              lTemp;
    BYTE              bTemp;
    BYTE              lCtr = 0;

    if( NULL == fResp )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get Flag in local variable */
    eRCode = GetWord( pTlvData, &lTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }
    *(fResp->pFlag) = lTemp;

    /* Get Stage Count in local variable */
    eRCode = GetWord( pTlvData, &lTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }
    *(fResp->pStageCnt) = lTemp;

    /* Get Stage 0 Value in local variable */
    for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
    {
        eRCode = GetByte( pTlvData, &bTemp );
        if (eQCWWAN_ERR_NONE != eRCode)
        {
            return eRCode;
        }
        fResp->pStage0Val[lCtr] = bTemp;
    }

    /* Get Stage 1 Value in local variable */
    for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
    {
        eRCode = GetByte( pTlvData, &bTemp );
        if (eQCWWAN_ERR_NONE != eRCode)
        {
            return eRCode;
        }
        fResp->pStage1Val[lCtr] = bTemp;
    }

    /* Get Stage 2 Value in local variable */
    for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
    {
        eRCode = GetByte( pTlvData, &bTemp );
        if (eQCWWAN_ERR_NONE != eRCode)
        {
            return eRCode;
        }
        fResp->pStage2Val[lCtr] = bTemp;
    }

    /* Get Stage 3 Value in local variable */
    for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
    {
        eRCode = GetByte( pTlvData, &bTemp );
        if (eQCWWAN_ERR_NONE != eRCode)
        {
            return eRCode;
        }
        fResp->pStage3Val[lCtr] = bTemp;
    }

    /* Get Stage 4 Value in local variable */
    for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
    {
        eRCode = GetByte( pTlvData, &bTemp );
        if (eQCWWAN_ERR_NONE != eRCode)
        {
            return eRCode;
        }
        fResp->pStage4Val[lCtr] = bTemp;
    }
    return eRCode;
}

/*
 * This function unpacks the SLQSGetAudioPathConfig RX PCM Fltr parameters from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvRXPCMfltr(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiAudioSLQSGetAudioPathConfigResp *lResp =
        (struct QmiAudioSLQSGetAudioPathConfigResp *)pResp;
    GetAudioPathConfigResp *tResp = lResp->pGetAudioPathConfigResp;
    RXPCMIIRFltr           *fResp = tResp->pRXPCMIIRFltr;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    WORD              lTemp;
    BYTE              bTemp;
    BYTE              lCtr = 0;

    if( NULL == fResp )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get Flag in local variable */
    eRCode = GetWord( pTlvData, &lTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }
    *(fResp->pFlag) = lTemp;

    /* Get Stage Count in local variable */
    eRCode = GetWord( pTlvData, &lTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }
    *(fResp->pStageCnt) = lTemp;

    /* Get Stage 0 Value in local variable */
    for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
    {
        eRCode = GetByte( pTlvData, &bTemp );
        if (eQCWWAN_ERR_NONE != eRCode)
        {
            return eRCode;
        }
        fResp->pStage0Val[lCtr] = bTemp;
    }

    /* Get Stage 1 Value in local variable */
    for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
    {
        eRCode = GetByte( pTlvData, &bTemp );
        if (eQCWWAN_ERR_NONE != eRCode)
        {
            return eRCode;
        }
        fResp->pStage1Val[lCtr] = bTemp;
    }

    /* Get Stage 2 Value in local variable */
    for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
    {
        eRCode = GetByte( pTlvData, &bTemp );
        if (eQCWWAN_ERR_NONE != eRCode)
        {
            return eRCode;
        }
        fResp->pStage2Val[lCtr] = bTemp;
    }

    /* Get Stage 3 Value in local variable */
    for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
    {
        eRCode = GetByte( pTlvData, &bTemp );
        if (eQCWWAN_ERR_NONE != eRCode)
        {
            return eRCode;
        }
        fResp->pStage3Val[lCtr] = bTemp;
    }

    /* Get Stage 4 Value in local variable */
    for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
    {
        eRCode = GetByte( pTlvData, &bTemp );
        if (eQCWWAN_ERR_NONE != eRCode)
        {
            return eRCode;
        }
        fResp->pStage4Val[lCtr] = bTemp;
    }
    return eRCode;
}

/*
 * This function unpacks the SLQSGetAudioPathConfig MIC Gain select from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvMICGain(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiAudioSLQSGetAudioPathConfigResp *lResp =
        (struct QmiAudioSLQSGetAudioPathConfigResp *)pResp;
    GetAudioPathConfigResp *tResp = lResp->pGetAudioPathConfigResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              bTemp;

    if( NULL == tResp->pMICGainSelect )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get MIC Gain Select in local variable */
    eRCode = GetByte( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    *(tResp->pMICGainSelect) = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetAudioPathConfig RX AVC/AGC Switch from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvRXAVCAGCSW(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiAudioSLQSGetAudioPathConfigResp *lResp =
        (struct QmiAudioSLQSGetAudioPathConfigResp *)pResp;
    GetAudioPathConfigResp *tResp = lResp->pGetAudioPathConfigResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              bTemp;

    if( NULL == tResp->pRXAVCAGCSwitch )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get RX AVC/AGC Switch in local variable */
    eRCode = GetByte( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    *(tResp->pRXAVCAGCSwitch) = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetAudioPathConfig TX AVC Switch from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvTXAVCSwitch(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiAudioSLQSGetAudioPathConfigResp *lResp =
        (struct QmiAudioSLQSGetAudioPathConfigResp *)pResp;
    GetAudioPathConfigResp *tResp = lResp->pGetAudioPathConfigResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              bTemp;

    if( NULL == tResp->pTXAVCSwitch )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get TX AVC Switch in local variable */
    eRCode = GetByte( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    *(tResp->pTXAVCSwitch) = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetAudioPathConfig RX AGC List from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvRXAGCList(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiAudioSLQSGetAudioPathConfigResp *lResp =
        (struct QmiAudioSLQSGetAudioPathConfigResp *)pResp;
    GetAudioPathConfigResp *tResp = lResp->pGetAudioPathConfigResp;
    RXAGCList              *rResp = tResp->pRXAGCList;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    WORD              bTemp;

    if( NULL == rResp )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get RX Static Gain in local variable */
    eRCode = GetWord( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }
    *(rResp->pRXStaticGain) = bTemp;

    /* Get RX Gain Selection Flag in local variable */
    eRCode = GetWord( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }
    *(rResp->pRXAIG) = bTemp;

    /* Get RX Expansion Threshold in local variable */
    eRCode = GetWord( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }
    *(rResp->pRXExpThres) = bTemp;

    /* Get RX Expansion Slope in local variable */
    eRCode = GetWord( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }
    *(rResp->pRXExpSlope) = bTemp;

    /* Get RX Compression Threshold in local variable */
    eRCode = GetWord( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }
    *(rResp->pRXComprThres) = bTemp;

    /* Get RX Compression Slope in local variable */
    eRCode = GetWord( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }
    *(rResp->pRXComprSlope) = bTemp;

    return eRCode;
}

/*
 * This function unpacks the SLQSGetAudioPathConfig RX AVC List from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvRXAVCList(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiAudioSLQSGetAudioPathConfigResp *lResp =
        (struct QmiAudioSLQSGetAudioPathConfigResp *)pResp;
    GetAudioPathConfigResp *tResp = lResp->pGetAudioPathConfigResp;
    RXAVCList              *rResp = tResp->pRXAVCList;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    WORD              bTemp;

    if( NULL == rResp )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get AVC sensitivity in local variable */
    eRCode = GetWord( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }
    *(rResp->pAVRXAVCSens) = bTemp;

    /* Get AVC headroom in local variable */
    eRCode = GetWord( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }
    *(rResp->pAVRXAVCHeadroom) = bTemp;

    return eRCode;
}

/*
 * This function unpacks the SLQSGetAudioPathConfig TX AGC List from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvTXAGCList(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiAudioSLQSGetAudioPathConfigResp *lResp =
        (struct QmiAudioSLQSGetAudioPathConfigResp *)pResp;
    GetAudioPathConfigResp *tResp = lResp->pGetAudioPathConfigResp;
    TXAGCList              *rResp = tResp->pTXAGCList;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    WORD              bTemp;

    if( NULL == rResp )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get TX Static Gain in local variable */
    eRCode = GetWord( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }
    *(rResp->pTXStaticGain) = bTemp;

    /* Get TX Gain Selection Flag in local variable */
    eRCode = GetWord( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }
    *(rResp->pTXAIG) = bTemp;

    /* Get TX Expansion Threshold in local variable */
    eRCode = GetWord( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }
    *(rResp->pTXExpThres) = bTemp;

    /* Get TX Expansion Slope in local variable */
    eRCode = GetWord( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }
    *(rResp->pTXExpSlope) = bTemp;

    /* Get TX Compression Threshold in local variable */
    eRCode = GetWord( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }
    *(rResp->pTXComprThres) = bTemp;

    /* Get TX Compression Slope in local variable */
    eRCode = GetWord( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }
    *(rResp->pTXComprSlope) = bTemp;

    return eRCode;
}

/*
 * This function unpacks the SLQSGetAudioPathConfig response message to a
 * user-provided response structure.
 *
 * \param     MdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UpkQmiAudioSLQSGetAudioPathConfig(
    BYTE                                      *pMdmResp,
    struct QmiAudioSLQSGetAudioPathConfigResp *pApiResp)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,       &qmUnpackTlvResultCode },
        { eTLV_AV_EC,             &UnpackTlvEC },
        { eTLV_AV_NS,             &UnpackTlvNS },
        { eTLV_AV_TXVOL,          &UnpackTlvTXVol },
        { eTLV_AV_DTMFTXG,        &UnpackTlvDTMFXG },
        { eTLV_AV_CODECSTG,       &UnpackTlvCodecSTG },
        { eTLV_AV_TXPCMIIRFLTR,   &UnpackTlvTXPCMfltr },
        { eTLV_AV_RXPCMIIRFLTR,   &UnpackTlvRXPCMfltr },
        { eTLV_AV_MICGAIN,        &UnpackTlvMICGain },
        { eTLV_RX_AVC_AGC_SWITCH, &UnpackTlvRXAVCAGCSW },
        { eTLV_TX_AVC_SWITCH,     &UnpackTlvTXAVCSwitch },
        { eTLV_RX_AGC_LIST,       &UnpackTlvRXAGCList },
        { eTLV_RX_AVC_LIST,       &UnpackTlvRXAVCList },
        { eTLV_TX_AGC_LIST,       &UnpackTlvTXAGCList },
        { eTLV_TYPE_INVALID,      NULL } /* Important. Sentinel.
                                          * Signifies last item in map.
                                          */
    };
    eRCode = qmunpackresp( pMdmResp,
                           (BYTE*)pApiResp,
                           map,
                           eQMI_AUDIO_GET_AUDIO_PATH_CONFIG );
    return eRCode;
}
