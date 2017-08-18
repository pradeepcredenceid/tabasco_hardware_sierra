#-------------------------------------------------------------------------------
#
#  $Id: apui.mak,v 1.4 2009/06/01 23:21:57 blee Exp $
#
#  Name:
#    apui.mak
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
#     make -f apui.mak [CPU={i386, arm9}] [target]
#       defaults to i386 if CPU is omitted

# Copyright (c) 2009 by Sierra Wireless, Incorporated.  All Rights Reserved.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Local Settings
#-------------------------------------------------------------------------------
PKG  = apui
SRCS = SwiApiUi.c
APUI_INCLUDE =  -I../ap -I../am -I../cn -I../ds -I../ic -I../os -I../sl \
                -I../dl -I../us -I../ci
HDRS = apuiidefs.h apuiudefs.h apuiiproto.h apuiuproto.h
       

#-------------------------------------------------------------------------------
# Split the object files into their respective groups
#-------------------------------------------------------------------------------
APUIOBJ  = $(OBJSDIR)/SwiApiUi.o 

# This is to make sure all .o 's are built
#
OBJS = $(APUIOBJ)

# LIB and OBJ files and additional flags required by $(PKG) test program
# Note: Need to include CMBASICLIB because am_api.c calls SwiApSaveError,
#       which is in the ap package !
TESTLIBS =  $(APUIOBJ) $(APILIB) $(CORELIB) $(CMBASICLIB) 

#-------------------------------------------------------------------------------
# Common Rules & Settings
#-------------------------------------------------------------------------------
include ../gen.mak

#
# $Log: apui.mak,v $
