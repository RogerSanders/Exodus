#ifndef __PROCESSORMENUS_H__
#define __PROCESSORMENUS_H__
#include "Extension/Extension.pkg"
#include "Processor/Processor.h"
class DebugMenuHandler;

class ProcessorMenus :public Extension
{
public:
	//Constructors
	ProcessorMenus(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID);
	~ProcessorMenus();

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
