/**
 *
 * $Id: SwiIntDecodeNotifyGsm.c,v 1.11 2010/06/10 00:23:06 epasheva Exp $
 *
 * Filename: SwiIntDecodeNotifyGsm.c
 *
 * Purpose:  Contains functions to handle notification decode for GSM
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
#include "SwiApiGsmVoice.h"
#include "aptestproto.h"

/**
 *
 * Name:    swiradiodecode
 *
 * Purpose: Decodes GSM radio notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles rssi notification and stores these 
 *          in the supplied notification structure
 *
 */
package SWI_RCODE swiradiodecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    struct SWI_STRUCT_Rssi rssivalues;           /* unpacking storage */
    struct SWI_STRUCT_Rssi *rssip = &rssivalues; /* ptr to unpacking storage */
    /* Unpack GSM data  */
    SwiUpkRssiGsm( paramfldp, rssip );

    /* Set notification result structure accordingly */
    notifyvariantp->_Rssi.iRssi = rssip->iRssi; 
    notifyvariantp->_Rssi.iBer  = rssip->iBer;

    return SWI_RCODE_OK;
}

/**
 *
 * Name:    swibytecntdecode
 *
 * Purpose: Decodes byte counter notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles byte counters notification and stores these 
 *          in the supplied notification structure
 *
 */
package SWI_RCODE swibytecntdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    SwiUpkByteCounters( paramfldp, 
                        &notifyvariantp->_ByteCounter.TransmitBytes, 
                        &notifyvariantp->_ByteCounter.ReceiveBytes);

    return SWI_RCODE_OK;
}
/**
 *
 * Name: swigsmcelldecode
 *
 * Purpose: Decodes GSM Cell Info notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles GSM Cell Info notification and stores these 
 *          in the supplied notification structure
 *
 */
package SWI_RCODE swigsmcelldecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    SWI_RCODE retcode = SWI_RCODE_OK;
    swi_bool version  = TRUE; /* flag indicationg whether the object version 
                               * is supported
                               */

    /* Unpack data */
    SwiUpkGsCellInfo ( paramfldp, &notifyvariantp->_GsmCellInfo, &version ); 
    /* Note: The notification structure is set with the packet data only if
     * the object version is supported ( regardless of packet validity ).
     * It is up to the application to decide how to handle data. 
     */

    if (!version)
    {
        /* unsupported vesrion */
        retcode = SWI_RCODE_FAILED;
    }
    else
    {
        retcode = SWI_RCODE_OK;
    }
    return retcode;
}

/**
 *
 * Name:    swisimstatusdecode
 *
 * Purpose: Decodes SIM Status notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   none
 *
 */
package SWI_RCODE swisimstatusdecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    SwiUpkGsmSimStatus ( paramfldp, &notifyvariantp->_SimStatusExp ); 

    return SWI_RCODE_OK;
}

/**
 *
 * Name:    swinetstatusdecode
 *
 * Purpose: Decodes Network Status notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles Network Status notification data 
 *          and stores these in the supplied notification structure.
 *          SWI_NOTIFY_NetworkStatus provides the application with the
 *          service status information needed to inform the user of 
 *          its current GPRS/EDGE service state.
 *
 *
 */
package SWI_RCODE  swinetstatusdecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    SwiUpkGsNetworkStatus(paramfldp, &notifyvariantp->_NetworkStatus);

    return SWI_RCODE_OK;
}

/**
 *
 * Name:    swigsmsystemstatusdecode
 *
 * Purpose: Decodes Gsm System Status notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles Network Status notification data 
 *          and stores these in the supplied notification structure.
 *          SWI_NOTIFY_GsmSystemtatus provides the application with the
 *          service status information needed to inform the user of 
 *          its current GPRS/EDGE service state.
 *
 *
 */
package SWI_RCODE  swigsmsystemstatusdecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    SwiUpkGsSystemStatus(paramfldp, &notifyvariantp->_GsmSystemStatus);

    return SWI_RCODE_OK;
}

/**
 *
 * Name:    swirscpeciodecode
 *
 * Purpose: Decodes Gsm Received Signal Code Power notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:   pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles RscpEcIo notification data 
 *          and stores these in the supplied notification structure.
 *          SWI_NOTIFY_RscpEcIo provides the application with the
 *          Gsm Received Signal Code Power (RSCP) and the total energy per chip 
 *          per power density (Ec/Io) value of the active set's strongest cells 
 *          to inform the user of its current GPRS/EDGE service state.
 *
 *
 */
