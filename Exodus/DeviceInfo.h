#ifndef __DEVICEINFO_H__
#define __DEVICEINFO_H__
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include <string>

class DeviceInfo :public IDeviceInfo
{
public:
	//Interface version functions
	virtual unsigned int GetIDeviceInfoVersion() const;

	//Getters
	virtual AllocatorPointer GetAllocator() const;
	virtual DestructorPointer GetDestructor() const;
	inline std::wstring GetDeviceClassName() const;
	inline std::wstring GetDeviceImplementationName() const;
	virtual unsigned int GetDeviceVersionNo() const;
	inline std::wstring GetDeviceCopyright() const;
	inline std::wstring GetDeviceComments() const;

	//Setters
	inline void SetDeviceSettings(AllocatorPointer aAllocator, DestructorPointer aDestructor, const std::wstring& adeviceClassName, const std::wstring& adeviceImplementationName, unsigned int adeviceVersionNo, const std::wstring& adeviceCopyright, const std::wstring& adeviceComments);
	virtual void SetDeviceAllocators(AllocatorPointer aAllocator, DestructorPointer aDestructor);
	inline void SetDeviceClassName(const std::wstring& adeviceClassName);
	inline void SetDeviceImplementationName(const std::wstring& adeviceImplementationName);
	virtual void SetDeviceVersionNo(unsigned int adeviceVersionNo);
	inline void SetDeviceCopyright(const std::wstring& adeviceCopyright);
	inline void SetDeviceComments(const std::wstring& adeviceComments);

protected:
	//Getters
	virtual void GetDeviceClassNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void GetDeviceImplementationNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void GetDeviceCopyrightInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void GetDeviceCommentsInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;

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
