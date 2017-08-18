/**
 *
 * $Id: SwiIntPkGsmLbs.c,v 1.5 2010/06/10 00:23:06 epasheva Exp $
 *
 * Filename: SwiIntPkGsmLbs.c
 *
 * Purpose:  Contains Packing routines for CnS objects in the 
 *           GSM Location Based Services category
 *
 * Copyright: © 2008 Sierra Wireless, Inc. all rights reserved
 *
 */
#include "apidefs.h"
#include "sl/sludefs.h"

/* Functions */
/**
 *
 * Packs the outgoing parameters for the SwiSetLbsFixSettings API.
 *             
 * \param   paramp: [OUT]    pointer to parameter field in set request
 * \param   objversion: [IN] object version of parameter field to pack
 * \param   settingsp: [IN]  pointer to LBS fix settings info to pack
 *
 * \return  Number of bytes packed by this routine
 *
 */
package swi_uint16 SwiPkLbsFixSettings(
    swi_uint8                        *paramp, 
    swi_uint16                       objversion,
    struct SWI_STRUCT_LbsFixSettings *settingsp)
{
    swi_uint8 usemask;
    swi_uint8  *localparmp;  /* Local copy of input parameter ptr */

    /* Assign the local parameter pointer */
    localparmp = paramp;


    /* copy object version to the parameter field */
    piput16(objversion, &localparmp);

    if(objversion >= APOBJVER1)
    {
        /* determine the fix type setting use mask and */
        /* copy it to the parameter field */
        usemask = 0;
        if(settingsp->sFixTypeUserOp.bReadable)
            usemask = SWIREADMASK;
        if(settingsp->sFixTypeUserOp.bWritable)
            usemask |= SWIWRITEMASK;
        *localparmp++ = usemask;

        /* copy fix type setting to the parameter field */
        piput16(settingsp->eFixType, &localparmp);

        /* determine the performance setting use mask and */
        /* copy it to the parameter field */
        usemask = 0;
        if(settingsp->sPerformanceUserOp.bReadable)
            usemask = SWIREADMASK;
        if(settingsp->sPerformanceUserOp.bWritable)
            usemask |= SWIWRITEMASK;
        *localparmp++ = usemask;

        /* copy performance setting to the parameter field */
        piput16(settingsp->nPerformance, &localparmp);

        /* determine the accuracy setting use mask and */
        /* copy it to the parameter field */
        usemask = 0;
        if(settingsp->sAccuracyUserOp.bReadable)
            usemask = SWIREADMASK;
        if(settingsp->sAccuracyUserOp.bWritable)
            usemask |= SWIWRITEMASK;
        *paramp++ = usemask;

        /* copy accuracy setting to the parameter field */
        piput32(settingsp->nAccuracy, &localparmp);

        /* determine the fix count setting use mask and */
        /* copy it to the parameter field */
        usemask = 0;
        if(settingsp->sFixCountUserOp.bReadable)
            usemask = SWIREADMASK;
        if(settingsp->sFixCountUserOp.bWritable)
            usemask |= SWIWRITEMASK;
        *localparmp++ = usemask;

        /* copy fix count setting to the parameter field */
        piput16(settingsp->nFixCount, &localparmp);

        /* determine the fix rate setting use mask and */
        /* copy it to the parameter field */
        usemask = 0;
        if(settingsp->sFixRateUserOp.bReadable)
            usemask = SWIREADMASK;
        if(settingsp->sFixRateUserOp.bWritable)
            usemask |= SWIWRITEMASK;
        *localparmp++ = usemask;

        /* copy fix rate setting to the parameter field */
        piput32(settingsp->nFixRate, &localparmp);
    }
    return localparmp - paramp;
}
/**
 *
 * Packs the outgoing parameters for the SwiSetLbsNiReq API.
 *             
 * \param   paramp: [OUT]      pointer to parameter field in set request
 * \param   objversion: [IN]   object version of parameter field to pack
 * \param   requestp: [IN]     pointer to info to pack into parameter field
 *
 * \return  Number of bytes packed by this routine
 *
 * Abort:   none
 *
 * Notes:   none
 *
 */
package swi_uint16 SwiPkLbsNiRequest(
    swi_uint8                *paramp, 
    swi_uint16               objversion,
    struct SWI_STRUCT_LbsNiReq *requestp)
{
    swi_uint8  *localparmp;  /* Local copy of input parameter ptr */

    /* Assign the local parameter pointer */
    localparmp = paramp;

    /* copy object version to the parameter field */
    piput16(objversion, &localparmp);

    if(objversion >= APOBJVER1)
    {
        *localparmp++ = requestp->nNiId;
        *localparmp++ = requestp->eReqResp; 
    }
    return localparmp - paramp;
}

/*
 * $Log: SwiIntPkGsmLbs.c,v $
 */
