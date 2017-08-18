/*************
 *
 * $Id: apiproto.h,v 1.35 2010/06/10 00:26:12 epasheva Exp $
 *
 * Filename:  apiproto.h
 *
 * Purpose:   This file contains internal prototypes for the ap package
 *
 * Copyright: � 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* SwiIntCore.c */
extern struct apicb *SwiApiGetCbp(void);
extern swi_bool SwiApiRunning( void );
extern swi_bool Swi_validatetimeout( swi_uint32 timeout );
extern void SwiUpkGsBandInfo (
    struct CNS_BAND_PAYLOAD *pPayload, 
    struct SWI_STRUCT_Band *sBand);
extern void SwiUpkGsAvailableFeatures (
    swi_uint8 *paramfieldp, 
    struct SWI_STRUCT_AvailFeatures * sFeaturesp);

/* SwiIntUpkCdmaGeneral.c */
extern void SwiUnpackRssicdma( 
    swi_uint8 *paramfieldp, 
    swi_int32 *rssip );

/* SwiIntUpkGsmBasic.c */
extern void SwiUpkGsBandInfo (
    struct CNS_BAND_PAYLOAD *paramfieldp, 
    struct SWI_STRUCT_Band *sBand);
extern void SwiUpkGsAvailableFeatures (
    swi_uint8 *paramfieldp, 
    struct SWI_STRUCT_AvailFeatures * sFeaturesp);
extern void SwiUpkGsSerialNumber (swi_uint8 *paramfieldp, 
                                  struct SWI_STRUCT_Fsn *sFsnp);
extern void SwiUpkGsThb(swi_uint8 *paramfieldp, 
                        struct SWI_STRUCT_Thb *sThbp);
extern void SwiUpkGsFeatureCustomizations (swi_uint8 *paramfieldp, 
                                           struct SWI_STRUCT_FeatureCustomizations * sFeaturesp);


/* SwiIntPckCdmaGeneral.c */
extern swi_uint32 SwiPackModePref( 
    swi_uint8 *paramfieldp, 
    swi_uint16 modepref,
    swi_uint16 modeterm,
    swi_uint32 modedur,
    swi_uint32 bandpref,
    swi_uint16 roampref );
extern swi_uint32 SwiPackSidNid( 
    swi_uint8 *paramfieldp, 
    const struct SWI_STRUCT_HomeSidNid *sidnidp, 
    swi_uint16 Nam );
extern SWI_API SWI_RCODE SwiSetTempModePref(
    swi_uint16 nModePref, 
    swi_uint16 nModeTerm, 
    swi_uint32 nModeDur, 
    swi_uint32 nBandPref, 
    swi_uint16 nRoamPref,
    swi_uint32 timeout);

/* SwiIntUpkGsmNetwork.c */
extern void SwiUpkRssiGsm( 
    swi_uint8 *paramfieldp, 
    struct SWI_STRUCT_Rssi *rssip );
extern void SwiUpkByteCounters( 
    swi_uint8 *paramfieldp, 
    swi_uint32 *sentp, 
    swi_uint32 *receivedp);
extern void SwiUpkGsProfileSummary(
    swi_uint8 *paramfieldp, 
    struct SWI_STRUCT_GsmProfileSummary* pProfileSummary);
extern swi_bool SwiUpkGsProfileBasic(
    swi_uint8 *paramfieldp, 
    struct SWI_STRUCT_GsmProfileBasic* pProfile);
extern swi_bool SwiUpkGsProfileQos(
    swi_uint8 *paramfieldp, 
    struct SWI_STRUCT_GsmProfileQos* pProfile);
extern swi_bool SwiUpkGsProfileDns(
    swi_uint8 *paramfieldp, 
    struct SWI_STRUCT_GsmProfileDns* pProfile);
extern swi_bool SwiUpkGsProfileTft(
    swi_uint8 *paramfieldp, 
    struct SWI_STRUCT_GsmProfileTft* pProfile);
extern void SwiUpkGsProfileChange( 
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_GsmProfileChange* pProfileChange);
extern void SwiUpkGsPktSessionStatus(
    swi_uint8 *paramfieldp, 
    struct SWI_STRUCT_PktSessionStatus *pSessionStatus);
extern void SwiUpkGsPlmnMode( 
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_PlmnMode* pPlmnMode);
extern void SwiUpkGsPlmnReady( 
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_PlmnReady* pPlmnReady);
extern void SwiUpkGsRscpEcIo(
    swi_uint8 *paramfieldp, 
    struct SWI_STRUCT_RscpEcIo* pRscpEcIo);
extern void SwiUpkGsSystemStatus(
    swi_uint8 *paramfieldp, 
    struct SWI_STRUCT_GsmSystemStatus* pSysStatus);
extern void SwiUpkGsIPAddress(
    swi_uint8 *paramfieldp, 
    struct SWI_STRUCT_IpAddress* pIp);
extern void  SwiUpkGsPLMNSelection( 
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_PlmnSelection* pPlmn);
extern void  SwiUpkGsRegistration(
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_Registration *pRegistration);
extern void  SwiUpkGsServiceIcon( 
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_ServiceIcon* pServiceIcon);
extern void  SwiUpkGsPlmnAvailable( 
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_PlmnAvailable* pPlmn);
extern void  SwiUpkGsNetworkStatus( 
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_GSM_NetworkStatus* pNetStatus);
extern void  SwiUpkGsPktSessionStatusExp(
    swi_uint8 *paramfieldp, 
    struct SWI_STRUCT_PktSessionCall *pPktSessionCall);
extern void  SwiUpkGsCellInfo (
    swi_uint8 *paramfieldp, 
    struct SWI_STRUCT_GsmCellInfo *pCellInfo,
    swi_bool *versionp); 
extern void SwiUpkGsGetRat (
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_Rat *psRat,
    swi_bool *pbversion);
