/*
 * \ingroup loc
 *
 * \file  qaSlqsLocSetCradleMountConfig.h
 *
 * \brief This file contains definitions, enumerations, structures and
 *        forward declarations for qaSlqsLocSetCradleMountConfig.c
 *
 * Copyright: © 2011-2016 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __LOC_SET_CRADLE_MOUNT_CONFIG_H__
#define __LOC_SET_CRADLE_MOUNT_CONFIG_H__

#include "qaGobiApiLoc.h"

enum eQMI_LOC_SET_CRADLE_MOUNT_CONFIG_REQ
{
    eTLV_CRADLE_MOUNT_STATE             = 0x01,
    eTLV_CRADLE_MOUNT_CONFIDENCE        = 0X10
};

struct QmiLocSetCradleMountConfigResp
{
    struct qmTlvResult results;
};

enum eQCWWANError PkQmiLocSetCradleMountConfig(
    WORD                   *pMlength,
    BYTE                   *pParamField,
    LocSetCradleMountReq   *pReq);

enum eQCWWANError UpkQmiLocSetCradleMountConfig(
                  BYTE                                    *pMdmResp,
                  struct QmiLocSetCradleMountConfigResp   *pApiResp);

#endif /* __LOC_SET_CRADLE_MOUNT_CONFIG_H__ */
