/*
 * $Id: cnudefs.h,v 1.17 2011/02/11 22:09:06 ygao Exp $
 *
 * Filename:  cnudefs.h
 *
 * Purpose:   User definitions for CN package
 *
 * Copyright: (c) 2011 Sierra Wireless Inc., all rights reserved
 *
 */

#ifndef cnudefs_h
#define cnudefs_h

/* Includes */
#include "am/amudefs.h"
#include "ap/SwiRcodes.h"

/* Defines */

/* Constant to write into the Application Parameter field of the 
 * CnS packet
 */
#define CNSAPPPARAMVALUE  0x0000  /* Unused field and set to zero */

/* CnS Parameter field maximum size */
#define CNSPARAMMAX   1024 

/* CnS Request Block Size. This sizes big enough for the 
 * largest possible CnS request that can be exchanged between 
 * the API and the SDK
 */
#define CNSREQBKSZ  (AMHEADERSZ + AMCNSREQSZ + CNSPARAMMAX)

/* CnS Response Block Size. This size is big enough for the 
 * largest possible CnS response that can be exchanged between 
 * the SDK and the API 
 */
#define CNSRESPBKSZ  (AMHEADERSZ + AMCNSRESPONSESZ + CNSPARAMMAX)

/* CNS Error storage array size. This is used to size the array
 * on the API side for storing the last error received from the
 * modem when the CnS Error Flag is set in a response header.
 * The size is the same as is defined in the Windows SDK
 */
#define CNFWERRORSZ    200

/* Items required for CnS object creation/parsing */

/* ID Of Modem's NV Ram item for Sid/Nid table */
#define CNSIDNIDTABLEID  0x103     

/*************
 *
 * Name:    CNSOPTYPES
 *
 * Purpose: enumerated list of CnS operation types. These types are
 *          filled into the CnS packet header by the sending device.
 *          
 * Members: CNS_MINOPERATION - For range checking purposes
 *          CNS_MAXOPERATION - For range checking purposes
 *          CNS_GETREQ   - Cns Get Request operation code
 *          CNS_GETRESP  - Cns response to a Get Request 
 *          CNS_SETREQ   - Cns Set Request operation code
 *          CNS_SETRESP  - CnS response to a Set Request
 *          CNS_NOTREQ   - Cns Notification Request operation code
 *          CNS_NOTRESP  - Cns Notification Response operation code
 *          CNS_NOTIFY   - Cns Notification operation code (modem to host)
 *          CNS_STOPREQ  - Cns Request to stop notifications 
 *          CNS_STOPRESP - Cns Notification stop response (modem to host)
 *          CNS_CHNGEREQ - Cns Change Notification Class request
 *          CNS_CHNGRESP - Cns Change Notification Class response
 *                       
 * Notes:   The numeric values for these requests are defined in Sierra 
 *          document, 2110636 "CWE Control and Status Protocol", section
 *          4.
 *
 *          Don't change the order or the numeration of this list
 *
 **************/
enum cnsoptypes {
    CNS_MINOPERATION = 0x1,
    CNS_GETREQ = CNS_MINOPERATION,  /* 0x01 */
    CNS_GETRESP,                    /* 0x02 */
    CNS_SETREQ,                     /* 0x03 */
    CNS_SETRESP,                    /* 0x04 */
    CNS_NOTREQ,                     /* 0x05 */
    CNS_NOTRESP,                    /* 0x06 */
    CNS_NOTIFY,                     /* 0x07 */
    CNS_STOPREQ,                    /* 0x08 */
    CNS_STOPRESP,                   /* 0x09 */
    CNS_CHNGREQ,                    /* 0x0A */
    CNS_CHNGRESP,                   /* 0x0B */
    CNS_MAXOPERATION                /* 0x0C */
};

