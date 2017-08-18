/**
 *
 * $Id: SwiIntDecodeNotify.c,v 1.39 2011/02/23 01:01:22 ckaria Exp $
 *
 * Filename: SwiIntDecodeNotify.c
 *
 * Purpose:  Contains functions to handle notifications
 *
 * Copyright: (c) 2011 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "am/amudefs.h"
#include "er/erudefs.h"
#include "cn/cnudefs.h"
#include "ap/apidefs.h"
#include "SwiIntUpkCmLbs.h"
#include "SwiIntUpkCdmaLbs.h"
#include "SwiIntDecodeNotify.h"

/**
 *
 * Name:    swicwenottable - CWE Objects Notification Table
 *
 * Purpose: This table contains decoding functions for CWE
 *          CnS notifications.  It is indexed by CWE CnS
 *          object ids.  
 *
 */
local const struct swinotifyhandler swicwenottable[] = 
{
                  /* notification type                     decode function */
    /* 0x0000 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0001 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0002 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0003 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0004 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0005 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0006 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0007 */ {  SWI_NOTIFY_RadioPower,                 swiradiopowerdecode},
    /* 0x0008 */ {  SWI_NOTIFY_RadioTempState,             swiradiotempdecode},
    /* 0x0009 */ {  SWI_NOTIFY_RadioBattery,               swiradiobatterydecode},
    /* 0x000A */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x000B */ {  SWI_NOTIFY_NvStore,                    swinvstoredecode},
    /* 0x000C */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x000D */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x000E */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x000F */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0010 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0011 */ {  SWI_NOTIFY_NVUpdateFromFile,           swinvupdatefromfiledecode},
    /* 0x0012 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0013 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0014 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0015 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0016 */ {  SWI_NOTIFY_RadioBand,                  swicurradiobanddecode},
    /* 0x0017 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0018 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0019 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x001A */ {  SWI_NOTIFY_AudioProfile,               swiaudioprofile},
    /* 0x001B */ {  SWI_NOTIFY_DeviceTime,                 swidevicetime},
    /* 0x001C */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x001D */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x001E */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x001F */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0020 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0021 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0022 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0023 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0024 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0025 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0026 */ {  SWI_NOTIFY_NULL,                       NULL}
};

#if defined(CDMA) || defined (ALL)
/**
 *
 * Name:    swidmnottable - DM Objects Notification Table
 *
 * Purpose: This table contains decoding functions for DM
 *          CnS notifications.  A CnS DM objectid can be 
 *          converted to an index for this table by subtracting
 *          0x0E00. 
 *
 */
local const struct swinotifyhandler swidmnottable[] = 
{
                  /* notification type                     decode function */
    /* 0x0E00 */ {  SWI_NOTIFY_OMADM_Config,               swiomadmconfigdecode},
    /* 0x0E01 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0E02 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0E03 */ {  SWI_NOTIFY_OMADM_State,                swiomadmstatedecode},
    /* 0x0E04 */ {  SWI_NOTIFY_OMADM_NI_Alert,             swiomadmnialertdecode},
    /* 0x0E05 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0E06 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0E07 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0E08 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0E09 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0E0A */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0E0B */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0E0C */ {  SWI_NOTIFY_OMADM_UI_Alert_Display,     swiomadmuialertdisplaydecode},
    /* 0x0E0D */ {  SWI_NOTIFY_OMADM_UI_Alert_Confirm,     swiomadmuialertconfirmdecode},
    /* 0x0E0E */ {  SWI_NOTIFY_OMADM_UI_Alert_Input,       swiomadmuialertinputdecode},
    /* 0x0E0F */ {  SWI_NOTIFY_OMADM_UI_Alert_Choice,      swiomadmuialertchoicedecode},
    /* 0x0E10 */ {  SWI_NOTIFY_DM_DL_Progress,             swidmdlprogressdecode},
    /* 0x0E11 */ {  SWI_NOTIFY_DM_DL_Alert,                swidmdlalertdecode},
    /* 0x0E12 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0E13 */ {  SWI_NOTIFY_DM_DL_InstallStatus,        swidmdlinstallstatusdecode},
};
#endif

/**
 *
 * Name:    swilbsnottable - LBS Objects Notification Table
 *
 * Purpose: This table contains decoding functions for CnS
 *          LBS notifications.  A CnS LBS objectid can be 
 *          converted to an index for this table by subtracting
 *          0x0F00. 
 *
 */
local const struct swinotifyhandler swilbsnottable[] = 
{
                  /* notification type                     decode function */
    /* 0x0F00 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0F01 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0F02 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0F03 */ {  SWI_NOTIFY_LbsPdGetStatus,             swilbspdgetstatusdecode},
    /* 0x0F04 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0F05 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0F06 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0F07 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0F08 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0F09 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0F0A */ {  SWI_NOTIFY_LbsPdData,                  swilbspddatadecode},
    /* 0x0F0B */ {  SWI_NOTIFY_LbsPdDone,                  swilbspddonedecode},
    /* 0x0F0C */ {  SWI_NOTIFY_LbsPdEnd,                   swilbspdenddecode},
    /* 0x0F0D */ {  SWI_NOTIFY_LbsPdBegin,                 swilbspdbegindecode},
    /* 0x0F0E */ {  SWI_NOTIFY_LbsPdUpdateFailure,         swilbspdupdatefailuredecode},
    /* 0x0F0F */ {  SWI_NOTIFY_LbsPdDloadBegin,            swilbspddloadbegindecode},
    /* 0x0F10 */ {  SWI_NOTIFY_LbsPdDloadData,             swilbspddloaddatadecode},
    /* 0x0F11 */ {  SWI_NOTIFY_LbsPdDloadDone,             swilbspddloaddonedecode},
    /* 0x0F12 */ {  SWI_NOTIFY_LbsPdDloadEnd,              swilbspddloadenddecode},
    /* 0x0F13 */ {  SWI_NOTIFY_LbsPaIpAddr,                swilbspaipaddrdecode},
    /* 0x0F14 */ {  SWI_NOTIFY_LbsPaGpsLock,               swilbspagpslockdecode},
    /* 0x0F15 */ {  SWI_NOTIFY_LbsPaPtlmMode,              swilbspaptlmmodedecode},
    /* 0x0F16 */ {  SWI_NOTIFY_LbsPaPortId,                swilbspaportiddecode},
    /* 0x0F17 */ {  SWI_NOTIFY_LbsPaPrivacy,               swilbspaprivacydecode},
    /* 0x0F18 */ {  SWI_NOTIFY_LbsPaNetAccess,             swilbspanetaccessdecode},
    /* 0x0F19 */ {  SWI_NOTIFY_LbsPaBsInfo,                swilbspabsinfodecode},
    /* 0x0F1A */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0F1B */ {  SWI_NOTIFY_LbsPaWarmBegin,             swilbspawarmbegindecode},
    /* 0x0F1C */ {  SWI_NOTIFY_LbsPaWarmStatus,            swilbspawarmstatusdecode},
    /* 0x0F1D */ {  SWI_NOTIFY_LbsPaWarmDone,              swilbspawarmdonedecode},
    /* 0x0F1E */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0F1F */ {  SWI_NOTIFY_LbsSatInfo,                 swilbssatinfodecode},
    /* 0x0F20 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0F21 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0F22 */ {  SWI_NOTIFY_LbsNiReq,                   swilbsnireqdecode},
    /* 0x0F23 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0F24 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0F25 */ {  SWI_NOTIFY_LbsXtraAssistData,          swilbsxtraassist},
    /* 0x0F26 */ {  SWI_NOTIFY_LbsXtraTimeInfo,            swilbsxtraassist},
    /* 0x0F27 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0F28 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x0F29 */ {  SWI_NOTIFY_LbsXtraDloadTrigger,        swilbsxtradloadtrigger}
};

