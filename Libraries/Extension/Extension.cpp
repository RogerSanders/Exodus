#include "Extension.h"

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
bool Extension::Construct(IHierarchicalStorageNode& node)
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
bool Extension::AddReference(const MarshalSupport::Marshal::In<std::wstring>& referenceName, IDevice* target)
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Extension::AddReference(const MarshalSupport::Marshal::In<std::wstring>& referenceName, IExtension* target)
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Extension::AddReference(const MarshalSupport::Marshal::In<std::wstring>& referenceName, IBusInterface* target)
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Extension::AddReference(const MarshalSupport::Marshal::In<std::wstring>& referenceName, IClockSource* target)
{
	return false;
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
//Name functions
//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> Extension::GetExtensionClassName() const
{
	return className;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> Extension::GetExtensionInstanceName() const
{
	return instanceName;
}

//----------------------------------------------------------------------------------------
unsigned int Extension::GetExtensionModuleID() const
{
	return moduleID;
}

//----------------------------------------------------------------------------------------
//Savestate functions
//----------------------------------------------------------------------------------------
void Extension::LoadSettingsState(IHierarchicalStorageNode& node)
{}

//----------------------------------------------------------------------------------------
void Extension::SaveSettingsState(IHierarchicalStorageNode& node) const
{}

//----------------------------------------------------------------------------------------
void Extension::LoadDebuggerState(IHierarchicalStorageNode& node)
{}

//----------------------------------------------------------------------------------------
void Extension::SaveDebuggerState(IHierarchicalStorageNode& node) const
{}

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
AssemblyHandle Extension::GetAssemblyHandle() const
{
	return assemblyHandle;
}

//----------------------------------------------------------------------------------------
void Extension::SetAssemblyHandle(AssemblyHandle aassemblyHandle)
{
	assemblyHandle = aassemblyHandle;
}

//----------------------------------------------------------------------------------------
bool Extension::RegisterSystemMenuHandler()
{
	return false;
}

//----------------------------------------------------------------------------------------
void Extension::UnregisterSystemMenuHandler()
{}

//----------------------------------------------------------------------------------------
bool Extension::RegisterModuleMenuHandler(unsigned int moduleID)
{
	return false;
}

//----------------------------------------------------------------------------------------
void Extension::UnregisterModuleMenuHandler(unsigned int moduleID)
{}

//----------------------------------------------------------------------------------------
bool Extension::RegisterDeviceMenuHandler(IDevice* targetDevice)
{
	return false;
}

//----------------------------------------------------------------------------------------
void Extension::UnregisterDeviceMenuHandler(IDevice* targetDevice)
{}

//----------------------------------------------------------------------------------------
bool Extension::RegisterExtensionMenuHandler(IExtension* targetExtension)
{
	return false;
}

//----------------------------------------------------------------------------------------
void Extension::UnregisterExtensionMenuHandler(IExtension* targetExtension)
{}

//----------------------------------------------------------------------------------------
void Extension::AddSystemMenuItems(SystemMenu systemMenu, IMenuSegment& menuSegment)
{}

//----------------------------------------------------------------------------------------
void Extension::AddModuleMenuItems(ModuleMenu moduleMenu, IMenuSegment& menuSegment, unsigned int moduleID)
{}

//----------------------------------------------------------------------------------------
void Extension::AddDeviceMenuItems(DeviceMenu deviceMenu, IMenuSegment& menuSegment, IDevice* targetDevice)
{}

//----------------------------------------------------------------------------------------
void Extension::AddExtensionMenuItems(ExtensionMenu extensionMenu, IMenuSegment& menuSegment, IExtension* targetExtension)
{}

//----------------------------------------------------------------------------------------
bool Extension::RestoreSystemViewState(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IViewPresenter** restoredViewPresenter)
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Extension::RestoreModuleViewState(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IViewPresenter** restoredViewPresenter, unsigned int moduleID)
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Extension::RestoreDeviceViewState(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IViewPresenter** restoredViewPresenter, IDevice* targetDevice)
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Extension::RestoreExtensionViewState(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IViewPresenter** restoredViewPresenter, IExtension* targetExtension)
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Extension::OpenSystemView(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName)
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Extension::OpenModuleView(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, unsigned int moduleID)
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Extension::OpenDeviceView(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IDevice* targetDevice)
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Extension::OpenExtensionView(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IExtension* targetExtension)
{
	return false;
}
