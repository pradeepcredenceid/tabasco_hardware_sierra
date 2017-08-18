/* ---------------------------------------------------------------------------
  Copyright (C) 2013-2014 Intel Mobile Communications GmbH

       Sec Class: Intel Confidential (IC)

  All rights reserved.
  ----------------------------------------------------------------------------
  Description:
  Converter functions for converting between various formats.
  ----------------------------------------------------------------------------
  Revision Information:
  $File name:  /msw_tools/FlashTool/libFlsTool/src/Convert.cpp $
  $CC-Version: .../dhkristx_140314_v0/1 $
  $Date:       2014-04-09    13:19:51 UTC $
  ------------------------------------------------------------------------- */

#include "Convert.h"
#include "FileIO.h"          //< Saving binary to file via FILE_setContent()
#include "StringExt.h"       //< For debugging output
#include "Exceptions.h"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <algorithm>

using namespace std;

#define debug     if (verbose > 0) cout << " ConvertHex> "
#define debug2    if (verbose > 1) cout << " ConvertHex> "
#define debug3    if (verbose > 2) cout << " ConvertHex> "
#define debug4    if (verbose > 3) cout << " ConvertHex> "

// Pre-declarations
static bool ihex_sort(ihex_t H1, ihex_t H2);
static vector<ihex_t>& loadIntelHexFile(string file, vector<ihex_t>& iHexVector, int verbose);
static void ConvertHexToBuffer(vector<ihex_t>& iHex, vector<uint8_t>& buffer, uint64_t start_addr, int verbose);


/**
 * Constructor - Parses a file in Intel Hex format
 *
 * @param in   Name of input file (Intel Hex file)
 */
IntelHexFile::IntelHexFile(string in, int verbose)
  : verbose(verbose),
    start_addr(0)
{
  if (!in.empty())
    loadIntelHexFile(in, iHex, verbose);
}

/**
 * Constructor - Parses a list of Intel Hex files
 *
 * @param in   Input file list
 */
IntelHexFile::IntelHexFile(vector<string> in, int verbose)
  : verbose(verbose),
    start_addr(0)
{
  for (size_t i=0; i < in.size(); ++i)
    loadIntelHexFile(in[i], iHex, verbose);
}

/**
 * Append to the HEX list
 *
 * @param in   Input file list
 */
void IntelHexFile::add(string in)
{
  loadIntelHexFile(in, iHex, verbose);
}

/**
 * Converts the iHex to binary and saves it to a file
 *
 * @param out  Name of output file (binary file). The binary data will be
 *             written in file with this name. If file already exists, it
 *             will be overwritten.
 */
void IntelHexFile::save(string out, bool overwrite)
{
  std::vector<uint8_t> buffer;

  if (fexists(out))
  {
    if (!overwrite)
      throw_exception("Output file '"+ out +"' exists");

    if (remove(out.c_str()))
      throw_exception("Error overwriting "+ out);
  }

  ConvertHexToBuffer(iHex, buffer, start_addr, verbose);
  FILE_setContent(out, buffer);
}

/**
 * Converts the iHex to binary and saves it to a buffer
 *
 * @param out  Output buffer reference
 */
void IntelHexFile::save(vector<uint8_t>& buffer)
{
  ConvertHexToBuffer(iHex, buffer, start_addr, verbose);
}

/**
 * Returns the base address of the stored memory
 *
 * @return The iHex base address
 */
uint64_t IntelHexFile::get_base_addr()
{
  if (start_addr == 0 && iHex.size() > 0)
  {
    // Sorting via the address field & use the first address
    std::sort(iHex.begin(),iHex.end(), ihex_sort);
    start_addr = iHex.front().addr;
  }

  return start_addr;
}

/**
 * Assigns a new start address
 *
 * This might cut the code & prints a warning if code is removed.
 *
 * @param addr The new iHex base address
 */
