#-------------------------------------------------------------------------------
#
#  $Id: dd.mak,v 1.4 2008/08/21 17:09:09 blee Exp $
#
#  Name:
#    dd.mak
#
#  Description:
#    Makefile to build the pkgs/dd package 
#
#   The following make targets are available in this makefile:
#
#     all           - make .o and .a image files (default)
#                     Test programs are also built when present
#     clean         - delete object directory and image files
#     ddtest        - builds the test program if available
#
#   The above targets can be made with the following command:
#
#     make -f dd.mak [CPU={i386, arm9}] [target]
#       defaults to i386 if CPU is omitted
#
# Copyright (c) 2008 by Sierra Wireless, Incorporated.  All Rights Reserved.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Local Settings 
#-------------------------------------------------------------------------------
PKG = dd
SRCS = dd.c dd_sdk.c
OBJS = $(OBJSDIR)/dd.o $(OBJSDIR)/dd_sdk.o
HDRS = ddudefs.h dduproto.h ddidefs.h

COREOBJ = $(OBJSDIR)/dd.o
SDKOBJ = $(OBJSDIR)/dd_sdk.o

# LIB and OBJ files required by $(PKG) test program
TESTLIBS = $(CORELIB) $(SDKLIB) 

#-------------------------------------------------------------------------------
# Common Rules & Settings
#-------------------------------------------------------------------------------
include ../gen.mak

#
# $Log: dd.mak,v $
