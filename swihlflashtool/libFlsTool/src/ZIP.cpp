/*  -------------------------------------------------------------------------
 Copyright (C) 2013-2014 Intel Mobile Communications GmbH

      Sec Class: Intel Confidential (IC)

 All rights reserved.
 -------------------------------------------------------------------------
 This document contains proprietary information belonging to IMC.
 Passing on and copying of this document, use and communication of its
 contents is not permitted without prior written authorization.
 -------------------------------------------------------------------------
 Revision Information:
$File name:  /msw_tools/FlashTool/libFlsTool/src/ZIP.cpp $
$Date:       2014-04-16    12:25:25 UTC $
 ------------------------------------------------------------------------- */

#include "ZIP.h"

#include <zlib.h>

#include "FlsFile.h"
#include "StringExt.h"

#include "FileIO.h"
#include "StructParser.h"
#include "Exceptions.h"

namespace ipa {

/**
 *  The ZIP file Structures
 */
const char zip_json[] =
"{"
  /* Local File Header */
  "\"zip_file\": ["
    "\"Signature               uint32\","
    "\"MinVersion              uint16\","
    "\"Flag                    uint16\","
    "\"Compression             uint16\","
    "\"ModifiedTime            uint16\","
    "\"ModifiedDate            uint16\","
    "\"CRC32                   uint32\","
    "\"CompressedSize          uint32\","
    "\"UnCompressedSize        uint32\","
    "\"NameLength              uint16\","
    "\"ExtraLength             uint16\","
    /* FileName                NameLength  */
    /* ExtraField              ExtraLength */
  "],"
  /* Central directory file header */
  "\"zip_central_dir\": ["
    "\"Signature               uint32\","
    "\"Version                 uint16\","
    "\"MinVersion              uint16\","
    "\"Flag                    uint16\","
    "\"Compression             uint16\","
    "\"ModifiedTime            uint16\","
    "\"ModifiedDate            uint16\","
    "\"CRC32                   uint32\","
    "\"CompressedSize          uint32\","
    "\"UnCompressedSize        uint32\","
    "\"NameLength              uint16\","
    "\"ExtraLength             uint16\","
    "\"CommentLength           uint16\","
    "\"Disk                    uint16\","
    "\"AttrInternal            uint16\","
    "\"AttrExternal            uint32\","
    "\"Offset                  uint32\","
    /* FileName                NameLength  */
    /* ExtraField              ExtraLength */
    /* Comment                 CommentLength */
  "],"
  /* Content Directory - EOF */
  "\"zip_eof\": ["
    "\"Signature               uint32\","
    "\"DiskNr                  uint16\","
    "\"DiskStart               uint16\","
    "\"Records                 uint16\","
    "\"TotalRecords            uint16\","
    "\"Size                    uint32\","
    "\"Offset                  uint32\","
    "\"CommentLen              uint16\","
    /* Comment comes now and is CommentLen bytes long */
  "],"

