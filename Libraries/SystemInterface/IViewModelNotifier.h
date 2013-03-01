#ifndef __IVIEWMODELNOTIFIER_H__
#define __IVIEWMODELNOTIFIER_H__

class IViewModelNotifier
{
public:
	//Interface version functions
	static inline unsigned int ThisIViewModelNotifierVersion();
	virtual unsigned int GetIViewModelNotifierVersion() const = 0;

	//Notification functions
	virtual void NotifyViewClosed() = 0;
};

#include "IViewModelNotifier.inl"
#endif
