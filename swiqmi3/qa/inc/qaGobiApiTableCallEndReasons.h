/**
 * \ingroup wds
 *
 * \file    qaGobiApiTableCallEndReasons.h
 *
 * \brief   Wireless Data Service Call End Reasons
 *
 * \section Table1 Call end reason codes (Code - Reason)
 * \subsection gen Technology-agnostic call end reasons
 * \li 1 - Reason unspecified
 * \li 2 - Client ended the call
 * \li 3 - Device has no service
 * \li 4 - Call ended abnormally
 * \li 5 - Received release from base station; no reason given
 * \li 6 - Access attempt already in progress; SD2.0 only
 * \li 7 - Access failure for reason other than the above
 * \li 8 - Call rejected because of redirection or handoff
 * \li 9 - Call failed because close is in progress
 * \li 10 - Authentication failed
 * \li 11 - Call ended because of internal call end. This error code is returned when data call is brought down
 *          due to some unknown error, such as not specific to any RAT
 * \li 12 - Call ended because of internal error. This error code is returned when data call is brought down 
 *          due to some unspecified internal error, such as NULL pointer
 * \li 13 - Internal unknown casue code
 * \subsection CDMA CDMA
 * \li 500 - Device is CDMA-locked until power cycle
 * \li 501 - Received intercept from base station; origination only
 * \li 502 - Received reorder from base station; origination only
 * \li 503 - Received release from base station; service option reject
 * \li 504 - Received incoming call from base station
 * \li 505 - Received alert stop from base station; incoming only
 * \li 506 - Received end activation; OTASP call only
 * \li 507 - Max access probes transmitted
 * \li 508 - Concurrent service is not supported by base station
 * \li 509 - No response received from base station
 * \li 510 - Call rejected by the base station; CDMA only
 * \li 511 - Concurrent services requested were not compatible; CDMA only
 * \li 512 - Corresponds to CM_CALL_ORIG_ERR_ALREADY_IN_TC
 * \li 513 - Used if Call manager subsystem is ending a GPS call in favor of a user call
 * \li 514 - Used if Call manager subsystem is ending a SMS call in favor of a user call
 * \li 515 - CDMA Only; Device has no service
 * \subsection GSM WCDMA/GSM call end reasons
 * \li 1000 - Call origination request failed; WCDMA/GSM Only
 * \li 1001 - Client rejected the incoming call; WCDMA/GSM Only
 * \li 1002 - Device has no UMTS service; WCDMA/GSM Only
 * \li 1003 - Network ended the call, look in cc_call; WCDMA/GSM Only
 * \li 1004 - LLC or SNDCP failure
 * \li 1005 - Insufficient resources
 * \li 1006 - Service option temporarily out of order
 * \li 1007 - NSAPI already used
 * \li 1008 - Regular PDP context deactivation
 * \li 1009 - Network failure, This cause is sent to the MS if the MSC cannot service an MS generated request because of PLMN failures, e.g.
 *            problems in MAP
 * \li 1010 - Reactivation requested
 * \li 1011 - Protocol error, unspecified
 * \li 1012 - Operator-determined barring
 * \li 1013 - Unknown or missing Access Point Name (APN)
 * \li 1014 - Unknown PDP address or PDP type
 * \li 1015 - Activation rejected by GGSN
 * \li 1016 - Activation rejected, unspecified
 * \li 1017 - Service option not supported
 * \li 1018 - Requested service option not subscribed
 * \li 1019 - Quality of Service (QoS) not accepted
 * \li 1020 - Semantic error in the TFT operation
 * \li 1021 - Syntactical error in the TFT operation
 * \li 1022 - Unknown PDP context
 * \li 1023 - Semantic errors in packet filter(s)
 * \li 1024 - Syntactical error in packet filter(s)
 * \li 1025 - PDP context without TFT already activated
 * \li 1026 - Invalid transaction identifier value
 * \li 1027 - Semantically incorrect message, This cause is used to report receipt of a message with semantically incorrect contents
 * \li 1028 - Invalid mandatory information, This cause indicates that the equipment sending this cause has received a message with a non-semantical
 *            mandatory IE error
 * \li 1029 - Message type non-existent or not implemented, This cause indicates that the equipment sending this cause has received a message with a
 *            message type it does not recognize either because this is a message not defined, or defined but not implemented by the equipment sending this cause.
 * \li 1030 - Message not compatible with state, This cause indicates that the equipment sending this cause has received a message not compatible with the
 *            protocol state
 * \li 1031 - Information element nonexistent or not implemented, This cause indicates that the equipment sending this cause has received a message which
 *            includes information elements not recognized because the information element identifier is not defined or it is defined but not implemented by the equipment
 *            sending the cause.
 * \li 1032 - Conditional information element error, This cause indicates that the equipment sending this cause has received a message with conditional IE errors
 * \li 1033 - Message not compatible with protocol state, This cause indicates that a message has been received which is incompatible with the protocol state or that a
 *            STATUS message has been received indicating an incompatible call state
 * \li 1034 - APN restriction value incompatible with active PDP context
 * \li 1035 - No GPRS context present
 * \li 1036 - Requested feature not supported
 * \li 1037 - Illegal MS, This cause is sent to the MS when the network refuses service to the MS either because an identity of the MS is
 *            not acceptable to the network or because the MS does not pass the authentication check, i.e. the SRES received from the MS
 *            is different from that generated by the network
 * \li 1038 - Illegal ME, This cause is sent to the MS if the ME used is not acceptable to the network, e.g. blacklisted
 * \li 1039 - GPRS and non GPRS services not allowed
 * \li 1040 - GPRS services not allowed
 * \li 1041 - MS identity not derived by the network
 * \li 1042 - Implicitly detached, This cause is sent to the MS either if the network has implicitly detached the MS, e.g. some while after the
 *            Mobile reachable timer has expired, or if the GMM context data related to the subscription dose not exist in the SGSN e.g. because of 
 *            a SGSN restart.
 * \li 1043 - PLMN not allowed, This cause is sent to the MS if it requests location updating in a PLMN where the MS, by subscription or due to
 *            operator determined barring is not allowed to operate
 * \li 1044 - LA not allowed, This cause is sent to the MS if it requests location updating in a location area where the HPLMN determines that
 *            the MS, by subscription, is not allowed to operate.
 * \li 1045 - GPRS services not allowed in PLMN
 * \li 1046 - PDP duplicate
 * \li 1047 - UE radio access technology change
 * \li 1048 - app preempted
 * \li 1049 - Congestion, This cause is sent if the service request or LOCATION UPDATING REQUEST message cannot be actioned
 *            because of congestion (e.g. congestion of the MSC or SGSN or GGSN or PDN Gateway; no channel; facility busy/congested etc.).
 * \li 1050 - No PDP context activated
 * \li 1051 - Access class DSAC rejection
 * \subsection CDMA_EVDO CDMA 1xEV-DO
 * \li 1500 - Abort connection setup due to the reception of a ConnectionDeny message with deny code set
 *            to either general or network busy.
 * \li 1501 - Abort connection setup due to the reception of a ConnectionDeny message with deny code set
 *            to either billing or authentication failure.
 * \li 1502 - Change HDR system due to redirection or PRL not preferred
 * \li 1503 - Exit HDR due to redirection or PRL not preferred
 * \li 1504 - No HDR session
 * \li 1505 - Used if Call manager is ending an HDR call origination in favor of a GPS fix
 * \li 1506 - Connection setup timeout
 * \li 1507 - Call manager released HDR call so 1x call can continue
 * \subsection reason call end reason type
 * \li 1 - Mobile IP
 * \li 2 - Internal
 * \li 3 - Call Manager defined
 * \li 6 - 3GPP specification defined
 * \li 7 - PPP
 * \li 8 - EHRPD
 * \li 9 - IPv6
 * \subsection MobileIP Mobile IP call end reasons (Type=1)
 * \li 64  - MIP_FA_ERR_REASON_UNSPECIFIED
 * \li 65  - MIP_FA_ERR_ADMINISTRATIVELY_PROHIBITED
 * \li 66  - MIP_FA_ERR_INSUFFICIENT_RESOURCES
 * \li 67  - MIP_FA_ERR_MOBILE_NODE_AUTHENTICATION_FAILURE
 * \li 68  - MIP_FA_ERR_HA_AUTHENTICATION_FAILURE
 * \li 69  - MIP_FA_ERR_REQUESTED_LIFETIME_TOO_LONG
 * \li 70  - MIP_FA_ERR_MALFORMED_REQUEST
 * \li 71  - MIP_FA_ERR_MALFORMED_REPLY
 * \li 72  - MIP_FA_ERR_ENCAPSULATION_UNAVAILABLE
 * \li 73  - MIP_FA_ERR_VJHC_UNAVAILABLE
 * \li 74  - MIP_FA_ERR_REVERSE_TUNNEL_UNAVAILABLE
 * \li 75  - MIP_FA_ERR_REVERSE_TUNNEL_IS_MANDATORY_AND_T_BIT_NOT_SET
 * \li 79  - MIP_FA_ERR_DELIVERY_STYLE_NOT_SUPPORTED
 * \li 97  - MIP_FA_ERR_MISSING_NAI
 * \li 98  - MIP_FA_ERR_MISSING_HA
 * \li 99  - MIP_FA_ERR_MISSING_HOME_ADDR
 * \li 104 - MIP_FA_ERR_UNKNOWN_CHALLENGE
 * \li 105 - MIP_FA_ERR_MISSING_CHALLENGE
 * \li 106 - MIP_FA_ERR_STALE_CHALLENGE
 * \li 128 - MIP_HA_ERR_REASON_UNSPECIFIED
 * \li 129 - MIP_HA_ERR_ADMINISTRATIVELY_PROHIBITED
 * \li 130 - MIP_HA_ERR_INSUFFICIENT_RESOURCES
 * \li 131 - MIP_HA_ERR_MOBILE_NODE_AUTHENTICATION_FAILURE
 * \li 132 - MIP_HA_ERR_FA_AUTHENTICATION_FAILURE
 * \li 133 - MIP_HA_ERR_REGISTRATION_ID_MISMATCH
 * \li 134 - MIP_HA_ERR_MALFORMED_REQUEST
 * \li 136 - MIP_HA_ERR_UNKNOWN_HA_ADDR
 * \li 137 - MIP_HA_ERR_REVERSE_TUNNEL_UNAVAILABLE
 * \li 138 - MIP_HA_ERR_REVERSE_TUNNEL_IS_MANDATORY_AND_T_BIT_NOT_SET
 * \li 139 - MIP_HA_ERR_ENCAPSULATION_UNAVAILABLE
 * \li 65536 - MIP_ERR_REASON_UNKNOWN
 * \subsection Internal Internal call end reasons (Type=2)
 * \li 200 - INTERNAL_MIN, internal error table offset value, no meaningful message to the error.
 * \li 201 - INTERNAL_ERROR
 * \li 202 - CALL_ENDED
 * \li 203 - INTERNAL_UNKNOWN_CAUSE_CODE
 * \li 204 - UNKNOWN_CAUSE_CODE
 * \li 205 - CLOSE_IN_PROGRESS
 * \li 206 - NW_INITIATED_TERMINATION
 * \li 207 - APP_PREEMPTED
 * \li 208 - ERR_PDN_IPV4_CALL_DISALLOWED
 * \li 209 - ERR_PDN_IPV4_CALL_THROTTLED
 * \li 210 - ERR_PDN_IPV6_CALL_DISALLOWED
 * \li 211 - ERR_PDN_IPV6_CALL_THROTTLED
 * \li 212 - MODEM_RESTART
 * \li 213 - PDP_PPP_NOT_SUPPORTED
 * \li 214 - UNPREFERRED_RAT
 * \li 215 - PHYS_LINK_CLOSE_IN_PROGRESS
 * \li 216 - APN_PENDING_HANDOVER
 * \li 217 - PROFILE_BEARER_INCOMPATIBLE
 * \li 218 - MMGSDI_CARD_EVT
 * \li 219 - LPM_OR_PWR_DOWN
 * \li 220 - APN_DISABLED
 * \li 221 - MPIT_EXPIRED
 * \li 222 - IPV6_ADDR_TRANSFER_FAILED
 * \li 223 - TRAT_SWAP_FAILED
 * \subsection CM Call Manager defined call end reasons (Type=3)
 * \li 500 - CDMA_LOCK
 * \li 501 - INTERCEPT
 * \li 502 - REORDER
 * \li 503 - REL_SO_REJ
 * \li 504 - INCOM_CALL
 * \li 505 - ALERT_STOP
 * \li 506 - ACTIVATION
 * \li 507 - MAX_ACCESS_PROBE
 * \li 508 - CCS_NOT_SUPPORTED_BY_BS
 * \li 509 - NO_RESPONSE_FROM_BS
 * \li 510 - REJECTED_BY_BS
 * \li 511 - INCOMPATIBLE
 * \li 512 - ALREADY_IN_TC
 * \li 513 - USER_CAL_ORIG_DURING_GPS
 * \li 514 - USER_CAL_ORIG_DURING_SMS
 * \li 515 - NO_CDMA_SRV
 * \li 516 - MC_ABORT
 * \li 517 - PSIST_NG
 * \li 518 - UIM_NOT_PRESENT
 * \li 519 - RETRY_ORDER
 * \li 520 - ACCESS_BLOCK
 * \li 521 - ACCESS_BLOCK_ALL
 * \li 522 - IS707B_MAX_ACC
 * \li 523 - THERMAL_EMERGENCY
 * \li 524 - CALL_ORIG_THROTTLED
 * \li 525 - USER CALL ORIGINATE DURING VOICE CALL
 * \li 1000 - CONF_FAILED
 * \li 1001 - INCOM_REJ
 * \li 1002 - NO_GW_SRV
 * \li 1003 - NO_GPRS_CONTEXT
 * \li 1004 - ILLEGAL_MS, This cause is sent to the MS when the network refuses service to the MS either because an identity of the MS is
 *            not acceptable to the network or because the MS does not pass the authentication check, i.e. the SRES received from the MS
 *            is different from that generated by the network
 * \li 1005 - ILLEGAL_ME, This cause is sent to the MS if the ME used is not acceptable to the network, e.g. blacklisted
 * \li 1006 - GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED
 * \li 1007 - GPRS_SERVICES_NOT_ALLOWED
 * \li 1008 - MS_IDENTITY_CANNOT_BE_DERVIVED_BY_THE_NETWORK
 * \li 1009 - IMPLICITLY_DETACHED
 * \li 1010 - PLMN_NOT_ALLOWED
 * \li 1011 - LA_NOT_ALLOWED
 * \li 1012 - GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN
 * \li 1013 - PDP_DUPLICATE
 * \li 1014 - UE_RAT_CHANGE
 * \li 1015 - CONGESTION
 * \li 1016 - NO_PDP_CONEXT_ACTIVATED
 * \li 1017 - ACCESS_CLASS_DSAC_REJECTION
 * \li 1018 - PDP_ACTIVATE_MAX_RETRY_FAILED
 * \li 1019 - RAB_FAILURE
 * \li 1020 - EPS_SERVICE_NOT_ALLOWED
 * \li 1021 - TRACKING_AREA_NOT_ALLOWED
 * \li 1022 - ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA
 * \li 1023 - NO_SUITABLE_CELLS_IN_TRACKING_AREA
 * \li 1024 - NOT_AUTHORIZED_FOR_THIS_CLOSED_SUBSCRIBER_GROUP
 * \li 1025 - ESM_UNKNOWN_EPS_BEARER_CONTEXT
 * \li 1026 - DRB_RELEASED_AT_RRC
 * \li 1027 - NAS_SIG_CONN_RELEASED
 * \li 1028 - EPS_MOBILITY_MANAGEMENT_DETACHED
 * \li 1029 - EPS_MOBILITY_MANAGEMENT_ATTACH_FAILED
 * \li 1030 - EPS_MOBILITY_MANAGEMENT_ATTACH_STARTED
 * \li 1031 - LTE_NAS_SERVICE_REQ_FAILED
 * \li 1032 - ESM_ACTIVE_DEDICATED_BEARER_REACTIVATED_BY_NW
 * \li 1033 - ESM_LOWER_LAYER_FAILURE
 * \li 1034 - ESM_SYNC_UP_WITH_NW
 * \li 1035 - ESM_NW_ACTIVATED_DED_BEARER_WITH_ID_OF_DEF_BEARER
 * \li 1036 - ESM_BAD_OTA_MESSAGE
 * \li 1037 - ESM_DS_REJECTED_THE_CALL
 * \li 1038 - ESM_CONTEXT_TRANSFERED_DUE_TO_IRAT
 * \li 1039 - DS_EXPLICIT_DEACT
 * \li 1040 - ESM_LOCAL_CAUSE_NONE
 * \li 1041 - LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE
 * \li 1042 - ACL_FAILURE, This error code should rarely triggered and reported to the application
 * \li 1043 - LTE_NAS_SERVICE_REQ_FAILED_DS_DISALLOW
 * \li 1044 - EMM_T3417_EXPIRED
 * \li 1045 - EMM_T3417_EXT_EXPIRED
 * \li 1046 - LRRC_UL_DATA_CNF_FAILURE_TXN - Light Radio Resource Controller Uplink data confirmation failure
 * \li 1047 - LRRC_UL_DATA_CNF_FAILURE_HO
 * \li 1048 - LRRC_UL_DATA_CNF_FAILURE_CONN_REL
 * \li 1049 - LRRC_UL_DATA_CNF_FAILURE_RLF
 * \li 1050 - LRRC_UL_DATA_CNF_FAILURE_CTRL_NOT_CONN
 * \li 1051 - LRRC_CONN_EST_FAILURE
 * \li 1052 - LRRC_CONN_EST_FAILURE_ABORTED
 * \li 1053 - LRRC_CONN_EST_FAILURE_ACCESS_BARRED
 * \li 1054 - LRRC_CONN_EST_FAILURE_CELL_RESEL
 * \li 1055 - LRRC_CONN_EST_FAILURE_CONFIG_FAILURE
 * \li 1056 - LRRC_CONN_EST_FAILURE_TIMER_EXPIRED
 * \li 1057 - LRRC_CONN_EST_FAILURE_LINK_FAILUR
 * \li 1058 - LRRC_CONN_EST_FAILURE_NOT_CAMPED
 * \li 1059 - LRRC_CONN_EST_FAILURE_SI_FAILURE
 * \li 1060 - LRRC_CONN_EST_FAILURE_CONN_REJECT
 * \li 1061 - LRRC_CONN_REL_NORMAL
 * \li 1062 - LRRC_CONN_REL_RLF
 * \li 1063 - LRRC_CONN_REL_CRE_FAILURE
 * \li 1064 - LRRC_CONN_REL_OOS_DURING_CRE
 * \li 1065 - LRRC_CONN_REL_ABORTED
 * \li 1066 - LRRC_CONN_REL_SIB_READ_ERROR
 * \li 1067 - DETACH_WITH_REATTACH_LTE_NW_DETACH
 * \li 1068 - DETACH_WITH_OUT_REATTACH_LTE_NW_DETACH
 * \li 1069 - ESM_PROC_TIME_OUT
 * \li 1070 - MESSAGE_EXCEED_MAX_L2_LIMIT
 * \li 1500 - CD_GEN_OR_BUSY
 * \li 1501 - CD_BILL_OR_AUTH
 * \li 1502 - CHG_HDR
 * \li 1503 - EXIT_HDR
 * \li 1504 - HDR_NO_SESSION
 * \li 1505 - HDR_ORIG_DURING_GPS_FIX
 * \li 1506 - HDR_CS_TIMEOUT
 * \li 1507 - HDR_RELEASED_BY_CM
 * \li 1508 - COLLOC_ACQ_FAIL
 * \li 1509 - OTASP_COMMIT_IN_PROG
 * \li 1510 - NO_HYBR_HDR_SRV
 * \li 1511 - HDR_NO_LOCK_GRANTED
 * \li 1512 - HOLD_OTHER_IN_PROG
 * \li 1513 - HDR_FADE
 * \li 1514 - HDR_ACC_FAIL
 * \li 2000 - CLIENT_END
 * \li 2001 - NO_SRV
 * \li 2002 - FADE
 * \li 2003 - REL_NORMAL
 * \li 2004 - ACC_IN_PROG
 * \li 2005 - ACC_FAIL
 * \li 2006 - REDIR_OR_HANDOFF
 * \li 2500 - OFFLINE
 * \li 2501 - EMERGENCY_MODE
 * \li 2502 - PHONE_IN_USE
 * \li 2503 - INVALID_MODE
 * \li 2504 - INVALID_SIM_STATE
 * \li 2505 - NO_COLLOC_HDR
 * \li 2506 - CALL_CONTROL_REJECTED
 * \subsection _3GPP 3GPP specification defined call end reasons (Type=6)
 * \li 8 - OPERATOR_DETERMINED_BARRING
 * \li 25 - LLC_SNDCP_FAILURE, PDP context deactivation initiated by the MS or by the Network
 * \li 26 - INSUFFICIENT_RESOURCES, PDP context deactivation initiated by the MS
 * \li 27 - UNKNOWN_APN, Unsuccessful MBMS context activation requested by the MS
 * \li 28 - UNKNOWN_PDP
 * \li 29 - AUTH_FAILED, Unsuccessful MBMS context activation requested by the MS
 * \li 30 - GGSN_REJECT, Unsuccessful MBMS context activation requested by the MS
 * \li 31 - ACTIVATION_REJECT, Unsuccessful MBMS context activation requested by the MS
 * \li 32 - OPTION_NOT_SUPPORTED
 * \li 33 - OPTION_UNSUBSCRIBED, This cause is sent when the MS requests a service option for which it has no subscription
 * \li 34 - OPTION_TEMP_OOO, service option temporarily out of order
 * \li 35 - NSAPI_ALREADY_USED
 * \li 36 - REGULAR_DEACTIVATION
 * \li 37 - QOS_NOT_ACCEPTED
 * \li 38 - NETWORK_FAILURE, This cause is sent to the MS if the MSC cannot service an MS generated request because of PLMN failures, e.g.
 *          problems in MAP
 * \li 39 - UMTS_REACTIVATION_REQ
 * \li 40 - FEATURE_NOT_SUPPORTED, Unsuccessful MBMS context activation requested by the network
 * \li 41 - TFT_SEMANTIC_ERROR
 * \li 42 - TFT_SYNTAX_ERROR
 * \li 43 - UNKNOWN_PDP_CONTEXT
 * \li 44 - FILTER_SEMANTIC_ERROR
 * \li 45 - FILTER_SYNTAX_ERROR
 * \li 46 - PDP_WITHOUT_ACTIVE_TFT
 * \li 50 - IPV4 ONLY ALLOWED
 * \li 51 - IPV6 ONLY ALLOWED
 * \li 52 - SINGLE ADDRESS BEARER ONLY
 * \li 53 - ESM INFORMATION NOT RECEIVED 
 * \li 54 - PND CONNECTION DOES NOT EXIST
 * \li 55 - MULTIPLE CONNECTION TO SAME PDN NOT ALLOWED
 * \li 81 - INVALID_TRANSACTION_ID
 * \li 95 - MESSAGE_INCORRECT_SEMANTIC
 * \li 96 - INVALID_MANDATORY_INFO
 * \li 97 - MESSAGE_TYPE_UNSUPPORTED
 * \li 98 - MSG_TYPE_NONCOMPATIBLE_STATE
 * \li 99 - UNKNOWN_INFO_ELEMENT
 * \li 100 - CONDITIONAL_IE_ERROR
 * \li 101 - MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE
 * \li 111 - PROTOCOL_ERROR
 * \li 112 - APN_TYPE_CONFLICT
 * \li 113 - INVALID PROXY-CALL SESSION CONTROL FUNCTION ADDRESS
 * \subsection PPP PPP call end reasons (Type=7)
 * \li 1 - TIMEOUT
 * \li 2 - AUTH_FAILURE
 * \li 3 - OPTION_MISMATCH
 * \li 31 - PAP_FAILURE
 * \li 32 - CHAP_FAILURE
 * \li 33 - CLOSE_IN_PROGRESS
 * \li -1 - UNKNOWN
 * \subsection EHRPD EHRPD call end reasons (Type=8)
 * \li 1 - SUBS_LIMITED_TO_V4
 * \li 2 - SUBS_LIMITED_TO_V6
 * \li 4 - VSNCP_TIMEOUT
 * \li 5 - VSNCP_FAILURE
 * \li 6 - VSNCP_3GPP2I_GEN_ERROR
 * \li 7 - VSNCP_3GPP2I_UNAUTH_APN
 * \li 8 - VSNCP_3GPP2I_PDN_LIMIT_EXCEED
 * \li 9 - VSNCP_3GPP2I_NO_PDN_GW
 * \li 10 - VSNCP_3GPP2I_PDN_GW_UNREACH
 * \li 11 - VSNCP_3GPP2I_PDN_GW_REJ
 * \li 12 - VSNCP_3GPP2I_INSUFF_PARAM
 * \li 13 - VSNCP_3GPP2I_RESOURCE_UNAVAIL
 * \li 14 - VSNCP_3GPP2I_ADMIN_PROHIBIT
 * \li 15 - VSNCP_3GPP2I_PDN_ID_IN_USE
 * \li 16 - VSNCP_3GPP2I_SUBSCR_LIMITATION
 * \li 17 - VSNCP_3GPP2I_PDN_EXISTS_FOR_THIS_APN
 * \subsection IPV6 IPV6 call end reasons (Type=9)
 * \li 1 - PREFIX_UNAVAILABLE
 * \li 2 - IPV6_ERR_HRPD_IPV6_DISABLED
 *
 * Copyright: © 2011-2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __GOBI_API_CALL_END_REASONS_H__
#define __GOBI_API_CALL_END_REASONS_H__


#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __GOBI_API_CALL_END_REASONS_H__ */

