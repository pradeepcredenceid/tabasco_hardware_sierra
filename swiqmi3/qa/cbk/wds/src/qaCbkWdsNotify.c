/*
 * \ingroup cbk
 *
 * \file    qaCbkWdsNotify.c
 *
 * \brief   Contains routines for the WDS Notifications.
 *
 * Copyright: © 2011-2013 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaGobiApiCbk.h"
#include "qaCbkWdsEventReportInd.h"
#include "qaCbkWdsGetPktSrvcStatusInd.h"
#include "qaQmiNotify.h"
#include "sludefs.h"
#include <string.h>

/* Functions */

/*************
 *
 * Name:    qaQmiWdsPktStatusNotify
 *
 * Purpose: Unpacks the recevied WDS indication and invokes the approriate
 *          callback based on the QMI message type.
 *
 * Parms:   pQmiIndication  [IN] - pointer to structure used to store all QMI
 *                                 Notification parameters.
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
package void qaQmiWdsPktStatusNotify(
    struct QmiNotification *pQmiIndication )
{
    void *pCallback;
    enum eQMICallbackIndex CbkIndex;

    CbkIndex  = eQMI_CB_SESSION_STATE;
    pCallback = qaQmiGetCallback( CbkIndex );
    if(pCallback)
    {
        slqsSessionStateInfo sessionStateInfo;
        struct PktSrvStatusTlv *lResp =
            &pQmiIndication->QmiInd.qaQmiCbkWdsSessionStateInd.pktStatTlv;

        struct SessEndReasonTlv *pResp =
            &pQmiIndication->QmiInd.qaQmiCbkWdsSessionStateInd.sERTlv;

        sessionStateInfo.pQmiInterfaceInfo = &pQmiIndication->qmiInterfaceInfo;
        sessionStateInfo.state = lResp->state;
        sessionStateInfo.reconfiguration_required = lResp->reconfiguration_required;
        sessionStateInfo.sessionEndReason = pResp->sessionEndReason;

        /* Invoke the callback */
        ((tFNSLQSSessionState)pCallback)( &sessionStateInfo );
    }

    CbkIndex  = eQMI_CB_PKT_SERV_STATUS;
    pCallback = qaQmiGetCallback( CbkIndex );
    if(pCallback)
    {
        /* Declare the callback parameters */
        packetSrvStatus PacketSrvStatus;

        /* Initialize the data structure */
        slmemset( (char*) &PacketSrvStatus, 0, sizeof( packetSrvStatus) );

        struct PktSrvStatusTlv *pPktStatTlv =
            &pQmiIndication->QmiInd.qaQmiCbkWdsSessionStateInd.pktStatTlv;

        PacketSrvStatus.pQmiInterfaceInfo = &pQmiIndication->qmiInterfaceInfo;

        /* The TLV was present in the Indication - hence process */
        if( TRUE == pPktStatTlv->TlvPresent )
        {
            /* Extract the Parameters */
            PacketSrvStatus.connStatus = pPktStatTlv->state;
            PacketSrvStatus.reconfigReqd =
                pPktStatTlv->reconfiguration_required;
        }
        else
        {
            /* Assign default max values */
            PacketSrvStatus.connStatus = 0xFF;
            PacketSrvStatus.reconfigReqd = 0xFF;
        }

        struct SessEndReasonTlv *pSERTlv =
            &pQmiIndication->QmiInd.qaQmiCbkWdsSessionStateInd.sERTlv;

        /* The TLV was present in the Indication - hence process */
        if( TRUE == pSERTlv->TlvPresent )
        {
            /* Extract the Parameters */
            PacketSrvStatus.sessionEndReason = pSERTlv->sessionEndReason;
        }
        else
        {
            /* Assign default max values */
            PacketSrvStatus.sessionEndReason = 0xFFFF;
        }

        struct VerbSessEndReasonTlv *pVSERTlv =
            &pQmiIndication->QmiInd.qaQmiCbkWdsSessionStateInd.vSERTlv;

        /* The TLV was present in the Indication - hence process */
        if( TRUE == pVSERTlv->TlvPresent )
        {
            /* Extract the Parameters */
            PacketSrvStatus.verboseSessnEndReasonType =
                pVSERTlv->verboseSessnEndReasonType;
            PacketSrvStatus.verboseSessnEndReason =
                pVSERTlv->verboseSessnEndReason;
        }
        else
        {
            /* Assign default max values */
            PacketSrvStatus.verboseSessnEndReasonType = 0xFFFF;
            PacketSrvStatus.verboseSessnEndReason = 0xFFFF;
        }

        struct IPFamilyTlv *pIPFTlv =
            &pQmiIndication->QmiInd.qaQmiCbkWdsSessionStateInd.IPFTlv;

        /* The TLV was present in the Indication - hence process */
        if( TRUE == pIPFTlv->TlvPresent )
        {
            /* Extract the Parameters */
            PacketSrvStatus.ipFamily = pIPFTlv->ipFamily;
        }
        else
        {
            /* Assign default max values */
            PacketSrvStatus.ipFamily = 0xFF;
        }

        struct TechNameTlv *pTNamTlv =
            &pQmiIndication->QmiInd.qaQmiCbkWdsSessionStateInd.tNamTlv;

        /* The TLV was present in the Indication - hence process */
        if( TRUE == pTNamTlv->TlvPresent )
        {
            /* Extract the Parameters */
            PacketSrvStatus.techName = pTNamTlv->techName;
        }
        else
        {
            /* Assign default max values */
            PacketSrvStatus.techName = 0xFFFF;
        }

        /* Invoke the callback */
        ((tFNPacketSrvState)pCallback)( &PacketSrvStatus );
    }

}