extern void SwiUpkGsSetRat (
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_Rat *psRat,
    swi_bool *pbversion);
extern void SwiUpkGsGetPrlRegion (
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_PrlRegion *psPrl,
    swi_bool *pbversion);
extern void SwiUpkGsSetPrlRegion (
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_PrlRegion *psPrl,
    swi_bool *pbversion);
extern swi_bool SwiUpkGsProfileAuth(
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_GsmProfileBasic* pProfile);

/* SwiIntPkGsmNetwork.c */
extern swi_uint16  SwiPkGsProfileBasic( 
    struct SWI_STRUCT_GsmProfileBasic sProfile, 
    swi_uint8 *paramfieldp);
extern swi_uint16  SwiPkGsProfileQos( 
    struct SWI_STRUCT_GsmProfileQos  sProfile, 
    swi_uint8 *paramfieldp);
extern swi_uint16  SwiPkGsProfileDns( 
    struct SWI_STRUCT_GsmProfileDns  sProfile, 
    swi_uint8 *paramfieldp);
extern swi_uint16  SwiPkGsProfileTft( 
    struct SWI_STRUCT_GsmProfileTft  sProfile, 
    swi_uint8 *paramfieldp);
extern swi_uint16 SwiPkGsProfileAuth(
    struct SWI_STRUCT_GsmProfileBasic *pProfile,
    swi_uint8 *paramfieldp);

/* SwiIntDecodeNotify.c */
extern SWI_RCODE swidecodeairchgnotif(
    struct amairserverparms *amairparmsp,
    struct SwiNotifyVariant *notifyvariantp);
extern SWI_RCODE SwiDecodeCnsNotify(
    struct amcnsrrparms   apamcnsrrparm,
    swi_uint8 *paramfldp,
    struct SwiNotifyVariant *notifyvariantp);
extern SWI_RCODE SwiDecodeFwDwnNotify(
    struct amfwdwnrrparms   apamfwdwnrrparm,
    struct SwiNotifyVariant *notifyvariantp);

/* SwiIntPkGsmSim.c */
extern swi_uint16 SwiPkGsmSimVerify( 
    swi_uint8                   *paramp, 
    const struct SWI_STRUCT_SIM_Verify *simp);
extern swi_uint16 SwiPkGsmSimLock(
    swi_uint8 *paramp, 
    const struct SWI_STRUCT_SIM_EnableLock *simp);
extern swi_uint16 SwiPkGsmSimPassword(
    swi_uint8                   *paramp, 
    const struct SWI_STRUCT_SIM_Password *simp);
extern swi_uint16 SwiPkChv2StatusKick(
    swi_uint8                      *paramp, 
    enum SWI_TYPE_SIM_Chv2KickType kicktype);

/* SwiIntUpkGsmSim.c */
extern void SwiUpkGsmSimLock( 
    swi_uint8 *paramfieldp, 
    struct SWI_STRUCT_SIM_LockStatus *simp );
extern void SwiUpkGsmSimStatus(
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_SIM_StatusExp *simp );
extern void SwiUpkGsmIMSI(
    swi_uint8 *paramfieldp,
    swi_charp MSINp, 
    swi_uint32 *MCCp, 
    swi_uint32 *MNCp);
extern void SwiUpkIccId(
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_IccId *sIccId);
extern void SwiUpkSpn(
    swi_uint8 *paramfieldp,
    swi_charp spnp,
    swi_uint32 maxspnlength);
extern void SwiUpkGsSimAuth(
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_SimAuth  *pSimAuth);
    
/* SwiIntPkCmUpgrade.c */
extern swi_uint16 SwiPkNvStore( 
    swi_uint8                 *paramp, 
    swi_uint16                objversion,
    struct SWI_STRUCT_NvStore *nvstorep);

/* SwiIntUpkCmUpgrade.c */
extern void SwiUpkNvStore( 
    swi_uint8                 *paramp, 
    struct SWI_STRUCT_NvStore *nvstorep);

/* SwiIntPkCmBasic.c */
extern swi_uint16 SwiPkRadioPower( 
    swi_uint8                    *paramp, 
    swi_uint16                   objversion,
    struct SWI_STRUCT_RadioPower *radiop);
extern swi_uint16 SwiPkRadioBandCfg( 
    swi_uint8                    *paramp, 
    swi_uint16                   objversion,
    swi_uint64                   radiobandgroup);
extern swi_uint16 SwiPkFlashImgInfo( 
    swi_uint8                      *paramp, 
    swi_uint16                     objversion,
    struct SWI_STRUCT_FlashImgInfo *FlashImgInfop);

extern swi_uint32 SwiPackNVUpdateFromFile( 
        swi_uint8 *paramfieldp, 
        SWI_STRUCT_NVUpdateFromFile *pNVUpdateFromFile );

extern swi_uint32 SwiPackApiUnlock( swi_uint8 *paramfieldp, 
                                   const SWI_STRUCT_ApiUnlock *pApiUnlock );

extern swi_uint32 SwiPackRemoteDiagAgentEnable(swi_uint8 *paramfieldp, 
                                               swi_uint8 bEnable);

extern swi_uint32 SwiPackRemovableMediaSupport(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_RM_CONFIG_SET *psRM);

extern swi_uint16 SwiPkAudioProfile(
    swi_uint8                            *paramp, 
    swi_uint16                           objversion,
    const struct SWI_STRUCT_AudioProfile *pAudioProfile);


/* SwiIntUpkCmBasic.c */
extern void SwiUpkRadioPowerNotify(
    swi_uint8                    *paramp, 
    struct SWI_STRUCT_RadioPower *radiop);

extern void SwiUpkRadioPowerResp(
    swi_uint8                    *paramp, 
    struct SWI_STRUCT_RadioPower *radiop);

