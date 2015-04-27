//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> TimedBufferAdvanceSession<DataType, TimesliceType>::TimedBufferAdvanceSession(const DataType& writeDataDefaultValue)
:retrieveWriteInfo(false), writeInfo(writeDataDefaultValue)
{}
