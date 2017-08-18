/**
 *    $Id: SwiIntCdma.h,v 1.4 2010/06/10 00:22:02 epasheva Exp $
 *
 *    Name       : SwiIntCdma.h
 *    
 *    Purpose    : SWI CDMA specific internal APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#ifndef __SWIINTCDMA_H__
#define __SWIINTCDMA_H__

#ifdef __cplusplus
extern "C" {
#endif

/* include files */
#include "SwiDataTypes.h"
#include "SwiRcodes.h"
#include "SwiApiCdmaBasic.h"

#define LENGTH_CDMA_PhoneNumber 15  /*!< Phone number length */
#define MAXLEN_OTASP_Dialstring 20  /*!< Max length of OTASP dial string */

/*! Max length of voice mail dial string */
#define MAXLEN_VMail_Dialstring 20

#define MAXLEN_MDN_Length       15  /*!< Max length of MDN */
#define LENGTH_DtmfBurstString  32  /*!< DTMF burst string length */

#define CNS_TECH_HDR_TC_DRC_NUM 15  /*!< DOrA traffic channel rates */
#define CNS_TECH_HDR_CC_DRC_NUM 2   /*!< DOrA control channel rates */

#define LENGTH_SectorId         16  /*!< Sector length */
#define LENGTH_HdrUati          16  /*!< HDR UATI length */

/*! Max EVDO personalities modem can store */
#define CNS_TECH_HDR_MAX_PERSONALITY    4

/*! HDR TC DRC number */
#define LENGTH_TechHdrTcDrcNum    15
#if defined(CNS_TECH_HDR_TC_DRC_NUM) && \
    CNS_TECH_HDR_TC_DRC_NUM != LENGTH_TechHdrTcDrcNum
#error CNS and SWI definition mismatch
#endif
 
/*! HDR CC DRC number */
#define LENGTH_TechHdrCcDrcNum    2
#if defined(CNS_TECH_HDR_CC_DRC_NUM) && \
    CNS_TECH_HDR_CC_DRC_NUM != LENGTH_TechHdrCcDrcNum
#error CNS and SWI definition mismatch
#endif

/*! Max length of HDR personality */
#define LENGTH_TechHdrMaxPersonality    4
#if defined(CNS_TECH_HDR_MAX_PERSONALITY) && \
    CNS_TECH_HDR_MAX_PERSONALITY != LENGTH_TechHdrMaxPersonality
#error CNS and SWI definition mismatch
#endif

/**
 *
 * An enumeration of data states fetch status. 
 *
 */
enum cns_data_stats_fetch_status_enum
{
  CNS_DATA_STATS_FETCH_SUCCESS,         /*!< Success */
  CNS_DATA_STATS_FETCH_INVALID_LINK,    /*!< Invalid link */
  CNS_DATA_STATS_FETCH_INVALID_TYPE,    /*!< Invalid type */
  CNS_DATA_STATS_FETCH_UNAVAILABLE      /*!< Unavailable */
};

/**
 *
 * An enumeration of link protocols. 
 *
 */
enum cns_protocol_link_enum
{
  CNS_PPP_OVER_UM,      /*!< PPP over UM */
  CNS_PPP_OVER_RM,      /*!< PPP over RM */
  CNS_IPV4_OVER_UM,     /*!< IPV4 over UM */
  CNS_ICMPV4_OVER_UM,   /*!< ICMPV4 over UM */
  CNS_TCP_OVER_UM,      /*!< TCP over UM */
  CNS_UDP_OVER_UM       /*!< UDP over UM */
};

/**
 *
 * An enumeration of PPP sub protocols. 
 *
 */
enum cns_ppp_sub_protocol_enum
{
  CNS_PPP_GENERAL,          /*!< General */
  CNS_PPP_VJ_COMPRESSION,   /*!< VJ compression */
  CNS_PPP_AUTH,             /*!< Authentication */
  CNS_PPP_LCP,              /*!< LCP */
  CNS_PPP_IPCP              /*!< IPCP */
};

/*
 *
 *    Not Supported / Internal Use - START                                    
 *
 */

/**
 *
 * An enumeration of device activation status. 
 *
 */
typedef enum SWI_TYPE_ActivationStatus
{
    SWI_ACTIVATION_STATUS_False,    /*!< Not activated */
    SWI_ACTIVATION_STATUS_True      /*!< Activated */
}SWI_TYPE_ActivationStatus;

/**
 *
 * Contains activation status data.
 *
 */
typedef struct SWI_STRUCT_ActivationStatus
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */

    /*! See  SWI_TYPE_ActivationStatus */
    SWI_TYPE_ActivationStatus eActivationStatus;
}SWI_STRUCT_ActivationStatus;

/**
 *
 * Contains ECIO data.
 *
 */
typedef struct SWI_STRUCT_Ecio
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */
    swi_uint32 nEcio;               /*!< ECIO */
}SWI_STRUCT_Ecio;

/**
 *
 * An enumeration of data states fetch status. 
 *
 */
typedef enum SWI_TYPE_V42BisEnabled
{
    SWI_V42_BIS_ENABLED_True,
    SWI_V42_BIS_ENABLED_False
}SWI_TYPE_V42BisEnabled;

/**
 *
 * Contains V42 Bis enabled data.
 *
 */
typedef struct SWI_STRUCT_V42BisEnabled
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */

    /*! See  SWI_TYPE_V42BisEnabled */
    SWI_TYPE_V42BisEnabled  eV42Bis;
}SWI_STRUCT_V42BisEnabled;

/**
 *
 * An enumeration of authentication status. 
 *
 */
typedef enum SWI_TYPE_AuthenticationStatus
{
    SWI_AUTHENTICATION_STATUS_True,
    SWI_AUTHENTICATION_STATUS_False
}SWI_TYPE_AuthenticationStatus;

/**
 *
 * Contains authentication statusdata.
 *
 */
typedef struct SWI_STRUCT_AuthenticationStatus
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */

    /*! See  SWI_TYPE_V42BisEnabled */
    SWI_TYPE_AuthenticationStatus   eAuthenticationStatus;
}SWI_STRUCT_AuthenticationStatus;

/**
 *
 * Contains Tech SID data.
 *
 */
typedef struct SWI_STRUCT_TechSid
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */
    swi_uint32 nSid;                /*!< SID */
}SWI_STRUCT_TechSid;

/**
 *
 * Contains Tech NID data.
 *
 */
typedef struct SWI_STRUCT_TechNid
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */
    swi_uint32 nNid;                /*!< NID */
}SWI_STRUCT_TechNid;

/**
 *
 * Contains basestation information.
 *
 */
typedef struct SWI_STRUCT_TechBsInfo
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */
    swi_uint32  nLongitude;         /*!< Longitude */
    swi_uint32  nLatitude;          /*!< Latitude */
}SWI_STRUCT_TechBsInfo;

/**
 *
 * Contains Reg zone ID data.
 *
 */
typedef struct SWI_STRUCT_TechRegZone
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */
    swi_uint32 nZoneID;             /*!< Zone ID */
}SWI_STRUCT_TechRegZone;

/**
 *
 * Contains Packet zone ID data.
 *
 */
typedef struct SWI_STRUCT_TechPacketZone
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */
    swi_uint32  nZoneID;            /*!< Zone ID */
}SWI_STRUCT_TechPacketZone;

/**
 *
 * An enumeration of PCS block. 
 *
 */
typedef enum SWI_TYPE_TechPcsBlock
{
    SWI_TECH_PCS_BLOCK_NoCoverage,  /*!< No coverage */
    SWI_TECH_PCS_BLOCK_A,           /*!< Block A */
    SWI_TECH_PCS_BLOCK_B,           /*!< Block B */
    SWI_TECH_PCS_BLOCK_C,           /*!< Block C*/
    SWI_TECH_PCS_BLOCK_D,           /*!< Block D*/
    SWI_TECH_PCS_BLOCK_E,           /*!< Block E*/
    SWI_TECH_PCS_BLOCK_F            /*!< Block F*/
}SWI_TYPE_TechPcsBlock;

/**
 *
 * Contains PCS block data.
 *
 */
typedef struct SWI_STRUCT_TechPcsBlock
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */

    /*! See  SWI_TYPE_TechPcsBlock */
    SWI_TYPE_TechPcsBlock   ePcs;
}SWI_STRUCT_TechPcsBlock;

/**
 *
 * An enumeration of cell channel. 
 *
 */
typedef enum SWI_TYPE_TechCellChannel
{
    SWI_TECH_CELL_CHANNEL_NoCoverage,   /*!< No coverage */
    SWI_TECH_CELL_CHANNEL_A,            /*!< Channel A */
    SWI_TECH_CELL_CHANNEL_B             /*!< Channel B */
}SWI_TYPE_TechCellChannel;

/**
 *
 * Contains cell channel data.
 *
 */
typedef struct SWI_STRUCT_TechCellChannel
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */

    /*! See  SWI_TYPE_TechCellChannel */
    SWI_TYPE_TechCellChannel    eChannel;
}SWI_STRUCT_TechCellChannel;

/**
 *
 * Contains system time data.
 *
 */
typedef struct _swi_systemtime
{
    swi_uint16 wYear;           /*!< Year */
    swi_uint16 wMonth;          /*!< Month */
    swi_uint16 wDayOfWeek;      /*!< Day of the week */
    swi_uint16 wDay;            /*!< Day */
    swi_uint16 wHour;           /*!< Hour */
    swi_uint16 wMinute;         /*!< Minute */
    swi_uint16 wSecond;         /*!< Second */
    swi_uint16 wMilliseconds;   /*!< Milli-second */
} swi_systemtime;

/**
 *
 * Contains system time data.
 *
 */
typedef struct SWI_STRUCT_SystemTime
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */

    /*! See  swi_systemtime */
    swi_systemtime  time;
}SWI_STRUCT_SystemTime;


/**
 *
 * Contains IOTA status data.
 *
 */
typedef struct SWI_STRUCT_HIOTAStatus
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */
    swi_uint32  nResult;            /*!< Results */
    swi_uint32  nMask;              /*!< Update mask */
}SWI_STRUCT_HIOTAStatus;

/**
 *
 * An enumeration of NAM value. 
 *
 */
typedef enum SWI_TYPE_NamValue
{
    SWI_NAM_VALUE_0,    /*!< NAM value 0 */
    SWI_NAM_VALUE_1,    /*!< NAM value 1 */
    SWI_NAM_VALUE_2,    /*!< NAM value 2 */
    SWI_NAM_VALUE_3,    /*!< NAM value 3 */
    SWI_NAM_VALUE_Auto  /*!< Auto NAM value */
}SWI_TYPE_NamValue;

/**
 *
 * Contains active NAM data.
 *
 */
typedef struct SWI_STRUCT_ActiveNam
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */

    /*! See  SWI_TYPE_NamValue */
    SWI_TYPE_NamValue   eNamIndex;
}SWI_STRUCT_ActiveNam;

/**
 *
 * An enumeration of modem too hot action. 
 *
 */
typedef enum SWI_TYPE_ModemTooHot
{
    SWI_MODEM_TOO_HOT_Disconnect    = 0x0011,   /*!< Disconnect */
    SWI_MODEM_TOO_HOT_ShutdownModem = 0x0012    /*!< Shutdown modem */
}SWI_TYPE_ModemTooHot;

/**
 *
 * Contains modem too hot actiondata.
 *
 */
typedef struct SWI_STRUCT_ModemTooHotIndication
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */

    /*! See  SWI_TYPE_ModemTooHot */
    SWI_TYPE_ModemTooHot    eAction;
}SWI_STRUCT_ModemTooHotIndication;

/**
 *
 * Contains flash burn data.
 *
 */
typedef struct SWI_STRUCT_FlashBurn
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */
    swi_uint16 nSector;             /*!< Flash burn */
}SWI_STRUCT_FlashBurn;

/**
 *
 * An enumeration of excess SPC failure. 
 *
 */
typedef enum SWI_TYPE_ExcessSPCFailures
{
    SWI_MODEM_RESET     = 0x0000,   /*!< Reset */
    SWI_MODEM_FREEZE    = 0x0001    /*!< Freeze */
}SWI_TYPE_ExcessSPCFailures;

/**
 *
 * Contains excess SPC failure data.
 *
 */
typedef struct SWI_STRUCT_ExcessSPCFailures
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */

    /*! See  SWI_TYPE_ExcessSPCFailures */
    SWI_TYPE_ExcessSPCFailures  eFailType;
}SWI_STRUCT_ExcessSPCFailures;


