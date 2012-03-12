//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
template<class T> ThinList<T>::ThinList()
:next(0)
{}

//----------------------------------------------------------------------------------------
template<class T> ThinList<T>::ThinList(const T& aobject, ThinList<T>* anext)
:next(anext), object(aobject)
{}

//----------------------------------------------------------------------------------------
template<class T> ThinList<T>::ThinList(const ThinList<T>& source)
:next(0), object(source.object)
{
	if(source.next != 0)
	{
		next = new ThinList<T>(*source.next);
	}
}

//----------------------------------------------------------------------------------------
template<class T> ThinList<T>::~ThinList()
{
	delete next;
}

//----------------------------------------------------------------------------------------
//Insert functions
//----------------------------------------------------------------------------------------
template<class T> void ThinList<T>::PushToBack(const T& entry)
{
	if(next == 0)
	{
		next = new ThinList<T>(entry);
	}
	else
	{
		next->PushToBack(entry);
	}
}

//----------------------------------------------------------------------------------------
template<class T> void ThinList<T>::InsertAfter(const T& entry)
{
	ThinList<T>* newEntry = new ThinList<T>(entry, next);
	next = newEntry;
}
