/*************
 *
 * Filename:    sdkudefs.h
 *
 * Purpose:     Global definitions used inside the SDK
 *
 * Copyright: © 2011 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* Pragmas */
#pragma once

/*************
 *
 * Name:    sdkpkg_e
 *
 * Purpose: SDK package enumeration
 *
 * Members: SDK_AAPKG    -
 *          SDK_AMPKG    - 
 *          SDK_CIPKG    - interprocess communication facilities
 *          SDK_DLPKG    - data logging
 *          SDK_DRPKG    - data relay
 *          SDK_DSPKG    - device services
 *          SDK_ERPKG    - error handling
 *          SDK_HPPHPKG  - HP Packet Handler task
 *          SDK_HPRXPKG  - HP Receive task
 *          SDK_HPTXPKG  - HP Transmit task
 *          SDK_ICPKG    - intertask communication facilities
 *          SDK_MMPKG    - memory management
 *          SDK_OSPKG    - operating System interface
 *          SDK_PIPKG    - Processor independent byte order insert/extract
 *          SDK_CNPKG    - CNS package 
 *          SDK_SDKPKG   - entities common to all other SDK packages
 *          SDK_SLPKG    - string Library
 *          SDK_SUPKG    - start-up
 *          SDK_UIPKG    - user interface
 *          SDK_USSPKG   - usb scan task
 *          SDK_USDPKG   - usb detection task
 *
 * Notes:   None
 *
 **************/
enum sdkpkg_e{
    SDK_AAPKG,
    SDK_AMPKG,   /* 1 */
    SDK_CIPKG,
    SDK_CNPKG,
    SDK_DLPKG,
    SDK_DRPKG,   /* 5 */
    SDK_DSPKG,   
    SDK_ERPKG,
    SDK_HPPHPKG,
    SDK_HPRXPKG,
    SDK_HPTXPKG, /* 10 */
    SDK_ICPKG,
    SDK_MMPKG,
    SDK_OSPKG,
    SDK_PIPKG,
    SDK_SDKPKG,  /* 15 */
    SDK_SLPKG,
    SDK_SUPKG,   
    SDK_UIPKG,
    SDK_USSPKG,
    SDK_USDPKG   /* 20 */
};
