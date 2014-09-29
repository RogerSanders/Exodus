/*--------------------------------------------------------------------------------------*\
Things to do:
-Remove the "Sleep" command in our worker thread. We need all this code to be platform
independent. I'd suggest you start your own "thread management" library with some thin
locks, priority functions, and a sleep command, to keep the main code platform
independent. In the case of our sleep command however, I suggest you try and write a
"high performance" sleep function, which takes a sleep interval in nanoseconds, and is
clever enough to decide when to call the OS-level sleep function and for how long, to
ensure that the sleep function doesn't last for longer than the specified interval, but
also maximizes the available processor cycles during that time. I'd also suggest a
separate overload of the sleep function which takes no arguments, and follows the
Sleep(0) behaviour of Windows.
-Create a new central worker thread, which is provided with a condition variable to wait
on, and the location of a message structure. The message structure will contain a code for
the type of message request. This will allow another thread to dispatch a request/command
to all devices simultaneously. We will need a thread-safe way to collect data from each
notified device however, such as for GetNextTimingPoint(). Providing a lock and a list
structure is probably sufficient, or we can provide an array, giving each device an index
number, and collect the data without any locks. At any rate, we still need a way to know
when all device worker threads have fully processed the message. For this, some kind of
thread-safe counter, possibly implemented using interlocked operations, would be
appropriate. The counter is set to the number of notified threads, each thread modifies it
directly, and when the counter reaches 0, the last device to finish notifies a supplied
conditional, which unlocks the calling thread.
\*--------------------------------------------------------------------------------------*/
#ifndef __DEVICECONTEXT_H__
#define __DEVICECONTEXT_H__
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "WindowsSupport/WindowsSupport.pkg"
#include "ThreadLib/ThreadLib.pkg"
#include "SystemInterface/SystemInterface.pkg"
#include "IExecutionSuspendManager.h"
#include <mutex>
#include <condition_variable>
#include <string>
#include <vector>

class DeviceContext :public IDeviceContext
{
public:
	//Structures
	struct DeviceContextCommand;
	struct DeviceDependency;

public:
	//Constructors
	inline DeviceContext(IDevice& adevice, ISystemGUIInterface& asystemObject);

	//Interface version functions
	virtual unsigned int GetIDeviceContextVersion() const;

	//Execute functions
	inline void NotifyUpcomingTimeslice(double nanoseconds);
	inline void NotifyBeforeExecuteCalled();
	inline void NotifyAfterExecuteCalled();
	inline void ExecuteTimeslice(double nanoseconds);
	inline double ExecuteStep();
	inline double ExecuteStep(unsigned int accessContext);
	inline void WaitForCompletion();
	inline void WaitForCompletionAndDetectSuspendLock(volatile ReferenceCounterType& suspendedThreadCount, volatile ReferenceCounterType& remainingThreadCount, std::mutex& commandMutex, IExecutionSuspendManager* asuspendManager);
	inline void Commit();
	inline void Rollback();
	inline void Initialize();

	//Timing functions
	virtual double GetCurrentTimesliceProgress() const;
	virtual void SetCurrentTimesliceProgress(double executionProgress);
	inline double GetNextTimingPoint(unsigned int& accessContext) const;
	inline double GetCurrentRemainingTime() const;
	inline double GetInitialRemainingTime() const;
	inline void ClearRemainingTime();

	//Control functions
	virtual bool DeviceEnabled() const;
	virtual void SetDeviceEnabled(bool state);

	//Worker thread control
	void BeginExecution(size_t deviceIndex, volatile ReferenceCounterType& remainingThreadCount, volatile ReferenceCounterType& suspendedThreadCount, std::mutex& commandMutex, std::condition_variable& commandSent, std::condition_variable& commandProcessed, IExecutionSuspendManager* asuspendManager, const DeviceContextCommand& command);

	//Device interface
	virtual IDevice& GetTargetDevice() const;
	virtual unsigned int GetDeviceIndexNo() const;
	inline void SetDeviceIndexNo(unsigned int adeviceIndexNo);
	inline bool ActiveDevice() const;

	//System message functions
	virtual void WriteLogEvent(const ILogEntry& entry);
	virtual void FlagStopSystem();
	virtual void StopSystem();
	virtual void RunSystem();
	virtual void ExecuteDeviceStep();
	inline std::wstring GetFullyQualifiedDeviceInstanceName() const;
	inline std::wstring GetModuleDisplayName() const;
	inline std::wstring GetModuleInstanceName() const;

	//Suspend functions
	virtual bool UsesExecuteSuspend() const;
	virtual bool UsesTransientExecution() const;
	virtual bool TimesliceExecutionSuspended() const;
	virtual void SuspendTimesliceExecution();
	virtual void WaitForTimesliceExecutionResume() const;
	virtual void ResumeTimesliceExecution();
	virtual bool TimesliceSuspensionDisabled() const;
	virtual bool TransientExecutionActive() const;
	virtual void SetTransientExecutionActive(bool state);
	virtual bool TimesliceExecutionCompleted() const;
	void DisableTimesliceExecutionSuspend();
	void EnableTimesliceExecutionSuspend();

