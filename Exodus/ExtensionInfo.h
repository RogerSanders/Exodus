#ifndef __EXTENSIONINFO_H__
#define __EXTENSIONINFO_H__
#include "SystemInterface/SystemInterface.pkg"
#include <string>

class ExtensionInfo :public IExtensionInfo
{
public:
	//Interface version functions
	virtual unsigned int GetIExtensionInfoVersion() const;

	//Setters
	virtual void SetExtensionVersionNo(unsigned int aextensionVersionNo);
	virtual void SetExtensionAllocators(AllocatorPointer aAllocator, DestructorPointer aDestructor);

	//Getters
	unsigned int GetExtensionVersionNo() const;
	std::wstring GetExtensionName() const;
	//This is much neater using a typedef. See below for the correct syntax without a
	//typedef for the function pointers:
	//IExtension* (*GetAllocator() const)(const wchar_t*, unsigned int);
	//void (*GetDestructor() const)(IExtension*);
	AllocatorPointer GetAllocator() const;
	DestructorPointer GetDestructor() const;

protected:
	//Setters
	virtual void SetExtensionNameInternal(const wchar_t* aextensionName);

private:
	unsigned int extensionVersionNo;
	std::wstring extensionName;
	AllocatorPointer Allocator;
	DestructorPointer Destructor;
};

#endif
