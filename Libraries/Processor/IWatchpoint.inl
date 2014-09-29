//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum class IWatchpoint::Condition
{
	Equal = 0,
	Greater,
	Less,
	GreaterAndLess
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
