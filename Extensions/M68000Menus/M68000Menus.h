#ifndef __M68000MENUS_H__
#define __M68000MENUS_H__
#include "Extension/Extension.pkg"
class DebugMenuHandler;

class M68000Menus :public Extension
{
public:
	//Constructors
	M68000Menus(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID);
	~M68000Menus();

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
