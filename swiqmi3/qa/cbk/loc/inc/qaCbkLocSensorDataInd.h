/*
 * \ingroup cbk
 *
 * \file qaCbkLocSensorDataInd.h
 *
 * \brief This file contains definitions, enumerations, structures and
 *        forward declarations for qaCbkLocSensorDataInd.c
 *
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __LOC_SENSOR_DATA_IND_H__
#define __LOC_SENSOR_DATA_IND_H__

#include "qaGobiApiCbk.h"
/*
 * An enumeration of eQMI_LOC_INJECT_SENSOR_DATA_IND response TLV IDs
 *
 */
enum eQMI_LOC_SENSOR_DATA_IND
{
    eTLV_IND_INJECT_SENSOR_STATUS = 0x01,
    eTLV_IND_OPAQUE_IDENTIFIER    = 0x10,
    eTLV_IND_ACCEL_SAMPLES_ACCEPT = 0x11,
    eTLV_IND_GYRO_SAMPLES_ACCEPT  = 0x12,
    eTLV_IND_ACCEL_TEMP_SAMPLES   = 0x13,
    eTLV_IND_GYRO_TEMP_SAMPLES    = 0x14
};

/*
 * Prototypes
 *
 */
enum eQCWWANError UpkQmiCbkLocSensorDataInd(
    BYTE                              *pMdmResp,
    QmiCbkLocInjectSensorDataInd      *pApiResp );

#endif /* __LOC_SENSOR_DATA_IND_H__ */
