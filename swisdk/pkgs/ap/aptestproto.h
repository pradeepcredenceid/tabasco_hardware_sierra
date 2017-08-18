/*************
 *
 * $Id: aptestproto.h,v 1.38 2010/06/10 00:05:59 epasheva Exp $
 *
 * Filename:  aptestproto.h
 *
 * Purpose:   This file contains test prototypes for the ap package
 *
 * Copyright: � 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* Forward definitions - Only allowed for test programs */
/* aptestroutines.c */
extern void doSwiGetMEIDESN( swi_uint32 timeout );
extern void doSwiSetTempModePref( swi_uint32 timeout );
extern void doSwiPstSetHomeSidNid( swi_uint32 timeout );
extern void doSwiGetFirmwareVersion(unsigned int timeout);
extern void doSwiGetTrafficStatistics(unsigned int timeout);
extern void doSwiResetTrafficStatistics(unsigned int timeout);
extern void doDeprecated( swi_uint32 timeout );
extern void apDisplayTestXref();
extern void apRunSelectedTest(char * testtagp, unsigned int timeout);
extern void apProcessTestList(char * testsp, unsigned int timeout);
extern void apProcessTestFile(char * testfilenamep, unsigned int timeout);
extern void apProcessTests(char * testsp, char * testsfilenamep,
                           unsigned int timeout);
extern void apaddcbkfp (SWI_CALLBACK_EX *sVariantp);

/* aptestgsmnet.c */
extern void doSwiGetGsmProfileSummary( unsigned int timeout );
extern void doSwiSetDefaultProfile( unsigned int timeout );
extern void doSwiGetGsmProfileBasic( unsigned int timeout );
extern void doSwiGetGsmProfileQos( unsigned int timeout );
extern void doSwiGetGsmProfileDns( unsigned int timeout );
extern void doSwiGetGsmProfileTft( unsigned int timeout );
extern void doSwiActivateProfile( unsigned int timeout);
extern void doSwiDeActivateProfile( unsigned int timeout);
extern void doSwiGsmStatus( unsigned int timeout );
extern void doSwiSetPLMNMode( unsigned int timeout );
extern void doSwiGetPLMNMode( unsigned int timeout );
extern void doSwiStartPLMNSearch( unsigned int timeout );
extern void doSwiGetPLMNSelection( unsigned int timeout );
extern void doSwiGetServiceStatusEx( unsigned int timeout );
extern void doSwiGetPktSessionStatusEx( unsigned int timeout );
extern void doSwiSetGsmProfileBasic(unsigned int timeout);
extern void doSwiSetGsmProfileQos(unsigned int timeout);
extern void doSwiSetGsmProfileDns(unsigned int timeout);
extern void doSwiSetGsmProfileTft(unsigned int timeout);
extern void doSwiEraseProfile(unsigned int timeout);
extern void doSwiGetRat(unsigned int timeout);
extern void doSwiSetRat(unsigned int timeout);
extern void doSwiGetPrlRegion(unsigned int timeout);
extern void doSwiSetPrlRegion(unsigned int timeout);
/* aptestcmbasic.c */
extern void doSwiGetFirmwareVersion( unsigned int timeout );
extern void doSwiGetFirmwareBuildDate( unsigned int timeout );
extern void doSwiGetHardwareVersion( unsigned int timeout );
extern void doSwiGetBootVersion( unsigned int timeout );
extern void doSwiGetBootloaderBuildDate( unsigned int timeout );

extern void doSwiGetRadioPower( unsigned int timeout );
extern void doSwiSetRadioPower( unsigned int timeout );
extern void doSwiGetRadioTempState( unsigned int timeout );
extern void doSwiGetRadioBattery( unsigned int timeout );
extern void doSwiGetCurrentRadioBand( unsigned int timeout );
extern void doSwiGetRadioBandCfg( unsigned int timeout );
extern void doSwiSetRadioBandCfg( unsigned int timeout );

extern void doSwiGetUsbdInfo( unsigned int timeout );
extern void doSwiGetPriInfo( unsigned int timeout );
extern void doSwiGetModemType( unsigned int timeout );
extern void doSwiGetDeviceTime( unsigned int timeout );
extern void doSwiGetDeviceID( unsigned int timeout );
extern void doSwiGetFlashImageInfo( unsigned int timeout );
extern void doSwiGetObjRngRev( unsigned int timeout );
extern void doSwiGetUsbPortName( unsigned int timeout );
extern void doSwiSoftModemReset(unsigned int timeout);
extern void doSwiResetToBootHold(unsigned int timeout);
extern void doSwiGetDeviceInformation(unsigned int timeout);
extern void doSwiGetSwocCfg(unsigned int timeout);

