//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IGenericAccess::ThisIGenericAccessVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Data info functions
//----------------------------------------------------------------------------------------
std::set<unsigned int> IGenericAccess::GetGenericDataIDList() const
{
	std::set<unsigned int> result;
	GetGenericDataIDListInternal(InteropSupport::STLObjectTarget<std::set<unsigned int>>(result));
	return result;
}

//----------------------------------------------------------------------------------------
//Command info functions
//----------------------------------------------------------------------------------------
std::set<unsigned int> IGenericAccess::GetGenericCommandIDList() const
{
	std::set<unsigned int> result;
	GetGenericCommandIDListInternal(InteropSupport::STLObjectTarget<std::set<unsigned int>>(result));
	return result;
}

//----------------------------------------------------------------------------------------
//Data read/write functions
//----------------------------------------------------------------------------------------
bool IGenericAccess::ReadGenericData(unsigned int dataID, const DataContext* dataContext, std::wstring& dataValue) const
{
	return ReadGenericDataInternal(dataID, dataContext, InteropSupport::STLObjectTarget<std::wstring>(dataValue));
}

//----------------------------------------------------------------------------------------
bool IGenericAccess::WriteGenericData(unsigned int dataID, const DataContext* dataContext, const std::wstring& dataValue)
{
	return WriteGenericDataInternal(dataID, dataContext, InteropSupport::STLObjectSource<std::wstring>(dataValue));
}
