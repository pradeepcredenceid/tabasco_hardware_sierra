/**
 * \ingroup dcs
 *
 * \file    qaGobiApiDcs.c
 *
 * \brief   Entry points for Gobi APIs for the Device Connectivity Service (DCS)
 *
 * Copyright: © 2011-2013 Sierra Wireless, Inc. all rights reserved
 *
 */
#include <string.h>  
#include "../../SWIWWANCMAPI.h"
#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "amudefs.h"
#include "sludefs.h"
#include "swi_osapi.h"
#include "swi_ossdk.h"
#include "qaQmiBasic.h"
#include "qaGobiApiDcs.h"
#include "qaDcsQCWWAN2kEnumerateDevices.h"
#include "qaDcsQCWWAN2kConnect.h"
#include "qaDcsQCWWANDisconnect.h"
#include "qaDcsQCWWAN2kGetConnectedDeviceID.h"
#include "qaDcsSLQSGetUsbPortNames.h"
#include "qaDcsSLQSGetDeviceMode.h"
#include "qaQmiNotify.h"
#include "qaDcsSLQSStartSrv.h"
#include "qaDcsSLQSQosMap.h"
#include "qaDcsSLQSQosUnmap.h"
#include "qaDcsSLQSQosClearMap.h"
#include "qaDcsSLQSQosEditMap.h"
#include "qaDcsSLQSQosReadMap.h"
#include "qaDcsSLQSQosDumpMap.h"

/*
 * Fully qualified path of SLQS executable (includes the executable file’s name)
 */
static LPCSTR pSDKpath = NULL;  /* Note: cannot initialize to default here */

#ifdef AGENT_AUTO_START
static char pRApath[500];
#endif

ULONG SLQSKillSDKProcess()
{
    BYTE    *pInParam;      /* ptr to param field rx'd from modem */
    BYTE    *pReqBuf;       /* Pointer to outgoing request buffer */
    USHORT  ParamLength;    /* Ret'd length of the Param field */
    enum eQCWWANError  eRCode;  /* Result of SwiSdkSendnWait() */

    /* Initialize the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* send the request to the SDK process */
    eRCode = SwiSdkSendnWait( pReqBuf,
                              AMSDKREQKILL,
                              ParamLength,
                              eQMI_TIMEOUT_2_S, /* 2 Seconds */
                              &pInParam,
                              &ParamLength );

    /* There is nothing to check - the SDK is gone by this time */
    qmrelreqbkp();
    eRCode = eQCWWAN_ERR_NONE;
    return eRCode;
}

#ifdef AGENT_AUTO_START
static ULONG StartRA(const char *pathp)
{
    char *mypath;
    mypath = (char *)malloc(strlen(pathp)+50);
    memset(mypath,0,sizeof(mypath));
    strcpy(mypath,"cd ");
    strcat(mypath , pathp);
    strcat(mypath, " && bin/agent | logger -t Mihini &");
    /*fprintf(stderr, "RA path = %s \n",mypath);*/
    swi_osapi_system(mypath);		/*start readyagent in new process*/
    free(mypath);
    return TRUE;
}

static ULONG StartRA_fw_update(const char *pathp)
{
    char *mypath;
    mypath = (char *)malloc(strlen(pathp)+50);
    memset(mypath,0,sizeof(mypath));
    strcpy(mypath , pathp);
    strcat(mypath, "/bin/slqs_fw_update&");
    /*fprintf(stderr, "background process = %s \n",mypath);*/
    swi_osapi_system(mypath);
    free(mypath);
    return TRUE;
}

