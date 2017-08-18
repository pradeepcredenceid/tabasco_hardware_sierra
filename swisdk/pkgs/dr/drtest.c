/*
 *  $Id: drtest.c,v 1.2 2010/06/09 23:37:07 epasheva Exp $
 *
 *  Filename: drtest.c
 *
 *  Purpose:  Contains code to test the DR package
 *
 * Copyright: © 2008 Sierra Wireless, Inc., all rights reserved
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
#include "dr/dridefs.h"


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
    printf( "There is no test for DR Package \n");
    return 0;
}

/*
 * $Log: drtest.c,v $
 */
