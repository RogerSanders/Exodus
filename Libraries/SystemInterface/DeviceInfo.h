#ifndef __DEVICEINFO_H__
#define __DEVICEINFO_H__
#include <string>
class IDevice;

//##TODO## Create a pure virtual base class, which all devices communicate with to expose
//data to this structure. Since we already create this object from the system and pass it
//to the devices to populate, it should be an easy change, and it will solve the problem
//of exposing stl objects across dll boundaries.
struct DeviceInfo
{
public:
	//Constructors
	DeviceInfo()
	:deviceVersionNo(0), Allocator(0), Destructor(0)
	{}
	DeviceInfo(const std::wstring& adeviceName, IDevice* (aAllocator)(const wchar_t* instanceName), void (aDestructor)(IDevice*), unsigned int adeviceVersionNo)
	:deviceName(adeviceName), deviceVersionNo(adeviceVersionNo), Allocator(aAllocator), Destructor(aDestructor)
	{}

public:
	unsigned int deviceVersionNo;
	std::wstring deviceName;
	IDevice* (*Allocator)(const wchar_t* instanceName);
	void (*Destructor)(IDevice*);
};

#endif
