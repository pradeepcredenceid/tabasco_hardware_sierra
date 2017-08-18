/*  -------------------------------------------------------------------------
    Copyright (C) 2011-2014 Intel Mobile Communications GmbH

         Sec Class: Intel Confidential (IC)

    All rights reserved.
    -------------------------------------------------------------------------
    This document contains proprietary information belonging to IMC.
    Passing on and copying of this document, use and communication of its
    contents is not permitted without prior written authorization.
    -------------------------------------------------------------------------
    Revision Information:
       $File name:  /msw_tools/FlashTool/libFlsTool/src/OS.cpp $
       $CC-Version: .../dhkristx_140314_v0/1 $
       $Date:       2014-04-22    9:14:13 UTC $
    ------------------------------------------------------------------------- */

#include "OS.h"

#include <stdio.h>    //< sys_nerr & sys_errlist
#include <string>
#include <stdexcept>

using namespace std;

const char* GetErrorString(int error_code)
{
  static char strError[256];
  char* ptr = strError;

  #ifdef WIN32
    int err = GetLastError();
    //strerror_s(strError, sizeof(strError), err);
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, err,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  ptr, sizeof(strError), 0);

    char* nl = strchr(strError, '\r');
    if (nl) nl = '\0';
  #else
    int err = errno;
    #if ANDROID
      strerror_r(err, strError, sizeof(strError));
    #else
      ptr = strerror_r(err, strError, sizeof(strError));
      if (!error_code)
        snprintf(strError, sizeof(strError), "%s", ptr);
    #endif
  #endif

  if (error_code)
    snprintf(strError, sizeof(strError), "%s (%d)", ptr, err);

  return strError;
}

/**
 * Returns the current timestamp in ISO 8601 format
 *
 * TODO: Move somewhere else (Maybe Time.cpp)
 * TODO: Figure out the %z option difference between Win/Linux
 *
 **/
char* GetTimestamp(time_t rawtime)
{
  struct tm * timeinfo;
  static char buffer[80];

  if (rawtime == 0)
    time (&rawtime);
  timeinfo = localtime (&rawtime);
  if (!timeinfo)
    return buffer;

  //strftime (buffer, sizeof(buffer), "%F %T %Z", timeinfo);             // ISO 8601 date format (same as "%F %T %Z")
  strftime (buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S %z", timeinfo); // ISO 8601 date format (same as "%Y-%m-%d %H:%M:%S %Z")

  return buffer;
}

/**
 * Finds the user home path
 */
string GetUserHomePath()
{
  const char* home = getenv("HOME");
  if (home or (home = getenv("USERPROFILE")))
    return string(home);

  const char* hdrive = getenv("HOMEDRIVE");
  if (!hdrive or !(home = getenv("HOMEPATH")))
  {
    #ifdef __EXCEPTIONS
      throw logic_error("Could not find user home directory");
    #else
      return string("");
    #endif
  }
  return string(hdrive) + string(home);
}

#ifdef STRNLEN_DEFINED
  size_t strnlen(const char* str,size_t len)
  {
    size_t i;
    for(i=0; i < len && *str != '\0'; ++i,++str)
	  ;
    return i;
  }
#endif

#ifdef TICK_COUNT_DEFINED
  uint32_t GetTickCount( void )
  {
    #if 1
      struct timeval now;
      gettimeofday( &now, NULL );
      return(uint32_t)( now.tv_sec*1000 + now.tv_usec/1000 );
    #else
      /* Much more precise, but also more CPU consuming */
      struct timespec ts;
      clock_gettime(CLOCK_MONOTONIC,&ts);
      return (uint32_t)( ts.tv_sec*1000 + ts.tv_nsec/1000000L ); /* Milliseconds */
    #endif
  }
#endif

#if defined (__CC_ARM)
  char *strdup(const char *s)
  {
    char *p = (char*)malloc(strlen(s) + 1);
    if(p) { strcpy(p, s); }
    return p;
  }
#endif
