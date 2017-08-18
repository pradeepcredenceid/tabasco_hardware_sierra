/**
 *    $Id: SwiIntPkCmDm.h,v 1.2 2010/06/10 00:23:06 epasheva Exp $
 *
 *    Name       : SwiIntPkCmDm.h
 *    
 *    Purpose    : Pack functions for OMA-DM APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#ifndef __SWIINTUPKCMDM_H__
#define __SWIINTUPKCMDM_H__

/* include files */
#include "SwiDataTypes.h"
#include "SwiApiCmDm.h"

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

#endif /* __SWIINTUPKCMDM_H__ */

/* 
 * $Log: SwiIntPkCmDm.h,v $
 */