package SWI_RCODE  swirscpeciodecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    SwiUpkGsRscpEcIo(paramfldp, &notifyvariantp->_RscpEcIo);

    return SWI_RCODE_OK;
}

/**
 *
 * Name:    swipktsessioncalldecode
 *
 * Purpose: Decodes Packet Session Call notification
 *
 * Parms:
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[IN]:    pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.
 *
 * Abort:   none
 *
 * Notes:   This function handles Packet Session Call notification data
 *          and stores these in the supplied notification structure.
 *          SWI_NOTIFY_PktSessionCall provides the application with the
 *          service status information needed to inform the user of
 *          its current GPRS/EDGE Packet Session state.
 *
 *
 */
package SWI_RCODE  swipktsessioncalldecode(
    swi_uint8 *paramfldp,
    struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    SwiUpkGsPktSessionStatusExp(paramfldp, &notifyvariantp->_PktSessionCall);

    return SWI_RCODE_OK;
}

/**
 *
 * Name:    swiserviceicondecode
 *
 * Purpose: Decodes Service Icon notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles Service Icon notification data 
 *          and stores these in the supplied notification structure.
 *          SWI_NOTIFY_ServiceIcon provides the application with the
 *          service status information needed to inform the user of 
 *          its current GPRS/EDGE service state.
 *
 *
 */
package SWI_RCODE  swiserviceicondecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    SwiUpkGsServiceIcon(paramfldp, &notifyvariantp->_ServiceIcon);

    return SWI_RCODE_OK;
}

/**
 *
 * Name:    swiplmnavaildecode
 *
 * Purpose: Decodes PLMN Available notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles PLMN Available notification data 
 *          and stores these in the supplied notification structure.
 *          SWI_NOTIFY_PlmnAvailable reports the success of the PLMN scan
 *          initiated by the SwiStartPLMNSearch function.  This notification 
 *          will indicate if any networks were found in the scan.  
 *
 */
package SWI_RCODE  swiplmnavaildecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    SwiUpkGsPlmnAvailable(paramfldp, &notifyvariantp->_PlmnAvailable);

    return SWI_RCODE_OK;
}

/**
 *
 * Name:    swiplmnreadydecode
 *
 * Purpose: Decodes PLMN Ready notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles PLMN Ready notification data 
 *          and stores these in the supplied notification structure
 *          This is 0x103C CNS_MANUAL_SELECT_READY notification
 *
 */
package SWI_RCODE  swiplmnreadydecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    SwiUpkGsPlmnReady( paramfldp, &notifyvariantp->_PlmnReady);

    return SWI_RCODE_OK;

}

/**
 *
 * Name:    swiplmnmodedecode
 *
 * Purpose: Decodes PLMN Mode notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles PLMN Mode notification data 
 *          and stores these in the supplied notification structure
 *
 */
package SWI_RCODE  swiplmnmodedecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    SwiUpkGsPlmnMode( paramfldp, &notifyvariantp->_PlmnMode);

    return SWI_RCODE_OK;

}

/**
 *
 * Name:    swiregistrationdecode
 *
 * Purpose: Decodes registration notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles Registration notification data 
 *          and stores these in the supplied notification structure
 *          This is 0x1043 CNS_REGISTRATION_STATE_EX notification.
 *
 */
package SWI_RCODE  swiregistrationdecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    SwiUpkGsRegistration( paramfldp, &notifyvariantp->_Registration);

    return SWI_RCODE_OK;
}

/**
 *
 * Name: swigsmprofilechangedecode
 *
 * Purpose: Decodes GSM Profile Change notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles GSM Profile Change notification data 
 *          and stores these in the supplied notification structure
 *
 */
package SWI_RCODE  swigsmprofilechangedecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    SwiUpkGsProfileChange ( paramfldp, &notifyvariantp->_GsmProfileChange); 

    return SWI_RCODE_OK;

}

/**
 *
 * Name: swigsmdefprofchangedecode
 *
 * Purpose: Decodes GSM Default Profile Change notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles GSM Default Profile Change notification data 
 *          and stores these in the supplied notification structure
 *
 */
