//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
ZIPChunkEndOfCentralDirectory::ZIPChunkEndOfCentralDirectory()
{
	Initialize();
}

//----------------------------------------------------------------------------------------------------------------------
void ZIPChunkEndOfCentralDirectory::Initialize()
{
	signature = ValidSignature;
	diskNumber = 0;
	diskNumberCentralDirectory = 0;
	centralDirectoryEntriesOnDisk = 0;
	centralDirectoryEntries = 0;
	centralDirectorySize = 0;
	centralDirectoryOffset = 0;
	commentLength = 0;
}

//----------------------------------------------------------------------------------------------------------------------
// Serialization functions
//----------------------------------------------------------------------------------------------------------------------
bool ZIPChunkEndOfCentralDirectory::LoadFromStream(Stream::IStream& stream)
{
	bool result = true;

	result &= stream.ReadData(signature);
	result &= stream.ReadData(diskNumber);
	result &= stream.ReadData(diskNumberCentralDirectory);
	result &= stream.ReadData(centralDirectoryEntriesOnDisk);
	result &= stream.ReadData(centralDirectoryEntries);
	result &= stream.ReadData(centralDirectorySize);
	result &= stream.ReadData(centralDirectoryOffset);
	result &= stream.ReadData(commentLength);
	result &= stream.ReadTextFixedLengthBufferAsASCII(commentLength, comment);

	return (result && (signature == ValidSignature));
}

//----------------------------------------------------------------------------------------------------------------------
bool ZIPChunkEndOfCentralDirectory::SaveToStream(Stream::IStream& stream) const
{
	bool result = true;

	commentLength = (unsigned short)comment.size();

	result &= stream.WriteData(signature);
	result &= stream.WriteData(diskNumber);
	result &= stream.WriteData(diskNumberCentralDirectory);
	result &= stream.WriteData(centralDirectoryEntriesOnDisk);
	result &= stream.WriteData(centralDirectoryEntries);
	result &= stream.WriteData(centralDirectorySize);
	result &= stream.WriteData(centralDirectoryOffset);
	result &= stream.WriteData(commentLength);
	result &= stream.WriteTextFixedLengthBufferAsASCII(commentLength, comment);

	return result;
}