static ULONG CheckAVADefaultPath(const char *pathp)
{
	DIR* dir;
    char *mypath;
    char *replacestr;
    const char SLQSSDK_NAME[]="/slqssdk";
    const char AVA_EXEC[]="/AirVantageAgent/runtime";

    mypath = (char *)malloc(strlen(pathp)+20);
    memset(mypath,0,sizeof(mypath));
    strcpy(mypath,pathp);

    replacestr=strstr(mypath,SLQSSDK_NAME);
    strcpy(replacestr, AVA_EXEC);

    /* fprintf(stderr, "%s\n",mypath);*/
    dir = opendir(mypath);

    if (dir)
    {
        /* Directory exists. */
        closedir(dir);
        /* fprintf(stderr, "exists \n"); */

        /* update AVA path */
        strcpy(pRApath,mypath);
        /* fprintf(stderr, "AVA_PATH=%s \n",pRApath); */

        free(mypath);
        return TRUE;
    }

    /* fprintf(stderr, "not exists \n"); */
    free(mypath);
    return FALSE;
}

static ULONG CheckAVAConfig(const char *pathp)
{
    FILE  *fp = NULL;
    char *mypath;
    char *replacestr;
    USHORT i;
    ULONG ret = FALSE;
    DIR* dir;
    char line [500];
    const char AVA_PATH_TAG[]="AVA_PATH=";
    const char AVA_CONFIG_FILE[]="/.sdk_config";
    const char DISABLE_TAG[]="NO_AVA";
    const char SLQSSDK_NAME[]="/slqssdk";

    mypath = (char *)malloc(strlen(pathp)+20);
    memset(mypath,0,sizeof(mypath));
    strcpy(mypath,pathp);
    replacestr=strstr(mypath,SLQSSDK_NAME);
    strcpy(replacestr, AVA_CONFIG_FILE);
    fp = fopen(mypath, "r");

    if(fp)
    {
        if(fgets (line, sizeof(line), fp ) != NULL)
        {
            /* remove carrage return & linefeed */
            for(i=0;i<sizeof(line)+1;i++)
                if(line[i]==0x0D || line[i]==0x0A)
                    line[i]=0x0;

            /* find the tag */
            replacestr = strstr(line,AVA_PATH_TAG);
            if(replacestr)
            {
                /* read path config */
                strcpy(pRApath,replacestr+sizeof(AVA_PATH_TAG)-1);
                /* fprintf(stderr, "AVA_PATH=%s \n",pRApath); */

                /* no ava */
                if(strstr(pRApath,DISABLE_TAG))
                {
                    *pRApath = 0;
                    /* fprintf(stderr, "AirVantage Agent disabled by user\n"); */
                    ret = TRUE;
                }
                else
                {
                    /* is AVA runtime folder exists*/
                    dir = opendir(pRApath);
                    if (dir)
                    {
                        /* Directory exists. */
                        closedir(dir);
                        ret = TRUE;
                    }
                    else
                    {
                        *pRApath = 0;
                        /* fprintf(stderr, "AirVantage Agent folder invalid \n"); */
                        ret = FALSE;
                    }
                }
            }
            else
            {
                /* no path tag, invalid config file */
                ret = FALSE;
            }
        }
        fclose(fp);
    }
    free(mypath);
    return ret;
}

static int isAVAgentRunning(void)
{
    int status;
    /* status = 0 if pidof cmd succeed => agent is running */
    /* status = 1 if agent is not running or pidof cmd not found */
    status = system("pidof agent > /dev/null");
    return status;
}

#endif

ULONG SLQSStart_AVAgent()
{

#ifdef AGENT_AUTO_START
    static BOOL baminitapicalled = FALSE;

    /* SDK started. Initialize the IPC channels, calling it once only */
    if (baminitapicalled == FALSE)
    {
        if (!aminitapi())
        {
            /* Unable to reserve an IPC channel for this Application to use
             * for communicating with the SDK process.
             */
            return eQCWWAN_ERR_SWICM_SOCKET_IN_USE;
        }
        baminitapicalled = TRUE;
    }

    /* Initialize the QA Package */
    qaInit();
    return eQCWWAN_ERR_NONE;
#else
    return SLQSStart();
#endif

}


