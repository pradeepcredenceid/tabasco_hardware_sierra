/**
 *    \if CvsId 
 *    $Id: SwiApiGsmBasic.h,v 1.17 2010/06/10 00:17:05 epasheva Exp $
 *    \endif
 *    \ingroup gsmbasic
 *    \file SwiApiGsmBasic.h
 * 
 *    \brief This file contains definitions, enumerations, structures and
 *           forward declarations for SwiApiGsmBasic.c
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#ifndef __SWIAPI_GSM_BASIC
#define __SWIAPI_GSM_BASIC

#ifdef __cplusplus
extern "C" {
#endif

/* include files */
#include "SwiDataTypes.h"
#include "SwiRcodes.h"

/* defines */

#define MAX_NO_CONTEXT              3
#define MAX_3G_GSM_PROFILES         16
#define NUMBER_PLMN                 14
#define NUMBER_Band                 5
#define LENGTH_IPAddress            16
#define LENGTH_AccessPointName      101
#define LENGTH_UserName             128
#define LENGTH_Label                31
#define LENGTH_Password             128
#define LENGTH_AddressNumber        20
#define LENGTH_SME                  20
#define LENGTH_SMSTimestamp         14
#define LENGTH_SMSValidityPeriod    16
#define LENGTH_MaxCHV               9
#define LENGTH_MaxPhone             20
#define LENGTH_CallerName           82
#define MAX_CALLS                   7
#define LENGTH_MaxDebugString       201
#define LENGTH_DcsMsgInd            3
#define LENGTH_Timestamp            14
#define LENGTH_Validity             16
#define LENGTH_MAX_Sms              1024
#define LENGTH_MAX_SmsMsgData       161
#define LENGTH_MsgWaiting           3
#define LENGTH_EntryName            (31 * UTF8_MAX_CHAR_SIZE) + 1
#define LENGTH_GSM_PhoneNumber      40
#define LENGTH_EmergencyPhoneNumber 6
#define LENGTH_ForwardNumber        38
#define LENGTH_ForwardInfo          18
#define LENGTH_SSPassword           4
#define VERSION_SuppService         0x0001
#define MAX_FWD_INFO_ENTRIES        8
#define MAX_CALL_BARR_ENTRIES       8
#define MAX_INFO_DATA               70
#define MAX_BS_LIST                 70
#define LENGTH_RawData              229
#define LENGTH_SSString             40
#define LENGTH_STKCommand           119
#define LENGTH_STKCommand_utf8      (119 * UTF8_MAX_CHAR_SIZE) + 1
#define LENGTH_STKRespInput         120
#define LENGTH_STKRespInput_utf8    (120 * UTF8_MAX_CHAR_SIZE) + 1
#define LENGTH_SSString             40
#define LENGTH_SetFeaturesKey       21
#define LENGTH_RandNum              16
#define LENGTH_Kc                   8
#define LENGTH_USSDInput            100
#define LENGTH_CommandData          157

#define LENGTH_ErrorString          33
#define LENGTH_IccId                10
#define LENGTH_Imei                 16 
#define LENGTH_Tac                  7
#define LENGTH_Fac                  3
#define LENGTH_Snr                  7
#define LENGTH_ChkByte              2
#define LENGTH_Fsn                  16
#define LENGTH_Signature            9


/* enum declarations */
/**
 *  This enumeration contains the frequency bands
 *
 * \note   The AC710 is a single band card.  It only supports PCS.
 *
 * \note   The AC750 is a tri-band card.  It supports EGSM, EGSM_DCS, PCS and Auto.
 *
 */
