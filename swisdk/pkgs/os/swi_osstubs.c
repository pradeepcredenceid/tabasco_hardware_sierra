/*************
 *
 * $Id: swi_osstubs.c,v 1.2 2008/10/24 20:59:35 blee Exp $
 *
 * Filename: swi_ossdk.c
 *
 * Purpose: Wrap OS functions for SDK-side (SDK process)
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* include files */
#include "../aa/aaglobal.h"    /* system wide typedefs */
#include "swi_ossdk.h"

#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>


/*************
 *
 * Name: swi_ossdklog
 *
 * Purpose:  Write to the system log /var/log/user.log
 *           
 * Params:   errorp  - string describing problem
 *
 * Return:   No value
 *
 * Abort:    none
 *
 * Notes:    Don't overuse or repeatedly call this function for the same error;
 *           i.e., for every retry
 *
 **************/
void swi_ossdklog(const char *errorp)
{
    printf( "%s\n", errorp );
}

/*************
 *
 * Name: swi_ossdkaborterror
 *
 * Purpose:  Abort the process, write to log, show low-level error
 *           
 * Params:   reasonp - string describing problem
 *           llerrno - low-level error number (from errno)
 *
 * Return:   No value
 *
 * Abort:    none
 *
 * Notes:    none
 *
 **************/

void swi_ossdkaborterror(const char *errorp, int llerrno)
{
    printf( "%s\n", errorp );
    
    exit(1);
}

/*************
 *
 * Name: swi_ossdkabort
 *
 * Purpose:  Abort the process, write to log /var/log/user.log
 *           
 * Params:   reasonp - string describing problem
 *
 * Return:   No value
 *
 * Abort:    none
 *
 * Notes:    none
 *
 **************/

void swi_ossdkabort(const char *errorp)
{
    swi_ossdkaborterror(errorp, 0);
}

/*************
 *
 * Name: swi_ossdkmemory
 *
 * Purpose: Permanently allocate memory
 *           
 * Params:  size    - size of memory to allocate
 *
 * Return:  Pointer to memory, aligned to sizeof(void *) or better
 *
 * Abort:   Out of memory, can't allocate
 *
 * Notes:   none
 *
 **************/
void *swi_ossdkmemory(swi_size_t size)
{
    void *memp;
    
    memp = (void *)malloc(size);
    
    if (memp)
        return memp;
    
    swi_ossdkabort("out of memory");            /* doesn't return */
    return (void *)NULL;                    /* keep compiler happy */
}

/*************
 *
 * Name: swi_ossdksemaphoreinit
 *
 * Purpose: Initialize the semaphore structure before it is used
 *           
 * Params:  sp      - pointer to semaphore structure
 *
 * Return:  No value
 *
 * Abort:   Semaphore is already in use, out of memory
 *
 * Notes:   Must be called before any threads use the structure.
 *
 **************/
void swi_ossdksemaphoreinit(struct swi_ossemaphore *sp)
{
        
}

/*************
 *
 * Name: swi_ossdksemaphorelock
 *
 * Purpose: Obtain exclusive access to the semaphore, waiting if necessary
 *           * Params:  sp      - pointer to semaphore structure
 *
 * Return:  No value
 *
 * Abort:   Semaphore not initialized or locked by same thread
 *
 * Notes:   swi_ossdksemaphoreinit must have been called first
 *
 **************/
void swi_ossdksemaphorelock(struct swi_ossemaphore *sp)
{

}

/*************
 *
 * Name: swi_ossdksemaphoreunlock
 *
 * Purpose: Release exclusive access to the semaphore
 *           
 * Params:  sp      - pointer to semaphore structure
 *
 * Return:  No value
 *
 * Abort:   semaphore not initialized or not locked by same thread
 *
 * Notes:   swi_ossdksemaphoreinit must have been called first
 *
 *           Semaphore must have been locked by this thread
 *
 **************/
void swi_ossdksemaphoreunlock(struct swi_ossemaphore *sp)
{
    
}

/*************
 *
 * Name: swi_ossdkwaiterinit
 *
 * Purpose: Initalize the waiter structure before it is used
 *           
 * Params:  wp      - pointer to waiter structure
 *
 * Return:  No value
 *
 * Abort:   Wait structure already in use, out of memory
 *
 * Notes:    Must be called before any threads use the structure.
 *
 **************/
void swi_ossdkwaiterinit(struct swi_oswaiter *wp)
{    

}

