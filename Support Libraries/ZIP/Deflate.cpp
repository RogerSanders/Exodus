#include "Deflate.h"
#include <zlib.h>
namespace Deflate {

//----------------------------------------------------------------------------------------
bool DeflateCompress(Stream::IStream& source, Stream::IStream& target, unsigned int& calculatedCRC, unsigned int inputCacheSize, unsigned int outputCacheSize)
{
	//If no input cache size was specified, set the input cache size to 1MB, and create
	//our input buffer.
	if(inputCacheSize <= 0)
	{
		inputCacheSize = (1024*1024);
	}
	std::vector<unsigned char> inputCache(inputCacheSize);

	//If no output cache size was specified, set the output cache size to 1MB, and create
	//our output buffer.
	if(outputCacheSize <= 0)
	{
		outputCacheSize = (1024*1024);
	}
	std::vector<unsigned char> outputCache(outputCacheSize);

	//Initialize zlib for compression
	z_stream strm;
	//We only set next_in to NULL here because in previous versions of zlib, this
	//parameter was used to pass the location of an application-defined buffer for zlib to
	//store internal data used during the compression process. If the parameter was NULL,
	//zlib would allocate its own internal buffer. In newer versions of zlib, an internal
	//buffer is always used, and information about this old obscure feature has been
	//removed from the documentation, however the documentation still says we need to
	//initialize next_in before calling delateInit2, although now it doesn't say how to
	//initialize it or what it's used for. I've verified this parameter isn't even looked
	//at in the current implementation, but we initialize it to NULL here just to ensure
	//this code works correctly, even on older versions of zlib, and just to ensure we
	//have explicitly initialized it, as the documentation still tells us to.
	strm.next_in = Z_NULL;
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	int deflateInitResult;
	//The negative parameter for the window size tells zlib not to add its own header or
	//footer to the compressed data stream. There must be no zlib header on the data in
	//order for the compressed stream to be used in a zip file.
	deflateInitResult = deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, -15, 9, Z_DEFAULT_STRATEGY);
	if(deflateInitResult != Z_OK)
	{
		return false;
	}

	//Set the initial value for the decompressed data running CRC
	uLong crc = crc32(0, Z_NULL, 0);

	//Compress the data
	strm.avail_in = 0;
	strm.avail_out = 0;
	int deflateResult = Z_OK;
	while(deflateResult == Z_OK)
	{
		//Reset the output buffer if required
		if(strm.avail_out <= 0)
		{
			strm.avail_out = outputCacheSize;
			strm.next_out = &outputCache[0];
		}

		//Read more data into the source buffer if required
		int flushParam = Z_NO_FLUSH;
		if(strm.avail_in <= 0)
		{
			//Grab enough data from the source to either fill our buffer, or reach the end
			//of the stream.
			Stream::IStream::SizeType remainingSourceData = (source.Size() - source.GetStreamPos());
			if(remainingSourceData < 0)
			{
				remainingSourceData = 0;
			}
			strm.avail_in = (uInt)inputCache.size();
			if((uInt)remainingSourceData <= strm.avail_in)
			{
				strm.avail_in = (uInt)remainingSourceData;
				flushParam = Z_FINISH;
			}
			if(!source.ReadData(&inputCache[0], strm.avail_in))
			{
				//If an error occurred while reading from the source stream, clean up and
				//return an error.
				deflateEnd(&strm);
				return false;
			}
			strm.next_in = &inputCache[0];

			//Update the running CRC for the new block of decompressed data
			crc = crc32(crc, strm.next_in, strm.avail_in);
		}

		//Compress the next block of source data
		deflateResult = deflate(&strm, flushParam);

		//If we've reached the end of the input stream, or the output buffer is full,
		//flush the output buffer to the target.
		if((deflateResult == Z_STREAM_END) || ((deflateResult == Z_OK) && (strm.avail_out <= 0)))
		{
			//Calculate the amount of data in the output buffer
			unsigned int usedBufferSize = (outputCacheSize - strm.avail_out);

			//Write the used portion of the output buffer to the stream
			if(!target.WriteData(&outputCache[0], usedBufferSize))
			{
				//If an error occurred while writing to the target stream, clean up and
				//return an error.
				deflateEnd(&strm);
				return false;
			}

			//Invalidate any remaining space in the output cache. We want the cache to be
			//reset now that we've written its contents.
			strm.avail_out = 0;
		}
	}

