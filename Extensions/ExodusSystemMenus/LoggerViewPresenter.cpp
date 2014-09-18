#include "LoggerViewPresenter.h"
#include "LoggerView.h"
#include "Stream/Stream.pkg"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
LoggerViewPresenter::LoggerViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, ExodusSystemMenus& aowner, ISystemGUIInterface& amodel)
:ViewPresenterBase(aowner.GetAssemblyHandle(), aviewGroupName, aviewName, aviewID), owner(aowner), model(amodel)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring LoggerViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Event Log";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* LoggerViewPresenter::CreateView(IUIManager& uiManager)
{
	return new LoggerView(uiManager, *this, model);
}

//----------------------------------------------------------------------------------------
void LoggerViewPresenter::DeleteView(IView* aview)
{
	delete aview;
}

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
void LoggerViewPresenter::OpenLoggerDetailsView(const ISystemGUIInterface::SystemLogEntry& alogEntry)
{
	owner.OpenLoggerDetailsView(alogEntry);
}

//----------------------------------------------------------------------------------------
//Log functions
//----------------------------------------------------------------------------------------
void LoggerViewPresenter::SaveEventLog(const std::vector<ISystemGUIInterface::SystemLogEntry>& eventLog, const std::wstring& filePath)
{
	Stream::File target;
	if(target.Open(filePath, Stream::File::OPENMODE_READANDWRITE, Stream::File::CREATEMODE_CREATE))
	{
		target.SetTextEncoding(Stream::IStream::TEXTENCODING_UTF8);
		target.InsertByteOrderMark();
		Stream::ViewText targetView(target);
		for(unsigned int i = 0; i < (unsigned int)eventLog.size(); ++i)
		{
			const ISystemGUIInterface::SystemLogEntry& logEntry = eventLog[i];
			targetView << logEntry.eventTimeString << L'\t' << logEntry.eventLevelString << L'\t' << logEntry.source << L'\t' << logEntry.text << L'\n';
		}
	}
}
