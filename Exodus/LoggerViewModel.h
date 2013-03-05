#include "System.h"
#ifndef __SYSTEM_LOGGERVIEWMODEL_H__
#define __SYSTEM_LOGGERVIEWMODEL_H__
#include "SystemInterface/SystemInterface.pkg"

class System::LoggerViewModel :public ViewModelBase
{
public:
	//Constructors
	LoggerViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, System* adevice);

	//View creation and deletion
	virtual IView* CreateView();
	virtual void DeleteView(IView* aview);

private:
	System* device;
};

#endif
