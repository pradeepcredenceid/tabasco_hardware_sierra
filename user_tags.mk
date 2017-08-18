#
# Sierra Wireless User Tags
#
# Add all Sierra specific modules to the list of modules grandfathered to use a
# user tag.  Technically, we are not supposed to add new modules to the list of
# grandfathered modules, but this is the easiest way to manage Sierra specific
# modules.  
#
#
# Copyright (C) 2011 Sierra Wireless, Inc.
#
# This file will be included by build/core/user_tags.mk, so assume the variable
# exists, and we are just adding to it.

PRODUCT_PACKAGES += \
	libswigpsqmi \
	libswigpsat \
	SierraImgMgr \
	SierraFwDl7xxx \
	libswims \
	libswiqmiapi \
	slqssdk \
	swiqmi_test \
	libsierra-ril \
	swirt_server \
	libswirt_ril \
	libswirt_server \
	swirt_client \
	SierraSARTool \
	SierraDMLog \
	libsierraat-ril \
	libsierrahl-ril \
	swihltrace \
	libDownloadTool \
	DownloadTool \
	FlsTool \
	HLDownload \
	swisdk \
	swifwdnld \
	libswisdkapi \
	com.sierra.logs \
	Atcmd \
	Hlfota \
    swiomadm \
	ash \
	rild
	
