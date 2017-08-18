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
       $File name:  /msw_tools/FlashTool/DownloadTool/src/IMCDownload.cpp $
       $CC-Version: .../dhkristx_140314_v0/7 $
       $Date:       2014-05-02    13:16:50 UTC $
    ------------------------------------------------------------------------- */

#include "targetver.h"
#include "IMCDownload.h"

#include "FlsFile.h"

#include "ZIP.h"
#include "INI.h"
#include "Utils.h"
#include "SysFs.h"
#include "FileIO.h"
#include "StringExt.h"
#include "Exceptions.h"

//#define DEBUG

#ifdef _WIN32
  #include <windows.h>
  #include "WinRegistry.h"

  #define MAX_KEY_LENGTH 255
  #define MAX_VALUE_NAME 16383
#else
  #include <unistd.h>    //< close()

  #include "SysFs.h"
#endif

#include <csignal>
#include <cstdarg>
#include <cstring>      //< memset()

#include <iomanip>

#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>     //< std::auto_ptr
#include <set>

typedef enum
{
  download_fls,
  download_eep,
  download_dffs,
  download_cust,
  download_addon,
  download_bin
} downloadtype;

#define MAX_NAME_LENGHT 80
#define NUM_MEM_CLASSES 18

using namespace std;
using namespace ipa;
using namespace ipa::fls;

//------------------------------------------------------------------------------
// The IFXDownload singleton (see singleton design pattern)
//   This is needed because the callback cannot be a member function
//------------------------------------------------------------------------------
DownloadTool* DownloadTool::instance = 0;

DownloadTool& DownloadTool::getInstance(std::string library)
{
/*
  cout << "Modem List: ";
  foreach(string modem, GetModemList())
      cout << modem << " ";
  cout << endl;

  cout << "Device List: ";
  foreach(string device, GetDeviceList())
      cout << device << " ";
  cout << endl;
*/
  if (instance == 0)
    instance = new DownloadTool(library);

  return *instance;
}

/*------------------------------------------------------------------------------
  Download Member function Callback

  Note:
   A singleton is required because the C style callback function cannot be
   referenced to a C++ style function :(
   Instead a singleton design pattern is used to retrieve the IFXDownload
   member functions and variables.
------------------------------------------------------------------------------*/

unsigned long ts,last_ts = 0, bandwidth = 0;
unsigned long total, bytes, last_bytes;
unsigned long sample_count, sample_total;

void DlCallBack(uint8_t channel, IFWD_DL_status_enum state, char* text)
{
  std::string str = text ? text : "";
  DownloadTool* dl = &DownloadTool::getInstance();

  switch(state)
  {
  case IFWD_DL_ProgressUpdated:
  {
     unsigned long average_bandwidth = 0;
     int procent = atoi(str.c_str());

     if (!procent || procent == 100)
     {
       // Saving information, to later calculate bandwidth
       sample_count = sample_total = 0;
       last_ts = last_bytes = 0;
       //total = file->size();
     }

     // Calculating bandwidth
     ts = GetTickCount();
     bytes = (procent*total/100);

     if ((ts-last_ts)>0 && (bytes-last_bytes)>0)
     {
       bandwidth = (bytes-last_bytes)/(ts-last_ts)*8;

       // Update with current sample
       if (bandwidth)
       {
         if (sample_count++)
         {
           sample_total += bandwidth;
           average_bandwidth = sample_total/(sample_count-1);
         }
       }
     }

     last_bytes = bytes;
     last_ts = ts;

     dl->signal_progress_update( procent, bandwidth );

     str += "%";
     if (average_bandwidth)
     {
       if (average_bandwidth > 1024)
         str += " ("+ Stringify(double(average_bandwidth)/1024, 2) +" Mb/s)";
       else
         str += " ("+ Stringify(average_bandwidth, 2) +" kb/s)";
     }
     break;
  }
  case IFWD_DL_ProcessOutlineUpdated:     dl->signal_outline_update( str );               break;
  case IFWD_DL_OK:                        dl->signal_status( int(channel) );             break;
  case IFWD_DL_Error:                     dl->signal_error( str );                        break;
  case IFWD_DL_AT_Command:                dl->signal_AT_command( str );                   break;

  // TODO: Make handles
  case IFWD_DL_ProcessDetailUpdated: break;
  case IFWD_DL_PhoneOn: break;
  case IFWD_DL_status_force_int_enum: break;
  }

  dl->signal_library_callback(int(channel), int(state), str);
}


void DownloadTool::OkCallBack(int channel)
{
  //cout << " *** Data received *** " << endl;
  synchronized = true;
  channel_state[channel] = CHANNEL_STATE_SYNCED;
}

void DownloadTool::LibraryCallBack(int channel, int state, const string& text)
{
  if (text.compare("Device synchronized.") == 0)
  {
    synchronized = true;
    //cout << "\t CHANNEL " << channel << " SYNCED!" << endl;
    channel_state[channel] = CHANNEL_STATE_SYNCED;
  }
  else if (state == IFWD_DL_Error
           || text.compare(0, 13, "Target error[") == 0)
  {
    std::copy_n("Error recieved from EBL", 24, ErrorText);
    //std::copy(text.begin(), text.end(), ErrorText);
    channel_state[channel] = CHANNEL_STATE_FAILED;
  }
  else if (state == IFWD_DL_OK)
  {
    // cout << "   GotOkCallback> " << text << endl;
    channel_state[channel] = CHANNEL_STATE_DONE;
  }
  return;
#if 0
  if (text.compare("Boot process finished\n") == 0)
  {
    cout << "\t CHANNEL " << channel << " BOOTED!" << endl;
    channel_state[channel] = CHANNEL_STATE_BOOTED;
  }
  else if (text.find("Start downloading item") == 0)
  {
    cout << "\t CHANNEL " << channel << " Downloading..." << endl;
    channel_state[channel] = CHANNEL_STATE_DOWNLOADING;
  }
  else if (text.find("Process time was") == 0)
  {
    cout << "\t CHANNEL " << channel << " Done..." << endl;
    channel_state[channel] = CHANNEL_STATE_IDLE;
  }
  else if (state == IFWD_DL_Error)
  {
    cout << "\t CHANNEL " << channel << " Error" << text << endl;
    channel_state[channel] = CHANNEL_STATE_FAILED;
  }
#endif
}

//------------------------------------------------------------------------------
// Debugging functions
//------------------------------------------------------------------------------
void Log(const char* format, ...)
{
  char buffer[500];
  va_list args;
  va_start(args, format);

  vsprintf(buffer,format, args);
  //clog << buffer << endl;
  cout << buffer << endl;
  va_end(args);
}


//----------------------------------------------------------------------------------
// The IFXDownload Class
//----------------------------------------------------------------------------------

DownloadTool::DownloadTool(string library)
  : m_channel(1),
    m_relay(""),
    m_trace_file(""),
    m_tracing(false),
    m_baudrate(921600),
    m_boot_timeout(60000),
    m_comm_timeout(30000),
    #ifdef DEBUG
      m_verbose(VERBOSE_DEBUG),
    #else
      m_verbose(false),
    #endif
    comport_open(false),
    power_button(0),
    reset_button(0),
    extern_relay(false)
{
  SetLibrary(library);

  memset(ErrorText,'\0',  500);
  IFWDfunction( dl->init_callback(DlCallBack, ErrorText) );

  signal_library_callback.connect( MemFun(&DownloadTool::LibraryCallBack, this) );
  signal_status.connect( MemFun(&DownloadTool::OkCallBack, this) );

  m_OnBootCtrl.DTR = 0;
  m_OnBootCtrl.RTS = 0;
  m_OnBootCtrl.CTS = 0;
  m_OnBootCtrl.DSR = 0;
  m_OnBootCtrl.RING = 0;
  m_OnBootCtrl.RLSD = 0;

  /*
   * == Direct callback Tested and failed :'( ==
   *
   * A singleton is required because the C style callback function cannot be
   * referenced to a C++ style function :(
   *
    callback_func* callb = cb.target<callback_func>();
    if (callb) {
      cout << "Setting member callback function :)" << endl;
      IFWDfunction( dl->init_callback( *callb, ErrorText) );
    }
  */
}

