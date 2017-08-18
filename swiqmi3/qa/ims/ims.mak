#-------------------------------------------------------------------------------
#
#  Name:
#    ims.mak
#
#  Description:
#    Makefile to build the pkgs/qa/ims package
#
#   The following make targets are available in this makefile:
#
#     all           - make .o and .a image files (default)
#                     Test programs are also built when present
#     clean         - delete object directory and image files
#
#   The above targets can be made with the following command:
#
# Copyright (c) 2013 by Sierra Wireless, Incorporated.  All Rights Reserved.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Local Settings
#-------------------------------------------------------------------------------
SRCS_IMS = ims/src/qaGobiApiIms.c \
           ims/src/qaImsSLQSSetSIPConfig.c \
           ims/src/qaImsSLQSSetRegMgrConfig.c \
           ims/src/qaImsSLQSSetIMSSMSConfig.c \
           ims/src/qaImsSLQSSetIMSUserConfig.c \
           ims/src/qaImsSLQSSetIMSVoIPConfig.c \
           ims/src/qaImsSLQSGetSIPConfig.c \
           ims/src/qaImsSLQSGetRegMgrConfig.c \
           ims/src/qaImsSLQSGetIMSSMSConfig.c \
           ims/src/qaImsSLQSGetIMSUserConfig.c \
           ims/src/qaImsSLQSGetIMSVoIPConfig.c \
           ims/src/qaImsSLQSImsConfigIndRegister.c
#-------------------------------------------------------------------------------
# Split the object files into their respective groups
#-------------------------------------------------------------------------------
IMSOBJ = $(OBJSDIR)/qaGobiApiIms.o \
         $(OBJSDIR)/qaImsSLQSSetSIPConfig.o \
         $(OBJSDIR)/qaImsSLQSSetRegMgrConfig.o \
         $(OBJSDIR)/qaImsSLQSSetIMSSMSConfig.o \
         $(OBJSDIR)/qaImsSLQSSetIMSUserConfig.o \
         $(OBJSDIR)/qaImsSLQSSetIMSVoIPConfig.o \
         $(OBJSDIR)/qaImsSLQSGetSIPConfig.o \
         $(OBJSDIR)/qaImsSLQSGetRegMgrConfig.o \
         $(OBJSDIR)/qaImsSLQSGetIMSSMSConfig.o \
         $(OBJSDIR)/qaImsSLQSGetIMSUserConfig.o \
         $(OBJSDIR)/qaImsSLQSGetIMSVoIPConfig.o \
         $(OBJSDIR)/qaImsSLQSImsConfigIndRegister.o