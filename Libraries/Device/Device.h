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
	using IDevice::AddReference;
	virtual bool AddReference(const std::wstring& referenceName, IDevice* target);
	virtual bool AddReference(const std::wstring& referenceName, IExtension* target);
	virtual bool AddReference(const std::wstring& referenceName, IBusInterface* target);
	virtual bool AddReference(const std::wstring& referenceName, IClockSource* target);
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
	inline std::wstring GetDeviceClassName() const;
	inline std::wstring GetDeviceInstanceName() const;
	inline std::wstring GetFullyQualifiedDeviceInstanceName() const;
	inline std::wstring GetModuleDisplayName() const;
	inline std::wstring GetModuleInstanceName() const;
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
	virtual unsigned int GetCELineID(const std::wstring& lineName, bool inputLine) const;
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
	virtual unsigned int GetLineID(const std::wstring& lineName) const;
	virtual std::wstring GetLineName(unsigned int lineID) const;
	virtual unsigned int GetLineWidth(unsigned int lineID) const;
	virtual void SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void TransparentSetLineState(unsigned int targetLine, const Data& lineData);
	virtual void RevokeSetLineState(unsigned int targetLine, const Data& lineData, double reportedTime, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual bool AdvanceToLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void AssertCurrentOutputLineState() const;
	virtual void NegateCurrentOutputLineState() const;

	//Clock source functions
	virtual unsigned int GetClockSourceID(const std::wstring& clockSourceName) const;
	virtual std::wstring GetClockSourceName(unsigned int clockSourceID) const;
	virtual void SetClockSourceRate(unsigned int clockInput, double clockRate, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void TransparentSetClockSourceRate(unsigned int clockInput, double clockRate);

	//Input functions
	virtual unsigned int GetKeyCodeID(const std::wstring& keyCodeName) const;
	virtual std::wstring GetKeyCodeName(unsigned int keyCodeID) const;
	virtual void HandleInputKeyDown(unsigned int keyCodeID);
	virtual void HandleInputKeyUp(unsigned int keyCodeID);

protected:
	//Reference functions
	virtual bool AddReferenceInternal(const InteropSupport::ISTLObjectSource<std::wstring>& referenceNameMarshaller, IDevice* target);
	virtual bool AddReferenceInternal(const InteropSupport::ISTLObjectSource<std::wstring>& referenceNameMarshaller, IExtension* target);
	virtual bool AddReferenceInternal(const InteropSupport::ISTLObjectSource<std::wstring>& referenceNameMarshaller, IBusInterface* target);
	virtual bool AddReferenceInternal(const InteropSupport::ISTLObjectSource<std::wstring>& referenceNameMarshaller, IClockSource* target);

	//Name functions
	virtual void GetDeviceClassNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void GetDeviceInstanceNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void GetFullyQualifiedDeviceInstanceNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void GetModuleDisplayNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void GetModuleInstanceNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const ;

	//CE line state functions
	virtual unsigned int GetCELineIDInternal(const InteropSupport::ISTLObjectSource<std::wstring>& lineNameMarshaller, bool inputLine) const;

	//Line functions
	virtual unsigned int GetLineIDInternal(const InteropSupport::ISTLObjectSource<std::wstring>& lineNameMarshaller) const;
	virtual void GetLineNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller, unsigned int lineID) const;

	//Clock source functions
	virtual unsigned int GetClockSourceIDInternal(const InteropSupport::ISTLObjectSource<std::wstring>& clockSourceNameMarshaller) const;
	virtual void GetClockSourceNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller, unsigned int clockSourceID) const;

	//Input functions
	virtual unsigned int GetKeyCodeIDInternal(const InteropSupport::ISTLObjectSource<std::wstring>& keyCodeNameMarshaller) const;
	virtual void GetKeyCodeNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller, unsigned int keyCodeID) const;

private:
	std::wstring implementationName;
	std::wstring instanceName;
	unsigned int moduleID;
	ISystemDeviceInterface* systemInterface;
	IDeviceContext* deviceContext;
};

#include "Device.inl"
#endif
