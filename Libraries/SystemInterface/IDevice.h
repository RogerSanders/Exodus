#ifndef __IDEVICE_H__
#define __IDEVICE_H__
#include <string>
#include <list>
#include "IBusInterface.h"
#include "IClockSource.h"
#include "IMenuSegment.h"
#include "ImageInterface/ImageInterface.pkg"
class IHeirarchicalStorageNode;
class Data;
class IDeviceContext;
class IExtension;
class IViewModelLauncher;

class IDevice
{
public:
	//Enumerations
	enum UpdateMethod;

	//Typedefs
	typedef void* AssemblyHandle;

public:
	//Constructors
	virtual ~IDevice() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIDeviceVersion();
	virtual unsigned int GetIDeviceVersion() const = 0;

	//Initialization functions
	virtual bool BindToDeviceContext(IDeviceContext* adeviceContext) = 0;
	virtual bool Construct(IHeirarchicalStorageNode& node) = 0;
	virtual bool BuildDevice() = 0;
	virtual bool ValidateDevice() = 0;
	virtual void Initialize() = 0;

	//Reference functions
	inline bool AddReference(const std::wstring& referenceName, IDevice* target);
	inline bool AddReference(const std::wstring& referenceName, IExtension* target);
	inline bool AddReference(const std::wstring& referenceName, IBusInterface* target);
	inline bool AddReference(const std::wstring& referenceName, IClockSource* target);
	virtual bool RemoveReference(IDevice* target) = 0;
	virtual bool RemoveReference(IExtension* target) = 0;
	virtual bool RemoveReference(IBusInterface* target) = 0;
	virtual bool RemoveReference(IClockSource* target) = 0;

	//Device context functions
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
	inline std::wstring GetDeviceImplementationName() const;
	inline std::wstring GetDeviceInstanceName() const;
	inline std::wstring GetFullyQualifiedDeviceInstanceName() const;
	inline std::wstring GetModuleDisplayName() const;
	inline std::wstring GetModuleInstanceName() const;
	virtual unsigned int GetDeviceModuleID() const = 0;

	//Savestate functions
	virtual bool GetScreenshot(IImage& image) const = 0;
	virtual void LoadState(IHeirarchicalStorageNode& node) = 0;
	virtual void SaveState(IHeirarchicalStorageNode& node) const = 0;
	virtual void LoadPersistentState(IHeirarchicalStorageNode& node) = 0;
	virtual void SavePersistentState(IHeirarchicalStorageNode& node) const = 0;
	virtual void LoadSettingsState(IHeirarchicalStorageNode& node) = 0;
	virtual void SaveSettingsState(IHeirarchicalStorageNode& node) const = 0;
	virtual void LoadDebuggerState(IHeirarchicalStorageNode& node) = 0;
	virtual void SaveDebuggerState(IHeirarchicalStorageNode& node) const = 0;

	//CE line state functions
	inline unsigned int GetCELineID(const std::wstring& lineName, bool inputLine) const;
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
	inline unsigned int GetLineID(const std::wstring& lineName) const;
	inline std::wstring GetLineName(unsigned int lineID) const;
	virtual unsigned int GetLineWidth(unsigned int lineID) const = 0;
	virtual void SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext) = 0;
	virtual void TransparentSetLineState(unsigned int targetLine, const Data& lineData) = 0;
	virtual void RevokeSetLineState(unsigned int targetLine, const Data& lineData, double reportedTime, IDeviceContext* caller, double accessTime, unsigned int accessContext) = 0;
	virtual bool AdvanceToLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext) = 0;
	virtual void AssertCurrentOutputLineState() const = 0;
	virtual void NegateCurrentOutputLineState() const = 0;

	//Clock source functions
	inline unsigned int GetClockSourceID(const std::wstring& clockSourceName) const;
	inline std::wstring GetClockSourceName(unsigned int clockSourceID) const;
	virtual void SetClockSourceRate(unsigned int clockInput, double clockRate, IDeviceContext* caller, double accessTime, unsigned int accessContext) = 0;
	virtual void TransparentSetClockSourceRate(unsigned int clockInput, double clockRate) = 0;

	//Input functions
	inline unsigned int GetKeyCodeID(const std::wstring& keyCodeName) const;
	inline std::wstring GetKeyCodeName(unsigned int keyCodeID) const;
	virtual void HandleInputKeyDown(unsigned int keyCodeID) = 0;
	virtual void HandleInputKeyUp(unsigned int keyCodeID) = 0;

	//Window functions
	virtual void SetAssemblyHandle(AssemblyHandle aassemblyHandle) = 0;
	virtual AssemblyHandle GetAssemblyHandle() const = 0;
	virtual void AddSettingsMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher) = 0;
	virtual void AddDebugMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher) = 0;
	inline void RestoreViewModelState(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher);
	inline void OpenViewModel(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IViewModelLauncher& viewModelLauncher);

protected:
	//Reference functions
	virtual bool AddReferenceInternal(const wchar_t* referenceName, IDevice* target) = 0;
	virtual bool AddReferenceInternal(const wchar_t* referenceName, IExtension* target) = 0;
	virtual bool AddReferenceInternal(const wchar_t* referenceName, IBusInterface* target) = 0;
	virtual bool AddReferenceInternal(const wchar_t* referenceName, IClockSource* target) = 0;

	//Name functions
	virtual const wchar_t* GetDeviceImplementationNameInternal() const = 0;
	virtual const wchar_t* GetDeviceInstanceNameInternal() const = 0;
	virtual const wchar_t* GetFullyQualifiedDeviceInstanceNameInternal() const = 0;
	virtual const wchar_t* GetModuleDisplayNameInternal() const = 0;
	virtual const wchar_t* GetModuleInstanceNameInternal() const = 0;

	//CE line state functions
	virtual unsigned int GetCELineIDInternal(const wchar_t* lineName, bool inputLine) const = 0;

	//Line functions
	virtual unsigned int GetLineIDInternal(const wchar_t* lineName) const = 0;
	virtual const wchar_t* GetLineNameInternal(unsigned int lineID) const = 0;

	//Clock source functions
	virtual unsigned int GetClockSourceIDInternal(const wchar_t* clockSourceName) const = 0;
	virtual const wchar_t* GetClockSourceNameInternal(unsigned int clockSourceID) const = 0;

	//Input functions
	virtual unsigned int GetKeyCodeIDInternal(const wchar_t* keyCodeName) const = 0;
	virtual const wchar_t* GetKeyCodeNameInternal(unsigned int keyCodeID) const = 0;

	//Window functions
	virtual void RestoreViewModelStateInternal(const wchar_t* viewModelGroupName, const wchar_t* viewModelName, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher) = 0;
	virtual void OpenViewModelInternal(const wchar_t* viewModelGroupName, const wchar_t* viewModelName, IViewModelLauncher& viewModelLauncher) = 0;
};

#include "IDevice.inl"
#endif
