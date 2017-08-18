#-------------------------------------------------------------------------------
#
#  Name:
#    imsa.mak
#
#  Description:
#    Makefile to build the pkgs/qa/imsa package
#
#   The following make targets are available in this makefile:
#
#     all           - make .o and .a image files (default)
#                     Test programs are also built when present
#     clean         - delete object directory and image files
#
#   The above targets can be made with the following command:
#
# Copyright (c) 2014 by Sierra Wireless, Incorporated.  All Rights Reserved.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Local Settings
#-------------------------------------------------------------------------------
SRCS_IMSA = imsa/src/qaGobiApiImsa.c \
            imsa/src/qaImsaSLQSIndRegister.c
#-------------------------------------------------------------------------------
# Split the object files into their respective groups
#-------------------------------------------------------------------------------
IMSAOBJ = $(OBJSDIR)/qaGobiApiImsa.o \
          $(OBJSDIR)/qaImsaSLQSIndRegister.o
