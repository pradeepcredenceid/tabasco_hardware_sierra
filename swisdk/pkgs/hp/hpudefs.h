/*
 * $Id: hpudefs.h,v 1.14 2010/06/09 23:47:21 epasheva Exp $
 *
 * Filename: hpudefs.h
 *
 * Purpose:   User definitions for the Host to Modem protocol (HIP)
 *            Host message IDs are defined here.
 *
 * Copyright: C 2008 Sierra Wireless Inc., all rights reserved
 *
 */

#ifndef hpudefs_h
#define hpudefs_h

#include "am/amudefs.h"
#include "ap/SwiApiCmBasic.h"
#include "ql/qludefs.h"

#define    HPNULLPARMFIELD      0  /* value to use for undefined parm fields */

/* max number of Forward HIP packets that can be handled at a single time */
#define HPMAXBUFFWD  (HPSMBUFNUM + HPMEDBUFNUM + HPLRGBUFNUM)

/* HIP header constants */
/* size of the message header (in words) */
#define HPHDRSIZE       2              

/* size of extended message header (in words) */
#define HPEXTHDRSIZE	HPHDRSIZE + 1

/* Validation token for modem change registration */
#define HPMDMVALIDATER 0xBEEFCAFE

/* Local constants and enumerated types */
#define HPMINPAYLEN 0        /* minimum message payload length (in bytes) */
#define HPMAXPAYLEN 2000     /* maximum message payload length (in bytes) */
#define HPMAXEXTPAYLEN 0xFFF
#define HPMAXMSGLEN	HPMAXEXTPAYLEN + HPMAXEXTPAYLEN*2 

/* Due to byte stuffing, tx buffer must be twice the size of the rx buffer */
/* plus one more for the pad byte */
#define HPTXMAXPAYLEN HPMAXMSGLEN*2 + 1

/* Extended HIP header constants */
#define HPEXTMSGIDSHF 16			
#define HPMEXTENDED	 (swi_uint32)0x3F  /* extended header escape code */
#define HPHEXTENDED	 (swi_uint32)0x7F  /* extended header escape code */

/* message ids in host-to-modem direction */
#define HPDLSTRTREQ  (swi_uint32)0x00    /* Download start request */
#define HPDLENDREQ   (swi_uint32)0x01    /* Download end request */
#define HPDLCONTREQ  (swi_uint32)0x02    /* Download continuation request */

#define HPSWIREQ     (swi_uint32)0x02    /* SWI request */
#define HPLOOPREQ    (swi_uint32)0x03    /* Loop Back request */
#define HPPROGREQ    (swi_uint32)0x04    /* flash program request */
#define HPPSUMREQ    (swi_uint32)0x05    /* Profile summary request */

#define HPCONFREQ    (swi_uint32)0x08    /* Profile Configuration request */
#define HPPSACTREQ   (swi_uint32)0x09    /* Packet session activation request */
#define HPPOWMANREQ  (swi_uint32)0x0A    /* Power Management Request */
#define HPRESETREQ   (swi_uint32)0x0B    /* Reset Modem Request */
#define HPWWANREGREQ (swi_uint32)0x0C    /* WWAN RSSI Indication */
#define HPRNAPCFGREQ (swi_uint32)0x10    /* RNAP Configuration Request */

#define HPMSYNCIND   (swi_uint32)0x20    /* modem data link sync indication */
#define HPMLAUNIND   (swi_uint32)0x21    /* launch fragment indication */
#define HPRDATAIND   (swi_uint32)0x23    /* reverse data indication */
#define HPMSWIIND    (swi_uint32)0x24    /* SWI modem indication */
#define HPML1IND     (swi_uint32)0x25    /* modem layer 1 indication */
#define HPSHUTIND    (swi_uint32)0x26    /* system shutdown indication */
#define HPSUSCIND    (swi_uint32)0x27    /* host suspend complete indication */
#define HPSTRTIND    (swi_uint32)0x28    /* start application indication */


