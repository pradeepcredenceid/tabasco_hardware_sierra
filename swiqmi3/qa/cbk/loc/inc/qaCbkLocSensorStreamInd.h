/*
 * \ingroup cbk
 *
 * \file qaCbkLocSensorStreamInd.h
 *
 * \brief This file contains definitions, enumerations, structures and
 *        forward declarations for qaCbkLocSensorStreamInd.c
 *
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __LOC_SENSOR_STREAM_IND_H__
#define __LOC_SENSOR_STREAM_IND_H__

#include "qaGobiApiCbk.h"
/*
 * An enumeration of eQMI_LOC_SENSOR_STREAM_IND response TLV IDs
 *
 */
enum eQMI_LOC_SENSOR_STREAM_IND
{
    eTLV_IND_ACCEL_ACCEPT_READY = 0x10,
    eTLV_IND_GYRO_ACCEPT_READY  = 0x11,
    eTLV_IND_ACCEL_TEMP_ACCEPT  = 0x12,
    eTLV_IND_GYRO_TEMP_ACCEPT   = 0x13
};

/*
 * Prototypes
 *
 */
enum eQCWWANError UpkQmiCbkLocSensorStreamInd(
    BYTE                              *pMdmResp,
    QmiCbkLocSensorStreamingInd       *pApiResp );

#endif /* __LOC_SENSOR_STREAM_IND_H__ */
