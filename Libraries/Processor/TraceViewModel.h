#include "Processor.h"
#ifndef __PROCESSOR_TRACEVIEWMODEL_H__
#define __PROCESSOR_TRACEVIEWMODEL_H__
#include "SystemInterface/SystemInterface.pkg"

class Processor::TraceViewModel :public ViewModelBase
{
public:
	//Constructors
	TraceViewModel(const std::wstring& amenuHandlerName, int aviewModelID, Processor* adevice);

	//View creation and deletion
	virtual IView* CreateView();
	virtual void DeleteView(IView* aview);

private:
	Processor* device;
};

#endif
