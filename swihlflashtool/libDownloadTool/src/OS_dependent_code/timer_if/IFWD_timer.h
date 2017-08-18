/*
 *       Copyright (C) 2011-2014 Intel Mobile Communications GmbH
 *
 *            Sec Class: Intel Confidential (IC)
 *
 *
 * This document contains proprietary information belonging to IMC.
 * Design A/S. Passing on and copying of this document, use and communication
 * of its contents is not permitted without prior written authorisation.
 *
 * Description:
 *   <hardcoded file description - the only part to be modified manually!>
 *
 * Revision Information:
 *  $File name:  /msw_tools/FlashTool/libDownloadTool/src/OS_dependent_code/timer_if/IFWD_timer.h $
 *  $CC-Version: .../oint_tools_flashtoole2_01/10 $
 *  $Date:       2014-03-13    15:41:46 UTC $
 *   Responsible: bentzen
 *   Comment:
 *     SMS00847521: "Need to remove Lint warnings..."
 */

#ifndef __TIMER_H__
#define __TIMER_H__

/****************************************************************************
 *                        STRUCTURES DEFINITIONS                            *
 ****************************************************************************/

typedef struct timer_event_timer_s
{
  uint32_t start_time;
  uint32_t expire_time;
  uint32_t overrun;
} timer_event_timer_t;

typedef struct timer_cronom_s
{
  uint32_t start_time;
  uint32_t stop_time;
  uint32_t time;
} timer_chronom_t;

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 * Function:     const char* timestamp();
 *
 *---------------------------------------------------------------------------
 * Purpose:    Created for debugging purposes
 *
 * Returns:    A Timestamp. milliseconds on Windows, and seconds.ms on Linux
 ******************************************************************************/
 const char* timestamp();

/****************************************************************************
 * Function:     void  New_timer_secs(timer_event_timer_t *timer, uint32_t time);
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function starts a timer with duration given in parameter
 *             "time". The duration is given i seconds
 *
 * Parameters:
 * IN
 * time:         gives the duration time in seconds
 *
 * IN/OUT
 * timer:        The function updates the field "start_time" to the current time
 *               and the field "expire_time" to the current_time + the duration
 *               time.
 *
 * Returns:      Nothing
 ******************************************************************************/
 void TIMER_new_timer_secs(timer_event_timer_t *timer, uint32_t time);

/****************************************************************************
 * Function:       void  New_timer(timer_event_timer_t *timer, uint32_t time);
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function starts a timer with duration given in parameter
 *             "time". The duration is given in milliseconds.
 *
 * Parameters:
 * IN
 * time:         gives the duration time in milliseconds
 *
 * IN/OUT
 * timer:        The function updates the field "start_time" to the current time
 *               and the field "expire_time" to the current_time + the duration
 *               time.
 *
 * Returns:      Nothing
 ******************************************************************************/
 void  TIMER_new_timer(timer_event_timer_t *timer, uint32_t time);


 /****************************************************************************
 * Function:        void  Stop_timer(timer_event_timer_t *timer);
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function stops an already started timer.
 *
 * Parameters:
 * IN            NONE
 *
 * IN/OUT
 * timer:        The function clear the field "start_time" and the field
 *               "expire_time".
 *
 * Returns:      Nothing
 ******************************************************************************/
 void  TIMER_stop_timer(timer_event_timer_t *timer);

/****************************************************************************
 * Function:     ubyte  TIMER_expired (timer_event_timer_t *timer);
 *
 *---------------------------------------------------------------------------
 * Purpose:      This function returns true (1) if the timer has expired.
 *               Otherwise, it returns false (0).
 *
 * Parameters:
 * IN
 * timer         Timer to check
 *
 * IN/OUT        NONE
 *
 * Returns:      true (1), if the timer has expired.
 *               false (0), if the timer has not expired.
 ******************************************************************************/
 uint8_t  TIMER_timer_expired (timer_event_timer_t *timer);


 /****************************************************************************
 * Function:     void  TIMER_start_chronom_millisec (timer_chronom_t *chronom);
 *
 *---------------------------------------------------------------------------
 * Purpose:      This function starts a chronometer in milliseconds.
 *
 * Parameters:
 * IN
 * timer         chronometer
 *
 * IN/OUT        NONE
 *
 * Returns:      Nothing
 *
 ******************************************************************************/
 void  TIMER_start_chronom_millisec (timer_chronom_t *chronom);

/****************************************************************************
 * Function:     void  TIMER_start_chronom_sec (timer_chronom_t *chronom);
 *
 *---------------------------------------------------------------------------
 * Purpose:      This function starts a chronometer in seconds.
 *
 * Parameters:
 * IN
 * timer         chronometer
 *
 * IN/OUT        NONE
 *
 * Returns:      Nothing
 *
 ******************************************************************************/
 void  TIMER_start_chronom_sec (timer_chronom_t *chronom);

/****************************************************************************
 * Function:     uint32_t  TIMER_stop_chronom_sec (timer_chronom_t *chronom);
 *
 *---------------------------------------------------------------------------
 * Purpose:      This function stops a chronometer.
 *
 * Parameters:
 * IN
 * timer         chronometer
 *
 * IN/OUT        NONE
 *
 * Returns:      Returns the time enlapsed (since chronometer start) in seconds
 *
 ******************************************************************************/
 uint32_t  TIMER_stop_chronom_sec (timer_chronom_t *chronom);

/****************************************************************************
 * Function:     uint32_t TIMER_get_time_elapsed_chronom_sec (timer_chronom_t *chronom);
 *
 *---------------------------------------------------------------------------
 * Purpose:      This function return the time elapsed since the chronometer
 *               started.
 *
 * Parameters:
 * IN
 * timer         chronometer
 *
 * IN/OUT        NONE
 *
 * Returns:      This function return the time elapsed since the chronometer
 *               started. The time returned is given in seconds.
 *
 ******************************************************************************/
#ifdef INCLUDE_DUT_USB_TEST
 uint32_t TIMER_get_time_elapsed_chronom_sec (timer_chronom_t *chronom);
#endif

#ifdef __cplusplus
}
#endif

#endif  /* from #ifndef __TIMER_H__ */
