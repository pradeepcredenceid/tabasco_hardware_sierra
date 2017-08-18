/*
** This source code is "Not a Contribution" under Apache license
**
** Sierra Wireless RIL
**
** Based on reference-ril by The Android Open Source Project
** and U300 RIL by ST-Ericsson.
** Modified by Sierra Wireless, Inc.
**
** Copyright (C) 2012 Sierra Wireless, Inc.
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

#ifndef SWIRIL_NETWORK_AT_H
#define SWIRIL_NETWORK_AT_H 1
#include "swiril_network.h"
#include "swiril_versiondefs.h"

void onNetworkTimeReceived(const char *s);
void onIndicatorEvent(const char *s);
int onNetworkRegistrationReceived(const char *s);

void requestSetBandMode(void *data, size_t datalen, RIL_Token t);
void requestQueryAvailableBandMode(void *data, size_t datalen, RIL_Token t);
void requestSetNetworkSelectionAutomatic(void *data, size_t datalen,
                                         RIL_Token t);
void requestSetNetworkSelectionManual(void *data, size_t datalen,
                                      RIL_Token t);
void requestQueryAvailableNetworks(void *data, size_t datalen,
                                   RIL_Token t);
void requestSetPreferredNetworkType(void *data, size_t datalen,
                                    RIL_Token t);
void requestGetPreferredNetworkType(void *data, size_t datalen,
                                    RIL_Token t);
void requestQueryNetworkSelectionMode(void *data, size_t datalen,
                                      RIL_Token t);
#if defined(SWI_RIL_VERSION_12)
void requestSignalStrength_r12(void *data, size_t datalen, RIL_Token t);
#elif defined(SWI_RIL_VERSION_6)
void requestSignalStrength_r6(void *data, size_t datalen, RIL_Token t);
#else
void requestSignalStrength(void *data, size_t datalen, RIL_Token t);
#endif
void requestRegistrationState(int request, void *data,
                              size_t datalen, RIL_Token t);
void requestSetLocationUpdates(void *data, size_t datalen, RIL_Token t);
void requestGetNeighboringCellIDS(void *data, size_t datalen, RIL_Token t);
bool getRILRatAlternative(RIL_RAT *rat);
bool isOperatingLTE(void);

#endif /* end of SWIRIL_NETWORK_AT_H */