/*************
 *
 * Name:    CNSOBJECTIDS
 *
 * Purpose: Enumerated List of CnS objects. These objects are
 *          known to the modem as well and must match their
 *          associated definitions within the Sierra Wireless
 *          documentation
 *          
 * Members:     CNS_MIN_ID = 0x0,
 *              CNS_HEART_BEAT                = CNS_MIN_ID,
 *              // CNS Objects - common to all projects 
 *              CNS_FIRMWARE_VERSION,
 *              CNS_FIRMWARE_BUILD_DATE,
 *              CNS_HARDWARE_VERSION,
 *              CNS_BOOT_VERSION,
 *              CNS_OBJECT_REVISION,
 *              CNS_OBJECT_REVISION_EXP,       // = 0x0006
 *              CNS_RADIO_POWER,               // = 0x0007
 *              CNS_RADIO_TEMPERATURE,         // = 0x0008
 *              CNS_RADIO_BATTERY,             // = 0x0009
 *              CNS_OBJECT_RANGE_REVISION      = 0x000C,
 *              CNS_BOOT_BUILD_DATE            = 0x000D,
 *              CNS_USBD_BUILD_INFO,           // = 0x000E
 *              CNS_HEADSET_ICON,              // = 0x000F
 *              CNS_RESET_DEVICE               = 0x0010,
 *              CNS_NVUPDATE_FROM_FILE         = 0x0011,
 *              CNS_API_UNLOCK                 = 0x0012,
 *              CNS_OEM_OPENLOCK               = 0x0013,
 *              CNS_SWI_OPENLOCK               = 0x0014,
 *              CNS_API_SETLOCK                = 0x0015,
 *              CNS_CURRENT_RADIO_BAND         = 0x0016,
 *              CNS_CONFIG_RADIO_BAND,         // = 0x0017
 *              CNS_DATA_STATS_RESET,           // 0x0018 
 *              CNS_DATA_STATS_FETCH,           // 0x0019 
 *              CNS_AUDIO_PROFILE              = 0x001A,
 *              CNS_DEVICE_TIME,               // = 0x001B
 *              CNS_PRI_INFO                   = 0x0023,
 *              CNS_FLASH_IMAGE_INFO           = 0x0024,
 *              CNS_RM_CONFIG,                 // 0x0025
 *              CNS_RDA_SUPPORT,             // 0x0026
 *              //-----------------------------------------------
 *              // LBS related Items
 *              CNS_LBS_OBJECT_START           = 0x0F00,
 *              CNS_LBS_PD_DOWNLOAD,           // = 0x0F01
 *              CNS_LBS_PD_GET_POS,            // = 0x0F02
 *              CNS_LBS_PD_GET_STATUS,         // = 0x0F03
 *              CNS_LBS_PD_TRACK,              // = 0x0F04
 *              CNS_LBS_PD_END_SESSION,        // = 0x0F05
 *              CNS_LBS_PA_GET_PARAM,          // = 0x0F06
 *              CNS_LBS_LOC_NOTIFY_STATUS,     // = 0x0F07
 *              CNS_LBS_DLOAD_NOTIFY_STATUS,   // = 0x0F08
 *              CNS_LBS_PARAM_NOTIFY_STATUS,   // = 0x0F09
 *              CNS_LBS_PD_DATA,               // = 0x0F0A
 *              CNS_LBS_PD_DONE,               // = 0x0F0B
 *              CNS_LBS_PD_END,                // = 0x0F0C
 *              CNS_LBS_PD_BEGIN,              // = 0x0F0D
 *              CNS_LBS_PD_UPDATE_FAILURE,     // = 0x0F0E
 *              CNS_LBS_PD_DLOAD_BEGIN,        // = 0x0F0F
 *              CNS_LBS_PD_DLOAD_DATA,         // = 0x0F10
 *              CNS_LBS_PD_DLOAD_DONE,         // = 0x0F11
 *              CNS_LBS_PD_DLOAD_END,          // = 0x0F12
 *              CNS_LBS_PA_IP_ADDR,            // = 0x0F13
 *              CNS_LBS_PA_GPS_LOCK,           // = 0x0F14
 *              CNS_LBS_PA_PTLM_MODE,          // = 0x0F15
 *              CNS_LBS_PA_PORT_ID,            // = 0x0F16
 *              CNS_LBS_PA_PRIVACY,            // = 0x0F17
 *              CNS_LBS_PA_NET_ACCESS,         // = 0x0F18
 *              CNS_LBS_PA_BS_INFO,            // = 0x0F19
 *              CNS_LBS_PA_KEEP_WARM_START,    // = 0x0F1A
 *              CNS_LBS_PA_WARM_BEGIN,         // = 0x0F1B
 *              CNS_LBS_PA_WARM_STATUS,        // = 0x0F1C
 *              CNS_LBS_PA_WARM_DONE,          // = 0x0F1D
 *              CNS_LBS_PA_KEEP_WARM_STOP,     // = 0x0F1E
 *              CNS_LBS_SAT_INFO,              // = 0x0F1F
 *              CNS_LBS_CLEAR_ASSISTANCE,      // = 0x0F20
 *              CNS_LBS_SETTINGS,              // = 0x0F21
 *              CNS_LBS_NI_REQUEST,            // = 0x0F22
 *              CNS_LBS_EXT_ASSIST,            // = 0x0F23
 *              CNS_LBS_OBJECT_MAX,
 *              CNS_LBS_XTRA_ASSIST_DATA       // = 0x0F25,
 *              CNS_LBS_XTRA_TIME_INFO         // = 0x0F26,
 *              CNS_LBS_XTRA_DATA_URL          // = 0x0F27,
 *              CNS_LBS_XTRA_TIME_URL          // = 0x0F28,
 *              CNS_LBS_XTRA_DLOAD_TRIGGER     // = 0x0F29,
 *           
 *              // MP objects
 *              CNS_MP_ENABLE_NOT              = 0x6000,
 *              CNS_MP_DISABLE_NOT,            // = 0x6001,
 *              CNS_MP_EXT_DIO_CFG,            // = 0x6002,
 *              CNS_MP_EXT_DIO_RD,             // = 0x6003,
 *              CNS_MP_EXT_AI_CFG,             // = 0x6004,
 *              CNS_MP_EXT_AI_RD,              // = 0x6005,
 *              CNS_MP_GPS_DATA,               // = 0x6006,
 *              CNS_MP_GPS_CMD,                // = 0x6007
 *              CNS_MP_GPS_RESET,              // = 0x6008
 *              CNS_MP_OBJECT_REVISION,        // = 0x6009
 *              CNS_MP_IGN_SENSE,              // = 0x600A
 *              CNS_MP_DATA_COUNT,             // = 0x600B
 *              CNS_MP_CALL_BYTE_CNT,          // = 0x600C
 *              CNS_MP_BYTE_CNT_RESET,         // = 0x600D
 *              CNS_MP_SIM_SLED,               // = 0x600E
 *              CNS_MP_RF_DNLD,                // = 0x600F
 *              CNS_MP_BAT_STATUS,             // = 0x6010
 *              CNS_MP_NETWORK_CONF,           // = 0x6011
 *              CNS_MP_FIRMWARE_VERSION,       // = 0x6012
 *              CNS_MP_FIRMWARE_BUILD_DATE,    // = 0x6013
 *              CNS_MP_BOOT_LOADER_VERSION,    // = 0x6014
 *              CNS_MP_HARDWARE_VERSION,       // = 0x6015
 *              CNS_MP_HW_BUILD_DATE,          // = 0x6016
 *              CNS_MP_BATCH_NUMBER,           // = 0x6017
 *              CNS_MP_FSN,                    // = 0x6018
 *              CNS_MP_TEST_HISTORY,           // = 0x6019
 *              CNS_MP_NAME,                   // = 0x601A
 *              CNS_MP_SHUTDOWN,               // = 0x601B
 *              CNS_MP_CNX_SETUP,              // = 0x601C
 *              CNS_MP_CNX_SETUP_RD,           // = 0x601D
 *              CNS_MP_STARTUP_DIAG,           // = 0x601E
 *              CNS_MP_CONNECT,                // = 0x601F
 *              CNS_MP_STARTUP_CONFIG,         // = 0x6020
 *              CNS_MP_DISCONNECT,             // = 0x6021
 *              CNS_MP_ACTIVE_PROFILE,         // = 0x6022
 *              CNS_MP555_CNX_SETUP,           // = 0x6023
 *              CNS_MP555_CNX_SETUP_RD,        // = 0x6024
 *              CNS_MP_RNAP_CFG                = 0x6026,
 *              CNS_MP_MT_CFG,                 // = 0x6027
 *              CNS_MP_MT_RD,                  // = 0x6028
 *              CNS_MP_CW_CFG,                 // = 0x6029    note: naming inconsistency stems from FW
 *              CNS_MP_GPS_INIT_CFG,           // = 0x602A
 *              CNS_MP_GPS_INIT_RD,            // = 0x602B
 *              CNS_MP_GPS_CFG,                // = 0x602C
 *              CNS_MP_FW_REMOVAL,             // = 0x602D
 *              CNS_MP_WAP_CFG,                // = 0x602E
 *              CNS_MP_UNLOCK,                 // = 0x602F
 *              CNS_MP_UNLOCK_CONFIG,          // = 0x6030
 *              CNS_MP_FEATURES,               // = 0x6031
 *              CNS_MP_FEATURE_CFG,            // = 0x6032
 *           
 *              CNS_NETWORK_STATE              = 0x1000,
 *              CNS_RSSI,                      //0x1001,
 *              CNS_ROAMING_DISABLE,           //0x1002,
 *              CNS_SERVICE_PROVIDER,          //0x1003,
 *              CNS_IMSI,                      //0x1004,
 *              CNS_HW_BUILD_DATE,             //0x1005,
 *              CNS_SERVICE_ICON,              //0x1006,    //previously CNS_IMEI 
 *              CNS_TTY_CONTROL,               //0x1007,    //previously CNS_SIM_VERIFY
 *              CNS_CALL_CONTROL_EXP,          //0x1008, 
 *              CNS_SS_USSD_REQ_EXP,           //0x1009,
 *              CNS_SIM_STATUS,                //0x100A,
 *              CNS_BAND,                      //0x100B,
 *              CNS_MIC_MUTE,                  //0x100C,
 *              CNS_SPKR_MUTE,                 //0x100D,
 *              CNS_SPKR_VOL,                  //0x100E,
 *              CNS_PLMN_MODE,                 //0x100F,
 *              CNS_MANUAL_SELECT,             //0x1010,
 *              CNS_MODEM_DISABLE,             //0x1011,
 *              CNS_TRANSMIT_SPEED,            //0x1012,
 *              CNS_TRANSMIT_ALERT,            //0x1013,
 *              CNS_FAX_INCOMING,              //0x1014,
 *              CNS_HEADSET_STATE,             //0x1015,
 *              CNS_FSN,                       //0x1016,
 *              CNS_THB,                       //0x1017,
 *              CNS_REGISTRATION_IND,          //0x1018,
 *              CNS_MEP_UNLOCK,                //0x1019,
 *              CNS_CUSTOMIZATIONS,            //0x101A,
 *              CNS_AVAILABLE_FEATURES,        //0x101B,
 *              CNS_IP_ADDRESS,                //0x101C,
 *              CNS_FPGA_VERSION,              //0x101D,
 *              CNS_TEMPERATURE,            
 *                                             //0x101E,
 *              CNS_PKT_SESSION_STATUS,        //0x101F,
 *              CNS_SMS_RECEIVED_MSG_STATUS    = 0x1020,
 *              CNS_SMS_UNREAD_RETRIEVE,       //0x1021,
 *              CNS_SMS_READY_STATUS,          //0x1022,
 *              CNS_SMS_READ_RETRIEVE          = 0x1023,
 *                                             //0x1024,
 *              CNS_SMS_STORE                  = 0x1025,
 *              CNS_SMS_SEND_SMS,              //0x1026,
 *              CNS_SMS_SEND_REFERENCE,        //0x1027,
 *              CNS_SMS_SEND_STATUS,           //0x1028,
 *              CNS_SMS_DELETE,                //0x1029,
 *              CNS_SMS_SIM_FULL,              //0x102A,
 *              CNS_SMS_PARAM,                 //0x102B,
 *              CNS_MISSED_CALL,               //0x102C,
 *              CNS_CALL_CREATE                = 0x102D,
 *              CNS_CALL_CONTROL,              //0x102E,
 *              //CNS_CALL_PROGRESS,           //0x102F,
 *              //CNS_CALL_STATUS              = 0x1030,
 *              CNS_PHONE_BOOK_SIZE            = 0x1031,
 *              CNS_PHONE_NUM_READ,            //0x1032,
 *              CNS_PHONE_NUM_ADD,             //0x1033,
 *              CNS_PHONE_NUM_EDIT,            //0x1034,
 *              CNS_PHONE_NUM_DEL,             //0x1035,
 *              CNS_TOTAL_UPTIME,              //0x1036,
 *              CNS_CALL_BYTE_CNT,             //0x1037,
 *              CNS_BYTE_CNT_RESET,            //0x1038,
 *              CNS_DTMF_SEND,                 //0x1039,
 *              CNS_NV_TONE_STORE,             //0x103A,
 *              CNS_AUDIO_COMFORT,             //0x103B,
 *              CNS_MANUAL_SELECT_READY,       //0x103C,
 *              CNS_ICON_DISPLAY,              //= 0x103D, deprecated
 *              CNS_SIM_VERIFY_EXP             = 0x103E,
 *              CNS_CHV_EN_EXP,                // 0x103F
 *              CNS_CHV_CHG_EXP,               // 0x1040
 *              CNS_SIM_STATUS_EXP,
 *              CNS_MANUAL_SELECT_EXP,
 *              CNS_REGISTRATION_IND_EXP,
 *              CNS_LAST_NUM_DEL               = 0x1044,
 *              CNS_PHONE_NUM_READ_EXP         = 0x1045,
 *              CNS_PHONE_NUM_ADD_EXP          = 0x1046,
 *              CNS_PHONE_NUM_EDIT_EXP         = 0x1047,
 *              CNS_PHONE_BOOK_READY_STATUS    = 0x1048,
 *              CNS_PHONE_BOOK_AVAIL,
 *              CNS_PHONE_BOOK_SIZE_EXP,
 *              CNS_CSC_CIPHER_STATUS,
 *              CNS_GPRS_CIPHER_STATUS,
 *              CNS_EMERGENCY_NUM,
 *              CNS_SS_REQ                     = 0x104E,
 *              CNS_SS_ABORT_REQ,
 *              CNS_SS_PWD_REG                 = 0x1050,
 *              CNS_SS_PWD_STS                 = 0x1051,
 *              CNS_SS_OK_RSP,
 *              CNS_SS_FWD_INFO_RSP,
 *              CNS_SS_CALL_BAR_RSP,
 *              CNS_SS_INFO_DATA_RSP,
 *              CNS_SS_STATUS_RSP,
 *              CNS_SS_FWD_NUM_RSP,
 *              CNS_SS_BS_LIST_RSP,
 *              CNS_SS_FWD_LIST_RSP,
 *              CNS_SS_RAW_RSP,
 *              CNS_SS_PWD_RSP                 = 0x105B,
 *              CNS_SS_CLIR_RSP,
 *              CNS_SS_ERR_RSP                 = 0x105D,
 *              CNS_SS_REJECT_RSP              = 0x105E,
 *              CNS_SS_CANCEL_RSP,             // 0x105F
 *              CNS_SS_RELEASE_RSP,            // 0x1060
 *              CNS_SS_NOTIFY,                 // 0x1061
 *              CNS_VMWF,                      // 0x1062
 *              CNS_CARD_TYPE,
 *           
 *              CNS_CALL_TIMER                 = 0x1064,
 *              CNS_DATA_COUNT,                // 0x1065
 *              CNS_CALL_PROGRESS_EXP          = 0x1066,
 *              CNS_IMEI_EXP                   = 0x1067,
 *           
 *              CNS_ICON_GPRS                  = 0x1069,
 *            
 *              CNS_ICON_ROAM,                 //= 0x106A
 *              CNS_BAND_WRITE                 = 0x106B,
 *              CNS_CONT_DTMF,                 // = 0x106C
 *              CNS_UDUB                       // = 0x106D
 *              CNS_CALL_FORWARDED,            // = 0x106E
 *              CNS_CALL_STATUS_EXP,           // = 0x106F
 *              CNS_ICON_CF,                   // = 0x1070
 *              CNS_HOST_SW_STATE,             // = 0x1071
 *              CNS_EMERG_NUM_EXP,             // = 0x1072
 *              CNS_STK_CMD,                   // = 0x1073,
 *              CNS_STK_RESP_USER_ACK,         // = 0x1074,
 *              CNS_STK_RESP_GET_INPUT,        // = 0x1075,
 *              CNS_STK_RESP_USER_CNF,         //= 0x1076,
 *              CNS_STK_RESP_SELECT_ITEM,      //= 0x1077,
 *              CNS_CLIR_SETTING               = 0x1078,
 *              CNS_FDN_MODE,                  //= 0x1079
 *              CNS_PHONE_NUM_DEL_EXP,         //= 0x107A
 *              CNS_CHV2_STATUS_KICK,          //= 0x107B
 *              CNS_DIS_ALL_NOT,               //= 0x107C,
 *              CNS_DATA_RATE,                 //= 0x107D,
 *              CNS_SS_REQ_EXP,                //= 0x107E
 *              CNS_APPLICATION_MODE           = 0x1080,
 *              CNS_MM_DEBUG_INFO,             //= 0x1081
 *              CNS_ENABLE_NOT,                //= 0x1082
 *              CNS_HOST_APP_LAUNCH,           //= 0x1083
 *              CNS_FEATURE_KEY,               //= 0x1084
 *              CNS_SIM_AUTH,                  //= 0x1085
 *              CNS_SS_USSD_REQ,               //= 0x1086
 *              CNS_SMS_STORE_EXP,             //= 0x1087
 *              CNS_SMS_SEND_EXP,              //= 0x1088
 *              CNS_SMS_RECEIVED,              //= 0x1089
 *              CNS_SMS_DELIVER,               //= 0x108A
 *              CNS_SMS_SUBMIT,                //= 0x108B
 *              CNS_SMS_COMMAND,               //= 0x108C
 *              CNS_SMS_STATUS_REPORT,         //= 0x108D
 *              CNS_SMS_STORAGE_TYPE,          //= 0x108E
 *              CNS_SMS_SIM_STATUS,            //= 0x108F
 *              CNS_SMS_MARK_READ,             //= 0x1090
 *              CNS_SMS_MT_GET,                //= 0x1091
 *              CNS_SMS_PDP_STATUS,            //= 0x1092
 *              CNS_SMS_CSCA,                  //= 0x1093
 *              CNS_SMS_CNMI,                  //= 0x1094
 *              // 3G objects
 *              // These objects should be moved into a different sub-class.  However, due to limitations
 *              // of the above layers, we can not move it at this time.
 *              CNS_NVRAM_STORE                = 0x7000,
 *              CNS_PROFILE_SUMMARY,           //= 0x7001
 *              CNS_PROFILE_READ,              //= 0x7002
 *              CNS_PROFILE_WRITE,             //= 0x7003
 *              CNS_PKT_SESSION_CALL,          //= 0x7004
 *              CNS_PKT_SESSION_STATUS_EXP,    //= 0x7005
 *              CNS_IP_ADDRESS_EXP,            //= 0x7006
 *              CNS_ICC_ID,                    //= 0x7007
 *              CNS_HSDPA_TEST_CHAN,           //= 0x7008
 *              CNS_CALL_CONTROL_UMTS,         //= 0x7009
 *              CNS_SYSTEM_STATUS,             //= 0x700A
 *              CNS_RSCP_ECIO,                 //= 0x700B
 *              CNS_GSM_CELL_INFO,             //= 0x700C
 *              CNS_SMS_STS_ENABLE,            //= 0x700D
 *              CNS_PROFILE_AUTH,              //= 0x700E
 *              CNS_RAT,                       //= 0x700F
 *              CNS_PRL_REGION,                //= 0x7010
 *              CNS_OBJECT_NULL                = 0xffff
 *                       
 * Notes:   
 *          Don't change the order or the numeration of this list
 *
 **************/
