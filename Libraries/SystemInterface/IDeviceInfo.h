#ifndef __IDEVICEINFO_H__
#define __IDEVICEINFO_H__
#include <string>
class IDevice;

class IDeviceInfo
{
public:
	//Typedefs
	typedef IDevice* (*AllocatorPointer)(const wchar_t* instanceName, unsigned int moduleID);
	typedef void (*DestructorPointer)(IDevice* device);

public:
	//Constructors
	virtual ~IDeviceInfo() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIDeviceInfoVersion();
	virtual unsigned int GetIDeviceInfoVersion() const = 0;

	//Setters
	inline void SetDeviceSettings(const std::wstring& adeviceName, AllocatorPointer aAllocator, DestructorPointer aDestructor, unsigned int adeviceVersionNo);
	inline void SetDeviceName(const std::wstring& adeviceName);
	virtual void SetDeviceVersionNo(unsigned int adeviceVersionNo) = 0;
	virtual void SetDeviceAllocators(AllocatorPointer aAllocator, DestructorPointer aDestructor) = 0;

protected:
	//Setters
	virtual void SetDeviceNameInternal(const wchar_t* adeviceName) = 0;
};

#include "IDeviceInfo.inl"
#endif
