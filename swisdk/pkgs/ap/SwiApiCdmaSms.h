/**
 *    \if CvsId 
 *    $Id: SwiApiCdmaSms.h,v 1.5 2010/06/10 00:12:08 epasheva Exp $
 *    \endif
 *    \ingroup cdmasms
 *    \file SwiApiCdmaSms.h
 * 
 *    \brief SWI CDMA specific SMS related APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */
#ifndef __SWIAPICDMASMS_H__
#define __SWIAPICDMASMS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* include files */
#include "SwiDataTypes.h"
#include "SwiRcodes.h"

#define CNS_MAX_PACKET_SIZE     256     /*!< Maximum CnS packet size */
#define CNS_PACKET_HEADER_SIZE  10      /*!< Maximum CnS packet header size */

/*! Maximum CnS packet payload size */
#define CNS_MAX_PAYLOAD_SIZE    (CNS_MAX_PACKET_SIZE - CNS_PACKET_HEADER_SIZE)

#define CNS_SMS_HEADER_SIZE     6       /*!< Maximum SMS header size */

/*! Maximum SMS size */
#define CNS_SMS_MAX_SIZE        (CNS_MAX_PAYLOAD_SIZE - CNS_SMS_HEADER_SIZE)

#define MAX_SMSI_CALL_BACK      32      /*!< Maximum SMS callback length */
#define MAX_SMSI_TL_ADDRESS     50      /*!< Maximum SMS address length */

/**
 *
 * An enumeration of SMS packet type.
 *
 */
typedef enum SWI_SMS_PACKET_TYPE
{
    SMS_SEGMENT_INTERMEDIATE = 0x00,    /*!< Intermediate segment */
    SMS_SEGMENT_FIRST,                  /*!< First segment */
    SMS_SEGMENT_LAST,                   /*!< Last segment */
    SMS_NOT_SEGMENTED                   /*!< Not a segment */
} SWI_SMS_PACKET_TYPE ;

/**
 *
 * An enumeration of SMS API type.
 *
 * This enum will be used to indicate if the legacy SMS APIs
 * [ SwiRetrieveSmsMessage ] are used instead of the new
 * [ SwiRetrieveSmsMessageEx and SwiSetMessageRetrieved ] APIs. The two sets 
 * above are mutually exclusive. And the mode will be switched to whoever 
 * uses the APIs first. So, if the first call happens to a new API, the mode 
 * will be switched to that and shall remain so throughout the life of the 
 * program.
 *
 */
typedef enum SMSAPIUsage
{
    SMS_Use_Uninitialized,      /*!< Not know yet. */
    SMS_Use_Legacy,             /*!< Use legacy SWI SMS API */
    SMS_Use_New                 /*!< Use new SWI SMS API */
}SMSAPIUsage;

/*
 *
 * SMS Messaging
 *
 * CDMA2000 1X modems can send and receive
 * SMS messages. To handle SMS messages, there
 * are many data types and structures involved.
 * Most of these relate to header content. The
 * details of the parameters in the SMS header are
 * defined in the CDMA specification and are
 * beyond the scope of this manual.
 *
 * Consult the documentation for a discussion of the SMS message handling 
 * process.
 *
 */

/**
 *
 * An enumeration of SMS parameter mask.
 *
 */
typedef enum SWI_TYPE_SMS_ParamMask
{
    SWI_SMS_PARAMASK_NO_PARAMS      = 0x00000000,   /*!< No parameters set */

    /*! Bearer Reply (TL Ack.) */
    SWI_SMS_PARAMASK_BEARER_REPLY   = 0x00000001,

    /*! Absolute Validity Time */
    SWI_SMS_PARAMASK_VALID_ABS      = 0x00000002,

    /*! Relative Validity Time */
    SWI_SMS_PARAMASK_VALID_REL      = 0x00000004,

    /*! Absolute Delivery Deferral Time */
    SWI_SMS_PARAMASK_DEFER_ABS      = 0x00000008,

    /*! Relative Delivery Deferral Time */
    SWI_SMS_PARAMASK_DEFER_REL      = 0x00000010,

    SWI_SMS_PARAMASK_USER_RESP      = 0x00000020,   /*!< User Response */
    SWI_SMS_PARAMASK_ADDRESS        = 0x00000040,   /*!< Address */
    SWI_SMS_PARAMASK_SUBADDR        = 0x00000080,   /*!< Sub Address */
    SWI_SMS_PARAMASK_USER_DATA      = 0x00000100,   /*!< Data Encoding */
    SWI_SMS_PARAMASK_PRIORITY       = 0x00000200,   /*!< Priority */
    SWI_SMS_PARAMASK_PRIVACY        = 0x00000400,   /*!< Privacy */
    
    /*! Reply Option (DAK & User Ack) */
    SWI_SMS_PARAMASK_REPLY_OPT      = 0x00000800,

    SWI_SMS_PARAMASK_NUM_OF_MSG     = 0x00001000,   /*!< Number of Messages */
    SWI_SMS_PARAMASK_ALERT          = 0x00002000,   /*!< Delivery Alert */
    SWI_SMS_PARAMASK_LANGUAGE       = 0x00004000,   /*!< Language */
    SWI_SMS_PARAMASK_CALL_BACK      = 0x00008000,   /*!< Call Back Number */
    SWI_SMS_PARAMASK_MC_TIME_STAMP  = 0x00010000,   /*!< Time Stamp */
    SWI_SMS_PARAMASK_DISP_MODE      = 0x00020000    /*!< Display Mode */
}SWI_TYPE_SMS_ParamMask;

