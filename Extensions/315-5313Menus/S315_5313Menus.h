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
	virtual bool RestoreDeviceViewState(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState, IDevice* targetDevice);
	virtual bool OpenDeviceView(const std::wstring& viewGroupName, const std::wstring& viewName, IDevice* targetDevice);
	void OpenSpriteListDetailsView(const IDevice* targetDevice, unsigned int aspriteIndex);
	void OpenPortMonitorDetailsView(const IDevice* targetDevice, const IS315_5313::PortMonitorEntry& aentry);

private:
	std::map<const IDevice*, DebugMenuHandler*> debugMenuHandlers;
	std::map<const IDevice*, SettingsMenuHandler*> settingsMenuHandlers;
};

#endif
