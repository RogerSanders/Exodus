#include "M68000.h"
#ifndef __M68000_REGISTERSVIEWMODEL_H__
#define __M68000_REGISTERSVIEWMODEL_H__
#include "SystemInterface/SystemInterface.pkg"
namespace M68000{

class M68000::RegistersViewModel :public ViewModelBase
{
public:
	//Constructors
	RegistersViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, M68000* adevice);

	//View creation and deletion
	virtual IView* CreateView();
	virtual void DeleteView(IView* aview);

private:
	M68000* device;
};

} //Close namespace M68000
#endif
