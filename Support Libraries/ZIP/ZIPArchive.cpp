#include "ZIPArchive.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
ZIPArchive::ZIPArchive(CompressionMethod compressionMethod)
:_compressionMethod(compressionMethod)
{ }

//----------------------------------------------------------------------------------------------------------------------
// Serialization functions
//----------------------------------------------------------------------------------------------------------------------
bool ZIPArchive::LoadFromStream(Stream::IStream& source)
{
	// Parse the ZIP file, and extract all data chunks
	bool done = false;
	while (!done)
	{
		// Take a peek at the chunk ID for the next file chunk
		unsigned int chunkID;
		if (!source.ReadData(chunkID))
		{
			return false;
		}
		// Seek back to the read position for the start of the chunk
		source.SetStreamPos(source.GetStreamPos() - sizeof(chunkID));

		// Determine the type of file chunk we're looking at
		switch (chunkID)
		{
		// Try and load a local file header chunk
		case ZIPChunkLocalFileHeader::ValidSignature:{
			ZIPFileEntry fileEntry;
			if (!fileEntry.LoadFromStream(source))
			{
				return false;
			}
			_fileList.push_back(fileEntry);
			break;}
		// Try and load a central file header chunk
		case ZIPChunkCentralFileHeader::ValidSignature:{
			ZIPChunkCentralFileHeader header;
			if (!header.LoadFromStream(source))
			{
				return false;
			}
			_centralDirectory.push_back(header);
			break;}
		// Try and load an end of central directory chunk
		case ZIPChunkEndOfCentralDirectory::ValidSignature:{
			if (!_endOfCentralDirectoryHeader.LoadFromStream(source))
			{
				return false;
			}
			// If we've just loaded the end of central directory chunk, we've finished
			// loading the zip file.
			done = true;
			break;}
		// If we've encountered an unknown file chunk, abort any further processing.
		default:
			return false;
		}
	}
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ZIPArchive::SaveToStream(Stream::IStream& target)
{
	//##TODO## Implement the compressionMethod flag

	// Save individual file records
	for (std::list<ZIPFileEntry>::iterator i = _fileList.begin(); i != _fileList.end(); ++i)
	{
		ZIPChunkCentralFileHeader centralDirectoryHeader = i->GetCentralDirectoryFileHeader();
		centralDirectoryHeader.relativeOffsetOfLocalHeader = (unsigned int)target.GetStreamPos();
		_centralDirectory.push_back(centralDirectoryHeader);
		if (!i->SaveToStream(target))
		{
			return false;
		}
	}

	// Fill out the end of central directory header
	_endOfCentralDirectoryHeader.centralDirectoryEntriesOnDisk = (unsigned short)_fileList.size();
	_endOfCentralDirectoryHeader.centralDirectoryEntries = (unsigned short)_fileList.size();
	_endOfCentralDirectoryHeader.centralDirectoryOffset = (unsigned int)target.GetStreamPos();

	// Save the central directory to the stream
	for (std::list<ZIPChunkCentralFileHeader>::iterator i = _centralDirectory.begin(); i != _centralDirectory.end(); ++i)
	{
		if (!i->SaveToStream(target))
		{
			return false;
		}
		_endOfCentralDirectoryHeader.centralDirectorySize += i->GetSize();
	}
	if (!_endOfCentralDirectoryHeader.SaveToStream(target))
	{
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
// File entry functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int ZIPArchive::GetFileEntryCount() const
{
	return (unsigned int)_fileList.size();
}

//----------------------------------------------------------------------------------------------------------------------
void ZIPArchive::AddFileEntry(const ZIPFileEntry& fileEntry)
{
	_fileList.push_back(fileEntry);
}

//----------------------------------------------------------------------------------------------------------------------
ZIPFileEntry* ZIPArchive::GetFileEntry(unsigned int fileNumber)
{
	if (fileNumber >= GetFileEntryCount())
	{
		return 0;
	}

	std::list<ZIPFileEntry>::iterator fileEntry = _fileList.begin();
	for (unsigned int i = 0; i < fileNumber; ++i)
	{
		++fileEntry;
	}

	return &(*fileEntry);
}

//----------------------------------------------------------------------------------------------------------------------
ZIPFileEntry* ZIPArchive::GetFileEntry(const std::wstring& fileName)
{
	for (std::list<ZIPFileEntry>::iterator i = _fileList.begin(); i != _fileList.end(); ++i)
	{
		if (i->GetFileName() == fileName)
		{
			return &(*i);
		}
	}
	return 0;
}