extern void SwiUpkRadioTemp(
    swi_uint8                        *paramp, 
    struct SWI_STRUCT_RadioTempState *radiop);

extern void SwiUpkRadioBattery(
    swi_uint8                       *paramp, 
    struct SWI_STRUCT_RadioBattery  *radiop);

extern void SwiUpkCurrentRadioBand(
    swi_uint8                       *paramp, 
    struct SWI_STRUCT_RadioBandList *radiop);

extern void SwiUpkRadioBandCfg(
    swi_uint8                         *paramp, 
    struct SWI_STRUCT_RadioBandConfig *radiop);

extern void SwiUpkPriInfo( 
    swi_uint8                 *paramp, 
    struct SWI_STRUCT_PriInfo *priinfop);
    
extern void SwiUpkModemType( 
    swi_uint8                    *paramp, 
    struct SWI_STRUCT_ModemType  *pModemType);
    
extern void SwiUpkDeviceTime( 
    swi_uint8                    *paramp, 
    struct SWI_STRUCT_DeviceTime *pDeviceTime);
    
extern void SwiUpkFlashImgInfoResp(
    swi_uint8                      *paramp, 
    struct SWI_STRUCT_FlashImgInfo *FlashImgInfop);

extern swi_bool SwiUpkObjRngRev(
    swi_uint8                   *paramp, 
    struct SWI_STRUCT_ObjRngRev *ObjRngRevp);
    
extern swi_bool SwiUpkUsbdInfo(
    swi_uint8                   *paramp,
    struct SWI_STRUCT_UsbdInfo  *UsbdInfop);

extern void SwiUnpackNVUpdateFromFile(
    swi_uint8 *paramfieldp, 
    SWI_STRUCT_NVUpdateFromFile *pNVUpdateFromFile);

extern void SwiUnpackRemoteDiagAgentSupport(
    swi_uint8 *paramfieldp, 
    SWI_STRUCT_RDA_SUPPORT *psRDA);

extern void SwiUpkAudioProfile(
    swi_uint8 *paramfieldp, 
    struct SWI_STRUCT_AudioProfile *pAudioProfile );
    
extern void SwiUpkAudioProfileResp(
    swi_uint8 *paramfieldp, 
    struct SWI_STRUCT_AudioProfile *pAudioProfile );

/* SwiIntPkGsmLbs.c */
extern swi_uint16 SwiPkLbsFixSettings(
    swi_uint8                        *paramp, 
    swi_uint16                       objversion,
    struct SWI_STRUCT_LbsFixSettings *settingsp);
extern swi_uint16 SwiPkLbsNiRequest(
    swi_uint8                *paramp,
    swi_uint16               objversion,
    struct SWI_STRUCT_LbsNiReq *requestp);  

/* SwiIntPkGsmVoice.c */
extern swi_uint16 SwiPkGsmNVToneStore(
    swi_uint8 *paramp, 
    const struct SWI_STRUCT_NVToneStore *pToneLength);
extern swi_uint16 SwiPkAudioProfile(
    swi_uint8                            *paramp, 
    swi_uint16                           objversion,
    const struct SWI_STRUCT_AudioProfile *pAudioProfile);
extern swi_uint16 SwiPkGsmCallCreate(
    swi_uint8                           *paramp, 
    const struct SWI_STRUCT_CALL_Create *pCall);
extern swi_uint16 SwiPkGsmDTMFSend(
    swi_uint8                           *paramp, 
    const struct SWI_STRUCT_DTMFSend    *pDTMFSend);
extern swi_uint16 SwiPkGsmCallControl(
    swi_uint8                           *paramp, 
    const struct SWI_STRUCT_CALL_UMTS_Control    *pCall);
extern swi_uint16 SwiPkGsmSetContDtmf(
    swi_uint8                           *paramp, 
    const struct SWI_STRUCT_ContDtmf    *pDtmf);
extern swi_uint16 SwiPkGsmSetSSRequest(
    swi_uint8                           *paramp, 
    const struct SWI_STRUCT_SS_Request  *sSSRequest);
extern swi_uint16 SwiPkGsmAbortRequest(
    swi_uint8                           *paramp, 
    const struct SWI_STRUCT_SS_Abort    *sSSAbort);
extern swi_uint16 SwiPkGsmStartSSPassReg(
    swi_uint8                           *paramp, 
    const struct SWI_STRUCT_SS_PasswordReg    *sSSPassReg);
extern swi_uint16 SwiPkGsmSetSSPassword(
    swi_uint8                           *paramp, 
    const struct SWI_STRUCT_SS_PasswordStatus    *sSSPassStatus);
    
/* SwiIntUpkGsmVoice.c */
extern void SwiUpkGsmNVToneStore(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_NVToneStore *pToneLength);
extern void SwiUpkAudioProfile(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_AudioProfile *pAudioProfile );
extern void SwiUpkAudioProfileResp(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_AudioProfile *pAudioProfile );
extern void SwiUpkGsmVoiceMessage(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_VoiceMessage *pVoiceMessage );
extern void SwiUpkGsmCallExpProgress(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_CALL_EXP_Progress *pCall );
extern void SwiUpkGsmCallForwarded(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_CallForwarded *pForward );
extern void SwiUpkGsmCallStatus(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_CALL_Status *pStatus );
extern void SwiUpkGsmSetSSRequest(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_SS_Request *sSSRequest );
extern void SwiUpkGsmStartSSPassReg(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_SS_PasswordReg *sSSPassReg );
extern void SwiUpkGsmSetSSPassword(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_SS_PasswordStatus *sSSPassStatus );
extern void SwiUpkGsmSSFwdInfoRsp(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_SS_RespFwdInfo *sSSFwdInfo );
extern void SwiUpkGsmSSCallBarRsp(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_SS_RespCallBar *sSSCallBar );
extern void SwiUpkGsmSSInfoDataRsp(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_SS_RespInfoData *sSSInfoData );
extern void SwiUpkGsmSSStatusRsp(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_SS_RespStatus *sSSStatus );
extern void SwiUpkGsmSSFwdNumRsp(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_SS_RespFwdNum *sSSFwdNum );
extern void SwiUpkGsmSSBSListRsp(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_SS_RespBSList *sSSBSList );
extern void SwiUpkGsmSSFwdListRsp(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_SS_RespListFwdInfo *sSSFwdList );
extern void SwiUpkGsmSSRawRsp(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_SS_RespRaw *sSSRaw );
extern void SwiUpkGsmSSPwdRsp(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_SS_RespPassword *sSSPwd );
extern void SwiUpkGsmSSClirRsp(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_SS_RespClir *sSSClir );
extern void SwiUpkGsmSSErrorRsp(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_SS_RespError *sSSError );
extern void SwiUpkGsmSSRejectRsp(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_SS_RespReject *sSSReject );
extern void SwiUpkGsmSSCancelRsp(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_SS_RespCancel *sSSCancel );

