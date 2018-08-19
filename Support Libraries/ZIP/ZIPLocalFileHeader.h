#ifndef __ZIPLOCALFILEHEADER_H__
#define __ZIPLOCALFILEHEADER_H__
#include "StreamInterface/StreamInterface.pkg"
#include <string>

struct ZIPChunkLocalFileHeader
{
public:
	//Constructors
	inline ZIPChunkLocalFileHeader();
	inline void Initialize();

	//Serialization functions
	inline bool LoadFromStream(Stream::IStream& stream);
	inline bool SaveToStream(Stream::IStream& stream) const;

public:
	//Constants
	static const unsigned int ValidSignature = 0x04034B50;

public:
	unsigned int signature;
	unsigned short versionToExtract;
	unsigned short bitFlags;
	unsigned short compressionMethod;
	unsigned short modFileTime;
	unsigned short modFileDate;
	unsigned int crc32;
	unsigned int compressedSize;
	unsigned int uncompressedSize;
	mutable unsigned short fileNameLength;
	mutable unsigned short extraFieldLength;
	std::wstring fileName;
	std::wstring extraField;
};

#include "ZIPLocalFileHeader.inl"
#endif
