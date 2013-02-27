#include "SystemInterface/SystemInterface.pkg"
#include "SN76489.h"

IDevice* GetSN76489(const wchar_t* instanceName, unsigned int moduleID)
{
	return new SN76489(instanceName, moduleID);
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
		entry.SetDeviceSettings(L"SN76489", GetSN76489, DeleteSN76489, 1);
		return true;
	}
	return false;
}
#endif
