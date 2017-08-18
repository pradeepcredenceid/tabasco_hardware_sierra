/**
 *    \if CvsId 
 *    $Id: SwiApiCdmaIota.h,v 1.5 2010/06/10 00:09:26 epasheva Exp $
 *    \endif
 *    \ingroup cdmaiota
 *    \file SwiApiCdmaIota.h 
 * 
 *    \brief SWI CDMA specific IOTA related APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */
#ifndef __SWIAPICDMAIOTA_H__
#define __SWIAPICDMAIOTA_H__

#ifdef __cplusplus
extern "C" {
#endif

/* include files */
#include "SwiDataTypes.h"
#include "SwiRcodes.h"
#include "SwiApiCdmaBasic.h"

#define LENGTH_WAPUri   128     /*!< Max URI length. */

/**
 *
 * An enumeration of embedded IOTA event type.
 *
 */
typedef enum SWI_EMIOTA_EVENT_TYPE 
{
    SWI_EMIOTA_SESSION_START,       /*!< 0:EM IOTA session started */
    SWI_EMIOTA_SESSION_END          /*!< 1:EM IOTA session end */
}SWI_EMIOTA_EVENT_TYPE;

/**
 *
 * An enumeration of embedded IOTA result.
 *
 */
typedef enum SWI_EMIOTA_EVENT_RESULT 
{
    SWI_EMIOTA_EVENT_RESULT_SUCCESS,    /*!< 0:event successful */
    SWI_EMIOTA_EVENT_RESULT_FAILURE     /*!< 1:event failed */
}SWI_EMIOTA_EVENT_RESULT;

/**
 *
 * An enumeration of embedded IOTA failure reason.
 *
 */
typedef enum SWI_EMIOTA_EVENT_FAIL_REASON 
{
    SWI_EMIOTA_STATUS_MIN = -1,             /*!< -1:IOTA status min */
    SWI_EMIOTA_STATUS_SUCCESS ,             /*!< 0:IOTA status success */
    SWI_EMIOTA_STATUS_ERROR_DISCONNECT,     /*!< 1:status error disconnect */

    /*! 2:status error iota session is not allowed */
    SWI_EMIOTA_STATUS_ERROR_NOT_ALLOWED,

    /*! 3:status error iota sesstion is already in progress */
    SWI_EMIOTA_STATUS_ERROR_ALREADY_ACTIVE,

    /*! 4:status error client aborted iota session */
    SWI_EMIOTA_STATUS_ERROR_CLIENT_ABORT,

    SWI_EMIOTA_STATUS_ERROR_HTTP,           /*!< 5:status error HTTP */
    SWI_EMIOTA_STATUS_ERROR_MIME,           /*!< 6:status error MIME */
    SWI_EMIOTA_STATUS_ERROR_XML,            /*!< 7:status error XML */
    SWI_EMIOTA_STATUS_ERROR_MMC,            /*!< 8:status error MMC */

    /*! 9:status error not trusted domain */
    SWI_EMIOTA_STATUS_ERROR_NOT_TRUSTED,

    /*! 10:status error session timed out */
    SWI_EMIOTA_STATUS_ERROR_SESSION_TIMEOUT,

    SWI_EMIOTA_STATUS_ERROR_UNKNOWN,        /*!< 11:status error unknown */
    SWI_EMIOTA_STATUS_ERROR_INTERNAL,       /*!< 12:status error internal */
    SWI_EMIOTA_STATUS_ERROR_NO_SERVICE,     /*!< 13:status error no service */

    /*! 14:status error user is in active data call */
    SWI_EMIOTA_STATUS_ERROR_ACTIVE_DATA_CALL,

    /*! 15:status error user is in active voice call */
    SWI_EMIOTA_STATUS_ERROR_ACTIVE_VOICE_CALL,

    /*! 16:status error user is no digital service ( roaming  mode ) */
    SWI_EMIOTA_STATUS_ERROR_NO_DIGITAL_SERVICE,

    SWI_EMIOTA_STATUS_MAX                   /*!< 17:iota status max */
}SWI_EMIOTA_EVENT_FAIL_REASON;

/**
 *
 * An enumeration of embedded IOTA failure error code.
 *
 * For HTTP Error - actual HTTP error code should be captured.
 *
 */
typedef enum SWI_EMIOTA_EVENT_ERR_CODE 
{
    SWI_EMIOTA_ERR_MMC_DISCONNECT,  /*!< 0:Normal Release (MMC "disconnect") */
    SWI_EMIOTA_ERR_INVALID_VALREF,  /*!< 1:MMC_RESULT_INVALID_VALUEREF */
    SWI_EMIOTA_ERR_CID_MISSING,     /*!< 2:MMC_RESULT_CID_MISSING */
    SWI_EMIOTA_ERR_DLD_FAILED,      /*!< 3:MMC_RESULT_DOWNLOAD_FAILED */
    SWI_EMIOTA_ERR_RESERVED,        /*!< 4:reserved */
    SWI_EMIOTA_ERR_INVALID_CONTENT, /*!< 5:MMC_RESULT_INVALID_CONTENT */
    SWI_EMIOTA_ERR_USR_DENIED,      /*!< 6:MMC_RESULT_USER_DENIED */
    SWI_EMIOTA_ERR_BAD_VALUE,       /*!< 7:MMC_RESULT_BAD_VALUE */
    SWI_EMIOTA_ERR_TIMEOUT,         /*!< 8:MMC_RESULT_TIMEOUT */
    SWI_EMIOTA_ERR_NONEW_DOC,       /*!< 9:MMC_RESULT_NEW_DOC_ARRIVED */
    SWI_EMIOTA_ERR_DELETE_FAILED,   /*!< 10:MMC_RESULT_DELETE_FAILED */
    SWI_EMIOTA_ERR_BAD_DATA,        /*!< 11:MMC_RESULT_BAD_DATA */
    SWI_EMIOTA_ERR_DISPLAY_DISABLED,/*!< 12:MMC_RESULT_DISPLAY_DISABLED */
    SWI_EMIOTA_ERR_WRITE_ONLY,      /*!< 13:MMC_RESULT_WRITE_ONLY */
    SWI_EMIOTA_ERR_RESULT_ABORT,    /*!< 14:MMC_RESULT_ABORT */
    SWI_EMIOTA_ERR_RESERVED1,       /*!< 15:reserved */
    SWI_EMIOTA_ERR_BAD_OBJECT,      /*!< 16:MMC_RESULT_BAD_OBJECT */
    SWI_EMIOTA_ERR_UNKNOWN_RESULT,  /*!< 17:MMC_RESULT_UNKNOWN_ERROR */
    SWI_EMIOTA_ERR_FILE_LOCKED,     /*!< 18:MMC_RESULT_FILE_LOCKED */
    SWI_EMIOTA_ERR_FILE_OPEN,       /*!< 19:MMC_RESULT_FILE_OPEN */
    SWI_EMIOTA_ERR_CHECK_FAILED,    /*!< 20:MMC_RESULT_CHECK_FAILED */
    SWI_EMIOTA_ERR_INVALID_SIZE,    /*!< 21:MMC_RESULT_INVALID_SIZE */
    SWI_EMIOTA_ERR_INVALID_OFFSET,  /*!< 22:MMC_RESULT_INVALID_OFFSET */
    SWI_EMIOTA_ERR_DEVICE_BUSY,     /*!< 23:MMC_RESULT_DEVICE_BUSY */
    SWI_EMIOTA_ERR_RESERVED2        /*!< 24:reserved */
}SWI_EMIOTA_EVENT_ERR_CODE;

/**
 *
 * Contains embedded ITOA notification data structure. 
 *
 * This data structure is used as a member of the notification
 * SWI_NOTIFY_IOTA_Status.  Refer to the description of
 * SWI_NOTIFY_IOTA_Status for more detail.
 *
 */
typedef struct SWI_STRUCT_IOTA_State
{
    swi_uint32 sizeStruct;      /*!< Size of this structure */

    /*! event type see SWI_EMIOTA_EVENT_TYPE for details */
    swi_uint16 iEvent;

    /*! event success or failure see SWI_EMIOTA_EVENT_RESULT for details */
    swi_uint16 iEventStatus;
               
    /*! Session end failure reason (future support) 
       see SWI_EMIOTA_EVENT_FAIL_REASON for details */
    swi_uint16 iSessionEndFailureReason;
               
    /*! Session end error code (depends on Param 3 Session  */
    swi_uint16 iSessionEndErrorCode;
            
    /* end failure reason) (future support) see SWI_EMIOTA_EVENT_ERR_CODE for
        details. For HTTP Error - actual HTTP error code MMC error.
        code see above 
     */

}SWI_STRUCT_IOTA_State;

/**
 *
 * Contains embedded ITOA WAP URI notification data structure. 
 *
 * This data structure is used as a member of the notification
 * SWI_NOTIFY_IOTA_WAPPush.  Refer to the description of
 * SWI_NOTIFY_IOTA_WAPPush for more detail.
 *
 */
typedef struct SWI_STRUCT_IOTA_WAPPush
{
    swi_uint32 sizeStruct;      /*!< Size of this structure */

    /*! null terminated URI contains wap trigger */
    swi_uint8 szWAPUri[LENGTH_WAPUri];
}SWI_STRUCT_IOTA_WAPPush;

/* Function prototypes */

SWI_API SWI_RCODE SwiGetEFileInfo(const swi_charp pszEFilename, 
                                  SWI_STRUCT_EFileInfo* psEFileInfo, 
                                  swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetEFile(const swi_charp pszEFilename, swi_uint8* pBuffer,
                              swi_uint16 nBufLen, swi_uint32 timeout);

SWI_API SWI_RCODE SwiStartIOTASession(swi_uint32 timeout);

SWI_API SWI_RCODE SwiStopIOTASession(swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetIOTADisable(swi_bool bDisable, swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetIOTADisable(swi_bool *pbDisable, swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetIOTALog(swi_uint8* pBuffer, swi_uint16 nBufLen,
                                swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetIOTALogSize(swi_uint16 *nBufLen, swi_uint32 timeout);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __SWIAPICDMAIOTA_H__ */

/* 
 * $Log: SwiApiCdmaIota.h,v $
 */