/**
 *
 * Contains HDR acquired state.
 *
 */
typedef struct SWI_STRUCT_Hdr_Acq 
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */
    swi_int32 lAcquired;             /*!< Acquired state */
}SWI_STRUCT_Hdr_Acq;

/**
 *
 * Contains HDR RSSI data.
 *
 */
typedef struct SWI_STRUCT_Hdr_Rssi 
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */
    swi_int32   iRssi;              /*!< RSSI */
}SWI_STRUCT_Hdr_Rssi;

/**
 *
 * Contains HDR notification status.
 *
 */
typedef struct SWI_STRUCT_TechHdrNotificationStatus
{
    swi_uint32 sizeStruct;                  /*!< Size of this structure */
    swi_uint16 iTechHdrNotificationStatus;  /*!< HDR notification status */
}SWI_STRUCT_TechHdrNotificationStatus;

/**
 *
 * Contains HDR Prev data.
 *
 */
typedef struct SWI_STRUCT_Tech_Hdr_Prev
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */
    swi_uint16 iHdrPRevMin;         /*!< HDR Prev Min */
    swi_uint16 iHdrPRevMax;         /*!< HDR Prev Max */
}SWI_STRUCT_Tech_Hdr_Prev;

/**
 *
 * Contains HDR Sector data.
 *
 */
typedef struct SWI_STRUCT_Tech_Hdr_Sector
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */
    swi_uint8  szHdrSectorId[LENGTH_SectorId];  /*!< Sector ID */
    swi_uint16 iHdrSubnetMask;                  /*!< Subnet mask */
    swi_uint16 iHdrColorCode;                   /*!< Color code */
    swi_uint16 iHdrPnOffset;                    /*!< PN offset */
}SWI_STRUCT_Tech_Hdr_Sector;

/**
 *
 * An enumeration of HDR AT state. 
 *
 */
typedef enum SWI_TYPE_Tech_Hdr_At_State
{
    SWI_TECH_HDR_AT_STATE_INACTIVE    = 0x00000000, /*!< Inactive */
    SWI_TECH_HDR_AT_STATE_ACQUISITION = 0x00000001, /*!< Acquisition */
    SWI_TECH_HDR_AT_STATE_SYNC        = 0x00000002, /*!< Sync */
    SWI_TECH_HDR_AT_STATE_IDLE        = 0x00000003, /*!< Idle */
    SWI_TECH_HDR_AT_STATE_ACCESS      = 0x00000004, /*!< Access */
    SWI_TECH_HDR_AT_STATE_CONNECTED   = 0x00000005  /*!< Connected */
}SWI_TYPE_Tech_Hdr_At_State;

/**
 *
 * An enumeration of HDR session state. 
 *
 */
typedef enum SWI_TYPE_Tech_Hdr_Session_State
{
    /*! Close */
    SWI_TECH_HDR_SESSION_STATE_CLOSE                    = 0x00000000,

    /*! Address management protocol setup */
    SWI_TECH_HDR_SESSION_STATE_ADDR_MGMT_PROTOCOL_SETUP = 0x00000001,
        
    /*! AT initiated */
    SWI_TECH_HDR_SESSION_STATE_AT_INITIATED             = 0x00000002,
        
    /*! AN initiated */
    SWI_TECH_HDR_SESSION_STATE_AN_INITIATED             = 0x00000003,
        
    /*! Open */
    SWI_TECH_HDR_SESSION_STATE_OPEN                     = 0x00000004
}SWI_TYPE_Tech_Hdr_Session_State;

/**
 *
 * Contains HDR state.
 *
 */
typedef struct SWI_STRUCT_Tech_Hdr_State
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */
    swi_uint16 iHdrAtState;         /*!< See  SWI_TYPE_Tech_Hdr_At_State */
    swi_uint16 iHdrSessionState;    /*!< See  SWI_TYPE_Tech_Hdr_Session_State */
}SWI_STRUCT_Tech_Hdr_State;

/**
 *
 * Contains HDR channel data.
 *
 */
typedef struct SWI_STRUCT_Tech_Hdr_Channel
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */
    swi_uint16 iHdrChannel;         /*!< Channel */
}SWI_STRUCT_Tech_Hdr_Channel;

/**
 *
 * Contains HDR session data.
 *
 */
typedef struct SWI_STRUCT_Tech_Hdr_Session
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */
    swi_uint8   szHdrUati[LENGTH_HdrUati];  /*!< UATI */
    swi_uint16  iHdrUatiColorCode;          /*!< UATI color code */
    swi_uint32  iHdrRati;                   /*!< RATI */
    swi_uint16  iHdrSessionDuration;        /*!< Duration */
    swi_uint32  iHdrSessionStart1;          /*!< Start1 */
    swi_uint32  iHdrSessionStart2;          /*!< Start2 */
    swi_uint32  iHdrSessionEnd1;            /*!< End1 */
    swi_uint32  iHdrSessionEnd2;            /*!< End2 */
}SWI_STRUCT_Tech_Hdr_Session;

/**
 *
 * An enumeration of AN authentication data. 
 *
 */
typedef enum SWI_TYPE_Tech_Hdr_An_Auth
{
    /*! Not authenticated */
    SWI_TECH_HDR_AN_AUTH_NOT_AUTHENTICATED            = 0x00000000,

    /*! Authenticated */
    SWI_TECH_HDR_AN_AUTH_AUTHENTICATED                = 0x00000001,

    /*! Failed */
    SWI_TECH_HDR_AN_AUTH_AUTHENTICATION_FAILED        = 0x00000002,

    /*! Disabled */
    SWI_TECH_HDR_AN_AUTH_AUTHENTICATION_DISABLED      = 0x00000003
}SWI_TYPE_Tech_Hdr_An_Auth;

/**
 *
 * Contains AN authentication data..
 *
 */
typedef struct SWI_STRUCT_Tech_Hdr_An_Auth
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */
    swi_uint16 iHdrAnAuthS;         /*!< See SWI_TYPE_Tech_Hdr_An_Auth */
}SWI_STRUCT_Tech_Hdr_An_Auth;

/**
 *
 * Contains HDR DRC value.
 *
 */
typedef struct SWI_STRUCT_Tech_Hdr_Drc_Val
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */
    swi_uint16 iHdrDrcValue;        /*!< HDR DRC value */
}SWI_STRUCT_Tech_Hdr_Drc_Val;

/**
 *
 * Contains HDR DRC cover data.
 *
 */
typedef struct SWI_STRUCT_Tech_Hdr_Drc_Cover
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */
    swi_uint16 iHdrDrcCover;        /*!< HDR DRC cover data */
}SWI_STRUCT_Tech_Hdr_Drc_Cover;

/**
 *
 * Contains HDR RRI data.
 *
 */
typedef struct SWI_STRUCT_Tech_Hdr_Rri
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */
    swi_uint16 iHdrRri;             /*!< HDR RRI data */
}SWI_STRUCT_Tech_Hdr_Rri;

/**
 *
 * Contains HDR state.
 *
 */
typedef struct SWI_STRUCT_Tech_Hdr_Stats
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */
    swi_uint16 iHdrRxAgc0;          /*!< RX Agc 0 */
    swi_uint16 iHdrRxAgc1;          /*!< RX Agc 1 */
    swi_uint16 iHdrTxTotal;         /*!< TX total */
    swi_uint16 iHdrTxAdj;           /*!< TX adjustment */
    swi_uint16 iHdrc2i;             /*!< C2I */
    swi_uint16 iHdrEcio;            /*!< ECIO */
    swi_uint16 iHdrRssi;            /*!< RSSI */
}SWI_STRUCT_Tech_Hdr_Stats;


/**
 *
 * Contains Su Fwd Statistics data
 *
 */
typedef struct SWI_STRUCT_Tech_Hdr_Su_Fwd_Stats
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */

    swi_uint32 nTcCrcGood[LENGTH_TechHdrTcDrcNum];  /*!< TC CRC good */
    swi_uint32 nTcCrcBad[LENGTH_TechHdrTcDrcNum];   /*!< TC CRC bad */
    swi_uint32 nCcCrcGood[LENGTH_TechHdrCcDrcNum];  /*!< CC CRC good */
    swi_uint32 nCcCrcBad[LENGTH_TechHdrCcDrcNum];   /*!< CC CRC bad */
    swi_uint32 nTotalPackets;                       /*!< Total packets */
    swi_uint32 nBadPackets;                         /*!< Bad packets */
    swi_uint32 nTcServedThroughput;             /*!< TC served throughput */
    swi_uint32 nTcInstantThroughput;            /*!< TC instant throughput */
    swi_uint32 nTcAvrgThroughtput;              /*!< TC average throughput */
}SWI_STRUCT_Tech_Hdr_Su_Fwd_Stats;

#define NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc0Tsc   16  /*!< TC DRC0 TSC num */
#define NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc1Tsc   16  /*!< TC DRC1 TSC num */
#define NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc2Tsc   8   /*!< TC DRC2 TSC num */
#define NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc3Tsc   4   /*!< TC DRC3 TSC num */
#define NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc4Tsc   2   /*!< TC DRC4 TSC num */
#define NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc5Tsc   4   /*!< TC DRC5 TSC num */
#define NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc6Tsc   1   /*!< TC DRC6 TSC num */
#define NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc7Tsc   2   /*!< TC DRC7 TSC num */
#define NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc8Tsc   2   /*!< TC DRC8 TSC num */
#define NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc9Tsc   1   /*!< TC DRC9 TSC num */

/**
 *
 * Contains HDR Su Fwd Slot Cnt data.
 *
 */
typedef struct SWI_STRUCT_Tech_Hdr_Su_Fwd_Slot_Cnt
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */

    /*! TC DRC0 TSC */
    swi_uint32 nTcDrc0Tsc[NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc0Tsc];

    /*! TC DRC1TSC */
    swi_uint32 nTcDrc1Tsc[NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc1Tsc];

    /*! TC DRC2TSC */
    swi_uint32 nTcDrc2Tsc[NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc2Tsc];

    /*! TC DRC3TSC */
    swi_uint32 nTcDrc3Tsc[NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc3Tsc];

    /*! TC DRC4TSC */
    swi_uint32 nTcDrc4Tsc[NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc4Tsc];

    /*! TC DRC5TSC */
    swi_uint32 nTcDrc5Tsc[NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc5Tsc];

    /*! TC DRC6TSC */
    swi_uint32 nTcDrc6Tsc[NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc6Tsc];

    /*! TC DRC7TSC */
    swi_uint32 nTcDrc7Tsc[NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc7Tsc];

    /*! TC DRC8TSC */
    swi_uint32 nTcDrc8Tsc[NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc8Tsc];

    /*! TC DRC9 TSC */
    swi_uint32 nTcDrc9Tsc[NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc9Tsc];
}SWI_STRUCT_Tech_Hdr_Su_Fwd_Slot_Cnt;

#define NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt2_TcDrc10Tsc 2  /*!< TC DRC10 TSC num */
#define NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt2_TcDrc11Tsc 1  /*!< TC DRC11 TSC num */
#define NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt2_TcDrc12Tsc 1  /*!< TC DRC12 TSC num */
#define NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt2_TcDrc13Tsc 2  /*!< TC DRC13 TSC num */
#define NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt2_TcDrc14Tsc 1  /*!< TC DRC14 TSC num */
#define NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt2_CcDrc1Tsc  16 /*!< CC DRC1 TSC num */
#define NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt2_CcDrc2Tsc  8  /*!< CC DRC2 TSC num */

/**
 *
 * Contains HDR Su Fwd Slot Cnt2 data.
 *
 */
typedef struct SWI_STRUCT_Tech_Hdr_Su_Fwd_Slot_Cnt2
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */

    /*! TC DRC10 TSC */
    swi_uint32 nTcDrc10Tsc[NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt2_TcDrc10Tsc];

    /*! TC DRC11 TSC */
    swi_uint32 nTcDrc11Tsc[NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt2_TcDrc11Tsc];

    /*! TC DRC12 TSC */
    swi_uint32 nTcDrc12Tsc[NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt2_TcDrc12Tsc];

    /*! TC DRC13 TSC */
    swi_uint32 nTcDrc13Tsc[NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt2_TcDrc13Tsc];

    /*! TC DRC14 TSC */
    swi_uint32 nTcDrc14Tsc[NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt2_TcDrc14Tsc];

    /*! CC DRC1 TSC */
    swi_uint32 nCcDrc1Tsc[NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt2_CcDrc1Tsc];

    /*! CC DRC2 TSC */
    swi_uint32 nCcDrc2Tsc[NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt2_CcDrc2Tsc];
}SWI_STRUCT_Tech_Hdr_Su_Fwd_Slot_Cnt2;


