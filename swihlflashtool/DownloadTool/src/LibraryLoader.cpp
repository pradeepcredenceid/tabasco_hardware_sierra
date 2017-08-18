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
       $File name:  /msw_tools/FlashTool/DownloadTool/src/LibraryLoader.cpp $
       $CC-Version: .../dhkristx_140314_v0/1 $
       $Date:       2014-05-01    14:01:27 UTC $
    ------------------------------------------------------------------------- */

#include <stdexcept>
#include <iostream>
#include <string>

#include "LibraryLoader.h"

#if defined(ANDROID) || defined(STATIC_DL_LINKING)

#include "IMC_DownloadDll.h"

#warning "Android build"
LibraryLoader::LibraryLoader(std::string _file)
{
  (void)_file;

  //DllMain(0, DLL_PROCESS_ATTACH, NULL); // my_load();

  get_version                = (IFWD_DL_get_versionfunc*)IFWD_DL_get_dll_version;
  set_module_verbose         = (IFWD_set_module_verbosefunc*)IFWD_set_module_verbose;
  get_sw_version_data        = (IFWD_DL_get_sw_version_datafunc*)IFWD_DL_get_sw_version_data;
  set_parameter              = (IFWD_DL_set_parameterfunc*)IFWD_DL_set_dll_parameter;
  get_parameter              = (IFWD_DL_get_parameterfunc*)IFWD_DL_dll_get_parameter;
  init_callback              = (IFWD_DL_init_callbackfunc*)IFWD_DL_init_callback;
  open_comm_port             = (IFWD_DL_open_comm_portfunc*)IFWD_DL_open_comm_port;
  close_comm_port            = (IFWD_DL_close_comm_portfunc*)IFWD_DL_close_comm_port;
  setup_prolific_gpio_reset  = (IFWD_DL_setup_prolific_gpio_resetfunc*)IFWD_DL_setup_prolific_gpio_reset;
  boot_target                = (IFWD_DL_boot_targetfunc*)IFWD_DL_boot_target;
  boot_bin_target            = (IFWD_DL_boot_bin_targetfunc*)IFWD_DL_boot_bin_target;
  set_get_control_signal     = (IFWD_DL_set_get_control_signalfunc*)IFWD_DL_set_get_control_signal;
  download_fls_file          = (IFWD_DL_download_fls_filefunc*)IFWD_DL_download_fls_file;
  download_eep_file          = (IFWD_DL_download_eep_filefunc*)IFWD_DL_download_eep_file;
  download_dffs_file         = (IFWD_DL_download_dffs_filefunc*)IFWD_DL_download_dffs_file;
  download_cust_file         = (IFWD_DL_download_cust_filefunc*)IFWD_DL_download_cust_file;
  upload_eep_file            = (IFWD_DL_upload_eep_filefunc*)IFWD_DL_upload_eep_file;
  update_static_eeprom       = (IFWD_DL_update_static_eepromfunc*)IFWD_DL_update_static_eeprom;
  upload_bin_image           = (IFWD_DL_upload_bin_imagefunc*)IFWD_DL_upload_bin_image;
  download_bin_image         = (IFWD_DL_download_bin_imagefunc*)IFWD_DL_download_bin_image;
  erase_image                = (IFWD_DL_erase_imagefunc*)IFWD_DL_erase_image;
  //verify_flash_image         = (IFWD_DL_verify_flash_imagefunc*)IFWD_DL_verify_flash_image;
  abort_process              = (IFWD_DL_abort_processfunc*)IFWD_DL_abort_process;
  force_target_reset         = (IFWD_DL_force_target_resetfunc*)IFWD_DL_force_target_reset;
  end_of_process             = (IFWD_DL_end_of_processfunc*)IFWD_DL_end_of_process;
  start_AT_mode              = (IFWD_DL_start_AT_modefunc*)IFWD_DL_start_AT_mode;
  stop_AT_mode               = (IFWD_DL_stop_AT_modefunc*)IFWD_DL_stop_AT_mode;
  AT_send                    = (IFWD_DL_AT_sendfunc*)IFWD_DL_AT_send;
  set_external_file_handling = (IFWD_DL_set_external_file_handlingfunc*)IFWD_DL_set_external_file_handling;

  TOC_get_nof_items          = (IFWD_DL_TOC_get_nof_itemsfunc*)IFWD_DL_TOC_get_nof_items;
  TOC_get_memory_class       = (IFWD_DL_TOC_get_memory_classfunc*)IFWD_DL_TOC_get_memory_class;
  TOC_get_hw_platform        = (IFWD_DL_TOC_get_hw_platformfunc*)IFWD_DL_TOC_get_hw_platform;
  TOC_get_sw_option          = (IFWD_DL_TOC_get_sw_optionfunc*)IFWD_DL_TOC_get_sw_option;
  get_sys_version            = (IFWD_DL_get_sys_versionfunc*)IFWD_DL_get_sys_version;
  TOC_get_filename           = (IFWD_DL_TOC_get_filenamefunc*)IFWD_DL_TOC_get_filename;
  TOC_get_checksum           = (IFWD_DL_TOC_get_checksumfunc*)IFWD_DL_TOC_get_checksum;
  TOC_get_filesize           = (IFWD_DL_TOC_get_filesizefunc*)IFWD_DL_TOC_get_filesize;
  TOC_get_flash_technology   = (IFWD_DL_TOC_get_flash_technologyfunc*)IFWD_DL_TOC_get_flash_technogy;
  get_nof_progressbars       = (IFWD_DL_get_nof_progressbarsfunc*)IFWD_DL_dll_get_nof_progressbars;

  NVM_get_nof_user_options   = (IFWD_DL_NVM_get_nof_user_optionsfunc*)IFWD_DL_NVM_get_nof_user_options;
  NVM_get_user_option        = (IFWD_DL_NVM_get_user_optionfunc*)IFWD_DL_NVM_get_user_option;
  NVM_set_user_option        = (IFWD_DL_NVM_set_user_optionfunc*)IFWD_DL_NVM_set_user_option;
  NVM_set_flashless_option   = (IFWD_DL_NVM_set_flashless_optionfunc*)IFWD_DL_NVM_set_flashless_option;

  trace_insert               = (IFWD_DL_trace_insertfunc*)IFWD_DL_trace_insert;
  out_of_session_comm        = (IFWD_DL_out_of_session_commfunc*)IFWD_DL_out_of_session_comm;
  get_next_free_channel      = (IFWD_get_next_free_channelfunc*)IFWD_get_next_free_channel;

  device_map                 = (IFWD_device_mapfunc*)IFWD_device_map;
  device_map_save            = (IFWD_device_map_savefunc*)IFWD_device_map_save;
  device_map_load            = (IFWD_device_map_loadfunc*)IFWD_device_map_load;
  secure_channel_setup       = (IFWD_DL_secure_channel_setupfunc*)IFWD_DL_secure_channel_setup;
}

