#include "SN76489.h"
#ifndef __SN76489_LOGGINGVIEWMODEL_H__
#define __SN76489_LOGGINGVIEWMODEL_H__
#include "SystemInterface/SystemInterface.pkg"

class SN76489::LoggingViewModel :public ViewModelBase
{
public:
	//Constructors
	LoggingViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, SN76489* adevice);

	//View creation and deletion
	virtual IView* CreateView();
	virtual void DeleteView(IView* aview);

private:
	SN76489* device;
};

#endif
