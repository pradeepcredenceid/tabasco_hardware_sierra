/*************
 *
 * $Id: hpidefs.h,v 1.27 2010/06/09 23:44:15 epasheva Exp $
 *
 * Filename: hpidefs.c
 *
 * Purpose:  This file contains internal structures and constants for the 
 *           HIP dispatcher.
 *
 * Copyright: � 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/
/* Includes */

#include "aa/aaglobal.h"
#include "am/amudefs.h"
#include "dd/ddudefs.h"
#include "dl/dludefs.h"
#include "hd/hdudefs.h"
#include "hp/hpudefs.h"
#include "ic/icudefs.h"
#include "ci/ciudefs.h"
#include "mm/mmudefs.h"
#include "pi/piudefs.h"
#include "ql/qludefs.h"
#include "sl/sludefs.h"
#include "os/swi_ossdk.h"

#include "hp/hpudefs.h"

/* Logging classes */
/*
 * Class A - General HP Logging messages, Tx or Rx, doesn't matter
 */
#define HPLOG_CLASSA    0x0001   
/* 
 * Class B - General HP logging messages, Tx or Rx, doesn't matter
 */
#define HPLOG_CLASSB    0x0002   

/* HIP Header Size */
#define HPHIPHDRSZ  4        /* Size of a HIP Header */

/* Transmit Buffer Pool Definitions */
#define HPTXPOOLSZ  20       /* # blocks in the pool */
#define HDTXBUFSIZE 2100     /* payload of each block in bytes */

/* Transmit Small Buffer Pool Definitions */
#define HPTXSMALLPLSZ  35      /* # blocks in the pool */
#define HDTXSMLBUFSIZE 300     /* payload of each block in bytes */

/* Event pool definitions for Transmit task */
#define HPTXEVPOOLSZ  40     /* # event blocks in the pool */

#define HPTXMSGDEPTH  80     /* Maximum # of inter-task SMT messages */


/* Event pool definitions for Packet Handler task */
#define HPPHEVPOOLSZ  20     /* # event blocks in the pool */
#define HPPHMSGDEPTH  80     /* Maximum # of inter-task SMT messages */

/* SMT timeout value for receiving events */
#define HPTXSMTRCVTOUT    0  /* Timeout duration in ms (0 to wait forever) */
#define HPPHSMTRCVTOUT    0  /* Timeout duration in ms (0 to wait forever) */

/* Receive buffer size */
#define HPRXPOOLSZ     1     /* # blocks in the pool */
#define HPRXMSGSZ   4096     /* size of input buffer is 4K */

#define HPLBTIMEOUT 2000     /* Loopback response timeout (ms) */

/* HP Receive task pause delay. Used when the modem disappears 
 * and a call to usrcvtty() returns FALSE
 */
#define HPRXSCANDELAY 1000  /* Delay in ms */

/* Device Information Request defines */
#define HPDIRTIMEOUT    2000     /* Device Information response timeout (ms) */
#define HPDIRPRODSTRSZ  16       /* Size of product string + NULL termination */
#define HPDIRGENFAIL    0xFF     /* DIR failure */

/*************
 *
 * Name:     hpdelivery
 *
 * Purpose:  This struct contains information necessary to, keep stats 
 *           and deliver a HIP message for a specific message id.
 *             
 * Parms:    deliverfnp - Function to call to deliver the incoming message
 *
 *               Parameters: msgdesignator - HIP message designator
 *                           msgparm      - HIP message specific parameter
 *                           payloadp     - ptr to newly received msg buff
 *                           paylength    - length of ret'd msg payload buff
 *                           errorflg     - YES- if message size != header size
 *                                          NO - Message length = header length
 *
 *           hpnotregister - # of msgs dropped due to unregistered msgids
 *           hptoolong     - # of msgs dropped due to payload length > max
 *           hpdropnobuf   - # of msgs dropped due to no buffers provided
 *
 * Notes:    The task that receives the msgs is responsible for freeing 
 *           the memory.
 *
 *************/
