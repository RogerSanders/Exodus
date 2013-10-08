#ifndef __IEXTENSION_H__
#define __IEXTENSION_H__
#include "InteropSupport/InteropSupport.pkg"
#include "IBusInterface.h"
#include "IClockSource.h"
#include "IMenuSegment.h"
#include "ISystemExtensionInterface.h"
#include "IGUIExtensionInterface.h"
#include <string>
class IDevice;

class IExtension
{
public:
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
	virtual bool Construct(IHeirarchicalStorageNode& node) = 0;
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
	inline void RestoreViewModelState(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher);
	inline void OpenViewModel(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IViewModelLauncher& viewModelLauncher);

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
	virtual void RestoreViewModelStateInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewModelGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewModelNameMarshaller, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher) = 0;
	virtual void OpenViewModelInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewModelGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewModelNameMarshaller, IViewModelLauncher& viewModelLauncher) = 0;
};

#include "IExtension.inl"
#endif
