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
       $File name:  /msw_tools/FlashTool/DownloadTool/src/main.cpp $
       $CC-Version: .../dhkristx_140314_v0/5 $
       $Date:       2014-05-02    13:00:42 UTC $
    ------------------------------------------------------------------------- */

#include "version.h"
#include "targetver.h"

#include "PrgOptions.h"
#include "StringExt.h"

#include <cctype>
#include <cstdarg>
#include <csignal>

#include <fstream>
#include <iostream>
#include <sstream>

#include "Utils.h"
#include "Watchdog.h"
#include "IMCDownload.h"
#include "Exceptions.h"

#ifndef WIN32
  #include <errno.h>
  #include <fcntl.h>
  #include <sys/wait.h>

  #ifndef ANDROID
    #include <execinfo.h>

    static void print_trace(string file = "");
    void signal_handler (int signo, siginfo_t *si, void *data);
  #endif
#endif

using namespace std;
using namespace ipa;

//----------------------------------------------------------------------------------

DownloadTool *lib;        //< Download Library Class

bool prompt   = false;   //< When double clicking in WIN32 the window will close on exit.
int m_verbose = 1;       //< Verbosity [0-4]

bool HandleCertificates(PrgOptions& args);
void sigint_handler (int sig);
extern bool FileExists(std::string filename);

//------------------------------------------------------------------------------


/* SWISTART */
/*
 *status from IFWD_DownloadDll.DLL functions, and status given to callback functions.
 * */
/* SWISTOP */

void LibraryCallback(int channel, int status, const string& data)
{
  if (channel>1)
    cout << "Ch" << channel << " ";

  switch(status)
  {
  case IFWD_DL_ProgressUpdated:
    if (data.compare(0,4,"100%") == 0)
      cout << "\r  Progress: " << data << endl;
    else
      cout << "\r  Progress: " << data;
    cout.flush();
  break;

  case IFWD_DL_ProcessOutlineUpdated:
  case IFWD_DL_OK:
    //if (data.compare(0, 13, "Target error[") == 0)
    //  cerr << "> " << data << endl;
    //else
      cout << "> " << data << endl;
    break;

  case IFWD_DL_ProcessDetailUpdated:
    if(m_verbose)
      cout << "Details> " << data << endl;
    break;

  case IFWD_DL_Error:
    cerr << "Error> " << data << endl;
    break;

  case IFWD_DL_AT_Command:
    cout << "AT> " << data << "(" << data.length() << ")"; // << endl;
    cout << " (0x" << hex << short(static_cast<unsigned char>(data[0])) << ") " << endl;
    break;

  default:
    cerr << "DownloadLibrary> Unknown status: " << status << endl;
    break;
  }
}

//----------------------------------------------------------------------------------

void Quit (int code, string why = "")
{
  cerr << why << endl;

  if (lib) lib->CloseComm();

  if (prompt)
  {
    cout << "Press return to quit: ";
    cout.flush();
    getchar();
  }
  exit( code );
}

//----------------------------------------------------------------------------------

#ifdef __linux__
  const string FlashUSIF = "/dev/ttyUSB0";
  const string FlashUSB  = "/dev/ttyIMC1";
  const string modem      = "/dev/ttyACM1";
#elif __APPLE__
  const string FlashUSIF = "/dev/cu.usbserial";
  const string FlashUSB  = "/dev/cu.usbserial";   //< USB not available for APPLE
  const string modem     = "/dev/cu.modemserial";
#else
  const string FlashUSIF = "COM1";
  const string FlashUSB  = "USB1";
  const string modem     = "1";
#endif

