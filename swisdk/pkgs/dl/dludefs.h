/*
 * $Id: dludefs.h,v 1.13 2010/06/09 23:36:10 epasheva Exp $
 *
 * Filename: dludefs.h
 *
 * Purpose: User definitions for the Debug Logger package (DL)
 *
 * NOTES:   none
 *
 * Copyright Sierra Wireless Inc., 2008. All rights reserved.
 */


#ifndef dludefs
#define dludefs

/* include files */

/* Definitions and enumerations */

#define DLCLASSALL    0xFFFF            /* Log regardless of classmask */

#define DLPID_MAX           30
#define DLPKGNAMELEN_MAX    10

/*
 * Name:    dlusercb
 *
 * Purpose: Users requiring to log debug information must have an instance 
 *          of this user block structure.
 *
 * Members:
 *          dlpkgid        - identify the package
 *          dlclassmask    - Mask of classes that are activated.
 *          dlmasterenable - Enable/Disable debug logging for package
 *          dlpkgnamep     - pointer to user supplied name
 * 
 * Note:    Normal package name should be 2 characters, eg, "SL", "DL". Some
 *          applications require more, eg, "DS_CNRR1", meaning DS shell, 
 *          CN RR channel 1. For UI display purposes, DLPKGNAMELEN_MAX is used
 *          and names will be truncated if longer and may not work in UI if the
 *          complete name is not provided.
 */
struct dlusercb 
{
    swi_uint16  dlpkgid;
    swi_uint16  dlclassmask;
    swi_bool    dlmasterenable;
    char       *dlpkgnamep;
};

#include "dl/dluproto.h"

#endif /* dludefs */

/*
 * $Log: dludefs.h,v $
 */
