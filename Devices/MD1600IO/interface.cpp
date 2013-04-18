#include "SystemInterface/SystemInterface.pkg"
#include "A10000.h"
#include "A04000.h"
#include "A06000.h"
#include "A11100.h"
#include "MDControl3.h"
#include "MDControl6.h"
#include "MDBusArbiter.h"

IDevice* GetA10000(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return new A10000(implementationName, instanceName, moduleID);
}

void DeleteA10000(IDevice* device)
{
	delete device;
}

IDevice* GetA04000(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return new A04000(implementationName, instanceName, moduleID);
}

void DeleteA04000(IDevice* device)
{
	delete device;
}

IDevice* GetA06000(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return new A06000(implementationName, instanceName, moduleID);
}

void DeleteA06000(IDevice* device)
{
	delete device;
}

IDevice* GetA11100(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return new A11100(implementationName, instanceName, moduleID);
}

void DeleteA11100(IDevice* device)
{
	delete device;
}

IDevice* GetMDControl3(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return new MDControl3(implementationName, instanceName, moduleID);
}

void DeleteMDControl3(IDevice* device)
{
	delete device;
}

IDevice* GetMDControl6(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return new MDControl6(implementationName, instanceName, moduleID);
}

void DeleteMDControl6(IDevice* device)
{
	delete device;
}

IDevice* GetMDBusArbiter(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return new MDBusArbiter(implementationName, instanceName, moduleID);
}

void DeleteMDBusArbiter(IDevice* device)
{
	delete device;
}

#ifdef EX_DLLINTERFACE
extern "C" __declspec(dllexport) unsigned int GetInterfaceVersion()
{
	return EXODUS_INTERFACEVERSION;
}

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
		entry.SetDeviceSettings(GetA10000, DeleteA10000, L"System.Bus.MegaDriveBusArbiter", L"A10000", 1, copyrightText, commentsText);
		return true;
	case 1:
		entry.SetDeviceSettings(GetA04000, DeleteA04000, L"Audio.YM2612", L"A04000", 1, copyrightText, commentsText);
		return true;
	case 2:
		entry.SetDeviceSettings(GetA06000, DeleteA06000, L"System.Bus.MegaDriveBusArbiter", L"A06000", 1, copyrightText, commentsText);
		return true;
	case 3:
		entry.SetDeviceSettings(GetA11100, DeleteA11100, L"System.Bus.MegaDriveBusArbiter", L"A11100", 1, copyrightText, commentsText);
		return true;
	case 4:
		entry.SetDeviceSettings(GetMDControl3, DeleteMDControl3, L"Peripheral.Control.MegaDrive", L"MDControl3", 1, copyrightText, commentsText);
		return true;
	case 5:
		entry.SetDeviceSettings(GetMDControl6, DeleteMDControl6, L"Peripheral.Control.MegaDrive", L"MDControl6", 1, copyrightText, commentsText);
		return true;
	case 6:
		entry.SetDeviceSettings(GetMDBusArbiter, DeleteMDBusArbiter, L"System.Bus.MegaDriveBusArbiter", L"MDBusArbiter", 1, copyrightText, commentsText);
		return true;
	}
	return false;
}
#endif