#define NUMBER_Tech_Hdr_Mu_Fwd_Stats_Drc3_Tsc   4   /*!< CC DRC3 TSC num */
#define NUMBER_Tech_Hdr_Mu_Fwd_Stats_Drc5Tsc    4   /*!< CC DRC5 TSC num */
#define NUMBER_Tech_Hdr_Mu_Fwd_Stats_Drc8Tsc    2   /*!< CC DRC8 TSC num */
#define NUMBER_Tech_Hdr_Mu_Fwd_Stats_Drc10Tsc   2   /*!< CC DRC10 TSC num */
#define NUMBER_Tech_Hdr_Mu_Fwd_Stats_Drc13Tsc   2   /*!< CC DRC13 TSC num */

/**
 *
 * Contains HDR Mu Fwd statistics data.
 *
 */
typedef struct SWI_STRUCT_Tech_Hdr_Mu_Fwd_Stats
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */

    swi_uint32 nDrc3_128_GoodCrc;       /*!< DRC3 128 good CRC */
    swi_uint32 nDrc3_256_GoodCrc;       /*!< DRC3 256 good CRC */
    swi_uint32 nDrc3_512_GoodCrc;       /*!< DRC3 512 good CRC */
    swi_uint32 nDrc3_1024_GoodCrc;      /*!< DRC3 1024 good CRC */
    swi_uint32 nDrc5_2048_GoodCrc;      /*!< DRC3 2048 good CRC */
    swi_uint32 nDrc8_3072_GoodCrc;      /*!< DRC3 3072 good CRC */
    swi_uint32 nDrc10_4096_GoodCrc;     /*!< DRC3 4096 good CRC */
    swi_uint32 nDrc13_5120_GoodCrc;     /*!< DRC3 5120 good CRC */
    swi_uint32 nDrc3BadCrc;             /*!< DRC3 bad CRC */
    swi_uint32 nDrc5BadCrc;             /*!< DRC5 bad CRC */
    swi_uint32 nDrc8BadCrc;             /*!< DRC8 bad CRC */
    swi_uint32 nDrc10BadCrc;            /*!< DRC10 bad CRC */
    swi_uint32 nDrc13BadCrc;            /*!< DRC13 bad CRC */

    /*! DRC3 128  TSC */
    swi_uint32 nDrc3_128_Tsc[NUMBER_Tech_Hdr_Mu_Fwd_Stats_Drc3_Tsc];

    /*! DRC3 256  TSC */
    swi_uint32 nDrc3_256_Tsc[NUMBER_Tech_Hdr_Mu_Fwd_Stats_Drc3_Tsc];

    /*! DRC3 512  TSC */
    swi_uint32 nDrc3_512_Tsc[NUMBER_Tech_Hdr_Mu_Fwd_Stats_Drc3_Tsc];

    /*! DRC3 1024  TSC */
    swi_uint32 nDrc3_1024_Tsc[NUMBER_Tech_Hdr_Mu_Fwd_Stats_Drc3_Tsc];

    /*! DRC5 TSC */
    swi_uint32 nDrc5Tsc[NUMBER_Tech_Hdr_Mu_Fwd_Stats_Drc5Tsc];

    /*! DRC8 TSC */
    swi_uint32 nDrc8Tsc[NUMBER_Tech_Hdr_Mu_Fwd_Stats_Drc8Tsc];

    /*! DRC10 TSC */
    swi_uint32 nDrc10Tsc[NUMBER_Tech_Hdr_Mu_Fwd_Stats_Drc10Tsc];

    /*! DRC13 TSC */
    swi_uint32 nDrc13Tsc[NUMBER_Tech_Hdr_Mu_Fwd_Stats_Drc13Tsc];
    
    swi_uint32 nTotalPackets;           /*!< Total packets */
    swi_uint32 nBadPackets;             /*!< Bad packets */
    swi_uint32 nServedThroughput;       /*!< Served throughput */
    swi_uint32 nInstantThroughput;      /*!< Instant throughput */
    swi_uint32 nAvrgThroughtput;        /*!< Average throughput */
}SWI_STRUCT_Tech_Hdr_Mu_Fwd_Stats;

#define NUMBER_Tech_Hdr_Personality_SubType 12  /*!< HDR personality subtype */

/**
 *
 * Contains HDR personality data.
 *
 */
typedef struct SWI_STRUCT_Tech_Hdr_Personality
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */

    swi_uint16 nCurrPersonality;    /*!< Current personality */
    swi_uint16 nSubType[NUMBER_Tech_Hdr_Personality_SubType];   /*!< Subtype */
}SWI_STRUCT_Tech_Hdr_Personality;

#define NUMBER_Tech_Hdr_Prot_SubType    12  /*!< HDR prot subtype */
#define NUMBER_Tech_Hdr_Prot_AppSubtype 4   /*!< HDR prot app subtype */

/**
 *
 * Contains HDR prot app subtype data.
 *
 */
typedef struct SWI_STRUCT_Tech_Hdr_Prot_App_Subtype
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */

    swi_uint16 nCurrPersonality;        /*!< Current personality */
    swi_uint16 nStoredPersonalities;    /*!< Stored personality */
    swi_uint16 nSubType[LENGTH_TechHdrMaxPersonality]
                       [NUMBER_Tech_Hdr_Prot_SubType];  /*!< Subtype */

    /*! App subtype */
    swi_uint16 nApplicationSubtype[NUMBER_Tech_Hdr_Prot_AppSubtype];
}SWI_STRUCT_Tech_Hdr_Prot_App_Subtype;

/*! HDR slot cycle timeout1 */
#define NUMBER_Tech_Hdr_Sleep_SlotCycleTimeout1 2

/*! HDR slot cycle timeout2 */
#define NUMBER_Tech_Hdr_Sleep_SlotCycleTimeout2 2

/**
 *
 * Contains HDR sleep parameters.
 *
 */
typedef struct SWI_STRUCT_Tech_Hdr_Sleep_Parms
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */

    swi_uint16 nSlotCycle1;         /*!< HDR slot cycle1 */
    swi_uint16 nSlotCycle2;         /*!< HDR slot cycle2 */
    swi_uint16 nSlotCycle3;         /*!< HDR slot cycle3 */

    /*! HDR slot cycle timeout1 */
    swi_uint32 nSlotCycleTimeout1[NUMBER_Tech_Hdr_Sleep_SlotCycleTimeout1];

    /*! HDR slot cycle timeout2 */
    swi_uint32 nSlotCycleTimeout2[NUMBER_Tech_Hdr_Sleep_SlotCycleTimeout2];
}SWI_STRUCT_Tech_Hdr_Sleep_Parms;

/**
 *
 * Contains PPP general state data.
 *
 */
typedef struct SWI_STRUCT_PppGeneralStats
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */

    swi_uint32 bad_fcs;             /*!< Bad fcs */
    swi_uint32 bad_proto;           /*!< Bad proto */
    swi_uint32 unsupported_proto;   /*!< Unsupported proto */
    swi_uint32 not_enabled_proto;   /*!< Not enabled proto */
    swi_uint32 missing_addr;        /*!< Missing addr */
    swi_uint32 missing_ctrl;        /*!< Missing ctrl */
    swi_uint32 pkts_dropped_rx;     /*!< Pkts dropped rx */
    swi_uint32 pkts_rx;             /*!< Pkts rx */
    swi_uint64 framed_bytes_rx;     /*!< Framed bytes rx */
    swi_uint64 unframed_bytes_rx;   /*!< Unframed bytes rx */
    swi_uint32 pkts_dropped_tx;     /*!< Pkts dropped tx */
    swi_uint32 pkts_tx;             /*!< Pkts tx */
    swi_uint64 framed_bytes_tx;     /*!< Framed bytes tx */
    swi_uint64 unframed_bytes_tx;   /*!< Unframed bytes tx */
    swi_uint32 ipv4_pkts_rx;        /*!< Ipv4 pkts rx */
    swi_uint32 ipv4_pkts_tx;        /*!< Ipv4 pkts tx */
}SWI_STRUCT_PppGeneralStats;

/**
 *
 * Contains PPP VJ compression data.
 *
 */
typedef struct SWI_STRUCT_PppVjCompressionStats
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */

    swi_uint32 vjch_pkts_rx;         /*!< VJCH pkts rx */
    swi_uint32 vjch_pkts_tx;         /*!< VJCH pkts tx */
    swi_uint32 vjuch_pkts_rx;        /*!< VJUCH pkts rx */
    swi_uint32 vjuch_pkts_tx;        /*!< VJUCH pkts tx */
}SWI_STRUCT_PppVjCompressionStats;

/**
 *
 * Contains PPP authentication data.
 *
 */
typedef struct SWI_STRUCT_PppAuthStats
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */

    swi_uint32 pap_pkts_rx;         /*!< PAP packets RX */
    swi_uint32 pap_pkts_tx;         /*!< PAP packets TX */
    swi_uint32 pap_auth_failures;   /*!< PAP authentication failed */
    swi_uint32 chap_pkts_rx;        /*!< CHAP packets RX */
    swi_uint32 chap_pkts_tx;        /*!< CHAP packets TX */
    swi_uint32 chap_auth_failures;  /*!< CHAP authentication failed */
}SWI_STRUCT_PppAuthStats;

/**
 *
 * Contains PPP LCP data.
 *
 */
typedef struct SWI_STRUCT_PppLcpStats
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */

    swi_uint32 lcp_pkts_rx;             /*!< Packets RX */
    swi_uint32 lcp_pkts_tx;             /*!< Packets TX */
    swi_uint32 lcp_config_requests_rx;  /*!< Config requests rx */
    swi_uint32 lcp_config_acks_rx;      /*!< Config acks rx */
    swi_uint32 lcp_config_naks_rx;      /*!< Config naks rx */
    swi_uint32 lcp_config_rejects_rx;   /*!< Config rejects rx */
    swi_uint32 lcp_term_requests_rx;    /*!< Term requests rx */
    swi_uint32 lcp_term_acks_rx;        /*!< Term acks rx */
    swi_uint32 lcp_code_rejects_rx;     /*!< Code rejects rx */
    swi_uint32 lcp_proto_rejects_rx;    /*!< Proto rejects rx */
    swi_uint32 lcp_echo_requests_rx;    /*!< Echo requests rx */
    swi_uint32 lcp_echo_replies_rx;     /*!< Echo replies rx */
    swi_uint32 lcp_discard_requests_rx; /*!< Discard requests rx */
    swi_uint32 lcp_identification_rx;   /*!< Identification rx */
    swi_uint32 lcp_time_remaining_rx;   /*!< Time remaining rx */
    swi_uint32 lcp_config_requests_tx;  /*!< Config requests tx */
    swi_uint32 lcp_config_acks_tx;      /*!< Config acks tx */
    swi_uint32 lcp_config_naks_tx;      /*!< Config naks tx */
    swi_uint32 lcp_config_rejects_tx;   /*!< Config rejects tx */
    swi_uint32 lcp_term_requests_tx;    /*!< Term requests tx */
    swi_uint32 lcp_term_acks_tx;        /*!< Term acks tx */
    swi_uint32 lcp_code_rejects_tx;     /*!< Code rejects tx */
    swi_uint32 lcp_proto_rejects_tx;    /*!< Proto rejects tx */
    swi_uint32 lcp_echo_requests_tx;    /*!< Echo requests tx */
    swi_uint32 lcp_echo_replies_tx;     /*!< Echo replies tx */
    swi_uint32 lcp_discard_requests_tx; /*!< Discard requests tx */
}SWI_STRUCT_PppLcpStats;

/**
 *
 * Contains PPP IPCP data.
 *
 */
