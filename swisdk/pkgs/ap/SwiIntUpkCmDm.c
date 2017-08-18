/**
 *    $Id: SwiIntUpkCmDm.c,v 1.3 2010/06/10 00:24:10 epasheva Exp $
 *
 *    Name       : SwiIntUpkCmDm.c
 *    
 *    Purpose    : Unpack functions for OMA-DM APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */
#include "SwiDataTypes.h"
#include "SwiRcodes.h"
#include "SwiApiCmDm.h"
#include "SwiIntUpkCmDm.h"
#include "SwiIntPkCmDm.h"
#include "apidefs.h"

/**
 *
 * Unpacks the OMA-DM configuration information from the CnS object's 
 * parameter field.
 *
 * \param   paramfieldp - Pointer to the CnS parameter field of the 
 *                        CnS object CNS_DM_CONFIG.
 * \param   psBsInfo    - Pointer to storage into which the OMA-DM 
 *                        configuration information will be placed 
 *                        by this function.
 *
 */
package void SwiUnpackOMADMConfig(swi_uint8 *paramfieldp, 
                                  SWI_STRUCT_OMADM_GET_CONFIG *psConfig)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    piget16(&parmfp);   /* Discard obj_ver */
    psConfig->nOMADMCapability = *parmfp++;
    psConfig->nMaxAcctSupport = *parmfp++;
    psConfig->nNumAccts = *parmfp++;
    psConfig->bAcctChanged = *parmfp++;
    psConfig->dwFeatureCapabilityMask = piget32(&parmfp);
    psConfig->dwFeatureEnableMask = piget32(&parmfp);
    psConfig->bHFAAttempted = *parmfp++;
}

/**
 *
 * Unpacks the OMA-DM session state information from the CnS object's 
 * parameter field.
 *
 * \param   paramfieldp - Pointer to the CnS parameter field of the 
 *                        CnS object CNS_DM_SESSION_STATE.
 * \param   psBsInfo    - Pointer to storage into which the OMA-DM 
 *                        session state information will be placed 
 *                        by this function.
 *
 */
package void SwiUnpackOMADMSessionState(
        swi_uint8 *paramfieldp, 
        SWI_STRUCT_OMADM_SESSION_STATE *psState)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    piget16(&parmfp);   /* Discard obj_ver */
    psState->nState = *parmfp++;
    psState->nHFARetryTime = piget16(&parmfp);
    psState->nLastSessionType = *parmfp++;
    psState->nLastSessionInitiator = *parmfp++;
    psState->nUIMode = *parmfp++;
    psState->nLastSessionResult = piget16(&parmfp);
    psState->nLastSessionCommit = *parmfp++;
}

/**
 *
 * Unpacks the OMA-DM Network Initiated Alert information from the CnS 
 * object's parameter field.
 *
 * \param   paramfieldp - Pointer to the CnS parameter field of the 
 *                        CnS object CNS_DM_NI_ALERT.
 * \param   psBsInfo    - Pointer to storage into which the OMA-DM 
 *                        Network Initiated Alert information will be 
 *                        placed by this function.
 *
 */
package void SwiUnpackOMADMNIAlert(swi_uint8 *paramfieldp, 
                                   SWI_STRUCT_OMADM_NI_ALERT *psAlert)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    piget16(&parmfp);   /* Discard obj_ver */
    psAlert->nPending = *parmfp++;
    psAlert->nUIMode = *parmfp++;
    psAlert->nForceUIMode = *parmfp++;
    psAlert->nType = *parmfp++;
}

/**
 *
 * Unpacks the DM tree node value data from the CnS object's 
 * parameter field.
 *
 * \param   paramfieldp - Pointer to the CnS parameter field of the 
 *                        CnS object CNS_DM_NODE_VALUE.
 * \param   psBsInfo    - Pointer to storage into which the DM 
 *                        tree node value data will be placed by 
 *                        this function.
 *
 */
package void SwiUnpackOMADMNodeValue(
        swi_uint8 *paramfieldp, 
        SWI_STRUCT_OMADM_NODE_VALUE_RESPONSE *psValue)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    swi_uint8 *textp;

    parmfp = paramfieldp;

    piget16(&parmfp);   /* Discard obj_ver */
    piget16(&parmfp);   /* Discard reserved */
    psValue->nStatus = *parmfp++;

    /* skip initial header */
    piget16(&parmfp);   /* DM-TLV type */
    piget16(&parmfp);   /* DM-TLV length */

    /* Get node format */
    piget16(&parmfp);   /* DM-TLV sub-TLV type */
    psValue->nNodeFormatLength = piget16(&parmfp);
    pigetmulti(&parmfp, psValue->psText, psValue->nNodeFormatLength);

    textp = psValue->psText+psValue->nNodeFormatLength;

    /* Get node type */
    piget16(&parmfp);   /* DM-TLV sub-TLV type */
    psValue->nNodeTypeLength = piget16(&parmfp);
    pigetmulti(&parmfp, psValue->psText, psValue->nNodeTypeLength);

    textp += psValue->nNodeTypeLength;

    /* Get node value */
    piget16(&parmfp);   /* DM-TLV sub-TLV type */
    psValue->nNodeValueLength = piget16(&parmfp);
    pigetmulti(&parmfp, textp, psValue->nNodeValueLength);
}

