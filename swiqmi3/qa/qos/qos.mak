#-------------------------------------------------------------------------------
#
#  Name:
#    qos.mak
#
#  Description:
#    Makefile to build the pkgs/qa/qos package 
#
#   The following make targets are available in this makefile:
#
#     all           - make .o and .a image files (default)
#                     Test programs are also built when present
#     clean         - delete object directory and image files
#
#   The above targets can be made with the following command:
#
# Copyright (c) 2010-13 by Sierra Wireless, Incorporated.  All Rights Reserved.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Local Settings
#-------------------------------------------------------------------------------
SRCS_QOS =	qos/src/qaGobiApiQos.c \
			qos/src/qaQosGetNWStatus.c \
			qos/src/qaQosCommon.c \
			qos/src/qaQosReq.c \
			qos/src/qaQosRel.c \
			qos/src/qaQosGetGranted.c \
			qos/src/qaQosGetFlowStatus.c \
			qos/src/qaQosSuspend.c \
			qos/src/qaQosResume.c \
			qos/src/qaQosReset.c \
			qos/src/qaQosGetNWProf.c

#-------------------------------------------------------------------------------
# Split the object files into their respective groups
#-------------------------------------------------------------------------------
QOSOBJ =	$(OBJSDIR)/qaGobiApiQos.o \
			$(OBJSDIR)/qaQosGetNWStatus.o \
			$(OBJSDIR)/qaQosCommon.o \
			$(OBJSDIR)/qaQosReq.o \
			$(OBJSDIR)/qaQosRel.o \
			$(OBJSDIR)/qaQosGetGranted.o \
			$(OBJSDIR)/qaQosGetFlowStatus.o \
			$(OBJSDIR)/qaQosSuspend.o \
			$(OBJSDIR)/qaQosResume.o \
			$(OBJSDIR)/qaQosReset.o \
			$(OBJSDIR)/qaQosGetNWProf.o

#DBGFLAGS = -D QOS_SIMULATE