#define HPMCNSIND    (swi_uint32)0x2B    /* modem CnS indication */
#define HPMATCMDIND  (swi_uint32)0x2C    /* modem AT Command indication */
#define HPMSTACKIND  (swi_uint32)0x2D    /* Protocol stack debugging 
                                          *   Host-> Modem 
                                          */
#define HPENVIND     (swi_uint32)0x2F    /* Host Environment Indication */
#define HPRESYNCIND  (swi_uint32)0x30    /* host resync indication */
#define HPLSCIND     (swi_uint32)0x31    /* Link Status Confirm Indication */
#define HPMNOTIND    (swi_uint32)0x32    /* modem (from Other Host) CnS 
                                          * notification indication 
                                          */
#define HPHSTRNAPIND (swi_uint32)0x33    /* Host RNAP Data Indication */
#define HPVSPRMDMSTS (swi_uint32)0x35    /* VSP Reverse Channel Modem Status
                                          *  Indication 
                                          */
#define HPVSPRDATA   (swi_uint32)0x36    /* VSP Reverse Channel data 
                                          * Indication 
                                          */


#define HPMINRMSGID  (swi_uint32)0x00    /* minimum reverse message id */
#define HPMAXRMSGID  (swi_uint32)0x3F    /* maximum reverse message id */


/* extended message ids in host-to-modem direction */
#define HPMOSPIND	 (swi_uint32)((0x0000<<HPEXTMSGIDSHF)|HPMEXTENDED) 
#define HPMDEVREQ	 (swi_uint32)((0x0001<<HPEXTMSGIDSHF)|HPMEXTENDED) 

/* Device Information request as defined in doc 4110048 */
#define HPMDEVREQVERSION 0x02	 
#define HPMDEVREQTECHTYP 0x01	 
#define HPMDEVREQPRDSTR  0x02	 
#define HPMDEVREQFWDNLD  0x03	
#define HPMDEVREQDEVENUM 0x04	 
#define HPMDEVREQCNS     0x05	 

#define HPMINREXTMSGID  (swi_uint32)0x0000    /* minimum reverse extended 
                                               * message id 
                                               */
#define HPMAXREXTMSGID  (swi_uint32)0x01FF    /* maximum reverse extended 
                                               * message id 
                                               */


/* message ids to the host */
#define HPDLSTRTRESP (swi_uint32)0x40    /* Download start response */
#define HPDLENDRESP  (swi_uint32)0x41    /* Download end response */
#define HPDLCONTRESP (swi_uint32)0x42    /* Download continuation response */

#define HPSWIRESP    (swi_uint32)0x42    /* SWI response */
#define HPLOOPRESP   (swi_uint32)0x43    /* Loop Back response */
#define HPPROGRESP   (swi_uint32)0x44    /* flash program response */
#define HPPSUMRESP   (swi_uint32)0x45    /* Profile summary response */


#define HPCONFRESP   (swi_uint32)0x48    /* Profile Configuration response */
#define HPPSACTRESP  (swi_uint32)0x49    /* Packet session activation 
                                          * response 
                                          */
#define HPRNAPCFGRSP (swi_uint32)0x4F    /* RNAP Configuration Response */


#define HPHSYNCIND   (swi_uint32)0x60    /* host data sync indication */
#define HPBOOTIND    (swi_uint32)0x61    /* boot information indication */
#define HPRSTARTIND  (swi_uint32)0x62    /* restart indication */
#define HPFDATAIND   (swi_uint32)0x63    /* forward data indication */
#define HPRGIND      (swi_uint32)0x64    /* reverse grant indication */
#define HPHSWIIND    (swi_uint32)0x65    /* SWI host indication */
#define HPHL1IND     (swi_uint32)0x66    /* host layer 1 indication */
#define HPSUSIND     (swi_uint32)0x67    /* host suspend indication */
#define HPRESIND     (swi_uint32)0x68    /* host resume indication */
#define HPLINKIND    (swi_uint32)0x69    /* link status indication */
#define HPABIND      (swi_uint32)0x6A    /* abort indication */
#define HPHCNSIND    (swi_uint32)0x6B    /* host CnS indication */
#define HPHATCMDIND  (swi_uint32)0x6C    /* host AT Command indication */
#define HPHSTACKIND  (swi_uint32)0x6D    /* Protocol Stack Indication. Modem
                                          *  to Host 
                                          */
