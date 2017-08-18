/**
 *    $Id: SwiIntPkCmLbs.c,v 1.2 2008/11/26 02:26:56 ckaria Exp $
 *
 *    Name       : SwiIntPkCmLbs.c
 *    
 *    Purpose    : Pack functions for Common LBS APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#include "SwiApiCmLbs.h"
#include "SwiIntPkCmLbs.h"
#include "apidefs.h"

/**
 *
 * This function packs LBS Get Pos data to the CnS object's 
 * parameter field for SwiSetLbsPdGetPos API.
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                          data will be placed by this function
 * \param psLbsPdGetPos[IN] LBS PD GetPOS Data. See SWI_STRUCT_LbsPdGetPos
 *                          for more details.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackLbsPdGetPos(
        swi_uint8 *paramfieldp,
        const struct SWI_STRUCT_LbsPdGetPos *psLbsPdGetPos)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(1, &localfieldp);
    piput16(psLbsPdGetPos->eFixType, &localfieldp);
    piput16(psLbsPdGetPos->nPerformance, &localfieldp);
    piput32(psLbsPdGetPos->nAccuracy, &localfieldp);

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );

}

/**
 *
 * This function packs LBS Pd Track to the CnS object's 
 * parameter field for SwiSetLbsPdTrack API.
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                          data will be placed by this function
 * \param psLbsPdTrack[IN]  LBS PD Track Data. See SWI_STRUCT_LbsPdTrack
 *                          for more details.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackLbsPdTrack(
        swi_uint8 *paramfieldp,
        const struct SWI_STRUCT_LbsPdTrack *psLbsPdTrack)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(1, &localfieldp);
    piput16(psLbsPdTrack->eFixType, &localfieldp);
    piput16(psLbsPdTrack->nPerformance, &localfieldp);
    piput32(psLbsPdTrack->nAccuracy, &localfieldp);
    piput16(psLbsPdTrack->nFixCount, &localfieldp);
    piput32(psLbsPdTrack->nFixRate, &localfieldp);

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );

}

/**
 *
 * This function packs LBS Pd EndSession to the CnS object's 
 * parameter field for SwiSetLbsPdTrack API.
 *
 * \param paramfieldp[OUT]      Pointer to storage into which the packed  
 *                              data will be placed by this function
 * \param psLbsPdEndSession[IN] LBS PD End Session Data. See 
 *                              SWI_STRUCT_LbsPdEndSession for more details.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackLbsPdEndSession(
        swi_uint8 *paramfieldp,
        const struct SWI_STRUCT_LbsPdEndSession *psLbsPdEndSession)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(1, &localfieldp);
    piput16(psLbsPdEndSession->eSessionType, &localfieldp);

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );

}

/**
 *
 * This function packs LBS Param IP address to the CnS object's 
 * parameter field for SwiSetLbsPaIpAddr API.
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                          data will be placed by this function
 * \param psPaIpAddr[IN]    LBS Pa IP addr Data. See 
 *                          SWI_STRUCT_LbsPaIpAddr for more details.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackLbsPaIpAddr(
        swi_uint8 *paramfieldp,
        const struct SWI_STRUCT_LbsPaIpAddr *psPaIpAddr)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(1, &localfieldp);
    piput32(psPaIpAddr->nIpAddress, &localfieldp);

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );

}

/**
 *
 * This function packs LBS Param PortID to the CnS object's 
 * parameter field for SwiSetLbsPaPortId API.
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                          data will be placed by this function
 * \param psPAPortID[IN]    LBS Pa Port ID Data. See 
 *                          SWI_STRUCT_LbsPaPortId for more details.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackLbsPaPortId(
        swi_uint8 *paramfieldp,
        const struct SWI_STRUCT_LbsPaPortId *psPAPortID)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(1, &localfieldp);
    piput32(psPAPortID->nPortId, &localfieldp);

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );

}

/**
 *
 * This function packs LBS Clear Assistance data to the CnS object's 
 * parameter field for SwiSetLbsClearAssistance API.
 *
 * \param paramfieldp[OUT]           Pointer to storage into which the packed  
 *                                   data will be placed by this function
 * \param psLbsClearAssistance[IN]   LBS Pa Port ID Data. See 
 *                                   SWI_STRUCT_LbsClearAssistance for more
 *                                   details.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackLbsClearAssistance(
        swi_uint8 *paramfieldp,
        const struct SWI_STRUCT_LbsClearAssistance *psLbsClearAssistance)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(1, &localfieldp);
    *localfieldp++ = psLbsClearAssistance->bClrEphemerisData;
    *localfieldp++ = psLbsClearAssistance->bClrAlmanacData;
    *localfieldp++ = psLbsClearAssistance->bClrPosData;
    *localfieldp++ = psLbsClearAssistance->bClrSelfLearnDb;
    *localfieldp++ = psLbsClearAssistance->bClrServSysDb;
    *localfieldp++ = psLbsClearAssistance->bClrTimeRef;
    *localfieldp++ = psLbsClearAssistance->bResetThrottleParams;

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );

}

/* 
 * $Log: SwiIntPkCmLbs.c,v $
 */
