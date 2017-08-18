/*
 * \ingroup cbk
 *
 * \file    qaCbkImsSLQSVoIPCfgInd.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaCbkImsSLQSVoIPCfgInd.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __IMS_VOIP_CONFIG_IND_H_
#define __IMS_VOIP_CONFIG_IND_H_

#include "qaGobiApiIms.h"
#include "qaGobiApiCbk.h"

/*
 * An enumeration of eQMI_IMS_VOIP_CONFIG_IND response TLV IDs
 */
enum eQMI_IMS_VOIP_CONFIG_IND
{
    eTLV_SESS_EXP_TMR     = 0x10,
    eTLV_MN_SESS_EXP      = 0x11,
    eTLV_AMR_WB_EN        = 0x12,
    eTLV_SCR_AMR_EN       = 0x13,
    eTLV_SCR_AMR_WB_EN    = 0x14,
    eTLV_AMR_MOD          = 0x15,
    eTLV_AMR_WB_MOD       = 0x16,
    eTLV_AMR_OCT_ALIGN    = 0x17,
    eTLV_AMR_WB_OCT_ALIGN = 0x18,
    eTLV_RINGING_TMR      = 0x19,
    eTLV_RINGBCK_TMR      = 0x1A,
    eTLV_RTPRTCP_INAC_TMR = 0x1B,
};

/*
 * eQMI_IMS_VOIP_CONFIG_IND TLVs defined below
 */

/*
 * Name:    SessDurTlv
 *
 * Purpose: Structure used to store Session Duration TLV Value.
 *
 * Members: TlvPresent - Boolean indicating the presence of the TLV
 *                        in the QMI response
 *
 *          sessExp    - Session expiry timer
 */
struct SessDurTlv
{
    BYTE TlvPresent;
    WORD sessExp;
};

/*
 * Name:    MinSessExpTlv
 *
 * Purpose: Structure used to store Min Sessn Expiry Timer TLV Value.
 *
 * Members: TlvPresent - Boolean indicating the presence of the TLV
 *                        in the QMI response
 *
 *          minSessExp - Minimum Session expiry timer
 */
struct MinSessExpTlv
{
    BYTE TlvPresent;
    WORD minSessExp;
};

/*
 * Name:    EnabAMRWBTlv
 *
 * Purpose: Structure used to store Enable AMR WB TLV Value.
 *
 * Members: TlvPresent  - Boolean indicating the presence of the TLV
 *                        in the QMI response
 *
 *          amrWBEnable - AMR WB Enable
 */
struct EnabAMRWBTlv
{
    BYTE TlvPresent;
    BYTE amrWBEnable;
};

/*
 * Name:    EnabSCRAMRTlv
 *
 * Purpose: Structure used to store Enable SCR AMR TLV Value.
 *
 * Members: TlvPresent   - Boolean indicating the presence of the TLV
 *                         in the QMI response
 *
 *          scrAmrEnable - SCR AMR Enable
 */
struct EnabSCRAMRTlv
{
    BYTE TlvPresent;
    BYTE scrAmrEnable;
};

/*
 * Name:    EnabSCRAMRWBTlv
 *
 * Purpose: Structure used to store Enable SCR AMR WB TLV Value.
 *
 * Members: TlvPresent     - Boolean indicating the presence of the TLV
 *                           in the QMI response
 *
 *          scrAmrWBEnable - SCR AMR Enable
 */
struct EnabSCRAMRWBTlv
{
    BYTE TlvPresent;
    BYTE scrAmrWBEnable;
};

/*
 * Name:    AMRModeTlv
 *
 * Purpose: Structure used to store AMR Mode TLV Value.
 *
 * Members: TlvPresent - Boolean indicating the presence of the TLV
 *                       in the QMI response
 *
 *          amrMode    - AMR Mode
 */
struct AMRModeTlv
{
    BYTE TlvPresent;
    BYTE amrMode;
};

