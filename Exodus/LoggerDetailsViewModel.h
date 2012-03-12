#include "System.h"
#ifndef __SYSTEM_LOGGERDETAILSVIEWMODEL_H__
#define __SYSTEM_LOGGERDETAILSVIEWMODEL_H__
#include "SystemInterface/SystemInterface.pkg"

class System::LoggerDetailsViewModel :public ViewModelBase
{
public:
	//Constructors
	LoggerDetailsViewModel(const std::wstring& amenuHandlerName, int aviewModelID, System* adevice, const LogEntryInternal& alogEntry);

	//Log entry functions
	void SetLogEntry(const LogEntryInternal& alogEntry);

	//View creation and deletion
	virtual IView* CreateView();
	virtual void DeleteView(IView* aview);

private:
	System* device;
	LogEntryInternal logEntry;
};

#endif
