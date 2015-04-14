#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "ExodusSystemMenus.h"

IExtension* GetExodusSystemMenus(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return static_cast<IExtension*>(new ExodusSystemMenus(implementationName, instanceName, moduleID));
}

void DeleteExodusSystemMenus(IExtension* extension)
{
	delete static_cast<ExodusSystemMenus*>(extension);
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
		entry.SetExtensionSettings(GetExodusSystemMenus, DeleteExodusSystemMenus, L"System.Menus", L"ExodusSystemMenus", 1, copyrightText, commentsText, true);
		return true;
	}
	return false;
}
#endif
