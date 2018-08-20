#ifndef __OBSERVERCOLLECTION_H__
#define __OBSERVERCOLLECTION_H__
#include "IObserverCollection.h"
#include "IObserverSubscription.h"
#include <set>
#include <mutex>

class ObserverCollection :public IObserverCollection
{
public:
	// Constructors
	inline ObserverCollection();
	inline virtual ~ObserverCollection();

	// Observer management functions
	inline virtual void AddObserver(IObserverSubscription& observer);
	inline virtual void RemoveObserver(IObserverSubscription& observer);
	inline virtual void RemoveAllObservers();

	// Notification functions
	inline virtual void NotifyObservers() const;

private:
	// Ensure this class is non-copyable, since our subscribed observers will keep a
	// reference to it.
	ObserverCollection(const ObserverCollection&) = delete;
	ObserverCollection& operator=(const ObserverCollection&) = delete;

private:
	mutable std::mutex _accessMutex;
	std::set<IObserverSubscription*> _observers;
};

#include "ObserverCollection.inl"
#endif
