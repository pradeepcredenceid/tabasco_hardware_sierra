/**
 *    $Id: SwiIntUpkCdmaInternal.c,v 1.2 2010/06/10 00:24:10 epasheva Exp $
 *
 *    Name       : SwiIntUpkCdmaInternal.c
 *    
 *    Purpose    : Unpack functions for CDMA specific internal APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#include "SwiIntCdma.h"
#include "SwiIntUpkCdmaInternal.h"
#include "apidefs.h"

/**
 *
 * Unpacks the HDR Su Fwd statistics.
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   pTechHdrSuFwdStats: [OUT]   HDR Su Fwd statistics.
 *                                      See SWI_STRUCT_Tech_Hdr_Su_Fwd_Stats.
 *
 * \return  none
 *
 */
package void SwiUnpackTechHdrSuFwdStats(
        swi_uint8 *paramfieldp, 
        SWI_STRUCT_Tech_Hdr_Su_Fwd_Stats *pTechHdrSuFwdStats)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    piget16(&parmfp);    /* Discard obj_version */

    piget32s(pTechHdrSuFwdStats->nTcCrcGood, CNS_TECH_HDR_TC_DRC_NUM, &parmfp);
    piget32s(pTechHdrSuFwdStats->nTcCrcBad, CNS_TECH_HDR_TC_DRC_NUM, &parmfp);
    piget32s(pTechHdrSuFwdStats->nCcCrcGood, CNS_TECH_HDR_CC_DRC_NUM, &parmfp);
    piget32s(pTechHdrSuFwdStats->nCcCrcBad, CNS_TECH_HDR_CC_DRC_NUM, &parmfp);
    pTechHdrSuFwdStats->nTotalPackets = piget32(&parmfp);
    pTechHdrSuFwdStats->nBadPackets = piget32(&parmfp);
    pTechHdrSuFwdStats->nTcServedThroughput = piget32(&parmfp);
    pTechHdrSuFwdStats->nTcInstantThroughput = piget32(&parmfp);
    pTechHdrSuFwdStats->nTcAvrgThroughtput = piget32(&parmfp);
}

/**
 *
 * Unpacks the HDR Su Fwd Slot Cnt.
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   pTechHdrSuFwdSlotCnt: [OUT]   HDR Su Fwd Slot Cnt.
 *                                  See SWI_STRUCT_Tech_Hdr_Su_Fwd_Slot_Cnt.
 *
 * \return  none
 *
 */
package void SwiUnpackTechHdrSuFwdSlotCnt(
        swi_uint8 *paramfieldp, 
        SWI_STRUCT_Tech_Hdr_Su_Fwd_Slot_Cnt *pTechHdrSuFwdSlotCnt)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    piget16(&parmfp);    /* Discard obj_version */

    piget32s(pTechHdrSuFwdSlotCnt->nTcDrc0Tsc, 
            NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc0Tsc, &parmfp);

    piget32s(pTechHdrSuFwdSlotCnt->nTcDrc1Tsc, 
            NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc1Tsc, &parmfp);

    piget32s(pTechHdrSuFwdSlotCnt->nTcDrc2Tsc, 
            NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc2Tsc, &parmfp);

    piget32s(pTechHdrSuFwdSlotCnt->nTcDrc3Tsc, 
            NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc3Tsc, &parmfp);

    piget32s(pTechHdrSuFwdSlotCnt->nTcDrc4Tsc, 
            NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc4Tsc, &parmfp);

    piget32s(pTechHdrSuFwdSlotCnt->nTcDrc5Tsc, 
            NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc5Tsc, &parmfp);

    piget32s(pTechHdrSuFwdSlotCnt->nTcDrc6Tsc, 
            NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc6Tsc, &parmfp);

    piget32s(pTechHdrSuFwdSlotCnt->nTcDrc7Tsc, 
            NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc7Tsc, &parmfp);

    piget32s(pTechHdrSuFwdSlotCnt->nTcDrc8Tsc, 
            NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc8Tsc, &parmfp);

    piget32s(pTechHdrSuFwdSlotCnt->nTcDrc9Tsc, 
            NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt_TcDrc9Tsc, &parmfp);
}

/**
 *
 * Unpacks the HDR Su Fwd Slot Cnt2.
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   pTechHdrSuFwdSlotCnt: [OUT]   HDR Su Fwd Slot Cnt2.
 *                                  See SWI_STRUCT_Tech_Hdr_Su_Fwd_Slot_Cnt2.
 *
 * \return  none
 *
 */
package void SwiUnpackTechHdrSuFwdSlotCnt2(
        swi_uint8 *paramfieldp, 
        SWI_STRUCT_Tech_Hdr_Su_Fwd_Slot_Cnt2 *pTechHdrSuFwdSlotCnt2)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    piget16(&parmfp);    /* Discard obj_version */
    
    piget32s(pTechHdrSuFwdSlotCnt2->nTcDrc10Tsc, 
            NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt2_TcDrc10Tsc, &parmfp);

    piget32s(pTechHdrSuFwdSlotCnt2->nTcDrc11Tsc, 
            NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt2_TcDrc11Tsc, &parmfp);

    piget32s(pTechHdrSuFwdSlotCnt2->nTcDrc12Tsc, 
            NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt2_TcDrc12Tsc, &parmfp);

    piget32s(pTechHdrSuFwdSlotCnt2->nTcDrc13Tsc, 
            NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt2_TcDrc13Tsc, &parmfp);

    piget32s(pTechHdrSuFwdSlotCnt2->nTcDrc14Tsc, 
            NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt2_TcDrc14Tsc, &parmfp);

    piget32s(pTechHdrSuFwdSlotCnt2->nCcDrc1Tsc, 
            NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt2_CcDrc1Tsc, &parmfp);

    piget32s(pTechHdrSuFwdSlotCnt2->nCcDrc2Tsc, 
            NUMBER_Tech_Hdr_Su_Fwd_Slot_Cnt2_CcDrc2Tsc, &parmfp);
}

/**
 *
 * Unpacks the HDR Mu Fwd statistics.
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   pTechHdrMuFwdStats: [OUT]   HDR Mu Fwd statistics.
 *                                  See SWI_STRUCT_Tech_Hdr_Mu_Fwd_Stats.
 *
 * \return  none
 *
 */
