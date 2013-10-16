#ifndef __THINLISTCONTAINER_H__
#define __THINLISTCONTAINER_H__

template<class T> struct ThinListContainer
{
public:
	//Typedefs
	typedef ThinList<T> Element;

	//Constructors
	ThinListContainer();
	~ThinListContainer();

	//Iteration functions
	Element* GetFirstEntry() const;

	//Insert functions
	void PushToFront(const T& entry);
	void InsertAfter(Element* element, const T& entry);

	//Erase functions
	void EraseFromBackToElement(Element* targetElement);
	void Clear();

	//Query functions
	bool Empty() const;

public:
	Element* first;
};

#include "ThinListContainer.inl"
#endif
