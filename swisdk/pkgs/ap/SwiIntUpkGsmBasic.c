/**
 *
 * $Id: SwiIntUpkGsmBasic.c,v 1.7 2010/06/10 00:25:24 epasheva Exp $
 *
 * Filename: SwiIntUpkGsmBasic.c
 *
 * Purpose:  Contains unpacking routines for GSM Objects in the basic
 *           category
 *
 * Copyright: © 2008 Sierra Wireless, Inc. all rights reserved
 *
 */
#include "apidefs.h"

/**
 *
 * This function unpacks the Band info from the CnS
 * object's parameter field
 *
 * \param  paramfieldp   - Pointer to the CnS parameter field
 *                         of the CnS object containing the band info
 * \param  sBand         - Pointer to storage into which the band info
 *                         will be placed by this function
 *
 */
package void SwiUpkGsBandInfo (struct CNS_BAND_PAYLOAD *paramfieldp, struct SWI_STRUCT_Band *sBand) 
{
    int i;
    sBand->eBandType       = piget16((swi_uint8 **)&paramfieldp); 
    sBand->cntValidBands   = piget16((swi_uint8 **)&paramfieldp); 
    for (i = 0; i < sBand->cntValidBands;  ++i)
        sBand->arrBand[i] = piget16((swi_uint8 **)&paramfieldp); 
}

/**
 *
 * This function unpacks the Available Features from the CnS
 * object's parameter field
 *
 * \param  paramfieldp   - Pointer to the CnS parameter field of the 
 *                         CnS object containing the Available features
 * \param  sFeaturesp    - Pointer to storage into which the Available features
 *                         will be placed by this function
 *
 */

package void SwiUpkGsAvailableFeatures (swi_uint8 *paramfieldp, struct SWI_STRUCT_AvailFeatures * sFeaturesp) 
{
    pigetmulti(&paramfieldp, (swi_uint8 *)&(sFeaturesp->bVoice), 1 ); 
    pigetmulti(&paramfieldp, (swi_uint8 *)&(sFeaturesp->bTriBand), 1); 
    pigetmulti(&paramfieldp, (swi_uint8 *)&(sFeaturesp->pdpType), 1); 
}

/**
 *
 * Purpose:  This function unpacks the FSN from the CnS object's 
 *           parameter field
 *
 * \param  paramfieldp - Pointer to the CnS parameter field of the CnS 
 *                       object containing the Available features
 * \param      sFsnp   - Pointer to storage into which the FSN will be 
 *                       placed by this function
 *
 */
package void SwiUpkGsSerialNumber (swi_uint8 *paramfieldp, struct SWI_STRUCT_Fsn *sFsnp) 
{
    sFsnp->sizeStruct = LENGTH_Fsn;
    slmemset ((char *)&(sFsnp->szFsn[0]), 0, sFsnp->sizeStruct);   

    pigetmulti(&paramfieldp, (swi_uint8 *)&(sFsnp->szFsn), LENGTH_Fsn); 
}

/**
 *
 * This function unpacks the Test History byte from the CnS
 * object's parameter field
 *
 * \param  paramfieldp   - Pointer to the CnS parameter field of the CnS 
 *                         object containing the Test history byte
 * \param  sThbp         - Pointer to storage into which the Test history byte
 *                         will be placed by this function
 *
 */
package void SwiUpkGsThb(swi_uint8 *paramfieldp, struct SWI_STRUCT_Thb *sThbp) 
{
    swi_uint32 dummy;
    
    dummy = piget16(&paramfieldp);
    
    pigetmulti(&paramfieldp, (swi_uint8 *)&(sThbp->bInitialTest), 1); 
    pigetmulti(&paramfieldp, (swi_uint8 *)&(sThbp->bRFScreen), 1); 
    pigetmulti(&paramfieldp, (swi_uint8 *)&(sThbp->bCalibration), 1); 
    pigetmulti(&paramfieldp, (swi_uint8 *)&(sThbp->bSystem), 1); 
}

/**
 *
 * This function unpacks the Feature Customizations from the CnS
 * object's parameter field
 *
 * \param  paramfieldp   - Pointer to the CnS parameter field of the CnS 
 *                         object containing the Feature Customizations 
 * \param  sFeaturesp    - Pointer to storage into which the Feature 
 *                         Customizations will be placed by this function
 *
 */

package void SwiUpkGsFeatureCustomizations (
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_FeatureCustomizations * sFeaturesp)
{
    /* See CNS object definitions */
    sFeaturesp->nVersion   = piget16 (&paramfieldp);
    
    pigetmulti(&paramfieldp, (swi_uint8 *)&(sFeaturesp->bAutoConnect), 1); 
    pigetmulti(&paramfieldp, (swi_uint8 *)&(sFeaturesp->bAdvanceProfile), 1); 
    pigetmulti(&paramfieldp, (swi_uint8 *)&(sFeaturesp->bDisplayDun), 1); 
    pigetmulti(&paramfieldp, (swi_uint8 *)&(sFeaturesp->bHideByteCounters), 1); 
    pigetmulti(&paramfieldp, (swi_uint8 *)&(sFeaturesp->bAllowCsd), 1); 
    pigetmulti(&paramfieldp, (swi_uint8 *)&(sFeaturesp->bHSDPASupport), 1); 
    pigetmulti(&paramfieldp, (swi_uint8 *)&(sFeaturesp->bScanProfile), 1); 
    pigetmulti(&paramfieldp, (swi_uint8 *)&(sFeaturesp->bBluetooth), 1); 
    pigetmulti(&paramfieldp, (swi_uint8 *)&(sFeaturesp->bGps), 1); 
}

/*
 * $Log: SwiIntUpkGsmBasic.c,v $
 */
