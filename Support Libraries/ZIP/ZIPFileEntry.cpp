#include "ZIPFileEntry.h"
#include "Deflate.h"
#include "WindowFunctions/WindowFunctions.pkg"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ZIPFileEntry::ZIPFileEntry()
:compressedDataWritten(false), data(0)
{}

//----------------------------------------------------------------------------------------
//Serialization functions
//These functions encode and decode the compressed data stream to and from a zip file.
//They do not compress or decompress data. Their job is to add/remove the zip header
//information surrounding the compressed data when the structure is being saved to or
//loaded from a zip file.
//----------------------------------------------------------------------------------------
bool ZIPFileEntry::LoadFromStream(Stream::IStream& source)
{
	//Load the local file header and compressed file data from the stream
	localFileHeader.LoadFromStream(source);
	data.Resize(localFileHeader.compressedSize);
	if(!source.ReadData(data.GetRawBuffer(), localFileHeader.compressedSize))
	{
		return false;
	}

	//Flag that the object has been populated with a compressed data stream
	compressedDataWritten = true;

	return true;
}

//----------------------------------------------------------------------------------------
bool ZIPFileEntry::SaveToStream(Stream::IStream& target) const
{
	//Only allow the data to be saved if it's been successfully compressed
	if(!compressedDataWritten)
	{
		return false;
	}

	//Save the local file header and compressed file data to the stream
	localFileHeader.SaveToStream(target);
	return target.WriteData(data.GetRawBuffer(), data.Size());
}

//----------------------------------------------------------------------------------------
//Data compression functions
//These functions perform the actual task of compressing/decompressing the data
//----------------------------------------------------------------------------------------
bool ZIPFileEntry::Compress(Stream::IStream& source, unsigned int inputCacheSize)
{
	//Calculate the uncompressed data size
	Stream::IStream::SizeType uncompressedDataSize = source.Size() - source.GetStreamPos();
	if(uncompressedDataSize < 0)
	{
		uncompressedDataSize = 0;
	}

	//Clean our data buffer. The buffer will automatically grow to a size large enough to
	//hold the compressed data.
	data.Resize(0);

	//Attempt to compress the file to our buffer using deflate compression
	unsigned int calculatedCRC;
	if(!Deflate::DeflateCompress(source, data, calculatedCRC, inputCacheSize, (unsigned int)data.Size()))
	{
		return false;
	}

	//Write the current system time as the modification time for the file
	SYSTEMTIME systemTime;
	FILETIME fileTime;
	WORD dosTime = 0;
	WORD dosDate = 0;
	GetLocalTime(&systemTime);
	SystemTimeToFileTime(&systemTime, &fileTime);
	FileTimeToDosDateTime(&fileTime, &dosDate, &dosTime);
	localFileHeader.modFileDate = dosDate;
	localFileHeader.modFileTime = dosTime;

	//Write header information for the data we just compressed
	//##TODO## Fix up the version to extract and compression method settings here
	localFileHeader.versionToExtract = 20;
	localFileHeader.compressionMethod = 8;	//Deflate compression
	localFileHeader.compressedSize = (unsigned int)data.Size();
	localFileHeader.uncompressedSize = (unsigned int)uncompressedDataSize;
	localFileHeader.crc32 = calculatedCRC;

	//Flag that the object has been populated with a compressed data stream
	compressedDataWritten = true;

	return true;
}

//----------------------------------------------------------------------------------------
bool ZIPFileEntry::Decompress(Stream::IStream& target, unsigned int outputCacheSize)
{
	//If the object hasn't been populated with a compressed data stream, abort with an
	//error.
	if(!compressedDataWritten)
	{
		return false;
	}

	//Attempt to decompress the file from our buffer using deflate compression
	unsigned int calculatedCRC;
	if(!Deflate::DeflateDecompress(data, target, calculatedCRC, (unsigned int)data.Size(), outputCacheSize))
	{
		return false;
	}

	//If the CRC of the decompressed data doesn't match the CRC reported in the header,
	//return an error.
	if(calculatedCRC != localFileHeader.crc32)
	{
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
//File name functions
//----------------------------------------------------------------------------------------
std::wstring ZIPFileEntry::GetFileName() const
{
	return localFileHeader.fileName;
}

//----------------------------------------------------------------------------------------
void ZIPFileEntry::SetFileName(const std::wstring& aname)
{
	localFileHeader.fileName = aname;
}

//----------------------------------------------------------------------------------------
//File header functions
//----------------------------------------------------------------------------------------
ZIPChunk_CentralFileHeader ZIPFileEntry::GetCentralDirectoryFileHeader() const
{
	return ZIPChunk_CentralFileHeader(localFileHeader);
}
