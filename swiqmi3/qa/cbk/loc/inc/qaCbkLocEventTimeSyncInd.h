/*
 * \ingroup cbk
 *
 * \file qaCbkLocEventTimeSyncInd.h
 *
 * \brief This file contains definitions, enumerations, structures and
 *        forward declarations for qaCbkLocEventTimeSyncInd.c
 *
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __LOC_EVENT_TIME_SYNC_IND_H__
#define __LOC_EVENT_TIME_SYNC_IND_H__

#include "qaGobiApiCbk.h"
/*
 * An enumeration of eQMI_LOC_EVENT_TIME_SYNC_IND response TLV IDs
 *
 */
enum eQMI_LOC_EVENT_TIME_SYNC_IND
{
    eTLV_IND_REF_COUNTER = 0x01
};

/*
 * Prototypes
 *
 */
enum eQCWWANError UpkQmiCbkLocEventTimeSyncInd(
    BYTE                                 *pMdmResp,
    QmiCbkLocEventTimeSyncInd            *pApiResp );

#endif /* __LOC_EVENT_TIME_SYNC_IND_H__ */
