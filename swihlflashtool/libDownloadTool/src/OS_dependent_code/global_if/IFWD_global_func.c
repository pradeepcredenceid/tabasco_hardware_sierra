/*
 *       Copyright (C) 2011-2014 Intel Mobile Communications GmbH
 *
 *            Sec Class: Intel Confidential (IC)
 *
 *
 * This document contains proprietary information belonging to IMC.
 * Denmark A/S. Passing on and copying of this document, use and communication
 * of its contents is not permitted without prior written authorisation.
 *
 * Description:
 *   IFWD Download stack global functions - code
 *
 * Revision Information:
 *  $File name:  /msw_tools/FlashTool/libDownloadTool/src/OS_dependent_code/global_if/IFWD_global_func.c $
 *  $CC-Version: .../oint_tools_flashtoole2_01/25 $
 *  $Date:       2014-03-13    15:42:39 UTC $
 *   Comment:
 *     Added support for concatinating "Internal Name" in project version information to the DLL version string
 */

#define THIS_MODULE MODULE_NONE

/*---------------------------------------------*/
/* Include files.                              */
/*---------------------------------------------*/
#include <stdio.h>     /* C standard lib: Containing printf scanf etc. */
#include <stdlib.h>
#include <stdarg.h>    /* C standard lib: Containing functions for handling variable number of arguments */
#include <string.h>    /* C standard lib: Containing strcpy memcpy etc. */
#include <time.h>      /* used for clock() */

#ifdef _WIN32
  #include <windows.h>
  #include <shlobj.h>
  #include <winbase.h>
#else
  #include <sys/utsname.h>
#endif

#include "OS.h"
#include "DebugModule.h"
#include "IFWD_text_def.h"
#include "IFWD_sec_channel.h"
#include "IFWD_global_func.h"
#include "IFWD_protocol_handler.h"

long (*GLOBAL_clock)(void) = clock;

/*---------------------------------------------*/
/* typedef                                     */
/*---------------------------------------------*/


/*---------------------------------------------*/
/* globale variabler                           */
/*---------------------------------------------*/
char GLOBAL_text[MAX_TEXT_LENGTH];

HANDLE done_event;
HANDLE idle_event;

uint8_t  GLOBAL_SFH_in_idle              = true; /* SFH is idle at design time */
uint8_t  GLOBAL_MFH_in_idle              = true; /* MFH is idle at design time */
/* SWISTART */
#ifndef SIERRA
uint32_t GLOBAL_erase_mode               = IFWD_DL_target_erase_before_write;
#endif/* SIERRA */
/* SWISTOP */
uint32_t GLOBAL_dll_instance_number      = GLOBAL_INSTANCE_NOT_ASSIGNED;
uint8_t  GLOBAL_trace_verbosity          = 0;
char*    GLOBAL_trace_filename           = NULL;
uint8_t *GLOBAL_trace_buffer             = NULL;
uint8_t  GLOBAL_force_plain_upload       = false;
uint8_t  GLOBAL_usb_auto_mode            = false;
uint8_t  GLOBAL_skip_wr_pack_crc         = false;
uint8_t  GLOBAL_check_sign_hw_cfg_value  = true;
uint8_t  GLOBAL_ban_eep_mode_writes      = false;
uint8_t  GLOBAL_UseCOMPortEmulation      = false;
uint8_t *GLOBAL_NVM_flashless_path       = NULL;
uint32_t GLOBAL_NVM_flashless_options    = 0;
uint32_t GLOBAL_use_vp_tx_chunk_delay    = 0;  /* Default time in ms */
uint32_t GLOBAL_use_vp_tx_chunk_size     = 512; /* Default chunk size */
uint8_t  GLOBAL_use_hw_ch_change         = false;
uint8_t  GLOBAL_use_alt_boot_speed       = false;
uint8_t  GLOBAL_dut_in_test_mode         = false;
GLOBAL_platform_group_type GLOBAL_platform_group   = GLOBAL_platform_group_unknown;
GLOBAL_dut_test_group_type GLOBAL_dut_test_request = GLOBAL_dut_test_unknown;
EmulateTx_t GLOBAL_EmulateTx           = 0;
EmulateRx_t GLOBAL_EmulateRx           = 0;
EmulateRxAvail_t GLOBAL_EmulateRxAvail = 0;
T_MAC_PARTITION_CONFIG GLOBAL_nand_partition_config;
sec_ch_callback_type secure_channel_cb_fx = NULL;


