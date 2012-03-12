#include "Z80.h"
#ifndef __Z80_REGISTERSVIEWMODEL_H__
#define __Z80_REGISTERSVIEWMODEL_H__
#include "SystemInterface/SystemInterface.pkg"
namespace Z80{

class Z80::RegistersViewModel :public ViewModelBase
{
public:
	//Constructors
	RegistersViewModel(const std::wstring& amenuHandlerName, int aviewModelID, Z80* adevice);

	//View creation and deletion
	virtual IView* CreateView();
	virtual void DeleteView(IView* aview);

private:
	Z80* device;
};

} //Close namespace Z80
#endif
