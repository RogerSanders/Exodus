#ifndef __DEVICE_H__
#define __DEVICE_H__
#include "DeviceInternal.h"

class Device :public DeviceInternal
{
public:
	//Constructors
	inline Device(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID);

	//Initialization functions
	virtual bool BindToSystemInterface(ISystemDeviceInterface* asystemInterface);
	virtual bool BindToDeviceContext(IDeviceContext* adeviceContext);
	virtual bool Construct(IHierarchicalStorageNode& node);
	virtual bool BuildDevice();
	virtual bool ValidateDevice();
	virtual void Initialize();

	//Reference functions
	virtual bool AddReference(const MarshalSupport::Marshal::In<std::wstring>& referenceName, IDevice* target);
	virtual bool AddReference(const MarshalSupport::Marshal::In<std::wstring>& referenceName, IExtension* target);
	virtual bool AddReference(const MarshalSupport::Marshal::In<std::wstring>& referenceName, IBusInterface* target);
	virtual bool AddReference(const MarshalSupport::Marshal::In<std::wstring>& referenceName, IClockSource* target);
	virtual bool RemoveReference(IDevice* target);
	virtual bool RemoveReference(IExtension* target);
	virtual bool RemoveReference(IBusInterface* target);
	virtual bool RemoveReference(IClockSource* target);

	//Interface functions
	inline ISystemDeviceInterface& GetSystemInterface() const;

	//Device context functions
	inline IDeviceContext* GetDeviceContext() const;
	inline double GetCurrentTimesliceProgress() const;
	inline void SetCurrentTimesliceProgress(double executionProgress);

	//Suspend functions
	virtual bool UsesExecuteSuspend() const;
	virtual bool UsesTransientExecution() const;

	//Execute functions
	virtual void BeginExecution();
	virtual void SuspendExecution();
	virtual double ExecuteStep();
	virtual void ExecuteTimeslice(double nanoseconds);
	virtual void ExecuteTimesliceTimingPointStep(unsigned int accessContext);
	virtual double GetNextTimingPointInDeviceTime(unsigned int& accessContext) const;
	virtual void ExecuteRollback();
	virtual void ExecuteCommit();
	virtual UpdateMethod GetUpdateMethod() const;
	virtual bool SendNotifyUpcomingTimeslice() const;
	virtual void NotifyUpcomingTimeslice(double nanoseconds);
	virtual bool SendNotifyBeforeExecuteCalled() const;
	virtual void NotifyBeforeExecuteCalled();
	virtual bool SendNotifyAfterExecuteCalled() const;
	virtual void NotifyAfterExecuteCalled();
	virtual void NotifyAfterExecuteStepFinishedTimeslice();

	//Name functions
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetDeviceClassName() const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetDeviceInstanceName() const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetFullyQualifiedDeviceInstanceName() const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetModuleDisplayName() const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetModuleInstanceName() const;
	virtual unsigned int GetDeviceModuleID() const;

	//Savestate functions
	virtual bool GetScreenshot(IImage& image) const;
	virtual void LoadState(IHierarchicalStorageNode& node);
	virtual void SaveState(IHierarchicalStorageNode& node) const;
	virtual void LoadPersistentState(IHierarchicalStorageNode& node);
	virtual void SavePersistentState(IHierarchicalStorageNode& node) const;
	virtual void LoadSettingsState(IHierarchicalStorageNode& node);
	virtual void SaveSettingsState(IHierarchicalStorageNode& node) const;
	virtual void LoadDebuggerState(IHierarchicalStorageNode& node);
	virtual void SaveDebuggerState(IHierarchicalStorageNode& node) const;

	//CE line state functions
	virtual unsigned int GetCELineID(const MarshalSupport::Marshal::In<std::wstring>& lineName, bool inputLine) const;
	virtual void SetCELineInput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber);
	virtual void SetCELineOutput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber);
	virtual unsigned int CalculateCELineStateMemory(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext, double accessTime) const;
	virtual unsigned int CalculateCELineStateMemoryTransparent(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext) const;
	virtual unsigned int CalculateCELineStatePort(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext, double accessTime) const;
	virtual unsigned int CalculateCELineStatePortTransparent(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext) const;

	//Memory functions
	virtual IBusInterface::AccessResult ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual IBusInterface::AccessResult WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext);
	virtual void TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext);

	//Port functions
	virtual IBusInterface::AccessResult ReadPort(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual IBusInterface::AccessResult WritePort(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void TransparentReadPort(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext);
	virtual void TransparentWritePort(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext);

	//Line functions
	virtual unsigned int GetLineID(const MarshalSupport::Marshal::In<std::wstring>& lineName) const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetLineName(unsigned int lineID) const;
	virtual unsigned int GetLineWidth(unsigned int lineID) const;
	virtual void SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void TransparentSetLineState(unsigned int targetLine, const Data& lineData);
	virtual void RevokeSetLineState(unsigned int targetLine, const Data& lineData, double reportedTime, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual bool AdvanceToLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void AssertCurrentOutputLineState() const;
	virtual void NegateCurrentOutputLineState() const;

	//Clock source functions
	virtual unsigned int GetClockSourceID(const MarshalSupport::Marshal::In<std::wstring>& clockSourceName) const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetClockSourceName(unsigned int clockSourceID) const;
	virtual void SetClockSourceRate(unsigned int clockInput, double clockRate, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void TransparentSetClockSourceRate(unsigned int clockInput, double clockRate);

	//Input functions
	virtual unsigned int GetKeyCodeID(const MarshalSupport::Marshal::In<std::wstring>& keyCodeName) const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetKeyCodeName(unsigned int keyCodeID) const;
	virtual void HandleInputKeyDown(unsigned int keyCodeID);
	virtual void HandleInputKeyUp(unsigned int keyCodeID);

private:
	std::wstring implementationName;
	std::wstring instanceName;
	unsigned int moduleID;
	ISystemDeviceInterface* systemInterface;
	IDeviceContext* deviceContext;
};

#include "Device.inl"
#endif
