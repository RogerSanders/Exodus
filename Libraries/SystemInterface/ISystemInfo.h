#ifndef __ISYSTEMINFO_H__
#define __ISYSTEMINFO_H__
class ISystemGUIInterface;
class IGUIExtensionInterface;
#include <string>

class ISystemInfo
{
public:
	//Typedefs
	typedef ISystemGUIInterface* (*AllocatorPointer)(IGUIExtensionInterface& guiInterface);
	typedef void (*DestructorPointer)(ISystemGUIInterface* system);

public:
	//Constructors
	virtual ~ISystemInfo() = 0 {}

	//Interface version functions
	static inline unsigned int ThisISystemInfoVersion();
	virtual unsigned int GetISystemInfoVersion() const = 0;

	//Getters
	virtual AllocatorPointer GetAllocator() const = 0;
	virtual DestructorPointer GetDestructor() const = 0;
	virtual unsigned int GetSystemVersionNo() const = 0;
	inline std::wstring GetSystemCopyright() const;
	inline std::wstring GetSystemComments() const;

	//Setters
	inline void SetSystemSettings(AllocatorPointer aAllocator, DestructorPointer aDestructor, unsigned int asystemVersionNo, const std::wstring& asystemCopyright, const std::wstring& asystemComments);
	virtual void SetSystemAllocators(AllocatorPointer aAllocator, DestructorPointer aDestructor) = 0;
	virtual void SetSystemVersionNo(unsigned int asystemVersionNo) = 0;
	inline void SetSystemCopyright(const std::wstring& asystemCopyright);
	inline void SetSystemComments(const std::wstring& asystemmComments);

protected:
	//Getters
	virtual void GetSystemCopyrightInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void GetSystemCommentsInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;

	//Setters
	virtual void SetSystemCopyrightInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller) = 0;
	virtual void SetSystemCommentsInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller) = 0;
};

#include "ISystemInfo.inl"
#endif
