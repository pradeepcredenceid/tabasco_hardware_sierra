/*
 * \ingroup cbk
 *
 * \file    qaCbkImsSLQSRegMgrCfgInd.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaCbkImsSLQSRegMgrCfgInd.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __IMS_REG_MGR_CONFIG_IND_H_
#define __IMS_REG_MGR_CONFIG_IND_H_

#include "qaGobiApiIms.h"
#include "qaGobiApiCbk.h"

/* Constant Defs */
#define MAX_CSCF_PORT_NAME_LEN 255
/*
 * An enumeration of eQMI_IMS_REG_MGR_CONFIG_IND response TLV IDs
 */
enum eQMI_IMS_REG_MGR_CONFIG_IND
{
    eTLV_PRI_PCSCF_PORT = 0x10,
    eTLV_CSCF_PORT_NAME = 0x11,
    eTLV_IMS_TST_MODE   = 0x12,
};

/*
 * eQMI_IMS_REG_MGR_CONFIG_IND TLVs defined below
 */

/*
 * Name:    PCSCFPortTlv
 *
 * Purpose: Structure used to store Primary PCSCF Port TLV Value.
 *
 * Members: TlvPresent  - Boolean indicating the presence of the TLV
 *                        in the QMI response
 *
 *          priCSCFPort - Primary CSCF port
 */
struct PCSCFPortTlv
{
    BYTE TlvPresent;
    WORD priCSCFPort;
};

/*
 * Name:    CSCFPortNameTlv
 *
 * Purpose: Structure used to store CSCF Port Name TLV Value.
 *
 * Members: TlvPresent   - Boolean indicating the presence of the TLV
 *                         in the QMI response
 *
 *          cscfPortName - CSCF Port Name
 */
struct CSCFPortNameTlv
{
    BYTE TlvPresent;
    BYTE cscfPortName[MAX_CSCF_PORT_NAME_LEN];
};

/*
 * Name:    IMSTestModeTlv
 *
 * Purpose: Structure used to store Subscribe Timer TLV Value.
 *
 * Members: TlvPresent  - Boolean indicating the presence of the TLV
 *                        in the QMI response
 *
 *          imsTestMode - IMS Test Mode
 */
struct IMSTestModeTlv
{
    BYTE TlvPresent;
    BYTE imsTestMode;
};

/*
 * Name:    QmiCbkImsRegMgrConfigInd
 *
 * Purpose: Structure used to store Reg Mgr Config Indication Parameters.
 *
 * Members: PCTlv - Pri. PCSCF Port Tlv
 *          PNTlv - CSCF Port Name Tlv
 *          TMTlv - IMS Test Mode Tlv
 *
 * Note:    None
 */
struct QmiCbkImsRegMgrConfigInd{
    struct PCSCFPortTlv    PCTlv;
    struct CSCFPortNameTlv PNTlv;
    struct IMSTestModeTlv  TMTlv;
};

/*
 * Prototypes
 */
enum eQCWWANError UpkQmiCbkImsRegMgrCfgInd(
    BYTE                            *pMdmResp,
    struct QmiCbkImsRegMgrConfigInd *pApiResp );

#endif /* __IMS_REG_MGR_CONFIG_IND_H_ */
