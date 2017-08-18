/**
 *    \if CvsId 
 *    $Id: SwiApiCmDm.h,v 1.4 2010/06/10 00:15:17 epasheva Exp $
 *    \endif
 *    \ingroup cmdm
 *    \file SwiApiCmDm.h
 * 
 *    \brief SWI OMA-DM related APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#ifndef __SWIAPICMDM_H__
#define __SWIAPICMDM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* include files */
#include "SwiDataTypes.h"
#include "SwiRcodes.h"



#define MAX_DL_PKG_NAME         232     /*!< Max length of DL name */
#define MAX_DL_DESCRIPTION      1024    /*!< Max length of DL description */

/* OMA-DM Types and Structures */

/**
 *
 * An enumeration of device OMA-DM capability.
 *
 */
typedef enum SWI_TYPE_OMADM_DEVICE_CAPABILITY
{
    OMADM_DEVICE_CAPABILITY_NONE    = 0x00,/*!< OMA-DM is not supported */
    
    /*! OMA-DM is supported via Sierra Wireless */
    OMADM_DEVICE_CAPABILITY_SWI     = 0x01,

    /*! OMA-DM is supported via Red Bend */
    OMADM_DEVICE_CAPABILITY_REDBEND = 0x02
}SWI_TYPE_OMADM_DEVICE_CAPABILITY;

/**
 *
 * An enumeration of device OMA-DM feature capability.
 *
 */
typedef enum SWI_TYPE_OMADM_FEATURE_CAPABILITY
{
    OMADM_FEATURE_CIDC   = 0x00000001,  /*!< Client initiated device config */
    OMADM_FEATURE_NIDC   = 0x00000002,  /*!< Network initiated device config */
    OMADM_FEATURE_CIPRL  = 0x00000004,  /*!< Client initiated PRL update */
    OMADM_FEATURE_NIPRL  = 0x00000008,  /*!< Network initiated PRL update */
    
    /*! Client initiated Firmware Update (FUMO) */
    OMADM_FEATURE_CIFUMO = 0x00000010,
    
    /*! Network initiated Firmware Update (FUMO) */
    OMADM_FEATURE_NIFUMO = 0x00000020,

    OMADM_FEATURE_HFA    = 0x00000040   /*!< hands free activation (HFA) */
}SWI_TYPE_OMADM_FEATURE_CAPABILITY;                                            

/**
 *
 * Contains device OMA-DM configuration data.
 *
 */
typedef struct SWI_STRUCT_OMADM_GET_CONFIG
{
    swi_uint32 sizeStruct;      /*!< size of this structure */
    
    /*! Device OMA-DM capability */
    SWI_TYPE_OMADM_DEVICE_CAPABILITY nOMADMCapability;
    
    /*! Max number of account supported (Valid Number: 1 - 8) */
    swi_uint8 nMaxAcctSupport;
    
    /*! Current number of account supported (Valid Number: 1 - 8) */
    swi_uint8 nNumAccts;

    /*! Used to notify if acount support/number changes */
    swi_uint8 bAcctChanged;

    /*! What features are supported (SWI_TYPE_OMADM_FEATURE_CAPABILITY) */
    swi_uint32 dwFeatureCapabilityMask;

    /*! Feature enable state (SWI_TYPE_OMADM_FEATURE_CAPABILITY) */
    swi_uint32 dwFeatureEnableMask;

    /*! TRUE if HFA has been attempted (only if HFA supported) */
    swi_uint8 bHFAAttempted;
}SWI_STRUCT_OMADM_GET_CONFIG;

/**
 *
 * An enumeration of OMA-DM status response.
 *
 */
typedef enum SWI_TYPE_OMADM_STATUS_RESP
{
    OMADM_STATUS_RESP_SUCCESS                = 0x00,    /*!< Success */
    OMADM_STATUS_RESP_FAIL                   = 0x01,    /*!< Fail */
    OMADM_STATUS_RESP_UNSUPPORTED            = 0x02,    /*!< Unsupported */
    OMADM_STATUS_RESP_BAD_PARAM              = 0x03,    /*!< Bad parameter */
    OMADM_STATUS_RESP_INVALID_OPTION         = 0x04,    /*!< Invalid option */
    
    /*! Security access denied */
    OMADM_STATUS_RESP_SECURITY_ACCESS_DENIED = 0x05,
    
    /*! Invalid service state */
    OMADM_STATUS_RESP_INVALID_DEVICE_STATE   = 0x06,

    /*! Invalid session state */
    OMADM_STATUS_RESP_INVALID_SESSION_STATE  = 0x07,
    
    /*! Unsupported node */
    OMADM_STATUS_RESP_UNSUPPORTED_NODE       = 0x08,
    
    /*! Node alrady exists */
    OMADM_STATUS_RESP_NODE_ALREADY_EXISTS    = 0x09,

    /*! Exceed URI length limit */
    OMADM_STATUS_RESP_MAX_URI_EXCEEDED       = 0x0A
}SWI_TYPE_OMADM_STATUS_RESP;

/**
 *
 * Contains data for setting OMA-DM configuration.
 *
 * Note: Reset HFA requires MSL unlock.
 *
 */
typedef struct SWI_STRUCT_OMADM_SET_CONFIG
{
  swi_uint32 sizeStruct;    /*!< size of this structure */

  /*! what features are supported (SWI_TYPE_OMADM_FEATURE_CAPABILITY) */
  swi_uint32 dwFeatureCapabilityMask;

  /*! feature enable state (SWI_TYPE_OMADM_FEATURE_CAPABILITY) */
  swi_uint32 dwFeatureEnableMask;

  /*! Set TRUE to reset HFA flag (reattempt HFA upon next power cycle) */
  swi_uint8 bResetHFA;
}SWI_STRUCT_OMADM_SET_CONFIG;

