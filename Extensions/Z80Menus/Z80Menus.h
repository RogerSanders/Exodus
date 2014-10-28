#ifndef __Z80MENUS_H__
#define __Z80MENUS_H__
#include "Extension/Extension.pkg"
#include "Z80/IZ80.h"
class DebugMenuHandler;

class Z80Menus :public Extension
{
public:
	//Constructors
	Z80Menus(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID);
	~Z80Menus();

	//Window functions
	virtual bool RegisterDeviceMenuHandler(IDevice* targetDevice);
	virtual void UnregisterDeviceMenuHandler(IDevice* targetDevice);
	virtual void AddDeviceMenuItems(DeviceMenu deviceMenu, IMenuSegment& menuSegment, IDevice* targetDevice);
	virtual bool RestoreDeviceViewState(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IDevice* targetDevice);
	virtual bool OpenDeviceView(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IDevice* targetDevice);

private:
	std::map<IDevice*, DebugMenuHandler*> debugMenuHandlers;
};

#endif
