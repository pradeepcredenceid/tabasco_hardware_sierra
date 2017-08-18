#-------------------------------------------------------------------------------
#
#  $Id: sl.trg,v 1.1 2008/03/06 20:39:20 rfiler Exp $
#
#  Name:
#    sl.trg
#
#  Description:
#    Target file used to build the pkgs\sl package
#
#  Copyright (c) 2008 by Sierra Wireless, Incorporated. All Rights Reserved.
#-------------------------------------------------------------------------------
   
# If SL_TARGET is not already defined, we are being called from 
# master makefile, so we need to define it relative to the build root
!IFNDEF SL_TARGET
SL_TARGET = $(ROOT)\pltfm\sl\objarm
!ENDIF

# This is a list of the files we are ultimately trying to build here
# in this case its a single obj file, rules for making this in sl.mak
SL_RESULT = \
                $(SL_TARGET)\sl.a

# List of all objects to go into the library file
SL_OBJECTS = \
                $(SL_TARGET)\slmanage.o \
		    $(SL_TARGET)\slvsprintf.o \
		    $(SL_TARGET)\slsscanf.o \
                $(SL_TARGET)\slcommon.o
# List of all c files to generate dependancy list for
SL_C_OBJS = \
                $(SL_TARGET)\slmanage.o \
		    $(SL_TARGET)\slvsprintf.o \
		    $(SL_TARGET)\slsscanf.o \
                $(SL_TARGET)\slcommon.o

# List of all assembler files to generate dependancy list for and
# assemble into objects
SL_INTERMEDIATES =

#
#  $Log: sl.trg,v $
#  Revision 1.1  2008/03/06 20:39:20  rfiler
#  Initial check in
#
#
#
