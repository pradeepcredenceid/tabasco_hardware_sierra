/*************
 *
 * Filename:  qatestproto.h
 *
 * Purpose:   This file contains test prototypes for the qa package
 *
 * Copyright: © 2011-2013 Sierra Wireless Inc., all rights reserved
 *
 **************/
#include "qmudefs.h"

/* macros */
#define SFPRINTF(fp,s,p)   if( NULL != p ){ fprintf(fp,s,*p); } else{ fprintf(fp,s,0x0L); }
#define VFPRINTF(fp,s,p)   if( NULL != p ){ fprintf(fp,s, p); } else{ fprintf(fp,s,""); }
#define IFPRINTF(fp,s,p)   if( NULL != p ){ fprintf(fp,s,*p); }

/* defines */
#define LETTER_t        't'
#define HASH            '#'
#define COLON           ':'
#define SQBRACKETOPEN   '['
#define SQBRACKETCLOSE  ']'
#define SEMICOLON       ';'
#define HYPHEN          '-'
#define OPENTAG         '<'
#define CLOSETAG        '>'
#define ENTER_KEY       0x0A

#define MAX_CHAR_IN_A_LINE 500

/* result of a test case sequence execution */
enum eTestCaseError{
    eRESULT_NOK =  0,
    eRESULT_OK  =  1,
};

/* test case sequence with repeat and delay parameters */
typedef struct _testcase_t
{
    char         test_case[MAX_CHAR_IN_A_LINE];
    int          repeat;
    unsigned int delay;
}testCase;


/* Forward definitions - Only allowed for test programs */
extern void PrintHex(
   void         *pBuffer,
   unsigned int bufSize);

extern FILE *tfopen (char *fnamep, char *modep);
extern void tfclose(FILE *fp);
extern void doprintreason(FILE *fp, ULONG nRet);

/* qaGobiApiWdsTest.c */
extern void doStartDataSession2();
extern void doStopDataSession();
extern void doStartDataSessionLTE();
extern void doGetDefaultProfile();
extern void doGetDefaultProfileLTE();
extern void doGetAutoconnect();
extern void doGetSessionState();
extern void doGetPacketStatus();
extern void doGetByteTotals();
extern void doGetDormancyState();
extern void doGetDataBearerTechnology();
extern void doGetSessionDuration();
extern void doGetIPAddress();
extern void doGetIPAddressLTE();
extern void doGetConnectionRate();
extern void doSetAutoconnect();
extern void doSetDefaultProfile();
extern void doSetDefaultProfileLTE();
extern void doGetHomeNetwork();
extern void doSetMobileIP();
extern void doGetMobileIP();
extern void doGetMobileIPProfile();
extern void doGetLastMobileIPError();
extern void doSLQSGetRuntimeSettings();
extern void doSLQSSetProfile();
extern void doSLQSGetProfile();
extern void doSLQSStartStopDataSession_startv4p1();
extern void doSLQSStartStopDataSession_startv4p2();
extern void doSLQSStartStopDataSession_stopv4p1();
extern void doSLQSStartStopDataSession_stopv4p2();
extern void doSLQSStartStopDataSession_startv6p3();
extern void doSLQSStartStopDataSession_startv6p4();
extern void doSLQSStartStopDataSession_stopv6p3();
extern void doSLQSStartStopDataSession_stopv6p4();
extern void doSLQSStartStopDataSession_startv4v6p5();
extern void doSLQSStartStopDataSession_startv4v6p6();
extern void doSLQSStartStopDataSession_stopv4v6p5();
extern void doSLQSStartStopDataSession_stopv4v6p6();
extern void doSLQSStartDataSessionOnSprint();
extern void doSLQSStopDataSessionOnSprint();
extern void doSetMobileIPParameters();
extern void doSLQSSetIPFamilyPreference_v4();
extern void doSLQSSetIPFamilyPreference_v6();
extern void doSLQSSetIPFamilyPreference_v4v6();
extern void doSLQSDeleteProfile();
extern void doSLQSCreateProfile();
extern void doSLQSAutoConnect();
extern void doSLQSGetDataBearerTechnology();
extern void doSLQSModifyProfile();
extern void doSLQSSet3GPPConfigItem();
extern void doSLQSGet3GPPConfigItem();
extern void doSLQSGetProfileSettings();
extern void doSetMobileIPProfile();
extern void doSLQSWdsSetEventReport();
extern void doSLQSWdsSwiPDPRuntimeSettings();
extern void doSLQSGetConnectionRate();
extern void doSLQSGetByteTotals();

