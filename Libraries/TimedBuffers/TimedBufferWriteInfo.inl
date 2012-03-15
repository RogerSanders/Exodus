//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> TimedBufferWriteInfo<DataType, TimesliceType>::TimedBufferWriteInfo(const DataType& defaultValue)
:exists(false), newValue(defaultValue)
{}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> TimedBufferWriteInfo<DataType, TimesliceType>::TimedBufferWriteInfo(bool aexists, unsigned int awriteAddress, TimesliceType awriteTime, DataType anewValue)
:exists(aexists), writeAddress(awriteAddress), writeTime(awriteTime), newValue(anewValue)
{}
