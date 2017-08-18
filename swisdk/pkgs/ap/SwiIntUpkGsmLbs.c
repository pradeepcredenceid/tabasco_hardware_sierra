/**
 *
 * $Id: SwiIntUpkGsmLbs.c,v 1.3 2010/06/10 00:25:24 epasheva Exp $
 *
 * Filename: SwiIntUpkGsmLbs.c
 *
 * Purpose:  Contains Unpacking routines for CnS objects in the 
 *           GSM Location Based Services category
 *
 * Copyright: © 2008 Sierra Wireless, Inc. all rights reserved
 *
 */
#include "apidefs.h"
#include "sl/sludefs.h"

/* CNS_LBS_NI_REQUEST constants */
#define SWIREQIDMAXLEN      64
#define SWICLIENTNAMEMAXLEN 64

/**
 *
 * This function unpacks the LbsPaWarmStatus get response.
 *
 * \param      paramfieldp  - Pointer to the CnS parameter field 
 * \param      warmp        - Pointer to storage into which the LBS 
 *                            warm status will be placed by this 
 *                            function.
 *
 */
package void SwiUpkLbsPaWarmStatus(
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_LbsPaWarmStatus *warmp)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    swi_uint32 sizeStruct;
    swi_uint16 objversion;

    /* get a local copy of the pointer */
    parmfp = paramfieldp;

    /* clear return structure memory before unpacking into it */
    sizeStruct = warmp->sizeStruct;
    slmemset((char *)warmp, 0, sizeStruct);
    warmp->sizeStruct = sizeStruct;
  
    /* get object version of the received packet */
    objversion = piget16(&parmfp);

    if (objversion >= APOBJVER1)
    {
        warmp->bEnabled = piget16(&parmfp);
        warmp->eStatus = piget16(&parmfp);
    } 
}
/**
 *
 * This function unpacks the LbsFixSettings get response.
 *
 * \param   paramfieldp   - Pointer to the CnS parameter field 
 * \param   settingsp     - Pointer to storage into which the LBS 
 *                          fix settings will be placed by this 
 *                          function.
 *
 */
package void SwiUpkLbsFixSettingsGetResp(
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_LbsFixSettings *settingsp)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    swi_uint32 sizeStruct;
    swi_uint16 objversion;
    swi_uint8 usemask;

    /* get a local copy of the pointer */
    parmfp = paramfieldp;

    /* clear return structure memory before unpacking into it */
    sizeStruct = settingsp->sizeStruct;
    slmemset((char *)settingsp, 0, sizeStruct);
    settingsp->sizeStruct = sizeStruct;
  
    /* get object version of the received packet */
    objversion = piget16(&parmfp);

    if (objversion >= APOBJVER1)
    {
        /* get fix type setting user mask */
        usemask = *parmfp++;
        if(usemask & SWIREADMASK)
            settingsp->sFixTypeUserOp.bReadable = TRUE;
        if(usemask & SWIWRITEMASK)
            settingsp->sFixTypeUserOp.bWritable = TRUE;

        /* get fix type setting */
        settingsp->eFixType = piget16(&parmfp);

        /* get performance setting user mask */
        usemask = *parmfp++;
        if(usemask & SWIREADMASK)
            settingsp->sPerformanceUserOp.bReadable = TRUE;
        if(usemask & SWIWRITEMASK)
            settingsp->sPerformanceUserOp.bWritable = TRUE;

        /* get performance setting */
        settingsp->nPerformance= piget16(&parmfp);

        /* get accuracy user mask */
        usemask = *parmfp++;
        if(usemask & SWIREADMASK)
            settingsp->sAccuracyUserOp.bReadable = TRUE;
        if(usemask & SWIWRITEMASK)
            settingsp->sAccuracyUserOp.bWritable = TRUE;

        /* get accuracy setting */
        settingsp->nAccuracy= piget32(&parmfp);

        /* get fix count setting user mask */
        usemask = *parmfp++;
        if(usemask & SWIREADMASK)
            settingsp->sFixCountUserOp.bReadable = TRUE;
        if(usemask & SWIWRITEMASK)
            settingsp->sFixCountUserOp.bWritable = TRUE;

        /* get fix count setting */
        settingsp->nFixCount= piget16(&parmfp);

        /* get fix rate setting user mask */
        usemask = *parmfp++;
        if(usemask & SWIREADMASK)
            settingsp->sFixRateUserOp.bReadable = TRUE;
        if(usemask & SWIWRITEMASK)
            settingsp->sFixRateUserOp.bWritable = TRUE;

        /* get fix rate setting */
        settingsp->nFixRate= piget32(&parmfp);

        /* set result code to the only one defined in OV1 */
        settingsp->eResult = SWI_LBSSETTINGSTATUS_Success;
    } 
}
/**
 *
 * This function unpacks the LbsFixSettings set response.
 *
 * \param   paramfieldp  - Pointer to the CnS parameter field 
 * \param   settingsp    - Pointer to storage into which the LBS 
 *                         fix settings will be placed by this 
 *                         function.
 *
 */
