/*
 *       Copyright (C) 2014 Intel Mobile Communications GmbH
 *
 *            Sec Class: Intel Confidential (IC)
 *
 *            All rights reserved.
 *
 *  -------------------------------------------------------------------------
 *   Revision Information:
 *      $File name:  /msw_tools/FlashTool/libFlsTool/src/Sha_x.cpp $
 *      $CC-Version: .../dhkristx_140314_v0/1 $
 *      $Date:       2014-04-16    12:18:57 UTC $
 *   ------------------------------------------------------------------------- */


#include "Sha_x.h"

//---------------------------------------------------------------------------


const unsigned char SHA1DER[15]=  {0x30,0x21,0x30,0x09,0x06,0x05,0x2b,0x0e,0x03,0x02,0x1a,0x05,0x00,0x04,0x14};
const unsigned char SHA256DER[19]={0x30,0x31,0x30,0x0d,0x06,0x09,0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x01,0x05,0x00,0x04,0x20};
unsigned long CurrentHashFunction=1;

SHA_X::SHA_X()
{
  crypto_sw_sha256_init(&SHA256);
}

SHA_X::~SHA_X()
{
  // The destructor does nothing
}

unsigned long SHA_X::GetDER(unsigned char** DER)
{
  unsigned long Size = 0;
  switch(CurrentHashFunction)
  {
    case 1:
      *DER = (unsigned char*)SHA1DER;
      Size = sizeof(SHA1DER);
      break;
    case 2:
      *DER = (unsigned char*)SHA256DER;
      Size = sizeof(SHA256DER);
      break;
    default:
      break;
  }
  return Size;
}


void SHA_X::Reset(unsigned long FunctionId)
{
  CurrentHashFunction=FunctionId;
  switch(CurrentHashFunction)
  {
    case 1:
      Sha1.Reset();
      break;
    case 2:
      crypto_sw_sha256_init(&SHA256);
      break;
    default:
      break;
  }
}

uint32_t SHA_X::ReverseLong(uint32_t In)
{
  uint8_t L[4];
  uint8_t* P;
  P=(uint8_t*)&In;
  L[0]=P[3];
  L[1]=P[2];
  L[2]=P[1];
  L[3]=P[0];
  return(*(uint32_t*)L);
}

bool SHA_X::Result(unsigned *message_digest_array)
{
  switch(CurrentHashFunction)
  {
    case 1:
      Sha1.Result(message_digest_array);
      break;
    case 2:
      crypto_sw_sha256_final(&SHA256,(uint8_t*) message_digest_array);
      uint32_t i;
      for(i=0;i<8;i++)message_digest_array[i]=ReverseLong(message_digest_array[i]);
      break;
    default:
      break;
  }
  return true;
}

void SHA_X::ScatterInput(
        const unsigned char *message_array,
        unsigned length,
        unsigned ScatterDistance)
{
}

void SHA_X::Input(
        const unsigned char *message_array,
        unsigned length)
{
  switch(CurrentHashFunction)
  {
    case 1:
      Sha1.Input(message_array,length);
      break;
    case 2:
      crypto_sw_sha256_update(&SHA256,(uint8_t*)message_array,length);
      break;
    default:
      break;
  }
}

void SHA_X::Input(
        const char *message_array,
        unsigned length)
{
  Input((unsigned char *) message_array, length);
}
