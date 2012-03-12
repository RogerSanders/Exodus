#include "ZIPArchive.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ZIPArchive::ZIPArchive(CompressionMethod acompressionmMethod)
:compressionMethod(acompressionmMethod)
{}

//----------------------------------------------------------------------------------------
//Serialization functions
//----------------------------------------------------------------------------------------
bool ZIPArchive::LoadFromStream(Stream::IStream& source)
{
	//Parse the ZIP file, and extract all data chunks
	bool done = false;
	while(!done)
	{
		//Take a peek at the chunk ID for the next file chunk
		unsigned int chunkID;
		if(!source.ReadData(chunkID))
		{
			return false;
		}
		//Seek back to the read position for the start of the chunk
		source.SetStreamPos(source.GetStreamPos() - sizeof(chunkID));

		//Determine the type of file chunk we're looking at
		switch(chunkID)
		{
		//Try and load a local file header chunk
		case ZIPChunk_LocalFileHeader::validSignature:{
			ZIPFileEntry fileEntry;
			if(!fileEntry.LoadFromStream(source))
			{
				return false;
			}
			fileList.push_back(fileEntry);
			break;}
		//Try and load a central file header chunk
		case ZIPChunk_CentralFileHeader::validSignature:{
			ZIPChunk_CentralFileHeader header;
			if(!header.LoadFromStream(source))
			{
				return false;
			}
			centralDirectory.push_back(header);
			break;}
		//Try and load an end of central directory chunk
		case ZIPChunk_EndOfCentralDirectory::validSignature:{
			if(!endOfCentralDirectoryHeader.LoadFromStream(source))
			{
				return false;
			}
			//If we've just loaded the end of central directory chunk, we've finished
			//loading the zip file.
			done = true;
			break;}
		//If we've encountered an unknown file chunk, abort any further processing.
		default:{
			return false;
			break;}
		}
	}
	return true;
}

//----------------------------------------------------------------------------------------
bool ZIPArchive::SaveToStream(Stream::IStream& target)
{
	//##TODO## Implement the compressionMethod flag

	//Save individual file records
	for(std::list<ZIPFileEntry>::iterator i = fileList.begin(); i != fileList.end(); ++i)
	{
		ZIPChunk_CentralFileHeader centralDirectoryHeader = i->GetCentralDirectoryFileHeader();
		centralDirectoryHeader.relativeOffsetOfLocalHeader = (unsigned int)target.GetStreamPos();
		centralDirectory.push_back(centralDirectoryHeader);
		if(!i->SaveToStream(target))
		{
			return false;
		}
	}

	//Fill out the end of central directory header
	endOfCentralDirectoryHeader.centralDirectoryEntriesOnDisk = (unsigned short)fileList.size();
	endOfCentralDirectoryHeader.centralDirectoryEntries	 = (unsigned short)fileList.size();
	endOfCentralDirectoryHeader.centralDirectoryOffset = (unsigned int)target.GetStreamPos();

	//Save the central directory to the stream
	for(std::list<ZIPChunk_CentralFileHeader>::iterator i = centralDirectory.begin(); i != centralDirectory.end(); ++i)
	{
		if(!i->SaveToStream(target))
		{
			return false;
		}
		endOfCentralDirectoryHeader.centralDirectorySize += i->GetSize();
	}
	if(!endOfCentralDirectoryHeader.SaveToStream(target))
	{
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
//File entry functions
//----------------------------------------------------------------------------------------
unsigned int ZIPArchive::GetFileEntryCount() const
{
	return (unsigned int)fileList.size();
}

//----------------------------------------------------------------------------------------
void ZIPArchive::AddFileEntry(const ZIPFileEntry& fileEntry)
{
	fileList.push_back(fileEntry);
}

//----------------------------------------------------------------------------------------
ZIPFileEntry* ZIPArchive::GetFileEntry(unsigned int fileNumber)
{
	if(fileNumber >= GetFileEntryCount())
	{
		return 0;
	}

	std::list<ZIPFileEntry>::iterator fileEntry = fileList.begin();
	for(unsigned int i = 0; i < fileNumber; ++i)
	{
		++fileEntry;
	}

	return &(*fileEntry);
}

//----------------------------------------------------------------------------------------
ZIPFileEntry* ZIPArchive::GetFileEntry(const std::wstring& fileName)
{
	for(std::list<ZIPFileEntry>::iterator i = fileList.begin(); i != fileList.end(); ++i)
	{
		if(i->GetFileName() == fileName)
		{
			return &(*i);
		}
	}
	return 0;
}