#define HPHRDIND     (swi_uint32)0x6E    /* Modem (Rosetta) Debug Indication.
                                          *  Modem to Host 
                                          */
#define HPDSLPRDYIND (swi_uint32)0x6F    /* UART deep sleep ready indication */
#define HPTEMPIND    (swi_uint32)0x70    /* Temperature for other hosts 
                                          * indication 
                                          */
#define HPOHNOTIND   (swi_uint32)0x71    /* other host CnS notification 
                                          * indication 
                                          */
#define HPRSSIIND    (swi_uint32)0x75    /* WWAN RSSI indication */
#define HPLNKSTAIND  (swi_uint32)0x78    /* link status indication */
#define HPDEVTYPEIND (swi_uint32)0x79    /* Device ID indication */
#define HPMDMRNAPIND (swi_uint32)0x7A    /* Modem RNAP Data Indication */
#define HPVSPFMDMSTS (swi_uint32)0x7C    /* VSP Forward Channel Modem Status
                                          *  Indication 
                                          */
#define HPVSPFDATA   (swi_uint32)0x7D    /* VSP Forward Channel data 
                                          * Indication 
                                          */
                                          
#define HPMINFMSGID  (swi_uint32)0x40    /* minimum forward message id */
#define HPMAXFMSGID  (swi_uint32)0x7F    /* maximum forward message id */

/* extended message ids to the host */
#define HPHOSPIND	 (swi_uint32)((0x0200<<HPEXTMSGIDSHF)|HPHEXTENDED)
#define HPHDEVRSP	 (swi_uint32)((0x0201<<HPEXTMSGIDSHF)|HPHEXTENDED)

#define HPMINFEXTMSGID  (swi_uint32)0x0200 /* minimum forward extended msg id */
#define HPMAXFEXTMSGID  (swi_uint32)0x03FF /* maximum forward extended msg id */