enum cnsobjectids {
    CNS_MIN_ID = 0x0,
    CNS_HEART_BEAT                = CNS_MIN_ID,
    /* CNS Objects - common to all projects */
    CNS_FIRMWARE_VERSION,
    CNS_FIRMWARE_BUILD_DATE,
    CNS_HARDWARE_VERSION,
    CNS_BOOT_VERSION,
    CNS_OBJECT_REVISION,
    CNS_OBJECT_REVISION_EXP,       /* = 0x0006 */
    CNS_RADIO_POWER,               /* = 0x0007 */
    CNS_RADIO_TEMPERATURE,         /* = 0x0008 */
    CNS_RADIO_BATTERY,             /* = 0x0009 */
    CNS_MODEM_TYPE                 = 0x000A,
    CNS_NVRAM_STORE                = 0x000B,
    CNS_OBJECT_RANGE_REVISION      = 0x000C,
    CNS_BOOT_BUILD_DATE            = 0x000D,
    CNS_USBD_BUILD_INFO            = 0x000E,
    CNS_HEADSET_ICON,              /* = 0x000F */
    CNS_RESET_DEVICE               = 0x0010,
    CNS_NVUPDATE_FROM_FILE         = 0x0011,
    CNS_API_UNLOCK                 = 0x0012,
    CNS_OEM_OPENLOCK               = 0x0013,
    CNS_SWI_OPENLOCK               = 0x0014,
    CNS_API_SETLOCK                = 0x0015,
    CNS_CURRENT_RADIO_BAND         = 0x0016,
    CNS_CONFIG_RADIO_BAND,         /* = 0x0017 */
    CNS_DATA_STATS_RESET,           /* 0x0018 */
    CNS_DATA_STATS_FETCH,           /* 0x0019 */
    CNS_AUDIO_PROFILE              = 0x001A,
    CNS_DEVICE_TIME,               /* = 0x001B */
    CNS_PRI_INFO                   = 0x0023,
    CNS_FLASH_IMAGE_INFO           = 0x0024,
    CNS_RM_CONFIG,                 /* 0x0025 */
    CNS_RDA_SUPPORT,               /* 0x0026 */
    CNS_SWoC_CONFIG,               /* 0x0027 */

