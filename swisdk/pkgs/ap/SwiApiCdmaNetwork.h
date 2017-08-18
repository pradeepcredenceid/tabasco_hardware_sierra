/**
 *    \if CvsId 
 *    $Id: SwiApiCdmaNetwork.h,v 1.4 2010/06/10 00:10:50 epasheva Exp $
 *    \endif
 *    \ingroup cdmanetwork
 *    \file SwiApiCdmaNetwork.h
 * 
 *    \brief SWI CDMA specific network related APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#ifndef __SWIAPICDMANETWORK_H__
#define __SWIAPICDMANETWORK_H__

#ifdef __cplusplus
extern "C" {
#endif

/* include files */
#include "SwiDataTypes.h"
#include "SwiRcodes.h"
#include "SwiApiCdmaBasic.h"

#define SWI_SIDNIDNUM                       20
#define SWI_MAX_ActiveSet                   6    /* Same as MAX_ActiveSet */

/* Connection Management */

/**
 *
 * Contains call notification status data structure.
 *
 */
typedef struct SWI_STRUCT_CallNotificationStatus
{
    swi_uint32 sizeStruct;         /*!< Size of this structure */

    /*! Norification status. 
                0x0000: Notification Off; 
                0x0001: Notification On */
    swi_uint32 nNotifyStatus;
}SWI_STRUCT_CallNotificationStatus;

/**
 *
 * An enumeration of call type.
 *
 */
typedef enum SWI_TYPE_CDMA_CallType
{
    /*! Standard voice call and originating an OTASP call. */
    SWI_CALL_TYPE_Voice             = 0x0000,

    /*! Call waiting (Incoming only) */
    SWI_CALL_TYPE_VoiceAlert        = 0x0100,

    /*! Asynchronous Data. QNC data call */
    SWI_CALL_TYPE_AsyncData         = 0x0001,

    SWI_CALL_TYPE_PacketData        = 0x0101,   /*!< Packet Data. */
    SWI_CALL_TYPE_Fax               = 0x0201,   /*!< Fax */

    /*! SMS. indicates the connection being made is for SMS transactions. */
    SWI_CALL_TYPE_Sms               = 0x0002,

    SWI_CALL_TYPE_Test              = 0x0003,   /*!< Test (not used) */
    
    /*! OTAPA. A call from a special number to update network settings in the 
        modem. (Incoming only) */
    SWI_CALL_TYPE_Otapa             = 0x0005,

    /*! Standard OTASP.  Not used in call origination or termination 
        (answering). When OTASP is established a voice call may change 
        to this type.*/
    SWI_CALL_TYPE_StandardOtasp     = 0x0006,

    /*! Nonstandard OTASP (not used) */
    SWI_CALL_TYPE_NonstandardOtasp  = 0x0007,

    /*! E911. An emergency mode call. */
    SWI_CALL_TYPE_E911              = 0x0008,

    SWI_CALL_TYPE_EVDO              = 0x0009,   /*!< EVDO call */
    SWI_CALL_TYPE_None              = 0xffff    /*!< Not used */
}SWI_TYPE_CDMA_CallType;

/**
 *
 * An enumeration of call state.
 *
 * This enumeration type identifies the elements of a bit-mapped call state. 
 * More than one bit can be
 *
 */
typedef enum SWI_TYPE_CDMA_CallState
{
    SWI_CALL_STATE_Idle             = 0x0000,   /*!< No call */
    SWI_CALL_STATE_CnsAsyncData     = 0x0001,   /*!< Async Data call */
    SWI_CALL_STATE_VoiceCall        = 0x0002,   /*!< Voice call */
    SWI_CALL_STATE_Packet           = 0x0004,   /*!< Packet 1xRtt call */

    /*! Async Data call (via AT cmds) */
    SWI_CALL_STATE_AtAsyncData      = 0x0010,

    /*! Voice call (via AT cmds) */
    SWI_CALL_STATE_AtVoiceCall      = 0x0020,

    /*! Packet 1xRtt call (via AT cmds) */
    SWI_CALL_STATE_AtPacket         = 0x0040,

    SWI_CALL_STATE_Fax              = 0x0080,   /*!< Fax call */
    SWI_CALL_STATE_Sms              = 0x0200,   /*!< SMS */
    SWI_CALL_STATE_OTA              = 0x0400,   /*!< OTA */
    SWI_CALL_STATE_Test             = 0x0800,   /*!< Not used */
    SWI_CALL_STATE_Cw               = 0x1000,   /*!< Not used */
    SWI_CALL_STATE_NotUsed          = 0x2000,   /*!< Not used */

    /*! Data connection is in dormant state */
    SWI_CALL_STATE_Dormant          = 0x4000,

    /*! An emergency (E911) call */
    SWI_CALL_STATE_Emergency        = 0x8000
}SWI_TYPE_CDMA_CallState;

/**
 *
 * An enumeration of reason for call termination.
 *
 */
typedef enum SWI_TYPE_EndReason
{
    SWI_END_REASON_Offline,                     /*!< Phone is offline */

    /*! Phone is CDMA locked until power-cycle */
    SWI_END_REASON_CdmaLockedUntilPowerCycle,

    SWI_END_REASON_NoService,                   /*!< Phone has no service */

    /*! Call has ended abnormally */
    SWI_END_REASON_CallEndedAbnormally,

    /*! Received intercept from BS (Orig. only) */
    SWI_END_REASON_ReceivedInterceptFromBs,

    /*! Received release from BS (Orig. only) */
    SWI_END_REASON_ReceivedReorderFromBs,

    /*! Received release from BS - no reason given */
    SWI_END_REASON_ReceivedReleaseFromBsNoReason,

    /*! Received release from BS - SO reject */
    SWI_END_REASON_ReceivedInterceptFromBsSoReject,

    /*! Received incoming call from BS */
    SWI_END_REASON_ReceivedIncomingCallFromBs,

    /*! Received alert stop from BS (incoming only) */
    SWI_END_REASON_ReceivedAlertStopFromBs,

    /*! Client ended the call */
    SWI_END_REASON_ClientEndedCall,

    /*! Received end activation (OTASP call only) */
    SWI_END_REASON_ReceivedEndActivation,

    /*! Obsolete */
    SWI_END_REASON_McAborts,

    /*! NDSS failure */
    SWI_END_REASON_NdssFailure,

    /*! Max access probes transmitted */
    SWI_END_REASON_MaxAccessProbesTransmitted,

    /*! Persistence test failure */
    SWI_END_REASON_PersistenceTestFailure,

    /*! RUIM not present */
    SWI_END_REASON_RuimNotPresent,

    /*! Access attempt already in progress */
    SWI_END_REASON_AccessAttemptAlreadyInProgress,

    /*! Access failure for reason other than the above reason */
    SWI_END_REASON_AccessFailureReasonUnspecified,

    /*! Received retry order (Orig. only) */
    SWI_END_REASON_ReceivedRetryOrder,
    
    /*! Reserved for special faked Disconnect event */
    SWI_END_REASON_Reserved1206 = 0x1206
}SWI_TYPE_EndReason;

