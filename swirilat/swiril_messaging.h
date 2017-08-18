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

#ifndef U300_RIL_MESSAGING_H
#define U300_RIL_MESSAGING_H 1

void onNewStatusReport(const char *sms_pdu);
void onNewSms(const char *sms_pdu);
void onNewSmsOnSIM(const char* s);
void requestSendSMS(void *data, size_t datalen, RIL_Token t);
void requestSMSAcknowledge(void *data, size_t datalen, RIL_Token t);
void requestDeleteSmsOnSim(void *data, size_t datalen, RIL_Token t);
void requestWriteSmsToSim(void *data, size_t datalen, RIL_Token t);
void requestGetSMSCAddress(void *data, size_t datalen, RIL_Token t);
void requestSetSMSCAddress(void *data, size_t datalen, RIL_Token t);

#endif
