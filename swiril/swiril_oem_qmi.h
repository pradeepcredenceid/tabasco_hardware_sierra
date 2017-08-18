/**
 *
 * @ingroup swiril
 *
 * @file 
 * Prototypes for QMI based OEM related Sierra functions
 *
 * @author
 * Copyright: © 2011 Sierra Wireless, Inc.
 *            All rights reserved
 *
 */

#ifndef RIL_OEM_QMI_H
#define RIL_OEM_QMI_H 1

void requestOEMHookStringsCMDAActQMI(void *data, size_t datalen, RIL_Token t);
void requestOEMHookStrings(void *data, size_t datalen, RIL_Token t);

void requestOEMHookStringsGetppData(const char **data, size_t count, RIL_Token t);
void requestOEMHookStringsSetppData(const char **data, size_t count, RIL_Token t);
void requestOEMHookStringsValidateSPC(const char **data, size_t count, RIL_Token t);
void requestOEMHookStringsVoiceGetConfig(const char **data, size_t count, RIL_Token t);
void requestOEMHookStringsSetDDTMPref(const char **data, size_t count, RIL_Token t);
void requestOEMHookStringsGetAGPSConfig(const char **data, size_t count, RIL_Token t);
void requestOEMHookStringsSetAGPSConfig(const char **data, size_t count, RIL_Token t);

void requestOEMHookStringsGet3GPP2SubscriptionInfo(const char **data, size_t count, RIL_Token t);
void requestOEMHookStringsSet3GPP2SubscriptionInfo(const char **data, size_t count, RIL_Token t);
void requestOEMHookStringsGetCdmaProvisionStatus(const char **data, size_t count, RIL_Token t);
void requestOEMHookStringsGetCdmaCarrier(const char **data, size_t count, RIL_Token t);
void requestOEMHookStringsResetCdmaRadioState(const char **data, size_t count, RIL_Token t);
void requestOEMHookStringsSetAudioProfile(const char **data, size_t count, RIL_Token t);

#endif