/**
 *
 * An enumeration of OMA-DM session type.
 *
 */
typedef enum SWI_TYPE_OMADM_SESSION_TYPE
{
    OMADM_SESSION_TYPE_UNKNOWN              = 0x00, /*!< Unknown */
    OMADM_SESSION_TYPE_DEVICE_CONFIG        = 0x01, /*!< Device config */
    OMADM_SESSION_TYPE_CDMA_PRL_UPDATE      = 0x02, /*!< PRL update */

    /*! FUMO Information Exchanged */
    OMADM_SESSION_TYPE_FUMO_INFO_EXCHANGE   = 0x03,

    /*! Hand free actiovation */
    OMADM_SESSION_TYPE_HFA                  = 0x04,

    /*! Download Descriptor Download */
    OMADM_SESSION_TYPE_DD_DOWNLOAD          = 0x05,

    /*! FUMO Package Download */
    OMADM_SESSION_TYPE_PKG_DOWNLOAD         = 0x06,

    /*! Status update to OMADM Server */
    OMADM_SESSION_TYPE_STATUS_REPORT        = 0x07
}SWI_TYPE_OMADM_SESSION_TYPE;

/**
 *
 * An enumeration of OMA-DM client initiated session type.
 *
 */
typedef enum SWI_TYPE_OMADM_USER_CI_SESSION_TYPE
{
    OMADM_USER_CI_SESSION_TYPE_UNKNOWN         = 0x00,  /*!< Unknown */

    /*! Device Configuration */
    OMADM_USER_CI_SESSION_TYPE_DEVICE_CONFIG   = 0x01,

    OMADM_USER_CI_SESSION_TYPE_CDMA_PRL_UPDATE = 0x02,  /*!< CDMA PRL Update */
    OMADM_USER_CI_SESSION_TYPE_FUMO            = 0x03   /*!< FUMO */
}SWI_TYPE_OMADM_USER_CI_SESSION_TYPE;

/**
 *
 * Contains data for starting OMA-DM session.
 *
 */
typedef struct SWI_STRUCT_OMADM_START_SESSION 
{
    swi_uint32 sizeStruct;    /*!< size of this structure */

    /*! Type of session to start */   
    SWI_TYPE_OMADM_USER_CI_SESSION_TYPE nType;

    /*! Account name length (0 if default) */
    swi_uint16 nAcctNameLen;

    /*! Variable length Account name (not NULL terminated) */
    swi_uint8* psAcctName;
}SWI_STRUCT_OMADM_START_SESSION;

/**
 *
 * An enumeration of OMA-DM cancel status.
 *
 */
typedef enum SWI_TYPE_OMADM_CANCEL_STATUS
{
    OMADM_CANCEL_SUCCESS    = 0x00, /*!< Succeess */
    OMADM_CANCEL_NO_SESSION = 0x01  /*!< No active session */
}SWI_TYPE_OMADM_CANCEL_STATUS;

/**
 *
 * Contains OMA-DM session cancelation respond data.
 *
 */
typedef struct SWI_STRUCT_OMADM_CANCEL_RESP
{
  swi_uint32 sizeStruct;    /*!< size of this structure */
  SWI_TYPE_OMADM_CANCEL_STATUS nStatus; /*!< Cancel status */
  swi_uint8 bQueuedForReattempt;        /*!< A reattempt is queued */
}SWI_STRUCT_OMADM_CANCEL_RESP;

/**
 *
 * An enumeration of OMA-DM session state.
 *
 */
typedef enum SWI_TYPE_OMADM_SESSION_STATE
{
    OMADM_SESSION_STATE_IDLE        = 0x00, /*!< No OMA-DM session */
    
    /*! Active OMA-DM session exists */
    OMADM_SESSION_STATE_ACTIVE      = 0x01,

    OMADM_SESSION_STATE_HFA_PENDING = 0x02  /*!< HFA pending */
}SWI_TYPE_OMADM_SESSION_STATE;

/**
 *
 * An enumeration of OMA-DM session initiator.
 *
 */
typedef enum SWI_TYPE_OMADM_SESSION_INITIATOR
{
    OMADM_SESSION_INITIATOR_UNKNOWN         = 0x00, /*!< Unknown */
    
    /*! User client initiated */
    OMADM_SESSION_INITIATOR_USER_CLIENT     = 0x01,

    /*! Device client initiated */
    OMADM_SESSION_INITIATOR_DEVICE_CLIENT   = 0x02,

    OMADM_SESSION_INITIATOR_NETWORK         = 0x03  /*!< Network initiated */
}SWI_TYPE_OMADM_SESSION_INITIATOR;

/**
 *
 * An enumeration of OMA-DM UI mode.
 *
 */
typedef enum SWI_TYPE_OMADM_SESSION_UI_MODE
{
    OMADM_SESSION_UI_MODE_UNKNOWN  = 0x00,  /*!< Unknown */
    OMADM_SESSION_UI_MODE_HIDDEN   = 0x01,  /*!< Hidden */
    OMADM_SESSION_UI_MODE_UNHIDDEN = 0x02   /*!< Unhidden */
}SWI_TYPE_OMADM_SESSION_UI_MODE;

