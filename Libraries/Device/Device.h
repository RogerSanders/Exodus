#ifndef __DEVICE_H__
#define __DEVICE_H__
#include "SystemInterface/SystemInterface.pkg"

class Device :public IDevice
{
public:
	//Constructors
	inline Device(const std::wstring& aclassName, const std::wstring& ainstanceName, unsigned int amoduleID);
	virtual ~Device();

	//Initialization functions
	virtual bool BindToDeviceContext(IDeviceContext* adeviceContext);
	virtual bool Construct(IHeirarchicalStorageNode& node);
	virtual bool BuildDevice();
	virtual bool ValidateDevice();
	virtual void Initialize();
	virtual void InitializeExternalConnections();

	//Reference functions
	virtual bool AddReference(const wchar_t* referenceName, IDevice* target);
	virtual bool AddReference(const wchar_t* referenceName, IBusInterface* target);
	virtual bool AddReference(const wchar_t* referenceName, IClockSource* target);
	virtual bool RemoveReference(IDevice* target);
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
	virtual unsigned int GetDeviceModuleID() const;

	//Savestate functions
	virtual bool GetScreenshot(IImage& image) const;
	virtual void LoadState(IHeirarchicalStorageNode& node);
	virtual void SaveState(IHeirarchicalStorageNode& node) const;
	virtual void LoadDebuggerState(IHeirarchicalStorageNode& node);
	virtual void SaveDebuggerState(IHeirarchicalStorageNode& node) const;

	//CE line state functions
	virtual unsigned int GetCELineID(const wchar_t* lineName, bool inputLine) const;
	virtual void SetCELineInput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber);
	virtual void SetCELineOutput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber);
	virtual unsigned int CalculateCELineStateMemory(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, double accessTime) const;
	virtual unsigned int CalculateCELineStateMemoryTransparent(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller) const;
	virtual unsigned int CalculateCELineStatePort(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, double accessTime) const;
	virtual unsigned int CalculateCELineStatePortTransparent(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller) const;

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
	virtual unsigned int GetLineID(const wchar_t* lineName) const;
	virtual const wchar_t* GetLineName(unsigned int lineID) const;
	virtual unsigned int GetLineWidth(unsigned int lineID) const;
	virtual void SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void RevokeSetLineState(unsigned int targetLine, const Data& lineData, double reportedTime, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual bool AdvanceToLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext);

	//Clock source functions
	virtual unsigned int GetClockSourceID(const wchar_t* lineName) const;
	virtual const wchar_t* GetClockSourceName(unsigned int lineID) const;
	virtual void SetClockSourceRate(unsigned int clockInput, double clockRate, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void TransparentSetClockSourceRate(unsigned int clockInput, double clockRate);

	//Input functions
	virtual unsigned int GetKeyCodeID(const wchar_t* keyCodeName) const;
	virtual void HandleInputKeyDown(unsigned int keyCode);
	virtual void HandleInputKeyUp(unsigned int keyCode);

	//Window functions
	virtual void SetAssemblyHandle(AssemblyHandle aassemblyHandle);
	virtual AssemblyHandle GetAssemblyHandle() const;
	virtual void AddGlobalDebugMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher);
	virtual void AddDebugMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher);
	virtual void RestoreViewModelState(const wchar_t* menuHandlerName, int viewModelID, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher);
	virtual void RestoreViewModelState(const std::wstring& menuHandlerName, int viewModelID, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher);
	virtual void OpenViewModel(const wchar_t* menuHandlerName, int viewModelID, IViewModelLauncher& viewModelLauncher);
	virtual void OpenViewModel(const std::wstring& menuHandlerName, int viewModelID, IViewModelLauncher& viewModelLauncher);

protected:
	//Name functions
	virtual const wchar_t* GetDeviceClassNameInternal() const;
	virtual const wchar_t* GetDeviceInstanceNameInternal() const;
	virtual const wchar_t* GetModuleDisplayNameInternal() const;

private:
	std::wstring className;
	std::wstring instanceName;
	mutable std::wstring moduleDisplayNameCached;
	unsigned int moduleID;
	IDeviceContext* deviceContext;
	AssemblyHandle assemblyHandle;
};

#include "Device.inl"
#endif
