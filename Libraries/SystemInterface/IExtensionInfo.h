#ifndef __IEXTENSIONINFO_H__
#define __IEXTENSIONINFO_H__
#include <string>
class IExtension;

class IExtensionInfo
{
public:
	//Constructors
	virtual ~IExtensionInfo() = 0 {}

	//Setters
	inline void SetExtensionSettings(const std::wstring& aextensionName, IExtension* (aAllocator)(const wchar_t*, unsigned int), void (aDestructor)(IExtension*), unsigned int aextensionVersionNo);
	inline void SetExtensionName(const std::wstring& aextensionName);
	virtual void SetExtensionVersionNo(unsigned int aextensionVersionNo) = 0;
	virtual void SetExtensionAllocators(IExtension* (aAllocator)(const wchar_t*, unsigned int), void (aDestructor)(IExtension*)) = 0;

protected:
	//Setters
	virtual void SetExtensionNameInternal(const wchar_t* aextensionName) = 0;
};

#include "IExtensionInfo.inl"
#endif
