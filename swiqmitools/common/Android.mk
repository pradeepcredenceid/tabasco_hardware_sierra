#
# This source code is "Not a Contribution" under Apache license
#
# Sierra Wireless common QMI tool static library
#
# Copyright (C) 2012 Sierra Wireless, Inc.
#

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:=     \
	swiqmitool_common.c      

LOCAL_CFLAGS:= 

LOCAL_C_INCLUDES := $(KERNEL_HEADERS) \
    $(TOP)/hardware/sierra/swiqmi3/qm \
    $(TOP)/hardware/sierra/swiqmi3/qa \
    $(TOP)/hardware/sierra/swiqmi3/qa/inc \
    $(TOP)/hardware/sierra/swiqmi3/qa/audio/inc \
    $(TOP)/hardware/sierra/swiqmi3/qa/ims/inc \
    $(TOP)/hardware/sierra/swiqmi3/qa/imsa/inc \
    $(TOP)/hardware/sierra/swiqmi3/qa/loc/inc \
    $(TOP)/hardware/sierra/swiqmi3/qa/swiaudio/inc \
    $(TOP)/hardware/sierra/swiqmi3/qa/qos/inc \
    $(TOP)/hardware/sierra/swiqmi3/qa/dms/inc  \
    $(TOP)/hardware/sierra/swiqmi3/qa/wds/inc  \
    $(TOP)/hardware/sierra/swiqmi3/qa/nas/inc  \
    $(TOP)/hardware/sierra/swiqmi3/qa/sms/inc  \
    $(TOP)/hardware/sierra/swiqmi3/qa/fms/inc  \
    $(TOP)/hardware/sierra/swiqmi3/qa/pds/inc  \
    $(TOP)/hardware/sierra/swiqmi3/qa/cat/inc  \
    $(TOP)/hardware/sierra/swiqmi3/qa/rms/inc    \
    $(TOP)/hardware/sierra/swiqmi3/qa/dcs/inc    \
    $(TOP)/hardware/sierra/swiqmi3/qa/swioma/inc    \
    $(TOP)/hardware/sierra/swiqmi3/qa/omadm/inc    \
    $(TOP)/hardware/sierra/swiqmi3/qa/sar/inc    \
    $(TOP)/hardware/sierra/swiqmi3/qa/voice/inc    \
    $(TOP)/hardware/sierra/swiqmi3/qa/uim/inc    \
    $(TOP)/hardware/sierra/swiqmi3/qa/swi/inc    \
    $(TOP)/hardware/sierra/swiqmi3/qa/cbk/inc   \
    $(TOP)/hardware/sierra/swicommon  

ifeq ($(TARGET_PRODUCT),hikey)
    LOCAL_C_INCLUDES :=  $(filter-out $(KERNEL_HEADERS), $(LOCAL_C_INCLUDES))
endif    

LOCAL_SHARED_LIBRARIES := \
	libswiqmiapi

# Disable prelink, or add to build/core/prelink-linux-arm.map
LOCAL_PRELINK_MODULE := false

#build shared lib
LOCAL_MODULE:= swiqmitool_common
include $(BUILD_STATIC_LIBRARY)
