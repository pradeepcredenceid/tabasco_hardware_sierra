/*
 * \ingroup cbk
 *
 * \file    qaCbkLocNotify.c
 *
 * \brief   Contains routines for the LOC Notifications.
 *
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaGobiApiCbk.h"
#include "qaCbkLocCradleMountInd.h"
#include "qaCbkLocEventTimeSyncInd.h"
#include "qaCbkLocInjectTimeInd.h"
#include "qaCbkLocSensorStreamInd.h"
#include "qaCbkLocSensorDataInd.h"
#include "qaCbkLocPositionRptInd.h"
#include "qaCbkLocEventGnssSvInfoInd.h"
#include "qaCbkLocOpModeInd.h"
#include "qaCbkLocAssistDataInd.h"
#include "qaCbkLocInjectUTCTimeInd.h"
#include "qaCbkLocSetServerInd.h"
#include "qaCbkLocGetServerInd.h"
#include "qaCbkLocGetOpModeInd.h"
#include "qaCbkLocGetFixCriteriaInd.h"
#include "qaQmiNotify.h"
#include "sludefs.h"
#include <string.h>

/*************
 *
 * Name:    qaQmiLocCradleMountNotify
 *
 * Purpose: To notify all the Callbacks associated with the Cradle Mount event.
 *
 * Parms:   QmiCbkLocCradleMountInd  [IN] - pointer to structure used to store all Cradle Mount Configuration Status
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
local void qaQmiLocCradleMountNotify(
        QmiCbkLocCradleMountInd *pResp )
{
    void  *pCallback;
    enum  eQMICallbackIndex CbkIndex;

    CbkIndex  = eQMI_CB_LOC_CRADLE_MOUNT_IND;
    pCallback = qaQmiGetCallback( CbkIndex );
    if ( pCallback )
    {
        /* Invoke the callback */
        ((tFNSetCradleMount)pCallback) ( pResp );
    }
}

/*************
 *
 * Name:    qaQmiLocEventTimeSyncNotify
 *
 * Purpose: To notify all the Callbacks associated with the Event Time Sync.
 *
 * Parms:   QmiCbkLocEventTimeSyncInd  [IN] - pointer to structure used to store all Event Time Sync.
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
local void qaQmiLocEventTimeSyncNotify(
        QmiCbkLocEventTimeSyncInd *pResp )
{
    void  *pCallback;
    enum  eQMICallbackIndex CbkIndex;

    CbkIndex  = eQMI_CB_LOC_EVENT_TIME_SYNC_IND;
    pCallback = qaQmiGetCallback( CbkIndex );
    if ( pCallback )
    {
        /* Invoke the callback */
        ((tFNSetEventTimeSync)pCallback) ( pResp );
    }
}

/*************
 *
 * Name:    qaQmiLocInjectTimeNotify
 *
 * Purpose: To notify all the Callbacks associated with the Event Time Sync.
 *
 * Parms:   QmiCbkLocInjectTimeInd  [IN] - pointer to structure used to store all Inject Time Sync Data.
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
local void qaQmiLocInjectTimeNotify(
        QmiCbkLocInjectTimeInd *pResp )
{
    void  *pCallback;
    enum  eQMICallbackIndex CbkIndex;

    CbkIndex  = eQMI_CB_LOC_INJECT_TIME_IND;
    pCallback = qaQmiGetCallback( CbkIndex );
    if ( pCallback )
    {
        /* Invoke the callback */
        ((tFNInjectTimeStatus)pCallback) ( pResp );
    }
}
/*************
 *
 * Name:    qaQmiLocSensorStreamNotify
 *
 * Purpose: To notify all the Callbacks associated with the Sensor Streaming Ready Status.
 *
 * Parms:   QmiCbkLocSensorStreamingInd  [IN] - pointer to structure used to store all Sensor Streaming Ready Status
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
local void qaQmiLocSensorStreamNotify(
        QmiCbkLocSensorStreamingInd *pResp )
{
    void  *pCallback;
    enum  eQMICallbackIndex CbkIndex;

    CbkIndex  = eQMI_CB_LOC_SENSOR_STREAMING_IND;
    pCallback = qaQmiGetCallback( CbkIndex );
    if ( pCallback )
    {
        /* Invoke the callback */
        ((tFNSensorStreaming)pCallback) ( pResp );
    }
}
/*************
 *
 * Name:    qaQmiLocSensorDataNotify
 *
 * Purpose: To notify all the Callbacks associated with the Inject Sensor Data.
 *
 * Parms:   QmiCbkLocInjectSensorDataInd  [IN] - pointer to structure used to store all Sensor Streaming Ready Status
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
local void qaQmiLocSensorDataNotify(
        QmiCbkLocInjectSensorDataInd *pResp )
{
    void  *pCallback;
    enum  eQMICallbackIndex CbkIndex;

    CbkIndex  = eQMI_CB_LOC_SENSOR_DATA_IND;
    pCallback = qaQmiGetCallback( CbkIndex );
    if ( pCallback )
    {
        /* Invoke the callback */
        ((tFNInjectSensorData)pCallback) ( pResp );
    }
}
/*************
 *
 * Name:    qaQmiLocPositionRptNotify
 *
 * Purpose: To notify all the Callbacks associated with the Event Position Report.
 *
 * Parms:   QmiCbkLocPositionReportInd  [IN] - pointer to structure used to store all Event Position Report Data.
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
local void qaQmiLocPositionRptNotify(
        QmiCbkLocPositionReportInd *pResp )
{
    void  *pCallback;
    enum  eQMICallbackIndex CbkIndex;

    CbkIndex  = eQMI_CB_LOC_POSITION_REPORT_IND;
    pCallback = qaQmiGetCallback( CbkIndex );
    if ( pCallback )
    {
        /* Invoke the callback */
        ((tFNEventPosition)pCallback) ( pResp );
    }
}

