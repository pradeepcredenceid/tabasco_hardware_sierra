/**
 *    $Id: SwiIntPkCdmaBasic.h,v 1.1 2008/10/15 21:20:45 YGao Exp $
 *
 *    Name       : SwiIntPkCdmaBasic.h
 *    
 *    Purpose    : Pack functions for  CDMA specific bassic APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#ifndef __SWIINTPKCDMABASIC_H__
#define __SWIINTPKCDMABASIC_H__

/* include files */
#include "SwiDataTypes.h"

extern swi_uint32 SwiPackChangeLockPIN(swi_uint8 *paramfieldp, 
                                       const swi_charp OldPassword, 
                                       const swi_charp NewPassword);

extern swi_uint32 SwiPackDeviceEVDOSetting(swi_uint8 *paramfieldp, 
                                       swi_uint16 wMask);

#endif /* __SWIINTPKCDMABASIC_H__ */

/* 
 * $Log: SwiIntPkCdmaBasic.h,v $
 */