LibraryLoader::~LibraryLoader()
{
  //DllMain(0, DLL_PROCESS_DETACH, NULL); // my_unload();
}

#else

#ifdef __EXCEPTIONS
  #include <stdexcept>
  #define throw_logic_error(str) throw logic_error(str)
#else
  static void throw_logic_error(string str)
  {
    fprintf(stderr,"%s\n", str.c_str() );
  }
#endif

LibraryLoader::LibraryLoader(std::string _file)
  : hinst(NULL), file(_file), updated(true)
{
  load( file );
}

LibraryLoader::~LibraryLoader(void)
{
  FreeLibrary( hinst );
}

#ifdef _WIN32
  static std::string dlerror()
  {
    char* pCopy;
    const wchar_t* Error = 0;
    DWORD err = GetLastError();

    if (::FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, err, 0, (LPTSTR)&Error, 0, (va_list*)0) == 0)
    {
        return std::string("");
    }
    if (sizeof(TCHAR) == sizeof(char))
    {
        size_t size = strlen((const char*)Error);
        pCopy = new char[size + 1];
        strcpy(pCopy, (const char*)Error);
    } else {
        size_t size = wcstombs((char*)0, Error, 0);
        pCopy = new char[size + 1];
        wcstombs(pCopy, Error, size+1);
    }
    return std::string(pCopy);
  }
#endif


/** ImportFunction()
 *
 *  Uses dlsym or GetProcAddress for loading a library symbol.
 *  Checks that the function was loaded, and fails on error.
 *
 *  \param  name  => The name of the symbol
 *  \return void* => The loaded symbol
 **/
void* LibraryLoader::ImportFunction(std::string name, bool fail_on_error)
{
  //cout << "Looking up " << name << endl;
  void* func = (void*)GetProcAddress(hinst, name.c_str());
  #ifdef _WIN32
    if (!func)
    {
      string name1 = std::string("_").append(name);
      //cout << "Looking up " << name1 << endl;
      func = (void*)GetProcAddress(hinst, name1.c_str());
      if (!func) {
        name1 = std::string("_").append(name1);
        func = (void*)GetProcAddress(hinst, name1.c_str());
      }
    }
  #endif

  if (!func && fail_on_error)
    throw_logic_error( "Error looking up function '"+ name +"': "+ string(dlerror()) );

  return func;
  //return reinterpret_cast<T*>(func);
}


