//----------------------------------------------------------------------------------------
//Name functions
//----------------------------------------------------------------------------------------
std::wstring IHeirarchicalStorageAttribute::GetName() const
{
	std::wstring result;
	GetNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
void IHeirarchicalStorageAttribute::SetName(const std::wstring& aname)
{
	SetNameInternal(InteropSupport::STLObjectSource<std::wstring>(aname));
}

//----------------------------------------------------------------------------------------
//Value read functions
//----------------------------------------------------------------------------------------
std::wstring IHeirarchicalStorageAttribute::GetValue() const
{
	ResetInternalStreamPosition();
	Stream::IStream& buffer = GetInternalStream();
	Stream::ViewText bufferView(buffer);
	std::wstring data;
	bufferView >> data;
	return data;
}

//----------------------------------------------------------------------------------------
template<class T> T IHeirarchicalStorageAttribute::ExtractValue()
{
	ResetInternalStreamPosition();
	Stream::IStream& buffer = GetInternalStream();
	Stream::ViewText bufferView(buffer);
	T data;
	bufferView >> data;
	return data;
}

//----------------------------------------------------------------------------------------
template<class T> T IHeirarchicalStorageAttribute::ExtractHexValue()
{
	ResetInternalStreamPosition();
	Stream::IStream& buffer = GetInternalStream();
	Stream::ViewText bufferView(buffer);
	T data;
	bufferView >> Stream::Hex() >> data;
	return data;
}

//----------------------------------------------------------------------------------------
template<class T> void IHeirarchicalStorageAttribute::ExtractValue(T& target)
{
	ResetInternalStreamPosition();
	Stream::IStream& buffer = GetInternalStream();
	Stream::ViewText bufferView(buffer);
	bufferView >> target;
}

//----------------------------------------------------------------------------------------
template<class T> void IHeirarchicalStorageAttribute::ExtractHexValue(T& target)
{
	ResetInternalStreamPosition();
	Stream::IStream& buffer = GetInternalStream();
	Stream::ViewText bufferView(buffer);
	bufferView >> Stream::Hex() >> target;
}

//----------------------------------------------------------------------------------------
//Value write functions
//----------------------------------------------------------------------------------------
template<class T> void IHeirarchicalStorageAttribute::SetValue(const T& adata)
{
	ResetInternalStreamPosition();
	EmptyInternalStream();
	Stream::IStream& buffer = GetInternalStream();
	Stream::ViewText bufferView(buffer);
	bufferView << adata;
}

//----------------------------------------------------------------------------------------
template<class T> void IHeirarchicalStorageAttribute::InsertValue(const T& adata)
{
	Stream::IStream& buffer = GetInternalStream();
	Stream::ViewText bufferView(buffer);
	bufferView << adata;
}

//----------------------------------------------------------------------------------------
template<class T> void IHeirarchicalStorageAttribute::InsertHexValue(const T& adata, unsigned int length)
{
	Stream::IStream& buffer = GetInternalStream();
	Stream::ViewText bufferView(buffer);
	bufferView << Stream::Hex(length) << adata;
}
