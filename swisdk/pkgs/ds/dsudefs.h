/*
 * $Id: dsudefs.h,v 1.7 2009/06/01 23:25:22 blee Exp $
 *
 * Filename:  dsudefs.h
 *
 * Purpose:   User definitions for DS package
 *
 * Copyright: C 2008 Sierra Wireless Inc., all rights reserved
 *
 */

#ifndef dsudefs_h
#define dsudefs_h

/* Includes */

/* DS Package Definitions */

/* DS structures */
/*************
 *
 * Name:    dsipcshell
 *
 * Purpose: DS IPC shell structure
 *             
 * Members: dsipcshellnamep  - Pointer to DS shell name
 *          dsipccbfcnp      - Callback function of the DS user.
 *
 * Note:    This structure is for theDS package
 *
 **************/
struct dsipcshell
{
    swi_uint8 *dsipcshellnamep;
    void      *dsipccbfcnp;
};

#include "ds/dsuproto.h"

#endif

/*
 * $Log: dsudefs.h,v $
 */
