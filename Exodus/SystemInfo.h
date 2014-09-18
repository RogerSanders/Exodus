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
	inline std::wstring GetSystemCopyright() const;
	inline std::wstring GetSystemComments() const;

	//Setters
	inline void SetSystemSettings(AllocatorPointer aAllocator, DestructorPointer aDestructor, unsigned int asystemVersionNo, const std::wstring& asystemCopyright, const std::wstring& asystemComments);
	virtual void SetSystemAllocators(AllocatorPointer aAllocator, DestructorPointer aDestructor);
	virtual void SetSystemVersionNo(unsigned int asystemVersionNo);
	inline void SetSystemCopyright(const std::wstring& asystemCopyright);
	inline void SetSystemComments(const std::wstring& asystemmComments);

protected:
	//Getters
	virtual void GetSystemCopyrightInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void GetSystemCommentsInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;

	//Setters
	virtual void SetSystemCopyrightInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller);
	virtual void SetSystemCommentsInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller);

private:
	AllocatorPointer Allocator;
	DestructorPointer Destructor;
	unsigned int versionNo;
	std::wstring copyright;
	std::wstring comments;
};

#include "SystemInfo.inl"
#endif
