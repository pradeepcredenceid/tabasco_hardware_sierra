/**
 *
 * $Id: SwiIntUpkGsmSim.c,v 1.8 2010/06/10 00:25:24 epasheva Exp $
 *
 * Filename: SwiIntUpkGsmSim.c
 *
 * Purpose:  Contains Unpacking routines for CnS objects in the 
 *           GSM SIM category
 *
 * Copyright: © 2008 Sierra Wireless, Inc. all rights reserved
 *
 */
#include "apidefs.h"
#include "sl/sludefs.h"

#define apMSINlengthmax 11 /* including NULL character */

/**
 * Name:     SwiUpkGsmSimLock
 *
 * Purpose:  This function unpacks the SIM lock status from the 
 *           get response parameter field.
 *
 * Parameters: paramfieldp           - Pointer to the CnS parameter field 
 *             simp                  - Pointer to storage into which the SIM
 *                                     lock status will be placed by this 
 *                                     function
 *
 * Return:    None
 *
 * Note:      None
 *
 */
package void SwiUpkGsmSimLock( 
    swi_uint8 *paramfieldp, 
    struct SWI_STRUCT_SIM_LockStatus *simp )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    /* Unpack SIM lock status (enable/disable flag) */
    simp->bEnabled = *parmfp++;
}
/**
 * Name:     SwiUpkGsmSimStatus
 *
 * Purpose:  This function unpacks the SIM status notification.
 *
 * Parameters: paramfieldp           - Pointer to the CnS parameter field 
 *             simp                  - Pointer to storage into which the SIM
 *                                     status will be placed by this function.
 *
 * Return:    None
 *
 * Note:      None
 *
 */
package void SwiUpkGsmSimStatus(
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_SIM_StatusExp *simp )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    /* Unpack SIM status  */
    simp->eStatus = *parmfp++;
    simp->eRequiredOperation = *parmfp++;
    simp->ePreviousOperation = *parmfp++;
    simp->ePreviousActionResult = *parmfp++;
    simp->bRetryInformationPresent = *parmfp++;
    simp->eRetryInformation = *parmfp++;
    simp->nNumberRetries = *parmfp++;           
}
/**
 * Name:     SwiUpkGsmIMSI
 *
 * Purpose:  This function unpacks the IMSI get response.
 *
 * Parameters: paramfieldp  - Pointer to the CnS parameter field 
 *             simp         - Pointer to storage into which the IMSI
 *                            information will be placed by this function.
 *
 * Return:    None
 *
 * Note:      None
 *
 */
package void SwiUpkGsmIMSI(
    swi_uint8 *paramfieldp,
    swi_charp MSINp, 
    swi_uint32 *MCCp, 
    swi_uint32 *MNCp)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    int MSINlength;       /* length of the MSIN string returned in the resp */

    parmfp = paramfieldp;

    /* Unpack IMSI info */
    *MCCp = piget16(&parmfp);
    *MNCp = piget16(&parmfp);
    slmemset((char *)MSINp, 0, apMSINlengthmax);
    MSINlength = *parmfp++;
    pigetmulti(&parmfp, (swi_uint8 *)MSINp, MSINlength);
}
/**
 * Name:     SwiUpkIccId
 *
 * Purpose:  This function unpacks the ICCID get response.
 *
 * Parameters: paramfieldp  - Pointer to the CnS parameter field 
 *             simp         - Pointer to storage into which the ICCID
 *                            information will be placed by this function.
 *
 * Return:    None
 *
 * Note:      None
 *
 */
package void SwiUpkIccId(
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_IccId *sIccId)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    /* Unpack ICCID */
    pigetmulti(&parmfp, &sIccId->nIccId[0], LENGTH_IccId);
}
/**
 * Name:     SwiUpkSpn
 *
 * Purpose:  This function unpacks the SPN get response.
 *
 * Parameters: paramfieldp  - Pointer to the CnS parameter field 
 *             simp         - Pointer to storage into which the SPN
 *                            string will be placed by this function.
 *             maxspnlength - Maximum length of SPN string supported 
 *                            (this length does not include a NULL char 
 *
 * Return:    None
 *
 * Note:      None
 *
 */
package void SwiUpkSpn(
    swi_uint8 *paramfieldp,
    swi_charp spnp,
    swi_uint32 maxspnlength)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    swi_uint8 length;     /* actual length of the SPN string (without NULL) */

    parmfp = paramfieldp;

    /* Clear buffer */
    slmemset((char *)spnp, 0, maxspnlength);
    /* Unpack SPN */
    length = *parmfp++;
     
    /* truncate string if it is longer than will fit in provided buffer */
    if(length > maxspnlength)
        length = maxspnlength;

    /* copy SPN string to provided buffer */
    pigetmulti(&parmfp, (swi_uint8 *)spnp, length);
}

/**
 * Name:     SwiUpkGsSimAuth
 *
 * Purpose:  This function unpacks the Sim Authentication response.
 *
 * Parameters: paramfieldp  - Pointer to the CnS parameter field 
 *             pSimAuth     - Pointer to storage into which the Sim
 *                            information will be placed by this function.
 *
 * Return:    None
 *
 * Note:      None
 *
 */
package void SwiUpkGsSimAuth(
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_SimAuth  *pSimAuth)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    /* Note: Structure not cleared to preserve input parameters */
    parmfp = paramfieldp;
    pSimAuth->SimResponse     = piget32(&parmfp); 
    pigetmulti(&parmfp, &(pSimAuth->Kc[0]), LENGTH_Kc);
}

/*
 * $Log: SwiIntUpkGsmSim.c,v $
 */

