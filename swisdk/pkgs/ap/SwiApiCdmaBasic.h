/**
 *    \if CvsId 
 *    $Id: SwiApiCdmaBasic.h,v 1.7 2011/02/11 22:09:02 ygao Exp $
 *    \endif
 *    \ingroup cdmabasic
 *    \file SwiApiCdmaBasic.h
 *    
 *    \brief SWI CDMA specific basic APIs.
 *
 *    Copyright (c) 2011 Sierra Wireless, Inc.  All rights reserved
 *
 */

#ifndef __SWIAPICDMABASIC_H__
#define __SWIAPICDMABASIC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* include files */
#include "SwiDataTypes.h"
#include "SwiRcodes.h"

#define G_SZ_FNERINAM1 "eri_nam1"

/** modem can only handle 120 size blocks. */
#define CNS_MAX_DATA_BLOCK_SIZE     120

#define LENGTH_NvNameNam            12  /*!< Max NAM length */
#define LENGTH_PhoneNumberMin       10  /*!< Min phone number length */
#define LENGTH_NVItemPayloadMax     240 /*!< Max phone number length */
#define LENGTH_Mdn                  15  /*!< Min Caller ID length */
#define LENGTH_Min                  10  /*!< Max MDN length */
#define LENGTH_PinPassword          4   /*!< Max MIN length */
#define LENGTH_SecureChallenge      8   /*!< Max Secure challenge length */
#define LENGTH_SecureRebuttal       4   /*!< Max secure rebuttal length */
#define LENGTH_ProvisioningDate     8   /*!< Max provision date length */

#define LENGTH_CallNumber           32  /*!< Max Caller ID length */
#define LENGTH_CallExtendedNumber   64  /*!< Max Extended Caller ID length */

#define SCP_PROT_SUBTYPE_MASK   0x000000F9  /*!< scp prot subtype mask */
#define SCP_BCMCS_SUBTYPE_MASK  0           /*!< scp bcmcs subtype mask */
#define SCP_APP_SUBTYPE_MASK    1           /*!< scp app subtype mask */

/**
 *
 * An enumeration of the possible bits in the failure mask field returned by 
 * SwiPstLock.
 *
 */
typedef enum SWI_TYPE_Commit_ErrorMask
{
    SWI_COMMIT_ERRMASK_NONE     = 0x00000000,   /*!< No error */
    SWI_COMMIT_ERRMASK_MDN      = 0x00000001,   /*!< MDN not set */
    SWI_COMMIT_ERRMASK_IMSI     = 0x00000002,   /*!< IMSI not set */
    SWI_COMMIT_ERRMASK_USERIDPW = 0x00000004,   /*!< No User ID/Password */
    SWI_COMMIT_ERRMASK_DATE     = 0x00000008,   /*!< No Provisioning Date */

    /* NAM profile index out of sequence. (NAM 0 must always be provisioned 
       first) */
    SWI_COMMIT_ERRMASK_NAMORDER = 0x00000010    /*!< Max */
}SWI_TYPE_Commit_ErrorMask;

/**
 *
 * Contains HDR MAC index information 
 *
 */
typedef struct SWI_STRUCT_Tech_Hdr_Mac_Index
{
    swi_uint32 sizeStruct;          /*!< Size of this structure */

    swi_uint8 mac_index_available;  /*!< Flag for MAC index availability */
    swi_uint8 mac_index;            /*!< MAC index */
}SWI_STRUCT_Tech_Hdr_Mac_Index;

/**
 *
 * An enumeration of Emergency 911 mode 
 *
 * When in E911 mode, some modem functionality may not be available. The 
 * modem can forced out of E911 mode using SwiSetE911Mode.
 *
 */
typedef enum SWI_TYPE_E911Mode
{
    SWI_E911_MODE_Off,      /*!< E911 mode off */
    SWI_E911_MODE_On        /*!< E911 mode on */
}SWI_TYPE_E911Mode;

/**
 *
 * Contains Emergency 911 notification data 
 *
 * \sa SWI_NOTIFY_E911Mode
 */
