#include "SystemInterface/SystemInterface.pkg"
#include "ROM.h"
#include "RAM.h"
#include "SharedRAM.h"
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
	//Retrieve any required information from the version info table for our plugin
	std::wstring copyrightText;
	std::wstring commentsText;
	HMODULE moduleHandle = NULL;
	BOOL getModuleHandleExReturn = GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)GetDeviceEntry, &moduleHandle);
	if(getModuleHandleExReturn != 0)
	{
		std::wstring modulePath = GetModuleFilePath(moduleHandle);
		GetModuleVersionInfoString(modulePath, VERSIONINFOPROPERTY_LEGALCOPYRIGHT, copyrightText);
		GetModuleVersionInfoString(modulePath, VERSIONINFOPROPERTY_COMMENTS, commentsText);
	}

	switch(entryNo)
	{
	case 0:
		entry.SetDeviceSettings(GetROM, DeleteROM, L"Memory.ROM", L"ROM", 1, copyrightText, commentsText);
		return true;
	case 1:
		entry.SetDeviceSettings(GetRAM, DeleteRAM, L"Memory.RAM", L"RAM", 1, copyrightText, commentsText);
		return true;
	case 2:
		entry.SetDeviceSettings(GetSharedRAM, DeleteSharedRAM, L"Memory.RAM", L"SharedRAM", 1, copyrightText, commentsText);
		return true;
	case 3:
		entry.SetDeviceSettings(GetTimedBufferIntDeviceDeviceInfo, DeleteTimedBufferIntDeviceDevice, L"Memory.RAM", L"TimedBufferIntDevice", 1, copyrightText, commentsText);
		return true;
	}
	return false;
}
#endif
