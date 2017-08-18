/*
 *  $Id: aptestnotify.c,v 1.25 2010/06/10 00:27:06 epasheva Exp $
 *
 *  Filename: aptestnotify.c
 *
 *  Purpose:  Contains functions for testing notifications.
 *
 * Copyright: © 2008 Sierra Wireless, Inc., all rights reserved
 *
 */

/* Linux definitions */
#include <stdio.h>
#include <stdlib.h>

/* include files */
#include "SwiDataTypes.h"
#include "SwiApiNotify.h"
#include "sl/sludefs.h"
#include "apidefs.h"
#include "apudefs.h"
#include "aptestdefs.h"

#define CBFPSZ 3
SWI_CALLBACK_EX *apcbfp[CBFPSZ];
int apmaxused = 0;

/*************
 *
 * Name:    aptestnotify
 *
 * Purpose: This structure contains notification handling information
 *          for a single notification type.
 *             
 * Members: eNotifytype  The notification type whose information is stored
 *                       in this instance of this structure.
 *          notifystring A string that will be printed to the screen to 
 *                       identify this notification to the user.
 *          printfp      A pointer to a function that will print the 
 *                       information contained in this notification to the 
 *                       screen.
 * Notes:   none
 *
 **************/
struct aptestnotify
{
    enum SWI_TYPE_Notify     eNotifytype;
    char                     notifystring[30];
    void (*printfp)(struct SwiNotifyVariant *sVariantp);
};

/*************
 *
 * Name:    aptcmnotify
 *
 * Purpose: This table contains all notifications that are tested
 *          by the AP test application.  Adding a notification
 *          to this function will cause it to be enabled and
 *          its print function to be called if it is received.
 *
 *************/
local struct aptestnotify aptcmnotify[] = 
{
    /* Notify Variant,          Notify String        Notify Print Func */
    {SWI_NOTIFY_FwDownload,     "FirmwareDownload",  apfwdownloadprint },
    {SWI_NOTIFY_AirServerChange,"AirServerChange",   apairchangeprint },
    {SWI_NOTIFY_NvStore,        "NV Store",          apnvstoreprint },
    {SWI_NOTIFY_NVUpdateFromFile,"NV update",        apnvupdateprint },
    {SWI_NOTIFY_RadioPower,     "Radio Power",       apradiopowerprint},
    {SWI_NOTIFY_RadioTempState, "Radio Temperature", apradiotempprint},
    {SWI_NOTIFY_RadioBattery,   "Radio Battery",     apradiobatteryprint},
    {SWI_NOTIFY_RadioBand,      "Current Radio Band",apcurradiobandprint}, 
    {SWI_NOTIFY_DeviceTime,     "Device Time",       apcmdevicetimeprint}
};

/*************
 *
 * Name:    aptgsmnotify
 *
 * Purpose: This table contains all notifications that are tested
 *          by the AP test application.  Adding a notification
 *          to this function will cause it to be enabled and
 *          its print function to be called if it is received.
 *
 *************/
local struct aptestnotify aptgsmnotify[] = 
{
    /* Notify Variant,            Notify String    Notify Print Func */
    {SWI_NOTIFY_Rssi,             "RSSI",             aprssiprint },
    /*{SWI_NOTIFY_GsmCellInfo,      "Gsm Cell Info",    apgsmcellprint }, */
    {SWI_NOTIFY_ByteCounter,      "Byte Counter",     apbytecntprint },
    {SWI_NOTIFY_SimStatusExp,     "Sim Status",       apsimstatusprint },
    {SWI_NOTIFY_GsmProfileChange, "Gsm Profile Change", apgsmprofileprint },
    {SWI_NOTIFY_PlmnReady,        "Plmn Ready",       apgsmplmnreadyprint },
    {SWI_NOTIFY_PlmnAvailable,    "Plmn Available",   apgsmplmnavailprint },
    {SWI_NOTIFY_RegistrationExp,  "GSM Registration", apgsmregistrprint}, 
    {SWI_NOTIFY_ModemDisabled,    "Modem Disabled",   apgsmmodemdisabledprint},
    {SWI_NOTIFY_PlmnMode,         "PLMN Mode",        apgsmplmnmodeprint},
    {SWI_NOTIFY_ServiceIcon,      "Service Icon",     apgsmsrviconprint},
    {SWI_NOTIFY_NetworkStatus,    "Network Status",   apgsmnetstatprint},
    {SWI_NOTIFY_PktSessionCall,   "Pkt Sess Act'n",   apgsmnetsessact},
    {SWI_NOTIFY_PhonebookEntry,   "Phonebook Entry",  apgsmphbkentry},
    {SWI_NOTIFY_PhonebookReady,   "Phonebook Ready",  apgsmphbkready},
    {SWI_NOTIFY_LbsPaWarmBegin,   "LBS Warm Begin",   aplbswarmbeginprint},
    {SWI_NOTIFY_LbsPaWarmStatus,  "LBS Warm Status",  aplbswarmstatusprint},
    {SWI_NOTIFY_LbsPaWarmDone,    "LBS Warm Done",    aplbswarmdoneprint},
    {SWI_NOTIFY_LbsNiReq,         "LBS NI Request",   aplbsnirequestprint},
    {SWI_NOTIFY_STKCommand,       "STK Command",      apstkcmdnotifprint},
    {SWI_NOTIFY_AudioProfile,     "Audio Profile",    apaudioprofileprint},
    {SWI_NOTIFY_MissedCall,       "Missed Call",      apgsmmissedcallprint},
    {SWI_NOTIFY_CallProgress,     "Call Exp Progress", apgsmcallexpprogressprint},
    {SWI_NOTIFY_CallForwarded,    "Call Forwarded",   apgsmcallforwardedprint},
    {SWI_NOTIFY_CallStatus,       "Call Status",      apgsmcallstatusprint},
    {SWI_NOTIFY_CFIcon,           "Display CF Icon",   apgsmdisplaycficonprint},
    {SWI_NOTIFY_SSPasswordStatus, "SS Password Status", apgsmsspasswordstatusprint},
    
    {SWI_NOTIFY_SSRespOK,         "SS OK Response",    apgsmssrespokprint},
    {SWI_NOTIFY_SSRespFwdInfo,    "SS Fwd Info Response", apgsmssfwdinforspprint},
    {SWI_NOTIFY_SSRespCallBar,    "SS Call Bar Response", apgsmsscallbarrspprint},
    {SWI_NOTIFY_SSRespInfoData,   "SS Info Data Rsp",  apgsmssinfodatarspprint},
    {SWI_NOTIFY_SSRespStatus,     "SS Status Response", apgsmssstatusrspprint},
    {SWI_NOTIFY_SSRespFwdNum,     "SS Fwd Num Response", apgsmssfwdnumrspprint},
    {SWI_NOTIFY_SSRespBSList,     "SS BS List Response", apgsmssbslistrspprint},
    {SWI_NOTIFY_SSRespFwdList,    "SS Fwd List Response", apgsmssfwdlistrspprint},
    {SWI_NOTIFY_SSRespRaw,        "SS Raw Response",   apgsmssrawrspprint},
    {SWI_NOTIFY_SSRespPassword,   "SS Password Response", apgsmsspasswordstatusprint},
    {SWI_NOTIFY_SSRespClir,       "SS Clir Response",  apgsmssclirrspprint},
    {SWI_NOTIFY_SSRespError,      "SS Error Response", apgsmsserrorrspprint},
    {SWI_NOTIFY_SSRespReject,     "SS Reject Response", apgsmssrejectrspprint},
    {SWI_NOTIFY_SSRespCancel,     "SS Cancel Response", apgsmsscancelrspprint},
    {SWI_NOTIFY_SSRespRelease,    "SS Release Response", apgsmssreleaserspprint},
    {SWI_NOTIFY_SSNotify,         "SS Notify",         apgsmssnotifyprint},
    {SWI_NOTIFY_VoiceMessage,     "Voice Message",     apgsmvoicemessageprint},
};

/*************
 *
 * Name:    aptcdmanotify
 *
 * Purpose: This table contains all notifications that are tested
 *          by the AP test application.  Adding a notification
 *          to this function will cause it to be enabled and
 *          its print function to be called if it is received.
 *
 *************/
local struct aptestnotify aptcdmanotify[] = 
{
    /* Notify Variant,        Notify String    Notify Print Func */
    {SWI_NOTIFY_Rssi,         "RSSI",          aprssiprint },
};

/* number of notifications in the above tables */
local int cmnotifymax = sizeof(aptcmnotify)/sizeof(struct aptestnotify);
local int gsmnotifymax = sizeof(aptgsmnotify)/sizeof(struct aptestnotify);
local int cdmanotifymax = sizeof(aptcdmanotify)/sizeof(struct aptestnotify);

/*************
 *
 * Name:    apsimstatusmap - SIM Status Mapping Table
 *
 * Purpose: This table maps Sim Status enum returned in SWI_NOTIFY_SimStatusExp
 *          to a string that can be printed to the user.
 *
 *************/
char apsimstatusmap[][20] = 
  {{"SIM Okay"}, 
   {"SIM not inserted" }, 
   {"SIM removed"}, 
   {"SIM init failure"}, 
   {"SIM locked"}, 
   {"PIN1 blocked"}, 
   {"PIN2 blocked"}, 
   {"PIN1 rejeced"}, 
   {"PIN2 rejected"}, 
   {"MEP locked"},
   {"Nework rejected"}};

/*************
 *
 * Name:    apreqop - Required Operation Mapping Table
 *
 * Purpose: This table maps Required Operation enum returned in 
 *          SWI_NOTIFY_SimStatusExp to a string that can be printed to 
 *          the user.
 *
 *************/
char apreqop[][20] = 
  {{"No Op"}, 
   {"Enter PIN"}, 
   {"Enter PIN2"}, 
   {"Enter PUK"}, 
   {"Enter PUK2"}, 
   {"Enter MEP code"}};

