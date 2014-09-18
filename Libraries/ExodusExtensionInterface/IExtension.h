#ifndef __IEXTENSION_H__
#define __IEXTENSION_H__
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "InteropSupport/InteropSupport.pkg"
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
	enum SystemMenu;
	enum ModuleMenu;
	enum DeviceMenu;
	enum ExtensionMenu;

	//Typedefs
	typedef void* AssemblyHandle;

public:
	//Constructors
	virtual ~IExtension() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIExtensionVersion();
	virtual unsigned int GetIExtensionVersion() const = 0;

	//Initialization functions
	virtual bool BindToSystemInterface(ISystemExtensionInterface* asystemInterface) = 0;
	virtual bool BindToGUIInterface(IGUIExtensionInterface* aguiInterface) = 0;
	virtual bool Construct(IHierarchicalStorageNode& node) = 0;
	virtual bool BuildExtension() = 0;
	virtual bool ValidateExtension() = 0;

	//Reference functions
	inline bool AddReference(const std::wstring& referenceName, IDevice* target);
	inline bool AddReference(const std::wstring& referenceName, IExtension* target);
	inline bool AddReference(const std::wstring& referenceName, IBusInterface* target);
	inline bool AddReference(const std::wstring& referenceName, IClockSource* target);
	virtual bool RemoveReference(IDevice* target) = 0;
	virtual bool RemoveReference(IExtension* target) = 0;
	virtual bool RemoveReference(IBusInterface* target) = 0;
	virtual bool RemoveReference(IClockSource* target) = 0;

	//Name functions
	//##TODO## Add a way here to determine if this is a global extension
	inline std::wstring GetExtensionClassName() const;
	inline std::wstring GetExtensionInstanceName() const;
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
	inline bool RestoreSystemViewState(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState);
	inline bool RestoreModuleViewState(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState, unsigned int moduleID);
	inline bool RestoreDeviceViewState(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState, IDevice* targetDevice);
	inline bool RestoreExtensionViewState(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState, IExtension* targetExtension);
	inline bool OpenSystemView(const std::wstring& viewGroupName, const std::wstring& viewName);
	inline bool OpenModuleView(const std::wstring& viewGroupName, const std::wstring& viewName, unsigned int moduleID);
	inline bool OpenDeviceView(const std::wstring& viewGroupName, const std::wstring& viewName, IDevice* targetDevice);
	inline bool OpenExtensionView(const std::wstring& viewGroupName, const std::wstring& viewName, IExtension* targetExtension);

	//##TODO## Add functions to deal with toolbars

protected:
	//Reference functions
	virtual bool AddReferenceInternal(const InteropSupport::ISTLObjectSource<std::wstring>& referenceNameMarshaller, IDevice* target) = 0;
	virtual bool AddReferenceInternal(const InteropSupport::ISTLObjectSource<std::wstring>& referenceNameMarshaller, IExtension* target) = 0;
	virtual bool AddReferenceInternal(const InteropSupport::ISTLObjectSource<std::wstring>& referenceNameMarshaller, IBusInterface* target) = 0;
	virtual bool AddReferenceInternal(const InteropSupport::ISTLObjectSource<std::wstring>& referenceNameMarshaller, IClockSource* target) = 0;

	//Name functions
	virtual void GetExtensionClassNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void GetExtensionInstanceNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;

	//Window functions
	virtual bool RestoreSystemViewStateInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewNameMarshaller, IHierarchicalStorageNode& viewState) = 0;
	virtual bool RestoreModuleViewStateInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewNameMarshaller, IHierarchicalStorageNode& viewState, unsigned int moduleID) = 0;
	virtual bool RestoreDeviceViewStateInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewNameMarshaller, IHierarchicalStorageNode& viewState, IDevice* targetDevice) = 0;
	virtual bool RestoreExtensionViewStateInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewNameMarshaller, IHierarchicalStorageNode& viewState, IExtension* targetExtension) = 0;
	virtual bool OpenSystemViewInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewNameMarshaller) = 0;
	virtual bool OpenModuleViewInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewNameMarshaller, unsigned int moduleID) = 0;
	virtual bool OpenDeviceViewInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewNameMarshaller, IDevice* targetDevice) = 0;
	virtual bool OpenExtensionViewInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewNameMarshaller, IExtension* targetExtension) = 0;
};

#include "IExtension.inl"
#endif
