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
#include <stdbool.h>

/* RIL version specific definitions */
#include "swiril_versiondefs.h"
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
extern int Default_CID(void);
extern bool IsHL8(void);
extern void resetpollSIMRetry(void);
extern void initLTEProperties(void);
extern void requestQueryIMSRegistrationState(void *data, size_t datalen,RIL_Token t);
extern void requestSetInitialAPN(void *data, size_t datalen, RIL_Token t);
extern void EnableATTimeoutHandler(void);
extern void DisableATTimeoutHandler(void);
extern bool isCancelQueryNetworkRequested(void);
#define FALSE       0
#define TRUE        1

#endif