int main(int argc, char **argv)
{
  string port = "1";
  string library = DEFAULT_LIBRARY;
  string at_command;

  string erase_nvm = "";
  string FL_erase = "";
  string FL_path = FileSystem_path(GetUserHomePath(), "Intel", "NVM_Data");
  string SecChCert_path = "";
  string trace_file;
  string mipi_port, boot_port, at_port, prolific_port;
  string upload_bin = "upload.bin";
  string usb_map_file = "usb_map.ini";
  string config = "downloadtool.cfg";
  string reset_switch, power_switch;
  string debug_modules = "";

  int baudrate = 921600;
  int boot_timeout = 60000;
  int comm_timeout = 60000;

  bool booted = false;
  int bootup_at_tries = 1;

  vector<int> reset_pins;
  int runs = 1;
  int erase_mode = 0;
  unsigned int start = 0, length = 0;

  #ifdef _WIN32  // Set STDOUT unbuffered (MS fix)
    setvbuf(stdout, (char *)0, 0, _IONBF);
    setvbuf(stderr, (char *)0, 0, _IONBF);
  #endif

  //====== 0. PARSE COMMAND LINE PARAMETERS =========================================
  PrgOptions args(84);
  args.addHeader( "Usage: "+ string(fname(argv[0])) +" [options] <file(s) to download>" );
  //opt.addDescription("Overall options:");

  args.add("Communication port:");
  args.add("comm-port,c=s",  &port,         string("Communication port. e.g. Generic USB: '-c1', FlashUSB Driver '-c "+FlashUSB+"', USIF: '-c"+FlashUSIF+"'.").c_str());
  args.add("mipi-port,m=s",  &mipi_port,      "Downloading via a valid MIPI device");
/* SWISTART */
#ifndef SIERRA
  args.add("boot-port=s",    &boot_port,      "Port used for booting the device");
#endif /* SIERRA */
/* SWISTOP */
  args.add("at-port=s",      &at_port,        "Port used for running AT commands after system boot");
  #ifdef _WIN32
    args.add("prolific-port", &prolific_port, "Setup the Prolific port (used with the --prolific-reset option)");
    args.add("prolific-reset",&reset_pins,    "Enable the Prolific GPIO reset. Defaults to 0. Pins avaliable are [0-3]");
  #endif

  args.add("Actions:");
  args.add("at=s",            &at_command,         "Send AT command or give a file with a list of commands");
/* SWISTART */
#ifndef SIERRA_NOERASE
  args.add("erase,e",                              "Defining that an erase is being done instead of a download (Remember to define --start-addr & --image-length)");
#endif /* SIERRA_NOERASE */
#ifndef SIERRA
  args.add("upload,u=s",      &upload_bin,         "e.g. -u image.bin, for uploading from the device, saving the memory in image.bin");
  args.add("test-sw-boot=i",  &bootup_at_tries, 3, "Testing that the device can boot (ReqForceHwReset).");
  args.add("test-boot=i",     &bootup_at_tries, 3, "Testing that the device can boot (via hardware reset).");
#endif/* SIERRA */
/* SWISTOP */
  args.add("run-mapper",      &usb_map_file,       "Run the USB Device Mapper and save in the specified map file");

  args.add("Options:");
  //args.add("config,o",       &config_file  , "Which config file to use");
/* SWISTART */
#ifndef SIERRA_NOERASE
#ifndef SIERRA
  args.add("start-addr=i",    &start,         "Define a starting address in HEX (used with --upload, --erase, & .bin files)");
  args.add("length=i",        &length,        "Define an image length in HEX (used with --upload, --erase, & .bin files)");
#else /* SIERRA */
  args.add("start-addr=i",    &start,         "Define a starting address in HEX (used with --erase)");
  args.add("length=i",        &length,        "Define an image length in HEX (used with --erase)");
#endif/* SIERRA */
#endif/* SIERRA_NOERASE */
/* SWISTOP */

  args.add("map",             &usb_map_file,  "Specify a USB Device Map (default: usb_map.ini)");
  args.add("trace-file,l=s",  &trace_file,    "File name for tracing the Download");
  args.add("baudrate,b=i",    &baudrate,      "Set the baud rate for serial connection");

  args.add("boot-timeout=i",  &boot_timeout,  "Set the boot timeout in microseconds");
  args.add("comm-timeout=i",  &comm_timeout,  "Set the communication timeout in microseconds");

/* SWISTART */
#ifndef SIERRA
  args.add("Boot Protocol Options:");
  args.add("no-boot",       &booted, (bool)1, "Don't boot the device before download (Assuming EBL is running)");
  args.add("ebl-boot",                        "Use EXEC to tell the EBL we are ready to download");
#endif/* SIERRA */
/* SWISTOP */

  args.add("Download Protocol Options:");
/* SWISTART */
#ifndef SIERRA
  args.add("no-download",                     "Skip the download procedure");
  args.add("erase-mode=i",    &erase_mode,    "Erase mode:\n   before write=0 (default), erase all=1, nothing=2, all=3");
  args.add("full-erase",      &erase_mode, 1, "Erasing whole flash before download. Same as --erase-mode=1");
  args.add("cond-erase",      &erase_mode, 2, "Conditional erase each area before download. Same as --erase-mode=2");
  args.add("nand-erase",      &erase_mode, 3, "Erasing whole flash including bad blocks. Same as --erase-mode=3");
#else /* SIERRA */
  args.add("erase-mode=i",    &erase_mode,    "Erase mode:\n   must be 0");
#endif/* SIERRA */
/* SWISTOP */

  args.add("skip-empty-blocks",               "Skip empty blocks (Faster download)");
  args.add("faster-crc",                      "Faster CRC Method");
  args.add("skip-checksum",                   "Skip Write Pack Checksum");
  args.add("skip-pre-erase",                  "Skip Pre-erase of written areas");
/* SWISTART */
#ifndef SIERRA
  args.add("area-erase",                      "Force Erase of total area length");
  args.add("cond-eeprom-erase",               "Conditional Erase of dynamic EEPROM");
#endif/* SIERRA */
/* SWISTOP */
  args.add("check-signature",                 "Check file signature HW cfg value");
  args.add("alternate-boot-speed",            "Alt. Boot Speed (ES1.0/ES1.1). Use faster speed");
/* SWISTART */
#ifndef SIERRA
  args.add("mbn",                             "Upload MBN Data");
  args.add("ban-eep-write",                   "Ban EEP Mode Write");
  args.add("eep-concept-testmode",            "Makes phone start in test mode for EEP files");
  args.add("precheck-mode",                   "Run a download without actually writing data to the target");
  //args.add("append-bootcore",                 "When uploading from NAND, append the Bootcore used for booting");

  args.add("Flashless, NAND & NVM Option:");
  args.add("flashless-path",    &FL_path,     "Path to the Flashless files");
  args.add("flashless-erase",   &FL_erase,    "Comma separated list of FlashLess parts to erase (parts: all,static,dynamic,calib)");
  args.add("flashless-disable",               "Disable the flashless feature (writing the NAND partitions)");

  args.add("testmode",                        "Start-up target in testmode");
  args.add("testmode-bltdo",                  "Start-up target in Battery Less Test & Development Operation mode");
  args.add("erase-nvm",         &erase_nvm,   "Comma separated list of NVM parts to erase (parts: calib, fix, dyn)");
#endif/* SIERRA */
/* SWISTOP */

/* SWISTART */
#ifndef SIERRA_CTF
  args.add("Certificates:");
  args.add("read-hw-info:s",                  "Read the Hardware Details");

  args.add("read-rd-cert:s",                  "Read the injected R&D certificate");
  args.add("read-cc-cert:s",                  "Read the injected CC certificate");
  args.add("read-hw-cert:s",                  "Read the injected HW certificate");
  args.add("read-mi-cert:s",                  "Read the injected Mobile ID certificate");
  args.add("read-key-cert:s",                 "Read the injected Key Swap certificate");

  args.add("write-rd-cert=s",                 "Write R&D certificate to the fls file");
  args.add("write-cc-cert=s",                 "Write CC certificate to the fls file");
  args.add("write-hw-cert=s",                 "Write HW certificate to the fls file");
  args.add("write-mi-cert=s",                 "Write Mobile ID certificate to the fls file");
  args.add("write-key-cert=s",                "Write Key Swap certificate to the fls file");

  args.add("erase-rd-cert",                   "Erase the injected R&D certificate");
  args.add("erase-cc-cert",                   "Erase the injected CC certificate");
  args.add("erase-hw-cert",                   "Erase the injected HW certificate");
  args.add("erase-mi-cert",                   "Erase the injected Mobile ID certificate");
  args.add("erase-key-cert",                  "Erase the injected Key Swap certificate");

  args.add("Secure channel:");
  args.add("certificate-path", &SecChCert_path, "Path to a folder containing the certificate(s)");
#endif/* SIERRA_CTF */
/* SWISTOP */

  args.add("Resetting:");
  args.add("no-reset",                         "Don't reset the device after download (via ReqForceHwReset command)");
  args.add("reset-switch=s",    &reset_switch,  string("e.g. -r "+modem+",1 for k8090 relay on serial port 1 and reset on switch 1").c_str());
  args.add("power-switch=s",    &power_switch,  string("e.g. -p "+modem+",3 for k8090 relay on serial port 1 and power on switch 3").c_str());

  args.add("General options:");
  args.add("runs,r",          &runs,          "Repeats the Flashing procedure arg times");
  args.add("boot-only-once",                  "With the --runs parameter this ensures we only boot target in the first iteration.");
  args.add("version",                         "Version of both DownloadTool and the loaded Library");
  args.add("sys-version",                     "Show System Software version info from an fls file");
  args.add("library",         &library,       "Use an alternative Download Library.");
  args.add("config",          &config,        "Load configuration from INI file (Default: downloadtool.cfg)");
  args.add("exit-prompt,x", &prompt, (bool)1, "Prompts before exit & between iterations if the --runs argument is used.");
  args.add("verbose,v=i",   &m_verbose,       "Verbose output of DownloadTool during execution [0-4].");
  args.add("debug,d=s",     &debug_modules,   "A comma separated list of module names [BOOT,SFH,MFH,COMM,USB]. Define debug verbosity with --verbose.");
  args.add("help,h",                          "Display this help and exit.");

  try
  {
    if (!args.parse(argc, argv))
      Quit(EXIT_FAILURE);
  }
  catch (const exception& e)
  {
    #ifndef __EXCEPTIONS
      myexception& e = chatched_exceptions.front();
    #endif

    args.showDescription();
    cout << endl;
    Quit(EXIT_FAILURE, e.what() );
  }

  if (argc < 2 || args.is_set("help"))
  {
    args.showDescription();
    Quit(EXIT_FAILURE);
  }

  vector<string> files; // = args.getFiles();
  const char** rest = args.getArguments();
  for (int i=0; rest[i]; ++i)
    files.push_back( rest[i] );

  //====== Load Download Library ==============================================
  try {
    lib = &DownloadTool::getInstance( library );

    cout << endl << "Intel Command Line FlashTool v." << VERSION_STRING
         << " (Download Library v." << lib->GetVersion() << ")" << endl << endl;

    if (args.is_set("version"))
      Quit(0);

    if ( m_verbose )
      lib->SetVerbose( m_verbose, debug_modules );

    lib->signal_library_callback.connect( FunPtr(LibraryCallback) );
    /*
     *  lib->signal_progress_update.connect( FunPtr(ProgressCallback) );
     *  lib->signal_outline_update.connect( FunPtr(OutlineCallback) );
     *  lib->signal_status.connect( FunPtr(StatusCallback) );
     *  lib->signal_error.connect( FunPtr(ErrorCallback) );
     *  lib->signal_AT_command.connect( FunPtr(ATCallback) );
     */

    //====== Verify Arguments =================================================
    if (files.size() == 0)
    {
/* SWISTART */
#ifndef SIERRA
      if (args.is_set("sys-version") || args.is_set("precheck-mode"))
#else /* SIERRA */
      if (args.is_set("sys-version"))
#endif/* SIERRA */
/* SWISTOP */
        Quit(1, "Please specify an fls file");

/* SWISTART */
#ifndef SIERRA
      if (args.is_set("erase") || args.is_set("upload") || args.is_set("erase-nvm"))
#else /* SIERRA */
#ifndef SIERRA_NOERASE
      if (args.is_set("erase"))
#else  /* SIERRA_NOERASE */
      if (0)
#endif /* SIERRA_NOERASE */
#endif/* SIERRA */
/* SWISTOP */
        Quit(1, "Please specify an fls file for booting");
    }

    // Upload & Erase needs address & length
/* SWISTART */
#ifndef SIERRA
    if (args.is_set("erase") || args.is_set("upload"))
#else /* SIERRA */
#ifndef SIERRA_NOERASE
    if (args.is_set("erase"))
#else  /* SIERRA_NOERASE */
    if (0)
#endif /* SIERRA_NOERASE */
#endif/* SIERRA */
/* SWISTOP */
      if (!args.is_set("start-addr") || !args.is_set("length"))
        Quit(1, "You need to define --start-addr & --length.");

    //====== Actions (Fls System Version & Mapper) ============================
    if (args.is_set("sys-version"))
    {
      foreach(file, files)
      {
        cout << "System Software Version:"
             << endl << lib->GetSystemVersion(*file) << endl;
      }
      Quit(EXIT_SUCCESS);
    }

    /* Load a USB Device Map */
    if (args.is_set("map"))
    {
      lib->LoadDeviceMap(usb_map_file);

      if (m_verbose)
        cout << " Map " << usb_map_file << " loaded." << endl;
    }

    /* Run the USB Device Mapper */
    if (args.is_set("run-mapper"))
    {
      int channels = (args.is_set("comm-port")) ? StringTo<int>( port ) : 1;
      if (channels == 1)
      {
        if (args.is_set("reset-switch"))   lib->SetResetRelay( reset_switch );
        if (args.is_set("power-switch"))   lib->SetPowerRelay( power_switch );
      }

      cout << "Mapping " << channels << " channel(s)..." << endl;
      for (int ch=1; ch <= channels; ch++)
      {
        cout << "  -> Please reboot device nr " << ch << " <-  " << endl;
        char* path = lib->DeviceMap(ch);
        if (m_verbose)
          cout << "  Channel " << ch << " mapped as '" << path << "'" << endl;
      }

      if (!lib->SaveDeviceMap( usb_map_file ))
        Quit(1, "Failed to save map: "+ usb_map_file);

      cout << "Map saved as " << usb_map_file << endl;
      Quit(EXIT_SUCCESS);
    }

  }
  #ifdef __EXCEPTIONS
  catch(exception& e) {
    cerr << "Error> " << e.what() << endl;
    #ifdef _WIN32
      prompt = true;
    #endif
    Quit(EXIT_FAILURE);
  }
  catch(...) {
    cerr << "Caught something really bad..." << endl;
    #ifdef _WIN32
      prompt = true;
    #endif
    Quit(EXIT_FAILURE);
  }
  #endif

  //====== Setting up download parameters =====================================
  try {
    if (fexists(config))
      lib->loadConfigIni( config );

    if (args.is_set("trace-file"))        lib->SetTraceFile( trace_file );

    lib->SetBaudrate( baudrate );
    lib->SetBootTimeout( boot_timeout );
    lib->SetCommTimeout( comm_timeout );

/* SWISTART */
#ifndef SIERRA
    if (args.is_set("precheck-mode"))
      lib->SetParameter(IFWD_DL_dll_parameter_set_precheck_mode, 1);
#endif/* SIERRA */
/* SWISTOP */

    lib->SetParameter(IFWD_DL_dll_parameter_stay_in_function, true);
    lib->SetParameter(IFWD_DL_dll_parameter_force_plain_upload, true);
/* SWISTART */
#ifndef SIERRA
    lib->SetParameter(IFWD_DL_dll_parameter_erase_mode, erase_mode);
#else /* SIERRA */
    erase_mode = 0;
    lib->SetParameter(IFWD_DL_dll_parameter_erase_mode, erase_mode);
#endif/* SIERRA */
/* SWISTOP */

    lib->SetParameter(IFWD_DL_dll_parameter_skip_empty_blocks,           args.is_set("skip-empty-blocks"));
    lib->SetParameter(IFWD_DL_dll_parameter_faster_crc_method,           args.is_set("faster-crc"));
    lib->SetParameter(IFWD_DL_dll_parameter_skip_wr_prak_crc,            args.is_set("skip-checksum"));
    lib->SetParameter(IFWD_DL_dll_parameter_use_pre_erase,              !args.is_set("skip-pre-erase"));
/* SWISTART */
#ifndef SIERRA
    lib->SetParameter(IFWD_DL_dll_parameter_force_area_erase,            args.is_set("area-erase"));
    lib->SetParameter(IFWD_DL_dll_parameter_cond_dyn_eep_erase,          args.is_set("cond-erase"));
#else /* SIERRA */
    lib->SetParameter(IFWD_DL_dll_parameter_force_area_erase,            false);
    lib->SetParameter(IFWD_DL_dll_parameter_cond_dyn_eep_erase,          false);
#endif/* SIERRA */
/* SWISTOP */
    lib->SetParameter(IFWD_DL_dll_parameter_check_sign_hw_cfg_value,     args.is_set("check-signature"));
    lib->SetParameter(IFWD_DL_dll_parameter_use_alt_boot_speed,          args.is_set("alternate-boot-speed"));
/* SWISTART */
#ifndef SIERRA
    lib->SetParameter(IFWD_DL_dll_parameter_upload_mbn_data,             args.is_set("mbn"));
    lib->SetParameter(IFWD_DL_dll_parameter_do_not_allow_EEP_mode_write, args.is_set("ban-eep-write"));
#else /* SIERRA */
    lib->SetParameter(IFWD_DL_dll_parameter_upload_mbn_data,             false);
    lib->SetParameter(IFWD_DL_dll_parameter_do_not_allow_EEP_mode_write, false);
#endif/* SIERRA */
/* SWISTOP */

/* SWISTART */
#ifndef SIERRA
    if (args.is_set("ebl-boot"))
      lib->SetParameter(IFWD_DL_dll_parameter_set_special_boot_sequence, IFWD_DL_boot_seq_use_sw_resident_ebl);
#endif/* SIERRA */
/* SWISTOP */

#ifdef _WIN32
    //< Setup Prolific GPIO reset
    if (args.is_set("prolific-reset") || args.is_set("prolific-port"))
    {
      if (!port.empty())
        lib->SetComPort( port );

      if (prolific_port.empty())
        lib->SetupProlificReset(port, reset_pins);
      else
        lib->SetupProlificReset(prolific_port, reset_pins);
      Sleep(300);
    }
#endif

    //if (args.is_set("test-mode"))         lib->m_test_mode = true;
    if (args.is_set("reset-switch"))      lib->SetResetRelay( reset_switch );
    if (args.is_set("power-switch"))      lib->SetPowerRelay( power_switch );

    //< Setting up the Communication Port...
    if (args.is_set("mipi-port"))
    {
      lib->SetParameter(IFWD_DL_dll_parameter_allow_hw_channel_switch, true);
      lib->SetComPort( port, mipi_port );
    }

/* SWISTART */
#ifndef SIERRA
    if (!lib->is_open())
      lib->SetComPort( port, boot_port.empty() ? port : boot_port );

    if (args.is_set("no-boot"))
    {
      if (!args.is_set("upload"))
        throw_exception("Option '--no-boot' only works with upload", 1);

      lib->OkCallBack(1);
    }
#else /* SIERRA */

    cout << "set com port" << endl;

    if (!lib->is_open())
          lib->SetComPort( port, port );

    cout << "set com port done"<< endl;
#endif /* SIERRA */

#ifndef SIERRA_CTF
    if (args.is_set("certificate-path"))
    {
      lib->setup_secure_channel(SecChCert_path);
    }
#endif /* SIERRA_CTF */
/* SWISTOP */
  }
  catch (const exception& e) {
    #ifndef __EXCEPTIONS
      myexception& e = chatched_exceptions.front();
    #endif
    Quit(1, "  Setup Error: "+ string(e.what()) );
  }
  #ifdef __EXCEPTIONS
  catch (const string& ex) {
    Quit(1, "  Setup Error String: "+ ex );
  }
  catch (...) {
    Quit(1, "Unknown error occurred during parsing of your arguments.");
  }
  #endif
  //  dl.saveConfigIni("download.conf");  // Save the new configuration

  signal(SIGINT, sigint_handler);  /* Interrupt from keyboard (ctrl+c) */
  #ifndef _WIN32
    signal(SIGHUP, sigint_handler);  /* Hangup */
    signal(SIGKILL, sigint_handler); /* Kill */
    signal(SIGTERM, sigint_handler); /* Termination */
    signal(SIGSTOP, sigint_handler); /* Stop the process */
  #endif

  #if defined(DEBUG) && !defined(ANDROID) && !defined(WIN32)
    struct sigaction sa, osa;
    sa.sa_flags = SA_ONSTACK | SA_RESTART | SA_SIGINFO;
    sa.sa_sigaction = signal_handler;
    //sigaction(SIGABRT, &sa, &osa);
    sigaction(SIGSEGV, &sa, &osa);
  #endif

  //====== Library API Actions ================================================
  try {
    //====== 3. Adding the flash files =========================================
    string download_bin;

/* SWISTART */
#ifndef SIERRA
    bool flashless = false;
#endif/* SIERRA */
/* SWISTOP */

    foreach(it, files)
    {
      string file = *it;
      FlsFile* fls = lib->AddFile(file);
      if (!fls)
        download_bin = file;
/* SWISTART */
#ifndef SIERRA
      else if (fls->platform.technology == TECHNOLOGY_FLASHLESS)
        flashless = true;
#endif/* SIERRA */
/* SWISTOP */
    }

/* SWISTART */
#ifndef SIERRA
    if (args.is_set("erase-nvm"))
      lib->SetupNvmErase( erase_nvm  );

    if (args.is_set("testmode"))
    {
      uint32_t tags[] = {(uint32_t)NVM_TAG_STARTUP_MODE_TEST_V2, (uint32_t)NVM_TAG_STARTUP_MODE_TEST, 0};
      lib->SetupTestmode(tags);
    }

    if (args.is_set("testmode-bltdo"))
    {
      uint32_t tags[] = {(uint32_t)NVM_TAG_BLTDO_V2, (uint32_t)NVM_TAG_BLTDO, 0};
      lib->SetupTestmode(tags);
    }

    // Checking for required arguments
    if (!download_bin.empty() && (!args.is_set("start-addr") || !args.is_set("length")))
      Quit(1, "You need to define --start-addr & --length.");
#else /* SIERRA */
    if (!download_bin.empty())
      Quit(1, "Not supported.");
#endif/* SIERRA */
/* SWISTOP */

    //====== Library API Actions ====
    for (int i=1; i <= runs; ++i)
    {
      if (runs>1)
      {
        if (prompt && i>1)
        {
          cout << endl << "Press return to start iteration " << i << ":";
          cout.flush();
          getchar();
        }

        cout << endl << " *** Iteration " << i << " *** " << endl;

        if (!prompt && i>1)
          Sleep(3000);
      }

      if (files.size())
      {
        // Setting Flashless options
/* SWISTART */
#ifndef SIERRA
        if (flashless && !args.is_set("flashless-disable"))
          lib->SetupFlashlessErase(FL_path, FL_erase);
#endif/* SIERRA */
/* SWISTOP */

        // Boot Target (PSI,EBL,SLB)
        if (!args.is_set("boot-only-once") && !booted)
        {
          if (!lib->BootTarget())
            throw_exception("Failed to boot", 1);
          booted = true;
        }

/* SWISTART */
        if (0) {}
#ifndef SIERRA_NOERASE
        if (args.is_set("erase"))
        {
          lib->Erase(start, length);
        }
#endif /* SIERRA_NOERASE */
#ifndef SIERRA
        else if (args.is_set("upload"))
        {
          lib->UploadFile(start, length, upload_bin);
        }
#endif/* SIERRA */
        else
        {
#ifndef SIERRA_CTF
          // Out of Session Certificate Mode
          HandleCertificates(args);
#endif /* SIERRA_CTF */
/* SWISTOP */

/* SWISTART */
#ifndef SIERRA
          if (!args.is_set("no-download"))
#endif/* SIERRA */
/* SWISTOP */
          {
            // Download files...
/* SWISTART */
#ifndef SIERRA
            if (!download_bin.empty())
              lib->DownloadBinary(start, length, download_bin);
            else
#endif/* SIERRA */
/* SWISTOP */
              lib->DownloadFiles();
          }
        }
      }

      //====== 4 Run the Action ===============================================
      if (args.is_set("no-reset") == 0)
      {
        cout << "Force Target Reset!" << endl;
        lib->ForceTargetReset();
      }

      if (!at_port.empty())
      {
        cout << "Changing to AT port: " << at_port << endl;
        lib->SetComPort(at_port);
        lib->CloseComm();
      }

      // Look into the FlsFile's to see if there is an embedded AT script
      vector<string> post_commands = lib->GetPostScript();

/* SWISTART */
#ifndef SIERRA
      if (post_commands.size() || args.is_set("test-boot") || args.is_set("test-sw-boot") || args.is_set("at"))
#else /* SIERRA */
      if (post_commands.size() ||args.is_set("at"))
#endif /* SIERRA */
/* SWISTOP */
      {
        cout << endl << " Waiting for phone to boot..." << endl;

/* SWISTART */
#ifndef SIERRA
        bool reboot = args.is_set("test-sw-boot") ? false : true;
#else /* SIERRA */
        bool reboot = true;
#endif /* SIERRA */
/* SWISTOP */

        if (!lib->startAT( reboot, bootup_at_tries ))
          Quit(1, " Failed> Could not connect to AT Terminal :(");

        cout << endl << " Success> AT Terminal Connected :)" << endl << endl;

        //< Run the internal Post commands first
        if (post_commands.size())
        {
          cout << " Sending Post AT Commands..." << endl;
          for (size_t i=0; i < post_commands.size(); ++i)
          {
            if (post_commands[i][0] != '#')
            {
              string resp = lib->sendAT( post_commands[i] );

              if (resp.find("OK") == string::npos)
                throw_exception("AT command failed", 2);
            }
          }
        }

        if (args.is_set("at"))
        {
          if (!fexists(at_command))
          {
            cout << "Sending AT command '" << at_command << "'..." << endl;
            cout << endl << "Response: " << endl << lib->sendAT( at_command ) << endl;
          }
          else
          {
            cout << "AT command list: " << at_command << endl;

            ifstream file(at_command.c_str());
            if (!file)
              throw_exception("Could not open AT command script", 2);

            while (std::getline(file, at_command))
            {
              if (at_command.empty())
                continue;

              string resp = lib->sendAT( at_command );
              if (resp.find("OK") == string::npos)
                throw_exception("AT command failed\nResponse: ["+resp+"] ", 2);
            }

            file.close();
          }
        }
      }

    } //< foreach repeat

    //====== 5 Done =================================================
    if (runs > 1)
      cout << endl << "Successfully run " << runs << " times!" << endl;

    //cout << endl << "Bye bye :)" << endl;
    Quit(0);
  }
  /* This catches all Download Library errors */
  catch(const exception& e) {
    #ifndef __EXCEPTIONS
      myexception& e = chatched_exceptions.front();
    #endif
    int code = lib->LastStatus || EXIT_FAILURE;
    cerr << "   Library Error: " << e.what() << " (" << code << ")" << endl;
    Quit( code );
  }
  #ifdef __EXCEPTIONS
  catch(...) {
    int code = lib->LastStatus || EXIT_FAILURE;
    if (code==EXIT_FAILURE)
      Quit(code, "   Library Error: Unknown exception");
    else
      Quit(code, "   Library Error: Unknown exception. Last error code: "+ Stringify(code));
  }
  #endif
}


