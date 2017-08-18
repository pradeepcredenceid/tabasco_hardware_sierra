/**
 *    $Id: SwiIntPkCdmaLbs.h,v 1.1 2008/10/15 21:20:45 YGao Exp $
 *
 *    Name       : SwiIntPkCdmaLbs.h
 *    
 *    Purpose    : Pack functions for  CDMA specific LBS APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#ifndef __SWIINTPKCDMALBS_H__
#define __SWIINTPKCDMALBS_H__

/* include files */
#include "SwiDataTypes.h"
#include "SwiApiCmLbs.h"
#include "SwiApiCdmaLbs.h"

extern swi_uint32 SwiPackLbsLocNotifyStatus(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_LbsLocNotifyStatus *psNotifyStatus);

extern swi_uint32 SwiPackLbsDloadNotifyStatus(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_LbsDloadNotifyStatus *psNotifyStatus);

extern swi_uint32 SwiPackLbsParamNotifyStatus(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_LbsParamNotifyStatus *psNotifyStatus);

extern swi_uint32 SwiPackLbsPdDownload(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_LbsPdDownload *psLbsPdDownload);

extern swi_uint32 SwiPackLbsPaGpsLock(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_LbsPaGpsLock *psPaGpsLock);

extern swi_uint32 SwiPackLbsPaPtlmMode(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_LbsPaPtlmMode *psPaPtlmMode);

extern swi_uint32 SwiPackLbsPaPrivacy(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_LbsPaPrivacy *psPaPrivacy);

extern swi_uint32 SwiPackLbsPaNetAccess(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_LbsPaNetAccess *psPaNetAccess);

extern swi_uint32 SwiPackLbsFixTypeControl(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_LBS_FixTypeControl *pFixTypeControl);

extern swi_uint32 SwiPackLbsAutoStart(
        swi_uint8 *paramfieldp, 
        swi_uint16 nNvIndex, 
        const SWI_STRUCT_LbsAutoStart *psLbsAutoStart);

#endif /* __SWIINTPKCDMALBS_H__ */


/* 
 * $Log: SwiIntPkCdmaLbs.h,v $
 */

