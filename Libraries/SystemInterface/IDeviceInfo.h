#ifndef __IDEVICEINFO_H__
#define __IDEVICEINFO_H__
#include <string>
class IDevice;

class IDeviceInfo
{
public:
	//Constructors
	virtual ~IDeviceInfo() = 0 {}

	//Setters
	inline void SetDeviceSettings(const std::wstring& adeviceName, IDevice* (aAllocator)(const wchar_t*, unsigned int), void (aDestructor)(IDevice*), unsigned int adeviceVersionNo);
	inline void SetDeviceName(const std::wstring& adeviceName);
	virtual void SetDeviceVersionNo(unsigned int adeviceVersionNo) = 0;
	virtual void SetDeviceAllocators(IDevice* (aAllocator)(const wchar_t*, unsigned int), void (aDestructor)(IDevice*)) = 0;

protected:
	//Setters
	virtual void SetDeviceNameInternal(const wchar_t* adeviceName) = 0;
};

#include "IDeviceInfo.inl"
#endif
