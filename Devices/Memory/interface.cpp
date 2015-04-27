#include "DeviceInterface/DeviceInterface.pkg"
#include "ROM8.h"
#include "ROM16.h"
#include "ROM32.h"
#include "ROM8Variable.h"
#include "ROM16Variable.h"
#include "ROM32Variable.h"
#include "RAM8.h"
#include "RAM16.h"
#include "RAM32.h"
#include "RAM8Variable.h"
#include "RAM16Variable.h"
#include "RAM32Variable.h"
#include "SharedRAM.h"
#include "TimedBufferIntDevice.h"

//----------------------------------------------------------------------------------------
IDevice* GetROM8(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return static_cast<IDevice*>(new ROM8(implementationName, instanceName, moduleID));
}

//----------------------------------------------------------------------------------------
void DeleteROM8(IDevice* device)
{
	delete static_cast<ROM8*>(device);
}

//----------------------------------------------------------------------------------------
IDevice* GetROM16(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return static_cast<IDevice*>(new ROM16(implementationName, instanceName, moduleID));
}

//----------------------------------------------------------------------------------------
void DeleteROM16(IDevice* device)
{
	delete static_cast<ROM16*>(device);
}

//----------------------------------------------------------------------------------------
IDevice* GetROM32(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return static_cast<IDevice*>(new ROM32(implementationName, instanceName, moduleID));
}

//----------------------------------------------------------------------------------------
void DeleteROM32(IDevice* device)
{
	delete static_cast<ROM32*>(device);
}

//----------------------------------------------------------------------------------------
IDevice* GetROM8Variable(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return static_cast<IDevice*>(new ROM8Variable(implementationName, instanceName, moduleID));
}

//----------------------------------------------------------------------------------------
void DeleteROM8Variable(IDevice* device)
{
	delete static_cast<ROM8Variable*>(device);
}

//----------------------------------------------------------------------------------------
IDevice* GetROM16Variable(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return static_cast<IDevice*>(new ROM16Variable(implementationName, instanceName, moduleID));
}

//----------------------------------------------------------------------------------------
void DeleteROM16Variable(IDevice* device)
{
	delete static_cast<ROM16Variable*>(device);
}

//----------------------------------------------------------------------------------------
IDevice* GetROM32Variable(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return static_cast<IDevice*>(new ROM32Variable(implementationName, instanceName, moduleID));
}

//----------------------------------------------------------------------------------------
void DeleteROM32Variable(IDevice* device)
{
	delete static_cast<ROM32Variable*>(device);
}

//----------------------------------------------------------------------------------------
IDevice* GetRAM8(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return static_cast<IDevice*>(new RAM8(implementationName, instanceName, moduleID));
}

//----------------------------------------------------------------------------------------
void DeleteRAM8(IDevice* device)
{
	delete static_cast<RAM8*>(device);
}

//----------------------------------------------------------------------------------------
IDevice* GetRAM16(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return static_cast<IDevice*>(new RAM16(implementationName, instanceName, moduleID));
}

//----------------------------------------------------------------------------------------
void DeleteRAM16(IDevice* device)
{
	delete static_cast<RAM16*>(device);
}

//----------------------------------------------------------------------------------------
IDevice* GetRAM32(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return static_cast<IDevice*>(new RAM32(implementationName, instanceName, moduleID));
}

//----------------------------------------------------------------------------------------
void DeleteRAM32(IDevice* device)
{
	delete static_cast<RAM32*>(device);
}

//----------------------------------------------------------------------------------------
IDevice* GetRAM8Variable(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return static_cast<IDevice*>(new RAM8Variable(implementationName, instanceName, moduleID));
}

//----------------------------------------------------------------------------------------
void DeleteRAM8Variable(IDevice* device)
{
	delete static_cast<RAM8Variable*>(device);
}

//----------------------------------------------------------------------------------------
IDevice* GetRAM16Variable(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return static_cast<IDevice*>(new RAM16Variable(implementationName, instanceName, moduleID));
}

