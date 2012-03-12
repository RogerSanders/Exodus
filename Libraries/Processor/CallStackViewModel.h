#include "Processor.h"
#ifndef __PROCESSOR_CALLSTACKVIEWMODEL_H__
#define __PROCESSOR_CALLSTACKVIEWMODEL_H__
#include "SystemInterface/SystemInterface.pkg"

class Processor::CallStackViewModel :public ViewModelBase
{
public:
	//Constructors
	CallStackViewModel(const std::wstring& amenuHandlerName, int aviewModelID, Processor* adevice);

	//View creation and deletion
	virtual IView* CreateView();
	virtual void DeleteView(IView* aview);

private:
	Processor* device;
};

#endif