typedef struct SWI_STRUCT_E911Mode
{
    swi_uint32 sizeStruct; /*!< Size of this structure */

    /*! Emergency 911 mode.  See SWI_TYPE_E911Mode */
    SWI_TYPE_E911Mode eMode;
}SWI_STRUCT_E911Mode;

/**
 *
 * Contains PowerSave (Enter) notification data.
 *
 * This data structure is used as a member of the notification 
 * SWI_NOTIFY_PowerSaveEnter.  Refer to the description of
 * SWI_NOTIFY_PowerSaveEnter for more detail.
 *
 */
typedef struct SWI_STRUCT_PowerSaveEnter
{
    swi_uint32 sizeStruct;  /*!< Size of this structure */
    /* no data */
}SWI_STRUCT_PowerSaveEnter;

/**
 *
 * Contains PowerSave (Exit) notification data.
 *
 * This data structure is used as a member of the notification 
 * SWI_NOTIFY_PowerSaveExit.  Refer to the description of
 * SWI_NOTIFY_PowerSaveExit for more detail.
 *
 */
typedef struct SWI_STRUCT_PowerSaveExit
{
    swi_uint32 sizeStruct;  /*!< Size of this structure */
    /* no data */
}SWI_STRUCT_PowerSaveExit;

/**
 *
 * An enumeration of Voice/Data Support mask bit. 
 *
 */
typedef enum SWI_TYPE_PRI_VoiceDataMask
{
    SWI_PRI_DATA_1X             = 0x0001,   /*!< 1x data calls supported */

    /*! IS95(QNC/Fax/Dialup) data calls supported */
    SWI_PRI_DATA_IS95           = 0x0002,

    /*! nternal use for Watcher only */
    SWI_PRI_DATA_AUTOCONNECT    = 0x0004,

#if FUTURE_USE
    SWI_PRI_DATA_               = 0x0008,   /*!< For future use */
    SWI_PRI_DATA_               = 0x0010,   /*!< For future use */
    SWI_PRI_DATA_               = 0x0020,   /*!< For future use */
    SWI_PRI_DATA_               = 0x0040,   /*!< For future use */
#endif
    SWI_PRI_VOICE               = 0x0080,   /*!< Voice calls supported */
    SWI_PRI_VD_ALL              = 0xFFFF,   /*!< Internal Use */

    /*! Enabled bit (Internal use only) */
    SWI_PRI_VD_MASK_ENABLED     = 0x8000
}SWI_TYPE_PRI_VoiceDataMask;

/**
 *
 * An enumeration of supported carrier ID. 
 *
 */
