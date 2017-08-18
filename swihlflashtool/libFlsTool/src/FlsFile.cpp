/*
 *       Copyright (C) 2011-2014 Intel Mobile Communications GmbH
 *
 *            Sec Class: Intel Confidential (IC)
 *
 *
 * This document contains proprietary information belonging to IMC.
 * Design A/S. Passing on and copying of this document, use and communication
 * of its contents is not permitted without prior written authorisation.
 *
 * Revision Information:
 *  $File name:  /msw_tools/FlashTool/libFlsTool/src/FlsFile.cpp $
 *  $CC-Version: .../dhkristx_140314_v0/6 $
 *  $Date:       2014-05-01    12:36:28 UTC $
 */

#include "FlsFile.h"
#include "Convert.h"         //< Parsing the Intel HEX format
#include "Exceptions.h"      //< throw_exception()
#include "StringExt.h"
#include "SecPack.h"
#include "SmartContainer.h"  //< find_in_vector()

#include "Sha_x.h"
#include "Pkcs.h"

#include "EBL2_nvm.h"      //< Locating NVM_TAGS
#include "Fls2Parser.h"
#include "Fls3Parser.h"

#include <cassert>
#include <iomanip>
#include <algorithm>       //< std::find()

using namespace std;
using namespace ipa;
using namespace ipa::fls;
using namespace ipa::crypto;

// TODO: Where does this PartitionMarker come from??
#define NAND_PARTITION_MARKER      0x50505056

/********************
 * ANSI C Interface *
 ********************/
uint8_t verify_fls(const char* file)
{
  try {
    FlsFile fls(file);
  }
  catch (exception& e)
  {
    #ifndef __EXCEPTIONS
      myexception& e = chatched_exceptions.front();
    #endif

    string type = detect_file_type(string(file));
    if (type.compare("Binary") != 0)
      cerr << " Fls parsing error> " << e.what() << " (Type: " << type << ")" << endl;
    return false;
  }
  return true;
}

#if 1
  /**
   * Dumps data just like "hexdump -C", but converts all non-ANSCII into '*'
   */
  void hexDump(const char* memory, size_t size, size_t offset)
  {
    size_t i;
    char str[18];
    memset(str, 0, 18);
    char* ptr = str;
    printf("Content: \n  %08X  ", (unsigned int)offset);
    for (i=1; i<size+1; ++i)
    {
      printf("%02X ", (unsigned char)memory[i-1]);
      if (!(i % 8)) printf(" ");
      if (!(i % 16)) {
        *ptr++ = 0;
        printf("|%s|\n  %08X  ", str, (unsigned int)(i+offset));
        ptr = str;
      } else {
        unsigned char c = memory[i-1];
        if (c < 32 || c > 127)
          c = '*';
        *ptr++ = c;
      }
    }
    printf("\n");
  }

  void hexDump(vector<uint8_t> data, size_t size, size_t offset)
  {
    if (!size)
      size = data.size() - offset;

    hexDump((const char*)&data[offset], size, offset);
  }
#endif

/**
 * Constructor - Load an FLS file
 *
 * @param path    Path of the Fls file
 * @param verbose Verbosity during loading. Can be set later via set_verbose()
 **/
FlsFile::FlsFile(string path, int verbose)
{
  saved = false;
  set_verbose(verbose);

  if (!path.empty())
    load(path);
}

/**
 * Destructor - Remove all tmp files
 */
FlsFile::~FlsFile()
{
  if (!saved && m_tmp_files.size())
  {
    cout << "Leaving tmp files:" << endl;
    for(size_t i=0; i < m_tmp_files.size(); ++i)
      cout << " -> " << m_tmp_files[i] << endl;
  }
  else
  if (m_tmp_files.size())
  {
    for(size_t i=0; i < m_tmp_files.size(); ++i)
    {
      if (!fexists(m_tmp_files[i]))
        cout << " File " << m_tmp_files[i] << " was already removed" << endl;
      else if (!FileSystem_remove(m_tmp_files[i]))
        cout << " Error> " << __func__ << " Failed to remove " 
             << m_tmp_files[i] << ": " << GetErrorString(0) << endl;
    }

    string tmp = FileSystem_tmpdir();
    if (fexists(tmp))
      if (!FileSystem_rmdir(tmp))
        cout << " Error> " << __func__ << " Failed to remove "
                           << tmp << ": " << GetErrorString(0) << endl;
  }
}

/**
 * Load an Fls file, saving the meta data only.
 *
 * @param path  The path to load
 **/
void FlsFile::load(string path)
{
  if (!fexists(path))
    throw_exception("File "+ string(path) +" does not exist!");

  if (!FileSystem_isfile(path))
    throw_exception("Path "+ string(path) +" is not a file!");

  m_name = fname(path);
  m_path = fcomplete(path);
  if (m_path.empty())
    throw_exception("Filename "+ m_name +" not found");

  string type = detect_file_type(path);
  info << " [Loading] " << path << " (" << type << ")" << endl;

  if (type.compare("Fls3") == 0)
  {
    Fls3Parser(*this, path, m_verbosity-2);
  }
  else if (type.compare("Fls2") == 0
        || type.compare("FFC") == 0
        || type.compare("Fls2NandBin") == 0)
  {
    // cout << " Parsing Fls2 file with verbosity " << (m_verbosity-1) << endl;
    Fls2Parser(*this, path, m_verbosity-2);

    // Generate SecPacks that wasn't loaded
    for (size_t i=0; i < download_files.size(); ++i)
    {
      DownloadFile* file = download_files[i];
      if (file->sec_pack.size() == 0)
        generate_sec_pack2(file, file->type);

      // file->dump();
    }
  }
  else
  {
    throw_exception("Input file is not FLS type ["+path+", Type: "+type+"]");
  }

  // We loaded a PRG file if these two lists are empty
  if (boot_files.empty() && download_files.empty())
    prg_loaded = true;
}

/**
 * Save the Fls file (Fls3 only)
 *
 * If the imported file was Fls2 all the embedded files are extracted to a
 * temporary folder and then added to the archive.
 *
 * For Fls3 the metadata is dumped to a temporary file and then updated.
 *
 * @param path  The full path to save to (including filename)
 */
void FlsFile::save(string path, bool overwrite)
{
  vector<uint8_t> buf;

  if (path.empty())
  {
    string ext = (version == 3) ? ".fls3" : ".fls";
    path = FlsFile_get_unique_name(m_path, ext, overwrite);
  }
  else if (fexists(path))
  {
    if (!overwrite)
      throw_exception("Output file '"+ path +"' exists!\n\t Use --replace to overwrite");

    if (overwrite && remove(path.c_str()))
      throw_exception("Error deleting "+ path);
  }
  else if (path_is_relative(path.c_str()))
  {
    string dir = fdir(path);
    if (!FileSystem_mkdir(dir))
      throw_exception("Could not create directory: "+dir);
  }

  // m_archive.set_verbose(0);
  if (!json_file)
  {
    debug << "   Creating meta.json" << endl;
    json_file = new FlsEmbedFile(m_fls_file, META_FILE_NAME);
  }

  last_modified = GetTimestamp(0);
  // json_file->modified( last_modified );
  json_file->set_content( get_meta_data(buf) );

  if (version == 2)
  {
    Fls2Parser::save(*this, path, m_verbosity-1);
    info << " [Saved] " << path << " (Fls2)" << endl;
  }
  else if (version == 3)
  {
    Fls3Parser::save(*this, path, m_verbosity-1);
    info << " [Saved] " << path << " (Fls3)" << endl;
  }
  else
  {
    throw_exception("Version not defied!");
  }
  saved = true;
}

/**
 * Extract an fls file from an embedded file via its memory map tag.
 *
 * Extract all embedded files including a JSON file with the meta data from
 * a tag specified by the user
 *
 * The algorithm requires the tag used in the load map is identically to
 * the type id in boot files.
 *
 * @param in   The full path to extract from (including filename)
 * @param out  The path to the output directory
 * @param tag  The tag to extract
 * @param file The original file FlsFile to extract from
 */
void FlsFile::import_tag(FlsFile& file, string tag, string path)
{
  string filename;
  DownloadFile* dl_file = NULL;
  EmbeddedFile* boot_file = NULL;
  vector<MemoryArea*> area;
  size_t k;

  info << " [Extract " << tag << "] " << path << endl;

  area = file.getMemoryAreaByTag(tag);
  if (area.size() == 0)
    throw_exception("Could not find '"+ tag +"'");

  for (k=0; k < area.size(); k++)
  {
    boot_file = file.getBootFile(area[k]->type.id);
    dl_file = file.getDownloadFileByAddress(area[k]->address, area[k]->type.id);

    if (dl_file || boot_file)
      break;
  }

  if (k >= area.size())
    throw_exception("Could not find a download or a boot file with tag '"+ tag +"'"
                   +" in '"+ m_fls_file +"'");

  if (boot_file)
  {
    debug << "  Tag '" << tag << "' belongs to BootFile '" << boot_file->name() << "'..." << endl;

    filename = boot_file->fls_name();

    debug << "   Generating new download file..." << endl;
    DownloadFile* new_file = new DownloadFile(filename);
    new_file->type = area[k]->type;
    new_file->timestamp = GetTimestamp( fmodified(path.c_str()) );

    debug << "Clone Boot file & rename to MemoryRegion..." << endl;
    MemoryRegion* region = new MemoryRegion(boot_file);
    region->rename( generateFileName(filename,0,new_file->type.name,"LoadMap0.bin") );
    region->area = getMemoryAreaByAddr(area[k]->address, area[k]->type.id);
    region->hash = boot_file->hash;

    new_file->load_map.push_back(region);
    download_files.push_back( new_file );
    generate_sec_pack2(new_file, boot_file->type);
  }
  else if (dl_file)
  {
    debug << "  Tag '" << tag << "' belongs to Download File '" << dl_file->name << "'..." << endl;
    cloneDownloadFile( dl_file );
  }

  // For each boot file/download file, there CAN exist a meta file
  // This HAS to be added together with the boot/download file
  for (size_t j=0; j<file.meta_files.size(); j++)
  {
    if (area[k]->type.id == file.meta_files[j]->type.id)
      cloneMetaFile( file.meta_files[j] );
  }

  if (area[k]->type.id == MEMORY_CLASS_PSI)
    return;

  // Then extracting dl file all boot and meta files must be copied
  if (dl_file)
  {
    for (size_t i=0; i<file.meta_files.size(); i++)
      cloneMetaFile( file.meta_files[i] );

    for (size_t i=0; i<file.boot_files.size(); i++)
      cloneBootFile( file.boot_files[i] );

    return;
  }

  // For each boot file, there CAN exist a meta file
  // This HAS to be added together with the boot file
  for (size_t i=0; i<file.boot_files.size(); i++)
  {
    if (area[k]->type.id == file.boot_files[i]->type.id)
      continue;

    cloneBootFile( file.boot_files[i] );
    for (size_t j=0; j<file.meta_files.size(); j++)
    {
      if (file.meta_files[j]->type.id == file.boot_files[i]->type.id)
        cloneMetaFile( file.meta_files[j] );
    }
  }
}