package SWI_RCODE  swigsmdefprofchangedecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    notifyvariantp->_GsmDefaultChange.sizeStruct = 
                    sizeof(struct SWI_STRUCT_GsmDefaultChange);
    notifyvariantp->_GsmDefaultChange.nProfileId = *paramfldp++;

    return SWI_RCODE_OK;

}

/**
 *
 * Name: swimodemdisabled
 *
 * Purpose: Decodes GSM Modem Disabled notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles GSM Modem Disabled notification data 
 *          and stores these in the supplied notification structure
 *
 */
package SWI_RCODE  swimodemdisabled( 
                        swi_uint8 *paramfldp, 
                        struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    notifyvariantp->_ModemDisabled.sizeStruct = 
                    sizeof(struct SWI_STRUCT_ModemDisabled);
    notifyvariantp->_ModemDisabled.bModemDisabled = *paramfldp++;

    return SWI_RCODE_OK;

}

/* GSM PHonebook */
/**
 *
 * Name: swigsmphonebookentry
 *
 * Purpose: Decodes phonebook entry info notification
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   none
 *
 */
package SWI_RCODE swiphonebookentrydecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    if (notifyvariantp->_PhonebookEntry.nID !=0)
        SwiUpkGsPhbkGetEntry(paramfldp, &notifyvariantp->_PhonebookEntry );
                         
    return SWI_RCODE_OK;

}
/**
 *
 * Name: swigsmphonebookready
 *
 * Purpose: Decodes phonebook ready info notification
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   none
 *
 */
package SWI_RCODE swiphonebookreadydecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
     /* set structure length */
    notifyvariantp->_PhonebookReady.sizeStruct = 
                    sizeof(struct SWI_STRUCT_PhonebookReady);
                    
    notifyvariantp->_PhonebookReady.bReady = *paramfldp;
                          
    return SWI_RCODE_OK;

}
/**
 *
 * Name: swistknotifcmddecode
 *
 * Purpose: Decodes swistknotifcmddecode notification
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   none
 *
 */
package SWI_RCODE swistkcmdnotifdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    SwiUpkGsStkCmdNotif(paramfldp, &notifyvariantp->_Notif_STKCommand );

    return SWI_RCODE_OK;
}

/* GSM Voice */

/**
 *
 * Name: swimissedcall
 *
 * Purpose: Decodes GSM Missed Call notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:   pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles GSM Missed Call notification data 
 *          and stores these in the supplied notification structure
 *
 */
package SWI_RCODE  swimissedcall( 
                        swi_uint8 *paramfldp, 
                        struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    notifyvariantp->_MissedCall.sizeStruct = 
                    sizeof(struct SWI_STRUCT_MissedCall);
    notifyvariantp->_MissedCall.nCount = *paramfldp;
    return SWI_RCODE_OK;
}

/**
 *
 * Name: swigsmcallprogressdecode
 *
 * Purpose: Decodes GSM Call Exp Progress notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:   pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles GSM Call Exp Progress notification data 
 *          and stores these in the supplied notification structure
 *
 */
package SWI_RCODE  swigsmcallprogressdecode( 
                        swi_uint8 *paramfldp, 
                        struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    notifyvariantp->_CallProgress.sizeStruct = 
                    sizeof(struct SWI_STRUCT_CALL_EXP_Progress);
    SwiUpkGsmCallExpProgress(paramfldp, &notifyvariantp->_CallProgress);

    return SWI_RCODE_OK;

}

/**
 *
 * Name: swigsmcallforwardeddecode
 *
 * Purpose: Decodes GSM Call Forwarded notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:   pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles GSM Call Forwarded notification data 
 *          and stores these in the supplied notification structure
 *
 */
package SWI_RCODE  swigsmcallforwardeddecode( 
                        swi_uint8 *paramfldp, 
                        struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    notifyvariantp->_CallForwarded.sizeStruct = 
                    sizeof(struct SWI_STRUCT_CallForwarded);
    
    SwiUpkGsmCallForwarded(paramfldp, &notifyvariantp->_CallForwarded);

    return SWI_RCODE_OK;

}

/**
 *
 * Name: swigsmcallstatusdecode
 *
 * Purpose: Decodes GSM Call Status notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:   pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles GSM Call Status notification data 
 *          and stores these in the supplied notification structure
 *
 */
