/* ---------------------------------------------------------------------------
  Copyright (C) 2013-2014 Intel Mobile Communications GmbH

       Sec Class: Intel Confidential (IC)

  All rights reserved.
  ----------------------------------------------------------------------------
  Description:
    TLV parser - For abstracting endianness on buffer inputs.
                 Later versions will parse type definition input files.
  ----------------------------------------------------------------------------
  Revision Information:
  $File name:  /msw_tools/FlashTool/libFlsTool/src/StructParser.cpp $
  $CC-Version: .../dhkristx_140314_v0/1 $
  $Date:       2014-04-16    12:30:30 UTC $
  ------------------------------------------------------------------------- */

#include <stddef.h>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "StructParser.h"

#include "FileIO.h"
#include "StringExt.h"
#include "Exceptions.h"

using namespace std;
using namespace ipa;

namespace ipa
{
  static const char* type_name[] = {
    "child",
    "uint8",
    "int8",
    "uint16",
    "int16",
    "uint32",
    "int32",
    "uint64",
    "int64",
    NULL
  };

  static size_t type_size[] = { 0, 1, 1, 2, 2, 4, 4, 8, 8 };
}

/**
 * Returns the StructParserType of a type name.
 * If not match is found it will assume a child type
 *
 * @param name   The name of the type variable
 *
 * @return       The StructParserType matching the type name
 */
StructParser::StructType StructParser::get_type_value(string name)
{
  for (size_t val=0; type_name[val]; ++val)
  {
    if (name.compare(type_name[val]) == 0)
      return (StructType)val;
  }
  return SP_TYPE_CHILD;
}

namespace ipa
{
  /**
   * Reads a 16 bit unsigned little endian integer from an 8 bit buffer.
   *
   * @param buf Buffer where value is read from.
   *
   * @return    Returns read value.
   */
  static uint16_t get_uint16_le(const vector<uint8_t>::iterator iter)
  {
    uint16_t value;

    value  = *iter;
    value |= *(iter+1) << 8;

    return value;
  }

  /**
   * Writes a 16 bit unsigned integer to an 8 bit buffer using little endian
   * encoding.
   *
   * @param buf   Buffer where value is written to.
   * @param value Value to write.
   */
  static void set_uint16_le(vector<uint8_t>::iterator iter, uint64_t value)
  {
    *iter++ = (uint8_t) value;
    *iter++ = (uint8_t) (value >> 8);
  }

  /**
   * Reads a 32 bit unsigned little endian integer from an 8 bit buffer.
   *
   * @param buf Buffer where value is read from.
   *
   * @return    Returns read value.
   */
  static uint32_t get_uint32_le(vector<uint8_t>::iterator iter)
  {
    uint32_t value;

    value  = *iter;
    value |= *(iter+1) << 8;
    value |= *(iter+2) << 16;
    value |= *(iter+3) << 24;

    return value;
  }

  /**
   * Writes a 32 bit unsigned integer to an 8 bit buffer using little endian
   * encoding.
   *
   * @param buf   Buffer where value is written to.
   * @param value Value to write.
   */
  static void set_uint32_le(vector<uint8_t>::iterator iter, uint64_t value)
  {
    *iter++ = (uint8_t) value;
    *iter++ = (uint8_t) (value >> 8);
    *iter++ = (uint8_t) (value >> 16);
    *iter++ = (uint8_t) (value >> 24);
  }

  /**
   * Reads a 64 bit unsigned little endian integer from an 8 bit buffer.
   *
   * @param buf Buffer where value is read from.
   *
   * @return    Returns read value.
   */
  static uint64_t get_uint64_le(const vector<uint8_t>::iterator iter)
  {
    uint64_t value;

    value  = (uint64_t)*iter;
    value |= (uint64_t)*(iter+1) << 8;
    value |= (uint64_t)*(iter+2) << 16;
    value |= (uint64_t)*(iter+3) << 24;
    value |= (uint64_t)*(iter+4) << 32;
    value |= (uint64_t)*(iter+5) << 40;
    value |= (uint64_t)*(iter+6) << 48;
    value |= (uint64_t)*(iter+7) << 56;

    return value;
  }

  /**
   * Writes a 64 bit unsigned integer to an 8 bit buffer using little endian
   * encoding.
   *
   * @param buf   Buffer where value is written to.
   * @param value Value to write.
   */
  static void set_uint64_le(vector<uint8_t>::iterator iter, uint64_t value)
  {
    *iter++ = (uint8_t) value;
    *iter++ = (uint8_t) (value >> 8);
    *iter++ = (uint8_t) (value >> 16);
    *iter++ = (uint8_t) (value >> 24);
    *iter++ = (uint8_t) (value >> 32);
    *iter++ = (uint8_t) (value >> 40);
    *iter++ = (uint8_t) (value >> 48);
    *iter++ = (uint8_t) (value >> 56);
  }

} /* Namespace IPA */


/**
 * Throws exception if an entry is not of expected type.
 *
 * @param entry Entry to check type on.
 * @param type  Expected type.
 */
