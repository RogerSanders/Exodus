#ifndef __MD1600IO_INTERFACE_H__
#define __MD1600IO_INTERFACE_H__
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"

void GetA10000DeviceInfo(IDeviceInfo& deviceInfo);
void GetA04000DeviceInfo(IDeviceInfo& deviceInfo);
void GetA06000DeviceInfo(IDeviceInfo& deviceInfo);
void GetA11100DeviceInfo(IDeviceInfo& deviceInfo);
void GetMDControl3DeviceInfo(IDeviceInfo& deviceInfo);
void GetMDControl6DeviceInfo(IDeviceInfo& deviceInfo);
void GetMDBusArbiterDeviceInfo(IDeviceInfo& deviceInfo);

#endif