package void SwiUnpackTechHdrMuFwdStats(
        swi_uint8 *paramfieldp, 
        SWI_STRUCT_Tech_Hdr_Mu_Fwd_Stats *pTechHdrMuFwdStats)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    piget16(&parmfp);    /* Discard obj_version */

    pTechHdrMuFwdStats->nDrc3_128_GoodCrc = piget16(&parmfp);
    pTechHdrMuFwdStats->nDrc3_256_GoodCrc = piget16(&parmfp);
    pTechHdrMuFwdStats->nDrc3_512_GoodCrc = piget16(&parmfp);
    pTechHdrMuFwdStats->nDrc3_1024_GoodCrc = piget16(&parmfp);
    pTechHdrMuFwdStats->nDrc5_2048_GoodCrc = piget16(&parmfp);
    pTechHdrMuFwdStats->nDrc8_3072_GoodCrc = piget16(&parmfp);
    pTechHdrMuFwdStats->nDrc10_4096_GoodCrc = piget16(&parmfp);
    pTechHdrMuFwdStats->nDrc13_5120_GoodCrc = piget16(&parmfp);
    pTechHdrMuFwdStats->nDrc3BadCrc = piget16(&parmfp);
    pTechHdrMuFwdStats->nDrc5BadCrc = piget16(&parmfp);
    pTechHdrMuFwdStats->nDrc8BadCrc = piget16(&parmfp);
    pTechHdrMuFwdStats->nDrc10BadCrc = piget16(&parmfp);
    pTechHdrMuFwdStats->nDrc13BadCrc = piget16(&parmfp);

    piget32s(pTechHdrMuFwdStats->nDrc3_128_Tsc, 
            NUMBER_Tech_Hdr_Mu_Fwd_Stats_Drc3_Tsc, &parmfp);

    piget32s(pTechHdrMuFwdStats->nDrc3_256_Tsc, 
            NUMBER_Tech_Hdr_Mu_Fwd_Stats_Drc3_Tsc, &parmfp);

    piget32s(pTechHdrMuFwdStats->nDrc3_512_Tsc, 
            NUMBER_Tech_Hdr_Mu_Fwd_Stats_Drc3_Tsc, &parmfp);

    piget32s(pTechHdrMuFwdStats->nDrc3_1024_Tsc, 
            NUMBER_Tech_Hdr_Mu_Fwd_Stats_Drc3_Tsc, &parmfp);

    piget32s(pTechHdrMuFwdStats->nDrc5Tsc, 
            NUMBER_Tech_Hdr_Mu_Fwd_Stats_Drc5Tsc, &parmfp);

    piget32s(pTechHdrMuFwdStats->nDrc8Tsc, 
            NUMBER_Tech_Hdr_Mu_Fwd_Stats_Drc8Tsc, &parmfp);

    piget32s(pTechHdrMuFwdStats->nDrc10Tsc, 
            NUMBER_Tech_Hdr_Mu_Fwd_Stats_Drc10Tsc, &parmfp);

    piget32s(pTechHdrMuFwdStats->nDrc13Tsc, 
            NUMBER_Tech_Hdr_Mu_Fwd_Stats_Drc13Tsc, &parmfp);

    pTechHdrMuFwdStats->nTotalPackets = piget16(&parmfp);
    pTechHdrMuFwdStats->nBadPackets = piget16(&parmfp);
    pTechHdrMuFwdStats->nServedThroughput = piget16(&parmfp);
    pTechHdrMuFwdStats->nInstantThroughput = piget16(&parmfp);
    pTechHdrMuFwdStats->nAvrgThroughtput = piget16(&parmfp);
}

/**
 *
 * Unpacks the Network user ID.
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   pszUserID: [OUT]   Network user ID.
 * \param   length: [OUT]   Length of network user ID.
 *
 * \return  See SWI_RCODE.
 *
 */
package SWI_RCODE SwiUnpackNetworkUserID(swi_uint8 *paramfieldp, 
                                         swi_charp pszUserID, 
                                         swi_uint32 length)
{
    SWI_RCODE resultcode; /* Result of call to cnSendWait() */
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    swi_uint16 nlen;

    parmfp = paramfieldp;
    

    piget16(&parmfp);   /* Discard profileType */
    piget16(&parmfp);   /* Discard profileIndex */

    nlen = piget16(&parmfp);
    if(length <= nlen)
    {
        resultcode = SWI_RCODE_SMALL_BUF;
    }
    else
    {
        slmemset((char *)pszUserID, 0, length); 
        pigetmulti(&parmfp, (swi_uint8 *)pszUserID, nlen);
        resultcode = SWI_RCODE_OK;
    }

    return resultcode;
}

/**
 *
 * Unpacks the HDR SCP custom config.
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   pSubtypeCustomConfig: [OUT]   HDR SCP custom config.
 *                                  See SWI_STRUCT_HDRSCP_Subtype_Custom_Config.
 *
 * \return  none
 *
 */
package void SwiUnpackHdrScpCustomerConfig(
        swi_uint8 *paramfieldp, 
        SWI_STRUCT_HDRSCP_Subtype_Custom_Config *pSubtypeCustomConfig)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    piget16(&parmfp);   /* Discard NV index */

    pSubtypeCustomConfig->bIsActive = piget16(&parmfp);
    pSubtypeCustomConfig->nProtSubtypeMask = piget32(&parmfp);
    pSubtypeCustomConfig->nBcmcsSubtypeMask = piget32(&parmfp);
    pSubtypeCustomConfig->nAppSubtypeMask = piget32(&parmfp);
}

/**
 *
 * Unpacks the HDR personality.
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   pTechHdrPersonality: [OUT]   HDR personality.
 *                                  See SWI_STRUCT_Tech_Hdr_Personality.
 *
 * \return  none
 *
 */
package void SwiUnpackTechHdrPersonality(
        swi_uint8 *paramfieldp, 
        SWI_STRUCT_Tech_Hdr_Personality *pTechHdrPersonality)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    piget16(&parmfp);   /* Discard obj_version */

    pTechHdrPersonality->nCurrPersonality = piget16(&parmfp);
    piget16s(pTechHdrPersonality->nSubType, 
            NUMBER_Tech_Hdr_Personality_SubType, &parmfp);
}

/**
 *
 * Unpacks the HDR Prot App subtype.
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   pTechHdrProtAppSubtype: [OUT]   HDR Prot App subtype.
 *                                  See SWI_STRUCT_Tech_Hdr_Prot_App_Subtype.
 *
 * \return  none
 *
 */
package void SwiUnpackTechHdrProtAppSubtype(
        swi_uint8 *paramfieldp, 
        SWI_STRUCT_Tech_Hdr_Prot_App_Subtype *pTechHdrProtAppSubtype)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    swi_uint16 i;
    swi_uint16 j;

    parmfp = paramfieldp;

    piget16(&parmfp);   /* Discard obj_version */
    pTechHdrProtAppSubtype->nCurrPersonality = piget16(&parmfp);
    pTechHdrProtAppSubtype->nStoredPersonalities = piget16(&parmfp);

    for(i = 0; i < CNS_TECH_HDR_MAX_PERSONALITY; i++)
    {
        for(j = 0; j < NUMBER_Tech_Hdr_Prot_SubType; j++)
        {
            pTechHdrProtAppSubtype->nSubType[i][j] = piget16(&parmfp);
        }
    }
    
    for(i = 0; i < NUMBER_Tech_Hdr_Prot_AppSubtype; i++)
    {
        pTechHdrProtAppSubtype->nApplicationSubtype[i] = piget16(&parmfp);
    }
}