/*************
 *
 * Name:    qaQmiWdsEventNotify
 *
 * Purpose: To notify all the Callbacks associated with the WDS Event Report
 *          Notification.
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
local void qaQmiWdsEventNotify(
    struct QmiNotification *pQmiIndication,
    BYTE                   instance )
{
    void                   *pCallback;
    enum eQMICallbackIndex CbkIndex;
    ULONG                  CbkParamOne;

    CbkIndex  = eQMI_CB_MOBILE_IP;
    pCallback = qaQmiGetCallback( CbkIndex );
    if ( pCallback )
    {
        struct MobileIPStatusTlv *pResp =
                &pQmiIndication->QmiInd.qaQmiCbkWdsEventStatusReportInd.MSTlv;

        /* The TLV was present in the Indication - hence process */
        /* Included for TLVs which are not implemented */

        if ( pResp->TlvPresent )
        {
            /* Extract the Parameters */
            CbkParamOne = pResp->MipStatus;

            /* Invoke the callback */
            ((tFNMobileIPStatus)pCallback) ( CbkParamOne );
        }
    }

    /* Resolve callback by QMI instance id */
    CbkIndex = qmiInst2WdsEvent(instance);

    pCallback = qaQmiGetCallback( CbkIndex );
    if ( pCallback )
    {
        slqsWdsEventInfo WdsEventInfo;
        struct TransferStatsTlv *pResp1 =
               &pQmiIndication->QmiInd.qaQmiCbkWdsEventStatusReportInd.TSTlv;

        WdsEventInfo.pQmiInterfaceInfo = &pQmiIndication->qmiInterfaceInfo;

        memset(&WdsEventInfo, 0, sizeof(WdsEventInfo));

        /* The TLV was present in the Indication - hence process */
        /* Included for TLVs which are not implemented */
        if ( pResp1->TlvPresent )
        {
            /* Extract the Parameters */
            WdsEventInfo.pPacketsCountTX = &pResp1->TxPacketsCount;
            WdsEventInfo.pPacketsCountRX = &pResp1->RxPacketsCount;
            WdsEventInfo.pTotalBytesTX   = &pResp1->TxTotalBytes;
            WdsEventInfo.pTotalBytesRX   = &pResp1->RxTotalBytes;
        }

        struct DormancyStatusTlv *pResp2 =
                &pQmiIndication->QmiInd.qaQmiCbkWdsEventStatusReportInd.DSTlv;

        /* The TLV was present in the Indication - hence process */
        /* Included for TLVs which are not implemented */

        if ( pResp2->TlvPresent )
        {
            /* Extract the Parameters */
            WdsEventInfo.pDormancyStatus = &pResp2->DormancyStatus;
        }

        struct DataBearerTechTlv *pResp3 =
            &pQmiIndication->QmiInd.qaQmiCbkWdsEventStatusReportInd.DBTechTlv;

        struct CurrentDataBearerTechTlv *pResp4 =
            &pQmiIndication->QmiInd.qaQmiCbkWdsEventStatusReportInd.CDBTechTlv;

        /* The TLV was present in the Indication - hence process */
        if( pResp3->TlvPresent || pResp4->TlvPresent )
        {
            /* Extract the Parameters */
            /* If both TLV's are present in the indication use current data
               bearer technology TLV */
            WdsEventInfo.pDataBearer = (pResp4->TlvPresent)?
                &pResp4->RATMask : &pResp3->DataBearerTechnology;

        }

        WdsEventInfo.pQmiInterfaceInfo = &pQmiIndication->qmiInterfaceInfo;

        /* Invoke the callback */
        ((tFNSLQSWDSEvent)pCallback) ( &WdsEventInfo );
    }
}