/*************
 *
 * Name:    apprevop - Previous Operation Mapping Table
 *
 * Purpose: This table maps Previous Operation enum returned in 
 *          SWI_NOTIFY_SimStatusExp to a string that can be printed to 
 *          the user.
 *
 *************/
char apprevop[][20] = 
   {{"No prev op"}, 
    {"Change PIN"}, 
    {"Change PIN2"}, 
    {"OBSOLETE"}, 
    {"Verify PIN"}, 
    {"Verify PIN2"},
    {"Verify PUK"}, 
    {"Verify PUK2"}, 
    {"MEP unlock"}, 
    {"Enable PIN"}, 
    {"Disable PIN"}};

/*************
 *
 * Name:    apprevrslt - Previous Result Mapping Table
 *
 * Purpose: This table maps Previous Result enum returned in 
 *          SWI_NOTIFY_SimStatusExp to a string that can be printed to 
 *          the user.
 *
 *************/
char apprevrslt[][20] = 
    {{"SUCCESS"}, 
     {"FAILED"}, 
     {"FAILED-PIN disabled"}};

/*************
 *
 * Name:    appchvtype - Password Type Mapping Table
 *
 * Purpose: This table maps Password Type enum returned in 
 *          SWI_NOTIFY_SimStatusExp to a string that can be printed to 
 *          the user.
 *
 *************/
char apchvtype[][20] = 
    {{"PIN"}, 
     {"PIN2"}, 
     {"PUK"}, 
     {"PUK2"}}; 

/*************
 *
 * Name:    aplbsnitypesmap- NI request ID type Mapping Table
 *
 * Purpose: This table maps TYPE_LbsNiReqClientName and  TYPE_LbsNiReqId
 *          enums returned in SWI_NOTIFY_LbsNiReq to a string that can be 
 *          printed to the user.
 *
 *************/
char aplbsnitypesmap[][20] = 
    {{"UNKNOWN"}, 
     {"Logical Name"}, 
     {"MSISDN"}, 
     {"Email Addr"}, 
     {"URL"}, 
     {"SIP URL"}, 
     {"IMS public identity"}, 
     {"MIN"}, 
     {"MDN"}};

/*************
 *
 * Name:    apfwdownstatemap- Firmware Download state Mapping Table
 *
 * Purpose: This table maps enum SWI_NOTIF_FW_TYPE returned in \
 *          SWI_NOTIFY_FwDownload to a string that can be 
 *          printed to the user.
 *
 *************/
char apfwdownstatemap[][20] = 
    {{"Init"}, 
     {"Init Error"}, 
     {"Soft Reset"}, 
     {"Soft Reset Error"}, 
     {"Start Request"}, 
     {"Start Request Error"}, 
     {"File Error"}, 
     {"Cont Request"}, 
     {"Cont Req Error"},
     {"End Request"},
     {"End Req Error"},
     {"Flash Program"},
     {"Flash Prog Error"},
     {"Launch Fragment"},
     {"Launch Frag Error"},
     {"Reset"},
     {"Reset Error"},
     {"Download Done"}};
 
 
 /*************
 *
 * Name:    apnetstatusmap - service status mapping table
 *
 * Purpose: This table maps service status enum returned in 
 *          struct  SWI_STRUCT_GsmSystemStatus. 
 *
 *************/
char apnetstatusmap[][20] = 
  {{"Normal"}, 
   {"Emergency" }, 
   {"NoService" }, 
   {"AccessDifficulty" }, 
   {"ForbiddenPLMN" }, 
   {"ForbiddenArea" }, 
   {"ForbiddenRoaming" }, 
   {"IllegalMobStation" }, 
   {"IllegalMobEquip" }, 
   {"UnknownIMSI" }, 
   {"AuthFailure"},
   {"PRSFailed"}};

 /*************
 *
 * Name:    apssclirsettingmap - Supplementary Service CLIR Setting 
 *                               mapping table
 *
 * Purpose: This table maps enum returned in 
 *          struct SWI_STRUCT_SS_RespClir. 
 *
 *************/
char apssclirsettingmap[][20] = 
   {{"Permanent"},
    {"TempCallIdRestrict"},
    {"TempCallIdAllow"}};

 /*************
 *
 * Name:    apsserrortypemap - Supplementary Service Error Type mapping 
 *                             table
 *
 * Purpose: This table maps enum returned in 
 *          struct SWI_STRUCT_SS_RespErrorc. 
 *
 *************/
char apsserrortypemap[][20] = 
   {{"NoInfo"},
    {"CallBarring"},
    {"Status"},
    {"SubsViolate"},
    {"AbsentSubs"},
    {"SystemFail"},
    {"PasswdRegFail"},
    {"Incompatible"}};

 /*************
 *
 * Name:    apsscallbarmap - Supplementary Service Car Barring Status mapping table
 *
 * Purpose: This table maps enum returned in 
 *          struct SWI_STRUCT_SS_RespError. 
 *
 *************/
char apsscallbarmap[][20] = 
   {{"Active"},
    {"OperatorBarring"}};

 /*************
 *
 * Name:    apsssystemfailmap - Supplementary Service System fail mapping table
 *
 * Purpose: This table maps enum returned in 
 *          struct SWI_STRUCT_SS_RespError. 
 *
 *************/
char apsssystemfailmap[][20] = 
   {{"Plmn"},
    {"Hlr"},
    {"Vlr"},
    {"Pvlr"},
    {"ControlMsc"},
    {"VisitMsc"},
    {"Eir"},
    {"Rss"}};

 /*************
 *
 * Name:    apsspasswdregfailmap - Supplementary Service Password Fail 
 *                                 mapping table
 *
 * Purpose: This table maps enum returned in 
 *          struct SWI_STRUCT_SS_RespError. 
 *
 *************/
char apsspasswdregfailmap[][20] = 
   {{"Undetermined"},
    {"InvalidFormat"},
    {"NewMismatch"}};

 /*************
 *
 * Name:    apsproblemtypemap - Supplementary Service Problem Type 
 *                              mapping table
 *
 * Purpose: This table maps enum returned in 
 *          struct SWI_STRUCT_SS_RespReject. 
 *
 *************/
char apsproblemtypemap[][20] = 
   {{"General"},
    {"Invoke"},
    {"ReturnResult"},
    {"ReturnError"}};

 /*************
 *
 * Name:    apsscancelreasonmap - Supplementary Service Cancel Reasons 
 *                                mapping table
 *
 * Purpose: This table maps enum returned in 
 *          struct SWI_STRUCT_SS_RespCancel. 
 *
 *************/
char apsscancelreasonmap[][20] = 
   {{"NoPipes"},
    {"OngoingTrans"},
    {"MnssReject"},
    {"NoControl"},
    {"InvalidId"}};

 /*************
 *
 * Name:    apsspasswordguidancemap - Supplementary Service Password 
 *                                    Guidance mapping table
 *
 * Purpose: This table maps enum returned in 
 *          struct SWI_STRUCT_SS_PasswordStatus. 
 *
 *************/
char apsspasswordguidancemap[][20] = 
   {{"Password"},
    {"NewPassword"},
    {"NewPasswordAgain"},
    {"BadPassword"},
    {"BadPasswordFormat"}};

 /*************
 *
 * Name:    apssbasicservgrptypemap - Supplementary Service Basic Service 
 *                                    Group Type mapping table
 *
 * Purpose: This table maps enum returned in 
 *          struct SWI_STRUCT_SS_Request, 
 *          struct STRUCT_SS_FwdInfo, 
 *          struct STRUCT_SS_CALL_BAR, 
 *          struct STRUCT_SS_BasicServiceGroup, 
 *          struct IncompatibilityReason. 
 *
 *************/
char apssbasicservgrptypemap[][20] = 
   {{"TeleServices"},
    {"BearerServices"}};

 /*************
 *
 * Name:    apumtscallcontrolmap - the actions that can be performed on 
 *                                 a call mapping table
 *
 * Purpose: This table maps enum returned in 
 *          struct SWI_STRUCT_CALL_UMTS_Control. 
 *
 *************/
char apumtscallcontrolmap[][30] = 
   {{"RelAllHeld"},
    {"RelAllActAccHeldWait"},
    {"ReleaseCallX"},
    {"HoldAllActAccHeldWait"},
    {"HoldAllActExceptCallX"},
    {"AddHeldCall"},
    {"ConTwoCallsAndDiscon"},
    {"DisconnectAll"}};

 /*************
 *
 * Name:    apgsmcallstatemap - all possible call states mapping table
 *
 * Purpose: This table maps enum returned in 
 *          struct SWI_STRUCT_CALL_State. 
 *
 *************/
char apgsmcallstatemap[][20] = 
{
    {"Null"},
    {"Connecting"},
    {"Queued"},
    {"Waiting"},
    {"Alerting"},
    {"Connected"},
    {"Disconnecting"}};

 /*************
 *
 * Name:    apcallprogressmap - call progress states mapping table
 *
 * Purpose: This table maps enum returned in  
 *          struct SWI_STRUCT_CALL_EXP_Progress. 
 *
 *************/
char apcallprogressmap[][20] = 
   {{"Failed"},
    {"Connecting"},
    {"Queued"},
    {"Waiting"},
    {"Incoming"},
    {"Ringing"},
    {"Connected"},
    {"Disconnecting"},
    {"Disconnected"},
    {"Forwarded"}};

 /*************
 *
 * Name:    apgsmcallerrormap - call error states mapping table
 *
 * Purpose: This table maps enum returned in  
 *          struct SWI_STRUCT_CALL_EXP_Progress. 
 *
 *************/
