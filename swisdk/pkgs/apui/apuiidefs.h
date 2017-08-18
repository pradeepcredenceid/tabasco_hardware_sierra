/*************
 *
 * $Id: apuiidefs.h,v 1.3 2009/06/01 23:21:57 blee Exp $
 *
 * Filename: apuiidefs.c
 *
 * Purpose:  This file contains internal structures and constants for the 
 *           API UI side image
 *
 * Copyright: © 2009 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* Includes */
#include "SwiDataTypes.h"    /* system wide typedefs */
#include "amudefs.h"
#include "cnudefs.h"
#include "icudefs.h"
#include "ciudefs.h"
#include "usudefs.h"
#include "dsudefs.h"
#include "dludefs.h"
#include "swi_osapi.h"   /* package user include file  */
#include "sludefs.h"
#include "ui/uiudefs.h"
#include "SwiApiUi.h"

#include "apuiudefs.h"  

/* Defines */

/* UI API Opened Sentinel. This value is purposely more than just
 * a TRUE/FALSE value because there is no way to initialize
 * memory before this program is started. Likely all memory
 * will be initialized to zero on startup, but there's no 
 * guarantee. The following value is highly unlikely to 
 * occur in memory naturally if it is not zero-filled on 
 * startup (we hope). Chosen without any particular algorithm
 * in mind
 */
#define APUIINITIALIZED     0xDEADCAFE


/* Size of receive data buffer as defined in IDD_DemuxMessaging document */
#define APUIBUFSIZE                 2048  
#define APUIDEFAULTTIMEOUT          3000  
     
/*************
 *
 * Name:    apuicb
 *
 * Purpose: API UI package global control block structure. Contains
 *          various elements needed by the API image for runtime
 *          operation
 *             
 * Members: apuiopened        APUIINITIALIZED - This is a special 32 bit 
 *                            value indicating that the image has been
 *                            initialized. Any other value means it has
 *                            not been
 *          apuiipccb         Array of IPC control blocks.
 *          apuirequestblock  Buffer for sending messages to SDK
 *          apuiresponseblock Buffer for receiving messages from SDK
 * 
 * Notes:   none
 *
 **************/
struct apuicb
{
    swi_uint32        apuiopened;  
    struct icipccb    apuiipccb;
    /* The following arrays are used for sending and receiving responses
     * between the API and the SDK side. We provide these here rather than 
     * clobber up AM buffers which are used on different channels.
     */
    swi_uint8 apuirequestblock[APUIBUFSIZE];
    swi_uint8 apuiresponseblock[APUIBUFSIZE];


};

#include "apuiiproto.h"

/*
 * $Log: apuiidefs.h,v $
 */
