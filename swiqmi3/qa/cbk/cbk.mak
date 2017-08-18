#-------------------------------------------------------------------------------
#
#  Name:
#    cbk.mak
#
#  Description:
#    Makefile to build the pkgs/qa/cbk package
#
#   The following make targets are available in this makefile:
#
#     all           - make .o and .a image files (default)
#                     Test programs are also built when present
#     clean         - delete object directory and image files
#
#   The above targets can be made with the following command:
#
#  Copyright (c) 2011-2013 by Sierra Wireless, Incorporated.  All Rights Reserved.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Local Settings
#-------------------------------------------------------------------------------
SRCS_CBK      = cbk/src/qaGobiApiCbk.c \
                cbk/src/qaQmiNotify.c \
                cbk/wds/src/qaCbkWdsNotify.c \
                cbk/wds/src/qaCbkWdsSetEventReport.c \
                cbk/wds/src/qaCbkWdsEventReportInd.c \
                cbk/wds/src/qaCbkWdsGetPktSrvcStatusInd.c \
                cbk/dms/src/qaCbkDmsNotify.c \
                cbk/dms/src/qaCbkDmsSetEventReport.c \
                cbk/dms/src/qaCbkDmsEventReportInd.c \
                cbk/dms/src/qaCbkDmsSwiSetEventReport.c \
                cbk/dms/src/qaCbkDmsSwiEventReportInd.c \
                cbk/nas/src/qaCbkNasNotify.c \
                cbk/nas/src/qaCbkNasServingSystemInd.c \
                cbk/nas/src/qaCbkNasSetEventReport.c \
                cbk/nas/src/qaCbkNasEventReportInd.c \
                cbk/nas/src/qaCbkNasSystemSelectionPreferenceInd.c \
                cbk/nas/src/qaCbkNasSysInfoInd.c \
                cbk/nas/src/qaCbkNasNetworkTimeInd.c \
                cbk/qos/src/qaCbkQosNotify.c \
                cbk/qos/src/qaCbkSetQosEventReport.c \
                cbk/qos/src/qaCbkQosEventReportInd.c \
                cbk/qos/src/qaCbkQosNetworkStatusInd.c \
                cbk/qos/src/qaCbkQosFlowStatusInd.c \
                cbk/qos/src/qaCbkQosPriEventInd.c \
                cbk/pds/src/qaCbkPdsNotify.c \
                cbk/pds/src/qaCbkPdsSetEventReport.c \
                cbk/pds/src/qaCbkPdsEventReportInd.c \
                cbk/pds/src/qaCbkPdsGpsServiceStateInd.c \
                cbk/wms/src/qaCbkWmsNotify.c \
                cbk/wms/src/qaCbkWmsSetEventReport.c \
                cbk/wms/src/qaCbkWmsEventReportInd.c \
                cbk/wms/src/qaCbkWmsTransLayerInfoInd.c \
                cbk/wms/src/qaCbkWmsTransNWRegInfoInd.c \
                cbk/cat/src/qaCbkCatNotify.c \
                cbk/cat/src/qaCbkCatSetEventReport.c \
                cbk/cat/src/qaCbkCatEventReportInd.c \
                cbk/swioma/src/qaCbkSwiOmaDmNotify.c \
		cbk/swioma/src/qaCbkSwiOmaDmNotifyExt.c \
                cbk/swioma/src/qaCbkSwiOmaDmSetEventReport.c \
		cbk/swioma/src/qaCbkSwiOmaDmSetEventReportExt.c \
                cbk/swioma/src/qaCbkSwiOmaDmEventReportInd.c \
		cbk/swioma/src/qaCbkSwiOmaDmEventReportIndExt.c \
                cbk/dcs/src/qaCbkDcsNotify.c \
                cbk/dcs/src/qaCbkDcsEventReportInd.c \
                cbk/dcs/src/qaCbkDcsSetNetChange.c \
                cbk/fms/src/qaCbkFmsNotify.c \
                cbk/fms/src/qaCbkFmsEventReportInd.c \
                cbk/omadm/src/qaCbkOmaDmNotify.c \
                cbk/omadm/src/qaCbkOmaDmSetEventReport.c \
                cbk/omadm/src/qaCbkOmaDmEventReportInd.c \
                cbk/voice/src/qaCbkVoiceNotify.c \
                cbk/voice/src/qaCbkVoiceUssdInd.c \
                cbk/voice/src/qaCbkVoiceUssdNoWaitInd.c \
                cbk/voice/src/qaCbkVoiceIndicationRegister.c \
                cbk/voice/src/qaCbkVoiceSUPSNotificationInd.c \
                cbk/voice/src/qaCbkVoiceSLQSVoiceAllCallStatusInd.c \
                cbk/voice/src/qaCbkVoiceSLQSVoiceDTMFInd.c \
                cbk/voice/src/qaCbkVoiceSLQSVoiceSUPSInd.c \
                cbk/uim/src/qaCbkUimNotify.c \
                cbk/uim/src/qaCbkUimSLQSUimSetRefreshInd.c \
                cbk/uim/src/qaCbkUimSLQSUimSetStatusChangeInd.c \
                cbk/voice/src/qaCbkVoiceSLQSVoicePrivacyInd.c \
                cbk/wms/src/qaCbkWmsMemoryFullInd.c \
                cbk/voice/src/qaCbkVoiceSLQSVoiceInfoRecInd.c \
                cbk/voice/src/qaCbkVoiceSLQSVoiceSetOTASPStatusInd.c \
                cbk/wms/src/qaCbkWmsMessageWaitingInd.c \
                cbk/nas/src/qaCbkNasSigInfoInd.c \
                cbk/nas/src/qaCbkNasSwiHDRPersonalityInd.c \
                cbk/ims/src/qaCbkImsNotify.c \
                cbk/ims/src/qaCbkImsSLQSSIPCfgInd.c \
                cbk/ims/src/qaCbkImsSLQSRegMgrCfgInd.c \
                cbk/ims/src/qaCbkImsSLQSSMSCfgInd.c \
                cbk/ims/src/qaCbkImsSLQSUserCfgInd.c \
                cbk/ims/src/qaCbkImsSLQSVoIPCfgInd.c \
                cbk/imsa/src/qaCbkImsaNotify.c \
                cbk/imsa/src/qaCbkImsSLQSRatStatusInd.c \
                cbk/imsa/src/qaCbkImsSLQSRegStatusInd.c \
                cbk/imsa/src/qaCbkImsSLQSSvcStatusInd.c \
                cbk/nas/src/qaCbkNasDDTMSettingsInd.c \
                cbk/loc/src/qaCbkLocNotify.c \
                cbk/loc/src/qaCbkLocCradleMountInd.c \
                cbk/loc/src/qaCbkLocEventTimeSyncInd.c \
                cbk/loc/src/qaCbkLocInjectTimeInd.c \
                cbk/loc/src/qaCbkLocSensorStreamInd.c \
                cbk/loc/src/qaCbkLocSensorDataInd.c \
                cbk/loc/src/qaCbkLocEventGnssSvInfoInd.c \
                cbk/loc/src/qaCbkLocOpModeInd.c \
                cbk/loc/src/qaCbkLocGetOpModeInd.c \
                cbk/loc/src/qaCbkLocAssistDataInd.c \
                cbk/loc/src/qaCbkLocPositionRptInd.c \
                cbk/loc/src/qaCbkLocInjectUTCTimeInd.c \
                cbk/loc/src/qaCbkLocGetServerInd.c \
                cbk/loc/src/qaCbkLocSetServerInd.c \
                cbk/loc/src/qaCbkLocGetFixCriteriaInd.c