/**
 *
 * An enumeration of Call Source (AT Cmds or API) enumeration.
 *
 * This enumerates the call source options which identify if a call was 
 * connected using AT commands or using the CnS protocol (used by the API).
 *
 */
typedef enum SWI_TYPE_CallSource
{
    SWI_CALL_SOURCE_At,     /*!< AT Commands */
    SWI_CALL_SOURCE_Cns     /*!< CNS (API) */
}SWI_TYPE_CallSource;

/**
 *
 * An enumeration of flash Context.
 *
 * Values for use with the SwiSetCallFlashCmd function.
 *
 */
typedef enum SWI_TYPE_FlashContext
{
    /*! Don’t Care. This passes the call number to the call manager without 
        checking that the flash command is valid in the present call state. 
        Use this context when you want manual control over the flash command. 
        If enabled, the [CallError] notifications are sent if the flash is 
        invoked in an inappropriate call state.*/
    SWI_FLASH_CONTEXT_DontCare          = 0x0000,

    /*! Call Waiting. This indicates to the call manager that the flash is 
        for servicing a waiting call. When first invoked, it puts the current 
        call on hold and connects the second call that is waiting. hold and 
        connects the second call that is waiting. original call and holds the 
        second call. */
    SWI_FLASH_CONTEXT_CallWaiting       = 0x0001,

    /*! Threeway Setup. This indicates to the call manager that the flash is 
        for setting up a threeway call. A call number must be provided or 
        else an error message will be returned as the response. This context 
        sends two flashes to the base station, the first without a phone 
        number and the second with a phone number. */
    SWI_FLASH_CONTEXT_ThreewaySetup     = 0x0002,

    /*! Threeway Connect. This indicates to the call manager that the flash 
        is to connect all three parties. For this context, a call number must 
        not be provided, otherwise an error message is returned as the 
        response. Use the Don't Care flash context with an empty call number 
        to disconnect the last connected party. */
    SWI_FLASH_CONTEXT_ThreewayConnect   = 0x0003,

    /*! Answer Hold Not implemented. */
    SWI_FLASH_CONTEXT_AnswerHold        = 0x0004,

    /*! Answer Forward Not implemented. */
    SWI_FLASH_CONTEXT_AnswerForward     = 0x0005
}SWI_TYPE_FlashContext;

/**
 *
 * An enumeration of answer State.
 *
 * The modes of operation the modem uses to determine how to answer the next 
 * incoming voice call.
 *
 */
typedef enum SWI_TYPE_AnswerState 
{
    /*! Answer voice as voice */
    SWI_ANSWER_STATE_VoiceAsVoice,

    /*! Answer voice as fax once (for the next 10 minutes) */
    SWI_ANSWER_STATE_VoiceAsFaxOnce,     

    /*! Answer voice as fax always */
    SWI_ANSWER_STATE_VoiceAsFaxAlways,   

    /*! Answer voice as modem once (for the next 10 minutes) */
    SWI_ANSWER_STATE_VoiceAsModemOnce,   

    /*! Answer voice as modem always */
    SWI_ANSWER_STATE_VoiceAsModemAlways  
}SWI_TYPE_AnswerState;

/* Service Status */

/**
 *
 * Contains Channel # notification data structure.
 *
 * This data structure is used as a member of the notification
 * SWI_NOTIFY_ChannelNumber.  Refer to the description of
 * SWI_NOTIFY_ChannelNumber for more detail.
 *
 * \sa SwiGetChannelNumber
 */
typedef struct SWI_STRUCT_ChannelNumber
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */
    swi_uint32 nChannelNumber;      /*!< Channel # */
}SWI_STRUCT_ChannelNumber;

/**
 *
 * An enumeration of channel state.
 *
 */
typedef enum SWI_TYPE_ChannelState
{
    SWI_CHANNEL_Sleep   = 0,    /*!< Sleep */
    SWI_CHANNEL_Aquired = 1,    /*!< Aquired */
    SWI_CHANNEL_Scan    = 5     /*!< Scan */
}SWI_TYPE_ChannelState;

/**
 *
 * Contains Channel State notification data structure.
 *
 * This data structure is used as a member of the notification
 * SWI_NOTIFY_ChannelState.  Refer to the description of 
 * SWI_NOTIFY_ChannelState for more detail.
 *
 * \sa SWI_TYPE_ChannelState
 */
typedef struct SWI_STRUCT_ChannelState
{
    swi_uint32 sizeStruct;                  /*!< Size of this structure */
    SWI_TYPE_ChannelState eChannelState;    /*!< Channel state */
}SWI_STRUCT_ChannelState;

/**
 *
 * An enumeration of bandclass.
 *
 */
typedef enum SWI_TYPE_TechBandClass
{
    SWI_TECH_BANDCLASS_Cellular,    /*!< Cellular */
    SWI_TECH_BANDCLASS_Pcs          /*!< PCS */
}SWI_TYPE_TechBandClass;

/**
 *
 * Contains Bandclass notification data structure.
 *
 * This data structure is used as a member of the notification
 * SWI_NOTIFY_TechBandClass.  Refer to the description of
 * SWI_NOTIFY_TechBandClass for more detail.
 *
 * \sa SWI_TYPE_TechBandClass
 */
typedef struct SWI_STRUCT_TechBandClass
{
    swi_uint32 sizeStruct;              /*!< Size of this structure */
    SWI_TYPE_TechBandClass eBandClass;  /*!< Bandclass */
}SWI_STRUCT_TechBandClass;

/**
 *
 * An enumeration of service indication.
 *
 * This enumeration contains the service indication states.
 * These do not include EVDO/HDR service states.  See SWI_TYPE_Hdr_Srv_State.
 *
 */
typedef enum SWI_TYPE_ServiceIndication 
{
    SWI_SERVICE_INDICATION_None,        /*!< No service */
    SWI_SERVICE_INDICATION_Analog,      /*!< analog */
    SWI_SERVICE_INDICATION_Digital      /*!< digital */
}SWI_TYPE_ServiceIndication;

/**
 *
 * Contains service indication notification data structure.
 *
 * This data structure is used as a member of the notification
 * SWI_NOTIFY_ServiceIndication.  Refer to the description of 
 * SWI_NOTIFY_ServiceIndication for more detail.
 *
 * \sa SWI_TYPE_ServiceIndication
 */
