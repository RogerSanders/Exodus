#ifndef __MEMORYMENUS_H__
#define __MEMORYMENUS_H__
#include "Extension/Extension.pkg"
#include "Memory/IMemory.h"
class DebugMenuHandler;

class MemoryMenus :public Extension
{
public:
	//Constructors
	MemoryMenus(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID);
	~MemoryMenus();

	//Window functions
	virtual bool RegisterDeviceMenuHandler(IDevice* targetDevice);
	virtual void UnregisterDeviceMenuHandler(IDevice* targetDevice);
	virtual void AddDeviceMenuItems(DeviceMenu deviceMenu, IMenuSegment& menuSegment, IDevice* targetDevice);
	virtual bool RestoreDeviceViewState(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState, IDevice* targetDevice);
	virtual bool OpenDeviceView(const std::wstring& viewGroupName, const std::wstring& viewName, IDevice* targetDevice);

private:
	std::map<IDevice*, DebugMenuHandler*> debugMenuHandlers;
};

#endif