/* SwiIntUpkGsmLbs.c */
extern void SwiUpkLbsPaWarmStatus(
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_LbsPaWarmStatus *warmp);
extern void SwiUpkLbsFixSettingsGetResp(
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_LbsFixSettings *settingsp);
extern void SwiUpkLbsFixSettingsSetResp(
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_LbsFixSettings *settingsp);
extern void SwiUpkLbsNiRequest(
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_LbsNiReq *requestp);
extern void SwiUpkLbsNiReqNotify(
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_LbsNiReq *requestp);

/* SwiIntPkGsmStk.c */
extern swi_uint16 SwiPkGsStkCmd( 
    struct SWI_STRUCT_STK_Command *STKCommandp, 
    swi_uint8 *paramfieldp);
extern swi_uint16 SwiPkGsStkRespGetInput( 
     const struct SWI_STRUCT_STK_RsepGetInput *RespGetInputp, 
     swi_uint8 *paramfieldp);

/* SwiIntUpkGsmStk.c */
extern void SwiUpkGsStkCmd(
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_STK_Command *STKCommandp);
extern void SwiUpkGsStkCmdNotif(
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_STK_Notif_Command *STKCmdNotifp);

/* SwiIntPkGsmPhonebook.c */
extern swi_uint16 SwiPkGsPhbkAdd( 
    struct SWI_STRUCT_PHONEBK_Entry *sPhonebookp, 
    swi_uint8 *paramfieldp);
extern swi_uint16 SwiPkGsPhbkEdit( 
    struct SWI_STRUCT_PHONEBK_Entry *sPhonebookp,
    swi_uint8 *paramfieldp);
    

/* SwiIntUpkGsmPhonebook.c */
extern void SwiUpkGsPhbkGetEntry(
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_PHONEBK_Entry* sPhonebookp );
extern void SwiUpkGsPhbkAvail(
    swi_uint8 *paramfieldp, 
     struct SWI_STRUCT_PHONEBK_Available* sPhonebookp );
extern void SwiUpkGsPhbkSize(
    swi_uint8 *paramfieldp, 
     struct SWI_STRUCT_PHONEBK_Size* sPhonebookszp );
extern void SwiUpkGsPhbkGetEmerNum(
    swi_uint8 *paramfieldp, 
     struct SWI_STRUCT_Emergency_Number * sPhonebookp );

/* SwiIntUpkCmDm.h */
extern void SwiUnpackOMADMConfig(swi_uint8 *paramfieldp, 
                                 SWI_STRUCT_OMADM_GET_CONFIG *psConfig);

extern void SwiUnpackOMADMSessionState(swi_uint8 *paramfieldp, 
                                SWI_STRUCT_OMADM_SESSION_STATE *psState);

extern void SwiUnpackOMADMNIAlert(swi_uint8 *paramfieldp, 
                           SWI_STRUCT_OMADM_NI_ALERT *psAlert);

extern void SwiUnpackOMADMUIAlertConfirm(swi_uint8 *paramfieldp);
extern void SwiUnpackOMADMUIAlertInput(swi_uint8 *paramfieldp);
extern void SwiUnpackOMADMUIAlertChoice(swi_uint8 *paramfieldp);

extern void SwiUnpackOMADMNodeValue(swi_uint8 *paramfieldp, 
                             SWI_STRUCT_OMADM_NODE_VALUE_RESPONSE *psValue);

extern void SwiUnpackDMDLProgress(swi_uint8 *paramfieldp, 
                           SWI_STRUCT_DM_DL_PROGRESS *psProgress);

extern SWI_RCODE SwiUnpackDMDLAllert(swi_uint8 *paramfieldp, 
                              SWI_STRUCT_DM_DL_ALERT *psAlert);

extern SWI_RCODE SwiUnpackDMDLDescription(swi_uint8 *paramfieldp, 
                                   SWI_STRUCT_DM_DL_DESCRIPTION *psDescription);

extern void SwiUnpackDMDLInstallStatus(swi_uint8 *paramfieldp, 
                                SWI_STRUCT_DM_DL_INSTALL_STATUS *psStatus);

/* SwiIntPkCmDm.h */
extern swi_uint32 SwiPackOMADMConfig(swi_uint8 *paramfieldp, 
                              const SWI_STRUCT_OMADM_SET_CONFIG *psConfig);

extern swi_uint32 SwiPackStartOMADMSession(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_OMADM_START_SESSION *psSession);

extern swi_uint32 SwiPackOMADMNIAlertResponse(swi_uint8 *paramfieldp, 
                                       swi_uint16 nResponse);

extern swi_uint32 SwiPackOMADMUIAlertConfirm(swi_uint8 *paramfieldp);
extern swi_uint32 SwiPackOMADMUIAlertInput(swi_uint8 *paramfieldp);
extern swi_uint32 SwiPackOMADMUIAlertChoice(swi_uint8 *paramfieldp);
extern swi_uint32 SwiPackGetOMADMNodeValue(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_OMADM_NODE_VALUE_REQUEST *psItem);

