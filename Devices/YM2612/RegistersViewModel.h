#include "YM2612.h"
#ifndef __YM2612_REGISTERSVIEWMODEL_H__
#define __YM2612_REGISTERSVIEWMODEL_H__
#include "SystemInterface/SystemInterface.pkg"

class YM2612::RegistersViewModel :public ViewModelBase
{
public:
	//Constructors
	RegistersViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, YM2612* adevice);

	//View creation and deletion
	virtual IView* CreateView();
	virtual void DeleteView(IView* aview);

private:
	YM2612* device;
};

#endif