/* aptestgsmbasic.c */
extern void doSwiGetImei(unsigned int timeout);
extern void doSwiGetBandInfo(unsigned int timeout);
extern void doSwiGetAvailableFeatures(unsigned int timeout);
extern void doSwiGetRssiBer(unsigned int timeout);
extern void doSwiGetSerialNumber(unsigned int timeout);
extern void doSwiGetTestHistoryByte(unsigned int timeout);
extern void doSwiGetSetModemDisable(unsigned int timeout);
extern void doSwiGetFeatureCustomizations(unsigned int timeout);
extern void doSwiStopAllNotif(unsigned int timeout);

/* aptestgsmsim.c */
extern void doSwiSetMEPUnlock(unsigned int timeout);
extern void doSwiSetSimVerify(unsigned int timeout);
extern void doSwiSetSimLock(unsigned int timeout);
extern void doSwiGetSimLock(unsigned int timeout);
extern void doSwiSetSimPassword(unsigned int timeout);
extern void doSwiChv2StatusKick(unsigned int timeout);
extern void doSwiGetGsmIMSI(unsigned int timeout);
extern void doSwiGetIccId(unsigned int timeout);
extern void doSwiGetNetworkProviderID(unsigned int timeout);
extern void doSwiSetSimAuthentication(unsigned int timeout);

/* aptestcmupgrade.c */
extern void doSwiNvStore(unsigned int timeout);
extern void apnvstoreprint( struct SwiNotifyVariant *sVariantp);
extern void apnvupdateprint( struct SwiNotifyVariant *sVariantp);
extern void doSwiFirmwareDownload(unsigned int timeout);
extern void apfwdownloadprint( struct SwiNotifyVariant *sVariantp);

/* aptestgsmlbs.c */
extern void doSwiSetLbsPaKeepWarmStart(unsigned int timeout);
extern void doSwiGetLbsPaWarmStatus(unsigned int timeout);
extern void doSwiSetLbsPaKeepWarmStop(unsigned int timeout);
extern void doSwiGetLbsFixSettings(unsigned int timeout);
extern void doSwiSetLbsFixSettings(unsigned int timeout);
extern void doSwiSetLbsNiReq(unsigned int timeout);

/* aptestnotify.c */
extern void doSwiRegisterCallback(unsigned int timeout);
extern void doDeprecated(swi_uint32 timeout);
extern void doSwiNotify(unsigned int timeout);
extern void doSwiWaitNotifications(unsigned int timeout);
extern void aprssiprint( struct SwiNotifyVariant *sVariantp);
extern void apgsmcellprint( struct SwiNotifyVariant *sVariantp);
extern void apbytecntprint( struct SwiNotifyVariant *sVariantp);
extern void apsimstatusprint( struct SwiNotifyVariant *sVariantp);
extern void apgsmprofileprint( struct SwiNotifyVariant *sVariantp);
extern void apradiopowerprint( struct SwiNotifyVariant *sVariantp);
extern void apradiotempprint( struct SwiNotifyVariant *sVariantp);
extern void apradiobatteryprint( struct SwiNotifyVariant *sVariantp);
extern void apcurradiobandprint( struct SwiNotifyVariant *sVariantp);
extern void apgsmplmnreadyprint( struct SwiNotifyVariant *sVariantp);
extern void apgsmplmnavailprint( struct SwiNotifyVariant *sVariantp);
extern void apgsmregistrprint( struct SwiNotifyVariant *sVariantp);
extern void apgsmplmnmodeprint( struct SwiNotifyVariant *sVariantp);
extern void apgsmsrviconprint( struct SwiNotifyVariant *sVariantp);
extern void apgsmnetstatprint( struct SwiNotifyVariant *sVariantp);
extern void apgsmnetsessact( struct SwiNotifyVariant *sVariantp);
extern void apgsmmodemdisabledprint( struct SwiNotifyVariant *sVariantp);
extern void apcmdevicetimeprint( struct SwiNotifyVariant *sVariantp);
extern void apairchangeprint( struct SwiNotifyVariant *sVariantp );
extern void aplbswarmbeginprint( struct SwiNotifyVariant *sVariantp);
extern void aplbswarmstatusprint( struct SwiNotifyVariant *sVariantp);
extern void aplbswarmdoneprint( struct SwiNotifyVariant *sVariantp);
extern void aplbsnirequestprint( struct SwiNotifyVariant *sVariantp);
extern void* doNotificationThread(unsigned int timeout);
extern SWI_CALLBACK_EX apnotifcallback( struct SwiNotifyVariant *sVariantp);
extern char apsimstatusmap[][20];
extern void apgsmphbkentry( struct SwiNotifyVariant *sVariantp );
extern void apgsmphbkready( struct SwiNotifyVariant *sVariantp );
extern void apstkcmdnotifprint( struct SwiNotifyVariant *sVariantp );
extern void apaudioprofileprint( struct SwiNotifyVariant *sVariantp );
extern void apgsmvoicemessageprint( struct SwiNotifyVariant *sVariantp );
extern void apgsmmissedcallprint( struct SwiNotifyVariant *sVariantp );
extern void apgsmcallexpprogressprint( struct SwiNotifyVariant *sVariantp );
extern void apgsmcallforwardedprint( struct SwiNotifyVariant *sVariantp );
extern void apgsmcallstatusprint( struct SwiNotifyVariant *sVariantp );
extern void apgsmdisplaycficonprint( struct SwiNotifyVariant *sVariantp );
extern void apgsmsspasswordstatusprint( struct SwiNotifyVariant *sVariantp );
extern void apgsmssrespokprint( struct SwiNotifyVariant *sVariantp );
extern void apgsmssfwdinforspprint( struct SwiNotifyVariant *sVariantp );
extern void apgsmsscallbarrspprint( struct SwiNotifyVariant *sVariantp );
extern void apgsmssinfodatarspprint( struct SwiNotifyVariant *sVariantp );
extern void apgsmssstatusrspprint( struct SwiNotifyVariant *sVariantp );
extern void apgsmssfwdnumrspprint( struct SwiNotifyVariant *sVariantp );
extern void apgsmssbslistrspprint( struct SwiNotifyVariant *sVariantp );
extern void apgsmssfwdlistrspprint( struct SwiNotifyVariant *sVariantp );
extern void apgsmssrawrspprint( struct SwiNotifyVariant *sVariantp );
extern void apgsmsspasswordstatusprint( struct SwiNotifyVariant *sVariantp );
extern void apgsmssclirrspprint( struct SwiNotifyVariant *sVariantp );
extern void apgsmsserrorrspprint( struct SwiNotifyVariant *sVariantp );
extern void apgsmssrejectrspprint( struct SwiNotifyVariant *sVariantp );
extern void apgsmsscancelrspprint( struct SwiNotifyVariant *sVariantp );
extern void apgsmssreleaserspprint( struct SwiNotifyVariant *sVariantp );
extern void apgsmssnotifyprint( struct SwiNotifyVariant *sVariantp );
extern void apgsmvoicemessageprint( struct SwiNotifyVariant *sVariantp );

