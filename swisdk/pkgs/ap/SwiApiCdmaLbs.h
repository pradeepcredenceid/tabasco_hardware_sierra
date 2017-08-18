/**
 *    \if CvsId 
 *    $Id: SwiApiCdmaLbs.h,v 1.5 2011/02/11 22:09:01 ygao Exp $
 *    \endif
 *    \ingroup cdmalbs
 *    \file SwiApiCdmaLbs.h  
 * 
 *    \brief SWI CDMA specific LBS related APIs.
 *
 *    Copyright (c) 2011 Sierra Wireless, Inc.  All rights reserved
 *
 */

#ifndef __SWIAPICDMALBS_H__
#define __SWIAPICDMALBS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* include files */
#include "SwiDataTypes.h"
#include "SwiRcodes.h"
#include "SwiApiCmLbs.h"
#include "SwiApiCdmaLbs.h"

/* GPS enhancement */

/**
 *
 * Contains LBS fix type control data.
 *
 */
typedef struct SWI_STRUCT_LBS_FixTypeControl
{
    /*! Enable/Disable Standalone Mode Support (0x01/0x00) */
    swi_uint8 bStandalone;

    /*! Enable/Disable MS-Based Mode Support (0x01/0x00) */
    swi_uint8 bMSBased;

    /*! Enable/Disable MS-Assisted Mode Support (0x01/0x00) */
    swi_uint8 bMSAssisted;

    /*! Enable/Disable Optimal Speed Mode Support (0x01/0x00) */
    swi_uint8 bOptimalAccuracy; 

    /*! Enable/Disable Optimal Accuracy Mode Support (0x01/0x00) */
    swi_uint8 bOptimalSpeed;

    /*! Enable/Disable Optimal Data Mode Support (0x01/0x00) */
    swi_uint8 bOptimalData;
}SWI_STRUCT_LBS_FixTypeControl;

/**
 *
 * An enumeration of LBS auto-track support mask.
 *
 */
enum SWI_TYPE_AUTOTRACK_SUPPORT
{
    SWI_AUTOTRACK_NOT_ALLOWED = 0x00, /*!< No auto tracking is allowed */

    /*! Auto tracking is allowed will start immediately if enabled. */
    SWI_AUTOTRACK_ALLOWED     = 0x01,

    /*! Auto tracking is allowed and is tied to start/stop of data 
         connection if enabled.*/
    SWI_AUTOTRACK_ON_CONNECT  = 0x02
};

/**
 *
 * Contains LBS auto-start data.
 *
 */
typedef struct SWI_STRUCT_LbsAutoStart
{
    swi_uint8  nAllowed;       /*!< see SWI_TYPE_AUTOTRACK_SUPPORT */
    
    /*! enables auto tracking if nAllowed == SWI_AUTOTRACK_ALLOWED or 
        SWI_AUTOTRACK_ON_CONNECT */
    swi_uint8  bEnabled;

    /*! see SWI_STRUCT_LbsPdTrack for definitions of this and the following 
        items */
    swi_uint16 nFixType;

    swi_uint16 nPerformance;    /*!< Performance to use for LBS fix */
    swi_uint32 nAccuracy;       /*!< nAccuracy to use for LBS fix */
    swi_uint16 nFixCount;       /*!< nFixCount to use for LBS fix */
    swi_uint32 nFixRate;        /*!< nFixRate to use for LBS fix */
}SWI_STRUCT_LbsAutoStart;

/* GPS enhancement end */

/* Function prototypes */

