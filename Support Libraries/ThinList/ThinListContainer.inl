//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
template<class T> ThinListContainer<T>::ThinListContainer()
:first(0)
{}

//----------------------------------------------------------------------------------------
template<class T> ThinListContainer<T>::~ThinListContainer()
{
	delete first;
}

//----------------------------------------------------------------------------------------
//Iteration functions
//----------------------------------------------------------------------------------------
template<class T> typename ThinListContainer<T>::Element* ThinListContainer<T>::GetFirstEntry() const
{
	return first;
}

//----------------------------------------------------------------------------------------
//Insert functions
//----------------------------------------------------------------------------------------
template<class T> void ThinListContainer<T>::PushToFront(const T& entry)
{
	Element* newEntry = new Element(entry, first);
	first = newEntry;
}

//----------------------------------------------------------------------------------------
template<class T> void ThinListContainer<T>::InsertAfter(Element* element, const T& entry)
{
	element->InsertAfter(entry);
}

//----------------------------------------------------------------------------------------
//Erase functions
//----------------------------------------------------------------------------------------
template<class T> void ThinListContainer<T>::EraseFromBackToElement(Element* targetElement)
{
	Element* deleteStart = targetElement->next;
	targetElement->next = 0;
	delete deleteStart;
}

//----------------------------------------------------------------------------------------
template<class T> void ThinListContainer<T>::Clear()
{
	delete first;
	first = 0;
}

//----------------------------------------------------------------------------------------
//Query functions
//----------------------------------------------------------------------------------------
template<class T> bool ThinListContainer<T>::Empty() const
{
	return (first == 0);
}
