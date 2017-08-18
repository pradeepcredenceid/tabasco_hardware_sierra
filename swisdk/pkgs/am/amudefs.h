/*
 * $Id: amudefs.h,v 1.21 2010/06/09 23:29:05 epasheva Exp $
 *
 * Filename:  amudefs.h
 *
 * Purpose:   User definitions for AM package
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 */

#ifndef amudefs_h
#define amudefs_h

/* Includes */
#include "ap/SwiRcodes.h"
#include "ic/icudefs.h"
#include "dr/drudefs.h"

/* AM Package Definitions */


/* AM Packet Header Version. This value is linked into 
 * the API and SDK side of the program. It should change
 * whenever there is a change to the protocol that would
 * render AM transactions between the API and SDK
 * incompatible. 
 */
#define AMHDRVERSION 0x0004

/* MAC Address length (bytes) */
#define AMMACSIZE          6

/* Maximum AM packet lengths */
#define AMMAXREQBKLEN   1024
#define AMMAXRESPBKLEN  1024

/* Sizes in bytes of the various AM
 * CNS Packet sizes. The AM package
 * uses PI to pack the bytes into 
 * each packet, so there is no 
 * struct available to take a 
 * sizeof(), which would probably
 * not work anyway because of 
 * compiler padding. The size must
 * match the definition of the packet
 * which is defined in 
 * IDD_SwiCnSMessaging.doc
 */

/* CnS Packet Sizes (bytes) */
#define AMCNSRESPONSESZ          9 
#define AMCNSREQSZ              12 
#define AMCNSNOTIFYSZ            5 

/* Admin Packet Sizes (bytes) */
#define AMFWDWNREQSZ            86 
#define AMFWDWNRESPSZ           41 
#define AMFWDWNNOTIFYSZ          1 
#define AMAIRSRVREQSZ           40
#define AMAIRSRVRESPSZ          40
#define AMNOTIFYAIRCSZ          40
#define AMPORTNAMEREQSZ        320
#define AMPORTNAMERESPSZ       320
#define AMRESETMDMREQSZ          1
#define AMRESETMDMRESPSZ         1
#define AMDEVINFOREQSZ          32
#define AMDEVINFORESPSZ         32

/* Demux Packet Sizes (bytes) */
#define AMDEMUXREQSZ             4 /* 2 bytes device service plus 2 bytes 
                                    * operation code
                                    */
#define AMDEMUXCTLINDSZ          48 /* 2 bytes device service, 1 byte operation
                                     * code, 1 byte device present, 4 bytes era
                                     * number, AMMAXERRSTRSZ bytes error
                                     *  explanation string
                                     */
#define AMDEMUXDATAINDSZ         2 /* 2 bytes device service */

/* UI Packet Sizes (bytes) */
#define AMUIREQSZ                2 /* 2 byte data length */
#define AMUIRESPSZ               2 /* 2 byte data length */

/* Size of the common part of the AM packet, this
 * is defined inside IDD_Appmessaging.doc
 */
#define AMHEADERSZ               7 

/* CnS Total Sizes */
#define AMCNSRESPTOTAL          (AMCNSRESPONSESZ + AMHEADERSZ)
#define AMCNSREQTOTAL           (AMCNSREQSZ + AMHEADERSZ)
#define AMCNSNOTFTOTAL          (AMCNSNOTIFYSZ + AMHEADERSZ)

/* Admin Total Sizes */
#define AMFWDWNREQTOTAL         (AMFWDWNREQSZ + AMHEADERSZ)
#define AMFWDWNRESPTOTAL        (AMFWDWNRESPSZ + AMHEADERSZ)
#define AMFWDWNNOTFTOTAL        (AMFWDWNNOTIFYSZ + AMHEADERSZ)
#define AMAIRSRVREQTOTAL        (AMAIRSRVREQSZ + AMHEADERSZ)
#define AMAIRSRVRESPTOTAL       (AMAIRSRVRESPSZ + AMHEADERSZ)
#define AMNOTAIRCTOTAL          (AMNOTIFYAIRCSZ + AMHEADERSZ)
#define AMPORTNAMEREQTOTAL      (AMPORTNAMEREQSZ + AMHEADERSZ)
#define AMPORTNAMERESPTOTAL     (AMPORTNAMERESPSZ + AMHEADERSZ)
#define AMRESETMDMREQTOTAL      (AMRESETMDMREQSZ + AMHEADERSZ)
#define AMRESETMDMRESPTOTAL     (AMRESETMDMRESPSZ + AMHEADERSZ)
#define AMDEVINFOREQTOTAL       (AMDEVINFOREQSZ + AMHEADERSZ)
#define AMDEVINFORESPTOTAL      (AMDEVINFORESPSZ + AMHEADERSZ)