typedef struct SWI_STRUCT_ServiceIndication
{
    swi_uint32 sizeStruct;      /*!< Size of this structure */
    SWI_TYPE_ServiceIndication eServiceIndicator;   /*!< Service indication */
}SWI_STRUCT_ServiceIndication;

/**
 *
 * An enumeration of Protocol Revision.
 *
 * This enumerates the protocol revision supported by the serving base station.
 * 
 * Values of 6 or 7 (Cdma2000) indicate 1xRTT coverage is availble. Value 1 
 * (Jstd008) is not supported.
 *
 */
typedef enum SWI_TYPE_PrevValue
{
    SWI_PREV_VALUE_Jstd008       = 1,   /*!< Not supported */
    SWI_PREV_VALUE_Is95,                /*!< IS95 */
    SWI_PREV_VALUE_Is95A,               /*!< IS95A */
    SWI_PREV_VALUE_MinIs95B,            /*!< Minimum IS95B */
    SWI_PREV_VALUE_FullIs95B,           /*!< Full IS95B */
    SWI_PREV_VALUE_Cdma2000Rev0,        /*!< CDMA 2000 Rev 0 */
    SWI_PREV_VALUE_Cdma2000Rev1         /*!< CDMA 2000 Rev 1 */
}SWI_TYPE_PrevValue;

/**
 *
 * Contains Protocol Revision notification data structure.
 *
 * This data structure is used as a member of the notification
 * SWI_NOTIFY_Prev.  Refer to the description of
 * SWI_NOTIFY_Prev for more detail.
 *
 * \sa SWI_TYPE_PrevValue
 */
typedef struct SWI_STRUCT_Prev
{
    swi_uint32 sizeStruct;      /*!< Size of this structure */
    SWI_TYPE_PrevValue eValue;  /*!< Protocal Revision value */
}SWI_STRUCT_Prev;

/**
 *
 * An enumeration of roaming state.
 *
 * This type definition covers the possible return values of the 
 * function SwiGetRoamingState, and the SWI_NOTIFY_NetworkStatus 
 * notification.
 *
 */
typedef enum SWI_TYPE_CdmaRoamingState
{
    SWI_CDMAROAMING_Home,       /*!< Not Roaming */
    SWI_CDMAROAMING_Roaming,    /*!< Roaming */
    SWI_CDMAROAMING_NonPref,    /*!< Not support */
    SWI_CDMAROAMING_None        /*!< Not support */
}SWI_TYPE_CdmaRoamingState;

/**
 *
 * Contains roaming state notification data structure.
 *
 * This data structure is used as a member of the notification
 * SWI_NOTIFY_RoamingStatus.  Refer to the description of 
 * SWI_NOTIFY_RoamingStatus for more detail.
 *
 * \sa SWI_TYPE_CdmaRoamingState
 */
typedef struct SWI_STRUCT_RoamingStatus
{
    swi_uint32 sizeStruct;              /*!< Size of this structure */
    SWI_TYPE_CdmaRoamingState eRoamingStatus;   /*!< Roaming status */
}SWI_STRUCT_RoamingStatus;

/* Call Management */

/**
 *
 * Contains byte Counter notification data structure.
 *
 * This data structure is used as a member of the notification
 * SWI_NOTIFY_CallByteCounter.  Refer to the description of
 * SWI_NOTIFY_CallByteCounter for more detail.
 *
 * The counters measure traffic over the local (host/modem) interface. 
 * The counter resets when the call is disconnected.
 *
 */
typedef struct SWI_STRUCT_CallByteCounter
{
    swi_uint32 sizeStruct;      /*!< Size of this structure */
    swi_uint32 nTxBytes;        /*!< Bytes transmitted */
    swi_uint32 nRxBytes;        /*!< Bytes received */
}SWI_STRUCT_CallByteCounter;

/**
 *
 * Contains call connecting notification data structure.
 *
 * This data structure is used as a member of the notification
 * SWI_NOTIFY_CallConnecting.  Refer to the description of
 * SWI_NOTIFY_CallConnecting for more detail.
 *
 */
typedef struct SWI_STRUCT_CallConnecting
{
    swi_uint32 sizeStruct;              /*!< Size of this structure */
    SWI_TYPE_CDMA_CallType eType;       /*!< Type of call being connected */
    
    /*! indicates if the connection was equested by AT command or CnS (API)*/
    SWI_TYPE_CallSource eSource;

    /*! standard caller ID of incoming calls (if available from the network). 
        Use SWI_NOTIFY_CallCallerID to determine CallerID prior to 
        connecting. */
    swi_char szNum[LENGTH_CallNumber+1];
}SWI_STRUCT_CallConnecting;

/**
 *
 * Contains call connected notification data structure.
 *
 * This data structure is used as a member of the notification
 * SWI_NOTIFY_CallConnected.  Refer to the description of
 * SWI_NOTIFY_CallConnected for more detail.
 *
 */
typedef struct SWI_STRUCT_CallConnected
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */
    SWI_TYPE_CDMA_CallState eState; /*!< Call State bit mask */
    swi_uint32 nSo;                 /*!< Service option of the call */
}SWI_STRUCT_CallConnected;

/**
 *
 * Contains call incoming notification data structure.
 *
 * This data structure is used as a member of the notification
 * SWI_NOTIFY_CallIncoming.  Refer to the description of
 * SWI_NOTIFY_CallIncoming for more detail.
 *
 */
typedef struct SWI_STRUCT_CallIncoming
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */
    SWI_TYPE_CDMA_CallType eType;   /*!< Incoming call type */
}SWI_STRUCT_CallIncoming;

/**
 *
 * Contains Caller ID notification data structure.
 *
 * This data structure is used as a member of the notification
 * SWI_NOTIFY_CallCallerID.  Refer to the description of
 * SWI_NOTIFY_CallCallerID for more detail.
 *
 * The Caller ID string is provided by the network.
 *
 */
typedef struct SWI_STRUCT_CallCallerID
{
    swi_uint32 sizeStruct;              /*!< Size of this structure */
    swi_char szNum[LENGTH_CallNumber+1];   /*!< Caller ID string */
}SWI_STRUCT_CallCallerID;

/**
 *
 * Contains Caller ID notification data structure.
 *
 * This data structure is used as a member of the notification
 * SWI_NOTIFY_CallCallerID.  Refer to the description of
 * SWI_NOTIFY_CallCallerID for more detail.
 *
 * The Extended Caller ID string is provided by the network.
 *
 */
typedef struct SWI_STRUCT_CallExtendedCallerID
{
    swi_uint32 sizeStruct;              /*!< Size of this structure */

    /*! Extended Caller ID string */
    swi_char szNum[LENGTH_CallExtendedNumber+1];
}SWI_STRUCT_CallExtendedCallerID;

