#ifndef __ISYSTEMINTERNAL_H__
#define __ISYSTEMINTERNAL_H__
#include "ISystemExternal.h"
#include "SystemInterface/SystemInterface.pkg"
class DeviceContext;

class ISystemInternal :public ISystemExternal
{
public:
	//Logging functions
	virtual void WriteLogEvent(const ILogEntry& entry) const = 0;

	//System interface functions
	virtual bool IsSystemRollbackFlagged() const = 0;
	virtual double SystemRollbackTime() const = 0;
	virtual void SetSystemRollback(IDeviceContext* atriggerDevice, IDeviceContext* arollbackDevice, double timeslice, void (*callbackFunction)(void*), void* callbackParams) = 0;
	virtual void ExecuteDeviceStep(DeviceContext* device) = 0;

	//Input functions
	virtual void HandleInputKeyDown(IDeviceContext::KeyCode keyCode) = 0;
	virtual void HandleInputKeyUp(IDeviceContext::KeyCode keyCode) = 0;
};

#endif
