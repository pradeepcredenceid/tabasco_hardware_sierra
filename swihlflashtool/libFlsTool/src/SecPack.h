/*
 *       Copyright (C) 2013-2014 Intel Mobile Communications GmbH
 *
 *            Sec Class: Intel Confidential (IC)
 *
 *
 * This document contains proprietary information belonging to IMC.
 * Design A/S. Passing on and copying of this document, use and communication
 * of its contents is not permitted without prior written authorisation.
 *
 * Revision Information:
 *  $File name:  /msw_tools/FlashTool/libFlsTool/src/SecPack.h $
 *  $CC-Version: .../dhkristx_140314_v0/1 $
 *  $Date:       2014-04-30    10:42:40 UTC $
 */

#ifndef SEC_PACK_H_
#define SEC_PACK_H_

#include "StructParser.h"

namespace ipa
{
  namespace fls
  {
    static const char sec_pack_json[] =
      "{\n"
      "  \"partition_entry\": [\n"
      "    \"ID                       uint32\",\n"
      "    \"StartBlock               uint32\",\n"
      "    \"EndBlock                 uint32\",\n"
      "    \"MaxBlocksInSubPartition  uint32\",\n"
      "    \"StartReservoirBlock      uint32\",\n"
      "    \"EndReservoirBlock        uint32\",\n"
      "    \"ImageType                uint32\",\n"
      "    \"Options                  uint32\",\n"
      "    \"Reserved                 uint32\"\n"
      "  ],\n"
      "  \"load_map_entry\": [\n"
      "    \"StartAddr                uint32\",\n"
      "    \"TotalLength              uint32\",\n"
      "    \"UsedLength               uint32\",\n"
      "    \"ImageFlags               uint32\"\n"
      "  ],\n"
      "  \"sec_pack\": [\n"
      "    \"SecureBlock0             uint8            256\",\n"
      "    \"SecureBlock1             uint8            256\",\n"
      "    \"Type                     uint32\",\n"
      "    \"DataBlock                uint8            135\",\n"
      "    \"UseAlternatePartition    uint8\",\n"
      "    \"PartitionId              uint32\",\n"
      "    \"CompressLoadMap          uint8            36\",\n"
      "    \"FileHashSha2             uint8            32\",\n"
      "    \"BootCoreVersion          uint32\",\n"
      "    \"EblVersion               uint32\",\n"
      "    \"BootStartAddr            uint32\",\n"
      "    \"BootLength               uint32\",\n"
      "    \"BootSignature            uint32             5\",\n"
      "    \"FileHash                 uint32             5\",\n"
      "    \"PartitionMarker          uint32\",\n"
      "    \"Partition                partition_entry   28\",\n"
      "    \"PartitionReserved        uint32             3\",\n"
      "    \"LoadAddrToPartition      uint32            28\",\n"
      "    \"LoadMagic                uint32\",\n"
      "    \"LoadMap                  load_map_entry     8\"\n"
      "  ]\n"
      "}\n";

    class SecPack : public StructParser
    {
    public:

      /**
       * SecPack Constructor creates a SecPack
       */
      SecPack()
        : StructParser(sec_pack_json, "sec_pack")
      { }

      void dump() { print("sec_pack", 0); };
    };

  }
}

#endif

