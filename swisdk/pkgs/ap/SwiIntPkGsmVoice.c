/**
 *
 * $Id: SwiIntPkGsmVoice.c,v 1.4 2010/06/10 00:24:10 epasheva Exp $
 *
 * Filename: SwiIntPkGsmVoice.c
 *
 * Purpose:  Contains Unpacking routines for CnS objects in the 
 *           GSM Voice category
 *
 * Copyright: © 2009 Sierra Wireless, Inc. all rights reserved
 *
 */
#include "apidefs.h"
#include "sl/sludefs.h"
#include "SwiApiGsmVoice.h"

/**
 *
 * Packs the outgoing parameters for the SwiSetNVToneStore API.
 *             
 * \param   paramp: [OUT]         pointer to parameter field in set request
 * \param   pToneLength: [IN]     pointer to Tone information to pack
 *
 * \return  Number of bytes packed into the CnS Parameter field
 *
 */
package swi_uint16 SwiPkGsmNVToneStore(
    swi_uint8 *paramp, const struct SWI_STRUCT_NVToneStore *pToneLength)
{
    swi_uint8    *localparmp;
    
    /* Make a local copy of the incoming parameter pointer */
    localparmp = paramp;
    
    /* copy NV Tone parameters to the parameter field */
    *localparmp++ = pToneLength->eToneLength;
    piput16((swi_uint16)pToneLength->nUserDefinedToneOnDuration, &localparmp);
    piput16((swi_uint16)pToneLength->nUserDefinedToneOffDuration, &localparmp);
    piput16((swi_uint16)pToneLength->nComfortToneOnDuration, &localparmp);
    piput16((swi_uint16)pToneLength->nComfortToneOffDuration, &localparmp);
    *localparmp++ = pToneLength->bEarpieceFeedback; 

    return( localparmp - paramp );
}

/**
 *
 * Packs the outgoing parameters for the SwiSetCallOriginate API.
 *             
 * \param   paramp: [OUT]         pointer to parameter field in set request
 * \param   pCall: [IN]           pointer to Audio Profile to pack
 *
 * \return  Number of bytes packed into the CnS Parameter field
 *
 */
package swi_uint16 SwiPkGsmCallCreate(
    swi_uint8                           *paramp, 
    const struct SWI_STRUCT_CALL_Create *pCall)
{
    swi_uint8    *localparmp;
    swi_uint32   i;           /* index variable */
    
    /* Make a local copy of the incoming parameter pointer */
    localparmp = paramp;
    
    /* copy Audio Profile parameters to the parameter field */
    *localparmp++ = pCall->eCallRestrict;
    *localparmp++ = pCall->bInternational;
    *localparmp++ = (swi_uint8)pCall->nNumberLength;
    for (i = 0; i < pCall->nNumberLength; i++)
    {
        *localparmp++ = pCall->bcdPhone[i];
    }
    
    return( localparmp - paramp );
}

/**
 *
 * Packs the outgoing parameters for the SwiSetDTMFSend API.
 *             
 * \param   paramp: [OUT]         pointer to parameter field in set request
 * \param   pCall: [IN]           pointer to Audio Profile to pack
 *
 * \return  Number of bytes packed into the CnS Parameter field
 *
 */
package swi_uint16 SwiPkGsmDTMFSend(
    swi_uint8                           *paramp, 
    const struct SWI_STRUCT_DTMFSend    *pDTMFSend)
{
    swi_uint8    *localparmp;
    swi_uint32   i;           /* index variable */
    
    /* Make a local copy of the incoming parameter pointer */
    localparmp = paramp;
    
    /* copy Audio Profile parameters to the parameter field */
    *localparmp++ = (swi_uint8)pDTMFSend->nNumberOfDigits;
    for( i = 0; i < 20; i++)
    {
        *localparmp++ = pDTMFSend->byDigits[i];
    }
    piput16((swi_uint16)pDTMFSend->nToneOnDuration, &localparmp);
    piput16((swi_uint16)pDTMFSend->nToneOffDuration, &localparmp);
    *localparmp++ = pDTMFSend->bEarpieceFeedback;
    
    return( localparmp - paramp );
}

