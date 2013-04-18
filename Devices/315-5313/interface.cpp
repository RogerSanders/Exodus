#include "SystemInterface/SystemInterface.pkg"
#include "VDP/VDP.h"
#include "S315-5313/S315_5313.h"

IDevice* GetVDP(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return new VDP(implementationName, instanceName, moduleID);
}

void DeleteVDP(IDevice* device)
{
	delete device;
}

IDevice* GetS315_5313(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID)
{
	return new S315_5313(implementationName, instanceName, moduleID);
}

void DeleteS315_5313(IDevice* device)
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
	//Retrieve any required information from the version info table for our plugin
	std::wstring copyrightText;
	std::wstring commentsText;
	HMODULE moduleHandle = NULL;
	BOOL getModuleHandleExReturn = GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)GetDeviceEntry, &moduleHandle);
	if(getModuleHandleExReturn != 0)
	{
		std::wstring modulePath = GetModuleFilePath(moduleHandle);
		GetModuleVersionInfoString(modulePath, VERSIONINFOPROPERTY_LEGALCOPYRIGHT, copyrightText);
		GetModuleVersionInfoString(modulePath, VERSIONINFOPROPERTY_COMMENTS, commentsText);
	}

	switch(entryNo)
	{
	case 0:
		entry.SetDeviceSettings(GetVDP, DeleteVDP, L"Video.315-5313", L"VDP", 1, copyrightText, commentsText);
		return true;
	case 1:
		entry.SetDeviceSettings(GetS315_5313, DeleteS315_5313, L"Video.315-5313", L"S315-5313", 1, copyrightText, commentsText);
		return true;
	}
	return false;
}

//##FIX## This is only here now for the old VDP core. Remove it when possible.
#include "../Memory/MemoryRead.cpp"
#include "../Memory/MemoryRead_Wnd.cpp"
#include "../Memory/DebugMenuHandler.cpp"
#include "../Memory/MemoryEditorView.cpp"
#include "../Memory/MemoryEditorViewModel.cpp"
#include "../Memory/MemoryEditorOldView.cpp"
#include "../Memory/MemoryEditorOldViewModel.cpp"
#include "../Memory/MemoryViewerOldView.cpp"
#include "../Memory/MemoryViewerOldViewModel.cpp"
#include "../Memory/MemoryWrite.cpp"
#include "../Memory/TimedRAM.cpp"

#endif