/* qaGobiApiDmsTest.c */
extern void doGetManufacturer();
extern void doGetModelID();
extern void doGetFirmwareRevision();
extern void doGetFirmwareRevisions();
extern void doGetPRLVersion();
extern void doGetIMSI();
extern void doGetSerialNumbers();
extern void doGetHardwareRevision();
extern void doGetNetworkTime();
extern void doUIMSetPINProtection();
extern void doUIMVerifyPIN();
extern void doUIMUnblockPIN();
extern void doUIMChangePIN();
extern void doGetVoiceNumber();
extern void doSetPower();
extern void doGetPower();
extern void doUIMGetControlKeyStatus();
extern void doUIMGetICCID();
extern void doUIMGetPINStatus();
extern void doGetOfflineReason();
extern void doUIMSetCkProtection();
extern void doUIMUnblockCk();
extern void doGetDeviceCapabilities ();
extern void doResetToFactoryDefaults();
extern void doValidateSPC();
extern void doActivateAutomatic();
extern void doGetActivationState();
extern void doUIMGetState();
extern void doSLQSgetBandCapability();
extern void doSLQSGetCustFeatures();
extern void doSLQSSetCustFeatures();
extern void doSLQGetCurrentPRLInfo();
extern void doSLQSGetERIFile();
extern void doSLQSSwiGetFSN();
extern void doSLQSSwiGetFirmwareCurr();

/* qaGobiApiNasTest.c */
extern void doGetHomeNetwork();
extern void doGetServingSystem();
extern void doGetSignalStrengths();
extern void doInitNetworkReg();
extern void doPerformNetworkScan();
extern void doGetServingNetworkCaps();
extern void doSetNetworkPreference();
extern void doGetNetworkPreference();
extern void doGetRFInfo();
extern void doInitiateDomainAttach();
extern void doGetACCOLC();
extern void doSetACCOLC();
extern void doSetCDMANetworkParameters();
extern void doGetCDMANetworkParameters();
extern void doGetANAAAAuthStatus();
extern void doSLQSGetServingSystem();
extern void doSLQSSetBandPreference();
extern void doSLQSNasIndicationRegister();
extern void doSLQSGetSignalStrength();
extern void doSLQSPerformNetworkScan();
extern void doSLQSSetSysSelectionPref();
extern void doSLQSGetSysSelectionPref();
extern void doSLQSNasGetSysInfo();
extern void doSLQSNasSwiModemStatus();
extern void doSLQSNasGetHDRColorCode();
extern void doSLQSNasGetTxRxInfo();
extern void doSLQSNasGetSigInfo();
extern void doSLQSNasIndicationRegisterExt();
extern void doSLQSGetPLMNName();
extern void doSLQSGetOperatorNameData();
extern void doSLQSGet3GPP2Subscription();
extern void doSLQSNasGetCellLocationInfo();
extern void doSLQSInitNetworkReg();
extern void doSLQSSwiGetHDRPersonality();
extern void doSLQSSwiGetHDRProtoSubtype();
extern void doSLQSSwiPSDetach();

/* qaGobiApiPdsTest.c */
extern void doGetPDSState();
extern void doSetPDSState();
extern void doStartPDSTrackingSessionExt();
extern void doStopPDSTrackingSession();
extern void doGetPDSDefaults();
extern void doSetPDSDefaults();
extern void doGetXTRAAutomaticDownload();
extern void doSetXTRAAutomaticDownload();
extern void doGetXTRANetwork();
extern void doSetXTRANetwork();
extern void doGetServiceAutomaticTracking();
extern void doSetServiceAutomaticTracking();
extern void doGetPortAutomaticTracking();
extern void doSetPortAutomaticTracking();
extern void doForceXTRADownload();
extern void doGetXTRAValidity();
extern void doResetPDSData();
extern void doPDSInjectTimeReference();
extern void doSLQSSetAGPSConfig();
extern void doSLQSPDSInjectAbsoluteTimeReference();
extern void doSLQSGetAGPSConfig();
extern void doSLQSPDSInjectPositionData();
extern void doSLQSPDSDeterminePosition();

