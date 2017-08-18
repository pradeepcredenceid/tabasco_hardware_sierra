/**
 *
 * $Id: SwiIntPkCmBasic.c,v 1.10 2010/06/10 00:23:06 epasheva Exp $
 *
 * Filename: SwiIntPkCmBasic
 *
 * Purpose:  Pack functions for outgoing request for APIs related to the 
 *           common basic functionality
 *
 * Copyright: © 2008 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "SwiApiCmBasic.h"
#include "ap/apidefs.h"

#define SWI_SETPOWER_SIZE 16         /* # of bytes in Set Power param field */
#define SWI_SETPOWER_UNUSED_SIZE 16  /* # unused in Set Power param field */

/**
 *
 * Packs the outgoing parameters for the SwiSetRadioPower API.
 * Currently supporting object version 2.
 *
 * \param   paramp[OUT]:  pointer to parameter field in set request
 * \param   objectversion[IN]: object version of the packet to build
 * \param   radiop[IN]: pointer to Radio Power parameters to pack
 *
 * \return  Number of bytes packed by this routine
 *
 */
package swi_uint16 SwiPkRadioPower( 
    swi_uint8                    *paramp, 
    swi_uint16                   objversion,
    struct SWI_STRUCT_RadioPower *radiop)
{
    swi_uint8 *localparmp;
    swi_uint16 unusedsize=0;


    /* Make a local copy of the incoming parameter pointer */
    localparmp = paramp;

    /* clear the entire packet based on the object version */
    switch(objversion)
    {
        case APOBJVER1:
        case APOBJVER2:
            slmemset((char *)localparmp, 0, SWI_SETPOWER_SIZE);
            unusedsize = SWI_SETPOWER_UNUSED_SIZE;
            break;
        default:
            erAbort("SwiPkRadioPower - unsupported objversion", objversion);
            break;
    }
    
    /* pack the object version 1 fields */
    piput16(objversion, &localparmp);
    piput16(radiop->eMode, &localparmp);

    /* check if object version 2 fields should be present in this packet */
    if(objversion >= APOBJVER2)
    {
        /* there are no object version 2 field associated with  */
        /* this set request */
    }

    /* check if object version 2 fields should be present in this packet */
    if(objversion >= APOBJVER3)
    {
        /* for future use */
        /* pack the object version 3 fields */
    }

    /* Need to return the exact length of the parameter 
     * field for this version of the object 
     */
    localparmp += unusedsize;

    /* Return the length used up by packing */
    return( localparmp - paramp );
}

/**
 *
 * Packs the outgoing parameters for the SwiSetRadioBandCfg API.
 * Currently supporting object version 1.
 *             
 * \param   paramp [OUT]:  pointer to parameter field in set request
 * \param   objectversion [IN]: object version of the packet to build
 * \param   radiobandgroup [IN]: radio band group information to pack
 *
 * \return  Number of bytes packed into the CnS Parameter field
 *
 *
 */
package swi_uint16 SwiPkRadioBandCfg( 
    swi_uint8                    *paramp, 
    swi_uint16                   objversion,
    swi_uint64                   radiobandgroup)
{
    swi_uint8  *localparmp;  /* Local copy of input parameter ptr */

    /* Assign the local parameter pointer */
    localparmp = paramp;

    /* clear the entire packet based on the object version */
    switch(objversion)
    {
        case APOBJVER1:
            slmemset((char *)localparmp, 0, 10);
            break;
        default:
            erAbort("SwiPkRadioBandCfg- unsupported objversion", objversion);
            break;
    }
    
    /* pack the object version 1 fields */
    piput16(objversion, &localparmp);
    piput64(radiobandgroup, &localparmp);

    /* check if object version 2 fields should be present in this packet */
    if(objversion >= APOBJVER2)
    {
        /* for future use */
        /* pack the object version 2 fields */
    }

    return localparmp - paramp;
}

/**
 *
 * Packs the outgoing parameters for the SwiSetRadioPower API.
 * Currently supporting object version 2.
 *
 * This call aborts on Unsupported Object Versions
 *
 * \param   paramp [OUT]:  pointer to parameter field in set request
 * \param   objectversion [IN]: object version of the packet to build
 * \param   FlashImgInfop [IN]: pointer to FlashImgInfo parameters to pack
 *
 * \return  Number of bytes packed by this routine
 *
 */
package swi_uint16 SwiPkFlashImgInfo( 
    swi_uint8                      *paramp, 
    swi_uint16                     objversion,
    struct SWI_STRUCT_FlashImgInfo *FlashImgInfop)
{
    swi_uint8  *localparmp;  /* Local copy of input parameter ptr */

    /* Assign the local parameter pointer */
    localparmp = paramp;

    /* clear the entire packet based on the object version */
    switch(objversion)
    {
        case APOBJVER1:
        case APOBJVER2:
            slmemset((char *)localparmp, 0,
                        sizeof (struct SWI_STRUCT_FlashImgInfo));
            break;
        default:
            erAbort("SwiPkFlashImgInfo - unsupported objversion", objversion);
            break;
    }
    