/* Demux Total Sizes */
#define AMDEMUXREQTOTAL         (AMDEMUXREQSZ + AMHEADERSZ)
#define AMDEMUXDATAINDTOTAL     (AMDEMUXDATAINDSZ + AMHEADERSZ)
#define AMDEMUXCTLINDTOTAL      (AMDEMUXCTLINDSZ + AMHEADERSZ)

/* UI Total Sizes */
#define AMUIREQTOTAL            (AMUIREQSZ + AMHEADERSZ)
#define AMUIRESPTOTAL           (AMUIRESPSZ + AMHEADERSZ)

/* Firmware download constants */
#define AMMAXFILENAMESZ         80
#define AMMAXERRORSTRINGSZ      40

/* AirServer Operation constants */
#define AMAIRSERVERAVAIL   0x00
#define AMAIRSERVERSELECT  0x01
#define AMAIRSERVERCHANGE  0x02

/* Port Name  Operation constants */

/* This is the size of the array in which returned strings are stored.
 * This  truncated in the code if the string length is longer than 40 characters
 */
#define AMPORTNAMESZ        40
/* 
 * Product name string as returned by the Device Information Request HIP message
 */
#define AMPRODSTRSIZE       16
/* 
 * Name:   ammsgtypes
 *
 * Purpose: Contains an enumerated list of values that identify an 
 *          AM packet's "type" field.  
 *
 *          Packets with these type fields are sent/received over
 *          Inter-process communication channels
 *
 * Members: AMMINTYPE         - Absolute minimum defined message type
 *
 *          AMCNSMINTYPE      - Minimum defined CnS message type
 *          AMCNSREQUEST      - CnS Request packet from an application
 *          AMCNSRESPONSE     - CnS Response packet from an application
 *          AMCNSNOTIFICATION - CnS Notification packet
 *          AMCNSMAXTYPE      - Maximum defined CnS message type + 1
 *
 *          AMADMINMINTYPE    - Minimum defined Admin message type
 *          AMFWDNREQUEST     - Firmware Download Request message 
 *          AMFWDWNRESPONSE   - Firmware Download response message
 *          AMFWDWNNOTIFICATION - Firmware Download Notification message
 *          AMAIRSRVREQ       - Air Server information request message
 *          AMAIRSRVRESP      - Air Server information response message
 *          AMAIRSRVCHGNOTIFY - Air Server Change notification message
 *          AMPORTNAMEREQ     - Modem device port name request message
 *          AMPORTNAMERESP    - Modem device port name response message
 *          AMMDMRESETRESP    - Modem reset request message
 *          AMMDMRESETNOTIFY  - Modem reset notification message
 *          AMUIREQUEST       - UI task request message
 *          AMUIRESPONSE      - UI task response message
 *          AMDEVINFOREQ      - Device Information Request message
 *          AMDEVINFORESP     - Device Information Response message
 *          AMMDMADMINMAXTYPE - Maximum defined administration message + 1
 *
 *          AMDEMUXMINTYPE    - Minumum defined Demux message 
 *          AMDEMUXREQUEST    - Demux begin/end request message
 *          AMDEMUXDATAIND    - Demux data indication (to/from) message
 *          AMDEMUXCTLIND     - Demux control indication message
 *          AMDEMUXMAXTYPE    - Maximum defined demux message + 1
 *
 *          AMMAXTYPE         - Absolute maximum defined message type
 *          AMINVALID         - Not a valid type. Used in some returns
 *         
 * Note:    None
 *
 */
enum ammsgtypes {
    /* Absolute minimum message value */
    AMMINTYPE = 0x0000,

