/**
 *
 * $Id: SwiIntDecodeNotifyCdma.c,v 1.2 2010/06/10 00:22:02 epasheva Exp $
 *
 * Filename: SwiIntDecodeNotifyCdma.c
 *
 * Purpose:  Contains functions to handle notification decode for CDMA
 *
 * Copyright: © 2008 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "am/amudefs.h"
#include "er/erudefs.h"
#include "cn/cnudefs.h"
#include "ap/apidefs.h"
#include "SwiIntDecodeNotify.h"
#include "SwiIntUpkCdmaNetwork.h"
#include "SwiIntUpkCdmaRuim.h"
#include "SwiIntUpkCmDm.h"
#include "SwiIntUpkCdmaInternal.h"
#include "SwiIntUpkCdmaSms.h"
#include "SwiIntUpkCdmaBasic.h"

/**
 *
 * Name:    swirssidecode
 *
 * Purpose: Decodes CDMA rssi notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles rssi notification and stores these 
 *          in the supplied notification structure
 *
 */
package SWI_RCODE swirssidecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    struct SWI_STRUCT_Rssi rssivalues;           /* unpacking storage */
    struct SWI_STRUCT_Rssi *rssip = &rssivalues; /* pointer to 
                                                  * unpacking storage 
                                                  */
    /* Unpack CDMA data */
    SwiUnpackRssiCdma( paramfldp, &rssip->iRssi );

    /* Set notification result structure accordingly */
    notifyvariantp->_Rssi.iRssi = rssip->iRssi;
    notifyvariantp->_Rssi.iBer  = (swi_int32)NULL;

    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swichannelnumberdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swichannelnumberdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_ChannelNumber.sizeStruct = 
            sizeof(SWI_STRUCT_ChannelNumber);
    
    notifyvariantp->_ChannelNumber.nChannelNumber = piget16(&paramfldp);
    
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swichannelstatedecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swichannelstatedecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_ChannelState.sizeStruct = sizeof(SWI_STRUCT_ChannelState);
    notifyvariantp->_ChannelState.eChannelState = piget16(&paramfldp);
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swiserviceindicationdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swiserviceindicationdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_ServiceIndication.sizeStruct = 
            sizeof(SWI_STRUCT_ServiceIndication);
    
    notifyvariantp->_ServiceIndication.eServiceIndicator = piget16(&paramfldp);

    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swiroamingstatusdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swiroamingstatusdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_RoamingStatus.sizeStruct = 
            sizeof(SWI_STRUCT_RoamingStatus);
    
    notifyvariantp->_RoamingStatus.eRoamingStatus = piget16(&paramfldp);
    
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swiactivationstatusdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swiactivationstatusdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_ActivationStatus.sizeStruct = 
            sizeof(SWI_STRUCT_ActivationStatus);

    notifyvariantp->_ActivationStatus.eActivationStatus = piget16(&paramfldp);
    
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swieciodecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swieciodecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_Ecio.sizeStruct = sizeof(SWI_STRUCT_Ecio);
    notifyvariantp->_Ecio.nEcio = piget16(&paramfldp);
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swiprevdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swiprevdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_Prev.sizeStruct = sizeof(SWI_STRUCT_Prev);
    notifyvariantp->_Prev.eValue = piget16(&paramfldp);
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swiv42bisenableddecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swiv42bisenableddecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_V42BisEnabled.sizeStruct = 
            sizeof(SWI_STRUCT_V42BisEnabled);
    
    notifyvariantp->_V42BisEnabled.eV42Bis = piget16(&paramfldp);
    
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swiauthenticationstatusdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swiauthenticationstatusdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_AuthenticationStatus.sizeStruct = 
            sizeof(SWI_STRUCT_AuthenticationStatus);
    
    notifyvariantp->_AuthenticationStatus.eAuthenticationStatus = 
            piget16(&paramfldp);
    
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swiactivenamdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swiactivenamdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_ActiveNam.sizeStruct = sizeof(SWI_STRUCT_ActiveNam);
    notifyvariantp->_ActiveNam.eNamIndex = piget16(&paramfldp);
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swisystemtimedecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swisystemtimedecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_SystemTime.sizeStruct = sizeof(SWI_STRUCT_SystemTime);

    notifyvariantp->_SystemTime.time.wYear = piget16(&paramfldp);
    notifyvariantp->_SystemTime.time.wMonth = piget16(&paramfldp);
    notifyvariantp->_SystemTime.time.wDay = piget16(&paramfldp);
    notifyvariantp->_SystemTime.time.wDayOfWeek = piget16(&paramfldp);
    notifyvariantp->_SystemTime.time.wHour = piget16(&paramfldp);
    notifyvariantp->_SystemTime.time.wMinute = piget16(&paramfldp);
    notifyvariantp->_SystemTime.time.wSecond = piget16(&paramfldp);
    notifyvariantp->_SystemTime.time.wMilliseconds = 0;
    
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swie911modedecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swie911modedecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_E911Mode.sizeStruct = sizeof(SWI_STRUCT_E911Mode);
    notifyvariantp->_E911Mode.eMode = piget16(&paramfldp);
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swipowersaveenterdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swipowersaveenterdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_PowerSaveEnter.sizeStruct = 
            sizeof(struct SWI_STRUCT_PowerSaveEnter);
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swipowersaveexitdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swipowersaveexitdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_PowerSaveExit.sizeStruct = 
            sizeof(struct SWI_STRUCT_PowerSaveExit);
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swiexcessspcfailuredecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swiexcessspcfailuredecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_ExcessSPCFailures.sizeStruct = 
            sizeof(struct SWI_STRUCT_ExcessSPCFailures);
    
    notifyvariantp->_ExcessSPCFailures.eFailType = piget16(&paramfldp);
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swiruimstatusdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swiruimstatusdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_RuimStatus.sizeStruct = sizeof(SWI_STRUCT_RuimStatus);
    SwiUnpackRUIMStatus( paramfldp, &notifyvariantp->_RuimStatus );
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swirssi_eciodecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swirssi_eciodecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_Rssi_Ecio.sizeStruct = sizeof(SWI_STRUCT_Rssi_Ecio);
    SwiUnpackRssiEcio( paramfldp, &notifyvariantp->_Rssi_Ecio );
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swihdrnotificationstatusdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swihdrnotificationstatusdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_HdrNotificationStatus.sizeStruct = 
            sizeof(SWI_STRUCT_HdrNotificationStatus);
    
    notifyvariantp->_HdrNotificationStatus.iHdrNotificationStatus = 
            piget16(&paramfldp);
    
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swihdr_rssidecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swihdr_rssidecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_Hdr_Rssi.sizeStruct = sizeof(SWI_STRUCT_Hdr_Rssi);
    notifyvariantp->_Hdr_Rssi.iRssi = piget16(&paramfldp);
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swihdr_roam_statusdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swihdr_roam_statusdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_Hdr_Roam_Status.sizeStruct = 
            sizeof(SWI_STRUCT_Hdr_Roam_Status);
    notifyvariantp->_Hdr_Roam_Status.iHdrRoamStatus = piget16(&paramfldp);
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swihdr_srv_statedecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swihdr_srv_statedecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_Hdr_Srv_State.sizeStruct = 
            sizeof(SWI_STRUCT_Hdr_Srv_State);
    notifyvariantp->_Hdr_Srv_State.iHdrSrvState = piget16(&paramfldp);
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swihdr_hybr_prefdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swihdr_hybr_prefdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_Hdr_Hybr_Pref.sizeStruct = 
            sizeof(SWI_STRUCT_Hdr_Hybr_Pref);
    notifyvariantp->_Hdr_Hybr_Pref.iHdrHybrPref = piget16(&paramfldp);
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swiidle_digital_modedecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swiidle_digital_modedecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_Idle_Digital_Mode.sizeStruct = 
            sizeof(SWI_STRUCT_Idle_Digital_Mode);
    
    notifyvariantp->_Idle_Digital_Mode.iPrevIdleDigitalMode = 
            piget16(&paramfldp);
    
    notifyvariantp->_Idle_Digital_Mode.iCurrIdleDigitalMode = 
            piget16(&paramfldp);
    
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swiddtm_prefdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swiddtm_prefdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_DDTM_Pref.sizeStruct = sizeof(SWI_STRUCT_DDTM_Pref);
    notifyvariantp->_DDTM_Pref.iDDTMPref = piget16(&paramfldp);
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swihdr_powersave_enterdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swihdr_powersave_enterdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_Hdr_PowerSave_Enter.sizeStruct = 
            sizeof(SWI_STRUCT_Hdr_PowerSave_Enter);
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swihdr_powersave_exitdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swihdr_powersave_exitdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_Hdr_PowerSave_Exit.sizeStruct = 
            sizeof(SWI_STRUCT_Hdr_PowerSave_Exit);
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swihdr_acqdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swihdr_acqdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_Hdr_Acq.sizeStruct = sizeof(SWI_STRUCT_Hdr_Acq);
    notifyvariantp->_Hdr_Acq.lAcquired = *paramfldp;
    return SWI_RCODE_OK;
}


