#include "Extension.h"
#include "HeirarchicalStorageInterface/HeirarchicalStorageInterface.pkg"
#include "SystemInterface/SystemInterface.pkg"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Extension::~Extension()
{}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int Extension::GetIExtensionVersion() const
{
	return ThisIExtensionVersion();
}

//----------------------------------------------------------------------------------------
//Initialization functions
//----------------------------------------------------------------------------------------
bool Extension::BindToSystemInterface(ISystemExtensionInterface* asystemInterface)
{
	systemInterface = asystemInterface;
	return true;
}

//----------------------------------------------------------------------------------------
bool Extension::BindToGUIInterface(IGUIExtensionInterface* aguiInterface)
{
	guiInterface = aguiInterface;
	return true;
}

//----------------------------------------------------------------------------------------
bool Extension::Construct(IHeirarchicalStorageNode& node)
{
	return true;
}

//----------------------------------------------------------------------------------------
bool Extension::BuildExtension()
{
	return true;
}

//----------------------------------------------------------------------------------------
bool Extension::ValidateExtension()
{
	return true;
}

//----------------------------------------------------------------------------------------
//Reference functions
//----------------------------------------------------------------------------------------
bool Extension::AddReference(const std::wstring& referenceName, IDevice* target)
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Extension::AddReference(const std::wstring& referenceName, IExtension* target)
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Extension::AddReference(const std::wstring& referenceName, IBusInterface* target)
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Extension::AddReference(const std::wstring& referenceName, IClockSource* target)
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Extension::AddReferenceInternal(const InteropSupport::ISTLObjectSource<std::wstring>& referenceNameMarshaller, IDevice* target)
{
	return AddReference(referenceNameMarshaller.MarshalTo(), target);
}

//----------------------------------------------------------------------------------------
bool Extension::AddReferenceInternal(const InteropSupport::ISTLObjectSource<std::wstring>& referenceNameMarshaller, IExtension* target)
{
	return AddReference(referenceNameMarshaller.MarshalTo(), target);
}

//----------------------------------------------------------------------------------------
bool Extension::AddReferenceInternal(const InteropSupport::ISTLObjectSource<std::wstring>& referenceNameMarshaller, IBusInterface* target)
{
	return AddReference(referenceNameMarshaller.MarshalTo(), target);
}

//----------------------------------------------------------------------------------------
bool Extension::AddReferenceInternal(const InteropSupport::ISTLObjectSource<std::wstring>& referenceNameMarshaller, IClockSource* target)
{
	return AddReference(referenceNameMarshaller.MarshalTo(), target);
}

//----------------------------------------------------------------------------------------
bool Extension::RemoveReference(IDevice* target)
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Extension::RemoveReference(IExtension* target)
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Extension::RemoveReference(IBusInterface* target)
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Extension::RemoveReference(IClockSource* target)
{
	return false;
}

//----------------------------------------------------------------------------------------
//Interface functions
//----------------------------------------------------------------------------------------
ISystemExtensionInterface* Extension::GetSystemInterface() const
{
	return systemInterface;
}

//----------------------------------------------------------------------------------------
IGUIExtensionInterface* Extension::GetGUIInterface() const
{
	return guiInterface;
}

//----------------------------------------------------------------------------------------
//Name functions
//----------------------------------------------------------------------------------------
void Extension::GetExtensionClassNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetExtensionClassName());
}

//----------------------------------------------------------------------------------------
void Extension::GetExtensionInstanceNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetExtensionInstanceName());
}

//----------------------------------------------------------------------------------------
unsigned int Extension::GetExtensionModuleID() const
{
	return moduleID;
}

//----------------------------------------------------------------------------------------
//Savestate functions
//----------------------------------------------------------------------------------------
void Extension::LoadSettingsState(IHeirarchicalStorageNode& node)
{}

//----------------------------------------------------------------------------------------
void Extension::SaveSettingsState(IHeirarchicalStorageNode& node) const
{}

//----------------------------------------------------------------------------------------
void Extension::LoadDebuggerState(IHeirarchicalStorageNode& node)
{}

//----------------------------------------------------------------------------------------
void Extension::SaveDebuggerState(IHeirarchicalStorageNode& node) const
{}

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
void Extension::SetAssemblyHandle(AssemblyHandle aassemblyHandle)
{
	assemblyHandle = aassemblyHandle;
}

//----------------------------------------------------------------------------------------
Extension::AssemblyHandle Extension::GetAssemblyHandle() const
{
	return assemblyHandle;
}

//----------------------------------------------------------------------------------------
void Extension::AddFileOpenMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher)
{}

//----------------------------------------------------------------------------------------
void Extension::AddSettingsMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher)
{}

//----------------------------------------------------------------------------------------
void Extension::AddGlobalSettingsMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher)
{}

//----------------------------------------------------------------------------------------
void Extension::RestoreViewModelState(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher)
{}

//----------------------------------------------------------------------------------------
void Extension::OpenViewModel(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IViewModelLauncher& viewModelLauncher)
{}

//----------------------------------------------------------------------------------------
void Extension::RestoreViewModelStateInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewModelGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewModelNameMarshaller, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher)
{
	RestoreViewModelState(viewModelGroupNameMarshaller.MarshalTo(), viewModelNameMarshaller.MarshalTo(), node, xpos, ypos, width, height, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void Extension::OpenViewModelInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewModelGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewModelNameMarshaller, IViewModelLauncher& viewModelLauncher)
{
	OpenViewModel(viewModelGroupNameMarshaller.MarshalTo(), viewModelNameMarshaller.MarshalTo(), viewModelLauncher);
}
