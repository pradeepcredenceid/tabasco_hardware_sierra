/*************
 *
 * $Id: swi_osapi.c,v 1.9 2010/06/09 23:50:32 epasheva Exp $
 *
 * Filename: swi_osapi.c
 *
 * Purpose:  Wrap OS functions for API-Side (application processes)
 *
 * Copyright: © 2008 Sierra Wireless, Inc. all rights reserved
 *
 * The SDK process creates UNIX IPC sockets. To view them, type:
 *      netstat -a --protocol=unix|grep SWI
 *
 **************/

#define SWI_SOCKET_REAP_MS 500        /* socket reap time after exit */
#define SWI_PROCESS_STARTUP_MS 1000   /* process startup time */
#define SWI_RETRY_MS 100              /* milliseconds */
#define SWI_MESSAGE_BYTES 5000
#define SWI_STACK_SIZE    (1024UL * 1024UL)    /* stack size for threads */

/* RILSTART */
#define SWI_OSWAITSTATE_SLEEP 0
#define SWI_OSWAITSTATE_AWAKE 1
#define SWI_OSWAITSTATE_ALERT 2
/* RILSTOP */

/* include files */
#include "swi_osapi.h"        /* packages user include file  */
#include <string.h>

#ifndef ANDROID_LOGGING
/* All platforms except Android */
#include <syslog.h>

local void swi_osapierrordefault(const char *, int);    /* fwd reference */

static void (*swi_osapierrorfnp)(const char *, int) = swi_osapierrordefault;
#else
/* Android logging */
#define LOG_TAG "RILQAPI"
#include "swiril_log.h"

local void swi_osapilogerror(const char *errorp, int llerrno);

static void (*swi_osapierrorfnp)(const char *, int) = swi_osapilogerror;
#endif

/* RILSTART */
/* PID of the SDK daemon */
pid_t ossdkpid;
/* RILSTOP */

/*************
 *
 * Name: swi_osapierrorhandlerset
 *
 * Purpose: Change handler for API-side OS error.
 *           
 * Params:  handlerp - function called as:
 *
 *              (*handlerp)(const char *errorp, int llerrno)
 *
 *          where:
 *              errorp - high-level explanation of failure
 *              llerrno - low-level error number (from errno)
 *
 * Return:  No value
 *
 * Abort:   none
 *
 * Notes:   The default handler writes to stderr
 *
 **************/ 

void swi_osapierrorhandlerset(void (*handlerp)(const char *, int))
{
    swi_osapierrorfnp = handlerp;
}


/*************
 *
 * Name: swi_osapilogerror
 *
 * Purpose:  Write to log, show low-level error
 *           
 * Params:   errorp  - string describing problem
 *           llerrno - low-level error number (from errno)
 *
 * Return:   No value
 *
 * Abort:    none
 *
 * Notes:    Don't overuse or repeatedly call this function for the same 
 *           error; i.e., for every retry
 *           Needed by the test application.
 *
 **************/
local void swi_osapilogerror(const char *errorp, int llerrno)
{
#ifdef ANDROID_LOGGING
    /* Android logging */
    if (llerrno != 0)
        LOGE("%5i : %s", llerrno, errorp);
    else
        LOGI("%s", errorp);
#else
    /* All other platforms */
    if (llerrno != 0)
    {
        errno = llerrno;
        syslog(LOG_USER | LOG_ERR, "%s: %m", errorp);
    }
    else
        syslog(LOG_USER | LOG_ERR, "%s", errorp);
#endif
}

/*************
 *
 * Name: swi_osapilog
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
 *           Needed by the test application.
 *
 **************/
void swi_osapilog(const char *errorp)
{
    swi_osapilogerror(errorp, 0);
}

/*************
 *
 * Name: swi_osapiaborterror
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
 * Notes:    Needed by the test application.
 *
 **************/

