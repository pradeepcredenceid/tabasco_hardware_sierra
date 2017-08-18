/* -------------------------------------------------------------------------
 Copyright (C) 2013-2014 Intel Mobile Communications GmbH

      Sec Class: Intel Confidential (IC)
 ----------------------------------------------------------------------
 Revision Information:
    $File name:  /msw_tools/FlashTool/libFlsTool/src/Fls3MetaParser.cpp $
    $CC-Version: .../dhkristx_140314_v0/1 $
    $Date:       2014-04-22    9:13:32 UTC $
 ----------------------------------------------------------------------
 by CCCT (0.12h)
 ---------------------------------------------------------------------- */

#include "Fls3MetaParser.h"

#include "JSON.h"
#include "Crypto.h"
#include "StringExt.h"

using namespace ipa::crypto;

namespace ipa
{
namespace fls
{

/**
 *  Helper functions for parsing the JSON values
 **/
static string SerializeJson(std::string path, json_value* value, int index = -1);
static int JsonIndexCount(string path);
static size_t JsonGetIndex(string path);
static uint64_t JsonParse_uint64(json_value*& json, string error_string);
static string JsonParse_string(json_value*& json, string error_string);
static StorageTech JsonParse_technology(json_value*& json);
static MemoryClass JsonParse_memclass(json_value*& json);
static bool JsonParse_memory_area(MemoryArea* area, string& name, json_value*& json);
static bool JsonParse_partition(NandPartition* partition, string& name, json_value*& json);
static bool JsonParse_embedded_file(EmbeddedFile* file, string& name, json_value*& json);

/**
 *  The Fls3 Meta data Parser Class
 *
 * @param ref        The reference to save to
 * @param buffer     The JSON buffer in RAW text
 * @param verbosity  Verbosity during parsing [0-2]
 **/
Fls3MetaParser::Fls3MetaParser(FlsMetaFile& ref, vector<uint8_t>& buffer, int verbosity)
  : m_ref(ref)
{
  set_verbose(verbosity);

  if (buffer.size() > 0 && buffer[0] == '{')
    load(buffer);
}

/**
 * Set Verbosity
 */
void Fls3MetaParser::set_verbose(int verbose)
{
  info.setStream((verbose > 0) ? &cout : 0);
  debug.setStream((verbose > 1) ? &cout : 0);
}

/**
 * Parses an Fls3/TAR meta data file (meta.json).
 *
 * The meta data structure is generated &
 * Embedded files are linked with their MemoryRegions.
 *
 * @param buffer   The JSON data in raw text
 */
void Fls3MetaParser::load(vector<uint8_t>& buffer)
{
  json_value* json = NULL;
  int errorLine = 0;
  char *errorPos = NULL;
  char *errorDesc = NULL;

  if (buffer.size() == 0)
    throw_exception("No meta data provided!");

  //< Make sure buffer ends with a NULL
  buffer.push_back('\0');

  // set_verbose(verbosity);
  json = json_parse((char*)&buffer[0], &errorPos, &errorDesc, &errorLine);
  if (!json)
  {
    throw_exception("JSON parser error at line "+ Stringify(errorLine) +": "
                    + string(errorDesc) +"\n'"+ string(errorPos) +"'\n\n");
  }

  // Run the recursive parser
  parse(json->first_child);

  json_destroy(json);
}


/**
 * The Fls3 JSON Recursive Parser
 *
 * This function calls itself for each non-branch value until the
 * entire JSON tree have been parsed.
 *
 * @param value   First time the first_child of the JSON root
 * @param path    This is the current path down to the JSON root
 * @param index   Current index in an array/object. Default -1.
 **/
void Fls3MetaParser::parse(json_value *value, string path, size_t index)
{
  int i=0;
  string name;

  if (value->name)
    name = str_to_lower( value->name );

  // Switch the JSON element type
  switch(value->type)
  {
  case JSON_ARRAY:
  case JSON_OBJECT:

    if (value->parent->type == JSON_ARRAY || name.empty())
    {
      stringstream ss;
      ss << "[" << index << "]";
      path += ss.str();
    }
    else
    {
      if (value->parent->parent)
        path += ".";
      path += name;
    }

    for (json_value *it = value->first_child; it; it = it->next_sibling)
    {
      parse(it, path, i++);
    }
    break;

  default:
    // Parsing the serialised value
    if (value->parent->type == JSON_ARRAY)
    {
      debug << SerializeJson(path, value, index) << endl;
      parse_value(path, value, index);
    }
    else
    {
      debug << SerializeJson(path, value) << endl;
      parse_value(path, value);
    }
    break;
  }
}


/**
 * Serialise a JSON path.
 *
 *  { "boo" => [ "foo" => 1, "var" => "blabla" ] }
 *
 *  boo.foo = 1
 *  boo.var = "blabla"
 *
 * @param path    The serialized path to the value in question.
 * @param value   The JSON value pointer
 * @param index   If the parent is an array, this represents its index herein
 */
static string SerializeJson(std::string path, json_value* value, int index)
{
  stringstream ss;
  ss << path;

  if (value->name)
    ss << " (" << value->name << ")";
  if (index >= 0)
  {
    if (!value->name && value->parent->name)
      ss << " (" << value->parent->name << ")";
    ss << "[" << index << "]";
  }

  switch(value->type)
  {
  case JSON_NULL:    ss << " = NULL";                                         break;
  case JSON_OBJECT:  ss << " = <OBJECT>";                                     break;
  case JSON_ARRAY:   ss << " = <ARRAY>";                                      break;
  case JSON_STRING:  ss << " = \"" << value->string_value << '"';             break;
  case JSON_INT:     ss << " = "  << dec << value->int_value;                 break;
  case JSON_FLOAT:   ss << " = "  << value->float_value;                      break;
  case JSON_BOOL:    ss << " = "  << (value->int_value ? "true\n" : "false"); break;
  }

  return ss.str();
}

/**
 * Counts the number of arrays in a JSON string (starting after "intel_fls").
 *
 * Ex. intel_fls.memory_map[1].image[0] => 2
 *
 **/
int JsonIndexCount(string path)
{
  int count = 0;
  size_t tmp = 9;
  size_t pos = tmp;

  while ((tmp = path.substr(pos).find_first_of('[')) != string::npos)
  {
    pos += tmp+1;
    count++;
  }

  return count;
}

/**
 * Extract the index of the last array in a JSON string.
 * KlocWorks wants we test the returned index value, so we
 * limit the index amount to 1000.
 *
 * Ex. intel_fls3.memory_map[10] => 10
 *
 **/
size_t JsonGetIndex(string path)
{
  size_t i = 0;
  size_t pos1 = path.find_first_of('[');
  size_t pos2 = path.find_first_of(']');

  if (pos1 != string::npos && pos2 != string::npos)
  {
    string number = path.substr(pos1+1, pos2-pos1-1);
    i = strtol( number.c_str(), NULL, 10 );
    if (errno == ERANGE || i > 1000)
      throw_exception("Integer out of range", i);
  }

  return i;
}


/**
 *  Verifies & returns the JSON uint64_t value
 **/
uint64_t JsonParse_uint64(json_value*& json, string error_string)
{
  uint64_t value;

  // Validating JSON value
  if (json->type < JSON_STRING
     || (json->type == JSON_STRING && (json->string_value[0] != '0' || json->string_value[1] != 'x')))
  {
    throw_exception("Value for '"+ string(error_string) +"' of invalid type!", 0);
  }

  // Parsing the value
  switch(json->type)
  {
    case JSON_STRING: value = strtoul(json->string_value, NULL, 0);       break;
    case JSON_INT:    value = json->int_value;                            break;
    default:
      throw_exception("Variable"+ string(error_string) +" is not right type", 0);
  }

  return value;
}

/**
 *  Verifies & returns the JSON string value
 **/
string JsonParse_string(json_value*& json, string error_string)
{
  if (json->type != JSON_STRING)
    throw_exception(error_string+" is not a string!", "");

  return string(json->string_value);
}

/**
 *  Verifies & parses the Storage Technology
 **/
StorageTech JsonParse_technology(json_value*& json)
{
  string tech = JsonParse_string(json, "Storage Technology");

  switch (tech[1])
  {
    case 'O': case 'o': return TECHNOLOGY_NOR;        break;
    case 'A': case 'a': return TECHNOLOGY_NAND;       break;
    case 'L': case 'l': return TECHNOLOGY_FLASHLESS;  break;
    default:
      throw_exception("Values supported for variable "+ string(json->name)
                     +": [ NOR, NAND, FLASHLESS ]", TECHNOLOGY_NOR);
  }
  // Assuming NOR
  return TECHNOLOGY_NOR;
}

/**
 *  Verifies & parses the Memory Class Type
 *
 *  Returns the memclass or MEMORY_CLASS_NOT_USED if not found or empty string.
 **/
MemoryClass JsonParse_memclass(json_value*& json)
{
  string memclass = JsonParse_string(json, "Memory Class Type");
  return get_memory_class(memclass);
}

bool JsonParse_memory_area(MemoryArea* area, string& name, json_value*& json)
{
  if (name.compare("addr") == 0)
    area->address = JsonParse_uint64(json,"MemoryMap Address");

  else if (name.compare("length") == 0)
    area->size = JsonParse_uint64(json,"MemoryMap Length");

  else if (name.compare("class") == 0)
    area->type = JsonParse_memclass(json);

  else if (name.compare("tag") == 0)
    area->tag = JsonParse_string(json, "MemoryMap Tag");

  else if (name.compare("options") == 0)
      area->flags += JsonParse_string(json, "MemoryMap Option");

  else
    return false;

  return true;
}


bool JsonParse_partition(NandPartition* partition, string& name, json_value*& json)
{
  if (name.compare("id") == 0)
    partition->id = JsonParse_uint64(json,"NAND Partition Address");

  else if (name.compare("addr") == 0)
    partition->address = JsonParse_uint64(json,"NAND Partition Address");

  else if (name.compare("start_block") == 0)
    partition->start_block = JsonParse_uint64(json,"NAND Partition StartBlock");

  else if (name.compare("end_block") == 0)
    partition->end_block = JsonParse_uint64(json,"NAND Partition EndBlock");

  else if (name.compare("max_block_in_sub_par") == 0)
    partition->max_block_in_sub_par = JsonParse_uint64(json,"NAND Partition MaxBlockInSubPar");

  else if (name.compare("start_res_block") == 0)
    partition->start_reservoir = JsonParse_uint64(json,"NAND Partition StartResBlock");

  else if (name.compare("end_res_block") == 0)
    partition->end_reservoir = JsonParse_uint64(json,"NAND Partition EndResBlock");

  else if (name.compare("image_type") == 0)
    partition->type = JsonParse_uint64(json,"NAND Partition ImageType");

  else if (name.compare("options") == 0)
    partition->options = JsonParse_uint64(json,"NAND Partition Options");

  else if (name.compare("reserved") == 0)
    partition->attributes = JsonParse_uint64(json,"NAND Partition Reserved");

  else
    return false;

  return true;
}


bool JsonParse_embedded_file(EmbeddedFile* file, string& name, json_value*& json)
{
  if (name.compare("name") == 0)
    file->rename( JsonParse_string(json,"EmbeddedFile Name") );

  else if (name.compare("type") == 0)
    file->type = JsonParse_memclass(json);

  else if (name.compare("size") == 0)
    file->resize( JsonParse_uint64(json, "EmbeddedFile Size") );

  else if (name.compare("sha1") == 0)
    file->hash[HASH_SHA1] = JsonParse_string(json, "EmbeddedFile SHA1");

  else if (name.compare("xor16") == 0)
    file->hash[HASH_XOR16] = JsonParse_string(json, "EmbeddedFile XOR16");

  else if (name.compare("crc8") == 0)
    file->hash[HASH_CRC8] = JsonParse_string(json, "EmbeddedFile CRC8");

  else if (name.compare("crc16") == 0)
    file->hash[HASH_CRC16] = JsonParse_string(json, "EmbeddedFile CRC16");

  else if (name.compare("sha256") == 0)
    file->hash[HASH_SHA256] = JsonParse_string(json, "EmbeddedFile SHA256");

  else
    return false;

  return true;
}

/**
 *  Parses the values of the JSON file
 *
 *  TODO: Remove all string::compare functions from this function
 *         in order to make it more readable.
 **/
void Fls3MetaParser::parse_value(std::string path, json_value* value, size_t index)
{
  std::string name = (value->name) ? value->name : "";
  if (name.empty())
    name = (value->parent->name) ? value->parent->name : "";

  try
  {
    // Intel_Fls3
    if (path.compare("intel_fls") == 0)
    {
      if (name.compare("fls_version") == 0)
        m_ref.fls_version = JsonParse_string(value, "Fls Version");

      else if (name.compare("tool_version") == 0)
        m_ref.tool_version = JsonParse_string(value, "Tool Version");

      else if (name.compare("created") == 0)
        m_ref.date_created = JsonParse_string(value, "Created Date");

      else if (name.compare("last_modified") == 0)
        m_ref.last_modified = JsonParse_string(value, "Modified Date");

      else
        throw_exception("Variable"+ name +" is not recognized in path "+ path);
    }

    // Platform
    else if (path.compare("intel_fls.platform") == 0)
    {
      if (name.compare("chip_name") == 0)
        m_ref.platform.chip_name = JsonParse_string(value, "Chip Name");

      else if (name.compare("chip_version") == 0)
        m_ref.platform.version = JsonParse_string(value, "Chip Version");

      else if (name.compare("technology") == 0)
        m_ref.platform.technology = JsonParse_technology(value);

      else if (name.compare("storage_addr") == 0)
        m_ref.platform.storage_address = JsonParse_uint64(value, "Storage Address");

      else if (name.compare("storage_size") == 0)
        m_ref.platform.storage_size = JsonParse_uint64(value, "Storage Length");

      else if (name.compare("usif_boot_speed") == 0)
        m_ref.platform.usif_boot_speed = JsonParse_uint64(value, "USIF Bootup Speed");

      else
        throw_exception("Variable"+ name +" is not recognized in path "+ path);
    }

    // Memory Map
    else if (path.compare(0, 20, "intel_fls.memory_map") == 0)
    {
      index = JsonGetIndex(path);

      if (index >= m_ref.memory_map.size())
        m_ref.memory_map.push_back(new MemoryArea);

      if (!JsonParse_memory_area(m_ref.memory_map[index], name, value))
        throw_exception("Variable '"+ name +"' is not recognized in path "+ path);
    }

    // NAND Partitions
    else if (path.compare(0, 20, "intel_fls.partitions") == 0)
    {
      index = JsonGetIndex(path);

      if (index >= m_ref.partitions.size())
        m_ref.partitions.push_back(new NandPartition);

      if (!JsonParse_partition(m_ref.partitions[index], name, value))
        throw_exception("Variable '"+ name +"' is not recognized in path "+ path);
    }

    // Meta Files
    else if (path.compare(0, 20, "intel_fls.meta_files") == 0)
    {
      index = JsonGetIndex(path);

      while (index >= m_ref.meta_files.size())
        m_ref.meta_files.push_back(new EmbeddedFile(m_ref.m_fls_file));

      EmbeddedFile* meta_file = m_ref.meta_files[index];
      if (!JsonParse_embedded_file(meta_file, name, value))
        throw_exception("Variable '"+ name +"' is not recognized in path "+ path);
    }

    // Boot Files
    else if (path.compare(0, 20, "intel_fls.boot_files") == 0)
    {
      index = JsonGetIndex(path);

      while (index >= m_ref.boot_files.size())
        m_ref.boot_files.push_back(new EmbeddedFile(m_ref.m_fls_file));

      EmbeddedFile* boot_file = m_ref.boot_files[index];
      if (!JsonParse_embedded_file(boot_file, name, value))
        throw_exception("Variable '"+ name +"' is not recognized in path "+ path);
    }

    /**
     * Download Files
     **/
    else if (path.compare(0, 24, "intel_fls.download_files") == 0)
    {
      DownloadFile* file;
      index = JsonGetIndex(path);

      // Create index if not existing
      if (index >= m_ref.download_files.size())
        m_ref.download_files.push_back(new DownloadFile(m_ref.m_fls_file));

      file = m_ref.download_files[index];

      /**
       *  Parsing Orig File + SecPack
       **/
      if (JsonIndexCount(path) == 1)
      {
        if (name.compare("orig_file_name") == 0)
          file->name = JsonParse_string(value,"DownloadFile Original Name");

        else if (name.compare("orig_file_type") == 0)
          file->type = JsonParse_memclass(value);

        else if (name.compare("orig_file_timestamp") == 0)
          file->timestamp = JsonParse_string(value, "DownloadFile Original Timestamp");

        else if (name.compare("sec_pack") == 0)
          file->sec_pack.rename( JsonParse_string(value, "DownloadFile SecPack Name") );

        else if (name.compare("sha1") == 0)
          file->sec_pack.hash[HASH_SHA1] = JsonParse_string(value, "DownloadFile SHA1");

        else if (name.compare("xor16") == 0)
          file->sec_pack.hash[HASH_XOR16] = JsonParse_string(value, "DownloadFile Seq Pack XOR16");

        else if (name.compare("crc16") == 0)
          file->sec_pack.hash[HASH_CRC16] = JsonParse_string(value, "DownloadFile Seq Pack CRC16");

        else if (name.compare("sha256") == 0)
          file->sec_pack.hash[HASH_SHA256] = JsonParse_string(value, "DownloadFile Seq Pack SHA256");

        else
          throw_exception("Variable '"+ name +"' is not recognized in path "+ path);
      }
      else
      {
        /**
         *  Parsing MemoryRegion
         **/
        MemoryRegion* region;
        size_t image_index;

        if (path.compare(28, 6, "images") != 0 && path.compare(28, 6, "region") != 0)
          throw_exception("Variable '"+ name +"' is not recognized in path "+ path);

        image_index = JsonGetIndex(path.substr(28));

        // Create index if not existing
        if (image_index >= file->load_map.size())
          file->load_map.push_back(new MemoryRegion(m_ref.m_fls_file));

        region = file->load_map[image_index];

        if (name.compare("name") == 0)
          region->rename( JsonParse_string(value,"DownloadFile MemoryMap Name") );

        else if (name.compare("addr") == 0)
          region->area = m_ref.getMemoryAreaByAddr(value->string_value, file->type.id);

        else if (name.compare("total_length") == 0)
          ;

        else if (name.compare("used_length") == 0)
          region->resize( JsonParse_uint64(value, "DownloadFile MemoryMap UsedLength") );

        else if (name.compare("sha1") == 0)
          region->hash[HASH_SHA1] = JsonParse_string(value, "MetaFile SHA1");

        else if (name.compare("xor16") == 0)
          region->hash[HASH_XOR16] = JsonParse_string(value, "MetaFile XOR16");

        else if (name.compare("crc16") == 0)
          region->hash[HASH_CRC16] = JsonParse_string(value, "MetaFile CRC16");

        else if (name.compare("sha256") == 0)
          region->hash[HASH_SHA256] = JsonParse_string(value, "MetaFile SHA256");

        else if (name.compare("flags") == 0)
        {
          region->flags += JsonParse_string(value, "MemoryMap Option");

          // TODO: Note: This is a Hack because there are two MemoryArea's for FLASHLESS
          if (region->flags.back() == FLASHLESS)
          {
            region->area = m_ref.getMemoryAreaByAddr(region->area->address,
                                                     get_memory_class("Binary").id);
            if (region->area)
              debug << "   Re-Linking region " << region->name()
                    << " to area (" << region->area->tag << ")" << endl;
          }
        }
        else
          throw_exception("Variable '"+ name +"' is not recognized in path "+ path);
      }
    }
    // Parser error
    else
    {
      throw_exception("Variable '"+ name +"' is not recognized in path "+ path);
    }
  }
  catch (const exception& e)
  {
    #ifndef __EXCEPTIONS
      myexception& e = chatched_exceptions.front();
    #endif
    cerr << endl << " Variable Type: " << (int)value->type << endl;
    if (value->name)          cerr << " Variable Name: " << value->name << endl;
    if (value->parent->name)  cerr << " Parent Name:   " << value->parent->name << endl;
    if (value->string_value)  cerr << " Variable Str:  " << value->string_value << endl;

    cerr << endl << "   JSON Parser Error> " << e.what() << endl << endl;
    exit(1);
  }
}


  } /* namespace fls */
} /* namespace ipa */