uint8_t GLOBAL_hsi_link = false;
uint8_t GLOBAL_skip_sw_load_if_sys_val_ok = false;
uint8_t GLOBAL_is_in_time_critical_section = false;
uint8_t GLOBAL_force_full_emulation = false;
uint32_t  GLOBAL_special_boot_seq = IFWD_DL_boot_seq_normal;


/*---------------------------------------------*/
/* Local defines.                              */
/*---------------------------------------------*/


/*---------------------------------------------*/
/* Local const                                 */
/*---------------------------------------------*/


/*---------------------------------------------*/
/* local variables   .                         */
/*---------------------------------------------*/
uint8_t  GLOBAL_use_plain_text        = true;  /* Flag to determine if texts are send out as plain text or coded. */
uint32_t GLOBAL_boot_timeout          = DEFAULT_GLOBAL_BOOT_TIMEOUT;
uint32_t GLOBAL_comm_timeout          = DEFAULT_GLOBAL_COMM_TIMEOUT;  /* Call "TRA_GetCommTimeOut()" to get the current timeout value for a given channel */
uint32_t GLOBAL_comm_process_timeout  = DEFAULT_COMM_PROCESS_TIMEOUT; /* TODO: Remove this timeout, as it is redundant */
uint8_t  GLOBAL_use_pre_erase         = false;
uint8_t  GLOBAL_cond_dyn_eep_erase    = false;
uint8_t  GLOBAL_usb_device            = false;
uint8_t  GLOBAL_use_GenericUSB        = false;
uint8_t  GLOBAL_precheck_mode         = false;
uint8_t  GLOBAL_force_area_erase      = false;
uint8_t  GLOBAL_use_sec_pack_erase_flag = true; /* Feature enabled by default */
uint8_t  GLOBAL_upload_mbn_data       = false;
uint32_t GLOBAL_use_pack_size[MAX_NUMBER_OF_CHANNELS+1]   = {DEFAULT_PACK_SIZE   ,DEFAULT_PACK_SIZE   ,DEFAULT_PACK_SIZE   ,DEFAULT_PACK_SIZE   ,DEFAULT_PACK_SIZE   ,DEFAULT_PACK_SIZE   ,DEFAULT_PACK_SIZE   ,DEFAULT_PACK_SIZE   ,DEFAULT_PACK_SIZE   ,DEFAULT_PACK_SIZE   ,DEFAULT_PACK_SIZE   ,DEFAULT_PACK_SIZE   ,DEFAULT_PACK_SIZE   ,DEFAULT_PACK_SIZE   ,DEFAULT_PACK_SIZE   ,DEFAULT_PACK_SIZE   ,DEFAULT_PACK_SIZE   ,DEFAULT_PACK_SIZE   ,DEFAULT_PACK_SIZE   ,DEFAULT_PACK_SIZE   ,DEFAULT_PACK_SIZE   ,DEFAULT_PACK_SIZE   ,DEFAULT_PACK_SIZE   ,DEFAULT_PACK_SIZE   ,DEFAULT_PACK_SIZE   ,DEFAULT_PACK_SIZE   ,DEFAULT_PACK_SIZE   ,DEFAULT_PACK_SIZE   ,DEFAULT_PACK_SIZE   ,DEFAULT_PACK_SIZE   ,DEFAULT_PACK_SIZE   ,DEFAULT_PACK_SIZE   ,DEFAULT_PACK_SIZE   };
uint32_t GLOBAL_comm_technology[MAX_NUMBER_OF_CHANNELS+1] = {COMM_TECH_ASYNC_COM,COMM_TECH_ASYNC_COM,COMM_TECH_ASYNC_COM,COMM_TECH_ASYNC_COM,COMM_TECH_ASYNC_COM,COMM_TECH_ASYNC_COM,COMM_TECH_ASYNC_COM,COMM_TECH_ASYNC_COM,COMM_TECH_ASYNC_COM,COMM_TECH_ASYNC_COM,COMM_TECH_ASYNC_COM,COMM_TECH_ASYNC_COM,COMM_TECH_ASYNC_COM,COMM_TECH_ASYNC_COM,COMM_TECH_ASYNC_COM,COMM_TECH_ASYNC_COM,COMM_TECH_ASYNC_COM,COMM_TECH_ASYNC_COM,COMM_TECH_ASYNC_COM,COMM_TECH_ASYNC_COM,COMM_TECH_ASYNC_COM,COMM_TECH_ASYNC_COM,COMM_TECH_ASYNC_COM,COMM_TECH_ASYNC_COM,COMM_TECH_ASYNC_COM,COMM_TECH_ASYNC_COM,COMM_TECH_ASYNC_COM,COMM_TECH_ASYNC_COM,COMM_TECH_ASYNC_COM,COMM_TECH_ASYNC_COM,COMM_TECH_ASYNC_COM,COMM_TECH_ASYNC_COM,COMM_TECH_ASYNC_COM};
uint32_t GLOBAL_comm_align_bytes[MAX_NUMBER_OF_CHANNELS+1] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
uint32_t GLOBAL_pre_boot[MAX_NUMBER_OF_CHANNELS+1] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uint8_t  GLOBAL_use_sec_ch_ext_cb[MAX_NUMBER_OF_CHANNELS+1] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
char     GLOBAL_comm_port_name[MAX_NUMBER_OF_CHANNELS+1][128] = {"","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","",""};
char     GLOBAL_ctrl_port_name[MAX_NUMBER_OF_CHANNELS+1][128] = {"","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","",""};

