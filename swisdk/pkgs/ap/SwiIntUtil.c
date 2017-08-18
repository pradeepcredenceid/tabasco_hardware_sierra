/**
 *
 * $Id: SwiIntUtil.c,v 1.38 2011/02/11 22:09:02 ygao Exp $
 *
 * Filename: SwiIntUtil.c
 *
 * Purpose:  Contains source code for Common API utilities for the
 *           SDK. Common means common to all wireless data
 *           technologies, CDMA, GSM, etc. 
 *
 * Copyright: (c) 2011 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "SwiDataTypes.h"
#include "SwiApiGsmBasic.h"
#include "SwiApiGsmNetwork.h"
#include "SwiApiCmBasic.h"
#include "SwiApiNotify.h"
#include "ap/apidefs.h"
#include "cn/cnudefs.h"

/**
 *
 * Name:    swiairtable - Device type to Product string mapping
 *
 * Purpose: This table contains mapping of product names to device type.  
 *
 * Notes:   MC879x, C888, C889, MC8777, MC8700, USB 306, USB 307, AC503
 *          AC501, MC8790, USB305
 */
local const struct apairservertable swiairtable[] = 
{
    { SWI_DEVICE_C888,    (swi_uint8 *)"C888" },
    { SWI_DEVICE_C889,    (swi_uint8 *)"C889" },
    { SWI_DEVICE_USB305,  (swi_uint8 *)"USB 305" },
    { SWI_DEVICE_USB306,  (swi_uint8 *)"USB 306" },
    { SWI_DEVICE_USB307,  (swi_uint8 *)"USB 307" },
    { SWI_DEVICE_USB308,  (swi_uint8 *)"USB 308" },
    { SWI_DEVICE_MC8700,  (swi_uint8 *)"MC8700" },
    { SWI_DEVICE_MC8704,  (swi_uint8 *)"MC8704" },
    { SWI_DEVICE_MC8705,  (swi_uint8 *)"MC8705" },
    { SWI_DEVICE_MC8777,  (swi_uint8 *)"MC8777" },
    { SWI_DEVICE_MC8790V, (swi_uint8 *)"MC8790V" },
    { SWI_DEVICE_MC8790,  (swi_uint8 *)"MC8790" },  /* Special Case configuration */
    { SWI_DEVICE_MC8801,  (swi_uint8 *)"MC8801" },
    { SWI_DEVICE_SL8080,  (swi_uint8 *)"SL8080" },
    { SWI_DEVICE_SL8081,  (swi_uint8 *)"SL8081" },
    { SWI_DEVICE_SL8082,  (swi_uint8 *)"SL8082" },
    { SWI_DEVICE_SL8083,  (swi_uint8 *)"SL8083" },
    { SWI_DEVICE_SL8084,  (swi_uint8 *)"SL8084" },
    { SWI_DEVICE_SL8085,  (swi_uint8 *)"SL8085" },
    { SWI_DEVICE_SL8090,  (swi_uint8 *)"SL8090" },
    { SWI_DEVICE_SL8091,  (swi_uint8 *)"SL8091" },
    { SWI_DEVICE_SL8092,  (swi_uint8 *)"SL8092" },
    { SWI_DEVICE_SL8092RD,(swi_uint8 *)"SL8092RD" },
    { SWI_DEVICE_SL8093,  (swi_uint8 *)"SL8093" }

};
#define swiairtabsz (sizeof (swiairtable))/(sizeof (struct apairservertable))

/**
 *
 * This is a internal help function that maps a given device product name .
 *
 * \param prodstrp[IN]     Product name string to search for
 * \param devtypep[OUT]    Result of the operation.The device type is returned.
 *          
 *
 * \return  See SwiRcodes.h - always SWI_RCODE_OK.
 *
 */
package SWI_API SWI_RCODE SwiIntMapProdStrToDevType( 
    swi_uint8 *prodstrp,
    swi_uint32 *devtypep)
 {
    swi_uint32 i, len;
    
    /* try to find the name from our tables and return the device type */
    for (i = 0; i < swiairtabsz; i++)
    {
        len = slstrlen ((const char *)prodstrp);
        if (slmemcmp (prodstrp,
                      swiairtable[i].apairprodstrp,
                      len) == 0)
        {
            *devtypep = swiairtable[i].apairdevtype;
            break;
        }
    }
    /* If we can't find it in our table, set it to unknown so that
     * the table can be updated if missing 
     */
    if ( i == swiairtabsz )
        *devtypep = SWI_DEVICE_None;
        
    return SWI_RCODE_OK;
}

/*************
 *
 * Name: SwiIntUcs2Unpack
 *
 * Purpose: Package-wide function designed to unpack a UCS2 string
 *          received in a network byte order CnS packet.
 *             
 * Parms: parmfpp   - memory location of the CnS packet pointer 
 *                    Note that the CnS packet pointer is advanced
 *                    by this function.
 *        destp     - UTF8 string buffer to unpack the UCS2 string into.
 *        maxbytes  - byte size of the UTF8 string buffer provided
 *        numchars  - number of UCS2 character in the string to be unpacked.
 *        fieldsize - size of the UCS2 string field in the CnS packet
 *                    This value is in bytes.
 *
 * Return:  TRUE  - if the conversion was successful
 *          FALSE - if the string was truncated
 *
 * Abort:   if the passed in UCS2 string is larger than the temporary
 *          buffer allocated by this function on the stack.  This abort
 *          indicates to the developer that the temporary buffer size should
 *          be increased.
 *
 * Notes:   This function makes use of a temporary buffer to convert
 *          the UCS2 string from network byte order to the Linux native
 *          format.  Rather than have the caller allocate this memory
 *          this function allocates a temporary buffer on the stack.  The 
 *          temporary buffer is sized to handle a UCS2 string the size of
 *          a 250 byte CnS packet.  This value seems reasonable however its
 *          size may need to be reassess in the future.
 * 
 **************/
package swi_bool SwiIntUcs2Unpack(
    swi_uint8 **parmfpp,
    swi_charp destp,
    swi_uint32 maxbytes,
    swi_uint8 numchars,
    swi_uint8 fieldsize)
{
    swi_UCS2  tempucs2string[125];  /* temporary UCS2 string buffer */
    swi_uint32   i;                 /* index variable */
    swi_uint8 *tempparmfp;          /* temporary pointer to CnS packet */
    swi_bool       success = TRUE;
        
    /* get pointer to CnS packet */
    tempparmfp = *parmfpp;
    
    /* validate that this function can convert a string of this length */
    /* See note in function header for more details on this check */
    if(numchars > 125)
    {
        erAbort("SwiIntUcs2Unpack - small buffer", 0);
    }
    
    /* loop through UCS2 string unpacket each character */
    for(i=0; i < numchars; i++)
    {
        tempucs2string[i] = piget16(&tempparmfp);
    }
    
    /* if the UCS2 string is not empty */
    if(numchars)
    {
        success = slUcs2ToUtf8(&tempucs2string[0], numchars,
                               (unsigned char *)destp, maxbytes);
    }
    
    /* 
     * advance CnS packet pointer past this UCS2 string field and 
     * return new pointer value 
     */
    *parmfpp = *parmfpp + fieldsize;
    
    return(success);
}
/**
 *
 * Translates  SWI_TYPE_Notify to CnS object ids
 *             
 * \param  eNotify  - user notification notation
 *
 * \return  returns the mapping to the corresponding  CNS object
 *
 */
