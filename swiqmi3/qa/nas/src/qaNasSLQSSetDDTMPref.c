/*
 * \ingroup : nas
 *
 * \file    : qaNasSLQSSetDDTMPref.c
 *
 * \brief   : Contains Packing and UnPacking routines for the
 *            eQMI_NAS_SET_DDTM_PREFERENCE message.
 *
 * Copyright: © 2011-2014 Sierra Wireless, Inc. all rights reserved
 */

/* include files */

#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaGobiApiNas.h"
#include "qaNasSLQSSetDDTMPref.h"

/******************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the SLQSSetDDTMPref DDTMPreference field to the
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
enum eQCWWANError BuildTlvDDTMPref( BYTE *pBuf, BYTE *pParam )
{
    struct QmiNasSLQSSetDDTMPrefReq *pReq =
                (struct QmiNasSLQSSetDDTMPrefReq *)pParam;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE Index;

    /* Add TLV data */
    // DDTM preference setting
    eRCode = PutByte( pBuf, pReq->pDDTMPref->DDTMPrefSetting );
    if ( eQCWWAN_ERR_NONE != eRCode )
        return eRCode;
    // DDTM Action
    eRCode = PutWord( pBuf, pReq->pDDTMPref->DDTMAction );
    if ( eQCWWAN_ERR_NONE != eRCode )
        return eRCode;
    // SO List Action
    eRCode = PutByte( pBuf, pReq->pDDTMPref->SOListAction );
    if ( eQCWWAN_ERR_NONE != eRCode )
        return eRCode;
    // SO Length
    eRCode = PutByte( pBuf, pReq->pDDTMPref->SOLen );
    if ( eQCWWAN_ERR_NONE != eRCode )
        return eRCode;
    // Service Option
    for( Index=0; Index < pReq->pDDTMPref->SOLen; Index++)
    {
        eRCode = PutWord( pBuf, pReq->pDDTMPref->pServiceOption[Index] );
        if ( eQCWWAN_ERR_NONE != eRCode )
            return eRCode;
    }

    return eRCode;
}

/*
 * This function packs the SLQSSetDDTMPref request parameters to the QMI
 * message SDU
 *
 * \param  pMlength     [OUT] - Total length of built message.
 *
 * \param  pParamField  [OUT] - Pointer to storage into which the packed
 *                              data will be placed by this function.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 * \sa qaGobiNpiNas.h for remaining parameter descriptions.
 */
enum eQCWWANError PkQmiNasSLQSSetDDTMPref (
    WORD *pMlength,
    BYTE *pParamField,
    struct DDTMPreference *pDDTMPref )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_DDTM_PREF,     &BuildTlvDDTMPref },
        { eTLV_TYPE_INVALID,  NULL } /* Important. Sentinel.
                                      * Signifies last item in map
                                      */
    };

    struct QmiNasSLQSSetDDTMPrefReq req;

        enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

        slmemset((CHAR *)&req, 0, sizeof(struct QmiNasSLQSSetDDTMPrefReq));
        req.pDDTMPref = pDDTMPref;

        eRCode = qmbuild( pParamField,
                          (BYTE *)&req,
                          map,
                          eQMI_NAS_SET_DDTM_PREF,
                          pMlength );
        return eRCode;
}

/******************************************************************************
 * Response handling
 ******************************************************************************/

/*
 * This function unpacks the SLQSSetDDTMPref response message to a
 * user-provided response structure.
 *
 * \param  pMdmResp           - Pointer to packed response from the modem.
 *
 * \param  pApiResp           - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UpkQmiNasSLQSSetDDTMPref(
    BYTE *pMdmResp,
    struct QmiNasSLQSSetDDTMPrefResp *pApiResp )
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
                           eQMI_NAS_SET_DDTM_PREF );
    return eRCode;
}
