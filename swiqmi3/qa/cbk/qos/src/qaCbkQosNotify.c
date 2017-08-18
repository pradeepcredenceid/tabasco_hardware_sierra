/*
 * \ingroup cbk
 *
 * \file    qaCbkQosNotify.c
 *
 * \brief   Contains routines for the WDS Notifications.
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaGobiApiCbk.h"
#include "qaCbkQosNetworkStatusInd.h"
#include "qaCbkQosEventReportInd.h"
#include "qaQmiNotify.h"
#include <syslog.h>
#include <string.h>

/* Functions */

/*************
 *
 * Name:    qaQmiQosNetworkStatusNotify
 *
 * Purpose: Unpacks the recevied QOS indication and invokes the approriate
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
package void qaQmiQosNetworkStatusNotify(
    struct QmiNotification *pQmiIndication )
{
    void *pCallback;
    enum eQMICallbackIndex CbkIndex;
    BYTE status;

    CbkIndex  = eQMI_CB_QOS_NETWORK_STATUS;
    pCallback = qaQmiGetCallback( CbkIndex );
    if(pCallback)
    {
        struct QmiCbkQosNetworkStatusInd *lResp =
                 &pQmiIndication->QmiInd.qaQmiCbkQosNetworkStatusInd;

        status = lResp->Status;
        /* Invoke the callback */
        ((tFNQosNWStatus)pCallback)( status );
    }
}

package void qaQmiQosEventNotify(
    struct QmiNotification *pQmiIndication,
    BYTE   instance )
{
    void *pCallback;
    enum eQMICallbackIndex CbkIndex;

    CbkIndex = qmiInst2QosEvent(instance);
    pCallback = qaQmiGetCallback( CbkIndex );
    if(pCallback)
    {
        QosFlowInfoState QFlowState;
        QosFlowInfo flowInfo;
        memset(&flowInfo, 0, sizeof(QosFlowInfo));

        swiQosFlow   txQFlowGranted;
        memset(&txQFlowGranted, 0, sizeof(swiQosFlow));

        dataRate    lDataRate;
        ULONG       latency;
        ULONG       jitter;
        BYTE        qci;

        struct FlowInfoTlv *pFlowInfoTlv =
            &pQmiIndication->QmiInd.qaQmiCbkQosEventReportInd.flowInfoTlv;

        /* The TLV was present in the Indication - hence process */
        if( TRUE == pFlowInfoTlv->TlvPresent )
        {
            /* Extract the Parameters */

            QFlowState.id = pFlowInfoTlv->id;
            QFlowState.isNewFlow = pFlowInfoTlv->isNew;
            QFlowState.state = pFlowInfoTlv->state;

            flowInfo.pQFlowState = &QFlowState;
        }

        struct FlowGrantedTlv *pTxFlowTlv =
            &pQmiIndication->QmiInd.qaQmiCbkQosEventReportInd.txFlowTlv;
        if( TRUE == pTxFlowTlv->TlvPresent )
        {
            flowInfo.pTxQFlowGranted = &txQFlowGranted;

            syslog(LOG_DEBUG, "tx flow present\n");
            syslog(LOG_DEBUG, "index %d\n", pTxFlowTlv->index);
            txQFlowGranted.index = pTxFlowTlv->index;

            struct DataRateTlv *pDataRateTlv = &pTxFlowTlv->dataRateTlv;
            if( TRUE == pDataRateTlv->TlvPresent )
            {
                txQFlowGranted.pDataRate = &lDataRate;
                syslog(LOG_DEBUG, "max %ld\n", pDataRateTlv->dataRateMax);
                syslog(LOG_DEBUG, "guaranteed %ld\n", pDataRateTlv->guaranteedRate);
                lDataRate.dataRateMax = pDataRateTlv->dataRateMax;
                lDataRate.guaranteedRate = pDataRateTlv->guaranteedRate;
            }

            struct LatencyTlv *pLatencyTlv = &pTxFlowTlv->latencyTlv;
            if( TRUE == pLatencyTlv->TlvPresent )
            {
                txQFlowGranted.pLatency = &latency;
                syslog(LOG_DEBUG, "latency %ld\n", pLatencyTlv->latency);
                latency = pLatencyTlv->latency;
            }

            struct JitterTlv *pJitterTlv = &pTxFlowTlv->jitterTlv;
            if( TRUE == pJitterTlv->TlvPresent )
            {
                txQFlowGranted.pJitter = &jitter;
                syslog(LOG_DEBUG, "jitter %ld\n", pJitterTlv->jitter);
                jitter = pJitterTlv->jitter;
            }

            struct QciTlv *pQciTlv = &pTxFlowTlv->qciTlv;
            if( TRUE == pQciTlv->TlvPresent )
            {
                txQFlowGranted.pLteQci = &qci;
                syslog(LOG_DEBUG, "qci %d\n", pQciTlv->qci);
                qci = pQciTlv->qci;
            }

        }

        /* Invoke the callback */
        ((tFNSLQSQOSEvent)pCallback)(
            instance,
            &flowInfo
            );
    }
}

