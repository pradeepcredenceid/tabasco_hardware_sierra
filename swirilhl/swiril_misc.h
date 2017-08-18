/**
 *
 * @ingroup swiril
 *
 * @file 
 * Prototypes for misc Sierra functions
 *
 * @author
 * Copyright: 2014 Sierra Wireless, Inc.
 *            All rights reserved
 *
 */

#include <stdbool.h>

/* Maximum number of DNS addresses */
#define SWI_MAX_DNS 4
#define ECCLIST_PROPERTY          "ril.ecclist"
#define PROPERTY_SIERRA_DHCP_SRVC "init.svc.sierra_dhcpcd"

extern void swicheckp(void *datap, char *errString);
extern int waitDHCPnsetDNS(void);
extern int PollDHCPnsetDNS(void);
extern void * swimalloc(int size, char *errString);
extern char *sltoupperstrn(char *destp, const char *srcp, unsigned long length);
extern void checkDHCPnStopService();
extern void clearDHCPproperty();
bool isVoiceEnabled (void);
extern char *slstrncpy(char *destp, const char *srcp, unsigned long length);
extern void activateNetworkInterface(bool bEnable);

/* SWI_TBD
   This function is not implemented in swiril_misc.c, but instead has separate
   implementations in different files for QMI and AT RIL.  The declaration has
   to be visible by both RILs so put it here.  Revisit this later to see if 
   there is a better way to organize this function.
 */
extern bool isDualModeModule(void);
extern bool isDualModeRunningLTE(void);
extern bool isFirmwareRunningCDMA(void);
bool isDataSessionCloseByClient(bool state);
extern bool isDataSessionActive(void);