/**
 * Clears the meta and archive files
 */
void FlsFile::clear_files()
{
  if (json_file)
  {
    delete json_file;
    json_file = NULL;
  }

  boot_files.clear();
  meta_files.clear();
  download_files.clear();
}

/**
 * Extract all embedded files including a JSON file with the metadata
 *
 * @param path   The full path to extract to (including filename)
 */
void FlsFile::extract(string path)
{
  //< Extracting path
  if (path.empty())
  {
    path = m_name;
    size_t ext = path.find_last_of('.');

    if (ext != string::npos)
      path = path.substr(0, ext);
  }
  if (path[path.size()-1] != SEPARATOR)
    path += SEPARATOR;

  if (!FileSystem_mkdir(path))
    throw_exception("Could not create path: "+path);

  vector<FlsEmbedFile*> list = get_file_list();
  debug << "Extracting Files (" << list.size()+1 << "):" << endl;

  // Saving JSON meta data
  string meta_file = path+"meta.json";
  FlsMetaFile::save(meta_file);

  info  << " [Extract] " << setw(10) << right
        << fsize(meta_file) << " " << meta_file << endl;

  // Saving the Embedded file list
  for (size_t i=0; i<list.size(); ++i)
  {
    FlsEmbedFile *file = list[i];
    if (file->name().compare("meta.json") != 0)
    {
      info  << " [Extract] " << setw(10) << right << file->size()
            << " " << path << file->name() << endl;

      file->save(path);
    }
  }
}

/**
 * Extract the Metadata via std::ostream
 **/
ostream& FlsFile::metadata(ostream& stream)
{
  // cout << "FlsFile::metadata of " << m_name << endl;
  return dump(stream);
}

ostream& operator <<(ostream& stream, FlsFile* file)
{
  return file->metadata(stream);
}

ostream& operator <<(ostream& stream, FlsFile& file)
{
  return file.metadata(stream);
}

/**
 * Dump all SecPack's to stdout
 */
void FlsFile::dump_sec_pack()
{
  SecPack sec_pack;

  for (size_t i=0; i < download_files.size(); ++i)
  {
    vector<uint8_t> buffer;
    DownloadFile* dl_file = download_files[i];

    info << "  " << dl_file->name << " SecPack:" << endl;

    dl_file->sec_pack.get_content( buffer );

    sec_pack.load(buffer);
    sec_pack.dump();
  }
}

/*************************
 *    Helper functions   *
 *************************/

/**
 * Find a unique file name for a new file.
 * Optionally you can define a new extension for the file.
 * If the filename with the new extension exists, it will append "_X.ext"
 * where X is an increasing number, until it finds a unique name.
 *
 * @param name       File name to work from
 * @param ext        New extension for the unique name (default "")
 * @param overwrite  If true the new filename is removed if present (default false)
 *
 * @return       A truly unique file name
 */
string FlsFile_get_unique_name(string name, string ext, bool overwrite)
{
  int i = 0;
  string path = fname(name);

  if (!ext.empty())
  {
    size_t pos = path.find_last_of('.');
    if (pos != string::npos)
      path = path.substr(0, pos);
  }

  string fullname = path+ext;

  if (!fexists(fullname))
    return fullname;

  if (overwrite && remove(fullname.c_str()))
  {
    throw_exception("Error deleting "+ fullname, string(""));
  }
  else
  {
    while (fexists(path +"_"+ Stringify(i) + ext)) i++;
    path = path + "_"+ Stringify(i) + ext;
  }

  return path;
}

/**
 * Validate a directory path.
 *
 * Throws an exception if the path is empty or the path can not be created.
 *
 * @param path  The directory path
 * @param type  A text string for the exceptions
 */
static void FlsFile_validate_dir( string& path, string type = "")
{
  if (path.empty())
    throw_exception("Please specify a "+ type +" path");

  if (!FileSystem_isdir( path ))
    if (!FileSystem_mkdir( path ))
      throw_exception("Can not create the "+ type +" path: "+ path);

  if (path[path.size()-1] != SEPARATOR)
    path += SEPARATOR;
}

/**
 * Validate a file path.
 *
 * Throws an exception if the path is empty or not existing
 *
 * @param path  The file path
 * @param type  A text string for the exceptions
 */
static void FlsFile_validate_file( string& path, string type = "")
{
  if (path.empty())
    throw_exception("Please specify a "+ type +" file path");

  if (!FileSystem_isfile( path ))
    throw_exception("Path "+ path +" is not a file path");
}

/**
 * Parse an input path/tag to see if a TagId or PartitionId is specified
 */
static string FlsFile_parse_path(string path, size_t *tag_id, size_t *partition_id)
{
  stringstream ss;
  size_t colon = path.find(':');
  size_t hash  = path.find('#');

  if (!tag_id || !partition_id)
    throw_exception("Need to specify TagId & PartitionId pointers.", "");

  if (colon != string::npos || hash != string::npos)
  {
    if (colon != string::npos)
      path[colon] = ' ';

    if (hash != string::npos)
      path[hash] = ' ';

    ss << path;

    if (colon < hash)
      ss >> path >> *tag_id >> *partition_id;
    else
      ss >> path >> *partition_id >> *tag_id;
  }

  if (*tag_id > 0)
    (*tag_id)--;      //< Tag IDs start at 1

  return path;
}


/**
 * Generate the SecPack from the current
 * PartitionTable & DownloadFile list
 */
void FlsFile::generate_sec_pack2(DownloadFile* dl_file, MemoryClass& type)
{
  SecPack sec_pack;
  vector<uint8_t> buffer;

  size_t UID = download_files.size();
  if (UID > 0) UID--;
  string sec_name = generateFileName(dl_file->name, UID,
                                     dl_file->type.name, "SecureBlock.bin");

  dl_file->sec_pack.rename(sec_name);

  if (dl_file->sec_pack.size() == 0)
  {
    info << " [SecPack] Generating empty " << sec_name << endl;
    sec_pack.set_uint32("BootCoreVersion", 0x00020000)
            .set_uint32("EblVersion", 0x00020000)
            .set_uint32("LoadMagic", 0x1FEDABE);
  }
  else
  {
    info << " [SecPack] Overwriting " << sec_name << " (Size: "
         << getHexString(dl_file->sec_pack.size()) << ")" << endl;
    dl_file->sec_pack.get_content( buffer );
    sec_pack.load(buffer);
  }

  if (partitions.size())
  {
    debug << " [SecPack] Creating Partition table (" << getHexString(partition_marker) << ")..." << endl;
    sec_pack.set_uint32("PartitionMarker", partition_marker);

    vector<uint32_t> load_addresses;
    load_addresses.reserve( partitions.size() );

    for (size_t j=0; j < partitions.size(); ++j)
    {
      NandPartition* part = partitions[j];
      load_addresses.push_back( part->address );

      sec_pack.get_child("Partition", j)
              .set_uint32("ID", part->id)
              .set_uint32("StartBlock", part->start_block)
              .set_uint32("EndBlock", part->end_block)
              .set_uint32("MaxBlocksInSubPartition", part->max_block_in_sub_par)
              .set_uint32("StartReservoirBlock", part->start_reservoir)
              .set_uint32("EndReservoirBlock", part->end_reservoir)
              .set_uint32("ImageType", part->type)
              .set_uint32("Options", part->options)
              .set_uint32("Reserved", part->attributes);
    }

    sec_pack.set_uint32_array("LoadAddrToPartition", load_addresses);
  }

  sec_pack.set_uint32("Type", (uint32_t)dl_file->type.id);

  for (size_t i=0; i < dl_file->load_map.size(); ++i)
  {
    uint64_t area_address = 0;
    uint64_t area_size = 0;

    MemoryRegion* region = dl_file->load_map[i];
    if (region->area)
    {
      area_address = region->area->address;
      area_size = region->area->size;
    }

/*
    debug << "LoadMap[" << i << "]"
          << " StartAddr="   << getHexString(area_address)
          << " TotalLength=" << getHexString(area_size)
          << " UsedLength="  << getHexString(region->size())
          << " ImageFlags="  << getHexString(region->flags.sum())
          << endl;
*/
    sec_pack.get_child("LoadMap", i)
            .set_uint32("StartAddr", (uint32_t)area_address)
            .set_uint32("TotalLength", (uint32_t)area_size)
            .set_uint32("UsedLength", (uint32_t)region->size())
            .set_uint32("ImageFlags", (uint32_t)region->flags.sum());
  }

  // Restrict SecPack to a specific NAND Partition
  if (dl_file->load_map[0]->area->tag.find('#') != string::npos)
  {
    size_t tag_id = 0, partition_id = 0;
    FlsFile_parse_path(dl_file->load_map[0]->area->tag, &tag_id, &partition_id);

    debug << " [SecPack] Use Partition ID #" << partition_id << endl;
    sec_pack.set_uint8("UseAlternatePartition", true);
    sec_pack.set_uint32("PartitionId", partition_id);
  }

  dl_file->sec_pack.set_content( sec_pack.buffer() );
}



