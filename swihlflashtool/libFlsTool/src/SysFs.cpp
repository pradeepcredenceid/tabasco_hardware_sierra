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
       $$File name:  /msw_tools/FlashTool/libFlsTool/src/SysFs.cpp $
       $$CC-Version: .../dhkristx_140314_v0/1 $
       $$Date:       2014-04-16    12:30:33 UTC $
     ---------------------------------------------------------------------- */

#ifndef WIN32

#include "SysFs.h"
#include "FileIO.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>         //< read, sleep
#include <ctype.h>          //< isdigit()
#include <sys/stat.h>
#include <sys/param.h>      //< MAXPATHLEN

//#define USB_DEV_DIR "/dev/bus/usb"
#define USB_SYS_DIR "/sys/bus/usb/devices"

/**
 * Extract a string from a file in a path
 */
char* get_string_from_file(const char* path, const char* file)
{
  FILE* fp;
  char filepath[MAXPATHLEN] = "";
  char str[512] = "";

  if (path[0] == '/')
    snprintf(filepath, sizeof(filepath), "%s/%s", path, file);
  else
    snprintf(filepath, sizeof(filepath), "%s/%s/%s", USB_SYS_DIR, path, file);

  fp = fopen(filepath, "r");
  if (fp)
  {
    fscanf(fp, "%512s", str);
    fclose(fp);
  }
  return strdup(str);
}

/**
 * Extract an integer from a file in a path
 */
int get_int_from_file(const char* path, const char* file)
{
  int number = -1;
  char* nr = get_string_from_file(path, file);
  if (!nr)
    return -1;

  sscanf(nr, "%d", &number);
  free(nr);
  return number;
}

/**
 * Extract HEX from a file in a path
 */
int get_hex_from_file(const char* path, const char* file)
{
  int number = -1;
  char* nr = get_string_from_file(path, file);
  if (!nr)
    return -1;

  sscanf(nr, "%x", &number);
  free(nr);
  return number;
}

/**
 * Dump all files in a syspath
 */
#if 0
void SysFs_get_device_capabilities(const char* syspath, int indent)
{
  DIR           *dir;
  struct dirent *entry;
  char    spath[MAXPATHLEN] = "";

  if (indent > 14)
  {
    printf("Maximum indent reached\n");
    return;
  }

  char space[16] = "";
  memset(space, ' ', indent*2);

  if (syspath[0] == '/')
    snprintf(spath, sizeof(spath), "%s", syspath);
  else
    snprintf(spath, sizeof(spath), "%s/%s", USB_SYS_DIR, syspath);

  // printf("%s/\n", syspath);

  if(!(dir = opendir(spath))) { printf(" Path '%s' does not exist\n", spath); return; }
  for(entry = readdir(dir); entry; entry = readdir( dir))
  {
    if (entry->d_name[0] == '.')
      continue;

    if(entry->d_type == DT_DIR)
    {
      printf("%s  %s/\n", space, entry->d_name);
      char next_path[MAXPATHLEN] = "";
      snprintf(next_path, sizeof(next_path), "%s/%s", spath, entry->d_name);
      SysFs_get_device_capabilities(next_path, indent+1);
    }
    else
    {
      printf("%s  %s (%s)\n", space, entry->d_name,
             get_string_from_file(spath, entry->d_name));
    }
  }
  closedir(dir);
}
#endif

int SysFs_get_device_busnum(const char* syspath)
{
  return get_int_from_file(syspath, "busnum");
}

int SysFs_get_device_devnum(const char* syspath)
{
  return get_int_from_file(syspath, "devnum");
}

/**
 * Locate a TTY device name from a devpath
 *
 * @param devpath  The device path e.g. '1-1.1'
 *
 * @return         The tty name e.g. 'ttyACM1'
 */
