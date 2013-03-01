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
	return GetNameInternal();
}