/**
 *
 * Unpacks the HDR sleep parameter.
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   pTechHdrSleepParms: [OUT]   HDR sleep parameter.
 *                                  See SWI_STRUCT_Tech_Hdr_Sleep_Parms.
 *
 * \return  none
 *
 */
package void SwiUnpackTechHdrSleepParms(
        swi_uint8 *paramfieldp, 
        SWI_STRUCT_Tech_Hdr_Sleep_Parms *pTechHdrSleepParms)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    swi_uint16 i;

    parmfp = paramfieldp;

    piget16(&parmfp);   /* Discard obj_version */

    pTechHdrSleepParms->nSlotCycle1 = piget16(&parmfp);
    pTechHdrSleepParms->nSlotCycle2 = piget16(&parmfp);
    pTechHdrSleepParms->nSlotCycle3 = piget16(&parmfp);

    for(i = 0; i < NUMBER_Tech_Hdr_Sleep_SlotCycleTimeout1; i++)
    {
        pTechHdrSleepParms->nSlotCycleTimeout1[i]= piget32(&parmfp);
    }

    for(i = 0; i < NUMBER_Tech_Hdr_Sleep_SlotCycleTimeout2; i++)
    {
        pTechHdrSleepParms->nSlotCycleTimeout2[i]= piget32(&parmfp);
    }
}

/**
 *
 * Unpacks the waranty date code.
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   pszWarranty: [OUT]   Waranty date code.
 * \param   lengthWarranty: [OUT]   Length of waranty date code.
 * \param   pszExtra: [OUT]   Extra data.
 * \param   lengthExtra: [OUT]   Length of extra data.
 *
 * \return  none
 *
 */
package SWI_RCODE SwiPstUnpackWarDateCode(swi_uint8 *paramfieldp, 
                                          swi_charp pszWarranty, 
                                          swi_uint32 lengthWarranty, 
                                          swi_charp pszExtra, 
                                          swi_uint32 lengthExtra)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    
    if (lengthWarranty < LENGTH_NvWdcDate || 
            lengthExtra < LENGTH_NvWdcExtra)
    {
        resultcode = SWI_RCODE_SMALL_BUF;
    }
    else
    {
        swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

        parmfp = paramfieldp;

        piget16(&parmfp);   /* Discard NV index */

        slmemset((char *)pszWarranty, 0, lengthWarranty);
        pigetmulti(&parmfp, (swi_uint8 *)pszWarranty, LENGTH_NvWdcDate);

        slmemset((char *)pszExtra, 0, lengthExtra);
        pigetmulti(&parmfp, (swi_uint8 *)pszExtra, LENGTH_NvWdcExtra);
        resultcode = SWI_RCODE_OK;
    }

    return resultcode;
}

/**
 *
 * Unpacks the dial phonebook.
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   status: [OUT]   Status.
 * \param   numberOfDigits: [OUT]   Number of digits.
 * \param   pszDigits: [OUT]   Digits.
 * \param   szDigitsLength: [OUT]   Length of Digits.
 * \param   pszName: [OUT]   Name.
 * \param   szNameLength: [IN]   Length of name.
 *
 * \return  none
 *
 */
package SWI_RCODE SwiPstUnpackDialPhonebook(swi_uint8 *paramfieldp, 
                                            swi_uint16* status, 
                                            swi_uint16* numberOfDigits, 
                                            swi_charp pszDigits, 
                                            swi_uint32 szDigitsLength, 
                                            swi_charp pszName, 
                                            swi_uint32 szNameLength)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    piget16(&parmfp);   /* Discard NV index */
    piget16(&parmfp);   /* Discard address */

    *status = piget16(&parmfp);
    *numberOfDigits = piget16(&parmfp);

    if(*numberOfDigits > LENGTH_NvDialDigits)
    {
        resultcode = SWI_RCODE_UNEXPECTED_RESP;
    }
    else if(*numberOfDigits > szDigitsLength)
    {
        resultcode = SWI_RCODE_SMALL_BUF;
    }
    else
    {
        slmemset((char *)pszDigits, 0, szDigitsLength);
        pigetmulti(&parmfp, (swi_uint8 *)pszDigits, LENGTH_NvDialDigits);

        slmemset((char *)pszName, 0, szNameLength);
        pigetmulti(&parmfp, (swi_uint8 *)pszName, LENGTH_NvDialLetters);
        resultcode = SWI_RCODE_OK;
    }

    return resultcode;
}

/**
 *
 * Unpacks the provision information.
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   nam1ProvFlag: [OUT]   NAM1 provision flag.
 * \param   pszNam1ProvDate: [OUT]   NAM1 provision date.
 * \param   szLengthNam1: [IN]   Length NAM1 provision date.
 * \param   nam2ProvFlag: [OUT]   NAM2 provision flag.
 * \param   pszNam2ProvDate: [OUT]   NAM2 provision date.
 * \param   szLengthNam2: [IN]   Length NAM2 provision date.
 *
 * \return  none
 *
 */
package SWI_RCODE SwiPstUnpackProvisionInfo(swi_uint8 *paramfieldp, 
                                            swi_uint32* nam1ProvFlag, 
                                            swi_charp pszNam1ProvDate, 
                                            swi_uint32 szLengthNam1, 
                                            swi_uint32* nam2ProvFlag, 
                                            swi_charp pszNam2ProvDate, 
                                            swi_uint32 szLengthNam2)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */

    if (szLengthNam1 < LENGTH_NvNam1ProvisionDate ||
            szLengthNam2 < LENGTH_NvNam2ProvisionDate)
    {
        resultcode = SWI_RCODE_SMALL_BUF;
    }
    else
    {
        swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

        parmfp = paramfieldp;

        piget16(&parmfp);   /* Discard NV index */

        *nam1ProvFlag = *parmfp++;
        slmemset((char *)pszNam1ProvDate, 0, szLengthNam1);
        pigetmulti(&parmfp, (swi_uint8 *)pszNam1ProvDate,
                     LENGTH_NvNam1ProvisionDate);

        *nam2ProvFlag = *parmfp++;
        slmemset((char *)pszNam2ProvDate, 0, szLengthNam2);
        pigetmulti(&parmfp, (swi_uint8 *)pszNam2ProvDate,
                         LENGTH_NvNam2ProvisionDate);
        resultcode = SWI_RCODE_OK;
    }
        
    return resultcode;
}

