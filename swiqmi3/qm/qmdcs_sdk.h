/*************
 *
 * Filename: qmdcs_sdk.h
 *
 * Purpose:  QMI Device Connectivity Service user types
 *
 * Copyright: © 2012 Sierra Wireless Inc., all rights reserved
 *
 **************/

#ifndef QMDCS_SDK_H
#define QMDCS_SDK_H

#include "os/swi_ossdk.h"
#include "qm/qmudefs.h"
#include "mm/mmudefs.h"
#include "ic/icudefs.h"

#define DCSMSGDEPTH  30
#define DCSEVTPOOLSZ 10

/*************
 *
 * Name:    qmdcsevtblock
 *
 * Purpose: Structure used to pass information into the QMI
 *          Request/Response task.
 *
 * Members: dcsmsgtype - Indicates the DCS message type
 *
 * Notes:   None
 *
 **************/
struct qmdcsevtblock
{
    swi_uint8  *qmdcsevtdatap;
    swi_uint8  *qmdcsevtmemfreep;
    swi_uint8  qmdcsevtipcchan;
    swi_uint16 xactionid;
    swi_uint8  instance;
};

/*************
 *
 * Name:    qmdcstcb
 *
 * Purpose: QM DCS Task control block structure
 *
 * Members: qmdcsdlcb      -   DL package control block for logging
 *          qmdcsicmsgque      IC SMT Message queue
 *          qmdcsevtpool   -   MM pool structure. Contains buffers used
 *                          to store information about QMI Reponses.
 *                          Groups several arguments that can be referenced
 *                          by a single handle
 *          qmdcstxpool        - qm task request large buffer pool
 *
 * Notes:   none
 *
 **************/
struct qmdcstcb
{
    struct dlusercb qmdcsdlcb;
    struct icsmtcb  qmdcsicmsgque;
    struct mmpool   qmdcsevtpool;
    swi_uint8       qmdcsipcchannel;
    swi_uint16      xactionid;
    struct swi_oswaiter qlenWaiter;
    //TODO maximum as macro
    swi_uint32      qlenPeriod[8];
    swi_uint32      qlenHiMark[8];
    swi_uint32      qlenLoMark[8];
    pthread_t       tid[8];
};

package struct qmdcstcb *qmdcsgetcbp(void);
package swi_bool qmIsAppRegistered(swi_uint8 channel, swi_uint8 ctlflgs);

#endif /* QMDCS_SDK_H */

