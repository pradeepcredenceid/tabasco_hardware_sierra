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
       $File name:  /msw_tools/FlashTool/libFlsTool/src/Mutex.cpp $
       $CC-Version: .../oint_tools_flashtoole2_01/3 $
       $Date:       2013-10-02    15:00:01 UTC $
    ------------------------------------------------------------------------- */

#include "Mutex.h"
#include <cerrno>

Mutex::Mutex()
      : m_isOwner(true)
{
  #ifdef WIN32
    m_pmutex = CreateMutex(0, false, NULL);
  #else
    //m_pmutex = new MutexType;
    pthread_mutex_init(&m_pmutex, 0);
  #endif
}

Mutex::~Mutex()
{
  if (!m_isOwner)
    return;

  #ifdef WIN32
    CloseHandle( m_pmutex );
  #else
    if (pthread_mutex_destroy(&m_pmutex) == EBUSY)
    {
      unlock();
      pthread_mutex_destroy(&m_pmutex);
    }
    //delete m_pmutex;
  #endif
}

Mutex::Mutex(const Mutex& copy)
{
  m_isOwner = false;
  m_pmutex  = copy.m_pmutex;
}

bool Mutex::lock()
{
  #ifdef WIN32
    if (WaitForSingleObject(m_pmutex, INFINITE))
      return true;
  #else
    if (pthread_mutex_lock( &m_pmutex ))
      return true;
  #endif
  return false;
}

bool Mutex::unlock()
{
  #ifdef WIN32
    if (ReleaseMutex(m_pmutex) == 0)
      return true;
  #else
    if (pthread_mutex_unlock( &m_pmutex ) == 0)
      return true;
  #endif
  return false;
}

bool Mutex::tryLock()
{
  #ifdef WIN32
    if (WaitForSingleObject(m_pmutex, 0))
      return true;
  #else
    if (pthread_mutex_trylock( &m_pmutex ))
      return true;
  #endif
  return false;
}
