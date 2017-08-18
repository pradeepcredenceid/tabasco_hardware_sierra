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
 *  $File name:  /msw_tools/FlashTool/libDownloadTool/src/OS_dependent_code/timer_if/IFWD_timer.c $
 *  $CC-Version: .../dhkristx_140314_v0/1 $
 *  $Date:       2014-04-23    10:53:24 UTC $
 *   Responsible: bentzen
 *   Comment:
 *     SMS00847521: "Need to remove Lint warnings..."
 */
#include "OS.h"
#include "IFWD_timer.h"
#include "IFWD_mem.h"

/*****************************************************************************/
/* Constant definitions */

#define TIME_RESOLUTION                     1  /* in milliseconds */

/*****************************************************************************/
/* Type definitions */


/*****************************************************************************
 *                             LOCAL PROTOTYPES                              *
 *****************************************************************************/
#if 0  //< This function is added in OS.h
    #include <limits.h>
	#include <sys/time.h>

	static long GetTickCount( void )
	{
		struct timeval now;

		gettimeofday( &now, NULL );
		return(long)( now.tv_sec*1000 + now.tv_usec/1000 );
	}
#endif

/*****************************************************************************
 *                            PUBLIC PROTOTYPES                              *
 *****************************************************************************/
void TIMER_new_timer_secs(timer_event_timer_t *timer, uint32_t Time);
uint8_t TIMER_timer_expired (timer_event_timer_t *timer);
void TIMER_new_timer(timer_event_timer_t *timer, uint32_t Time);
void  TIMER_start_chronom_millisec (timer_chronom_t *chronom);
void  TIMER_start_chronom_sec (timer_chronom_t *chronom);

/***************************** IMPLEMENTATION ********************************/

const char* timestamp()
{
  static char* ts;
  if (!ts) ts = (char*)MEM_malloc(24);
  if (ts)
  {
    #ifdef _WIN32
      sprintf(ts, "%ld", GetTickCount());
    #else
      struct timeval now;
      gettimeofday( &now, NULL );
      sprintf(ts, "%ld.%03ld", now.tv_sec, now.tv_usec/1000);
    #endif
  }
  return ts;
}

/******************************************************************************
* Function:... TIMER_new_timer_secs(timer_event_timer_t *timer, uint32_t Time)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 23.01.05 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
void TIMER_new_timer_secs(timer_event_timer_t *timer, uint32_t Time) /* Time given in sec  */
{
  timer->start_time = GetTickCount();
  if (Time == 0)
    timer->expire_time = 0;
  else
  {
    timer->expire_time = (Time * 1000 / TIME_RESOLUTION + timer->start_time) % (uint32_t)ULONG_MAX;
    timer->overrun = (Time * 1000  / TIME_RESOLUTION + timer->start_time) / (uint32_t)ULONG_MAX;
  }
}


/******************************************************************************
* Function:... TIMER_timer_expired()
* Parameters:. EventTimer
* Returns:.... 1 if timeout, otherwise 0
* Description:
*
* Created:.... 20.09.01 by (AlanBugtrup, DWD / AB)
* Modified:... 20.09.01 by (Name, DWD / Initials)
*
******************************************************************************/
uint8_t TIMER_timer_expired (timer_event_timer_t *timer)
{
  uint32_t Time;
  if (timer->expire_time == 0)
    return 0;
  Time = GetTickCount();
  if ((Time > timer->expire_time) && (timer->overrun == 0))
    return 1;
  else
  if (((Time > timer->expire_time) && (Time < timer->start_time )) && (timer->overrun == 1))
    return 1;
  else
    return 0;
}

/******************************************************************************
* Function:... void  TIMER_stop_timer(timer_event_timer_t *timer)
* Parameters:. EventTimer
* Returns:....
* Description:
*
* Created:.... 20.09.01 by (AlanBugtrup, DWD / AB)
* Modified:... 20.09.01 by (Name, DWD / Initials)
*
******************************************************************************/
void  TIMER_stop_timer(timer_event_timer_t *timer)
{
  timer->expire_time = 0;
}

/******************************************************************************
* Function:... TIMER_new_timer(event_timer *´timer, uint32_t Time)
* Parameters:. EventTimer
* Returns:.... 1 if timeout, otherwise 0
* Description:
*
* Created:.... 07.02.02 by (AlanBugtrup, DWD / AB)
* Modified:... 07.02.02 by (Name, DWD / Initials)
*
******************************************************************************/
void TIMER_new_timer(timer_event_timer_t *timer, uint32_t Time)
{
  timer->start_time = GetTickCount() / TIME_RESOLUTION;
  if (Time == 0)
    timer->expire_time = 0;
  else
  {
    timer->expire_time = (Time + (timer->start_time) / TIME_RESOLUTION) % ULONG_MAX;
    timer->overrun = (Time  + (timer->start_time) / TIME_RESOLUTION) / ULONG_MAX;
  }
}


/******************************************************************************
* Function:... void  TIMER_start_chronom_millisec (timer_chronom_t *chronom)
* Parameters:.
* Returns:....
* Description:
*
* Created:.... 07.02.02 by (AlanBugtrup, DWD / AB)
* Modified:... 07.02.02 by (Name, DWD / Initials)
*
******************************************************************************/
void  TIMER_start_chronom_millisec (timer_chronom_t *chronom)
{
   chronom->start_time = GetTickCount();
}

/******************************************************************************
* Function:... void  TIMER_start_chronom_sec (timer_chronom_t *chronom)
* Parameters:.
* Returns:....
* Description:
*
* Created:.... 07.02.02 by (AlanBugtrup, DWD / AB)
* Modified:... 07.02.02 by (Name, DWD / Initials)
*
******************************************************************************/
void  TIMER_start_chronom_sec (timer_chronom_t *chronom)
{
   chronom->start_time = GetTickCount();
}

/******************************************************************************
* Function:... uint32_t  TIMER_stop_chronom_sec (timer_chronom_t *chronom)
* Parameters:.
* Returns:....
* Description:
*
* Created:.... 07.02.02 by (AlanBugtrup, DWD / AB)
* Modified:... 07.02.02 by (Name, DWD / Initials)
*
******************************************************************************/
uint32_t  TIMER_stop_chronom_sec (timer_chronom_t *chronom)
{
  chronom->stop_time = GetTickCount();

  if (GetTickCount() < chronom->start_time)
    chronom->time = ((ULONG_MAX - chronom->start_time) + GetTickCount()) * TIME_RESOLUTION;
  else
    chronom->time = (GetTickCount() - chronom->start_time) * TIME_RESOLUTION;

  return (chronom->time / 1000); /* From milliseconds to sec */
}

/******************************************************************************
* Function:... uint32_t  TIMER_get_time_elapsed_chronom_sec(timer_chronom_t *chronom)
* Parameters:.
* Returns:....
* Description:
*
* Created:.... 07.02.02 by (AlanBugtrup, DWD / AB)
* Modified:... 07.02.02 by (Name, DWD / Initials)
*
******************************************************************************/
#ifdef INCLUDE_DUT_USB_TEST
uint32_t  TIMER_get_time_elapsed_chronom_sec(timer_chronom_t *chronom)
{
  if (GetTickCount() < chronom->start_time)
    chronom->time = ((ULONG_MAX - chronom->start_time) + GetTickCount()) * TIME_RESOLUTION;
  else
    chronom->time = (GetTickCount() - chronom->start_time) * TIME_RESOLUTION;

  return (chronom->time / 1000); /* From milliseconds to sec */
}
#endif
