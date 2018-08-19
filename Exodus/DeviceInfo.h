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
	virtual Marshal::Ret<std::wstring> GetDeviceClassName() const;
	virtual Marshal::Ret<std::wstring> GetDeviceImplementationName() const;
	virtual unsigned int GetDeviceVersionNo() const;
	virtual Marshal::Ret<std::wstring> GetDeviceCopyright() const;
	virtual Marshal::Ret<std::wstring> GetDeviceComments() const;

	//Setters
	virtual void SetDeviceSettings(AllocatorPointer allocator, DestructorPointer destructor, const Marshal::In<std::wstring>& deviceClassName, const Marshal::In<std::wstring>& deviceImplementationName, unsigned int deviceVersionNo, const Marshal::In<std::wstring>& deviceCopyright, const Marshal::In<std::wstring>& deviceComments);
	virtual void SetDeviceAllocators(AllocatorPointer allocator, DestructorPointer destructor);
	virtual void SetDeviceClassName(const Marshal::In<std::wstring>& deviceClassName);
	virtual void SetDeviceImplementationName(const Marshal::In<std::wstring>& deviceImplementationName);
	virtual void SetDeviceVersionNo(unsigned int deviceVersionNo);
	virtual void SetDeviceCopyright(const Marshal::In<std::wstring>& deviceCopyright);
	virtual void SetDeviceComments(const Marshal::In<std::wstring>& deviceComments);

private:
	AllocatorPointer _allocator;
	DestructorPointer _destructor;
	std::wstring _className;
	std::wstring _implementationName;
	unsigned int _versionNo;
	std::wstring _copyright;
	std::wstring _comments;
};

#endif