char apgsmcallerrormap[][20] = 
   {{"OK"},
    {"SubsciberBusy"},
    {"Congestions"},
    {"RadioPathUnavail"},
    {"Error"},
    {"NumberUnobtain"},
    {"AuthFailure"},
    {"NoNumber"},
    {"BckgndCallCtrlFail"},
    {"BckgndMMFail"},
    {"ChannelBusy"},
    {"FDNMismatch"},
    {"BCFail"},
    {"EmergencyOnly"},
    {"ACMExceeded"},
    {"HoldError"},
    {"BusyProcReq"},
    {"ChannelUnavail"},
    {"OutgoingBarred"},
    {"NumberBlacklisted"},
    {"BlackListFull"},
    {"RedialNotTimedOut"},
    {"ModifyFail"},
    {"NotInControl"},
    {"NoCauseCode"},
    {"CallBarred"}};

 /*************
 *
 * Name:    apcallpresentationmap - call presentation settings mapping 
 *                                  table
 *
 * Purpose: This table maps enum returned in 
 *          struct SWI_STRUCT_CALL_EXP_Progress. 
 *
 *************/
char apcallpresentationmap[][20] = 
   {{"Number"},
    {"Restricted"},
    {"NoAvail"}};

 /*************
 *
 * Name:    apcallscreeningmap - call screening settings mapping table
 *
 * Purpose: This table maps enum returned in 
 *          struct SWI_STRUCT_CALL_EXP_Progress. 
 *
 *************/
char apcallscreeningmap[][20] = 
   {{"NotScreened"},
    {"Passed"},
    {"Failed"},
    {"Network"}};

 /*************
 *
 * Name:    apgsmcalltypemap - call types mapping table
 *
 * Purpose: This table maps enum returned in 
 *          struct SWI_STRUCT_CALL_EXP_Progress. 
 *
 *************/
char apgsmcalltypemap[][20] = 
   {{"VoiceCallLine1"},
    {"VoiceCallLine2"},
    {"CSD"},
    {"FAX"}};

 /*************
 *
 * Name:    apcallvalidmap - call information states mapping table
 *
 * Purpose: This table maps enum returned in  
 *          struct SWI_STRUCT_CALL_EXP_Progress. 
 *
 *************/
char apcallvalidmap[][20] = 
   {{"NoInfo"},
    {"InfoValid"}};

 /*************
 *
 * Name:    apcallrestrictmap - call restriction settings mapping table
 *
 * Purpose: This table maps enum returned in 
 *          struct SWI_STRUCT_CLIR_Setting, 
 *          struct SWI_STRUCT_CALL_Create. 
 *
 *************/
char apcallrestrictmap[][20] = 
   {{"Default"},
    {"Allowed"},
    {"Restricted"},
    {"Unknown"}};

 /*************
 *
 * Name:    aptonelengthmap - tone lengths used for DTMF over-dial 
 *                            digits mapping table
 *
 * Purpose: This table maps enum returned in 
 *          struct SWI_STRUCT_NVToneStore. 
 *
 *************/
char aptonelengthmap[][20] = 
   {{"Short"},
    {"Medium"},
    {"Long"},
    {"UserDefined"}};

 /*************
 *
 * Name:    apaudioprofilemap - audio profiles mapping table
 *
 * Purpose: This table maps enum returned in 
 *          struct SWI_STRUCT_AudioProfile. 
 *
 *************/
char apaudioprofilemap[][20] = 
   {{"Handset"},
    {"Headset"},
    {"SpeakerPhone"},
    {"CarKit"},
    {"TtyFullAudio"},
    {"TtyTalkAudio"},
    {"TtyHearAudio"}};

 /*************
 *
 * Name:    apaudiogenmap - audio generator values mapping table
 *
 * Purpose: This table maps enum returned in  
 *          struct SWI_STRUCT_AudioProfile. 
 *
 *************/
char apaudiogenmap[][20] = 
   {{"Voice"},
    {"KeyBeep"},
    {"Midi"}};

 /*************
 *
 * Name:    apaudioresultmap - audio action result mapping table
 *
 * Purpose: This table maps enum returned in 
 *          struct SWI_STRUCT_AudioProfile. 
 *
 *************/
char apaudioresultmap[][20] = 
   {{"Success"},
    {"InvalidSetting"}};

/* Functions */
/**************
 *
 * Name:     aprssiprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_Rssi.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void aprssiprint( struct SwiNotifyVariant *sVariantp)
{
    printf("Modem RSSI:    -%ddBm\n", -(int)(sVariantp->_Rssi.iRssi));
    printf("Modem BER:     %x\n", (unsigned int)sVariantp->_Rssi.iBer );
}

/**************
 *
 * Name:     apbytecntprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_ByteCounter.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apbytecntprint( struct SwiNotifyVariant *sVariantp)
{
    printf("Modem tx bytes: %d\n", (int)sVariantp->_ByteCounter.TransmitBytes); 
    printf("Modem rx bytes: %d\n", (int)sVariantp->_ByteCounter.ReceiveBytes);
}

/**************
 *
 * Name:     apgsmcellprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_GsmCellInfo.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apgsmcellprint( struct SwiNotifyVariant *sVariantp)
{
    /* prints selected fields for testing */
    printf("Gsm Cell Info valid?: %d\n", (int)sVariantp->_GsmCellInfo.bValid ); 
    printf("Gsm Cell Info Mcc: %d\n", (int)sVariantp->_GsmCellInfo.nMcc ); 
    printf("Gsm Cell Info Mnc: %d\n",(int)sVariantp->_GsmCellInfo.nMnc ); 
    printf("Gsm Cell Info CellId: %d\n",(int)sVariantp->_GsmCellInfo.nCellId ); 
    printf("Gsm Cell Info Bsic: %d\n",(int)sVariantp->_GsmCellInfo.nBsic ); 
    printf("Gsm Cell Info NetColorCode: %d\n", 
            (int)sVariantp->_GsmCellInfo.nNetColorCode ); 
    printf("Number of valid neighboring cells: %d\n", 
            (int)sVariantp->_GsmCellInfo.nNumValidNeighborCells );
}

/**************
 *
 * Name:     apgsmprofileprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_GsmProfileChange.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apgsmprofileprint( struct SwiNotifyVariant *sVariantp)
{
    /* prints selected fields for testing */
    printf("Gsm Profile Change: %d\n", 
        (int)sVariantp->_GsmProfileChange.nProfileId ); 
    printf("Gsm Profile Change Type: %d\n", 
        (int)sVariantp->_GsmProfileChange.eType ); 
}

/**************
 *
 * Name:     apgsmplmnreadyprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_PlmnReady.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apgsmplmnreadyprint( struct SwiNotifyVariant *sVariantp)
{
    /* prints selected fields for testing */
    printf("Plmn Ready: %d\n", 
        (int)sVariantp->_PlmnReady.bReady ); 
}

/**************
 *
 * Name:     apgsmplmnavailprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_PlmnAvailable.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apgsmplmnavailprint( struct SwiNotifyVariant *sVariantp)
{
    /* prints selected fields for testing */
    printf("PLMN Available: %d\n", 
        (int)sVariantp->_PlmnAvailable.bAvailable ); 
}

/**************
 *
 * Name:     apgsmregistrprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_RegistrationExp.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apgsmregistrprint( struct SwiNotifyVariant *sVariantp)
{
    /* prints selected fields for testing */
    printf("Registration Indication: "); 
    if (sVariantp->_Registration.szSPN[0] != 0)
        printf(" Service Provider: %s\n", sVariantp->_Registration.szSPN);
    
    if(sVariantp->_Registration.szPLMN[0] != 0)
        printf(" Network Name; %s\n", sVariantp->_Registration.szPLMN);
    
    if(sVariantp->_Registration.szAbvPLMN[0] != 0)
        printf(" Network Name; %s\n", sVariantp->_Registration.szAbvPLMN);  

    printf("\n"); 
}

/**************
 *
 * Name:     apgsmplmnmodeprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_PlmnMode.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apgsmplmnmodeprint( struct SwiNotifyVariant *sVariantp)
{
    /* prints selected fields for testing  */
    printf("PLMN Mode Change Request Result: %d\n",
        (int)sVariantp->_PlmnMode.eResult ); 

    switch (sVariantp->_PlmnMode.eResult)
    {
        case SWI_PLMNRESULT_Success:
            printf("Success\n");
        break;
        case SWI_PLMNRESULT_AttemptFailed:
            printf("Attempt Failed\n");
        break;
        case SWI_PLMNRESULT_InProgress:
            printf("InProgress\n");
        break;
        case SWI_PLMNRESULT_GeneralFail:
            printf("GeneralFail\n");
        break;
        case SWI_PLMNRESULT_InternalErr:
            printf("Internal Error\n");
        break;
        case SWI_PLMNRESULT_InvalidSelection:
            printf("Invalid Selection\n");
        break;
        default:
        break;
    }
}

/**************
 *
 * Name:     apgsmsrviconprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_ServiceIcon.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apgsmsrviconprint( struct SwiNotifyVariant *sVariantp)
{
    /* obj id 0x1006; prints selected fields for testing */
    printf("Service Type: %d\n", 
        (int)sVariantp->_ServiceIcon.eService); 
}

/**************
 *
 * Name:     apgsmnetstatprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_NetworkStatus.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apgsmnetstatprint( struct SwiNotifyVariant *sVariantp)
{
    /* obj id 0x1000; prints selected fields for testing */
    printf("Network Service Status: %s\n", apnetstatusmap[
        sVariantp->_NetworkStatus.eServiceStatus]); 
}

