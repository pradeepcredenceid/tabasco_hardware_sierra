/*
 * \ingroup nas
 *
 * \file qaNasSLQSSwiPSDetach.c
 *
 * \brief  Contains Packing and UnPacking routines for the
 *         QMI_NAS_SWI_PS_DETACH message.
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaGobiApiNas.h"
#include "qaNasSLQSSwiPSDetach.h"

/*****************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the PS Detach Action parameter to the QMI message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed
 *                         data will be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError BuildNasTlvPSDetach(
    BYTE *pBuf,
    BYTE *pParam )
{
    PSDetachReq *pReq =
    ((struct QmiNasSlqsSwiPSDetachReq *)pParam)->pPSDetachReq;

    if( NULL == pReq->pDetachAction )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert the Detach Action */
    return PutByte(pBuf, *pReq->pDetachAction );
}

/*
 * This function packs the SLQSSwiPSDetach parameters
 * to the QMI message SDU
 *
 * \param  pParamField [OUT] - Pointer to storage into which the packed
 *                             data will be placed by this function.
 *
 * \param  pMlength    [OUT] - Total length of built message.
 *
 * \param  pPSDetachReq [IN] - structure containing data to be packed.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError PkQmiNasSlqsSwiPSDetach(
    WORD        *pMlength,
    BYTE        *pParamField,
    PSDetachReq *pPSDetachReq )
{
    /* There is no mandatory TLV and hence the map is empty */
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_PS_DETACH_ACTION, &BuildNasTlvPSDetach },
        { eTLV_TYPE_INVALID,     NULL }  /* Important. Sentinel.
                                          * Signifies last item in map.
                                          */
    };

    struct QmiNasSlqsSwiPSDetachReq req;
    slmemset( (char *)&req,
               0,
               sizeof (struct QmiNasSlqsSwiPSDetachReq) );

    req.pPSDetachReq = pPSDetachReq;

    enum eQCWWANError eRCode;
    eRCode = qmbuild( pParamField,
                      (BYTE*)&req,
                      map,
                      eQMI_NAS_SWI_PS_DETACH,
                      pMlength);
    return eRCode;
}

/*****************************************************************************
 * Response handling
 ******************************************************************************/

/*
 * This function unpacks the SlqsSwiPSDetach response message to to a
 * user-provided response structure.
 *
 * \param     pMdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return    eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UpkQmiNasSlqsSwiPSDetach(
    BYTE                             *pMdmResp,
    struct QmiNasSlqsSwiPSDetachResp *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,  &qmUnpackTlvResultCode },
        { eTLV_TYPE_INVALID, NULL }  /* Important. Sentinel.
                                      * Signifies last item in map.
                                      */
    };
    eRCode = qmunpackresp( pMdmResp,
                          (BYTE*)pApiResp,
                          map,
                          eQMI_NAS_SWI_PS_DETACH );
    return eRCode;
}
