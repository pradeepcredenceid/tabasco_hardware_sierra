/**
 *    \if CvsId 
 *    $Id: SwiApiCdmaSms.c,v 1.4 2010/06/10 00:11:41 epasheva Exp $
 *    \endif
 *    \ingroup cdmasms
 *    \file SwiApiCdmaSms.c
 * 
 *    \brief SWI CDMA specific SMS related APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#include "SwiApiCdmaSms.h"
#include "SwiIntUpkCdmaSms.h"
#include "ap/apidefs.h"

/*! There are two set of SMS APIs. The first SMS related API call decides
    which set of the APIs is used, which is indicated by this flag. */
SMSAPIUsage g_eSmsApiUsage = SMS_Use_Uninitialized;

/**
 *
 * This function reports the modem's SMS capabilities.
 *
 * \param smsMask[OUT]     The destination in which the
 *                         the data is returned. Possible mask
 *                         bit values are defined in
 *                         SWI_TYPE_PRI_SmsMask.
 *
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPriGetSMSMask(swi_uint16* smsMask, swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_SWINV, SWI_NV_PRI_WA_SMS, smsMask,
                          SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets maximum outgoing SMS length
 *
 * \param puMaxLen[OUT]    The destination in which the maximum 
 *                         length in bytes is returned.
 *
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiPriGetMaxMosmsLen(swi_uint16* puMaxLen,
                                       swi_uint32 timeout)
{
    return SwiIntGetNvU16(CNS_PST_SWINV, SWI_NV_PRI_MAX_MOSMS_LEN,
                          puMaxLen, SWI_AMCDMA, timeout);
}

/**
 *
 * This function starts sending stored SMS messages.
 *
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSendCdmaSMS(swi_uint32 timeout)
{
    return SwiIntSet(CNS_SMS_SEND_SMS_CDMA, SWI_AMCDMA, timeout);
}

/**
 *
 * This function queries the outstanding SMS message status
 *
 * \param pSMS_Status[OUT]  Outstanding SMS message status.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetCdmaSMSMessageStatus(
                                    SWI_STRUCT_CDMA_SMS_Status *pSMS_Status,
                                    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         paramlength; /* Returned length of the CnS parm field*/

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Normally a Pack function call would go here, but this 
     * request has no CnS Parameter field to pack
     */

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_SMS_RECEIVED_MSG_STATUS_CDMA, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &paramlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackCdmaSMSMessageStatus( inparamp, pSMS_Status );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * Unpacks the CnS SMS header parameter field.
 *
 * \param   paramp: [IN]            Pointer to parameter field in set request.
 * \param   psmsPacketHeader[OUT]   Memory location to store the unpacked 
 *                                  contents of CnS SMS header.
 *
 * \return  Number of bytes unpacked.
 *
 */
local swi_uint32 UnpackCnsSmsHeader( swi_uint8 *paramfieldp, 
                                    CNS_SMS_HEADER    *psmsPacketHeader)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    /* Assign the local copy */
    parmfp = paramfieldp;

    psmsPacketHeader->messageID = piget16(&parmfp);   /* SMS msg ID */
    psmsPacketHeader->seqNumber = *parmfp++;          /* Message sequence
                                                         number (modulo 256)*/
    psmsPacketHeader->packetType = *parmfp++;         /* 0 : intermediate
                                                         1 : first frame
                                                         2 : last frame
                                                         3 : not fragmented */

    psmsPacketHeader->remainSms = *parmfp++;     /* Number of remaining SMS */
    psmsPacketHeader->length = *parmfp++;        /* Length of this SMS part */

    return (swi_uint32)(parmfp - paramfieldp);
}

/**
 *
 * Unpacks the CDMA SMS header parameter field
 *
 * \param   paramp: [IN]        Pointer to parameter field in set request.
 * \param   pcdmaHeader[OUT]    Memory location to store the unpacked 
 *                              contents of CDMA SMS header.
 *
 */