void IntelHexFile::set_base_addr(uint64_t addr)
{
  start_addr = addr;

  if (iHex.size() > 1)
  {
    // Sorting via the address field
    std::sort(iHex.begin(),iHex.end(), ihex_sort);

    debug << " Setting iHex StartAddr: " << getHexString(addr)
          << ", Current StartAddr: " << getHexString(start_addr)
          << ", iHex[0] StartAddr: " << getHexString(iHex[0].addr) << endl;

    size_t i;
    for (i=0; i < iHex.size(); ++i)
    {
      if (iHex[i].addr >= start_addr)
      {
        debug << i << ": " << getHexString(iHex[i].addr) << " >= " << getHexString(start_addr) << endl;
        debug2 << "Prev: " << getHexString(iHex[i-1].addr) << " > " << getHexString(start_addr) << endl;
        break;
      }
    }

    if (i > 0)
    {
      if (i > 1)
      {
        //throw_exception("Start Address ("+ getHexString(addr) +") overlaps the binary sections of the hex file");
        cout << endl << "   Warning> !!! StartAddr (" << getHexString(addr) << ") removing " << i << " starting iHex lines !!!" << endl << endl;
        iHex.erase(iHex.begin()+1, iHex.begin()+i);
      }

      // First element is reserved to assure the right start address
      iHex[0].hexString = ":1000000000000000000000000000000000000000F0";
      iHex[0].type = HEX_TYPE_EXT_SEG_ADDR;
      iHex[0].len = iHex[0].crc = 0;
      iHex[0].addr = addr;
    }
  }
}

/**
 * Assigns the maximum allowed image length
 *
 * This might cut the code & prints a warning if code is removed.
 *
 * @param addr The maximum allowed length
 */
void IntelHexFile::set_max_length(uint64_t length)
{
  max_length = length;

  if (iHex.size() > 1)
  {
    start_addr = get_base_addr();

    size_t i;
    size_t max_addr = length + start_addr;
    debug << " Setting iHex MaxAddr: " << getHexString(max_addr) << endl;

    for (i = iHex.size()-1; i>0; --i)
    {
      size_t end_addr = (iHex[i].addr + iHex[i].len);
      if (end_addr <= max_addr)
      {
        debug << i << ": " << getHexString(end_addr) << " > " << getHexString(max_addr) << endl;
        // debug2 << "Prev: " << getHexString(iHex[i+1].addr + iHex[i+1].len) << " > " << getHexString(max_addr) << endl;
        break;
      }
    }

    if (i < iHex.size()-1)
    {
      // throw_exception(" Warning> !!! Removing "+ Stringify(iHex.size()-i) +" lines from the iHex file !!!");
      cerr << endl << "   Warning> !!! MaxLength (" << getHexString(length) << ") removing " << (iHex.size()-i) << " ending iHex lines !!!" << endl << endl;
      iHex.erase(iHex.begin()+i+1, iHex.end()-1);

      // Last element is reserved to assure zero-padding to the right end address
      i = iHex.size()-1;
      iHex[i].hexString = ":1000000000000000000000000000000000000000F0";
      iHex[i].type = HEX_TYPE_EXT_SEG_ADDR;
      iHex[i].len = iHex[i].crc = 0;
      iHex[i].addr = max_addr;
    }
/*
    //< In case the MaxLength should be zero-padded
    else if (iHex[i].addr != max_addr)
    {
      i++;
      ihex_t last_addr;
      iHex.push_back(last_addr);
    }
*/
  }
}


/**
 * Wrapper for calling via ANSI C
 */
void ConvertHexToBin(const char *in, const char *out, int verbose)
{
  try
  {
    IntelHexFile(string(in), verbose).save(string(out));
  }
  catch (exception& e)
  {
    #ifndef __EXCEPTIONS
      myexception& e = chatched_exceptions.front();
    #endif
    cerr << e.what() << endl;
  }
}


/**
 *  Function for sorting the vector via the iHex address field
 */
static bool ihex_sort(ihex_t H1, ihex_t H2)
{
  return (H1.addr < H2.addr);
}

/**
 *  Converting Hex to its number
 *  This function have been optimized for speed
 */
