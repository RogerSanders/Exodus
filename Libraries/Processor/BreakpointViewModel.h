#include "Processor.h"
#ifndef __PROCESSOR_BREAKPOINTVIEWMODEL_H__
#define __PROCESSOR_BREAKPOINTVIEWMODEL_H__
#include "SystemInterface/SystemInterface.pkg"

class Processor::BreakpointViewModel :public ViewModelBase
{
public:
	//Constructors
	BreakpointViewModel(const std::wstring& amenuHandlerName, int aviewModelID, Processor* adevice);

	//View creation and deletion
	virtual IView* CreateView();
	virtual void DeleteView(IView* aview);

private:
	Processor* device;
};

#endif
