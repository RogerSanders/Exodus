#include "Processor.h"
#ifndef __PROCESSOR_DISASSEMBLYVIEWMODEL_H__
#define __PROCESSOR_DISASSEMBLYVIEWMODEL_H__
#include "SystemInterface/SystemInterface.pkg"

class Processor::DisassemblyViewModel :public ViewModelBase
{
public:
	//Constructors
	DisassemblyViewModel(const std::wstring& amenuHandlerName, int aviewModelID, Processor* adevice);

	//View creation and deletion
	virtual IView* CreateView();
	virtual void DeleteView(IView* aview);

private:
	Processor* device;
};

#endif
