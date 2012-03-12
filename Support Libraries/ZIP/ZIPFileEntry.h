#ifndef __ZIPFILEENTRY_H__
#define __ZIPFILEENTRY_H__
#include <vector>
#include "ZIPLocalFileHeader.h"
#include "ZIPCentralFileHeader.h"
#include "Stream/Stream.pkg"

class ZIPFileEntry
{
public:
	//Constructors
	ZIPFileEntry();

	//Serialization functions
	bool LoadFromStream(Stream::IStream& source);
	bool SaveToStream(Stream::IStream& target) const;

	//Data compression functions
	//##TODO## Implement alternate methods of doing this. We want to avoid the need to
	//fully store the compressed/decompressed data stream in memory. We need to be able to
	//go directly from source file to target file. In the case of compression, consider
	//setting up a reference to a source stream, which is then only accessed and
	//compressed during the SaveToStream() function call, where the target stream is
	//actually the target stream for the zip archive, and the source stream is the source
	//for the uncompressed data.
	//##TODO## Implement a compressionMethod flag to the Compress function, and modify the
	//function to support multiple compression methods.
	bool Compress(Stream::IStream& source, unsigned int inputCacheSize = 0);
	//##TODO## Implement support for multiple compression methods.
	bool Decompress(Stream::IStream& target, unsigned int outputCacheSize = 0);

	//File name functions
	std::wstring GetFileName() const;
	void SetFileName(const std::wstring& aname);

	//File header functions
	ZIPChunk_CentralFileHeader GetCentralDirectoryFileHeader() const;

private:
	bool compressedDataWritten;
	Stream::Buffer data;
	ZIPChunk_LocalFileHeader localFileHeader;
};

#endif
