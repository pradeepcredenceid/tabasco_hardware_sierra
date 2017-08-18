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
      $File name:  /msw_tools/FlashTool/libDownloadTool/src/DownloadTool.h $
      $CC-Version: .../oint_tools_flashtoole2_01/13 $
      $Date:       2014-03-13    15:43:13 UTC $
    -------------------------------------------------------------------------
    Description:
      Dynamic interface for loading the IFWD_DownloadDll.dll
      Include this header and run "LoadDownloadDll()" to
      dynamically import IFWD_DownloadDll.dll
     ------------------------------------------------------------------------- */

#ifndef DOWNLOAD_LIBRARY_LOADER_H
#define DOWNLOAD_LIBRARY_LOADER_H

#include "IMC_DownloadDll.h"

/*---------------------------------------------*/
/* Cross-platform Compatability                */
/*---------------------------------------------*/
#ifndef _WIN32
  #include <dlfcn.h>

  #define HINSTANCE void*
  #define LoadLibrary( file )                   dlopen(file, RTLD_LAZY)
  #define FreeLibrary( handle )                 dlclose( handle )
  #define GetProcAddress( handle, func )        dlsym( handle, func )
  /* #define GetLastError()                        dlerror() */
#else
  #include <windows.h>

  #define EXPORT __declspec(dllimport) __cdecl
#endif

#define IFWD_RET   IFWD_DL_status_enum

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
/* Download Library typedefs                   */
/*---------------------------------------------*/

//typedef int     (*DllEntryPointfunc*)(HINSTANCE, unsigned long, void*);
typedef char*    (IFWD_DL_get_versionfunc)(void);
typedef void     (IFWD_set_module_verbosefunc)(MODULE_enum, int);
typedef IFWD_RET (IFWD_DL_get_sw_version_datafunc)(uint8_t,char *,uint8_t *,uint8_t *,uint16_t,char *);
typedef IFWD_RET (IFWD_DL_set_parameterfunc)(uint8_t, uint32_t, char *);
typedef IFWD_RET (IFWD_DL_get_parameterfunc)(uint8_t, uint32_t *, char *);
typedef IFWD_RET (IFWD_DL_init_callbackfunc)(void (*HostCallBack)(uint8_t, IFWD_DL_status_enum, char *), char *);
typedef IFWD_RET (IFWD_DL_open_comm_portfunc)(uint8_t, char *, char *, uint32_t, char *);
typedef IFWD_RET (IFWD_DL_setup_prolific_gpio_resetfunc)(uint8_t, char *, uint32_t, char *);
typedef IFWD_RET (IFWD_DL_close_comm_portfunc)(uint8_t, char *);
typedef IFWD_RET (IFWD_DL_boot_targetfunc)(uint8_t, char *, IFWD_DL_modem_control_signals_type *, char *);
typedef IFWD_RET (IFWD_DL_boot_bin_targetfunc)(uint8_t, uint8_t *, IFWD_DL_modem_control_signals_type *, char *);
typedef IFWD_RET (IFWD_DL_set_get_control_signalfunc)(uint8_t, IFWD_DL_modem_control_signals_type *, char *);
typedef IFWD_RET (IFWD_DL_download_fls_filefunc)(uint8_t, char *, char *);
typedef IFWD_RET (IFWD_DL_download_eep_filefunc)(uint8_t, char *,  uint8_t, char *);
typedef IFWD_RET (IFWD_DL_download_dffs_filefunc)(uint8_t, char *, uint8_t, char *);
typedef IFWD_RET (IFWD_DL_download_cust_filefunc)(uint8_t, char *, char *);
typedef IFWD_RET (IFWD_DL_upload_eep_filefunc)(uint8_t, char *, char *, char *);
typedef IFWD_RET (IFWD_DL_update_static_eepromfunc)(uint8_t, char *, char *, char *, char *);
typedef IFWD_RET (IFWD_DL_upload_bin_imagefunc)(uint8_t, uint32_t, uint32_t, char *, char *, char *);
typedef IFWD_RET (IFWD_DL_download_bin_imagefunc)(uint8_t, uint32_t, uint32_t, uint32_t, char *, char *, char *);
typedef IFWD_RET (IFWD_DL_erase_imagefunc)(uint8_t, uint32_t, uint32_t,char *, char *);
typedef IFWD_RET (IFWD_DL_verify_flash_imagefunc)(uint8_t, uint32_t, uint32_t, char *, char *);
typedef IFWD_RET (IFWD_DL_abort_processfunc)(uint8_t, char *);
typedef IFWD_RET (IFWD_DL_force_target_resetfunc)(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, char *);
typedef IFWD_RET (IFWD_DL_end_of_processfunc)(uint8_t, char *);
typedef IFWD_RET (IFWD_DL_start_AT_modefunc)(uint8_t, char *);
typedef IFWD_RET (IFWD_DL_stop_AT_modefunc)(uint8_t, char *);
typedef IFWD_RET (IFWD_DL_AT_sendfunc)(uint8_t, char *, uint16_t, char *);
typedef void     (IFWD_DL_set_external_file_handlingfunc)(IFWD_ExtFileHandleType *);
typedef IFWD_RET (IFWD_DL_TOC_get_nof_itemsfunc)(char *, uint32_t *);
typedef IFWD_RET (IFWD_DL_TOC_get_memory_classfunc)(char *, uint32_t, uint32_t *);
typedef IFWD_RET (IFWD_DL_TOC_get_hw_platformfunc)(char *filename, uint32_t *hw_platform_output);
typedef IFWD_RET (IFWD_DL_TOC_get_sw_optionfunc)(char *, uint32_t, uint32_t, uint32_t *);
typedef IFWD_RET (IFWD_DL_get_sys_versionfunc)(char *, uint32_t, char *, uint32_t);
typedef IFWD_RET (IFWD_DL_TOC_get_filenamefunc)(char *, uint32_t, char *);
typedef IFWD_RET (IFWD_DL_TOC_get_checksumfunc)(char *, uint32_t, uint32_t *);
typedef IFWD_RET (IFWD_DL_TOC_get_filesizefunc)(char *, uint32_t, uint32_t *);
typedef IFWD_RET (IFWD_DL_TOC_get_flash_technologyfunc)(char *, uint32_t *);
typedef uint32_t (IFWD_DL_get_nof_progressbarsfunc)(uint8_t, char *, uint32_t *);
typedef IFWD_RET (IFWD_DL_NVM_get_nof_user_optionsfunc)(char *, uint32_t *);
typedef IFWD_RET (IFWD_DL_NVM_get_user_optionfunc)(char *, uint32_t, uint32_t *);
typedef IFWD_RET (IFWD_DL_NVM_set_user_optionfunc)(char *,uint32_t, uint8_t);
typedef IFWD_RET (IFWD_DL_NVM_set_flashless_optionfunc)(uint8_t *, uint32_t);
typedef uint8_t  (IFWD_DL_trace_insertfunc)(uint8_t, uint8_t *, uint32_t, uint8_t);
typedef IFWD_RET (IFWD_DL_out_of_session_commfunc)(uint8_t, uint32_t, uint32_t, uint32_t, uint32_t, uint8_t *);
typedef uint8_t  (IFWD_get_next_free_channelfunc)();
typedef uint8_t  (IFWD_get_comm_handlefunc)(uint8_t id);