/**
 *
 * An enumeration of SMS category.
 *
 */
typedef enum SWI_TYPE_SMS_MsgCategory
{
    SWI_SMS_MSGCAT_POINT_TO_POINT,  /*!< IS-637 SMS Point-to-Point Message */
    SWI_SMS_MSGCAT_BROADCAST_MSG,   /*!< IS-637 SMS Broadcast Message */
    SWI_SMS_MSGCAT_ACK_MSG          /*!< IS-637 SMS Acknowledge Message */
}SWI_TYPE_SMS_MsgCategory;

/**
 *
 * An enumeration of SMS teleservice.
 *
 * Refer to TIA/EIA-41-D Table 175 for further information.
 *
 */
typedef enum SWI_TYPE_SMS_Teleservice
{
    SWI_SMS_TELESRV_CELL_PAGING,    /*!< SMSI_CPT_95: IS-95A Cellular Paging */

    /*! SMSI_CMT_95: IS-95A Cellular Messaging */
    SWI_SMS_TELESRV_CELL_MESSAGING,
    
    /*! SMSI_VMN_95: IS-95A Voice Mail Notification */
    SWI_SMS_TELESRV_VOICE_MAIL_NOT,
    
    /*! SMSI_CMT_91_CLI: IS-91 Extended Protocol - CLI Order */
    SWI_SMS_TELESRV_CLI_ORDER,
    
    /*! SMSI_CMT_91_VOICE_MAIL: IS-91 Extended Protocol - Voice Mail and 
         Embedded Message Waiting Indicator */
    SWI_SMS_TELESRV_VOICE_IND,

    /*! SMSI_CMT_91_SHORT_MSG: IS-91 Extended Protocol - Short Message */
    SWI_SMS_TELESRV_SHORT_MSG,

    SWI_SMS_TELESRV_ALERT_INFO,     /*!< SMSI_AWI_95: IS-95A Alert With Info */
    SWI_SMS_TELESRV_WAP             /*!< SMSI_WAP:  WAP Message */
}SWI_TYPE_SMS_Teleservice;

/**
 *
 * An enumeration of SMS ID.
 *
 */
typedef enum SWI_TYPE_SMS_MsgIdType
{
    SWI_SMS_MSGIDTYPE_NOTUSED,      /*!< NOT USED */
    SWI_SMS_MSGIDTYPE_DELIVER,      /*!< Deliver (Regular MT SMS) */
    SWI_SMS_MSGIDTYPE_SUBMIT,       /*!< Submit (Regular MO SMS) */
    
    /*! Cancel previous SMS delivery (MO Only) */
    SWI_SMS_MSGIDTYPE_CANCEL,
    SWI_SMS_MSGIDTYPE_DELIVERY_ACK, /*!< Delivery Ack.  (MT Only) */

    /*! User Acknowledgement (Either Directions) */
    SWI_SMS_MSGIDTYPE_USER_ACK
}SWI_TYPE_SMS_MsgIdType;

/**
 *
 * An enumeration of SMS priority.
 *
 */
typedef enum SWI_TYPE_SMS_Priority
{
    SWI_SMS_PRIORITY_NORMAL,        /*!< Normal */
    SWI_SMS_PRIORITY_INTERACTIVE,   /*!< Interactive */
    SWI_SMS_PRIORITY_URGENT,        /*!< Urgent */
    SWI_SMS_PRIORITY_EMERGENCY      /*!< Emergency */
}SWI_TYPE_SMS_Priority;

/**
 *
 * An enumeration of SMS privacy.
 *
 */
