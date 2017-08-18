/* -------------------------------------------------------------------------
 Copyright (C) 2012-2013 Intel Mobile Communications GmbH

      Sec Class: Intel Confidential (IC)
 ----------------------------------------------------------------------
 Revision Information:
    $File name:  /msw_tools/FlashTool/libFlsTool/src/Thread.h $
    $CC-Version: .../oint_tools_flashtoole2_01/7 $
    $Date:       2013-11-27    14:43:21 UTC $
 ----------------------------------------------------------------------
 by CCCT (0.12f)
 ---------------------------------------------------------------------- */
#ifndef THREAD_H_
#define THREAD_H_

#include "MemberCallback.h"

/**
 * Defining Thread functions for ANSI C
 **/
#ifdef WIN32
  #include <windows.h>

  #define ThreadType   HANDLE
  extern  DWORD THREAD_exit_code;
  typedef DWORD thread_exit_t;
  typedef long unsigned (*thread_func_t)(void*);

  #define THREAD_create( pid, func, args )  *pid = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)func, args, 0, NULL)
  #define THREAD_exit( ret )                ExitThread( ret )
  #define THREAD_kill( pid )                { CloseHandle( pid ); GetExitCodeThread( pid, &THREAD_exit_code ); }
  #define THREAD_join( pid )                WaitForSingleObject( pid, INFINITE )
  #define THREAD_cancel( pid )              TerminateThread( pid, THREAD_exit_code )
  #define THREAD_yield()                    SwitchToThread()
#else
  #include <pthread.h>
  #include <errno.h>
  #include <sys/time.h>
  #include <csignal>

  #define ThreadType   pthread_t
  extern  void* THREAD_exit_code;
  typedef void* thread_exit_t;
  typedef void* (*thread_func_t)(void*);

  #define THREAD_create( pid, func, args )   pthread_create( pid, NULL, func, args )
  #define THREAD_exit( ret )                 pthread_exit( ret )
  #define THREAD_join( pid )                 pthread_join( pid, &THREAD_exit_code)
//#define THREAD_kill( pid )

  #ifdef ANDROID
    #define THREAD_yield()                     usleep(10)
    #define THREAD_cancel( pid )               pthread_kill(pid, SIGKILL)
  #else
    #define THREAD_yield()                     pthread_yield()
    #define THREAD_cancel( pid )               pthread_cancel( pid )
  #endif

#endif

#ifdef __cplusplus

class Thread
{
public:
  Thread( thread_func_t func, void *args );
  virtual ~Thread();

  virtual void run();

  void cancel();
  thread_exit_t join();

private: // prevent copy-constructors
    Thread(const Thread&);
    Thread& operator=(const Thread&);

    friend thread_exit_t ThreadFunc(void *);

private:
  ThreadType    m_pid;
  bool          m_isOwner;
  bool          m_running;

  void*         m_args;
  thread_exit_t   m_exit_code;
  thread_func_t m_func_ptr;
};

#endif

#endif /* THREAD_H_ */
