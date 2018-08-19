//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ObserverCollection::ObserverCollection()
{}

//----------------------------------------------------------------------------------------
ObserverCollection::~ObserverCollection()
{
	RemoveAllObservers();
}

//----------------------------------------------------------------------------------------
//Observer management functions
//----------------------------------------------------------------------------------------
void ObserverCollection::AddObserver(IObserverSubscription& observer)
{
	std::unique_lock<std::mutex> lock(_accessMutex);
	if(_observers.find(&observer) == _observers.end())
	{
		observer.NotifyLinkedToCollection(*this);
		_observers.insert(&observer);
	}
}

//----------------------------------------------------------------------------------------
void ObserverCollection::RemoveObserver(IObserverSubscription& observer)
{
	std::unique_lock<std::mutex> lock(_accessMutex);
	std::set<IObserverSubscription*>::iterator observerIterator = _observers.find(&observer);
	if(observerIterator != _observers.end())
	{
		observer.NotifyUnlinkedFromCollection(*this);
		_observers.erase(observerIterator);
	}
}

//----------------------------------------------------------------------------------------
void ObserverCollection::RemoveAllObservers()
{
	std::unique_lock<std::mutex> lock(_accessMutex);
	while(!_observers.empty())
	{
		std::set<IObserverSubscription*>::iterator observerIterator = _observers.begin();
		IObserverSubscription& observer = *(*observerIterator);
		observer.NotifyUnlinkedFromCollection(*this);
		_observers.erase(observerIterator);
	}
}

//----------------------------------------------------------------------------------------
//Notification functions
//----------------------------------------------------------------------------------------
void ObserverCollection::NotifyObservers() const
{
	std::unique_lock<std::mutex> lock(_accessMutex);
	for(std::set<IObserverSubscription*>::const_iterator i = _observers.begin(); i != _observers.end(); ++i)
	{
		const IObserverSubscription& observer = *(*i);
		observer.Notify();
	}
}