/*************
 *
 * Name:    swiomadmconfigdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swiomadmconfigdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_OMADM_GetConfig.sizeStruct = 
            sizeof(SWI_STRUCT_OMADM_GET_CONFIG);

    SwiUnpackOMADMConfig( paramfldp, &notifyvariantp->_OMADM_GetConfig );
    
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swiomadmstatedecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swiomadmstatedecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_OMADM_SessionState.sizeStruct = 
            sizeof(SWI_STRUCT_OMADM_SESSION_STATE);

    SwiUnpackOMADMSessionState(paramfldp, 
            &notifyvariantp->_OMADM_SessionState);
    
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swiomadmnialertdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swiomadmnialertdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_OMADM_NIAlert.sizeStruct = 
            sizeof(SWI_STRUCT_OMADM_NI_ALERT);

    SwiUnpackOMADMNIAlert( paramfldp, &notifyvariantp->_OMADM_NIAlert );
    
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swiomadmuialertdisplaydecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swiomadmuialertdisplaydecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    return SWI_RCODE_NOT_SUP;
}

/*************
 *
 * Name:    swiomadmuialertconfirmdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swiomadmuialertconfirmdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    return SWI_RCODE_NOT_SUP;
}

/*************
 *
 * Name:    swiomadmuialertinputdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swiomadmuialertinputdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    return SWI_RCODE_NOT_SUP;
}

/*************
 *
 * Name:    swiomadmuialertchoicedecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swiomadmuialertchoicedecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    return SWI_RCODE_NOT_SUP;
}

/*************
 *
 * Name:    swidmdlprogressdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swidmdlprogressdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_DMDL_Progress.sizeStruct = 
            sizeof(SWI_STRUCT_DM_DL_PROGRESS);
    
    SwiUnpackDMDLProgress( paramfldp, &notifyvariantp->_DMDL_Progress );
    
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swidmdlalertdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swidmdlalertdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_DMDL_Alert.sizeStruct = sizeof(SWI_STRUCT_DM_DL_ALERT);
    return SwiUnpackDMDLAllert(paramfldp, &notifyvariantp->_DMDL_Alert);
}

/*************
 *
 * Name:    swidmdlinstallstatusdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swidmdlinstallstatusdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_DMDL_InstallStatus.sizeStruct = 
            sizeof(SWI_STRUCT_DM_DL_INSTALL_STATUS);

    SwiUnpackDMDLInstallStatus(paramfldp, 
            &notifyvariantp->_DMDL_InstallStatus);
    
    return SWI_RCODE_OK;
}


/*************
 *
 * Name:    swismsstatusdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swismsstatusdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_CdmaSmsStatus.sizeStruct = 
            sizeof(SWI_STRUCT_CDMA_SMS_Status);
    
    SwiUnpackCdmaSMSMessageStatus(paramfldp, &notifyvariantp->_CdmaSmsStatus);
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swismssendstatusdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swismssendstatusdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_CdmaSmsSendStatus.sizeStruct = 
            sizeof(SWI_STRUCT_CDMA_SMS_SendStatus);
    
    notifyvariantp->_CdmaSmsSendStatus.nMsgID = piget16(&paramfldp);
    notifyvariantp->_CdmaSmsSendStatus.eErrorClass = piget16(&paramfldp);
    notifyvariantp->_CdmaSmsSendStatus.eCauseCode = piget16(&paramfldp);
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swiiota_statusdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swiiota_statusdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_EMIOTAStauts.sizeStruct = sizeof(SWI_STRUCT_IOTA_State);
    
    notifyvariantp->_EMIOTAStauts.iEvent = piget16(&paramfldp);
    notifyvariantp->_EMIOTAStauts.iEventStatus = piget16(&paramfldp);
    notifyvariantp->_EMIOTAStauts.iSessionEndFailureReason = 
            piget16(&paramfldp);
    notifyvariantp->_EMIOTAStauts.iSessionEndErrorCode = piget16(&paramfldp);
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swiiota_wappushdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swiiota_wappushdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_EMIOTAWAPPush.sizeStruct = sizeof(SWI_STRUCT_IOTA_WAPPush);
    pigetmulti(&paramfldp, (swi_uint8 *)notifyvariantp->_EMIOTAWAPPush.szWAPUri, 
            LENGTH_WAPUri);
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swicallnotificationstatusdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swicallnotificationstatusdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_CallNotificationStatus.sizeStruct = 
            sizeof(SWI_STRUCT_CallNotificationStatus);
    
    notifyvariantp->_CallNotificationStatus.nNotifyStatus = 
            piget16(&paramfldp);
    
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swicallbytecounterdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swicallbytecounterdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_CallByteCounter.sizeStruct = 
            sizeof(SWI_STRUCT_CallByteCounter);
    
    notifyvariantp->_CallByteCounter.nTxBytes = piget32(&paramfldp);
    notifyvariantp->_CallByteCounter.nRxBytes = piget32(&paramfldp);

    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swicallincomingdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swicallincomingdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_CallIncoming.sizeStruct = sizeof(SWI_STRUCT_CallIncoming);
    notifyvariantp->_CallIncoming.eType = piget16(&paramfldp);
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swicallcalleriddecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swicallcalleriddecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_CallCallerID.sizeStruct = sizeof(SWI_STRUCT_CallCallerID);
    
    pigetmulti(&paramfldp, (swi_uint8 *)notifyvariantp->_CallCallerID.szNum, 
            LENGTH_CallNumber);
    notifyvariantp->_CallCallerID.szNum[LENGTH_CallNumber] = EOS;
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swicallextendedcalleriddecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swicallextendedcalleriddecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_CallExtendedCallerID.sizeStruct = 
            sizeof(SWI_STRUCT_CallExtendedCallerID);
    
    pigetmulti(&paramfldp, (swi_uint8 *)notifyvariantp->_CallExtendedCallerID.szNum, 
            LENGTH_CallExtendedNumber);
    
    notifyvariantp->_CallExtendedCallerID.szNum[LENGTH_CallExtendedNumber] = 
            EOS;
    
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swicallconnecteddecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swicallconnecteddecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_CallConnected.sizeStruct = 
            sizeof(SWI_STRUCT_CallConnected);
    
    SwiUnpackCallConnectionStateEx(paramfldp, &notifyvariantp->_CallConnected);
    
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swicalldisconnecteddecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swicalldisconnecteddecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_CallDisconnected.sizeStruct = 
            sizeof(SWI_STRUCT_CallDisconnected);
    
    notifyvariantp->_CallDisconnected.eState = piget16(&paramfldp);
    notifyvariantp->_CallDisconnected.eReason = piget16(&paramfldp);
    
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swicallerrordecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swicallerrordecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_CallError.sizeStruct = sizeof(SWI_STRUCT_CallError);
    notifyvariantp->_CallError.eCmd = piget16(&paramfldp);
    notifyvariantp->_CallError.eError = piget16(&paramfldp);
    notifyvariantp->_CallError.eSource = piget16(&paramfldp);

    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swicallconnectingdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swicallconnectingdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_CallConnecting.sizeStruct = 
            sizeof(SWI_STRUCT_CallConnecting);

    notifyvariantp->_CallConnecting.eType = piget16(&paramfldp);
    notifyvariantp->_CallConnecting.eSource = piget16(&paramfldp);

    pigetmulti(&paramfldp, (swi_uint8 *)notifyvariantp->_CallConnecting.szNum, 
            LENGTH_CallNumber);
    notifyvariantp->_CallConnecting.szNum[LENGTH_CallNumber] = EOS;

    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swicalldormantdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swicalldormantdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_CallDormant.sizeStruct = sizeof(SWI_STRUCT_CallDormant);
    notifyvariantp->_CallDormant.eState = piget16(&paramfldp);
    notifyvariantp->_CallDormant.eReason = piget16(&paramfldp);
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swicallprivacydecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swicallprivacydecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_CallPrivacy.sizeStruct = sizeof(SWI_STRUCT_CallPrivacy);
    notifyvariantp->_CallPrivacy.eMode = piget16(&paramfldp);
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swiotaspstatedecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swiotaspstatedecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_OTASPState.sizeStruct = sizeof(SWI_STRUCT_OTASPState);
    notifyvariantp->_OTASPState.eState = piget16(&paramfldp);
    notifyvariantp->_OTASPState.nSuccess = piget16(&paramfldp);
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    switechsiddecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE switechsiddecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_TechSid.sizeStruct = sizeof(SWI_STRUCT_TechSid);
    notifyvariantp->_TechSid.nSid = piget16(&paramfldp);
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    switechniddecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE switechniddecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_TechNid.sizeStruct = sizeof(SWI_STRUCT_TechNid);
    notifyvariantp->_TechNid.nNid = piget16(&paramfldp);
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    switechbsinfodecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE switechbsinfodecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_TechBsInfo.sizeStruct = sizeof(SWI_STRUCT_TechBsInfo);
    notifyvariantp->_TechBsInfo.nLongitude = piget32(&paramfldp);
    notifyvariantp->_TechBsInfo.nLatitude = piget32(&paramfldp);
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    switechbandclassdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE switechbandclassdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_TechBandClass.sizeStruct = 
            sizeof(SWI_STRUCT_TechBandClass);
    notifyvariantp->_TechBandClass.eBandClass = piget16(&paramfldp);
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    switechregzonedecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE switechregzonedecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_TechRegZone.sizeStruct = sizeof(SWI_STRUCT_TechRegZone);
    notifyvariantp->_TechRegZone.nZoneID = piget16(&paramfldp);
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    switechpacketzonedecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE switechpacketzonedecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_TechPacketZone.sizeStruct = 
            sizeof(SWI_STRUCT_TechPacketZone);
    
    notifyvariantp->_TechPacketZone.nZoneID = piget16(&paramfldp);
    
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    switechpcsblockdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE switechpcsblockdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_TechPcsBlock.sizeStruct = sizeof(SWI_STRUCT_TechPcsBlock);
    notifyvariantp->_TechPcsBlock.ePcs = piget16(&paramfldp);
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    switechcellchanneldecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE switechcellchanneldecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_TechCellChannel.sizeStruct = 
            sizeof(SWI_STRUCT_TechCellChannel);
    
    notifyvariantp->_TechCellChannel.eChannel = piget16(&paramfldp);
    
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swiflashburndecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swiflashburndecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_FlashBurn.sizeStruct = 
            sizeof(struct SWI_STRUCT_FlashBurn);
    
    notifyvariantp->_FlashBurn.nSector = piget16(&paramfldp);
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    switechhdrnotificationstatusdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE switechhdrnotificationstatusdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_TechHdrNotificationStatus.sizeStruct = 
            sizeof(SWI_STRUCT_TechHdrNotificationStatus);
    
    notifyvariantp->_TechHdrNotificationStatus.iTechHdrNotificationStatus = 
            piget16(&paramfldp);
    
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    switech_hdr_channeldecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE switechhdrchanneldecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_Tech_Hdr_Channel.sizeStruct = 
            sizeof(SWI_STRUCT_Tech_Hdr_Channel);
    
    notifyvariantp->_Tech_Hdr_Channel.iHdrChannel = piget16(&paramfldp);
    
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    switech_hdr_an_authdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE switechhdranauthdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_Tech_Hdr_An_Auth.sizeStruct = 
            sizeof(SWI_STRUCT_Tech_Hdr_An_Auth);
    
    notifyvariantp->_Tech_Hdr_An_Auth.iHdrAnAuthS = piget16(&paramfldp);
    
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    switech_hdr_drc_valdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE switechhdrdrcvaldecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_Tech_Hdr_Drc_Val.sizeStruct = 
            sizeof(SWI_STRUCT_Tech_Hdr_Drc_Val);
    
    notifyvariantp->_Tech_Hdr_Drc_Val.iHdrDrcValue= piget16(&paramfldp);
    
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    switech_hdr_drc_coverdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE switechhdrdrccoverdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_Tech_Hdr_Drc_Cover.sizeStruct = 
            sizeof(SWI_STRUCT_Tech_Hdr_Drc_Cover);
    
    notifyvariantp->_Tech_Hdr_Drc_Cover.iHdrDrcCover = piget16(&paramfldp);
    
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    switech_hdr_drc_coverdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE switechhdrrridecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_Tech_Hdr_Rri.sizeStruct = 
            sizeof(SWI_STRUCT_Tech_Hdr_Rri);
    
    notifyvariantp->_Tech_Hdr_Rri.iHdrRri = piget16(&paramfldp);
    
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    switech_hdr_drc_coverdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE switechhdrstatsdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_Tech_Hdr_Stats.sizeStruct = 
            sizeof(SWI_STRUCT_Tech_Hdr_Stats);
    
    notifyvariantp->_Tech_Hdr_Stats.iHdrRxAgc0 = piget16(&paramfldp);
    notifyvariantp->_Tech_Hdr_Stats.iHdrRxAgc1 = piget16(&paramfldp);
    notifyvariantp->_Tech_Hdr_Stats.iHdrTxTotal = piget16(&paramfldp);
    notifyvariantp->_Tech_Hdr_Stats.iHdrTxAdj = piget16(&paramfldp);
    notifyvariantp->_Tech_Hdr_Stats.iHdrc2i = piget16(&paramfldp);
    notifyvariantp->_Tech_Hdr_Stats.iHdrEcio = piget16(&paramfldp);
    notifyvariantp->_Tech_Hdr_Stats.iHdrRssi = piget16(&paramfldp);
    
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    switech_hdr_su_fwd_statsdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE switechhdrsufwdstatsdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_Tech_Hdr_Su_Fwd_Stats.sizeStruct = 
            sizeof(SWI_STRUCT_Tech_Hdr_Su_Fwd_Stats);

    SwiUnpackTechHdrSuFwdStats(paramfldp, 
            &notifyvariantp->_Tech_Hdr_Su_Fwd_Stats);
    
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    switech_hdr_su_fwd_cntdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE switechhdrsufwdslotcntdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_Tech_Hdr_Su_Fwd_Slot_Cnt.sizeStruct = 
            sizeof(SWI_STRUCT_Tech_Hdr_Su_Fwd_Slot_Cnt);

    SwiUnpackTechHdrSuFwdSlotCnt(paramfldp, 
            &notifyvariantp->_Tech_Hdr_Su_Fwd_Slot_Cnt);

    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    switech_hdr_su_fwd_cnt2decode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE switechhdrsufwdslotcnt2decode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_Tech_Hdr_Su_Fwd_Slot_Cnt2.sizeStruct = 
            sizeof(SWI_STRUCT_Tech_Hdr_Su_Fwd_Slot_Cnt2);

    SwiUnpackTechHdrSuFwdSlotCnt2(paramfldp, 
            &notifyvariantp->_Tech_Hdr_Su_Fwd_Slot_Cnt2);

    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    switech_hdr_mu_fwd_statsdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE switechhdrmufwdstatsdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_Tech_Hdr_Mu_Fwd_Stats.sizeStruct = 
            sizeof(SWI_STRUCT_Tech_Hdr_Mu_Fwd_Stats);
    

    SwiUnpackTechHdrMuFwdStats(paramfldp, 
            &notifyvariantp->_Tech_Hdr_Mu_Fwd_Stats);

    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    switech_hdr_personalitydecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE switechhdrpersonalitydecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_Tech_Hdr_Personality.sizeStruct = 
            sizeof(SWI_STRUCT_Tech_Hdr_Personality);
    
    SwiUnpackTechHdrPersonality( paramfldp, 
            &notifyvariantp->_Tech_Hdr_Personality );
    
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    switech_hdr_mac_indexdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE switechhdrmacindexdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_Tech_Hdr_Mac_Index.sizeStruct = 
            sizeof(SWI_STRUCT_Tech_Hdr_Mac_Index);
    
    SwiUnpackTechHdrMacIndex(paramfldp, &notifyvariantp->_Tech_Hdr_Mac_Index);
    
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    switechhdrprevdecode
 *
 * Purpose: Decodes CDMA Tech HDR protocal revision notification
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE switechhdrprevdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_Tech_Hdr_Prev.sizeStruct = 
            sizeof(SWI_STRUCT_Tech_Hdr_Prev);
    
    notifyvariantp->_Tech_Hdr_Prev.iHdrPRevMin = piget16(&paramfldp);
    notifyvariantp->_Tech_Hdr_Prev.iHdrPRevMax = piget16(&paramfldp);
    
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    switechhdrsectordecode
 *
 * Purpose: Decodes CDMA Tech HDR sector notification
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE switechhdrsectordecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{

    notifyvariantp->_Tech_Hdr_Sector.sizeStruct = 
            sizeof(SWI_STRUCT_Tech_Hdr_Sector);

    pigetmulti(&paramfldp,
               (swi_uint8 *)notifyvariantp->_Tech_Hdr_Sector.szHdrSectorId, 
               LENGTH_SectorId);
    notifyvariantp->_Tech_Hdr_Sector.szHdrSectorId[LENGTH_SectorId] = EOS;

    notifyvariantp->_Tech_Hdr_Sector.iHdrSubnetMask = piget16(&paramfldp);
    notifyvariantp->_Tech_Hdr_Sector.iHdrColorCode = piget16(&paramfldp);
    notifyvariantp->_Tech_Hdr_Sector.iHdrPnOffset = piget16(&paramfldp);

    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    switechhdrstatedecode
 *
 * Purpose: Decodes CDMA Tech HDR state notification
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE switechhdrstatedecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_Tech_Hdr_State.sizeStruct = 
            sizeof(SWI_STRUCT_Tech_Hdr_State);

    notifyvariantp->_Tech_Hdr_State.iHdrAtState = piget16(&paramfldp);
    notifyvariantp->_Tech_Hdr_State.iHdrSessionState = piget16(&paramfldp);

    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    switechhdrsessioninfodecode
 *
 * Purpose: Decodes CDMA Tech HDR info notification
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE switechhdrsessioninfodecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_Tech_Hdr_Session.sizeStruct = 
            sizeof(SWI_STRUCT_Tech_Hdr_Session);

    pigetmulti(&paramfldp,
               (swi_uint8 *)notifyvariantp->_Tech_Hdr_Session.szHdrUati, 
               LENGTH_HdrUati);
    notifyvariantp->_Tech_Hdr_Session.szHdrUati[LENGTH_HdrUati] = EOS;

    notifyvariantp->_Tech_Hdr_Session.iHdrUatiColorCode = piget16(&paramfldp);
    notifyvariantp->_Tech_Hdr_Session.iHdrRati = piget32(&paramfldp);
    notifyvariantp->_Tech_Hdr_Session.iHdrSessionDuration = piget16(&paramfldp);
    notifyvariantp->_Tech_Hdr_Session.iHdrSessionStart1 = piget32(&paramfldp);
    notifyvariantp->_Tech_Hdr_Session.iHdrSessionStart2 = piget32(&paramfldp);
    notifyvariantp->_Tech_Hdr_Session.iHdrSessionEnd1 = piget32(&paramfldp);
    notifyvariantp->_Tech_Hdr_Session.iHdrSessionEnd2 = piget32(&paramfldp);

    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swimodemtoohotindicationdecode
 *
 * Purpose: Decodes
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swimodemtoohotindicationdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_ModemTooHotIndication.sizeStruct = 
            sizeof(SWI_STRUCT_ModemTooHotIndication);
    
    notifyvariantp->_ModemTooHotIndication.eAction = piget16(&paramfldp);
    
    return SWI_RCODE_OK;
}

/*
 * $Log: SwiIntDecodeNotifyCdma.c,v $
 */
