/**
 *
 * $Id: SwiIntUpkGsmVoice.c,v 1.4 2010/06/10 00:25:24 epasheva Exp $
 *
 * Filename: SwiIntUpkGsmVoice.c
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
 * Name:     SwiUpkGsmNVToneStore
 *
 * Purpose:  This function unpacks the NV Tone info from the CnS
 *           object's parameter field
 *
 * Parameters: paramfieldp           - Pointer to the CnS parameter field 
 *             pToneLength           - Pointer to storage into which the Tone
 *                                     information will be placed by this 
 *                                     function.
 *
 * Return:    None
 *
 * Note:      None
 *
 */
package void SwiUpkGsmNVToneStore(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_NVToneStore *pToneLength )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    parmfp = paramfieldp;

    pToneLength->eToneLength = *parmfp++;
    pToneLength->nUserDefinedToneOnDuration = (swi_uint32)piget16( &parmfp );
    pToneLength->nUserDefinedToneOffDuration = (swi_uint32)piget16( &parmfp );
    pToneLength->nComfortToneOnDuration = (swi_uint32)piget16( &parmfp );
    pToneLength->nComfortToneOffDuration = (swi_uint32)piget16( &parmfp );
    pToneLength->bEarpieceFeedback = *parmfp++;
}



/**
 * Name:     SwiUpkGsmVoiceMessage
 *
 * Purpose:  This function unpacks the Voice Message info from the CnS
 *           object's parameter field
 *
 * Parameters: paramfieldp           - Pointer to the CnS parameter field 
 *             pVoiceMessage         - Pointer to storage into which the Voice
 *                                     Message will be placed by this function.
 *
 * Return:    None
 *
 * Note:      None
 *
 */
package void SwiUpkGsmVoiceMessage(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_VoiceMessage *pVoiceMessage )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    parmfp = paramfieldp;

    pVoiceMessage->bVoiceMailWaitingFeature = *parmfp++;
    pVoiceMessage->bVoiceMailWaitingLine1 = *parmfp++;
    pVoiceMessage->bVoiceMailWaitingLine2 = *parmfp++;
    pVoiceMessage->bFaxMssageWaiting1 = *parmfp++;
    pVoiceMessage->bFaxMssageWaiting2 = *parmfp++;
}

/**
 * Name:     SwiUpkGsmCallExpProgress
 *
 * Purpose:  This function unpacks the Call Progress info from the CnS
 *           object's parameter field
 *
 * Parameters: paramfieldp           - Pointer to the CnS parameter field 
 *             pCall                 - Pointer to storage into which the Call
 *                                     Progress will be placed by this function.
 *
 * Return:    None
 *
 * Note:      None
 *
 */
package void SwiUpkGsmCallExpProgress(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_CALL_EXP_Progress *pCall )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    swi_uint8 NumCallerName;
    swi_uint32   i;           /* index variable */
    parmfp = paramfieldp;

    pCall->CallID = piget16( &parmfp );
    pCall->eCallProgress = *parmfp++;
    pCall->eCallError = *parmfp++;
    pCall->eCallValid = *parmfp++;
    pCall->eCallPresentation = *parmfp++;
    pCall->eCallScreening = *parmfp++;
    pCall->bInternational = *parmfp++;
    pCall->nNumberLength = *parmfp++;
    for (i = 0; i < 20; i++ )
    {
        pCall->bcdPhone[i] = *parmfp++;
    }
    pCall->eCallType = *parmfp++;
    NumCallerName = *parmfp++;
    /*
    for (i = 0; i < LENGTH_CallerName ; i++ )
    {
        pCall->szCallerName[i] = *parmfp++;
    }
    */
    SwiIntUcs2Unpack(&parmfp, 
                 &pCall->szCallerName[0], 
                 LENGTH_CallerNameVoice, 
                 NumCallerName, 
                 162);
    
}

/**
 * Name:     SwiUpkGsmCallForwarded
 *
 * Purpose:  This function unpacks the Call Forwarded info from the CnS
 *           object's parameter field
 *
 * Parameters: paramfieldp           - Pointer to the CnS parameter field 
 *             pForward              - Pointer to storage into which the Call
 *                                     Forwarded will be placed by this function.
 *
 * Return:    None
 *
 * Note:      None
 *
 */
