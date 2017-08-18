/*  -------------------------------------------------------------------------
    Copyright (C) 2014 Intel Mobile Communications GmbH

         Sec Class: Intel Confidential (IC)

    All rights reserved.
    -------------------------------------------------------------------------
    This document contains proprietary information belonging to IMC.
    Passing on and copying of this document, use and communication of its
    contents is not permitted without prior written authorization.
    -------------------------------------------------------------------------
    Revision Information:
       $File name:  /msw_tools/FlashTool/libFlsTool/inc/memtest.h $
       $CC-Version: .../oint_tools_flashtoole2_01/1 $
       $Date:       2014-03-13    15:39:22 UTC $
    ------------------------------------------------------------------------- */
#ifndef __MEMTEST_H__
#define __MEMTEST_H__

#ifndef FLASHTOOL_E2
  //#define USE_TEXT_CLASS            //< Adds the strings (below)
  //#define MEMORY_STABILITY_TESTS      //< Adds a lot of more testing
  #include "IMC_bootcore_strings.h"   //< Creating the test enums
#endif

// The TestError packet structure.
// This packet always follows a "PSI_NACK" return code
typedef struct
{
  uint32_t failed_test;             //< IMC_bootloader_strings
  uint32_t failed_address;
  uint32_t expected_value;
  uint32_t read_value;
} memtest_error_t;

typedef uint32_t(*testfunction_t)(volatile uint32_t *base_address, uint32_t num_bytes, memtest_error_t *p_result);

uint32_t logic_memtest_device_present(volatile uint32_t * address,  uint32_t nbytes, memtest_error_t *result);
uint32_t logic_memtest_databus(volatile uint32_t * address,  uint32_t nbytes, memtest_error_t *result);
uint32_t logic_memtest_addressbus(volatile uint32_t * base, uint32_t nBytes,  memtest_error_t *result);
uint32_t logic_memtest_accesswidth(volatile uint32_t * baseAddress, uint32_t nBytes,  memtest_error_t *result);
uint32_t logic_memtest_soldertest(uint32_t base, uint32_t length,  memtest_error_t *result);

#ifdef MEMORY_STABILITY_TESTS
  typedef void(*printf_t)(char *format, ...);
  #define MEMTEST_LOG_CHATTY  3
  #define MEMTEST_LOG_SUMMARY 2
  #define MEMTEST_LOG_RESULT  1
  #define MEMTEST_LOG_SILENT  0

  void logic_memtest_setprintf_func(printf_t func);

  uint32_t logic_memtest_solid_ones_and_zeros(volatile uint32_t* addr, uint32_t nbytes, memtest_error_t *result);
  uint32_t logic_memtest_solid_ones_and_zeros_16bit(volatile uint32_t* addr, uint32_t nbytes, memtest_error_t *result);
  uint32_t logic_memtest_alternating_checkerboard(volatile uint32_t* addr, uint32_t nbytes, memtest_error_t *result);
  uint32_t logic_memtest_alternating_singlebit(volatile uint32_t* addr, uint32_t nbytes, memtest_error_t *result);
  uint32_t logic_memtest_random_fill(volatile uint32_t* addr, uint32_t nbytes, memtest_error_t *result);
  uint32_t logic_memtest_rmw_and(volatile uint32_t* addr, uint32_t nbytes, memtest_error_t *result);
  uint32_t logic_memtest_rmw_xor(volatile uint32_t* addr, uint32_t nbytes, memtest_error_t *result);
  uint32_t logic_memtest_rmw_or(volatile uint32_t* addr, uint32_t nbytes, memtest_error_t *result);
  uint32_t logic_memtest_stabilitytest(uint32_t base, uint32_t length,  uint32_t nr_iterations, uint32_t verbosity, memtest_error_t *result);
#endif

#endif

