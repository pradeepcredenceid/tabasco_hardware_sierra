/**
 * \if CvsId
 * $Id: apdxidefs.h,v 1.7 2009/11/03 01:06:37 epasheva Exp $
 * \endif
 *
 * \file     apdxidefs.h
 *
 * \brief    This file contains internal structures and constants for the 
 *           API Demux side image
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 */

/* Includes */
#include "SwiApiDemux.h"
#include "SwiDataTypes.h"    /*! system wide typedefs */
#include "amudefs.h"
#include "cnudefs.h"
#include "usudefs.h"
#include "dsudefs.h"
#include "icudefs.h"
#include "ciudefs.h"
#include "swi_osapi.h"   /*! package user include file  */
#include "sludefs.h"

#include "apdxudefs.h"  

/* Defines */

/** Demux API Opened Sentinel. This value is purposely more than just
 * a TRUE/FALSE value because there is no way to initialize
 * memory before this program is started. Likely all memory
 * will be initialized to zero on startup, but there's no 
 * guarantee. The following value is highly unlikely to 
 * occur in memory naturally if it is not zero-filled on 
 * startup (we hope). Chosen without any particular algorithm
 * in mind
 */
#define APDXINITIALIZED     0xCAFEDECA

/** Unique code combination to mark the header of a data indication packet 
 * during construction. It is used to enforce the caller to perform 
 * certain checks before sending the data packet.
 */
#define APDXDATAHDRMARK     0xDACEFACE

#define APDXDEFAULTTIMEOUT  3000

/** Size of receive data buffer as defined in IDD_DemuxMessaging document */
#define APDXBUFSIZE         AMDEMUXCTLINDTOTAL + LENGTH_DxData  
     
/** Define the maximum number of channels the Application can support. 
 * This constant is defined here so that the Application can size 
 * its resources same way as SDK.
 */
#define APDXMAXCHANNELS    CIMAXIPCCHAN

#define APDXTESTPAUSE      60  /*! time period to observe execution of test
                                *  program 
                                */
#define APDXDIAGNSPLDLEN    4  /*! length of diagnostic device service 
                                * test payload 
                                */  
#define APDXDATAINDOFF      AMDEMUXDATAINDTOTAL  /*! offset to data section
                                                  *  within demux data  
                                                  * indication packet 
                                                  */                             
                                                        
                                
/**
 *      APDX IPC Task control block structure 
 *             
 * \note   Runs on the API side only. 
 *          This is a 'generic' APDX IPC Task control block structure
 *          Several APDX IPC Tasks may exist concurrently; each such task
 *          serves one IPC channel.
 *          User callbacks are installed during Registration.
 *          
 */
struct apdxipccb
{
    /*! Device service */
    swi_uint16       apdxdevsrv;
    /*! Pointer to a user-supplied name useful for troubleshooting */
    swi_uint8       *apdxnamep; 
    /*! stores channel number if needed for debugging */
    swi_uint16       apdxchannel;
    /*! pointer to IPC Receive handler which is installed
     *  when the channel is opened with the following 
     *  prototype:
     *      
     *      inipcmsgp - pointer to the beginning of the received message
     *      
     *      inmsglen  - length of the received message */
    void (*apdxrcvcbckp)(swi_uint8 *inipcmsgp, swi_uint32 inmsglen);
    /*! Pointer to user callback function for Demux Data
     *  with the following prototype:
     *      
     *      deviceservice - device service - diagnostic or NMEA
     * 
     *      datap         - pointer to received data
     * 
     *      datalen       - length of data packet
     * 
     *      userp         - user parameter supplied during registartion
     *                      i.e. apdxdatauserp member of control block */
    void (*apdxdatacbfp)(swi_uint16 deviceservice, swi_uint8 *datap, 
        swi_uint32 datalen, void *userp);
    /*! User parameter for data callback */
    void *apdxdatauserp;
    /*! Pointer to user callback for control indication
     *  with the following prototype:
     * 
     *      deviceservice - device service - diagnostic or NMEA
     * 
     *      ctldatap      - pointer to control data, e.g. device 
     *                      (modem) presence status
     * 
     *      userp         - user parameter supplied during registartion
     *                      i.e. apdxctluserp member of control block */
    void (*apdxctlcbfp)(swi_uint16 deviceservice, swi_uint8 *ctldatap, 
        void *userp);
    /*! user parameter supplied during registartion
    *       i.e. apdxctluserp member of control block */
    void *apdxctluserp;
};

/**
 *           Structure used to keep Air Server status information within 
 *           the APDX package. One of the members of APDX package global 
 *           control block structure
*/
struct apdxairserverstatus
{
    /*! SWI_ONLINE, device is connected, SWI_OFFLINE, device
     *  is disconnected */
    swi_uint8             apdxairserverpresent;
    /*! era number of the latest status change;
     *  this is a unique  identifier of the change 
     *  and can be used by the Demux Application 
     *  in maintaining proper order of events */
    swi_uint32           apdxairchgera;
};

/**
 *          API Demux package global control block structure. Contains
 *          various elements needed by the API image for runtime
 *          operation
*/
struct apdxcb
{
    /*! APDXINITIALIZED - This is a special 32 bit 
    *  value indicating that the image has been
    *  initialized. Any other value means it has
    *  not been */
    swi_uint32        apdxopened;  
    /*! Structure to keep the latest status of the Air
     *  Server conveyed by Control Indication */
    struct apdxairserverstatus apdxairstate;
    /*! Maximum number of channels as obtained from SDK */
    swi_uint16        apdxmaxnumchan;
    /*! Array of IPC control blocks. */
    struct apdxipccb  apdxchancb[APDXMAXCHANNELS];
    /*! pointer to generic receive handler which is to be
    *  installed within each channel control block when
    *  the channel is opened. */
    void (*apdxrcvhandlerp)(swi_uint8 *inipcmsgp, swi_uint32 inmsglen);
};

#include "apdxiproto.h"

/*
 * $Log: apdxidefs.h,v $
 */
