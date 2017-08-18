/*
*       Copyright (C) 2011-2014 Intel Mobile Communications GmbH
*
*            Sec Class: Intel Confidential (IC)
*
*
* This document contains proprietary information belonging to IMC.
* Design A/S. Passing on and copying of this document, use and communication
* of its contents is not permitted without prior written authorisation.
*
* Description: Functions for doing Tracing of data end events in the other modules.
*
*
* Revision Information:
*  $File name:  /msw_tools/FlashTool/libDownloadTool/src/OS_dependent_code/debug_os_if/DebugModule.c $
*  $CC-Version: .../dhkristx_140314_v0/1 $
*  $Date:       2014-05-01    14:00:06 UTC $
*   Comment:
*
*/

/*---------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "telephony/ril.h"

#define THIS_MODULE MODULE_NONE
#include "DebugModule.h"

//#define DEBUG_VERBOSITY

/*---------------------------------------------------------------------------*/

HANDLE debug_mutex;
uint8_t debug_active = false;
FILE *trace_file = 0;

static DebugVerbosityLevel m_verbose[MODULE_ALL]; // = { DEBUG_VERBOSITY_NORMAL };

#ifdef DEBUG_VERBOSITY
  #if 0
    static const char* verbosity_name[] = {
      "DEBUG_VERBOSITY_ERROR",
      "DEBUG_VERBOSITY_NORMA",
      "DEBUG_VERBOSITY_DEBUG",
      "DEBUG_VERBOSITY_DEBUG_MORE",
      "DEBUG_VERBOSITY_DEBUG_ALOT",
      "DEBUG_VERBOSITY_STATE_TRACE"
    };
  #endif
  static const char* module_name[] = {
    "MODULE_NONE",
    "MODULE_MAIN",
    "MODULE_MFH",
    "MODULE_SFH",
    "MODULE_TRA",
    "MODULE_PROT",
    "MODULE_BOOT",
    "MODULE_COMM",
    "MODULE_GENERIC_USB",
    "MODULE_ALL"
  };
#endif

void DebugSetVerbosity(DebugVerbosityLevel verbosity)
{
  #ifdef DEBUG_VERBOSITY
    printf("DebugSetVerbosity> Setting verbosity to %d for ALL modules!\n", verbosity);
  #endif
  int i;
  for (i=0;i<MODULE_ALL; ++i)
    m_verbose[i]  = verbosity;
}

void DebugSetModuleVerbosity(MODULE_enum module, DebugVerbosityLevel verbosity)
{
  #ifdef DEBUG_VERBOSITY
    printf("DebugSetVerbosity> Setting verbosity to %d for %s\n", verbosity, module_name[module]);
  #endif
  m_verbose[module] = verbosity;
}


/**
 * 	This can be used to print out Debug messages in
 *	the trace log, Borland debug console & Terminal consoles
 **/
void PrintLog(MODULE_enum module, int level, const char* format, ...)
{
  if (level > GLOBAL_trace_verbosity && level > (int)m_verbose[module])
    return;

  #ifdef DEBUG_VERBOSITY
    printf("PrintLog> %s (%d) Output verbose: %d\n", module_name[module], m_verbose[module], level);
  #endif

  va_list args;
  va_start(args, format);

  uint32_t len = findFormatSize(format, args);
  char* buffer = malloc(sizeof(char)*len+1);
  if (!buffer) return;
  vsnprintf(buffer, len, format, args);
  va_end(args);

  /* Insert a text string in the trace log */
  if(level <= GLOBAL_trace_verbosity)
  {
    int trace_type = (level > DEBUG_VERBOSITY_NORMAL) ? TR_ExtDebug_Text : TR_ExtInfo_Text;

    TR_BEGIN(TR_ExtInfo, trace_type, 0);
    TR_STR( buffer );
    TR_END;
  }

  if (level <= (int)m_verbose[module])
  {
    FILE* output = (level > DEBUG_VERBOSITY_NORMAL) ? stderr : stdout;
    fprintf(output, " %s", buffer);

    #ifdef ANDROID
    #if (RIL_VERSION >= 7)
    ALOGI(" %s", buffer);
    #else
    LOGI(" %s", buffer);
    #endif
    #endif
  }
  free(buffer);
}