#if defined(CDMA) || defined (ALL)
/**
 *
 * Name:    swicdmanottable - CDMA Objects Notification Table
 *
 * Purpose: This table contains decoding functions for CnS
 *          CDMA notifications.  A CnS CDMA objectid can be 
 *          converted to an index for this table by subtracting
 *          0x1000. 
 *
 */
local const struct swinotifyhandler swicdmanottable[] = 
{
                  /* notification type                     decode function */
    /* 0x1000 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1001 */ {  SWI_NOTIFY_Rssi,                       swirssidecode},
    /* 0x1002 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1003 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1004 */ {  SWI_NOTIFY_ChannelNumber,              swichannelnumberdecode},
    /* 0x1005 */ {  SWI_NOTIFY_ChannelState,               swichannelstatedecode},
    /* 0x1006 */ {  SWI_NOTIFY_ServiceIndication,          swiserviceindicationdecode},
    /* 0x1007 */ {  SWI_NOTIFY_RoamingStatus,              swiroamingstatusdecode},
    /* 0x1008 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1009 */ {  SWI_NOTIFY_ActivationStatus,           swiactivationstatusdecode},
    /* 0x100A */ {  SWI_NOTIFY_Ecio,                       swieciodecode},
    /* 0x100B */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x100C */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x100D */ {  SWI_NOTIFY_V42BisEnabled,              swiv42bisenableddecode},
    /* 0x100E */ {  SWI_NOTIFY_AuthenticationStatus,       swiauthenticationstatusdecode},
    /* 0x100F */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1010 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1011 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1012 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1013 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1014 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1015 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1016 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1017 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1018 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1019 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x101A */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x101B */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x101C */ {  SWI_NOTIFY_Prev,                       swiprevdecode},
    /* 0x101D */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x101E */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x101F */ {  SWI_NOTIFY_ActiveNam,                  swiactivenamdecode},
    /* 0x1020 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1021 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1022 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1023 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1024 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1025 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1026 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1027 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1028 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1029 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x102A */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x102B */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x102C */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x102D */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x102E */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x102F */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1030 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1031 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1032 */ {  SWI_NOTIFY_SystemTime,                 swisystemtimedecode},
    /* 0x1033 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1034 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1035 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1036 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1037 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1038 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1039 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x103A */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x103B */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x103C */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x103D */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x103E */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x103F */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1040 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1041 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1042 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1043 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1044 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1045 */ {  SWI_NOTIFY_E911Mode,                   swie911modedecode},
    /* 0x1046 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1047 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1048 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1049 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x104A */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x104B */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x104C */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x104D */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x104E */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x104F */ {  SWI_NOTIFY_PowerSaveEnter,             swipowersaveenterdecode},
    /* 0x1050 */ {  SWI_NOTIFY_PowerSaveExit,              swipowersaveexitdecode},
    /* 0x1051 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1052 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1053 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1054 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1055 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1056 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1057 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1058 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1059 */ {  SWI_NOTIFY_ExcessSPCFailure,           swiexcessspcfailuredecode},
    /* 0x105A */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x105B */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x105C */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x105D */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x105E */ {  SWI_NOTIFY_RuimStatus,                 swiruimstatusdecode},
    /* 0x105F */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1060 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1061 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1062 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1063 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1064 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1065 */ {  SWI_NOTIFY_Rssi_Ecio,                  swirssi_eciodecode},
    /* 0x1066 */ {  SWI_NOTIFY_HdrNotificationStatus,      swihdrnotificationstatusdecode},
    /* 0x1067 */ {  SWI_NOTIFY_Hdr_Rssi,                   swihdr_rssidecode},
    /* 0x1068 */ {  SWI_NOTIFY_Hdr_Roam_Status,            swihdr_roam_statusdecode},
    /* 0x1069 */ {  SWI_NOTIFY_Hdr_Srv_State,              swihdr_srv_statedecode},
    /* 0x106A */ {  SWI_NOTIFY_Hdr_Hybr_Pref,              swihdr_hybr_prefdecode},
    /* 0x106B */ {  SWI_NOTIFY_Idle_Digital_Mode,          swiidle_digital_modedecode},
    /* 0x106C */ {  SWI_NOTIFY_DDTM_Pref,                  swiddtm_prefdecode},
    /* 0x106D */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x106E */ {  SWI_NOTIFY_Hdr_PowerSave_Enter,        swihdr_powersave_enterdecode},
    /* 0x106F */ {  SWI_NOTIFY_Hdr_PowerSave_Exit,         swihdr_powersave_exitdecode},
    /* 0x1070 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1071 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1072 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1073 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1074 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1075 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1076 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1077 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1078 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1079 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x107A */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x107B */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x107C */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x107D */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x107E */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x107F */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1080 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1081 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1082 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1083 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1084 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1085 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1086 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1087 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1088 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1089 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x108A */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x108B */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x108C */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x108D */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x108E */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x108F */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1090 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1091 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1092 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1093 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1094 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1095 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1096 */ {  SWI_NOTIFY_NULL,                       NULL}
};
#endif

#if defined(CDMA) || defined (ALL)
local const struct swinotifyhandler swicdmahdracqnottable[] = 
{ 
                    /* notification type                     decode function */
    /* 0x1200 */ {  SWI_NOTIFY_Hdr_Acq,                    swihdr_acqdecode},
};
#endif

