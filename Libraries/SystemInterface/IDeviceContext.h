#ifndef __IDEVICECONTEXT_H__
#define __IDEVICECONTEXT_H__
class ILogEntry;
class IDevice;

class IDeviceContext
{
public:
	//Enumerations
	enum KeyCode;

public:
	//Constructors
	virtual ~IDeviceContext() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIDeviceContextVersion();
	virtual unsigned int GetIDeviceContextVersion() const = 0;

	//Timing functions
	virtual double GetCurrentTimesliceProgress() const = 0;
	virtual void SetCurrentTimesliceProgress(double executionProgress) = 0;

	//Control functions
	virtual bool DeviceEnabled() const = 0;
	virtual void SetDeviceEnabled(bool state) = 0;

	//Device/System interface
	virtual IDevice* GetTargetDevice() const = 0;

	//System message functions
	virtual void SetSystemRollback(IDeviceContext* atriggerDevice, IDeviceContext* arollbackDevice, double timeslice, unsigned int accessContext, void (*callbackFunction)(void*) = 0, void* callbackParams = 0) = 0;
	virtual void WriteLogEvent(const ILogEntry& entry) = 0;
	virtual void FlagStopSystem() = 0;
	virtual void StopSystem() = 0;
	virtual void RunSystem() = 0;
	virtual void ExecuteDeviceStep() = 0;
	inline std::wstring GetCapturePath() const;
	inline std::wstring GetFullyQualifiedDeviceInstanceName() const;
	inline std::wstring GetModuleDisplayName() const;
	inline std::wstring GetModuleInstanceName() const;

	//Suspend functions
	virtual bool UsesExecuteSuspend() const = 0;
	virtual bool UsesTransientExecution() const = 0;
	virtual bool TimesliceExecutionSuspended() const = 0;
	virtual void SuspendTimesliceExecution() = 0;
	virtual void WaitForTimesliceExecutionResume() const = 0;
	virtual void ResumeTimesliceExecution() = 0;
	virtual bool TimesliceSuspensionDisabled() const = 0;
	virtual bool TransientExecutionActive() const = 0;
	virtual void SetTransientExecutionActive(bool state) = 0;
	virtual bool TimesliceExecutionCompleted() const = 0;

	//Input functions
	virtual void HandleInputKeyDown(KeyCode keyCode) const = 0;
	virtual void HandleInputKeyUp(KeyCode keyCode) const = 0;
	virtual bool TranslateKeyCode(unsigned int platformKeyCode, KeyCode& inputKeyCode) = 0;

	//Dependent device functions
	virtual void SetDeviceDependencyEnable(IDeviceContext* targetDevice, bool state) = 0;

protected:
	//System message functions
	virtual const wchar_t* GetCapturePathInternal() const = 0;
	virtual const wchar_t* GetFullyQualifiedDeviceInstanceNameInternal() const = 0;
	virtual const wchar_t* GetModuleDisplayNameInternal() const = 0;
	virtual const wchar_t* GetModuleInstanceNameInternal() const = 0;
};

#include "IDeviceContext.inl"
#endif
