#include "LoggerDetailsViewModel.h"
#include "LoggerDetailsView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
System::LoggerDetailsViewModel::LoggerDetailsViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, System* adevice, const LogEntryInternal& alogEntry)
:ViewModelBase(aviewModelGroupName, aviewModelName, aviewModelID, true, false, L"", 0), device(adevice), logEntry(alogEntry)
{}

//----------------------------------------------------------------------------------------
//Log entry functions
//----------------------------------------------------------------------------------------
void System::LoggerDetailsViewModel::SetLogEntry(const LogEntryInternal& alogEntry)
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

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* System::LoggerDetailsViewModel::CreateView()
{
	return new LoggerDetailsView(device, logEntry);
}

//----------------------------------------------------------------------------------------
void System::LoggerDetailsViewModel::DeleteView(IView* aview)
{
	delete aview;
}
