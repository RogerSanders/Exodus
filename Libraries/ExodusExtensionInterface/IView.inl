//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum IView::InitialDockPos
{
	INITIALDOCKPOS_LEFT,
	INITIALDOCKPOS_RIGHT,
	INITIALDOCKPOS_TOP,
	INITIALDOCKPOS_BOTTOM,
	INITIALDOCKPOS_CENTER
};

//----------------------------------------------------------------------------------------
enum IView::InitialDialogPos
{
	INITIALDIALOGPOS_DEFAULT,
	INITIALDIALOGPOS_CENTER
};

//----------------------------------------------------------------------------------------
enum IView::ViewType
{
	VIEWTYPE_DIALOG,
	VIEWTYPE_DOCKABLE,
	VIEWTYPE_DOCUMENT
};

//----------------------------------------------------------------------------------------
enum IView::DialogMode
{
	DIALOGMODE_MODAL,
	DIALOGMODE_MODELESS
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
