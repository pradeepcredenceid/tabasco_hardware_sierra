/*************
 *
 * $Id: uiidefs.h,v 1.3 2009/06/01 23:30:40 blee Exp $
 *
 * Filename:  uiidefs.c
 *
 * Purpose:   This file contains internal structures and constants for 
 *            the User Interface package
 *
 * Copyright:  ©  2009 Sierra Wireless Inc., all rights reserved
 *
 **************/
/* Includes */
#include <stdio.h>

#include "aa/aaglobal.h"
#include "am/amudefs.h"
#include "cn/cnudefs.h"
#include "dd/ddudefs.h"
#include "dl/dludefs.h"
#include "us/usudefs.h"
#include "ds/dsudefs.h"
#include "hp/hpudefs.h"
#include "ic/icudefs.h"
#include "mm/mmudefs.h"
#include "pi/piudefs.h"
#include "sdk/sdkudefs.h"
#include "sl/sludefs.h"
#include "su/suudefs.h"
#include "ci/ciudefs.h"
#include "ui/uiudefs.h"

/* Definitions */

/* DL package logging classes */
#define UILOG_CLASSA  0x0001

/* Timeout to use when waiting for incoming IPC traffic from API side */
/* MM buffer pool definitions */
#define UIIPCBUFSZ     1000 /* Size of an IPC Buffer - TBD tune later */
#define UIIPCPOOLSZ    10   /* # of buffers in pool  - TBD tune later */

#define UIIPCTIMEOUT  0  /* IPC timeout in ms */

/* Timeout to use when waiting for incoming shared memory messages
 *  within SDK
 */
#define UISMTRCVTOUT 0 /* Timeout duration in ms (0 to wait forever) */

/* MM buffer pool definitions */
/* Note: The size of a pool  block must match the largest message 
 * size that can be received.
 * Therefore the size of a pool block is the sizeof the largest 
 * event block from the union of possible event blocks.
 */
#define UIBUFSZ     512 /* Size of the shared 
                          * memory buffer - tuning TBD
                          */

#define UIPOOLSZ    15   /* # of buffers in pool  - TBD tune later */
#define UIEVPOOLSZ  20   /* # event blocks in the pool */
#define UIMSGDEPTH  80   /* Maximum # of inter-task SMT messages */

/*************
 *
 * Name:    uitaskcb
 *
 * Purpose: User Interface Task control block structure 
 *             
 * Members: uisendenabled - Flag indicating whether ui send interface is 
 *                          enabled by Application
 *          uidlcb        - DL package control block for logging 
 *          uibufpool     - MM pool structure. Blocks in this pool are used
 *                          to store incoming data packets from the API side
 *          uiicmsgque    - IC SMT Message queue
 *          uievtpool     - MM pool structure. Contains buffers used
 *                          to store information about data received 
 *                          through the global entry point. 
 *          uiipcshell    - storage for DS shell structure for DS shell creation
 * 
 * Notes:   The ui task runs on the SDK side only. 
 *
 *          
 *
 **************/
struct uitaskcb
{
    swi_bool            uisendenabled;  
    struct dlusercb     uidlcb;  
    struct mmpool       uibufpool;
    struct icsmtcb      uiicmsgque; 
    struct mmpool       uievtpool;
    struct dsipcshell   uiipcshell;
};
/*************
 *
 * Name:     uieventtype
 *
 * Purpose:  UI Task event types
 *             
 * Members:  UIAPPREQUEST       - Request from the Application
 *           UIINVALID          - Invalid event type indication
 * 
 * Notes:    None
 *
 **************/
enum uieventtype
{
    UIAPPREQUEST,
    UIINVALID  /* should be always last */  
};

/*************
 *
 * Name:    uievtblock
 *
 * Purpose: Structure used to pass information into the UI task. 
 *             
 * Members: uievent    - UI task event type 
 *          uimemfreep - Pointer to location to use when releasing caller-
 *                       supplied memory, if applicable. NULL if nothing to 
 *                       release
 *          uidatalen  - Length of data being passed around
 *          uireqstr   - Array for storing data (Size may be adjusted in future)
 *                       
 * Notes:    The union of events will allow for future extensions.
 *
 **************/
struct uievtblock
{
    enum uieventtype uievent;
    swi_uint8        *uimemfreep; 
    swi_uint16      uidatalen;
    swi_uint8       uireqstr[UIBUFSZ];
};

/*************
*
* Name:     uiCmdTable - Command Table
*
* Purpose:  This structure contains .
*                   
* Members:  cmdStrp       - pointer to command string
*           cmdFp         - pointer to command processing function
*           SubTblp       - pointer to sub-command table
*           cmdHelp       - pointer to command help string
*
* Notes:       none
*
**************/
struct uiCmdTable
{
      const char                *cmdStrp;
      swi_bool                  (*cmdFp)(char *);
      struct uiCmdTable const   *SubTblp;
      const char                *cmdHelp;
};

#include "ui/uiiproto.h"

/*
 * $Log: uiidefs.h,v $
 */
