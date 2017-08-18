/* -------------------------------------------------------------------------
 Copyright (C) 2014 Intel Mobile Communications GmbH

      Sec Class: Intel Confidential (IC)
 ----------------------------------------------------------------------
 Revision Information:
    $File name:  /msw_tools/FlashTool/libFlsTool/src/Timer.h $
    $CC-Version: .../dhkristx_140314_v0/1 $
    $Date:       2014-04-16    12:25:02 UTC $
 ----------------------------------------------------------------------
 by CCCT (0.12h)
 ---------------------------------------------------------------------- */

#ifndef _TIMER_H
#define _TIMER_H

#include <string>

#include <time.h>
#include <stdint.h>

#ifdef WIN32
  #include <windows.h>
#endif

using namespace std;

namespace ipa
{
  //< strftime("%c") - Same as ctime()
  #define DEFAULT_TIMESTAMP  "%c"

  class Timer
  {
  public:
    Timer();
    ~Timer() {};

    void Reset();
    uint32_t Stop();
    uint32_t GetTime(bool stop = false);

    string GetTimeStr();
    string GetStartDateTime();
    string GetEndDateTime();

  private:
    bool stopped;
    const char* format;
    uint32_t elapsed_ms;
    #ifdef WIN32
      SYSTEMTIME realtime;
      DWORD starttime, endtime;
    #else
      struct timespec starttime, endtime, realtime;
    #endif
  };
}

#endif
