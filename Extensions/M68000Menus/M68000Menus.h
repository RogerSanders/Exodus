#ifndef __M68000MENUS_H__
#define __M68000MENUS_H__
#include "Extension/Extension.pkg"
class DebugMenuHandler;

class M68000Menus :public Extension
{
public:
	// Constructors
	M68000Menus(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID);
	~M68000Menus();

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