/* qaGobiApiFmsTest.c */
extern void doExecuteFmsStubs();
extern void doSLQSGetFirmwareInfo();
extern void doUpgradeFirmware2k();
extern void doSLQSGetImageInfo();
extern void doGetFirmwareInfo();
extern void doGetImageInfo();
extern void doGetImagesPreference();
extern void doSetImagesPreference();
extern void doGetStoredImages();
extern void doSLQSUpgradeGobiFw();
extern void doDeleteStoredImage();
extern void doSLQSGetImageInfo_9x15();

/* qaGobiApiCatTest.c */
extern void doCATSendEnvelopeCommand();
extern void doCATSendTerminalResponse();

/* qaGobiApiRmsTest.c */
extern void doGetSMSWake();
extern void doSetSMSWake();

/* qaGobiApiSmsTest.c */
extern void doExecuteSmsStubs();
extern void doSLQSDeleteSMS();
extern void doGetSMSCAddress();
extern void doSetSMSCAddress();
extern void doSendSMS();
extern void doSaveSMS();
extern void doSLQSGetSMS();
extern void doSLQSGetSMSList();
extern void doSLQSModifySMSStatus();
extern void doSLQSGetSMSBroadcastConfig();
extern void doSLQSSetSmsBroadcastConfig();
extern void doSetSmsBroadcastActivation();
extern void doSLQSCDMAEncodeMOTextMsg();
extern void doSLQSCDMADecodeMTTextMsg();
extern void doSLQSWCDMAEncodeMOTextMsg();
extern void doSLQSWCDMADecodeMTTextMsg();
extern void doSLQSSendSMS();
extern void doSLQSGetTransLayerInfo();
extern void doSLQSGetTransNWRegInfo();
extern void doSLQSGetIndicationRegister();
extern void doSLQSSetIndicationRegister();
extern void doSLQSSmsSetRoutes();
extern void doSLQSSmsGetMessageProtocol();
extern void doSLQSSmsGetMaxStorageSize();
extern void doSLQSGetMessageWaiting();

/* qaGobiApiDcsTest.c */
extern void doDCSCancel();
extern void doDCSGetConnecteDevID();
extern void doSLQSGetUsbPortNames();
extern void doSLQSGetDeviceMode();
extern void doSLQSStartSrv();
void doSLQSQosMap();
void doSLQSQosUnmap();
void doSLQSQosClearMap();
void doSLQSQosEditMap();
void doSLQSQosReadMap();
void doSLQSQosDumpMap();