typedef enum SWI_TYPE_SMS_Privacy
{
    SWI_SMS_PRIVACY_NOT_RESTR,      /*!< Not restricted (Level 0) */
    SWI_SMS_PRIVACY_RESTRICTED,     /*!< Restricted (Level 1) */
    SWI_SMS_PRIVACY_CONFIDENTIAL,   /*!< Confidential (Level 2) */
    SWI_SMS_PRIVACY_SECRET          /*!< Secret (Level 3) */
}SWI_TYPE_SMS_Privacy;

/**
 *
 * An enumeration of SMS message body language.
 *
 */
typedef enum SWI_TYPE_SMS_Language
{
    SWI_SMS_LANGUAGE_UNSPECIFIED,   /*!< UNSPECIFIED */
    SWI_SMS_LANGUAGE_ENGLISH,       /*!< ENGLISH */
    SWI_SMS_LANGUAGE_FRENCH,        /*!< FRENCH */
    SWI_SMS_LANGUAGE_SPANISH,       /*!< SPANISH */
    SWI_SMS_LANGUAGE_JAPANESE,      /*!< JAPANESE */
    SWI_SMS_LANGUAGE_KOREAN,        /*!< KOREAN */
    SWI_SMS_LANGUAGE_CHINESE,       /*!< CHINESE */
    SWI_SMS_LANGUAGE_HEBREW         /*!< HEBREW */
}SWI_TYPE_SMS_Language;

/**
 *
 * An enumeration of SMS display mode.
 *
 */
typedef enum SWI_TYPE_SMS_MsgDispMode
{
    SWI_SMS_MSGDISPMODE_IMMEDIATE,      /*!< Immediate Display */
    SWI_SMS_MSGDISPMODE_DEFAULT,        /*!< Radio default setting */
    SWI_SMS_MSGDISPMODE_USER_INVOKE     /*!< User Invoke */
}SWI_TYPE_SMS_MsgDispMode;

/**
 *
 * An enumeration of possible data encoding methods used on the SMS message 
 * body.
 *
 * For a more complete list, see SMS standard TSB-58C.
 *
 */
typedef enum SWI_TYPE_SMS_DataEncodeType
{
    /*! Octet, unspecified (i.e. Binary) */
    SWI_SMS_DATAENCODE_8bitAscii = 0,

    SWI_SMS_DATAENCODE_IS91EP,              /*!< varies */
    SWI_SMS_DATAENCODE_7bitAscii,           /*!< Standard ascii, 7-bit */
    
    /*! 7-bit  encoding (International Alphabet No.5 same as ASCII) */
    SWI_SMS_DATAENCODE_IA5,

    SWI_SMS_DATAENCODE_Unicode,             /*!< Unicode */
    
    /*! 8 or 16-bit. It's also known as SJIS or MS Kanji for Japanese 
         support */
    SWI_SMS_DATAENCODE_Shift_Jis,

    SWI_SMS_DATAENCODE_Korean,              /*!< 8 or 16-bit. Korean support */
    
    /*! 8-bit. specific to the Hebrew script ISO-8859-8 and Windows-1255 */
    SWI_SMS_DATAENCODE_LatinHebrew,

    SWI_SMS_DATAENCODE_Latin,               /*!< HEBREW */
    
    /*! 7-bit. defined in ETSI GSM 03.38 */
    SWI_SMS_DATAENCODE_GSM7BitDefault,

    SWI_SMS_DATAENCODE_MAX32 = 0x10000000   /*!< Place holder */
}SWI_TYPE_SMS_DataEncodeType;

/**
 *
 * An enumeration of SMS send error class 
 *
 * This enumeration identifies the possible Error Class values that may be 
 * returned with the notification SWI_NOTIFY_SmsSendStatus
 *
 */
typedef enum SWI_TYPE_SMS_ErrorClass
{
    SWI_SMS_ERRORCLASS_NoError,     /*!< No Error */
    SWI_SMS_ERRORCLASS_Reserved,    /*!< Reserved (Ignore) */
    SWI_SMS_ERRORCLASS_Temporary,   /*!< Temporary.  A retry is recommended */
    
    /*! Permanent. A retry is not recommened */
    SWI_SMS_ERRORCLASS_Permanent
}SWI_TYPE_SMS_ErrorClass;

/**
 *
 * An enumeration of SMS send cause code. 
 *
 * This enumeration identifies the possible Cause Code values that may be 
 * returned with the notification SWI_NOTIFY_SmsSendStatus
 *
 * For detailed explanation of each of the codes, please refer to the IS-41 
 * SMS Standard.
 *
 */
