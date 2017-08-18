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
       $File name:  /msw_tools/FlashTool/DownloadTool/src/Phone.cpp $
       $CC-Version: .../oint_tools_flashtoole2_01/17 $
       $Date:       2014-03-13    15:45:06 UTC $
    ------------------------------------------------------------------------- */

#include "Phone.h"
#include "Utils.h"

/* Phone Class */
Phone::Phone(string dev, int baudrate)
  : SerialPort(dev.c_str(),(unsigned int)baudrate),
    m_owner(true),
    m_device( dev ),
    m_baudrate( baudrate ),
    m_verbose(false),
    request(""),
    response("")
{
  THREAD_COND_init(&m_cond);

  setWait(WAIT_FOR_RESPONSE);
  setReadCallback( MemFun(&Phone::onRecv, this) );
}


Phone::~Phone()
{
  if (m_owner)
    THREAD_COND_destroy(&m_cond);
}

void Phone::setVerbose(int verbose)
{
  if (verbose <= 0)
    m_verbose = 0;
  else
    m_verbose = verbose;
}

void Phone::setWait(int msec)
{
  m_wait_ms = msec;
  //timeval   timenow;
  //gettimeofday(&timenow, 0);
  //m_wait.tv_sec = timenow.tv_sec + msec*1000;
}

#if 0
const struct timespec *Phone::getWait()
{
  static struct timespec timeout;
  struct timeval  now;
  gettimeofday(&now, 0);
  //cout << " Now: " << now.tv_sec << "." << now.tv_usec << endl;
  int usec = (now.tv_usec + m_wait_ms*1000);
  timeout.tv_sec = now.tv_sec+(int)(usec/1000000);
  timeout.tv_nsec = (usec%1000000)*1000;
  //cout << " Timeout: " << timeout.tv_sec << "." << (timeout.tv_nsec/1000) << endl;
  return &timeout;
}
#endif

/* Phone Helper Functions */
static string dumpVector(std::vector<char>& data)
{
  string line;
  for(size_t i=0; i < data.size(); ++i)
    line += data[i];
  return line;
}

/**
 * TODO: Look for the 'OK' to determine whole response
 */
#if 0
static string getATresponse(std::vector<char>& Buffer, string& request)
{
  size_t pos;
  string response;

  response = dumpVector( Buffer ).substr( request.length() );

  while ( (pos = response.find('\n')) != string::npos )
    response.erase(pos,1);

  while ( (pos = response.find('\r')) != string::npos )
    response.erase(pos,1);

  return response;
}
#endif

void Phone::onRecv(const char *data, size_t len)
{
  Mutex::scope_lock l(&m_in_mutex);
  Mutex::scope_lock lo(&in_mutex);

  for (size_t i=0; i < len; ++i)
  {
    Incomming.push_back(data[i]);

    size_t size = Incomming.size();
    if ((size > 1 && Incomming.at(size-2) == 'O'
                  && Incomming.at(size-1) == 'K')
        || // OK / ERROR
        (size > 4 && Incomming.at(size-5) == 'E'
                  && Incomming.at(size-4) == 'R'
                  && Incomming.at(size-3) == 'R'
                  && Incomming.at(size-2) == 'O'
                  && Incomming.at(size-1) == 'R')
        )
    {
      response = dumpVector( Incomming );

      //< if (!echo) Remove echo
      {
        size_t offset = 0;
        size_t len = request.length()-1;
        while (response[offset] == '\n' || response[offset] == '\r')
          offset++;

        if (response.compare(offset, len, request.substr(0, len)) == 0)
        {
          while (response[len+offset] == '\n' || response[len+offset] == '\r')
            offset++;
          response = response.substr( len+offset );
        }
      }

      if (m_verbose)
        cout << endl << "[" << timestamp() << "] " << m_device
             << " > '" << response << "'" << endl;

      //at_ready_condition.notify_all();
      THREAD_COND_broadcast( &m_cond );

      // TODO: signal_receive(response);
    }
  }
}


void Phone::sendAT(string req)
{
  if (m_verbose)
  {
    cout << "\r[" << timestamp() << "] " << m_device << " < '" << dumpData(req) << "'";
    cout.flush();
  }

  if (req.at(req.size()-1) != '\r')
    req += '\r';

  request = req;
  send( PhoneCommand(request) );
}

/**
 *  Wait for the next AT command and return it.
 *  If a previous call
 */
string Phone::recvAT(bool blocking)
{
  if (blocking) {
    THREAD_COND_wait( &m_cond );
    //at_ready_condition.wait();
  } else {
    //if (!at_ready_condition.timed_wait(timeout))
    if (THREAD_COND_timedwait(&m_cond, m_wait_ms) != 0)
      return string("");
  }

  in_mutex.lock();
  string tmp = response;
  response.clear();
  Incomming.clear();
  in_mutex.unlock();

  return tmp;
}

/**
 * Connecting to the AT console
 * It sleeps a second before starting and between each AT,
 * because we don't want to enter the bootloader.
 *
 * @return true if it connected, and false if it failed
 */
bool Phone::connect(int tries)
{
  if (errorStatus())
    cout << "Phone Error " << getErrorStatus() << endl;

  for (int i=0; i<tries; ++i)
  {
    Sleep(100);            ///< Give the phone some time to startup
    sendAT("AT");
    string resp = recvAT();
    if (resp.find("OK") != string::npos)
      return true;
    else if (resp.find("ERROR") != string::npos)
      return false;
  }
  return false;
}


/* Low level functions */
void Phone::send(PhoneCommand cmd)
{
  write( cmd.data(), cmd.size() );
}

std::vector<char> Phone::recv(bool blocking)
{
  vector<char> tmp;
  if (blocking) {
    //at_ready_condition.wait();
    THREAD_COND_wait( &m_cond );
  } else {
    //if (!at_ready_condition.timed_wait(timeout))
    if (THREAD_COND_timedwait(&m_cond, m_wait_ms) != 0)
      return tmp;
  }

  if (Incomming.size() > 0)
  {
    Mutex::scope_lock l(&in_mutex);
    tmp.assign(Incomming.begin(), Incomming.end());
    Incomming.clear();
  }

  return tmp;
}

