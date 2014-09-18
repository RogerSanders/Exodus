#ifndef __EXTENSIONINFO_H__
#define __EXTENSIONINFO_H__
#include "ExodusExtensionInterface/ExodusExtensionInterface.pkg"
#include <string>

class ExtensionInfo :public IExtensionInfo
{
public:
	//Interface version functions
	virtual unsigned int GetIExtensionInfoVersion() const;

	//Getters
	virtual AllocatorPointer GetAllocator() const;
	virtual DestructorPointer GetDestructor() const;
	inline std::wstring GetExtensionClassName() const;
	inline std::wstring GetExtensionImplementationName() const;
	virtual unsigned int GetExtensionVersionNo() const;
	inline std::wstring GetExtensionCopyright() const;
	inline std::wstring GetExtensionComments() const;

	//Setters
	inline void SetExtensionSettings(AllocatorPointer aAllocator, DestructorPointer aDestructor, const std::wstring& aextensionClassName, const std::wstring& aextensionImplementationName, unsigned int aextensionVersionNo, const std::wstring& aextensionCopyright, const std::wstring& aextensionComments);
	virtual void SetExtensionAllocators(AllocatorPointer aAllocator, DestructorPointer aDestructor);
	inline void SetExtensionClassName(const std::wstring& aextensionClassName);
	inline void SetExtensionImplementationName(const std::wstring& aextensionImplementationName);
	virtual void SetExtensionVersionNo(unsigned int aextensionVersionNo);
	inline void SetExtensionCopyright(const std::wstring& aextensionCopyright);
	inline void SetExtensionComments(const std::wstring& aextensionComments);

protected:
	//Getters
	virtual void GetExtensionClassNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void GetExtensionImplementationNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void GetExtensionCopyrightInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void GetExtensionCommentsInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;

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
