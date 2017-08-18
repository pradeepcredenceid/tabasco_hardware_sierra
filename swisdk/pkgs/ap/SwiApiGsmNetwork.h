/**
 *    \if CvsId 
 *    $Id: SwiApiGsmNetwork.h,v 1.17 2010/06/10 00:18:47 epasheva Exp $
 *    \endif
 *    \ingroup gsmnetwork
 *    \file SwiApiGsmNetwork.h
 * 
 *    \brief This file contains definitions, enumerations, structures and
 *           forward declarations for SwiApiGsmNetwork.c file
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */
#ifndef __SWIAPI_GSM_NETWORK_H
#define __SWIAPI_GSM_NETWORK_H

#ifdef __cplusplus
extern "C" {
#endif

/* include files */
#include "SwiDataTypes.h"
#include "SwiRcodes.h"
#include "SwiApiGsmBasic.h"

/* defines */
#define MAX_CELLS                         6
#define LENGTH_CallNumber                32       /* Max Caller ID length      */
#define LENGTH_CallExtendedNumber        64       /* Max Ext'd Caller ID lngth */
#define LENGTH_SectorId                  16
#define LENGTH_WAPUri                   128
#define LENGTH_HdrUati                   16
#define LENGTH_MaxPPPString              33
#define LENGTH_IPAddress                 16
#define LENGTH_LogFileName              128
#define LENGTH_SdkPathName              128
#define LENGTH_CommPort                   7
#define LENGTH_UsbdVersion               85
#define LENGTH_UsbdBuildDate              9
#define LENGTH_BootBuildDate              9
#define NUMVALUE_WIMAX                80216
#define MAX_NOTIF_LIST                  122
#define LENGTH_DriverName               100
#define LENGTH_MacAddress                 6
#define LENGTH_ProductName               20
#define LENGTH_ModemName                 75
#define LENGTH_HardwareId                50
#define MAX_NUM_LBS_SV                   12
#define LENGTH_TimeStamp                 16
#define MAX_BAND_GROUPS                  50
#define MAX_BAND_LIST                    50
#define MAX_AIR_SERVERS                  50  /* Must be < than a frame. */

#define LENGTH_SPN                  (16 * UTF8_MAX_CHAR_SIZE) + 1
#define LENGTH_PLMN                 (31 * UTF8_MAX_CHAR_SIZE) + 1
#define LENGTH_AbvPLMN              (13 * UTF8_MAX_CHAR_SIZE) + 1
#define LENGTH_MEPCode              13
#define LENGTH_USSDInputExp         230
#define LENGTH_Country              (3 * UTF8_MAX_CHAR_SIZE) + 1
#define LENGTH_Network              (8 * UTF8_MAX_CHAR_SIZE) + 1
#define LENGTH_MCCMNC_BUFFER        3
#define LENGTH_MCCMNC_TEXT          6
#define LENGTH_UnlockKey            11
/* Note: The following definitions reflect the spec 
 *       These constants are used for pointer advancement 
 *       in unpack functions
 */

/* object 0x700A */
#define OBJ_LENGTH_Country          6
#define OBJ_LENGTH_Network          16

/* object 0x1042 & 0x1043 */
#define OBJ_LENGTH_PLMN             62
#define OBJ_LENGTH_AbvPLMN          26
/* object 0x1043 */
#define OBJ_LENGTH_SPN              32

/* GsmProfiles */
#define LENGTH_RsvdGsmProfileBasic      11
#define LENGTH_CNS_PROFILE_READ_PARAM   3
#define LENGTH_CNS_PROFILE_WRITE_MAX    245
#define LENGTH_CNS_PLMN_MODE_SET_PARAM  5
#define LENGTH_CNS_PROFILE_AUTH_GET     3
/* object 0x700E */
#define OBJ_PROFILE_AUTH_NUMOBJ        2 /* can only set username & password */
#define OBJ_TYPE_UserName           0x00
#define OBJ_TYPE_Password           0x01
#define OBJ_LENGTH_UserName         128
#define OBJ_LENGTH_Password         128
#define OBJ_LENGTH_GetRat           2
#define OBJ_LENGTH_SetRat           3
#define OBJ_LENGTH_GetPrlRegion     2
#define OBJ_LENGTH_SetPrlRegion     3

/* PLMN status bit mask */
#define PLMN_REGISTERED 0x01
#define PLMN_FORBIDDEN  0x02
#define PLMN_HOME       0x04
#define PLMN_WEAK       0x08
#define PLMN_GPRS_BIT   0x10

/**
 *
 * Defines the roaming states returned by the modem.
 *
 * \sa SWI_STRUCT_GSM_NetworkStatus 
 * \sa SwiGetRoamingState 
 *
 */

enum SWI_TYPE_RoamingState
{
    SWI_ROAMING_Home,     /*!< not roaming */
    SWI_ROAMING_Roaming,  /*!< roaming */
    SWI_ROAMING_NonPref,  /*!< no roaming information */
    SWI_ROAMING_None      /*!< unknown roaming state */
};

/**
 *
 * An enumeration of GSM profile information types.
 *
 * \sa SWI_STRUCT_GsmProfileChange
 *
 */
enum SWI_TYPE_ProfileInfo
{
    SWI_PROFILEINFO_Basic,  /*! Basic */
    SWI_PROFILEINFO_Qos,    /*! Quality of service */
    SWI_PROFILEINFO_Dns,    /*! Domain name service */
    SWI_PROFILEINFO_Tft     /*! Traffic flow template */
};

/**
 *
 * An enumeration of quality of service precedence types.
 *
 * \sa SWI_STRUCT_PktSessionStatus
 * \sa SWI_STRUCT_Profile
 * \sa SWI_STRUCT_GsmProfileQos
 * \sa SWI_STRUCT_PktSessionCall
 *
 */
enum SWI_TYPE_Precedence
{
    SWI_PRECED_Subscribed,      /*!< Subscribed precedence */    
    SWI_PRECED_HighPriority,    /*!< High priority */
    SWI_PRECED_MediumPriority,  /*!< Medium priority */
    SWI_PRECED_LowPriority      /*!< Low priority */
};

/**
 *
 * An enumeration of result codes for a network selection.
 *
 * \sa SWI_STRUCT_PlmnMode
 *
 */
enum SWI_TYPE_PLMN_Result
{
    SWI_PLMNRESULT_Success,              /*!< Succeeded in selecting the
                                             requested PLMN */
    SWI_PLMNRESULT_AttemptFailed,        /*!< Attempt to select requested PLMN
                                             failed */
    SWI_PLMNRESULT_InProgress,           /*!< PLMN selection is underway */
    SWI_PLMNRESULT_GeneralFail,          /*!< General failure */
    SWI_PLMNRESULT_InternalErr,          /*!< Internal failure */
    SWI_PLMNRESULT_InvalidSelection      /*!< Invalid PLMN selection */
};

/**
 *
 * An enumeration of modem power status.
 *
 * \sa SWI_STRUCT_GsmSystemStatus
 *
 */
enum SWI_TYPE_ModemStatus
{
    SWI_MODEMSTATUS_Offline,    /*!< Offline */
    SWI_MODEMSTATUS_Online,     /*!< Online */
    SWI_MODEMSTATUS_Lpm         /*!< Lower power mode */
};

/**
 *
 * An enumeration of service errors types.
 *
 * \sa SWI_STRUCT_GsmSystemStatus
 *
 */
enum SWI_TYPE_ServiceErr
{
    SWI_SERVICEERR_None,                    /*!< No error */
    SWI_SERVICEERR_Unspecified = 0xFFFF     /*!< Unspecified protocol error */
};

/**
 *
 * An enumeration of services types.
 *
 * \sa SWI_STRUCT_ServiceIcon
 * \sa SWI_STRUCT_GsmSystemStatus
 *
 */
enum SWI_TYPE_Service
{
    SWI_SERVICE_None,        /*!< No service. */
    SWI_SERVICE_Gprs,        /*!< GPRS */
    SWI_SERVICE_Edge,        /*!< EDGE */
    SWI_SERVICE_Umts,        /*!< UMTS */
    SWI_SERVICE_Hsdpa,       /*!< HSDPA */
    SWI_SERVICE_Hsupa,       /*!< HSUPA */
    SWI_SERVICE_Hspa,        /*!< HSDPA + HSUPA */
    SWI_SERVICE_HspaPlus,    /*!< HSPA+ */
    SWI_SERVICE_DcHspaPlus,  /*!< DC HSPA+ */
    SWI_SERVICE_Lte,         /*!< LTE */
    
};

/**
 *
 * An enumeration of MNC formats
 *
 * \sa SWI_STRUCT_GsmSystemStatus
 *
 */
enum SWI_TYPE_MncFormat
{
    SWI_MNCFORMAT_2Digit,   /*!< 2 digit format */
    SWI_MNCFORMAT_3Digit    /*!< 3 digit format */
};

/**
 *
 * An enumeration of quality of service delay types.
 *
 * \sa SWI_STRUCT_PktSessionStatus
 * \sa SWI_STRUCT_Profile
 * \sa SWI_STRUCT_GsmProfileQos
 * \sa SWI_STRUCT_PktSessionCall
 *
 */
enum SWI_TYPE_Delay
{
    SWI_DELAY_Subscribed,    /*!< Subscribed delay */        
    SWI_DELAY_Low,           /*!< Low delay */
    SWI_DELAY_Medium,        /*!< Medium delay */
    SWI_DELAY_High,          /*!< High delay */
    SWI_DELAY_BestEffort     /*!< Best effort delay */    
};

/**
 *
 * An enumeration of quality of service reliability types.
 *
 * \sa SWI_STRUCT_PktSessionStatus
 * \sa SWI_STRUCT_Profile
 * \sa SWI_STRUCT_GsmProfileQos
 * \sa SWI_STRUCT_PktSessionCall
 *
 */
enum SWI_TYPE_Reliability
{
    SWI_RELI_Subscribed,                /*!< Default reliability class */ 
    SWI_RELI_GTPAcknowledged,           /*!< GTP reliability class */
    SWI_RELI_LLCFrameAcknowledged,      /*!< LCC frame reliability class */
    SWI_RELI_RLCBlockModeAcknowledged,  /*!< RLC acknowledged reliability
                                             class */
    SWI_RELI_LLCDataProtected,          /*!< LCC data protected reliability
                                             class */
    SWI_RELI_UnackUnprotected           /*!< Unacknowledged reliability 
                                             class */
};

/**
 *
 * An enumeration of quality of service peak throughput types.
 *
 * \sa SWI_STRUCT_PktSessionStatus
 * \sa SWI_STRUCT_Profile
 * \sa SWI_STRUCT_GsmProfileQos
 * \sa SWI_STRUCT_PktSessionCall
 *
 */
enum SWI_TYPE_PeakThroughput
{
    SWI_THROUGHPUT_Subscribed,  /*!< Default to peak throughput class */
    SWI_THROUGHPUT_1000,        /*!< Up to 1000 bytes (8 kbit/s) */
    SWI_THROUGHPUT_2000,        /*!< up to 2000 bytes (16 kbit/s) */
    SWI_THROUGHPUT_4000,        /*!< Up to 4000 bytes (32 kbit/s) */
    SWI_THROUGHPUT_8000,        /*!< Up to 8000 bytes (64 kbit/s) */
    SWI_THROUGHPUT_16000,       /*!< Up to 16000 bytes (128 kbit/s) */
    SWI_THROUGHPUT_32000,       /*!< Up to 32000 bytes (256 kbit/s) */
    SWI_THROUGHPUT_64000,       /*!< up to 64000 bytes (512 kbit/s) */
    SWI_THROUGHPUT_128000,      /*!< Up to 128000 bytes (1024 kbit/s) */
    SWI_THROUGHPUT_256000       /*!< Up to 256000 bytes (2048 kbit/s) */
};

/**
 *
 * An enumeration of quality of service mean throughput types.
 *
 * \sa SWI_STRUCT_PktSessionStatus
 * \sa SWI_STRUCT_Profile
 * \sa SWI_STRUCT_GsmProfileQos
 * \sa SWI_STRUCT_PktSessionCall
 *
 */
enum SWI_TYPE_MeanThroughput
{
    SWI_MEANTHROUGH_Subscribed,     /*!< Subscribed */
    SWI_MEANTHROUGH_100,            /*!< 100 octets/hr (~0.22 bit/s) */
    SWI_MEANTHROUGH_200,            /*!< 200 octets/hr (~0.44 bit/s) */
    SWI_MEANTHROUGH_500,            /*!< 500 octets/hr (~1.11 bit/s) */
    SWI_MEANTHROUGH_1K,             /*!< 1000 octets/hr (~2.2 bit/s) */
    SWI_MEANTHROUGH_2K,             /*!< 2000 octets/hr (~4.4 bit/s) */
    SWI_MEANTHROUGH_5K,             /*!< 5000 octets/hr (~11.1 bit/s) */
    SWI_MEANTHROUGH_10K,            /*!< 10000 octets/hr (~22 bit/s) */
    SWI_MEANTHROUGH_20K,            /*!< 20000 octets/hr (~44 bit/s) */
    SWI_MEANTHROUGH_50K,            /*!< 50000 octets/hr (~111 bit/s) */
    SWI_MEANTHROUGH_100K,           /*!< 100000 octets/hr (~0.22 kbit/s) */
    SWI_MEANTHROUGH_200K,           /*!< 200000 octets/hr (~0.44 kbit/s) */
    SWI_MEANTHROUGH_500K,           /*!< 500000 octets/hr (~1.11 kbit/s) */
    SWI_MEANTHROUGH_1M,             /*!< 1000000 octets/hr (~2.2 kbit/s) */
    SWI_MEANTHROUGH_2M,             /*!< 2000000 octets/hr (~4.4 kbit/s) */
    SWI_MEANTHROUGH_5M,             /*!< 5000000 octets/hr (~11.1 kbit/s) */
    SWI_MEANTHROUGH_10M,            /*!< 10000000 octets/hr (~22 kbit/s) */
    SWI_MEANTHROUGH_20M,            /*!< 20000000 octets/hr (~44 kbit/s) */
    SWI_MEANTHROUGH_50M,            /*!< 50000000 octets/hr (~111 kbit/s) */
    SWI_MEANTHROUGH_BestEffort      /*!< Best effort */
};

/**
 *
 * An enumeration of packet session inactivity reason types.
 *
 * \sa SWI_STRUCT_PktSessionStatus
 * \sa SWI_STRUCT_PktSessionCall
 *
 */
enum SWI_TYPE_InactivityReason
{
    /*! Operator determined barring */
    SWI_INACTIVITYREASON_OperatorBarring,
    
    /*! LCC */
    SWI_INACTIVITYREASON_LLC,
    
    /*! Insufficient resources */
    SWI_INACTIVITYREASON_InsufficientResources,
    
    /*! APN unknown or missing */
    SWI_INACTIVITYREASON_MissingOrUnknownAPN,
    
    /*! PDP address unknown or missing */
    SWI_INACTIVITYREASON_UnknownPDPAddressOrType,
    
    /*! User authentication failed */
    SWI_INACTIVITYREASON_UserAuthenticationFailed,
    
    /*! GGSN rejection */
    SWI_INACTIVITYREASON_ActivationRejectedByGGSN,
    
    /*! Service not supported */
    SWI_INACTIVITYREASON_ServiceOptionNotSupported,
    
    /*! Not subscribed for that service */
    SWI_INACTIVITYREASON_ServiceOptionNotSubscribed,
    
    /*! Service out of order */
    SWI_INACTIVITYREASON_ServiceOptionTemporarilyOutOfOrder,    
    
    /*! Regular deactivation */
    SWI_INACTIVITYREASON_RegularDeactivation,
    
    /*! QoS not accepted */
    SWI_INACTIVITYREASON_QosNotAccepted,
    
    /*! Network failure */
    SWI_INACTIVITYREASON_NetworkFailure,
    
    /*! Reactivation required */
    SWI_INACTIVITYREASON_ReactivationRequired,
    
    /*! Feature not supported */
    SWI_INACTIVITYREASON_FeatureNotSup,
    
    /*! Semantic error in the TFT operation */
    SWI_INACTIVITYREASON_SemErrTftOper,
    
    /*! Syntactical error in the TFT operation */
    SWI_INACTIVITYREASON_SynErrTftOper,
    
    /*! Unknown PDP context */
    SWI_INACTIVITYREASON_UnknownPdpContext,
    
    /*! PDP context without TFT already activated */
    SWI_INACTIVITYREASON_TftActive,
    
    /*! Semantic error in packet filter(s) */
    SWI_INACTIVITYREASON_SemErrPktFilter,
    
    /*! Syntactical error in the packet filter(s) */
    SWI_INACTIVITYREASON_SynErrPktFilter,
    
    /*! Invalid transaction identifier value */
    SWI_INACTIVITYREASON_InvalidTransId,
    
    /*! Semantically incorrect message */
    SWI_INACTIVITYREASON_SemIncMsg,
    
    /*! Invalid mandatory information */
    SWI_INACTIVITYREASON_InvalidInfo,
    
    /*! Message type non-existent or not implemented */
    SWI_INACTIVITYREASON_MsgNonExist,
    
    /*! Message type not compatible with the protocol state */
    SWI_INACTIVITYREASON_MsgTypeNotComp,
    
    /*! Information element non-existent or not implemented */
    SWI_INACTIVITYREASON_InfoNonExist,
    
    /*! Conditional IE error */
    SWI_INACTIVITYREASON_IeErr,
    
    /*! Message not compatible with the protocol state */
    SWI_INACTIVITYREASON_MsgNotComp,
    
    /*! Protocol error, unspecified */                    
    SWI_INACTIVITYREASON_ProtErr,
    
    /*! Normal */                    
    SWI_INACTIVITYREASON_Normal
};

/**
 *
 * An enumeration of profile states
 *
 * \sa SWI_STRUCT_ProfileStatus
 * \sa SWI_STRUCT_GsmProfileSummary
 *
 */
enum  SWI_TYPE_ProfileState
{
    /*! Indicates the account profile has a valid account definition but is not 
        currently being used in a session. */
    SWI_PROFILE_STATE_Inactive,
    
    /*! The account has a valid definition and is in use with the current
        session. */
    SWI_PROFILE_STATE_Activated,
    
    /*! The profile is empty-without a valid account definition. In fact, the 
        profile contains default values but cannot be made active. This profile
        can be used to store a new account definition. */
    SWI_PROFILE_STATE_Deleted            
};

/**
 *
 * An enumeration of available configurable network selection modes
 *
 * \sa SwiGetPLMNMode
 * \sa SWI_STRUCT_PlmnSelection
 *
 */
enum SWI_TYPE_PLMN_Mode
{
    SWI_PLMNMODE_Automatic,     /*!< Automatic PLMN selection */
    SWI_PLMNMODE_Manual,        /*!< Manual PLMN selection (3G preferred) */
    SWI_PLMNMODE_Manual_2G      /*!< Manual PLMN selection (2G preferred) */
};

/**
 *
 * An enumeration of network service status types.
 *
 * \sa SWI_STRUCT_GSM_NetworkStatus
 * \sa SWI_STRUCT_GsmSystemStatus
 * \sa SWI_STRUCT_GsmCellMeasure
 * \sa SwiGetServiceStatusEx
 *
 */
enum SWI_TYPE_ServiceStatus
{
    /*! The modem has acquired normal GPRS service and can use all available 
        features. */
    SWI_SERVICE_STATUS_Normal,
    
    /*! Full service is not available but there is coverage permitting 911 
        emergency voice calls.  The reasons full service is not available vary
        and can include: security lock, account activation, and others. */
    SWI_SERVICE_STATUS_Emergency,
    
    /*! The modem has not acquired GSM GPRS service, or the modem is security
        locked preventing service. */
    SWI_SERVICE_STATUS_NoService,
    
    /*! The network reports this fault. The modem is unable to register. The 
        user should check their signal strength or contact their carrier. */
    SWI_SERVICE_STATUS_AccessDifficulty,
    
    /*! The network indicates that you do not have permission to use it. */
    SWI_SERVICE_STATUS_ForbiddenPLMN,
    
    /*! The modem is in a geographic area where use of the service is not
        permitted. */
    SWI_SERVICE_STATUS_ForbiddenArea,
    
    /*! The modem is not in its home coverage area and is not permitted to roam
        in the current coverage area. */
    SWI_SERVICE_STATUS_ForbiddenRoaming,
    
    /*! The user's account is not permitted on the network. */
    SWI_SERVICE_STATUS_IllegalMobStation,
    
    /*! The modem is not permitted on the network, possibly for regulatory 
        reasons. */
    SWI_SERVICE_STATUS_IllegalMobEquip,
    
    /*! The SIM Card's identifier is unrecognized (or disqualified) by 
        carrier. */
    SWI_SERVICE_STATUS_UnknownIMSI,
    
    /*! The user name and/or password associated with the account profile 
        failed to authenticate. */
    SWI_SERVICE_STATUS_AuthenticationFailure,    
    
    /*! GPRS service is not available. */
    SWI_SERVICE_STATUS_GPRSFailed
};

/**
 *
 * An enumeration of UMTS source statistics descriptor values.
 *
 * \sa SWI_STRUCT_GsmProfileQos
 * \sa SWI_STRUCT_PktSessionCall
 *
 */
enum SWI_TYPE_QOSUMTS_SrcStatDesc
{
    SWI_QOSUMTS_SRCSTATDESC_Unknown,    /*!< Unknown */
    SWI_QOSUMTS_SRCSTATDESC_Speech      /*!< Speech */
};

/**
 *
 * An enumeration of UMTS signaling indication values.
 *
 * \sa SWI_STRUCT_GsmProfileQos
 * \sa SWI_STRUCT_PktSessionCall
 *
 */
enum SWI_TYPE_QOSUMTS_SignInd
{
    SWI_QOSUMTS_SIGNIND_NotOpt,    /*!< Not optimized for signaling traffic. */
    SWI_QOSUMTS_SIGNIND_Opt        /*!< Optimized for signaling traffic. */
};

/**
 *
 * An enumeration of PDP initiated types.
 *
 * \sa SWI_STRUCT_GsmProfileBasic
 *
 */
enum  SWI_TYPE_PdpInitiated
{
    SWI_PDP_INITIATED_PrimaryMobile,    /*!< Primary context */
    SWI_PDP_INITIATED_SecondaryMobile,  /*!< Secondary context */
    SWI_PDP_INITIATED_Network,          /*!< Currently not supported */
    SWI_PDP_INITIATED_None              /*!< Currently not supported */
};

/**
 *
 * An enumeration of authentication types.
 *
 * \sa SWI_STRUCT_GsmProfileBasic
 *
 */
enum  SWI_TYPE_Authentication
{
    SWI_PDPAUTHENTICATION_None,       /*!< No authentication */
    SWI_PDPAUTHENTICATION_Pap,        /*!< PAP authentication (default) */
    SWI_PDPAUTHENTICATION_Chap        /*!< CHAP authentication */
};

/**
 *
 * An enumeration of UMTS bit rate values.
 *
 * \sa SWI_STRUCT_GsmProfileQos
 * \sa SWI_STRUCT_PktSessionCall
 *
 */
enum SWI_TYPE_QOSUMTS_Bitrate
{
    SWI_QOSUMTS_BITRATE_Subscribed,     /*!< subscribed */
    SWI_QOSUMTS_BITRATE_16Kbps,         /*!< 16 Kbps */
    SWI_QOSUMTS_BITRATE_32Kbps,         /*!< 32 Kbps */
    SWI_QOSUMTS_BITRATE_64Kbps,         /*!< 64 Kbps */
    SWI_QOSUMTS_BITRATE_128Kbps,        /*!< 128 Kbps */
    SWI_QOSUMTS_BITRATE_256Kbps,        /*!< 256 Kbps */
    SWI_QOSUMTS_BITRATE_384Kbps,        /*!< 384 Kbps */
    SWI_QOSUMTS_BITRATE_1_8Mbps,        /*!< 1.8 Mbps */
    SWI_QOSUMTS_BITRATE_3_6Mbps,        /*!< 3.6 Mbps */
    SWI_QOSUMTS_BITRATE_7_2Mbps,        /*!< 7.2 Mbps */
    SWI_QOSUMTS_BITRATE_14_4Mbps        /*!< 14.4 Mbps */
};

/**
 *
 * An enumeration of UMTS quality of service traffic class types.
 *
 * \sa SWI_STRUCT_GsmProfileQos
 * \sa SWI_STRUCT_PktSessionCall
 *
 */
enum SWI_TYPE_QOSUMTS_TrafficClass
{
    SWI_QOSUMTS_TRAFFCLS_Subscribed,    /*!< Subscribed */
    SWI_QOSUMTS_TRAFFCLS_Conver,        /*!< Conversational */
    SWI_QOSUMTS_TRAFFCLS_Stream,        /*!< Streaming */
    SWI_QOSUMTS_TRAFFCLS_Interact,      /*!< Interactive */
    SWI_QOSUMTS_TRAFFCLS_Backgnd        /*!< Background */
};

/**
 *
 * An enumeration of UMTS delivery order values.
 *
 * \sa SWI_STRUCT_GsmProfileQos
 * \sa SWI_STRUCT_PktSessionCall
 *
 */
enum SWI_TYPE_QOSUMTS_DelOrder
{
    SWI_QOSUMTS_DELORDER_Subscribed,    /*!< Subscribed */
    SWI_QOSUMTS_DELORDER_With,          /*!< With delivery order */
    SWI_QOSUMTS_DELORDER_Without        /*!< Without delivery order */
};

/**
 *
 * An enumeration of UMTS delivery of erroneous SDU values.
 *
 * \sa SWI_STRUCT_GsmProfileQos
 * \sa SWI_STRUCT_PktSessionCall
 *
 */
enum SWI_TYPE_QOSUMTS_DelErrSdu
{
    SWI_QOSUMTS_DELERRSDU_Subscribed,   /*!< Subscribed */
    SWI_QOSUMTS_DELERRSDU_NoDetect,     /*!< No detect */
    SWI_QOSUMTS_DELERRSDU_Del,          /*!< Erroneous SDUs are delivered */
    SWI_QOSUMTS_DELERRSDU_NotDel        /*!< Erroneous SDUs are not delivered */
};

/**
 *
 * An enumeration of UMTS SDU error ratio values.
 *
 * \sa SWI_STRUCT_GsmProfileQos
 * \sa SWI_STRUCT_PktSessionCall
 *
 */
enum SWI_TYPE_QOSUMTS_SduErrRatio
{
    SWI_QOSUMTS_SDUERRRATIO_Subscribed, /*!< subscribed */
    SWI_QOSUMTS_SDUERRRATIO_1E2,        /*!< 1E-2 */
    SWI_QOSUMTS_SDUERRRATIO_7E3,        /*!< 7E-3 */
    SWI_QOSUMTS_SDUERRRATIO_1E3,        /*!< 1E-3 */
    SWI_QOSUMTS_SDUERRRATIO_1E4,        /*!< 1E-4 */
    SWI_QOSUMTS_SDUERRRATIO_1E5,        /*!< 1E-5 */
    SWI_QOSUMTS_SDUERRRATIO_1E6,        /*!< 1E-6 */
    SWI_QOSUMTS_SDUERRRATIO_1E1         /*!< 1E-1 */
};

/**
 *
 * An enumeration of UMTS residual bit error ratio values.
 *
 * \sa SWI_STRUCT_GsmProfileQos
 * \sa SWI_STRUCT_PktSessionCall
 *
 */
enum SWI_TYPE_QOSUMTS_ResBitErrRatio
{
    SWI_QOSUMTS_RESBITERRRATIO_Subscribed,    /*!< subscribed */
    SWI_QOSUMTS_RESBITERRRATIO_5E2,           /*!< 5E-2 */
    SWI_QOSUMTS_RESBITERRRATIO_1E2,           /*!< 1E-2 */
    SWI_QOSUMTS_RESBITERRRATIO_5E3,           /*!< 5E-3 */
    SWI_QOSUMTS_RESBITERRRATIO_4E3,           /*!< 4E-3 */
    SWI_QOSUMTS_RESBITERRRATIO_1E3,           /*!< 1E-3 */
    SWI_QOSUMTS_RESBITERRRATIO_1E4,           /*!< 1E-4 */
    SWI_QOSUMTS_RESBITERRRATIO_1E5,           /*!< 1E-5 */
    SWI_QOSUMTS_RESBITERRRATIO_1E6,           /*!< 1E-6 */
    SWI_QOSUMTS_RESBITERRRATIO_6E8            /*!< 6E-8 */
};

/**
 *
 * An enumeration of UMTS traffic handling priority values.
 *
 * \sa SWI_STRUCT_GsmProfileQos
 * \sa SWI_STRUCT_PktSessionCall
 *
 */
enum SWI_TYPE_QOSUMTS_TraffHandPri
{
    SWI_QOSUMTS_TRAFFHANDPRI_Subscribed,    /*!< Subscribed */
    SWI_QOSUMTS_TRAFFHANDPRI_PriLvl1,       /*!< Priority level 1 */
    SWI_QOSUMTS_TRAFFHANDPRI_PriLvl2,       /*!< Priority level 2 */
    SWI_QOSUMTS_TRAFFHANDPRI_PriLvl3        /*!< Priority level 3 */
};

/**
 *
 * An enumeration of currently selected network selection modes.
 *
 * \sa SWI_STRUCT_GSM_NetworkStatus
 * \sa SWI_STRUCT_GsmSystemStatus
 *
 */
enum SWI_TYPE_ModeSelection
{
    SWI_MODE_SELECTION_Automatic,    /*!< Automatic network selection */
    SWI_MODE_SELECTION_Manual        /*!< Manual network selection */
};

/**
 *
 * An enumeration of network types.
 *
 * \sa SWI_STRUCT_PlmnSelection
 *
 */
enum SWI_TYPE_Network
{
    SWI_NETWORK_Unknown,    /*!< No network */
    SWI_NETWORK_GSM,        /*!< GSM network */
    SWI_NETWORK_DCS,        /*!< DCS network */
    SWI_NETWORK_GSMDCS,     /*!< GSM and DCS network */
    SWI_NETWORK_PCS,        /*!< PCS network */
    SWI_NETWORK_GSM_PCS,    /*!< GSM and PCS network */
    SWI_NETWORK_IO,         /*!< IO network */
    SWI_NETWORK_UMTS        /*!< UMTS */
};

/**
 *
 * An enumeration of service types.
 *
 * For modems capable of both circuit switched(GSM) as well as packet (GPRS) 
 * data operations, this enumerated type identifies the type of service 
 * currently available.
 *
 * \sa SWI_STRUCT_GSM_NetworkStatus
 * \sa SWI_STRUCT_GsmSystemStatus
 *
 */
enum SWI_TYPE_ServiceType
{
    SWI_SERVICE_TYPE_CircuitSwitched, /*!< Circuit switched */
    SWI_SERVICE_TYPE_GPRS,            /*!< Packet switched */
    SWI_SERVICE_TYPE_Combined,        /*!< Combined circuit and packet switched */
    SWI_SERVICE_TYPE_InvalidService   /*!< Invalid service */        
};

/**
 *
 * This structure contains total number of bytes transmitted and received 
 * since the counter was last reset.  
 * 
 * This structure is used by SwiGetTrafficStatistics.  
 * SwiResetTrafficStatistics can be called to reset byte counters.
 * 
 * Byte counter info is persistent across connections.
 *
 * \sa SWI_NOTIFY_ByteCounter
 * \sa SwiGetTrafficStatistics
 *
 */
struct SWI_STRUCT_ByteCounter
{
    /*!    The size, in bytes, of this structure.  This value must always be
        populated by the calling application. */
    swi_uint32    sizeStruct;  
    
    /*! Bytes transmitted */                                
    swi_uint32    TransmitBytes;     
    
    /*! Bytes received */                    
    swi_uint32    ReceiveBytes;                    
};


/**
 *
 * This structure contains the radio signal strength indicator and bit
 *    error rate information. 
 *
 * Supports PC cards, embedded devices, and MP line of products. 
 *
 *    \sa    SWI_NOTIFY_Rssi
 *
 */
struct SWI_STRUCT_Rssi
{
    /*! The size, in bytes, of this structure. */
    swi_uint32 sizeStruct;  
        
    /*!    Radio signal strength signal in dBm. */
    swi_int32 iRssi;    

    /*!    Bit error rate (currently only used by GSM products). */            
    swi_int32 iBer;            
};

/**
 *
 * This structure contains cell measurements.
 *
 * \sa SWI_TYPE_ServiceStatus
 * \sa SWI_STRUCT_GsmCellInfo
 *
 */
struct SWI_STRUCT_GsmCellMeasure
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct;

    /*! Service status */
    enum SWI_TYPE_ServiceStatus eServStatus;
    
    /*! Radio band.  Values can be mapped to SWI_TYPE_RadioBand which is 
        defined in SwiApiGsmBasic.h. */
    swi_uint32 eRadioBand;
    
    /*! ARFCN */                        
    swi_uint32 nArfcn;                            
    
    /*! Rx level in dBm */    
    swi_uint32 nRxLevel;
    
    /*! C1 */                    
    swi_uint32 nC1;
    
    /*! C2 */                        
    swi_uint32 nC2;
    
    /*! C31 */                            
    swi_uint32 nC31;
    
    /*! C32 */                            
    swi_uint32 nC32;                            
};