/**
 *
 * Contains call disconnected notification data structure.
 *
 * This data structure is used as a member of the notification
 * SWI_NOTIFY_CallDisconnected.  Refer to the description of
 * SWI_NOTIFY_CallDisconnected for more detail.
 *
 */
typedef struct SWI_STRUCT_CallDisconnected
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */
    SWI_TYPE_CDMA_CallState eState; /*!< Call State bit mask */
    SWI_TYPE_EndReason eReason;     /*!< reason the call was disconnected */
}SWI_STRUCT_CallDisconnected;

/**
 *
 * An enumeration of call error.
 *
 * This enumerates the call errors provided by the notification
 * SWI_NOTIFY_CallError.
 *
 */
typedef enum SWI_TYPE_CDMA_CallError
{
    /*! No errors found */
    SW_CALL_ERROR_None,

    /*! Invalid client ID - null or not initialized */
    SW_CALL_ERROR_InvalidClientID,

    /*! Bad call type parameter */
    SW_CALL_ERROR_BadCallTypeParameter,

    /*! Bad service type parameter */
    SW_CALL_ERROR_BadServiceTypeParameter,

    /*! Bad service option parameter */
    SW_CALL_ERROR_BadServiceOptionParameter,

    /*! Expecting a number to be provided via the number parameter */
    SW_CALL_ERROR_ExpectingNumber,

    /*! Number length is > 0, but number buffer points to NULL */
    SW_CALL_ERROR_NumberLengthGreaterThanZeroBufferNull,

    /*! Invalid digits found in number buffer */
    SW_CALL_ERROR_InvalidDigitsNumberBuffer,

    /*! Out of range value found in number length field */
    SW_CALL_ERROR_OutOfRangeNumberLength,

    /*! Alpha length is > 0, but alpha buffer points to NULL */
    SW_CALL_ERROR_AlphaLengthGreaterThanZeroBufferNull,

    /*! Out of range value found in alpha length field */
    SW_CALL_ERROR_OutOfRangeAlphaLength,

    /*! Invalid OTASP activation code */
    SW_CALL_ERROR_InvalidOtaspActivationCode,

    /*! Phone is offline */
    SW_CALL_ERROR_PhoneOffline,

    /*! Phone is CDMA locked */
    SW_CALL_ERROR_PhoneCdmaLocked,

    /*! Client tried to send a flash while a call that not supporting flashes 
        is in progress*/
    SW_CALL_ERROR_ClientSendingFlasForUnsupportdFlashCall,

    /*! Dialed number is not allowed under the current origination 
        restriction */
    SW_CALL_ERROR_DialedNumberNotPermitted,

    /*! Client tried to originate a NONE E911 call while E911 mode preference 
        is in effect*/
    SW_CALL_ERROR_NonE911CallWhileE911PreferenceInEffect,

    /*! Can't perform the command in question while phone is in-use */
    SW_CALL_ERROR_InvalidCmdWhilePhoneInUse,

    /*! Client originated with service type that is currently not supported 
        by the phone/PRL*/
    SW_CALL_ERROR_ServiceTypeNotSupportedByPhonePrl,

    /*! Client answered with inappropriate call type relative to the type of 
        the incoming call*/
    SW_CALL_ERROR_ClientAnsweredWithWrongCallType,

    /*! The command in question is invalid in the current call state */
    SW_CALL_ERROR_CmdInvalidForCurrentState,

    /*! The command in question is invalid in the current Phone serving 
        state */
    SW_CALL_ERROR_CmdInvalidForCurrentPhoneServingState,

    /*! No valid service found for origination with AUTOMATIC or 
        CDMA_AUTOMATIC service type*/
    SW_CALL_ERROR_NoServiceFoundForAutomatic,

    /*! Client is not allowed to answer the incoming call */
    SW_CALL_ERROR_ClientCannotAnswerIncomingCall,

    /*! Bad privacy preference setting */
    SW_CALL_ERROR_BadPrivacyPref,

    /*! No available CM command buffers to queue the command */
    SW_CALL_ERROR_NoAvailableCmdBuffers,

    /*! Communication problems with MC */
    SW_CALL_ERROR_CommunicationProblemsWithMc,

    /*! A problem other than the above was found */
    SW_CALL_ERROR_Unspecified
}SWI_TYPE_CDMA_CallError;

/**
 *
 * An enumeration of call command.
 *
 * This enumerates the call commands provided by the notification
 * SWI_NOTIFY_CallError.
 *
 */
typedef enum SWI_TYPE_CallCmd
{
    SWI_CALL_CMD_Orig,          /*!< Call origination command */
    SWI_CALL_CMD_Answer,        /*!< Call answer command */
    SWI_CALL_CMD_End,           /*!< Call end command */
    SWI_CALL_CMD_Flash,         /*!< Flash command */
    SWI_CALL_CMD_PrivacyPref,   /*!< Privacy Pref. Command */
    SWI_CALL_CMD_InfoGet,       /*!< Info command */
    SWI_CALL_CMD_Hold,          /*!< Hold command */ 
    SWI_CALL_CMD_None = 0x00FF  /*!< No command */
}SWI_TYPE_CallCmd;

/**
 *
 * Contains call error notification data structure.
 *
 * This data structure is used as a member of the notification
 * SWI_NOTIFY_CallError.  Refer to the description of
 * SWI_NOTIFY_CallError for more detail.
 *
 * \sa SWI_TYPE_CallCmd
 * \sa SWI_TYPE_CDMA_CallError
 * \sa SWI_TYPE_CallSource
 */
typedef struct SWI_STRUCT_CallError
{
    swi_uint32 sizeStruct;              /*!< Size of this structure */

    /*! The command or action being used when the error occured.*/
    SWI_TYPE_CallCmd eCmd;

    SWI_TYPE_CDMA_CallError eError;     /*!< The nature of the error */
    
    /*! indicates if the action was requested by AT command or CnS (API)*/
    SWI_TYPE_CallSource eSource;
}SWI_STRUCT_CallError;

/**
 *
 * Contains dormant state notification data structure.
 *
 * This data structure is used as a member of the notification
 * SWI_NOTIFY_CallDormant.  Refer to the description of
 * SWI_NOTIFY_CallDormant for more detail.
 *
 * \sa SWI_TYPE_CDMA_CallState
 * \sa SWI_TYPE_EndReason
 */
typedef struct SWI_STRUCT_CallDormant
{
    swi_uint32 sizeStruct;              /*!< Size of this structure */
    SWI_TYPE_CDMA_CallState eState;     /*!< Call State bit mask */
    SWI_TYPE_EndReason eReason;         /*!< Not Used */
}SWI_STRUCT_CallDormant;

