#ifndef __YM2612MENUS_H__
#define __YM2612MENUS_H__
#include "Extension/Extension.pkg"
#include "YM2612/YM2612.h"
class DebugMenuHandler;

class YM2612Menus :public Extension
{
public:
	//Constructors
	YM2612Menus(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID);
	~YM2612Menus();

	//Window functions
	virtual bool RegisterDeviceMenuHandler(IDevice* targetDevice);
	virtual void UnregisterDeviceMenuHandler(IDevice* targetDevice);
	virtual void AddDeviceMenuItems(DeviceMenu deviceMenu, IMenuSegment& menuSegment, IDevice* targetDevice);
	virtual bool RestoreDeviceViewState(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IViewPresenter** restoredViewPresenter, IDevice* targetDevice);
	virtual bool OpenDeviceView(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IDevice* targetDevice);
	void OpenOperatorView(const IDevice* targetDevice, unsigned int achannelNo, unsigned int aoperatorNo);

private:
	std::map<const IDevice*, DebugMenuHandler*> debugMenuHandlers;
};

#endif
