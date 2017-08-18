/*
 * \ingroup cbk
 *
 * \file    qaCbkImsaNotify.c
 *
 * \brief   Contains routines for the IMSA Notifications
 *
 * Copyright: © 2014 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaGobiApiCbk.h"
#include "qaQmiNotify.h"
#include "sludefs.h"

/*************
 *
 * Name:    qaQmiIMSConfigNotify
 *
 * Purpose: To notify all the Callbacks associated with the IMSA Indications 
 *
 * Parms:   pQmiIndication  [IN] - pointer to structure used to store all QMI
 *                                 Notification parameters.
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   Must be called after qminit() has been called
 *
 **************/
local void qaQmiIMSAIndicationsNotify(
    struct QmiNotification *pQmiIndication )
{
    void  *pCallback;
    enum  eQMICallbackIndex CbkIndex;
    ULONG CbkDefValLong = 0xFFFFFFFF;
    WORD  CbkDefValWord = 0xFFFF;
    BYTE  CbkDefValByte = 0xFF;

    CbkIndex  = eQMI_CB_IMSA_REG_STATUS_IND;
    pCallback = qaQmiGetCallback( CbkIndex );
    if ( pCallback )
    {
        imsaRegStatusInfo IMSARegStatusNotification;

        /* Initialize the data structure */
        slmemset( (char*) &IMSARegStatusNotification,
                  0,
                  sizeof( imsaRegStatusInfo ) );

        /* Set the default Values */
        IMSARegStatusNotification.pbIMSRegistered = &CbkDefValByte;
        IMSARegStatusNotification.pRegStatusErrorCode = &CbkDefValWord;
        IMSARegStatusNotification.pImsRegStatus = &CbkDefValLong;

        struct IMSRegStatusTlv *pIMSRegStatus =
                &pQmiIndication->QmiInd.qaQmiCbkImsaRegStatusInfo.IMSRegistration;

        /* The TLV was present in the Indication - hence process */
        if ( pIMSRegStatus->TlvPresent )
        {
            /* Extract the Parameters */
            IMSARegStatusNotification.pbIMSRegistered = &pIMSRegStatus->ImsRegistered;
        }
     
        struct IMSRegStatusErrorCodeTlv *pIMSRegStatusError =
                &pQmiIndication->QmiInd.qaQmiCbkImsaRegStatusInfo.IMSRegistrationError;

        /* The TLV was present in the Indication - hence process */
        if ( pIMSRegStatusError->TlvPresent )
        {
            /* Extract the Parameters */
            IMSARegStatusNotification.pRegStatusErrorCode = &pIMSRegStatusError->ErrorCode;
        }
 
        struct NewIMSRegStatusTlv *pNewIMSRegStatus =
                &pQmiIndication->QmiInd.qaQmiCbkImsaRegStatusInfo.NewIMSRegistration;

        /* The TLV was present in the Indication - hence process */
        if ( pNewIMSRegStatus->TlvPresent )
        {
            /* Extract the Parameters */
            IMSARegStatusNotification.pImsRegStatus = &pNewIMSRegStatus->ImsRegStatus;
        }  

        /* Invoke the callback */
        ((tFNImsaRegStatus)pCallback) ( &IMSARegStatusNotification );
    }

