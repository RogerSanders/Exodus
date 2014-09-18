#include "SystemInterface/SystemInterface.pkg"
#include "System.h"

ISystemGUIInterface* GetSystem(IGUIExtensionInterface& guiInterface)
{
	return static_cast<ISystemGUIInterface*>(new System(guiInterface));
}

void DeleteSystem(ISystemGUIInterface* system)
{
	delete static_cast<System*>(system);
}

#ifdef EX_DLLINTERFACE
extern "C" __declspec(dllexport) unsigned int GetInterfaceVersion()
{
	return EXODUS_INTERFACEVERSION;
}

extern "C" __declspec(dllexport) bool GetSystemEntry(unsigned int entryNo, ISystemInfo& entry)
{
	//Retrieve any required information from the version info table for our assembly
	std::wstring copyrightText;
	std::wstring commentsText;
	HMODULE moduleHandle = NULL;
	BOOL getModuleHandleExReturn = GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)GetSystemEntry, &moduleHandle);
	if(getModuleHandleExReturn != 0)
	{
		std::wstring modulePath = GetModuleFilePath(moduleHandle);
		GetModuleVersionInfoString(modulePath, VERSIONINFOPROPERTY_LEGALCOPYRIGHT, copyrightText);
		GetModuleVersionInfoString(modulePath, VERSIONINFOPROPERTY_COMMENTS, commentsText);
	}

	switch(entryNo)
	{
	case 0:
		entry.SetSystemSettings(GetSystem, DeleteSystem, 1, copyrightText, commentsText);
		return true;
	}
	return false;
}
#endif
