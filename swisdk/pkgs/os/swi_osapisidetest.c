/*************
 *
 * $Id: swi_osapisidetest.c,v 1.4 2008/05/02 18:56:41 rfiler Exp $
 *
 * Filename: swi_osapisidetest.c
 *
 * Purpose:  Test program for OS wrapper that emulates application process
 *
 * Copyright: © 2008 Sierra Wireless, Inc. all rights reserved
 *
 * The SDK process creates UNIX IPC sockets. To view them, type:
 *      netstat -a --protocol=unix|grep SWI
 *
 * To see what is happening on the SDK side, look in the log:
 *      tail -100 /var/log/user.log
 *
 **************/

/* include files */
#include "../aa/aaglobal.h"    /* system wide typedefs */
#include "swi_osapi.h"         /* package user include file  */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

/*************
 *
 * Name:     main
 *
 * Purpose:  Main entry point for application process.
 *           
 * Params:   No values
 *
 * Return:   No value
 *
 * Abort:    none
 *
 * Notes:    none
 *
 **************/
int main(int argc, const char **argv)
{
    struct swi_osipcapi ipc;
    char ipcbuffer[3000];
    size_t length;
    int i;

    /* create the main sdk process */
    swi_osapisdkprocesscreate("./sdkprocess");
    
    /* create an IPC socket, number 9 chosen arbitrarily */
    if (!swi_osapiipcopen(&ipc, 9))
    {
        fprintf(stderr, "API test: swi_osapiipcopen failed\n");
        exit(1);
    }

    /* loop for a while, writing IPC message and getting rsponse */
    
    for ( i = 0; i < 20; i++)
    {
        
        length = 2000;
        if (swi_osapiipcwrite(&ipc, ipcbuffer, length))
                fprintf(stderr, "API test: swi_osapiipcwrite OK\n");        
        else
                fprintf(stderr, "API test: swi_osapiipcwrite failed\n");        

        length = sizeof(ipcbuffer);
        if (swi_osapiipcread(&ipc, ipcbuffer, &length, 10000))
        {
            if (length > 0)
                fprintf(stderr,
                        "API test: swi_osapiipcread received %ld bytes\n",
                        length);
            else
                fprintf(stderr, "API test: swi_osapiipcread timed out\n");
        }
        else
            fprintf(stderr, "API test: swi_osapiipcread serious error\n");        
    }

    return 0;
}

/* 
 * $Log: swi_osapisidetest.c,v $
 */