/**
 *
 * An enumeration of OMA-DM session results.
 *
 */
typedef enum SWI_TYPE_OMADM_SESSION_RESULT
{
    OMADM_SESSION_RESULT_SUCCESS                = 0x0000,   /*!< Success */
    OMADM_SESSION_RESULT_FAIL                   = 0x0001,   /*!< Fail */
    
    /*! No command recieved */
    OMADM_SESSION_RESULT_NO_COMMANDS_RECEIVED   = 0x0002,
    
    OMADM_SESSION_RESULT_ABORT_CLIENT_USER      = 0x0100,   /*!< User abort */
    
    /*! Device abort */
    OMADM_SESSION_RESULT_ABORT_CLIENT_DEVICE    = 0x0101,

    /*! Server abort */
    OMADM_SESSION_RESULT_ABORT_SERVER           = 0x0102,

    /*! Memory error */
    OMADM_SESSION_RESULT_MEMORY_ERROR           = 0x0200,

    /*! Heap too small */
    OMADM_SESSION_RESULT_HEAP_TOO_SMALL         = 0x0201,

    /*! Buffer too small */
    OMADM_SESSION_RESULT_BUFFER_TOO_SMALL       = 0x0202,

    /*! Routine not allowed */
    OMADM_SESSION_RESULT_ROUTINE_NOT_ALLOWED    = 0x0300,

    /*! Routine suspend */
    OMADM_SESSION_RESULT_ROUTINE_WHEN_SUSPEND   = 0x0301,

    /*! Bad input parameter */
    OMADM_SESSION_RESULT_BAD_INPUT_PARAM        = 0x0302,

    /*! Invalid protocol */
    OMADM_SESSION_RESULT_INVALID_PROTOCOL       = 0x0400,

    /*! Authentication failure */
    OMADM_SESSION_RESULT_AUTH_FAILURE           = 0x0401,

    /*! Missing start message */
    OMADM_SESSION_RESULT_MISSING_START_MSG      = 0x0402,

    /*! Missing ending message */
    OMADM_SESSION_RESULT_MISSING_END_MSG        = 0x0403,

    /*! Feature not available */
    OMADM_SESSION_RESULT_FEATURE_NOT_AVAIL      = 0x0404,

    /*! UIA parse error */
    OMADM_SESSION_RESULT_UIA_PARSE_ERROR        = 0x0405,

    /*! Data too big to RTN */
    OMADM_SESSION_RESULT_DATA_TOO_BIG_TO_RTN    = 0x0406,

    /*! Unsupported protocol */
    OMADM_SESSION_RESULT_UNSUPPORTED_PROTOCOL   = 0x0500,

    /*! MIME type mismatch */
    OMADM_SESSION_RESULT_MIME_TYPE_MISMATCH     = 0x0501,

    /*! NTWK faile to open */
    OMADM_SESSION_RESULT_NTWK_FAIL_TO_OPEN      = 0x0502,

    /*! SSL faile to open */
    OMADM_SESSION_RESULT_SSL_FAIL_TO_OPEN       = 0x0503,

    /*! Fatal network error */
    OMADM_SESSION_RESULT_FATAL_NTWK_ERROR       = 0x0504,

    /*! Tree node exists */
    OMADM_SESSION_RESULT_TREE_NODE_EXISTS       = 0x1000,

    /*! Tree node missing */
    OMADM_SESSION_RESULT_TREE_NODE_MISSING      = 0x1001,

    /*! Leaf node operation error */
    OMADM_SESSION_RESULT_LEAF_NODE_OP_ERROR     = 0x1002,

    /*! Invalid node type */
    OMADM_SESSION_RESULT_INVALID_NODE_TYPE      = 0x1003,

    /*! Unknown property */
    OMADM_SESSION_RESULT_UNKNOWN_PROPERTY       = 0x1004,
    
    /*! Delete perm node */
    OMADM_SESSION_RESULT_DELETE_PERM_NODE       = 0x1005,

    /*! Access type invalid */
    OMADM_SESSION_RESULT_ACCESS_TYPE_INVALID    = 0x1006,

    /*! Bad name for tree */
    OMADM_SESSION_RESULT_BAD_NAME_FOR_TREE      = 0x1007,

    /*! Partial write ext data */
    OMADM_SESSION_RESULT_PARTIAL_WRITE_EXT_DATA = 0x1008,

    /*! Wxt write temp blocked */
    OMADM_SESSION_RESULT_EXT_WRITE_TEMP_BLOCKED = 0x1009,

    /*! Value not writable */
    OMADM_SESSION_RESULT_VALUE_NOT_WRITABLE     = 0x100A,

    /*! Value not readable */
    OMADM_SESSION_RESULT_VALUE_NOT_READABLE     = 0x100B,

    /*! Cannot execute node */
    OMADM_SESSION_RESULT_CANNOT_EXECUTE_NODE    = 0x100C,

    /*! Tree open error */
    OMADM_SESSION_RESULT_TREE_OPEN_ERROR        = 0x100D,

    /*! Tree commit error */
    OMADM_SESSION_RESULT_TREE_COMMIT_ERROR      = 0x100E,

    /*! Bootstrap reserved */
    OMADM_SESSION_RESULT_BOOTSTRAP_RSVD         = 0x2000,

    /*! FUMO reserved */
    OMADM_SESSION_RESULT_FUMO_RSVD              = 0x3000
}SWI_TYPE_OMADM_SESSION_RESULT;

