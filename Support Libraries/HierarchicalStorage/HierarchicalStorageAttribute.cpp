#include "HierarchicalStorageAttribute.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
HierarchicalStorageAttribute::HierarchicalStorageAttribute()
{}

//----------------------------------------------------------------------------------------
HierarchicalStorageAttribute::HierarchicalStorageAttribute(const std::wstring& aname)
:name(aname)
{}

//----------------------------------------------------------------------------------------
//Name functions
//----------------------------------------------------------------------------------------
void HierarchicalStorageAttribute::GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetName());
}

//----------------------------------------------------------------------------------------
void HierarchicalStorageAttribute::SetNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller)
{
	SetName(marshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
//Stream functions
//----------------------------------------------------------------------------------------
void HierarchicalStorageAttribute::ResetInternalStreamPosition() const
{
	buffer.SetStreamPos(0);
}

//----------------------------------------------------------------------------------------
void HierarchicalStorageAttribute::EmptyInternalStream()
{
	buffer.Resize(0);
}

//----------------------------------------------------------------------------------------
Stream::IStream& HierarchicalStorageAttribute::GetInternalStream() const
{
	return buffer;
}
