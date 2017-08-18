/**
 * \if CvsId
 * $Id: SwiApiGsmVoice.h,v 1.7 2010/06/10 00:20:27 epasheva Exp $
 * \endif
 * \ingroup gsmvoice
 * \file SwiApiGsmVoice.h
 * 
 * \brief This file contains definitions, enumerations, structures and
 *        forward declarations for SwiApiGsmVoice.c file
 *
 * Copyright (c) 2009 Sierra Wireless, Inc.  All rights reserved
 */

#ifndef __SWIAPI_GSM_VOICE
#define __SWIAPI_GSM_VOICE

#ifdef __cplusplus
extern "C" {
#endif

#define LENGTH_MaxPhone             20
#define LENGTH_CallerNameVoice      (80 * UTF8_MAX_CHAR_SIZE) + 1
#define MAX_CALLS                   7
#define LENGTH_ForwardNumber        38
#define LENGTH_SSString             40
#define LENGTH_SSPassword           4
#define LENGTH_ForwardInfo          18
#define MAX_FWD_INFO_ENTRIES        8
#define MAX_INFO_DATA               70
#define MAX_BS_LIST                 70
#define LENGTH_RawData              229

/* include files */
#include "SwiDataTypes.h"
#include "SwiRcodes.h"
#include "SwiApiGsmSs.h"

/* enum declarations */
/**
 *   This enumeration contains all the Supplementary Service Service Status
 */
enum SWI_TYPE_SS_ServiceStatus
{
    SWI_STATUS_ServActiveMask       = 0x01,
    SWI_STATUS_ServRegisteredMask   = 0x02,
    SWI_STATUS_ServProvMask         = 0x04,
    SWI_STATUS_ServQuiescentMask    = 0x08
};

/**
 *   This enumeration contains all the Supplementary Service Subscription Option
 *   Type
 */
enum SWI_TYPE_SS_SubsOptionType
{
    SWI_SUBSOPTIONTYPE_Override             = 0x01,
    SWI_SUBSOPTIONTYPE_CLIR
};

/**
 *   This enumeration contains all the Supplementary Service Subscription Option
 *   Code
 */
enum SWI_TYPE_SS_SubsOptionCode
{
    SWI_SUBSOPTIONCODE_OverrideEnabled      = 0x00,
    SWI_SUBSOPTIONCODE_OverrideDisabled     = 0x01,
    SWI_SUBSOPTIONCODE_ClirPermanent        = 0x00,
    SWI_SUBSOPTIONCODE_TempCallIdRestrict   = 0x01,
    SWI_SUBSOPTIONCODE_TempCallIdAllow      = 0x02
};

/**
 *   This enumeration contains all the Supplementary Service Forwarding Option
 */
enum SWI_TYPE_SS_ForwardOption
{
    SWI_FORWARDOPTION_NotifyFwdParyMask     = 0x80,
    SWI_FORWARDOPTION_NotifyCalledPartyMask = 0x20,
    SWI_FORWARDOPTION_MobileNotReachMask    = 0x00,
    SWI_FORWARDOPTION_FwdBusyMask           = 0x04,
    SWI_FORWARDOPTION_FwdNotReach           = 0x08,
    SWI_FORWARDOPTION_FwdAllCalls           = 0x0C
};

/**
 *   This enumeration contains all the Supplementary Service CLIR Setting
 */
enum SWI_TYPE_SS_CLIRSetting
{
    SWI_CLIRSETTING_Permanent,
    SWI_CLIRSETTING_TempCallIdRestrict,
    SWI_CLIRSETTING_TempCallIdAllow
};

/**
 *   This enumeration contains all the Supplementary Service Error Code
 */
enum SWI_TYPE_SS_ErrorCode
{
    SWI_ERRORCODE_UnknownSubs       = 0x01,
    SWI_ERRORCODE_IllegalSubs       = 0x09,
    SWI_ERRORCODE_BearServNotProv,  /* 0x0A */
    SWI_ERRORCODE_TellServNotProv,  /* 0x0B */
    SWI_ERRORCODE_IllegalEquip,     /* 0x0C */
    SWI_ERRORCODE_CallBarred,       /* 0x0D */
    SWI_ERRORCODE_IllegalOper       = 0x10,
    SWI_ERRORCODE_ErrorStatus,      /* 0x11 */
    SWI_ERRORCODE_NotAvail,         /* 0x12 */
    SWI_ERRORCODE_SubsViolate,      /* 0x13 */
    SWI_ERRORCODE_Incompatible,     /* 0x14 */
    SWI_ERRORCODE_FacilityNotSup,   /* 0x15 */
    SWI_ERRORCODE_AbsentSubs        = 0x1B,
    SWI_ERRORCODE_SystemFail        = 0x22,
    SWI_ERRORCODE_DataMissing,      /* 0x23 */
    SWI_ERRORCODE_UnexpectValue,    /* 0x24 */
    SWI_ERRORCODE_PasswdRegFail,    /* 0x25 */
    SWI_ERRORCODE_NegPasswdChk,     /* 0x26 */
    SWI_ERRORCODE_NumPasswdViolate  = 0x2B,
    SWI_ERRORCODE_UnknownAlphabet   = 0x47,
    SWI_ERRORCODE_UssdBusy,         /* 0x48 */
    SWI_ERRORCODE_MaxMultiExceed    = 0x7E,
    SWI_ERRORCODE_ResourceNotAvail /* 0x7F */
};

/**
 *   This enumeration contains all the Supplementary Service Error Type
 */
enum SWI_TYPE_SS_ErrorType
{
    SWI_ERRORTYPE_NoInfo,
    SWI_ERRORTYPE_CallBarring,
    SWI_ERRORTYPE_Status,
    SWI_ERRORTYPE_SubsViolate,
    SWI_ERRORTYPE_AbsentSubs,
    SWI_ERRORTYPE_SystemFail,
    SWI_ERRORTYPE_PasswdRegFail,
    SWI_ERRORTYPE_Incompatible
};

/**
 *   This enumeration contains all the Supplementary Service Car Barring Status
 */
enum SWI_TYPE_SS_CallBar
{
    SWI_CALLBAR_Active,
    SWI_CALLBAR_OperatorBarring
};

/**
 *   This enumeration contains all the Supplementary Service System can fail
 */
enum SWI_TYPE_SS_SystemFail
{
    SWI_SYSTEMFAIL_Plmn,
    SWI_SYSTEMFAIL_Hlr,
    SWI_SYSTEMFAIL_Vlr,
    SWI_SYSTEMFAIL_Pvlr,
    SWI_SYSTEMFAIL_ControlMsc,
    SWI_SYSTEMFAIL_VisitMsc,
    SWI_SYSTEMFAIL_Eir,
    SWI_SYSTEMFAIL_Rss
};

/**
 *   This enumeration contains all the reasons Supplementary Service Password
 *   can fail
 */
enum SWI_TYPE_SS_PasswdRegFail
{
    SWI_PASSWDFAILREASON_Undetermined,
    SWI_PASSWDFAILREASON_InvalidFormat,
    SWI_PASSWDFAILREASON_NewMismatch
};

/**
 *   This enumeration contains all the Supplementary Service Problem Type
 */
enum SWI_TYPE_SS_ProblemType
{
    SWI_PROBLEMTYPE_General,
    SWI_PROBLEMTYPE_Invoke,
    SWI_PROBLEMTYPE_ReturnResult,
    SWI_PROBLEMTYPE_ReturnError
};

/**
 *   This enumeration contains all the Supplementary Service Reject Codes
 */
enum SWI_TYPE_SS_RejectCode
{
    SWI_REJECTCODE_GeneralUnrecognize       = 0x00,
    SWI_REJECTCODE_GeneralMistype           = 0x01,
    SWI_REJECTCODE_GeneralBadStruct         = 0x02,
    SWI_REJECTCODE_InvokeDupId              = 0x00,
    SWI_REJECTCODE_InvokeUnrecognizeId      = 0x01,
    SWI_REJECTCODE_InvokeMistype            = 0x02,
    SWI_REJECTCODE_InvokeResource           = 0x03,
    SWI_REJECTCODE_InvokeInitRelease        = 0x04,
    SWI_REJECTCODE_InvokeUnrecognizeLink    = 0x05,
    SWI_REJECTCODE_InvokeLinkRspUnexpect    = 0x06,
    SWI_REJECTCODE_InvokeLinkOpUnexpect     = 0x07,
    SWI_REJECTCODE_ResultUnrecognizeId      = 0x00,
    SWI_REJECTCODE_ResultUnexpected         = 0x01,
    SWI_REJECTCODE_ResultMistype            = 0x02,
    SWI_REJECTCODE_ErrorUnrecognizeId       = 0x00,
    SWI_REJECTCODE_ErrorUnexpected          = 0x01,
    SWI_REJECTCODE_ErrorUnrecognizedErr     = 0x02,
    SWI_REJECTCODE_ErrorUnexpectedErr       = 0x03,
    SWI_REJECTCODE_ErrorMistype             = 0x04
};

/**
 *   This enumeration contains all the Supplementary Service Cancel Reasons
 */
enum SWI_TYPE_SS_CancelReason
{
    SWI_CANCELREASON_NoPipes,
    SWI_CANCELREASON_OngoingTrans,
    SWI_CANCELREASON_MnssReject,
    SWI_CANCELREASON_NoControl,
    SWI_CANCELREASON_InvalidId
};


/**
 *   This enumeration contains all the Supplementary Service Password Guidance
 */
enum SWI_TYPE_SS_PasswordGuidance
{
    SWI_SSPASSWORDGUIDANCE_Password,
    SWI_SSPASSWORDGUIDANCE_NewPassword,
    SWI_SSPASSWORDGUIDANCE_NewPasswordAgain,
    SWI_SSPASSWORDGUIDANCE_BadPassword,
    SWI_SSPASSWORDGUIDANCE_BadPasswordFormat
};

/**
 *   This enumeration contains all the Supplementary Service Service Code
 */
enum SWI_TYPE_SS_ServiceCode
{
    SWI_SERVCODE_AllServices,
    SWI_SERVCODE_AllLineIdServices      = 0x10,
    SWI_SERVCODE_CallerIdPresent,           /* 0x11 */
    SWI_SERVCODE_CallerIdRestrict,          /* 0x12 */
    SWI_SERVCODE_ConnectIdPresent,          /* 0x13 */
    SWI_SERVCODE_ConnectIdRestrict,         /* 0x14 */
    SWI_SERVCODE_MCI,                       /* 0x15 */
    SWI_SERVCODE_CallingNamePresent     = 0x19,
    SWI_SERVCODE_CallFwdAllServices     = 0x20,
    SWI_SERVCODE_CallFwdUnconditional,      /* 0x21 */
    SWI_SERVCODE_CallFwdAllConditional  = 0x28,
    SWI_SERVCODE_CallFwdBusy,               /* 0x29 */
    SWI_SERVCODE_CallFwdNoReply         = 0x2A,
    SWI_SERVCODE_CallFwdNotReachable,       /* 0x2B */
    SWI_SERVCODE_AllCallOffering        = 0x30,
    SWI_SERVCODE_CT,                        /* 0x31 */
    SWI_SERVCODE_MAH,                       /* 0x32 */
    SWI_SERVCODE_AllCallComplete        = 0x40,
    SWI_SERVCODE_CallWaiting,               /* 0x41 */
    SWI_SERVCODE_CallHold,                  /* 0x42 */
    SWI_SERVCODE_CompletionBusySubsciber,   /* 0x43 */
    SWI_SERVCODE_AllMultiparty          = 0x50,
    SWI_SERVCODE_Multiply,                  /* 0x51 */
    SWI_SERVCODE_ALL_COMM_OF_INT        = 0x60,
    SWI_SERVCODE_ClosedUserGroup,           /* 0x61 */
    SWI_SERVCODE_AllCharging            = 0x70,
    SWI_SERVCODE_AdviceChargeInfo,          /* 0x71 */
    SWI_SERVCODE_AdviceChargeCharging,      /* 0x72 */
    SWI_SERVCODE_AllAdditionalInfo      = 0x80,
    SWI_SERVCODE_UserToUserSignalling,      /* 0x81 */
    SWI_SERVCODE_AllCallRestriction     = 0x90,
    SWI_SERVCODE_BarrOutgoing,              /* 0x91 */
    SWI_SERVCODE_BarrAllOutgoing,           /* 0x92 */
    SWI_SERVCODE_BarrOutgoingInter,         /* 0x93 */
    SWI_SERVCODE_BarrOutgoingInterExcept,   /* 0x94 */
    SWI_SERVCODE_BarrIncoming           = 0x99,
    SWI_SERVCODE_BarrAllIncoming,           /* 0x9A */
    SWI_SERVCODE_BarrIncomingRoam,          /* 0x9B */
    SWI_SERVCODE_PLMNAllSpecific        = 0xF0,
    SWI_SERVCODE_PLMNSpecific1,             /* 0xF1 */
    SWI_SERVCODE_PLMNSpecific2,             /* 0xF2 */
    SWI_SERVCODE_PLMNSpecific3,             /* 0xF3 */
    SWI_SERVCODE_PLMNSpecific4,             /* 0xF4 */
    SWI_SERVCODE_PLMNSpecific5,             /* 0xF5 */
    SWI_SERVCODE_PLMNSpecific6,             /* 0xF6 */
    SWI_SERVCODE_PLMNSpecific7,             /* 0xF7 */
    SWI_SERVCODE_PLMNSpecific8,             /* 0xF8 */
    SWI_SERVCODE_PLMNSpecific9,             /* 0xF9 */
    SWI_SERVCODE_PLMNSpecificA,             /* 0xFA */
    SWI_SERVCODE_PLMNSpecificB,             /* 0xFB */
    SWI_SERVCODE_PLMNSpecificC,             /* 0xFC */
    SWI_SERVCODE_PLMNSpecificD,             /* 0xFD */
    SWI_SERVCODE_PLMNSpecificE,             /* 0xFE */
    SWI_SERVCODE_PLMNSpecificF              /* 0xFF */
};

/**
 *   This enumeration contains all the Supplementary Service Basic Service 
 *   Group Type
 */
enum SWI_TYPE_SS_BasicServGrpType
{
    SWI_BASICSERVGRPTYPE_TeleServices,
    SWI_BASICSERVGRPTYPE_BearerServices
};

/**
 *   This enumeration contains all the Supplementary Service request types.
 */
enum SWI_TYPE_SS_RequestType
{
    SWI_SSREQUESTTYPE_Erase             = 0x01,
    SWI_SSREQUESTTYPE_Activate,
    SWI_SSREQUESTTYPE_Deactivate,
    SWI_SSREQUESTTYPE_Interrogate,
    SWI_SSREQUESTTYPE_Register
};

/**
 *   This enumeration contains the actions that can be performed on a call. 
 */
enum SWI_TYPE_UMTS_CallControl
{
    SWI_CALL_CONTROL_UMTS_RelAllHeld,            /*!< release all Held Calls. */
    SWI_CALL_CONTROL_UMTS_RelAllActAccHeldWait,   /*!< release all Active Calls and accept held or waiting call. */
    SWI_CALL_CONTROL_UMTS_ReleaseCallX,           /*!< release call X. */
    SWI_CALL_CONTROL_UMTS_HoldAllActAccHeldWait,  /*!< hold all Active Calls and accept held or waiting call. */
    SWI_CALL_CONTROL_UMTS_HoldAllActExceptCallX,  /*!< hold all Active Calls except X. */
    SWI_CALL_CONTROL_UMTS_AddHeldCall,            /*!< add held call to the conversation. */
    SWI_CALL_CONTROL_UMTS_ConTwoCallsAndDiscon,   /*!< connect two calls together and disconnect from the conservation. */
    SWI_CALL_CONTROL_UMTS_DisconnectAll           /*!< disconnect All Calls. */
};

/**
 *   This enumeration contains all possible call states.
 */
enum SWI_TYPE_GSM_CallState
{
    SWI_CALLSTATE_Null,          /*!< null state. */
    SWI_CALLSTATE_Connecting,    /*!< connecting. */
    SWI_CALLSTATE_Queued,        /*!< queued. */
    SWI_CALLSTATE_Waiting,       /*!< waiting. */
    SWI_CALLSTATE_Alerting,      /*!< alerting. */
    SWI_CALLSTATE_Connected,     /*!< connected. */
    SWI_CALLSTATE_Disconnecting  /*!< disconnecting. */
};

/**
 *   This enumeration contains all the call progress states.
 */
enum SWI_TYPE_CallProgress
{
    SWI_CALL_PROGRESS_Failed,           /*!< the call has failed. */
    SWI_CALL_PROGRESS_Connecting,       /*!< the call is connecting. */
    SWI_CALL_PROGRESS_Queued,           /*!< this applies to outgoing calls only 
                                        *  and indicates that this is a "call waiting" 
                                        *  on the other party's phone. */
    SWI_CALL_PROGRESS_Waiting,          /*!< the call waiting feature must be 
                                        *  provisioned to use this. */
    SWI_CALL_PROGRESS_Incoming,         /*!< there is an incoming call. */
    SWI_CALL_PROGRESS_Ringing,          /*!< the incoming call is ringing. */
    SWI_CALL_PROGRESS_Connected,        /*!< the call is connected. */
    SWI_CALL_PROGRESS_Disconnecting,    /*!< the call is disconnecting. */
    SWI_CALL_PROGRESS_Disconnected,     /*!< the call is disconnected. */
    SWI_CALL_PROGRESS_Forwarded         /*!< this applies to incoming calls only 
                                        *  and indicates the call has been forwarded 
                                        *  to the modem by another phone.. */
};

/**
 *   This enumeration contains all the call error states.
 */
enum SWI_TYPE_GSM_CallError
{
    SWI_CALL_ERROR_OK,                      /*!< OK - no error. */
    SWI_CALL_ERROR_SubsciberBusy,           /*!< subscriber busy. */
    SWI_CALL_ERROR_Congestions,             /*!< congestion. */
    SWI_CALL_ERROR_RadioPathUnavail,        /*!< radio path unavailable. */
    SWI_CALL_ERROR_Error,                   /*!< general error. */
    SWI_CALL_ERROR_NumberUnobtain,          /*!< number unobtainable. */
    SWI_CALL_ERROR_AuthFailure,             /*!< authentication failure. */
    SWI_CALL_ERROR_NoNumber,                /*!< no number present. */
    SWI_CALL_ERROR_BckgndCallCtrlFail,      /*!< background call control failure. */
    SWI_CALL_ERROR_BckgndMMFail,            /*!< background mobility management failure. */
    SWI_CALL_ERROR_ChannelBusy,             /*!< channel busy. */
    SWI_CALL_ERROR_FDNMismatch,             /*!< FDN mismatch. */
    SWI_CALL_ERROR_BCFail,                  /*!< BC fail. */
    SWI_CALL_ERROR_EmergencyOnly,           /*!< emergency calls only. */
    SWI_CALL_ERROR_ACMExceeded,             /*!< ACM limit exceeded. */
    SWI_CALL_ERROR_HoldError,               /*!< hold error. */
    SWI_CALL_ERROR_BusyProcReq,             /*!< busy processing request. */
    SWI_CALL_ERROR_ChannelUnavail,          /*!< active channel unavailable. */
    SWI_CALL_ERROR_OutgoingBarred,          /*!< outgoing calls barred. */
    SWI_CALL_ERROR_NumberBlacklisted,       /*!< number blacklisted. */
    SWI_CALL_ERROR_BlackListFull,           /*!< black list full. */
    SWI_CALL_ERROR_RedialNotTimedOut,       /*!< redial not timed out. */
    SWI_CALL_ERROR_ModifyFail,              /*!< modify failed. */
    SWI_CALL_ERROR_NotInControl,            /*!< not in control. */
    SWI_CALL_ERROR_NoCauseCode,             /*!< no cause code. */
    SWI_CALL_ERROR_CallBarred               /*!< call barred. */
};

/**
 *   This enumeration contains all possible call presentation settings. 
 */
enum SWI_TYPE_CallPresentation
{
    SWI_CALL_PRESENTATION_Number,       /*!< present number. */
    SWI_CALL_PRESENTATION_Restricted,   /*!< presentation restricted. */
    SWI_CALL_PRESENTATION_NoAvail       /*!< number not available. */
};

/**
 *   This enumeration contains all possible call screening settings. 
 */
enum SWI_TYPE_CallScreening
{
    SWI_CALL_SCREENING_NotScreened,     /*!< user not screened. */
    SWI_CALL_SCREENING_Passed,          /*!< user provided passed. */
    SWI_CALL_SCREENING_Failed,          /*!< user provided failed. */
    SWI_CALL_SCREENING_Network          /*!< network provided. */
};

/**
 *   This enumeration contains all possible call types.
 */
enum SWI_TYPE_GSM_CallType
{
    SWI_CALL_TYPE_VoiceCallLine1,   /*!< voice call line 1. */
    SWI_CALL_TYPE_VoiceCallLine2,   /*!< voice call line 2. */
    SWI_CALL_TYPE_CSD,              /*!< circuit switched data. */
    SWI_CALL_TYPE_FAX               /*!< FAX. */
};

/**
 * This enumeration contains call information states.
 */
enum SWI_TYPE_CallValid
{
    SWI_CALL_VALID_NoInfo,          /*!< no number information. */
    SWI_CALL_VALID_InfoValid        /*!< phone number information. */
};


/**
 * This enumeration contains all possible call restriction settings. 
 */
enum SWI_TYPE_CallRestrict
{
    SWI_CALL_RESTRICT_Default,     /*!< default. */
    SWI_CALL_RESTRICT_Allowed,     /*!< Caller ID allowed. */
    SWI_CALL_RESTRICT_Restricted,  /*!< Caller ID restricted. */
    SWI_CALL_RESTRICT_Unknown      /*!< Caller ID unknown. */
};

/**
 * This enumeration contains the tone lengths used for DTMF over-dial digits.
 */
enum SWI_TYPE_ToneLength
{
    SWI_TONELENGTH_Short,       /*!< short DTMF tone length. */
    SWI_TONELENGTH_Medium,      /*!< medium DTMF tone length. */
    SWI_TONELENGTH_Long,        /*!< long DTMF tone length. */
    SWI_TONELENGTH_UserDefined  /*!< user defined DTMF tone length. */
};

/**
 *   Enumeration of audio profiles
 */
enum SWI_TYPE_AudioProfile
{
    SWI_AUDIOPROFILE_Handset,       /*!< Handset profile. */
    SWI_AUDIOPROFILE_Headset,       /*!< Headset profile. */
    SWI_AUDIOPROFILE_SpeakerPhone,  /*!< Speaker phone profile. */
    SWI_AUDIOPROFILE_CarKit,        /*!< Car kit profile. */
    SWI_AUDIOPROFILE_TtyFullAudio,  /*!< TTY-full audio profile. */
    SWI_AUDIOPROFILE_TtyTalkAudio,  /*!< TTY-talk audio profile. */
    SWI_AUDIOPROFILE_TtyHearAudio   /*!< TTY-hear audio profile. */
};

/**
 *   Enumeration of audio volume levels
 */
enum SWI_TYPE_AudioVolLevel
{
    SWI_AUDIOVOLLEVEL_0,    /*!< Audio volume level 0. */
    SWI_AUDIOVOLLEVEL_1,    /*!< Audio volume level 1. */
    SWI_AUDIOVOLLEVEL_2,    /*!< Audio volume level 2. */
    SWI_AUDIOVOLLEVEL_3,    /*!< Audio volume level 3. */
    SWI_AUDIOVOLLEVEL_4,    /*!< Audio volume level 4. */
    SWI_AUDIOVOLLEVEL_5,    /*!< Audio volume level 5. */
    SWI_AUDIOVOLLEVEL_6,    /*!< Audio volume level 6. */
    SWI_AUDIOVOLLEVEL_7     /*!< Audio volume level 7. */
};

/**
 *   Enumeration of audio generator values
 */
enum SWI_TYPE_AudioGen
{
    SWI_AUDIOGEN_Voice,     /*!< Voice. */
    SWI_AUDIOGEN_KeyBeep,   /*!< Key beep. */
    SWI_AUDIOGEN_Midi       /*!< Midi. */
};

/**
 *   Enumeration of audio action result
 */
enum SWI_TYPE_AudioResult
{
    SWI_AUDIORESULT_Success,        /*!< Valid audio profile setting. */
    SWI_AUDIORESULT_InvalidSetting  /*!< Invalid audio profile setting. */
};

/* structure declarations */
/**
 * This structure is used as input by the SwiSetMicMute function 
 * and output by the SwiGetMicMute function.
 *
 * \sa SwiGetMicMute
 * \sa SwiSetMicMute
 */
struct SWI_STRUCT_MicMute
{
    /*! size of this structure */
    swi_uint32            sizeStruct;      
    /*! microphone mute flag
    *   1 - microphone mute
    *   0 - otherwise */
    swi_bool              bMute;
};

/**
 * This structure is used as input by the SwiSetSpeakerVolume function 
 *          and output by the SwiGetSpeakerVolume function.
 *
 * \sa SwiSetGsmSpeakerVolume 
 * \sa SwiGetGsmSpeakerVolume
 */
struct SWI_STRUCT_GSM_SpeakerVolume
{
    /*! The size, in bytes, of this structure.  This value must always be 
    *   populated by the calling application. */
    swi_uint32  sizeStruct;  

    /*! percentage of maximum value
    *   - valid range 0% to 100% */
    swi_uint32 nVolume;        
};

/**
 * This structure is used as input by the SwiSetSpeakerMute function 
 * and output by the SwiGetSpeakerMute function.
 *
 * \sa SwiGetSpeakerMute
 * \sa SwiSetSpeakerMute
 */
struct SWI_STRUCT_GSM_SpeakerMute
{
    /*! size of this structure */
    swi_uint32           sizeStruct;    
    /*! speaker mute flag
    *   1 - speaker mute
    *   0 - otherwise */
    swi_bool             bMute;
};

/**
 * This structure is used as input by the SwiSetNVToneStore function and
 * output by the SwiGetNVToneStore function.
 *
 * \sa SwiGetNVToneStore
 * \sa SwiSetNVToneStore
 */
struct SWI_STRUCT_NVToneStore
{
    /*! size of this structure */
    swi_uint32                   sizeStruct;
    /*! tone length type: short, medium, long or user defined */
    enum SWI_TYPE_ToneLength     eToneLength;
    /*! 80ms < Range < 3000ms */
    swi_uint32                   nUserDefinedToneOnDuration;
    /*! 80ms < Range < 3000ms */
    swi_uint32                   nUserDefinedToneOffDuration;
    /*! 80ms < Range < 500ms */
    swi_uint32                   nComfortToneOnDuration;
    /*! 80ms < Range < 500ms */
    swi_uint32                   nComfortToneOffDuration;
    /*! earpiece feedback flag
    *   1 - play DTMF to earpiece
    *   0 - do not play DTMF to earpiece */
    swi_bool                     bEarpieceFeedback;
};

/**
 * This structure contains audio profile information
 *
 * \note Audio profile settings are not persistent
 * 
 * \sa SwiGetAudioProfile
 * \sa SwiSetAudioProfile
 */
struct SWI_STRUCT_AudioProfile
{
    /*! size of this structure */
    swi_uint32              sizeStruct;
    /*! Current audio profile setting */
    enum SWI_TYPE_AudioProfile   eCurrent;
    /*! True if ear piece muted, false otherwise */
    swi_bool                bEarPieceMute;
    /*! True if microphone muted, false otherwise */
    swi_bool                bMicMute;
    /*! Volume level */
    enum SWI_TYPE_AudioVolLevel  eVolLevel;
    /*! Audio generator.  Only valid in SET request */
    enum SWI_TYPE_AudioGen       eAudioGen;
    /*! Result of SET request.  Ignore when doing a query. */
    enum SWI_TYPE_AudioResult    eActionResult;
};

/**
 * This structure is used as input by the SwiSetCLIRSetting function and as
 * output by the SwiGetCLIRSetting function.  
 * 
 * \sa SwiGetCLIRSetting
 * \sa SwiSetCLIRSetting
 */
struct SWI_STRUCT_CLIR_Setting
{
    /*! size of this structure. */
    swi_uint32              sizeStruct;     
    /*! call restriction setting. */
    enum SWI_TYPE_CallRestrict   eCallRestrict;  
};

/**
 * This structure is used by the SWI_NOTIFY_VoiceMessage notification. 
 *
 * \sa SwiGetVoiceMessage
 */
struct SWI_STRUCT_VoiceMessage
{
    /*! size of this structure. */
    swi_uint32  sizeStruct;
    /*! SIM supports voice message flag
    *  1 - voice messages supported
    *  0 - otherwise */
    swi_bool    bVoiceMailWaitingFeature;   
    /*! message waiting for line 1 flag 
    *  1 - voice mail waiting for line 1
    *  0 - no voice mail */
    swi_bool    bVoiceMailWaitingLine1;    
    /*! message waiting for line 2 flag
    *  1 - voice mail waiting for line 2
    *  0 - no voice mail */
    swi_bool    bVoiceMailWaitingLine2;     
    /*! FAX message waiting flag 
    *  1 - fax message waiting
    *  0 - no fax messages */
    swi_bool    bFaxMssageWaiting1;         
    /*! Data message waiting flag
    *  1 - data message waiting
    *  0 - no data messages */
    swi_bool    bFaxMssageWaiting2;         
};

/**
 * This structure is used by the SWI_NOTIFY_MissedCall notification 
 * to indicate that there are missed voice calls.
 *
 * \sa SwiAcknowledgeMissedCall
 */
struct SWI_STRUCT_MissedCall
{
    /*! size of this structure. */
    swi_uint32 sizeStruct; 
    /*! number of missed calls. */
    swi_uint32 nCount;    
};

/**
 * This structure is used as input by the SwiSetCallOriginate function.  
 *
 * \sa SwiSetCallOriginate
 */
struct SWI_STRUCT_CALL_Create
{
    /*! size of this structure */
    swi_uint32              sizeStruct;
    /*! call restriction setting. */
    enum SWI_TYPE_CallRestrict   eCallRestrict;
    /*! international phone number flag
    *  1 - international number
    *  0 - otherwise */
    swi_bool                bInternational;                
    /*! number of BCD characters (is this true?). */
    swi_uint32              nNumberLength;
    /*! phone number in BCD. */
    swi_uint8               bcdPhone[LENGTH_MaxPhone];
};

/**
 * This structure is used by the SWI_NOTIFY_CallProgress to report the 
 * the progress of an individual call.
 *
 * \sa SwiUpkGsmCallExpProgress
 */
struct SWI_STRUCT_CALL_EXP_Progress
{
    /*! size of this structure */
    swi_uint32                      sizeStruct;
    /*! call id */
    swi_uint32                      CallID;
    /*! call progress state */
    enum SWI_TYPE_CallProgress      eCallProgress;
    /*! call error code */
    enum SWI_TYPE_GSM_CallError     eCallError;
    /*! phone number information valid */
    enum SWI_TYPE_CallValid         eCallValid;
    /*! call presentation setting */
    enum SWI_TYPE_CallPresentation  eCallPresentation;
    /*! call screening setting */
    enum SWI_TYPE_CallScreening     eCallScreening;
    /*! international phone number flag
    *  1 - international number
    *  0 - otherwise */
    swi_bool                        bInternational;
    /*! number of BCD digits in the following phone number field. */
    swi_uint32                      nNumberLength;
    /*! phone number */
    swi_uint8                       bcdPhone[LENGTH_MaxPhone];
    /*! call type: line1, line2, CSD or FAX */
    enum SWI_TYPE_GSM_CallType      eCallType;                      
    /*! caller name */
    swi_char                        szCallerName[LENGTH_CallerNameVoice];
};

/**
 *   This structure is used by the SWI_NOTIFY_CallForwarded notification.
 * 
 * \sa SwiUpkGsmCallForwarded
 */
struct SWI_STRUCT_CallForwarded
{
    /*! size of this structure */
    swi_uint32      sizeStruct;
    /*! call id */
    swi_uint32      nCallID;
    /*! notification status present
    *  1 - notification status present
    *  0 - otherwise */
    swi_bool        bNotificationStatusPresent;
    /*! 0x01: Incoming is a forwarded call mask
    *  0x02: Incoming call was forwarded mask
    *  0x04: Outgoing call was forwarded mask */
    swi_uint32      nNotificationStatus;
};

/**
 *   This structure contains the call state of a single call.   
 */
struct SWI_STRUCT_CALL_State
{
    /*! size of this structure */
    swi_uint32                      sizeStruct;
    /*! call identifier */
    swi_uint32                      CallID;    
    /*! call state */
    enum SWI_TYPE_GSM_CallState     eCallState;
    /*! call info
    *  bit 0 - call held mask
    *  bit 1 - call in multi-party mask
    *  bit 2 - call on hold (remotely) mask
    *  bit 3 - call in multi-party (remotely) mask
    *  bit 4 - call suspended mask
    *  bit 5 - host originated call mask */
    swi_uint32                      CallInfo;   
};


/**
 *   This structure is used by the SWI_NOTIFY_CallStatus notifications 
 *   to report changes to the calls list. 
 *
 * \sa SwiUpkGsmCallStatus
 */
struct SWI_STRUCT_CALL_Status
{
    /*! size of this structure */
    swi_uint32                     sizeStruct;          
    /*! total number of calls */
    swi_uint32                     nCallSum;            
    /*! number of outgoing calls */
    swi_uint32                     nCallOutgoing;       
    /*! number of held calls */
    swi_uint32                     nCallHeld;           
    /*! number of calls waiting */
    swi_uint32                     nCallWaiting;        
    /*! number of calls queued */
    swi_uint32                     nCallQueued;         
    /*! number of multi-party calls */
    swi_uint32                     nCallMultiparty;     
    /*! number of calls in list (valid values are 0 through 7) */
    swi_uint8                      nNumberCalls;        
    /*! call list */
    struct SWI_STRUCT_CALL_State     CallList[MAX_CALLS]; 
};

/**
 *   This structure is used as input by the SwiSetDTMFSend function.
 *
 * \sa SwiSetDTMFSend
 */
struct SWI_STRUCT_DTMFSend
{
    /*! size of this structure */
    swi_uint32      sizeStruct;          
    /*! number of DTMF digits in the byDigits buffer    */
    swi_uint32      nNumberOfDigits;     
    /*! a buffer of DTMF digits in ASCII (',' '*' 
    *  '#' '0'-'9' 'A'-'D'). "," indicates a 3 second pause */
    swi_uint8       byDigits[20];       
    /*! 80ms < Range < 3000ms */
    swi_uint32      nToneOnDuration;     
    /*! 80ms < Range < 3000ms */
    swi_uint32      nToneOffDuration;    
    /*! earpiece feedback flag
    *  1 - play DTMF to earpiece
    *  0 - do not play DTMF to earpiece */
    swi_bool        bEarpieceFeedback;   
};

/**
 *   This structure is used as input by the SwiSetCallUMTSControl function.  
 *
 * \sa SwiSetCallControlUMTS
 */
struct SWI_STRUCT_CALL_UMTS_Control
{
    /*! size of this structure */
    swi_uint32                      sizeStruct;
    /*! version number */
    swi_uint32                      nVersion;
    /*! call id of the call to be acted on */
    swi_uint32                      nCallID;
    /*! action to be taken on the specified call.  If the action
    *  applies to all calls then the call id will be ignored 
    *  by the modem */
    enum SWI_TYPE_UMTS_CallControl  eCallAction;
};

/**
 *   This structure is as input for the SwiSetContDtmf function.  
 *
 * \sa SwiSetContDtmf
 */
struct SWI_STRUCT_ContDtmf
{
    /*! size of this structure */
    swi_uint32                      sizeStruct;
    /*! DTMF tone type */
    swi_uint32                      nDtmfTone;
    /*! DTMF start flag
    *  1 - start DTMF
    *  0 - stop DTMF */
    swi_bool                        bStart;
};

/**
 *   This structure contains the state of the call forwarding icon.  
 *
 * \note The icon indicats that unconditional call forwarding is activated.
 */
struct SWI_STRUCT_CFIcon
{
    /*! size of this structure */
    swi_uint32     sizeStruct;
    /*! call forwarding icon on flag
    *  1 - icon is highlighted
    *  0 - otherwise */
    swi_bool    bIconOn;
};

/**
 *   This structure contains the Supplementary Service Request information.  
 *
 * \sa SwiSetSSRequest
 */
struct SWI_STRUCT_SS_Request
{
    /*! size of this structure */
    swi_uint32  sizeStruct;                     
    /*! version of this object */
    swi_uint32  nVersion;                        
    /*! handle of request */
    swi_uint32  nHandle;                         
    /*! request type */
    enum SWI_TYPE_SS_RequestType eRequestType;        
    /*! supplementary service code */
    enum SWI_TYPE_SS_ServiceCode eSuppServiceCode;    
    /*! basic service group information */
    swi_bool    bBasicServGrpInfo;               
    /*! basic service group type */
    enum SWI_TYPE_SS_BasicServGrpType    eBasicServGrpType;
    /*! basic service group code */
    swi_uint32  eBasicServGrpCode;              
    /*! 0 = no '+' char */
    swi_bool    bInternationalNumFlag;           
    /*! length of forwarding number */
    swi_uint32  nLengthFwdNum;                   
    /*! forwarding number */
    swi_uint8   byFwdNum[LENGTH_ForwardNumber];  
    /*! no reply time information present */
    swi_bool    bNoReplyInfoPresent;             
    /*! in seconds - signed 8-bits */
    swi_int32   nNoReplyTime;                    
    /*! string present */
    swi_bool    bStringPresent;                  
    /*! length of string */
    swi_uint32  nLengthSSString;                 
    /*! supplementary service string */
    swi_uint8   bySSString[LENGTH_SSString];     
    /*! request status */
    enum SWI_TYPE_SS_RequestStatus   eRequestStatus;  
};

/**
 *   This structure contains the Supplementary Service Abort information.  
 *
 * \sa SwiAbortRequest
 */
struct SWI_STRUCT_SS_Abort
{
    /*! size of this structure */
    swi_uint32      sizeStruct;
    /*! version of this object */
    swi_uint32      nVersion;
    /*! handle of request */
    swi_uint32      nHandle;
};

/**
 *   This structure contains the Supplementary Service set information.  
 *
 * \sa SwiStartSSPassReg
 */
struct SWI_STRUCT_SS_PasswordReg
{
    /*! size of this structure */
    swi_uint32                          sizeStruct;
    /*! version of this object */
    swi_uint32                          nVersion;
    /*! handle of request */
    swi_uint32                          nHandle;
    /*! supplementary service code */
    enum SWI_TYPE_SS_ServiceCode        eSuppServiceCode;
};

/**
 *   This structure contains the Supplementary Service Password status
 *   information.  
 *
 * \sa SwiSetSSPassword
 */
struct SWI_STRUCT_SS_PasswordStatus
{
    /*! size of this structure */
    swi_uint32      sizeStruct;
    /*! version of this object */
    swi_uint32      nVersion;
    /*! handle of requeste */
    swi_uint32      nHandle;
    /*! length of password (1-4) */
    swi_uint32      nPasswordLength;
    /*! password */
    swi_uint8       byPassword[LENGTH_SSPassword];
    /*! password operation to perform */
    enum SWI_TYPE_SS_PasswordGuidance ePasswordGuidance;
};

/**
 *   This structure is used by the SWI_NOTIFY_SSRespOK notifications 
 *   to report that the supplementary service is okay
 */
struct SWI_STRUCT_SS_RespOK
{
    /*! size of this structure */
    swi_uint32     sizeStruct;
    /*! version of this object */
    swi_uint32     nVersion;
    /*! handle of request */
    swi_uint32     nHandle;
};

/**
 *   This structure is used by the SWI_NOTIFY_SSRespOK notifications 
 *   to report that the supplementary service is okay
 */
struct STRUCT_SS_FwdInfo
{
    /*! basic service group information present */
    swi_bool                            bBasicServGrpInfoPresent;
    /*! basic service group type */
    enum SWI_TYPE_SS_BasicServGrpType   eBasicServGrpType;
    /*! basic service group code */
    swi_uint32                          eBasicServGrpCode;
    /*! service status information present */
    swi_bool                            bServStatusInfoPresent;
    /*! service status */
    enum SWI_TYPE_SS_ServiceStatus      eServStatus;
    /*! false - no '+' character */
    swi_bool                            bInternational;
    /*! length of forwarding number */
    swi_uint32                          nLengthFwdNum;
    /*! array of binary numbers */
    swi_uint8                           byFwdNum[LENGTH_ForwardInfo];
    /*! forward options information present */
    swi_bool                            bFwdOptionsInfoPresent;
    /*! forwarding options */
    enum SWI_TYPE_SS_ForwardOption      eFwdOptions;
    /*! no reply time information present */
    swi_bool                            bNoReplyTimePresent;
    /*! seconds */
    swi_uint32                          NoReplyTime;
};

/**
 *   This structure is used by the SWI_NOTIFY_SSRespFwdInfo notifications 
 *   to hold the forwarding information
 */
struct SWI_STRUCT_SS_RespFwdInfo
{
    /*! size of this structure */
    swi_uint32                          sizeStruct;
    /*! version of this object */
    swi_uint32                          nVersion;
    /*! handle of request */
    swi_uint32                          nHandle;
    /*! service code present */
    swi_bool                            bServiceCodePresent;
    /*! service code */
    enum SWI_TYPE_SS_ServiceCode        eServiceCode;
    /*! number of entries in list */
    swi_uint32                          nNumEntries;
    /*! forward info list */
    struct STRUCT_SS_FwdInfo            sList[MAX_FWD_INFO_ENTRIES];
};

/**
 *   This structure is used by the SWI_NOTIFY_SSRespCallBar notifications 
 *   to hold information regarding the call being barred
 */
struct STRUCT_SS_CALL_BAR
{
    /*! basic service group information present */
    swi_bool                            bBasicServGrpInfoPresent;
    /*! basic service group type */
    enum SWI_TYPE_SS_BasicServGrpType   eBasicServGrpType;
    /*! basic service group code */
    swi_uint32                          eBasicServGrpCode;
    /*! service status information present */
    swi_bool                            bServStatusInfoPresent;
    /*! service status */
    swi_uint32                          nServStatus;
};

/**
 *   This structure is used by the SWI_NOTIFY_SSRespCallBar notifications 
 */
struct SWI_STRUCT_SS_RespCallBar
{
    /*! size of this structure */
    swi_uint32                          sizeStruct;
    /*! version of this object */
    swi_uint32                          nVersion;
    /*! handle of request */
    swi_uint32                          nHandle;
    /*! service code present */
    swi_bool                            bServiceCodePresent;
    /*! service code */
    enum SWI_TYPE_SS_ServiceCode        eServiceCode;
    /*! number of entries in list */
    swi_uint32                          nNumEntries;
    /*! call barring list */
    struct STRUCT_SS_CALL_BAR           sList[MAX_FWD_INFO_ENTRIES];
};

/**
 *   This structure is used by the SWI_NOTIFY_SSRespInfoData notifications 
 *   to hold the information regarding basic service group
 */
struct STRUCT_SS_BasicServiceGroup
{
    /*! basic service group type */
    enum SWI_TYPE_SS_BasicServGrpType   eBasicServGrpType;
    /*! basic service group code */
    swi_uint32                          eBasicServGrpCode;
};

/**
 *   This structure is used by the SWI_NOTIFY_SSRespInfoData notifications 
 */
struct SWI_STRUCT_SS_RespInfoData
{
    /*! size of this structure */
    swi_uint32                          sizeStruct;
    /*! version of this object */
    swi_uint32                          nVersion;
    /*! handle of request */
    swi_uint32                          nHandle;
    /*! service code present */
    swi_bool                            bServiceCodePresent;
    /*! service code */
    enum SWI_TYPE_SS_ServiceCode        eServiceCode;
    /*! service code present */
    swi_bool                            bServStatusPresent;
    /*! service status */
    enum SWI_TYPE_SS_ServiceStatus      eServStatus;
    /*! subscription option information present */
    swi_bool                            bOptionInfoPresent;
    /*! subscription option type */
    enum SWI_TYPE_SS_SubsOptionType     eSubsOptionType;
    /*! subscription option code */
    enum SWI_TYPE_SS_SubsOptionCode     eSubsOptionCode;
    /*! number of entries in list */
    swi_uint32                          nNumEntries;
    /*! call barring list */
    struct STRUCT_SS_BasicServiceGroup  sList[MAX_INFO_DATA];
};

/**
 *   This structure is used by the SWI_NOTIFY_SSRespStatus notifications 
 */
struct SWI_STRUCT_SS_RespStatus
{
    /*! size of this structure */
    swi_uint32                          sizeStruct;
    /*! version of this object */
    swi_uint32                          nVersion;
    /*! handle of request */
    swi_uint32                          nHandle;
    /*! service status */
    swi_uint32                          nServiceStatus;
};

/**
 *   This structure is used by the SWI_NOTIFY_SSRespFwdNum notifications 
 */
struct SWI_STRUCT_SS_RespFwdNum
{
    /*! size of this structure */
    swi_uint32                          sizeStruct;
    /*! version of this object */
    swi_uint32                          nVersion;
    /*! handle of request */
    swi_uint32                          nHandle;
    /*! international flag */
    swi_bool                            bInternational;
    /*! length of forwarding number */
    swi_uint32                          nLengthFwdNum;
    /*! forwarding number */
    swi_uint8                           byFwdNum[LENGTH_ForwardNumber];
};

/**
 *   This structure is used by the SWI_NOTIFY_SSRespBSList notifications 
 */
struct SWI_STRUCT_SS_RespBSList
{
    /*! size of this structure */
    swi_uint32                          sizeStruct;
    /*! version of this object */
    swi_uint32                          nVersion;
    /*! handle of request */
    swi_uint32                          nHandle;
    /*! number of entries in list */
    swi_uint32                          nNumEntries;
    /*! basic service list */
    struct STRUCT_SS_BasicServiceGroup  sList[MAX_BS_LIST];
};

/**
 *   This structure is used by the SWI_NOTIFY_SSRespFwdList notifications 
 */
struct SWI_STRUCT_SS_RespListFwdInfo
{
    /*! size of this structure */
    swi_uint32                          sizeStruct;
    /*! version of this object */
    swi_uint32                          nVersion;
    /*! handle of request */
    swi_uint32                          nHandle;
    /*! number of entries in list */
    swi_uint32                          nNumEntries;
    /*! forward info list */
    struct STRUCT_SS_FwdInfo            sList[MAX_FWD_INFO_ENTRIES];
};

/**
 *   This structure is used by the SWI_NOTIFY_SSRespRaw notifications 
 */
struct SWI_STRUCT_SS_RespRaw
{
    /*! size of this structure */
    swi_uint32                          sizeStruct;
    /*! version of this object */
    swi_uint32                          nVersion;
    /*! handle of request */
    swi_uint32                          nHandle;
    /*! length of raw data */
    swi_uint32                          nLength;
    /*! raw data array */
    swi_uint8                           byData[LENGTH_RawData];
};

/**
 *   This structure is used by the SWI_NOTIFY_SSRespPassword notifications 
 */
struct SWI_STRUCT_SS_RespPassword
{
    /*! size of this structure */
    swi_uint32                          sizeStruct;
    /*! version of this object */
    swi_uint32                          nVersion;
    /*! handle of request */
    swi_uint32                          nHandle;
    /*! length of password */
    swi_uint32                          nPasswordLength;
    /*! password, 4 BCD */
    swi_uint8                           byPassword[LENGTH_SSPassword];
};

/**
 *   This structure is used by the SWI_NOTIFY_SSRespClir notifications 
 */
struct SWI_STRUCT_SS_RespClir
{
    /*! size of this object */
    swi_uint32                          sizeStruct;
    /*! version of this object */
    swi_uint32                          nVersion;
    /*! handle of request */
    swi_uint32                          nHandle;
    /*! service status */
    enum SWI_TYPE_SS_ServiceStatus      eServStatus;
    /*! CLIR setting */
    enum SWI_TYPE_SS_CLIRSetting        eClir;
};

/**
 *   This structure is used by the SWI_NOTIFY_SSRespError notifications 
 *   to hold the error causes related to subscription violation
 */
struct SubscriptionViolation
{
    /*! subscription option type */
    enum SWI_TYPE_SS_SubsOptionType     eSubsOptionType;
    /*! subscription option code */
    enum SWI_TYPE_SS_SubsOptionCode     eSubsOptionCode;
};

/**
 *   This structure is used by the SWI_NOTIFY_SSRespError notifications 
 *   to hold the error causes related to incompatibility
 */
struct IncompatibilityReason
{
    /*! service code present */
    swi_bool                            bServiceCodePresent;
    /*! service code */
    enum SWI_TYPE_SS_ServiceCode        eServiceCode;
    /*! basic service group present */
    swi_bool                            bBasicServGrpPresent;
    /*! basic service group type */
    enum SWI_TYPE_SS_BasicServGrpType   eBasicServGrpType;
    /*! basic service group code */
    swi_uint32                          eBasicServGrpCode;
    /*! service status information present */
    swi_bool                            bServStatusInfoPresent;
    /*! service status */
    enum SWI_TYPE_SS_ServiceStatus      eServStatusErr;
};

/**
 *   This structure is used by the SWI_NOTIFY_SSRespError notifications 
 *   to determine the cause of the error
 */
union ErrorCause
{
    /*! call barring */
    enum SWI_TYPE_SS_CallBar            eCallBarring;
    /*! service status */
    enum SWI_TYPE_SS_ServiceStatus      eServStatus;
    /*! 0 - subscriber not present */
    swi_bool                            bAbsentSubs;
    /*! system failure */
    enum SWI_TYPE_SS_SystemFail         eSystemFail;
    /*! password registration failure reason */
    enum SWI_TYPE_SS_PasswdRegFail      ePasswdRegFail;
    /*! subscription violation structure */
    struct SubscriptionViolation        sSubscripViolation;
    /*! incompatibility reason structure */
    struct IncompatibilityReason        sIncompatibilityReason;
};

/**
 *   This structure is used by the SWI_NOTIFY_SSRespError notifications 
 */
struct SWI_STRUCT_SS_RespError
{
    /*! size */
    swi_uint32                          sizeStruct;
    /*! version of this object */
    swi_uint32                          nVersion;
    /*! handle of request */
    swi_uint32                          nHandle;
    /*! error code */
    enum SWI_TYPE_SS_ErrorCode          eErrorCode;
    /*! error type */
    enum SWI_TYPE_SS_ErrorType          eErrorType;
    /*! error cause union */
    union ErrorCause uErrorCause;
};

/**
 *   This structure is used by the SWI_NOTIFY_SSRespReject notifications 
 */
struct SWI_STRUCT_SS_RespReject
{
    /*! size of this structure */
    swi_uint32                          sizeStruct;
    /*! version of this object */
    swi_uint32                          nVersion;
    /*! handle of request */
    swi_uint32                          nHandle;
    /*! problem type */
    enum SWI_TYPE_SS_ProblemType        eProblemType;
    /*! reject code */
    enum SWI_TYPE_SS_RejectCode         eRejectCode;
};

/**
 *   This structure is used by the SWI_NOTIFY_SSRespCancel notifications 
 */
struct SWI_STRUCT_SS_RespCancel
{
    /*! size of this structure */
    swi_uint32                          sizeStruct;
    /*! version of this object */
    swi_uint32                          nVersion;
    /*! handle of request */
    swi_uint32                          nHandle;
    /*! cancel reason */
    enum SWI_TYPE_SS_CancelReason       eCancelReason;
};

/**
 *   This structure is used by the SWI_NOTIFY_SSRespRelease notifications 
 */
struct SWI_STRUCT_SS_RespRelease
{
    /*! size of this structure */
    swi_uint32                          sizeStruct;
    /*! version of this object */
    swi_uint32                          nVersion;
    /*! handle of request */
    swi_uint32                          nHandle;
};

/**
 *   This structure is used by the SWI_NOTIFY_SSNotify notifications 
 */
struct SWI_STRUCT_SSNotify
{
    /*! size of this structure */
    swi_uint32                          sizeStruct;
    /*! version of this object */
    swi_uint32                          nVersion;
    /*! service code */
    enum SWI_TYPE_SS_ServiceCode        eServiceCode;
};

/* function prototypes */
extern SWI_API SWI_RCODE SwiGetGsmSpeakerVolume(
    struct SWI_STRUCT_GSM_SpeakerVolume *sSpeakerVolume, swi_uint32 timeout);
extern SWI_API SWI_RCODE SwiSetGsmSpeakerVolume(
    struct SWI_STRUCT_GSM_SpeakerVolume *sSpeakerVolume, swi_uint32 timeout);
extern SWI_API SWI_RCODE SwiGetMicMute(
    struct SWI_STRUCT_MicMute *pMicMute, swi_uint32 timeout);
extern SWI_API SWI_RCODE SwiSetMicMute(
    struct SWI_STRUCT_MicMute *pMicMute, swi_uint32 timeout);
extern SWI_API SWI_RCODE SwiGetSpeakerMute(
    struct SWI_STRUCT_GSM_SpeakerMute *pSpeakerMute, swi_uint32 timeout);
extern SWI_API SWI_RCODE SwiSetSpeakerMute(
    struct SWI_STRUCT_GSM_SpeakerMute *pSpeakerMute, swi_uint32 timeout);
extern SWI_API SWI_RCODE SwiGetNVToneStore(
    struct SWI_STRUCT_NVToneStore *pToneLength, swi_uint32 timeout);
extern SWI_API SWI_RCODE SwiSetNVToneStore(
    struct SWI_STRUCT_NVToneStore *pToneLength, swi_uint32 timeout);
extern SWI_API SWI_RCODE SwiGetAudioProfile(
    struct SWI_STRUCT_AudioProfile* pAudioProfile, swi_uint32 timeout);
extern SWI_API SWI_RCODE SwiSetAudioProfile(
    struct SWI_STRUCT_AudioProfile* pAudioProfile, swi_uint32 timeout);
extern SWI_API SWI_RCODE SwiGetCLIRSetting(
    struct SWI_STRUCT_CLIR_Setting  *pCLIR, swi_uint32 timeout);
extern SWI_API SWI_RCODE SwiSetCLIRSetting(
    const struct SWI_STRUCT_CLIR_Setting *pCLIR, swi_uint32 timeout);
extern SWI_API SWI_RCODE SwiGetVoiceMessage(
    struct SWI_STRUCT_VoiceMessage *pVoiceMessage, swi_uint32 timeout);
extern SWI_API SWI_RCODE SwiAcknowledgeMissedCall(
    struct SWI_STRUCT_MissedCall  *pMissedCall, swi_uint32 timeout);
extern SWI_API SWI_RCODE SwiSetCallOriginate(
    const struct SWI_STRUCT_CALL_Create* pCall, swi_uint32 timeout);
extern SWI_API SWI_RCODE SwiSetDTMFSend(
    struct SWI_STRUCT_DTMFSend *pDTMFSend, swi_uint32 timeout);
extern SWI_API SWI_RCODE SwiSetCallControlUMTS(
    const struct SWI_STRUCT_CALL_UMTS_Control* pCall, swi_uint32 timeout);
extern SWI_API SWI_RCODE SwiSetContDtmf(
    struct SWI_STRUCT_ContDtmf *pDtmf, swi_uint32 timeout);
extern SWI_API SWI_RCODE SwiSetSSRequest (
    struct SWI_STRUCT_SS_Request* sSSRequest, swi_uint32 timeout);
extern SWI_API SWI_RCODE SwiAbortRequest (
    struct SWI_STRUCT_SS_Abort* sSSAbort, swi_uint32 timeout);
extern SWI_API SWI_RCODE SwiStartSSPassReg (
    struct SWI_STRUCT_SS_PasswordReg* sSSPassReg, swi_uint32 timeout);
extern SWI_API SWI_RCODE SwiSetSSPassword (
    struct SWI_STRUCT_SS_PasswordStatus* sSSPassStatus, swi_uint32 timeout);
extern SWI_API SWI_RCODE SwiSetUserdefinedUserBusy(swi_uint32 timeout);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __SWIAPI_GSM_VOICE */

/* 
 * $Log: SwiApiGsmVoice.h,v $
 */