/**
 *
 * Unpacks the Finger.
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   TechFing: [OUT]   Finger.
 *                                  See SWI_STRUCT_TechFinger.
 *
 * \return  See SWI_RCODE
 *
 */
package SWI_RCODE SwiUnpackTechFinger(swi_uint8 *paramfieldp, 
                                      SWI_STRUCT_TechFinger* TechFing)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    swi_uint16 i;
    swi_uint16 j;

    parmfp = paramfieldp;

    TechFing->nCount = piget16(&parmfp);
    if(TechFing->nCount > MAX_FingerSet)
    {
        resultcode = SWI_RCODE_SMALL_BUF;
    }
    else
    {
        for(i = 0; i < TechFing->nCount; i++)
        {
            TechFing->FingerSet[i].iPilot = (swi_int32)piget16(&parmfp);
            for(j = 0; j < MAX_WalshCode; j++)
            {
                TechFing->FingerSet[i].nWalshCode[j] = piget16(&parmfp);
            }
            TechFing->FingerSet[i].iRssi = (swi_int32)piget16(&parmfp);
        }
        resultcode = SWI_RCODE_OK;
    }
        
    return resultcode;
}

/**
 *
 * Unpacks the E911 information.
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   startLocation: [OUT]   Start location.
 * \param   endLocation: [OUT]   End location.
 * \param   programMask: [OUT]   Program mask.
 *
 * \return  None.
 *
 */
package void SwiUnpackE911Info(swi_uint8 *paramfieldp, 
                               swi_uint16* startLocation, 
                               swi_uint16* endLocation, 
                               swi_uint16* programMask)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    *startLocation = piget16(&parmfp);
    *endLocation = piget16(&parmfp);
    *programMask = piget16(&parmfp);
}

/**
 *
 * Unpacks the E911 information.
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   errFatalFlag: [OUT]   fatal error flag.
 * \param   errCount: [OUT]   Error count.
 * \param   errFilename: [OUT]   Error file name.
 * \param   length: [IN]   Error file name length.
 * \param   errLineNumber: [OUT]   Error line number.
 *
 * \return  See SWI_RCODE.
 *
 */
package SWI_RCODE SwiUnpackTechErrorStat(swi_uint8 *paramfieldp, 
                                         swi_uint8* errFatalFlag, 
                                         swi_uint8* errCount, 
                                         swi_charp errFilename, 
                                         swi_uint32 length, 
                                         swi_uint16* errLineNumber)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */

    if(length < LENGTH_TechErrFileName)
    {
        resultcode = SWI_RCODE_SMALL_BUF;
    }
    else
    {
        swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

        parmfp = paramfieldp;

        piget16(&parmfp);   /* Discard errSeqNumber */

        *errFatalFlag = *parmfp++;
        *errCount = *parmfp++;

        slmemset((char *)errFilename, 0, length);
        pigetmulti(&parmfp, (swi_uint8 *)errFilename, LENGTH_TechErrFileName);

        *errLineNumber = piget16(&parmfp);
        resultcode = SWI_RCODE_OK;
    }
        
    return resultcode;
}

/**
 *
 * Unpacks the MIP SS user prof.
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param mnHaSs[OUT]           Mn HA SS.
 * \param pmnHaSsLength[OUT]    Mn HA SS length. 
 * \param mnHaSsBufLength[IN]   Size off buffer that holds Mn HA SS. 
 * \param mnAaaSs[OUT]           Mn AAA SS.
 * \param pmnAaaSsLength[OUT]    Mn AAA SS length. 
 * \param mnAaaSsBufLength[IN]   Size off buffer that holds Mn AAA SS. 
 *
 * \return  See SWI_RCODE.
 *
 */
package SWI_RCODE SwiPstUnpackDsMipSsUserProf(swi_uint8 *paramfieldp, 
                                              swi_charp mnHaSs, 
                                              swi_uint16* pmnHaSsLength, 
                                              swi_uint16 mnHaSsBufLength, 
                                              swi_charp mnAaaSs, 
                                              swi_uint16* pmnAaaSsLength, 
                                              swi_uint16 mnAaaSsBufLength)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;
    piget16(&parmfp);   /* Discard NV index */
    piget16(&parmfp);   /* index */

    *pmnHaSsLength = piget16(&parmfp);
    if(*pmnHaSsLength > LENGTH_NvMnAaaSs)
    {
        resultcode = SWI_RCODE_UNEXPECTED_RESP;
    }
    else if(*pmnHaSsLength > mnHaSsBufLength)
    {
        resultcode = SWI_RCODE_SMALL_BUF;
    }
    else
    {
        slmemset((char *)mnHaSs, 0, mnHaSsBufLength);
        pigetmulti(&parmfp, (swi_uint8 *)mnHaSs, *pmnHaSsLength);
        parmfp += LENGTH_NvMnAaaSs-(*pmnHaSsLength);

        *pmnAaaSsLength = piget16(&parmfp);
        if(*pmnAaaSsLength > LENGTH_NvMnAaaSs)
        {
            resultcode = SWI_RCODE_UNEXPECTED_RESP;
        }
        else if(*pmnAaaSsLength > mnAaaSsBufLength)
        {
            resultcode = SWI_RCODE_SMALL_BUF;
        }
        else
        {
            slmemset((char *)mnAaaSs, 0, mnAaaSsBufLength);
            pigetmulti(&parmfp, (swi_uint8 *)mnAaaSs, *pmnAaaSsLength);
            resultcode = SWI_RCODE_OK;
        }
    }
        
    return resultcode;
}

/**
 *
 * Unpacks the MIP Gen user bak.
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param naiLength[IN]           length of NAI.
 * \param nai[OUT]    NAI. 
 * \param pMnHaSpiEnable[OUT]   Mn HA SPI enabled. 
 * \param pMnHaSpi[OUT]           Mn HA SPI.
 * \param pMnAaaSpiEnable[OUT]   Mn AAA SPI enabled. 
 * \param pMnAaaSpi[OUT]           Mn AAA SPI.
 * \param pReverseTunneling[OUT]   Reverse tunneling. 
 * \param pHomeAddress[OUT]   Home address. 
 * \param pPrimaryHaAddress[OUT]   Primary HA address. 
 * \param pSecondaryHaAddress[OUT]   Second HA address. 
 *
 * \return  See SWI_RCODE.
 *
 */
