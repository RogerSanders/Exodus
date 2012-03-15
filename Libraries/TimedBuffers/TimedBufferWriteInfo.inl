//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> TimedBufferWriteInfo<DataType, TimesliceType>::TimedBufferWriteInfo()
{}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> TimedBufferWriteInfo<DataType, TimesliceType>::TimedBufferWriteInfo(const DataType& defaultValue)
:newValue(defaultValue)
{}
