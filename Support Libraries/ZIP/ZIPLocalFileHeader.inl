//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ZIPChunk_LocalFileHeader::ZIPChunk_LocalFileHeader()
{
	Initialize();
}

//----------------------------------------------------------------------------------------
void ZIPChunk_LocalFileHeader::Initialize()
{
	signature = validSignature;
	versionToExtract = 0;
	bitFlags = 0;
	compressionMethod = 0;
	modFileTime = 0;
	modFileDate = 0;
	crc32 = 0;
	compressedSize = 0;
	uncompressedSize = 0;
	fileNameLength = 0;
	extraFieldLength = 0;
}

//----------------------------------------------------------------------------------------
//Serialization functions
//----------------------------------------------------------------------------------------
bool ZIPChunk_LocalFileHeader::LoadFromStream(Stream::IStream& stream)
{
	bool result = true;

	result &= stream.ReadData(signature);
	result &= stream.ReadData(versionToExtract);
	result &= stream.ReadData(bitFlags);
	result &= stream.ReadData(compressionMethod);
	result &= stream.ReadData(modFileTime);
	result &= stream.ReadData(modFileDate);
	result &= stream.ReadData(crc32);
	result &= stream.ReadData(compressedSize);
	result &= stream.ReadData(uncompressedSize);
	result &= stream.ReadData(fileNameLength);
	result &= stream.ReadData(extraFieldLength);
	result &= stream.ReadTextFixedLengthBufferAsASCII(fileNameLength, fileName);
	result &= stream.ReadTextFixedLengthBufferAsASCII(extraFieldLength, extraField);

	return (result && (signature == validSignature));
}

//----------------------------------------------------------------------------------------
bool ZIPChunk_LocalFileHeader::SaveToStream(Stream::IStream& stream) const
{
	bool result = true;

	fileNameLength = (unsigned short)fileName.size();
	extraFieldLength = (unsigned short)extraField.size();

	result &= stream.WriteData(signature);
	result &= stream.WriteData(versionToExtract);
	result &= stream.WriteData(bitFlags);
	result &= stream.WriteData(compressionMethod);
	result &= stream.WriteData(modFileTime);
	result &= stream.WriteData(modFileDate);
	result &= stream.WriteData(crc32);
	result &= stream.WriteData(compressedSize);
	result &= stream.WriteData(uncompressedSize);
	result &= stream.WriteData(fileNameLength);
	result &= stream.WriteData(extraFieldLength);
	result &= stream.WriteTextFixedLengthBufferAsASCII(fileNameLength, fileName);
	result &= stream.WriteTextFixedLengthBufferAsASCII(extraFieldLength, extraField);

	return result;
}
