#ifndef __OBSERVERCOLLECTION_H__
#define __OBSERVERCOLLECTION_H__
#include "IObserverCollection.h"
#include "IObserverSubscription.h"
#include <set>
#include <mutex>

class ObserverCollection :public IObserverCollection
{
public:
	//Constructors
	inline ObserverCollection();
	inline virtual ~ObserverCollection();

	//Observer management functions
	inline virtual void AddObserver(IObserverSubscription& observer);
	inline virtual void RemoveObserver(IObserverSubscription& observer);
	inline virtual void RemoveAllObservers();

	//Notification functions
	inline virtual void NotifyObservers() const;

private:
	//Ensure this class is non-copyable, since our subscribed observers will keep a
	//reference to it.
	inline ObserverCollection(const ObserverCollection&)
	{}
	inline ObserverCollection& operator=(const ObserverCollection&)
	{}

private:
	mutable std::mutex accessMutex;
	std::set<IObserverSubscription*> observers;
};

#include "ObserverCollection.inl"
#endif
