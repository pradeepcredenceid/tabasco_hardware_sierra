/*
 * \ingroup dcs
 *
 * \file    qaDcsSLQSQosMap
 *
 * \brief   Contains Packing and UnPacking routines for the
 *          eQMI_DCS_QOS_MAP message.
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */
/* include files */

#include "SwiDataTypes.h"
#include "qaQmiBasic.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"

#include "qaDcsSLQSQosMap.h"
#include "qaGobiApiDcs.h"

/******************************************************************************
 * Request handling
 ******************************************************************************/
/*
 * This function packs the SLQSQosMap parameters
 * to the QMI message SDU
 *
 * \param   pParamField [OUT] - Pointer to storage into which the packed
 *                              data will be placed by this function.
 *
 * \param   pMlength    [OUT] - Total length of built message.
 *
 * \return  eQCWWAN_ERR_NONE
 * \return  eQCWWAN_ERR_MEMORY
 *
 * \sa      qaGobiApiDcs.h for remaining parameter descriptions.
 *
 */
enum eQCWWANError PkQmiDcsSLQSQosMap(
    WORD *pMlength,
    BYTE *pParamField,
    BYTE dscp,
    ULONG qos_id )
{
    return qm_dcs_qos_map_pack( dscp,
    		                      qos_id,
                                  pParamField,
                                  pMlength );
}

/******************************************************************************
 * Response handling
 ******************************************************************************/
/*
 * This function unpacks the SLQSQosMap response message to a
 * user-provided response structure.
 *
 * \param     pMdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp   [OUT] - Pointer to storage to unpack into.
 *
 * \return  eQCWWAN_ERR_NONE
 * \return  eQCWWAN_ERR_MEMORY
 */
enum eQCWWANError  UpkQmiDcsSLQSQosMap(
    BYTE *pMdmResp,
    struct qm_qmi_response_tlvs_values *pApiResp)
{
    return qm_dcs_qos_map_response_unpack( pMdmResp, pApiResp );
}