/*************
 *
 * Name: swi_ossdkwaiterwait
 *
 * Purpose:  Wait for another thread to wake us up
 *           
 * Params:   wp      - pointer to waiter structure
 *           timeout - timeout in ms (0 to wait forever)
 *
 * Return:   TRUE if swi_ossdkwaiterwake may have been called, FALSE if timed
 *           out and swi_ossdkwaiterwake was not called
 *
 * Abort:    Corrupted structure, not initialized
 *
 * Notes:    Spurious wakeups are expected; the thread is guaranteed to wake 
 *           up at least once after a swi_ossdkwaiterwake call.
 *
 *           swi_ossdkwaiterinit must have been called first
 *
 **************/
swi_bool swi_ossdkwaiterwait(struct swi_oswaiter *wp, swi_uint32 timeout)
{
    return TRUE;
}

/*************
 *
 * Name: swi_ossdkwaiterwake
 *
 * Purpose: Wake up a waiting thread
 *           
 * Params:  wp      - pointer to waiter structure
 *
 * Return:  No value
 *
 * Abort:   Corrupted structure, not initialized
 *
 * Notes:   Spurious wakeups are expected; the waiting thread is guaranteed
 *          to wake up at least once after a swi_ossdkwaiterwake call. Wakes 
 *          are not stacked; multiple wakes in quick succession may only 
 *          result in the waiting thread waking up once.
 *
 *          swi_ossdkwaiterinit must have been called first
 *
 **************/
void swi_ossdkwaiterwake(struct swi_oswaiter *wp)
{
        
}


/*************
 *
 * Name: swi_ossdkipcopen
 *
 * Purpose: Open an inter-process communication channel on the SDK side to
 *          communicate with the application process or processes
 *
 * Params:  ip       - pointer to IPC structure
 *          ipcindex - channel number
 *
 * Return:  No value
 *
 * Abort:   No permission, no kernel memory, called twice with same channel
 *          number
 *
 * Notes:   none
 *
 **************/
void swi_ossdkipcopen(struct swi_osipc *ip, swi_size_t ipcindex)
{

}

/*************
 *
 * Name: swi_ossdkipcwrite
 *
 * Purpose:  Write a message to an IPC channel
 *           
 * Params:   ip      - pointer to IPC structure
 *           bufp    - pointer to buffer to write
 *           length  - size of buffer to write (in bytes)
 *
 * Return:   TRUE if OK, false if channel doesn't exist (e.g., if application
 *           process no longer exists); discard msg and continue
 *
 * Abort:    Unexpected Linux error
 *
 * Notes:    swi_ossdkipcapiopen or swi_ossdkipcsdkopen must have been 
 *           called first
 *
 **************/
swi_bool swi_ossdkipcwrite(struct swi_osipc *ip, 
                           const void *bufp, 
                           swi_size_t length)
{
    return TRUE;
}

/*************
 *
 * Name: swi_ossdkipcread
 *
 * Purpose: Read a message from an IPC channel
 *           
 * Params:  ip      - pointer to IPC structure
 *          bufp    - pointer to buffer in which to read
 *          lengthp - pointer to size of read buffer
 *          timeout - timeout in ms (0 to wait forever)
 *
 * Return:  TRUE if OK, false if timeout occurs.
 *
 * Abort:   none
 *
 * Notes:   The number of bytes read is returned in lengthp, which is zero
 *          if the timeout occurred before a message arrived
 *
 *          There is no way to determine if the application process is running
 *          with this function. Just waits if not.
 *
 *          swi_ossdkipcapiopen or swi_ossdkipcsdkopen must have been called 
 *          first
 *
 **************/
swi_bool swi_ossdkipcread(struct swi_osipc *ip, 
                          void *bufp, 
                          swi_size_t *lengthp, 
                          swi_uint32 timeout)
{
    return TRUE;
}

/*************
 *
 * Name: swi_ossdkusbscan
 *
 * Purpose: Scan the system, updating the list of endpoints to ttyUSB
 *          mappings in the swi_osusb structure
 *           
 * Params:  up  - pointer to usb structure
 *
 * Return:  No value
 *
 * Abort:   Linux problem - corrupt or no memory, etc.
 *
 * Notes:   Should be called periodically to update structure (from a 
 *          thread)
 *
 *          swi_ossdkusbinit must have been called first
 *
 **************/
void swi_ossdkusbscan(struct swi_osusb *up)
{

}

/*************
 *
 * Name: swi_ossdkusbinit
 *
 * Purpose: Initialize the USB structure before it is used
 *           
 * Params:  up  - pointer to USB structure
 *          vendoridlistp - pointer to NULL-terminated list of vendor IDs
 *
 * Return:  No value
 *
 * Abort:   Linux problem - corrupt or no memory, etc.
 *
 * Notes:   Must be called before any threads use the structure.
 *
 **************/
