/*  -------------------------------------------------------------------------
    Copyright (C) 2011-2013 Intel Mobile Communications GmbH

         Sec Class: Intel Confidential (IC)

    All rights reserved.
    -------------------------------------------------------------------------
    This document contains proprietary information belonging to IMC.
    Passing on and copying of this document, use and communication of its
    contents is not permitted without prior written authorization.
    -------------------------------------------------------------------------
    Revision Information:
       $File name:  /msw_tools/FlashTool/libFlsTool/src/Mutex.h $
       $CC-Version: .../oint_tools_flashtoole2_01/4 $
       $Date:       2013-10-02    14:59:59 UTC $
    ------------------------------------------------------------------------- */

#ifndef MUTEX_H_
#define MUTEX_H_

/**
 * Defining Mutex functions for ANSI C
 **/
#ifdef _WIN32
  #include <windows.h>

  #define MutexType   HANDLE

  #define MUTEX_init( mutex )       mutex = CreateMutex(0, false, NULL);
  #define MUTEX_lock( mutex )       WaitForSingleObject(mutex, INFINITE)
  #define MUTEX_tryLock( mutex )    WaitForSingleObject(mutex, 0)
  #define MUTEX_unlock( mutex )     ReleaseMutex(mutex)
  #define MUTEX_destroy( mutex )    CloseHandle(mutex)

#else
  #include <pthread.h>
  #include <errno.h>

  #define MutexType   pthread_mutex_t

  #define MUTEX_init( mutex )       pthread_mutex_init( &mutex, 0 )
  #define MUTEX_lock( mutex )       pthread_mutex_lock( &mutex )
  #define MUTEX_tryLock( mutex )    pthread_mutex_trylock( &mutex )
  #define MUTEX_unlock( mutex )     pthread_mutex_unlock( &mutex )
  #define MUTEX_destroy( mutex ) \
    if (pthread_mutex_destroy(&mutex) == EBUSY) \
    { \
      pthread_mutex_unlock(&mutex); \
      pthread_mutex_destroy(&mutex); \
    }

#endif


#ifdef __cplusplus

//#include <iostream>
//using namespace std;

class Mutex
{
public:
    //friend class Condition;
    Mutex();
    virtual ~Mutex();
    Mutex(const Mutex& copy);

    bool lock();
    bool unlock();
    bool tryLock();

    /* Lock a mutex in a code block (unlocks when it goes out of scope) */
    class scope_lock
    {
    public:
      Mutex *m_mutex;
      bool locked;

      explicit scope_lock(Mutex* mutex)
        : m_mutex(mutex), locked(true)
      {
        m_mutex->lock();
      }

      void unlock()
      {
        locked = false;
        m_mutex->unlock();
      }

      ~scope_lock()
      {
        if (locked)
          m_mutex->unlock();
      }
    };

private:
  MutexType m_pmutex;
  bool      m_isOwner;
};

#endif /* __cplusplus */
#endif /* MUTEX_H_ */