void swi_osapiaborterror(const char *errorp, int llerrno)
{
    swi_osapilogerror(errorp, llerrno);
    
    abort();
}

/*************
 *
 * Name: swi_osapiabort
 *
 * Purpose:  Abort the process, write to log /var/log/user.log
 *           
 * Params:   reasonp - string describing problem
 *
 * Return:   No value
 *
 * Abort:    none
 *
 * Notes:    Needed by the test application.
 *
 **************/

void swi_osapiabort(const char *errorp)
{
    swi_osapiaborterror(errorp, 0);
}




/*************
 *
 * Name: swi_osapierrordefault
 *
 * Purpose: Default reporter of an API-side error.
 *           
 * Params:  errorp - high-level explanation of failure
 *
 *          llerrno - low-level error number (from errno)
 *
 * Return:  No value
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/ 

local void swi_osapierrordefault(const char *errorp, int llerrno)
{
    char msg[128];
    
    snprintf(msg, sizeof(msg), "SWI API: %s", errorp);
    
    if (llerrno != 0)
    {
        errno = llerrno;
        perror(msg);
    }
    else
        fprintf(stderr, "%s\n", msg);        
}

/*************
 *
 * Name: swi_osapierror
 *
 * Purpose: Report an API-side error.
 *           
 * Params:  errorp - high-level explanation of failure
 *          llerrno - low-level error number (from errno)
 *
 * Return:  No value
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/

local void swi_osapierror(const char *errorp, int llerrno)
{
    if (swi_osapierrorfnp != NULL)
        (*swi_osapierrorfnp)(errorp, llerrno);
}

/*************
 *
 * Name: swi_osapidelay
 *
 * Purpose: Block for a specified time.
 *           
 * Params:  timeout - timeout in ms
 *
 * Return:  No value
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/

local void swi_osapidelay(swi_uint32 timeout)
{
    struct timespec timev;
    struct timespec timer;
    int rv;

    timer.tv_sec = timeout / 1000UL;
    timer.tv_nsec = (timeout * 1000000UL) % 1000000000UL;
    
    do
    {
        timev = timer;        /* remaining time */
        rv = nanosleep(&timev, &timer);
        
    } while (rv < 0 && errno == EINTR);
}


/*************
 *
 * Name: swi_osapisdkrunning
 *
 * Purpose: Determine if SDK process is already running
 *           
 * Params:  runverifytime  - time to verify that process is running 
 *                           and hasn't just ended
 *          stopverifytime - time to verify that process is stopped and
 *                           hasn't just been started
 *
 * Return:  TRUE if already running
 *
 * Abort:   no memory
 *
 * Notes:   Errs in the direction of saying process is NOT running as
 *          a newly-created SDK process also checks to see if it is a
 *          duplicate.
 *
 **************/
local swi_bool swi_osapisdkrunning(swi_uint32 runverifytime, 
                                   swi_uint32 stopverifytime)
{
    struct sockaddr_un run_sa;
    int rv;
    int delay;
    int sock;
    swi_bool isrunning;
    
    run_sa.sun_family = AF_UNIX;
    memset(run_sa.sun_path, 0, sizeof(run_sa.sun_path)); /* must be zeroed */
    snprintf(&run_sa.sun_path[1], sizeof(run_sa.sun_path) - 1,
        "SWI_SDK_SOCKET_MUTEX");  /* use abstract namespace */
    
    sock = socket(PF_LOCAL, SOCK_DGRAM, 0);    /* must be SOCK_DGRAM */
    if (sock < 0)
    {
        swi_osapierror("SDK run check (s) failed", errno);
        return FALSE;
    }

    delay = 0;
    for ( ; ; )
    {
        if (connect(sock, (struct sockaddr *)&run_sa, 
                            sizeof(run_sa)) < 0) /* probably not running */
        {
            if (errno == EINTR)
            {
                /* interrupted - repeat */
            }
            else if (delay >= stopverifytime)    /* if not running */
            {
                isrunning = FALSE;
                break;
            }
            else
            {
                swi_osapidelay(SWI_RETRY_MS);
                delay += SWI_RETRY_MS;
            }
        }
        else        /* probably running */
        {
            run_sa.sun_family = AF_UNSPEC;
            connect(sock, (struct sockaddr *)&run_sa,
                    sizeof(run_sa));    /* disassociate */
            run_sa.sun_family = AF_UNIX;
        
            if (delay >= runverifytime) /* bind timout is socket reap time */
            {
                isrunning = TRUE;
                break;
            }
            
            swi_osapidelay(SWI_RETRY_MS);
            delay += SWI_RETRY_MS;
        }
    }

    do
    {
        rv = close(sock);
    } while (rv < 0 && errno == EINTR);

    return isrunning;
}


