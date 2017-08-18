/**
 *    $Id: SwiIntPkCdmaVoice.c,v 1.1 2008/10/15 21:22:28 YGao Exp $
 *
 *    Name       : SwiIntPkCdmaVoice.c
 *    
 *    Purpose    : Pack functions for CDMA specific VOICE APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */
 
#include "SwiIntPkCdmaVoice.h"
#include "apidefs.h"

/**
 *
 * This function packs DTMF key tone data to the CnS object's 
 * parameter field for SwiSetDTMFKey API.
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                          data will be placed by this function
 * \param DTMFKey[IN]       The enumerated type for the key tone to generate.
 *                          NOTE: Only the first 12 entries in
 *                          the data type are valid for this
 *                          function: #, *, 0-9. See SWI_TYPE_PhoneKey.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackDTMFKey(swi_uint8 *paramfieldp, swi_uint16 DTMFKey)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;
    *localfieldp++ = 0;         /* Reserved. Must be 0. */
    *localfieldp++ = (swi_uint8)DTMFKey;

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/* 
 * $Log: SwiIntPkCdmaVoice.c,v $
 */
