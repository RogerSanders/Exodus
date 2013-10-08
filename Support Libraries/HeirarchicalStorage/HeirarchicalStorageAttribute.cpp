#include "HeirarchicalStorageAttribute.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
HeirarchicalStorageAttribute::HeirarchicalStorageAttribute()
{}

//----------------------------------------------------------------------------------------
HeirarchicalStorageAttribute::HeirarchicalStorageAttribute(const std::wstring& aname)
:name(aname)
{}

//----------------------------------------------------------------------------------------
//Name functions
//----------------------------------------------------------------------------------------
void HeirarchicalStorageAttribute::GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetName());
}

//----------------------------------------------------------------------------------------
void HeirarchicalStorageAttribute::SetNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller)
{
	SetName(marshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
//Stream functions
//----------------------------------------------------------------------------------------
void HeirarchicalStorageAttribute::ResetInternalStreamPosition() const
{
	buffer.SetStreamPos(0);
}

//----------------------------------------------------------------------------------------
void HeirarchicalStorageAttribute::EmptyInternalStream()
{
	buffer.Resize(0);
}

//----------------------------------------------------------------------------------------
Stream::IStream& HeirarchicalStorageAttribute::GetInternalStream() const
{
	return buffer;
}