typedef struct SWI_STRUCT_PppIpcpStats
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */

    swi_uint32 ipcp_pkts_rx;            /*!< Pkts rx */
    swi_uint32 ipcp_pkts_tx;            /*!< Pkts tx */            
    swi_uint32 ipcp_config_requests_rx; /*!< Config requests rx */
    swi_uint32 ipcp_config_acks_rx;     /*!< Config acks rx */
    swi_uint32 ipcp_config_naks_rx;     /*!< Config naks rx */
    swi_uint32 ipcp_config_rejects_rx;  /*!< Config rejects rx */
    swi_uint32 ipcp_term_requests_rx;   /*!< term requests rx */
    swi_uint32 ipcp_term_acks_rx;       /*!< term acks RC */
    swi_uint32 ipcp_code_rejects_rx;    /*!< Code requests rx */
    swi_uint32 ipcp_config_requests_tx; /*!< Config requests tx */
    swi_uint32 ipcp_config_acks_tx;     /*!< Config acks tx */
    swi_uint32 ipcp_config_naks_tx;     /*!< Config naks tx */
    swi_uint32 ipcp_config_rejects_tx;  /*!< Config rejects tx */
    swi_uint32 ipcp_term_requests_tx;   /*!< Term requests tx */
    swi_uint32 ipcp_term_acks_tx;       /*!< Term acks tx */
    swi_uint32 ipcp_code_rejects_tx;    /*!< Code rejects tx */
}SWI_STRUCT_PppIpcpStats;

/**
 *
 * Contains PPP data.
 *
 */
typedef struct SWI_STRUCT_PppStats
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */

    swi_uint8 ppp_sub_protocol;     /*!< Of type SWI_TYPE_PppSubProtocol */

    union
    {
        /*! See  SWI_STRUCT_PppGeneralStats */
        SWI_STRUCT_PppGeneralStats general;

        /*! See  SWI_STRUCT_PppVjCompressionStats */
        SWI_STRUCT_PppVjCompressionStats vj;

        /*! See  SWI_STRUCT_PppAuthStats */
        SWI_STRUCT_PppAuthStats auth;

        /*! See  SWI_STRUCT_PppLcpStats */
        SWI_STRUCT_PppLcpStats lcp;

        /*! See  SWI_STRUCT_PppIpcpStats */
        SWI_STRUCT_PppIpcpStats ipcp;    
    }sub_protocol;
}SWI_STRUCT_PppStats;

/**
 *
 * Contains IPV4 data.
 *
 */
typedef struct SWI_STRUCT_Ipv4Stats
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */

    /*! Desc: # of packets with invalid header length */
    swi_uint32 bad_hdr_len;

    /*! Desc: # of packets with bad payload lengths  */
    swi_uint32 bad_len;

    /*! Desc: # of packets with errors in option. NOT IMPLEMENTED CURRENTLY*/
    swi_uint32 bad_options;

    /*! # of packets with bad version in ip header */
    swi_uint32 bad_version;

    /*! # of packets with bad header checksum */
    swi_uint32 bad_chksum;

    /*! # of ip packets that are shorter than the size of a valid ip header */
    swi_uint32 too_short;

    /*! # of lpackets not transmitted because of absence of route */
    swi_uint32 no_route;

    /*! # of incoming packets with unknown protocol */
    swi_uint32 no_proto;

    /*! # of incoming packets */
    swi_uint32 pkts_rx;

    /*! # of incoming packets dropped at the ip layer */
    swi_uint32 pkts_dropped_rx;

    /*! # of outgoing packets dropped at the ip layer */
    swi_uint32 pkts_dropped_tx;

    swi_uint32 pkts_fwd;        /*!< # of forwarded ip packets*/
    swi_uint32 pkts_tx;         /*!< # of outgoing packets*/
    swi_uint32 mcast_rx;        /*!< # of incoming multicast packets*/
    swi_uint32 mcast_tx;        /*!< # of outgoing multicast packets*/

    /*! # of  ip packets that have been successfully fragmented  */
    swi_uint32 frag_ok;

    /*! # of ip packets that failed fragmentation 
    (e.g Don’t fragment flag was set) */
    swi_uint32 frag_fails;
    
    swi_uint32 frag_create;     /*!< # of ip fragments created   */

    /*! # of incoming ip fragments that needed to be reassembled */
    swi_uint32 reasm_reqd;

    /*! # of incoming ip packets successfully reassembled */
    swi_uint32 reasm_ok;

    /*! # of  incoming ip packets that failed reassembly */
    swi_uint32 reasm_fails;

    /*! # of incoming packets that failed reassembly due to timeout */
    swi_uint32 reasm_timeout;
}SWI_STRUCT_Ipv4Stats;

/**
 *
 * Contains ICMPV4 data.
 *
 */
typedef struct SWI_STRUCT_Icmpv4Stats
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */

    swi_uint32 msgs_rx;         /*!< # of incoming icmp messages */
    swi_uint32 msgs_dropped_rx; /*!< # of incoming icmp messages dropped */

    /*! # of incoming addr mask replies. NOT IMPLEMENTED CURRENTLY*/
    swi_uint32 addr_mask_replies_rx;

    /*! # of incoming address masks. NOT IMPLEMENTED CURRENTLY*/
    swi_uint32 addr_masks_rx;

    swi_uint32 dest_unreach_rx; /*!< # of incoming destination unreachables*/
    swi_uint32 echo_rx;         /*!< # of incoming echo mesgs*/

    /*! # of incoming echo replies. NOT IMPLEMENTED CURRENTLY*/
    swi_uint32 echo_reply_rx;

    /*! # of incoming icmp messages with bad checksum*/
    swi_uint32 bad_chksum_rx;

    /*! # of incoming icmp messages with unsupported type  */
    swi_uint32 unsupported_type_rx;

    /*! # of incoming icmp parameter problem messages*/
    swi_uint32 parm_probs_rx;

    swi_uint32 bad_code_rx;     /*!< # of incoming messages with bad code*/
    
    /*! # of incoming redirects. note: NOT IMPLEMENTED CURRENTLY*/
    swi_uint32 redirects_rx;

    swi_uint32 src_quench_rx;   /*!< # of incoming source quench messages*/
    swi_uint32 time_exceeds_rx; /*!< # of incoming time exceeded messages*/
    
    /*! # of incoming timestamp replies. note: NOT IMPLEMENTED CURRENTLY*/
    swi_uint32 tstamp_reply_rx;

    swi_uint32 tstamp_rx;       /*!< # of incoming timestamps*/
    swi_uint32 msgs_tx;         /*!< # of outgoing icmp messages*/
    
    /*! # of outgoing addr mask replies. note: NOT IMPLEMENTED CURRENTLY*/
    swi_uint32 addr_mask_replies_tx;

    /*! # of outgoing address masks. note: NOT IMPLEMENTED CURRENTLY*/
    swi_uint32 addr_masks_tx;

    swi_uint32 dest_unreach_tx; /*!< # of outgoing destination unreachables*/
    swi_uint32 echo_tx;             /*!< # of outgoing echo mesgs*/
    swi_uint32 echo_reply_tx;   /*!< # of outgoing echo replies*/
    
    /*! # of outgoing icmp parameter problem messages*/
    swi_uint32 parm_probs_tx;

    swi_uint32 redirects_tx;    /*!< # of outgoing redirects*/
    swi_uint32 src_quench_tx;   /*!< # of outgoing source quench messages*/
    swi_uint32 time_exceeds_tx; /*!< # of outgoing time exceeded messages*/
    swi_uint32 tstamp_reply_tx; /*!< # of outgoing timestamp replies*/
    
    /*! # of outgoing timestamps. note: NOT IMPLEMENTED CURRENTLY*/
    swi_uint32 tstamp_tx;

    swi_uint32 rate_limit_tx;   /*!< # of unsent messages due to rate limit*/
    swi_uint32 msgs_dropped_tx; /*!< # of outgoing messages dropped*/
}SWI_STRUCT_Icmpv4Stats;

/**
 *
 * Contains TCP data.
 *
 */
typedef struct SWI_STRUCT_TcpStats
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */

    swi_uint32 crnt_open_conns;         /*!< Crnt open conns */
    swi_uint32 bad_hdr_len;             /*!< Bad hdr len */
    swi_uint32 bad_chksum;              /*!< Bad chksum */
    swi_uint32 too_short;               /*!< Too short */
    swi_uint32 conn_aborts;             /*!< Conn aborts */
    swi_uint32 conn_timeout_aborts;     /*!< Conn timeout aborts */
    swi_uint32 conn_keepalive_aborts;   /*!< Conn keepalive aborts */
    swi_uint32 rst_tx;                  /*!< Rst tx */
    swi_uint32 rst_rx;                  /*!< Rst rx */
    swi_uint32 dropped_segs_noconn_rx;  /*!< Dropped segs noconn rx */
    swi_uint32 dropped_segs_rx;         /*!< Dropped segs rx */
    swi_uint64 bytes_tx;                /*!< Bytes tx */
    swi_uint64 bytes_rx;                /*!< Bytes rx */
    swi_uint32 segs_tx;                 /*!< Segs tx */
    swi_uint32 segs_rx;                 /*!< Segs rx */
    swi_uint64 bytes_rexmit;            /*!< Bytes rexmit */
    swi_uint32 segs_rexmit;             /*!< Segs rexmit */        
    swi_uint32 dup_bytes_rx;            /*!< Dup bytes rx */
    swi_uint32 dup_segs_rx;             /*!< Dup segs rx */
    swi_uint32 window_probe_tx;         /*!< Window probe tx */
    swi_uint32 out_of_order_segs_rx;    /*!< Out of order segs rx */
    swi_uint32 out_of_order_bytes_rx;   /*!< Out of order bytes rx */
    swi_uint32 rexmit_timeouts;         /*!< Rexmit timeouts */
    swi_uint32 piggybacked_acks_tx;     /*!< Piggybacked acks tx */
    swi_uint32 piggybacked_acks_rx;     /*!< Piggybacked acks rx */
    swi_uint32 dup_acks_rx;             /*!< Dup acks rx */
    swi_uint32 dup_acks_tx;             /*!< Dup acks tx */
}SWI_STRUCT_TcpStats;

/**
 *
 * Contains UDP data.
 *
 */
typedef struct SWI_STRUCT_UdpStats
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */

    swi_uint32 bad_len;     /*!< Desc: # of datagrams with bad payload lengths*/
    swi_uint32 bad_chksum;  /*!< # of packets with bad header checksum */
    swi_uint32 dgram_rx;    /*!< # of incoming datagrams */
    swi_uint32 dgram_tx;    /*!< # of outgoing datagrams */
    swi_uint64 bytes_rx;    /*!< # of bytes received */
    swi_uint64 bytes_tx;    /*!< # of bytes transmitted */
    swi_uint32 no_port; /*!< # of incoming datagrams for an unreachable port */
    swi_uint32 dgram_dropped_rx;    /*!< # of incoming datagrams dropped */
    swi_uint32 dgram_dropped_tx;    /*!< # of outcoming datagrams dropped */
}SWI_STRUCT_UdpStats;

/*
 *
 * Name:    SWI_STRUCT_DataStatsFetch - statistics data
 *
 * Purpose:    This structure is used for getting statistics.
 *
 * Members: sizeStruct  - Size of this structure
 *          status - Data fetch status. Refer to SWI_TYPE_DataStatsFetchStatus
 *          protocol - Link protocol for data. Refer to SWI_TYPE_LinkProtocol
 *          protocol_union  - Union that contains statistics data
 *
 * Notes:    None
 *
 */
 
/**
 *
 * Contains statistics data.
 *
 */
typedef struct SWI_STRUCT_DataStatsFetch
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */

    /*! Data fetch status. Refer to SWI_TYPE_DataStatsFetchStatus */
    swi_uint8 status;

    /*! Link protocol for data. Refer to SWI_TYPE_LinkProtocol */
    swi_uint8 protocol;
  
    union
    {
        SWI_STRUCT_PppStats ppp;        /*!< See SWI_STRUCT_PppStats */
        SWI_STRUCT_Ipv4Stats ipv4;      /*!< See SWI_STRUCT_Ipv4Stats */
        SWI_STRUCT_Icmpv4Stats icmpv4;  /*!< See SWI_STRUCT_Icmpv4Stats */
        SWI_STRUCT_TcpStats tcp;        /*!< See SWI_STRUCT_TcpStats */
        SWI_STRUCT_UdpStats udp;        /*!< See SWI_STRUCT_UdpStats */
    }protocol_union;
}SWI_STRUCT_DataStatsFetch;

/**
 *
 * Contains statistics data.
 *
 */
typedef struct SWI_STRUCT_DataStatsReset
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */

    /*! Data reset result. Refer to SWI_TYPE_DataStatsResetResult */
    swi_uint8 result;
}SWI_STRUCT_DataStatsReset;


/* Below are from SwiStructsInternalCdma.h */

#define MAX_STRING_SIZE             240     /*!< Max string length */ 

#define MAX_PATH                    260     /*!< Max path lenfth */ 

