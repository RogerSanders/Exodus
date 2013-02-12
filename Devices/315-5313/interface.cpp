#include "SystemInterface/SystemInterface.pkg"
#include "VDP/VDP.h"
#include "S315-5313/S315_5313.h"

IDevice* GetVDP(const wchar_t* instanceName, unsigned int moduleID)
{
	return new VDP(instanceName, moduleID);
}

void DeleteVDP(IDevice* device)
{
	delete device;
}

IDevice* GetS315_5313(const wchar_t* instanceName, unsigned int moduleID)
{
	return new S315_5313(instanceName, moduleID);
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

extern "C" __declspec(dllexport) bool GetLibraryEntry(unsigned int entryNo, IDeviceInfo& entry)
{
	switch(entryNo)
	{
	case 0:
		entry.SetDeviceSettings(L"VDP", GetVDP, DeleteVDP, 1);
		return true;
	case 1:
		entry.SetDeviceSettings(L"S315-5313", GetS315_5313, DeleteS315_5313, 1);
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