/**************
 *
 * Name:     apgsmnetsessact
 *
 * Purpose:  Prints the contents of the SWI_NOTIFY_PktSessionCall
 *           notification variant. This notification is generated 
 *           in response to a call to SwiActivateProfile(), once
 *           the outcome of the activation attempt is known
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apgsmnetsessact( struct SwiNotifyVariant *sVariantp)
{
    struct SWI_STRUCT_PktSessionCall *pPktSes;

    pPktSes = &sVariantp->_PktSessionCall;

    /* obj id 0x7004 */
    printf("GSM Profile Activation Notification\n");
    printf("===================================\n");

    /* Print out the contents of the variant structure */
    printf("ProfileID:                     %d\n", (int)pPktSes->nProfileId );
    printf("Activation Status:             %d\n",
          (int)pPktSes->bPacketSessionActive);
    printf("Negotiated QoS:                %d\n", (int)pPktSes->bValidQos);
    printf("Negotiated Precedence:         %d\n", (int)pPktSes->ePrec);
    printf("Negotiated Delay:              %d\n", (int)pPktSes->eDelay);
    printf("Negotiated Reliability:        %d\n", (int)pPktSes->eReli );
    printf("Negotiated Peak:               %d\n", (int)pPktSes->ePeak );
    printf("Negotiated Mean:               %d\n", (int)pPktSes->eMean );
    printf("Negotiated Traffic Class:      %d\n", (int)pPktSes->eTrafficClass);
    printf("Negotiated Max Bitrate UL:     %d\n", (int)pPktSes->eMaxBitrateUl);
    printf("Negotiated Max Bitrate DL:     %d\n", (int)pPktSes->eMaxBitrateDl);
    printf("Negotiated Guaran'd Bitrate UL %d\n", (int)pPktSes->eGuarBitrateUl);
    printf("Negotiated Guaran'd Bitrate DL %d\n", (int)pPktSes->eGuarBitrateDl);
    printf("Negotiated Delivery Order:     %d\n", (int)pPktSes->eDelOrder);
    printf("Negotiated Delivery Err SDU:   %d\n", (int)pPktSes->eDelErrSdu);
    printf("Negotiated Max SDU Size:       %d\n", (int)pPktSes->nMaxSdu);
    printf("Negotiated SDU Error Ratio:    %d\n", (int)pPktSes->eSduErrRatio);
    printf("Negotiated Residual BERatio:   %d\n", (int)pPktSes->eResBitErrRatio);
    printf("Negotiated Transfer Delay:     %d\n", (int)pPktSes->nTransDelay);
    printf("Negotiated Traffic Hndl Prio:  %d\n", (int)pPktSes->eTraffHandPri);
    printf("Negotiated Source Stats Desc:  %d\n", (int)pPktSes->eSrcStatDesc);
    printf("Negotiated Signalling Ind'n    %d\n", (int)pPktSes->eSignInd);
    printf("Inactivity Reason Code:        %04x\n",
           (unsigned int)pPktSes->eInactivityReason);
}

/**************
 *
 * Name:     apgsmmodemdisabledprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_ModemDisabled.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apgsmmodemdisabledprint( struct SwiNotifyVariant *sVariantp)
{
    /* obj id 0x1011; prints selected fields for testing */
    printf("Modem Disabled Status: %s\n", 
        (sVariantp->_ModemDisabled.bModemDisabled?"Enabled":"Disabled")); 
}


/**************
 *
 * Name:     apsimstatusprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_SimStatusExp.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apsimstatusprint( struct SwiNotifyVariant *sVariantp)
{
    printf("SIM Status: %s\n", 
           apsimstatusmap[sVariantp->_SimStatusExp.eStatus]);
    printf("  Required op:     %s\n", 
           apreqop[sVariantp->_SimStatusExp.eRequiredOperation]);
    printf("  Previous op:     %s\n",  
           apprevop[sVariantp->_SimStatusExp.ePreviousOperation]);
    printf("  Previous result: %s\n", 
           apprevrslt[sVariantp->_SimStatusExp.ePreviousActionResult]);
    printf("  Retry Info present:  %d\n", 
           (int)sVariantp->_SimStatusExp.bRetryInformationPresent);
    printf("    Password type:     %s\n", 
           apchvtype[sVariantp->_SimStatusExp.eRetryInformation]);
    printf("    Retries remaining: %d\n", 
           (int)sVariantp->_SimStatusExp.nNumberRetries);
}

/**************
 *
 * Name:     apnvstoreprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_NvStore.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apnvstoreprint( struct SwiNotifyVariant *sVariantp)
{
    printf("NV Store Status:   %d\n", (int)sVariantp->_NvStore.bSuccess);
    printf("  Operation:       %d\n", (int)sVariantp->_NvStore.eOperation);
    printf("  Category:        %d\n", (int)sVariantp->_NvStore.eCategory);
    printf("  num Stored:      %d\n", (int)sVariantp->_NvStore.nStored);
    printf("  num Skipped:     %d\n", (int)sVariantp->_NvStore.nSkipped);
    printf("  num Zero Length: %d\n", (int)sVariantp->_NvStore.nZeroLength);
}

/**************
 *
 * Name:     apnvupdateprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_NVUpdateFromFile.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apnvupdateprint( struct SwiNotifyVariant *sVariantp)
{
    printf("NV Update Status: %d\n", (int)sVariantp->_NVUpdateFromFile.nStatus);
    printf("  Items Updated: %d\n",(int)sVariantp->_NVUpdateFromFile.nItemsUpdated);
    printf("  Items Sipped:  %d\n",(int)sVariantp->_NVUpdateFromFile.nItemsSipped);
}

/**************
 *
 * Name:     apcurradiobandprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_RadioBand.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apcurradiobandprint( struct SwiNotifyVariant *sVariantp)
{
    int i;

    printf("  Number of current bands: %d\n",
           (int)sVariantp->_RadioBand.nNumBands);
    for(i = 0; i < MAX_BAND_LIST; i++)
    {
        if (i == sVariantp->_RadioBand.nNumBands)
            break; 
        printf("   %d-Radio Type:   %d  Current Band: %d\n", i,
               (int)sVariantp->_RadioBand.sList[i].eType, 
               (int)sVariantp->_RadioBand.sList[i].eBand);
    }
}
/**************
 *
 * Name:     apradiopowerprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_RadioPower.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apradiopowerprint( struct SwiNotifyVariant *sVariantp)
{
    printf("Radio Power State:    %d\n", (int)sVariantp->_RadioPower.eMode);
    printf("  State Reason:       %d\n",
           (int)sVariantp->_RadioPower.nLowPowerReason);
    printf("  Power Down Request: %d\n",
           (int)sVariantp->_RadioPower.bModuleReqPowerDown);
    printf("  Request Reason:     %d\n",
           (int)sVariantp->_RadioPower.eRequestReason);
}
/**************
 *
 * Name:     apradiotempprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_RadioTempState.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apradiotempprint( struct SwiNotifyVariant *sVariantp)
{
    printf("Radio Temperature State:   %d\n",
           (int)sVariantp->_RadioTempState.eState);
    printf("  Current Temperature:     %d\n",
           (int)sVariantp->_RadioTempState.nCurrent);
}
/**************
 *
 * Name:     apradiobatteryprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_RadioBattery.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apradiobatteryprint( struct SwiNotifyVariant *sVariantp)
{
    printf("Radio Battery  State: %d\n", (int)sVariantp->_RadioBattery.eState);
    printf("  Current Voltage:    %d\n",(int)sVariantp->_RadioBattery.nCurrent);
}
/**************
 *
 * Name:     aplbswarmbeginprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_LbsPaWarmBegin.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void aplbswarmbeginprint( struct SwiNotifyVariant *sVariantp)
{
    printf("LBS Warm Processing Begins!\n");
}
 
/**************
 *
 * Name:     aplbswarmstatusprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_LbsPaWarmStatus.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void aplbswarmstatusprint( struct SwiNotifyVariant *sVariantp)
{
    printf ("\nLBS PA Keep Warm Status:\n");
    printf ("    Keep Warm Processing Enabled: %s\n", (sVariantp->_LbsPaWarmStatus.bEnabled?"TRUE":"FALSE"));
    printf ("    Currently Warm:               %s\n", (sVariantp->_LbsPaWarmStatus.eStatus?"TRUE":"FALSE"));

}
 
/**************
 *
 * Name:     aplbswarmdoneprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_LbsPaWarmDone.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void aplbswarmdoneprint( struct SwiNotifyVariant *sVariantp)
{
    printf("LBS Warm Processing Finished!\n");
}
 
/**************
 *
 * Name:     aplbsnirequestprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_LbsNiReq.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void aplbsnirequestprint( struct SwiNotifyVariant *sVariantp)
{
    printf("NI request information:\n");
    printf("   NI Id: %d\n", (int)sVariantp->_LbsNiReq.nNiId);
    printf("   Timeout: %d\n", (int)sVariantp->_LbsNiReq.nTimeout);
    printf("   Notification type: %s\n", (sVariantp->_LbsNiReq.eNotifType?"User response required":"Notify only"));
    printf("   Requestor type: %s\n", aplbsnitypesmap[sVariantp->_LbsNiReq.eReqIdType]);
    printf("   Requestor ID: %s\n", &sVariantp->_LbsNiReq.szReqId[0]);
    printf("   Client type: %s\n", aplbsnitypesmap[sVariantp->_LbsNiReq.eClientName]);
    printf("   Client name: %s\n", &sVariantp->_LbsNiReq.szClientName[0]);
}
/**************
 *
 * Name:     apcmdevicetimeprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_ModemDisabled.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apcmdevicetimeprint( struct SwiNotifyVariant *sVariantp)
{
    /* obj id 0x101B; prints selected fields for testing */
    printf("Device Time: \n");
    printf("  Year     : %d\n", (int)sVariantp->_DeviceTime.nYear); 
    printf("  Month    : %d\n", (int)sVariantp->_DeviceTime.eMonth);
    printf("  Day      : %d\n", (int)sVariantp->_DeviceTime.nDay);
    printf("  Weekday  : %d\n", (int)sVariantp->_DeviceTime.eWeekDay);
    printf("  Hour     : %d\n", (int)sVariantp->_DeviceTime.nHour);
    printf("  Minute   : %d\n", (int)sVariantp->_DeviceTime.nMinute);
    printf("  Second   : %d\n", (int)sVariantp->_DeviceTime.nSecond);
    printf("  Time Zone: %d\n", (int)sVariantp->_DeviceTime.nTimeZone);
    printf("  Daylight Savings: %d\n",
           (int)sVariantp->_DeviceTime.eDaylightSavings);
}
/**************
 *
 * Name:     apfwdownloadprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_FwDownload.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apfwdownloadprint( struct SwiNotifyVariant *sVariantp)
{
    printf("Firmware Download Update....\n");
    printf("    %s\n", apfwdownstatemap[sVariantp->_FwDownload.State]);
}
/**************
 *
 * Name:     apairchangeprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_AirServerChange notification.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apairchangeprint( struct SwiNotifyVariant *sVariantp )
{
    /* Pointers to variant structures for better readability */
    struct SWI_STRUCT_AirServerChange *aschgp;
    struct SWI_STRUCT_AirServer       *asp;

    /* Initialize the local pointers */
    aschgp = &sVariantp->_AirServerChange;
    asp = &aschgp->AirServer;

    /* Display the contents of the AirServerChange Notification */
    printf("AirServer Change Notification: \n");
    printf("  Device Type:               %d\n", (int)asp->eDevice );
    printf("  Client MAC:        %d:%d:%d:%d:%d:%d\n", 
           (int)asp->ClientMac[0], (int)asp->ClientMac[1], 
           (int)asp->ClientMac[2], 
           (int)asp->ClientMac[3], 
           (int)asp->ClientMac[4],
           (int)asp->ClientMac[5] );
    printf("  Server MAC:        %d:%d:%d:%d:%d:%d\n", 
           (int)asp->ServerMac[0], 
           (int)asp->ServerMac[1],
           (int)asp->ServerMac[2], 
           (int)asp->ServerMac[3], 
           (int)asp->ServerMac[4], 
           (int)asp->ServerMac[5] );
    printf("  Device Present:              %d\n", (int)aschgp->bInsert );
}