package SWI_RCODE SwiPstUnpackDsMipGenUserBak(swi_uint8 *paramfieldp, 
                                              swi_uint16 naiLength, 
                                              swi_charp nai, 
                                              swi_uint16* pMnHaSpiEnable, 
                                              swi_uint32* pMnHaSpi, 
                                              swi_uint16* pMnAaaSpiEnable, 
                                              swi_uint32* pMnAaaSpi, 
                                              swi_uint16* pReverseTunneling, 
                                              swi_uint32* pHomeAddress, 
                                              swi_uint32* pPrimaryHaAddress, 
                                              swi_uint32* pSecondaryHaAddress)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    swi_uint16 nReceivedNaiLength;

    parmfp = paramfieldp;
    piget16(&parmfp);   /* Discard NV index */
    piget16(&parmfp);   /* index */

    nReceivedNaiLength = piget16(&parmfp);
    if(nReceivedNaiLength > LENGTH_NvNai)
    {
        resultcode = SWI_RCODE_UNEXPECTED_RESP;
    }
    else if(nReceivedNaiLength > naiLength)
    {
        resultcode = SWI_RCODE_SMALL_BUF;
    }
    else
    {
        slmemset((char *)nai, 0, naiLength);
        pigetmulti(&parmfp, (swi_uint8 *)nai, nReceivedNaiLength);
        parmfp += LENGTH_NvNai-nReceivedNaiLength;

        *pMnHaSpiEnable = piget16(&parmfp);
        *pMnHaSpi = piget32(&parmfp);
        *pMnAaaSpiEnable = piget16(&parmfp);
        *pMnAaaSpi = piget32(&parmfp);
        *pReverseTunneling = piget16(&parmfp);
        *pHomeAddress = piget32(&parmfp);
        *pPrimaryHaAddress = piget32(&parmfp);
        *pSecondaryHaAddress = piget32(&parmfp);

        resultcode = SWI_RCODE_OK;
    }

    return resultcode;
}

/**
 *
 * Unpacks the MIP SS  user bak.
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param mnHaSsLength[IN]  Size of the buffer that holds HA SS.
 * \param mnHaSs[OUT]    HA SS. 
 * \param mnAaaSsLength[IN]  Size of the buffer that holds AAA SS.
 * \param mnAaaSs[OUT]    AAA SS. 
 *
 * \return  See SWI_RCODE.
 *
 */
package SWI_RCODE SwiPstUnpackDsMipSsUserBak(swi_uint8 *paramfieldp, 
                                             swi_uint16 mnHaSsLength, 
                                             swi_charp mnHaSs, 
                                             swi_uint16 mnAaaSsLength, 
                                             swi_charp mnAaaSs)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    swi_uint16 nLength;

    parmfp = paramfieldp;
    piget16(&parmfp);   /* Discard NV index */
    piget16(&parmfp);   /* index */

    nLength = piget16(&parmfp);
    if(nLength > LENGTH_NvMnAaaSs)
    {
        resultcode = SWI_RCODE_UNEXPECTED_RESP;
    }
    else if(nLength > mnHaSsLength)
    {
        resultcode = SWI_RCODE_SMALL_BUF;
    }
    else
    {
        slmemset((char *)mnHaSs, 0, mnHaSsLength);
        pigetmulti(&parmfp, (swi_uint8 *)mnHaSs, nLength);
        parmfp += LENGTH_NvMnAaaSs-nLength;

        nLength = piget16(&parmfp);
        if(nLength > LENGTH_NvMnAaaSs)
        {
            resultcode = SWI_RCODE_UNEXPECTED_RESP;
        }
        else if(nLength > mnAaaSsLength)
        {
            resultcode = SWI_RCODE_SMALL_BUF;
        }
        else
        {
            slmemset((char *)mnAaaSs, 0, mnAaaSsLength);
            pigetmulti(&parmfp, (swi_uint8 *)mnAaaSs, nLength);
            resultcode = SWI_RCODE_OK;
        }
    }
        
    return resultcode;
}

/**
 *
 * Unpacks EFile information.
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   psEFileInfo: [OUT]   EFile information.
 *                                  See SWI_STRUCT_EFileInfo.
 *
 * \return  None
 *
 */
package void SwiUnpackEFileInfo(swi_uint8 *paramfieldp, 
                                SWI_STRUCT_EFileInfo* psEFileInfo)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    psEFileInfo->nResult = piget16(&parmfp);
    psEFileInfo->nSize = piget32(&parmfp);
    psEFileInfo->nBlocks = piget16(&parmfp);
}

/**
 *
 * Unpacks PRL size.
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   NumBlocks: [OUT]   Number of blocks.
 * \param   Size: [OUT]   PRL size.
 *
 * \return  None
 *
 */
package void SwiUnpackPRLSize(swi_uint8 *paramfieldp, 
                              swi_uint16 *NumBlocks, 
                              swi_uint32 *Size)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;
    
    *NumBlocks = piget16(&parmfp);
    *Size = piget32(&parmfp);
}

/**
 *
 * Unpacks EVDO UATI.
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   lUati: [OUT]   UATI.
 * \param   Size: [OUT]   UATI size.
 *
 * \return  See SWI_RCODE
 *
 */
package SWI_RCODE SwiEVDOUnpackUati(swi_uint8 *paramfieldp, 
                                    swi_uint8* lUati, 
                                    swi_uint32 Size)
{
    SWI_RCODE resultcode; /* Result of call to cnSendWait() */

    if(SWI_EVDO_UATI_SIZE > Size)
    {
        resultcode = SWI_RCODE_SMALL_BUF;
    }
    else
    {
        swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
        swi_uint8 i;

        parmfp = paramfieldp;
        
        for(i = 0; i < SWI_EVDO_UATI_SIZE; i++)
        {
            *lUati++ = *parmfp++;
        }
        
        resultcode = SWI_RCODE_OK;
    }

    return resultcode;
}

/**
 *
 * Unpacks data statistics reset information.
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   pResetResult: [OUT]   data statistics reset information.
 *                                  See SWI_STRUCT_DataStatsReset.
 *
 * \return  None
 *
 */
package void SwiUnpackDataStatsReset(swi_uint8 *paramfieldp, 
                                     SWI_STRUCT_DataStatsReset *pResetResult)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    piget16(&parmfp);   /* Discard obj_version */

    pResetResult->result = *parmfp;
}

/**
 *
 * Unpacks general PPP status.
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   pPppStatusGeneral: [OUT]   general PPP status.
 *                                  See SWI_STRUCT_PppGeneralStats.
 *
 * \return  None
 *
 */
