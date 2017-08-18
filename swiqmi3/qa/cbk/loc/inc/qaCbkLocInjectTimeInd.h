/*
 * \ingroup cbk
 *
 * \file qaCbkLocInjectTimeInd.h
 *
 * \brief This file contains definitions, enumerations, structures and
 *        forward declarations for qaCbkLocInjectTimeInd.c
 *
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __LOC_INJECT_TIME_IND_H__
#define __LOC_INJECT_TIME_IND_H__

#include "qaGobiApiCbk.h"
/*
 * An enumeration of eQMI_LOC_EVENT_TIME_SYNC_IND response TLV IDs
 *
 */
enum eQMI_LOC_INJECT_TIME_IND
{
    eTLV_IND_INJECT_TIME_STATUS = 0x01
};

/*
 * Prototypes
 *
 */
enum eQCWWANError UpkQmiCbkLocInjectTimeInd(
    BYTE                              *pMdmResp,
    QmiCbkLocInjectTimeInd            *pApiResp );

#endif /* __LOC_INJECT_TIME_IND_H__ */