void StructParser::validate_type(StructEntry *entry, StructType type)
{
  if (entry->type != type)
  {
    throw_exception("Entry with name "+ entry->name +" is not a "+ string(type_name[type]));
  }
}

/**
 * Throws exception if an index is not valid for a given entry.
 *
 * @param entry Entry to check index on.
 * @param index Index to check.
 */
void StructParser::validate_index(StructEntry *entry, uint32_t index)
{
  if (index >= entry->size)
  {
    throw_exception("Index "+ Stringify(index) +" out of bounds for entry '"+ entry->name +"'");
  }
}

/**
 * Throws exception if an entry does not have an expected number of values.
 *
 * @param entry       Entry to check number of values on.
 * @param size  Expected number of values.
 */
void StructParser::validate_size(StructEntry *entry, uint32_t size)
{
  if (entry->size < size)
  {
    throw_exception("Array size does not match for entry '"+ entry->name +"': "
                   + Stringify(entry->size) +" < "+ Stringify(size));
  }
}

/**
 * Prints the content of the buffer, while interpreting the data as the struct_name
 * Struct_name should have been loaded via the load() function.
 *
 * @param struct_name   Name of the structure to use for interpretation.
 * @param indent        Indentation level. 2*indent spaces will be prepended to each
 *                      printed line.
 */
void StructParser::print(string struct_name, uint32_t indent)
{
  cout << " Map '" << struct_name << "' (" << m_buffer.size() << ")" << endl;

  const StructEntryList& list = get_structure(struct_name);

  for (StructEntryList::const_iterator it = list.begin();
       it != list.end(); it++)
  {
    print_struct(*it, indent);
  }
}

/**
 * Dumps the entire structure
 */
void StructParser::dump()
{
  if (m_struct.empty())
    throw_exception("No structure have been defined yet...");

  print(m_struct);
}


/**
 * Dumps the currently loaded StructureMap to output
 */
void StructParser::dump_map()
{
  for(DefinitionMap::iterator list = m_struct_map.begin();
      list != m_struct_map.end(); list++)
  {
    cout << endl << " Structure: '" << list->first << "':" << endl;
    for (StructEntryList::iterator it = list->second.begin();
      it != list->second.end(); it++)
    {
      StructEntry* entry = *it;
      string type = entry->type ? type_name[entry->type] : entry->child;

      cout << "     " << entry->name
           << " (type: " << type
           << ", length: " << entry->size
           << ", offset: " << entry->offset
           << ")" << endl;
    }
  }
  cout << endl;
}

/**
 * Load a MemoryMap via a JSON definition file or a buffer
 *
 * There are two input forms - A filename or a buffer.
 *
 * The syntax:
 * {
 *   "struct_name": [
 *     "<name1>        <type1>        [<offset>]",
 *     "example_int    uint32",
 *   ],
 *   "struct_name2": [
 *     "example        struct_name"   //< name="example", type="struct_name"
 *   ]
 * }
 *
 * @param file   The input JSON definition file or beginning of buffer.
 *               A buffer has to start with a '{'.
 *
 * @return       The size of the new memory_map
 */
size_t StructParser::load_memory_map(const char* file)
{
  json_value* json;
  vector<char> buffer;
  int errorLine = 0;
  char *errorPos = NULL;
  char *errorDesc = NULL;

  if (file[0] == '{')
  {
    buffer.assign(file, file+strlen(file)+1);
    //cout << " Parsing JSON MemoryMap (Size: " << buffer.size() << ")..." << endl;
  }
  else
  {
    //cout << " Loading MemoryMap file " << file << "..." << endl;
    FILE_getContent(file, buffer);
    buffer.push_back('\0');
  }

  json = json_parse(&buffer[0], &errorPos, &errorDesc, &errorLine);
  if (!json)
  {
    throw_exception("JSON parser error at line "+ Stringify(errorLine) +": "
                      + string(errorDesc) +"\n'"+ string(errorPos) +"'\n\n", 0);
  }

  // Run the Fls3 Parser on all the types
  try {
    //cout << " Looping through JSON MemoryMap..." << endl;
    for (json_value *it = json->first_child; it; it = it->next_sibling)
    {
      structure_parser(it);
    }
    json_destroy(json);
  }
  catch (exception &e) {
    #ifndef __EXCEPTIONS
      myexception& e = chatched_exceptions.front();
    #endif
    json_destroy(json);
    throw_exception(" JSON Structure Parser Error: "+ string(e.what()), 0);
  }

  return m_map_size;
}

/**
 * Parses a JSON structure definition.
 * This is an iterative function that loops through the values parsed by
 * the JSON.cpp parser and generating structures in the m_struct_map.
 *
 * @param current  Current JSON value
 * @param parent   Pointer to the currently parsing Structure
 * @param parent   Pointer to the currently parsing Entry
 */
