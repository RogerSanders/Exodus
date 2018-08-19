#include "ViewStateChangeNotifier.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
ViewStateChangeNotifier::ViewStateChangeNotifier(IViewManagerNotifierInterface& viewManager, IViewPresenter& viewPresenter)
:_viewManager(viewManager), _viewPresenter(viewPresenter)
{}

//----------------------------------------------------------------------------------------------------------------------
// Interface version functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int ViewStateChangeNotifier::GetIViewStateChangeNotifierVersion() const
{
	return ThisIViewStateChangeNotifierVersion();
}

//----------------------------------------------------------------------------------------------------------------------
// Notification functions
//----------------------------------------------------------------------------------------------------------------------
void ViewStateChangeNotifier::NotifyViewClosed()
{
	_viewManager.NotifyViewClosed(_viewPresenter);
}