void DownloadTool::SetLibrary(std::string& library)
{
  if (m_verbose >= VERBOSE_DEBUG)
    cout << "Setting Library " << library << "..." << endl;

  dl = new LibraryLoader( library );

  // Saving the library version: e.g. 4.XX
  string version = dl->get_version();

  // TODO: Fix this. Float makes 4.100 less than 4.98!
  lib_version = StringTo<float>(version);
}

string DownloadTool::GetVersion()
{
  return dl->get_version();
}

char* DownloadTool::DeviceMap(int id)
{
  ThreadType relay_thread;

  if (!dl->device_map)
    throw_exception("The USB Device Mapper is supported from version 4.107"
                        " of the Download Library.\n", NULL);

  THREAD_create(&relay_thread, &DownloadTool::PowerTargetSync, this);

  char *path = dl->device_map( id );
  synchronized = true;

  THREAD_join(relay_thread);

  return path;
}

bool DownloadTool::LoadDeviceMap(string map)
{
  if (!dl->device_map_load)
    throw_exception("The USB Device Mapper is supported from version 4.107"
                        " of the Download Library.\n", false);
  return dl->device_map_load( map.c_str() );
}

bool DownloadTool::SaveDeviceMap(string map)
{
  if (!dl->device_map_save)
    throw_exception("The USB Device Mapper is supported from version 4.107"
                        " of the Download Library.\n", false);
  return dl->device_map_save( map.c_str() );
}

string DownloadTool::GetSystemVersion(const string& file)
{
  char sys_version[1024];
  memset(sys_version, 0, sizeof(sys_version));

  if (dl->get_sys_version((char*)file.c_str(), IFWD_DL_sys_version_all, (char*)&sys_version, sizeof(sys_version)))
    throw_exception("Could not retrieve the System SW Version", sys_version);

  //cout << "  System SW Version:" << endl << sys_version << endl;
  return sys_version;
}

/*
float IFXDownload::GetLibraryVersion()
{
  return lib_version;
}
*/
void DownloadTool::CloseComm()
{
  //if (m_verbose >= VERBOSE_DEBUG)
  //  cout << "Unsetting Library" << endl;

  dl->stop_AT_mode(m_channel, ErrorText);
  StopTrace();
  if(comport_open)
  {
    IFWDfunction( dl->close_comm_port(m_channel, ErrorText) );
    comport_open = false;
  }
  // Some times if freezes here, so I just removed it...
  // It will be deleted as soon as the program ends anywho.
  //if (dl) delete dl;
}

DownloadTool::~DownloadTool()
{
  dl->stop_AT_mode(m_channel, ErrorText);
  if (m_tracing)
    StopTrace();
  if(comport_open)
  {
    dl->close_comm_port(m_channel, ErrorText);
    comport_open = false;
  }

  //delete dl;
}

// Config files: INI, JSON, XML

/**
 * Loading a config file with INI format.
 * If the file does not exist then it is created.
 *
 * @param file = path for the config INI file
 */
static int ini_handler(void*arg, const char* section,
                          const char* key, const char* value)
{
  string Key = key;
  string Val = value;
  DownloadTool* tool = static_cast<DownloadTool*>(arg);

  if (StringCaseCompare(key, "comport"))
    tool->SetComPort( value );

  else if (StringCaseCompare(Key, "guid"))
    tool->SetParameter(IFWD_DL_dll_parameter_set_winusb_guid, (uintptr_t)value);

  else if (StringCaseCompare(Key, "relay"))
    tool->SetRelayPort( value );

  else if (StringCaseCompare(Key, "trace"))
    tool->SetTraceFile( value );

  else if (StringCaseCompare(Key, "baudrate"))
    tool->SetBaudrate( strtoul(value, NULL, 0) );

  else if (StringCaseCompare(Key, "verbose"))
    tool->SetVerbose( strtoul(value, NULL, 0), "" );

  else if (StringCaseCompare(Key, "boot-timeout"))
    tool->SetBootTimeout( strtoul(value, NULL, 0) );

  else if (StringCaseCompare(Key, "comm-timeout"))
    tool->SetCommTimeout( strtoul(value, NULL, 0) );

  return true;
}

void DownloadTool::loadConfigIni(std::string file)
{
  if (!fexists(file))
    throw_exception("Cannot load config file: "+ file);

  if (!ParseINI(file.c_str(), &ini_handler, this))
    throw_exception("Loading INI config file failed: "+ file);
}

void DownloadTool::saveConfigIni(string file)
{
  string ini = ";---------------------------------;\n"
                ";    DownloadTool config file    ;\n"
                ";---------------------------------;\n"
                "\n[Global]\n";

  ini += "\n[comport]\n";      ini += m_comport;
  ini += "\n[baudrate]\n";     ini += m_baudrate;
  ini += "\n[trace]\n";        ini += m_trace_file;
  ini += "\n[relay]\n";        ini += m_relay;
  ini += "\n[boot-timeout]\n"; ini += m_boot_timeout;
  ini += "\n[comm-timeout]\n"; ini += m_comm_timeout;

  FILE_setContent(file.c_str(), ini.c_str(), ini.length());
}


string DownloadTool::RelayPortName(string port)
{
  if (port=="?")
  {
    #ifdef __linux__
      char* tty = NULL;
      char* ptr = SysFs_find_device(0x10cf,0x8090,1);
      if (ptr)
        tty = SysFs_get_device_tty(ptr);
      port = (ptr && tty) ? string("/dev/").append(tty) : "0";
    #else
      port = winreg::find_device(0x10cf,0x8090);
    #endif
  }

  if (isdigit(port[0]))
    port = string(MODEM_PRE_STRING).append( port );

  if (DeviceExist(port))
    return port;
  return string("");
}


/**
 *  Executing a function from the "Download Library"
 **/
bool DownloadTool::IFWDfunction(IFWD_DL_status_enum Status)
{
  LastStatus = Status;

  if(Status == IFWD_DL_Error)
  {
    if(ErrorText[0] == 0)
      strcpy(ErrorText, "No error recieved from Download Library");

    throw_exception(ErrorText, false);
  }
  return true;
}



//------------------------------------------------------------------------------
void DownloadTool::SetVerbose(int verbose, string debug_modules)
{
  m_verbose = verbose;

  if (!debug_modules.empty())
  {
    if (!dl->set_module_verbose)
      throw_exception("The individual module debugging is supported from version 4.119"
                      " of the Download Library.\n");

    // Setting verbosity for modules: BOOT,SFH,MFH,COMM,USB
    if (debug_modules.find("BOOT") != string::npos)
      dl->set_module_verbose(MODULE_BOOT, verbose);

    if (debug_modules.find("SFH") != string::npos)
      dl->set_module_verbose(MODULE_SFH, verbose);

    if (debug_modules.find("MFH") != string::npos)
      dl->set_module_verbose(MODULE_MFH, verbose);

    if (debug_modules.find("COMM") != string::npos)
      dl->set_module_verbose(MODULE_COMM, verbose);

    if (debug_modules.find("USB") != string::npos)
      dl->set_module_verbose(MODULE_USB, verbose);

    if (debug_modules.find("ALL") != string::npos)
      dl->set_module_verbose(MODULE_ALL, verbose);
  }
}