extern swi_uint32 SwiPackSetOMADMNodeValue(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_OMADM_NODE_VALUE *psValue);

extern swi_uint32 SwiPackDMDLAlertResponse(swi_uint8 *paramfieldp, 
                                    swi_uint8 nResponse);

/* SwiIntUtil.c */
extern SWI_RCODE SwiIntSendnWaitAirServerRequest(
    swi_uint8                    reqtype,
    swi_bool                     *airserverpresentp,
    struct SWI_STRUCT_AirServer  *airserverp );
extern SWI_RCODE SwiIntSendnWait(
    swi_uint8           *reqbufferp,
    swi_uint16          objid,
    swi_uint16          length,
    swi_uint32          timeout,
    swi_uint8           operation,
    swi_uint8           airint, 
    swi_uint8           **inparampp,
    swi_uint16          *parmlength );
extern SWI_RCODE SwiSendnWaitFwdDwnRequest(
    char                   *filenamep,
    enum swi_amfwimagetype imagetype,
    swi_bool               mpdownload,
    swi_uint32             maxchunksize);
extern swi_bool SwiIntUcs2Unpack(
    swi_uint8 **parmfpp,
    swi_charp destp,
    swi_uint32 maxbytes,
    swi_uint8 numchars,
    swi_uint8 fieldsize);
extern swi_bool swigsmdualrangeuser(void);
extern SWI_API SWI_RCODE SwiIntMapProdStrToDevType( 
    swi_uint8 *prodstrp,
    swi_uint32 *devtypep);
extern swi_uint32 SwiPackU8s( 
                              swi_uint8 *paramfieldp, 
                              const swi_uint8 *pnValue, 
                              swi_uint16 nNum );

extern swi_uint32 SwiPackU16s( 
                               swi_uint8 *paramfieldp, 
                               const swi_uint16 *pnValue, 
                               swi_uint16 nNum );

extern swi_uint32 SwiPackU16( swi_uint8 *paramfieldp, swi_uint16 nValue );

extern swi_uint32 SwiPackU16U8s( 
                                 swi_uint8 *paramfieldp, 
                                 swi_uint16 nValue, 
                                 const swi_uint8 *pnValue, 
                                 swi_uint16 nNum );

extern swi_uint32 SwiPackU16U16( 
                                 swi_uint8 *paramfieldp, 
                                 swi_uint16 nValue1, 
                                 swi_uint16 nValue2 );

extern swi_uint32 SwiPackU32s( 
                               swi_uint8 *paramfieldp, 
                               const swi_uint32 *pnValue, 
                               swi_uint16 nNum );

extern swi_uint32 SwiPackU32( swi_uint8 *paramfieldp, swi_uint32 nValue );
extern swi_uint32 SwiPackNv( swi_uint8 *paramfieldp, swi_uint16 nNvIndex );
extern swi_uint32 SwiPackNam( swi_uint8 *paramfieldp, swi_uint16 nNam );

extern swi_uint32 SwiPackNvU8s( 
                                swi_uint8 *paramfieldp, 
                                swi_uint16 nNvIndex, 
                                const swi_uint8 *pnValue, 
                                swi_uint16 nNum );

extern swi_uint32 SwiPackNvU16s( 
                                 swi_uint8 *paramfieldp, 
                                 swi_uint16 nNvIndex, 
                                 const swi_uint16 *pnValue, 
                                 swi_uint16 nNum );

extern swi_uint32 SwiPackNvU16( 
                                swi_uint8 *paramfieldp, 
                                swi_uint16 nNvIndex, 
                                swi_uint16 nValue );

extern swi_uint32 SwiPackNvU32s( 
                                 swi_uint8 *paramfieldp, 
                                 swi_uint16 nNvIndex, 
                                 const swi_uint32 *pnValue, 
                                 swi_uint16 nNum );

extern swi_uint32 SwiPackNvNamU16s( 
                                    swi_uint8 *paramfieldp, 
                                    swi_uint16 nNvIndex, 
                                    swi_uint16 nNamIndex, 
                                    const swi_uint16 *pnValue, 
                                    swi_uint16 nNum );

extern swi_uint32 SwiPackNvNamU32s( 
                                    swi_uint8 *paramfieldp, 
                                    swi_uint16 nNvIndex, 
                                    swi_uint16 nNamIndex, 
                                    const swi_uint32 *pnValue, 
                                    swi_uint16 nNum );

extern swi_uint32 SwiPackNvNam( 
                                swi_uint8 *paramfieldp, 
                                swi_uint16 nNvIndex, 
                                swi_uint16 nNamIndex );

extern swi_uint32 SwiPackNvU16U16( 
                                   swi_uint8 *paramfieldp, 
                                   swi_uint16 nNvIndex, 
                                   swi_uint16 nValue1, 
                                   swi_uint16 nValue2 );

extern swi_uint32 SwiPackString( 
                                 swi_uint8 *paramfieldp, 
                                 const swi_charp pszString, 
                                 swi_uint16 szLength );

extern swi_uint32 SwiPackU16sString( 
                                     swi_uint8 *paramfieldp, 
                                     const swi_uint16 *pnValue, 
                                     swi_uint16 nNum, 
                                     const swi_charp pszString, 
                                     swi_uint16 szLength );

extern swi_uint32 SwiPackNvString( 
                                   swi_uint8 *paramfieldp, 
                                   swi_uint16 nNvIndex, 
                                   const swi_charp pszString, 
                                   swi_uint16 szLength );

extern swi_uint32 SwiPackNvNamString( 
                                      swi_uint8 *paramfieldp, 
                                      swi_uint16 nNvIndex, 
                                      swi_uint16 nNamIndex, 
                                      const swi_charp pszString, 
                                      swi_uint16 szLength );

