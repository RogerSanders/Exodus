#ifndef __EXTENSION_H__
#define __EXTENSION_H__
#include "SystemInterface/SystemInterface.pkg"

class Extension :public IExtension
{
public:
	//Constructors
	inline Extension(const std::wstring& aclassName, const std::wstring& ainstanceName, unsigned int amoduleID);
	virtual ~Extension();

	//Initialization functions
	virtual bool BindToSystemInterface(ISystemExtensionInterface* asystemInterface);
	virtual bool BindToGUIInterface(IGUIExtensionInterface* aguiInterface);
	virtual bool Construct(IHeirarchicalStorageNode& node);
	virtual bool BuildExtension();
	virtual bool ValidateExtension();

	//Reference functions
	virtual bool AddReference(const wchar_t* referenceName, IDevice* target);
	virtual bool AddReference(const wchar_t* referenceName, IExtension* target);
	virtual bool AddReference(const wchar_t* referenceName, IBusInterface* target);
	virtual bool AddReference(const wchar_t* referenceName, IClockSource* target);
	virtual bool RemoveReference(IDevice* target);
	virtual bool RemoveReference(IExtension* target);
	virtual bool RemoveReference(IBusInterface* target);
	virtual bool RemoveReference(IClockSource* target);

	//Interface functions
	virtual ISystemExtensionInterface* GetSystemInterface() const;
	virtual IGUIExtensionInterface* GetGUIInterface() const;

	//Name functions
	virtual unsigned int GetExtensionModuleID() const;

	//Savestate functions
	virtual void LoadSettingsState(IHeirarchicalStorageNode& node);
	virtual void SaveSettingsState(IHeirarchicalStorageNode& node) const;
	virtual void LoadDebuggerState(IHeirarchicalStorageNode& node);
	virtual void SaveDebuggerState(IHeirarchicalStorageNode& node) const;

	//Window functions
	virtual void SetAssemblyHandle(AssemblyHandle aassemblyHandle);
	virtual AssemblyHandle GetAssemblyHandle() const;
	virtual void AddFileOpenMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher);
	virtual void AddSettingsMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher);
	virtual void AddGlobalSettingsMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher);
	virtual void RestoreViewModelState(const wchar_t* menuHandlerName, int viewModelID, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher);
	virtual void RestoreViewModelState(const std::wstring& menuHandlerName, int viewModelID, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher);
	virtual void OpenViewModel(const wchar_t* menuHandlerName, int viewModelID, IViewModelLauncher& viewModelLauncher);
	virtual void OpenViewModel(const std::wstring& menuHandlerName, int viewModelID, IViewModelLauncher& viewModelLauncher);

protected:
	//Name functions
	virtual const wchar_t* GetExtensionClassNameInternal() const;
	virtual const wchar_t* GetExtensionInstanceNameInternal() const;

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
