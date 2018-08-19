#ifndef __MEMORYMENUS_H__
#define __MEMORYMENUS_H__
#include "Extension/Extension.pkg"
#include "Memory/IMemory.h"
class DebugMenuHandler;

class MemoryMenus :public Extension
{
public:
	//Constructors
	MemoryMenus(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID);
	~MemoryMenus();

	//Window functions
	virtual bool RegisterDeviceMenuHandler(IDevice* targetDevice);
	virtual void UnregisterDeviceMenuHandler(IDevice* targetDevice);
	virtual void AddDeviceMenuItems(DeviceMenu deviceMenu, IMenuSegment& menuSegment, IDevice* targetDevice);
	virtual bool RestoreDeviceViewState(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IViewPresenter** restoredViewPresenter, IDevice* targetDevice);
	virtual bool OpenDeviceView(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, IDevice* targetDevice);

private:
	std::map<IDevice*, DebugMenuHandler*> _debugMenuHandlers;
};

#endif
