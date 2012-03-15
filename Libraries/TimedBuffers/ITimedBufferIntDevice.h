#ifndef __ITIMEDBUFFERINTDEVICE_H__
#define __ITIMEDBUFFERINTDEVICE_H__
class ITimedBufferInt;

class ITimedBufferIntDevice
{
public:
	//Make sure the object can't be deleted from this base
	protected: virtual ~ITimedBufferIntDevice() = 0 {} public:

	//Buffer functions
	virtual ITimedBufferInt* GetTimedBuffer() = 0;
};

#endif
