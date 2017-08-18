/**
 * \ingroup fms
 *
 * \file    qaGobiApiFms.c
 *
 * \brief   Entry points for Gobi APIs for the Firmware Management Service (FMS)
 *
 * Copyright: © 2012 Sierra Wireless, Inc. all rights reserved
 *
 */
#include <string.h>
#include "sludefs.h"
#include "SwiDataTypes.h"
#include "qaQmiBasic.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "amudefs.h"
#include "qaGobiApiFms.h"
#include "qaFmsSetImagePath.h"
#include "qaFmsSetFirmwarePreference.h"
#include "qaFmsGetImagesPreference.h"
#include "qaFmsSetImagesPreference.h"
#include "qaFmsSlqsGetFirmwareInfo.h"
#include "qaFmsSlqsGetImageInfo.h"
#include "qaFmsGetImageInfo.h"
#include "qaFmsEnableForceDownload.h"
#include "qaGobiApiDms.h"
#include "qaGobiApiDcs.h"
#include "swi_osapi.h"
#include "qaDmsGetDeviceRevisionID.h"
#include "qaFmsGetFirmwareInfo.h"
#include "qaFmsGetStoredImages.h"
#include "qaFmsDeleteStoredImage.h"
#include "qaGobiApiCbk.h"
#include <syslog.h>
#include <stdio.h>

#define LOG_TAG "SDK"
#include <utils/Log.h>


#define MC_SERIES      "MC77"
#define WP_SERIES      "WP71"
#define WP75XX         "WP75"
#define WP76XX         "WP76"
#define GOBI5K_SERIES  "MC73"
#define MC78_SERIES    "MC78"
#define MC7804         "MC7804"
#define MC7304         "MC7304"
#define AR75_SERIES    "AR75"
#define EM_SERIES      "EM73"
#define EM88_SERIES    "EM88"
#define SL8092RESPIN   "SL8092RD"
#define SL_MC_9090     "9090"

/* for eGetDeviceSeries */
#define MC74XX         "MC74"
#define EM74XX         "EM74"
#define EM75XX         "EM75"
#define MC83XX         "MC83"
#define SL9090         "SL9090"
#define MC9090         "MC9090"
#define EM73XX         "EM73"
#define SL8092RD       "SL8092RD"

#define BOOT_VERSION_2_0_2_2 0x02000202 /* this is the boot version still use G3K firmware download method */

#define IMGMASK_IGNORE 0xFFFF

#define PRI_IMGTYPE      0x01
#define MDM_IMGTYPE      0x00
#define ASCII_0          0x30
#define MAX_MODEL_ID_SIZE 128
#define RESET_WITH_DM_CMD 0x10
#define HOST   1
#define DEVICE 0
#define MAX_IMAGE_PATH     514
#define FW_DOWNLOAD_TIMEOUT_SEC 180
#define FW_PRI_BUILD_MATCH_LEN 11
#define IMG_ID_LEN   16
#define BUILD_ID_LEN   32

static volatile BOOL fwDownloadDone = FALSE;

/* Local structures */
local ULONG GobiImageInfoTable[][3] =
{
    {0,     eGOBI_IMG_CAR_FACTORY,          eGOBI_IMG_REG_NA    },
    {1,     eGOBI_IMG_CAR_VERIZON,          eGOBI_IMG_REG_NA    },
    {2,     eGOBI_IMG_CAR_SPRINT,           eGOBI_IMG_REG_NA    },
    {3,     eGOBI_IMG_CAR_ATT,              eGOBI_IMG_REG_NA    },
    {4,     eGOBI_IMG_CAR_VODAFONE,         eGOBI_IMG_REG_EU    },
    {5,     eGOBI_IMG_CAR_TMOBILE,          eGOBI_IMG_REG_EU    },
    {9,     eGOBI_IMG_CAR_GENERIC,          eGOBI_IMG_REG_GLOBAL},
    {10,    eGOBI_IMG_CAR_GENERIC_CDMA,     eGOBI_IMG_REG_GLOBAL},
    {11,    eGOBI_IMG_CAR_ORANGE,           eGOBI_IMG_REG_EU    },
    {12,    eGOBI_IMG_CAR_TELEFONICA,       eGOBI_IMG_REG_EU    },
    {13,    eGOBI_IMG_CAR_NTT_DOCOMO,       eGOBI_IMG_REG_ASIA  },
    {14,    eGOBI_IMG_CAR_TELCOM_ITALIA,    eGOBI_IMG_REG_EU    },
    {18,    eGOBI_IMG_CAR_TELCOM_NZ,        eGOBI_IMG_REG_AUS   },
    {19,    eGOBI_IMG_CAR_CHINA_TELECOM,    eGOBI_IMG_REG_ASIA  },
    {20,    eGOBI_IMG_CAR_OMH,              eGOBI_IMG_REG_GLOBAL},
    {22,    eGOBI_IMG_CAR_AMX_TELCEL,       eGOBI_IMG_REG_LA    },
    {23,    eGOBI_IMG_CAR_NORF,             eGOBI_IMG_REG_GLOBAL},
    {24,    eGOBI_IMG_CAR_FACTORY,          eGOBI_IMG_REG_GLOBAL},
    {25,    eGOBI_IMG_CAR_BRASIL_VIVO,      eGOBI_IMG_REG_LA},
    {160,   eGOBI_IMG_CAR_AERIS,            eGOBI_IMG_REG_NA},
    {161,   eGOBI_IMG_CAR_ROGERS,           eGOBI_IMG_REG_NA}

};
/*structure to extract CarrierId from carrier name*/
struct carrierData
{
   CHAR carrierName[20];
   ULONG  carrierId;
};

struct carrierData carrierDataMap[] =
{
    {"UNKNOWN",      0 },
    {"GENERIC" ,     1 },
    {"TELSTRA",      2 },
    {"RESERVED" ,    3 },
    {"ATT" ,         4 },
    {"VERIZON" ,     5 },
    {"RESERVED" ,    6 },
    {"RESERVED" ,    7 },
    {"RESERVED" ,    8 },
    {"RESERVED" ,    9 },
    {"RESERVED" ,    10 },
    {"SPRINT" ,      11 },
    {"TELEFONICA" ,  12 },
    {"VERIZON2" ,    101 },
    {"SPRINT2" ,     102 },
    {"ALLTEL" ,      103 },
    {"BELL"   ,      104 },
    {"TELUS" ,       105 },
    {"US" ,          106 },
    {"TELSTRA2" ,    107 },
    {"CHINA-UNICOM", 108 },
    {"TELECOM-NZ",   109 },
    {"SK-TELECOM",   110 },
    {"RELIANCE",     111},
    {"TATA",         112},
    {"METROPCS",     113},
    {"LEAP",         114},
    {"KDDI" ,        115 },
    {"IUSACELL" ,    116 },
    {"CHINA-TELECOM",117 },
    {"OMH" ,         118 },
    {"ROGERS" ,      176 },
    {"NETINDEX",     177 },
    {"DNA" ,         178 },
    {"BIG-POND",     179 },
    {"AERIS",        180 },
    {"ATT2",         201 },
    {"VODAFONE" ,    202 },
    {"TMOBILE" ,     203 },
    {"Orange" ,      204 },
    {"TELEFONICA2" , 205 },
    {"TELCOM-ITALIA",206 },
    {"3" ,           207 },
    {"O2" ,          208 },
    {"SFR" ,         209 },
    {"SWISSCOM" ,    210 },
    {"CHINA-MOBILE" ,211 },
    {"TELSTRA3" ,    212 },
    {"SINGTEL-OPTUS",213 },
    {"RELIANCE2" ,   214 },
    {"BHARTI" ,      215 },
    {"DOCOMO" ,      216 },
    {"EMOBILE" ,     217 },
    {"SOFTBANK" ,    218 },
    {"KT-FREETEL" ,  219 },
    {"SK-TELCOM2" ,  220 },
    {"TELENOR" ,     221 },
    {"NETCOM" ,      222 },
    {"TELIASONERA" , 223 },
    {"AMX-TELCEL" ,  224 },
    {"BRASIL-VIVO" , 225 },
    {"PTCRB" ,       254 },
    {"RESERVED" ,    255 },
};

/*structure to extract folderId from carrier name*/
struct devFolderData
{
   CHAR carrierName[20];
   ULONG  folderId;
};

//FileHold #4118344
struct devFolderData folderDataMap[] =
{
    {"VODAFONE",     0 },
    {"VERIZON" ,     1 },
    {"ATT" ,         2 },
    {"SPRINT",       3 },
    {"TMOBILE" ,     4 },
    {"GENERIC"   ,   6 },
    {"TELEFONICA",   7 },
    {"ITALIA",       8 },
    {"ORANGE",       9 },
    {"ORANGE-EU",    9 },
    {"TELSTRA",      10 },
    {"BELL",         12 },
    {"TELUS",        13 },
    {"ROGERS",       14 },
    {"DOCOMO",       15 },
    {"SWISSCOM",     16 },
    {"AERIS",        17 },
    {"SOFTBANK",     18 },
    {"KDDI",         19 },
    {"KT",           20 },
};

static CHAR     devFirmwareBuildId[5][32];
static CHAR     devPriBuildId[50][32];
static BYTE     devFirmwareImageId[5][16];
static BYTE     devPriImageId[50][16];