/**
 *
 * This structure contains cell information.
 *
 * \sa SWI_STRUCT_GsmCellMeasure
 * \sa SWI_NOTIFY_GsmCellInfo
 * \sa SwiGetGsmCellInfo
 *
 */
struct SWI_STRUCT_GsmCellInfo
{
    /*! The size, in bytes, of this structure. */
    swi_uint32    sizeStruct;
    
    /*! True if information provided in this structure is valid, false 
        otherwise. */
    swi_bool bValid;
    
    /*! MCC */
    swi_uint32    nMcc;
    
    /*! MNC */
    swi_uint32    nMnc;
    
    /*! LAC */
    swi_uint32  nLac;
    
    /*! Cell ID */
    swi_uint32    nCellId;
    
    /*! BSIC */
    swi_uint32    nBsic;
    
    /*! Network color code */
    swi_uint32    nNetColorCode;
    
    /*! Base station color code */
    swi_uint32     nBsColorCode;
    
    /*! RAC */
    swi_uint32    nRac;
    
    /*! Minimum RX level to register in dBm */
    swi_uint32 nMinRxLevel;
    
    /*! Max power on RACH burst (not implemented 0xFFFF). */
    swi_uint32 nMaxRach;

    /*! Current cell measurements */    
    struct SWI_STRUCT_GsmCellMeasure  sMeasurements;                    

