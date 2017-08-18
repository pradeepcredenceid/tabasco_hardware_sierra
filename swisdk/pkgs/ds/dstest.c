/*
 *  $Id: dstest.c,v 1.2 2010/06/09 23:39:36 epasheva Exp $
 *
 *  Filename: dstest.c
 *
 *  Purpose:  Contains code to test the DS package
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
#include "ds/dsidefs.h"

/**************
 *
 * Name:     main
 *
 * Purpose:  Entry point where testing of the DS package starts. 
 *           
 * Parms:    argc   - Not used
 *           argv[] - Not used
 * Return:   0 - Normal termination
 *           1 - Problem with the execution
 *
 * Notes:    The test involves parsing the packet above and printing
 *           out the contents of each of the fields for verification.
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
    printf( "There is no test for DS Package \n");
    return 0;
}

/*
 * $Log: dstest.c,v $
 */
