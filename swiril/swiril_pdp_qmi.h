/**
 *
 * @ingroup swiril
 *
 * @file 
 * Prototypes for PDP context related QMI Sierra functions
 *
 * @author
 * Copyright: © 2011 Sierra Wireless, Inc.
 *            All rights reserved
 *
 */

#ifndef RIL_PDP_QMI_H
#define RIL_PDP_QMI_H 1

#include <stdbool.h>
#include "swiril_versiondefs.h"

#define GOBI_DEFAULT_PROFILE_TYPE 0 /**< UMTS */
#define GOBI_DEFAULT_PDP_TYPE 0 /**< IP V4 */
#define GOBI_DATA_SESSION_TECHNOLOGY_UMTS 1 /**< UMTS */
#define GOBI_DATA_SESSION_TECHNOLOGY_CDMA 2 /**< CDMA */
#define GOBI_CALL_END_REASONS_CLIENT_END  2 /**< Client side end the data call */

/* 
 * From QCOM document "GobiConnectionMgmt.pdf, 80-VF219-10B", section 2.5.1
 */
typedef enum {
    PACKET_STATE_DISCONNECTED = 1,
    PACKET_STATE_CONNECTED,
    PACKET_STATE_SUSPENDED,
    PACKET_STATE_AUTHENTICATING
} Packet_SessionState;


typedef enum {
    QMI_TECH_UMTS = 1,
    QMI_TECH_CDMA2000
} QMI_PDPTech;

typedef enum {
    QMI_TRAFFIC_CHANNEL_DORMANT = 1,
    QMI_TRAFFIC_CHANNEL_ACTIVE
} QMI_DormancyState;

typedef enum {
    QMI_IPV4_ADDRESS = 4,
    QMI_IPV6_ADDRESS = 6,
    QMI_IPV4V6_ADDRESS = 7, /* this value is defined by SLQS not QCT */
    QMI_IP_ADDRESS_UNSPEC = 8
} QMI_IPFamilyPreference;

#define INSTANCE_0 0
#define WDS_EVENT_INTERVAL 30

void registerSessionStateCallbackQMI(void);
void registerByteTotalsCallbackQMI(void);
void onPDPContextListChangedQMI(void *param);
#ifdef SWI_RIL_VERSION_12
void requestPDPContextListQMI_r12(void *data, size_t datalen, RIL_Token t);
void requestDeactivateDefaultPDPQMI_r12(void *data, size_t datalen, RIL_Token t);
void requestSetupDefaultPDPQMI_r12(void *data, size_t datalen, RIL_Token t);
void dormancyStatusCallback_r12(ULONG dormancyStatus);
void initDataCallResponseList_r12(void);
void requestOrSendPDPContextListQMI_r12(RIL_Token *t);
#else
void requestPDPContextListQMI(void *data, size_t datalen, RIL_Token t);
void requestSetupDefaultPDPQMI(void *data, size_t datalen, RIL_Token t);
void requestDeactivateDefaultPDPQMI(void *data, size_t datalen, RIL_Token t);
void dormancyStatusCallback(ULONG dormancyStatus);
void requestOrSendPDPContextListQMI(RIL_Token *t);
void initDataCallResponseList(void);
#endif
void checkEHRPD(void *params);
void requestLastPDPFailCauseQMI(void *data, size_t datalen, RIL_Token t);
void requestAllowData(void *data, size_t datalen, RIL_Token t);

int getLastPdpFailCauseQMI(void);
void setLastPdpFailCauseQMI(int errCode);
int retrievePDPRejectCauseQMI(ULONG qmicode);
bool isDataSessionActive(void);
bool isAPNProfileIdentical(CHAR *pAPNName, ULONG *pAuthentication, CHAR  *pUsername, CHAR  *pPassword);

#endif
