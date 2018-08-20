#ifndef __ZIPARCHIVE_H__
#define __ZIPARCHIVE_H__
#include <string>
#include <list>
#include "StreamInterface/StreamInterface.pkg"
#include "ZIPCentralFileHeader.h"
#include "ZIPEndOfCentralDirectory.h"
#include "ZIPLocalFileHeader.h"
#include "ZIPFileEntry.h"

//##TODO## Create a separate compression library, which just contains the algorithms to
// compress or decompress using the various compression methods. Our ZIP library should
// just reference this compression library in order to implement its algorithms. After this
// change, our ZIP library should just implement the ZIP file structure itself.
//##TODO## Implement support for:
// 1. Stored (uncompressed)
// 2. LZMA (free, unrestrictive license, LZMA SDK from 7-zip)
// 3. PPMd (free, unrestrictive license, LZMA SDK from 7-zip)
// 4. bzip2 (free, apparently unrestrictive license, bzip.org)
// 5. WavPack? (free, unrestrictive license)

//##TODO## Non-zip related formats:
// 6. FLAC (free, unrestrictive license, http://flac.sourceforge.net)
// 7. LZMA2 (free, unrestrictive license, LZMA SDK from 7-zip)

class ZIPArchive
{
public:
	// Enumerations
	enum class CompressionMethod
	{
		Store       = 0,
		Shrunk      = 1,
		Imploded    = 6,
		Deflate     = 8,
		BZip2       = 12,
		Lzma        = 14,
		WavPack     = 97,
		PPMd        = 98
	};

	// 0 - The file is stored (no compression)
	// 1 - The file is Shrunk
	// 2 - The file is Reduced with compression factor 1
	// 3 - The file is Reduced with compression factor 2
	// 4 - The file is Reduced with compression factor 3
	// 5 - The file is Reduced with compression factor 4
	// 6 - The file is Imploded
	// 7 - Reserved for Tokenizing compression algorithm
	// 8 - The file is Deflated
	// 9 - Enhanced Deflating using Deflate64(tm)
	// 10 - PKWARE Data Compression Library Imploding (old IBM TERSE)
	// 11 - Reserved by PKWARE
	// 12 - File is compressed using BZIP2 algorithm
	// 13 - Reserved by PKWARE
	// 14 - LZMA (EFS)
	// 15 - Reserved by PKWARE
	// 16 - Reserved by PKWARE
	// 17 - Reserved by PKWARE
	// 18 - File is compressed using IBM TERSE (new)
	// 19 - IBM LZ77 z Architecture (PFS)
	// 97 - WavPack compressed data
	// 98 - PPMd version I, Rev 1

	// Constructors
	ZIPArchive(CompressionMethod compressionMethod = CompressionMethod::Deflate);

	// Serialization functions
	bool LoadFromStream(Stream::IStream& source);
	bool SaveToStream(Stream::IStream& target);

	// File entry functions
	unsigned int GetFileEntryCount() const;
	void AddFileEntry(const ZIPFileEntry& fileEntry);
	ZIPFileEntry* GetFileEntry(unsigned int fileNumber);
	ZIPFileEntry* GetFileEntry(const std::wstring& fileName);

private:
	std::list<ZIPFileEntry> _fileList;
	std::list<ZIPChunkCentralFileHeader> _centralDirectory;
	ZIPChunkEndOfCentralDirectory _endOfCentralDirectoryHeader;
};

#endif
