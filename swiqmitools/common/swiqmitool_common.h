/**
 *
 * @ingroup swiqmitool
 *
 * @file
 * common functions and data types for swi qmi tools
 *
 * @author
 * Copyright: 2015 Sierra Wireless, Inc.
 *            All rights reserved
 *
 */


#ifndef __SWI_QMI_TOOL_COMMON_H__
#define __SWI_QMI_TOOL_COMMON_H__


extern BOOL bootupdated;
void qmiDeviceConnect( void );
void qmiDeviceDisconnect( void );
unsigned short ril_piget16(unsigned char **packetpp);
unsigned long ril_piget32(unsigned char **packetpp);

#endif /* __SWI_QMI_TOOL_COMMON_H__ */

