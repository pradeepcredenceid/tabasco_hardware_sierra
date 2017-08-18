#-------------------------------------------------------------------------------
#
#  $Id: pkgs.mak,v 1.16 2009/11/03 19:34:47 rfiler Exp $
#
#  Name:
#    pkgs.mak
#
#  Description:
#    Top level Makefile to build the pkgs 
#
#   The following make targets are available in this makefile:
#
#     all           - make .o and .a image files (default)and test program
#                     where available
#     clean         - delete object directory and image files
#     $(PKG)        - builds the selected package
#
#   The above targets can be made with the following command:
#
#     make -f pkgs.mak [CPU={i386, arm9}] [target] [TECHNOLOGY={ALL,CDMA,UMTS}]
#       defaults to i386 if CPU is omitted
#       defaults to UMTS if TECHNOLOGY is omitted
#
#    make -f pkgs.mak build
#       copies binaries, libraries and include files into area for Applications
#
# Copyright (c) 2008 by Sierra Wireless, Incorporated.  All Rights Reserved.
#-------------------------------------------------------------------------------
SHELL := /bin/bash
DOXY=/usr/bin/doxygen
#-------------------------------------------------------------------------------
# if CPU is not specified, default to i386 (needed for library deletion here)
# Could be i386 or arm9
#-------------------------------------------------------------------------------
ifeq ($(strip $(CPU)),)
  CPU = i386
endif

include pkgs.trg
#
# Need the following file for list of Sample Applications & Utilities
#
include ../Sample_Code/samples.trg
include ../Utilities/utilities.trg
#-------------------------------------------------------------------------------
# Default Target
#-------------------------------------------------------------------------------
all:
	@echo
	@echo  "#######################################"
	@echo  "###        BUILDING ALL TARGETS     ###"
	@echo  "#######################################"
	@echo
	# sleep is required for dependency to work since build
	# happens so fast, the time compare fails
	for i in $(SUBDIRS) ; do    \
               ( cd $$i ; $(MAKE) -f $$i.mak ; sleep 1) ;       \
	done
#
#-------------------------------------------------------------------------------
# Manually copy include files and sdk binary to distribution area 
# for application developers
#-------------------------------------------------------------------------------
.PHONY: build
build:  $(HDRS)
	@if [ -d ../build/include ] ; then echo "../build/include exists..." ;\
	else mkdir -p ../build/include ;\
	fi;
	@cp ./ap/SwiApi*.h ../build/include
	@cp ./ap/SwiData*.h ../build/include
	@cp ./ap/SwiR*.h ../build/include	
	@cp ./sl/slu*.h ../build/include	
#
# SwiIntCdma.h should not be included by Sample Apps. Need to clean up
# Right now SwiApiNotify.h includes it. 
#
	@if [ -d ../build/bin/$(CPU) ] ; then echo "../build/bin/$(CPU) exists..." ;\
	else mkdir -p ../build/bin/$(CPU);\
	fi;
	@cp ./ap/SwiIntCdma.h ../build/include
	@cp ./apdx/Swi*.h ../build/include
	@cp ./apui/Swi*.h ../build/include
	@cp ./apui/apui$(CPU) ../build/bin/$(CPU)/swiapui
	@cp ./sdk/sdk$(CPU) ../build/bin/$(CPU)/swisdk
	@echo "include files and sdk executable copied to ../build..."
#
#-------------------------------------------------------------------------------
# Manually build everything. Similar to all the following steps.
# - make -f pkgs.mak clean
# - make -f pkgs.mak
# - make -f pkgs.mak build
# - build all the Sample apps and Utilities
#-------------------------------------------------------------------------------
.PHONY: complete
complete:  
	$(MAKE) -f pkgs.mak clean;
	$(MAKE) -f pkgs.mak;
	$(MAKE) -f pkgs.mak build;

#
#       - Build all sample applications
#
	cd ../Sample_Code; \
	for i in $(SAMPLEDIRS); \
	do \
	  ( echo "Building sample app $$i for $(CPU)"; \
	  cd $$i; \
	  $(MAKE) TECHNOLOGY=ALL;\
	  cd ..; ) \
	done 
#
#       - Build all utilities
#
	cd ../Utilities; \
	for i in $(UTILITIESDIRS); \
	do \
	  ( echo "Building utilities $$i for $(CPU)"; \
	  cd $$i; \
	  $(MAKE) TECHNOLOGY=ALL; \
	  cd ..; ) \
	done 

#
#-------------------------------------------------------------------------------
# Build all sample applications
# Note: This assumes "make -f pkgs.mak" and "make -f pkgs.mak build" have 
#       been run. If not, consider using "make -f pkgs.mak complete" instead
#-------------------------------------------------------------------------------
#
.PHONY: samples
samples:  
	cd ../Sample_Code; \
	for i in $(SAMPLEDIRS); \
	do \
	  ( echo "Building sample app $$i for $(CPU)"; \
	  cd $$i; \
	  $(MAKE) TECHNOLOGY=ALL;\
	  cd ..; ) \
	done 
