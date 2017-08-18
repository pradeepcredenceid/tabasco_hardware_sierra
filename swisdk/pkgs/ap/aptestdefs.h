/*************
 *
 * $Id: aptestdefs.h,v 1.5 2008/11/05 19:39:42 rfiler Exp $
 *
 * Filename: aptestdefs.h
 *
 * Purpose:  This file contains internal structures and constants for the 
 *           test programs
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* Includes */

/* Defines */
#define DEFAULTTIMEOUT 3000

/* Max chars on command line for "-n" option */
#define APMAXTESTS  80      

/*************
 *
 * Name:    aptestab
 *
 * Purpose: API package test routines structure. Contains
 *          various elements needed by the API test functions 
 *             
 * Members: testroutinep  Pointer to API test function 
 *          estrp         Pointer to test function tag name
 *          fstrp         Pointer to API function name
 * Notes:   none
 *
 **************/

struct aptesttab
{
    void (*testroutinep)(swi_uint32);
    char *estrp;   /* enum string pointer */
    char *fstrp;   /* function name string pointer */
};

# include "aptestproto.h"

/*
 * $Log: aptestdefs.h,v $
 */