SWI_API SWI_RCODE SwiGetLbsLocNotifyStatus(
        SWI_STRUCT_LbsLocNotifyStatus *psNotifyStatus, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetLbsLocNotifyStatus(
        const SWI_STRUCT_LbsLocNotifyStatus *psNotifyStatus, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetLbsDloadNotifyStatus(
        SWI_STRUCT_LbsDloadNotifyStatus *psNotifyStatus, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetLbsDloadNotifyStatus(
        const SWI_STRUCT_LbsDloadNotifyStatus *psNotifyStatus, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetLbsParamNotifyStatus(
        SWI_STRUCT_LbsParamNotifyStatus *psNotifyStatus, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetLbsParamNotifyStatus(
        const SWI_STRUCT_LbsParamNotifyStatus *psNotifyStatus, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetLbsPdDownload(
        const SWI_STRUCT_LbsPdDownload *psLbsPdDownload, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetLbsPaGpsLock(
        SWI_STRUCT_LbsPaGpsLock *psPaGpsLock, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetLbsPaGpsLock(
        const SWI_STRUCT_LbsPaGpsLock *psPaGpsLock, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetLbsPaPtlmMode(
        SWI_STRUCT_LbsPaPtlmMode *psPaPtlmMode, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetLbsPaPtlmMode(
        const SWI_STRUCT_LbsPaPtlmMode *psPaPtlmMode, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetLbsPaPrivacy(
        SWI_STRUCT_LbsPaPrivacy *psPaPrivacy, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetLbsPaPrivacy(
        const SWI_STRUCT_LbsPaPrivacy *psPaPrivacy, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetLbsPaNetAccess(
        SWI_STRUCT_LbsPaNetAccess *psPaNetAccess, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetLbsPaNetAccess(
        const SWI_STRUCT_LbsPaNetAccess *psPaNetAccess, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetLbsPaBsInfo(
        SWI_STRUCT_LbsPaBsInfo *psBsInfo, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetLbsSmartModeControl(swi_uint8 *pnMask, 
                                            swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetLbsSmartModeControl(swi_uint8 nMask, 
                                            swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetLbsSWSmartModeControl(swi_uint8 *pnMask, 
                                              swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetLbsSWSmartModeControl(swi_uint8 nMask, 
                                              swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetLbsFixTypeControl(
        SWI_STRUCT_LBS_FixTypeControl *pFixTypeControl, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetLbsFixTypeControl(
        const SWI_STRUCT_LBS_FixTypeControl *pFixTypeControl, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetLbsCLMControl(swi_uint8 *pnMask, swi_uint32 timeout);
SWI_API SWI_RCODE SwiSetLbsCLMControl(swi_uint8 nMask, swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetLbsNemaOutputMessageControl(swi_uint8 *pnMask, 
                                                    swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetLbsNemaOutputMessageControl(swi_uint8 nMask, 
                                                    swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetLbsAutoStart(SWI_STRUCT_LbsAutoStart *psLbsAutoStart, 
                                     swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetLbsAutoStart(
        const SWI_STRUCT_LbsAutoStart *psLbsAutoStart, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetLbsFixTypeSupportStatus(swi_uint8 *pStatus, 
                                                swi_uint32 nLen, 
                                                swi_uint32 timeout);

/* === GPS One Xtra === */

typedef enum LBS_XTRA_ASSIST_DATA_OP_ASSISTANCE_STATUS
{
    LBS_XTRA_ASSIST_DATA_OP_ASSISTANCE_STATUS_INVALID = 0x00, /*!< Invalid */
    LBS_XTRA_ASSIST_DATA_OP_ASSISTANCE_STATUS_VALID,          /*!< Valid */
} LBS_XTRA_ASSIST_DATA_OP_ASSISTANCE_STATUS;

typedef enum LBS_XTRA_ASSIST_DATA_OP
{
	/*!< Data download */
    LBS_XTRA_ASSIST_DATA_OP_ASSISTANCE_DATA_DOWNLOAD = 0x00,

    LBS_XTRA_ASSIST_DATA_OP_ASSISTANCE_EFS_FILE,            /*!< EFS file */
    LBS_XTRA_ASSIST_DATA_OP_ASSISTANCE_DOWNLOAD_FAILED,     /*!< Download failed */
} LBS_XTRA_ASSIST_DATA_OP;

typedef enum LBS_XTRA_ASSIST_DATA_RESULT
{
    LBS_XTRA_ASSIST_DATA_RESULT_NO_ERROR = 0x00,        /*!< No error */
    LBS_XTRA_ASSIST_DATA_RESULT_NOT_SUPPORTED,          /*!< NOt supported */
    LBS_XTRA_ASSIST_DATA_RESULT_UNABLE_TO_ACCESS_EFS,   /*!< Unable access */
    LBS_XTRA_ASSIST_DATA_RESULT_IN_PROGRESS,            /*!< In progress */
    LBS_XTRA_ASSIST_DATA_RESULT_INCORRECT_PARAMETER,    /*!< Wrong parameter */
    LBS_XTRA_ASSIST_DATA_RESULT_DOWNLOAD_NOT_PERMITTED, /*!< Not permitted */
} LBS_XTRA_ASSIST_DATA_RESULT;

typedef enum LBS_XTRA_ASSIST_DATA_NOTIFY_STATUS
{
    LBS_XTRA_ASSIST_DATA_NOTIFY_STATUS_SUCCESS = 0x00,    /*!< Success */
    LBS_XTRA_ASSIST_DATA_NOTIFY_STATUS_BAD_CRC,           /*!< Bad CRC */

    /*!< Bad buffer Length */
    LBS_XTRA_ASSIST_DATA_NOTIFY_STATUS_BAD_BUFFER_LENGTH,

    LBS_XTRA_ASSIST_DATA_NOTIFY_STATUS_BAD_TIME_RANGE, /*!< Bad Timer range */
    LBS_XTRA_ASSIST_DATA_NOTIFY_STATUS_FAILURE,        /*!< Failed */
    LBS_XTRA_ASSIST_DATA_NOTIFY_STATUS_GPS_BUSY,       /*!< GPS busy */

    /*!< E911 in progress */
    LBS_XTRA_ASSIST_DATA_NOTIFY_STATUS_E911_IN_PROGRESS,

    LBS_XTRA_ASSIST_DATA_NOTIFY_STATUS_RETRY_EXCEEDED,  /*!< Retry exceeded */
    LBS_XTRA_ASSIST_DATA_NOTIFY_STATUS_DATA_TIMEOUT,    /*!< Timeout */

    /*!< File time unchanged */
    LBS_XTRA_ASSIST_DATA_NOTIFY_STATUS_FILE_TIME_UNCHANGED,
} LBS_XTRA_ASSIST_DATA_NOTIFY_STATUS;

typedef enum LBS_XTRA_TIME_INFO_SET_OP
{
    LBS_XTRA_TIME_INFO_SET_SNTP_SERVER = 0x00,  /*!< SNTP server */
    LBS_XTRA_TIME_INFO_SET_GSM_UMTS_NETWORK,    /*!<  GSM/UMTS network */
    LBS_XTRA_TIME_INFO_SET_HOST,                /*!< Host */
} LBS_XTRA_TIME_INFO_SET_OP;

typedef enum LBS_XTRA_TIME_INFO_SET_TIME_REF
{
    LBS_XTRA_TIME_INFO_SET_GPS = 0x00,          /*!< GPS */
    LBS_XTRA_TIME_INFO_SET_UTC,                 /*!< UTC */
} LBS_XTRA_TIME_INFO_SET_TIME_REF;

typedef enum LBS_XTRA_DLOAD_TRIGGER_OP
{
    LBS_XTRA_DLOAD_TRIGGER_OP_HTTP = 0x00,      /*!< HTTP */
    LBS_XTRA_DLOAD_TRIGGER_OP_SNTP,             /*!< SNTP */
} LBS_XTRA_DLOAD_TRIGGER_OP;

typedef struct SWI_STRUCT_LBS_XTRA_ASSIST_DATA_GET
{
	/*!< Assistance data status */
    LBS_XTRA_ASSIST_DATA_OP_ASSISTANCE_STATUS assistanceDataStatus;

    /*!< YYYYMMDDdHHMMMSS<NULL> where d = day of week (0=Mon - 6=Sun) */
    swi_char assistanceDataTimeStamp[16];

    /*!< Same format as assistanceDataTimeStamp */
    swi_char assistanceDataTimeStampExpiry[16];
} SWI_STRUCT_LBS_XTRA_ASSIST_DATA_GET;

typedef struct SWI_STRUCT_LBS_XTRA_ASSIST_DATA_SET
{
    LBS_XTRA_ASSIST_DATA_OP    operation;/*!<  */
    swi_char                    assistanceDataFilename[8];/*!<  */
} SWI_STRUCT_LBS_XTRA_ASSIST_DATA_SET;

typedef struct SWI_STRUCT_LBS_XTRA_TIME_INFO_SET
{
    LBS_XTRA_TIME_INFO_SET_OP operation;            /*!< Operation */
    swi_uint64 time;                                /*!< In msec */
    swi_uint32 timeUncertainty;                     /*!< In msec */
    LBS_XTRA_TIME_INFO_SET_TIME_REF timeReference;  /*!< Time reference */

    /*!< only valid if operation is LBS_XTRA_TIME_INFO_SET_HOST (0x02) */
    swi_uint16 timeForce;
} SWI_STRUCT_LBS_XTRA_TIME_INFO_SET;

typedef struct SWI_STRUCT_LBS_XTRA_SERVER_URL
{
    swi_uint8 numberOfRetries;          /*!<  */
    swi_char serverURL[128];            /*!<  <NULL> terminated */

    /*!< 0 = no more; 1 = more URLs available; should be an enum. */
    swi_uint8 moreURLs;
} SWI_STRUCT_LBS_XTRA_SERVER_URL;

typedef struct SWI_STRUCT_LbsXtraAssist
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */
    LBS_XTRA_ASSIST_DATA_NOTIFY_STATUS status;    /*!< Status */
} SWI_STRUCT_LbsXtraAssist;

typedef struct SWI_STRUCT_LbsXtraDloadTrigger
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */
    LBS_XTRA_DLOAD_TRIGGER_OP operation;    /*!< Operation */

    /*!< SNTP 1-way delay failover threshold (msec); only valid when
      operation is LBS_XTRA_DLOAD_TRIGGER_OP_SNTP */
    swi_uint32 failoverThreshold;
} SWI_STRUCT_LbsXtraDloadTrigger; // LBS_XTRA_DLOAD_TRIGGER_NOTIFY_Packed;

SWI_API SWI_RCODE SwiGetLbsXtraAssistData( SWI_STRUCT_LBS_XTRA_ASSIST_DATA_GET *data,
                                   swi_uint32 timeout );

SWI_API SWI_RCODE SwiSetLbsXtraAssistData( SWI_STRUCT_LBS_XTRA_ASSIST_DATA_SET *data,
                                   LBS_XTRA_ASSIST_DATA_RESULT *response,
                                   swi_uint32 timeout );

SWI_API SWI_RCODE SwiSetLbsXtraTimeInfo( SWI_STRUCT_LBS_XTRA_TIME_INFO_SET *data,
                                 LBS_XTRA_ASSIST_DATA_RESULT *response,
                                 swi_uint32 timeout );

SWI_API SWI_RCODE SwiGetLbsXtraDataServerURL( SWI_STRUCT_LBS_XTRA_SERVER_URL *data,
                                      swi_uint32 timeout );

SWI_API SWI_RCODE SwiGetLbsXtraTimeServerURL( SWI_STRUCT_LBS_XTRA_SERVER_URL *data,
                                      swi_uint32 timeout );

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __SWIAPICDMALBS_H__ */

/* 
 * $Log: SwiApiCdmaLbs.h,v $
 */

