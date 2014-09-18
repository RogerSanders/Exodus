#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "S315_5313Menus.h"

IExtension* GetS315_5313Menus(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return static_cast<IExtension*>(new S315_5313Menus(implementationName, instanceName, moduleID));
}

void DeleteS315_5313Menus(IExtension* extension)
{
	delete static_cast<S315_5313Menus*>(extension);
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
		entry.SetExtensionSettings(GetS315_5313Menus, DeleteS315_5313Menus, L"Video.S315-5313.Menus", L"S315-5313Menus", 1, copyrightText, commentsText);
		return true;
	}
	return false;
}
#endif
