/**
 *
 * @ingroup swiril
 *
 * @file 
 * Prototypes for QMI Location Service (LOC) functions
 *
 * @author
 * Copyright: © 2015 Sierra Wireless, Inc.
 *            All rights reserved
 *
 */
 
#ifndef SWIRIL_GPS_QMI_LOC_H
#define SWIRIL_GPS_QMI_LOC_H 1

#include "SWIWWANCMAPI.h"
#include "qmerrno.h"

typedef enum {
    LOC_INJECT_UTC = 0,
    LOC_GET_SERVER,
    LOC_SET_SERVER,
    LOC_GET_OPMODE,
    LOC_GET_FIX_CRT
}LOC_EVENT;

typedef enum {
    MODE_STANDALONE = 0,
    MODE_MSB =1,
    MODE_MSA =2
}OP_MODE;


ULONG enableLocCallback(LOC_EVENT id);
ULONG disableLocCallback(LOC_EVENT id);
int getLocReqInfo(LOC_EVENT id);
void setLocReqInfo(LOC_EVENT id, int value);
ULONG rilLocInjTime(int filedesc, ULONGLONG time, ULONG uncertainty);
ULONG rilLocSetServer(int filedesc, ULONG ipaddr, ULONG port, BYTE *urlbuf, BYTE urllen);
ULONG rilLocGetServer(int filedesc);
ULONG rilLocGetDef(int filedesc);
ULONG rilLocGetOpMode();
ULONG rilLocGetFixCriteria();
#endif