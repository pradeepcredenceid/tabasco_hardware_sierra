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
* Description: Functions for reading INI files.
*
*
* Revision Information:
*  $File name:  /msw_tools/FlashTool/libDownloadTool/src/Portable_code/single_file_handler/FlashInitRead.c $
*  $CC-Version: .../oint_tools_flashtoole2_01/10 $
*  $Date:       2014-03-13    15:40:44 UTC $
*   Resonsible:
*   Comment:
*
*/

#include "FlashInitRead.h"
#include "FileIO.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/***************************************************************************************/

#define MAX_LINE_LENGTH 100
#define NOF_TYPE_NAMES 13

static const char * const Names[NOF_TYPE_NAMES] = { "Algorithm", "Size",
  "Regions", "PreErase", "MaxBufferWrite", "Section0Sectors", "Section0Size",
  "Section1Sectors", "Section1Size",  "Section2Sectors", "Section2Size",
  "Section3Sectors", "Section3Size" };

static int const NameLength[NOF_TYPE_NAMES] = { 9, 4, 7, 8, 14, 15, 12, 15, 12, 15, 12, 15, 12 };

typedef enum
{
  cfiUnknown = -1, cfiAlgorithm = 0, cfiSize, cfiRegions, cfiPreErase, cfiMaxBufferWrite,
  cfiSection0Sectors, cfiSection0Size, cfiSection1Sectors, cfiSection1Size,
  cfiSection2Sectors, cfiSection2Size, cfiSection3Sectors, cfiSection3Size
} cfiParameterType;

#define BASESTATUS  ((1 << cfiAlgorithm) | (1 << cfiSize) | (1 << cfiRegions) | (1 << cfiPreErase) | (1 << cfiMaxBufferWrite))
#define BASESTATUS1 (BASESTATUS  | (1 << cfiSection0Sectors) | (1 << cfiSection0Size))
#define BASESTATUS2 (BASESTATUS1 | (1 << cfiSection1Sectors) | (1 << cfiSection1Size))
#define BASESTATUS3 (BASESTATUS2 | (1 << cfiSection2Sectors) | (1 << cfiSection2Size))
#define BASESTATUS4 (BASESTATUS3 | (1 << cfiSection3Sectors) | (1 << cfiSection3Size))

static uint16_t StatusWord;
static const uint16_t ValidStatusWords[4] = { BASESTATUS1, BASESTATUS2, BASESTATUS3, BASESTATUS4 };

/***************************************************************************************/
uint8_t FileFindText(FILE *in, char *find);
static cfiParameterType GetIndexOf(char *line);
static void Int2Hex(uint32_t number, char *dest);
static uint32_t GetHexNumber(char *p, int max_digits);
static uint32_t GetNumber(char *p);
static uint8_t IsWhiteSpace(char ch);
static char *skip_white_space(char *p);
static char *GetNextLine(FILE *in);
/***************************************************************************************/

static cfiParameterType GetIndexOf(char *line)
{
  int n;
  char ch;
  for(n = 0 ; n < NOF_TYPE_NAMES ; n++)
  {
    if(strncmp(line,Names[n],NameLength[n]) == 0) /* possible match? */
    {
      ch = line[NameLength[n]];
      if(IsWhiteSpace(ch) || ch == (char)'=') /* whole word match? */
        return (cfiParameterType)n;
    }
  }
  return cfiUnknown;
}

/***************************************************************************************/
/* Text search in file. Return true if found and the file is at the position just after found text */
uint8_t FileFindText(FILE *in, char *find)
{
  uint32_t pos = 0;
  int match = 0;
  uint8_t allow_back = false;
  char ch;
  uint8_t AtEOF;

  do
  {
    AtEOF = (FILE_read(in,(uint8_t*)&ch,1) == FILE_status_EOF);
    if(find[match] == ch)
    {
      if(match==0)
      {
        pos = FILE_pos(in);
        allow_back = true;
      }
      match++;
    }
    else
    {
      if(match && allow_back)
      {
        FILE_seek(in, pos); /* Go back to were we were */
        allow_back = false;
      }
      match = 0;
    }
  }while(find[match] != 0 && !AtEOF); /* Okay we have written to EOF or the searched word(s) */
  return(find[match] == 0); /* Found ? */
}
/***************************************************************************************/
static void Int2Hex(uint32_t number, char *dest)
{
  int shift, digit;
  for(shift = 28; shift >= 0 ; shift -= 4)
  {
    digit = ( number >> shift ) & 0xF;               /* get wanted nibble       */
    *dest++ = digit + ((digit < 10) ? '0' : 'A'-10); /* convert nibble to ASCII */
  }
  *dest = 0; /* NULL-terminate string */
}
/***************************************************************************************/

static uint32_t GetHexNumber(char *p, int max_digits)
{
  int ch;
  uint32_t result = 0;
  int digits = 0;

  while(digits++ < max_digits)
  {
    ch = toupper(*p++);
    ch = isdigit(ch) ? ch - '0' : ch - 'A' + 10;
    if(ch < 0 || ch > 15)
      return result;
    result = 16*result + ch; /* multiplication by 16 is done with shifts by the compiler */
  }
  return result;
}
/***************************************************************************************/
static uint32_t GetNumber(char *p)
{
  if(p[0] == (char)'0' && p[1] == (char)'x') /* Hex? */
    return GetHexNumber(p+2,8);
  return atoi(p);
}
/***************************************************************************************/

