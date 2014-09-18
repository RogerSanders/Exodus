#include "LoggerDetailsViewPresenter.h"
#include "LoggerDetailsView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
LoggerDetailsViewPresenter::LoggerDetailsViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, ExodusSystemMenus& aowner, ISystemGUIInterface& amodel, const ISystemGUIInterface::SystemLogEntry& alogEntry)
:ViewPresenterBase(aowner.GetAssemblyHandle(), aviewGroupName, aviewName, aviewID), owner(aowner), model(amodel), logEntry(alogEntry)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring LoggerDetailsViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Event Log Entry";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* LoggerDetailsViewPresenter::CreateView(IUIManager& uiManager)
{
	return new LoggerDetailsView(uiManager, *this, model, logEntry);
}

//----------------------------------------------------------------------------------------
void LoggerDetailsViewPresenter::DeleteView(IView* aview)
{
	delete aview;
}

//----------------------------------------------------------------------------------------
//Log entry functions
//----------------------------------------------------------------------------------------
void LoggerDetailsViewPresenter::SetLogEntry(const ISystemGUIInterface::SystemLogEntry& alogEntry)
{
	logEntry = alogEntry;
	IView* viewBase = GetOpenView();
	if(viewBase != 0)
	{
		LoggerDetailsView* view = dynamic_cast<LoggerDetailsView*>(viewBase);
		if(view != 0)
		{
			view->SetLogEntry(logEntry);
		}
	}
}
