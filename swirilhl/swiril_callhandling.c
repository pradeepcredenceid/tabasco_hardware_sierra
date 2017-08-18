/* 
** This source code is "Not a Contribution" under Apache license
**
** Sierra Wireless RIL
**
** Based on reference-ril by The Android Open Source Project
** and U300 RIL by ST-Ericsson.
** Modified by Sierra Wireless, Inc.
*
** Copyright (C) 2011 Sierra Wireless, Inc.
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

#include <stdio.h>
#include <string.h>
#include <telephony/ril.h>
#include <assert.h>
#include <stdbool.h>
#include "at_channel.h"
#include "at_tok.h"
#include "swiril_main.h"
#include "swiril_callhandling.h"
#include "swiril_misc.h"

#define LOG_TAG "RIL"
#include "swiril_log.h"

#define MAX_CALLS 8
typedef struct {
    bool bCallValid;
    int currentCallState;
    int callId;
} currentCallsState;
    
typedef struct {
    int waitingCalls;
    int activeCalls;
    int holdCalls;
    currentCallsState allCallsState[MAX_CALLS];
} currentCallsInfo;    
 
int getHangupCondition(currentCallsInfo *pAllCallsInfo);
/** 
 * Enumaration for call state management to send
 * RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED notification to the application layer
 */ 
typedef enum {
    CALL_ACTIVE    = 0,
    CALL_HOLDING   = 1,
    CALL_DIALING   = 2,  /* MO call only */
    CALL_ALERTING  = 3,  /* MO call only */
    CALL_INCOMING  = 4,  /* MT call only */
    CALL_WAITING   = 5,  /* MT call only */
    CALL_NO_ACTIVE = 6,  /* No active call */
} CallState;

/**
 * Voice call cache list is generated from this structure. The calls gets added
 * and deleted into this list based on the information obtained from the CLCC 
 * information. The list is created on a MO or MT call, and gets added up based
 * on conference or multi-party calls. The list is empty when there is no
 * current call in progress.
 */
typedef struct callState{
    int       callID;           /* call connection ID */
    CallState state;            /* call state information */
    bool      isChecked;        /* to know list element has to be deleted */
    struct    callState *next;  /* pointer for the linked list */
} Voice_Call_State;

/**
 * Pointers to the call state cache list, the s_firstCallState always points to
 * beginning of the list and the s_currentCallState points to the cache call
 * state currently being updated
 */
static Voice_Call_State *s_firstCallState, *s_currentCallState;

/* TBD: Last call fail cause */
static int s_lastCallFailCause = CALL_FAIL_ERROR_UNSPECIFIED;
static int s_currentMuteValue = 0;

/* Timer which is used to poll the call state change (1 sec) */
static const struct timeval callStateChangePollingTimer = { 1, 0 };

/* Variable to track whether the call state polling has been started */ 
static bool isCallStatePollingStarted = false;

static int clccStateToRILState(int state, RIL_CallState * p_state)
{
    switch (state) {
    case 0:
        *p_state = RIL_CALL_ACTIVE;
        return 0;
    case 1:
        *p_state = RIL_CALL_HOLDING;
        return 0;
    case 2:
        *p_state = RIL_CALL_DIALING;
        return 0;
    case 3:
        *p_state = RIL_CALL_ALERTING;
        return 0;
    case 4:
        *p_state = RIL_CALL_INCOMING;
        return 0;
    case 5:
        *p_state = RIL_CALL_WAITING;
        return 0;
    default:
        return -1;
    }
}

/**
 * Note: Directly modified line and has *p_call point directly into
 * modified line.
 */