package swi_uint16 SwiGetObjid( enum SWI_TYPE_Notify eNotify )
{
    switch (eNotify)
    {
        case SWI_NOTIFY_RadioPower:
            return CNS_RADIO_POWER;
            break;
        case SWI_NOTIFY_RadioTempState:
            return CNS_RADIO_TEMPERATURE;
            break;
        case SWI_NOTIFY_RadioBattery:
            return CNS_RADIO_BATTERY;
            break;
        case SWI_NOTIFY_RadioBand:
            return CNS_CURRENT_RADIO_BAND;
            break;
        case SWI_NOTIFY_AudioProfile:
            return CNS_AUDIO_PROFILE;
            break;
        case SWI_NOTIFY_HeartBeat:
            return CNS_HEART_BEAT;
            break;
        case SWI_NOTIFY_HeadsetIcon:
            return CNS_HEADSET_ICON;
            break;
        case SWI_NOTIFY_NetworkStatus:
            return CNS_NETWORK_STATE;
            break;
        case SWI_NOTIFY_Rssi:
            return CNS_RSSI;
            break;
        case SWI_NOTIFY_Band:
            return CNS_BAND;
            break;
        case SWI_NOTIFY_BandWrite:
            return CNS_BAND_WRITE;
            break;
        case SWI_NOTIFY_HeadsetState:
            return CNS_HEADSET_STATE;
            break;
        case SWI_NOTIFY_MissedCall:
            return CNS_MISSED_CALL;
            break;
        case SWI_NOTIFY_Temperature:
            return CNS_TEMPERATURE;
            break;
        case SWI_NOTIFY_PktSessionStatus:
            return CNS_PKT_SESSION_STATUS;
            break;
        case SWI_NOTIFY_Registration:
            return CNS_REGISTRATION_IND;
            break;
        case SWI_NOTIFY_TransmitAlert:
            return CNS_TRANSMIT_ALERT;
            break;
        case SWI_NOTIFY_SmsStatus:
            /* GSM device */
            if(swigsmdualrangeuser())
            {
                return CNS_SMS_RECEIVED_MSG_STATUS;
            }
            /* CDMA device */
            else
            {
                return CNS_SMS_RECEIVED_MSG_STATUS_CDMA;
            }
            break;
        case SWI_NOTIFY_SmsSendReference:
            return CNS_SMS_SEND_REFERENCE;
            break;
        case SWI_NOTIFY_SmsSendStatus:
            /* GSM device */
            if(swigsmdualrangeuser())
            {
                return CNS_SMS_SEND_STATUS;
            }
            /* CDMA device */
            else
            {
                return CNS_SMS_SEND_STATUS_CDMA;
            }
            break;
        case SWI_NOTIFY_SmsSimFull:
            return CNS_SMS_SIM_FULL;
            break;
        case SWI_NOTIFY_SimStatusExp:
            return CNS_SIM_STATUS_EXP;
            break;
        case SWI_NOTIFY_RegistrationExp:
            return CNS_REGISTRATION_IND_EXP;
            break;
        case SWI_NOTIFY_CallProgress:
            return CNS_CALL_PROGRESS_EXP;
            break;
        case SWI_NOTIFY_CallStatus:
            return CNS_CALL_STATUS_EXP;
            break;
        case SWI_NOTIFY_ByteCounter:
            return CNS_CALL_BYTE_CNT;
            break;
        case SWI_NOTIFY_SmsReady:
            return CNS_SMS_READY_STATUS;
            break;
        case SWI_NOTIFY_PhonebookReady:
            return CNS_PHONE_BOOK_READY_STATUS;
            break;
        case SWI_NOTIFY_PhonebookEntry:
            return CNS_PHONE_NUM_READ_EXP;
            break;
        case SWI_NOTIFY_PlmnMode:
            return CNS_PLMN_MODE;
            break;
        case SWI_NOTIFY_PlmnAvailable:
            return CNS_MANUAL_SELECT_EXP;
            break;
        case SWI_NOTIFY_ModemDisabled:
            return CNS_MODEM_DISABLE;
            break;
        case SWI_NOTIFY_DeviceTime:
            return CNS_DEVICE_TIME;
            break;
        case SWI_NOTIFY_SSPasswordStatus:
            return CNS_SS_PWD_STS;
            break;
        case SWI_NOTIFY_SSRespOK:
            return CNS_SS_OK_RSP;
            break;
        case SWI_NOTIFY_SSRespFwdInfo:
            return CNS_SS_FWD_INFO_RSP;
            break;
        case SWI_NOTIFY_SSRespInfoData:
            return CNS_SS_INFO_DATA_RSP;
            break;
        case SWI_NOTIFY_SSRespCallBar:
            return CNS_SS_CALL_BAR_RSP;
            break;
        case SWI_NOTIFY_SSRespRaw:
            return CNS_SS_RAW_RSP;
            break;
        case SWI_NOTIFY_SSRespStatus:
            return CNS_SS_STATUS_RSP;
            break;
        case SWI_NOTIFY_SSRespFwdNum:
            return CNS_SS_FWD_NUM_RSP;
            break;
        case SWI_NOTIFY_SSRespBSList:
            return CNS_SS_BS_LIST_RSP;
            break;
        case SWI_NOTIFY_SSRespFwdList:
            return CNS_SS_FWD_LIST_RSP;
            break;
        case SWI_NOTIFY_SSRespPassword:
            return CNS_SS_PWD_RSP;
            break;
        case SWI_NOTIFY_SSRespClir:
            return CNS_SS_CLIR_RSP;
            break;
        case SWI_NOTIFY_SSRespError:
            return CNS_SS_ERR_RSP;
            break;
        case SWI_NOTIFY_SSRespReject:
            return CNS_SS_REJECT_RSP;
            break;
        case SWI_NOTIFY_SSRespCancel:
            return CNS_SS_CANCEL_RSP;
            break;
        case SWI_NOTIFY_SSRespRelease:
            return CNS_SS_RELEASE_RSP;
            break;
        case SWI_NOTIFY_SSNotify:
            return CNS_SS_NOTIFY;
            break;
        case SWI_NOTIFY_PlmnReady:
            return CNS_MANUAL_SELECT_READY;
            break;
        case SWI_NOTIFY_VoiceMessage:
            return CNS_VMWF;
            break;
        case SWI_NOTIFY_IconGprsState:
            return CNS_ICON_GPRS;
            break;
        case SWI_NOTIFY_IconRoamState:
            return CNS_ICON_ROAM;
            break;
        case SWI_NOTIFY_CallForwarded:
            return CNS_CALL_FORWARDED;
            break;
        case SWI_NOTIFY_CFIcon:
            return CNS_ICON_CF;
            break;
        case SWI_NOTIFY_STKCommand:
            return CNS_STK_CMD;
            break;
        case SWI_NOTIFY_DataRate:
            return CNS_DATA_RATE;
            break;
        case SWI_NOTIFY_SmsStoreExp:
            return CNS_SMS_STORE_EXP;
            break;
        case SWI_NOTIFY_SmsSendExp:
            return CNS_SMS_SEND_EXP;
            break;
        case SWI_NOTIFY_ServiceIcon:
            return CNS_SERVICE_ICON;
            break;
        case SWI_NOTIFY_TTYControl:
            return CNS_TTY_CONTROL;
            break;
        case SWI_NOTIFY_SmsReceived:
            return CNS_SMS_RECEIVED;
            break;
        case SWI_NOTIFY_SmsDeliver:
            return CNS_SMS_DELIVER;
            break;
        case SWI_NOTIFY_SmsSubmit:
            return CNS_SMS_SUBMIT;
            break;
        case SWI_NOTIFY_SmsStatusReport:
            return CNS_SMS_STATUS_REPORT;
            break;
        case SWI_NOTIFY_SmsSimStatus:
            return CNS_SMS_SIM_STATUS;
            break;
        case SWI_NOTIFY_SS_USSD_REQ:
            return CNS_SS_USSD_REQ_EXP;
            break;
        case SWI_NOTIFY_PktSessionCall:
            return CNS_PKT_SESSION_CALL;
            break;
        case SWI_NOTIFY_NvStore:
            return CNS_NVRAM_STORE;
            break;
        case SWI_NOTIFY_NVUpdateFromFile:
            return CNS_NVUPDATE_FROM_FILE;
            break;
        case SWI_NOTIFY_HSDPA_TestChan:
            return CNS_HSDPA_TEST_CHAN;
            break;
        case SWI_NOTIFY_GsmSystemStatus:
            return CNS_SYSTEM_STATUS;
            break;
        case SWI_NOTIFY_RscpEcIo:
            return CNS_RSCP_ECIO;
            break;
        case SWI_NOTIFY_GsmCellInfo:
            return CNS_GSM_CELL_INFO;
            break;
        case SWI_NOTIFY_GsmProfileChange:
            return CNS_PROFILE_WRITE;
            break;
        case SWI_NOTIFY_LbsPdData:
            return CNS_LBS_PD_DATA;
            break;
        case SWI_NOTIFY_LbsPdDone:
            return CNS_LBS_PD_DONE;
            break;
        case SWI_NOTIFY_LbsPdEnd:
            return CNS_LBS_PD_END;
            break;
        case SWI_NOTIFY_LbsPdBegin:
            return CNS_LBS_PD_BEGIN;
            break;
        case SWI_NOTIFY_LbsPdUpdateFailure:
            return CNS_LBS_PD_UPDATE_FAILURE;
            break;
        case SWI_NOTIFY_LbsPdDloadBegin:
            return CNS_LBS_PD_DLOAD_BEGIN;
            break;
        case SWI_NOTIFY_LbsPdDloadData:
            return CNS_LBS_PD_DLOAD_DATA;
            break;
        case SWI_NOTIFY_LbsPdDloadDone:
            return CNS_LBS_PD_DLOAD_DONE;
            break;
        case SWI_NOTIFY_LbsPdDloadEnd:
            return CNS_LBS_PD_DLOAD_END;
            break;
        case SWI_NOTIFY_LbsPaIpAddr:
            return CNS_LBS_PA_IP_ADDR;
            break;
        case SWI_NOTIFY_LbsPaGpsLock:
            return CNS_LBS_PA_GPS_LOCK;
            break;
        case SWI_NOTIFY_LbsPaPtlmMode:
            return CNS_LBS_PA_PTLM_MODE;
            break;
        case SWI_NOTIFY_LbsPaPortId:
            return CNS_LBS_PA_PORT_ID;
            break;
        case SWI_NOTIFY_LbsPaPrivacy:
            return CNS_LBS_PA_PRIVACY;
            break;
        case SWI_NOTIFY_LbsPaNetAccess:
            return CNS_LBS_PA_NET_ACCESS;
            break;
        case SWI_NOTIFY_LbsPaBsInfo:
            return CNS_LBS_PA_BS_INFO;
            break;
        case SWI_NOTIFY_LbsPaWarmBegin:
            return CNS_LBS_PA_WARM_BEGIN;
            break;
        case SWI_NOTIFY_LbsPaWarmStatus:
            return CNS_LBS_PA_WARM_STATUS;
            break;
        case SWI_NOTIFY_LbsPaWarmDone:
            return CNS_LBS_PA_WARM_DONE;
            break;
        case SWI_NOTIFY_LbsSatInfo:
            return CNS_LBS_SAT_INFO;
            break;
        case SWI_NOTIFY_LbsPdGetStatus:
            return CNS_LBS_PD_GET_STATUS;
            break;
        case SWI_NOTIFY_LbsNiReq:
            return CNS_LBS_NI_REQUEST;
            break;
        case SWI_NOTIFY_LbsXtraAssistData:
            return CNS_LBS_XTRA_ASSIST_DATA;
            break;
        case SWI_NOTIFY_LbsXtraTimeInfo:
            return CNS_LBS_XTRA_TIME_INFO;
            break;
        case SWI_NOTIFY_LbsXtraDloadTrigger:
            return CNS_LBS_XTRA_DLOAD_TRIGGER;
            break;

        /* OMA-DM */
        case SWI_NOTIFY_OMADM_Config:
            return CNS_DM_CONFIG;
            break;
        case SWI_NOTIFY_OMADM_State:
            return CNS_DM_SESSION_STATE;
            break;
        case SWI_NOTIFY_OMADM_NI_Alert:
            return CNS_DM_NI_ALERT;
            break;
        case SWI_NOTIFY_OMADM_UI_Alert_Display:
            return CNS_DM_UI_ALERT_DISPLAY;
            break;
        case SWI_NOTIFY_OMADM_UI_Alert_Confirm:
            return CNS_DM_UI_ALERT_CONFIRM;
            break;
        case SWI_NOTIFY_OMADM_UI_Alert_Input:
            return CNS_DM_UI_ALERT_INPUT;
            break;
        case SWI_NOTIFY_OMADM_UI_Alert_Choice:
            return CNS_DM_UI_ALERT_CHOICE;
            break;
        /* DM-DL */
        case SWI_NOTIFY_DM_DL_Progress:
            return CNS_DM_DL_PROGRESS;
            break;
        case SWI_NOTIFY_DM_DL_Alert:
            return CNS_DM_DL_ALERT;
            break;
        case SWI_NOTIFY_DM_DL_InstallStatus:
            return CNS_DM_DL_INSTALL_STATUS;
            break;

        case SWI_NOTIFY_ChannelNumber:    
            return CNS_CHANNEL_NUMBER;
            break;
        case SWI_NOTIFY_ChannelState:
            return CNS_CHANNEL_STATE;
            break;
        case SWI_NOTIFY_ServiceIndication:
            return CNS_SERVICE_INDICATION;
            break;
        case SWI_NOTIFY_RoamingStatus:
            return CNS_ROAMING_STATUS;
            break;
        case SWI_NOTIFY_Ecio:
            return CNS_ECIO;
            break;
        case SWI_NOTIFY_AuthenticationStatus:
            return CNS_AUTH_STATUS;
            break;
        case SWI_NOTIFY_CallByteCounter:
            return CNS_CALL_BYTE_COUNTER;
            break;
        case SWI_NOTIFY_CallConnected:
            return CNS_CALL_CONNECTED;
            break;
        case SWI_NOTIFY_CallConnecting:
            return CNS_CALL_CONNECTING;
            break;
        case SWI_NOTIFY_CallIncoming:
            return CNS_CALL_INCOMING;
            break;
        case SWI_NOTIFY_CallCallerID:
            return CNS_CALL_CALLER_ID;
            break;
        case SWI_NOTIFY_CallExtendedCallerID:
            return CNS_CALL_EXTENDED_CALLER_ID;
            break;
        case SWI_NOTIFY_CallDisconnected:
            return CNS_CALL_DISCONNECTED;
            break;
        case SWI_NOTIFY_CallError:
            return CNS_CALL_ERROR;
            break;
        case SWI_NOTIFY_Prev:
            return CNS_PREV;
            break;
        case SWI_NOTIFY_ActiveNam:
            return CNS_ACTIVE_NAM;
            break;
        case SWI_NOTIFY_TechSid:
            return CNS_TECH_SID;
            break;
        case SWI_NOTIFY_TechNid:
            return CNS_TECH_NID;
            break;
        case SWI_NOTIFY_TechBsInfo:
            return CNS_TECH_BS_INFO;
            break;
        case SWI_NOTIFY_TechBandClass:
            return CNS_TECH_BANDCLASS;
            break;
        case SWI_NOTIFY_TechRegZone:
            return CNS_TECH_REG_ZONE;
            break;
        case SWI_NOTIFY_TechPacketZone:
            return CNS_TECH_PACKET_ZONE;
            break;
        case SWI_NOTIFY_TechPcsBlock:
            return CNS_TECH_PCS_BLOCK;
            break;
        case SWI_NOTIFY_TechCellChannel:
            return CNS_TECH_CELL_CHANNEL;
            break;
        case SWI_NOTIFY_SystemTime:
            return CNS_SYSTEM_TIME;
            break;
        case SWI_NOTIFY_ActivationStatus:
            return CNS_ACTIVATION_STATUS;
            break;
        case SWI_NOTIFY_V42BisEnabled:
            return CNS_V42BIS_ENABLED;
            break;
        case SWI_NOTIFY_CallNotificationStatus:
            return CNS_CALL_NOTIFICATION_STATUS;
            break;
        case SWI_NOTIFY_CallDormant:
            return CNS_CALL_DORMANT;
            break;
        case SWI_NOTIFY_CallPrivacy:
            return CNS_CALL_PRIVACY;
            break;
        case SWI_NOTIFY_E911Mode:
            return CNS_E911_MODE;
            break;
        case SWI_NOTIFY_ModemTooHotIndication:
            return CNS_MDM_TOOHOT_INDICATION;
            break;
        case SWI_NOTIFY_OTASPState:
            return CNS_CALL_OTASP_STATE;
            break;
#if 0            
        case SWI_NOTIFY_SPCS_HIOTA_STATUS:
            return CNS_SPCS_HIOTA_STATUS;
            break;
#endif
        case SWI_NOTIFY_PowerSaveEnter:
            return CNS_POWERSAVE_ENTER;
            break;
        case SWI_NOTIFY_PowerSaveExit:
            return CNS_POWERSAVE_EXIT;
            break;
        case SWI_NOTIFY_FlashBurn:
            return CNS_FLASH_BURN_CYCLE;
            break;
        case SWI_NOTIFY_ExcessSPCFailure:
            return CNS_EXCESS_SPC_FAILURE;
            break;
        case SWI_NOTIFY_RuimStatus:
            return CNS_RUIM_STATUS;
            break;
        case SWI_NOTIFY_Power:
            return CNS_RADIO_POWER_LEGACY;
            break;
        case SWI_NOTIFY_IOTA_Status:
            return CNS_IOTA_STATUS_LEGACY;
            break;
        case SWI_NOTIFY_IOTA_WAPPush:
            return CNS_IOTA_WAP_PUSH_LEGACY;
            break;

        case SWI_NOTIFY_HdrNotificationStatus:
            return CNS_HDR_NOTIFICATION_STATUS;
            break;
        case SWI_NOTIFY_Hdr_Acq:
            return CNS_HDR_ACQ;
            break;
        case SWI_NOTIFY_Hdr_Rssi:
            return CNS_HDR_RSSI;
            break;
        case SWI_NOTIFY_Rssi_Ecio:
            return CNS_RSSI_ECIO;
            break;
        case SWI_NOTIFY_Hdr_Roam_Status:
            return CNS_HDR_ROAM_STATUS;
            break;
        case SWI_NOTIFY_Hdr_Srv_State:
            return CNS_HDR_SRV_STATE;
            break;
        case SWI_NOTIFY_Hdr_Hybr_Pref:
            return CNS_HDR_HYBR_PREF;
            break;
        case SWI_NOTIFY_Idle_Digital_Mode:
            return CNS_IDLE_DIGITAL_MODE;
            break;
        case SWI_NOTIFY_DDTM_Pref:
            return CNS_DDTM_PREF;
            break;
        case SWI_NOTIFY_Hdr_PowerSave_Enter:
            return CNS_HDR_POWERSAVE_ENTER;
            break;
        case SWI_NOTIFY_Hdr_PowerSave_Exit:
            return CNS_HDR_POWERSAVE_EXIT;
            break;

        case SWI_NOTIFY_TechHdrNotificationStatus:
            return CNS_TECH_HDR_NOTIFICATION_STATUS;
            break;
        case SWI_NOTIFY_Tech_Hdr_Prev:
            return CNS_TECH_HDR_PREV;
            break;
        case SWI_NOTIFY_Tech_Hdr_Sector:
            return CNS_TECH_HDR_SECTOR;
            break;
        case SWI_NOTIFY_Tech_Hdr_State:
            return CNS_TECH_HDR_STATE;
            break;
        case SWI_NOTIFY_Tech_Hdr_Channel:
            return CNS_TECH_HDR_CHANNEL;
            break;
        case SWI_NOTIFY_Tech_Hdr_Session:
            return CNS_TECH_HDR_SESSION;
            break;
        case SWI_NOTIFY_Tech_Hdr_An_Auth:
            return CNS_TECH_HDR_AN_AUTH;
            break;
        case SWI_NOTIFY_Tech_Hdr_Drc_Val:
            return CNS_TECH_HDR_DRC_VAL;
            break;
        case SWI_NOTIFY_Tech_Hdr_Drc_Cover:
            return CNS_TECH_HDR_DRC_COVER;
            break;
        case SWI_NOTIFY_Tech_Hdr_Rri:
            return CNS_TECH_HDR_RRI;
            break;
        case SWI_NOTIFY_Tech_Hdr_Stats:
            return CNS_TECH_HDR_STATS;
            break;

        case SWI_NOTIFY_Tech_Hdr_Su_Fwd_Stats:
            return CNS_TECH_HDR_SU_FWD_STATS;
            break;
        case SWI_NOTIFY_Tech_Hdr_Su_Fwd_Slot_Cnt:
            return CNS_TECH_HDR_SU_FWD_SLOT_CNT;
            break;
        case SWI_NOTIFY_Tech_Hdr_Su_Fwd_Slot_Cnt2:
            return CNS_TECH_HDR_SU_FWD_SLOT_CNT_2;
            break;
        case SWI_NOTIFY_Tech_Hdr_Mu_Fwd_Stats:
            return CNS_TECH_HDR_MU_FWD_STATS;
            break;
        case SWI_NOTIFY_Tech_Hdr_Personality:
            return CNS_TECH_HDR_PERSONALITY;
            break;
        case SWI_NOTIFY_Tech_Hdr_Mac_Index:
            return CNS_TECH_HDR_MAC_INDEX;
            break;

        case SWI_NOTIFY_SpkrVol:
            return CNS_SPKR_VOL;
            break;

        /* don't do anything for these types */
        case SWI_NOTIFY_NULL:
        case SWI_NOTIFY_Reset:
        case SWI_NOTIFY_Debug:
        case SWI_NOTIFY_FwDownload:
        case SWI_NOTIFY_Dhcp:
        case SWI_NOTIFY_PPP:
        case SWI_NOTIFY_AirServerChange:
            break;
        default:
            break;
    }
    return 0xFFFF; /* when the mapping is not found */
}

/**
 *
 * Translates CnS object ids to SWI_TYPE_Notify 
 *          
 * \param  cnsobjid - CNS object id
 *
 * \return  the mapping to the corresponding  SWI_NOTIFY_TYPE
 *
 */