ULONG SLQSStart()
{
    enum eQCWWANError eRCode;
    static BOOL baminitapicalled = FALSE;

    /* If caller never called SetSDKImagePath, set it to default here */
    if (pSDKpath == NULL)
        pSDKpath = SDKPATHDEFAULT;

#ifdef AGENT_AUTO_START
    if (isAVAgentRunning())
    {
        /*fprintf(stderr, "AVA not started \n");*/
        /* check AVA config file before start SDK*/
        if(CheckAVAConfig((const char *)pSDKpath ))
        {
            /* config existes and valid */
            /*fprintf(stderr, "Start AirVantage Agent Service with conifg path\n");*/
        }
        else
        {
            /* check AVAgent in SDK path */
            if(!CheckAVADefaultPath(pSDKpath))
            {
                fprintf(stderr, "SDK cannot start without AirVantage Agent Service. Please check SDK Application developer guide.\n");
                eRCode =  eQCWWAN_ERR_INTERNAL;
                return eRCode;
            }
        }
    }
    else
        /* Agent have already started */
        *pRApath = 0;

    /* Start up RA */
    if (*pRApath)
    {
        /*fprintf(stderr, "AirVantage Agent going to start! \n");*/
        StartRA(pRApath);
        StartRA_fw_update(pRApath);
    }
#endif

    /* Start up the SDK */
     if( swi_osapisdkprocesscreate( (const char *)pSDKpath ) )
     {
         eRCode = eQCWWAN_ERR_NONE;
     }
     else
     {
         eRCode =  eQCWWAN_ERR_INTERNAL;
         return eRCode;
     }

    /* SDK started. Initialize the IPC channels, calling it once only */
    if (baminitapicalled == FALSE)
    {
        if (!aminitapi())
        {
            /* Unable to reserve an IPC channel for this Application to use
             * for communicating with the SDK process.
             */
            fprintf(stderr, "Too many Application connected to SDK\n");
            return eQCWWAN_ERR_SWICM_SOCKET_IN_USE;
        }
        baminitapicalled = TRUE;
    }

    /* Initialize the QA Package */
    qaInit();
    return eQCWWAN_ERR_NONE;
}

ULONG QCWWAN2kEnumerateDevices(
    BYTE    *pDevicesSize,
    BYTE    *pDevices )
{
    BYTE    *pInParam;      /* ptr to param field rx'd from modem */
    BYTE    *pOutParam;     /* ptr to outbound param field */
    BYTE    *pReqBuf;       /* Pointer to outgoing request buffer */
    USHORT  ParamLength;    /* Ret'd length of the Param field */
    enum eQCWWANError  eRCode;  /* Result of SwiQmiSendnWait() */
    struct qm_qmi_response_tlvs_values response;

    if( pDevicesSize == NULL || *pDevicesSize == 0 )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Pack the QMI request */
    eRCode = PkQmiDcsQCWWAN2kEnumerateDevices( &ParamLength, pOutParam );

    if (eRCode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return eRCode;
    }

    /* send the request to the SDK process */
    eRCode = SwiQmiSendnWait( pReqBuf,
                              eQMI_SVC_DCS,
                              ParamLength,
                              eQMI_TIMEOUT_5_S, /* 5 Seconds */
                              &pInParam,
                              &ParamLength );

    if (eRCode == eQCWWAN_ERR_NONE)
    {
        /* Set response with "0" to avoid garbage output */
        memset( &response, 0, sizeof(response) );

        /* extract response data */
        eRCode = UpkQmiDcsQCWWAN2kEnumerateDevices( pInParam,
                                                    &response );

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

        /* Check if both the TLVs were present */
        if ( !response.tlvvalues.qmdcstlvs.enumerate.dev.devnodestrsize ||
             !response.tlvvalues.qmdcstlvs.enumerate.dev.devkeystrsize )
        {
            qmrelreqbkp();
            return eQCWWAN_ERR_INVALID_QMI_RSP;
        }

        /* device found */
        *pDevicesSize = 1;

        /* copy device info to caller's buffers */
        slmemcpy( ((struct device_info_param *)pDevices)->deviceNode,
                  response.tlvvalues.qmdcstlvs.enumerate.dev.devnode,
                  response.tlvvalues.qmdcstlvs.enumerate.dev.devnodestrsize );

        slmemcpy( ((struct device_info_param *)pDevices)->deviceKey,
                  response.tlvvalues.qmdcstlvs.enumerate.dev.devkey,
                  response.tlvvalues.qmdcstlvs.enumerate.dev.devkeystrsize );
    }
    qmrelreqbkp();
    return eRCode;
}

