#-------------------------------------------------------------------------------
#
#  $Id: pi.trg,v 1.1 2008/03/06 20:36:34 rfiler Exp $
#
#  Name:
#    PI.TRG
#
#  Description:
#    Target file used to build the pltfm\pi Bandit package
#    for the PrairieComm (ARM) ASIC and DSP (OAK).
#
#  Copyright (c) 2008 by Sierra Wireless, Incorporated. All Rights Reserved.
#-------------------------------------------------------------------------------

# If PI_TARGET is not already defined, we are being called from 
# master makefile, so we need to define it relative to the build root
!IFNDEF PI_TARGET
PI_TARGET = $(ROOT)\pltfm\pi\objarm
!ENDIF

# This is a list of the files we are ultimately trying to build here
# in this case its a single obj file, rules for making this in pi.mak
PI_RESULT = \
                $(PI_TARGET)\pi.a

# List of all objects to go into the library file
PI_OBJECTS = \
                $(PI_TARGET)\pi.o

# List of all c files to generate dependancy list for
PI_C_OBJS = \
                $(PI_TARGET)\pi.o

# List of all assembler files to generate dependancy list for and
# assemble into objects
PI_INTERMEDIATES =

