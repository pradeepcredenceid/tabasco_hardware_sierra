/*  -------------------------------------------------------------------------
    Copyright (C) 2013-2014 Intel Mobile Communications GmbH

         Sec Class: Intel Confidential (IC)
    ----------------------------------------------------------------------
    Revision Information:
       $File name:  /msw_tools/FlashTool/libFlsTool/src/Logger.h $
       $CC-Version: .../dhkristx_140314_v0/1 $
       $Date:       2014-04-22    9:14:05 UTC $
    ----------------------------------------------------------------------
    Description:
       A simple logging class.
    ---------------------------------------------------------------------- */

#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>

namespace ipa
{
  /**
   * A Logger Stream class
   *
   * Can be used to log to any streams just like cout.
   *
   * Note: It is not thread-safe!
   */
  class Logger
  {
  public:
    Logger()
      : m_out(0),
        m_owner(false) {}

    virtual ~Logger()
    {
      setStream(0, false);
    }

    void setStream(std::ostream* stream, bool owner = false)
    {
      if(m_owner)
        delete m_out;
      m_out   = stream;
      m_owner = owner;
    }

    template<typename T>
    Logger& operator<<(const T& object)
    {
      if(m_out)
        (*m_out) << object;
      return *this;
    }

    Logger &operator<<(std::ostream& (*pf) (std::ostream&))
    {
      if(m_out)
        (*m_out) << pf;
      return *this;
    }

  private:
    std::ostream *m_out;
    bool m_owner;
  };

} /* namspace ipa */

#endif
