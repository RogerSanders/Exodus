//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
template<class T> ThinListDoubleContainer<T>::ThinListDoubleContainer()
:first(0), last(0)
{}

//----------------------------------------------------------------------------------------
template<class T> ThinListDoubleContainer<T>::~ThinListDoubleContainer()
{
	delete first;
}

//----------------------------------------------------------------------------------------
//Iteration functions
//----------------------------------------------------------------------------------------
template<class T> typename ThinListDoubleContainer<T>::Element* ThinListDoubleContainer<T>::GetFirstEntry() const
{
	return first;
}

//----------------------------------------------------------------------------------------
template<class T> typename ThinListDoubleContainer<T>::Element* ThinListDoubleContainer<T>::GetLastEntry() const
{
	return last;
}

//----------------------------------------------------------------------------------------
//Insert functions
//----------------------------------------------------------------------------------------
template<class T> void ThinListDoubleContainer<T>::PushToFront(const T& entry)
{
	if(first != 0)
	{
		first->InsertBefore(entry);
		first = first->previous;
	}
	else
	{
		Element* newEntry = new Element(entry);
		first = newEntry;
		last = newEntry;
	}
}

//----------------------------------------------------------------------------------------
template<class T> void ThinListDoubleContainer<T>::PushToBack(const T& entry)
{
	if(last != 0)
	{
		last->InsertAfter(entry);
		last = last->next;
	}
	else
	{
		Element* newEntry = new Element(entry);
		first = newEntry;
		last = newEntry;
	}
}

//----------------------------------------------------------------------------------------
template<class T> void ThinListDoubleContainer<T>::InsertBefore(Element* element, const T& entry)
{
	element->InsertBefore(entry);
	if(element == first)
	{
		first = first->previous;
	}
}

//----------------------------------------------------------------------------------------
template<class T> void ThinListDoubleContainer<T>::InsertAfter(Element* element, const T& entry)
{
	element->InsertAfter(entry);
	if(element == last)
	{
		last = last->next;
	}
}

//----------------------------------------------------------------------------------------
//Erase functions
//----------------------------------------------------------------------------------------
template<class T> void ThinListDoubleContainer<T>::EraseFromFrontToElement(Element* targetElement)
{
	if(targetElement != first)
	{
		Element* deleteStart = first;
		first = targetElement;
		if(targetElement->previous != 0)
		{
			targetElement->previous->next = 0;
			targetElement->previous = 0;
		}
		delete deleteStart;
	}
}

//----------------------------------------------------------------------------------------
template<class T> void ThinListDoubleContainer<T>::EraseFromBackToElement(Element* targetElement)
{
	Element* deleteStart = targetElement->next;
	last = targetElement;
	targetElement->next = 0;
	delete deleteStart;
}

//----------------------------------------------------------------------------------------
template<class T> void ThinListDoubleContainer<T>::Clear()
{
	delete first;
	first = 0;
	last = 0;
}

//----------------------------------------------------------------------------------------
//Query functions
//----------------------------------------------------------------------------------------
template<class T> bool ThinListDoubleContainer<T>::Empty() const
{
	return (first == 0);
}