#if defined(UMTS) || defined (ALL)
/**
 *
 * Name:    swigsmnottable - GSM Objects Notification Table
 *
 * Purpose: This table contains decoding functions for CnS
 *          GSM notifications.  A CnS GSM objectid can be 
 *          converted to an index for this table by subtracting
 *          0x1000. 
 *
 */
local const struct swinotifyhandler swigsmnottable[] = 
{
                  /* notification type                     decode function */
    /* 0x1000 */ {  SWI_NOTIFY_NetworkStatus,              swinetstatusdecode},
    /* 0x1001 */ {  SWI_NOTIFY_Rssi,                       swiradiodecode},
    /* 0x1002 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1003 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1004 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1005 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1006 */ {  SWI_NOTIFY_ServiceIcon,                swiserviceicondecode},
    /* 0x1007 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1008 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1009 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x100A */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x100B */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x100C */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x100D */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x100E */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x100F */ {  SWI_NOTIFY_PlmnMode,                   swiplmnmodedecode},
    /* 0x1010 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1011 */ {  SWI_NOTIFY_ModemDisabled,              swimodemdisabled},
    /* 0x1012 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1013 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1014 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1015 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1016 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1017 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1018 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1019 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x101A */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x101B */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x101C */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x101D */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x101E */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x101F */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1020 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1021 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1022 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1023 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1024 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1025 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1026 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1027 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1028 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1029 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x102A */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x102B */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x102C */ {  SWI_NOTIFY_MissedCall,                 swimissedcall},
    /* 0x102D */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x102E */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x102F */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1030 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1031 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1032 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1033 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1034 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1035 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1036 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1037 */ {  SWI_NOTIFY_ByteCounter,                swibytecntdecode},
    /* 0x1038 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1039 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x103A */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x103B */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x103C */ {  SWI_NOTIFY_PlmnReady,                  swiplmnreadydecode},
    /* 0x103D */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x103E */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x103F */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1040 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1041 */ {  SWI_NOTIFY_SimStatusExp,               swisimstatusdecode},
    /* 0x1042 */ {  SWI_NOTIFY_PlmnAvailable,              swiplmnavaildecode},
    /* 0x1043 */ {  SWI_NOTIFY_RegistrationExp,            swiregistrationdecode},
    /* 0x1044 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1045 */ {  SWI_NOTIFY_PhonebookEntry,             swiphonebookentrydecode},
    /* 0x1046 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1047 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1048 */ {  SWI_NOTIFY_PhonebookReady,             swiphonebookreadydecode},
    /* 0x1049 */ {  SWI_NOTIFY_PhonebookReady,             swiphonebookreadydecode},
    /* 0x104A */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x104B */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x104C */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x104D */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x104E */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x104F */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1050 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1051 */ {  SWI_NOTIFY_SSPasswordStatus,           swisspasswordstatusdecode},
    /* 0x1052 */ {  SWI_NOTIFY_SSRespOK,                   swissrespokdecode},
    /* 0x1053 */ {  SWI_NOTIFY_SSRespFwdInfo,              swissfwdinforspdecode},
    /* 0x1054 */ {  SWI_NOTIFY_SSRespCallBar,              swisscallbarrspdecode},
    /* 0x1055 */ {  SWI_NOTIFY_SSRespInfoData,             swissinfodatarspdecode},
    /* 0x1056 */ {  SWI_NOTIFY_SSRespStatus,               swissstatusrspdecode},
    /* 0x1057 */ {  SWI_NOTIFY_SSRespFwdNum,               swissfwdnumrspdecode},
    /* 0x1058 */ {  SWI_NOTIFY_SSRespBSList,               swissbslistrspdecode},
    /* 0x1059 */ {  SWI_NOTIFY_SSRespFwdList,              swissfwdlistrspdecode},
    /* 0x105A */ {  SWI_NOTIFY_SSRespRaw,                  swissrawrspdecode},
    /* 0x105B */ {  SWI_NOTIFY_SSRespPassword,             swisspasswordstatusdecode},
    /* 0x105C */ {  SWI_NOTIFY_SSRespClir,                 swissclirrspdecode},
    /* 0x105D */ {  SWI_NOTIFY_SSRespError,                swisserrorrspdecode},
    /* 0x105E */ {  SWI_NOTIFY_SSRespReject,               swissrejectrspdecode},
    /* 0x105F */ {  SWI_NOTIFY_SSRespCancel,               swisscancelrspdecode},
    /* 0x1060 */ {  SWI_NOTIFY_SSRespRelease,              swissreleaserspdecode},
    /* 0x1061 */ {  SWI_NOTIFY_SSNotify,                   swissnotifydecode},
    /* 0x1062 */ {  SWI_NOTIFY_VoiceMessage,               swivoicemessagedecode},
    /* 0x1063 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1064 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1065 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1066 */ {  SWI_NOTIFY_CallProgress,               swigsmcallprogressdecode},
    /* 0x1067 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1068 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1069 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x106A */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x106B */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x106C */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x106D */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x106E */ {  SWI_NOTIFY_CallForwarded,              swigsmcallforwardeddecode},
    /* 0x106F */ {  SWI_NOTIFY_CallStatus,                 swigsmcallstatusdecode},
    /* 0x1070 */ {  SWI_NOTIFY_CFIcon,                     swigsmdisplaycficondecode},
    /* 0x1071 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1072 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1073 */ {  SWI_NOTIFY_STKCommand,                 swistkcmdnotifdecode},
    /* 0x1074 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1075 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1076 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1077 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1078 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1079 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x107A */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x107B */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x107C */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x107D */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x107E */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x107F */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1080 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1081 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1082 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1083 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1084 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1085 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1086 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1087 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1088 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1089 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x108A */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x108B */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x108C */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x108D */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x108E */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x108F */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1090 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1091 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1092 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1093 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1094 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1095 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x1096 */ {  SWI_NOTIFY_NULL,                       NULL}
};

#endif

#if defined(UMTS) || defined (ALL)
/**
 *
 * Name:    swiumtsnottable - UMTS Objects Notification Table
 *
 * Purpose: This table contains decoding functions for CnS
 *          UMTS notifications.  A CnS UMTS objectid can be 
 *          converted to an index for this table by subtracting
 *          0x7000. 
 *
 */
