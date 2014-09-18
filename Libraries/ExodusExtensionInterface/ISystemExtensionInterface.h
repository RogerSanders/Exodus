#ifndef __ISYSTEMEXTENSIONINTERFACE_H__
#define __ISYSTEMEXTENSIONINTERFACE_H__
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "InteropSupport/InteropSupport.pkg"
#include "ILoadedModuleInfo.h"
#include "IConnectorInfo.h"
#include <list>

class ISystemExtensionInterface :public ISystemDeviceInterface
{
public:
	//Interface version functions
	static inline unsigned int ThisISystemExtensionInterfaceVersion();
	virtual unsigned int GetISystemExtensionInterfaceVersion() const = 0;

	//System interface functions
	virtual void FlagInitialize() = 0;
	virtual void InitializeDevice(IDevice* device) = 0;
	virtual void Initialize() = 0;

	//Loaded module info functions
	inline std::list<unsigned int> GetLoadedModuleIDs() const;
	virtual bool GetLoadedModuleInfo(unsigned int moduleID, ILoadedModuleInfo& moduleInfo) const = 0;
	inline bool GetModuleDisplayName(unsigned int moduleID, std::wstring& moduleDisplayName) const;
	inline bool GetModuleInstanceName(unsigned int moduleID, std::wstring& moduleInstanceName) const;
	virtual void LoadedModulesChangeNotifyRegister(IObserverSubscription& observer) = 0;
	virtual void LoadedModulesChangeNotifyDeregister(IObserverSubscription& observer) = 0;

	//Connector info functions
	inline std::list<unsigned int> GetConnectorIDs() const;
	virtual bool GetConnectorInfo(unsigned int connectorID, IConnectorInfo& connectorInfo) const = 0;

	//Loaded device info functions
	inline std::list<IDevice*> GetLoadedDevices() const;
	inline bool GetDeviceDisplayName(IDevice* device, std::wstring& deviceDisplayName) const;
	inline bool GetDeviceInstanceName(IDevice* device, std::wstring& deviceInstanceName) const;
	inline bool GetFullyQualifiedDeviceDisplayName(IDevice* device, std::wstring& fullyQualifiedDeviceDisplayName) const;

	//Loaded extension info functions
	inline std::list<IExtension*> GetLoadedExtensions() const;

	//System execution functions
	//##TODO## Provide a way to register a callback handler which is called when the
	//system running state changes
	virtual bool SystemRunning() const = 0;
	virtual void RunSystem() = 0;
	virtual void StopSystem() = 0;
	virtual void ExecuteDeviceStep(IDevice* device) = 0;
	virtual void ExecuteSystemStep(double maximumTimeslice) = 0;

protected:
	//Loaded module info functions
	virtual void GetLoadedModuleIDsInternal(const InteropSupport::ISTLObjectTarget<std::list<unsigned int>>& marshaller) const = 0;
	virtual bool GetModuleDisplayNameInternal(unsigned int moduleID, const InteropSupport::ISTLObjectTarget<std::wstring>& moduleDisplayNameMarshaller) const = 0;
	virtual bool GetModuleInstanceNameInternal(unsigned int moduleID, const InteropSupport::ISTLObjectTarget<std::wstring>& moduleInstanceNameMarshaller) const = 0;

	//Connector info functions
	virtual void GetConnectorIDsInternal(const InteropSupport::ISTLObjectTarget<std::list<unsigned int>>& marshaller) const = 0;

	//Loaded device info functions
	virtual void GetLoadedDevicesInternal(const InteropSupport::ISTLObjectTarget<std::list<IDevice*>>& marshaller) const = 0;
	virtual bool GetDeviceDisplayNameInternal(IDevice* device, const InteropSupport::ISTLObjectTarget<std::wstring>& deviceDisplayNameMarshaller) const = 0;
	virtual bool GetDeviceInstanceNameInternal(IDevice* device, const InteropSupport::ISTLObjectTarget<std::wstring>& deviceInstanceNameMarshaller) const = 0;
	virtual bool GetFullyQualifiedDeviceDisplayNameInternal(IDevice* device, const InteropSupport::ISTLObjectTarget<std::wstring>& fullyQualifiedDeviceDisplayNameMarshaller) const = 0;

	//Loaded extension info functions
	virtual void GetLoadedExtensionsInternal(const InteropSupport::ISTLObjectTarget<std::list<IExtension*>>& marshaller) const = 0;
};

#include "ISystemExtensionInterface.inl"
#endif
