#
# This source code is "Not a Contribution" under Apache license
#
# Sierra Wireless DM Log function
#
# Copyright (C) 2014 Sierra Wireless, Inc.
#

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:=     \
   tracelog.c 

LOCAL_CFLAGS:= -g -O0

# Disable prelink, or add to build/core/prelink-linux-arm.map
LOCAL_PRELINK_MODULE := false

LOCAL_MODULE_TAGS := optional

# Build executable
LOCAL_MODULE:= swihltrace
include $(BUILD_EXECUTABLE)
