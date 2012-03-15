#include "SystemInterface/SystemInterface.pkg"
#include "ROM.h"
#include "RAM.h"
#include "SharedRAM.h"
#include "TimedRAM.h"
#include "TimedBufferIntDevice.h"

//----------------------------------------------------------------------------------------
IDevice* GetROM(const wchar_t* instanceName, unsigned int moduleID)
{
	return new ROM(instanceName, moduleID);
}

//----------------------------------------------------------------------------------------
void DeleteROM(IDevice* device)
{
	delete device;
}

//----------------------------------------------------------------------------------------
IDevice* GetRAM(const wchar_t* instanceName, unsigned int moduleID)
{
	return new RAM(instanceName, moduleID);
}

//----------------------------------------------------------------------------------------
void DeleteRAM(IDevice* device)
{
	delete device;
}

//----------------------------------------------------------------------------------------
IDevice* GetSharedRAM(const wchar_t* instanceName, unsigned int moduleID)
{
	return new SharedRAM(instanceName, moduleID);
}

//----------------------------------------------------------------------------------------
void DeleteSharedRAM(IDevice* device)
{
	delete device;
}

//----------------------------------------------------------------------------------------
IDevice* GetTimedRAM(const wchar_t* instanceName, unsigned int moduleID)
{
	return new TimedRAM(instanceName, moduleID);
}

//----------------------------------------------------------------------------------------
void DeleteTimedRAM(IDevice* device)
{
	delete device;
}

//----------------------------------------------------------------------------------------
IDevice* GetTimedBufferIntDeviceDeviceInfo(const wchar_t* instanceName, unsigned int moduleID)
{
	return new TimedBufferIntDevice(instanceName, moduleID);
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
extern "C" __declspec(dllexport) unsigned int GetSTLVersion()
{
	return EXODUS_STLVERSION;
}

//----------------------------------------------------------------------------------------
extern "C" __declspec(dllexport) bool GetLibraryEntry(unsigned int entryNo, IDeviceInfo& entry)
{
	switch(entryNo)
	{
	case 0:
		entry.SetDeviceSettings(L"ROM", GetROM, DeleteROM, 1);
		return true;
	case 1:
		entry.SetDeviceSettings(L"RAM", GetRAM, DeleteRAM, 1);
		return true;
	case 2:
		entry.SetDeviceSettings(L"SharedRAM", GetSharedRAM, DeleteSharedRAM, 1);
		return true;
	case 3:
		entry.SetDeviceSettings(L"TimedRAM", GetTimedRAM, DeleteTimedRAM, 1);
		return true;
	case 4:
		entry.SetDeviceSettings(L"TimedBufferIntDevice", GetTimedBufferIntDeviceDeviceInfo, DeleteTimedBufferIntDeviceDevice, 1);
		return true;
	}
	return false;
}
#endif
