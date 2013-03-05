#include "MemoryRead.h"
#ifndef __MEMORYREAD_MEMORYEDITORVIEWMODEL_H__
#define __MEMORYREAD_MEMORYEDITORVIEWMODEL_H__
#include "SystemInterface/SystemInterface.pkg"

class MemoryRead::MemoryEditorViewModel :public ViewModelBase
{
public:
	//Constructors
	MemoryEditorViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, MemoryRead* adevice);

	//View creation and deletion
	virtual IView* CreateView();
	virtual void DeleteView(IView* aview);

private:
	MemoryRead* device;
};

#endif
