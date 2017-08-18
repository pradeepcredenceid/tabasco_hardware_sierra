/* -------------------------------------------------------------------------
 Copyright (C) 2013-2014 Intel Mobile Communications GmbH

      Sec Class: Intel Confidential (IC)
 ----------------------------------------------------------------------
 Revision Information:
    $File name:  /msw_tools/FlashTool/libFlsTool/src/GenericFile.cpp $
    $CC-Version: .../dhkristx_140314_v0/2 $
    $Date:       2014-04-29    11:39:42 UTC $
 ----------------------------------------------------------------------
 by CCCT (0.12h)
 ---------------------------------------------------------------------- */

#include "GenericFile.h"

#include "OS.h"       //< GetErrorString()
#include "FileIO.h"

#include <iostream>

using namespace std;
using namespace ipa;
using namespace ipa::fls;

// #define DEBUG_ENABLED
#ifdef DEBUG_ENABLED
  #define debug    cout
#else
  #define debug    if (0) cout
#endif

//< Buffer size for file I/O
static const size_t buf_size = 1024*1024;

GenericFile::GenericFile(string path, uint64_t size)
  : file(NULL),
    m_name(path),
    m_size(size),
    m_hash(0)
{
  if (size == 0 && fexists(path))
    m_size = fsize(path);
}

GenericFile::GenericFile(const GenericFile& copy)
{
  file = NULL;
  m_name = copy.m_name;
  m_size = copy.m_size;
  m_hash = copy.m_hash;
}

GenericFile& GenericFile::operator=(const GenericFile& copy)
{
  if(this != &copy)
  {
    file = NULL;
    m_name = copy.m_name;
    m_size = copy.m_size;
    m_hash = copy.m_hash;
  }
  return *this;
}

GenericFile::~GenericFile()
{
  if (is_open())
    file->close();
}

void GenericFile::open(ios_base::openmode mode)
{
  if (is_open())
    throw_exception("File (" + m_name + ") already open");

  //if (!(mode & (ios_base::in | ios_base::out))
  //  throw_exception("Mode not supported");

  debug << " GenericFile::open(" << m_name << ", " << Stringify((int)mode) << ")" << endl;
  file = new fstream(m_name.c_str(), ifstream::binary | mode);
  if (!is_open())
    throw_exception("Failed opening file ("+ m_name +") for reading (Mode: "
                    + Stringify((int)mode) +"): "+ GetErrorString(false));
}

bool GenericFile::is_open()
{
  return (file && file->is_open());
}

#if 0
void GenericFile::verify_is_open(string func)
{
  if (!is_open())
    throw_exception(func +" failed: File '"+ m_name +"' is not open");
}
#endif

void GenericFile::close()
{
  if (!is_open())
    throw_exception(string(__func__)+" failed: File '"+ m_name +"' is not open");

  debug << " GenericFile::close(" << m_name << ")" << endl;
  file->close();
}

void GenericFile::seek(uint64_t pos)
{
  if (!is_open())
    throw_exception(string(__func__)+" failed: File '"+ m_name +"' is not open");

  debug << " GenericFile::seek(" << m_name << ", " << getHexString(pos) << ")" << endl;
  file->seekg(pos);
}

uint64_t GenericFile::tell()
{
  VerifyFileOpen();
  return file->tellg();
}

string GenericFile::modified_str()
{
  char buf[24] = { 0 };
  time_t modtime = modified();
  struct tm *T = localtime(&modtime);

  if (T && strftime(buf,24,"%Y-%m-%d %H:%M:%S", T))
    return string(buf);
  return string("");
}

string GenericFile::username() const
{
  return fusername(m_name.c_str());
}

void GenericFile::username(string username)
{
  fset_username(m_name.c_str(), username.c_str());
}

void GenericFile::groupname(string groupname)
{
  fset_groupname(m_name.c_str(), groupname.c_str());
}

void GenericFile::modified(time_t modtime)
{
  throw_exception("GenericFile::modified() not yet implemented");
}

time_t GenericFile::modified() const
{
  return fmodified(m_name.c_str());
}

uint64_t GenericFile::read(std::vector<uint8_t> &buffer, uint64_t length)
{
  VerifyFileOpen();

  if (length > buffer.size())
    buffer.resize(length);

  if (file)
  {
    debug << " GenericFile::read(" << m_name << ", " << length << ")" << endl;
    file->read((char*)&buffer[0], buffer.size());
  }
  if (!file)
    throw_exception("Failed reading from file (" + m_name + ")", 0);

  uint64_t nbytes = file->gcount();
  if (buffer.size() > nbytes)
    buffer.resize(nbytes);

  return nbytes;
}