/*************************
 *    Inject NVM parts   *
 *************************/

/**
 *  Append/Update a meta file in the archive.
 *  If the file doesn't exist it will be created.
 *  If it does it will be overwritten.
 *
 * @param path   Path to the file
 *
 * @return       Reference to the newly appended file
 */
EmbeddedFile* FlsFile::inject_meta_file(string path, string name, bool replace)
{
  FlsFile_validate_file(path, "Appending meta file");

  info << " [Append Meta] " << path << " -> " << name << endl;

  EmbeddedFile* embed = getMetaFile( name, false );
  if (embed)
  {
    if (!replace)
      throw_exception("Metafile '"+ name +"' already exists in "+ m_path, NULL);

    info << " Warning> Overwriting metafile: " << name << " (" << embed->name() << endl;
  }
  else
  {
    embed = getMetaFile( name, true );
  }

  embed->type = get_memory_class( detect_file_type(path) );
  embed->set_content(path);

  return embed;
}


/**
 * Append a download file section to the Fls
 *
 * @param tag   The MemoryMap input tag
 * @param type  The memory class to append
 * @param path  The file to append (iHex or Fls)
 */
void FlsFile::append(string tag, string path, string name)
{
  MemoryArea* area = NULL;
  vector<MemoryArea*> area_list;
  string type, timestamp, tmp;

  size_t tag_id = 0, partition_id = 0;
  path = FlsFile_parse_path(path, &tag_id, &partition_id);

  FlsFile_validate_file(path, "Append "+ tag );
  type = detect_file_type(path);
  info << " [Append " << tag << "] " << path << " (" << type << ")" << endl;

  timestamp = GetTimestamp( fmodified(path.c_str()) );

  if (name.empty())
    name = fname(path);

  if (!tag.empty())
  {
    area_list = getMemoryAreaByTag(tag);
    if (area_list.size() <= tag_id)
      throw_exception("Could not find '"+ tag +"["+ Stringify(tag_id) +"]' class in MemoryMap");
    area = area_list[tag_id];
  }

  if (type.compare(0, 3, "Fls") == 0)
  {
    FlsFile fls(path);

    // Compare MemoryMap if existing in input file
    if (!prg_loaded && fls.memory_map.size() > 0)
    {
      if (!compare_memory_map(fls))
        throw_exception("MemoryMap mismatch between "+ m_fls_file +" & "+ path);
    }

    if (fls.download_files.size() == 0)
      throw_exception("No download files in "+ path);

    // Note: Boot files are ignored unless TAG is defined as PSI or EBL
    for (size_t i=0; i < fls.download_files.size(); ++i)
    {
      DownloadFile* file = fls.download_files[i];

      if (area)
      {
        debug << "Injecting from " << path << " based on MemoryMap Tag '" << tag << "'..." << endl;
        area_list = getMemoryAreaByTag(tag, file->type.id);
        if (area_list.size() <= tag_id)
          throw_exception("Could not find '"+ tag +"["+ Stringify(tag_id) +"]' class in MemoryMap");
        area = area_list[tag_id];

        info << " [Append " << area->type.name << "] " << file->name << " (" << path << ")" << endl;
        cloneDownloadFile(file, tag);
      }
      else
      {
        MemoryRegion* region = file->load_map[0];
        uint64_t addr = region->area->address;

        area = getMemoryAreaByAddr(addr, file->type.id);
        if (!area)
          throw_exception("Could not find address '"+ getHexString(addr) +"' in MemoryMap");

        if (!is_bootable(area))
        {
          debug << " Cloning DownloadFile " << file->name << " into " << area->tag << endl;
          cloneDownloadFile(file, area->tag);
        }
        else
        {
          debug << "Injecting from " << path << " based on MemoryMap address" << endl;

          if (file->load_map.size() > 1)
            throw_exception("Expected a single LoadMap in the Bootable file "+ file->name);

          debug << " Creating BootFile from " << file->type.name << " into " << area->tag  << endl;

          EmbeddedFile* boot = getBootFile(area->type.id, true);
          boot->type = area->type;
          boot->setup( region );
          boot->rename( file->name +"_inj_"+ area->type.name +".bin" );

          info << " [Append " << area->type.name << "] " << region->name() << " (" << boot->name() << ")" << endl;
        }

        // Clone associated meta files
        EmbeddedFile* meta = fls.getMetaFile(region->area->type.id);
        if (meta)
        {
          debug << " Adding metafile[" << meta->name() << "] for " << area->tag << endl;
          cloneMetaFile( meta );
        }
      }
    }
  }
  else if (type.compare("HEX") == 0)
  {
    if (!area)
      throw_exception("Need a --tag for where to put the code.");

    string description = generateFileName(name, download_files.size(), area->type.name);
    string bin = FileSystem_tmpdir() + description +"LoadMap0.bin";

    info << " [ConvertHex] " << path << " -> " << bin << endl;

    IntelHexFile iHexFile(path);

    if (area->address != iHexFile.get_base_addr())
      throw_exception("BaseAddr ("+ getHexString(iHexFile.get_base_addr()) +") is different from "
                      "MemoryArea address ("+ getHexString(area->address) +")");

    iHexFile.save(bin);
    m_tmp_files.push_back(bin);

    //< Append file and keep the timestamp of the Hex file
    debug << " [Append " << area->type.name << "] " << bin << endl;

    DownloadFile* file = append_file(area, bin, name);
    file->timestamp = timestamp;
  }
  else
  {
    /**
     *  Type identified as Binary.. Using other methods for detecting.
     */
    if (!area)
      throw_exception("Need --tag to find the appropriate class in MemoryMap");

    if (string(fext(path)).compare("iffs") == 0)
    {
      debug << " Guessing for IFFS type (" << fext(path) << ")..." << endl;
      append_iffs_file(tag, path, name);
    }
    else if (area->type.id == MEMORY_CLASS_CUST)
    {
      debug << " Inserting Binary -> CUST section (" << fext(path) << ")..." << endl;

      DownloadFile* file = append_file(area, path, name);
      file->timestamp = timestamp;
    }
    else
    {
      string tag  = area ? area->tag : "Not found";
      string type = area ? area->type.name : "None";
      throw_exception("Could not determine the type of file "+ path
                     +" (Area: "+ tag +", Type: "+ type +")");
    }
  }
}


/**
 * Append a download file section to the Fls from a list of input files.
 * Hex files are merged together before appending, while other files are
 * individually appended.
 *
 * @param tag    The MemoryMap input tag
 * @param paths  List of files to append (iHex or Fls)
 * @param name   Name of the download file
 */
void FlsFile::append_list(string tag, vector<string> paths, string name)
{
  string type, timestamp;
  vector<string> hex_files;

  for (size_t i=0; i < paths.size(); ++i)
  {
    size_t tag_id = 0, partition_id = 0;
    string path = FlsFile_parse_path(paths[i], &tag_id, &partition_id);

    FlsFile_validate_file(path, "Append "+ tag );
    type = detect_file_type(path);
    if (type.compare("HEX") == 0)
    {
      hex_files.push_back(paths[i]);
    }
    else
    {
      append(tag, paths[i], name);
    }
  }

  if (hex_files.size())
  {
    if (tag.empty())
      throw_exception("Need a --tag for Hex files");

    vector<MemoryArea*> area = getMemoryAreaByTag(tag);
    if (area.size() == 0)
      throw_exception("Could not find '"+ tag +"' class in MemoryMap");

    string desc = generateFileName(name, download_files.size(), area[0]->type.name);
    string bin = FileSystem_tmpdir() + desc +"LoadMap0.bin";

    IntelHexFile iHexFile;
    for (size_t i=0; i < hex_files.size(); ++i)
    {
      info << " [ConvertHex] " << hex_files[i] << " -> " << bin <<  endl;
      iHexFile.add(hex_files[i]);
    }

    debug << " [ConvertHex] Restricting the iHex to the MemoryMap entry (StartAddr: "
          << getHexString(area[0]->address) << ", Size: " << getHexString(area[0]->size) << ")" << endl;
    iHexFile.set_base_addr(area[0]->address);
    iHexFile.set_max_length(area[0]->size);

    iHexFile.save(bin, true);
    m_tmp_files.push_back(bin);

    //< Append file and keep the timestamp of the Hex file
    info << " [Append " << area[0]->type.name << "] " << bin << endl;
    DownloadFile* file = append_file(area[0], bin, name);
    file->timestamp = GetTimestamp( fmodified(paths[0].c_str()) );
  }
}

/**
 *  Append/Update an embedded DownloadFile
 *  If the file doesn't exist it will be created.
 *  If it does it will be overwritten.
 *
 * @param area   MemoryMap location of the file
 * @param path   Path to the file
 * @param name   Name of the origin file
 *
 * @return       Reference to the newly appended file
 */
