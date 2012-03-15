#ifndef __ITIMEDBUFFERACCESSTARGET_H__
#define __ITIMEDBUFFERACCESSTARGET_H__

template<class DataType, class TimesliceType> struct TimedBufferAccessTarget
{
public:
	//Enumerations
	enum Target
	{
		TARGET_COMMITTED,
		TARGET_COMMITTED_TIME,
		TARGET_LATEST,
		TARGET_TIME
	};

public:
	//Constructors
	inline TimedBufferAccessTarget();

	//Access functions
	inline TimedBufferAccessTarget& AccessCommitted();
	inline TimedBufferAccessTarget& AccessCommitted(TimesliceType atime);
	inline TimedBufferAccessTarget& AccessLatest();
	inline TimedBufferAccessTarget& AccessTime(TimesliceType atime);

public:
	//Data members
	Target target;
	TimesliceType time;
};

#include "TimedBufferAccessTarget.inl"
#endif
