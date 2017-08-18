/*
 * \ingroup cbk
 *
 * \file    qaCbkWdsGetPktSrvcStatusInd.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaCbkWdsGetPktSrvcStatusInd.c
 *
 * Copyright: © 2011-2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __WDS_GET_PKT_SRVC_STATUS_IND_H__
#define __WDS_GET_PKT_SRVC_STATUS_IND_H__

/* An enumeration of eQMI_WDS_GET_PKT_SRVC_STATUS_IND response TLV IDs */
enum eQMI_WDS_GET_PKT_SRVC_STATUS_IND
{
    eTLV_PACKET_SERVICE_STATUS    = 0x01,
    eTLV_CALL_END_REASON          = 0x10,
    eTLV_VERBOSE_CALL_END_REASON  = 0x11,
    eTLV_IP_FAMILY                = 0x12,
    eTLV_TECH_NAME                = 0x13,
};

/*
 * eQMI_WDS_PKT_STATUS_IND TLVs defined below
 */

/*
 * Name:    PktSrvStatusTlv
 *
 * Purpose: Structure used to store Packet Service Status TLV Value.
 *
 * Members: TlvPresent               - boolean indicating the presence of the
 *                                     TLV in the QMI response
 *          state                    - session state
 *          reconfiguration_required - indicates if host needs to be
 *                                     reconfigured
 */
struct PktSrvStatusTlv
{
    BYTE  TlvPresent;
    ULONG state;
    ULONG reconfiguration_required;
};

/*
 * Name:    SessEndReasonTlv
 *
 * Purpose: Structure used to store Call End Reason TLV Value.
 *
 * Members: TlvPresent       - boolean indicating the presence of the
 *                             TLV in the QMI response
 *          sessionEndReason - session end reason
 */
struct SessEndReasonTlv
{
    BYTE  TlvPresent;
    ULONG sessionEndReason;
};

/*
 * Name:    VerbSessEndReasonTlv
 *
 * Purpose: Structure used to store Verbose call end reason TLV Value.
 *
 * Members: TlvPresent                - boolean indicating the presence of the
 *                                      TLV in the QMI response
 *          verboseSessnEndReasonType - verbose session end reason type
 *          verboseSessnEndReason     - verbose session end reason
 */
struct VerbSessEndReasonTlv
{
    BYTE  TlvPresent;
    WORD  verboseSessnEndReasonType;
    WORD  verboseSessnEndReason;
};

/*
 * Name:    IPFamilyTlv
 *
 * Purpose: Structure used to store IP Family TLV Value.
 *
 * Members: TlvPresent - boolean indicating the presence of the
 *                       TLV in the QMI response
 *          ipFamily   - IP Family
 */
struct IPFamilyTlv
{
    BYTE  TlvPresent;
    BYTE  ipFamily;
};

/*
 * Name:    TechNameTlv
 *
 * Purpose: Structure used to store Technology Name TLV Value.
 *
 * Members: TlvPresent - boolean indicating the presence of the
 *                       TLV in the QMI response
 *          ipFamily   - IP Family
 */
struct TechNameTlv
{
    BYTE  TlvPresent;
    WORD  techName;
};

/*
 * Name:    QmiCbkWdsSessionStateInd
 *
 * Purpose: Structure used to store all QMI Notification parameters.
 *
 * Members: PktSrvStatusTlv - Packet Status Tlv
 *          sERTlv          - Session End Reason Tlv
 *          vSERTlv         - Verbose Session End Reason Tlv
 *          IPFTlv          - IP Family Tlv
 *          tNamTlv         - Technology Name Tlv
 *
 * Note:    None
 */
struct QmiCbkWdsSessionStateInd{
    struct PktSrvStatusTlv      pktStatTlv;
    struct SessEndReasonTlv     sERTlv;
    struct VerbSessEndReasonTlv vSERTlv;
    struct IPFamilyTlv          IPFTlv;
    struct TechNameTlv          tNamTlv;
};

/*************
 * Prototypes
 **************/
enum eQCWWANError UpkQmiCbkWdsGetPktSrvcStatusInd (
    BYTE                            *pMdmResp,
    struct QmiCbkWdsSessionStateInd *pAipResp );

#endif /* __WDS_GET_PKT_SRVC_STATUS_IND_H__ */

