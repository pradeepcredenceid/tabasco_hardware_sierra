/*
 * \ingroup : loc
 *
 * \file    : qaSlqsLocGetServer.c
 *
 * \brief   : Contains Packing and UnPacking routines for the
 *            eQMI_LOC_GET_SERVER message.
 *
 * Copyright: © 2011-2015 Sierra Wireless, Inc. all rights reserved
 */

/* include files */

#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaQmiBasic.h"

#include "qaSlqsLocGetServer.h"

/******************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the LOC GetServer request server type field
 * to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 */
enum eQCWWANError BuildTlvGetServerType( BYTE *pBuf, BYTE *pParam )
{

    LOCGetServerReq *lReq = (LOCGetServerReq *)pParam;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if(lReq==NULL)
        return eQCWWAN_ERR_INVALID_ARG;
    /* Insert server type */
    eRCode = PutLong( pBuf, lReq->serverType );
    return eRCode;
}

/*
 * This function packs the LOC GetServer request address type field
 * to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 */
enum eQCWWANError BuildTlvAddressType( BYTE *pBuf, BYTE *pParam )
{
    LOCGetServerReq *lReq = (LOCGetServerReq *)pParam;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    
    if(lReq==NULL)
        return eQCWWAN_ERR_INVALID_ARG;

    if( NULL == lReq->pAddressType )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert address Type */
    eRCode = PutByte( pBuf, *(lReq->pAddressType));
    return eRCode;
}



/*
 * Packs the PkQmiLocGetServerReq parameters to the QMI message SDU
 *
 * \param  pParamField[OUT]         - Pointer to storage into which the packed
 *                                    data will be placed by this function.
 *
 * \param  pMlength[OUT]            - Total length of built message.
 *
 * \param  pLOCGetServerReq[IN]     - LOC GetServer Request Parameters
 *
 * \return  eQCWWAN_ERR_NONE on success, eQCWWAN_ERR_XXX on error
 *
 */
enum eQCWWANError PkQmiLocGetServerReq(
    WORD                *pMlength,
    BYTE                *pParamField,
    LOCGetServerReq     *pLOCGetServerReq )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_GETSERVER_TYPE,       &BuildTlvGetServerType },
        { eTLV_ADDRESS_TYPE,         &BuildTlvAddressType },
        { eTLV_TYPE_INVALID,         NULL }  /* Important. Sentinel.
                                              * Signifies last item in map.
                                              */
    };
    enum eQCWWANError eRCode;

    eRCode = qmbuild( pParamField,
                      (BYTE *)pLOCGetServerReq,
                      map,
                      eQMI_LOC_GET_SERVER,
                      pMlength );

    return eRCode;
}


/******************************************************************************
 * Response handling
 ******************************************************************************

 * This function unpacks the eQMI_LOC_GET_SERVER response message
 * to a user-provided response structure.
 *
 * \param  pMdmResp  [IN]  - Pointer to packed response from the modem.
 *
 * \param  pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE  - on success;  eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UpkQmiLocGetServerResp(
                  BYTE                         *pMdmResp,
                  struct QmiLocGetServerResp   *pApiResp)
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
                           (BYTE *)pApiResp,
                           map,
                           eQMI_LOC_GET_SERVER );
    return eRCode;
}
