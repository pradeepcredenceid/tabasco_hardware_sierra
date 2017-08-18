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
       $File name:  /msw_tools/FlashTool/DownloadTool/src/Utils.h $
       $CC-Version: .../oint_tools_flashtoole2_01/13 $
       $Date:       2013-10-02    15:03:37 UTC $
    ------------------------------------------------------------------------- */
#ifndef UTILS_H_
#define UTILS_H_

#include <cstdio>
#include <cstdlib>
#include <ctime>

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdexcept>

using namespace std;

// Taken from the OS.h
#ifdef __GNUC__
  #include <unistd.h>     //< access()
  #include <sys/stat.h>   //< stat(), mkdir()
  #include <sys/types.h>  //< types
  #include <ctype.h>
#endif

#ifndef DirExists
  #define DirExists(dir)         !access(dir, 0)
#endif

#ifdef _WIN32
  #include <windows.h>

  #define SEPARATOR '\\'
  #ifndef DirCreate
    #define DirCreate(dir)         mkdir(dir)
  #endif
#else
  #define SEPARATOR '/'
  #ifndef DirCreate
    #define DirCreate(dir)         mkdir(dir, 0777)
  #endif
#endif

#ifdef _WIN32
  #define MODEM_PRE_STRING "\\\\.\\COM"
  #define PHONE_PRE_STRING "\\\\.\\COM"
  #define USB_PRE_STRING   "\\\\.\\USB"
#elif __linux__
  #define MODEM_PRE_STRING "/dev/ttyACM"
  #define PHONE_PRE_STRING "/dev/ttyUSB"
  #define USB_PRE_STRING   "/dev/ttyIMC"
#elif __APPLE__
  #define MODEM_PRE_STRING "/dev/cu.usbmodem"
  #define PHONE_PRE_STRING "/dev/cu.usbserial"
  #define USB_PRE_STRING   "/dev/cu.usbmodem" //< TODO: Make this work!
#endif

#ifndef _WIN32
  extern "C" {
    unsigned int GetTickCount( void );
  }
#endif

string ComPortName(string port = "0");

string GetUserHomePath();
string FileGetName(string& path);
string FileGetName(const char* path);
string FileGetPath(string& path);

/* String functions (Note: template functions need to be defined in the header) */
//string& replace(string& str, const string what, const string into);
vector<string> split(const string& str, const string& delimiters = " ");

/**
 * Dumps data or a single char into a HEX string
 */
string data2hex(const char* data, size_t size);
string char2hex(const char c);

/**
 * Returns either the char if inside the ASCII table,
 * or returns a symbol string: <CR>, <LF>, <ESC> or the
 * HEX equivalent, using the char2hex() function
 */
string getChar(unsigned char c);

/**
 * Dumps the data using the getChar() function
 */
//string dumpVector(VectorPtr data);
string dumpData(string data);
string dumpData(const char* data, size_t len);

/**
 * Creates a timestamp in this format "Hours:Minutes:Seconds"
 **/
string timestamp();

char* timestamp_date();

/* File System */
bool DeviceExist(string filename);

/* File I/O */
vector<string> FindFilesContaining(const string& path, const string& str);
vector<string> GetList(string path);
vector<string> GetDeviceList();
vector<string> GetModemList();

#endif /* UTILS_H_ */


