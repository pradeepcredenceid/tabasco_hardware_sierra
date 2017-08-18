/* -------------------------------------------------------------------------
 Copyright (C) 2012-2014 Intel Mobile Communications GmbH

      Sec Class: Intel Confidential (IC)
 ----------------------------------------------------------------------
 Revision Information:
    $File name:  /msw_tools/FlashTool/libFlsTool/src/SerialPort.cpp $
    $CC-Version: .../oint_tools_flashtoole2_01/11 $
    $Date:       2014-03-13    15:37:11 UTC $
 ----------------------------------------------------------------------
 by CCCT (0.12j)
 ---------------------------------------------------------------------- */

#include "SerialPort.h"
#include "Exceptions.h"

#ifndef WIN32
  #include <sys/select.h>
#endif


#include <string>
#include <iostream>
#include <algorithm>
#include <iostream>

using namespace std;

/**
 * Throw an error
 */
#define fail(format, arg...) \
  { \
    setErrorStatus("%s:%d: " format,__FUNCTION__,__LINE__, ## arg); \
    close(); \
    if (m_reading_thread) \
      THREAD_join(m_reading_thread); \
    throw_exception( m_error_string, false ); \
  }

#define fail2(format, arg...) \
  { \
    setErrorStatus("%s:%d: " format,__FUNCTION__,__LINE__, ## arg); \
    close(); \
    if (m_reading_thread) \
      THREAD_join(m_reading_thread); \
    throw_exception( m_error_string ); \
  }

#ifdef WIN32
void dump_port_settings(DCB& port_settings)
{
  cout << " Port Settings:" << endl;
  cout << "   DCBlength         " << (int)port_settings.DCBlength << endl;
  cout << "   BaudRate          " << (int)port_settings.BaudRate << endl;
  cout << "   fBinary           " << (int)port_settings.fBinary << endl;
  cout << "   fParity           " << (int)port_settings.fParity << endl;
  cout << "   fOutxCtsFlow      " << (int)port_settings.fOutxCtsFlow << endl;
  cout << "   fOutxDsrFlow      " << (int)port_settings.fOutxDsrFlow << endl;
  cout << "   fDtrControl       " << (int)port_settings.fDtrControl << endl;
  cout << "   fDsrSensitivity   " << (int)port_settings.fDsrSensitivity << endl;
  cout << "   fTXContinueOnXoff " << (int)port_settings.fTXContinueOnXoff << endl;
  cout << "   fOutX             " << (int)port_settings.fOutX << endl;
  cout << "   fInX              " << (int)port_settings.fInX << endl;
  cout << "   fErrorChar        " << (int)port_settings.fErrorChar << endl;
  cout << "   fNull             " << (int)port_settings.fNull << endl;
  cout << "   fRtsControl       " << (int)port_settings.fRtsControl << endl;
  cout << "   fAbortOnError     " << (int)port_settings.fAbortOnError << endl;
  cout << "   fDummy2           " << (int)port_settings.fDummy2 << endl;
  cout << "   wReserved         " << (int)port_settings.wReserved << endl;
  cout << "   XonLim            " << (int)port_settings.XonLim << endl;
  cout << "   XoffLim           " << (int)port_settings.XoffLim << endl;
  cout << "   ByteSize          " << (int)port_settings.ByteSize << endl;
  cout << "   Parity            " << (int)port_settings.Parity << endl;
  cout << "   StopBits          " << (int)port_settings.StopBits << endl;
  cout << "   XonChar           " << (int)port_settings.XonChar << endl;
  cout << "   XoffChar          " << (int)port_settings.XoffChar << endl;
  cout << "   ErrorChar         " << (int)port_settings.ErrorChar << endl;
  cout << "   EofChar           " << (int)port_settings.EofChar << endl;
  cout << "   EvtChar           " << (int)port_settings.EvtChar << endl;
  cout << "   wReserved1        " << (int)port_settings.wReserved1 << endl;
}
#endif

//
//Class SerialPort
//
// TODO: Add (parity_bit, character_size, flow_control, stop_bits)
//

SerialPort::SerialPort()
  : m_fd(0),
    m_baudrate(0),
    m_is_open(false),
    m_reading_thread(0)
{
  // Do something ?
}

SerialPort::SerialPort(const char* port, unsigned int baudrate)
  : m_fd(0),
    m_port(port),
    m_baudrate(baudrate),
    m_is_open(false),
    m_reading_thread(0)
{
  open(port, baudrate);
}

bool SerialPort::open(const char* port, unsigned int baudrate)
{
#ifndef WIN32
  char real_port[PATH_MAX];
  struct termios new_attributes;

  switch(baudrate)
  {
      case    9600 : m_baudrate = B9600;      break;
      case   19200 : m_baudrate = B19200;     break;
      case   38400 : m_baudrate = B38400;     break;
      case   57600 : m_baudrate = B57600;     break;
      case  115200 : m_baudrate = B115200;    break;
      case  230400 : m_baudrate = B230400;    break;
      case  460800 : m_baudrate = B460800;    break;
      case  500000 : m_baudrate = B500000;    break;
      case  576000 : m_baudrate = B576000;    break;
      case  921600 : m_baudrate = B921600;    break;
      case 1000000 : m_baudrate = B1000000;   break;
      case 1152000 : m_baudrate = B1152000;   break;
      case 1500000 : m_baudrate = B1500000;   break;
      case 2000000 : m_baudrate = B2000000;   break;
      case 2500000 : m_baudrate = B2500000;   break;
      case 3000000 : m_baudrate = B3000000;   break;
      case 3500000 : m_baudrate = B3500000;   break;
      case 4000000 : m_baudrate = B4000000;   break;

      default:
        fail("Baudrate %u not supported", baudrate);
        return false;
  }

  // Opening the comport
  char* tmp = realpath(port, real_port);
  if (!tmp)
    fail("Unable to find port path %s (%s)", port, real_port);

  do {
    m_fd = ::open(tmp, O_RDWR | O_NOCTTY | O_NONBLOCK);
  } while (m_fd == -1 && errno == EBUSY);

  if (m_fd < 0)
    fail("Unable to open %s (%s)", port, real_port);

  m_is_open = true;

  /**
   *	KlocWork issue here, but m_fd is closed in the fail()!!!
   **/
  if (!isatty(m_fd))
    fail("Device '%s' is not a TTY", port);

  if (tcgetattr(m_fd, &new_attributes) < 0)
    fail("Unable to get attributes from %s", port);

  new_attributes.c_iflag = IGNBRK;
  new_attributes.c_oflag = 0;
  new_attributes.c_lflag = 0;
  new_attributes.c_cflag = (m_baudrate | CS8 | CREAD | CLOCAL); //8 data bit,Enable receiver,Ignore modem
  new_attributes.c_cc[VMIN]=1;  // Minimum number of characters to read before returning error
  new_attributes.c_cc[VTIME]=1; // Set timeouts in tenths of second

  if (tcsetattr(m_fd, TCSANOW, &new_attributes) < 0)
    fail("Unable to set attributes for %s", port);

#else
  switch(baudrate)
  {
      case    9600 : m_baudrate = 9600;      break;
      case   19200 : m_baudrate = 19200;     break;
      case   38400 : m_baudrate = 38400;     break;
      case   57600 : m_baudrate = 57600;     break;
      case  115200 : m_baudrate = 115200;    break;
      case  230400 : m_baudrate = 230400;    break;
      case  460800 : m_baudrate = 460800;    break;
      case  500000 : m_baudrate = 500000;    break;
      case  576000 : m_baudrate = 576000;    break;
      case  921600 : m_baudrate = 921600;    break;
      case 1000000 : m_baudrate = 1000000;   break;
      case 1152000 : m_baudrate = 1152000;   break;
      case 1500000 : m_baudrate = 1500000;   break;
      case 2000000 : m_baudrate = 2000000;   break;
      case 2500000 : m_baudrate = 2500000;   break;
      case 3000000 : m_baudrate = 3000000;   break;
      case 3500000 : m_baudrate = 3500000;   break;
      case 4000000 : m_baudrate = 4000000;   break;

      default:
        fail("Baudrate %u not supported", baudrate);
        return false;
  }

  // cout << " Opening the comport '" << port << "' (" << m_baudrate << ")" << endl;
  m_fd = CreateFile(port, GENERIC_READ | GENERIC_WRITE,
                     0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (m_fd == INVALID_HANDLE_VALUE)
    fail("Unable to open %s", port);

  m_is_open = true;

  DCB port_settings = { 0 };

  if (!GetCommState(m_fd, &port_settings))
    fail("Unable to set attributes for %s", port);

  port_settings.BaudRate = m_baudrate;
  port_settings.ByteSize = 8;
  port_settings.StopBits = ONESTOPBIT;
  port_settings.Parity = NOPARITY;
  port_settings.fDtrControl = DTR_CONTROL_ENABLE;
  port_settings.fRtsControl = RTS_CONTROL_ENABLE;

  port_settings.fNull = 0;
  port_settings.fInX = 0;
  port_settings.fOutX = 0;

  if (!SetCommState(m_fd, &port_settings))
    fail("Unable to set attributes for %s", port);

  COMMTIMEOUTS timeouts = { 0 };
  timeouts.ReadIntervalTimeout = MAXDWORD;

  if (!SetCommTimeouts(m_fd, &timeouts))
    fail("Unable to set attributes for %s", port);

	//cout << " Settings string: " << settings << endl;
#endif
  //cout << " Comport " << port << " open!" << endl;
  return m_is_open;
}

SerialPort::~SerialPort()
{
  clearReadCallback();
  close();
}

/**
 * The ONLY way to read is asynchronously via this callback
 */
//
//void SerialPort::warn_people(const char*data, size_t size)
//{
//  cout << "There are " << size << " aliens in the carpark!" << endl;
//}
//
void SerialPort::setReadCallback(const Callback2<void, const char*, size_t>& read_callback)
{
  //signal_read.connect( sigc::mem_fun(this, &SerialPort::warn_people) );
  m_read_callback = read_callback;

  if (!m_reading_thread)
    THREAD_create(&m_reading_thread, &SerialPort::run_reading_thread, this);
}

void SerialPort::clearReadCallback()
{
  m_read_callback.clear();

  if (m_reading_thread)
  {
    THREAD_cancel(m_reading_thread);
    THREAD_join(m_reading_thread);
  }
}

/*
 * I/O
 */
void* SerialPort::start_reading()
{
  int received;
  // cout << " ** Read loop in spawned thread ** " << endl;
#ifndef WIN32
  fd_set rfds, reading_set;
  struct timeval tv;
  int fd;

  FD_ZERO(&rfds);
  FD_SET(m_fd, &rfds);

  for(;;)
  {
    memcpy(&reading_set, &rfds, sizeof(rfds));

    /* Wait up to 1 second */
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    fd = select(m_fd+1, &reading_set, NULL, NULL, &tv);

    if (fd == -1)
    {
      setErrorStatus("Reading Thread");
      break;
    }
    else if (fd == 0)
    {
      THREAD_yield();
      continue;
    }
    else
    {
      received = ::read(m_fd, m_read_buffer, sizeof(m_read_buffer));
#else
  for(;;)
  {
    ReadFile(m_fd, m_read_buffer, sizeof(m_read_buffer), (LPDWORD)((void*)&received), NULL);
#endif

      if(received < 0)
      {
        if (errno == EAGAIN)
        {
          THREAD_yield();
          continue;
        }

        //cout << endl << " Reading thread returned: " << received << endl;
        if(!m_is_open)
          return (void*)1;     //< Thread interrupted because port closed
        else
          setErrorStatus("Reading Thread");
      }
      else if (received > 0)
      {
        m_in_mutex.lock();
        m_in_buffer.assign(m_read_buffer, m_read_buffer+received);
        m_in_mutex.unlock();

        //cout << " Read " << received  << " (" << m_in_buffer.size() << ") bytes: " << endl;
        //cout << "Buffer: " << m_read_buffer << endl;
        //m_read_callback(m_read_buffer, received);
        m_read_callback((const char*)&m_in_buffer[0], received);
      }
      else
      {
        #ifdef WIN32
          Sleep(1); // Timeout.
        #endif
      }
#ifndef WIN32
    }
#endif
  }
  return (void*)0;
}

void SerialPort::write(const char *data, size_t size)
{
  int res;
  size_t wrote = 0;
  //if(!m_is_open)

  do {
    #ifdef WIN32
      WriteFile(m_fd, data+wrote, size-wrote, (LPDWORD)((void*)&res), NULL);
    #else
      res = ::write(m_fd, data+wrote, size-wrote);
    #endif

    if (res < 0)
      fail2("Write error");

    if (res > 0)
      wrote += res;

    //cout << endl << " Wrote " << wrote << " bytes..." << endl;
  } while (wrote < size);

  if(wrote != size)
    fail2("Write error");
}

void SerialPort::write(const vector<char>& data)
{
  write(&data[0], data.size());
//  if(::write(m_fd, &data[0], data.size()) != data.size())
//    fail("write vector<char>");
}

void SerialPort::write(const string& s)
{
  write(&s[0], s.size());
//  if(::write(m_fd, &s[0], s.size()) != s.size())
//    fail("write string");
}

void SerialPort::setErrorStatus(const char* format, ...)
{
  char buffer[512];
  va_list args;
  va_start(args, format);
  #ifdef WIN32
    vsprintf(buffer, format, args);
  #else
    vsnprintf(buffer, sizeof(buffer), format, args);
  #endif
  va_end(args);

  m_error_string = buffer;
  m_error_string += ": ";
  m_error_string += strerror(errno);
}

void SerialPort::close()
{
  if (m_is_open)
  {
    #ifdef WIN32
      CloseHandle(m_fd);
    #else
      ::close(m_fd);
    #endif
    m_is_open = false;
  }
}