struct hpdelivery
{
    void (*hpdeliverbuffp)( swi_uint32 msgdesignator, 
                            swi_uint8 msgparm, 
                            swi_uint8 *memfreep, 
                            swi_uint8 *payloadp, 
                            swi_uint16 paylength, 
                            swi_bool errorflg);
    swi_uint32 hpnotregister;
    swi_uint32 hptoolong;
    swi_uint32 hpdropnobuf;
};

/*************
 *
 * Name: hprxcb
 *
 * Purpose: HP Receive Task control block. Contains items required by the
 *          HIP Receive task during normal operation
 *             
 * Members:  hprxipcbufp  - Pointer to a flat buffer used to receive 
 *                          IPCs from the application and send responses
 *                          back to the application
 *           hpRxPool     - MM pool structure. This pool contains
 *                          blocks used to receive messages from HIP endpoint
 *           hprxdluser   - Users requiring to log debug information must 
 *                          have an instance of this user block structure.
 *           hprxpktsdropped - number of rx packets dropped
 * Notes:    None
 *
 **************/
struct hprxcb {
    swi_uint8           *hpipcbufp;
    struct mmpool       hpRxPool; 
    struct dlusercb     hprxdluser;
    struct swi_oswaiter hprxwaiter;
    swi_uint32          hprxpktsdropped;
};


/*************
 *
 * Name:     hptxcb
 *
 * Purpose:  HP Transmit Task control block. Contains items required by the
 *           HIP Transmit task during normal operation
 *             
 * Members:  hptxipcbufp  - Pointer to a flat buffer used to receive 
 *                          IPCs from the application and send responses
 *                          back to the application
 *           hpTxPool     - MM pool structure. This pool contains
 *                          blocks used to transmit HIP messages to an endpoint
 *           hpTxSmlPool  - MM pool structure. This pool contains small
 *                          blocks used to transmit HIP messages to an endpoint
 *           hptxicmsgque - IC SMT Message queue
 *           hptxevtpool  - MM pool structure. Contains buffers used
 *                          to store information about data received through 
 *                          the global entry point. 
 *                          Groups several arguments that can be referenced
 *                          by a single handle
 *           hptxdluser   - Users requiring to log debug information must 
 *                          have an instance of this user block structure.
 *           hptxicregbk  - IC Package periodic tick registration block 
 *  Notes:   None
 *
 **************/
struct hptxcb {
    swi_uint8           *hptxipcbufp;
    struct mmpool       hpTxPool;
    struct mmpool       hpTxSmlPool; 
    struct icsmtcb      hptxicmsgque; 
    struct mmpool       hptxevtpool;
    struct dlusercb     hptxdluser;
    struct icttregqblk  hptxicregbk;
};

/*************
 *
 * Name:     hptxevtblock
 *
 * Purpose:  Structure used to pass information into the HP
 *           Transmit task. 
 *             
 * Members:  hptxeventtype  - Indicates the remaining content of
 *                            this block
 *           hptxmemfreep   - Pointer to the address to use when releasing
 *                            the memory containing the hptx message
 *           Message specific information:
 *           HIP message:
 *           msgdesignator  - message designator
 *           msgparm        - message specific parameter
 *           hippacketp     - pointer to a HIP packet header & filled-in 
 *                            payload
 *           paylength      - length of the message payload field
 *                       
 * Notes:    None
 *
 **************/
struct hptxevtblock
{
    swi_uint16 hptxeventtype;
    swi_uint8  *hptxmemfreep;
    swi_uint32 msgdesignator;
    swi_uint8 msgparm;
    swi_uint8 *hippacketp;
    swi_uint16 paylength;
};