package swi_uint16 SwiGetNotify( enum cnsobjectids cnsobjid )
{
    switch (cnsobjid)
    {
        case CNS_RADIO_POWER:
            return SWI_NOTIFY_RadioPower;
            break;
        case CNS_RADIO_TEMPERATURE:
            return SWI_NOTIFY_RadioTempState;
            break;
        case CNS_RADIO_BATTERY:
            return SWI_NOTIFY_RadioBattery;
            break;
        case CNS_CURRENT_RADIO_BAND:
            return SWI_NOTIFY_RadioBand;
            break;
        case CNS_AUDIO_PROFILE:
            return SWI_NOTIFY_AudioProfile;
            break;
        case CNS_HEART_BEAT:
            return SWI_NOTIFY_HeartBeat;
            break;
        case CNS_HEADSET_ICON:
            return SWI_NOTIFY_HeadsetIcon;
            break;
        case CNS_NETWORK_STATE:
            return SWI_NOTIFY_NetworkStatus;
            break;
        case CNS_RSSI:
            return (swi_uint16 )SWI_NOTIFY_Rssi;
            break;
        case CNS_BAND:
            return SWI_NOTIFY_Band;
            break;
        case CNS_BAND_WRITE:
            return SWI_NOTIFY_BandWrite;
            break;
        case CNS_HEADSET_STATE:
            return SWI_NOTIFY_HeadsetState;
            break;
        case CNS_MISSED_CALL:
            return SWI_NOTIFY_MissedCall;
            break;
        case CNS_TEMPERATURE:
            return SWI_NOTIFY_Temperature;
            break;
        case CNS_PKT_SESSION_STATUS:
            return SWI_NOTIFY_PktSessionStatus;
            break;
        case CNS_REGISTRATION_IND:
            return SWI_NOTIFY_Registration;
            break;
        case CNS_TRANSMIT_ALERT:
            return SWI_NOTIFY_TransmitAlert;
            break;
        case CNS_SMS_RECEIVED_MSG_STATUS:
            return SWI_NOTIFY_SmsStatus;
            break;
        case CNS_SMS_SEND_REFERENCE:
            return SWI_NOTIFY_SmsSendReference;
            break;
        case CNS_SMS_SEND_STATUS:
            return SWI_NOTIFY_SmsSendStatus;
            break;
        case CNS_SMS_SIM_FULL:
            return SWI_NOTIFY_SmsSimFull;
            break;
        case CNS_SIM_STATUS_EXP:
            return SWI_NOTIFY_SimStatusExp;
            break;
        case CNS_REGISTRATION_IND_EXP:
            return SWI_NOTIFY_RegistrationExp;
            break;
        case CNS_CALL_PROGRESS_EXP:
            return SWI_NOTIFY_CallProgress;
            break;
        case CNS_CALL_STATUS_EXP:
            return SWI_NOTIFY_CallStatus;
            break;
        case CNS_CALL_BYTE_CNT:
            return SWI_NOTIFY_ByteCounter;
            break;
        case CNS_SMS_READY_STATUS:
            return SWI_NOTIFY_SmsReady;
            break;
        case CNS_PHONE_BOOK_READY_STATUS:
            return SWI_NOTIFY_PhonebookReady;
            break;
        case CNS_PHONE_NUM_READ_EXP:
            return SWI_NOTIFY_PhonebookEntry;
            break;
        case CNS_PLMN_MODE:
            return SWI_NOTIFY_PlmnMode;
            break;
        case CNS_MANUAL_SELECT_EXP:
            return SWI_NOTIFY_PlmnAvailable;
            break;
        case CNS_MODEM_DISABLE:
            return SWI_NOTIFY_ModemDisabled;
            break;
        case CNS_SS_PWD_STS:
            return SWI_NOTIFY_SSPasswordStatus;
            break;
        case CNS_SS_OK_RSP:
            return SWI_NOTIFY_SSRespOK;
            break;
        case CNS_SS_FWD_INFO_RSP:
            return SWI_NOTIFY_SSRespFwdInfo;
            break;
        case CNS_SS_INFO_DATA_RSP:
            return SWI_NOTIFY_SSRespInfoData;
            break;
        case CNS_SS_CALL_BAR_RSP:
            return SWI_NOTIFY_SSRespCallBar;
            break;
        case CNS_SS_RAW_RSP:
            return SWI_NOTIFY_SSRespRaw;
            break;
        case CNS_SS_STATUS_RSP:
            return SWI_NOTIFY_SSRespStatus;
            break;
        case CNS_SS_FWD_NUM_RSP:
            return SWI_NOTIFY_SSRespFwdNum;
            break;
        case CNS_SS_BS_LIST_RSP:
            return SWI_NOTIFY_SSRespBSList;
            break;
        case CNS_SS_FWD_LIST_RSP:
            return SWI_NOTIFY_SSRespFwdList;
            break;
        case CNS_SS_PWD_RSP:
            return SWI_NOTIFY_SSRespPassword;
            break;
        case CNS_SS_CLIR_RSP:
            return SWI_NOTIFY_SSRespClir;
            break;
        case CNS_SS_ERR_RSP:
            return SWI_NOTIFY_SSRespError;
            break;
        case CNS_SS_REJECT_RSP:
           return SWI_NOTIFY_SSRespReject;
            break;
        case CNS_SS_CANCEL_RSP:
            return SWI_NOTIFY_SSRespCancel;
            break;
        case CNS_SS_RELEASE_RSP:
            return SWI_NOTIFY_SSRespRelease;
            break;
        case CNS_SS_NOTIFY:
            return SWI_NOTIFY_SSNotify;
            break;
        case CNS_MANUAL_SELECT_READY:
            return SWI_NOTIFY_PlmnReady;
            break;
        case CNS_VMWF:
            return SWI_NOTIFY_VoiceMessage;
            break;
        case CNS_ICON_GPRS:
            return SWI_NOTIFY_IconGprsState;
            break;
        case CNS_ICON_ROAM:
            return SWI_NOTIFY_IconRoamState;
            break;
        case CNS_CALL_FORWARDED:
            return SWI_NOTIFY_CallForwarded;
            break;
        case CNS_ICON_CF:
            return SWI_NOTIFY_CFIcon;
            break;
        case CNS_STK_CMD:
            return SWI_NOTIFY_STKCommand;
            break;
        case CNS_DATA_RATE:
            return SWI_NOTIFY_DataRate;
            break;
        case CNS_SMS_STORE_EXP:
            return SWI_NOTIFY_SmsStoreExp;
            break;
        case CNS_SMS_SEND_EXP:
            return SWI_NOTIFY_SmsSendExp;
            break;
        case CNS_SERVICE_ICON:
            return SWI_NOTIFY_ServiceIcon;
            break;
        case CNS_TTY_CONTROL:
            return SWI_NOTIFY_TTYControl;
            break;
        case CNS_SMS_RECEIVED:
            return SWI_NOTIFY_SmsReceived;
            break;
        case CNS_SMS_DELIVER:
            return SWI_NOTIFY_SmsDeliver;
            break;
        case CNS_SMS_SUBMIT:
            return SWI_NOTIFY_SmsSubmit;
            break;
        case CNS_SMS_STATUS_REPORT:
            return SWI_NOTIFY_SmsStatusReport;
            break;
        case CNS_SMS_SIM_STATUS:
            return SWI_NOTIFY_SmsSimStatus;
            break;
        case CNS_SS_USSD_REQ_EXP:
            return SWI_NOTIFY_SS_USSD_REQ;
            break;
        case CNS_PKT_SESSION_CALL:
            return SWI_NOTIFY_PktSessionCall;
            break;
        case CNS_NVRAM_STORE:
            return SWI_NOTIFY_NvStore;
            break;
        case CNS_NVUPDATE_FROM_FILE:
            return SWI_NOTIFY_NVUpdateFromFile;
            break;
        case CNS_HSDPA_TEST_CHAN:
            return SWI_NOTIFY_HSDPA_TestChan;
            break;
        case CNS_SYSTEM_STATUS:
            return SWI_NOTIFY_GsmSystemStatus;
            break;
        case CNS_RSCP_ECIO:
            return SWI_NOTIFY_RscpEcIo;
            break;
        case CNS_GSM_CELL_INFO:
            return SWI_NOTIFY_GsmCellInfo;
            break;
        case CNS_PROFILE_WRITE:
            return SWI_NOTIFY_GsmProfileChange;
            break;
        case CNS_SPKR_VOL:
            return SWI_NOTIFY_SpkrVol;
            break;
        default:
            break;
    }
    return 0xffff; /* when the mapping is not found */
}

/**
 *
 * Determines modem technology type given modem type. The function uses
 * information received from the SDK process as a result of an 
 * SWI_NOTIFY_AirServerChange or as a result of a call to 
 * SwiGetAvailAirServers(). If neither of these events has taken place
 * yet, then the information returned by this function is invalid
 *             
 * \return  TRUE  - The modem technology is GSM/UMTS-based
 * \return  FALSE - The modem technology is CDMA-based
 *
 */
package swi_bool swigsmdualrangeuser()
{
    enum SWI_TYPE_Device devtype;
    struct apicb *apicbp;

    apicbp = SwiApiGetCbp();

    /* Read the device type of the last reported modem */
    devtype = apicbp->apairstate.apairdevice;

    /* Translate the AM type to a SWI type */
    switch (devtype)
    {
        /* PC Cards */
        case SWI_DEVICE_AC550:
        case SWI_DEVICE_AC575:
        case SWI_DEVICE_AC580:
        case SWI_DEVICE_AC595:
        case SWI_DEVICE_AC597:
            return FALSE;
            break;

        case SWI_DEVICE_AC710:
        case SWI_DEVICE_AC775:
        case SWI_DEVICE_AC850:
        case SWI_DEVICE_AC860:
        case SWI_DEVICE_AC875:
        case SWI_DEVICE_AC880:
        case SWI_DEVICE_AC881:
            return TRUE;
            break;

        /* Embedded Modules */
        case SWI_DEVICE_MC5720:
        case SWI_DEVICE_MC5725:
        case SWI_DEVICE_MC5727:
        case SWI_DEVICE_MC5728:
        case SWI_DEVICE_SL501X:
            return FALSE;
            break;

        case SWI_DEVICE_MC8704:
        case SWI_DEVICE_MC8705:
        case SWI_DEVICE_MC8755:
        case SWI_DEVICE_MC8765:
        case SWI_DEVICE_MC8775:
        case SWI_DEVICE_MC8780:
        case SWI_DEVICE_MC8781:
        case SWI_DEVICE_MC8785:
        case SWI_DEVICE_MC8790:
        case SWI_DEVICE_MC8790V:
        case SWI_DEVICE_MC8791:
        case SWI_DEVICE_MC8777:
        case SWI_DEVICE_MC8801:
        case SWI_DEVICE_SL8080:
        case SWI_DEVICE_SL8081:
        case SWI_DEVICE_SL8082:
        case SWI_DEVICE_SL8083:
        case SWI_DEVICE_SL8084:
        case SWI_DEVICE_SL8085:
        case SWI_DEVICE_SL8090:
        case SWI_DEVICE_SL8091:
        case SWI_DEVICE_SL8092:
        case SWI_DEVICE_SL8092RD:
        case SWI_DEVICE_SL8093:
            return TRUE;
            break;

        case SWI_DEVICE_EM5625:
        case SWI_DEVICE_EM5725:
            return FALSE;
            break;

        case SWI_DEVICE_EM8780:
        case SWI_DEVICE_EM8781:
        case SWI_DEVICE_M81A:
        case SWI_DEVICE_M81B:
            return TRUE;
            break;

        /* MP - Not Used on Linux SDK */
        case SWI_DEVICE_MP555:
        case SWI_DEVICE_MP595:
            return FALSE;
            break;

        case SWI_DEVICE_MP750:
        case SWI_DEVICE_MP775:
        case SWI_DEVICE_MP875:
        case SWI_DEVICE_MP880:
        case SWI_DEVICE_MP881:
            return TRUE;
            break;

        /* ExpressCards */
        case SWI_DEVICE_AC597E:
        case SWI_DEVICE_AC598E:
            return FALSE;
            break;

        case SWI_DEVICE_AC875E:
        case SWI_DEVICE_AC880E:
        case SWI_DEVICE_AC881E:
        case SWI_DEVICE_AC885E:
        case SWI_DEVICE_AC886E:
            return TRUE; 
            break;

        /* USB Modems */
        case SWI_DEVICE_AC595U:
        case SWI_DEVICE_T598:
        case SWI_DEVICE_T11:
        case SWI_DEVICE_AC802:
            return FALSE;
            break;

        case SWI_DEVICE_AC875U:
        case SWI_DEVICE_AC880U:
        case SWI_DEVICE_AC881U:
        case SWI_DEVICE_C885:
        case SWI_DEVICE_C886:
        case SWI_DEVICE_C888:
        case SWI_DEVICE_C889:
        case SWI_DEVICE_K888:
        case SWI_DEVICE_K889:
        case SWI_DEVICE_C22:
        case SWI_DEVICE_C23:
        case SWI_DEVICE_K22:
        case SWI_DEVICE_K23:
        case SWI_DEVICE_CHspa:
        case SWI_DEVICE_USB306:
            return TRUE;
            break;

        /* Should never happen */
        default:
            erAbort("dualrangeuser: device undefined", devtype );
            return FALSE;
            break;
    }
}

/**
 *
 * Package-wide function designed to accept basic CnS request
 * parameters, package them up and send them to the modem, 
 * blocking the caller until a response is received
 *             
 * \param  reqbufferp - [IN] Pointer to the buffer into which the outgoing
 *                           request will be built
 * \param  objid      - [IN] CnS object id for this request
 * \param  length     - [IN] CnS request parameter field length of the outgoing
 *                           packet. 0 if there is no parameter field in the 
 *                           request
 * \param  timeout    - [IN] Caller-specified CnS timeout for this 
 *                           request/response transaction
 * \param  operation  - [IN] CnS operation type, eg CNS_GET, CNS_SET, etc
 * \param  airint     - [IN] Air interface technology: eg SWI_AMCOMMON, 
 *                           SWI_AMGSM
 * \param inparampp  - [OUT] Pointer to pointer of incoming CnS Parameter field
 *                           from the modem, if any
 * \param parmlengthp - [OUT] Pointer to storage into which the length, if any,
 *                           of the CnS parameter field from the modem will be
 *                           written
 *
 * \return  SWI_RCODE_OK - no problems or, 
 * \sa      Refer to SWI_RCODES.h for details
 *
 */
package SWI_RCODE SwiIntSendnWait(
    swi_uint8           *reqbufferp,
    swi_uint16          objid,
    swi_uint16          length,
    swi_uint32          timeout,
    swi_uint8           operation,
    swi_uint8           airint, 
    swi_uint8           **inparampp,
    swi_uint16          *parmlengthp )

{
    struct amrrparms reqparms;   /* For collecting up the caller's args */
    SWI_RCODE        resultcode; /* Result of calling AM entry point */
    struct apicb *apicbp;

    /* Do the standard checks */
    if(!SwiApiRunning())
    {
        return SWI_RCODE_API_NOT_OPEN;
    }
    
    /* Initialize the package global control block pointer */
    apicbp = SwiApiGetCbp();

    /* If there is no Air Server present do not run API */
    if (!(apicbp->apairstate.apairserverpresent))
    {
        return SWI_RCODE_DEVICE_UNAVAIL;
    }

    /* If the API requires specific technology but the device attached does not
     * match this technology do not run API
     */    
    if ( airint != SWI_AMCOMMON ) 
    {
        if( (swigsmdualrangeuser() && (airint != SWI_AMGSM)) ||
            ( !swigsmdualrangeuser() && (airint != SWI_AMCDMA)) )
        {
            return SWI_RCODE_DEVICE_NOT_SUP;
        }  
    }  

    /* Fill in the AM structure with the required values for this request */
    reqparms.amparmtype = SWI_PARM_CNS; 
    reqparms.amtimeout = timeout;
    reqparms.amparm.amcns.amcnstimeout = timeout;
    reqparms.amparm.amcns.amcnsparameter = cngetappparm();
    reqparms.amparm.amcns.amcnsobjid = objid;
    reqparms.amparm.amcns.amcnslength = length; 
    reqparms.amparm.amcns.amcnsoperation = operation;
    reqparms.amparm.amcns.amcnsairint = airint;

    /* Dispatch this packet to the SDK side, waiting for a response */
    resultcode = amsendnwait( reqbufferp, &reqparms, inparampp );

    /* Pass the caller the length of the CnS parameter field */
    *parmlengthp = reqparms.amparm.amcns.amcnslength;
    
    return(resultcode);
}

/**
 *
 * Prepare and send the Firmware Download request to the SDK.
 *
 * \param filenamep    - pointer to filename buffer 
 * \param imagetype    - image type to be downloaded
 * \param mpdownload   - flag indicating if downloading to an MP
 * \param maxchunksize - maximum image chunk size to be copied at one time
 *                       to the device
 *
 * \return  SWI_RCODE_OK - no problems
 * \sa      See SwiRcodes.h for other result codes.
 *
 */