    /*-----------------------------------------------*/
    /* OMADM related Items */
    CNS_DM_OBJECT_START             = 0x0E00,
    CNS_DM_CONFIG                   = CNS_DM_OBJECT_START,
    CNS_DM_SESSION_START,           /* 0x0E01 */
    CNS_DM_SESSION_CANCEL,          /* 0x0E02 */
    CNS_DM_SESSION_STATE,           /* 0x0E03 */
    CNS_DM_NI_ALERT,                /* 0x0E04 */
    CNS_DM_BOOT_ALGORITHM,          /* 0x0E05 */
    CNS_DM_NODE_VALUE,              /* 0x0E06 */
    CNS_DM_NODE_ADD_INTERIOR,       /* 0x0E07 */
    CNS_DM_NODE_ADD_LEAF,           /* 0x0E08 */
    CNS_DM_NODE_DELETE,             /* 0x0E09 */
    CNS_DM_NODE_EXECUTE,            /* 0x0E0A */
    CNS_DM_NODE_PROPERTY,           /* 0x0E0B */
    CNS_DM_UI_ALERT_DISPLAY,        /* 0x0E0C */
    CNS_DM_UI_ALERT_CONFIRM,        /* 0x0E0D */
    CNS_DM_UI_ALERT_INPUT,          /* 0x0E0E */
    CNS_DM_UI_ALERT_CHOICE,         /* 0x0E0F */ 
    /* DM-DL objects */
    CNS_DM_DL_PROGRESS,             /* 0x0E10 */
    CNS_DM_DL_ALERT,                /* 0x0E11 */
    CNS_DM_DL_DESCRIPTION,          /* 0x0E12 */
    CNS_DM_DL_INSTALL_STATUS,       /* 0x0E13 */ 
    
    CNS_DM_OBJECT_MAX,
    
    /*-----------------------------------------------*/
    /* LBS related Items */
    CNS_LBS_OBJECT_START           = 0x0F00,
    CNS_LBS_PD_DOWNLOAD,           /* = 0x0F01 */
    CNS_LBS_PD_GET_POS,            /* = 0x0F02 */
    CNS_LBS_PD_GET_STATUS,         /* = 0x0F03 */
    CNS_LBS_PD_TRACK,              /* = 0x0F04 */
    CNS_LBS_PD_END_SESSION,        /* = 0x0F05 */
    CNS_LBS_PA_GET_PARAM,          /* = 0x0F06 */
    CNS_LBS_LOC_NOTIFY_STATUS,     /* = 0x0F07 */
    CNS_LBS_DLOAD_NOTIFY_STATUS,   /* = 0x0F08 */
    CNS_LBS_PARAM_NOTIFY_STATUS,   /* = 0x0F09 */
    CNS_LBS_PD_DATA,               /* = 0x0F0A */
    CNS_LBS_PD_DONE,               /* = 0x0F0B */
    CNS_LBS_PD_END,                /* = 0x0F0C */
    CNS_LBS_PD_BEGIN,              /* = 0x0F0D */
    CNS_LBS_PD_UPDATE_FAILURE,     /* = 0x0F0E */
    CNS_LBS_PD_DLOAD_BEGIN,        /* = 0x0F0F */
    CNS_LBS_PD_DLOAD_DATA,         /* = 0x0F10 */
    CNS_LBS_PD_DLOAD_DONE,         /* = 0x0F11 */
    CNS_LBS_PD_DLOAD_END,          /* = 0x0F12 */
    CNS_LBS_PA_IP_ADDR,            /* = 0x0F13 */
    CNS_LBS_PA_GPS_LOCK,           /* = 0x0F14 */
    CNS_LBS_PA_PTLM_MODE,          /* = 0x0F15 */
    CNS_LBS_PA_PORT_ID,            /* = 0x0F16 */
    CNS_LBS_PA_PRIVACY,            /* = 0x0F17 */
    CNS_LBS_PA_NET_ACCESS,         /* = 0x0F18 */
    CNS_LBS_PA_BS_INFO,            /* = 0x0F19 */
    CNS_LBS_PA_KEEP_WARM_START,    /* = 0x0F1A */
    CNS_LBS_PA_WARM_BEGIN,         /* = 0x0F1B */
    CNS_LBS_PA_WARM_STATUS,        /* = 0x0F1C */
    CNS_LBS_PA_WARM_DONE,          /* = 0x0F1D */
    CNS_LBS_PA_KEEP_WARM_STOP,     /* = 0x0F1E */
    CNS_LBS_SAT_INFO,              /* = 0x0F1F */
    CNS_LBS_CLEAR_ASSISTANCE,      /* = 0x0F20 */
    CNS_LBS_SETTINGS,              /* = 0x0F21 */
    CNS_LBS_NI_REQUEST,            /* = 0x0F22 */
    CNS_LBS_EXT_ASSIST,            /* = 0x0F23 */

    /* GPS Extra Interface */
    CNS_LBS_XTRA_ASSIST_DATA       = 0x0F25,
    CNS_LBS_XTRA_TIME_INFO,        /* = 0x0F26 */
    CNS_LBS_XTRA_DATA_URL,         /* = 0x0F27 */
    CNS_LBS_XTRA_TIME_URL,         /* = 0x0F28 */
    CNS_LBS_XTRA_DLOAD_TRIGGER,    /* = 0x0F29 */
    CNS_LBS_OBJECT_MAX,