enum SWI_TYPE_Band
{
    SWI_BAND_None,                  /*!< no frequency band. */
    SWI_BAND_PGSM,                  /*!< 900MHz (not supported). */
    SWI_BAND_EGSM,                  /*!< extended 900MHz. */
    SWI_BAND_DCS,                   /*!< 1800MHz (not supported). */
    SWI_BAND_PGSM_DCS,              /*!< 900MHz and 1800MHz (not supported). */
    SWI_BAND_EGSM_DCS,              /*!< extended 900MHz and 1800MHz. */
    SWI_BAND_PCS,                   /*!< 1900MHz. */
    SWI_BAND_PGSM_PCS,              /*!< 900 and 1900 MHz. */
    SWI_BAND_EGSM_PCS,              /*!< 900e and 1900 MHz. */
    SWI_BAND_GSM850,                /*!< 850MHz (not supported). */
    SWI_BAND_GSM850_PCS,            /*!< 850 and 1900 MHz (not supported). */
    SWI_BAND_GSM_ALL,               /*!< All GSM bands. */
    SWI_BAND_WCDMA_EU,              /*!< EU band: WCDMA 2100. */
    SWI_BAND_WCDMA_NA,              /*!< NA band:  WCDMA 850/1900. */
    SWI_BAND_WCDMA_GSM_NA,          /*!< NA bands: WDMA 850/1900 & GSM 850/1900. */
    SWI_BAND_WCDMA_GSM_EU,          /*!< EU bands:  WCDMA 2100 & GSM 900/1800. */
    SWI_BAND_WCDMA_ALL = 0x0010,    /*!< All WCDMA bands. */
    SWI_BAND_WCDMA_AU,              /*!< Australia bands: WCDMA 2100/850. */
    SWI_BAND_WCDMA_JAP,             /*!< Japan bands: WCDMA 2100/800. */
    SWI_BAND_WCDMA_GSM_AU,          /*!< Australia bands: WCDMA 2100/850 GSM 900/1800. */
    SWI_BAND_WCDMA_GSM_AU_2,        /*!< Australia bands: WCDMA 850 GSM 900/1800. */
    SWI_BAND_WCDMA_GSM_AU_3,        /*!< Australia bands: WCDMA 850. */
    SWI_BAND_AUTO = 0xffff         /*!< auto band selection. */
};


/**
 * This enumeration contains the PDP connection types. 
 *
 */
enum SWI_TYPE_PDP
{
    SWI_PDP_IP,               /*!< IPV4. */
    SWI_PDP_PPP,              /*!< PPP. */
    SWI_PDP_IPV6              /*!< IPV6. */
};


/* structure declarations */
/**
 * This structure is used as output to SwiGetAvailableFeatures
 * state. 
 */
struct SWI_STRUCT_AvailFeatures
{
    /*! size of this structure */
    swi_uint32           sizeStruct;
    /*! is voice supported? */
    swi_bool             bVoice;
    /*! Tri-band supported?  (Single-band 1900mhz only if false) */
    swi_bool             bTriBand;
    /*! PDP type supported */
    enum SWI_TYPE_PDP    pdpType;
};

/**
 * This structure contains the current frequency band and a list of all 
 * frequency bands that the modem supports.  The bChangeSuccess field
 * should not be used.  
 *
 * \sa SwiGetBandInfo
 * \sa SWI_NOTIFY_Band
 *
 */
struct SWI_STRUCT_Band 
{
    /*! size of this structure */
    swi_uint32              sizeStruct;             
    /*! Number of valid bands in the array */
    swi_uint32              cntValidBands;          
    /*! Current Band */
    enum SWI_TYPE_Band      eBandType;              
    /*! List of bands */
    enum SWI_TYPE_Band      arrBand[NUMBER_Band];   
    /*! unused parameter */
    swi_bool                bChangeSuccess;         
};



/**
 * This structure contains the current frequency band and a list of all 
 * frequency bands that the modem supports.  
 *
 * \sa SwiGetBandInfo
 */
struct CNS_BAND_PAYLOAD {
    /*! Current Band */
    swi_uint16 CurBand;        
    /*! Number of valid bands */
    swi_uint16 cntValidBands;  
    /*! List of bands */
    swi_uint16 Band[5];        

};

/**
 * This structure is used as output by the SwiGetSerialNumber function. 
 */
struct SWI_STRUCT_Fsn
{
    /*! size of this structure. */
    swi_uint32     sizeStruct;
    /*! 15-character null terminated factory serial number string */
    swi_char  szFsn[LENGTH_Fsn];
};

/**
 * This structure is used as output by the SwiGetTestHistoryByte function. 
 */
struct SWI_STRUCT_Thb
{
    /*! size of this structure. */
    swi_uint32  sizeStruct;           
    /*! Initial test flag
    *  1 - initial tests passed
    *  0 - otherwise */
    swi_bool    bInitialTest;   
    /*! RF Screen test flag
    *  1 - RF screen tests passed
    *  0 - otherwise */
    swi_bool    bRFScreen;     
    /*! Calibration test flag
    *  1 - calibration tests passed
    *  0 - otherwise */
    swi_bool    bCalibration;   
    /*! Systems test flag
    *  1 - systems tests passed
    *  0 - otherwise */
    swi_bool    bSystem;        
};

