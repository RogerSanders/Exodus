#ifndef __IEXTENSION_H__
#define __IEXTENSION_H__
#include <string>
#include "IBusInterface.h"
#include "IClockSource.h"
#include "IMenuSegment.h"
#include "ISystemExtensionInterface.h"
#include "IGUIExtensionInterface.h"
class IDevice;

class IExtension
{
public:
	//Typedefs
	typedef void* AssemblyHandle;

public:
	//Constructors
	virtual ~IExtension() = 0 {}

	//Initialization functions
	virtual bool BindToSystemInterface(ISystemExtensionInterface* asystemInterface) = 0;
	virtual bool BindToGUIInterface(IGUIExtensionInterface* aguiInterface) = 0;
	virtual bool Construct(IHeirarchicalStorageNode& node) = 0;
	virtual bool BuildExtension() = 0;
	virtual bool ValidateExtension() = 0;

	//Reference functions
	inline bool AddReference(const std::wstring& referenceName, IDevice* target);
	virtual bool AddReference(const wchar_t* referenceName, IDevice* target) = 0;
	inline bool AddReference(const std::wstring& referenceName, IExtension* target);
	virtual bool AddReference(const wchar_t* referenceName, IExtension* target) = 0;
	inline bool AddReference(const std::wstring& referenceName, IBusInterface* target);
	virtual bool AddReference(const wchar_t* referenceName, IBusInterface* target) = 0;
	inline bool AddReference(const std::wstring& referenceName, IClockSource* target);
	virtual bool AddReference(const wchar_t* referenceName, IClockSource* target) = 0;
	virtual bool RemoveReference(IDevice* target) = 0;
	virtual bool RemoveReference(IExtension* target) = 0;
	virtual bool RemoveReference(IBusInterface* target) = 0;
	virtual bool RemoveReference(IClockSource* target) = 0;

	//Name functions
	inline std::wstring GetExtensionClassName() const;
	inline std::wstring GetExtensionInstanceName() const;
	virtual unsigned int GetExtensionModuleID() const = 0;

	//Savestate functions
	virtual void LoadSettingsState(IHeirarchicalStorageNode& node) = 0;
	virtual void SaveSettingsState(IHeirarchicalStorageNode& node) const = 0;
	virtual void LoadDebuggerState(IHeirarchicalStorageNode& node) = 0;
	virtual void SaveDebuggerState(IHeirarchicalStorageNode& node) const = 0;

	//Window functions
	virtual void SetAssemblyHandle(AssemblyHandle aassemblyHandle) = 0;
	virtual AssemblyHandle GetAssemblyHandle() const = 0;
	virtual void AddFileOpenMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher) = 0;
	virtual void AddSettingsMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher) = 0;
	virtual void AddGlobalSettingsMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher) = 0;
	inline void RestoreViewModelState(const std::wstring& menuHandlerName, int viewModelID, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher);
	virtual void RestoreViewModelState(const wchar_t* menuHandlerName, int viewModelID, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher) = 0;
	inline void OpenViewModel(const std::wstring& menuHandlerName, int viewModelID, IViewModelLauncher& viewModelLauncher);
	virtual void OpenViewModel(const wchar_t* menuHandlerName, int viewModelID, IViewModelLauncher& viewModelLauncher) = 0;

protected:
	//Name functions
	virtual const wchar_t* GetExtensionClassNameInternal() const = 0;
	virtual const wchar_t* GetExtensionInstanceNameInternal() const = 0;
};

#include "IExtension.inl"
#endif
