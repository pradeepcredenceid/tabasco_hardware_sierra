/* -------------------------------------------------------------------------
 Copyright (C) 2012-2014 Intel Mobile Communications GmbH

      Sec Class: Intel Confidential (IC)
 ----------------------------------------------------------------------
 Revision Information:
    $File name:  /msw_tools/FlashTool/libFlsTool/src/SerialPort.h $
    $CC-Version: .../dhkristx_140314_v0/1 $
    $Date:       2014-04-16    12:30:23 UTC $
 ----------------------------------------------------------------------
 by CCCT (0.12j)
 ---------------------------------------------------------------------- */

#ifndef SERIALPORT_H_
#define SERIALPORT_H_

#include "Mutex.h"
#include "Thread.h"
#include "MemberCallback.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifndef _WIN32
  #include <errno.h>
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <fcntl.h>
  #include <termios.h>
  #include <unistd.h>
  #include <limits.h>

  #ifndef Sleep
    #define Sleep(ms) usleep(ms*1000)
  #endif
#endif

#include <vector>
#include <memory>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <iostream>

using namespace std;


class SerialPort
{
public:
  SerialPort();
  SerialPort(const char* port, unsigned int baudrate = 921600);

  ~SerialPort();
  //virtual ~SimpleComPort()=0;

  bool open(const char* port, unsigned int baudrate = 921600);
  void close();

  /**
   * The ONLY way to read is asynchronously via this callback
   */
  void setReadCallback(const Callback2<void, const char*, size_t>& read_callback);
  void clearReadCallback();

  static void *run_reading_thread(void* obj)
  {
    return static_cast<SerialPort*>(obj)->start_reading();
  }

  /*
   * I/O
   */
  void* start_reading();

  void write(const char *data, size_t size);
  void write(const vector<char>& data);
  void write(const string& s);

  bool errorStatus() { return !m_error_string.empty(); };
  void setErrorStatus(const char* format, ...);
  string getErrorStatus() { return m_error_string; };

  // TODO: REmove
  void warn_people(const char*data, size_t size);

protected:
  Mutex m_in_mutex;            //< Protecting the IN buffer
  vector<char> m_in_buffer;    //< Data ready for reading by upper layers (signalled)

private:
  #ifdef WIN32
    HANDLE m_fd;
  #else
    int m_fd;
  #endif
  string m_port;
  string m_error_string;
  unsigned int m_baudrate;

  bool m_is_open;
  char m_read_buffer[512];     //< Buffer for reading

/*
  /// Data are queued here before they go in writeBuffer
  pthread_mutex_t* writeQueueMutex;
  mutable pthread_mutex_t *errorMutex;

  vector<char> writeQueue;
  vector<char> writeBuffer;
  char readBuffer[readBufferSize];
*/
  /// Read complete thread & callback
  ThreadType m_reading_thread;

  //sigc::signal<void, const char*, size_t> signal_read;
  Callback2<void, const char*, size_t> m_read_callback;
  //Callback<void, const char*, size_t> m_write_callback;

};



#endif /* SERIALPORT_H_ */
