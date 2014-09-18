//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum IWatchpoint::Condition
{
	CONDITION_EQUAL = 0,
	CONDITION_GREATER,
	CONDITION_LESS,
	CONDITION_GREATER_AND_LESS
};

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IWatchpoint::ThisIWatchpointVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Name functions
//----------------------------------------------------------------------------------------
std::wstring IWatchpoint::GetName() const
{
	std::wstring result;
	GetNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
void IWatchpoint::SetName(const std::wstring& aname)
{
	SetNameInternal(InteropSupport::STLObjectSource<std::wstring>(aname));
}

//----------------------------------------------------------------------------------------
std::wstring IWatchpoint::GenerateName() const
{
	std::wstring result;
	GenerateNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}
