#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "MemoryMenus.h"

IExtension* GetMemoryMenus(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return static_cast<IExtension*>(new MemoryMenus(implementationName, instanceName, moduleID));
}

void DeleteMemoryMenus(IExtension* extension)
{
	delete static_cast<MemoryMenus*>(extension);
}

#ifdef EX_DLLINTERFACE
extern "C" __declspec(dllexport) unsigned int GetInterfaceVersion()
{
	return EXODUS_INTERFACEVERSION;
}

extern "C" __declspec(dllexport) bool GetExtensionEntry(unsigned int entryNo, IExtensionInfo& entry)
{
	//Retrieve any required information from the version info table for our plugin
	std::wstring copyrightText;
	std::wstring commentsText;
	HMODULE moduleHandle = NULL;
	BOOL getModuleHandleExReturn = GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)GetExtensionEntry, &moduleHandle);
	if(getModuleHandleExReturn != 0)
	{
		std::wstring modulePath = GetModuleFilePath(moduleHandle);
		GetModuleVersionInfoString(modulePath, VERSIONINFOPROPERTY_LEGALCOPYRIGHT, copyrightText);
		GetModuleVersionInfoString(modulePath, VERSIONINFOPROPERTY_COMMENTS, commentsText);
	}

	switch(entryNo)
	{
	case 0:
		entry.SetExtensionSettings(GetMemoryMenus, DeleteMemoryMenus, L"Memory.Menus", L"MemoryMenus", 1, copyrightText, commentsText, true);
		return true;
	}
	return false;
}
#endif