typedef enum SWI_TYPE_SMS_CauseCode
{
    /*! Below are network problems */
    SWI_SMS_CAUSECODE_AddressVacant                 = 0x00,
    SWI_SMS_CAUSECODE_AddressTranslationFailure,
    SWI_SMS_CAUSECODE_NetworkResourceShortage,
    SWI_SMS_CAUSECODE_NetworkFailure,
    SWI_SMS_CAUSECODE_InvalidTeleserviceId,
    SWI_SMS_CAUSECODE_OtherNetworkProblem,
    SWI_SMS_CAUSECODE_OtherNetworkProblemMoreFirst  = 0x06,
    SWI_SMS_CAUSECODE_OtherNetworkProblemMoreLast   = 0x1F,

    /* ! Below are terminal Problems */
    SWI_SMS_CAUSECODE_NoPageResponse                = 0x20,
    SWI_SMS_CAUSECODE_DestinationBusy,
    SWI_SMS_CAUSECODE_NoAck,
    SWI_SMS_CAUSECODE_DestinationResourceShortage,
    SWI_SMS_CAUSECODE_SmsDeliveryPostponed,
    SWI_SMS_CAUSECODE_DestinationOutOfService,
    SWI_SMS_CAUSECODE_DestinationNoLongerAtThisAddress,
    SWI_SMS_CAUSECODE_OtherTerminalProblem,
    SWI_SMS_CAUSECODE_OtherTerminalProblemMoreFirst = 0x28,
    SWI_SMS_CAUSECODE_OtherTerminalProblemMoreLast  = 0x2F,
    SWI_SMS_CAUSECODE_SmsDeliveryPostponedMoreFirst = 0x30,
    SWI_SMS_CAUSECODE_SmsDeliveryPostponedMoreLast  = 0x3F,

    /*! Below are radio Interface Problems */
    SWI_SMS_CAUSECODE_Radio_If_Resource_Shortage    = 0x40,
    SWI_SMS_CAUSECODE_RadioIfIncompatible,
    SWI_SMS_CAUSECODE_OtherRadioIfProblem,
    SWI_SMS_CAUSECODE_OtherRadioIfProblemMoreFirst  = 0x43,
    SWI_SMS_CAUSECODE_OtherRadioIfProblemMoreLast   = 0x5F, 

    /*! Below are general Problems */
    SWI_SMS_CAUSECODE_UnexpectedParmSize            = 0x60,
    SWI_SMS_CAUSECODE_SmsOriginationDenied,
    SWI_SMS_CAUSECODE_SmsTerminationDenied,
    SWI_SMS_CAUSECODE_SupplServiceNotSupported,
    SWI_SMS_CAUSECODE_SmsNotSupported,
    SWI_SMS_CAUSECODE_Reserved101,
    SWI_SMS_CAUSECODE_MissingExpectedParm,
    SWI_SMS_CAUSECODE_MissingMandatoryParm,
    SWI_SMS_CAUSECODE_UnrecognizedParmValue,
    SWI_SMS_CAUSECODE_UnexpectedParmValue,
    SWI_SMS_CAUSECODE_UserDataSizeError,
    SWI_SMS_CAUSECODE_OtherGeneralProblems,
    SWI_SMS_CAUSECODE_OtherGeneralProblemsMoreFirst = 0x6C,
    SWI_SMS_CAUSECODE_OtherGeneralProblemsMoreLast  = 0xFF,

    /*! Below are internal SMS Cause Code (From the Mobile Station) */
    SWI_SMS_CAUSECODE_DtcCallDisconnect             = 0x4000,
    SWI_SMS_CAUSECODE_DtcCallError                  = 0x4001, 
    SWI_SMS_CAUSECODE_Ok                            = 0x8000,
    SWI_SMS_CAUSECODE_WaitingForTlAck,
    SWI_SMS_CAUSECODE_OutOfResources,
    SWI_SMS_CAUSECODE_AccesTooLarge,
    SWI_SMS_CAUSECODE_DtcTooLarge,
    SWI_SMS_CAUSECODE_NetworkNotReady,
    SWI_SMS_CAUSECODE_PhoneNotReady,
    SWI_SMS_CAUSECODE_NotAllowedInAmps,
    SWI_SMS_CAUSECODE_CannotSendBroadcast,
    SWI_SMS_CAUSECODE_InvalidTransactionID,
    SWI_SMS_CAUSECODE_MsgNotSent,
    SWI_SMS_CAUSECODE_MsgBeingSent
}SWI_TYPE_SMS_CauseCode;

