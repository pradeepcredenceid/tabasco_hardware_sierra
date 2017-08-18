/*
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 */
/* include files */

#include "SwiDataTypes.h"
#include "qaQmiBasic.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"

#include "qaDcsSLQSQosEditMap.h"
#include "qaGobiApiDcs.h"

enum eQCWWANError PkQmiDcsSLQSQosEditMap(
    WORD *pMlength,
    BYTE *pParamField,
    BYTE dscp,
    ULONG qos_id )
{
    return qm_dcs_qos_edit_map_pack( dscp,
    		                      qos_id,
                                  pParamField,
                                  pMlength );
}

enum eQCWWANError  UpkQmiDcsSLQSQosEditMap(
    BYTE *pMdmResp,
    struct qm_qmi_response_tlvs_values *pApiResp)
{
    return qm_dcs_qos_edit_map_response_unpack( pMdmResp, pApiResp );
}


