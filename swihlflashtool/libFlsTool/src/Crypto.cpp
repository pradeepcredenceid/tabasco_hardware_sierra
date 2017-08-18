/* -------------------------------------------------------------------------
 Copyright (C) 2014 Intel Mobile Communications GmbH

      Sec Class: Intel Confidential (IC)
 ----------------------------------------------------------------------
 Revision Information:
    $File name:  /msw_tools/FlashTool/libFlsTool/src/Crypto.cpp $
    $CC-Version: .../dhkristx_140314_v0/2 $
    $Date:       2014-04-15    12:02:47 UTC $
 ----------------------------------------------------------------------
 by CCCT (0.12l)
 ---------------------------------------------------------------------- */

#include "Crypto.h"
#include "StringExt.h"
#include "Exceptions.h"

#include <fstream>
#include <iostream>

using namespace std;

namespace ipa
{
  namespace crypto
  {

/**
 * Convert an Algorithm name to a string
 */
string to_string(Algorithm algorithm)
{
  switch(algorithm)
  {
    case HASH_SHA1:   return string("sha1");
    case HASH_XOR16:  return string("xor16");
    case HASH_CRC8:   return string("crc8");
    case HASH_CRC16:  return string("crc16");
    case HASH_SHA256: return string("sha256");
    default:
      throw_exception("getHashAlgorithm "+ getIntString(algorithm) +" does not exist", "");
      break;
  }
  return string("");
}


/**
 * Creating Hash from a file (XOR16)
 *
 * @param path  The path of the file to hash
 * @return      The XOR16 hash
 */
uint64_t get_hash(string path, Algorithm algorithm)
{
  uint32_t hash = 0;
  vector<uint8_t> buffer(buf_size);

  ifstream in(path.c_str(), ios::binary | ios::ate);
  if (!in.is_open())
    throw_exception("Could not open "+ path, 0);

  uint64_t left = in.tellg();
  in.seekg(0);

  while (left > buf_size)
  {
    in.read((char*)&buffer[0], buf_size);
    if (in.bad())
      throw_exception("Failed reading from file (" + path + ")", 0);

    left -= in.gcount();
    hash ^= get_hash(buffer, algorithm);
  }
  buffer.resize(left);
  in.read((char*)&buffer[0], left);
  if (in.bad())
    throw_exception("Failed reading from file (" + path + ")", 0);

  hash ^= get_hash(buffer, algorithm);
  in.close();

  return hash;
}

/**
 * Creating Hash from a buffer
 *
 * @param content    Buffer with content to hash
 * @param algorithm  The hash algorithm to run (XOR32, XOR16, or XOR8)
 *
 * @return           The calculated hash
 */
uint64_t get_hash(vector<uint8_t>& content, Algorithm algorithm)
{
  uint64_t hash = 0;
  data_convert_t conv;

  uint8_t extra = 4-(content.size() % 4);
  if (extra < 4)
    content.insert(content.end(), extra, 0);

  for (size_t i=0; i<(content.size()-1);)
  {
    conv.data[0] = content[i++];
    conv.data[1] = content[i++];
    conv.data[2] = content[i++];
    conv.data[3] = content[i++];
    hash ^= conv.uint32[0];
  }

  conv.uint32[0] = hash;

  if (extra < 4)
    content.erase( content.end() - (extra) );

  if (algorithm == HASH_XOR32)
    return hash;
  else if (algorithm == HASH_XOR16)
    return (conv.uint16[0]^conv.uint16[1]);
  else if (algorithm == HASH_CRC8)
    return (conv.data[0]^conv.data[1]^conv.data[2]^conv.data[3]);
  else
    throw_exception("Hash algorithm ("+ Stringify((int)algorithm) +") not supported", 0);
}

  } /* namespace ipa::crypto */
} /* namespace ipa */

