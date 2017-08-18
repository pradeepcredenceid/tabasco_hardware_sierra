/*
 * \ingroup cbk
 *
 * \file qaDmsSwiSetEventReport.c
 *
 * \brief Contains Packing and UnPacking routines for the
 *        DMS_SWI_SET_EVENT_REPORT message.
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaCbkDmsSwiSetEventReport.h"

/* Functions */

/* Request handlers */

/*
 * This function packs the Temperature field to the QMI message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed
 *                         data will be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE   - Increment successful
 * \return eQCWWAN_ERR_MEMORY - Access beyond allowed size attempted
 *
 */
local enum eQCWWANError BuildTlvTemp(
    BYTE *pBuf,
    BYTE *pParam )
{
    struct QmiCbkDmsSwiSetEventReportReq *pReq =
        (struct QmiCbkDmsSwiSetEventReportReq *)pParam;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* If the parameter has not been set do not fill the TLV */
    if (QMI_CBK_PARAM_NOCHANGE == pReq->eTemperature)
        return eRCode;

    eRCode = PutByte( pBuf, pReq->eTemperature );
    return eRCode;
}

/*
 * This function packs the Voltage field to the QMI message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed
 *                         data will be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE   - Increment successful
 * \return eQCWWAN_ERR_MEMORY - Access beyond allowed size attempted
 *
 */
local enum eQCWWANError BuildTlvVoltage(
    BYTE *pBuf,
    BYTE *pParam )
{
    struct QmiCbkDmsSwiSetEventReportReq *pReq =
        (struct QmiCbkDmsSwiSetEventReportReq *)pParam;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* If the parameter has not been set do not fill the TLV */
    if (QMI_CBK_PARAM_NOCHANGE == pReq->eVoltage)
        return eRCode;

    eRCode = PutByte( pBuf, pReq->eVoltage );
    return eRCode;
}

/*
 * This function packs the DMS Set Event Report parameters to the
 * QMI message SDU
 *
 * \param  pParamField [OUT] - Pointer to storage into which the packed
 *                             data will be placed by this function.
 *
 * \param  pMlength    [OUT] - Total length of built message.
 *
 * \param  ePowerState   [IN] - Set/Reset
 *
 * \param  eBatteryLevel [IN] - Set/Reset
 *
 * \return eQCWWAN_ERR_NONE   - Increment successful
 * \return eQCWWAN_ERR_MEMORY - Access beyond allowed size attempted
 *
 */
package enum eQCWWANError PkQmiCbkDmsSwiSetEventReport(
    WORD                  *pMlength,
    enum eQmiCbkSetStatus eTemperature,
    enum eQmiCbkSetStatus eVoltage,
    BYTE                  *pParamField )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_TEMPERATURE,  &BuildTlvTemp },
        { eTLV_VOLTAGE,      &BuildTlvVoltage },
        { eTLV_TYPE_INVALID, NULL } /* Important. Sentinel.
                                     * Signifies last item in map.
                                     */
    };

    struct QmiCbkDmsSwiSetEventReportReq req;
    slmemset( (CHAR *)&req,
              0,
              sizeof (struct QmiCbkDmsSwiSetEventReportReq) );

    req.eTemperature = eTemperature;
    req.eVoltage     = eVoltage;

    enum eQCWWANError eRCode;

    eRCode = qmbuild( pParamField,
                      (BYTE *)&req,
                      map,
                      eQMI_DMS_SWI_SET_EVENT_REPORT,
                      pMlength );

    return eRCode;
}

/* Response handlers */

/*
 * This function unpacks the DMS_SWI_SET_EVENT_REPORT response
 * message to a user-provided response structure.
 *
 * \param  pMdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param  pApiResp   [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE   - Increment successful
 * \return eQCWWAN_ERR_MEMORY - Access beyond allowed size attempted
 *
 */
package enum eQCWWANError UpkQmiCbkDmsSwiSetEventReport(
    BYTE                                  *pMdmResp,
    struct QmiCbkDmsSwiSetEventReportResp *pApiResp )
{
    ULONG eRCode;

    /* The unpack of the Callback contains only result code */
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
                           eQMI_DMS_SWI_SET_EVENT_REPORT );
    return eRCode;
}

