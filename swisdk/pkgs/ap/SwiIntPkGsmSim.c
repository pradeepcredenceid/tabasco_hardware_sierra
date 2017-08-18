/**
 *
 * $Id: SwiIntPkGsmSim.c,v 1.7 2010/06/10 00:24:10 epasheva Exp $
 *
 * Filename: SwiIntPkGsmSim.c
 *
 * Purpose:  Contains Packing routines for CnS objects in the 
 *           GSM SIM category
 *
 * Copyright: © 2008 Sierra Wireless, Inc. all rights reserved
 *
 */
#include "apidefs.h"
#include "sl/sludefs.h"

/* Functions */
/**
 *
 * Packs the outgoing parameters for the SwiSetSimVerify API.
 *             
 * \param   paramp: [OUT]  pointer to parameter field in set request
 * \param   simp: [IN]     pointer to SIM information to pack
 *
 * \return  Number of bytes packed into the CnS Parameter field
 *
 */
package swi_uint16 SwiPkGsmSimVerify( 
    swi_uint8                    *paramp, 
    const struct SWI_STRUCT_SIM_Verify *simp)
{
    swi_uint8 *localparmp;

    /* Make a local copy of the incoming parameter pointer */
    localparmp = paramp;
    
    /* copy password type to the parameter field */
    piput16(simp->eType, &localparmp);

    /* copy length of password string to the parameter field */
    *localparmp++ = slstrnlen((char *)&simp->szPassword[0], LENGTH_MaxCHV);

    /* copy password string  to parameter field */
    piputmulti((const swi_uint8 *)&simp->szPassword[0], &localparmp,
                LENGTH_MaxCHV -1);

    /* copy length of unblocking password string to the parameter field */
    *localparmp++ = slstrnlen((char *)&simp->szUnblockPassword[0],
                              LENGTH_MaxCHV);

    /* copy unblocking password string to parameter field */
    piputmulti ((const swi_uint8 *)&simp->szUnblockPassword[0], &localparmp,
                LENGTH_MaxCHV -1);

    return( localparmp - paramp );
}
/**
 *
 * Packs the outgoing parameters for the SwiSetSimVerify API.
 *             
 * \param   paramp: [OUT]  pointer to parameter field in set request
 * \param    simp: [IN]    pointer to SIM information to pack
 *
 * \return  Number of bytes packed into the CnS Parameter field
 *
 */
package swi_uint16 SwiPkGsmSimLock(
    swi_uint8                        *paramp, 
    const struct SWI_STRUCT_SIM_EnableLock *simp)
{
    swi_uint8 *localparmp;

    /* Make a local copy of the incoming parameter pointer */
    localparmp = paramp;
    
    /* copy requested enable/disable flag to the parameter field */
    *localparmp++ =  simp->bEnable;

    /* copy length of CHV1 to parameter field */
    *localparmp++ = slstrnlen((char *)&simp->szPassword[0], LENGTH_MaxCHV);

    /* copy CHV1 to parameter field */
    piputmulti((const swi_uint8 *)&simp->szPassword[0], &localparmp,
                LENGTH_MaxCHV -1);

    return( localparmp - paramp );
}
/**
 *
 * Packs the outgoing parameters for the SwiSetSimPassword API.
 *             
 * \param   paramp: [OUT]  pointer to parameter field in set request
 * \param   simp: [IN]     pointer to SIM information to pack
 *
 * \return  Number of bytes packed into the CnS Parameter field
 *
 */
package swi_uint16 SwiPkGsmSimPassword(
    swi_uint8                   *paramp, 
    const struct SWI_STRUCT_SIM_Password *simp)
{
    swi_uint8 *localparmp;

    /* Make a local copy of the incoming parameter pointer */
    localparmp = paramp;
    
    /* copy password type to the parameter field */
    piput16(simp->eType, &localparmp);

    /* copy length of old password string to the parameter field */
    *localparmp++ = slstrnlen((char *)&simp->szOldPassword[0], LENGTH_MaxCHV);

    /* copy old password string to parameter field */
    piputmulti ((const swi_uint8 *)&simp->szOldPassword[0], &localparmp,
                 LENGTH_MaxCHV -1);

    /* copy length of new password string to the parameter field */
    *localparmp++ = slstrnlen((char *)&simp->szNewPassword[0], LENGTH_MaxCHV);

    /* copy new password string to parameter field */
    piputmulti((const swi_uint8 *)&simp->szNewPassword[0], &localparmp,
                LENGTH_MaxCHV -1);

    return( localparmp - paramp );

}
/**
 *
 * Packs the outgoing parameters for the SwiChv2StatusKick API.
 *
 * \param   paramp: [OUT]      pointer to parameter field in set request
 * \param   kicktype: [IN]     CHV2 kick type request
 *
 * \return  Number of bytes packed into the CnS Parameter field
 *
 */
package swi_uint16 SwiPkChv2StatusKick(
    swi_uint8                      *paramp, 
    enum SWI_TYPE_SIM_Chv2KickType kicktype)
{
    swi_uint8 *localparmp;

    /* Make a local copy of the incoming parameter pointer */
    localparmp = paramp;
    
    /* copy kick type to the parameter field */
    *localparmp++ = kicktype;

    /* Return the length used up by packing */
    return( localparmp - paramp );
}
/*
 * $Log: SwiIntPkGsmSim.c,v $
 */
