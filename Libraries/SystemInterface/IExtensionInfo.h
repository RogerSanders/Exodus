#ifndef __IEXTENSIONINFO_H__
#define __IEXTENSIONINFO_H__
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
	static inline unsigned int ThisIExtensionInfoVersion();
	virtual unsigned int GetIExtensionInfoVersion() const = 0;

	//Setters
	inline void SetExtensionSettings(AllocatorPointer aAllocator, DestructorPointer aDestructor, const std::wstring& aextensionClassName, const std::wstring& aextensionImplementationName, unsigned int aextensionVersionNo, const std::wstring& aextensionCopyright, const std::wstring& aextensionComments);
	virtual void SetExtensionAllocators(AllocatorPointer aAllocator, DestructorPointer aDestructor) = 0;
	inline void SetExtensionClassName(const std::wstring& aextensionClassName);
	inline void SetExtensionImplementationName(const std::wstring& aextensionImplementationName);
	virtual void SetExtensionVersionNo(unsigned int aextensionVersionNo) = 0;
	inline void SetExtensionCopyright(const std::wstring& aextensionCopyright);
	inline void SetExtensionComments(const std::wstring& aextensionComments);

protected:
	//Setters
	virtual void SetExtensionClassNameInternal(const wchar_t* aextensionClassName) = 0;
	virtual void SetExtensionImplementationNameInternal(const wchar_t* aextensionImplementationName) = 0;
	virtual void SetExtensionCopyrightInternal(const wchar_t* aextensionCopyright) = 0;
	virtual void SetExtensionCommentsInternal(const wchar_t* aextensionComments) = 0;
};

#include "IExtensionInfo.inl"
#endif
