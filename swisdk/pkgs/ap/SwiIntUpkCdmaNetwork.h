/**
 *    $Id: SwiIntUpkCdmaNetwork.h,v 1.1 2008/10/15 21:20:45 YGao Exp $
 *
 *    Name       : SwiIntUpkCdmaNetwork.h
 *    
 *    Purpose    : Unpack functions for CDMA specific network APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#ifndef __SWIINTUPKCDMANETWORK_H__
#define __SWIINTUPKCDMANETWORK_H__

/* include files */
#include "SwiDataTypes.h"
#include "SwiApiCdmaNetwork.h"

extern void SwiPstUnpackHomeSidNid(swi_uint8 *paramfieldp, 
                                   SWI_STRUCT_HomeSidNid *HomeSidNid);

extern void SwiUnpackCallConnectionStateEx(
        swi_uint8 *paramfieldp, 
        SWI_STRUCT_CallConnected* CallState);

extern void SwiUnpackRssiEcio(swi_uint8 *paramfieldp, 
                              SWI_STRUCT_Rssi_Ecio* stRssiEcio);

extern void SwiUnpackTechActiveSet(swi_uint8 *paramfieldp, 
                                   SWI_STRUCT_TechActiveSet* pTechActSet);

extern void SwiUnpackRssiCdma( swi_uint8 *paramfieldp, swi_int32 *rssip );

#endif /* __SWIINTUPKCDMANETWORK_H__ */
/* 
 * $Log: SwiIntUpkCdmaNetwork.h,v $
 */

