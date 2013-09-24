//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
template<class T, size_t S> ThinVector<T,S>::ThinVector()
:arraySize(S)
{}

//----------------------------------------------------------------------------------------
//Initialization functions
//----------------------------------------------------------------------------------------
template<class T, size_t S> void ThinVector<T,S>::InitializeData()
{
	//Initialize all entries in the array to 0
	memset((void*)&array[0], 0, arraySize*sizeof(T));
}

//----------------------------------------------------------------------------------------
template<class T, size_t S> void ThinVector<T,S>::CopyData(const T* sourceDataArray, size_t sourceDataArraySize)
{
	if(sourceDataArraySize < arraySize)
	{
		memcpy((void*)&array[0], (const void*)sourceDataArray, sourceDataArraySize*sizeof(T));
		memset((void*)&array[sourceDataArraySize], 0, (arraySize-sourceDataArraySize)*sizeof(T));
	}
	else
	{
		memcpy((void*)&array[0], (const void*)sourceDataArray, arraySize*sizeof(T));
	}
}