local const struct swinotifyhandler swiumtsnottable[] = 
{
                  /* notification type                     decode function */
    /* 0x7000 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x7001 */ {  SWI_NOTIFY_GsmDefaultChange,           swigsmdefprofchangedecode},
    /* 0x7002 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x7003 */ {  SWI_NOTIFY_GsmProfileChange,           swigsmprofilechangedecode},
    /* 0x7004 */ {  SWI_NOTIFY_PktSessionCall,             swipktsessioncalldecode},
    /* 0x7005 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x7006 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x7007 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x7008 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x7009 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x700A */ {  SWI_NOTIFY_GsmSystemStatus,            swigsmsystemstatusdecode},
    /* 0x700B */ {  SWI_NOTIFY_RscpEcIo,                   swirscpeciodecode},
    /* 0x700C */ {  SWI_NOTIFY_GsmCellInfo,                swigsmcelldecode},
    /* 0x700D */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x700E */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x700F */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x7010 */ {  SWI_NOTIFY_NULL,                       NULL}
};
#endif

#if defined(CDMA) || defined (ALL)
local const struct swinotifyhandler swismsnottable[] = 
{
                  /* notification type                     decode function */
    /* 0x2000 */ {  SWI_NOTIFY_SmsStatus,                  swismsstatusdecode},
    /* 0x2001 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x2002 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x2003 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x2004 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x2005 */ {  SWI_NOTIFY_SmsSendStatus,              swismssendstatusdecode}
};
#endif

#if defined(CDMA) || defined (ALL)
local const struct swinotifyhandler swiiotanottable[] = 
{
                          /* notification type                     decode function */
    /* 0x2102/0x7002 */ {  SWI_NOTIFY_IOTA_Status,  swiiota_statusdecode},
    /* 0x2103/0x7003 */ {  SWI_NOTIFY_IOTA_WAPPush, swiiota_wappushdecode},
};
#endif

#if defined(CDMA) || defined (ALL)
local const struct swinotifyhandler swicallnottable[] = 
{
                  /* notification type                     decode function */
    /* 0x3000 */ {  SWI_NOTIFY_CallNotificationStatus,     swicallnotificationstatusdecode},
    /* 0x3001 */ {  SWI_NOTIFY_CallByteCounter,            swicallbytecounterdecode},
    /* 0x3002 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x3003 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x3004 */ {  SWI_NOTIFY_CallIncoming,               swicallincomingdecode},
    /* 0x3005 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x3006 */ {  SWI_NOTIFY_CallCallerID,               swicallcalleriddecode},
    /* 0x3007 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x3008 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x3009 */ {  SWI_NOTIFY_CallExtendedCallerID,       swicallextendedcalleriddecode},
    /* 0x300A */ {  SWI_NOTIFY_CallConnected,              swicallconnecteddecode},
    /* 0x300B */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x300C */ {  SWI_NOTIFY_CallDisconnected,           swicalldisconnecteddecode},
    /* 0x300D */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x300E */ {  SWI_NOTIFY_CallError,                  swicallerrordecode},
    /* 0x300F */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x3010 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x3011 */ {  SWI_NOTIFY_CallConnecting,             swicallconnectingdecode},
    /* 0x3012 */ {  SWI_NOTIFY_CallDormant,                swicalldormantdecode},
    /* 0x3013 */ {  SWI_NOTIFY_CallPrivacy,                swicallprivacydecode},
    /* 0x3014 */ {  SWI_NOTIFY_OTASPState,                 swiotaspstatedecode},
};

local const struct swinotifyhandler switechnottable[] = 
{
                  /* notification type                     decode function */
    /* 0x4000 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x4001 */ {  SWI_NOTIFY_TechSid,                    switechsiddecode},
    /* 0x4002 */ {  SWI_NOTIFY_TechNid,                    switechniddecode},
    /* 0x4003 */ {  SWI_NOTIFY_TechBsInfo,                 switechbsinfodecode},
    /* 0x4004 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x4005 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x4006 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x4007 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x4008 */ {  SWI_NOTIFY_TechBandClass,              switechbandclassdecode},
    /* 0x4009 */ {  SWI_NOTIFY_TechRegZone,                switechregzonedecode},
    /* 0x400A */ {  SWI_NOTIFY_TechPacketZone,             switechpacketzonedecode},
    /* 0x400B */ {  SWI_NOTIFY_TechPcsBlock,               switechpcsblockdecode},
    /* 0x400C */ {  SWI_NOTIFY_TechCellChannel,            switechcellchanneldecode},
    /* 0x400D */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x400E */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x400F */ {  SWI_NOTIFY_FlashBurn,                  swiflashburndecode},
    /* 0x4010 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x4011 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x4012 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x4013 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x4014 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x4015 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x4016 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x4017 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x4018 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x4019 */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x401A */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x401B */ {  SWI_NOTIFY_NULL,                       NULL},
    /* 0x401C */ {  SWI_NOTIFY_TechHdrNotificationStatus,  switechhdrnotificationstatusdecode},
    /* 0x401D */ {  SWI_NOTIFY_Tech_Hdr_Prev,              switechhdrprevdecode},
    /* 0x401E */ {  SWI_NOTIFY_Tech_Hdr_Sector,            switechhdrsectordecode},
    /* 0x401F */ {  SWI_NOTIFY_Tech_Hdr_State,              switechhdrstatedecode},
    /* 0x4020 */ {  SWI_NOTIFY_Tech_Hdr_Channel,           switechhdrchanneldecode},
    /* 0x4021 */ {  SWI_NOTIFY_Tech_Hdr_Session,           switechhdrsessioninfodecode},
    /* 0x4022 */ {  SWI_NOTIFY_Tech_Hdr_An_Auth,           switechhdranauthdecode},
    /* 0x4023 */ {  SWI_NOTIFY_Tech_Hdr_Drc_Val,           switechhdrdrcvaldecode},
    /* 0x4024 */ {  SWI_NOTIFY_Tech_Hdr_Drc_Cover,         switechhdrdrccoverdecode},
    /* 0x4025 */ {  SWI_NOTIFY_Tech_Hdr_Rri,               switechhdrrridecode},
    /* 0x4026 */ {  SWI_NOTIFY_Tech_Hdr_Stats,             switechhdrstatsdecode},
    /* 0x4027 */ {  SWI_NOTIFY_Tech_Hdr_Su_Fwd_Stats,      switechhdrsufwdstatsdecode},
    /* 0x4028 */ {  SWI_NOTIFY_Tech_Hdr_Su_Fwd_Slot_Cnt,   switechhdrsufwdslotcntdecode},
    /* 0x4029 */ {  SWI_NOTIFY_Tech_Hdr_Su_Fwd_Slot_Cnt2,  switechhdrsufwdslotcnt2decode},
    /* 0x402A */ {  SWI_NOTIFY_Tech_Hdr_Mu_Fwd_Stats,      switechhdrmufwdstatsdecode},
    /* 0x402B */ {  SWI_NOTIFY_Tech_Hdr_Personality,       switechhdrpersonalitydecode},
    /* 0x402C*/ {  SWI_NOTIFY_NULL,                        NULL},
    /* 0x402D*/ {  SWI_NOTIFY_NULL,                        NULL},
    /* 0x402E*/ {  SWI_NOTIFY_Tech_Hdr_Mac_Index,          switechhdrmacindexdecode},
};