void StructParser::structure_parser(
    json_value* current,
    string parent,
    StructEntry* entry)
{
  StructType stype;
  string name, type, size, offset;

  if (current->name)
    name = current->name;

  switch(current->type)
  {
  case JSON_ARRAY:
    //< New Structure (StructEntryList)
    if (m_struct_map[name].size() != 0)
      throw_exception("Name '"+ name +"' double defined!");

    for (json_value *it = current->first_child; it; it = it->next_sibling)
    {
      structure_parser(it, name);
    }
    break;

  case JSON_OBJECT:
    throw_exception("Structure definitions are defined as Arrays");
    break;

  default:
  {
    stringstream line;

    if (parent.empty())
      throw_exception("Structure definitions are defined as Arrays");

    if (current->type != JSON_STRING)
      throw_exception("Expected STRING from One-liner entry '"+ name +"'");

    line << current->string_value;
    line >> name >> type >> size >> offset;

    stype = get_type_value(type);

    if (name.empty())
      throw_exception("Name missing from entry!");

    if (type.empty())
      throw_exception("'type' field missing from '"+ name +"'");

    string fullname = parent+name;
    if (m_entry.find(fullname) != m_entry.end())
      throw_exception("Name '"+ parent +"."+ name +"' double defined!");

    if (stype == SP_TYPE_CHILD and m_struct_map[type].size() == 0)
      throw_exception("Unknown type '"+ type +"' for entry '"+ name +"'");

    //< Create the new Entry!
    entry = new StructEntry();
    entry->name   = name;
    entry->type   = stype;
    entry->size   = size.empty()   ?  1 : strtol(size.c_str(), NULL, 0);
    entry->offset = offset.empty() ? -1 : strtol(offset.c_str(), NULL, 0);

    if (stype == SP_TYPE_CHILD)
      entry->child = type;

    m_entry[fullname] = entry;
    m_struct_map[parent].push_back( entry );

    break;
  }
  } //< Switch
}

/**
 * Calulates the offsets for a specific loaded Structure. All entries where offset
 * is less than zero are updated with an offset value aligning the entry
 * after the previous entry. If the array contains children, these arrays are
 * updated too.
 *
 * @param struct_name  Name of the loaded struct to calculate
 *
 * @return             The calculated size of the structure map
 */
size_t StructParser::calc_struct_size(string struct_name)
{
  size_t size = 0;
  const StructEntryList& list = get_structure(struct_name);
  //cout << " Setting up map '" << struct_name << "'..." << endl;

  for (StructEntryList::const_iterator it = list.begin();
       it != list.end(); it++)
  {
    StructEntry* entry = *it;
    //cout << "   -> " << struct_name << "." << entry->name << " (" << dec << size << ")"<< endl;

    if (entry->offset < 0)
    {
      entry->offset = size;
    }

    if (entry->type == SP_TYPE_CHILD)
    {
      if (m_type_size_map[entry->child] == 0)
      {
        m_type_size_map[entry->child] = calc_struct_size(entry->child);
      }

      size = entry->offset + m_type_size_map[entry->child] * entry->size;
    }
    else
    {
      size = entry->offset + type_size[entry->type] * entry->size;
    }
  }

  return size;
}

/**
 * Class constructor.
 */
StructParser::StructParser()
  : is_owner(true), m_map_size(0)
{
}

/**
 * Class constructor.
 *
 * @param json_map    Either a file or a string with a JSON definition map
 * @param struct_name The name of the root structure. This can also be set
 *                    when loading the buffer.
 */
StructParser::StructParser(const char *json_map, const char* struct_name)
  : is_owner(true),
    m_map_size(0)
{
  load_memory_map( json_map );

  // TODO: Move to StructParser::set_struct_name()
  if (struct_name)
  {
    m_struct = struct_name;
    if (m_type_size_map[m_struct] == 0)
      m_type_size_map[m_struct] = calc_struct_size(m_struct);

    m_map_size = m_type_size_map[m_struct];

    m_buffer.assign(m_map_size, 0);
    set_iter(m_buffer.begin(), 0);
  }
}

/**
 * Class Copy constructor.
 *
 * Since we are using this alot for children it has been optimized.
 * Instead of copying the entire buffer, the iter is simply pointing
 * to the parent buffer.
 */
StructParser::StructParser(const StructParser& _copy)
{
  is_owner = false;
  m_type_size_map = _copy.m_type_size_map;
  m_struct_map = _copy.m_struct_map;

  m_entry = _copy.m_entry;
  m_struct = _copy.m_struct;
  m_map_size = _copy.m_map_size;

  //vector<uint8_t> m_buffer;
  m_iter = _copy.m_iter;
}

/**
 * Class destructor.
 * Remove all allocated StructParserEntries & Children.
 */
StructParser::~StructParser()
{
  if (is_owner)
  {
    for (StructEntryMap::iterator it = m_entry.begin();
      it != m_entry.end(); it++)
    {
      // cout << " Deleting " << it->first << endl;
      delete it->second;
    }
    m_entry.clear();
  }
  for (StructChildMap::iterator it = m_child_map.begin();
    it != m_child_map.end(); it++)
  {
    //cout << " Deleting child " << it->first << endl;
    delete it->second;
  }
  m_child_map.clear();
}