typedef char*    (IFWD_device_mapfunc)(uint8_t id);
typedef uint8_t  (IFWD_device_map_savefunc)(const char*);
typedef uint8_t  (IFWD_device_map_loadfunc)(const char*);

typedef IFWD_RET (IFWD_DL_secure_channel_setupfunc)(uint8_t, uint32_t, sec_ch_callback_type, char**, char*);

//typedef uint8_t (Test_Setupfunc)(void*);
//typedef void (switch_portfunc)(uint8_t, char *, uint32_t);


/*---------------------------------------------*/
/* Function Pointer Structure                  */
/*---------------------------------------------*/
typedef struct {
  //DllEntryPointfunc* DllEntryPoint;
  //Test_Setupfunc* Test_Setup;
  //switch_portfunc* DL_switch_port;
  IFWD_DL_get_versionfunc*                get_version;
  IFWD_DL_get_sw_version_datafunc*        get_sw_version_data;
  IFWD_DL_set_parameterfunc*              set_parameter;
  IFWD_DL_get_parameterfunc*              get_parameter;
  IFWD_DL_init_callbackfunc*              init_callback;

  IFWD_DL_open_comm_portfunc*             open_comm_port;
  IFWD_DL_close_comm_portfunc*            close_comm_port;
  IFWD_DL_boot_targetfunc*                boot_target;
  IFWD_DL_boot_bin_targetfunc*            boot_bin_target;
  IFWD_DL_set_get_control_signalfunc*     set_get_control_signal;

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
  IFWD_get_comm_handlefunc*               get_comm_handle;

  IFWD_device_mapfunc*                    device_map;
  IFWD_device_map_savefunc*               device_map_save;
  IFWD_device_map_loadfunc*               device_map_load;

  IFWD_DL_secure_channel_setupfunc*       secure_channel_setup;
} DownloadLibraryStruct;