/**
 *
 * An enumeration of OTASP state.
 *
 * This enumeration type identifies the available OTASP progress states.
 *
 * A successful OTASP session assumes no failures for any states, and a 
 * successful commit.
 *
 */
typedef enum SWI_TYPE_OTASPState
{
    /*! Programming started */
    SWI_OTASP_STATE_PROG_START      = 0x0000,

    /*! SPL unlock - required to access certain parameters */
    SWI_OTASP_STATE_SPL_UNLOCK      = 0x0001,

    /*! AKey exchange - Authentication key has been exchanged */
    SWI_OTASP_STATE_AKEY_EXCHG      = 0x0002,

    /*! Shared Secret Data has been updated */
    SWI_OTASP_STATE_SSD_UPDT        = 0x0003,

    /*! New NAM parameters have been downloaded */
    SWI_OTASP_STATE_NAM_DL          = 0x0004,

    /*! New MDM has been downloaded */
    SWI_OTASP_STATE_MDN_DL          = 0x0005,

    /*! IMSI has been downloaded */
    SWI_OTASP_STATE_IMSI_DL         = 0x0006,

    /*! PRL has been downloaded */
    SWI_OTASP_STATE_PRL_DL          = 0x0007,

    /*! Newly downloaded parameters have been transferred to NV-RAM */
    SWI_OTASP_STATE_COMMIT          = 0x0008,

    /*! Programming ended */
    SWI_OTASP_STATE_PROG_END        = 0x0009
}SWI_TYPE_OTASPState;

/**
 *
 * Contains OTASP state notification data structure.
 *
 * This data structure is used as a member of the notification
 * SWI_NOTIFY_OTASPState.  Refer to the description of
 * SWI_NOTIFY_OTASPState for more detail.
 *
 * \sa SWI_TYPE_OTASPState
 */
typedef struct SWI_STRUCT_OTASPState
{
    swi_uint32 sizeStruct;              /*!< Size of this structure */
    
    /*! The current stage in the OTASP process */
    SWI_TYPE_OTASPState eState;
    
    /*! Indicates the success (1) or failure (0) of the stage indicated by 
        eState */
    swi_uint32 nSuccess;
}SWI_STRUCT_OTASPState;

/**
 *
 * An enumeration of privacy state.
 *
 * The data enumerates as privacy either on or off
 *
 */
typedef enum SWI_TYPE_Privacy
{
    SWI_PRIVACY_Off,    /*!< Privacy off */
    SWI_PRIVACY_On      /*!< Privacy on */
}SWI_TYPE_Privacy;

/**
 *
 * Contains OTASP state notification data structure.
 *
 * This data structure is used as a member of the notification
 * SWI_NOTIFY_CallPrivacy.  Refer to the description of
 * SWI_NOTIFY_CallPrivacy for more detail.
 *
 * \sa SWI_TYPE_Privacy
 */
typedef struct SWI_STRUCT_CallPrivacy
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */
    SWI_TYPE_Privacy eMode;         /*!< Privacy state */
}SWI_STRUCT_CallPrivacy;

/**
 *
 * Contains SID/NID pair data.
 *
 * This structure is used as a member of SWI_STRUCT_HomeSidNid.
 *
 */
typedef struct SWI_STRUCT_SidNid
{
    swi_uint16 Sid;     /*!< SID value */
    swi_uint16 Nid;     /*!< NID value */
}SWI_STRUCT_SidNid;

/**
 *
 * Contains SID/NID pairs data.
 *
 * This structure is used to report (SwiPstGetHomeSidNid) or set
 * (SwiPstSetHomeSidNid) the home (non-roaming)
 * SID / NID table for an account profile.
 *
 */
typedef struct SWI_STRUCT_HomeSidNid
{
    SWI_STRUCT_SidNid SidNid[SWI_SIDNIDNUM];    /*!< SID NID pair array */
}SWI_STRUCT_HomeSidNid;

/**
 *
 * An enumeration of modem pref.
 *
 * For compatibility with QXDM database, please do not change the following 
 * fix assigned values. If new values are added please also update the QXDM
 * database.
 *
 */
typedef enum cm_mode_pref_e {

    CM_MODE_PREF_NONE = -1,     /*!< FOR INTERNAL USE OF CM ONLY! */

    /*! Service is limited to analog only. = NV_MODE_ANALOG_ONLY */
    CM_MODE_PREF_ANALOG_ONLY = 0,

    /*! Service is limited to digital (CDMA or HDR) only. 
        = NV_MODE_DIGITAL_ONLY */
    CM_MODE_PREF_DIGITAL_ONLY = 1,

    /*! Determine the mode automatically from the PRL order.
        = NV_MODE_AUTOMATIC */
    CM_MODE_PREF_AUTOMATIC = 2,

    /*! Emergency mode. = NV_MODE_E911 */
    CM_MODE_PREF_E911 = 3,

    /*! PCS mode - obsolete in SD 2.0 (use band preference).
        = NV_MODE_PCS_CDMA_ONLY */
    CM_MODE_PREF_PCS_CDMA_ONLY = 4,

    /*! Cellular mode - obsolete in SD 2.0 (use band preference).
        = NV_MODE_CELL_CDMA_ONLY */
    CM_MODE_PREF_CELL_CDMA_ONLY=5,

    /* New for SD 2.0. For compatibility with QPST, do not change values or
    ** order. We start with NV_MODE_CELL_CDMA_ONLY+1 (i.e. 9).
    */

    /*! Service is limited to CDMA only */
    CM_MODE_PREF_CDMA_ONLY = 9,

    /*! Service is limited to HDR only */
    CM_MODE_PREF_HDR_ONLY=10,

    /*! Service is limited to CDMA and AMPS only */
    CM_MODE_PREF_CDMA_AMPS_ONLY=11,

    /*! Service is limited to GPS only */
    CM_MODE_PREF_GPS_ONLY=12,

    /*
    ** The following enums are the radio access technologies for
    ** WCDMA and GSM.
    */

    /*! Service is limited to GSM only */
    CM_MODE_PREF_GSM_ONLY=13,

   /*! Service is limited to WCMDA only */
    CM_MODE_PREF_WCDMA_ONLY=14,

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*! Return to persistent mode preference */
    CM_MODE_PREF_PERSISTENT,

    /*! Do not change the mode preference */
    CM_MODE_PREF_NO_CHANGE,

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    CM_MODE_PREF_MAX          /*!< FOR INTERNAL USE OF CM ONLY! */

} cm_mode_pref_e_type;

/**
 *
 * An enumeration of mode terms.
 *
 * Mode terms that are assigned automatically by call manager when mode is 
 * forced on the fly during a call origination.
 *
 * CM unforce the mode back to it original settings at end of call or some 
 * short time after end of call.
 *
 * NOTE! These mode term should NOT be used by clients when changing the 
 * mode preference.
 *
 */
