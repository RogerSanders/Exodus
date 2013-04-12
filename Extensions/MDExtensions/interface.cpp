#include "SystemInterface/SystemInterface.pkg"
#include "MegaDriveROMLoader.h"

IExtension* GetMegaDriveROMLoader(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return new MegaDriveROMLoader(implementationName, instanceName, moduleID);
}

void DeleteMegaDriveROMLoader(IExtension* extension)
{
	delete extension;
}

#ifdef EX_DLLINTERFACE
extern "C" __declspec(dllexport) unsigned int GetInterfaceVersion()
{
	return EXODUS_INTERFACEVERSION;
}

extern "C" __declspec(dllexport) bool GetExtensionEntry(unsigned int entryNo, IExtensionInfo& entry)
{
	switch(entryNo)
	{
	case 0:
		entry.SetExtensionSettings(GetMegaDriveROMLoader, DeleteMegaDriveROMLoader, L"Loaders.MegaDriveROM", L"MegaDriveROMLoader", 1, L"", L"");
		return true;
	}
	return false;
}
#endif
