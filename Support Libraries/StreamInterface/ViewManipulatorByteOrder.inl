namespace Stream {

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ByteOrder::ByteOrder(IStream::ByteOrder abyteOrder)
:byteOrder(abyteOrder)
{}

//----------------------------------------------------------------------------------------
//Byte order functions
//----------------------------------------------------------------------------------------
IStream::ByteOrder ByteOrder::GetByteOrder() const
{
	return byteOrder;
}

//----------------------------------------------------------------------------------------
void ByteOrder::SetByteOrder(IStream::ByteOrder abyteOrder)
{
	byteOrder = abyteOrder;
}

//----------------------------------------------------------------------------------------
//Stream operators
//----------------------------------------------------------------------------------------
ViewText& operator>>(ViewText& stream, const ByteOrder& object)
{
	stream.SetViewByteOrder(object.byteOrder);
}

//----------------------------------------------------------------------------------------
ViewText& operator<<(ViewText& stream, const ByteOrder& object)
{
	stream.SetViewByteOrder(object.byteOrder);
}

} //Close namespace Stream
