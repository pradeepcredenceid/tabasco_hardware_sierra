/*
 * \ingroup loc
 *
 * \file  qaSlqsLocInjectSensorData.h
 *
 * \brief This file contains definitions, enumerations, structures and
 *        forward declarations for qaSlqsLocInjectSensorData.c
 *
 * Copyright: © 2011-2016 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __LOC_INJECT_SENSOR_DATA_H__
#define __LOC_INJECT_SENSOR_DATA_H__

#include "qaGobiApiLoc.h"

enum eQMI_LOC_INJECT_SENSOR_DATA_REQ
{
    eTLV_INJECT_OPAQUE_ID             = 0x10,
    eTLV_INJECT_ACCELERO_DATA         = 0X11,
    eTLV_INJECT_GYRO_DATA             = 0X12,
    eTLV_ACCELERO_DATA_TIME_SRC       = 0X13,
    eTLV_GYRO_DATA_TIME_SRC           = 0X14,
    eTLV_INJECT_ACCELERO_TEMP_DATA    = 0X15,
    eTLV_INJECT_GYRO_TEMP_DATA        = 0X16
};

struct QmiLocInjectSensorDataResp
{
    struct qmTlvResult results;
};

enum eQCWWANError PkQmiLocInjectSensorData(
    WORD                     *pMlength,
    BYTE                     *pParamField,
    LocInjectSensorDataReq   *pReq);

enum eQCWWANError UpkQmiLocInjectSensorData(
                  BYTE                                    *pMdmResp,
                  struct QmiLocInjectSensorDataResp   *pApiResp);

#endif /* __LOC_INJECT_SENSOR_DATA_H__ */