DownloadFile* FlsFile::append_file(MemoryArea* area, string path, string name)
{
  FlsFile_validate_file(path, "Appending "+ area->tag +" file of type "+ area->type.name);

  size_t file_size = fsize(path);
  if (area->size < file_size)
    throw_exception("Not enough space in "+area->type.name+" for "+path
                   +" ("+ getHexString(area->size) +" < "+ getHexString(file_size), NULL);

  if (name.empty())
    name = fname(path);

  DownloadFile* file = getDownloadFile( name, area->type, true );
  if (file->timestamp.empty())
    file->timestamp = GetTimestamp( fmodified(path.c_str()) );

  MemoryRegion* region = new MemoryRegion(m_fls_file, path);

  size_t UID = 0;
  while (file != download_files[UID]) UID++;
  region->rename( generateFileName(name, UID, area->type.name,
                                   "LoadMap"+ Stringify(file->load_map.size()) +".bin"));

  region->area = area;
  region->set_content(path);

  file->load_map.push_back( region );

  //< Generating the Fls2 SecPack
  generate_sec_pack2(file, area->type);

  return file;
}

/**
 *  Append/Update a file in the archive.
 *  If the file doesn't exist it will be created.
 *  If it does it will be overwritten.
 *
 * @param path   Path to input file (Binary or Fls)
 * @param id     The MemoryClass ID to append (MEMORY_CLASS_BINARY for both?)
 *
 * @return       Reference to the newly appended file
 */
EmbeddedFile* FlsFile::append_boot_file(string path, MemoryClassId id)
{
  size_t i = 0;
  EmbeddedFile* new_boot_file = NULL;
  string type_name = get_memory_class(id).name;
  vector<MemoryArea*> area = getMemoryArea(id);
  if (area.size() == 0)
    throw_exception("No area of type "+ type_name, NULL);

  if (area.size() > 1)
  {
    if (area[i]->tag.find("RAM") == string::npos) i++;
    if (area[i]->tag.find("RAM") == string::npos)
      throw_exception("Could not find area with type '"+ type_name +"' and 'RAM' in its tag", NULL);
  }

  FlsFile_validate_file(path, "Appending "+ area[i]->tag +" file of type "+ type_name);

  string name = fname(path);
  string type = detect_file_type(path);
  debug << " [AppendBootFile] " << path << " (" << type << ")" << endl;

  if (type.compare(0, 3, "Fls") == 0)
  {
    FlsFile fls(path);

    EmbeddedFile* boot_file = fls.getBootFile(id);
    if (boot_file)
    {
      info << " [Append " << type_name << "] " << boot_file->name() << " (" << name << ")" << endl;
      new_boot_file = cloneBootFile(boot_file);
    }
    else
    {
      DownloadFile* dl_file = NULL;
      MemoryRegion* region = NULL;
      for (size_t j=0; j<fls.download_files.size(); ++j)
      {
        if (fls.download_files[j]->type.id == id)
        {
          dl_file = fls.download_files[j];
          region = dl_file->load_map[0];
        }
      }

      if (!dl_file)
        throw_exception("Could not find any download region in "+ path +" with type "+ type_name, NULL);

      if (area[i]->size < region->size())
        throw_exception("Not enough space in "+ type_name +" for "+ region->name(), NULL);

      debug << " Importing BootFile from MemoryRegion " << region->name() << " MemoryRegion into " << area[i]->tag  << endl;

      new_boot_file = getBootFile(dl_file->type.id, true);
      new_boot_file->type = dl_file->type;
      new_boot_file->setup(region);
      new_boot_file->rename( name +"_inj_"+ type_name +".bin" );

      info << " [Append " << type_name << "] " << region->name() << " -> " << new_boot_file->name() << endl;

      uint16_t hash = new_boot_file->get_hash(HASH_XOR16);
      new_boot_file->hash[HASH_XOR16] = getHexString(hash, 4);

      // Clone associated meta files
      EmbeddedFile* meta = fls.getMetaFile(dl_file->type.id);
      if (meta)
      {
        string new_name = meta->name();
        replace(new_name, "_version.txt", "_"+dl_file->type.name+"_ver.txt");

        EmbeddedFile* new_meta = cloneMetaFile( meta );
        new_meta->rename( new_name );

        debug << " [MetaFile " << area[i]->tag << "] " << meta->name() << " -> " << new_meta->name() << endl;
      }
    }
  }
  else
  {

  if (area[i]->size < fsize(path))
    throw_exception("Not enough space in "+ type_name +" for "+ path, NULL);

    new_boot_file = getBootFile(area[i]->type.id, true);
    new_boot_file->rename(name+"_inj_"+ type_name +".bin");
    new_boot_file->type = area[i]->type;

    new_boot_file->set_content(path);
  }

  return new_boot_file;
}

/**
 *  Append/Update an IFFS file in the archive.
 *  If the file doesn't exist it will be created.
 *  If it does it will be overwritten.
 *
 * @param tag   The MemoryMap input tag
 * @param path   Path to the file
 * @param name   Name of the origin file
 *
 * @return       Reference to the newly appended file
 */
DownloadFile* FlsFile::append_iffs_file(string tag, string path, string name)
{
  const char iffs_header[] =
  "{"
    "\"iffs_header\": ["
      "\"LoadAddr                  uint32\","
      "\"Signature                 uint32     5\","
      "\"Version                   uint32\","
      "\"DynamicLength             uint32\","
      "\"StaticLength              uint32\","
    "],"
  "}";

  vector<MemoryArea*> area;
  DownloadFile* file = NULL;
  StructParser iffs(iffs_header, "iffs_header");

  ifstream ifile(path.c_str(), ios::binary);
  if (!ifile)
    throw_exception("Could not open IFFS file "+path, NULL);

  iffs.load(ifile);

  string timestamp = GetTimestamp( fmodified(path.c_str()) );
  size_t dynamic_len = iffs.get_uint32("DynamicLength");
  size_t static_len = iffs.get_uint32("StaticLength");

  // Validating size of file without header of 0x100
  if ((static_len + dynamic_len) != (fsize(path) - 0x100))
    throw_exception("Invalid IFFS file type", NULL);

  ifile.seekg(0x100);

  area = getMemoryAreaByTag(tag, MEMORY_CLASS_DYN_FFS);
  if (area.size() > 0)
  {
    string bin = generateFileName(fname(path), download_files.size(), area[0]->type.name, "LoadMap0.bin");

    info << " [Append " << area[0]->type.name << "] " << bin << endl;
    save_stream_to_file(ifile, bin, dynamic_len);

    file = append_file(area[0], bin, name);
    file->timestamp = timestamp;
  }

  area = getMemoryAreaByTag(tag, MEMORY_CLASS_STAT_FFS);
  if (area.size() > 0)
  {
    string bin = generateFileName(fname(path), download_files.size(), area[0]->type.name, "LoadMap0.bin");

    info << " [Append " << area[0]->type.name << "] " << bin << endl;
    save_stream_to_file(ifile, bin, static_len);

    file = append_file(area[0], bin, name);
    file->timestamp = timestamp;
  }

  ifile.close();

  return file;
}

/**
 * @brief Get the System Version information stored in the CODE section
 *
 * @param labels  Reference to a label structure or null
 *
 * @return        The main system version string
 */
string FlsFile::get_system_version(SystemVersionLabelType* labels)
{
  size_t i, j;
  vector<uint8_t> data;
  static const char SwVersion[] = "!SYSTEM_VERSION DATA STRUCTURE!";

  if (!SysVersion.empty())
    goto done;

  for (i=0; i < download_files.size(); ++i)
  {
    for (j=0; j < download_files[i]->load_map.size(); ++j)
    {
      MemoryRegion* file = download_files[i]->load_map[j];

      if (file->size() < 1024*1024)
        continue;

      //< Only search the first MB
      size_t offset = file->offset();
      file->get_content(data, 1024*1024);

      // Search for the SYSTEM_VERSION string
      for(size_t n = 0; n < data.size()-sizeof(SwVersion); ++n)
      {
        if (strncmp((char*)&data[n], SwVersion, sizeof(SwVersion)) != 0)
          continue;

        // GoTo & read the SwVersion pointer of 4 bytes
        n += sizeof(SwVersion) + sizeof(int)*3;
        int low = (data[n] & 0xFF);
        int high = (data[n+1] & 0xFF);
        size_t SwVersionOffset = low + (int)(high<<8);

        //printf(" -> Found %s at Pos: 0x%X (EmbedFileOffset: 0x%X, FileOffset: 0x%X)\n",
        //       SwVersion, SwVersionOffset, n, offset);

        if (n > SwVersionOffset)
        {
          SwVersionOffset += NAND_SYSTEM_IMAGE_OFFSET;
          // printf(" -> NAND SwVersionOffset: 0x%X\n", (uint32_t)SwVersionOffset);
        }

        /* Parsing the strings into our Labels structure */
        ifstream fs( m_path.c_str(), ios::binary );
        if (fs)
        {
          fs.seekg(SwVersionOffset + offset);

          std::getline(fs, SysVersion, '\0');

          for(int i=0; i<3; ++i)
            std::getline(fs, SysCustomer[i], '\0');

          for(int i=0; i<3; ++i)
            std::getline(fs, SysSubsystem[i], '\0');

          fs.close();
          goto done;
        }
      }
    }
  }

done:
  /* Save labels in the label structure if parsed */
  if (labels)
  {
    labels->filename = strdup(m_name.c_str());
    labels->version = strdup(SysVersion.c_str());

    for(i=0; i<3; ++i)
      labels->customer[i] = strdup(SysCustomer[i].c_str());

    for(i=0; i<10; ++i)
      labels->subsystem[i] = strdup(SysSubsystem[i].c_str());
  }

  return SysVersion;
}


