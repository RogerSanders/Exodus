#include "Processor.h"
#ifndef __PROCESSOR_DISASSEMBLYOLDVIEWMODEL_H__
#define __PROCESSOR_DISASSEMBLYOLDVIEWMODEL_H__
#include "SystemInterface/SystemInterface.pkg"

class Processor::DisassemblyOldViewModel :public ViewModelBase
{
public:
	//Constructors
	DisassemblyOldViewModel(const std::wstring& amenuHandlerName, int aviewModelID, Processor* adevice);

	//View creation and deletion
	virtual IView* CreateView();
	virtual void DeleteView(IView* aview);

private:
	Processor* device;
};

#endif
