#ifndef __YM2612MENUS_H__
#define __YM2612MENUS_H__
#include "Extension/Extension.pkg"
#include "YM2612/YM2612.h"
class DebugMenuHandler;

class YM2612Menus :public Extension
{
public:
	// Constructors
	YM2612Menus(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID);
	~YM2612Menus();

	// Window functions
	virtual bool RegisterDeviceMenuHandler(IDevice* targetDevice);
	virtual void UnregisterDeviceMenuHandler(IDevice* targetDevice);
	virtual void AddDeviceMenuItems(DeviceMenu deviceMenu, IMenuSegment& menuSegment, IDevice* targetDevice);
	virtual bool RestoreDeviceViewState(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IViewPresenter** restoredViewPresenter, IDevice* targetDevice);
	virtual bool OpenDeviceView(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, IDevice* targetDevice);
	void OpenOperatorView(const IDevice* targetDevice, unsigned int channelNo, unsigned int operatorNo);

private:
	std::map<const IDevice*, DebugMenuHandler*> _debugMenuHandlers;
};

#endif