/*************
 *
 * Name: swi_osapisdkprocesscreate
 *
 * Purpose: Create the SDK process, if it hasn't been created already,
 *          otherwise do nothing.
 *           
 * Params:  pathp     - pointer to path name of SDK executable
 *
 * Return:  TRUE if process created or was already running. FALSE if
 *          fatal Linux error occurred.
 *
 * Abort:   none
 *
 * Notes:   This function should not be called more than once by a single
 *          process. It blocks for awhile when the SDK process is already
 *          running to make sure it wasn't just killed and to see it
 *          started up.
 *
 **************/

swi_bool swi_osapisdkprocesscreate(const char *pathp)
{
    char *argp[2];
    char *envp[1];
    pid_t pid;

    argp[0] = (char *)pathp;
    argp[1] = NULL;
    envp[0] = NULL;
    
    if (swi_osapisdkrunning(SWI_SOCKET_REAP_MS, 0)) /* if already running */
    {
        return TRUE;
    }
    /* RILSTART */
    /* Initialize the SDK's PID to non-existent */
    ossdkpid = 0;
    /* RILSTOP */

    /* start SDK process */
    pid = fork();
    
    if (pid < 0)
    {
        swi_osapierror("process creation failed", errno);
        return FALSE;
    }
    else if (pid == 0)    /* in child process */
    {
        execve(pathp, argp, envp);        /* launch executable */
        swi_osapierror("SDK launch failed", errno);
        setsid();                         /* prevent zombification */
        exit(1);                          /* terminate child */
    }
    
    /* still in parent process */
    
    if (!swi_osapisdkrunning(SWI_PROCESS_STARTUP_MS,
                            SWI_PROCESS_STARTUP_MS))
    /* if not now running, giving time to open sockets */
    {
        swi_osapierror("SDK process startup failed", 0);
        return FALSE;
    }    
    /* RILSTART */
    ossdkpid = pid;
    /* RILSTOP */
    return TRUE;
}

/*************
 *
 * Name: swi_osapiipcformlocalsocketname
 *
 * Purpose: Form local socket name
 *           
 * Params:  str         - Point to buffer that save socket name. 
 *          size        - Size of the buffer pointer by str
 *          ipcindex    - IPC channel No.
 *          ipcnamep    - Pointer to channel name.
 *
 * Return:  None.
 *
 * Abort:   None.
 *
 * Notes:   The implementation of this function must match the implementation
 *          of function swi_ossdkipcformremotesocketname().
 *
 **************/
local void swi_osapiipcformlocalsocketname(char *str, 
                                      size_t size, 
                                      size_t ipcindex,
                                      swi_uint8 * ipcnamep)
{
    memset(str, 0, size); /* must be zeroed */

    /* use abstract namespace */
    snprintf(str+1, size - 1, "SWI_API_%s_SOCKET_%u", ipcnamep, ipcindex);
}
/* RILSTART */
/*************
 *
 * Name: swi_osapistopsdk
 *
 * Purpose: Stop the SDK daemon if it is running
 *           
 * Params:  none
 *
 * Return:  None.
 *
 * Abort:   None.
 *
 * Notes:   None
 *
 **************/