/**
 *
 * An enumeration of OMA-DM session commit mask.
 *
 * The commit results have specific meanings for each session type:
 * CIDC/NIDC/HFA                : DC not updated(0x00)/updated(0x01)
 * CIPRL/NIPRL                  : PRL not updated(0x00)/updated(0x01)
 * CIFUMO/NIFUMO Info Exchange  : Pkg not available(0x00)/Pkg available(0x01)
 * CI/NI DD Download/Package Download/Status Report: N/A
 *
 */
typedef enum SWI_TYPE_OMADM_SESSION_COMMIT
{
    OMADM_SESSION_COMMIT_DATA_NOT_UPDATED = 0x00,   /*!< Updated */
    OMADM_SESSION_COMMIT_DATA_UPDATED     = 0x01    /*!< Not updated */
}SWI_TYPE_OMADM_SESSION_COMMIT;

/**
 *
 * Contains OMA-DM session state data.
 *
 *
 */
typedef struct SWI_STRUCT_OMADM_SESSION_STATE
{
    swi_uint32 sizeStruct;      /*!< size of this structure */
    SWI_TYPE_OMADM_SESSION_STATE nState;    /*!< Session state */
    swi_uint16 nHFARetryTime;           /*!< HFA retry time left in second */
    SWI_TYPE_OMADM_SESSION_TYPE nLastSessionType;   /*!< Last session type */
    
    /*! Last session Initiator */
    SWI_TYPE_OMADM_SESSION_INITIATOR nLastSessionInitiator;

    SWI_TYPE_OMADM_SESSION_UI_MODE nUIMode;     /*!< UI mode */
    
    /*! Last session result */
    SWI_TYPE_OMADM_SESSION_RESULT nLastSessionResult;

    /*! Last session commit mask */
    SWI_TYPE_OMADM_SESSION_COMMIT nLastSessionCommit;
}SWI_STRUCT_OMADM_SESSION_STATE;

/**
 *
 * An enumeration of OMA-DM NIA pending state.
 *
 */
 typedef enum SWI_TYPE_OMADM_NIA_PENDING
{
    OMADM_NIA_STATE_PENDING_NONE    = 0x00, /*!< No pending */
    OMADM_NIA_STATE_PENDING_BLOCKED = 0x01, /*!< Pending blocked */
    OMADM_NIA_STATE_PENDING_READY   = 0x02  /*!< Pending ready */
}SWI_TYPE_OMADM_NIA_PENDING;

/**
 *
 * An enumeration of OMA-DM NIA UI mode.
 *
 */
typedef enum SWI_TYPE_OMADM_NIA_UI_MODE
{
    OMADM_NIA_UI_MODE_UNKNOWN           = 0x00, /*!< Unknown */
    OMADM_NIA_UI_MODE_BACKGROUND        = 0x01,/*!< Background */
    OMADM_NIA_UI_MODE_INFORMATIVE       = 0x02,/*!< Informative */
    OMADM_NIA_UI_MODE_INTERACTIVE       = 0x03,/*!< Interactive */
    OMADM_NIA_UI_MODE_DORMANT_INTERRUPT = 0x04 /*!< Interrupt */
}SWI_TYPE_OMADM_NIA_UI_MODE;

/**
 *
 * An enumeration of OMA-DM NI alert session type.
 *
 */
typedef enum SWI_TYPE_OMADM_NIA_SESSION_TYPE
{
    OMADM_NIA_SESSION_TYPE_UNKNOWN         = 0x00, /*!< Unknown */
    OMADM_NIA_SESSION_TYPE_DEVICE_CONFIG   = 0x01, /*!< Device Configuration */
    OMADM_NIA_SESSION_TYPE_CDMA_PRL_UPDATE = 0x02, /*!< CDMA PRL Update */
    OMADM_NIA_SESSION_TYPE_FUMO            = 0x03  /*!< FUMO */
}SWI_TYPE_OMADM_NIA_SESSION_TYPE;

/**
 *
 * Contains OMA-DM NI alert data structure.
 *
 */
typedef struct SWI_STRUCT_OMADM_NI_ALERT
{
    swi_uint32 sizeStruct;                      /*!< Size of this structure */
    SWI_TYPE_OMADM_NIA_PENDING nPending;        /*!< NIA pending state */
    SWI_TYPE_OMADM_NIA_UI_MODE nUIMode;         /*!< NIA UI mode */
    SWI_TYPE_OMADM_NIA_UI_MODE nForceUIMode;    /*!< NIA force UI mode */
    SWI_TYPE_OMADM_NIA_SESSION_TYPE nType;      /*!< NIA session type */
}SWI_STRUCT_OMADM_NI_ALERT;

/**
 *
 * An enumeration of OMA-DM NI alert response status.
 *
 */
typedef enum SWI_TYPE_OMADM_NI_ALERT_RESP_STATUS
{
    OMADM_NI_ALERT_RESP_STATUS_ACCEPTED   = 0x00,   /*!< Accepted */
    OMADM_NI_ALERT_RESP_STATUS_UNEXPECTED = 0x01,   /*!< Unexpected */
    OMADM_NI_ALERT_RESP_STATUS_INVALID    = 0x02    /*!< Invalid */
}SWI_TYPE_OMADM_NI_ALERT_RESP_STATUS;

/**
 *
 * Contains OMA-DM NI alert response data structure.
 *
 */
