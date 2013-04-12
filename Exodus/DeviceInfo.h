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
	unsigned int GetDeviceVersionNo() const;
	std::wstring GetDeviceCopyright() const;
	std::wstring GetDeviceComments() const;

	//Setters
	virtual void SetDeviceAllocators(AllocatorPointer aAllocator, DestructorPointer aDestructor);
	virtual void SetDeviceVersionNo(unsigned int adeviceVersionNo);

protected:
	//Setters
	virtual void SetDeviceClassNameInternal(const wchar_t* adeviceClassName);
	virtual void SetDeviceImplementationNameInternal(const wchar_t* adeviceImplementationName);
	virtual void SetDeviceCopyrightInternal(const wchar_t* adeviceCopyright);
	virtual void SetDeviceCommentsInternal(const wchar_t* adeviceComments);

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
