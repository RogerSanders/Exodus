#ifndef __HIERARCHICALSTORAGEATTRIBUTE_H__
#define __HIERARCHICALSTORAGEATTRIBUTE_H__
#include "HierarchicalStorageInterface/HierarchicalStorageInterface.pkg"
#include "Stream/Stream.pkg"

class HierarchicalStorageAttribute :public IHierarchicalStorageAttribute
{
public:
	//Constructors
	HierarchicalStorageAttribute();
	explicit HierarchicalStorageAttribute(const std::wstring& name);

	//Name functions
	virtual Marshal::Ret<std::wstring> GetName() const;
	virtual void SetName(const Marshal::In<std::wstring>& name);

protected:
	//Stream functions
	virtual void ResetInternalStreamPosition() const;
	virtual void EmptyInternalStream();
	virtual Stream::IStream& GetInternalStream() const;

private:
	std::wstring _name;
	mutable Stream::Buffer _buffer;
};

#endif
