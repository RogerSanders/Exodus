//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ObserverSubscription::ObserverSubscription()
:callback(0)
{}

//----------------------------------------------------------------------------------------
ObserverSubscription::ObserverSubscription(const boost::function<void()>& acallback)
{
	callback = new boost::function<void()>(acallback);
}

//----------------------------------------------------------------------------------------
ObserverSubscription::~ObserverSubscription()
{
	UnsubscribeAll();
	delete callback;
}

//----------------------------------------------------------------------------------------
//Callback binding functions
//----------------------------------------------------------------------------------------
void ObserverSubscription::SetBoundCallback(const boost::function<void()>& acallback)
{
	delete callback;
	callback = new boost::function<void()>(acallback);
}

//----------------------------------------------------------------------------------------
void ObserverSubscription::RemoveBoundCallback()
{
	delete callback;
	callback = 0;
}

//----------------------------------------------------------------------------------------
//Subscription functions
//----------------------------------------------------------------------------------------
void ObserverSubscription::Subscribe(IObserverCollection& targetCollection)
{
	targetCollection.AddObserver(*this);
}

//----------------------------------------------------------------------------------------
void ObserverSubscription::Unsubscribe(IObserverCollection& targetCollection)
{
	targetCollection.RemoveObserver(*this);
}

//----------------------------------------------------------------------------------------
void ObserverSubscription::UnsubscribeAll()
{
	boost::mutex::scoped_lock lock(accessMutex);
	while(!linkedCollections.empty())
	{
		IObserverCollection& targetCollection = *(*linkedCollections.begin());
		lock.unlock();
		targetCollection.RemoveObserver(*this);
		lock.lock();
	}
}

//----------------------------------------------------------------------------------------
//Notification functions
//----------------------------------------------------------------------------------------
void ObserverSubscription::Notify() const
{
	if(callback != 0)
	{
		(*callback)();
	}
}

//----------------------------------------------------------------------------------------
void ObserverSubscription::NotifyLinkedToCollection(IObserverCollection& targetCollection)
{
	boost::mutex::scoped_lock lock(accessMutex);
	linkedCollections.insert(&targetCollection);
}

//----------------------------------------------------------------------------------------
void ObserverSubscription::NotifyUnlinkedFromCollection(IObserverCollection& targetCollection)
{
	boost::mutex::scoped_lock lock(accessMutex);
	linkedCollections.erase(&targetCollection);
}
