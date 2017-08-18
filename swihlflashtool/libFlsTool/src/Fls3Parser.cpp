/* -------------------------------------------------------------------------
 Copyright (C) 2013-2014 Intel Mobile Communications GmbH

      Sec Class: Intel Confidential (IC)
 ----------------------------------------------------------------------
 Revision Information:
    $File name:  /msw_tools/FlashTool/libFlsTool/src/Fls3Parser.cpp $
    $CC-Version: .../dhkristx_140314_v0/1 $
    $Date:       2014-04-22    9:13:38 UTC $
 ----------------------------------------------------------------------
 by CCCT (0.12h)
 ---------------------------------------------------------------------- */

#include "Fls3Parser.h"
#include "Fls3MetaParser.h"

#include "JSON.h"
#include "FileIO.h"
#include "StructParser.h"

// #include <time.h>       /* time_t, struct tm, time, localtime, strftime */

/* Intel Platform Abstraction - Fls */
namespace ipa {
namespace fls {

static uint64_t parseOctal(vector<uint8_t> buf);
static uint32_t compute_checksum(vector<uint8_t>& buf);
static vector<uint8_t>& getOctalBytes(uint64_t value, vector<uint8_t>& buf);

const char tar_header_json[] =
"{"
/* ElementHeaderStructType */
  "\"tar_header\": ["
    "\"Name                      uint8                100\","
    "\"Mode                      uint8                  8\","
    "\"UID                       uint8                  8\","
    "\"GID                       uint8                  8\","
    "\"Size                      uint8                 12\","
    "\"ModTime                   uint8                 12\","
    "\"Checksum                  uint8                  8\","
    "\"LinkFlag                  uint8                  2\","
    "\"LinkName                  uint8                99\","
    "\"Magic                     uint8                  6\","
    "\"MagicVer                  uint8                  2\","
    "\"UserName                  uint8                 32\","
    "\"GroupName                 uint8                 32\","
    "\"DevMajor                  uint8                  8\","
    "\"DevMinor                  uint8                  8\","
    "\"NULL                      uint8                167\","
  "],"
"}";

/**
 *  The Intel Fls3 Parser Class
 *
 * @param ref        The reference to save to
 * @param file       The file to load
 * @param verbose    Verbosity during parsing [0-2]
 */
Fls3Parser::Fls3Parser(FlsMetaFile& ref, string file, int verbose)
        : m_ref(ref),
          verbosity(verbose)
{
  m_ref.version      = 3;
  m_ref.fls_version  = "3.0";

  set_verbose(verbosity);

  if (!file.empty())
    load(file.c_str());
}

/**
 * Set Verbosity
 */
void Fls3Parser::set_verbose(int verbose)
{
  verbosity = verbose;
  info.setStream((verbose > 0) ? &cout : 0);
  debug.setStream((verbose > 1) ? &cout : 0);
}

/**
 * Loads the Fls3 file from a path
 */
void Fls3Parser::load(const char* file)
{
  if (!fexists(file))
    throw_exception("File "+ string(file) +" does not exist!");

  m_ref.m_fls_file   = file;
  m_ref.date_created = GetTimestamp(fcreated(file));

  ifstream fls(file, fstream::binary);
  if (!fls.is_open())
    throw_exception("Could not open "+ string(file));

  try
  {
    //< Loading the embedded files
    Fls3Parser::fls3_parse(fls);

    if (meta_data.size() == 0)
      throw_exception("Meta data missing from "+ string(file));

    //< Parsing the meta data
    Fls3MetaParser(m_ref, meta_data, verbosity - 2);

    vector<FlsEmbedFile*> list = m_ref.get_file_list();
    if (embed_files.size() != (list.size()+1)) // + meta.json in embed_files
      throw_exception("JSON says "+ Stringify(list.size())
                     +" files. Actual files in FLS is "
                     + Stringify(embed_files.size()));

    // Linking EmbedFiles <-> FlsStructure
    // Don't link meta.json or files with size 0
    for (size_t i=0; i < embed_files.size(); ++i)
    {
      FlsEmbedFile *file = embed_files[i];
      string filename = file->name();
      if (filename.compare(META_FILE_NAME) != 0)
      {
        FlsEmbedFile *sfile = m_ref.find(filename);
        if (file->size() > 0 || (sfile && sfile->size() > 0))
          sfile->setup(file);
      }
    }

  }
  catch (std::exception& e)
  {
    #ifndef __EXCEPTIONS
      myexception& e = chatched_exceptions.front();
    #endif
    fls.close();
    throw_exception(e.what());
  }

  fls.close();
}


/**
 * Saves an Fls3/TAR file to a path
 *
 * @param ref      Reference to meta data file
 * @param file     Output file to save
 * @param verbose  Verbosity during the save [0-2]
 */
void Fls3Parser::save(FlsMetaFile& ref, string output, int verbose)
{
  vector<uint8_t> empty_checksum(8, ' ');
  StructParser header(tar_header_json, "tar_header");

  vector<FlsEmbedFile*> list = ref.get_file_list();

  if (verbose)
    cout << " Saving " << list.size() << " files in " << output << endl;

  ofstream fls(output.c_str(), fstream::binary);
  if (!fls.is_open())
    throw_exception("Could not open "+ string(output) +": "+ string(strerror( errno )));

  for (size_t i=0; i < list.size(); ++i)
  {
    vector<uint8_t> buf(12, 0);
    FlsEmbedFile *file = list[i];

    if (!file)
      throw_exception("Embedded file list corrupt at "+ Stringify(i));

    header.clear();
    header.set_string("Name", file->name());
    header.set_string("Mode", "000666 ");
    header.set_string("UID", "000000 ");  //< TODO: Save from last load?
    header.set_string("GID", "000000 ");  //< TODO: Save from last load?
    header.set_uint8_array("Size",    getOctalBytes(file->size(), buf));
    header.set_uint8_array("ModTime", getOctalBytes(file->modified(), buf));
    header.set_uint8_array("Checksum", empty_checksum);
    header.set_uint8("LinkFlag", TAR_TYPE_NORMAL);
    header.set_string("Magic", "ustar");
    buf.assign(2, '0');
    header.set_uint8_array("MagicVer", buf);
    header.set_string("UserName", file->username());
    header.set_string("GroupName", file->username()); //< TODO: Use?

    buf.assign(7, ' ');
    uint32_t crc = compute_checksum(header.buffer());
    header.set_uint8_array("Checksum", getOctalBytes(crc, buf));
    header.set_uint8("Checksum", 0, 6);
    header.set_uint8("Checksum", ' ', 7);

    if (verbose > 1)
      cout << " [" << setw(8) << right << file->size() << "]"
           << "  " << setw(42) << left << file->name()
           << " by " << file->username() << " (" << file->modified_str() << ")"
           << endl;

    //< Save the Fls3/TAR header
    fls.write((const char*)&header.buffer()[0], header.size());

    //< Save the file content + padding
    file->save_to_stream(fls);

    int padding = file->size() % TAR_DATA_BLOCK;
    if (padding > 0)
    {
      buf.assign(TAR_DATA_BLOCK - padding, 0);
      fls.write((const char*)&buf[0], buf.size());
    }
  }

  vector<uint8_t> eof_block(1024);
  fls.write((const char*)&eof_block[0], eof_block.size());

  fls.close();
}

/**
 * Parses an Fls3/TAR file.
 *
 * All embedded file pointers and lengths are saved and can be extracted
 * via the extract function.
 *
 * @return true if parse completed successfully
 */
void Fls3Parser::fls3_parse(std::ifstream& fls)
{
  string name, username, magic;
  uint32_t size, checksum;
  time_t modtime;

  size_t next_header = 0;
  string& file = m_ref.m_fls_file;
  vector<uint8_t> empty_checksum(8, ' ');
  StructParser header(tar_header_json, "tar_header");

  info << "  -- FLS Parser (" << file << ") -- " << endl;

  do {
    debug << "   Loading next header at " << getHexString(next_header) << endl;
    fls.seekg( next_header );
    header.load(fls);

    debug << "   Reading header..." << endl;
    name     = header.get_string("Name");
    username = header.get_string("UserName");
    magic    = header.get_string("Magic");
    modtime  = parseOctal(header.get_uint8_array("ModTime"));
    size     = parseOctal(header.get_uint8_array("Size"));
    checksum = parseOctal(header.get_uint8_array("Checksum"));

    if (name.empty() && size == 0)
    {
      debug << "   No name or size - assuming EOF" << endl;
      break;
    }

    info << " Parsing " << name
         << " (by " << username << ", " << size << ")" << endl;

    // Magic test
    if (magic.compare(0,5,"ustar") != 0 && magic.compare(0,5,"fls 3") != 0)
      throw_exception("Corrupt Fls3 file["+magic+"]");

    // Checksum test
    header.set_uint8_array("Checksum", empty_checksum);

    if (compute_checksum(header.buffer()) != checksum)
      throw_exception("Checksum error in file "+ m_ref.m_fls_file);

    int extra = size % TAR_DATA_BLOCK;
    int padding = extra ? TAR_DATA_BLOCK - extra : 0;
    next_header += header.size() + size + padding;

    // Save Embedded file info
    FlsEmbedFile* embed = new FlsEmbedFile(file, fname(name), fls.tellg(), size);
    embed->username(username);
    embed->modified(modtime);
    embed_files.push_back( embed );

    // Save the content of the meta data file
    if (name.compare("meta.json") == 0)
      embed->get_content(meta_data);

  } while (!fls.eof());

  info << "  -- FLS Parser END -- " << endl;
}


/**
 *  Calculates the checksum
 *
 * @param buf  The buffer to calculate from
 *
 * @return     The calculated checksum
 */
static uint32_t compute_checksum(vector<uint8_t>& buf)
{
  uint32_t crc = 0;

  for (size_t i=0; i<buf.size(); ++i)
    crc += (uint32_t)buf[i];

  return crc;
}

/**
 * Parses Octal -> uint64
 *
 * @param buf   The octal buffer to parse
 *
 * @return      The uin64 deduced value
 */
static uint64_t parseOctal(vector<uint8_t> buf)
{
  uint64_t result = 0;
  bool padding = true;

  for (size_t i=0; i < buf.size(); ++i)
  {
    if (buf[i] == 0)
      break;

    if (buf[i] == ' ' || buf[i] == '0')
    {
      if (padding)
        continue;

      if (buf[i] == ' ')
        break;
    }

    padding = false;
    result = (result << 3) + (buf[i] - '0');
  }

  return result;
}

/**
 * Parses uint64 -> Octal
 *
 * @param value   The value to parse
 * @param buf     Buffer to save the octals in
 *
 * @return        The buf reference
 */
static vector<uint8_t>& getOctalBytes(uint64_t value, vector<uint8_t>& buf)
{
  int64_t idx = buf.size() - 1;

  // buf.at(idx--) = 0;
  buf.at(idx--) = ' ';

  if (value == 0)
  {
    buf[idx--] = '0';
  }
  else
  {
    for (size_t val = value; idx >= 0 && val > 0; --idx)
    {
      buf[idx] = '0' + (val & 7);
      val = val >> 3;
    }
  }

  while (idx >= 0)
    buf[idx--] = '0';

  return buf;
}


  } /* namespace fls */
} /* namespace ipa */