local void SwiUnpackPPPStatusGeneral(
        swi_uint8 *paramfieldp, 
        SWI_STRUCT_PppGeneralStats *pPppStatusGeneral)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    pPppStatusGeneral->bad_fcs = piget32(&parmfp);
    pPppStatusGeneral->bad_proto = piget32(&parmfp);
    pPppStatusGeneral->unsupported_proto = piget32(&parmfp);
    pPppStatusGeneral->not_enabled_proto = piget32(&parmfp);
    pPppStatusGeneral->missing_addr = piget32(&parmfp);
    pPppStatusGeneral->missing_ctrl = piget32(&parmfp);
    pPppStatusGeneral->pkts_dropped_rx = piget32(&parmfp);
    pPppStatusGeneral->pkts_rx = piget32(&parmfp);
    pPppStatusGeneral->framed_bytes_rx = piget64(&parmfp);
    pPppStatusGeneral->unframed_bytes_rx = piget64(&parmfp);
    pPppStatusGeneral->pkts_dropped_tx = piget32(&parmfp);
    pPppStatusGeneral->pkts_tx = piget32(&parmfp);
    pPppStatusGeneral->framed_bytes_tx = piget64(&parmfp);
    pPppStatusGeneral->unframed_bytes_tx = piget64(&parmfp);
    pPppStatusGeneral->ipv4_pkts_rx = piget32(&parmfp);
    pPppStatusGeneral->ipv4_pkts_tx = piget32(&parmfp);
}

/**
 *
 * Unpacks VJ compression PPP status.
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   pPppStatusVj: [OUT]   VJ compression PPP status.
 *                                  See SWI_STRUCT_PppVjCompressionStats.
 *
 * \return  None
 *
 */
local void SwiUnpackPppStatusVjCompression(
        swi_uint8 *paramfieldp, 
        SWI_STRUCT_PppVjCompressionStats *pPppStatusVj)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    pPppStatusVj->vjch_pkts_rx = piget32(&parmfp);
    pPppStatusVj->vjch_pkts_tx = piget32(&parmfp);
    pPppStatusVj->vjuch_pkts_rx = piget32(&parmfp);
    pPppStatusVj->vjuch_pkts_tx = piget32(&parmfp);
}

/**
 *
 * Unpacks authentication PPP status.
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   pPppStatusAuth: [OUT]   Authentication PPP status.
 *                                  See SWI_STRUCT_PppAuthStats.
 *
 * \return  None
 *
 */
local void SwiUnpackPppAuth(swi_uint8 *paramfieldp, 
                            SWI_STRUCT_PppAuthStats *pPppStatusAuth)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    pPppStatusAuth->pap_pkts_rx = piget32(&parmfp);
    pPppStatusAuth->pap_pkts_tx = piget32(&parmfp);
    pPppStatusAuth->pap_auth_failures = piget32(&parmfp);
    pPppStatusAuth->chap_pkts_rx = piget32(&parmfp);
    pPppStatusAuth->chap_pkts_tx = piget32(&parmfp);
    pPppStatusAuth->chap_auth_failures = piget32(&parmfp);
}

/**
 *
 * Unpacks LCP PPP status.
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   pPppStatusLcp: [OUT]   LCP PPP status.
 *                                  See SWI_STRUCT_PppLcpStats.
 *
 * \return  None
 *
 */
local void SwiUnpackPppStatusLcp(swi_uint8 *paramfieldp, 
                                 SWI_STRUCT_PppLcpStats *pPppStatusLcp)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    pPppStatusLcp->lcp_pkts_rx = piget32(&parmfp);
    pPppStatusLcp->lcp_pkts_tx = piget32(&parmfp);
    pPppStatusLcp->lcp_config_requests_rx = piget32(&parmfp);
    pPppStatusLcp->lcp_config_acks_rx = piget32(&parmfp);
    pPppStatusLcp->lcp_config_naks_rx = piget32(&parmfp);
    pPppStatusLcp->lcp_config_rejects_rx = piget32(&parmfp);
    pPppStatusLcp->lcp_term_requests_rx = piget32(&parmfp);
    pPppStatusLcp->lcp_term_acks_rx = piget32(&parmfp);
    pPppStatusLcp->lcp_code_rejects_rx = piget32(&parmfp);
    pPppStatusLcp->lcp_proto_rejects_rx = piget32(&parmfp);
    pPppStatusLcp->lcp_echo_requests_rx = piget32(&parmfp);
    pPppStatusLcp->lcp_echo_replies_rx = piget32(&parmfp);
    pPppStatusLcp->lcp_discard_requests_rx = piget32(&parmfp);
    pPppStatusLcp->lcp_identification_rx = piget32(&parmfp);
    pPppStatusLcp->lcp_time_remaining_rx = piget32(&parmfp);
    pPppStatusLcp->lcp_config_requests_tx = piget32(&parmfp);
    pPppStatusLcp->lcp_config_acks_tx = piget32(&parmfp);
    pPppStatusLcp->lcp_config_naks_tx = piget32(&parmfp);
    pPppStatusLcp->lcp_config_rejects_tx = piget32(&parmfp);
    pPppStatusLcp->lcp_term_requests_tx = piget32(&parmfp);
    pPppStatusLcp->lcp_term_acks_tx = piget32(&parmfp);
    pPppStatusLcp->lcp_code_rejects_tx = piget32(&parmfp);
    pPppStatusLcp->lcp_proto_rejects_tx = piget32(&parmfp);
    pPppStatusLcp->lcp_echo_requests_tx = piget32(&parmfp);
    pPppStatusLcp->lcp_echo_replies_tx = piget32(&parmfp);
    pPppStatusLcp->lcp_discard_requests_tx = piget32(&parmfp);
}

/**
 *
 * Unpacks IPCP PPP status.
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   pPppStatusIpcp: [OUT]   IPCP PPP status.
 *                                  See SWI_STRUCT_PppIpcpStats.
 *
 * \return  None
 *
 */
local void SwiUnpackPppStatusIpcp(swi_uint8 *paramfieldp, 
                                  SWI_STRUCT_PppIpcpStats *pPppStatusIpcp)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    pPppStatusIpcp->ipcp_pkts_rx = piget32(&parmfp);
    pPppStatusIpcp->ipcp_pkts_tx = piget32(&parmfp);
    pPppStatusIpcp->ipcp_config_requests_rx = piget32(&parmfp);
    pPppStatusIpcp->ipcp_config_acks_rx = piget32(&parmfp);
    pPppStatusIpcp->ipcp_config_naks_rx = piget32(&parmfp);
    pPppStatusIpcp->ipcp_config_rejects_rx = piget32(&parmfp);
    pPppStatusIpcp->ipcp_term_requests_rx = piget32(&parmfp);
    pPppStatusIpcp->ipcp_term_acks_rx = piget32(&parmfp);
    pPppStatusIpcp->ipcp_code_rejects_rx = piget32(&parmfp);
    pPppStatusIpcp->ipcp_config_requests_tx = piget32(&parmfp);
    pPppStatusIpcp->ipcp_config_acks_tx = piget32(&parmfp);
    pPppStatusIpcp->ipcp_config_naks_tx = piget32(&parmfp);
    pPppStatusIpcp->ipcp_config_rejects_tx = piget32(&parmfp);
    pPppStatusIpcp->ipcp_term_requests_tx = piget32(&parmfp);
    pPppStatusIpcp->ipcp_term_acks_tx = piget32(&parmfp);
    pPppStatusIpcp->ipcp_code_rejects_tx = piget32(&parmfp);
}

