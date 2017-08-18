/**
 *    \if CvsId 
 *    $Id: SwiApiGsmLbs.h,v 1.8 2010/06/10 00:18:03 epasheva Exp $
 *    \endif
 *    \ingroup gsmlbs
 *    \file SwiApiGsmLbs.h
 * 
 *    \brief  Header file for for GSM APIs related to the 
 *            Location Based Services
 *
 *    Copyright: © 2008 Sierra Wireless, Inc. all rights reserved
 *
 */
#ifndef __SWIAPI_GSM_LBS
#define __SWIAPI_GSM_LBS

#ifdef __cplusplus
extern "C" {
#endif

/* include files */
#include "SwiDataTypes.h"
#include "SwiRcodes.h"

#define MAX_NUM_LBS_SV        12 /*!< Maximum number of Satelites */
#define LENGTH_LbsReqId       33 /*!< Maximum requestor ID length*/
#define LENGTH_LbsClientName  33 /*!< Maximum client name length */

/**
 *
 * Enumeration of PA error values
 *
 * \sa SWI_STRUCT_LbsPaKeepWarmStart
 * \sa SWI_STRUCT_LbsPaKeepWarmStop
 *
 */
enum SWI_TYPE_LbsPaErr
{
    SWI_LBSPAERR_NoError,
    SWI_LBSPAERR_InternalError,
    SWI_LBSPAERR_InvalidClientId,
    SWI_LBSPAERR_ErrorInParamToSet,
    SWI_LBSPAERR_ErrorInLockType,
    SWI_LBSPAERR_PhoneOffline,
    SWI_LBSPAERR_ParamCommandAlreadyActive,
    SWI_LBSPAERR_IncorrectAppSpecificInfo
};

/**
 *
 * Structure containing Keep Warm start status.
 *
 * This object is only supported on UMTS devices.
 *
 * \sa SWI_TYPE_LbsPaErr
 *
 */
struct SWI_STRUCT_LbsPaKeepWarmStart
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct;
    
    /*! An enumeration of type SWI_TYPE_LbsPaErr representing the PA error
        state. */
    enum SWI_TYPE_LbsPaErr eStatus;
};

/**
 *
 * Skeleton structure returned when Keep Warm processing has begun.
 *
 * This object is only supported on UMTS devices.
 *
 * \sa SWI_NOTIFY_LbsPaWarmBegin
 *
 */
struct SWI_STRUCT_LbsPaWarmBegin
{
    swi_uint32 sizeStruct; /*!< The size, in bytes, of this structure. */
};

/**
 *
 * Enumeration of Keep Warm status
 *
 * \sa SWI_STRUCT_LbsPaWarmStatus
 *
 */
enum SWI_TYPE_LbsPaWarmStatus
{
    SWI_LBSPAWARMSTATUS_NotWarm,
    SWI_LBSPAWARMSTATUS_Warm
};

/**
 *
 * Structure containing Keep Warm status information.
 *
 * This object is only supported on UMTS devices.
 *
 * \sa SWI_TYPE_LbsPaWarmStatus
 * \sa SWI_NOTIFY_LbsPaWarmStatus
 * \sa SwiGetLbsPaWarmStatus
 *
 */
struct SWI_STRUCT_LbsPaWarmStatus
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct;
    
    /*! True is returned if Keep Warm processing is enabled, false otherwise.*/
    swi_bool bEnabled;
    
    /*! An enumeration of type SWI_TYPE_LbsPaWarmStatus representing the
        Keep Warm status. */
    enum SWI_TYPE_LbsPaWarmStatus eStatus;
};

/**
 *
 * Skeleton structure returned when Keep Warm processing has completed.
 *
 * \sa SWI_NOTIFY_LbsPaWarmDone
 *
 */
struct SWI_STRUCT_LbsPaWarmDone
{
    swi_uint32 sizeStruct; /*!< The size, in bytes, of this structure. */
};

/**
 *
 * Structure used to stop the Keep Warm processing.
 *
 * This object is only supported on UMTS devices.
 *
 * \sa SWI_TYPE_LbsPaErr
 *
 */
struct SWI_STRUCT_LbsPaKeepWarmStop
{
    /*! The size, in bytes, of this structure. */
    swi_uint32 sizeStruct;      
    
    /*! An enumeration of type SWI_TYPE_LbsPaErr representing the result of
        the Keep Warm stop request */       
    enum SWI_TYPE_LbsPaErr eStatus;
};


/**
 *
 * Structure containing LBS user options
 *
 * \sa SWI_STRUCT_LbsFixSettings
 *
 */
struct SWI_STRUCT_LbsUserOptions
{       
    /*! Indicates if the LBS option is readable by the user. */
    swi_bool bReadable;
    
