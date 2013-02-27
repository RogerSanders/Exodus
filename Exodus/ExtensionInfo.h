#ifndef __EXTENSIONINFO_H__
#define __EXTENSIONINFO_H__
#include "SystemInterface/SystemInterface.pkg"
#include <string>

class ExtensionInfo :public IExtensionInfo
{
public:
	virtual void SetExtensionVersionNo(unsigned int aextensionVersionNo);
	virtual void SetExtensionAllocators(IExtension* (aAllocator)(const wchar_t*, unsigned int), void (aDestructor)(IExtension*));
	unsigned int GetExtensionVersionNo() const;
	std::wstring GetExtensionName() const;

	//This is much neater using a typedef. See below for the correct syntax without a
	//typedef for the function pointers:
	//IExtension* (*GetAllocator() const)(const wchar_t*, unsigned int);
	//void (*GetDestructor() const)(IExtension*);
	typedef IExtension* (*AllocatorPointer)(const wchar_t*, unsigned int);
	typedef void (*DestructorPointer)(IExtension*);
	AllocatorPointer GetAllocator() const;
	DestructorPointer GetDestructor() const;

protected:
	virtual void SetExtensionNameInternal(const wchar_t* aextensionName);

private:
	unsigned int extensionVersionNo;
	std::wstring extensionName;
	AllocatorPointer Allocator;
	DestructorPointer Destructor;
};

#endif
