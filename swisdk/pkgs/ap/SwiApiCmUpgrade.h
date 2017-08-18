/**
 *    \if CvsId 
 *    $Id: SwiApiCmUpgrade.h,v 1.10 2010/06/10 00:16:35 epasheva Exp $
 *    \endif
 *    \ingroup cmupgrade
 *    \file SwiApiCmUpgrade.h
 * 
 *    \brief This file contains definitions, enumerations, structures and
 *           forward declarations for SwiApiCmUpgrade.c file
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#ifndef __SWI_API_CM_UPGRADE_H__
#define __SWI_API_CM_UPGRADE_H__

#ifdef __cplusplus
extern "C" {
#endif

/* include files */
#include "SwiDataTypes.h"
#include "SwiRcodes.h"

/* defines */

/**
 *
 * An enumeration of NV backup operation types.
 *
 * \sa SWI_STRUCT_NvStore
 * \sa SWI_NOTIFY_NvStore
 *
 */
enum SWI_TYPE_NvOperation
{
    SWI_NVOPER_BackupAll,   /*!< NV backup all operation. */
    SWI_NVOPER_RestoreAll,  /*!< NV restore all operation. */
    SWI_NVOPER_Backup,      /*!< NV backup operation. */
    SWI_NVOPER_Restore      /*!< NV restore operation. */
};

/**
 *
 * An enumeration of NV category types.
 *
 * \sa SWI_STRUCT_NvStore
 * \sa SWI_NOTIFY_NvStore
 *
 */
enum SWI_TYPE_NvCategory
{
    SWI_NVCAT_Factory,  /*!< Factory settings. */
    SWI_NVCAT_Oem,      /*!< OEM settings. */
    SWI_NVCAT_User      /*!< User settings (default). */
};

/**
 *
 * This structure contains NV back up information. 
 *
 * \sa SWI_TYPE_NvOperation
 * \sa SWI_TYPE_NvCategory
 * \sa SWI_NOTIFY_NvStore
 * \sa SwiNvStore
 *
 */
struct SWI_STRUCT_NvStore
{
    /*! The size, in bytes, of this structure. */
    swi_uint32 sizeStruct;
    
    /*! This value is only valid with the SWI_NOTIFY_NvStore notification.  
        A value of "true" is returned if the back up succeed, false 
        otherwise. */
    swi_bool bSuccess;
    
    /*! Type of operation to perform.  Must be specified when doing a 
        backup/restore. */              
    enum SWI_TYPE_NvOperation eOperation;   
    
    /*! NV category.  Must be specified when doing a backup/restore. */
    enum SWI_TYPE_NvCategory    eCategory;
    
    /*! Items stored for backup operation or restored for restore operation,
        0 for other operations. */          
    swi_uint32 nStored;
    
    /*! Items skipped for backup operation only, 0 for other operations. */
    swi_uint32 nSkipped;
    
    /*! Items with 0 length for backup operation only, 0 for other 
        operations. */  
    swi_uint32 nZeroLength;         
};
/**
 *
 * Enumerates firmware image types.
 *
 * Supports all GSM devices.  CDMA MPs are also supported, other CDMA
 * devices are not supported.
 *
 * \sa SWI_STRUCT_FlashImgInfo
 * \sa  SwiFirmwareDownload
 *
 */
enum SWI_TYPE_FW_IMAGE
{
    FW_DWN_APPL,    /*!< Firmware application image */
    FW_DWN_BOOT,    /*!< Firmware bootloader image */
    FW_DWN_QCOM,    /*!< Qualcomm bootloader image */
    FW_DWN_USBD,    /*!< USB device descriptor image */
    FW_DWN_EXEC,    /*!< Firmware executable image */
    FW_DWN_SWOC     /*!< Software on card image */
};

/*!
 * An enumeration of firmware download states.  
 *
 * Supports all GSM devices.  CDMA MPs are also supported, other CDMA
 * devices are not supported.
 * 
 *  \sa SWI_STRUCT_FwDownload
 *  \sa SWI_NOTIFY_FwDownload
 *
 */
enum SWI_NOTIF_FW_TYPE
{       
    /*! Initializing and opening the specified image file to download.  */
    FW_INIT,
        
    /*! Initializing error occurred. */
    FW_INIT_ERR,
    
    /*! Resetting the modem. */
    FW_SOFT_RESET,
    
    /*! Timeout expired.  Failed to detect the modem after a reset. */
    FW_SOFT_RESET_ERR,

    /*! Sending the Start Firmware Download request. */
    FW_START_REQ,
    
    /*! A Firmware Download Start request error has occurred.  Possible 
        errors include:
            -# A firmware download is already in progress.  
            -# Failed to read from the firmware image file
            -# The modem rejected the Firmware Download Start request.
        Call SwiGetLastError to get additional information of the error. */
    FW_START_REQ_ERR,
    
    /*! Error reading from firmware file. */
    FW_FILE_ERR,
    
    /*! Firmware download packets being sent. */
    FW_DWN,
    
    /*! A error occurred during a Firmware Download Continuation request.
        Possible errors include:
            -# Timeout waiting for response from modem.
            -# Failed to read from the firmware image file. */
    FW_DWN_ERR,
    
    /*! Calculating the check sum. */
    FW_CHKSUM,
    
    /*! Modem rejected the check sum value.  Possible causes:
            -# Firmware image may be corrupt.
            -# Loss of packet during the firmware download process. */
    FW_CHKSUM_ERR,
    
    /*! Sending the Program Flash request (Application image only) */
    FW_FLASH,
    
    /*! An error has occurred while sending the Program Flash request. */
    FW_FLASH_ERR,
    
    /*! Sending the Launch Fragment request (for all firmware image types
        except Application images). */
    FW_LAUNCH_FRAG,
    
    /*! An error has occurred while sending the Launch Fragment request. */
    FW_LAUNCH_FRAG_ERR,
            
    /*! Modem performing a reset. */
    FW_RESET,
    
    /*! Firmware reset error occurred.  The API was unable to detect the
        modem within a predefined timeout period after the modem reset. */
    FW_RESET_ERR,
            
    /*! Firmware download complete. */
    FW_DONE
} ;

/**
 *
 * This structure is used to notify firmware download states.
 *
 *  Supports all GSM devices.  CDMA MPs are also supported, other CDMA
 *  devices are not supported.
 *
 *  \sa SWI_NOTIF_FW_TYPE
 *  \sa SWI_NOTIFY_FwDownload
 *
 */
struct SWI_STRUCT_FwDownload
{
    /*! The size, in bytes, of this structure.  This value should always be
        populated by the calling application. */
    swi_uint32 sizeStruct;
        
    /*! Firmware download state.  See SWI_NOTIF_FW_TYPE for additional
        information. */ 
    enum SWI_NOTIF_FW_TYPE State;                        
};

/* prototypes */
extern SWI_API SWI_RCODE SwiNvStore(
    struct  SWI_STRUCT_NvStore sNvStore, 
    swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiFirmwareDownload(
    swi_charp szFileName,
    enum SWI_TYPE_FW_IMAGE eFwDwnType,
    swi_bool bMpDownload,
    swi_uint32 nHIPPayloadSize,
    swi_uint32 timeout);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /*__SWI_API_CM_UPGRADE_H__*/

/* 
 * $Log: SwiApiCmUpgrade.h,v $
 */
