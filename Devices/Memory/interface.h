#ifndef __MEMORY_INTERFACE_H__
#define __MEMORY_INTERFACE_H__
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"

void GetROM8DeviceInfo(IDeviceInfo& deviceInfo);
void GetROM16DeviceInfo(IDeviceInfo& deviceInfo);
void GetROM32DeviceInfo(IDeviceInfo& deviceInfo);
void GetROM8VariableDeviceInfo(IDeviceInfo& deviceInfo);
void GetROM16VariableDeviceInfo(IDeviceInfo& deviceInfo);
void GetROM32VariableDeviceInfo(IDeviceInfo& deviceInfo);
void GetRAM8DeviceInfo(IDeviceInfo& deviceInfo);
void GetRAM16DeviceInfo(IDeviceInfo& deviceInfo);
void GetRAM32DeviceInfo(IDeviceInfo& deviceInfo);
void GetRAM8VariableDeviceInfo(IDeviceInfo& deviceInfo);
void GetRAM16VariableDeviceInfo(IDeviceInfo& deviceInfo);
void GetRAM32VariableDeviceInfo(IDeviceInfo& deviceInfo);
void GetSharedRAMDeviceInfo(IDeviceInfo& deviceInfo);
void GetTimedBufferIntDeviceDeviceInfo(IDeviceInfo& deviceInfo);

#endif
