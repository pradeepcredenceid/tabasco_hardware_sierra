/* 
** This source code is "Not a Contribution" under Apache license
**
** Sierra Wireless RIL
**
** Based on reference-ril by The Android Open Source Project
** and U300 RIL by ST-Ericsson.
** Modified by Sierra Wireless, Inc.
**
** Copyright (C) 2011 Sierra Wireless, Inc.
** Copyright (C) ST-Ericsson AB 2008-2009
** Copyright 2006, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** Based on the Android ril daemon and reference RIL by 
** The Android Open Source Project.
**
** Heavily modified for ST-Ericsson U300 modems.
** Author: Christian Bejram <christian.bejram@stericsson.com>
*/

#include <stdlib.h>
#include <telephony/ril.h>
#include <assert.h>
#include <string.h>

#define LOG_TAG "RIL"
#include "swiril_log.h"

#include <swiril_main.h>


/* Handler functions. The names are because we cheat by including
 * ril_commands.h from rild. In here we generate local allocations
 * of the data representations, as well as free:ing them after
 * they've been handled.
 *
 * This design might not be ideal, but considering the alternatives,
 * it's good enough.
 */
static void *dummyDispatch(void *data, size_t datalen);

#define dispatchCdmaSms dispatchRaw
#define dispatchCdmaSmsAck dispatchRaw
#define dispatchRilCdmaSmsWriteArgs dispatchRaw
#define dispatchCdmaBrSmsCnf dummyDispatch
#define dispatchVoiceRadioTech dummyDispatch
#define dispatchCdmaSubscriptionSource dummyDispatch

static void *dispatchCallForward(void *data, size_t datalen);
static void *dispatchDial(void *data, size_t datalen);
static void *dispatchSIM_IO(void *data, size_t datalen);
static void *dispatchSmsWrite(void *data, size_t datalen);
static void *dispatchString(void *data, size_t datalen);
static void *dispatchStrings(void *data, size_t datalen);
static void *dispatchRaw(void *data, size_t datalen);
static void *dispatchVoid(void *data, size_t datalen);
static void *dispatchGsmBrSmsCnf(void *data, size_t datalen);

#define dispatchInts dispatchRaw
#define dispatchDataCall dispatchStrings

static void dummyResponse(void);

#define responseCallForwards dummyResponse
#define responseCallList dummyResponse
#define responseCellList dummyResponse
#define responseContexts dummyResponse
#define responseInts dummyResponse
#define responseRaw dummyResponse
#define responseSIM_IO dummyResponse
#define responseSMS dummyResponse
#define responseString dummyResponse
#define responseStrings dummyResponse
#define responseVoid dummyResponse

#define responseSimStatus dummyResponse
#define responseRilSignalStrength dummyResponse
#define responseDataCallList dummyResponse
#define responseGsmBrSmsCnf dummyResponse
#define responseStringsNetworks dummyResponse
#define responseCdmaBrSmsCnf dummyResponse
#ifdef SWI_RIL_VERSION_6
#define responseSetupDataCall dummyResponse
#endif
#ifdef SWI_RIL_VERSION_9
/* for Android 4.4*/
static void *dispatchImsSms(void *data, size_t datalen);
static void *dispatchSetInitialAttachApn (void *data, size_t datalen);
#define responseCellInfoList dummyResponse
#define responseGetDataCallProfile dummyResponse
#define dispatchUiccSubscripton dummyDispatch
#endif

#ifdef SWI_RIL_VERSION_10
/* for Android 5.0*/
#define dispatchSIM_APDU dummyDispatch
#define dispatchNVReadItem dummyDispatch
#define dispatchNVWriteItem dummyDispatch
#define dispatchSimAuthentication dummyDispatch
#define dispatchDataProfile dummyDispatch
#define responseDcRtInfo dummyResponse
#define responseHardwareConfig dummyResponse
#endif

#ifdef SWI_RIL_VERSION_11
/* for Android 5.1*/
#define dispatchRadioCapability dummyDispatch
#define responseRadioCapability dummyResponse
#endif

#ifdef SWI_RIL_VERSION_14
static void *dispatchCarrierRestrictions(void *data, size_t datalen);
#define responseCarrierRestrictions dummyResponse
#else
#define dispatchCarrierRestrictions dummyDispatch
#define responseCarrierRestrictions dummyResponse
#endif

#define responseFailCause responseInts
#define responseLceStatus dummyResponse
#define responseLceData dummyResponse
#define responseActivityData dummyResponse

typedef struct CommandInfo {
    int requestId;
    void *(*dispatchFunction) (void *data, size_t datalen);
    void (*responseFunction) (void);
} CommandInfo;

/* RILD made me do it! */
static CommandInfo s_commandInfo[] = {
#include <ril_commands.h>
};

static void *dummyDispatch(void *data, size_t datalen)
{
    return 0;
}

static void dummyResponse(void)
{
    return;
}

/**
 * dupRequestData will copy the data pointed to by *data, returning a pointer
 * to a freshly allocated representation of the data.
 */
