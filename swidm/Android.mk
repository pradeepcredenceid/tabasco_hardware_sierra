#
# This source code is "Not a Contribution" under Apache license
#
# Sierra Wireless DM Log function
#
# Copyright (C) 2011 Sierra Wireless, Inc.
#

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:=     \
    swidm_main.c      \
    swiserial.c

LOCAL_CFLAGS:= -g -O0

LOCAL_C_INCLUDES := $(KERNEL_HEADERS) \
    $(TOP)/hardware/sierra/swims \
    $(TOP)/hardware/sierra/swicommon

ifeq ($(TARGET_PRODUCT),hikey)
    LOCAL_C_INCLUDES :=  $(filter-out $(KERNEL_HEADERS), $(LOCAL_C_INCLUDES))
endif

LOCAL_SHARED_LIBRARIES := \
    libcutils libswims

# Disable prelink, or add to build/core/prelink-linux-arm.map
LOCAL_PRELINK_MODULE := false

LOCAL_MODULE_TAGS := optional

# Build executable
LOCAL_MODULE:= SierraDMLog
include $(BUILD_EXECUTABLE)