/**************
 *
 * Name:     apgsmphbkentry
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_PhonebookEntry notification.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apgsmphbkentry( struct SwiNotifyVariant *sVariantp )
{
    struct SWI_STRUCT_PHONEBK_Entry *phep;
    int i;
    
    /* Initialize the local pointers */
    phep = &sVariantp->_PhonebookEntry;
    
    /* Display the contents of the Notification */
    if (phep->nID != 0)
    {
        printf("Phonebook Entry Notification: \n");
        printf("  Index:   %d\n", (int)phep->nID );
        printf("  Name:    %s\n", phep->szEntryName);
        for (i = 0; i < phep->nPhoneNumberLength; i++)
        {
            printf( "%d", (int)phep->byPhoneNumber[i]);
        }
        printf ("\n");
    }
    else
        printf ("Notif: Id 0 received\n");
}

/**************
 *
 * Name:     apgsmphbkready
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_PhonebookReady notification.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apgsmphbkready( struct SwiNotifyVariant *sVariantp )
{
    struct SWI_STRUCT_PhonebookReady *phep;

    /* Initialize the local pointers */
    phep = &sVariantp->_PhonebookReady;
    
    /* Display the contents of the Notification */
    printf("Phonebook Ready Notification: \n");
    printf("  Status:   %s\n", phep->bReady ? "Ready":"Not Ready" );

}

/**************
 *
 * Name:     apstkcmdnotifprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_STKCommand notification.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apstkcmdnotifprint( struct SwiNotifyVariant *sVariantp )
{
    struct SWI_STRUCT_STK_Notif_Command *sstknotifp;

    /* Initialize the local pointers */
    sstknotifp = &sVariantp->_Notif_STKCommand;
    
    /* Display the contents of the Notification */
    printf("STK Command Notification: \n");
    printf("  handle %d\n", (int)sstknotifp->nSTK_Handle);
    printf("  timeout %d\n", (int)sstknotifp->nTimeout);
    printf("  Command %d\n", (int)sstknotifp->eSTKCommand);
    printf("  Operation %d\n", (int)sstknotifp->eOperation);
    printf("  Text type  %d\n", (int)sstknotifp->eTextType);
    printf("  Min Input length %d\n", (int)sstknotifp->nMinLengthOfInputExpected);
    printf("  Max Input length %d\n", (int)sstknotifp->nMaxLengthOfInputExpected);
    printf("  echo %s\n", sstknotifp->bEcho ? "True" : "False");
    printf("  Key type %d\n", sstknotifp->eKeyType);
    printf("  # menu items %d\n", (int)sstknotifp->cntItemsInMenu);
    printf("  priority %d\n", (int)sstknotifp->ePriority);
}


/**************
 *
 * Name:     apaudioprofileprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_AudioProfile notification.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apaudioprofileprint( struct SwiNotifyVariant *sVariantp )
{
    struct SWI_STRUCT_AudioProfile *phep;

    /* Initialize the local pointers */
    phep = &sVariantp->_AudioProfile;
    
    /* Display the contents of the Notification */
    printf("\n");
    printf("Audio Profile Notification: \n");
    printf("Current audio profile setting: %s\n", apaudioprofilemap[phep->eCurrent]);
    printf("Ear Piece Mute Status: %s\n", 
        phep->bEarPieceMute ? "Muted":"Unmuted");
    printf("Mic Mute Status: %s\n", phep->bMicMute ? "Muted":"Unmuted");
    printf("Volume level: %d\n", (int)phep->eVolLevel);
    printf("\n");
}

/**************
 *
 * Name:     apgsmmissedcallprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_MissedCall notification.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apgsmmissedcallprint( struct SwiNotifyVariant *sVariantp )
{
    struct SWI_STRUCT_MissedCall *phep;

    /* Initialize the local pointers */
    phep = &sVariantp->_MissedCall;
    
    /* Display the contents of the Notification */
    printf("\n");
    printf("Missed Call Indication Notification: \n");
    printf("Missed Call Count: %d\n", (int)phep->nCount);
    printf("\n");
}

/**************
 *
 * Name:     apgsmcallexpprogressprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_CallProgress notification.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apgsmcallexpprogressprint( struct SwiNotifyVariant *sVariantp )
{
    struct SWI_STRUCT_CALL_EXP_Progress *phep;
    int i;

    /* Initialize the local pointers */
    phep = &sVariantp->_CallProgress;
    
    /* Display the contents of the Notification */
    printf("\n");
    printf("Call Progress Notification: \n");
    printf("Call ID: %d\n", (int)phep->CallID);
    printf("Call Progress State: %s\n", apcallprogressmap[phep->eCallProgress]);
    printf("Call Error Code: %s\n", apgsmcallerrormap[phep->eCallError]);
    printf("Phone Number Information Valid: %s\n", apcallvalidmap[phep->eCallValid]);
    printf("Call Presentation Setting: %s\n", 
        apcallpresentationmap[phep->eCallPresentation]);
    printf("Call Screening Setting: %s\n", apcallscreeningmap[phep->eCallScreening]);
    printf("International Phone Number Flag: %d\n", (int)phep->bInternational);
    printf("Phone Number Length: %d\n", (int)phep->nNumberLength);
    printf("Phone Number: ");
    for (i = 0; i < phep->nNumberLength; i++)
    {
        printf("%d", (int)phep->bcdPhone[i]);
    }
    printf("\n");
    printf("Call Type: %s\n", apgsmcalltypemap[phep->eCallType]);
    printf("Caller Name: %s\n", (char *) &phep->szCallerName[LENGTH_CallerName]);
    printf("\n");
}

/**************
 *
 * Name:     apgsmcallforwardedprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_CallProgress notification.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apgsmcallforwardedprint( struct SwiNotifyVariant *sVariantp )
{
    struct SWI_STRUCT_CallForwarded *phep;

    /* Initialize the local pointers */
    phep = &sVariantp->_CallForwarded;
    
    /* Display the contents of the Notification */
    printf("\n");
    printf("Call Forwarded Notification: \n");
    printf("Call ID: %d\n", (int)phep->nCallID);
    printf("Notification Present: %s\n", phep->bNotificationStatusPresent ? "True":"False");
    printf("Notification Status: %d\n", (int)phep->nNotificationStatus);
    printf("\n");
}

/**************
 *
 * Name:     apgsmcallstatusprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_CallProgress notification.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apgsmcallstatusprint( struct SwiNotifyVariant *sVariantp )
{
    struct SWI_STRUCT_CALL_Status *phep;
    int i;

    /* Initialize the local pointers */
    phep = &sVariantp->_CallStatus;
    
    /* Display the contents of the Notification */
    printf("\n");
    printf("Call Status Notification: \n");
    printf("Total Number of Calls: %d\n", (int)phep->nCallSum);
    printf("Number of outgoing calls: %d\n", (int)phep->nCallOutgoing);
    printf("Number of Held Calls: %d\n", (int)phep->nCallHeld);
    printf("Number of Call Waiting: %d\n", (int)phep->nCallWaiting);
    printf("Number of Call Queued: %d\n", (int)phep->nCallQueued);
    printf("Number of Multiparty Calls: %d\n", (int)phep->nCallMultiparty);
    printf("Number of Calls in List: %d\n", (int)phep->nNumberCalls);
    
    for (i = 0; i < phep->nNumberCalls; i++)
    {
        printf("\n");
        printf("Call Identifier: %d", (int)phep->CallList[i].CallID);
        printf("Call State: %s", apgsmcallstatemap[phep->CallList[i].eCallState]);
        printf("Call Info: %d", (int)phep->CallList[i].CallInfo);
        printf("\n");
    }
    printf("\n");
}

/**************
 *
 * Name:     apgsmdisplaycficonprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_CFIcon notification.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apgsmdisplaycficonprint( struct SwiNotifyVariant *sVariantp )
{
    struct SWI_STRUCT_CFIcon *phep;

    /* Initialize the local pointers */
    phep = &sVariantp->_CFIcon;
    
    /* Display the contents of the Notification */
    printf("\n");
    printf("Display Call Forward Icon Notification: \n");
    printf("Display CF icon flag: %s\n", phep->bIconOn ? "True":"False");
    
    printf("\n");
}

