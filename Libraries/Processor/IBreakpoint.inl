//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum IBreakpoint::Condition
{
	CONDITION_EQUAL = 0,
	CONDITION_GREATER,
	CONDITION_LESS,
	CONDITION_GREATER_AND_LESS
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
