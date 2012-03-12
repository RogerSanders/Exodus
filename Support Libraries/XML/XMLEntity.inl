//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
XMLEntity::XMLEntity()
:parent(0), binaryDataPresent(false), inlineBinaryData(false), dataStream(Stream::IStream::TEXTENCODING_UTF16, 0)
{}

//----------------------------------------------------------------------------------------
XMLEntity::XMLEntity(const std::wstring& aname)
:parent(0), name(aname), binaryDataPresent(false), inlineBinaryData(false), dataStream(Stream::IStream::TEXTENCODING_UTF16, 0)
{}

//----------------------------------------------------------------------------------------
template<class T> XMLEntity::XMLEntity(const std::wstring& aname, const T& adata)
:parent(0), name(aname), binaryDataPresent(false), inlineBinaryData(false), dataStream(Stream::IStream::TEXTENCODING_UTF16, 0)
{
	InsertData(adata);
}

//----------------------------------------------------------------------------------------
template<class T> XMLEntity::XMLEntity(const std::wstring& aname, const T& adata, unsigned int length)
:parent(0), name(aname), binaryDataPresent(false), inlineBinaryData(false), dataStream(Stream::IStream::TEXTENCODING_UTF16, 0)
{
	InsertHexData(adata, length);
}

//----------------------------------------------------------------------------------------
//Name functions
//----------------------------------------------------------------------------------------
std::wstring XMLEntity::GetName() const
{
	return name;
}

//----------------------------------------------------------------------------------------
void XMLEntity::SetName(const std::wstring& aname)
{
	name = aname;
}

//----------------------------------------------------------------------------------------
//Parent and child functions
//----------------------------------------------------------------------------------------
XMLEntity* XMLEntity::GetParent() const
{
	return parent;
}

//----------------------------------------------------------------------------------------
void XMLEntity::SetParent(XMLEntity* aparent)
{
	parent = aparent;
}

//----------------------------------------------------------------------------------------
void XMLEntity::AddChild(const XMLEntity& child)
{
	children.push_back(child);
	children.back().SetParent(this);
}

//----------------------------------------------------------------------------------------
//Attribute functions
//----------------------------------------------------------------------------------------
template<class T> XMLEntity& XMLEntity::CreateAttribute(const std::wstring& name, const T& value)
{
	AddAttribute(XMLAttribute(name, value));
	return *this;
}

//----------------------------------------------------------------------------------------
template<class T> XMLEntity& XMLEntity::CreateAttributeHex(const std::wstring& name, const T& value, unsigned int length)
{
	AddAttribute(XMLAttribute(name, value, length));
	return *this;
}

