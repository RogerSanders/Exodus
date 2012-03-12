#ifndef __MEMORY_INTERFACE_H__
#define __MEMORY_INTERFACE_H__
#include "SystemInterface/SystemInterface.pkg"

void GetROMDeviceInfo(IDeviceInfo& deviceInfo);
void GetRAMDeviceInfo(IDeviceInfo& deviceInfo);
void GetSharedRAMDeviceInfo(IDeviceInfo& deviceInfo);
void GetTimedRAMDeviceInfo(IDeviceInfo& deviceInfo);
void GetManagedBufferIntDeviceInfo(IDeviceInfo& deviceInfo);


#endif
