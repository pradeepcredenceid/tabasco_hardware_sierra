/*
 * \ingroup swiaudio
 *
 * \file   qaAudioSLQSSetM2MAVMute.c
 *
 * \brief  Contains Packing and UnPacking routines for the
 *         QMI_M2M_AUDIO_SET_AVMUTE_MSG message.
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
#include "qaAudioSLQSSetM2MAVMute.h"

/*****************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the SLQSSetM2MAVMute Profile field to the
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
enum eQCWWANError BuildTlvSetAVMuteProfile( BYTE *pBuf, BYTE *pParam )
{
    struct QmiAudioSLQSSetM2MAVMuteReq *pReq =
                     (struct QmiAudioSLQSSetM2MAVMuteReq *)pParam;

    SetM2MAVMuteReq *lReq = (SetM2MAVMuteReq *)pReq->setM2MAVMuteReq;

    /* Add TLV data */
    return PutByte( pBuf, lReq->Profile );
}

/*
 * This function packs the SLQSSetM2MAVMute Ear Mute field to the
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
enum eQCWWANError BuildTlvSetAVMuteEarMute( BYTE *pBuf, BYTE *pParam )
{
    struct QmiAudioSLQSSetM2MAVMuteReq *pReq =
                     (struct QmiAudioSLQSSetM2MAVMuteReq *)pParam;

    SetM2MAVMuteReq *lReq = (SetM2MAVMuteReq *)pReq->setM2MAVMuteReq;

    /* Add TLV data */
    return PutByte( pBuf, lReq->EarMute );
}

/*
 * This function packs the SLQSSetM2MAVMute Mic Mute field to the
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
enum eQCWWANError BuildTlvSetAVMuteMicMute( BYTE *pBuf, BYTE *pParam )
{
    struct QmiAudioSLQSSetM2MAVMuteReq *pReq =
                     (struct QmiAudioSLQSSetM2MAVMuteReq *)pParam;

    SetM2MAVMuteReq *lReq = (SetM2MAVMuteReq *)pReq->setM2MAVMuteReq;

    /* Add TLV data */
    return PutByte( pBuf, lReq->MicMute );
}

/*
 * This function packs the SLQSSetM2MAVMute CWT Mute field to the
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
enum eQCWWANError BuildTlvSetAVMuteCWTMute( BYTE *pBuf, BYTE *pParam )
{
    struct QmiAudioSLQSSetM2MAVMuteReq *pReq =
                     (struct QmiAudioSLQSSetM2MAVMuteReq *)pParam;

    SetM2MAVMuteReq *lReq = (SetM2MAVMuteReq *)pReq->setM2MAVMuteReq;

    if( NULL == lReq->pCwtMute)
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Add TLV data */
    return PutByte( pBuf, *(lReq->pCwtMute) );
}
/*
 * This function packs the SLQSSetM2MAVMute parameters to the QMI message
 * SDU
 *
 * \param  pParamField [OUT] - Pointer to storage into which the packed
 *                             data will be placed by this function.
 *
 * \param  pMlength    [OUT] - Total length of built message.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError PkQmiAudioSLQSSetM2MAVMute(
    WORD             *pMlength,
    BYTE             *pParamField,
    SetM2MAVMuteReq  *setM2MAVMuteReq )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_SET_M2M_AVMUTE_PROFILE, &BuildTlvSetAVMuteProfile },
        { eTLV_SET_M2M_AVMUTE_EARMUTE, &BuildTlvSetAVMuteEarMute },
        { eTLV_SET_M2M_AVMUTE_MICMUTE, &BuildTlvSetAVMuteMicMute },
        { eTLV_SET_M2M_AVMUTE_CWTMUTE, &BuildTlvSetAVMuteCWTMute },
        { eTLV_TYPE_INVALID,           NULL } /* Important. Sentinel.
                                               * Signifies last item in map
                                               */
    };

    enum   eQCWWANError                      eRCode;
    struct QmiAudioSLQSSetM2MAVMuteReq req;

    req.setM2MAVMuteReq = setM2MAVMuteReq;

    eRCode = qmbuild( pParamField,
                      (BYTE *)&req,
                      map,
                      eQMI_SWIAUDIO_SET_AVMUTE,
                      pMlength );
    return eRCode;
}

/*****************************************************************************
 * Response handling
 ******************************************************************************/

/*
 * This function unpacks the SLQSSetM2MAVMute response message to a
 * user-provided response structure.
 *
 * \param     MdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UpkQmiAudioSLQSSetM2MAVMute(
    BYTE                                *pMdmResp,
    struct QmiAudioSLQSSetM2MAVMuteResp *pApiResp)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_SET_M2M_AVMUTE_EC, &qmUnpackTlvResultCode },
        { eTLV_TYPE_INVALID,      NULL } /* Important. Sentinel.
                                          * Signifies last item in map.
                                          */
    };
    eRCode = qmunpackresp( pMdmResp,
                           (BYTE*)pApiResp,
                           map,
                           eQMI_SWIAUDIO_SET_AVMUTE );
    return eRCode;
}
