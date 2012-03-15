//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> TimedBufferAdvanceSession<DataType, TimesliceType>::TimedBufferAdvanceSession()
:initialized(false)
{}

//----------------------------------------------------------------------------------------
//Session functions
//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> void TimedBufferAdvanceSession<DataType, TimesliceType>::Reset()
{
	initialized = false;
}
