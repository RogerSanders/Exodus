#ifndef __EXTENSIONINFO_H__
#define __EXTENSIONINFO_H__
#include "ExtensionInterface/ExtensionInterface.pkg"
#include <string>

class ExtensionInfo :public IExtensionInfo
{
public:
	// Constructors
	ExtensionInfo();

	// Interface version functions
	virtual unsigned int GetIExtensionInfoVersion() const;

	// Getters
	virtual AllocatorPointer GetAllocator() const;
	virtual DestructorPointer GetDestructor() const;
	virtual Marshal::Ret<std::wstring> GetExtensionClassName() const;
	virtual Marshal::Ret<std::wstring> GetExtensionImplementationName() const;
	virtual unsigned int GetExtensionVersionNo() const;
	virtual Marshal::Ret<std::wstring> GetExtensionCopyright() const;
	virtual Marshal::Ret<std::wstring> GetExtensionComments() const;
	virtual bool GetIsPersistentGlobalExtension() const;

	// Setters
	virtual void SetExtensionSettings(AllocatorPointer allocator, DestructorPointer destructor, const Marshal::In<std::wstring>& extensionClassName, const Marshal::In<std::wstring>& extensionImplementationName, unsigned int extensionVersionNo, const Marshal::In<std::wstring>& extensionCopyright, const Marshal::In<std::wstring>& extensionComments, bool persistentGlobalExtension = false);
	virtual void SetExtensionAllocators(AllocatorPointer allocator, DestructorPointer destructor);
	virtual void SetExtensionClassName(const Marshal::In<std::wstring>& extensionClassName);
	virtual void SetExtensionImplementationName(const Marshal::In<std::wstring>& extensionImplementationName);
	virtual void SetExtensionVersionNo(unsigned int extensionVersionNo);
	virtual void SetExtensionCopyright(const Marshal::In<std::wstring>& extensionCopyright);
	virtual void SetExtensionComments(const Marshal::In<std::wstring>& extensionComments);
	virtual void SetIsPersistentGlobalExtension(bool state);

private:
	AllocatorPointer _allocator;
	DestructorPointer _destructor;
	std::wstring _className;
	std::wstring _implementationName;
	unsigned int _versionNo;
	std::wstring _copyright;
	std::wstring _comments;
	bool _persistentGlobalExtension;
};

#endif