package SWI_RCODE SwiSendnWaitFwdDwnRequest(
    char                   *filenamep,
    enum swi_amfwimagetype imagetype,
    swi_bool               mpdownload,
    swi_uint32             maxchunksize)
{
    swi_uint8            *inparamp;  /* ptr to param field rx'd from modem */
    struct amrrparms     reqparms;   /* Structure to input values */
    struct apicb         *apicbp;  

    /* Do the standard checks */
    if(!SwiApiRunning())
    {
        return SWI_RCODE_API_NOT_OPEN;
    }
    
    /* Initialize the package global control block pointer */
    apicbp = SwiApiGetCbp();
    
    /* If there is no Air Server present do not run API */
    if (!(apicbp->apairstate.apairserverpresent))
    {
        return SWI_RCODE_DEVICE_UNAVAIL;
    }
    
    /* Fill in the AM structure with the required values for this request */
    reqparms.amparmtype = SWI_PARM_FWDWN; 
    reqparms.amresultcode = SWI_AMMAXRESULT;
    reqparms.amtimeout = 0;  /* no timeout */
    slmemset((char *)&reqparms.amparm.amfwdwn.amfilename[0], 0,
                AMMAXFILENAMESZ); 
    (void)slstrncpy(&reqparms.amparm.amfwdwn.amfilename[0], 
                     filenamep, AMMAXFILENAMESZ - 1);
    reqparms.amparm.amfwdwn.amfwimagetype  = imagetype;
    reqparms.amparm.amfwdwn.ammpdownload   = mpdownload;
    reqparms.amparm.amfwdwn.ammaxchunksize = maxchunksize;

    /* Send the request on its way - Note, this is a blocking call */
    return(amsendnwait( amgetreqbufp(), &reqparms, &inparamp));
}
/*************
 *
 * Name: SwiIntSendnWaitAirServerRequest
 *
 * Purpose: Package-wide function designed to package up a AirServer
 *          request and send it to the modem.  This call will block until 
 *          a response is received from the modem
 *             
 * Parms: [IN]  reqtype            - AirServer Request type
 *        [OUT] airserverpresentp  - Memory location to put the AirServer
 *                                   present flag value. Only changed if
 *                                   result code from amsendnwait() indicates
 *                                   success. Caller must initialize this
 *        [OUT] airserverp         - Pointer to storage in which the present
 *                                   AirServer information will be returned.
 *
 * Return:  SWI_RCODE_OK - no problems or, 
 *          Refer to SWI_RCODES.h for details
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
package SWI_RCODE SwiIntSendnWaitAirServerRequest(
    swi_uint8                    reqtype,
    swi_bool                     *airserverpresentp,
    struct SWI_STRUCT_AirServer  *airserverp )
{
    struct amrrparms reqparms;   /* For collecting up the caller's args */
    SWI_RCODE        resultcode; /* Result of calling AM entry point */
    struct apicb     *apicbp;
    swi_uint8        *inparamp;   
    swi_uint32       devtype;
    
    /* Do the standard checks */
    if(!SwiApiRunning())
    {
        return SWI_RCODE_API_NOT_OPEN;
    }
    
    /* Initialize the package global control block pointer */
    apicbp = SwiApiGetCbp();

    /* Fill in the AM structure with the required values for this request */
    reqparms.amparmtype = SWI_PARM_AIRSRV; 
    reqparms.amtimeout = 0;
    reqparms.amparm.amairsrv.amairoperation = reqtype;

    /* Dispatch this packet to the SDK side, waiting for a response */
    resultcode = amsendnwait( amgetreqbufp(), &reqparms, &inparamp );

    /* Only handle the response if it's valid */
    if( resultcode == SWI_RCODE_OK )
    {
        /* Record the air server only if this info is more up-to-date */
        if(apicbp->apairstate.apairchgera < reqparms.amparm.amairsrv.amairchgera)
        {
            apicbp->apairstate.apairserverpresent = 
                reqparms.amparm.amairsrv.amairdevicestat;
            apicbp->apairstate.apairchgera = 
                reqparms.amparm.amairsrv.amairchgera;
            apicbp->apapprunning = reqparms.amparm.amairsrv.amairapprunning;
            apicbp->apairstate.apairdevice = reqparms.amparm.amairsrv.amairdevicetype;
        }
    
        /* copy the result of request to returning structure */
        airserverp->eDevice = reqparms.amparm.amairsrv.amairdevicetype;
        if ( airserverp->eDevice == SWI_DEVICE_UNIFIEDPID )
        {          
            SwiIntMapProdStrToDevType (&reqparms.amparm.amairsrv.amairprodstr[0], 
                                       &devtype);
    
            apicbp->apairstate.apairdevice = devtype;
        }
        
        slmemcpy(&airserverp->ClientMac[0], 
            &reqparms.amparm.amairsrv.amairclimac[0], 
            LENGTH_MacAddress);
        slmemcpy(&airserverp->ServerMac[0], 
            &reqparms.amparm.amairsrv.amairservmac[0], 
            LENGTH_MacAddress);  

        /* Return if the Air Server information is available */
        *airserverpresentp = reqparms.amparm.amairsrv.amairdevicestat;
    
    }
    return(resultcode);
}

/*************
 *
 * Name: SwiIntSendnWaitATdataPortNameRequest
 *
 * Purpose: Package-wide function designed to package up a Port name
 *          request and send it to the modem.  This call will block until 
 *          a response is received from the modem
 *             
 * Parms: [OUT] sPortNamep         - Pointer to storage in which the present
 *                                   port name information will be returned.
 *
 * Return:  SWI_RCODE_OK - no problems or, 
 *          Refer to SWI_RCODES.h for details
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
package SWI_RCODE SwiIntSendnWaitUsbPortNameRequest(
    struct SWI_STRUCT_UsbPortName *sPortNamep )
{
    struct amrrparms reqparms;   /* For collecting up the caller's args */
    SWI_RCODE        resultcode; /* Result of calling AM entry point */
    struct apicb     *apicbp;
    swi_uint8        *inparamp;   

    /* Do the standard checks */
    if(!SwiApiRunning())
    {
        return SWI_RCODE_API_NOT_OPEN;
    }
    
    /* Initialize the package global control block pointer */
    apicbp = SwiApiGetCbp();

    /* Fill in the AM structure with the required values for this request */
    reqparms.amparmtype = SWI_PARM_PORTNAME; 
    reqparms.amtimeout = 0;

    /* Dispatch this packet to the SDK side, waiting for a response */
    resultcode = amsendnwait( amgetreqbufp(), &reqparms, &inparamp );
        
    /* copy the result of request to returning structure */
    slmemcpy(&(sPortNamep->HipPort[0]),
             &(reqparms.amparm.amusbportname.amhipport[0]),
             AMPORTNAMESZ);

    slmemcpy(&(sPortNamep->DiagPort[0]),
             &(reqparms.amparm.amusbportname.amdiagport[0]),
             AMPORTNAMESZ);

    slmemcpy(&(sPortNamep->NmeaPort[0]),
             &(reqparms.amparm.amusbportname.amnmeaport[0]),
             AMPORTNAMESZ);

    slmemcpy(&(sPortNamep->AtCmdPort[0]),
             &(reqparms.amparm.amusbportname.amatcmdport[0]),
             AMPORTNAMESZ);

    slmemcpy(&(sPortNamep->Data1Port[0]),
             &(reqparms.amparm.amusbportname.amdata1port[0]),
             AMPORTNAMESZ);

    slmemcpy(&(sPortNamep->Data2Port[0]),
             &(reqparms.amparm.amusbportname.amdata2port[0]),
             AMPORTNAMESZ);
             
    slmemcpy(&(sPortNamep->Data3Port[0]),
             &(reqparms.amparm.amusbportname.amdata3port[0]),
             AMPORTNAMESZ);
  
    return(resultcode);
}

/**
 *
 * This function packs a byte array to the CnS object's parameter 
 * field
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                           data will be placed by this function
 * \param pnValue[IN]   Pointer to storage that contains byte array.
 * \param nNum[IN]   Number of bytes to pack.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackU8s( swi_uint8 *paramfieldp, 
                              const swi_uint8 *pnValue, 
                              swi_uint16 nNum )
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */
    
    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piputmulti((const swi_uint8 *)pnValue, &localfieldp, nNum);
    
    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs a swi_uint16 array to the CnS object's parameter 
 * field
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                           data will be placed by this function
 * \param pnValue[IN]   Pointer to storage that contains swi_uint16 array.
 * \param nNum[IN]   Number of swi_uint16 to pack.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackU16s( swi_uint8 *paramfieldp, 
                               const swi_uint16 *pnValue, 
                               swi_uint16 nNum )
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */
    
    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16s( pnValue, nNum, &localfieldp );
    
    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs a swi_uint16 value to the CnS object's parameter 
 * field
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                           data will be placed by this function
 * \param nValue[IN]   Value to pack.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackU16( swi_uint8 *paramfieldp, swi_uint16 nValue )
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */
    
    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16( nValue, &localfieldp );
    
    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs a swi_uint16 value  and a byte array to the CnS 
 * object's parameter field
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                           data will be placed by this function
 * \param nValue[IN]   swi_uint16 value to pack.
 * \param pnValue[IN]   Pointer to storage that contains byte array.
 * \param nNum[IN]   Number of bytes in pnValue to pack.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackU16U8s( swi_uint8 *paramfieldp, 
                                 swi_uint16 nValue, 
                                 const swi_uint8 *pnValue, 
                                 swi_uint16 nNum )
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */
    
    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16( nValue, &localfieldp );
    piputmulti((const swi_uint8 *)pnValue, &localfieldp, nNum);
    
    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs two  swi_uint16 value to the CnS object's parameter 
 * field
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                           data will be placed by this function
 * \param nValue1[IN]   First swi_uint16 value to pack.
 * \param nValue2[IN]   Second swi_uint16 value to pack.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackU16U16( swi_uint8 *paramfieldp, 
                                 swi_uint16 nValue1, 
                                 swi_uint16 nValue2 )
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */
    
    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16( nValue1, &localfieldp );
    piput16( nValue2, &localfieldp );
    
    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs a swi_uint32 array to the CnS object's parameter 
 * field
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                           data will be placed by this function
 * \param pnValue[IN]   Pointer to storage that contains swi_uint32 array.
 * \param nNum[IN]   Number of swi_uint32 to pack.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackU32s( swi_uint8 *paramfieldp, 
                               const swi_uint32 *pnValue, 
                               swi_uint16 nNum )
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */
    
    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput32s( pnValue, nNum, &localfieldp );
    
    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs NV index to the CnS object's parameter 
 * field
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                           data will be placed by this function
 * \param nNvIndex[IN]   NV index to pack.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackNv( swi_uint8 *paramfieldp, swi_uint16 nNvIndex )
{
    return SwiPackU16(paramfieldp, nNvIndex);
}

/**
 *
 * This function packs NAM index to the CnS object's parameter 
 * field
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                           data will be placed by this function
 * \param nNam[IN]   NAM index to pack.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackNam( swi_uint8 *paramfieldp, swi_uint16 nNam )
{
    return SwiPackU16(paramfieldp, nNam);
}

/**
 *
 * This function packs NV index and a swi_uint16 value to the CnS 
 * object's parameter field
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                           data will be placed by this function
 * \param nNvIndex[IN]   NV index to pack.
 * \param nValue[IN]   swi_uint16 value  to pack.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackNvU16( swi_uint8 *paramfieldp, 
                                swi_uint16 nNvIndex, 
                                swi_uint16 nValue )
{
    return SwiPackNvU16s(paramfieldp, nNvIndex, &nValue, 1);
}

/**
 *
 * This function packs NV index and a byte array to the CnS 
 * object's parameter field
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                           data will be placed by this function
 * \param nNvIndex[IN]   NV index to pack.
 * \param pnValue[IN]   Pointer to storage that contains byte array.
 * \param nNum[IN]   Number of bytes in pnValue to pack.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackNvU8s( swi_uint8 *paramfieldp, 
                                swi_uint16 nNvIndex, 
                                const swi_uint8 *pnValue, 
                                swi_uint16 nNum )
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */
    
    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16( nNvIndex, &localfieldp );
    piputmulti((const swi_uint8 *)pnValue, &localfieldp, nNum);
    
    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs NV index and a swi_uint16 array to the CnS 
 * object's parameter field
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                           data will be placed by this function
 * \param nNvIndex[IN]   NV index to pack.
 * \param pnValue[IN]   Pointer to storage that contains swi_uint16 array.
 * \param nNum[IN]   Number of swi_uint16 in pnValue to pack.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackNvU16s( swi_uint8 *paramfieldp, 
                                 swi_uint16 nNvIndex, 
                                 const swi_uint16 *pnValue, 
                                 swi_uint16 nNum )
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */
    
    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16( nNvIndex, &localfieldp );
    piput16s( pnValue, nNum, &localfieldp );
    
    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs NV index and a swi_uint32 value to the CnS 
 * object's parameter field
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                           data will be placed by this function
 * \param nNvIndex[IN]   NV index to pack.
 * \param nValue[IN]   swi_uint32 value  to pack.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackNvU32( swi_uint8 *paramfieldp, 
                                swi_uint16 nNvIndex, 
                                swi_uint32 nValue )
{
    return SwiPackNvU32s(paramfieldp, nNvIndex, &nValue, 1);
}

/**
 *
 * This function packs NV index and a swi_uint32 array to the CnS 
 * object's parameter field
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                           data will be placed by this function
 * \param nNvIndex[IN]   NV index to pack.
 * \param pnValue[IN]   Pointer to storage that contains swi_uint32 array.
 * \param nNum[IN]   Number of swi_uint32 in pnValue to pack.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackNvU32s( swi_uint8 *paramfieldp, 
                                 swi_uint16 nNvIndex, 
                                 const swi_uint32 *pnValue, 
                                 swi_uint16 nNum )
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */
    
    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16( nNvIndex, &localfieldp );
    piput32s( pnValue, nNum, &localfieldp );
    
    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs NV index, NAM index and a swi_uint16 array to the CnS 
 * object's parameter field
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                           data will be placed by this function
 * \param nNvIndex[IN]   NV index to pack.
 * \param nNamIndex[IN]   NAM index to pack.
 * \param pnValue[IN]   Pointer to storage that contains swi_uint16 array.
 * \param nNum[IN]   Number of swi_uint16 in pnValue to pack.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackNvNamU16s( swi_uint8 *paramfieldp, 
                                    swi_uint16 nNvIndex, 
                                    swi_uint16 nNamIndex, 
                                    const swi_uint16 *pnValue, 
                                    swi_uint16 nNum )
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */
    
    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16( nNvIndex, &localfieldp );
    piput16( nNamIndex, &localfieldp );
    piput16s( pnValue, nNum, &localfieldp );
    
    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs NV index, NAM index and a swi_uint32 array to the CnS 
 * object's parameter field
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                           data will be placed by this function
 * \param nNvIndex[IN]   NV index to pack.
 * \param nNamIndex[IN]   NAM index to pack.
 * \param pnValue[IN]   Pointer to storage that contains swi_uint32 array.
 * \param nNum[IN]   Number of swi_uint32 in pnValue to pack.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackNvNamU32s( swi_uint8 *paramfieldp, 
                                    swi_uint16 nNvIndex, 
                                    swi_uint16 nNamIndex, 
                                    const swi_uint32 *pnValue, 
                                    swi_uint16 nNum )
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */
    
    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16( nNvIndex, &localfieldp );
    piput16( nNamIndex, &localfieldp );
    piput32s( pnValue, nNum, &localfieldp );
    
    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs NV index and NAM index to the CnS 
 * object's parameter field
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                           data will be placed by this function
 * \param nNvIndex[IN]   NV index to pack.
 * \param nNamIndex[IN]   NAM index to pack.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackNvNam( swi_uint8 *paramfieldp, 
                                swi_uint16 nNvIndex, 
                                swi_uint16 nNamIndex )
{
    return SwiPackNvNamU16s(paramfieldp, nNvIndex, nNamIndex, NULL, 0);
}

