/*
 * \ingroup ims
 *
 * \file   qaImsSLQSSetRegMgrConfig.c
 *
 * \brief  Contains Packing and UnPacking routines for the
 *         QMI_IMS_SETTINGS_SET_REG_MGR_CFG message.
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
#include "qaGobiApiIms.h"
#include "qaImsSLQSSetSIPConfig.h"
#include "qaImsSLQSSetRegMgrConfig.h"

/*****************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the SLQSSetRegMgrConfig Primary CSCF port to the QMI
 * message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvPriCSCFport(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiImsSLQSSetRegMgrConfigReq *pReq =
                        (struct QmiImsSLQSSetRegMgrConfigReq *)pParam;
    SetRegMgrConfigReq *tReq = pReq->pSetRegMgrConfigReq;

    if( NULL == tReq->pPriCSCFPort )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert the Primary CSCF port into the QMI message */
    return PutWord( pBuf, *(tReq->pPriCSCFPort) );
}

/*
 * This function packs the SLQSSetRegMgrConfig CSCF port name to the QMI
 * message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvCSCFPort(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiImsSLQSSetRegMgrConfigReq *pReq =
                        (struct QmiImsSLQSSetRegMgrConfigReq *)pParam;
    SetRegMgrConfigReq *tReq = pReq->pSetRegMgrConfigReq;

    enum eQCWWANError eRCode  = eQCWWAN_ERR_NONE;
    BYTE              lCtr    = 0;
    BYTE              lStrlen = 0;

    if( NULL == tReq->pCSCFPortName ||
        NULL == tReq->pCSCFPortNameLen )
    {
        return eQCWWAN_ERR_NONE;
    }

    lStrlen  = *(tReq->pCSCFPortNameLen);

    /* Insert the CSCF Port Name into the QMI message */
    for( lCtr = 0; lCtr < lStrlen; lCtr++ )
    {
        eRCode = PutByte( pBuf, tReq->pCSCFPortName[lCtr] );
        if (eQCWWAN_ERR_NONE != eRCode)
        {
            return eRCode;
        }
    }

    return eRCode;
}

/*
 * This function packs the SLQSSetRegMgrConfig IMS Test Mode to the QMI
 * message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvIMSTestMode(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiImsSLQSSetRegMgrConfigReq *pReq =
                        (struct QmiImsSLQSSetRegMgrConfigReq *)pParam;
    SetRegMgrConfigReq *tReq = pReq->pSetRegMgrConfigReq;

    if( NULL == tReq->pIMSTestMode )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert the IMS Test Mode into the QMI message */
    return PutByte( pBuf, *(tReq->pIMSTestMode) );
}

/*
 * This function packs the SLQSSetRegMgrConfig parameters to the QMI message
 * SDU
 *
 * \param  pParamField [OUT] - Pointer to storage into which the packed
 *                             data will be placed by this function.
 *
 * \param  pMlength    [OUT] - Total length of built message.
 *
 * \param  pSetRegMgrConfigReq [IN] - Request parameters for packing.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError PkQmiImsSLQSSetRegMgrConfig(
    WORD               *pMlength,
    BYTE               *pParamField,
    SetRegMgrConfigReq *pSetRegMgrConfigReq )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_REGMGR_CFG_PCSCF_PORT, &BuildTlvPriCSCFport },
        { eTLV_REGMGR_PRI_CSCF,       &BuildTlvCSCFPort },
        { eTLV_IMS_TEST_MODE_ENABLED, &BuildTlvIMSTestMode },
        { eTLV_TYPE_INVALID,          NULL } /* Important. Sentinel.
                                              * Signifies last item in map
                                              */
    };

    struct QmiImsSLQSSetRegMgrConfigReq req;
    enum   eQCWWANError                 eRCode;

    /* Intialize the request structure */
    slmemset( (char *)&req,
              0,
              sizeof(struct QmiImsSLQSSetRegMgrConfigReq) );
    req.pSetRegMgrConfigReq = pSetRegMgrConfigReq;

    eRCode = qmbuild( pParamField,
                      (BYTE *)&req,
                      map,
                      eQMI_IMS_SETTINGS_SET_REG_MGR_CFG,
                      pMlength );
    return eRCode;
}

/*****************************************************************************
 * Response handling
 ******************************************************************************/

/*
 * This function unpacks the SLQSSetRegMgrConfig Settings Resp from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvSettingResp(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiImsSLQSSetRegMgrConfigResp *lResp =
        (struct QmiImsSLQSSetRegMgrConfigResp *)pResp;
    SetRegMgrConfigResp *tResp = lResp->pSetRegMgrConfig;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              bTemp;

    if( NULL == tResp->pSettingResp )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get Settings Response in local variable */
    eRCode = GetByte( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    *(tResp->pSettingResp) = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSSetRegMgrConfig response message to a
 * user-provided response structure.
 *
 * \param     MdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UpkQmiImsSLQSSetRegMgrConfig(
    BYTE                                 *pMdmResp,
    struct QmiImsSLQSSetRegMgrConfigResp *pApiResp)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,   &qmUnpackTlvResultCode },
        { eTLV_SETTINGS_RESP, &UnpackTlvSettingResp },
        { eTLV_TYPE_INVALID,  NULL } /* Important. Sentinel.
                                      * Signifies last item in map.
                                      */
    };
    eRCode = qmunpackresp( pMdmResp,
                           (BYTE*)pApiResp,
                           map,
                           eQMI_IMS_SETTINGS_SET_REG_MGR_CFG );
    return eRCode;
}