static int callFromCLCCLine(char *line, RIL_Call * p_call)
{
    /* +CLCC: 1,0,2,0,0,\"+18005551212\",145
       index,isMT,state,mode,isMpty(,number,TOA)? */
    int err;
    int state;
    int mode;

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &(p_call->index));
    if (err < 0)
        goto error;

    err = at_tok_nextbool(&line, &(p_call->isMT));
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &state);
    if (err < 0)
        goto error;

    err = clccStateToRILState(state, &(p_call->state));
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &mode);
    if (err < 0)
        goto error;

    p_call->isVoice = (mode == 0);

    err = at_tok_nextbool(&line, &(p_call->isMpty));
    if (err < 0)
        goto error;

    if (at_tok_hasmore(&line)) {
        err = at_tok_nextstr(&line, &(p_call->number));

        /* Tolerate null here */
        if (err < 0)
            return 0;

        err = at_tok_nextint(&line, &p_call->toa);
        if (err < 0)
            goto error;
    }

    return 0;

error:
    LOGE("invalid CLCC line\n");
    return -1;
}

/**
 * The call state cache pointers are initialized
 *
 * @param 
 *     none
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 */
static void initializeCallCache()
{
    /* Initialize the pointers for the call cache */
    s_firstCallState = s_currentCallState = NULL;
}

/**
 * The call state cache list, if present, is freed.
 *
 * @param 
 *     none
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 */
static void freeCallCache()
{
    Voice_Call_State *temp;

    /* Remove any elements in the call cache and exit */
    while(s_firstCallState != NULL) {
        temp = s_firstCallState->next;
        free(s_firstCallState);
        s_firstCallState = temp;
    }
    s_currentCallState = NULL;
}

/**
 * The call state polling is started. Call cache pointers are initialised.
 *
 * @param 
 *     none
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 */
void startCallStatePolling()
{
    /**
     * If the call state polling has not been started, then start the polling
     * and also send the usolicited response for the call state change.
     */ 
    if (!isCallStatePollingStarted) {
        initializeCallCache();
        isCallStatePollingStarted = true;
        RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED,
                                  NULL,
                                  0);
        enqueueRILEvent(RIL_EVENT_QUEUE_PRIO,
                        pollCallState,
                        NULL,
                        &callStateChangePollingTimer);
    }
}

/**
 * The call state polling is terminated. Any call state cache is present 
 * is cleared
 *
 * @param 
 *     none
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 */
void stopCallStatePolling()
{
    /* Do not poll for the call state again */ 
    if (isCallStatePollingStarted) {
        freeCallCache();
        isCallStatePollingStarted = false;
    }
}

/**
 * The call state cache list is searched to find out any calls which has been 
 * terminated. This has to be done once the current calls has been updated in 
 * the cache. Any terminated call is removed from the list
 *
 * @param 
 *     none
 *
 * @return
 *     true if atleast a call has been terminated, false otherwise
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 */
static bool checkforCallTerminated()
{
    bool callTerminated = false;
    Voice_Call_State *cur, *prev;

    /**
     * The cur and prev pointers to the list on the current and previous nodes
     * These pointers are used to delete a node from the list. The prev is
     * linked to the next of cur and cur is deleted whenever the node is found 
     * which is a "terminated call"
     */
    cur = prev = s_firstCallState;

    /** 
     * Free the node which has not been checked, meaning the CLCC did not 
     * update this node. This call has been terminated and has to be removed
     * from the list
     */
    while (cur != NULL) {
        /* This node has to be removed as it is not present in CLCC info */
        if (!cur->isChecked) {
            if (cur == s_firstCallState) {
                s_firstCallState = cur->next;
                prev = s_firstCallState;
                free(cur);
                cur = prev;
            } else {
                prev->next = cur->next;
                free(cur);
                cur = prev->next;
            }
            callTerminated = true;
        /* This node should not be removed as this is present in CLCC info */
        } else {
            if (cur == prev) {
                cur = cur->next;
            } else {
                cur = cur->next;
                prev = prev->next;
            }
        }      
    }
    return callTerminated;
}

/**
 * The call which has been found in the CLCC info and cache call list will be 
 * updated with the new state
 *
 * @param [in] currentCall
 *     the current call information obtained from CLCC, this would be specific
 *     to a call ID
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     This function should be invoked when the checkCallStateChange() function 
 *     returns true.
 */
static void updateCallState( RIL_Call *currentCall )
{
    s_currentCallState->state     = currentCall->state;
    s_currentCallState->isChecked = true;
}

