#include "EventLogViewPresenter.h"
#include "EventLogView.h"
#include "Stream/Stream.pkg"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
EventLogViewPresenter::EventLogViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, ExodusSystemMenus& aowner, ISystemGUIInterface& amodel)
:ViewPresenterBase(aowner.GetAssemblyHandle(), aviewGroupName, aviewName, aviewID), owner(aowner), model(amodel)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring EventLogViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Event Log";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* EventLogViewPresenter::CreateView(IUIManager& uiManager)
{
	return new EventLogView(uiManager, *this, model);
}

//----------------------------------------------------------------------------------------
void EventLogViewPresenter::DeleteView(IView* aview)
{
	delete aview;
}

//----------------------------------------------------------------------------------------
//Log functions
//----------------------------------------------------------------------------------------
void EventLogViewPresenter::SaveEventLog(const std::vector<ISystemGUIInterface::SystemLogEntry>& eventLog, const std::wstring& filePath)
{
	Stream::File target;
	if(target.Open(filePath, Stream::File::OpenMode::ReadAndWrite, Stream::File::CreateMode::Create))
	{
		target.SetTextEncoding(Stream::IStream::TextEncoding::UTF8);
		target.InsertByteOrderMark();
		Stream::ViewText targetView(target);
		for(unsigned int i = 0; i < (unsigned int)eventLog.size(); ++i)
		{
			const ISystemGUIInterface::SystemLogEntry& logEntry = eventLog[i];
			targetView << logEntry.eventTimeString << L'\t' << logEntry.eventLevelString << L'\t' << logEntry.source << L'\t' << logEntry.text << L'\n';
		}
	}
}