package SWI_RCODE  swigsmcallstatusdecode( 
                        swi_uint8 *paramfldp, 
                        struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    notifyvariantp->_CallStatus.sizeStruct = 
                    sizeof(struct SWI_STRUCT_CALL_Status);
    
    SwiUpkGsmCallStatus(paramfldp, &notifyvariantp->_CallStatus);

    return SWI_RCODE_OK;

}


/**
 *
 * Name: swigsmdisplaycficondecode
 *
 * Purpose: Decodes GSM Display Call Forward Icon notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:   pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles GSM Display Call Forward Icon notification 
 *          data and stores these in the supplied notification structure
 *
 */
package SWI_RCODE  swigsmdisplaycficondecode( 
                        swi_uint8 *paramfldp, 
                        struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    notifyvariantp->_CFIcon.sizeStruct = 
                    sizeof(struct SWI_STRUCT_CFIcon);
    notifyvariantp->_CFIcon.bIconOn = *paramfldp++;
    return SWI_RCODE_OK;

}

/**
 *
 * Name: swisspasswordstatusdecode
 *
 * Purpose: Decodes GSM SS Password Status notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:   pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles GSM SS Password Status notification 
 *          data and stores these in the supplied notification structure
 *
 */
package SWI_RCODE  swisspasswordstatusdecode( 
                        swi_uint8 *paramfldp, 
                        struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    notifyvariantp->_SSPasswordStatus.sizeStruct = 
                    sizeof(struct SWI_STRUCT_SS_PasswordStatus);
    SwiUpkGsmSetSSPassword(paramfldp, &notifyvariantp->_SSPasswordStatus);

    return SWI_RCODE_OK;

}

/**
 *
 * Name: swissrespokdecode
 *
 * Purpose: Decodes GSM SS Response Ok notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:   pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles GSM SS Response Ok notification 
 *          data and stores these in the supplied notification structure
 *
 */
package SWI_RCODE  swissrespokdecode( 
                        swi_uint8 *paramfldp, 
                        struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    notifyvariantp->_SSRespOK.sizeStruct = 
                    sizeof(struct SWI_STRUCT_SS_RespOK);
    notifyvariantp->_SSRespOK.nVersion = piget16( &paramfldp );
    notifyvariantp->_SSRespOK.nHandle = piget32( &paramfldp );

    return SWI_RCODE_OK;

}

/**
 *
 * Name: swissfwdinforspdecode
 *
 * Purpose: Decodes GSM SS Response Forwarding Information notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:   pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles GSM SS Response Forwarding Info notification 
 *          data and stores these in the supplied notification structure
 *
 */
package SWI_RCODE  swissfwdinforspdecode( 
                        swi_uint8 *paramfldp, 
                        struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    notifyvariantp->_SSRespFwdInfo.sizeStruct = 
                    sizeof(struct SWI_STRUCT_SS_RespFwdInfo);

    SwiUpkGsmSSFwdInfoRsp(paramfldp, &notifyvariantp->_SSRespFwdInfo);
    
    return SWI_RCODE_OK;

}

/**
 *
 * Name: swisscallbarrrspdecode
 *
 * Purpose: Decodes GSM SS Response Call Bar notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:   pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles GSM SS Response Call Bar notification 
 *          data and stores these in the supplied notification structure
 *
 */
package SWI_RCODE  swisscallbarrspdecode( 
                        swi_uint8 *paramfldp, 
                        struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    notifyvariantp->_SSRespCallBar.sizeStruct = 
                    sizeof(struct SWI_STRUCT_SS_RespCallBar);

    SwiUpkGsmSSCallBarRsp(paramfldp, &notifyvariantp->_SSRespCallBar);
    
    return SWI_RCODE_OK;
}

/**
 *
 * Name: swissinfodatarspdecode
 *
 * Purpose: Decodes GSM SS Info Data Response notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:   pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles GSM SS Info Data Response notification 
 *          data and stores these in the supplied notification structure
 *
 */
package SWI_RCODE  swissinfodatarspdecode( 
                        swi_uint8 *paramfldp, 
                        struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    notifyvariantp->_SSRespInfoData.sizeStruct = 
                    sizeof(struct SWI_STRUCT_SS_RespInfoData);

    SwiUpkGsmSSInfoDataRsp(paramfldp, &notifyvariantp->_SSRespInfoData);
    
    return SWI_RCODE_OK;
}

