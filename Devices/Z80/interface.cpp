#include "SystemInterface/SystemInterface.pkg"
#include "Z80.h"

IDevice* GetZ80(const wchar_t* instanceName, unsigned int moduleID)
{
	return new Z80::Z80(instanceName, moduleID);
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

extern "C" __declspec(dllexport) bool GetLibraryEntry(unsigned int entryNo, IDeviceInfo& entry)
{
	switch(entryNo)
	{
	case 0:
		entry.SetDeviceSettings(L"Z80", GetZ80, DeleteZ80, 1);
		return true;
	}
	return false;
}
#endif