ULONG  QCWWAN2kConnect(
    CHAR    *pDeviceID,
    CHAR    *pDeviceKey )
{
    BYTE              *pInParam;    /* ptr to param field rx'd from modem */
    BYTE              *pOutParam;   /* ptr to outbound param field */
    BYTE              *pReqBuf;     /* Pointer to outgoing request buffer */
    USHORT             ParamLength; /* Ret'd length of the Param field */
    enum eQCWWANError  eRCode;      /* Result of SwiQmiSendnWait() */

    struct qm_qmi_response_tlvs_values response;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message and sends the message. */
    eRCode = PkQmiDcsQCWWAN2kConnect( &ParamLength,
                                       pOutParam,
                                       pDeviceID,
                                       pDeviceKey);

    if (eRCode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return eRCode;
    }

    /* send the request to the SDK process */
    eRCode = SwiQmiSendnWait( pReqBuf,
                              eQMI_SVC_DCS,
                              ParamLength,
                              eQMI_TIMEOUT_2_S, /* 2 Seconds */
                              &pInParam,
                              &ParamLength );

    if (eRCode == eQCWWAN_ERR_NONE)
    {
        /* unpack QMI response */
        eRCode = qm_dcs_connect_response_unpack( pInParam, &response );

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
    else
    {
        qmrelreqbkp();
        return eRCode;
    }

    /* Device available */
    qmrelreqbkp();
    return eRCode;
}

ULONG QCWWANDisconnect()
{
    BYTE              *pInParam;    /* ptr to param field rx'd from modem */
    BYTE              *pOutParam;   /* ptr to outbound param field */
    BYTE              *pReqBuf;     /* Pointer to outgoing request buffer */
    USHORT             ParamLength; /* Ret'd length of the Param field */
    enum eQCWWANError  eRCode;      /* Result of SwiQmiSendnWait() */

    struct QmiDcsQCWWANDisconnectResp response;

    /* Deregister all callbacks */
    qaQmiRemoveAllCallbacks();

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message and sends the message. */
    eRCode = PkQmiDcsQCWWANDisconnect( &ParamLength,
                                       pOutParam);

    if (eRCode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return eRCode;
    }

     /* send the request to the SDK process */
     eRCode = SwiQmiSendnWait( pReqBuf,
                              eQMI_SVC_DCS,
                              ParamLength,
                              eQMI_TIMEOUT_2_S, /* 2 Seconds */
                              &pInParam,
                              &ParamLength );

    if( eRCode == eQCWWAN_ERR_NONE )
    {
        /* Copy to the caller's buffer */
        eRCode = UpkQmiDcsQCWWANDisconnect( pInParam, &response );

        /* check for unpack error */
        if( eRCode != eQCWWAN_ERR_NONE )
        {
            qmrelreqbkp();
            return eRCode;
        }

        /* Check that the mandatory response TLV was received */
        if( !response.results.TlvResultCode )
        {
            qmrelreqbkp();
            return eQCWWAN_ERR_INVALID_QMI_RSP;
        }

        /* check mandatory response tlv result code */
        if( response.results.DeviceResult != eQCWWAN_ERR_NONE )
        {
            qmrelreqbkp();
            return response.results.DeviceError;
        }
    }
    qmrelreqbkp();
    return eRCode;
}

ULONG QCWWAN2kGetConnectedDeviceID(
    ULONG   deviceIDSize,
    CHAR    *pDeviceID,
    ULONG   deviceKeySize,
    CHAR    *pDeviceKey )
{
    BYTE              *pInParam;    /* ptr to param field rx'd from modem */
    BYTE              *pOutParam;   /* ptr to outbound param field */
    BYTE              *pReqBuf;     /* Pointer to outgoing request buffer */
    USHORT             ParamLength; /* Ret'd length of the Param field */
    enum eQCWWANError  eRCode;      /* Result of SwiQmiSendnWait() */

    struct qm_qmi_response_tlvs_values response;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message and sends the message. */
    eRCode = PkQmiDcsQCWWAN2kGetConnectedDeviceID( &ParamLength,
                                                   pOutParam);

    if (eRCode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return eRCode;
    }

    /* send the request to the SDK process */
    eRCode = SwiQmiSendnWait( pReqBuf,
                              eQMI_SVC_DCS,
                              ParamLength,
                              eQMI_TIMEOUT_2_S, /* 2 Seconds */
                              &pInParam,
                              &ParamLength );

    if( eRCode == eQCWWAN_ERR_NONE )
    {
        /* unpack response from SDK */
        eRCode = UpkQmiDcsQCWWAN2kGetConnectedDeviceID( pInParam,
                                                        &response );

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

        /* check that client buffers are large enough for response data */
        if( deviceIDSize < response.tlvvalues.qmdcstlvs.enumerate.dev.devnodestrsize ||
            deviceKeySize < response.tlvvalues.qmdcstlvs.enumerate.dev.devkeystrsize )
        {
            qmrelreqbkp();
            return eQCWWAN_ERR_BUFFER_SZ;
        }

        /* Copy to the caller's buffers */
        slmemcpy( pDeviceID,
                  response.tlvvalues.qmdcstlvs.enumerate.dev.devnode,
                  response.tlvvalues.qmdcstlvs.enumerate.dev.devnodestrsize );

        slmemcpy( pDeviceKey,
                  response.tlvvalues.qmdcstlvs.enumerate.dev.devkey,
                  response.tlvvalues.qmdcstlvs.enumerate.dev.devkeystrsize );
    }
    qmrelreqbkp();
    return eRCode;
}

ULONG SetSDKImagePath (
    LPCSTR pPath )
{
    /* Set the executable path to default if not provided */
    if(pPath == NULL)
        pSDKpath = SDKPATHDEFAULT;
    else
        pSDKpath = pPath;

    return eQCWWAN_ERR_NONE;
}

ULONG  SLQSGetUsbPortNames(
    struct DcsUsbPortNames *pUsbPortNames )
{
    BYTE              *pInParam;    /* ptr to param field rx'd from modem */
    BYTE              *pOutParam;   /* ptr to outbound param field */
    BYTE              *pReqBuf;     /* Pointer to outgoing request buffer */
    USHORT             ParamLength; /* Ret'd length of the Param field */
    enum eQCWWANError  eRCode;      /* Result of SwiQmiSendnWait() */

    struct qm_qmi_response_tlvs_values response;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message and sends the message. */
    eRCode = PkQmiDcsSLQSGetUsbPortNames ( &ParamLength,
                                           pOutParam );
    if (eRCode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return eRCode;
    }

    /* send the request to the SDK process */
    eRCode = SwiQmiSendnWait( pReqBuf,
                              eQMI_SVC_DCS,
                              ParamLength,
                              eQMI_TIMEOUT_2_S, /* 2 Seconds */
                              &pInParam,
                              &ParamLength );

    if (eRCode == eQCWWAN_ERR_NONE)
    {
        /* unpack QMI response */
        slmemset ( (char *)&response,
                   EOS,
                   sizeof (struct qm_qmi_response_tlvs_values));

        eRCode = UpkQmiDcsSLQSGetUsbPortNames ( pInParam, &response );

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
    else
    {
        qmrelreqbkp();
        return eRCode;
    }

    /* copy Port Names to caller's buffers */
    slmemcpy( pUsbPortNames->AtCmdPort,
              response.tlvvalues.qmdcstlvs.portnames.usbportnames.AtCmdPort,
              strlen(response.tlvvalues.qmdcstlvs.portnames.usbportnames.AtCmdPort) );

    slmemcpy( pUsbPortNames->NmeaPort,
              response.tlvvalues.qmdcstlvs.portnames.usbportnames.NmeaPort,
              strlen(response.tlvvalues.qmdcstlvs.portnames.usbportnames.NmeaPort) );

    slmemcpy( pUsbPortNames->DmPort,
              response.tlvvalues.qmdcstlvs.portnames.usbportnames.DmPort,
              strlen(response.tlvvalues.qmdcstlvs.portnames.usbportnames.DmPort) );

    qmrelreqbkp();
    return eRCode;
}

ULONG  SLQSGetDeviceMode(
    BYTE *pDeviceMode )
{
    BYTE              *pInParam;    /* ptr to param field rx'd from modem */
    BYTE              *pOutParam;   /* ptr to outbound param field */
    BYTE              *pReqBuf;     /* Pointer to outgoing request buffer */
    USHORT             ParamLength; /* Ret'd length of the Param field */
    enum eQCWWANError  eRCode;      /* Result of SwiQmiSendnWait() */

    struct qm_qmi_response_tlvs_values response;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message and sends the message. */
    eRCode = PkQmiDcsSLQSGetDeviceMode ( &ParamLength,
                                           pOutParam );
    if (eRCode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return eRCode;
    }

    /* send the request to the SDK process */
    eRCode = SwiQmiSendnWait( pReqBuf,
                              eQMI_SVC_DCS,
                              ParamLength,
                              eQMI_TIMEOUT_2_S, /* 2 Seconds */
                              &pInParam,
                              &ParamLength );

    if (eRCode == eQCWWAN_ERR_NONE)
    {
        /* unpack QMI response */
        eRCode = UpkQmiDcsSLQSGetDeviceMode ( pInParam, &response );

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
    else
    {
        qmrelreqbkp();
        return eRCode;
    }

    *pDeviceMode = response.tlvvalues.qmdcstlvs.devicestate.devstate;

    qmrelreqbkp();
    return eRCode;
}

/* Deprecated APIs */
ULONG  QCWWANEnumerateDevices(
    BYTE    *pDevicesSize,
    BYTE    *pDevices )
{
    return QCWWAN2kEnumerateDevices( pDevicesSize, pDevices );
}

ULONG  QCWWANConnect(
    CHAR *pDeviceID,
    CHAR *pDeviceKey)
{
    /* Call the newer API */
    return ( QCWWAN2kConnect( pDeviceID, pDeviceKey ) );
}

ULONG SLQSStartSrv( BYTE action, BYTE mask )
{
    BYTE              *pInParam;    /* ptr to param field rx'd from modem */
    BYTE              *pOutParam;   /* ptr to outbound param field */
    BYTE              *pReqBuf;     /* Pointer to outgoing request buffer */
    USHORT             ParamLength; /* Ret'd length of the Param field */
    enum eQCWWANError  eRCode;      /* Result of SwiQmiSendnWait() */

    struct qm_qmi_response_tlvs_values response;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message and sends the message. */
    eRCode = PkQmiDcsSLQSStartSrv ( &ParamLength,
                                    pOutParam,
                                    action,
                                    mask );
    if (eRCode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return eRCode;
    }

    /* send the request to the SDK process */
    eRCode = SwiQmiSendnWait( pReqBuf,
                              eQMI_SVC_DCS,
                              ParamLength,
                              eQMI_TIMEOUT_2_S, /* 2 Seconds */
                              &pInParam,
                              &ParamLength );

    if (eRCode == eQCWWAN_ERR_NONE)
    {
        /* unpack QMI response */
        eRCode = UpkQmiDcsSLQSStartSrv ( pInParam, &response );

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
    else
    {
        qmrelreqbkp();
        return eRCode;
    }

    qmrelreqbkp();
    return eRCode;
}

ULONG SLQSQosMap(
        BYTE    instance,
        BYTE    dscp,
        ULONG   qos_id
        )
{
    BYTE              *pInParam;    /* ptr to param field rx'd from modem */
    BYTE              *pOutParam;   /* ptr to outbound param field */
    BYTE              *pReqBuf;     /* Pointer to outgoing request buffer */
    USHORT             ParamLength; /* Ret'd length of the Param field */
    enum eQCWWANError  eRCode;      /* Result of SwiQmiSendnWait() */

    struct qm_qmi_response_tlvs_values response;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message and sends the message. */
    eRCode = PkQmiDcsSLQSQosMap ( &ParamLength,
                                    pOutParam,
                                    dscp,
                                    qos_id );
    if (eRCode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return eRCode;
    }

    /* send the request to the SDK process */
    eRCode = SwiQmiMISendnWait( pReqBuf,
                              eQMI_SVC_DCS,
                              ParamLength,
                              eQMI_TIMEOUT_2_S, /* 2 Seconds */
                              &pInParam,
                              &ParamLength,
                              instance );

    if (eRCode == eQCWWAN_ERR_NONE)
    {
        /* unpack QMI response */
        eRCode = UpkQmiDcsSLQSQosMap ( pInParam, &response );

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
    else
    {
        qmrelreqbkp();
        return eRCode;
    }

    qmrelreqbkp();
    return eRCode;
}

ULONG SLQSQosUnmap(
        BYTE    instance,
        BYTE    dscp
        )
{
    BYTE              *pInParam;    /* ptr to param field rx'd from modem */
    BYTE              *pOutParam;   /* ptr to outbound param field */
    BYTE              *pReqBuf;     /* Pointer to outgoing request buffer */
    USHORT             ParamLength; /* Ret'd length of the Param field */
    enum eQCWWANError  eRCode;      /* Result of SwiQmiSendnWait() */

    struct qm_qmi_response_tlvs_values response;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message and sends the message. */
    eRCode = PkQmiDcsSLQSQosUnmap ( &ParamLength,
                                    pOutParam,
                                    dscp);
    if (eRCode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return eRCode;
    }

    /* send the request to the SDK process */
    eRCode = SwiQmiMISendnWait( pReqBuf,
                              eQMI_SVC_DCS,
                              ParamLength,
                              eQMI_TIMEOUT_2_S, /* 2 Seconds */
                              &pInParam,
                              &ParamLength,
                              instance );

    if (eRCode == eQCWWAN_ERR_NONE)
    {
        /* unpack QMI response */
        eRCode = UpkQmiDcsSLQSQosUnmap ( pInParam, &response );

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
    else
    {
        qmrelreqbkp();
        return eRCode;
    }

    qmrelreqbkp();
    return eRCode;
}

ULONG SLQSQosClearMap(
        BYTE    instance
        )
{
    BYTE              *pInParam;    /* ptr to param field rx'd from modem */
    BYTE              *pOutParam;   /* ptr to outbound param field */
    BYTE              *pReqBuf;     /* Pointer to outgoing request buffer */
    USHORT             ParamLength; /* Ret'd length of the Param field */
    enum eQCWWANError  eRCode;      /* Result of SwiQmiSendnWait() */

    struct qm_qmi_response_tlvs_values response;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message and sends the message. */
    eRCode = PkQmiDcsSLQSQosClearMap ( &ParamLength,
                                    pOutParam);
    if (eRCode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return eRCode;
    }

    /* send the request to the SDK process */
    eRCode = SwiQmiMISendnWait( pReqBuf,
                              eQMI_SVC_DCS,
                              ParamLength,
                              eQMI_TIMEOUT_2_S, /* 2 Seconds */
                              &pInParam,
                              &ParamLength,
                              instance );

    if (eRCode == eQCWWAN_ERR_NONE)
    {
        /* unpack QMI response */
        eRCode = UpkQmiDcsSLQSQosClearMap ( pInParam, &response );

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
    else
    {
        qmrelreqbkp();
        return eRCode;
    }
    qmrelreqbkp();
    return eRCode;
}

ULONG SLQSQosEditMap(
        BYTE    instance,
        BYTE    dscp,
        ULONG   qos_id
        )
{
    BYTE              *pInParam;    /* ptr to param field rx'd from modem */
    BYTE              *pOutParam;   /* ptr to outbound param field */
    BYTE              *pReqBuf;     /* Pointer to outgoing request buffer */
    USHORT             ParamLength; /* Ret'd length of the Param field */
    enum eQCWWANError  eRCode;      /* Result of SwiQmiSendnWait() */

    struct qm_qmi_response_tlvs_values response;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message and sends the message. */
    eRCode = PkQmiDcsSLQSQosEditMap ( &ParamLength,
                                    pOutParam,
                                    dscp,
                                    qos_id );
    if (eRCode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return eRCode;
    }

    /* send the request to the SDK process */
    eRCode = SwiQmiMISendnWait( pReqBuf,
                              eQMI_SVC_DCS,
                              ParamLength,
                              eQMI_TIMEOUT_2_S, /* 2 Seconds */
                              &pInParam,
                              &ParamLength,
                              instance );

    if (eRCode == eQCWWAN_ERR_NONE)
    {
        /* unpack QMI response */
        eRCode = UpkQmiDcsSLQSQosEditMap ( pInParam, &response );

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
    else
    {
        qmrelreqbkp();
        return eRCode;
    }

    qmrelreqbkp();
    return eRCode;
}

ULONG SLQSQosReadMap(
        BYTE    instance,
        BYTE    dscp,
        ULONG   *pQos_id
        )
{
    BYTE              *pInParam;    /* ptr to param field rx'd from modem */
    BYTE              *pOutParam;   /* ptr to outbound param field */
    BYTE              *pReqBuf;     /* Pointer to outgoing request buffer */
    USHORT             ParamLength; /* Ret'd length of the Param field */
    enum eQCWWANError  eRCode;      /* Result of SwiQmiSendnWait() */

    struct qm_qmi_response_tlvs_values response;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message and sends the message. */
    eRCode = PkQmiDcsSLQSQosReadMap ( &ParamLength,
                                    pOutParam,
                                    dscp);
    if (eRCode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return eRCode;
    }

    /* send the request to the SDK process */
    eRCode = SwiQmiMISendnWait( pReqBuf,
                              eQMI_SVC_DCS,
                              ParamLength,
                              eQMI_TIMEOUT_2_S, /* 2 Seconds */
                              &pInParam,
                              &ParamLength,
                              instance );

    if (eRCode == eQCWWAN_ERR_NONE)
    {
        /* unpack QMI response */
        eRCode = UpkQmiDcsSLQSQosReadMap ( pInParam, &response );

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
    else
    {
        qmrelreqbkp();
        return eRCode;
    }

    *pQos_id = response.tlvvalues.qmdcstlvs.qosMap.qos_id;

    qmrelreqbkp();
    return eRCode;
}

ULONG SLQSQosDumpMap(
        BYTE    instance,
        struct QosMap *pmap,
        BYTE    *plen
        )
{
    BYTE              *pInParam;    /* ptr to param field rx'd from modem */
    BYTE              *pOutParam;   /* ptr to outbound param field */
    BYTE              *pReqBuf;     /* Pointer to outgoing request buffer */
    USHORT             ParamLength; /* Ret'd length of the Param field */
    enum eQCWWANError  eRCode;      /* Result of SwiQmiSendnWait() */

    struct qm_qmi_response_tlvs_values response;

    if( pmap == NULL || plen == NULL )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    if( *plen < MAX_QOS_MAP )
    {
        *plen = MAX_QOS_MAP;
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message and sends the message. */
    eRCode = PkQmiDcsSLQSQosDumpMap ( &ParamLength,
                                    pOutParam);

    if (eRCode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return eRCode;
    }

    /* send the request to the SDK process */
    eRCode = SwiQmiMISendnWait( pReqBuf,
                              eQMI_SVC_DCS,
                              ParamLength,
                              eQMI_TIMEOUT_2_S, /* 2 Seconds */
                              &pInParam,
                              &ParamLength,
                              instance );

    if (eRCode == eQCWWAN_ERR_NONE)
    {
        /* unpack QMI response */
        eRCode = UpkQmiDcsSLQSQosDumpMap ( pInParam, &response );

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
    else
    {
        qmrelreqbkp();
        return eRCode;
    }

    slmemcpy( pmap,
            &response.tlvvalues.qmdcstlvs.qosMaps[0],
            sizeof(response.tlvvalues.qmdcstlvs.qosMaps));
    *plen = MAX_QOS_MAP;

    qmrelreqbkp();
    return eRCode;
}