#
#-------------------------------------------------------------------------------
# Build all utilities
# Note: This assumes "make -f pkgs.mak" and "make -f pkgs.mak build" have 
#       been run. If not, consider using "make -f pkgs.mak complete" instead
#-------------------------------------------------------------------------------
#
.PHONY: utilities
utilities:  
	cd ../Utilities; \
	for i in $(UTILITIESDIRS); \
	do \
	  ( echo "Building utilities $$i for $(CPU)"; \
	  cd $$i; \
	  $(MAKE) TECHNOLOGY=ALL; \
	  cd ..; ) \
	done 
#
#-------------------------------------------------------------------------------
# Manually generate Doxygen files
#-------------------------------------------------------------------------------
.PHONY: doxygen
doxygen:  
#
# if usr/bin/doxygen does not exist, bail out
#
	@if [ -e $(DOXY) ] ;\
        then echo "$(DOXY) exists - good." ;\
        else echo  ;\
             echo ">>>>>ERROR!!! $(DOXY) does not exist, please install first.";\
             exit 2;\
        fi;
#
	( cd ap; doxygen ../../tools/scripts/Doxyfile; cd ..;)

#-------------------------------------------------------------------------------
# Clean up top level if run at the pkgs level. Otherwise keep the libraries.
#-------------------------------------------------------------------------------
.PHONY: clean
CURRENT_DIR = $(notdir $(PWD))
clean:
	@rm -f *~
ifeq ($(CURRENT_DIR),pkgs)
	@echo "Cleaning top level libraries..."
	@-rm ../build/include/*.h
	@-rm ../build/lib/$(CPU)/lib*.a
	@-rm ../build/bin/$(CPU)/*
endif
	@for i in $(SUBDIRS) ; do    \
              ( cd $$i ; $(MAKE) -f $$i.mak clean) ;  \
	done
#
#       - Clean Sample Applications exes
#
	cd ../Sample_Code; \
	for i in $(SAMPLEDIRS); \
    	do \
	  ( echo "(x86) Building sample app $$i"; \
	  cd $$i; \
	  $(MAKE) clean;\
	  cd ..; ) \
	done 
#
#       - Clean utilities exes
#
	@cd ../Utilities; \
	for i in $(UTILITIESDIRS); \
	do \
	  ( echo "(x86) Building utilities $$i"; \
	  cd $$i; \
	  $(MAKE) clean; \
	  cd ..; ) \
	done 

#
# $Log: pkgs.mak,v $
# Revision 1.16  2009/11/03 19:34:47  rfiler
# Bug 19444, changed this file to allow apui headers to be copied into
# build/include.
#
# Revision 1.15  2009/04/01 17:57:10  rfiler
# Removed creation of bin directory in Sample_Code path. This was made
# obsolete when all executables were placed in build/bin/<CPU> in a
# previous change.
#
# Revision 1.14  2009/03/05 00:43:45  blee
# Phase II. cp sludefs, sluproto to build/include
#
# Revision 1.13  2009/03/04 02:40:52  rfiler
# Replaced "make" calls with "$(MAKE)" as recommended in online Gnu Make
# user's guide, section 5.7
#
# Revision 1.12  2009/02/11 19:51:11  blee
# Added missing mkdir of ../build/bin/ - causing make to fail on fresh checkout
#
# Revision 1.11  2008/11/28 23:17:37  blee
# Clean up-do not copy SwiIntxxx.h files to build/include , except for SwiIntCdma.h for now. Added targets 'samples' and 'utilities'
#
# Revision 1.10  2008/11/27 19:46:03  blee
# Added targets 'complete' to build the world in development environment and 'doxygen' to generate doxygen files
#
# Revision 1.9  2008/11/21 01:46:56  epasheva
# Added copying of apdx Swi* include files to LinuxSDK/build/include directory
#
# Revision 1.8  2008/11/13 23:11:34  epasheva
# Added copy of sdk executable to build/bin directory
# Modified clean target accordingly
#
# Revision 1.7  2008/10/29 00:51:02  rfiler
# Replaced RAT with TECHNOLOGY
#
# Revision 1.6  2008/10/23 16:27:51  blee
# Added comments about command-line option for UMTS,CDMA or ALL libraries
#
# Revision 1.5  2008/09/04 23:58:14  blee
# Fixed typo, not cleaning .a files properly
#
# Revision 1.4  2008/08/28 22:44:30  blee
# Adjusted Sample_Code directory level
#
# Revision 1.3  2008/08/21 17:08:21  blee
# Clean .a files from the correct directories and added target build
#
# Revision 1.2  2008/05/27 23:46:24  blee
# Re-ordered build order of some packages, added clean at top level and pkg level
#
# Revision 1.1  2008/04/12 01:18:20  blee
# Initial check-in of top level Makefiles
#