  "\"zip_data\": ["
    "\"Signature               uint32\","
    "\"CRC32                   uint32\","
    "\"CompressedSize          uint32\","
    "\"UnCompressedSize        uint32\","
  "],"
    "\"zip_data2\": ["
      "\"CRC32                   uint32\","
      "\"CompressedSize          uint32\","
      "\"UnCompressedSize        uint32\","
    "],"
"}";

/**
 * Converts a DOS timestamp to Unix timestamp
 *
 *                24                16                 8                 0
 * +-+-+-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+
 * |Y|Y|Y|Y|Y|Y|Y|M| |M|M|M|D|D|D|D|D| |h|h|h|h|h|m|m|m| |m|m|m|s|s|s|s|s|
 * +-+-+-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+
 *  \___________/\________/\_________/ \________/\____________/\_________/
 *     year        month       day      hour       minute        second
 *
 * @param yr_mon   The Year & Month 16bit representation
 * @param hr_sec   The Hour & Second 16bit representation
 *
 * @return   The resulting Unix timestamp
 */
time_t timestamp_dos_to_unix(uint16_t yr_mon, uint16_t hr_sec)
{
  struct tm stamp;

  stamp.tm_sec = (0x001F & hr_sec);
  stamp.tm_min = (0x003F & (hr_sec>>5));
  stamp.tm_hour = (0x001F & (hr_sec>>11));

  stamp.tm_mday = (0x001F & yr_mon);
  stamp.tm_mon = (0x000F & (yr_mon>>5))-1;
  stamp.tm_year = 80+(0x003F & (yr_mon>>9));

  //cout << " => " << asctime(&stamp) << endl;
  return mktime(&stamp);
}

/**
 *  Return the zlib error from an error code
 **/
static const string zerr(int ret)
{
  switch (ret)
  {
    case Z_ERRNO:         return "I/O error";
    case Z_STREAM_ERROR:  return "Invalid compression level";
    case Z_DATA_ERROR:    return "Invalid or incomplete deflate data";
    case Z_MEM_ERROR:     return "Out of memory";
    case Z_VERSION_ERROR: return "Zlib version mismatch!";
    default:              return "Unknown error";
  }
}

/**
 * The ZIP constructor
 *
 * @param file     The ZIP file
 * @param verbose  Verbosity [0-3]
 */
ZIP::ZIP(string file, int verbose)
  : m_file(file),
    m_file_offset(0),
    m_size(0)
{
  set_verbose(verbose);

  if (!file.empty())
    load(file.c_str());
}

/**
 * The ZIP constructor (for embedded files)
 *
 * @param file     The file containing the ZIP file
 * @param offset   The ZIP file Offset
 * @param size     Size of the embedded file
 * @param verbose  Verbosity [0-3]
 */
ZIP::ZIP(string file, size_t offset, size_t size, int verbose)
  : m_file(file),
    m_file_offset(offset),
    m_size(size)
{
  set_verbose(verbose);

  if (!file.empty())
    load(file.c_str(), offset, size);
}

/**
 * Set Verbosity
 */
void ZIP::set_verbose(int verbose)
{
  verbosity = verbose;
  info.setStream((verbose > 0) ? &cout : 0);
  debug.setStream((verbose > 1) ? &cout : 0);
}

/**
 * Loads the ZIP file from a path
 *
 * @param in   The path to load
 * @param offset  ZIP location in the IN file (default: 0)
 * @param size    Size of the ZIP file (default: file size)
 */
void ZIP::load(string in, size_t offset, size_t size)
{
  if (!fexists(in))
    throw_exception("File "+ in +" does not exist!");

  ifstream zip(in.c_str(), ios::binary | ios::ate);
  if (!zip.is_open())
    throw_exception("Could not open "+ in);

  m_file = in;
  m_size = (size == 0) ? (size_t)zip.tellg() : size;
  m_file_offset = offset;

  debug << " ZIP::load(" << m_file
        << ", Size: " << getHexString(m_size)
        << ", Offset: " << getHexString(m_file_offset)
        << ")" << endl;

  zip.seekg(m_file_offset);

#ifdef ANDROID
  try {
#endif

    //< Loading the embedded files (Metadata only)
    files.clear();
    ZIP::parse(zip);

#ifdef ANDROID
  } catch (std::exception& e) {
    #ifndef __EXCEPTIONS
      myexception& e = chatched_exceptions.front();
    #endif
    zip.close();
    throw_exception(e.what());
  }
#endif
  zip.close();
}

/**
 * Helper - Used to get strings from the stream
 */
string istream_get_string(istream& stream, size_t len)
{
  vector<uint8_t> buffer(len+1);
  stream.read((char*)&buffer[0], len);
  buffer.push_back('\0');
  return string(buffer.begin(), buffer.begin()+len);
}

/**
 * Parses a ZIP file.
 *
 * All embedded file pointers and lengths are saved and can be extracted
 * via the extract function.
 *
 * @return true if parse completed successfully
 */
void ZIP::parse(std::istream& zip)
{
  uint32_t signature;
  StructParser toc(zip_json, "zip_eof");
  StructParser cdir(zip_json, "zip_central_dir");

  bool found_eof = false;
  size_t iterator = 0xffff;
  size_t next_seek = m_file_offset + m_size - toc.size();
  string cd_eof = ZIP_END_OF_DIR_MAGIC;

  info << "  -- ZIP Parser (" << m_file << ") -- " << endl;

  //< 1) Locate Content Directory EOF Magic
  while (!found_eof)
  {
    if (next_seek == 0 || iterator-- == 0)
      throw_exception(m_file +" is not a ZIP file!");

    zip.seekg(next_seek--);
    if (zip.peek() != cd_eof[0])
      continue;

    found_eof = true;
    for (size_t i=0; i < cd_eof.size(); ++i)
      if (zip.get() != cd_eof[i])
        found_eof = false;
  }
  zip.seekg(next_seek+1);

  //< 2) Read Content Directory
  toc.load(zip);

  size_t size = toc.get_uint32("Size");
  size_t offset = toc.get_uint32("Offset");
  size_t total = toc.get_uint16("TotalRecords");

  #if 1
    debug << "Content Directory content:" << endl;
    debug << " Signature:    " << getHexString(toc.get_uint32("Signature")) << endl;
    debug << " DiskNr:       " << toc.get_uint16("DiskNr") << endl;
    debug << " DiskStart:    " << toc.get_uint16("DiskStart") << endl;
    debug << " Records:      " << toc.get_uint16("Records") << endl;
    debug << " TotalRecords: " << total << endl;
    debug << " Size:         " << getHexString(size, 4) << endl;
    debug << " Offset:       " << getHexString(offset, 4) << endl;
    debug << " Comment:      " << getHexString(toc.get_uint16("CommentLen"), 2) << endl;
    debug << endl;
    /* Comment comes now and is CommentLen bytes long */
  #endif

  //< 3) Now read ALL the Central Directories
  zip.seekg(m_file_offset + offset);
  for (size_t i=0; i < total; ++i)
  {
    ZipFile a_file;
    cdir.load(zip);

    signature = cdir.get_uint32("Signature");

    if (signature != ZIP_DIR_SIGNATURE)
      throw_exception("Central directory "+ Stringify(i) +" corrupt!");

    uint16_t yr_mon = cdir.get_uint16("ModifiedTime");
    uint16_t hr_sec = cdir.get_uint16("ModifiedDate");

    a_file.crc = cdir.get_uint32("CRC32");
    a_file.compression = cdir.get_uint16("Compression");
    a_file.size = cdir.get_uint32("UnCompressedSize");
    a_file.compressed_size = cdir.get_uint32("CompressedSize");
    a_file.offset = cdir.get_uint32("Offset");

    a_file.mod_time = timestamp_dos_to_unix(yr_mon, hr_sec);
    a_file.name = istream_get_string(zip, cdir.get_uint16("NameLength"));
    a_file.extra = istream_get_string(zip, cdir.get_uint16("ExtraLength"));

    files.push_back(a_file);

    #if 1
      debug << a_file.name << " loaded: " << endl;
      debug << " Signature:        " << getHexString(signature) << endl;
      debug << " Flag:             " << cdir.get_uint16("Flag") << endl;
      debug << " Compression:      " << a_file.compression << endl;
      debug << " ModifiedTime:     " << getHexString(cdir.get_uint16("ModifiedTime")) << endl;
      debug << " ModifiedDate:     " << getHexString(cdir.get_uint16("ModifiedDate")) << endl;
      debug << " CRC32:            " << getHexString(a_file.crc) << endl;
      debug << " CompressedSize:   " << cdir.get_uint32("CompressedSize") << endl;
      debug << " UnCompressedSize: " << a_file.size << endl;
      debug << " NameLength:       " << cdir.get_uint16("NameLength") << endl;
      debug << " ExtraLength:      " << cdir.get_uint16("ExtraLength") << endl;
      debug << " CommentLength:    " << cdir.get_uint16("CommentLength") << endl;
      debug << " Disk:             " << cdir.get_uint16("Disk") << endl;
      debug << " AttrInternal:     " << getHexString(cdir.get_uint16("AttrInternal")) << endl;
      debug << " AttrExternal:     " << getHexString(cdir.get_uint32("AttrExternal")) << endl;
      debug << " Offset:           " << getHexString(a_file.offset) << endl;
      debug << endl;
    #endif
  }

  info << "  -- FLS Parser END -- " << endl;
}

static void unzip(vector<uint8_t>& in, vector<uint8_t>& out)
{
  //out.assign(compressBound(in.size())+1, 0);

  /* allocate inflate state */
  int ret = 0;
  z_stream strm;
  memset((void*)&strm, 0, sizeof(strm));
  //strm.zalloc = strm.zfree = strm.opaque = Z_NULL;
  //strm.next_in = strm.avail_in = 0;

  /**
   * WindowBits are < 0 -> No zlib header
   */
  ret = inflateInit2(&strm, -MAX_WBITS);
  if (ret != Z_OK)
      throw_exception("inflateInit: "+ zerr(ret));

//  do {
    strm.next_in = (Bytef *)&in[0];
    strm.avail_in = in.size();
    //do {
      strm.next_out = (Bytef *)&out[0];
      strm.avail_out = out.size();
      ret = inflate(&strm, Z_SYNC_FLUSH);

      if (ret >= 0 && strm.msg != NULL)
        throw_exception("ZIP Inflate Error: "+ string(strm.msg));

    //} while (strm.avail_out == 0);
//      break;
//  } while (ret != Z_STREAM_END);

  out.push_back('\0');
  inflateEnd(&strm);
}


/**
 *  Get embedded file (de-crypting while loading)
 *
 * @param i  The index to load
 *
 * @return   Reference to the file
 */
ZipFile& ZIP::get_file(int i)
{
  if (files.at(i).data.empty())
  {
    //cout << " Opening ZIP file: " << m_file << endl;
    ifstream zip(m_file.c_str(), ios::binary | ios::ate);
    if (zip)
    {
      vector<uint8_t> data;
      StructParser toc(zip_json, "zip_file");
      ZipFile& zf = files[i];

      //cout << " Loading data from " << getHexString(files[i].offset) << endl;
      zip.seekg(m_file_offset + zf.offset + toc.size() + zf.name.size());

      data.resize(zf.compressed_size);
      zip.read((char*)&data[0], data.size());
      //cout << " DATA "; hexDump(data);

      zf.data.assign(zf.size, 0);
      unzip(data, zf.data);

      zip.close();
    }
  }

  return files.at(i);
}

/**
 * Locates an embedded file via its name
 *
 * @param name   File name to look for
 *
 * @return    Pointer to the file or NULL if not found
 */
ZipFile* ZIP::operator[](string name)
{
  for(size_t i=0; i < files.size(); ++i)
  {
    if (files[i].name.compare(name) == 0)
      return &get_file(i);
  }
  return NULL;
}

/**
 * Saves a reference to the content of the file
 *
 * @param content   Reference to buffer
 *
 * @return  A pointer to the first char in the buffer
 */
char* ZipFile::get_content(vector<uint8_t>& content)
{
  content = data;
  return (char*)&content[0];
}


} /* namespace ipa */
