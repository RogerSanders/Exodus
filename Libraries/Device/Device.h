#ifndef __DEVICE_H__
#define __DEVICE_H__
#include "SystemInterface/SystemInterface.pkg"

class Device :public IDevice
{
public:
	//Constructors
	inline Device(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID);
	virtual ~Device();

	//Interface version functions
	virtual unsigned int GetIDeviceVersion() const;

	//Initialization functions
	virtual bool BindToDeviceContext(IDeviceContext* adeviceContext);
	virtual bool Construct(IHeirarchicalStorageNode& node);
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

	//Device context functions
	virtual IDeviceContext* GetDeviceContext() const;
	virtual double GetCurrentTimesliceProgress() const;
	void SetCurrentTimesliceProgress(double executionProgress);

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
	inline std::wstring GetDeviceImplementationName() const;
	inline std::wstring GetDeviceInstanceName() const;
	inline std::wstring GetFullyQualifiedDeviceInstanceName() const;
	inline std::wstring GetModuleDisplayName() const;
	inline std::wstring GetModuleInstanceName() const;
	virtual unsigned int GetDeviceModuleID() const;

	//Savestate functions
	virtual bool GetScreenshot(IImage& image) const;
	virtual void LoadState(IHeirarchicalStorageNode& node);
	virtual void SaveState(IHeirarchicalStorageNode& node) const;
	virtual void LoadPersistentState(IHeirarchicalStorageNode& node);
	virtual void SavePersistentState(IHeirarchicalStorageNode& node) const;
	virtual void LoadSettingsState(IHeirarchicalStorageNode& node);
	virtual void SaveSettingsState(IHeirarchicalStorageNode& node) const;
	virtual void LoadDebuggerState(IHeirarchicalStorageNode& node);
	virtual void SaveDebuggerState(IHeirarchicalStorageNode& node) const;

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

	//Window functions
	virtual void SetAssemblyHandle(AssemblyHandle aassemblyHandle);
	virtual AssemblyHandle GetAssemblyHandle() const;
	virtual void AddSettingsMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher);
	virtual void AddDebugMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher);
	virtual void RestoreViewModelState(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher);
	virtual void OpenViewModel(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IViewModelLauncher& viewModelLauncher);

protected:
	//Reference functions
	virtual bool AddReferenceInternal(const wchar_t* referenceName, IDevice* target);
	virtual bool AddReferenceInternal(const wchar_t* referenceName, IExtension* target);
	virtual bool AddReferenceInternal(const wchar_t* referenceName, IBusInterface* target);
	virtual bool AddReferenceInternal(const wchar_t* referenceName, IClockSource* target);

	//Name functions
	virtual const wchar_t* GetDeviceImplementationNameInternal() const;
	virtual const wchar_t* GetDeviceInstanceNameInternal() const;
	virtual const wchar_t* GetFullyQualifiedDeviceInstanceNameInternal() const;
	virtual const wchar_t* GetModuleDisplayNameInternal() const;
	virtual const wchar_t* GetModuleInstanceNameInternal() const;

	//CE line state functions
	virtual unsigned int GetCELineIDInternal(const wchar_t* lineName, bool inputLine) const;

	//Line functions
	virtual unsigned int GetLineIDInternal(const wchar_t* lineName) const;
	virtual const wchar_t* GetLineNameInternal(unsigned int lineID) const;

	//Clock source functions
	virtual unsigned int GetClockSourceIDInternal(const wchar_t* clockSourceName) const;
	virtual const wchar_t* GetClockSourceNameInternal(unsigned int clockSourceID) const;

	//Input functions
	virtual unsigned int GetKeyCodeIDInternal(const wchar_t* keyCodeName) const;
	virtual const wchar_t* GetKeyCodeNameInternal(unsigned int keyCodeID) const;

	//Window functions
	virtual void RestoreViewModelStateInternal(const wchar_t* viewModelGroupName, const wchar_t* viewModelName, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher);
	virtual void OpenViewModelInternal(const wchar_t* viewModelGroupName, const wchar_t* viewModelName, IViewModelLauncher& viewModelLauncher);

private:
	std::wstring implementationName;
	std::wstring instanceName;
	mutable std::wstring fullyQualifiedDeviceInstanceNameCached;
	mutable std::wstring moduleDisplayNameCached;
	mutable std::wstring moduleInstanceNameCached;
	mutable std::wstring lineNameCached;
	mutable std::wstring clockSourceNameCached;
	mutable std::wstring keyCodeNameCached;
	unsigned int moduleID;
	IDeviceContext* deviceContext;
	AssemblyHandle assemblyHandle;
};

#include "Device.inl"
#endif
