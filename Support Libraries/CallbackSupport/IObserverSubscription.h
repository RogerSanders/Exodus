#ifndef __IOBSERVERSUBSCRIPTION_H__
#define __IOBSERVERSUBSCRIPTION_H__
class IObserverCollection;

class IObserverSubscription
{
public:
	//Constructors
	virtual ~IObserverSubscription() = 0 {}

	//Subscription functions
	virtual void Subscribe(IObserverCollection& targetCollection) = 0;
	virtual void Unsubscribe(IObserverCollection& targetCollection) = 0;
	virtual void UnsubscribeAll() = 0;

	//Notification functions
	virtual void Notify() const = 0;
	virtual void NotifyLinkedToCollection(IObserverCollection& targetCollection) = 0;
	virtual void NotifyUnlinkedFromCollection(IObserverCollection& targetCollection) = 0;
};

#endif
