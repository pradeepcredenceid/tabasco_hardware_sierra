/*************
 *
 * $Id: sludefs.h,v 1.5 2009/03/05 00:42:25 blee Exp $
 *
 * Filename - sludefs.h - sl package user definitions and declarations
 *
 * Purpose -  This is the public include file for the String
 *            Library package
 *
 * Copyright: © 2008 Sierra Wireless Inc.,  all rights reserved
 *
 **************/
#ifndef SLUSER_H
#define SLUSER_H

/* Include files */
#include <stdio.h>       /* needed for size_t */
#include <stdint.h>      /* needed for uintptr_t */

/*
 *  Local constants and enumerated types.
 */
#define    SLCR    0x0D        /* carriage return */
#define    SLLF    0x0A        /* line feed */

/*
 * structures
 */

/*************
 *
 * Name:  slstrtokblk
 *
 * Purpose:  This structure contains the pointer to where the 
 *           untokenized character sequence is located.             
 *             
 * Members:  slstrtokevalp - points to first character after the 
 *                           delimiter.  Used to evaluate whether 
 *                           the sequence of characters after the 
 *                           delimiter is a token. 
 *
 * Notes:    Supports static data for slstrtok() reentrancy. 
 *
 **************/
struct slstrtokblk
{
    char *slstrtokevalp;                
};

#include "sluproto.h"        /* user prototypes */

#endif
/*
 * $Log: sludefs.h,v $
 */
