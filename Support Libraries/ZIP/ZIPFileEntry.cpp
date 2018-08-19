#include "ZIPFileEntry.h"
#include "Deflate.h"
#include "WindowsSupport/WindowsSupport.pkg"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ZIPFileEntry::ZIPFileEntry()
:_compressedDataWritten(false), _data(0)
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
	_localFileHeader.LoadFromStream(source);
	_data.Resize(_localFileHeader.compressedSize);
	if(!source.ReadData(_data.GetRawBuffer(), _localFileHeader.compressedSize))
	{
		return false;
	}

	//Flag that the object has been populated with a compressed data stream
	_compressedDataWritten = true;

	return true;
}

//----------------------------------------------------------------------------------------
bool ZIPFileEntry::SaveToStream(Stream::IStream& target) const
{
	//Only allow the data to be saved if it's been successfully compressed
	if(!_compressedDataWritten)
	{
		return false;
	}

	//Save the local file header and compressed file data to the stream
	_localFileHeader.SaveToStream(target);
	return target.WriteData(_data.GetRawBuffer(), _data.Size());
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
	_data.Resize(0);

	//Attempt to compress the file to our buffer using deflate compression
	unsigned int calculatedCRC;
	if(!Deflate::DeflateCompress(source, _data, calculatedCRC, inputCacheSize, (unsigned int)_data.Size()))
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
	_localFileHeader.modFileDate = dosDate;
	_localFileHeader.modFileTime = dosTime;

	//Write header information for the data we just compressed
	//##TODO## Fix up the version to extract and compression method settings here
	_localFileHeader.versionToExtract = 20;
	_localFileHeader.compressionMethod = 8;	//Deflate compression
	_localFileHeader.compressedSize = (unsigned int)_data.Size();
	_localFileHeader.uncompressedSize = (unsigned int)uncompressedDataSize;
	_localFileHeader.crc32 = calculatedCRC;

	//Flag that the object has been populated with a compressed data stream
	_compressedDataWritten = true;

	return true;
}

//----------------------------------------------------------------------------------------
bool ZIPFileEntry::Decompress(Stream::IStream& target, unsigned int outputCacheSize)
{
	//If the object hasn't been populated with a compressed data stream, abort with an
	//error.
	if(!_compressedDataWritten)
	{
		return false;
	}

	//Attempt to decompress the file from our buffer using deflate compression
	unsigned int calculatedCRC;
	if(!Deflate::DeflateDecompress(_data, target, calculatedCRC, (unsigned int)_data.Size(), outputCacheSize))
	{
		return false;
	}

	//If the CRC of the decompressed data doesn't match the CRC reported in the header,
	//return an error.
	if(calculatedCRC != _localFileHeader.crc32)
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
	return _localFileHeader.fileName;
}

//----------------------------------------------------------------------------------------
void ZIPFileEntry::SetFileName(const std::wstring& name)
{
	_localFileHeader.fileName = name;
}

//----------------------------------------------------------------------------------------
//File header functions
//----------------------------------------------------------------------------------------
ZIPChunkCentralFileHeader ZIPFileEntry::GetCentralDirectoryFileHeader() const
{
	return ZIPChunkCentralFileHeader(_localFileHeader);
}
