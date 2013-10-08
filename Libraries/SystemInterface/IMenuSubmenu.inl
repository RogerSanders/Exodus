//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IMenuSubmenu::ThisIMenuSubmenuVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Menu name functions
//----------------------------------------------------------------------------------------
std::wstring IMenuSubmenu::GetMenuName() const
{
	std::wstring result;
	GetMenuNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
//Menu segment functions
//----------------------------------------------------------------------------------------
std::list<IMenuSegment*> IMenuSubmenu::GetMenuSegments() const
{
	std::list<IMenuSegment*> result;
	GetMenuSegmentsInternal(InteropSupport::STLObjectTarget<std::list<IMenuSegment*>>(result));
	return result;
}