package void SwiUpkGsmCallForwarded(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_CallForwarded *pForward )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    
    parmfp = paramfieldp;

    pForward->nCallID = piget16( &parmfp );
    pForward->bNotificationStatusPresent = *parmfp++;
    pForward->nNotificationStatus = *parmfp++;
}

/**
 * Name:     SwiUpkGsmCallStatus
 *
 * Purpose:  This function unpacks the Call Status info from the CnS
 *           object's parameter field
 *
 * Parameters: paramfieldp           - Pointer to the CnS parameter field 
 *             pStatus               - Pointer to storage into which the Call
 *                                     Status will be placed by this function.
 *
 * Return:    None
 *
 * Note:      None
 *
 */
package void SwiUpkGsmCallStatus(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_CALL_Status *pStatus )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    swi_uint32   i;           /* index variable */
    
    parmfp = paramfieldp;

    pStatus->nCallSum = *parmfp++;
    pStatus->nCallOutgoing = *parmfp++;
    pStatus->nCallHeld = *parmfp++;
    pStatus->nCallWaiting = *parmfp++;
    pStatus->nCallQueued = *parmfp++;
    pStatus->nCallMultiparty = *parmfp++;
    pStatus->nNumberCalls = *parmfp++;
    for (i = 0; i < pStatus->nNumberCalls; i++ )
    {
        pStatus->CallList[i].CallID = piget16( &parmfp );
        pStatus->CallList[i].eCallState = *parmfp++;
        pStatus->CallList[i].CallInfo = *parmfp++;
    }
}

/**
 * Name:     SwiUpkGsmSetSSRequest
 *
 * Purpose:  This function unpacks the Supplementary Service info from the CnS
 *           object's parameter field
 *
 * Parameters: paramfieldp           - Pointer to the CnS parameter field 
 *             sSSRequest            - Pointer to storage into which the
 *                                     Supplementary Service information will 
 *                                     be placed by this function.
 *
 * Return:    None
 *
 * Note:      None
 *
 */
package void SwiUpkGsmSetSSRequest(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_SS_Request *sSSRequest )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    
    parmfp = paramfieldp;

    sSSRequest->nVersion = piget16( &parmfp );
    sSSRequest->nHandle = piget32( &parmfp );
    sSSRequest->eRequestStatus = *parmfp++;
}

/**
 * Name:     SwiUpkGsmStartSSPassReg
 *
 * Purpose:  This function unpacks the Supplementary Service password 
 *           registration info from the CnS object's parameter field
 *
 * Parameters: paramfieldp           - Pointer to the CnS parameter field 
 *             sSSPassReg            - Pointer to storage into which the
 *                                     Supplementary Service information will 
 *                                     be placed by this function.
 *
 * Return:    None
 *
 * Note:      None
 *
 */
package void SwiUpkGsmStartSSPassReg(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_SS_PasswordReg *sSSPassReg )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    
    parmfp = paramfieldp;

    sSSPassReg->nVersion = piget16( &parmfp );
    sSSPassReg->nHandle = piget32( &parmfp );
}

/**
 * Name:     SwiUpkGsmSetSSPassword
 *
 * Purpose:  This function unpacks the Supplementary Service password 
 *           status info from the CnS object's parameter field
 *
 * Parameters: paramfieldp           - Pointer to the CnS parameter field 
 *             sSSPassStatus         - Pointer to storage into which the
 *                                     Supplementary Service information will 
 *                                     be placed by this function.
 *
 * Return:    None
 *
 * Note:      None
 *
 */
package void SwiUpkGsmSetSSPassword(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_SS_PasswordStatus *sSSPassStatus )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    
    parmfp = paramfieldp;

    sSSPassStatus->nVersion = piget16( &parmfp );
    sSSPassStatus->nHandle = piget32( &parmfp );
    sSSPassStatus->ePasswordGuidance = *parmfp++;
}

