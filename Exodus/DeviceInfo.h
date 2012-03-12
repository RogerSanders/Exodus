#ifndef __DEVICEINFO_H__
#define __DEVICEINFO_H__
#include "SystemInterface/SystemInterface.pkg"
#include <string>

class DeviceInfo :public IDeviceInfo
{
public:
	virtual void SetDeviceVersionNo(unsigned int adeviceVersionNo);
	virtual void SetDeviceAllocators(IDevice* (aAllocator)(const wchar_t*, unsigned int), void (aDestructor)(IDevice*));
	unsigned int GetDeviceVersionNo() const;
	std::wstring GetDeviceName() const;

	//This is much neater using a typedef. See below for the correct syntax without a
	//typedef for the function pointers:
	//IDevice* (*GetAllocator() const)(const wchar_t*, unsigned int);
	//void (*GetDestructor() const)(IDevice*);
	typedef IDevice* (*AllocatorPointer)(const wchar_t*, unsigned int);
	typedef void (*DestructorPointer)(IDevice*);
	AllocatorPointer GetAllocator() const;
	DestructorPointer GetDestructor() const;

protected:
	virtual void SetDeviceNameInternal(const wchar_t* adeviceName);

private:
	unsigned int deviceVersionNo;
	std::wstring deviceName;
	IDevice* (*Allocator)(const wchar_t* instanceName, unsigned int moduleID);
	void (*Destructor)(IDevice*);
};

#endif