    /*! Number of valid neighbor cells. */
    swi_uint32    nNumValidNeighborCells;
    
    /*! Neighbor cell measurements. */
    struct SWI_STRUCT_GsmCellMeasure arNeighborCells[MAX_CELLS];
};


/**
 *
 *
 *    An enumeration of Radio Access Technology (RAT) mode.
 *
 *     \sa SWI_STRUCT_Rat
 *
 */
enum SWI_TYPE_RatMode
{
    SWI_RATMODE_Automatic,          /*!< Automatic */
    SWI_RATMODE_UMTS3G_Only,        /*!< UMTS 3G Only */
    SWI_RATMODE_GSM2G_Only,         /*!< GSM 2G Only */
    SWI_RATMODE_UMTS3G_Preferred,   /*!< UMTS 3G Preferred */
    SWI_RATMODE_GSM2G_Preferred     /*!< GSM 2G Preferred */
};

/**
 *
 *    This structure contains Radio Access Technology (RAT) information.
 *
 *     \sa SWI_TYPE_RatMode
 *     \sa SwiGetRat
 *     \sa SwiSetRat
 */
struct SWI_STRUCT_Rat
{
    /*! The size, in bytes, of this structure. */
    swi_uint32     sizeStruct;

    /*! The returned or requested RAT mode */
    enum SWI_TYPE_RatMode eRatMode;

