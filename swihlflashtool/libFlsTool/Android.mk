# -------------------------------------------------------------------------
# Copyright (C) 2013-2014 Intel Mobile Communications GmbH
#
#      Sec Class: Intel Confidential (IC)
#
# All rights reserved.
# -------------------------------------------------------------------------
# This document contains proprietary information belonging to IMC.
# Passing on and copying of this document, use and communication of its
# contents is not permitted without prior written authorization.
# -------------------------------------------------------------------------
# Revision Information:
#    $File name:  /msw_tools/FlashTool/libFlsTool/Android.mk $
#    $CC-Version: .../dhkristx_140314_v0/1 $
#    $Date:       2014-04-30    10:53:24 UTC $
# -------------------------------------------------------------------------

LOCAL_PATH := $(call my-dir)

# Uncomment to enable debug
#libfls_cflags += -DDEBUG

# Disable exceptions and realtime type identification
libfls_cppflags := -fno-exceptions -fno-rtti -std=gnu++0x

libfls_src_files := \
  src/Crypto.cpp \
  src/Convert.cpp \
  src/Condition.cpp \
  src/Exceptions.cpp \
  src/FileIO.cpp \
  src/FlsFile.cpp \
  src/FlsMetaFile.cpp \
  src/Fls2Parser.cpp \
  src/Fls3Parser.cpp \
  src/Fls3MetaParser.cpp \
  src/GenericFile.cpp \
  src/INI.cpp \
  src/JSON.cpp \
  src/Mutex.cpp \
  src/OS.cpp \
  src/Pkcs.c \
  src/PrgOptions.cpp \
  src/PrgHandler.cpp \
  src/Security.cpp \
  src/SerialPort.cpp \
  src/Sha_x.cpp \
  src/StringExt.cpp \
  src/StructParser.cpp \
  src/SysFs.cpp \
  src/Thread.cpp \
  src/Timer.cpp \
  src/Watchdog.cpp \
  src/WinRegistry.cpp \
  src/ZIP.cpp \
  src/c_sha1.c \
  src/rsa_tool.c \
  src/sha1.cpp \
  src/sha256.cpp \
  src/utilfunctions.c

libfls_c_includes := \
  $(TOP)/hardware/ril/libril/ \
  bionic \
  $(call include-path-for, stlport) \
  $(call include-path-for, libusb) \
  $(call include-path-for, zlib) \
  $(LOCAL_PATH)/src \
  $(LOCAL_PATH)/inc \
  $(LOCAL_PATH)/inc/strings
  

libfls_c_includes += external/stlport/stlport 
libfls_c_includes += external/zlib

libfls_cflags += \
  -D_REENTRANT -D_GNU_SOURCE \
  -DONLY_BASIC_TYPES \
  -DSWI_ARM \
  -DSIERRA \
  -DSIERRA_CTF \
  -DSWI_ZLIB


############ Build static library ############

include $(CLEAR_VARS)

LOCAL_MODULE      := libFlsTool
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES  := $(libfls_src_files)
LOCAL_C_INCLUDES := $(libfls_c_includes)
LOCAL_CFLAGS     := $(libfls_cflags)
LOCAL_CPPFLAGS   := $(libfls_cppflags)

include $(BUILD_STATIC_LIBRARY)

