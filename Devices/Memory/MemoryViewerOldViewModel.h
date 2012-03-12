#include "MemoryRead.h"
#ifndef __MEMORYREAD_MEMORYVIEWEROLDVIEWMODEL_H__
#define __MEMORYREAD_MEMORYVIEWEROLDVIEWMODEL_H__
#include "SystemInterface/SystemInterface.pkg"

class MemoryRead::MemoryViewerOldViewModel :public ViewModelBase
{
public:
	//Constructors
	MemoryViewerOldViewModel(const std::wstring& amenuHandlerName, int aviewModelID, MemoryRead* adevice);

	//View creation and deletion
	virtual IView* CreateView();
	virtual void DeleteView(IView* aview);

private:
	MemoryRead* device;
};

#endif
