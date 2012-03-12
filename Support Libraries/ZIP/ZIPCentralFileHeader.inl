//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ZIPChunk_CentralFileHeader::ZIPChunk_CentralFileHeader()
{
	Initialize();
}

//----------------------------------------------------------------------------------------
ZIPChunk_CentralFileHeader::ZIPChunk_CentralFileHeader(const ZIPChunk_LocalFileHeader& localFileHeader)
{
	Initialize();
	versionMadeBy = localFileHeader.versionToExtract;
	versionNeededToExtract = localFileHeader.versionToExtract;
	bitFlags = localFileHeader.bitFlags;
	compressionMethod = localFileHeader.compressionMethod;
	lastModFileTime = localFileHeader.modFileTime;
	lastModFileDate = localFileHeader.modFileDate;
	crc32 = localFileHeader.crc32;
	compressedSize = localFileHeader.compressedSize;
	uncompressedSize = localFileHeader.uncompressedSize;
	fileName = localFileHeader.fileName;
	extraField = localFileHeader.extraField;
	fileNameLength = (unsigned short)fileName.size();
	extraFieldLength = (unsigned short)extraField.size();
}

//----------------------------------------------------------------------------------------
void ZIPChunk_CentralFileHeader::Initialize()
{
	signature = validSignature;
	versionMadeBy = 0;
	versionNeededToExtract = 0;
	bitFlags = 0;
	compressionMethod = 0;
	lastModFileTime = 0;
	lastModFileDate = 0;
	crc32 = 0;
	compressedSize = 0;
	uncompressedSize = 0;
	fileNameLength = 0;
	extraFieldLength = 0;
	fileCommentLength = 0;
	diskNumberStart = 0;
	internalFileAttributes = 0;
	externalFileAttributes = 0;
	relativeOffsetOfLocalHeader = 0;
	fileName.clear();
	extraField.clear();
	fileComment.clear();
}

//----------------------------------------------------------------------------------------
//Serialization functions
//----------------------------------------------------------------------------------------
bool ZIPChunk_CentralFileHeader::LoadFromStream(Stream::IStream& stream)
{
	bool result = true;

	result &= stream.ReadData(signature);
	result &= stream.ReadData(versionMadeBy);
	result &= stream.ReadData(versionNeededToExtract);
	result &= stream.ReadData(bitFlags);
	result &= stream.ReadData(compressionMethod);
	result &= stream.ReadData(lastModFileTime);
	result &= stream.ReadData(lastModFileDate);
	result &= stream.ReadData(crc32);
	result &= stream.ReadData(compressedSize);
	result &= stream.ReadData(uncompressedSize);
	result &= stream.ReadData(fileNameLength);
	result &= stream.ReadData(extraFieldLength);
	result &= stream.ReadData(fileCommentLength);
	result &= stream.ReadData(diskNumberStart);
	result &= stream.ReadData(internalFileAttributes);
	result &= stream.ReadData(externalFileAttributes);
	result &= stream.ReadData(relativeOffsetOfLocalHeader);
	result &= stream.ReadTextFixedLengthBufferAsASCII(fileNameLength, fileName);
	result &= stream.ReadTextFixedLengthBufferAsASCII(extraFieldLength, extraField);
	result &= stream.ReadTextFixedLengthBufferAsASCII(fileCommentLength, fileComment);

	return (result && (signature == validSignature));
}

//----------------------------------------------------------------------------------------
bool ZIPChunk_CentralFileHeader::SaveToStream(Stream::IStream& stream) const
{
	bool result = true;

	fileNameLength = (unsigned short)fileName.size();
	extraFieldLength = (unsigned short)extraField.size();
	fileCommentLength = (unsigned short)fileComment.size();

	result &= stream.WriteData(signature);
	result &= stream.WriteData(versionMadeBy);
	result &= stream.WriteData(versionNeededToExtract);
	result &= stream.WriteData(bitFlags);
	result &= stream.WriteData(compressionMethod);
	result &= stream.WriteData(lastModFileTime);
	result &= stream.WriteData(lastModFileDate);
	result &= stream.WriteData(crc32);
	result &= stream.WriteData(compressedSize);
	result &= stream.WriteData(uncompressedSize);
	result &= stream.WriteData(fileNameLength);
	result &= stream.WriteData(extraFieldLength);
	result &= stream.WriteData(fileCommentLength);
	result &= stream.WriteData(diskNumberStart);
	result &= stream.WriteData(internalFileAttributes);
	result &= stream.WriteData(externalFileAttributes);
	result &= stream.WriteData(relativeOffsetOfLocalHeader);
	result &= stream.WriteTextFixedLengthBufferAsASCII(fileNameLength, fileName);
	result &= stream.WriteTextFixedLengthBufferAsASCII(extraFieldLength, extraField);
	result &= stream.WriteTextFixedLengthBufferAsASCII(fileCommentLength, fileComment);

	return result;
}

//----------------------------------------------------------------------------------------
//Size functions
//----------------------------------------------------------------------------------------
unsigned int ZIPChunk_CentralFileHeader::GetSize()
{
	unsigned int size = 0;

	size += sizeof(signature);
	size += sizeof(versionMadeBy);
	size += sizeof(versionNeededToExtract);
	size += sizeof(bitFlags);
	size += sizeof(compressionMethod);
	size += sizeof(lastModFileTime);
	size += sizeof(lastModFileDate);
	size += sizeof(crc32);
	size += sizeof(compressedSize);
	size += sizeof(uncompressedSize);
	size += sizeof(fileNameLength);
	size += sizeof(extraFieldLength);
	size += sizeof(fileCommentLength);
	size += sizeof(diskNumberStart);
	size += sizeof(internalFileAttributes);
	size += sizeof(externalFileAttributes);
	size += sizeof(relativeOffsetOfLocalHeader);
	size += (unsigned int)fileName.size();
	size += (unsigned int)extraField.size();
	size += (unsigned int)fileComment.size();

	return size;
}
