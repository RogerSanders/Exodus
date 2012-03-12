#ifndef __ZIPLOCALFILEHEADER_H__
#define __ZIPLOCALFILEHEADER_H__
#include "StreamInterface/StreamInterface.pkg"
#include <string>

struct ZIPChunk_LocalFileHeader
{
public:
	//Constructors
	inline ZIPChunk_LocalFileHeader();
	inline void Initialize();

	//Serialization functions
	inline bool LoadFromStream(Stream::IStream& stream);
	inline bool SaveToStream(Stream::IStream& stream) const;

public:
	static const unsigned int validSignature = 0x04034B50;
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
