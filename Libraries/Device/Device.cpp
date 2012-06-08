#include "Device.h"
#include "HeirarchicalStorageInterface/HeirarchicalStorageInterface.pkg"
#include "SystemInterface/SystemInterface.pkg"
//##DEBUG##
#include <iostream>
#include <iomanip>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Device::~Device()
{
	delete deviceContext;
}

//----------------------------------------------------------------------------------------
//Initialization functions
//----------------------------------------------------------------------------------------
bool Device::BindToDeviceContext(IDeviceContext* adeviceContext)
{
	deviceContext = adeviceContext;
	return true;
}

//----------------------------------------------------------------------------------------
bool Device::Construct(IHeirarchicalStorageNode& node)
{
	return true;
}

//----------------------------------------------------------------------------------------
bool Device::BuildDevice()
{
	return true;
}

//----------------------------------------------------------------------------------------
bool Device::ValidateDevice()
{
	return true;
}

//----------------------------------------------------------------------------------------
void Device::Initialize()
{}

//----------------------------------------------------------------------------------------
//Reference functions
//----------------------------------------------------------------------------------------
bool Device::AddReference(const wchar_t* referenceName, IBusInterface* target)
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Device::AddReference(const wchar_t* referenceName, IDevice* target)
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Device::RemoveReference(IBusInterface* target)
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Device::RemoveReference(IDevice* target)
{
	return false;
}

//----------------------------------------------------------------------------------------
//Device context functions
//----------------------------------------------------------------------------------------
IDeviceContext* Device::GetDeviceContext() const
{
	return deviceContext;
}

//----------------------------------------------------------------------------------------
double Device::GetCurrentTimesliceProgress() const
{
	return deviceContext->GetCurrentTimesliceProgress();
}

//----------------------------------------------------------------------------------------
//Suspend functions
//----------------------------------------------------------------------------------------
bool Device::UsesExecuteSuspend() const
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Device::UsesTransientExecution() const
{
	return false;
}

//----------------------------------------------------------------------------------------
//Execute functions
//----------------------------------------------------------------------------------------
//The BeginExecution and SuspendExecution functions are called when the system starts and
//stops actively executing. These notifications are triggered when the system is paused
//and resumed, as well as when a system is first booting to when the system is unloaded.
//These hooks are intended to be used by advanced devices which may have separate
//threads which are spawned, and run independently of the main execution thread of the
//device.
//----------------------------------------------------------------------------------------
void Device::BeginExecution()
{}

//----------------------------------------------------------------------------------------
void Device::SuspendExecution()
{}

//----------------------------------------------------------------------------------------
double Device::ExecuteStep()
{
	//##TODO## Replace this with an error logging event.
	//##TODO## Implement a critical error system abort option, which instructs the system
	//to halt execution when it is raised, and instruct the user to check the event log to
	//determine the cause of the critical error. Note that it should be possible to
	//disable the halt feature in the system options.
	//##DEBUG##
	std::wcout << "WARNING! Called ExecuteStep on a device which has not defined this function.\n";
	return 0;
}

//----------------------------------------------------------------------------------------
void Device::ExecuteTimeslice(double nanoseconds)
{
	//##TODO## Replace this with an error logging event.
	//##TODO## Implement a critical error system abort option, which instructs the system
	//to halt execution when it is raised, and instruct the user to check the event log to
	//determine the cause of the critical error. Note that it should be possible to
	//disable the halt feature in the system options.
	//##DEBUG##
	std::wcout << "WARNING! Called ExecuteTimeslice on a device which has not defined this function.\n";
}

//----------------------------------------------------------------------------------------
void Device::ExecuteTimesliceTimingPointStep(unsigned int accessContext)
{
	//##TODO## Replace this with an error logging event.
	//##TODO## Implement a critical error system abort option, which instructs the system
	//to halt execution when it is raised, and instruct the user to check the event log to
	//determine the cause of the critical error. Note that it should be possible to
	//disable the halt feature in the system options.
	//##DEBUG##
	std::wcout << "WARNING! Called ExecuteTimesliceTimingPointStep on a device which has not defined this function.\n";
}