    CNS_NETWORK_STATE              = 0x1000,
    CNS_RSSI,                      /*0x1001,*/
    CNS_ROAMING_DISABLE,           /*0x1002,*/
    CNS_SERVICE_PROVIDER,          /*0x1003,*/
    CNS_IMSI,                      /*0x1004,*/
    CNS_HW_BUILD_DATE,             /*0x1005,*/
    CNS_SERVICE_ICON,              /*0x1006,*/    /*previously CNS_IMEI*/ 
    CNS_TTY_CONTROL,               /*0x1007,*/    /*previously CNS_SIM_VERIFY*/
    CNS_CALL_CONTROL_EXP,          /*0x1008,*/ 
    CNS_SS_USSD_REQ_EXP,           /*0x1009,*/
    CNS_SIM_STATUS,                /*0x100A,*/
    CNS_BAND                       = 0x100B,
    CNS_MIC_MUTE,                  /*0x100C,*/
    CNS_SPKR_MUTE,                 /*0x100D,*/
    CNS_SPKR_VOL,                  /*0x100E,*/
    CNS_PLMN_MODE,                 /*0x100F,*/
    CNS_MANUAL_SELECT,             /*0x1010,*/
    CNS_MODEM_DISABLE,             /*0x1011,*/
    CNS_TRANSMIT_SPEED,            /*0x1012,*/
    CNS_TRANSMIT_ALERT,            /*0x1013,*/
    CNS_FAX_INCOMING,              /*0x1014,*/
    CNS_HEADSET_STATE,             /*0x1015,*/
    CNS_FSN,                       /*0x1016,*/
    CNS_THB,                       /*0x1017,*/
    CNS_REGISTRATION_IND,          /*0x1018,*/
    CNS_MEP_UNLOCK                 = 0x1019,
    CNS_CUSTOMIZATIONS,            /*0x101A,*/
    CNS_AVAILABLE_FEATURES         = 0x101B,
    CNS_IP_ADDRESS,                /*0x101C,*/
    CNS_FPGA_VERSION,              /*0x101D,*/
    CNS_TEMPERATURE,            
                                   /*0x101E,*/
    CNS_PKT_SESSION_STATUS,        /*0x101F,*/
    CNS_SMS_RECEIVED_MSG_STATUS    = 0x1020,
    CNS_SMS_UNREAD_RETRIEVE,       /*0x1021,*/
    CNS_SMS_READY_STATUS,          /*0x1022,*/
    CNS_SMS_READ_RETRIEVE          = 0x1023,
                                   /*0x1024,*/
    CNS_SMS_STORE                  = 0x1025,
    CNS_SMS_SEND_SMS,              /*0x1026,*/
    CNS_SMS_SEND_REFERENCE,        /*0x1027,*/
    CNS_SMS_SEND_STATUS,           /*0x1028,*/
    CNS_SMS_DELETE,                /*0x1029,*/
    CNS_SMS_SIM_FULL,              /*0x102A,*/
    CNS_SMS_PARAM,                 /*0x102B,*/
    CNS_MISSED_CALL,               /*0x102C,*/
    CNS_CALL_CREATE                = 0x102D,
    CNS_CALL_CONTROL,              /*0x102E,*/
    /*CNS_CALL_PROGRESS, */          /*0x102F,*/
    /*CNS_CALL_STATUS              = 0x1030, */
    CNS_PHONE_BOOK_SIZE            = 0x1031,
    CNS_PHONE_NUM_READ,            /*0x1032,*/
    CNS_PHONE_NUM_ADD              = 0x1033,
    CNS_PST_NV                     = 0x1033, /* These IDs shared by CDMA & GSM*/
    CNS_PHONE_NUM_EDIT,            /*0x1034,*/
    CNS_PHONE_NUM_DEL,             /*0x1035,*/
    CNS_TOTAL_UPTIME,              /*0x1036,*/
    CNS_CALL_BYTE_CNT,             /*0x1037,*/
    CNS_BYTE_CNT_RESET,            /*0x1038,*/
    CNS_DTMF_SEND,                 /*0x1039,*/
    CNS_NV_TONE_STORE,             /*0x103A,*/
    CNS_AUDIO_COMFORT,             /*0x103B,*/
    CNS_MANUAL_SELECT_READY,       /*0x103C,*/
    CNS_ICON_DISPLAY,              /*= 0x103D, deprecated*/
    CNS_SIM_VERIFY_EXP             = 0x103E,
    CNS_CHV_EN_EXP,                /*0x103F*/
    CNS_CHV_CHG_EXP,               /*0x1040*/
    CNS_SIM_STATUS_EXP,
    CNS_MANUAL_SELECT_EXP,
    CNS_REGISTRATION_IND_EXP,
    CNS_LAST_NUM_DEL               = 0x1044,
    CNS_PHONE_NUM_READ_EXP         = 0x1045,
    CNS_PHONE_NUM_ADD_EXP          = 0x1046,
    CNS_PHONE_NUM_EDIT_EXP         = 0x1047,
    CNS_PHONE_BOOK_READY_STATUS    = 0x1048,
    CNS_PHONE_BOOK_AVAIL,
    CNS_PHONE_BOOK_SIZE_EXP,
    CNS_CSC_CIPHER_STATUS,
    CNS_GPRS_CIPHER_STATUS,
    CNS_EMERGENCY_NUM,
    CNS_SS_REQ                     = 0x104E,
    CNS_SS_ABORT_REQ,
    CNS_SS_PWD_REG                 = 0x1050,
    CNS_SS_PWD_STS                 = 0x1051,
    CNS_SS_OK_RSP,
    CNS_SS_FWD_INFO_RSP,
    CNS_SS_CALL_BAR_RSP,
    CNS_SS_INFO_DATA_RSP,
    CNS_SS_STATUS_RSP,
    CNS_SS_FWD_NUM_RSP,
    CNS_SS_BS_LIST_RSP,
    CNS_SS_FWD_LIST_RSP,
    CNS_SS_RAW_RSP,
    CNS_SS_PWD_RSP                 = 0x105B,
    CNS_SS_CLIR_RSP,
    CNS_SS_ERR_RSP                 = 0x105D,
    CNS_SS_REJECT_RSP              = 0x105E,
    CNS_SS_CANCEL_RSP,             /* 0x105F */
    CNS_SS_RELEASE_RSP,            /* 0x1060 */
    CNS_SS_NOTIFY,                 /* 0x1061 */
    CNS_VMWF,                      /* 0x1062 */
    CNS_CARD_TYPE,

    CNS_CALL_TIMER                 = 0x1064,
    CNS_DATA_COUNT,                /* 0x1065 */
    CNS_CALL_PROGRESS_EXP          = 0x1066,
    CNS_IMEI_EXP                   = 0x1067,

    CNS_ICON_GPRS                  = 0x1069,
  