/**
 * Reads through the format list and arguments to find out
 * how many bytes is needed to fit this string in a buffer.
 */
uint32_t findFormatSize(const char* format, va_list args)
{
  assert(format != NULL);
  assert(strlen(format) > 0);

  char* ptr;
  uint32_t len = strlen(format);

  for (; *format; ++format)
  {
    if (*format != '%')
      continue;

    format++;
    while (isdigit(*format)
         || *format == '+' || *format == '-'
         || *format == '*' || *format == '.'
         || *format == '$' || *format == '\'')
    {
      //< Something like "%04s" will add 4 to the string length
      if (isdigit(*format))
        len += *format;
      format++;
    }

    switch(*format)
    {
      case 'd': case 'i': case 'o':
      case 'x': case 'X': case 'u':
      case 'c':
        (void)va_arg(args, int);
        len += sizeof(int);
        break;

      case 'f': case 'F':
      case 'e': case 'E':
      case 'g': case 'G':
      case 'l': case 'L':
        (void)va_arg(args, double);
        len += sizeof(double);
        break;

      case 's':
        ptr = va_arg(args, char*);
        len += ptr ? strlen(ptr) : 6; //< for "(null)"
        break;

      default:
        printf(" Format '%c' not understood \n", *format);
        break;
    }
  }
  return (len+1); // Add the ending '\0'
}


/**
 * Outputs a char, so its readable in a terminal
 */
char *getChar(unsigned char c) {
  static char result[8+1];
  if ((c < 32) || (c > 126)) {
    switch (c) {
      case 10:  snprintf(result,8,"<LF>");       break;
      case 13:  snprintf(result,8,"<CR>");       break;
      case 27:  snprintf(result,8,"<ESC>");      break;
      default:  snprintf(result,8,"<%02hX>",c);  break;
    }
  } else {
    snprintf(result,sizeof(result),"%c",c);
  }
  return result;
}


/**
 * Uses the getChar() function to format a string to readable characters
 */
const char* getHex(const uint8_t* buffer, size_t nbytes)
{
  size_t i;
  static char hEx[512];
  hEx[0] = '\0';

  if ((nbytes*3+1) > sizeof(hEx))
    return NULL;

  // NOTE: If this should be changed, it should be into std::string...
  for(i=0; i < nbytes; ++i)
    snprintf(&hEx[3*i], 4, "%02X ", buffer[i]);

  if (i>0)
    hEx[3*i - 1] = '\0'; // Note: Already done in snprintf()

  return hEx;
}

/*---------------------------------------------------------------------------*/

#ifndef DISABLE_LIBUSB
  #include "GenericUSB.h"
#endif
#include "EepDeltaMerge.h"

#ifdef INCLUDE_DUT_USB_TEST
  #include "IFWD_dut_usb_test.h"
#endif

uint8_t self_test(void)
{
	char cstr[500];

// Debug module
	PrintLog(MODULE_NONE, DEBUG_VERBOSITY_NORMAL, "Running self test\n");
	getChar(13);
	stpcpy0(cstr,"abc");

// EepDeltaMerge
	//EEP_linearize_bering((uint16_t*)cstr, 100, 10); // Must return true


  #ifdef INCLUDE_DUT_USB_TEST
	DUT_usb_test(NULL);
  #endif

// GenericUSB
  #ifndef DISABLE_LIBUSB
	GenericUSB_close(0); // ID 0 must give error
  #endif

// Global func
	get_os_api_error_text(0);
	GetSystem32FileVersion("a",cstr);
	ShowMessage(NULL);
  GetOSVersion(cstr);

	return true;
}