#-------------------------------------------------------------------------------
# Split the object files into their respective groups
#-------------------------------------------------------------------------------
CBKOBJ        = $(OBJSDIR)/qaGobiApiCbk.o \
                $(OBJSDIR)/qaQmiNotify.o \
                $(OBJSDIR)/qaCbkWdsSetEventReport.o \
                $(OBJSDIR)/qaCbkWdsEventReportInd.o \
                $(OBJSDIR)/qaCbkWdsGetPktSrvcStatusInd.o \
                $(OBJSDIR)/qaCbkDmsSetEventReport.o \
                $(OBJSDIR)/qaCbkDmsEventReportInd.o \
                $(OBJSDIR)/qaCbkDmsSwiSetEventReport.o \
                $(OBJSDIR)/qaCbkDmsSwiEventReportInd.o \
                $(OBJSDIR)/qaCbkNasServingSystemInd.o \
                $(OBJSDIR)/qaCbkNasSetEventReport.o \
                $(OBJSDIR)/qaCbkNasEventReportInd.o \
                $(OBJSDIR)/qaCbkNasSystemSelectionPreferenceInd.o \
                $(OBJSDIR)/qaCbkNasSysInfoInd.o \
                $(OBJSDIR)/qaCbkNasNetworkTimeInd.o \
                $(OBJSDIR)/qaCbkPdsSetEventReport.o \
                $(OBJSDIR)/qaCbkPdsEventReportInd.o \
                $(OBJSDIR)/qaCbkPdsGpsServiceStateInd.o \
                $(OBJSDIR)/qaCbkWmsSetEventReport.o \
                $(OBJSDIR)/qaCbkWmsEventReportInd.o \
                $(OBJSDIR)/qaCbkCatSetEventReport.o \
                $(OBJSDIR)/qaCbkCatEventReportInd.o \
                $(OBJSDIR)/qaCbkSwiOmaDmSetEventReport.o \
		$(OBJSDIR)/qaCbkSwiOmaDmSetEventReportExt.o \
                $(OBJSDIR)/qaCbkSwiOmaDmEventReportInd.o \
		$(OBJSDIR)/qaCbkSwiOmaDmEventReportIndExt.o \
                $(OBJSDIR)/qaCbkDcsEventReportInd.o \
                $(OBJSDIR)/qaCbkDcsSetNetChange.o \
                $(OBJSDIR)/qaCbkFmsEventReportInd.o \
                $(OBJSDIR)/qaCbkOmaDmSetEventReport.o \
                $(OBJSDIR)/qaCbkOmaDmEventReportInd.o \
                $(OBJSDIR)/qaCbkVoiceUssdInd.o \
                $(OBJSDIR)/qaCbkVoiceUssdNoWaitInd.o \
                $(OBJSDIR)/qaCbkVoiceSLQSVoiceAllCallStatusInd.o \
                $(OBJSDIR)/qaCbkWdsNotify.o \
                $(OBJSDIR)/qaCbkDmsNotify.o \
                $(OBJSDIR)/qaCbkNasNotify.o \
                $(OBJSDIR)/qaCbkQosNotify.o \
                $(OBJSDIR)/qaCbkPdsNotify.o \
                $(OBJSDIR)/qaCbkWmsNotify.o \
                $(OBJSDIR)/qaCbkCatNotify.o \
                $(OBJSDIR)/qaCbkSwiOmaDmNotify.o \
		$(OBJSDIR)/qaCbkSwiOmaDmNotifyExt.o \
                $(OBJSDIR)/qaCbkDcsNotify.o \
                $(OBJSDIR)/qaCbkFmsNotify.o \
                $(OBJSDIR)/qaCbkOmaDmNotify.o \
                $(OBJSDIR)/qaCbkVoiceNotify.o \
                $(OBJSDIR)/qaCbkVoiceIndicationRegister.o \
                $(OBJSDIR)/qaCbkVoiceSUPSNotificationInd.o \
                $(OBJSDIR)/qaCbkWmsTransLayerInfoInd.o \
                $(OBJSDIR)/qaCbkWmsTransNWRegInfoInd.o \
                $(OBJSDIR)/qaCbkUimNotify.o \
                $(OBJSDIR)/qaCbkUimSLQSUimSetRefreshInd.o \
                $(OBJSDIR)/qaCbkUimSLQSUimSetStatusChangeInd.o \
                $(OBJSDIR)/qaCbkVoiceSLQSVoicePrivacyInd.o \
                $(OBJSDIR)/qaCbkVoiceSLQSVoiceDTMFInd.o \
                $(OBJSDIR)/qaCbkVoiceSLQSVoiceSUPSInd.o \
                $(OBJSDIR)/qaCbkWmsMemoryFullInd.o \
                $(OBJSDIR)/qaCbkVoiceSLQSVoiceInfoRecInd.o \
                $(OBJSDIR)/qaCbkVoiceSLQSVoiceSetOTASPStatusInd.o \
                $(OBJSDIR)/qaCbkSetQosEventReport.o \
                $(OBJSDIR)/qaCbkQosEventReportInd.o \
                $(OBJSDIR)/qaCbkQosNetworkStatusInd.o \
                $(OBJSDIR)/qaCbkQosFlowStatusInd.o \
                $(OBJSDIR)/qaCbkQosPriEventInd.o \
                $(OBJSDIR)/qaCbkWmsMessageWaitingInd.o \
                $(OBJSDIR)/qaCbkNasSigInfoInd.o \
                $(OBJSDIR)/qaCbkNasSwiHDRPersonalityInd.o \
                $(OBJSDIR)/qaCbkImsNotify.o \
                $(OBJSDIR)/qaCbkImsSLQSSIPCfgInd.o \
                $(OBJSDIR)/qaCbkImsSLQSRegMgrCfgInd.o \
                $(OBJSDIR)/qaCbkImsSLQSSMSCfgInd.o \
                $(OBJSDIR)/qaCbkImsSLQSUserCfgInd.o \
                $(OBJSDIR)/qaCbkImsSLQSVoIPCfgInd.o \
                $(OBJSDIR)/qaCbkImsaNotify.o \
                $(OBJSDIR)/qaCbkImsSLQSRatStatusInd.o \
                $(OBJSDIR)/qaCbkImsSLQSRegStatusInd.o \
                $(OBJSDIR)/qaCbkImsSLQSSvcStatusInd.o \
                $(OBJSDIR)/qaCbkNasDDTMSettingsInd.o \
                $(OBJSDIR)/qaCbkLocNotify.o \
                $(OBJSDIR)/qaCbkLocCradleMountInd.o \
                $(OBJSDIR)/qaCbkLocEventTimeSyncInd.o \
                $(OBJSDIR)/qaCbkLocInjectTimeInd.o \
                $(OBJSDIR)/qaCbkLocSensorStreamInd.o \
                $(OBJSDIR)/qaCbkLocSensorDataInd.o \
                $(OBJSDIR)/qaCbkLocEventGnssSvInfoInd.o \
                $(OBJSDIR)/qaCbkLocOpModeInd.o \
                $(OBJSDIR)/qaCbkLocGetOpModeInd.o \
                $(OBJSDIR)/qaCbkLocAssistDataInd.o \
                $(OBJSDIR)/qaCbkLocInjectUTCTimeInd.o \
                $(OBJSDIR)/qaCbkLocGetServerInd.o \
                $(OBJSDIR)/qaCbkLocSetServerInd.o \
                $(OBJSDIR)/qaCbkLocGetFixCriteriaInd.o \
                $(OBJSDIR)/qaCbkLocPositionRptInd.o