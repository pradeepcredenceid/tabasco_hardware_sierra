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
* Description:
*   DWD Static eeprom update ANSI C library functions
*
* Revision Information:
*  $File name:  /msw_tools/FlashTool/libDownloadTool/src/Portable_code/single_file_handler/EepDeltaMerge.c $
*  $CC-Version: .../oint_tools_flashtoole2_01/13 $
*  $Date:       2014-03-13    15:40:50 UTC $
*   Responsible: bentzen
*   Comment:
*     SMS00847521: "Need to remove Lint warnings..."
*/

#include "OS.h"
#include "FileIO.h"

#include <ctype.h>

#include "IFWD_mem.h"
#include "SList.h"
#include "IFWD_global_func.h"

#define SendDebugText(fmt, ...) do { } while(0)

/*---------------------------------------------------------------------------*/
#define MAX_LINE_LENGTH 150
#define EEP_NO_ADDRESS 0xffff
/*---------------------------------------------------------------------------*/
#define NOF_DATATYPES 6
const char * const DataTypeNames[NOF_DATATYPES + 1] = { "U_CHAR", "S_CHAR", "U_INT", "S_INT", "U_LONG", "S_LONG", "Unknown" };
enum DataTypes{ dtU8,  dtS8,  dtU16,  dtS16,  dtU32,  dtS32,  dtUnknown };
/*---------------------------------------------------------------------------*/
#define ALL_LIST(i) ((TAllData*)SList_Object(i, AllList))

typedef struct
{
  uint32_t       CurrAddr;
  uint32_t /**/  CurrValue;
  uint32_t       NewAddr;
  uint32_t /**/  NewValue;
  uint8_t        DataType;
  uint8_t  UseForUpdate;
  uint8_t  CalValNotNewer;
} TAllData;
/*---------------------------------------------------------------------------*/
SList *AllList = NULL;
uint8_t CurrCfgIsLoaded;
uint8_t NewCfgIsLoaded;
/*uint8_t BepIsLoaded;*/
uint8_t DepIsLoaded;
int phEepRev;
void (*fxCbFunc)(char *text) = NULL; /* Definitions for debug call-back function */
/*---------------------------------------------------------------------------*/
/* Local functions:                                                          */
/*---------------------------------------------------------------------------*/
static uint8_t GetDataType(char *type)
{
  if(type && *type)
  {
    int n;
    for(n = 0 ; n < NOF_DATATYPES ; n++)
    {
      if(strcmp(DataTypeNames[n],type) == 0) /* found match? */
        return (uint8_t)n;
    }
  }
  return (uint8_t)dtUnknown;
}

/*---------------------------------------------------------------------------*/
/* callback function that allows to free any allocated memory without having to go though the list manually */
static void OnAllListItemDelete(void *object)
{
  TAllData *p = (TAllData *)object;
  if(p)
    MEM_free((uint8_t*)p);
}
/*---------------------------------------------------------------------------*/
/* Get the version number from the CFG file name */
static int ExtractVersion(char *filename)
{
  int version = -1;
  char *p = FILE_ExtractFileExt(filename);
  if(p && *p)
  {
    p--; /* go back before extension */
    while(isdigit(*p) && p >= filename)
      p--;
    if(tolower(p[0]) == (char)'p' && tolower(p[-1]) == (char)'e' && tolower(p[-2]) == (char)'e') /* is "eep" just before digits? */
    {
      p++;
      while(*p == (char)'0') /* skip trailing zeros */
        p++;
      if(isdigit(*p)) /* any digits?*/
        version = atoi(p);
      else if(p[-1] == (char)'0') /* skipped all zeros in version 0 ? */
        version = 0;
    }
  }
  return version;
}

/*---------------------------------------------------------------------------*/
/* External functions:                                                       */
/*---------------------------------------------------------------------------*/