/**
 * Locates the user defined NVM tags inside DownloadFile (CODE only)
 *
 *  TODO: Move to DownloadFile since it is analyzing DownloadFile content
 *  TODO: Implement THIS in (MFH_NVM_get_nof_user_options + MFH_NVM_get_user_option)
 *
 * @param file  A Downloadfile reference (Must be CODE)
 *
 * @return A list of NVM tags
 */
vector<uint32_t> FlsFile::get_nvm_tags(DownloadFile* file)
{
  vector<uint32_t> tags;
  vector<uint8_t> buffer;
  MemoryRegion* embed = NULL;
  uint32_t magic = 0, offset = 0, version = 1;
  string nvm_body = "nvm_body_v1";
  uint32_t nvm_end_tag = NVM_TAG_TERMINATOR;

  static const char nvm_structure_json[] =
    "{\n"
    "  \"nvm_head\": [\n"
    "    \"Length         uint32\",\n"
    "    \"Tag            uint32\",\n"
    "  ],\n"
    "  \"nvm_body_v1\": [\n"
      "  \"Address        uint32\",\n"
      "  \"ByteCount      uint32\",\n"
      "  \"EraseMode      uint32\",\n"
    "  ],\n"
      "  \"nvm_body_v2\": [\n"
        "  \"Address        uint32\",\n"
        "  \"ByteCount      uint32\",\n"
        "  \"DATA           uint32         10\",\n"
        "  \"EraseMode      uint32\",\n"
      "  ],\n"
    "}\n";

  if (!file)
    throw_exception(" No file provided", tags);

  if (file->type.id != MEMORY_CLASS_CODE)
    throw_exception(file->name +" is not a CODE class", tags);

  //debug << " Reading NVM tags from " << embed->name() << "..." << endl;
  for (size_t i=0; i < file->load_map.size(); ++i)
  {
    embed = file->load_map[i];
    offset = (platform.technology == TECHNOLOGY_NOR) ?
              NVM_STRUCT_START_ADDR : NVM_STRUCT_OFFSET_NAND;

    if (embed->size() < NVM_STRUCT_MAGIC_SIZE+offset)
      continue;

    embed->get_content(buffer, NVM_STRUCT_MAGIC_SIZE, offset);
    magic = *(uint32_t*)&buffer[0];

    if (magic != NVM_STRUCT_MAGIC)
    {
      if (embed->size() < NVM_STRUCT_MAGIC_SIZE+NVM_STRUCT_OFFSET)
        continue;

      offset = NVM_STRUCT_OFFSET;
      embed->get_content(buffer, NVM_STRUCT_MAGIC_SIZE, NVM_STRUCT_OFFSET);
      magic = *(uint32_t*)&buffer[0];

      if (magic != NVM_STRUCT_MAGIC)
        continue;
    }
  }

  // 1.1 Return if Magic NVM Structure wasn't found 
  if (magic != NVM_STRUCT_MAGIC)
    return tags;

  //< 2. Get NVM Structure version (NOR is forced to 1 (EBLs are not ver2 ready))
  if (platform.technology != TECHNOLOGY_NOR)
  {
    version = *(uint32_t*)&buffer[sizeof(magic)];

    if (version == 2)
    {
      nvm_body = "nvm_body_v2";
      nvm_end_tag = NVM_TAG_TERMINATOR_V2;
    }
  }

  offset += NVM_STRUCT_MAGIC_SIZE;

  //< 3. Searching the CODE section for NVM_TAGs
  uint32_t nvm_len, nvm_tag;
  StructParser nvm_struct(nvm_structure_json);

  embed->open(ifstream::in);

  do {
    embed->seek(offset);
    nvm_struct.load(embed->stream(), "nvm_head");

    nvm_len = nvm_struct.get_uint32("Length");
    nvm_tag = nvm_struct.get_uint32("Tag");

    offset += nvm_len;

    // Only add tag if not already found
    if (nvm_len && !find_in_vector(tags, nvm_tag))
    {
      /*lint -fallthrough */
      switch (nvm_tag)
      {
        case NVM_TAG_TERMINATOR:
        case NVM_TAG_TERMINATOR_V2:
          break;

        case NVM_TAG_FOLLOW_WRITE:
        case NVM_TAG_FOLLOW_WRITE_V2:
        case NVM_TAG_CONDITIONAL:
        case NVM_TAG_OFFSET:
        case NVM_TAG_SET_U32_LGT:
          // Do nothing?
          break;

        case NVM_TAG_ERASE_CAL:
        case NVM_TAG_ERASE_FIX:
        case NVM_TAG_ERASE_DYN:
        case NVM_TAG_ERASE_CAL_V2:
        case NVM_TAG_ERASE_FIX_V2:
        case NVM_TAG_ERASE_DYN_V2:
        case NVM_TAG_ERASE_MISC_V2:
          //debug << " -> NVM_TAG_ERASE <- " << endl;
          nvm_struct.load(embed->stream(), nvm_body);
          if (nvm_struct.get_uint32("EraseMode") == NVM_ERASE_OPTIONAL)
            tags.push_back( nvm_tag );
          break;

        case NVM_TAG_STARTUP_MODE_TEST:
        case NVM_TAG_STARTUP_MODE_NORMAL:
        case NVM_TAG_BLTDO:
        case NVM_TAG_STARTUP_MODE_TEST_V2:
        case NVM_TAG_STARTUP_MODE_NORMAL_V2:
        case NVM_TAG_BLTDO_V2:
          //debug << " -> NVM_TAG_STARTUP_MODE <- " << endl;
          tags.push_back( nvm_tag );
          break;

        default:
          info << "Unknown NVM tag: " << nvm_tag << endl;
          //throw_exception("Didn't understand tag '"+ Stringify(nvm_tag) +"'");
          break;
      }
      /*lint +fallthrough */
    }
  } while (nvm_tag != nvm_end_tag && !embed->eof());

  embed->close();

  debug << endl << __func__ << " Found " << tags.size() << " NVM tags in " << file->name << endl;
  return tags;
}

/**
 * Injecting NVM data area's into the fls3 file.
 *
 * If the NVM files are not present in the path pointed to they will be
 * generated as empty (0xFF) files. The files should be named:
 *
 * static.nvm
 * dynamic.nvm
 * calib.nvm
 *
 * @param path  Directory path for the NVM files
 */
void FlsFile::inject_nvm(string path)
{
  string nvm_path;
  size_t i, j;
  vector<MemoryArea*> nvm_areas;

  FlsFile_validate_dir(path, "NVM");

  info << " [Inject] Loading NVM from " << path << "..." << endl;

  // Flashless Sanity check (Needed MemoryMap entries & count)
  if (platform.technology != TECHNOLOGY_FLASHLESS)
    throw_exception("File is not Flashless");

  for (i=0; i < memory_map.size(); ++i)
  {
    MemoryArea* area = memory_map[i];

    if (area->tag.find("_NVM") == string::npos)
      continue;

    nvm_areas.push_back( area );
  }

  if (nvm_areas.size() == 0)
    throw_exception("NVM Area missing in LoadMap");

  // Going through the DownloadFile list
  for (i=0; i < download_files.size(); ++i)
  {
    DownloadFile* file = download_files[i];

    for (size_t k=0; k < file->load_map.size(); ++k)
    {
      MemoryRegion* region = file->load_map[k];

      if (region->flags.has(FLASHLESS) < 0)
        continue;

      /**
       * HACK: Locating the Area with the Binary class instead of CODE
       */
      if (region->area->type.id != MEMORY_CLASS_BINARY)
      {
        string tag = region->area->tag;
        region->area = getMemoryAreaByAddr(region->area->address, MEMORY_CLASS_BINARY);

        if (!region->area)
          throw_exception("Could not find Binary MemoryArea of NVM Region "+ tag);

        debug << "   Re-Linking region " << region->name()
              << " Area (" << tag << " => " << region->area->tag << ")" << endl;
      }

      if (region->area->tag.compare("SEC_DATA") == 0)
      {
        debug << " [Inject] Ignoring SEC_DATA" << endl;
        continue;
      }

      for(j=0; j < nvm_areas.size(); ++j)
      {
        if (region->area != nvm_areas[j])
          continue;

        string tag = nvm_areas[j]->tag;
        replace(tag, "_", ".");

        nvm_path = path + StringToLower(tag);

        // Creating NVM files if not existing
        if (!FileSystem_isfile( nvm_path ) || fsize(nvm_path) != region->area->size)
        {
          debug << " [Inject] Generating empty NVM file " << nvm_path
                << " (Size: " << getHexString(region->area->size) << ")" << endl;
          FILE_setContent(nvm_path, 0xFF, region->area->size);
          m_tmp_files.push_back(nvm_path);
        }

        info << " [Inject] " << region->area->tag << " <- " << nvm_path
             << " (Size: " << getHexString(region->area->size) << ")" << endl;
        region->set_content( nvm_path );
      }

      if (region->size() == 0)
        throw_exception(m_fls_file +" NVM Region "+ region->area->tag +" not found in MemoryMap!");
    }

    /* Regenerate the SecPack with new NVM sections */
    generate_sec_pack2(file, file->type);
  }
}


#if 0

/**
 * Injecting Security Data area into the fls3 file.
 *
 * @param path  Path to the binary security file
 */
