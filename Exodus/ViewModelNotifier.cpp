#include "ViewModelNotifier.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ViewModelNotifier::ViewModelNotifier(IViewModelManager& aviewManager, IViewModel* aviewModel)
:viewManager(aviewManager), viewModel(aviewModel)
{}

//----------------------------------------------------------------------------------------
//Notification functions
//----------------------------------------------------------------------------------------
void ViewModelNotifier::NotifyViewClosed()
{
	viewManager.NotifyModelViewClosed(viewModel);
}