//----------------------------------------------------------------------------------------
double Device::GetNextTimingPointInDeviceTime(unsigned int& accessContext) const
{
	return -1.0;
}

//----------------------------------------------------------------------------------------
void Device::ExecuteRollback()
{}

//----------------------------------------------------------------------------------------
void Device::ExecuteCommit()
{}

//----------------------------------------------------------------------------------------
Device::UpdateMethod Device::GetUpdateMethod() const
{
	return UPDATEMETHOD_NONE;
}

//----------------------------------------------------------------------------------------
bool Device::SendNotifyUpcomingTimeslice() const
{
	return false;
}

//----------------------------------------------------------------------------------------
//This notification is sent to all devices before each separate timeslice is sent to any
//devices for execution. It was implemented to support the RandomTime containers, which
//require this information.
//----------------------------------------------------------------------------------------
void Device::NotifyUpcomingTimeslice(double nanoseconds)
{}

//----------------------------------------------------------------------------------------
bool Device::SendNotifyBeforeExecuteCalled() const
{
	return false;
}

//----------------------------------------------------------------------------------------
void Device::NotifyBeforeExecuteCalled()
{}

//----------------------------------------------------------------------------------------
bool Device::SendNotifyAfterExecuteCalled() const
{
	return false;
}

//----------------------------------------------------------------------------------------
//This notification is sent to all devices after all devices have reported that they have
//completed execution. It was implemented to more easily support separate device worker
//threads which are triggered during the execution process. This function allows a device
//to ensure any worker threads separate from the main execution step have completed their
//work before the system advances.
//----------------------------------------------------------------------------------------
void Device::NotifyAfterExecuteCalled()
{}

//----------------------------------------------------------------------------------------
//Name functions
//----------------------------------------------------------------------------------------
const wchar_t* Device::GetDeviceClassNameInternal() const
{
	return className.c_str();
}

//----------------------------------------------------------------------------------------
const wchar_t* Device::GetDeviceInstanceNameInternal() const
{
	return instanceName.c_str();
}

//----------------------------------------------------------------------------------------
const wchar_t* Device::GetModuleDisplayNameInternal() const
{
	moduleDisplayNameCached = deviceContext->GetModuleDisplayName();
	return moduleDisplayNameCached.c_str();
}

//----------------------------------------------------------------------------------------
unsigned int Device::GetDeviceModuleID() const
{
	return moduleID;
}

//----------------------------------------------------------------------------------------
//Savestate functions
//----------------------------------------------------------------------------------------
bool Device::GetScreenshot(IImage& image) const
{
	return false;
}

//----------------------------------------------------------------------------------------
void Device::LoadState(IHeirarchicalStorageNode& node)
{}

//----------------------------------------------------------------------------------------
void Device::GetState(IHeirarchicalStorageNode& node) const
{}

//----------------------------------------------------------------------------------------
void Device::SaveDebuggerState(IHeirarchicalStorageNode& node) const
{}

//----------------------------------------------------------------------------------------
void Device::LoadDebuggerState(IHeirarchicalStorageNode& node)
{}

//----------------------------------------------------------------------------------------
//CE line state functions
//----------------------------------------------------------------------------------------
unsigned int Device::GetCELineID(const wchar_t* lineName, bool inputLine) const
{
	return 0;
}

//----------------------------------------------------------------------------------------
void Device::SetCELineInput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber)
{}

//----------------------------------------------------------------------------------------
void Device::SetCELineOutput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber)
{}

//----------------------------------------------------------------------------------------
unsigned int Device::CalculateCELineStateMemory(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, double accessTime) const
{
	return 0;
}

//----------------------------------------------------------------------------------------
unsigned int Device::CalculateCELineStateMemoryTransparent(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller) const
{
	return 0;
}

