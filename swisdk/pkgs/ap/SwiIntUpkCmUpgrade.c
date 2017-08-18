/**
 *
 * $Id: SwiIntUpkCmUpgrade.c,v 1.2 2008/08/21 02:43:00 rfiler Exp $
 *
 * Filename: SwiIntUpkCmUpgrade
 *
 * Purpose:  Unpack functions for outgoing request for APIs related to the 
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
 * Unpacks the received NvStore Notification. Currently supports 
 * object version 1
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   nvstorep: [OUT] memory location to store the unpacked contents of
 *                          the NV store notification.
 *
 */
package void SwiUpkNvStore( 
    swi_uint8                 *paramp, 
    struct SWI_STRUCT_NvStore *nvstorep)
{
    swi_uint16 objversion;

    slmemset((char *)nvstorep, 0, sizeof(struct SWI_STRUCT_NvStore));

    objversion = piget16(&paramp);

    if(objversion >= APOBJVER1)
    {
        nvstorep->bSuccess    = *paramp++;
        nvstorep->eOperation  = *paramp++;
        nvstorep->eCategory   = *paramp++;
        nvstorep->nStored     = piget32(&paramp);
        nvstorep->nSkipped    = piget32(&paramp);
        nvstorep->nZeroLength = piget32(&paramp);
    }
   
    /* for future use */
    if(objversion >= APOBJVER2)
    {
        /* unpack object version 2 fields */
    }
}

/*
 * $Log: SwiIntUpkCmUpgrade.c,v $
 */
