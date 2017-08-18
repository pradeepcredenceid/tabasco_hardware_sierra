/**
 *    $Id: SwiIntPkCdmaNetwork.c,v 1.2 2010/06/10 00:23:06 epasheva Exp $
 *
 *    Name       : SwiIntPkCdmaNetwork.c
 *    
 *    Purpose    : Pack functions for  CDMA specific network APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */
 
#include "SwiIntPkCdmaNetwork.h"
#include "apidefs.h"

/**
 *
 * This function packs Mobile IP various settings to the CnS 
 * object's parameter field for SwiPstSetDsMipGenUser API.
 *
 * \param paramfieldp[OUT]          Pointer to storage into which the packed 
 *                                  data will be placed by this function
 * \param index[IN]                 Index # of Profile to modify: 0-5.
 * \param naiLength[IN]             Length of NAI field in bytes.
 * \param nai[IN]                   NAI (username)
 * \param mnHaSpiEnable[IN]         MN-HA SPI enable
 *                                    0 - disable
 *                                    1 - enable
 * \param mnHaSpi[IN]               MN-HA SPI
 * \param mnAaaSpiEnable[IN]        MN-AAA SPI enable
 *                                    0 - disable
 *                                    1 - enable
 * \param mnAaaSpi[IN]              MN-AAA SPI
 * \param reverseTunneling[IN]      Reverse Tunneling Preferred
 *                                    0 - disable
 *                                    1 - enable
 * \param homeAddress[IN]           Home IP Address
 * \param primaryHaAddress[IN]      Primary HA IP Address (big-endian)
 * \param secondaryHaAddress[IN]    Secondary HA IP Address (big-endian)
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPstPackDsMipGenUser(
        swi_uint8 *paramfieldp, 
        swi_uint16 index, 
        swi_uint16 naiLength, 
        swi_charp nai, 
        swi_uint16 mnHaSpiEnable, 
        swi_uint32 mnHaSpi, 
        swi_uint16 mnAaaSpiEnable, 
        swi_uint32 mnAaaSpi, 
        swi_uint16 reverseTunneling, 
        swi_uint32 homeAddress, 
        swi_uint32 primaryHaAddress, 
        swi_uint32 secondaryHaAddress)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(NV_DS_MIP_GEN_USER_PROF, &localfieldp);
    piput16(index, &localfieldp);
    piput16(naiLength, &localfieldp);
    piputmulti((const swi_uint8 *)nai, &localfieldp, naiLength);
    localfieldp += (LENGTH_NvNai-naiLength);
    piput16(mnHaSpiEnable, &localfieldp);
    piput32(mnHaSpi, &localfieldp);
    piput16(mnAaaSpiEnable, &localfieldp);
    piput32(mnAaaSpi, &localfieldp);
    piput16(reverseTunneling, &localfieldp);
    piput32(homeAddress, &localfieldp);
    piput32(primaryHaAddress, &localfieldp);
    piput32(secondaryHaAddress, &localfieldp);

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs Mobile IP Profile enable data to the CnS object's 
 * parameter field for SwiPstSetDsMipEnableProf API.
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed 
 *                          data will be placed by this function.
 * \param psMipEnabled[IN]  MIP Profile Enabled list. 
 *                          (see SWI_STRUCT_MipEnabled)
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPstPackDsMipEnableProf(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_MipEnabled *psMipEnabled)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(NV_DS_MIP_ENABLE_PROF, &localfieldp);
    piput16s(psMipEnabled->nMipEnabled, MAX_ActiveSet, &localfieldp);

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/*************
 * Name:     SwiPackModePref
 *
 * Purpose:  This function packs a CDMA System Selection preferences
 *           CnS Request
 *
 * Parameters: paramfieldp   - Pointer to the CnS parameter field 
 *                             into which the SidNid table will be 
 *                             written
 *             modepref      - Modem preference
 *             modeterm      - Modem preference change termination
 *             modedur       - Time in secs for temp. mode preference chg
 *             bandpref      - Band preference to switch to
 *             roampref      - Roaming preference
 *
 * Return:     The number of bytes written into the memory pointed to by
 *             paramfieldp
 *
 * Note:       Applies to:
 *             ==========
 *             parameter field for Set request of CnS object 1070, 
 *             CN_PH_SS_PREF
 *
 **************/
package swi_uint32 SwiPackModePref( swi_uint8 *paramfieldp, 
                                  swi_uint16 modepref,
                                  swi_uint16 modeterm,
                                  swi_uint32 modedur,
                                  swi_uint32 bandpref,
                                  swi_uint16 roampref )
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    /* Store the modem preference */
    piput16( modepref, &localfieldp );

    /* next comes the term mode */
    piput16( modeterm, &localfieldp );

    /* then comes mode change duration */
    piput32( modedur, &localfieldp );

    /* now the band preference */
    piput32( bandpref, &localfieldp );

    /* finally the roaming preference */
    piput16( roampref, &localfieldp );

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/*************
 * Name:     SwiPackSidNid
 *
 * Purpose:  This function packs the 20-entry SidNid table into the 
 *           memory specified
 *
 * Parameters: paramfieldp           - Pointer to the CnS parameter field 
 *                                     into which the SidNid table will be 
 *                                     written
 *             sidnidp               - Pointer to an initialized structure 
 *                                     of type SWI_STRUCT_SidNid 
 *             Nam                   - Index (base 0) of the NAM account
 *                                     to set 
 *
 * Return:     The number of bytes written into the memory pointed to by
 *             paramfieldp
 *
 * Note:       Applies to:
 *             ==========
 *             Fills in the parameter field for CnS object 1033, NV Table
 *             entry 0x103
 *
 **************/
package swi_uint32 SwiPackSidNid(swi_uint8 *paramfieldp, 
                                 const struct SWI_STRUCT_HomeSidNid *sidnidp, 
                                 swi_uint16 Nam )
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */
    swi_uint32 i;             /* Loop variable */
    swi_uint32 limit;         /* Limit of the number of entries to process */
    swi_uint16 nextsid;  
    swi_uint16 nextnid;  

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    /* Store the NV RAM Location ID */
    piput16( CNSIDNIDTABLEID, &localfieldp );

    /* The NAM index needs to come next */
    piput16( Nam, &localfieldp );

    /* Derive the number of entries we need to process */
    limit = (sizeof(struct SWI_STRUCT_HomeSidNid) /
             sizeof(struct SWI_STRUCT_SidNid) );

    /* Read and write the entire table's worth of entries */
    for( i=0; i<limit; i++ )
    {
        nextsid = sidnidp->SidNid[i].Sid;
        nextnid = sidnidp->SidNid[i].Nid;

        /* Write the next 2 values into the CnS structure */
        piput16( nextsid, &localfieldp );
        piput16( nextnid, &localfieldp );
    }

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/* 
 * $Log: SwiIntPkCdmaNetwork.c,v $
 */

