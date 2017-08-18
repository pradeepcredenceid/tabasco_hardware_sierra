/*************
 *
 * Filename:  qmiproto.h
 *
 * Purpose:   This file contains internal prototypes for the qm package
 *
 * Copyright: © 2011 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* include files */
#include "qm/qmdcs.h"
#include "qm/qmfms.h"

/* qmtask_sdk.c */
struct qmtcb *qmgetcbp(void);

void qmclearrespinfo(void);

void qmshortresp(
    enum eQCWWANError resultcode,
    swi_uint32 ipcchannel );

const struct qmicontrolpoint *qmiclientinfoget(
    enum qmisupportedclients client );

void qmconnect(swi_uint8 channel);
void qmdisconnect(swi_uint8 channel);

void qmqmirequestcancel(void);

enum qmisupportedclients qmisvc2qmiclnt_map(
    swi_uint8 instance,
    enum eQMIService svc );

void qm_ds_dev_notification_cb(
    enum ds_sio_event event );

void qmwdsclientset(
    enum qmisupportedclients client );

void qmSetActiveIPFamily(
    int activeHandle );

int qmGetActiveIPFamily( );

enum qmisupportedclients qmwdsclientget();

void qmQmiResponseLock();

void qmQmiResponseUnlock();

void qmSetStartTime(time_t start_t);
time_t qmGetStartTime(void);

swi_int32 qmGetFwDownloadStatus( void );

void qmSetFwDownloadStatus( swi_int32 fwdldstatus );


/* qmcommon.c */
enum eQCWWANError qmunpack(
    swi_uint8 *bufp,
    swi_uint8 *resp,
    struct qmTlvUnpackerItem *pItem,
    const swi_uint16 messageId );

/* qmdcscommon.c */
enum eQCWWANError
qm_dcs_enumerate_devices_response_unpack(
    swi_uint8 *preq,
    struct qm_qmi_response_tlvs_values *preqtlvs );

/* qmdcs.c */
void qm_dcs_handler(
    swi_uint8 *preq,
    swi_uint8 instance );

void qm_dcs_dev_state_change_notify(
     swi_uint8 devstate );

void qmdcsinit();

/* qmfms.c */
void qm_fms_handler(
    swi_uint8 *preq );

void qm_fms_fw_dwld_complete_notify(
    swi_int32 fwdldstatus );

void qmfmsinit();

/* qmparser.c */
package void qmparser(  swi_uint8 qmiinstanceid,
                        enum eQMIService svc,
                        swi_uint8* pmsg );
/* qmtask_sdk.c */
void qm_register_startup_clients();

/* qmtask_sdk.c */
void qm_register_startup_api_clients( swi_int8 srvcnt, swi_int8 mask );

package void qmderegisterclients(void);
