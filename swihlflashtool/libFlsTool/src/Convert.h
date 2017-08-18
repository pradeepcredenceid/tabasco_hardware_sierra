/* ---------------------------------------------------------------------------
  Copyright (C) 2013-2014 Intel Mobile Communications GmbH

       Sec Class: Intel Confidential (IC)

  All rights reserved.
  ----------------------------------------------------------------------------
  Description:
  Converter functions for converting between various formats.
  ----------------------------------------------------------------------------
  Revision Information:
  $File name:  /msw_tools/FlashTool/libFlsTool/src/Convert.h $
  $CC-Version: .../dhkristx_140314_v0/1 $
  $Date:       2014-04-09    13:19:58 UTC $
  ------------------------------------------------------------------------- */

#ifndef ConvertH
#define ConvertH

#include "OS.h"

/* ANSI C Interface */
#ifdef __cplusplus
extern "C" {
#endif

  void ConvertHexToBin(const char *in, const char *out, int verbose);

#ifdef __cplusplus
}
#endif


/* STL C++ Interface */
#ifdef __cplusplus
  #include <string>
  #include <vector>

  using namespace std;

  /* The Intel HEX structure */
  typedef enum {
    HEX_TYPE_DATA = 0,
    HEX_TYPE_EOF,
    HEX_TYPE_EXT_SEG_ADDR,
    HEX_TYPE_START_SEG_ADDR,
    HEX_TYPE_EXT_ADDR,
    HEX_TYPE_START_ADDR,
  } HexType;

  typedef struct {
    string   hexString;
    uint8_t  len; /* 16/32 (64bit is not in intel Hex Standard yet) */
    uint64_t addr;
    uint8_t  type;
    uint8_t data[32];
    uint8_t  crc;
  } ihex_t;

  /* Class - IntelHexFile */
  class IntelHexFile
  {
    int verbose;
    uint64_t start_addr;
    uint64_t max_length;

    vector<ihex_t> iHex;

  public:
    IntelHexFile(string in = "", int verbose = 0);
    IntelHexFile(vector<string> in, int verbose = 0);

    void add(string in);

    void save(string out, bool overwrite = false);
    void save(vector<uint8_t>& buffer);

    uint64_t get_base_addr();

    void set_base_addr(uint64_t addr);
    void set_max_length(uint64_t length);
  };
#endif

#endif