void FlsFile::inject_security(string sec_path)
{
  size_t i,k;
  int flashless = 0;
  MemoryArea* sec_area = NULL;

  //info << "Injecting SecData from " << sec_path << "..." << endl;
  FlsFile_validate_file(sec_path, "SecData");

  // Flashless Sanity check (Needed MemoryMap entries & count)
  if (platform.technology != TECHNOLOGY_FLASHLESS)
    throw runtime_error("File is not Flashless");

  for (i=0; i < memory_map.size(); ++i)
  {
    MemoryArea* area = memory_map[i];

    if (area->tag.compare("SEC_DATA") == 0)
      sec_area = area;

    if (find_in_vector(area->flags, FLASHLESS))
      flashless++;
  }

  if (!sec_area)
    throw_exception("Security area missing in LoadMap");

  if (flashless < 4)
    throw_exception("Security element(s) missing in LoadMap");

  // Going through the DownloadFile list
  for (i=0; i < download_files.size(); ++i)
  {
    DownloadFile* file = download_files[i];

    for (k=0; k < file->load_map.size(); ++k)
    {
      MemoryRegion* region = file->load_map[k];

      if (!find_in_vector(region->flags, FLASHLESS))
        continue;

      if (region->area == sec_area)
      {
        info << " [Inject] " << file->name << "3 <- " << sec_path << endl;
        region->file->set_content(sec_path);
      }
    }
  }
}


/**
 * Injecting Certificate area into the fls3 file.
 *
 * Note: The module test case for this fails because of different CRC's
 * TODO: Figure out if this feature is being used and get a REAL testcase
 *
 * @param path  Path to the certificate file
 */
void FlsFile::inject_certificate(string cert_path)
{
  //info << "Injecting Certificate from " << cert_path << "..." << endl;
  FlsFile_validate_file(cert_path, "Certificate");

  uint8_t CertMarker[] = {0xCD, 0xAB, 0xCD, 0xAB, 0xEF, 0xCD, 0xEF, 0xCD};
  std::vector<uint8_t> new_ebl(CertMarker, CertMarker + sizeof(CertMarker));

  uint32_t cert_size = fsize(cert_path);
  new_ebl.resize( cert_size + sizeof(CertMarker) );

  EmbeddedFile* ebl = getBootFile(MEMORY_CLASS_EBL, true);

  info << " [Inject] " << ebl->name << " <- " << cert_path
       << " (Experimental)" << endl;

  ifstream cert(cert_path.c_str(), ios::binary);
  if (!cert)
    throw_exception("Could not read certificate "+ cert_path);

  cert.read((char*)&new_ebl[sizeof(CertMarker)-1], cert_size);
  cert.close();

  // 32bit Padding...
  while (new_ebl.size() % 4)
    new_ebl.push_back(0x00);

  std::vector<uint8_t> buff;
  ebl->get_content(buff);
  new_ebl.insert(new_ebl.end(), buff.begin(), buff.end());

  ebl->set_content(new_ebl);
  ebl->size = ebl->file->size();       //< EmbeddedFile::size = FlsEmbedFile::size()
}

#endif

/**
 *  Load the contents of an embedded file via its MemoryRegion
 *
 * @param region    The region to load from
 * @param buffer    Buffer to load into (make sure its big enough)
 * @param length    How much to read
 * @param offset    Where to start reading
 *
 * @return          Pointer to the beginning byte of the buffer
 */
uint8_t* FlsFile::get_content(MemoryRegion* region, std::vector<uint8_t> &buffer, uint64_t length, uint64_t offset)
{
  if (!region)
    throw_exception("MemoryRegion not found", NULL);

  if (!region->name().empty())
    throw_exception("MemoryRegion has no name", NULL);

  region->get_content(buffer, length, offset);

  return &buffer[0];
}


/**
 *  Runs a script on the currently parsed LoadMap
 *
 * @param path   Path to the script
 */
typedef enum {
  SCRIPT_ACTION_NONE,
  SCRIPT_ACTION_XOR4,
  SCRIPT_ACTION_GENSIGN,
  SCRIPT_ACTION_GENREGIONSIGN,
  SCRIPT_ACTION_SIGNFILE,
  SCRIPT_ACTION_PSISIGN,
  SCRIPT_ACTION_GENHASH,
  SCRIPT_ACTION_SIGNSECPACK,
} script_action_t;

/**
 * Swap the endianess of a U32
 */
uint32_t swap_endian_uint32( uint32_t val )
{
  val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
  return (val << 16) | (val >> 16);
}

/**
 * Create a matrix of HEX & returns the string
 */
template<typename T>
string get_hex_matrix(const T* data, size_t length, size_t row_size = 8)
{
  stringstream ss;
  for (size_t i=0; i<length; ++i)
  {
    if (!(i % row_size)) ss << endl << "   ";
    ss << " " << getHexString(data[i], 2);
  }
  ss << endl;
  return ss.str();
}

/**
 * Fixes endianess of the data in case of Big endian
 */
template<typename T>
void fix_endian(uint32_t sign_endian,  T* data, size_t length)
{
  if (sign_endian == BIG_ENDIAN_ARCH)
  {
    for (size_t i=0; i<length; ++i)
      data[i] = swap_endian_uint32(data[i]);
  }
}

/**
 * Reverse the order of bytes in an array
 */
void reverse_order(uint8_t* data, size_t size)
{
  assert(size!=0);

  char tmp[size];
  for(size_t i=0; i<size; ++i)
  {
    tmp[size-i-1] = data[i];
  }
  memcpy(data, tmp, size);
}