void DownloadTool::SetParameter(IFWD_DL_dll_parameter_enum param, int value)
{
  IFWDfunction( dl->set_parameter(param, value, ErrorText) );
}

int DownloadTool::GetParameter(IFWD_DL_dll_parameter_enum param)
{
  unsigned int value = 0;

  cout << "Get parameter " << int(param) << endl;

  if (!dl->get_parameter)
    throw_exception("IFWD_DL_get_dll_parameter function not found in library.", 0);

  IFWDfunction( dl->get_parameter(param, &value, ErrorText) );
  cout << "  => value: " << value << endl;

  return int(value);
}

/* SWISTART */
#ifndef SIERRA_CTF
/**
 * Out Of Session Functions
 **/
void DownloadTool::OutOfSessionCommit()
{
  if (m_verbose >= VERBOSE_DEBUG)
    cout << "OutOfSession> Commit!" << endl;

  IFWDfunction( dl->out_of_session_comm(m_channel,
                          IFWD_DL_oosc_action_commit,
                          IFWD_DL_oosc_type_secure_area, 0, 0, NULL) );

  if (m_verbose >= VERBOSE_OVERALL)
    cout << "OutOfSession> Success!" << endl << endl;
}

bool DownloadTool::OutOfSessionErase(IFWD_DL_oosc_type_enum type)
{
  if (m_verbose >= VERBOSE_OVERALL)
    cout << "OutOfSession> Erasing area " << int(type) << "..." << endl;

  IFWDfunction( dl->out_of_session_comm(
                          m_channel, (uint32_t)IFWD_DL_oosc_action_erase, (uint32_t)type,
                          0, 0, NULL) );
  return true;
}

bool DownloadTool::OutOfSessionWrite(IFWD_DL_oosc_type_enum type, string path)
{
  vector<char> content;
  FILE_getContent(path, content);

  if (content.size() == 0)
    throw_exception("File "+ path +" is either empty or non-existing!", false);

  //if ((content.size() % 4) == 0)
  //  throw_exception("Certificate is not 32bit aligned!", false);

  if (m_verbose >= VERBOSE_OVERALL)
    cout << "OutOfSession> Writing area " << int(type) << " from " << path
         << " (Size: " << content.size() << ")" << "..." << endl;

  IFWDfunction( dl->out_of_session_comm(
                          m_channel, IFWD_DL_oosc_action_write, type,
                          content.size(), 0, (uint8_t*)&content[0]) );

  if (m_verbose >= VERBOSE_DEBUG)
    cout << "OutOfSession> Data ["
         << data2hex(&content[0], content.size()) << "]"
         << endl;

  return true;
}

bool DownloadTool::OutOfSessionRead(IFWD_DL_oosc_type_enum type, string file)
{
  int len = 0;
  IFWDfunction( dl->out_of_session_comm(
                          m_channel, IFWD_DL_oosc_action_read, type,
                          0, 0, (uint8_t*)&len) );

  if (len == 0)
    throw_exception("Could not get length of OutOfSession type "
                    +Stringify((int)type), false);

  char* data = new char[len+1];
  if (!data)
    throw_exception("Memory allocation", false);

  memset(data, 1, len+1);
  if (m_verbose >= VERBOSE_OVERALL)
    cout << "OutOfSession> Reading area " << int(type) << " into " << file
         << " (" << len << ")" << "..." << endl;

  IFWDfunction( dl->out_of_session_comm(
                          m_channel, IFWD_DL_oosc_action_read, type,
                          len, 0, (uint8_t*)data) );

  if (data && m_verbose >= VERBOSE_DEBUG)
    cout << "OutOfSession> Data ["
         << data2hex(data, len) << "]"
         << endl;

  FILE_setContent(file.c_str(), data, len);
  delete[] data;

  return true;
}

string DownloadTool::OutOfSessionGetHwDetails(string hwid)
{
  if (m_verbose >= VERBOSE_DLL)
    cout << "OutOfSession> Getting HW Details..." << endl;

  string path = FileSystem_path(GetUserHomePath(), "Intel", "HWID") + SEPARATOR;
  if (!FileSystem_isdir(path))
    FileSystem_mkdir(path);

  string tmp = path + "hwid.bin";

  OutOfSessionRead(IFWD_DL_oosc_type_hw_details, tmp);

  string content = FILE_getContent(tmp.c_str());
  if (!content.empty())
  {
    //< Generate HWID filename "xxxxxxxxxxxxxxxx.hwd"
    stringstream ss;
    for (size_t i=0; i<content.size() && i<16; ++i)
      ss << uppercase << setw(2) << setfill('0') << hex << (content[i] & 0xFF);
    ss << ".hwd";

    // No name -> Output: "xxxxxxxxxxxxxxxx.hwd"
    if (hwid.empty())
      hwid = ss.str();

    // Directory provided -> Output: "<path>/xxxxxxxxxxxxxxxx.hwd"
    if (FileSystem_isdir(hwid))
      hwid = FileSystem_path(hwid, ss.str());

    // Finally if file exists, remove
    if (FileSystem_isfile(hwid))
        unlink(hwid.c_str());

    // Move the hwid.bin to output location
    if (!FileSystem_move(tmp, hwid))
      throw_exception("Failed to move "+ tmp +" to "+ hwid, hwid);
      // +" "+ Stringify(errno) +": "+ string(strerror(errno)), hwid);

    if (m_verbose >= VERBOSE_OVERALL)
      cout << "OutOfSession> HW Detail info file: " << hwid << endl;
  }
  return hwid;
}
#endif /* SIERRA_CTF */
/* SWISTOP */


/* SWISTART */
#ifndef SIERRA
/**
 * Flashless Functions
 **/
void DownloadTool::SetupFlashlessErase(string& path, string erase)
{
  int options = 0x00;
  if (erase.find("all") != string::npos)
    options = FLASHLESS_STATIC_ERASE | FLASHLESS_DYNAMIC_ERASE | FLASHLESS_CALIB_ERASE;

  if (erase.find("static") != string::npos)
    options |= FLASHLESS_STATIC_ERASE;

  if (erase.find("dynamic") != string::npos)
    options |= FLASHLESS_DYNAMIC_ERASE;

  if (erase.find("calib") != string::npos)
    options |= FLASHLESS_CALIB_ERASE;

  if (m_verbose >= VERBOSE_DLL)
    cout << "Setting up FlashLess NVM"
         << "(Path: " << path << ", Erasing: " << erase << ")" << endl;

  IFWDfunction( dl->NVM_set_flashless_option((uint8_t*)path.c_str(), options) );
}
#endif /* SIERRA */
/* SWISTOP */

/**
 * Prolific GPIO reset feature
 *
 * The gpio mask is generated
 */
void DownloadTool::SetupProlificReset(string& prolific_port, vector<int> gpio)
{
// TODO: fix this check comparing float makes 4.100 less than 4.99
//  if (lib_version <= 4.98)
//    throw_exception("Please upgrade to Download Library v.4.99 or above to use this feature.");

  int gpio_mask = 0;
  for (size_t i=0; i<gpio.size(); ++i)
      gpio_mask ^= 1 << gpio[i];

  char* port = (char*)ComPortName(prolific_port).c_str();

  if (m_verbose >= VERBOSE_DLL)
    cout << "Setting up Prolific GPIO reset (Port: " << port
         << ", Mask: 0x" << Stringify(gpio_mask, 2) << ")" << endl;

  IFWDfunction( dl->setup_prolific_gpio_reset(m_channel, port, gpio_mask, ErrorText) );
  extern_relay = true;
}

