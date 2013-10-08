//----------------------------------------------------------------------------------------
//Error handling functions
//----------------------------------------------------------------------------------------
std::wstring HeirarchicalStorageTree::GetErrorString() const
{
	return errorString;
}

//----------------------------------------------------------------------------------------
//Node access functions
//----------------------------------------------------------------------------------------
std::list<IHeirarchicalStorageNode*> HeirarchicalStorageTree::GetBinaryDataNodeList()
{
	std::list<IHeirarchicalStorageNode*> binaryEntityList;
	root->AddBinaryDataEntitiesToList(binaryEntityList);
	return binaryEntityList;
}