/* qaGobiApiCbkTest.c */
extern void doSetSessionStateCallback();
extern void doClearSessionStateCallback();
extern void doSLQSSetWdsEventCallback();
extern void doClearSLQSWdsEventCallback();
extern void doSetPowerCallback();
extern void doClearPowerCallback();
extern void doSetActivationStatusCallback();
extern void doClearActivationStatusCallback();
extern void doSetMobileIPStatusCallback();
extern void doClearMobileIPStatusCallback();
extern void doSetRoamingIndicatorCallback();
extern void doClearRoamingIndicatorCallback();
extern void doSetDataCapabilitiesCallback();
extern void doClearDataCapabilitiesCallback();
extern void doSetSignalStrengthCallback();
extern void doClearSignalStrengthCallback();
extern void doSetRFInfoCallback();
extern void doClearRFInfoCallback();
extern void doSetLURejectCallback();
extern void doClearLURejectCallback();
extern void doSetNMEACallback();
extern void doClearNMEACallback();
extern void doSetPdsStateCallback();
extern void doClearPdsStateCallback();
extern void doSetNewSMSCallback();
extern void doClearNewSMSCallback();
extern void doSetCATEventCallback();
extern void doClearCATEventCallback();
extern void doSetSLQSOMADMAlertCallback();
extern void doClearSLQSOMADMAlertCallback();
extern void doSetDeviceStateChangeCallback();
extern void doClearDeviceStateChangeCallback();
extern void doSetFwDldCompletionCallback();
extern void doClearFwDldCompletionCallback();
extern void doSetSLQSServingSystemCallback();
extern void doClearSLQSServingSystemCallback();
extern void doSetOMADMStateCallback();
extern void doClearOMADMStateCallback();
extern void doSetSLQSSetBandPrefCallBack();
extern void doClearSLQSSetBandPrefCallback();
extern void doSetUSSDReleaseCallback();
extern void doClearSetUSSDReleaseCallback();
extern void doSetUSSDNotificationCallback();
extern void doClearUSSDNotificationCallback();
extern void doSLQSSetSMSEventCallback();
extern void doClearSLQSSetSMSEventCallback();
extern void doSLQSSetSignalStrengthsCallback();
extern void doClearSLQSSetSignalStrengthsCallback();
extern void doSLQSVoiceSetSUPSNotificationCallback();
extern void doClearSLQSVoiceSetSUPSNotificationCallback();
extern void doSetSLQSSDKTerminatedCallback();
extern void doClearSLQSSDKTerminatedCallback();
extern void doSetSLQSVoiceSetAllCallStatusCallback();
extern void doClearSLQSVoiceSetAllCallStatusCallback();
extern void doSLQSSetTransLayerInfoCallback();
extern void doClearSLQSSetTransLayerInfoCallback();
extern void doSLQSSetTransNWRegInfoCallback();
extern void doClearSLQSSetTransNWRegInfoCallback();
extern void doSLQSSetSysSelectionPrefCallBack();
extern void doClearSLQSSetSysSelectionPrefCallBack();
extern void doSetSLQSVoiceSetPrivacyChangeCallback();
extern void doClearSLQSVoiceSetPrivacyChangeCallback();
extern void doSLQSUIMSetStatusChangeCallBack();
extern void doClearSLQSUIMSetStatusChangeCallBack();
extern void doSLQSUIMSetRefreshCallBack();
extern void doClearSLQSUIMSetRefreshCallBack();
extern void doSetSLQSVoiceSetDTMFEventCallback();
extern void doClearSLQSVoiceSetDTMFEventCallback();
extern void doSetSLQSVoiceSetSUPSCallback();
extern void doClearSLQSVoiceSetSUPSCallback();
extern void doSetSLQSNasSysInfoCallBack();
extern void doClearSLQSNasSysInfoCallBack();
extern void doSetSLQSNasNetworkTimeCallBack();
extern void doClearSLQSNasNetworkTimeCallBack();
extern void doSetSLQSWmsMemoryFullCallBack();
extern void doClearSLQSWmsMemoryFullCallBack();
extern void doSLQSVoiceSetOTASPStatusCallback();
extern void doClearSLQSVoiceSetOTASPStatusCallback();
extern void doSetSLQSVoiceInfoRecCallback();
extern void doClearSLQSVoiceInfoRecCallback();
extern void doSetSLQSSetQosNWStatusCallback( void );
extern void doClearSLQSSetQosNWStatusCallback( void );
extern void doSetSLQSSetQosStatusCallback( void );
extern void doClearSLQSSetQosStatusCallback( void );
extern void doSetSLQSSetQosPriEventCallback( void );
extern void doClearSLQSSetQosPriEventCallback( void );
extern void doSetNetChangeCbk( void );
extern void doClearNetChangeCbk( void );
extern void doSetSLQSWmsMessageWaitingCallBack();
extern void doClearSLQSWmsMessageWaitingCallBack();
extern void doSetSLQSNasSigInfoCallBack();
extern void doClearSLQSNasSigInfoCallBack();
extern void doSLQSSetQosFirstInstEventCallback();
extern void doSLQSClearQosFirstInstEventCallback();
extern void doSLQSSetQosSecondInstEventCallback();
extern void doSLQSClearQosSecondInstEventCallback();
extern void doSLQSSetQosThirdInstEventCallback();
extern void doSLQSClearQosThirdInstEventCallback();
extern void doSLQSSetModemTempCallback();
extern void doClearSLQSSetModemTempCallback();
extern void doSLQSSetPacketSrvStatusCallback();
extern void doClearSLQSSetPacketSrvStatusCallback();
extern void doSetSLQSSetSwiHDRPersCallBack();
extern void doClearSLQSSetSwiHDRPersCallBack();
extern void doSLQSSetSIPConfigCallback();
extern void doClearSLQSSetSIPConfigCallback();
extern void doSLQSSetRegMgrConfigCallback();
extern void doClearSLQSSetRegMgrConfigCallback();
extern void doSLQSSetSMSConfigCallback();
extern void doClearSLQSSetSMSConfigCallback();
extern void doSLQSSetUserConfigCallback();
extern void doClearSLQSSetUserConfigCallback();
extern void doSLQSSetVoIPConfigCallback();
extern void doClearSLQSSetVoIPConfigCallback();
extern void doSetUSSDNoWaitIndicationCallback();
extern void doClearUSSDNoWaitIndicationCallback();
extern void doSetSLQSSetIMSARegStatusCallback();
extern void doClearSLQSSetIMSARegStatusCallback();
extern void doSetSLQSSetIMSASvcStatusCallback();
extern void doClearSLQSSetIMSASvcStatusCallback();
extern void doSetSLQSSetIMSARatStatusCallback();
extern void doClearSLQSSetIMSARatStatusCallback();
extern void doSetLocCradleMountCallback();
extern void doClearLocCradleMountCallback();
extern void doSetLocEventTimeSyncCallback();
extern void doClearLocEventTimeSyncCallback();
extern void doSetLocInjectTimeCallback();
extern void doClearLocInjectTimeCallback();
extern void doSetLocSensorStreamCallback();
extern void doClearLocSensorStreamCallback();
extern void doSetLocInjectSensorDataCallback();
extern void doClearLocInjectSensorDataCallback();
extern void doSetLocEventPositionCallback();
extern void doClearLocEventPositionCallback();
extern void doSetLocGnssSvInfoCallback();
extern void doClearLocGnssSvInfoCallback();
extern void doSetLocOpModeCallback();
extern void doClearLocOpModeCallback();
extern void doSetLocDeleteAssistDataCallback();
extern void doClearLocDeleteAssistDataCallback();