/**
 * Name:     SwiUpkGsmSSFwdInfoRsp
 *
 * Purpose:  This function unpacks the Supplementary Service Forwarding 
 *           Info data from the CnS object's parameter field
 *
 * Parameters: paramfieldp           - Pointer to the CnS parameter field 
 *             sSSFwdInfo            - Pointer to storage into which the
 *                                     Supplementary Service information will 
 *                                     be placed by this function.
 *
 * Return:    None
 *
 * Note:      None
 *
 */
package void SwiUpkGsmSSFwdInfoRsp(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_SS_RespFwdInfo *sSSFwdInfo )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    swi_uint32   i;           /* index variable */
    swi_uint32   j;           /* index variable */
    
    parmfp = paramfieldp;

    sSSFwdInfo->nVersion = piget16( &parmfp );
    sSSFwdInfo->nHandle = piget32( &parmfp );
    sSSFwdInfo->bServiceCodePresent = *parmfp++;
    sSSFwdInfo->eServiceCode = *parmfp++;
    sSSFwdInfo->nNumEntries = *parmfp++;
    for (i = 0; i < sSSFwdInfo->nNumEntries; i++)
    {
        sSSFwdInfo->sList[i].bBasicServGrpInfoPresent = *parmfp++;
        sSSFwdInfo->sList[i].eBasicServGrpType = *parmfp++;
        sSSFwdInfo->sList[i].eBasicServGrpCode = *parmfp++;
        sSSFwdInfo->sList[i].bServStatusInfoPresent = *parmfp++;
        sSSFwdInfo->sList[i].eServStatus = *parmfp++;
        sSSFwdInfo->sList[i].bInternational = *parmfp++;
        sSSFwdInfo->sList[i].nLengthFwdNum = *parmfp++;
        for (j = 0; j < sSSFwdInfo->sList[i].nLengthFwdNum; j++)
        {
            sSSFwdInfo->sList[i].byFwdNum[j] = *parmfp++;
        }
        sSSFwdInfo->sList[i].bFwdOptionsInfoPresent = *parmfp++;
        sSSFwdInfo->sList[i].eFwdOptions = *parmfp++;
        sSSFwdInfo->sList[i].bNoReplyTimePresent = *parmfp++;
        sSSFwdInfo->sList[i].NoReplyTime = *parmfp++;
    }
}

/**
 * Name:     SwiUpkGsmSSCallBarRsp
 *
 * Purpose:  This function unpacks the Supplementary Service Call Barring 
 *           data from the CnS object's parameter field
 *
 * Parameters: paramfieldp           - Pointer to the CnS parameter field 
 *             sSSCallBar            - Pointer to storage into which the
 *                                     Supplementary Service information will 
 *                                     be placed by this function.
 *
 * Return:    None
 *
 * Note:      None
 *
 */
package void SwiUpkGsmSSCallBarRsp(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_SS_RespCallBar *sSSCallBar )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    swi_uint32   i;           /* index variable */
    
    parmfp = paramfieldp;

    sSSCallBar->nVersion = piget16( &parmfp );
    sSSCallBar->nHandle = piget32( &parmfp );
    sSSCallBar->bServiceCodePresent = *parmfp++;
    sSSCallBar->eServiceCode = *parmfp++;
    sSSCallBar->nNumEntries = *parmfp++;
    for (i = 0; i < sSSCallBar->nNumEntries; i++)
    {
        sSSCallBar->sList[i].bBasicServGrpInfoPresent = *parmfp++;
        sSSCallBar->sList[i].eBasicServGrpType = *parmfp++;
        sSSCallBar->sList[i].eBasicServGrpCode = *parmfp++;
        sSSCallBar->sList[i].bServStatusInfoPresent = *parmfp++;
        sSSCallBar->sList[i].nServStatus = *parmfp++;
    }
}

/**
 * Name:     SwiUpkGsmSSInfoDataRsp
 *
 * Purpose:  This function unpacks the Supplementary Service Info Data 
 *           information from the CnS object's parameter field
 *
 * Parameters: paramfieldp           - Pointer to the CnS parameter field 
 *             sSSInfoData           - Pointer to storage into which the
 *                                     Supplementary Service information will 
 *                                     be placed by this function.
 *
 * Return:    None
 *
 * Note:      None
 *
 */
