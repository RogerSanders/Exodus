#include "SystemInterface/SystemInterface.pkg"
#include "A10000.h"
#include "A04000.h"
#include "A06000.h"
#include "A11100.h"
#include "MDControl3.h"
#include "MDBusArbiter.h"

IDevice* GetA10000(const wchar_t* instanceName, unsigned int moduleID)
{
	return new A10000(instanceName, moduleID);
}

void DeleteA10000(IDevice* device)
{
	delete device;
}

IDevice* GetA04000(const wchar_t* instanceName, unsigned int moduleID)
{
	return new A04000(instanceName, moduleID);
}

void DeleteA04000(IDevice* device)
{
	delete device;
}

IDevice* GetA06000(const wchar_t* instanceName, unsigned int moduleID)
{
	return new A06000(instanceName, moduleID);
}

void DeleteA06000(IDevice* device)
{
	delete device;
}

IDevice* GetA11100(const wchar_t* instanceName, unsigned int moduleID)
{
	return new A11100(instanceName, moduleID);
}

void DeleteA11100(IDevice* device)
{
	delete device;
}

IDevice* GetMDControl3(const wchar_t* instanceName, unsigned int moduleID)
{
	return new MDControl3(instanceName, moduleID);
}

void DeleteMDControl3(IDevice* device)
{
	delete device;
}

IDevice* GetMDBusArbiter(const wchar_t* instanceName, unsigned int moduleID)
{
	return new MDBusArbiter(instanceName, moduleID);
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

extern "C" __declspec(dllexport) unsigned int GetSTLVersion()
{
	return EXODUS_STLVERSION;
}

extern "C" __declspec(dllexport) bool GetLibraryEntry(unsigned int entryNo, IDeviceInfo& entry)
{
	switch(entryNo)
	{
	case 0:
		entry.SetDeviceSettings(L"A10000", GetA10000, DeleteA10000, 1);
		return true;
	case 1:
		entry.SetDeviceSettings(L"A04000", GetA04000, DeleteA04000, 1);
		return true;
	case 2:
		entry.SetDeviceSettings(L"A06000", GetA06000, DeleteA06000, 1);
		return true;
	case 3:
		entry.SetDeviceSettings(L"A11100", GetA11100, DeleteA11100, 1);
		return true;
	case 4:
		entry.SetDeviceSettings(L"MDControl3", GetMDControl3, DeleteMDControl3, 1);
		return true;
	case 5:
		entry.SetDeviceSettings(L"MDBusArbiter", GetMDBusArbiter, DeleteMDBusArbiter, 1);
		return true;
	}
	return false;
}
#endif
