#ifndef __IEXTENSIONINFO_H__
#define __IEXTENSIONINFO_H__
#include "MarshalSupport/MarshalSupport.pkg"
#include <string>
class IExtension;

class IExtensionInfo
{
public:
	//Typedefs
	typedef IExtension* (*AllocatorPointer)(const wchar_t* implementationName, const wchar_t* instanceName, unsigned int moduleID);
	typedef void (*DestructorPointer)(IExtension* extension);

public:
	//Constructors
	virtual ~IExtensionInfo() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIExtensionInfoVersion() { return 1; }
	virtual unsigned int GetIExtensionInfoVersion() const = 0;

	//Getters
	virtual AllocatorPointer GetAllocator() const = 0;
	virtual DestructorPointer GetDestructor() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetExtensionClassName() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetExtensionImplementationName() const = 0;
	virtual unsigned int GetExtensionVersionNo() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetExtensionCopyright() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetExtensionComments() const = 0;
	virtual bool GetIsPersistentGlobalExtension() const = 0;

	//Setters
	virtual void SetExtensionSettings(AllocatorPointer aAllocator, DestructorPointer aDestructor, const MarshalSupport::Marshal::In<std::wstring>& aextensionClassName, const MarshalSupport::Marshal::In<std::wstring>& aextensionImplementationName, unsigned int aextensionVersionNo, const MarshalSupport::Marshal::In<std::wstring>& aextensionCopyright, const MarshalSupport::Marshal::In<std::wstring>& aextensionComments, bool apersistentGlobalExtension = false) = 0;
	virtual void SetExtensionAllocators(AllocatorPointer aAllocator, DestructorPointer aDestructor) = 0;
	virtual void SetExtensionClassName(const MarshalSupport::Marshal::In<std::wstring>& aextensionClassName) = 0;
	virtual void SetExtensionImplementationName(const MarshalSupport::Marshal::In<std::wstring>& aextensionImplementationName) = 0;
	virtual void SetExtensionVersionNo(unsigned int aextensionVersionNo) = 0;
	virtual void SetExtensionCopyright(const MarshalSupport::Marshal::In<std::wstring>& aextensionCopyright) = 0;
	virtual void SetExtensionComments(const MarshalSupport::Marshal::In<std::wstring>& aextensionComments) = 0;
	virtual void SetIsPersistentGlobalExtension(bool state) = 0;
};

#endif