/* qaGobiApiSwiOmadmsTest.c */
extern void doSLQSOMADMStartSession();
extern void doSLQSOMADMCancelSession();
extern void doSLQSOMADMGetSessionInfo();
extern void doSLQSOMADMSendSelection();
extern void doSLQSOMADMGetSettings();
extern void doSLQSOMADMSetSettings();
extern void doSLQSOMADMSetSettings2();
extern void doSLQSOMADMGetSettings2();

/* qaGobiApiOmaDmTest.c*/
extern void doOMADMSStartSession();
extern void doOMADMSCancelSession();
extern void doOMADMSGetSessionInfo();
extern void doOMADMSGetPendingNIA();

/* qaGobiApiSarTest.c */
extern void doSLQSGetRfSarState();
extern void doSLQSSetRfSarState();

/* qaGobiApiVoiceTest.c */
extern void doOriginateUSSD();
extern void doAnswerUSSD();
extern void doCancelUSSD();
extern void doSLQSVoiceDialCall();
extern void doSLQSVoiceEndCall();
extern void doFunctionalDialEndCall();
extern void doSLQSVoiceSetSUPSService();
extern void doSLQSVoiceSetConfig();
extern void doSLQSAnswerCall();
extern void doSLQSVoiceGetCLIR();
extern void doSLQSVoiceGetCLIP();
extern void doSLQSVoiceGetCallWaiting();
extern void doSLQSVoiceGetCallBarring();
extern void doSLQSVoiceGetCallForwardingStatus();
extern void doSLQSVoiceSetCallBarringPassword();
extern void doSLQSVoiceGetCallInfo();
extern void doSLQSVoiceGetAllCallInfo();
extern void doSLQSVoiceManageCalls();
extern void doSLQSVoiceBurstDTMF();
extern void doSLQSVoiceStartContDTMF();
extern void doSLQSVoiceStopContDTMF();
extern void doSLQSVoiceSendFlash();
extern void doSLQSVoiceSetPreferredPrivacy();
extern void doSLQSVoiceIndicationRegister();
extern void doSLQSVoiceGetConfig();
extern void doSLQSVoiceOrigUSSDNoWait();
extern void doSLQSVoiceBindSubscription();
extern void doSLQSVoiceALSSetLineSwitching();
extern void doSLQSVoiceALSSelectLine();
extern void doSLQSVoiceGetCOLP();
extern void doSLQSVoiceGetCOLR();
extern void doSLQSVoiceGetCNAP();

