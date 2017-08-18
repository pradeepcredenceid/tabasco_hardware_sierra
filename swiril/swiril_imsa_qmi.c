/**
 *
 * @ingroup swiril
 *
 * @file 
 * Provides IMSA Service specific functions
 *
 * @author
 * Copyright: © 2014 Sierra Wireless, Inc.
 *            All rights reserved
 *
 */

#include <telephony/ril.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <cutils/properties.h>

#include "swiril_main.h"
#include "swiril_cache.h"
#include "swiril_misc_qmi.h"
#include "at_channel.h"
#include "at_tok.h"

#include "aa/aaglobal.h"
#include "qmerrno.h"
#include "qmudefs.h"
#include "SWIWWANCMAPI.h"

#define LOG_TAG "RIL"
#include "swiril_log.h"


static void ImsaRegStatusCallback(imsaRegStatusInfo *pImsaRegStatusInfo)
{
}

/**
 * Register IMSA reg status call back function
 *
 * @param 
 *     none
 *
 * @return
 *     none
 */
void registerImsaRegStatusCallback(void)
{
    LOGD("%s::Entered:\n", __func__);
    ULONG nRet = 0;

    /* register callback function */
    nRet = SLQSSetIMSARegStatusCallback(ImsaRegStatusCallback);
    if ( nRet != eQCWWAN_ERR_NONE ) {
        LOGE("%s:: SLQSSetIMSARegStatusCallback failed, nRet: %lu\n", __func__, nRet );
    }
}

