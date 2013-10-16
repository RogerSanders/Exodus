#ifndef __EXTENSION_H__
#define __EXTENSION_H__
#include "SystemInterface/SystemInterface.pkg"

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
	virtual ISystemExtensionInterface* GetSystemInterface() const;
	virtual IGUIExtensionInterface* GetGUIInterface() const;

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
	virtual void SetAssemblyHandle(AssemblyHandle aassemblyHandle);
	virtual AssemblyHandle GetAssemblyHandle() const;
	virtual void AddFileOpenMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher);
	virtual void AddSettingsMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher);
	virtual void AddGlobalSettingsMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher);
	virtual void RestoreViewModelState(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IHierarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher);
	virtual void OpenViewModel(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IViewModelLauncher& viewModelLauncher);

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
	virtual void RestoreViewModelStateInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewModelGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewModelNameMarshaller, IHierarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher);
	virtual void OpenViewModelInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewModelGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewModelNameMarshaller, IViewModelLauncher& viewModelLauncher);

private:
	std::wstring className;
	std::wstring instanceName;
	unsigned int moduleID;
	AssemblyHandle assemblyHandle;
	ISystemExtensionInterface* systemInterface;
	IGUIExtensionInterface* guiInterface;
};

#include "Extension.inl"
#endif
