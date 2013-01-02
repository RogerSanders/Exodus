namespace Stream {

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ViewBinary::ViewBinary(IStreamNonSeekable& astream)
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

//----------------------------------------------------------------------------------------
template<class T> ViewBinary& ViewBinary::operator>>(std::vector<T>& data)
{
	stream.ReadData(data, data.size());
	return *this;
}

//----------------------------------------------------------------------------------------
template<class T> ViewBinary& ViewBinary::operator<<(const std::vector<T>& data)
{
	stream.WriteData(data);
	return *this;
}

} //Close namespace Stream
