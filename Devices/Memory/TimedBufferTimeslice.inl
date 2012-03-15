//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> TimedBufferTimeslice<DataType, TimesliceType>::TimedBufferTimeslice(typename RandomTimeAccessBuffer<DataType, TimesliceType>::Timeslice atimeslice)
:timeslice(atimeslice)
{}