global void swi_osapistopsdk( void )
{
    /* If it's not running, then don't try to kill it */
    if (!swi_osapisdkrunning(SWI_SOCKET_REAP_MS, 0)) 
    {
        return;
    }

    if( ossdkpid != 0 ) {
        /* Else it is running so stop it */
        if( kill( ossdkpid, SIGTERM ) < 0 ) 
        {
            swi_osapilogerror("SDK shutdown failed", errno);
        }
    }
}
/* RILSTOP */
/*************
 *
 * Name: swi_osapiipcformremotesocketname
 *
 * Purpose: Form remote socket name
 *           
 * Params:  str         - Point to buffer that save socket name. 
 *          size        - Size of the buffer pointer by str
 *          ipcindex    - IPC channel No.
 *          ipcnamep    - Pointer to channel name.
 *
 * Return:  None.
 *
 * Abort:   None.
 *
 * Notes:   The implementation of this function must match the implementation
 *          of function swi_ossdkipcformlocalsocketname().
 *
 **************/
local void swi_osapiipcformremotesocketname(char *str, 
                                      size_t size, 
                                      size_t ipcindex,
                                      swi_uint8 * ipcnamep)
{
    memset(str, 0, size); /* must be zeroed */

    /* use abstract namespace */    	
    snprintf(str+1, size - 1, "SWI_SDK_%s_SOCKET_%u", ipcnamep, ipcindex);    
}

/*************
 *
 * Name: swi_osapiipcformlocalsocketaddr
 *
 * Purpose: Form local socket address
 *           
 * Params:  sap         - Pointer to structure that save socket address. 
 *          ipcindex    - IPC channel No.
 *          ipcnamep    - Pointer to channel name.
 *
 * Return:  None.
 *
 * Abort:   None.
 *
 **************/
local void swi_osapiipcformlocalsocketaddr(struct sockaddr_un *sap, 
                                         size_t ipcindex,
                                         swi_uint8 * ipcnamep)
{
    sap->sun_family = AF_UNIX;
    swi_osapiipcformlocalsocketname(sap->sun_path, sizeof(sap->sun_path), 
            ipcindex, ipcnamep);
}

/*************
 *
 * Name: swi_osapiipcformremotesocketaddr
 *
 * Purpose: Form remote socket address
 *           
 * Params:  sap         - Pointer to structure that save socket address. 
 *          ipcindex    - IPC channel No.
 *          ipcnamep    - Pointer to channel name.
 *
 * Return:  None.
 *
 * Abort:   None.
 *
 **************/
local void swi_osapiipcformremotesocketaddr(struct sockaddr_un *sap, 
                                         size_t ipcindex,
                                         swi_uint8 * ipcnamep)
{
    sap->sun_family = AF_UNIX;
    swi_osapiipcformremotesocketname(sap->sun_path, sizeof(sap->sun_path), 
            ipcindex, ipcnamep);
}

/*************
 *
 * Name: swi_osapiipcsetdest
 *
 * Purpose: Set destination socket
 *           
 * Params:  ip          - Pointer to structure that saves destination socket 
 *                        address to be set. 
 *          ipcindex    - IPC channel No. to set.
 *          ipcnamep    - Pointer to channel name.
 *
 * Return:  None.
 *
 * Abort:   None.
 *
 **************/
void swi_osapiipcsetdest(struct swi_osipcapi *ip, 
                         swi_size_t ipcindex, 
                         swi_uint8 * ipcnamep)
{
    swi_osapiipcformremotesocketaddr(&ip->osipcdest, ipcindex, ipcnamep);    
}


