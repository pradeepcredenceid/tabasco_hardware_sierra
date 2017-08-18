/**
 *    $Id: SwiIntUpkCmLbs.h,v 1.1 2008/11/24 19:06:13 ckaria Exp $
 *
 *    Name       : SwiIntUpkCmLbs.h
 *    
 *    Purpose    : Unpack functions for Common LBS APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#ifndef __SWIINTUPKCMLBS_H__
#define __SWIINTUPKCMLBS_H__

/* include files */
#include "SwiDataTypes.h"
#include "SwiApiCmLbs.h"

extern void SwiUnpackLbsSatInfo(swi_uint8 *paramfieldp, 
                         struct SWI_STRUCT_LbsSatInfo *psSatInfo);

extern void SwiUnpackLbsPdData(swi_uint8 *paramfieldp, 
                         struct SWI_STRUCT_LbsPdData *psLbsPdData);

extern void SwiUnpackLbsPaParam(swi_uint8 *paramfieldp, 
                         struct SWI_STRUCT_LbsPaGetParam *psLbsPaGetParam);

extern void SwiUnpackLbsPdStatus(swi_uint8 *paramfieldp, 
                         struct SWI_STRUCT_LbsPdGetStatus *psLbsPdGetStatus);

extern void SwiUnpackLbsPdDloadData(swi_uint8 *paramfieldp, 
                           struct SWI_STRUCT_LbsPdDloadData *psLbsPdDloadData);
#endif /* __SWIINTUPKCMLBS_H__ */

/* 
 * $Log: SwiIntUpkCmLbs.h,v $
 */