    /*! True is returned if request to set RAT mode is successful,
    false otherwise. */
    swi_bool  bSuccess;
};

/**
 *
 *     An enumeration of PRL regions.
 *
 *     \sa SWI_STRUCT_PrlRegion
 *
 *
 */
enum SWI_TYPE_PrlRegion
{
    SWI_RRLREGION_Default,          /*!< default (internal) */
    SWI_RRLREGION_Europe,           /*!< Europe */
    SWI_RRLREGION_NorthAmerica,     /*!< North America */
    SWI_RRLREGION_Australia,        /*!< Australia */
    SWI_RRLREGION_Japan,            /*!< Japan */
    SWI_RRLREGION_Reserved,         /*!< Reserved */
    SWI_RRLREGION_Italy             /*!< Italy */
};

/**
 *
 * An enumeration of PRL region mode change status returned by \sa SwiSetPrl.
 *
 * \sa SWI_STRUCT_PrlRegion
 * \sa SwiSetPrl
 *
 */
enum SWI_TYPE_PrlChangeStatus
{
    SWI_PRLCHANGE_Success,          /*!< request successful */
    SWI_PRLCHANGE_InvalidPassword,  /*!< invalid password key */
    SWI_PRLCHANGE_GeneralFailure    /*!< general failure */
};

/**
 *
 * This structure contains PRL Region information.
 *
 * \sa SWI_TYPE_PrlRegion
 * \sa SwiGetPrl
 * \sa SwiSetPrl
 *
 */
struct SWI_STRUCT_PrlRegion
{
    /*! The size, in bytes, of this structure. */
    swi_uint32     sizeStruct;

    /*! The returned or requested PRL region */
    enum SWI_TYPE_PrlRegion ePrlRegion;

    /*! returned change status, it is only used as a return to SwiSetPrl and
        is the actual result of the SwiSetPrl operation.*/
    enum SWI_TYPE_PrlChangeStatus        ePrlChangeStatus;

    /*! A NULL terminated string representing the password to unlock API */
    swi_char szUnlockKey[LENGTH_UnlockKey];
};

/**
 *
 * This structure contains information regarding a specific PLMN.
 * 
 * This structure is used as output by the SwiGetPLMNSelection function and
 * SWI_NOTIFY_PlmnMode notification as well as input by the SwiSetPLMNMode 
 * function.   
 * 
 * \sa SWI_TYPE_PLMN_Mode
 * \sa SWI_TYPE_Network
 * \sa SwiGetPLMNSelection
 * \sa SwiSetPLMNMode
 * \sa SWI_NOTIFY_PlmnMode
 *
 */
struct SWI_STRUCT_PlmnSelection
{
    /*! The size, in bytes, of this structure. This value must be supplied
        when calling SwiSetPLMNMode or SwiGetPLMNSelection. */
    swi_uint32 sizeStruct;  
    
    /*! Network selection mode to use. This field is only applicable when used
        with SwiSetPLMNMode. */
    enum SWI_TYPE_PLMN_Mode eMode;
    
    /*! A NULL terminated string representing the PLMN name. This field is only
        applicable when used with SwiGetPLMNSelection. */        
    swi_char szName[LENGTH_PLMN];
    
    /*! A NULL terminated string representing the abbreviated PLMN name. This 
        field is only applicable when used with SwiGetPLMNSelection. */
    swi_char szAbvPLMN[LENGTH_AbvPLMN];
    
    /*! Mobile Country Code.  This value must be supplied when used with 
        SwiSetPLMNMode to configure manual mode. */
    swi_uint32 nMCC;
    
    /*! Mobile Network Code.  This value must be supplied when used with 
        SwiSetPLMNMode to configure manual mode. */
    swi_uint32 nMNC;
    
    /*! Network type of this PLMN.  This field is only applicable when used 
        with SwiGetPLMNSelection. */
    enum SWI_TYPE_Network eNetwork;
    
    /*! True is returned if the modem is registered with this PLMN, otherwise 
        false is returned.  This field is only applicable when used with 
        SwiGetPLMNSelection. */
    swi_bool bRegistered;
    
    /*! True is returned if this PLMN is forbidden, otherwise false is 
        returned.  This field is only applicable when used with 
        SwiGetPLMNSelection. */
    swi_bool bForbidden;
    
    /*! True is returned if this is the home PLMN, otherwise false is returned.
        This field is only applicable when used with SwiGetPLMNSelection. */
    swi_bool bHome;
    
    /*! True is returned if this PLMN is weak (signal is not of high quality), 
        otherwise false is returned.  This field is only applicable when used 
        with SwiGetPLMNSelection. */
    swi_bool bWeak;
    
    /*! True is returned if GPRS is supported on this PLMN, otherwise false is 
        returned. This field is only applicable when used with 
        SwiGetPLMNSelection. */
    swi_bool bGprsSupport;
    
    /*! True is returned if there are more PLMNs to be fetched from the modem.  
        False is returned if this is the last item in the list. This field is 
        only applicable when used with SwiGetPLMNSelection.*/
    swi_bool bMorePlmn;
};
/**
 *
 * This structure contains system status information.
 *            
 * \sa    SWI_TYPE_ModemStatus
 * \sa    SWI_TYPE_ServiceStatus
 * \sa    SWI_TYPE_ServiceErr
 * \sa    SWI_TYPE_ServiceType
 * \sa    SWI_TYPE_Service
 * \sa    SWI_TYPE_ModeSelection
 * \sa    SWI_TYPE_MncFormat
 * \sa    SWI_NOTIFY_GsmSystemStatus
 * \sa    SwiGetGsmSystemStatus
 *
 */
struct SWI_STRUCT_GsmSystemStatus
{
    /*! The size, in bytes, of this structure. */
    swi_uint32 sizeStruct;
    
    /*! Modem status */
    enum SWI_TYPE_ModemStatus eModemStatus;
    
    /*! Service status */
    enum SWI_TYPE_ServiceStatus eServiceStatus;
    
    /*! Service error */
    enum SWI_TYPE_ServiceErr eServiceErr;
    
    /*! Service type */
    enum SWI_TYPE_ServiceType eServiceType;
    
    /*! System mode */
    enum SWI_TYPE_Service eSystemMode;
    
    /*! Current radio band.  Values can be mapped to SWI_TYPE_RadioBand which
        is defined in SwiStructsCm.h. */
    swi_uint32 eCurrentBand;
    
    /*! True if roaming, false if on home network */
    swi_bool bRoaming;
    
