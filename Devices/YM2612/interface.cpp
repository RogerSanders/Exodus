#include "SystemInterface/SystemInterface.pkg"
#include "YM2612.h"

IDevice* GetYM2612(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return new YM2612(implementationName, instanceName, moduleID);
}

void DeleteYM2612(IDevice* device)
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
		entry.SetDeviceSettings(GetYM2612, DeleteYM2612, L"Audio.YM2612", L"YM2612", 1, L"", L"");
		return true;
	}
	return false;
}
#endif
