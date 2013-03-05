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
bool Extension::AddReferenceInternal(const wchar_t* referenceName, IDevice* target)
{
	return AddReference(referenceName, target);
}

//----------------------------------------------------------------------------------------
bool Extension::AddReferenceInternal(const wchar_t* referenceName, IExtension* target)
{
	return AddReference(referenceName, target);
}

//----------------------------------------------------------------------------------------
bool Extension::AddReferenceInternal(const wchar_t* referenceName, IBusInterface* target)
{
	return AddReference(referenceName, target);
}

//----------------------------------------------------------------------------------------
bool Extension::AddReferenceInternal(const wchar_t* referenceName, IClockSource* target)
{
	return AddReference(referenceName, target);
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
const wchar_t* Extension::GetExtensionClassNameInternal() const
{
	return className.c_str();
}

//----------------------------------------------------------------------------------------
const wchar_t* Extension::GetExtensionInstanceNameInternal() const
{
	return instanceName.c_str();
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
void Extension::RestoreViewModelStateInternal(const wchar_t* viewModelGroupName, const wchar_t* viewModelName, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher)
{
	RestoreViewModelState(viewModelGroupName, viewModelName, node, xpos, ypos, width, height, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void Extension::OpenViewModelInternal(const wchar_t* viewModelGroupName, const wchar_t* viewModelName, IViewModelLauncher& viewModelLauncher)
{
	OpenViewModel(viewModelGroupName, viewModelName, viewModelLauncher);
}
