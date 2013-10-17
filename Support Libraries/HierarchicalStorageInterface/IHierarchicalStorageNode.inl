//----------------------------------------------------------------------------------------
//Name functions
//----------------------------------------------------------------------------------------
std::wstring IHierarchicalStorageNode::GetName() const
{
	std::wstring result;
	GetNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
void IHierarchicalStorageNode::SetName(const std::wstring& aname)
{
	SetNameInternal(InteropSupport::STLObjectSource<std::wstring>(aname));
}

//----------------------------------------------------------------------------------------
//Child functions
//----------------------------------------------------------------------------------------
IHierarchicalStorageNode& IHierarchicalStorageNode::CreateChild(const std::wstring& aname)
{
	return CreateChildInternal(InteropSupport::STLObjectSource<std::wstring>(aname));
}

//----------------------------------------------------------------------------------------
template<class T> IHierarchicalStorageNode& IHierarchicalStorageNode::CreateChild(const std::wstring& aname, const T& adata)
{
	IHierarchicalStorageNode& child = CreateChildInternal(InteropSupport::STLObjectSource<std::wstring>(aname));
	child.InsertData(adata);
	return child;
}

//----------------------------------------------------------------------------------------
template<class T> IHierarchicalStorageNode& IHierarchicalStorageNode::CreateChildHex(const std::wstring& aname, const T& adata, unsigned int length)
{
	IHierarchicalStorageNode& child = CreateChildInternal(InteropSupport::STLObjectSource<std::wstring>(aname));
	child.InsertHexData(adata, length);
	return child;
}

//----------------------------------------------------------------------------------------
template<class T> IHierarchicalStorageNode& IHierarchicalStorageNode::CreateChildBinary(const std::wstring& aname, const T& adata, const std::wstring& bufferName, bool ainlineBinaryData)
{
	IHierarchicalStorageNode& child = CreateChildInternal(InteropSupport::STLObjectSource<std::wstring>(aname));
	child.InsertBinaryData(adata, bufferName, ainlineBinaryData);
	return child;
}

//----------------------------------------------------------------------------------------
template<class T> IHierarchicalStorageNode& IHierarchicalStorageNode::CreateChildBinary(const std::wstring& aname, const T* buffer, unsigned int entries, const std::wstring& bufferName, bool ainlineBinaryData)
{
	IHierarchicalStorageNode& child = CreateChildInternal(InteropSupport::STLObjectSource<std::wstring>(aname));
	child.InsertBinaryData(buffer, entries, bufferName, ainlineBinaryData);
	return child;
}

//----------------------------------------------------------------------------------------
std::list<IHierarchicalStorageNode*> IHierarchicalStorageNode::GetChildList()
{
	std::list<IHierarchicalStorageNode*> result;
	GetChildListInternal(InteropSupport::STLObjectTarget<std::list<IHierarchicalStorageNode*>>(result));
	return result;
}

//----------------------------------------------------------------------------------------
//Attribute functions
//----------------------------------------------------------------------------------------
bool IHierarchicalStorageNode::IsAttributePresent(const std::wstring& name) const
{
	return IsAttributePresentInternal(InteropSupport::STLObjectSource<std::wstring>(name));
}

//----------------------------------------------------------------------------------------
IHierarchicalStorageAttribute* IHierarchicalStorageNode::GetAttribute(const std::wstring& name) const
{
	return GetAttributeInternal(InteropSupport::STLObjectSource<std::wstring>(name));
}

//----------------------------------------------------------------------------------------
IHierarchicalStorageAttribute& IHierarchicalStorageNode::CreateAttribute(const std::wstring& name)
{
	return CreateAttributeInternal(InteropSupport::STLObjectSource<std::wstring>(name));
}

//----------------------------------------------------------------------------------------
template<class T> IHierarchicalStorageNode& IHierarchicalStorageNode::CreateAttribute(const std::wstring& name, const T& value)
{
	IHierarchicalStorageAttribute& attribute = CreateAttribute(name);
	attribute.InsertValue(value);
	return *this;
}

//----------------------------------------------------------------------------------------
template<class T> IHierarchicalStorageNode& IHierarchicalStorageNode::CreateAttributeHex(const std::wstring& name, const T& value, unsigned int length)
{
	IHierarchicalStorageAttribute& attribute = CreateAttribute(name);
	attribute.InsertHexValue(value, length);
	return *this;
}

//----------------------------------------------------------------------------------------
template<class T> bool IHierarchicalStorageNode::ExtractAttribute(const std::wstring& name, T& target)
{
	IHierarchicalStorageAttribute* attribute = GetAttribute(name);
	if(attribute == 0)
	{
		return false;
	}
	attribute->ExtractValue(target);
	return true;
}

//----------------------------------------------------------------------------------------
template<class T> bool IHierarchicalStorageNode::ExtractAttributeHex(const std::wstring& name, T& target)
{
	IHierarchicalStorageAttribute* attribute = GetAttribute(name);
	if(attribute == 0)
	{
		return false;
	}
	attribute->ExtractHexValue(target);
	return true;
}

//----------------------------------------------------------------------------------------
std::list<IHierarchicalStorageAttribute*> IHierarchicalStorageNode::GetAttributeList()
{
	std::list<IHierarchicalStorageAttribute*> result;
	GetAttributeListInternal(InteropSupport::STLObjectTarget<std::list<IHierarchicalStorageAttribute*>>(result));
	return result;
}

//----------------------------------------------------------------------------------------
//Data read functions
//----------------------------------------------------------------------------------------
std::wstring IHierarchicalStorageNode::GetData() const
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
template<class T> T IHierarchicalStorageNode::ExtractData()
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
template<class T> T IHierarchicalStorageNode::ExtractHexData()
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
template<class T> IHierarchicalStorageNode& IHierarchicalStorageNode::ExtractData(T& target)
{
	Stream::IStream& dataStream = GetInternalStream();
	ResetInternalStreamPosition();
	Stream::ViewText bufferView(dataStream);
	bufferView >> target;
	ResetInternalStreamPosition();
	return *this;
}

//----------------------------------------------------------------------------------------
template<class T> IHierarchicalStorageNode& IHierarchicalStorageNode::ExtractHexData(T& target)
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
template<class T> IHierarchicalStorageNode& IHierarchicalStorageNode::SetData(const T& adata)
{
	SetBinaryDataPresent(false);
	Stream::IStream& dataStream = GetInternalStream();
	Stream::ViewText bufferView(dataStream);
	bufferView << adata;
	return *this;
}

//----------------------------------------------------------------------------------------
template<class T> IHierarchicalStorageNode& IHierarchicalStorageNode::InsertData(const T& adata)
{
	SetBinaryDataPresent(false);
	Stream::IStream& dataStream = GetInternalStream();
	Stream::ViewText bufferView(dataStream);
	bufferView << adata;
	return *this;
}

//----------------------------------------------------------------------------------------
template<class T> IHierarchicalStorageNode& IHierarchicalStorageNode::InsertHexData(const T& adata, unsigned int length)
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
std::wstring IHierarchicalStorageNode::GetBinaryDataBufferName() const
{
	std::wstring result;
	GetBinaryDataBufferNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
void IHierarchicalStorageNode::SetBinaryDataBufferName(const std::wstring& aname)
{
	return SetBinaryDataBufferNameInternal(InteropSupport::STLObjectSource<std::wstring>(aname));
}

//----------------------------------------------------------------------------------------
//Binary data read functions
//----------------------------------------------------------------------------------------
template<class T> T IHierarchicalStorageNode::ExtractBinaryData()
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
template<class T> IHierarchicalStorageNode& IHierarchicalStorageNode::ExtractBinaryData(T& target)
{
	Stream::IStream& dataStream = GetInternalStream();
	ResetInternalStreamPosition();
	Stream::ViewBinary bufferView(dataStream);
	bufferView >> target;
	ResetInternalStreamPosition();
	return *this;
}

//----------------------------------------------------------------------------------------
//Binary data write functions
//----------------------------------------------------------------------------------------
template<class T> IHierarchicalStorageNode& IHierarchicalStorageNode::InsertBinaryData(const T& adata, const std::wstring& bufferName, bool ainlineBinaryData)
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
template<class T> IHierarchicalStorageNode& IHierarchicalStorageNode::InsertBinaryData(const T* buffer, size_t entries, const std::wstring& bufferName, bool ainlineBinaryData)
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
