//----------------------------------------------------------------------------------------
//Hosted content methods
//----------------------------------------------------------------------------------------
void IDockingWindow::AddHostedContent(HWND contentWindow, const std::wstring& contentTitle)
{
	AddHostedContentInternal(contentWindow, InteropSupport::STLObjectSource<std::wstring>(contentTitle));
}

//----------------------------------------------------------------------------------------
std::wstring IDockingWindow::GetHostedContentTitle(unsigned int contentEntryNo) const
{
	std::wstring result;
	GetHostedContentTitleInternal(contentEntryNo, InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
//Child container methods
//----------------------------------------------------------------------------------------
std::list<IDockingWindow*> IDockingWindow::GetNestedChildDockingWindowList() const
{
	std::list<IDockingWindow*> result;
	GetNestedChildDockingWindowListInternal(InteropSupport::STLObjectTarget<std::list<IDockingWindow*>>(result));
	return result;
}
