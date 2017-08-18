/*
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 */
/* include files */

#include "SwiDataTypes.h"
#include "qaQmiBasic.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"

#include "qaDcsSLQSQosReadMap.h"
#include "qaGobiApiDcs.h"

enum eQCWWANError PkQmiDcsSLQSQosReadMap(
    WORD *pMlength,
    BYTE *pParamField,
    BYTE dscp)
{
    return qm_dcs_qos_read_map_pack( dscp,
                                  pParamField,
                                  pMlength );
}

enum eQCWWANError  UpkQmiDcsSLQSQosReadMap(
    BYTE *pMdmResp,
    struct qm_qmi_response_tlvs_values *pApiResp)
{
    return qm_dcs_qos_read_map_response_unpack( pMdmResp, pApiResp );
}