/**************
 *
 * Name:     apgsmsspasswordstatusprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_SSPasswordStatus notification.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apgsmsspasswordstatusprint( struct SwiNotifyVariant *sVariantp )
{
    struct SWI_STRUCT_SS_PasswordStatus *phep;

    /* Initialize the local pointers */
    phep = &sVariantp->_SSPasswordStatus;
    
    /* Display the contents of the Notification */
    printf("\n");
    printf("Display SS Password Status Notification: \n");
    printf("Object Version: %d\n", (int)phep->nVersion);
    printf("Supplementary Service Handle: %d\n", (int)phep->nHandle);
    printf("SS Password Guidance: %d\n", (int)phep->ePasswordGuidance);
    
    printf("\n");
}

/**************
 *
 * Name:     apgsmssrespokprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_SSRespOK notification.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apgsmssrespokprint( struct SwiNotifyVariant *sVariantp )
{
    struct SWI_STRUCT_SS_RespOK *phep;

    /* Initialize the local pointers */
    phep = &sVariantp->_SSRespOK;
    
    /* Display the contents of the Notification */
    printf("\n");
    printf("Display SS Response OK Notification: \n");
    printf("Object Version: %d\n", (int)phep->nVersion);
    printf("Supplementary Service Handle: %d\n", (int)phep->nHandle);
    
    printf("\n");
}

/**************
 *
 * Name:     apgsmssfwdinforspprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_SSRespFwdInfo notification.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apgsmssfwdinforspprint( struct SwiNotifyVariant *sVariantp )
{
    struct SWI_STRUCT_SS_RespFwdInfo *phep;
    int i;
    int j;
    
    /* Initialize the local pointers */
    phep = &sVariantp->_SSRespFwdInfo;
    
    /* Display the contents of the Notification */
    printf("\n");
    printf("Display SS Forwarding Information Response Notification: \n");
    printf("Object Version: %d\n", (int)phep->nVersion);
    printf("Supplementary Service Handle: %d\n", (int)phep->nHandle);
    printf("Supplementary Service Code Present: %s\n", 
        phep->bServiceCodePresent ? "True":"False");
    printf("Supplementary Service Code: %d\n", (int)phep->eServiceCode);
    printf("Number of Forwarding Info Entries: %d\n", (int)phep->nNumEntries);
    for (i = 0; i < phep->nNumEntries; i++)
    {
        printf("\n");
        printf("Basic Service Group Info Present: %s\n", 
            phep->sList[i].bBasicServGrpInfoPresent ? "True":"False");
        printf("Basic Service Group Type: %s\n", 
            apssbasicservgrptypemap[phep->sList[i].eBasicServGrpType]);
        printf("Basic Service Group Code: %d\n", 
            (int)phep->sList[i].eBasicServGrpCode);
        printf("Service Status Info Present: %s\n", 
            phep->sList[i].bServStatusInfoPresent ? "True":"False");
        printf("Service Status: %d\n", 
            (int)phep->sList[i].eServStatus);
        printf("International Number Flag: %s\n", 
            phep->sList[i].bInternational ? "True":"False");
        printf("Length of Forwarding Number: %d\n", 
            (int)phep->sList[i].nLengthFwdNum);
        printf("Forwarding Number: ");
        for (j = 0; j < phep->sList[i].nLengthFwdNum; j++)
        {
            printf("%d", (int)phep->sList[i].byFwdNum[j]);
        }
        printf("\n");
        printf("Forwarding Options Info Present: %s\n", 
            phep->sList[i].bFwdOptionsInfoPresent ? "True":"False");
        printf("Forwarding Options: %d\n", 
            (int)phep->sList[i].eFwdOptions);
        printf("No Reply Time Info Present: %s\n", 
            phep->sList[i].bNoReplyTimePresent ? "True":"False");
        printf("No Reply Time (in seconds): %d\n", 
            (int)phep->sList[i].NoReplyTime);
    }
    
    printf("\n");
}

/**************
 *
 * Name:     apgsmsscallbarrspprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_SSRespCallBar notification.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apgsmsscallbarrspprint( struct SwiNotifyVariant *sVariantp )
{
    struct SWI_STRUCT_SS_RespCallBar *phep;
    int i;
    
    /* Initialize the local pointers */
    phep = &sVariantp->_SSRespCallBar;
    
    /* Display the contents of the Notification */
    printf("\n");
    printf("Display SS Call Barring Response Notification: \n");
    printf("Object Version: %d\n", (int)phep->nVersion);
    printf("Supplementary Service Handle: %d\n", (int)phep->nHandle);
    printf("Supplementary Service Code Present: %s\n", 
        phep->bServiceCodePresent ? "True":"False");
    printf("Supplementary Service Code: %d\n", (int)phep->eServiceCode);
    printf("Number of Car Barring Entries: %d\n", (int)phep->nNumEntries);
    for (i = 0; i < phep->nNumEntries; i++)
    {
        printf("\n");
        printf("Basic Service Group Info Present: %s\n", 
            phep->sList[i].bBasicServGrpInfoPresent ? "True":"False");
        printf("Basic Service Group Type: %s\n", 
            apssbasicservgrptypemap[phep->sList[i].eBasicServGrpType]);
        printf("Basic Service Group Code: %d\n", 
            (int)phep->sList[i].eBasicServGrpCode);
        printf("Service Status Info Present: %s\n", 
            phep->sList[i].bServStatusInfoPresent ? "True":"False");
        printf("Service Status: %d\n", (int)phep->sList[i].nServStatus);
    }
    
    printf("\n");
}

/**************
 *
 * Name:     apgsmssinfodatarspprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_SSRespInfoData notification.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apgsmssinfodatarspprint( struct SwiNotifyVariant *sVariantp )
{
    struct SWI_STRUCT_SS_RespInfoData *phep;
    int i;
    
    /* Initialize the local pointers */
    phep = &sVariantp->_SSRespInfoData;
    
    /* Display the contents of the Notification */
    printf("\n");
    printf("Display SS Info Data Response Notification: \n");
    printf("Object Version: %d\n", (int)phep->nVersion);
    printf("Supplementary Service Handle: %d\n", (int)phep->nHandle);
    printf("Supplementary Service Code Present: %s\n", 
        phep->bServiceCodePresent ? "True":"False");
    printf("Supplementary Service Code: %d\n", (int)phep->eServiceCode);
    printf("Service Status Present: %s\n", 
        phep->bServStatusPresent ? "True":"False");
    printf("Service Status: %d\n", (int)phep->eServStatus);
    printf("Subscription Option Info Present: %s\n", 
        phep->bOptionInfoPresent ? "True":"False");
    printf("Subscription Option Type: %d\n", 
        (int)phep->eSubsOptionType);
    printf("Subscription Option Code: %d\n", (int)phep->eSubsOptionCode);
    printf("Number of Basic Service Entries: %d\n", (int)phep->nNumEntries);
    for (i = 0; i < phep->nNumEntries; i++)
    {
        printf("\n");
        printf("Basic Service Group Type: %s\n", 
            apssbasicservgrptypemap[phep->sList[i].eBasicServGrpType]);
        printf("Basic Service Group Code: %d\n", 
            (int)phep->sList[i].eBasicServGrpCode);
    }
    
    printf("\n");
}

/**************
 *
 * Name:     apgsmssstatusrspprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_SSRespStatus notification.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apgsmssstatusrspprint( struct SwiNotifyVariant *sVariantp )
{
    struct SWI_STRUCT_SS_RespStatus *phep;
    
    /* Initialize the local pointers */
    phep = &sVariantp->_SSRespStatus;
    
    /* Display the contents of the Notification */
    printf("\n");
    printf("Display SS Status Response Notification: \n");
    printf("Object Version: %d\n", (int)phep->nVersion);
    printf("Supplementary Service Handle: %d\n", (int)phep->nHandle);
    printf("Service Status: %d\n", (int)phep->nServiceStatus);
    
    printf("\n");
}

/**************
 *
 * Name:     apgsmssfwdnumrspprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_SSRespFwdNum notification.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apgsmssfwdnumrspprint( struct SwiNotifyVariant *sVariantp )
{
    struct SWI_STRUCT_SS_RespFwdNum *phep;
    int i;
    
    /* Initialize the local pointers */
    phep = &sVariantp->_SSRespFwdNum;
    
    /* Display the contents of the Notification */
    printf("\n");
    printf("Display SS Fowarding Number Response Notification: \n");
    printf("Object Version: %d\n", (int)phep->nVersion);
    printf("Supplementary Service Handle: %d\n", (int)phep->nHandle);
    printf("International Number Flag: %s\n", 
        phep->bInternational ? "True":"False");
    printf("Forwarding Number: ");
    for (i = 0; i < phep->nLengthFwdNum; i++)
    {
        printf("%d", (int)phep->byFwdNum[i]);
    }
    
    printf("\n");
}

/**************
 *
 * Name:     apgsmssbslistrspprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_SSRespBSList notification.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apgsmssbslistrspprint( struct SwiNotifyVariant *sVariantp )
{
    struct SWI_STRUCT_SS_RespBSList *phep;
    int i;
    
    /* Initialize the local pointers */
    phep = &sVariantp->_SSRespBSList;
    
    /* Display the contents of the Notification */
    printf("\n");
    printf("Display SS BS List Response Notification: \n");
    printf("Object Version: %d\n", (int)phep->nVersion);
    printf("Supplementary Service Handle: %d\n", (int)phep->nHandle);
    printf("Number of Basic Service Group Entries: %d\n", (int)phep->nNumEntries);
    for (i = 0; i < phep->nNumEntries; i++)
    {
        printf("Basic Service Group Type: %s\n", 
               apssbasicservgrptypemap[phep->sList[i].eBasicServGrpType]);
        printf("Basic Service Group Code: %d\n",
               (int)phep->sList[i].eBasicServGrpCode);
    }
    
    printf("\n");
}

