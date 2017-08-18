/*************
 *
 * $Id: swi_ossdksidetest.c,v 1.6 2008/10/24 20:59:35 blee Exp $
 *
 * Filename: swi_ossdksidetest.c
 *
 * Purpose:  Test program for OS wrapper that emulates sdk process
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* include files */
#include "../aa/aaglobal.h"    /* system wide typedefs */
#include "swi_ossdk.h"         /* package user include file  */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>

/* inter process communication */
local struct swi_osipc ipc;

/* inter-thread  communication */
local struct swi_ossemaphore itc_sem;
local struct swi_oswaiter itc_waitstore;
local struct swi_oswaiter itc_waitread;
local char itc_buffer[3000];
local swi_size_t itc_length;
local swi_bool itc_in_use;

/* usb */
local struct swi_osusb usbs;

static unsigned char hipbuf[] = {
    0x7e,
    0x00,
    0x15, 
    0x03,     /* HIP Loopback request ID */
    0x00,
    0x11,
    0x11,
    0x11,
    0x11,
    0x11,
    0x11,
    0x11,
    0x11,
    0x11,
    0x11,
    0x11,
    0x11,
    0x11,
    0x11,
    0x11,
    0x11,
    0x11,
    0x11,
    0x11,
    0x11,
    0x11,
    0x7e
};

/*************
 *
 * Name: receivethread
 *
 * Purpose: Thread that receives messages.
 *           
 * Params:  No values
 *
 * Return:  No value
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
local void receivethread(void)
{
    char ipcbuffer[3000];
    swi_size_t length;
    char lmsg[128];

    for ( ; ; )
    {
        length = sizeof(ipcbuffer);
        if (swi_ossdkipcread(&ipc, ipcbuffer, &length, 10000))
        {
            if (length > 0)
            {
                snprintf(lmsg, sizeof(lmsg),
                            "swi_ossdkipcread received %lu bytes\n",
                            length);
                            
                swi_ossdklog(lmsg);

                swi_ossdksemaphorelock(&itc_sem);
                itc_length = length;
                memcpy(itc_buffer, ipcbuffer, length);
                itc_in_use = TRUE;    
                swi_ossdksemaphoreunlock(&itc_sem);
                swi_ossdkwaiterwake(&itc_waitread); /* wake up sendthread */
                
                /* wait for transmission by sendthread */            
                for ( ; ; )
                {
                    swi_ossdkwaiterwait(&itc_waitstore, 0);

                    swi_ossdksemaphorelock(&itc_sem);
                    
                    if (!itc_in_use)
                    {
                        swi_ossdksemaphoreunlock(&itc_sem);
                        break;
                    }
                    
                    swi_ossdksemaphoreunlock(&itc_sem);
                }
            }
            else
                swi_ossdklog("swi_ossdkipcread got zero length -- error\n");

        }
        else
            swi_ossdklog("swi_ossdkipcread timed out\n");
    }
        
}

/*************
 *
 * Name: sendthread
 *
 * Purpose: Thread that sends messages back.
 *           
 * Params:  No values
 *
 * Return:  No value
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
local void sendthread(void)
{
    for ( ; ; )
    {
        if (swi_ossdkwaiterwait(&itc_waitread, 9000))
        {
                swi_ossdksemaphorelock(&itc_sem);
                if (itc_in_use)        /* if message from receivethread */
                {
                    if (swi_ossdkipcwrite(&ipc, itc_buffer, itc_length))
                            swi_ossdklog("swi_ossdkipcwrite OK\n");        
                    else
                            swi_ossdklog("swi_ossdkipcwrite failed\n");
                    
                    itc_in_use = FALSE;
                    swi_ossdkwaiterwake(&itc_waitstore); /* wake up Rxthread */
                }
                swi_ossdksemaphoreunlock(&itc_sem);
        }
        else
            swi_ossdklog("send thread periodic timeout\n");        
    }
}

/*************
 *
 * Name: usbscanthread
 *
 * Purpose: Thread that scans usb ports.
 *           
 * Params: No values
 *
 * Return: No value
 *
 * Abort:  none
 *
 * Notes:  none
 *
 **************/
local void usbscanthread(void)
{
    for ( ; ; )
    {
        sleep(1);
        swi_ossdklog("Scanning USB\n");
        swi_ossdkusbscan(&usbs);    /* see if modem is present */
        
    }
    
}

