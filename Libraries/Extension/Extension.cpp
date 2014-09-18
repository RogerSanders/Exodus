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
bool Extension::RestoreSystemViewState(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState)
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Extension::RestoreModuleViewState(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState, unsigned int moduleID)
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Extension::RestoreDeviceViewState(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState, IDevice* targetDevice)
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Extension::RestoreExtensionViewState(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState, IExtension* targetExtension)
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Extension::OpenSystemView(const std::wstring& viewGroupName, const std::wstring& viewName)
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Extension::OpenModuleView(const std::wstring& viewGroupName, const std::wstring& viewName, unsigned int moduleID)
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Extension::OpenDeviceView(const std::wstring& viewGroupName, const std::wstring& viewName, IDevice* targetDevice)
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Extension::OpenExtensionView(const std::wstring& viewGroupName, const std::wstring& viewName, IExtension* targetExtension)
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Extension::RestoreSystemViewStateInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewNameMarshaller, IHierarchicalStorageNode& viewState)
{
	return RestoreSystemViewState(viewGroupNameMarshaller.MarshalTo(), viewNameMarshaller.MarshalTo(), viewState);
}

//----------------------------------------------------------------------------------------
bool Extension::RestoreModuleViewStateInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewNameMarshaller, IHierarchicalStorageNode& viewState, unsigned int moduleID)
{
	return RestoreModuleViewState(viewGroupNameMarshaller.MarshalTo(), viewNameMarshaller.MarshalTo(), viewState, moduleID);
}

//----------------------------------------------------------------------------------------
bool Extension::RestoreDeviceViewStateInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewNameMarshaller, IHierarchicalStorageNode& viewState, IDevice* targetDevice)
{
	return RestoreDeviceViewState(viewGroupNameMarshaller.MarshalTo(), viewNameMarshaller.MarshalTo(), viewState, targetDevice);
}

//----------------------------------------------------------------------------------------
bool Extension::RestoreExtensionViewStateInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewNameMarshaller, IHierarchicalStorageNode& viewState, IExtension* targetExtension)
{
	return RestoreExtensionViewState(viewGroupNameMarshaller.MarshalTo(), viewNameMarshaller.MarshalTo(), viewState, targetExtension);
}

//----------------------------------------------------------------------------------------
bool Extension::OpenSystemViewInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewNameMarshaller)
{
	return OpenSystemView(viewGroupNameMarshaller.MarshalTo(), viewNameMarshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
bool Extension::OpenModuleViewInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewNameMarshaller, unsigned int moduleID)
{
	return OpenModuleView(viewGroupNameMarshaller.MarshalTo(), viewNameMarshaller.MarshalTo(), moduleID);
}

//----------------------------------------------------------------------------------------
bool Extension::OpenDeviceViewInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewNameMarshaller, IDevice* targetDevice)
{
	return OpenDeviceView(viewGroupNameMarshaller.MarshalTo(), viewNameMarshaller.MarshalTo(), targetDevice);
}

//----------------------------------------------------------------------------------------
bool Extension::OpenExtensionViewInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewNameMarshaller, IExtension* targetExtension)
{
	return OpenExtensionView(viewGroupNameMarshaller.MarshalTo(), viewNameMarshaller.MarshalTo(), targetExtension);
}