void *dupRequestData(int requestId, void *data, size_t datalen)
{
    CommandInfo *ci = &s_commandInfo[requestId];

    return ci->dispatchFunction(data, datalen);
}

static void *dispatchCallForward(void *data, size_t datalen)
{
    RIL_CallForwardInfo *ret = dispatchRaw(data, datalen);

    if (ret->number)
        ret->number = strdup(ret->number);

    return ret;
}

static void *dispatchDial(void *data, size_t datalen)
{
    RIL_Dial *ret = dispatchRaw(data, datalen);

    if (ret->address)
        ret->address = strdup(ret->address);

    return ret;
}

#ifdef   SWI_RIL_VERSION_12
static void *dispatchSIM_IO(void *data, size_t datalen)
{
    RIL_SIM_IO *ret = dispatchRaw(data, datalen);

    if (ret->path)
        ret->path = strdup(ret->path);
    if (ret->data)
        ret->data = strdup(ret->data);
    if (ret->pin2)
        ret->pin2 = strdup(ret->pin2);
    if (ret->aidPtr)
        ret->aidPtr = strdup(ret->aidPtr);
    return ret;
}
#else
static void *dispatchSIM_IO(void *data, size_t datalen)
{
    RIL_SIM_IO *ret = dispatchRaw(data, datalen);

    if (ret->path)
        ret->path = strdup(ret->path);
    if (ret->data)
        ret->data = strdup(ret->data);
    if (ret->pin2)
        ret->pin2 = strdup(ret->pin2);

    return ret;
}
#endif

static void *dispatchSmsWrite(void *data, size_t datalen)
{
    RIL_SMS_WriteArgs *ret = dispatchRaw(data, datalen);

    if (ret->pdu)
        ret->pdu = strdup(ret->pdu);

    if (ret->smsc)
        ret->smsc = strdup(ret->smsc);

    return ret;
}

static void *dispatchString(void *data, size_t datalen)
{
    assert(datalen == sizeof(char *));

    if (data)
        return strdup((char *) data);

    return NULL;
}

static void *dispatchStrings(void *data, size_t datalen)
{
    char **a = (char **)data;
    char **ret;
    int strCount = datalen / sizeof(char *);
    int i;

    assert((datalen % sizeof(char *)) == 0);

    ret = malloc(strCount * sizeof(char *));
    memset(ret, 0, sizeof(char *) * strCount);

    for (i = 0; i < strCount; i++) {
        if (a[i])
            ret[i] = strdup(a[i]);
    }

    return (void *) ret;
}

static void *dispatchGsmBrSmsCnf(void *data, size_t datalen)
{
    RIL_GSM_BroadcastSmsConfigInfo **a = 
        (RIL_GSM_BroadcastSmsConfigInfo **) data;
    int count;
    void **ret;
    int i;

    count = datalen / sizeof(RIL_GSM_BroadcastSmsConfigInfo *);

    ret = malloc(count * sizeof(RIL_GSM_BroadcastSmsConfigInfo *));
    memset(ret, 0, sizeof(*ret));

    for (i = 0; i < count; i++) {
        if (a[i])
            ret[i] = dispatchRaw(a[i], sizeof(RIL_GSM_BroadcastSmsConfigInfo));
    }

    return ret;
}
#ifdef SWI_RIL_VERSION_9
static void *dispatchImsSms(void *data, size_t datalen)
{
    RIL_IMS_SMS_Message *ims = (RIL_IMS_SMS_Message *)data;
    RIL_IMS_SMS_Message *ret = malloc(sizeof(RIL_IMS_SMS_Message));
    
    ret->tech = ims->tech;
    ret->retry = ims->retry;
    ret->messageRef = ims->messageRef;
    
    if (RADIO_TECH_3GPP == ret->tech) {
        ret->message.gsmMessage=(char **)dispatchStrings((void *)ims->message.gsmMessage, (datalen-sizeof(RIL_RadioTechnologyFamily)-sizeof(uint8_t)-sizeof(int32_t)));
    } else if (RADIO_TECH_3GPP2 == ret->tech) {
        ret->message.cdmaMessage=malloc(sizeof(RIL_CDMA_SMS_Message));
        memcpy(ret->message.cdmaMessage, ims->message.cdmaMessage, sizeof(RIL_CDMA_SMS_Message));
    } else {
        LOGE("requestImsSendSMS invalid format value =%d", ret->tech);
    }
    return ret;
}

static void *dispatchSetInitialAttachApn(void *data, size_t datalen)
{
    RIL_InitialAttachApn *ret = dispatchRaw(data, datalen);
    if(ret) {
        if (ret->apn)
            ret->apn = strdup(ret->apn);

        if (ret->protocol)
            ret->protocol = strdup(ret->protocol);

        if (ret->username)
            ret->username = strdup(ret->username);
            
        if (ret->password)
            ret->password = strdup(ret->password);
    }
    return ret;
}

#endif