/**************
 *
 * Name:     apgsmssfwdlistrspprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_SSRespFwdList notification.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apgsmssfwdlistrspprint( struct SwiNotifyVariant *sVariantp )
{
    struct SWI_STRUCT_SS_RespListFwdInfo *phep;
    int i;
    int j;
    
    /* Initialize the local pointers */
    phep = &sVariantp->_SSRespFwdList;
    
    /* Display the contents of the Notification */
    printf("\n");
    printf("Display SS Forwarding List Response Notification: \n");
    printf("Object Version: %d\n", (int)phep->nVersion);
    printf("Supplementary Service Handle: %d\n", (int)phep->nHandle);
    printf("Number of Forwarding Info Entries: %d\n", (int)phep->nNumEntries);
    for (i = 0; i < phep->nNumEntries; i++)
    {
        printf("\n");
        printf("Basic Service Group Info Present: %s\n", 
            phep->sList[i].bBasicServGrpInfoPresent ? "True":"False");
        printf("Basic Service Group Type: %s\n", 
            apssbasicservgrptypemap[phep->sList[i].eBasicServGrpType]);
        printf("Basic Service Group Code: %d\n", 
            (int)phep->sList[i].eBasicServGrpCode);
        printf("Service Status Info Present: %s\n", 
            phep->sList[i].bServStatusInfoPresent ? "True":"False");
        printf("Service Status: %d\n", (int)phep->sList[i].eServStatus);
        printf("International Number Flag: %s\n", 
            phep->sList[i].bInternational ? "True":"False");
        printf("Length of Forwarding Number: %d\n", 
            (int)phep->sList[i].nLengthFwdNum);
        printf("Forwarding Number: ");
        for (j = 0; j < phep->sList[i].nLengthFwdNum; j++)
        {
            printf("%d", (int)phep->sList[i].byFwdNum[j]);
        }
        printf("\n");
        printf("Forwarding Options Info Present: %s\n", 
            phep->sList[i].bFwdOptionsInfoPresent ? "True":"False");
        printf("Forwarding Options: %d\n", (int)phep->sList[i].eFwdOptions);
        printf("No Reply Time Info Present: %s\n", 
            phep->sList[i].bNoReplyTimePresent ? "True":"False");
        printf("No Reply Time (in seconds): %d\n", 
            (int)phep->sList[i].NoReplyTime);
    }
    
    printf("\n");
}

/**************
 *
 * Name:     apgsmssrawrspprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_SSRespRaw notification.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apgsmssrawrspprint( struct SwiNotifyVariant *sVariantp )
{
    struct SWI_STRUCT_SS_RespRaw *phep;
    int i;
    
    /* Initialize the local pointers */
    phep = &sVariantp->_SSRespRaw;
    
    /* Display the contents of the Notification */
    printf("\n");
    printf("Display SS Raw Data Response Notification: \n");
    printf("Object Version: %d\n", (int)phep->nVersion);
    printf("Supplementary Service Handle: %d\n", (int)phep->nHandle);
    printf("Raw Data Length: %d\n", (int)phep->nLength);
    printf("Raw Data: ");
    for (i = 0; i < phep->nLength; i++)
    {
        printf("%d", (int)phep->byData[i]);
    }
    
    printf("\n");
}

/**************
 *
 * Name:     apgsmsspwdrspprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_SSRespPassword notification.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apgsmsspwdrspprint( struct SwiNotifyVariant *sVariantp )
{
    struct SWI_STRUCT_SS_RespPassword *phep;
    int i;
    
    /* Initialize the local pointers */
    phep = &sVariantp->_SSRespPassword;
    
    /* Display the contents of the Notification */
    printf("\n");
    printf("Display SS Password Response Notification: \n");
    printf("Object Version: %d\n", (int)phep->nVersion);
    printf("Supplementary Service Handle: %d\n", (int)phep->nHandle);
    printf("Password Length: %d\n", (int)phep->nPasswordLength);
    printf("Password: ");
    for (i = 0; i < phep->nPasswordLength; i++)
    {
        printf("%d", (int)phep->byPassword[i]);
    }
    
    printf("\n");
}

/**************
 *
 * Name:     apgsmssclirrspprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_SSRespClir notification.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apgsmssclirrspprint( struct SwiNotifyVariant *sVariantp )
{
    struct SWI_STRUCT_SS_RespClir *phep;
    
    /* Initialize the local pointers */
    phep = &sVariantp->_SSRespClir;
    
    /* Display the contents of the Notification */
    printf("\n");
    printf("Display SS Clir Response Notification: \n");
    printf("Object Version: %d\n", (int)phep->nVersion);
    printf("Supplementary Service Handle: %d\n", (int)phep->nHandle);
    printf("Service Status: %d\n", (int)phep->eServStatus);
    printf("CLIR Setting: %s\n", apssclirsettingmap[phep->eClir]);
    
    printf("\n");
}

/**************
 *
 * Name:     apgsmsserrorrspprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_SSRespError notification.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apgsmsserrorrspprint( struct SwiNotifyVariant *sVariantp )
{
    struct SWI_STRUCT_SS_RespError *phep;
    
    /* Initialize the local pointers */
    phep = &sVariantp->_SSRespError;
    
    /* Display the contents of the Notification */
    printf("\n");
    printf("Display SS Error Response Notification: \n");
    printf("Object Version: %d\n", (int)phep->nVersion);
    printf("Supplementary Service Handle: %d\n", (int)phep->nHandle);
    printf("Error Code: %d\n", phep->eErrorCode);
    printf("Error Type: %s\n", apsserrortypemap[phep->eErrorType]);
    printf("Error Cause - Call Barring: %s\n", 
        apsscallbarmap[phep->uErrorCause.eCallBarring]);
    printf("Error Cause - Service Status: %d\n", 
        phep->uErrorCause.eServStatus);
    printf("Error Cause - Subscription Violation - Subscription Option Type: %d\n", 
        phep->uErrorCause.sSubscripViolation.eSubsOptionType);
    printf("Error Cause - Subscription Violation - Subscription Option Code: %d\n", 
        phep->uErrorCause.sSubscripViolation.eSubsOptionCode);
    printf("Error Cause - Absent Subscriber: %s\n", 
        phep->uErrorCause.bAbsentSubs ? "True":"False");
    printf("Error Cause - System Failure: %s\n", 
        apsssystemfailmap[phep->uErrorCause.eSystemFail]);
    printf("Error Cause - Password Registration Failure Reason: %s\n", 
        apsspasswdregfailmap[phep->uErrorCause.ePasswdRegFail]);
    printf("Error Cause - Incompatibility Reason - Supplementary Service Code Present: %s\n", 
        phep->uErrorCause.sIncompatibilityReason.bServiceCodePresent ? "True":"False");
    printf("Error Cause - Incompatibility Reason - Supplementary Service Code: %d\n", 
        phep->uErrorCause.sIncompatibilityReason.eServiceCode);
    printf("Error Cause - Incompatibility Reason - Basic Service Group Information Present: %s\n", 
        phep->uErrorCause.sIncompatibilityReason.bBasicServGrpPresent ? "True":"False");
    printf("Error Cause - Incompatibility Reason - Basic Service Group Type: %s\n", 
        apssbasicservgrptypemap[phep->uErrorCause.sIncompatibilityReason.eBasicServGrpType]);
    printf("Error Cause - Incompatibility Reason - Basic Service Group Code: %d\n", 
        (int)phep->uErrorCause.sIncompatibilityReason.eBasicServGrpCode);
    printf("Error Cause - Incompatibility Reason - Service Status Information Present: %s\n", 
        phep->uErrorCause.sIncompatibilityReason.bServStatusInfoPresent ? "True":"False");
    printf("Error Cause - Incompatibility Reason - Service Status: %d\n", 
        phep->uErrorCause.sIncompatibilityReason.eServStatusErr);

    printf("\n");
}

/**************
 *
 * Name:     apgsmssrejectrspprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_SSRespReject notification.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apgsmssrejectrspprint( struct SwiNotifyVariant *sVariantp )
{
    struct SWI_STRUCT_SS_RespReject *phep;
    
    /* Initialize the local pointers */
    phep = &sVariantp->_SSRespReject;
    
    /* Display the contents of the Notification */
    printf("\n");
    printf("Display SS Reject Response Notification: \n");
    printf("Object Version: %d\n", (int)phep->nVersion);
    printf("Supplementary Service Handle: %d\n", (int)phep->nHandle);
    printf("Problem Type: %s\n", apsproblemtypemap[phep->eProblemType]);
    printf("Reject Code: %d\n", phep->eRejectCode);
    printf("\n");
}

/**************
 *
 * Name:     apgsmsscancelrspprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_SSRespCancel notification.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apgsmsscancelrspprint( struct SwiNotifyVariant *sVariantp )
{
    struct SWI_STRUCT_SS_RespCancel *phep;
    
    /* Initialize the local pointers */
    phep = &sVariantp->_SSRespCancel;
    
    /* Display the contents of the Notification */
    printf("\n");
    printf("Display SS Cancel Response Notification: \n");
    printf("Object Version: %d\n", (int)phep->nVersion);
    printf("Supplementary Service Handle: %d\n", (int)phep->nHandle);
    printf("\n");
}

/**************
 *
 * Name:     apgsmssreleaserspprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_SSRespRelease notification.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apgsmssreleaserspprint( struct SwiNotifyVariant *sVariantp )
{
    struct SWI_STRUCT_SS_RespRelease *phep;
    
    /* Initialize the local pointers */
    phep = &sVariantp->_SSRespRelease;
    
    /* Display the contents of the Notification */
    printf("\n");
    printf("Display SS Release Response Notification: \n");
    printf("Object Version: %d\n", (int)phep->nVersion);
    printf("Supplementary Service Handle: %d\n", (int)phep->nHandle);
    printf("\n");
}