inline uint64_t HexToChar(const char *p, int size)
{
  uint8_t data[8];
  uint64_t value = 0;

  for (int j=0;j<size;++j)
  {
    switch(*p++)
    {
      case '0': data[j] = 0; break;
      case '1': data[j] = 1; break;
      case '2': data[j] = 2; break;
      case '3': data[j] = 3; break;
      case '4': data[j] = 4; break;
      case '5': data[j] = 5; break;
      case '6': data[j] = 6; break;
      case '7': data[j] = 7; break;
      case '8': data[j] = 8; break;
      case '9': data[j] = 9; break;
      case 'A': data[j] = 10; break;
      case 'B': data[j] = 11; break;
      case 'C': data[j] = 12; break;
      case 'D': data[j] = 13; break;
      case 'E': data[j] = 14; break;
      case 'F': data[j] = 15; break;
      default:
        throw_exception("Character is not Hex!", 0);
    }
  }

  switch (size)
  {
    case 2:
      value = data[0]*16 + data[1];
      break;

    case 4:
      value  = (data[0]*16) << 8;
      value += (data[1]*16) << 4;
      value += (data[2]*16);
      value += data[3];
      break;

    case 8:
      value += (data[0]*16)<< 28;
      value += (data[1]*16)<< 24;
      value += (data[2]*16)<< 20;
      value += (data[3]*16)<< 16;
      value += (data[4]*16)<< 12;
      value += (data[5]*16)<< 8;
      value += (data[6]*16)<< 4;
      value += data[7];
      break;
  }

  return value;
}

/**
 * Parses a single line of Intel HEX
 *
 * @param line   An Intel HEX line
 * @param iHex   The iHex type to parse into
 *
 * @return       Reference to the input iHex
 */
static ihex_t& loadHexLine(string line, ihex_t& iHex)
{
  uint8_t CRC;
  const char* ptr = line.c_str();

  if (*ptr++ != ':')
    throw_exception("Invalid hex input file", iHex);

  iHex.hexString = line;
  iHex.len = HexToChar(ptr, 2);

  if (iHex.len > 32)
    throw_exception("iHex.length too long! I only support lenghts of 16, this is "+ itos(iHex.len), iHex);

  iHex.addr = HexToChar(ptr+2, 4);
  iHex.type = HexToChar(ptr+6, 2);
  iHex.crc = HexToChar(ptr+iHex.len*2+8, 2);

  CRC = iHex.len + iHex.type;
  CRC += (uint8_t)(iHex.addr & 0xFF);
  CRC += (uint8_t)((iHex.addr >> 8) & 0xFF);

  ptr += 8;
  for(int i=0; i < iHex.len; ++i)
  {
    iHex.data[i] = HexToChar(ptr, 2);
    CRC += iHex.data[i];
    ptr += 2;
  }
  CRC = 0 - CRC; // Final checksum

  if (CRC != iHex.crc)
    throw_exception(" Checksum error ["+ itos(CRC) +" != "+ itos(iHex.crc) +"]", iHex);

  return iHex;
}

/**
 * Loads and sorts an Intel HEX file into a vector.
 *
 * @param file        The input iHex file
 * @param iHexVector  The vector of iHex types to parse into
 * @param start_addr  Pointer to
 *
 * @return       A vector of data entries
 */
