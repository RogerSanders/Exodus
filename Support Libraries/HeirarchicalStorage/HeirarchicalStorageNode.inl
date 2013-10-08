//----------------------------------------------------------------------------------------
//Name functions
//----------------------------------------------------------------------------------------
std::wstring HeirarchicalStorageNode::GetName() const
{
	return name;
}

//----------------------------------------------------------------------------------------
void HeirarchicalStorageNode::SetName(const std::wstring& aname)
{
	name = aname;
}

//----------------------------------------------------------------------------------------
//Binary data functions
//----------------------------------------------------------------------------------------
std::wstring HeirarchicalStorageNode::GetBinaryDataBufferName() const
{
	return binaryDataName.c_str();
}

//----------------------------------------------------------------------------------------
void HeirarchicalStorageNode::SetBinaryDataBufferName(const std::wstring& aname)
{
	binaryDataName = aname;
}