local void UnpackCdmaSmsHeader( swi_uint8 *paramfieldp, 
                               SWI_STRUCT_SMS_CdmaHeader *pcdmaHeader )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    /* Assign the local copy */
    parmfp = paramfieldp;

    pcdmaHeader->sizeStruct = sizeof(SWI_STRUCT_SMS_CdmaHeader);

    /* Parameter mask for the available fields */
    pcdmaHeader->eParamMask = piget32(&parmfp);

    /* Message Catagory (Point-to-Point/Broadcast/Acknowledge) */
    pcdmaHeader->eMsgCategory = *parmfp++;  /*
                                            --0 :IS-637 SMS Point-to-Point Msg
                                            --1 :IS-637 SMS Broadcast Message
                                            --2 :IS-637 SMS Acknowledge Msg 
                                            */
    /* Teleservice identifier (CMT-91/CPT-95/CMP-95/VMN-95/WAP)*/
    pcdmaHeader->eTeleservice = *parmfp++;     /* Refer to TIA/EIA-41-D Table
                                               175 for identifier values*/

    /* Service Category (News, Restaraunts, etc...)*/
    pcdmaHeader->serviceCategory = piget16(&parmfp);

    /* TL Ack Request*/
    pcdmaHeader->tlAckReq = *parmfp++;

    /* Message ID Type*/
    pcdmaHeader->eMsgIdType = *parmfp++;

    /* Destination/Origination Address */
    pcdmaHeader->addrDigitMode = *parmfp++;
    pcdmaHeader->addrNumMode = *parmfp++;
    pcdmaHeader->addrNumType = *parmfp++;
    pcdmaHeader->addrNumPlan = *parmfp++;
    pcdmaHeader->addrLen = *parmfp++;
    pigetmulti(&parmfp, (swi_uint8 *)pcdmaHeader->szAddress,
                    MAX_SMSI_TL_ADDRESS);

    /* Destination/Origination Sub-address */
    pcdmaHeader->subType = *parmfp++;
    pcdmaHeader->subOdd = *parmfp++;
    pcdmaHeader->subLen = *parmfp++;
    pigetmulti(&parmfp, (swi_uint8 *)pcdmaHeader->szSubAddress,
                     MAX_SMSI_TL_ADDRESS);

    /* Callback number */
    pcdmaHeader->callBackLen = *parmfp++;
    pigetmulti(&parmfp, (swi_uint8 *)pcdmaHeader->szCallBack,
                     MAX_SMSI_CALL_BACK);

    /* Priority Indicator */
    pcdmaHeader->ePriority = *parmfp++;    /* 0: Normal
                                            * 1: Interactive
                                            * 2: Urgent
                                            * 3: Emergency 
                                            */

    /* Privacy Incidator */
    pcdmaHeader->ePrivacy = *parmfp++;     /* 0: Not restricted (Level 0)
                                            * 1: Restricted (Level 1)
                                            * 2: Confidential (Level 2) 
                                            * 3: Secret (Level 3)
                                            */

    /* Language */
    pcdmaHeader->eLanguage = *parmfp++;    /* Refer to TSB58-A */

    /* Reply Option */
    pcdmaHeader->user_ack_req = *parmfp++;
    pcdmaHeader->dak_req = *parmfp++;

    /* User Response */
    pcdmaHeader->userResp = *parmfp++;

    /* Number of message (for Voice Message SMS) */
    pigetmulti(&parmfp, (swi_uint8 *)pcdmaHeader->szNumMsg, 2);

    /* Delivery Alert Enable */
    pcdmaHeader->deliveryAlert = *parmfp++;

    /* Message Body Data encoding */
    pcdmaHeader->dataEncoding = *parmfp++;

    /* Time Stamp */
    pcdmaHeader->ts_yr = *parmfp++;        
    pcdmaHeader->ts_mon = *parmfp++;        
    pcdmaHeader->ts_day = *parmfp++;        
    pcdmaHeader->ts_hrs = *parmfp++;        
    pcdmaHeader->ts_min = *parmfp++;
    pcdmaHeader->ts_sec = *parmfp++;

    /* Absolute Validity Period */
    pcdmaHeader->abVal_yr = *parmfp++;
    pcdmaHeader->abVal_mon = *parmfp++;
    pcdmaHeader->abVal_day = *parmfp++;
    pcdmaHeader->abVal_hrs = *parmfp++;
    pcdmaHeader->abVal_min = *parmfp++;
    pcdmaHeader->abVal_sec = *parmfp++;

    /* Relative Validity Period */
    pcdmaHeader->relVal_time = *parmfp++;

    /* Absolute Deferred Delivery Time */
    pcdmaHeader->abDef_yr = *parmfp++;
    pcdmaHeader->abDef_mon = *parmfp++;
    pcdmaHeader->abDef_day = *parmfp++;
    pcdmaHeader->abDef_hrs = *parmfp++;
    pcdmaHeader->abDef_min = *parmfp++;
    pcdmaHeader->abDef_sec = *parmfp++;

    /* Relative Deferred Delivery Time */
    pcdmaHeader->relDef_time = *parmfp++;

    /* Message Display Mode */
    pcdmaHeader->eMsgDispMode = *parmfp++;
}

