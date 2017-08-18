/* -------------------------------------------------------------------------
 Copyright (C) 2013-2014 Intel Mobile Communications GmbH

      Sec Class: Intel Confidential (IC)
 ----------------------------------------------------------------------
 Revision Information:
    $File name:  /msw_tools/FlashTool/libFlsTool/src/Fls3MetaParser.h $
    $CC-Version: .../dhkristx_140314_v0/1 $
    $Date:       2014-04-22    9:13:35 UTC $
 ----------------------------------------------------------------------
 by CCCT (0.12h)
 ---------------------------------------------------------------------- */

#ifndef FLS3METAPARSER_H_
#define FLS3METAPARSER_H_

#include <stdint.h>

#ifdef __cplusplus

#include "JSON.h"
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
    /**
     *  The Fls3 Meta data Parser Class
     **/
    class Fls3MetaParser
    {
    private:
      Logger info;
      Logger debug;
      FlsMetaFile& m_ref;

    public:
      Fls3MetaParser(FlsMetaFile& ref, vector<uint8_t>& buffer, int verbosity);

      void set_verbose(int verbose);

      void load(vector<uint8_t>& buffer);

      void parse(json_value *value, string path = "", size_t index = 0);

      void parse_value(std::string path, json_value* value, size_t index = -1);

    private:
      FlsEmbedFile* lookup_embedded_file(string filename);
    };

  } /* namespace fls */
} /* namespace ipa */
#endif /* __cplusplus */
#endif /* FLS3METAPARSER_H_ */