package void SwiUpkGsmSSInfoDataRsp(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_SS_RespInfoData *sSSInfoData )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    swi_uint32   i;           /* index variable */
    
    parmfp = paramfieldp;

    sSSInfoData->nVersion = piget16( &parmfp );
    sSSInfoData->nHandle = piget32( &parmfp );
    sSSInfoData->bServiceCodePresent = *parmfp++;
    sSSInfoData->eServiceCode = *parmfp++;
    sSSInfoData->bServStatusPresent = *parmfp++;
    sSSInfoData->eServStatus = *parmfp++;
    sSSInfoData->bOptionInfoPresent = *parmfp++;
    sSSInfoData->eSubsOptionType = *parmfp++;
    sSSInfoData->eSubsOptionCode = *parmfp++;
    sSSInfoData->nNumEntries = *parmfp++;
    for (i = 0; i < sSSInfoData->nNumEntries; i++)
    {
        sSSInfoData->sList[i].eBasicServGrpType = *parmfp++;
        sSSInfoData->sList[i].eBasicServGrpCode = *parmfp++;
    }
}

/**
 * Name:     SwiUpkGsmSSStatusRsp
 *
 * Purpose:  This function unpacks the Supplementary Service Status 
 *           data from the CnS object's parameter field
 *
 * Parameters: paramfieldp           - Pointer to the CnS parameter field 
 *             sSSStatus             - Pointer to storage into which the
 *                                     Supplementary Service information will 
 *                                     be placed by this function.
 *
 * Return:    None
 *
 * Note:      None
 *
 */
package void SwiUpkGsmSSStatusRsp(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_SS_RespStatus *sSSStatus )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    
    parmfp = paramfieldp;

    sSSStatus->nVersion = piget16( &parmfp );
    sSSStatus->nHandle = piget32( &parmfp );
    sSSStatus->nServiceStatus = *parmfp++;
}

/**
 * Name:     SwiUpkGsmSSFwdNumRsp
 *
 * Purpose:  This function unpacks the Supplementary Service Forwarding 
 *           Number data from the CnS object's parameter field
 *
 * Parameters: paramfieldp           - Pointer to the CnS parameter field 
 *             sSSFwdNum             - Pointer to storage into which the
 *                                     Supplementary Service information will 
 *                                     be placed by this function.
 *
 * Return:    None
 *
 * Note:      None
 *
 */
package void SwiUpkGsmSSFwdNumRsp(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_SS_RespFwdNum *sSSFwdNum )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    swi_uint32   i;           /* index variable */
    
    parmfp = paramfieldp;

    sSSFwdNum->nVersion = piget16( &parmfp );
    sSSFwdNum->nHandle = piget32( &parmfp );
    sSSFwdNum->bInternational = *parmfp++;
    sSSFwdNum->nLengthFwdNum = *parmfp++;
    for (i = 0; i < sSSFwdNum->nLengthFwdNum; i++)
    {
        sSSFwdNum->byFwdNum[i] = *parmfp++;
    }
}

/**
 * Name:     SwiUpkGsmSSBSListRsp
 *
 * Purpose:  This function unpacks the Supplementary Service BS List 
 *           data from the CnS object's parameter field
 *
 * Parameters: paramfieldp           - Pointer to the CnS parameter field 
 *             sSSBSList             - Pointer to storage into which the
 *                                     Supplementary Service information will 
 *                                     be placed by this function.
 *
 * Return:    None
 *
 * Note:      None
 *
 */
package void SwiUpkGsmSSBSListRsp(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_SS_RespBSList *sSSBSList )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    swi_uint32   i;           /* index variable */
    
    parmfp = paramfieldp;

    sSSBSList->nVersion = piget16( &parmfp );
    sSSBSList->nHandle = piget32( &parmfp );
    sSSBSList->nNumEntries = *parmfp++;
    for (i = 0; i < sSSBSList->nNumEntries; i++)
    {
        sSSBSList->sList[i].eBasicServGrpType = *parmfp++;
        sSSBSList->sList[i].eBasicServGrpCode = *parmfp++;
    }
}

/**
 * Name:     SwiUpkGsmSSFwdListRsp
 *
 * Purpose:  This function unpacks the Supplementary Service Forwarding 
 *           List info from the CnS object's parameter field
 *
 * Parameters: paramfieldp           - Pointer to the CnS parameter field 
 *             sSSFwdList            - Pointer to storage into which the
 *                                     Supplementary Service information will 
 *                                     be placed by this function.
 *
 * Return:    None
 *
 * Note:      None
 *
 */
