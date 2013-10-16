//----------------------------------------------------------------------------------------
//Error handling functions
//----------------------------------------------------------------------------------------
std::wstring HierarchicalStorageTree::GetErrorString() const
{
	return errorString;
}

//----------------------------------------------------------------------------------------
//Node access functions
//----------------------------------------------------------------------------------------
std::list<IHierarchicalStorageNode*> HierarchicalStorageTree::GetBinaryDataNodeList()
{
	std::list<IHierarchicalStorageNode*> binaryEntityList;
	root->AddBinaryDataEntitiesToList(binaryEntityList);
	return binaryEntityList;
}
