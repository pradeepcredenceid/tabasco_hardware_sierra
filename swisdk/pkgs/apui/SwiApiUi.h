/**
 *
 *    $Id: SwiApiUi.h,v 1.2 2010/06/09 23:31:54 epasheva Exp $
 *
 *    Name       : SwiApiUi.h
 *    
 *    Purpose    : This file contains definitions, enumerations, structures and
 *                 forward declarations for SwiApiUi.c file
 *
 *    Copyright (c) 2009 Sierra Wireless, Inc.  All rights reserved
 *
 */
#ifndef __SWIAPUI
#define __SWIAPUI

/* include files */
#include "SwiDataTypes.h"
#include "SwiRcodes.h"
#include "SwiApiCmBasic.h"

/* defines */

/* enum declarations */

/* Function prototypes */
extern SWI_API SWI_RCODE SwiApiUiStartup (
    struct SWI_STRUCT_ApiStartup *pOpenParams); 
extern swi_bool SwiApUiRunning( void );
extern struct apuicb *SwiApUiGetCbp(void);
extern SWI_API SWI_RCODE SwiApiUiSend(
    swi_uint8 *datap, 
    swi_uint16 datalen);
extern SWI_API SWI_RCODE SwiApiUiSend( 
    swi_uint8 *datap, 
    swi_uint16 datalen);
extern SWI_API SWI_RCODE SwiApiUiReceive(
    swi_uint8 **respp, 
    swi_uint32 timeout);

#endif /* __SWIAPUI */

/* 
 * $Log: SwiApiUi.h,v $
 */