/**
 *
 * Name: swissstatusrspdecode
 *
 * Purpose: Decodes GSM SS Status Response notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:   pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles GSM SS Status Response notification 
 *          data and stores these in the supplied notification structure
 *
 */
package SWI_RCODE  swissstatusrspdecode( 
                        swi_uint8 *paramfldp, 
                        struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    notifyvariantp->_SSRespStatus.sizeStruct = 
                    sizeof(struct SWI_STRUCT_SS_RespStatus);

    SwiUpkGsmSSStatusRsp(paramfldp, &notifyvariantp->_SSRespStatus);
    
    return SWI_RCODE_OK;
}

/**
 *
 * Name: swissfwdnumrspdecode
 *
 * Purpose: Decodes GSM SS Forwarding Number Response notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:   pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles GSM SS Forwarding Number Response notification 
 *          data and stores these in the supplied notification structure
 *
 */
package SWI_RCODE  swissfwdnumrspdecode( 
                        swi_uint8 *paramfldp, 
                        struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    notifyvariantp->_SSRespFwdNum.sizeStruct = 
                    sizeof(struct SWI_STRUCT_SS_RespFwdNum);

    SwiUpkGsmSSFwdNumRsp(paramfldp, &notifyvariantp->_SSRespFwdNum);
    
    return SWI_RCODE_OK;
}

/**
 *
 * Name: swissbslistrspdecode
 *
 * Purpose: Decodes GSM SS BS List Response notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:   pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles GSM SS BS List Response notification 
 *          data and stores these in the supplied notification structure
 *
 */
package SWI_RCODE  swissbslistrspdecode( 
                        swi_uint8 *paramfldp, 
                        struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    notifyvariantp->_SSRespBSList.sizeStruct = 
                    sizeof(struct SWI_STRUCT_SS_RespBSList);

    SwiUpkGsmSSBSListRsp(paramfldp, &notifyvariantp->_SSRespBSList);
    
    return SWI_RCODE_OK;
}

/**
 *
 * Name: swissfwdlistrspdecode
 *
 * Purpose: Decodes GSM SS Forwarding List Response notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:   pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles GSM SS Forwarding List Response notification 
 *          data and stores these in the supplied notification structure
 *
 */
package SWI_RCODE  swissfwdlistrspdecode( 
                        swi_uint8 *paramfldp, 
                        struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    notifyvariantp->_SSRespFwdList.sizeStruct = 
                    sizeof(struct SWI_STRUCT_SS_RespListFwdInfo);

    SwiUpkGsmSSFwdListRsp(paramfldp, &notifyvariantp->_SSRespFwdList);
    
    return SWI_RCODE_OK;
}

/**
 *
 * Name: swissrawrspdecode
 *
 * Purpose: Decodes GSM SS Raw Response notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:   pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles GSM SS Raw Response notification 
 *          data and stores these in the supplied notification structure
 *
 */
package SWI_RCODE  swissrawrspdecode( 
                        swi_uint8 *paramfldp, 
                        struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    notifyvariantp->_SSRespRaw.sizeStruct = 
                    sizeof(struct SWI_STRUCT_SS_RespRaw);

    SwiUpkGsmSSRawRsp(paramfldp, &notifyvariantp->_SSRespRaw);
    
    return SWI_RCODE_OK;
}

/**
 *
 * Name: swisspwdrspdecode
 *
 * Purpose: Decodes GSM SS Password Response notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:   pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles GSM SS Password Response notification 
 *          data and stores these in the supplied notification structure
 *
 */
package SWI_RCODE  swisspwdrspdecode( 
                        swi_uint8 *paramfldp, 
                        struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    notifyvariantp->_SSRespPassword.sizeStruct = 
                    sizeof(struct SWI_STRUCT_SS_RespPassword);

    SwiUpkGsmSSPwdRsp(paramfldp, &notifyvariantp->_SSRespPassword);
    
    return SWI_RCODE_OK;
}

/**
 *
 * Name: swissclirrspdecode
 *
 * Purpose: Decodes GSM SS CLIR Response notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:   pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles GSM SS CLIR Response notification 
 *          data and stores these in the supplied notification structure
 *
 */