uint64_t GenericFile::read(char* buffer, uint64_t size)
{
  VerifyFileOpen();

  debug << " GenericFile::read(" << m_name << ", " << getHexString(size) << ")" << endl;
  file->read(buffer, size);
  if (file->bad())
    throw_exception("Failed reading from file (" + m_name + ")", 0);

  return file->gcount();
}

uint64_t GenericFile::write(std::vector<uint8_t> &buffer, uint64_t length)
{
  if (length == 0)
    length  = buffer.size();
  else if (length > buffer.size())
    buffer.resize(length);

  return write((const char*)&buffer[0], length);
}

size_t GenericFile::write(const char* buffer, size_t size, size_t offset)
{
  VerifyFileOpen();

  debug << " GenericFile::write(" << m_name << ", buffer[" << size << "])" << endl;

  if (offset)
    file->seekp(offset);

  file->write(buffer, size);
  m_hash = 0;

  if (m_size < (uint64_t)file->tellp())
    m_size = file->tellp();

  return size;
}

uint64_t GenericFile::get_content(vector<uint8_t> &buffer,
                                  uint64_t length,
                                  uint64_t offset)
{
  debug << "GenericFile::get_content(" << getHexString(length)
        << "," << getHexString(offset) << ")..." << endl;

  uint64_t bytes_read = 0;
  uint64_t previous_pos = 0;
  bool close_again = is_open();

  if (!length)
    length = m_size - offset;

  if (m_size < offset+length)
    throw_exception("File size "+ getHexString(m_size)
                   +" bytes, but "+ getHexString(offset+length)
                   +" was requested for file "+ m_name, 0);

  if (!close_again)
    open(ifstream::in);
  else
    previous_pos = tell();

  seek(offset);
  buffer.resize(length);
  bytes_read = read(buffer, length);

  if (!close_again)
    close();
  else
    seek(previous_pos);

  return bytes_read;
}

void GenericFile::set_content(string file)
{
  std::vector<uint8_t> buffer;

  bool close_again = is_open();
  if (!close_again)
    open();
  else
    seek(0);

  ifstream in(file.c_str(), ios::binary);
  if (!in)
    throw_exception("Could not open "+ file);

  while (in.read((char*)&buffer[0], buf_size))
    write(buffer);

  in.close();

  if (!close_again)
    close();
}

void GenericFile::set_content(vector<uint8_t> &buffer, uint64_t size)
{
  bool close_again = is_open();

  if (!close_again)
    open(ifstream::in);

  if (!size)
    size = buffer.size();

  write(buffer, size);

  if (!close_again)
    close();
}

/**
 * Save the file content
 *
 * @param filename Full save path
 */
void GenericFile::save(string filename)
{
  string path = fdir(filename);

  if (!FileSystem_isdir(path))
    FileSystem_mkdir(path);

  debug << " GenericFile::save(" << filename << ")" << endl;
  ofstream out(filename.c_str(), fstream::binary);
  if (!out.is_open())
    throw_exception(string("Could not open ")+ filename);

  save_to_stream(out);
  out.close();
}

/**
 * Save the file to an open ostream
 *
 * @param out  The ostream reference
 */
void GenericFile::save_to_stream(ostream& out)
{
  debug << " GenericFile::save_to_stream()" << endl;

  std::vector<uint8_t> buffer;
  uint64_t left = size();
  bool close_again = is_open();

  if (!close_again)
    open(ifstream::in);
  else
    seek(0);

  while (left > buf_size)
  {
    left -= read(buffer, buf_size);
    out.write((char*)&buffer[0], buf_size);
  }
  if (left)
  {
    read(buffer, left);
    out.write((char*)&buffer[0], left);
  }

  if (!close_again)
    close();
}

/**
 * Calculate the hash of the file content
 */
uint64_t  GenericFile::get_hash(crypto::Algorithm algorithm)
{
  if (m_hash)
  {
    debug << "  GenericFile::get_hash(" << name() << "): "
          << getHexString(m_hash) << " (Cached)" << endl;
    return m_hash;
  }

  bool close_again = is_open();
  if (!close_again)
    open(ifstream::in);
  else
    seek(0);

  uint64_t left = size();
  std::vector<uint8_t> buffer;
  while (left > crypto::buf_size)
  {
    left -= read(buffer, crypto::buf_size);
    m_hash ^= crypto::get_hash(buffer, algorithm);
  }
  if (left)
  {
    read(buffer, left);
    m_hash ^= crypto::get_hash(buffer, algorithm);
  }

  if (!close_again)
    close();

  debug << "  GenericFile::get_hash(" << name() << "): " << getHexString(m_hash) << endl;
  return m_hash;
}

