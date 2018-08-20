namespace Stream {

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
ByteOrder::ByteOrder(IStream::ByteOrder byteOrder)
:_byteOrder(byteOrder)
{ }

//----------------------------------------------------------------------------------------------------------------------
// Byte order functions
//----------------------------------------------------------------------------------------------------------------------
IStream::ByteOrder ByteOrder::GetByteOrder() const
{
	return _byteOrder;
}

//----------------------------------------------------------------------------------------------------------------------
void ByteOrder::SetByteOrder(IStream::ByteOrder abyteOrder)
{
	_byteOrder = abyteOrder;
}

//----------------------------------------------------------------------------------------------------------------------
// Stream operators
//----------------------------------------------------------------------------------------------------------------------
ViewText& operator>>(ViewText& stream, const ByteOrder& object)
{
	stream.SetViewByteOrder(object._byteOrder);
	return stream;
}

//----------------------------------------------------------------------------------------------------------------------
ViewText& operator<<(ViewText& stream, const ByteOrder& object)
{
	stream.SetViewByteOrder(object._byteOrder);
	return stream;
}

} // Close namespace Stream
