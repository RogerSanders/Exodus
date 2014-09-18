#ifndef __IDEVICEINFO_H__
#define __IDEVICEINFO_H__
#include <string>
class IDevice;

class IDeviceInfo
{
public:
	//Typedefs
	typedef IDevice* (*AllocatorPointer)(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID);
	typedef void (*DestructorPointer)(IDevice* device);

public:
	//Constructors
	virtual ~IDeviceInfo() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIDeviceInfoVersion();
	virtual unsigned int GetIDeviceInfoVersion() const = 0;

	//Getters
	virtual AllocatorPointer GetAllocator() const = 0;
	virtual DestructorPointer GetDestructor() const = 0;
	inline std::wstring GetDeviceClassName() const;
	inline std::wstring GetDeviceImplementationName() const;
	virtual unsigned int GetDeviceVersionNo() const = 0;
	inline std::wstring GetDeviceCopyright() const;
	inline std::wstring GetDeviceComments() const;

	//Setters
	inline void SetDeviceSettings(AllocatorPointer aAllocator, DestructorPointer aDestructor, const std::wstring& adeviceClassName, const std::wstring& adeviceImplementationName, unsigned int adeviceVersionNo, const std::wstring& adeviceCopyright, const std::wstring& adeviceComments);
	virtual void SetDeviceAllocators(AllocatorPointer aAllocator, DestructorPointer aDestructor) = 0;
	inline void SetDeviceClassName(const std::wstring& adeviceClassName);
	inline void SetDeviceImplementationName(const std::wstring& adeviceImplementationName);
	virtual void SetDeviceVersionNo(unsigned int adeviceVersionNo) = 0;
	inline void SetDeviceCopyright(const std::wstring& adeviceCopyright);
	inline void SetDeviceComments(const std::wstring& adeviceComments);

protected:
	//Getters
	virtual void GetDeviceClassNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void GetDeviceImplementationNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void GetDeviceCopyrightInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void GetDeviceCommentsInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;

	//Setters
	virtual void SetDeviceClassNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller) = 0;
	virtual void SetDeviceImplementationNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller) = 0;
	virtual void SetDeviceCopyrightInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller) = 0;
	virtual void SetDeviceCommentsInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller) = 0;
};

#include "IDeviceInfo.inl"
#endif
