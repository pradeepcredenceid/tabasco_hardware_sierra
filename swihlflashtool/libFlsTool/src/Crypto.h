/* -------------------------------------------------------------------------
 Copyright (C) 2014 Intel Mobile Communications GmbH

      Sec Class: Intel Confidential (IC)
 ----------------------------------------------------------------------
 Revision Information:
    $File name:  /msw_tools/FlashTool/libFlsTool/src/Crypto.h $
    $CC-Version: .../dhkristx_140314_v0/2 $
    $Date:       2014-04-15    12:02:50 UTC $
 ----------------------------------------------------------------------
 by CCCT (0.12l)
 ---------------------------------------------------------------------- */

#ifndef CRYPTO_H
#define CRYPTO_H

#include <stdint.h>

#ifdef __cplusplus

#include <string>
#include <vector>

using namespace std;

namespace ipa
{
  namespace crypto
  {
    /**
     * Maximum file input buffer size
     */
    const uint32_t buf_size = 1024*1024;

    /**
     * Supported algorithms
     */
    typedef enum
    {
      HASH_SHA1,
      HASH_XOR16,
      HASH_XOR32,
      HASH_CRC8,
      HASH_CRC16,
      HASH_SHA256
    } Algorithm;

    /**
     * Structure used for converting between data sizes
     */
    typedef union {
      uint8_t data[8];
      uint16_t uint16[4];
      uint32_t uint32[2];
      uint64_t uint64;
    } data_convert_t;

    /**
     * Convert an Algorithm name to a string
     */
    string to_string(Algorithm algorithm);

    /**
     * Creating Hash from a file (XOR16)
     *
     * @param path  The path of the file to hash
     * @return      The XOR16 hash
     */
    uint64_t get_hash(string path, Algorithm algorithm);

    /**
     * Creating Hash from a buffer
     *
     * @param content    Buffer with content to hash
     * @param algorithm  The hash algorithm to run (XOR32, XOR16, or XOR8)
     *
     * @return           The calculated hash
     */
    uint64_t get_hash(vector<uint8_t>& content, Algorithm algorithm);
  }
}

// TODO: Add the ANSI porting code here:

#endif

#endif
