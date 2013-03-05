#include "YM2612.h"
#ifndef __YM2612_LOGGINGVIEWMODEL_H__
#define __YM2612_LOGGINGVIEWMODEL_H__
#include "SystemInterface/SystemInterface.pkg"

class YM2612::LoggingViewModel :public ViewModelBase
{
public:
	//Constructors
	LoggingViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, YM2612* adevice);

	//View creation and deletion
	virtual IView* CreateView();
	virtual void DeleteView(IView* aview);

private:
	YM2612* device;
};

#endif
