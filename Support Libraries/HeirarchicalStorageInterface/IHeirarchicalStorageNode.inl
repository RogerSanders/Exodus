//----------------------------------------------------------------------------------------
//Name functions
//----------------------------------------------------------------------------------------
std::wstring IHeirarchicalStorageNode::GetName() const
{
	return GetNameInternal();
}

//----------------------------------------------------------------------------------------
void IHeirarchicalStorageNode::SetName(const std::wstring& aname)
{
	SetNameInternal(aname.c_str());
}

//----------------------------------------------------------------------------------------
//Child functions
//----------------------------------------------------------------------------------------
IHeirarchicalStorageNode& IHeirarchicalStorageNode::CreateChild(const std::wstring& aname)
{
	return CreateChild(aname.c_str());
}

//----------------------------------------------------------------------------------------
template<class T> IHeirarchicalStorageNode& IHeirarchicalStorageNode::CreateChild(const std::wstring& aname, const T& adata)
{
	IHeirarchicalStorageNode& child = CreateChild(aname.c_str());
	child.InsertData(adata);
	return child;
}

//----------------------------------------------------------------------------------------
template<class T> IHeirarchicalStorageNode& IHeirarchicalStorageNode::CreateChildHex(const std::wstring& aname, const T& adata, unsigned int length)
{
	IHeirarchicalStorageNode& child = CreateChild(aname.c_str());
	child.InsertHexData(adata, length);
	return child;
}

//----------------------------------------------------------------------------------------
std::list<IHeirarchicalStorageNode*> IHeirarchicalStorageNode::GetChildList()
{
	std::list<IHeirarchicalStorageNode*> list;
	unsigned int childCount;
	IHeirarchicalStorageNode** childBuffer = GetChildList(childCount);
	for(unsigned int i = 0; i < childCount; ++i)
	{
		list.push_back(*(childBuffer + i));
	}
	return list;
}

//----------------------------------------------------------------------------------------
//Attribute functions
//----------------------------------------------------------------------------------------
std::list<IHeirarchicalStorageAttribute*> IHeirarchicalStorageNode::GetAttributeList()
{
	std::list<IHeirarchicalStorageAttribute*> list;
	unsigned int attributeCount;
	IHeirarchicalStorageAttribute** attributeBuffer = GetAttributeList(attributeCount);
	for(unsigned int i = 0; i < attributeCount; ++i)
	{
		list.push_back(*(attributeBuffer + i));
	}
	return list;
}

//----------------------------------------------------------------------------------------
bool IHeirarchicalStorageNode::IsAttributePresent(const std::wstring& name) const
{
	return IsAttributePresent(name.c_str());
}

//----------------------------------------------------------------------------------------
IHeirarchicalStorageAttribute* IHeirarchicalStorageNode::GetAttribute(const std::wstring& name) const
{
	return GetAttribute(name.c_str());
}

//----------------------------------------------------------------------------------------
IHeirarchicalStorageAttribute& IHeirarchicalStorageNode::CreateAttribute(const std::wstring& name)
{
	return CreateAttribute(name.c_str());
}

//----------------------------------------------------------------------------------------
template<class T> IHeirarchicalStorageNode& IHeirarchicalStorageNode::CreateAttribute(const std::wstring& name, const T& value)
{
	IHeirarchicalStorageAttribute& attribute = CreateAttribute(name);
	attribute.InsertValue(value);
	return *this;
}

//----------------------------------------------------------------------------------------
template<class T> IHeirarchicalStorageNode& IHeirarchicalStorageNode::CreateAttributeHex(const std::wstring& name, const T& value, unsigned int length)
{
	IHeirarchicalStorageAttribute& attribute = CreateAttribute(name);
	attribute.InsertHexValue(value, length);
	return *this;
}

//----------------------------------------------------------------------------------------
template<class T> bool IHeirarchicalStorageNode::ExtractAttribute(const std::wstring& name, T& target)
{
	IHeirarchicalStorageAttribute* attribute = GetAttribute(name);
	if(attribute == 0)
	{
		return false;
	}
	attribute->ExtractValue(target);
	return true;
}