/**
 *
 * Packs the outgoing parameters for the SwiSetCallControlUMTS API.
 *             
 * \param   paramp: [OUT]         pointer to parameter field in set request
 * \param   pCall: [IN]           pointer to Call Control information to pack
 *
 * \return  Number of bytes packed into the CnS Parameter field
 *
 */
package swi_uint16 SwiPkGsmCallControl(
    swi_uint8                           *paramp, 
    const struct SWI_STRUCT_CALL_UMTS_Control    *pCall)
{
    swi_uint8    *localparmp;
    swi_uint32   i;           /* index variable */
    
    /* Make a local copy of the incoming parameter pointer */
    localparmp = paramp;
    
    /* copy Audio Profile parameters to the parameter field */
    piput16((swi_uint16)pCall->nVersion, &localparmp);
    piput16((swi_uint16)pCall->nCallID, &localparmp);
    *localparmp++ = pCall->eCallAction;
    for( i = 0; i < 15; i++)
    {
        *localparmp++ = (swi_uint8)0;
    }
    
    return( localparmp - paramp );
}

/**
 *
 * Packs the outgoing parameters for the SwiSetContDtmf API.
 *             
 * \param   paramp: [OUT]         pointer to parameter field in set request
 * \param   pDtmf: [IN]           pointer to DTMF information to pack
 *
 * \return  Number of bytes packed into the CnS Parameter field
 *
 */
package swi_uint16 SwiPkGsmSetContDtmf(
    swi_uint8                           *paramp, 
    const struct SWI_STRUCT_ContDtmf    *pDtmf)
{
    swi_uint8    *localparmp;

    /* Make a local copy of the incoming parameter pointer */
    localparmp = paramp;

    /* copy Audio Profile parameters to the parameter field */
    *localparmp++ = (swi_uint8)pDtmf->nDtmfTone;
    *localparmp++ = pDtmf->bStart;
    
    return( localparmp - paramp );
}


/**
 *
 * Packs the outgoing parameters for the SwiSetContDtmf API.
 *             
 * \param   paramp: [OUT]         pointer to parameter field in set request
 * \param   sSSRequest: [IN]      pointer to Supplementary Service information 
 *                                to pack
 *
 * \return  Number of bytes packed into the CnS Parameter field
 *
 */
package swi_uint16 SwiPkGsmSetSSRequest(
    swi_uint8                           *paramp, 
    const struct SWI_STRUCT_SS_Request  *sSSRequest)
{
    swi_uint8    *localparmp;
    swi_uint32   i;           /* index variable */
    swi_uint8    emptyFwdNum;
    
    emptyFwdNum = LENGTH_ForwardNumber - (sSSRequest->nLengthFwdNum);
    
    /* Make a local copy of the incoming parameter pointer */
    localparmp = paramp;

    /* copy Supplementary Service parameters to the parameter field */
    piput16((swi_uint16)sSSRequest->nVersion, &localparmp);
    *localparmp++ = sSSRequest->eRequestType;
    *localparmp++ = sSSRequest->eSuppServiceCode;
    *localparmp++ = (swi_uint8)sSSRequest->bBasicServGrpInfo;
    *localparmp++ = sSSRequest->eBasicServGrpType;
    *localparmp++ = (swi_uint8)sSSRequest->eBasicServGrpCode;
    if (sSSRequest->eRequestType == SWI_SSREQUESTTYPE_Register &&
        (sSSRequest->eSuppServiceCode == SWI_SERVCODE_CallFwdAllServices || 
        sSSRequest->eSuppServiceCode == SWI_SERVCODE_CallFwdUnconditional || 
        sSSRequest->eSuppServiceCode == SWI_SERVCODE_CallFwdAllConditional || 
        sSSRequest->eSuppServiceCode == SWI_SERVCODE_CallFwdBusy || 
        sSSRequest->eSuppServiceCode == SWI_SERVCODE_CallFwdNoReply || 
        sSSRequest->eSuppServiceCode == SWI_SERVCODE_CallFwdNotReachable))
    {
        *localparmp++ = (swi_uint8)sSSRequest->bInternationalNumFlag;
        *localparmp++ = (swi_uint8)sSSRequest->nLengthFwdNum;
        for (i = 0; i < sSSRequest->nLengthFwdNum; i++)
        {
            *localparmp++ = sSSRequest->byFwdNum[i];
        }
        
    /*for (i = 0; i < emptyFwdNum; i++) 
        {
            *localparmp++ = 0;
        }
        * */

        *localparmp += emptyFwdNum;
        *localparmp++ = (swi_uint8)sSSRequest->bNoReplyInfoPresent;
        *localparmp++ = (swi_uint8)sSSRequest->nNoReplyTime;
    }
    return( localparmp - paramp );    
}