typedef enum SWI_TYPE_Mode_Term
{

    CNS_MODE_TERM_NONE = -1,    /*!< FOR INTERNAL USE OF CM ONLY! */


    CNS_MODE_TERM_PERMANENT,    /*!< Permanent mode change */

    CNS_MODE_TERM_PWR_CYCLE,    /*!< Until power cycle */

    CNS_MODE_TERM_1_CALL,       /*!< Until end of next call or power-cycle */

    /*! Until end of next call, specified timeout or power-cycle */
    CNS_MODE_TERM_1_CALL_OR_TIME,

    CNS_MODE_TERM_CM_1_CALL,
    CNS_MODE_TERM_CM_1_CALL_PLUS,
    CNS_MODE_TERM_CM_MO_SMS,

    CNS_MODE_TERM_MAX           /*!< FOR INTERNAL USE OF CM ONLY! */

}SWI_TYPE_Mode_Term;

/**
 *
 * An enumeration of band preferences.
 *
 */
typedef enum SWI_TYPE_Band_Pref
{
    CNS_BAND_PREF_NONE      = 0,        /*!< Acquire no system */

    /*! Acquire band class 0, A-Side systems only (US Cellular A 800 MHz) */
    CNS_BAND_PREF_BC0_A     = 0x01,

    /*! Acquire band class 0, B-Side systems only (US Cellular B 800 MHz) */
    CNS_BAND_PREF_BC0_B     = 0x02,

    /*! Acquire band class 0 systems only (US Cellular A/B 800 MHz) */
    CNS_BAND_PREF_BC0       = 0x03,

    /*! Acquire band class 1 systems only (US PCS 1900 MHz) */
    CNS_BAND_PREF_BC1       = 0x04,

    /*! Acquire band class 3 systems only (Japanese Cellular 800 MHz) */
    CNS_BAND_PREF_BC3       = 0x10,

    /*! Acquire band class 4 systems only (Korean PCS 1800 MHz) */
    CNS_BAND_PREF_BC4       = 0x20,

    /*! Acquire band class 5 systems only (450 MHz) */
    CNS_BAND_PREF_BC5       = 0x40,

    /*! Acquire band class 6 systems only (IMT2000) */
    CNS_BAND_PREF_BC6       = 0x80,

    /*! Acquire any band class systems */
    CNS_BAND_PREF_ANY       = 0xFFFF,

    /*! Do not change the band preference */
    CNS_BAND_PREF_NO_CHANGE = 0x10000,   

    CNS_BAND_PREF_MAX       /*!< Place holder */

}SWI_TYPE_Band_Pref;

/**
 *
 * An enumeration of system selection roam preferences.
 *
 */
typedef enum SWI_TYPE_Roam_Pref
{
    CNS_ROAM_PREF_NONE = -1,        /*!< FOR INTERNAL USE OF CM ONLY! */

    /*! Acquire only systems for which the roaming indication is off */
    CNS_ROAM_PREF_HOME     = 0x01,

    /*! Acquire only systems for which the roaming indication is strictly on */
    CNS_ROAM_PREF_ROAM     = 0x02,

    /*! Acquire only systems for which the roaming indication is off or
        solid on - i.e. not flashing */
    CNS_ROAM_PREF_AFFIL    = 0x03,
    
    /*! Acquire systems regardless of their roaming indication */
    CNS_ROAM_PREF_ANY      = 0xFF,
    
    /*! Do not change the roam preference */
    CNS_ROAM_PREF_NO_CHANGE,

    CNS_ROAM_PREF_MAX      /*!< FOR INTERNAL USE OF CM ONLY! */

}SWI_TYPE_Roam_Pref;

/**
 *
 * An enumeration of NV pref op mode.
 *
 */
typedef enum SWI_TYPE_NvPrefOpMode
{
    SWI_NV_PREF_OP_MODE_CdmaThenAnalog,     /*!< Not supported */
    SWI_NV_PREF_OP_MODE_CdmaOnly,           /*!< 2G CDMA */
    SWI_NV_PREF_OP_MODE_AnalogThenCdma,     /*!< Not supported */
    SWI_NV_PREF_OP_MODE_AnalogOnly,         /*!< Not supported */
    SWI_NV_PREF_OP_MODE_Automatic,          /*!< Automatic */
    SWI_NV_PREF_OP_MODE_Emergency,          /*!< Emergency */
    SWI_NV_PREF_OP_MODE_RestrictHomeOnly,   /*!< Restrict Home Only */
    SWI_NV_PREF_OP_MODE_PcsCdmaOnly,        /*!< 1900Mhz CDMA Only */
    SWI_NV_PREF_OP_MODE_CellCdmaOnly,       /*!< 800Mhz CDMA Only */
    SWI_NV_PREF_OP_MODE_Cdma1xOnly,         /*!< 1xRTT CDMA Only */
    SWI_NV_PREF_OP_MODE_CdmaEvdoOnly        /*!< 1xEVDO CDMA Only */
}SWI_TYPE_NvPrefOpMode;

/**
 *
 * Contains one active set data structure.
 *
 */
typedef struct SWI_STRUCT_TechActiveSetSingle
{
    swi_uint32 sizeStruct;      /*!< Size of this structure */
    swi_int32 iPilot;           /*!< Pilot offset */
    swi_int32 iEcio;            /*!< Pilot strength (Ec/Io)*/
}SWI_STRUCT_TechActiveSetSingle;

/**
 *
 * Contains whole active set data structure.
 *
 * \sa SWI_STRUCT_TechActiveSetSingle
 *
 */
typedef struct SWI_STRUCT_TechActiveSet
{
    swi_uint32 sizeStruct;  /*!< Size of this structure */
    swi_uint32 nCount;      /*!< Number of active set */

    /*! Active set array */
    SWI_STRUCT_TechActiveSetSingle ActiveSet[SWI_MAX_ActiveSet];
}SWI_STRUCT_TechActiveSet;

/**
 *
 * Contains MIP enabling data structure.
 *
 */
typedef struct SWI_STRUCT_MipEnabled
{
    swi_uint32  sizeStruct;     /*!< Size of this structure */

    /*! Indicates whether MIP is enabled */
    swi_uint16  nMipEnabled[SWI_MAX_ActiveSet];
}SWI_STRUCT_MipEnabled;

/**
 *
 * Contains RSSI and EcIo data structure.
 *
 * This data structure is used to hold RSSI and EcIo data.
 *
 */