/**
 *
 * Unpacks PPP status.
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   pPppStatus: [OUT]   PPP status.
 *                                  See SWI_STRUCT_PppStats.
 *
 * \return  None
 *
 */
local void SwiUnpackPppStatus(swi_uint8 *paramfieldp, 
                              SWI_STRUCT_PppStats *pPppStatus)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    pPppStatus->ppp_sub_protocol = *parmfp++;
    switch(pPppStatus->ppp_sub_protocol)
    {
        case CNS_PPP_GENERAL:
            SwiUnpackPPPStatusGeneral(parmfp, 
                    &pPppStatus->sub_protocol.general);
            break;
        case CNS_PPP_VJ_COMPRESSION:
            SwiUnpackPppStatusVjCompression(parmfp, 
                    &pPppStatus->sub_protocol.vj);
            break;
        case CNS_PPP_AUTH:
            SwiUnpackPppAuth(parmfp, &pPppStatus->sub_protocol.auth);
            break;
        case CNS_PPP_LCP:
            SwiUnpackPppStatusLcp(parmfp, &pPppStatus->sub_protocol.lcp);
            break;
        case CNS_PPP_IPCP:
            SwiUnpackPppStatusIpcp(parmfp, &pPppStatus->sub_protocol.ipcp);
            break;
        default:
            /* Can't go here */
            break;
    }
}

/**
 *
 * Unpacks IPV4 status.
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   pIpv4Status: [OUT]   IPV4 status.
 *                                  See SWI_STRUCT_Ipv4Stats.
 *
 * \return  None
 *
 */
local void SwiUnpackIpv4Status(swi_uint8 *paramfieldp, 
                               SWI_STRUCT_Ipv4Stats *pIpv4Status)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    pIpv4Status->bad_hdr_len = piget32(&parmfp);
    pIpv4Status->bad_len = piget32(&parmfp);
    pIpv4Status->bad_options = piget32(&parmfp);
    pIpv4Status->bad_version = piget32(&parmfp);
    pIpv4Status->bad_chksum = piget32(&parmfp);
    pIpv4Status->too_short = piget32(&parmfp);
    pIpv4Status->no_route = piget32(&parmfp);
    pIpv4Status->no_proto = piget32(&parmfp);
    pIpv4Status->pkts_rx = piget32(&parmfp);
    pIpv4Status->pkts_dropped_rx = piget32(&parmfp);
    pIpv4Status->pkts_dropped_tx = piget32(&parmfp);
    pIpv4Status->pkts_fwd = piget32(&parmfp);
    pIpv4Status->pkts_tx = piget32(&parmfp);
    pIpv4Status->mcast_rx = piget32(&parmfp);
    pIpv4Status->mcast_tx = piget32(&parmfp);
    pIpv4Status->frag_ok = piget32(&parmfp);
    pIpv4Status->frag_fails = piget32(&parmfp);
    pIpv4Status->frag_create = piget32(&parmfp);
    pIpv4Status->reasm_reqd = piget32(&parmfp);
    pIpv4Status->reasm_ok = piget32(&parmfp);
    pIpv4Status->reasm_fails = piget32(&parmfp);
    pIpv4Status->reasm_timeout = piget32(&parmfp);
}

/**
 *
 * Unpacks ICMPV4 status.
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   pIcmpv4Status: [OUT]   ICMPV4 status.
 *                                  See SWI_STRUCT_Icmpv4Stats.
 *
 * \return  None
 *
 */
local void SwiUnpackIcmpv4Status(swi_uint8 *paramfieldp, 
                                 SWI_STRUCT_Icmpv4Stats *pIcmpv4Status)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    pIcmpv4Status->msgs_rx = piget32(&parmfp);
    pIcmpv4Status->msgs_dropped_rx = piget32(&parmfp);
    pIcmpv4Status->addr_mask_replies_rx = piget32(&parmfp);
    pIcmpv4Status->addr_masks_rx = piget32(&parmfp);
    pIcmpv4Status->dest_unreach_rx = piget32(&parmfp);
    pIcmpv4Status->echo_rx = piget32(&parmfp);
    pIcmpv4Status->echo_reply_rx = piget32(&parmfp);
    pIcmpv4Status->bad_chksum_rx = piget32(&parmfp);
    pIcmpv4Status->unsupported_type_rx = piget32(&parmfp);
    pIcmpv4Status->parm_probs_rx = piget32(&parmfp);
    pIcmpv4Status->bad_code_rx = piget32(&parmfp);
    pIcmpv4Status->redirects_rx = piget32(&parmfp);
    pIcmpv4Status->src_quench_rx = piget32(&parmfp);
    pIcmpv4Status->time_exceeds_rx = piget32(&parmfp);
    pIcmpv4Status->tstamp_reply_rx = piget32(&parmfp);
    pIcmpv4Status->tstamp_rx = piget32(&parmfp);
    pIcmpv4Status->msgs_tx = piget32(&parmfp);
    pIcmpv4Status->addr_mask_replies_tx = piget32(&parmfp);
    pIcmpv4Status->addr_masks_tx = piget32(&parmfp);
    pIcmpv4Status->dest_unreach_tx = piget32(&parmfp);
    pIcmpv4Status->echo_tx = piget32(&parmfp);
    pIcmpv4Status->echo_reply_tx = piget32(&parmfp);
    pIcmpv4Status->parm_probs_tx = piget32(&parmfp);
    pIcmpv4Status->redirects_tx = piget32(&parmfp);
    pIcmpv4Status->src_quench_tx = piget32(&parmfp);
    pIcmpv4Status->time_exceeds_tx = piget32(&parmfp);
    pIcmpv4Status->tstamp_reply_tx = piget32(&parmfp);
    pIcmpv4Status->tstamp_tx = piget32(&parmfp);
    pIcmpv4Status->rate_limit_tx = piget32(&parmfp);
    pIcmpv4Status->msgs_dropped_tx = piget32(&parmfp);
}

/**
 *
 * Unpacks TCP status.
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   pTcpStatus: [OUT]   TCP status.
 *                                  See SWI_STRUCT_TcpStats.
 *
 * \return  None
 *
 */
