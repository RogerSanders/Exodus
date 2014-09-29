#ifndef __ISYSTEMDEVICEINTERFACE_H__
#define __ISYSTEMDEVICEINTERFACE_H__
#include "InteropSupport/InteropSupport.pkg"
#include <string>

class ISystemDeviceInterface
{
public:
	//Enumerations
	enum class KeyCode;
	enum class AxisCode;
	enum class ScrollCode;

public:
	//Constructors
	virtual ~ISystemDeviceInterface() = 0 {}

	//Interface version functions
	static inline unsigned int ThisISystemDeviceInterfaceVersion();
	virtual unsigned int GetISystemDeviceInterfaceVersion() const = 0;

	//Path functions
	inline std::wstring GetCapturePath() const;

	//Logging functions
	virtual void WriteLogEvent(const ILogEntry& entry) const = 0;

	//System execution functions
	virtual void FlagStopSystem() = 0;
	virtual bool IsSystemRollbackFlagged() const = 0;
	virtual double SystemRollbackTime() const = 0;
	//##TODO## Define a typedef for the callback function type
	virtual void SetSystemRollback(IDeviceContext* atriggerDevice, IDeviceContext* arollbackDevice, double timeslice, unsigned int accessContext, void (*callbackFunction)(void*) = 0, void* callbackParams = 0) = 0;

	//Input functions
	virtual bool TranslateKeyCode(unsigned int platformKeyCode, KeyCode& inputKeyCode) const = 0;
	virtual bool TranslateJoystickButton(unsigned int joystickNo, unsigned int buttonNo, KeyCode& inputKeyCode) const = 0;
	virtual bool TranslateJoystickAxisAsButton(unsigned int joystickNo, unsigned int axisNo, bool positiveAxis, KeyCode& inputKeyCode) const = 0;
	virtual bool TranslateJoystickAxis(unsigned int joystickNo, unsigned int axisNo, AxisCode& inputAxisCode) const = 0;
	virtual void HandleInputKeyDown(KeyCode keyCode) = 0;
	virtual void HandleInputKeyUp(KeyCode keyCode) = 0;
	virtual void HandleInputAxisUpdate(AxisCode axisCode, float newValue) = 0;
	virtual void HandleInputScrollUpdate(ScrollCode scrollCode, int scrollTicks) = 0;

protected:
	//Path functions
	virtual void GetCapturePathInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
};

#include "ISystemDeviceInterface.inl"
#endif
