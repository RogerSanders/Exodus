#ifndef __DEVICEINTERNAL_H__
#define __DEVICEINTERNAL_H__
#include "DeviceInterface/DeviceInterface.pkg"

//Note that the only purpose of this class is for optimization purposes. Profiling showed
//that the use of virtual functions for GetCurrentTimesliceProgress and GetDeviceContext
//methods is hurting us because these functions get called a lot by the derived device
//classes, and there's no inlining. Simply marking this virtual method as inline won't
//really help though, since the compiler won't know for sure the type that's being
//operated on isn't a more derived type that overrides the virtual method. In C++11
//there's the "final" keyword that can inform the compiler that the method will not be
//overridden again, so marking a final virtual method as inline should allow inlining
//freely if the type is known to at least include the sealed implementation. When we
//upgrade the code base to use C++11, we should eliminate this intermediate class and use
//inline final virtual methods instead, and change deviceContext back to private.
class DeviceInternal :public IDevice
{
public:
	//Constructors
	DeviceInternal();
	virtual ~DeviceInternal();

	//Interface version functions
	virtual unsigned int GetIDeviceVersion() const;

	//Device context functions
	virtual IDeviceContext* GetDeviceContext() const;
	virtual double GetCurrentTimesliceProgress() const;

protected:
	//Note that even though this data member is protected and not private, it should not
	//be accessed directly by any derived class other than the "Device" base class. This
	//data member is only marked as protected temporarily so we can implement a
	//performance workaround until C++11 support is added.
	IDeviceContext* deviceContext;
};

#endif
