#ifndef __DEVICEINFO_H__
#define __DEVICEINFO_H__
#include "SystemInterface/SystemInterface.pkg"
#include <string>

class DeviceInfo :public IDeviceInfo
{
public:
	//Interface version functions
	virtual unsigned int GetIDeviceInfoVersion() const;

	//Getters
	//This is much neater using a typedef. See below for the correct syntax without a
	//typedef for the function pointers:
	//IDevice* (*GetAllocator() const)(const wchar_t*, const wchar_t*, unsigned int);
	//void (*GetDestructor() const)(IDevice*);
	AllocatorPointer GetAllocator() const;
	DestructorPointer GetDestructor() const;
	std::wstring GetDeviceClassName() const;
	std::wstring GetDeviceImplementationName() const;
	virtual unsigned int GetDeviceVersionNo() const;
	std::wstring GetDeviceCopyright() const;
	std::wstring GetDeviceComments() const;

	//Setters
	inline void SetDeviceSettings(AllocatorPointer aAllocator, DestructorPointer aDestructor, const std::wstring& adeviceClassName, const std::wstring& adeviceImplementationName, unsigned int adeviceVersionNo, const std::wstring& adeviceCopyright, const std::wstring& adeviceComments);
	virtual void SetDeviceAllocators(AllocatorPointer aAllocator, DestructorPointer aDestructor);
	inline void SetDeviceClassName(const std::wstring& adeviceClassName);
	inline void SetDeviceImplementationName(const std::wstring& adeviceImplementationName);
	virtual void SetDeviceVersionNo(unsigned int adeviceVersionNo);
	inline void SetDeviceCopyright(const std::wstring& adeviceCopyright);
	inline void SetDeviceComments(const std::wstring& adeviceComments);

protected:
	//Setters
	virtual void SetDeviceClassNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller);
	virtual void SetDeviceImplementationNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller);
	virtual void SetDeviceCopyrightInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller);
	virtual void SetDeviceCommentsInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller);

private:
	AllocatorPointer Allocator;
	DestructorPointer Destructor;
	std::wstring className;
	std::wstring implementationName;
	unsigned int versionNo;
	std::wstring copyright;
	std::wstring comments;
};

#include "DeviceInfo.inl"
#endif