/**
 *
 * Unpacks the SMS header parameter field
 *
 * \param   paramp: [IN]            Pointer to parameter field in set request.
 * \param   psmsPacketHeader[OUT]   Memory location to store the unpacked 
 *                                  contents of CnS SMS header.
 * \param   pcdmaHeader[OUT]        Memory location to store the unpacked 
 *                                  contents of CDMA SMS header.
 *
 */
local void UnpackSmsHeader( swi_uint8 *paramfieldp, 
                               CNS_SMS_HEADER    *psmsPacketHeader, 
                               SWI_STRUCT_SMS_CdmaHeader *pcdmaHeader )
{
    swi_uint32 len;     /* sizeof(CNS_SMS_HEADER) */
    
    len = UnpackCnsSmsHeader(paramfieldp, psmsPacketHeader);
    UnpackCdmaSmsHeader(paramfieldp + len, pcdmaHeader);
}

/**
 *
 * This function retrieves SMS header
 *
 * \param   psmsPacketHeader[OUT]   Memory location to store the unpacked 
 *                                  contents of CnS SMS header.
 * \param   pcdmaHeader[OUT]        Memory location to store the unpacked 
 *                                  contents of CDMA SMS header.
 * \param   timeout[IN]             Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
local SWI_RCODE RetrieveSMSHeader ( CNS_SMS_HEADER    *psmsPacketHeader, 
                                    SWI_STRUCT_SMS_CdmaHeader *pcdmaHeader,
                                    swi_uint32             timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         paramlength; /* Returned length of the CnS parm field*/

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Normally a Pack function call would go here, but this 
     * request has no CnS Parameter field to pack
     */

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_SMS_RETRIEVE_SMS, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &paramlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        UnpackSmsHeader(inparamp, psmsPacketHeader, pcdmaHeader);
    }
    
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * Unpacks SMS body
 *
 * \param   paramp: [IN]            Pointer to parameter field in set request.
 * \param   psmsPacketHeader[OUT]   Memory location to store the unpacked 
 *                                  contents of CnS SMS header.
 * \param   pMsgBody[OUT]           Memory location to store the unpacked 
 *                                  contents of SMS body.
 * \param   pSizeSegment[OUT]       Memory location to store the unpacked .
 *                                  contents of segment size.
 *
 */
local void UnpackSmsBody( swi_uint8 *paramfieldp, 
                               CNS_SMS_HEADER    *psmsPacketHeader, 
                               swi_uint8              *pMsgBody,
                               swi_uint32              *pSizeSegment )
{
    swi_uint32 len;     /* sizeof(CNS_SMS_HEADER) */
    
    len = UnpackCnsSmsHeader(paramfieldp, psmsPacketHeader);
    paramfieldp += len;
    *pSizeSegment = psmsPacketHeader->length;
    pigetmulti(&paramfieldp, (swi_uint8 *)pMsgBody, *pSizeSegment);
}