#define LENGTH_NvDialDigits         32      /*!< NV dial digits length */
#define LENGTH_NvDialLetters        12      /*!< NV dial letter length */
#define LENGTH_NvLockCode           4       /*!< NV lock code length */
#define LENGTH_NvSecCode            6       /*!< NV sec code length */
#define LENGTH_NvFscCode            6       /*!< NV fsc code length */
#define LENGTH_NvWdcDate            5       /*!< NV wdc date length */
#define LENGTH_NvWdcExtra           3       /*!< NV wdc extra length */
#define LENGTH_NvPcsDirNum          15      /*!< NV PCS dir num length */
#define LENGTH_NvOtksl              6       /*!< NV OTASL length */
#define LENGTH_NvDialString         16      /*!< NV dial string length */
#define LENGTH_NvPapUserID          64      /*!< NV PAP user ID length */
#define LENGTH_NvPapPassword        14      /*!< NV PAP password length */
#define LENGTH_NvSipUserID          72      /*!< NV SIP user ID length */
#define LENGTH_NvSipPassword        16      /*!< NV SIP password length */
#define LENGTH_NvFsn                15      /*!< NV FSN length */
#define LENGTH_NvCalDate            8       /*!< NV CAL date length */
#define LENGTH_NvNam1ProvisionDate  8   /*!< NV NAM1 provision date length */
#define LENGTH_NvNam2ProvisionDate  8   /*!< NV NAM2 provision date length */
#define LENGTH_NvNai                72      /*!< NV NAI length */
#define LENGTH_NvMnHaSs             16      /*!< NV MnHaSS length */
#define LENGTH_NvMnAaaSs            16      /*!< NV MnAaaSS length */
#define LENGTH_NvPppUserID          127     /*!< NV PPP user ID length */
#define LENGTH_NvHdrUserID          127     /*!< NV HDR user ID length */

#define LENGTH_NvHdrPassword        127     /*!< NV HDR password length */

#define LENGTH_NvBannerText         40      /*!< NV banner text length */

#define SWI_LENGTH_DebugFileName    32      /*!< Max debug file name length */

/*! Max debug format string length */
#define SWI_LENGTH_DebugFormatString    256

#define LENGTH_DebugFormatString    40  /*!< Max debug format string length */
#define LENGTH_DebugFileName        13  /*!< Max debug file name length */

#define LENGTH_TechHwVersion        3       /*!< hardware version length */
#define LENGTH_EFilename            8       /*!< Max EFile file name length */
#define LENGTH_AKey                 20      /*!< A key length */
#define LENGTH_AKeyChecksum         26      /*!< A key checksum length */
#define LENGTH_DMU_MN_AUTH          4       /*!< DMU MC authentication length */
#define LENGTH_TechErrFileName      8       /*!< Max error file name length */
#define LENGTH_IPAddress            16      /*!< IP address length */
#define LENGTH_MIPSessionPwd        17  /*!< Max MIP session password length */
#define LENGTH_UserID               72      /*!< Max user ID length */
#define LENGTH_UserPassword         16      /*!< Max user password length */
#define LENGTH_Chv                  8       /*!< CHV length */
#define LENGTH_SetFeaturesKey       21      /*!< Max feature key length */

#define MAX_WalshCode               2       /*!< Max Walsh code length */
#define MAX_FingerSet               6       /*!< Max Finger set length */

/*! Max active set.  This must be larger than/same with MIP_MAX_PROFILES */
#define MAX_ActiveSet               6

#define MAX_EFileBlock              115     /*!< Max EFile block size */

#define MAXLEN_ConnectoidName       20  /*!< Max connection name length */
#define MAXLEN_SIPUserIDFormat      72  /*!< Max FSIP user ID format length */

#define NUMBER_TechIcmpStat         8   /*!< ICMP statistics data number */
#define NUMBER_TechUdpStat          3   /*!< UDP statistics data number */
#define NUMBER_TechTcpStat          10  /*!< TCP statistics data number */
#define NUMBER_TechIpStat           5   /*!< IP statistics data number */
#define NUMBER_TechPppStat          18  /*!< PPP statistics data number */
#define NUMBER_TechRlpStat          19  /*!< RLP statistics data number */
#define NUMBER_TechRlp33Stat        17  /*!< RLP33 statistics data number */

#define NV_MAX_NAI_PREFIX           64  /*!< NV Max NAI prefix length */
#define NV_MAX_NAI_SUFFIX           64  /*!< NV Max NAI surfix length */

#define SWI_EVDO_UATI_SIZE          17  /*!< Max EVDO UATI size */

/*! Max secure challenge response length */
#define LENGTH_SecureChallengeResponse  10

/**
 *
 * Contains debug message package notification data.
 *
 * This data structure is used as a member of the notification 
 * SWI_NOTIFY_DebugRevMsg.  Refer to the description of
 * SWI_NOTIFY_DebugRevMsg for more detail.
 *
 */
typedef struct SWI_STRUCT_DebugPacket
{
    swi_uint32 sizeStruct;      /*!< Size of this structure */

    swi_uint32 nCommandCode;    /*!< Command code */
    swi_uint32 nNumberMsg;      /*!< Number of message */
    swi_uint32 nDroppedMsg;     /*!< Dropped message */
    swi_uint32 nTotalMsg;       /*!< Total message */
    swi_uint32 nSeverityLevel;  /*!< Severity level */
    swi_char szFileName[LENGTH_DebugFileName + 1];         /*!< File name */
    swi_uint32 nLineNumber;     /*!< Line number */
    swi_char szFormatString[LENGTH_DebugFormatString + 1]; /*!< String format */
    swi_uint32 nParameter1;     /*!< Parameter 1 */
    swi_uint32 nParameter2;     /*!< Parameter 2 */
    swi_uint32 nParameter3;     /*!< Parameter 3 */
}SWI_STRUCT_DebugPacket;

/**
 * Definition of the debug packet structure
 *
 * The main reason for creating this structure was to increase the length
 *  of the filename and description string which is very useful for logging 
 *  (the SWI_STRUCT_DebugPacket only allows 13 chars for filename and
 *  40 chars for description - now increased to 32 and 256, respectively)
 * 
 */
typedef struct _STRUCT_DebugPacket
{
    /*! Debug packet structure defined by FW */
    SWI_STRUCT_DebugPacket fwDebugPacket;
        
    /*! String format */
    swi_char szFormatString[SWI_LENGTH_DebugFormatString + 1];
    
    swi_char szFileName[SWI_LENGTH_DebugFileName + 1];     /*!< File name */
} STRUCT_DebugPacket;

/**
 *
 * An enumeration of interface mode. 
 *
 */
typedef enum SWI_INTERFACE_MODE
{
    SWI_INTERFACE_DTR       = 0x00,     /*!< DTR */
    SWI_INTERFACE_RADIO     = 0x01      /*!< Radio */
}SWI_INTERFACE_MODE;

/**
 *
 * Contains single Finger data.
 *
 */
typedef struct SWI_STRUCT_TechFingerSingle
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */
    swi_int32 iPilot;                       /*!< Pilot */
    swi_uint32 nWalshCode[MAX_WalshCode];   /*!< Wals code */
    swi_int32 iRssi;                        /*!< RSSI */
}SWI_STRUCT_TechFingerSingle;

/**
 *
 * Contains Finger data.
 *
 */
typedef struct SWI_STRUCT_TechFinger
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */
    swi_uint32 nCount;              /*!< Count */

    /*! See SWI_STRUCT_TechFingerSingle */
    SWI_STRUCT_TechFingerSingle FingerSet[MAX_FingerSet];
}SWI_STRUCT_TechFinger;

/**
 *
 * An enumeration of Dialpad keys. 
 *
 * This enumeration type identifies the dial pad keys.
 *
 * Members: SWI_PHONE_KEY_Pound            - '#' key, ASCII '#'
 *          SWI_PHONE_KEY_Star             - '*' key, ASCII '*'
 *          SWI_PHONE_KEY_0                - '0' key, ASCII '0'
 *          SWI_PHONE_KEY_1                - '1' key, ASCII '1'
 *          SWI_PHONE_KEY_2                - '2' key, ASCII '2'
 *          SWI_PHONE_KEY_3                - '3' key, ASCII '3'
 *          SWI_PHONE_KEY_4                - '4' key, ASCII '4'
 *          SWI_PHONE_KEY_5                - '5' key, ASCII '5'
 *          SWI_PHONE_KEY_6                - '6' key, ASCII '6'
 *          SWI_PHONE_KEY_7                - '7' key, ASCII '7'
 *          SWI_PHONE_KEY_8                - '8' key, ASCII '8'
 *          SWI_PHONE_KEY_9                - '9' key, ASCII '9'
 *          SWI_PHONE_KEY_Send             - Send key
 *          SWI_PHONE_KEY_End       - End key or Power key (Based on Target)
 *          SWI_PHONE_KEY_Clear            - Clear key
 *          SWI_PHONE_KEY_Store            - Store key
 *          SWI_PHONE_KEY_Up               - Up-arrow key was pressed
 *          SWI_PHONE_KEY_Down             - Down-arrow key was pressed
 *          SWI_PHONE_KEY_Mute             - Mute Key
 *          SWI_PHONE_KEY_Recall           - Recall key
 *     
 * Only digit keys 0-9, #, and * have any real use (i.e., DTMF tones are 
 * played accordingly)
 *
 * Command keys (Send, End, Recall, etc) have no effect and  are not used
 *
 */
typedef enum SWI_TYPE_PhoneKey
{
    SWI_PHONE_KEY_Pound     = 0x23,
    SWI_PHONE_KEY_Star      = 0x2A,        
    SWI_PHONE_KEY_0         = 0x30,     
    SWI_PHONE_KEY_1         = 0x31,     
    SWI_PHONE_KEY_2         = 0x32,     
    SWI_PHONE_KEY_3         = 0x33,     
    SWI_PHONE_KEY_4         = 0x34,     
    SWI_PHONE_KEY_5         = 0x35,     
    SWI_PHONE_KEY_6         = 0x36,     
    SWI_PHONE_KEY_7         = 0x37,     
    SWI_PHONE_KEY_8         = 0x38,     
    SWI_PHONE_KEY_9         = 0x39,     
    SWI_PHONE_KEY_Send      = 0x50,     
    SWI_PHONE_KEY_End       = 0x51,     
    SWI_PHONE_KEY_Clear     = 0x52,     
    SWI_PHONE_KEY_Store     = 0x53,     
    SWI_PHONE_KEY_Up        = 0x54,     
    SWI_PHONE_KEY_Down      = 0x55,     
    SWI_PHONE_KEY_Mute      = 0x56,     
    SWI_PHONE_KEY_Recall    = 0x57     
}SWI_TYPE_PhoneKey;

/**
 *
    nv_auto_populate_info_ap_type in FW is different(FW uses swi_uint8 for 
    index, duration and trigger). FW tend to extend swi_uint8 
    data(except array) to swi_uint16.
*/
typedef struct nv_auto_populate_info_ap_type{
    swi_uint16 index;               /*!< Index */
    swi_uint16 duration;            /*!< nv_auto_populate_duration_type */
    swi_uint16 trigger;             /*!< nv_auto_populate_trigger_type */
    swi_uint8 host_indicators[4];   /*!< Used pure by host definitions */
} nv_auto_populate_info_ap_type;

#define SWI_LED_MAX_BLINK_TYPES   5     /*!< Max LED blink types */

/**
 *
 * Contains LED config table.
 *
 */
typedef struct SWI_STRUCT_LED_CFG_TBL
{
  swi_uint16 index;         /*!< Index */
  swi_uint32 gpio;          /*!< Gpio */
  swi_uint8 inverted;       /*!< Inverted */
  swi_uint8 led_type;       /*!< LED type */
  swi_uint16 reserved;      /*!< Reserved */
  swi_uint32 led_blink;     /*!< LED blink */
  swi_uint32 blink_period[SWI_LED_MAX_BLINK_TYPES]; /*!< Blink period */
  swi_uint32 blink_rate[SWI_LED_MAX_BLINK_TYPES];   /*!< Blink rate */
}SWI_STRUCT_LED_CFG_TBL;

/**
 *
 * Contains HDR SCP subtype custom config data.
 *
 */
typedef struct SWI_STRUCT_HDRSCP_Subtype_Custom_Config
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */

    /*! 1: customer config active; otherwise not active */
    swi_bool bIsActive;
    
    swi_uint32 nProtSubtypeMask;    /*!< Prot subtype mask */
    swi_uint32 nBcmcsSubtypeMask;   /*!< Bcmcs subtype mask */
    swi_uint32 nAppSubtypeMask;     /*!< App subtype mask */
}SWI_STRUCT_HDRSCP_Subtype_Custom_Config;

