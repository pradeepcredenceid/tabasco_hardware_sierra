#-------------------------------------------------------------------------------
#
#  $Id: am.mak,v 1.12 2009/01/27 21:21:19 blee Exp $
#
#  Name:
#    am.mak
#
#  Description:
#    Makefile to build the pkgs/am package 
#
#   The following make targets are available in this makefile:
#
#     all           - make .o and .a image files (default)
#                     Test programs are also built when present
#     clean         - delete object directory and image files
#     amtest        - builds the test program if available
#
#   The above targets can be made with the following command:
#
#     make -f am.mak [CPU={i386, arm9}] [target]
#       defaults to i386 if CPU is omitted
#
# Copyright (c) 2008 by Sierra Wireless, Incorporated.  All Rights Reserved.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Local Settings-
#
#-------------------------------------------------------------------------------
PKG  = am
SRCS = am.c amcns.c amipctask_sdk.c amadmin.c amdemux.c am_api.c \
	amui_api.c amui_sdk.c
OBJS = $(OBJSDIR)/am.o $(OBJSDIR)/amcns.o $(OBJSDIR)/amipctask_sdk.o \
       $(OBJSDIR)/amadmin.o $(OBJSDIR)/amdemux.o $(OBJSDIR)/am_api.o

HDRS = amidefs.h amudefs.h amiproto.h amuproto.h

APIOBJ = $(OBJSDIR)/am_api.o $(OBJSDIR)/amui_api.o
COREOBJ = $(OBJSDIR)/am.o $(OBJSDIR)/amadmin.o $(OBJSDIR)/amcns.o 
SDKOBJ = $(OBJSDIR)/amipctask_sdk.o $(OBJSDIR)/amui_sdk.o
DEMUXOBJ = $(OBJSDIR)/amdemux.o

# LIB and OBJ files required by $(PKG) test program
TESTLIBS = $(APILIB) $(APIDXLIB) $(CMBASICLIB) $(CORELIB) $(SDKLIB) $(CORELIB) \
    $(SDKLIB)

#-------------------------------------------------------------------------------
# Common Rules & Settings
#-------------------------------------------------------------------------------
include ../gen.mak

#
# $Log: am.mak,v $
