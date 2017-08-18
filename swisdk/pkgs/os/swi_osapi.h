/*************
 *
 * $Id: swi_osapi.h,v 1.8 2010/06/09 23:50:42 epasheva Exp $
 *
 * Filename: swi_osapi.h
 *
 * Purpose:  OS wrapper package API-side (application process) definitions
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/
#ifndef swi_osapi_h
#define swi_osapi_h

/* include files */
#include "../aa/aaglobal.h"
#include <limits.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
/* RILSTART */
#include <time.h>
/* RILSTOP */
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <dirent.h>
/* RILSTART */
// removed syslog.h
/* RILSTOP */
#include <signal.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include <sys/select.h>
#include <errno.h>
#include <stdio.h>
#include <sys/resource.h>
#include <sys/prctl.h>
/* RILSTART */
// removed ucontext.h
/* RILSTOP */
#include <pthread.h>

/*
 * constants
 */

/*
 * structures (do not rely on their internals -- they will change)
 */
struct swi_osipcapi  {
    int osipcsocket;
    struct sockaddr_un osipcdest;
};

/* RILSTART */
/* thread wait/wake */
struct swi_osapiwaiter {
    pthread_mutex_t swi_oswaitmutex;          /* pthread mutex */
    pthread_cond_t  swi_oswaitcondition;      /* pthread condition variable */
    volatile swi_uint32 swi_oswaitsleepstate; /* SWI_OSWAITSTATE_xxx */
};
/* RILSTOP */

/* prototypes */
void swi_osapierrorhandlerset(void (*handlerp)(const char *, int));
swi_bool swi_osapisdkprocesscreate(const char *pathp);
void swi_osapiipcsetdest(struct swi_osipcapi *ip, 
                         swi_size_t ipcindex, 
                         swi_uint8 * ipcnamep);
swi_bool swi_osapiipcopen(struct swi_osipcapi *ip, size_t ipcindex,
    swi_uint8* ipcnamep);
swi_bool swi_osapiipcclose(struct swi_osipcapi *picp);
void swi_osapilog(const char *errorp);
swi_bool swi_osapiipcwrite(struct swi_osipcapi *ip, 
                           const void *bufp, 
                           size_t length);
swi_bool swi_osapiipcread(struct swi_osipcapi *ip, 
                          void *bufp, 
                          size_t *lengthp, 
                          swi_uint32 timeout);
void swi_osapithreadcreate(void (*fnp)(void));
/* RILSTART */
void swi_osapistopsdk( void );
void swi_osapiwaiterinit(struct swi_osapiwaiter *wp);
swi_bool swi_osapiwaiterwait(struct swi_osapiwaiter *wp, swi_uint32 timeout);
void swi_osapiwaiterwake(struct swi_osapiwaiter *wp);
/* RILSTOP */
#endif

/*
 * $Log: swi_osapi.h,v $
 */
