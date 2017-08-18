/* -------------------------------------------------------------------------
 Copyright (C) 2014 Intel Mobile Communications GmbH

      Sec Class: Intel Confidential (IC)
 ----------------------------------------------------------------------
 Revision Information:
    $File name:  /msw_tools/FlashTool/libFlsTool/src/Timer.cpp $
    $CC-Version: .../dhkristx_140314_v0/1 $
    $Date:       2014-04-16    12:24:59 UTC $
 ----------------------------------------------------------------------
 by CCCT (0.12h)
 ---------------------------------------------------------------------- */

#include "Timer.h"

#include <stdio.h>   //< snprintf()
#include <sstream>
#include <iomanip>

using namespace std;

namespace ipa
{

/**
 *  Creates a timer & starts/resets it
 */
Timer::Timer()
  : format(DEFAULT_TIMESTAMP)
// : format("%Y-%m-%d, %H:%M:%S (%A)")
{
  Reset();
}

//---------------------------------------------------------------------------

/**
 *  Resetting the timer
 */
void Timer::Reset()
{
  stopped = false;
  elapsed_ms = 0;
  #ifdef WIN32
    starttime = GetTickCount();
    GetSystemTime(&realtime);
  #else
    // time(&starttime);                        //< Gives secs, but uses "Wall clock"
    // gettimeofday(&starttime, NULL);          //< Gives ms, but uses "Wall clock"
    clock_gettime(CLOCK_MONOTONIC, &starttime); //< Needs -lrt compiler flag
    clock_gettime(CLOCK_REALTIME, &realtime);
  #endif
}

//---------------------------------------------------------------------------

/**
 * Stops the timer & calculates the elapsed ms.
 * Subsequent calls to this function will do nothing
 *
 * @return The elapsed ms
 */
uint32_t Timer::Stop()
{
  stopped = true;
  #ifdef WIN32
    endtime = GetTickCount();
  #else
    clock_gettime(CLOCK_MONOTONIC, &endtime);
  #endif
  return GetTime(false);
}

/**
 * Calculates the time
 */
uint32_t Timer::GetTime(bool stop)
{
  if (!stopped)
  {
    Stop();
    stopped = stop;
  }

  #ifdef WIN32
    if(endtime < starttime) // Rollover
     endtime += 0xFFFFFFFF;
    // return (0xFFFFFFFF - starttime) + endtime; // Previous

    elapsed_ms = endtime - starttime;
  #else
    if (endtime.tv_nsec < starttime.tv_nsec)
      endtime.tv_nsec += 1000000000;

    struct timespec tm;
    tm.tv_sec = endtime.tv_sec - starttime.tv_sec;
    tm.tv_nsec = endtime.tv_nsec - starttime.tv_nsec;

    elapsed_ms = (tm.tv_sec*1000 + int(tm.tv_nsec/1000000));
  #endif

  return elapsed_ms;
}

//---------------------------------------------------------------------------

#ifdef WIN32
static const char* days[]   = { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };
static const char* months[] = { "Jan", "Feb", "Mar", "Apr", "Aug", "Sep", "Oct", "Nov", "Dec" };

static string systime_format(SYSTEMTIME st)
{
  char buff[80];
  snprintf(buff, sizeof(buff), "%s %s %3d %02d:%02d:%02d %d\n",
           months[st.wMonth], days[st.wDayOfWeek], st.wDayOfWeek,
           st.wHour, st.wMinute, st.wSecond, st.wYear);

  return buff;
}

SYSTEMTIME systime_add(SYSTEMTIME st, uint64_t ms)
{
  FILETIME ft;
  ULARGE_INTEGER u;

  SystemTimeToFileTime(&st, &ft);

  memcpy(&u , &ft, sizeof(u));
  u.QuadPart += ms / 100.*1.e-6;
  memcpy(&ft, &u, sizeof(ft));

  FileTimeToSystemTime(&ft, &st);
  return st;
}
#else

string systime_format(time_t* st, const char* format)
{
  char buff[80] = { 0 };
  struct tm *timeinfo = localtime(st);
  if (timeinfo)
    strftime(buff, sizeof(buff), format, timeinfo);
  return buff;
}
#endif

/**
 *  Formats a string of the elapsed time - e.g. "02:10:123"
 *
 *  If no input time is provided it will stop the timer and
 *  return this. Otherwise it will just format the input time.
 *
 * @param ms  Input time in milliseconds
 *
 * @return    The formatted string
 */
string Timer::GetTimeStr()
{
  stringstream format;
  uint64_t sec, msec;
  uint32_t ms = GetTime();

  sec = ms/1000;
  msec = ms%1000;

  int hr = int(sec/60/60);
  sec -= hr*60*60;

  int min = int(sec/60);
  sec -= min*60;

  while (msec > 1000) // Reducing precision to 3 decimals
    msec /= 10;

  format.fill('0');
  format << setw(2) << min << "m"
         << setw(2) << sec << "."
         << setw(3) << msec << "s";

  return format.str();
}

/**
 *  Returns the start time in a readable format
 *
 *  Default Windows: "M:S:MS"
 *  Default Format: "Day Mon  D H:M:S YEAR"
 *        Previous: "%Y-%m-%d, %H:%M:%S (%A)"
 *
 * @return Formatted output of starttime
 */
string Timer::GetStartDateTime()
{
  #ifdef WIN32
    return systime_format(realtime);
  #else
    return systime_format(&realtime.tv_sec, format);
  #endif
}

/**
 *  Returns the end time in a readable format
 *
 *  Default Windows: "M:S:MS"
 *    Default Linux: "Day Mon  D H:M:S YEAR"
 *         Previous: "%Y-%m-%d, %H:%M:%S (%A)"
 *
 * @return Formatted output of endtime
 */
string Timer::GetEndDateTime()
{
  #ifdef WIN32
    SYSTEMTIME st = systime_add(realtime, elapsed_ms);
    return systime_format(st);
  #else
    time_t real_endtime = realtime.tv_sec + (endtime.tv_sec - starttime.tv_sec);
    return systime_format(&real_endtime, format);
  #endif
}

} //< namespace ipa