/* SWISTART */
#ifndef SIERRA_CTF
/**
 * Out of Session Certificate Handling
 */
static string& getCertHwId(const char* arg = "")
{
  static string hwid;
  if (hwid.empty())
  {
    hwid = fname( lib->OutOfSessionGetHwDetails( arg ).c_str() );
    hwid = hwid.substr(0, hwid.size()-4);
  }
  return hwid;
}


bool HandleCertificates(PrgOptions& args)
{
  bool commit = false;

  //< Erase Certificates
  if (args.is_set("erase-rd-cert"))
    commit |= lib->OutOfSessionErase(IFWD_DL_oosc_type_rd_cert);
  if (args.is_set("erase-cc-cert"))
    commit |= lib->OutOfSessionErase(IFWD_DL_oosc_type_cc_cert);
  if (args.is_set("erase-hw-cert"))
    commit |= lib->OutOfSessionErase(IFWD_DL_oosc_type_hw_cert);
  if (args.is_set("erase-mi-cert"))
    commit |= lib->OutOfSessionErase(IFWD_DL_oosc_type_mi_cert);
  if (args.is_set("erase-key-cert"))
    commit |= lib->OutOfSessionErase(IFWD_DL_oosc_type_key_cert);

  //< Write Certificates
  if (args.is_set("write-rd-cert"))
    commit |= lib->OutOfSessionWrite(IFWD_DL_oosc_type_rd_cert,
                                    args.get<char*>("write-rd-cert"));
  if (args.is_set("write-cc-cert"))
    commit |= lib->OutOfSessionWrite(IFWD_DL_oosc_type_cc_cert,
                                    args.get<char*>("write-cc-cert"));
  if (args.is_set("write-hw-cert"))
    commit |= lib->OutOfSessionWrite(IFWD_DL_oosc_type_hw_cert,
                                    args.get<char*>("write-hw-cert"));
  if (args.is_set("write-mi-cert"))
    commit |= lib->OutOfSessionWrite(IFWD_DL_oosc_type_mi_cert,
                                    args.get<char*>("write-mi-cert"));
  if (args.is_set("write-key-cert"))
    commit |= lib->OutOfSessionWrite(IFWD_DL_oosc_type_key_cert,
                                    args.get<char*>("write-key-cert"));

  //< Reading HW Details
  if ((commit |= args.is_set("read-hw-info")))
  {
    char* arg = args.get<char*>("read-hw-info");
    getCertHwId( ((!arg || arg[0] == 1) ? "" : arg) );
  }

  //< Read Certificates
  if (args.is_set("read-rd-cert"))
  {
    string hwid = args.get<char*>("read-rd-cert");
    if (hwid[0] == 1)
      hwid = getCertHwId()+"_RDC.bin";

    commit |= lib->OutOfSessionRead(IFWD_DL_oosc_type_rd_cert, (char*)hwid.c_str());
  }

  if (args.is_set("read-cc-cert"))
  {
    string hwid = args.get<char*>("read-cc-cert");
    if (hwid[0] == 1)
      hwid = getCertHwId()+"_CCC.bin";

    commit |= lib->OutOfSessionRead(IFWD_DL_oosc_type_cc_cert, (char*)hwid.c_str());
  }

  if (args.is_set("read-hw-cert"))
  {
    string hwid = args.get<char*>("read-hw-cert");
    if (hwid[0] == 1)
      hwid = getCertHwId()+"_HWC.bin";

    commit |= lib->OutOfSessionRead(IFWD_DL_oosc_type_hw_cert, (char*)hwid.c_str());
  }

  if (args.is_set("read-mi-cert"))
  {
    string hwid = args.get<char*>("read-mi-cert");
    if (hwid[0] == 1)
      hwid = getCertHwId()+"_MIC.bin";

    commit |= lib->OutOfSessionRead(IFWD_DL_oosc_type_mi_cert, (char*)hwid.c_str());
  }

  if (args.is_set("read-key-cert"))
  {
    string hwid = args.get<char*>("read-key-cert");
    if (hwid[0] == 1)
      hwid = getCertHwId()+"_Key.bin";

    commit |= lib->OutOfSessionRead(IFWD_DL_oosc_type_key_cert, (char*)hwid.c_str());
  }

  if (commit)
    lib->OutOfSessionCommit();

  return commit;
}
//----------------------------------------------------------------------------------
#endif/* SIERRA_CTF */
/* SWISTOP */