typedef struct SWI_STRUCT_Rssi_Ecio
{
    swi_uint32  sizeStruct;         /*!< Size of this structure */
    swi_uint16  iCdmaRssi;          /*!< Cdma RSSI */   
    swi_uint16  iCdmaEcio;          /*!< Cdma EcIo */
    swi_uint16  iHdrRssi;           /*!< HDR RSSI */
    swi_uint16  iHdrEcio;           /*!< HDR EcIo */
}SWI_STRUCT_Rssi_Ecio;

/**
 *
 * An enumeration of EVDO/HDR roaming state.
 *
 * This type definition covers the possible return values of the function 
 * SwiGetHdrRoamStatus, and the SWI_NOTIFY_Hdr_Roam_Status notification.
 *
 * Values above 2 are carrier dependent.
 *
 */
typedef enum SWI_TYPE_Hdr_Roam_Status
{
    /*! Not Roaming */
    SWI_HDR_ROAM_STATUS_NO_ROAMING                          = 0x00000000,

    /*! Roaming (preferred)*/
    SWI_HDR_ROAM_STATUS_ROAMING_WITH_GAURANTEED_VALID_SIDS  = 0x00000001,

    /*! Roaming (non-preferred) */
    SWI_HDR_ROAM_STATUS_ROAMING_WITHOUT_GAURANTEED_SIDS     = 0x00000002
}SWI_TYPE_Hdr_Roam_Status;

/**
 *
 * Contains EVDO/HDR roaming state data structure.
 *
 * \sa SWI_TYPE_Hdr_Roam_Status
 *
 */
typedef struct SWI_STRUCT_Hdr_Roam_Status
{
    swi_uint32 sizeStruct;      /*!< Size of this structure */

    /*! EVDO/HDR roaming state. See SWI_TYPE_Hdr_Roam_Status */
    swi_uint16 iHdrRoamStatus;
}SWI_STRUCT_Hdr_Roam_Status;

/**
 *
 * An enumeration of EVDO/HDR service state.
 *
 */
typedef enum SWI_TYPE_Hdr_Srv_State
{
    /*! No service */
    SWI_HDR_SRV_STATE_NO_SERVICE      = 0x00000000,

    /*! Digital service */
    SWI_HDR_SRV_STATE_DIGITAL_SERVICE = 0x00000002,

    /*! Digital service Rev 0 */
    SWI_HDR_SRV_STATE_DIGITAL_SERVICE_Rev0  = 
                                            SWI_HDR_SRV_STATE_DIGITAL_SERVICE,

    /*! Digital service Rev A */
    SWI_HDR_SRV_STATE_DIGITAL_SERVICE_RevA  = 0x00000004
}SWI_TYPE_Hdr_Srv_State;

/**
 *
 * Contains EVDO/HDR roaming state data structure.
 *
 * \sa SWI_TYPE_Hdr_Srv_State
 *
 */
typedef struct SWI_STRUCT_Hdr_Srv_State
{
    swi_uint32 sizeStruct;      /*!< Size of this structure */
    
    /*! HDR service state. See SWI_TYPE_Hdr_Srv_State. */
    swi_uint16  iHdrSrvState;
}SWI_STRUCT_Hdr_Srv_State;

/**
 *
 * An enumeration of EVDO/HDR Hybrid pref.
 *
 */
typedef enum SWI_TYPE_Hdr_Hybr_Pref
{
    SWI_HDR_HYBR_PREF_MODE_OFF  = 0x00000000,   /*!< Hybrid pref mode off */
    SWI_HDR_HYBR_PREF_MODE_ON   = 0x00000001    /*!< Hybrid pref mode on */
}SWI_TYPE_Hdr_Hybr_Pref;

/**
 *
 * Contains EVDO/HDR Hybrid pref mode data structure.
 *
 * \sa SWI_TYPE_Hdr_Hybr_Pref
 *
 */
typedef struct SWI_STRUCT_Hdr_Hybr_Pref
{
    swi_uint32 sizeStruct;      /*!< Size of this structure */

    /*! HDR Hybrid pref mode. See SWI_TYPE_Hdr_Hybr_Pref. */
    swi_uint16 iHdrHybrPref;   
}SWI_STRUCT_Hdr_Hybr_Pref;

/**
 *
 * An enumeration of power save mode.
 *
 */
typedef enum SWI_TYPE_TechPowersaveMode
{
    SWI_TECH_POWERSAVE_MODE_OFF,     /*!< Off */
    SWI_TECH_POWERSAVE_MODE_ON       /*!< On */
}SWI_TYPE_TechPowersaveMode;

/**
 *
 * An enumeration of system preference.
 *
 */
typedef enum SWI_TYPE_NvSystemPref
{
    SWI_NV_SYSTEM_PREF_AOnly,           /*!< System A only */
    SWI_NV_SYSTEM_PREF_BOnly,           /*!< System B only */
    SWI_NV_SYSTEM_PREF_HomeOnly,        /*!< Home only */
    SWI_NV_SYSTEM_PREF_HomePreferred,   /*!< Home prefered */
    SWI_NV_SYSTEM_PREF_AutoA,           /*!< Automatic A */
    SWI_NV_SYSTEM_PREF_AutoB            /*!< Automatic B */
}SWI_TYPE_NvSystemPref;

/* Function prototypes */

