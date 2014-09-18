#ifndef __OBSERVERSUBSCRIPTION_H__
#define __OBSERVERSUBSCRIPTION_H__
#include "IObserverCollection.h"
#include <set>
#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>

class ObserverSubscription :public IObserverSubscription
{
public:
	//Constructors
	inline ObserverSubscription();
	inline ObserverSubscription(const boost::function<void()>& acallback);
	inline virtual ~ObserverSubscription();

	//Callback binding functions
	inline void SetBoundCallback(const boost::function<void()>& acallback);
	inline void RemoveBoundCallback();

	//Subscription functions
	inline virtual void Subscribe(IObserverCollection& targetCollection);
	inline virtual void Unsubscribe(IObserverCollection& targetCollection);
	inline virtual void UnsubscribeAll();

	//Notification functions
	inline virtual void Notify() const;
	inline virtual void NotifyLinkedToCollection(IObserverCollection& targetCollection);
	inline virtual void NotifyUnlinkedFromCollection(IObserverCollection& targetCollection);

private:
	//Ensure this class is non-copyable, since any collections we're subscribed to will
	//keep a reference to this object.
	inline ObserverSubscription(const ObserverSubscription& source) {}
	inline ObserverSubscription& operator=(const ObserverSubscription& source) {}

private:
	mutable boost::mutex accessMutex;
	std::set<IObserverCollection*> linkedCollections;
	boost::function<void()>* callback;
};

#include "ObserverSubscription.inl"
#endif