local const struct swinotifyhandler switempnottable[] = 
{
                  /* notification type                     decode function */
    /* 0x4500 */ {  SWI_NOTIFY_ModemTooHotIndication,      swimodemtoohotindicationdecode},
};
#endif

/* Number of objects in CWE Object Notification Table */
local swi_uint16 swicwenotmax = 
    sizeof(swicwenottable)/sizeof(struct swinotifyhandler);

/* Number of objects in LBS Object Notification Table */
local swi_uint16 swilbsnotmax = 
    sizeof(swilbsnottable)/sizeof(struct swinotifyhandler);

#if defined(UMTS) || defined (ALL)
/* Number of objects in GSM Object Notification Table */
local swi_uint16 swigsmnotmax = 
    sizeof(swigsmnottable)/sizeof(struct swinotifyhandler);

/* Number of objects in UMTS Object Notification Table */
local swi_uint16 swiumtsnotmax = 
    sizeof(swiumtsnottable)/sizeof(struct swinotifyhandler);
#endif

#if defined(CDMA) || defined (ALL)
/* Number of objects in DM Object Notification Table */
local swi_uint16 swidmnotmax = 
    sizeof(swidmnottable)/sizeof(struct swinotifyhandler);

/* Number of objects in CDMA Object Notification Table */
local swi_uint16 swicdmanotmax = 
    sizeof(swicdmanottable)/sizeof(struct swinotifyhandler);

local const swi_uint16 swismsnotmax = 
    sizeof(swismsnottable)/sizeof(struct swinotifyhandler);

local const swi_uint16 swiiotanotmax = 
    sizeof(swiiotanottable)/sizeof(struct swinotifyhandler);

local const swi_uint16 swicallnotmax = 
    sizeof(swicallnottable)/sizeof(struct swinotifyhandler);

local const swi_uint16 switechnotmax = 
    sizeof(switechnottable)/sizeof(struct swinotifyhandler);

local const swi_uint16 switempnotmax = 
    sizeof(switempnottable)/sizeof(struct swinotifyhandler);
#endif

/* Functions */


/**
 *
 * Name:    swigetnotifyhandler
 *
 * Purpose: This function returns a pointer to an entry in one of the 
 *          CnS Object Notification Tables.  First it determines which
 *          Table to look in based on the object id.  Then it converts
 *          the object id to an index in the table.  It returns the 
 *          indexed record.
 *             
 * Parms:   objectid[IN]:      object id
 *
 * Return:  Pointer to notification handler block
 *
 * Abort:   none
 *
 * Notes:   none
 *
 */
local const struct swinotifyhandler *swigetnotifyhandler(swi_uint16 objectid)
{ 
    if(objectid < (SWICWERANGEMIN + swicwenotmax))
    {
        return(&swicwenottable[objectid - SWICWERANGEMIN]);
    }
    if((objectid >= SWILBSRANGEMIN) && 
       (objectid < (SWILBSRANGEMIN + swilbsnotmax)))
    {
        return(&swilbsnottable[objectid - SWILBSRANGEMIN]);
    }
    
#if defined(UMTS) || defined (ALL)
    if((objectid >= SWIUMTSRANGEMIN) && 
       (objectid < (SWIUMTSRANGEMIN + swiumtsnotmax)))
    {
        /* GSM device */
        if(swigsmdualrangeuser())
        {
            return(&swiumtsnottable[objectid - SWIUMTSRANGEMIN]);
        }
    }
#endif
    
    if((objectid >= SWIDUALRANGEMIN) && (objectid <= SWIDUALRANGEMAX))
    {   
        /* GSM device */
        if(swigsmdualrangeuser())
        {
#if defined(UMTS) || defined (ALL)
            
            if (objectid < (SWIGSMRANGEMIN + swigsmnotmax))
            {
                return(&swigsmnottable[objectid - SWIGSMRANGEMIN]);
            } 
#endif
        }
        /* CDMA device */
        else
        {
#if defined(CDMA) || defined (ALL)
            if (objectid < (SWICDMARANGEMIN + swicdmanotmax))
            {
                return(&swicdmanottable[objectid - SWICDMARANGEMIN]);
            }
            else if(objectid == CNS_HDR_ACQ)
            {
                return (&swicdmahdracqnottable[0]);
            }
#endif
        }
    }

#if defined(CDMA) || defined (ALL)
    if((objectid >= SWIDMRANGEMIN) && 
       (objectid < (SWIDMRANGEMIN + swidmnotmax)))
    {
        return(&swidmnottable[objectid - SWIDMRANGEMIN]);
    }

    if(objectid >= SWISMSRANGEMIN && objectid < SWISMSRANGEMIN + swismsnotmax)
    {
        return (&swismsnottable[objectid-SWISMSRANGEMIN]);
    }

    if(objectid >= SWIIOTARANGEMIN && 
            objectid < SWIIOTARANGEMIN + swiiotanotmax)
    {
        if(swigsmdualrangeuser())
        {
            /* GSM device */
            /* Skip */
        }
        else
        {
            /* CDMA device */
            return (&swiiotanottable[objectid-SWIIOTARANGEMIN]);
        }
    }

    if(objectid >= SWIIOTARANGEMIN_LEGACY && 
            objectid < SWIIOTARANGEMIN_LEGACY + swiiotanotmax)
    {
        return (&swiiotanottable[objectid-SWIIOTARANGEMIN_LEGACY]);
    }

    if(objectid >= SWICALLRANGEMIN && 
            objectid < SWICALLRANGEMIN + swicallnotmax)
    {
        return (&swicallnottable[objectid-SWICALLRANGEMIN]);
    }

    if(objectid >= SWITECHRANGEMIN && 
            objectid < SWITECHRANGEMIN + switechnotmax)
    {
        return (&switechnottable[objectid-SWITECHRANGEMIN]);
    }
    
    if(objectid >= SWITEMPRANGEMIN && 
            objectid < SWITEMPRANGEMIN + switempnotmax)
    {
        return (&switempnottable[objectid-SWITEMPRANGEMIN]);
    }
#endif
    return(NULL);
}