/**
 *
 * This function packs NV index and two swi_uint16 values to the CnS 
 * object's parameter field
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                           data will be placed by this function
 * \param nNvIndex[IN]   NV index to pack.
 * \param nValue1[IN]   First swi_uint16 value  to pack.
 * \param nValue2[IN]   Second swi_uint16 value  to pack.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackNvU16U16( swi_uint8 *paramfieldp, 
                                   swi_uint16 nNvIndex, 
                                   swi_uint16 nValue1, 
                                   swi_uint16 nValue2 )
{
    swi_uint16 array[] = { nNvIndex, nValue1, nValue2 };
    return SwiPackU16s(paramfieldp, array, 3);
}

/**
 *
 * This function packs string to the CnS object's parameter field
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                           data will be placed by this function
 * \param pszString[IN]   String data.
 * \param szLength[IN]   Strng length.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackString( swi_uint8 *paramfieldp, 
                                 const swi_charp pszString, 
                                 swi_uint16 szLength )
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piputmulti((const swi_uint8 *)pszString, &localfieldp, szLength);


    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs a swi_uint16 array and a string to the CnS object's 
 * parameter field
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                           data will be placed by this function
 * \param pnValue[IN]   Pointer to storage that contains swi_uint16 array.
 * \param nNum[IN]   Number of swi_uint16 in pnValue to pack.
 * \param pszString[IN]   String data to pack.
 * \param szLength[IN]   Strng length.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackU16sString( swi_uint8 *paramfieldp, 
                                     const swi_uint16 *pnValue, 
                                     swi_uint16 nNum, 
                                     const swi_charp pszString, 
                                     swi_uint16 szLength )
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16s( pnValue, nNum, &localfieldp );
    piputmulti((const swi_uint8 *)pszString, &localfieldp, szLength);


    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs NV index and a string to the CnS object's 
 * parameter field
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                           data will be placed by this function
 * \param nNvIndex[IN]   NV index to pack.
 * \param pszString[IN]   String data to pack.
 * \param szLength[IN]   Strng length.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackNvString( swi_uint8 *paramfieldp, 
                                   swi_uint16 nNvIndex, 
                                   const swi_charp pszString, 
                                   swi_uint16 szLength )
{
    return SwiPackU16sString(paramfieldp, &nNvIndex, 1, pszString, szLength);
}

/**
 *
 * This function packs NV index, NAM index and a string to the CnS object's 
 * parameter field
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                           data will be placed by this function
 * \param nNvIndex[IN]   NV index to pack.
 * \param nNamIndex[IN]   NAM index to pack.
 * \param pszString[IN]   String data to pack.
 * \param szLength[IN]   Strng length.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackNvNamString( swi_uint8 *paramfieldp, 
                                      swi_uint16 nNvIndex, 
                                      swi_uint16 nNamIndex, 
                                      const swi_charp pszString, 
                                      swi_uint16 szLength )
{
    swi_uint16 array[] = {nNvIndex, nNamIndex };
    return SwiPackU16sString(paramfieldp, array, 2, pszString, szLength);
}

/**
 *
 * This function packs a swi_uint16 array and a string to the CnS object's 
 * parameter field
 *
 * This function is different from SwiPackU16sString() in that there is a 
 * swi_uint8 type string length before the string in packed data.
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                           data will be placed by this function
 * \param pnValue[IN]   Pointer to storage that contains swi_uint16 array.
 * \param nNum[IN]   Number of swi_uint16 in pnValue to pack.
 * \param pszString[IN]   String data to pack.
 * \param szLength[IN]   Strng length.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackU16sU8LenString( swi_uint8 *paramfieldp, 
                                          const swi_uint16 *pnValue, 
                                          swi_uint16 nNum, 
                                          const swi_charp pszString, 
                                          swi_uint8 szLength )
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    /* if no U16 array, do not pack it. */
    if(pnValue)
    {
        piput16s( pnValue, nNum, &localfieldp );
    }

    /* if no string, do not pack length. */
    if(pszString)
    {
        piputmulti((const swi_uint8 *)&szLength, &localfieldp, 1);
        piputmulti((const swi_uint8 *)pszString, &localfieldp, szLength);
    }

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs a swi_uint16 array and a string to the CnS object's 
 * parameter field
 *
 * This function is different from SwiPackU16sString() in that there is a 
 * swi_uint16 type string length before the string in packed data.
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                           data will be placed by this function
 * \param pnValue[IN]   Pointer to storage that contains swi_uint16 array.
 * \param nNum[IN]   Number of swi_uint16 in pnValue to pack.
 * \param pszString[IN]   String data to pack.
 * \param szLength[IN]   Strng length.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackU16sU16LenString( swi_uint8 *paramfieldp, 
                                           const swi_uint16 *pnValue, 
                                           swi_uint16 nNum, 
                                           const swi_charp pszString, 
                                           swi_uint16 szLength )
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    /* if no U16 array, do not pack it. */
    if(pnValue)
    {
        piput16s( pnValue, nNum, &localfieldp );
    }

    /* if no string, do not pack length. */
    if(pszString)
    {
         piput16s( &szLength, 1, &localfieldp );
        piputmulti((const swi_uint8 *)pszString, &localfieldp, szLength);
    }

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs NV index and a string to the CnS object's 
 * parameter field
 *
 * This function is different from SwiPackNvString() in that there is a
 * swi_uint16 type string length before the string in packed data.
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                           data will be placed by this function
 * \param nNvIndex[IN]   NV index to pack.
 * \param pszString[IN]   String data to pack.
 * \param szLength[IN]   Strng length.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackNvU16LenString( swi_uint8 *paramfieldp, 
                                         swi_uint16 nNvIndex, 
                                         const swi_charp pszString, 
                                         swi_uint16 szLength )
{
    return SwiPackU16sU16LenString(paramfieldp, &nNvIndex, 1, 
            pszString, szLength);
}

/**
 *
 * This function packs NV index, NAM index and string to the CnS object's 
 * parameter field
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                           data will be placed by this function
 * \param nNamIndex[IN]   NAM index to pack.
 * \param pszString[IN]   String data to pack.
 * \param szLength[IN]   Strng length.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackNamU16LenString( swi_uint8 *paramfieldp, 
                                            swi_uint16 nNamIndex, 
                                            const swi_charp pszString, 
                                            swi_uint16 szLength )
{
    return SwiPackU16sU16LenString(paramfieldp, &nNamIndex, 1, pszString, 
            szLength);
}

/**
 *
 * This function packs NV index, NAM index and string to the CnS object's 
 * parameter field
 *
 * This function is different from SwiPackNvNamString() in that there is a
 * swi_uint16 type string length before the string in packed data.
 * 
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                           data will be placed by this function
 * \param nNvIndex[IN]   NV index to pack.
 * \param nNamIndex[IN]   NAM index to pack.
 * \param pszString[IN]   String data to pack.
 * \param szLength[IN]   Strng length.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackNvNamU16LenString( swi_uint8 *paramfieldp, 
                                            swi_uint16 nNvIndex, 
                                            swi_uint16 nNamIndex, 
                                            const swi_charp pszString, 
                                            swi_uint16 szLength )
{
    swi_uint16 array[] = { nNvIndex, nNamIndex };
    return SwiPackU16sU16LenString(paramfieldp, array, 2, pszString, szLength);
}

/**
 *
 * Unpacks a string from the CnS object's parameter field.
 *
 * \param   paramfieldp - Pointer to the CnS parameter field 
 *                        of the CnS object CNS_TECH_HDR_MAC_INDEX.
 * \param   pszString   - Pointer to storage to hold the string unpacked 
 *                        by this function. 
 * \param   szLength    - Length of the string unpacked 
 *
 */
package void SwiUnpackString( swi_uint8 *paramfieldp, 
                             swi_charp pszString, 
                             swi_uint32 szLength )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;
    
    pigetmulti(&parmfp, (swi_uint8 *)pszString, szLength);
}

/**
 *
 * Unpacks a string from the CnS object's parameter field.
 *
 * \param   paramfieldp - Pointer to the CnS parameter field 
 *                        of the CnS object CNS_TECH_HDR_MAC_INDEX.
 * \param   pszString   - Pointer to storage to hold the string unpacked 
 *                        by this function. 
 * \param   szLength    - Length of the string unpacked 
 *
 */
package void SwiUnpackNvString( swi_uint8 *paramfieldp, 
                               swi_charp pszString, 
                               swi_uint32 szLength )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;
    
    piget16(&parmfp);    /* Discard NV item */
    
    pigetmulti(&parmfp, (swi_uint8 *)pszString, szLength);
}

/**
 *
 * Unpacks a string from the CnS object's parameter field.
 *
 * \param   paramfieldp - Pointer to the CnS parameter field 
 *                        of the CnS object CNS_TECH_HDR_MAC_INDEX.
 * \param   pszString   - Pointer to storage to hold the string unpacked 
 *                        by this function. 
 * \param   szLength    - Length of the string unpacked 
 *
 */
package void SwiUnpackNvNamString( swi_uint8 *paramfieldp, 
                                  swi_charp pszString, 
                                  swi_uint32 szLength )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;
    
    piget16(&parmfp);    /* Discard NV item */
    piget16(&parmfp);    /* Discard NAM item */
    
    pigetmulti(&parmfp, (swi_uint8 *)pszString, szLength);
}

/**
 *
 * Unpacks a swi_uint16 array and a string from the CnS object's parameter 
 * field.
 *
 * \param   paramfieldp - Pointer to the CnS parameter field 
 *                        of the CnS object CNS_TECH_HDR_MAC_INDEX.
 * \param   pnValue     - Pointer to storage to hold the swi_uint16 array 
 *                        unpacked by this function. 
 * \param   nNum        - Size of the swi_uint16 array.
 * \param   pszString   - Pointer to storage to hold the string unpacked 
 *                        by this function. 
 * \param   szLength    - Length of the string unpacked 
 *
 */
package void SwiUnpackU16sU16LenString( swi_uint8 *paramfieldp, 
                                       swi_uint16 *pnValue, 
                                       swi_uint16 nNum, 
                                       swi_charp pszString, 
                                       swi_uint32 szLength )
{
    swi_uint16 len;
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;
    
    piget16s(pnValue, nNum, &parmfp);
    
    len = piget16(&parmfp);    /* Length of the string */
    if(len > szLength)
    {
        len = szLength;
    }

    pigetmulti(&parmfp, (swi_uint8 *)pszString, len);
}

/**
 *
 * Unpacks a string from the CnS object's parameter field.
 *
 * This function is different from SwiUnpackNvString() in that there is a 
 * swi_uint16 type string length in front of  the string in the CnS 
 * parameter field.
 *
 * \param   paramfieldp - Pointer to the CnS parameter field 
 *                        of the CnS object CNS_TECH_HDR_MAC_INDEX.
 * \param   pszString   - Pointer to storage to hold the string unpacked 
 *                        by this function. 
 * \param   szLength    - Length of the string unpacked 
 *
 */
package void SwiUnpackNvU16LenString( swi_uint8 *paramfieldp, 
                                     swi_charp pszString, 
                                     swi_uint32 szLength )
{
    swi_uint16 len;
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;
    
    piget16(&parmfp);    /* Discard NV item */
    
    len = piget16(&parmfp);    /* Length of the string */
    if(len > szLength)
    {
        len = szLength;
    }

    pigetmulti(&parmfp, (swi_uint8 *)pszString, len);
}

/**
 *
 * Unpacks a string from the CnS object's parameter field.
 *
 * \param   paramfieldp - Pointer to the CnS parameter field 
 *                        of the CnS object CNS_TECH_HDR_MAC_INDEX.
 * \param   pszString   - Pointer to storage to hold the string unpacked 
 *                        by this function. 
 * \param   szLength    - Length of the string unpacked 
 *
 */
package void SwiUnpackNamString( swi_uint8 *paramfieldp, 
                                swi_charp pszString, 
                                swi_uint32 szLength )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;
    
    piget16(&parmfp);    /* Discard Nam item */
    
    pigetmulti(&parmfp, (swi_uint8 *)pszString, szLength);
}

/**
 *
 * Unpacks a string from the CnS object's parameter field.
 *
 * \param   paramfieldp - Pointer to the CnS parameter field 
 *                        of the CnS object CNS_TECH_HDR_MAC_INDEX.
 * \param   pszString   - Pointer to storage to hold the string unpacked 
 *                        by this function. 
 * \param   szLength    - Length of the string unpacked 
 *
 */
package void SwiUnpackNamU16LenString( swi_uint8 *paramfieldp, 
                                      swi_charp pszString, 
                                      swi_uint32 szLength )
{
    swi_uint16 len;
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;
    
    piget16(&parmfp);    /* Discard Nam item */
    
    len = piget16(&parmfp);    /* Length of the string */
    if(len > szLength)
    {
        len = szLength;
    }

    pigetmulti(&parmfp, (swi_uint8 *)pszString, len);
}

/**
 *
 * Unpacks a string from the CnS object's parameter field.
 *
 * This function is different from SwiUnpackNvNamString() in that there is a 
 * swi_uint16 type string length in front of  the string in the CnS 
 * parameter field.
 *
 * \param   paramfieldp - Pointer to the CnS parameter field 
 *                        of the CnS object CNS_TECH_HDR_MAC_INDEX.
 * \param   pszString   - Pointer to storage to hold the string unpacked 
 *                        by this function. 
 * \param   szLength    - Length of the string unpacked 
 *
 */
package void SwiUnpackNvNamU16LenString( swi_uint8 *paramfieldp, 
                                        swi_charp pszString, 
                                        swi_uint32 szLength )
{
    swi_uint16 len;
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;
    
    piget16(&parmfp);    /* Discard NV item */
    piget16(&parmfp);    /* Discard NAM item */
    
    len = piget16(&parmfp);    /* Length of the string */
    if(len > szLength)
    {
        len = szLength;
    }

    pigetmulti(&parmfp, (swi_uint8 *)pszString, len);
}

/**
 *
 * Unpacks a byte array from the CnS object's parameter field.
 *
 * \param   paramfieldp - Pointer to the CnS parameter field 
 *                        of the CnS object CNS_TECH_HDR_MAC_INDEX.
 * \param   pnValue     - Pointer to storage to hold the byte array 
 *                        unpacked by this function. 
 * \param   nNum        - Size of the byte array. 
 *
 */
package void SwiUnpackNvU8s( swi_uint8 *paramfieldp, 
                            swi_uint8 *pnValue, 
                            swi_uint16 nNum )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    piget16(&parmfp);    /* Discard NV item */
    
    pigetmulti(&parmfp, (swi_uint8 *)pnValue, nNum);
}

/**
 *
 * Unpacks a swi_uint16 array from the CnS object's parameter field.
 *
 * \param   paramfieldp - Pointer to the CnS parameter field 
 *                        of the CnS object CNS_TECH_HDR_MAC_INDEX.
 * \param   pnValue     - Pointer to storage to hold the swi_uint16 array 
 *                        unpacked by this function. 
 * \param   nNum        - Size of the swi_uint16 array. 
 *
 */
package void SwiUnpackNvU16s( swi_uint8 *paramfieldp, 
                             swi_uint16 *pnValue, 
                             swi_uint16 nNum )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    piget16(&parmfp);    /* Discard NV item */
    
    piget16s(pnValue, nNum, &parmfp);
}

/**
 *
 * Unpacks a swi_uint32 array from the CnS object's parameter field.
 *
 * \param   paramfieldp - Pointer to the CnS parameter field 
 *                        of the CnS object CNS_TECH_HDR_MAC_INDEX.
 * \param   pnValue     - Pointer to storage to hold the swi_uint32 array 
 *                        unpacked by this function. 
 * \param   nNum        - Size of the swi_uint32 array. 
 *
 */
package void SwiUnpackNvU32s( swi_uint8 *paramfieldp, 
                             swi_uint32 *pnValue, 
                             swi_uint16 nNum )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    piget16(&parmfp);    /* Discard NV item */
    
    piget32s(pnValue, nNum, &parmfp);
}

/**
 *
 * Unpacks a byte array from the CnS object's parameter field.
 *
 * \param   paramfieldp - Pointer to the CnS parameter field 
 *                        of the CnS object CNS_TECH_HDR_MAC_INDEX.
 * \param   pnValue     - Pointer to storage to hold the byte array 
 *                        unpacked by this function. 
 * \param   nNum        - Size of the byte array. 
 *
 */
package void SwiUnpackNvNamU8s( swi_uint8 *paramfieldp, 
                               swi_uint8 *pnValue, 
                               swi_uint16 nNum )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    piget16(&parmfp);    /* Discard NV item */
    piget16(&parmfp);    /* Discard NAM item */
    
    pigetmulti(&parmfp, (swi_uint8 *)pnValue, nNum );
}

/**
 *
 * Unpacks a swi_uint16 array from the CnS object's parameter field.
 *
 * \param   paramfieldp - Pointer to the CnS parameter field 
 *                        of the CnS object CNS_TECH_HDR_MAC_INDEX.
 * \param   pnValue     - Pointer to storage to hold the swi_uint16 array 
 *                        unpacked by this function. 
 * \param   nNum        - Size of the swi_uint16 array. 
 *
 */
package void SwiUnpackNvNamU16s( swi_uint8 *paramfieldp, 
                                swi_uint16 *pnValue, 
                                swi_uint16 nNum )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    piget16(&parmfp);    /* Discard NV item */
    piget16(&parmfp);    /* Discard NAM item */
    
    piget16s(pnValue, nNum, &parmfp);
}

/**
 *
 * Unpacks a swi_uint32 array from the CnS object's parameter field.
 *
 * \param   paramfieldp - Pointer to the CnS parameter field 
 *                        of the CnS object CNS_TECH_HDR_MAC_INDEX.
 * \param   pnValue     - Pointer to storage to hold the swi_uint32 array 
 *                        unpacked by this function. 
 * \param   nNum        - Size of the swi_uint32 array. 
 *
 */
package void SwiUnpackNvNamU32s( swi_uint8 *paramfieldp, 
                                swi_uint32 *pnValue, 
                                swi_uint16 nNum )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    piget16(&parmfp);    /* Discard NV item */
    piget16(&parmfp);    /* Discard NAM item */
    
    piget32s(pnValue, nNum, &parmfp);
}

