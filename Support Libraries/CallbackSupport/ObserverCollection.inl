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
	boost::mutex::scoped_lock lock(accessMutex);
	if(observers.find(&observer) == observers.end())
	{
		observer.NotifyLinkedToCollection(*this);
		observers.insert(&observer);
	}
}

//----------------------------------------------------------------------------------------
void ObserverCollection::RemoveObserver(IObserverSubscription& observer)
{
	boost::mutex::scoped_lock lock(accessMutex);
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
	boost::mutex::scoped_lock lock(accessMutex);
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
	boost::mutex::scoped_lock lock(accessMutex);
	for(std::set<IObserverSubscription*>::const_iterator i = observers.begin(); i != observers.end(); ++i)
	{
		const IObserverSubscription& observer = *(*i);
		observer.Notify();
	}
}
