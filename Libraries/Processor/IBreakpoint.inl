//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum class IBreakpoint::Condition
{
	Equal = 0,
	Greater,
	Less,
	GreaterAndLess
};

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IBreakpoint::ThisIBreakpointVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Name functions
//----------------------------------------------------------------------------------------
std::wstring IBreakpoint::GetName() const
{
	std::wstring result;
	GetNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
void IBreakpoint::SetName(const std::wstring& aname)
{
	SetNameInternal(InteropSupport::STLObjectSource<std::wstring>(aname));
}

//----------------------------------------------------------------------------------------
std::wstring IBreakpoint::GenerateName() const
{
	std::wstring result;
	GenerateNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}