/**
 *
 * This function retrieves SMS body
 *
 * \param   previousSegmentNumber[IN]   Previous segment number.
 * \param   psmsPacketHeader[IN]        SMS packet header. 
 * \param   pMsgBody[OUT]               Memory Location to store message body 
 *                                      retrieved.
 * \param   psizeSegment[OUT]           Segment size. 
 * \param   timeout[IN]                 Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
local SWI_RCODE RetrieveSMSBody (swi_uint8        previousSegmentNumber,
                                         CNS_SMS_HEADER  *psmsPacketHeader,
                                         swi_uint8              *pMsgBody,
                                         swi_uint32             *psizeSegment,
                                         swi_uint32             timeout
                                         )
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         paramlength; /* Returned length of the CnS parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Parameter Field Construction Zone - get a pointer to the 
     * location to fill in the CnS parameter field 
     */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Pack the CnS Parameter field with the input parameters */
    paramlength = SwiPackU16(outparamp, previousSegmentNumber);

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_SMS_RETRIEVE_SMS, 
                                  paramlength,
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &paramlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        UnpackSmsBody(inparamp, psmsPacketHeader, pMsgBody, psizeSegment);
    }
    
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function retrieves SMS.
 *
 * \param pSms[OUT]                 Memory location to store message body 
 *                                  retrieved.
 * \param bDeleteAfterRetrieve[IN]  Delete after SMS is retrieved if true. 
 *                                  Otherwise not delete.
 * \param timeout[IN]               Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
local SWI_RCODE RetrieveSMSMessageEx( SWI_STRUCT_CDMA_SMS_RetrieveSms *pSms,
                                swi_bool bDeleteAfterRetrieve,
                                swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */

    CNS_SMS_HEADER  smsHeader;
    swi_uint8              byteArray[CNS_SMS_MAX_SIZE];
    swi_uint32             sizeMsg = 0;
    swi_uint32                sizeSegment = 0;

    resultcode = RetrieveSMSHeader(&smsHeader, &pSms->sHeader, timeout);

    if ( resultcode != SWI_RCODE_OK) 
    {
        return resultcode;
    }

    /* get msg ID and NumberMsg */
    pSms->nSMSId            = smsHeader.messageID;
    pSms->cntRemainingSMS    = (swi_uint32)smsHeader.remainSms;

    if (smsHeader.length > 0)
    {
        resultcode = RetrieveSMSBody (0, &smsHeader, byteArray,
                                      &sizeSegment, timeout);
        
        if (resultcode == SWI_RCODE_OK)
        {
            sizeMsg += sizeSegment;
            if (sizeMsg > pSms->sizeBuffer)
            {
                return SWI_RCODE_SMALL_BUF;
            }

            /* only one segment for this sms */
            slmemcpy(pSms->pMessage, byteArray, sizeSegment);

            if ( smsHeader.packetType != SMS_NOT_SEGMENTED)
            {
                /* retrieve many segments here */
                while (smsHeader.packetType != SMS_SEGMENT_LAST)
                {
                    /* continuousely retrieve segment for this message */
                    resultcode = RetrieveSMSBody( smsHeader.seqNumber,
                                                  &smsHeader, byteArray,
                                                  &sizeSegment, timeout);
                    if (resultcode != SWI_RCODE_OK)
                    {
                        return SWI_RCODE_FAILED;
                    }
                    else
                    {
                        sizeMsg += sizeSegment;
                        if (sizeMsg > pSms->sizeBuffer)
                            return SWI_RCODE_SMALL_BUF;
                        slmemcpy(pSms->pMessage + sizeMsg, byteArray,
                                 sizeSegment);
                    }
                }
            }
            
            if(bDeleteAfterRetrieve)
            {
                pSms->retrievedStatus = (swi_uint32)SwiSetMessageRetrieved(
                                                       pSms->nSMSId, timeout);
            }
        }

        /*Swap the bytes for wide character codes */
        if (pSms->sHeader.dataEncoding    == SWI_SMS_DATAENCODE_Unicode ||
            pSms->sHeader.dataEncoding    == SWI_SMS_DATAENCODE_Latin   ||
            pSms->sHeader.eLanguage        == SWI_SMS_LANGUAGE_CHINESE)
        {
            swi_uint32 idx;
            swi_uint8 temp;

            for (idx = 0; idx < sizeMsg-1; idx = idx + 2)
            {
                temp = pSms->pMessage [idx];
                pSms->pMessage [idx] = pSms->pMessage [idx + 1];
                pSms->pMessage [idx + 1] = temp;
            }
        }

        pSms->sizeBuffer = sizeMsg;
    }

    return resultcode;
}