    /*! Network selection mode (either manual or automatic) */
    enum SWI_TYPE_ModeSelection eManualMode;
    
    /*! NULL terminated country string */
    swi_char szCountry[LENGTH_Country];
    
    /*! NULL terminated network string */
    swi_char szNetwork[LENGTH_Network];
    
    /*! Mobile country code */
    swi_uint32 nMcc;
    
    /*! Mobile network code */
    swi_uint32 nMnc;
    
    /*! Location area code */
    swi_uint32 nLac;
    
    /*! Routing area code */
    swi_uint32 nRac;
    
    /*! Cell ID */
    swi_uint32 nCellId;
    
    /*! Channel number */
    swi_uint32 nChannelNum;
    
    /*! Primary Scrambling Code (WCDMA only, 0 for GSM) */
    swi_uint32 nPriScramCode;
    
    /*! MNC format (either 2-digit or 3-digit) */
    enum SWI_TYPE_MncFormat eMncFormat;
    
    /*! PLMN service error bit mask:
        - bit 0: manual PLMN acquisition time out
        - bit 1-7: unused */
    swi_uint32 nPlmnServiceErrMask;
};


/**
 *
 * This structure contains primary scramble code, RSCP, and Ec/Io  
 * information for an individual cell.
 *            
 * \sa SWI_STRUCT_RscpEcIo
 *
 */
struct SWI_STRUCT_RscpEcIoInfo
{
    swi_uint32 nPriScramCode;    /*!< Primary scramble code */
    swi_uint32 nRscp;            /*!< Received signal code power */
    swi_uint32 nEcIo;            /*!< Energy per chip per power density */
};

/**
 *
 * This object provides Received Signal Code Power (RSCP) and the total
 * Energy per chip per power density (Ec/Io) value of the active set's 
 * strongest cells, to a maximum of 6 (WCDMA only).  The Ec/Io value
 * will be provided as a U32 representation -dB in half step increments
 * and in order of strongest to weakest cell, which is based on the RSCP.
 *
 * For example, if the value returned for Ec/Io is 0x0015, Ec/Io = -10.5dB.
 *
 * These measurements are only valid for UMTS, if the current service is
 * GSM, then the number of cells will be set to 0.
 *
 * The notifications, if enabled, will be sent to the host application
 * about every 3 seconds only if something changes.
 *    
 * \sa SWI_STRUCT_RscpEcIoInfo    
 * \sa  SWI_NOTIFY_RscpEcIo
 * \sa SwiGetRscpEcIo
 *
 */
struct SWI_STRUCT_RscpEcIo
{
    /*! The size, in bytes, of this structure. */
    swi_uint32 sizeStruct;    
    
    /*!< Number of cells (max 6). */    
    swi_uint32 nNumOfCells;
    
    /*! Array of cell information. */
    struct SWI_STRUCT_RscpEcIoInfo    arCellInfo[MAX_CELLS]; 
};

/**
 *
 * Profile data structure used to read and store account information.
 * 
 * This structure should only be used with 2G devices.
 *
 * \sa SWI_TYPE_PDP
 * \sa SWI_TYPE_Precedence
 * \sa SWI_TYPE_Delay
 * \sa SWI_TYPE_Reliability
 * \sa SWI_TYPE_PeakThroughput
 * \sa SWI_TYPE_MeanThroughput
 * \sa SwiGetProfile
 * \sa SwiSetProfile
 *
 */
struct SWI_STRUCT_Profile
{
    /*!    The size, in bytes, of this structure.  This value must always be
        populated by the calling application. */
    swi_uint32 sizeStruct;
    
    /*! A NULL terminated string representing a user-supplied "handle" to help
        them identify the account. */                           
    swi_char szLabel[LENGTH_Label];
    
    /*! A NULL terminated string representing the carrier assigned account
        name for carrier authentication. */                        
    swi_char szUserName[LENGTH_UserName];
    
    /*! A NULL terminated string representing the account password for carrier
        authentication. */           
    swi_char szPassword[LENGTH_Password];
    
    /*! A NULL terminated string representing the access point name provided by
        the carrier. */           
    swi_char szAccessPoint[LENGTH_AccessPointName];
    
    /*! True indicates V.42bis is to be used for data, false indicates data 
        compression is off.  The carrier indicates if this is supported.  It is
        always safe to leave as "false", although throughput may be reduced. */    
    swi_bool bDataCompression;
    
    /*! True indicates TCP header compression is on, false indicates header 
        compression is off.  The carrier indicates if this is supported.  It is
        always safe to leave as "false", although throughput may be reduced. */                      
    swi_bool bHeaderCompression;
    
    /*! True indicates that this account should be automatically connected if 
        the profile is designated as default (see SwiSetDefaultProfile).  
        False indicates the host software must manually request to connect 
        with this account. */
    swi_bool bAutoActivation;
    
    /*! True indicates the IP address is to be manually assigned 
        (dwPDPAddress), false indicates the IP address is to be dynamically 
        assigned by the network.  Note that DHCP is not necessarily exchanged 
        over the air. */                       
    swi_bool bAutomaticIP;
    
    /*! PDP type (either IP or PPP). */                    
    enum SWI_TYPE_PDP ePDP;
    
    /*! Requested QoS precedence value. */                                  
    enum SWI_TYPE_Precedence eReqPrecedence;    
    
    /*! Requested QoS delay value. */                            
    enum SWI_TYPE_Delay eReqDelay;
    
    /*! Requested QoS reliability value. */                                 
    enum SWI_TYPE_Reliability eReqReliability;
    
    /*! Requested QoS peak throughput. */                         
    enum SWI_TYPE_PeakThroughput eReqPeakThroughput;
    
    /*! Requested QoS mean throughput. */                         
    enum SWI_TYPE_MeanThroughput eReqMeanThroughput;
    
    /*! Minimum QoS precedence value. */                   
    enum SWI_TYPE_Precedence eMinPrecedence;
    
    /*! Minimum QoS delay value. */                        
    enum SWI_TYPE_Delay eMinDelay;
    
    /*! Minimum QoS reliability value. */                             
    enum SWI_TYPE_Reliability eMinReliability;
    
    /*! Minimum QoS peak throughput. */                     
    enum SWI_TYPE_PeakThroughput eMinPeakThroughput;
    
    /*! Minimum QoS mean throughput. */                     
    enum SWI_TYPE_MeanThroughput eMinMeanThroughput;
    
    /*! Primary DNS can be defined, or if set to 0.0.0.0, obtained from the 
        network (where the network supports automatic assignment). */
    swi_uint32 dwPrimaryDns;
    
    /*! Secondary DNS can be defined, or if set to 0.0.0.0, obtained from the 
        network (where the network supports automatic assignment).    */
    swi_uint32 dwSecondaryDns;
    
    /*! The modem's dedicated IP address in cases where bAutomaticIP is set to
        false. */
    swi_uint32 dwPDPAddress;
    
    /*! True is returned if the profile is read only.  Otherwise, false is 
        returned.  Note that the read only attribute is a host software 
        enforced restriction.  The modem will simply save any information 
        the host software sends down. */
    swi_bool bReadOnly;
    
    /*! True is returned if the user should be prompted to enter a password.  
        Otherwise, false is returned. */                                
    swi_bool bPromptPassword;
    
    /*! True indicates to the host application a VPN client should be ran 
        automatically after a successful connection is made.  Otherwise, false 
        is returned. */        
    swi_bool bAutoRunVPN;                           
};



/**
 *
 * This structures contains the status of a profile.
 *
 * \sa SWI_STRUCT_ProfileSummary
 *
 */
struct SWI_STRUCT_ProfileStatus
{
    /*! Profile ID */
    swi_uint32 nProfileID;    
     
     /*! Current state of the profile (activated, inactive, or deleted). */  
    enum SWI_TYPE_ProfileState eProfileState;        
};


/**
 *
 * This structure contains a summary of all the profiles on the modem. 
 *
 * This structure should only be used with 2G devices.
 * 
 * \sa SwiGetProfileSummary
 *
 */
struct SWI_STRUCT_ProfileSummary
{
    /*!    The size, in bytes, of this structure.  This value must always be
        populated by the calling application. */
    swi_uint32 sizeStruct;
    
    /*! Default profile ID used by the modem for automatic connection to the 
        GPRS network. */
    swi_uint32 nDefaultProfileID;
    
    /*! The profile ID currently in use and connected. */    
    swi_uint32 nCurrentProfileID;
    
    /*! State of each profile.  Index 0 is associated with profile 1. */
    struct SWI_STRUCT_ProfileStatus arrProfileList[MAX_NO_CONTEXT];    
};

/**
 *
 * 3G GSM Profiles
 *
 */

/**
 *
 * This structure contains a summary of all the profiles on the modem.
 * 
 * This structure is used with 3G and newer devices.
 *
 * \sa SwiGetGsmProfileSummary
 *
 */
struct SWI_STRUCT_GsmProfileSummary
{
    /*!    The size, in bytes, of this structure.  This value must always be
        populated by the calling application. */
    swi_uint32 sizeStruct;
    
    /*! Maximum number of profiles supported by the modem. */                        
    swi_uint32 nMaxProfiles;
    
    /*! Maximum number of TFTs per profile. */    
    swi_uint32 nMaxTft;
    
    /*! Default profile ID used by the modem for automatic connection to the 
        GPRS network.  Note that the automatic flag must be enabled on the
        profile before an automatic connection is attempted by the modem. */    
    swi_uint32 nDefaultProfileId;
    
    /*! State of each profile.  Index 0 is associated with profile 1. */
    enum SWI_TYPE_ProfileState arrState[MAX_3G_GSM_PROFILES];    
};

/**
 *
 *   CNS ID     : CNS_PROFILE_AUTH (0x700E)
 *
 */
enum SWI_TYPE_ProfileAuth
{
    PROFILEAUTH_Username,
    PROFILEAUTH_Password,
    PROFILEAUTH_MaxUsername,
    PROFILEAUTH_MaxPassword
};

/**
 *
 * This object contains basic 3G GSM profile settings
 * 
 * This structure is used with 3G and newer devices.
 *
 * \sa SWI_TYPE_PDP
 * \sa SWI_TYPE_PdpInitiated
 * \sa SWI_TYPE_Authentication
 * \sa SwiGetGsmProfileBasic
 * \sa SwiSetGsmProfileBasic
 * \sa SWI_NOTIFY_GsmProfileChange
 *
 */
struct SWI_STRUCT_GsmProfileBasic
{
    /*!    The size, in bytes, of this structure. */
    swi_uint32 sizeStruct;
    
    /*! Profile ID.  Valid range is 1-16. */            
    swi_uint8 nProfileId;
    
    /*! True indicates if the basic profile info is valid, false otherwise. */
    swi_bool bValid;
    