/*************
 *
 * Name:     hppheventtype
 *
 * Purpose:  HP Packet Handler Task event types
 *             
 * Members:  HPPHLOOPREQ    - Loopback request
 *           HPPHLOOPRESP   - Loopback response
 *           HPPHLINKIND    - Link status indication
 *           HPPHFWDWNCB    - Fimware Download callback
 *           HPPHFWDWNREQ   - Firmware Download
 *           HPPHAIRREQ     - Air Server Request
 *           HPPHAIRCHG     - Air Server Change Notification
 *           HPMDMCHGREQ    - Modem state change registration request
 *           HPPHTICK         - Timer tick event
 *           HPPHMODEMSTATECB - Modem change state callback
 *           HPPHPORTREQ      - Usb Port Name request
 *           HPPHDIRREQ       - Device Information Request
 *           HPPHDIRRESP      - Device Information Response
 *           HPPHINVALID      - Invalid event type indication
 * 
 * Notes:    None
 *
 **************/
enum hppheventtype
{
    HPPHLOOPREQ,
    HPPHLOOPRESP,
    HPPHLINKIND,
    HPPHFWDWNCB,
    HPPHFWDWNREQ,
    HPPHAIRREQ,
    HPPHAIRCHG,
    HPMDMCHGREQ,
    HPPHTICK,
    HPPHMODEMSTATECB,
    HPPHPORTREQ,
    HPPHDIRREQ,
    HPPHDIRRESP,
    HPPHINVALID  /* should be always last */  
};

/*************
 *
 * Name:     hptxeventtype
 *
 * Purpose:  HP Tx Task event types
 *             
 * Members:  HPTXHIP      - send HIP message event
 *           HPTXTICK     - Timer tick event
 *           HPTXINVALID  - Invalid event type indication
 * 
 * Notes:    None
 *
 **************/
enum hptxeventtype
{
    HPTXHIP,
    HPTXTICK,
    HPTXINVALID  /* should be always last */  
};

/*************
 *
 * Name:     hpphamreq
 *
 * Purpose:  Structure used to pass  an AM request packet to the HPPH task.
 *             
 * Members:  hpmsgdatap - Pointer to received AM request packet
 *                       
 * Notes:    This structure is used for Firmware Download.
 *
 **************/
struct hpphamreq
{
    swi_uint8          *hpmsgdatap;
};

/*************
 *
 * Name:     hphipevt
 *
 * Purpose:  Structure used to pass in HIP packet information for 
 *           incoming messages from the modem for which the HPPH task
 *           has registered. Examples of packets this task registers
 *           for are HPLOOPRESP, HPLINKSTAIND, etc. 
 *             
 * Members:  hpphmsgdesignator - HIP message ID of the incoming packet
 *           hpphmsgparm       - Contents of the Parameter field from the 
 *                               HIP packet
 *           hpphdatap         - Pointer to the payload portion of the 
 *                               HIP packet
 *           hpphdatasize      - Length of the payload portion of the 
 *                               HIP packet
 *           hppherrorflg      - Error status of the received packet
 *                       
 * Notes:    None
 *
 **************/
struct hphipevt
{
    swi_uint32         hpphmsgdesignator;  
    swi_uint8          hpphmsgparm;    
    swi_uint8          *hpphdatap;
    swi_uint32         hpphdatasize;
    swi_bool           hppherrorflg;
};

/*************
 *
 * Name:     hpphairtlv
 *
 * Purpose:  Structure used to extract information using the TLV notation
 *             
 * Members:  hpphtlvtype    - Type of info
 *           hpphtlvlen     - length of info
 *           hpphpayld      - union of returned data:
 *                 hpphtlvvalue  - single byte value 
 *                 hpphtlvvalstr - string  array     
 * Notes:    None
 *
 **************/
struct hpphairtlv
{
    swi_uint8   hpphtlvtype;
    swi_uint8   hpphtlvlen;
    union
    {
        swi_uint8 hpphtlvvalue;
        swi_uint8 hpphtlvvalstr[HPDIRPRODSTRSZ];
    } hpphpayld;
};