BOOL IsGobiDevice()
{
    BYTE stringsize = 128;
    CHAR buffer[stringsize];
    ULONG bootversion;
    ULONG rc;
    
    GetModelID( stringsize, buffer );

    if ( strstr(buffer, MC_SERIES) || strstr(buffer, SL8092RESPIN))
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

BOOL Is9x30Device()
{
    BYTE stringsize = 128;
    CHAR ModelId[stringsize];

    GetModelID( stringsize,
                ModelId );

    if ((strstr(ModelId, MC74XX)) ||
        (strstr(ModelId, EM74XX)))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL Is9x15Device()
{
    BYTE stringsize = 128;
    CHAR ModelId[stringsize];

    GetModelID( stringsize,
                ModelId );

    if ((strstr(ModelId, WP_SERIES)) ||
        (strstr(ModelId, WP75XX)) ||
        (strstr(ModelId, GOBI5K_SERIES)) ||
        (strstr(ModelId, MC78_SERIES)) ||
        (strstr(ModelId, AR75_SERIES)) ||
        (strstr(ModelId, EM_SERIES)) ||
        (strstr(ModelId, EM88_SERIES)))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL Is8092RespinDevice()
{
    BYTE stringsize = 128;
    CHAR ModelId[stringsize];

    GetModelID( stringsize,
                ModelId );

    if (strcmp(ModelId, SL8092RESPIN) == 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

ULONG GetImageStore(
    WORD  imageStorePathSize,
    CHAR  *pImageStorePath )
{
    UNUSEDPARAM( imageStorePathSize );
    UNUSEDPARAM( pImageStorePath );

    return eQCWWAN_ERR_SWICM_NOT_IMPLEMENTED;
}

local ULONG GetImageInfoInternal(
    LPCSTR pPath,
    ULONG  mbntype,
    struct qm_qmi_response_tlvs_values *presp )
{
    BYTE    *pInParam;    /* ptr to param field rx'd from modem */
    BYTE    *pOutParam;   /* ptr to outbound param field */
    BYTE    *pReqBuf;     /* Pointer to outgoing request buffer */
    WORD    ParamLength;  /* Ret'd length of the QMI Param field */

    ULONG eRCode;

    /* initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* pack the QMI request */
    eRCode = PkQmiGetMbnInfo( &ParamLength,
                              pOutParam,
                              pPath,
                              mbntype );

    if( eRCode != eQCWWAN_ERR_NONE )
    {
        qmrelreqbkp();
        return eRCode;
    }

    /* send the QMI request */
    eRCode = SwiQmiSendnWait( pReqBuf,
                              eQMI_SVC_FMS,
                              ParamLength,
                              eQMI_TIMEOUT_5_S,  /* 5 seconds */
                              &pInParam,
                              &ParamLength );

    if( eRCode == eQCWWAN_ERR_NONE )
    {
        /* unpack the QMI response */
        eRCode = UpkQmiGetMbnInfo( pInParam, presp);

        /* check for unpack error */
        if( eRCode != eQCWWAN_ERR_NONE )
        {
            qmrelreqbkp();
            return eRCode;
        }

        /* Check that the mandatory response TLV was received */
        if( !presp->qmiresult.validresponse )
        {
            qmrelreqbkp();
            return eQCWWAN_ERR_INVALID_QMI_RSP;
        }

        /* check mandatory response tlv result code */
        if( presp->qmiresult.result != eQCWWAN_ERR_NONE )
        {
            qmrelreqbkp();
            return presp->qmiresult.error;
        }
    }

    qmrelreqbkp();
    return eRCode;
}

ULONG GetImageInfoInt(
    CHAR  *pPath,
    ULONG *pFirmwareID,
    ULONG *pTechnology,
    ULONG *pCarrier,
    ULONG *pRegion,
    ULONG *pGPSCapability )
{
    enum eQCWWANError eRCode;       /* Result of SwiQmiSendnWait() */

    struct qm_qmi_response_tlvs_values response;

    /* input parameter validation */
    if ( !pPath       ||
         !pFirmwareID ||
         !pTechnology ||
         !pCarrier    ||
         !pRegion     ||
         !pGPSCapability )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* output parameter initialization - set values to unknown */
    *pTechnology    = 0xFFFFFFFF;
    *pCarrier       = 0xFFFFFFFF;
    *pRegion        = 0xFFFFFFFF;
    *pGPSCapability = 0xFFFFFFFF;

    GetImageInfoInternal( pPath, MBN_IMG_TYPE_UQCN, &response);

    /* pointer to mbn image response data */
    struct qm_fms_mbn_response_tlv_values *pdat =
        &response.tlvvalues.qmfmstlvs.mbn_rsp;

    /* copy firmware ID */
     *pFirmwareID = pdat->firmwareid;

    /* parse the UQCN version ID to extract technology and gps capability */
    eRCode = qaParseFirmwareID( *pFirmwareID,
                                *pCarrier,
                                pTechnology,
                                pGPSCapability );

    if( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    /* determine firmware major version id */
    ULONG majorid =
    (*pFirmwareID & GOBI_FW_MAJOR_ID_MASK) >> GOBI_FW_MAJOR_ID_OFFSET;

    /* determine carrier and region */
    BYTE n = sizeof(GobiImageInfoTable)/sizeof(GobiImageInfoTable[0]);
    BYTE i;
    for( i = 0 ; i < n ; i++ )
    {
        if( majorid == GobiImageInfoTable[i][0] )
        {
            *pCarrier = GobiImageInfoTable[i][1];
            *pRegion = GobiImageInfoTable[i][2];
            break;
        }
    }

    return eRCode;
}

local ULONG GetFirmwareInfoInternal(
    ULONG *pFirmwareID,
    ULONG *pTechnology,
    ULONG *pCarrier,
    ULONG *pRegion,
    ULONG *pGPSCapability )
{
    ULONG  resultcode;  /* Result of SwiQmiSendnWait() */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT ParamLength; /* Ret'd length of the  QMI Param field */
    CHAR   AMSSString[QMI_MAX_REV_ID_LENGTH];
    CHAR   BootString[QMI_MAX_REV_ID_LENGTH];
    CHAR   PRIString[QMI_MAX_REV_ID_LENGTH];

    /* Storage for results and response variable */
    struct QmiDmsGetDeviceRevIDResp response;

    /* initialize the response structure as in some cases, the firmware file may not have
       all the firmware related information, such case causes un-initialized response structrue
       will result in memory corruptions. */
    memset(&response, 0, sizeof(response));
    
    /* Input parameter validation  */
    if( !pFirmwareID ||
        !pTechnology ||
        !pCarrier    ||
        !pRegion     ||
        !pGPSCapability )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* output parameter initialization - set values to unknown */
    *pTechnology    = 0xFFFFFFFF;
    *pCarrier       = 0xFFFFFFFF;
    *pRegion        = 0xFFFFFFFF;
    *pGPSCapability = 0xFFFFFFFF;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* pack QMI request */
    resultcode = PkQmiDmsGetDeviceRevID( &ParamLength,
                                         pOutParam  );
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* send QMI request */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_DMS,
                                  ParamLength,
                                  eQMI_TIMEOUT_2_S,
                                  &pInParam,
                                  &ParamLength );

    if (resultcode == eQCWWAN_ERR_NONE)
    {
        slmemset(AMSSString, 0, sizeof(AMSSString) );
        slmemset(BootString, 0, sizeof(BootString) );
        slmemset(PRIString,  0, sizeof(PRIString) );

        /* initialize response structure */
        response.pAMSSString = AMSSString;
        response.pBootString = BootString;
        response.pPRIString  = PRIString;
        response.amssSize    = sizeof(AMSSString);
        response.bootSize    = sizeof(BootString);
        response.priSize     = sizeof(PRIString);

        /* Copy to the caller's buffer */
        resultcode = UpkQmiDmsGetDeviceRevID(pInParam, &response);
    }

    if( resultcode == eQCWWAN_ERR_NONE )
    {
        if( slstrlen(PRIString) != QMI_PRI_STRING_LENGTH )
        {
            qmrelreqbkp();
            return eQCWWAN_ERR_INVALID_QMI_RSP;
        }
        else
        {
            /* convert PRI string to firmware ID */
            *pFirmwareID = slahtol(PRIString);

            /* parse the firmware ID to extract technology and gps capability */
            resultcode = qaParseFirmwareID( *pFirmwareID,
                                            *pCarrier,
                                            pTechnology,
                                            pGPSCapability );

            if( eQCWWAN_ERR_NONE != resultcode )
            {
                qmrelreqbkp();
                return resultcode;
            }

            /* determine firmware major version id */
            ULONG majorid =
            (*pFirmwareID & GOBI_FW_MAJOR_ID_MASK) >> GOBI_FW_MAJOR_ID_OFFSET;

            /* determine carrier and region */
            BYTE n = sizeof(GobiImageInfoTable)/sizeof(GobiImageInfoTable[0]);
            BYTE i;
            for( i = 0 ; i < n ; i++ )
            {
                if( majorid == GobiImageInfoTable[i][0] )
                {
                    *pCarrier = GobiImageInfoTable[i][1];
                    *pRegion = GobiImageInfoTable[i][2];
                    break;
                }
            }
        }
    }

    qmrelreqbkp();
    return resultcode;
}

local ULONG SLQSGetFirmwareInfoInternal(
    struct slqsfwinfo_s *pinfo )
{
    ULONG  resultCode;  /* Result of SwiQmiSendnWait() */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiFmsSlqsGetFirmwareInfoResp response;

    /* initialize the response structure as in some cases, the firmware file may not have
       all the firmware related information, such case causes un-initialized response structrue
       will result in memory corruptions. */
    memset(&response, 0, sizeof(response));

    /* pinfo is an OUT parameter and hence should not be NULL */
    if ( !pinfo )
        return eQCWWAN_ERR_INVALID_ARG;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message and sends the message. */
    resultCode = PkQmiFmsSlqsGetFirmwareInfo( &paramLength, pOutParam );

    if ( eQCWWAN_ERR_NONE != resultCode )
    {
        qmrelreqbkp();
        return resultCode; /* No Memory */
    }

    /* Prepare and send the blocking call */
    resultCode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_DMS,
                                  paramLength,
                                  eQMI_TIMEOUT_2_S, /* 2 Seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultCode )
    {
        /* Copy the obtained values to the function OUT parameters */
        slmemset((char *)pinfo, 0, sizeof(struct slqsfwinfo_s));
        response.pinfo = pinfo;

        /* Copy to the caller's buffer */
        resultCode = UpkQmiFmsSlqsGetFirmwareInfo( pInParam, &response );
    }

    qmrelreqbkp();
    return resultCode;
}

ULONG SLQSGetFirmwareInfo(
    struct qmifwinfo_s *pinfo )
{
    ULONG  resultCode;
    struct sGetDeviceSeriesResult result;
    eGetDeviceSeries(&result);
    
    if (result.eDevice == eGOBI_DEV_SERIES_NON_GOBI ||
        result.eDevice == eGOBI_DEV_SERIES_9X15 || 
        result.eDevice == eGOBI_DEV_SERIES_9X07_9X50 ||
        result.eDevice == eGOBI_DEV_SERIES_9X30 )
    {
        /* Found a non-Gobi device */
        resultCode =  SLQSGetFirmwareInfoInternal( &pinfo->dev.s );

    }
    else
    {
        resultCode =   GetFirmwareInfoInternal( &pinfo->dev.g.FirmwareID,
                                        &pinfo->dev.g.Technology,
                                        &pinfo->dev.g.Carrier,
                                        &pinfo->dev.g.Region,
                                        &pinfo->dev.g.GPSCapability );
    }
    return resultCode;
}

ULONG SLQSGetImageInfoInternal(
    LPCSTR              path,
    BYTE                imgType,
    struct slqsfwinfo_s *pinfo )
{
    BYTE              *pInParam;    /* ptr to param field rx'd from modem */
    BYTE              *pOutParam;   /* ptr to outbound param field */
    BYTE              *pReqBuf;     /* Pointer to outgoing request buffer */
    WORD              ParamLength;  /* Ret'd length of the QMI Param field */
    enum eQCWWANError eRCode;       /* Result of SwiQmiSendnWait() */

    struct qm_qmi_response_tlvs_values response;

    /* input argument validation */
    if ( !pinfo || !path  )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* pack the QMI request */
    eRCode = PkQmiGetCweSpkgsInfo(  &ParamLength,
                                    pOutParam,
                                    path,
                                    imgType );

    if( eRCode != eQCWWAN_ERR_NONE )
    {
        qmrelreqbkp();
        return eRCode;
    }

    /* send the QMI request */
    eRCode = SwiQmiSendnWait( pReqBuf,
                              eQMI_SVC_FMS,
                              ParamLength,
                              eQMI_TIMEOUT_5_S,  /* 5 seconds */
                              &pInParam,
                              &ParamLength );

    if( eRCode == eQCWWAN_ERR_NONE )
    {
        /* Initialize the response structure */
        slmemset((char *)&response, 0, sizeof(response));

        /* unpack the QMI response */
        eRCode = UpkQmiGetCweSpkgsInfo( pInParam, &response );

        /* check for unpack error */
        if( eRCode != eQCWWAN_ERR_NONE )
        {
            qmrelreqbkp();
            return eRCode;
        }

        /* Check that the mandatory response TLV was received */
        if( !response.qmiresult.validresponse )
        {
            qmrelreqbkp();
            return eQCWWAN_ERR_INVALID_QMI_RSP;
        }

        /* check mandatory response tlv result code */
        if( response.qmiresult.result != eQCWWAN_ERR_NONE )
        {
            qmrelreqbkp();
            return response.qmiresult.error;
        }
    }

    /* clear the user memory before copying any data */
    slmemset( (void *)pinfo,
              0 ,
              sizeof(struct slqsfwinfo_s) );

    /* intialize the buffer provided by the user */
    struct qm_fms_cwe_spkgs_response_tlv_values *pdat =
        &response.tlvvalues.qmfmstlvs.cwe_spkgs_rsp;

    /* copy application version */
    slstrncpy( pinfo->appversion_str,
               pdat->fwversion_str,
               slstrlen(pdat->fwversion_str) );

    /* copy boot version */
    slstrncpy( pinfo->bootversion_str,
               pdat->fwversion_str,
               slstrlen(pdat->fwversion_str) );

    /* copy carrier string */
    slstrncpy( pinfo->carrier_str,
               pdat->carrier_str,
               slstrlen(pdat->carrier_str) );

    /* copy model id */
    slstrncpy( pinfo->modelid_str,
               pdat->modelid_str,
               slstrlen(pdat->modelid_str) );

    /* copy package id */
    slstrncpy( pinfo->packageid_str,
               pdat->packageid_str,
               slstrlen(pdat->packageid_str) );

    /* copy pri version */
    slstrncpy( pinfo->priversion_str,
               pdat->priversion_str,
               slstrlen(pdat->priversion_str) );

    /* copy sku string */
    slstrncpy( pinfo->sku_str,
               pdat->sku_str,
               slstrlen(pdat->sku_str) );

    qmrelreqbkp();
    return eRCode;
}

ULONG SLQSGetImageInfoMC83xx(
    LPCSTR              path,
    struct qmifwinfo_s *pinfo )
{
    return  GetImageInfoInt( (CHAR *)path,
                             &pinfo->dev.g.FirmwareID,
                             &pinfo->dev.g.Technology,
                             &pinfo->dev.g.Carrier,
                             &pinfo->dev.g.Region,
                             &pinfo->dev.g.GPSCapability );
}

ULONG SLQSGetImageInfoMC77xx(
    LPCSTR              path,
    struct qmifwinfo_s *pinfo )
{
    ULONG errCode = eQCWWAN_ERR_NONE;
    struct qmifwinfo_s info;
    memset(&info, 0, sizeof(struct qmifwinfo_s));
    if (eQCWWAN_ERR_NONE != SLQSGetImageInfoInternal( path,
                                    (BYTE)IMG_FW_TYPE_CWE,
                                     &pinfo->dev.s ))
    {
        return SLQSGetImageInfoInternal( path,
                                     (BYTE)IMG_FW_TYPE_SPK,
                                     &pinfo->dev.s );
    }
    else
    {
        /* If CWE is found at path, let us search for NVU
         * if NVU is found, return information of NVU,
         * otherwise return CWE info
         */
        errCode = SLQSGetImageInfoInternal( path,
                                    (BYTE)IMG_PRI_TYPE_NVU,
                                     &info.dev.s );
        if (errCode == eQCWWAN_ERR_NONE)
            memcpy(&pinfo->dev.s, &info.dev.s, sizeof(info.dev.s));
        return eQCWWAN_ERR_NONE;
    }

}

ULONG SLQSGetImageInfo(
    LPCSTR              path,
    struct qmifwinfo_s *pinfo )
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    struct sGetDeviceSeriesResult result;
    eGetDeviceSeries(&result);
        
    if (( result.eDevice == eGOBI_DEV_SERIES_NON_GOBI ) ||
       (( result.eDevice == eGOBI_DEV_SERIES_SIERRA_GOBI) && (SLQSIsSpkgFormatRequired())))
    {
        /* MC77xx device */
        eRCode =  SLQSGetImageInfoMC77xx( path, pinfo );
    }
    else if( result.eDevice == eGOBI_DEV_SERIES_9X15 || 
        result.eDevice == eGOBI_DEV_SERIES_9X30)
    {
        /* 9x15 device */
        eRCode = SLQSGetImageInfo_9x15(path, IMG_FW_TYPE_CWE, &pinfo->dev.s );

        if( eQCWWAN_ERR_NONE != eRCode )
        {
            eRCode = SLQSGetImageInfo_9x15(path, IMG_PRI_TYPE_NVU, &pinfo->dev.s );
        }
    }
    else
    {
        /* Found a MC83xx */
        eRCode =  SLQSGetImageInfoMC83xx( (CHAR *)path,
                                       pinfo);
    }

    return eRCode;
}

ULONG SLQSGetImageInfo_9x15(
    LPCSTR              path,
    BYTE                imgType,
    struct slqsfwinfo_s *pinfo )
{
    return SLQSGetImageInfoInternal( path,
                                     imgType,
                                     pinfo );
}

/*
 *  NOTE: This API is not to be exposed to a client application!
 *
 *  Used by UpgradeFirmware2k to set the fully qualified path of directory
 *  containing candidate firmware to be downloaded to the device.
 *
 *  \param  path[IN]
 *          - fully qualified path to firmware image to download
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Support: UMTS/CDMA\n
 *          Timeout: N/A
 *
 */
ULONG SetFirmwarePreference()
{
    BYTE                *pInParam;    /* ptr to param field rx'd from modem */
    BYTE                *pOutParam;   /* ptr to outbound param field */
    BYTE                *pReqBuf;     /* Pointer to outgoing request buffer */
    WORD                ParamLength;  /* Ret'd length of the QMI Param field */
    enum eQCWWANError   eRCode;       /* Result of SwiQmiSendnWait() */

    struct qm_qmi_response_tlvs_values response;

    /* initialize the response structure as in some cases, the firmware file may not have
       all the firmware related information, such case causes un-initialized response structrue
       will result in memory corruptions. */
    memset(&response, 0, sizeof(response));

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    eRCode = PkQmiFmsSetFirmwarePreference( &ParamLength,
                                            pOutParam  );

    if (eRCode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return eRCode;
    }

    /* send the request to the SDK process */
    eRCode = SwiQmiSendnWait( pReqBuf,
                              eQMI_SVC_DMS,
                              ParamLength,
                              eQMI_TIMEOUT_30_S,  /* 30 seconds */
                              &pInParam,
                              &ParamLength );

    if (eRCode == eQCWWAN_ERR_NONE)
    {
        /* unpack QMI response */
        eRCode = UpkQmiFmsSetFirmwarePreference( pInParam, &response );

        /* check for unpack error */
        if( eRCode != eQCWWAN_ERR_NONE )
        {
            qmrelreqbkp();
            return eRCode;
        }

        /* Check that the mandatory response TLV was received */
        if( !response.qmiresult.validresponse )
        {
            qmrelreqbkp();
            return eQCWWAN_ERR_INVALID_QMI_RSP;
        }

        /* check mandatory response tlv result code */
        if( response.qmiresult.result != eQCWWAN_ERR_NONE )
        {
            qmrelreqbkp();
            return response.qmiresult.error;
        }
    }

    qmrelreqbkp();
    return eRCode;
}

/*
 *  NOTE: This API is not to be exposed to a client application!
 *
 *  Used to enable the force download of firmware in boot mode.
 *
 *  \param  None
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   None
 *
 */
ULONG EnableForceDownload()
{
    BYTE                *pInParam;    /* ptr to param field rx'd from modem */
    BYTE                *pOutParam;   /* ptr to outbound param field */
    BYTE                *pReqBuf;     /* Pointer to outgoing request buffer */
    WORD                ParamLength;  /* Ret'd length of the QMI Param field */
    enum eQCWWANError   eRCode;       /* Result of SwiQmiSendnWait() */

    struct qm_qmi_response_tlvs_values response;

    /* initialize the response structure as in some cases, the firmware file may not have
       all the firmware related information, such case causes un-initialized response structrue
       will result in memory corruptions. */
    memset(&response, 0, sizeof(response));

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    eRCode = PkQmiFmsEnableForceDownload( &ParamLength,
                                          pOutParam  );

    if (eRCode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return eRCode;
    }

    /* send the request to the SDK process */
    eRCode = SwiQmiSendnWait( pReqBuf,
                              eQMI_SVC_FMS,
                              ParamLength,
                              eQMI_TIMEOUT_5_S,  /* 5 seconds */
                              &pInParam,
                              &ParamLength );

    if (eRCode == eQCWWAN_ERR_NONE)
    {
        /* unpack QMI response */
        eRCode = UpkQmiFmsEnableForceDownload( pInParam, &response );

        /* check for unpack error */
        if( eRCode != eQCWWAN_ERR_NONE )
        {
            qmrelreqbkp();
            return eRCode;
        }

        /* Check that the mandatory response TLV was received */
        if( !response.qmiresult.validresponse )
        {
            qmrelreqbkp();
            return eQCWWAN_ERR_INVALID_QMI_RSP;
        }

        /* check mandatory response tlv result code */
        if( response.qmiresult.result != eQCWWAN_ERR_NONE )
        {
            qmrelreqbkp();
            return response.qmiresult.error;
        }
    }

    qmrelreqbkp();
    return eRCode;
}

/*
 *  NOTE: This API is not to be exposed to a client application!
 *
 *  Used by UpgradeFirmware2k to set the fully qualified path of directory
 *  containing candidate firmware to be downloaded to the device.
 *
 *  \param  path[IN]
 *          - fully qualified path to firmware image to download
 *
 *  \param  isMDM9x15[IN]
 *          - parameter indicating whether the device used is a 9x15 device
 *
 *  \param  imgMask[IN]
 *          - Valid only for MDM 9x15 i.e. MC devices with PRI + fw separate
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Support: UMTS/CDMA\n
 *          Timeout: N/A
 *
 */
ULONG SetImagePath(
    CHAR  *path,
    ULONG isMDM9x15,
    WORD  imgMask,
    BYTE  mdmProtocol )
{
    BYTE                *pInParam;    /* ptr to param field rx'd from modem */
    BYTE                *pOutParam;   /* ptr to outbound param field */
    BYTE                *pReqBuf;     /* Pointer to outgoing request buffer */
    WORD                ParamLength;  /* Ret'd length of the QMI Param field */
    enum eQCWWANError   eRCode;       /* Result of SwiQmiSendnWait() */

    struct qm_qmi_response_tlvs_values response;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    eRCode = PkQmiFmsSetImagePath(  &ParamLength,
                                    pOutParam,
                                    path,
                                    isMDM9x15,
                                    imgMask,
                                    mdmProtocol );

    if (eRCode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return eRCode;
    }

    /* send the request to the SDK process */
    eRCode = SwiQmiSendnWait( pReqBuf,
                              eQMI_SVC_FMS,
                              ParamLength,
                              eQMI_TIMEOUT_5_S,  /* 5 seconds */
                              &pInParam,
                              &ParamLength );

    if (eRCode == eQCWWAN_ERR_NONE)
    {
        eRCode = UpkQmiFmsSetImagePath( pInParam, &response );

        /* check for unpack error */
        if( eRCode != eQCWWAN_ERR_NONE )
        {
            qmrelreqbkp();
            return eRCode;
        }

        /* Check that the mandatory response TLV was received */
        if( !response.qmiresult.validresponse )
        {
            qmrelreqbkp();
            return eQCWWAN_ERR_INVALID_QMI_RSP;
        }

        /* check mandatory response tlv result code */
        if( response.qmiresult.result != eQCWWAN_ERR_NONE )
        {
            qmrelreqbkp();
            return response.qmiresult.error;
        }
    }

    qmrelreqbkp();
    return eRCode;
}

local ULONG upgrade_gobi3k_fw(
    LPCSTR path )
{
    ULONG eRCode;
    BYTE  slotID = 0;

    /* get the UQCN image info */
    struct qm_qmi_response_tlvs_values response;
    eRCode = GetImageInfoInternal(path, MBN_IMG_TYPE_UQCN, &response);
    if( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    /* pointer to mbn info response data */
    struct qm_fms_mbn_response_tlv_values *pdat =
        &response.tlvvalues.qmfmstlvs.mbn_rsp;

    /* image preference list construction object */
    struct PrefImageList prefimglist;
    prefimglist.listSize = 2;

    /* construct first entry of the image preference list */
    struct ImageElement *p = &prefimglist.listEntries[0];
    p->imageType = 1; /* 1 => UQCN */
    slmemcpy( p->imageId, pdat->fwidstr, sizeof(pdat->fwidstr) );
    slmemcpy( p->buildId, pdat->fwversionstr, sizeof(pdat->fwversionstr) );
    p->buildIdLength = slstrlen(p->buildId);

    /* Get the AMSS image info. If there's an error, the AMSS image may not
     * exist in the current directory - try the directory for the generic UMTS
     * image.
     */
    CHAR generic[IM_IMAGE_PATH_MAX_SZ];
    slmemcpy( generic, (CHAR *)path, slstrlen(path) + 1);
    ULONG offset = slstrlen(generic) - 2;
    while( generic[offset] != '/' )
    {
        offset--;
    }
    slotID = generic[offset + 1] - ASCII_0; /* Store slotID */
    generic[offset + 1] = IM_AMSS_GENERIC_UMTS_MBN_DIR;
    generic[offset + 2] = 0; /* null termination */

    eRCode = GetImageInfoInternal(path, MBN_IMG_TYPE_AMSS, &response);
    if( eQCWWAN_ERR_NONE != eRCode )
    {
        eRCode = GetImageInfoInternal(generic, MBN_IMG_TYPE_AMSS, &response);
        if( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
    }

    /* construct second entry of the image preference list */
    p = &prefimglist.listEntries[1];
    p->imageType = 0; /* 0 => AMSS */
    slmemcpy( p->imageId, pdat->fwidstr, sizeof(pdat->fwidstr) );
    slmemcpy( p->buildId, pdat->fwversionstr, sizeof(pdat->fwversionstr) );
    p->buildIdLength = slstrlen(p->buildId);

    /* Set the image preferences on the device */
    BYTE imageTypes[8];
    ULONG imageTypesSize = sizeof(imageTypes);
    ULONG bForceDownload = 0x00;
    if ( TRUE == IsSierraGobiDevice() )
    {
        bForceDownload = 0x01;
    }
    else
    {
        bForceDownload = 0x00;
    }
    if( eQCWWAN_ERR_NONE !=
            ( eRCode = SetImagesPreference( prefimglist.listSize,
                                            (BYTE *)&prefimglist,
                                            bForceDownload, /* force download */
                                            slotID, /* SlotID */
                                            &imageTypesSize,
                                            imageTypes) ) )
    {
        return eRCode;
    }

    CHAR *pr = strrchr( generic, '/');
    if ( pr != NULL)
    {
        *(pr + 1) = 0;
    }

    /* set the firmware download image path */
    eRCode = SetImagePath( generic, (ULONG)FALSE, IMGMASK_IGNORE, 0 );
    if( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    /* reset the device to initiate the download */
    if ( eRCode == eQCWWAN_ERR_NONE )
    {
        eRCode = SetPower( DEVICE_SHUTDOWN );
    }

    return eRCode;
}

ULONG upgrade_mc77xx_fw(
    LPCSTR path )
{
    ULONG eRCode;
    BYTE devicemode;

    /* set the firmware download image path */
    if( eQCWWAN_ERR_NONE !=
        (eRCode = SetImagePath((CHAR *)path, (ULONG)FALSE, IMGMASK_IGNORE, 0 ) ))
    {
        return eRCode;
    }

    eRCode = SLQSGetDeviceMode ((BYTE *)&devicemode);

    if ( devicemode != DEVICE_STATE_BOOT )
    {
        /* set the firmware preferences on the device */
        if( eQCWWAN_ERR_NONE != (eRCode = SetFirmwarePreference()) )
        {
            return eRCode;
        }
    }
    return eRCode;
}

ULONG upgrade_9x07_9x50_fw(
    LPCSTR path )
{
    ULONG eRCode;
    BYTE devicemode;

    /* set the firmware download image path */
    if( eQCWWAN_ERR_NONE !=
        (eRCode = SetImagePath((CHAR *)path, (ULONG)FALSE, IMGMASK_IGNORE, 1 ) ))
    {
        return eRCode;
    }

    eRCode = SLQSGetDeviceMode ((BYTE *)&devicemode);

    if ( devicemode != DEVICE_STATE_BOOT )
    {
        /* set the firmware preferences on the device */
        if( eQCWWAN_ERR_NONE != (eRCode = SetFirmwarePreference()) )
        {
            return eRCode;
        }
    }

    return eRCode;
}

local void update_pref_img_list(
        struct PrefImageList *list,
        BYTE index,
        BYTE type,
        char* imageid_left,
        char* imageid_right,
        char* buildid_left,
        char* buildid_right
        )
{
    struct ImageElement *p;
    p = &list->listEntries[index];
    slmemset( (CHAR *)p, 0, sizeof(struct ImageElement));

    p->imageType = type;
    sprintf((char*)p->imageId, "%s_%s", imageid_left, imageid_right);
    sprintf((char*)p->buildId, "%s_%s", buildid_left, buildid_right);
    p->buildIdLength= slstrlen(p->buildId);
}

local void use_pref_img_list_from_modem(
        struct PrefImageList *list,
        BYTE index,
        BYTE type,
        char* imageid,
        char* buildid
        )
{
    struct ImageElement *p;
    p = &list->listEntries[index];
    slmemset( (CHAR *)p, 0, sizeof(struct ImageElement));

    p->imageType = type;
    slmemcpy(p->imageId, imageid, slstrlen(imageid));
    slmemcpy(p->buildId, buildid, slstrlen(buildid));
    p->buildIdLength= slstrlen(p->buildId);
}

local ULONG upgrade_9x15_fw(
    LPCSTR path,
    ULONG bForceDownload)
{
    ULONG eRCode = eQCWWAN_ERR_NONE;
    BOOL  priFlag = FALSE;
    BOOL  FWFlag  = FALSE;
    BOOL  spkFlag = FALSE;
    ULONG genericLen;
    CHAR generic[IM_IMAGE_PATH_MAX_SZ];
    BYTE PriIndex = 0;
    BYTE MdmIndex = 0;

    slmemset( generic, 0, IM_IMAGE_PATH_MAX_SZ );
    slmemcpy( generic, (CHAR *)path, slstrlen(path) + 1);

    /* Account for '/' at end of file path */
    genericLen = slstrlen(generic);
    CHAR *pr = &generic[genericLen - 1];
    if( *pr != '/' )
      *(pr + 1) = '/';

    /* get the PRI Image info */
    struct slqsfwinfo_s priInfo;
    eRCode = SLQSGetImageInfo_9x15(generic, (BYTE)IMG_PRI_TYPE_NVU, &priInfo);
    if( eQCWWAN_ERR_NONE == eRCode )
    {
        priFlag = TRUE;
    }

    /* Get the Fw image info */

    struct slqsfwinfo_s fwInfo;
    eRCode = SLQSGetImageInfo_9x15(generic, (BYTE)IMG_FW_TYPE_CWE, &fwInfo);
    if( eQCWWAN_ERR_NONE == eRCode )
    {
        FWFlag = TRUE;
    }

    /* get the SPK Image info */
    struct slqsfwinfo_s spkInfo;
    /* Get the spk image info, we can treat the SPK case same as PRI case */
    eRCode = SLQSGetImageInfo_9x15(generic, (BYTE)IMG_FW_TYPE_SPK, &spkInfo);
    if( eQCWWAN_ERR_NONE == eRCode )
    {
        spkFlag = TRUE;
    }

    /*
     * Call GetImagesPreference() in case one of the files was not present.
     * ULONG GetImagesPreference( ULONG *pImageListSize,
                                  BYTE  *pImageList ) */

    struct PrefImageList ImageList;
    ULONG imageListSize = sizeof( struct PrefImageList );
    eRCode = GetImagesPreference( &imageListSize,
                                  (BYTE*)&ImageList );
    if( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    /* Determine the PRI index and modem index */
    if( PRI_IMGTYPE == ImageList.listEntries[0].imageType )
    {
        PriIndex = 0;
        MdmIndex = 1;
    }
    else
    {
        PriIndex = 1;
        MdmIndex = 0;
    }

    /* image preference list construction object */
    struct PrefImageList prefimglist;
    slmemset( (CHAR *)&prefimglist, 0, sizeof(struct PrefImageList));

    /* If image was found at the path construct image details from
     * image on the host */
    if( TRUE == priFlag )
    {
        /* image ID(unique ID) : priver_pkgver
         * build ID string as  : fwversion_carrier */
        update_pref_img_list(&prefimglist, 0 /*index*/, PRI_IMGTYPE,
                             priInfo.priversion_str, priInfo.packageid_str,
                             priInfo.appversion_str, priInfo.carrier_str);

        prefimglist.listSize = 1;
    }
    else
    {
        /* wildcard PRI imageid & buildid when downloading MDM(cwe) only */
        update_pref_img_list(&prefimglist, 0 /*index*/, PRI_IMGTYPE,
                             "?", "?",
                             "?", "?");

        prefimglist.listSize = 1;
    }

    /* If image was found at the path construct image details from
     * image on the host */
    if( TRUE == FWFlag)
    {
        /* construct second entry of the image preference list */
        /* image ID(unique ID) : priver_pkgver
         * build ID string as  : fwversion_carrier */
        update_pref_img_list(&prefimglist, 1 /*index*/, MDM_IMGTYPE,
                             fwInfo.priversion_str, fwInfo.packageid_str,
                             fwInfo.appversion_str, fwInfo.carrier_str);

        prefimglist.listSize = prefimglist.listSize + 1;
    }
    else
    {
        /* wildcard MDM imageid & buildid except app version when downloading PRI(nvu) only */
        use_pref_img_list_from_modem(&prefimglist, 1 /*index*/, MDM_IMGTYPE,
                                     (CHAR*)ImageList.listEntries[MdmIndex].imageId,
                                     ImageList.listEntries[MdmIndex].buildId);

        update_pref_img_list(&prefimglist, 1 /*index*/, MDM_IMGTYPE,
                             "?", "?",
                             priInfo.appversion_str, "?");

        prefimglist.listSize = prefimglist.listSize + 1;
    }

    /* special handling for spk file since a FW+PRI will be provied by the spk file */
    if( TRUE == spkFlag )
    {
        /* image ID(unique ID) : priver_pkgver
         * build ID string as  : fwversion_carrier */
        update_pref_img_list(&prefimglist, 0 /*index*/, PRI_IMGTYPE,
                             spkInfo.priversion_str, spkInfo.packageid_str,
                             spkInfo.appversion_str, spkInfo.carrier_str);

        /* if the build ID of the provided spk file is different to the existing PRI build ID,
         * then the spk file shall be downloaded */
        if (slstrcmp(ImageList.listEntries[PriIndex].buildId, prefimglist.listEntries[0].buildId) != 0)
        {
            /* construct second entry of the image preference list */
            /* image ID(unique ID) : priver_pkgver
            * build ID string as  : fwversion_carrier */
            update_pref_img_list(&prefimglist, 1 /*index*/, MDM_IMGTYPE,
                                 spkInfo.priversion_str, spkInfo.packageid_str,
                                 spkInfo.appversion_str, spkInfo.carrier_str);

        }
    }

    /* Set the image preferences on the device */
    BYTE  imageTypes[8];
    ULONG imageTypesSize = sizeof(imageTypes);
    WORD  imageMask = 0;
    WORD  imageCtr = 0;

    if( eQCWWAN_ERR_NONE !=
            ( eRCode = SetImagesPreference( prefimglist.listSize,
                                            (BYTE *)&prefimglist,
                                            bForceDownload, /* force download */
                                            0xFF, /* device manages modem index */
                                            &imageTypesSize,
                                            imageTypes) ) )
    {
        return eRCode;
    }

    /* Set bit mask depending on type of images required for download.
     * Read bit mask as follows:
     * 00 - No image requested by device
     * 01 - Modem Image requested by device
     * 10 - PRI Image requested by device
     * 11 - Both Modem and PRI requested by device */
    for( imageCtr = 0 ; imageCtr < imageTypesSize; imageCtr++ )
    {
        // MDM(0)/PRI(1) becomes 0x01/0x02
        imageMask |= 1 << imageTypes[imageCtr];
    }

    /* only set the image path when the firmware/pri files is required by the modem */
    if ( 0 != (imageMask & ((priFlag<<1)|FWFlag)) )
    {
        syslog(LOG_DEBUG, "priFlag/FWFlag: %d/%d\n", priFlag, FWFlag);

        if ( (priFlag == 0) && (FWFlag == 0) )
        {
            eRCode = SetImagePath( generic, FALSE, IMGMASK_IGNORE, 0 );
        }
        else
        {
            //only set the MDM/PRI mask when cwe/nvu exist
            eRCode = SetImagePath( generic, (ULONG)TRUE, imageMask & ((priFlag<<1)|FWFlag), 0 );
        }

        if( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
    }

    /* reset the device to initiate the download */
    if ( eRCode == eQCWWAN_ERR_NONE )
    {
        eRCode = SetPower( DEVICE_SHUTDOWN );
        if( eQCWWAN_ERR_NONE != eRCode )
        {
            swi_osapilog("SetPower Fail");
        }
        eRCode = SetPower( DEVICE_RESET );
    }

    return eRCode;
}


local void extractpath(CHAR *mPath,ULONG *folderId)
{
    ULONG count = 2;
    CHAR *b;

    while(*(mPath +strlen(mPath)-count) != '/')
    {
        count++;
    }
    b = (mPath + strlen(mPath)-count+1);
    if(*(b+(strlen(b)-1))=='/')
    {
       *(mPath+(strlen(mPath)-1)) = '\0';
    }
    *folderId = atoi(b);
    *(mPath+strlen(mPath)-count+2)= 0;
}

local ULONG upgrade_9x30_fw(
    LPCSTR path,
    ULONG bForceDownload)
{
    ULONG eRCode = eQCWWAN_ERR_NONE;
    ULONG genericLen;
    CHAR generic[IM_IMAGE_PATH_MAX_SZ];

    ULONG  itemsCount =255, index=0;;
    struct SWI_STRUCT_CarrierImage CarrierImages[20];
    CHAR mPath[IM_IMAGE_PATH_MAX_SZ];
    ULONG folderId;
    WORD imageCtr;

    CHAR fw_image_id[16];
    CHAR fw_build_id[32];
    CHAR pri_image_id[16];
    CHAR pri_build_id[32];

    /* image preference list construction object */
    struct PrefImageList prefimglist;
    slmemset( (CHAR *)&prefimglist, 0, sizeof(struct PrefImageList));

    slmemset( generic, 0, IM_IMAGE_PATH_MAX_SZ );
    slmemcpy( generic, (CHAR *)path, slstrlen(path) + 1);

   /* Account for '/' at end of file path */
    genericLen = slstrlen(generic);
    CHAR *pr = &generic[genericLen - 1];
    if( *pr != '/' )
      *(pr + 1) = '/';
    slmemset( mPath, 0, IM_IMAGE_PATH_MAX_SZ );
    slmemcpy( mPath, generic, slstrlen(generic) - 1);   // create mPath from generic and remove trailing '/'
    /* Check for the Image type is SPK file*/
    struct slqsfwinfo_s slqsImageInfo;
    eRCode = SLQSGetImageInfo_9x15( path,
                                         (BYTE)IMG_FW_TYPE_SPK,
                                          &slqsImageInfo );
    if(eQCWWAN_ERR_NONE == eRCode)
       return eQCWWAN_ERR_GENERAL;

    pr = strrchr(mPath, '/');   // find last '/' in path, everything after is folderId
    if (pr == NULL) pr = mPath;  // if no '/' found, treat entire string as folderId
    folderId = atol(pr + 1);
    *pr = '\0';   // removes folderId from mPath

    if ( folderId == 0)
    {
        syslog(LOG_DEBUG, "[%s] cannot convert last segment as integer", __func__);
        return eQCWWAN_ERR_GENERAL;
    }

    eRCode = SLQSSwiGetAllCarrierImages(
             &itemsCount,CarrierImages,mPath);

    if ((eRCode != eQCWWAN_ERR_NONE) || (itemsCount == 0))
    {
        //if there is no images, there is nothing to proceed further
        return eQCWWAN_ERR_GENERAL;
    }
    
    /* If the folder doesn't exist on host it use pseudo folder ID to switch from device */
    while(index < itemsCount)
    {
        if(CarrierImages[index].m_nFolderId == folderId)
        break;
        index++;
    }

    if (index == itemsCount)
    {
        //there is no matches
        return eQCWWAN_ERR_GENERAL;
    }

    /* initialize image id and build id of fw and pri before making the string copy */
    slmemset(fw_image_id, 0, sizeof(fw_image_id));
    slmemset(fw_build_id, 0, sizeof(fw_build_id)); 
    slmemset(pri_image_id, 0, sizeof(pri_image_id));
    slmemset(pri_build_id, 0, sizeof(pri_build_id));

    strcpy(fw_image_id,(CHAR*)CarrierImages[index].m_FwImageId);
    strcpy(fw_build_id,(CHAR*)CarrierImages[index].m_FwBuildId);
    strcpy(pri_image_id,(CHAR*)CarrierImages[index].m_PriImageId);
    strcpy(pri_build_id,(CHAR*)CarrierImages[index].m_PriBuildId);

    /* Set the image preferences on the device */
    BYTE  imageTypes[8]={0};
    ULONG imageTypesSize = sizeof(imageTypes);

    prefimglist.listSize = 2;
    strcpy((char*)prefimglist.listEntries[0].imageId, &fw_image_id[0]);
    prefimglist.listEntries[0].imageType = MDM_IMGTYPE;
    strcpy((char*)prefimglist.listEntries[0].buildId, &fw_build_id[0]);
    prefimglist.listEntries[0].buildIdLength = strlen(fw_build_id);
    strcpy((char*)prefimglist.listEntries[1].imageId, &pri_image_id[0]);
    prefimglist.listEntries[1].imageType = PRI_IMGTYPE;
    strcpy((char*)prefimglist.listEntries[1].buildId, &pri_build_id[0]);
    prefimglist.listEntries[1].buildIdLength = strlen(pri_build_id);

    if( eQCWWAN_ERR_NONE !=
            ( eRCode = SetImagesPreference( 2,
                                            (BYTE *)&prefimglist,
                                            bForceDownload, /* force download */
                                            0xFF, /* device manages modem index */
                                            &imageTypesSize,
                                            imageTypes) ) )
    {
        return eRCode;
    }

    /* Set bit mask depending on type of images required for download.
     * Read bit mask as follows:
     * 00 - No image requested by device
     * 01 - Modem Image requested by device
     * 10 - PRI Image requested by device
     * 11 - Both Modem and PRI requested by device */
    WORD  imageMask = 0;

    for( imageCtr = 0 ; imageCtr < imageTypesSize; imageCtr++ )
    {
      imageMask = imageMask | (imageTypes[imageCtr] + 1);
    }

    if ( 0 != imageMask )
    {
        /* set the firmware download image path */
        eRCode = SetImagePath( generic, (ULONG)TRUE, imageMask, 0 );
        if( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
    }

    /* reset the device to initiate the download */
    if ( eRCode == eQCWWAN_ERR_NONE )
    {
        eRCode = SetPower( DEVICE_SHUTDOWN );
        if( eQCWWAN_ERR_NONE != eRCode )
        {
            swi_osapilog("SetPower Fail");
        }
        eRCode = SetPower( DEVICE_RESET );
    }
    return eRCode;
}

local ULONG SetImagePathForPurpose(
    LPCSTR path, BYTE isFirehose )
{

    ULONG eRCode = eQCWWAN_ERR_NONE;
    ULONG genericLen;
    CHAR generic[IM_IMAGE_PATH_MAX_SZ]={0};

    slmemset( generic, 0, IM_IMAGE_PATH_MAX_SZ );
    slmemcpy( generic, (CHAR *)path, slstrlen(path) + 1);

    /* Account for '/' at end of file path */
    genericLen = slstrlen(generic);
    CHAR *pr = &generic[genericLen - 1];
    if( *pr != '/' )
      *(pr + 1) = '/';

    if (isFirehose)
        eRCode = SetImagePath( generic, (ULONG)FALSE, RESET_WITH_DM_CMD, 1 );
    else
        /* set imgMask as FORCE_DOWNLOAD_MODE to indicate requesting modem enters
           boot and hold mode through DM command */
        eRCode = SetImagePath( generic, (ULONG)FALSE, RESET_WITH_DM_CMD, 0 );

    if( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    return eRCode;
}



ULONG UpgradeFirmware2k(
    CHAR *pDestinationPath,
    ULONG bForceDownload )
{
    struct slqsfwinfo_s spkInfo;
    enum eQCWWANError eRCode;
    BYTE devicemode;

    /* validate input args for NULL only In case of Device Image switching we use Pseudo Path*/
    if(pDestinationPath == NULL)
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    eRCode = SLQSGetDeviceMode ((BYTE *)&devicemode);

    struct sGetDeviceSeriesResult result = {eGOBI_DEV_SERIES_UNKNOWN,eQCWWAN_ERR_ENUM_BEGIN};
    if ( devicemode != DEVICE_STATE_BOOT )
    {
        eGetDeviceSeries(&result);
        if( eQCWWAN_ERR_NONE != result.uResult)
        {
            return result.uResult;
        }
    }
    else
    {
        eGetDeviceSeries(&result);
    }

    if( (result.eDevice == eGOBI_DEV_SERIES_NON_GOBI) ||
        (( result.eDevice == eGOBI_DEV_SERIES_SIERRA_GOBI) && (SLQSIsSpkgFormatRequired())) ||        
        (devicemode == DEVICE_STATE_BOOT ) )
    {
        if(devicemode == DEVICE_STATE_BOOT )
        {
            /* Enable force download in boot mode, in boot mode all devices use upgrade_mc77xx_fw  */
            if( eQCWWAN_ERR_NONE != (eRCode = EnableForceDownload()) )
            {
                return eRCode;
            }
        }
        /* MC77xx */
        eRCode = upgrade_mc77xx_fw(pDestinationPath);
    }
    else if( result.eDevice == eGOBI_DEV_SERIES_9X30)
    {
        struct slqsfwinfo_s priInfo;
        struct slqsfwinfo_s fwInfo;
        CHAR generic[IM_IMAGE_PATH_MAX_SZ];
        ULONG genericLen;
        int priInfoAvail;

        slmemset( generic, 0, IM_IMAGE_PATH_MAX_SZ );
        slmemcpy( generic, (CHAR *)pDestinationPath, slstrlen(pDestinationPath) + 1);

        /* Account for '/' at end of file path */
        genericLen = slstrlen(generic);
        CHAR *pr = &generic[genericLen - 1];
        if( *pr != '/' )
            *(pr + 1) = '/';

        priInfoAvail = 
            (SLQSGetImageInfo_9x15(generic, (BYTE)IMG_PRI_TYPE_NVU, &priInfo)== eQCWWAN_ERR_NONE);

        if ( SLQSGetImageInfo_9x15(generic, (BYTE)IMG_FW_TYPE_SPK,  &spkInfo) == eQCWWAN_ERR_NONE )
        {
            syslog(LOG_DEBUG, "[%s] spk case", __func__);
            eRCode = upgrade_mc77xx_fw(pDestinationPath);
            if ( eQCWWAN_ERR_NONE != eRCode )
            {
                eRCode = SetImagePathForPurpose(pDestinationPath, 0);
            }
        }

        else if (
            ( !priInfoAvail) ^
            ( SLQSGetImageInfo_9x15(generic, (BYTE)IMG_FW_TYPE_CWE, &fwInfo)  != eQCWWAN_ERR_NONE  ))
        {
            syslog(LOG_DEBUG, "[%s] sku_str %s", __func__, priInfo.sku_str);
            if ( (priInfoAvail && (!strncmp(priInfo.sku_str, "9999999", 7)))
              || !priInfoAvail )
            {
                //Carrier PRI
                syslog(LOG_DEBUG, "[%s] carrier single cwe/nvu case", __func__);
                eRCode = upgrade_9x15_fw(pDestinationPath, bForceDownload);
            }
            else
            {
                //OEM PRI
                syslog(LOG_DEBUG, "[%s] oem single cwe/nvu case", __func__);
                eRCode = upgrade_mc77xx_fw(pDestinationPath);
                if ( eQCWWAN_ERR_NONE != eRCode )
                {
                    eRCode = SetImagePathForPurpose(pDestinationPath, 0);
                }
            }
        }
        else
        {
            syslog(LOG_DEBUG, "[%s] 9x30 device base image switching", __func__);
            eRCode = upgrade_9x30_fw(pDestinationPath, bForceDownload);
            if(eRCode!=eQCWWAN_ERR_NONE)
            {
                syslog(LOG_DEBUG, "[%s] fallback to 9x15 cwe/nvu pair or spk", __func__);
                eRCode = upgrade_9x15_fw(pDestinationPath, bForceDownload);
                if(eRCode!=eQCWWAN_ERR_NONE)
                {
                    syslog(LOG_DEBUG, "[%s] fallback to 77xx cwe or nvu", __func__);
                    eRCode = upgrade_mc77xx_fw(pDestinationPath);
                    if ( eQCWWAN_ERR_NONE != eRCode )
                    {
                        eRCode = SetImagePathForPurpose(pDestinationPath, 0);
                    }
                }
            }
        }
    }
    else if(result.eDevice == eGOBI_DEV_SERIES_9X15 )
    {
        if ( SLQSGetImageInfo_9x15(pDestinationPath, (BYTE)IMG_FW_TYPE_SPK,  &spkInfo) == eQCWWAN_ERR_NONE )
        {
            syslog(LOG_DEBUG, "[%s] spk case", __func__);
            eRCode = upgrade_mc77xx_fw(pDestinationPath);
            if ( eQCWWAN_ERR_NONE != eRCode )
            {
                eRCode = SetImagePathForPurpose(pDestinationPath, 0);
            }
        }
        else
        {
            /* 9x15 fw upgrade */
            eRCode = upgrade_9x15_fw(pDestinationPath, bForceDownload);

            /* If upgrade_9x15_fw fails, it could mean that device is not in
             * Gobi Image Management mode. Need to check if image upgrade is
             * possible using Non-IM mode before returning error code */
            if( eQCWWAN_ERR_NONE != eRCode )
            {
                eRCode = upgrade_mc77xx_fw(pDestinationPath);
            }
        }
    }
    else if (result.eDevice == eGOBI_DEV_SERIES_UNKNOWN) 
    {
        /* if device is unkown, probably modem stay at disconnect mode,
           try to force modem enters download modem through DM command */
        eRCode = SetImagePathForPurpose(pDestinationPath, 0);
    }
    else if (result.eDevice == eGOBI_DEV_SERIES_9X07_9X50) 
    {
        /* For 9x07 and 9x50 devices */
        eRCode = SetImagePathForPurpose(pDestinationPath, 1);
    }
    else
    {
        /* Gobi 3000 fw upgrade */
        eRCode = upgrade_gobi3k_fw(pDestinationPath);
    }

    return eRCode;
}

ULONG UpgradeFirmwareG3k(
    CHAR *pDestinationPath )
{
    enum eQCWWANError eRCode;
    BYTE devicemode;

    /* validate input args */
    if( (pDestinationPath == NULL) || (swi_osapi_stat(pDestinationPath) != 0) )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }
    eRCode = SLQSGetDeviceMode ((BYTE *)&devicemode);
    CHAR modelID[128];
    if ( devicemode != DCS_DEVICE_MODE_BOOT_READY )
    {
        if( eQCWWAN_ERR_NONE != (eRCode = GetModelID(sizeof(modelID), modelID)) )
        {
            return eRCode;
        }    
        /* Gobi 3000 fw upgrade */
        eRCode = upgrade_gobi3k_fw(pDestinationPath);
    }
    else {
        // G3K method is not supported for BOOT READY mode
        eRCode = eQCWWAN_ERR_QMI_DEVICE_NOT_READY;
    }
    return eRCode;        
}

ULONG GetImagesPreference(
    ULONG *pImageListSize,
    BYTE  *pImageList )
{
    ULONG  resultCode;  /* Result of SwiQmiSendnWait() */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiFmsGetImagesPreferenceResp response;

    /*
     * pImageListSize and pImagePref are OUT parameters and hence
     * should not be NULL
     */
    if ( !pImageListSize ||
         !pImageList )
        return eQCWWAN_ERR_INVALID_ARG;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message and sends the message. */
    resultCode = PkQmiFmsGetImagesPreference( &paramLength,
                                              pOutParam );

    if ( eQCWWAN_ERR_NONE != resultCode )
    {
        qmrelreqbkp();
        return resultCode; /* No Memory */
    }

    /* Prepare and send the blocking call */
    resultCode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_DMS,
                                  paramLength,
                                  eQMI_TIMEOUT_2_S, /* 2 Seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultCode )
    {
        /* Copy the obtained values to the function OUT parameters */
        response.pImageListSize = pImageListSize;
        slmemset( (char *)pImageList, 0, *pImageListSize );
        response.pImageList     = (struct PrefImageList *)pImageList;

        /* Copy to the caller's buffer */
        resultCode = UpkQmiFmsGetImagesPreference( pInParam, &response );
    }

    qmrelreqbkp();
    return resultCode;
}

ULONG SetImagesPreference(
    ULONG imageListSize,
    BYTE  *pImageList,
    ULONG bForceDownload,
    BYTE  modemIndex,
    ULONG *pImageTypesSize,
    BYTE  *pImageTypes )
{
    ULONG  resultCode;  /* Result of SwiQmiSendnWait() */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiFmsSetImagesPreferenceResp response;

    /* pImageTypesSize and pImageTypes are OUT parameter and hence
     * should not be NULL
     */
    if ( !pImageTypesSize ||
         !pImageTypes )
        return eQCWWAN_ERR_INVALID_ARG;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs the QMI message */
    resultCode = PkQmiFmsSetImagesPreference( &paramLength,
                                              pOutParam,
                                              imageListSize,
                                              pImageList,
                                              bForceDownload,
                                              modemIndex );
    if ( eQCWWAN_ERR_NONE != resultCode )
    {
        qmrelreqbkp();
        return resultCode; /* No Memory */
    }

    resultCode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_DMS,
                                  paramLength,
                                  eQMI_TIMEOUT_2_S, /* 2 Seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultCode )
    {
        /* Copy the obtained values to the function OUT parameters */
        response.pImageTypesSize = pImageTypesSize;
        response.pImageTypes     = pImageTypes;

        /* Copy to the caller's buffer */
        resultCode = UpkQmiFmsSetImagesPreference( pInParam, &response );
    }

    qmrelreqbkp();
    return resultCode;
}

ULONG GetStoredImages(
    ULONG *pImageListSize,
    BYTE  *pImageList )
{
    ULONG  resultCode;  /* Result of SwiQmiSendnWait() */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiFmsGetStoredImagesResp response;

    /* OUT parameters should not be NULL.*/
    if ( !pImageListSize ||
         !pImageList )
    {
         return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Pack QMI message */
    resultCode = PkQmiFmsGetStoredImages( &paramLength, pOutParam );

    if ( eQCWWAN_ERR_NONE != resultCode )
    {
        qmrelreqbkp();
        return resultCode; /* No memory */
    }

    /* Send QMI request */
    resultCode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_DMS,
                                  paramLength,
                                  eQMI_TIMEOUT_2_S, /* 2 seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( resultCode == eQCWWAN_ERR_NONE )
    {
        /* Intialize the buffer provided by user with 0 */
        slmemset( (char *)pImageList, 0, *( pImageListSize ) );

        /* Copy the obtained values to the function OUT parameters */
        response.pImagelistSize = pImageListSize;
        response.pImageList = (struct ImageList  *)pImageList;

        /* Copy to the caller's buffer */
        resultCode = UpkQmiFmsGetStoredImages( pInParam, &response );
    }

    qmrelreqbkp();
    return resultCode;
}

ULONG DeleteStoredImage(
    ULONG imageInfoSize,
    BYTE  *pImageInfo )
{
    ULONG  resultCode;  /* Result of SwiQmiSendnWait() */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiFmsDeleteStoredImageResp response;

    /*
     * imageInfoSize should be greater than 0 and pImageInfo should not be NULL
     */
    if ( !( 0 < imageInfoSize ) ||
         !pImageInfo )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs the QMI message */
    resultCode = PkQmiFmsDeleteStoredImage( &paramLength,
                                            pOutParam,
                                            pImageInfo );
    if ( eQCWWAN_ERR_NONE != resultCode )
    {
        qmrelreqbkp();
        return resultCode; /* No Memory */
    }

    resultCode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_DMS,
                                  paramLength,
                                  eQMI_TIMEOUT_2_S, /* 2 Seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultCode )
    {
        /* Copy to the caller's buffer */
        resultCode = UpkQmiFmsDeleteStoredImage( pInParam, &response );
    }
    qmrelreqbkp();
    return resultCode;
}

BOOL IsSierraGobiDevice()
{
    BYTE stringsize = 128;
    CHAR ModelId[stringsize];

    GetModelID( stringsize,
                ModelId );

    if  (strstr (ModelId, "SL9090") || strstr (ModelId, "MC9090") )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


ULONG SLQSGetBootVersionNumber(ULONG *bootversion)
{
    struct qmifwinfo_s imageInfo; 
    ULONG resultCode;
    CHAR *pch;
    BYTE tmp[4] = {0};
    BYTE count = 0;

    /* initialize the bootversion */
    *bootversion = 0;

    resultCode =  SLQSGetFirmwareInfoInternal( &imageInfo.dev.s );
    if ( resultCode != eQCWWAN_ERR_NONE)
    {
        return resultCode;     
    }

    /* locate the '_' which the boot version follow after the symbol, 
       e.g, the boot version of MC9090 looks like "SWI6600X_02.00.00.04" */
    pch = strchr(imageInfo.dev.s.bootversion_str, '_');
    if (pch != NULL)
    {
        /* the first number is after symbol '_', the starting location is pch+1 */
        pch = strtok(pch+1, ".");
        while (pch != NULL)
        { 
            /* convert the string to number */
            tmp[count] = atoi(pch);
            /* the delimiter of the minor version is  "." */
            pch = strtok (NULL, ".");
            if (strlen(pch) > 3)
            {
               /* the minor version is in two digit, so retrieve it from the last string */
               pch[2] = '\0';
               tmp[count+1] = atoi(pch);
               break;
            }
            count++;
        }
        *bootversion = tmp[0] << 24 | tmp[1] << 16 | tmp[2] << 8 | tmp[3];
    }

    return resultCode;
}

BOOL SLQSIsSpkgFormatRequired(void)
{
    BYTE stringsize = 128;
    CHAR ModelId[stringsize];
    ULONG bootversion;
    ULONG rc = FALSE;

    GetModelID( stringsize, ModelId );

    /* MC/SL9090 special handling */
    if ( strstr(ModelId, SL_MC_9090))
    {
        /* if it is SL9090 or MC9090, check the boot loader version to determine 
           which firmware format download method should be applied (G3K or SPKG) */
        rc = SLQSGetBootVersionNumber(&bootversion);
        if ( (rc == eQCWWAN_ERR_NONE) && (bootversion >= BOOT_VERSION_2_0_2_2) )
        {
            /* if boot version is equal or greater than SWI6600X_02.00.02.02, SPKG firmware download
               method should be used */
            rc = TRUE;
        }
        else
        {
            rc = FALSE;
        }
    }

    return rc;
}


void eGetDeviceSeries(struct sGetDeviceSeriesResult* result)
{
    CHAR ModelId[128] = {0};
    BYTE stringsize = sizeof(ModelId);
    result->uResult = eQCWWAN_ERR_ENUM_BEGIN;
    result->eDevice = eGOBI_DEV_SERIES_UNKNOWN;
    result->uResult = GetModelID( stringsize,
                ModelId );

    if (result->uResult != eQCWWAN_ERR_NONE)
    {
        fprintf(stderr, "[%s] Modle ID error\n" , __func__);
        return;
    }
    else
        fprintf(stderr, "[%s] Modle ID = %s\n", __func__, ModelId);    
        
    if  (strstr (ModelId, SL9090) || 
        strstr (ModelId, MC9090) )
    {
        result->eDevice = eGOBI_DEV_SERIES_SIERRA_GOBI;
    }else if( (strncmp(ModelId,"0",1)==0) && (strlen(ModelId)==1))
    {
        result->eDevice = eGOBI_DEV_SERIES_9X30;
    }
    else if ((strstr(ModelId, MC74XX)) ||
           (strstr(ModelId, EM74XX)))
    {
        result->eDevice = eGOBI_DEV_SERIES_9X30;
    }else if ((strstr(ModelId, WP_SERIES)) ||
        (strstr(ModelId, WP75XX)) ||
        (strstr(ModelId, GOBI5K_SERIES)) ||
        (strstr(ModelId, MC78_SERIES)) ||
        (strstr(ModelId, AR75_SERIES)) ||
        (strstr(ModelId, EM88_SERIES)) ||
        (strstr(ModelId, EM73XX)) )
    {
        result->eDevice = eGOBI_DEV_SERIES_9X15;
    }
    else if ( (strstr (ModelId, WP76XX)) ||
        (strstr(ModelId, EM75XX)) )
    {
        result->eDevice = eGOBI_DEV_SERIES_9X07_9X50;
    }    
    else if (strstr (ModelId, MC83XX))
    {
        result->eDevice = eGobi_DEV_SERIES_MC83;
    }
    else
    {
        if ( strstr(ModelId, MC_SERIES) || 
            strstr(ModelId, SL8092RD) )
        {
             result->eDevice = eGOBI_DEV_SERIES_NON_GOBI;
        }
        else 
        { 
            if (SLQSIsSpkgFormatRequired())
                result->eDevice = eGOBI_DEV_SERIES_NON_GOBI;
        }
    }

    return;
}

void extractID(CHAR *lfirmBuildId,CHAR *devFirmwareBuildId)
{
     while(*devFirmwareBuildId != 0)
     {
        if(*devFirmwareBuildId == '_')
            break;
        *lfirmBuildId = *devFirmwareBuildId;
        devFirmwareBuildId++;
        lfirmBuildId++;
    }
    *lfirmBuildId = 0;
}
ULONG getFolderId(ULONG *folderID, CHAR* carrierName)
{
    *folderID = 0;
    unsigned int i;
    for(i=0;i<(sizeof(folderDataMap)/sizeof(struct devFolderData));i++)
    {
        if(!strcasecmp(carrierName,folderDataMap[i].carrierName))
        {
            *folderID = folderDataMap[i].folderId;
            break;
        }
    }
    if ( i == (sizeof(folderDataMap)/sizeof(struct devFolderData)) )
        fprintf(stderr, "%s unregonized carrierName %s\n", __func__, carrierName);
    return 0;
 }

ULONG getCarrierId(ULONG *carrierID, CHAR* carrierName)
{
    *carrierID =0;
    unsigned int i;
    for(i=0;i<(sizeof(carrierDataMap)/sizeof(struct carrierData));i++)
    {
        if(!strcasecmp(carrierName,carrierDataMap[i].carrierName))
        {
            *carrierID = carrierDataMap[i].carrierId;
            break;
        }
    }
return 0;
}

ULONG SLQSSwiGetAllCarrierImages(
               ULONG *pNumOfItems,
               struct SWI_STRUCT_CarrierImage *pCarrierImages,
               CHAR  *pFolderPath)
{
    struct qmifwinfo_s    fwInfo;
    struct slqsfwinfo_s   slqsfwinfo;
    BYTE                  imgType;
    DIR                   *dirp = NULL;
    ULONG                 carrierId = 0 ;
    ULONG                 folderId = 0;
    struct dirent         *entry;
    CHAR                  newpath[MAX_IMAGE_PATH];
    ULONG                 dir_count = 0;
    ULONG                 numOfItems = 0;
    /* Parameters for GetStoredImages */
    struct ImageList      imageList, *pImageList = NULL;
    ULONG                 imageListsize = 4308;
    ULONG                 nRet;
    BYTE                  idx1 = 0, idx2 = 0;
    BYTE                  imageIdEntries   = 0;
    CHAR                  *pch;
    CHAR                  carrierName[20];

    ULONG firmwareBuildCount = 0,priBuildCount = 0 ;
    /* validate input args */
    if( (pCarrierImages == NULL) || (pNumOfItems == NULL))
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    if ( *pNumOfItems == 0 )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /*Clear the Global static variables used*/
    memset(devFirmwareBuildId, 0, sizeof(devFirmwareBuildId));
    memset(devPriBuildId,      0, sizeof(devPriBuildId));
    memset(devFirmwareImageId, 0, sizeof(devFirmwareImageId));
    memset(devPriImageId,      0, sizeof(devPriImageId));
    
    if (pFolderPath != NULL)
    {
        dirp = opendir((CHAR*)pFolderPath);
    }

    if(NULL != dirp)
    {
        /*Reading directories */
        while ((entry = readdir(dirp)) != NULL)
        {
            if (entry->d_type == DT_DIR &&  (entry->d_name[0]!='.'))
            {
            /* If the entry is a directory*/
                memset(newpath, 0, sizeof(newpath));
                strcpy(newpath,(CHAR*)pFolderPath);                
                strcat(newpath, "/");
                strcat(newpath,entry->d_name);
                strcat(newpath,"/");
                /*Checking for all type of images CWE, NVU & SPK*/
                for(imgType=2;imgType<=4;imgType++)
                {
                    memset(&fwInfo, 0, sizeof(fwInfo));
                    memset(&slqsfwinfo, 0 , sizeof(slqsfwinfo));

                    /* Get the information about the image located at specified path */
                    nRet = SLQSGetImageInfoInternal(newpath,imgType,&slqsfwinfo);

                    if(!nRet)
                    {
                        if (*pNumOfItems < numOfItems )
                        {
                            *pNumOfItems = numOfItems;
                            return eQCWWAN_ERR_BUFFER_SZ;
                        }

                        /* image preference list construction object */
                        struct PrefImageList prefimglist;
                        slmemset( (CHAR *)&prefimglist, 0, sizeof(struct PrefImageList));

                        /* image ID(unique ID) : priver_pkgver
                         * build ID string as  : fwversion_carrier */
                        update_pref_img_list(&prefimglist, 0 /*index*/, PRI_IMGTYPE,
                        slqsfwinfo.priversion_str, slqsfwinfo.packageid_str,
                        slqsfwinfo.appversion_str, slqsfwinfo.carrier_str);

                        if(imgType == IMG_PRI_TYPE_NVU)
                        {
                           strcpy((char*)pCarrierImages->m_PriImageId,(char*)prefimglist.listEntries[0].imageId);
                           strncpy((char*)pCarrierImages->m_PriBuildId,
                                   prefimglist.listEntries[0].buildId,
                                   sizeof(pCarrierImages->m_PriBuildId));
                           getCarrierId(&carrierId, slqsfwinfo.carrier_str);
                        }

                        if(imgType == IMG_FW_TYPE_CWE)
                        {
                             strcpy((char*)pCarrierImages->m_FwImageId,(char*)prefimglist.listEntries[0].imageId);
                             strncpy((char*)pCarrierImages->m_FwBuildId,
                                     prefimglist.listEntries[0].buildId,
                                     sizeof(pCarrierImages->m_FwBuildId));
                        }

                        if(imgType == IMG_FW_TYPE_SPK)
                        {
                            strcpy((char*)pCarrierImages->m_PriImageId,(char*)prefimglist.listEntries[0].imageId);
                            strncpy((char*)pCarrierImages->m_PriBuildId,
                                    prefimglist.listEntries[0].buildId,
                                    sizeof(pCarrierImages->m_PriBuildId));
                            getCarrierId(&carrierId, slqsfwinfo.carrier_str);

                            /* construct second entry of the image preference list */
                            /* image ID(unique ID) : priver_pkgver
                            * build ID string as  : fwversion_carrier */
                            update_pref_img_list(&prefimglist, 1 /*index*/, MDM_IMGTYPE,
                                    "?", "?",
                                    slqsfwinfo.appversion_str, "?");

                            strcpy((char*)pCarrierImages->m_FwImageId,(char*)prefimglist.listEntries[1].imageId);
                            strncpy((char*)pCarrierImages->m_FwBuildId,
                                    prefimglist.listEntries[1].buildId,
                                    sizeof(pCarrierImages->m_FwBuildId));
                        }
                    }
                }

                if (*pNumOfItems < numOfItems )
                {
                    *pNumOfItems = numOfItems;
                    return eQCWWAN_ERR_BUFFER_SZ;
                }
                pCarrierImages->m_nFolderId = atol(entry->d_name);
                pCarrierImages->m_nStorage = HOST;
                pCarrierImages->m_nCarrierId = carrierId;

                numOfItems++;
                pCarrierImages++;
                dir_count++;
            }
        }
    }
    /*Get Stored Images from device*/
    /* Call the API */
    pImageList = &imageList;
    memset(&imageList, 0, sizeof(imageList));
    nRet =  GetStoredImages( &imageListsize,
                             (BYTE *)pImageList );
    if ( !nRet )
    {
        for( idx1 = 0; idx1 < pImageList->listSize; idx1++ )
        {
            imageIdEntries = pImageList->imageIDEntries[idx1].imageIDSize ;

            for ( idx2 = 0; idx2 < imageIdEntries; idx2++ )
            {
                struct ImageIdElement *pImageIdElement = NULL;
                pImageIdElement = ( struct ImageIdElement *)
                                   &pImageList->imageIDEntries[idx1].imageIDElement[idx2];
               /* Copy Firmware Image info */
                if(0 == pImageList->imageIDEntries[idx1].imageType )
                {
                    strncpy(devFirmwareBuildId[firmwareBuildCount],
                            pImageIdElement->buildID,
                            sizeof(devFirmwareBuildId[firmwareBuildCount]));
                    strcpy((CHAR*)devFirmwareImageId[firmwareBuildCount], (CHAR*)pImageIdElement->imageID);
                    firmwareBuildCount++;
                }

                if(1 == pImageList->imageIDEntries[idx1].imageType )
                {
                    strncpy(devPriBuildId[priBuildCount],
                            pImageIdElement->buildID,
                            sizeof(devPriBuildId[priBuildCount]));
                    strcpy((CHAR*)devPriImageId[priBuildCount],(CHAR*)pImageIdElement->imageID);
                    priBuildCount++;
                }
            }   //end idx2
        }  //end idx1
    } // end nRet

    ULONG i,j;
    CHAR lFirmBuildId[32];
    CHAR lPriBuildId[32];
    for(i=0;i< firmwareBuildCount;i++)
    {
        extractID(lFirmBuildId, devFirmwareBuildId[i]);
        for(j=0; j< priBuildCount;j++)
        {
            extractID(lPriBuildId,devPriBuildId[j]);
            if(!strcmp(lFirmBuildId,lPriBuildId ))
            {
                if (*pNumOfItems < numOfItems )
                {
                    *pNumOfItems = numOfItems;
                    return eQCWWAN_ERR_BUFFER_SZ;
                }
                pCarrierImages->m_nStorage = DEVICE;
                strcpy((CHAR*)pCarrierImages->m_FwImageId,(CHAR*)devFirmwareImageId[i]);
                strcpy((CHAR*)pCarrierImages->m_FwBuildId,devFirmwareBuildId[i]);
                strcpy((CHAR*)pCarrierImages->m_PriImageId,(CHAR*)devPriImageId[j]);
                strcpy((CHAR*)pCarrierImages->m_PriBuildId,devPriBuildId[j]);
                /*extract carrier name */
                pch = strchr(devPriBuildId[j], '_');
                if (pch != NULL)
                {
                    /* the first number is after symbol '_', the starting location is pch+1 */
                    strcpy(carrierName, pch+1);
                    getCarrierId(&carrierId, carrierName);
                    getFolderId(&folderId, carrierName);
                }

                pCarrierImages->m_nCarrierId = carrierId;
                pCarrierImages->m_nFolderId = folderId;
                numOfItems++;
                pCarrierImages++;
            }
        }
    }

    *pNumOfItems = numOfItems;

    return 0;
}