local void qaQmiLocEventGnssSvInfoNotify(
        gnssSvInfoNotification *pResp )
{
    void  *pCallback;
    enum  eQMICallbackIndex CbkIndex;

    CbkIndex  = eQMI_CB_LOC_GNSS_SV_INFO_IND;
    pCallback = qaQmiGetCallback( CbkIndex );
    if ( pCallback )
    {
        /* Invoke the callback */
        ((tFNGnssSvInfo)pCallback) ( pResp );
    }
}

local void qaQmiLocEventOpModeNotify(
        ULONG mode )
{
    void  *pCallback;
    enum  eQMICallbackIndex CbkIndex;

    CbkIndex  = eQMI_CB_LOC_OP_MODE_IND;
    pCallback = qaQmiGetCallback( CbkIndex );
    if ( pCallback )
    {
        /* Invoke the callback */
        ((tFNOpMode)pCallback) ( mode );
    }
}

local void qaQmiLocAssistDataNotify(
        delAssistDataStatus *pStatus )
{
    void  *pCallback;
    enum  eQMICallbackIndex CbkIndex;

    CbkIndex  = eQMI_CB_LOC_DELETE_ASSIST_DATA_IND;
    pCallback = qaQmiGetCallback( CbkIndex );
    if ( pCallback )
    {
        /* Invoke the callback */
        ((tFNDelAssistData)pCallback) ( pStatus );
    }
}

local void qaQmiLocInjectUTCTimeNotify(
        QmiCbkLocInjectUTCTimeInd *pInjectUTCTimeNotification )
{
    void  *pCallback;
    enum  eQMICallbackIndex CbkIndex;

    CbkIndex  = eQMI_CB_LOC_INJECT_UTC_TIME_IND;
    pCallback = qaQmiGetCallback( CbkIndex );
    if ( pCallback )
    {
        /* Invoke the callback */
        ((tFNInjectUTCTime)pCallback) ( pInjectUTCTimeNotification );
    }
}

local void qaQmiLocGetServerNotify(
        QmiCbkLocGetServerInd *pGetServerNotification )
{
    void  *pCallback;
    enum  eQMICallbackIndex CbkIndex;

    CbkIndex  = eQMI_CB_LOC_GET_SERVER_IND;
    pCallback = qaQmiGetCallback( CbkIndex );
    if ( pCallback )
    {
        /* Invoke the callback */
        ((tFNGetServer)pCallback) ( pGetServerNotification );
    }
}

local void qaQmiLocSetServerNotify(
        QmiCbkLocSetServerInd *pSetServerNotification )
{
    void  *pCallback;
    enum  eQMICallbackIndex CbkIndex;

    CbkIndex  = eQMI_CB_LOC_SET_SERVER_IND;
    pCallback = qaQmiGetCallback( CbkIndex );
    if ( pCallback )
    {
        /* Invoke the callback */
        ((tFNSetServer)pCallback) ( pSetServerNotification );
    }
}

local void qaQmiLocGetOpModeNotify(
        QmiCbkLocGetOpModeInd *pGetOpModeNotification )
{
    void  *pCallback;
    enum  eQMICallbackIndex CbkIndex;

    CbkIndex  = eQMI_CB_LOC_GET_OP_MODE_IND;
    pCallback = qaQmiGetCallback( CbkIndex );
    if ( pCallback )
    {
        /* Invoke the callback */
        ((tFNGetOpMode)pCallback) ( pGetOpModeNotification );
    }
}

