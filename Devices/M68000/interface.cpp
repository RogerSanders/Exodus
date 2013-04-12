#include "SystemInterface/SystemInterface.pkg"
#include "M68000.h"

IDevice* GetM68000(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return new M68000::M68000(implementationName, instanceName, moduleID);
}

void DeleteM68000(IDevice* device)
{
	delete device;
}

void GetM68000DeviceInfo(IDeviceInfo& deviceInfo)
{
	deviceInfo.SetDeviceSettings(GetM68000, DeleteM68000, L"Processor.68000", L"M68000", 1, L"", L"");
}

#ifdef EX_DLLINTERFACE
extern "C" __declspec(dllexport) unsigned int GetInterfaceVersion()
{
	return EXODUS_INTERFACEVERSION;
}

extern "C" __declspec(dllexport) bool GetDeviceEntry(unsigned int entryNo, IDeviceInfo& entry)
{
	switch(entryNo)
	{
	case 0:
		GetM68000DeviceInfo(entry);
		return true;
	}
	return false;
}
#endif