uint8_t EEP_linearize(uint16_t *eep_orig, uint32_t static_size, uint32_t block_size)
{
  uint32_t i=static_size/2;  //point at end of linear part
  uint32_t k;
  uint32_t sector_length;
  uint32_t chksum;
  uint32_t calc_chksum=0;
  uint32_t offset;

  PARAM_NOT_USED(block_size);

  block_size -= 0x400; // Checksum is located 0x400 from the end of the block. Do not loop further.

  if (i & 1) // i % 4
  {
    i = i + (2 -(i & 1)); //32 bit allign
  }

  if (static_size > block_size-4) //if block full
  {
    return true; /* Nothing to linearize */
  }
  while (i<block_size/2) //until end of block
  {

    if (*(eep_orig+i) != 0xFFFF) //is sector length valid
    {
      sector_length = *(eep_orig+i);  //read sector length
      if (i + sector_length*2 > block_size/2)  //boundery check
      {
        return false;
      }

      offset = *(eep_orig+i+1); //read offset
      chksum = (*(eep_orig+i+(uint32_t)(sector_length*2)+2) << 16); //read checksum
      chksum += *(eep_orig+i+(uint32_t)(sector_length*2)+3);        //read checksum

      for (k=0;k<sector_length*2;k++)
      {
        calc_chksum += ((*(eep_orig+i+2+k) & 0xFF00) >> 8) + (*(eep_orig+i+2+k) & 0x00FF); //calc checksum
      }

      if (chksum == calc_chksum)
      {
        for (k=0;k<sector_length*2;k++)
        {
          *(eep_orig+offset/2+k) = *(eep_orig+i+2+k); //copy from patch to linear part
        }
      }
      i += sector_length*2 + 4; //update pointer to begining of patch
      calc_chksum=0;
    }
    else
    {
      i++; // if not valid patch, update pointer
    }
  }
  return true;
}

/*---------------------------------------------------------------------------*/

uint8_t EEP_linearize_bering(uint16_t *eep_orig, uint32_t static_size, uint32_t block_size)
{
  uint32_t i=static_size/2;  //point at end of linear part
  uint32_t k;
  uint32_t sector_length;
  uint16_t chksum;
  uint16_t calc_chksum=0;
  uint32_t offset;

  if (i & 1) // i % 2
  {
    i = i + (2 -(i & 1)); //32 bit allign
  }

  if (static_size > block_size-4) //if block full
  {
    return true; /* Nothing to linearize */
  }
  while (i<block_size/2-1) //until end of block - checksum
  {

    if (*(eep_orig+i) != 0xFFFF) //is sector length valid
    {
      sector_length = *(eep_orig+i);  //read sector length BYTES
      if (i + sector_length/2 > block_size/2)  //boundery check
      {
        return false;
      }

      offset = *(eep_orig+i+1); //read offset
      chksum = *(eep_orig+i+(sector_length/2)+2); //read checksum
      for (k=0;k<(sector_length/2);k++)
      {
        calc_chksum += ((*(eep_orig+i+2+k) & 0xff00)>>8)+ (*(eep_orig+i+2+k) & 0x00ff); //calc checksum

      }

      if (chksum == calc_chksum)
      {
        for (k=0;k<(sector_length/2);k++)
        {
          *(eep_orig+offset/2+k) = *(eep_orig+i+2+k); //copy from patch to linear part
        }
      }
      i += (sector_length/2) + 3; //update pointer to begining of patch
      calc_chksum=0;
    }
    else
    {
      i++; // if not valid patch, update pointer
    }
  }
  return true;
}

/*---------------------------------------------------------------------------*/

void EEP_Init(void) /* Do initialization here */
{
  AllList = SList_new();
  if (AllList)
  {
    AllList->OnDelete = OnAllListItemDelete; /* Make sure we automatically delete the objects attached to AllList when we clear the list */
  }
  CurrCfgIsLoaded = false;
  NewCfgIsLoaded  = false;
  /*BepIsLoaded     = false;*/
  DepIsLoaded     = false;
}
/*---------------------------------------------------------------------------*/
void EEP_FreeAllMem(void)
{
  SList_delete(AllList);
  AllList = NULL;
}
/*---------------------------------------------------------------------------*/
void EEP_Load_CurrCFG_file(char *CurrCFG_filename)
{
  TAllData     *AllData;
  char         TempParamName[MAX_LINE_LENGTH+1];
  char         TempDataType[10];
  unsigned int TempAddr;
  FILE *in;
  uint8_t done = false;
  char tempLine[MAX_LINE_LENGTH+1];

  NewCfgIsLoaded = false;
  /*BepIsLoaded    = false;*/
  DepIsLoaded    = false;

  SList_Clear(AllList);

  phEepRev = ExtractVersion(CurrCFG_filename);

  in = FILE_open(CurrCFG_filename, FILE_open_mode_read);
  if(in)
  {
    FILE_ReadLine(in, tempLine, MAX_LINE_LENGTH); /* skip first two lines */
    FILE_ReadLine(in, tempLine, MAX_LINE_LENGTH);

    /* Read lines... */
    while(!done)
    {
      FILE_ReadLine(in, tempLine, MAX_LINE_LENGTH);
      /* Separate the data elements in the line */
      if(sscanf(tempLine,"%x %8s %s",&TempAddr, TempDataType, TempParamName) == 3)
      {
        if(strstr(TempParamName,"eep_static") != NULL)
        {
          /* Create a new object of type "AllData" */
          AllData = (TAllData*)MEM_malloc(sizeof(TAllData));
          if(AllData)
          {
            /* Fill in the object fields */
            AllData->CurrAddr       = TempAddr;
            AllData->NewAddr        = EEP_NO_ADDRESS;
            AllData->DataType       = GetDataType(TempDataType);
            AllData->UseForUpdate   = false;
            AllData->CalValNotNewer = false;

            /* Add it all to the AllList */
            SList_AddObject(TempParamName, AllData, AllList);
          }
        }
      }
      else
        done = true;
    }
    FILE_close(in);
  }
  SList_Sort(AllList); /* sort the list so we can lookup strings using binary search */
  CurrCfgIsLoaded = true;
}
/*---------------------------------------------------------------------------*/

