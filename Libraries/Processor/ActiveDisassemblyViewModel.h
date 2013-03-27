#include "Processor.h"
#ifndef __PROCESSOR_ACTIVEDISASSEMBLYVIEWMODEL_H__
#define __PROCESSOR_ACTIVEDISASSEMBLYVIEWMODEL_H__
#include "SystemInterface/SystemInterface.pkg"

class Processor::ActiveDisassemblyViewModel :public ViewModelBase
{
public:
	//Constructors
	ActiveDisassemblyViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, Processor* adevice);

	//View creation and deletion
	virtual IView* CreateView();
	virtual void DeleteView(IView* aview);

private:
	Processor* device;
};

#endif