/**
 *
 * This function packs the PRL block to the CnS  object's parameter field
 *
 * \param paramfieldp       - Pointer to storage into which the packed 
 *                            data will be placed by this function
 * \param namIndex          - NAM index.
 * \param packetSeqNumber   - Packet sequence No...
 * \param lenByteArray      - Length of PRL segment body  to be sent.
 * \param pSegmentBody      - Point to PRL segment body to be sent.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
local swi_uint32 SwiIntPackPRLBlock(swi_uint8 *paramfieldp, 
                                    swi_uint16 namIndex, 
                                    swi_uint16 packetSeqNumber,
                                    swi_uint16 lenByteArray,
                                    swi_uint8 *pSegmentBody )
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16( namIndex, &localfieldp );
    piput16( packetSeqNumber, &localfieldp );
    piput16( lenByteArray, &localfieldp );
    piputmulti(pSegmentBody, &localfieldp, lenByteArray);

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This is a internal help function that sends one PRL block to modem.
 *
 * \param namIndex[IN]          NAM index.
 * \param packetSeqNumber[IN]   Package sequence No..
 * \param lenByteArray[IN]      Length of buffer.
 * \param pSegmentBody[IN]      Point to PRL block to be sent.
 * \param timeout[IN]           Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntSendPRLBlock (swi_uint16 namIndex, 
                                              swi_uint16 packetSeqNumber,
                                              swi_uint16 lenByteArray,
                                              swi_uint8 *pSegmentBody,
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
    parmlength = SwiIntPackPRLBlock( outparamp, namIndex,
                                                packetSeqNumber,
                                                lenByteArray,
                                                pSegmentBody);
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PRL_WRITE, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    if(SWI_RCODE_OK == resultcode)
    {
        swi_uint16 nResult;
        nResult = piget16(&inparamp);
        if(0 != nResult)
        {
            resultcode = SWI_RCODE_FAILED;
        }
    }
    
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function packs the file body to the CnS  object's parameter field
 *
 * \param paramfieldp           - Pointer to storage into which the packed 
 *                                data will be placed by this function
 * \param pszEFilename          - Efile name.
 * \param szLength              - Length of the EFile name.
 * \param packetSeqNumber       - Ppackage sequence No..
 * \param totalNumberPackets    - Total number of package.
 * \param lenByteArray          - Length of the segment body.
 * \param pSegmentBody          - Point to segment body.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
local swi_uint32 SwiIntPackSendFileBody(swi_uint8 *paramfieldp, 
                                        const swi_charp pszEFilename, 
                                        swi_uint32 szLength,
                                        swi_uint16 packetSeqNumber,
                                        swi_uint16 totalNumberPackets,
                                        swi_uint32 lenByteArray,
                                        swi_uint8 *pSegmentBody )
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piputmulti((const swi_uint8 *)pszEFilename, &localfieldp, szLength);
    localfieldp += LENGTH_EFilename - szLength;
    piput16(packetSeqNumber, &localfieldp);
    piput16(totalNumberPackets, &localfieldp);
    piput32(lenByteArray, &localfieldp);
    piputmulti(pSegmentBody, &localfieldp, lenByteArray);

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This is a internal help function that sends file body to modem.
 *
 * \param pszEFilename[IN]          Efile name.
 * \param packetSeqNumber[IN]       Package sequence No..
 * \param totalNumberPackets[IN]    Total number of package
 * \param lenByteArray[IN]          Length of segment body.
 * \param pSegmentBody[IN]          Point to segment body to be sent.
 * \param timeout[IN]               Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntSendFileBody (const swi_charp pszEFilename, 
                                              swi_uint16 packetSeqNumber,
                                              swi_uint16 totalNumberPackets,
                                              swi_uint32 lenByteArray,
                                              swi_uint8  *pSegmentBody,
                                              swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */
    swi_uint32          szLength;  /* Length of string */ 

    szLength = slstrlen(pszEFilename);
    if(szLength < 1 || szLength > LENGTH_EFilename)
    {
        return SWI_RCODE_INVALID_PAR;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Parameter Field Construction Zone - get a pointer to the 
     * location to fill in the CnS parameter field 
     */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Pack the CnS Parameter field with the input parameters */
    parmlength = SwiIntPackSendFileBody(outparamp, pszEFilename, 
            szLength, packetSeqNumber, totalNumberPackets, lenByteArray, 
            pSegmentBody);

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_FILE_WRITE, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */
                     
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        swi_uint16 nResult, nErrorCode;

        nResult = piget16(&inparamp);
        nErrorCode = piget16(&inparamp);

        if (1 != nResult || 0 != nErrorCode)
        {
            resultcode = SWI_RCODE_FAILED;
        }
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This is a internal help function that retrieves string from modem.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param pszString[OUT]    Buffer to save string retrieved.
 * \param szLength[IN]      Length of buffer.
 * \param nMinSize[IN]      Min size of the string retrieved. If this value
 *                          is 0, this value is ignored.   
 * \param nFixSize[IN]      Expected size of the string retrieved. If this 
 *                          value is 0, this value is ignored.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntGetString(swi_uint16 nObjectID, 
                                          swi_charp pszString,
                                          swi_uint32 szLength, 
                                          swi_uint16 nMinSize, 
                                          swi_uint16 nFixSize, 
                                          swi_uint8 airint, 
                                          swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         paramlength; /* Returned length of the CnS parm field */

    if((nMinSize != 0 && szLength < nMinSize) || 
            (nFixSize != 0 && szLength < nFixSize))
    {
        return SWI_RCODE_INVALID_PAR;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Normally a Pack function call would go here, but this 
     * request has no CnS Parameter field to pack
     */

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  nObjectID, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  airint, 
                                  &inparamp,
                                  &paramlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        if(nFixSize != 0 && paramlength != nFixSize)
        {
            resultcode = SWI_RCODE_UNEXPECTED_RESP;
        }
        else if(paramlength > szLength)
        {
            resultcode = SWI_RCODE_SMALL_BUF;
        }
        else
        {
            slmemset ((char *)pszString, 0, szLength);
            SwiUnpackString( inparamp, pszString, paramlength );
        }
    }
    
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This is a internal help function that retrieves string of certain NV.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param nNvIndex[IN]      NV index.
 * \param pszString[OUT]    Buffer to save string retrieved.
 * \param szLength[IN]      Length of buffer.
 * \param nMinSize[IN]      Min size of the string retrieved. If this value
 *                          is 0, this value is ignored.   
 * \param nFixSize[IN]      Expected size of the string retrieved. If this 
 *                          value is 0, this value is ignored.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntGetNvString(swi_uint16 nObjectID, 
                                            swi_uint16 nNvIndex, 
                                            swi_charp pszString, 
                                            swi_uint32 szLength, 
                                            swi_uint16 nMinSize, 
                                            swi_uint16 nFixSize, 
                                            swi_uint8 airint, 
                                            swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    if((nMinSize != 0 && szLength < nMinSize) || 
            (nFixSize != 0 && szLength < nFixSize))
    {
        return SWI_RCODE_INVALID_PAR;
    }
    
    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Parameter Field Construction Zone - get a pointer to the 
     * location to fill in the CnS parameter field 
     */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Pack the CnS Parameter field with the input parameters */
    parmlength = SwiPackNv( outparamp, nNvIndex );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  nObjectID, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  airint, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        swi_uint32 respstrlen;
        
        respstrlen = parmlength - sizeof(swi_uint16);   /* Minus Nv */
        if(nFixSize != 0 && respstrlen != nFixSize)
        {
            resultcode = SWI_RCODE_UNEXPECTED_RESP;
        }
        else if(respstrlen > szLength)
        {
            resultcode = SWI_RCODE_SMALL_BUF;
        }
        else
        {
            slmemset ((char *)pszString, 0, szLength);
            SwiUnpackNvString( inparamp, pszString, respstrlen );
        }
    }
    
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This is a internal help function that sets unsigned 16bits array and string 
 * to modem.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param pnValue[IN]       Point to 16bits array to be set.
 * \param nNum[IN]          Size of the array.
 * \param pszString[IN]     String to be set.
 * \param nMaxSize[IN]      Max size of the string to set. If this value
 *                          is 0, this value is ignored.   
 * \param nFixSize[IN]      Expected size of the string to set. If this 
 *                          value is 0, this value is ignored.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntSetU16sString(swi_uint16 nObjectID, 
                                              swi_uint16 *pnValue, 
                                              swi_uint16 nNum, 
                                              const swi_charp pszString, 
                                              swi_uint16 nMaxSize, 
                                              swi_uint16 nFixSize, 
                                              swi_uint8 airint, 
                                              swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */
    swi_uint32         szLength;  /* Length of string */ 

    szLength = slstrlen(pszString);
    if((nMaxSize != 0 && szLength > nMaxSize) || (nFixSize != 0 && 
            szLength != nFixSize) || (szLength > LENGTH_NVItemPayloadMax))
    {
        return SWI_RCODE_INVALID_PAR;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Parameter Field Construction Zone - get a pointer to the 
     * location to fill in the CnS parameter field 
     */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Pack the CnS Parameter field with the input parameters */
    parmlength = SwiPackU16sString( outparamp, pnValue, nNum, pszString, 
            szLength );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  nObjectID, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  airint, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */
                     
    /* The response packet for this request contains the same 
     * information we just sent in the request packet. The 
     * 3G SDK "does not care" about payloads in responses to 
     * Set operations and discards the information without 
     * looking at it. Therefore it's OK for us to do the same
     */

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This is a internal help function that sets unsigned 16bits and string 
 * to modem.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param nValue[IN]       16bits value to be set.
 * \param pszString[IN]     String to be set.
 * \param nMaxSize[IN]      Max size of the string to set. If this value
 *                          is 0, this value is ignored.   
 * \param nFixSize[IN]      Expected size of the string to set. If this 
 *                          value is 0, this value is ignored.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntSetU16String(swi_uint16 nObjectID, 
                                             swi_uint16 nValue, 
                                             const swi_charp pszString, 
                                             swi_uint16 nMaxSize, 
                                             swi_uint16 nFixSize, 
                                             swi_uint8 airint, 
                                             swi_uint32 timeout)
{
    return SwiIntSetU16sString(nObjectID, &nValue, 1, pszString, nMaxSize, 
            nFixSize, airint, timeout);
}

/**
 *
 * This is a internal help function that sets unsigned 16bits array and string 
 * to modem.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param pnValue[IN]       Point to 16bits array to be set.
 * \param nNum[IN]          Size of the array.
 * \param pszString[IN]     String to be set.
 * \param nMaxSize[IN]      Max size of the string to set. If this value
 *                          is 0, this value is ignored.   
 * \param nFixSize[IN]      Expected size of the string to set. If this 
 *                          value is 0, this value is ignored.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntSetU16sU8LenString(swi_uint16 nObjectID, 
                                                   swi_uint16 *pnValue, 
                                                   swi_uint16 nNum, 
                                                   const swi_charp pszString, 
                                                   swi_uint16 nMaxSize, 
                                                   swi_uint16 nFixSize, 
                                                   swi_uint8 airint, 
swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */
    swi_uint32         szLength;  /* Length of string */ 

    szLength = slstrlen(pszString);
    if((nMaxSize != 0 && szLength > nMaxSize) || 
            (nFixSize != 0 && szLength != nFixSize) || 
            (szLength > LENGTH_NVItemPayloadMax))
    {
        return SWI_RCODE_INVALID_PAR;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Parameter Field Construction Zone - get a pointer to the 
     * location to fill in the CnS parameter field 
     */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Pack the CnS Parameter field with the input parameters */
    parmlength = SwiPackU16sU8LenString( outparamp, pnValue, nNum, 
            pszString, szLength );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  nObjectID, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  airint, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */
                     
    /* All done, send the result to the caller */
    return resultcode;
}


/**
 *
 * This is a internal help function that sets unsigned 16bits array and string 
 * to modem.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param pnValue[IN]       Point to 16bits array to be set.
 * \param nNum[IN]          Size of the array.
 * \param pszString[IN]     String to be set.
 * \param nMaxSize[IN]      Max size of the string to set. If this value
 *                          is 0, this value is ignored.   
 * \param nFixSize[IN]      Expected size of the string to set. If this 
 *                          value is 0, this value is ignored.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntSetU16sU16LenString(swi_uint16 nObjectID, 
                                                    swi_uint16 *pnValue, 
                                                    swi_uint16 nNum, 
                                                    const swi_charp pszString, 
                                                    swi_uint16 nMaxSize, 
                                                    swi_uint16 nFixSize, 
                                                    swi_uint8 airint, 
                                                    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */
    swi_uint32         szLength;  /* Length of string */ 

    szLength = slstrlen(pszString);
    if((nMaxSize != 0 && szLength > nMaxSize) || 
            (nFixSize != 0 && szLength != nFixSize) || 
            (szLength > LENGTH_NVItemPayloadMax))
    {
        return SWI_RCODE_INVALID_PAR;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Parameter Field Construction Zone - get a pointer to the 
     * location to fill in the CnS parameter field 
     */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Pack the CnS Parameter field with the input parameters */
    parmlength = SwiPackU16sU16LenString( outparamp, pnValue, nNum, 
            pszString, szLength );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  nObjectID, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  airint, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */
                     
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This is a internal help function that sets string of certain NV.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param nNvIndex[IN]      NV index.
 * \param pszString[IN]     String to be set.
 * \param nMaxSize[IN]      Max size of the string to set. If this value
 *                          is 0, this value is ignored.   
 * \param nFixSize[IN]      Expected size of the string to set. If this 
 *                          value is 0, this value is ignored.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntSetNvString(swi_uint16 nObjectID, 
                                            swi_uint16 nNvIndex, 
                                            const swi_charp pszString, 
                                            swi_uint16 nMaxSize, 
                                            swi_uint16 nFixSize, 
                                            swi_uint8 airint, 
                                            swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */
    swi_uint32         szLength;  /* Length of string */ 

    szLength = slstrlen(pszString);
    if((nMaxSize != 0 && szLength > nMaxSize) || 
            (nFixSize != 0 && szLength != nFixSize) || 
            (szLength > LENGTH_NVItemPayloadMax))
    {
        return SWI_RCODE_INVALID_PAR;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Parameter Field Construction Zone - get a pointer to the 
     * location to fill in the CnS parameter field 
     */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Pack the CnS Parameter field with the input parameters */
    parmlength = SwiPackNvString( outparamp, nNvIndex, pszString, szLength );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  nObjectID, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  airint, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */
                     
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This is a internal help function that sets string of certain 
 * NV and NAM.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param nNvIndex[IN]      NV index.
 * \param namIndex[IN]      NAM index.
 * \param pszString[IN]     String to be set.
 * \param nMaxSize[IN]      Max size of the string to set. If this value
 *                          is 0, this value is ignored.   
 * \param nFixSize[IN]      Expected size of the string to set. If this 
 *                          value is 0, this value is ignored.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntSetNvNamString(swi_uint16 nObjectID, 
                                               swi_uint16 nNvIndex, 
                                               swi_uint16 namIndex, 
                                               const swi_charp pszString, 
                                               swi_uint16 nMaxSize, 
                                               swi_uint16 nFixSize, 
                                               swi_uint8 airint, 
                                               swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */
    swi_uint32         szLength;  /* Length of string */ 

    szLength = slstrlen(pszString);
    if((nMaxSize != 0 && szLength > nMaxSize) || 
            (nFixSize != 0 && szLength != nFixSize) || 
            (szLength > LENGTH_NVItemPayloadMax))
    {
        return SWI_RCODE_INVALID_PAR;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Parameter Field Construction Zone - get a pointer to the 
     * location to fill in the CnS parameter field 
     */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Pack the CnS Parameter field with the input parameters */
    parmlength = SwiPackNvNamString( outparamp, nNvIndex, namIndex, 
            pszString, szLength );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  nObjectID, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  airint, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This is a internal help function that sets string of certain NV.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param nNvIndex[IN]      NV index.
 * \param pszString[IN]     String to be set.
 * \param nMaxSize[IN]      Max size of the string to set. If this value
 *                          is 0, this value is ignored.   
 * \param nFixSize[IN]      Expected size of the string to set. If this 
 *                          value is 0, this value is ignored.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntSetNvU16LenString(swi_uint16 nObjectID, 
                                                  swi_uint16 nNvIndex, 
                                                  const swi_charp pszString, 
                                                  swi_uint16 nMaxSize, 
                                                  swi_uint16 nFixSize, 
                                                  swi_uint8 airint, 
                                                  swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */
    swi_uint32         szLength;  /* Length of string */ 

    szLength = slstrlen(pszString);
    if((nMaxSize != 0 && szLength > nMaxSize) || 
            (nFixSize != 0 && szLength != nFixSize) || 
            (szLength > LENGTH_NVItemPayloadMax))
    {
        return SWI_RCODE_INVALID_PAR;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Parameter Field Construction Zone - get a pointer to the 
     * location to fill in the CnS parameter field 
     */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Pack the CnS Parameter field with the input parameters */
    parmlength = SwiPackNvU16LenString( outparamp, nNvIndex, pszString, 
            szLength );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  nObjectID, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  airint, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This is a internal help function that sets string of certain NAM.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param namIndex[IN]      NAM index.
 * \param pszString[IN]     String to be set.
 * \param nMaxSize[IN]      Max size of the string to set. If this value
 *                          is 0, this value is ignored.   
 * \param nFixSize[IN]      Expected size of the string to set. If this 
 *                          value is 0, this value is ignored.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntSetNamU16LenString(swi_uint16 nObjectID, 
                                                     swi_uint16 namIndex, 
                                                     const swi_charp pszString, 
                                                     swi_uint16 nMaxSize, 
                                                     swi_uint16 nFixSize, 
                                                     swi_uint8 airint, 
                                                     swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */
    swi_uint32         szLength;  /* Length of string */ 

    szLength = slstrlen(pszString);
    if((nMaxSize != 0 && szLength > nMaxSize) || 
            (nFixSize != 0 && szLength != nFixSize) || 
            (szLength > LENGTH_NVItemPayloadMax))
    {
        return SWI_RCODE_INVALID_PAR;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Parameter Field Construction Zone - get a pointer to the 
     * location to fill in the CnS parameter field 
     */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Pack the CnS Parameter field with the input parameters */
    parmlength = SwiPackNamU16LenString( outparamp, namIndex, 
            pszString, szLength );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  nObjectID, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  airint, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This is a internal help function that sets string of certain NV and NAM.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param nNvIndex[IN]      NV index.
 * \param namIndex[IN]      NAM index.
 * \param pszString[IN]     String to be set.
 * \param nMaxSize[IN]      Max size of the string to set. If this value
 *                          is 0, this value is ignored.   
 * \param nFixSize[IN]      Expected size of the string to set. If this 
 *                          value is 0, this value is ignored.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntSetNvNamU16LenString(swi_uint16 nObjectID, 
                                                     swi_uint16 nNvIndex, 
                                                     swi_uint16 namIndex, 
                                                     const swi_charp pszString, 
                                                     swi_uint16 nMaxSize, 
                                                     swi_uint16 nFixSize, 
                                                     swi_uint8 airint, 
                                                     swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */
    swi_uint32         szLength;  /* Length of string */ 

    szLength = slstrlen(pszString);
    if((nMaxSize != 0 && szLength > nMaxSize) || 
            (nFixSize != 0 && szLength != nFixSize) || 
            (szLength > LENGTH_NVItemPayloadMax))
    {
        return SWI_RCODE_INVALID_PAR;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Parameter Field Construction Zone - get a pointer to the 
     * location to fill in the CnS parameter field 
     */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Pack the CnS Parameter field with the input parameters */
    parmlength = SwiPackNvNamU16LenString( outparamp, nNvIndex, namIndex, 
            pszString, szLength );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  nObjectID, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  airint, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This is a internal help function that sets unsigned 32bits array of certain 
 * NV.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param nNvIndex[IN]      NV index.
 * \param pnValue[IN]       Point to unsigned 32bits array to be set.
 * \param nNum[IN]          Size of the array.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntSetU32s(swi_uint16 nObjectID, 
                                        swi_uint32 *pnValue, 
                                        swi_uint16 nNum, 
                                        swi_uint8 airint, 
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
    parmlength = SwiPackU32s( outparamp, pnValue, nNum );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  nObjectID, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  airint, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* All done, send the result to the caller */
    return resultcode;
}

package SWI_API SWI_RCODE SwiIntSet(swi_uint16 nObjectID, 
                                    swi_uint8 airint, 
                                    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  nObjectID, 
                                  0, 
                                  timeout, 
                                  CNS_SETREQ,
                                  airint, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This is a internal help function that sets unsigned 8bits array of certain 
 * NV.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param nNvIndex[IN]      NV index.
 * \param pnValue[IN]       Point to unsigned 8bits array to be set.
 * \param nNum[IN]          Size of the array.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntSetU8s(swi_uint16 nObjectID, 
                                       swi_uint8 *pnValue, 
                                       swi_uint16 nNum, 
                                       swi_uint8 airint, 
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
    parmlength = SwiPackU8s( outparamp, pnValue, nNum );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  nObjectID, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  airint, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This is a internal help function that sets unsigned 8bits to modem.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param nValue[IN]        Value to be set.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntSetU8(swi_uint16 nObjectID, 
                                      swi_uint8 nValue, 
                                      swi_uint8 airint, 
                                      swi_uint32 timeout)
{
    return SwiIntSetU8s(nObjectID, &nValue, 1, airint, timeout);
}

/**
 *
 * This is a internal help function that sets unsigned 16bits array of certain 
 * NV.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param nNvIndex[IN]      NV index.
 * \param pnValue[IN]       Point to unsigned 16bits array to be set.
 * \param nNum[IN]          Size of the array.
 * \param pnResult[OUT]     Result of the operation. If this parameter is 
 *                          NULL, this paramtere is ignored.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntSetU16s(swi_uint16 nObjectID, 
                                        swi_uint16 *pnValue, 
                                        swi_uint16 nNum, 
                                        swi_uint16 *pnResult, 
                                        swi_uint8 airint, 
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
    parmlength = SwiPackU16s( outparamp, pnValue, nNum );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  nObjectID, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  airint, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK && NULL != pnResult)
    {
        *pnResult = piget16(&inparamp);
    }
    
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This is a internal help function that sets unsigned 16bits to modem.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param nValue[IN]        Value to be set.
 * \param pnResult[OUT]     Result of the operation. If this parameter is 
 *                          NULL, this paramtere is ignored.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntSetU16(swi_uint16 nObjectID, 
                                       swi_uint16 nValue, 
                                       swi_uint16 *pnResult, 
                                       swi_uint8 airint, 
                                       swi_uint32 timeout)
{
    return SwiIntSetU16s(nObjectID, &nValue, 1, pnResult, airint, timeout);
}

/**
 *
 * This is a internal help function that sets unsigned 8bits of certain NV.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param nNvIndex[IN]      NV index.
 * \param nValue[IN]        Value to be set.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntSetNvU8(swi_uint16 nObjectID, 
                                        swi_uint16 nNvIndex, 
                                        swi_uint8 nValue, 
                                        swi_uint8 airint, 
                                        swi_uint32 timeout)
{
    return SwiIntSetNvU8s(nObjectID, nNvIndex, &nValue, 1, airint, timeout);
}

/**
 *
 * This is a internal help function that sets unsigned 8bits array of certain 
 * NV.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param nNvIndex[IN]      NV index.
 * \param pnValue[IN]       Point to unsigned 8bits array to be set.
 * \param nNum[IN]          Size of the array.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntSetNvU8s(swi_uint16 nObjectID, 
                                         swi_uint16 nNvIndex, 
                                         swi_uint8 *pnValue, 
                                         swi_uint16 nNum, 
                                         swi_uint8 airint, 
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
    parmlength = SwiPackNvU8s( outparamp, nNvIndex, pnValue, nNum );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  nObjectID, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  airint, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This is a internal help function that sets unsigned 16bits array of certain 
 * NV.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param nNvIndex[IN]      NV index.
 * \param pnValue[IN]       Point to unsigned 16bits array to be set.
 * \param nNum[IN]          Size of the array.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntSetNvU16s(swi_uint16 nObjectID, 
                                          swi_uint16 nNvIndex, 
                                          swi_uint16 *pnValue, 
                                          swi_uint16 nNum, 
                                          swi_uint8 airint, 
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
    parmlength = SwiPackNvU16s( outparamp, nNvIndex, pnValue, nNum );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  nObjectID, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  airint, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This is a internal help function that sets unsigned 16bits of certain NV.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param nNvIndex[IN]      NV index.
 * \param nValue[IN]        Value to be set.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntSetNvU16(swi_uint16 nObjectID, 
                                         swi_uint16 nNvIndex, 
                                         swi_uint16 nValue, 
                                         swi_uint8 airint, 
                                         swi_uint32 timeout)
{
    return SwiIntSetNvU16s(nObjectID, nNvIndex, &nValue, 1, airint, timeout);
}

/**
 *
 * This is a internal help function that sets unsigned 16bits array of certain 
 * NV and NAM.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param nNvIndex[IN]      NV index.
 * \param namIndex[IN]      NAM index.
 * \param pnValue[IN]       Point to unsigned 16bits array to be set.
 * \param nNum[IN]          Size of the array.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntSetNvNamU16s(swi_uint16 nObjectID, 
                                             swi_uint16 nNvIndex, 
                                             swi_uint16 namIndex, 
                                             swi_uint16 *pnValue, 
                                             swi_uint16 nNum, 
                                             swi_uint8 airint, 
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
    parmlength = SwiPackNvNamU16s( outparamp, nNvIndex, namIndex, pnValue, 
            nNum );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  nObjectID, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  airint, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This is a internal help function that sets unsigned 16bits of certain 
 * NV and NAM.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param nNvIndex[IN]      NV index.
 * \param namIndex[IN]      NAM index.
 * \param nValue[IN]        Value to be set.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntSetNvNamU16(swi_uint16 nObjectID, 
                                            swi_uint16 nNvIndex, 
                                            swi_uint16 namIndex, 
                                            swi_uint16 nValue, 
                                            swi_uint8 airint, 
                                            swi_uint32 timeout)
{
    return SwiIntSetNvNamU16s(nObjectID, nNvIndex, namIndex, &nValue, 1, 
            airint, timeout);
}

/**
 *
 * This is a internal help function that sets unsigned 32bits of certain 
 * NV and NAM.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param nNvIndex[IN]      NV index.
 * \param namIndex[IN]      NAM index.
 * \param nValue[IN]        Value to be set.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntSetNvNamU32(swi_uint16 nObjectID, 
                                            swi_uint16 nNvIndex, 
                                            swi_uint16 namIndex, 
                                            swi_uint32 nValue, 
                                            swi_uint8 airint, 
                                            swi_uint32 timeout)
{
    return SwiIntSetNvNamU32s(nObjectID, nNvIndex, namIndex, &nValue, 1, 
            airint, timeout);
}

/**
 *
 * This is a internal help function that sets unsigned 32bits array of certain 
 * NV.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param nNvIndex[IN]      NV index.
 * \param pnValue[IN]       Point to unsigned 32bits array to be set.
 * \param nNum[IN]          Size of the array.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntSetNvU32s(swi_uint16 nObjectID, 
                                          swi_uint16 nNvIndex, 
                                          swi_uint32 *pnValue, 
                                          swi_uint16 nNum, 
                                          swi_uint8 airint, 
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
    parmlength = SwiPackNvU32s( outparamp, nNvIndex, pnValue, nNum );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  nObjectID, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  airint, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This is a internal help function that sets unsigned 32bits array of certain 
 * NV and NAM.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param nNvIndex[IN]      NV index.
 * \param namIndex[IN]      NAM index.
 * \param pnValue[IN]       Point to unsigned 32bits array to be set.
 * \param nNum[IN]          Size of the array.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntSetNvNamU32s(swi_uint16 nObjectID, 
                                             swi_uint16 nNvIndex, 
                                             swi_uint16 namIndex, 
                                             swi_uint32 *pnValue, 
                                             swi_uint16 nNum, 
                                             swi_uint8 airint, 
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
    parmlength = SwiPackNvNamU32s( outparamp, nNvIndex, namIndex, pnValue, 
            nNum );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  nObjectID, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  airint, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This is a internal help function that sets unsigned 32bits of certain NV.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param nNvIndex[IN]      NV index.
 * \param nValue[IN]        Value to be set.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntSetNvU32(swi_uint16 nObjectID, 
                                         swi_uint16 nNvIndex, 
                                         swi_uint32 nValue, 
                                         swi_uint8 airint, 
                                         swi_uint32 timeout)
{
    return SwiIntSetNvU32s(nObjectID, nNvIndex, &nValue, 1, airint, timeout);
}

/**
 *
 * This is a internal help function that retrieves string of certain 
 * NV and NAM.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param nNvIndex[IN]      NV index.
 * \param namIndex[IN]      NAM index.
 * \param pszString[OUT]    Buffer to save string retrieved.
 * \param szLength[IN]      Length of buffer.
 * \param nMinSize[IN]      Min size of the string retrieved. If this value
 *                          is 0, this value is ignored.   
 * \param nFixSize[IN]      Expected size of the string retrieved. If this 
 *                          value is 0, this value is ignored.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntGetNvNamString(swi_uint16 nObjectID, 
                                               swi_uint16 nNvIndex, 
                                               swi_uint16 namIndex, 
                                               swi_charp pszString, 
                                               swi_uint32 szLength, 
                                               swi_uint16 nMinSize, 
                                               swi_uint16 nFixSize, 
                                               swi_uint8 airint, 
                                               swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    if((nMinSize != 0 && szLength < nMinSize) || 
            (nFixSize != 0 && szLength < nFixSize))
    {
        return SWI_RCODE_INVALID_PAR;
    }
    
    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Parameter Field Construction Zone - get a pointer to the 
     * location to fill in the CnS parameter field 
     */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Pack the CnS Parameter field with the input parameters */
    parmlength = SwiPackNvNam( outparamp, nNvIndex, namIndex );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  nObjectID, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  airint, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        swi_uint32 respstrlen;
        
        respstrlen = parmlength - sizeof(swi_uint16)*2;   /* Minus Nv and Nam */
        if(nFixSize != 0 && respstrlen != nFixSize)
        {
            resultcode = SWI_RCODE_UNEXPECTED_RESP;
        }
        else if(respstrlen > szLength)
        {
            resultcode = SWI_RCODE_SMALL_BUF;
        }        
        else
        {
            slmemset ((char *)pszString, 0, szLength);
            SwiUnpackNvString( inparamp, pszString, respstrlen );
        }
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This is a internal help function that retrieves string of certain NV.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param nNvIndex[IN]      NV index.
 * \param pszString[OUT]    Buffer to save string retrieved.
 * \param szLength[IN]      Length of buffer.
 * \param nMinSize[IN]      Min size of the string retrieved. If this value
 *                          is 0, this value is ignored.   
 * \param nFixSize[IN]      Expected size of the string retrieved. If this 
 *                          value is 0, this value is ignored.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntGetNvU16LenString(swi_uint16 nObjectID, 
                                                  swi_uint16 nNvIndex, 
                                                  swi_charp pszString, 
                                                  swi_uint32 szLength, 
                                                  swi_uint16 nMinSize, 
                                                  swi_uint16 nFixSize, 
                                                  swi_uint8 airint, 
                                                  swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    if((nMinSize != 0 && szLength < nMinSize) || 
            (nFixSize != 0 && szLength < nFixSize))
    {
        return SWI_RCODE_INVALID_PAR;
    }
    
    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Parameter Field Construction Zone - get a pointer to the 
     * location to fill in the CnS parameter field 
     */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Pack the CnS Parameter field with the input parameters */
    parmlength = SwiPackNv( outparamp, nNvIndex );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  nObjectID, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  airint, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        swi_uint32 respstrlen;
        
        respstrlen = parmlength - sizeof(swi_uint16)*2;   /* Minus Nv and u16 */
        if(nFixSize != 0 && respstrlen != nFixSize)
        {
            resultcode = SWI_RCODE_UNEXPECTED_RESP;
        }
        else if(respstrlen > szLength)
        {
            resultcode = SWI_RCODE_SMALL_BUF;
        }
        else
        {
            slmemset ((char *)pszString, 0, szLength);
            SwiUnpackNvU16LenString( inparamp, pszString, respstrlen );
        }
    }
    
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This is a internal help function that retrieves string of certain NAM.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param namIndex[IN]      NAM index.
 * \param pszString[OUT]    Buffer to save string retrieved.
 * \param szLength[IN]      Length of buffer.
 * \param nMinSize[IN]      Min size of the string retrieved. If this value
 *                          is 0, this value is ignored.   
 * \param nFixSize[IN]      Expected size of the string retrieved. If this 
 *                          value is 0, this value is ignored.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntGetNamString(swi_uint16 nObjectID, 
                                             swi_uint16 namIndex, 
                                             swi_charp pszString, 
                                             swi_uint32 szLength, 
                                             swi_uint16 nMinSize, 
                                             swi_uint16 nFixSize, 
                                             swi_uint8 airint, 
                                             swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    if((nMinSize != 0 && szLength < nMinSize) || 
            (nFixSize != 0 && szLength < nFixSize))
    {
        return SWI_RCODE_INVALID_PAR;
    }
    
    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Parameter Field Construction Zone - get a pointer to the 
     * location to fill in the CnS parameter field 
     */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Pack the CnS Parameter field with the input parameters */
    parmlength = SwiPackNam( outparamp, namIndex );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  nObjectID, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  airint, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        swi_uint32 respstrlen;
        
        respstrlen = parmlength - sizeof(swi_uint16);     /* Minus Nam */
        if(nFixSize != 0 && respstrlen != nFixSize)
        {
            resultcode = SWI_RCODE_UNEXPECTED_RESP;
        }
        else if(respstrlen > szLength)
        {
            resultcode = SWI_RCODE_SMALL_BUF;
        }
        else
        {
            slmemset ((char *)pszString, 0, szLength);
            SwiUnpackNamString( inparamp, pszString, respstrlen );
        }
    }

    /* All done, send the result to the caller */
    return resultcode;
}


/**
 *
 * This is a internal help function that retrieves string of certain NAM.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param namIndex[IN]      NAM index.
 * \param pszString[OUT]    Buffer to save string retrieved.
 * \param szLength[IN]      Length of buffer.
 * \param nMinSize[IN]      Min size of the string retrieved. If this value
 *                          is 0, this value is ignored.   
 * \param nFixSize[IN]      Expected size of the string retrieved. If this 
 *                          value is 0, this value is ignored.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntGetNamU16LenString(swi_uint16 nObjectID, 
                                                   swi_uint16 namIndex, 
                                                   swi_charp pszString, 
                                                   swi_uint32 szLength, 
                                                   swi_uint16 nMinSize, 
                                                   swi_uint16 nFixSize, 
                                                   swi_uint8 airint, 
                                                   swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    if((nMinSize != 0 && szLength < nMinSize) || 
            (nFixSize != 0 && szLength < nFixSize))
    {
        return SWI_RCODE_INVALID_PAR;
    }
    
    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Parameter Field Construction Zone - get a pointer to the 
     * location to fill in the CnS parameter field 
     */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Pack the CnS Parameter field with the input parameters */
    parmlength = SwiPackNam( outparamp, namIndex );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  nObjectID, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  airint, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        swi_uint32 respstrlen;
        
        /* Minus Nam and U16 */
        respstrlen = parmlength - sizeof(swi_uint16)*2;
        
        if(nFixSize != 0 && respstrlen != nFixSize)
        {
            resultcode = SWI_RCODE_UNEXPECTED_RESP;
        }
        else if(respstrlen > szLength)
        {
            resultcode = SWI_RCODE_SMALL_BUF;
        }
        else
        {
            slmemset ((char *)pszString, 0, szLength);
            SwiUnpackNamU16LenString( inparamp, pszString, respstrlen );
        }
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This is a internal help function that retrieves string of certain 
 * NV and NAM.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param nNvIndex[IN]      NV index.
 * \param namIndex[IN]      NAM index.
 * \param pszString[OUT]    Buffer to save string retrieved.
 * \param szLength[IN]      Length of buffer.
 * \param nMinSize[IN]      Min size of the string retrieved. If this value
 *                          is 0, this value is ignored.   
 * \param nFixSize[IN]      Expected size of the string retrieved. If this 
 *                          value is 0, this value is ignored.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntGetNvNamU16LenString(swi_uint16 nObjectID, 
                                                     swi_uint16 nNvIndex, 
                                                     swi_uint16 namIndex, 
                                                     swi_charp pszString, 
                                                     swi_uint32 szLength, 
                                                     swi_uint16 nMinSize, 
                                                     swi_uint16 nFixSize, 
                                                     swi_uint8 airint, 
                                                     swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    if((nMinSize != 0 && szLength < nMinSize) || 
            (nFixSize != 0 && szLength < nFixSize))
    {
        return SWI_RCODE_INVALID_PAR;
    }
    
    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Parameter Field Construction Zone - get a pointer to the 
     * location to fill in the CnS parameter field 
     */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Pack the CnS Parameter field with the input parameters */
    parmlength = SwiPackNvNam( outparamp, nNvIndex, namIndex );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  nObjectID, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  airint, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        swi_uint32 respstrlen;
        
        /* Minus Nv and Nam and U16 */
        respstrlen = parmlength - sizeof(swi_uint16)*3;
        
        if(nFixSize != 0 && respstrlen != nFixSize)
        {
            resultcode = SWI_RCODE_UNEXPECTED_RESP;
        }
        else if(respstrlen > szLength)
        {
            resultcode = SWI_RCODE_SMALL_BUF;
        }
        else
        {
            slmemset ((char *)pszString, 0, szLength);
            SwiUnpackNvNamU16LenString( inparamp, pszString, respstrlen );
        }
    }
    
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This is a internal help function that retrieves unsigned 8bit array of 
 * certain NV.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param nNvIndex[IN]      NV index.
 * \param pnValue[IN]       Pointer to the array value retrieved.
 * \param nNum[IN]          Size of the array.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntGetNvU8s(swi_uint16 nObjectID, 
                                         swi_uint16 nNvIndex, 
                                         swi_uint8 *pnValue, 
                                         swi_uint16 nNum, 
                                         swi_uint8 airint, 
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
    parmlength = SwiPackNv( outparamp, nNvIndex );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  nObjectID, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  airint, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackNvU8s( inparamp, pnValue, nNum );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This is a internal help function that retrieves unsigned 8bit value of 
 * certain NV.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param nNvIndex[IN]      NV index.
 * \param pnValue[IN]       Pointer to the value retrieved.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntGetNvU8(swi_uint16 nObjectID, 
                                        swi_uint16 nNvIndex, 
                                        swi_uint8 *pnValue, 
                                        swi_uint8 airint, 
                                        swi_uint32 timeout)
{
    return SwiIntGetNvU8s(nObjectID, nNvIndex, pnValue, 1, airint, timeout);
}

/**
 *
 * This is a internal help function that retrieves unsigned 8bit array from  
 * modem.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param pnValue[IN]       Pointer to the array value retrieved.
 * \param nNum[IN]          Size of the array.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntGetU8s(swi_uint16 nObjectID, 
                                       swi_uint8 *pnValue, 
                                       swi_uint16 nNum, 
                                       swi_uint8 airint, 
                                       swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  nObjectID, 
                                  0, 
                                  timeout, 
                                  CNS_GETREQ,
                                  airint, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        pigetmulti(&inparamp, (swi_uint8 *)pnValue, nNum);
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This is a internal help function that retrieves unsigned 8bit value from 
 * modem.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param pnValue[IN]       Pointer to the value retrieved.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntGetU8(swi_uint16 nObjectID, 
                                      swi_uint8 *pnValue, 
                                      swi_uint8 airint, 
                                      swi_uint32 timeout)
{
    return SwiIntGetU8s(nObjectID, pnValue, 1, airint, timeout);
}

/**
 *
 * This is a internal help function that retrieves unsigned 16bit array from  
 * modem.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param pnValue[IN]       Pointer to the array value retrieved.
 * \param nNum[IN]          Size of the array.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntGetU16s(swi_uint16 nObjectID, 
                                        swi_uint16 *pnValue, 
                                        swi_uint16 nNum, 
                                        swi_uint8 airint, 
                                        swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  nObjectID, 
                                  0, 
                                  timeout, 
                                  CNS_GETREQ,
                                  airint, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        piget16s(pnValue, nNum, &inparamp);
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This is a internal help function that retrieves unsigned 16bit value from 
 * modem.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param pnValue[IN]       Pointer to the value retrieved.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntGetU16(swi_uint16 nObjectID, 
                                       swi_uint16 *pnValue,
                                       swi_uint8 airint, 
                                       swi_uint32 timeout)
{
    return SwiIntGetU16s(nObjectID, pnValue, 1, airint, timeout);
}

/**
 *
 * This is a internal help function that retrieves unsigned 32bit array from  
 * modem.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param pnValue[IN]       Pointer to the array value retrieved.
 * \param nNum[IN]          Size of the array.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntGetU32s(swi_uint16 nObjectID, 
                                        swi_uint32 *pnValue,
                                        swi_uint16 nNum, 
                                        swi_uint8 airint, 
                                        swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  nObjectID, 
                                  0, 
                                  timeout, 
                                  CNS_GETREQ,
                                  airint, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        piget32s(pnValue, nNum, &inparamp);
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This is a internal help function that retrieves unsigned 32bit value from 
 * modem.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param pnValue[IN]       Pointer to the value retrieved.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntGetU32(swi_uint16 nObjectID, 
                                       swi_uint32 *pnValue,
                                       swi_uint8 airint, 
                                       swi_uint32 timeout)
{
    return SwiIntGetU32s(nObjectID, pnValue, 1, airint, timeout);
}

/**
 *
 * This is a internal help function that retrieves unsigned 16bit array of 
 * certain NV.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param nNvIndex[IN]      NV index.
 * \param pnValue[IN]       Pointer to the array value retrieved.
 * \param nNum[IN]          Size of the array.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntGetNvU16s(swi_uint16 nObjectID, 
                                          swi_uint16 nNvIndex, 
                                          swi_uint16 *pnValue, 
                                          swi_uint16 nNum, 
                                          swi_uint8 airint, 
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
    parmlength = SwiPackNv( outparamp, nNvIndex );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  nObjectID, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  airint, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackNvU16s( inparamp, pnValue, nNum );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This is a internal help function that retrieves unsigned 16bit value of 
 * certain NV.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param nNvIndex[IN]      NV index.
 * \param pnValue[IN]       Pointer to the value retrieved.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntGetNvU16(swi_uint16 nObjectID, 
                                         swi_uint16 nNvIndex, 
                                         swi_uint16 *pnValue, 
                                         swi_uint8 airint, 
                                         swi_uint32 timeout)
{
    return SwiIntGetNvU16s(nObjectID, nNvIndex, pnValue, 1, airint, timeout);
}

/**
 *
 * This is a internal help function that retrieves unsigned 32bit array of 
 * certain NV.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param nNvIndex[IN]      NV index.
 * \param pnValue[IN]       Pointer to the array value retrieved.
 * \param nNum[IN]          Size of the array.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntGetNvU32s(swi_uint16 nObjectID, 
                                          swi_uint16 nNvIndex, 
                                          swi_uint32 *pnValue, 
                                          swi_uint16 nNum, 
                                          swi_uint8 airint, 
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
    parmlength = SwiPackNv( outparamp, nNvIndex );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  nObjectID, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  airint, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackNvU32s( inparamp, pnValue, nNum );
    }
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This is a internal help function that retrieves unsigned 32bit value of 
 * certain NV.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param nNvIndex[IN]      NV index.
 * \param pnValue[IN]       Pointer to the value retrieved.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntGetNvU32(swi_uint16 nObjectID, 
                                         swi_uint16 nNvIndex, 
                                         swi_uint32 *pnValue, 
                                         swi_uint8 airint, 
                                         swi_uint32 timeout)
{
    return SwiIntGetNvU32s(nObjectID, nNvIndex, pnValue, 1, airint, timeout);
}

/**
 *
 * This is a internal help function that retrieves unsigned 8bit array of 
 * certain NV and NAM.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param nNvIndex[IN]      NV index.
 * \param namIndex[IN]      NAM index.
 * \param pnValue[IN]       Pointer to the array value retrieved.
 * \param nNum[IN]          Size of the array.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntGetNvNamU8s(swi_uint16 nObjectID, 
                                            swi_uint16 nNvIndex, 
                                            swi_uint16 namIndex, 
                                            swi_uint8 *pnValue, 
                                            swi_uint16 nNum, 
                                            swi_uint8 airint, 
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
    parmlength = SwiPackNvNam( outparamp, nNvIndex, namIndex );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  nObjectID, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  airint, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackNvNamU8s( inparamp, pnValue, nNum );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This is a internal help function that retrieves unsigned 16bit array of 
 * certain NV and NAM.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param nNvIndex[IN]      NV index.
 * \param namIndex[IN]      NAM index.
 * \param pnValue[IN]       Pointer to the array value retrieved.
 * \param nNum[IN]          Size of the array.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntGetNvNamU16s(swi_uint16 nObjectID, 
                                             swi_uint16 nNvIndex, 
                                             swi_uint16 namIndex, 
                                             swi_uint16 *pnValue, 
                                             swi_uint16 nNum, 
                                             swi_uint8 airint, 
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
    parmlength = SwiPackNvNam( outparamp, nNvIndex, namIndex );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  nObjectID, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  airint, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackNvNamU16s( inparamp, pnValue, nNum );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This is a internal help function that retrieves unsigned 16bit value of 
 * certain NV and NAM.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param nNvIndex[IN]      NV index.
 * \param namIndex[IN]      NAM index.
 * \param pnValue[IN]       Pointer to the value retrieved.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntGetNvNamU16(swi_uint16 nObjectID, 
                                            swi_uint16 nNvIndex, 
                                            swi_uint16 namIndex, 
                                            swi_uint16 *pnValue, 
                                            swi_uint8 airint, 
                                            swi_uint32 timeout)
{
    return SwiIntGetNvNamU16s(nObjectID, nNvIndex, namIndex, pnValue, 1, 
            airint, timeout);
}

/**
 *
 * This is a internal help function that retrieves unsigned 32bit array of 
 * certain NV and NAM.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param nNvIndex[IN]      NV index.
 * \param namIndex[IN]      NAM index.
 * \param pnValue[IN]       Pointer to the array value retrieved.
 * \param nNum[IN]          Size of the array.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntGetNvNamU32s(swi_uint16 nObjectID, 
                                             swi_uint16 nNvIndex, 
                                             swi_uint16 namIndex, 
                                             swi_uint32 *pnValue, 
                                             swi_uint16 nNum, 
                                             swi_uint8 airint, 
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
    parmlength = SwiPackNvNam( outparamp, nNvIndex, namIndex );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  nObjectID, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  airint, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackNvNamU32s( inparamp, pnValue, nNum );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This is a internal help function that retrieves unsigned 32bit value of 
 * certain NV and NAM.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param nNvIndex[IN]      NV index.
 * \param namIndex[IN]      NAM index.
 * \param pnValue[IN]       Pointer to the value retrieved.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntGetNvNamU32(swi_uint16 nObjectID, 
                                            swi_uint16 nNvIndex, 
                                            swi_uint16 namIndex, 
                                            swi_uint32 *pnValue, 
                                            swi_uint8 airint, 
                                            swi_uint32 timeout)
{
    return SwiIntGetNvNamU32s(nObjectID, nNvIndex, namIndex, pnValue, 1, 
        airint, timeout);
}

/**
 *
 * This is a internal help function that sets string to modem.
 *
 * \param nObjectID[IN]     CnS object ID.
 * \param pszString[IN]     String to be set.
 * \param pnResult[OUT]      Result of the operation. If this parameter is 
 *                          NULL, this paramtere is ignored.
 * \param nMaxSize[IN]      Max size of the string to set. If this value
 *                          is 0, this value is ignored.   
 * \param nFixSize[IN]      Expected size of the string to set. If this 
 *                          value is 0, this value is ignored.
 * \param airint[IN]        Air interface ID. See enum swi_amairiftypes.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
package SWI_API SWI_RCODE SwiIntSetString(swi_uint16 nObjectID, 
                                          const swi_charp pszString, 
                                          swi_uint16 *pnResult, 
                                          swi_uint16 nMaxSize, 
                                          swi_uint16 nFixSize, 
                                          swi_uint8 airint, 
                                          swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */
    swi_uint32         szLength;  /* Length of string */ 

    szLength = slstrlen(pszString);
    if((nMaxSize != 0 && szLength > nMaxSize) || 
            (nFixSize != 0 && szLength != nFixSize) || 
            (szLength > LENGTH_NVItemPayloadMax))
    {
        return SWI_RCODE_INVALID_PAR;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Parameter Field Construction Zone - get a pointer to the 
     * location to fill in the CnS parameter field 
     */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Pack the CnS Parameter field with the input parameters */
    parmlength = SwiPackString( outparamp, pszString, szLength );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  nObjectID, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  airint, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK && NULL != pnResult)
    {
        *pnResult = piget16(&inparamp);
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/*
 * $Log: SwiIntUtil.c,v $
 */
