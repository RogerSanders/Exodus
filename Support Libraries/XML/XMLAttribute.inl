//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
XMLAttribute::XMLAttribute()
:value(Stream::IStream::TEXTENCODING_UTF16, 0)
{}

//----------------------------------------------------------------------------------------
XMLAttribute::XMLAttribute(const std::wstring& aname)
:value(Stream::IStream::TEXTENCODING_UTF16, 0), name(aname)
{}

//----------------------------------------------------------------------------------------
template<class T> XMLAttribute::XMLAttribute(const std::wstring& aname, const T& adata)
:value(Stream::IStream::TEXTENCODING_UTF16, 0), name(aname)
{
	InsertValue(adata);
}

//----------------------------------------------------------------------------------------
template<class T> XMLAttribute::XMLAttribute(const std::wstring& aname, const T& adata, unsigned int length)
:value(Stream::IStream::TEXTENCODING_UTF16, 0), name(aname)
{
	InsertHexValue(adata, length);
}

//----------------------------------------------------------------------------------------
//Name functions
//----------------------------------------------------------------------------------------
std::wstring XMLAttribute::GetName() const
{
	return name;
}

//----------------------------------------------------------------------------------------
void XMLAttribute::SetName(const std::wstring& aname)
{
	name = aname;
}

//----------------------------------------------------------------------------------------
//Value read functions
//----------------------------------------------------------------------------------------
std::wstring XMLAttribute::GetValue() const
{
	value.SetStreamPos(0);
	Stream::ViewText bufferView(value);
	std::wstring data;
	bufferView >> data;
	return data;
}

//----------------------------------------------------------------------------------------
template<class T> T XMLAttribute::ExtractValue()
{
	value.SetStreamPos(0);
	Stream::ViewText bufferView(value);
	T data;
	bufferView >> data;
	return data;
}

//----------------------------------------------------------------------------------------
template<class T> T XMLAttribute::ExtractHexValue()
{
	value.SetStreamPos(0);
	Stream::ViewText bufferView(value);
	T data;
	bufferView >> Stream::Hex() >> data;
	return data;
}

//----------------------------------------------------------------------------------------
template<class T> void XMLAttribute::ExtractValue(T& target)
{
	value.SetStreamPos(0);
	Stream::ViewText bufferView(value);
	bufferView >> target;
}

//----------------------------------------------------------------------------------------
template<class T> void XMLAttribute::ExtractHexValue(T& target)
{
	value.SetStreamPos(0);
	Stream::ViewText bufferView(value);
	bufferView >> Stream::Hex() >> target;
}

//----------------------------------------------------------------------------------------
//Value write functions
//----------------------------------------------------------------------------------------
template<class T> void XMLAttribute::SetValue(const T& adata)
{
	value.SetStreamPos(0);
	value.Resize(0);
	Stream::ViewText bufferView(value);
	bufferView << adata;
}

//----------------------------------------------------------------------------------------
template<class T> void XMLAttribute::InsertValue(const T& adata)
{
	value.SetStreamPos(0);
	Stream::ViewText bufferView(value);
	bufferView << adata;
}

//----------------------------------------------------------------------------------------
template<class T> void XMLAttribute::InsertHexValue(const T& adata, unsigned int length)
{
	value.SetStreamPos(0);
	Stream::ViewText bufferView(value);
	bufferView << Stream::Hex(length) << adata;
}
