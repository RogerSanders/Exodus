#include "DeviceInterface/DeviceInterface.pkg"
#include "A10000.h"
#include "MDControl3.h"
#include "MDControl6.h"
#include "MDBusArbiter.h"

IDevice* GetA10000(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return static_cast<IDevice*>(new A10000(implementationName, instanceName, moduleID));
}

void DeleteA10000(IDevice* device)
{
	delete static_cast<A10000*>(device);
}

IDevice* GetMDControl3(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return static_cast<IDevice*>(new MDControl3(implementationName, instanceName, moduleID));
}

void DeleteMDControl3(IDevice* device)
{
	delete static_cast<MDControl3*>(device);
}

IDevice* GetMDControl6(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return static_cast<IDevice*>(new MDControl6(implementationName, instanceName, moduleID));
}

void DeleteMDControl6(IDevice* device)
{
	delete static_cast<MDControl6*>(device);
}

IDevice* GetMDBusArbiter(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return static_cast<IDevice*>(new MDBusArbiter(implementationName, instanceName, moduleID));
}

void DeleteMDBusArbiter(IDevice* device)
{
	delete static_cast<MDBusArbiter*>(device);
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
		entry.SetDeviceSettings(GetMDControl3, DeleteMDControl3, L"Peripheral.Control.MegaDrive", L"MDControl3", 1, copyrightText, commentsText);
		return true;
	case 2:
		entry.SetDeviceSettings(GetMDControl6, DeleteMDControl6, L"Peripheral.Control.MegaDrive", L"MDControl6", 1, copyrightText, commentsText);
		return true;
	case 3:
		entry.SetDeviceSettings(GetMDBusArbiter, DeleteMDBusArbiter, L"System.Bus.MegaDriveBusArbiter", L"MDBusArbiter", 1, copyrightText, commentsText);
		return true;
	}
	return false;
}
#endif