    CbkIndex  = eQMI_CB_IMSA_SVC_STATUS_IND;
    pCallback = qaQmiGetCallback( CbkIndex );
    if ( pCallback )
    {
        imsaSvcStatusInfo IMSASvcStatusNotification;

        /* Initialize the data structure */
        slmemset( (char*) &IMSASvcStatusNotification,
                  0,
                  sizeof( imsaSvcStatusInfo ) );

       /* Set the default Values */        
        IMSASvcStatusNotification.pSMSSvcStatus = &CbkDefValLong;
        IMSASvcStatusNotification.pVOIPSvcStatus = &CbkDefValLong;
        IMSASvcStatusNotification.pVTSvcStatus = &CbkDefValLong;
        IMSASvcStatusNotification.pSMSSvcRAT = &CbkDefValLong;
        IMSASvcStatusNotification.pVOIPSvcRAT = &CbkDefValLong;
        IMSASvcStatusNotification.pVTSvcRAT = &CbkDefValLong;
        IMSASvcStatusNotification.pUTSvcStatus = &CbkDefValLong;
        IMSASvcStatusNotification.pUTSvcRAT = &CbkDefValLong;

        struct SmsSvcStatusTlv *pSmsSvcStatus =
                &pQmiIndication->QmiInd.qaQmiCbkImsaSvcStatusInfo.SmsService;
 

        /* The TLV was present in the Indication - hence process */
        if ( pSmsSvcStatus->TlvPresent )
        {
            /* Extract the Parameters */
            IMSASvcStatusNotification.pSMSSvcStatus = &pSmsSvcStatus->SmsSvcStatus;
        }

        struct VoipSvcStatusTlv *pVoipSvcStatus =
                &pQmiIndication->QmiInd.qaQmiCbkImsaSvcStatusInfo.VoipService;

        /* The TLV was present in the Indication - hence process */
        if ( pVoipSvcStatus->TlvPresent )
        {
            /* Extract the Parameters */
            IMSASvcStatusNotification.pVOIPSvcStatus = &pVoipSvcStatus->VoipSvcStatus;
        }

        struct VtSvcStatusTlv *pVtSvcStatus =
                &pQmiIndication->QmiInd.qaQmiCbkImsaSvcStatusInfo.VtService;

        /* The TLV was present in the Indication - hence process */
        if ( pVtSvcStatus->TlvPresent )
        {
            /* Extract the Parameters */
            IMSASvcStatusNotification.pVTSvcStatus = &pVtSvcStatus->VtSvcStatus;
        }

        struct SmsRatTlv *pSmsRat =
                &pQmiIndication->QmiInd.qaQmiCbkImsaSvcStatusInfo.SmsRat;

        /* The TLV was present in the Indication - hence process */
        if ( pSmsRat->TlvPresent )
        {
            /* Extract the Parameters */
            IMSASvcStatusNotification.pSMSSvcRAT = &pSmsRat->SmsRatVal;
        }

        struct VoipRatTlv *pVoipRat =
                &pQmiIndication->QmiInd.qaQmiCbkImsaSvcStatusInfo.VoipRat;

        /* The TLV was present in the Indication - hence process */
        if ( pVoipRat->TlvPresent )
        {
            /* Extract the Parameters */
            IMSASvcStatusNotification.pVOIPSvcRAT = &pVoipRat->VoipRatVal;
        }

        struct VtRatTlv *pVtRat =
                &pQmiIndication->QmiInd.qaQmiCbkImsaSvcStatusInfo.VtRat;

        /* The TLV was present in the Indication - hence process */
        if ( pVtRat->TlvPresent )
        {
            /* Extract the Parameters */
            IMSASvcStatusNotification.pVTSvcRAT = &pVtRat->VtRatVal;
        }

        struct UtSvcStatusTlv *pUtSvcStatus =
                &pQmiIndication->QmiInd.qaQmiCbkImsaSvcStatusInfo.UtService;

        /* The TLV was present in the Indication - hence process */
        if ( pUtSvcStatus->TlvPresent )
        {
            /* Extract the Parameters */
            IMSASvcStatusNotification.pUTSvcStatus = &pUtSvcStatus->UtSvcStatus;
        }

        struct UtRatTlv *pUtRat =
                &pQmiIndication->QmiInd.qaQmiCbkImsaSvcStatusInfo.UtRat;

        /* The TLV was present in the Indication - hence process */
        if ( pUtRat->TlvPresent )
        {
            /* Extract the Parameters */
            IMSASvcStatusNotification.pUTSvcRAT = &pUtRat->UtRatVal;
        }  

        /* Invoke the callback */
        ((tFNImsaSvcStatus)pCallback) ( &IMSASvcStatusNotification );
    }

