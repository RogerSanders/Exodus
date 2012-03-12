#include "YM2612.h"
#ifndef __YM2612_DEBUGGERVIEWMODEL_H__
#define __YM2612_DEBUGGERVIEWMODEL_H__
#include "SystemInterface/SystemInterface.pkg"

class YM2612::DebuggerViewModel :public ViewModelBase
{
public:
	//Constructors
	DebuggerViewModel(const std::wstring& amenuHandlerName, int aviewModelID, YM2612* adevice);

	//View creation and deletion
	virtual IView* CreateView();
	virtual void DeleteView(IView* aview);

private:
	YM2612* device;
};

#endif