/**
 *  Function watchdog thread
 **/
static int sig_catched = 0;
static int signals_catched = 0;

void watchdog_close(int)
{
  exit( sig_catched );
}

/**
 *  Catches all signals...
 **/
void sigint_handler (int sig)
{
  sig_catched = sig;
  if (signals_catched++) //< This should not be required anymore
    exit(sig);

  cerr << endl << "Caught signal " << sig << ". Aborting processes and exit..."
       << endl << "Abort logged in trace file" << endl;

  {
    Watchdog scope_wd(10000, watchdog_close); //< 10 sec
    DownloadTool::getInstance().CloseComm();
  }

  if (prompt)
  {
    cout << "Press return to quit: ";
    cout.flush();
    getchar();
  }
  exit(sig);
}

#if !defined(ANDROID) && !defined(WIN32)
//#if defined(DEBUG)

void my_terminate(void);

namespace {
  static const bool SET_TERMINATE = std::set_terminate(my_terminate);
}

/**
 * Some signals are handled by this function (SIGFAULT)
 */
void signal_handler (int signo, siginfo_t *si, void *data)
{
  cerr << endl << "Segmentation fault!" << endl;
  print_trace();
  exit(signo);
}

/**
 *  Dump the stack trace via GDB
 *
 * @param file   Define a file to dump to (Default: stdout)
 */