typedef enum SWI_TYPE_CarrierID
{
    SWI_CARRIER_UNDEFINED               = -1,   /*!< Not defined */
    SWI_CARRIER_RESERVED                = 0,    /*!< Reserved */
    SWI_CARRIER_AC555_GENERIC,                  /*!< Generic */
    SWI_CARRIER_AC550_SPRINT,                   /*!< Sprint */
    SWI_CARRIER_AC555_BELL,                     /*!< Bell Mobility */
    SWI_CARRIER_AC555_TELUS,                    /*!< Telus */
    SWI_CARRIER_AC555_VZW,                      /*!< Verizon Wireless */
    SWI_CARRIER_AC555_WESTERNWIRELESS,          /*!< Western Wireless */
    SWI_CARRIER_AC555_SMARTCOM,                 /*!< SmartCom */
    SWI_CARRIER_AC555_ALLTEL,                   /*!< AllTel */
    SWI_CARRIER_AC555_USCELLULAR,               /*!< US Cellular */
    SWI_CARRIER_SB550T_SPRINT,                  /*!< (Thera) Sprint */
    SWI_CARRIER_SB555T_BELL,                    /*!< (Thera) Bell Mobility */
    SWI_CARRIER_SB555T_TELUS,                   /*!< (Thera) Telus */
    SWI_CARRIER_SB555T_VZW,                 /*!< (Thera) Verizon Wireless */
    SWI_CARRIER_AC555_CHINAUNICOM,              /*!< China Unicom */
    SWI_CARRIER_AC555_HUTCHISONTHAILAND,        /*!< Hutchison Thailand */
    SWI_CARRIER_AC555_MOVILNET,                 /*!< MovilNet */
    SWI_CARRIER_AC555_TARJETAS_IUSACELL_MEXICO, /*!< Iusacell Mexico */
    SWI_CARRIER_AC555_TELECOMNEWZEALAND,        /*!< Telecom NewZealand */
    SWI_CARRIER_AC555_RELIANCE,                 /*!< Reliance */
    SWI_CARRIER_AC555_TELSTRA,                  /*!< Telstra */
    SWI_CARRIER_AC555_MOBILITYCANADA,           /*!< Mobility Canada */
    SWI_CARRIER_AC555_VZW_PUERTO_RICO,  /*!< Verizon Wireless Puerto Rico */
    SWI_CARRIER_AC555_PELEPHONE,                /*!< Pelephone */
    SWI_CARRIER_AC555_BELL_CANADA,              /*!< Bell Canada */
    SWI_CARRIER_AC555_INDOSOL_INDONESIA,        /*!< Indosol */
    SWI_CARRIER_AC555_MIDWEST_WIRELESS,         /*!< MidWest Wireless */
    SWI_CARRIER_AC555_BELL_SOUTH_CHILE,         /*!< South Chile */
    SWI_CARRIER_AC555_BELL_SOUTH_PANAMA,        /*!< South Panama */
    SWI_CARRIER_AC555_TATA                      /*!< Tata */
}SWI_TYPE_CarrierID;

/**
 *
 * An enumeration of supported distributor ID. 
 *
 */
typedef enum SWI_TYPE_DistributorID
{
    SWI_DISTRIBUTOR_UNDEFINED           = -1,   /*!< Not defined */
    SWI_DISTRIBUTOR_NONE                = 0,    /*!< Reserved */
    SWI_DISTRIBUTOR_SIERRA_WIRELESS     = 1,    /*!< SWIR */
    SWI_DISTRIBUTOR_EASTCOM             = 2,    /*!< Easter Communications */
    SWI_DISTRIBUTOR_TONGFANG            = 3,    /*!< TSINGHUA TONGFANG */
    SWI_DISTRIBUTOR_ZTE                 = 4,    /*!< ZTE */
    SWI_DISTRIBUTOR_CHINA_PUTIAN        = 5,    /*!< NANJING PUTIAN ZHONGYOU */
    SWI_DISTRIBUTOR_NCL                 = 6,    /*!< NCL Technology */
    SWI_DISTRIBUTOR_SHANGHAI_BELL       = 7     /*!< SHANGHAI BELL */
}SWI_TYPE_DistributorID;

/**
 *
 * Contains HDR notification data.
 *
 */
typedef struct SWI_STRUCT_HdrNotificationStatus
{
  swi_uint32 sizeStruct;                /*!< Size of this structure */
  swi_uint16 iHdrNotificationStatus;    /*!< Mask of enabled notifications */
}SWI_STRUCT_HdrNotificationStatus;

/**
 *
 * An enumeration of digital mode. 
 *
 */
typedef enum SWI_TYPE_Digital_Mode
{
    SWI_HDR_MODE_NO_SERVICE     = 0x00000000,   /*!< No service */
    SWI_HDR_MODE_AMPS           = 0x00000001,   /*!< Amps */
    SWI_HDR_MODE_CDMA           = 0x00000002,   /*!< CDMA */
    SWI_HDR_MODE_GSM            = 0x00000003,   /*!< GSM */
    SWI_HDR_MODE_HDR            = 0x00000004,   /*!< HDR (EVDO) */
    SWI_HDR_MODE_WCDMA          = 0x00000005,   /*!< WCDMA */
    SWI_HDR_MODE_GPS            = 0x00000006    /*!< GPS */
}SWI_TYPE_Digital_Mode;

