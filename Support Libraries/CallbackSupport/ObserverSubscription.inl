//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
ObserverSubscription::ObserverSubscription()
:_callback(0)
{ }

//----------------------------------------------------------------------------------------------------------------------
ObserverSubscription::ObserverSubscription(const std::function<void()>& callback)
{
	_callback = new std::function<void()>(callback);
}

//----------------------------------------------------------------------------------------------------------------------
ObserverSubscription::~ObserverSubscription()
{
	UnsubscribeAll();
	delete _callback;
}

//----------------------------------------------------------------------------------------------------------------------
// Callback binding functions
//----------------------------------------------------------------------------------------------------------------------
void ObserverSubscription::SetBoundCallback(const std::function<void()>& callback)
{
	delete _callback;
	_callback = new std::function<void()>(callback);
}

//----------------------------------------------------------------------------------------------------------------------
void ObserverSubscription::RemoveBoundCallback()
{
	delete _callback;
	_callback = 0;
}

//----------------------------------------------------------------------------------------------------------------------
// Subscription functions
//----------------------------------------------------------------------------------------------------------------------
void ObserverSubscription::Subscribe(IObserverCollection& targetCollection)
{
	targetCollection.AddObserver(*this);
}

//----------------------------------------------------------------------------------------------------------------------
void ObserverSubscription::Unsubscribe(IObserverCollection& targetCollection)
{
	targetCollection.RemoveObserver(*this);
}

//----------------------------------------------------------------------------------------------------------------------
void ObserverSubscription::UnsubscribeAll()
{
	std::unique_lock<std::mutex> lock(_accessMutex);
	while (!_linkedCollections.empty())
	{
		IObserverCollection& targetCollection = *(*_linkedCollections.begin());
		lock.unlock();
		targetCollection.RemoveObserver(*this);
		lock.lock();
	}
}

//----------------------------------------------------------------------------------------------------------------------
// Notification functions
//----------------------------------------------------------------------------------------------------------------------
void ObserverSubscription::Notify() const
{
	if (_callback != 0)
	{
		(*_callback)();
	}
}

//----------------------------------------------------------------------------------------------------------------------
void ObserverSubscription::NotifyLinkedToCollection(IObserverCollection& targetCollection)
{
	std::unique_lock<std::mutex> lock(_accessMutex);
	_linkedCollections.insert(&targetCollection);
}

//----------------------------------------------------------------------------------------------------------------------
void ObserverSubscription::NotifyUnlinkedFromCollection(IObserverCollection& targetCollection)
{
	std::unique_lock<std::mutex> lock(_accessMutex);
	_linkedCollections.erase(&targetCollection);
}
