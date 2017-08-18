/**
 *    $Id: SwiIntUpkCdmaLbs.h,v 1.1 2008/10/15 21:20:45 YGao Exp $
 *
 *    Name       : SwiIntUpkCdmaLbs.h
 *    
 *    Purpose    : Unpack functions for CDMA specific Lbs APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#ifndef __SWIINTUPKCDMALBS_H__
#define __SWIINTUPKCDMALBS_H__

/* include files */
#include "SwiDataTypes.h"
#include "SwiApiCmLbs.h"
#include "SwiApiCdmaLbs.h"

extern void SwiUnpackLbsPaBsInfo(swi_uint8 *paramfieldp, 
                                 SWI_STRUCT_LbsPaBsInfo *psBsInfo);

extern void SwiUnpackLbsFixTypeControl(
        swi_uint8 *paramfieldp, 
        SWI_STRUCT_LBS_FixTypeControl *pFixTypeControl);

extern void SwiUnpackLbsAutoStart(swi_uint8 *paramfieldp, 
                                  SWI_STRUCT_LbsAutoStart *psLbsAutoStart);

#endif /* __SWIINTUPKCDMALBS_H__ */

/* 
 * $Log: SwiIntUpkCdmaLbs.h,v $
 */

