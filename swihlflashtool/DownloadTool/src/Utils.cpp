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
       $File name:  /msw_tools/FlashTool/DownloadTool/src/Utils.cpp $
       $CC-Version: .../dhkristx_140314_v0/3 $
       $Date:       2014-05-02    13:00:39 UTC $
    ------------------------------------------------------------------------- */

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>         //< getenv

#include "Utils.h"
#include "FileIO.h"

#ifdef _WIN32
  #include "WinRegistry.h"
  using namespace ipa;
#else
  #include "OS.h"
#endif

using namespace std;

#ifdef __EXCEPTIONS
  #include <stdexcept>
  #define throw_logic_error(str) throw logic_error(str)
#else
  static void throw_logic_error(string str)
  {
    fprintf(stderr,"%s\n", str.c_str() );
  }
#endif

/**
 * Converts a user input Comport name into its real value
 */
string ComPortName(string port)
{
  if (isdigit(port[0]))
    return port;

  if (port[0] == '{' || port[0] == '"')
    return string("1");

  if (port.find("u") == 0)
    port = string(USB_PRE_STRING).append( port.substr(1) );
  #ifdef _WIN32
    else if (port.find("USB") == 0)
      port = string(USB_PRE_STRING).append( port.substr(3) );
    else if (port.find("COM") == 0)
      port = string(PHONE_PRE_STRING).append( port.substr(3) );
  #endif

  return port;
}

/**
 * Finds the user home path
 */

#if 0 // Moved to libFlsTool/Helpers.cpp

string GetUserHomePath()
{
  const char* home = getenv("HOME");
  if (home or (home = getenv("USERPROFILE")))
    return string(home);

  const char* hdrive = getenv("HOMEDRIVE");
  if (!hdrive or !(home = getenv("HOMEPATH")))
  {
    throw_logic_error("Could not find user home directory");
    return "";
  }

  return string(hdrive) + string(home);
}

string FileGetName(string& path)
{
  size_t pos = path.rfind( SEPARATOR );
  if (pos != string::npos)
    return path.substr(pos+1);
  return path;
}

string FileGetName(const char* _path)
{
  string path = _path;
  return FileGetName(path);
}

string FileGetPath(string& path)
{
  size_t pos = path.rfind( SEPARATOR );
  if (pos != string::npos)
  return path.substr(0, pos);
  return path;
}

/**
 *  String replacing
 *
 * TODO: Add method to the string class (if possible)
 * 	string& replace(string& what, string& into);
 * 	string& replace(string& what, string& into);
 **
string& replace(string& str, const string what, const string into)
{
  size_t colon = str.find(what);
  while (colon != string::npos) {
    str.replace(colon, 1, into);
    colon = str.find(what);
  }
  return str;
}
*/

/**
 * Splitting a string
 */
vector<string> split(const string& str, const string& delimiters)
{
  vector<string> parts;
  size_t pos, lastPos = 0;
  while (true)
  {
    pos  = str.find_first_of(delimiters, lastPos);
    if (pos == string::npos)
      break;

    if (pos != lastPos)
      parts.push_back( string(str.data()+lastPos, pos-lastPos) );

    lastPos = pos+1;
  }
  // Adding the last part
  parts.push_back( string(str.data()+lastPos) );
  return parts;
}
#endif

/**
 * Dumping Utility Functions
 *
 * string dumpString(string data)                    - Dumps a data string
 * string dumpData(const char* data, size_t len)     - Dumps a data blob
 *
 * string getChar(unsigned char c)                   - Converts special chars (\n\r\t)
 * string char2hex(const char c)                     - Converts a single char to its hex number
 *
 * string data2hex(const char* data, size_t size)    - Formats a string of HEX numbers (use dumpData instead)
 **/

string dumpData(string data)
{
  return dumpData(data.c_str(), data.size());
}

string dumpData(const char* data, size_t len)
{
  string line;
  for(size_t i=0; i<len; ++i)
    line += getChar(data[i]);
  return line;
}

string getChar(unsigned char c)
{
  if ((c < 32) | (c > 126)) {
    switch (c) {
      case 9:   return "<TAB>";
      case 10:  return "<LF>";
      case 13:  return "<CR>";
      case 27:  return "<ESC>";
      default:  return char2hex(c);
    }
  }
  return string(1,c);
}

string char2hex(const char c)
{
    stringstream ss (stringstream::in | stringstream::out);
    ss << uppercase << setw(2) << setfill('0')
       << hex << short(static_cast<unsigned char>(c));
    return ss.str();
}

// DEPRECATED - use dumpData() instead
string data2hex(const char* data, size_t size)
{
    string h;
    stringstream ss (stringstream::in | stringstream::out);
    for (size_t i=0; i<size; ++i)
        ss << uppercase << setw(2) << setfill('0')
           << hex << short(static_cast<unsigned char>(data[i])) << " ";
    h = ss.str();
    h.resize(h.length()-1);
    return h;
}

/**
 * Time stamp Utilities
 *
 *  string timestamp()      - Returns a timestamp e.g. "00:13:37"
 *  string timstamp_date()  - Returns a date timestamp "Sat May 20 15:21:51 2000"
 */
string timestamp()
{
  char buffer[64];
  time_t now = time(0);
  struct tm* tm = localtime(&now);
  if (!tm)
    return string("");

  strftime(buffer, 64, "%H:%M:%S", tm);
  return string(buffer);
}

char* timestamp_date()
{
  time_t now = time(0);
  struct tm* tm = localtime(&now);
  if (!tm)
    return (char*)"";
  return asctime(tm);
}

/**
 * Filesystem/device Utilities
 *
 * bool FileExist(string filename)         - Does a file exist?
 * bool DeviceExist(string port)           - Same as above for *nix systems, but different for Windows
 *
 **/

bool DeviceExist(string port)
{
  #ifndef _WIN32
    if (!fexists(port))
  #else
    if (!winreg::device_exist(port))
  #endif
  {
    throw_logic_error("Device "+ port +" does not exist!");
    return false;
  }
  return true;
}

/**
 *  Generic File location Utilities
 *
 * vector<string> FindFilesContaining(const string& path, const string& str)  - The main function
 * vector<string> GetList(string path)     - Gets a list of files in the path
 * vector<string> GetDeviceList()          - Returns a list of Devices on the machine
 * vector<string> GetModemList()          - Returns a list of Modems on the machine
 *
 **/
#ifndef _WIN32
  #include <dirent.h>

  vector<string> FindFilesContaining(const string& path, const string& str)
  {
    vector<string> matches;
    DIR* dir = opendir( path.c_str() );
    if(dir)
    {
      struct dirent *ent;
      while((ent = readdir(dir)))
      {
        string name = ent->d_name;
        if (name.find( str ) != string::npos)
          matches.push_back( string(path).append(name) );
      }
    }
    return matches;
  }

  vector<string> GetList(string path)
  {
    size_t last_sep = path.rfind('/')+1;
    string dir = path.substr(0, last_sep);
    string str =  path.substr(last_sep);
    return FindFilesContaining(dir, str);
  }

  vector<string> GetDeviceList() {
    return GetList(PHONE_PRE_STRING);
  }

  vector<string> GetModemList() {
    return GetList(MODEM_PRE_STRING);
  }
#endif
