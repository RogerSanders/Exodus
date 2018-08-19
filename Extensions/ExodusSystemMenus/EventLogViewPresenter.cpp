#include "EventLogViewPresenter.h"
#include "EventLogView.h"
#include "Stream/Stream.pkg"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
EventLogViewPresenter::EventLogViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, ExodusSystemMenus& owner, ISystemGUIInterface& model)
:ViewPresenterBase(owner.GetAssemblyHandle(), viewGroupName, viewName, viewID), _owner(owner), _model(model)
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
	return new EventLogView(uiManager, *this, _model);
}

//----------------------------------------------------------------------------------------
void EventLogViewPresenter::DeleteView(IView* view)
{
	delete view;
}

//----------------------------------------------------------------------------------------
//Log functions
//----------------------------------------------------------------------------------------
void EventLogViewPresenter::SaveEventLog(const std::vector<ISystemGUIInterface::SystemLogEntry>& eventLog, const std::wstring& filePath)
{
	Stream::File target;
	if (target.Open(filePath, Stream::File::OpenMode::ReadAndWrite, Stream::File::CreateMode::Create))
	{
		target.SetTextEncoding(Stream::IStream::TextEncoding::UTF8);
		target.InsertByteOrderMark();
		Stream::ViewText targetView(target);
		for (unsigned int i = 0; i < (unsigned int)eventLog.size(); ++i)
		{
			const ISystemGUIInterface::SystemLogEntry& logEntry = eventLog[i];
			targetView << logEntry.eventTimeString << L'\t' << logEntry.eventLevelString << L'\t' << logEntry.source << L'\t' << logEntry.text << L'\n';
		}
	}
}