/**
 *  Generic implementation of embedded Fls files
 */
FlsEmbedFile::FlsEmbedFile(string file, string name, uint64_t offset, uint64_t size)
  : GenericFile(file, size),
    m_embed_name(name),
    m_offset(offset),
    m_modtime(0),
    m_ref(NULL)
{
  debug << " FlsEmbedFile(" << file << ", " << name << ")" << endl;

  // If embedded name is a complete path, we are setting up an external file
  if (path_is_complete(name) && FileSystem_isfile(name))
  {
    set_content( name );
    m_modtime = fmodified(file);
  }
  else if (FileSystem_isfile(file))
  {
    m_modtime = fmodified(file);
  }
}

/**
 * Create a generic file with a MemoryRegion reference.
 * This reference is later used to generate the filename during Fls2Parser execution.
 */
FlsEmbedFile::FlsEmbedFile(string file, uint64_t offset, uint64_t size, void* ref)
  : GenericFile(file, size),
    m_offset(offset),
    m_ref(ref)
{
  debug << " FlsEmbedFile(" << getHexString((uintptr_t)ref) << ")" << endl;
  m_modtime = fmodified(file.c_str());
}

FlsEmbedFile::FlsEmbedFile(const FlsEmbedFile& copy)
  : GenericFile(copy)
{
  m_buffer     = copy.m_buffer;
  m_embed_name = copy.m_embed_name;
  m_tmp_file   = copy.m_tmp_file;
  m_offset     = copy.m_offset;
  m_modtime    = copy.m_modtime;
  m_username   = copy.m_username;
}

FlsEmbedFile& FlsEmbedFile::operator=(const FlsEmbedFile& copy)
{
  if(this != &copy)
  {
    GenericFile::operator=(copy);
    m_buffer     = copy.m_buffer;
    m_embed_name = copy.m_embed_name;
    m_tmp_file   = copy.m_tmp_file;
    m_offset     = copy.m_offset;
    m_modtime    = copy.m_modtime;
    m_username   = copy.m_username;
  }
  return *this;
}

void FlsEmbedFile::setup(const FlsEmbedFile* ref)
{
  if (!ref)
    throw_exception("FlsEmbedFile::setup(NULL)!");

  GenericFile::rename( ref->parent() );
  GenericFile::resize( ref->size() );

  m_buffer     = ref->m_buffer;
  m_embed_name = ref->m_embed_name;
  m_tmp_file   = ref->m_tmp_file;
  m_offset     = ref->m_offset;
  m_modtime    = ref->m_modtime;
  m_username   = ref->m_username;
}

/**
 * Destructor
 * If made wriable a tmp file is cleaned up.
 */
FlsEmbedFile::~FlsEmbedFile()
{
  if (!m_tmp_file.empty())
  {
    #ifdef DEBUG_ENABLED
      debug << " Leaving tmp file " << m_tmp_file << "..." << endl;
    #else
      debug << " Removing tmp file " << m_tmp_file << "..." << endl;
      FileSystem_remove(m_tmp_file);

      if (!FileSystem_rmdir(fdir(m_tmp_file)))
        debug << __func__ << " Failed to remove tmpdir: "
              << fdir(m_tmp_file) << ": " << GetErrorString(0) << endl;
    #endif
  }
}

/**
 * Open the embedded file
 *
 * @param mode   The open mode (default: ios_base::in)
 */
void FlsEmbedFile::open(ios_base::openmode mode)
{
  GenericFile::open(mode);

  if (m_offset > 0)
    GenericFile::seek(m_offset);
}
/*
void FlsEmbedFile::close()
{
  m_offset = (offset == 0) ? region->area->address : offset;
  GenericFile::size( (size == 0) ? region->area->size : size );
}*/

/**
 * Seek to a position in the file (without embedded offset)
 */
void FlsEmbedFile::seek(uint64_t pos)
{
  // debug << "FlsEmbedFile::seek(" << getHexString(pos)
  //     << " + " << getHexString(m_offset) << ")" << endl;
  GenericFile::seek(pos + m_offset);
}

/**
 * Tell the position of the read pointer (embedded offset subtracted)
 */
uint64_t FlsEmbedFile::tell()
{
  return GenericFile::tell() - m_offset;
}

/**
 * Save the contents of a buffer as the content of the file
 *
 * @param buffer  The buffer to save
 * @param size    The size of the buffer to save. Zero means all (default: 0)
 * @param offset  The read offset in the file. (default: 0)
 */
