/* -------------------------------------------------------------------------
 Copyright (C) 2013-2014 Intel Mobile Communications GmbH

      Sec Class: Intel Confidential (IC)
 ----------------------------------------------------------------------
 Revision Information:
    $File name:  /msw_tools/FlashTool/libFlsTool/src/Watchdog.cpp $
    $CC-Version: .../dhkristx_140314_v0/1 $
    $Date:       2014-04-16    12:25:05 UTC $
 ----------------------------------------------------------------------
 by CCCT (0.12l)
 ---------------------------------------------------------------------- */

#include "Watchdog.h"

#include <iostream>
using namespace std;

#include <stdlib.h>
#include <signal.h>

/**
 * The main thread of the Watchdog timer.
 * This function is waiting for a condition signal
 */
thread_exit_t Watchdog_thread(void* arg)
{
  thread_exit_t ret = 0;
  WatchdogType* wd = (WatchdogType*)arg;
  //cerr << "Watchdog waiting " << wd->wait_ms << " ms..." << endl;
  if (THREAD_COND_timedwait(&wd->cond, wd->wait_ms) != 0)
  {
    cerr << "Watchdog timeout!" << endl;
    wd->signal_fun(SIGABRT);
    ret = (thread_exit_t)1;
  }
  //cerr << "Watchdog thread done" << endl;
  THREAD_exit(ret);
  return ret;
}

/**
 * The default signal function is to simply quit
 */
static void watchdog_signal(int sig)
{
  //cout << "watchdog_signal(" << sig << ")" << endl;
  exit( sig );
}

/**
 * Initialising the condition & starting up the thread
 */
void Watchdog_init(WatchdogType* wd, int timeout_ms)
{
  //cout << "Watchdog_init(" << timeout_ms << ")" << endl;
  THREAD_COND_init(&wd->cond);
  wd->wait_ms = timeout_ms;
  wd->signal_fun = &watchdog_signal; //< Default signal function.
  THREAD_create( &wd->thread, Watchdog_thread, wd);
}

/**
 * Destroy
 */
void Watchdog_destroy(WatchdogType* wd)
{
  //cout << "Watchdog_destroy()" << endl;
  THREAD_COND_signal(&wd->cond);
  THREAD_join(wd->thread);
  THREAD_COND_destroy(&wd->cond);
}

/**
 * The C++ implementation of the Watchdog class
 */
#ifdef __cplusplus

Watchdog::Watchdog(int timeout_ms)
{
  Watchdog_init(&wd, timeout_ms);
}

Watchdog::Watchdog(int timeout_ms, void (*signal_fun)(int))
{
  Watchdog_init(&wd, timeout_ms);
  wd.signal_fun = signal_fun;
}

Watchdog::~Watchdog()
{
  Watchdog_destroy(&wd);
}

void Watchdog::setSignalFunction(void (*signal_fun)(int))
{
  wd.signal_fun = signal_fun;
}

#endif