/**
 *
 * Contains previous and current digital mode.
 *
 * \sa SWI_TYPE_Digital_Mode
 */
typedef struct SWI_STRUCT_Idle_Digital_Mode
{
    swi_uint32 sizeStruct;              /*!< Size of this structure */

    /*! Previous digital mode. See SWI_TYPE_Digital_Mode */
    swi_uint16 iPrevIdleDigitalMode;

    /*! Current digital mode. See SWI_TYPE_Digital_Mode */
    swi_uint16 iCurrIdleDigitalMode;
}SWI_STRUCT_Idle_Digital_Mode;

/**
 *
 * Contains HDR PowerSave (Enter) notification data.
 *
 * This data structure is used as a member of the notification 
 * SWI_NOTIFY_Hdr_PowerSave_Enter.  Refer to the description of
 * SWI_NOTIFY_Hdr_PowerSave_Enter for more detail.
 *
 */
typedef struct SWI_STRUCT_Hdr_PowerSave_Enter
{
    swi_uint32 sizeStruct;  /*!< Size of this structure */
    /* no data */
}SWI_STRUCT_Hdr_PowerSave_Enter;

/**
 *
 * Contains HDR PowerSave (Exit) notification data.
 *
 * This data structure is used as a member of the notification 
 * SWI_NOTIFY_Hdr_PowerSave_Exit.  Refer to the description of
 * SWI_NOTIFY_Hdr_PowerSave_Exit for more detail.
 *
 */
typedef struct SWI_STRUCT_Hdr_PowerSave_Exit
{
    swi_uint32 sizeStruct;  /*!< Size of this structure */
    /* no data */
}SWI_STRUCT_Hdr_PowerSave_Exit;

/**
 *
 * An enumeration of CDMA  service settings. 
 *
 */
typedef enum SWI_TYPE_Device_CDMA_Settings
{
    SWI_REV_UNKNOWN = 0x00ff,   /*!< Unknown */
    SWI_REV_0       = 0x0000,   /*!< CDMA Rev 0 */
    SWI_REV_A       = 0x0001    /*!< CDMA Rev A */
}SWI_TYPE_Device_CDMA_Settings;

/**
 *
 * Contains MEID and ESN data.
 *
 */
typedef struct SWI_STRUCT_MeidEsn
{
    swi_uint32  sizeStruct;     /*!< Size of this structure */

    /*! TRUE - MEID/pESN is used in the modem; 
        FALSE - tESN is used in the modem */
    swi_uint16  nMeidEnabled;
            
    /*! TRUE - MEID provisioned in the modem; 
        FALSE - MEID has never been written */
    swi_uint16  nMeidPresent;
               
    /*! (high word of qword) 56 bits of MEID if MEID is provisioned; 
        otherwise, all 0's */
    swi_uint32  nMeidHigh;
               
    /*! (low word of qword) 56 bits of MEID if MEID is provisioned; 
        otherwise, all 0's */
    swi_uint32  nMeidLow;
               
    /*! 32 bits of pESN if MEID  is provisioned; otherwise, all 0's */
    swi_uint32  nPEsn;
            
    /*! TRUE - tESN provisioned in the modem; 
        FALSE - tESN has never been written */
    swi_uint16  nTEsnPresent;
            
    /*! 32 bits of tESN if tESN provisioned; otherwise, all 0's */
    swi_uint32  nEsn;
            
}SWI_STRUCT_MeidEsn;

/* Below are from SwiApiExt.h */
/*
 *        TYPE DEFINITIONS
 */

/**
 *
 * An enumeration of CDMA features. 
 *
 */
