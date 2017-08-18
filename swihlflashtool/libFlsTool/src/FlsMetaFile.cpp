/*  -------------------------------------------------------------------------
    Copyright (C) 2013-2014 Intel Mobile Communications GmbH

         Sec Class: Intel Confidential (IC)
    ----------------------------------------------------------------------
    Revision Information:
       $File name:  /msw_tools/FlashTool/libFlsTool/src/FlsMetaFile.cpp $
       $CC-Version: .../dhkristx_140314_v0/3 $
       $Date:       2014-04-30    10:42:30 UTC $
    ----------------------------------------------------------------------
    Description:
       The generic FlsMetaFile format.
    ---------------------------------------------------------------------- */

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <memory>

#include "OS.h"        //< GetTimestamp()
#include "FlsFile.h"

#include "FlsMetaFile.h"
#include "Exceptions.h"
#include "StringExt.h"

using namespace std;
using namespace ipa::crypto;

namespace ipa
{
  namespace fls
  {
  //---------------------------------------------------------------------------
  // Chip Name vs. index
  //---------------------------------------------------------------------------
  static const char* ChipName[] =
  {
    "E_GOLD_CHIPSET_V2",
    "E_GOLD_LITE_CHIPSET",
    "S_GOLD_CHIPSET_V1",
    "S_GOLD_LITE_CHIPSET",
    "E_GOLD_CHIPSET_V3",
    "S_GOLD_CHIPSET_V2",
    "S_GOLD_CHIPSET_V3",
    "E_GOLD_RADIO_V1",
    "E_GOLD_VOICE_V1",
    "S_GOLD_RADIO_V1",
    "X_GOLD_618",
    "X_GOLD_102",
    "X_GOLD_213",
    "X_GOLD_110",
    "X_GOLD_626",
    "X_GOLD_223",
    "X_GOLD_215",
    "X_GOLD_706",
    "X_GOLD_636",
    "X_GOLD_225",
    "X_GOLD_716",
    "X_GOLD_631",
    "X_GOLD_221",
    "X_GOLD_726",
    "X_GOLD_632",
    NULL
  };

  int32_t getChipNameId(string chip_name)
  {
    for (size_t i=0; ChipName[i]; ++i)
    {
      if (chip_name.compare(ChipName[i]) == 0)
        return (int32_t)i;
    }
    return -1;
  }

  const char* getChipName(int index)
  {
    if (ChipName[index])
      return ChipName[index];
    return "Unknown";
  }

  //---------------------------------------------------------------------------
  // Memory Class
  //---------------------------------------------------------------------------

  /* TODO: This could also be generated with a simple INI file */
  /* This could be the default weight if none was specified in the INI */
  MemoryClass memclasses [] =
  {
    { MEMORY_CLASS_PSI,           "PSI",         "BOOT_CORE_PSI",    1 },
    { MEMORY_CLASS_SLB,           "SLB",         "BOOT_CORE_SLB",    2 },
    { MEMORY_CLASS_EBL,           "EBL",         "BOOT_CORE_EBL",    3 },
    { MEMORY_CLASS_CODE,          "Code",        "CODE",             4 },
    { MEMORY_CLASS_CUST,          "Cust",        "CUST",            10 },
    { MEMORY_CLASS_DYN_EEP,       "Dynamic EEP", "DYN_EEP",          5 },
    { MEMORY_CLASS_STAT_EEP,      "Static EEP",  "STAT_EEP",         6 },
    { MEMORY_CLASS_SWAP_BLOCK,    "Swap Block",  "SWAP_BLOCK",      11 },
    { MEMORY_CLASS_SEC_BLOCK,     "SEC Block",   "SEC_BLOCK",       12 },
    { MEMORY_CLASS_EXCEP_LOG,     "Excep Log",   "EXCEP_LOG",       13 },
    { MEMORY_CLASS_STAT_FFS,      "Static FFS",  "STAT_FFS",         9 },
    { MEMORY_CLASS_DYN_FFS,       "Dynamic FFS", "DYN_FFS",          8 },
    { MEMORY_CLASS_DSP_SW,        "DSP",         "DSP",              7 },
    { MEMORY_CLASS_ROOT_DISK,     "Root Disk",   "ROOT_DISK",       14 },
    { MEMORY_CLASS_CUST_DISK,     "Cust Disk",   "CUST_DISK",       15 },
    { MEMORY_CLASS_USER_DISK,     "User Disk",   "USER_DISK",     0xFA },
    { MEMORY_CLASS_HEX_EXTRACT,   "HEX Extract", "HEX",           0xFB },
    { MEMORY_CLASS_BINARY,        "Binary",      "BIN",              3 },
    { MEMORY_CLASS_TEXT,          "Text",        "TEXT",          0xFC },
    { MEMORY_CLASS_NOT_USED,      "Not Used",    "",              0xFF }
  };

  MemoryClass& get_memory_class(MemoryClassId memclass)
  {
    if (memclass > MEMORY_CLASS_NOT_USED-1)
      return memclasses[MEMORY_CLASS_NOT_USED-1];

    if (memclasses[memclass].id == memclass)
      return memclasses[memclass];

    for (size_t i=0; memclasses[i].id != MEMORY_CLASS_NOT_USED; ++i)
    {
      if (memclasses[i].id == memclass)
        return memclasses[i];
    }
    return memclasses[MEMORY_CLASS_NOT_USED-1];
  }