#ifdef SWI_RIL_VERSION_14
static void *dispatchCarrierRestrictions(void *data, size_t datalen)
{
    RIL_CarrierRestrictions *src = (RIL_CarrierRestrictions *)data;
    RIL_CarrierRestrictions *ret = malloc(sizeof(RIL_CarrierRestrictions));
    int count = 0;
    int i;

    if(ret) {    
        ret->len_allowed_carriers = src->len_allowed_carriers;
        ret->len_excluded_carriers = src->len_excluded_carriers;
        
        count = ret->len_allowed_carriers;
        ret->allowed_carriers = malloc(count * sizeof(RIL_Carrier));
        for (i = 0; i < count; i++) {
            RIL_Carrier a = src->allowed_carriers[i];
            ret->allowed_carriers[i].mcc = strdup(a.mcc);
            ret->allowed_carriers[i].mnc = strdup(a.mnc);
            ret->allowed_carriers[i].match_type = a.match_type;
            ret->allowed_carriers[i].match_data = strdup(a.match_data);
        }
        
        count = ret->len_excluded_carriers;
        ret->excluded_carriers = malloc(count * sizeof(RIL_Carrier));
        for (i = 0; i < count; i++) {
            RIL_Carrier a = src->excluded_carriers[i];
            ret->excluded_carriers[i].mcc = strdup(a.mcc);
            ret->excluded_carriers[i].mnc = strdup(a.mnc);
            ret->excluded_carriers[i].match_type = a.match_type;
            ret->excluded_carriers[i].match_data = strdup(a.match_data);
        }
    }
    return ret;
}
#endif

static void *dispatchRaw(void *data, size_t datalen)
{
    void *ret = malloc(datalen);
    if(ret && data) {
        memcpy(ret, data, datalen);
        return (void *) ret;
    }
    else
        return NULL;
}

static void *dispatchVoid(void *data, size_t datalen)
{
    return NULL;
}

static void freeDial(void *data)
{
    RIL_Dial *d = data;

    if (d->address)
        free(d->address);

    free(d);
}

static void freeStrings(void *data, size_t datalen)
{
    int count = datalen / sizeof(char *);
    int i;

    for (i = 0; i < count; i++) {
        if (((char **) data)[i])
            free(((char **) data)[i]);
    }

    free(data);
}

static void freeGsmBrSmsCnf(void *data, size_t datalen)
{
    int count = datalen / sizeof(RIL_GSM_BroadcastSmsConfigInfo);
    int i;

    for (i = 0; i < count; i++) {
        if (((RIL_GSM_BroadcastSmsConfigInfo **) data)[i])
            free(((RIL_GSM_BroadcastSmsConfigInfo **) data)[i]);
    }

    free(data);
}

#ifdef   SWI_RIL_VERSION_12
static void freeSIM_IO(void *data)
{
    RIL_SIM_IO *sio = data;

    if (sio->path)
        free(sio->path);
    if (sio->data)
        free(sio->data);
    if (sio->pin2)
        free(sio->pin2);
    if (sio->aidPtr)
        free(sio->aidPtr);
    free(sio);
}
#else
static void freeSIM_IO(void *data)
{
    RIL_SIM_IO *sio = data;

    if (sio->path)
        free(sio->path);
    if (sio->data)
        free(sio->data);
    if (sio->pin2)
        free(sio->pin2);

    free(sio);
}
#endif

static void freeSmsWrite(void *data)
{
    RIL_SMS_WriteArgs *args = data;

    if (args->pdu)
        free(args->pdu);

    if (args->smsc)
        free(args->smsc);

    free(args);
}

static void freeCallForward(void *data)
{
    RIL_CallForwardInfo *cff = data;

    if (cff->number)
        free(cff->number);

    free(cff);
}

#ifdef SWI_RIL_VERSION_9
static void freeSetInitialAttachApn(void *data)
{
    RIL_InitialAttachApn *profile = data;
    
    if (profile->apn)
        free(profile->apn);

    if (profile->protocol)
        free(profile->protocol);

    if (profile->username)
        free (profile->username);
        
    if (profile->password)
        free (profile->password);
        
    free(profile);   
    
}
#endif

void freeRequestData(int requestId, void *data, size_t datalen)
{
    CommandInfo *ci = &s_commandInfo[requestId];

    if (ci->dispatchFunction == dispatchInts ||
        ci->dispatchFunction == dispatchRaw ||
        ci->dispatchFunction == dispatchString) {
        if (data)
            free(data);
    } else if (ci->dispatchFunction == dispatchStrings) {
        freeStrings(data, datalen);
    } else if (ci->dispatchFunction == dispatchSIM_IO) {
        freeSIM_IO(data);
    } else if (ci->dispatchFunction == dispatchDial) {
        freeDial(data);
    } else if (ci->dispatchFunction == dispatchVoid) {
    } else if (ci->dispatchFunction == dispatchCallForward) {
        freeCallForward(data);
    } else if (ci->dispatchFunction == dispatchSmsWrite) {
        freeSmsWrite(data);
    } else if (ci->dispatchFunction == dispatchGsmBrSmsCnf) {
        freeGsmBrSmsCnf(data, datalen);
    }
#ifdef SWI_RIL_VERSION_9    
    else if (ci->dispatchFunction == dispatchSetInitialAttachApn) {
        freeSetInitialAttachApn(data);
    }
#endif
}
