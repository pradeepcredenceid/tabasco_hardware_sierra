/* -------------------------------------------------------------------------
 Copyright (C) 2013-2014 Intel Mobile Communications GmbH

      Sec Class: Intel Confidential (IC)
 ----------------------------------------------------------------------
 Revision Information:
    $File name:  /msw_tools/FlashTool/libFlsTool/src/GenericFile.h $
    $CC-Version: .../dhkristx_140314_v0/1 $
    $Date:       2014-04-22    9:13:59 UTC $
 ----------------------------------------------------------------------
 by CCCT (0.12h)
 ---------------------------------------------------------------------- */

#ifndef GENERIC_FILE_IO
#define GENERIC_FILE_IO

#include <stdint.h>

#ifdef __cplusplus

#include <string>
#include <fstream>
#include <vector>

#include "Crypto.h"
#include "Exceptions.h"

using namespace std;

namespace ipa
{
  #define VerifyFileOpen(format, arg...) \
  { \
    if (!file || !is_open()) \
      throw_exception(string(__func__)+" failed: File '"+ m_name +"' is not open", 0); \
  }

  /**
   *  The Generic File Interface
   **/
  class GenericFile
  {
    fstream* file;
    string m_name;
    uint64_t m_size;
    uint64_t m_hash;

  public:
    GenericFile(string path = "", uint64_t size = 0);
    GenericFile(const GenericFile& copy);
    GenericFile& operator=(const GenericFile& copy);
    virtual ~GenericFile();

    virtual void open(ios_base::openmode mode = ios_base::in);
    virtual void close();
    virtual void seek(uint64_t pos);
    virtual uint64_t tell();

    virtual uint64_t read(vector<uint8_t> &buffer, uint64_t length = 0);
    virtual uint64_t read(char* buffer, uint64_t size);

    virtual uint64_t write(std::vector<uint8_t> &buffer, uint64_t length = 0);
    virtual size_t write(const char* buffer, size_t size, size_t offset = 0);

    virtual bool is_open();

    /**
     * Opens, seeks, reads, & closes the file in one go
     **/
    virtual uint64_t get_hash(crypto::Algorithm algorithm);

    virtual uint64_t get_content(vector<uint8_t> &buffer,
                                  uint64_t length = 0,
                                  uint64_t offset = 0);

    virtual void set_content(string file);

    virtual void set_content(vector<uint8_t> &buffer, uint64_t size = 0);

    virtual void save(string path);
    virtual void save_to_stream(ostream& out);

    bool eof()          { return file->eof(); }
    fstream& stream()    { return *file; }
    fstream& operator*() { return stream(); }

    /* get/set */
    virtual string name() const { return m_name; }
    virtual void rename(string new_name) { m_name = new_name; }

    virtual uint64_t size() const { return m_size; }
    virtual void resize(uint64_t new_size) { m_size = new_size; }

    /* Read-only Specifics */
    string modified_str();

    virtual string username() const;
    virtual void username(string name);

    virtual void groupname(string name);

    virtual time_t modified() const;
    virtual void modified(time_t modtime);
  };

  /**
   *  Handling of embedded files in Fls
   *  Basically these files are Read-Only so when changing the content
   *  the original content is not changed.
   *  Instead either a buffer is setup or a copy is created in a temporary
   *  folder.
   **/
  namespace fls
  {
    #define TAR_DATA_BLOCK    512

    class FlsEmbedFile : public GenericFile
    {
      vector<uint8_t> m_buffer;
      string m_embed_name;
      string m_tmp_file;
      uint64_t m_offset;

      /* Fls3/TAR metadata */
      time_t m_modtime;
      string m_username;
      // string groupname;

    public:
      void* m_ref; //< If the TOC haven't been read, this is saved.

    public:
      FlsEmbedFile(string file, string name, uint64_t offset = 0, uint64_t size = 0);
      FlsEmbedFile(string file, uint64_t offset = 0, uint64_t size = 0, void* ref = NULL);
      FlsEmbedFile(const FlsEmbedFile& copy);
      FlsEmbedFile& operator=(const FlsEmbedFile& copy);

      ~FlsEmbedFile();

      void open(ios_base::openmode mode = ios_base::in | ios_base::out);
      void seek(uint64_t pos);
      uint64_t tell();

      //< Because of Polymorphism we have to create empty FlsEmbedFile's
      //< This will copy the info from a pure FlsEmbedFile to an inherited class
      void setup(const FlsEmbedFile* ref);
      void enable_write();

      /**
       * Writing in embedded files might need to allocate space first
       */
      size_t write(const char* data, size_t length, size_t offset = 0);
      //uint64_t write(std::vector<uint8_t> &buffer, uint64_t length = 0);
      //uint64_t write(const char* buffer, uint64_t size);

      void set_content(string file);
      void set_content(vector<uint8_t> &buffer, uint64_t size = 0);

      uint64_t get_hash(crypto::Algorithm algorithm);

      uint64_t get_content(vector<uint8_t> &buffer,
                           uint64_t length = 0,
                           uint64_t offset = 0);
      /* get/set */
      string parent() const { return GenericFile::name(); }

      string name() const { return m_embed_name; }
      void rename(string new_name) { m_embed_name = new_name; }

      uint64_t offset() const { return m_offset; }
      void offset(uint64_t new_offset) { m_offset = new_offset; }

      string username() const { return m_username; }
      void username(string name) { m_username = name; }

      time_t modified() const { return m_modtime; }
      void modified(time_t modtime) { m_modtime = modtime; }

      /**
       *  Save embedded file
       */
      void save(string path);
      void save_to_stream(ostream& out);
    };
  }

}

#endif

#endif
