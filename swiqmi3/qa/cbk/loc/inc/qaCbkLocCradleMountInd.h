/*
 * \ingroup cbk
 *
 * \file qaCbkLocCradleMountInd.h
 *
 * \brief This file contains definitions, enumerations, structures and
 *        forward declarations for qaCbkLocCradleMountInd.c
 *
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __LOC_CRADLE_MOUNT_IND_H__
#define __LOC_CRADLE_MOUNT_IND_H__

#include "qaGobiApiCbk.h"
/*
 * An enumeration of eQMI_LOC_CRADLE_MOUNT_IND response TLV IDs
 *
 */
enum eQMI_LOC_CRADLE_MOUNT_IND
{
    eTLV_IND_CRADLE_MOUNT_STATUS = 0x01
};

/*
 * Prototypes
 *
 */
enum eQCWWANError UpkQmiCbkLocCradleMountInd(
    BYTE                                 *pMdmResp,
    QmiCbkLocCradleMountInd              *pApiResp );

#endif /* __LOC_CRADLE_MOUNT_IND_H__ */
