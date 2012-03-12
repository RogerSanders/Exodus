#include "SystemInterface/SystemInterface.pkg"
#include "YM2612.h"

IDevice* GetYM2612(const wchar_t* instanceName, unsigned int moduleID)
{
	return new YM2612(instanceName, moduleID);
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

extern "C" __declspec(dllexport) unsigned int GetSTLVersion()
{
	return EXODUS_STLVERSION;
}

extern "C" __declspec(dllexport) bool GetLibraryEntry(unsigned int entryNo, IDeviceInfo& entry)
{
	switch(entryNo)
	{
	case 0:
		entry.SetDeviceSettings(L"YM2612", GetYM2612, DeleteYM2612, 1);
		return true;
	}
	return false;
}
#endif
