//----------------------------------------------------------------------------------------
//Name functions
//----------------------------------------------------------------------------------------
std::wstring IHeirarchicalStorageNode::GetName() const
{
	std::wstring result;
	GetNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
void IHeirarchicalStorageNode::SetName(const std::wstring& aname)
{
	SetNameInternal(InteropSupport::STLObjectSource<std::wstring>(aname));
}

//----------------------------------------------------------------------------------------
//Child functions
//----------------------------------------------------------------------------------------
IHeirarchicalStorageNode& IHeirarchicalStorageNode::CreateChild(const std::wstring& aname)
{
	return CreateChildInternal(InteropSupport::STLObjectSource<std::wstring>(aname));
}

//----------------------------------------------------------------------------------------
template<class T> IHeirarchicalStorageNode& IHeirarchicalStorageNode::CreateChild(const std::wstring& aname, const T& adata)
{
	IHeirarchicalStorageNode& child = CreateChildInternal(InteropSupport::STLObjectSource<std::wstring>(aname));
	child.InsertData(adata);
	return child;
}

//----------------------------------------------------------------------------------------
template<class T> IHeirarchicalStorageNode& IHeirarchicalStorageNode::CreateChildHex(const std::wstring& aname, const T& adata, unsigned int length)
{
	IHeirarchicalStorageNode& child = CreateChildInternal(InteropSupport::STLObjectSource<std::wstring>(aname));
	child.InsertHexData(adata, length);
	return child;
}

//----------------------------------------------------------------------------------------
template<class T> IHeirarchicalStorageNode& IHeirarchicalStorageNode::CreateChildBinary(const std::wstring& aname, const T& adata, const std::wstring& bufferName, bool ainlineBinaryData)
{
	IHeirarchicalStorageNode& child = CreateChildInternal(InteropSupport::STLObjectSource<std::wstring>(aname));
	child.InsertBinaryData(adata, bufferName, ainlineBinaryData);
	return child;
}

//----------------------------------------------------------------------------------------
template<class T> IHeirarchicalStorageNode& IHeirarchicalStorageNode::CreateChildBinary(const std::wstring& aname, const T* buffer, unsigned int entries, const std::wstring& bufferName, bool ainlineBinaryData)
{
	IHeirarchicalStorageNode& child = CreateChildInternal(InteropSupport::STLObjectSource<std::wstring>(aname));
	child.InsertBinaryData(buffer, entries, bufferName, ainlineBinaryData);
	return child;
}

//----------------------------------------------------------------------------------------
std::list<IHeirarchicalStorageNode*> IHeirarchicalStorageNode::GetChildList()
{
	std::list<IHeirarchicalStorageNode*> result;
	GetChildListInternal(InteropSupport::STLObjectTarget<std::list<IHeirarchicalStorageNode*>>(result));
	return result;
}

//----------------------------------------------------------------------------------------
//Attribute functions
//----------------------------------------------------------------------------------------
bool IHeirarchicalStorageNode::IsAttributePresent(const std::wstring& name) const
{
	return IsAttributePresentInternal(InteropSupport::STLObjectSource<std::wstring>(name));
}

//----------------------------------------------------------------------------------------
IHeirarchicalStorageAttribute* IHeirarchicalStorageNode::GetAttribute(const std::wstring& name) const
{
	return GetAttributeInternal(InteropSupport::STLObjectSource<std::wstring>(name));
}

//----------------------------------------------------------------------------------------
IHeirarchicalStorageAttribute& IHeirarchicalStorageNode::CreateAttribute(const std::wstring& name)
{
	return CreateAttributeInternal(InteropSupport::STLObjectSource<std::wstring>(name));
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
std::list<IHeirarchicalStorageAttribute*> IHeirarchicalStorageNode::GetAttributeList()
{
	std::list<IHeirarchicalStorageAttribute*> result;
	GetAttributeListInternal(InteropSupport::STLObjectTarget<std::list<IHeirarchicalStorageAttribute*>>(result));
	return result;
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
	std::wstring result;
	GetBinaryDataBufferNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
void IHeirarchicalStorageNode::SetBinaryDataBufferName(const std::wstring& aname)
{
	return SetBinaryDataBufferNameInternal(InteropSupport::STLObjectSource<std::wstring>(aname));
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
template<> IHeirarchicalStorageNode& IHeirarchicalStorageNode::ExtractBinaryData(std::vector<bool>& target)
{
	Stream::IStream& dataStream = GetInternalStream();
	ResetInternalStreamPosition();
	Stream::IStream::SizeType readCount = (dataStream.Size() / (Stream::IStream::SizeType)sizeof(bool));
	Stream::ViewBinary bufferView(dataStream);
	for(unsigned int i = 0; (i < readCount) && (i < target.size()); ++i)
	{
		bool value;
		bufferView >> value;
		target[i] = value;
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
	SetBinaryDataBufferNameInternal(InteropSupport::STLObjectSource<std::wstring>(bufferName));
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
	SetBinaryDataBufferNameInternal(InteropSupport::STLObjectSource<std::wstring>(bufferName));
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
	SetBinaryDataBufferNameInternal(InteropSupport::STLObjectSource<std::wstring>(bufferName));
	Stream::IStream& dataStream = GetInternalStream();
	dataStream.WriteData(adata);
	return *this;
}

//----------------------------------------------------------------------------------------
template<class T> IHeirarchicalStorageNode& IHeirarchicalStorageNode::InsertBinaryData(const T* buffer, size_t entries, const std::wstring& bufferName, bool ainlineBinaryData)
{
	SetBinaryDataPresent(true);
	SetInlineBinaryDataEnabled(ainlineBinaryData);
	SetBinaryDataBufferNameInternal(InteropSupport::STLObjectSource<std::wstring>(bufferName));
	Stream::IStream& dataStream = GetInternalStream();
	Stream::ViewBinary bufferView(dataStream);
	for(size_t i = 0; i < entries; ++i)
	{
		bufferView << *(buffer + i);
	}
	return *this;
}
