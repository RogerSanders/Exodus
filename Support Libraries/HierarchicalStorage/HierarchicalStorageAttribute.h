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
	inline std::wstring GetName() const;
	inline void SetName(const std::wstring& aname);

protected:
	//Name functions
	virtual void GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void SetNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller);

	//Stream functions
	virtual void ResetInternalStreamPosition() const;
	virtual void EmptyInternalStream();
	virtual Stream::IStream& GetInternalStream() const;

private:
	std::wstring name;
	mutable Stream::Buffer buffer;
};

#include "HierarchicalStorageAttribute.inl"
#endif