typedef struct SWI_STRUCT_OMADM_NI_ALERT_RESP
{
    swi_uint32 sizeStruct;                      /*!< Size of this structure */
    SWI_TYPE_OMADM_NI_ALERT_RESP_STATUS nStatus;    /*!< NIA repsonse status */
    
    /*! Indicate NIA is queued for re-attempt if not 0. */
    swi_uint8 bQueuedForReattempt;
}SWI_STRUCT_OMADM_NI_ALERT_RESP;

/**
 *
 * Contains OMA-DM UI alert display data structure.
 *
 */
typedef struct SWI_STRUCT_OMADM_UI_ALERT_DISPLAY
{
    swi_uint32 sizeStruct;                      /*!< Size of this structure */

    /*! in seconds -- 0xFFFF indicates not specified */
    swi_uint16 nMinDisplayTime;

    /*! in seconds -- 0xFFFF indicates not specified */
    swi_uint16 nMaxDisplayTime;

    /*! length of text to display */
    swi_uint16 nTextLength;

    /*! Variable length text (not NULL terminated) */
    swi_uint8*  psText;
}SWI_STRUCT_OMADM_UI_ALERT_DISPLAY;

/**
 *
 * An enumeration of OMA-DM UI alert confirm defaule value.
 *
 */
typedef enum SWI_TYPE_OMADM_UI_ALERT_CONFIRM_DEFAULT
{
    OMADM_UI_ALERT_CONFIRM_REJECT      = 0x00,/*!< Reject */
    OMADM_UI_ALERT_CONFIRM_ACCEPT      = 0x01,/*!< Accept */
    OMADM_UI_ALERT_CONFIRM_UNSPECIFIED = 0xFF /*!< Unspecified */
}SWI_TYPE_OMADM_UI_ALERT_CONFIRM_DEFAULT;

/**
 *
 * Contains OMA-DM UI alert confirm data structure.
 *
 */
typedef struct SWI_STRUCT_OMADM_UI_ALERT_CONFIRM
{
    swi_uint32 sizeStruct;                  /*!< Size of this structure */

    /*! in seconds -- 0xFFFF indicates not specified */
    swi_uint16 nMinDisplayTime;

    /*! in seconds -- 0xFFFF indicates not specified */
    swi_uint16 nMaxDisplayTime;
    
    /*! NIA confirm defaule value */
    SWI_TYPE_OMADM_UI_ALERT_CONFIRM_DEFAULT nDefResp;
    
    swi_uint16 nTextLength;                 /*!< NIA alert text length */

    /*! Variable length text (not NULL terminated) */
    swi_uint8* psText;
}SWI_STRUCT_OMADM_UI_ALERT_CONFIRM;

/**
 *
 * An enumeration of OMA-DM UI alert user confirm type.
 *
 */
typedef enum SWI_TYPE_OMADM_UI_ALERT_USER_CONFIRM
{
    OMADM_UI_ALERT_USER_CONFIRM_USER   = 0x00,  /*!< Confirmed by user */
    OMADM_UI_ALERT_USER_CONFIRM_CANCEL = 0x01   /*!< Canceled by user */
}SWI_TYPE_OMADM_UI_ALERT_USER_CONFIRM;

/**
 *
 * An enumeration of OMA-DM UI alert user confirm response value.
 *
 */
typedef enum SWI_TYPE_OMADM_UI_ALERT_CONFIRM_RESP
{
    OMADM_UI_CONFIRM_RESP_REJECT  = 0x00,   /*!< Response is rejected */
    OMADM_UI_CONFIRM_RESP_CONFIRM = 0x01    /*!< Response is confirmed */
}SWI_TYPE_OMADM_UI_ALERT_CONFIRM_RESP;

/**
 *
 * Contains OMA-DM UI alert confirm data structure.
 *
 */
typedef struct SWI_STRUCT_OMADM_UI_ALERT_CONFIRM_RESP
{
    swi_uint32 sizeStruct;                  /*!< Size of this structure */
    
    SWI_TYPE_OMADM_UI_ALERT_USER_CONFIRM nUserAction;   /*!< User action */

    /*! Response to user action. 
        Ignored if nUserAction = OMADM_UI_ALERT_USER_CONFIRM_CANCEL */
    SWI_TYPE_OMADM_UI_ALERT_CONFIRM_RESP nResp;
}SWI_STRUCT_OMADM_UI_ALERT_CONFIRM_RESP;

/**
 *
 * An enumeration of OMA-DM UI alert input type.
 *
 */
typedef enum SWI_TYPE_OMADM_UI_ALERT_INPUT_TYPE
{
    OMADM_UI_ALERT_INPUT_TYPE_ALPHANUM  = 0x00, /*!< Alpha-numeric input */
    OMADM_UI_ALERT_INPUT_TYPE_NUMERIC   = 0x01, /*!< Numeric */
    OMADM_UI_ALERT_INPUT_TYPE_DATE      = 0x02, /*!< "DDMMYYYY" */
    OMADM_UI_ALERT_INPUT_TYPE_TIME      = 0x03, /*!< "hhmmss" */
    
    /*! Phone number input (All numeric characters, "+","p","w" and "s". "+" 
        must be first if present in phone number) */
    OMADM_UI_ALERT_INPUT_TYPE_PHONENUM  = 0x04,

    /*! IP address input www.xxx.yyy.zzz */
    OMADM_UI_ALERT_INPUT_TYPE_IP        = 0x05,

    OMADM_UI_ALERT_INPUT_TYPE_IPV6      = 0x06  /*!< IPV6 input */
}SWI_TYPE_OMADM_UI_ALERT_INPUT_TYPE;

