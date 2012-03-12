namespace Stream {

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ViewBinary::ViewBinary(IStream& astream)
:stream(astream)
{}

//----------------------------------------------------------------------------------------
//Binary stream functions
//----------------------------------------------------------------------------------------
template<class T> ViewBinary& ViewBinary::operator>>(T& data)
{
	stream.ReadData(data);
	return *this;
}

//----------------------------------------------------------------------------------------
template<class T> ViewBinary& ViewBinary::operator<<(const T& data)
{
	stream.WriteData(data);
	return *this;
}

} //Close namespace Stream