static uint8_t IsWhiteSpace(char ch)
{
  switch(ch)
  {
    case ' ':
    case 10: /* Line feed */
    case 13: /* CR */
    case 9:  /* tab */
      return true;
    default:
      return false;
  }
}
/***************************************************************************************/
static char *skip_white_space(char *p)
{
  if(p)
  {
    while(*p && IsWhiteSpace(*p))
      p++;
  }
  return p;
}
/***************************************************************************************/

static char *GetNextLine(FILE *in)
{
  static char line[MAX_LINE_LENGTH+1];
  char ch = 0, *p;
  int len;

  p = line;
  len = MAX_LINE_LENGTH;
  while(ch != 10 && len > 0)
  {
    if(FILE_read(in,(uint8_t*)&ch,1) == FILE_status_EOF)
    {
      *p = 0;
      return line;
    }
    if(ch != 13) /* don't store CR */
    {
      *p++ = ch;
      len--;
    }
  }
  *p = 0;
  return line;
}
/***************************************************************************************/
uint8_t FlashInitRead(char *FileName, FlashInfoStruct *cfi)
{
  uint8_t result = false;
  FILE *init = FILE_open(FileName, FILE_open_mode_read);
  int n;
  if(init)
  {
    char tag[12] = "[";
    char *line, *p;
    uint8_t done;
    cfiParameterType parameter;
    uint32_t num, nofRegions;

    result = true; /* assume okay for now */
    for(n = 0 ; n < 4 ; n++)
    {
      num = cfi->Manufacturer | ((cfi->ID & 0xFFFF) << 16);
      if(num) /* valid cfi? */
      {
        Int2Hex(num, tag+1); /* Make tag string to search for */
        tag[9] = ']'; /* insert end tag character */
        tag[10] = 0; /* NULL-terminate string */
        FILE_seek(init, 0); /* Go back beginning of file to ensure we can find tags above current position */
        if(FileFindText(init,tag))
        {
          GetNextLine(init); /* skip first line with the tag in */
          done = false;
          StatusWord = 0; /* reset status word so we are ready to process a new tag */
          nofRegions = 0;
          while(!done) /* get lines until invalid line it read */
          {
            line = GetNextLine(init);
            p = strchr(line,'=');
            if(p)
            {
              p = skip_white_space(p + 1); /* skip white space after equal sign */
              if(!isdigit(*p)) /* not a digit after '=' ? (hex numbers also start with a digit "0x" */
                p = NULL; /* indicate error */
            }
            if(!p) /* no equal sign, then this is not a valid line */
              done = true;
            else
            {
              parameter = GetIndexOf(line);
              num = GetNumber(p); /* extract the number (hex or decimal) */
              switch(parameter)
              {
                case cfiUnknown:   done = true; break; /* stop on read error, and evaluate the results */
                case cfiAlgorithm: cfi->Algorithm = num; StatusWord |= (1 << cfiAlgorithm); break;
                case cfiSize:      cfi->Size      = num; StatusWord |= (1 << cfiSize);      break;
                case cfiRegions:   cfi->Regions   = num; StatusWord |= (1 << cfiRegions); nofRegions = num; break;
                case cfiPreErase:  cfi->PreErase  = num; StatusWord |= (1 << cfiPreErase);  break;
                case cfiMaxBufferWrite:  cfi->MaxBufferWrite = num; StatusWord |= (1 << cfiMaxBufferWrite);       break;
                case cfiSection0Sectors: cfi->Sections[0].Sectors = num; StatusWord |= (1 << cfiSection0Sectors); break;
                case cfiSection0Size:    cfi->Sections[0].Size    = num; StatusWord |= (1 << cfiSection0Size);    break;
                case cfiSection1Sectors: cfi->Sections[1].Sectors = num; StatusWord |= (1 << cfiSection1Sectors); break;
                case cfiSection1Size:    cfi->Sections[1].Size    = num; StatusWord |= (1 << cfiSection1Size);    break;
                case cfiSection2Sectors: cfi->Sections[2].Sectors = num; StatusWord |= (1 << cfiSection2Sectors); break;
                case cfiSection2Size:    cfi->Sections[2].Size    = num; StatusWord |= (1 << cfiSection2Size);    break;
                case cfiSection3Sectors: cfi->Sections[3].Sectors = num; StatusWord |= (1 << cfiSection3Sectors); break;
                case cfiSection3Size:    cfi->Sections[3].Size    = num; StatusWord |= (1 << cfiSection3Size);    break;
              }
            }
          }
          /* Now evaluate the results: */
          if(StatusWord == 0) /* nothing assigned? */
            result = false;
          else
          {
            if(StatusWord != (1 << cfiPreErase)) /* not just an allowed preErase? */
            {
              if(nofRegions > 0 && nofRegions <= 4) /* Regions in the valid range [1;4] */
              {
                if(StatusWord != ValidStatusWords[nofRegions - 1]) /* not all parameters found? */
                  result = false;
              }
              else
                result = false;
            }
          }
        }
        else  /* tag not found */
        {
          if(!cfi->CfiCompliant) /* if it is not CfiCompliant then it is an error */
            result = false;
        }
      }
      cfi++; /* go to next struct */
    } /* eof "for(n = 0 ; n < 4 ; n++)" */
    FILE_close(init);
  }
  return result;
}
/***************************************************************************************/