/**
 *  This structure contains the modem disable state. 
 */
struct SWI_STRUCT_ModemDisabled
{
    /*! size of this structure. */
    swi_uint32  sizeStruct;        
    /*! modem is disabled flag
    *  1 - modem is disabled
    *  0 - modem is enabled */
    swi_bool    bModemDisabled;    
};

/**
 *
 * This structure contains the International Mobile Equipment Identity (IMEI).
 *
 * The IMEI is a number unique to every GSM mobile phone.  This structure is 
 * used as output by the SwiGetImei function. 
 *
 * \sa SwiGetImei
 */
struct SWI_STRUCT_Imei
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct;     
    
    /*! A NULL terminated string containing the IMEI. */
    swi_char szImei[LENGTH_Imei]; 
    
    /*! A 6 character, NULL terminated string representing the Type Approval 
        Code. */
    swi_char szTac[LENGTH_Tac];
    
    /*! A 2 character, NULL terminated string representing the Assembly Code. */
    swi_char szFac[LENGTH_Fac];
    
    /*! A 6 character, NULL terminated string representing the Serial Number. */
    swi_char szSnr[LENGTH_Snr];
    
    /*! Check byte */
    swi_char szCheckByte[LENGTH_ChkByte];
};

/**
 * This structure is used as input by the SwiGetFeatureCustomizations 
 * function.
 */
struct SWI_STRUCT_FeatureCustomizations
{
    /*! size of this structure. */
    swi_uint32    sizeStruct;
    /*! current version of this object. */
    swi_uint32    nVersion;
    /*! auto connect allowed
    *   1 - allowed
    *   0 - not allowed */
    swi_bool      bAutoConnect;
    /*! advanced profiles allowed
    *  1 - allowed
    *  0 - not allowed */
    swi_bool      bAdvanceProfile;
    /*! hide byte counters
    *  1 - hide
    *  0 - display */
    swi_bool      bDisplayDun;
    /*! hide byte counters
    *  1 - hide
    *  0 - display */
    swi_bool      bHideByteCounters;
    /*! circuit switch connection supported
    *  1 - supported
    *  0 - not supported */
    swi_bool      bAllowCsd;
    /*! HSDPA test channel supported
    *  1 - supported
    *  0 - not supported */
    swi_bool      bHSDPASupport;
    /*! feature is enabled in firmware (version 3 field)
    *  0  firmware will not do the profile scanning so software is free to.
    *  1  firmware will deal will do the profile scanning */
    swi_bool      bScanProfile;
    /*! Bluetooth support
    *  1 - supported
    *  0 - not supported */
    swi_bool      bBluetooth;
    /*! GPS support
    *  1 - supported
    *  0 - not supported */
    swi_bool      bGps;
};

/* function prototypes */
extern SWI_API SWI_RCODE SwiGetImei(struct SWI_STRUCT_Imei *pImei, 
                                    swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiGetBandInfo(struct SWI_STRUCT_Band *sBandInfo, 
                                        swi_uint32 sizeBuffer, 
                                        swi_uint32 timeout);
                                        
extern SWI_API SWI_RCODE SwiGetAvailableFeatures(struct SWI_STRUCT_AvailFeatures * sFeatures,
                                                swi_uint32 sizeBuffer, 
                                                swi_uint32 timeout);
                                         
extern SWI_API SWI_RCODE SwiGetSerialNumber(struct SWI_STRUCT_Fsn *sFsnp, 
                                            swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiGetTestHistoryByte (struct SWI_STRUCT_Thb *Thbp, swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiGetModemDisable(swi_bool* pModemDisable, 
                                            swi_uint32 timeout);
                                            
extern SWI_API SWI_RCODE SwiSetModemDisable(const swi_bool bModemDisable, 
                                            swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiGetFeatureCustomizations(struct SWI_STRUCT_FeatureCustomizations* sCustomizationsp, 
                                                     swi_uint32 timeout);
                                                     
extern SWI_API SWI_RCODE SwiStopAllNotif(swi_uint32 timeout);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __SWIAPI_GSM_BASIC */

/* 
 * $Log: SwiApiGsmBasic.h,v $
 */
