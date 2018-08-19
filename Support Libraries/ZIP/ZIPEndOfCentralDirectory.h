#ifndef __ZIPENDOFCENTRALDIRECTORY_H__
#define __ZIPENDOFCENTRALDIRECTORY_H__
#include "StreamInterface/StreamInterface.pkg"
#include <string>

struct ZIPChunkEndOfCentralDirectory
{
public:
	// Constructors
	inline ZIPChunkEndOfCentralDirectory();
	inline void Initialize();

	// Serialization functions
	inline bool LoadFromStream(Stream::IStream& stream);
	inline bool SaveToStream(Stream::IStream& stream) const;

public:
	// Constants
	static const unsigned int ValidSignature = 0x06054B50;

public:
	unsigned int signature;
	unsigned short diskNumber;
	unsigned short diskNumberCentralDirectory;
	unsigned short centralDirectoryEntriesOnDisk;
	unsigned short centralDirectoryEntries;
	unsigned int centralDirectorySize;
	unsigned int centralDirectoryOffset;
	mutable unsigned short commentLength;
	std::wstring comment;
};

#include "ZIPEndOfCentralDirectory.inl"
#endif
