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
 * Revision Information:
 *  $File name:  /msw_tools/FlashTool/libFlsTool/src/INI.cpp $
 *  $CC-Version: .../dhkristx_140314_v0/3 $
 *  $Date:       2014-05-01    12:46:48 UTC $
*/

#include "INI.h"
#include "StringExt.h"

#include <assert.h>

char INI_Error[INI_MAX_LINE + 48];

/**
 * SyntaxError()
 *
 *  Saves the error line and prints the "line"
 */
static int SyntaxError(int line, char* ptr)
{
  snprintf(INI_Error, sizeof(INI_Error),
          "INI: Syntax error at line %d near '%s'..\n", line, ptr);
  return 0;
}

/**
 * RemoveComments()
 *
 *  Remove the comments from a value string
 **/
static char* RemoveComments(char* s)
{
  char* p = s;
  int was_space = 0;
  while (*p && !(was_space && *p == ';')) {
    was_space = isspace(*p);
    p++;
  }
  *p = '\0';
  return rtrim(s);
}


/**
 * RemoveApostrophes()
 *
 *  Remove ' & " from a value strings
 **/
static char* RemoveApostrophes(char* s)
{
  assert(s != NULL);

  char* first = s;
  size_t len = strlen(s);
  if (len > 1)
  {
    char* last = first + len - 1;

    if ((*first == '"'  && *last == '"')
     || (*first == '\'' && *last == '\''))
    {
      *first = *last = '\0';
      first++;
    }
  }
  return first;
}

/**
 * Simple INI Parser
 **/
int ParseINI(const char* ini_file,
              int (*callback)(void*, const char*, const char*, const char*),
              void* args)
{
  INI_Error[0] = 0;

  int linenr = 0;
  char line[INI_MAX_LINE];
  char section[INI_MAX_LINE];
  section[0] = 0;

  FILE* fd = fopen(ini_file, "r");
  if (!fd) {
    snprintf(INI_Error, INI_MAX_LINE,
            "INI: Cannot open input file: %s\n", ini_file);
    return 0;
  }

  while (fgets(line, sizeof(line), fd) != NULL)
  {
    linenr++;
    char* ptr = trim(line);

    //< Ignore comments
    if (*ptr == ';' || *ptr == '#' || !*ptr)
      continue;

    //< Parsing [Section]
    if (*ptr == '[')
    {
      char* end = strchr(ptr, ']');
      if (!end)
	  {
        fclose(fd);
        return SyntaxError(linenr, ptr);
	  }

      strncpy0(section, ptr+1, end-ptr);

      if (!callback(args, section, NULL, NULL))
        break;
    }
    //< Parsing "key=value"
    else
    {
      char* equal = strchr(ptr, '=');
      if (!equal)
	  {
        fclose(fd);
        return SyntaxError(linenr, ptr);
	  }

      *equal = '\0';
      char *key = trim(ptr);
      char *value = RemoveComments(equal+1);
      value = trim(value);
      value = RemoveApostrophes(value);

      if (!key || !value)
	  {
        fclose(fd);
        return SyntaxError(linenr, ptr);
	  }

      if (key && value)
        if (!callback(args, section, key, value))
          break;
    }
  }
  fclose(fd);
  return 1;
}



/*
int SaveINI(const char* ini_file, const char* content)
{
  size_t size = 1024;
  char* ini = (char*)malloc(size);
  memset(ini, 0, size);

  sprintf(ini, "\n; - Auto-generated INI - ;\n\n");

  for (int i=1; content[i]; ++i)
  {
    size_t new_size = strlen(ini) + 12 + strlen(content[i]);

    if (new_size > size) {
      size = new_size;
      ini = (char*)realloc(ini, size);
    }
    sprintf(ini, "%s%d=\"%s\"\n", ini, i, content[i]);
  }

  Debug2("INI content\n'%s' (%d)\n", ini, strlen(ini));
  int res = FILE_setContent(file, ini, strlen(ini));
  free(ini);

  if (res)
    Debug(" USB Device Map saved as %s.\n", file);
  return res;
}
*/
