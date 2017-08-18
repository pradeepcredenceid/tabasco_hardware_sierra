/*
 * \ingroup dms
 *
 * \file    qaDmsSLQSSwiGetFirmwareCurr.c
 *
 * \brief   Contains Packing and UnPacking routines for the
 *          QMI_DMS_SWI_GET_FIRMWARE_CURR message.
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaDmsSLQSSwiGetFirmwareCurr.h"

/******************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the Get Current Firmware request parameters to the QMI
 * message SDU
 *
 * \param  pMlength     [OUT] - Total length of built message.
 *
 * \param  pParamField  [OUT] - Pointer to storage into which the packed
 *                              data will be placed by this function.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 * \sa qaGobiApiDms.h for remaining parameter descriptions.
 */
enum eQCWWANError PkQmiDmsSLQSSwiGetCurrFw(
    WORD *pMlength,
    BYTE *pParamField )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_TYPE_INVALID,   NULL } /* Important. Sentinel.
                                       * Signifies last item in map
                                       */
    };

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    eRCode = qmbuild( pParamField,
                      NULL,
                      map,
                      eQMI_DMS_SWI_GET_FIRMWARE_CURR,
                      pMlength );
    return eRCode;
}

/******************************************************************************
 * Response handling
 ******************************************************************************/
/*
 * This function unpacks the current image list from the QMI response
 * message to a user provided response structure
 *
 * \param pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param pResp    - Pointer to structure containing storage
 *                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE
 */
enum eQCWWANError UnpackTlvCurrImgList(BYTE *pTlvData, BYTE *pResp)
{
    CurrentImgList *lResp =
        ((struct QmiDmsGetFirmwareCurrResp *)pResp)->pCurrentImgList;

    enum eQCWWANError eRCode    = eQCWWAN_ERR_NONE;
    BYTE              lTemp     = 0;
    BYTE              lCtr      = 0;
    BYTE              ImgListSz = 0;
    BYTE              bldIDLen  = 0;
    BYTE              bTemp[UNIQUE_ID_LEN];
    BYTE              StrCtr    = 0;

    /* Get the number of entries in the image list */
    eRCode = GetByte( pTlvData, &lTemp );
    if( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    /* Check for buffer undersize error */
    if( lResp->numEntries < lTemp)
    {
        return eQCWWAN_ERR_BUFFER_SZ;
    }
    lResp->numEntries = lTemp;
    ImgListSz = lTemp;

    for( lCtr = 0; lCtr < ImgListSz; lCtr++ )
    {
        CurrImageInfo *pCurrImg = &lResp->pCurrImgInfo[lCtr];

        /* Extract Image Type */
        eRCode = GetByte( pTlvData, &lTemp );
        if( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
        pCurrImg->imageType = lTemp;

        /* Extract unique ID */
        for( StrCtr = 0; StrCtr < (BYTE)UNIQUE_ID_LEN; StrCtr++ )
        {
            eRCode = GetByte( pTlvData, &bTemp[StrCtr] );
            if( eQCWWAN_ERR_NONE != eRCode )
            {
                return eRCode;
            }
            pCurrImg->uniqueID[StrCtr] = bTemp[StrCtr];
        }


        /* Extract build ID Length */
        eRCode = GetByte( pTlvData, &lTemp );
        if( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
        pCurrImg->buildIDLen = lTemp;

        /* Extract build ID */
        bldIDLen = lTemp;
        for( StrCtr = 0; StrCtr < bldIDLen; StrCtr++ )
        {
            eRCode = GetByte( pTlvData, &lTemp );
            if( eQCWWAN_ERR_NONE != eRCode )
            {
                return eRCode;
            }
            pCurrImg->buildID[StrCtr] = lTemp;
        }
    }
    return eRCode;
}

/*
 * This function unpacks the get firmware current response message to a user
 * provided response structure.
 *
 * \param  pMdmResp - Pointer to packed response from the modem.
 *
 * \param  pApiResp - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UpkQmiDmsSLQSSwiGetCurrFw(
    BYTE                             *pMdmResp,
    struct QmiDmsGetFirmwareCurrResp *pApiResp )
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,   &qmUnpackTlvResultCode },
        { eTLV_CURR_IMG_LIST, &UnpackTlvCurrImgList },
        { eTLV_TYPE_INVALID,  NULL } /* Important. Sentinel.
                                      * Signifies last item in map.
                                      */
    };

    /*  */
    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_DMS_SWI_GET_FIRMWARE_CURR );
    return eRCode;
}