/**
 *
 * An enumeration of OMA-DM UI alert echo type.
 *
 */
typedef enum SWI_TYPE_OMADM_UI_ALERT_ECHO_TYPE
{
    OMADM_UI_ALERT_ECHO_TYPE_TEXT       = 0x00, /*!< Text input (default) */

    /*! Password input (hide/mask user input text) */
    OMADM_UI_ALERT_ECHO_TYPE_PASSWORD   = 0x01
}SWI_TYPE_OMADM_UI_ALERT_ECHO_TYPE;

/**
 *
 * Contains OMA-DM UI alert input data structure.
 *
 */
typedef struct SWI_STRUCT_OMADM_UI_ALERT_INPUT
{
    swi_uint32 sizeStruct;                  /*!< Size of this structure */

    /*! Minimum display time (in seconds) -- 0xFFFF indicates not specified */
    swi_uint16 nMinDisplayTime;

    /*! Maximum display time (in seconds) -- 0xFFFF indicates not specified */
    swi_uint16 nMaxDisplayTime;

    /*! Maximum length of user input (in octets).
        0xFFFF indicates not specified */
    swi_uint16 nMaxLenUserInput;

    /*! Input type */
    SWI_TYPE_OMADM_UI_ALERT_INPUT_TYPE nInputType;

    /*! Echo type */
    SWI_TYPE_OMADM_UI_ALERT_ECHO_TYPE nEcho;

    /*! 0xFFFF indicates not specified */
    swi_uint16 nPromptLength;

    /*! Variable length text (includes Text 1st and prompt 2nd - 
        not NULL terminated) */
    swi_uint8* psText;
}SWI_STRUCT_OMADM_UI_ALERT_INPUT;

/**
 *
 * Contains OMA-DM UI alert input response data structure.
 *
 */
typedef struct SWI_STRUCT_OMADM_UI_ALERT_INPUT_RESP
{
    swi_uint32 sizeStruct;                  /*!< Size of this structure */

    /*! User action */
    SWI_TYPE_OMADM_UI_ALERT_USER_CONFIRM nUserAction;

    /*! nTextLength/strText ignored if 
        nUserAction = OMADM_UI_ALERT_USER_CONFIRM_CANCEL */
    swi_uint16 nTextLength;

    /*! Variable length text */
    swi_uint8* psText;
}SWI_STRUCT_OMADM_UI_ALERT_INPUT_RESP;

/**
 *
 * An enumeration of OMA-DM UI alert choice type.
 *
 */
typedef enum SWI_TYPE_OMADM_UI_ALERT_CHOICE_TYPE
{
    /*! User can only select a single choice */
    OMADM_UI_ALERT_CHOICE_SINGLE    = 0x00,

    /*! User allowed multiple selections */
    OMADM_UI_ALERT_CHOICE_MULTIPLE  = 0x01
}SWI_TYPE_OMADM_UI_ALERT_CHOICE_TYPE;

/**
 *
 * Contains OMA-DM UI alert choice data structure.
 *
 */
typedef struct SWI_STRUCT_OMADM_UI_ALERT_CHOICE
{
    swi_uint32 sizeStruct;                  /*!< Size of this structure */
    SWI_TYPE_OMADM_UI_ALERT_CHOICE_TYPE nChoiceType;    /*!< Choice type */
    swi_uint8 nNumChoices;      /*!< Number of choice */

    /*! in seconds -- 0xFFFF indicates not specified */
    swi_uint16 nMinDisplayTime;

    /*! in seconds -- 0xFFFF indicates not specified */
    swi_uint16  nMaxDisplayTime;

    swi_uint32 dwChoiceMask;        /*!< Indicates default choices */
    swi_uint16 nTextLength;         /*!< Text length */

    /*! Variable length text (not NULL terminated) */
    swi_uint8* psText;
}SWI_STRUCT_OMADM_UI_ALERT_CHOICE;

/**
 *
 * Contains OMA-DM UI alert choice response data structure.
 *
 */
typedef struct SWI_STRUCT_OMADM_UI_ALERT_CHOICE_RESP
{
    swi_uint32 sizeStruct;                  /*!< Size of this structure */
    SWI_TYPE_OMADM_UI_ALERT_USER_CONFIRM nUserAction;   /*!< User action */
    swi_uint32 dwChoiceMask;        /*!< Indicates user selected choice(s) */
}SWI_STRUCT_OMADM_UI_ALERT_CHOICE_RESP;

/**
 *
 * Contains OMA-DM node value request data structure.
 *
 */
typedef struct SWI_STRUCT_OMADM_NODE_VALUE_REQUEST
{
    swi_uint32 sizeStruct;                  /*!< Size of this structure */
    swi_uint16 nNodePathLength;             /*!< Node path length */
    swi_uint8* psText;      /*!< variable length text (not NULL terminated) */
}SWI_STRUCT_OMADM_NODE_VALUE_REQUEST;

/**
 *
 * Contains OMA-DM node value response data structure.
 *
 */
typedef struct SWI_STRUCT_OMADM_NODE_VALUE_RESPONSE
{
    swi_uint32 sizeStruct;                  /*!< Size of this structure */
    SWI_TYPE_OMADM_STATUS_RESP nStatus;     /*!< Response status */
    swi_uint16 nNodeFormatLength;           /*!< Node format length */
    swi_uint16 nNodeTypeLength;             /*!< Node type length */
    swi_uint16 nNodeValueLength;            /*!< Node value length */

    /*! Include Format, Type, Value strings (in that order - not NULL 
         terminated) */
    swi_uint8* psText;
}SWI_STRUCT_OMADM_NODE_VALUE_RESPONSE;