    /*! PDP type (either IPV4, PPP, or IPV6). */                        
    enum SWI_TYPE_PDP ePdpType;    
    
    /*! True indicates V.42bis is to be used for data, false indicates data 
        compression is off.  The carrier indicates if this is supported.  
        It is always safe to leave as "false", although throughput may be 
        reduced. */    
    swi_bool bDataCompression;

    /*! True indicates TCP header compression is on, false indicates header 
        compression is off.  The carrier indicates if this is supported.  It is
        always safe to leave as "false", although throughput may be reduced. */                      
    swi_bool bHeaderCompression;

    /*! True indicates the IP address is to be manually assigned 
        (dwPDPAddress), false indicates the IP address is to be dynamically 
        assigned by the network.  Note that DHCP is not necessarily exchanged 
        over the air. */           
    swi_bool bAutomaticIP;
    
    /*! Length of PDP address (0-16).  0 indicates dynamic addressing. */                            
    swi_uint32 nLengthPdp;
    
    /*! IP or PDP address when using static PDP address configuration.
        Example of an IPV4 address is 0x7F000201 which means 127.0.2.1. */                            
    swi_uint8 byPdp[LENGTH_IPAddress];
    
    /*! A NULL terminated string representing the access point name provided by
        the carrier. */                
    swi_char  szAccessPoint[LENGTH_AccessPointName];
    
    /*! PDP initiated type */    
    enum SWI_TYPE_PdpInitiated ePdpInitiated;
    
    /*! This field is only valid if ePdpInitiated is set to Secondary Mobile 
        Initiated. */
    swi_uint32 nPrimaryProfileId;
    
    /*! Authentication type.  Default should be PAP.*/                
    enum SWI_TYPE_Authentication    eAuthentication;
               
    /*! A NULL terminated string representing the carrier assigned account
        name for carrier authentication. */            
    swi_char  szUserName[LENGTH_UserName];
    
    /*! A NULL terminated string representing the account password for carrier
        authentication. */            
    swi_char  szPassword[LENGTH_Password];
    
    /*! A NULL terminated string representing a user-supplied "handle" to help
        them identify the account. */           
    swi_char  szLabel[LENGTH_Label];
    
    /*! Indicates the maximum username string length that is supported by
        the device.  Valid values from this field will be from 32 to 127
        inclusive. */
    swi_uint32 nMaxUsernameLength;

    /*! Indicates the maximum password string length that is supported by
        the device.  Valid values from this field will be from 32 to 127
        inclusive. */

    swi_uint32 nMaxPasswordLength;
        /*! True indicates that this account should be automatically connected if
        the profile is designated as default (see SwiSetDefaultProfile).  
        False indicates the host software must manually request to connect 
        with this account. */

    swi_bool bAutoActivation;                        
    
    /*! True is returned if the profile is read only.  Otherwise, false is 
        returned.  Note that the read only attribute is a host software 
        enforced restriction.  The modem will simply save any information 
        the host software sends down. */
    swi_bool bReadOnly;

    /*! True is returned if the user should be prompted to enter a password.  
        Otherwise, false is returned. */                                
    swi_bool bPromptPassword;

    /*! True indicates to the host application a VPN client should be ran 
        automatically after a successful connection is made.
        Otherwise, false is returned. */        
    swi_bool bAutoRunVPN; 
    
    /*! Linger timer in minutes (1-32767).  A value of 0 means disabled. */                         
    swi_uint32 nLingerTimer;
    
    /*! Bitsets to store software options
        - 00000001    : HSDPA profile */
    swi_uint8 nSoftwareOptions;
    
    /*! Three bytes to store MCC/MNC value */
    swi_uint8 MccMnc[LENGTH_MCCMNC_BUFFER];
    
    /*! Reserved for future use */  
    swi_uint8 Reserved[11];                
};

/**
 *
 * This structure contains 3G GSM QoS profile settings.
 * 
 * This structure should only be used with 3G devices or newer.
 *
 * \sa SWI_TYPE_QOSUMTS_TrafficClass
 * \sa SWI_TYPE_QOSUMTS_Bitrate
 * \sa SWI_TYPE_QOSUMTS_DelOrder
 * \sa SWI_TYPE_QOSUMTS_DelErrSdu
 * \sa SWI_TYPE_QOSUMTS_SduErrRatio
 * \sa SWI_TYPE_QOSUMTS_ResBitErrRatio
 * \sa SWI_TYPE_QOSUMTS_TraffHandPri
 * \sa SWI_TYPE_QOSUMTS_SrcStatDesc
 * \sa SWI_TYPE_QOSUMTS_SignInd
 * \sa SWI_TYPE_Precedence
 * \sa SWI_TYPE_Delay
 * \sa SWI_TYPE_Reliability
 * \sa SWI_TYPE_PeakThroughput
 * \sa SWI_TYPE_MeanThroughput
 * \sa SWI_NOTIFY_GsmProfileChange
 * \sa SwiGetGsmProfileQos
 * \sa SwiSetGsmProfileQos
 *
 */
struct SWI_STRUCT_GsmProfileQos
{
    /*!    The size, in bytes, of this structure. */
    swi_uint32 sizeStruct;

    /*! Profile ID.  Valid range is 1-16. */        
    swi_uint8 nProfileId;    
    
    /*! True indicates the requested UMTS QoS values are valid,
        false otherwise. */                
    swi_bool bValidUmtsReq;
    
    /*! Requested UMTS QoS traffic class. */        
    enum SWI_TYPE_QOSUMTS_TrafficClass eReqTrafficClass;
    
    /*! Requested UMTS QoS maximum upload bit rate. */        
    enum SWI_TYPE_QOSUMTS_Bitrate eReqMaxBitrateUl;
    
    /*! Requested UMTS QoS maximum download bit rate. */        
    enum SWI_TYPE_QOSUMTS_Bitrate eReqMaxBitrateDl;
    
    /*! Requested UMTS QoS guaranteed upload bit rate. */
    enum SWI_TYPE_QOSUMTS_Bitrate eReqGuarBitrateUl;
    
    /*! Requested UMTS QoS guaranteed download bit rate. */        
    enum SWI_TYPE_QOSUMTS_Bitrate eReqGuarBitrateDl;
    
    /*! Requested UMTS QoS delivery order. */        
    enum SWI_TYPE_QOSUMTS_DelOrder eReqDelOrder;
    
    /*! Requested UMTS QoS delivery of erroneous SDU. */        
    enum SWI_TYPE_QOSUMTS_DelErrSdu eReqDelErrSdu;
    
    /*! Requested UMTS QoS maximum SDU size    (0-1520). */            
    swi_uint32 nReqMaxSdu;
    
    /*! Requested UMTS QoS SDU error ratio. */                
    enum SWI_TYPE_QOSUMTS_SduErrRatio eReqSduErrRatio;
    
    /*! Requested UMTS QoS residual bit error ratio. */        
    enum SWI_TYPE_QOSUMTS_ResBitErrRatio    eReqResBitErrRatio;
    
    /*! Requested UMTS QoS transfer delay (0-4000) */
    swi_uint32 nReqTransDelay;
    
    /*! Requested UMTS QoS traffic handling priority. */            
    enum SWI_TYPE_QOSUMTS_TraffHandPri eReqTraffHandPri;
    
    /*! Requested UMTS QoS source statistics descriptor. */        
    enum SWI_TYPE_QOSUMTS_SrcStatDesc eReqSrcStatDesc;
    
    /*! Requested UMTS QoS signal indication. */        
    enum SWI_TYPE_QOSUMTS_SignInd eReqSignInd;
    
    /*! True indicates the minimum UMTS QoS values are valid, false 
        otherwise. */
    swi_bool bValidUmtsMin;
    
    /*! Minimum UMTS QoS traffic class. */
    enum SWI_TYPE_QOSUMTS_TrafficClass eMinTrafficClass;
    
    /*! Minimum UMTS QoS maximum upload bit rate. */    
    enum SWI_TYPE_QOSUMTS_Bitrate eMinMaxBitrateUl;
    
    /*! Minimum UMTS QoS maximum download bit rate. */        
    enum SWI_TYPE_QOSUMTS_Bitrate eMinMaxBitrateDl;
    
    /*! Minimum UMTS QoS guaranteed upload bit rate. */    
    enum SWI_TYPE_QOSUMTS_Bitrate eMinGuarBitrateUl;
    
    /*! Minimum UMTS QoS guaranteed download bit rate. */
    enum SWI_TYPE_QOSUMTS_Bitrate eMinGuarBitrateDl;
    
    /*! Minimum UMTS QoS delivery order. */        
    enum SWI_TYPE_QOSUMTS_DelOrder eMinDelOrder;
    
    /*! Minimum UMTS QoS delivery of erroneous SDU. */            
    enum SWI_TYPE_QOSUMTS_DelErrSdu eMinDelErrSdu;
    
    /*! Minimum UMTS QoS maximum SDU size (0-1520). */            
    swi_uint32 nMinMaxSdu;    
    
    /*! Minimum UMTS QoS SDU error ratio. */            
    enum SWI_TYPE_QOSUMTS_SduErrRatio eMinSduErrRatio;
    
    /*! Minimum UMTS QoS residual bit error ratio. */
    enum SWI_TYPE_QOSUMTS_ResBitErrRatio    eMinResBitErrRatio;
    
    /*! Minimum UMTS QoS transfer delay (0-4000). */    
    swi_uint32 nMinTransDelay;
    
    /*! Minimum UMTS QoS traffic handling priority. */
    enum SWI_TYPE_QOSUMTS_TraffHandPri eMinTraffHandPri;
    
    /*! Minimum UMTS QoS source statistics descriptor. */
    enum SWI_TYPE_QOSUMTS_SrcStatDesc eMinSrcStatDesc;
    
    /*! Minimum UMTS QoS signal indication. */        
    enum SWI_TYPE_QOSUMTS_SignInd eMinSignInd;
    
    /*! True indicates if the following requested QoS values are valid, false 
        otherwise. */
    swi_bool bValidGprsReq;            
    
    /*! Requested QoS precedence value. */                                  
    enum SWI_TYPE_Precedence eReqPrecedence;    

    /*! Requested QoS delay value. */                        
    enum SWI_TYPE_Delay eReqDelay;

    /*! Requested QoS reliability value. */                                 
    enum SWI_TYPE_Reliability eReqReliability;

    /*! Requested QoS peak throughput. */                         
    enum SWI_TYPE_PeakThroughput eReqPeak;

    /*! Requested QoS mean throughput. */                         
    enum SWI_TYPE_MeanThroughput eReqMean;
    
    /*! True indicates the minimum QoS values are valid, false otherwise. */
    swi_bool bValidGprsMin;                                
    
