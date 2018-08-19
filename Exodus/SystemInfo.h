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
	virtual Marshal::Ret<std::wstring> GetSystemCopyright() const;
	virtual Marshal::Ret<std::wstring> GetSystemComments() const;

	//Setters
	virtual void SetSystemSettings(AllocatorPointer allocator, DestructorPointer destructor, unsigned int systemVersionNo, const Marshal::In<std::wstring>& systemCopyright, const Marshal::In<std::wstring>& systemComments);
	virtual void SetSystemAllocators(AllocatorPointer allocator, DestructorPointer destructor);
	virtual void SetSystemVersionNo(unsigned int systemVersionNo);
	virtual void SetSystemCopyright(const Marshal::In<std::wstring>& systemCopyright);
	virtual void SetSystemComments(const Marshal::In<std::wstring>& systemmComments);

private:
	AllocatorPointer _allocator;
	DestructorPointer _destructor;
	unsigned int _versionNo;
	std::wstring _copyright;
	std::wstring _comments;
};

#endif
