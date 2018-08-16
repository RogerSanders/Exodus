#ifndef __EXTENSIONINFO_H__
#define __EXTENSIONINFO_H__
#include "ExtensionInterface/ExtensionInterface.pkg"
#include <string>

class ExtensionInfo :public IExtensionInfo
{
public:
	//Constructors
	ExtensionInfo();

	//Interface version functions
	virtual unsigned int GetIExtensionInfoVersion() const;

	//Getters
	virtual AllocatorPointer GetAllocator() const;
	virtual DestructorPointer GetDestructor() const;
	virtual Marshal::Ret<std::wstring> GetExtensionClassName() const;
	virtual Marshal::Ret<std::wstring> GetExtensionImplementationName() const;
	virtual unsigned int GetExtensionVersionNo() const;
	virtual Marshal::Ret<std::wstring> GetExtensionCopyright() const;
	virtual Marshal::Ret<std::wstring> GetExtensionComments() const;
	virtual bool GetIsPersistentGlobalExtension() const;

	//Setters
	virtual void SetExtensionSettings(AllocatorPointer aAllocator, DestructorPointer aDestructor, const Marshal::In<std::wstring>& aextensionClassName, const Marshal::In<std::wstring>& aextensionImplementationName, unsigned int aextensionVersionNo, const Marshal::In<std::wstring>& aextensionCopyright, const Marshal::In<std::wstring>& aextensionComments, bool apersistentGlobalExtension = false);
	virtual void SetExtensionAllocators(AllocatorPointer aAllocator, DestructorPointer aDestructor);
	virtual void SetExtensionClassName(const Marshal::In<std::wstring>& aextensionClassName);
	virtual void SetExtensionImplementationName(const Marshal::In<std::wstring>& aextensionImplementationName);
	virtual void SetExtensionVersionNo(unsigned int aextensionVersionNo);
	virtual void SetExtensionCopyright(const Marshal::In<std::wstring>& aextensionCopyright);
	virtual void SetExtensionComments(const Marshal::In<std::wstring>& aextensionComments);
	virtual void SetIsPersistentGlobalExtension(bool state);

private:
	AllocatorPointer Allocator;
	DestructorPointer Destructor;
	std::wstring className;
	std::wstring implementationName;
	unsigned int versionNo;
	std::wstring copyright;
	std::wstring comments;
	bool persistentGlobalExtension;
};

#endif
