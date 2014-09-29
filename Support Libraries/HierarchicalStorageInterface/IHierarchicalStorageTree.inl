//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum class IHierarchicalStorageTree::StorageMode
{
	XML
};

//----------------------------------------------------------------------------------------
//Error handling functions
//----------------------------------------------------------------------------------------
std::wstring IHierarchicalStorageTree::GetErrorString() const
{
	std::wstring result;
	GetErrorStringInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
//Node access functions
//----------------------------------------------------------------------------------------
std::list<IHierarchicalStorageNode*> IHierarchicalStorageTree::GetBinaryDataNodeList()
{
	std::list<IHierarchicalStorageNode*> result;
	GetBinaryDataNodeListInternal(InteropSupport::STLObjectTarget<std::list<IHierarchicalStorageNode*>>(result));
	return result;
}