static vector<ihex_t>& loadIntelHexFile(string file, vector<ihex_t>& iHexVector, int verbose)
{
  string line;
  ihex_t iHex;
  uint64_t line_nr = 0;
  uint64_t base_addr = 0;

  try
  {
    ifstream in(file.c_str(), ifstream::binary);
    if (!in)
      throw_exception("Could not open input HEX file: "+ file, iHexVector);

    /* If first line starts with data assume (StartAddr==0) */
    getline(in, line);
    loadHexLine(line, iHex);

    if ((HexType)iHex.type == HEX_TYPE_DATA)
    {
      ihex_t start;
      start.addr = start.len = start.crc = 0;
      start.type = HEX_TYPE_EXT_SEG_ADDR;
      iHexVector.push_back( start );
      // cout << " Start Address => 0 " << endl;
    }
    in.seekg(0);

    /* Parses all lines */
    while (getline(in, line))
    {
      line_nr++;
      loadHexLine(line, iHex);
      iHex.addr += base_addr;

      switch((HexType)iHex.type)
      {
      case HEX_TYPE_EOF:
        in.close();
        return iHexVector;

      case HEX_TYPE_DATA:
        debug3 << line_nr << " Data (" << getHexString(iHex.addr) << ")" << line << endl;
        iHexVector.push_back( iHex );
        break;

      case HEX_TYPE_EXT_SEG_ADDR:
        base_addr = (((uint64_t) iHex.data[0]) << 12)
                  + (((uint64_t) iHex.data[1]) << 4);
        debug2 << line_nr << " BaseAddr (Type2)   == " << getHexString(base_addr)
              << " Line: " << line << endl;
        break;

      case HEX_TYPE_EXT_ADDR:
        base_addr = (((uint64_t) iHex.data[0]) << 24)
                  + (((uint64_t) iHex.data[1]) << 16);
        debug2 << line_nr << " BaseAddr (Type4)   == " << getHexString(base_addr)
              << " Line: " << line << endl;
        break;

      case HEX_TYPE_START_ADDR:
        base_addr = (((uint64_t) iHex.data[0]) << 24)
                  + (((uint64_t) iHex.data[1]) << 16);
        debug2 << line_nr << " BaseAddr (Type5)   == " << getHexString(base_addr)
              << " Line: " << line << endl;
        break;

      case HEX_TYPE_START_SEG_ADDR:
        //start_addr = (((uint64_t) iHex.data[0]) << 24)
        //           + (((uint64_t) iHex.data[1]) << 16)
        //           + (((uint64_t) iHex.data[2]) << 8)
        //           + (((uint64_t) iHex.data[3]) & 0xFF);
        debug2 << line_nr << " StartAddr (Type3)  == " << getHexString(iHex.addr)
              << " (IGNORING)" << endl;
        break;

      default:
        throw_exception("iHex type '"+ Stringify(iHex.type) +"' not supported", iHexVector);
        break;
      }
    }
    in.close();
  }
  catch (exception& e)
  {
    #ifndef __EXCEPTIONS
      myexception& e = chatched_exceptions.front();
    #endif
    throw_exception(string(e.what())+" on line "+ Stringify(line_nr) +" ["+ line+"]", iHexVector);
  }

  // Sorting via the address field
  //std::sort(iHexVector.begin(),iHexVector.end(), ihex_sort);

  return iHexVector;
}

/**
 *  Saves a vector of HEX to a binary buffer.
 *  In case two consecutive buffers have a gap, this gap will be
 *  filled out with NULLs.
 *
 *  @param iHex     The Intel Hex vector
 *  @param buffer   The buffer of to fill
 */
static void ConvertHexToBuffer(vector<ihex_t>& iHex, vector<uint8_t>& buffer, uint64_t start_addr, int verbose)
{
  uint64_t addr, size, pos = 0;

  // Sorting via the address field
  std::sort(iHex.begin(),iHex.end(), ihex_sort);

  if (start_addr == 0)
    start_addr = iHex.front().addr;
  size = (iHex.back().addr + iHex.back().len) - start_addr;

  debug << "size"
        << ":  " << getHexString(iHex.back().addr)
        << " - " << getHexString(start_addr)
        << " + " << getHexString(iHex.back().len)
        << " = " << getHexString(size) << endl;

  buffer.resize( size );

  for (size_t i=0; i < iHex.size(); ++i)
  {
    size = iHex[i].len;
    addr = iHex[i].addr - start_addr;
    uint8_t* data = iHex[i].data;

    if (pos < addr)
    {
      debug2 << "Filling Zeros to " << getHexString(addr) << endl;

      std::fill_n(buffer.begin()+pos, addr-pos, 0);
      pos = addr;
    }

    debug4 << "Filling from: "
           << getHexString(addr) << ": " << iHex[i].hexString << endl;

    std::copy(data, data+size, &buffer[pos]);
    pos += size;
  }
}
