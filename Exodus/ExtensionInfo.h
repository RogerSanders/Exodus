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
	virtual unsigned int GetExtensionVersionNo() const;
	std::wstring GetExtensionCopyright() const;
	std::wstring GetExtensionComments() const;

	//Setters
	inline void SetExtensionSettings(AllocatorPointer aAllocator, DestructorPointer aDestructor, const std::wstring& aextensionClassName, const std::wstring& aextensionImplementationName, unsigned int aextensionVersionNo, const std::wstring& aextensionCopyright, const std::wstring& aextensionComments);
	virtual void SetExtensionAllocators(AllocatorPointer aAllocator, DestructorPointer aDestructor);
	inline void SetExtensionClassName(const std::wstring& aextensionClassName);
	inline void SetExtensionImplementationName(const std::wstring& aextensionImplementationName);
	virtual void SetExtensionVersionNo(unsigned int aextensionVersionNo);
	inline void SetExtensionCopyright(const std::wstring& aextensionCopyright);
	inline void SetExtensionComments(const std::wstring& aextensionComments);

protected:
	//Setters
	virtual void SetExtensionClassNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller);
	virtual void SetExtensionImplementationNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller);
	virtual void SetExtensionCopyrightInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller);
	virtual void SetExtensionCommentsInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller);

private:
	AllocatorPointer Allocator;
	DestructorPointer Destructor;
	std::wstring className;
	std::wstring implementationName;
	unsigned int versionNo;
	std::wstring copyright;
	std::wstring comments;
};

#include "ExtensionInfo.inl"
#endif