/**
 * The cache call list is searched to find out whether the current call is
 * present in the call cache list
 *
 * @param [in] currentCall
 *     the current call information obtained from CLCC, this would be specific
 *     to a call ID
 *
 * @return
 *     true if there is a call state change, false otherwise
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     This function should be invoked when the checkCallPresent() function 
 *     returns true.
 */
static bool checkCallStateChange( RIL_Call *currentCall )
{
    /**
     * Check in the cache call list array to find out whether the call state
     * has changed from the previous call 
     */
    if (s_currentCallState->state == currentCall->state) {
        s_currentCallState->isChecked = true;
        return false;
    }
    return true;
}

/**
 * The cache call list is updated with the current call. A node is added to the
 * call list, or the call list is created if this is the first entry.
 *
 * @param [in] currentCall
 *     the current call information obtained from CLCC, this would be specific
 *     to a call ID
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     This function should be invoked when the checkCallPresent() function 
 *     returns false.
 */
static void updateCallIndex( RIL_Call *currentCall )
{
    Voice_Call_State *temp = NULL;

    temp = (Voice_Call_State *) swimalloc(sizeof(Voice_Call_State),
                          "updateCallIndex: cannot allocate cache");

    /* Update the created cache with the current call list */
    temp->callID = currentCall->index;
    temp->state  = currentCall->state;
    temp->isChecked = true;
    temp->next   = NULL;

    /**
     * Insert the created call cache into the call cache list, create the list
     * if this is the first entry
     */
    if (s_firstCallState == NULL) {
        s_firstCallState = temp;
    } else {
        temp->next  = s_firstCallState->next;
        s_firstCallState->next = temp;
    }
}

/**
 * The cache call list is searched to find out whether the current call is
 * present in the call cache list.
 *
 * @param [in] currentCall
 *     the current call information obtained from CLCC, this would be specific
 *     to a call ID.
 *
 * @return
 *     true if call is present, false otherwise
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     This function updated the s_currentCallState value to the identified
 *     call ID in the call cache list. The checkCallStateChange() function can 
 *     be invoked to check for any change in state of the call, and 
 *     updateCallIndex() to add the call to the call cache
 */
static bool checkCallPresent( RIL_Call *currentCall )
{
    /**
     * Check in the cache call list to find out whether the call ID is present,
     * and if so return true and update the s_currentCallState pointer
     */
    Voice_Call_State *temp = s_firstCallState;
    while (temp != NULL) {
        if (temp->callID == currentCall->index) {
            s_currentCallState = temp;
            return true;
        }
        temp = temp->next;
    }
    return false;
}

/**
 * The cache call list updated to be in unchecked states. This is done prior to
 * an update with new call information. This would also let us know the
 * terminated call after the update.
 *
 * @param
 *     none
 *
 * @return
 *     true if call is present, false otherwise
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 */
static void setAllCallsUnchecked()
{
    Voice_Call_State *temp = s_firstCallState;

    while (temp != NULL) {
        temp->isChecked = false;
        temp = temp->next;
    }
}