/**
 *
 * Contains OMA-DM node value data structure.
 *
 */
typedef struct SWI_STRUCT_OMADM_NODE_VALUE
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */
    swi_uint16 nNodePathLength;     /*!< Node path length */
    swi_uint16 nNodeFormatLength;   /*!< Node format length */
    swi_uint16 nNodeTypeLength;     /*!< Node type length */
    swi_uint16 nNodeValueLength;    /*!< Node value length */

    /*! Includes Path,Format,Type,Value strings (in that order - 
       not NULL terminated) */
    swi_uint8*    psText;
}SWI_STRUCT_OMADM_NODE_VALUE;

/**
 *
 * An enumeration of DM-DL progress type.
 *
 */
typedef enum SWI_TYPE_DM_DL_PROGRESS_TYPE
{
    DM_DL_PROGRESS_TYPE_NO_PKG   = 0x00,    /*!< No package */
    DM_DL_PROGRESS_TYPE_FOTA_PKG = 0x01,    /*!< FOTA package */
    DM_DL_PROGRESS_TYPE_RESERVED = 0x02     /*!< Reserved */
}SWI_TYPE_DM_DL_PROGRESS_TYPE;

/**
 *
 * Contains DM-DL progress data structure.
 *
 */
typedef struct SWI_STRUCT_DM_DL_PROGRESS
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */

    SWI_TYPE_DM_DL_PROGRESS_TYPE nDLType;    /*!< Download Type */

    /*! Number of bytes that have been downloaded */
    swi_uint32 nBytesDownloaded;

    /*! Size of Package to download in bytes */
    swi_uint32 nTotalBytes;
}SWI_STRUCT_DM_DL_PROGRESS;

/**
 *
 * An enumeration of DM-DL valid response type.
 *
 */
typedef enum SWI_TYPE_DM_DL_VALID_RESP
{
    DM_DL_VALID_RESP_REJECT     = 0x01, /*!< Reject */
    DM_DL_VALID_RESP_CONFIRM    = 0x02, /*!< Confirm */

    /*! Delay. May not be valid for some carriers (ie. Sprint for now). */
    DM_DL_VALID_RESP_DELAY      = 0x04
}SWI_TYPE_DM_DL_VALID_RESP;

/**
 *
 * An enumeration of DM-DL alert prompt type.
 *
 */
typedef enum SWI_TYPE_DM_DL_ALERT_PROMPT
{
    /*! Dowload and install */
    DM_DL_ALERT_PROMPT_DL_AND_INSTALL   = 0x00,

    /*! Install only. Dowload has already completed */
    DM_DL_ALERT_PROMPT_INSTALL_ONLY     = 0x01
}SWI_TYPE_DM_DL_ALERT_PROMPT;

/**
 *
 * Contains DM-DL alert data structure.
 *
 */
typedef struct SWI_STRUCT_DM_DL_ALERT
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */
    swi_uint16 nReserved;           /*!< Reserved */

    /*! +mask+ of valid Alert responses */
    SWI_TYPE_DM_DL_VALID_RESP nValidRespMask;

    /*! Alert type */
    SWI_TYPE_DM_DL_ALERT_PROMPT nAlertType;

    /*! boolean to indicate if Description text is available.
       Description text is retrieved using SwiGetDMDLDescription() */
    swi_uint8 bDescAvail;

    /*! Estimated download time in seconds (0 = info not avail from server) */
    swi_uint16 nEstDLTime;

    /*! Estimated install time in seconds (0 = info not avail from server) */
    swi_uint16 nEstInstTime;

    /*! Indicates when the modem will timeout the response in seconds. */
    swi_uint16 nTimeout;

    /*! Length of Package name (0 - 230 chars) */
    swi_uint16 nPkgNameLen;

    /*! Package name (non-Null terminated) */         
    swi_uint8 szName[MAX_DL_PKG_NAME];
}SWI_STRUCT_DM_DL_ALERT;

/**
 *
 * An enumeration of DM-DL alert response type.
 *
 */
typedef enum SWI_TYPE_DM_DL_ALERT_RESP
{
    DM_DL_ALERT_RESP_REJECT     = 0x00, /*!< Reject */
    DM_DL_ALERT_RESP_CONFIRM    = 0x01, /*!< Confirm */
    DM_DL_ALERT_RESP_DELAY      = 0x02  /*!< Delay */
}SWI_TYPE_DM_DL_ALERT_RESP;

/**
 *
 * An enumeration of DM-DL alert response status.
 *
 */
typedef enum SWI_TYPE_DM_DL_ALERT_RESP_STATUS
{
    DM_DL_ALERT_RESP_STATUS_SUCCESS                 = 0x00, /*!< Success */
    DM_DL_ALERT_RESP_STATUS_FAILED_UNSPECIFIED      = 0x01, /*!< Unspecified */

    /*! Modem does not expect the response sent by Host */
    DM_DL_ALERT_RESP_STATUS_FAILED_INVALID_RESPONSE = 0x02
}SWI_TYPE_DM_DL_ALERT_RESP_STATUS;

/**
 *
 * Contains DM-DL description data structure.
 *
 */
typedef struct SWI_STRUCT_DM_DL_DESCRIPTION
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */
    swi_uint16 nReserved;           /*!< Reserved */
    swi_uint16 nDescLength; /*!< Description length (0 - MAX_DL_DESCRIPTION) */

    /*! variable length text (not NULL terminated) */
    swi_uint8  szDescription[MAX_DL_DESCRIPTION];
}SWI_STRUCT_DM_DL_DESCRIPTION;

