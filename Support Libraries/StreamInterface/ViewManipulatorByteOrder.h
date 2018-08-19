#ifndef __VIEWMANIPULATORBYTEORDER_H__
#define __VIEWMANIPULATORBYTEORDER_H__
#include "IStream.h"
namespace Stream {
class ViewText;
class ViewBinary;

struct ByteOrder
{
public:
	// Constructors
	inline ByteOrder(IStream::ByteOrder byteOrder = IStream::ByteOrder::Platform);

	// Byte order functions
	inline IStream::ByteOrder GetByteOrder() const;
	inline void SetByteOrder(IStream::ByteOrder byteOrder);

	// Stream operators
	friend inline ViewBinary& operator>>(ViewBinary& stream, const ByteOrder& object);
	friend inline ViewBinary& operator<<(ViewBinary& stream, const ByteOrder& object);
	friend inline ViewText& operator>>(ViewText& stream, const ByteOrder& object);
	friend inline ViewText& operator<<(ViewText& stream, const ByteOrder& object);

private:
	IStream::ByteOrder _byteOrder;
};

} // Close namespace Stream
#include "ViewManipulatorByteOrder.inl"
#endif
