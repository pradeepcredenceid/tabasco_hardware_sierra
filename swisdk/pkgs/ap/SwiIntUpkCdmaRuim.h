/**
 *    $Id: SwiIntUpkCdmaRuim.h,v 1.1 2008/10/15 21:20:45 YGao Exp $
 *
 *    Name       : SwiIntUpkCdmaRuim.h
 *    
 *    Purpose    : Unpack functions for CDMA specific RUIM APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#ifndef __SWIINTUPKCDMARUIM_H__
#define __SWIINTUPKCDMARUIM_H__

/* include files */
#include "SwiDataTypes.h"
#include "SwiApiCdmaRuim.h"

extern void SwiUnpackRUIMStatus(swi_uint8 *paramfieldp,
                                SWI_STRUCT_RuimStatus* sRuimStatus);
#endif /* __SWIINTUPKCDMARUIM_H__ */

/* 
 * $Log: SwiIntUpkCdmaRuim.h,v $
 */