    /* pack the object version 1 fields */
    piput16(objversion, &localparmp);
    piputmulti((const swi_uint8 *)FlashImgInfop->szImageType, &localparmp, 4);

    /* check if object version 2 fields should be present in this packet */
    if(objversion >= APOBJVER2)
    {
        /* there are no object version 2 field associated with  */
        /* this set request */
    }

    /* check if object version 2 fields should be present in this packet */
    if(objversion >= APOBJVER3)
    {
        /* for future use */
        /* pack the object version 3 fields */
    }

    return localparmp - paramp;
}

/**
 *
 * Packs the outgoing parameters for the SwiNVUpdateFromFile API.
 * Currently supporting object version 1.
 *
 * \param   paramfieldp [OUT]:  Pointer to parameter field in set request
 * \param   pNVUpdateFromFile [IN]: Structure contains EFile name 
 *
 * \return  Number of bytes packed by this routine
 *
 */
package swi_uint32 SwiPackNVUpdateFromFile( 
        swi_uint8 *paramfieldp, 
        SWI_STRUCT_NVUpdateFromFile *pNVUpdateFromFile )
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;
    swi_uint32 len;

    piput16(1, &localfieldp);                   /* obj_ver */

    len = slstrlen((const char *)pNVUpdateFromFile->szFileName);
    piputmulti(pNVUpdateFromFile->szFileName, &localfieldp, len);
    slmemset((char *)localfieldp, 0, SWI_FILENAME_LEN-len);
    localfieldp += SWI_FILENAME_LEN-len;

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * Packs the outgoing parameters for the SwiApiUnlock API.
 * Currently supporting object version 1.
 *
 * \param   paramfieldp [OUT]:  Pointer to parameter field in set request
 * \param   pApiUnlock [IN]: Structure contains unlock level and password.
 *                           See SWI_STRUCT_ApiUnlock.
 *
 * \return  Number of bytes packed by this routine
 *
 */
package swi_uint32 SwiPackApiUnlock( swi_uint8 *paramfieldp, 
                                    const SWI_STRUCT_ApiUnlock *pApiUnlock )
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(1, &localfieldp);                   /* obj_ver */
    *localfieldp++ = pApiUnlock->nLevel;        /* level */
    *localfieldp++ = pApiUnlock->nPasswordLen;  /* pwd_length */
    piputmulti(pApiUnlock->szPassword, &localfieldp, 
            pApiUnlock->nPasswordLen);          /* pwd */

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * Packs the outgoing parameters for the SwiSetRemoteDiagAgentEnable API.
 * Currently supporting object version 1.
 *
 * \param   paramfieldp [OUT]:  Pointer to parameter field in set request
 * \param   bEnable[IN]      Disable = 0, Enable = 1;
 *
 * \return  Number of bytes packed by this routine
 *
 */
package swi_uint32 SwiPackRemoteDiagAgentEnable(swi_uint8 *paramfieldp, 
                                                swi_uint8 bEnable)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(1, &localfieldp);                   /* obj_ver */
    *localfieldp++ = bEnable;

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * Packs the outgoing parameters for the SwiSetRemovableMediaSupport API.
 * Currently supporting object version 1.
 *
 * \param   paramfieldp [OUT]:  Pointer to parameter field in set request
 * \param  psRM[IN]        Contains RM support config. 
 *                         See SWI_STRUCT_RM_CONFIG_SET.
 *
 * \return  Number of bytes packed by this routine
 *
 */
package swi_uint32 SwiPackRemovableMediaSupport(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_RM_CONFIG_SET *psRM)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(1, &localfieldp);                   /* obj_ver */
    piput16(psRM->nRMFeature, &localfieldp);
    *localfieldp++ = psRM->bEnable;

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * Packs the outgoing parameters for the SwiPkAudioProfile API.
 *             
 * \param   paramp: [OUT]         pointer to parameter field in set request
 * \param   pAudioProfile: [IN]   pointer to Audio Profile to pack
 * \param   objversion: [IN]      pointer to the object version of the request
 *                                to pack
 *
 * \return  Number of bytes packed into the CnS Parameter field
 *
 */
package swi_uint16 SwiPkAudioProfile(
    swi_uint8                            *paramp, 
    swi_uint16                           objversion,
    const struct SWI_STRUCT_AudioProfile *pAudioProfile)
{
    swi_uint8    *localparmp;

    /* Make a local copy of the incoming parameter pointer */
    localparmp = paramp;
    
    /* copy Audio Profile parameters to the parameter field */
    piput16( objversion , &localparmp );
    piput16( pAudioProfile->eCurrent , &localparmp );
    if( objversion >= APOBJVER2 )
    {
        *localparmp++ = (swi_uint8)pAudioProfile->bEarPieceMute;
        *localparmp++ = (swi_uint8)pAudioProfile->bMicMute;
        *localparmp++ = (swi_uint8)pAudioProfile->eAudioGen;
        *localparmp++ = (swi_uint8)pAudioProfile->eVolLevel;
    }
    return( localparmp - paramp );
}

/*
 * $Log: SwiIntPkCmBasic.c,v $
 */