    CbkIndex  = eQMI_CB_IMSA_RAT_HANDOVER_STATUS_IND;
    pCallback = qaQmiGetCallback( CbkIndex );
    if ( pCallback )
    {
        imsaRatStatusInfo IMSARatStatusNotification;

        /* Initialize the data structure */
        slmemset( (char*) &IMSARatStatusNotification,
                  0,
                  sizeof( imsaRatStatusInfo ) );

        struct RatHandoverStatusTlv *pRatStatus =
                &pQmiIndication->QmiInd.qaQmiCbkImsaRatStatusInfo.RatHandover;

        /* The TLV was present in the Indication - hence process */
        if ( pRatStatus->TlvPresent )
        {
            /* Extract the Parameters */
            IMSARatStatusNotification.pRATStatus = &pRatStatus->RatHandoverStatus;
            IMSARatStatusNotification.pSrcRAT = &pRatStatus->SourceRAT;
            IMSARatStatusNotification.pTgtRAT = &pRatStatus->TargetRAT;
            if (pRatStatus->ErrorCodeLen)
                IMSARatStatusNotification.pErrorCodeStr =  pRatStatus->ErrorCodeData ;            
        }
     
        /* Invoke the callback */
        ((tFNImsaRatStatus)pCallback) ( &IMSARatStatusNotification );
    }    
}

/*************
 *
 * Name:   UpkQmiCbkIMSANotification
 *
 * Purpose: Unpacks the received Imsa indication and invokes the approriate
 *          callback based on the QMI message type.
 *
 * Parms:   QmiMsgID        - QMI Message ID
 *          pMdmResp   [IN] - Pointer to packed response from the modem.
 *          pNotifResp [IN] - Notification Structure to fill in the parameters.
 *
 * Return:  eQCWWAN_ERR_NONE on success, eQCWWAN_ERR_XXX otherwise
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
package enum eQCWWANError UpkQmiCbkIMSANotification(
    USHORT                 QmiMsgID,
    BYTE                   *pMdmResp,
    struct QmiNotification *pNotifResp )
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    switch(QmiMsgID)
    {
        case eQMI_IMSA_REGISTRATION_STATUS_IND:
        {
            struct QmiCbkImsaRegStatusInd *pResp =
                &pNotifResp->QmiInd.qaQmiCbkImsaRegStatusInfo;

            /* Set all the TLVs to be in Not-Present State */
            pResp->IMSRegistration.TlvPresent  = FALSE;
            pResp->IMSRegistrationError.TlvPresent  = FALSE;
            pResp->NewIMSRegistration.TlvPresent  = FALSE;

            /* Unpack the IMS Registration Status Indication */
            eRCode = UpkQmiCbkImsaRegStatusInd ( pMdmResp,
                                             pResp );

            /* Notify to the Callbacks associated */
            qaQmiIMSAIndicationsNotify( pNotifResp );
            break;
        }
        case eQMI_IMSA_SERVICE_STATUS_IND:
        {
            struct QmiCbkImsaSvcStatusInd *pResp =
                &pNotifResp->QmiInd.qaQmiCbkImsaSvcStatusInfo;

            /* Set all the TLVs to be in Not-Present State */
            pResp->SmsService.TlvPresent  = FALSE;
            pResp->VoipService.TlvPresent  = FALSE;
            pResp->VtService.TlvPresent  = FALSE;
            pResp->SmsRat.TlvPresent  = FALSE;
            pResp->VoipRat.TlvPresent  = FALSE;
            pResp->VtRat.TlvPresent  = FALSE;
            pResp->UtService.TlvPresent  = FALSE;
            pResp->UtRat.TlvPresent  = FALSE;

            /* Unpack the Service Status Indication */
            eRCode = UpkQmiCbkImsaSvcStatusInd ( pMdmResp,
                                                pResp );

            /* Notify to the Callbacks associated */
            qaQmiIMSAIndicationsNotify( pNotifResp );
            break;
        }
        case eQMI_IMSA_RAT_HANDOVER_STATUS_IND:
        {
            struct QmiCbkImsaRatStatusInd *pResp =
                &pNotifResp->QmiInd.qaQmiCbkImsaRatStatusInfo;

            /* Set all the TLVs to be in Not-Present State */
            pResp->RatHandover.TlvPresent   = FALSE;

            /* Unpack the RAT Handover Status Indication */
            eRCode = UpkQmiCbkImsaRatStatusInd ( pMdmResp,
                                             pResp );

            /* Notify to the Callbacks associated */
            qaQmiIMSAIndicationsNotify( pNotifResp );
            break;
        }
        default:
        {
            eRCode = eQCWWAN_ERR_INVALID_ARG;
            break;
        }
    }

    return eRCode;
}
