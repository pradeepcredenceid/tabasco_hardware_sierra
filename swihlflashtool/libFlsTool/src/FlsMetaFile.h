/*  -------------------------------------------------------------------------
    Copyright (C) 2013-2014 Intel Mobile Communications GmbH

         Sec Class: Intel Confidential (IC)
    ----------------------------------------------------------------------
    Revision Information:
       $File name:  /msw_tools/FlashTool/libFlsTool/src/FlsMetaFile.h $
       $CC-Version: .../dhkristx_140314_v0/2 $
       $Date:       2014-04-29    11:39:31 UTC $
    ----------------------------------------------------------------------
    Description:
       The generic FlsMetaFile format.
    ---------------------------------------------------------------------- */

#ifndef FLS_META_FILE_H
#define FLS_META_FILE_H

#include <stdint.h>

/**
 * Memory Class defines
 */
typedef enum
{
  MEMORY_CLASS_BINARY = 0,
  MEMORY_CLASS_PSI,
  MEMORY_CLASS_SLB,
  MEMORY_CLASS_EBL,
  MEMORY_CLASS_CODE,
  MEMORY_CLASS_CUST,
  MEMORY_CLASS_DYN_EEP,
  MEMORY_CLASS_STAT_EEP,
  MEMORY_CLASS_SWAP_BLOCK,
  MEMORY_CLASS_SEC_BLOCK,
  MEMORY_CLASS_EXCEP_LOG,
  MEMORY_CLASS_STAT_FFS,
  MEMORY_CLASS_DYN_FFS,
  MEMORY_CLASS_DSP_SW,
  MEMORY_CLASS_ROOT_DISK,
  MEMORY_CLASS_CUST_DISK,
  MEMORY_CLASS_USER_DISK,
  MEMORY_CLASS_HEX_EXTRACT,
  MEMORY_CLASS_TEXT,
  MEMORY_CLASS_NOT_USED
} MemoryClassId;

/* Flash Technologies (enum) */
typedef enum
{
  TECHNOLOGY_NOR = 0,
  TECHNOLOGY_NAND,
  TECHNOLOGY_FLASHLESS
} StorageTech;

/* MemoryRegion & MemoryMap options */
#ifndef BOOT_CORE_HASH_MARKER
#define BOOT_CORE_HASH_MARKER   1
#define ALLOW_SEC_BOOT_UPDATE   2
#define DONT_TOUCH_REGION       4
#define SKIP_VERSION_CHECK      8
#define ERASE_TOTAL_LENGTH      16
#define ERASE_ONLY              32
#define COMPRESSED_IMAGE        64
#define COMPRESSED_TRANSFER     128
#define USE_DRIVER_FROM_FILE    256
#define NAND_SKIP_PT_CHECK      512
#define IMAGE_HAS_PUBLIC_KEYS   1024
#define IMAGE_IS_BOOT_IMAGE     2048

#define LOAD_TO_RAM             0x00010000
#define LOAD_TO_SD              0x00020000
#define NO_LOAD_TO_RAM          0x00040000

#define EXCLUDE_UTA_FLASH       0x00100000
#define EXCLUDE_VALIDATION      0x00200000
#define EXCLUDE_RAM_LOADING     0x00400000
#define EXCLUDE_PAGING          0x00800000

#define ERASE_TOTAL             0x20000000
#define FLASHLESS               0x40000000
#define ERASE_REGION            0x80000000

#define NVM_REGION              FLASHLESS
#define ENO_OPTION              LOAD_TO_RAM
#define SD_CARD_OPTION          LOAD_TO_SD
#endif

#ifdef __cplusplus

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include "Logger.h"
#include "GenericFile.h"

using namespace std;
using namespace ipa;

/* Intel Platform Abstraction - Fls */
namespace ipa
{
  namespace fls
  {
    #define META_FILE_NAME "meta.json"
    #define PARTITION_MAGIC_NAND       0x50505056
    // #define PARTITION_MAGIC_EMMC  0x00001234

    typedef struct
    {
      MemoryClassId id;
      string name;
      string name2;
      int weight;
    } MemoryClass;

    /* Memory Options (enum) */
    class MemoryOptions
    {
       vector<uint64_t> list;
    public:
      static uint64_t get_uint64(string& name);
      static string get_string(uint64_t key);

      uint64_t front();
      uint64_t back();

      uint64_t sum();
      string to_string(uint32_t i = -1);

      void clear();
      size_t size();
      uint64_t operator[](size_t i);

      int has(uint64_t flag);

      bool remove(uint64_t flag);
      bool operator-=(uint64_t flag);

      bool add(uint64_t flag);
      bool operator+=(uint64_t flag);

      bool add(string flag);
      bool operator+=(string flag);

      bool set(uint64_t flags);
    };

//    typedef vector<uint64_t> MemoryOptions;
    typedef map<string,uint64_t> MemoryOptionNames;

    /* Platform structure */
    class PlatformInfo
    {
    public:
      PlatformInfo()
        : technology(TECHNOLOGY_NOR),
          storage_address(0),
          storage_size(0),
          usif_boot_speed(0),
          SwVersionStringLocation(0)
      {}