    CNS_ICON_ROAM,                 /*= 0x106A */
    CNS_BAND_WRITE                 = 0x106B,
    CNS_CONT_DTMF,                 /* = 0x106C */
    CNS_UDUB,                      /* = 0x106D */
    CNS_CALL_FORWARDED,            /* = 0x106E */
    CNS_CALL_STATUS_EXP,           /* = 0x106F */
    CNS_ICON_CF                    = 0x1070,
    CNS_PH_SS_PREF                 = 0x1070,   /* CDMA & GSM Share this ID */
    CNS_HOST_SW_STATE,             /* = 0x1071 */
    CNS_EMERG_NUM_EXP,             /* = 0x1072 */
    CNS_STK_CMD,                   /* = 0x1073, */
    CNS_STK_RESP_USER_ACK,         /* = 0x1074, */
    CNS_STK_RESP_GET_INPUT,        /* = 0x1075, */
    CNS_STK_RESP_USER_CNF,         /*= 0x1076, */
    CNS_STK_RESP_SELECT_ITEM,      /*= 0x1077, */
    CNS_CLIR_SETTING               = 0x1078,
    CNS_MEID_ESN                   = 0x1078,  /* CDMA & GSM Share this ID */
    CNS_FDN_MODE,                  /*= 0x1079*/
    CNS_PHONE_NUM_DEL_EXP,         /*= 0x107A*/
    CNS_CHV2_STATUS_KICK,          /*= 0x107B*/
    CNS_DIS_ALL_NOT,               /*= 0x107C,*/
    CNS_DATA_RATE,                 /*= 0x107D,*/
    CNS_SS_REQ_EXP,                /*= 0x107E*/
    CNS_APPLICATION_MODE           = 0x1080,
    CNS_MM_DEBUG_INFO,             /*= 0x1081*/
    CNS_ENABLE_NOT,                /*= 0x1082*/
    CNS_HOST_APP_LAUNCH,           /*= 0x1083*/
    CNS_FEATURE_KEY,               /*= 0x1084*/
    CNS_SIM_AUTH,                  /*= 0x1085*/
    CNS_SS_USSD_REQ,               /*= 0x1086*/
    CNS_SMS_STORE_EXP,             /*= 0x1087*/
    CNS_SMS_SEND_EXP,              /*= 0x1088*/
    CNS_SMS_RECEIVED,              /*= 0x1089*/
    CNS_SMS_DELIVER,               /*= 0x108A*/
    CNS_SMS_SUBMIT,                /*= 0x108B*/
    CNS_SMS_COMMAND,               /*= 0x108C*/
    CNS_SMS_STATUS_REPORT,         /*= 0x108D*/
    CNS_SMS_STORAGE_TYPE,          /*= 0x108E*/
    CNS_SMS_SIM_STATUS,            /*= 0x108F*/
    CNS_SMS_MARK_READ,             /*= 0x1090*/
    CNS_SMS_MT_GET,                /*= 0x1091*/
    CNS_SMS_PDP_STATUS,            /*= 0x1092*/
    CNS_SMS_CSCA,                  /*= 0x1093*/
    CNS_SMS_CNMI,                  /*= 0x1094*/
    /* 3G objects */
    /* Note: These objects should be moved into a different sub-class.
     * However, due to limitations of the above layers, we can not move
     * it at this time.
     */
    CNS_NVRAM_STORE_OLD              = 0x7000,
    CNS_PROFILE_SUMMARY,           /*= 0x7001*/
    CNS_PROFILE_READ,              /*= 0x7002*/
    CNS_PROFILE_WRITE,             /*= 0x7003*/
    CNS_PKT_SESSION_CALL,          /*= 0x7004*/
    CNS_PKT_SESSION_STATUS_EXP,    /*= 0x7005*/
    CNS_IP_ADDRESS_EXP,            /*= 0x7006*/
    CNS_ICC_ID,                    /*= 0x7007*/
    CNS_HSDPA_TEST_CHAN,           /*= 0x700*/
    CNS_CALL_CONTROL_UMTS,         /*= 0x7009*/
    CNS_SYSTEM_STATUS,             /*= 0x700A*/
    CNS_RSCP_ECIO,                 /*= 0x700B*/
    CNS_GSM_CELL_INFO,             /*= 0x700C*/
    CNS_SMS_STS_ENABLE,            /*= 0x700D*/
    CNS_PROFILE_AUTH,              /*= 0x700E*/
    CNS_RAT,                       /*= 0x700F*/
    CNS_PRL_REGION,                /*= 0x7010*/
    CNS_OBJECT_NULL                = 0xffff,
    
    
    /* Below are CDMA CnS object */
    CNS_GEN_OBJECT_START            = 0x1000,
    CNS_ESN                         = CNS_GEN_OBJECT_START,
    CNS_RSSI_DUMMY,             /* 0x1001, CDMA & GSM share CNS_RSSI */
    CNS_PHONE_NUMBER,
    CNS_IP_ADDRESS_CDMA,
    CNS_CHANNEL_NUMBER  ,
    CNS_CHANNEL_STATE ,
    CNS_SERVICE_INDICATION  ,
    CNS_ROAMING_STATUS  ,
    CNS_PRL_VERSION ,
    CNS_ACTIVATION_STATUS ,
    CNS_ECIO  ,
    CNS_NAM_VALID_STATUS ,
    CNS_PRI_CHKSUM  ,
    CNS_V42BIS_ENABLED  ,
    CNS_AUTH_STATUS ,
    CNS_RESET_DEVICE_LEGACY ,
    CNS_CNS_MODE_STATUS ,            /* 0x1010 */
    CNS_DEBUG_STATUS  ,
    CNS_DEBUG_MSG_LVL ,
    CNS_TOTAL_UP_TIME ,
    CNS_ANSWER_STATE  ,
    CNS_SECURE_CHALLENGE  ,
    CNS_SECURE_REBUTTAL ,
    CNS_SECURE_COMMIT_UPDATE  ,
    CNS_LOCK_STATUS ,
    CNS_LOCK  ,
    CNS_CHANGE_LOCK_PIN ,
    CNS_TOTAL_CALL_DURATION ,
    CNS_PREV  ,
    CNS_HEADSET_ICON_LEGACY ,
    CNS_SELECT_NAM  ,
    CNS_ACTIVE_NAM,
    CNS_AUTO_NAM,                    /* 0x1020 */
    CNS_PORT,
    CNS_POWERDOWN ,
    CNS_RADIO_ENABLE  ,
    CNS_NDIS_POWERCTRL_STATUS ,
    CNS_API_FIRMWARE_HANDSHAKE  ,
    CNS_SPEAKER_VOL ,
    CNS_SPEAKER_MUTE  ,
    CNS_MIC_LEVEL ,
    CNS_MIC_MUTE_CDMA,
    CNS_TONE_LEVEL  ,
    CNS_TONE_MUTE ,
    CNS_DTMF_LEVEL  ,
    CNS_DTMF_MUTE ,
    CNS_DTMF_KEY  ,
    CNS_DTMF_DURATION ,
    CNS_KEY_PRESSED ,                /* 0x1030 */
    CNS_KEY_RELEASED  ,
    CNS_SYSTEM_TIME,
    CNS_PST_NV_DUMMY,           /* 0x1033,  CDMA & GSM share CNS_PST_NV */
    CNS_DTMF_BURST,
    CNS_ECHO_CANCEL,
    CNS_SPEAKER_LEVEL,
    CNS_PROVISIONING_DATE            = 0x1037,
    CNS_MOBILE_DIRECTORY_NUM,
    CNS_IMSI_CDMA,
    CNS_USER_ID,
    CNS_USER_PASSWORD,
    CNS_MIP_PROFILE_INFO,
    CNS_SIP_PROFILE_INFO,
    CNS_ACTIVE_PROFILE,
    CNS_NAM_INFO,                    /* 0x103F */
    CNS_CONT_DTMF_START                = 0x1040,
    CNS_CONT_DTMF_STOP,
    CNS_CONT_DTMF_MODE,
    CNS_AUTO_LOCK                    = 0x1043,
    CNS_E911_INFO,
    CNS_E911_MODE,
    CNS_TTY_OPTION,
    CNS_PST_SWINV                    = 0x1048,

    CNS_FILE_FREE_SPACE                = 0x1049,
    CNS_FILE_NAME,
    CNS_FILE_DELETE,
    CNS_FILE_WRITE,
    CNS_FILE_READ,
    CNS_MIP_SESSION_PW                = 0x104E,
    CNS_POWERSAVE_ENTER,
    CNS_POWERSAVE_EXIT,
    CNS_SECURE_MSL_REBUTTAL            = 0x1051,
    CNS_PRL_SIZE                    = 0x1052,
    CNS_PRL_READ,
    CNS_PRL_WRITE,
    CNS_PRL_VALID,
    CNS_MIP_ERR_CODE,
    CNS_AKEY,
    CNS_AKEY_CHKSUM,
    CNS_EXCESS_SPC_FAILURE,
    CNS_DMU_MN_AUTH                    = 0X105A,    /* DMU items */
    CNS_DMU_PK_INFO,
    CNS_DMU_PK_INDEX,
    CNS_DMU_MN_AUTH_RESET,

    /*-----------------------------------------------*/

    CNS_RUIM_STATUS             = 0x105E,
    CNS_RUIM_VERIFY_CHV,
    CNS_RUIM_UNBLOCK_CHV,
    CNS_RUIM_CHANGE_CHV,
    CNS_RUIM_ENDIS_CHV,
    CNS_FEATURE_KEY_CDMA                = 0x1063,
    CNS_AUTOCONNECT,


    /* ******************************************************************
     *           E            V            D            O
     * ******************************************************************
     */
    CNS_RSSI_ECIO                       = 0x1065,
    CNS_HDR_NOTIFICATION_STATUS         = 0x1066,
    CNS_HDR_RSSI                        = 0x1067,
    CNS_HDR_ROAM_STATUS                 = 0x1068,
    CNS_HDR_SRV_STATE                   = 0x1069,
    CNS_HDR_HYBR_PREF                   = 0x106A,
    CNS_IDLE_DIGITAL_MODE               = 0x106B,
    CNS_DDTM_PREF                       = 0x106C,
    CNS_HDR_CLOSE_SESS                  = 0x106D,
    CNS_HDR_POWERSAVE_ENTER             = 0x106E,
    CNS_HDR_POWERSAVE_EXIT              = 0x106F,
    CNS_PH_SS_PREF_DUMMY, /* 0x1070,  CDMA & GSM Share CNS_PH_SS_PREF */
    CNS_DFLT_NV                         = 0x1071,
    CNS_TIMER_TEST,
    CNS_NOTIF_OBJECT_ID,

    CNS_OBSOLETE,

   /*-----------------------------------------------
    * The gap between 0x1063 and 0x1073 are all
    * filled up by the CNS_HDR (defined in SwiCnsEVDO.h)
    * and a few newer items added on the EM5625 firmware as of 04/27/05
    *
    * As of 05/19/05, change from 0x1074 to 0x1072 since the firmware 0.71
    * wrongly implements RTN_RESET on 0x1074 and 0x1072 is not used anywhere 
    * yet
    * ...
    *
    * As of 06/02/05, change from 0x1072 to 0x1075 since the firmware team
    * found that 0x1072 is somehow reserved (but not fully implemented)
    * by another CNS object
    */   
    CNS_RADIO_POWER_LEGACY                = 0x1075,
    CNS_RADIO_TEMPERATURE_LEGACY,
    CNS_RADIO_BATTERY_LEGACY,
    CNS_MEID_ESN_DUMMY, /* 0x1078, CDMA & GSM Share CNS_MEID_ESN */

    CNS_GEN_OBJECT_MAX,

