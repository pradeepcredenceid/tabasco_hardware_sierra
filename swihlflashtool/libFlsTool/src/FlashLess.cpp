/*
 *       Copyright (C) 2011-2014 Intel Mobile Communications GmbH
 *
 *            Sec Class: Intel Confidential (IC)
 *
 *
 * This document contains proprietary information belonging to IMC.
 * Denmark A/S. Passing on and copying of this document, use and communication
 * of its contents is not permitted without prior written authorisation.
 *
 * Description:
 *  Adds a static, dynamic and calibration field in a flash file.
 *  It is also possible to sign the flash file with a certificate.
 *
 * Revision Information:
 *  $File name:  /msw_tools/FlashTool/libFlsTool/src/FlashLess.cpp $
 *  $CC-Version: .../dhkristx_140314_v0/2 $
 *  $Date:       2014-04-28    9:31:08 UTC $
 *   Resonsible:
 *   Comment:
 *
 */

#include "FlashLess.h"

#include "FileIO.h"
#include "PrgSequencer.h"
#include "MemoryStream.h"
#include "Exceptions.h"

#include "FlsFile.h"       //< verify_fls()

#include <fstream>

#ifdef ANDROID
  #define LOG_TAG "lib_dl"        //< Android logcat name
  #define LOG_NDEBUG 0            //< Default verbose behaviour
  #include <utils/Log.h>
#endif

using namespace std;

//< Default verbosity
#ifdef ANDROID
  static bool m_verbose = true;
#else
  static bool m_verbose = false;
#endif

#define MAX_LOADMAP_ENTRIES 8
#define MAX_DYN_ENTRIES 100
#define MY_VERSION_MAJOR (0x0002)
#define MY_VERSION_MINOR (0x0016)
#define MY_VERSION ((MY_VERSION_MAJOR<<16)|(MY_VERSION_MINOR))

//---------------------------------------------------------------------------

#define CERT_MARKER1 0xabcdabcd
#define CERT_MARKER2 0xcdefcdef
#define CERT_MARKER_SIZE 8

//Dynamic elements from PRG sequencer:
InjectionElementStructType* EblInjectStruct;
MemoryMapElementStructType* MemoryMapElementPtr;
SecurityElementStructType* SecurityElements[MAX_DYN_ENTRIES];
DownloadDataElementStructType* DownloadElements[MAX_DYN_ENTRIES];
uint32_t NoOfSecStructs;
uint32_t NoOfDownloadStructs;
PRGSequencer FirstFile;
IFWD_MemoryStream NewCertFile;
uint8_t *NewEblData;
bool NewElementEnabled;
uint8_t *DataPtr;
string error_text;
//---------------------------------------------------------------------------

char *FlashlessGetVersion(void)
{
  return (char *)FLASHLESS_VERSION;
}

void FlashlessSetVerbose(uint8_t verbose)
{
  m_verbose = verbose;
}

void Print(const string str, int error = 0)
{
  if (m_verbose)
  {
    #ifdef ANDROID
      if (error)
        LOGE("Error: %s", str.c_str());
      else
        LOGI("%s", str.c_str());
    #endif
    if (error)
      cerr << str << endl;
    else
      cout << str << endl;
  }
}

#define Fail(err) \
  { \
    error_text = err; \
    Print(error_text, 1); \
    return false; \
  }

//---------------------------------------------------------------------------

void NewElement(void* ElementPtr)
{
  if (NewElementEnabled)
    switch (((ElementHeaderStructType*)ElementPtr)->Type)
    {
    case DOWNLOADDATA_ELEM_TYPE:
      DownloadElements[NoOfDownloadStructs++]=(DownloadDataElementStructType*)ElementPtr;
      break;
    case SECURITY_ELEM_TYPE:
      SecurityElements[NoOfSecStructs++]=(SecurityElementStructType*)ElementPtr;
      //Print("SEC\n");
      break;
    case MEMORYMAP_ELEM_TYPE:
      MemoryMapElementPtr=(MemoryMapElementStructType*)ElementPtr;
      break;
    case INJECTED_EBL_ELEM_TYPE:
      EblInjectStruct=(InjectionElementStructType*)ElementPtr;
      DataPtr=EblInjectStruct->Data.Data;
      break;

    }
}
//---------------------------------------------------------------------------