/*************
 *
 * Name: swi_osapiipcopen
 *
 * Purpose: Initialize an inter-process communication channel on the API side
 *          for communication with the SDK process
 *           
 * Params:  ip       - pointer to IPC structure
 *          ipcindex - channel number
 *          ipcnamep - pointer to channel name
 *
 * Return:  TRUE if OK, false if channel already exists or fatal Linux error.
 *
 * Abort:   No permission
 *
 * Notes:   swi_osapisdkprocesscreate must have been called and returned first
 *          Writes error message using error handler
 *
 **************/

swi_bool swi_osapiipcopen(  struct swi_osipcapi *ip, 
                            size_t ipcindex,
                            swi_uint8 *ipcnamep)
{
    struct sockaddr_un loc_sa;
    int socketsize;
    int delay;
    
    /* build our local address */
    swi_osapiipcformlocalsocketaddr(&loc_sa, ipcindex, ipcnamep);
    
    /* build remote address in IPC structure */
    swi_osapiipcsetdest(ip, ipcindex, ipcnamep);
    
    ip->osipcsocket = socket(PF_LOCAL, SOCK_DGRAM, 0);
    
    if (ip->osipcsocket < 0)
    {
        swi_osapierror("IPC open (s) failed", errno);
        return FALSE;
    }
    

    delay = 0;
    for ( ; ; )
    {
        /* bind local address */
        if (bind(ip->osipcsocket, (struct sockaddr *)&loc_sa, 
                                            sizeof(loc_sa)) >= 0)
            break;
        
        if (delay >= SWI_SOCKET_REAP_MS ||
                            errno != EADDRINUSE)
        {
            swi_osapierror("IPC open (b) failed", errno);
            return FALSE;
        }
            
        /* linux takes a while to reap sockets so retry after SWI_RETRY_MS */
        swi_osapidelay(SWI_RETRY_MS);
        delay += SWI_RETRY_MS;
    }
    
    socketsize = SWI_MESSAGE_BYTES;
    if (setsockopt(ip->osipcsocket, SOL_SOCKET, SO_SNDBUF, &socketsize,
                                sizeof(socketsize)) < 0)
    {
        swi_osapierror("IPC open (o) failed", errno);
        return FALSE;
    }
    
    return TRUE;
}

/*************
 *
 * Name: swi_osapiipcclose
 *
 * Purpose: Close socket
 *           
 * Params:  picp        - Pointer to structure that saves destination socket 
 *                        address to be closed. 
 *
 * Return:  TRUE if socket is closed succefully; otherwise return FALSE;
 *
 * Abort:   None.
 *
 **************/
swi_bool swi_osapiipcclose(struct swi_osipcapi *picp)
{
    /* Close socket */
    return (close(picp->osipcsocket) == 0);
}

/*************
 *
 * Name: swi_osapiipcwrite
 *
 * Purpose: Write a message to an IPC channel
 *           
 * Params:  ip      - pointer to IPC structure
 *          bufp    - pointer to buffer to write
 *          length  - size of buffer to write (in bytes)
 *
 * Return:  TRUE if OK, false if SDK process doesn't exist or hasn't opened
 *          its IPC channel yet.
 *
 * Abort:   none
 *
 * Notes:   swi_osipcapiopen must have been called first
 *          Writes error message using error handler
 *          Retries the first time to give the SDK process time to start
 *
 **************/
swi_bool swi_osapiipcwrite(struct swi_osipcapi *ip, 
                           const void *bufp, 
                           size_t length)
{
    int delay;

    delay = 0;    
    for ( ; ; )
    {
        if (sendto(ip->osipcsocket, bufp, length, MSG_EOR | MSG_NOSIGNAL,
                        (struct sockaddr *)&ip->osipcdest,
                        sizeof(ip->osipcdest)) >= 0)
            break;
        
        if (errno == EINTR)
        {
            /* signal - just retry */
        }
        else if (errno != ECONNREFUSED)
        {
            swi_osapierror("IPC write failed", errno);
            return FALSE;
        }
        else if (delay > 0)        /* if tried once before */
        {
            return FALSE;
        }
        else
        {
            swi_osapidelay(SWI_RETRY_MS);
            delay += SWI_RETRY_MS;
        }
    }
    
    return TRUE;
}

