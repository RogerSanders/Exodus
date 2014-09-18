#ifndef __EXTENSION_H__
#define __EXTENSION_H__
#include "ExodusExtensionInterface/ExodusExtensionInterface.pkg"

class Extension :public IExtension
{
public:
	//Constructors
	inline Extension(const std::wstring& aclassName, const std::wstring& ainstanceName, unsigned int amoduleID);
	virtual ~Extension();

	//Interface version functions
	virtual unsigned int GetIExtensionVersion() const;

	//Initialization functions
	virtual bool BindToSystemInterface(ISystemExtensionInterface* asystemInterface);
	virtual bool BindToGUIInterface(IGUIExtensionInterface* aguiInterface);
	virtual bool Construct(IHierarchicalStorageNode& node);
	virtual bool BuildExtension();
	virtual bool ValidateExtension();

	//Reference functions
	using IExtension::AddReference;
	virtual bool AddReference(const std::wstring& referenceName, IDevice* target);
	virtual bool AddReference(const std::wstring& referenceName, IExtension* target);
	virtual bool AddReference(const std::wstring& referenceName, IBusInterface* target);
	virtual bool AddReference(const std::wstring& referenceName, IClockSource* target);
	virtual bool RemoveReference(IDevice* target);
	virtual bool RemoveReference(IExtension* target);
	virtual bool RemoveReference(IBusInterface* target);
	virtual bool RemoveReference(IClockSource* target);

	//Interface functions
	inline ISystemExtensionInterface& GetSystemInterface() const;
	inline IGUIExtensionInterface& GetGUIInterface() const;
	inline IViewManager& GetViewManager() const;

	//Name functions
	inline std::wstring GetExtensionClassName() const;
	inline std::wstring GetExtensionInstanceName() const;
	virtual unsigned int GetExtensionModuleID() const;

	//Savestate functions
	virtual void LoadSettingsState(IHierarchicalStorageNode& node);
	virtual void SaveSettingsState(IHierarchicalStorageNode& node) const;
	virtual void LoadDebuggerState(IHierarchicalStorageNode& node);
	virtual void SaveDebuggerState(IHierarchicalStorageNode& node) const;

	//Window functions
	virtual AssemblyHandle GetAssemblyHandle() const;
	virtual void SetAssemblyHandle(AssemblyHandle aassemblyHandle);
	virtual bool RegisterSystemMenuHandler();
	virtual void UnregisterSystemMenuHandler();
	virtual bool RegisterModuleMenuHandler(unsigned int moduleID);
	virtual void UnregisterModuleMenuHandler(unsigned int moduleID);
	virtual bool RegisterDeviceMenuHandler(IDevice* targetDevice);
	virtual void UnregisterDeviceMenuHandler(IDevice* targetDevice);
	virtual bool RegisterExtensionMenuHandler(IExtension* targetExtension);
	virtual void UnregisterExtensionMenuHandler(IExtension* targetExtension);
	virtual void AddSystemMenuItems(SystemMenu systemMenu, IMenuSegment& menuSegment);
	virtual void AddModuleMenuItems(ModuleMenu moduleMenu, IMenuSegment& menuSegment, unsigned int moduleID);
	virtual void AddDeviceMenuItems(DeviceMenu deviceMenu, IMenuSegment& menuSegment, IDevice* targetDevice);
	virtual void AddExtensionMenuItems(ExtensionMenu extensionMenu, IMenuSegment& menuSegment, IExtension* targetExtension);
	virtual bool RestoreSystemViewState(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState);
	virtual bool RestoreModuleViewState(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState, unsigned int moduleID);
	virtual bool RestoreDeviceViewState(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState, IDevice* targetDevice);
	virtual bool RestoreExtensionViewState(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState, IExtension* targetExtension);
	virtual bool OpenSystemView(const std::wstring& viewGroupName, const std::wstring& viewName);
	virtual bool OpenModuleView(const std::wstring& viewGroupName, const std::wstring& viewName, unsigned int moduleID);
	virtual bool OpenDeviceView(const std::wstring& viewGroupName, const std::wstring& viewName, IDevice* targetDevice);
	virtual bool OpenExtensionView(const std::wstring& viewGroupName, const std::wstring& viewName, IExtension* targetExtension);

protected:
	//Reference functions
	virtual bool AddReferenceInternal(const InteropSupport::ISTLObjectSource<std::wstring>& referenceNameMarshaller, IDevice* target);
	virtual bool AddReferenceInternal(const InteropSupport::ISTLObjectSource<std::wstring>& referenceNameMarshaller, IExtension* target);
	virtual bool AddReferenceInternal(const InteropSupport::ISTLObjectSource<std::wstring>& referenceNameMarshaller, IBusInterface* target);
	virtual bool AddReferenceInternal(const InteropSupport::ISTLObjectSource<std::wstring>& referenceNameMarshaller, IClockSource* target);

	//Name functions
	virtual void GetExtensionClassNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void GetExtensionInstanceNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;

	//Window functions
	virtual bool RestoreSystemViewStateInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewNameMarshaller, IHierarchicalStorageNode& viewState);
	virtual bool RestoreModuleViewStateInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewNameMarshaller, IHierarchicalStorageNode& viewState, unsigned int moduleID);
	virtual bool RestoreDeviceViewStateInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewNameMarshaller, IHierarchicalStorageNode& viewState, IDevice* targetDevice);
	virtual bool RestoreExtensionViewStateInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewNameMarshaller, IHierarchicalStorageNode& viewState, IExtension* targetExtension);
	virtual bool OpenSystemViewInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewNameMarshaller);
	virtual bool OpenModuleViewInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewNameMarshaller, unsigned int moduleID);
	virtual bool OpenDeviceViewInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewNameMarshaller, IDevice* targetDevice);
	virtual bool OpenExtensionViewInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewNameMarshaller, IExtension* targetExtension);

private:
	std::wstring className;
	std::wstring instanceName;
	unsigned int moduleID;
	AssemblyHandle assemblyHandle;
	ISystemExtensionInterface* systemInterface;
	IGUIExtensionInterface* guiInterface;
	IViewManager* viewManager;
};

#include "Extension.inl"
#endif
