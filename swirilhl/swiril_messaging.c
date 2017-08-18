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

#include <telephony/ril.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "at_channel.h"
#include "at_tok.h"
#include "swiril_main.h"
#include "swiril_messaging.h"

#define LOG_TAG "RIL"
#include "swiril_log.h"

static char s_outstanding_acknowledge = 0;

#define OUTSTANDING_SMS    0
#define OUTSTANDING_STATUS 1

struct held_pdu {
    char type;
    char *sms_pdu;
    struct held_pdu *next;
};

static pthread_mutex_t s_held_pdus_mutex = PTHREAD_MUTEX_INITIALIZER;
static struct held_pdu *s_held_pdus = NULL;

static struct held_pdu *dequeue_held_pdu()
{
    struct held_pdu *hpdu = NULL;

    if (s_held_pdus != NULL) {
        hpdu = s_held_pdus;
        s_held_pdus = hpdu->next;
        hpdu->next = NULL;
    }
    return hpdu;
}

static void enqueue_held_pdu(char type, const char *sms_pdu)
{
    struct held_pdu *hpdu = malloc(sizeof(*hpdu));

    memset(hpdu, 0, sizeof(*hpdu));
    hpdu->type = type;
    hpdu->sms_pdu = strdup(sms_pdu);

    if (s_held_pdus == NULL)
       s_held_pdus = hpdu; 
    else {
        struct held_pdu *p = s_held_pdus;
        while (p->next != NULL)
            p = p->next;

        p->next = hpdu;
    }
}

void onNewSms(const char *sms_pdu)
{
    pthread_mutex_lock(&s_held_pdus_mutex);

    if (s_outstanding_acknowledge) {
        LOGI("Waiting for ack for previous sms, enqueueing PDU.");
        enqueue_held_pdu(OUTSTANDING_SMS, sms_pdu);
    } else {
        s_outstanding_acknowledge = 1;
        RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_NEW_SMS,
                                  sms_pdu, strlen(sms_pdu));
    }
    pthread_mutex_unlock(&s_held_pdus_mutex);
}

void onNewStatusReport(const char *sms_pdu)
{
    char *response = NULL;
    pthread_mutex_lock(&s_held_pdus_mutex);
    if (s_outstanding_acknowledge) {
        LOGE("Waiting for previous ack, enqueueing PDU..");
        enqueue_held_pdu(OUTSTANDING_STATUS, sms_pdu);
    } else {
        s_outstanding_acknowledge = 1;
        RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT,
                sms_pdu, strlen(sms_pdu));
    }
    pthread_mutex_unlock(&s_held_pdus_mutex);
}

void onNewSmsOnSIM(const char *s)
{
    char *line;
    char *mem;
    char *tok;
    int err = 0;
    int index = -1;

    tok = line = strdup(s);

    err = at_tok_start(&tok);
    if (err < 0)
        goto error;

    err = at_tok_nextstr(&tok, &mem);
    if (err < 0)
        goto error;

    if (strncmp(mem, "SM", 2) != 0)
        goto error;

    err = at_tok_nextint(&tok, &index);
    if (err < 0)
        goto error;

    /* framework SMS index starts from 1, modem index starts from 0 */
    index ++;
    RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_NEW_SMS_ON_SIM, 
                              &index, sizeof(int *));

finally:
    free(line);
    return;

error:
    LOGE("Failed to parse +CMTI.");
    goto finally;
}

/**
 * RIL_REQUEST_SEND_SMS
 * 
 * Sends an SMS message.
 */
