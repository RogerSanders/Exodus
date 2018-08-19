//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
FileStreamReference::FileStreamReference(const IGUIExtensionInterface& guiExtensionInterface)
:_guiExtensionInterface(guiExtensionInterface), _stream(0)
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
	_stream = _guiExtensionInterface.OpenExistingFileForRead(path);

	//Return the result to the caller
	bool streamOpenedSuccessfully = (_stream != 0);
	return streamOpenedSuccessfully;
}

//----------------------------------------------------------------------------------------
void FileStreamReference::CloseFileStream()
{
	//If a file stream is currently open, close it.
	if (_stream != 0)
	{
		_guiExtensionInterface.DeleteFileStream(_stream);
		_stream = 0;
	}
}

//----------------------------------------------------------------------------------------
//Dereference operators
//----------------------------------------------------------------------------------------
const Stream::IStream& FileStreamReference::operator*() const
{
	return *_stream;
}

//----------------------------------------------------------------------------------------
Stream::IStream& FileStreamReference::operator*()
{
	return *_stream;
}

//----------------------------------------------------------------------------------------
const Stream::IStream& FileStreamReference::operator->() const
{
	return *_stream;
}

//----------------------------------------------------------------------------------------
Stream::IStream& FileStreamReference::operator->()
{
	return *_stream;
}