static void print_trace(string file)
{
  char pid_buf[30];
  sprintf(pid_buf, "%d", getpid());
  char name_buf[512];
  name_buf[readlink("/proc/self/exe", name_buf, 511)]=0;
  int child_pid = fork();
  if (!child_pid)
  {
    if (!file.empty())
    {
      int fd = open(file.c_str(),O_CREAT | O_RDWR , 00777 );
      dup2(fd, 1); // redirect output to file
      close(fd);
    }
    else
    {
      dup2(2, 1); // redirect output to stderr
    }

    cout << endl << "------------ Stack Trace (" << pid_buf << ") ------------" << endl;

    // For more debug change "bt" to "bt full"
    execlp("gdb", "gdb", "--batch", "-n", "-ex", "thread", "-ex", "bt", name_buf, pid_buf, NULL);
    abort();
  } else {
    waitpid(child_pid,NULL,0);
    if (!file.empty())
      cerr << endl << " Stack Trace saved in " << file << endl;
  }
}

/**
 *  Catching exceptions & Dumping stack trace
 */
void my_terminate(void)
{
//  string trace_file = "stacktrace.dbg";
  static int tried_throw = false;

  try {
      // try once to re-throw currently active exception
      if (!tried_throw++) throw;
  }
  catch (const std::exception &e) {
      cerr << endl << "  Error> " << e.what() << endl;
  }
  catch (...) {
      cerr << endl << "  Unknown/Unhandled Exception." << endl;
  }

  print_trace();

  abort();
}
#endif
