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
#    $File name:  /msw_tools/FlashTool/DownloadTool/Android.mk $
#    $CC-Version: .../dhkristx_140314_v0/2 $
#    $Date:       2014-04-28    8:17:23 UTC $
# -------------------------------------------------------------------------

LOCAL_PATH  := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := DownloadTool
LOCAL_MODULE_TAGS := optional

# Tool only works for 32 bit environment
LOCAL_MULTILIB:= 32

LOCAL_SRC_FILES  := \
  src/IMCDownload.cpp \
  src/LibraryLoader.cpp \
  src/k8090.cpp \
  src/main.cpp \
  src/Phone.cpp \
  src/Utils.cpp

LOCAL_C_INCLUDES := \
  bionic \
  $(call include-path-for, stlport) \
  $(call include-path-for, libusb) \
  $(LOCAL_PATH)/src \
  $(LOCAL_PATH)/../libFlsTool/src \
  $(LOCAL_PATH)/../libFlsTool/inc \
  $(LOCAL_PATH)/../libDownloadTool/src


LOCAL_C_INCLUDES += external/stlport/stlport 


LOCAL_SHARED_LIBRARIES := libc libz libcutils liblog libstlport libstdc++
LOCAL_STATIC_LIBRARIES := libDownloadTool_static libFlsTool libusbhosthl 

#LOCAL_FORCE_STATIC_EXECUTABLE := true
#LOCAL_STATIC_LIBRARIES := libc libcutils libDownloadTool libFlsTool \
#                          libstlport_static libstdc++ \
#                          liblog libusbhost

LOCAL_CFLAGS     := -Wall -DSWI_ARM -DSIERRA -DSWI_ZLIB -DSIERRA_CTF
LOCAL_CPPFLAGS   := -fno-exceptions -fno-rtti -std=gnu++0x

LOCAL_PRELINK_MODULE := false


include $(BUILD_EXECUTABLE)

