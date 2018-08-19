#ifndef __ZIPCENTRALFILEHEADER_H__
#define __ZIPCENTRALFILEHEADER_H__
#include "StreamInterface/StreamInterface.pkg"
#include <string>
#include "ZIPLocalFileHeader.h"

struct ZIPChunkCentralFileHeader
{
public:
	// Constructors
	inline ZIPChunkCentralFileHeader();
	inline ZIPChunkCentralFileHeader(const ZIPChunkLocalFileHeader& localFileHeader);
	inline void Initialize();

	// Serialization functions
	inline bool LoadFromStream(Stream::IStream& stream);
	inline bool SaveToStream(Stream::IStream& stream) const;

	// Size functions
	inline unsigned int GetSize();

public:
	// Constants
	static const unsigned int ValidSignature = 0x02014B50;

public:
	unsigned int signature;
	unsigned short versionMadeBy;
	unsigned short versionNeededToExtract;
	unsigned short bitFlags;
	unsigned short compressionMethod;
	unsigned short lastModFileTime;
	unsigned short lastModFileDate;
	unsigned int crc32;
	unsigned int compressedSize;
	unsigned int uncompressedSize;
	mutable unsigned short fileNameLength;
	mutable unsigned short extraFieldLength;
	mutable unsigned short fileCommentLength;
	unsigned short diskNumberStart;
	unsigned short internalFileAttributes;
	unsigned int externalFileAttributes;
	unsigned int relativeOffsetOfLocalHeader;
	std::wstring fileName;
	std::wstring extraField;
	std::wstring fileComment;
};

#include "ZIPCentralFileHeader.inl"
#endif