void FlsFile::run_script(string path)
{
  char line[256];
  stringstream ss;
  script_action_t action;         //< The action to take when hitting an END tag
  string output_file;             //< GenHash generates an output file
  string section_string;          //< A LoadMap entry tag -> Injected file
  string key_string;              //< An input signing key
  uint32_t sha_type = 1;          //< HashFunction (1: Sha1, 2: Sha256)
  uint32_t hash_length = 5;       //< Length of the MessageDigest (HashFunction)
  uint32_t hash_func = 1;         //< Supported formats (1: Big-endian, 2: Little-endian)
  uint64_t start_addr = 0, length = 0, write_addr = 0;
  uint64_t use_start_addr = 0;    //< Use alternative StartAddr (USEADDRESS)
  uint32_t no_rsa = false;
  DownloadFile* file = NULL;
  MemoryRegion* start_region = NULL;
  MemoryRegion* write_region = NULL;
  string cmd;

  FlsFile_validate_file(path, "LoadMap Script");
  action = SCRIPT_ACTION_NONE;

  info << " [PostScript] " << path << endl;

  ifstream script(path.c_str(), ios::binary);
  if (!script)
    throw_exception("Could not open script file "+ path);

  while(!script.eof())
  {
    script.getline(line, sizeof(line));

    // Skip comments
    if (line[0] == '\0' or line[0] == '#' or (line[0] == '/' and line[1] == '/'))
      continue;

    debug << " Interpreting: '" << line << "'" << endl;
    ss.str("");
    ss.clear();
    ss << line;
    ss >> cmd;

    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

    if (cmd.empty() || cmd.compare("ALIGN") == 0)
    {
      info << " > Obsolete '" << cmd << "' (ignoring)" << endl;
      continue;
    }

    /**
     *  Interpreting line
     **/
    if  (cmd.compare("SECTION") == 0)
    {
      ss >> section_string;

      MemoryClass section = get_memory_class(section_string);
      if (section.id == MEMORY_CLASS_NOT_USED)
        throw_exception("Wrong Memory Class: "+ section_string);

      for (size_t i=0; i < download_files.size(); ++i)
      {
        file = download_files[i];
        if (file->type.id == section.id)
          break;
      }
      if (file->type.id != section.id)
        file = NULL;
      else
        debug << " Signing Section " << section_string << " of " << file->name << endl;
    }
    else if  (cmd.compare("XOR4") == 0)
    {
      start_addr = 0, length = 0, write_addr = 0;
      ss >> hex >> start_addr >> length >> write_addr;
      action = SCRIPT_ACTION_XOR4;
    }
    else if  (cmd.compare("KEY") == 0)
    {
      ss >> key_string;

      FlsFile_validate_file(key_string, "Signing Key");
    }
    else if  (cmd.compare("USEADDRESS") == 0)
    {
      use_start_addr = 0;
      ss >> hex >> use_start_addr;
    }
    else if (cmd.compare("NO_RSA") == 0)
    {
      no_rsa = true;
    }
    else if (cmd.compare("USE_DER_SIGNATURES") == 0)
    {
      cout << " Ignoring USE_DER_SIGNATURES for now..." << endl;
    }
    else if  (cmd.compare("SIGNFORMAT") == 0)
    {
      ss >> sha_type;

      if (sha_type < 1 && sha_type > 2)
        throw_exception("Interpreter Error> '"+ cmd +"' only supports [1,2]");
    }
    else if (cmd.compare("HASHFUNCTION") == 0)
    {
      int hash_function;
      ss >> hash_function;

      if (hash_function < 1 && hash_function > 2)
        throw_exception("Interpreter Error> '"+ cmd +"' only supports [1,2]");

      hash_func = hash_function;
      hash_length = (hash_function==2) ? 8 : 5;
    }
    else if (cmd.compare("SKIPVERSIONCHECK") == 0)
    {
      int skip;
      ss >> skip;
      if (skip != 0)
      {
        debug << " > Adding SKIP_VERSION_CHECK to " << file->name << endl;
        file->load_map[0]->flags += SKIP_VERSION_CHECK;

        /* Regenerate the SecPack if NVM sections was injected */
        generate_sec_pack2(file, file->type);
      }
    }
    else if (cmd.compare("GENSIGN") == 0)
    {
      start_addr = 0, length = 0, write_addr = 0;
      ss >> hex >> start_addr >> length >> write_addr;
      action = SCRIPT_ACTION_GENSIGN;
    }
    else if (cmd.compare("GENREGIONSIGN") == 0)
    {
      start_addr = 0, length = 0, write_addr = 0;
      ss >> hex >> start_addr >> write_addr;
      action = SCRIPT_ACTION_GENREGIONSIGN;
    }
    else if (cmd.compare("SIGNFILE") == 0)
    {
      start_addr = 0, length = 0, write_addr = 0;
      action = SCRIPT_ACTION_SIGNFILE;
    }
    else if (cmd.compare("SIGNSECPACK") == 0)
    {
      start_addr = 0, length = 0, write_addr = 0;
      action = SCRIPT_ACTION_SIGNSECPACK;
    }
    else if (cmd.compare("PSISIGN") == 0)
    {
      start_addr = 0, length = 0, write_addr = 0;
      ss >> hex >> start_addr >> length >> write_addr;
      action = SCRIPT_ACTION_PSISIGN;
    }
    else if (cmd.compare("GENHASH") == 0)
    {
      start_addr = 0, length = 0;
      ss >> hex >> start_addr >> length >> output_file;
      if (output_file.empty())
        throw_exception("GenHash Error> I need an output file '"+path+"'!");
      action = SCRIPT_ACTION_GENHASH;
    }
    else
    {
      if (cmd.compare("LOG") && cmd.compare("END"))
        throw_exception("Interpreter Error> Didn't understand '"+cmd+"'!");
    }

    /**
     *  Continue reading script or run action
     **/
    if (action == SCRIPT_ACTION_NONE)
      continue;

    if (action == SCRIPT_ACTION_NONE)
      throw_exception("Interpreter Error> END tag without an action!");

    if (section_string.empty())
      throw_exception("Interpreter Error> SECTION field missing!");

    if (!file)
    {
      action = SCRIPT_ACTION_NONE;
      cout << " Warning> NOT Signing " << section_string << ": Section not existing" << endl;
      continue;
    }

    if (start_addr > 0)
    {
      start_region = getMemoryRegion(start_addr, length);
      if (!start_region || !start_region->area)
         throw_exception("Start MemoryRegion not found");

      start_addr -= start_region->area->address;
    }

    if (write_addr > 0)
    {
      write_region = getMemoryRegion(write_addr, sizeof(uint32_t)); //< sizeof(XOR)
      if (!write_region || !write_region->area)
         throw_exception("Write MemoryRegion not found");

      write_addr -= write_region->area->address;
      write_region->enable_write();
    }

    switch (action)
    {
      case SCRIPT_ACTION_XOR4:
      {
        uint32_t XOR = 0;
        vector<uint8_t> content;

        debug << " > Creating XOR4 Checksum"
              << "(Section: "    << section_string
              << ", StartAddr: " << getHexString(start_addr)
              << ", Length: "    << getHexString(length)
              << ", WriteAddr: " << getHexString(write_addr)
              << ")" << endl;

        if (!start_region)
           throw_exception("Read MemoryRegion not found");

        if (!write_region)
           throw_exception("Write MemoryRegion not found");

        //< Load the region to Hash over
        start_region->get_content(content, length*4, start_addr);
        XOR = crypto::get_hash(content, HASH_XOR32);

        //< Save the XOR32 in the write region & update hash
        write_region->write((char*)&XOR, sizeof(XOR), write_addr);
        XOR = crypto::get_hash(write_region->parent(), HASH_XOR16);
        write_region->hash[HASH_XOR16] = getHexString(XOR, 4);
        break;
      }

      case SCRIPT_ACTION_GENSIGN:
      case SCRIPT_ACTION_GENHASH:
      case SCRIPT_ACTION_PSISIGN:
      case SCRIPT_ACTION_GENREGIONSIGN:
      {
        SHA_X Sha;
        vector<uint8_t> buffer;
        uint8_t signature[59];
        uint32_t message_digest[8];

        if (action != SCRIPT_ACTION_GENHASH && key_string.empty())
          throw_exception(" Signing Error> Needs a 'Key' file");

        if (action == SCRIPT_ACTION_GENREGIONSIGN)
          length = start_region->size() - start_addr;

        if (!start_region)
           throw_exception("Read MemoryRegion not found");

        memset(signature, 0, sizeof(signature));
        memset(message_digest, 0, sizeof(message_digest));

        info << " > Signing "
              << "(Section: "    << section_string
              << ", ReadAddr: " << getHexString(start_addr)
              << ", Length: "    << getHexString(length)
              << ", WriteAddr: " << getHexString(write_addr)
              << ")" << endl;

        info << " > ReadRegion:  " << start_region->name() << endl;
        if (action == SCRIPT_ACTION_GENHASH || action == SCRIPT_ACTION_GENSIGN)
          start_region->get_content(buffer, length*sizeof(uint32_t), start_addr);
        else if (length > start_region->size() - start_addr)
          start_region->get_content(buffer, start_region->size() - start_addr, start_addr);
        else
          start_region->get_content(buffer, length, start_addr);

        Sha.Reset(hash_func);
        Sha.Input(&buffer[0], std::min(length,(uint64_t)buffer.size()));

        if (length > start_region->size() - start_addr)
        {
          uint64_t extra_buffer = length - start_region->size() - start_addr;
          buffer.assign(extra_buffer, 0xFF);
          Sha.Input(&buffer[0], buffer.size());
        }

        if (!Sha.Result(message_digest))
          throw_exception(" Signing Error> Could not calculate SHA digest");

        fix_endian(sha_type, message_digest, hash_length);
        debug << "  Sign:" << get_hex_matrix(message_digest, hash_length);

        start_addr += start_region->area->address;

        // TODO: Might need to make a "memcpy_endian()"
        size_t signature_length = sizeof(uint32_t)*2;
        if (use_start_addr) //< TODO: Maybe using a boolean for recognizing (addr==0)
          memcpy((uint32_t*)&signature, &use_start_addr, sizeof(uint32_t));
        else
          memcpy((uint32_t*)&signature, &start_addr, sizeof(uint32_t));

        memcpy((uint32_t*)&signature[sizeof(uint32_t)], &length, sizeof(uint32_t));

        if (action == SCRIPT_ACTION_PSISIGN)
        {
          uint8_t* DER;
          size_t len = Sha.GetDER(&DER);
          memcpy((uint32_t*)&signature[signature_length], DER, len);
          signature_length += len;
        }

        memcpy((uint32_t*)&signature[signature_length], &message_digest, sizeof(uint32_t)*hash_length);
        signature_length += sizeof(uint32_t)*hash_length;

        if (action == SCRIPT_ACTION_GENHASH)
        {
          ofstream of(output_file.c_str(), ios::binary);
          if (!of)
            throw_exception(" GenHash> Could not open "+ output_file);

          of.write((const char*)signature, signature_length);
          info << " [Saved] " << output_file << endl;
          of.close();
        }
        else
        {
          // PKCS Packing & Encryption
          uint8_t pkcs_data[512];
          uint32_t pkcs_length = 0;

          PrivateKeyStructType *key = (PrivateKeyStructType *)
            FILE_getContent(key_string, buffer, sizeof(PrivateKeyStructType));

          if (key->KeyType == DEV_KEY)
            write_addr += MAX_MODULUS_LEN;

          if (PKCS_Encrypt(pkcs_data, &pkcs_length,
                           signature, signature_length,
                           key, AREA_BLOCK_TYPE, 0, no_rsa)) //< UseSalt==0 (Disabled for good)
          {
            throw_exception("Interpreter Error> PKCS_Encrypt failed");
          }

          if (sha_type == 2)
          {
            debug << " > Reversing Encrypted data order" << endl;
            reverse_order(pkcs_data, pkcs_length);
          }

          debug << " > Encrypted data:" << get_hex_matrix(pkcs_data, pkcs_length);

          if (write_addr+MAX_MODULUS_LEN > write_region->size())
          {
            uint64_t padding = write_addr + MAX_MODULUS_LEN - write_region->size();
            debug << " > WriteRegion> Growing to " << getHexString(write_addr+MAX_MODULUS_LEN) << endl;

            buffer.assign(padding, 0xFF);
            write_region->write((char*)&buffer[0], padding, write_region->size());

            // Update SecPack with new RegionSize
            size_t uid;
            for (uid=0; uid < file->load_map.size(); ++uid)
              if (file->load_map[uid] == write_region)
                break;

// TODO: file->sec_pack.set(write_region->uid, "UsedLength", write_region->size);
            SecPack sec_pack;
            file->sec_pack.get_content( buffer );
            sec_pack.load(buffer);
            sec_pack.get_child("LoadMap", uid)
                    .set_uint32("UsedLength", write_region->size());
            file->sec_pack.set_content( sec_pack.buffer() );
          }

          //< Save the PKCS in the write region & updating hash
          info << " > WriteRegion: " << write_region->name()
               << " (Offset: " << getHexString(write_addr) << ")" << endl;

          write_region->write((char*)&pkcs_data, pkcs_length, write_addr);
          uint32_t hash = crypto::get_hash(write_region->parent(), HASH_XOR16);
          write_region->hash[HASH_XOR16] = getHexString(hash, 4);
        }
        break;
      }

      case SCRIPT_ACTION_SIGNFILE:
      case SCRIPT_ACTION_SIGNSECPACK:
      {
        SHA_X Sha;
        uint32_t sha_digest[8];
        uint8_t *signature = NULL;
        size_t signature_length = 0;
        uint8_t raw_signature[64] = {0};       //< SCRIPT_ACTION_SIGNSECPACK
        DownloadCryptoBlockStructType crypt;   //< SCRIPT_ACTION_SIGNFILE

        vector<uint8_t> buffer;
        const size_t buf_size = 1024;

        if (key_string.empty())
          throw_exception(" Signing Error> Needs a 'Key' file");

        // Getting MemoryRegion(s) signature
        for (size_t i=0; i<download_files.size(); ++i)
        {
          Sha.Reset(hash_func);
          DownloadFile* file = download_files[i];

          for (size_t j=0; j<file->load_map.size(); ++j)
          {
            MemoryRegion* region = file->load_map[j];

            info << " > ReadRegion:  " << region->name()
                 << " (StartAddr: " << getHexString(region->area->address)
                 << ", Size: " << getHexString(region->size()) << ")" << endl;

            size_t offset = 0;
            while (	(offset+buf_size) < region->size())
            {
              region->get_content(buffer, buf_size, offset);
              Sha.Input(&buffer[0], buf_size);
              offset += buf_size;
            }
            region->get_content(buffer, region->size()-offset, offset);
            Sha.Input(&buffer[0], buffer.size());
          }

          if (!Sha.Result(sha_digest))
            throw_exception("Interpreter Error> GenSign could not calculate SHA digest");

          fix_endian(sha_type, sha_digest, hash_length);
          debug << "  RegionSign:" << get_hex_matrix(sha_digest, hash_length);

          if (action == SCRIPT_ACTION_SIGNFILE)
          {
            memcpy(crypt.ChipId, sha_digest, sizeof(crypt.ChipId));
            memcpy(crypt.FlsSignature, sha_digest, sizeof(crypt.FlsSignature));
          }
          else if (action == SCRIPT_ACTION_SIGNSECPACK)
          {
             SecPack sec_pack;
             file->sec_pack.get_content( buffer );
             sec_pack.load(buffer);

             if (hash_func == 1)
             {
               memcpy((uint32_t*)sec_pack.get_data_ptr("FileHash"),
                      sha_digest, sec_pack.size("FileHash")*sizeof(uint32_t));
               memset((uint32_t*)sec_pack.get_data_ptr("FileHashSha2"),
                      0, sec_pack.size("FileHashSha2"));
             }
             else
             {
               memset((uint32_t*)sec_pack.get_data_ptr("FileHash"),
                      0, sec_pack.size("FileHash")*sizeof(uint32_t));
               memcpy((uint32_t*)sec_pack.get_data_ptr("FileHashSha2"),
                      sha_digest, sec_pack.size("FileHashSha2"));
             }
             file->sec_pack.set_content( sec_pack.buffer() );
          }

          // Getting SecPack signature
          Sha.Reset(hash_func);
          // Note: Skipping the 512 byte "SecureBlock
          file->sec_pack.get_content(buffer, 0, 512);

          Sha.Input(&buffer[0], buffer.size());

          if (!Sha.Result(sha_digest))
            throw_exception("Interpreter Error> GenSign could not calculate SHA digest");

          fix_endian(sha_type, sha_digest, hash_length);
          debug << "  SecPackSign:" << get_hex_matrix(sha_digest, hash_length);

          if (action == SCRIPT_ACTION_SIGNFILE)
          {
            memcpy(crypt.DataSignature, sha_digest, sizeof(crypt.DataSignature));
            memcpy(crypt.FlsSignature2, sha_digest, sizeof(crypt.FlsSignature2));

            signature = (uint8_t*)&crypt;
            signature_length = sizeof(crypt);
          }
          else if (action == SCRIPT_ACTION_SIGNSECPACK)
          {
            uint8_t* DER;
            size_t len = Sha.GetDER(&DER);
            memcpy((uint32_t*)&raw_signature[0], DER, len);
            memcpy((uint32_t*)&raw_signature[len], sha_digest, hash_length*sizeof(uint32_t));

            signature = raw_signature;
            signature_length = len + hash_length*sizeof(uint32_t);
          }

          // PKCS Packing & Encryption
          vector<uint8_t> pkcs_data;
          uint32_t pkcs_length = 0;

          PrivateKeyStructType *key = (PrivateKeyStructType *)
            FILE_getContent(key_string, buffer, sizeof(PrivateKeyStructType));

          // debug << " KeyLength:  " << key->Key.KeyLength << endl;
          // debug << " PkcsLength: " << ((key->Key.KeyLength + 7) / 8) << endl;
          pkcs_data.assign(512, 0);

          if (PKCS_Encrypt(&pkcs_data[0], &pkcs_length,
                           signature, signature_length,
                           key, SECURE_BLOCK_TYPE, 0, no_rsa)) //< UseSalt==0 (Disabled for good)
          {
            throw_exception("Interpreter Error> PKCS_Encrypt failed");
          }

          debug << "  Encrypted data:" << get_hex_matrix(&pkcs_data[0], pkcs_length);

          //< Save the PKCS in the write region (Default: SecureBlock0)
          if (key->KeyType == DEV_KEY)
            write_addr += 256;       //< Write in SecureBlock1

          info << " > SecPackRegion: " << file->sec_pack.name()
               << " (Offset: " << getHexString(write_addr) << ")" << endl;

          file->sec_pack.write((char*)&pkcs_data[0], pkcs_length, write_addr);
          uint32_t hash = crypto::get_hash(file->sec_pack.parent(), HASH_XOR16);
          file->sec_pack.hash[HASH_XOR16] = getHexString(hash, 4);
        } /* download_file loop */
        break;
      }

      default:
        throw_exception("Interpreter Error> Action '"+ Stringify((int)action) +"' not recognized");
    }
    action = SCRIPT_ACTION_NONE;
  }
  script.close();
}