/**
 * A MO or MT or multipart or conference call is in process. Poll to find the 
 * change in state of the call. Polling is terminated when there is no call.
 *
 * @param [in] param
 *     in this case param is NULL, as there is no parameters passed in
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 */
void pollCallState( void* param )
{
    int err;
    ATResponse *atresponse;
    ATLine *cursor;
    RIL_Call *calls;
    bool voiceCallPresent = false;
    bool callStateChanged = false;

    /**
     * Set all calls in unchecked state so that we could find the terminated 
     * call at the end
     */
    setAllCallsUnchecked();

    err = at_send_command_multiline("AT+CLCC", "+CLCC:", &atresponse);

    /**
     * The CLCC command fails soon after the connection of a MT call, for 3-4
     * times. Need to poll until success (until the call is really connected)
     */ 
    if (err != 0 || atresponse->success == 0) {
        LOGE("AT+CLCC Error : Poll again until success");
        enqueueRILEvent(RIL_EVENT_QUEUE_PRIO,
                        pollCallState,
                        NULL,
                        &callStateChangePollingTimer);
        return;
    }

    calls = (RIL_Call *) swimalloc(sizeof(RIL_Call),
                                   "pollCallState: cannot allocate cache");
    memset(calls, 0, sizeof(RIL_Call));

    /**
     * The current voice calls are searched and the state is compared with the
     * previously stored call state. If there is a change in state, or if there
     * are new calls or terminated calls notify the application layer
     */
    for (cursor = atresponse->p_intermediates;
         cursor != NULL;
         cursor = cursor->p_next) {
        err = callFromCLCCLine(cursor->line, calls);

        if (err != 0) {
            continue;
        }

        /**
         * Need to process only if this is a voice call; the call ID and the
         * call state need to be updated if this has changed and the upper layer
         * should be notified of a call state change.
         * Check whether the current call is present in the cache list, if not
         * add the call to the list. If the call is present and the state is not
         * the same update the state in the cache 
         */
        if (calls->isVoice) {
            voiceCallPresent = true;
            /* Call is present */
            if (checkCallPresent(calls)) {
                if (checkCallStateChange(calls)) {
                    updateCallState(calls);
                    callStateChanged = true;
                }
            /* Call is not present */
            } else {
                updateCallIndex(calls);
                callStateChanged = true;
            }
        }
    }
    /* Check for any call which has been terminated, and delete from list */ 
    if (checkforCallTerminated()) {
        callStateChanged = true;
    }

    /* Update the call state change response to the application layer */ 
    if (callStateChanged) {
        LOGD("Call State Changed");
        Voice_Call_State *temp = s_firstCallState;
        while (temp != NULL) {
            LOGD("Call ID::%d  Call State::%d  Call Checked::%d",
                 temp->callID,
                 temp->state,
                 temp->isChecked);
            temp = temp->next;
        }
        RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED,
                                  NULL,
                                  0);
    }

    /**
     * If there is no voice call present stop polling, otherwise enqueue the
     * the polling function 
     */
    if(voiceCallPresent)
        enqueueRILEvent(RIL_EVENT_QUEUE_PRIO,
                        pollCallState,
                        NULL,
                        &callStateChangePollingTimer);
    else
        stopCallStatePolling();
}

void setCurrentMuteValue( int mute )
{
    s_currentMuteValue = mute;
}

int getCurrentMuteValue( )
{
    return s_currentMuteValue;
}

