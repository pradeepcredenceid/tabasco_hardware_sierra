/*
 * \ingroup swioma
 *
 * \file    qaSwiOmaDmSetSettingsExt.c
 *
 * \brief   Contains Packing and UnPacking routines for the
 *          QMI_SWIOMA_DM_SET_SETTINGS message.
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaSwiOmaDmSetSettingsExt.h"

/******************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the SLQSOMADMSetSettingsExt FOTA Download field to the
 * QMI message SDU
 *
 * \param  pBuf   [OUT] - Pointer to storage into which the packed
 *                        data will be placed by this function.
 *
 * \param  pParam [IN]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError BuildTlvFOTADownloadExt( BYTE *pBuf, BYTE *pParam )
{
    SLQSOMADMSettingsReqParamsExt *pReq = (SLQSOMADMSettingsReqParamsExt *)pParam;

    /* Add TLV data */
    return PutByte( pBuf, pReq->FOTAdownload );
}

/*
 * This function packs the SLQSOMADMSetSettings FOTA Update field to the
 * QMI message SDU
 *
 * \param  pBuf   [OUT] - Pointer to storage into which the packed
 *                        data will be placed by this function.
 *
 * \param  pParam [IN]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError BuildTlvFOTAUpdateExt( BYTE *pBuf, BYTE *pParam )
{
    SLQSOMADMSettingsReqParamsExt *pReq = (SLQSOMADMSettingsReqParamsExt *)pParam;

    /* Add TLV data */
    return PutByte( pBuf, pReq->FOTAUpdate );
}

enum eQCWWANError BuildTlvOmaDmEnableExt( BYTE *pBuf, BYTE *pParam )
{
    SLQSOMADMSettingsReqParamsExt *pReq = (SLQSOMADMSettingsReqParamsExt *)pParam;

    /* Add TLV data */
    return PutByte( pBuf, pReq->OMADMEnable );
}

enum eQCWWANError BuildTlvOmaDmLogEnableExt( BYTE *pBuf, BYTE *pParam )
{
    SLQSOMADMSettingsReqParamsExt *pReq = (SLQSOMADMSettingsReqParamsExt *)pParam;

    /* Add TLV data */
    return PutByte( pBuf, pReq->OMADMLogEnable );
}

enum eQCWWANError BuildTlvOmaDmFumoEnableExt( BYTE *pBuf, BYTE *pParam )
{
    SLQSOMADMSettingsReqParamsExt *pReq = (SLQSOMADMSettingsReqParamsExt *)pParam;

    /* Add TLV data */
    return PutByte( pBuf, pReq->FUMOEnable );
}

enum eQCWWANError BuildTlvPRLEnableExt( BYTE *pBuf, BYTE *pParam )
{
    SLQSOMADMSettingsReqParamsExt *pReq = (SLQSOMADMSettingsReqParamsExt *)pParam;

    /* Add TLV data */
    return PutByte( pBuf, pReq->PRLEnable );
}


/*
 * This function packs the SLQSOMADMSetSettings parameters to the QMI
 * message SDU
 *
 * \param  pMlength     [OUT] - Total length of built message.
 *
 * \param  pParamField  [OUT] - Pointer to storage into which the packed
 *                              data will be placed by this function.
 *
 * \param  pReq         [OUT] - Pointer to the structure containing the data
 *                              to be packed.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 * \sa qaGobiApiSwiOmadms.h for remaining parameter descriptions.
 */
enum eQCWWANError PkQmiSwiOmaDmSetSettingsExt(
    WORD                          *pMlength,
    BYTE                          *pParamField,
    SLQSOMADMSettingsReqParamsExt *pReq )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_SET_FOTA_AUTOMATIC_DOWNLOAD_EXT, &BuildTlvFOTADownloadExt },
        { eTLV_SET_FOTA_AUTOMATIC_UPDATE_EXT,   &BuildTlvFOTAUpdateExt },
        { eTLV_SET_OMADM_ENABLE_EXT,            &BuildTlvOmaDmEnableExt },
	{ eTLV_SET_OMADM_LOG_ENABLE_EXT,        &BuildTlvOmaDmLogEnableExt },
	{ eTLV_SET_OMADM_FUMO_ENABLE_EXT,       &BuildTlvOmaDmFumoEnableExt },
	{ eTLV_SET_OMADM_PRL_ENABLE_EXT,        &BuildTlvPRLEnableExt },
        { eTLV_TYPE_INVALID,                    NULL } /* Important. Sentinel.
                                                        * Signifies last item in map
                                                        */
    };

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    eRCode = qmbuild( pParamField,
                      (BYTE *)pReq,
                      map,
                      eQMI_SWIOMA_SET_SETTINGS,
                      pMlength );
    return eRCode;
}

/******************************************************************************
 * Response handling
 ******************************************************************************/

/*
 * This function unpacks the SLQSOMADMSetSettings response message to a
 * user-provided response structure.
 *
 * \param  pMdmResp           - Pointer to packed response from the modem.
 *
 * \param  pApiResp           - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UpkQmiSwiOmaDmSetSettingsExt(
    BYTE                                 *pMdmResp,
    struct QmiSwiOmaDmSetSettingsRespExt *pApiResp )
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,  &qmUnpackTlvResultCode },
        { eTLV_TYPE_INVALID, NULL } /* Important. Sentinel.
                                     * Signifies last item in map.
                                     */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_SWIOMA_SET_SETTINGS );
    return eRCode;
}