    /*! Indicates if the LBS option is writable by the user. */
    swi_bool bWritable;
};

/**
 *
 * Enumeration of LBS setting status return codes.
 *
 */
enum SWI_TYPE_LbsSettingStatus
{
    SWI_LBSSETTINGSTATUS_Success
};

/**
 *
 * Structure containing LBS settings.
 *
 * This object allows the host to use the device as storage for location
 *  fix settings.  This allows the location fix settings to persist.
 * 
 * In the future if it possible that the initial values of these settings 
 * will be customized for specific carriers.  Carriers may also require that
 * some of these settings be read only.  However, until the requirements are
 * better understood this object will allow settings to be written.
 *
 * \sa SWI_STRUCT_LbsUserOptions
 * \sa SWI_TYPE_LbsFix
 * \sa SWI_TYPE_LbsSettingStatus
 * \sa SwiGetLbsFixSettings
 * \sa SwiSetLbsFixSettings
 *
 */
struct SWI_STRUCT_LbsFixSettings
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct; 
    
    /*! An enumeration of type SWI_STRUCT_LbsUserOptions representing fix type
        user options */
    struct SWI_STRUCT_LbsUserOptions sFixTypeUserOp;
    
    /*! An enumeration of type SWI_TYPE_LbsFix representing the fix type 
        setting */
    enum SWI_TYPE_LbsFix eFixType;
    
    /*! An enumeration of type SWI_STRUCT_LbsUserOptions representing the
        performance user option. */
    struct SWI_STRUCT_LbsUserOptions sPerformanceUserOp;
    
    /*! Time allowed for satellite acquisition in seconds. */
    swi_uint32 nPerformance;
    
    /*! An enumeration of type SWI_STRUCT_LbsUserOptions representing the
        accuracy user option. */
    struct SWI_STRUCT_LbsUserOptions sAccuracyUserOp;
    
    /*! Accuracy limit preferred in meters. */
    swi_uint32 nAccuracy;
    
    /*! An enumeration of type SWI_STRUCT_LbsUserOptions representing the
        fix count user option. */
    struct SWI_STRUCT_LbsUserOptions sFixCountUserOp;
    
    /*! Number of fixes to obtain between 1 and 999, use 1000 for continuous
        mode. */
    swi_uint32 nFixCount;
    
    /*! An enumeration of type SWI_STRUCT_LbsUserOptions representing the
        fix rate user option. */
    struct SWI_STRUCT_LbsUserOptions sFixRateUserOp;
    
    /*! Frequency of fixes in seconds. */
    swi_uint32 nFixRate;
    
    /*! An enumeration of type SWI_TYPE_LbsSettingStatus representing the
        result of configuration request. */ 
    enum SWI_TYPE_LbsSettingStatus eResult;
};

/**
 *
 * Enumeration of LBS network initiated request responses.
 *
 * \sa SWI_STRUCT_LbsNiReq
 * \sa SWI_NOTIFY_LbsNiReq
 * \sa SwiSetLbsNiReq
 *
 */
enum TYPE_LbsNiReqResp 
{
    /*! This value should be used with SwiSetLbsNiReq to signal the host has 
        rejected the network initiated request. */
    LBSNIREQRESP_Reject,
    
    /*! This value should be used with SwiSetLbsNiReq to signal the host has 
        accepted the network initiated request. */  
    LBSNIREQRESP_Accept,
    
    /*! This value should be used with SwiSetLbsNiReq to signal the host has
        canceled the network initiated request or the specified timeout
        period has expired. */
    LBSNIREQRESP_Cancel
};

/**
 *
 * Enumeration of LBS network initiated error codes.
 * 
 * Used by SWI_STRUCT_LbsNiReq to indicate the status of the SwiLbsSetNiReq
 * request.
 *
 * \sa SWI_STRUCT_LbsNiReq
 * \sa SwiSetLbsNiReq
 *
 */
enum TYPE_LbsNiReqErrCode 
{
    /*! Request was successful. */
    LBSNIREQERRCODE_NoError,
    
    /*! Network request ID provided is invalid. */
    LBSNIREQERRCODE_HandleMismatch,
    
    /*! Response was not received within the specified timeout period. */
    LBSNIREQERRCODE_UserRespTimeout,
    
    /*! No user response was required. */
    LBSNIREQERRCODE_NoUserRespReq
};

/**
 *
 * Enumeration of LBS network initiated notification types.
 * 
 * Used by SWI_STRUCT_LbsNiReq to indicate whether the SWI_NOTIFY_LbsNiReq
 * notification requires a user response.
 *
 * \sa SWI_STRUCT_LbsNiReq
 * \sa SWI_NOTIFY_LbsNiReq
 * \sa SwiSetLbsNiReq
 *
 */
