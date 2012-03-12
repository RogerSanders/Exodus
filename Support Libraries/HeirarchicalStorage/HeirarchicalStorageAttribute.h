#ifndef __HEIRARCHICALSTORAGEATTRIBUTE_H__
#define __HEIRARCHICALSTORAGEATTRIBUTE_H__
#include "HeirarchicalStorageInterface/HeirarchicalStorageInterface.pkg"
#include "Stream/Stream.pkg"

class HeirarchicalStorageAttribute :public IHeirarchicalStorageAttribute
{
public:
	//Constructors
	HeirarchicalStorageAttribute();
	explicit HeirarchicalStorageAttribute(const std::wstring& aname);

protected:
	//Name functions
	virtual const wchar_t* GetNameInternal() const;
	virtual void SetNameInternal(const wchar_t* aname);

	//Stream functions
	virtual void ResetInternalStreamPosition() const;
	virtual void EmptyInternalStream();
	virtual Stream::IStream& GetInternalStream() const;

private:
	std::wstring name;
	mutable Stream::Buffer buffer;
};

#endif
