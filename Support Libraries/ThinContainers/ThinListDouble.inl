//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
template<class T>
ThinListDouble<T>::ThinListDouble()
:previous(0), next(0)
{ }

//----------------------------------------------------------------------------------------------------------------------
template<class T>
ThinListDouble<T>::ThinListDouble(const T& aobject, ThinListDouble<T>* aprevious, ThinListDouble<T>* anext)
:previous(aprevious), next(anext), object(aobject)
{ }

//----------------------------------------------------------------------------------------------------------------------
template<class T>
ThinListDouble<T>::ThinListDouble(const ThinListDouble<T>& source, ThinListDouble<T>* aprevious)
:previous(aprevious), next(0), object(source.object)
{
	if (source.next != 0)
	{
		next = new ThinListDouble<T>(*source.next, this);
	}
}

//----------------------------------------------------------------------------------------------------------------------
template<class T>
ThinListDouble<T>::~ThinListDouble()
{
	ThinListDouble<T>* deleteEntry = next;
	while (deleteEntry != 0)
	{
		ThinListDouble<T>* nextDeleteEntry = deleteEntry->next;
		deleteEntry->next = 0;
		delete deleteEntry;
		deleteEntry = nextDeleteEntry;
	}
}

//----------------------------------------------------------------------------------------------------------------------
// Insert functions
//----------------------------------------------------------------------------------------------------------------------
template<class T>
void ThinListDouble<T>::PushToBack(const T& entry)
{
	ThinListDouble<T>* targetEntry = this;
	while (targetEntry->next != 0)
	{
		targetEntry = targetEntry->next;
	}
	targetEntry->next = new ThinListDouble<T>(entry, targetEntry);
}

//----------------------------------------------------------------------------------------------------------------------
template<class T>
void ThinListDouble<T>::InsertAfter(const T& entry)
{
	ThinListDouble<T>* newEntry = new ThinListDouble<T>(entry, this, next);
	if (next != 0)
	{
		next->previous = newEntry;
	}
	next = newEntry;
}

//----------------------------------------------------------------------------------------------------------------------
template<class T>
void ThinListDouble<T>::InsertBefore(const T& entry)
{
	ThinListDouble<T>* newEntry = new ThinListDouble<T>(entry, previous, this);
	if (previous != 0)
	{
		previous->next = newEntry;
	}
	previous = newEntry;
}
