/*
 * \ingroup cbk
 *
 * \file    qaCbkSwiOmaDmNotifyExt.c
 *
 * \brief   Contains routines for the SWIOMA DM Notifications.
 *
 * Copyright: ©  2013 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaGobiApiCbk.h"
#include "qaCbkSwiOmaDmEventReportInd.h"
#include "qaQmiNotify.h"


extern enum eQCWWANError UpkQmiCbkSwiOmaDmEventReportIndExt(
    BYTE                                      *pMdmResp,
    struct QmiCbkSwiOmaDmEventStatusReportInd *pApiResp );

/*************
 *
 * Name:    qaQmiSwiOmaDmEventNotifyExt
 *
 * Purpose: To notify all the Callbacks associated with the SWIOMA-DM Event
 *          Report Notification.
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
local void qaQmiSwiOmaDmEventNotifyExt(
    struct QmiNotification *pQmiIndication )
{
    void  *pCallback;
    enum  eQMICallbackIndex CbkIndex;
    ULONG CbkParamOne;  /* Session Type */
    BYTE  *CbkParamTwo; /* Session Information : Union */

    CbkIndex  = eQMI_CB_SWIOMADM_ALERT;
    pCallback = qaQmiGetCallback( CbkIndex );
    if ( pCallback )
    {
        struct sessionInfoTlvExt *pResp =
            &pQmiIndication->QmiInd.qaQmiCbkSwiOmaDmEventStatusReportInd.SITlv;

        /* The TLV was present in the Indication - hence process */
        /* Included for TLVs which are not implemented */
        if ( pResp->TlvPresent )
        {
            /* Extract the Parameters */
            CbkParamOne = pResp->sessionType;
            CbkParamTwo = (BYTE*)&(pResp->sessionInfo);

            /* Invoke the callback */
            ((tFNSLQSOMADMAlert)pCallback) ( CbkParamOne, CbkParamTwo );
        }
    }
}

/*************
 *
 * Name:    UpkQmiCbkSwiOmaDmNotification
 *
 * Purpose: Unpacks the recevied SWIOMA-DM indication and invokes the approriate
 *          callback based on the QMI message type.
 *
 * Parms:   QmiMsgID   - QMI Message ID
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
package enum eQCWWANError UpkQmiCbkSwiOmaDmNotificationExt(
    USHORT                 QmiMsgID,
    BYTE                   *pMdmResp,
    struct QmiNotification *pNotifResp )
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    switch(QmiMsgID)
    {
        case eQMI_SWIOMA_EVENT_IND:
        {
            struct QmiCbkSwiOmaDmEventStatusReportIndExt *pResp =
                &pNotifResp->QmiInd.qaQmiCbkSwiOmaDmEventStatusReportInd;

            /* Set all the TLVs to be in Not-Present State */
            pResp->SITlv.TlvPresent = FALSE;

            /* Unpack the SWIOMA-DM Event Indication */
            eRCode = UpkQmiCbkSwiOmaDmEventReportIndExt( pMdmResp,
                                                      pResp );

            /* Notify to the Callbacks associated */
            qaQmiSwiOmaDmEventNotifyExt( pNotifResp );
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