package void SwiUpkGsmSSFwdListRsp(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_SS_RespListFwdInfo *sSSFwdList )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    swi_uint32   i;           /* index variable */
    swi_uint32   j;           /* index variable */
    
    parmfp = paramfieldp;

    sSSFwdList->nVersion = piget16( &parmfp );
    sSSFwdList->nHandle = piget32( &parmfp );
    sSSFwdList->nNumEntries = *parmfp++;
    for (i = 0; i < sSSFwdList->nNumEntries; i++)
    {
        sSSFwdList->sList[i].bBasicServGrpInfoPresent = *parmfp++;
        sSSFwdList->sList[i].eBasicServGrpType = *parmfp++;
        sSSFwdList->sList[i].eBasicServGrpCode = *parmfp++;
        sSSFwdList->sList[i].bServStatusInfoPresent = *parmfp++;
        sSSFwdList->sList[i].eServStatus = *parmfp++;
        sSSFwdList->sList[i].bInternational = *parmfp++;
        sSSFwdList->sList[i].nLengthFwdNum = *parmfp++;
        for (j = 0; j < sSSFwdList->sList[i].nLengthFwdNum; j++)
        {
            sSSFwdList->sList[i].byFwdNum[j] = *parmfp++;
        }
        sSSFwdList->sList[i].bFwdOptionsInfoPresent = *parmfp++;
        sSSFwdList->sList[i].eFwdOptions = *parmfp++;
        sSSFwdList->sList[i].bNoReplyTimePresent = *parmfp++;
        sSSFwdList->sList[i].NoReplyTime = *parmfp++;
    }
}

/**
 * Name:     SwiUpkGsmSSRawRsp
 *
 * Purpose:  This function unpacks the Supplementary Service Raw 
 *           info from the CnS object's parameter field
 *
 * Parameters: paramfieldp           - Pointer to the CnS parameter field 
 *             sSSRaw                - Pointer to storage into which the
 *                                     Supplementary Service information will 
 *                                     be placed by this function.
 *
 * Return:    None
 *
 * Note:      None
 *
 */
package void SwiUpkGsmSSRawRsp(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_SS_RespRaw *sSSRaw )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    swi_uint32   i;           /* index variable */
    
    parmfp = paramfieldp;

    sSSRaw->nVersion = piget16( &parmfp );
    sSSRaw->nHandle = piget32( &parmfp );
    sSSRaw->nLength = *parmfp++;
    for (i = 0; i < sSSRaw->nLength; i++)
    {
        sSSRaw->byData[i] = *parmfp++;
    }
}

/**
 * Name:     SwiUpkGsmSSPwdRsp
 *
 * Purpose:  This function unpacks the Supplementary Service Password 
 *           info from the CnS object's parameter field
 *
 * Parameters: paramfieldp           - Pointer to the CnS parameter field 
 *             sSSPwd                - Pointer to storage into which the
 *                                     Supplementary Service information will 
 *                                     be placed by this function.
 *
 * Return:    None
 *
 * Note:      None
 *
 */
package void SwiUpkGsmSSPwdRsp(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_SS_RespPassword *sSSPwd )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    swi_uint32   i;           /* index variable */
    
    parmfp = paramfieldp;

    sSSPwd->nVersion = piget16( &parmfp );
    sSSPwd->nHandle = piget32( &parmfp );
    sSSPwd->nPasswordLength = *parmfp++;
    for (i = 0; i < sSSPwd->nPasswordLength; i++)
    {
        sSSPwd->byPassword[i] = *parmfp++;
    }
}

/**
 * Name:     SwiUpkGsmSSClirRsp
 *
 * Purpose:  This function unpacks the Supplementary Service CLIR 
 *           data from the CnS object's parameter field
 *
 * Parameters: paramfieldp           - Pointer to the CnS parameter field 
 *             sSSClir               - Pointer to storage into which the
 *                                     Supplementary Service information will 
 *                                     be placed by this function.
 *
 * Return:    None
 *
 * Note:      None
 *
 */
