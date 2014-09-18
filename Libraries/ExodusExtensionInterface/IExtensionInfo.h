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

	//Getters
	virtual AllocatorPointer GetAllocator() const = 0;
	virtual DestructorPointer GetDestructor() const = 0;
	inline std::wstring GetExtensionClassName() const;
	inline std::wstring GetExtensionImplementationName() const;
	virtual unsigned int GetExtensionVersionNo() const = 0;
	inline std::wstring GetExtensionCopyright() const;
	inline std::wstring GetExtensionComments() const;

	//Setters
	inline void SetExtensionSettings(AllocatorPointer aAllocator, DestructorPointer aDestructor, const std::wstring& aextensionClassName, const std::wstring& aextensionImplementationName, unsigned int aextensionVersionNo, const std::wstring& aextensionCopyright, const std::wstring& aextensionComments);
	virtual void SetExtensionAllocators(AllocatorPointer aAllocator, DestructorPointer aDestructor) = 0;
	inline void SetExtensionClassName(const std::wstring& aextensionClassName);
	inline void SetExtensionImplementationName(const std::wstring& aextensionImplementationName);
	virtual void SetExtensionVersionNo(unsigned int aextensionVersionNo) = 0;
	inline void SetExtensionCopyright(const std::wstring& aextensionCopyright);
	inline void SetExtensionComments(const std::wstring& aextensionComments);

protected:
	//Getters
	virtual void GetExtensionClassNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void GetExtensionImplementationNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void GetExtensionCopyrightInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void GetExtensionCommentsInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;

	//Setters
	virtual void SetExtensionClassNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller) = 0;
	virtual void SetExtensionImplementationNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller) = 0;
	virtual void SetExtensionCopyrightInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller) = 0;
	virtual void SetExtensionCommentsInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller) = 0;
};

#include "IExtensionInfo.inl"
#endif
