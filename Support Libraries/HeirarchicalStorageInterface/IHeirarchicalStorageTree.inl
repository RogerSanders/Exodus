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
	std::wstring result;
	GetErrorStringInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
//Node access functions
//----------------------------------------------------------------------------------------
std::list<IHeirarchicalStorageNode*> IHeirarchicalStorageTree::GetBinaryDataNodeList()
{
	std::list<IHeirarchicalStorageNode*> result;
	GetBinaryDataNodeListInternal(InteropSupport::STLObjectTarget<std::list<IHeirarchicalStorageNode*>>(result));
	return result;
}
