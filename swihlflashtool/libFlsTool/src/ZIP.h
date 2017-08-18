/* -------------------------------------------------------------------------
 Copyright (C) 2013-2014 Intel Mobile Communications GmbH

      Sec Class: Intel Confidential (IC)
 ----------------------------------------------------------------------
 Revision Information:
    $File name:  /msw_tools/FlashTool/libFlsTool/src/ZIP.h $
    $CC-Version: .../dhkristx_140314_v0/1 $
    $Date:       2014-04-16    12:25:29 UTC $
 ----------------------------------------------------------------------
 by CCCT (0.12h)
 ---------------------------------------------------------------------- */

#ifndef ZIP_H_
#define ZIP_H_

#include <stdint.h>

#ifdef __cplusplus

#include "Logger.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

using namespace std;
using namespace ipa;


/* Intel Platform Abstraction */
namespace ipa
{
  //#define ZIP_LOAD_MAGIC           0x04034b50
  //#define ZIP_OPTIONAL_SIGNATURE   0x08074b50
  //#define ZIP_DIR_END_SIGNATURE    0x06054b50
  #define ZIP_DIR_SIGNATURE        0x02014b50

  //#define ZIP_CENTRAL_MAGIC       "PK\1\2"
  //#define ZIP_LOCAL_MAGIC         "PK\3\4"
  #define ZIP_END_OF_DIR_MAGIC    "PK\5\6"
  //#define DATADES_MAGIC    "PK\7\8"
  //#define EOCD64LOC_MAGIC  "PK\6\7"
  //#define EOCD64_MAGIC     "PK\6\6"

  class ZipFile
  {
  public:
    string name;
    string extra;

    uint32_t crc;
    uint32_t size;
    uint32_t compressed_size;
    uint32_t offset;

    time_t mod_time;
    int compression;

    vector<uint8_t> data;

    char* get_content(vector<uint8_t>& content);
  };

  class ZIP
  {
    Logger info;
    Logger debug;
    int verbosity;

    string m_file;
    size_t m_file_offset;
    size_t m_size;

    vector<ZipFile> files;

  public:
    ZIP(string file = "", int verbose = 0);
    ZIP(string file = "", size_t offset = 0, size_t size = 0, int verbose = 0);
    virtual ~ZIP() { };

    void load(string file, size_t offset = 0, size_t size = 0);

    void set_verbose(int verbose);

    ZipFile& get_file(int i);

    /* STL C++ Syntax */
    ZipFile* operator[](string name);
    ZipFile& operator[](size_t i) { return get_file(i); }
    size_t size() { return files.size(); }

  private:
    /**
     * Parsing a ZIP file
     */
    void parse(std::istream& zip);
  };

} /* namespace ipa */
#else

  // TODO: ANSI C implementation

#endif /* __cplusplus */
#endif /* ZIP_H_ */
