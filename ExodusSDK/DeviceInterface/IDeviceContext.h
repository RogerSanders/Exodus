#ifndef __IDEVICECONTEXT_H__
#define __IDEVICECONTEXT_H__
#include "MarshalSupport/MarshalSupport.pkg"
class ILogEntry;
class IDevice;

//##TODO## Eliminate any methods here which are merely wrappers of system methods, and
//don't logically belong on this interface.
class IDeviceContext
{
public:
	//Constructors
	virtual ~IDeviceContext() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIDeviceContextVersion() { return 1; }
	virtual unsigned int GetIDeviceContextVersion() const = 0;

	//Timing functions
	virtual double GetCurrentTimesliceProgress() const = 0;
	virtual void SetCurrentTimesliceProgress(double executionProgress) = 0;

	//Control functions
	virtual bool DeviceEnabled() const = 0;
	virtual void SetDeviceEnabled(bool state) = 0;

	//Device interface
	virtual IDevice& GetTargetDevice() const = 0;
	virtual unsigned int GetDeviceIndexNo() const = 0;

	//System message functions
	//##TODO## Remove all these functions from here entirely, and let the device use its
	//provided system interface to execute these instead.
	//##TODO## Remove this method once we have a new logging system with dedicated logger
	//objects, and a source setting for the logger object.
	virtual void WriteLogEvent(const ILogEntry& entry) = 0;
	virtual void FlagStopSystem() = 0;
	virtual void StopSystem() = 0;
	virtual void RunSystem() = 0;
	virtual void ExecuteDeviceStep() = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetFullyQualifiedDeviceInstanceName() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetModuleDisplayName() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetModuleInstanceName() const = 0;

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

	//Dependent device functions
	virtual void SetDeviceDependencyEnable(IDeviceContext* targetDevice, bool state) = 0;
};

#endif
