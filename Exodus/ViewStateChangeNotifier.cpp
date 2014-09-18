#include "ViewStateChangeNotifier.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ViewStateChangeNotifier::ViewStateChangeNotifier(IViewManagerNotifierInterface& aviewManager, IViewPresenter& aviewPresenter)
:viewManager(aviewManager), viewPresenter(aviewPresenter)
{}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int ViewStateChangeNotifier::GetIViewStateChangeNotifierVersion() const
{
	return ThisIViewStateChangeNotifierVersion();
}

//----------------------------------------------------------------------------------------
//Notification functions
//----------------------------------------------------------------------------------------
void ViewStateChangeNotifier::NotifyViewClosed()
{
	viewManager.NotifyViewClosed(viewPresenter);
}