/**
 * Calculates the structure size & setups the buffer
 *
 * @param struct_name   The name from the loaded structure map.
 */
StructParser& StructParser::set_structure(string struct_name)
{
  get_structure(struct_name);

  m_struct = struct_name;

  if (m_type_size_map[m_struct] == 0)
    m_type_size_map[m_struct] = calc_struct_size(m_struct);

  m_map_size = m_type_size_map[m_struct];

  m_buffer.assign(m_map_size, 0);
  //m_buffer.assign(&buffer[0], &buffer[size]);

  set_iter( m_buffer.begin(), m_map_size );
  return *this;
}


/**
 *  Loads a structure type equal to the struct_name via an input stream.
 *  This name should already have been loaded via a JSON file.
 *
 * @param struct_name   Structure to interpret (loaded via JSON)
 * @param stream        Input stream to read from
 */
void StructParser::load(std::istream &stream, string struct_name)
{
  if (!struct_name.empty())
    set_structure(struct_name);

  stream.read((char*)&m_buffer[0], m_map_size);

  if (!stream)
  {
    if (stream.eof())
      throw_exception("Failed reading '"+ m_struct
                     +"' ("+ Stringify(m_map_size) +") - Reached EOF");

    throw_exception("Failed reading '"+ m_struct
                     +"' ("+ Stringify(m_map_size) +")");
  }
}


/**
 *  Loads a buffer with a structure type equal to the struct_name.
 *  This name should already have been loaded via a JSON file.
 *
 * @param struct_name   The structure to load
 * @param buffer        The structure buffer
 */
void StructParser::load(string struct_name, std::vector<char> &buffer)
{
  get_structure(struct_name);

  load(buffer);
  m_struct = struct_name;

  if (m_type_size_map[m_struct] == 0)
    m_type_size_map[m_struct] = calc_struct_size(m_struct);

  m_map_size = m_type_size_map[m_struct];
}

void StructParser::load(string struct_name, std::vector<uint8_t> &buffer)
{
  get_structure(struct_name);

  load(buffer);
  m_struct = struct_name;

  if (m_type_size_map[m_struct] == 0)
    m_type_size_map[m_struct] = calc_struct_size(m_struct);

  m_map_size = m_type_size_map[m_struct];
}

/**
 * Sets the buffer iterator used in the class. This is used for all child
 * StructParser's instead of copying the buffer.
 *
 * @param iter Pointer to a StructParser::m_buffer element
 */
void StructParser::set_iter(vector<uint8_t>::iterator iter, size_t size)
{
  if (size && m_map_size > m_buffer.size())
    m_buffer.resize(m_map_size, 0);

  m_iter = iter;
}

/**
 * Sets the raw buffer used in the class.
 *
 * @param buffer Buffer where entries are written to / read from. The buffer
 *               must have room for at least size() bytes.
 */
void StructParser::load(std::vector<char> &buffer)
{
  load((uint8_t*)&buffer[0], buffer.size());
}

/**
 * Sets the raw buffer used in the class.
 *
 * @param buffer Buffer where entries are written to / read from. The buffer
 *               must have room for at least size() bytes.
 */
void StructParser::load(std::vector<uint8_t> &buffer)
{
  load(&buffer[0], buffer.size());
}

/**
 * Sets the raw buffer used in the class.
 *
 * @param buffer Buffer where entries are written to / read from.
 * @param size   Size of the input buffer.
 */
void StructParser::load(uint8_t *buffer, size_t size)
{
  if (buffer && size > 0)
  {
    if (size < m_map_size)
    {
      throw_exception("Input buffer not large enough for this memory map ("+ Stringify(size) +" < "+ Stringify(m_map_size) +")");
    }

    m_buffer.resize(size, 0);
    m_buffer.assign(&buffer[0], &buffer[size]);
    set_iter(m_buffer.begin(), size);
  }
}

/**
 *  Save the current buffer
 *
 * @param file_name  The file name to save the buffer into
 **/
void StructParser::save(string file_name)
{
  ofstream file(file_name.c_str(), fstream::binary);
  if (!file)
    throw_exception("Could not open file "+ file_name);

  file.write((char*)&m_buffer[0], m_buffer.size());

  file.close();
}

/**
 * Clears the internal buffer
 */
void StructParser::clear()
{
  m_buffer.assign( m_map_size, 0);
  set_iter(m_buffer.begin(), 0);
}

/**
 * Returns the raw buffer used in the class. This is a pointer to the memory
 * previously set using load() / StructParser().
 * If the buffer is resized remember to reset the iterator via:
 * set_iter( m_buffer.begin() );
 *
 * @return Returns the raw buffer used in the class.
 */
