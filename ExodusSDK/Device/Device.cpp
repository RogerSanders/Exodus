#include "Device.h"
#include "DeviceInterface/DeviceInterface.pkg"
//##DEBUG##
#include <iostream>
#include <iomanip>

//----------------------------------------------------------------------------------------
//Initialization functions
//----------------------------------------------------------------------------------------
bool Device::BindToSystemInterface(ISystemDeviceInterface* asystemInterface)
{
	systemInterface = asystemInterface;
	return true;
}

//----------------------------------------------------------------------------------------
bool Device::BindToDeviceContext(IDeviceContext* adeviceContext)
{
	deviceContext = adeviceContext;
	return true;
}

//----------------------------------------------------------------------------------------
bool Device::Construct(IHierarchicalStorageNode& node)
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
bool Device::AddReference(const MarshalSupport::Marshal::In<std::wstring>& referenceName, IDevice* target)
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Device::AddReference(const MarshalSupport::Marshal::In<std::wstring>& referenceName, IExtension* target)
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Device::AddReference(const MarshalSupport::Marshal::In<std::wstring>& referenceName, IBusInterface* target)
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Device::AddReference(const MarshalSupport::Marshal::In<std::wstring>& referenceName, IClockSource* target)
{
	return false;
}

//----------------------------------------------------------------------------------------
void Device::RemoveReference(IDevice* target)
{}

//----------------------------------------------------------------------------------------
void Device::RemoveReference(IExtension* target)
{}

//----------------------------------------------------------------------------------------
void Device::RemoveReference(IBusInterface* target)
{}

//----------------------------------------------------------------------------------------
void Device::RemoveReference(IClockSource* target)
{}

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
	return UpdateMethod::None;
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
void Device::NotifyAfterExecuteStepFinishedTimeslice()
{}

//----------------------------------------------------------------------------------------
//Name functions
//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> Device::GetDeviceClassName() const
{
	return implementationName;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> Device::GetDeviceInstanceName() const
{
	return instanceName;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> Device::GetFullyQualifiedDeviceInstanceName() const
{
	return deviceContext->GetFullyQualifiedDeviceInstanceName();
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> Device::GetModuleDisplayName() const
{
	return deviceContext->GetModuleDisplayName();
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> Device::GetModuleInstanceName() const
{
	return deviceContext->GetModuleInstanceName();
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
void Device::LoadState(IHierarchicalStorageNode& node)
{}

//----------------------------------------------------------------------------------------
void Device::SaveState(IHierarchicalStorageNode& node) const
{}

//----------------------------------------------------------------------------------------
void Device::LoadPersistentState(IHierarchicalStorageNode& node)
{}

//----------------------------------------------------------------------------------------
void Device::SavePersistentState(IHierarchicalStorageNode& node) const
{}

//----------------------------------------------------------------------------------------
void Device::LoadSettingsState(IHierarchicalStorageNode& node)
{}

//----------------------------------------------------------------------------------------
void Device::SaveSettingsState(IHierarchicalStorageNode& node) const
{}

//----------------------------------------------------------------------------------------
void Device::LoadDebuggerState(IHierarchicalStorageNode& node)
{}

//----------------------------------------------------------------------------------------
void Device::SaveDebuggerState(IHierarchicalStorageNode& node) const
{}

//----------------------------------------------------------------------------------------
//CE line state functions
//----------------------------------------------------------------------------------------
unsigned int Device::GetCELineID(const MarshalSupport::Marshal::In<std::wstring>& lineName, bool inputLine) const
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
unsigned int Device::CalculateCELineStateMemory(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext, double accessTime) const
{
	return 0;
}

//----------------------------------------------------------------------------------------
unsigned int Device::CalculateCELineStateMemoryTransparent(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext) const
{
	return 0;
}

//----------------------------------------------------------------------------------------
unsigned int Device::CalculateCELineStatePort(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext, double accessTime) const
{
	return 0;
}

//----------------------------------------------------------------------------------------
unsigned int Device::CalculateCELineStatePortTransparent(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext) const
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
unsigned int Device::GetLineID(const MarshalSupport::Marshal::In<std::wstring>& lineName) const
{
	return 0;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> Device::GetLineName(unsigned int lineID) const
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
void Device::TransparentSetLineState(unsigned int targetLine, const Data& lineData)
{}

//----------------------------------------------------------------------------------------
void Device::RevokeSetLineState(unsigned int targetLine, const Data& lineData, double reportedTime, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	//##DEBUG##
	std::wcout << "WARNING! Called RevokeSetLineState on a device which has not defined this function.\n";
}

//----------------------------------------------------------------------------------------
bool Device::AdvanceToLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	return false;
}

//----------------------------------------------------------------------------------------
void Device::AssertCurrentOutputLineState() const
{}

//----------------------------------------------------------------------------------------
void Device::NegateCurrentOutputLineState() const
{}

//----------------------------------------------------------------------------------------
//Clock source functions
//----------------------------------------------------------------------------------------
unsigned int Device::GetClockSourceID(const MarshalSupport::Marshal::In<std::wstring>& clockSourceName) const
{
	return 0;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> Device::GetClockSourceName(unsigned int clockSourceID) const
{
	return L"";
}

//----------------------------------------------------------------------------------------
void Device::SetClockSourceRate(unsigned int clockInput, double clockRate, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{}

//----------------------------------------------------------------------------------------
void Device::TransparentSetClockSourceRate(unsigned int clockInput, double clockRate)
{}

//----------------------------------------------------------------------------------------
//Input functions
//----------------------------------------------------------------------------------------
unsigned int Device::GetKeyCodeID(const MarshalSupport::Marshal::In<std::wstring>& keyCodeName) const
{
	return 0;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> Device::GetKeyCodeName(unsigned int keyCodeID) const
{
	return L"";
}

//----------------------------------------------------------------------------------------
void Device::HandleInputKeyDown(unsigned int keyCodeID)
{}

//----------------------------------------------------------------------------------------
void Device::HandleInputKeyUp(unsigned int keyCodeID)
{}