package void qaQmiQosFlowStatusNotify(
    struct QmiNotification *pQmiIndication,
    BYTE   instance )
{
    void *pCallback;
    enum eQMICallbackIndex CbkIndex;

    CbkIndex = qmiInst2QosFlowStatusEvent(instance);
    pCallback = qaQmiGetCallback( CbkIndex );
    if(pCallback)
    {
        struct QmiCbkQosFlowStatusInd *lResp =
                 &pQmiIndication->QmiInd.qaQmiCbkQosFlowStatusInd;

        /* Invoke the callback */
        ((tFNQosStatus)pCallback)(
            instance,
            lResp->id,
            lResp->status,
            lResp->event,
            lResp->reason
            );
    }
}

package void qaQmiQosPriEventNotify(
    struct QmiNotification *pQmiIndication
    )
{
    void *pCallback;
    pCallback = qaQmiGetCallback( eQMI_CB_QOS_PRI_FLOW );

    if(pCallback)
    {
        struct QmiCbkQosPriEventInd *lResp =
                 &pQmiIndication->QmiInd.qaQmiCbkQosPriEventInd;

        /* Invoke the callback */
        ((tFNQosPriEvent)pCallback)( lResp->event );
    }
}

/*************
 *
 * Name:    UpkQmiCbkQosNotification
 *
 * Purpose: Unpacks the recevied QOS indication and invokes the approriate
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
package enum eQCWWANError UpkQmiCbkQosNotification(
    USHORT                  QmiMsgID,
    BYTE                    *pMdmResp,
    struct QmiNotification  *pNotifResp,
    BYTE                    instance )
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    switch (QmiMsgID)
    {
        case eQMI_QOS_EVENT_IND:
        {
            struct QmiCbkQosEventReportInd *pResp =
                &pNotifResp->QmiInd.qaQmiCbkQosEventReportInd;

            //TODO add instance

            memset(pResp, 0, sizeof(struct QmiCbkQosEventReportInd));

            /* Unpack the QOS Network Support Status Indication */
            eRCode = UpkQmiCbkQosEventInd( pMdmResp, pResp );
            /* Notify to the Callbacks associated */
            qaQmiQosEventNotify( pNotifResp, instance );
            break;
        }
        case eQMI_QOS_NETWORK_STATUS_IND:
        {
            struct QmiCbkQosNetworkStatusInd *pResp =
                &pNotifResp->QmiInd.qaQmiCbkQosNetworkStatusInd;

            /* Unpack the QOS Network Support Status Indication */
            eRCode = UpkQmiCbkQosNetworkStatusInd( pMdmResp,
                                                      pResp );
            /* Notify to the Callbacks associated */
            qaQmiQosNetworkStatusNotify( pNotifResp );
            break;
        }
        case eQMI_QOS_FLOW_STATUS_IND:
        {
            struct QmiCbkQosFlowStatusInd *pResp =
                &pNotifResp->QmiInd.qaQmiCbkQosFlowStatusInd;

            eRCode = UpkQmiCbkQosFlowStatusInd( pMdmResp, pResp );
            qaQmiQosFlowStatusNotify( pNotifResp, instance );
            break;
        }
        case eQMI_QOS_PRIMARY_QOS_EVENT_IND:
        {
            struct QmiCbkQosPriEventInd *pResp =
                &pNotifResp->QmiInd.qaQmiCbkQosPriEventInd;

            eRCode = UpkQmiCbkQosPriEventInd( pMdmResp, pResp );
            qaQmiQosPriEventNotify( pNotifResp );
            break;
        }
        default:
            break;
    }
    return eRCode;
}

