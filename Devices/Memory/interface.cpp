#include "SystemInterface/SystemInterface.pkg"
#include "ROM.h"
#include "RAM.h"
#include "SharedRAM.h"
#include "TimedRAM.h"
#include "TimedBufferIntDevice.h"

//----------------------------------------------------------------------------------------
IDevice* GetROM(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return new ROM(implementationName, instanceName, moduleID);
}

//----------------------------------------------------------------------------------------
void DeleteROM(IDevice* device)
{
	delete device;
}

//----------------------------------------------------------------------------------------
IDevice* GetRAM(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return new RAM(implementationName, instanceName, moduleID);
}

//----------------------------------------------------------------------------------------
void DeleteRAM(IDevice* device)
{
	delete device;
}

//----------------------------------------------------------------------------------------
IDevice* GetSharedRAM(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return new SharedRAM(implementationName, instanceName, moduleID);
}

//----------------------------------------------------------------------------------------
void DeleteSharedRAM(IDevice* device)
{
	delete device;
}

//----------------------------------------------------------------------------------------
IDevice* GetTimedRAM(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return new TimedRAM(implementationName, instanceName, moduleID);
}

//----------------------------------------------------------------------------------------
void DeleteTimedRAM(IDevice* device)
{
	delete device;
}

//----------------------------------------------------------------------------------------
IDevice* GetTimedBufferIntDeviceDeviceInfo(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return new TimedBufferIntDevice(implementationName, instanceName, moduleID);
}

//----------------------------------------------------------------------------------------
void DeleteTimedBufferIntDeviceDevice(IDevice* device)
{
	delete device;
}

#ifdef EX_DLLINTERFACE
//----------------------------------------------------------------------------------------
extern "C" __declspec(dllexport) unsigned int GetInterfaceVersion()
{
	return EXODUS_INTERFACEVERSION;
}

//----------------------------------------------------------------------------------------
extern "C" __declspec(dllexport) bool GetDeviceEntry(unsigned int entryNo, IDeviceInfo& entry)
{
	switch(entryNo)
	{
	case 0:
		entry.SetDeviceSettings(GetROM, DeleteROM, L"Memory.ROM", L"ROM", 1, L"", L"");
		return true;
	case 1:
		entry.SetDeviceSettings(GetRAM, DeleteRAM, L"Memory.RAM", L"RAM", 1, L"", L"");
		return true;
	case 2:
		entry.SetDeviceSettings(GetSharedRAM, DeleteSharedRAM, L"Memory.RAM", L"SharedRAM", 1, L"", L"");
		return true;
	case 3:
		entry.SetDeviceSettings(GetTimedRAM, DeleteTimedRAM, L"Memory.RAM", L"TimedRAM", 1, L"", L"");
		return true;
	case 4:
		entry.SetDeviceSettings(GetTimedBufferIntDeviceDeviceInfo, DeleteTimedBufferIntDeviceDevice, L"Memory.RAM", L"TimedBufferIntDevice", 1, L"", L"");
		return true;
	}
	return false;
}
#endif
