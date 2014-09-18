#ifndef __IVIEWSTATECHANGENOTIFIER_H__
#define __IVIEWSTATECHANGENOTIFIER_H__

class IViewStateChangeNotifier
{
public:
	//Interface version functions
	static inline unsigned int ThisIViewStateChangeNotifierVersion();
	virtual unsigned int GetIViewStateChangeNotifierVersion() const = 0;

	//Notification functions
	virtual void NotifyViewClosed() = 0;
};

#include "IViewStateChangeNotifier.inl"
#endif
