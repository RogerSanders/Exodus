#ifndef __S315_5313MENUS_H__
#define __S315_5313MENUS_H__
#include "Extension/Extension.pkg"
#include "315-5313/IS315_5313.h"
class DebugMenuHandler;
class SettingsMenuHandler;

class S315_5313Menus :public Extension
{
public:
	//Constructors
	S315_5313Menus(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID);
	~S315_5313Menus();

	//Window functions
	virtual bool RegisterDeviceMenuHandler(IDevice* targetDevice);
	virtual void UnregisterDeviceMenuHandler(IDevice* targetDevice);
	virtual void AddDeviceMenuItems(DeviceMenu deviceMenu, IMenuSegment& menuSegment, IDevice* targetDevice);
	virtual bool RestoreDeviceViewState(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IViewPresenter** restoredViewPresenter, IDevice* targetDevice);
	virtual bool OpenDeviceView(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, IDevice* targetDevice);

private:
	std::map<const IDevice*, DebugMenuHandler*> debugMenuHandlers;
	std::map<const IDevice*, SettingsMenuHandler*> settingsMenuHandlers;
};

#endif
