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
       $File name:  /msw_tools/FlashTool/DownloadTool/src/Phone.h $
       $CC-Version: .../dhkristx_140314_v0/2 $
       $Date:       2014-04-16    10:53:54 UTC $
    -------------------------------------------------------------------------
    For more AT support:
	   http://michaelgellis.tripod.com/modem.html
    ------------------------------------------------------------------------- */

#ifndef PHONE_H_
#define PHONE_H_

#include "Thread.h"
#include "Condition.h"
#include "Mutex.h"
#include "SerialPort.h"

#include <iostream>

#ifndef _WIN32
  #include <sys/time.h>
  #include <unistd.h>
#endif

using namespace std;

#define WAIT_FOR_RESPONSE      500

// == Commands API ==
class PhoneCommand
{
public:
  PhoneCommand(string data)
  {
    for(size_t i=0; i < data.length(); ++i)
      cmd.push_back( data.at(i) );
  }

  size_t size()       { return cmd.size(); }
  const char* data() { return &cmd[0]; }

private:
  std::vector<char> cmd;
};


class Phone : public SerialPort
{
public:
  Phone(string dev, int baudrate = 115200);

  ~Phone();

  void setVerbose(int verbose);
  void setWait(int ms);
  const struct timespec *getWait();

  /**
   * Connecting to the AT console
   * It sleeps a second before starting and between each AT,
   * because we don't want to enter the bootloader.
   *
   * @param tries how many attempts to connect (also how many seconds to wait)
   * @return true if it connected, and false if it failed
   */
  bool connect(int tries = 10);

  // String data (AT mode)
  void sendAT(string cmd);
  string recvAT(bool blocking = false);

  // Raw data (Binary mode)
  void send(PhoneCommand cmd);
  std::vector<char> recv(bool blocking = false);

private:
  bool m_owner;
  string m_device;
  int   m_baudrate;
  int   m_verbose;

  int m_wait_ms;
  //struct timespec m_wait_time;

  Mutex in_mutex;
  std::vector<char> Incomming;    //< Incomming data
  std::vector<char> InBuffer;     //< when data is ready it is pushed here

  void onRecv(const char *data, size_t len);
  //Signal<void(const char *data, size_t len)> signal_recv;

  string request;
  string response;

  CondType  m_cond;
};

#endif /* PHONE_H_ */