void requestSendSMS(void *data, size_t datalen, RIL_Token t)
{
    int err;
    const char *smsc;
    const char *pdu;
    char *line;
    int tpLayerLength;
    char *cmd1, *cmd2;
    RIL_SMS_Response response;
    RIL_Errno ret = RIL_E_SUCCESS;
    ATResponse *atresponse = NULL;

    smsc = ((const char **) data)[0];
    pdu = ((const char **) data)[1];

    tpLayerLength = strlen(pdu) / 2;

    /* NULL for default SMSC. */
    if (smsc == NULL) {
        smsc = "00";
    }

    asprintf(&cmd1, "AT+CMGS=%d", tpLayerLength);
    asprintf(&cmd2, "%s%s", smsc, pdu);

    err = at_send_command_sms(cmd1, cmd2, "+CMGS:", &atresponse);
    free(cmd1);
    free(cmd2);

    if (err != 0 || atresponse->success == 0)
        goto error;

    memset(&response, 0, sizeof(response));
    line = atresponse->p_intermediates->line;

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &response.messageRef);
    if (err < 0)
        goto error;

    response.ackPDU = NULL;
    response.errorCode = -1;
    RIL_onRequestComplete(t, RIL_E_SUCCESS, &response, sizeof(response));

finally:
    at_response_free(atresponse);
    return;

error:
    switch (at_get_cms_error(atresponse)) {
    case 332:
    case 331:
        ret = RIL_E_SMS_SEND_FAIL_RETRY;
        break;
    default:
        ret = RIL_E_GENERIC_FAILURE;
        break;
    }
    RIL_onRequestComplete(t, ret, NULL, 0);
    goto finally;
}




void requestSendSMS_HL7(void *data, size_t datalen, RIL_Token t)
{
    int err;
    const char *smsc;
    const char *pdu;
    char *line;
    int tpLayerLength;
    char *cmd1, *cmd2;
    RIL_SMS_Response response;
    RIL_Errno ret = RIL_E_SUCCESS;
    ATResponse *atresponse = NULL;

    smsc = ((const char **) data)[0];
    pdu = ((const char **) data)[1];

    tpLayerLength = strlen(pdu) / 2;

    /* NULL for default SMSC. */
    if (smsc == NULL) {
        smsc = "00";
    }

    asprintf(&cmd1, "AT+CMGS=%d", tpLayerLength);
    asprintf(&cmd2, "%s%s", smsc, pdu);

    /* either REPI: or RAT, added special handle in processLine */
    err = at_send_command_sms(cmd1, cmd2, "REPI:", &atresponse);
    free(cmd1);
    free(cmd2);

    if (err != 0 || atresponse->success == 0)
    {
        LOGE("err = %d ", err);
        goto error;
    }
    memset(&response, 0, sizeof(response));
    line = atresponse->p_intermediates->line;

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    response.ackPDU = NULL;
    response.errorCode = -1;
    RIL_onRequestComplete(t, RIL_E_SUCCESS, &response, sizeof(response));

finally:
    at_response_free(atresponse);
    return;

error:
    switch (at_get_cms_error(atresponse)) {
    case 332:
    case 331:
        ret = RIL_E_SMS_SEND_FAIL_RETRY;
        break;
    default:
        ret = RIL_E_GENERIC_FAILURE;
        break;
    }
    RIL_onRequestComplete(t, ret, NULL, 0);
    goto finally;
}

#ifdef SWI_RIL_VERSION_9
void requestIMSSendSMS(void *data, size_t datalen, RIL_Token t)
{
    RIL_IMS_SMS_Message *p_args;
    RIL_SMS_Response response;

    LOGD("%s:: entered\n", __func__);
    memset(&response, 0, sizeof(response));

    // figure out if this is gsm/cdma format
    // then route it to requestSendSMS vs requestCdmaSendSMS respectively
    p_args = (RIL_IMS_SMS_Message *)data;

    if (RADIO_TECH_3GPP == p_args->tech) {
        LOGD("%s:: RADIO_TECH_3GPP\n", __func__);
        requestSendSMS_HL7(p_args->message.gsmMessage, datalen - sizeof(RIL_RadioTechnologyFamily),t );
        return;
    }
    else if (RADIO_TECH_3GPP2 == p_args->tech)
        LOGD("%s:: RADIO_TECH_3GPP2 not supported\n", __func__);
    else
        LOGE("%s:: invalid format value =%d", __func__, p_args->tech);

    response.messageRef = -2;
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, &response, sizeof(response));
    LOGD("%s:: GENERIC_FAILURE\n", __func__ );
    return;
}
#endif

