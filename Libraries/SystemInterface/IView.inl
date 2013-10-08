//----------------------------------------------------------------------------------------
//Title functions
//----------------------------------------------------------------------------------------
std::wstring IView::GetViewTitle() const
{
	std::wstring result;
	GetViewTitleInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IView::ThisIViewVersion()
{
	return 1;
}
