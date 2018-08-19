#ifndef __PROCESSORMENUS_H__
#define __PROCESSORMENUS_H__
#include "Extension/Extension.pkg"
#include "Processor/Processor.h"
class DebugMenuHandler;

class ProcessorMenus :public Extension
{
public:
	//Constructors
	ProcessorMenus(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID);
	~ProcessorMenus();

	//Window functions
	virtual bool RegisterDeviceMenuHandler(IDevice* targetDevice);
	virtual void UnregisterDeviceMenuHandler(IDevice* targetDevice);
	virtual void AddDeviceMenuItems(DeviceMenu deviceMenu, IMenuSegment& menuSegment, IDevice* targetDevice);
	virtual bool RestoreDeviceViewState(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IViewPresenter** restoredViewPresenter, IDevice* targetDevice);
	virtual bool OpenDeviceView(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, IDevice* targetDevice);

private:
	std::map<IDevice*, DebugMenuHandler*> _debugMenuHandlers;
};

#endif
