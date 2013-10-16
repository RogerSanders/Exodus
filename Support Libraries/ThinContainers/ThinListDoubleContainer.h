#ifndef __THINLISTDOUBLECONTAINER_H__
#define __THINLISTDOUBLECONTAINER_H__

template<class T> struct ThinListDoubleContainer
{
public:
	//Typedefs
	typedef ThinListDouble<T> Element;

	//Constructors
	ThinListDoubleContainer();
	~ThinListDoubleContainer();

	//Iteration functions
	Element* GetFirstEntry() const;
	Element* GetLastEntry() const;

	//Insert functions
	void PushToFront(const T& entry);
	void PushToBack(const T& entry);
	void InsertBefore(Element* element, const T& entry);
	void InsertAfter(Element* element, const T& entry);

	//Erase functions
	void EraseFromFrontToElement(Element* targetElement);
	void EraseFromBackToElement(Element* targetElement);
	void Clear();

	//Query functions
	bool Empty() const;

public:
	Element* first;
	Element* last;
};

#include "ThinListDoubleContainer.inl"
#endif