SWI_API SWI_RCODE SwiGetRssi(swi_int32 *iRssi, swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetChannelNumber(swi_uint32 *nChannelNumber, 
                                      swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetServiceIndication(swi_int32* lServiceIndicator, 
                                          swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetCurrentBaseStation(swi_uint16 *P_PRV, 
                                           swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetRoamingStatus(swi_uint32* lRoamingStatus, 
                                      swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetTechPowersaveMode(swi_uint16 *powersaveMode, 
                                          swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetTechFrame(swi_uint32* totalFrames, 
                                  swi_uint32* badFrames, 
                                  swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetTransmitPower(swi_int16* TxPower, 
                                      swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetCallPrivacy(swi_uint16* privacyMode, 
                                    swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetByteCounter(swi_uint32* pSent, 
                                    swi_uint32* pReceived, 
                                    swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetTechBSInfo(swi_int32 *longitude, 
                                   swi_int32 *latitude, 
                                   swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetMobileDirNum(swi_uint16 Nam, 
                                     swi_charp MDN, 
                                     swi_uint32 length, 
                                     swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetHomeSidNid(swi_uint16 Nam, 
                                      SWI_STRUCT_HomeSidNid *HomeSidNid, 
                                      swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetSystemPref(swi_uint16 Nam, 
                                      swi_uint16 *PrefServ, 
                                      swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetSipUserId(swi_charp pszUserId, 
                                     swi_uint32 szLength, 
                                     swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetSipPassword(swi_charp pszPassword, 
                                       swi_uint32 szLength, 
                                       swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetSip2UserId(swi_charp pszUserId, 
                                      swi_uint32 szLength, 
                                      swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetSip2Password(swi_charp pszPassword, 
                                        swi_uint32 szLength, 
                                        swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetDsMipGenUser(swi_uint16 index, 
                                        swi_uint16 naiLength, 
                                        swi_charp nai, 
                                        swi_uint16 mnHaSpiEnable, 
                                        swi_uint32 mnHaSpi, 
                                        swi_uint16 mnAaaSpiEnable, 
                                        swi_uint32 mnAaaSpi, 
                                        swi_uint16 reverseTunneling, 
                                        swi_uint32 homeAddress, 
                                        swi_uint32 primaryHaAddress, 
                                        swi_uint32 secondaryHaAddress, 
                                        swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetDsMipGenUser(swi_uint16 index, 
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

SWI_API SWI_RCODE SwiPstGetDsQcMip(swi_uint16* pBehaviour, 
                                   swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetMobileDirNum(swi_uint16 Nam, 
                                     const swi_charp MDN, 
                                     swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetHomeSidNid(
        swi_uint16 Nam, 
        const SWI_STRUCT_HomeSidNid *pHomeSidNid, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetSystemPref(swi_uint16 Nam, 
                                      swi_uint16 PrefServ, 
                                      swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetSipUserId(const swi_charp pszUserId, 
                                     swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetSipPassword(const swi_charp pszPassword, 
                                       swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetSip2UserId(const swi_charp pszUserId, 
                                      swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetSip2Password(const swi_charp pszPassword, 
                                        swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetCallConnectionState(swi_uint16* CallState, 
                                            swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetCallConnectionStateEx(
        SWI_STRUCT_CallConnected* CallState, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetCallOriginateCmd(swi_uint16 callType, 
                                         const swi_charp szCallNumber, 
                                         swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetCallDisconnectCmd();

SWI_API SWI_RCODE SwiGetCallDuration(swi_uint32* pCallDuration, 
                                     swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetDsMipActiveProf(swi_uint16* pActive, 
                                           swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetDsMipActiveProf(swi_uint16 active, 
                                           swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetMipErrCode(swi_uint16 *nErrCode, swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetMipErrCodeEx(swi_uint16 *nErrCode1, 
                                     swi_uint16 *nErrCode2, 
                                     swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetServiceAreaAlert(swi_uint16* srvAreaAlert, 
                                            swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetServiceAreaAlert(swi_uint16 srvAreaAlert, 
                                            swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetTempModePref(swi_uint16 nModePref, 
                                     swi_uint16 nModeTerm, 
                                     swi_uint32 nModeDur, 
                                     swi_uint32 nBandPref, 
                                     swi_uint16 nRoamPref, 
                                     swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetRssiEcio(SWI_STRUCT_Rssi_Ecio* stRssiEcio, 
                                 swi_uint32 timeout);

SWI_API SWI_RCODE SwiEVDOGetRssi(swi_int32* lRssi, swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetHdrRoamStatus(swi_uint16* iHdrRoamStatus, 
                                      swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetHdrSrvState(swi_uint16* iHdrSrvState, 
                                    swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetHdrHybrPref(swi_uint16* iHdrHybrPref, 
                                    swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetHdrHybrPref(swi_uint16 iHdrHybrPref, 
                                    swi_uint32 timeout);

SWI_API SWI_RCODE SwiHdrCloseSession(swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetHdrNotificationStatus(
        swi_uint16* iHdrNotificationStatus,
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetTechHdrNotificationStatus(
        swi_uint16* iTechHdrNotificationStatus,
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetHdrUserId(swi_charp pszUserId, 
                                     swi_uint32 szLength, 
                                     swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetHdrUserId(const swi_charp pszUserId, 
                                     swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetHdrPassword(swi_charp pszPassword, 
                                       swi_uint32 szLength, 
                                       swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetHdrPassword(const swi_charp pszPassword, 
                                       swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetAutoconnect(swi_uint16* pnEnable, swi_uint32 timeout);
SWI_API SWI_RCODE SwiSetAutoconnect(swi_uint16 nEnable, swi_uint32 timeout);
SWI_API SWI_RCODE SwiPstSetDsQcMip(swi_uint16 behaviour, swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetPrefMode(swi_uint16 Nam, 
                                    swi_uint16 *PrefMode, 
                                    swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetPrefMode(swi_uint16 Nam, 
                                    swi_uint16 PrefMode, 
                                    swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetTechPacketZone(swi_uint16 *packetZoneID, 
                                       swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetTechSID(swi_uint16 *systemID, swi_uint32 timeout);
SWI_API SWI_RCODE SwiGetTechNID(swi_uint16 *networkID, swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetTermSid(swi_uint16 Nam, 
                                   swi_uint16 *TermSidEnable, 
                                   swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetTermSid(swi_uint16 Nam, 
                                   swi_uint16 TermSidEnable, 
                                   swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetTermNid(swi_uint16 Nam, 
                                   swi_uint16 *TermNidEnable, 
                                   swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetTermNid(swi_uint16 Nam, 
                                   swi_uint16 TermNidEnable, 
                                   swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetTechActiveSet(SWI_STRUCT_TechActiveSet* pTechActSet, 
                                      swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetDsMipRetries(swi_uint16 retries, 
                                        swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetDsMipRetries(swi_uint16* pRetries, 
                                        swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetDsMipRetryInt(swi_uint16 interval, 
                                         swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetDsMipRetryInt(swi_uint16* pInterval, 
                                         swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetDsMipPrereRrqTime(swi_uint16 rereg, 
                                             swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetDsMipPrereRrqTime(swi_uint16* pRereg, 
                                             swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetDsMipQcDrsOpt(swi_uint16 active, 
                                         swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetDsMipQcDrsOpt(swi_uint16* pActive, 
                                         swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetDsMip2002BisMnHaAuth(swi_uint16 rfc2002, 
                                                swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetDsMip2002BisMnHaAuth(swi_uint16* pRfc2002, 
                                                swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetDsMipRrqIfTfrk(swi_uint16 reregistration, 
                                          swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetDsMipRrqIfTfrk(swi_uint16* pReregistration, 
                                          swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetDsMipEnableProf(SWI_STRUCT_MipEnabled* sMipEnabled, 
                                           swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetDsMipEnableProf(
        const SWI_STRUCT_MipEnabled *psMipEnabled, 
        swi_uint32 timeout);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __SWIAPICDMANETWORK_H__ */

/* 
 * $Log: SwiApiCdmaNetwork.h,v $
 */

