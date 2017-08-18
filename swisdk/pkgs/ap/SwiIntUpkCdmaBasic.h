/**
 *    $Id: SwiIntUpkCdmaBasic.h,v 1.1 2008/10/15 21:20:45 YGao Exp $
 *
 *    Name       : SwiIntUpkCdmaBasic.h
 *    
 *    Purpose    : Unpack functions for CDMA specific bassic APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#ifndef __SWIINTUPKCDMABASIC_H__
#define __SWIINTUPKCDMABASIC_H__


/* include files */
#include "SwiDataTypes.h"
#include "SwiApiCdmaBasic.h"

extern void SwiUnpackMeidEsn( swi_uint8 *paramfieldp, 
                             SWI_STRUCT_MeidEsn *meidesnp );

extern void SwiUnpackTechHdrMacIndex(
        swi_uint8 *paramfieldp, 
        SWI_STRUCT_Tech_Hdr_Mac_Index *pTechHdrMacIndex);

#endif /* __SWIINTUPKCDMABASIC_H__ */

/* 
 * $Log: SwiIntUpkCdmaBasic.h,v $
 */

