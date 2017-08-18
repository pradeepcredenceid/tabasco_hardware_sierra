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
       $File name:  /msw_tools/FlashTool/DownloadTool/src/IMCDownload.h $
       $CC-Version: .../dhkristx_140314_v0/1 $
       $Date:       2014-05-02    13:00:35 UTC $
    ------------------------------------------------------------------------- */

#ifndef DOWNLOAD_TOOL_H_
#define DOWNLOAD_TOOL_H_

//#include "DownloadDll.h"

#include "k8090.h"    //< USB Relay device controller (verbose at level 2)
#include "Phone.h"    //< Phone "AT" mode (verbose at level 2)

#include "FileIO.h"
#include "Exceptions.h"
#include "LibraryLoader.h"
//typedef auto_ptr<LibraryLoader> LibraryLoaderPtr;

#include "FlsFile.h"
#include "Thread.h"
#include "SmartContainer.h"
#include "MemberCallback.h"

#include <string.h>

#include <memory>
#include <iostream>
#include <set>
#include <map>

using namespace std;
using namespace ipa;

#define VERBOSE_NONE      0
#define VERBOSE_OVERALL   1
#define VERBOSE_ACTIONS   2
#define VERBOSE_DLL       3
#define VERBOSE_DEBUG     4

// FlashLess support - Windows only!
// TODO: Linux support!
#define FLASHLESS_STATIC_ERASE    0x01
#define FLASHLESS_DYNAMIC_ERASE   0x02
#define FLASHLESS_CALIB_ERASE     0x04


// Each channel might be in these states
typedef enum {
  CHANNEL_STATE_IDLE,
  CHANNEL_STATE_SYNCED,
  CHANNEL_STATE_BOOTED,
  CHANNEL_STATE_DOWNLOADING,
  CHANNEL_STATE_TESTING_BOOT,
  CHANNEL_STATE_DONE,
  CHANNEL_STATE_FAILED
} channel_state_t;

#define DlCallback(X) { if(m_dl_callback) m_dl_callback(X, m_dl_userdata); }
#define LogCallback(X) { if(m_log_callback) m_log_callback(X, m_log_userdata); }

/*---------------------------------------------*/
/* DownloadTool Class  (Singleton)             */
/*---------------------------------------------*/

class DownloadTool
{
private:
  static DownloadTool* instance;
  DownloadTool(std::string library);

  string RelayPortName(string port = "?");

public: // == Interface ==
  static DownloadTool& getInstance(std::string library = DEFAULT_LIBRARY);
  ~DownloadTool();

  // Settings
  char* DeviceMap(int id);
  bool LoadDeviceMap(string map);
  bool SaveDeviceMap(string map);

  void loadConfigIni(string iniFile);
  void saveConfigIni(string iniFile);

  /**
   *  Wrapper functions for the Download Library
   *
   **/
  FlsFile* AddFile(string file);

  bool BootTarget(string file = "");

  void Erase(uint32_t start, uint32_t length);
  void UploadFile(uint32_t start, uint32_t length, const string& bin_file); // Upload meaning: Target -> PC
  void DownloadFiles();                                           // Download meaning: PC -> Target
  void DownloadFiles(vector<string> files);                       // Adds the list of files, and calls above function
  void DownloadBinary(uint32_t start, uint32_t length, const string& bin_file);// Simple Download of a single file
  //void AbortDownloading(void);

  vector<string> GetPostScript();

  /**
   *  Wrapper function for the Phone Class
   *
   **/
  bool startAT(bool reboot = false, int tries = 10);
  void stopAT();
  string sendAT(string cmd, bool blocking = true);

  void setup_secure_channel(string path, uint32_t options = 0);

  /**
   * Low level Download Library functions
   *
   */
  string GetVersion();
  string GetSystemVersion(const string& file);

  //float GetLibraryVersion();

  void booted(std::string resp);

  //uint32_t* GetParameter(uint8_t);
  //bool SetParameter(uint8_t, uint32_t);

  /**
   * Setting up...
   *
   */
  // This is the member function - I cannot get it to work, so I'm using a singleton instead.
  void CloseComm();
  void SetLibrary(std::string& library);
  void SetTraceFile(std::string path);
  void StartTrace();
  void StopTrace();

