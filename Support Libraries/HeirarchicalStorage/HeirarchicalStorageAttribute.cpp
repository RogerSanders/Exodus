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
const wchar_t* HeirarchicalStorageAttribute::GetNameInternal() const
{
	return name.c_str();
}

//----------------------------------------------------------------------------------------
void HeirarchicalStorageAttribute::SetNameInternal(const wchar_t* aname)
{
	name = aname;
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