/* aptestgsmphonebook.c */
extern void doSwiGetPhonebookEntryExp(unsigned int timeout);
extern void doSwiGetPhonebookAvailable(unsigned int timeout);
extern void doSwiGetPhonebookSize(unsigned int timeout);
extern void doSwiGetFdnMode(unsigned int timeout);
extern void doSwiSetFdnModeEnable(unsigned int timeout);
extern void doSwiSetFdnModeDisable(unsigned int timeout);
extern void doSwiGetEmergencyEntry(unsigned int timeout);
extern void doSwiAddPhonebookEntryExp(unsigned int timeout);
extern void doSwiDeletePhonebookEntry(unsigned int timeout);
extern void doSwiEditPhonebookEntryExp(unsigned int timeout);
extern void doSwiDeletePhonebookLastNum(unsigned int timeout);

/* aptestvoice.c */
extern void doSwiGetSpeakerVolume( unsigned int timeout);
extern void doSwiSetSpeakerVolume( unsigned int timeout);
extern void doSwiGetMicMute (unsigned int timeout);
extern void doSwiSetMicMute (unsigned int timeout);
extern void doSwiGetSpeakerMute (unsigned int timeout);
extern void doSwiSetSpeakerMute (unsigned int timeout);
extern void doSwiGetNVToneStore (unsigned int timeout);
extern void doSwiSetNVToneStore (unsigned int timeout);
extern void doSwiGetAudioProfile (unsigned int timeout);
extern void doSwiSetAudioProfile (unsigned int timeout);
extern void doSwiGetCLIRSetting (unsigned int timeout);
extern void doSwiSetCLIRSetting (unsigned int timeout);
extern void doSwiGetVoiceMessage (unsigned int timeout);
extern void doSwiAcknowledgeMissedCall (unsigned int timeout);
extern void doSwiSetCallOriginate (unsigned int timeout);
extern void doSwiSetDTMFSend (unsigned int timeout);
extern void doSwiSetCallControlUMTS (unsigned int timeout);
extern void doSwiSetContDtmf (unsigned int timeout);
extern void doSwiSetUserdefinedUserBusy (unsigned int timeout);
extern void doSwiSetSSRequest (unsigned int timeout);
extern void doSwiAbortRequest (unsigned int timeout);
extern void doSwiSetSSPassword (unsigned int timeout);
extern void doSwiSetSSRequestErase (unsigned int timeout);
extern void doSwiSetSSRequestActivate (unsigned int timeout);
extern void doSwiSetSSRequestDeactivate (unsigned int timeout);
extern void doSwiSetSSRequestInterrogate (unsigned int timeout);
extern void doSwiSetSSRequestRegister (unsigned int timeout);

/* aptestgsmstk.c */
extern void doSwiGetSIMToolKitCommand(unsigned int timeout);

/*
 * $Log: aptestproto.h,v $
 */
