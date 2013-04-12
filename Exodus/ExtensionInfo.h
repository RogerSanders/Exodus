#ifndef __EXTENSIONINFO_H__
#define __EXTENSIONINFO_H__
#include "SystemInterface/SystemInterface.pkg"
#include <string>

class ExtensionInfo :public IExtensionInfo
{
public:
	//Interface version functions
	virtual unsigned int GetIExtensionInfoVersion() const;

	//Getters
	//This is much neater using a typedef. See below for the correct syntax without a
	//typedef for the function pointers:
	//IExtension* (*GetAllocator() const)(const wchar_t*, const wchar_t*, unsigned int);
	//void (*GetDestructor() const)(IExtension*);
	AllocatorPointer GetAllocator() const;
	DestructorPointer GetDestructor() const;
	std::wstring GetExtensionClassName() const;
	std::wstring GetExtensionImplementationName() const;
	unsigned int GetExtensionVersionNo() const;
	std::wstring GetExtensionCopyright() const;
	std::wstring GetExtensionComments() const;

	//Setters
	virtual void SetExtensionVersionNo(unsigned int aextensionVersionNo);
	virtual void SetExtensionAllocators(AllocatorPointer aAllocator, DestructorPointer aDestructor);

protected:
	//Setters
	virtual void SetExtensionClassNameInternal(const wchar_t* aextensionClassName);
	virtual void SetExtensionImplementationNameInternal(const wchar_t* aextensionImplementationName);
	virtual void SetExtensionCopyrightInternal(const wchar_t* aextensionCopyright);
	virtual void SetExtensionCommentsInternal(const wchar_t* aextensionComments);

private:
	AllocatorPointer Allocator;
	DestructorPointer Destructor;
	std::wstring className;
	std::wstring implementationName;
	unsigned int versionNo;
	std::wstring copyright;
	std::wstring comments;
};

#endif
