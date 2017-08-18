/**
 *
 * $Id: SwiIntPkCmUpgrade.c,v 1.5 2010/06/10 00:23:06 epasheva Exp $
 *
 * Filename: SwiIntPkCmUpgrade
 *
 * Purpose:  Pack functions for outgoing request for APIs related to the 
 *           Firmware Download
 *
 * Copyright: © 2008 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "SwiApiCmUpgrade.h"
#include "ap/apidefs.h"

/**
 *
 * Packs the outgoing parameters for the SwiNvStore API.
 * Currently supporting object version 1.
 *             
 * \param   paramp: [OUT]  pointer to parameter field in set request
 * \param   nvstorep: [IN] pointer to NV store parameters to pack
 *
 * \return  Number of bytes packed by this routine
 *
 */
package swi_uint16 SwiPkNvStore( 
    swi_uint8                 *paramp, 
    swi_uint16                objversion,
    struct SWI_STRUCT_NvStore *nvstorep)
{
    swi_uint8  *localparmp;  /* Local copy of input parameter ptr */

    /* Assign the local parameter pointer */
    localparmp = paramp;

    /* clear the entire packet based on the object version */
    switch(objversion)
    {
        case APOBJVER1:
            slmemset((char *)localparmp, 0, 20);
            break;
        default:
            /* Do nothing here, log will show error */
            break;
    }
    
    /* pack the object version 1 fields */
    piput16(objversion, &localparmp);
    *localparmp++ = nvstorep->eOperation;
    *localparmp++ = nvstorep->eCategory;

    /* for future use */
    if(objversion >= APOBJVER2)
    {
        /* pack the object version 2 fields */
    }
    
    /* for future use */
    if(objversion >= APOBJVER3)
    {
        /* pack the object version 3 fields */
    }

    return (localparmp - paramp);
}
/*
 * $Log: SwiIntPkCmUpgrade.c,v $
 */