extern swi_uint32 SwiPackU16sU8LenString(
                                          swi_uint8 *paramfieldp, 
                                          const swi_uint16 *pnValue, 
                                          swi_uint16 nNum, 
                                          const swi_charp pszString, 
                                          swi_uint8 szLength );

extern swi_uint32 SwiPackU16sU16LenString( 
                                           swi_uint8 *paramfieldp, 
                                           const swi_uint16 *pnValue, 
                                           swi_uint16 nNum, 
                                           const swi_charp pszString, 
                                           swi_uint16 szLength );

extern swi_uint32 SwiPackNvU16LenString( 
                                         swi_uint8 *paramfieldp, 
                                         swi_uint16 nNvIndex, 
                                         const swi_charp pszString, 
                                         swi_uint16 szLength );

extern swi_uint32 SwiPackNamU16LenString( 
                                            swi_uint8 *paramfieldp, 
                                            swi_uint16 nNamIndex, 
                                            const swi_charp pszString, 
                                            swi_uint16 szLength );

extern swi_uint32 SwiPackNvNamU16LenString( 
                                            swi_uint8 *paramfieldp, 
                                            swi_uint16 nNvIndex, 
                                            swi_uint16 nNamIndex, 
                                            const swi_charp pszString, 
                                            swi_uint16 szLength );

extern void SwiUnpackString( 
                             swi_uint8 *paramfieldp, 
                             swi_charp pszString, 
                             swi_uint32 szLength );

extern void SwiUnpackNvString( 
                               swi_uint8 *paramfieldp, 
                               swi_charp pszString, 
                               swi_uint32 szLength );

extern void SwiUnpackNvNamString( 
                                  swi_uint8 *paramfieldp, 
                                  swi_charp pszString, 
                                  swi_uint32 szLength );

extern void SwiUnpackU16sU16LenString( 
                                       swi_uint8 *paramfieldp, 
                                       swi_uint16 *pnValue, 
                                       swi_uint16 nNum, 
                                       swi_charp pszString, 
                                       swi_uint32 szLength );

extern void SwiUnpackNvU16LenString( 
                                     swi_uint8 *paramfieldp, 
                                     swi_charp pszString, 
                                     swi_uint32 szLength );

extern void SwiUnpackNamString( 
                                swi_uint8 *paramfieldp, 
                                swi_charp pszString, 
                                swi_uint32 szLength );

extern void SwiUnpackNamU16LenString( 
                                      swi_uint8 *paramfieldp, 
                                      swi_charp pszString, 
                                      swi_uint32 szLength );

extern void SwiUnpackNvNamU16LenString( 
                                        swi_uint8 *paramfieldp, 
                                        swi_charp pszString, 
                                        swi_uint32 szLength );

extern void SwiUnpackNvU8s( 
                            swi_uint8 *paramfieldp, 
                            swi_uint8 *pnValue, 
                            swi_uint16 nNum );

extern void SwiUnpackNvU16s( 
                             swi_uint8 *paramfieldp, 
                             swi_uint16 *pnValue, 
                             swi_uint16 nNum );

extern void SwiUnpackNvU32s( 
                             swi_uint8 *paramfieldp, 
                             swi_uint32 *pnValue, 
                             swi_uint16 nNum );

extern void SwiUnpackNvNamU8s( 
                               swi_uint8 *paramfieldp, 
                               swi_uint8 *pnValue, 
                               swi_uint16 nNum );

extern void SwiUnpackNvNamU16s( 
                                swi_uint8 *paramfieldp, 
                                swi_uint16 *pnValue, 
                                swi_uint16 nNum );

extern void SwiUnpackNvNamU32s( 
                                swi_uint8 *paramfieldp, 
                                swi_uint32 *pnValue, 
                                swi_uint16 nNum );