/**
 * RIL_REQUEST_UDUB
 * Send UDUB (user determined used busy) to ringing or waiting call answer
 * (RIL_BasicRequest r).
 *
 * @param [in] data
 *     data is NULL
 * @param datalen
 *     length of the data parameter received
 * @param t
 *     token passed to the RIL command request
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 */
void requestUDUB(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err;

    err = at_send_command("ATH", &atresponse);
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

/**
 * RIL_REQUEST_SET_MUTE
 * Turn on or off uplink (microphone) mute.
 *
 * @param [in] data
 *     data is an "int *". (int *)data)[0] is 1 for "enable mute" 
 *     and 0 for "disable mute"
 * @param datalen
 *     length of the data parameter received
 * @param t
 *     token passed to the RIL command request
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     Will only be sent while voice call is active. Will always be reset to 
 *     "disable mute" when a new voice call is initiated.
 */
void requestSetMute(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int mute = ((int *) data)[0];
    int err = 0;
    char *cmd = NULL;

    if (!(mute == 0 || mute == 1))
        goto error;

    asprintf(&cmd, "AT+CMUT=%d", mute);
    at_send_command(cmd, &atresponse);
    free(cmd);
    if (err < 0 || atresponse->success == 0)
        goto error;

    setCurrentMuteValue(mute);
    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

finally:
    at_response_free(atresponse);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_GET_MUTE
 * Queries the current state of the uplink mute setting
 *
 * @param [in] data
 *     "data" is NULL
 * @param datalen
 *     length of the data parameter received
 * @param t
 *     token passed to the RIL command request
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 */
void requestGetMute(void *data, size_t datalen, RIL_Token t)
{
    int response = 0;

    response = getCurrentMuteValue();
    RIL_onRequestComplete(t, RIL_E_SUCCESS, &response, sizeof(int));
    return;

}
/**
 * RIL_REQUEST_LAST_CALL_FAIL_CAUSE
 * Requests the failure cause code for the most recently terminated call
 *
 * @param [in] data
 *     "data" is NULL
 * @param datalen
 *     length of the data parameter received
 * @param t
 *     token passed to the RIL command request
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     See also: RIL_REQUEST_LAST_PDP_FAIL_CAUSE
 */
void requestLastCallFailCause(void *data, size_t datalen, RIL_Token t)
{
    int err;
    ATResponse *atresponse;
    char* ptr = NULL;

    err = at_send_command_multiline("AT+CEER", "+CEER", &atresponse);

    if (err != 0 || atresponse->success == 0) {
        goto complete;     
    }
    
    ptr = strchr(atresponse->p_intermediates->line,':');
    
    /* The below error codes should be changed depending on the error codes returned by the SWI 8092 module. Presently, 
       the values have been taken from the 3GPP 24.008, but if the module provides some other strings as a response, then
       the values should be updated in the code below */
    if (ptr!=NULL)
    {
        ptr = ptr+1;
        if (strstr(ptr,"User Busy") !=NULL)
        {
             s_lastCallFailCause = 17;
        }
        else if (strstr(ptr,"No User Responding") !=NULL || strstr(ptr,"User alerting, no answer") !=NULL)
        {
             s_lastCallFailCause = 18;
        }
        else if (strstr(ptr,"Normal Call Clearing") !=NULL || strstr(ptr,"Network ended call") !=NULL)
        {
             s_lastCallFailCause = 16;
        }
        else if (strstr(ptr,"Call Rejected") !=NULL)
        {
             s_lastCallFailCause = 21;
        }
        else if (strstr(ptr,"Congestion") !=NULL)
        {
             s_lastCallFailCause = 42;
        }
        else
        {
            LOGE("Unhandled CEER string, %s, consider it normal", ptr);
            s_lastCallFailCause = 16;
        }
    }
    at_response_free(atresponse);

complete:
    RIL_onRequestComplete(t, RIL_E_SUCCESS, &s_lastCallFailCause,
                          sizeof(int));

}

/**
 * RIL_REQUEST_GET_CURRENT_CALLS
 * Requests current call list
 *
 * @param [in] data
 *     "data" is NULL
 * @param datalen
 *     length of the data parameter received
 * @param t
 *     token passed to the RIL command request
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 */
void requestGetCurrentCalls(void *data, size_t datalen, RIL_Token t)
{
    int err;
    ATResponse *atresponse;
    ATLine *cursor;
    int countCalls;
    int countValidCalls;
    RIL_Call *calls;
    RIL_Call **response;
    int i;

    err = at_send_command_multiline("AT+CLCC", "+CLCC:", &atresponse);

    if (err != 0 || atresponse->success == 0) {
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    /* Count the calls. */
    for (countCalls = 0, cursor = atresponse->p_intermediates;
         cursor != NULL; cursor = cursor->p_next) {
        countCalls++;
    }

    /* Yes, there's an array of pointers and then an array of structures. */
    response = (RIL_Call **) alloca(countCalls * sizeof(RIL_Call *));
    calls = (RIL_Call *) alloca(countCalls * sizeof(RIL_Call));
    memset(calls, 0, countCalls * sizeof(RIL_Call));

    /* Init the pointer array. */
    for (i = 0; i < countCalls; i++) {
        response[i] = &(calls[i]);
    }

    for (countValidCalls = 0, cursor = atresponse->p_intermediates;
         cursor != NULL; cursor = cursor->p_next) {
        err = callFromCLCCLine(cursor->line, calls + countValidCalls);

        if (err != 0) {
            continue;
        }

        countValidCalls++;
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, response,
                          countValidCalls * sizeof(RIL_Call *));

finally:
    at_response_free(atresponse);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_DIAL
 * Initiate voice call
 *
 * @param [in] data
 *     data is const RIL_Dial *
 * @param datalen
 *     length of the data parameter received
 * @param t
 *     token passed to the RIL command request
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 */
void requestDial(void *data, size_t datalen, RIL_Token t)
{
    RIL_Dial *dial;
    ATResponse *atresponse = NULL;
    char *cmd;
    const char *clir;
    int err;

    dial = (RIL_Dial *) data;

    switch (dial->clir) {
    case 1:
        clir = "I";
        break;                  /* Invocation */
    case 2:
        clir = "i";
        break;                  /* Suppression */
    default:
    case 0:
        clir = "";
        break;                  /* Subscription default */
    }

    asprintf(&cmd, "ATD%s%s;", dial->address, clir);

    err = at_send_command(cmd, &atresponse);

    free(cmd);

    if (err < 0 || atresponse->success == 0)
        goto error;

    /* Success or failure is ignored by the upper layer here,
       it will call GET_CURRENT_CALLS and determine success that way. */
    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

    /* Start the call state polling with 1 second polling interval */
    startCallStatePolling();

finally:
    at_response_free(atresponse);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_ANSWER
 * Answer incoming call
 *
 * @param [in] data
 *     "data" is NULL
 * @param datalen
 *     length of the data parameter received
 * @param t
 *     token passed to the RIL command request
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     Will not be called for WAITING calls.
 *     RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE will be used in this 
 *     case instead
 */
void requestAnswer(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err;

    err = at_send_command("ATA", &atresponse);

    if (err < 0 || atresponse->success == 0)
        goto error;

    /* Success or failure is ignored by the upper layer here,
       it will call GET_CURRENT_CALLS and determine success that way. */
    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

finally:
    at_response_free(atresponse);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_HANGUP
 * Hang up a specific line (like AT+CHLD=1x)
 *
 * @param [in] data
 *     data is an "int *"
 *     (int *)data)[0] contains Connection index (value of 'x' in CHLD above)
 * @param datalen
 *     length of the data parameter received
 * @param t
 *     token passed to the RIL command request
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 */
void requestHangup(void *data, size_t datalen, RIL_Token t)
{
    int cid;
    char *cmd = NULL;
    ATLine *cursor;
    int err;
    int i;
    int countCalls;
    ATResponse *atresponse;
    RIL_Call *calls;

    cid = ((int *) data)[0];

    /* 
     * Until we get some silver bullet AT-command that will kill whatever
     * call we have, we need to check what state we're in and act accordingly.
     *
     * TODO: Refactor this and merge with the other query to CLCC.
     */
    err = at_send_command_multiline("AT+CLCC", "+CLCC:", &atresponse);

    if (err != 0 || atresponse->success == 0) {        
        goto error;
    }

    /* Count the calls. */
    for (countCalls = 0, cursor = atresponse->p_intermediates;
         cursor != NULL; cursor = cursor->p_next) {
        countCalls++;
    }

    if (countCalls <= 0)
        goto error;

    calls = (RIL_Call *) alloca(countCalls * sizeof(RIL_Call));
    memset(calls, 0, countCalls * sizeof(RIL_Call));

    for (i = 0, cursor = atresponse->p_intermediates; cursor != NULL;
         cursor = cursor->p_next) {
        err = callFromCLCCLine(cursor->line, calls + i);

        if (err != 0) {
            continue;
        }

        if (calls[i].index == cid)
            break;

        i++;
    }

    at_response_free(atresponse);
    atresponse = NULL;

    /* We didn't find the call. Just drop the request and let android decide. */
    if (calls[i].index != cid)
        goto error;

    /* MO call with "dialing or alerting state are also hanged up with AT+CHLD=1x
     * 3GPP 22.030 6.5.5 "Releases a specific active call X" 
     */
    asprintf(&cmd, "AT+CHLD=1%d", cid);

    err = at_send_command(cmd, &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

finally:
    if (cmd)
        free(cmd);
    if (atresponse)
        at_response_free(atresponse);
    return;

error:
    /* Success or failure is ignored by the upper layer here,
       it will call GET_CURRENT_CALLS and determine success that way. */
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND
 * Hang up waiting or held (like AT+CHLD=0)
 *
 * @param [in] data
 *     "data" is NULL
 * @param datalen
 *     length of the data parameter received
 * @param t
 *     token passed to the RIL command request
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 */
void requestHangupWaitingOrBackground(void *data, size_t datalen,
                                      RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err;

    /* 3GPP 22.030 6.5.5
       "Releases all held calls or sets User Determined User Busy
        (UDUB) for a waiting call." */
    err = at_send_command("AT+CHLD=0", &atresponse);
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


/**
 * Store the information about all voice calls in a struct
 * Return 0 if error or no calls are found
 * Return currentCallsInfo struct
 *
 * @param
 *     currentCallsInfo struct
 *
 * @return
 *     0   if no waiting call
 *     currentCallsInfo struct
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 */
int getHangupCondition(currentCallsInfo *pAllCallsInfo)
{
    char *cmd = NULL;
    ATLine *cursor;
    int err;
    int countCalls;
    int i;    
    ATResponse *atresponse;
    RIL_Call *calls;

    err = at_send_command_multiline("AT+CLCC", "+CLCC:", &atresponse);

    if (err != 0 || atresponse->success == 0) {
        at_response_free(atresponse);
        return 0;
    }

    /* Count the calls. */
    for (countCalls = 0, cursor = atresponse->p_intermediates;
         cursor != NULL; cursor = cursor->p_next) {
        countCalls++;
    }

    if (countCalls <= 0) {
        at_response_free(atresponse);
        return 0;
    }

    calls = (RIL_Call *) alloca(countCalls * sizeof(RIL_Call));
    memset(calls, 0, countCalls * sizeof(RIL_Call));

    for (i = 0, cursor = atresponse->p_intermediates; cursor != NULL;
         cursor = cursor->p_next)
    {
        err = callFromCLCCLine(cursor->line, calls + i);
        if (err != 0) {
            continue;
        }        

        if (calls[i].isVoice)
        {
            pAllCallsInfo->allCallsState[i].bCallValid = true;
            pAllCallsInfo->allCallsState[i].currentCallState = calls[i].state;
            pAllCallsInfo->allCallsState[i].callId = calls[i].index;

            if( calls[i].state == RIL_CALL_ACTIVE)
                pAllCallsInfo->activeCalls++;
            else if( calls[i].state == RIL_CALL_WAITING)
                pAllCallsInfo->waitingCalls++;
            else if( calls[i].state == RIL_CALL_HOLDING)
                pAllCallsInfo->holdCalls++;
        }
        i++;
    }

    at_response_free(atresponse);
    atresponse = NULL;

    return 1;
}

/**
 * RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND
 * Hang up waiting or held (like AT+CHLD=1)
 *
 * @param [in] data
 *     "data" is NULL
 * @param datalen
 *     length of the data parameter received
 * @param t
 *     token passed to the RIL command request
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 */
void requestHangupForegroundResumeBackground(void *data, size_t datalen,
                                             RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err=0;
    currentCallsInfo allcallsInfo;
    int i;
    int ret = 0;

    char *cmd = NULL;
    /* 3GPP 22.030 6.5.5
       "Releases all active calls (if any exist) and accepts
        the other (held or waiting) call." */
    memset(&allcallsInfo,0,sizeof(currentCallsInfo));

    /* CUS73656: RIL handle different scenarios as :
     * - if AT+CLCC response is error anyhow or more than 1 call are put on hold (conference call)
     * AT+CHLD=1 is issued to hangup active calls and activate hold/wait calls
     * - For other cases, all active calls are hung up one by one AT+CHLD=1%x and if a hold call (if exist)
     * is activated. This scenario will not connect waiting call automatically
     */
    ret = getHangupCondition(&allcallsInfo);
    if ((!ret) || (allcallsInfo.holdCalls > 1)) {
        err = at_send_command("AT+CHLD=1", &atresponse);
        if (err < 0 || atresponse->success == 0)
            goto error;
        at_response_free(atresponse); 
    }
    else {
        /* hang up all active calls only, resume hold call if any */
        for(i=0; i<MAX_CALLS; i++) {
            if((allcallsInfo.allCallsState[i].bCallValid) && 
                (allcallsInfo.allCallsState[i].currentCallState == RIL_CALL_ACTIVE)) {
                asprintf(&cmd, "AT+CHLD=1%d", allcallsInfo.allCallsState[i].callId);
                err = at_send_command(cmd, &atresponse);
                free(cmd);
                if (err < 0 || atresponse->success == 0)
                    goto error;
                at_response_free(atresponse); 
             }
        }
        if (allcallsInfo.holdCalls == 1) {
            for(i=0; i<MAX_CALLS; i++) {
                if((allcallsInfo.allCallsState[i].bCallValid) && 
                    (allcallsInfo.allCallsState[i].currentCallState == RIL_CALL_HOLDING)) {
                    asprintf(&cmd, "AT+CHLD=2%d", allcallsInfo.allCallsState[i].callId);
                    err = at_send_command(cmd, &atresponse);
                    free(cmd);
                    if (err < 0 || atresponse->success == 0)
                        goto error;
                    at_response_free(atresponse);
                    break;
                } 
            }
        }
    }
    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

finally:
    return;
error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    at_response_free(atresponse);
    goto finally;
}

/**
 * RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE
 * Switch waiting or holding call and active call (like AT+CHLD=2)
 *
 * @param [in] data
 *     "data" is NULL
 * @param datalen
 *     length of the data parameter received
 * @param t
 *     token passed to the RIL command request
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 */
void requestSwitchWaitingOrHoldingAndActive(void *data, size_t datalen,
                                            RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err;

    /* 3GPP 22.030 6.5.5
       "Places all active calls (if any exist) on hold and accepts
        the other (held or waiting) call." */
    err = at_send_command("AT+CHLD=2", &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;

    /**
     * AT+CLCC will follow soon after for getting the status of the current
     * calls, and need a delay to make sure that state change has taken effect
     */ 
    sleep(1);
    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

finally:
    at_response_free(atresponse);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_CONFERENCE
 * Conference holding and active (like AT+CHLD=3)
 *
 * @param data
 *     "data" is NULL
 * @param datalen
 *     length of the data parameter received
 * @param t
 *     token passed to the RIL command request
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 */
void requestConference(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err;

    /* 3GPP 22.030 6.5.5
       "Adds a held call to the conversation." */
    err = at_send_command("AT+CHLD=3", &atresponse);
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

/**
 * RIL_REQUEST_SEPARATE_CONNECTION
 * Separate a party from a multiparty call placing the multiparty call
 * (less the specified party) on hold and leaving the specified party 
 * as the only other member of the current (active) call (Like AT+CHLD=2x)
 *
 * @param [in] data
 *     data is an "int *"
 *     (int *)data)[0] contains Connection index (value of 'x' in CHLD above)
 * @param datalen
 *     length of the data parameter received
 * @param t
 *     token passed to the RIL command request
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     See TS 22.084 1.3.8.2 (iii)
 *     TS 22.030 6.5.5 "Entering "2X followed by send"
 *     TS 27.007 "AT+CHLD=2x"
 */
void requestSeparateConnection(void *data, size_t datalen, RIL_Token t)
{
    char cmd[12];
    int party = ((int *) data)[0];
    int err;
    ATResponse *atresponse = NULL;

    /* Make sure that party is a single digit. */
    if (party < 1 || party > 9)
        goto error;

    sprintf(cmd, "AT+CHLD=2%d", party);
    err = at_send_command(cmd, &atresponse);
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

/**
 * RIL_REQUEST_EXPLICIT_CALL_TRANSFER
 * Connects the two calls and disconnects the subscriber from both calls
 *
 * @param [in] data
 *     "data" is NULL
 * @param datalen
 *     length of the data parameter received
 * @param t
 *     token passed to the RIL command request
 *
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 */
void requestExplicitCallTransfer(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err;

    /* 3GPP TS 22.091
       Connects the two calls and disconnects the subscriber from both calls. */
    err = at_send_command("AT+CHLD=4", &atresponse);
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

