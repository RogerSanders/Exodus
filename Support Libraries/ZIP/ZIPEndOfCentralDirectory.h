#ifndef __ZIPENDOFCENTRALDIRECTORY_H__
#define __ZIPENDOFCENTRALDIRECTORY_H__
#include "StreamInterface/StreamInterface.pkg"
#include <string>

struct ZIPChunk_EndOfCentralDirectory
{
public:
	//Constructors
	inline ZIPChunk_EndOfCentralDirectory();
	inline void Initialize();

	//Serialization functions
	inline bool LoadFromStream(Stream::IStream& stream);
	inline bool SaveToStream(Stream::IStream& stream) const;

public:
	static const unsigned int validSignature = 0x06054B50;
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