extern SWI_API SWI_RCODE SwiIntSendPRLBlock (swi_uint16 namIndex, 
                                      swi_uint16 packetSeqNumber,
                                      swi_uint16 lenByteArray,
                                      swi_uint8 *pSegmentBody,
                                      swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntSendFileBody (const swi_charp pszEFilename, 
                                      swi_uint16 packetSeqNumber,
                                      swi_uint16 totalNumberPackets,
                                      swi_uint32 lenByteArray,
                                      swi_uint8  *pSegmentBody,
                                      swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntGetString(swi_uint16 nObjectID, 
                                         swi_charp pszString, 
                                         swi_uint32 szLength, 
                                         swi_uint16 nMinSize, 
                                         swi_uint16 nFixSize, 
                                         swi_uint8 airint, 
                                         swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntGetNvString(swi_uint16 nObjectID, 
                                           swi_uint16 nNvIndex, 
                                           swi_charp pszString, 
                                           swi_uint32 szLength, 
                                           swi_uint16 nMinSize, 
                                           swi_uint16 nFixSize, 
                                           swi_uint8 airint, 
                                           swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntSetU16sString(swi_uint16 nObjectID, 
                                             swi_uint16 *pnValue, 
                                             swi_uint16 nNum, 
                                             const swi_charp pszString, 
                                             swi_uint16 nMaxSize, 
                                             swi_uint16 nFixSize, 
                                             swi_uint8 airint, 
                                             swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntSetU16String(swi_uint16 nObjectID, 
                                            swi_uint16 nValue, 
                                            const swi_charp pszString, 
                                            swi_uint16 nMaxSize, 
                                            swi_uint16 nFixSize, 
                                            swi_uint8 airint, 
                                            swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntSetU16sU8LenString(swi_uint16 nObjectID, 
                                                  swi_uint16 *pnValue, 
                                                  swi_uint16 nNum, 
                                                  const swi_charp pszString, 
                                                  swi_uint16 nMaxSize, 
                                                  swi_uint16 nFixSize, 
                                                  swi_uint8 airint, 
                                                  swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntSetU16sU16LenString(swi_uint16 nObjectID, 
                                                   swi_uint16 *pnValue, 
                                                   swi_uint16 nNum, 
                                                   const swi_charp pszString, 
                                                   swi_uint16 nMaxSize, 
                                                   swi_uint16 nFixSize, 
                                                   swi_uint8 airint, 
                                                   swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntSetNvString(swi_uint16 nObjectID, 
                                           swi_uint16 nNvIndex, 
                                           const swi_charp pszString, 
                                           swi_uint16 nMaxSize, 
                                           swi_uint16 nFixSize, 
                                           swi_uint8 airint, 
                                           swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntSetNvNamString(swi_uint16 nObjectID, 
                                              swi_uint16 nNvIndex, 
                                              swi_uint16 namIndex, 
                                              const swi_charp pszString, 
                                              swi_uint16 nMaxSize, 
                                              swi_uint16 nFixSize, 
                                              swi_uint8 airint, 
                                              swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntSetNvU16LenString(swi_uint16 nObjectID, 
                                                 swi_uint16 nNvIndex, 
                                                 const swi_charp pszString, 
                                                 swi_uint16 nMaxSize, 
                                                 swi_uint16 nFixSize, 
                                                 swi_uint8 airint, 
                                                 swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntSetNamU16LenString(swi_uint16 nObjectID, 
                                                    swi_uint16 namIndex, 
                                                    const swi_charp pszString, 
                                                    swi_uint16 nMaxSize, 
                                                    swi_uint16 nFixSize, 
                                                    swi_uint8 airint, 
                                                    swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntSetNvNamU16LenString(swi_uint16 nObjectID, 
                                                    swi_uint16 nNvIndex, 
                                                    swi_uint16 namIndex, 
                                                    const swi_charp pszString, 
                                                    swi_uint16 nMaxSize, 
                                                    swi_uint16 nFixSize, 
                                                    swi_uint8 airint, 
                                                    swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntSetU32s(swi_uint16 nObjectID, 
                                       swi_uint32 *pnValue, 
                                       swi_uint16 nNum, 
                                       swi_uint8 airint, 
                                       swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntSet(swi_uint16 nObjectID, 
                                   swi_uint8 airint, 
                                   swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntSetU8s(swi_uint16 nObjectID, 
                                      swi_uint8 *pnValue, 
                                      swi_uint16 nNum, 
                                      swi_uint8 airint, 
                                      swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntSetU8(swi_uint16 nObjectID, 
                                     swi_uint8 nValue, 
                                     swi_uint8 airint, 
                                     swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntSetU16s(swi_uint16 nObjectID, 
                                       swi_uint16 *pnValue, 
                                       swi_uint16 nNum, 
                                       swi_uint16 *pnResult, 
                                       swi_uint8 airint, 
                                       swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntSetU16(swi_uint16 nObjectID, 
                                      swi_uint16 nValue, 
                                      swi_uint16 *pnResult, 
                                      swi_uint8 airint, 
                                      swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntSetNvU8s(swi_uint16 nObjectID, 
                                        swi_uint16 nNvIndex, 
                                        swi_uint8 *pnValue, 
                                        swi_uint16 nNum, 
                                        swi_uint8 airint, 
                                        swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntSetNvU8(swi_uint16 nObjectID, 
                                       swi_uint16 nNvIndex, 
                                       swi_uint8 nValue, 
                                       swi_uint8 airint, 
                                       swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntSetNvU16s(swi_uint16 nObjectID, 
                                         swi_uint16 nNvIndex, 
                                         swi_uint16 *pnValue, 
                                         swi_uint16 nNum, 
                                         swi_uint8 airint, 
                                         swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntSetNvU16(swi_uint16 nObjectID, 
                                        swi_uint16 nNvIndex, 
                                        swi_uint16 nValue, 
                                        swi_uint8 airint, 
                                        swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntSetNvNamU16s(swi_uint16 nObjectID, 
                                            swi_uint16 nNvIndex, 
                                            swi_uint16 namIndex, 
                                            swi_uint16 *pnValue, 
                                            swi_uint16 nNum, 
                                            swi_uint8 airint, 
                                            swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntSetNvNamU16(swi_uint16 nObjectID, 
                                           swi_uint16 nNvIndex, 
                                           swi_uint16 namIndex, 
                                           swi_uint16 nValue, 
                                           swi_uint8 airint, 
                                           swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntSetNvNamU32s(swi_uint16 nObjectID, 
                                            swi_uint16 nNvIndex, 
                                            swi_uint16 namIndex, 
                                            swi_uint32 *pnValue, 
                                            swi_uint16 nNum, 
                                            swi_uint8 airint, 
                                            swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntSetNvNamU32(swi_uint16 nObjectID, 
                                           swi_uint16 nNvIndex, 
                                           swi_uint16 namIndex, 
                                           swi_uint32 nValue, 
                                           swi_uint8 airint, 
                                           swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntSetNvU32s(swi_uint16 nObjectID, 
                                         swi_uint16 nNvIndex, 
                                         swi_uint32 *pnValue, 
                                         swi_uint16 nNum, 
                                         swi_uint8 airint, 
                                         swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntSetNvU32(swi_uint16 nObjectID, 
                                        swi_uint16 nNvIndex, 
                                        swi_uint32 nValue, 
                                        swi_uint8 airint, 
                                        swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntGetNvNamString(swi_uint16 nObjectID, 
                                              swi_uint16 nNvIndex, 
                                              swi_uint16 namIndex, 
                                              swi_charp pszString, 
                                              swi_uint32 szLength, 
                                              swi_uint16 nMinSize, 
                                              swi_uint16 nFixSize, 
                                              swi_uint8 airint, 
                                              swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntGetNvU16LenString(swi_uint16 nObjectID, 
                                                 swi_uint16 nNvIndex, 
                                                 swi_charp pszString, 
                                                 swi_uint32 szLength, 
                                                 swi_uint16 nMinSize, 
                                                 swi_uint16 nFixSize, 
                                                 swi_uint8 airint, 
                                                 swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntGetNamString(swi_uint16 nObjectID, 
                                            swi_uint16 namIndex, 
                                            swi_charp pszString, 
                                            swi_uint32 szLength, 
                                            swi_uint16 nMinSize, 
                                            swi_uint16 nFixSize, 
                                            swi_uint8 airint, 
                                            swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntGetNamU16LenString(swi_uint16 nObjectID, 
                                                  swi_uint16 namIndex, 
                                                  swi_charp pszString, 
                                                  swi_uint32 szLength, 
                                                  swi_uint16 nMinSize, 
                                                  swi_uint16 nFixSize, 
                                                  swi_uint8 airint, 
                                                  swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntGetNvNamU16LenString(swi_uint16 nObjectID, 
                                                    swi_uint16 nNvIndex, 
                                                    swi_uint16 namIndex, 
                                                    swi_charp pszString, 
                                                    swi_uint32 szLength, 
                                                    swi_uint16 nMinSize, 
                                                    swi_uint16 nFixSize, 
                                                    swi_uint8 airint, 
                                                    swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntGetNvU8s(swi_uint16 nObjectID, 
                                        swi_uint16 nNvIndex, 
                                        swi_uint8 *pnValue, 
                                        swi_uint16 nNum, 
                                        swi_uint8 airint, 
                                        swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntGetNvU8(swi_uint16 nObjectID, 
                                       swi_uint16 nNvIndex, 
                                       swi_uint8 *pnValue, 
                                       swi_uint8 airint, 
                                       swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntGetU8s(swi_uint16 nObjectID, 
                                      swi_uint8 *pnValue, 
                                      swi_uint16 nNum, 
                                      swi_uint8 airint, 
                                      swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntGetU8(swi_uint16 nObjectID, 
                                     swi_uint8 *pnValue, 
                                     swi_uint8 airint, 
                                     swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntGetU16s(swi_uint16 nObjectID, 
                                       swi_uint16 *pnValue, 
                                       swi_uint16 nNum, 
                                       swi_uint8 airint, 
                                       swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntGetU16(swi_uint16 nObjectID, 
                                      swi_uint16 *pnValue, 
                                      swi_uint8 airint, 
                                      swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntGetU32s(swi_uint16 nObjectID, 
                                       swi_uint32 *pnValue, 
                                       swi_uint16 nNum, 
                                       swi_uint8 airint, 
                                       swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntGetU32(swi_uint16 nObjectID, 
                                      swi_uint32 *pnValue, 
                                      swi_uint8 airint, 
                                      swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntGetNvU16s(swi_uint16 nObjectID, 
                                         swi_uint16 nNvIndex, 
                                         swi_uint16 *pnValue, 
                                         swi_uint16 nNum, 
                                         swi_uint8 airint, 
                                         swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntGetNvU16(swi_uint16 nObjectID, 
                                        swi_uint16 nNvIndex, 
                                        swi_uint16 *pnValue, 
                                        swi_uint8 airint, 
                                        swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntGetNvU32s(swi_uint16 nObjectID, 
                                         swi_uint16 nNvIndex, 
                                         swi_uint32 *pnValue, 
                                         swi_uint16 nNum, 
                                         swi_uint8 airint, 
                                         swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntGetNvU32(swi_uint16 nObjectID, 
                                        swi_uint16 nNvIndex, 
                                        swi_uint32 *pnValue, 
                                        swi_uint8 airint, 
                                        swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntGetNvNamU8s(swi_uint16 nObjectID, 
                                           swi_uint16 nNvIndex,  
                                           swi_uint16 namIndex, 
                                           swi_uint8 *pnValue, 
                                           swi_uint16 nNum, 
                                           swi_uint8 airint, 
                                           swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntGetNvNamU16s(swi_uint16 nObjectID, 
                                            swi_uint16 nNvIndex,  
                                            swi_uint16 namIndex, 
                                            swi_uint16 *pnValue, 
                                            swi_uint16 nNum, 
                                            swi_uint8 airint, 
                                            swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntGetNvNamU16(swi_uint16 nObjectID, 
                                           swi_uint16 nNvIndex,  
                                           swi_uint16 namIndex, 
                                           swi_uint16 *pnValue, 
                                           swi_uint8 airint, 
                                           swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntGetNvNamU32s(swi_uint16 nObjectID, 
                                            swi_uint16 nNvIndex,  
                                            swi_uint16 namIndex, 
                                            swi_uint32 *pnValue, 
                                            swi_uint16 nNum, 
                                            swi_uint8 airint, 
                                            swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntGetNvNamU32(swi_uint16 nObjectID, 
                                           swi_uint16 nNvIndex,  
                                           swi_uint16 namIndex, 
                                           swi_uint32 *pnValue, 
                                           swi_uint8 airint, 
                                           swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiIntSetString(swi_uint16 nObjectID, 
                                         const swi_charp pszString, 
                                         swi_uint16 *pnResult, 
                                         swi_uint16 nMaxSize, 
                                         swi_uint16 nFixSize, 
                                         swi_uint8 airint, 
                                         swi_uint32 timeout);
extern swi_uint16 SwiGetObjid( enum SWI_TYPE_Notify eNotify );

/*
 * $Log: apiproto.h,v $
 */
