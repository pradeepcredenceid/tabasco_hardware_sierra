/**
 *    \if CvsId
 *    $Id: SwiApiDemux.h,v 1.11 2009/11/03 01:15:07 epasheva Exp $
 *    \endif
 *    \ingroup demux
 *    \file SwiApiDemux.h
 * 
 *    \brief This file contains definitions, enumerations, structures and
 *           forward declarations for SwiApiDxBasic.c file
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */
#ifndef __SWIAPDXBASIC
#define __SWIAPDXBASIC

#ifdef __cplusplus
extern "C" {
#endif

/* include files */
#include "SwiDataTypes.h"
#include "SwiRcodes.h"
#include "SwiApiCmBasic.h"

/* defines */
#define APDXCALLERLEN 5  /* length of caller id */

#define LENGTH_DxErrorString   40
#define LENGTH_DxData          4500

/* enum declarations */

/**
 *  This enumeration contains the Device Service indication
 */
enum SWI_TYPE_DeviceService
{
    SWI_DIAG,             /*!< Diagnostic service. */
    SWI_NMEA              /*!< NMEA service. */
};

/**
 *  This enumeration contains the Operation Code for Demux Request
 *  message
 */
enum SWI_TYPE_OperationCode
{
    SWI_DxStart,        /*!< API to SDK: Start Demux device service. */
    SWI_DxStop          /*!< API to SDK: Stop Demux device service. */
};

/**
 *  This enumeration contains Device Present for Demux Control
 *  Indication message
 */
enum SWI_TYPE_DevicePresent
{
    SWI_OFFLINE,            /*!< device offline. */
    SWI_ONLINE              /*!< device online. */
};

/**
 *  This enumeration contains the Operation Type for Demux Control
 *  Indication message
 */
enum SWI_TYPE_OperationType
{
    SWI_MdmStatusChange,        /*!< SDK to API: Modem Status Change. */
    SWI_APDXShutdown,           /*!< API to SDK: Application Shutdown. */
    SWI_SDKShutdown,            /*!< SDK to API: SDK shutdown. */
    SWI_StartReqResponse,       /*!< SDK to API: Response to Start Request. */
    SWI_StopReqResponse         /*!< SDK to API: Response to Stop Request. */
};

/**
 *  This structure is used to store Demux Request message. 
 */
struct SWI_STRUCT_DemuxRequest
{
    /*! size of this structure */
    swi_uint32                   sizeStruct;
    /*! device service - diagnostic or NMEA */
    enum SWI_TYPE_DeviceService  eDeviceService;
    /*! operation code */
    enum SWI_TYPE_OperationCode  eOpCode;
};

/**
 *  This structure is used to store Demux Control Indication
 *  message. 
 */
struct SWI_STRUCT_DemuxCtlInd
{
    /*! size of this structure */
    swi_uint32                    sizeStruct;
    /*! deviceservice - diagnostic or NMEA */
    enum SWI_TYPE_DeviceService   eDeviceService;
    /*! operation type */
    enum SWI_TYPE_OperationType   eOpType;
    /*! device present indication */
    enum SWI_TYPE_DevicePresent   eDevicePresent;
    /*! error string */
    swi_uint8                     sErrorString[LENGTH_DxErrorString];
};

/**
 *  This structure is used to store Demux Data Indication
 *  message. 
 */
struct SWI_STRUCT_DemuxDataInd
{
    /*! size of this structure */
    swi_uint32                    sizeStruct;
    /*! deviceservice - diagnostic or NMEA */
    enum SWI_TYPE_DeviceService   eDeviceService;
    /*! length of data packet */
    swi_uint32                    nDataLen;
    /*! variable size data packet up to LENGTH_DxData bytes */
    swi_uint8                     nData[LENGTH_DxData];
};

/* Function prototypes */
extern SWI_API SWI_RCODE SwiApiDxStartup(swi_uint16 numchannels, 
    struct SWI_STRUCT_ApiStartup *pOpenParams);
extern SWI_API SWI_RCODE SwiApiDxRegister(swi_uint16 deviceservice, 
    void (*datacbfp)(swi_uint16 deviceservice, swi_uint8 *datap, 
    swi_uint32 datalen, void *userp), void *datauserp, 
    void (*ctlcbfp)(swi_uint16 deviceservice, swi_uint8 *ctldatap, void *userp), 
    void *ctluserp ); 
extern SWI_API SWI_RCODE SwiApiDxBegin(swi_uint16 deviceservice);
extern SWI_API SWI_RCODE SwiApiDxEnd(swi_uint16 deviceservice);
extern SWI_API SWI_RCODE SwiApiDxSend(swi_uint16 deviceservice, 
    swi_uint8 *datap, swi_uint32 datalen);
extern SWI_API SWI_RCODE SwiWaitDxIndication(swi_uint16 deviceservice);
extern swi_bool SwiApDxRunning( void );
extern swi_bool SwiApDxAirServerPresent( void );
extern struct apdxcb *SwiApDxGetCbp(void);
extern swi_uint8 *SwiApDxDevToName( swi_uint16 deviceservice );
extern swi_uint32 SwiApDxDevToChan( swi_uint16 deviceservice );
extern SWI_API SWI_RCODE SwiGetDataPldOffset( swi_uint8 *bufferp, 
    swi_uint32 datalen, swi_uint8 **payloadpp, swi_uint32 *bytesbuildp);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif // __SWIAPDXBASIC

/* 
 * $Log: SwiApiDemux.h,v $
 */
