
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
 *  $File name:  /msw_tools/FlashTool/libDownloadTool/src/Portable_code/text_func/IFWD_TextFunc.c $
 *  $CC-Version: .../dhkristx_140314_v0/1 $
 *  $Date:       2014-04-23    10:40:50 UTC $
 *   Responsible: bentzen
 *   Comment:
 *     SMS00847521: "Need to remove Lint warnings..."
 */

#define THIS_MODULE MODULE_NONE

/*---------------------------------------------*/
/* Include files.                              */
/*---------------------------------------------*/
#include <stdio.h>     /* C standard lib: Containing printf scanf etc. */
#include <stdarg.h>    /* C standard lib: Containing functions for handling variable number of arguments */
#include <string.h>    /* C standard lib: Containing strcpy memcpy etc. */

#include "OS.h"
#include "DebugModule.h"
#include "IFWD_text_def.h"
#include "IFWD_global_func.h"

/*---------------------------------------------*/
/* typedef                                     */
/*---------------------------------------------*/

#define USE_TEXT_CLASS
#include "IMC_bootcore_strings.h"        //< Defining the IMC_bootcore_enum[] array with strings


/// Only include the strings
#define TEXT_DEF TEXT_CONST
#define TEXT_CLASS_IFWD_ERRORS
const char * IFWD_ERR_struct[] = {
  #include "IFWD_text_def_file_list.h"
  NULL
};
#undef TEXT_CLASS_IFWD_ERRORS

#define TEXT_CLASS_IFWD_INFO
const char *IFWD_INFO_struct[] = {
  #include "IFWD_text_def_file_list.h"
  NULL
};
#undef TEXT_CLASS_IFWD_INFO
#undef TEXT_DEF

#undef USE_TEXT_CLASS


/*---------------------------------------------*/
/* globale variabler                           */
/*---------------------------------------------*/

/*---------------------------------------------*/
/* Local defines.                              */
/*---------------------------------------------*/


/*---------------------------------------------*/
/* Local const                                 */
/*---------------------------------------------*/


/*---------------------------------------------*/
/* local variables   .                         */
/*---------------------------------------------*/

/*---------------------------------------------*/
/* local functions.                            */
/*---------------------------------------------*/






/****************************************************************************************
* Function:... GLOBAL_format_text
* Parameters:. text_class In The text class to use.
*              text_code  In The text code to use.
*              ...        In A variable number of parameters. (Must match the number of formatting codes in the english standard text)
* Returns:.... An encoded text string
* Description: Depending on the state of the GLOBAL_use_plain text flag this function will -
*              If true produce a plain english text.
*              If false produce an encoded string containing the text class, code,
*                       parameter types and  paramters in ASCII representation and separated by '|'
*              The text string in the header files can use the format codes known form the 'printf' function.
* Created:.... DD.MM.YY by (Full name / initials)
* Modified:... DD.MM.YY by (Full name / initials)
*                Modifications note.
****************************************************************************************/