void EEP_Load_NewCFG_file(char *NewCFG_filename)
{
  TAllData     *AllData, *data;
  /*  int          item = 0;*/
  char         TempParamName[MAX_LINE_LENGTH+1];
  char         TempDataType[10];
  unsigned int TempAddr;
  char tempLine[MAX_LINE_LENGTH+1];
  FILE *in;
  uint8_t done = false;

  int CurrIndex;

  if(!CurrCfgIsLoaded)
    SendDebugText("CurrCFG file is not loaded");
  else if(NewCfgIsLoaded)
    SendDebugText("NewCFG file is already loaded");
  else
  {
    in = FILE_open(NewCFG_filename, FILE_open_mode_read);
    if(in)
    {
      NewCfgIsLoaded = true;
      FILE_ReadLine(in, tempLine, MAX_LINE_LENGTH); /* skip first two lines */
      FILE_ReadLine(in, tempLine, MAX_LINE_LENGTH);

      /* Read lines... */
      while(!done)
      {
        FILE_ReadLine(in, tempLine, MAX_LINE_LENGTH);
        /* Separate the data elements in the line */
        if(sscanf(tempLine,"%x %8s %s",&TempAddr, TempDataType, TempParamName) == 3)
        {
          if(strstr(TempParamName,"eep_static") != NULL)
          {
            CurrIndex = SList_IndexOf(TempParamName, AllList); /* Check if the param name is in AllList */
            if(CurrIndex >= 0 && AllList != NULL) /* Parameter found in list.? */
            {
			  data = SList_Object(CurrIndex, AllList);
              if (data)
                data->NewAddr = TempAddr;
            }
            else
            {
              AllData = (TAllData*)MEM_malloc(sizeof(TAllData)); /* Parameter not found in list => Create it. */
              if(AllData)
              {
                AllData->CurrAddr       = EEP_NO_ADDRESS; /* Fill in the object fields */
                AllData->NewAddr        = TempAddr;
                AllData->DataType       = GetDataType(TempDataType);
                AllData->UseForUpdate   = false;
                AllData->CalValNotNewer = false;

                SList_AddObject(TempParamName, AllData, AllList);  // Add it all to the AllList
                /*              sprintf(TempParamName, "item %i not found",item);
                SendDebugText(TempParamName);*/
              }
            }
          }
          /*          item++;*/
        }
        else
          done = true;
      }
      FILE_close(in);
      SList_Sort(AllList); /* sort the list so we can lookup strings using binary search */
    }
  }
}

/*---------------------------------------------------------------------------*/

