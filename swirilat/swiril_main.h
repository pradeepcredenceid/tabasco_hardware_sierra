/*
** This source code is "Not a Contribution" under Apache license
**
** Sierra Wireless RIL
**
** Based on reference-ril by The Android Open Source Project
** and U300 RIL by ST-Ericsson.
** Modified by Sierra Wireless, Inc.
**
** Copyright (C) 2010 Sierra Wireless, Inc.
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
** Based on reference-ril by The Android Open Source Project.
**
** Heavily modified for ST-Ericsson U300 modems.
** Author: Christian Bejram <christian.bejram@stericsson.com>
*/

#ifndef U300_RIL_H
#define U300_RIL_H 1

/* RIL version specific definitions */
#include "swiril_versiondefs.h"
#include <stdbool.h>

/* For GCF/PTCRB protocol test purpose
 * SMS will switch to mode 2 if GCF_TEST_FLAG was on
 * RIL will disable call forwarding retry if GCF_TEST_FLAG was on
 * RIL will use CHLD=1 to disconnect all call if GCF_TEST_FLAG was on
 * RIL will set gcfen=1 in modem when HSM host perform factory reset
 * Disable by default
 */
/*#define RIL_GCF_TEST_FLAG */

RIL_RadioState currentState();
void setRadioState(RIL_RadioState newState);
void getScreenStateLock();
int getScreenState();
void releaseScreenStateLock();
int is_modem_ps_only(void);
int ecclistReadModemNVSet(void);
int ecclistReadSimCardSet(void);
const char * default_property_get(const char *key);
#define PROPERTY_GET(key,value) property_get(key, value, default_property_get(key))
void updateAudioProfile(bool);
void switchAudioProfile(void);
void updateVolumeLevel(int);
void updateParameters(int,char*,char*);
void updateIIRFilterParameters(int,char*,char*);
bool getDefaultVolumebyProfile(int profile, int *volume);

char* ril_iface;

const struct RIL_Env *s_rilenv;

#define RIL_onRequestComplete(t, e, response, responselen) s_rilenv->OnRequestComplete(t,e, response, responselen)
#define RIL_onUnsolicitedResponse(a,b,c) s_rilenv->OnUnsolicitedResponse(a,b,c)

void enqueueRILEvent(int isPrio, void (*callback) (void *param), 
                     void *param, const struct timeval *relativeTime);

#define RIL_EVENT_QUEUE_NORMAL 0
#define RIL_EVENT_QUEUE_PRIO 1
#define RIL_EVENT_QUEUE_ALL 2

extern void onRequest(int request, void *data, size_t datalen,
                      RIL_Token t);
extern int onSupports(int requestCode);
extern void onCancel(RIL_Token t);
extern const char *getVersion(void);

#define FALSE       0
#define TRUE        1

#endif