size_t FlsEmbedFile::write(const char* data, size_t length, size_t offset)
{
  bool was_open = is_open();
    debug << "FlsEmbedFile::write(Length: " << getHexString(length)
          << ", Offset: " << getHexString(offset) << ")" << endl;
  enable_write();

  if (!is_open())
    open();

  uint64_t wrote = GenericFile::write((char*)data, length, offset);

  if (!was_open)
    close();

  return wrote;
}

/**
 * Saves the contents of an external file as the content of the file
 *
 * @param file   The file to save
 */
void FlsEmbedFile::set_content(string file)
{
  if (is_open())
    close();

  m_offset = 0;
  m_buffer.clear();
  GenericFile::rename( file );
  GenericFile::resize( fsize(file) );

  // Writable if located in the process tmp folder
  string tmp = FileSystem_tmpdir();
  if (file.compare(0, tmp.size(), tmp) == 0)
  {
    m_tmp_file = file;
  }
}

/**
 * Save the contents of a buffer as the content of the file
 *
 * @param buffer  The buffer to save
 * @param size    The size of the buffer to save. Zero means all (default: 0)
 */
void FlsEmbedFile::set_content(vector<uint8_t> &buffer, uint64_t size)
{
  if (is_open())
    close();

  if (size == 0)
    size = buffer.size();

  m_buffer.assign(buffer.begin(), buffer.begin()+size);
  GenericFile::resize( m_buffer.size() );
}

/**
 * Calculate the hash of the file content
 */
uint64_t  FlsEmbedFile::get_hash(crypto::Algorithm algorithm)
{
  debug << "FlsEmbedFile::get_hash(" << m_embed_name << ")" << endl;

  uint64_t hash = 0;
  if (!m_buffer.empty())
  {
    hash ^= crypto::get_hash(m_buffer, algorithm);
  }
  else
  {
    hash = GenericFile::get_hash(algorithm);
  }

  return hash;
}

/**
 * Get the contents of a file.
 * If the file is in the buffer, the pointer is simply changed otherwise it is
 * done just like the GenericFile::get_content().
 *
 * @param buffer   Buffer to load content into
 * @param length   How much to read. Zero means all (default: 0)
 * @param offset   The read offset in the file. (default: 0)
 */
uint64_t FlsEmbedFile::get_content(vector<uint8_t> &buffer,
                                   uint64_t length,
                                   uint64_t offset)
{
  debug << "FlsEmbedFile::get_content(" << name()
        << ", Length: " << length << ", Offset: " << offset << ")" << endl;

  if (m_buffer.empty() || !m_tmp_file.empty())
  {
    if (size() < offset+length)
      throw_exception(name()+ " is only "+ getHexString(size())
                     +" bytes, but "+ getHexString(offset+length)
                     +" was requested", 0);

    return GenericFile::get_content(buffer, length, offset);
  }
  else
  {
    if (!length && !offset)
    {
      buffer = m_buffer;
    }
    else
    {
      if (!length)
        length = size()-offset;

      if (m_buffer.size() < offset+length)
        throw_exception("Only buffered "+ getHexString(m_buffer.size())
                       +" bytes, but "+ getHexString(offset+length)
                       +" was requested for file "+ name(), 0);

      buffer.resize(length);
      buffer.assign(&m_buffer[offset], &m_buffer[offset+length]);
    }
    return buffer.size();
  }
}

/**
 * Save the embedded file as an external file.
 * The output file will have the embedded file name.
 *
 * @param path   Path to save the file to (without filename)
 */
void FlsEmbedFile::save(string path)
{
  if (m_embed_name.empty())
      throw_exception("Embedded file does not have a name!");

  if (path[path.size()-1] != SEPARATOR)
    path += SEPARATOR;

  GenericFile::save(path+m_embed_name);
}

/**
 * Save the embedded file as an external file.
 * The output file will have the embedded file name.
 *
 * @param path   Path to save the file to (without filename)
 */
void FlsEmbedFile::save_to_stream(ostream& out)
{
  debug << " FlsEmbedFile::save_to_stream()" << endl;

  if (!m_buffer.empty())
  {
    debug << " FlsEmbedFile::save(From InBuffer!)" << endl;
    out.write((char*)&m_buffer[0], m_buffer.size());
  }
  else
  {
     GenericFile::save_to_stream(out);
  }
}

/**
 * Embedded files are read-only by default.
 * Save the file and return the new file name.
 */
void FlsEmbedFile::enable_write()
{
  if (m_tmp_file.empty())
  {
    if (is_open())
      close();

    string tmp = FileSystem_tmpdir();
    m_tmp_file = tmp + name();

    debug << "FlsEmbedFile::enable_write(" << m_tmp_file << ")" << endl;
    save( tmp );

    // Saving the new file
    m_offset = 0;
    m_buffer.clear();
    GenericFile::rename( m_tmp_file );
  }
}
