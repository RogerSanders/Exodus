#ifndef __IDEVICE_H__
#define __IDEVICE_H__
#include "IBusInterface.h"
#include "IClockSource.h"
#include "ImageInterface/ImageInterface.pkg"
#include "MarshalSupport/MarshalSupport.pkg"
#include "HierarchicalStorageInterface/HierarchicalStorageInterface.pkg"
#include <string>
#include <list>
class Data;
class ISystemDeviceInterface;
class IDeviceContext;
class IExtension;
class IViewManager;

class IDevice
{
public:
	//Enumerations
	enum class UpdateMethod;

public:
	//Constructors
	virtual ~IDevice() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIDeviceVersion() { return 1; }
	virtual unsigned int GetIDeviceVersion() const = 0;

	//Initialization functions
	virtual bool BindToSystemInterface(ISystemDeviceInterface* asystemInterface) = 0;
	virtual bool BindToDeviceContext(IDeviceContext* adeviceContext) = 0;
	virtual bool Construct(IHierarchicalStorageNode& node) = 0;
	virtual bool BuildDevice() = 0;
	virtual bool ValidateDevice() = 0;
	virtual void Initialize() = 0;

	//Reference functions
	virtual bool AddReference(const MarshalSupport::Marshal::In<std::wstring>& referenceName, IDevice* target) = 0;
	virtual bool AddReference(const MarshalSupport::Marshal::In<std::wstring>& referenceName, IExtension* target) = 0;
	virtual bool AddReference(const MarshalSupport::Marshal::In<std::wstring>& referenceName, IBusInterface* target) = 0;
	virtual bool AddReference(const MarshalSupport::Marshal::In<std::wstring>& referenceName, IClockSource* target) = 0;
	virtual void RemoveReference(IDevice* target) = 0;
	virtual void RemoveReference(IExtension* target) = 0;
	virtual void RemoveReference(IBusInterface* target) = 0;
	virtual void RemoveReference(IClockSource* target) = 0;

	//Device context functions
	//##FIX## Make this return a reference rather than a pointer
	virtual IDeviceContext* GetDeviceContext() const = 0;
	virtual double GetCurrentTimesliceProgress() const = 0;

	//Suspend functions
	virtual bool UsesExecuteSuspend() const = 0;
	virtual bool UsesTransientExecution() const = 0;

	//Execute functions
	virtual void BeginExecution() = 0;
	virtual void SuspendExecution() = 0;
	virtual double ExecuteStep() = 0;
	virtual void ExecuteTimeslice(double nanoseconds) = 0;
	virtual void ExecuteTimesliceTimingPointStep(unsigned int accessContext) = 0;
	virtual double GetNextTimingPointInDeviceTime(unsigned int& accessContext) const = 0;
	virtual void ExecuteRollback() = 0;
	virtual void ExecuteCommit() = 0;
	virtual UpdateMethod GetUpdateMethod() const = 0;
	virtual bool SendNotifyUpcomingTimeslice() const = 0;
	virtual void NotifyUpcomingTimeslice(double nanoseconds) = 0;
	virtual bool SendNotifyBeforeExecuteCalled() const = 0;
	virtual void NotifyBeforeExecuteCalled() = 0;
	virtual bool SendNotifyAfterExecuteCalled() const = 0;
	virtual void NotifyAfterExecuteCalled() = 0;
	virtual void NotifyAfterExecuteStepFinishedTimeslice() = 0;

	//Name functions
	//##TODO## Here are the rules for display name generation for a device or extension:
	//-The display name for a device is exactly what is specified in the module definition
	//-Where no display name has been specified for a device, the display name is the
	//instance name
	//-Where only one instance of a module which contains a given device is loaded, and
	//that module doesn't import any connectors, the fully qualified display name for a
	//device is the display name.
	//-Where multiple instances of a module are loaded which contain a given device, the
	//fully qualified display name for the device is the display name, followed by a
	//period, followed by the display name for the containing module.
	//-Where the parent module imports a connector, and there is more than one connector
	//in the loaded system which is compatible, the fully qualified display name for a
	//device is the display name, followed by the parent module name, followed by a space,
	//followed by the imported connector name surrounded in braces.
	//##TODO## Remove all these name functions from the device interface entirely, and
	//only store this data and allow retrieval of it through the system interface.
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetDeviceClassName() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetDeviceInstanceName() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetFullyQualifiedDeviceInstanceName() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetModuleDisplayName() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetModuleInstanceName() const = 0;
	virtual unsigned int GetDeviceModuleID() const = 0;

