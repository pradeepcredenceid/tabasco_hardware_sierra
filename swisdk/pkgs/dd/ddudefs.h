/*
 * $Id: ddudefs.h,v 1.2 2008/08/06 22:06:38 rfiler Exp $
 *
 * Filename:  ddudefs.h
 *
 * Purpose:   User definitions for DD package
 *
 * Copyright: C 2008 Sierra Wireless Inc., all rights reserved
 *
 */
#ifndef ddudefs_h
#define ddudefs_h


/*
 * Name: ddwitech
 *
 * Purpose: Enumerated list of wireless technologies 
 *
 * Members: DDUNDEF  - No modem is currently connected
 *          DDGSM    - Indicates GSM technology
 *          DDCDMA   - Inficates CDMA technology
 *
 * Note:    None
 *
 */
enum ddwitech {
        DDUNDEF, 
        DDGSM, 
        DDCDMA
};

#include "dd/dduproto.h"

#endif

/*
 * $Log: ddudefs.h,v $
 */
