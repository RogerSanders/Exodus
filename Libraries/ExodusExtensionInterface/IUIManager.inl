//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum IUIManager::WindowType
{
	WINDOWTYPE_DIALOG,
	WINDOWTYPE_DOCKABLE
};

//----------------------------------------------------------------------------------------
enum IUIManager::InitialDockingLocation
{
	//##TODO##
};

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IUIManager::ThisIUIManagerVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Window management functions
//----------------------------------------------------------------------------------------
bool IUIManager::ShowWindowFirstTime(IView& view, IViewPresenter& viewPresenter, HWND windowHandle, const std::wstring& windowTitle, IHierarchicalStorageNode* windowState)
{
	return ShowWindowFirstTimeInternal(view, viewPresenter, windowHandle, InteropSupport::STLObjectSource<std::wstring>(windowTitle), windowState);
}

//----------------------------------------------------------------------------------------
//Window title functions
//----------------------------------------------------------------------------------------
void IUIManager::UpdateWindowTitle(IView& view, IViewPresenter& viewPresenter, HWND windowHandle, const std::wstring& windowTitle)
{
	UpdateWindowTitleInternal(view, viewPresenter, windowHandle, InteropSupport::STLObjectSource<std::wstring>(windowTitle));
}