	//If an error occurred during compression, clean up and return an error.
	if(deflateResult != Z_STREAM_END)
	{
		deflateEnd(&strm);
		return false;
	}

	//Clean up zlib
	if(deflateEnd(&strm) != Z_OK)
	{
		return false;
	}

	//Set the calculated CRC value parameter
	calculatedCRC = (unsigned int)crc;

	return true;
}

//----------------------------------------------------------------------------------------
bool DeflateDecompress(Stream::IStream& source, Stream::IStream& target, unsigned int& calculatedCRC, unsigned int inputCacheSize, unsigned int outputCacheSize)
{
	//If no input cache size was specified, set the input cache size to 1MB, and create
	//our input buffer.
	if(inputCacheSize <= 0)
	{
		inputCacheSize = (1024*1024);
	}
	std::vector<unsigned char> inputCache(inputCacheSize);

	//If no output cache size was specified, set the output cache size to 1MB, and create
	//our output buffer.
	if(outputCacheSize <= 0)
	{
		outputCacheSize = (1024*1024);
	}
	std::vector<unsigned char> outputCache(outputCacheSize);

	//Initialize zlib for decompression
	z_stream strm;
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
	int inflateInitResult;
	//The negative parameter for the window size tells zlib not to attempt to read zlib
	//or gzip header or footer data from the compressed stream. This data is not present
	//in a zip file, as zip has its own header structures. We only want to process the
	//raw deflate stream.
	inflateInitResult = inflateInit2(&strm, -15);
	if(inflateInitResult != Z_OK)
	{
		return false;
	}

	//Set the initial value for the decompressed data running CRC
	uLong crc = crc32(0, Z_NULL, 0);

	//Decompress the data
	strm.avail_in = 0;
	strm.avail_out = 0;
	int inflateResult = Z_OK;
	while(inflateResult == Z_OK)
	{
		//Reset the output buffer if required
		if(strm.avail_out <= 0)
		{
			strm.avail_out = outputCacheSize;
			strm.next_out = &outputCache[0];
		}

		//Read more data into the source buffer if required
		if(strm.avail_in <= 0)
		{
			//Grab enough data from the source to either fill our buffer, or reach the end
			//of the stream.
			Stream::IStream::SizeType remainingSourceData = (source.Size() - source.GetStreamPos());
			if(remainingSourceData < 0)
			{
				remainingSourceData = 0;
			}
			strm.avail_in = (uInt)inputCache.size();
			if((uInt)remainingSourceData <= strm.avail_in)
			{
				strm.avail_in = (uInt)remainingSourceData;
			}
			if(!source.ReadData(&inputCache[0], strm.avail_in))
			{
				//If an error occurred while reading from the source stream, clean up and
				//return an error.
				inflateEnd(&strm);
				return false;
			}
			strm.next_in = &inputCache[0];
		}

		//Decompress the next data chunk
		inflateResult = inflate(&strm, Z_NO_FLUSH);

		//If we've reached the end of the input stream, or the output buffer is full,
		//flush the output buffer to the target.
		if((inflateResult == Z_STREAM_END) || ((inflateResult == Z_OK) && (strm.avail_out <= 0)))
		{
			//Calculate the amount of data in the output buffer
			unsigned int usedBufferSize = (outputCacheSize - strm.avail_out);

			//Update the running CRC for the new block of decompressed data
			crc = crc32(crc, &outputCache[0], usedBufferSize);

			//Write the used portion of the output buffer to the stream
			if(!target.WriteData(&outputCache[0], usedBufferSize))
			{
				//If an error occurred while writing to the target stream, clean up and
				//return an error.
				inflateEnd(&strm);
				return false;
			}

			//Invalidate any remaining space in the output cache. We want the cache to be
			//reset now that we've written its contents.
			strm.avail_out = 0;
		}
	}

	//If an error occurred during decompression, clean up and return an error.
	if(inflateResult != Z_STREAM_END)
	{
		inflateEnd(&strm);
		return false;
	}

	//Clean up zlib
	if(inflateEnd(&strm) != Z_OK)
	{
		return false;
	}

	//Set the calculated CRC value parameter
	calculatedCRC = (unsigned int)crc;

	return true;
}

} //Close namespace Deflate