    CNS_HDR_ACQ                         = 0x1200,
    CNS_HDR_UATI                        = 0x1202,
    /*-----------------------------------------------*/
    /* SMS Items  */
    CNS_SMS_OBJECT_START            = 0x2000,
    CNS_SMS_RECEIVED_MSG_STATUS_CDMA    = CNS_SMS_OBJECT_START,
    CNS_SMS_RETRIEVE_SMS  ,
    CNS_SMS_RETRIEVED ,
    CNS_SMS_STORE_SMS  ,
    CNS_SMS_SEND_SMS_CDMA,
    CNS_SMS_SEND_STATUS_CDMA,
    CNS_SMS_VMN_MWI_CLEAR,
    CNS_SMS_MEM_FULL,
    CNS_SMS_OBJECT_MAX,

    /* Embedded IOTA related Items  */
    CNS_IOTA_OBJECT_START            = 0X2100,
    CNS_IOTA_START_SESSION            = CNS_IOTA_OBJECT_START,
    CNS_IOTA_STOP_SESSION,
    CNS_IOTA_STATUS,
    CNS_IOTA_WAP_PUSH,
    CNS_IOTA_DISBALE,
    CNS_IOTA_OBJECT_MAX,

    /*-----------------------------------------------*/
    /* Call Related Items */
    CNS_CALL_OBJECT_START           = 0x3000,
    CNS_CALL_NOTIFICATION_STATUS    = CNS_CALL_OBJECT_START,
    CNS_CALL_BYTE_COUNTER ,
    CNS_CALL_BYTE_CNT_RESET ,
    CNS_CALL_DURATION ,
    CNS_CALL_INCOMING ,
    CNS_CALL_ANSWER_CMD ,
    CNS_CALL_CALLER_ID  ,
    CNS_CALL_ORIGINATE_CMD  ,
    CNS_CALL_ORIGINATE_SO_CMD ,
    CNS_CALL_EXTENDED_CALLER_ID ,
    CNS_CALL_CONNECTED  ,
    CNS_CALL_CONNECTION_STATE ,
    CNS_CALL_DISCONNECTED ,
    CNS_CALL_DISCONNECT_CMD ,
    CNS_CALL_ERROR,
    CNS_CALL_FLASH_CMD,
    CNS_CALL_FLASHED                = 0x3010,
    CNS_CALL_CONNECTING ,
    CNS_CALL_DORMANT,
    CNS_CALL_PRIVACY,
    CNS_CALL_OTASP_STATE,
    CNS_CALL_MB_COUNTER                = 0x3015,
    CNS_CALL_OBJECT_MAX,

    /*-----------------------------------------------*/
    /* Tech Mode Items */
    CNS_TECH_OBJECT_START           = 0x4000,
    CNS_TECH_RLP_STAT               = CNS_TECH_OBJECT_START,
    CNS_TECH_SID  ,
    CNS_TECH_NID  ,
    CNS_TECH_BS_INFO  ,
    CNS_TECH_TEMP ,
    CNS_TECH_FER  ,
    CNS_TECH_FING ,
    CNS_TECH_TX_PWR,
    CNS_TECH_BANDCLASS,
    CNS_TECH_REG_ZONE,
    CNS_TECH_PACKET_ZONE,
    CNS_TECH_PCS_BLOCK,
    CNS_TECH_CELL_CHANNEL,
    CNS_TECH_RLP33_STAT,
    CNS_TECH_ERROR_LOG,
    CNS_FLASH_BURN_CYCLE,
    CNS_TECH_ICMP_STAT                = 0x4010,
    CNS_TECH_UDP_STAT,
    CNS_TECH_TCP_STAT,
    CNS_TECH_IP_STAT,
    CNS_TECH_PPP_STAT,
    CNS_TECH_ERR_STAT,
    CNS_TECH_GET_ERRNUM,
    CNS_TECH_RX_PWR,
    CNS_TECH_HW_VERSION,
    CNS_TECH_ACTIVE_SET,
    CNS_TECH_DEFAULT_ROAM_IND,
    CNS_TECH_POWERSAVE_STATE,
    CNS_TECH_HDR_NOTIFICATION_STATUS    = 0x401C,
    CNS_TECH_HDR_PREV                   = 0x401D,
    CNS_TECH_HDR_SECTOR                 = 0x401E,
    CNS_TECH_HDR_STATE                  = 0x401F,
    CNS_TECH_HDR_CHANNEL                = 0x4020,
    CNS_TECH_HDR_SESSION                = 0x4021,
    CNS_TECH_HDR_AN_AUTH                = 0x4022,
    CNS_TECH_HDR_DRC_VAL                = 0x4023,
    CNS_TECH_HDR_DRC_COVER              = 0x4024,
    CNS_TECH_HDR_RRI                    = 0x4025,
    CNS_TECH_HDR_STATS                  = 0x4026,
    CNS_TECH_HDR_SU_FWD_STATS           = 0x4027,
    CNS_TECH_HDR_SU_FWD_SLOT_CNT        = 0x4028,
    CNS_TECH_HDR_SU_FWD_SLOT_CNT_2      = 0x4029,
    CNS_TECH_HDR_MU_FWD_STATS           = 0x402A,
    CNS_TECH_HDR_PERSONALITY            = 0x402B,
    CNS_TECH_HDR_PROT_APP_SUBTYPE       = 0x402C,
    CNS_TECH_HDR_SLEEP_PARMS            = 0x402D,
    CNS_TECH_HDR_MAC_INDEX              = 0x402E,
    CNS_TECH_OBJECT_MAX,

    /*--------------------------------------------------*/
    /* High temperature protection indication */
    CNS_TEMP_OBJECT_START           = 0x4500,
    CNS_MDM_TOOHOT_INDICATION       = CNS_TEMP_OBJECT_START,
    CNS_TEMPERATURE_THRESH,
    CNS_TEMP_OBJECT_MAX,

    /* CNS_UNLOCK */
    CNS_UNLOCK,
        
    /*-----------------------------------------------*/
    /* SB-specific Items */

    CNS_SB_OBJECT_START   = 0x5000,
    CNS_SB_SCRPAD         = CNS_SB_OBJECT_START,
    CNS_SB_IPR2,
    CNS_SB_GPIO,
    CNS_SB_SOMOD,
    CNS_SB_SHDMOD,
    CNS_SB_SHTDWN,
    CNS_SB_AUDMOD,
    CNS_SB_WWKUP,
    CNS_SB_WGETWK,
    CNS_SB_STGLVL,
    CNS_SB_TCXO_CTL_MODE,
    CNS_SB_GO_DORMANT,
    CNS_SB_OBJECT_MAX,

    /* Embedded IOTA related Items */
    CNS_IOTA_OBJECT_START_LEGACY        = 0X7000,
    CNS_IOTA_START_SESSION_LEGACY    = CNS_IOTA_OBJECT_START_LEGACY,
    CNS_IOTA_STOP_SESSION_LEGACY,
    CNS_IOTA_STATUS_LEGACY,
    CNS_IOTA_WAP_PUSH_LEGACY,
    CNS_IOTA_DISBALE_LEGACY,
    CNS_IOTA_OBJECT_MAX_LEGACY
};