void EEP_Load_DEP_file(char *DEP_filename)
{
  TAllData* data;
  int  CurrIndex;
  char TempParamName[MAX_LINE_LENGTH+1];
  int  TempValue;
  char TempAddr[80];
  char itemType;
  int  itemVer;
  FILE *in;
  uint8_t done = false;
  char tempLine[MAX_LINE_LENGTH+1];


  if(!CurrCfgIsLoaded)
    SendDebugText("CurrCFG file is not loaded");
  else  if(!NewCfgIsLoaded)
    SendDebugText("NewCFG file is not loaded");
  else if(DepIsLoaded)
    SendDebugText("DEP file is already loaded");
  else
  {
    in = FILE_open(DEP_filename, FILE_open_mode_read);
    if(in)
    {
      DepIsLoaded = true;

      /* Read lines... */
      while(!done)
      {
        FILE_ReadLine(in, tempLine, MAX_LINE_LENGTH);
        if(sscanf(tempLine,"%80s %150s %d",TempAddr, TempParamName,&TempValue) == 3)
        {
          CurrIndex = SList_IndexOf(TempParamName, AllList);
		  data = (CurrIndex < 0) ? NULL : SList_Object(CurrIndex, AllList);
          if(data) /* found ?*/
          {
            data->NewValue = TempValue;
            itemType = TempAddr[0]; /* Get the TYPE and VER info from the first parameter on each line */
            TempAddr[0] = '0';
            if(itemType > (char)'9')
            {
              char *p = &TempAddr[1];
              while(*p && *p == (char)'0')
                p++;
              if(!isdigit(*p) && p[-1] == (char)'0') /* skipped to many zeros? */
                p--;
              itemVer  = atoi(p);
            }
            else
              itemVer = 0;
            if(!((itemType == (char)'C')  &&  (itemVer <= phEepRev)))
            {
                data->CalValNotNewer = false;
            }
            else
            {
                data->CalValNotNewer = true;
            }

            if(data->CalValNotNewer)
            {
              /*SendDebugText("Cal. param. not newer that current (Not updated): ["+AnsiString(TempAddr)+"]  "+(AnsiString)TempParamName);*/
            }
            else
            {
              data->UseForUpdate = true;
            }
          }
          else
          {
            /*     SendDebugText("ERROR: Unknown parameter name: "+(AnsiString)TempParamName);*/
          }
        }
        else
          done = true;
      }
      FILE_close(in);
    }
  }
}

/*---------------------------------------------------------------------------*/

void EEP_CnvToBin(uint8_t *BinPtr, uint32_t BinSize)
{
  int i;
  int Value;
  TAllData *all;

  for(i = 0 ; i < AllList->Count ; i++) /* Run thru the list and fill in the bin memory */
  {
    all = ALL_LIST(i);
    if(all && all->NewAddr <= BinSize)
    {
      if(all->UseForUpdate)
      {
        Value = all->NewValue;
      }
      else
      {
        Value = all->CurrValue;
      }
      switch(all->DataType)
      {
        case dtU8:  *((uint8_t  *)(BinPtr + all->NewAddr)) = (uint8_t )Value; break;
        case dtS8:  *((int8_t  *)(BinPtr + all->NewAddr)) = (int8_t )Value; break;
        case dtU16: *((uint16_t *)(BinPtr + all->NewAddr)) = (uint16_t)Value; break;
        case dtS16: *((int16_t *)(BinPtr + all->NewAddr)) = (int16_t)Value; break;
        case dtU32: *((uint32_t *)(BinPtr + all->NewAddr)) = (uint32_t)Value; break;
        case dtS32: *((int32_t *)(BinPtr + all->NewAddr)) = (int32_t)Value; break;
      }
    }
    else
    {
      SendDebugText("ERROR: Address exceeds bin size limit ");
    }
  }
}

/*---------------------------------------------------------------------------*/

void EEP_CnvFromBin(uint8_t *BinPtr, uint32_t BinSize)
{
  int i;
  int Value = 0;
  TAllData *all;

  PARAM_NOT_USED(BinSize);

  for(i = 0; i < AllList->Count ; i++) /* Prepare the list */
  {
    all = ALL_LIST(i);
    if(all)
    {
      if(all->CurrAddr != EEP_NO_ADDRESS)
      {
        switch(all->DataType)
        {
          case dtU8:  Value = *((uint8_t  *)(BinPtr + all->CurrAddr)); break;
          case dtS8:  Value = *((int8_t  *)(BinPtr + all->CurrAddr)); break;
          case dtU16: Value = *((uint16_t *)(BinPtr + all->CurrAddr)); break;
          case dtS16: Value = *((int16_t *)(BinPtr + all->CurrAddr)); break;
          case dtU32: Value = *((uint32_t *)(BinPtr + all->CurrAddr)); break;
          case dtS32: Value = *((int32_t *)(BinPtr + all->CurrAddr)); break;
        }
      }
      all->CurrValue = Value;
    }
  }
}

/*---------------------------------------------------------------------------*/