/**
 * Insert this code for loading the library
 *
 *  Alternately you can use the LibraryLoader c++ class
 *  Contact the author for more information
 */
#if 0

#ifdef __cplusplus
extern "C"
#endif
DownloadLibraryStruct* LoadDownloadLibrary(const char* library)
{
   /*LoadLibrary*/
   HINSTANCE hdll = LoadLibrary (library );
   if (!hdll) { fprintf(stderr, "Error loading download library\n"); return NULL; }

   DownloadLibraryStruct* DownloadLibrary = (DownloadLibraryStruct*)malloc(sizeof(DownloadLibraryStruct));

   /*GetProcAddress*/
   //DownloadLibrary->DllEntryPoint            = (DllEntryPointfunc*)GetProcAddress(hdll, "DllEntryPoint");
   //DownloadLibrary->Test_Setup               = (Test_Setupfunc*)GetProcAddress(hdll, "Test_Setup");
   //DownloadLibrary->switch_port              = (switch_portfunc*)GetProcAddress(hdll, "switch_port");
   DownloadLibrary->get_version                = (IFWD_DL_get_versionfunc*)GetProcAddress(hdll, "IFWD_DL_get_dll_version");
   DownloadLibrary->get_sw_version_data        = (IFWD_DL_get_sw_version_datafunc*)GetProcAddress(hdll, "IFWD_DL_get_sw_version_data");
   DownloadLibrary->set_parameter              = (IFWD_DL_set_parameterfunc*)GetProcAddress(hdll, "IFWD_DL_set_dll_parameter");
   DownloadLibrary->get_parameter              = (IFWD_DL_get_parameterfunc*)GetProcAddress(hdll, "IFWD_DL_dll_get_parameter");
   DownloadLibrary->init_callback              = (IFWD_DL_init_callbackfunc*)GetProcAddress(hdll, "IFWD_DL_init_callback");
   DownloadLibrary->open_comm_port             = (IFWD_DL_open_comm_portfunc*)GetProcAddress(hdll, "IFWD_DL_open_comm_port");
   DownloadLibrary->close_comm_port            = (IFWD_DL_close_comm_portfunc*)GetProcAddress(hdll, "IFWD_DL_close_comm_port");
   DownloadLibrary->boot_target                = (IFWD_DL_boot_targetfunc*)GetProcAddress(hdll, "IFWD_DL_boot_target");
   DownloadLibrary->boot_bin_target            = (IFWD_DL_boot_bin_targetfunc*)GetProcAddress(hdll, "IFWD_DL_boot_bin_target");
   DownloadLibrary->set_get_control_signal     = (IFWD_DL_set_get_control_signalfunc*)GetProcAddress(hdll, "IFWD_DL_set_get_control_signal");
   DownloadLibrary->download_fls_file          = (IFWD_DL_download_fls_filefunc*)GetProcAddress(hdll, "IFWD_DL_download_fls_file");
   DownloadLibrary->download_eep_file          = (IFWD_DL_download_eep_filefunc*)GetProcAddress(hdll, "IFWD_DL_download_eep_file");
   DownloadLibrary->download_dffs_file         = (IFWD_DL_download_dffs_filefunc*)GetProcAddress(hdll, "IFWD_DL_download_dffs_file");
   DownloadLibrary->download_cust_file         = (IFWD_DL_download_cust_filefunc*)GetProcAddress(hdll, "IFWD_DL_download_cust_file");
   DownloadLibrary->upload_eep_file            = (IFWD_DL_upload_eep_filefunc*)GetProcAddress(hdll, "IFWD_DL_upload_eep_file");
   DownloadLibrary->update_static_eeprom       = (IFWD_DL_update_static_eepromfunc*)GetProcAddress(hdll, "IFWD_DL_update_static_eeprom");
   DownloadLibrary->upload_bin_image           = (IFWD_DL_upload_bin_imagefunc*)GetProcAddress(hdll, "IFWD_DL_upload_bin_image");
   DownloadLibrary->download_bin_image         = (IFWD_DL_download_bin_imagefunc*)GetProcAddress(hdll, "IFWD_DL_download_bin_image");
   DownloadLibrary->erase_image                = (IFWD_DL_erase_imagefunc*)GetProcAddress(hdll, "IFWD_DL_erase_image");
   DownloadLibrary->verify_flash_image         = (IFWD_DL_verify_flash_imagefunc*)GetProcAddress(hdll, "IFWD_DL_verify_flash_image");
   DownloadLibrary->abort_process              = (IFWD_DL_abort_processfunc*)GetProcAddress(hdll, "IFWD_DL_abort_process");
   DownloadLibrary->force_target_reset         = (IFWD_DL_force_target_resetfunc*)GetProcAddress(hdll, "IFWD_DL_force_target_reset");
   DownloadLibrary->end_of_process             = (IFWD_DL_end_of_processfunc*)GetProcAddress(hdll, "IFWD_DL_end_of_process");
   DownloadLibrary->start_AT_mode              = (IFWD_DL_start_AT_modefunc*)GetProcAddress(hdll, "IFWD_DL_start_AT_mode");
   DownloadLibrary->stop_AT_mode               = (IFWD_DL_stop_AT_modefunc*)GetProcAddress(hdll, "IFWD_DL_stop_AT_mode");
   DownloadLibrary->AT_send                    = (IFWD_DL_AT_sendfunc*)GetProcAddress(hdll, "IFWD_DL_AT_send");
   DownloadLibrary->set_external_file_handling = (IFWD_DL_set_external_file_handlingfunc*)GetProcAddress(hdll, "IFWD_DL_set_external_file_handling");
   DownloadLibrary->TOC_get_nof_items          = (IFWD_DL_TOC_get_nof_itemsfunc*)GetProcAddress(hdll, "IFWD_DL_TOC_get_nof_items");
   DownloadLibrary->TOC_get_memory_class       = (IFWD_DL_TOC_get_memory_classfunc*)GetProcAddress(hdll, "IFWD_DL_TOC_get_memory_class");
   DownloadLibrary->TOC_get_sw_option          = (IFWD_DL_TOC_get_sw_optionfunc*)GetProcAddress(hdll, "IFWD_DL_TOC_get_sw_option");
   DownloadLibrary->get_sys_version            = (IFWD_DL_get_sys_versionfunc*)GetProcAddress(hdll, "IFWD_DL_get_sys_version");

   DownloadLibrary->TOC_get_filename           = (IFWD_DL_TOC_get_filenamefunc*)GetProcAddress(hdll, "IFWD_DL_TOC_get_filename");
   DownloadLibrary->TOC_get_checksum           = (IFWD_DL_TOC_get_checksumfunc*)GetProcAddress(hdll, "IFWD_DL_TOC_get_checksum");
   DownloadLibrary->TOC_get_filesize           = (IFWD_DL_TOC_get_filesizefunc*)GetProcAddress(hdll, "IFWD_DL_TOC_get_filesize");
   DownloadLibrary->get_nof_progressbars       = (IFWD_DL_get_nof_progressbarsfunc*)GetProcAddress(hdll, "IFWD_DL_dll_get_nof_progressbars");
   DownloadLibrary->NVM_get_nof_user_options   = (IFWD_DL_NVM_get_nof_user_optionsfunc*)GetProcAddress(hdll, "IFWD_DL_NVM_get_nof_user_options");
   DownloadLibrary->NVM_get_user_option        = (IFWD_DL_NVM_get_user_optionfunc*)GetProcAddress(hdll, "IFWD_DL_NVM_get_user_option");
   DownloadLibrary->NVM_set_user_option        = (IFWD_DL_NVM_set_user_optionfunc*)GetProcAddress(hdll, "IFWD_DL_NVM_set_user_option");
   DownloadLibrary->trace_insert               = (IFWD_DL_trace_insertfunc*)GetProcAddress(hdll, "IFWD_DL_trace_insert");
   DownloadLibrary->out_of_session_comm        = (IFWD_DL_out_of_session_commfunc*)GetProcAddress(hdll, "IFWD_DL_out_of_session_comm");
   DownloadLibrary->get_comm_handle            = (IFWD_get_comm_handlefunc*)GetProcAddress(hdll, "IFWD_get_comm_handle");

   DownloadLibrary->device_map                 = (IFWD_map_devicefunc*)GetProcAddress(hdll, "IFWD_device_map");
   DownloadLibrary->device_map_save            = (IFWD_map_devicefunc*)GetProcAddress(hdll, "IFWD_device_map_save");
   DownloadLibrary->device_map_load            = (IFWD_map_devicefunc*)GetProcAddress(hdll, "IFWD_device_map_load");
   DownloadLibrary->secure_channel_setup       = (IFWD_DL_secure_channel_setupfunc*)GetProcAddress(hdll, "IFWD_DL_secure_channel_setup");

   return DownloadLibrary;
}
#endif
#endif

