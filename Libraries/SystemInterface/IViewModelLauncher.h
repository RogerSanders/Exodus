#ifndef __IVIEWMODELLAUNCHER_H__
#define __IVIEWMODELLAUNCHER_H__
#include "IViewModel.h"

class IViewModelLauncher
{
public:
	//Constructors
	virtual ~IViewModelLauncher() = 0 {}

	//View management functions
	virtual bool OpenViewModel(IViewModel* aviewModel, bool waitToClose = true, bool openHidden = false) = 0;
	virtual void CloseViewModel(IViewModel* aviewModel, bool waitToClose = true) = 0;
	virtual void ShowViewModel(IViewModel* aviewModel) = 0;
	virtual void HideViewModel(IViewModel* aviewModel) = 0;
	virtual void ActivateViewModel(IViewModel* aviewModel) = 0;
	virtual bool WaitUntilViewModelOpened(IViewModel* aviewModel) = 0;
	virtual void WaitUntilViewModelClosed(IViewModel* aviewModel) = 0;
};

#endif