/*************
 *
 * Name:    UpkQmiCbkWdsNotification
 *
 * Purpose: Unpacks the recevied WDS indication and invokes the approriate
 *          callback based on the QMI message type.
 *
 * Parms:   QmiMsgID        - QMI Message ID
 *          pMdmResp   [IN] - Pointer to packed response from the modem.
 *          pNotifResp [IN] - Notification Structure to fill in the parameters.
 *
 * Parms:   instance    - PDP instance ID
 *
 * Return:  eQCWWAN_ERR_NONE on success, eQCWWAN_ERR_XXX otherwise
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
package enum eQCWWANError UpkQmiCbkWdsNotification(
    USHORT                  QmiMsgID,
    BYTE                    *pMdmResp,
    struct QmiNotification  *pNotifResp,
    BYTE                    instance )
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    switch (QmiMsgID)
    {
        case eQMI_WDS_EVENT_IND:
        {
            struct QmiCbkWdsEventStatusReportInd *pResp =
                &pNotifResp->QmiInd.qaQmiCbkWdsEventStatusReportInd;

            /* Set all the TLVs to be in Not-Present State */
            pResp->DBTechTlv.TlvPresent  = FALSE;
            pResp->DSTlv.TlvPresent      = FALSE;
            pResp->MSTlv.TlvPresent      = FALSE;
            pResp->TSTlv.TlvPresent      = FALSE;
            pResp->CDBTechTlv.TlvPresent = FALSE;

            /* Unpack the WDS Event Indication */
            eRCode = UpkQmiCbkWdsEventReportInd( pMdmResp,
                                                 pResp );
            /* Notify to the Callbacks associated */
            qaQmiWdsEventNotify( pNotifResp, instance );
            break;
        }
        case eQMI_WDS_PKT_STATUS_IND:
        {
            struct QmiCbkWdsSessionStateInd *pResp =
                &pNotifResp->QmiInd.qaQmiCbkWdsSessionStateInd;

            pResp->pktStatTlv.TlvPresent = FALSE;
            pResp->sERTlv.TlvPresent     = FALSE;
            pResp->vSERTlv.TlvPresent    = FALSE;
            pResp->IPFTlv.TlvPresent     = FALSE;
            pResp->tNamTlv.TlvPresent    = FALSE;

            /* Unpack the WDS Packet Status Indication */
            eRCode = UpkQmiCbkWdsGetPktSrvcStatusInd( pMdmResp,
                                                      pResp );
            /* Notify to the Callbacks associated */
            qaQmiWdsPktStatusNotify( pNotifResp );
            break;
        }
        default:
            break;
    }
    return eRCode;
}