      string chip_name;
      string version;
      StorageTech technology;
      uint64_t storage_address;
      uint64_t storage_size;
      unsigned int usif_boot_speed;

      uint32_t SwVersionStringLocation;    //< Backwards compatible with Fls2
    };

    /* Memory Areas in the Memory Map */
    MemoryClass& get_memory_class(MemoryClassId memclass);
    MemoryClass& get_memory_class(string memclass);

    class MemoryArea
    {
    public:
      MemoryArea(MemoryClassId memclass = MEMORY_CLASS_NOT_USED)
        : type( get_memory_class(memclass) ),
          address(0),
          size(0)
      {}

      string tag;
      MemoryClass type;
      MemoryOptions flags;

      uint64_t address;
      uint64_t size;
    };


    /* NAND Partition structure */
    class NandPartition
    {
    public:
      NandPartition()
        : id(0), type(0), address(0),
          start_block(0),
          end_block(0),
          start_reservoir(0),
          end_reservoir(0),
          options(0),
          attributes(0),
          physical_partition(0),
          partition_length(0),
          max_block_in_sub_par(0),
          load_address_to_partition(0)
      {}

      uint32_t id;
      uint64_t type;                      //< ImageType (Not used in FTLE)
      uint64_t address;                   //< LoadAddrTable/UTA Address
      uint64_t start_block;               //< Start Block/Sector/Region
      uint64_t end_block;                 //< End Block/Sector/Region
      uint64_t start_reservoir;           //< NandPartitionV1::start_res_block
      uint64_t end_reservoir;             //< NandPartitionV1::end_res_block
      uint64_t options;                   //< Image Options

      string name;                        //< FTLE Name (U8[24])
      uint32_t attributes;                //< FTLE Attributes
      uint32_t physical_partition;        //< FTLE Physical Partition

      uint64_t partition_length;          //< FTL Length
      uint64_t max_block_in_sub_par;      //< NandPartitionV1
      uint64_t load_address_to_partition; //< FTL & FTLE
    };

    /**
     *  Embedded files structure
     **/
    class EmbeddedFile : public FlsEmbedFile
    {
    public:
      EmbeddedFile(EmbeddedFile const &copy);
      EmbeddedFile& operator=(EmbeddedFile const &copy);

      EmbeddedFile(FlsEmbedFile* file)
        : FlsEmbedFile(*file) { }

      EmbeddedFile(string filename, string name, uint64_t offset = 0, uint64_t size = 0)
        : FlsEmbedFile(filename, name, offset, size) { }

      EmbeddedFile(string filename, uint64_t offset = 0, uint64_t size = 0)
        : FlsEmbedFile(filename, offset, size, this) { }

    public:
      string fls_name();

      void set_content(string path);
      void set_content(vector<uint8_t> &buffer, uint64_t size = 0);

    public:
      MemoryClass type;
      map<crypto::Algorithm, string> hash;
    };


    /**
     *  Memory Regions - Parts of Download files
     */
    class MemoryRegion : public FlsEmbedFile
    {
    public:
      MemoryRegion(MemoryRegion const &copy);
      MemoryRegion& operator=(MemoryRegion const &copy);

      MemoryRegion(FlsEmbedFile* file)
        : FlsEmbedFile(*file),
          area(NULL) { }

      MemoryRegion(string filename, string name, uint64_t offset = 0, uint64_t size = 0)
        : FlsEmbedFile(filename, name, offset, size),
          area(NULL) { }

      MemoryRegion(string filename, uint64_t offset = 0, uint64_t size = 0)
        : FlsEmbedFile(filename, offset, size, this),
          area(NULL) { }

    public:
      void set_content(string path);

    public:
      // uint64_t address;        //< area->address

      // uint64_t total_size;     //< area->size
      MemoryArea* area;            //< Reference to the MemoryMap entry
      // NandPartition* partition; //< Extracted from tag or lookup

      MemoryOptions flags;
      map<crypto::Algorithm, string> hash;
    };

    /* Embedded files structure */
    class DownloadFile // : public EmbeddedFile
    {
    public:
      DownloadFile(DownloadFile &copy);
      DownloadFile& operator=(const DownloadFile& copy);

      DownloadFile(string filename, uint64_t offset = 0, uint64_t size = 0);

    public:
      size_t size();
      void dump();

    public:
      string name;                //< Part of EmbeddedFile
      string toc_path;            //< Used in DownloadLibrary v.4.xxx
      MemoryClass type;           //< Part of EmbeddedFile
      string timestamp;
      // Compression compressed;  //< Part of EmbeddedFile

      EmbeddedFile sec_pack;
      vector<MemoryRegion*> load_map;
    };

    /**
     * The Great FLS Class (Prototype)
     **/
    class FlsMetaFile
    {
    public:
      FlsMetaFile(int verbose = 0);

      FlsMetaFile(const FlsMetaFile& copy);
      FlsMetaFile& operator=(const FlsMetaFile& copy);

