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
       $File name:  /msw_tools/FlashTool/DownloadTool/src/LibraryLoader.h $
       $CC-Version: .../oint_tools_flashtoole2_01/12 $
       $Date:       2013-12-04    19:31:36 UTC $
    ------------------------------------------------------------------------- */

#ifndef LIBRARYLOADER_H_
#define LIBRARYLOADER_H_

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

#include "DownloadTool.h"


#ifndef DEFAULT_LIBRARY
  #ifdef __linux__
    #define DEFAULT_LIBRARY   "libDownloadTool.so"
    #define SECONDARY_LIBRARY "./libDownloadTool.so"
  #elif _WIN32
    #define DEFAULT_LIBRARY   "DownloadTool.dll"
    #define SECONDARY_LIBRARY "IMC_Download.dll"
  #elif __APPLE__
    #define DEFAULT_LIBRARY   "dylibDownloadTool.dylib"
    #define SECONDARY_LIBRARY "dylibIMCdownload.dylib"
  #else
    #define DEFAULT_LIBRARY ""
    #define SECONDARY_LIBRARY ""
  #endif
#endif

/*---------------------------------------------*/
/* Library Loader Class (C++)                  */
/*---------------------------------------------*/
class LibraryLoader
{
public:
  LibraryLoader(std::string _file = DEFAULT_LIBRARY);
  ~LibraryLoader(void);

#if !defined(ANDROID) || !defined(STATIC_DL_LINKING)
  void load(std::string& file);

private:
  HINSTANCE hinst;        //< Pointer to the loaded library

  void* ImportFunction(std::string name, bool fail_on_error = true);

public:
  std::string file;       //< Name of the Library file
  bool updated;           //< Indicator that the library is top version
#endif

public:
  IFWD_DL_get_versionfunc*                get_version;
  IFWD_set_module_verbosefunc*            set_module_verbose;
  IFWD_DL_get_sw_version_datafunc*        get_sw_version_data;
  IFWD_DL_set_parameterfunc*              set_parameter;
  IFWD_DL_get_parameterfunc*              get_parameter;
  IFWD_DL_init_callbackfunc*              init_callback;

  IFWD_DL_open_comm_portfunc*             open_comm_port;
  IFWD_DL_close_comm_portfunc*            close_comm_port;
  IFWD_DL_boot_targetfunc*                boot_target;
  IFWD_DL_boot_bin_targetfunc*            boot_bin_target;
  IFWD_DL_set_get_control_signalfunc*     set_get_control_signal;
  IFWD_DL_setup_prolific_gpio_resetfunc*  setup_prolific_gpio_reset;

  IFWD_DL_download_bin_imagefunc*         download_bin_image;
  IFWD_DL_download_fls_filefunc*          download_fls_file;
  IFWD_DL_download_eep_filefunc*          download_eep_file;
  IFWD_DL_download_dffs_filefunc*         download_dffs_file;
  IFWD_DL_download_cust_filefunc*         download_cust_file;

  IFWD_DL_upload_eep_filefunc*            upload_eep_file;
  IFWD_DL_upload_bin_imagefunc*           upload_bin_image;

  IFWD_DL_update_static_eepromfunc*       update_static_eeprom;
  IFWD_DL_erase_imagefunc*                erase_image;
  IFWD_DL_verify_flash_imagefunc*         verify_flash_image;
  IFWD_DL_abort_processfunc*              abort_process;
  IFWD_DL_force_target_resetfunc*         force_target_reset;
  IFWD_DL_end_of_processfunc*             end_of_process;
  IFWD_DL_start_AT_modefunc*              start_AT_mode;
  IFWD_DL_stop_AT_modefunc*               stop_AT_mode;
  IFWD_DL_AT_sendfunc*                    AT_send;
  IFWD_DL_set_external_file_handlingfunc* set_external_file_handling;

  IFWD_DL_get_nof_progressbarsfunc*       get_nof_progressbars;
  IFWD_DL_TOC_get_nof_itemsfunc*          TOC_get_nof_items;
  IFWD_DL_TOC_get_memory_classfunc*       TOC_get_memory_class;
  IFWD_DL_TOC_get_hw_platformfunc*        TOC_get_hw_platform;
  IFWD_DL_TOC_get_sw_optionfunc*          TOC_get_sw_option;
  IFWD_DL_get_sys_versionfunc*            get_sys_version;
  IFWD_DL_TOC_get_filenamefunc*           TOC_get_filename;
  IFWD_DL_TOC_get_checksumfunc*           TOC_get_checksum;
  IFWD_DL_TOC_get_filesizefunc*           TOC_get_filesize;
  IFWD_DL_TOC_get_flash_technologyfunc*   TOC_get_flash_technology;

  IFWD_DL_NVM_get_nof_user_optionsfunc*   NVM_get_nof_user_options;
  IFWD_DL_NVM_get_user_optionfunc*        NVM_get_user_option;
  IFWD_DL_NVM_set_user_optionfunc*        NVM_set_user_option;
  IFWD_DL_NVM_set_flashless_optionfunc*   NVM_set_flashless_option;

  IFWD_DL_trace_insertfunc*               trace_insert;
  IFWD_DL_out_of_session_commfunc*        out_of_session_comm;
  IFWD_get_next_free_channelfunc*         get_next_free_channel;

  IFWD_device_mapfunc*                    device_map;
  IFWD_device_map_savefunc*               device_map_save;
  IFWD_device_map_loadfunc*               device_map_load;

  IFWD_DL_secure_channel_setupfunc*           secure_channel_setup;
};

#endif /* LIBRARYLOADER_H_ */

