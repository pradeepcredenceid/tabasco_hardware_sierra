#-------------------------------------------------------------------------------
#
#  $Id: ui.mak,v 1.1 2009/01/27 21:38:24 blee Exp $
#
#  Name:
#    ui.mak
#
#  Description:
#    Makefile to build the pkgs/ui package 
#
#   The following make targets are available in this makefile:
#
#     all           - make .o and .a image files (default)
#                     Test programs are also built when present
#     clean         - delete object directory and image files
#     uitest        - builds the test program if available
#
#   The above targets can be made with the following command:
#
#     make -f ui.mak [CPU={i386, arm9}] [target]
#       defaults to i386 if CPU is omitted
#
# Copyright (c) 2008 by Sierra Wireless, Incorporated.  All Rights Reserved.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Local Settings-
#
#-------------------------------------------------------------------------------
PKG  = ui
SRCS = uitask_sdk.c uitables.c uicmd.c uidl.c uiutil.c
OBJS = $(OBJSDIR)/uitask_sdk.o $(OBJSDIR)/uitables.o $(OBJSDIR)/uicmd.o \
	$(OBJSDIR)/uidl.o

HDRS = uiidefs.h uiudefs.h uiiproto.h uiuproto.h

APIOBJ = 
COREOBJ = 
SDKOBJ = $(OBJSDIR)/uitask_sdk.o $(OBJSDIR)/uitables.o $(OBJSDIR)/uicmd.o \
	$(OBJSDIR)/uidl.o $(OBJSDIR)/uiutil.o

# LIB and OBJ files required by $(PKG) test program
TESTLIBS = $(APILIB) $(CMBASICLIB) $(CORELIB) $(SDKLIB) $(CORELIB) $(SDKLIB)

#-------------------------------------------------------------------------------
# Common Rules & Settings
#-------------------------------------------------------------------------------
include ../gen.mak

#
# $Log: ui.mak,v $
