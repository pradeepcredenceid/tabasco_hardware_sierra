/*
 *  $Id: uitest.c,v 1.2 2010/06/09 23:54:54 epasheva Exp $
 *
 *  Filename: uitest.c
 *
 *  Purpose:  Contains code to test the UI package
 *
 * Copyright: © 2009 Sierra Wireless, Inc., all rights reserved
 *
 * Note:      none
 *
 */

/* Linux definitions */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>

/* include files */
#include "ui/uiidefs.h"


/**************
 *
 * Name:     main
 *
 * Purpose:  Entry point where testing of the DR package starts. 
 *           
 * Parms:    argc   - Not used
 *           argv[] - Not used
 * Return:   0 - Normal termination
 *           1 - Problem with the execution
 *
 * Notes:    None
 *
 **************/
int main( int argc, char *argv[] )
{
    /* Currently there is no test */
    /* Note: Initial testing of the package were performed using the
     * exisitng functionality in SDK and registering for a
     * HPLOOPRESP message with hpregisterhip() function.
     */
    /* Print out the result */
    printf( "There is no test for UI Package \n");

    return 0;
}

/*
 * $Log: uitest.c,v $
 */
