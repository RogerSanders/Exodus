#ifndef __SYSTEMINFO_H__
#define __SYSTEMINFO_H__
#include "SystemInterface/SystemInterface.pkg"
#include <string>

class SystemInfo :public ISystemInfo
{
public:
	//Interface version functions
	virtual unsigned int GetISystemInfoVersion() const;

	//Getters
	virtual AllocatorPointer GetAllocator() const;
	virtual DestructorPointer GetDestructor() const;
	virtual unsigned int GetSystemVersionNo() const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetSystemCopyright() const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetSystemComments() const;

	//Setters
	virtual void SetSystemSettings(AllocatorPointer aAllocator, DestructorPointer aDestructor, unsigned int asystemVersionNo, const MarshalSupport::Marshal::In<std::wstring>& asystemCopyright, const MarshalSupport::Marshal::In<std::wstring>& asystemComments);
	virtual void SetSystemAllocators(AllocatorPointer aAllocator, DestructorPointer aDestructor);
	virtual void SetSystemVersionNo(unsigned int asystemVersionNo);
	virtual void SetSystemCopyright(const MarshalSupport::Marshal::In<std::wstring>& asystemCopyright);
	virtual void SetSystemComments(const MarshalSupport::Marshal::In<std::wstring>& asystemmComments);

private:
	AllocatorPointer Allocator;
	DestructorPointer Destructor;
	unsigned int versionNo;
	std::wstring copyright;
	std::wstring comments;
};

#endif