/*************
 *
 * Name:     hpphdevinfoevt
 *
 * Purpose:  Structure used to convey Air Server Device Information
 *           into the HP PH task. One of the members of
 *           the hpphevtblock structure
 *             
 * Members:  hpdevcbfp         - Callback funtion for data returned from modem
 *              userp          - Pointer to user data to be passed to call back
 *           hpairuserp        - Pointer to user data that is passed in
 *           hpphdevinforeqver - Version of the request message
 *           hpphdevinforeqtyp - type of info requested
 *           hpphdevinfoipcch  - IPC channel used for the request
 * 
 * Notes:    None
 *
 **************/
struct hpphdevinfoevt
{
    void (*hpdevcbfp)(  enum hpphstatus resultcode,
                          void *userp);
    void                *hpdevuserp;
    swi_uint8           hpphdevinforeqver;
    swi_uint8           hpphdevinforeqtyp;
    swi_uint16          hpphdevinfoipcch;
};

/*************
 *
 * Name:     hpphairchgevt
 *
 * Purpose:  Structure used to convey Air Server Change notification
 *           information into the HP PH task. One of the members of
 *           the hpphevtblock structure
 *             
 * Members:  hpphairstatus  - TRUE, device is connected, FALSE, device
 *                            is disconnected
 *           hpphapprunning - if hpphairstatus is TRUE, then this field
 *                            indicates whether the modem is executing
 *                            it's boot loader image (FALSE) or in the 
 *                            application image (TRUE)
 *                       
 * Notes:    None
 *
 **************/
struct hpphairchgevt
{
    swi_bool           hpphairstatus;
    swi_bool           hpphapprunning;
};

/*************
 *
 * Name:     hpmdmregevt
 *
 * Purpose:  Event block carrying requests from other packages to register
 *           for modem state change notifications.
 *             
 * Members:  
 *           hpmdmreghndlp - Pointer to the caller-supplied modem registration
 *                           structure
 *           hpmdmnamep    - Arbitrary label callers can pass to help identify
 *                           their handle for debugging purposes
 *           hpmdmcbfp     - Caller-supplied callback function, see below for 
 *                           prototype
 *           hpmdmuserp    - Caller-supplied arbitrary value, passed to the 
 *                           callback function whenever it is called
 *                       
 * Notes:    The callback function prototype is documented in hpudefs.h
 *           in the structure definition for hpmdmreghndl
 */
struct hpmdmregevt
{
    struct hpmdmreghndl *hpmdmreghndlp;
    swi_uint8           *hpmdmnamep;
    void (*hpmdmcbfp)( enum hpmdmstate modemstate,
                       swi_bool apprunning,
                       enum SWI_TYPE_Device devtype,
                       void *userp );
    void *hpmdmuserp;
};

/*************
 *
 * Name:     hplreqbevt
 *
 * Purpose:  Structure used to pass in loopback requests to the task
 *           from other packages. One of a union of structures used
 *           for sending events to the HPPH task.
 *             
 * Members:  
 *           hpphdatap    - pointer to the payload of the loopback message
 *           hpphdatasize - size of the loopback message
 *           hplbrescbfp  - pointer to callback on loopback response
 *           hpphuser     - Caller-supplied arbitrary value
 *                       
 * Notes:    None
 *
 **************/
struct hplbreqevt
{
    swi_uint8          *hpphdatap;
    swi_uint32         hpphdatasize;
    void (*hplbrescbfp)(  enum hpphstatus resultcode,
                          swi_bool runningapp, 
                          void *userp);
    void               *hpphuser;
 };
 
 /*************
 *
 * Name:     hpdevchgevt
 *
 * Purpose:  Structure used to pass the changing state of the modem to 
 *           Firmware Download module running in the HPPH task context.
 *             
 * Members:  modemstate - online/offline state of the modem
 *           devtype    - Modem hardware type
 *           apprunning - Application mode flag
 *                       
 * Notes:    This structure is used for Firmware Download.
 *
 **************/
 struct hpdevchgevt
 {
    enum hpmdmstate modemstate; 
    enum SWI_TYPE_Device devtype;
    swi_bool          apprunning;
};
/*************
 *
 * Name:     hpphevtblock
 *
 * Purpose:  Structure used to pass information into the HP
 *           Packet Handler task. 
 *             
 * Members:  hpphevent    - LB task event type 
 *           hpphmemfreep - Pointer to location to use when releasing
 *                          caller-supplied memory, if applicable. NULL
 *                          if nothing to release
 *           hpphipcchannel - IPC channel whence this message came from.
 *           hpevtmember  - A union of additional structures that 
 *                          are used for receiving different types
 *                          of events for the HPPH task. See structure
 *                          definitions above for specific details of 
 *                          each union member
 *                       
 * Notes:    None
 *
 **************/
