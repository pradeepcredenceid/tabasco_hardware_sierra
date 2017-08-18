/*
 * $Id: usudefs.h,v 1.17 2010/08/24 20:31:58 mdejong Exp $
 *
 * Filename: usudefs.h
 *
 * Purpose:  User definitions for the US package
 *
 * Copyright: C 2008 Sierra Wireless Inc., All rights reserved
 *
 */

#ifndef ususer_h
#define ususer_h

/* Includes */
#include "aa/aaglobal.h"

/* Defines */

#define USNOTERMINATION SWI_NOREAD_TERMINATION
#define USNOTHRESHOLD SWI_NOREAD_THRESHOLD

/* Invalid EP numbers */
#define USEPINVALID 0

/* Direction Definitions */
#define USWRITE       FALSE
#define USREAD        TRUE

/* When waiting for the modem to appear we need 
 * to know in advance what EP to wait on. All 
 * Sierra modems have an EP 2, so this is the 
 * one we always use; when the modem finally 
 * appears, whatever its product ID, it will 
 * have an endpoint 2. However, only 1 thread 
 * per endpoint is allowed - 2 threads in this
 * case, one for the read EP and one for the 
 * write EP. After we return from the wait call
 * we can determine which actual EP we need
 * to use for HIP, etc based on the product ID.
 */
#define USREADWAITEP     2
#define USWRITEWAITEP    2

/*
 * Name: swi_usvendors
 *
 * Purpose: Enumerated list of USB Vendor IDs known by this SDK. 
 *
 * Members: SWI_VIDSIERRA  - Sierra Wireless Vendor ID
 *          SWI_VIDHP      - Sierra Wireless/HP products
 *          SWI_VIDAIRPM   - Sierra Wireless/Airprime
 *
 * Notes: none
 *
 */
enum swi_usvendors
{
    SWI_VIDSIERRA = 0x1199,
    SWI_VIDHP     = 0x03F0,
    SWI_VIDAIRPM  = 0x0F3D
};


/*
 * Name: swi_usproducts
 *
 * Purpose: Enumerated list of product IDs known by this SDK. This
 *          list should match the product IDs known by the Sierra
 *          driver.
 *
 * Members: --- CDMA Modems --- 
 *          SWI_MC5725GM    - Sierra Generic w/MUX Driver
 *          SWI_MC5725LM    - Lenovo w/MUX Driver
 *          SWI_MC5725HM    - HP w/MUX Driver
 *          SWI_AC595GM     - Sierra Generic w/MUX Driver
 *          SWI_AC597EGM    - Sierra Generic w/MUX
 *          SWI_C597UM      - Compass 597 Generic w/MUX
 *          SWI_MC5727GM    - Generic w/MUX Driver
 *          SWI_MC5727LM    - Lenovo w/MUX Driver
 *          SWI_AC597EM     - Generic w/MUX Driver
 *          SWI_T598UM      - Generic w/MUX Driver
 *          SWI_T11UM       - Generic w/MUX Driver
 *          SWI_AC402GM     - Generic w/MUX Driver
 *          SWI_MC5728GM    - Generic w/MUX Driver
 *          SWI_SL501XGM    - Generic w/MUX Driver
 *          --- GSM Modems ---
 *          SWI_MC8775GM    - Generic w/MUX Driver
 *          SWI_MC8775LM    - Lenovo w/MUX Driver
 *          SWI_MC8775CM    - Cisco w/MUX Driver
 *          SWI_MC8775EM    - Ericsson w/MUX Driver
 *          SWI_AC875G      - AC875 Generic
 *          SWI_AC875UG     - AC875U Generic
 *          SWI_AC875EG     - AC875E Generic
 *          SWI_AC880G      - AC880 Generic
 *          SWI_AC880EG     - AC880E Generic
 *          SWI_AC881G      - AC881 Generic
 *          SWI_AC881UG     - AC881U Generic
 *          SWI_AC881EG     - AC881E Generic
 *          SWI_AC885G      - Compass AC885 Generic
 *          SWI_AC885EG     - Compass AC885E GenericL
 *          SWI_MC8785GC    - MC8785 Generic Composite
 *          SWI_MC8790GC    - MC8790 Generic Composite
 *          SWI_AC888G      - Compass 888/USB 303/USB 304/AirCard 501 
 *          SWI_MC8777      - MC8777 Composite PID
 *          SWI_USBDIP      - All Direct IP modems
 * 
 * Notes: In the list above, "Generic" indicates the modem has
 *        the Sierra Wireless Vendor ID in its USB descriptor
 *
 *        If the modem detected is a Direct IP modem, then 
 *        the SDK must perform additional querying to determine
 *        exactly which modem it is
 */
