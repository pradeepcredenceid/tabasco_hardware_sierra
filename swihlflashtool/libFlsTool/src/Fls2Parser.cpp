/* -------------------------------------------------------------------------
 Copyright (C) 2013-2014 Intel Mobile Communications GmbH

      Sec Class: Intel Confidential (IC)
 ----------------------------------------------------------------------
 Revision Information:
    $File name:  /msw_tools/FlashTool/libFlsTool/src/Fls2Parser.cpp $
    $CC-Version: .../dhkristx_140314_v0/2 $
    $Date:       2014-05-01    13:43:22 UTC $
 ----------------------------------------------------------------------
 by CCCT (0.12h)
 ---------------------------------------------------------------------- */

#include "Fls2Parser.h"
#include "GenericFile.h"
#include "FlsMetaFile.h"
//#include "Fls2MetaParser.h"

#include "FlsFile.h"           //< FlsFile_get_hash()
#include "FileIO.h"
#include "StructParser.h"

using namespace ipa::fls;
using namespace ipa::crypto;

/* Intel Platform Abstraction - Fls */
namespace ipa {
namespace fls {


/**
 *  Generates an output filename according to the origin file, UID, and type
 *
 *  @param filename   Origin filename
 *  @param UID        The region UID
 *  @param type       Name of the type of the file
 *  @param extra      If extra text is appended (empty string by default)
 *
 *  @return           The generated name: <filename>_ID<UID>_<type>_<extra>
 */
string generateFileName(string filename, int UID, string type, string extra)
{
  size_t pos;
  stringstream name;

  while ((pos = type.find(' ')) != string::npos)
    type[pos] = '_';

  if (!filename.empty())
    name << filename << "_";

  if (UID >= 0)
    name << "ID" << UID << "_";
  else
    name << "inj_";

  name << StringToUpper( type ) << "_" << extra;

  return name.str();
}

/**
 * The JSON structure of the Fls2 format
 */
const char fls2_header_json[] =
"{"
/* ElementHeaderStructType */
  "\"header\": ["
    "\"Type                      uint32\","
    "\"Size                      uint32\","
    "\"UID                       uint32\","
  "],"
"}";

const char fls2_json[] =
"{"
/* ElementHeaderStructType */
  "\"header\": ["
    "\"Type                      uint32\","
    "\"Size                      uint32\","
    "\"UID                       uint32\","
  "],"
/* GenericHeaderDataStructType */
  "\"body\": ["
    "\"FileType                  uint32\","
    "\"Marker                    uint32\","
    "\"Sha1Digest                uint32                 5\","
  "],"
/* ToolVersionStructType */
  "\"tool_version\": ["
    "\"Required                  uint32\","
    "\"Used                      uint32\","
  "],"
/* SG3ChipSelectStructType */
  "\"sg3_chip\": ["
    "\"ChipSelectNumber          uint32\","
    "\"BUSRCON                   uint32\","
    "\"BUSWCON                   uint32\","
    "\"BUSRP                     uint32\","
    "\"BUSWP                     uint32\","
  "],"
/* HardwareStructType */
  "\"hardware\": ["
    "\"Platform                  uint32\","
    "\"SystemSize                uint32\","
    "\"BootSpeed                 uint32\","
    "\"HandshakeMode             uint32\","
    "\"UsbCapable                uint32\","
    "\"FlashTechnology           uint32\","
    "\"SG3ChipSelect             sg3_chip          3\","
    "\"PADDING                   uint8                  4\","
    "\"ProjectName               uint8                 20\","
    "\"DepTemplate               uint8                 32\","
    "\"CfgTemplate               uint8                 32\","
  "],"
/* MemoryMapEntryStructType */
  "\"map_entry\": ["
    "\"Start                    uint32\","
    "\"Length                   uint32\","
    "\"Class                    uint32\","
    "\"Flags                    uint32\","
    "\"FormatSpecify            uint8                 16\","
  "],"
/* MemoryMapStructType */
  "\"memory_map\": ["
    "\"Entry                    map_entry        50\","
    "\"EepRevAddrInSw           uint32\","
    "\"EepRevAddrInEep          uint32\","
    "\"EepStartModeAddr         uint32\","
    "\"NormalModeValue          uint32\","
    "\"TestModeValue            uint32\","
    "\"SwVersionStringLocation  uint32\","
    "\"FlashStartAddr           uint32\","
  "],"
/* TocEntryStructType */
  "\"toc_entry\": ["
    "\"UID                      uint32\","
    "\"MemoryClass              uint32\","
    "\"Reserved                 uint32                 2\","
    "\"FileName                 uint8                128\","
  "],"
/* TocEntryStructType */
  "\"toc\": ["
    "\"NoOfEntries              uint32\","
    "\"DataPtr                  uint32\","
    "\"DataOffset               uint32\","
  "],"
/* DownloadDataStructType */
  "\"download_data\": ["
    "\"LoadMapIndex             uint32\","
    "\"CompressionAlgorithm     uint32\","
    "\"CompressedLength         uint32\","
    "\"CRC                      uint32\","
    "\"DataLength               uint32\","
    "\"DataPtr                  uint32\","
    "\"DataOffset               uint32\","
  "],"
/* PartitionEntryStructType */
  "\"partition_entry\": ["
    "\"ID                       uint32\","
    "\"StartBlock               uint32\","
    "\"EndBlock                 uint32\","
    "\"MaxBlocksInSubPartition  uint32\","
    "\"StartReservoirBlock      uint32\","
    "\"EndReservoirBlock        uint32\","
    "\"ImageType                uint32\","
    "\"Options                  uint32\","
    "\"Reserved                 uint32\","
  "],"
/* NandPartitionDataStructType */
  "\"nand_partition_data\": ["
    "\"PartitionMarker          uint32\","
    "\"Partition                partition_entry  28\","
    "\"Reserved                 uint32                 3\","
    "\"LoadAddr                 uint32                28\","
  "],"
/* CodewordStructType */
  "\"codeword\": ["
    "\"Enabled                  uint32\","
    "\"Addr                     uint32\","
    "\"Value1                   uint32\","
    "\"Value2                   uint32\","
  "],"
/* InjectionDataStructType */
  "\"injection_data\": ["
    "\"CRC_16                   uint32\","
    "\"CRC_8                    uint32\","
    "\"DataLength               uint32\","
    "\"DataPtr                  uint32\","
    "\"DataOffset               uint32\","
  "],"
/* VersionDataStructType */
  "\"version_data\": ["
    "\"DataLength               uint32\","
    "\"DataPtr                  uint32\","
    "\"DataOffset               uint32\","
  "],"
/* RegionStructType */
  "\"load_map_entry\": ["
    "\"StartAddr                uint32\","
    "\"TotalLength              uint32\","
    "\"UsedLength               uint32\","
    "\"ImageFlags               uint32\""
  "],"
/* NewType - Because 'fls2_map_entry' contains a name! */
  "\"load_map\": ["
    "\"LoadMagic                uint32\","
    "\"LoadMap                  load_map_entry    8\""
  "],"
/* SecurityStructType */
  "\"sec_pack\": ["
    "\"SecureBlock0             uint8                256\","
    "\"SecureBlock1             uint8                256\","
    "\"Type                     uint32\","
    "\"DataBlock                uint8                208\","
    "\"BootCoreVersion          uint32\","
    "\"EblVersion               uint32\","
    "\"BootStartAddr            uint32\","
    "\"BootLength               uint32\","
    "\"BootSignature            uint32                 5\","
    "\"FileHash                 uint32                 5\","
    "\"PartitionMarker          uint32\","
    "\"Partition                partition_entry  28\","
    "\"PartitionReserved        uint32                 3\","
    "\"LoadAddrToPartition      uint32                28\","
    "\"LoadMagic                uint32\","
    "\"LoadMap                  load_map_entry    8\""
  "]"
"}";

typedef enum
{
  GENERIC_HEADER_ELEM_TYPE = 1000,//0x3E8
  END_MARKER_ELEM_TYPE=2,
  PRGHANDLER_TOOL_ELEM_TYPE, // All the tool version element types
  PRGSEQUENCER_TOOL_ELEM_TYPE,
  MAKEPRG_TOOL_ELEM_TYPE,
  HEXTOFLS_TOOL_ELEM_TYPE,
  FLSSIGN_TOOL_ELEM_TYPE,
  DWDTOHEX_TOOL_ELEM_TYPE,
  FLASHTOOL_TOOL_ELEM_TYPE,
  STORAGETOOL_TOOL_ELEM_TYPE,
  MEMORYMAP_ELEM_TYPE,
  DOWNLOADDATA_ELEM_TYPE,
  HARDWARE_ELEM_TYPE,
  NANDPARTITION_ELEM_TYPE,
  SECURITY_ELEM_TYPE,
  TOC_ELEM_TYPE,
  SAFE_BOOT_CODEWORD_ELEM_TYPE,
  INJECTED_PSI_ELEM_TYPE,
  INJECTED_EBL_ELEM_TYPE,
  GENERIC_VERSION_ELEM_TYPE,
  INJECTED_PSI_VERSION_ELEM_TYPE,
  INJECTED_EBL_VERSION_ELEM_TYPE,
  ROOT_DISK_ELEM_TYPE,
  CUST_DISK_ELEM_TYPE,
  USER_DISK_ELEM_TYPE,
  FLSTOHEADER_TOOL_ELEM,
  BOOT_CORE_TOOL_ELEM,
  EBL_TOOL_ELEM,
  INDIRECT_DOWNLOADDATA_ELEM_TYPE,
  FLSTONAND_TOOL_ELEM,
  INJECTED_INI_ELEM_TYPE,
  INJECTED_PDF_ELEM_TYPE,
  NUM_ELEM_TYPES // This should be the last
} PrgElementType;

static string PrgElementName[] =
{
  "Not Used",
  "Not Used",
  "END_MARKER_ELEM_TYPE",
  "PRGHANDLER_TOOL_ELEM_TYPE",
  "PRGSEQUENCER_TOOL_ELEM_TYPE",
  "MAKEPRG_TOOL_ELEM_TYPE",
  "HEXTOFLS_TOOL_ELEM_TYPE",
  "FLSSIGN_TOOL_ELEM_TYPE",
  "DWDTOHEX_TOOL_ELEM_TYPE",
  "FLASHTOOL_TOOL_ELEM_TYPE",
  "STORAGETOOL_TOOL_ELEM_TYPE",
  "MEMORYMAP_ELEM_TYPE",
  "DOWNLOADDATA_ELEM_TYPE",
  "HARDWARE_ELEM_TYPE",
  "NANDPARTITION_ELEM_TYPE",
  "SECURITY_ELEM_TYPE",
  "TOC_ELEM_TYPE",
  "SAFE_BOOT_CODEWORD_ELEM_TYPE",
  "INJECTED_PSI_ELEM_TYPE",
  "INJECTED_EBL_ELEM_TYPE",
  "GENERIC_VERSION_ELEM_TYPE",
  "INJECTED_PSI_VERSION_ELEM_TYPE",
  "INJECTED_EBL_VERSION_ELEM_TYPE",
  "ROOT_DISK_ELEM_TYPE",
  "CUST_DISK_ELEM_TYPE",
  "USER_DISK_ELEM_TYPE",
  "FLSTOHEADER_TOOL_ELEM",
  "BOOT_CORE_TOOL_ELEM",
  "EBL_TOOL_ELEM",
  "INDIRECT_DOWNLOADDATA_ELEM_TYPE",
  "FLSTONAND_TOOL_ELEM",
  "INJECTED_INI_ELEM_TYPE",
  "INJECTED_PDF_ELEM_TYPE",
  "NUM_ELEM_TYPES"
};


uint32_t fls2_get_size_of(const char* structure)
{
  StructParser format(fls2_json, structure);
  return format.size();
}

/**
 *  The Intel Fls2 Parser Class
 *
 * @param ref        The reference to save to
 * @param file       The file to load
 * @param verbose    Verbosity during parsing [0-2]
 */
Fls2Parser::Fls2Parser(FlsMetaFile& ref, string file, int verbose)
        : m_ref(ref),
          m_size(0),
          verbosity(verbose)
{
  m_ref.version      = 2;
  m_ref.fls_version  = "2.0";

  set_verbose(verbosity);

  if (!file.empty())
    load(file.c_str());
}

/**
 * Set Verbosity
 */
void Fls2Parser::set_verbose(int verbose)
{
  verbosity = verbose;
  info.setStream((verbose > 0) ? &cout : 0);
  debug.setStream((verbose > 1) ? &cout : 0);
}

/**
 * Loads the Fls2 file from a path
 */
void Fls2Parser::load(const char* file)
{
  if (!fexists(file))
    throw_exception("File "+ string(file) +" does not exist!");

  m_ref.m_fls_file   = file;
  m_ref.date_created = GetTimestamp(fcreated(file));

  ifstream fls(file, fstream::binary | ios_base::ate);
  if (!fls.is_open())
    throw_exception("Could not open "+ string(file));

  try
  {
    m_size = fls.tellg();
    fls.seekg(0);

    // Parses the Meta data
    Fls2Parser::fls2_parse(fls);

    // Save the Meta data as an embedded file
    vector<uint8_t> buf;
    m_ref.json_file = new FlsEmbedFile("", META_FILE_NAME);
    m_ref.json_file->set_content( m_ref.get_meta_data(buf) );
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


DownloadFile* Fls2Parser::get_download_file(size_t uid)
{
  while (uid >= m_ref.download_files.size())
    m_ref.download_files.push_back(new DownloadFile(m_ref.m_fls_file));

  return m_ref.download_files[uid];
}

/**
 *  Updates the header.Size & writes to the output stream
 */
static void fls2_write_entry(
  ostream& fls,
  StructParser& header,
  StructParser& body)
{
  header.set_uint32("Size", body.size() + header.size());
  fls.write(header.data(), header.size());
  fls.write(body.data(), body.size());
}


/**
 * Saves an Fls2 file to a path
 *
 * @param ref      Reference to meta data file
 * @param file     Output file to save
 * @param verbose  Verbosity during the save [0-2]
 */
void Fls2Parser::save(FlsMetaFile& ref, string file, int verbose)
{
  Logger info, debug;
  StructParser body(fls2_json);
  StructParser header(fls2_header_json, "header");
  StructParser load_map(fls2_json, "load_map");
  StructParser partition_table(fls2_json, "nand_partition_data");

  info.setStream((verbose > 0) ? &cout : 0);
  debug.setStream((verbose > 1) ? &cout : 0);

  ofstream fls(file.c_str(), fstream::binary);
  if (!fls.is_open())
    throw_exception("Could not open "+ string(file)
                   +": "+ string(strerror( errno )));

  string ext = fext(file.c_str());
  //< Appending Header
  body.set_structure("body")
      .set_uint32("Marker", PRG_MAGIC_NUMBER)
      .set_uint32("FileType", (ext == "prg") ? PRG_FILE_TYPE : FLS_FILE_TYPE);

  fls2_write_entry(fls, header.set_uint32("Type", GENERIC_HEADER_ELEM_TYPE), body);

  //< Appending ToolVersions
  body.set_structure("tool_version")
      .set_uint32("Used", 0x00030000);

  fls2_write_entry(fls, header.set_uint32("Type", PRGHANDLER_TOOL_ELEM_TYPE),
                          body.set_uint32("Required", 0x00020003));

  fls2_write_entry(fls, header.set_uint32("Type", PRGSEQUENCER_TOOL_ELEM_TYPE),
                          body.set_uint32("Required", 0x00020004));

  fls2_write_entry(fls, header.set_uint32("Type", MAKEPRG_TOOL_ELEM_TYPE),
                          body.set_uint32("Required", 0x0002000C));

  fls2_write_entry(fls, header.set_uint32("Type", HEXTOFLS_TOOL_ELEM_TYPE),
                          body.set_uint32("Required", 0x00020000));

  fls2_write_entry(fls, header.set_uint32("Type", FLSSIGN_TOOL_ELEM_TYPE),
                          body.set_uint32("Required", 0x00020000));

  fls2_write_entry(fls, header.set_uint32("Type", DWDTOHEX_TOOL_ELEM_TYPE),
                          body.set_uint32("Required", 0x00020000));

  fls2_write_entry(fls, header.set_uint32("Type", FLASHTOOL_TOOL_ELEM_TYPE),
                          body.set_uint32("Required", 0x00040000));

  fls2_write_entry(fls, header.set_uint32("Type", STORAGETOOL_TOOL_ELEM_TYPE),
                          body.set_uint32("Required", 0x00010000));


  //< Appending Hardware info
  body.set_structure("hardware")
      .set_uint32("Platform", getChipNameId(ref.platform.chip_name))
      .set_uint32("FlashTechnology", ref.platform.technology)
      .set_uint32("SystemSize", ref.platform.storage_size)
      .set_uint32("BootSpeed", ref.platform.usif_boot_speed);

  fls2_write_entry(fls, header.set_uint32("Type", HARDWARE_ELEM_TYPE), body);

  //< Constructing & Appending MemoryMap
  body.set_structure("memory_map")
      .set_uint32("FlashStartAddr", ref.platform.storage_address);

  for (size_t i=0; i<ref.memory_map.size(); ++i)
  {
    uint64_t flags = 0;
    MemoryArea* area = ref.memory_map[i];

    for (size_t j=0; j < area->flags.size(); ++j)
      flags += area->flags[j];

    body.get_child("Entry", i)
          .set_uint32("Start", area->address)
          .set_uint32("Length", area->size)
          .set_uint32("Class", area->type.id)
          .set_uint32("Flags", flags)
          .set_string("FormatSpecify", area->tag);
  }
  for (size_t i=ref.memory_map.size(); i < body.size("Entry"); ++i)
  {
    body.get_child("Entry", i).set_uint32("Start", 0xFFFFFFFF);
  }
  fls2_write_entry(fls, header.set_uint32("Type", MEMORYMAP_ELEM_TYPE), body);

  //< Constructing & Appending NAND Partitions
  if (ref.partitions.size() > 0)
  {
    partition_table.set_uint32("PartitionMarker", ref.partition_marker);
    for (size_t i=0; i<ref.partitions.size(); ++i)
    {
      NandPartition* part = ref.partitions[i];
      partition_table.set_uint32("LoadAddr", part->address, i);

      partition_table.get_child("Partition", i)
            .set_uint32("ID", part->id)
            .set_uint32("ImageType", part->type)
            .set_uint32("StartBlock", part->start_block)
            .set_uint32("EndBlock", part->end_block)
            .set_uint32("MaxBlocksInSubPartition", part->max_block_in_sub_par)
            .set_uint32("StartReservoirBlock", part->start_reservoir)
            .set_uint32("EndReservoirBlock", part->end_reservoir)
            .set_uint32("Options", part->options)
            .set_uint32("Reserved", part->attributes);
    }
    fls2_write_entry(fls, header.set_uint32("Type", NANDPARTITION_ELEM_TYPE), partition_table);
  }

  //< Appending TOC
  body.set_structure("toc");
  header.set_uint32("Type", TOC_ELEM_TYPE);

  uint32_t NoOfEntries = ref.download_files.size();
  body.set_uint32("NoOfEntries", NoOfEntries);
  header.set_uint32("Size", body.size() + header.size()
                            + NoOfEntries * body.size("toc_entry"));

  body.set_uint32("DataOffset", (uint32_t)fls.tellp()+header.size()+body.size());
  fls.write(header.data(), header.size());
  fls.write(body.data(), body.size());

  //< Appending TOC Entries (No header)
  body.set_structure("toc_entry");
  for (size_t i=0; i < NoOfEntries; ++i)
  {
    DownloadFile* file = ref.download_files[i];
    body.set_uint32("UID", i)
        .set_string("FileName", file->name)
        .set_uint32("MemoryClass", file->type.id);
    fls.write(body.data(), body.size());
  }

  //< Appending Meta+Boot Files
  vector<EmbeddedFile*> meta_files(ref.meta_files);
  meta_files.insert(meta_files.end(),
                    ref.boot_files.begin(), ref.boot_files.end());

  for (size_t i=0; i < meta_files.size(); ++i)
  {
    PrgElementType type;
    vector<uint8_t> content;
    EmbeddedFile* file = meta_files[i];
    ext = fext(file->name());

    info << "  Adding file: " << file->name() << " (" << ext << ")" << endl;
    if (ext.compare("txt") == 0)
    {
      body.set_structure("version_data");

      if (file->name().find("inj_version.txt") != string::npos)
        type = GENERIC_VERSION_ELEM_TYPE;

      else if (file->name().find("PSI") != string::npos)
        type = INJECTED_PSI_VERSION_ELEM_TYPE;

      else if (file->name().find("EBL") != string::npos)
        type = INJECTED_EBL_VERSION_ELEM_TYPE;

      else
        throw_exception("Could not determine meta_file type of "+ file->name());

      // TODO: Remember NULL padding
    }
    else if (ext.compare("zip") == 0)
    {
      body.set_structure("injection_data");
      type = INJECTED_INI_ELEM_TYPE;
      // TODO: When to use INJECTED_PDF_ELEM_TYPE ???

      // body.set_uint32("CRC_8", (hash>>8)^(hash&0xFF));
      body.set_uint32("CRC_16", StringTo<uint32_t>(file->hash[HASH_XOR16]));
    }
    else if (ext.compare("bin") == 0)
    {
      body.set_structure("injection_data");

      if (file->name().find("PSI") != string::npos)
        type = INJECTED_PSI_ELEM_TYPE;
      else if (file->name().find("EBL") != string::npos)
        type = INJECTED_EBL_ELEM_TYPE;
      else
        throw_exception("Could not determine meta_file type of "+ file->name());

      body.set_uint32("CRC_16", StringTo<uint32_t>(file->hash[HASH_XOR16]));
    }
    else
    {
      throw_exception("Could not determine meta_file type["+ext+"] of "+ file->name());
    }

    header.set_uint32("Type", (uint32_t)type)
          .set_uint32("Size", body.size() + header.size() + file->size());
    fls.write(header.data(), header.size());

    body.set_uint32("DataLength", file->size())
        .set_uint32("DataOffset", (uint32_t)fls.tellp()+body.size());
    fls.write(body.data(), body.size());

    //< Getting file content
    file->save_to_stream(fls);
  }


  //< Appending Download Files
  for (size_t i=0; i < ref.download_files.size(); ++i)
  {
    vector<uint8_t> sec_pack;
    DownloadFile* file = ref.download_files[i];

    info << "  Adding DownloadFile: " << file->name << endl;

    //< Saving each LoadMap element as "fls2_download_data"
    body.set_structure("download_data");

    header.set_uint32("Type", DOWNLOADDATA_ELEM_TYPE);
    header.set_uint32("UID", i);

    for (size_t uid=0; uid < file->load_map.size(); ++uid)
    {
      vector<uint8_t> content;
      MemoryRegion* region = file->load_map[uid];

      if (region->size() == 0) //< Flashless sections are virtual
        continue;

      header.set_uint32("Size", body.size() + header.size() + region->size());
      fls.write(header.data(), header.size());

      body.set_uint32("LoadMapIndex", uid);
      body.set_uint32("CRC",   StringTo<uint32_t>(region->hash[HASH_XOR16]));
      body.set_uint32("DataLength", region->size());
      body.set_uint32("DataOffset", (uint32_t)fls.tellp() + body.size());
      fls.write(body.data(), body.size());

      region->save_to_stream(fls);
    }

    //< Appending SecPack
    header.set_uint32("Type", SECURITY_ELEM_TYPE);
    header.set_uint32("Size", body.size("sec_pack") + header.size());
    fls.write(header.data(), header.size());

    info << "  Adding SecPack: " << file->sec_pack.name() << endl;
    file->sec_pack.save_to_stream(fls);
  }


  //< Appending EOF
  header.set_uint32("Type", END_MARKER_ELEM_TYPE);
  header.set_uint32("Size", header.size());
  fls.write(header.data(), header.size());

  fls.close();
}


/**
 * Parses an Fls2 file.
 *
 * Parses an Fls2 format file and saves in the local FlsMetafile reference.
 * All embedded file pointers and lengths are saved and can be extracted
 * via the extract function.
 *
 * @return true if parse completed successfully
 */
void Fls2Parser::fls2_parse(std::ifstream& fls)
{
  size_t i, j;
  bool done = false;
  string& fls_file = m_ref.m_fls_file;
  uint32_t type, uid, size;
  size_t next_header = 0;
  StructParser element(fls2_json, "body");
  StructParser header(fls2_header_json, "header");

  info << "  -- FLS Parser (" << fls_file << ") -- " << endl;

  // Check for FFC header
  char ffc[4];
  fls.read((char*)&ffc, sizeof(ffc));
  if (memcmp(ffc, "\x91\x64\x1D\xED", 4) == 0)
  {
    fls.seekg(0x26);
    fls.read((char*)&ffc, sizeof(ffc));
    next_header  = ffc[0] << 24;
    next_header |= ffc[1] << 16;
    next_header |= ffc[2] << 8;
    next_header |= ffc[3];
    debug << " -> FFC Header Offset: " << next_header << endl;
  }
  fls.seekg(next_header);

  // Read the header of the first element
  header.load(fls);

  // If type not found - Test for Binary NAND with embedded EBL
  if (header.get_uint32("Type") != GENERIC_HEADER_ELEM_TYPE)
  {
    char meta[8];
    // The file may be an uploaded binary file from a NAND system
    // if so there is a offset to an appended EBL.FLS file at EOF -8
    fls.seekg(m_size-8);
    fls.read(meta, 8);
    uint32_t ebl_offset = *((uint32_t*)meta);
    if (ebl_offset < (m_size - header.size() - 8))
    {
      fls.seekg(ebl_offset);
      header.load(fls);
    }

    // Try comparing the header again.  
    if (header.get_uint32("Type") != GENERIC_HEADER_ELEM_TYPE)
      throw_exception("Wrong fls header type");

    // The Binary part is considered a DownloadFile
    debug << " -> Binary with injected EBL" << endl;
    DownloadFile* file = get_download_file(1);
    file->name = fname(fls_file);
    file->type = get_memory_class(MEMORY_CLASS_BINARY);
    file->load_map.push_back(new MemoryRegion(fls_file, 0, ebl_offset));

    uint32_t Xor16 = file->load_map[0]->get_hash(crypto::HASH_XOR16);
    file->load_map[0]->hash[crypto::HASH_XOR16] = getHexString(Xor16, 4);

    next_header = ebl_offset;
//    set_verbose(3);
  }

  if (header.get_uint32("Size") != header.size()+element.size("body"))
    throw_exception("Wrong fls header size");

  next_header += header.get_uint32("Size");

  do {
    fls.seekg( next_header );
    header.load(fls);

    uid  = header.get_uint32("UID");
    type = header.get_uint32("Type");
    size = header.get_uint32("Size") - header.size();
    next_header += header.size() + size;

    // debug << " Reading " << PrgElementName[type] << " (Next: " << next_header << ")" << endl;

    switch(type)
    {
      case END_MARKER_ELEM_TYPE:
      {
        debug << "END_MARKER_ELEM_TYPE" << endl << endl;
        done = true;
        break;
      }

      case PRGHANDLER_TOOL_ELEM_TYPE:
      case PRGSEQUENCER_TOOL_ELEM_TYPE:
      case MAKEPRG_TOOL_ELEM_TYPE:
      case HEXTOFLS_TOOL_ELEM_TYPE:
      case FLSSIGN_TOOL_ELEM_TYPE:
      case DWDTOHEX_TOOL_ELEM_TYPE:
      case FLASHTOOL_TOOL_ELEM_TYPE:
      case STORAGETOOL_TOOL_ELEM_TYPE:
      case FLSTOHEADER_TOOL_ELEM:
      case BOOT_CORE_TOOL_ELEM:
      case EBL_TOOL_ELEM:
      case FLSTONAND_TOOL_ELEM:
      {
        element.load(fls, "tool_version");

        if (size != element.size())
          throw_exception("Element length does not match element type: "+ PrgElementName[type]);

        debug << PrgElementName[type] << endl
             << "UID = "      << getHexString(uid) << endl
             << "Required = " << getHexString(element.get_uint32("Required")) << endl
             << "Used = "     << getHexString(element.get_uint32("Used")) << endl
             << endl;
        break;
      }

      case HARDWARE_ELEM_TYPE:
        {
          element.load(fls, "hardware");

          if (size != element.size())
            throw_exception("Element length does not match element type: "+ PrgElementName[type]);

          m_ref.platform.version = "Not Specified";
          m_ref.platform.chip_name = getChipName(element.get_uint32("Platform"));
          m_ref.platform.technology = (StorageTech)element.get_uint32("FlashTechnology");
          //m_ref.platform.storage_address = element.get_uint32("FlashStartAddr"); //< From MemoryMap
          m_ref.platform.storage_size = element.get_uint32("SystemSize");
          m_ref.platform.usif_boot_speed = element.get_uint32("BootSpeed");

          debug << PrgElementName[type] << endl
               << "UID = "             << getHexString(uid) << endl
               << "Platform = "        << getHexString(element.get_uint32("Platform")) << " (platform.chip_name)" << endl
               << "SystemSize = "      << getHexString(element.get_uint32("SystemSize")) << " (platform.storage_size)" << endl
               << "BootSpeed = "       << getHexString(element.get_uint32("BootSpeed")) << " (platform.usif_boot_speed)" << endl
               << "HandshakeMode = "   << getHexString(element.get_uint32("HandshakeMode")) << endl
               << "UsbCapable = "      << getHexString(element.get_uint32("UsbCapable")) << endl
               << "FlashTechnology = " << getHexString(element.get_uint32("FlashTechnology")) << " (platform.technology)" << endl;

          size = element.size("SG3ChipSelect");
          for (i=0; i<size; i++)
          {
            StructParser& chip = element.get_child("SG3ChipSelect", i);
            debug << "ChipSelect[" << i << "]"
                  << " ChipSelectNumber=" << getHexString(chip.get_uint32("ChipSelectNumber"))
                  << " BUSRCON=" << getHexString(chip.get_uint32("BUSRCON"))
                  << " BUSWCON=" << getHexString(chip.get_uint32("BUSWCON"))
                  << " BUSRP="   << getHexString(chip.get_uint32("BUSRP"))
                  << " BUSWP="   << getHexString(chip.get_uint32("BUSWP"))
                  << endl;
          }

          debug << "ProjectName='" << element.get_string("ProjectName") << "'" << endl
                << "DepTemplate='" << element.get_string("DepTemplate") << "'" << endl
                << "CfgTemplate='" << element.get_string("CfgTemplate") << "'" << endl
                << endl;
          break;
        }

      case MEMORYMAP_ELEM_TYPE:
      {
        element.load(fls, "memory_map");

        if (size != element.size())
          throw_exception("Element length does not match element type: "+ PrgElementName[type]);

        m_ref.platform.storage_address = element.get_uint32("FlashStartAddr");
        m_ref.platform.SwVersionStringLocation = element.get_uint32("SwVersionStringLocation");

        debug << PrgElementName[type] << endl
             << "UID = " << getHexString(uid) << endl;

        size = element.size("Entry");
        for (i=0; i<size; i++)
        {
          StructParser& entry = element.get_child("Entry", i);
          string tag = entry.get_string("FormatSpecify");
          uint32_t start_addr = entry.get_uint32("Start");
          uint32_t length = entry.get_uint32("Length");
          uint32_t flags = entry.get_uint32("Flags");
          MemoryClassId class_id = (MemoryClassId)entry.get_uint32("Class");

          debug << "Entry[" << i << "]"
               << " Start=" << getHexString(start_addr)
               << " Length=" << getHexString(length)
               << " Class=" << getHexString(class_id)
               << " Flags=" << getHexString(flags)
               << " FormatSpecify='" << tag << "'"
               << endl;

          if (!start_addr)
            continue;

          if (!tag.empty() || length)
          {
            MemoryArea* area = m_ref.getMemoryAreaByAddr(start_addr, class_id, false);
            if (!area || !tag.empty())
            {
              area = new MemoryArea(class_id);
              area->address = start_addr;
              area->size = length;
              m_ref.memory_map.push_back(area);
            }
            else
            {
              if (area->address != start_addr)
                throw_exception("MemoryArea::StartAddr ("+ getHexString(area->address)
                               +") != LoadMap::StartAddr ("+ getHexString(start_addr) +")");

              if (area->size != length)
                throw_exception("MemoryArea::TotalLength ("+ getHexString(area->size)
                               +") != LoadMap::TotalLength ("+ getHexString(length) +")");
            }
            area->tag = tag;
            area->flags.set( flags );
          }
        }

        debug << "EepRevAddrInSw=" << getHexString(element.get_uint32("EepRevAddrInSw")) << endl
              << "EepRevAddrInEep=" << getHexString(element.get_uint32("EepRevAddrInEep")) << endl
              << "EepStartModeAddr=" << getHexString(element.get_uint32("EepStartModeAddr")) << endl
              << "NormalModeValue=" << getHexString(element.get_uint32("NormalModeValue")) << endl
              << "TestModeValue=" << getHexString(element.get_uint32("TestModeValue")) << endl
              << "SwVersionStringLocation=" << getHexString(element.get_uint32("SwVersionStringLocation")) << endl
              << "FlashStartAddr=" << getHexString(element.get_uint32("FlashStartAddr")) << " (platform.storage_address)" << endl
              << endl;
        break;
      }

      case TOC_ELEM_TYPE:
      {
        element.load(fls, "toc");

        debug << PrgElementName[type] << endl
              << "UID = "        << getHexString(uid) << endl
              << "NoOfEntries="  << getHexString(element.get_uint32("NoOfEntries")) << endl
              << "DataPtr="      << getHexString(element.get_uint32("DataPtr")) << endl
              << "DataOffset="   << getHexString(element.get_uint32("DataOffset")) << endl;

        uint32_t ent_size = element.get_uint32("NoOfEntries");

        if (size != element.size() + ent_size * element.size("toc_entry"))
          throw_exception("Element length does not match element type: "+ PrgElementName[type]);

        for (i=0; i < ent_size; i++)
        {
          element.load(fls, "toc_entry");

          uid = element.get_uint32("UID");

          // Find the e->UID of this Entry
          DownloadFile* file = get_download_file(uid);

          file->name = fname(element.get_string("FileName").c_str());
          file->type = get_memory_class((MemoryClassId)element.get_uint32("MemoryClass"));

          debug << "TocEntry[" << i << "]"
                << " UID="          << getHexString(uid)
                << " MemoryClass="  << getHexString(element.get_uint32("MemoryClass"))
                << " (" << file->type.name << ")"
                << " Reserved[0]="  << getHexString(element.get_uint32("Reserved", 0))
                << " Reserved[1]="  << getHexString(element.get_uint32("Reserved", 1))
                << " FileName='"    << element.get_string("FileName") << "'" << endl
                << endl;
        }
        break;
      }

      case DOWNLOADDATA_ELEM_TYPE:
      {
        element.load(fls, "download_data");

        i = element.get_uint32("LoadMapIndex");
        uint32_t crc = element.get_uint32("CRC");
        uint32_t offset = element.get_uint32("DataOffset");
        uint32_t length =  element.get_uint32("DataLength");

        if (size != element.size()+length)
          throw_exception("Element length does not match element type: "+ PrgElementName[type]);

        DownloadFile* file = get_download_file(uid);

        // file->sec_pack.hash[HASH_XOR16] = getHexString(element.get_uint32("CRC"), 4);

        debug << PrgElementName[type] << endl
              << "UID = "                   << getHexString(uid) << endl
              << "LoadMapIndex = "          << getHexString(i) << endl
              << "CompressionAlgorithm = "  << getHexString(element.get_uint32("CompressionAlgorithm")) << endl
              << "CompressedLength = "      << getHexString(element.get_uint32("CompressedLength")) << endl
              << "CRC = "                   << getHexString(crc) << " (download_files.hash[HASH_XOR16])" << endl
              << "DataLength = "            << getHexString(length) << endl
              << "Data = "                  << getHexString(element.get_uint32("DataPtr")) << endl
              << "DataOffset = "            << getHexString(offset) << endl
              << endl;

        // Create LoadMap if not existing
        while (i > file->load_map.size())
          file->load_map.push_back(new MemoryRegion(fls_file));

        MemoryRegion* region;
        if (i == file->load_map.size())
        {
          region = new MemoryRegion(fls_file, fls.tellg(), length);
          file->load_map.push_back(region);
        }
        else
        {
          region = file->load_map[i];
          region->offset( fls.tellg() );
          region->resize( length );
        }
        region->hash[HASH_XOR16] = getHexString(crc, 4);
        break;
      }
      case SECURITY_ELEM_TYPE:
      {
        element.load(fls, "sec_pack");

        if (size != element.size())
          throw_exception("Element length does not match element type: "+ PrgElementName[type]);

        DownloadFile* file = get_download_file(uid);

        file->type = get_memory_class((MemoryClassId)element.get_uint32("Type"));
        file->sec_pack.type = file->type;

        file->sec_pack.rename( generateFileName(file->name, uid,
                                  file->type.name, "SecureBlock.bin") );

        //// file->DataBlock = sec.DataBlock;
        file->sec_pack.set_content(element.buffer());

        uint16_t hash = file->sec_pack.get_hash(HASH_XOR16);
        file->sec_pack.hash[HASH_XOR16] = getHexString(hash, 4);

        debug << PrgElementName[type] << endl
              << "UID = "                      << getHexString(uid) << endl
              << "SecureBlock[0] .. 256 bytes" << endl
              << "SecureBlock[1] .. 256 bytes" << endl
              << "Type = "                     << getHexString(element.get_uint32("Type")) << " (sec_pack.type)" << endl
              << "DataBlock .. 208 bytes"      << endl
              << "BootCoreVersion = "          << getHexString(element.get_uint32("BootCoreVersion")) << endl
              << "EblVersion = "               << getHexString(element.get_uint32("EblVersion")) << endl
              << "FileHash ="
              << " " << getHexString(element.get_uint32("FileHash", 0))
              << " " << getHexString(element.get_uint32("FileHash", 1))
              << " " << getHexString(element.get_uint32("FileHash", 2))
              << " " << getHexString(element.get_uint32("FileHash", 3))
              << " " << getHexString(element.get_uint32("FileHash", 4))
              << endl;

        size = element.size("Partition");
        for (i=0; i<size; i++)
        {
          StructParser& part = element.get_child("Partition", i);

          debug << "Partition[" << i << "]"
                << " ID="                      << getHexString(part.get_uint32("ID"))
                << " StartBlock="              << getHexString(part.get_uint32("StartBlock"))
                << " EndBlock="                << getHexString(part.get_uint32("EndBlock"))
                << " MaxBlocksInSubPartition=" << getHexString(part.get_uint32("MaxBlocksInSubPartition"))
                << " StartReservoirBlock="     << getHexString(part.get_uint32("StartReservoirBlock"))
                << " ImageType="               << getHexString(part.get_uint32("ImageType"))
                << " Options="                 << getHexString(part.get_uint32("Options"))
                << " Reserved="                << getHexString(part.get_uint32("Reserved"))
                << " LoadToPartition="         << getHexString(element.get_uint32("LoadAddrToPartition", i))
                << endl;
        }

        debug << "LoadMagic = " << getHexString(element.get_uint32("LoadMagic")) << endl;
        size = element.size("LoadMap");
        for (i=0; i<size; i++)
        {
          StructParser& map = element.get_child("LoadMap", i);

          uint32_t start_addr = map.get_uint32("StartAddr");
          uint32_t total_length = map.get_uint32("TotalLength");
          uint32_t used_length = map.get_uint32("UsedLength");
          uint32_t flags = map.get_uint32("ImageFlags");

          debug << "LoadMap[" << i << "]"
                << " StartAddr="   << getHexString(start_addr)
                << " TotalLength=" << getHexString(total_length)
                << " UsedLength="  << getHexString(used_length)
                << " ImageFlags="  << getHexString(flags)
                << endl;

          // Find the LoadMap entry of the file
          if (total_length)
          {
            while (i >= file->load_map.size())
              file->load_map.push_back(new MemoryRegion(fls_file));

            MemoryRegion* region = file->load_map[i];
            region->flags.set( flags );
            region->resize( used_length );

            region->area = m_ref.getMemoryAreaByAddr(start_addr, file->type.id, false);
            if (!region->area)
            {
              region->area = new MemoryArea();
              region->area->address = start_addr;
              region->area->size = total_length;
              m_ref.memory_map.push_back(region->area);
            }
            else
            {
              if (region->area->address != start_addr)
                throw_exception("MemoryArea::StartAddr != LoadMap::StartAddr");

              if (region->area->size != total_length)
                throw_exception("MemoryArea::TotalLength != LoadMap::TotalLength");
            }
          }
        }
        debug << endl;
        break;
      }

      case INDIRECT_DOWNLOADDATA_ELEM_TYPE:
      {
        element.load(fls, "download_data");

        if (size != element.size()+element.get_uint32("DataLength"))
          throw_exception("Element length does not match element type: "+ PrgElementName[type]);

        uint32_t i = element.get_uint32("LoadMapIndex");
        uint32_t crc = element.get_uint32("CRC");
        uint32_t length = element.get_uint32("DataLength");
        uint32_t offset = element.get_uint32("DataOffset");
        string name = generateFileName(m_ref.download_files[uid]->name, uid, "CODE",
                                       "LoadMap"+ Stringify(i) +".bin");

        debug << PrgElementName[type] << endl
              << "UID = "                  << getHexString(uid) << endl
              << "LoadMapIndex = "         << getHexString(i) << endl
              << "CompressionAlgorithm = " << getHexString(element.get_uint32("CompressionAlgorithm")) << endl
              << "CompressedLength = "     << getHexString(element.get_uint32("CompressedLength")) << endl
              << "CRC = "                  << getHexString(crc) << endl
              << "DataLength = "           << getHexString(length) << endl
              << "Data = "                 << getHexString(element.get_uint32("DataPtr")) << endl
              << "DataOffset = "           << getHexString(offset) << endl
              << endl;

        // Save Embedded file info
        MemoryRegion* region;
        DownloadFile* file = get_download_file(uid);

        while (i > file->load_map.size())
          file->load_map.push_back(new MemoryRegion(fls_file));

        if (i == file->load_map.size())
        {
          region = new MemoryRegion(fls_file, fls.tellg(), length);
          file->load_map.push_back(region);
        }
        else
        {
          region = file->load_map[i];
          region->offset( fls.tellg() );
          region->resize( length );
        }
        region->hash[HASH_XOR16] = getHexString(crc, 4);

        break;
      }

      case NANDPARTITION_ELEM_TYPE:
      {
        element.load(fls, "nand_partition_data");
        if (size != element.size())
          throw_exception("Element length does not match element type: "+ PrgElementName[type]);

        m_ref.partition_marker = element.get_uint32("PartitionMarker");

        debug << PrgElementName[type] << endl
              << "UID = "             << getHexString(uid) << endl
              << "PartitionMarker = " << getHexString(m_ref.partition_marker)
              << endl;

        vector<uint32_t> load_addr_table = element.get_uint32_array("LoadAddr");

        size = element.size("Partition");
        for (i=0; i<size; i++)
        {
          StructParser& part = element.get_child("Partition", i);

          if (part.get_uint32("EndBlock"))
           {
            NandPartition* nand = new NandPartition();
            nand->id              = part.get_uint32("ID");
            nand->type            = part.get_uint32("ImageType");
            nand->address         = load_addr_table[i];
            nand->start_block     = part.get_uint32("StartBlock");
            nand->end_block       = part.get_uint32("EndBlock");
            nand->max_block_in_sub_par = part.get_uint32("MaxBlocksInSubPartition");
            nand->start_reservoir = part.get_uint32("StartReservoirBlock");
            nand->end_reservoir   = part.get_uint32("EndReservoirBlock");
            nand->options         = part.get_uint32("Options");
            nand->attributes      = part.get_uint32("Reserved");
            m_ref.partitions.push_back(nand);
          }
          debug << "Partition[" << i << "]"
                << " ID="                      << getHexString(part.get_uint32("ID"))
                << " StartBlock="              << getHexString(part.get_uint32("StartBlock"))
                << " EndBlock="                << getHexString(part.get_uint32("EndBlock"))
                << " MaxBlocksInSubPartition=" << getHexString(part.get_uint32("MaxBlocksInSubPartition"))
                << " StartReservoirBlock="     << getHexString(part.get_uint32("StartReservoirBlock"))
                << " EndReservoirBlock="       << getHexString(part.get_uint32("EndReservoirBlock"))
                << " ImageType="               << getHexString(part.get_uint32("ImageType"))
                << " Options="                 << getHexString(part.get_uint32("Options"))
                << " Reserved="                << getHexString(part.get_uint32("Reserved"))
                << " LoadToPartition="         << getHexString(load_addr_table[i])
                << endl;
        }

        vector<uint32_t> reserved = element.get_uint32_array("Reserved");
        for (i=0; i<reserved.size(); i++)
        {
          debug << "Reserved[" << i << "] = " << getHexString(reserved[i]) << endl;
        }
        debug << endl;
        break;
      }

      case SAFE_BOOT_CODEWORD_ELEM_TYPE:
      {
        element.load(fls, "codeword");

        if (size != element.size())
          throw_exception("Element length does not match element type: "+ PrgElementName[type]);

        debug << PrgElementName[type] << endl
              << "UID = "     << getHexString(uid) << endl
              << "Enabled = " << getHexString(element.get_uint32("Enabled")) << endl
              << "Addr = "    << getHexString(element.get_uint32("Addr")) << endl
              << "Value1 = "  << getHexString(element.get_uint32("Value1")) << endl
              << "Value2 = "  << getHexString(element.get_uint32("Value2")) << endl
              << endl;
        break;
      }

      case INJECTED_PSI_ELEM_TYPE: //< IMG_INJ_PSI
      case INJECTED_EBL_ELEM_TYPE: //< IMG_INJ_EBL
      case INJECTED_INI_ELEM_TYPE: //< IMG_INJ_ZIP
      case INJECTED_PDF_ELEM_TYPE: //< IMG_INJ_XML
      {
        element.load(fls, "injection_data");

        uint32_t offset = element.get_uint32("DataOffset");
        uint32_t data_len = element.get_uint32("DataLength");
        string hash = getHexString(element.get_uint32("CRC_16"), 4);

        debug << PrgElementName[type] << endl
              << "UID = "         << getHexString(uid) << endl
              << "CRC_16 = "      << hash << endl
              << "CRC_8 = "       << getHexString(element.get_uint32("CRC_8")) << endl
              << "DataLength = "  << getHexString(data_len) << endl
              << "Data = "        << getHexString(element.get_uint32("DataPtr")) << endl
              << "DataOffset = "  << getHexString(offset) << endl
              << endl;

        if (size != element.size()+data_len)
          cerr << " WARNING> Element length does not match DataLength of "+ PrgElementName[type] << endl;
//          throw_exception("Element length does not match DataLength of "+ PrgElementName[type]);

        DownloadFile* file = get_download_file(uid);

        EmbeddedFile* embed = new EmbeddedFile(fls_file, fls.tellg(), data_len);
        embed->hash[HASH_XOR16] = hash;

        // TODO: If the file have not been parsed first its name is empty.
        switch(type)
        {
          case INJECTED_PSI_ELEM_TYPE: //< IMG_INJ_PSI
            embed->type = get_memory_class(MEMORY_CLASS_PSI);
            embed->rename( file->name+"_inj_"+ embed->type.name +".bin" );
            m_ref.boot_files.push_back(embed);
            break;
          case INJECTED_EBL_ELEM_TYPE: //< IMG_INJ_EBL
            embed->type = get_memory_class(MEMORY_CLASS_EBL);
            embed->rename( file->name+"_inj_"+ embed->type.name +".bin" );
            m_ref.boot_files.push_back(embed);
            break;
          //case MEMORY_CLASS_BOOT_CORE_SLB:
          case INJECTED_INI_ELEM_TYPE: //< IMG_INJ_ZIP
            embed->type = get_memory_class(MEMORY_CLASS_BINARY);
            embed->rename( file->name+"_inj_archive.zip" );
            m_ref.meta_files.push_back(embed);
            break;
          case INJECTED_PDF_ELEM_TYPE: //< IMG_INJ_XML
            embed->type = get_memory_class(MEMORY_CLASS_TEXT);
            embed->rename( file->name+"_inj_archive.xml" );
            m_ref.meta_files.push_back(embed);
            break;
        }
        break;
      }
      case INJECTED_PSI_VERSION_ELEM_TYPE: //< IMG_INJ_PSI_VER
      case INJECTED_EBL_VERSION_ELEM_TYPE: //< IMG_INJ_EBL_VER
      case GENERIC_VERSION_ELEM_TYPE:
      {
        element.load(fls, "version_data");

        if (size != element.size()+element.get_uint32("DataLength"))
          throw_exception("Element length does not match element type: "+ PrgElementName[type]);

        uint32_t offset = element.get_uint32("DataOffset");
        uint32_t size = element.get_uint32("DataLength");

        DownloadFile* file = get_download_file(uid);
        EmbeddedFile* embed = new EmbeddedFile(fls_file, fls.tellg(), size);

        // TODO: If the file have not been parsed first its name is empty.
        switch(type)
        {
          case GENERIC_VERSION_ELEM_TYPE:
            embed->type = file->type;
            embed->rename( file->name+"_inj_version.txt" );
            m_ref.meta_files.push_back(embed);
            break;
          case INJECTED_PSI_VERSION_ELEM_TYPE: //< IMG_INJ_PSI
            embed->type = get_memory_class(MEMORY_CLASS_PSI);
            embed->rename( file->name+"_inj_"+ embed->type.name +"_ver.txt" );
            m_ref.meta_files.push_back(embed);
            break;
          case INJECTED_EBL_VERSION_ELEM_TYPE: //< IMG_INJ_EBL
            embed->type = get_memory_class(MEMORY_CLASS_EBL);
            embed->rename( file->name+"_inj_"+ embed->type.name +"_ver.txt" );
            m_ref.meta_files.push_back(embed);
            break;
          default:
            throw_exception("Could not interpret VERSION_ELEMENT_TYPE");
        }

        debug << PrgElementName[type] << endl
              << "UID = "         << getHexString(uid) << endl
              << "DataLength = "  << getHexString(size) << endl
              << "Data = "        << getHexString(element.get_uint32("DataPtr")) << endl
              << "DataOffset = "  << getHexString(offset) << endl
              << endl;

        // Removing NULL's in the end of text files
        uint8_t nulls = 4;
        fls.seekg((size_t)fls.tellg() + size - nulls);
        for (; nulls && (fls.get() != 0); --nulls);
        if (nulls)
          embed->resize(size - nulls);
        break;
      }

      case ROOT_DISK_ELEM_TYPE:
      case CUST_DISK_ELEM_TYPE:
      case USER_DISK_ELEM_TYPE:
      {
        element.load(fls, "version_data");

        if (size != element.size()+element.get_uint32("DataLength"))
          throw_exception("Element length does not match element type: "+ PrgElementName[type]);

        DownloadFile* file = new DownloadFile(fls_file);
        uint32_t offset = element.get_uint32("DataOffset");
        uint32_t length = element.get_uint32("DataLength");
        file->name = fname(fls_file);

        switch (type)
        {
        case ROOT_DISK_ELEM_TYPE: file->type = get_memory_class(MEMORY_CLASS_ROOT_DISK); break;
        case CUST_DISK_ELEM_TYPE: file->type = get_memory_class(MEMORY_CLASS_CUST_DISK); break;
        case USER_DISK_ELEM_TYPE: file->type = get_memory_class(MEMORY_CLASS_USER_DISK); break;
        }

        debug << PrgElementName[type] << endl
              << "UID = "         << getHexString(uid) << endl
              << "DataLength = "  << getHexString(length) << endl
              << "Data = "        << getHexString(element.get_uint32("DataPtr")) << endl
              << "DataOffset = "  << getHexString(offset) << endl
              << endl;

        // Create the LoadMap & Save Embedded file info
        MemoryRegion* region = new MemoryRegion(fls_file, fls.tellg(), length);
        region->area = new MemoryArea();
        region->area->address = fls.tellg();
        region->area->size = length;

        file->load_map.push_back(region);
        m_ref.download_files.push_back(file);
        break;
      }
      default:
        throw_exception("Unknown element type: "+ Stringify(type));
    }

  } while (!done && !fls.eof());

  debug << "Fixing the MemoryRegion Naming..." << endl;
  for (i=0; i < m_ref.download_files.size(); ++i)
  {
    DownloadFile* file = m_ref.download_files[i];
    for (j=0; j < file->load_map.size(); ++j)
    {
      MemoryRegion* region = file->load_map[j];

      if ( region->name().empty() )
        region->rename( generateFileName(file->name, i, file->type.name,
                                         "LoadMap"+ Stringify(j) +".bin"));
    }
  }

  debug << "Fls2Parser(" << m_ref.m_fls_file << ") Done." << endl;
  // m_ref.dump();
}


  } /* namespace fls */
} /* namespace ipa */