/**
 * ReqForceHwReset
 */
void DownloadTool::ForceTargetReset()
{
  if (!comport_open)
    throw_exception("ForceTargetReset failed - comport not open");

  if (m_verbose >= VERBOSE_ACTIONS)
    cout << "Forcing target SW reset..." << endl;

  IFWDfunction( dl->force_target_reset(m_channel, 1, 16, 17, 0, ErrorText) );
}

/**
 * SetupNvmOptions()
 *
 * Setup the NVM erase options for either Calibration, Fixed or Dynamic area.
 * For erasing all areas the keyword "all" can be used.
 *
 * The fls files have to be loaded before calling this function, because
 * when loading fls files the available_nvm_tags will be populated.
 **/
int isInSet(set<uint32_t> list, const uint32_t value)
{
  int index = 0;
  set<uint32_t>::iterator it;
  for (it=list.begin(); it!=list.end(); it++,index++)
    if (*it == value)
      return index;
  return -1;
}

/* SWISTART */
#ifndef SIERRA
void DownloadTool::SetupNvmErase(string erase)
{
  if (downloadlist.empty())
    throw_exception("Please add fls files before setting NVM parameters");

  if (m_verbose >= VERBOSE_DEBUG)
    cout << "Setting up NVM tags (" << erase << ")..." << endl;

  int index = -1;
  bool all = (erase.find("all") != string::npos);

  if (all || erase.find("cal") != string::npos)
  {
    index = isInSet(available_nvm_tags,(uint32_t)NVM_TAG_ERASE_CAL_V2);
    if (index < 0)
      index = isInSet(available_nvm_tags,(uint32_t)NVM_TAG_ERASE_CAL);

    if (index < 0)
      throw_exception("Can not erase Calibration NVM area.");

    if (m_verbose >= VERBOSE_ACTIONS)
      cout << "Setting NVM tag Erase Calibration (" << index << ")" << endl;

    m_nvm_tags.insert(index);
  }

  if (all || erase.find("fix") != string::npos)
  {
    index = isInSet(available_nvm_tags,(uint32_t)NVM_TAG_ERASE_FIX_V2);
    if (index < 0)
      index = isInSet(available_nvm_tags,(uint32_t)NVM_TAG_ERASE_FIX);

    if (index < 0)
      throw_exception("Can not erase Fixed NVM area.");

    if (m_verbose >= VERBOSE_ACTIONS)
      cout << "Setting NVM tag Erase Fixed (" << index << ")" << endl;

    m_nvm_tags.insert(index);
  }

  if (all || erase.find("dyn") != string::npos)
  {
    index = isInSet(available_nvm_tags,(uint32_t)NVM_TAG_ERASE_DYN_V2);
    if (index < 0)
      index = isInSet(available_nvm_tags,(uint32_t)NVM_TAG_ERASE_DYN);

    if (index < 0)
      throw_exception("Can not erase Dynamic NVM area.");

    if (m_verbose >= VERBOSE_ACTIONS)
      cout << "Setting NVM tag Erase Dynamic (" << index << ")" << endl;

    m_nvm_tags.insert(index);
  }
}

/**
 * SetupTestmode()
 *
 * Setup the NVM test mode boot-up.
 *
 * The conditions for doing this is that the Fixed erase is either not available
 * or that the user defined it.
 *
 * The fls files have to be loaded before calling this function.
 * If the Fixed area is wanted to be erased you should call the SetupNvmOptions()
 * first.
 *
 */
void DownloadTool::SetupTestmode(uint32_t tags[])
{
  int index = -1;
  if (downloadlist.empty())
    throw_exception("Please add fls files before setting NVM parameters");

  if (m_verbose >= VERBOSE_OVERALL)
    cout << "Setting up Test mode..." << endl;

  if (m_verbose >= VERBOSE_DEBUG)
  {
    cout << "   Available NVM tags detected in fls: [ ";
    foreach(tag, available_nvm_tags)
      cout << *tag << ", ";
    cout << "\b\b" << " ]" << endl;
  }

  for (int i=0; tags[i]; ++i)
  {
    index = isInSet(available_nvm_tags,tags[i]);
    if (index >= 0)
      break;
  }

  if (index < 0)
    throw_exception("Can not setup NVM Test mode: Option not available.");

  //< Setting Erase Fixed if available
  int erase_fix = isInSet(available_nvm_tags,(uint32_t)NVM_TAG_ERASE_FIX_V2);
  if (erase_fix < 0)
    erase_fix = isInSet(available_nvm_tags,(uint32_t)NVM_TAG_ERASE_FIX);

  if (erase_fix > 0)
  {
    if (isInSet(m_nvm_tags,erase_fix) < 0)
    {
      if (m_verbose >= VERBOSE_ACTIONS)
        cout << "Forcing NVM tag Erase Fixed (" << erase_fix << ")" << endl;
      m_nvm_tags.insert( erase_fix );
    }
  }

  // Setting testboot mode
  if (m_verbose >= VERBOSE_ACTIONS)
    cout << "Setting NVM tag for Testmode (" << index << ")..." << endl;
  m_nvm_tags.insert(index);
}
#endif /* SIERRA */
/* SWISTOP */


void DownloadTool::SetBootTimeout(int timeout)
{
  if(timeout > 0 && timeout < 15000)
    cout << "Warning: Timeout is so low, the target won't be able to upload the EBL!" << endl;
  m_boot_timeout = timeout;
  SetParameter(IFWD_DL_dll_parameter_boot_process_timeout, timeout);
}
void DownloadTool::SetCommTimeout(int timeout)
{
  if (m_verbose == VERBOSE_DEBUG)
    cerr << "Setting Communication timeout to " << timeout << endl;

  //if(timeout < 15000)
  //  cout << "Warning: Timeout is so low, the target won't be able to upload the EBL!" << endl;
  m_comm_timeout = timeout;
  SetParameter(IFWD_DL_dll_parameter_comm_timeout, timeout);
}



void DownloadTool::SetBaudrate(int baudrate)
{
#ifndef WIN32
  switch(baudrate)
  {
  case   57600: case  115200: case  230400:
  case  460800: case  500000: case  576000:
  case  921600: case 1000000: case 1152000:
  case 1500000: case 2000000: case 2500000:
  case 3000000: case 3500000: case 4000000:
    break;
  default:
    throw_exception("Baudrate not supported!");
  }
#endif
  //cout << "Setting baudrate to " << baudrate << endl;
  m_baudrate = baudrate;

  if (comport_open)
    SetComPort(m_comport);
}

// ---------------- TRACE - Start --------------------------
void DownloadTool::SetTraceFile(std::string filepath)
{
  StopTrace();
  m_trace_file.clear();
  if (filepath.length() == 0) return;

  if (fexists(filepath))
    cout << "   Warning: Overwriting file " << filepath << endl;

  m_trace_file = filepath;
  StartTrace();
}

void DownloadTool::StartTrace()
{
  if (m_trace_file.length() > 0)
  {
    if (m_verbose >= VERBOSE_DLL)
      cout << "Enabling trace (" << m_trace_file << ")..." << endl;

    uint32_t tracefile = reinterpret_cast<uintptr_t>(m_trace_file.c_str());
    IFWDfunction( dl->set_parameter(IFWD_DL_dll_parameter_set_trace_filename, tracefile, ErrorText) );
    IFWDfunction( dl->set_parameter(IFWD_DL_dll_parameter_set_trace, m_verbose, ErrorText) );
    m_tracing = true;
  }
}