/** load()
 *
 *  Loads all the required functionalities from a given Download Library
 *  Exceptions are thrown if the library can't load or a function is missing.
 *
 *  \param  file  => Filename or path to the Library
 **/
void LibraryLoader::load(std::string& file)
{
  //cout << "Freeing old library..." << endl;
  if (hinst)
    FreeLibrary(hinst);

  //cout << "Loading " << file << endl;
  hinst = LoadLibraryA( (char*)file.c_str() );
  //cout << file << " loaded" << endl;

  if(!hinst)
  {
    #ifdef SECONDARY_LIBRARY
      cerr << "Could not load library: "+ string(dlerror())
           << endl << "Trying secoday library (" << SECONDARY_LIBRARY << ")..." << endl;
      hinst = LoadLibraryA( SECONDARY_LIBRARY );
    #endif
    if(!hinst)
    {
      throw_logic_error( "Could not load library: "+ string(dlerror()) );
      return;
    }
  }

  get_version                = (IFWD_DL_get_versionfunc*)ImportFunction("IFWD_DL_get_dll_version");
  set_module_verbose         = (IFWD_set_module_verbosefunc*)ImportFunction("IFWD_set_module_verbose", false);

  get_sw_version_data        = (IFWD_DL_get_sw_version_datafunc*)ImportFunction("IFWD_DL_get_sw_version_data");
  set_parameter              = (IFWD_DL_set_parameterfunc*)ImportFunction("IFWD_DL_set_dll_parameter");
  get_parameter              = (IFWD_DL_get_parameterfunc*)ImportFunction("IFWD_DL_dll_get_parameter");
  init_callback              = (IFWD_DL_init_callbackfunc*)ImportFunction("IFWD_DL_init_callback");
  open_comm_port             = (IFWD_DL_open_comm_portfunc*)ImportFunction("IFWD_DL_open_comm_port");
  close_comm_port            = (IFWD_DL_close_comm_portfunc*)ImportFunction("IFWD_DL_close_comm_port");
  setup_prolific_gpio_reset  = (IFWD_DL_setup_prolific_gpio_resetfunc*)ImportFunction("IFWD_DL_setup_prolific_gpio_reset", false);    // v.4.99
  boot_target                = (IFWD_DL_boot_targetfunc*)ImportFunction("IFWD_DL_boot_target");
  boot_bin_target            = (IFWD_DL_boot_bin_targetfunc*)ImportFunction("IFWD_DL_boot_bin_target");
  set_get_control_signal     = (IFWD_DL_set_get_control_signalfunc*)ImportFunction("IFWD_DL_set_get_control_signal");
  download_fls_file          = (IFWD_DL_download_fls_filefunc*)ImportFunction("IFWD_DL_download_fls_file");
  download_eep_file          = (IFWD_DL_download_eep_filefunc*)ImportFunction("IFWD_DL_download_eep_file");
  download_dffs_file         = (IFWD_DL_download_dffs_filefunc*)ImportFunction("IFWD_DL_download_dffs_file");
  download_cust_file         = (IFWD_DL_download_cust_filefunc*)ImportFunction("IFWD_DL_download_cust_file");
/* SWISTART */
#ifndef SIERRA
  upload_eep_file            = (IFWD_DL_upload_eep_filefunc*)ImportFunction("IFWD_DL_upload_eep_file");
  update_static_eeprom       = (IFWD_DL_update_static_eepromfunc*)ImportFunction("IFWD_DL_update_static_eeprom");
  upload_bin_image           = (IFWD_DL_upload_bin_imagefunc*)ImportFunction("IFWD_DL_upload_bin_image");
#endif/* SIERRA */
/* SWISTOP */
  download_bin_image         = (IFWD_DL_download_bin_imagefunc*)ImportFunction("IFWD_DL_download_bin_image");

/* SWISTART */
#ifndef SIERRA_NOERASE
  erase_image                = (IFWD_DL_erase_imagefunc*)ImportFunction("IFWD_DL_erase_image");
#endif /* SIERRA_NOERASE */
/* SWISTOP */
  //verify_flash_image         = (IFWD_DL_verify_flash_imagefunc*)ImportFunction("IFWD_DL_verify_flash_image");
  abort_process              = (IFWD_DL_abort_processfunc*)ImportFunction("IFWD_DL_abort_process");
  force_target_reset         = (IFWD_DL_force_target_resetfunc*)ImportFunction("IFWD_DL_force_target_reset");
  end_of_process             = (IFWD_DL_end_of_processfunc*)ImportFunction("IFWD_DL_end_of_process");
  start_AT_mode              = (IFWD_DL_start_AT_modefunc*)ImportFunction("IFWD_DL_start_AT_mode");
  stop_AT_mode               = (IFWD_DL_stop_AT_modefunc*)ImportFunction("IFWD_DL_stop_AT_mode");
  AT_send                    = (IFWD_DL_AT_sendfunc*)ImportFunction("IFWD_DL_AT_send");
  set_external_file_handling = (IFWD_DL_set_external_file_handlingfunc*)ImportFunction("IFWD_DL_set_external_file_handling");

  TOC_get_nof_items          = (IFWD_DL_TOC_get_nof_itemsfunc*)ImportFunction("IFWD_DL_TOC_get_nof_items");
  TOC_get_memory_class       = (IFWD_DL_TOC_get_memory_classfunc*)ImportFunction("IFWD_DL_TOC_get_memory_class");
  TOC_get_hw_platform        = (IFWD_DL_TOC_get_hw_platformfunc*)ImportFunction("IFWD_DL_TOC_get_hw_platform");
  TOC_get_sw_option          = (IFWD_DL_TOC_get_sw_optionfunc*)ImportFunction("IFWD_DL_TOC_get_sw_option");
  get_sys_version            = (IFWD_DL_get_sys_versionfunc*)ImportFunction("IFWD_DL_get_sys_version", false);    // v.4.10x
  TOC_get_filename           = (IFWD_DL_TOC_get_filenamefunc*)ImportFunction("IFWD_DL_TOC_get_filename");
  TOC_get_checksum           = (IFWD_DL_TOC_get_checksumfunc*)ImportFunction("IFWD_DL_TOC_get_checksum");
  TOC_get_filesize           = (IFWD_DL_TOC_get_filesizefunc*)ImportFunction("IFWD_DL_TOC_get_filesize");
  TOC_get_flash_technology   = (IFWD_DL_TOC_get_flash_technologyfunc*)ImportFunction("IFWD_DL_TOC_get_flash_technogy");
  get_nof_progressbars       = (IFWD_DL_get_nof_progressbarsfunc*)ImportFunction("IFWD_DL_dll_get_nof_progressbars");

  NVM_get_nof_user_options   = (IFWD_DL_NVM_get_nof_user_optionsfunc*)ImportFunction("IFWD_DL_NVM_get_nof_user_options");
  NVM_get_user_option        = (IFWD_DL_NVM_get_user_optionfunc*)ImportFunction("IFWD_DL_NVM_get_user_option");
  NVM_set_user_option        = (IFWD_DL_NVM_set_user_optionfunc*)ImportFunction("IFWD_DL_NVM_set_user_option");
/* SWISTART */
#ifndef SIERRA
  NVM_set_flashless_option   = (IFWD_DL_NVM_set_flashless_optionfunc*)ImportFunction("IFWD_DL_NVM_set_flashless_option");
#endif /* SIERRA */
/* SWISTOP */

  trace_insert               = (IFWD_DL_trace_insertfunc*)ImportFunction("IFWD_DL_trace_insert") ;
/* SWISTART */
#ifndef SIERRA_CTF
  out_of_session_comm        = (IFWD_DL_out_of_session_commfunc*)ImportFunction("IFWD_DL_out_of_session_comm");
#endif /* SIERRA_CTF */
/* SWISTOP */
  get_next_free_channel      = (IFWD_get_next_free_channelfunc*)ImportFunction("IFWD_get_next_free_channel", false);

  device_map                 = (IFWD_device_mapfunc*)ImportFunction("IFWD_device_map", false);              // v.4.106
  device_map_save            = (IFWD_device_map_savefunc*)ImportFunction("IFWD_device_map_save", false);    // v.4.106
  device_map_load            = (IFWD_device_map_loadfunc*)ImportFunction("IFWD_device_map_load", false);    // v.4.106

/* SWISTART */
#ifndef SIERRA_CTF
  secure_channel_setup       = (IFWD_DL_secure_channel_setupfunc*)ImportFunction("IFWD_DL_secure_channel_setup", false);  // v.4.137
#endif /* SIERRA_CTF */
/* SWISTOP */

}

#endif
