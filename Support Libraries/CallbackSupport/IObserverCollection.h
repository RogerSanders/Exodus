#ifndef __IOBSERVERCOLLECTION_H__
#define __IOBSERVERCOLLECTION_H__
class IObserverSubscription;

//##TODO## Consider a template argument approach using variadic templates when we move to
//C++11, to support notification with arguments.
class IObserverCollection
{
public:
	//Constructors
	virtual ~IObserverCollection() = 0 {}

	//Observer management functions
	virtual void AddObserver(IObserverSubscription& observer) = 0;
	virtual void RemoveObserver(IObserverSubscription& observer) = 0;
	virtual void RemoveAllObservers() = 0;

	//Notification functions
	virtual void NotifyObservers() const = 0;
};

#endif
