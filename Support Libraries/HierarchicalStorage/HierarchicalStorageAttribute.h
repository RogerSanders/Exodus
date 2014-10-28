#ifndef __HIERARCHICALSTORAGEATTRIBUTE_H__
#define __HIERARCHICALSTORAGEATTRIBUTE_H__
#include "HierarchicalStorageInterface/HierarchicalStorageInterface.pkg"
#include "Stream/Stream.pkg"

class HierarchicalStorageAttribute :public IHierarchicalStorageAttribute
{
public:
	//Constructors
	HierarchicalStorageAttribute();
	explicit HierarchicalStorageAttribute(const std::wstring& aname);

	//Name functions
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetName() const;
	virtual void SetName(const MarshalSupport::Marshal::In<std::wstring>& aname);

protected:
	//Stream functions
	virtual void ResetInternalStreamPosition() const;
	virtual void EmptyInternalStream();
	virtual Stream::IStream& GetInternalStream() const;

private:
	std::wstring name;
	mutable Stream::Buffer buffer;
};

#endif