/**************
 *
 * Name:     apgsmssnotifyprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_SSNotify notification.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apgsmssnotifyprint( struct SwiNotifyVariant *sVariantp )
{
    struct SWI_STRUCT_SSNotify *phep;
    
    /* Initialize the local pointers */
    phep = &sVariantp->_SSNotify;
    
    /* Display the contents of the Notification */
    printf("\n");
    printf("Display SS Notify Response Notification: \n");
    printf("Object Version: %d\n", (int)phep->nVersion);
    printf("Service Code: %d\n", phep->eServiceCode);
    printf("\n");
}

/**************
 *
 * Name:     apgsmvoicemessageprint
 *
 * Purpose:  This function prints the information received in the 
 *           SWI_NOTIFY_VoiceMessage notification.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apgsmvoicemessageprint( struct SwiNotifyVariant *sVariantp )
{
    struct SWI_STRUCT_VoiceMessage *phep;
    
    /* Initialize the local pointers */
    phep = &sVariantp->_VoiceMessage;
    
    /* Display the contents of the Notification */
    printf("\n");
    printf("Display Voice Message Notification: \n");
    printf("VMWF support: %s\n", phep->bVoiceMailWaitingFeature ? "True":"False");
    printf("Voice mail waiting status for line #1: %s\n", 
        phep->bVoiceMailWaitingLine1 ? "True":"False");
    printf("Voice mail waiting status for line #2: %s\n", 
        phep->bVoiceMailWaitingLine2 ? "True":"False");
    printf("Fax message waiting status: %s\n", 
        phep->bFaxMssageWaiting1 ? "True":"False");
    printf("Data message waiting status: %s\n", 
        phep->bFaxMssageWaiting2 ? "True":"False");
    printf("\n");
}

/**************
 *
 * Name:     apnotifcallback
 *
 * Purpose:  Callback function for notifications
 *           
 *           
 * Parms:   sVariantp    - Pointer to notification structure 
 *                         containing notification information 
 * 
 * Return:   none
 *
 * Notes:    Prints data received for notification
 *
 **************/
package SWI_CALLBACK_EX apnotifcallback( struct SwiNotifyVariant *sVariantp)
{
    int i;

    printf("\nReceived notification:");

    for(i = 0; i < cmnotifymax; i++)
    {
        if(aptcmnotify[i].eNotifytype == sVariantp->eNotify)
        {
            printf("  %s (0x%04x)\n", 
                   aptcmnotify[i].notifystring, 
                   SwiGetObjid(sVariantp->eNotify));
            aptcmnotify[i].printfp(sVariantp);
            return 0;
        }
    } 
    if(swigsmdualrangeuser())
    {
        for(i = 0; i < gsmnotifymax; i++)
        {
            if(aptgsmnotify[i].eNotifytype == sVariantp->eNotify)
            {
                printf("  %s (0x%04x)\n", 
                       aptgsmnotify[i].notifystring, 
                       SwiGetObjid(sVariantp->eNotify));
                aptgsmnotify[i].printfp(sVariantp);
                return 0;
            }
        }
    } 
    else
    {
        for(i = 0; i < cdmanotifymax; i++)
        {
            if(aptcdmanotify[i].eNotifytype == sVariantp->eNotify)
            {
                printf("  %s (0x%04x)\n", 
                       aptcdmanotify[i].notifystring, 
                       SwiGetObjid(sVariantp->eNotify));
                aptcdmanotify[i].printfp(sVariantp);
                return 0;
            }
        }
    } 

    printf("  Unhandled Notification for objectid 0x%04x\n", 
           SwiGetObjid(sVariantp->eNotify)); 
    return 0;
}

/**************
 *
 * Name:     apcallback
 *
 * Purpose:  This function calls each callback from the callback array
 *           one by one.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apcallback( struct SwiNotifyVariant *sNotifyVariant)
{
    int i;
    SWI_CALLBACK_EX callback;

    /* Run each callback in the callback array */
    for (i = 0; i < apmaxused; i++)
    {
        callback = (SWI_CALLBACK_EX)apcbfp[i];
        callback(sNotifyVariant);
    }
}

/**************
 *
 * Name:     doSwiRegisterCallback
 *
 * Purpose:  Runs the test to register a callback with the modem
 *           
 *           
 * Parms:    timeout    - unused input parameter added for consistency
 *                        with all tests.
 * 
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiRegisterCallback(unsigned int timeout)
{
    SWI_RCODE               retcode;
    swi_uint64              nNotifications;

    /* Set notifications filter */
    nNotifications = 0xffffffff;

    /* Register callback notification for Rssi */
    retcode = SwiRegisterCallback((void*)apcallback, nNotifications);

    if( retcode == SWI_RCODE_OK )
        printf("\nRegister callback notification successful\n");
    else
        printf("\nRegister callback notification returned error: %d\n", 
               retcode );
}
/**************
 *
 * Name:     doSwiNotify
 *
 * Purpose:  Runs the test to enable notification messages from the 
 *           modem 
 *           
 * Parms:    timeout   - response timeout value as
 *                       passed in on the command line or defaulted.  
 * 
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiNotify(unsigned int timeout)
{
    SWI_RCODE  retcode;
    int        i;
    swi_char lasterror[80];

    printf("\nEnabling notifications:\n");
 
    for(i = 0; i < cmnotifymax; i++)
    {
        /* Enable all notifications */
        retcode = SwiNotify(aptcmnotify[i].eNotifytype, timeout);
        if( retcode == SWI_RCODE_OK )
            printf("  Completed for %s (0x%04x)\n", 
                   aptcmnotify[i].notifystring, 
                   SwiGetObjid(aptcmnotify[i].eNotifytype));
        else
            printf("  Failed for %s (0x%04x): %d\n",
                   aptcmnotify[i].notifystring, 
                   SwiGetObjid(aptcmnotify[i].eNotifytype), retcode );
            if( retcode == SWI_RCODE_REQUEST_REJECTED )
            {
                SwiGetLastError( lasterror, sizeof(lasterror) );
                printf("  Mdm reports CnS Error: %s\n", lasterror );
            }

    }

    if(swigsmdualrangeuser())
    {
        for(i = 0; i < gsmnotifymax; i++)
        {
            /* Enable notification */
            retcode = SwiNotify(aptgsmnotify[i].eNotifytype, timeout);
            if( retcode == SWI_RCODE_OK )
                printf("  Completed for %s (0x%04x)\n", 
                       aptgsmnotify[i].notifystring, 
                       SwiGetObjid(aptgsmnotify[i].eNotifytype));
            else
                printf("  Failed for %s (0x%04x): %d\n",
                       aptgsmnotify[i].notifystring, 
                       SwiGetObjid(aptgsmnotify[i].eNotifytype), retcode );
        }
    }
    else
    {
        for(i = 0; i < cdmanotifymax; i++)
        {
            /* Enable notification */
            retcode = SwiNotify(aptcdmanotify[i].eNotifytype, timeout);
            if( retcode == SWI_RCODE_OK )
                printf("  Completed for %s (0x%04x)\n", 
                       aptcdmanotify[i].notifystring, 
                       SwiGetObjid(aptcdmanotify[i].eNotifytype));
            else
                printf("  Failed for %s (0x%04x): %d\n",
                       aptcdmanotify[i].notifystring, 
                       SwiGetObjid(aptcdmanotify[i].eNotifytype), retcode );
        }
    }
}

/**************
 *
 * Name:     doSwiWaitNotifications
 *
 * Purpose:  Runs the test receiving a notification message from the 
 *           modem 
 *           
 * Parms:    timeout    - unused input parameter added for consistency
 *                        with all tests.   
 * 
 * Return:   none
 *
 * Notes:    This test never returns.
 *           Ctrl-C  interrupts the test. 
 *
 **************/
package void doSwiWaitNotifications(unsigned int timeout)
{
    SWI_RCODE               retcode;
   
    while (1)
    {
        /* Wait for notifications */
        retcode     = SwiWaitNotification();
        if( retcode != SWI_RCODE_OK ) 
        { 
            printf("\nSwiWaitNotification returned error: %d\n", retcode );
        }
    }
}
/**************
 *
 * Name:     apaddcbkfp
 *
 * Purpose:  This function checks if the callback is a duplicate and would
 *           add the callback to the callback array only if there is capacity.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apaddcbkfp (SWI_CALLBACK_EX *sVariantp)
{
    int i;
    swi_bool duplicate = FALSE;
    
    for (i = 0; i < apmaxused; i++)
    {
        if (apcbfp[i] == sVariantp)
        {
            printf ("\nDuplicate callback rejected\n");
            duplicate = TRUE;
        }
    }
    
    if (!duplicate)
    {
        if (apmaxused >= CBFPSZ)
        {
            printf ("Exceeded max capacity of array");
            exit (1);
        }
        apcbfp[apmaxused] = sVariantp;
        apmaxused++;
        printf ("Callback has been added to callback array\n");
    }
} 

/**************
 *
 * Name:     doNotificationThread
 *
 * Purpose:  This is the thread function started by the doSimThreadn program to 
 *           prepare Notifications handling in a separate thread. 
 *           
 * Parms:    timeout    - unused input parameter added for consistency
 *                        with all tests.   
 * 
 * Return:   none
 *
 * Notes:    This thread never returns.
 *           Ctrl-C  interrupts the test.
 *           Actions:
 *              register a callback function, 
 *              wait for notifications
 *
 *           This thread is NOT allowed to initiate stop-and-wait requests under
 *           any circumstances
 *
 **************/
package void* doNotificationThread(unsigned int timeout)
{
    doSwiRegisterCallback(timeout);
    doSwiWaitNotifications(timeout);
    return 0;
}

/*
 * $Log: aptestnotify.c,v $
 */