/**
 *
 * Name: swidecodeairchgnotif
 *
 * Purpose: Decodes an Air Change notification packet
 *             
 * Parms: 
 *          amairparmsp [IN]:    Ptr to a packet containing an AM-formatted
 *                               AMNOTIFYAIRCHG which needs to be unpacked
 *          notifyvariantp[OUT]:  Ptr to global notification structure for
 *                               storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   none
 *
 */
package SWI_RCODE swidecodeairchgnotif(
    struct amairserverparms *amairparmsp,
    struct SwiNotifyVariant *notifyvariantp)
{
    struct SWI_STRUCT_AirServerChange *aschgp;
    struct SWI_STRUCT_AirServer       *asp;
    struct apicb *apicbp;
    enum SWI_TYPE_Device devtype;
    
    /* Set the eNotify field */
    notifyvariantp->eNotify = SWI_NOTIFY_AirServerChange;
    
    /* Initialize the package global control block pointer */
    apicbp = SwiApiGetCbp();

    /* Get pointers to the structures
     * to improve readability
     */
    aschgp = &notifyvariantp->_AirServerChange;
    asp    = &aschgp->AirServer;

    /* set structure lengths */
    aschgp->sizeStruct = sizeof(struct SWI_STRUCT_AirServerChange);
    asp->sizeStruct    = sizeof( struct SWI_STRUCT_AirServer );

    /* Move the fields over into the variant structure */
    asp->eDevice = amairparmsp->amairdevicetype;
    
    /* Copy the MAC address fields into the variant locations */
    slmemcpy( asp->ClientMac, &amairparmsp->amairclimac[0], AMMACSIZE );
    slmemcpy( asp->ServerMac, &amairparmsp->amairservmac[0], AMMACSIZE );

    /* Get apprunning field */
    apicbp->apapprunning                  = amairparmsp->amairapprunning;

    /* Now fill in the fields of SWI_STRUCT_AirServerChange */
    aschgp->bInsert = amairparmsp->amairdevicestat;
    
    /* Only update the local information if the 
     * incoming era number is newer than the one
     * we have stored
     */
    if(apicbp->apairstate.apairchgera < amairparmsp->amairchgera)
    {
        /* Record the air server change locally */
        apicbp->apairstate.apairserverpresent = amairparmsp->amairdevicestat;
        
        apicbp->apairstate.apairchgera        = amairparmsp->amairchgera;
        if ( asp->eDevice == SWI_DEVICE_UNIFIEDPID )
        {
            slmemcpy( &apicbp->apairstate.apairprodstr[0], 
                      &amairparmsp->amairprodstr[0], AMPRODSTRSIZE );
              
            SwiIntMapProdStrToDevType (&amairparmsp->amairprodstr[0], 
                                       (swi_uint32 *)&devtype);

            apicbp->apairstate.apairdevice = devtype;
            asp->eDevice = devtype;
        }
        else
            apicbp->apairstate.apairdevice        = asp->eDevice;
        
    }

    return SWI_RCODE_OK;
}

/**
 *
 * Name:    SwiDecodeNotify
 *
 * Purpose: This function decodes incoming notifications and invokes
 *          the appropriate handler for the specific type of notification
 *             
 * Parms:   apamcnsrrparm[IN]:     CnS parameter storage structure
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:    pointer to union structure SwiNotifyVariant
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   aborts on unsupported object id 
 *
 * Notes:   None
 *          
 *
 *
 */
package SWI_RCODE SwiDecodeCnsNotify(struct amcnsrrparms   apamcnsrrparm, 
                                  swi_uint8 *paramfldp, 
                                  struct SwiNotifyVariant *notifyvariantp)
{
    const struct swinotifyhandler *handlerblkp = NULL;

    handlerblkp = swigetnotifyhandler(apamcnsrrparm.amcnsobjid);

    if((handlerblkp == NULL) || handlerblkp->swihandlerfp == NULL)
    {
        /* this is an unhandled notification                   */
        /* for now abort...in the future we should not abort   */
        /* since it is possible for newer firmware to send us  */
        /* a unknown notification - it would be bad to go down */
        /* then.  (TBD- Rachel )                               */
        erAbort("SwiDecodeNotify: Unsupported notification", 
                apamcnsrrparm.amcnsobjid); 
        return SWI_RCODE_FAILED;
    }

    /* set notification type */
    notifyvariantp->eNotify = handlerblkp->swinotifytype;

    /* call notification's handler function to decode its' contents */
    handlerblkp->swihandlerfp( paramfldp, notifyvariantp );

    return SWI_RCODE_OK;
};
/**
 *
 * Name:    SwiDecodeFwDwnNotify
 *
 * Purpose: This function prepares the Firmware Download notification.
 *             
 * Parms:   apamfwdwnrrparm[IN]: fimware download message structure
 *          notifyvariantp[OUT]:  pointer to union structure SwiNotifyVariant
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   aborts on unsupported object id 
 *
 * Notes:   None
 *          
 *
 *
 */
package SWI_RCODE SwiDecodeFwDwnNotify(
    struct amfwdwnrrparms   apamfwdwnrrparm,
    struct SwiNotifyVariant *notifyvariantp)
{
    /* set notification type */
    notifyvariantp->eNotify = SWI_NOTIFY_FwDownload;
    
    /* set notification size */
    notifyvariantp->_FwDownload.sizeStruct = 
        sizeof(struct SWI_STRUCT_FwDownload);
     
    /* fill in firmware download notification state information */
    notifyvariantp->_FwDownload.State = apamfwdwnrrparm.amfwdwnstate;

    return SWI_RCODE_OK;
};

/* Technology specific decode functions below here. These are common to both 
 * CDMA & UMTS which are better grouped together than separated.
 */

