/*
 * \ingroup uim
 *
 * \file    qaUimSLQSUimGetConfiguration.c
 *
 * \brief   Contains Packing and UnPacking routines for the
 *          QMI_UIM_GET_CONFIGURATION message.
 *
 * Copyright: © 2016 Sierra Wireless Inc., all rights reserved
 *
 */

/* include files */
#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaUimSLQSUimGetConfiguration.h"
#include "qaGobiApiUimCommon.h"

/******************************************************************************
 * Request handling
 ******************************************************************************/
/*
 * This function packs the Response Indication parameter to the QMI message SDU.
 *
 * \param  pBuf   [OUT] - Pointer to storage into which the packed
 *                        data will be placed by this function.
 *
 * \param  pParam [IN]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError BuildTlvConfigMaskReq( BYTE *pBuf, BYTE *pParam )
{
    UIMGetConfigurationReq *pReq = ( UIMGetConfigurationReq *)pParam;

    /* Check For Invalid Parameter */
    if ( NULL == pReq->pConfigurationMask )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert indication token */
    return PutLong( pBuf, *pReq->pConfigurationMask );
}

/*
 * This function packs the SLQSUIMGetConfiguration request parameters to the QMI message SDU.
 *
 * \param  pMlength     [OUT] - Total length of built message.
 *
 * \param  pParamField  [OUT] - Pointer to storage into which the packed
 *                              data will be placed by this function.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 * \sa qaUimSLQSUimGetConfiguration.h for remaining parameter descriptions.
 */
enum eQCWWANError PkQmiUimSlqsGetConfiguration(
    WORD                    *pMlength,
    BYTE                    *pBuffer,
    UIMGetConfigurationReq  *pUIMGetConfigurationReq)
{
    /* There is no mandatory TLV and hence the map is empty */
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_CONFIG_MASK_REQ,      &BuildTlvConfigMaskReq },
        { eTLV_TYPE_INVALID,         NULL } /* Important. Sentinel.
                                             * Signifies last item in map
                                             */
    };

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    eRCode = qmbuild( pBuffer,
                      (BYTE *)pUIMGetConfigurationReq,
                      map,
                      eQMI_UIM_GET_CONFIGURATION,
                      pMlength );
    return eRCode;
}

/******************************************************************************
 * Response handling
 ******************************************************************************/
/*
 * This function unpacks the Automatic Selection from the QMI response
 * message to a user provided response structure
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvAutoSelection(
    BYTE *pTlvData,
    BYTE *pResp )
{
    UIMGetConfigurationResp *lResp =
    ((struct QmiUimSLQSGetConfigurationResp *)pResp)->pUIMGetConfigurationResp;

    /* Check For Invalid Parameter */
    if ( NULL == lResp->pAutoSelection )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Copy the automatic selection into the List */
    return GetByte( pTlvData, lResp->pAutoSelection );
}

/*
 * This function unpacks the Personalization Status from the QMI response
 * message to a user provided response structure
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */

enum eQCWWANError UnpackTlvPersStatus( BYTE *pTlvData, BYTE *pResp )
{
    UIMGetConfigurationResp *lResp =
    ((struct QmiUimSLQSGetConfigurationResp *)pResp)->pUIMGetConfigurationResp;

    personalizationStatus *pPersonalizationStatus = lResp->pPersonalizationStatus;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              Ctr;
    BYTE              lTemp1;

    /* Check For Invalid Parameter */
    if ( NULL == lResp->pPersonalizationStatus)
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Extract the parameters into the structure */
    /* num features: Byte */
    eRCode = GetByte( pTlvData, &pPersonalizationStatus->numFeatures );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    lTemp1 = pPersonalizationStatus->numFeatures;
    for( Ctr = 0; Ctr < lTemp1; Ctr++ )
    {
        /* feature: Byte */
        eRCode = GetByte( pTlvData, &pPersonalizationStatus->feature[Ctr] );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }

        /* verifyLeft: Byte */
        eRCode = GetByte( pTlvData, &pPersonalizationStatus->verifyLeft[Ctr] );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }

        /* unblockLeft: Byte */
        eRCode = GetByte( pTlvData, &pPersonalizationStatus->unblockLeft[Ctr] );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }

    }

    return eRCode;
}
/*
 * This function unpacks the Halt Subscription from the QMI response
 * message to a user provided response structure
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvHaltSubscription(
    BYTE *pTlvData,
    BYTE *pResp )
{
    UIMGetConfigurationResp *lResp =
    ((struct QmiUimSLQSGetConfigurationResp *)pResp)->pUIMGetConfigurationResp;

    /* Check For Invalid Parameter */
    if ( NULL == lResp->pHaltSubscription )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Copy the halt subscription into the List */
    return GetByte( pTlvData, lResp->pHaltSubscription );
}

/*
 * This function unpacks the SLQSUIMGetConfiguration response message to a
 * user-provided response structure.
 *
 * \param  pMdmResp           - Pointer to packed response from the modem.
 *
 * \param  pApiResp           - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UpkQmiUimSlqsGetConfiguration(
    BYTE                                   *pMdmResp,
    struct QmiUimSLQSGetConfigurationResp  *pApiResp )
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,                &qmUnpackTlvResultCode },
        { eTLV_GET_AUTO_SELECTION,         &UnpackTlvAutoSelection },
        { eTLV_GET_PERS_STATUS,            &UnpackTlvPersStatus },
        { eTLV_GET_HALT_SUBSCRIPTION,      &UnpackTlvHaltSubscription},
        { eTLV_TYPE_INVALID,                NULL } /* Important. Sentinel.
                                             * Signifies last item in map.
                                             */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_UIM_GET_CONFIGURATION);
    return eRCode;
}


