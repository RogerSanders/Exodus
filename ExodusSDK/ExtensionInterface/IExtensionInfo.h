#ifndef __IEXTENSIONINFO_H__
#define __IEXTENSIONINFO_H__
#include "MarshalSupport/MarshalSupport.pkg"
#include <string>
class IExtension;
using namespace MarshalSupport::Operators;

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
	virtual Marshal::Ret<std::wstring> GetExtensionClassName() const = 0;
	virtual Marshal::Ret<std::wstring> GetExtensionImplementationName() const = 0;
	virtual unsigned int GetExtensionVersionNo() const = 0;
	virtual Marshal::Ret<std::wstring> GetExtensionCopyright() const = 0;
	virtual Marshal::Ret<std::wstring> GetExtensionComments() const = 0;
	virtual bool GetIsPersistentGlobalExtension() const = 0;

	//Setters
	virtual void SetExtensionSettings(AllocatorPointer allocator, DestructorPointer destructor, const Marshal::In<std::wstring>& extensionClassName, const Marshal::In<std::wstring>& extensionImplementationName, unsigned int extensionVersionNo, const Marshal::In<std::wstring>& extensionCopyright, const Marshal::In<std::wstring>& extensionComments, bool persistentGlobalExtension = false) = 0;
	virtual void SetExtensionAllocators(AllocatorPointer allocator, DestructorPointer destructor) = 0;
	virtual void SetExtensionClassName(const Marshal::In<std::wstring>& extensionClassName) = 0;
	virtual void SetExtensionImplementationName(const Marshal::In<std::wstring>& extensionImplementationName) = 0;
	virtual void SetExtensionVersionNo(unsigned int extensionVersionNo) = 0;
	virtual void SetExtensionCopyright(const Marshal::In<std::wstring>& extensionCopyright) = 0;
	virtual void SetExtensionComments(const Marshal::In<std::wstring>& extensionComments) = 0;
	virtual void SetIsPersistentGlobalExtension(bool state) = 0;
};

#endif
