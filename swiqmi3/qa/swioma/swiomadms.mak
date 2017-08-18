#-------------------------------------------------------------------------------
#
#  Name:
#    swiomadms.mak
#
#  Description:
#    Makefile to build the pkgs/qa/swioma package
#
#   The following make targets are available in this makefile:
#
#     all           - make .o and .a image files (default)
#                     Test programs are also built when present
#     clean         - delete object directory and image files
#
#   The above targets can be made with the following command:
#
# Copyright (c) 2011 by Sierra Wireless, Incorporated.  All Rights Reserved.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Local Settings
#-------------------------------------------------------------------------------
SRCS_SWIOMADMS = swioma/src/qaGobiApiSwiOmadms.c \
                 swioma/src/qaGobiApiSwiOmadmsExt.c \
		 swioma/src/qaSwiOmaDmSessionStart.c \
		 swioma/src/qaSwiOmaDmSessionStartExt.c \
                 swioma/src/qaSwiOmaDmSessionCancel.c \
		 swioma/src/qaSwiOmaDmSessionCancelExt.c \
                 swioma/src/qaSwiOmaDmSessionGetInfo.c \
                 swioma/src/qaSwiOmaDmSelection.c \
		 swioma/src/qaSwiOmaDmSelectionExt.c \
                 swioma/src/qaSwiOmaDmGetSettings.c \
                 swioma/src/qaSwiOmaDmSetSettings.c \
		 swioma/src/qaSwiOmaDmSetSettingsExt.c \
                 swioma/src/qaSwiOmaDmSessionGetInfoExt.c

#-------------------------------------------------------------------------------
# Split the object files into their respective groups
#-------------------------------------------------------------------------------
SWIOMADMSOBJ   = $(OBJSDIR)/qaGobiApiSwiOmadms.o \
                 $(OBJSDIR)/qaGobiApiSwiOmadmsExt.o \
		 $(OBJSDIR)/qaSwiOmaDmSessionStart.o \
                 $(OBJSDIR)/qaSwiOmaDmSessionStartExt.o \
		 $(OBJSDIR)/qaSwiOmaDmSessionCancel.o \
                 $(OBJSDIR)/qaSwiOmaDmSessionCancelExt.o \
		 $(OBJSDIR)/qaSwiOmaDmSessionGetInfo.o \
                 $(OBJSDIR)/qaSwiOmaDmSelection.o \
		 $(OBJSDIR)/qaSwiOmaDmSelectionExt.o \
                 $(OBJSDIR)/qaSwiOmaDmGetSettings.o \
                 $(OBJSDIR)/qaSwiOmaDmSetSettings.o \
		 $(OBJSDIR)/qaSwiOmaDmSetSettingsExt.o \
                 $(OBJSDIR)/qaSwiOmaDmSessionGetInfoExt.o