typedef struct {
  const char* name;
  size_t offset;
  size_t length;
  const char* magic;
} magic_signature_t;

magic_signature_t signatures[] = {
    { "Fls3", 257, 5, "ustar"},           // tar
    { "Fls3", 257, 8, "fls 3\x0010"},     // Fls3 1.0
    { "Fls2", 0, 4, "\xE8\x03\x00\x00" }, // 0x000003E8
    { "FFC", 0, 4, "\x91\x64\x1D\xED" },  // 0xED1D6491
    { "XML", 0, 5, "<?xml" },
    { "HEX", 0, 7, ":020000" },
    { "HEX", 0, 7, ":040000" },
    { "ZIP", 0, 4, "PK\x01\x02" },
    { "ZIP", 0, 4, "PK\x03\x04" },
    { "ZIP", 0, 4, "PK\x05\x06" },
    { "ZIP", 0, 4, "PK\x07\x08" },
    { NULL, 0, 0, ""}
};

#define MAGIC_TYPE_HEX        "HEX"
#define MAGIC_TYPE_TEXT       "Text"
#define MAGIC_TYPE_BINARY     "Binary"
#define MAGIC_TYPE_FLS2NBIN   "Fls2NandBin"

/**
 * Detect the file type via a list of Magic Signatures
 * If no type is detected, the first 32 bytes are looked through
 * in order to determine if the file is binary or text.
 *
 * @param path   Path to the file
 *
 * @return       A text string representing the meta type detected or
 *               NULL on Android if file is not readable. On other systems
 *               an exception will be thrown.
 */
const char* detect_file_type(string path)
{
  char meta[32];
  bool type_found = false;
  const char* type = MAGIC_TYPE_TEXT;
  memset(meta, 0, sizeof(meta));

  ifstream detector(path.c_str(), ios::binary);
  if (!detector)
    throw_exception("Could not open "+ path, NULL);

  size_t file_size = fsize(path);
  for (int i=0; signatures[i].name; i++)
  {
    if ((signatures[i].offset + signatures[i].length) > file_size
        || signatures[i].length == 0)
      continue;

    detector.seekg(signatures[i].offset);
    detector.read(meta, signatures[i].length);
    /*
    printf("Signature: ");
    for (int s=0; s<signatures[i].length; ++s)
      printf("%02X ", meta[s]);
    printf("\n");
    */
    if (memcmp(meta, signatures[i].magic, signatures[i].length) == 0)
    {
      type_found = true;
      type = signatures[i].name;
    }
  }

  if (!type_found)
  {
    uint32_t ebl_offset;
    // Now the file may be an uploaded binary file from a NAND system
    // if so there is a offset to an appended EBL.FLS file at EOF -8

    detector.seekg(file_size-8);
    detector.read(meta, 8);
    ebl_offset = *((uint32_t*)meta);
    if (ebl_offset < file_size-8)
    {
      detector.seekg(ebl_offset);
      detector.read(meta, signatures[2].length);
      if (memcmp(meta, signatures[2].magic, signatures[2].length) == 0)
      {
        type = MAGIC_TYPE_FLS2NBIN;
        type_found = true;
      }
    }
  }

  // Determine if file is text or binary (via first 32 bytes)
  if (!type_found)
  {
    // If non-ASCII is found, assume binary
    detector.seekg(ios::beg);
    detector.read(meta, 32);

    for (uint8_t i=0; i<32 && meta[i]; ++i)
    {
      if (!isalnum(meta[i]) && !isblank(meta[i]) && !ispunct(meta[i])
          && meta[i] != '\r' && meta[i] != '\n')
      {
        type = MAGIC_TYPE_BINARY;
        type_found = true;
      }
    }

    // Guess HEX if starting with ':' & only containing HEX chars
    if (!type_found && meta[0] == ':')
    {
      type = MAGIC_TYPE_HEX;
      for (uint8_t i=1; i<32 && meta[i] && meta[i] != '\r' && meta[i] != '\n'; ++i)
      {
        if (!isxdigit(meta[i]))
          type = MAGIC_TYPE_TEXT;
      }
    }
  }

  detector.close();
  return type;
}
