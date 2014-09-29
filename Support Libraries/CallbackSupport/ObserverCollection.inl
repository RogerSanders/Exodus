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
	std::unique_lock<std::mutex> lock(accessMutex);
	if(observers.find(&observer) == observers.end())
	{
		observer.NotifyLinkedToCollection(*this);
		observers.insert(&observer);
	}
}

//----------------------------------------------------------------------------------------
void ObserverCollection::RemoveObserver(IObserverSubscription& observer)
{
	std::unique_lock<std::mutex> lock(accessMutex);
	std::set<IObserverSubscription*>::iterator observerIterator = observers.find(&observer);
	if(observerIterator != observers.end())
	{
		observer.NotifyUnlinkedFromCollection(*this);
		observers.erase(observerIterator);
	}
}

//----------------------------------------------------------------------------------------
void ObserverCollection::RemoveAllObservers()
{
	std::unique_lock<std::mutex> lock(accessMutex);
	while(!observers.empty())
	{
		std::set<IObserverSubscription*>::iterator observerIterator = observers.begin();
		IObserverSubscription& observer = *(*observerIterator);
		observer.NotifyUnlinkedFromCollection(*this);
		observers.erase(observerIterator);
	}
}

//----------------------------------------------------------------------------------------
//Notification functions
//----------------------------------------------------------------------------------------
void ObserverCollection::NotifyObservers() const
{
	std::unique_lock<std::mutex> lock(accessMutex);
	for(std::set<IObserverSubscription*>::const_iterator i = observers.begin(); i != observers.end(); ++i)
	{
		const IObserverSubscription& observer = *(*i);
		observer.Notify();
	}
}
