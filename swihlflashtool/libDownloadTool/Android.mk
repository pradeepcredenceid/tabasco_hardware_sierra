# -------------------------------------------------------------------------
# Copyright (C) 2012 Intel Mobile Communications GmbH
# All rights reserved.
# -------------------------------------------------------------------------
# This document contains proprietary information belonging to IMC.
# Passing on and copying of this document, use and communication of its
# contents is not permitted without prior written authorization.
# -------------------------------------------------------------------------
# Revision Information:
#    $File name:  /msw_tools/FlashTool/libIMCdownload/Android.mk $
#    $CC-Version: .../oint_tools_flashtoole2_01/1 $
#    $Date:       2012-01-17    10:04:39 UTC $
# -------------------------------------------------------------------------

LOCAL_PATH := $(call my-dir)

# Uncomment to enable debug
#libmodem_cflags := -DDEBUG

# Disable exceptions and realtime type identification
libmodem_cppflags := -fno-exceptions -fno-rtti -std=gnu++0x

# Add libusb support
libmodem_cflags    := -DLIBUSBHOST
libmodem_src_files := src/OS_dependent_code/comm_if/GenericUSB.c

libmodem_src_files += \
  src/OS_dependent_code/comm_if/devices.cpp \
  src/OS_dependent_code/comm_if/IFWD_comport.c \
  src/OS_dependent_code/comm_if/IFWD_emuport.c \
  src/OS_dependent_code/debug_os_if/DebugModule.c \
  src/OS_dependent_code/global_if/IFWD_global_func.c \
  src/OS_dependent_code/memory_os_if/IFWD_mem.c \
  src/OS_dependent_code/security_if/IFWD_sec.c \
  src/OS_dependent_code/security_if/IFWD_sec_channel.c \
  src/OS_dependent_code/security_if/crypto_sw_hash.c \
  src/OS_dependent_code/security_if/crypto_sw_hash_core.c \
  src/OS_dependent_code/timer_if/IFWD_timer.c \
  src/Portable_code/boot_handler/IFWD_boot.c \
  src/Portable_code/boot_handler/IFWD_boot_egold.c \
  src/Portable_code/boot_handler/IFWD_boot_sgold.c \
  src/Portable_code/boot_handler/IFWD_boot_test \
  src/Portable_code/comm_handler/IFWD_comm.c \
  src/Portable_code/multi_file_handler/IFWD_multi_file_handler.c \
  src/Portable_code/protocol_handler/IFWD_protocol_handler.c \
  src/Portable_code/protocol_handler/IFWD_target_error_handler.c \
  src/Portable_code/single_file_handler/EepDeltaMerge.c \
  src/Portable_code/single_file_handler/FlashInitRead.c \
  src/Portable_code/single_file_handler/IFWD_single_file_handler.c \
  src/Portable_code/single_file_handler/SList.c \
  src/Portable_code/text_func/IFWD_TextFunc.c \
  src/Portable_code/transport_handler/IFWD_transport_handler.c \
  src/IMC_DownloadDll_main.c 
  
libmodem_c_includes := \
  bionic \
  $(call include-path-for, stlport) \
  $(call include-path-for, libusb) \
  $(LOCAL_PATH)/../libFlsTool/src \
  $(LOCAL_PATH)/../libFlsTool/inc \
  $(LOCAL_PATH)/../libFlsTool/inc/strings \
  $(LOCAL_PATH)/../../../mhw_tools/signing_module/ \
  $(LOCAL_PATH)/src \
  $(LOCAL_PATH)/src/OS_dependent_code/comm_if \
  $(LOCAL_PATH)/src/OS_dependent_code/debug_os_if \
  $(LOCAL_PATH)/src/OS_dependent_code/global_if \
  $(LOCAL_PATH)/src/OS_dependent_code/memory_os_if \
  $(LOCAL_PATH)/src/OS_dependent_code/security_if \
  $(LOCAL_PATH)/src/OS_dependent_code/timer_if \
  $(LOCAL_PATH)/src/Portable_code/boot_handler \
  $(LOCAL_PATH)/src/Portable_code/boot_handler/boot \
  $(LOCAL_PATH)/src/Portable_code/boot_handler/ebl \
  $(LOCAL_PATH)/src/Portable_code/comm_handler \
  $(LOCAL_PATH)/src/Portable_code/multi_file_handler \
  $(LOCAL_PATH)/src/Portable_code/protocol_handler \
  $(LOCAL_PATH)/src/Portable_code/single_file_handler \
  $(LOCAL_PATH)/src/Portable_code/text_func \
  $(LOCAL_PATH)/src/Portable_code/transport_handler

libmodem_cflags += \
  -D_REENTRANT -D_GNU_SOURCE \
  -DDISABLE_ANIMATION \
  -DONLY_BASIC_TYPES -DNAND_PRESENT \
  -DFLASHTOOL_E2 -DUSE_TEXT_CLASS_IFWD_INFO \
  -DUSE_TEXT_CLASS_IFWD_ERRORS \
  -DSWI_ARM \
  -DSIERRA \
  -DSWI_ZLIB \
  -DSIERRA_CTF

libmodem_c_includes += external/stlport/stlport 
libmodem_c_includes += $(TOP)/hardware/ril/libril/

############ Build shared library ############

include $(CLEAR_VARS)
SDK_ANDROID_70 := 24

LOCAL_MODULE := libDownloadTool

LOCAL_MODULE_TAGS := optional

#LOCAL_COPY_HEADERS := msw_tools/FlashTool/libDownloadTool/src/DownloadTool.h
#LOCAL_SRC_FILES := lib/linux/libsigning_module.a
#include $(PREBUILT_STATIC_LIBRARY)

LOCAL_SRC_FILES  := $(libmodem_src_files)
LOCAL_C_INCLUDES := $(libmodem_c_includes)
LOCAL_CFLAGS     := $(libmodem_cflags)
LOCAL_CPPFLAGS   := $(libmodem_cppflags)

LOCAL_CLANG_CFLAGS += -Wno-error=tautological-pointer-compare \
                      -Wno-error=pointer-bool-conversion

#LOCAL_LDLIBS    := -L$(LOCAL_PATH)/lib/linux/ -lsigning_module
    
LOCAL_SHARED_LIBRARIES := libdl libc libm libcutils liblog libstlport libstdc++
LOCAL_STATIC_LIBRARIES := libFlsTool libusbhosthl


#LOCAL_SHARED_LIBRARIES := libdl
#LOCAL_STATIC_LIBRARIES := libc libm libcutils libFlsTool \
#                          libstlport_static liblog libstdc++

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)


############ Build static library ############

include $(CLEAR_VARS)
SDK_ANDROID_70 := 24
LOCAL_MODULE := libDownloadTool_static

LOCAL_SRC_FILES  := $(libmodem_src_files)
LOCAL_C_INCLUDES := $(libmodem_c_includes)
LOCAL_CFLAGS     := $(libmodem_cflags)
LOCAL_CPPFLAGS   := $(libmodem_cppflags)

LOCAL_CLANG_CFLAGS += -Wno-error=tautological-pointer-compare \
                      -Wno-error=pointer-bool-conversion

LOCAL_STATIC_LIBRARIES := libFlsTool libusbhosthl

include $(PREBUILT_STATIC_LIBRARY)
include $(BUILD_STATIC_LIBRARY)

