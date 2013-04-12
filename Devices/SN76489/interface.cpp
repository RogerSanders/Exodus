#include "SystemInterface/SystemInterface.pkg"
#include "SN76489.h"

IDevice* GetSN76489(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return new SN76489(implementationName, instanceName, moduleID);
}

void DeleteSN76489(IDevice* device)
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
		entry.SetDeviceSettings(GetSN76489, DeleteSN76489, L"Audio.SN76489", L"SN76489", 1, L"", L"");
		return true;
	}
	return false;
}
#endif