struct hpphevtblock
{
    enum hppheventtype hpphevent;
    swi_uint8          *hpphmemfreep; 
    swi_uint8           hpphipcchannel;
    union {
        struct hphipevt      hpmdmpkt;
        struct hplbreqevt    hplbreq;
        struct hpphairchgevt hpphairchg;
        struct hpphamreq     hppham;
        struct hpmdmregevt   hpmdmreg;
        struct hpdevchgevt   hpdevchg;
        struct hpphdevinfoevt hpdevinfo;
    } hpevtmember;
};

/*************
 *
 * Name:     hpphloopbk
 *
 * Purpose:  Control block for managing the HP PH Loopback facility
 *             
 * Members:  
 *           hpphlbtimer   - Loopback timer, for detecting timeouts on 
 *                           outstanding loopback requests to the modem
 *           hploopbusy    - Indicates whether the Loopback facility task 
 *                           is busy with an existing loopback request
 *           hpphdatap     - Pointer to the caller's data if a request is
 *                           active, NULL otherwise
 *           hpphdatasize  - Length(bytes) of the caller-specified loopback
 *                           buffer. Note, this is the HIP payload only
 *           hpphuser      - User-supplied data, returned via callback
 *           hplbrescbfp   - callback for request responses. See below for
 *                           prototype and explanation
 *
 *           void hplbrescbf( enum hpphstatus resultcode, 
 *                            swi_bool runningapp,
 *                            void *userp );
 *           Where:
 *           ======
 *           resultcode: one of those defined by the enumeration. See
 *                       hpudefs.h for more information
 *           runningapp: TRUE  - Modem is running in the application
 *                       FALSE - Modem is running in boot-and-hold
 *           uparm     : The value the caller passed in at the request
 *                       is returned in this argument
 * 
 * Notes:    Provides a convenient structure to group data elements related
 *           to the HP PH Loopback facility
 *
 **************/
struct hpphloopbk {
    swi_uint32    hpphlbtimer;
    swi_bool      hploopbusy;
    swi_uint8     *hpphdatap; 
    swi_uint16    hpphdatasize;
    void          *hpphuser;

    void (*hplbrescbfp)( enum hpphstatus resultcode, 
                         swi_bool runningapp,
                         void *userp);
};

/*************
 *
 * Name:     hpmdmstreg
 *
 * Purpose:  Control block containing elements related to the HPPH Task's
 *           handling of modem state change notifications. When the modem
 *           enumerates or de-enumerates, notifications will be sent to 
 *           callers who have previously registered to receive them. 
 *             
 * Members:  
 *           hpmdmreghead       - Head of queue of pointers to packages who 
 *                                have registered to be notified when modem
 *                                state changes occur
 *           hpmdmapprunning    - TRUE, modem is executing its Application
 *                                image, FALSE, modem is executing its boot
 *                                image (i.e. Boot and Hold operation )
 *
 * Notes:    None
 *
 **************/
struct hpmdmstreg {
    struct qllink hpmdmreghead;
    swi_bool      hpmdmapprunning;
};

