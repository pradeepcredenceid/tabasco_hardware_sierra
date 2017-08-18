#-------------------------------------------------------------------------------
#
#  Name:
#    loc.mak
#
#  Description:
#    Makefile to build the pkgs/qa/loc package
#
#   The following make targets are available in this makefile:
#
#     all           - make .o and .a image files (default)
#                     Test programs are also built when present
#     clean         - delete object directory and image files
#
#   The above targets can be made with the following command:
#
# Copyright (c) 2010-2015 by Sierra Wireless, Incorporated.  All Rights Reserved.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Local Settings
#-------------------------------------------------------------------------------
SRCS_LOC  = loc/src/qaGobiApiLoc.c \
            loc/src/qaSlqsLocEventRegister.c \
            loc/src/qaSlqsLocExtPowerState.c \
            loc/src/qaSlqsLocStart.c \
            loc/src/qaSlqsLocStop.c \
            loc/src/qaSlqsLocSetOpMode.c \
            loc/src/qaSlqsLocDeleteAssistData.c \
            loc/src/qaSlqsLocInjectUTCTime.c \
            loc/src/qaSlqsLocGetServer.c \
            loc/src/qaSlqsLocSetServer.c \
            loc/src/qaSlqsLocGetOpMode.c \
            loc/src/qaSlqsLocGetFixCriteria.c \
            loc/src/SwiLocAutoStart.c \
            loc/src/qaSlqsLocInjectPosition.c \
            loc/src/qaSlqsLocSetCradleMountConfig.c \
            loc/src/qaSlqsLocInjectSensorData.c 

#-------------------------------------------------------------------------------
# Split the object files into their respective groups
#-------------------------------------------------------------------------------

LOCOBJ  = $(OBJSDIR)/qaGobiApiLoc.o \
          $(OBJSDIR)/qaSlqsLocEventRegister.o \
          $(OBJSDIR)/qaSlqsLocExtPowerState.o \
          $(OBJSDIR)/qaSlqsLocStart.o \
          $(OBJSDIR)/qaSlqsLocStop.o \
          $(OBJSDIR)/qaSlqsLocSetOpMode.o \
          $(OBJSDIR)/qaSlqsLocDeleteAssistData.o \
          $(OBJSDIR)/qaSlqsLocInjectUTCTime.o \
          $(OBJSDIR)/qaSlqsLocGetServer.o \
          $(OBJSDIR)/qaSlqsLocSetServer.o \
          $(OBJSDIR)/qaSlqsLocGetOpMode.o \
          $(OBJSDIR)/qaSlqsLocGetFixCriteria.o \
          $(OBJSDIR)/SwiLocAutoStart.o \
          $(OBJSDIR)/qaSlqsLocInjectPosition.o \
          $(OBJSDIR)/qaSlqsLocSetCradleMountConfig.o \
          $(OBJSDIR)/qaSlqsLocInjectSensorData.o 

