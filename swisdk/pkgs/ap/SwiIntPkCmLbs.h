/**
 *    $Id: SwiIntPkCmLbs.h,v 1.1 2008/11/24 19:05:28 ckaria Exp $
 *
 *    Name       : SwiIntPkCmLbs.h
 *    
 *    Purpose    : Pack functions for Common LBS APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */
#ifndef __SWIINTPKCMLBS_H__
#define __SWIINTPKCMLBS_H__

/* include files */
#include "SwiDataTypes.h"

extern swi_uint32 SwiPackLbsPdGetPos(
              swi_uint8 *paramfieldp,
              const struct SWI_STRUCT_LbsPdGetPos *psLbsPdGetPos);

extern swi_uint32 SwiPackLbsPdTrack(
              swi_uint8 *paramfieldp,
              const struct SWI_STRUCT_LbsPdTrack *psLbsPdTrack);

extern swi_uint32 SwiPackLbsPdEndSession(
              swi_uint8 *paramfieldp,
              const struct SWI_STRUCT_LbsPdEndSession *psLbsPdEndSession);

extern swi_uint32 SwiPackLbsPaIpAddr(
              swi_uint8 *paramfieldp,
              const struct SWI_STRUCT_LbsPaIpAddr *psPaIpAddr);

extern swi_uint32 SwiPackLbsPaPortId(
              swi_uint8 *paramfieldp,
              const struct SWI_STRUCT_LbsPaPortId *psPAPortID);

extern swi_uint32 SwiPackLbsClearAssistance(
              swi_uint8 *paramfieldp,
            const struct SWI_STRUCT_LbsClearAssistance *psLbsClearAssistance);

#endif /* __SWIINTPKCMLBS_H__ */
/* 
 * $Log: SwiIntPkCmLbs.h,v $
 */

