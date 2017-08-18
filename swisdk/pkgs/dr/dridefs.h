/*************
 *
 * $Id: dridefs.h,v 1.11 2010/06/09 23:38:00 epasheva Exp $
 *
 * Filename: dridefs.c
 *
 * Purpose:  This file contains internal structures and constants for 
 *           the Data Relay package
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/
/* Includes */
#include "aa/aaglobal.h"
#include "am/amudefs.h"
#include "ap/SwiApiCmBasic.h"
#include "cn/cnudefs.h"
#include "dd/ddudefs.h"
#include "dl/dludefs.h"
#include "er/erudefs.h"
#include "us/usudefs.h"
#include "ds/dsudefs.h"
#include "hp/hpudefs.h"
#include "ic/icudefs.h"
#include "ci/ciudefs.h"
#include "mm/mmudefs.h"
#include "pi/piudefs.h"
#include "sl/sludefs.h"
#include "sdk/sdkudefs.h"
#include "su/suudefs.h"
#include "dr/drudefs.h"

/* Definitions */

/* DL package logging classes */
#define DRLOG_CLASSA  0x0001
#define DRLOG_CLASSB  0x0002
#define DRLOG_CLASSC  0x0004

/* Timeout to use when waiting for incoming IPC traffic from API side */
#define DRIPCTIMEOUT  0  /* IPC timeout in ms */

/* Timeout to use when waiting for incoming shared memory messages
 *  within SDK
 */
#define DRSMTRCVTOUT 0 /* Timeout duration in ms (0 to wait forever) */

/* MM buffer pool definitions */
/* Note: The size of a pool  block must match the largest message 
 * size that can be received.
 * Therefore the size of a pool block is the sizeof the largest 
 * event block from the union of possible event blocks.
 */
#define DRBUFSZ     4500 /* Size of the shared 
                          * memory buffer 
                          */

#define DRPOOLSZ    15   /* # of buffers in pool  - TBD tune later */
#define DREVPOOLSZ  60   /* # event blocks in the pool */
#define DRMSGDEPTH  80   /* Maximum # of inter-task SMT messages */


/*************
 *
 * Name:    drtaskcb
 *
 * Purpose: Data Relay Task control block structure 
 *             
 * Members: drsendenabled  - Flag indicating whether DR send interface is 
 *                           enabled by Application
 *          drdlcb         - DL package control block for logging 
 *          drbufpool      - MM pool structure. Blocks in this pool are used
 *                           to store incoming data packets from the API side
 *          dricmsgque     - IC SMT Message queue
 *          drevtpool      - MM pool structure. Contains buffers used
 *                           to store information about data received 
 *                           through the global entry point. 
 *          drdevchgreqblk - registration block for modem state change
 *          dreranumber    - to uniquely identify Control Indictions from SDK
 *                           to Application
 *          dripcsh        - Storage for IPC shell structure
 * 
 * Notes:   The DR task runs on the SDK side only. 
 *
 *          
 *
 **************/
struct drtaskcb
{
    swi_bool            drsendenabled;  
    struct dlusercb     drdlcb;  
    struct mmpool       drbufpool;
    struct icsmtcb      dricmsgque; 
    struct mmpool       drevtpool;
    struct hpmdmreghndl drdevchgreqblk;
    swi_uint32          dreranumber;
    struct dsipcshell   dripcsh[CIMAXDRSVC];
};

/*************
 *
 * Name:     dreventtype
 *
 * Purpose:  DR Data Relay Task event types
 *             
 * Members:  DRMODEMSTATECB     - Modem status change
 *           DRAPPREQUEST       - Request from the Application
 *           DRDIAGMSG          - Diagnostic Message from Modem
 *           DRINVALID          - Invalid event type indication
 * 
 * Notes:    None
 *
 **************/
enum dreventtype
{
    DRMODEMSTATECB,
    DRAPPREQUEST,
    DRDIAGMSG,
    DRINVALID  /* should be always last */  
};

/*************
 *
 * Name:     drmdmstate
 *
 * Purpose:  Enumerated list of modem states returned to the caller 
 *           whenever a modem appears or disappears
 *             
 * Members:  DRMDMOFFLINE - No modem is currently enumerated
 *           DRMDMONLINE  - Modem is enumerated
 * 
 * Notes:    None
 *
 **************/
enum drmdmstate
{
    DRMDMOFFLINE,
    DRMDMONLINE
};


/*************
 *
 * Name:     drdevchgevt
 *
 * Purpose:  Structure used to convey modem status change
 *             
 * Members:  drmdmstate - online/offline state of the modem
 *           drdevtype  - Modem hardware type
 *                       
 * Notes:    None
 *
 **************/
struct drdevchgevt
{
    enum hpmdmstate      drmdmstate; 
    enum SWI_TYPE_Device drdevtype;
};

/*************
 *
 * Name:     drappreqevt
 *
 * Purpose:  Structure used to convey application requests
 *             
 * Members:  drdevservice  - DIAG or NMEA
 *           droperation   - request operation code
 *                       
 * Notes:    None
 *
 **************/
struct drappreqevt
{
    swi_uint16 drdevservice;
    swi_uint16 droperation;    
};

/*************
 *
 * Name:     drdiagevt
 *
 * Purpose:  Structure used to convey diagnostic information
 *             
 * Members:  drresplength  - Response length
 *           drinbufp      - Pointer to Buffer location
 *                       
 * Notes:    None
 *
 **************/
struct drdiagevt
{
    swi_uint16 drresplength;
    swi_uint8 *drinbufp;    
};

/*************
 *
 * Name:     drevtblock
 *
 * Purpose:  Structure used to pass information into the DR
 *           Data Relay task. 
 *             
 * Members:  drevent    -  DR task event type 
 *           drmemfreep -  Pointer to location to use when releasing caller-
 *                         supplied memory, if applicable. NULL if nothing to 
 *                         release
 *           drevtmember - A union of additional structures that are used for 
 *                         receiving different types of events for the DR task. 
 *                         See structure definitions above for specific details 
 *                         of each union member
 *                       
 * Notes:    The union of events will allow for future extensions.
 *
 **************/
struct drevtblock
{
    enum dreventtype drevent;
    swi_uint8        *drmemfreep; 
    union 
    {
        struct drdevchgevt   drdevchg;
        struct drappreqevt   drapprequest;
        struct drdiagevt     drdiagmsg;
    } drevtmember;
};

#include "dr/driproto.h"

/*
 * $Log: dridefs.h,v $
 */