/**
 *
 * Contains HDR SCP session status data.
 *
 */
typedef struct SWI_STRUCT_HDRSCP_Session_Status
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */
    swi_uint16 nSessionStatus;      /*!< Session status */
}SWI_STRUCT_HDRSCP_Session_Status;

/**
 *
 * An enumeration of GUI Settings mask.
 *
 */
typedef enum SWI_TYPE_PRI_GUIMask
{
    SWI_PRI_GUI_LOGOS               = 0x0001,   /*!< carrier-specific logos */

    /*! carrier-specific trademark text */
    SWI_PRI_GUI_TEXT                = 0x0002,

    SWI_PRI_GUI_DORMANCY            = 0x0004,    /*!< Dormancy GUI support */
    SWI_PRI_GUI_CALLGUARD           = 0x0008,    /*!< Call Guard feature */
    SWI_PRI_GUI_WARN_VOICECALL      = 0x0010,    /*!< Voice call warning */
    
    /*! Allow IMSI to be visible thru-out watcher (e.g. About Screen) */
    SWI_PRI_GUI_SHOW_IMSI           = 0x0020,
    
    /*! Cumulative Byte Counter in the Call Log */
    SWI_PRI_GUI_SHOW_BYTE_COUNTER   = 0x0040,

    SWI_PRI_GUI_RUIM_SUPPORT        = 0x0080,   /*!< RUIM support enabled */
    SWI_PRI_GUI_SINGLENAM           = 0x0100,    /*!< Single NAM support */
    SWI_PRI_GUI_SWI_LOGO            = 0x0200,    /*!< SWI specific logo */
    
    /*! Connection Manager enable indicator */
    SWI_PRI_GUI_CM                  = 0x0400,

    SWI_PRI_GUI_NET_MODE            = 0x0800,    /*! Network mode selection */
    
    /*! Dormant Disconnect feature */
    SWI_PRI_GUI_DORM_DISC           = 0x1000,
    
    SWI_PRI_GUI_ALL                 = 0xFFFF,   /*!< All enabled */
    SWI_PRI_GUI_MASK_ENABLED        = 0x8000    /*!< Enabled bit */
}SWI_TYPE_PRI_GUIMask;

/**
 *
 * An enumeration of Activation Wizard mask.
 *
 * Activation Wizard disabled if none of the AW_OPT bits are set
 *
 */
typedef enum SWI_TYPE_PRI_ActivWizMask
{
    /*! Enable manual provisioning option */
    SWI_PRI_AW_OPT_MANUAL           = 0x0001,

    /*! Enable Voice assisted option (Exclusive to AW_OPT_AUTOMATED) */
    SWI_PRI_AW_OPT_VOICEASSIST      = 0x0002,

    /*! Enable Automated option (Exclusive to AW_OPT_VOICEAST) */
    SWI_PRI_AW_OPT_AUTOMATED        = 0x0004,

    /*! Enable headset requirement    (used only with AW_OPT_VOICEAST or 
        AW_OPT_AUTOMATED) */
    SWI_PRI_AW_HEADSET              = 0x0008,

    /*! Enable "Call service provider..." screen */
    SWI_PRI_AW_DLG_CALLCARRIER      = 0x0010,

    /*! ( reserved, set manually via old CNS SWINV 39 ) Enable SID screen */
    SWI_PRI_AW_DLG_SID              = 0x0020,

    /*! ( reserved, set manually via old CNS SWINV 40 ) Enable QNC user/pw 
        screen */
    SWI_PRI_AW_DLG_QNCUSERPW        = 0x0040,

    /*! Enable user to set IMSI (independent of MDN) 
        (Exclusive to AW_SET_IMSI_XX) */
    SWI_PRI_AW_SET_IMSI             = 0x0080,

    /*! Automatically set IMSI (to equal the MDN) (Exclusive to AW_SET_IMSI_XX) */
    SWI_PRI_AW_SET_IMSI_EQ_MDN      = 0x0100,

    /*! Enable 1x username / pw */
    SWI_PRI_AW_DLG_1XUSERPW         = 0x0200,

    /*! Enable dialpad for assisted/automated activation */
    SWI_PRI_AW_OTASP_DIALPAD        = 0x0800,

    /*! Bypass OTKSL entry */
    SWI_PRI_AW_BYPASS_OTKSL         = 0x1000,

    /*! Bypass Introduction Screen */
    SWI_PRI_AW_BYPASS_INTRO_SCREEN  = 0x2000,

    /*! Combined mask of SWI_PRI_AW_OPT_MANUAL, SWI_PRI_AW_OPT_VOICEASSIST and 
        SWI_PRI_AW_OPT_AUTOMATED */
    SWI_PRI_AW_ENABLED              = SWI_PRI_AW_OPT_MANUAL | 
                                      SWI_PRI_AW_OPT_VOICEASSIST | 
                                      SWI_PRI_AW_OPT_AUTOMATED,

    SWI_PRI_AW_ALL                  = 0xFFFF,   /*!< All enabled */
    SWI_PRI_AW_MASK_ENABLED         = 0x8000    /*!< Enabled bit */
}SWI_TYPE_PRI_ActivWizMask;

/**
 *
 * An enumeration of data warning mask.
 *
 */
typedef enum SWI_TYPE_PRI_DataWarningMask
{
    SWI_PRI_WARNING_1x      = 0x0001,    /*!< Enable 1x usage warning */
    SWI_PRI_WARNING_QNC     = 0x0002,    /*!< Enable QNC usage warning */
    SWI_PRI_WARNING_ALL     = 0xFFFF    /*!< All enabled */

    /* 'Enabled' bit is NOT needed for this mask because we are porting
        the NV over from an existing PRI NV item. */
    /* SWI_PRI_SMS_MASK_ENABLED = 0x8000 */    /* Enabled bit */
}SWI_TYPE_PRI_DataWarningMask;

/**
 *
 * An enumeration of feature key mask.
 *
 */
typedef enum SWI_TYPE_FEATURE_MASK
{
    SWI_FM_WIFI         = 0x01,     /*!< Support WiFi manager */
    SWI_FM_OUTLOOK      = 0x02,     /*!< Support Outlook integration */
    SWI_FM_APPLAUNCH    = 0x04,     /*!< Support application auto launch */
    SWI_FM_ADVANCE      = 0x08,     /*!< Upgradeable to Advanced */
    SWI_FM_ITMANAGER    = 0x10000,  /*!< Support IT manager */
    SWI_FM_EAPSIM       = 0x20000,  /*!< Support EAP-SIM */

    /*! Support all feature */
    SWI_FM_ALL          = SWI_FM_WIFI|SWI_FM_OUTLOOK | 
                          SWI_FM_APPLAUNCH | 
                          SWI_FM_ADVANCE | 
                          SWI_FM_ITMANAGER | 
                          SWI_FM_EAPSIM
}SWI_TYPE_FEATURE_MASK;

/* Function prototypes */