//----------------------------------------------------------------------------------------
void DeleteRAM16Variable(IDevice* device)
{
	delete static_cast<RAM16Variable*>(device);
}

//----------------------------------------------------------------------------------------
IDevice* GetRAM32Variable(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return static_cast<IDevice*>(new RAM32Variable(implementationName, instanceName, moduleID));
}

//----------------------------------------------------------------------------------------
void DeleteRAM32Variable(IDevice* device)
{
	delete static_cast<RAM32Variable*>(device);
}

//----------------------------------------------------------------------------------------
IDevice* GetSharedRAM(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return static_cast<IDevice*>(new SharedRAM(implementationName, instanceName, moduleID));
}

//----------------------------------------------------------------------------------------
void DeleteSharedRAM(IDevice* device)
{
	delete static_cast<SharedRAM*>(device);
}

//----------------------------------------------------------------------------------------
IDevice* GetTimedBufferIntDeviceDeviceInfo(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return static_cast<IDevice*>(new TimedBufferIntDevice(implementationName, instanceName, moduleID));
}

//----------------------------------------------------------------------------------------
void DeleteTimedBufferIntDeviceDevice(IDevice* device)
{
	delete static_cast<TimedBufferIntDevice*>(device);
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
		entry.SetDeviceSettings(GetROM8, DeleteROM8, L"Memory.ROM", L"ROM8", 1, copyrightText, commentsText);
		return true;
	case 1:
		entry.SetDeviceSettings(GetROM16, DeleteROM16, L"Memory.ROM", L"ROM16", 1, copyrightText, commentsText);
		return true;
	case 2:
		entry.SetDeviceSettings(GetROM32, DeleteROM32, L"Memory.ROM", L"ROM32", 1, copyrightText, commentsText);
		return true;
	case 3:
		entry.SetDeviceSettings(GetROM8Variable, DeleteROM8Variable, L"Memory.ROM", L"ROM8Variable", 1, copyrightText, commentsText);
		return true;
	case 4:
		entry.SetDeviceSettings(GetROM16Variable, DeleteROM16Variable, L"Memory.ROM", L"ROM16Variable", 1, copyrightText, commentsText);
		return true;
	case 5:
		entry.SetDeviceSettings(GetROM32Variable, DeleteROM32Variable, L"Memory.ROM", L"ROM32Variable", 1, copyrightText, commentsText);
		return true;
	case 6:
		entry.SetDeviceSettings(GetRAM8, DeleteRAM8, L"Memory.RAM", L"RAM8", 1, copyrightText, commentsText);
		return true;
	case 7:
		entry.SetDeviceSettings(GetRAM16, DeleteRAM16, L"Memory.RAM", L"RAM16", 1, copyrightText, commentsText);
		return true;
	case 8:
		entry.SetDeviceSettings(GetRAM32, DeleteRAM32, L"Memory.RAM", L"RAM32", 1, copyrightText, commentsText);
		return true;
	case 9:
		entry.SetDeviceSettings(GetRAM8Variable, DeleteRAM8Variable, L"Memory.RAM", L"RAM8Variable", 1, copyrightText, commentsText);
		return true;
	case 10:
		entry.SetDeviceSettings(GetRAM16Variable, DeleteRAM16Variable, L"Memory.RAM", L"RAM16Variable", 1, copyrightText, commentsText);
		return true;
	case 11:
		entry.SetDeviceSettings(GetRAM32Variable, DeleteRAM32Variable, L"Memory.RAM", L"RAM32Variable", 1, copyrightText, commentsText);
		return true;
	case 12:
		entry.SetDeviceSettings(GetSharedRAM, DeleteSharedRAM, L"Memory.RAM", L"SharedRAM", 1, copyrightText, commentsText);
		return true;
	case 13:
		entry.SetDeviceSettings(GetTimedBufferIntDeviceDeviceInfo, DeleteTimedBufferIntDeviceDevice, L"Memory.RAM", L"TimedBufferIntDevice", 1, copyrightText, commentsText);
		return true;
	}
	return false;
}
#endif
