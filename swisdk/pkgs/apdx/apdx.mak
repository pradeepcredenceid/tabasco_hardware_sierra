#-------------------------------------------------------------------------------
#
#  $Id: apdx.mak,v 1.6 2009/06/01 23:41:04 blee Exp $
#
#  Name:
#    apdx.mak
#
#  Description:
#    Makefile to build the demux application in pkgs/apdx package 
#
#   The following make targets are available in this makefile:
#
#     all           - make .o and .a image files (default)
#                     Test programs are also built when present
#     clean         - delete object directory and image files
#
#   The above targets can be made with the following command:
#
#     make -f apdx.mak [CPU={i386, arm9}] [target]
#       defaults to i386 if CPU is omitted

# Copyright (c) 2008 by Sierra Wireless, Incorporated.  All Rights Reserved.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Local Settings
#-------------------------------------------------------------------------------
PKG  = apdx
SRCS = SwiApiDemux.c
APDX_INCLUDE =  -I../ap -I../am -I../cn -I../ds -I../ic -I../os -I../sl \
                -I../us -I../ci

HDRS = apdxidefs.h apdxudefs.h apdxiproto.h apdxuproto.h
       

#-------------------------------------------------------------------------------
# Split the object files into their respective groups
#-------------------------------------------------------------------------------
DEMUXOBJ  = $(OBJSDIR)/SwiApiDemux.o 

# This is to make sure all .o 's are built
#
OBJS = $(DEMUXOBJ)

# LIB and OBJ files and additional flagsrequired by $(PKG) test program
#
TESTLIBS = $(DEMUXOBJ) $(APIDXLIB) $(CORELIB)  $(APILIB) 

#-------------------------------------------------------------------------------
# Common Rules & Settings
#-------------------------------------------------------------------------------
include ../gen.mak

#
# $Log: apdx.mak,v $