uint32_t FindSizeInMemoryMap(char *Tag)
{
  uint32_t i;
  for (i=0;i<MAX_MAP_ENTRIES;i++)
  {
    if (MemoryMapElementPtr->Data.Entry[i].Class==NOT_USED)
    {
      if (!strcmp(Tag,(char*)MemoryMapElementPtr->Data.Entry[i].FormatSpecify))
      {
        return(MemoryMapElementPtr->Data.Entry[i].Length);
      }
    }
  }
  return 0;
}

//---------------------------------------------------------------------------

uint32_t FindInMemoryMap(char *Tag, uint32_t uid)
{
  uint32_t i;
  uint32_t Found=0;
  uint32_t Start=0;
  uint32_t Length=0;
  uint32_t RetVal=0xFF;
  for (i=0;i<MAX_MAP_ENTRIES;i++)
  {
    if (MemoryMapElementPtr->Data.Entry[i].Class==NOT_USED)
    {
      if (!strcmp(Tag,(char*)MemoryMapElementPtr->Data.Entry[i].FormatSpecify))
      {
        Start = MemoryMapElementPtr->Data.Entry[i].Start;
        Length = MemoryMapElementPtr->Data.Entry[i].Length;
        Found=1;
        break;
      }
    }
  }
  if (Found)
  {
    for (i=0;i<8;i++)
    {
      {
        if ((SecurityElements[uid]->Data.LoadMap[i].StartAddr==Start)&&
            (SecurityElements[uid]->Data.LoadMap[i].TotalLength==Length))
        {
          RetVal=i;
        }
      }
    }
  }
  return RetVal;
}

//---------------------------------------------------------------------------
// EXPORTED FUNCTIONS
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

char *IFWD_get_dll_version(void)
{
  return FlashlessGetVersion();
}

uint32_t FlashlessEnd(void)
{
  //CleanUp();
  return 0;
}

//---------------------------------------------------------------------------

void CleanUp(void)
{

}

//---------------------------------------------------------------------------

uint32_t swap_endian_uint32_t( uint32_t val )
{
  val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
  return (val << 16) | (val >> 16);
}

string getSnumString(uint8_t* RomBootResp)
{
  char str[25];
  boot_response_t* ptr = reinterpret_cast<boot_response_t*>(RomBootResp);

  uint32_t snum1 = ptr->snum_1;
  uint32_t snum2 = ptr->snum_2;
  uint32_t snum3 = ptr->snum_3;

  sprintf(str, "%08X%08X%08X",
           swap_endian_uint32_t(snum1),
           swap_endian_uint32_t(snum2),
           swap_endian_uint32_t(snum3));

  return string(str);
}


//---------------------------------------------------------------------------

void FlashlessGetEblParameters(
    uint8_t** EblData,
    uint8_t*  EblCrc,
    uint32_t* EblLength)
{
  if (EblInjectStruct)
  {
    // Parse EBL parameters to caller
    if (EblCrc && EblData && EblLength)
    {
      *EblCrc    = EblInjectStruct->Data.CRC_8;
      *EblData   = EblInjectStruct->Data.Data;
      *EblLength = EblInjectStruct->Data.DataLength;
    }
  }
}

//---------------------------------------------------------------------------

char* FlashlessGetErrorString(char* buffer, uint32_t size)
{
  if (!buffer)
    return (char*)error_text.c_str();

  strncpy(buffer, error_text.c_str(), size);

  return buffer;
}

//---------------------------------------------------------------------------