//----------------------------------------------------------------------------------------
template<class T> bool IHeirarchicalStorageNode::ExtractAttributeHex(const std::wstring& name, T& target)
{
	IHeirarchicalStorageAttribute* attribute = GetAttribute(name);
	if(attribute == 0)
	{
		return false;
	}
	attribute->ExtractHexValue(target);
	return true;
}

//----------------------------------------------------------------------------------------
//Data read functions
//----------------------------------------------------------------------------------------
std::wstring IHeirarchicalStorageNode::GetData() const
{
	Stream::IStream& dataStream = GetInternalStream();
	ResetInternalStreamPosition();
	std::wstring data;
	Stream::ViewText bufferView(dataStream);
	bufferView.ReadTextString(data, false);
	ResetInternalStreamPosition();
	return data;
}

//----------------------------------------------------------------------------------------
template<class T> T IHeirarchicalStorageNode::ExtractData()
{
	Stream::IStream& dataStream = GetInternalStream();
	ResetInternalStreamPosition();
	T data;
	Stream::ViewText bufferView(dataStream);
	bufferView >> data;
	ResetInternalStreamPosition();
	return data;
}

//----------------------------------------------------------------------------------------
template<class T> T IHeirarchicalStorageNode::ExtractHexData()
{
	Stream::IStream& dataStream = GetInternalStream();
	ResetInternalStreamPosition();
	T data;
	Stream::ViewText bufferView(dataStream);
	bufferView >> Stream::Hex() >> data;
	ResetInternalStreamPosition();
	return data;
}

//----------------------------------------------------------------------------------------
template<class T> IHeirarchicalStorageNode& IHeirarchicalStorageNode::ExtractData(T& target)
{
	Stream::IStream& dataStream = GetInternalStream();
	ResetInternalStreamPosition();
	Stream::ViewText bufferView(dataStream);
	bufferView >> target;
	ResetInternalStreamPosition();
	return *this;
}

//----------------------------------------------------------------------------------------
template<class T> IHeirarchicalStorageNode& IHeirarchicalStorageNode::ExtractHexData(T& target)
{
	Stream::IStream& dataStream = GetInternalStream();
	ResetInternalStreamPosition();
	Stream::ViewText bufferView(dataStream);
	bufferView >> Stream::Hex() >> target;
	ResetInternalStreamPosition();
	return *this;
}

//----------------------------------------------------------------------------------------
//Data write functions
//----------------------------------------------------------------------------------------
template<class T> IHeirarchicalStorageNode& IHeirarchicalStorageNode::SetData(const T& adata)
{
	SetBinaryDataPresent(false);
	Stream::IStream& dataStream = GetInternalStream();
	Stream::ViewText bufferView(dataStream);
	bufferView << adata;
	return *this;
}

//----------------------------------------------------------------------------------------
template<class T> IHeirarchicalStorageNode& IHeirarchicalStorageNode::InsertData(const T& adata)
{
	SetBinaryDataPresent(false);
	Stream::IStream& dataStream = GetInternalStream();
	Stream::ViewText bufferView(dataStream);
	bufferView << adata;
	return *this;
}

//----------------------------------------------------------------------------------------
template<class T> IHeirarchicalStorageNode& IHeirarchicalStorageNode::InsertHexData(const T& adata, unsigned int length)
{
	SetBinaryDataPresent(false);
	Stream::IStream& dataStream = GetInternalStream();
	Stream::ViewText bufferView(dataStream);
	bufferView << Stream::Hex(length) << adata;
	return *this;
}

//----------------------------------------------------------------------------------------
//Binary data functions
//----------------------------------------------------------------------------------------
std::wstring IHeirarchicalStorageNode::GetBinaryDataBufferName() const
{
	return GetBinaryDataBufferNameInternal();
}

//----------------------------------------------------------------------------------------
void IHeirarchicalStorageNode::SetBinaryDataBufferName(const std::wstring& aname)
{
	return SetBinaryDataBufferName(aname.c_str());
}

//----------------------------------------------------------------------------------------
//Binary data read functions
//----------------------------------------------------------------------------------------
template<class T> T IHeirarchicalStorageNode::ExtractBinaryData()
{
	Stream::IStream& dataStream = GetInternalStream();
	ResetInternalStreamPosition();
	T data;
	Stream::ViewBinary bufferView(dataStream);
	bufferView >> data;
	ResetInternalStreamPosition();
	return data;
}

