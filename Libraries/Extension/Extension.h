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
	virtual bool AddReference(const MarshalSupport::Marshal::In<std::wstring>& referenceName, IDevice* target);
	virtual bool AddReference(const MarshalSupport::Marshal::In<std::wstring>& referenceName, IExtension* target);
	virtual bool AddReference(const MarshalSupport::Marshal::In<std::wstring>& referenceName, IBusInterface* target);
	virtual bool AddReference(const MarshalSupport::Marshal::In<std::wstring>& referenceName, IClockSource* target);
	virtual bool RemoveReference(IDevice* target);
	virtual bool RemoveReference(IExtension* target);
	virtual bool RemoveReference(IBusInterface* target);
	virtual bool RemoveReference(IClockSource* target);

	//Interface functions
	inline ISystemExtensionInterface& GetSystemInterface() const;
	inline IGUIExtensionInterface& GetGUIInterface() const;
	inline IViewManager& GetViewManager() const;

	//Name functions
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetExtensionClassName() const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetExtensionInstanceName() const;
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
	virtual bool RestoreSystemViewState(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IViewPresenter** restoredViewPresenter);
	virtual bool RestoreModuleViewState(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IViewPresenter** restoredViewPresenter, unsigned int moduleID);
	virtual bool RestoreDeviceViewState(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IViewPresenter** restoredViewPresenter, IDevice* targetDevice);
	virtual bool RestoreExtensionViewState(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IViewPresenter** restoredViewPresenter, IExtension* targetExtension);
	virtual bool OpenSystemView(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName);
	virtual bool OpenModuleView(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, unsigned int moduleID);
	virtual bool OpenDeviceView(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IDevice* targetDevice);
	virtual bool OpenExtensionView(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IExtension* targetExtension);

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
