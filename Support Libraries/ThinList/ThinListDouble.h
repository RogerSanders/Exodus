#ifndef __THINLISTDOUBLE_H__
#define __THINLISTDOUBLE_H__

template<class T> struct ThinListDouble
{
public:
	//Constructors
	ThinListDouble();
	ThinListDouble(const T& aobject, ThinListDouble<T>* aprevious = 0, ThinListDouble<T>* anext = 0);
	ThinListDouble(const ThinListDouble<T>& source, ThinListDouble<T>* aprevious = 0);
	~ThinListDouble();

	//Insert functions
	void PushToBack(const T& entry);
	void InsertAfter(const T& entry);
	void InsertBefore(const T& entry);

public:
	ThinListDouble<T>* previous;
	ThinListDouble<T>* next;
	T object;
};

#include "ThinListDouble.inl"
#endif