/*************
 *
 * Name: swi_osapiipcread
 *
 * Purpose: Read a message from an IPC channel
 *           
 * Params:  ip      - pointer to IPC structure
 *          bufp    - pointer to buffer in which to read
 *          lengthp - pointer to size of read buffer
 *          timeout - timeout in ms (0 to wait forever)
 *
 * Return:  TRUE if OK, false if serious error.
 *
 * Abort:   none
 *
 * Notes:   The number of bytes read is returned in lengthp, which is zero
 *          if the timeout occurred before a message arrived
 *
 *          There is no way to determine if the SDK process is running
 *          with this function. Just waits if not.
 *
 *          swi_osipcapiopen must have been called first
 *
 *          Writes error message using error handler
 *
 **************/
swi_bool swi_osapiipcread(struct swi_osipcapi *ip, void *bufp, size_t *lengthp, swi_uint32 timeout)
{
    ssize_t rv;
    struct timeval times;
    struct timeval *timesp;
    fd_set fdmask;
    
    if (timeout != 0)
    {
        timesp = &times;
        times.tv_sec = timeout / 1000UL;
        times.tv_usec = (timeout * 1000UL) % 1000000UL;
    }
    else
        timesp = NULL;
    
    /* wait for message */
    
    FD_ZERO(&fdmask);
    FD_SET(ip->osipcsocket, &fdmask); /* set file descriptor mask bit */
    do {
        rv = select(ip->osipcsocket + 1, &fdmask, NULL, &fdmask, timesp);
    } while (rv < 0 && errno == EINTR);
    
    if (rv < 0)
    {
        swi_osapierror("IPC read (s) failed", errno);
        return FALSE;
    }
    else if (rv == 0 && timeout > 0)    /* timeout */
    {
        *lengthp = 0;
        return TRUE;
    }
    /* else rv is 1 */
    
    /* read message */
    do
    {
        rv = recv(ip->osipcsocket, bufp, *lengthp, MSG_NOSIGNAL);
    } while (rv < 0 && errno == EINTR);
    
    if (rv < 0)
    {
        *lengthp = 0;
        swi_osapierror("IPC read (r) failed", errno);
        return FALSE;
    }
        
    *lengthp = (size_t)rv;
    return TRUE;
}

/*************
 *
 * Name: swi_osapithreadcreate
 *
 * Purpose: Create a thread in the current process
 *           
 * Params:  fnp   - pointer to function that comprises the thread
 *
 * Return:  No value
 *
 * Abort:   Out of memory; can't create.
 *
 * Notes:   None
 *
 **************/
void swi_osapithreadcreate(void (*fnp)(void))
{
    pthread_attr_t attr;
    pthread_t tid;
    void *stackloc;

    if ( pthread_attr_init(&attr) < 0)
        swi_osapiaborterror("thread create (i) failed", errno);

    stackloc = malloc(SWI_STACK_SIZE);
    if (!stackloc)
        swi_osapiabort("thread create (m) failed: No memory");
    
    if (pthread_attr_setstack(&attr, stackloc, SWI_STACK_SIZE) < 0)
        swi_osapiaborterror("thread create (s) failed", errno);
        
    if ( pthread_attr_setdetachstate(&attr,
                        PTHREAD_CREATE_DETACHED) < 0)
        swi_osapiaborterror("thread create (d) failed", errno);
#ifndef ANDROID_NO_STACKDUMP
    if ( pthread_attr_setinheritsched(&attr,
                        PTHREAD_INHERIT_SCHED) < 0)
        swi_osapiaborterror("thread create (v) failed", errno);
#endif
    if (pthread_create(&tid, &attr, (void *(*)(void *))fnp, NULL) < 0)
        swi_osapiaborterror("thread create (c) failed", errno);
}
/* RILSTART */
/*************
 *
 * Name: swi_osapiwaiterinit
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
void swi_osapiwaiterinit(struct swi_osapiwaiter *wp)
{    
    if (pthread_mutex_init(&wp->swi_oswaitmutex, NULL) < 0)
        swi_osapiaborterror("waiter initialze (m) failed", errno);

    if (pthread_cond_init(&wp->swi_oswaitcondition, NULL) < 0)
        swi_osapiaborterror("waitier initialize (c) failed", errno);

    wp->swi_oswaitsleepstate = SWI_OSWAITSTATE_AWAKE;
}

/*************
 *
 * Name: swi_osapiwaiterwait
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
 *           swi_osapiwaiterinit must have been called first
 *
 **************/
