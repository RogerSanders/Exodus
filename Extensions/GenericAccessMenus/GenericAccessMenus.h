#ifndef __GENERICACCESSMENUS_H__
#define __GENERICACCESSMENUS_H__
#include "Extension/Extension.pkg"
class DebugMenuHandler;
class SettingsMenuHandler;

//##TODO## Register menu handlers for extensions or the system itself if they expose
//generic access interfaces too
class GenericAccessMenus :public Extension
{
public:
	//Constructors
	GenericAccessMenus(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID);
	~GenericAccessMenus();

	//Window functions
	virtual bool RegisterDeviceMenuHandler(IDevice* targetDevice);
	virtual void UnregisterDeviceMenuHandler(IDevice* targetDevice);
	virtual void AddDeviceMenuItems(DeviceMenu deviceMenu, IMenuSegment& menuSegment, IDevice* targetDevice);
	virtual bool RestoreDeviceViewState(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IViewPresenter** restoredViewPresenter, IDevice* targetDevice);
	virtual bool OpenDeviceView(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, IDevice* targetDevice);

private:
	std::map<IDevice*, DebugMenuHandler*> _debugMenuHandlers;
	std::map<IDevice*, SettingsMenuHandler*> _settingsMenuHandlers;
};

#endif