    /* AMCNS: 0x0000 to 0x003F */
    AMCNSMINTYPE = AMMINTYPE,
    AMCNSREQUEST = AMCNSMINTYPE,             
    AMCNSRESPONSE,            /* 0x0001 */
    AMCNSNOTIFICATION,        /* 0x0002 */
    AMCNSMAXTYPE,             /* 0x0003 */

    /* ADMIN: 0x0040 to 0x007F */
    AMADMINMINTYPE = 0x0040,  
    AMFWDWNREQUEST = AMADMINMINTYPE,
    AMFWDWNRESPONSE,          /* 0x0041 */
    AMFWDWNNOTIFICATION,      /* 0x0042 */
    AMAIRSRVREQ,              /* 0x0043 */
    AMAIRSRVRESP,             /* 0x0044 */
    AMAIRSRVCHGNOTIFY,        /* 0x0045 */
    AMPORTNAMEREQ,            /* 0x0046 */
    AMPORTNAMERESP,           /* 0x0047 */
    AMMDMRESETREQ,            /* 0x0048 */
    AMMDMRESETRESP,           /* 0x0049 */
    AMMDMRESETNOTIFY,         /* 0x004A */
    AMUIREQUEST,              /* 0x004B */
    AMUIRESPONSE,             /* 0x004C */
    AMDEVINFOREQ,             /* 0x004D */
    AMDEVINFORESP,            /* 0x004E */
    AMADMINMAXTYPE,           /* 0x004F */

    /* Demux Range: 0x0080 to 0x00BF */
    AMDEMUXMINTYPE = 0x0080,
    AMDEMUXREQUEST = AMDEMUXMINTYPE, 
    AMDEMUXDATAIND,            /* 0x0081 */
    AMDEMUXCTLIND,             /* 0x0082 */
    AMDEMUXMAXTYPE,            /* 0x0083 */
    
    /* Maximum currently defined message + 1 */
    AMMAXTYPE = AMDEMUXMAXTYPE,

    /* Invalid message - Always last in this list */
    AMINVALID = 0xFFFF
};

/* 
 * Name:   swi_amairiftypes
 *
 * Purpose: Enumerated list of AM package air interface identifiers
 *
 * Members: SWI_AMNODEM       - No modem connected
 *          SWI_AMGSM         - Ids this packet as destined for GSM device
 *          SWI_AMCDMA        - Ids this packet as destined for CDMA device
 *          SWI_AMCOMMON      - Ids this packet as destined for any device
 *         
 * Note:    None
 *
 */
enum swi_amairiftypes {
    SWI_AMNOMODEM = 0,
    SWI_AMGSM, 
    SWI_AMCDMA,
    SWI_AMCOMMON
};

/* 
 * Name:   swi_amresults
 *
 * Purpose: Enumerated list of AM package result codes available for
 *          use by any package requiring system-wide result codes
 *
 * Members: SWI_AMMINRESULT   - For range checking
 *          SWI_AMMAXRESULT   - For range checking
 *          SWI_AMSUCCESS     - The operation was a success
 *          SWI_AMTIMEOUT     - The operation timed out
 *          SWI_AMNODEVICE    - Device not connected/detected
 *          SWI_AMCNSERROR    - Parameter field contains ASCII string
 *                              explaining the failure
 *          SWI_AMVERSERROR   - AM packet header version mismatch error
 *          SWI_AMAIFMISMATCH - The API call was for an air interface
 *                              technology that doesn't match the modem
 *                              connected. E.g. CDMA call on a GSM modem
 *          SWI_AMVERSERROR   - AM packet version error
 *          SWI_AMFWDWNABORTED- Firmware download process aborted
 *          SWI_AMFWDWNBUSY   - Firmware download already in progress
 *          SWI_AMFAILED      - Request failed
 *          SWI_AMDEVNOTSUP   - The API does not support the modem connected
 *         
 * Note:    None
 *
 */
