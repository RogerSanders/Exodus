//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IMenuSelectableOption::ThisIMenuSelectableOptionVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Menu handler functions
//----------------------------------------------------------------------------------------
std::wstring IMenuSelectableOption::GetName() const
{
	std::wstring result;
	GetNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}
