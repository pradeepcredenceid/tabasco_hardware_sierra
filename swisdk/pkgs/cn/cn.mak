#-------------------------------------------------------------------------------
#
#  $Id: cn.mak,v 1.5 2008/08/21 17:09:08 blee Exp $
#
#  Name:
#    cn.mak
#
#  Description:
#    Makefile to build the pkgs/cn package 
#
#   The following make targets are available in this makefile:
#
#     all           - make .o and .a image files (default)
#                     Test programs are also built when present
#     clean         - delete object directory and image files
#     cntest        - builds the test program if available
#
#   The above targets can be made with the following command:
#
#     make -f cn.mak [CPU={i386, arm9}] [target]
#       defaults to i386 if CPU is omitted
#
# Copyright (c) 2008 by Sierra Wireless, Incorporated.  All Rights Reserved.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Local Settings- 
#-------------------------------------------------------------------------------
PKG  = cn
SRCS = cnrrtask_sdk.c cnpkg_api.c
OBJS = $(OBJSDIR)/cnrrtask_sdk.o $(OBJSDIR)/cnpkg_api.o

HDRS = cnidefs.h cnudefs.h cniproto.h cnuproto.h

APIOBJ = $(OBJSDIR)/cnpkg_api.o
SDKOBJ = $(OBJSDIR)/cnrrtask_sdk.o 

# LIB and OBJ files required by $(PKG) test program
TESTLIBS = $(APILIB) $(CORELIB) $(SDKLIB)

#-------------------------------------------------------------------------------
# Common Rules & Settings
#-------------------------------------------------------------------------------
include ../gen.mak

#
# $Log: cn.mak,v $
