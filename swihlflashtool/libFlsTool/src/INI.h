/*
 *       Copyright (C) 2012-2013 Intel Mobile Communications GmbH
 *
 *            Sec Class: Intel Confidential (IC)
 *
 *
 * This document contains proprietary information belonging to IMC.
 * Denmark A/S. Passing on and copying of this document, use and communication
 * of its contents is not permitted without prior written authorisation.
 *
 * Revision Information:
 *  $File name:  /msw_tools/FlashTool/libFlsTool/src/INI.h $
 *  $CC-Version: .../oint_tools_flashtoole2_01/3 $
 *  $Date:       2013-10-02    15:00:12 UTC $
 *
 * Description:
 *  Simple INI Parser for loading and saving INI files
 *
 * Live Parsing Example:
 *  int ini_handler(void* arg, const char* section,
 *                  const char* key, const char* value)
 *  {
 *    if (!key)   printf("[%s]\n", section);
 *    else        printf("%s=%s\n", key, value);
 *  }
 *
 *  int main(int argc, char** argv)
 *  {
 *    if (argc < 2)
 *      perror("\n  Usage: %s <INI File>\n\n", argv[0]);
 *
 *    parseINI(argv[1], ini_handler, NULL);
 *    return 0;
 *  }
 */

#ifndef INI_H_
#define INI_H_

#include <ctype.h>       // isspace
#include <string.h>
/*
  #include <stdio.h>
  #include <stdarg.h>
  #include <stdlib.h>    // malloc
  #include <unistd.h>    // exit

  #include <sys/types.h>
  #include <sys/stat.h>
*/
#define INI_MAX_LINE      256

#ifdef __cplusplus
extern "C" {
#endif

//< Read this if an INI function fails
extern char INI_Error[];


/**
 * Simple INI Parser
 *
 * The callback argument is called for every key/value found,
 *  -> callback(args, section, key, value);
 *
 * Parsing can be cancelled if the callback function returns false.
 *
 * Returns true on success and fails on failure
 **/
int ParseINI(const char* ini_file,
              int (*callback)(void*, const char*, const char*, const char*),
              void* args);

#ifdef __cplusplus
}
#endif

#endif /* INI_H_ */
