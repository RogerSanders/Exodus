#ifndef __THINVECTOR_H__
#define __THINVECTOR_H__

template<class T, size_t S> struct ThinVector
{
public:
	//Constructors
	inline ThinVector();

	//Initialization functions
	inline void InitializeData();
	inline void CopyData(const T* sourceDataArray, size_t sourceDataArraySize);

public:
	size_t arraySize;
	T array[S];
};

#include "ThinVector.inl"
#endif