  /**
   *  Returns the memclass or MEMORY_CLASS_NOT_USED if not found or empty string.
   **/
  MemoryClass& get_memory_class(string memclass)
  {
    if (memclass.empty())
      return memclasses[MEMORY_CLASS_NOT_USED-1];

    for (size_t i=0; memclasses[i].id != MEMORY_CLASS_NOT_USED; ++i)
    {
      if (StringCaseCompare(memclass, memclasses[i].name) or
          StringCaseCompare(memclass, memclasses[i].name2))
        return memclasses[i];
    }

    return memclasses[MEMORY_CLASS_NOT_USED-1];
  }

/**
 * Special function to replace the UID number in file names
 */
bool replace_uid(std::string& str, size_t uid)
{
  stringstream to;
  to << "_ID" << uid << "_";
  string from = "_ID?_";

  if (uid >= 10)
    throw_exception("UID ["+ Stringify(uid) +"] too large!", false);

  for(size_t i=0; i<10; ++i)
  {
    from[3] = '0'+i;
    if (replace(str, from, to.str()))
      return true;
  }
  return false;
}

/**
 * Constructor
 */
FlsMetaFile::FlsMetaFile(int verbose)
  : prg_loaded(false),
    json_file(NULL),
    partition_marker(PARTITION_MAGIC_NAND) /* 0x00001234 ONLY for eMMC */
{
  set_verbose(verbose);
}

/**
 * Destructor
 */
FlsMetaFile::~FlsMetaFile()
{
  unload();
}

/**
 * Copy Constructor
 */
FlsMetaFile::FlsMetaFile(const FlsMetaFile& copy)
{
  prg_loaded = copy.prg_loaded;
  m_fls_file = copy.m_fls_file;
  json_file = copy.json_file;
  m_verbosity = copy.m_verbosity;
  info = copy.info;
  debug = copy.debug;
  version = copy.version;
  fls_version = copy.fls_version;
  tool_version = copy.tool_version;
  date_created = copy.date_created;
  last_modified = copy.last_modified;
  platform = copy.platform;

  size_t i;
  for (i=0; i < copy.partitions.size(); i++)
    partitions.push_back(new NandPartition(*copy.partitions[i]));

  for (i=0; i < copy.memory_map.size(); i++)
    memory_map.push_back(new MemoryArea(*copy.memory_map[i]));

  for (i=0; i < copy.meta_files.size(); i++)
    cloneMetaFile( copy.meta_files[i] );

  for (i=0; i < copy.boot_files.size(); i++)
    cloneBootFile( copy.boot_files[i] );

  for (i=0; i < copy.download_files.size(); i++)
    cloneDownloadFile( copy.download_files[i] );
}

/**
 * Copy Constructor
 */
FlsMetaFile& FlsMetaFile::operator=(const FlsMetaFile& copy)
{
  if(this != &copy)
  {
    prg_loaded = copy.prg_loaded;
    m_fls_file = copy.m_fls_file;
    json_file = copy.json_file;
    m_verbosity = copy.m_verbosity;
    info = copy.info;
    debug = copy.debug;
    version = copy.version;
    fls_version = copy.fls_version;
    tool_version = copy.tool_version;
    date_created = copy.date_created;
    last_modified = copy.last_modified;
    platform = copy.platform;

    size_t i;
    for (i=0; i < copy.partitions.size(); i++)
      partitions.push_back(new NandPartition(*copy.partitions[i]));

    for (i=0; i < copy.memory_map.size(); i++)
      memory_map.push_back(new MemoryArea(*copy.memory_map[i]));

    for (i=0; i < copy.meta_files.size(); i++)
      cloneMetaFile( copy.meta_files[i] );

    for (i=0; i < copy.boot_files.size(); i++)
      cloneBootFile( copy.boot_files[i] );

    for (i=0; i < copy.download_files.size(); i++)
      cloneDownloadFile( copy.download_files[i] );
  }
  return *this;
}


/**
 * Clone an MemoryRegion element & add its file to the embedded files list
 *
 * @param copy   Pointer to element to copy
 *
 * @return  The new element
 */
MemoryRegion* FlsMetaFile::cloneMemoryRegion(MemoryRegion* copy)
{
  MemoryRegion* region = new MemoryRegion(*copy);
  if (copy->area)
    region->area = getMemoryAreaByAddr(copy->area->address, copy->area->type.id);
  return region;
}

/**
 * Clone a MetaFile & add to internal lists
 *
 * @param copy   Pointer to DownloadFile element to copy
 *
 * @return  The new EmbeddedFile element
 */
EmbeddedFile* FlsMetaFile::cloneMetaFile(EmbeddedFile* copy)
{
  EmbeddedFile* new_file = new EmbeddedFile(*copy);
  meta_files.push_back( new_file );
  return new_file;
}

/**
 * Clone a BootFile & add to internal lists
 *
 * @param copy   Pointer to DownloadFile element to copy
 *
 * @return  The new EmbeddedFile element
 */
EmbeddedFile* FlsMetaFile::cloneBootFile(EmbeddedFile* copy)
{
  EmbeddedFile* new_file = new EmbeddedFile(*copy);
  boot_files.push_back( new_file );
  return new_file;
}

/**
 * Clone a DownloadFile
 *
 * All MemoryRegions + the SecPack is copied.
 * File references will poit to the original file location.
 *
 * If tag is set the download regions are linked to the MemoryMap
 * elements with this TAG. Otherwise we lookup using the original addresses.
 *
 * @param dl_copy   Pointer to DownloadFile element to copy
 * @param tag       TAG to lookup in the MemoryMap
 *
 * @return  The new DownloadFile element
*/
DownloadFile*  FlsMetaFile::cloneDownloadFile(DownloadFile* copy, string tag)
{
  size_t uid = download_files.size();
  DownloadFile* new_file = new DownloadFile(*copy);

  string name = new_file->sec_pack.name();
  if (replace_uid(name, uid))
    new_file->sec_pack.rename( name );

  for (size_t j=0; j < copy->load_map.size(); j++)
  {
    MemoryArea* copy_area = copy->load_map[j]->area;
    MemoryRegion* new_region = new_file->load_map[j];

    // Clone the embedded file
    string name = new_region->name();
    if (replace_uid(name, uid))
      new_region->rename( name );

    // Re-Link area if TAG is specified
    if (!tag.empty())
      new_region->area = getMemoryAreaByTag(tag, new_file->type.id).at(0);
    else if (copy_area)
      new_region->area = getMemoryAreaByAddr(copy_area->address, copy_area->type.id);
    else if (copy->load_map[j]->size() == 0)
      continue;

    // Make sure we actually found an area
    if (!new_region->area)
    {
      delete new_file;
      throw_exception("Copy contains no MemoryArea for "+ copy->load_map[j]->name(), NULL);
    }
  }

  download_files.push_back(new_file);
  return new_file;
}

/**
 * Set Verbosity
 */
void FlsMetaFile::set_verbose(int verbose)
{
  m_verbosity = verbose;
  info.setStream((verbose > 0) ? &cout : 0);
  debug.setStream((verbose > 1) ? &cout : 0);
}

void FlsMetaFile::unload()
{
  for (size_t i=0; i < memory_map.size(); ++i)
    delete memory_map[i];

  memory_map.clear();
}

bool FlsMetaFile::is_fls2()
{
  return (fls_version[0] == '2');
}

bool FlsMetaFile::is_fls3()
{
  return (fls_version[0] == '3');
}

bool FlsMetaFile::is_bootable(MemoryArea* area)
{
  return (area->type.id == MEMORY_CLASS_EBL
       || (area->type.id == MEMORY_CLASS_PSI
           && area->tag.find("FLASH") == string::npos));
}

void FlsMetaFile::extract(std::string path)
{
  cout << "FlsMetaFile::extract(" << path << ")" << endl;
}

/**
 *  Saving to JSON format
 **/
void FlsMetaFile::save(string path, bool overwrite)
{
  if (fexists(path))
  {
    if (overwrite && remove(path.c_str()))
      throw_exception("Error deleting "+ path);
  }

  ofstream file(path.c_str(), ios_base::binary);

  if (!file.is_open())
    throw_exception("Could not open file "+ string(path));

  debug << " [FlsMetaFile] Saving JSON to " << path << endl;
  file << *this; // dump(file);

  file.close();
}

/**
 *  Dumps the JSON output to any output stream (default cout)
 **/
std::ostream& operator <<(std::ostream& os, FlsMetaFile* file)
{
  return file->dump(os);
}

std::ostream& operator <<(std::ostream& os, FlsMetaFile& file)
{
  return file.dump(os);
}

/**
 * Compare if two Fls files have the same MemoryMap
 *
 * @param file  The file to compare with
 *
 * @return True if the two MemoryMaps are equal
 */
bool FlsMetaFile::compare_memory_map(FlsMetaFile& file)
{
  if (memory_map.size() != file.memory_map.size())
    throw_exception("MemoryMap size mismatch between "
                    + m_fls_file +" & "+ file.m_fls_file, false);

  for (size_t i=0; i < memory_map.size(); ++i)
  {
    if (memory_map[i]->tag != file.memory_map[i]->tag)
    {
      debug << " Area[" << i << "] TAG mismatch: "
            << memory_map[i]->tag << " != " << file.memory_map[i]->tag << endl;
      return false;
    }

    if (memory_map[i]->type.id != file.memory_map[i]->type.id)
    {
      debug << " Area[" << i << "] Type mismatch on " << memory_map[i]->tag
            << (int)memory_map[i]->type.id << " != " << (int)file.memory_map[i]->type.id << endl;
      return false;
    }

    if (memory_map[i]->size != file.memory_map[i]->size)
    {
      debug << " Area[" << i << "] Size mismatch on " << memory_map[i]->tag
            << getHexString(memory_map[i]->size)
            << " != " << getHexString(file.memory_map[i]->size) << endl;
      return false;
    }

    if (memory_map[i]->address != file.memory_map[i]->address)
    {
      debug << " Area[" << i << "] Address mismatch on " << memory_map[i]->tag
            << getHexString(memory_map[i]->address)
            << " != " << getHexString(file.memory_map[i]->address) << endl;
      return false;
    }

    uint64_t flags1 = memory_map[i]->flags.sum();
    uint64_t flags2 = file.memory_map[i]->flags.sum();

    if (flags1 != flags2)
    {
      debug << " Area[" << i << "] Flag mismatch on " << memory_map[i]->tag
            << getHexString(flags1) << " != " << getHexString(flags2) << endl;
      return false;
    }
  }
  return true;
}

/**
 * Return a list of the JSON, Meta, Boot, & Download files
 */
vector<FlsEmbedFile*> FlsMetaFile::get_file_list()
{
  vector<FlsEmbedFile*> list;

  if (json_file)
    list.push_back(json_file);

  list.insert(list.end(), meta_files.begin(), meta_files.end());
  list.insert(list.end(), boot_files.begin(), boot_files.end());

  for (size_t i=0; i < download_files.size(); ++i)
  {
    vector<MemoryRegion*>& load_map = download_files[i]->load_map;
    list.push_back(&download_files[i]->sec_pack);

    // TODO: If Save() saves the empty FLASHLESS files, we can add this:
    //       list.insert(list.end(), load_map.begin(), load_map.end());
    for (size_t j=0; j < load_map.size(); ++j)
      if (load_map[j]->size() > 0)
        list.push_back(load_map[j]);
  }

  return list;
}

/**
 * Locate an embedded file via its name.
 *
 * Note: This function will pack a list of all files each time called.
 *
 * @param filename The file to look for
 *
 * @return  Either NULL or the file found
 */
FlsEmbedFile* FlsMetaFile::find(string filename)
{
  vector<FlsEmbedFile*> files = get_file_list();

  for (size_t i=0; i < files.size(); ++i)
  {
    if (filename.compare( files[i]->name() ) == 0)
      return files[i];
  }

  info << "FlsMetaFile::find(" << filename << ") lookup failed!" << endl;
  return NULL;
}

#if 0
/**
 * Locate a meta file via its extension
 *
 * @param ext   The extension to look for
 *
 * @return  Either NULL or the file found
 */
EmbedFileList FlsMetaFile::findFileByExt(string ext)
{
  EmbedFileList files;

  for (size_t i=0; i < meta_files.size(); ++i)
  {
    EmbeddedFile* file = meta_files[i];
    string file_ext = fext(file->name());
    if (file_ext.compare( ext ) == 0)
      files.push_back(file);
  }

  return files;
}
#endif

/**
 * Get a meta file section
 *
 * @param name    Name of file
 * @param create  Create if none existing (default ON)
 *
 * @return  Either the boot file pointer or a newly created one
 */
EmbeddedFile* FlsMetaFile::getMetaFile(string filename, bool create)
{
  EmbeddedFile* file = NULL;

  for (size_t i=0; i<meta_files.size(); ++i)
  {
    file = meta_files[i];
    if (filename.compare( file->name() ) == 0)
      return file;
  }

  if (create)
  {
    file =  new EmbeddedFile(m_fls_file, filename);
    meta_files.push_back(file);
    return file;
  }

  return NULL;
}

/**
 * Get a meta file section
 *
 * @param type    Type to look for: Usually PSI, SLB or EBL
 *
 * @return  Either the boot file pointer or a newly created one
 */
EmbeddedFile* FlsMetaFile::getMetaFileByType(MemoryClassId type)
{
  EmbeddedFile* file = NULL;

  for (size_t i=0; i<meta_files.size(); ++i)
  {
    file = meta_files[i];
    if (file->type.id == type)
      return file;
  }

  return NULL;
}

/**
 * Get a meta file section
 *
 * @param type    Type to look for: Usually PSI, SLB or Binary
 * @param create  Create if none existing (default ON)
 *
 * @return  Either the meta file pointer or a newly created one
 */
EmbeddedFile* FlsMetaFile::getMetaFile(MemoryClassId type, bool create)
{
  EmbeddedFile* file = NULL;

  for (size_t i=0; i < meta_files.size(); ++i)
  {
    file = meta_files[i];
    if (file->type.id == type)
      return file;
  }

  if (create)
  {
    file = new EmbeddedFile(m_fls_file);
    file->type = get_memory_class(type);
    meta_files.push_back(file);
    return file;
  }

  return NULL;
}
/**
 * Get a boot file section
 *
 * @param type    Type to look for: Usually PSI, SLB or EBL
 * @param create  Create if none existing (default ON)
 *
 * @return  Either the boot file pointer or a newly created one
 */
EmbeddedFile* FlsMetaFile::getBootFile(MemoryClassId type, bool create)
{
  EmbeddedFile* file = NULL;

  for (size_t i=0; i<boot_files.size(); ++i)
  {
    file = boot_files[i];
    if (file->type.id == type)
      return file;
  }

  if (create)
  {
    file =  new EmbeddedFile(m_fls_file);
    file->type = get_memory_class(type);
    boot_files.push_back(file);
    return file;
  }

  return NULL;
}

/**
 * Get a download file section
 * This function searches for both the name of the file and for the SecPack name
 *
 * @param name    File name to look for
 * @param type    Type of the file to look for
 * @param create  Create if none existing (default ON)
 *
 * @return  Either the file pointer or a newly created one
 */
DownloadFile* FlsMetaFile::getDownloadFile(string name, MemoryClass type, bool create)
{
  DownloadFile* file = NULL;

  for (size_t i=0; i<download_files.size(); ++i)
  {
    file = download_files[i];
    if (file->name.compare( name ) == 0
        or file->sec_pack.name().compare( name ) == 0)
    {
      if (type.id == MEMORY_CLASS_BINARY
          or file->type.id == type.id)
      {
        return file;
      }
    }
  }

  if (create)
  {
    file = new DownloadFile(m_fls_file);
    file->name = name;
    file->type = type;
    download_files.push_back(file);
    return file;
  }

  return NULL;
}


/**
 * Find a DownloadFile via its Address field and its Type field
 *
 * @param address The address to search for
 * @param type    Type to look for
 *
 * @return        The DownloadFile with this region or NULL
 */
DownloadFile* FlsMetaFile::getDownloadFileByAddress(uint64_t address, MemoryClassId type)
{
  for (size_t i=0; i<download_files.size(); ++i)
  {
    DownloadFile* file = download_files[i];
    for (size_t j=0; j<download_files[i]->load_map.size(); ++j)
    {
    	if (file->load_map[j]->area->address == address && file->type.id == type)
    	{
    		return file;
    	}
    }
  }
  return NULL;
}

/**
 * Find the DownloadFile associated with a MemoryRegion
 *
 * @param region  The region to search for
 *
 * @return        The DownloadFile with this region or NULL
 */
DownloadFile* FlsMetaFile::getDownloadFile(MemoryRegion* region)
{
  for (size_t i=0; i<download_files.size(); ++i)
  {
    DownloadFile* file = download_files[i];
    for (size_t j=0; j < file->load_map.size(); ++j)
    {
      if (region == file->load_map[j])
        return file;
    }
  }
  return NULL;
}

/**
 * Find a memory area via its Type
 *
 * @param tag   The tag to search for
 *
 * @return      A vector of memory areas
 */
vector<MemoryArea*> FlsMetaFile::getMemoryArea(MemoryClassId id)
{
  vector<MemoryArea*> area;

  for (size_t i=0; i < memory_map.size(); ++i)
  {
    if (memory_map[i]->type.id == id)
      area.push_back(memory_map[i]);
  }

  return area;
  // throw_exception("Could not find '"+ get_memory_class(id).name +"' in MemoryMap", NULL);
}

/**
 * Find a memory area via its Tag
 *
 * @param tag   The tag to search for
 *
 * @return      A vector of memory areas
 */
vector<MemoryArea*> FlsMetaFile::getMemoryAreaByTag(string tag, MemoryClassId memclass)
{
  vector<MemoryArea*> area;

  if (tag.empty())
    return area;

  for (size_t i=0; i < memory_map.size(); ++i)
  {
    string& map_tag = memory_map[i]->tag;
    size_t tag_size = map_tag.size();
    size_t colon = map_tag.find(':');
    size_t hash  = map_tag.find('#');
    if (colon != string::npos || hash != string::npos)
      tag_size = std::min(colon,hash);

    if (map_tag.compare(0, tag_size, tag) == 0)
    {
      if (memclass == MEMORY_CLASS_BINARY
          or memory_map[i]->type.id == memclass)
        area.push_back(memory_map[i]);
    }
  }

  return area;
  // throw_exception("Could not find '"+ tag +"' in MemoryMap", NULL);
}

/**
 * Find a memory area via its Address field
 *
 * @param address            The MemoryArea address to search for
 * @param memclass           Set to a specific memory class ID or MEMORY_CLASS_BINARY
 * @param fail_if_not_found  Throws an exception if set & MemoryArea not found
 *
 * @return        A pointer to the memory area or NULL if not found
 */
MemoryArea* FlsMetaFile::getMemoryAreaByAddr(
                    uint64_t address, 
                    MemoryClassId memclass,
                    bool fail_if_not_found)
{
  for (size_t i=0; i < memory_map.size(); ++i)
  {
    if (memory_map[i]->address == address)
    {
      if (memclass == MEMORY_CLASS_BINARY
          or memory_map[i]->type.id == memclass)
        return memory_map[i];
    }
  }

  if (fail_if_not_found)
  {
    cout << "Looking up address " << getHexString(address) << " (" << int(memclass) << ")" << endl;
    for (size_t i=0; i < memory_map.size(); ++i)
    {
      cout << "   != " << getHexString(memory_map[i]->address)
         << ", Size: " << getHexString(memory_map[i]->size)
         << ", MemClass: " << int(memory_map[i]->type.id)
         << ", " << memory_map[i]->type.name
         << endl;
    }

    throw_exception("Could not find '"+ getHexString(address) +"' in MemoryMap (memoryClass "+ getHexString(int(memclass)) +")", NULL);
  }
  return NULL;
}

/**
 * Find a memory area via its Address field
 *
 * @param address The address to search for in string format e.g. "0x00000000"
 *
 * @return        A pointer to the memory area or NULL if not found
 */
MemoryArea* FlsMetaFile::getMemoryAreaByAddr(string address, MemoryClassId memclass)
{
  return getMemoryAreaByAddr(strtoul(address.c_str(), NULL, 0), memclass);
}

/**
 * Locate a memory region of an address range
 *
 * @param address   Start address of the range
 * @param length    Length of the range
 *
 * @return          Region if found and NULL if not
 */
MemoryRegion* FlsMetaFile::getMemoryRegion(uint64_t address, uint64_t length)
{
  for (size_t i=0; i < download_files.size(); ++i)
  {
    DownloadFile* file = download_files[i];
    for (size_t j=0; j < file->load_map.size(); ++j)
    {
      MemoryRegion* region = file->load_map[j];
      if (address >= region->area->address
//          and (length==0 or length <= region->size))
          and (length==0 or length <= region->area->size))
      {
        return region;
      }
    }
  }

  throw_exception("Could not find a LoadMap within "
                  + getHexString(address) +" - "+ getHexString(length), NULL);
}

/**
 * Dump the meta data to a buffer
 *
 * @param buffer Output buffer
 *
 * @return Reference to the buffer
 */
vector<uint8_t>& FlsMetaFile::get_meta_data(vector<uint8_t>& buffer)
{
  stringstream ss;

  dump(ss);
  string data = ss.str();

  buffer.assign(data.begin(), data.end());
  return buffer;
}



/**
 * Dump the meta data to any file stream (defaults to cout)
 *
 * @param json   Output stream to dump to
 *
 * @return       The json input is also returned
 */
ostream& FlsMetaFile::dump(ostream& json)
{
  size_t i,j;
  map<crypto::Algorithm,string>::iterator algo;

  if (date_created.empty())
    date_created = GetTimestamp(0);

  json << "{ " << endl
       << "  \"intel_fls\": {" << endl
       << "    \"fls_version\": \""   << fls_version << "\"," << endl
       << "    \"tool_version\": \""  << tool_version << "\"," << endl
       << "    \"created\": \""       << date_created << "\"," << endl
       << "    \"last_modified\": \"" << GetTimestamp(0) << "\"," << endl;

  if (!platform.version.empty())
  {
    string tech = "Unknown";
    switch(platform.technology) {
      case TECHNOLOGY_NOR:       tech = "NOR";       break;
      case TECHNOLOGY_NAND:      tech = "NAND";      break;
      case TECHNOLOGY_FLASHLESS: tech = "FLASHLESS"; break;
    }

    json << "    \"platform\": {" << endl
         << "      \"chip_name\": \""       << platform.chip_name << "\"," << endl
         << "      \"chip_version\": \""    << platform.version << "\"," << endl
         << "      \"technology\": \""      << tech << "\"," << endl
         << "      \"storage_addr\": \""    << getHexString(platform.storage_address) << "\"," << endl
         << "      \"storage_size\": \""    << getHexString(platform.storage_size) << "\"," << endl
         << "      \"usif_boot_speed\": "   << dec << platform.usif_boot_speed << "," << endl
         << "    }," << endl;
  }

  if (memory_map.size())
  {
    json << "    \"memory_map\": [" << endl;
    for (i=0; i<memory_map.size(); ++i)
    {
      MemoryArea* area = memory_map[i];
      json << "      {" << endl
           << "        \"addr\": \""   << getHexString(area->address) << "\"," << endl
           << "        \"length\": \"" << getHexString(area->size) << "\"," << endl
           << "        \"class\": \""  << area->type.name << "\"," << endl
           << "        \"tag\": \""    << area->tag << "\"," << endl
           << "        \"options\": [ " << area->flags.to_string() << " ]," << endl
           << "      }," << endl;
    }
    json << "    ]," << endl;
  }

  if (partitions.size())
  {
    json << "    \"partitions\": [" << endl;
    for (i=0; i<partitions.size(); ++i)
    {
      NandPartition* part = partitions[i];
      json << "      {" << endl
           << "        \"id\": \""                   << getHexString(part->id, 2) << "\"," << endl
           << "        \"addr\": \""                 << getHexString(part->address) << "\"," << endl
           << "        \"start_block\": \""          << getHexString(part->start_block) << "\"," << endl
           << "        \"end_block\": \""            << getHexString(part->end_block) << "\"," << endl
           << "        \"max_block_in_sub_par\": \"" << getHexString(part->max_block_in_sub_par) << "\"," << endl
           << "        \"start_res_block\": \""      << getHexString(part->start_reservoir) << "\"," << endl
           << "        \"end_res_block\": \""        << getHexString(part->end_reservoir) << "\"," << endl
           << "        \"image_type\": \""           << getHexString(part->type) << "\"," << endl
           << "        \"options\": \""              << getHexString(part->options) << "\"," << endl
           << "        \"reserved\": \""             << getHexString(part->attributes) << "\"," << endl
           << "      }," << endl;
    }
    json << "    ]," << endl;
  }

  if (meta_files.size())
  {
    json << "    \"meta_files\": [" << endl;
    for (i=0; i<meta_files.size(); ++i)
    {
      EmbeddedFile* file = meta_files[i];
      json << "      {" << endl
           << "        \"name\": \""  << file->name() << "\"," << endl
           << "        \"type\": \""  << file->type.name << "\"," << endl;
           //<< "        \"size\": \""  << getHexString(file->size) << "\"," << endl;

      if (file->hash.size())
      {
        json << "        \"hash\": {" << endl;
        for(algo = file->hash.begin(); algo != file->hash.end(); ++algo)
          json << "          \"" << crypto::to_string(algo->first)
                                 << "\": \"" << algo->second << "\"," << endl;
        json << "        }" << endl;
      }

      json << "      }," << endl;
    }
    json << "    ]," << endl;
  }

  if (boot_files.size())
  {
    json << "    \"boot_files\": [" << endl;
    for (i=0; i<boot_files.size(); ++i)
    {
      EmbeddedFile* file = boot_files[i];
      json << "      {" << endl
           << "        \"name\": \""  << file->name() << "\"," << endl
           << "        \"type\": \""  << file->type.name << "\"," << endl
           << "        \"size\": \""  << getHexString(file->size()) << "\"," << endl;

      if (file->hash.size())
      {
        json << "        \"hash\": {" << endl;
        for(algo = file->hash.begin(); algo != file->hash.end(); ++algo)
          json << "          \"" << crypto::to_string(algo->first)
                                 << "\": \"" << algo->second << "\"," << endl;
        json << "        }" << endl;
      }

      json << "      }," << endl;
    }
    json << "    ]," << endl;
  }

  if (download_files.size())
  {
    json << "    \"download_files\": [" << endl;
    for (i=0; i<download_files.size(); ++i)
    {
      DownloadFile* file = download_files[i];
      json << "      {" << endl
           << "        \"orig_file_name\": \""  << file->name << "\"," << endl
           << "        \"orig_file_type\": \""  << file->type.name << "\"," << endl
           << "        \"orig_file_timestamp\": \""  << file->timestamp << "\"," << endl
           << "        \"sec_pack\": \""  << file->sec_pack.name() << "\"," << endl;

      if (file->sec_pack.hash.size())
      {
        json << "        \"sec_pack_hash\": {" << endl;
        for(algo = file->sec_pack.hash.begin(); algo != file->sec_pack.hash.end(); ++algo)
          json << "          \"" << crypto::to_string(algo->first)
                                 << "\": \"" << algo->second << "\"," << endl;
        json << "        }," << endl;
      }
#if 0
      if (file->hash.size())
      {
        json << "        \"hash\": {" << endl;
        for(algo = file->hash.begin(); algo != file->hash.end(); ++algo)
          json << "          \"" << crypto::to_string(algo->first)
                                 << "\": \"" << algo->second << "\"," << endl;
        json << "        }," << endl;
      }
#endif
      if (file->load_map.size() > 0)
      {
        json << "        \"region\": [" << endl;

        for (j=0; j<file->load_map.size(); ++j)
        {
          uint64_t area_address = 0;
          uint64_t area_size = 0;

          MemoryRegion* region = file->load_map[j];
          if (region->area)
          {
            area_address = region->area->address;
            area_size = region->area->size;
          }

          json << "          {" << endl
               << "            \"name\": \""  << region->name() << "\"," << endl
               << "            \"addr\": \""  << getHexString(area_address) << "\"," << endl
               << "            \"used_length\": \""  << getHexString(region->size()) << "\"," << endl
               << "            \"total_length\": \""  << getHexString(area_size) << "\"," << endl
               << "            \"flags\": [ " << region->flags.to_string() << " ]," << endl;

          if (region->hash.size())
          {
            json << "            \"hash\": {" << endl;
            for(algo = region->hash.begin(); algo != region->hash.end(); ++algo)
              json << "              \"" << crypto::to_string(algo->first)
                                         << "\": \"" << algo->second << "\"," << endl;
            json << "            }" << endl;
          }

          json << "          }," << endl;
        }
        json << "        ]," << endl;
      }
      json << "      }," << endl;
    }
    json << "    ]" << endl;
  }
  json << "  }" << endl;
  json << "}" << endl;

  return json;
}



/**
 * DownloadFile declarations
 */
DownloadFile::DownloadFile(DownloadFile& copy)
  : sec_pack(copy.sec_pack)
{
  name = copy.name;
  type = copy.type;
  timestamp = copy.timestamp;

  for (size_t i=0; i < copy.load_map.size(); i++)
    load_map.push_back(new MemoryRegion(*copy.load_map[i]));
}

DownloadFile& DownloadFile::operator=(const DownloadFile& copy)
{
  if(this != &copy)
  {
    name = copy.name;
    type = copy.type;
    timestamp = copy.timestamp;
    sec_pack = copy.sec_pack;

    for (size_t i=0; i < copy.load_map.size(); i++)
      load_map.push_back(new MemoryRegion(*copy.load_map[i]));
  }
  return *this;
}

DownloadFile::DownloadFile(string filename, uint64_t offset, uint64_t size)
  : sec_pack(filename, offset, size) // Note: SecPack Size is "almost" constant
{
  name = fname(filename);

  // Note: Setting size=0 will force generation of a new sec_pack
  if (size == 0)
    sec_pack.resize(0);
}

size_t DownloadFile::size()
{
  size_t size = 0; // sec_pack.size();
  for (size_t i=0; i < load_map.size(); ++i)
    size += load_map[i]->size();
  return size;
}

void DownloadFile::dump()
{
  cout << " DownloadFile " << name << ":" << endl;
  cout << "    SecPack: " << sec_pack.name()
       << " (" << sec_pack.parent()
       << ", " << sec_pack.size() << ")" << endl;
  for (size_t a=0; a < load_map.size(); a++)
  {
    MemoryRegion* region = load_map[a];
    cout << "    LoadMap" << a << ": " << region->name()
         << " (" << region->parent()
         << ", " << getHexString(region->size()) << ")" << endl;
  }
  cout << endl;
}

/**
 * MemoryRegion declarations
 */
MemoryRegion::MemoryRegion(MemoryRegion const &copy)
  : FlsEmbedFile(copy)
{
  area = NULL;
  // area = getMemoryAreaByAddr(copy.area->address, copy.area->type.id);
  flags = copy.flags;
  hash = copy.hash;
}

MemoryRegion& MemoryRegion::operator=(MemoryRegion const &copy)
{
  if(this != &copy)
  {
    area = NULL;
    flags = copy.flags;
    hash = copy.hash;
  }
  return *this;
}

void MemoryRegion::set_content(string path)
{
  if (!area)
    throw_exception("MemoryRegion is missing an area!");

  hash.clear();
  FlsEmbedFile::set_content(path);

  uint16_t Xor16 = get_hash(HASH_XOR16);
  hash[HASH_XOR16] = getHexString(Xor16, 4);
}

/**
 * EmbeddedFile declarations
 */
EmbeddedFile::EmbeddedFile(EmbeddedFile const &copy)
  : FlsEmbedFile(copy)
{
  type = copy.type;
  hash = copy.hash;
}

EmbeddedFile& EmbeddedFile::operator=(const EmbeddedFile& copy)
{
  if(this != &copy)
  {
    FlsEmbedFile::operator=(copy);
    type = copy.type;
    hash = copy.hash;
  }
  return *this;
}

string EmbeddedFile::fls_name()
{
  string name = FlsEmbedFile::name();
  if (name.empty())
    throw_exception("EmbeddedFile has no name (Parent: "+ parent() +")!", "");

  size_t pos = StringToLower(name).find(".fls");
  if (pos == string::npos)
    pos = StringToLower(name).find(".prg");

  if (pos != string::npos)
    return name.substr(0, pos+4);

  throw_exception("No FLS in filename "+ name, "");
  return string(""); // TODO: Return "name" or nothing?
}

void EmbeddedFile::set_content(string path)
{
  FlsEmbedFile::set_content(path);

  uint16_t Xor16 = get_hash(HASH_XOR16);
  hash[HASH_XOR16] = getHexString(Xor16, 4);
}

void EmbeddedFile::set_content(vector<uint8_t> &buffer, uint64_t size)
{
  FlsEmbedFile::set_content(buffer, size);

  uint16_t Xor16 = get_hash(HASH_XOR16);
  hash[HASH_XOR16] = getHexString(Xor16, 4);
}



/**
 * MemoryOptions declarations
 */
static MemoryOptionNames& get_memory_options()
{
  static MemoryOptionNames options;

  if (options.empty())
  {
    options["BOOT_CORE_HASH_MARKER"] = 1;
    options["ALLOW_SEC_BOOT_UPDATE"] = 2;
    options["DONT_TOUCH_REGION"]     = 4;
    options["SKIP_VERSION_CHECK"]    = 8;
    options["ERASE_TOTAL_LENGTH"]    = 16;
    options["ERASE_ONLY"]            = 32;
    options["COMPRESSED_IMAGE"]      = 64;
    options["COMPRESSED_TRANSFER"]   = 128;
    options["USE_DRIVER_FROM_FILE"]  = 256;
    options["NAND_SKIP_PT_CHECK"]    = 512;
    options["IMAGE_HAS_PUBLIC_KEYS"] = 1024;
    options["IMAGE_IS_BOOT_IMAGE"]   = 2048;

    options["LOAD_TO_RAM"]           = 0x00010000;
    options["LOAD_TO_SD"]            = 0x00020000;
    options["NO_LOAD_TO_RAM"]        = 0x00040000;

    options["EXCLUDE_UTA_FLASH"]     = 0x00100000;
    options["EXCLUDE_VALIDATION"]    = 0x00200000;
    options["EXCLUDE_RAM_LOADING"]   = 0x00400000;
    options["EXCLUDE_PAGING"]        = 0x00800000;

    options["ERASE_TOTAL"]           = 0x20000000;
    options["FLASHLESS"]             = 0x40000000;
    options["ERASE_REGION"]          = 0x80000000;

    options["ENO_OPTION"]            = options["LOAD_TO_RAM"];
    options["SD_CARD_OPTION"]        = options["LOAD_TO_SD"];
    options["NVM_REGION"]            = options["NO_LOAD_TO_RAM"];
   }
   return options;
}

/**
 *  Get a memory option number from a string
 *
 * @param name   Name of the option
 * @return       Number of the option
 */
static uint64_t get_memory_option(string& name)
{
  MemoryOptionNames& options = get_memory_options();
  MemoryOptionNames::const_iterator it = options.find(name);
  if (it == options.end())
    throw_exception("Could not find option "+ string(name), 0);
   return it->second;
}

/**
 *  Get a memory option string from a number
 *
 * @param key    Number of the option
 * @return       Name of the option (string)
 */
static string get_memory_option(uint64_t key)
{
  MemoryOptionNames& options = get_memory_options();
  MemoryOptionNames::const_iterator it;
  for (it = options.begin(); it != options.end(); ++it)
    if (it->second == key)
      return it->first;

  // throw_exception("Could not find option "+ getHexString(key), "");
  return getHexString(key);
}

uint64_t MemoryOptions::get_uint64(string& name)
{
  return get_memory_option(name);
}

string MemoryOptions::get_string(uint64_t key)
{
  return get_memory_option(key);
}

int MemoryOptions::has(uint64_t flag)
{
  for (size_t i=0; i < list.size(); ++i)
    if (list[i] == flag)
      return (int)i;
   return -1;
}

bool MemoryOptions::remove(uint64_t flag)
{
  int i = has(flag);
  if (i<0)
    return false;
//    throw_exception("Flag "+ get_memory_option(flag) +" does not exist in MemoryArea "
//                    + getHexString(address));
  list.erase( list.begin()+i );
  return true;
}

bool MemoryOptions::operator-=(uint64_t flag)
{
  return remove(flag);
}

bool MemoryOptions::add(uint64_t flag)
{
  if (has(flag) > 0)
    return false;
//    throw_exception("Flag "+ get_memory_option(flag) +" already exists in MemoryArea "
//                   + getHexString(address));
  list.push_back(flag);
  return true;
}

bool MemoryOptions::operator+=(uint64_t flag)
{
  return add(flag);
}

bool MemoryOptions::add(string flag)
{
  uint64_t i = get_memory_option(flag);
  if (has(i) > 0)
    return false;
//    throw_exception("Flag "+ get_memory_option(flag) +" already exists in MemoryArea "
//                   + getHexString(address));
  list.push_back(i);
  return true;
}

bool MemoryOptions::operator+=(string flag)
{
  return add(flag);
}

/**
 *  Get all memory options in a single uint64
 *
 * @return       uint64 with the flags OR'd together
 */
uint64_t MemoryOptions::sum()
{
  uint64_t sum = 0;
  for (size_t i=0; i < list.size(); ++i)
    sum |= list[i];
  return sum;
}

/**
 *  Get all memory options in a single string (for the JSON)
 *
 * @return  Space seperated string of all the flags
 */
string MemoryOptions::to_string(uint32_t i)
{
  stringstream ss;
  if (!list.empty())
  {
    if ((int)i == -1)
    {
      for (i=0; i < (uint32_t)list.size(); ++i)
        ss << " \"" << get_memory_option( list[i] ) << "\"";

      return ss.str().substr(1);
    }
    else
    {
      ss << get_memory_option( list[i] );
    }
  }
  return ss.str();
}

/**
 *  Extracts the memory options from a number & appends to the list
 *
 * @param flags  Number with OR'd flags
 *
 * @return True if all flags where parsed and False if not
 */
bool MemoryOptions::set(uint64_t flags)
{
  MemoryOptionNames& options = get_memory_options();
  MemoryOptionNames::const_iterator it;
  for (it = options.begin(); it != options.end(); ++it)
  {
    if (flags & it->second)
    {
      flags -= it->second;
      add( it->second );
    }
  }

  if (flags)
  {
    cout << "Warning> Could not parse all flags (Missing: "
         << getHexString(flags) << ")!" << endl;

    add( flags );
    return false;
  }
  return true;
}


/**
 *  STL container like handling
 */
void MemoryOptions::clear()
{
  list.clear();
}

size_t MemoryOptions::size()
{
  return list.size();
}

uint64_t MemoryOptions::operator[](size_t i)
{
  return list[i];
}

uint64_t MemoryOptions::front()
{
  return list.at(0);
}

uint64_t MemoryOptions::back()
{
  return list.at( list.size()-1 );
}

  } /* namspace fls */
} /* namspace ipa */
