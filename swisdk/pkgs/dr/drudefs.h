/*
 * $Id: drudefs.h,v 1.2 2008/10/15 00:20:10 epasheva Exp $
 *
 * Filename:  drudefs.h
 *
 * Purpose:   User definitions for DR package
 *
 * Copyright: C 2008 Sierra Wireless Inc., all rights reserved
 *
 */

#ifndef drudefs_h
#define drudefs_h

/* Includes */

/* DR Package Definitions */

/* 
 * Name:   drdevservicetype
 *
 * Purpose: Enumerated list of DR  device service types
 *
 * Members: DRDIAG    - DIAG device service
 *          DRNMEA    - NMEA device service
 *
 * Note:    The device service determines the IPC channel used by
 *          a DS shell instance
 *          Future extensions will allow endpoins and IPC to be treated 
 *          uniformly by the DS shell instance tasks.
 *
 */
enum drdevservicetype
{
    DRDIAG,
    DRNMEA
};

#include "dr/druproto.h"

#endif

/*
 * $Log: drudefs.h,v $
 */