void DownloadTool::StopTrace()
{
  if(m_tracing)
  {
    IFWDfunction( dl->set_parameter(IFWD_DL_dll_parameter_set_trace, false, ErrorText) );
    m_tracing = false;

    if (m_verbose >= VERBOSE_DLL)
      cout << "Trace saved in file: " << m_trace_file << endl;
  }
}
// ---------------- TRACE - End --------------------------


//------------------------------------------------------------------------------
void DownloadTool::SetChannel(int channel)
{
  m_channel = channel;
}

bool DownloadTool::SetComPort(string Port, string DlPort)
{
  if (m_verbose >= VERBOSE_DEBUG)
    cout << "SetComPort(" << Port << ", " << DlPort << ")" << endl;

  if (Port.length() == 0) return true;
  string tmp = ComPortName( Port );

  DlPort = DlPort.empty() ? tmp : ComPortName( DlPort );

  if (tmp.find(PHONE_PRE_STRING) != string::npos)
    DeviceExist(tmp);

  if (comport_open)
    IFWDfunction( dl->close_comm_port(m_channel, ErrorText) );

  m_comport.clear();

  // Finding the channel and opening the port
  if (isdigit(Port[0]))
  {
    if (Port.size() > 1 && Port[1] == '-')
      m_channel = 1;                    //< syspath provided
    else
      m_channel = StringTo<int>(Port);  //< GenericUSB channel
  }
  else if (Port[0] == '{' || Port[0] == '"')
  {
    const char* guid_str = Port.c_str();
    if (dl->set_parameter(IFWD_DL_dll_parameter_set_winusb_guid,  (uintptr_t) guid_str, ErrorText) != IFWD_DL_OK)
      throw_exception("Could not set alternative GUID: ", false);
  }
  else if (dl->get_next_free_channel)
    m_channel = dl->get_next_free_channel();
  else
    m_channel = 1;
  SetChannel(m_channel);

  if (!dl->open_comm_port)
    throw_exception("Could not find IFWD_DL_open_comm_port()!", false);

  char* comport = (char*)tmp.c_str();
  char* dlport = (char*)DlPort.c_str();

  if (m_verbose >= VERBOSE_ACTIONS)
  {
    cout << "Setting Comport " << comport;

    if (strcmp(comport,dlport) != 0)
      cout << ", Ctrl: " << dlport;

    if (tmp.find("ttyUSB") != string::npos)
      cout << ", Baudrate: " << m_baudrate;

    cout << " (Channel " << m_channel << ")" << endl;
  }

  if (!IFWDfunction( dl->open_comm_port(m_channel, comport, dlport, m_baudrate, ErrorText) ))
    throw_exception("Can not open Comport "+ tmp, false);

  comport_open = true;
  m_comport = tmp;
  return true;
}

/**
 * String formats:
 *    -r 1                  - Relay number 1 on device /dev/ttyACM0
 *    -p /dev/ttyACM0,1     - Power relay 1 on device /dev/ttyACM0
 *    -r /dev/ttyACM0,1     - Reset relay 1 on device /dev/ttyACM0
 *    -p 8                  - Sets the power switch to be relay number 8
 */
bool DownloadTool::SetRelayPort(string Port)
{
  m_relay.clear();
  if (Port.length() == 0) return true;

  string tmp = ComPortName( Port );
  try {
    DeviceExist(tmp);
  } catch (logic_error& e) {
    #ifdef __EXCEPTIONS
      throw_exception("Can not setup Relay port: "+ string(e.what()), false);
    #endif
  }

  if (m_verbose >= VERBOSE_DLL)
    cout << "Setting Relay to " << tmp << endl;

  m_relay = tmp;
  relay = new k8090( m_relay );
  if(relay->errorStatus())
    throw_exception("Error opening relay", false);
    //return false;

  relay->setVerbose(false);
  return true;
}

bool DownloadTool::SetResetRelay(string button)
{
  reset_button = 0;

  size_t pos = button.find(',');
  if (pos != string::npos)
  {
    reset_button = atoi( button.substr(pos+1).c_str() );
    SetRelayPort( button.substr(0, pos) );
  } else {
    if (atoi(button.c_str()) != 0)
    {
      reset_button = atoi(button.c_str());
      SetRelayPort( RelayPortName() );
    }
    else
      SetRelayPort( button );
  }
  return true;
}

bool DownloadTool::SetPowerRelay(string button)
{
  power_button = 0;

  size_t pos = button.find(',');
  if (pos != string::npos)
  {
    power_button = atoi( button.substr(pos+1).c_str() );
    SetRelayPort( button.substr(0, pos) );
  } else {
    if (atoi(button.c_str()) != 0)
    {
      power_button = atoi(button.c_str());
      SetRelayPort( RelayPortName() );
    }
    else
      SetRelayPort( button );
  }
  return true;
}

/**
 * Rebooting the phone and attempts to connect to the AT console
 *
 * TODO: Maybe we need to create the "relay device"
 */
bool DownloadTool::startAT(bool reboot, int tries)
{
  int turn;
  if (reboot) // TODO: && technology != "FLASHLESS")
  {
    if (m_verbose >= VERBOSE_ACTIONS)
      cout << "Forcing target HW reset..." << endl;
    PowerTarget();
  }
  else
  {
    if (!m_relay.empty())
      relay->setOn( power_button );

    if (comport_open)
      ForceTargetReset();
  }

  // TODO: Rename to something like "CloseComm"
  CloseComm();

  // TODO: Use --boot-timeout
  for (turn=1; turn <= tries; turn++)
  {
    //cout << "Turn " << turn << " of " << tries << endl;

    #ifndef _WIN32
      if (isdigit(m_comport[0]) && m_comport[1] == '-')
      {
        char* tty = SysFs_get_device_tty(m_comport.c_str());
        if (!tty)
        {
          cout << "Waiting for phone to come online (" << m_comport << ").." << endl;
          Sleep(1000);

          for (int i=0; !tty && i < 30; i++)
          {
            Sleep(1000);
            tty = SysFs_get_device_tty(m_comport.c_str());
          }

          if (!tty)
            return false;
        }

        m_comport = "/dev/"+ string(tty);
        // cout << "Comport set to " << m_comport << endl;
      }

      if (!fexists(m_comport))
      {
        cout << "Waiting for phone to come online (" << m_comport << ")..." << endl;
        Sleep(1000);
        for (int i=0; !fexists(m_comport) && i < 30; i++)
          Sleep(1000);

        if (!fexists(m_comport))
          return false;
      }
    #endif

    if (m_verbose >= VERBOSE_DLL)
      cout << "New phone (" << m_comport << "," << fexists(m_comport) << ")..." << endl;

    /**
     *  3 KlocWork issues here.. Can't seem to figure it out
     **/
/* SWISTART */
#ifndef SIERRA
    phone = new Phone(m_comport);
#else
    phone = new Phone(m_comport,m_baudrate);
#endif /* SIERRA */
/* SWISTOP */
    phone->setVerbose(m_verbose-1);

    if (phone->connect(15*turn))
    {
      phone->setVerbose(m_verbose);
      return true;
    }

    delete phone;

    // Reset once and try again
    if (reboot)
    {
      if (m_verbose >= VERBOSE_DLL)
        cout << endl << "Forcing target HW reset..." << endl;
      PowerTarget();
    }
  }
  return false;
}

/**
 * Send AT command the the Phone
 */
