/*  -------------------------------------------------------------------------
    Copyright (C) 2011-2014 Intel Mobile Communications GmbH

         Sec Class: Intel Confidential (IC)

     ----------------------------------------------------------------------
     This document contains proprietary information belonging to IMC.
     Design A/S. Passing on and copying of this document, use and communication
     of its contents is not permitted without prior written authorisation.
     ----------------------------------------------------------------------
     Description:
     Uses the sysfs to find information about devices attached to the system.
     ----------------------------------------------------------------------
     Revision Information:
       $$File name:  /msw_tools/FlashTool/libFlsTool/src/WinRegistry.cpp $
       $$CC-Version: .../dhkristx_140314_v0/1 $
       $$Date:       2014-04-16    12:25:17 UTC $
     ---------------------------------------------------------------------- */

#ifdef WIN32

#include "WinRegistry.h"

#include <windows.h>
#include <stdio.h>

#include <iostream>
#include <stdexcept>

using namespace std;
using namespace ipa;

namespace ipa {
namespace winreg {

string get_key_value(string key, string param)
{
  HKEY hKey;
  TCHAR value[MAX_PATH] = { 0 };
  DWORD val_size = sizeof(value);

  //cout << "WinReg> Opening " << key << endl;
  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, key.c_str(), 0, KEY_READ, &hKey) == 0)
  {
    //cout << "WinReg> Saving parameter " << param << endl;
    RegQueryValueEx(hKey, param.c_str(), NULL, NULL, (BYTE*)value, &val_size);
    //cout << "WinReg> Value = " << value << endl;
  }

  return string(value);
}


string find_device(uint16_t vid, uint16_t pid, int number /* = 1 */)
{
  string found = "";
  HKEY hKey, hSubKey;
  DWORD nr;
  TCHAR name[MAX_PATH];
  TCHAR key_name[MAX_PATH];
  snprintf(key_name, MAX_PATH,
           "SYSTEM\\CurrentControlSet\\Enum\\USB\\VID_%04X&PID_%04X", vid, pid);

  //cout << "WinReg> Opening " << key_name << endl;
  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, key_name, 0, KEY_READ, &hKey) == 0)
  {
    for (nr=0; RegEnumKey(hKey, nr, name, MAX_PATH) == 0; ++nr)
    {
      string SubKeyName = string(key_name) + "\\" + string(name);
      string SubKeyPath = SubKeyName + "\\Control";

      //cout << " Looking for " << SubKeyName << ", Nr: " << number << endl;
      if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, (char*)SubKeyPath.c_str(), 0, KEY_READ, &hSubKey) != 0)
        continue; //< Device ONLINE

      RegCloseKey(hSubKey);
      if (number-- > 1)
        continue; //< Requested Device Count

      SubKeyPath = SubKeyName + "\\Device Parameters";
      if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, (char*)SubKeyPath.c_str(), 0, KEY_READ, &hSubKey) == 0)
      {
        TCHAR value[MAX_PATH] = { 0 };
        DWORD val_size = sizeof(value);

        //< Prolific: PortName (COMx)
        if (RegQueryValueEx(hSubKey, "PortName", NULL, NULL, (BYTE*)value, &val_size) == 0)
        {
          found = value;
        }

        //< FlashUSB: CreateFileName (USBx)
        if (RegQueryValueEx(hSubKey, "CreateFileName", NULL, NULL, (BYTE*)value, &val_size) == 0)
        {
          if (value[0] != 0 && string("USB0").compare(value) != 0)
          {
            found = SubKeyName;
          }
        }
        /*
        //< WinUSB: SymbolicName (\??USB#VID_xxxx&PID_xxxx#x&xxxxxxxx&x#<GUID>
        if (RegQueryValueEx(hSubKey, "SymbolicName", NULL, NULL, (BYTE*)value, &val_size) == 0)
        {
          found = value;
        }*/
        RegCloseKey(hSubKey);
      }
    }
    RegCloseKey(hKey);
  }

  return found;
}