/* CWE */
/**
 *
 * Name:    swinvstoredecode
 *
 * Purpose: Decodes NV store notification 
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
package SWI_RCODE swinvstoredecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    SwiUpkNvStore( paramfldp, &notifyvariantp->_NvStore ); 

    return SWI_RCODE_OK;
}

/**
 *
 * Name:    swinvupdatefromfiledecode
 *
 * Purpose: Decodes NV update fromfile notification 
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
package SWI_RCODE swinvupdatefromfiledecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    SwiUnpackNVUpdateFromFile( paramfldp, &notifyvariantp->_NVUpdateFromFile );

    return SWI_RCODE_OK;
}
/**
 *
 * Name:    swicurradiobanddecode
 *
 * Purpose: Decodes Current Radio Band notification 
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
package SWI_RCODE swicurradiobanddecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    SwiUpkCurrentRadioBand( paramfldp, &notifyvariantp->_RadioBand); 

    return SWI_RCODE_OK;
}
/**
 *
 * Name:    swiradiopowerdecode
 *
 * Purpose: Decodes Radio Power notification
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
package SWI_RCODE swiradiopowerdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    SwiUpkRadioPowerNotify( paramfldp, &notifyvariantp->_RadioPower); 

    return SWI_RCODE_OK;
}
/**
 *
 * Name:    swiradiotempdecode
 *
 * Purpose: Decodes Radio Temperature notification 
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
package SWI_RCODE swiradiotempdecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    SwiUpkRadioTemp( paramfldp, &notifyvariantp->_RadioTempState); 

    return SWI_RCODE_OK;
}
/**
 *
 * Name:    swiradiobatterydecode
 *
 * Purpose: Decodes Radio Battery notification
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
package SWI_RCODE swiradiobatterydecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    SwiUpkRadioBattery( paramfldp, &notifyvariantp->_RadioBattery); 

    return SWI_RCODE_OK;
}

/**
 *
 * Name: swidevicetime
 *
 * Purpose: Decodes GSM Device Time notification 
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
 * Notes:   This function handles GSM Device Time notification data 
 *          and stores these in the supplied notification structure
 *
 */
package SWI_RCODE  swidevicetime( 
                        swi_uint8 *paramfldp, 
                        struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    SwiUpkDeviceTime ( paramfldp, &notifyvariantp->_DeviceTime); 

    return SWI_RCODE_OK;

}

/* LBS - CDMA */
/*************
 *
 * Name:    swilbspagpslockdecode
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
package SWI_RCODE swilbspagpslockdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_LbsPaGpsLock.sizeStruct = sizeof(SWI_STRUCT_LbsPaGpsLock);

    piget16(&paramfldp);    /* Discard obj_ver */
    notifyvariantp->_LbsPaGpsLock.eGpsLock = piget16(&paramfldp);
    
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swilbspaptlmmodedecode
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
package SWI_RCODE swilbspaptlmmodedecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_LbsPaPtlmMode.sizeStruct = 
            sizeof(SWI_STRUCT_LbsPaPtlmMode);

    piget16(&paramfldp);    /* Discard obj_ver */
    notifyvariantp->_LbsPaPtlmMode.eTransportMech = piget16(&paramfldp);

    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swilbspaprivacydecode
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
package SWI_RCODE swilbspaprivacydecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_LbsPaPrivacy.sizeStruct = sizeof(SWI_STRUCT_LbsPaPrivacy);

    piget16(&paramfldp);    /* Discard obj_ver */
    notifyvariantp->_LbsPaPrivacy.ePrivacy = piget16(&paramfldp);
    
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swilbspanetaccessdecode
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
package SWI_RCODE swilbspanetaccessdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_LbsPaNetAccess.sizeStruct = 
            sizeof(SWI_STRUCT_LbsPaNetAccess);

    piget16(&paramfldp);    /* Discard obj_ver */
    notifyvariantp->_LbsPaNetAccess.eNetAccess = piget16(&paramfldp);

    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swilbspabsinfodecode
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
package SWI_RCODE swilbspabsinfodecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_LbsPaBsInfo.sizeStruct = sizeof(SWI_STRUCT_LbsPaBsInfo);

    SwiUnpackLbsPaBsInfo( paramfldp, &notifyvariantp->_LbsPaBsInfo );
    
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swilbspddloadbegindecode
 *
 * Purpose: Decodes LBS PD DLOAD BEGIN notification
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
package SWI_RCODE swilbspddloadbegindecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_LbsPdDloadBegin.sizeStruct = sizeof(SWI_STRUCT_LbsPdDloadBegin);

    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swilbspddloaddatadecode
 *
 * Purpose: Decodes LBS PD DLOAD DATA notification
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
package SWI_RCODE swilbspddloaddatadecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    /* set structure length AND Unpack data */
    SwiUnpackLbsPdDloadData( paramfldp, &notifyvariantp->_LbsPdDloadData );

    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swilbspddloaddonedecode
 *
 * Purpose: Decodes LBS PD DLOAD DONE notification
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
package SWI_RCODE swilbspddloaddonedecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_LbsPdDloadDone.sizeStruct = sizeof(SWI_STRUCT_LbsPdDloadDone);
 
    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swilbspddloadenddecode
 *
 * Purpose: Decodes LBS PD DLOAD END notification
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
package SWI_RCODE swilbspddloadenddecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_LbsPdDloadEnd.sizeStruct = sizeof(SWI_STRUCT_LbsPdDloadEnd);
    
    piget16(&paramfldp);    /* Discard obj_ver */
    notifyvariantp->_LbsPdDloadEnd.eEndStatus = piget16(&paramfldp);

    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swilbsxtraassist
 *
 * Purpose: Decodes LBS Xtra Assist Data & LBS Xtra Time Info notifications
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:   pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swilbsxtraassist(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_LbsXtraAssist.sizeStruct =
            sizeof(SWI_STRUCT_LbsXtraAssist);

    piget16(&paramfldp);    /* Discard obj_ver */
    notifyvariantp->_LbsXtraAssist.status = *paramfldp++;

    return SWI_RCODE_OK;
}

/*************
 *
 * Name:    swilbsxtradloadtrigger
 *
 * Purpose: Decodes LBS Xtra Dload Trigger notification
 *
 * Parms: 
 *          paramfldp[IN]:         pointer to parameter field in notification
 *          notifyvariantp[OUT]:   pointer to global notification structure 
 *                                 for storing notification results
 *
 * Return:  See SWI_RCODE for additional information.  
 *
 * Abort:   none
 *
 * Notes:   
 *
 **************/
package SWI_RCODE swilbsxtradloadtrigger(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    notifyvariantp->_LbsXtraDloadTrigger.sizeStruct =
            sizeof(SWI_STRUCT_LbsXtraDloadTrigger);

    piget16(&paramfldp);    /* Discard obj_ver */
    notifyvariantp->_LbsXtraDloadTrigger.operation = *paramfldp++;
    notifyvariantp->_LbsXtraDloadTrigger.failoverThreshold =
            piget32(&paramfldp);

    return SWI_RCODE_OK;
}