SWI_API SWI_RCODE SwiSetCallOriginateSoCmd(swi_uint16 SOValue, 
                                           const swi_charp szCallNumber, 
                                           swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetCallNotificationStatus(swi_uint16* NotifStatus, 
                                               swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetCallExtendedCallerID(swi_charp szExtndedCallerID, 
                                             swi_uint32 iLength, 
                                             swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetSecureChallenge(const swi_charp pszSCD);
SWI_API SWI_RCODE SwiSetSecureRebuttal(const swi_charp pszSecureRebuttal);

SWI_API SWI_RCODE SwiSetSecureCommitUpdate(swi_uint16 commitType, 
                                           swi_uint16 namIndex, 
                                           swi_uint16* commitResult, 
                                           swi_uint16* failureMask);

SWI_API SWI_RCODE SwiSetProvisioningDate(swi_uint16 namIndex, 
                                         const swi_charp provisoningDate);

SWI_API SWI_RCODE SwiGetChannelStateEx(swi_uint16* State, swi_uint32 timeout);
SWI_API SWI_RCODE SwiGetPriChksum(swi_uint16* pPriChksum, swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetV42BISEnableStatus(swi_uint16* pV42BISEnableStatus, 
                                           swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetAuthenticationStatus(swi_uint16* pAuthenticationStatus, 
                                             swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetTotalCallDuration(swi_uint32* pTotalCallDuration, 
                                          swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetCumulativeByteCounter(swi_uint32 *Sent, 
                                              swi_uint32 *Received, 
                                              swi_uint32 timeout);

SWI_API SWI_RCODE SwiResetCumulativeByteCounter(swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetCNSModeStatus(swi_uint16* pCnsModeStatus, 
                                      swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetDebugStatus(swi_uint16* pDebugStatus, 
                                    swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetDebugMsgLevel(swi_uint16* pDebugMsgLevel, 
                                      swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetSpeakerLevel(swi_uint16* pSpeakerLevel, 
                                     swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetMICLevel(swi_uint16* pMICLevel, swi_uint32 timeout);
SWI_API SWI_RCODE SwiGetDTMFLevel(swi_uint16* pDTMFLevel, swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetPort(swi_uint8* pDiagPort, 
                             swi_uint8* pDataPort, 
                             swi_uint8* pBluetoothPort, 
                             swi_uint8* pMultimediaPort, 
                             swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetEchoCancelProfile(swi_uint16* pEchoCancelProfile, 
                                          swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetPhoneNumber(const swi_charp PhoneNumber, 
                                    swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetV42BISEnableStatus(swi_uint16 V42BISEnableStatus, 
                                           swi_uint32 timeout);

SWI_API SWI_RCODE SwiResetByteCounter();

SWI_API SWI_RCODE SwiSetCNSModeStatus(swi_uint16 CnsModeStatus, 
                                      swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetDebugStatus(swi_uint16 DebugStatus, 
                                    swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetDebugMsgLevel(swi_uint16 DebugMsgLevel, 
                                      swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetSpeakerLevel(swi_uint16 SpeakerLevel, 
                                     swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetMICLevel(swi_uint16 MICLevel, swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetDTMFBurst(const swi_charp DTMFBurst, 
                                  swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetDTMFLevel(swi_uint16 DTMFLevel, swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetEchoCancelProfile(swi_uint16 EchoCancelProfile, 
                                          swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetPort(swi_uint8 DiagPort, 
                             swi_uint8 DataPort, 
                             swi_uint8 BluetoothPort, 
                             swi_uint8 MultimediaPort, 
                             swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetSelectNam(swi_uint16 namIndex, swi_uint32 timeout);
SWI_API SWI_RCODE SwiGetSelectNam(swi_uint16* namIndex, swi_uint32 timeout);
SWI_API SWI_RCODE SwiGetActiveNam(swi_uint16 *namIndex, swi_uint32 timeout);
SWI_API SWI_RCODE SwiGetAutoNam(swi_bool *bAutoNam, swi_uint32 timeout);
SWI_API SWI_RCODE SwiSetAutoNam(swi_bool bAutoNam, swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetApiFirmwareHandshake(swi_bool bApiConnected, 
                                             swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetApiFirmwareHandshake(swi_bool *bApiConnected, 
                                             swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetPowerDown();
SWI_API SWI_RCODE SwiSetRadioEnable(swi_bool bEnable, swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetTechBandClass(swi_uint16 *bandClass, 
                                      swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetTechRegZone(swi_uint16 *zoneID, swi_uint32 timeout);
SWI_API SWI_RCODE SwiGetTechPCSBlock(swi_uint16 *pcs, swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetTechCellChannel(swi_uint16 *channel, 
                                        swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetNetworkUserID(swi_uint16 profileType, 
                                      swi_uint16 profileIndex, 
                                      const swi_charp pszUserID, 
                                      swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetNetworkUserID(swi_uint16 profileType, 
                                      swi_uint16 profileIndex, 
                                      swi_charp pszUserID, 
                                      swi_uint32 length, 
                                      swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetNetworkUserPassword(swi_uint16 profileType, 
                                            swi_uint16 profileIndex, 
                                            const swi_charp pszUserPassword, 
                                            swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetMIPSessionPassword(swi_uint16 profileIndex, 
                                           const swi_charp pszUserPassword, 
                                           swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetNextMipProfileIndex(swi_uint16 *mipProfileIndex, 
                                            swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetNextSipProfileIndex(swi_uint16 *sipProfileIndex, 
                                            swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetActiveProfile(swi_uint16 profileType, 
                                      swi_uint16 profileIndex, 
                                      swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetActiveProfile(swi_uint16 *profileType, 
                                      swi_uint16 *profileIndex, 
                                      swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetHdrScpCustomerConfig(
        SWI_STRUCT_HDRSCP_Subtype_Custom_Config *pSubtypeCustomConfig, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetHdrScpCustomerConfig(
        const SWI_STRUCT_HDRSCP_Subtype_Custom_Config *pSubtypeCustomConfig, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetHdrScpSessionStatus(
        swi_uint16 Nam, 
        SWI_STRUCT_HDRSCP_Session_Status *pSessionStatus, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetHdrScpSessionStatus(
        swi_uint16 Nam, 
        const SWI_STRUCT_HDRSCP_Session_Status *pSessionStatus, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetTechHdrSuFwdStats(
        const SWI_STRUCT_Tech_Hdr_Su_Fwd_Stats *pTechHdrSuFwdStats, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetTechHdrSuFwdSlotCnt(
        const SWI_STRUCT_Tech_Hdr_Su_Fwd_Slot_Cnt *pTechHdrSuFwdSlotCnt, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetTechHdrMuFwdStats(
        const SWI_STRUCT_Tech_Hdr_Mu_Fwd_Stats *pTechHdrMuFwdStats, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetTechHdrPersonality(
        SWI_STRUCT_Tech_Hdr_Personality *pTechHdrPersonality, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetTechHdrProtAppSubtype(
        SWI_STRUCT_Tech_Hdr_Prot_App_Subtype *pTechHdrProtAppSubtype, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetTechHdrSleepParms(
        SWI_STRUCT_Tech_Hdr_Sleep_Parms *pTechHdrSleepParms, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetDisplayBannerName(swi_charp pszDisplayBannerName, 
                                             swi_uint32 szLength, 
                                             swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetSci(swi_uint16 Sci, swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetMobCaiRev(swi_uint16 *MobCaiRev, 
                                     swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetMobCaiRev(swi_uint16 MobCaiRev, swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetPrefServ(swi_uint16 Nam, swi_uint16 *PrefService, 
                                    swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetPrimaryChan(swi_uint16 Nam, 
                                       swi_uint16* ChannelA, 
                                       swi_uint16* ChannelB, 
                                       swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetPrimaryChan(swi_uint16 Nam, 
                                       swi_uint16 ChannelA, 
                                       swi_uint16 ChannelB, 
                                       swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetSecondaryChan(swi_uint16 Nam, 
                                         swi_uint16* ChannelA, 
                                         swi_uint16* ChannelB, 
                                         swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetSecondaryChan(swi_uint16 Nam, 
                                         swi_uint16 ChannelA, 
                                         swi_uint16 ChannelB, 
                                         swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetTermHome(swi_uint16 Nam, 
                                    swi_uint16 *TermHomeEnable, 
                                    swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetTermHome(swi_uint16 Nam, 
                                    swi_uint16 TermHomeEnable, 
                                    swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetSidNid(swi_uint16 Nam, 
                                  swi_uint16 *Sid, 
                                  swi_uint16 *Nid, 
                                  swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetSidNid(swi_uint16 Nam, 
                                  swi_uint16 Sid, 
                                  swi_uint16 Nid, 
                                  swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetAutoNam(swi_uint16 *AutoNam, 
                                   swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetAutoNam(swi_uint16 AutoNam, 
                                   swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetAutoRedial(swi_uint16 *Enable, 
                                      swi_uint16 *Rings, 
                                      swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetAutoRedial(swi_uint16 Enable, 
                                      swi_uint16 Rings, 
                                      swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetServProgCode(swi_charp ServProgCode, 
                                        swi_uint32 szLength, 
                                        swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetServProgCode(const swi_charp ServProgCode, 
                                        swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetFieldServCode(swi_charp pszFsc, 
                                         swi_uint32 szLength, 
                                         swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetFieldServCode(const swi_charp pszFsc, 
                                         swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetWarDateCode(swi_charp pszWarranty, 
                                       swi_uint32 lengthWarranty, 
                                       swi_charp pszExtra, 
                                       swi_uint32 lengthExtra, 
                                       swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetWarDateCode(const swi_charp pszWarranty, 
                                       const swi_charp pszExtra, 
                                       swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetImsiAddrNum(swi_uint16 Nam, 
                                       swi_uint16 *AddrNum, 
                                       swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetImsiAddrNum(swi_uint16 Nam, 
                                       swi_uint16 AddrNum, 
                                       swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetDirNumPcs(swi_uint16 Nam, 
                                     swi_charp pszDirNumPcs, 
                                     swi_uint32 szLength, 
                                     swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetDirNumPcs(swi_uint16 Nam, 
                                     const swi_charp pszDirNumPcs, 
                                     swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetOneTimeSubsidyLock(swi_charp pszOneTimeSubsidyLock, 
                                              swi_uint32 szLength, 
                                              swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetOneTimeSubsidyLock(
        const swi_charp pszOneTimeSubsidyLock, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetImsiTS1(swi_uint16 Nam, 
                                   swi_uint32 *ImsiTS1, 
                                   swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetImsiTS1(swi_uint16 Nam, 
                                   swi_uint32 ImsiTS1, 
                                   swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetImsiTS2(swi_uint16 Nam, 
                                   swi_uint16 *ImsiTS2, 
                                   swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetImsiTS2(swi_uint16 Nam, 
                                   swi_uint16 ImsiTS2, 
                                   swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetTrueImsiMcc(swi_uint16 Nam, 
                                       swi_uint16 *ImsiMcc, 
                                       swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetTrueImsiMcc(swi_uint16 Nam, 
                                       swi_uint16 ImsiMcc, 
                                       swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetTrueImsi1112(swi_uint16 Nam, 
                                        swi_uint16 *Imsi1112, 
                                        swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetTrueImsi1112(swi_uint16 Nam, 
                                        swi_uint16 Imsi1112, 
                                        swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetSpcChgEnabled(swi_uint16 *Enable, 
                                         swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetSpcChgEnabled(swi_uint16 Enable, 
                                         swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetPapUserId(swi_charp pszUserId, 
                                     swi_uint32 szLength, 
                                     swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetPapUserId(const swi_charp pszUserId, 
                                     swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetPapPassword(swi_charp pszPassword, 
                                       swi_uint32 szLength, 
                                       swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetPapPassword(const swi_charp pszPassword, 
                                       swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetPrefRC(swi_uint16* pnRC, swi_uint32 timeout);
SWI_API SWI_RCODE SwiPstSetPrefRC(swi_uint16 nRC, swi_uint32 timeout);
SWI_API SWI_RCODE SwiPstGetPrimaryDns(swi_uint32 *Dns, swi_uint32 timeout);
SWI_API SWI_RCODE SwiPstSetPrimaryDns(swi_uint32 Dns, swi_uint32 timeout);
SWI_API SWI_RCODE SwiPstGetSecondaryDns(swi_uint32 *Dns, swi_uint32 timeout);
SWI_API SWI_RCODE SwiPstSetSecondaryDns(swi_uint32 Dns, swi_uint32 timeout);
SWI_API SWI_RCODE SwiPstGetScrmEnabled(swi_uint16 *Enable, swi_uint32 timeout);
SWI_API SWI_RCODE SwiPstSetScrmEnabled(swi_uint16 Enable, swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetDialPhonebook(swi_uint16 address, 
                                         swi_uint16* status, 
                                         swi_uint16* numberOfDigits, 
                                         swi_charp pszDigits, 
                                         swi_uint32 szDigitsLength, 
                                         swi_charp pszName, 
                                         swi_uint32 szNameLength, 
                                         swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetDialPhonebook(swi_uint16 address, 
                                         swi_uint16 status, 
                                         swi_uint16 numberOfDigits, 
                                         const swi_charp pszDigits, 
                                         const swi_charp pszName, 
                                         swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetVoicePrivacy(swi_uint16* voicePrivacy, 
                                        swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetVoicePrivacy(swi_uint16 voicePrivacy, 
                                        swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetQncCallEnable(swi_uint16* enableQnc, 
                                         swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetQncCallEnable(swi_uint16 enableQnc, 
                                         swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetDataCallOption(swi_uint16* dataServiceOption, 
                                          swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetDataCallOption(swi_uint16 dataServiceOption, 
                                          swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetEvrcVoiceService(swi_uint16 Nam, 
                                            swi_uint16* enable, 
                                            swi_uint16* homePageOption, 
                                            swi_uint16* homeOrigOption, 
                                            swi_uint16* roamOrigOption, 
                                            swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetEvrcVoiceService(swi_uint16 Nam, 
                                            swi_uint16 enable, 
                                            swi_uint16 homePageOption, 
                                            swi_uint16 homeOrigOption, 
                                            swi_uint16 roamOrigOption, 
                                            swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetMdrDataMode(swi_uint16* modeDataMdr, 
                                       swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetMdrDataMode(swi_uint16 modeDataMdr, 
                                       swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetPktCallDialString(swi_charp pszDialString, 
                                             swi_uint32 szLength, 
                                             swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetPktCallDialString(const swi_charp pszDialString, 
                                             swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetKeyDtmf(swi_uint16* keyDtmf, swi_uint32 timeout);
SWI_API SWI_RCODE SwiPstSetKeyDtmf(swi_uint16 keyDtmf, swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetCallFadeAlert(swi_uint16* fadeAlert, 
                                         swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetCallFadeAlert(swi_uint16 fadeAlert, 
                                         swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetModemAccessLock(swi_uint16* accessLock, 
                                           swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetModemAccessLock(swi_uint16 accessLock, 
                                           swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetModemAccessLockCode(swi_charp pszLockCode, 
                                               swi_uint32 szLength, 
                                               swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetModemAccessLockCode(const swi_charp pszLockCode, 
                                               swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetModemAutoPowerLock(swi_uint16* autoPowerLock, 
                                              swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetModemAutoPowerLock(swi_uint16 autoPowerLock, 
                                              swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetMessageAlertVolume(swi_uint16* alertVolume, 
                                              swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetMessageAlertVolume(swi_uint16 alertVolume, 
                                              swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetKeyBeepVolume(swi_uint16* beepVolume, 
                                         swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetKeyBeepVolume(swi_uint16 beepVolume, 
                                         swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetEarPieceVolume(swi_uint16* pieceVolume, 
                                          swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetEarPieceVolume(swi_uint16 pieceVolume, 
                                          swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetVoicePrivacyAlert(swi_uint16* privacyAlert, 
                                             swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetVoicePrivacyAlert(swi_uint16 privacyAlert, 
                                             swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetPrlEnable(swi_uint16* prlEnable, 
                                     swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetPrlEnable(swi_uint16 prlEnable, swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetSmsTimeFormat(swi_uint16* smsUtcTime, 
                                         swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetSmsTimeFormat(swi_uint16 smsUtcTime, 
                                         swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetRoamRinger(swi_uint16* roamRinger, 
                                      swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetRoamRinger(swi_uint16 roamRinger, 
                                      swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetDataPktAutoDetect(swi_uint16* pktAutoDetect, 
                                             swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetDataPktAutoDetect(swi_uint16 pktAutoDetect, 
                                             swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetSmsReminderTone(swi_uint16* reminderTone, 
                                           swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetSmsReminderTone(swi_uint16 reminderTone, 
                                           swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetTempThreshold(swi_uint16 *tempThreshold, 
                                         swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetTempThreshold(swi_uint16 tempThreshold, 
                                         swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetFsn(swi_charp pszFsn, 
                               swi_uint32 szLength, 
                               swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetFsn(const swi_charp pszFsn, swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetCalibrationDate(swi_charp pszCalDate, 
                                           swi_uint32 szLength, 
                                           swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetCalibrationDate(const swi_charp pszCalDate, 
                                           swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetTestHistory(swi_uint16* testHistory, 
                                       swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetTestHistory(swi_uint16 testHistory, 
                                       swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetProvisionInfo(swi_uint32* nam1ProvFlag, 
                                         swi_charp pszNam1ProvDate, 
                                         swi_uint32 szLengthNam1, 
                                         swi_uint32* nam2ProvFlag, 
                                         swi_charp pszNam2ProvDate, 
                                         swi_uint32 szLengthNam2, 
                                         swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetProvisionInfo(swi_uint32 nam1ProvFlag, 
                                         const swi_charp pszNam1ProvDate, 
                                         swi_uint32 nam2ProvFlag, 
                                         const swi_charp pszNam2ProvDate, 
                                         swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstMSLUnlock(swi_uint32 SubsidyLock, swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetTechFinger(SWI_STRUCT_TechFinger* TechFing, 
                                   swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetE911Info(swi_uint16 e911Number, 
                                 swi_uint16* startLocation, 
                                 swi_uint16* endLocation, 
                                 swi_uint16* programMask, 
                                 swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetAudioMode(swi_uint16* audioMode, swi_uint32 timeout);
SWI_API SWI_RCODE SwiSetAudioMode(swi_uint16 audioMode, swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetTechRlpStat(swi_uint16 techRlpStat[NUMBER_TechRlpStat], 
                                    swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetTechRlpStat(swi_uint16 techRlpStat[NUMBER_TechRlpStat], 
                                    swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetTechRlp33Stat(
        swi_uint32 techRlp33Stat[NUMBER_TechRlp33Stat], 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetTechRlp33Stat(
        swi_uint32 techRlp33Stat[NUMBER_TechRlp33Stat], 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetDtmfMode(swi_uint16* DtmfMode, swi_uint32 timeout);
SWI_API SWI_RCODE SwiSetDtmfMode(swi_uint16 DtmfMode, swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetKeyDtmfStart(swi_uint16 startKeyDtmf, 
                                     swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetKeyDtmfStop();

SWI_API SWI_RCODE SwiGetTtyOption(swi_uint16* ttyOption, swi_uint32 timeout);
SWI_API SWI_RCODE SwiSetTtyOption(swi_uint16 ttyOption, swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetTechIcmpStat(
        swi_uint16 techIcmpStat[NUMBER_TechIcmpStat], 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetTechIcmpStat(
        swi_uint16 techIcmpStat[NUMBER_TechIcmpStat], 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetTechUdpStat(
        swi_uint16 techUdpStat[NUMBER_TechUdpStat], 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetTechUdpStat(
        swi_uint16 techUdpStat[NUMBER_TechUdpStat], 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetTechTcpStat(
        swi_uint16 techTcpStat[NUMBER_TechTcpStat], 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetTechTcpStat(
        swi_uint16 techTcpStat[NUMBER_TechTcpStat], 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetTechIpStat(swi_uint16 techIpStat[NUMBER_TechIpStat], 
                                   swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetTechIpStat(swi_uint16 techIpStat[NUMBER_TechIpStat], 
                                   swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetTechPppStat(
        swi_uint16 techPppStat[NUMBER_TechPppStat], 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetTechPppStat(
        swi_uint16 techPppStat[NUMBER_TechPppStat], 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetTechErrorNumber(swi_uint16* errorNumber, 
                                        swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetTechErrorStat(swi_uint16 errSeqNumber, 
                                      swi_uint8* errFatalFlag, 
                                      swi_uint8* errCount, 
                                      swi_charp errFilename, 
                                      swi_uint32 length, 
                                      swi_uint16* errLineNumber, 
                                      swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetTechErrorStat(swi_uint16 errSeqNumber, 
                                      swi_uint8 errFatalFlag, 
                                      swi_uint8 errCount, 
                                      const swi_charp errFilename, 
                                      swi_uint16 errLineNumber, 
                                      swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetTemperatureThreshold(swi_uint16* tempThreshold, 
                                             swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetTemperatureThreshold(swi_uint16 tempThreshold, 
                                             swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetIncomingCallType(swi_uint16* callType, 
                                         swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetReceivePower(swi_int16* RxPower, swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetTechHardwareVersion(swi_charp pszHardwareVersion, 
                                            swi_uint32 szLength, 
                                            swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetSidetoneGainLevel(swi_uint16* sidetoneGainLvl, 
                                          swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetSidetoneGainLevel(swi_uint16 sidetoneGainLvl, 
                                          swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetSmsTrafficOnly(swi_uint16* trafficOnly, 
                                          swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetSmsTrafficOnly(swi_uint16 trafficOnly, 
                                          swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetOTKSLCounter(swi_uint16* OTKSLCounter, 
                                        swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetOTKSLCounter(swi_uint16 OTKSLCounter, 
                                        swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetOTKSLMaxFailures(swi_uint16* OTKSLMaxFailures, 
                                            swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetOTKSLMaxFailures(swi_uint16 MaxFailures, 
                                            swi_uint32 timeout);

SWI_API SWI_RCODE SwiPriGetCreateQnc(swi_uint16* createQnc, 
                                     swi_uint32 timeout);

SWI_API SWI_RCODE SwiPriSetCreateQnc(swi_uint16 createQnc, swi_uint32 timeout);

SWI_API SWI_RCODE SwiPriGetDataWarningMask(swi_uint16* pnDataWarningMask, 
                                           swi_uint32 timeout);

SWI_API SWI_RCODE SwiPriSetDataWarningMask(swi_uint16 nDataWarningMask, 
                                           swi_uint32 timeout);

SWI_API SWI_RCODE SwiPriGetSidActivPrompt(swi_uint16* pnSidActivPrompt, 
                                          swi_uint32 timeout);

SWI_API SWI_RCODE SwiPriSetSidActivPrompt(swi_uint16 nSidActivPrompt, 
                                          swi_uint32 timeout);

SWI_API SWI_RCODE SwiPriGetQncActivPrompt(swi_uint16* pnQncActivPrompt, 
                                          swi_uint32 timeout);

SWI_API SWI_RCODE SwiPriSetQncActivPrompt(swi_uint16 nQncActivPrompt, 
                                          swi_uint32 timeout);

SWI_API SWI_RCODE SwiPriGetConnectoidNameQnc(swi_charp pszQncName, 
                                             swi_uint32 szLength, 
                                             swi_uint32 timeout);

SWI_API SWI_RCODE SwiPriGetConnectoidName1x(swi_charp psz1xName, 
                                            swi_uint32 szLength, 
                                            swi_uint32 timeout);

SWI_API SWI_RCODE SwiPriGetActivationMask(swi_uint16* awMask, 
                                          swi_uint32 timeout);

SWI_API SWI_RCODE SwiPriGetMDNLength(swi_uint16* minLen, 
                                     swi_uint16* maxLen, 
                                     swi_uint32 timeout);

SWI_API SWI_RCODE SwiPriGetGUISettings(swi_uint16* guiSettings, 
                                       swi_uint32 timeout);

SWI_API SWI_RCODE SwiPriGetSIPUserIDFormat(swi_charp sipFormat, 
                                           swi_uint32 szLength, 
                                           swi_uint32 timeout);

SWI_API SWI_RCODE SwiPriGetOTASPDialstring(swi_charp otaspString, 
                                           swi_uint32 szLength, 
                                           swi_uint32 timeout);

SWI_API SWI_RCODE SwiPriGetVoicemailDialstring(swi_charp vmailString, 
                                               swi_uint32 szLength, 
                                               swi_uint32 timeout);

SWI_API SWI_RCODE SwiPriGetEnableDmu(swi_uint16* nEnableDmu, 
                                     swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetDsMipNumProf(swi_uint16 profiles, 
                                        swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetDsMipNumProf(swi_uint16* pProfiles, 
                                        swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetDsMipSsUserProf(swi_uint16 index, 
                                           swi_uint16 mnHaSsLength, 
                                           swi_charp mnHaSs, 
                                           swi_uint16 mnAaaSsLength, 
                                           swi_charp mnAaaSs, 
                                           swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetDsMipSsUserProf(swi_uint16 index, 
                                           swi_charp mnHaSs, 
                                           swi_uint16* pmnHaSsLength, 
                                           swi_uint16 mnHaSsBufLength, 
                                           swi_charp mnAaaSs, 
                                           swi_uint16* pmnAaaSsLength, 
                                           swi_uint16 mnAaaSsBufLength, 
                                           swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetDsMipMnHaTimeDelta(swi_uint16 index, 
                                              swi_uint32 mnHaDelta, 
                                              swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetDsMipMnHaTimeDelta(swi_uint16 index, 
                                              swi_uint32* pMnHaDelta, 
                                              swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetDsMipGenUserBak(swi_uint16 index, 
                                           swi_uint16 naiLength, 
                                           swi_charp nai, 
                                           swi_uint16* pMnHaSpiEnable, 
                                           swi_uint32* pMnHaSpi, 
                                           swi_uint16* pMnAaaSpiEnable, 
                                           swi_uint32* pMnAaaSpi, 
                                           swi_uint16* pReverseTunneling, 
                                           swi_uint32* pHomeAddress, 
                                           swi_uint32* pPrimaryHaAddress, 
                                           swi_uint32* pSecondaryHaAddress, 
                                           swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetDsMipSsUserBak(swi_uint16 index, 
                                          swi_uint16 mnHaSsLength, 
                                          swi_charp mnHaSs, 
                                          swi_uint16 mnAaaSsLength, 
                                          swi_charp mnAaaSs, 
                                          swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetQncDialString(swi_charp pszDialString, 
                                         swi_uint32 szLength, 
                                         swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetQncDialString(const swi_charp pszDialString, 
                                         swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetPRLLocked(swi_uint16* pnPRLLocked, 
                                     swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetDMUMnAuth(swi_uint32 *pnMnAuth, swi_uint32 timeout);
SWI_API SWI_RCODE SwiResetDMUMnAuth(swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetDMUPublicKeyIndex(swi_uint16* pPublicKeyIndex, 
                                          swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetDMUPublicKeyIndex(swi_uint16 publicKeyIndex, 
                                          swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetCdmaFeaturesKey(swi_uint32* pnFeatureMask, 
                                        swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetCdmaFeaturesKey(const swi_charp pszFeatKey, 
                                        swi_uint16* pnResult, 
                                        swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetEFileInfo(const swi_charp pszEFilename, 
                                  SWI_STRUCT_EFileInfo* psEFileInfo, 
                                  swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetEFile(const swi_charp pszEFilename, 
                              swi_uint8* pBuffer, 
                              swi_uint16 nBufLen, 
                              swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetEFileInfo(const swi_charp pszEFilename, 
                                  swi_uint32 nSize, 
                                  swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetEFile(const swi_charp pszEFilename, 
                              swi_uint8* pBuffer, 
                              swi_uint16 nBufLen, 
                              swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetPRLSize(swi_uint16 Nam, 
                                swi_uint16 *NumBlocks, 
                                swi_uint32 *Size, 
                                swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetPRLSize(swi_uint16 Nam, 
                                swi_uint32 nSize, 
                                swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetPRLValid(swi_uint16 Nam, 
                                 swi_uint16 *Validity, 
                                 swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetPRL(swi_uint16 NamIndex, 
                            swi_uint8* pBuffer, 
                            swi_uint16 nBufLen, 
                            swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetDormant(swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetTCXOMode(SWI_INTERFACE_MODE nInterfaceMode, 
                                 swi_uint32 timeout);

SWI_API SWI_RCODE SwiEVDONetworkAcquired(swi_int32* lAcquired, 
                                         swi_uint32 timeout);

SWI_API SWI_RCODE SwiEVDOGetUati(swi_uint8* lUati, 
                                 swi_uint32 Size, 
                                 swi_uint32 timeout);

SWI_API SWI_RCODE SwiDataStatsReset(swi_uint16 eProtocol, 
                                    SWI_STRUCT_DataStatsReset *pResetResult, 
                                    swi_uint32 timeout);

SWI_API SWI_RCODE SwiDataStatsFetch(swi_uint16 eProtocol, 
                                    swi_uint16 ePppSubProtocol, 
                                    SWI_STRUCT_DataStatsFetch *pDataStats, 
                                    swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetAutoPopulateInfoAp(
        swi_uint16 nIndex, 
        nv_auto_populate_info_ap_type *pData, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetAutoPopulateInfoAp(
        const nv_auto_populate_info_ap_type *pData, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetEVDOSCPSettings(swi_uint16* pwCustCfgActive, 
                                        swi_uint32 *pdwProtMask, 
                                        swi_uint32 *pdwBCMCSMask, 
                                        swi_uint32 *pdwAppMask, 
                                        swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetEVDOSCPSettings(swi_uint16 wCustCfgActive, 
                                        swi_uint32 dwProtMask, 
                                        swi_uint32 dwBCMCSMask, 
                                        swi_uint32 dwAppMask, 
                                        swi_uint32 timeout);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __SWIINTCDMA_H__ */

/* 
 * $Log: SwiIntCdma.h,v $
 */

