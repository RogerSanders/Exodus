#include "HierarchicalStorageAttribute.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
HierarchicalStorageAttribute::HierarchicalStorageAttribute()
{ }

//----------------------------------------------------------------------------------------------------------------------
HierarchicalStorageAttribute::HierarchicalStorageAttribute(const std::wstring& name)
:_name(name)
{ }

//----------------------------------------------------------------------------------------------------------------------
// Name functions
//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> HierarchicalStorageAttribute::GetName() const
{
	return _name;
}

//----------------------------------------------------------------------------------------------------------------------
void HierarchicalStorageAttribute::SetName(const Marshal::In<std::wstring>& name)
{
	_name = name;
}

//----------------------------------------------------------------------------------------------------------------------
// Stream functions
//----------------------------------------------------------------------------------------------------------------------
void HierarchicalStorageAttribute::ResetInternalStreamPosition() const
{
	_buffer.SetStreamPos(0);
}

//----------------------------------------------------------------------------------------------------------------------
void HierarchicalStorageAttribute::EmptyInternalStream()
{
	_buffer.Resize(0);
}

//----------------------------------------------------------------------------------------------------------------------
Stream::IStream& HierarchicalStorageAttribute::GetInternalStream() const
{
	return _buffer;
}
