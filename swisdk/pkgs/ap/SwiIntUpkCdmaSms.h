/**
 *    $Id: SwiIntUpkCdmaSms.h,v 1.1 2008/10/15 21:20:45 YGao Exp $
 *
 *    Name       : SwiIntUpkCdmaSms.h
 *    
 *    Purpose    : Unpack functions for CDMA specific SMS APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#ifndef __SWIINTUPKCDMASMS_H__
#define __SWIINTUPKCDMASMS_H__

/* include files */
#include "SwiDataTypes.h"
#include "SwiApiCdmaSms.h"

extern void SwiUnpackCdmaSMSMessageStatus(swi_uint8 *paramfieldp,
                                     SWI_STRUCT_CDMA_SMS_Status *pSMS_Status);
#endif /* __SWIINTUPKCDMASMS_H__ */

/* 
 * $Log: SwiIntUpkCdmaSms.h,v $
 */
