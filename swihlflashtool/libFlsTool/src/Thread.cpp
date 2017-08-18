/* -------------------------------------------------------------------------
 Copyright (C) 2012-2014 Intel Mobile Communications GmbH
 
      Sec Class: Intel Confidential (IC)
 ----------------------------------------------------------------------
 Revision Information:
    $File name:  /msw_tools/FlashTool/libFlsTool/src/Thread.cpp $
    $CC-Version: .../dhkristx_140314_v0/3 $
    $Date:       2014-05-01    12:47:44 UTC $
 ----------------------------------------------------------------------
 by CCCT (0.12l)
 ---------------------------------------------------------------------- */

#include "Thread.h"

#ifdef WIN32
  DWORD THREAD_exit_code = 0;
#else
  void* THREAD_exit_code = NULL;
#endif

/**
 * TODO: Add typeid to make callbacks easier to define
 */
thread_exit_t ThreadFunc(void *obj)
{
  thread_exit_t retval = 0;
  Thread *thread = reinterpret_cast<Thread *>(obj);
  thread->m_running = true;
  thread->run();
  thread->m_running = false;
  #ifdef WIN32
    ExitThread(0);
  #else
    pthread_exit(NULL);
  #endif
  return retval;
}

//
// TODO: Add to MemberCallback.h templates
//
Thread::Thread( thread_func_t func, void *args )
  : m_pid(0),
    m_isOwner(true),
    m_running(false),
    m_args(args),
    m_exit_code(0),
    m_func_ptr(func)
{

}

Thread::~Thread()
{
  #ifdef WIN32
    TerminateThread( m_pid, THREAD_exit_code );
  #else
    #ifdef ANDROID
      pthread_kill(m_pid, SIGKILL);
    #else
      pthread_cancel( m_pid );
    #endif
  #endif
}


void Thread::run()
{
  #ifdef WIN32
    m_pid = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)m_func_ptr, m_args, 0, NULL);
  #else
    pthread_create( &m_pid, NULL, m_func_ptr, m_args );
  #endif
}

void Thread::cancel()
{
  #ifdef WIN32
    TerminateThread( m_pid, m_exit_code );
  #else
    #ifdef ANDROID
      pthread_kill(m_pid, SIGKILL);
    #else
      pthread_cancel( m_pid );
    #endif
  #endif
}

thread_exit_t Thread::join()
{
  #ifdef WIN32
    TerminateThread( m_pid, m_exit_code );
  #else
    pthread_join( m_pid, &m_exit_code );
  #endif
  return m_exit_code;
}