/*---------------------------------------------*/
/* local functions.                            */
/*---------------------------------------------*/


/****************************************************************************************
* Function:... GLOBAL_callback_stub
* Parameters:. channel     In  Number of the logical channel (1..N)
*              status      In The status code to be sent to the host.
*              status_text In The status text to be sent to the host.
* Returns:.... -
* Description: The default call back function if no fx-pointer is received from a host.
* Created:.... DD.MM.YY by (Full name / initials)
* Modified:... DD.MM.YY by (Full name / initials)
*                Modifications note.
****************************************************************************************/

void GLOBAL_callback_stub (uint8_t channel, IFWD_DL_status_enum status, char *status_text)
{
  PARAM_NOT_USED(channel);
  PARAM_NOT_USED(status);
  PARAM_NOT_USED(status_text);
}

/****************************************************************************************
* Function:... GLOBAL_callback
* Parameters:. channel     In  Number of the logical channel (1..N)
*              status      In The status code to be sent to the host.
*              status_text In The status text to be sent to the host.
* Returns:.... -
* Description: A wrapper for the DLL call back function that can be used if the host functions
*              are handled locally.
* Created:.... DD.MM.YY by (Full name / initials)
* Modified:... DD.MM.YY by (Full name / initials)
*                Modifications note.
****************************************************************************************/
void GLOBAL_callback(uint8_t channel, IFWD_DL_status_enum status, char *status_text)
{
  if(callback)
  {
    callback(channel, status, status_text);

    TR_BEGIN(TR_Callback,status,channel);
    TR_STR(status_text);
    TR_END;
  }
}

/****************************************************************************************
* Function:... callback function pointer;
* Parameters:. channel     In  Number of the logical channel (1..N)
*              status      In The status code to be sent to the host.
*              status_text In The status text to be sent to the host.
* Returns:.... -
* Description: A wrapper for the DLL call back function that can be used if the host functions
*              are handled locally.
* Created:.... DD.MM.YY by (Full name / initials)
* Modified:... DD.MM.YY by (Full name / initials)
*                Modifications note.
****************************************************************************************/
void (*callback)(uint8_t channel, IFWD_DL_status_enum status, char *status_text); /* Pointer to the CallBack function */




void get_os_api_error_text(uint16_t error_code)
{
#ifdef _WIN32
  if (!FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        (DWORD) error_code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), /* Default language */
        (LPTSTR) GLOBAL_text,
        MAX_TEXT_LENGTH,
        NULL ))
#endif
  {
    /* Handle the error. */
    snprintf(GLOBAL_text, MAX_TEXT_LENGTH,"Unknown OS API error code: %d", error_code);
  }

}
/*-------------------------------------------------------------------------------------*/

void GLOBAL_signal_not_in_idle(void)
{
  GLOBAL_SFH_in_idle = false;
  GLOBAL_MFH_in_idle = false;
  SetEvent(idle_event); /* make sure we get the main loop out of the wait */
}
/*-------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------
 *   char *GLOBAL_TOC_filename(char *filename, uint32_t *TOC_index_output)
 *
 *   Extracts the real filename and TOC number from a string
 *
 *   INPUT PARAMATERS:
 *     filename: the filename. e.g. "|0|c:\MPE_download.fls" or "c:\sw.fls"
 *     TOC_index_output: pointer to place the TOC number at. Extracted for "filename" (-1 if it is an ordinary file)
 *                       If this is not wanted a NULL pointer is passed
 *   OUTPUT:
 *     returns the input filename stripped of any TOC characters, e.g.
 *       "|0|c:\MPE_download.fls" => "c:\MPE_download.fls"
 *       "c:\MPE_download.fls" => "c:\MPE_download.fls"
 -------------------------------------------------------------------------------------*/