typedef enum SWI_TYPE_General_Features
{
    SWI_GEN_FEATURE_IOTA_2_4    = 0x00000001,   /*!< IOTA */
    SWI_GEN_FEATURE_ENH_GPS     = 0x00000002,   /*!< Enhanced GPS  */
    SWI_GEN_FEATURE_ENH_LED     = 0x00000004,   /*!< Enhanced LED */
    SWI_GEN_FEATURE_RESERVED3   = 0x00000008,   /*!< Reserved  */
    SWI_GEN_FEATURE_RESERVED4   = 0x00000010,   /*!< Reserved */
    SWI_GEN_FEATURE_RESERVED5   = 0x00000020,   /*!< Reserved */
    SWI_GEN_FEATURE_RESERVED6   = 0x00000040,   /*!< Reserved */
    SWI_GEN_FEATURE_RESERVED7   = 0x00000080,   /*!< Reserved */
    SWI_GEN_FEATURE_RESERVED8   = 0x00000100,   /*!< Reserved */
    SWI_GEN_FEATURE_RESERVED9   = 0x00000200    /*!< Reserved */
    /*remaining bits reserved for future feature definitions... */
}SWI_TYPE_General_Features;

/**
 *
 * An enumeration of LED type. 
 *
 */
typedef enum SWI_TYPE_LED_TYPE
{
    SWI_LED_TYPE_UNDEFINED          = 0x00, /*!< Undefined */
    SWI_LED_TYPE_POWER_AMBER        = 0x01, /*!< Power amber */
    SWI_LED_TYPE_SIGNAL_AMBER       = 0x02, /*!< Signal amber */
    SWI_LED_TYPE_POWER_BLUE         = 0x03, /*!< Power blue */
    SWI_LED_TYPE_SIGNAL_BLUE        = 0x04, /*!< Signal blue */
    SWI_LED_TYPE_GLOWTOP            = 0x05, /*!< Glowtop */
    SWI_LED_TYPE_RESVD1             = 0x06, /*!< Reerved */
    SWI_LED_TYPE_RESVD2             = 0x07, /*!< Reerved */
    SWI_LED_TYPE_RESVD3             = 0x08, /*!< Reerved */
    SWI_LED_TYPE_RESVD4             = 0x09  /*!< Reerved */
}SWI_TYPE_LED_TYPE;

/**
 *
 * An enumeration of LED enable state. 
 *
 */
typedef enum SWI_TYPE_LED_ENABLE_STATE
{
    SWI_LED_DISABLED    = 0,    /*!< Disabled */
    SWI_LED_ENABLED     = 1     /*!< Enabled */
}SWI_TYPE_LED_ENABLE_STATE;

#define SWI_MAX_LEDS              10    /*!< Max number of LEDs */

/**
 *
 * Contains LED configuration data.
 *
 */
typedef struct SWI_STRUCT_LED_CFG_ENABLE
{
    swi_uint8 nType[SWI_MAX_LEDS];     /*!< type, See SWI_TYPE_LED_TYPE */
    swi_uint8 bEnable[SWI_MAX_LEDS];   /*!< See  SWI_TYPE_LED_ENABLE_STATE */
}SWI_STRUCT_LED_CFG_ENABLE;

/**
 *
 * Contains EFile information data.
 *
 */
typedef struct SWI_STRUCT_EFileInfo
{
    swi_uint32 sizeStruct;      /*!< Size of this structure */
    swi_uint16 nResult;         /*!< Result for get command */
    swi_uint32 nSize;           /*!< Size of EFile */
    swi_uint16 nBlocks;         /*!< Num of blocks */
}SWI_STRUCT_EFileInfo;

/**
 *
 * Contains HDR SCP data.
 *
 * This is data structure for CNS_HDRSCP_SUBTYPE_CUSTOM_CONFIG_I item. 
 * To be used with CNS_PST_NV object ID.
 *
 */
typedef struct CNS_HDRSCP_subtype_custom_config_type
{
    /*! custom config active status. 1: active; 0: not active. */
    swi_uint16 custom_config_is_active;
    
    swi_uint32 prot_subtype_mask;   /*!< scp prot subtype mask */
    swi_uint32 bcmcs_subtype_mask;  /*!< scp bcmcs subtype mask */
    swi_uint32 app_subtype_mask;    /*!< scp app subtype mask */
} CNS_HDRSCP_subtype_custom_config_type;