enum swi_amresults {
    SWI_AMMINRESULT = 0, 
    SWI_AMSUCCESS = SWI_AMMINRESULT,
    SWI_AMTIMEOUT,
    SWI_AMNODEVICE,
    SWI_AMCNSERROR,
    SWI_AMAIFMISMATCH,
    SWI_AMVERSERROR,
    SWI_AMFWDWNABORTED,
    SWI_AMFWDWNBUSY,
    SWI_AMFAILED,
    SWI_AMDEVNOTSUP,
    SWI_AMMAXRESULT
};
/*
 * Name:    amcnsrrparms
 *
 * Purpose: Structure used to conveniently store all the CnS Request/
 *          Response parameters. The intent with it is to save creating
 *          functions which need this information with a whole bunch of
 *          calling arguments. Instead, a single pointer to this struct
 *          pre-filled with the information
 *
 * Members: amresultcode   - Used when building CNSRESPONSE to indicate
 *                           success or a timeout occurred
 *          amcnsparameter - Contents of the CnS Parameter field
 *          amcnsobjid     - Contents of the CnS object ID field
 *          amcnslength    - Length of the Parameter section of the CnS packet
 *          amcnstimeout   - Application-specified timeout waiting for a resp
 *          amcnsoperation - Contents of the Cns Operation Type field
 *          amcnsairint    - The air interface the request is associated with
 *
 * Note:    The members in this structure are arranged with 32 bit fields
 *          first, then 16 bit fields and finally 8-bit fields. 
 *
 *          The size of the fields in this structure are coded to match 
 *          the definition of the CnS Request packet format in document
 *          Swi_Cns_Messaging_Protocol.doc, located within the IDD folder
 *          of the SDK repository.
 */
struct amcnsrrparms {
    enum swi_amresults amresultcode;
    swi_uint32    amcnsparameter;
    swi_uint16    amcnsobjid;
    swi_uint16    amcnslength;
    swi_uint16    amcnstimeout;
    swi_uint8     amcnsoperation;
    swi_uint8     amcnsairint;
};

/*
 * Name:    amairserverparms
 *
 * Purpose: Structure used to contain Air Server Change notification
 *          parameters.
 *
 * Members: amairoperation  - Contains AirServer operation
 *          amairdevicestat - TRUE, the device is present, FALSE, the 
 *                            device is not present
 *          amairchgera     - era number associated with the device status
 *                            change
 *          amairdevicetype - TBD

 *          amairclimac     - Client's MAC address or zero-filled if
 *                            not applicable
 *          amaircservmac   - Server's MAC address or zero-filled if
 *                            not applicable
 *          amairapprunning - TRUE, the modem is executing its application
 *                            FALSE, the modem is in boot-and-hold operation
 *          amairprodstr    - Product name string returned by the modem when
 *                            detected using HIP Device Information Request, 
 *                            NULL terminated.
 *          amairtechtype   - Technology type, obtained as per amairprodstr.
 *
 * Note:    None
 *
 */
struct amairserverparms 
{
    swi_uint32       amairoperation;
    swi_bool         amairdevicestat;
    swi_uint32       amairchgera;
    swi_uint32       amairdevicetype;
    swi_uint8        amairclimac[AMMACSIZE];
    swi_uint8        amairservmac[AMMACSIZE];
    swi_bool         amairapprunning;
    swi_uint8        amairprodstr[AMPRODSTRSIZE];
    swi_uint32       amairtechtype;
};

/*
 * Name:    amdevinfoparms
 *
 * Purpose: Structure used to contain Air Server Device Information parameters.
 *
 * Members: amdevversion      - Version of request/response
 *          amdevreqstatus    - Status of request. Set in response
 *          amdevinfotype     - type of info requested
 *          amdevinfolen      - Length of info in response
 *          amdevtechology    - Technology of device
 *          amdevprodstr      - Product String
 *          amdevfwdnldproto  - FW Download protocol
 *          amdevreenum       - Device re-enumeration
 *          amdevcnssupported - CNS supported by application
 * 
 * Note:    Last for dependent on amdevinfotype requested, by per-request basis
 *
 */
struct amdevinfoparms 
{
    swi_uint16       amdevversion;
    swi_uint16       amdevreqstatus;
    swi_uint16       amdevinfotype;
    swi_uint16       amdevinfolen;
    swi_uint16       amdevtechology;
    swi_uint8        amdevprodstr[AMPRODSTRSIZE];
    swi_uint16       amdevfwdnldproto;
    swi_uint16       amdevreenum;
    swi_uint16       amdevcnssupported;
};