/**
 *
 * An enumeration of SMS support mask bit.
 *
 * The enumerated mask bits identifies the SMS features supported by the modem.
 *
 */
typedef enum SWI_TYPE_PRI_SmsMask
{
    /*! Mobile Terminated (Incoming) SMS supported */
    SWI_PRI_SMS_MT              = 0x0001,
    
    /*! Mobile Originated (Outgoing) SMS supported */
    SWI_PRI_SMS_MO              = 0x0002,

    SWI_PRI_SMS_ALL             = 0xFFFF,   /*!< Internal Use */

    /*! Enabled bit (Internal use only) */
    SWI_PRI_SMS_MASK_ENABLED    = 0x8000
}SWI_TYPE_PRI_SmsMask;

/**
 *
 * Contains status of SMS messages on the modem.
 *
 * This structure reports the number of SMS messages in each of the three 
 * buffers in the modem: urgent, regular, and voice.
 *
 */
typedef struct SWI_STRUCT_CDMA_SMS_Status
{
    swi_uint32 sizeStruct;      /*!< Size of this structure */      
    swi_uint32 cntUrgentMsg;    /*!< Urgent msg count */   
    swi_uint32 cntRegularMsg;   /*!< Regular msg count */   
    swi_uint32 cntVoiceMsg;     /*!< Voice msg count */   
}SWI_STRUCT_CDMA_SMS_Status;

/**
 *
 * Contains CDMA SMS header.
 *
 * A portion of SWI_STRUCT_CDMA_SMS_RetrieveSms, this structure defines the 
 * header of incoming SMS messages. Most of the data items are defined in the 
 * CDMA specification and are not explained here. The enumerated types are 
 * described following this structure.
 *
 */
