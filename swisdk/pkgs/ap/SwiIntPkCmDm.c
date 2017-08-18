/**
 *    $Id: SwiIntPkCmDm.c,v 1.1 2008/10/15 21:22:28 YGao Exp $
 *
 *    Name       : SwiIntPkCmDm.c
 *    
 *    Purpose    : Pack functions for OMA-DM APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */
 
#include "SwiApiCmDm.h"
#include "SwiIntPkCmDm.h"
#include "apidefs.h"

/**
 *
 * This function packs OMA-DM configuration data to the CnS object's 
 * parameter field for SwiSetOMADMConfig API.
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed 
 *                          data will be placed by this function
 * \param psConfig[IN]      Struct containing configuration params.
 *                          See SWI_STRUCT_OMADM_SET_CONFIG.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackOMADMConfig(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_OMADM_SET_CONFIG *psConfig)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(1, &localfieldp);   /* obj_ver */
    piput32(psConfig->dwFeatureCapabilityMask, &localfieldp);
    piput32(psConfig->dwFeatureEnableMask, &localfieldp);
    *localfieldp++ = psConfig->bResetHFA;

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs OMA-DM session data to the CnS object's 
 * parameter field for SwiStartOMADMSession API.
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                          data will be placed by this function
 * \param psSession[IN]     Struct containing sesssion start params.
 *                          See SWI_STRUCT_OMADM_START_SESSION.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackStartOMADMSession(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_OMADM_START_SESSION *psSession)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(1, &localfieldp);   /* obj_ver */
    piput16(0, &localfieldp);   /* Always 0 for now */
    *localfieldp++ = psSession->nType;

    /* DM TLV account node name */
    piput16(DM_TLV_Account_Node_Name, &localfieldp);
    piput16(psSession->nAcctNameLen, &localfieldp);
    piputmulti(psSession->psAcctName, &localfieldp, psSession->nAcctNameLen);

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs NI Alert response data to the CnS object's 
 * parameter field for SwiSetOMADMNIAlertResponse API.
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                          data will be placed by this function
 * \param nResponse[IN]     0 = Deny; 1 = Allow.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackOMADMNIAlertResponse(swi_uint8 *paramfieldp, 
                                               swi_uint16 nResponse)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(1, &localfieldp);   /* obj_ver */
    *localfieldp++ = nResponse;

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs DM tree node value data to the CnS object's 
 * parameter field for SwiGetOMADMNodeValue API.
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                          data will be placed by this function
 * \param psItem[IN]        Node value to retrieve.
 *                          See SWI_STRUCT_OMADM_NODE_VALUE_REQUEST.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackGetOMADMNodeValue(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_OMADM_NODE_VALUE_REQUEST *psItem)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(1, &localfieldp);   /* obj_ver */
    piput16(0, &localfieldp);   /* Reserved, always o */

    /* DM TLV node path */
    piput16(DM_TLV_Node_Path, &localfieldp);
    piput16(psItem->nNodePathLength, &localfieldp);
    piputmulti(psItem->psText, &localfieldp, psItem->nNodePathLength);

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs DM tree node value data to the CnS object's 
 * parameter field for SwiSetOMADMNodeValue API.
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                          data will be placed by this function
 * \param psValue[IN]       Node value item and data.
 *                          See SWI_STRUCT_OMADM_NODE_VALUE.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackSetOMADMNodeValue(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_OMADM_NODE_VALUE *psValue)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */
    swi_uint8  *textp;  /* Local copy of paramfieldp */
    swi_uint16 nLen;

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    nLen = LENGTH_DM_TLV_HEADER*NUM_Node_Data_Sub_TLV + 
            psValue->nNodeFormatLength + psValue->nNodePathLength + 
            psValue->nNodeTypeLength + psValue->nNodeValueLength;

    piput16(1, &localfieldp);   /* obj_ver */
    piput16(0, &localfieldp);   /* Reserved, always o */

    /* DM TLV set node data */
    piput16(DM_TLV_Set_Node_Data, &localfieldp);
    piput16(nLen, &localfieldp);

    /* DM TLV sub node, node path */
    piput16(DM_TLV_Node_Path, &localfieldp);
    piput16(psValue->nNodePathLength, &localfieldp);
    piputmulti(psValue->psText, &localfieldp, psValue->nNodePathLength);

    textp = psValue->psText + psValue->nNodePathLength;

    /* DM TLV sub node, node format */
    piput16(DM_TLV_Node_Format, &localfieldp);
    piput16(psValue->nNodeFormatLength, &localfieldp);
    piputmulti(textp, &localfieldp, psValue->nNodeFormatLength);

    textp += psValue->nNodeFormatLength;

    /* DM TLV sub node, node type */
    piput16(DM_TLV_Node_Type, &localfieldp);
    piput16(psValue->nNodeTypeLength, &localfieldp);
    piputmulti(textp, &localfieldp, psValue->nNodeTypeLength);

    textp += psValue->nNodeTypeLength;

    /* DM TLV sub node, node value */
    piput16(DM_TLV_Node_Value, &localfieldp);
    piput16(psValue->nNodeValueLength, &localfieldp);
    piputmulti(textp, &localfieldp, psValue->nNodeValueLength);

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs DM Download Alert Response data to the CnS object's 
 * parameter field for SwiSetDMDLAlertResponse API.
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                          data will be placed by this function
 * \param nResponse[IN]     User response to alert.
 *                          See SWI_TYPE_DM_DL_ALERT_RESP
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackDMDLAlertResponse(swi_uint8 *paramfieldp, 
                                            swi_uint8 nResponse)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(1, &localfieldp);   /* obj_ver */
    *localfieldp++ = nResponse;
    
    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/* 
 * $Log: SwiIntPkCmDm.c,v $
 */
