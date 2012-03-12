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
\*--------------------------------------------------------------------------------------*/
#ifndef __DEVICECONTEXT_H__
#define __DEVICECONTEXT_H__
#include "SystemInterface/SystemInterface.pkg"
#include "WindowFunctions/WindowFunctions.pkg"
#include "ISystemInternal.h"
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <string>

class DeviceContext :public IDeviceContext
{
public:
	//Constructors
	inline DeviceContext(IDevice* adevice);

	//Execute functions
	inline void NotifyUpcomingTimeslice(double nanoseconds);
	inline void NotifyBeforeExecuteCalled();
	inline void NotifyAfterExecuteCalled();
	inline void ExecuteTimeslice(double nanoseconds);
	inline double ExecuteStep();
	inline void WaitForCompletion();
	inline void Commit();
	inline void Rollback();
	inline void Initialize();

	//Timing functions
	virtual double GetCurrentTimesliceProgress() const;
	inline double GetNextTimingPoint() const;
	inline double GetCurrentRemainingTime() const;
	inline double GetInitialRemainingTime() const;
	inline void ClearRemainingTime();

	//Control functions
	virtual bool DeviceEnabled() const;
	virtual void SetDeviceEnabled(bool state);

	//Worker thread control
	void BeginExecution();
	void SuspendExecution();

	//Device/System interface
	virtual IDevice* GetTargetDevice() const;
	inline void SetSystemInterface(ISystemInternal* asystemObject);
	inline bool ActiveDevice() const;
	inline bool SendNotifyUpcomingTimeslice() const;
	inline bool SendNotifyBeforeExecuteCalled() const;
	inline bool SendNotifyAfterExecuteCalled() const;

	//System message functions
	virtual void SetSystemRollback(IDeviceContext* atriggerDevice, IDeviceContext* arollbackDevice, double timeslice, void (*callbackFunction)(void*) = 0, void* callbackParams = 0);
	virtual void WriteLogEvent(const ILogEntry& entry);
	virtual void FlagStopSystem();
	virtual void StopSystem();
	virtual void RunSystem();
	virtual void ExecuteDeviceStep();

	//Dependent device functions
	inline void AddDependentDevice(DeviceContext* device);
	inline void RemoveDependentDevice(DeviceContext* device);

	//Input functions
	virtual bool TranslateKeyCode(unsigned int platformKeyCode, KeyCode& inputKeyCode);
	virtual void HandleInputKeyDown(KeyCode keyCode) const;
	virtual void HandleInputKeyUp(KeyCode keyCode) const;

protected:
	//System message functions
	virtual const wchar_t* GetCapturePathInternal() const;
	virtual const wchar_t* GetModuleDisplayNameInternal() const;

private:
	//Worker thread control
	void WorkerThread();
	void WorkerThreadStep();
	void WorkerThreadStepWithDependents();
	void WorkerThreadTimeslice();

private:
	//Main access mutex
	mutable boost::mutex accessMutex;

	//Device properties
	IDevice* device;
	bool deviceEnabled;
	bool active;
	std::vector<DeviceContext*> dependentTargets;

	//Execute thread data
	boost::condition stateChanged;
	boost::condition justCompletedTimeslice;
	boost::condition executeThreadReady;
	boost::condition executeThreadStopped;
	bool timesliceCompleted;
	double timeslice;
	double remainingTime;
	double remainingTimeBackup;
	double currentTimesliceProgress;

	//Cached settings
	mutable std::wstring capturePathCached;
	mutable std::wstring moduleDisplayNameCached;

	//Callback parameters
	ISystemInternal* systemObject;
};

#include "DeviceContext.inl"
#endif
