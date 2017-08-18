/**
 *    \if CvsId 
 *    $Id: SwiApiGsmSs.h,v 1.6 2010/06/10 00:20:12 epasheva Exp $
 *    \endif
 *    \ingroup gsmss
 *    \file SwiApiGsmSs.h
 * 
 *    \brief  Header file for for GSM APIs related to the 
 *            Supplementary Services.
 *
 *    Copyright: © 2008 Sierra Wireless, Inc. all rights reserved
 *
 */
#ifndef __SWIAPI_GSM_SS
#define __SWIAPI_GSM_SS

#ifdef __cplusplus
extern "C" {
#endif

/* include files */
#include "SwiDataTypes.h"
#include "SwiRcodes.h"

/* contants */
#define LENGTH_USSDInputExp         230

/* enumerations */
/**
 *
 * This enumeration contains supplementary services request status.
 *
 * \sa struct SWI_STRUCT_SS_Request 
 * \sa struct SWI_STRUCT_UnstructSupplServiceExp
 *
 */
enum SWI_TYPE_SS_RequestStatus
{
    /*! request proceeding */
    SWI_SSREQUESTSTATUS_Processing = 0x00,

    /*! request rejected due to FDN mismatch */
    SWI_SSREQUESTSTATUS_Fdn
};


/* structures */
/**
 *
 * The notification delivers unstructured supplementary service information 
 * to the application.
 * 
 * This structure is used in the SWI_NOTIFY_SuplementryServiceReq 
 * notification.
 *
 * \sa SWI_NOTIFY_SuplementryServiceReq
 *
 */
struct SWI_STRUCT_UnstructSupplServiceExp
{
    /*! The size, in bytes, of this structure */
    swi_uint32 sizeStruct;
    
    /*! Version of unstructured supplementary service expanded.  Ignore this
        field. */
    swi_uint32 nVersion;
    
    /*! Supplementary service handle. */
    swi_uint32 nHandle;
    
    /*! False is returned if no user response required, true otherwise. */
    swi_bool bUserResponseReq;
    
    /*! Request response status */
    enum SWI_TYPE_SS_RequestStatus eRequestStatus;
    
    /*! Count of characters in the byMessage.  */
    swi_uint32 nCntChars;
    
    /*! Raw USSD message for user (GSM code). */
    swi_uint8 byMessage[LENGTH_USSDInputExp];
};

/* prototypes */

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __SWIAPI_GSM_SS */
/* 
 * $Log: SwiApiGsmSs.h,v $
 */