/*************
 *
 * Name: main
 *
 * Purpose: Main entry point for SDK process.
 *           
 * Params:  No values
 *
 * Return:  No value
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
int main(int argc, const char **argv)
{    
    
    swi_uint32 vendors[2];
    char ttydevname[256];
    int i;
    int j;
    swi_uint32 vendorid;
    swi_uint32 productid;
    char modemstatusmsg[256];
    swi_uint32 rdendpoint;
    swi_uint32 wrendpoint;
    char usbtext[1024];
    swi_size_t usbtextlen;
    
    swi_ossdkprocessinit();    /* must be called first thing */    

    /* don't try to write to stdout or stderr -- they've been
     * closed and this process is a detached daemon. Use
     * swi_ossdklog instead, here, and look in /var/log/user.log 
     */
       
    vendors[0] = 0x1199;
    vendors[1] = 0;
    
    swi_ossdkusbinit(&usbs, vendors);    /* init the USB structure */

    /* IPC initialization */
    swi_ossdkipcopen(&ipc, 9);
    
    /* inter-thread initialization */
    swi_ossdksemaphoreinit(&itc_sem);
    swi_ossdkwaiterinit(&itc_waitstore);
    swi_ossdkwaiterinit(&itc_waitread);
    itc_in_use = FALSE;
    
    swi_ossdkthreadcreate(receivethread);
    swi_ossdkthreadcreate(sendthread);
    swi_ossdkthreadcreate(usbscanthread);
    
    /* usb tests */
    if (swi_ossdkusbproduct(&usbs, &vendorid, &productid))
    {
        
        snprintf(modemstatusmsg, sizeof(modemstatusmsg),
                    "USB modem found, vendor %x, product %x, endpoints:\n",
                    vendorid, productid);
        swi_ossdklog(modemstatusmsg);
    }
    else
        swi_ossdklog("USB modem not found");
    
    /* dump out read endpoints */
    for (i = 0; i < SWI_OS_MAXENDPOINTS; i++)
    {
        swi_ossdklog("Scanning USB\n");
        if (swi_ossdkusbendpoint(&usbs, i,
                                TRUE, ttydevname, sizeof(ttydevname)))
        {
            snprintf(modemstatusmsg, sizeof(modemstatusmsg),
                    "    USB read endpoint %d is %s\n", i, ttydevname);
            swi_ossdklog(modemstatusmsg);
        }
    }
    /* dump out write endpoints */
    for (i = 0; i < SWI_OS_MAXENDPOINTS; i++)
    {
        if (swi_ossdkusbendpoint(&usbs, i,
                                FALSE, ttydevname, sizeof(ttydevname)))
        {
            snprintf(modemstatusmsg, sizeof(modemstatusmsg),
                    "    USB write endpoint %d is %s\n", i, ttydevname);
            swi_ossdklog(modemstatusmsg);
        }
    }

    swi_ossdklog("waiting for write endpoint 2");
    swi_ossdkusbwait(&usbs, 2, FALSE);
    swi_ossdklog("write endpoint 2 present");
    
    switch(productid)
    {
        case 0x0018:        /* MC5720  */
        case 0x0218:        /* MC5720  */
        case 0x1B1D:        /* MC5720  */
        case 0x0020:        /* MC5725  */
        case 0x0220:        /* MC5725  */
        case 0x0019:        /* AirCard 595  */
        case 0x0021:        /* AirCard 597E  */
        case 0x0120:        /* USB Dongle 595U  */
            rdendpoint = 2;
            wrendpoint = 2;
            break;
        case 0x6802:        /* MC8755  */
        case 0x6804:        /* MC8755  */
        case 0x6803:        /* MC8765  */
        case 0x6812:        /* MC8775 & AC 875U  */
        case 0x6820:        /* AirCard 875  */
        case 0x6821:        /* AC 875U  */
        case 0x6832:        /* MC8780  */
        case 0x6833:        /* MC8781  */

            rdendpoint = 4;
            wrendpoint = 4;
            break;
        case 0x683B:        /* MC8785   */
        case 0x6859:        /* AirCard 885 E */
            rdendpoint = 5;
            wrendpoint = 4;
            break;
        default:
            swi_ossdkabort("Unrecognized product ID\n");
            
    }
    
    for (i = 0; i < 30; i++)
    {
        if (i == 0)
        {
            usbtext[0] = 'A';
            usbtext[1] = 'T';
            usbtext[2] = 'E';
            usbtext[3] = '0';
            usbtext[4] = 'I';
            usbtext[5] = '0';
            usbtext[6] = 0xd;
            usbtextlen = 7;
        }
        else
        {
            usbtext[0] = 'A';
            usbtext[1] = 'T';
            usbtext[2] = 0xd;
            usbtextlen = 3;
        }
        
//        if (swi_ossdkusbwrite(&usbs, wrendpoint, usbtext, usbtextlen))
        if (swi_ossdkusbwrite(&usbs, wrendpoint, hipbuf, sizeof(hipbuf)))
        {
            swi_ossdklog("Echo Request Written\n");
            //swi_ossdklog("AT command written to USB\n");
        }
        else
        {
            swi_ossdklog("AT command write failed to USB\n");        
        }
        
        usbtextlen = sizeof(usbtext) - 1;
        if (swi_ossdkusbread(&usbs, rdendpoint, usbtext, &usbtextlen, 1000))
        {
            if (usbtextlen == 0)
            {
                swi_ossdklog("USB read timeout\n");
            }
            else
            {
                usbtext[usbtextlen] = '\0';
                
                for (j = 0; j < usbtextlen; j++)
                {
                    if (usbtext[j] < ' ')
                        usbtext[j] = '.';
                }
                
                snprintf(modemstatusmsg, sizeof(modemstatusmsg),
                    "USB read succeeded, got '%s'\n", usbtext);
                swi_ossdklog(modemstatusmsg);
                sleep(1);
            }
        }
        else
        {
            // swi_ossdklog("USB read failed\n");
            // sleep(1);
            //
            swi_ossdklog( "waiting for endpoint\n" );
            swi_ossdkusbwait( &usbs, rdendpoint, TRUE );
        }
    }



    /* Unlike real SDK process, we run for a while and exit to aid testing;
     * the real SDK process never exits. This creates a window in the
     * test program where, if the test is started repeatedly, we might exit
     * in the middle of the test, resulting in timeout on the API side.
     * Remember to look at the log to see what happened. 
     */
    swi_ossdklog("completed, exiting ------------------------------------\n");        
    return 0;
}

/* 
 * $Log: swi_ossdksidetest.c,v $
 */
