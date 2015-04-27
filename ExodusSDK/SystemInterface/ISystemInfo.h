#ifndef __ISYSTEMINFO_H__
#define __ISYSTEMINFO_H__
#include "MarshalSupport/MarshalSupport.pkg"
#include <string>
class ISystemGUIInterface;
class IGUIExtensionInterface;

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
	static inline unsigned int ThisISystemInfoVersion() { return 1; }
	virtual unsigned int GetISystemInfoVersion() const = 0;

	//Getters
	virtual AllocatorPointer GetAllocator() const = 0;
	virtual DestructorPointer GetDestructor() const = 0;
	virtual unsigned int GetSystemVersionNo() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetSystemCopyright() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetSystemComments() const = 0;

	//Setters
	virtual void SetSystemSettings(AllocatorPointer aAllocator, DestructorPointer aDestructor, unsigned int asystemVersionNo, const MarshalSupport::Marshal::In<std::wstring>& asystemCopyright, const MarshalSupport::Marshal::In<std::wstring>& asystemComments) = 0;
	virtual void SetSystemAllocators(AllocatorPointer aAllocator, DestructorPointer aDestructor) = 0;
	virtual void SetSystemVersionNo(unsigned int asystemVersionNo) = 0;
	virtual void SetSystemCopyright(const MarshalSupport::Marshal::In<std::wstring>& asystemCopyright) = 0;
	virtual void SetSystemComments(const MarshalSupport::Marshal::In<std::wstring>& asystemmComments) = 0;
};

#endif