/**
 *
 * Unpacks the DM download progress information from the CnS object's 
 * parameter field.
 *
 * \param   paramfieldp - Pointer to the CnS parameter field of the 
 *                        CnS object CNS_DM_DL_PROGRESS.
 * \param   psBsInfo    - Pointer to storage into which the DM 
 *                        download progress information will be placed 
 *                        by this function.
 *
 */
package void SwiUnpackDMDLProgress(swi_uint8 *paramfieldp, 
                                   SWI_STRUCT_DM_DL_PROGRESS *psProgress)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;
    
    piget16(&parmfp);   /* Discard obj_ver */
    psProgress->nDLType = *parmfp++;
    psProgress->nBytesDownloaded = piget32(&parmfp);
    psProgress->nTotalBytes = piget32(&parmfp);
}

/**
 *
 * Unpacks the DM download alert information from the CnS object's 
 * parameter field.
 *
 * \param   paramfieldp - Pointer to the CnS parameter field of the 
 *                        CnS object CNS_DM_DL_ALERT.
 * \param   psBsInfo    - Pointer to storage into which the DM 
 *                        download alert information will be placed 
 *                        by this function.
 *
 */
package SWI_RCODE SwiUnpackDMDLAllert(swi_uint8 *paramfieldp, 
                                      SWI_STRUCT_DM_DL_ALERT *psAlert)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    resultcode = SWI_RCODE_OK;
    parmfp = paramfieldp;
    
    piget16(&parmfp);   /* Discard obj_ver */
    piget16(&parmfp);   /* Discard reserved */
    psAlert->nValidRespMask = *parmfp++;
    psAlert->nAlertType = *parmfp++;
    psAlert->bDescAvail = *parmfp++;
    psAlert->nEstDLTime = piget16(&parmfp);
    psAlert->nEstInstTime = piget16(&parmfp);
    psAlert->nTimeout = piget16(&parmfp);
    psAlert->nPkgNameLen = piget16(&parmfp);
    if(psAlert->nPkgNameLen > MAX_DL_PKG_NAME)
    {
        resultcode = SWI_RCODE_UNEXPECTED_RESP;
    }
    else
    {
        pigetmulti(&parmfp, psAlert->szName, psAlert->nPkgNameLen);
    }

    return resultcode;
    
}

/**
 *
 * Unpacks the DM download alert description information from the CnS 
 * object's parameter field.
 *
 * \param   paramfieldp - Pointer to the CnS parameter field of the 
 *                        CnS object CNS_DM_DL_DESCRIPTION.
 * \param   psBsInfo    - Pointer to storage into which the DM 
 *                        download alert description information will 
 *                        be placed by this function.
 *
 */
package SWI_RCODE SwiUnpackDMDLDescription(
        swi_uint8 *paramfieldp, 
        SWI_STRUCT_DM_DL_DESCRIPTION *psDescription)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    resultcode = SWI_RCODE_OK;
    parmfp = paramfieldp;

    piget16(&parmfp);   /* Discard obj_ver */
    piget16(&parmfp);   /* Discard reserved */
    psDescription->nDescLength = piget16(&parmfp);
    if(psDescription->nDescLength > MAX_DL_DESCRIPTION)
    {
        resultcode = SWI_RCODE_UNEXPECTED_RESP;
    }
    else
    {
        pigetmulti(&parmfp, psDescription->szDescription, 
                psDescription->nDescLength);
    }

    return resultcode;
}

/**
 *
 * Unpacks the DM-DL install status from the CnS object's 
 * parameter field.
 *
 * \param   paramfieldp - Pointer to the CnS parameter field of the 
 *                        CnS object CNS_DM_DL_INSTALL_STATUS.
 * \param   psBsInfo    - Pointer to storage into which the DM-DL 
 *                        install status will be placed by this function.
 *
 */
package void SwiUnpackDMDLInstallStatus(
        swi_uint8 *paramfieldp, 
        SWI_STRUCT_DM_DL_INSTALL_STATUS *psStatus)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    piget16(&parmfp);   /* Discard obj_ver */
    psStatus->nEstInstallTime = piget16(&parmfp);
    psStatus->nInstallState = *parmfp++;
    psStatus->nDMStatusCode = piget16(&parmfp);
}

/* 
 * $Log: SwiIntUpkCmDm.c,v $
 */