typedef struct SWI_STRUCT_SMS_CdmaHeader
{
    swi_uint32 sizeStruct;      /*!< Size of this structure */ 
    
    /*! Parameter Mask indicating which of the below fields are valid */
    SWI_TYPE_SMS_ParamMask eParamMask;

    SWI_TYPE_SMS_MsgCategory eMsgCategory;    /*!< Message Category */
    SWI_TYPE_SMS_Teleservice eTeleservice;    /*!< Message Teleservice */
    
    /*! Service Category (News, Restaraunts, etc.). Refer to TIA/EIA-637 
         Section 3.4.3.2 for more detail. */
    swi_uint16 serviceCategory;

    /*! Indicates if the client needs to confirm whether the message is 
         received successfully or not.*/
    swi_uint8 tlAckReq;

    SWI_TYPE_SMS_MsgIdType eMsgIdType;      /*!< Message ID Type */
    swi_uint8 addrDigitMode;                /*!< Address Digit Mode */
    swi_uint8 addrNumMode;                  /*!< Address number mode */

    swi_uint8 addrNumType;                  /*!< Address number type */ 

    /*! Address number plan
           if NumMode == 0
               WMS_NUMBER_UNKNOWN        = 0,
               WMS_NUMBER_INTERNATIONAL  = 1,
               WMS_NUMBER_NATIONAL       = 2,
               WMS_NUMBER_NETWORK        = 3,
               WMS_NUMBER_SUBSCRIBER     = 4,

               WMS_NUMBER_ALPHANUMERIC   = 5,  
               GSM SMS: addr value is GSM 7-bit chars 

               WMS_NUMBER_ABBREVIATED    = 6,
               WMS_NUMBER_RESERVED_7     = 7,
           if NumMode == 1            
               WMS_NUMBER_DATA_IP        = 1
               WMS_NUMBER_INTERNET_EMAIL = 2
    */
    swi_uint8 addrNumPlan;

    swi_uint8 addrLen;                      /*!< Length of Address */
    
    /*! Destination address if we are sending out a MO SMS */
    swi_char szAddress[MAX_SMSI_TL_ADDRESS];

    swi_uint8 subType;                      /*!< Sub address type */
    swi_uint8 subOdd;                       /*!< Sub Address Odd byte */
    swi_uint8 subLen;                       /*!< Length of the sub-address */
    
    /*! Destination sub-address if we are sending out a MO SMS. 
         Origination sub-address if we are retrieving a MT SMS */
    swi_char szSubAddress[MAX_SMSI_TL_ADDRESS];

    swi_uint8 callBackLen;              /*!< Length of the Callback number */
    
    /*! String containing the Call Back number with a 32 maximum characters */
    swi_char szCallBack[MAX_SMSI_CALL_BACK];

    SWI_TYPE_SMS_Priority ePriority;        /*!< Message Priority  */
    SWI_TYPE_SMS_Privacy ePrivacy;          /*!< Message Privacy Level */
    SWI_TYPE_SMS_Language eLanguage;        /*!< Message Language */
    
    /*! Specify whether user Acknowledgement is required True/False */
    swi_uint8 user_ack_req;

    /*! Specify whether delivery acknowledge is required */
    swi_uint8 dak_req;

    swi_uint8 userResp;             /*!< User Response Code */
    
    /*! Number of Voice messages (SMSI_VMN_95) on the network */
    swi_char szNumMsg[2];

    swi_uint8 deliveryAlert;        /*!< Alert on Delivery */
    
    /*! Message Body Data Encoding. */
    SWI_TYPE_SMS_DataEncodeType dataEncoding;

    /*! Time Stamp Year: the value is presented in decimal format. */
    swi_uint8 ts_yr;
    
    /*! Time Stamp Month: the value is presented in hexadecimal format. 
         E.g. December will be interpreted as 0x12 */
    swi_uint8 ts_mon;
    
    /*! Time Stamp Day- the value is presented in hexadecimal format. 
         E.g. 30th will be interpreted as 0x30. */
    swi_uint8 ts_day;

    /*! Time Stamp Hour - the value is presented in hexadecimal format. 
         E.g. eleven o'clock is presented as 0x0x11. Represented in 24-hour 
         time. */
    swi_uint8 ts_hrs;

    /*! Time Stamp Minute - the value is presented in hexadecimal format. 
         E.g. thirty minute is presented as 0x30. */
    swi_uint8 ts_min;

    /*! Time Stamp Second - the value is presented in hexadecimal format. 
         E.g. thirty minute is presented as 0x30. */
    swi_uint8 ts_sec;

    /*! Absolute Validity Year - the value is presented in decimal format. */
    swi_uint8 abVal_yr;

    /*! Absolute Validity Month - the value is presented in hexadecimal 
         format. E.g. December will be interpreted as 0x12. */
    swi_uint8 abVal_mon;
    
    /*! Absolute Validity Day- the value is presented in hexadecimal format. 
         E.g. 30th will be interpreted as 0x30. */
    swi_uint8 abVal_day;
    
    /*! Absolute Validity Hour - the value is presented in hexadecimal 
         format. E.g. eleven o'clock is presented as 0x0x11. Represented 
         in 24-hour time. */
    swi_uint8 abVal_hrs;
    
    /*! Absolute Validity Minute - the value is presented in hexadecimal 
         format. Thus, thirty minute is presented as 0x30. */
    swi_uint8 abVal_min;
    
    /*! Absolute Validity Second - the value is presented in hexadecimal 
         format. E.g. thirty minute is presented as 0x30. */
    swi_uint8 abVal_sec;
    
    /*! Relative Validity Time of a SMS transmission. 
         See TIA/EIA 637-A Section 4.5.6 for more detail. */
    swi_uint8 relVal_time;
    
    /*! Absolute Delivery Deferral Year - the value is presented in decimal 
         format. */
    swi_uint8 abDef_yr;
    
    /*! Absolute Delivery Deferral Month - the value is presented in 
         hexadecimal format. E.g. December will be interpreted as 0x12. */
    swi_uint8 abDef_mon;
    
    /*! Absolute Delivery Deferral Day - the value is presented in 
         hexadecimal format. E.g. 30th will be interpreted as 0x30. */
    swi_uint8 abDef_day;
    
    /*! Absolute Delivery Deferral Hour - the value is presented in 
         hexadecimal format. E.g. eleven o'clock  is presented as 0x0x11. 
         Represented in 24-hour time. */
    swi_uint8 abDef_hrs;
    
    /*! Absolute Delivery Deferral Minute - the value is presented in 
         hexadecimal format. E.g. thirty minute is presented as 0x30. */
    swi_uint8 abDef_min;

    /*! Absolute Delivery Deferral Second - the value is presented in 
         hexadecimal format. E.g. thirty minute is presented as 0x30. */
    swi_uint8 abDef_sec;
    
    /*! Relative Delivery Deferral Time of a SMS transmission. 
         See Reference 1 Section 4.5.8 for more detail. */
    swi_uint8 relDef_time;

    SWI_TYPE_SMS_MsgDispMode eMsgDispMode;  /*!< Message Display Mode */
}SWI_STRUCT_SMS_CdmaHeader;

/**
 *
 * Contains CDMA SMS header.
 *
 * This structure is used to provide input to, and receive output from the 
 * SwiRetrieveSMSMessage function.
 *
 */