    /*! Minimum QoS precedence value. */                   
    enum SWI_TYPE_Precedence eMinPrecedence;

    /*! Minimum QoS delay value. */                        
    enum SWI_TYPE_Delay eMinDelay;

    /*! Minimum QoS reliability value. */                             
    enum SWI_TYPE_Reliability eMinReliability;

    /*! Minimum QoS peak throughput. */                     
    enum SWI_TYPE_PeakThroughput eMinPeak;

    /*! Minimum QoS mean throughput. */                     
    enum SWI_TYPE_MeanThroughput eMinMean;            
};

/**
 *
 * This structure contains 3G GSM DNS profile settings.
 *
 * \sa SWI_NOTIFY_GsmProfileChange
 * \sa SwiGetGsmProfileDns
 * \sa SwiSetGsmProfileDns
 *
 */
struct SWI_STRUCT_GsmProfileDns
{
    /*!    The size, in bytes, of this structure. */
    swi_uint32 sizeStruct;

    /*! Profile ID.  Valid range is 1-16. */                        
    swi_uint8 nProfileId;    
    
    /*! Length of the primary DNS address.  Valid range is 0-16.  0 indicates 
        no DNS address present and in this case the modem will solicit a 
        network-supplied DNS address.    */    
    swi_uint32    nLengthPriDns;
    
    /*! A binary representation of the primary DNS.
        Ex. 0x7F000201 is 127.0.2.1. */                
    swi_uint8 byPriDns[LENGTH_IPAddress];    
    
    /*! Length of the primary DNS address.  Valid range is 0-16.  0 indicates 
        no DNS address present and in this case the modem will solicit a 
        network-supplied DNS address. */
    swi_uint32 nLengthSecDns;
    
    /*! A binary representation of the secondary DNS 
        (Ex. 0x7F000201 is 127.0.2.1.) */            
    swi_uint8 bySecDns[LENGTH_IPAddress];    
};


/**
 *
 * This structure contains 3G GSM TFT profile settings.
 *
 * \sa SWI_NOTIFY_GsmProfileChange
 * \sa SwiGetGsmProfileTft
 * \sa SwiSetGsmProfileTft
 *
 */
struct SWI_STRUCT_GsmProfileTft
{
    /*!    The size, in bytes, of this structure. */
    swi_uint32 sizeStruct;

    /*! Profile ID.  Valid range is 1-16. */                        
    swi_uint8 nProfileId;    
    
    /*! TFT filter ID.  Only 2 filters are currently supported (1-2). */                        
    swi_uint32 nFilterId;
    
    /*! True indicates the TFT information is valid, false otherwise. */                        
    swi_bool bValid;
    
    /*! Evaluation precedence index.  Valid range is 0-255. */
    swi_uint32 nEpi;
    
    /*! IP length, where 4 indicates IPV4 while 16 indicates IPV6. */                            
    swi_uint32 nLengthIp;
    
    /*! A binary value representing the source address.
        Ex. 0xFFFFFF00 is 255.255.255.0. */                        
    swi_uint8 bySrcIp[LENGTH_IPAddress];    
    
    /*! A binary value representing the source address mask.  
        Ex. 0xFFFFFF00 is 255.255.255.0. */
    swi_uint8 bySrcIpMask[LENGTH_IPAddress];    
    
    /*! Protocol number (IPV4) / next header (IPV6). */
    swi_uint32  nProtoNum;    
    
    /*! Destination port range "from".  Valid values range from 0-65535). */                        
    swi_uint32 nDestPortFrom;
    
    /*! Destination port range "to".  Valid values range from 0-65535). */                        
    swi_uint32 nDestPortTo;
    
    /*! Source port range "from".  Valid values range from 0-65535). */                    
    swi_uint32 nSrcPortFrom;
    
    /*! Source port range "to".  Valid values range from 0-65535). */
    swi_uint32 nSrcPortTo;    
    
    /*! Security parameter index.  Valid values range from 
        0x00000000-0xFFFFFFFF. */
    swi_uint32 nSecurParamInd;    
    
    /*! Type of service (IPV4) / traffic class (IPV6). */                
    swi_uint32    nTypeServ;
    
    /*! Type of service mask (IPV4) / traffic class mask (IPV6). */                    
    swi_uint32 nTypeServMask;
    
    /*! Flow label (IPV6).  Valid values range from 0x00000 - 0xFFFFF. */                    
    swi_uint32 nFlowLabel;                        
};


/**
 *
 * This structure contains packet session status information.  
 *
 * \sa SWI_TYPE_Precedence
 * \sa SWI_TYPE_Delay
 * \sa SWI_TYPE_Reliability
 * \sa SWI_TYPE_MeanThroughput
 * \sa SWI_TYPE_PeakThroughput
 * \sa SWI_TYPE_InactivityReason
 * \sa SWI_NOTIFY_PktSessionStatus
 * \sa SwiGetPktSessionStatus
 *
 */
struct SWI_STRUCT_PktSessionStatus
{
    /*!    The size, in bytes, of this structure.  This value must be provided
        when calling SwiGetPktSessionStatus.  */
    swi_uint32 sizeStruct;
    
    /*! True is returned if a packet session is active.  Otherwise, false is 
        returned. */
    swi_bool bPacketSessionActive;    
    
    /*! True is returned of the following QoS fields are valid.  Otherwise, 
        false is returned. */
    swi_bool bValidQos;
    
    /*! Current QoS precedence.  Only valid if bValidQos is true. */        
    enum SWI_TYPE_Precedence eNegoPrecedence;
    
    /*! Current QoS delay.  Only valid if bValidQos is true.   */        
    enum SWI_TYPE_Delay eNegoDelay;
    
    /*! Current QoS reliability.  Only valid if bValidQos is true. */            
    enum SWI_TYPE_Reliability eNegoReliability;    
    
    /*! Current QoS mean throughput.  Only valid if bValidQos is true. */    
    enum SWI_TYPE_MeanThroughput eNegoMeanThroughput;    
    
    /*! Current QoS peak throughput.  Only valid if bValidQos is true. */
    enum SWI_TYPE_PeakThroughput eNegoPeakThroughput;    
    
    /*! Inactivity reason code.  Only valid if bPacketSessionActive is false.*/
    enum SWI_TYPE_InactivityReason eInactivityReason;        
};


/**
 *
 * This structure contains 3G packet session information.  
 *
 * \sa SWI_TYPE_Precedence
 * \sa SWI_TYPE_Delay
 * \sa SWI_TYPE_Reliability
 * \sa SWI_TYPE_MeanThroughput
 * \sa SWI_TYPE_PeakThroughput
 * \sa SWI_TYPE_QOSUMTS_TrafficClass
 * \sa SWI_TYPE_QOSUMTS_Bitrate
 * \sa SWI_TYPE_QOSUMTS_DelOrder
 * \sa SWI_TYPE_QOSUMTS_DelErrSdu
 * \sa SWI_TYPE_QOSUMTS_SduErrRatio
 * \sa SWI_TYPE_QOSUMTS_ResBitErrRatio
 * \sa SWI_TYPE_QOSUMTS_TraffHandPri
 * \sa SWI_TYPE_QOSUMTS_SrcStatDesc
 * \sa SWI_TYPE_QOSUMTS_SignInd
 * \sa SWI_TYPE_InactivityReason
 * \sa SWI_NOTIFY_PktSessionCall
 *
 */
struct SWI_STRUCT_PktSessionCall
{
    /*! The size, in bytes, of this structure. */
    swi_uint32 sizeStruct;    
    
    /*! Profile ID. */     
    swi_uint32 nProfileId;
    
    /*! True if packets session active, false otherwise. */            
    swi_bool bPacketSessionActive;    
    
    /*! True if QoS information valid, false otherwise. */
    swi_bool bValidQos;    
    
    /*! Negotiated QoS precedence. */            
    enum SWI_TYPE_Precedence ePrec;
    
    /*! Negotiated QoS delay. */                    
    enum SWI_TYPE_Delay eDelay;    
    
    /*! Negotiated QoS reliability. */                
    enum SWI_TYPE_Reliability eReli;    
    
    /*! Negotiated QoS mean throughput. */                
    enum SWI_TYPE_MeanThroughput eMean;
    
    /*! Negotiated QoS peak throughput. */                
    enum SWI_TYPE_PeakThroughput ePeak;    
    
    /*! Negotiated UMTS QoS traffic class. */            
    enum SWI_TYPE_QOSUMTS_TrafficClass eTrafficClass;    
    
    /*! Negotiated UMTS QoS maximum upload bit rate. */        
    enum SWI_TYPE_QOSUMTS_Bitrate eMaxBitrateUl;    
    
    /*! Negotiated UMTS QoS maximum download bit rate. */        
    enum SWI_TYPE_QOSUMTS_Bitrate eMaxBitrateDl;    
    
    /*! Negotiated UMTS QoS guaranteed upload bit rate. */    
    enum SWI_TYPE_QOSUMTS_Bitrate eGuarBitrateUl;    
    
    /*! Negotiated UMTS QoS guaranteed download bit rate. */        
    enum SWI_TYPE_QOSUMTS_Bitrate eGuarBitrateDl;
    
    /*! Negotiated UMTS QoS delivery order. */            
    enum SWI_TYPE_QOSUMTS_DelOrder eDelOrder;
    
    /*! Negotiated UMTS QoS delivery of erroneous SDU. */            
    enum SWI_TYPE_QOSUMTS_DelErrSdu eDelErrSdu;
    
    /*! Negotiated UMTS QoS maximum SDU size (0-1520). */            
    swi_uint32 nMaxSdu;
    
    /*! Negotiated UMTS QoS SDU error ratio. */                
    enum SWI_TYPE_QOSUMTS_SduErrRatio eSduErrRatio;
    
    /*! Negotiated UMTS QoS residual bit error ratio. */            
    enum SWI_TYPE_QOSUMTS_ResBitErrRatio    eResBitErrRatio;
    
    /*! Negotiated UMTS QoS transfer delay (0-4000). */        
    swi_uint32 nTransDelay;
    
    /*! Negotiated UMTS QoS traffic handling priority. */            
    enum SWI_TYPE_QOSUMTS_TraffHandPri eTraffHandPri;
    
    /*! Negotiated UMTS QoS source statistics descriptor. */            
    enum SWI_TYPE_QOSUMTS_SrcStatDesc eSrcStatDesc;
    
    /*! Negotiated UMTS QoS signal indication. */            
    enum SWI_TYPE_QOSUMTS_SignInd eSignInd;
    
    /*! Inactivity reason.  This code is only used when bpacketSessionActive 
        is false. */
    enum SWI_TYPE_InactivityReason eInactivityReason;        
};

/**
 *
 * This structure contains an IP address.
 *
 * \sa SwiGetIPAddressEx
 *
 */
