/**
 * \if CvsId
 * $Id: apidefs.h,v 1.21 2010/06/10 00:26:12 epasheva Exp $
 * \endif
 * 
 * \file     apidefs.h
 *
 * \brief    This file contains internal structures and constants for the 
 *           API side image
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 */

#ifndef __APIDEFS_H__
#define __APIDEFS_H__

/* Includes */
#include "SwiDataTypes.h"    /*!< system wide typedefs */
#include "SwiApiCmBasic.h"
#include "am/amudefs.h"
#include "cn/cnudefs.h"
#include "us/usudefs.h"
#include "ds/dsudefs.h"
#include "er/erudefs.h"
#include "pi/piudefs.h"
#include "sl/sludefs.h"
#include "os/swi_osapi.h"   /*!< package user include file  */
#include "SwiApiUser.h"

#include "ap/apudefs.h"  


/* Defines */

/* Base RSSI in dBm */
#define GSMRSSIBASE  -110   /*!< Basic reported GSM value in dBm */

/* Ranges for bars to display based upon the RSSI Level */
#define RSSI_LEVEL0  -109   /*!< No bars */
#define RSSI_LEVEL1  -102   /*!< 1 bar */
#define RSSI_LEVEL2   -93   /*!< 2 bars */
#define RSSI_LEVEL3   -87   /*!< 3 bars */
#define RSSI_LEVEL4   -78   /*!< 4 bars */
#define RSSI_LEVEL5     0   /*!< 5 bars */

/** API Opened Sentinel. This value is purposely more than just
 * a TRUE/FALSE value because there is no way to initialize
 * memory before this program is started. Likely all memory
 * will be initialized to zero on startup, but there's no 
 * guarantee. The following value is highly unlikely to 
 * occur in memory naturally if it is not zero-filled on 
 * startup (we hope). Chosen without any particular algorithm
 * in mind
 */
#define APINITIALIZED 0xBEEFCAFE
#define APNTBUFSIZE 4096          /*!< size of receive buffer for notification handler function */
#define NOTTIMEOUT  5000           /*!< notification handler timeout */
#define APTIMEOUT   5000            /*!< SWI API default timeout */

/* CNS_LBS_SETTINGS constants */
#define SWIREADMASK  0x0001
#define SWIWRITEMASK 0x0002

/* CNS_FLASH_IMAGE_INFO constants */
#define SWI_CDMA_IMAGEVERSION       0x00
#define SWI_CDMA_USDBVERSION        0x01       
#define SWI_UMTS_IMAGEVERSION       0x02
#define SWI_UMTS_USDBVERSION        0x03
#define SWI_SWOC_IMAGEVERSION       0x04
#define SWI_RELEASEDATE             0x05
#define SWI_PRODUCTTYPE             0x06
#define SWI_IMAGESIZE               0x07
#define SWI_GENERIC_IMAGEVERSION    0x08

/**
 *           Structure used to keep Air Server device type to product name
 *           mapping information within the API package. 
 *           This may in future be replaced by an XML data file for easier
 *           addition of new device types.
 */
struct apairservertable
{
    /*! The type of airserver device connected
     *  to the host */
    enum SWI_TYPE_Device apairdevtype;
    /*! The product name airserver device connected
     *  to the host */
    swi_uint8            *apairprodstrp;
};

/**
 *           Structure used to keep Air Server status information within 
 *           the API package. One of the members of API package global 
 *           control block structure
*/
struct apairserverstatus
{
    /*! TRUE, device is connected, FALSE, device
     *  is disconnected */
    swi_bool             apairserverpresent;
    /*! era number of the latest status change;
     *  this is a unique  identifier of the change 
     *  and can be used by the Application 
     *  to order air server status indications
     *  received on different channels */
    swi_uint32           apairchgera;
    /*! The type of airserver device connected
     *  to the host */
    enum SWI_TYPE_Device apairdevice;
    /*! The technology type of airserver device connected
     *  to the host, if available */
    swi_uint32           apairtechtype;
    /*! The product string name of airserver device connected
     *  to the host, if available */
    swi_uint8            apairprodstr[LENGTH_ProductStr];
};


/**
 *          API package global control block structure. Contains
 *          various elements needed by the API image for runtime
 *          operation
 */
struct apicb
{
    /*! APINITIALIZED - This is a special 32 bit 
     *  value indicating that the image has been
     *  initialized. Any other value means it has
     *  not been */
    swi_uint32               apiopened;    
    /*! Structure to keep the latest status of the Air
    *  Server conveyed by Air Server Change notification */
    struct apairserverstatus apairstate;
    /*! TRUE, the modem is executing its application, FALSE
    *  the modem is running in boot-and-hold mode */
    swi_bool                 apapprunning;
    /*! Array for storing an error message from
     *  a CnS packet. The error is present if
     *  the Error Flag is set in the CnS Header. 
     *  Applications should call SwiGetLastError
     *  to retrieve the message */
    swi_char                 aplasterror[CNFWERRORSZ];
    /*! Pointer to user callback function for notifications */
    SWI_CALLBACK_EX          apcallbackp;
    /*! User specified notification filter */
    swi_uint64               apnotifications;
    /* Wait notification function pointer */
    void *                   apwaitnotiffnp;
    /* RILSTART */
    /* Waiter structure to halt main thread while the 
     * notification thread is created
     */
    struct swi_osapiwaiter apnotifsynchronizer;
    /* RILSTOP */
};

#include "ap/apiproto.h"

#endif /* __APIDEFS_H__ */

/*
 * $Log: apidefs.h,v $
 */