bool device_exist(string& port)
{
  if (port.compare(0,4,"\\\\.\\") == 0)
    port = port.substr(4);

  if (port.compare(0,3,"COM") != 0) {
    cout << "   Checking for USB device Not yet supported (returning true)" << endl;
    return true;
  }
  //cout << "Checking if device " << port << " exists..." << endl;

  string KeyName = "Hardware\\DeviceMap\\SerialComm";

  HKEY hKey;
  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, (char*)KeyName.c_str(), 0, KEY_READ, &hKey) == 0)
  {
    TCHAR name[MAX_PATH];
    DWORD nr, key_size;
    for (nr=0, key_size=MAX_PATH;
        RegEnumValue(hKey, nr, name, &key_size, NULL, NULL, NULL, NULL) == 0;
        ++nr, key_size=MAX_PATH)
    {
      //cout << "  Name " << nr << ": " << name << endl;
/* SWISTART */
#if 0
      bool relay = (string(name).compare(8,6,"USBSER") == 0);
      bool Prolific = (string(name).compare(8,14,"ProlificSerial") == 0);
      bool Comneon = (string(name).compare(8,8,"cocdcacm") == 0);
#endif /* 0 */
/* SWISTOP */

      TCHAR value[MAX_PATH];
      DWORD val_size = MAX_PATH;
      if (RegQueryValueEx(hKey, name, NULL, NULL, (BYTE*)value, &val_size) == 0)
      {
        //cout << "    Data: " << value << endl;
        if (port.compare(value) == 0)
        {
/* SWISTART */
#if 0
          if (!Prolific && !Comneon && !relay)
          {
            if (string(name).compare(8,6,"Serial") == 0)
              cerr << "Warning: Please upgrade your prolific driver!" << endl;
            else
              throw logic_error("Comport "+ port +" found, but does not look like a known device (Prolific/k8090)!");
          }
#endif /* 0 */
/* SWISTOP */
          RegCloseKey(hKey);
          return true;
        }
      }
    }
    RegCloseKey(hKey);
  }

  //return false;
  throw logic_error("Device "+ port +" does not exist!");
}

#if 0
string WinReg_find_k8090(int number /* = 1 */)
{
  HKEY hKey;
  string found = "";
  string KeyName = "Hardware\\DeviceMap\\SerialComm";

  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, (char*)KeyName.c_str(), 0, KEY_READ, &hKey) == 0)
  {
    TCHAR name[MAX_PATH];
    DWORD nr, key_size;
    for (nr=0, key_size=MAX_PATH;
        RegEnumValue(hKey, nr, name, &key_size, NULL, NULL, NULL, NULL) == 0;
        ++nr, key_size=MAX_PATH)
    {
      //cout << "  Name " << nr << ": " << name << endl;
      bool relay = (string(name).compare(8,6,"USBSER") == 0);

      // if (strncmp(name[8], 6, "USBSER") == 0)
      if (relay && number-- == 1)
      {
        TCHAR value[MAX_PATH];
        DWORD val_size = MAX_PATH;
        if (RegQueryValueEx(hKey, name, NULL, NULL, (BYTE*)value, &val_size) == 0)
        {
        found = value;
        }
        break;
      }
    }
    RegCloseKey(hKey);
  }

  return found;
}
#endif
}
}

/* ANSI C interface */
const char* WinReg_get_key_value(const char* key, const char* param)
{
  string ret = winreg::get_key_value(key, param);
  return ret.c_str();
}

const char* WinReg_find_device(uint16_t vid, uint16_t pid, int number)
{
  string ret = winreg::find_device(vid,pid,number);
  return ret.c_str();
}

uint8_t WinReg_device_exist(const char* port)
{
  string m_port = port;
  return winreg::device_exist(m_port);
}

#endif // WIN32