string DownloadTool::sendAT(string cmd, bool blocking)
{
  if (!phone)
  {
    if (m_verbose >= VERBOSE_ACTIONS)
      cout << " Connecting to phone..." << endl;

    if (!startAT())
      throw_exception("Could not connect to phone", "");
  }

  if (m_verbose >= VERBOSE_ACTIONS)
    cout << "Sending: " << endl << cmd << endl;
  phone->sendAT(cmd);

  string recv = phone->recvAT(blocking);
  if (m_verbose >= VERBOSE_ACTIONS)
    cout << "Response: " << endl << recv << endl;

  return recv;
}

void DownloadTool::stopAT()
{
  delete phone;
}

/* SWISTART */
#ifndef SIERRA_CTF
/**
 *  Secure channel stuff
 *  No support for setting up call-back here!
 */
void DownloadTool::setup_secure_channel(string path, uint32_t options)
{
  char **dlist;
  string temp;

  if (!FileSystem_isdir(path))
    throw_exception("Certificate path ["+path+"] is not a folder");

  temp = fcomplete(path);
  dlist = FileSystem_dirlist((const char*) temp.c_str(), true); // true to have the function return a list of files with full path.
  if (! dlist)
  {
    FileSystem_free_string_array(dlist);
    throw_exception("Certificate path ["+path+"] is empty");
  }

  if (dl->secure_channel_setup(m_channel, options, NULL, dlist, ErrorText) != IFWD_DL_OK)
  {
    FileSystem_free_string_array(dlist);
    throw_exception("Secure channel setup failed");
  }

  FileSystem_free_string_array(dlist);
}
#endif/* SIERRA_CTF */
/* SWISTOP */

/**
 * Wrapper functions for the Download library
 *
 */
void DownloadTool::Init()
{
  if (!comport_open)
  {
    if (m_comport.empty())
      SetComPort( ComPortName() );

    // OpenComPort();
    char* comport = (char*)m_comport.c_str();
    if (!IFWDfunction( dl->open_comm_port(m_channel, comport, comport, m_baudrate, ErrorText) ))
      throw_exception("Can not open Comport "+ m_comport);
    comport_open = true;

    if (!comport_open)
      throw_exception("Comport "+ m_comport +"is not opened!");
  }
}


bool DownloadTool::WaitForNextState(uint8_t channel, int timeout)
{
  channel_state_t state = channel_state[channel];
  while (state == channel_state[channel])
  {
    if (!timeout--) break;
    Sleep(100);
  }
  cerr << "   TIMEOUT!" << endl;
  return false;
}




/**
 * Adding new files to the download list
 *
 * @return A smart pointer to the newly allocated Fls File
 */
FlsFile* DownloadTool::AddFile(string file)
{
  FlsFile* fls_file = NULL;
  vector<uint32_t> nvm_tags;
  unsigned int count = 0;

  try
  {
    fls_file = new FlsFile(file, m_verbose-2);
    count = fls_file->download_files.size();

    cout << "  " << setw(32) << left << fname(file) << " (";

    for (size_t i=0; i < count; ++i)
    {
      stringstream toc_index;
      toc_index << "|" << i << "|" << fls_file->path();
      //cout << "  Tag " << i << ": " << tag << endl;
      DownloadFile* dl_file = fls_file->download_files[i];
      //fls->nvm_tags.insert(tag);
      dl_file->toc_path = toc_index.str();

      for (size_t j=0; j < dl_file->load_map.size(); ++j)
      {
        MemoryRegion* region = dl_file->load_map[j];

        //< Note: FLASHLESS regions are automatically included in the download
        //< TODO: ...or is it because |1| means ALL Download Regions?
        if (region->size() == 0 || region->area->flags.has(FLASHLESS) >= 0)
          continue;

        cout << dl_file->type.name;
        if (i < (count-1)) cout << ", ";
      }

      //< Remembering available NVM tags
      if (dl_file->type.id == MEMORY_CLASS_CODE)
        nvm_tags = fls_file->get_nvm_tags(dl_file);

    }

    switch (fls_file->platform.technology)
    {
      case TECHNOLOGY_NOR:         cout << ") (NOR)" << endl;        break;
      case TECHNOLOGY_NAND:        cout << ") (NAND)" << endl;       break;
      case TECHNOLOGY_FLASHLESS:   cout << ") (Flashless)" << endl;  break;
      default:                         cout << ")" << endl;              break;
    }

    //< Appending NVM tags from CODE sections
    for(size_t i=0; i < nvm_tags.size(); ++i)
    {
      //cout << " NVM Tag[" << i << "] " << nvm_tags[i] << endl;
      available_nvm_tags.insert(nvm_tags[i]);
    }

    downloadlist.push_back( fls_file );
  }
  catch (exception& e)
  {
    #ifndef __EXCEPTIONS
      myexception& e = chatched_exceptions.front();
    #endif
    string error = e.what();
    if (error.find("not FLS") == string::npos)
      throw_exception(e.what(), NULL);

    if (!FileSystem_isfile(file))
      throw_exception("Path "+ file +" is not a regular file", NULL);

    cout << "  " << setw(32) << left << fname(file) << " (Binary) (NOR)" << endl;
  }

  return fls_file;
}

/**
 * Returns a stream to a post_download.sh / terminal.script
 */
vector<string> DownloadTool::GetPostScript()
{
  vector<string> commands;
  for (size_t i=0; i < downloadlist.size(); ++i)
  {
    FlsFile* fls = downloadlist[i];
    for (size_t j=0; j < fls->meta_files.size(); ++j)
    {
      FlsEmbedFile* file =fls->meta_files[j];
      string ext = fext(file->name());
      if (ext.compare("zip") == 0)
      {
        ZIP zip(fls->path(), file->offset(), file->size());
        ZipFile* zf = zip["post_boot.script"];
        if (zf)
        {
          char* ptr, *str_ptr;
          vector<uint8_t> buffer;

          zf->get_content(buffer);
          buffer.push_back('\0');

          ptr = str_ptr = (char*)&buffer[0];
          do {
            if (*ptr == '\n')
            {
              *ptr = '\0';
              commands.push_back( str_ptr );
              str_ptr = ptr+1;
            }
            ptr++;
          } while (*ptr);
        } /* found post boot script */
      } /* found ZIP */
    } /* foreach meta_files */
  } /* foreach FLS files */
  return commands;
}



/**
 * Power Target
 *  - Using the k8090 relay to power up or reset the target.
 *  - If no relay switch is available it requests
 *    the user to flip the switch.
 */
void DownloadTool::PowerTarget()
{
  // Reboot device if the power button is set
  if (power_button > 0)
  {
    if (m_verbose >= VERBOSE_DLL)
      cout << "  Rebooting switch " << power_button << endl;

    relay->reboot( power_button );
  }

  // Reset the device if the reset button is set
  if (reset_button > 0)
  {
    Sleep(BOOT_TIME/3);
    if (m_verbose >= VERBOSE_DLL)
      cout << "  Resetting switch " << reset_button << endl;

    relay->reset(reset_button);
  }

  Sleep(BOOT_TIME/3);
}

/**
 * Power Target & Synchronise
 *  - Using the k8090 relay to power up or reset the target.
 *  - If no relay switch is available it requests
 *    the user to flip the switch.
 */
