/*************
 *    
 * $Id: dltest.c,v 1.4 2010/06/09 23:35:51 epasheva Exp $
 *    
 * Filename: dltest.c - Test the logging library
 *
 * Purpose:  This file has test routines for the logging
 *           library (excl. tests for the ARM250 routines)
 *
 * Copyright: � 2008 Sierra Wireless Inc.,  all rights reserved
 *
 **************/

/* include files */

#include "aa/aaglobal.h"
#include <string.h> 
#include <stdlib.h>
#include <stdio.h>
#include "dludefs.h"

/*
 *  Local constants and enumerated types.
 */

/*
 * local structures
 */

/* 
 * functions    
 */

/*************
 *    
 * Name: main 
 *    
 * Purpose: This function is the main entry point for the
 *          logging library testing program
 *             
 * Parms:   argc - quantity of command line parameters (not used)
 *          argv - command line parameters (not used)
 *
 * Return:  0 
 *
 * Abort:   None (No intentional fatal errors).
 *
 * Notes:   None. 
 *
 **************/
int main(int argc, char* argv[])
{
    struct dlusercb  usercb;
    swi_uint16 myPID = 0;
    char temp[10];

    /* tell user what function is being tested */
    printf("Testing dlLog package!\n");

    for (; myPID < DLPID_MAX; myPID++)
    {
        memset (temp, 0, 10);
        snprintf (&temp[0], 10, "DLTEST%d", myPID );
        dlregister ( &temp[0], &usercb, TRUE);
        printf ("Global enable, package enable, %d, %d\n", myPID, DLPID_MAX);
        dllogenable (TRUE);
        usercb.dlmasterenable = TRUE;
        dlLog (&usercb, DLCLASSALL, "This is package %d, %c %s\n", myPID, 
                                  (swi_uint32)'c',(swi_uint32)"test", 
                                  (swi_uint32)NULL);
        dlLog (&usercb, DLCLASSALL, 
               "This is package %d with 4 arguments, %s %s %i\n", 
               myPID, (swi_uint32)"tada" ,(swi_uint32)"test", myPID+1000);
        
        /* We want to print the percent symbol here */
        dlLog (&usercb, DLCLASSALL, 
               "This is package %d with %% symbol, %s %s %i\n", 
               myPID, (swi_uint32)"tada" ,(swi_uint32)"test", myPID+1000);

        /* This will cause an erAbort. Uncomment this to test erAbort() */
        /*
        dlLog (&usercb, DLCLASSALL, 
               "This is package %d with 5 formats for 4 arguments, %s %s %i %d\n", 
               myPID, (swi_uint32)"tada" ,(swi_uint32)"test", myPID+1000);
        */
        usercb.dlmasterenable = FALSE;
        printf ("Global enable, package disable -\
You will not see this line in the user.log\n");
        dlLog (&usercb, DLCLASSALL, "You will not see this line in the user.log\n"
                                  ,(swi_uint32)NULL,
                                  (swi_uint32)NULL,
                                  (swi_uint32)NULL,
                                  (swi_uint32)NULL);

        dllogenable (FALSE);
        usercb.dlmasterenable = TRUE;

        printf ("Global disable, package enable -\
You will not see this line in the user.log\n");
        dlLog (&usercb, DLCLASSALL, "You will not see this line in the user.log\n"
                                  ,(swi_uint32)NULL,
                                  (swi_uint32)NULL,
                                  (swi_uint32)NULL,
                                  (swi_uint32)NULL);
    }
    return 0;
}

/*
 * $Log: dltest.c,v $
 */