/* 
 * Name:   swi_amfwimagetype
 *
 * Purpose: Enumerated list of firmware download status states.
 *
 * Members: SWI_FW_INIT            - firmware download procedure initializing
 *          SWI_FW_INIT_ERR        - initializing procedure error
 *          SWI_FW_SOFT_RESET      - firmware download reset to bootnhold mode
 *          SWI_FW_SOFT_RESET_ERR  - soft reset error
 *          SWI_FW_START_REQ       - firmware download start request sent
 *          SWI_FW_START_REQ_ERR   - start request error
 *          SWI_FW_FILE_ERR        - firmware image file error
 *          SWI_FW_DWN             - firmware download continuation request sent
 *          SWI_FW_DWN_ERR         - continuation request error
 *          SWI_FW_CHKSUM          - firmware download end request sent
 *          SWI_FW_CHKSUM_ERR      - end request error
 *          SWI_FW_FLASH           - program flash request sent
 *          SWI_FW_FLASH_ERR       - program flash request error
 *          SWI_FW_LAUNCH_FRAG     - launch fragment request sent
 *          SWI_FW_LAUNCH_FRAG_ERR - launch fragment request error
 *          SWI_FW_RESAMAIRSRVREQTOTALET           - device resetting
 *          SWI_FW_RESET_ERR       - device reset error
 *          SWI_FW_DONE            - firmware download successfully completed
 *
 * Note:    None
 *
 */
enum swi_amfwdwnnotiftype
{       
    SWI_FW_INIT,
    SWI_FW_INIT_ERR,
    SWI_FW_SOFT_RESET,
    SWI_FW_SOFT_RESET_ERR,
    SWI_FW_START_REQ,
    SWI_FW_START_REQ_ERR,
    SWI_FW_FILE_ERR,
    SWI_FW_DWN,
    SWI_FW_DWN_ERR,
    SWI_FW_CHKSUM,
    SWI_FW_CHKSUM_ERR,
    SWI_FW_FLASH,
    SWI_FW_FLASH_ERR,
    SWI_FW_LAUNCH_FRAG,
    SWI_FW_LAUNCH_FRAG_ERR,
    SWI_FW_RESET,
    SWI_FW_RESET_ERR,
    SWI_FW_DONE
};

/* 
 * Name:   swi_amfwimagetype
 *
 * Purpose: Enumerated list of image types supported by firmware
 *          download operation.
 *
 * Members:   SWI_FW_DWN_APPL  - application
 *            SWI_FW_DWN_BOOT  - SWI boot loader
 *            SWI_FW_DWN_QCOM  - QCOM boot loader
 *            SWI_FW_DWN_USBD  - USB descriptor
 *            SWI_FW_DWN_EXEC  - device executable
 *            SWI_FW_DWN_SWOC  - software on card
 *         
 * Note:    None
 *
 */
enum swi_amfwimagetype
{
    SWI_FW_DWN_APPL,
    SWI_FW_DWN_BOOT,
    SWI_FW_DWN_QCOM,
    SWI_FW_DWN_USBD,
    SWI_FW_DWN_EXEC,
    SWI_FW_DWN_SWOC
};

/*
 * Name:    amfwdwnrrparms
 *
 * Purpose: Structure used to conveniently store all the Firmware Download
 *          Request/Response parameters.
 *
 * Members: amresultcode   - Used when building FWDWNRESPONSE to indicate
 *                           success or a timeout occurred
 *          amfilename     - Name of the file containing the image to be
 *                           downloaded to the device.
 *          amfwimagetype  - Type of image to be downloaded to the device.
 *          ammpdownload   - Flag indicating if downloading to an MP.
 *          ammaxchunksize - Maximum chunk size to be used when copying
 *                           image to the device.
 *          amerrorstrlen  - length of the ASCII error string returned
 *          amerrorstr     - ASCII error string
 *
 * Note:    The size of the fields in this structure are coded to match 
 *          the definition of the Firmware Download request packet format 
 *          in document Swi_Cns_Messaging_Protocol.doc, located within 
 *          the IDD folder of the SDK repository.
 */