/**
 *
 * Packs the outgoing parameters for the SwiAbortRequest API.
 *             
 * \param   paramp: [OUT]         pointer to parameter field in set request
 * \param   sSSRequest: [IN]      pointer to Supplementary Service information 
 *                                to pack
 *
 * \return  Number of bytes packed into the CnS Parameter field
 *
 */
package swi_uint16 SwiPkGsmAbortRequest(
    swi_uint8                           *paramp, 
    const struct SWI_STRUCT_SS_Abort    *sSSAbort)
{
    swi_uint8    *localparmp;
    
    /* Make a local copy of the incoming parameter pointer */
    localparmp = paramp;

    /* copy Supplementary Service parameters to the parameter field */
    piput16((swi_uint16)sSSAbort->nVersion, &localparmp);
    piput32(sSSAbort->nHandle, &localparmp);

    return( localparmp - paramp );
}

/**
 *
 * Packs the outgoing parameters for the SwiStartSSPassReg API.
 *             
 * \param   paramp: [OUT]         pointer to parameter field in set request
 * \param   sSSRequest: [IN]      pointer to Supplementary Service information 
 *                                to pack
 *
 * \return  Number of bytes packed into the CnS Parameter field
 *
 */
package swi_uint16 SwiPkGsmStartSSPassReg(
    swi_uint8                           *paramp, 
    const struct SWI_STRUCT_SS_PasswordReg    *sSSPassReg)
{
    swi_uint8    *localparmp;
    
    /* Make a local copy of the incoming parameter pointer */
    localparmp = paramp;

    /* copy Supplementary Service parameters to the parameter field */
    piput16((swi_uint16)sSSPassReg->nVersion, &localparmp);
    *localparmp++ = (swi_uint8)sSSPassReg->eSuppServiceCode;

    return( localparmp - paramp );
}

/**
 *
 * Packs the outgoing parameters for the SwiSetSSPassword API.
 *             
 * \param   paramp: [OUT]         pointer to parameter field in set request
 * \param   sSSRequest: [IN]      pointer to Supplementary Service information 
 *                                to pack
 *
 * \return  Number of bytes packed into the CnS Parameter field
 *
 */
package swi_uint16 SwiPkGsmSetSSPassword(
    swi_uint8                           *paramp, 
    const struct SWI_STRUCT_SS_PasswordStatus    *sSSPassStatus)
{
    swi_uint8    *localparmp;
    swi_uint32   i;           /* index variable */
    
    /* Make a local copy of the incoming parameter pointer */
    localparmp = paramp;

    /* copy Supplementary Service parameters to the parameter field */
    piput16((swi_uint16)sSSPassStatus->nVersion, &localparmp);
    piput32(sSSPassStatus->nHandle, &localparmp);
    *localparmp++ = sSSPassStatus->nPasswordLength;
    for (i = 0; i < sSSPassStatus->nPasswordLength; i++)
    {
        *localparmp++ = sSSPassStatus->byPassword[i];
    }
    for (i = 0; i < (238 - sSSPassStatus->nPasswordLength); i++)
    {
        *localparmp++ = 0;
    }
    
    return( localparmp - paramp );
}
/*
 * $Log: SwiIntPkGsmVoice.c,v $
 */