/**
 *
 * This function retrieves an SMS message
 *
 * \param pSms[OUT]        SMS message struct.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiRetrieveCdmaSMSMessage(
                                        SWI_STRUCT_CDMA_SMS_RetrieveSms *pSms,
                                        swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */

    if(SMS_Use_New == g_eSmsApiUsage)
    {
        return SWI_RCODE_NOT_SUP;
    }


    resultcode = RetrieveSMSMessageEx(pSms, TRUE, timeout);
    g_eSmsApiUsage = SMS_Use_Legacy;

    return resultcode;
}

/**
 *
 * This function retrieves an SMS message along with an option to delete it
 * or not.
 *
 * \param pSms[OUT]                 Memory to save SMS message retrieved.
 * \param bDeleteAfterRetrieve[IN]  Delete the message from modem after 
 *                                  retrieval if TRUE; Otherwise leave it 
 *                                  in the modem store.
 * \param timeout[IN]               Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiRetrieveSMSMessageEx(
                                 SWI_STRUCT_CDMA_SMS_RetrieveSms *pSms,
                                 swi_bool bDeleteAfterRetrieve,
                                 swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */

    if(SMS_Use_Legacy == g_eSmsApiUsage)
    {
        return SWI_RCODE_NOT_SUP;
    }

    resultcode = RetrieveSMSMessageEx(pSms, bDeleteAfterRetrieve, timeout);
    g_eSmsApiUsage = SMS_Use_New;

    return resultcode;
}

/**
 *
 * This function sets a message as retrieved so modem can delete it
 *
 * \param msgID[IN]        SMS message ID to be deleted.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetMessageRetrieved(swi_uint16 msgID, swi_uint32 timeout)
{
    if(SMS_Use_Legacy == g_eSmsApiUsage)
    {
        return SWI_RCODE_NOT_SUP;
    }
    else
    {
        g_eSmsApiUsage = SMS_Use_New;
        return SwiIntSetU16(CNS_SMS_RETRIEVED,  msgID, NULL,
                            SWI_AMCDMA, timeout);
    }
}

/**
 *
 * Pack SMS header.
 *
 * \param   paramfieldp[OUT]    Pointer to parameter field in set request.
 * \param   psStoreSms[IN]      Pointer to SMS header to pack.
 *
 * \return  Number of bytes packed.
 *
 */
