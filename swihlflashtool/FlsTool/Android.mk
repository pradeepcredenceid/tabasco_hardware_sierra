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
#    $File name:  /msw_tools/FlashTool/FlsTool/Android.mk $
#    $CC-Version: .../dhkristx_140314_v0/1 $
#    $Date:       2014-04-28    8:21:43 UTC $
# -------------------------------------------------------------------------

LOCAL_PATH  := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := FlsTool
LOCAL_MODULE_TAGS := optional

# Tool only works for 32 bit environment
LOCAL_MULTILIB := 32

LOCAL_SRC_FILES  := \
  src/main.cpp

LOCAL_C_INCLUDES := \
  bionic \
  $(call include-path-for, stlport) \
  $(call include-path-for, libusb) \
  $(call include-path-for, zlib) \
  $(LOCAL_PATH)/src \
  $(LOCAL_PATH)/../libFlsTool/src \
  $(LOCAL_PATH)/../libFlsTool/inc

LOCAL_C_INCLUDES += external/stlport/stlport 




LOCAL_SHARED_LIBRARIES := libc libz libcutils libstlport libstdc++
LOCAL_STATIC_LIBRARIES := libFlsTool

#LOCAL_FORCE_STATIC_EXECUTABLE := true
#LOCAL_STATIC_LIBRARIES := libc libz libcutils libFlsTool \
#                          libstlport_static libstdc++

LOCAL_CFLAGS     := -Werror -Wall
LOCAL_CPPFLAGS   := -fno-exceptions -fno-rtti -std=gnu++0x

include $(BUILD_EXECUTABLE)

