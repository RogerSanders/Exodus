#ifndef __IEXTENSION_H__
#define __IEXTENSION_H__
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "MarshalSupport/MarshalSupport.pkg"
#include "AssemblyHandle.h"
#include "IMenuSegment.h"
#include "ISystemExtensionInterface.h"
#include "IGUIExtensionInterface.h"
#include <string>
class IDevice;

class IExtension
{
public:
	//Enumerations
	enum class SystemMenu;
	enum class ModuleMenu;
	enum class DeviceMenu;
	enum class ExtensionMenu;

	//Typedefs
	typedef void* AssemblyHandle;

public:
	//Constructors
	virtual ~IExtension() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIExtensionVersion() { return 1; }
	virtual unsigned int GetIExtensionVersion() const = 0;

	//Initialization functions
	virtual bool BindToSystemInterface(ISystemExtensionInterface* asystemInterface) = 0;
	virtual bool BindToGUIInterface(IGUIExtensionInterface* aguiInterface) = 0;
	virtual bool Construct(IHierarchicalStorageNode& node) = 0;
	virtual bool BuildExtension() = 0;
	virtual bool ValidateExtension() = 0;

	//Reference functions
	virtual bool AddReference(const MarshalSupport::Marshal::In<std::wstring>& referenceName, IDevice* target) = 0;
	virtual bool AddReference(const MarshalSupport::Marshal::In<std::wstring>& referenceName, IExtension* target) = 0;
	virtual bool AddReference(const MarshalSupport::Marshal::In<std::wstring>& referenceName, IBusInterface* target) = 0;
	virtual bool AddReference(const MarshalSupport::Marshal::In<std::wstring>& referenceName, IClockSource* target) = 0;
	virtual bool RemoveReference(IDevice* target) = 0;
	virtual bool RemoveReference(IExtension* target) = 0;
	virtual bool RemoveReference(IBusInterface* target) = 0;
	virtual bool RemoveReference(IClockSource* target) = 0;

	//Name functions
	//##TODO## Add a way here to determine if this is a global extension
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetExtensionClassName() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetExtensionInstanceName() const = 0;
	virtual unsigned int GetExtensionModuleID() const = 0;

	//Savestate functions
	virtual void LoadSettingsState(IHierarchicalStorageNode& node) = 0;
	virtual void SaveSettingsState(IHierarchicalStorageNode& node) const = 0;
	virtual void LoadDebuggerState(IHierarchicalStorageNode& node) = 0;
	virtual void SaveDebuggerState(IHierarchicalStorageNode& node) const = 0;

	//Window functions
	virtual AssemblyHandle GetAssemblyHandle() const = 0;
	virtual void SetAssemblyHandle(AssemblyHandle aassemblyHandle) = 0;
	virtual bool RegisterSystemMenuHandler() = 0;
	virtual void UnregisterSystemMenuHandler() = 0;
	virtual bool RegisterModuleMenuHandler(unsigned int moduleID) = 0;
	virtual void UnregisterModuleMenuHandler(unsigned int moduleID) = 0;
	virtual bool RegisterDeviceMenuHandler(IDevice* targetDevice) = 0;
	virtual void UnregisterDeviceMenuHandler(IDevice* targetDevice) = 0;
	virtual bool RegisterExtensionMenuHandler(IExtension* targetExtension) = 0;
	virtual void UnregisterExtensionMenuHandler(IExtension* targetExtension) = 0;
	virtual void AddSystemMenuItems(SystemMenu systemMenu, IMenuSegment& menuSegment) = 0;
	virtual void AddModuleMenuItems(ModuleMenu moduleMenu, IMenuSegment& menuSegment, unsigned int moduleID) = 0;
	virtual void AddDeviceMenuItems(DeviceMenu deviceMenu, IMenuSegment& menuSegment, IDevice* targetDevice) = 0;
	virtual void AddExtensionMenuItems(ExtensionMenu extensionMenu, IMenuSegment& menuSegment, IExtension* targetExtension) = 0;
	virtual bool RestoreSystemViewState(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState) = 0;
	virtual bool RestoreModuleViewState(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, unsigned int moduleID) = 0;
	virtual bool RestoreDeviceViewState(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IDevice* targetDevice) = 0;
	virtual bool RestoreExtensionViewState(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IExtension* targetExtension) = 0;
	virtual bool OpenSystemView(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName) = 0;
	virtual bool OpenModuleView(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, unsigned int moduleID) = 0;
	virtual bool OpenDeviceView(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IDevice* targetDevice) = 0;
	virtual bool OpenExtensionView(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IExtension* targetExtension) = 0;

	//##TODO## Add functions to deal with toolbars
};

#include "IExtension.inl"
#endif