local swi_uint32 PackSmsHeader(swi_uint8 *paramfieldp,
                         SWI_STRUCT_CDMA_SMS_StoreSms *psStoreSms)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */
    swi_uint8  *sdmasmsheaderlenp;  /* sizeof(CDMA_SMS_HEADER)  */
    swi_uint8  *cdmasmsheaderfieldp;  /* point to start of CDMA_SMS_HEADER */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    /* CNS_SMS_HEADER */
    piput16(psStoreSms->nSMSId, &localfieldp);      /* messageID */
    *localfieldp++ = 0;                             /* seqNumber */
    *localfieldp++ = SMS_SEGMENT_FIRST;             /* packetType */
    *localfieldp++ = 0;                             /* remainSms */

    sdmasmsheaderlenp = localfieldp++;              /* Remember the position */
    cdmasmsheaderfieldp = localfieldp;              /* Remember the position */
    
    /* CDMA_SMS_HEADER */
    /* paramMask */
    piput32(psStoreSms->sSMSHeader.eParamMask, &localfieldp);
    /* msgCategory */
    *localfieldp++ = psStoreSms->sSMSHeader.eMsgCategory;
    /* teleservice */
    *localfieldp++ = psStoreSms->sSMSHeader.eTeleservice;
    /* serviceCategory */
    piput16(psStoreSms->sSMSHeader.serviceCategory, &localfieldp);
    /* tlAckReq */
    *localfieldp++ = psStoreSms->sSMSHeader.tlAckReq;
    /* msgIdType */
    *localfieldp++ = psStoreSms->sSMSHeader.eMsgIdType;     
    
    /* Destination/Origination Address */
    *localfieldp++ = psStoreSms->sSMSHeader.addrDigitMode; /* addrDigitMode */
    *localfieldp++ = psStoreSms->sSMSHeader.addrNumMode;   /* addrNumMode */
    *localfieldp++ = psStoreSms->sSMSHeader.addrNumType;   /* addrNumType */
    *localfieldp++ = psStoreSms->sSMSHeader.addrNumPlan;   /* addrNumPlan */
    *localfieldp++ = psStoreSms->sSMSHeader.addrLen;       /* addrLen */
    piputmulti((const swi_uint8 *)psStoreSms->sSMSHeader.szAddress, 
            &localfieldp, psStoreSms->sSMSHeader.addrLen); /* address */

    localfieldp += (MAX_SMSI_TL_ADDRESS - psStoreSms->sSMSHeader.addrLen);

    /* Destination/Origination Sub-address */
    *localfieldp++ = psStoreSms->sSMSHeader.subType;     /* subType */
    *localfieldp++ = psStoreSms->sSMSHeader.subOdd;      /* subOdd */
    *localfieldp++ = psStoreSms->sSMSHeader.subLen;      /* subLen */
    piputmulti((const swi_uint8 *)psStoreSms->sSMSHeader.szSubAddress, 
            &localfieldp, psStoreSms->sSMSHeader.subLen);/* subAddress */

    localfieldp += (MAX_SMSI_TL_ADDRESS - psStoreSms->sSMSHeader.subLen);

    /* Callback number */
    *localfieldp++ = psStoreSms->sSMSHeader.callBackLen;  /* callBackLen */
    piputmulti((const swi_uint8 *)psStoreSms->sSMSHeader.szCallBack, 
            &localfieldp, psStoreSms->sSMSHeader.callBackLen); /* callBack */

    localfieldp += (MAX_SMSI_CALL_BACK - psStoreSms->sSMSHeader.callBackLen);

    /* Priority Indicator */
    *localfieldp++ = psStoreSms->sSMSHeader.ePriority;   /* priority */

    /* Privacy Incidator */
    *localfieldp++ = psStoreSms->sSMSHeader.ePrivacy;    /* privacy */

    /* Language */
    *localfieldp++ = psStoreSms->sSMSHeader.eLanguage;   /* language */

    /* Reply Option */
    *localfieldp++ = psStoreSms->sSMSHeader.user_ack_req; /* user_ack_req */
    *localfieldp++ = psStoreSms->sSMSHeader.dak_req;      /* dak_req */

    /* User Response */
    *localfieldp++ = psStoreSms->sSMSHeader.userResp;      /* userResp */

    /* Number of message (for Voice Message SMS) */
    *localfieldp++ = psStoreSms->sSMSHeader.szNumMsg[0];  /* numMsg */
    *localfieldp++ = psStoreSms->sSMSHeader.szNumMsg[1];  /* numMsg */

    /* Delivery Alert Enable */
    *localfieldp++ = psStoreSms->sSMSHeader.deliveryAlert; /* deliveryAlert */

    /* Message Body Data encoding */
    *localfieldp++ = psStoreSms->sSMSHeader.dataEncoding;  /* dataEncoding */

    /* Time Stamp */
    *localfieldp++ = psStoreSms->sSMSHeader.ts_yr;             /* ts_yr */
    *localfieldp++ = psStoreSms->sSMSHeader.ts_mon;             /* ts_mon */
    *localfieldp++ = psStoreSms->sSMSHeader.ts_day;             /* ts_day */
    *localfieldp++ = psStoreSms->sSMSHeader.ts_hrs;             /* ts_hrs */
    *localfieldp++ = psStoreSms->sSMSHeader.ts_min;             /* ts_min */
    *localfieldp++ = psStoreSms->sSMSHeader.ts_sec;             /* ts_sec */

    /* Absolute Validity Period */
    *localfieldp++ = psStoreSms->sSMSHeader.abVal_yr;         /* abVal_yr */
    *localfieldp++ = psStoreSms->sSMSHeader.abVal_mon;        /* abVal_mon */
    *localfieldp++ = psStoreSms->sSMSHeader.abVal_day;        /* abVal_day */
    *localfieldp++ = psStoreSms->sSMSHeader.abVal_hrs;        /* abVal_hrs */
    *localfieldp++ = psStoreSms->sSMSHeader.abVal_min;        /* abVal_min */
    *localfieldp++ = psStoreSms->sSMSHeader.abVal_sec;        /* abVal_sec */

    /* Relative Validity Period */
    *localfieldp++ = psStoreSms->sSMSHeader.relVal_time;      /* relVal_time */

    /* Absolute Deferred Delivery Time */
    *localfieldp++ = psStoreSms->sSMSHeader.abDef_yr;         /* abDef_yr */
    *localfieldp++ = psStoreSms->sSMSHeader.abDef_mon;        /* abDef_mon */
    *localfieldp++ = psStoreSms->sSMSHeader.abDef_day;        /* abDef_day */
    *localfieldp++ = psStoreSms->sSMSHeader.abDef_hrs;        /* abDef_hrs */
    *localfieldp++ = psStoreSms->sSMSHeader.abDef_min;        /* abDef_min */
    *localfieldp++ = psStoreSms->sSMSHeader.abDef_sec;        /* abDef_sec */
    

    /* Relative Deferred Delivery Time */
    *localfieldp++ = psStoreSms->sSMSHeader.relDef_time;      /* relDef_time */

    /* Message Display Mode */
    *localfieldp++ = psStoreSms->sSMSHeader.eMsgDispMode;     /* msgDispMode */

    /* Set the value of sizeof(CDMA_SMS_HEADER).  Must be sure all values of
    CDMA_SMS_HEADER are packed, so the length can be calucated in below way. */
    *sdmasmsheaderlenp = (swi_uint8)(localfieldp-cdmasmsheaderfieldp);

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * Send SMS header.
 *
 * \param   psStoreSms[IN]  Pointer to SMS header to send.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