//----------------------------------------------------------------------------------------
unsigned int Device::CalculateCELineStatePort(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, double accessTime) const
{
	return 0;
}

//----------------------------------------------------------------------------------------
unsigned int Device::CalculateCELineStatePortTransparent(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller) const
{
	return 0;
}

//----------------------------------------------------------------------------------------
//Memory functions
//----------------------------------------------------------------------------------------
IBusInterface::AccessResult Device::ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	return true;
}

//----------------------------------------------------------------------------------------
IBusInterface::AccessResult Device::WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	return true;
}

//----------------------------------------------------------------------------------------
void Device::TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext)
{}

//----------------------------------------------------------------------------------------
void Device::TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext)
{}

//----------------------------------------------------------------------------------------
//Port functions
//----------------------------------------------------------------------------------------
IBusInterface::AccessResult Device::ReadPort(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	return true;
}

//----------------------------------------------------------------------------------------
IBusInterface::AccessResult Device::WritePort(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	return true;
}

//----------------------------------------------------------------------------------------
void Device::TransparentReadPort(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext)
{}

//----------------------------------------------------------------------------------------
void Device::TransparentWritePort(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext)
{}

//----------------------------------------------------------------------------------------
//Line functions
//----------------------------------------------------------------------------------------
unsigned int Device::GetLineID(const wchar_t* lineName) const
{
	return 0;
}

//----------------------------------------------------------------------------------------
const wchar_t* Device::GetLineName(unsigned int lineID) const
{
	return L"";
}

//----------------------------------------------------------------------------------------
unsigned int Device::GetLineWidth(unsigned int lineID) const
{
	return 0;
}

//----------------------------------------------------------------------------------------
void Device::SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{}

//----------------------------------------------------------------------------------------
//Clock source functions
//----------------------------------------------------------------------------------------
unsigned int Device::GetClockSourceID(const wchar_t* lineName) const
{
	return 0;
}

//----------------------------------------------------------------------------------------
const wchar_t* Device::GetClockSourceName(unsigned int lineID) const
{
	return L"";
}

//----------------------------------------------------------------------------------------
void Device::SetClockSourceRate(unsigned int clockInput, double clockRate, IDeviceContext* caller, double accessTime)
{}

//----------------------------------------------------------------------------------------
//Input functions
//----------------------------------------------------------------------------------------
unsigned int Device::GetKeyCodeID(const wchar_t* keyCodeName) const
{
	return 0;
}

//----------------------------------------------------------------------------------------
void Device::HandleInputKeyDown(unsigned int keyCode)
{}

//----------------------------------------------------------------------------------------
void Device::HandleInputKeyUp(unsigned int keyCode)
{}

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
void Device::SetAssemblyHandle(AssemblyHandle aassemblyHandle)
{
	assemblyHandle = aassemblyHandle;
}

//----------------------------------------------------------------------------------------
Device::AssemblyHandle Device::GetAssemblyHandle() const
{
	return assemblyHandle;
}

//----------------------------------------------------------------------------------------
void Device::AddGlobalDebugMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher)
{}

//----------------------------------------------------------------------------------------
void Device::AddDebugMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher)
{}

//----------------------------------------------------------------------------------------
void Device::RestoreViewModelState(const wchar_t* menuHandlerName, int viewModelID, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher)
{
	std::wstring menuHandlerNameString = menuHandlerName;
	RestoreViewModelState(menuHandlerNameString, viewModelID, node, xpos, ypos, width, height, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void Device::RestoreViewModelState(const std::wstring& menuHandlerName, int viewModelID, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher)
{}

//----------------------------------------------------------------------------------------
void Device::OpenViewModel(const wchar_t* menuHandlerName, int viewModelID, IViewModelLauncher& viewModelLauncher)
{
	std::wstring menuHandlerNameString = menuHandlerName;
	OpenViewModel(menuHandlerNameString, viewModelID, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void Device::OpenViewModel(const std::wstring& menuHandlerName, int viewModelID, IViewModelLauncher& viewModelLauncher)
{}
