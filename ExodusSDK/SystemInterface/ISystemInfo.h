#ifndef __ISYSTEMINFO_H__
#define __ISYSTEMINFO_H__
#include "MarshalSupport/MarshalSupport.pkg"
#include <string>
class ISystemGUIInterface;
class IGUIExtensionInterface;
using namespace MarshalSupport::Operators;

class ISystemInfo
{
public:
	// Typedefs
	typedef ISystemGUIInterface* (*AllocatorPointer)(IGUIExtensionInterface& guiInterface);
	typedef void (*DestructorPointer)(ISystemGUIInterface* system);

public:
	// Constructors
	inline virtual ~ISystemInfo() = 0;

	// Interface version functions
	static inline unsigned int ThisISystemInfoVersion() { return 1; }
	virtual unsigned int GetISystemInfoVersion() const = 0;

	// Getters
	virtual AllocatorPointer GetAllocator() const = 0;
	virtual DestructorPointer GetDestructor() const = 0;
	virtual unsigned int GetSystemVersionNo() const = 0;
	virtual Marshal::Ret<std::wstring> GetSystemCopyright() const = 0;
	virtual Marshal::Ret<std::wstring> GetSystemComments() const = 0;

	// Setters
	virtual void SetSystemSettings(AllocatorPointer allocator, DestructorPointer destructor, unsigned int systemVersionNo, const Marshal::In<std::wstring>& systemCopyright, const Marshal::In<std::wstring>& systemComments) = 0;
	virtual void SetSystemAllocators(AllocatorPointer allocator, DestructorPointer destructor) = 0;
	virtual void SetSystemVersionNo(unsigned int systemVersionNo) = 0;
	virtual void SetSystemCopyright(const Marshal::In<std::wstring>& systemCopyright) = 0;
	virtual void SetSystemComments(const Marshal::In<std::wstring>& systemmComments) = 0;
};
ISystemInfo::~ISystemInfo() { }

#endif
