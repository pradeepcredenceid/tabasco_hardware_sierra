/*
 *  $Id: ostest.c,v 1.1 2008/05/13 04:55:06 rfiler Exp $
 *
 *  Filename: ostest.c
 *
 *  Purpose:  This file contains routine(s) that exercise the 
 *            OS package
 *
 * Copyright: © 2008 Sierra Wireless, Inc., all rights reserved
 *
 */

/* Linux definitions */
#include <stdio.h>
#include <stdlib.h>

/* include files */
#include "aa/aaglobal.h"
#include "mm/mmudefs.h"

/**************
 *
 * Name:     main
 *
 * Purpose:  Entry point where testing of the OS package starts. 
 *           
 * Parms:    argc   - Not used
 *           argv[] - Not used
 * Return:   0 - Normal termination
 *           1 - Problem with the execution
 *
 * Notes:    none
 *
 **************/
int main( int argc, char *argv[] )
{
    printf("Currently there is no OS package test. Instead, execute the\n");
    printf("following command from within the os directory:\n");
    printf("          make -f osmakefile\n" );
    printf("and then execute the resulting test program - sdktest\n");
    return 0;
}

/*
 * $Log: ostest.c,v $
 */