	//Savestate functions
	virtual bool GetScreenshot(IImage& image) const = 0;
	virtual void LoadState(IHierarchicalStorageNode& node) = 0;
	virtual void SaveState(IHierarchicalStorageNode& node) const = 0;
	virtual void LoadPersistentState(IHierarchicalStorageNode& node) = 0;
	virtual void SavePersistentState(IHierarchicalStorageNode& node) const = 0;
	virtual void LoadSettingsState(IHierarchicalStorageNode& node) = 0;
	virtual void SaveSettingsState(IHierarchicalStorageNode& node) const = 0;
	virtual void LoadDebuggerState(IHierarchicalStorageNode& node) = 0;
	virtual void SaveDebuggerState(IHierarchicalStorageNode& node) const = 0;

	//CE line state functions
	virtual unsigned int GetCELineID(const MarshalSupport::Marshal::In<std::wstring>& lineName, bool inputLine) const = 0;
	virtual void SetCELineInput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber) = 0;
	virtual void SetCELineOutput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber) = 0;
	virtual unsigned int CalculateCELineStateMemory(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext, double accessTime) const = 0;
	virtual unsigned int CalculateCELineStateMemoryTransparent(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext) const = 0;
	virtual unsigned int CalculateCELineStatePort(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext, double accessTime) const = 0;
	virtual unsigned int CalculateCELineStatePortTransparent(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext) const = 0;

	//Memory functions
	virtual IBusInterface::AccessResult ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext) = 0;
	virtual IBusInterface::AccessResult WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext) = 0;
	virtual void TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext) = 0;
	virtual void TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext) = 0;

	//Port functions
	virtual IBusInterface::AccessResult ReadPort(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext) = 0;
	virtual IBusInterface::AccessResult WritePort(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext) = 0;
	virtual void TransparentReadPort(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext) = 0;
	virtual void TransparentWritePort(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext) = 0;

	//Line functions
	virtual unsigned int GetLineID(const MarshalSupport::Marshal::In<std::wstring>& lineName) const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetLineName(unsigned int lineID) const = 0;
	virtual unsigned int GetLineWidth(unsigned int lineID) const = 0;
	virtual void SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext) = 0;
	virtual void TransparentSetLineState(unsigned int targetLine, const Data& lineData) = 0;
	virtual void RevokeSetLineState(unsigned int targetLine, const Data& lineData, double reportedTime, IDeviceContext* caller, double accessTime, unsigned int accessContext) = 0;
	virtual bool AdvanceToLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext) = 0;
	virtual void AssertCurrentOutputLineState() const = 0;
	virtual void NegateCurrentOutputLineState() const = 0;

	//Clock source functions
	virtual unsigned int GetClockSourceID(const MarshalSupport::Marshal::In<std::wstring>& clockSourceName) const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetClockSourceName(unsigned int clockSourceID) const = 0;
	virtual void SetClockSourceRate(unsigned int clockInput, double clockRate, IDeviceContext* caller, double accessTime, unsigned int accessContext) = 0;
	virtual void TransparentSetClockSourceRate(unsigned int clockInput, double clockRate) = 0;

	//Input functions
	virtual unsigned int GetKeyCodeID(const MarshalSupport::Marshal::In<std::wstring>& keyCodeName) const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetKeyCodeName(unsigned int keyCodeID) const = 0;
	virtual void HandleInputKeyDown(unsigned int keyCodeID) = 0;
	virtual void HandleInputKeyUp(unsigned int keyCodeID) = 0;
};

#include "IDevice.inl"
#endif
