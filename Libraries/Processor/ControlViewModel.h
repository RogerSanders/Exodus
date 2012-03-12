#include "Processor.h"
#ifndef __PROCESSOR_CONTROLVIEWMODEL_H__
#define __PROCESSOR_CONTROLVIEWMODEL_H__
#include "SystemInterface/SystemInterface.pkg"

class Processor::ControlViewModel :public ViewModelBase
{
public:
	//Constructors
	ControlViewModel(const std::wstring& amenuHandlerName, int aviewModelID, Processor* adevice);

	//View creation and deletion
	virtual IView* CreateView();
	virtual void DeleteView(IView* aview);

private:
	Processor* device;
};

#endif
