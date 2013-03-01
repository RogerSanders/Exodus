#ifndef __VIEWMODELNOTIFIER_H__
#define __VIEWMODELNOTIFIER_H__
#include "SystemInterface/SystemInterface.pkg"
#include "IViewModelManager.h"

class ViewModelNotifier :public IViewModelNotifier
{
public:
	//Constructors
	ViewModelNotifier(IViewModelManager& aviewManager, IViewModel* aviewModel);

	//Interface version functions
	virtual unsigned int GetIViewModelNotifierVersion() const;

	//Notification functions
	virtual void NotifyViewClosed();

private:
	IViewModel* viewModel;
	IViewModelManager& viewManager;
};

#endif
