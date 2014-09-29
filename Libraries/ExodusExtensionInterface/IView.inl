//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum class IView::DockPos
{
	Left,
	Right,
	Top,
	Bottom,
	Center
};

//----------------------------------------------------------------------------------------
enum class IView::DialogPos
{
	Default,
	Center
};

//----------------------------------------------------------------------------------------
enum class IView::ViewType
{
	Dialog,
	Dockable,
	Document
};

//----------------------------------------------------------------------------------------
enum class IView::DialogMode
{
	Modal,
	Modeless
};

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IView::ThisIViewVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//New window state
//----------------------------------------------------------------------------------------
std::wstring IView::GetViewDockingGroup() const
{
	std::wstring result;
	GetViewDockingGroupInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}
