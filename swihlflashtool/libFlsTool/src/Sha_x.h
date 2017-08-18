/*
 *       Copyright (C) 2014 Intel Mobile Communications GmbH
 *
 *            Sec Class: Intel Confidential (IC)
 *
 *            All rights reserved.
 *
 *  -------------------------------------------------------------------------
 *   Revision Information:
 *      $File name:  /msw_tools/FlashTool/libFlsTool/src/Sha_x.h $
 *      $CC-Version: .../dhkristx_140314_v0/1 $
 *      $Date:       2014-04-16    12:19:02 UTC $
 *   ------------------------------------------------------------------------- */

#ifndef _SHAX_H_
#define _SHAX_H_

#include "sha1.h"
#include "sha256.h"

class SHA_X
{
  public:
    SHA_X();
    virtual ~SHA_X();

    /*
    *	Get pointer to DER coding of algorithm (and length)
    */
    unsigned long GetDER(unsigned char** DER);

    /*
    * Re-initialize the class
    */
    void Reset(unsigned long FunctionId);

    /*
    * Returns the message digest
    */
    bool Result(unsigned *message_digest_array);

    /*
    * Provide input to SHA_X
    */
    void ScatterInput(
      const unsigned char *message_array,
      unsigned length,
      unsigned ScatterDistance);

    void Input(const unsigned char *message_array, unsigned length);
    void Input(const char *message_array, unsigned length);
    uint32_t ReverseLong(uint32_t In);

  private:
    SHA1 Sha1;
    T_SW_HASH_CONTEXT SHA256;
};

#endif