/* The following sizes are used to define the HP Receive Task's
 * pool block allocation size. These pool blocks are actually
 * created within the HD package using these constants as the
 * block size. The following diagram illustrates the block:
 *
 * +------+--------+-------------------------------------+
 * | RSV  | HPHDR  | HPPAYLOAD                           |
 * +------+--------+-------------------------------------+
 *
 * Where:
 *     Rsv is space reserved between the start of the allocated
 *     memory block and the start of the HIP Header area
 *
 *     HPHDR is the portion of the block occupied by the HIP
 *     header whose size depends upon the HIP packet type in use
 *     Possibilities are 4 or 6 bytes for the original HIP or
 *     the extended HIP header
 *
 *     HPPAYLOAD is the storage area which will contain the 
 *     up to 2000 bytes of a HIP message. This portion is
 *     occupied by whatever protocol is being carried in this
 *     message, usually CnS. 
 *
 *     NOTE: For HIP packets containing CnS data, the following
 *     explanation is very important to understand if you need 
 *     to change the offsets for the RSV area:
 *     =======================================================
 *     The pool for these blocks is allocated by the HD package
 *     using these constants (below) for sizing the blocks. The
 *     HD package is configured with an offset into the buffer
 *     at which it should commence storing the deframed HIP 
 *     message - that offset corresponds to the size of the RSV
 *     area, above. This block is allocated within HD, but doesn't
 *     get deallocated until the CNS RR task sends the contents 
 *     to the API side over the IPC channel. The packet is decoded
 *     layer-by-layer as it proceeds up the protocols, first by 
 *     HIP, then by CnS. The pointer into the buffer is advanced
 *     as the packet is handed to each layer. The CnS layer 
 *     parses up to the start of the CnS payload area and stops. 
 *     This area is not unpacked until the packet is received by
 *     the API side. 
 *
 *     To get to the API side, the packet must be sent over an 
 *     IPC channel. IPC traffic in the SDK uses another header;
 *     instead of copying the CnS payload area into another buffer
 *     this buffer is re-used for sending over the IPC channel by
 *     prepending an AM header to it. In general, the size of the 
 *     AM header is not the same as the combined sizes of the HIP
 *     and CnS headers. A function in the CN package, 
 *     cnGetAmFromCnSp() backs up the active pointer into this 
 *     buffer by an amount equal to the size of the AMCNSRESPONSE
 *     packet. If the AM header is bigger than the combines sizes
 *     of the CNS and HIP headers, then the pointer would be backed
 *     up past the beginning of the buffer and memory not belonging
 *     to this buffer would be overwritten. This is very bad and 
 *     there is an abort condition which catches this if it should
 *     ever happen. The abort is located in cnGetAmFromCnSp() and
 *     has tripped once, which is why this long comment has been 
 *     added.
 *
 *     This header file takes a stab at reserving enough space so
 *     that the abort call should never trip again, but a proper
 *     fix would be to hand the allocation of the receive buffers
 *     up to the AM package instead of leaving it in the HD package.
 *     Since time is short and this solution (along with the abort
 *     and this explanation) will work, we'll stick with it. If the
 *     abort ever goes off again, there are two solutions:
 *
 *     1) Increase the amount of the RSV space enough that the new
 *        RSV plus the size of the HIP and CnS headers is larger 
 *        than the size of the largest AM packet (AMCNSRESPONSE,
 *        at this writing)
 *     2) Rewrite this buffer allocation mechanism so that the HD
 *        package calls a function in AM where the buffer pool is
 *        created during startup and where incoming CnS buffers 
 *        are ultimately sent. 
 *
 *     Option 2 is more complicated, so we're sticking with option
 *     1 for now. 
 *
 *     One final note: the AM packet header does not necessarily
 *     start at the beginning of the RSV area. In general, we want
 *     the combined sizes of RSV, HPHDR and the CnS header (not 
 *     shown above) to be greater than the size of the largest AM
 *     packet header. That way, as the AM packet increases in size
 *     the beginning of the buffer will have room to grow backwards
 *     to the start of the RSV area. Once the pointer goes over the
 *     limit, the SDK will abort and you will need to select option
 *     1 or 2 above to fix it. 
 *
 */

#define HPHDRLENGTH    4          /* length of normal HIP header in (bytes) */
#define HPHDRLENGTHEXT 6          /* length of extended HIP header (bytes) */
#define HPPAYLENGTH  2000         /* max length of HIP msg payload (bytes) */
#define HPRESERVE      20         /* Size of the RSV area depicted above */

/* The amount of memory to allocated for the HD receive 
 * pool blocks is defined as follows
 */
#define HPMAXMESSAGE (HPRESERVE + HPHDRLENGTH + HPPAYLENGTH)

#define HPMSGIDMSK       0x7F         /* message id mask */
#define HPPADBMSK        0x80         /* pad byte mask */
#define HPEXTMSGIDMSK    0x3FF        /* extended message id mask */
#define HPPAYLENMSK      0x7FF        /* original payload length mask */
#define HPEXTPAYLENMSK   0x3FFF       /* extended payload length mask */

/* 
 * this constant is used to indicated that 
 * information on all message ids should be dumped
 */
#define HPALLMSGID    (uint16)0xFFFF

/* Common endpoint used for detecting modem presence */
#define HPCMNENDPT  2  

/*************
 *
 * Name: hpphstatus
 *
 * Purpose:  HP Packet Handler Task status. Contains the result of handling 
 *           a request by HP Packet Handler Task.
 *             
 * Members:  HPOK - OK,
 *           HPTIMEOUT   - request timed out
 *           HPLBBUSY    - task busy with another loopback request
 *           HPFAILED    - the request failed. This could happen if there is
 *                         no modem detection and all outstanding requests 
 *                         need to be cleared. In this case the task sets the 
 *                         status to HPFAILED and calls all registerd callbacks
 *           HPDIRBUSY   - task busy with another Device Information Request
 *           HPINVALID   - Used to indicate an undefined result if required
 * Notes:   none
 *
 **************/
