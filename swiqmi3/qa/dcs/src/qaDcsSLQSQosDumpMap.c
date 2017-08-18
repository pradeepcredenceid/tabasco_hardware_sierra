/*
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 */
/* include files */

#include "SwiDataTypes.h"
#include "qaQmiBasic.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"

#include "qaDcsSLQSQosDumpMap.h"
#include "qaGobiApiDcs.h"

enum eQCWWANError PkQmiDcsSLQSQosDumpMap(
    WORD *pMlength,
    BYTE *pParamField)
{
    return qm_dcs_qos_dump_map_pack( pParamField,
                                  pMlength );
}

enum eQCWWANError  UpkQmiDcsSLQSQosDumpMap(
    BYTE *pMdmResp,
    struct qm_qmi_response_tlvs_values *pApiResp)
{
    return qm_dcs_qos_dump_map_response_unpack( pMdmResp, pApiResp );
}