enum swi_usproducts
{
    /* --- CDMA Modems */
    SWI_MC5725GM    = 0x0020,
    SWI_MC5725LM    = 0x0220,
    SWI_MC5725HM    = 0x211D,
    SWI_AC595GM     = 0x0019,
    SWI_AC595UGM    = 0x0120,
    SWI_AC597EM     = 0x0021,
    SWI_C597UM      = 0x0023,
    SWI_MC5727GM    = 0x0024,
    SWI_MC5727LM    = 0x0224,
    SWI_T598UM      = 0x0025,
    SWI_T11UM       = 0x0026,
    SWI_AC402GM     = 0x0027,
    SWI_MC5728GM    = 0x0028,
    SWI_SL501XGM    = 0x0300, /* SL5010 and SL5011*/
    SWI_AC802GM     = 0x0400,
    /* --- GSM Modems --- */
    SWI_MC8775GM    = 0x6812,
    SWI_MC8775LM    = 0x6813,
    SWI_MC8775CM    = 0x6815,
    SWI_MC8775EM    = 0x6816,
    SWI_AC875G      = 0x6820,
    SWI_AC875UG     = 0x6821,
    SWI_AC875EG     = 0x6822,
    SWI_MC8780      = 0x6832, /* Sierra Wireless MC8780 */
    SWI_MC8781      = 0x6833, /* Sierra Wireless MC8781 */
    SWI_AC880G      = 0x6850,
    SWI_AC880EG     = 0x6852,
    SWI_AC881G      = 0x6851,
    SWI_AC881EG     = 0x6853,
    SWI_AC880U      = 0x6855,
    SWI_AC881UG     = 0x6856,
    SWI_AC885G      = 0x6880,
    SWI_AC885EG     = 0x6859,
    SWI_MC8785GC    = 0x683B,
    SWI_MC8790GC    = 0x683C,
    SWI_AC888G      = 0x6890, /* Compass 888/USB 303/USB 304/AirCard 501 */
    SWI_MC8777      = 0x6893, /* MC8777 */
    SWI_USBDIP      = 0x68A3  /* All Direct IP Modems */
};

/*
 * Name: usmodemmode
 *
 * Purpose: Enumerated list of modem operating modes, used during
 *          device detection to identify what mode the modem has
 *          been detected in
 *          
 * Members: USMODE_MIN      - For range checking
 *          USMODE_BOOTHOLD - For boot-and-hold operation
 *          USMODE_NORMAL   - For normal modem operation
 *          USMODE_MAX      - Maximum value for range checking
 *
 * Notes: DO NOT CHANGE THIS ENUMERATION UNLESS YOU KNOW WHAT YOU
 *        ARE DOING. YOU WILL BREAK THE LOGIC IN "usdoloopback()"
 *        IF YOU DO.
 */
enum usmodemmode {
    USMODE_MIN = 0,
    USMODE_BOOTHOLD = USMODE_MIN,
    USMODE_NORMAL,
    USMODE_MAX
};

/*
 * Name:    useptype
 *
 * Purpose: Enumerated list of end point types. Users use these to request
 *          end point name. 
 *          
 * Members:     USEP_HIP    - The HIP end point
 *              USEP_DIAG   - The DIAG end point  
 *              USEP_NMEA   - The NMEA endpoint
 *              USEP_ATCMD  - The AT Command end point
 *              USEP_DATA1  - Data 1 end point
 *              USEP_DATA2  - Data 2 end point 
 *              USEP_DATA3  - Data 3 end point
 *              USEP_DIP    - Direct IP 
 *              USEP_QMI    - QMI 
 *              USEP_MASS_STORAGE
 *              USEP_INVALID- Invalid end point
 * 
 *  Notes:  Refer to document 2130634 USB Driver Developer's Guide for details
 */
enum useptype {
    USEP_HIP,
    USEP_DIAG,
    USEP_NMEA,
    USEP_ATCMD,
    USEP_DATA1,
    USEP_DATA2,
    USEP_DATA3,
    USEP_DIP,         /* not implemented yet */
    USEP_QMI,         /* not implemented yet */
    USEP_MASS_STORAGE, /* not implemented yet */
    USEP_INVALID
};

/*
 * Name:    usbep
 *
 * Purpose: US endpoints 
 *          
 * Members: usreadep    - read endpoint  
 *          uswriteep   - write endpoint
 *
 * Notes: none
 *
 */
struct usbep {
    swi_uint32 usreadep;
    swi_uint32 uswriteep;
};

#include "usuproto.h"

#endif

/*
 * $Log: usudefs.h,v $
 */
