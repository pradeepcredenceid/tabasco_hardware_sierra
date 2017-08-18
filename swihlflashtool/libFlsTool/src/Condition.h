/* -------------------------------------------------------------------------
 Copyright (C) 2012-2014 Intel Mobile Communications GmbH

      Sec Class: Intel Confidential (IC)
 ----------------------------------------------------------------------
 Revision Information:
    $File name:  /msw_tools/FlashTool/libFlsTool/src/Condition.h $
    $CC-Version: .../dhkristx_140314_v0/1 $
    $Date:       2014-04-09    13:21:38 UTC $
 ----------------------------------------------------------------------
 by CCCT (0.12f)
 ---------------------------------------------------------------------- */

#ifndef CONDITION_H_
#define CONDITION_H_

#include "Mutex.h"

/**
 * Defining Thread functions for ANSI C
 **/
#ifdef WIN32
  #include <windows.h>

  typedef struct {
     int waiting;
     HANDLE cond;
     CRITICAL_SECTION   lock;
  } CondType;
#else
  #include <pthread.h>
  #include <errno.h>
  #include <sys/time.h>

  typedef struct {
     int set;
     pthread_mutex_t mutex;
     pthread_cond_t cond;
  } CondType;
#endif


#ifdef __cplusplus
extern "C" {
#endif

  // Cross-platform version of the POSIX Condition
  void THREAD_COND_init(CondType* ev);
  void THREAD_COND_destroy(CondType* ev);

  int THREAD_COND_signal(CondType* ev);
  int THREAD_COND_broadcast(CondType* ev);

  int THREAD_COND_wait(CondType* ev);
  int THREAD_COND_timedwait(CondType* ev, int ms);

#ifdef __cplusplus
}

#if 0
/**
 * The Condition C++ Class
 **/
class Condition
{
public:
  Condition();
  virtual ~Condition();
};

#endif

#endif /* __cplusplus */

#endif /* CONDITION_H_ */