package SWI_RCODE  swissclirrspdecode( 
                        swi_uint8 *paramfldp, 
                        struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    notifyvariantp->_SSRespClir.sizeStruct = 
                    sizeof(struct SWI_STRUCT_SS_RespClir);

    SwiUpkGsmSSClirRsp(paramfldp, &notifyvariantp->_SSRespClir);
    
    return SWI_RCODE_OK;
}

/**
 *
 * Name: swisserrorrspdecode
 *
 * Purpose: Decodes GSM SS Error Response notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:   pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles GSM SS Error Response notification 
 *          data and stores these in the supplied notification structure
 *
 */
package SWI_RCODE  swisserrorrspdecode( 
                        swi_uint8 *paramfldp, 
                        struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    notifyvariantp->_SSRespError.sizeStruct = 
                    sizeof(struct SWI_STRUCT_SS_RespError);

    SwiUpkGsmSSErrorRsp(paramfldp, &notifyvariantp->_SSRespError);
    
    return SWI_RCODE_OK;
}

/**
 *
 * Name: swissrejectrspdecode
 *
 * Purpose: Decodes GSM SS Reject Response notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:   pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles GSM SS Reject Response notification 
 *          data and stores these in the supplied notification structure
 *
 */
package SWI_RCODE  swissrejectrspdecode( 
                        swi_uint8 *paramfldp, 
                        struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    notifyvariantp->_SSRespReject.sizeStruct = 
                    sizeof(struct SWI_STRUCT_SS_RespReject);

    SwiUpkGsmSSRejectRsp(paramfldp, &notifyvariantp->_SSRespReject);
    
    return SWI_RCODE_OK;
}

/**
 *
 * Name: swisscancelrspdecode
 *
 * Purpose: Decodes GSM SS Cancel Response notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:   pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles GSM SS Cancel Response notification 
 *          data and stores these in the supplied notification structure
 *
 */
package SWI_RCODE  swisscancelrspdecode( 
                        swi_uint8 *paramfldp, 
                        struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    notifyvariantp->_SSRespCancel.sizeStruct = 
                    sizeof(struct SWI_STRUCT_SS_RespCancel);

    SwiUpkGsmSSCancelRsp(paramfldp, &notifyvariantp->_SSRespCancel);
    
    return SWI_RCODE_OK;
}

/**
 *
 * Name: swissreleaserspdecode
 *
 * Purpose: Decodes GSM SS Release Response notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:   pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles GSM SS Release Response notification 
 *          data and stores these in the supplied notification structure
 *
 */
package SWI_RCODE  swissreleaserspdecode( 
                        swi_uint8 *paramfldp, 
                        struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    notifyvariantp->_SSRespRelease.sizeStruct = 
                    sizeof(struct SWI_STRUCT_SS_RespRelease);

    notifyvariantp->_SSRespRelease.nVersion = piget16( &paramfldp );
    notifyvariantp->_SSRespRelease.nHandle = piget32( &paramfldp );
    
    return SWI_RCODE_OK;
}

/**
 *
 * Name: swissnotifydecode
 *
 * Purpose: Decodes GSM SS Notify notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:   pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles GSM SS Notify notification 
 *          data and stores these in the supplied notification structure
 *
 */
package SWI_RCODE  swissnotifydecode( 
                        swi_uint8 *paramfldp, 
                        struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    notifyvariantp->_SSNotify.sizeStruct = 
                    sizeof(struct SWI_STRUCT_SSNotify);

    notifyvariantp->_SSNotify.nVersion = piget16( &paramfldp );
    notifyvariantp->_SSNotify.eServiceCode = *paramfldp++;
    
    return SWI_RCODE_OK;
}

/**
 *
 * Name: swivoicemessagedecode
 *
 * Purpose: Decodes GSM Voice Message notification 
 *             
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:   pointer to global notification structure for
 *                                 storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   This function handles GSM Voice Message notification 
 *          data and stores these in the supplied notification structure
 *
 */
package SWI_RCODE  swivoicemessagedecode( 
                        swi_uint8 *paramfldp, 
                        struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    notifyvariantp->_VoiceMessage.sizeStruct = 
                    sizeof(struct SWI_STRUCT_VoiceMessage);

    SwiUpkGsmVoiceMessage(paramfldp, &notifyvariantp->_VoiceMessage);
    
    return SWI_RCODE_OK;
}
/*
 * $Log: SwiIntDecodeNotifyGsm.c,v $
 */
