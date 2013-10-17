namespace Stream {

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ViewBinary::ViewBinary(IStreamNonSeekable& astream)
:stream(astream), byteOrder(IStream::BYTEORDER_PLATFORM), noErrorState(true)
{}

//----------------------------------------------------------------------------------------
//State functions
//----------------------------------------------------------------------------------------
bool ViewBinary::NoErrorsOccurred() const
{
	return noErrorState;
}

//----------------------------------------------------------------------------------------
void ViewBinary::ClearErrorState()
{
	noErrorState = true;
}

//----------------------------------------------------------------------------------------
bool ViewBinary::IsAtEnd() const
{
	return stream.IsAtEnd();
}

//----------------------------------------------------------------------------------------
IStream::ByteOrder ViewBinary::GetViewByteOrder() const
{
	return byteOrder;
}

//----------------------------------------------------------------------------------------
void ViewBinary::SetViewByteOrder(IStream::ByteOrder abyteOrder)
{
	byteOrder = abyteOrder;
}

//----------------------------------------------------------------------------------------
//Binary stream functions
//----------------------------------------------------------------------------------------
template<class T> ViewBinary& ViewBinary::operator>>(T& data)
{
	noErrorState &= stream.ReadData(byteOrder, data);
	return *this;
}

//----------------------------------------------------------------------------------------
template<class T> ViewBinary& ViewBinary::operator<<(const T& data)
{
	noErrorState &= stream.WriteData(byteOrder, data);
	return *this;
}

//----------------------------------------------------------------------------------------
template<class T> ViewBinary& ViewBinary::operator>>(std::vector<T>& data)
{
	size_t elementCount = data.size();
	for(size_t i = 0; i < elementCount; ++i)
	{
		*this >> data[i];
	}
	return *this;
}

//----------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator>>(std::vector<bool>& data)
{
	noErrorState &= stream.ReadData(byteOrder, data, data.size());
	return *this;
}

//----------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator>>(std::vector<char>& data)
{
	noErrorState &= stream.ReadData(byteOrder, data, data.size());
	return *this;
}

//----------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator>>(std::vector<signed char>& data)
{
	noErrorState &= stream.ReadData(byteOrder, data, data.size());
	return *this;
}

//----------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator>>(std::vector<unsigned char>& data)
{
	noErrorState &= stream.ReadData(byteOrder, data, data.size());
	return *this;
}

//----------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator>>(std::vector<wchar_t>& data)
{
	noErrorState &= stream.ReadData(byteOrder, data, data.size());
	return *this;
}

//----------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator>>(std::vector<short>& data)
{
	noErrorState &= stream.ReadData(byteOrder, data, data.size());
	return *this;
}

//----------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator>>(std::vector<unsigned short>& data)
{
	noErrorState &= stream.ReadData(byteOrder, data, data.size());
	return *this;
}

//----------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator>>(std::vector<int>& data)
{
	noErrorState &= stream.ReadData(byteOrder, data, data.size());
	return *this;
}

//----------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator>>(std::vector<unsigned int>& data)
{
	noErrorState &= stream.ReadData(byteOrder, data, data.size());
	return *this;
}

//----------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator>>(std::vector<long>& data)
{
	noErrorState &= stream.ReadData(byteOrder, data, data.size());
	return *this;
}

//----------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator>>(std::vector<unsigned long>& data)
{
	noErrorState &= stream.ReadData(byteOrder, data, data.size());
	return *this;
}

//----------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator>>(std::vector<long long>& data)
{
	noErrorState &= stream.ReadData(byteOrder, data, data.size());
	return *this;
}

//----------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator>>(std::vector<unsigned long long>& data)
{
	noErrorState &= stream.ReadData(byteOrder, data, data.size());
	return *this;
}

//----------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator>>(std::vector<float>& data)
{
	noErrorState &= stream.ReadData(byteOrder, data, data.size());
	return *this;
}

//----------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator>>(std::vector<double>& data)
{
	noErrorState &= stream.ReadData(byteOrder, data, data.size());
	return *this;
}

//----------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator>>(std::vector<long double>& data)
{
	noErrorState &= stream.ReadData(byteOrder, data, data.size());
	return *this;
}

//----------------------------------------------------------------------------------------
template<class T> ViewBinary& ViewBinary::operator<<(const std::vector<T>& data)
{
	size_t elementCount = data.size();
	for(size_t i = 0; i < elementCount; ++i)
	{
		*this << data[i];
	}
	return *this;
}

//----------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator<<(const std::vector<bool>& data)
{
	noErrorState &= stream.WriteData(byteOrder, data);
	return *this;
}

//----------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator<<(const std::vector<char>& data)
{
	noErrorState &= stream.WriteData(byteOrder, data);
	return *this;
}

//----------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator<<(const std::vector<signed char>& data)
{
	noErrorState &= stream.WriteData(byteOrder, data);
	return *this;
}

//----------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator<<(const std::vector<unsigned char>& data)
{
	noErrorState &= stream.WriteData(byteOrder, data);
	return *this;
}

//----------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator<<(const std::vector<wchar_t>& data)
{
	noErrorState &= stream.WriteData(byteOrder, data);
	return *this;
}

//----------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator<<(const std::vector<short>& data)
{
	noErrorState &= stream.WriteData(byteOrder, data);
	return *this;
}

//----------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator<<(const std::vector<unsigned short>& data)
{
	noErrorState &= stream.WriteData(byteOrder, data);
	return *this;
}

//----------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator<<(const std::vector<int>& data)
{
	noErrorState &= stream.WriteData(byteOrder, data);
	return *this;
}

//----------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator<<(const std::vector<unsigned int>& data)
{
	noErrorState &= stream.WriteData(byteOrder, data);
	return *this;
}

//----------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator<<(const std::vector<long>& data)
{
	noErrorState &= stream.WriteData(byteOrder, data);
	return *this;
}

//----------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator<<(const std::vector<unsigned long>& data)
{
	noErrorState &= stream.WriteData(byteOrder, data);
	return *this;
}

//----------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator<<(const std::vector<long long>& data)
{
	noErrorState &= stream.WriteData(byteOrder, data);
	return *this;
}

//----------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator<<(const std::vector<unsigned long long>& data)
{
	noErrorState &= stream.WriteData(byteOrder, data);
	return *this;
}

//----------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator<<(const std::vector<float>& data)
{
	noErrorState &= stream.WriteData(byteOrder, data);
	return *this;
}

//----------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator<<(const std::vector<double>& data)
{
	noErrorState &= stream.WriteData(byteOrder, data);
	return *this;
}

//----------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator<<(const std::vector<long double>& data)
{
	noErrorState &= stream.WriteData(byteOrder, data);
	return *this;
}

} //Close namespace Stream
