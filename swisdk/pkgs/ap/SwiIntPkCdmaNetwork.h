/**
 *    $Id: SwiIntPkCdmaNetwork.h,v 1.1 2008/10/15 21:20:45 YGao Exp $
 *
 *    Name       : SwiIntPkCdmaNetwork.h
 *    
 *    Purpose    : Pack functions for  CDMA specific network APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#ifndef __SWIINTPKCDMANETWORK_H__
#define __SWIINTPKCDMANETWORK_H__

/* include files */
#include "SwiDataTypes.h"
#include "SwiApiCdmaNetwork.h"

swi_uint32 SwiPstPackDsMipGenUser(swi_uint8 *paramfieldp, 
                                  swi_uint16 index, 
                                  swi_uint16 naiLength, 
                                  swi_charp nai, 
                                  swi_uint16 mnHaSpiEnable, 
                                  swi_uint32 mnHaSpi, 
                                  swi_uint16 mnAaaSpiEnable, 
                                  swi_uint32 mnAaaSpi, 
                                  swi_uint16 reverseTunneling, 
                                  swi_uint32 homeAddress, 
                                  swi_uint32 primaryHaAddress, 
                                  swi_uint32 secondaryHaAddress);

swi_uint32 SwiPstPackDsMipEnableProf(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_MipEnabled *psMipEnabled);

package swi_uint32 SwiPackModePref( swi_uint8 *paramfieldp, 
                                  swi_uint16 modepref,
                                  swi_uint16 modeterm,
                                  swi_uint32 modedur,
                                  swi_uint32 bandpref,
                                  swi_uint16 roampref );

package swi_uint32 SwiPackSidNid( swi_uint8 *paramfieldp, 
                                 const struct SWI_STRUCT_HomeSidNid *sidnidp, 
                                 swi_uint16 Nam );

#endif /* __SWIINTPKCDMANETWORK_H__ */

/* 
 * $Log: SwiIntPkCdmaNetwork.h,v $
 */