local void qaQmiLocGetFixCriteriaNotify(
        QmiCbkLocGetFixCriteriaInd *pGetFixCriteriaNotification )
{
    void  *pCallback;
    enum  eQMICallbackIndex CbkIndex;

    CbkIndex  = eQMI_CB_LOC_GET_FIX_CRITERIA_IND;
    pCallback = qaQmiGetCallback( CbkIndex );
    if ( pCallback )
    {
        /* Invoke the callback */
        ((tFNGetFixCriteria)pCallback) ( pGetFixCriteriaNotification );
    }
}
/*************
 *
 * Name:    UpkQmiCbkLOCNotification
 *
 * Purpose: Unpacks the recevied LOC indication and invokes the approriate
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
package enum eQCWWANError UpkQmiCbkLOCNotification(
    USHORT                 QmiMsgID,
    BYTE                   *pMdmResp,
    struct QmiNotification *pNotifResp )
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    switch(QmiMsgID)
    {
        case eQMI_LOC_SET_CRADLE_MOUNT_CONFIG_IND:
        {
            QmiCbkLocCradleMountInd *pResp =
                &pNotifResp->QmiInd.qaQmiCbkLocCradleMountInd;

            /* Set all the TLVs to be in Not-Present State */
            memset( pResp, 0, sizeof( QmiCbkLocCradleMountInd ));

            /* Unpack the System Information Indication */
            eRCode = UpkQmiCbkLocCradleMountInd( pMdmResp,
                                             pResp );

            /* Notify to the Callbacks associated */
            qaQmiLocCradleMountNotify( pResp );
            break;
        }
        case eQMI_LOC_EVENT_TIME_SYNC_REQ_IND:
        {
            QmiCbkLocEventTimeSyncInd *pResp =
                &pNotifResp->QmiInd.qaQmiCbkLocEventTimeSyncInd;

            /* Set all the TLVs to be in Not-Present State */
            memset( pResp, 0, sizeof( QmiCbkLocEventTimeSyncInd ));

            /* Unpack the System Information Indication */
            eRCode = UpkQmiCbkLocEventTimeSyncInd( pMdmResp,
                                             pResp );

            /* Notify to the Callbacks associated */
            qaQmiLocEventTimeSyncNotify( pResp );
            break;
        }
        case eQMI_LOC_INJECT_TIME_SYNC_DATA_IND:
        {
            QmiCbkLocInjectTimeInd *pResp =
                &pNotifResp->QmiInd.qaQmiCbkLocInjectTimeInd;

            /* Set all the TLVs to be in Not-Present State */
            memset( pResp, 0, sizeof( QmiCbkLocInjectTimeInd ));

            /* Unpack the System Information Indication */
            eRCode = UpkQmiCbkLocInjectTimeInd( pMdmResp,
                                             pResp );

            /* Notify to the Callbacks associated */
            qaQmiLocInjectTimeNotify( pResp );
            break;
        }
        case eQMI_LOC_SENSOR_STREAMING_STATUS_IND:
        {
              QmiCbkLocSensorStreamingInd *pResp =
                &pNotifResp->QmiInd.qaQmiCbkLocSensorStreamingInd;

            /* Set all the TLVs to be in Not-Present State */
            memset( pResp, 0, sizeof( QmiCbkLocSensorStreamingInd ));

            /* Unpack the System Information Indication */
            eRCode = UpkQmiCbkLocSensorStreamInd( pMdmResp,
                                             pResp );

            /* Notify to the Callbacks associated */
            qaQmiLocSensorStreamNotify( pResp );
            break;
        }
        case eQMI_LOC_INJECT_SENSOR_DATA_IND:
        {
            QmiCbkLocInjectSensorDataInd *pResp =
                &pNotifResp->QmiInd.qaQmiCbkLocInjectSensorDataInd;

            /* Set all the TLVs to be in Not-Present State */
            memset( pResp, 0, sizeof( QmiCbkLocInjectSensorDataInd ));

            /* Unpack the System Information Indication */
            eRCode = UpkQmiCbkLocSensorDataInd( pMdmResp,
                                             pResp );

            /* Notify to the Callbacks associated */
            qaQmiLocSensorDataNotify( pResp );
            break;
        }
        case eQMI_LOC_EVENT_POSITION_REPORT_IND:
        {
            QmiCbkLocPositionReportInd *pResp =
                &pNotifResp->QmiInd.qaQmiCbkLocPositionReportInd;

            /* Set all the TLVs to be in Not-Present State */
            memset( pResp, 0, sizeof( QmiCbkLocPositionReportInd ));

            /* Unpack the System Information Indication */
            eRCode = UpkQmiCbkLocPositionRptInd( pMdmResp,
                                             pResp );

            /* Notify to the Callbacks associated */
            qaQmiLocPositionRptNotify( pResp );
            break;
        }
        case eQMI_LOC_EVENT_GNSS_SV_INFO_IND:
        {
            gnssSvInfoNotification *pResp =
                &pNotifResp->QmiInd.qaQmiCbkLocGnssSvInfoInd;

            /* Set all the TLVs to be in Not-Present State */
            memset( pResp, 0, sizeof( gnssSvInfoNotification ));

            /* Unpack the System Information Indication */
            eRCode = UpkQmiCbkLocEventGnssSvInfoInd( pMdmResp,
                                             pResp );

            /* Notify to the Callbacks associated */
            qaQmiLocEventGnssSvInfoNotify( pResp );
            break;
        }

        case eQMI_LOC_SET_OPERATION_MODE:
        {
            ULONG mode;
            /* Unpack the System Information Indication */
            eRCode = UpkQmiCbkLocOpModeInd( pMdmResp,
                                             &mode );

            /* Notify to the Callbacks associated */
            qaQmiLocEventOpModeNotify( mode );
            break;
        }

        case eQMI_LOC_DELETE_ASSIST_DATA:
        {
            delAssistDataStatus status;
            /* Unpack the System Information Indication */
            eRCode = UpkQmiCbkLocAssistDataInd( pMdmResp,
                                             &status );

            /* Notify to the Callbacks associated */
            qaQmiLocAssistDataNotify( &status );
            break;
        }

        case eQMI_LOC_INJECT_UTC_TIME:
        {
            QmiCbkLocInjectUTCTimeInd *pResp =
                &pNotifResp->QmiInd.qaQmiCbkLocInjectUTCTimeInd;
                
            /* Set all the TLVs to be in Not-Present State */
            memset( pResp, 0, sizeof( QmiCbkLocInjectUTCTimeInd ));  
            
            /* Unpack the Inject UTC time Indication */
            eRCode = UpkQmiCbkLocInjectUTCTimeInd( pMdmResp,
                                            pResp );

            /* Notify to the Callbacks associated */
            qaQmiLocInjectUTCTimeNotify( pResp );
            break;
        }
        
        case eQMI_LOC_SET_SERVER:
        {
            QmiCbkLocSetServerInd *pResp =
                &pNotifResp->QmiInd.qaQmiCbkLocSetServerInd;
                
            /* Set all the TLVs to be in Not-Present State */
            memset( pResp, 0, sizeof( QmiCbkLocSetServerInd ));  
            
            /* Unpack the Set Server Indication */
            eRCode = UpkQmiCbkLocSetServerInd( pMdmResp,
                                               pResp );

            /* Notify to the Callbacks associated */
            qaQmiLocSetServerNotify( pResp );
            break;
        }

        case eQMI_LOC_GET_SERVER:
        {
            QmiCbkLocGetServerInd *pResp =
                &pNotifResp->QmiInd.qaQmiCbkLocGetServerInd;
                
            /* Set all the TLVs to be in Not-Present State */
            memset( pResp, 0, sizeof( QmiCbkLocGetServerInd ));  
            
            /* Unpack the System Information Indication */
            eRCode = UpkQmiCbkLocGetServerInd( pMdmResp,
                                               pResp );

            /* Notify to the Callbacks associated */
            qaQmiLocGetServerNotify( pResp );
            break;
        }        
        
        case eQMI_LOC_GET_OPERATION_MODE:
        {
            QmiCbkLocGetOpModeInd *pResp =
                &pNotifResp->QmiInd.qaQmiCbkLocGetOpModeInd;
                
            /* Set all the TLVs to be in Not-Present State */
            memset( pResp, 0, sizeof( QmiCbkLocGetOpModeInd ));  
            
            /* Unpack the System Information Indication */
            eRCode = UpkQmiCbkLocGetOpModeInd( pMdmResp,
                                               pResp );

            /* Notify to the Callbacks associated */
            qaQmiLocGetOpModeNotify( pResp );
            break;
        } 
        
        case eQMI_LOC_GET_FIX_CRITERIA:
        {
            QmiCbkLocGetFixCriteriaInd *pResp =
                &pNotifResp->QmiInd.qaQmiCbkLocGetFixCriteriaInd;
                
            /* Set all the TLVs to be in Not-Present State */
            memset( pResp, 0, sizeof( QmiCbkLocGetFixCriteriaInd ));  
            
            /* Unpack the System Information Indication */
            eRCode = UpkQmiCbkLocGetFixCriteriaInd( pMdmResp,
                                                    pResp );

            /* Notify to the Callbacks associated */
            qaQmiLocGetFixCriteriaNotify( pResp );
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

