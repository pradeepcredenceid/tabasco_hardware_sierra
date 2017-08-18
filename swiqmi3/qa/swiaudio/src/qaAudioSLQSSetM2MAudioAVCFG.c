/*
 * \ingroup swiaudio
 *
 * \file   qaAudioSLQSSetM2MAudioAVCFG.c
 *
 * \brief  Contains Packing and UnPacking routines for the
 *         QMI_M2M_AUDIO_SET_AVCFG_MSG message.
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
#include "qaGobiApiSwiAudio.h"
#include "qaAudioSLQSSetM2MAudioAVCFG.h"

/*****************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the SLQSSetM2MAudioAVCFG Profile field to the
 * QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                  data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE  on success, eQCWWAN_ERR_XXX on error
 *
 */
enum eQCWWANError BuildTlvSetAVCFGProfile( BYTE *pBuf, BYTE *pParam )
{
    struct QmiAudioSLQSSetM2MAudioAVCFGReq *pReq =
                     (struct QmiAudioSLQSSetM2MAudioAVCFGReq *)pParam;

    SetM2MAudioAVCFGReq *lReq =
                     (SetM2MAudioAVCFGReq *)pReq->pSetM2MAudioAVCFGReq;

    /* Add TLV data */
    return PutByte( pBuf, lReq->Profile );
}

/*
 * This function packs the SLQSSetM2MAudioAVCFG Device field to the
 * QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                  data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE  on success, eQCWWAN_ERR_XXX on error
 *
 */
enum eQCWWANError BuildTlvSetDevice( BYTE *pBuf, BYTE *pParam )
{
    struct QmiAudioSLQSSetM2MAudioAVCFGReq *pReq =
                     (struct QmiAudioSLQSSetM2MAudioAVCFGReq *)pParam;

    SetM2MAudioAVCFGReq *lReq =
                     (SetM2MAudioAVCFGReq *)pReq->pSetM2MAudioAVCFGReq;

    /* Add TLV data */
    return PutByte( pBuf, lReq->Device );
}

/*
 * This function packs the SLQSSetM2MAudioAVCFG PIFace ID to the
 * QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                  data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE  on success, eQCWWAN_ERR_XXX on error
 *
 */
enum eQCWWANError BuildTlvSetPIFACEId( BYTE *pBuf, BYTE *pParam )
{
    struct QmiAudioSLQSSetM2MAudioAVCFGReq *pReq =
                     (struct QmiAudioSLQSSetM2MAudioAVCFGReq *)pParam;

    SetM2MAudioAVCFGReq *lReq =
                     (SetM2MAudioAVCFGReq *)pReq->pSetM2MAudioAVCFGReq;

    /* Add TLV data */
    return PutByte( pBuf, lReq->PIFACEId );
}

/*
 * This function packs the SLQSSetM2MAudioAVCFG PCM params to the
 * QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                  data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE  on success, eQCWWAN_ERR_XXX on error
 *
 */
enum eQCWWANError BuildTlvPCMParams( BYTE *pBuf, BYTE *pParam )
{
    struct QmiAudioSLQSSetM2MAudioAVCFGReq *pReq =
                     (struct QmiAudioSLQSSetM2MAudioAVCFGReq *)pParam;

    SetM2MAudioAVCFGReq *lReq =
                     (SetM2MAudioAVCFGReq *)pReq->pSetM2MAudioAVCFGReq;

    BYTE      lCtr = 0;
    PCMparams *pTemp = lReq->pPCMParams;
    enum      eQCWWANError eRCode;

    if( NULL == pTemp )
    {
        return eQCWWAN_ERR_NONE;
    }

    for( lCtr = 0; lCtr < pTemp->iFaceTabLen; lCtr++ )
    {
        /* Add TLV data */
        eRCode = PutByte( pBuf, pTemp->iFaceTab[lCtr] );
        if( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
    }
    return eRCode;
}
/*
 * This function packs the SLQSSetM2MAudioAVCFG parameters to the QMI message
 * SDU
 *
 * \param  pParamField [OUT] - Pointer to storage into which the packed
 *                             data will be placed by this function.
 *
 * \param  pMlength    [OUT] - Total length of built message.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError PkQmiAudioSLQSSetM2MAudioAVCFG(
    WORD                *pMlength,
    BYTE                *pParamField,
    SetM2MAudioAVCFGReq *pSetM2MAudioAVCFGReq )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_SET_M2M_AUDIO_AVCFG_PROFILE,    &BuildTlvSetAVCFGProfile },
        { eTLV_SET_M2M_AUDIO_AVCFG_DEVICE,     &BuildTlvSetDevice },
        { eTLV_SET_M2M_AUDIO_AVCFG_PIFACEID,   &BuildTlvSetPIFACEId },
        { eTLV_SET_M2M_AUDIO_AVCFG_PCM_PARAMS, &BuildTlvPCMParams },
        { eTLV_TYPE_INVALID,                   NULL } /* Important. Sentinel.
                                                       * Signifies last item in map
                                                       */
    };

    enum   eQCWWANError                    eRCode;
    struct QmiAudioSLQSSetM2MAudioAVCFGReq req;

    req.pSetM2MAudioAVCFGReq = pSetM2MAudioAVCFGReq;

    eRCode = qmbuild( pParamField,
                      (BYTE *)&req,
                      map,
                      eQMI_SWIAUDIO_SET_AVCFG,
                      pMlength );
    return eRCode;
}

/*****************************************************************************
 * Response handling
 ******************************************************************************/

/*
 * This function unpacks the SLQSSetM2MAudioAVCFG response message to a
 * user-provided response structure.
 *
 * \param     MdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UpkQmiAudioSLQSSetM2MAudioAVCFG(
    BYTE                                    *pMdmResp,
    struct QmiAudioSLQSSetM2MAudioAVCFGResp *pApiResp)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_SET_M2M_AUDIO_AVCFG_EC, &qmUnpackTlvResultCode },
        { eTLV_TYPE_INVALID,           NULL } /* Important. Sentinel.
                                               * Signifies last item in map.
                                               */
    };
    eRCode = qmunpackresp( pMdmResp,
                           (BYTE*)pApiResp,
                           map,
                           eQMI_SWIAUDIO_SET_AVCFG );
    return eRCode;
}
