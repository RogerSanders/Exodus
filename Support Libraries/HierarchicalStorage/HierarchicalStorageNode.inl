//----------------------------------------------------------------------------------------
//Name functions
//----------------------------------------------------------------------------------------
std::wstring HierarchicalStorageNode::GetName() const
{
	return name;
}

//----------------------------------------------------------------------------------------
void HierarchicalStorageNode::SetName(const std::wstring& aname)
{
	name = aname;
}

//----------------------------------------------------------------------------------------
//Binary data functions
//----------------------------------------------------------------------------------------
std::wstring HierarchicalStorageNode::GetBinaryDataBufferName() const
{
	return binaryDataName.c_str();
}

//----------------------------------------------------------------------------------------
void HierarchicalStorageNode::SetBinaryDataBufferName(const std::wstring& aname)
{
	binaryDataName = aname;
}
