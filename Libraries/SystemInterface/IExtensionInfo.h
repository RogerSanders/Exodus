#ifndef __IEXTENSIONINFO_H__
#define __IEXTENSIONINFO_H__
#include <string>
class IExtension;

class IExtensionInfo
{
public:
	//Typedefs
	typedef IExtension* (*AllocatorPointer)(const wchar_t* instanceName, unsigned int moduleID);
	typedef void (*DestructorPointer)(IExtension* extension);

public:
	//Constructors
	virtual ~IExtensionInfo() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIExtensionInfoVersion();
	virtual unsigned int GetIExtensionInfoVersion() const = 0;

	//Setters
	inline void SetExtensionSettings(const std::wstring& aextensionName, AllocatorPointer aAllocator, DestructorPointer aDestructor, unsigned int aextensionVersionNo);
	inline void SetExtensionName(const std::wstring& aextensionName);
	virtual void SetExtensionVersionNo(unsigned int aextensionVersionNo) = 0;
	virtual void SetExtensionAllocators(AllocatorPointer aAllocator, DestructorPointer aDestructor) = 0;

protected:
	//Setters
	virtual void SetExtensionNameInternal(const wchar_t* aextensionName) = 0;
};

#include "IExtensionInfo.inl"
#endif
