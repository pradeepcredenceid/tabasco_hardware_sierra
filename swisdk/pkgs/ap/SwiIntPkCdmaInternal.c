/**
 *    $Id: SwiIntPkCdmaInternal.c,v 1.2 2010/06/10 00:23:06 epasheva Exp $
 *
 *    Name       : SwiIntPkCdmaInternal.c
 *    
 *    Purpose    : Pack functions for  CDMA specific internal APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#include "SwiIntCdma.h"
#include "SwiIntPkCdmaInternal.h"
#include "apidefs.h"

/**
 *
 * This function packs HDR SCP subtype custom config data to the CnS object's 
 * parameter field for SwiSetHdrScpCustomerConfig API.
 *
 * \param paramfieldp[OUT]          Pointer to storage into which the packed 
 *                                  data will be placed by this function.
 * \param pSubtypeCustomConfig[IN]  HDR SCP subtype custom config data. 
 *                              (see SWI_STRUCT_HDRSCP_Subtype_Custom_Config)
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackHdrScpCustomerConfig(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_HDRSCP_Subtype_Custom_Config *pSubtypeCustomConfig)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(NV_HDRSCP_SUBTYPE_CUSTOM_CONFIG_I, &localfieldp);

    piput16(pSubtypeCustomConfig->bIsActive, &localfieldp);

    piput32(pSubtypeCustomConfig->nProtSubtypeMask, &localfieldp);
    piput32(pSubtypeCustomConfig->nBcmcsSubtypeMask, &localfieldp);
    piput32(pSubtypeCustomConfig->nAppSubtypeMask, &localfieldp);

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs HDR Su Fwd Statistics data to the CnS object's 
 * parameter field for SwiSetTechHdrSuFwdStats API.
 *
 * \param paramfieldp[OUT]          Pointer to storage into which the packed 
 *                                  data will be placed by this function.
 * \param pTechHdrSuFwdStats[IN]    HDR Su Fwd Statistics data. 
 *                                  (see SWI_STRUCT_Tech_Hdr_Su_Fwd_Stats)
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackTechHdrSuFwdStats(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_Tech_Hdr_Su_Fwd_Stats *pTechHdrSuFwdStats)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(1, &localfieldp);   /* obj_ver = 1*/

    piput32s(pTechHdrSuFwdStats->nTcCrcGood, 
            CNS_TECH_HDR_TC_DRC_NUM, &localfieldp);

    piput32s(pTechHdrSuFwdStats->nTcCrcBad, 
            CNS_TECH_HDR_TC_DRC_NUM, &localfieldp);

    piput32s(pTechHdrSuFwdStats->nCcCrcGood, 
            CNS_TECH_HDR_CC_DRC_NUM, &localfieldp);

    piput32s(pTechHdrSuFwdStats->nCcCrcBad, 
            CNS_TECH_HDR_CC_DRC_NUM, &localfieldp);

    piput32(pTechHdrSuFwdStats->nTotalPackets, &localfieldp);
    piput32(pTechHdrSuFwdStats->nBadPackets, &localfieldp);
    piput32(pTechHdrSuFwdStats->nTcServedThroughput, &localfieldp);
    piput32(pTechHdrSuFwdStats->nTcInstantThroughput, &localfieldp);
    piput32(pTechHdrSuFwdStats->nTcAvrgThroughtput, &localfieldp);

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs HDR Su Fwd Slot Cnt data to the CnS object's 
 * parameter field for SwiSetTechHdrSuFwdSlotCnt API.
 *
 * \param paramfieldp[OUT]          Pointer to storage into which the packed 
 *                                  data will be placed by this function.
 * \param pTechHdrSuFwdSlotCnt[IN]  HDR Su Fwd Slot Cnt data. 
 *                                  (see SWI_STRUCT_Tech_Hdr_Su_Fwd_Slot_Cnt)
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackTechHdrSuFwdSlotCnt(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_Tech_Hdr_Su_Fwd_Slot_Cnt *pTechHdrSuFwdSlotCnt)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(1, &localfieldp);   /* obj_ver = 1*/

    piput32s(pTechHdrSuFwdSlotCnt->nTcDrc0Tsc, 
            NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc0Tsc, &localfieldp);

    piput32s(pTechHdrSuFwdSlotCnt->nTcDrc1Tsc, 
            NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc1Tsc, &localfieldp);

    piput32s(pTechHdrSuFwdSlotCnt->nTcDrc2Tsc, 
            NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc2Tsc, &localfieldp);

    piput32s(pTechHdrSuFwdSlotCnt->nTcDrc3Tsc, 
            NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc3Tsc, &localfieldp);

    piput32s(pTechHdrSuFwdSlotCnt->nTcDrc4Tsc, 
            NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc4Tsc, &localfieldp);

    piput32s(pTechHdrSuFwdSlotCnt->nTcDrc5Tsc, 
            NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc5Tsc, &localfieldp);

    piput32s(pTechHdrSuFwdSlotCnt->nTcDrc6Tsc, 
            NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc6Tsc, &localfieldp);

    piput32s(pTechHdrSuFwdSlotCnt->nTcDrc7Tsc, 
            NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc7Tsc, &localfieldp);

    piput32s(pTechHdrSuFwdSlotCnt->nTcDrc8Tsc, 
            NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc8Tsc, &localfieldp);

    piput32s(pTechHdrSuFwdSlotCnt->nTcDrc9Tsc, 
            NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc9Tsc, &localfieldp);

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs HDR Mu Fwd statistics data to the CnS object's 
 * parameter field for SwiSetTechHdrMuFwdStats API.
 *
 * \param paramfieldp[OUT]          Pointer to storage into which the packed  
 *                                  data will be placed by this function.
 * \param pTechHdrMuFwdStats[IN]    HDR Mu Fwd statistics data. 
 *                                  (see SWI_STRUCT_Tech_Hdr_Mu_Fwd_Stats)
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackTechHdrMuFwdStats(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_Tech_Hdr_Mu_Fwd_Stats *pTechHdrMuFwdStats)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(1, &localfieldp);   /* obj_ver = 1*/

    piput32(pTechHdrMuFwdStats->nDrc3_128_GoodCrc, &localfieldp);
    piput32(pTechHdrMuFwdStats->nDrc3_256_GoodCrc, &localfieldp);
    piput32(pTechHdrMuFwdStats->nDrc3_512_GoodCrc, &localfieldp);
    piput32(pTechHdrMuFwdStats->nDrc3_1024_GoodCrc, &localfieldp);
    piput32(pTechHdrMuFwdStats->nDrc5_2048_GoodCrc, &localfieldp);
    piput32(pTechHdrMuFwdStats->nDrc8_3072_GoodCrc, &localfieldp);
    piput32(pTechHdrMuFwdStats->nDrc10_4096_GoodCrc, &localfieldp);
    piput32(pTechHdrMuFwdStats->nDrc13_5120_GoodCrc, &localfieldp);
    piput32(pTechHdrMuFwdStats->nDrc3BadCrc, &localfieldp);
    piput32(pTechHdrMuFwdStats->nDrc5BadCrc, &localfieldp);
    piput32(pTechHdrMuFwdStats->nDrc8BadCrc, &localfieldp);
    piput32(pTechHdrMuFwdStats->nDrc10BadCrc, &localfieldp);
    piput32(pTechHdrMuFwdStats->nDrc13BadCrc, &localfieldp);

    piput32s(pTechHdrMuFwdStats->nDrc3_128_Tsc, 
            NUMBER_Tech_Hdr_Mu_Fwd_Stats_Drc3_Tsc, &localfieldp);

    piput32s(pTechHdrMuFwdStats->nDrc3_256_Tsc, 
            NUMBER_Tech_Hdr_Mu_Fwd_Stats_Drc3_Tsc, &localfieldp);

    piput32s(pTechHdrMuFwdStats->nDrc3_512_Tsc, 
            NUMBER_Tech_Hdr_Mu_Fwd_Stats_Drc3_Tsc, &localfieldp);

    piput32s(pTechHdrMuFwdStats->nDrc3_1024_Tsc, 
            NUMBER_Tech_Hdr_Mu_Fwd_Stats_Drc3_Tsc, &localfieldp);

    piput32s(pTechHdrMuFwdStats->nDrc5Tsc, 
            NUMBER_Tech_Hdr_Mu_Fwd_Stats_Drc5Tsc, &localfieldp);

    piput32s(pTechHdrMuFwdStats->nDrc8Tsc, 
            NUMBER_Tech_Hdr_Mu_Fwd_Stats_Drc8Tsc, &localfieldp);

    piput32s(pTechHdrMuFwdStats->nDrc10Tsc, 
            NUMBER_Tech_Hdr_Mu_Fwd_Stats_Drc10Tsc, &localfieldp);

    piput32s(pTechHdrMuFwdStats->nDrc13Tsc, 
            NUMBER_Tech_Hdr_Mu_Fwd_Stats_Drc13Tsc, &localfieldp);

    piput32(pTechHdrMuFwdStats->nTotalPackets, &localfieldp);
    piput32(pTechHdrMuFwdStats->nBadPackets, &localfieldp);
    piput32(pTechHdrMuFwdStats->nServedThroughput, &localfieldp);
    piput32(pTechHdrMuFwdStats->nInstantThroughput, &localfieldp);
    piput32(pTechHdrMuFwdStats->nAvrgThroughtput, &localfieldp);

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs warranty data to the CnS object's 
 * parameter field for SwiPstSetWarDateCode API.
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                          data will be placed by this function.
 * \param pszWarranty[IN]   Waranty data. 
 * \param pszExtra[IN]      Extry data. 
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPstPackWarDateCode(swi_uint8 *paramfieldp, 
                                         const swi_charp pszWarranty, 
                                         const swi_charp pszExtra)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(NV_WDC, &localfieldp);   /* NV index */
    
    piputmulti((const swi_uint8 *)pszWarranty, &localfieldp, LENGTH_NvWdcDate);
    piputmulti((const swi_uint8 *)pszExtra, &localfieldp, LENGTH_NvWdcExtra);

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs dial phonebook data to the CnS object's 
 * parameter field for SwiPstSetDialPhonebook API.
 *
 * \param paramfieldp[OUT]      Pointer to storage into which the packed 
 *                              data will be placed by this function.
 * \param address[IN]           Address. 
 * \param status[IN]            Status. 
 * \param numberOfDigits[IN]    Number of digits. 
 * \param pszDigits[IN]         Digit. 
 * \param pszName[IN]           Name. 
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPstPackDialPhonebook(swi_uint8 *paramfieldp, 
                                           swi_uint16 address, 
                                           swi_uint16 status, 
                                           swi_uint16 numberOfDigits, 
                                           const swi_charp pszDigits, 
                                           const swi_charp pszName)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(NV_DIAL_I, &localfieldp);   /* NV index */

    piput16(address, &localfieldp);   /* NV index */
    piput16(status, &localfieldp);   /* NV index */
    piput16(numberOfDigits, &localfieldp);   /* NV index */
    piputmulti((const swi_uint8 *)pszDigits, &localfieldp, numberOfDigits);
    localfieldp += LENGTH_NvDialDigits-numberOfDigits;

    slmemset((char *)localfieldp, 0, LENGTH_NvDialLetters);
    piputmulti((const swi_uint8 *)pszName, &localfieldp, slstrlen(pszName));
    localfieldp += LENGTH_NvDialLetters-slstrlen(pszName);

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs provision information to the CnS object's 
 * parameter field for SwiPstSetProvisionInfo API.
 *
 * \param paramfieldp[OUT]      Pointer to storage into which the packed 
 *                              data will be placed by this function.
 * \param nam1ProvFlag[IN]      NAM1 provision flag.
 * \param pszNam1ProvDate[IN]   NAM1 provision date. 
 * \param nam2ProvFlag[IN]      NAM2 provision flag. 
 * \param pszNam2ProvDate[IN]   NAM2 provision date.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPstPackProvisionInfo(swi_uint8 *paramfieldp, 
                                           swi_uint32 nam1ProvFlag, 
                                           const swi_charp pszNam1ProvDate, 
                                           swi_uint32 nam2ProvFlag, 
                                           const swi_charp pszNam2ProvDate)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(SWI_NV_PROVISION_INFO, &localfieldp);   /* NV index */

    *localfieldp++ = nam1ProvFlag;
    slmemset((char *)pszNam1ProvDate, 0, LENGTH_NvNam1ProvisionDate);
    piputmulti((const swi_uint8 *)pszNam1ProvDate, &localfieldp, slstrlen(pszNam1ProvDate));
    localfieldp += LENGTH_NvNam1ProvisionDate-slstrlen(pszNam1ProvDate);

    *localfieldp++ = nam2ProvFlag;
    slmemset((char *)pszNam1ProvDate, 0, LENGTH_NvNam2ProvisionDate);
    piputmulti((const swi_uint8 *)pszNam2ProvDate, &localfieldp, slstrlen(pszNam2ProvDate));
    localfieldp += LENGTH_NvNam2ProvisionDate-slstrlen(pszNam2ProvDate);

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs error state to the CnS object's 
 * parameter field for SwiSetTechErrorStat API.
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed 
 *                          data will be placed by this function.
 * \param errSeqNumber[IN]  Error sequence number.
 * \param errFatalFlag[IN]  Fatal error flag. 
 * \param errCount[IN]      Error number. 
 * \param errFilename[IN]   Error file name
 * \param errLineNumber[IN] Error line number.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackTechErrorStat(swi_uint8 *paramfieldp, 
                                        swi_uint16 errSeqNumber, 
                                        swi_uint8 errFatalFlag, 
                                        swi_uint8 errCount, 
                                        const swi_charp errFilename, 
                                        swi_uint16 errLineNumber)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(errSeqNumber, &localfieldp);
    *localfieldp++ = errFatalFlag;
    *localfieldp++ = errCount;
    slmemset((char *)localfieldp, 0, LENGTH_TechErrFileName);
    piputmulti((const swi_uint8 *)errFilename, &localfieldp, slstrlen(errFilename));
    localfieldp += LENGTH_TechErrFileName-slstrlen(errFilename);
    piput16(errLineNumber, &localfieldp);

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs MIP Ss user prof data to the CnS object's 
 * parameter field for SwiPstSetDsMipSsUserProf API.
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed 
 *                          data will be placed by this function.
 * \param index[IN]         Index.
 * \param mnHaSsLength[IN]  Mm HA SS length. 
 * \param mnHaSs[IN]        Mm HA SS. 
 * \param mnAaaSsLength[IN] Mm AAA SS length
 * \param mnAaaSs[IN]       Mm AAA SS.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPstPackDsMipSsUserProf(swi_uint8 *paramfieldp, 
                                             swi_uint16 index, 
                                             swi_uint16 mnHaSsLength, 
                                             swi_charp mnHaSs, 
                                             swi_uint16 mnAaaSsLength, 
                                             swi_charp mnAaaSs)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(NV_DS_MIP_SS_USER_PROF, &localfieldp);   /* NV index */
    piput16(index, &localfieldp);

    piput16(mnHaSsLength, &localfieldp);
    piputmulti((const swi_uint8 *)mnHaSs, &localfieldp, mnHaSsLength);
    localfieldp += LENGTH_NvMnAaaSs-mnHaSsLength;

    piput16(mnAaaSsLength, &localfieldp);
    piputmulti((const swi_uint8 *)mnAaaSs, &localfieldp, mnAaaSsLength);
    localfieldp += LENGTH_NvMnAaaSs-mnHaSsLength;

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs MIP Mn HA time delta data to the CnS object's 
 * parameter field for SwiPstSetDsMipMnHaTimeDelta API.
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed 
 *                          data will be placed by this function.
 * \param index[IN]         Index.
 * \param mnHaDelta[IN]     Mn HA delta. 
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPstPackDsMipMnHaTimeDelta(swi_uint8 *paramfieldp, 
                                                swi_uint16 index, 
                                                swi_uint32 mnHaDelta)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(NV_DS_MIP_MN_HA_TIME_DELTA, &localfieldp);   /* NV index */
    piput16(index, &localfieldp);
    piput32(mnHaDelta, &localfieldp);

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs PRL size data to the CnS object's 
 * parameter field for SwiSetPRLSize API.
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed 
 *                          data will be placed by this function.
 * \param Nam[IN]           NAM Index.
 * \param nSize[IN]         PRL size. 
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackPRLSize(swi_uint8 *paramfieldp, 
                                  swi_uint16 Nam, 
                                  swi_uint32 nSize)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(Nam, &localfieldp);
    piput32(nSize, &localfieldp);

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs data statistics reset data to the CnS object's 
 * parameter field for SwiDataStatsReset API.
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed 
 *                          data will be placed by this function.
 * \param eProtocol[IN]     Protocol.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackDataStatsReset(swi_uint8 *paramfieldp, 
                                         swi_uint16 eProtocol)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(1, &localfieldp);   /* current obj_ver is 1 */
    *localfieldp++ = (swi_uint8)eProtocol;

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs data statistics fetch data to the CnS object's 
 * parameter field for SwiDataStatsFetch API.
 *
 * \param paramfieldp[OUT]      Pointer to storage into which the packed 
 *                              data will be placed by this function.
 * \param eProtocol[IN]         Protocol.
 * \param ePppSubProtocol[IN]   Sub protocol.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackDataStatsFetch(swi_uint8 *paramfieldp, 
                                         swi_uint16 eProtocol, 
                                         swi_uint16 ePppSubProtocol)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(1, &localfieldp);   /* current obj_ver is 1 */
    *localfieldp++ = (swi_uint8)eProtocol;
    *localfieldp++ = (swi_uint8)ePppSubProtocol;

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs auto populate information to the CnS object's 
 * parameter field for SwiPstSetAutoPopulateInfoAp API.
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed 
 *                          data will be placed by this function.
 * \param pData[IN]         Auto populate information.
 *                          See nv_auto_populate_info_ap_type.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPstPackAutoPopulateInfoAp(
        swi_uint8 *paramfieldp, 
        const nv_auto_populate_info_ap_type *pData)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(NV_AUTO_POPULATE_INFO_AP_I, &localfieldp);   /* NV index */
    piput16(pData->index, &localfieldp);
    piput16(pData->duration, &localfieldp);
    piput16(pData->trigger, &localfieldp);
    piputmulti((const swi_uint8 *)pData->host_indicators, &localfieldp, 4);

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs EFile block information to the CnS object's 
 * parameter field for SwiGetEFileBlock API.
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed 
 *                          data will be placed by this function.
 * \param pszEFilename[IN]  EFile name.
 * \param nSeq[IN]          Segment No.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackEFileBlock(swi_uint8 *paramfieldp, 
                                     const swi_charp pszEFilename, 
                                     swi_uint16 nSeq)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    slmemset((char *)localfieldp, 0, LENGTH_EFilename); 
    piputmulti((const swi_uint8 *)pszEFilename, &localfieldp, slstrlen(pszEFilename)); 
    localfieldp = paramfieldp + LENGTH_EFilename;
    piput16(nSeq, &localfieldp);

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs EFile information to the CnS object's 
 * parameter field for SwiSetEFileInfo API.
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed 
 *                          data will be placed by this function.
 * \param pszEFilename[IN]  EFile name.
 * \param nSize[IN]         Size.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackEFileInfo(swi_uint8 *paramfieldp, 
                                    const swi_charp pszEFilename, 
                                    swi_uint32 nSize)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    slmemset((char *)localfieldp, 0, LENGTH_EFilename); 
    piputmulti((const swi_uint8 *)pszEFilename, &localfieldp, slstrlen(pszEFilename)); 
    localfieldp = paramfieldp + LENGTH_EFilename;
    piput32(nSize, &localfieldp);

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/* 
 * $Log: SwiIntPkCdmaInternal.c,v $
 */

