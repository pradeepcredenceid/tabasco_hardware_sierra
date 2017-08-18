/* -------------------------------------------------------------------------
 Copyright (C) 2012-2014 Intel Mobile Communications GmbH

      Sec Class: Intel Confidential (IC)
 ----------------------------------------------------------------------
 Revision Information:
    $File name:  /msw_tools/FlashTool/libFlsTool/src/Condition.cpp $
    $CC-Version: .../dhkristx_140314_v0/1 $
    $Date:       2014-04-09    13:21:34 UTC $
 ----------------------------------------------------------------------
 by CCCT (0.12j)
 ---------------------------------------------------------------------- */

#include "Condition.h"

#include <cstdint>
#include <iostream>

using namespace std;

/**
 * POSIX Threads Condition
 */
void THREAD_COND_init(CondType* ev)
{
  #ifdef WIN32
    ev->cond = CreateEvent(NULL, false, false, NULL);
    //InitializeConditionVariable (&ev->cond);
    InitializeCriticalSection (&ev->lock);
    ev->waiting = 0;
  #else
    if (pthread_mutex_init(&ev->mutex, 0) != 0)
      cerr << " POSIX mutex init failed!" << endl;

    if (pthread_cond_init(&ev->cond, 0) != 0)
      cerr << " POSIX condition init failed!" << endl;
    ev->set = 0;
  #endif
}

void THREAD_COND_destroy(CondType* ev)
{
  #ifdef WIN32
    ev->waiting = 0;
    CloseHandle( ev->cond );
  #else
    if (pthread_mutex_destroy(&ev->mutex) == EBUSY)
    {
      pthread_mutex_unlock(&ev->mutex);
      pthread_mutex_destroy(&ev->mutex);
    }
    if (pthread_cond_destroy(&ev->cond) == EBUSY)
    {
      pthread_cond_broadcast(&ev->cond);
      pthread_cond_destroy(&ev->cond);
    }
  #endif
}

int THREAD_COND_signal(CondType* ev)
{
  int ret = 0;

  #ifdef WIN32
    EnterCriticalSection (&ev->lock);
    SetEvent( ev->cond );
    //WakeConditionVariable( &ev->cond );
    LeaveCriticalSection (&ev->lock);
  #else
    pthread_mutex_lock( &ev->mutex );
    ev->set = 1;
    ret = pthread_cond_signal( &ev->cond );
    pthread_mutex_unlock( &ev->mutex );
  #endif

  return ret;
}

int THREAD_COND_broadcast(CondType* ev)
{
  int ret = 0;

  #ifdef WIN32
    EnterCriticalSection (&ev->lock);
    while (ev->waiting-- > 0)
      SetEvent( ev->cond ); //< Wakes only one thread!!!
    //WakeAllConditionVariable( &ev->cond );
    ev->waiting = 0;
    LeaveCriticalSection (&ev->lock);
  #else
    pthread_mutex_lock( &ev->mutex );
    ev->set = 1;
    ret = pthread_cond_broadcast( &ev->cond );
    pthread_mutex_unlock( &ev->mutex );
  #endif

  return ret;
}

int THREAD_COND_wait(CondType* ev)
{
  int ret;
  #ifdef WIN32
    EnterCriticalSection (&ev->lock);
    ev->waiting++;
    LeaveCriticalSection (&ev->lock);
    ret = WaitForSingleObject(ev->cond, INFINITE);
    //return SleepConditionVariableCS(&ev->cond, &ev->mutex, INFINITE);
  #else
    pthread_mutex_lock( &ev->mutex );
    ret = pthread_cond_wait( &ev->cond, &ev->mutex );
    pthread_mutex_unlock( &ev->mutex );
  #endif
  return ret;
}


int THREAD_COND_timedwait(CondType* ev, int ms)
{
  int ret = 0;
  #ifdef WIN32
    EnterCriticalSection (&ev->lock);
    ev->waiting++;
    LeaveCriticalSection (&ev->lock);
    ret = WaitForSingleObject(ev->cond, ms);
    if (ret == WAIT_TIMEOUT)
      ev->waiting--;
    //return SleepConditionVariableCS(&ev->cond, &ev->mutex, ms);
  #else
    pthread_mutex_lock( &ev->mutex );
    if (ms == 0)
    {
      ret = pthread_cond_wait( &ev->cond, &ev->mutex );
    }
    else
    {
      struct timeval now;
      static struct timespec timeout;
      gettimeofday( &now, NULL );
      //cout << " Now: " << now.tv_sec << "." << now.tv_usec << endl;
      uint32_t usec = (uint32_t)now.tv_usec + ms*1000;
      timeout.tv_sec = now.tv_sec+(usec/1000000);
      timeout.tv_nsec = (usec%1000000)*1000;

      while (!ev->set && ret == 0)
        ret = pthread_cond_timedwait( &ev->cond, &ev->mutex, &timeout );
      //if (ret == ETIMEDOUT)
      //  cout << "Timeout! (set: " << ev->set << ")" << endl;
      //else
      // cout << " Returning " << ret << "(set: " << ev->set << ")" << endl;
    }
    ev->set = 0;
    pthread_mutex_unlock( &ev->mutex );
  #endif

  return ret;
}
