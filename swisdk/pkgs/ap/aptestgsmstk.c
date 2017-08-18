/*
 *  $Id: aptestgsmstk.c,v 1.2 2010/06/10 00:27:06 epasheva Exp $
 *
 *  Filename: aptestgsmstk.c
 *
 *  Purpose:  Contains functions for testing STK APIs
 *
 * Copyright: © 2009 Sierra Wireless, Inc., all rights reserved
 *
 */

/* Linux definitions */
#include <stdio.h>

/* include files */
#include "SwiDataTypes.h"
#include "SwiApiGsmBasic.h"
#include "SwiApiGsmStk.h"
#include "apudefs.h"
#include "aptestdefs.h"

/**************
 *
 * Name:     doSwiGetSIMToolKitCommand
 *
 * Purpose:  Exercises the doSwiGetSIMToolKitCommand API - calls the API
 *           and prints information related to the API
 *
 * Parms:    timeout   - timeout value for stop and wait functions as
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    This API requires interative activity.
 *
 **************/
package void doSwiGetSIMToolKitCommand(unsigned int timeout)
{
    printf ("\nPlease use the STK sample application to perform this action.\n");
}

 /*
  * $Log: aptestgsmstk.c,v $
  */