void DownloadTool::PowerTargetWaitSync(int tries)
{
  synchronized = false;

  for(int loops=1; loops<tries && !synchronized; ++loops)
  {
    // Reboot device if the power button is set
    if (power_button > 0)
    {
      if (m_verbose >= VERBOSE_DLL)
        cout << "  Rebooting relay switch " << power_button << endl;
      relay->reboot( power_button, BOOT_TIME*loops );

      for(int extra=15; extra && !synchronized; --extra)
        Sleep(BOOT_TIME/15);
    }

    // Reset the device if the reset button is set
    if (reset_button > 0)
    {
      if (m_verbose >= VERBOSE_DLL)
        cout << "  Resetting relay switch " << reset_button << endl;
      relay->reset(reset_button);

      for(int extra=50; extra && !synchronized; --extra)
        Sleep(loops*BOOT_TIME/15);
    }
  }
}

/**
 * Sorting the download_files list in a FlsFile (Bubble sort)
 *
 * @param fls   File to sort
 */
static void bubble_sort_fls_file(FlsFile* fls)
{
  bool done = false;
  while (!done)
  {
    done = true;
    for (size_t i=0; i < fls->download_files.size()-1; ++i)
    {
      DownloadFile* file1 = fls->download_files[i];
      DownloadFile* file2 = fls->download_files[i+1];
      if (file2->type.weight < file1->type.weight)
      {
        fls->download_files[i]   = file2;
        fls->download_files[i+1] = file1;
        done = false;
      }
    }
  }
}

/**
 * Sorting a list of DownloadFiles by MemoryClass
 */
static bool sort_fls(FlsFile* f1, FlsFile* f2)
{
  return (f1->download_files[0]->type.weight < f2->download_files[0]->type.weight);
}

/**
 * Booting Target
 *
 * - Starts a "power up" thread, and then calls the DownloadLib
 * - Can boot binary, EBL and PSI files
 */
bool DownloadTool::BootTarget(string file)
{
  ThreadType relay_thread;

  Init();
  if (file.empty() && downloadlist.empty())
    throw_exception("No files for download!", false);

  //< Sort by memory class, so PSI is downloaded first
  //< Note: Priorities between FlsFiles are NOT taken into accuont!
  for (size_t i=0; i < downloadlist.size(); ++i)
    bubble_sort_fls_file(downloadlist[i]);

  downloadlist.sort(sort_fls);

#if 0
  cout << endl << " Download List (ordered):" << endl;
  foreach(it, downloadlist)
  {
    FlsFile* fls_file = *it;
    cout << " " << fls_file->path() << endl;
    for (size_t k=0; k < fls_file->download_files.size(); ++k)
    {
      DownloadFile* file = fls_file->download_files[k];
      cout << "   " << k << ". " << file->name << " " 
                    << file->type.name << " (" << file->type.weight << ")" << endl;
    }
    cout << endl;
  }
#endif

  //downloadlist.sort(FlsDownloadFile::sort);
  FlsFile* boot_file = file.empty() ? downloadlist.front() : AddFile(file);

  if (m_verbose >= VERBOSE_OVERALL)
    cout << "Booting '" << boot_file->path() << "', memclass: " << boot_file->type().name << endl;

  THREAD_create( &relay_thread, &DownloadTool::PowerTargetSync, this);

  if (m_relay.empty() and !extern_relay)
    cout << "  -> Please reboot your phone device <-  " << endl;
  else
  if (!extern_relay)
    Sleep(REBOOT_TIME);

  if (boot_file->type().id == MEMORY_CLASS_BINARY
   || boot_file->type().id == MEMORY_CLASS_EBL)
  {
    if (m_verbose >= VERBOSE_DEBUG)
      cout << "boot_bin_target(" << m_channel << ", " << boot_file->path() << ")" << endl;
    IFWDfunction( dl->boot_bin_target(m_channel, (uint8_t*)boot_file->path().c_str(), &m_OnBootCtrl, ErrorText) );
  }
  else
  {
    if (m_verbose >= VERBOSE_DEBUG)
      cout << "boot_target(" << m_channel << ", " << boot_file->path() << ")" << endl;

    IFWDfunction( dl->boot_target(m_channel, (char*)boot_file->path().c_str(), &m_OnBootCtrl, ErrorText) );
  }

  THREAD_join(relay_thread);
  /*
  if (!relay_thread.joinable())
    relay_thread.interrupt();
  relay_thread.join();
  */
  //WaitForNextState(m_channel);

  if (!synchronized)
    throw_exception("Could not synchronize with target!", false);
  return synchronized;
}


/* SWISTART */
#ifndef SIERRA_NOERASE
/**
 * Erase
 */
void DownloadTool::Erase(uint32_t start, uint32_t length)
{
  if (!synchronized)
    throw_exception("Cannot erase anything... Target is not booted yet!");

  if (m_verbose >= VERBOSE_OVERALL)
    cout << "Erasing from address: 0x" << hex << start
         << ", length: 0x" << hex << length << "..." << endl;

  // Enabling erase first, then we can skip the empty blocks
  IFWDfunction( dl->set_parameter(IFWD_DL_dll_parameter_use_pre_erase, true, ErrorText) );
  IFWDfunction( dl->set_parameter(IFWD_DL_dll_parameter_skip_empty_blocks, true, ErrorText) );

  IFWDfunction( dl->set_parameter(IFWD_DL_dll_parameter_erase_mode, IFWD_DL_target_erase_all, ErrorText) );
  // IFWD_DL_erase_image(m_channel, (uint32_t)start_addr, (uint32_t)len, NULL, Err);
  //IFWDfunction( dl->erase_image(m_channel, start, length, (char*)bin_file.c_str(), ErrorText) );
  IFWDfunction( dl->erase_image(m_channel, start, length, 0, ErrorText) );
}

#endif /* SIERRA_NOERASE */
/* SWISTOP */

/* SWISTART */
#ifndef SIERRA
/**
 * Upload
 */
void DownloadTool::UploadFile(uint32_t start, uint32_t length, const string& bin_file)
{
  if (!synchronized)
    throw_exception("Cannot upload image... Target is not booted yet!");

  string fullpath = fcomplete(bin_file);
  if (fullpath.empty())
    throw_exception("Cannot find upload image!");

  if (downloadlist.empty())
    throw_exception("No boot reference file (Fls)!");

  for (size_t i=0; i < downloadlist.size(); ++i)
    bubble_sort_fls_file(downloadlist[i]);
  downloadlist.sort(sort_fls);

  string boot_file = fcomplete(downloadlist[0]->path());
  if (boot_file.empty())
    throw_exception("No boot reference file (Fls)!");

  if (m_verbose >= VERBOSE_OVERALL)
    cout << "Uploading to '" << fullpath
           << "', starting from address: 0x" << hex << start
           << ", length: 0x" << hex << length
           << ", BootFile: " << boot_file << endl;

  //SetBaudrate(3000000);
  //IFWDfunction( dl->set_parameter(IFWD_DL_dll_parameter_upload_mbn_data, false, ErrorText) );
  // INFO: Set to false when using NOR
//IFWDfunction( dl->set_parameter(IFWD_DL_dll_parameter_set_nand_bitfield, MAC_INCLUDING, ErrorText) );
  IFWDfunction( dl->upload_bin_image(m_channel, start, length, 
                (char*)fullpath.c_str(), (char*)boot_file.c_str(), ErrorText) );
}
#endif /* SIERRA */
/* SWISTOP */

void DownloadTool::DownloadFiles(vector<string> files)
{
  foreach(file, files)
    AddFile(*file);

  DownloadFiles();
}

/* SWISTART */
#ifndef SIERRA
/**
 * Special function for downloading a binary file...
 * DEPRECATED: But only if normal download works
 */