      /**
       * Helper function to copy embedded files [ Meta, Boot, Download ]
       */
      FlsEmbedFile* cloneEmbedFile(FlsEmbedFile* copy);
      EmbeddedFile* cloneMetaFile(EmbeddedFile* copy);
      EmbeddedFile* cloneBootFile(EmbeddedFile* copy);
      DownloadFile* cloneDownloadFile(DownloadFile* region, string tag = "");
      MemoryRegion* cloneMemoryRegion(MemoryRegion* copy);

      virtual ~FlsMetaFile();

      /**
       * Set Verbosity
       */
      void set_verbose(int verbose);

      /**
       * Testing which Fls format is used
       *
       * @return True if Fls2 file
       */
      bool is_fls2();

      /**
       * Testing which Fls format is used
       *
       * @return True if Fls3 file
       */
      bool is_fls3();

      /**
       * Tests if the area is a boot area type.
       *
       * Bootable area's are EBL & PSI.
       * PSI's are also tested for the text "FLASH" in its TAG.
       *
       * @return True if area is bootable
       */
      bool is_bootable(MemoryArea* area);

      // Defaults to JSON, because we don't want to save in old formats
      // Defaults to JSON, because we don't want to save in old formats
      virtual void save(string file, bool overwrite = false);    //< Open a ofstream and send to the next function

      /**
       * Cleanup the allocated memory
       */
      void unload();

      /**
       * Returns true if comparison is equal
       */
      bool compare_memory_map(FlsMetaFile& file);
      // bool compare(FlsFile& file); //< Add verbosity

      /**
       * Dump the meta data to a buffer
       */
      vector<uint8_t>& get_meta_data(vector<uint8_t>& buffer);

      /**
       *  Saves the JSON data to any output stream (default cout)
       **/
      ostream& dump(ostream& json = cout);

      /**
       *  Becoming friends with the ostream object
       */
      friend ostream& operator <<(ostream& os, FlsMetaFile* file);
      friend ostream& operator <<(ostream& os, FlsMetaFile& file);

      /***
       * Extract the FLS content into path
       *
       * @param path   Output path
       */
      virtual void extract(string path);

      /**
       * Return the current PSI or create a new and return that
       */
      vector<FlsEmbedFile*> get_file_list();
      FlsEmbedFile* find(string filename);

      //FlsEmbedFile* findFileByName(string name);
      //EmbedFileList findFilesByExt(string ext);

      virtual EmbeddedFile* getMetaFile(string filename, bool create = false);
      virtual EmbeddedFile* getMetaFile(MemoryClassId type, bool create = false);

      // virtual EmbeddedFile* getBootFile(string filename, bool create = false);
      virtual EmbeddedFile* getBootFile(MemoryClassId type, bool create = false);

      EmbeddedFile* getMetaFileByType(MemoryClassId type);

      DownloadFile* getDownloadFile(string name, MemoryClass type, bool create = false);

      DownloadFile* getDownloadFile(MemoryRegion* region);

      DownloadFile* getDownloadFileByAddress(uint64_t address,
                                MemoryClassId type = MEMORY_CLASS_BINARY);
      /**
       * Find a memory area via either ClassId, Address, or Tag
       *
       * @param id    The Memory Type Class ID to search for
       * @param addr  The Address to search for (either a string or an int)
       * @param tag   The text string Tag to search for
       *
       * @return      a pointer to the memory area or throws an exception
       */
      vector<MemoryArea*> getMemoryArea(MemoryClassId id);

      vector<MemoryArea*> getMemoryAreaByTag(string tag,
                                 MemoryClassId memclass = MEMORY_CLASS_BINARY);

      MemoryArea* getMemoryAreaByAddr(uint64_t addr,
                                 MemoryClassId memclass = MEMORY_CLASS_BINARY,
                                 bool fail_if_not_found = true);
      MemoryArea* getMemoryAreaByAddr(string addr,
                                 MemoryClassId memclass = MEMORY_CLASS_BINARY);

      /**
       *  Locate a memory region of an address range
       */
      MemoryRegion* getMemoryRegion(uint64_t address, uint64_t length = 0);

      //MemoryRegion* get_address_file(uint64_t address, MemoryClassId id, string file);


    private:
      /**
       * Verbosity
       *
       * Currently its different, but the idea is:
       * [ 0: error, 1: warning, 2: info, 3: debugging ]
       */
      int m_verbosity;
      //Logger error; //< cerr
      //Logger warn;  //< cout
      Logger info;
      Logger debug;

      friend class FlsFile;
      friend class Fls2Parser;
      friend class Fls3Parser;
    public:
      int version;
      bool prg_loaded;
      string m_fls_file;
      FlsEmbedFile* json_file;

      string fls_version;
      string tool_version;
      string date_created;
      string last_modified;

      PlatformInfo platform;
      uint64_t partition_marker;           //< Used by Storage
      vector<NandPartition*> partitions;

      vector<MemoryArea*> memory_map;
      vector<EmbeddedFile*> meta_files;
      vector<EmbeddedFile*> boot_files;
      vector<DownloadFile*> download_files;
    };


    /**
     *  Helper Functions
     **/
    int32_t getChipNameId(string chip_name);
    const char* getChipName(int index);

  } /* namspace fls */
} /* namspace ipa */

#endif /* __cplusplus */
#endif