/* LBS - UMTS */
/**
 *
 * Name: swilbspawarmstatusdecode
 *
 * Purpose: Decodes LBS PA Warm Status notification
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
package SWI_RCODE swilbspawarmstatusdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    /* set structure length */
    notifyvariantp->_LbsPaWarmStatus.sizeStruct = 
                    sizeof(struct SWI_STRUCT_LbsPaWarmStatus);

    /* Unpack data */
    SwiUpkLbsPaWarmStatus(paramfldp,  &notifyvariantp->_LbsPaWarmStatus);

    return SWI_RCODE_OK;

}

/**
 *
 * Name: swilbspawarmbegindecode
 *
 * Purpose: Decodes LBS PA Warm Begin notification
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
package SWI_RCODE swilbspawarmbegindecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    /* set structure length */
    notifyvariantp->_LbsPaWarmBegin.sizeStruct = 
                    sizeof(struct SWI_STRUCT_LbsPaWarmBegin);

    return SWI_RCODE_OK;

}
/**
 *
 * Name: swilbspawarmdonedecode
 *
 * Purpose: Decodes LBS PA Warm Done notification
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
package SWI_RCODE swilbspawarmdonedecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    /* set structure length */
    notifyvariantp->_LbsPaWarmDone.sizeStruct = 
                    sizeof(struct SWI_STRUCT_LbsPaWarmDone);

    return SWI_RCODE_OK;

}
/**
 *
 * Name: swilbsnireqdecode
 *
 * Purpose: Decodes LBS NI Request notification
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
package SWI_RCODE swilbsnireqdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    /* set structure length */
    notifyvariantp->_LbsNiReq.sizeStruct = 
                    sizeof(struct SWI_STRUCT_LbsNiReq);

    /* Unpack data */
    SwiUpkLbsNiReqNotify(paramfldp, &notifyvariantp->_LbsNiReq);
                         
    return SWI_RCODE_OK;
}

/* LBS - Common */
/**
 *
 * Name: swilbspddatadecode
 *
 * Purpose: Decodes LBS PD Data notification
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
package SWI_RCODE swilbspddatadecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    /* set structure length AND Unpack data */
    SwiUnpackLbsPdData(paramfldp, &notifyvariantp->_LbsPdData);

    return SWI_RCODE_OK;

}

/**
 *
 * Name: swilbspddonedecode
 *
 * Purpose: Decodes LBS PD Done notification
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
package SWI_RCODE swilbspddonedecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    /* set structure length */
    notifyvariantp->_LbsPdDone.sizeStruct = 
                    sizeof(struct SWI_STRUCT_LbsPdDone);

    return SWI_RCODE_OK;

}

/**
 *
 * Name: swilbspdenddecode
 *
 * Purpose: Decodes LBS PD End notification
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
package SWI_RCODE swilbspdenddecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    /* set structure length */
    notifyvariantp->_LbsPdEnd.sizeStruct = 
                    sizeof(struct SWI_STRUCT_LbsPdEnd);

    piget16(&paramfldp);    /* Discard obj_ver */
    notifyvariantp->_LbsPdEnd.eEndStatus = piget16(&paramfldp);
    return SWI_RCODE_OK;

}

/**
 *
 * Name: swilbspdbegindecode
 *
 * Purpose: Decodes LBS PD Begin notification
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
package SWI_RCODE swilbspdbegindecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    /* set structure length */
    notifyvariantp->_LbsPdBegin.sizeStruct = 
                    sizeof(struct SWI_STRUCT_LbsPdBegin);

    return SWI_RCODE_OK;

}

/**
 *
 * Name: swilbspdupdatefailuredecode
 *
 * Purpose: Decodes LBS PD Update Failure notification
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
package SWI_RCODE swilbspdupdatefailuredecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    /* set structure length */
    notifyvariantp->_LbsPdUpdateFailure.sizeStruct = 
                    sizeof(struct SWI_STRUCT_LbsPdUpdateFailure);

    return SWI_RCODE_OK;

}

/**
 *
 * Name: swilbspdgetstatusdecode
 *
 * Purpose: Decodes LBS PD Get Status notification
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
package SWI_RCODE swilbspdgetstatusdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    /* set structure length AND Unpack data */
    SwiUnpackLbsPdStatus(paramfldp, &notifyvariantp->_LbsPdGetStatus);

    return SWI_RCODE_OK;

}


/**
 *
 * Name: swilbssatinfodecode
 *
 * Purpose: Decodes LBS satelite info notification
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
package SWI_RCODE swilbssatinfodecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{

    /* set structure length AND Unpack data */
    SwiUnpackLbsSatInfo(paramfldp, &notifyvariantp->_LbsSatInfo);

    return SWI_RCODE_OK;

}

/**
 *
 * Name: swilbspaipaddrdecode
 *
 * Purpose: Decodes LBS param IP Address info notification
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
package SWI_RCODE swilbspaipaddrdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    /* set structure length */
    notifyvariantp->_LbsPaIpAddr.sizeStruct = 
                    sizeof(struct SWI_STRUCT_LbsPaIpAddr);

    /* Unpack data */
    piget16(&paramfldp);    /* Discard obj_ver */
    notifyvariantp->_LbsPaIpAddr.nIpAddress = piget32(&paramfldp);

    return SWI_RCODE_OK;

}

/**
 *
 * Name: swiaudioprofile
 *
 * Purpose: Decodes Audio Profile notification 
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
 * Notes:   This function handles Audio Profile notification data 
 *          and stores these in the supplied notification structure
 *
 */
package SWI_RCODE  swiaudioprofile( 
                        swi_uint8 *paramfldp, 
                        struct SwiNotifyVariant *notifyvariantp)
{
    /* Unpack data */
    notifyvariantp->_AudioProfile.sizeStruct = 
                    sizeof(struct SWI_STRUCT_AudioProfile);
    
    SwiUpkAudioProfile(paramfldp, &notifyvariantp->_AudioProfile);

    return SWI_RCODE_OK;

}


/**
 *
 * Name: swilbspaportiddecode
 *
 * Purpose: Decodes LBS param Port ID info notification
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
package SWI_RCODE swilbspaportiddecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp)
{
    /* set structure length */
    notifyvariantp->_LbsPaPortId.sizeStruct = 
                    sizeof(struct SWI_STRUCT_LbsPaPortId);

    /* Unpack data */
    piget16(&paramfldp);    /* Discard obj_ver */
    notifyvariantp->_LbsPaPortId.nPortId = piget32(&paramfldp);

    return SWI_RCODE_OK;

}

/*
 * $Log: SwiIntDecodeNotify.c,v $
 */
