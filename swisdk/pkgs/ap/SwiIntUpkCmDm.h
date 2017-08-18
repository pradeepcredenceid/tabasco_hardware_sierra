/**
 *    $Id: SwiIntUpkCmDm.h,v 1.2 2010/06/10 00:25:24 epasheva Exp $
 *
 *    Name       : SwiIntUpkCmDm.h
 *    
 *    Purpose    : Unpack functions for OMA-DM APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#ifndef __SWIINTUPKCMDM_H__
#define __SWIINTUPKCMDM_H__

/* include files */
#include "SwiDataTypes.h"
#include "SwiApiCmDm.h"

extern void SwiUnpackOMADMConfig(swi_uint8 *paramfieldp, 
                                 SWI_STRUCT_OMADM_GET_CONFIG *psConfig);

extern void SwiUnpackOMADMSessionState(swi_uint8 *paramfieldp, 
                                SWI_STRUCT_OMADM_SESSION_STATE *psState);

extern void SwiUnpackOMADMNIAlert(swi_uint8 *paramfieldp, 
                           SWI_STRUCT_OMADM_NI_ALERT *psAlert);

extern void SwiUnpackOMADMUIAlertConfirm(swi_uint8 *paramfieldp);
extern void SwiUnpackOMADMUIAlertInput(swi_uint8 *paramfieldp);
extern void SwiUnpackOMADMUIAlertChoice(swi_uint8 *paramfieldp);

extern void SwiUnpackOMADMNodeValue(swi_uint8 *paramfieldp, 
                             SWI_STRUCT_OMADM_NODE_VALUE_RESPONSE *psValue);

extern void SwiUnpackDMDLProgress(swi_uint8 *paramfieldp, 
                           SWI_STRUCT_DM_DL_PROGRESS *psProgress);

extern SWI_RCODE SwiUnpackDMDLAllert(swi_uint8 *paramfieldp, 
                              SWI_STRUCT_DM_DL_ALERT *psAlert);

extern SWI_RCODE SwiUnpackDMDLDescription(swi_uint8 *paramfieldp, 
                                   SWI_STRUCT_DM_DL_DESCRIPTION *psDescription);

extern void SwiUnpackDMDLInstallStatus(swi_uint8 *paramfieldp, 
                                SWI_STRUCT_DM_DL_INSTALL_STATUS *psStatus);

#endif /* __SWIINTUPKCMDM_H__ */

/* 
 * $Log: SwiIntUpkCmDm.h,v $
 */
