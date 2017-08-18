/**
 *    $Id: SwiIntPkCdmaRuim.h,v 1.1 2008/10/15 21:20:45 YGao Exp $
 *
 *    Name       : SwiIntPkCdmaRuim.h
 *    
 *    Purpose    : Pack functions for CDMA specific RUIM APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */
#ifndef __SWIINTPKCDMARUIM_H__
#define __SWIINTPKCDMARUIM_H__

/* include files */
#include "SwiDataTypes.h"

extern swi_uint32 SwiPackChangeCHV(swi_uint8 *paramfieldp, 
                                   swi_uint16 Chv,
                                   const swi_charp szOldCHV, 
                                   const swi_charp szNewCHV);

extern swi_uint32 SwiPackUnblockCHV(swi_uint8 *paramfieldp, 
                                    swi_uint16 Chv,
                                    const swi_charp szUnblockCHV, 
                                    const swi_charp szCHV);

#endif /* __SWIINTPKCDMARUIM_H__ */
/* 
 * $Log: SwiIntPkCdmaRuim.h,v $
 */
