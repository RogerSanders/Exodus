//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IMenuSelectableOption::ThisIMenuSelectableOptionVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Menu title functions
//----------------------------------------------------------------------------------------
std::wstring IMenuSelectableOption::GetMenuTitle() const
{
	std::wstring result;
	GetMenuTitleInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}