/*************
 *
 * Name:     hpairserverstatus
 *
 * Purpose:  Structure used to keep Air Server Status information 
 *           within the HP PH task. One of the members of HP PH 
 *           task control block
 *             
 * Members:  hpairserverpresent - TRUE, device is connected, FALSE, device
 *                                is disconnected
 *           hpairchgera        - era number of the latest status change;
 *                                this is a unique  identifier of the change 
 *                                and can be used by the Application 
 *                                to order air server status indications
 *                                received over the 2 IPC channels.
 *          hpairtechtype       - Technology type of the modem obtained using 
 *                                HIP messaging when modem is detecrted.
 *          hpairprodstr        - Product string of modem obtained as per 
 *                                Technology type.
 *                       
 * Notes:    None
 *
 **************/
struct hpairserverstatus
{
    swi_bool           hpairserverpresent;
    swi_uint32         hpairchgera;
    swi_uint8          hpairtechtype;
    swi_uint8          hpairprodstr[HPDIRPRODSTRSZ];
};

/*************
 *
 * Name:     hpdevinfo
 *
 * Purpose:  Structure used to keep Air Server Device information 
 *           within the HP PH task. One of the members of HP PH 
 *           task control block
 *             
 * Members:  hpdevbusy     - flag to check when requesting HIP info
 *           hpdevtechtype - Technology type of the device
 *           hpdevprodstr  - Product string of device
 *           hpdevcbfp     - Callback function pointer
 *           hppdevhuserp  - Callback function user data pointer
 *           hpdevamchan   - Channel array, return IPC channels
 * 
 * Notes:    None
 *
 **************/
struct hpdevinfo
{
    swi_uint32         hpphdevtimer;
    swi_bool           hpdevbusy;
    swi_uint8          hpdevtechtype;
    swi_uint8          hpdevprodstr[HPDIRPRODSTRSZ];
    void (*hpdevcbfp)( enum hpphstatus resultcode, 
                       void *userp);
    void               *hppdevhuserp;
    swi_uint16         hpdevamchan[CIMAXCNRRPAIR];
};

/*************
 *
 * Name:     hpphcb
 *
 * Purpose:  HP Packet Handler Task control block. Contains items required 
 *           by the HIP Packet Handler task during normal operation
 *             
 * Members:  
 *           hpphdluser    - Users requiring to log debug information must have
 *                           an instance of this user block structure.
 *           hpPhPool      - MM pool structure. This pool contains
 *                           blocks used to transmit HIP messages to an 
 *                           endpoint
 *           hpphicmsgque  - IC SMT Message queue
 *           hpicregbk     - IC Package periodic tick registration block 
 *           hpphevtpool   - MM pool structure. Contains buffers used
 *                           to store information about data received through 
 *                           the global entry point. 
 *                           Groups several arguments that can be referenced
 *                           by a single handle
 *           hpphloopcb    - Loopback handler control block substructure
 *           hpphmdmregcb  - Control block containing elements related to modem
 *                           state change handling
 *           hpphairstate  - Structure to keep the latest status of the Air
 *                           Server conveyed by Air Server Change notification
 *                           event.
 *           hpphdevinfo   - Device information Request/Response structure.
 *           hpphipcchanrr - IPC channel for download rr use
 *           hpphipcchannot- IPC channel for download notification use
 * 
 * Notes:    None
 *
 **************/
struct hpphcb {
    struct dlusercb    hpphdluser;
    struct mmpool      hpPhPool; 
    struct icsmtcb     hpphicmsgque; 
    struct icttregqblk hpicregbk;
    struct mmpool      hpphevtpool;

    /* Loopback Specific Elements */
    struct hpphloopbk hpphloopcb;

    /* Modem state change notification elements */
    struct hpmdmstreg           hpphmdmregcb;
    
    /* Air Server status information */
    struct hpairserverstatus    hpphairstate;

    /* Air Server Device information */
    struct hpdevinfo            hpphdevinfo;

    swi_uint8                   hpphipcchanrr; 
    swi_uint8                   hpphipcchannot; 

};

#include "hp/hpudefs.h"
#include "hp/hpiproto.h"

/*
 * $Log: hpidefs.h,v $
 */
