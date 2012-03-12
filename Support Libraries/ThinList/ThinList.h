#ifndef __THINLIST_H__
#define __THINLIST_H__

template<class T> struct ThinList
{
public:
	//Constructors
	ThinList();
	ThinList(const T& aobject, ThinList<T>* anext = 0);
	ThinList(const ThinList<T>& source);
	~ThinList();

	//Insert functions
	void PushToBack(const T& entry);
	void InsertAfter(const T& entry);

public:
	ThinList<T>* next;
	T object;
};

#include "ThinList.inl"
#endif