enum TYPE_LbsNiReqNotif 
{
    /*! No user response required. */
    LBSNIREQNOTIF_NotifyOnly,
    
    /*! User must respond with SwiSetLbsNiReq. */
    LBSNIREQNOTIF_UserRespReq
};

/**
 *
 * Enumeration of LBS network initiated requester ID types.
 *
 */
enum TYPE_LbsNiReqId 
{
    LBSNIREQID_Unknown,
    LBSNIREQID_LogicalName,
    LBSNIREQID_Msisdn,
    LBSNIREQID_Email,
    LBSNIREQID_Url,
    LBSNIREQID_SipUrl,
    LBSNIREQID_ImsPublicId,
    LBSNIREQID_Min,
    LBSNIREQID_Mdn
};

/**
 *
 * Enumeration of LBS network initiated client name types.
 *
 */
enum TYPE_LbsNiReqClientName
{
    LBSNIREQCLIENTNAME_Unknown,
    LBSNIREQCLIENTNAME_LogicalName,
    LBSNIREQCLIENTNAME_Msisdn,
    LBSNIREQCLIENTNAME_Email,
    LBSNIREQCLIENTNAME_Url,
    LBSNIREQCLIENTNAME_SipUrl,
    LBSNIREQCLIENTNAME_ImsPublicId,
    LBSNIREQCLIENTNAME_Min,
    LBSNIREQCLIENTNAME_Mdn
}; 

/**
 *
 * This object contains network initiated location fix request information.
 * 
 * SWI_NOTIFY_LbsNiReq uses this object to notify the host of a network
 * initiated location fix request.  
 * 
 * SwiLbsSetNiReq uses this object to send the user's response to the modem.
 * This object also contains the response to the SwiSetLbsNiReq transaction.
 *
 * \sa SWI_NOTIFY_LbsNiReq
 * \sa SwiSetLbsNiReq
 *
 */
struct SWI_STRUCT_LbsNiReq
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct;

    /*! Notification ID.  This value is returned in SWI_NOTIFY_LbsNiReq and
        must be supplied when calling SwiSetLbsNiReq. */
    swi_uint32  nNiId;

    /*! An enumeration of type TYPE_LbsNiReqResp, this value must be supplied
        by the caller when calling SwiSetLbsNiReq.  This value represents the
        response the user has chosen.  See notes above.  This field is not
        valid with SWI_NOTIFY_LbsNiReq. */
    enum TYPE_LbsNiReqResp eReqResp; 

    /*! An enumeration of type TYPE_LbsNiReqErrCode which indicates the status
        of the SwiSetLbsNiReq request.  This field is not valid with 
        SWI_NOTIFY_LbsNiReq.*/
    enum TYPE_LbsNiReqErrCode eErrCode;

    /*! Timeout period allocated for the host to respond to the network 
        initiated location fix request.  Valid ranges are between 0x0000 -
        0xFFFF.  If the host does not respond within this specified timeout
        period, the modem will assume the host has canceled the request. */
    swi_uint32  nTimeout;

    /*! An enumeration of type TYPE_LbsNiReqNotif which indicates whether a
        user response is required.  This field is ignored when used with
        SwiSetLbsNiReq. */
    enum TYPE_LbsNiReqNotif eNotifType;

    /*! An enumeration of type TYPE_LbsNiReqId which indicates the format of
        szReqId.  This field is ignored when used with SwiSetLbsNiReq. */
    enum TYPE_LbsNiReqId eReqIdType;

    /*! A NULL terminated string containing the requester ID.  */
    swi_char szReqId[LENGTH_LbsReqId];

    /*! An enumeration of type TYPE_LbsNiReqClientName which indicates the
        format of szClientName.  This field is ignored when used with 
        SwiSetLbsNiReq. */
    enum TYPE_LbsNiReqClientName eClientName;

    /*! A NULL terminated string containing the client name.  */
    swi_char szClientName[LENGTH_LbsClientName];
};

/* prototypes */
extern SWI_API SWI_RCODE SwiSetLbsPaKeepWarmStart(
    swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiGetLbsPaWarmStatus(
    struct SWI_STRUCT_LbsPaWarmStatus *psLbsPaWarmStatus, 
    swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiSetLbsPaKeepWarmStop(
    swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiGetLbsFixSettings(
    struct SWI_STRUCT_LbsFixSettings *psLbsFixSettings, 
    swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiSetLbsFixSettings(
    struct SWI_STRUCT_LbsFixSettings *psLbsFixSettings, 
    swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiSetLbsNiReq(
    struct SWI_STRUCT_LbsNiReq *pLbsNiReq,
    swi_uint32 timeout);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __SWIAPI_GSM_LBS */
/* 
 * $Log: SwiApiGsmLbs.h,v $
 */