char *SysFs_get_device_tty(const char* devpath)
{
  DIR             *dir;
  struct dirent   *entry;
  static char     path[MAXPATHLEN] = "";
  static char     spath[MAXPATHLEN] = "";

  if (devpath[0] == '/')
    snprintf(path, sizeof(path), "%s", devpath);
  else
    snprintf(path, sizeof(path), "%s/%s", USB_SYS_DIR, devpath);

  if (FileSystem_isdir(path))
  {
    snprintf(spath, sizeof(spath), "%s/%s:1.0/tty", path, fname(devpath));

    if(!(dir = opendir(spath))) { return NULL; }
    for(entry = readdir(dir); entry; entry = readdir(dir))
    {
      //< Locating the 'ttyACM%n' name
      if(entry->d_type == DT_DIR
       && strncmp(entry->d_name, "tty", 3) == 0)
      {
        closedir(dir);
        // printf("Found %s in '%s'...\n",entry->d_name, spath);
        return entry->d_name;
      }
    }
    closedir(dir);
  }
  return NULL;
}

/**
 * Locate a device via its ttyUSB device path
 * Returns the device syspath if found and NULL if not found
 */
char *SysFs_find_tty_device(const char* tty)
{
  DIR             *dir;
  struct dirent   *entry;
  static char     spath[MAXPATHLEN] = "";
  char* tty_name = fname(tty);

  if(!(dir = opendir(USB_SYS_DIR))) { perror("opendir"); return NULL; }
  for(entry = readdir( dir); entry; entry = readdir( dir))
  {
    if(entry->d_type == DT_LNK
     && isdigit(entry->d_name[0])    //< Catching USB device symlinks
     && !strchr(entry->d_name, ':')) //< Catching all the 'usb_device' types
    {
      snprintf(spath, sizeof(spath), "%s/%s/%s:1.0/tty/%s",
               USB_SYS_DIR, entry->d_name, entry->d_name, tty_name);

      //< Found Device!
      //  printf("Looking at %s\n", spath);
      if (fexists(spath))
      {
        closedir(dir);
        return entry->d_name;
      }
    }
  }

  closedir(dir);
  return NULL;
}

/**
 * Locates a tty device & saves the PID/VID
 */
uint8_t SysFs_find_tty_ids(const char* tty, uint16_t* vid, uint16_t* pid)
{
  char* syspath = SysFs_find_tty_device(tty);
  if (syspath)
  {
    *vid = get_hex_from_file(syspath, "idVendor");
    *pid = get_hex_from_file(syspath, "idProduct");
    return (*vid > 0 && *pid > 0);
  }
  return false;
}

/**
 * Reads the PID/VID from a sysfs device path
 */
uint8_t SysFs_get_ids(const char* syspath, uint16_t* vid, uint16_t* pid)
{
  if (syspath)
  {
    *vid = get_hex_from_file(syspath, "idVendor");
    *pid = get_hex_from_file(syspath, "idProduct");
    return (*vid > 0 && *pid > 0);
  }
  return false;
}

/**
 * Locate a device via Vendor & Product IDs
 * Returns the device syspath if found and NULL if not found
 */
char *SysFs_find_device(uint16_t vid, uint16_t pid, int number /* = 1 */)
{
  DIR             *dir;
  struct dirent   *entry;
  static char     spath[MAXPATHLEN] = "";

  if(!(dir = opendir(USB_SYS_DIR))) { perror("opendir"); return NULL; }
  for(entry = readdir( dir); entry; entry = readdir( dir))
  {
    if(entry->d_type == DT_LNK
     && isdigit(entry->d_name[0])    //< Catching USB device symlinks
     && !strchr(entry->d_name, ':')) //< Catching all the 'usb_device' types
    {
      //printf("Looking at %s\n", entry->d_name);
      snprintf(spath, sizeof(spath), "%s/%s", USB_SYS_DIR, entry->d_name);

      int id = get_hex_from_file(spath, "idProduct");
      if (id < 0 || pid != id)
        continue;

      id = get_hex_from_file(spath, "idVendor");
      if (id < 0 || vid != id)
        continue;

      //< Found Device!
      if (number-- == 1)
      {
        closedir(dir);
        return spath;
      }
    }
  }

  closedir(dir);
  return NULL;
}
#endif
