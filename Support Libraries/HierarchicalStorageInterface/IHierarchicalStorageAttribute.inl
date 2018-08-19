//----------------------------------------------------------------------------------------
//Value read functions
//----------------------------------------------------------------------------------------
std::wstring IHierarchicalStorageAttribute::GetValue() const
{
	ResetInternalStreamPosition();
	Stream::IStream& buffer = GetInternalStream();
	Stream::ViewText bufferView(buffer);
	std::wstring data;
	bufferView >> data;
	return data;
}

//----------------------------------------------------------------------------------------
template<class T> T IHierarchicalStorageAttribute::ExtractValue()
{
	ResetInternalStreamPosition();
	Stream::IStream& buffer = GetInternalStream();
	Stream::ViewText bufferView(buffer);
	T data;
	bufferView >> data;
	return data;
}

//----------------------------------------------------------------------------------------
template<class T> T IHierarchicalStorageAttribute::ExtractHexValue()
{
	ResetInternalStreamPosition();
	Stream::IStream& buffer = GetInternalStream();
	Stream::ViewText bufferView(buffer);
	T data;
	bufferView >> Stream::Hex() >> data;
	return data;
}

//----------------------------------------------------------------------------------------
template<class T> void IHierarchicalStorageAttribute::ExtractValue(T& target)
{
	ResetInternalStreamPosition();
	Stream::IStream& buffer = GetInternalStream();
	Stream::ViewText bufferView(buffer);
	bufferView >> target;
}

//----------------------------------------------------------------------------------------
template<class T> void IHierarchicalStorageAttribute::ExtractHexValue(T& target)
{
	ResetInternalStreamPosition();
	Stream::IStream& buffer = GetInternalStream();
	Stream::ViewText bufferView(buffer);
	bufferView >> Stream::Hex() >> target;
}

//----------------------------------------------------------------------------------------
//Value write functions
//----------------------------------------------------------------------------------------
template<class T> void IHierarchicalStorageAttribute::SetValue(const T& data)
{
	ResetInternalStreamPosition();
	EmptyInternalStream();
	Stream::IStream& buffer = GetInternalStream();
	Stream::ViewText bufferView(buffer);
	bufferView << data;
}

//----------------------------------------------------------------------------------------
template<class T> void IHierarchicalStorageAttribute::InsertValue(const T& data)
{
	Stream::IStream& buffer = GetInternalStream();
	Stream::ViewText bufferView(buffer);
	bufferView << data;
}

//----------------------------------------------------------------------------------------
template<class T> void IHierarchicalStorageAttribute::InsertHexValue(const T& data, unsigned int length)
{
	Stream::IStream& buffer = GetInternalStream();
	Stream::ViewText bufferView(buffer);
	bufferView << Stream::Hex(length) << data;
}
