//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> TimedBufferAccessTarget<DataType, TimesliceType>::TimedBufferAccessTarget()
{}

//----------------------------------------------------------------------------------------
//Access functions
//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> TimedBufferAccessTarget<DataType, TimesliceType>& TimedBufferAccessTarget<DataType, TimesliceType>::AccessCommitted()
{
	target = TARGET_COMMITTED;
	return *this;
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> TimedBufferAccessTarget<DataType, TimesliceType>& TimedBufferAccessTarget<DataType, TimesliceType>::AccessCommitted(TimesliceType atime)
{
	target = TARGET_COMMITTED_TIME;
	time = atime;
	return *this;
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> TimedBufferAccessTarget<DataType, TimesliceType>& TimedBufferAccessTarget<DataType, TimesliceType>::AccessLatest()
{
	target = TARGET_LATEST;
	return *this;
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> TimedBufferAccessTarget<DataType, TimesliceType>& TimedBufferAccessTarget<DataType, TimesliceType>::AccessTime(TimesliceType atime)
{
	target = TARGET_TIME;
	time = atime;
	return *this;
}
