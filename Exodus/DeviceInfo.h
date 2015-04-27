#ifndef __DEVICEINFO_H__
#define __DEVICEINFO_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include <string>

class DeviceInfo :public IDeviceInfo
{
public:
	//Interface version functions
	virtual unsigned int GetIDeviceInfoVersion() const;

	//Getters
	virtual AllocatorPointer GetAllocator() const;
	virtual DestructorPointer GetDestructor() const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetDeviceClassName() const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetDeviceImplementationName() const;
	virtual unsigned int GetDeviceVersionNo() const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetDeviceCopyright() const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetDeviceComments() const;

	//Setters
	virtual void SetDeviceSettings(AllocatorPointer aAllocator, DestructorPointer aDestructor, const MarshalSupport::Marshal::In<std::wstring>& adeviceClassName, const MarshalSupport::Marshal::In<std::wstring>& adeviceImplementationName, unsigned int adeviceVersionNo, const MarshalSupport::Marshal::In<std::wstring>& adeviceCopyright, const MarshalSupport::Marshal::In<std::wstring>& adeviceComments);
	virtual void SetDeviceAllocators(AllocatorPointer aAllocator, DestructorPointer aDestructor);
	virtual void SetDeviceClassName(const MarshalSupport::Marshal::In<std::wstring>& adeviceClassName);
	virtual void SetDeviceImplementationName(const MarshalSupport::Marshal::In<std::wstring>& adeviceImplementationName);
	virtual void SetDeviceVersionNo(unsigned int adeviceVersionNo);
	virtual void SetDeviceCopyright(const MarshalSupport::Marshal::In<std::wstring>& adeviceCopyright);
	virtual void SetDeviceComments(const MarshalSupport::Marshal::In<std::wstring>& adeviceComments);

private:
	AllocatorPointer Allocator;
	DestructorPointer Destructor;
	std::wstring className;
	std::wstring implementationName;
	unsigned int versionNo;
	std::wstring copyright;
	std::wstring comments;
};

#endif
