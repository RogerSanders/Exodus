#include "LoggingViewPresenter.h"
#include "LoggingView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
LoggingViewPresenter::LoggingViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, SN76489Menus& aowner, const IDevice& amodelInstanceKey, ISN76489& amodel)
:ViewPresenterBase(aowner.GetAssemblyHandle(), aviewGroupName, aviewName, aviewID, amodelInstanceKey.GetDeviceInstanceName(), amodelInstanceKey.GetDeviceModuleID(), amodelInstanceKey.GetModuleDisplayName()), owner(aowner), modelInstanceKey(amodelInstanceKey), model(amodel)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring LoggingViewPresenter::GetUnqualifiedViewTitle()
{
	return L"WAV File Logging";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* LoggingViewPresenter::CreateView(IUIManager& uiManager)
{
	return new LoggingView(uiManager, *this, model);
}

//----------------------------------------------------------------------------------------
void LoggingViewPresenter::DeleteView(IView* aview)
{
	delete aview;
}

//----------------------------------------------------------------------------------------
//Logging functions
//----------------------------------------------------------------------------------------
bool LoggingViewPresenter::SelectLoggingPath(std::wstring& fileName) const
{
	return owner.GetGUIInterface().SelectNewFile(L"Wave file|wav", L"wav", fileName, L"", fileName);
}
