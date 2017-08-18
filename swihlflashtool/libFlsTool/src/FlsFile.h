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
 *  $File name:  /msw_tools/FlashTool/libFlsTool/src/FlsFile.h $
 *  $CC-Version: .../dhkristx_140314_v0/3 $
 *  $Date:       2014-04-30    10:42:36 UTC $
 */

#ifndef FLSFILE_H_
#define FLSFILE_H_

#include "FileIO.h"
#include "FlsMetaFile.h"

/**
 * System Version define
 */
typedef struct {
  char* filename;
  char* version;
  char* customer[3];
  char* subsystem[10];
} SystemVersionLabelType;

typedef enum {
  UNKNOWN_ENDIAN_ARCH,
  LITTLE_ENDIAN_ARCH,
  BIG_ENDIAN_ARCH
} endian_t;

#define NAND_SYSTEM_IMAGE_OFFSET       0x40000

#ifdef __cplusplus
extern "C" {
#endif

void hexDump(const char* memory, size_t size, size_t offset);

#ifdef __cplusplus
}

#include "Logger.h"
#include "FlsMetaFile.h"

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>
#include <set>

using namespace std;
using namespace ipa;
using namespace ipa::fls;

//------------------------------------------------------------------------------
// Memory Class defines
void hexDump(vector<uint8_t> data, size_t size = 0, size_t offset = 0);


//------------------------------------------------------------------------------
// Memory Class defines
//------------------------------------------------------------------------------

typedef struct {
        unsigned int id;
        const char *name;
        int         weight;
} MemClassType;

MemClassType findMemClass(unsigned int memclass);

const char* detect_file_type(string path);

string FlsFile_get_unique_name(string name,
                               string ext = "",
                               bool overwrite = false);

//------------------------------------------------------------------------------
// Flash Files Class
//------------------------------------------------------------------------------

/* Intel Platform Abstraction - Fls */
namespace ipa
{
  namespace fls
  {

class FlsFile : public FlsMetaFile
{
private:
  string m_name;               //< Original filename
  string m_path;               //< Original full filename path
  bool saved;
  vector<string> m_tmp_files;  //< List of temporary files

  /* SysVersion information */
  string SysVersion;
  string SysCustomer[3];
  string SysSubsystem[10];

public:
  /* The new format */
  FlsFile(string path = "", int verbose = 0);
  virtual ~FlsFile();

  /**
   * Load an Fls file (Supported versions: 2 & 3)
   */
  void load(string path);

  /**
   * Save the Fls file (Fls3 only)
   */
  void save(string path, bool overwrite = false);

  /***
   * Extract the FLS content into path
   *
   * @param path   Output path
   */
  void extract(string path);

  /**
   * Clear all files in the object, leaving us with a PRG file
   */
  void clear_files();

  /***
   * Extract the FLS content into path
   *
   * @param path   Output path
   */
  void import_tag(FlsFile& file, string tag, string path);

  /**
   * Extract the Metadata via std::ostream
   */
  ostream& metadata(ostream& stream = cout);

  friend ostream& operator <<(ostream& os, FlsFile* file);
  friend ostream& operator <<(ostream& os, FlsFile& file);

  /**
   *  Dumps all SecPacks
   */
  void dump_sec_pack();

  /**
   * HexToFls - Adding sections
   */
  // void append(FlsFile& file);
  void append(string tag, string path, string name = "");

  void append_list(string tag, vector<string> paths, string name = "");

  /**
   *  Low level HexToFls functions
   */
  DownloadFile* append_file(MemoryArea* area, string path, string name = "");

  EmbeddedFile* append_boot_file(string path, MemoryClassId id);

  DownloadFile* append_iffs_file(string tag, string path, string name = "");

  /**
   *  Getting embedded file contents
   *
   * @param region  The MemoryRegion to load from
   * @param buffer  The buffer to fill
   * @param length  Amount of bytes to read (default 0: meaning whole file)
   * @param offset  Where to start reading (default 0)
   *
   * @return        Pointer to the first uint8_t of the buffer
   **/
  uint8_t* get_content(MemoryRegion* region, std::vector<uint8_t> &buffer, uint64_t length = 0, uint64_t offset = 0);

  /**
   *  Saves the contents of an embedded file via its MemoryRegion
   *
   * @param region    The region to save to
   * @param buffer    Buffer to save
   * @param length    How much to read
   * @param offset    Where to start reading
   *
   * @return          Pointer to the beginning byte of the buffer
   */
  uint8_t* set_content(MemoryRegion* region, std::vector<uint8_t> &buffer, uint64_t length);

  /**
   *  Saves the contents of an embedded file via its MemoryRegion
   *
   * @param region    The region to save to
   * @param file      File to setup as the content
   */
  void set_content(MemoryRegion* region, string file);

  /**
   * Generate the Fls2 SecPack from the current
   * PartitionTable and MemoryMap.
   *
   * Note: If the SecPack exists it will be overwritten!
   */
  void generate_sec_pack2(DownloadFile* dl_file, MemoryClass& type);

  /**
   * Injecting a file (PSI / EBL / MetaFiles)
   */
  void inject_file(MemoryClassId type, string path, string name = "");
  EmbeddedFile* inject_meta_file(string path, string name = "", bool replace = false);

  /**
   * Get the System Version string from a CODE section
   */
  string get_system_version(SystemVersionLabelType* labels = NULL);

  /**
   * Inject NVM/Flashless parts
   */
  void inject_nvm(string path);
  // void inject_security(string path);
  // void inject_certificate(string cert_path);

  vector<uint32_t> get_nvm_tags(DownloadFile* file);

  /**
   *  Runs a script on the currently parsed MemoryMap
   */
  void run_script(string path);

  /**
   * Get'ers
   */
  inline std::string name() { return m_name; };
  inline std::string path() { return m_path; };
  inline MemoryClass type() { return download_files[0]->type; }
};

  } /* namspace fls */
} /* namspace ipa */


  /**
   * ANSI C Interface
   */

extern "C" {
#endif /* __cplusplus */

  /**
   * Verifies the integrity of an Fls file
   *
   * @param file   The fls file to verify
   * @return       True if verified, otherwise false
   */
  uint8_t verify_fls(const char* file);

#ifdef __cplusplus
}
#endif

#endif /* FLSFILE_H_ */