typedef struct SWI_STRUCT_CDMA_SMS_RetrieveSms
{
    swi_uint32 sizeStruct;      /*!< Size of this structure */
    swi_bool bUnreadSMS;            /*!< Not used */
    
    /*! The SMS header for this message. */
    SWI_STRUCT_SMS_CdmaHeader sHeader;

    swi_uint16 nSMSId;  /*!< The message ID number assigned by the network. */

    /*! The number of SMS messages remaining in the modem (all three queues 
         combined). */
    swi_uint32 cntRemainingSMS;
    
    /*! A pointer provided by the application in which to place the body of 
         the SMS message. */
    swi_uint8 *pMessage;
    
    /*! The length in bytes of the buffer supplied to receive the body of 
         the SMS message. This should be based on the maximum size of message 
         supported by the network. The API currently supports up to 240 bytes, 
         although most networks limit this to a lower value. */
    swi_uint32 sizeBuffer;

     /*! For future use; ignore. On return , this parameter will be set to 
          the no. of bytes copied into the pMessage buffer. */
    swi_uint32 retrievedStatus;
}SWI_STRUCT_CDMA_SMS_RetrieveSms;

/**
 *
 * Contains SMS Send Status notification data structure.
 *
 * This data structure is used as a member of the notification 
 * SWI_NOTIFY_SmsSendStatus.  Refer to the description of
 * SWI_NOTIFY_SmsSendStatus for more detail.
 *
 */
typedef struct SWI_STRUCT_CDMA_SMS_SendStatus
{
    swi_uint32 sizeStruct;      /*!< Size of this structure */ 
    
    /*! Message ID of the corresponding SMS that had been sent */
    swi_uint32 nMsgID;

    SWI_TYPE_SMS_ErrorClass eErrorClass;    /*!< Error type */
    SWI_TYPE_SMS_CauseCode  eCauseCode;     /*!< Error cause code */
}SWI_STRUCT_CDMA_SMS_SendStatus;

/**
 *
 * Contains data used to store SMS.
 *
 * This structure is used to provide input to the 
 * SwiStoreCdmaSMSMessage function.
 *
 */
typedef struct SWI_STRUCT_CDMA_SMS_StoreSms
{
    swi_uint32 sizeStruct;  /*!< Size of this structure */         

    /*! This is a unique identifer set by the application. The onus is on 
        the application to ensure message IDs are not duplicated. This ID
        is used to report the status of sent messages via 
        SWI_NOTIFY_SmsSendStatus. */
    swi_uint16 nSMSId;

    /*! he SMS header for this message, as described on */ 
    SWI_STRUCT_SMS_CdmaHeader sSMSHeader;
    
    /*! The body of the SMS message. The true length is stored in another 
        field (limited to 240 bytes by the API) */
    swi_uint8 *pMsgBody;
    
    /*! The length of the message body in bytes up to 240, or a lower limit 
        imposed by your service provider.*/
    swi_uint32 sizeSMSBody;

    swi_uint32 bStoreStatus;    /*!< For future use; ignore. */
}SWI_STRUCT_CDMA_SMS_StoreSms;

/**
 *
 * Contains SMS header structure.
 *
 * Note: While this appears redundant to SWI_STRUCT_SMS_CdmaHeader,
 * they serve two different purposes.  The below header is used
 * to cast the payload, hence the strict definition of the member data types.
 * SWI_STRUCT_SMS_CdmaHeader uses enum's in its members, to enforce proper
 * setting by the developer for SDK/API use.
 *
 */