local void SwiUnpackTcpStatus(swi_uint8 *paramfieldp, 
                              SWI_STRUCT_TcpStats *pTcpStatus)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    pTcpStatus->crnt_open_conns = piget32(&parmfp);
    pTcpStatus->bad_hdr_len = piget32(&parmfp);
    pTcpStatus->bad_chksum = piget32(&parmfp);
    pTcpStatus->too_short = piget32(&parmfp);
    pTcpStatus->conn_aborts = piget32(&parmfp);
    pTcpStatus->conn_timeout_aborts = piget32(&parmfp);
    pTcpStatus->conn_keepalive_aborts = piget32(&parmfp);
    pTcpStatus->rst_tx = piget32(&parmfp);
    pTcpStatus->rst_rx = piget32(&parmfp);
    pTcpStatus->dropped_segs_noconn_rx = piget32(&parmfp);
    pTcpStatus->dropped_segs_rx = piget32(&parmfp);
    pTcpStatus->bytes_tx = piget64(&parmfp);
    pTcpStatus->bytes_rx = piget64(&parmfp);
    pTcpStatus->segs_tx = piget32(&parmfp);
    pTcpStatus->segs_rx = piget32(&parmfp);
    pTcpStatus->bytes_rexmit = piget64(&parmfp);
    pTcpStatus->segs_rexmit = piget32(&parmfp);
    pTcpStatus->dup_bytes_rx = piget32(&parmfp);
    pTcpStatus->dup_segs_rx = piget32(&parmfp);
    pTcpStatus->window_probe_tx = piget32(&parmfp);
    pTcpStatus->out_of_order_segs_rx = piget32(&parmfp);
    pTcpStatus->out_of_order_bytes_rx = piget32(&parmfp);
    pTcpStatus->rexmit_timeouts = piget32(&parmfp);
    pTcpStatus->piggybacked_acks_tx = piget32(&parmfp);
    pTcpStatus->piggybacked_acks_rx = piget32(&parmfp);
    pTcpStatus->dup_acks_rx = piget32(&parmfp);
    pTcpStatus->dup_acks_tx = piget32(&parmfp);
}

/**
 *
 * Unpacks UDP status.
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   pUdpStatus: [OUT]   UDP status.
 *                                  See SWI_STRUCT_UdpStats.
 *
 * \return  None
 *
 */
local void SwiUnpackUdpStatus(swi_uint8 *paramfieldp, 
                              SWI_STRUCT_UdpStats *pUdpStatus)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    pUdpStatus->bad_len = piget32(&parmfp);
    pUdpStatus->bad_chksum = piget32(&parmfp);
    pUdpStatus->dgram_rx = piget32(&parmfp);
    pUdpStatus->dgram_tx = piget32(&parmfp);
    pUdpStatus->bytes_rx = piget64(&parmfp);
    pUdpStatus->bytes_tx = piget64(&parmfp);
    pUdpStatus->no_port = piget32(&parmfp);
    pUdpStatus->dgram_dropped_rx = piget32(&parmfp);
    pUdpStatus->dgram_dropped_tx = piget32(&parmfp);
}

/**
 *
 * Unpacks data statistics fetch information.
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   pUdpStatus: [OUT]   Data statistics fetch information.
 *                                  See SWI_STRUCT_DataStatsFetch.
 *
 * \return  None
 *
 */
package void SwiUnpackDataStatsFetch(swi_uint8 *paramfieldp, 
                                     SWI_STRUCT_DataStatsFetch *pDataStats)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    piget16(&parmfp);   /* Discard obj_version */

    pDataStats->status = *parmfp++;
    if(CNS_DATA_STATS_FETCH_SUCCESS == pDataStats->status)
    {
        pDataStats->protocol = *parmfp++;
        switch(pDataStats->protocol)
        {
            case CNS_PPP_OVER_UM:
            case CNS_PPP_OVER_RM:
                SwiUnpackPppStatus(parmfp, &pDataStats->protocol_union.ppp);
                break;
            case CNS_IPV4_OVER_UM:
                SwiUnpackIpv4Status(parmfp, &pDataStats->protocol_union.ipv4);
                break;
            case CNS_ICMPV4_OVER_UM:
                SwiUnpackIcmpv4Status(parmfp, 
                        &pDataStats->protocol_union.icmpv4);
                break;
            case CNS_TCP_OVER_UM:
                SwiUnpackTcpStatus(parmfp, &pDataStats->protocol_union.tcp);
                break;
            case CNS_UDP_OVER_UM:
                SwiUnpackUdpStatus(parmfp, &pDataStats->protocol_union.udp);
                break;
            default:
                /* Can't go here */
                break;
        }
    }
}

/**
 *
 * Unpacks auto populate information.
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   pData: [OUT]   Auto populate information.
 *                                  See nv_auto_populate_info_ap_type.
 *
 * \return  None
 *
 */
package void SwiPstUnpackAutoPopulateInfoAp(
        swi_uint8 *paramfieldp, 
        nv_auto_populate_info_ap_type *pData)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    piget16(&parmfp);   /* Discard NV index */

    pData->index = piget16(&parmfp);
    pData->duration = piget16(&parmfp);
    pData->trigger = piget16(&parmfp);
    pigetmulti(&parmfp, (swi_uint8 *)pData->host_indicators, 4);
}

/**
 *
 * Unpacks EFile block information.
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   nSeq: [OUT]   Sequence number.
 * \param   pBuffer: [OUT]   Buffer to hold EFile blcok information.
 * \param   nBufLen: [IN]   Buffer length.
 * \param   pnBlockSize: [OUT]   Block size.
 *
 * \return  See SWI_RCODE
 *
 */
package SWI_RCODE SwiUnpackEFileBlock(swi_uint8 *paramfieldp, 
                                      swi_uint16 nSeq, 
                                      swi_uint8* pBuffer, 
                                      swi_uint16 nBufLen, 
                                      swi_uint32 *pnBlockSize)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    SWI_RCODE resultcode; /* Result of call to cnSendWait() */
    swi_uint16 nResult;
    swi_uint16 nBlockSeq;
    swi_uint32 nBlockSize;

    parmfp = paramfieldp;

    nResult = piget16(&parmfp);
    parmfp += LENGTH_EFilename;   /* Skip file name*/
    nBlockSeq = piget16(&parmfp);
    nBlockSize = piget32(&parmfp);
    if (1           != nResult    ||
        nSeq        != nBlockSeq  ||
        MAX_EFileBlock < nBlockSize ||
        nBufLen < nBlockSize)
    {
        resultcode = SWI_RCODE_FAILED;
    }
    else
    {
        pigetmulti(&parmfp, (swi_uint8 *)pBuffer, nBlockSize);
        *pnBlockSize = nBlockSize;
        resultcode = SWI_RCODE_OK;
    }

    return resultcode;
}

/* 
 * $Log: SwiIntUpkCdmaInternal.c,v $
 */

