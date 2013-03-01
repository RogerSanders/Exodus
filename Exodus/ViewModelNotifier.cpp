#include "ViewModelNotifier.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ViewModelNotifier::ViewModelNotifier(IViewModelManager& aviewManager, IViewModel* aviewModel)
:viewManager(aviewManager), viewModel(aviewModel)
{}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int ViewModelNotifier::GetIViewModelNotifierVersion() const
{
	return ThisIViewModelNotifierVersion();
}

//----------------------------------------------------------------------------------------
//Notification functions
//----------------------------------------------------------------------------------------
void ViewModelNotifier::NotifyViewClosed()
{
	viewManager.NotifyModelViewClosed(viewModel);
}