void DownloadTool::DownloadBinary(uint32_t start, uint32_t length, const string& bin_file)
{
  if (!synchronized)
    BootTarget( bin_file );

  //if (downloadlist.empty())
  //  throw_exception("No files for download!");

  //BootTarget( boot_file );

  if (m_verbose >= VERBOSE_OVERALL)
    cout << "Downloading binary '" << bin_file
           << "', starting from address: 0x" << hex << start
           << ", length: 0x" << hex << length << "..." << endl;

  SetParameter(IFWD_DL_dll_parameter_upload_mbn_data, false);
  SetParameter(IFWD_DL_dll_parameter_force_plain_upload, false);
  SetParameter(IFWD_DL_dll_parameter_faster_crc_method, true);
  SetParameter(IFWD_DL_dll_parameter_skip_empty_blocks, false); // INFO: Changed to true!
  SetParameter(IFWD_DL_dll_parameter_erase_mode, false);
  SetParameter(IFWD_DL_dll_parameter_set_usb_auto_mode, true);
  SetParameter(IFWD_DL_dll_parameter_use_pre_erase, true);
  SetParameter(IFWD_DL_dll_parameter_force_area_erase, false);
  SetParameter(IFWD_DL_dll_parameter_do_not_allow_EEP_mode_write, true);

  IFWD_DL_nand_control_bitfield_type nand;
  nand.Specification = MAC_PRESERVING;
  memset(nand.PartitionBit, '\0', sizeof(nand.PartitionBit));
  SetParameter(IFWD_DL_dll_parameter_set_nand_bitfield, intptr_t(&nand));

  SetParameter(IFWD_DL_dll_parameter_cond_dyn_eep_erase, false);
  SetParameter(IFWD_DL_dll_parameter_skip_wr_prak_crc, false);
  SetParameter(IFWD_DL_dll_parameter_check_sign_hw_cfg_value, true); //< Only valid for EGoldVoice in secure mode.
  //SetParameter(IFWD_DL_dll_parameter_use_alt_boot_speed, false);   //< if true this forces bootspeed of 460800

  //IFWDfunction( dl->download_bin_image(m_channel, start, length, 0, (char*)bin_file.c_str(), (char*)boot_file.c_str(), ErrorText) );
  IFWDfunction( dl->download_bin_image(m_channel, start, length, 0, (char*)bin_file.c_str(), 0, ErrorText) );
}
#endif /* SIERRA */
/* SWISTOP */

void DownloadTool::DownloadFiles() // throws exception
{
  if (downloadlist.empty())
    throw_exception("No files for download!");

  if (!synchronized)
    BootTarget();

  /*
   * TODO: Remove, because the sorting algorithm should handle this "exception"
   * NOTE: I think this is leftovers from old times, because I've never encountered
   *       that we enter this list.
   */
#if 0
  uint32_t look_for_mem_class = 0;
  if(dl->get_parameter(IFWD_DL_dll_get_parameter_ebl_info_expected_mem_class, &look_for_mem_class, (char *)(&m_channel)) == IFWD_DL_OK)
  {
    if(look_for_mem_class != 0) // something special expected first?
    {
      cout << "Looking for memory class: " << fls::get_memory_class((fls::MemoryClassId)look_for_mem_class).name << endl;

      foreach(it, downloadlist)
      {
        FlsDownloadFile* f = *it;
        if(f->memclass.id == look_for_mem_class)
        {
          downloadlist.remove(f);
          downloadlist.push_front(f);
          break;
        }
      }
    }
  }
#endif
  //====== 3. DOWNLOAD ==============================================================

  //WaitForNextState(m_channel);
  cout << "Downloading files (" << downloadlist.size() << ")..." << endl;

  foreach(it, downloadlist)
  {
    FlsFile* fls_file = *it;

    for (size_t k=0; k < fls_file->download_files.size(); ++k)
    {
      DownloadFile* file = fls_file->download_files[k];
      char* TOC = (char*)file->toc_path.c_str();

      // Saving information, to later calculate bandwidth
      sample_count = sample_total = 0;
      last_ts = last_bytes = 0;
      total = file->size();

      //WaitForNextState(m_channel);
      Log("Downloading %s...\n", file->type.name.c_str());
      //Log("   MemoryClass: %s... \n Path: %s\n Toc: %s\n", file->type.name.c_str(), file->name.c_str(), TOC);

      //TODO: file->memclass.downloadFunction(1, p_file, ErrorText);
      switch(file->type.id)
      {
        case MEMORY_CLASS_PSI:        //Log("Downloading PSI...");
        case MEMORY_CLASS_SLB:        //Log("Downloading SLB...");
        case MEMORY_CLASS_EBL:
          LastStatus = dl->download_fls_file(m_channel, TOC, ErrorText);
          break;

        case MEMORY_CLASS_CODE:                 //Log("Downloading FLS...");

          // TODO: Call this directly "fls_file.get_nvm_tags();"
          foreach(tag, m_nvm_tags)
            IFWDfunction( dl->NVM_set_user_option((char*)file->name.c_str(), (uint32_t)*tag, true) );

          LastStatus = dl->download_fls_file(m_channel, TOC, ErrorText);
          break;

        case MEMORY_CLASS_DYN_EEP:             //Log("Downloading EEPROM...");
          if(m_test_mode)
            LastStatus = dl->download_eep_file(m_channel, TOC, IFWD_DL_target_boot_mode_test, ErrorText);
          else
            LastStatus = dl->download_eep_file(m_channel, TOC, IFWD_DL_target_boot_mode_normal, ErrorText);
//        Status = IFWD_download_eep_file(m_channel, p_file, IFWD_DL_target_boot_mode_test, ErrorText);
//        Status = IFWD_update_static_eeprom(m_channel, p->args->Strings[2].c_str() , p->args->Strings[3].c_str() , p->filename.c_str(), ErrorText);
          break;

        case MEMORY_CLASS_STAT_FFS:             //Log("Downloading DFFS...");
          LastStatus = dl->download_dffs_file(m_channel, TOC, IFWD_DL_dffs_load_sel_static, ErrorText);
          break;

        case MEMORY_CLASS_DYN_FFS:              //Log("Downloading DFFS...");
          LastStatus = dl->download_dffs_file(m_channel, TOC, IFWD_DL_dffs_load_sel_dynamic, ErrorText);
          break;

        case MEMORY_CLASS_CUST:                 //Log("Downloading Cust: ", Fls File*);
        case MEMORY_CLASS_DSP_SW:               //Log("Downloading DSP...");
          LastStatus = dl->download_cust_file(m_channel, TOC, ErrorText);
          break;

        case MEMORY_CLASS_ROOT_DISK:
        case MEMORY_CLASS_CUST_DISK:
        case MEMORY_CLASS_USER_DISK:
          //Log("Downloading DFAT...");
          LastStatus = dl->download_dffs_file(m_channel, TOC, IFWD_DL_dffs_load_sel_nand_disk, ErrorText);
          break;

      //  case DYN_EEP_CLASS:
      //  case SWAP_BLOCK_CLASS:
      //  case SEC_BLOCK_CLASS:
      //  case EXCEP_LOG_CLASS:
      //  case HEX_EXTRACT_CLASS:
      //  case MEMORY_CLASS_BINARY:

        default:
          throw_exception("Did not recognize the download file type...");
      } // switch()

      if (!IFWDfunction( LastStatus ))
      {
        if (m_tracing) StopTrace();

        throw_exception("Error while Downloading");
      }
      Sleep(10); // Give the Download Library some time to think :)
      //WaitForNextState(m_channel, 1);
    }
  } // foreach(file in downloadlist)

  Log("\nSuccess!\n");
}

//---------------------------- THE END --------------------------------------------------
