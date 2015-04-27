#ifndef __OBSERVERSUBSCRIPTION_H__
#define __OBSERVERSUBSCRIPTION_H__
#include "IObserverSubscription.h"
#include "IObserverCollection.h"
#include <set>
#include <functional>
#include <mutex>

class ObserverSubscription :public IObserverSubscription
{
public:
	//Constructors
	inline ObserverSubscription();
	inline ObserverSubscription(const std::function<void()>& acallback);
	inline virtual ~ObserverSubscription();

	//Callback binding functions
	inline void SetBoundCallback(const std::function<void()>& acallback);
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
	mutable std::mutex accessMutex;
	std::set<IObserverCollection*> linkedCollections;
	std::function<void()>* callback;
};

#include "ObserverSubscription.inl"
#endif