std::vector<uint8_t>& StructParser::buffer()
{
  if (!is_owner) // && m_type_size_map[m_struct] != m_map_size)
  {
    //cout << " Copying original data to local buffer!" << endl;
    m_map_size = m_type_size_map[m_struct];
    m_buffer.assign( m_iter, m_iter + m_map_size);
    set_iter( m_buffer.begin(), 0 );
  }
  return m_buffer;
}

/**
 * Returns the raw ANSI C char pointer used in the class derived from buffer()
 *
 * @return Returns the raw ANSI C char pointer
 */
char* StructParser::data()
{
  return (char*)&buffer()[0];
}

/**
 * Returns total size in bytes of the memory map set using
 * set_memory_map() / StructParser().
 *
 * @return Returns total size in bytes of memory map.
 */
uint32_t StructParser::size()
{
  return m_type_size_map[m_struct];
}

/**
 * Returns the number of values in a given memory map entry.
 *
 * @param entry_name Name of entry.
 *
 * @return           Returns the number of values in the memory map entry named
 *                   entry_name.
 */
uint32_t StructParser::size(const char *entry_name)
{
  if (m_struct_map.find( entry_name ) != m_struct_map.end())
  {
    if (m_type_size_map[entry_name] == 0)
      m_type_size_map[entry_name] = calc_struct_size(entry_name);

    return m_type_size_map[entry_name];
  }

  return get_entry(entry_name)->size;
}

/**
 * Prints content and meta data for a memory map entry to cout. Useful for
 * debugging purposes.
 *
 * @param entry  Entry to print.
 * @param indent Indentation level. 2*indent spaces will be prepended to each
 *               printed line.
 */
void StructParser::print_struct(StructEntry *entry, uint32_t indent)
{
  uint32_t offset = entry->offset;
  const char* name = entry->name.c_str();

  string indent_str = "";
  char prev_fill = cout.fill('0');
  for (uint32_t i=0; i<indent; i++)
  {
    indent_str += "  ";
  }

  /* Print entry name, type, size, and offset */
  if (entry->type != SP_TYPE_CHILD)
  {
    cout << indent_str << entry->name
         << " (type: " << type_name[entry->type]
         << ", size: " << entry->size
         << ", offset: " << dec << entry->offset << ")";
  }

  /* Print entry values */
  for (uint32_t i=0; i<entry->size; i++)
  {
    switch (entry->type)
    {
      case SP_TYPE_CHILD:
      {
        cout << indent_str << entry->name << "[" << i << "]" << endl;
        string my_struct = m_struct;
        vector<uint8_t>::iterator iter = m_iter;

        size_t size = m_type_size_map[entry->child];
        set_iter(m_iter + offset + i*size, size);
        m_struct = entry->child;
        print(entry->child, indent+1);

        m_struct = my_struct;
        set_iter(iter, 0);
        break;
      }
      case SP_TYPE_UINT8:
      case SP_TYPE_INT8:
        if (i%16 == 0) /* Make a newline for each 16'th value */
        {
          cout << endl << indent_str << "  ";
        }
        cout << setw(2) << hex << (int)get_uint8(name, i) << " ";
        break;
      case SP_TYPE_UINT16_LE:
      case SP_TYPE_INT16_LE:
        if (i%8 == 0) /* Make a newline for each 8'th value */
        {
          cout << endl << indent_str << "  ";
        }
        cout << setw(4) << hex << get_uint16(name, i) << " ";
        break;
      case SP_TYPE_UINT32_LE:
      case SP_TYPE_INT32_LE:
        if (i%8 == 0) /* Make a newline for each 8'th value */
        {
          cout << endl << indent_str << "  ";
        }
        cout << setw(8) << hex << get_uint32(name, i) << " ";
        break;
      case SP_TYPE_UINT64_LE:
      case SP_TYPE_INT64_LE:
        if (i%4 == 0) /* Make a newline for each 4'th value */
        {
          cout << endl << indent_str << "  ";
        }
        cout << setw(16) << hex << get_uint64(name, i) << " ";
        break;
      default:
        throw_exception("Unknown entry type");
    }
  }

  cout << dec;
  if (entry->type != SP_TYPE_CHILD)
  {
    cout << endl;
  }
  cout.fill(prev_fill);
}

/**
 * Finds an entry by a given name.
 *
 * @param entry_name Entry name to look for.
 *
 * @return           Returns entry named entry_name.
 */
StructParser::StructEntry *StructParser::get_entry(const string& entry_name)
{
  if (m_entry.find( m_struct+entry_name ) != m_entry.end())
    return m_entry[m_struct+entry_name];

  throw_exception("Could not find entry '"+ m_struct+"."+entry_name+"'", NULL);
}

/**
 * Returns a new StructParser object for a given memory map entry. The entry
 * must be of type SP_TYPE_CHILD.
 *
 * @param entry_name Name of entry.
 * @param index      The index of the child value to get. Must be less than
 *                   size(entry_name).
 *
 * @return           Returns a new StructParser object for a given memory map
 *                   entry. The object must be freed by caller.
 */