//----------------------------------------------------------------------------------------
template<class T> IHeirarchicalStorageNode& IHeirarchicalStorageNode::ExtractBinaryData(T& target)
{
	Stream::IStream& dataStream = GetInternalStream();
	ResetInternalStreamPosition();
	Stream::ViewBinary bufferView(dataStream);
	bufferView >> target;
	ResetInternalStreamPosition();
	return *this;
}

//----------------------------------------------------------------------------------------
template<class T> IHeirarchicalStorageNode& IHeirarchicalStorageNode::ExtractBinaryData(std::vector<T>& target)
{
	Stream::IStream& dataStream = GetInternalStream();
	ResetInternalStreamPosition();
	Stream::IStream::SizeType readCount = (dataStream.Size() / (Stream::IStream::SizeType)sizeof(T));
	Stream::ViewBinary bufferView(dataStream);
	for(unsigned int i = 0; (i < readCount) && (i < target.size()); ++i)
	{
		bufferView >> target[i];
	}
	ResetInternalStreamPosition();
	return *this;
}

//----------------------------------------------------------------------------------------
template<> IHeirarchicalStorageNode& IHeirarchicalStorageNode::ExtractBinaryData(std::vector<unsigned char>& target)
{
	Stream::IStream& dataStream = GetInternalStream();
	ResetInternalStreamPosition();
	Stream::IStream::SizeType readCount = (dataStream.Size() / (Stream::IStream::SizeType)sizeof(unsigned char));
	dataStream.ReadData(target, readCount);
	ResetInternalStreamPosition();
	return *this;
}

//----------------------------------------------------------------------------------------
//Binary data write functions
//----------------------------------------------------------------------------------------
template<class T> IHeirarchicalStorageNode& IHeirarchicalStorageNode::InsertBinaryData(const T& adata, const std::wstring& bufferName, bool ainlineBinaryData)
{
	SetBinaryDataPresent(true);
	SetInlineBinaryDataEnabled(ainlineBinaryData);
	SetBinaryDataBufferName(bufferName.c_str());
	Stream::IStream& dataStream = GetInternalStream();
	Stream::ViewBinary bufferView(dataStream);
	bufferView << adata;
	return *this;
}

//----------------------------------------------------------------------------------------
template<class T> IHeirarchicalStorageNode& IHeirarchicalStorageNode::InsertBinaryData(const std::vector<T>& adata, const std::wstring& bufferName, bool ainlineBinaryData)
{
	SetBinaryDataPresent(true);
	SetInlineBinaryDataEnabled(ainlineBinaryData);
	SetBinaryDataBufferName(bufferName.c_str());
	Stream::IStream& dataStream = GetInternalStream();
	Stream::ViewBinary bufferView(dataStream);
	for(unsigned int i = 0; i < adata.size(); ++i)
	{
		bufferView << adata[i];
	}
	return *this;
}

//----------------------------------------------------------------------------------------
template<> IHeirarchicalStorageNode& IHeirarchicalStorageNode::InsertBinaryData(const std::vector<unsigned char>& adata, const std::wstring& bufferName, bool ainlineBinaryData)
{
	SetBinaryDataPresent(true);
	SetInlineBinaryDataEnabled(ainlineBinaryData);
	SetBinaryDataBufferName(bufferName.c_str());
	Stream::IStream& dataStream = GetInternalStream();
	dataStream.WriteData(adata);
	return *this;
}

//----------------------------------------------------------------------------------------
template<class T> IHeirarchicalStorageNode& IHeirarchicalStorageNode::InsertBinaryData(T* buffer, unsigned int entries, const std::wstring& bufferName, bool ainlineBinaryData)
{
	SetBinaryDataPresent(true);
	SetInlineBinaryDataEnabled(ainlineBinaryData);
	SetBinaryDataBufferName(bufferName.c_str());
	Stream::IStream& dataStream = GetInternalStream();
	Stream::ViewBinary bufferView(dataStream);
	for(unsigned int i = 0; i < entries; ++i)
	{
		bufferView << *(buffer + i);
	}
	return *this;
}