/**
 * RIL_REQUEST_SMS_ACKNOWLEDGE
 *
 * Acknowledge successful or failed receipt of SMS previously indicated
 * via RIL_UNSOL_RESPONSE_NEW_SMS .
*/
void requestSMSAcknowledge(void *data, size_t datalen, RIL_Token t)
{
    struct held_pdu *hpdu;

    pthread_mutex_lock(&s_held_pdus_mutex);

    hpdu = dequeue_held_pdu();

    if (hpdu != NULL) {
        LOGE("Outstanding requests in queue, dequeueing and sending.");
        int unsolResponse = 0;

        if (hpdu->type == OUTSTANDING_SMS)
            unsolResponse = RIL_UNSOL_RESPONSE_NEW_SMS;
        else
            unsolResponse = RIL_UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT;

        RIL_onUnsolicitedResponse(unsolResponse, hpdu->sms_pdu,
                                  strlen(hpdu->sms_pdu));

        free(hpdu->sms_pdu);
        free(hpdu);
    } else
        s_outstanding_acknowledge = 0;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

    pthread_mutex_unlock(&s_held_pdus_mutex);
}

/**
 * RIL_REQUEST_WRITE_SMS_TO_SIM
 *
 * Stores a SMS message to SIM memory.
 */
void requestWriteSmsToSim(void *data, size_t datalen, RIL_Token t)
{
    RIL_SMS_WriteArgs *args;
    char *cmd;
    char *pdu;
    char *line;
    int length;
    int index;
    int err;
    ATResponse *atresponse = NULL;

    args = (RIL_SMS_WriteArgs *) data;

    length = strlen(args->pdu) / 2;
    asprintf(&cmd, "AT+CMGW=%d,%d", length, args->status);
    asprintf(&pdu, "%s%s", (args->smsc ? args->smsc : "00"), args->pdu);

    err = at_send_command_sms(cmd, pdu, "+CMGW:", &atresponse);
    free(cmd);
    free(pdu);

    if (err < 0 || atresponse->success == 0)
        goto error;

    if (atresponse->p_intermediates->line == NULL)
        goto error;

    line = atresponse->p_intermediates->line;

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &index);
    if (err < 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, &index, sizeof(int *));

finally:
    at_response_free(atresponse);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_DELETE_SMS_ON_SIM
 *
 * Deletes a SMS message from SIM memory.
 */
void requestDeleteSmsOnSim(void *data, size_t datalen, RIL_Token t)
{
    char *cmd;
    ATResponse *atresponse = NULL;
    int err;

    asprintf(&cmd, "AT+CMGD=%d", ((int *) data)[0] - 1);
    err = at_send_command(cmd, &atresponse);
    free(cmd);
    if (err < 0 || atresponse->success == 0) {
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    } else {
        RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    }
    at_response_free(atresponse);
    return;
}

/**
 * RIL_REQUEST_GET_SMSC_ADDRESS
 */
void requestGetSMSCAddress(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err;
    char *line;
    char *response;

    err = at_send_command_singleline("AT+CSCA?", "+CSCA:", &atresponse);

    if (err < 0 || atresponse->success == 0)
        goto error;

    line = atresponse->p_intermediates->line;

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    err = at_tok_nextstr(&line, &response);
    if (err < 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, response, sizeof(char *));

finally:
    at_response_free(atresponse);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_SET_SMSC_ADDRESS
 */
void requestSetSMSCAddress(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err;
    char *cmd;
    const char *smsc = ((const char *)data);

    asprintf(&cmd, "AT+CSCA=\"%s\"", smsc);
    err = at_send_command(cmd, &atresponse);
    free(cmd);
    if (err < 0 || atresponse->success == 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

finally:
    at_response_free(atresponse);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}
