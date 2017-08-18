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
 *  Helper functions for mainly filesystem related tasks
 *
 * Revision Information:
 *  $File name:  /msw_tools/FlashTool/libFlsTool/src/StringExt.cpp $
 *  $CC-Version: .../oint_tools_flashtoole2_01/13 $
 *  $Date:       2014-03-13    15:37:08 UTC $
 *   Resonsible:
 *   Comment:
 *
 */

#include "StringExt.h"
#include "Exceptions.h"

#include <limits.h>

#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>

using namespace std;

#ifdef _WIN32
  char* strndup(const char* s, size_t n)
  {
    char* dest = (char*)malloc(n+1);
    strncpy(dest, s, n);
    dest[n] = 0;
    return dest;
  }
#endif

int stoi(const string& str)
{
#ifdef ANDROID
  return (int)strtol(str.c_str(), NULL, 0);
#else
  int i = 0;
  istringstream iss(str);
  if (!(iss >> i))
    throw runtime_error("Could not convert string to integer");
  return i;
#endif
}

/**
 *  Convert integer to string
 */
string itos(uint32_t obj)
{
  stringstream ss;
  if (!(ss << obj))
    throw_exception("Could not stringify!", "");
  return ss.str();
}


char* str_to_lower(char* str)
{
  for (char*p = str; *p; ++p)
    *p = tolower(*p);
  return str;
}

// Insensitive case string compare
bool StringCaseCompare(const string& str1, const string& str2)
{
  for (size_t i=0; i<str1.size(); ++i)
    if (tolower(str1[i]) != tolower(str2[i]))
      return false;
  return true;
}

bool replace(string& str, const string& from, const string& to)
{
    size_t pos = str.find(from);
    if(pos == string::npos)
        return false;
    str.replace(pos, from.length(), to);
    return true;
}

string& TrimRight(string& s, const string& delimiters )
{
  return s.erase( s.find_last_not_of( delimiters ) + 1 );
}

string& TrimLeft(string& s, const string& delimiters )
{
  return s.erase( 0, s.find_first_not_of( delimiters ) );
}

string& Trim(string& s, const string& delimiters )
{
  return TrimLeft( TrimRight( s, delimiters ), delimiters );
}




string StringToLower(string str)
{
  for (string::iterator p = str.begin(); str.end() != p; ++p)
    *p = tolower(*p);
  return str;
}

string StringToUpper(string str)
{
  std::transform(str.begin(), str.end(), str.begin(), ::toupper);
  return str;
}

string getHexString(uint64_t value, int width, char fill)
{
  stringstream ss;
  ss << "0x" << setw(width) << setfill(fill) << uppercase << hex << value;
  return ss.str();
}

string getIntString(uint64_t value, int width, char fill)
{
  stringstream ss;
  ss << setw(width) << setfill(fill) << uppercase << value;
  return ss.str();
}

/* Strip whitespace chars off end of given string, in place. Return s. */
char* rtrim(char* s)
{
  char* p = s + strlen(s);
  while (p > s && isspace(*--p))
    *p = '\0';
  return s;
}

/* Return pointer to first non-whitespace char in given string. */
char* ltrim(const char* str)
{
  while (*str && isspace(*str))
    str++;
  return (char*)str;
}

char* trim(char* str)
{
  char* ptr = str;
  return ltrim(rtrim(ptr));
}

/* Version of strncpy that ensures dest (size bytes) is null-terminated. */
char* strncpy0(char* dest, const char* src, size_t size)
{
    strncpy(dest, src, size);
    dest[size - 1] = '\0';
    return dest;
}

/* Wrapper for snprintf for returning zero when the buffer is too small */
size_t snprintf0(char* str, size_t size, const char* append, ...)
{
  va_list args;
  va_start(args, append);
  size_t copied = vsnprintf(str, size, append, args);
  va_end(args);

  if (copied >= size) {
    printf("Insufficient buffer (Size: %d)!\n", (int)size);
    return 0;
  }
  return copied;
}

/**
 * Return pointer to byte *AFTER* NULL-termination
 * which is NOT standard stpcpy
 **/
char *stpcpy0(char *dest, const char *src)
{
  if (!src)
    return dest;

  do {
    *dest++ = *src;
  } while (*src++ != '\0');

  return dest;
}

/**
 * Locates any of the delimiters in the str,
 * returning the position in str or -1 if not found.
 */
int strfind(char* str, const char* delimiters)
{
  char* pos = NULL;
  if (!delimiters || !str) return -1;

  for(size_t i=0; i<strlen(delimiters); i++)
    if ((pos = strchr(str, delimiters[i])) != NULL)
      break;

  if (pos)
    return strlen(str)-strlen(pos);

  return -1;
}

/**
 * Split a string on a number of delimiters or just a single.
 * Returns a newly allocated list of strings, which have been splitted.
 */
char** strsplit(const char* _str, const char* delimiters)
{
  if (!delimiters || !_str) return NULL;

  char** final = NULL;
  int pos, tmp, i=0;
  char* list[64+1];
  char* str = strdup(_str);

  if (strfind(str, delimiters) == -1)
    list[i++] = str;
  else
  {
    for (pos=0; (tmp = strfind(&str[pos], delimiters)) >= 0;)
    {
      if (pos==0 && tmp > 0)
        list[i++] = &str[pos];
      pos += tmp;
      str[pos++] = 0;
      list[i++] = &str[pos];
      if (i==64)
        break;
    }
  }
  list[i++] = NULL;

  final = (char**)malloc(sizeof(char*)*i);
  if (final)
  {
    for(pos=0; pos<i;pos++)
      final[pos] = list[pos] ? strdup(list[pos]) : NULL;
  }
  free(str);

  return final;
}