StructParser& StructParser::get_child(const char *entry_name, uint32_t index)
{
  size_t size;
  StructParser *child;
  StructEntry *entry;

  entry = get_entry(entry_name);
  validate_type(entry, SP_TYPE_CHILD);
  validate_index(entry, index);

  string child_name = entry->child +"/"+ Stringify(index);

  if (m_child_map.find(child_name) == m_child_map.end())
  {
    child = new StructParser(*this);
    m_child_map[child_name] = child;
  }
  else
  {
    child = m_child_map[child_name];
  }

  child->m_struct = entry->child;
  size = m_type_size_map[entry->child];
  child->set_iter(m_iter + entry->offset + index*size, size);
  return *child;
}

char* StructParser::get_data_ptr(const char *entry_name, uint32_t index)
{
  size_t size;
  StructEntry *entry;

  entry = get_entry(entry_name);
  //validate_type(entry, SP_TYPE_CHILD);
  validate_index(entry, index);

  size = m_type_size_map[entry->child];
  return (char*)&m_buffer.at(entry->offset + index*size);
}

/**
 * Retrieve a Structure definition from the Structure Map.
 * Throws an exception if the structure have not been loaded.
 *
 * @param struct_name The Structure to retrieve
 *
 * @return            A list of the structure
 */
StructParser::StructEntryList& StructParser::get_structure(const string& struct_name)
{
  if (m_struct_map[struct_name].size() == 0)
  {
    throw_exception("Structure '"+ struct_name +"' does not exist", m_struct_map[struct_name]);
  }

  return (StructEntryList&)m_struct_map[struct_name];
}

/* ------------------------- UINT 8 GETTERS/SETTERS --------------------- */

/**
 * Gets array of unsigned 8 bit integers for a given memory map entry. The entry
 * must be of type SP_TYPE_UINT8.
 *
 * @param entry_name Name of entry.
 *
 * @return           Returns a vector of uint8_ts for the memory map entry named
 *                   entry_name.
 */
vector<uint8_t> StructParser::get_uint8_array(const char *entry_name)
{
  StructEntry *entry;

  entry = get_entry(entry_name);
  validate_type(entry, SP_TYPE_UINT8);

  vector<uint8_t>::iterator iter = m_iter + entry->offset;
  return vector<uint8_t>(iter, iter+entry->size);
}

/**
 * Gets an unsigned 8 bit integer for a given memory map entry. The entry must
 * be of type SP_TYPE_UINT8.
 *
 * @param entry_name Name of entry.
 * @param index      The index of the entry value to get. Must be less than
 *                   size(entry_name).
 *
 * @return           Returns uint8_t at given index for the memory map entry
 *                   named entry_name.
 */
uint8_t StructParser::get_uint8(const char *entry_name, uint32_t index)
{
  StructEntry *entry;

  entry = get_entry(entry_name);
  validate_type(entry, SP_TYPE_UINT8);
  validate_index(entry, index);

  return *(m_iter + entry->offset + index);
}

/**
 * Gets array of unsigned 8 bit integers as a std::string for a given memory map
 * entry. The entry must be of type SP_TYPE_UINT8. Only chars up to the first
 * '\0's in the entry are appended to the string. If no '\0' is present in the
 * entry, the length of the string will equal size(entry_name).
 *
 * @param entry_name Name of entry.
 *
 * @return           Returns a std::string for the memory map entry named
 *                   entry_name.
 */
string StructParser::get_string(const char *entry_name)
{
  string str;
  StructEntry *entry;

  entry = get_entry(entry_name);
  validate_type(entry, SP_TYPE_UINT8);

  str.reserve(entry->size);
  vector<uint8_t>::iterator iter = m_iter + entry->offset;
  for (size_t i=0; i<entry->size && isprint(*iter); ++iter, ++i)
    str += *iter;

  return str;
}

/**
 * Sets a std::string for a given memory map entry.
 * The entry must be of type SP_TYPE_UINT8 and the string must fit in size including.
 * the ending '\0' char.
 *
 * @param entry_name Name of entry.
 * @param value      String to insert
 */
StructParser& StructParser::set_string(const char *entry_name, string str)
{
  StructEntry *entry;

  entry = get_entry(entry_name);
  validate_type(entry, SP_TYPE_UINT8);
  validate_size(entry, str.size()+1);

  vector<uint8_t>::iterator iter = m_iter + entry->offset;
  for (size_t i=0; i<str.size()+1; ++iter, ++i)
    *iter = str[i];
  return *this;
}

/**
 * Sets array of unsigned 8 bit integers for a given memory map entry. The entry
 * must be of type SP_TYPE_UINT8.
 *
 * @param entry_name Name of entry.
 * @param vect       Vector containing the uint8_ts to set. Must contain
 *                   size(entry_name) values.
 */
