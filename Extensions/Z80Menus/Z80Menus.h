#ifndef __Z80MENUS_H__
#define __Z80MENUS_H__
#include "Extension/Extension.pkg"
#include "Z80/IZ80.h"
class DebugMenuHandler;

class Z80Menus :public Extension
{
public:
	// Constructors
	Z80Menus(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID);
	~Z80Menus();

	// Window functions
	virtual bool RegisterDeviceMenuHandler(IDevice* targetDevice);
	virtual void UnregisterDeviceMenuHandler(IDevice* targetDevice);
	virtual void AddDeviceMenuItems(DeviceMenu deviceMenu, IMenuSegment& menuSegment, IDevice* targetDevice);
	virtual bool RestoreDeviceViewState(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IViewPresenter** restoredViewPresenter, IDevice* targetDevice);
	virtual bool OpenDeviceView(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, IDevice* targetDevice);

private:
	std::map<IDevice*, DebugMenuHandler*> _debugMenuHandlers;
};

#endif