struct SWI_STRUCT_IpAddress
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct;    
    
    /*! Profile ID. */              
    swi_uint32 nProfileId;
    
    /*! Length of the IP address  
        - 0: no IP address
        - 4: IPV4
        - 16: IPV6 */                
    swi_uint32 nIpLength;
    
    /*! IP address.  Example of an IPV4 address is 0x7F000201 which means 
        127.0.2.1. */
    swi_uint8 byIp[LENGTH_IPAddress];    
};
/**
 *
 * This structure contains information pertaining to the current network
 * status. 
 *
 * \sa SWI_TYPE_ServiceStatus
 * \sa SWI_TYPE_ServiceType
 * \sa SWI_TYPE_RoamingState
 * \sa SWI_TYPE_ModeSelection
 * \sa SWI_NOTIFY_NetworkStatus
 * \sa SwiGetNetworkStatus
 *
 */
struct SWI_STRUCT_GSM_NetworkStatus
{
    /*!    The size, in bytes, of this structure. */
    swi_uint32 sizeStruct;        
    
    /*! Current service status. */
    enum SWI_TYPE_ServiceStatus eServiceStatus;    
        
    /*! Current service type. */
    enum SWI_TYPE_ServiceType eServiceType;    
    
    /*! Device roaming state. */
    enum SWI_TYPE_RoamingState eRoaming;            
    
    /*! Current network selection mode. */
    enum SWI_TYPE_ModeSelection eModeSelection;
    
    /*! A NULL terminated string representing the country name. */        
    swi_char szCountry[5];        
    
    /*! A NULL terminated string representing the network name. */
    swi_char szNetwork[9];        
    
    /*! Mobile Country Code */
    swi_uint32 nMCC;
    
    /*! Mobile Network Code */                
    swi_uint32 nMNC;        
    
    /*! Local Area Code */        
    swi_uint32 nLAC;        
    
    /*! Cell ID */        
    swi_uint32 nCellID;            
};

/**
 *
 * This structure is used to indicate if the modem is ready to conduct a
 * PLMN network search.          
 *
 * No PLMN searches should be initiated by the SwiStartPLMNSearch API
 * until the modem is ready.
 *
 * \sa SWI_NOTIFY_PlmnReady
 * \sa SwiStartPLMNSearch
 *            
 */
struct SWI_STRUCT_PlmnReady
{
    /*!    The size, in bytes, of this structure. */
    swi_uint32 sizeStruct;    
    
    /*! True is returned if the modem is ready to conduct a PLMN search, 
        false otherwise. */    
    swi_bool bReady;                
};


/**
 *
 * This structure is used to indicate if the modem PLMN selection change
 * was successful.
 *            
 * \sa SWI_NOTIFY_PlmnMode
 * \sa SwiSetPLMNMode
 *
 */
struct SWI_STRUCT_PlmnMode
{
    /*!    The size, in bytes, of this structure. */
    swi_uint32 sizeStruct;    
    
    /*! Result of a PLMN mode change request. */
    enum SWI_TYPE_PLMN_Result eResult;    
};

/**
 *
 * This structure contains name strings to be displayed to the user to
 * indicate what network the modem is registered on.  The service provider 
 * name and network name (PLMN) are given.
 *
 * \sa SWI_NOTIFY_Registration
 *
 */
struct SWI_STRUCT_Registration
{
    /*!    The size, in bytes, of this structure.  This value must always be
        populated by the calling application. */
    swi_uint32 sizeStruct;

    /*! True indicates the home string should be displayed in place of the 
        service provider name.  Otherwise, false is returned.  This flag is 
        present for historical reasons and can be ignored. */
    swi_bool bDisplayHomeString; 
    
    /*! True indicates the home country roaming should be displayed.  
        Otherwise, false is returned.  This flag is present for historical 
        reasons and can be ignored. */
    swi_bool bDisplayHCR;
    
    /*! A NULL terminated string representing the service provider name. */
    swi_char szSPN [LENGTH_SPN];
    
    /*! A NULL terminated string representing the PLMN. */
    swi_char szPLMN[LENGTH_PLMN];
    
    /*! A NULL terminated string representing the abbreviated PLMN name. */
    swi_char szAbvPLMN[LENGTH_AbvPLMN];    
};




/**
 *
 * This structure is used to convey the result of a PLMN search.  
 *
 * \sa SWI_NOTIFY_PlmnAvailable
 *
 */
struct SWI_STRUCT_PlmnAvailable
{
    /*!    The size, in bytes, of this structure. */
    swi_uint32 sizeStruct;    
    
    /*! True is returned if the PLMN list is available, false otherwise. */        
    swi_bool bAvailable;            
};


/**
 *
 * This structure contains service state information.
 *
 * \sa SWI_TYPE_Service
 * \sa SWI_NOTIFY_ServiceIcon
 *
 */
struct SWI_STRUCT_ServiceIcon
{
    /*! The size, in bytes, of this structure.  */
    swi_uint32 sizeStruct;
    
    /*! Version of this structure.  This field is obsolete and should be
        ignored. */
    swi_uint32 nVersion;
    
    /*! True is returned if the service icon should be displayed, false
        otherwise.  This was created because some carriers prefers not to 
        display the service icon even if there is service in the area. */
    swi_bool bDisplayIcon;
    
    /*! Type of service that is available. */
    enum SWI_TYPE_Service eService;
    
    /*! True if the device is attached to the network, false otherwise. */
    swi_bool bAttached;
    
    /*! True if there is an active packet session, false otherwise. */
    swi_bool bPktSessionActive;
};

/**
 *
 * Function Prototypes
 *
 */
SWI_API SWI_RCODE SwiGetRssiBer(struct SWI_STRUCT_Rssi *sRssiBer, 
                                swi_uint32 timeout);
SWI_API SWI_RCODE SwiGetRssiBars(const swi_int32 iRssi, 
                                 swi_uint32 *pNumBars);
SWI_API SWI_RCODE SwiGetTrafficStatistics(swi_uint32 *nSent, 
                                          swi_uint32 *nReceived, 
                                          swi_uint32 Timeout);
SWI_API SWI_RCODE SwiResetTrafficStatistics(swi_uint32 timeout);
SWI_API SWI_RCODE SwiGetRscpEcIo(struct SWI_STRUCT_RscpEcIo* pRscpEcIo, 
                                 swi_uint32 timeout);

/* GSM Profile APIs */
SWI_API SWI_RCODE SwiGetGsmProfileSummary(
    struct SWI_STRUCT_GsmProfileSummary* pProfileSummary, 
    swi_uint32 timeout);
SWI_API SWI_RCODE SwiGetGsmProfileBasic(
    struct SWI_STRUCT_GsmProfileBasic* pProfile, 
    swi_uint32 timeout);
SWI_API SWI_RCODE SwiGetGsmProfileQos(
    struct SWI_STRUCT_GsmProfileQos* pProfile, 
    swi_uint32 timeout);
SWI_API SWI_RCODE SwiGetGsmProfileDns(
    struct SWI_STRUCT_GsmProfileDns* pProfile, 
    swi_uint32 timeout);
SWI_API SWI_RCODE SwiGetGsmProfileTft(
    struct SWI_STRUCT_GsmProfileTft* pProfile, 
    swi_uint32 timeout);
SWI_API SWI_RCODE SwiSetGsmProfileBasic(
    struct SWI_STRUCT_GsmProfileBasic pProfile, 
    swi_uint32 timeout);
SWI_API SWI_RCODE SwiSetGsmProfileQos(
    struct SWI_STRUCT_GsmProfileQos pProfile, 
    swi_uint32 timeout);
SWI_API SWI_RCODE SwiSetGsmProfileDns(
    struct SWI_STRUCT_GsmProfileDns pProfile, 
    swi_uint32 timeout);
SWI_API SWI_RCODE SwiSetGsmProfileTft(
   struct SWI_STRUCT_GsmProfileTft pProfile, 
   swi_uint32 timeout);
SWI_API SWI_RCODE SwiEraseProfile(
    const swi_uint32 nProfileID, 
    swi_uint32 timeout);
SWI_API SWI_RCODE SwiSetDefaultProfile(
    const swi_uint32 nProfileID, 
    swi_uint32 timeout);
SWI_API SWI_RCODE SwiActivateProfile(
    const swi_uint32 nProfileID, swi_bool Activate, 
    swi_uint32 timeout);

/* GSM Network APIs */
SWI_API SWI_RCODE SwiSetHostStartup(
    swi_bool bStartup, 
    swi_uint32 timeout);
SWI_API SWI_RCODE SwiGetIPAddressEx(
    struct SWI_STRUCT_IpAddress* pIp, 
    swi_uint32 timeout);
SWI_API SWI_RCODE SwiGetPktSessionStatus(
    struct SWI_STRUCT_PktSessionStatus *sSessionStatus, 
    swi_uint32 timeout);
SWI_API SWI_RCODE SwiGetPLMNMode(
    enum SWI_TYPE_PLMN_Mode* eMode, 
    swi_uint32 timeout);
SWI_API SWI_RCODE SwiSetPLMNMode(
    const struct SWI_STRUCT_PlmnSelection sPlmn, 
    swi_uint32 timeout);
SWI_API SWI_RCODE SwiStartPLMNSearch(swi_uint32 timeout);
SWI_API SWI_RCODE SwiGetPLMNSelection(
    struct SWI_STRUCT_PlmnSelection* sPlmn, 
    swi_uint32 timeout);
SWI_API SWI_RCODE SwiGetServiceStatusEx(
    enum SWI_TYPE_ServiceStatus *eServiceStatus, 
    swi_uint32 timeout);
SWI_API SWI_RCODE SwiGetPktSessionStatusEx(
    struct SWI_STRUCT_PktSessionCall *sPktSessionCall, 
    swi_uint32 timeout);
SWI_API SWI_RCODE SwiGetRat(
    struct SWI_STRUCT_Rat *psRat,
    swi_uint32 timeout);
SWI_API SWI_RCODE SwiSetRat(
    struct SWI_STRUCT_Rat *psRat,
    swi_uint32 timeout);
SWI_API SWI_RCODE SwiGetPrlRegion(
    struct SWI_STRUCT_PrlRegion *psPrl,
    swi_uint32 timeout);
SWI_API SWI_RCODE SwiSetPrlRegion(
    struct SWI_STRUCT_PrlRegion *psPrl,
    swi_uint32 timeout);
SWI_API SWI_RCODE SwiGetGsmSystemStatus(
    struct SWI_STRUCT_GsmSystemStatus* pSysStatus, 
    swi_uint32 timeout);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __SWIAPI_GSM_NETWORK_H */

/* 
 * $Log: SwiApiGsmNetwork.h,v $
 */