/* qaGobiApiUimTest.c */
extern void doSLQSUIMReset();
extern void doSLQSUIMPowerDown();
extern void doSLQSUIMGetCardStatus();
extern void doSLQSUIMSetPinProtection();
extern void doSLQSUIMVerifyPin();
extern void doSLQSUIMChangePin();
extern void doSLQSUIMUnblockPin();
extern void doSLQSUIMRefreshOK();
extern void doSLQSUIMRefreshRegister();
extern void doSLQSUIMEventRegister();
extern void doSLQSUIMRefreshGetLastEvent();
extern void doSLQSUIMRefreshComplete();
extern void doSLQSUIMGetFileAttributes();
extern void doSLQSUIMDepersonalization();
extern void doSLQSUIMAuthenticate();
extern void doSLQSUIMGetConfiguration();

/* qaGobiApiAudioTest.c */
extern void doSLQSGetAudioProfile();
extern void doSLQSSetAudioProfile();
extern void doSLQSGetAudioPathConfig();
extern void doSLQSSetAudioPathConfig();
extern void doSLQSGetAudioVolTLBConfig();
extern void doSLQSSetAudioVolTLBConfig();

/* qaGobiApiImsTest.c */
extern void doSLQSSetSIPConfig();
extern void doSLQSSetRegMgrConfig();
extern void doSLQSSetIMSSMSConfig();
extern void doSLQSSetIMSUserConfig();
extern void doSLQSSetIMSVoIPConfig();
extern void doSLQSGetSIPConfig();
extern void doSLQSGetRegMgrConfig();
extern void doSLQSGetIMSSMSConfig();
extern void doSLQSGetIMSUserConfig();
extern void doSLQSGetIMSVoIPConfig();
extern void doSLQSImsConfigIndicationRegister();

/* qaGobiApiImsaTest.c */
extern void doSLQSRegisterIMSAIndication();

/* qaGobiApiSwiAudioTest.c */
extern void doSLQSGetM2MAudioProfile();
extern void doSLQSSetM2MAudioProfile();
extern void doSLQSGetM2MAudioVolume();
extern void doSLQSSetM2MAudioVolume();
extern void doSLQSSetM2MAudioAVCFG();
extern void doSLQSSetM2MAudioLPBK();
extern void doSLQSSetM2MAudioNVDef();
extern void doSLQSGetM2MSpkrGain();
extern void doSLQSSetM2MSpkrGain();
extern void doSLQSGetM2MAVMute();
extern void doSLQSSetM2MAVMute();

/* qaGobiApiSwiTest.c */
extern void doSLQSKillSDKProcess();

/* qaGobiApiQosTest.c */
void doSLQSQosGetNetworkStatus();
void doSLQSQosReq();
void doSLQSQosRel();
void doSLQSQosGetFlowStatus();
void doSLQSQosGetGranted();
void doSLQSQosSuspend();
void doSLQSQosResume();
void doSLQSQosReset();
void doSLQSQosGetNWProf();

/* qaGobiApiLocTest.c */
void doSLQSLOCEventRegister();
void doSLQSLOCSetExtPowerState();
void doSLQSLOCStart();
void doSLQSLOCStop();
void doSLQSLOCSetOpMode();
void doSLQSLOCDelAssData();
void doSLQSLOCInjectUTCTime(void);
void doSLQSLOCGetServer(void);
void doSLQSLOCSetServer(void);
void doSLQSLOCGetOpMode(void);
void doSLQSLOCGetFixCriteria(void);
void doSwiLocGetAutoStart();
void doSwiLocSetAutoStart();
void doSLQSLOCInjectPosition(void);
void doSLQSLOCSetCradleMountConfig(void);
void doSLQSLOCInjectSensorData(void);

extern package void qaProcessTests(
    char         *testsp,
    char         *testsfilenamep,
    char         *testsequencename,
    unsigned int timeout);

enum eTestCaseError qaExecuteTestSequence(
    char          *pCurrentTestCase,
    unsigned long curIndex,
    unsigned long finalIndex,
    unsigned int  timeout );

extern void FlushStdinStream();