void swi_ossdkusbinit(struct swi_osusb *up, const swi_uint32 *vendoridlistp)
{
    
}

/*************
 *
 * Name: swi_ossdkusbproduct
 *
 * Purpose: Get the product IDs of the USB device, if it exists
 *           
 * Params:  up         - pointer to usb structure
 *          vendoridp  - pointer to location in which to store vendor ID
 *          productidp - pointer to location in which to store product ID
 *
 * Return:  TRUE if product exists, false if not
 *
 * Abort:   Linux problem - corrupt or no memory, etc.
 *
 * Notes:   swi_ossdkusbinit must have been called first
 *
 **************/
swi_bool swi_ossdkusbproduct(struct swi_osusb *up, 
                             swi_uint32 *vendoridp, 
                             swi_uint32 *productidp)
{
    *vendoridp = 0x1199;
    *productidp = 0x6812;
    
    return *vendoridp != 0 && *productidp != 0;
}

/*************
 *
 * Name: swi_ossdkusbendpoint
 *
 * Purpose: Get the OS text name of an endpoint (for applications doing I/O)
 *           
 * Params:  up           - pointer to usb structure
 *          endpoint     - endpoint ID
 *          readnotwrite - TRUE if read endpoint, FALSE if write outpoint
 *          namep        - pointer to location in which to store the name
 *          size         - size of name storage (should be PATH_MAX)
 *
 * Return:  TRUE if endpoint exist and name was copied, FALSE if not
 *
 * Abort:   Huge endpoint number
 *          Linux problem - corrupt or no memory, etc.
 *
 * Notes:    swi_ossdkusbinit must have been called first
 *
 *           Must be called from a thread that can access the usb structure
 *
 **************/
swi_bool swi_ossdkusbendpoint(struct swi_osusb *up, 
                              swi_uint32 endpoint,
                              swi_bool readnotwrite, 
                              char *namep, 
                              swi_size_t size)
{
    return TRUE;
}

/*************
 *
 * Name: swi_ossdkusbwait
 *
 * Purpose: Block until the specified USB endpoint exists and is assigned
 *          to a /dev/ttyUSB. Clears any endpoint error condition.
 *           
 * Params:  up           - pointer to usb structure
 *          endpoint     - endpoint ID
 *          readnotwrite - TRUE if read endpoint, FALSE if write endpoint
 *
 * Return:  No value
 *
 * Abort:   Crazy endpoint number
 *          Linux problem - corrupt or no memory, etc.
 *
 * Notes:   swi_ossdkusbinit must have been called first
 *          Somebody must be calling swi_ossdkusbscan
 *          Must be called from a process that can access the usb structure
 *          Only one thread per endpoint can call this function and
 *          the read or write (as appropriate) function.
 *
 **************/
void swi_ossdkusbwait(struct swi_osusb *up, swi_uint32 endpoint,
                                            swi_bool readnotwrite)
{

}

/*************
 *
 * Name: swi_ossdkusbwrite
 *
 * Purpose: Write characters to a USB endpoint
 *           
 * Params:  up       - pointer to usb structure
 *          endpoint - endpoint ID
 *          bufp     - pointer to buffer to write
 *          length   - size of buffer to write (in bytes)
 *
 * Return:  TRUE if OK, FALSE if device no longer exists
 *
 * Abort:   Huge endpoint number
 *
 * Notes:   swi_ossdkusbinit must have been called first
 *          Only one thread may write to each endpoint.
 *
 **************/
swi_bool swi_ossdkusbwrite(struct swi_osusb *up, 
                           swi_uint32 endpoint, 
                           const void *bufp, 
                           swi_size_t length)
{
    return TRUE;
}

/*************
 *
 * Name: swi_ossdkusbread
 *
 * Purpose: Read characters from a USB enpoint
 *           
 * Params: up        - pointer to usb structure
 *         endpoint  - endpoint ID
 *         bufp      - pointer to buffer in which to read
 *         lengthp   - pointer to size of read buffer
 *         timeout   - timeout in ms (0 to wait forever)
 *
 * Return: TRUE if OK, FALSE if device no longer exists
 *
 * Abort:  none
 *
 * Notes:  The number of bytes read is returned in lengthp, which is zero
 *         if the timeout occurred before any bytes arrived
 *
 *         swi_ossdkusbinit must have been called first
 *         Only one thread may read from each endpoint.
 *
 **************/
swi_bool swi_ossdkusbread(struct swi_osusb *up,
                          swi_uint16 endpoint, 
                          void *bufp, 
                          swi_size_t *lengthp, 
                          swi_uint32 timeout)
{
    return TRUE;
}


/* 
 * $Log: swi_osstubs.c,v $
 */
