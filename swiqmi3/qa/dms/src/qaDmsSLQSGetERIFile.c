/*
 * \ingroup dms
 *
 * \file    qaDmsSLQSGetERIFile.c
 *
 * \brief   Contains Packing and UnPacking routines for the
 *          eQMI_DMS_READ_ERI_FILE message.
 *
 * Copyright: © 2012 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaGobiApiDms.h"
#include "qaDmsSLQSGetERIFile.h"

/*****************************************************************************
 * Request handling
 *****************************************************************************/
/*
 * This function packs the SLQS Get ERI File request parameters to
 * the QMI message SDU
 *
 * \param  pMlength     [OUT] - Total length of built message.
 *
 * \param  pParamField  [OUT] - Pointer to storage into which the packed data
 *                              will be placed by this function.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError PkQmiDmsSLQSGetERIFile(
    WORD *pMlength,
    BYTE *pParamField )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_TYPE_INVALID, NULL }/* Important. Sentinel.
                                    * Signifies last item in map
                                    */
    };

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    eRCode =  qmbuild( pParamField,
                       NULL,
                       map,
                       eQMI_DMS_READ_ERI_FILE,
                       pMlength );
    return eRCode;
}

/*****************************************************************************
 * Response handling
 *****************************************************************************/
/*
 * Unpacks the ERI File from the QMI response message to a user
 * provided response structure
 *
 * \param       pTlvData [IN/OUT] - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]    - Pointer to structure containing storage
 *                                  to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvERIFile( BYTE *pTlvData, BYTE *pResp )
{
    ERIFileparams *lResp =
        ( ( struct QmiDmsSLQSGetERIFileResp * )pResp )->pERIFileparams;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    WORD lTemp;
    WORD lCtr = 0;

    /* Check For Invalid Parameter */
    if ( NULL == lResp->pFileSize ||
         NULL == lResp->pFile )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Extract the parameters into the structure */
    eRCode = GetWord( pTlvData, &lTemp );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    /* Check if the user supplied file size is large enough */
    if( lTemp > *(lResp->pFileSize) )
    {
        return eQCWWAN_ERR_BUFFER_SZ;
    }

    /* Fill the response structure */
    *(lResp->pFileSize) = lTemp;
    for( lCtr = 0; lCtr< lTemp; lCtr++ )
    {
        eRCode = GetByte( pTlvData, &lResp->pFile[lCtr] );
        if( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
    }
    return eRCode;
}

/*
 * This function unpacks the SLQSGetERIFile response to a user-provided
 * response structure.
 *
 * \param  pMdmResp           - Pointer to packed response from the modem.
 *
 * \param  pApiResp           - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UpkQmiDmsSLQSGetERIFile(
    BYTE                            *pMdmResp,
    struct QmiDmsSLQSGetERIFileResp *pApiResp )
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,        &qmUnpackTlvResultCode },
        { eTLV_DMS_ERI_FILE,       &UnpackTlvERIFile },
        { eTLV_TYPE_INVALID,       NULL } /* Important. Sentinel.
                                           * Signifies last item in map.
                                           */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_DMS_READ_ERI_FILE );
    return eRCode;
}
