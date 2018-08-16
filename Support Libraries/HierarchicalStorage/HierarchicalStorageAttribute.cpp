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
Marshal::Ret<std::wstring> HierarchicalStorageAttribute::GetName() const
{
	return name;
}

//----------------------------------------------------------------------------------------
void HierarchicalStorageAttribute::SetName(const Marshal::In<std::wstring>& aname)
{
	name = aname;
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