typedef struct CDMA_SMS_HEADER
{
    /*! Parameter mask for the available fields */
    swi_uint32 paramMask;


    /*! Message Catagory (Point-to-Point/Broadcast/Acknowledge)
            0 :IS-637 SMS Point-to-Point Message
            1 :IS-637 SMS Broadcast Message
            2 :IS-637 SMS Acknowledge Message
     */
    swi_uint8 msgCategory;    

    /*! Teleservice identifier (CMT-91/CPT-95/CMP-95/VMN-95/WAP)
       Refer to TIA/EIA-41-D Table 175 for identifier values */
    swi_uint8 teleservice;     

    /*! Service Category (News, Restaraunts, etc...) */
    swi_uint16 serviceCategory;

    /*! TL Ack Request */
    swi_uint8 tlAckReq;

    /*! Message ID Type */
    swi_uint8 msgIdType;

    /*! Destination/Origination Address */
    swi_uint8 addrDigitMode;
    swi_uint8 addrNumMode;
    swi_uint8 addrNumType;
    swi_uint8 addrNumPlan;
    swi_uint8 addrLen;
    swi_uint8 address[MAX_SMSI_TL_ADDRESS];

    /*! Destination/Origination Sub-address */
    swi_uint8 subType;
    swi_uint8 subOdd;
    swi_uint8 subLen;
    swi_uint8 subAddress[MAX_SMSI_TL_ADDRESS];

    /*! Callback number */
    swi_uint8 callBackLen;
    swi_uint8 callBack[MAX_SMSI_CALL_BACK];

    /*! Priority Indicator
        0: Normal
        1: Interactive
        2: Urgent
        3: Emergency
     */
    swi_uint8 priority;    

    /*! Privacy Incidator
        0: Not restricted (Level 0)
        1: Restricted (Level 1)
        2: Confidential (Level 2) 
        3: Secret (Level 3)
     */
    swi_uint8 privacy;     

    /*! Language. Refer to TSB58-A */
    swi_uint8 language;

    /*! Reply Option */
    swi_uint8 user_ack_req;
    swi_uint8 dak_req;

    /*! User Response */
    swi_uint8 userResp;

    /*! Number of message (for Voice Message SMS) */
    swi_uint8 numMsg[2];

    /*! Delivery Alert Enable */
    swi_uint8 deliveryAlert;

    /*! Message Body Data encoding */
    swi_uint8 dataEncoding;

    /*! Below are time Stamp */
    swi_uint8 ts_yr;        
    swi_uint8 ts_mon;        
    swi_uint8 ts_day;        
    swi_uint8 ts_hrs;        
    swi_uint8 ts_min;
    swi_uint8 ts_sec;

    /*! Below are absolute Validity Period */
    swi_uint8 abVal_yr;
    swi_uint8 abVal_mon;
    swi_uint8 abVal_day;
    swi_uint8 abVal_hrs;
    swi_uint8 abVal_min;
    swi_uint8 abVal_sec;

    /*! Relative Validity Period */
    swi_uint8 relVal_time;

    /*! Below are bsolute Deferred Delivery Time */
    swi_uint8 abDef_yr;
    swi_uint8 abDef_mon;
    swi_uint8 abDef_day;
    swi_uint8 abDef_hrs;
    swi_uint8 abDef_min;
    swi_uint8 abDef_sec;

    /*! Relative Deferred Delivery Time */
    swi_uint8 relDef_time;

    /*! Message Display Mode */
    swi_uint8 msgDispMode;

}CDMA_SMS_HEADER;

/**
 *
 * Contains general SMS header data.
 *
 */
typedef struct CNS_SMS_HEADER
{
    swi_uint16 messageID;   /*!< SMS msg ID */
    swi_uint8 seqNumber;    /*!< Message sequence number (modulo 256) */

    /*! 0 : intermediate
        1 : first frame
        2 : last frame 
        3 : not fragmented 
     */
    swi_uint8 packetType;   

    swi_uint8 remainSms;    /*!< Number of remaining SMS */
    swi_uint8 length;       /*!< Length of this SMS part */
}CNS_SMS_HEADER;

/* Function prototypes */

SWI_API SWI_RCODE SwiPriGetSMSMask(swi_uint16* smsMask, swi_uint32 timeout);

SWI_API SWI_RCODE SwiPriGetMaxMosmsLen(swi_uint16* puMaxLen,
                                       swi_uint32 timeout);

SWI_API SWI_RCODE SwiSendCdmaSMS(swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetCdmaSMSMessageStatus(
                                      SWI_STRUCT_CDMA_SMS_Status *pSMS_Status,
                                      swi_uint32 timeout);

SWI_API SWI_RCODE SwiRetrieveCdmaSMSMessage(
                                      SWI_STRUCT_CDMA_SMS_RetrieveSms *pSMS,
                                      swi_uint32 timeout);

SWI_API SWI_RCODE SwiRetrieveSMSMessageEx(
                                      SWI_STRUCT_CDMA_SMS_RetrieveSms *pSms,
                                      swi_bool bDeleteAfterRetrieve,
                                      swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetMessageRetrieved(swi_uint16 msgID, swi_uint32 timeout);

SWI_API SWI_RCODE SwiStoreCdmaSMSMessage(
                                      SWI_STRUCT_CDMA_SMS_StoreSms *pStoreSMS,
                                      swi_uint32 timeout);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __SWIAPICDMASMS_H__ */

/* 
 * $Log: SwiApiCdmaSms.h,v $
 */
