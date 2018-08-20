#ifndef __IDEVICEINFO_H__
#define __IDEVICEINFO_H__
#include "MarshalSupport/MarshalSupport.pkg"
#include <string>
class IDevice;
using namespace MarshalSupport::Operators;

class IDeviceInfo
{
public:
	// Typedefs
	typedef IDevice* (*AllocatorPointer)(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID);
	typedef void (*DestructorPointer)(IDevice* device);

public:
	// Constructors
	virtual ~IDeviceInfo() = 0 { }

	// Interface version functions
	static inline unsigned int ThisIDeviceInfoVersion() { return 1; }
	virtual unsigned int GetIDeviceInfoVersion() const = 0;

	// Getters
	virtual AllocatorPointer GetAllocator() const = 0;
	virtual DestructorPointer GetDestructor() const = 0;
	virtual Marshal::Ret<std::wstring> GetDeviceClassName() const = 0;
	virtual Marshal::Ret<std::wstring> GetDeviceImplementationName() const = 0;
	virtual unsigned int GetDeviceVersionNo() const = 0;
	virtual Marshal::Ret<std::wstring> GetDeviceCopyright() const = 0;
	virtual Marshal::Ret<std::wstring> GetDeviceComments() const = 0;

	// Setters
	virtual void SetDeviceSettings(AllocatorPointer allocator, DestructorPointer destructor, const Marshal::In<std::wstring>& deviceClassName, const Marshal::In<std::wstring>& deviceImplementationName, unsigned int deviceVersionNo, const Marshal::In<std::wstring>& deviceCopyright, const Marshal::In<std::wstring>& deviceComments) = 0;
	virtual void SetDeviceAllocators(AllocatorPointer allocator, DestructorPointer destructor) = 0;
	virtual void SetDeviceClassName(const Marshal::In<std::wstring>& deviceClassName) = 0;
	virtual void SetDeviceImplementationName(const Marshal::In<std::wstring>& deviceImplementationName) = 0;
	virtual void SetDeviceVersionNo(unsigned int deviceVersionNo) = 0;
	virtual void SetDeviceCopyright(const Marshal::In<std::wstring>& deviceCopyright) = 0;
	virtual void SetDeviceComments(const Marshal::In<std::wstring>& deviceComments) = 0;
};

#endif