uint32_t FlashlessSetPath(uint8_t *RomBootResp,       //< Unique NVM path
                     uint8_t **FileName,         //< string& InFile
                     uint8_t *NVM_Path,          //< Unique NVM path
                     uint32_t NVM_EraseOptions,  //< Erase NVM files
                     uint8_t **EblData,
                     uint8_t *EblCrc,
                     uint32_t *EblLength)
{
  // Creating special path for this Boot Response
  string in = reinterpret_cast<const char*>(*FileName);
  string path = reinterpret_cast<const char*>(NVM_Path);
  string out = in+".fls";
  string cert, sec;

  string nvm = FileSystem_path(path, getSnumString(RomBootResp));

  error_text = " ";

  if (!FileSystem_isdir(nvm))
  {
    FileSystem_mkdir(nvm.c_str());
  }

  if (nvm[nvm.size()-1] != SEPARATOR)
    nvm += SEPARATOR;

  // Erase NVM Files
  if (NVM_EraseOptions & FLASHLESS_STATIC_ERASE)
  {
    path = nvm+"static.nvm";
    if (FileSystem_isfile( path ))
      FileSystem_remove(path.c_str());
  }

  if (NVM_EraseOptions & FLASHLESS_DYNAMIC_ERASE)
  {
    path = nvm+"dynamic.nvm";
    if (FileSystem_isfile( path ))
      FileSystem_remove(path.c_str());
  }

  if (NVM_EraseOptions & FLASHLESS_CALIB_ERASE)
  {
    path = nvm+"calib.nvm";
    if (FileSystem_isfile( path ))
      FileSystem_remove(path.c_str());
  }

  // Setting up Certificate.bin
  if (FileSystem_isfile(nvm+"certificate.bin") && EblData && EblCrc && EblLength)
      cert = nvm;

  // Setting up sec.bin
  if (FileSystem_isfile(nvm+"sec.bin"))
    sec = nvm;

  // Generate the Injected File
  //FlashlessSetVerbose(true);

  if (!FlashlessGenerate(in, out, nvm, cert, sec))
    Fail("Flashless Generation error - "+ error_text);

  // Parse EBL parameters to caller
  if (!cert.empty())
  {
    if (EblCrc && EblData && EblLength)
    {
      *EblCrc    = EblInjectStruct->Data.CRC_8;
      *EblData   = EblInjectStruct->Data.Data;
      *EblLength = EblInjectStruct->Data.DataLength;
    }
  }

  // INFO: This buffer is never freed
  char* out_file = strdup(out.c_str());
  *FileName = reinterpret_cast<uint8_t*>( out_file );
  return true;
}


uint32_t FlashlessGenFile(
    const char* InFile,
    const char* OutFile,
    const char* NVM_Path,
    const char* CertPath,
    const char* SecPath)
{
  string in = InFile;
  string out = OutFile;
  string nvm = NVM_Path;
  string cert = CertPath;
  string sec = SecPath;

  return FlashlessGenerate(in, out, nvm, cert, sec);
}