/* Function prototypes */

SWI_API SWI_RCODE SwiSetEFile(const swi_charp pszEFilename, 
                              swi_uint8* pBuffer, 
                              swi_uint16 nBufLen, 
                              swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetEFileInfo(const swi_charp pszEFilename, 
                                  SWI_STRUCT_EFileInfo* psEFileInfo, 
                                  swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetEFileBlock(const swi_charp pszEFilename, 
                                   swi_uint16 nSeq, 
                                   swi_uint8* pBuffer, 
                                   swi_uint16 nBufLen, 
                                   swi_uint32 *pnBlockSize, 
                                   swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetPRLSize(swi_uint16 Nam, 
                                swi_uint32 nSize, 
                                swi_uint32 timeout);

SWI_API SWI_RCODE SwiIsModemReady(swi_uint32 timeout);
SWI_API SWI_RCODE SwiGetESN(swi_int32* lEsn, swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetMEIDESN(SWI_STRUCT_MeidEsn *pMeidEsn, 
                                swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetPRLVersion(swi_uint16* pPrlVersion, 
                                   swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetTemperature(swi_int32  *iTemperature, 
                                    swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetActivationStatus(swi_uint16* pActivationStatus, 
                                         swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetPhoneNumber(swi_charp PhoneNumber, 
                                    swi_uint32 BufferSize, 
                                    swi_uint32 timeout);

SWI_API SWI_RCODE SwiPriGetVoiceDataMask(swi_uint16* voiceDataMask, 
                                         swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetCarrierID(swi_int16* pnCarrierID, swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetDistributorID(swi_int16* pnDistributorID, 
                                      swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetSystemTime(swi_uint16 *year, 
                                   swi_uint16 *month, 
                                   swi_uint16 *day, 
                                   swi_uint16 *weekDay, 
                                   swi_uint16 *hour, 
                                   swi_uint16 *minute, 
                                   swi_uint16 *second, 
                                   swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetE911Mode(swi_uint16* e911Mode, swi_uint32 timeout);
SWI_API SWI_RCODE SwiSetE911Mode(swi_uint16 e911Mode, swi_uint32 timeout);
SWI_API SWI_RCODE SwiGetECIO(swi_uint16* pEcio, swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetNamInfo(swi_uint16 *cntNamProfile, 
                                swi_uint16 *nextNamProfile, 
                                swi_uint32  timeout);

SWI_API SWI_RCODE SwiPstGetCurrentNam(swi_uint16 *CurrentNam, 
                                      swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetNamValidStatus(swi_uint16* pNamValidStatus, 
                                       swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetCurrentNam(swi_uint16 CurrentNam, 
                                      swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetProvisioningDate(swi_uint16 namIndex, 
                                         swi_charp pszProvisioningData, 
                                         swi_uint32 length, 
                                         swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetCdmaIMSI(swi_uint16 namIndex, 
                                 swi_charp pszMIN, 
                                 swi_uint32 length, 
                                 swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstUnlock(swi_uint32 SubsidyLock, swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetIMSI(swi_uint16 namIndex, 
                             const swi_charp pszMIN, 
                             swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstLock(swi_uint16 Type, 
                             swi_uint16 Nam, 
                             swi_uint16* commitResult, 
                             swi_uint16* failureMask, 
                             swi_uint32 timeout);

SWI_API SWI_RCODE SwiResetModem();
SWI_API SWI_RCODE SwiSetModemOffline();

SWI_API SWI_RCODE SwiGetLockStatus(swi_uint16* pLockStatus, 
                                   swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetAutoLock(swi_uint16* autoLock, swi_uint32 timeout);
SWI_API SWI_RCODE SwiSetAutoLock(swi_uint16 autoLock, swi_uint32 timeout);

SWI_API SWI_RCODE SwiLockModem(const swi_charp Password, 
                               swi_uint16* pResponse, 
                               swi_uint32 timeout);

SWI_API SWI_RCODE SwiChangeLockPIN(const swi_charp OldPassword, 
                                   const swi_charp NewPassword, 
                                   swi_uint16* pResponse, 
                                   swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetERIVer(const swi_charp pszERIFilename, 
                               swi_uint16* pnERIVer, 
                               swi_uint32 timeout);

SWI_API SWI_RCODE SwiPowerSaveExit();
SWI_API SWI_RCODE SwiSetPower(swi_uint16 fwPowerState);

SWI_API SWI_RCODE SwiNotifyEx(swi_uint16 eNotify, 
                              swi_uint16 mask, 
                              swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetAccolc(swi_uint16 Nam, 
                                  swi_uint16 *Accolc, 
                                  swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetAccolc(swi_uint16 Nam, 
                                  swi_uint16 Accolc, 
                                  swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetSci(swi_uint16 *Sci, swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetPRL(swi_uint16 NamIndex, 
                            swi_uint8* pPRLBuffer, 
                            swi_uint16 nPRLBufLen, 
                            swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetPRLEx(swi_uint16 NamIndex, 
                              swi_uint8* pPRLBuffer, 
                              swi_uint16 nPRLBufLen, 
                              swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetERI(swi_uint8* pERIBuffer, 
                            swi_uint16 nERIBufLen, 
                            swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetTotalUpTime(swi_uint32* pTotalUpTime, 
                                    swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetImsiMS1(swi_uint16 Nam, 
                                   swi_uint32 *IMSI, 
                                   swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetImsiMS1(swi_uint16 Nam, 
                                   swi_uint32 Imsi_MS1, 
                                   swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetImsiMS2(swi_uint16 Nam, 
                                   swi_uint16 *Imsi_MS2, 
                                   swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetImsiMS2(swi_uint16 Nam, 
                                   swi_uint16 Imsi_MS2, 
                                   swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetNamName(swi_uint16 Nam, 
                                   swi_charp pszNamName, 
                                   swi_uint32 length, 
                                   swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetNamName(swi_uint16 Nam, 
                                   const swi_charp pszNamName, 
                                   swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetImsiMcc(swi_uint16 Nam, 
                                   swi_uint16 *Mcc, 
                                   swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetImsiMcc(swi_uint16 Nam, 
                                   swi_uint16 Mcc, 
                                   swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstGetImsi1112(swi_uint16 Nam, 
                                    swi_uint16 *Digits1112, 
                                    swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetImsi1112(swi_uint16 Nam, 
                                    swi_uint16 Digits1112, 
                                    swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetAKey(swi_charp szAKey, swi_uint32 timeout);
SWI_API SWI_RCODE SwiRtnReset(swi_uint8 nType, swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetTechHdrMacIndex(
        SWI_STRUCT_Tech_Hdr_Mac_Index *pTechHdrMacIndex, 
        swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetLEDEnable(swi_bool *pbEnable, swi_uint32 timeout);
SWI_API SWI_RCODE SwiSetLEDEnable(swi_bool bEnable, swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetLEDEnableEx(SWI_STRUCT_LED_CFG_ENABLE *pLED, 
                                    swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetLEDEnableEx(const SWI_STRUCT_LED_CFG_ENABLE *pLED, 
                                    swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetGeneralFeatureSupport(swi_uint32 *pMask, 
                                              swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetDeviceEVDOSetting(swi_uint16 *pwMask, 
                                          swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetDeviceEVDOSetting(swi_uint16 wMask, 
                                          swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetStationClassMark(swi_uint8 *pbSCM, 
                                         swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetPRIVersion(swi_uint8 *pMajor, swi_uint8 *pMinor, 
                                   swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetPortMapping(swi_uint8 * pnPortMapping, 
                                      swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetPortMapping(swi_uint8 nPortMapping, 
                                      swi_uint32 timeout);


#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif  /* __SWIAPICDMABASIC_H__ */

/* 
 * $Log: SwiApiCdmaBasic.h,v $
 */