local SWI_RCODE SendSMSHeader(SWI_STRUCT_CDMA_SMS_StoreSms *psStoreSms,
                        swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Parameter Field Construction Zone - get a pointer to the 
     * location to fill in the CnS parameter field 
     */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Pack the CnS Parameter field with the input parameters */
    parmlength = PackSmsHeader(outparamp, psStoreSms);

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_SMS_STORE_SMS, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */
                     
    return resultcode; 
}

/**
 *
 * Pack SMS body.
 *
 * \param   paramfieldp[OUT]:   Pointer to parameter field in set request.
 * \param   msgID[IN]           Message ID.
 * \param   segmentType[IN]     Segment type. See SWI_SMS_PACKET_TYPE.
 * \param   packetSeqNumber[IN] Packet segment number.
 * \param   pSegmentBody[IN]    Segment body.
 * \param   lenByteArray[IN]    Length of segment body in byte.
 *
 * \return  Number of bytes packed.
 *
 */
swi_uint32 PackSmsBody(swi_uint8 *paramfieldp, swi_uint16 msgID,
                                     swi_uint8  segmentType,
                                     swi_uint8  packetSeqNumber,
                                     swi_uint8        *pSegmentBody,
                                     swi_uint32       lenByteArray)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    /* CNS_SMS_HEADER */
    piput16(msgID, &localfieldp);      /* messageID */
    *localfieldp++ = packetSeqNumber;         /* seqNumber */
    *localfieldp++ = segmentType;             /* packetType */
    *localfieldp++ = 0;                       /* remainSms */
    *localfieldp++ = lenByteArray;       /* length */

    piputmulti((const swi_uint8 *)pSegmentBody, &localfieldp, lenByteArray);
    
    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * Send SMS body.
 *
 * \param   msgID[IN]           Message ID.
 * \param   segmentType[IN]     Segment type. See SWI_SMS_PACKET_TYPE.
 * \param   packetSeqNumber[IN] Packet segment number.
 * \param   pSegmentBody[IN]    Segment body.
 * \param   lenByteArray[IN]    Length of segment body in byte.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