  bool SetRelayPort(std::string RelayPort);
  bool SetResetRelay(std::string button);
  bool SetPowerRelay(std::string button);

  void SetChannel(int channel);
  bool SetComPort(std::string Port, std::string DlPort = "");

  inline bool is_open() { return comport_open; };
  //void SetUSBPort(int USBPort);

  /* Setting download parameters */
  void SetParameter(IFWD_DL_dll_parameter_enum param, int value);
  int GetParameter(IFWD_DL_dll_parameter_enum param);

  // Commit is required upon Write & Erase
  bool OutOfSessionErase(IFWD_DL_oosc_type_enum type);
  bool OutOfSessionWrite(IFWD_DL_oosc_type_enum type, string file);
  bool OutOfSessionRead(IFWD_DL_oosc_type_enum type, string file);
  void OutOfSessionCommit();

  string OutOfSessionGetHwDetails(string hwid = "");


  void SetupFlashlessErase(std::string& path, std::string erase);
  void SetupProlificReset(std::string& prolific_port, vector<int> gpio);
  void ForceTargetReset();

  // NVM erase & testmode
  void SetupNvmErase(string erase);
  void SetupTestmode(uint32_t tags[]);

  void SetBootTimeout(int timeout);
  void SetCommTimeout(int timeout);
  void SetBaudrate(int baudrate);
  void SetVerbose(int verbose, std::string module_debug);

  //void SetUserData(void *UserData); // use this to set e.g. a "this" pointer to be returned in the callbacks
  //void SetEEPUpdate(char *u_file, char *r_file, char *prefix);


  /**
   *  Callbacks from the library have been converted to signals
   *  in order to make them minimum time consuming.
   *
   *  There is the global signal_library_callback, which includes
   *  an integer defining which kind of status.
   *
   *  You can also connect to a signal for each kind of status.
   *
   **/
  void OkCallBack(int channel);
  void LibraryCallBack(int channel, int state, const string& text);

  Signal<void, int, int, const string&> signal_library_callback;

  //////////////////////////////
  // TODO: Signal Wrapper     //
  //////////////////////////////

  Signal2<void, int, int>   signal_progress_update;
  Signal1<void, std::string> signal_outline_update;
  Signal1<void, int>         signal_status;
  Signal1<void, std::string> signal_error;
  Signal1<void, std::string> signal_AT_command;

public:
  // TODO: Make a wrapper to this class, enabling the channels
  std::map<uint8_t,channel_state_t> channel_state;
  //std::pair<uint8_t,std::list<FilePtr> > downloadlist;

  // Config settings
  int m_channel;
  std::string m_comport;
  std::string m_relay;
  std::string m_trace_file;
  bool m_tracing;
  int m_baudrate;
  int m_boot_timeout;
  int m_comm_timeout;
  bool m_test_mode;
  int m_verbose;

  // All errors and status from DownloadDLL is stored in these variables
  bool synchronized;
  bool device_booted;
  IFWD_DL_status_enum LastStatus;

private:
  IFWD_DL_modem_control_signals_type  m_OnBootCtrl;

  SmartList< FlsFile* > downloadlist;

  set<uint32_t> m_nvm_tags;          //< User defined NVM tags (all, calib,fixed,dynamic)
  set<uint32_t> available_nvm_tags;  //< Cababilities from the fls file

  Phone* phone;
  k8090* relay;

  bool comport_open;
  int power_button;
  int reset_button;
  bool extern_relay;

  char ErrorText[500];

protected:
  /**
   *  These two functions should be called before & after
   * any operations with ComPorts
   **/
  void Init();
  void End();

  bool IFWDfunction(IFWD_DL_status_enum Status);
  void PowerTarget();
  void PowerTargetWaitSync(int tries = 3);
  bool WaitForNextState(uint8_t channel, int timeout = 1000);


  static void *PowerTargetSync(void* obj)
  {
    static_cast<DownloadTool*>(obj)->PowerTargetWaitSync();
    return (void*)0;
  }

  // Download Library functions....
  LibraryLoader* dl;
  float lib_version;
};

#endif /* IFXDOWNLOAD_H_ */