enum SWI_TYPE_PstNvIndex
{
    /*--------------------------------------------
    Definition of the CDMA PST NV items
    ----------------------------------------------*/
    NV_SCM_I                = 4,
    NV_SCI                    = 5,
    NV_MOB_CAI_REV            = 6,
    NV_PREF_MODE            = 10,
    NV_PREF_SERV            = 11,
    NV_PCDMACH                = 20,
    NV_SCDMACH                = 21,
    NV_MIN1                    = 32,
    NV_MIN2                    = 33,
    NV_TERM_HOME            = 34,
    NV_TERM_SID                = 35,
    NV_TERM_NID                = 36,
    NV_ACCOLC                = 37,
    NV_SID_NID                = 38,
    NV_CURR_NAM                = 40,
    NV_AUTO_NAM                = 42,
    NV_NAME_NAM_I           = 43,
    NV_DIAL_I                = 57,
    NV_CALL_BEEP            = 66,
    NV_CONT_KEY_DTMF        = 67,
    NV_SVC_AREA_ALERT        = 69,
    NV_CALL_FADE_ALERT        = 70,
    NV_BANNER_I                = 71,
    NV_AUTO_REDIAL            = 75,
    NV_LOCK_I                = 81,
    NV_LOCK_CODE_I            = 82,
    NV_AUTO_LOCK_I            = 83,
    NV_SEC_CODE_I            = 85,
    NV_ALERTS_I                = 161,
    NV_ALERTS_SHADOW        = 162,
    NV_BEEP_LVL_SHADOW        = 164,
    NV_EAR_LVL_SHADOW        = 165,
    NV_IMSI_MCC                = 176,
    NV_IMSI_11_12            = 177,
    NV_VOICE_PRIV            = 179,
    NV_FSC                    = 184,
    NV_WDC                    = 197,
    NV_IMSI_ADDR_NUM        = 209,
    NV_DIR_NUMBER_PCS        = 215,
    NV_OTKSL                = 219,
    NV_VOICE_PRIV_ALERT        = 237,
    NV_DATA_QNC_ENABLE        = 240,
    NV_DATA_SO_SET            = 241,
    NV_PRL_ENABLED            = 256,
    NV_SYSTEM_PREF            = 258,
    NV_HOME_SID_NID            = 259,
    NV_IMSI_T_S1            = 262,
    NV_IMSI_T_S2            = 263,
    NV_IMSI_T_MCC            = 264,
    NV_IMSI_T_11_12            = 265,
    NV_SMS_UTC_I            = 269,
    NV_ROAM_RINGER            = 270,
    NV_PREF_VOICE            = 285,
    NV_SPC_CHG_ENABLED        = 296,
    NV_DATA_MDR_MODE        = 297,
    NV_PKT_ORIG_STR            = 298,
    NV_DATA_AUTO_PACKET_DET    = 300,
    NV_SMS_REMINDER            = 306,
    NV_PAP_USER_ID            = 318,
    NV_PAP_PASSWORD            = 319,        /* Used as QNC password */
    NV_GPS1_CAPABILITIES_I    = 400,        /* One 8-Bit Data Field to Enable/Disable the CLM Request Message Support */
    NV_PREF_FOR_RC            = 405,
    NV_SIO_DEV_MAP_MENU_ITEM_I= 408,
    NV_PRIMARY_DNS            = 423,
    NV_SECONDARY_DNS        = 424,
    NV_SCRM_ENABLED            = 429,
    NV_DS_QCMIP                = 459,
    NV_DS_MIP_RETRIES        = 460,
    NV_DS_MIP_RETRY_INT        = 461,
    NV_DS_MIP_PRE_RE_RRQ_TIME = 462,
    NV_DS_MIP_NUM_PROF        = 463,
    NV_DS_MIP_ACTIVE_PROF    = 464,
    NV_DS_MIP_GEN_USER_PROF    = 465,
    NV_DS_MIP_SS_USER_PROF    = 466,
    NV_HDRSCP_SESSION_STATUS_I = 475,
    NV_DS_MIP_MN_HA_TIME_DELTA = 494,
    NV_DS_MIP_QC_DRS_OPT    = 495,
    NV_DS_MIP_2002BIS_MN_HA_AUTH = 546,
    NV_SIP_USER_ID            = 565,
    NV_SIP_USER_ID_REV_1    = 512,    /* For backwards compatability. */
    NV_SIP_PASSWORD            = 566,
    NV_SIP_PASSWORD_REV_1    = 513,    /* For backwards compatability.*/
    NV_TEMP_THRESH          = 568,
    NV_SIP2_USER_ID            = 570,
    NV_SIP2_PASSWORD        = 571,
    NV_HDR_AN_AUTH_NAI        = 579,
    NV_HDR_AN_AUTH_PASSWORD    = 580,
    NV_DS_MIP_RRQ_IF_TFRK    = 707,
    NV_DS_MIP_ENABLE_PROF    = 714,
    NV_PPP_PASSWORD = 906,
    NV_PPP_USER_ID = 910,    /* Used as QNC username */
    NV_HDRSCP_SUBTYPE_CUSTOM_CONFIG_I = 3458,
    NV_GPS1_DYNAMIC_MODE_I    = 3756,    /* One 8-Bit Data Field with two bits assigned to two different smart modes and ORd Together */
    NV_GPS1_NMEA_OUTPUT_I    = 4100,    /* One 8-Bit Data Field with 5 bits defined and OR'd together to control the different NMEA message types being output */
    NV_HDRSCP_FORCE_REL0_CONFIG_I        = 4204,
    NV_HW_REV_ID_I                        = 16410,        /* 0x401A */
    NV_AUTO_POPULATE_INFO_AP_I            = 16524,        /* 0x408c */
    NV_AUTO_POPULATE_INFO_DATA_AP_I        = 16525,    /* 0x408d */
    NV_PRI_VER                          = 24612,    /* 0x6024 */
    NV_SWI_GPS_AUTO_TRACK_I                = 24679,    /* LBS Auto Tracking Configuration */
    NV_SWI_LED_ENABLE_I               = 24712,
    NV_SWI_GPS_MSB_FALLBACK_I         = 24716,    /* One 8-Bit Field to allow Enable/Disable of Sierra MS-Based Fallback */
    NV_SWI_GPS_MODE_SUPPORT_I         = 24717,    /* Six 8-Bit Fields that allow individual Enable/Disable of six different Fix types supported */
    NV_SWI_GENERAL_FEATURE_SUPPORT_I  = 24718, /* 32 bit mask of supported features. */
    NV_SWI_LED_CONFIG_TABLE_I         = 24729,
    NV_SWI_LED_ENABLE_TABLE_I         = 24732
};

/* SWI NV Index Table - mapped from 0 - */
/* #ifndef SWI_EVDO */
enum SWI_TYPE_SwiPstNvIndex
{
    SWI_NV_PCS_PDM1,
    SWI_NV_CNS_SECURE_FAILURES,
    SWI_NV_FSN,
    SWI_NV_CAL_DATE,
    SWI_NV_TEST_HISTORY,
    SWI_NV_AKEY_CHECKSUM,
    SWI_NV_KST_SCRPAD_MSG,
    SWI_NV_KST_GPIO_DIR,
    SWI_NV_KST_SO_MODE,
    SWI_NV_KST_SHTDWN_MODE,
    SWI_NV_KST_MIKE_CONTROL,
    SWI_NV_KST_SPKR_CONTROL,
    SWI_NV_DTMF_CONTROL,
    SWI_NV_KST_WKUP_MODE,
    SWI_NV_KST_ECHO_LEVEL,
    SWI_NV_KST_SPRK_LEVEL,
    SWI_NV_STACKS,
    SWI_NV_PROVISION_INFO,
    SWI_NV_SIP_USER_ID,
    SWI_NV_SIP_PASSWORD,
    SWI_NV_KST_STG_LEVEL,
    SWI_NV_KST_ADC_THROTL,
    SWI_NV_TEMP_THRESH,
    SWI_NV_SIP2_USER_ID,
    SWI_NV_SIP2_PASSWORD,
    SWI_NV_OTKSL_COUNTER,
    SWI_NV_SBSDY_LCK_COUNTER,
    SWI_NV_SMS_TRAFFIC_DLVRY,
    SWI_NV_PRI_CREATE_QNC,
    SWI_NV_UPGR_PROTECT_OPTION,
    SWI_NV_PRI_QNC_ORIG_STR            = 31,
    SWI_NV_CSV,
    SWI_NV_DS_MIP_GEN_USER_PROF_BAK,
    SWI_NV_DS_MIP_SS_USER_PROF_BAK,
    SWI_NV_CARRIER_ID,
    SWI_NV_PRI_DATA_WARN_MASK        = 37,
    SWI_NV_PRI_SID_ACTIV_PROMPT        = 39,
    SWI_NV_PRI_QNC_ACTIV_PROMPT,
    SWI_NV_QNC_CONNECTOID_NAME_I,
    SWI_NV_1X_CONNECTOID_NAME_I,
    SWI_NV_PRI_WA_VOICEDATA            = 46,
    SWI_NV_PRI_WA_SMS,
    SWI_NV_PRI_AW_SETTINGS,
    SWI_NV_PRI_MDN_LEN,
    SWI_NV_PRI_SIP_USER_ID_FMT,
    SWI_NV_PRI_OTASP_DIAL,
    SWI_NV_PRI_VOICEMAIL,
    SWI_NV_PRI_GUI_SETTINGS,
    SWI_NV_PRI_MAX_MOSMS_LEN,
    SWI_NV_PRI_ENABLE_DMU            = 65,
    SWI_NV_WATCHER_TITLE = 67
};

#define CNS_RADIO CNS_RSSI /* this is for initial testing */

#include "cn/cnuproto.h"

#endif

/*
 * $Log: cnudefs.h,v $
 */
