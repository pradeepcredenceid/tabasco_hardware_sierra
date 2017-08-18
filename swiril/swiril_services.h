/**
 *
 * @ingroup swiril
 *
 * @file 
 * Prototypes for voice RIL QMI Sierra functions
 *
 * @author
 * Copyright: © 2012 Sierra Wireless, Inc.
 *            All rights reserved
 *
 */

#ifndef SWIRIL_SERVICES_H
#define SWIRIL_SERVICES_H 1

void requestQueryClip(void *data, size_t datalen, RIL_Token t);
void requestGetCLIRQMI(void *data, size_t datalen, RIL_Token t);
void requestSetCLIRQMI(void *data, size_t datalen, RIL_Token t);
void requestSetCallForward(void *data, size_t datalen, RIL_Token t);
void requestQueryCallForwardStatus(void *data, size_t datalen, RIL_Token t);
void requestQueryCallWaiting(void *data, size_t datalen, RIL_Token t);
void requestSetCallWaiting(void *data, size_t datalen, RIL_Token t);
void requestStartDTMF(void *data, size_t datalen, RIL_Token t);
void requestStopDTMF(void *data, size_t datalen, RIL_Token t);
void requestBurstDTMF(void *data, size_t datalen, RIL_Token t);
void requestSendUSSD(void *data, size_t datalen, RIL_Token t);
void requestCancelUSSD(void *data, size_t datalen, RIL_Token t);
void requestChangeBarringPassword(void *data, size_t datalen, RIL_Token t);
void requestSendUSSD_AT(void *data, size_t datalen, RIL_Token t);
void requestCancelUSSD_AT(void *data, size_t datalen, RIL_Token t);

#endif
