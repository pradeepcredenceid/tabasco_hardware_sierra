/*
 * \ingroup dcs
 *
 * \file    qaDcsSLQSStartSrv
 *
 * \brief   Contains Packing and UnPacking routines for the
 *          eQMI_DCS_START_SRV message.
 *
 * Copyright: © 2011 Sierra Wireless, Inc. all rights reserved
 *
 */
/* include files */

#include "SwiDataTypes.h"
#include "qaQmiBasic.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"

#include "qaDcsSLQSStartSrv.h"
#include "qaGobiApiDcs.h"

/******************************************************************************
 * Request handling
 ******************************************************************************/
/*
 * This function packs the SLQSStartSrv parameters
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
enum eQCWWANError PkQmiDcsSLQSStartSrv(
    WORD *pMlength,
    BYTE *pParamField,
    BYTE action,
    BYTE mask )
{
    return qm_dcs_start_srv_pack( action,
    		                      mask,
                                  pParamField,
                                  pMlength );
}

/******************************************************************************
 * Response handling
 ******************************************************************************/
/*
 * This function unpacks the SLQSStartSrv response message to a
 * user-provided response structure.
 *
 * \param     pMdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp   [OUT] - Pointer to storage to unpack into.
 *
 * \return  eQCWWAN_ERR_NONE
 * \return  eQCWWAN_ERR_MEMORY
 */
enum eQCWWANError  UpkQmiDcsSLQSStartSrv(
    BYTE *pMdmResp,
    struct qm_qmi_response_tlvs_values *pApiResp)
{
    return qm_dcs_start_srv_response_unpack( pMdmResp, pApiResp );
}