char *GLOBAL_TOC_filename(char *filename, int32_t *TOC_index_output)
{
  if(*filename == (char)'|')
  {
    int toc_index = atoi(filename + 1);
    filename = strchr(filename + 1,'|'); /* find end of number */
    if(filename)
    {
      if(TOC_index_output)
        *TOC_index_output = toc_index;
      return filename + 1; /* skip the '|' and return real filename */
    }
  }
  /* Normal filename: */
  if(TOC_index_output)
    *TOC_index_output = -1; /* mark as normal filename */
  return filename;
}
/*---------------------------------------------------------------------------*/

// This code is only used in WIN32... But if it where to be, then use the STL version
// STL: class string { string& append ( const string& str ); }
#ifdef _WIN32
void GLOBAL_append_file_parts(char *base, char *remaining_part)
{
  if(base)
  {
    int len = strlen(base);
    if(len > 0 && !ISPATHDELIMITER(base[len-1])) // add trailing backslash if needed
    {
      base[len] = '\\';
      base[len+1] = 0;
      len++;
    }
    if(remaining_part)
    {
      if(ISPATHDELIMITER(*remaining_part)) // already path delimiter?
        remaining_part++;                                   // then skip it, since we have added it
      strcpy(&base[len],remaining_part);
    }
  }
}
#endif

//---------------------------------------------------------------------------


/**
 *  The following are OS specific or retreival of information
 *  stored inside a DLL file.
 *  This does not reside inside Lib files.
 *
 *   TODO: I don't know, but I suspect that MAC are behave same as Linux....
 **/
#ifndef _WIN32
//---------------------------------------------------------------------------
void ShowMessage(char *text)
{
  if (text)
    puts(text);
}
/*---------------------------------------------------------------------------*/
uint8_t GLOBAL_GetFileVersion(char *File, uint16_t *Major, uint16_t *Minor, uint16_t *Release, uint16_t *Build)
{
  return false;
}

//---------------------------------------------------------------------------
uint8_t sprintf_version_info(char *dest, const char *lpszModuleName, const char *desc)
{
  return false;
}
//---------------------------------------------------------------------------
void GLOBAL_get_special_folder(int folder, char *dest)
{
}
//---------------------------------------------------------------------------
uint8_t GetSystem32FileVersion(char *driver_file, char *dest)
{
  return false;
}
//---------------------------------------------------------------------------
void GetOSVersion(char *dest)
{
  char ch;
  FILE* fp;
  struct utsname name;

  if(!dest) return;
  *dest = 0;

  if (uname(&name) == 0)
  {
    #ifndef _UTSNAME_LENGTH
      #define _UTSNAME_LENGTH 65
    #endif
    snprintf(dest, _UTSNAME_LENGTH*2+1,"%s %s", name.sysname, name.release);
  }
  else if ((fp = fopen("/proc/version", "r")))
  {
    while(fread(&ch, sizeof(ch), 1, fp))
      *dest ++ = ch;
    *dest = 0;
    fclose(fp);
  }
  else
  {
    snprintf(dest, 12, "OS unknown");
    perror("cannot get system name");
  }
}
//---------------------------------------------------------------------------
#else  // WIN32
//---------------------------------------------------------------------------
void ShowMessage(char *text)
{
  if (text)
    MessageBox(0, text, "Download DLL", MB_OK);
}
/*---------------------------------------------------------------------------*/

