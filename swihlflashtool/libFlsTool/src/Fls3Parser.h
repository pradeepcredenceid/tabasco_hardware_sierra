/* -------------------------------------------------------------------------
 Copyright (C) 2013-2014 Intel Mobile Communications GmbH

      Sec Class: Intel Confidential (IC)
 ----------------------------------------------------------------------
 Revision Information:
    $File name:  /msw_tools/FlashTool/libFlsTool/src/Fls3Parser.h $
    $CC-Version: .../dhkristx_140314_v0/1 $
    $Date:       2014-04-22    9:13:41 UTC $
 ----------------------------------------------------------------------
 by CCCT (0.12h)
 ---------------------------------------------------------------------- */

#ifndef FLS3PARSER_H_
#define FLS3PARSER_H_

#include <stdint.h>

#ifdef __cplusplus

#include "Logger.h"
#include "FlsMetaFile.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

/* Intel Platform Abstraction - Fls */
namespace ipa
{
  namespace fls
  {
    #define TAR_DATA_BLOCK    512
    #define TAR_TYPE_NORMAL   '0'

    /**
     *  The Intel Fls3 Parser Class
     **/
    class Fls3Parser
    {
    private:
      FlsMetaFile& m_ref;
      vector<uint8_t> meta_data;
      vector<FlsEmbedFile*> embed_files;

      Logger info;
      Logger debug;
      int verbosity;

    public:
      Fls3Parser(FlsMetaFile& ref, string file, int verbose);

      void load(const char* file);

      static void save(FlsMetaFile& ref, string file, int verbose);

      void set_verbose(int verbose);

    private:
      /**
       * Parsing the Fls3 file (TAR)
       */
      void fls3_parse(std::ifstream& fls);

      /**
       * Parsing the JSON meta data
       */
      //void json_parse(vector<uint8_t>& buffer);
    };

  } /* namespace fls */
} /* namespace ipa */
#endif /* __cplusplus */
#endif /* FLS3PARSER_H_ */