enum hpphstatus
{
    HPOK,
    HPTIMEOUT,
    HPLBBUSY,
    HPFAILED,
    HPDIRBUSY,
    HPINVALID
};

/*************
 *
 * Name:     hpmdmstate
 *
 * Purpose:  Enumerated list of modem states returned to the caller 
 *           whenever a modem appears or disappears
 *             
 * Members:  HPMDMOFFLINE - No modem is currently enumerated
 *           HPMDMONLINE  - Modem is enumerated
 * 
 * Notes:    None
 *
 **************/
enum hpmdmstate
{
    HPMDMOFFLINE,
    HPMDMONLINE
};

/*************
 *
 * Name: hpmdmreghndl
 *
 * Purpose: This structure is for internal use only, but the memory for
 *          it must be supplied by callers whenever they are registering
 *          to receive modem state change notifications. 
 *             
 * Members: hpmdmvalid    - Contains a validation code to ensure the 
 *                          caller's handle hasn't become corrupter
 *          hpmdmregql    - Queue link used to add the caller to the 
 *                          registration queue
 *          hpmdmregname  - Pointer to arbitrary string passed by caller
 *                          to help identify the owner of each instance
 *                          of this structure. For debugging purposes
 *          hpmdmcbfp     - Pointer to the callback function provided
 *                          by the caller at registration time
 *          hpmdmuser     - User's arbitrary value which is returned to
 *                          them in the callback whenever it is called
 *
 * Note:    None
 *              void hpmdmcbfp( enum hpmdmstate modemstate, 
 *                              swi_bool apprunning,
 *                              enum SWI_TYPE_Device devtype,
 *                              void *userp );
 *
 *              Where:
 *              =====
 *              mdmstate    is one of the enumerations of enum hpmdmstate,
 *              apprunning  TRUE, mdm is running application, FALSE, boot ldr
 *              devtype     is the type of device connected using the AM package
 *                          enumerations. Invalid if no modem is enumerated
 *              userp       is the arg passed in the request for registration
 *                          for modem state change events
 *
 */
struct hpmdmreghndl 
{
    swi_uint32    hpmdmvalid;
    struct qllink hpmdmregql; 
    swi_uint8     *hpmdmregname;
    void (*hpmdmcbfp)( enum hpmdmstate modemstate,
                       swi_bool apprunning,
                       enum SWI_TYPE_Device devtype,
                       void *userp );
    void          *hpmdmuser;
};

/*************
 *
 * Name: hpregblk
 *
 * Purpose: This struct contains information necessary to, keep stats and 
 *          deliver a HIP message for a specific message id.
 *             
 * Members: deliverfnp - Function to call to deliver the incoming message
 *
 *               Parameters: msgdesignator - HIP message id
 *                           msgparm       - HIP message specific parameter
 *                           payloadp      - pointer to newly received 
 *                                           message buffer
 *                           paylength     - length of returned msg payload 
 *                                           buf data
 *                           errorflg      - YES - if msg is shorter/longer 
 *                                               than header indicated 
 *                                           NO  - Msg length = the expected 
 *                                               length
 *
 *          hpnotregister - # of messages dropped due to unregistered msgids
 *          hptoolong     - # of messages dropped due to payload length > max
 *          hpdropnobuf   - # of messages dropped due to no buffers provided
 *
 * Notes:   The task that receives the msg is responsible for freeing 
 *          the memory
 *
 *************/
struct hpregblk
{
    void (*hpdeliverbuffp)( swi_uint32 msgdesignator, 
                            swi_uint8 msgparm, 
                            swi_uint8 *memfreep, 
                            swi_uint8 *payloadp, 
                            swi_uint16 paylength, 
                            swi_bool errorflg);
    swi_uint32 hpnotregister;
    swi_uint32 hptoolong;
};

#include "hp/hpuproto.h"

#endif


/*
 * $Log: hpudefs.h,v $
 */
