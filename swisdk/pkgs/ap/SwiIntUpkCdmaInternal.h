/**
 *    $Id: SwiIntUpkCdmaInternal.h,v 1.1 2008/10/15 21:20:45 YGao Exp $
 *
 *    Name       : SwiIntUpkCdmaInternal.h
 *    
 *    Purpose    : Unpack functions for CDMA specific internal APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#ifndef __SWIINTUPKCDMAINTERNAL_H__
#define __SWIINTUPKCDMAINTERNAL_H__

/* include files */
#include "SwiDataTypes.h"
#include "SwiIntCdma.h"

extern void SwiUnpackTechHdrSuFwdStats(
        swi_uint8 *paramfieldp, 
        SWI_STRUCT_Tech_Hdr_Su_Fwd_Stats *pTechHdrSuFwdStats);

extern void SwiUnpackTechHdrSuFwdSlotCnt(
        swi_uint8 *paramfieldp, 
        SWI_STRUCT_Tech_Hdr_Su_Fwd_Slot_Cnt *pTechHdrSuFwdSlotCnt);

extern void SwiUnpackTechHdrSuFwdSlotCnt2(
        swi_uint8 *paramfieldp, 
        SWI_STRUCT_Tech_Hdr_Su_Fwd_Slot_Cnt2 *pTechHdrSuFwdSlotCnt2);

extern void SwiUnpackTechHdrMuFwdStats(
        swi_uint8 *paramfieldp, 
        SWI_STRUCT_Tech_Hdr_Mu_Fwd_Stats *pTechHdrMuFwdStats);

extern SWI_RCODE SwiUnpackNetworkUserID(swi_uint8 *paramfieldp, 
                                        swi_charp pszUserID, 
                                        swi_uint32 length);

extern void SwiUnpackHdrScpCustomerConfig(
        swi_uint8 *paramfieldp, 
        SWI_STRUCT_HDRSCP_Subtype_Custom_Config *pSubtypeCustomConfig);

extern void SwiUnpackTechHdrPersonality(
        swi_uint8 *paramfieldp, 
        SWI_STRUCT_Tech_Hdr_Personality *pTechHdrPersonality);

extern void SwiUnpackTechHdrProtAppSubtype(
        swi_uint8 *paramfieldp, 
        SWI_STRUCT_Tech_Hdr_Prot_App_Subtype *pTechHdrProtAppSubtype);

extern void SwiUnpackTechHdrSleepParms(
        swi_uint8 *paramfieldp, 
        SWI_STRUCT_Tech_Hdr_Sleep_Parms *pTechHdrSleepParms);

extern SWI_RCODE SwiPstUnpackWarDateCode(swi_uint8 *paramfieldp, 
                                         swi_charp pszWarranty, 
                                         swi_uint32 lengthWarranty, 
                                         swi_charp pszExtra, 
                                         swi_uint32 lengthExtra);

extern SWI_RCODE SwiPstUnpackDialPhonebook(swi_uint8 *paramfieldp, 
                                           swi_uint16* status, 
                                           swi_uint16* numberOfDigits, 
                                           swi_charp pszDigits, 
                                           swi_uint32 szDigitsLength, 
                                           swi_charp pszName, 
                                           swi_uint32 szNameLength);

extern SWI_RCODE SwiPstUnpackProvisionInfo(swi_uint8 *paramfieldp, 
                                           swi_uint32* nam1ProvFlag, 
                                           swi_charp pszNam1ProvDate, 
                                           swi_uint32 szLengthNam1, 
                                           swi_uint32* nam2ProvFlag, 
                                           swi_charp pszNam2ProvDate, 
                                           swi_uint32 szLengthNam2);

extern SWI_RCODE SwiUnpackTechFinger(swi_uint8 *paramfieldp, 
                                     SWI_STRUCT_TechFinger* TechFing);

extern void SwiUnpackE911Info(swi_uint8 *paramfieldp, 
                              swi_uint16* startLocation, 
                              swi_uint16* endLocation, 
                              swi_uint16* programMask);

extern SWI_RCODE SwiUnpackTechErrorStat(swi_uint8 *paramfieldp, 
                                        swi_uint8* errFatalFlag, 
                                        swi_uint8* errCount, 
                                        swi_charp errFilename, 
                                        swi_uint32 length, 
                                        swi_uint16* errLineNumber);

extern SWI_RCODE SwiPstUnpackDsMipSsUserProf(swi_uint8 *paramfieldp, 
                                             swi_charp mnHaSs, 
                                             swi_uint16* pmnHaSsLength, 
                                             swi_uint16 mnHaSsBufLength, 
                                             swi_charp mnAaaSs, 
                                             swi_uint16* pmnAaaSsLength, 
                                             swi_uint16 mnAaaSsBufLength);

extern SWI_RCODE SwiPstUnpackDsMipGenUserBak(swi_uint8 *paramfieldp, 
                                             swi_uint16 naiLength, 
                                             swi_charp nai, 
                                             swi_uint16* pMnHaSpiEnable, 
                                             swi_uint32* pMnHaSpi, 
                                             swi_uint16* pMnAaaSpiEnable, 
                                             swi_uint32* pMnAaaSpi, 
                                             swi_uint16* pReverseTunneling, 
                                             swi_uint32* pHomeAddress, 
                                             swi_uint32* pPrimaryHaAddress, 
                                             swi_uint32* pSecondaryHaAddress);

extern SWI_RCODE SwiPstUnpackDsMipSsUserBak(swi_uint8 *paramfieldp, 
                                            swi_uint16 mnHaSsLength, 
                                            swi_charp mnHaSs, 
                                            swi_uint16 mnAaaSsLength, 
                                            swi_charp mnAaaSs);

extern void SwiUnpackEFileInfo(swi_uint8 *paramfieldp, 
                               SWI_STRUCT_EFileInfo* psEFileInfo);

extern void SwiUnpackPRLSize(swi_uint8 *paramfieldp, 
                             swi_uint16 *NumBlocks, 
                             swi_uint32 *Size);

extern SWI_RCODE SwiEVDOUnpackUati(swi_uint8 *paramfieldp, 
                                   swi_uint8* lUati, 
                                   swi_uint32 Size);

extern void SwiUnpackDataStatsReset(swi_uint8 *paramfieldp, 
                                    SWI_STRUCT_DataStatsReset *pResetResult);

extern void SwiUnpackDataStatsFetch(swi_uint8 *paramfieldp, 
                                    SWI_STRUCT_DataStatsFetch *pDataStats);

extern void SwiPstUnpackAutoPopulateInfoAp(
        swi_uint8 *paramfieldp, 
        nv_auto_populate_info_ap_type *pData);

extern SWI_RCODE SwiUnpackEFileBlock(swi_uint8 *paramfieldp, 
                                     swi_uint16 nSeq, 
                                     swi_uint8* pBuffer, 
                                     swi_uint16 nBufLen, 
                                     swi_uint32 *pnBlockSize);

#endif /* __SWIINTUPKCDMAINTERNAL_H__ */

/* 
 * $Log: SwiIntUpkCdmaInternal.h,v $
 */