StructParser& StructParser::set_uint8_array(const char *entry_name, vector<uint8_t>& vect)
{
  StructEntry *entry;

  entry = get_entry(entry_name);
  validate_type(entry, SP_TYPE_UINT8);
  validate_size(entry, vect.size());

  for (uint32_t i=0; i<vect.size(); i++)
  {
    m_iter[entry->offset + i] = vect[i];
  }
  return *this;
}

/**
 * Sets an unsigned 8 bit integer for a given memory map entry. The entry must
 * be of type SP_TYPE_UINT8.
 *
 * @param entry_name Name of entry.
 * @param value      The uint8_t value to set.
 * @param index      The index of the entry value to set. Must be less than
 *                   size(entry_name).
 */
StructParser& StructParser::set_uint8(const char *entry_name, uint8_t value, uint32_t index)
{
  StructEntry *entry;

  entry = get_entry(entry_name);
  validate_type(entry, SP_TYPE_UINT8);
  validate_index(entry, index);

  m_iter[entry->offset + index] = value;
  return *this;
}

/* ------------------------- UINT 16 GETTERS/SETTERS --------------------- */

/**
 * Gets array of unsigned 16 bit integers for a given memory map entry. The
 * entry must be of type SP_TYPE_UINT16_LE.
 *
 * @param entry_name Name of entry.
 *
 * @return           Returns a vector of uint16_ts for the memory map entry
 *                   named entry_name.
 */
vector<uint16_t> StructParser::get_uint16_array(const char *entry_name)
{
  vector<uint16_t> vect;
  StructEntry *entry;
  uint16_t value;

  entry = get_entry(entry_name);
  validate_type(entry, SP_TYPE_UINT16_LE);

  vect.reserve(entry->size);

  for (uint32_t i=0; i < entry->size; i++)
  {
    value = get_uint16_le(m_iter + entry->offset + i*2);
    vect.push_back(value);
  }

  return vect;
}

/**
 * Gets an unsigned 16 bit integer for a given memory map entry. The entry must
 * be of type SP_TYPE_UINT16_LE.
 *
 * @param entry_name Name of entry.
 * @param index      The index of the entry value to get. Must be less than
 *                   size(entry_name).
 *
 * @return           Returns uint16_t at given index for the memory map entry
 *                   named entry_name.
 */
uint16_t StructParser::get_uint16(const char *entry_name, uint32_t index)
{
  StructEntry *entry;

  entry = get_entry(entry_name);
  validate_type(entry, SP_TYPE_UINT16_LE);
  validate_index(entry, index);

  return get_uint16_le(m_iter + entry->offset + index*2);
}

/**
 * Sets array of unsigned 16 bit integers for a given memory map entry. The
 * entry must be of type SP_TYPE_UINT16_LE.
 *
 * @param entry_name Name of entry.
 * @param vect       Vector containing the uint16_ts to set. Must contain
 *                   size(entry_name) values.
 */
StructParser& StructParser::set_uint16_array(const char *entry_name, vector<uint16_t>& vect)
{
  StructEntry *entry;

  entry = get_entry(entry_name);
  validate_type(entry, SP_TYPE_UINT16_LE);
  validate_size(entry, vect.size());

  for (uint32_t i=0; i<vect.size(); i++)
  {
    set_uint16_le((m_iter + entry->offset + i*2), vect[i]);
  }
  return *this;
}

/**
 * Sets an unsigned 16 bit integer for a given memory map entry. The entry must
 * be of type SP_TYPE_UINT16_LE.
 *
 * @param entry_name Name of entry.
 * @param value      The uint16_t value to set.
 * @param index      The index of the entry value to set. Must be less than
 *                   size(entry_name).
 */
StructParser& StructParser::set_uint16(const char *entry_name, uint16_t value, uint32_t index)
{
  StructEntry *entry;

  entry = get_entry(entry_name);
  validate_type(entry, SP_TYPE_UINT16_LE);
  validate_index(entry, index);

  set_uint16_le((m_iter + entry->offset + index*2), value);
  return *this;
}

/* ------------------------- UINT 32 GETTERS/SETTERS --------------------- */

/**
 * Gets array of unsigned 32 bit integers for a given memory map entry. The
 * entry must be of type SP_TYPE_UINT32_LE.
 *
 * @param entry_name Name of entry.
 *
 * @return           Returns a vector of uint32_ts for the memory map entry
 *                   named entry_name.
 */
vector<uint32_t> StructParser::get_uint32_array(const char *entry_name)
{
  vector<uint32_t> vect;
  StructEntry *entry;
  uint32_t value;

  entry = get_entry(entry_name);
  validate_type(entry, SP_TYPE_UINT32_LE);

  vect.reserve(entry->size);
  for (uint32_t i=0; i<entry->size; i++)
  {
    value = get_uint32_le(m_iter + entry->offset + i*4);
    vect.push_back(value);
  }

  return vect;
}

/**
 * Gets an unsigned 32 bit integer for a given memory map entry. The entry must
 * be of type SP_TYPE_UINT32_LE.
 *
 * @param entry_name Name of entry.
 * @param index      The index of the entry value to get. Must be less than
 *                   size(entry_name).
 *
 * @return           Returns uint32_t at given index for the memory map entry
 *                   named entry_name.
 */
