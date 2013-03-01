#ifndef __DEVICEINFO_H__
#define __DEVICEINFO_H__
#include "SystemInterface/SystemInterface.pkg"
#include <string>

class DeviceInfo :public IDeviceInfo
{
public:
	//Interface version functions
	virtual unsigned int GetIDeviceInfoVersion() const;

	//Setters
	virtual void SetDeviceVersionNo(unsigned int adeviceVersionNo);
	virtual void SetDeviceAllocators(AllocatorPointer aAllocator, DestructorPointer aDestructor);

	//Getters
	unsigned int GetDeviceVersionNo() const;
	std::wstring GetDeviceName() const;
	//This is much neater using a typedef. See below for the correct syntax without a
	//typedef for the function pointers:
	//IDevice* (*GetAllocator() const)(const wchar_t*, unsigned int);
	//void (*GetDestructor() const)(IDevice*);
	AllocatorPointer GetAllocator() const;
	DestructorPointer GetDestructor() const;

protected:
	//Setters
	virtual void SetDeviceNameInternal(const wchar_t* adeviceName);

private:
	unsigned int deviceVersionNo;
	std::wstring deviceName;
	AllocatorPointer Allocator;
	DestructorPointer Destructor;
};

#endif