char *GLOBAL_format_text(IFWD_DL_text_class_enum text_class, uint32_t text_code, ...)
{
  char *return_ptr;                   /* Pointer to the global text array */
  const char *format_ptr = "";              /* Used to parse thru the format string */
  char  work_format[MAX_TEXT_LENGTH]; /* Used to hold the format to merge the parts together incl the real format code code like %d or %08X */
  char  temp_text[MAX_TEXT_LENGTH];   /* A working copy ot the encoded text */
  char *work_format_ptr;              /* Used to add characters to the format string */
  const char *real_format_ptr;              /* Used to point to the real format code. */
  const char *last_format_ptr;
/*  char  hax[MAX_TEXT_LENGTH];*/


  uint8_t is_format;                     /* true if a % sign is found and until the format letter is found */
  va_list args;                           /* used tha nhandle the variable paramter list */
  uint16_t char_counter;

  return_ptr = GLOBAL_text;
  /*work_format_ptr = hax; */
  work_format_ptr = work_format;

  /* Prepare to read the variable list of paramenters. */
  va_start(args, text_code);

  /* Get the english standard text sting incl its formatting codes according to the text class */
  switch (text_class)
  {
    case IFWD_DL_text_class_ifx_error_code:                   /*Handle IFWD error codes */
    {
      GLOBAL_text[0] = '\0';
      format_ptr = IFWD_ERR_struct[text_code];
      break;
    }
    case IFWD_DL_text_class_process_info:                     /* Handle IFWD information texts */
    {
      GLOBAL_text[0] = '\0';
      format_ptr = IFWD_INFO_struct[text_code];
      break;
    }
    case IFWD_DL_text_class_os_api_error_code:                /* Handle OS API error codes */
    {
      get_os_api_error_text(text_code);
      break;
    }
    case IFWD_PSI_error_code:
    {
      GLOBAL_text[0] = '\0';
      format_ptr = (char*)get_psi_error_description((IMC_bootcore_enum)text_code);
      break;
    }
  }


  if (!GLOBAL_use_plain_text)
  {
    snprintf(GLOBAL_text, MAX_TEXT_LENGTH,"%d|%d",text_class,text_code);
  }

  last_format_ptr = format_ptr;
  real_format_ptr = format_ptr;

  /* parse the format string to get the parameter types */
  is_format = false;
  char_counter = 0;
  do
  {
/* SWISTART */
#ifndef SWI_ARM
    if (is_format && args != NULL)
#else /* !SWI_ARM */
    if (is_format)
#endif /* !SWI_ARM */
    {
      snprintf(work_format_ptr++, MAX_TEXT_LENGTH,"%c",*format_ptr);  /* Add format codes the the work_format string */
      switch (*format_ptr)
      {
        case 'd':
        case 'i':
        case 'o':
        case 'x':
        case 'X':
        case 'u':
        case 'c':
        {
          if (GLOBAL_use_plain_text)
          {
            memcpy (work_format, last_format_ptr, ++char_counter); /* Copy from the part last written to the after this format sub-str  */
            work_format[char_counter] = 0;                       /* strncpy did not work correctly so use memcpy and add \0 */
            snprintf(temp_text, MAX_TEXT_LENGTH, work_format ,va_arg(args, int));    /* Print the plain text string and use its format sub-str to print the integer value */
            strncat(GLOBAL_text,temp_text, sizeof(GLOBAL_text)-strlen(temp_text)-1);                         /* Add the newly created string to the global string */
            last_format_ptr = format_ptr+1;                        /* Set the last used pointer to here */
            char_counter = 0;                                      /* Reset the char_counter */
          }
          else
          {
            snprintf(temp_text, MAX_TEXT_LENGTH,work_format,temp_text,real_format_ptr,va_arg(args, int));  /* Print the format sub-str and parameter as a string */
            strncat(GLOBAL_text,temp_text, sizeof(GLOBAL_text)-strlen(temp_text)-1);                                               /* Add the newly created string to the global string */
          }
          is_format = false;  /* Reset the format flag */
          break;
        }
        case 's':
        {
          if (GLOBAL_use_plain_text)
          {
            memcpy (work_format, last_format_ptr, ++char_counter);    /* Copy from the part last written to the after this format sub-str  */
            work_format[char_counter] = 0;                          /* strncpy did not work correctly so use memcpy and add \0 */
            snprintf(temp_text, MAX_TEXT_LENGTH, work_format ,va_arg(args, char *));    /* Print the plain text string and use its format sub-str to print the integer value */
            strncat(GLOBAL_text,temp_text, sizeof(GLOBAL_text)-strlen(temp_text)-1);                            /* Add the newly created string to the global string */
            last_format_ptr = format_ptr+1;                           /* Set the last used pointer to here */
            char_counter = 0;                                         /* Reset the char_counter */
          }
          else
          {
            snprintf(temp_text, MAX_TEXT_LENGTH,work_format,temp_text,real_format_ptr,va_arg(args, char *));  /* Print the format sub-str and parameter as a string */
            strncat(GLOBAL_text,temp_text, sizeof(GLOBAL_text)-strlen(temp_text)-1);                                                  /* Add the newly created string to the global string */
          }
          is_format = false;  /* Reset the format flag */
          break;
        }
        case 'f':
        case 'e':
        case 'E':
        case 'g':
        case 'G':
        {
          if (GLOBAL_use_plain_text)
          {
            memcpy (work_format, last_format_ptr, ++char_counter);    /* Copy from the part last written to the after this format sub-str  */
            work_format[char_counter] = 0;                          /* strncpy did not work correctly so use memcpy and add \0 */
            snprintf(temp_text, MAX_TEXT_LENGTH, work_format ,va_arg(args, double));    /* Print the plain text string and use its format sub-str to print the integer value */
            strncat(GLOBAL_text,temp_text, sizeof(GLOBAL_text)-strlen(temp_text)-1);                            /* Add the newly created string to the global string */
            last_format_ptr = format_ptr+1;                           /* Set the last used pointer to here */
            char_counter = 0;                                         /* Reset the char_counter */
          }
          else
          {
            snprintf(temp_text, MAX_TEXT_LENGTH,work_format,temp_text,real_format_ptr,va_arg(args, double));  /* Print the format sub-str and parameter as a string */
            strncat(GLOBAL_text,temp_text, sizeof(GLOBAL_text)-strlen(temp_text)-1);                                                  /* Add the newly created string to the global string */
          }
          is_format = false;  /* Reset the format flag */
          break;
        }
        default:
        {
        }
      }
    }
    else
    {
      if (*format_ptr == (char)'%') /* Found a formatter char */
      {
        is_format = true;
        snprintf(work_format, MAX_TEXT_LENGTH,"%%s|%%s|%%");                     /* Prepare the format string to use to collect the parts into one string */
        work_format_ptr = work_format+strlen(work_format);     /* This pointer is used to collect the rest of the format string */
        real_format_ptr = work_format+strlen(work_format)-1;   /* This pointer points to the single format sub-string as found in the herder files */
      }
    }
    char_counter++;
  } while (*format_ptr++);

  if (GLOBAL_use_plain_text && (char_counter != 0))  /* Handle text after the last format sub-str */
  {
     snprintf(temp_text, MAX_TEXT_LENGTH, "%s",last_format_ptr);
     strncat(GLOBAL_text,temp_text, sizeof(GLOBAL_text)-strlen(temp_text)-1);
  }


  va_end(args);
  return (return_ptr);
}

/***************************************************************************************
  Just returns the raw fixed text given the text code and class without any formatting
****************************************************************************************/
const char *GLOBAL_get_raw_text(IFWD_DL_text_class_enum text_class, uint32_t text_code)
{
  const char *format_ptr = "";

  switch (text_class)
  {
    case IFWD_DL_text_class_os_api_error_code:  break;
    case IFWD_DL_text_class_ifx_error_code:  format_ptr = IFWD_ERR_struct[text_code];         break;
    case IFWD_DL_text_class_process_info:    format_ptr = IFWD_INFO_struct[text_code];        break;
    case IFWD_PSI_error_code:
    {
      format_ptr = (char*)get_psi_error_name((IMC_bootcore_enum)0); // for the function coverage.
      format_ptr = (char*)get_psi_error_description((IMC_bootcore_enum)text_code);
      break;
    }
  }
  return format_ptr;
}


