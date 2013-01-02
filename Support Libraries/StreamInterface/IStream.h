#ifndef __ISTREAM_H__
#define __ISTREAM_H__
#include "IStreamNonSeekable.h"
namespace Stream {

class IStream :public IStreamNonSeekable
{
public:
	//Stream position
	virtual SizeType Size() const = 0;
	virtual SizeType GetStreamPos() const = 0;
	virtual void SetStreamPos(SizeType position) = 0;
};

} //Close namespace Stream
#endif
