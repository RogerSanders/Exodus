namespace Stream {

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
ViewBinary::ViewBinary(IStreamNonSeekable& stream)
:_stream(stream), _byteOrder(IStream::ByteOrder::Platform), _noErrorState(true)
{ }

//----------------------------------------------------------------------------------------------------------------------
// State functions
//----------------------------------------------------------------------------------------------------------------------
bool ViewBinary::NoErrorsOccurred() const
{
	return _noErrorState;
}

//----------------------------------------------------------------------------------------------------------------------
void ViewBinary::ClearErrorState()
{
	_noErrorState = true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ViewBinary::IsAtEnd() const
{
	return _stream.IsAtEnd();
}

//----------------------------------------------------------------------------------------------------------------------
IStream::ByteOrder ViewBinary::GetViewByteOrder() const
{
	return _byteOrder;
}

//----------------------------------------------------------------------------------------------------------------------
void ViewBinary::SetViewByteOrder(IStream::ByteOrder byteOrder)
{
	_byteOrder = byteOrder;
}

//----------------------------------------------------------------------------------------------------------------------
// Binary stream functions
//----------------------------------------------------------------------------------------------------------------------
template<class T> ViewBinary& ViewBinary::operator>>(T& data)
{
	_noErrorState &= _stream.ReadData(_byteOrder, data);
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
template<class T> ViewBinary& ViewBinary::operator<<(const T& data)
{
	_noErrorState &= _stream.WriteData(_byteOrder, data);
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
template<class T> ViewBinary& ViewBinary::operator>>(std::vector<T>& data)
{
	size_t elementCount = data.size();
	for (size_t i = 0; i < elementCount; ++i)
	{
		*this >> data[i];
	}
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator>>(std::vector<bool>& data)
{
	_noErrorState &= _stream.ReadData(_byteOrder, data, data.size());
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator>>(std::vector<char>& data)
{
	_noErrorState &= _stream.ReadData(_byteOrder, data, data.size());
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator>>(std::vector<signed char>& data)
{
	_noErrorState &= _stream.ReadData(_byteOrder, data, data.size());
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator>>(std::vector<unsigned char>& data)
{
	_noErrorState &= _stream.ReadData(_byteOrder, data, data.size());
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator>>(std::vector<wchar_t>& data)
{
	_noErrorState &= _stream.ReadData(_byteOrder, data, data.size());
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator>>(std::vector<short>& data)
{
	_noErrorState &= _stream.ReadData(_byteOrder, data, data.size());
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator>>(std::vector<unsigned short>& data)
{
	_noErrorState &= _stream.ReadData(_byteOrder, data, data.size());
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator>>(std::vector<int>& data)
{
	_noErrorState &= _stream.ReadData(_byteOrder, data, data.size());
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator>>(std::vector<unsigned int>& data)
{
	_noErrorState &= _stream.ReadData(_byteOrder, data, data.size());
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator>>(std::vector<long>& data)
{
	_noErrorState &= _stream.ReadData(_byteOrder, data, data.size());
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator>>(std::vector<unsigned long>& data)
{
	_noErrorState &= _stream.ReadData(_byteOrder, data, data.size());
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator>>(std::vector<long long>& data)
{
	_noErrorState &= _stream.ReadData(_byteOrder, data, data.size());
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator>>(std::vector<unsigned long long>& data)
{
	_noErrorState &= _stream.ReadData(_byteOrder, data, data.size());
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator>>(std::vector<float>& data)
{
	_noErrorState &= _stream.ReadData(_byteOrder, data, data.size());
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator>>(std::vector<double>& data)
{
	_noErrorState &= _stream.ReadData(_byteOrder, data, data.size());
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator>>(std::vector<long double>& data)
{
	_noErrorState &= _stream.ReadData(_byteOrder, data, data.size());
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
template<class T> ViewBinary& ViewBinary::operator<<(const std::vector<T>& data)
{
	size_t elementCount = data.size();
	for (size_t i = 0; i < elementCount; ++i)
	{
		*this << data[i];
	}
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator<<(const std::vector<bool>& data)
{
	_noErrorState &= _stream.WriteData(_byteOrder, data);
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator<<(const std::vector<char>& data)
{
	_noErrorState &= _stream.WriteData(_byteOrder, data);
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator<<(const std::vector<signed char>& data)
{
	_noErrorState &= _stream.WriteData(_byteOrder, data);
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator<<(const std::vector<unsigned char>& data)
{
	_noErrorState &= _stream.WriteData(_byteOrder, data);
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator<<(const std::vector<wchar_t>& data)
{
	_noErrorState &= _stream.WriteData(_byteOrder, data);
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator<<(const std::vector<short>& data)
{
	_noErrorState &= _stream.WriteData(_byteOrder, data);
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator<<(const std::vector<unsigned short>& data)
{
	_noErrorState &= _stream.WriteData(_byteOrder, data);
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator<<(const std::vector<int>& data)
{
	_noErrorState &= _stream.WriteData(_byteOrder, data);
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator<<(const std::vector<unsigned int>& data)
{
	_noErrorState &= _stream.WriteData(_byteOrder, data);
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator<<(const std::vector<long>& data)
{
	_noErrorState &= _stream.WriteData(_byteOrder, data);
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator<<(const std::vector<unsigned long>& data)
{
	_noErrorState &= _stream.WriteData(_byteOrder, data);
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator<<(const std::vector<long long>& data)
{
	_noErrorState &= _stream.WriteData(_byteOrder, data);
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator<<(const std::vector<unsigned long long>& data)
{
	_noErrorState &= _stream.WriteData(_byteOrder, data);
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator<<(const std::vector<float>& data)
{
	_noErrorState &= _stream.WriteData(_byteOrder, data);
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator<<(const std::vector<double>& data)
{
	_noErrorState &= _stream.WriteData(_byteOrder, data);
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ViewBinary& ViewBinary::operator<<(const std::vector<long double>& data)
{
	_noErrorState &= _stream.WriteData(_byteOrder, data);
	return *this;
}

} // Close namespace Stream