	//Dependent device functions
	virtual void SetDeviceDependencyEnable(IDeviceContext* targetDevice, bool state);
	inline void AddDeviceDependency(DeviceContext* targetDevice);
	inline void RemoveDeviceDependency(DeviceContext* targetDevice);
	inline const std::vector<DeviceDependency>& GetDeviceDependencyArray() const;
	inline const std::vector<DeviceContext*>& GetDependentDeviceArray() const;

protected:
	//System message functions
	virtual void GetFullyQualifiedDeviceInstanceNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void GetModuleDisplayNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void GetModuleInstanceNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;

private:
	//Worker thread control
	void SuspendExecution();

	//Command worker thread control
	void StartCommandWorkerThread(size_t deviceIndex, volatile ReferenceCounterType& remainingThreadCount, volatile ReferenceCounterType& suspendedThreadCount, std::mutex& commandMutex, std::condition_variable& commandSent, std::condition_variable& commandProcessed, IExecutionSuspendManager* asuspendManager, const DeviceContextCommand& command);
	void StopCommandWorkerThread();
	void CommandWorkerThread(size_t deviceIndex, volatile ReferenceCounterType& remainingThreadCount, volatile ReferenceCounterType& suspendedThreadCount, std::mutex& commandMutex, std::condition_variable& commandSent, std::condition_variable& commandProcessed, IExecutionSuspendManager* asuspendManager, const DeviceContextCommand& command);
	void ProcessCommand(size_t deviceIndex, const DeviceContextCommand& command, volatile ReferenceCounterType& remainingThreadCount);

	//Execute worker thread control
	void StartExecuteWorkerThread();
	void StopExecuteWorkerThread();
	void ExecuteWorkerThread();
	void ExecuteWorkerThreadStep();
	void ExecuteWorkerThreadStepWithDependencies();
	static void ExecuteWorkerThreadStepMultipleDeviceSharedDependencies(DeviceContext* device1, DeviceContext* device2);
	void ExecuteWorkerThreadStepSharedExecutionThreadSpinoff();
	void ExecuteWorkerThreadTimeslice();
	void ExecuteWorkerThreadTimesliceWithDependencies();

	//Dependent device functions
	inline void AddDependentDevice(DeviceContext* targetDevice);
	inline void RemoveDependentDevice(DeviceContext* targetDevice);

private:
	//Device properties
	IDevice& device;
	unsigned int deviceIndexNo;
	bool deviceEnabled;
	std::vector<DeviceDependency> deviceDependencies;
	std::vector<DeviceContext*> dependentDevices;

	//Command worker thread data
	bool commandWorkerThreadActive;
	std::condition_variable commandThreadReady;

	//Execute worker thread data
	bool executeWorkerThreadActive;
	mutable std::mutex executeThreadMutex;
	std::condition_variable executeTaskSent;
	mutable std::condition_variable executeCompletionStateChanged;
	bool executeThreadRunningState;
	std::condition_variable executeThreadReady;
	std::condition_variable executeThreadStopped;
	std::mutex* commandMutexPointer;
	volatile ReferenceCounterType* suspendedThreadCountPointer;
	volatile ReferenceCounterType* remainingThreadCountPointer;
	volatile bool executingWaitForCompletionCommand;
	IExecutionSuspendManager* suspendManager;
	volatile bool timesliceCompleted;
	volatile bool timesliceSuspended;
	volatile bool timesliceSuspensionDisable;
	volatile bool transientExecutionActive;
	double timeslice;
	double remainingTime;
	double remainingTimeBackup;
	volatile double currentTimesliceProgress;

	//Combined worker thread data
	bool sharingExecuteThread;
	bool primarySharedExecuteThreadDevice;
	DeviceContext* otherSharedExecuteThreadDevice;
	DeviceContext* currentSharedExecuteThreadOwner;
	volatile bool sharedExecuteThreadSpinoffActive;
	volatile bool sharedExecuteThreadSpinoffRejoinRequested;
	bool sharedExecuteThreadSpinoffPaused;
	bool sharedExecuteThreadSpinoffStopRequested;
	bool sharedExecuteThreadSpinoffRunning;
	bool sharedExecuteThreadSpinoffTimesliceAvailable;
	std::condition_variable sharedExecuteThreadSpinoffStateChangeRequested;
	std::condition_variable sharedExecuteThreadSpinoffStoppedOrPaused;
	std::condition_variable sharedExecuteThreadSpinoffTimesliceProcessingBegun;

	//Callback parameters
	ISystemGUIInterface& systemObject;
};

#include "DeviceContext.inl"
#endif
