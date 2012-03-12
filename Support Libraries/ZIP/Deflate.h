#ifndef __DEFLATE_H__
#define __DEFLATE_H__
#include "StreamInterface/StreamInterface.pkg"
namespace Deflate {

bool DeflateCompress(Stream::IStream& source, Stream::IStream& target, unsigned int& calculatedCRC, unsigned int inputCacheSize = 0, unsigned int outputCacheSize = 0);
bool DeflateDecompress(Stream::IStream& source, Stream::IStream& target, unsigned int& calculatedCRC, unsigned int inputCacheSize = 0, unsigned int outputCacheSize = 0);

} //Close namespace Deflate
#endif