package void SwiUpkLbsFixSettingsSetResp(
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_LbsFixSettings *settingsp)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    swi_uint16 objversion;

    /* get a local copy of the pointer */
    parmfp = paramfieldp;

    /* get object version of the received packet */
    objversion = piget16(&parmfp);

    if (objversion >= APOBJVER1)
    {
        /* set result code to the only one defined in OV1 */
        settingsp->eResult = SWI_LBSSETTINGSTATUS_Success;
    } 
}
/**
 *
 * This function unpacks the LbsNiRequest set response.
 *
 * \param      paramfieldp - Pointer to the CnS parameter field 
 * \param      requestp    - Pointer to storage into which the LBS 
 *                           NI request info will be placed by this 
 *                           function.
 *
 */
package void SwiUpkLbsNiRequest(
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_LbsNiReq *requestp)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    swi_uint32 sizeStruct;
    swi_uint32	nNiId;
    enum TYPE_LbsNiReqResp eReqResp; 
    swi_uint16 objversion;

    /* get a local copy of the pointer */
    parmfp = paramfieldp;

    /* clear return structure memory before unpacking into it */
    sizeStruct = requestp->sizeStruct;
    nNiId= requestp->nNiId;
    eReqResp= requestp->eReqResp;
    slmemset((char *)requestp, 0, sizeStruct);
    requestp->sizeStruct = sizeStruct;
    requestp->nNiId = nNiId;
    requestp->eReqResp = eReqResp;
  
    /* get object version of the received packet */
    objversion = piget16(&parmfp);

    if (objversion >= APOBJVER1)
    {
        requestp->eErrCode = *parmfp++;
    }
}
/**
 *
 * This function unpacks the NI Request notification
 *
 * \param      paramfieldp  - Pointer to the CnS parameter field 
 * \param      requestp     - Pointer to storage into which the LBS 
 *                            NI request info will be placed by this 
 *                            function.
 *
 */
package void SwiUpkLbsNiReqNotify(
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_LbsNiReq *requestp)
{
    swi_uint8 *parmfp; 
    swi_uint8 length;     
    swi_uint32 sizeStruct;
    swi_uint16 objversion;

    /* get a local copy of the pointer */
    parmfp = paramfieldp;

    /* clear return structu re memory before unpacking into it */
    sizeStruct = requestp->sizeStruct;
    slmemset((char *)requestp, 0, sizeStruct);
    requestp->sizeStruct = sizeStruct;
  
    /* get object version of the received packet */
    objversion = piget16(&parmfp);

    if (objversion >= APOBJVER1)
    {
        requestp->nNiId = *parmfp++;

        requestp->eReqResp = piget16(&parmfp);

        requestp->eNotifType= *parmfp++;

        requestp->eReqIdType= *parmfp++;

        length = *parmfp++;
        slmemcpy(&requestp->szReqId[0], parmfp, length);
        parmfp += SWIREQIDMAXLEN;

        requestp->eClientName = *parmfp++;

        length = *parmfp++;
        slmemcpy(&requestp->szClientName[0], parmfp, length);
        parmfp += SWICLIENTNAMEMAXLEN;
    }
}

/*
 * $Log: SwiIntUpkGsmLbs.c,v $
 */

