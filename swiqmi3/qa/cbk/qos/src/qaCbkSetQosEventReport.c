/*
 *
 * \ingroup : cbk
 *
 * \file    : qaSetQosEventReport.c
 *
 * \brief   : Contains Packing and UnPacking routines for the
 *            QOS_SET_EVENT_REPORT message.
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaCbkSetQosEventReport.h"

/* Functions */

/* Request handlers */

/*
 * This function packs the Global Flow info reporting field to the QMI message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed
 *                         data will be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE    - Increment successful
 * \return eQCWWAN_ERR_MEMORY   - Increment beyond allowed size attempted
 *
 */
local enum eQCWWANError BuildTlvGloFlowReport(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiCbkSetQosEventReportReq *pReq =
        (struct QmiCbkSetQosEventReportReq *)pParam;

    /* If the parameter has not been set do not fill the TLV */
    if (QMI_CBK_PARAM_NOCHANGE == pReq->eGlobalFlow)
        return eQCWWAN_ERR_NONE;

    return PutByte ( pBuf, pReq->eGlobalFlow);
}

/*
 * This function packs the Total Byte and Packet Count field to the QMI message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed
 *                         data will be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE    - Increment successful
 * \return eQCWWAN_ERR_MEMORY   - Increment beyond allowed size attempted
 *
 */
local enum eQCWWANError BuildTlvQosProfChange(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiCbkSetQosEventReportReq *pReq =
        (struct QmiCbkSetQosEventReportReq *)pParam;
    WORD extTechPref;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* If the parameter has not been set do not fill the TLV */
    if( QMI_CBK_PARAM_NOCHANGE == pReq->eQosProfChange )
        return eRCode;

    /* fill in the enable/disenable value */
    eRCode = PutByte ( pBuf, pReq->eQosProfChange);
    if( eQCWWAN_ERR_NONE != eRCode )
        return eRCode;

    /* Insert extend technology preference */
    if( QMI_CBK_PARAM_SET == pReq->eQosProfChange )
        extTechPref = EXTEND_TECHNOLOGY_PREFERENCE_CDMA;
    else if( QMI_CBK_PARAM_RESET == pReq->eQosProfChange )
        extTechPref = 0;

    eRCode = PutWord ( pBuf, extTechPref );
    return eRCode;
}

/*
 * This function packs the QOS Set Event Report parameters to the
 * QMI message SDU
 *
 * \param  pMlength[OUT]
 *         - Total length of built message.
 *
 * \param  eGlobalFlow[IN]
 *         - Global flow info indication
 *
 * \param  eQosProfChange[IN]
 *         - Network supported QoS Profile change Indicator
 *
 * \param  pParamField[OUT]
 *         - Pointer to storage into which the packed
 *         data will be placed by this function.
 *
 * \return eQCWWAN_ERR_NONE   - Increment successful
 * \return eQCWWAN_ERR_MEMORY - Increment beyond allowed size attempted
 *
 */
package enum eQCWWANError PkQmiCbkSetQosEventReport(
    WORD                   *pMlength,
    enum eQmiCbkSetStatus  eGlobalFlow,
    enum eQmiCbkSetStatus  eQosProfChange,
    BYTE                   *pParamField )
{
    static struct qmTlvBuilderItem map[] =
    {
         { eTLV_GLOBAL_FLOW_REPORT, BuildTlvGloFlowReport },
         { eTLV_NETWORK_SUPPORTED_QOS_PROFILE_CHANGE,      BuildTlvQosProfChange },
         { eTLV_TYPE_INVALID,             NULL }  /* Important. Sentinel.
                                                   * Signifies last item in map.
                                                   */
    };

    struct QmiCbkSetQosEventReportReq req;
    slmemset( (char *)&req,
              0,
              sizeof (struct QmiCbkSetQosEventReportReq) );

    req.eGlobalFlow        = eGlobalFlow;
    req.eQosProfChange    = eQosProfChange;

    enum eQCWWANError eRCode;

    eRCode = qmbuild( pParamField,
                      (BYTE *)&req,
                      map,
                      eQMI_QOS_SET_EVENT,
                      pMlength );
    return eRCode;
}

/* Response handlers */

/*
 *
 * This function unpacks the WDS_SET_EVENT_REPORT response
 * message to a user-provided response structure.
 *
 * \param     pMdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp   [OUT] - Pointer to storage to unpack into.
 *
 * \return    eQCWWAN_ERR_NONE, on success
 *
 */
package enum eQCWWANError UpkQmiCbkSetQosEventReport(
    BYTE    *pMdmResp,
    struct  QmiCbkSetQosEventReportResp *pApiResp)
{
    enum eQCWWANError eRCode;

    /* The unpack of the Callback contains only result code */
    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE       ,&qmUnpackTlvResultCode },
        { eTLV_TYPE_INVALID      ,NULL }  /* Important. Sentinel.
                                           * Signifies last item in map.
                                           */
    };
    eRCode =  qmunpackresp( pMdmResp,
                           (BYTE*)pApiResp,
                           map,
                           eQMI_QOS_SET_EVENT);
    return eRCode;
}