local SWI_RCODE SendSMSBody (swi_uint16 msgID,
                                     swi_uint8  segmentType,
                                     swi_uint8  packetSeqNumber,
                                     swi_uint8        *pSegmentBody,
                                     swi_uint32       lenByteArray,
                                     swi_uint32       timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Parameter Field Construction Zone - get a pointer to the 
     * location to fill in the CnS parameter field 
     */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Pack the CnS Parameter field with the input parameters */
    parmlength = PackSmsBody(outparamp, msgID, segmentType, packetSeqNumber, 
            pSegmentBody, lenByteArray);

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_SMS_STORE_SMS, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */
                     
    return resultcode; 
}

/**
 *
 * This function is used to store an SMS message.
 *
 * \param psStoreSms[OUT]  SMS message to store.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiStoreCdmaSMSMessage(
                                    SWI_STRUCT_CDMA_SMS_StoreSms *psStoreSms,
                                    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint32          idx;
    swi_uint8           temp;
    swi_uint32          cntSegments;

    resultcode = SendSMSHeader (psStoreSms, timeout);
    if (resultcode != SWI_RCODE_OK)
    {
        return resultcode;
    }

       /*Swap the bytes for wide character codes */
    if (psStoreSms->sSMSHeader.dataEncoding   == SWI_SMS_DATAENCODE_Unicode ||
        psStoreSms->sSMSHeader.dataEncoding   == SWI_SMS_DATAENCODE_Latin   ||
        psStoreSms->sSMSHeader.eLanguage       == SWI_SMS_LANGUAGE_CHINESE)
    {
        for (idx = 0; idx < psStoreSms->sizeSMSBody; idx = idx + 2)
        {
            temp = psStoreSms->pMsgBody [idx];
            psStoreSms->pMsgBody [idx] = psStoreSms->pMsgBody [idx + 1];
            psStoreSms->pMsgBody [idx + 1] = temp;
        }
    }

    if ( psStoreSms->sizeSMSBody > (swi_uint32)CNS_SMS_MAX_SIZE )
    {
        cntSegments = psStoreSms->sizeSMSBody/CNS_SMS_MAX_SIZE;

        /* send intermediate segment */
        for ( idx = 0; idx < cntSegments; idx++)
        {
            SendSMSBody(psStoreSms->nSMSId, SMS_SEGMENT_INTERMEDIATE,
                       (swi_uint8)(idx+1), psStoreSms->pMsgBody,
                       CNS_SMS_MAX_SIZE, timeout);
            psStoreSms->pMsgBody += CNS_SMS_MAX_SIZE;
        }
        resultcode = SendSMSBody(psStoreSms->nSMSId, SMS_SEGMENT_LAST,
                               (swi_uint8)(idx+1), psStoreSms->pMsgBody, 
                               psStoreSms->sizeSMSBody - idx*CNS_SMS_MAX_SIZE,
                               timeout);
    }
    else
    {
        /* send the only one sms segment */
        resultcode = SendSMSBody(psStoreSms->nSMSId, SMS_SEGMENT_LAST, 1,
                                 psStoreSms->pMsgBody,psStoreSms->sizeSMSBody,
                                 timeout);
    }

    return resultcode;
}
/* 
 * $Log: SwiApiCdmaSms.c,v $
 */
