#ifndef __ISYSTEMEXTENSIONINTERFACE_H__
#define __ISYSTEMEXTENSIONINTERFACE_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include "MarshalSupport/MarshalSupport.pkg"
#include "CallbackSupport/CallbackSupport.pkg"
#include "ILoadedModuleInfo.h"
#include "IConnectorInfo.h"
#include <list>

class ISystemExtensionInterface :public ISystemDeviceInterface
{
public:
	//Interface version functions
	static inline unsigned int ThisISystemExtensionInterfaceVersion() { return 1; }
	virtual unsigned int GetISystemExtensionInterfaceVersion() const = 0;

	//System interface functions
	virtual void FlagInitialize() = 0;
	virtual void InitializeDevice(IDevice* device) = 0;
	virtual void Initialize() = 0;

	//Loaded module info functions
	virtual MarshalSupport::Marshal::Ret<std::list<unsigned int>> GetLoadedModuleIDs() const = 0;
	virtual bool GetLoadedModuleInfo(unsigned int moduleID, ILoadedModuleInfo& moduleInfo) const = 0;
	virtual bool GetModuleDisplayName(unsigned int moduleID, const MarshalSupport::Marshal::Out<std::wstring>& moduleDisplayName) const = 0;
	virtual bool GetModuleInstanceName(unsigned int moduleID, const MarshalSupport::Marshal::Out<std::wstring>& moduleInstanceName) const = 0;
	virtual void LoadedModulesChangeNotifyRegister(IObserverSubscription& observer) = 0;
	virtual void LoadedModulesChangeNotifyDeregister(IObserverSubscription& observer) = 0;

	//Connector info functions
	virtual MarshalSupport::Marshal::Ret<std::list<unsigned int>> GetConnectorIDs() const = 0;
	virtual bool GetConnectorInfo(unsigned int connectorID, IConnectorInfo& connectorInfo) const = 0;

	//Loaded device info functions
	virtual MarshalSupport::Marshal::Ret<std::list<IDevice*>> GetLoadedDevices() const = 0;
	virtual bool GetDeviceDisplayName(IDevice* device, const MarshalSupport::Marshal::Out<std::wstring>& deviceDisplayName) const = 0;
	virtual bool GetDeviceInstanceName(IDevice* device, const MarshalSupport::Marshal::Out<std::wstring>& deviceInstanceName) const = 0;
	virtual bool GetFullyQualifiedDeviceDisplayName(IDevice* device, const MarshalSupport::Marshal::Out<std::wstring>& fullyQualifiedDeviceDisplayName) const = 0;

	//Loaded extension info functions
	virtual MarshalSupport::Marshal::Ret<std::list<IExtension*>> GetLoadedExtensions() const = 0;

	//System execution functions
	//##TODO## Provide a way to register a callback handler which is called when the
	//system running state changes
	virtual bool SystemRunning() const = 0;
	virtual void RunSystem() = 0;
	virtual void StopSystem() = 0;
	virtual void ExecuteDeviceStep(IDevice* device) = 0;
	virtual void ExecuteSystemStep(double maximumTimeslice) = 0;
};

#endif