//----------------------------------------------------------------------------------------
template<class T> bool XMLEntity::ExtractAttribute(const std::wstring& name, T& target)
{
	XMLAttribute* attribute = GetAttribute(name);
	if(attribute != 0)
	{
		attribute->ExtractValue(target);
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------------------
template<class T> bool XMLEntity::ExtractAttributeHex(const std::wstring& name, T& target)
{
	XMLAttribute* attribute = GetAttribute(name);
	if(attribute != 0)
	{
		attribute->ExtractHexValue(target);
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------------------
//Data read functions
//----------------------------------------------------------------------------------------
std::wstring XMLEntity::GetData() const
{
	Stream::ViewText bufferView(dataStream);
	std::wstring data;
	dataStream.SetStreamPos(0);
	bufferView >> data;
	dataStream.SetStreamPos(0);
	return data;
}

//----------------------------------------------------------------------------------------
template<class T> T XMLEntity::ExtractData()
{
	Stream::ViewText bufferView(dataStream);
	T data;
	dataStream.SetStreamPos(0);
	bufferView >> data;
	dataStream.SetStreamPos(0);
	return data;
}

//----------------------------------------------------------------------------------------
template<class T> T XMLEntity::ExtractHexData()
{
	Stream::ViewText bufferView(dataStream);
	T data;
	dataStream.SetStreamPos(0);
	bufferView >> Stream::Hex() >> data;
	dataStream.SetStreamPos(0);
	return data;
}

//----------------------------------------------------------------------------------------
template<class T> void XMLEntity::ExtractData(T& target)
{
	Stream::ViewText bufferView(dataStream);
	dataStream.SetStreamPos(0);
	bufferView >> target;
	dataStream.SetStreamPos(0);
}

//----------------------------------------------------------------------------------------
template<class T> void XMLEntity::ExtractHexData(T& target)
{
	Stream::ViewText bufferView(dataStream);
	dataStream.SetStreamPos(0);
	bufferView >> Stream::Hex() >> target;
	dataStream.SetStreamPos(0);
}

//----------------------------------------------------------------------------------------
//Data write functions
//----------------------------------------------------------------------------------------
template<class T> void XMLEntity::SetData(const T& adata)
{
	binaryDataPresent = false;
	Stream::ViewText bufferView(dataStream);
	bufferView << adata;
}

//----------------------------------------------------------------------------------------
template<class T> void XMLEntity::InsertData(const T& adata)
{
	binaryDataPresent = false;
	Stream::ViewText bufferView(dataStream);
	bufferView << adata;
}

//----------------------------------------------------------------------------------------
template<class T> void XMLEntity::InsertHexData(const T& adata, unsigned int length)
{
	binaryDataPresent = false;
	Stream::ViewText bufferView(dataStream);
	bufferView << Stream::Hex(length) << adata;
}

//----------------------------------------------------------------------------------------
//Binary data functions
//----------------------------------------------------------------------------------------
inline std::wstring XMLEntity::GetBinaryDataBufferName() const
{
	return binaryDataName;
}

//----------------------------------------------------------------------------------------
Stream::Buffer& XMLEntity::GetBinaryDataBufferStream()
{
	dataStream.SetStreamPos(0);
	return dataStream;
}

//----------------------------------------------------------------------------------------
bool XMLEntity::GetInlineBinaryDataEnabled() const
{
	return inlineBinaryData;
}

//----------------------------------------------------------------------------------------
void XMLEntity::SetInlineBinaryDataEnabled(bool state)
{
	inlineBinaryData = state;
}

//----------------------------------------------------------------------------------------
//Binary data read functions
//----------------------------------------------------------------------------------------
template<class T> T XMLEntity::ExtractBinaryData()
{
	Stream::ViewBinary bufferView(dataStream);
	T data;
	dataStream.SetStreamPos(0);
	bufferView >> data;
	dataStream.SetStreamPos(0);
	return data;
}

//----------------------------------------------------------------------------------------
template<class T> void XMLEntity::ExtractBinaryData(T& target)
{
	Stream::ViewBinary bufferView(dataStream);
	dataStream.SetStreamPos(0);
	bufferView >> target;
	dataStream.SetStreamPos(0);
}

//----------------------------------------------------------------------------------------
template<class T> void XMLEntity::ExtractBinaryData(std::vector<T>& target)
{
	dataStream.SetStreamPos(0);
	dataStream.ReadData(target, dataStream.Size());
}

//----------------------------------------------------------------------------------------
//Binary data write functions
//----------------------------------------------------------------------------------------
template<class T> void XMLEntity::InsertBinaryData(const T& adata, const std::wstring& bufferName, bool ainlineBinaryData)
{
	binaryDataPresent = true;
	inlineBinaryData = ainlineBinaryData;
	binaryDataName = bufferName;
	Stream::ViewBinary bufferView(dataStream);
	bufferView << adata;
}

//----------------------------------------------------------------------------------------
template<class T> void XMLEntity::InsertBinaryData(const std::vector<T>& adata, const std::wstring& bufferName, bool ainlineBinaryData)
{
	binaryDataPresent = true;
	inlineBinaryData = ainlineBinaryData;
	binaryDataName = bufferName;
	Stream::ViewBinary bufferView(dataStream);
	for(unsigned int i = 0; i < adata.size(); ++i)
	{
		bufferView << adata[i];
	}
}

//----------------------------------------------------------------------------------------
template<> void XMLEntity::InsertBinaryData(const std::vector<unsigned char>& adata, const std::wstring& bufferName, bool ainlineBinaryData)
{
	binaryDataPresent = true;
	inlineBinaryData = ainlineBinaryData;
	binaryDataName = bufferName;
	dataStream.WriteData(adata);
}

//----------------------------------------------------------------------------------------
template<class T> void XMLEntity::InsertBinaryData(T* buffer, unsigned int entries, const std::wstring& bufferName, bool ainlineBinaryData)
{
	binaryDataPresent = true;
	inlineBinaryData = ainlineBinaryData;
	binaryDataName = bufferName;
	Stream::ViewBinary bufferView(dataStream);
	for(unsigned int i = 0; i < entries; ++i)
	{
		bufferView << *(buffer + i);
	}
}
