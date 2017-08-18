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

# telephony.mk packages
PRODUCT_PACKAGES += \
	CarrierConfig \
	Dialer \
	CallLogBackup \
	CellBroadcastReceiver \
	EmergencyInfo \
	rild

PRODUCT_PROPERTY_OVERRIDES += \
	ro.carrier=unknown \
	persist.sierra.sim_ready_delay=15 \
	ro.sierra.voice=1 \
	ro.com.android.mobiledata=true \
	ro.telephony.default_network=10

PRODUCT_COPY_FILES += \
	frameworks/native/data/etc/android.hardware.telephony.cdma.xml:system/etc/permissions/android.hardware.telephony.cdma.xml \
	frameworks/native/data/etc/android.hardware.telephony.gsm.xml:system/etc/permissions/android.hardware.telephony.gsm.xml \
	hardware/sierra/etc/init.config.ip:system/bin/init.config.ip \
	hardware/sierra/etc/init.dhcpcd:system/bin/init.dhcpcd \
	hardware/sierra/etc/init.sierra.rc:root/init.sierra.rc