swi_bool swi_osapiwaiterwait(struct swi_osapiwaiter *wp, swi_uint32 timeout)
{
    swi_bool waswoken;
    struct timespec times;
    
    /* clock_gettime requires the -lrt compiler option (real-time library) */
    clock_gettime(CLOCK_REALTIME, &times);
    
    /* Only do this if a non-zero timeout was specified */
    if( timeout )
    {
        times.tv_sec += timeout / 1000;
        times.tv_nsec += (timeout % 1000) * 1000000UL; /* milliseconds to ns */
        times.tv_sec += times.tv_nsec / 1000000000UL;
        times.tv_nsec %= 1000000000UL; /* nsec must be less than 1E9 */
    }
    
    if (pthread_mutex_lock(&wp->swi_oswaitmutex) < 0)
        swi_osapiaborterror("waiter wait (m) failed", errno);

    if (wp->swi_oswaitsleepstate != SWI_OSWAITSTATE_ALERT)
                                /* if nobody woke us up */
    {
        wp->swi_oswaitsleepstate = SWI_OSWAITSTATE_SLEEP;
        if( timeout )
        {
            if (pthread_cond_timedwait(&wp->swi_oswaitcondition,
                        &wp->swi_oswaitmutex, &times) < 0 &&
                        errno != ETIMEDOUT)
                swi_osapiaborterror("waiter wait (tc) failed", errno);
        }
        /* No timeout specified so use another pthread call */
        else
        {
            if (pthread_cond_wait(&wp->swi_oswaitcondition,
                        &wp->swi_oswaitmutex ) < 0 )
                swi_osapiaborterror("waiter wait (c) failed", errno);
        }
    }
        
    waswoken = wp->swi_oswaitsleepstate != SWI_OSWAITSTATE_SLEEP;
    
    wp->swi_oswaitsleepstate = SWI_OSWAITSTATE_AWAKE;
    
    if (pthread_mutex_unlock(&wp->swi_oswaitmutex) < 0)
        swi_osapiaborterror("waiter wait (u) failed", errno);
    
    return waswoken;
}

/*************
 *
 * Name: swi_osapiwaiterwake
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
 *          swi_osapiwaiterinit must have been called first
 *
 **************/
void swi_osapiwaiterwake(struct swi_osapiwaiter *wp)
{
    if (pthread_mutex_lock(&wp->swi_oswaitmutex) < 0)
        swi_osapiaborterror("waiter wake (m) failed", errno);
   
    if (wp->swi_oswaitsleepstate == SWI_OSWAITSTATE_SLEEP) /* if asleep */
    {
        if (pthread_cond_signal(&wp->swi_oswaitcondition) < 0)
            swi_osapiaborterror("waiter wake (c) failed", errno);
    }
    
    wp->swi_oswaitsleepstate = SWI_OSWAITSTATE_ALERT;
    
    if (pthread_mutex_unlock(&wp->swi_oswaitmutex) < 0)
        swi_osapiaborterror("waiter wake (u) failed", errno);
}
/* RILSTOP */

/* 
 * $Log: swi_osapi.c,v $
 */