package void SwiUpkGsmSSClirRsp(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_SS_RespClir *sSSClir )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    
    parmfp = paramfieldp;

    sSSClir->nVersion = piget16( &parmfp );
    sSSClir->nHandle = piget32( &parmfp );
    sSSClir->eServStatus = *parmfp++;
    sSSClir->eClir = *parmfp++;
}

/**
 * Name:     SwiUpkGsmSSErrorRsp
 *
 * Purpose:  This function unpacks the Supplementary Service Error 
 *           data from the CnS object's parameter field
 *
 * Parameters: paramfieldp           - Pointer to the CnS parameter field 
 *             sSSError              - Pointer to storage into which the
 *                                     Supplementary Service information will 
 *                                     be placed by this function.
 *
 * Return:    None
 *
 * Note:      None
 *
 */
package void SwiUpkGsmSSErrorRsp(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_SS_RespError *sSSError )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    
    parmfp = paramfieldp;

    sSSError->nVersion = piget16( &parmfp );
    sSSError->nHandle = piget32( &parmfp );
    sSSError->eErrorCode = *parmfp++;
    sSSError->eErrorType = *parmfp++;
    sSSError->uErrorCause.eCallBarring = *parmfp++;
    sSSError->uErrorCause.eServStatus = *parmfp++;
    sSSError->uErrorCause.sSubscripViolation.eSubsOptionType = *parmfp++;
    sSSError->uErrorCause.sSubscripViolation.eSubsOptionCode = *parmfp++;
    sSSError->uErrorCause.bAbsentSubs = *parmfp++;
    sSSError->uErrorCause.eSystemFail = *parmfp++;
    sSSError->uErrorCause.ePasswdRegFail = *parmfp++;
    sSSError->uErrorCause.sIncompatibilityReason.bServiceCodePresent = *parmfp++;
    sSSError->uErrorCause.sIncompatibilityReason.eServiceCode = *parmfp++;
    sSSError->uErrorCause.sIncompatibilityReason.bBasicServGrpPresent = *parmfp++;
    sSSError->uErrorCause.sIncompatibilityReason.eBasicServGrpType = *parmfp++;
    sSSError->uErrorCause.sIncompatibilityReason.eBasicServGrpCode = *parmfp++;
    sSSError->uErrorCause.sIncompatibilityReason.bServStatusInfoPresent = *parmfp++;
    sSSError->uErrorCause.sIncompatibilityReason.eServStatusErr = *parmfp++;
}

/**
 * Name:     SwiUpkGsmSSRejectRsp
 *
 * Purpose:  This function unpacks the Supplementary Service Reject 
 *           data from the CnS object's parameter field
 *
 * Parameters: paramfieldp           - Pointer to the CnS parameter field 
 *             sSSReject             - Pointer to storage into which the
 *                                     Supplementary Service information will 
 *                                     be placed by this function.
 *
 * Return:    None
 *
 * Note:      None
 *
 */
package void SwiUpkGsmSSRejectRsp(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_SS_RespReject *sSSReject )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    
    parmfp = paramfieldp;

    sSSReject->nVersion = piget16( &parmfp );
    sSSReject->nHandle = piget32( &parmfp );
    sSSReject->eProblemType = *parmfp++;
    sSSReject->eRejectCode = *parmfp++;
}

/**
 * Name:     SwiUpkGsmSSCancelRsp
 *
 * Purpose:  This function unpacks the Supplementary Service Cancel 
 *           data from the CnS object's parameter field
 *
 * Parameters: paramfieldp           - Pointer to the CnS parameter field 
 *             sSSCancel             - Pointer to storage into which the
 *                                     Supplementary Service information will 
 *                                     be placed by this function.
 *
 * Return:    None
 *
 * Note:      None
 *
 */
package void SwiUpkGsmSSCancelRsp(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_SS_RespCancel *sSSCancel )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    
    parmfp = paramfieldp;

    sSSCancel->nVersion = piget16( &parmfp );
    sSSCancel->nHandle = piget32( &parmfp );
    sSSCancel->eCancelReason = *parmfp++;
}
/*
 * $Log: SwiIntUpkGsmVoice.c,v $
 */
