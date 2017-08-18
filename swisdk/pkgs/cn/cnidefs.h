/*************
 *
 * $Id: cnidefs.h,v 1.10 2010/06/10 00:03:34 epasheva Exp $
 *
 * Filename: cnidefs.c
 *
 * Purpose:  This file contains internal structures and constants for the 
 *           Control and Status package
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/
/* Includes */
#include "aa/aaglobal.h"
#include "am/amudefs.h"
#include "dl/dludefs.h"
#include "er/erudefs.h"
#include "hp/hpudefs.h"
#include "mm/mmudefs.h"
#include "pi/piudefs.h"
#include "ic/icudefs.h"
#include "sdk/sdkudefs.h"
#include "sl/sludefs.h"
#include "su/suudefs.h"
#include "os/swi_ossdk.h"
#include "cn/cnudefs.h"

/* CN Constant Definitions */

/* Timeout guard interval. This value is the basic
 * timeout applied by the api side whenever it calls
 * icapi_rcv(). It is added to the application's 
 * specified timeout on API calls, while the caller's 
 * timeout is used directly by the SDK when it is 
 * awaiting a response from the modem for a CnS Get/
 * Set/etc. operation. This ensure that the modem
 * timeout will always happen first. If the icapi_rcv()
 * times out, then this is some kind of system failure,
 * like the SDK process has died.
 */
#define CNTIMEGUARD 1000  

/* Timeout value to use in CN package when calling icsmt_rcv() */
#define CNNOTIMEOUT    0

/* CnS Request/Response task Event IDs */
#define CNSRESPONSE            1  /* Event ID for CnS Response packet */

/* Request/Response (RR) Task */
#define CNRRMRESPTIMEOUT    5000  /* Modem response timeout in ms - TBD tune */
#define CNRREVTPOOLSZ         30  /* # of buffers in event pool */
#define CNRRMSGDEPTH        (CNRREVTPOOLSZ + 1) /* CN RR Task msg que depth */

/* Bit Masks within the CnS header */
#define CNSERRORFLAGMASK    0x80
#define CNSOPERATIONMASK    0x7F

/* Offsets to fields within the CnS header */
#define CNSOFFSET_OPTYPE    0x02
#define CNSOFFSET_APPARM    0x04

/* Size of the CnS packet header in bytes */
#define CNSHEADERSZ         10

/* Logging masks */
#define CNLOG_CLASSA        0x0001
#define CNLOG_CLASSB        0x0002
#define CNLOG_CLASSC        0x0004

/* Maximum length of CnS header */
#define CNMAXCNSHDRSZ       AMCNSREQSZ + HPHDRLENGTHEXT

/*************
 *
 * Name:    cneventtypes
 *
 * Purpose: Enumerated list of event block types. These are the types
 *          that get written into the CnS Event Block whenever an 
 *          inter-task message is to be sent to the CNS RR Task. 
 *             
 * Members: CNRR_MIN     - For error checking (optional)
 *          CNRR_IPCMSG  - Event block contains a message from the IPC task
 *          CNRR_HPMSG   - Event block contains a modem CnS response via 
 *                         HIP Receive task
 *          CNRR_TMRTICK - Timer Tick event, received periodically.
 *          CNRR_MAX     - Maximum event block type for error checking
 *                         (optional)
 *                       
 * Notes:   None
 *
 **************/
enum cneventtype {
    CNRR_MIN = 0,
    CNRR_IPCMSG = CNRR_MIN, 
    CNRR_HPMSG, 
    CNRR_TMRTICK,
    CNRR_MAX
};

/*************
 *
 * Name:    cnsevtblock
 *
 * Purpose: Structure used to pass information into the CnS 
 *          Request/Response task. The event block is general enough
 *          that it can be used to send IPC and CnS messages into the
 *          task with the event type being used to distinguish between
 *          them
 *             
 * Members: cneventtype  - Indicates the remaining content of
 *                         this block
 *          cnevtdatap   - Pointer to a buffer containing data to be
 *                         handled by the CNS RR Task. 
 *          cnevtmemfreep- Pointer to the address to use when releasing
 *                         the memory containing the data. NULL if nothing
 *                         needs to be released
 *                       
 * Notes:   None
 *
 **************/
struct cnsevtblock
{
    enum cneventtype cneventtype;
    swi_uint8  *cnevtdatap;
    swi_uint8  *cnevtmemfreep;
};

/*************
 *
 * Name:    cnswdata
 *
 * Purpose: Structure to hold key information about the outstanding
 *          packet being sent to the modem. When the response is 
 *          received it must correlate directly with this information
 *             
 * Members: cnsbusy       - Whether the CnS RR task is already waiting
 *                          for a CnS response from the modem
 *          cntimeoutflag - Indicates whether the thread timed out 
 *                          waiting for a response from the modem
 *          cnsobjid      - The ID of the object we expect to see in the 
 *                          response packet's CnS header
 *          cnsoperation  - The value we expect to see in the operation
 *                          type field of the response packet
 *          cnsipcchannel - The IPC channel that the CNS request came in.
 *                       
 * Notes:   The cnstoflag is currently unused and may be deleted once the
 *          design is clearer
 *
 **************/
struct cnswdata
{
    swi_bool   cnsbusy;
    swi_bool   cntimeoutflag;
    swi_uint16 cnsobjid;
    swi_uint8  cnsoperation;
    swi_uint8  cnsipcchannel;
};

/*************
 *
 * Name:    cnrrtcb
 *
 * Purpose: CN RR Task control block structure 
 *             
 * Members: cndlcb     - DL package control block for logging 
 *          cnicmsgque - IC SMT Message queue
 *          cnbufpool  - MM pool structure. This pool contains
 *                       blocks used to receive IPC traffic from
 *                       the application
 *          cnevtpool  - MM pool structure. Contains buffers used
 *                       to store information about CnS Reponses. 
 *                       Groups several arguments that can be referenced
 *                       by a single handle
 *          cnswdata   - Structure to hold the information about
 *                       the current transaction awaiting a response
 *                       from the modem
 *          cnictthndl - IC package timer handle used by this task to
 *                       use the periodic timer tick facility of the IC
 *                       package
 *          cnmdmwaittmr Downcounting timer used to detect timeouts when
 *                       waiting for modem responses. Tick interval is 
 *                       given by:
 *                                  <timeout(ms)>/ICTICKINTERVAL
 * Notes:   none
 *
 **************/
struct cnrrtcb
{
    struct dlusercb    cndlcb;  
    struct icsmtcb     cnicmsgque;
    struct mmpool      cnevtpool;
    struct cnswdata    cnswdata;
    struct icttregqblk cnictthndl;
    swi_uint32         cnmdmwaittmr;
};


/*************
 *
 * Name:    cnapicb
 *
 * Purpose: Control block used by the API-side CnS package
 *             
 * Members: cnapiipcrr   - IPC control block used for communications
 *                       between the API and CnS Request/Response 
 *                       task
 *          cnapiipcnot   - IPC control block used for communications
 *                       between the API and CnS Notification
 *                       task
 *
 * Note:    This structure is for the API side and is not required
 *          by the SDK side at all
 *
 **************/
struct cnapicb {
    struct icipccb cnapiipcrr;
    struct icipccb cnapiipcnot;
};

#include "cn/cniproto.h"

/*
 * $Log: cnidefs.h,v $
 */