uint8_t GLOBAL_GetFileVersion(char *File, uint16_t *Major, uint16_t *Minor, uint16_t *Release, uint16_t *Build)
{
  DWORD i = GetFileVersionInfoSize(File, 0);
  VS_FIXEDFILEINFO *pval;
  UINT valsize;
  PVOID pbuf;
  if(!i)
  {
    return false;
  }
   pbuf = malloc(i);
  if (!GetFileVersionInfo(File, 0, i, pbuf))
  {
    free(pbuf);
    return false;
  }
  if (!VerQueryValue(pbuf, "\\", (PVOID*)&pval, &valsize))
  {
    free(pbuf);
    return false;
  }
  if(Major)
    *Major = pval->dwFileVersionMS >> 16;
  if(Minor)
    *Minor = pval->dwFileVersionMS & 0xFFFF;
  if(Release)
    *Release = pval->dwFileVersionLS >> 16;
  if(Build)
    *Build = pval->dwFileVersionLS & 0xFFFF;
  free(pbuf);
  return true;
}
//---------------------------------------------------------------------------
uint8_t sprintf_version_info(char *dest, const char *lpszModuleName, const char *desc)
{
  uint8_t result = false;
  DWORD Handle, DataSize;
  BYTE *pVersionData = 0;

  // Get the version information size:
  DataSize = GetFileVersionInfoSize((LPTSTR)lpszModuleName, &Handle);
  if(DataSize)
  {
    pVersionData = MEM_malloc(DataSize); // Allocate buffer and get version information
    if(GetFileVersionInfo((LPTSTR)lpszModuleName, Handle, DataSize,(void**)pVersionData))
    {
      UINT nQuerySize;
      DWORD *pTransTable;
      // Get language identifier:
      if(VerQueryValue(pVersionData, "\\VarFileInfo\\Translation",(void **)&pTransTable, &nQuerySize))
      {
        DWORD dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));
        LPVOID lpData;
        char strBlockName[500];
        sprintf(strBlockName,"\\StringFileInfo\\%08lx\\%s", dwLangCharset, desc);
        if(VerQueryValue((void **)pVersionData, strBlockName, &lpData, &nQuerySize))
        {
          if(dest)
            strcpy(dest,(char*)lpData);
          result = true;
        }
      }
    }
  }
  if(pVersionData)
    MEM_free(pVersionData);
  if(!result && dest)
    *dest = 0;
  return result;
}
//---------------------------------------------------------------------------

void GLOBAL_get_special_folder(int folder, char *dest)
{
  if(dest)
  {
    *dest = 0;
    if(folder >= 0)
    {
      LPITEMIDLIST pidl;
      if(SHGetSpecialFolderLocation(0,folder,&pidl) == NOERROR)
      {
        SHGetPathFromIDList(pidl,dest);
      }
    }
  }
}
//---------------------------------------------------------------------------
void GetOSVersion(char *dest)
{
  if(dest)
  {
    OSVERSIONINFO osvi;
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&osvi);
    *dest = 0;

    switch(osvi.dwPlatformId)
    {
      case 1: // win95/98/ME?
        if(osvi.dwMajorVersion == 4)
        {
          switch(osvi.dwMinorVersion)
          {
            case  0: strcpy(dest,"Win95"); break;
            case 10: strcpy(dest,"Win98"); break;
            case 90: strcpy(dest,"WinME"); break;
          }
        }
      break;
      case 2:  // NT/2K/XP/Vista
        switch(osvi.dwMajorVersion)
        {
          case 4:
            if(osvi.dwMinorVersion == 0)
              strcpy(dest,"WinNT");
          break;
          case 5:
            switch(osvi.dwMinorVersion)
            {
              case 0: strcpy(dest,"Win2000"); break;
              case 1: strcpy(dest,"WinXP"); break;
              case 2: strcpy(dest,"Win2003 Server"); break;
            }
          break;
          case 6:
            switch(osvi.dwMinorVersion)
            {
              case 0: strcpy(dest,"Vista"); break;
              case 1: strcpy(dest,"Windows 7"); break;
            }
          break;
        }
      break;
    }

    if(*dest == 0) // not a known OS => just write the numbers
      sprintf(dest,"%ul.%ul.%ul.%ul",
              (uint32_t)osvi.dwPlatformId,
              (uint32_t)osvi.dwMajorVersion,
              (uint32_t)osvi.dwMinorVersion,
              (uint32_t)osvi.dwBuildNumber);
  }
}
//---------------------------------------------------------------------------
// driver_file: the path from system32 folder to driver, e.g. "drivers\\ser2pl.sys" or "drivers\FlashUSB.sys"
uint8_t GetSystem32FileVersion(char *driver_file, char *dest)
{
  uint8_t result = false;
  if(dest && driver_file)
  {
    char filename[500];

    filename[0] = 0;
    GLOBAL_get_special_folder(CSIDL_SYSTEM, filename);
    if(*filename == 0) // not found?
      strcpy(filename,"c:\\WINDOWS\\system32"); // then hard code the best guess
    GLOBAL_append_file_parts(filename, driver_file);

    if(FILE_exists(filename))
    {
      uint16_t Major=0, Minor=0, Release=0, Build=0;
      GLOBAL_GetFileVersion(filename, &Major, &Minor, &Release, &Build);
      if(Major != 0 || Minor != 0 || Release != 0 || Build != 0)
      {
        sprintf(dest,"%i.%i.%i.%i",Major, Minor, Release, Build);
        result = true;
      }
    }
    if(!result)
      strcpy(dest,"N/A");
  }
  return result;
}
//---------------------------------------------------------------------------
#endif // !WIN32
