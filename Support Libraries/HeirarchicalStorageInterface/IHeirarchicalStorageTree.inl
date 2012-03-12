//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum IHeirarchicalStorageTree::StorageMode
{
	STORAGEMODE_XML
};

//----------------------------------------------------------------------------------------
//Error handling functions
//----------------------------------------------------------------------------------------
std::wstring IHeirarchicalStorageTree::GetErrorString() const
{
	return std::wstring(GetErrorStringInternal());
}

//----------------------------------------------------------------------------------------
//Node access functions
//----------------------------------------------------------------------------------------
std::list<IHeirarchicalStorageNode*> IHeirarchicalStorageTree::GetBinaryDataNodeList()
{
	std::list<IHeirarchicalStorageNode*> list;
	CreateBinaryDataNodeList();
	unsigned int nodeCount;
	IHeirarchicalStorageNode** nodeBuffer = GetBinaryDataNodeList(nodeCount);
	for(unsigned int i = 0; i < nodeCount; ++i)
	{
		list.push_back(*(nodeBuffer + i));
	}
	DeleteBinaryDataNodeList();
	return list;
}