struct amfwdwnrrparms 
{
    char                   amfilename[AMMAXFILENAMESZ];
    enum swi_amfwimagetype amfwimagetype;
    swi_bool               ammpdownload;
    swi_uint32             ammaxchunksize;
    swi_uint16             amerrorstrlen;
    char                   amerrorstr[AMMAXERRORSTRINGSZ];
    enum swi_amfwdwnnotiftype amfwdwnstate;
};

/*
 * Name:    amusbportnameparms
 *
 * Purpose: Structure used to conveniently store all the endpoint port names
 *          Request/Response parameters.
 *
 * Members: amhipport   - HIP port name
 *          amdiagport  - DIAG port name
 *          amnmeaport  - NMEA port name
 *          amatcmdport - AT command port name
 *          amdata1port - DATA 1 port name
 *          amdata2port - DATA 2 port name
 *          amdata3port - DATA 3 port name
 *          ammstorport - DMass Storage port name)
 *
 * Note:    The size of the fields in this structure are coded to match 
 *          the definition of the Usb port name request packet format 
 *          in document Swi_Cns_Messaging_Protocol.doc, located within 
 *          the IDD folder of the SDK repository.
 */
struct amusbportnameparms 
{
    swi_uint8   amhipport[AMPORTNAMESZ];
    swi_uint8   amdiagport[AMPORTNAMESZ];
    swi_uint8   amnmeaport[AMPORTNAMESZ];
    swi_uint8   amatcmdport[AMPORTNAMESZ];
    swi_uint8   amdata1port[AMPORTNAMESZ];
    swi_uint8   amdata2port[AMPORTNAMESZ];
    swi_uint8   amdata3port[AMPORTNAMESZ];
    swi_uint8   ammstorport[AMPORTNAMESZ];
};

/*
 * Name:    ammdmresetparms
 *
 * Purpose: Structure used to store the modem reset request parameters
 *
 * Members: amresettype - FALSE - normal soft reset
 *                        TRUE  - reset to boot and hold
 *
 * Note:    None
 *
 */
struct ammdmresetparms 
{
    /*! TRUE, reset to boot and hold, FALSE normal soft reset */
    swi_bool  amresettype;
};

/* 
 * Name:   amrrparmtype
 *
 * Purpose: Enumerated list of AM packet parameter field types
 *
 * Members: SWI_PARM_CNS      - CnS AM packet
 *          SWI_PARM_FWDWN    - Firmware Download AM packet
 *          SWI_PARM_AIRSRV   - Air Server AM packet
 *          SWI_PARM_PORTNAME - Port Name AM packet
 *          SWI_PARM_DEMUX    - Demux AM packet
 *          SWI_PARM_RESET    - Reset the modem
 *          SWI_PARM_DEV_INFO - Request for device information
 *
 * Note:    None
 *
 */
enum amrrparmtype
{
    SWI_PARM_CNS,
    SWI_PARM_FWDWN,
    SWI_PARM_AIRSRV,
    SWI_PARM_PORTNAME,
    SWI_PARM_DEMUX,
    SWI_PARM_RESET,
    SWI_PARM_DEV_INFO
};


/*
 * Name:    amrrparms
 *
 * Purpose: Structure used to store all AM Request/Response parameters.
 *
 * Members: amparmtype   - Type of the AM parameter field
 *          amtimeout    - Name of the file containing the image to be
 *                           downloaded to the device.
 *          amresultcode - result of the AM request/response transaction
 *          amparm       - AM parameter field union
 *              amfwdwn  - Firmware Download request/response parameters
 *              amcns    - CnS request/response parameters
 *              amairchg - Storage for incoming Air Server Change notifications
 *              amusbportname - Storage for endpoints port names
 * 
 * Note:    None
 */
struct amrrparms
{
    enum amrrparmtype  amparmtype;
    swi_uint16         amtimeout;
    enum swi_amresults amresultcode;
    union
    {
        struct amfwdwnrrparms   amfwdwn;
        struct amcnsrrparms     amcns;
        struct amairserverparms amairsrv;
        struct amusbportnameparms amusbportname;
        struct ammdmresetparms  amreset;
        struct amdevinfoparms   amdevinfo;
    } amparm;
};

#include "am/amuproto.h"

#endif

/*
 * $Log: amudefs.h,v $
 */