uint32_t FlashlessGenerate(
    string& InFile,
    string& OutFile,
    string& NVM_Path,
    string& CertPath,
    string& SecPath)
{
  uint32_t i,j,Result,CRC, Index,Size;
  uint32_t nvm_uid = 0;
  string path;
  IFWD_MemoryStream StaticFile;
  IFWD_MemoryStream DynamicFile;
  IFWD_MemoryStream CalibFile;
  IFWD_MemoryStream CertFile;
  IFWD_MemoryStream SecFile;

  Print("FlashlessGenerate("+ OutFile +")\n");
  if (!FileSystem_isfile(InFile))
    Fail("Could not find file: "+ InFile);

  //< Verify Fls2 Integrity
  if (!verify_fls(InFile.c_str()))
    Fail("File '"+ InFile +"' is not an fls file");

  if (!FileSystem_isdir(NVM_Path))
  {
    if (!FileSystem_mkdir(NVM_Path))
      Fail("Could not create path: " + NVM_Path);
  }

  if (NVM_Path[NVM_Path.size()-1] != SEPARATOR)
    NVM_Path += SEPARATOR;

  if(SecPath.size())
  {
    if (SecPath[SecPath.size()-1] != SEPARATOR)
      SecPath += SEPARATOR;
  }

  //clear dyn pointers:
  for (i=0;i<MAX_DYN_ENTRIES;i++)
  {
    SecurityElements[i]=0;
    DownloadElements[i]=0;
  }
  NoOfDownloadStructs=0;
  NoOfSecStructs=0;
  EblInjectStruct=0;
  NewElementEnabled=true;
  // read FLS file
  Result=FirstFile.ReadFile((char*)InFile.c_str(),FLSSIGN_TOOL_ELEM_TYPE,MY_VERSION,NewElement);
  NewElementEnabled=false;
  if (Result)
    Fail("Failure while reading FLS file: " + InFile);

  if (!NVM_Path.empty())
  {
    // handle nvm stuff
    // check if we have NVM entries in loadmap
    // this is to avoid doing NVM stuff for CDS

    uint32_t flag_found_count = 0;
    for (nvm_uid=0; nvm_uid<NoOfSecStructs; nvm_uid++)
    {
      flag_found_count = 0;
      for (i=0; i<NUMBER_OF_LOAD_MAPS; i++)
      {
        if (SecurityElements[nvm_uid]->Data.LoadMap[i].ImageFlags==0x40000000)
          flag_found_count++;
      }
      if (flag_found_count >= 3)
        break;
    }
    if (flag_found_count < 3)
    {
      // do nothing, cleanup and return FLASHLESS_BAD_INPUT_FLS
      FirstFile.CleanUp();
      Fail("Could not find elements with NVM flag in LoadMap in file: " + InFile);
    }

    // now generate download elements for the nvm areas:
    // STATIC
    path = NVM_Path+"static.nvm";
    if (!FileSystem_isfile( path ))
    {
      Print("   Generating NVM file: "+ path);
      Size=FindSizeInMemoryMap((char*)"STATIC_NVM");
      StaticFile.SetSize(Size);
      memset((char*)StaticFile.Memory,0xFF,Size);
      StaticFile.SaveToFile( path );
    }
    else
    {
      Print("   Injecting "+ path);
      StaticFile.LoadFromFile( path );
    }
    Index=FindInMemoryMap((char*)"STATIC_NVM", nvm_uid);
    //check index here
    if (Index==0xFF)
    {
      Fail("Could not elements STATIC_NVM in MemoryMap/LoadMap in file: " + InFile);
    }
    DownloadElements[NoOfDownloadStructs]=(DownloadDataElementStructType*)FirstFile.CreateElement(DOWNLOADDATA_ELEM_TYPE);
    DownloadElements[NoOfDownloadStructs]->Header.UID = nvm_uid;
    DownloadElements[NoOfDownloadStructs]->Data.LoadMapIndex=Index;
    DownloadElements[NoOfDownloadStructs]->Data.DataLength=StaticFile.Size;
    DownloadElements[NoOfDownloadStructs]->Data.Data=(unsigned char*)StaticFile.Memory;
    CRC=0;
    for (j=0;j<DownloadElements[NoOfDownloadStructs]->Data.DataLength/2;j++)
      CRC^=((unsigned short*)DownloadElements[NoOfDownloadStructs]->Data.Data)[j];
    DownloadElements[NoOfDownloadStructs]->Data.CRC=CRC;
    SecurityElements[nvm_uid]->Data.LoadMap[Index].UsedLength=StaticFile.Size;


    // DYNAMIC
    path = NVM_Path+"dynamic.nvm";
    if (!FileSystem_isfile( path ))
    {
      Print("   Generating NVM file: "+ path);
      Size=FindSizeInMemoryMap((char*)"DYNAMIC_NVM");
      DynamicFile.SetSize(Size);
      memset((char*)DynamicFile.Memory,0xFF,Size);
      DynamicFile.SaveToFile(path);
    }
    else
    {
      Print("   Injecting "+ path);
      DynamicFile.LoadFromFile(path);
    }
    Index=FindInMemoryMap((char*)"DYNAMIC_NVM", nvm_uid);
    //check index here
    if (Index==0xFF)
    {
      Fail("Could not DYNAMIC_NVM element in MemoryMap/LoadMap in file: " + InFile);
    }
    DownloadElements[NoOfDownloadStructs]=(DownloadDataElementStructType*)FirstFile.CreateElement(DOWNLOADDATA_ELEM_TYPE);
    DownloadElements[NoOfDownloadStructs]->Header.UID = nvm_uid;
    DownloadElements[NoOfDownloadStructs]->Data.LoadMapIndex=Index;
    DownloadElements[NoOfDownloadStructs]->Data.DataLength=DynamicFile.Size;
    DownloadElements[NoOfDownloadStructs]->Data.Data=(unsigned char*)DynamicFile.Memory;
    CRC=0;
    for (j=0;j<DownloadElements[NoOfDownloadStructs]->Data.DataLength/2;j++)
      CRC^=((unsigned short*)DownloadElements[NoOfDownloadStructs]->Data.Data)[j];
    DownloadElements[NoOfDownloadStructs]->Data.CRC=CRC;
    SecurityElements[nvm_uid]->Data.LoadMap[Index].UsedLength=DynamicFile.Size;

    // CALIB
    path = NVM_Path+"calib.nvm";
    if (!FileSystem_isfile( path ))
    {
      Print("   Generating NVM file: "+ path);
      Size=FindSizeInMemoryMap((char*)"CALIB_NVM");
      CalibFile.SetSize(Size);
      memset((char*)CalibFile.Memory,0xFF,Size);
      CalibFile.SaveToFile(path);
    }
    else
    {
      Print("   Injecting "+ path);
      CalibFile.LoadFromFile(path);
    }
    Index=FindInMemoryMap((char*)"CALIB_NVM", nvm_uid);
    //check index here
    if (Index==0xFF)
    {
      Fail("Could not find CALIB_NVM element in MemoryMap/LoadMap in file: " + InFile);
    }
    DownloadElements[NoOfDownloadStructs]=(DownloadDataElementStructType*)FirstFile.CreateElement(DOWNLOADDATA_ELEM_TYPE);
    DownloadElements[NoOfDownloadStructs]->Header.UID = nvm_uid;
    DownloadElements[NoOfDownloadStructs]->Data.LoadMapIndex=Index;
    DownloadElements[NoOfDownloadStructs]->Data.DataLength=CalibFile.Size;
    DownloadElements[NoOfDownloadStructs]->Data.Data=(unsigned char*)CalibFile.Memory;
    CRC=0;
    for (j=0;j<DownloadElements[NoOfDownloadStructs]->Data.DataLength/2;j++)
      CRC^=((unsigned short*)DownloadElements[NoOfDownloadStructs]->Data.Data)[j];
    DownloadElements[NoOfDownloadStructs]->Data.CRC=CRC;
    SecurityElements[nvm_uid]->Data.LoadMap[Index].UsedLength=CalibFile.Size;
  }

  if (!SecPath.empty())
  {
    // handle nvm stuff
    // check if we have NVM + SEC entries in loadmap
    for (i=1;i<5;i++)
    {
      if ((SecurityElements[nvm_uid]->Data.LoadMap[i].StartAddr==0)||
          (SecurityElements[nvm_uid]->Data.LoadMap[i].ImageFlags==0))
      {
        FirstFile.CleanUp();
        Fail("Could not find SEC_DATA element in MemoryMap/LoadMap in file: " + InFile);
      }
    }
    // now generate download element for the sec areas:
    // STATIC
    path = SecPath+"sec.bin";
    if (!FileSystem_isfile( path ))
    {
      Fail("Could not find file: " + path);
    }
    else
    {
      Print("   Injecting "+ path +" (Sec)");
      SecFile.LoadFromFile( path );
    }
    Index=FindInMemoryMap((char*)"SEC_DATA", nvm_uid);
    //check index here
    if (Index==0xFF)
    {
      Fail("Could not find SEC_DATA element in MemoryMap/LoadMap in file: " + InFile);
    }
    DownloadElements[NoOfDownloadStructs]=(DownloadDataElementStructType*)FirstFile.CreateElement(DOWNLOADDATA_ELEM_TYPE);
    DownloadElements[NoOfDownloadStructs]->Header.UID = nvm_uid;
    DownloadElements[NoOfDownloadStructs]->Data.LoadMapIndex=Index;
    DownloadElements[NoOfDownloadStructs]->Data.DataLength=SecFile.Size;
    DownloadElements[NoOfDownloadStructs]->Data.Data=(unsigned char*)SecFile.Memory;
    CRC=0;
    for (j=0;j<DownloadElements[NoOfDownloadStructs]->Data.DataLength/2;j++)
      CRC^=((unsigned short*)DownloadElements[NoOfDownloadStructs]->Data.Data)[j];
    DownloadElements[NoOfDownloadStructs]->Data.CRC=CRC;
    SecurityElements[nvm_uid]->Data.LoadMap[Index].UsedLength=SecFile.Size;
  }

  // save the new fls file
  if (FirstFile.WriteFile((char*)OutFile.c_str()))
  {
    FirstFile.CleanUp();
    error_text = "Cannot write file: " + OutFile;
    return false;
  }
  FirstFile.CleanUp();
  return true;
}
