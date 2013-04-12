//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
FileStreamReference::FileStreamReference(const IGUIExtensionInterface& aguiExtensionInterface)
:guiExtensionInterface(aguiExtensionInterface), stream(0)
{}

//----------------------------------------------------------------------------------------
FileStreamReference::~FileStreamReference()
{
	CloseFileStream();
}

//----------------------------------------------------------------------------------------
//File handling functions
//----------------------------------------------------------------------------------------
bool FileStreamReference::OpenExistingFileForRead(const std::wstring& path)
{
	//Close any existing file stream we may have open
	CloseFileStream();

	//Open a new file stream to the target file
	stream = guiExtensionInterface.OpenExistingFileForRead(path);

	//Return the result to the caller
	bool streamOpenedSuccessfully = (stream != 0);
	return streamOpenedSuccessfully;
}

//----------------------------------------------------------------------------------------
void FileStreamReference::CloseFileStream()
{
	//If a file stream is currently open, close it.
	if(stream != 0)
	{
		guiExtensionInterface.DeleteFileStream(stream);
		stream = 0;
	}
}

//----------------------------------------------------------------------------------------
//Dereference operators
//----------------------------------------------------------------------------------------
const Stream::IStream& FileStreamReference::operator*() const
{
	return *stream;
}

//----------------------------------------------------------------------------------------
Stream::IStream& FileStreamReference::operator*()
{
	return *stream;
}

//----------------------------------------------------------------------------------------
const Stream::IStream& FileStreamReference::operator->() const
{
	return *stream;
}

//----------------------------------------------------------------------------------------
Stream::IStream& FileStreamReference::operator->()
{
	return *stream;
}
