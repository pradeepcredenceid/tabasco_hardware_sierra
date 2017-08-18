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
  $File name:  /msw_tools/FlashTool/libFlsTool/src/StructParser.h $
  $CC-Version: .../dhkristx_140314_v0/1 $
  $Date:       2014-04-22    9:14:26 UTC $
  ------------------------------------------------------------------------- */

#ifndef _STRUCT_PARSER_H_
#define _STRUCT_PARSER_H_

#include <stdint.h>
#include <vector>
#include <string>
#include <map>

#include "JSON.h"

using namespace std;

namespace ipa
{
  /**
   * Class for accessing mixed data stored in a single buffer. Typical use is for
   * read/write of a c struct saved as a single memory area to disk and loaded
   * again (possibly on another machine). Hereby the memory can be accessed in a
   * consistent way independent on machine endianess, word size and structure
   * padding.
   */
  class StructParser
  {
    /**
     * Entry type used in the StructEntry struct.
     */
    typedef enum
    {
      SP_TYPE_CHILD,      /**< Pointer to child element (StructEntryList) */
      SP_TYPE_UINT8,      /**< 8 bit unsigned little endian integer */
      SP_TYPE_INT8,       /**< 8 bit signed little endian integer */
      SP_TYPE_UINT16_LE,  /**< 16 bit unsigned little endian integer */
      SP_TYPE_INT16_LE,   /**< 16 bit signed little endian integer */
      SP_TYPE_UINT32_LE,  /**< 32 bit unsigned little endian integer */
      SP_TYPE_INT32_LE,   /**< 32 bit signed little endian integer */
      SP_TYPE_UINT64_LE,  /**< 64 bit unsigned little endian integer */
      SP_TYPE_INT64_LE    /**< 64 bit signed little endian integer */
    } StructType;

    /**
     * Struct defining an entry in a raw buffer.
     */
    typedef struct
    {
      string name;                     /**< Name of entry - should be unique in an array of StructEntrys  */
      StructType type;           /**< Type of entry */
      uint32_t size;                   /**< Number of values - if greater than 1 then entry is an array of values (with size values) */
      int32_t offset;                  /**< Position of entry in buffer - a value less than zero indicates that the offset should be auto calculated */
      string child;                    /**< If type child, this is the child structure name */
    } StructEntry;

    //< Description is list of entries
    typedef vector<StructEntry*>           StructEntryList;

    //< Maps of structures & type sizes
    typedef map<string, uint32_t>          TypeSizeMap;
    typedef map<string, StructEntryList>   DefinitionMap;
    typedef map<string, StructEntry*>      StructEntryMap;
    typedef map<string, StructParser*>     StructChildMap;

  private:
    bool is_owner;                     /**< Not the owner when using the copy constructor */
    TypeSizeMap m_type_size_map;       /**< Map of type sizes */
    DefinitionMap m_struct_map;        /**< Map of structure definitions */
    StructChildMap m_child_map;        /**< List of children structures (Created when requested) */
    StructEntryMap m_entry;            /**< Hash of all entries in a structure */

    string m_struct;                   /**< The Currently loaded struct */
    uint32_t m_map_size;               /**< The parsed map size */
    vector<uint8_t> m_buffer;          /**< The entire buffer */
    vector<uint8_t>::iterator m_iter;  /**< Pointing to the first byte of the structure */

  private:
    void validate_type(StructEntry *entry, StructType type);
    void validate_index(StructEntry *entry, uint32_t index);
    void validate_size(StructEntry *entry, uint32_t size);

    void set_iter(vector<uint8_t>::iterator iter, size_t size);
    StructType get_type_value(string name);
    StructEntry *get_entry(const string& entry_name);
    StructEntryList& get_structure(const string& struct_name);

    void structure_parser(json_value* current, string parent = "", StructEntry* entry = NULL);
    size_t calc_struct_size(string struct_name);

    void print_struct(StructEntry *entry, uint32_t indent=0);

  public:
    StructParser();
    StructParser(const StructParser& _copy);
    StructParser(const char *json_map, const char* struct_name = NULL);

    virtual ~StructParser();

    virtual void dump();
    virtual void dump_map();

    size_t load_memory_map(const char* file);

    StructParser& set_structure(string struct_name);

    void load(std::vector<char>& buffer);
    void load(std::vector<uint8_t>& buffer);
    void load(uint8_t *buffer, size_t size);
    void load(std::istream &stream, string struct_name = "");
    void load(string struct_name, std::vector<char>& buffer);
    void load(string struct_name, std::vector<uint8_t>& buffer);

    void save(string file_name);

    void clear();
    std::vector<uint8_t> & buffer();
    char* data();
    //uint8_t* operator&() { return &m_buffer[0]; }

    uint32_t size();
    uint32_t size(const char *entry_name);

    void print(string struct_name, uint32_t indent=0);

    StructParser& get_child(const char *entry_name, uint32_t index=0);
    char* get_data_ptr(const char *entry_name, uint32_t index=0);

    /* String getter and setter */
    std::string get_string(const char *entry_name);
    StructParser& set_string(const char *entry_name, std::string value);

    /* Uint8 getters and setters */
    std::vector<uint8_t> get_uint8_array(const char *entry_name);
    uint8_t get_uint8(const char *entry_name, uint32_t index=0);
    StructParser& set_uint8_array(const char *entry_name, std::vector<uint8_t>& vect);
    StructParser& set_uint8(const char *entry_name, uint8_t value, uint32_t index=0);

    /* Uint16 getters and setters */
    std::vector<uint16_t> get_uint16_array(const char *entry_name);
    uint16_t get_uint16(const char *entry_name, uint32_t index=0);
    StructParser& set_uint16_array(const char *entry_name, std::vector<uint16_t>& vect);
    StructParser& set_uint16(const char *entry_name, uint16_t value, uint32_t index=0);

    /* Uint32 getters and setters */
    std::vector<uint32_t> get_uint32_array(const char *entry_name);
    uint32_t get_uint32(const char *entry_name, uint32_t index=0);
    StructParser& set_uint32_array(const char *entry_name, std::vector<uint32_t>& vect);
    StructParser& set_uint32(const char *entry_name, uint32_t value, uint32_t index=0);

    /* Uint64 getters and setters */
    std::vector<uint64_t> get_uint64_array(const char *entry_name);
    uint64_t get_uint64(const char *entry_name, uint32_t index=0);
    StructParser& set_uint64_array(const char *entry_name, std::vector<uint64_t>& vect);
    StructParser& set_uint64(const char *entry_name, uint64_t value, uint32_t index=0);
  };

} /* Namespace IPA */

#endif /* Include guard */