/*
 * Name:    AMRWBModeTlv
 *
 * Purpose: Structure used to store AMR WB Mode TLV Value.
 *
 * Members: TlvPresent - Boolean indicating the presence of the TLV
 *                       in the QMI response
 *
 *          amrWBMode  - AMR WB Mode
 */
struct AMRWBModeTlv
{
    BYTE TlvPresent;
    WORD amrWBMode;
};

/*
 * Name:    AMROctAlgnTlv
 *
 * Purpose: Structure used to store AMR Octet Aligned TLV Value.
 *
 * Members: TlvPresent - Boolean indicating the presence of the TLV
 *                       in the QMI response
 *
 *          amrOctAlgn - AMR Octet Aligned
 */
struct AMROctAlgnTlv
{
    BYTE TlvPresent;
    BYTE amrOctAlgn;
};

/*
 * Name:    AMRWBOctAlgnTlv
 *
 * Purpose: Structure used to store AMR WB Octet Aligned TLV Value.
 *
 * Members: TlvPresent   - Boolean indicating the presence of the TLV
 *                         in the QMI response
 *
 *          amrWBOctAlgn - AMR Octet Aligned
 */
struct AMRWBOctAlgnTlv
{
    BYTE TlvPresent;
    BYTE amrWBOctAlgn;
};

/*
 * Name:    RngTmrTlv
 *
 * Purpose: Structure used to store Ringing Timer Duration TLV Value.
 *
 * Members: TlvPresent - Boolean indicating the presence of the TLV
 *                       in the QMI response
 *
 *          RingTmr    - Ringing Timer
 */
struct RngTmrTlv
{
    BYTE TlvPresent;
    WORD RingTmr;
};

/*
 * Name:    RngBkTmrTlv
 *
 * Purpose: Structure used to store Ringback Timer Duration TLV Value.
 *
 * Members: TlvPresent - Boolean indicating the presence of the TLV
 *                       in the QMI response
 *
 *          RingBkTmr  - Ring Back Timer
 */
struct RngBkTmrTlv
{
    BYTE TlvPresent;
    WORD RingBkTmr;
};

/*
 * Name:    RTPRTCPInactTmrDurTlv
 *
 * Purpose: Structure used to store RTP/RTCP Inactivity Timer Duration
 *          TLV Value.
 *
 * Members: TlvPresent - Boolean indicating the presence of the TLV
 *                       in the QMI response
 *
 *          InactTmr   - RTP/RTCP Inactivity Timer
 */
struct RTPRTCPInactTmrDurTlv
{
    BYTE TlvPresent;
    WORD InactTmr;
};

/*
 * Name:    QmiCbkImsVoIPConfigInd
 *
 * Purpose: Structure used to store VoIP Config Indication Parameters.
 *
 * Members: SFTlv   - SMS Format Tlv
 *          SINTlv  - SMS Over IP NW Indication Flag Tlv
 *          PCURTlv - Phone Context URI Tlv
 *
 * Note:    None
 */
struct QmiCbkImsVoIPConfigInd{
    struct SessDurTlv            SDTlv;
    struct MinSessExpTlv         MSETlv;
    struct EnabAMRWBTlv          EAWTlv;
    struct EnabSCRAMRTlv         ESATlv;
    struct EnabSCRAMRWBTlv       ESAWTlv;
    struct AMRModeTlv            AMTlv;
    struct AMRWBModeTlv          AWMTlv;
    struct AMROctAlgnTlv         AOATlv;
    struct AMRWBOctAlgnTlv       AWOATlv;
    struct RngTmrTlv             RTTlv;
    struct RngBkTmrTlv           RBTTlv;
    struct RTPRTCPInactTmrDurTlv RTIDTlv;
};

/*
 * Prototypes
 */
enum eQCWWANError UpkQmiCbkImsVoIPCfgInd(
    BYTE                          *pMdmResp,
    struct QmiCbkImsVoIPConfigInd *pApiResp );


#endif /* __IMS_VOIP_CONFIG_IND_H_ */