uint32_t StructParser::get_uint32(const char *entry_name, uint32_t index)
{
  StructEntry *entry;

  entry = get_entry(entry_name);
  validate_type(entry, SP_TYPE_UINT32_LE);
  validate_index(entry, index);

  // debug << " Reading entry '" << entry_name << "' from offset: "
  //       << (entry->offset + index*4) << endl;

  return get_uint32_le(m_iter + entry->offset + index*4);
}

/**
 * Sets array of unsigned 32 bit integers for a given memory map entry. The
 * entry must be of type SP_TYPE_UINT32_LE.
 *
 * @param entry_name Name of entry.
 * @param vect       Vector containing the uint32_ts to set. Must contain
 *                   size(entry_name) values.
 */
StructParser& StructParser::set_uint32_array(const char *entry_name, vector<uint32_t>& vect)
{
  StructEntry *entry;

  entry = get_entry(entry_name);
  validate_type(entry, SP_TYPE_UINT32_LE);
  validate_size(entry, vect.size());

  for (uint32_t i=0; i<vect.size(); i++)
  {
    set_uint32_le((m_iter + entry->offset + i*4), vect[i]);
  }
  return *this;
}

/**
 * Sets an unsigned 32 bit integer for a given memory map entry. The entry must
 * be of type SP_TYPE_UINT32_LE.
 *
 * @param entry_name Name of entry.
 * @param value      The uint32_t value to set.
 * @param index      The index of the entry value to set. Must be less than
 *                   size(entry_name).
 */
StructParser& StructParser::set_uint32(const char *entry_name, uint32_t value, uint32_t index)
{
  StructEntry *entry;

  entry = get_entry(entry_name);
  validate_type(entry, SP_TYPE_UINT32_LE);
  validate_index(entry, index);

  set_uint32_le((m_iter + entry->offset + index*4), value);
  return *this;
}

/* ------------------------- UINT 64 GETTERS/SETTERS --------------------- */

/**
 * Gets array of unsigned 64 bit integers for a given memory map entry. The
 * entry must be of type SP_TYPE_UINT64_LE.
 *
 * @param entry_name Name of entry.
 *
 * @return           Returns a vector of uint64_ts for the memory map entry
 *                   named entry_name.
 */
vector<uint64_t> StructParser::get_uint64_array(const char *entry_name)
{
  vector<uint64_t> vect;
  StructEntry *entry;
  uint64_t value;

  entry = get_entry(entry_name);
  validate_type(entry, SP_TYPE_UINT64_LE);

  vect.reserve(entry->size);
  for (uint32_t i=0; i<entry->size; i++)
  {
    value = get_uint64_le(m_iter + entry->offset + i*8);
    vect.push_back(value);
  }

  return vect;
}

/**
 * Gets an unsigned 64 bit integer for a given memory map entry. The entry must
 * be of type SP_TYPE_UINT64_LE.
 *
 * @param entry_name Name of entry.
 * @param index      The index of the entry value to get. Must be less than
 *                   size(entry_name).
 *
 * @return           Returns uint64_t at given index for the memory map entry
 *                   named entry_name.
 */
uint64_t StructParser::get_uint64(const char *entry_name, uint32_t index)
{
  StructEntry *entry;

  entry = get_entry(entry_name);
  validate_type(entry, SP_TYPE_UINT64_LE);
  validate_index(entry, index);

  return get_uint64_le(m_iter + entry->offset + index*8);
}

/**
 * Sets array of unsigned 64 bit integers for a given memory map entry. The
 * entry must be of type SP_TYPE_UINT64_LE.
 *
 * @param entry_name Name of entry.
 * @param vect       Vector containing the uint64_ts to set. Must contain
 *                   size(entry_name) values.
 */
StructParser& StructParser::set_uint64_array(const char *entry_name, vector<uint64_t>& vect)
{
  StructEntry *entry;

  entry = get_entry(entry_name);
  validate_type(entry, SP_TYPE_UINT64_LE);
  validate_size(entry, vect.size());

  for (uint32_t i=0; i<vect.size(); i++)
  {
    set_uint64_le((m_iter + entry->offset + i*8), vect[i]);
  }
  return *this;
}

/**
 * Sets an unsigned 64 bit integer for a given memory map entry. The entry must
 * be of type SP_TYPE_UINT64_LE.
 *
 * @param entry_name Name of entry.
 * @param value      The uint64_t value to set.
 * @param index      The index of the entry value to set. Must be less than
 *                   size(entry_name).
 */
StructParser& StructParser::set_uint64(const char *entry_name, uint64_t value, uint32_t index)
{
  StructEntry *entry;

  entry = get_entry(entry_name);
  validate_type(entry, SP_TYPE_UINT64_LE);
  validate_index(entry, index);

  set_uint64_le((m_iter + entry->offset + index*8), value);
  return *this;
}