/**
 *
 * An enumeration of DM-DL install state.
 *
 */
typedef enum SWI_TYPE_DM_DL_INSTALL_STATE
{
    DM_DL_INSTALL_STATE_NULL                        = 0x00, /*!< Unknown */
    DM_DL_INSTALL_STATE_OFFLINE_INSTALL_STARTING    = 0x01, /*!< Starting */
    DM_DL_INSTALL_STATE_OFFLINE_INSTALL_SUCCESS     = 0x02, /*!< Success */

    /*! Failed. DM Status code contains Error Number. */
    DM_DL_INSTALL_STATE_FAIL                        = 0xFF
}SWI_TYPE_DM_DL_INSTALL_STATE;
    
/**
 *
 * Contains DM-DL description data structure.
 *
 */
typedef struct SWI_STRUCT_DM_DL_INSTALL_STATUS
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */

    /*! Estimated install time in seconds */
    swi_uint16 nEstInstallTime;

    /*! Current state/result of install process */
    SWI_TYPE_DM_DL_INSTALL_STATE nInstallState;

    /*! DM status code (use for more detailed failure description) */
    swi_uint16 nDMStatusCode;
}SWI_STRUCT_DM_DL_INSTALL_STATUS;

/* CWE DM TLV Definitions */

/**
 *
 * An enumeration of CWE DM TLV types.
 *
 */
typedef enum SWI_TYPE_DM_TLV_TYPE
{
    DM_TLV_Account_Node_Name                = 1,    /*!< account node name */
    DM_TLV_Node_Path                        = 2,    /*!< node path */
    DM_TLV_Node_Format                      = 3,    /*!< node format */
    DM_TLV_Node_Type                        = 4,    /*!< node type */
    DM_TLV_Node_Value                       = 5,    /*!< node value */
    DM_TLV_Node_Property                    = 6,    /*!< node property */
    DM_TLV_UI_Alert_Text                    = 7,    /*!< UI alert text */

    /*! UI alert default response text */
    DM_TLV_UI_Alert_Default_Response_Text   = 8,
    
    DM_TLV_Get_Node_Data                    = 101,  /*!< get node data */
    DM_TLV_Set_Node_Data                    = 102,  /*!< set node data */
    DM_TLV_Set_Node_Property                = 103,  /*!< set node property */
    DM_TLV_Add_Interior_Node                = 104,  /*!< add interior node */
    DM_TLV_UI_Alert_Text_Input              = 105   /*!< UI alert text input */
}SWI_TYPE_DM_TLV_TYPE;

#define LENGTH_DM_TLV_HEADER    4       /*!< DM TLV Header size in byte */
#define NUM_Node_Data_Sub_TLV   4       /*!< Number of sub-TLV for Node data */

/* Function prototypes */

SWI_API SWI_RCODE SwiGetOMADMConfig(SWI_STRUCT_OMADM_GET_CONFIG *psConfig, 
                                    swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetOMADMConfig(
        const SWI_STRUCT_OMADM_SET_CONFIG *psConfig, 
        swi_uint8 *pStatus, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiStartOMADMSession(
        const SWI_STRUCT_OMADM_START_SESSION *psSession, 
        swi_uint8 *pStatus, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiCancelOMADMSession(SWI_STRUCT_OMADM_CANCEL_RESP *pStatus, 
                                        swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetOMADMSessionState(
        SWI_STRUCT_OMADM_SESSION_STATE *psState, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetOMADMNIAlert(SWI_STRUCT_OMADM_NI_ALERT *psAlert, 
                                     swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetOMADMNIAlertResponse(
        swi_uint16 nResponse, 
        SWI_STRUCT_OMADM_NI_ALERT_RESP *psStatus, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetOMADMUIAlertConfirm(
        const SWI_STRUCT_OMADM_UI_ALERT_CONFIRM_RESP *psConfirm, 
        swi_uint16 *pConfResp, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetOMADMUIAlertInput(
        const SWI_STRUCT_OMADM_UI_ALERT_INPUT_RESP *psInput, 
        swi_uint16 *pInputResp, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetOMADMUIAlertChoice(
        const SWI_STRUCT_OMADM_UI_ALERT_CHOICE_RESP *psChoice, 
        swi_uint16 *pConfResp, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetOMADMBootAlgorithm(swi_uint16 *pStatus, 
                                           swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetOMADMNodeValue(
        const SWI_STRUCT_OMADM_NODE_VALUE_REQUEST *psItem, 
        SWI_STRUCT_OMADM_NODE_VALUE_RESPONSE *psValue, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetOMADMNodeValue(
        const SWI_STRUCT_OMADM_NODE_VALUE *psValue, 
        swi_uint8 *pStatus, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetDMDLAlertResponse(swi_uint8 nResponse, 
                                          swi_uint8* pStatus, 
                                          swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetDMDLProgress(SWI_STRUCT_DM_DL_PROGRESS *psProgress, 
                                     swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetDMDLDescription(
        SWI_STRUCT_DM_DL_DESCRIPTION *psDescription, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetDMDLInstallStatus(
        SWI_STRUCT_DM_DL_INSTALL_STATUS *psStatus, 
        swi_uint32 timeout);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __SWIAPICMDM_H__ */

/* 
 * $Log: SwiApiCmDm.h,v $
 */

