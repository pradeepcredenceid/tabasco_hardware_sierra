/**
 *    $Id: SwiIntPkCdmaInternal.h,v 1.1 2008/10/15 21:20:45 YGao Exp $
 *
 *    Name       : SwiIntPkCdmaInternal.h
 *    
 *    Purpose    : Pack functions for CDMA specific internal APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#ifndef __SWIINTPKCDMAINTERNAL_H__
#define __SWIINTPKCDMAINTERNAL_H__

/* include files */
#include "SwiDataTypes.h"
#include "SwiApiCdmaBasic.h"
#include "SwiIntCdma.h"

extern swi_uint32 SwiPackHdrScpCustomerConfig(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_HDRSCP_Subtype_Custom_Config *pSubtypeCustomConfig);

extern swi_uint32 SwiPackTechHdrSuFwdStats(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_Tech_Hdr_Su_Fwd_Stats *pTechHdrSuFwdStats);

extern swi_uint32 SwiPackTechHdrSuFwdSlotCnt(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_Tech_Hdr_Su_Fwd_Slot_Cnt *pTechHdrSuFwdSlotCnt);

extern swi_uint32 SwiPackTechHdrMuFwdStats(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_Tech_Hdr_Mu_Fwd_Stats *pTechHdrMuFwdStats);

extern swi_uint32 SwiPstPackWarDateCode(swi_uint8 *paramfieldp, 
                                        const swi_charp pszWarranty, 
                                        const swi_charp pszExtra);

extern swi_uint32 SwiPstPackDialPhonebook(swi_uint8 *paramfieldp, 
                                          swi_uint16 address, 
                                          swi_uint16 status, 
                                          swi_uint16 numberOfDigits, 
                                          const swi_charp pszDigits, 
                                          const swi_charp pszName);

extern swi_uint32 SwiPstPackProvisionInfo(swi_uint8 *paramfieldp, 
                                          swi_uint32 nam1ProvFlag, 
                                          const swi_charp pszNam1ProvDate, 
                                          swi_uint32 nam2ProvFlag, 
                                          const swi_charp pszNam2ProvDate);

extern swi_uint32 SwiPackTechErrorStat(swi_uint8 *paramfieldp, 
                                       swi_uint16 errSeqNumber, 
                                       swi_uint8 errFatalFlag, 
                                       swi_uint8 errCount, 
                                       const swi_charp errFilename, 
                                       swi_uint16 errLineNumber);

extern swi_uint32 SwiPstPackDsMipSsUserProf(swi_uint8 *paramfieldp, 
                                            swi_uint16 index, 
                                            swi_uint16 mnHaSsLength, 
                                            swi_charp mnHaSs, 
                                            swi_uint16 mnAaaSsLength, 
                                            swi_charp mnAaaSs);

extern swi_uint32 SwiPstPackDsMipMnHaTimeDelta(swi_uint8 *paramfieldp, 
                                               swi_uint16 index, 
                                               swi_uint32 mnHaDelta);

extern swi_uint32 SwiPackPRLSize(swi_uint8 *paramfieldp, 
                                 swi_uint16 Nam, 
                                 swi_uint32 nSize);

extern swi_uint32 SwiPackDataStatsReset(swi_uint8 *paramfieldp, 
                                        swi_uint16 eProtocol);

extern swi_uint32 SwiPackDataStatsFetch(swi_uint8 *paramfieldp, 
                                        swi_uint16 eProtocol, 
                                        swi_uint16 ePppSubProtocol);

extern swi_uint32 SwiPstPackAutoPopulateInfoAp(
        swi_uint8 *paramfieldp, 
        const nv_auto_populate_info_ap_type *pData);

extern swi_uint32 SwiPackEFileBlock(swi_uint8 *paramfieldp, 
                                    const swi_charp pszEFilename, 
                                    swi_uint16 nSeq);

extern swi_uint32 SwiPackEFileInfo(swi_uint8 *paramfieldp, 
                                   const swi_charp pszEFilename, 
                                   swi_uint32 nSize);

#endif /* __SWIINTPKCDMAINTERNAL_H__ */

/* 
 * $Log: SwiIntPkCdmaInternal.h,v $
 */

