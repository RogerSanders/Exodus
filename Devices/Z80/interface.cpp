#include "SystemInterface/SystemInterface.pkg"
#include "Z80.h"

IDevice* GetZ80(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return new Z80::Z80(implementationName, instanceName, moduleID);
}

void DeleteZ80(IDevice* device)
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
	switch(entryNo)
	{
	case 0:
		entry.SetDeviceSettings(GetZ80, DeleteZ80, L"Processor.Z80", L"Z80", 1, L"", L"");
		return true;
	}
	return false;
}
#endif
