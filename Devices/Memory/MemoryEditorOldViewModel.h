#include "MemoryRead.h"
#ifndef __MEMORYREAD_MEMORYEDITOROLDVIEWMODEL_H__
#define __MEMORYREAD_MEMORYEDITOROLDVIEWMODEL_H__
#include "SystemInterface/SystemInterface.pkg"

class MemoryRead::MemoryEditorOldViewModel :public ViewModelBase
{
public:
	//Constructors
	MemoryEditorOldViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, MemoryRead* adevice, unsigned int atargetMemoryAddress = 0);

	//Memory address functions
	void SetTargetMemoryAddress(unsigned int atargetMemoryAddress);

	//View creation and deletion
	virtual IView* CreateView();
	virtual void DeleteView(IView* aview);

private:
	MemoryRead* device;
	unsigned int targetMemoryAddress;
};

#endif
