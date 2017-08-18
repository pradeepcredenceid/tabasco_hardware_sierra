/* -------------------------------------------------------------------------
 Copyright (C) 2013-2014 Intel Mobile Communications GmbH

      Sec Class: Intel Confidential (IC)
 ----------------------------------------------------------------------
 Revision Information:
    $File name:  /msw_tools/FlashTool/libFlsTool/src/Fls2Parser.h $
    $CC-Version: .../dhkristx_140314_v0/1 $
    $Date:       2014-04-22    9:13:29 UTC $
 ----------------------------------------------------------------------
 by CCCT (0.12h)
 ---------------------------------------------------------------------- */

#ifndef FLS2PARSER_H_
#define FLS2PARSER_H_

#include <stdint.h>

#ifdef __cplusplus

#include "Logger.h"
#include "FlsMetaFile.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

using namespace ipa::fls;

/* Intel Platform Abstraction - Fls */
namespace ipa
{
  namespace fls
  {
    #define PRG_MAGIC_NUMBER      0x693729F1
    #define FLS2_LOAD_MAGIC       0x01FEDABE

    typedef enum
    {
      PRG_FILE_TYPE = 1,
      FLS_FILE_TYPE,
      EEP_FILE_TYPE,
      DFFS_FILE_TYPE,
      DISK_FILE_TYPE,
      NUM_FILE_TYPES // This should be the last
    } prg_file_type;

    string generateFileName(string filename, int UID, string type, string extra = "");

    uint32_t fls2_get_size_of(const char* structure);

    /**
     *  The Intel Fls2 Parser Class
     **/
    class Fls2Parser
    {
    private:
      FlsMetaFile& m_ref;
      size_t m_size;
      vector<uint8_t> meta_data;

      Logger info;
      Logger debug;
      int verbosity;

    public:
      Fls2Parser(FlsMetaFile& ref, string file, int verbose);

      void load(const char* file);

      static void save(FlsMetaFile& ref, string file, int verbose);

      void set_verbose(int verbose);

      /**
       * Generate the SecPack
       *
       * @return  Reference to the sec_pack buffer
       */
      vector<uint8_t>& generate_sec_pack();

    private:
      /**
       * Parsing the Fls2 file
       */
      void fls2_parse(std::ifstream& fls);

      DownloadFile* get_download_file(size_t uid);

    };

  } /* namespace fls */
} /* namespace ipa */
#endif /* __cplusplus */
#endif /* FLS3PARSER_H_ */
