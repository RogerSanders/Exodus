namespace Stream {

//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum File::OpenMode
{
	OPENMODE_READONLY,
	OPENMODE_WRITEONLY,
	OPENMODE_READANDWRITE
};

//----------------------------------------------------------------------------------------
enum File::CreateMode
{
	CREATEMODE_OPEN,       //Open an existing file. Fails if the file doesn't exist.
	CREATEMODE_OPENALWAYS, //Open an existing file. If the file doesn't exist, create it.
	CREATEMODE_CREATE,     //Create a new file. Truncates the file if it exists.
	CREATEMODE_CREATENEW,  //Create a new file. Fails if the file already exists.
	CREATEMODE_OVERWRITE   //The file must exist, and will be truncated.
};

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
File::File()
:fileOpen(false)
{}

//----------------------------------------------------------------------------------------
File::File(TextEncoding atextEncoding)
:Stream(atextEncoding), fileOpen(false)
{}

//----------------------------------------------------------------------------------------
File::File(TextEncoding atextEncoding, NewLineEncoding anewLineEncoding)
:Stream(atextEncoding, anewLineEncoding), fileOpen(false)
{}

//----------------------------------------------------------------------------------------
File::File(TextEncoding atextEncoding, NewLineEncoding anewLineEncoding, ByteOrder abyteOrder)
:Stream(atextEncoding, anewLineEncoding, abyteOrder), fileOpen(false)
{}

//----------------------------------------------------------------------------------------
//File binding
//----------------------------------------------------------------------------------------
bool File::Open(const std::string& filename, OpenMode openMode, CreateMode createMode)
{
	return Open(ConvertStringToWString(filename), openMode, createMode);
}

//----------------------------------------------------------------------------------------
bool File::Open(const std::wstring& filename, OpenMode openMode, CreateMode createMode)
{
	//If a file handle is currently open, close it.
	if(IsOpen())
	{
		Close();
	}

	//Build our parameters for CreateFile
	DWORD shareMode;
	DWORD desiredAccess;
	DWORD creationDisposition;

	//Process the openMode parameter
	switch(openMode)
	{
	case OPENMODE_READONLY:
		desiredAccess = GENERIC_READ;
		shareMode = FILE_SHARE_READ;
		break;
	case OPENMODE_WRITEONLY:
		desiredAccess = GENERIC_WRITE;
		shareMode = 0;
		break;
	case OPENMODE_READANDWRITE:
		desiredAccess = GENERIC_READ | GENERIC_WRITE;
		shareMode = 0;
		break;
	default:
		return false;
	}

	//Process the createMode parameter
	switch(createMode)
	{
	case CREATEMODE_OPEN: //Open an existing file. Fails if the file doesn't exist.
		creationDisposition = OPEN_EXISTING;
		break;
	case CREATEMODE_OPENALWAYS: //Open an existing file. If the file doesn't exist, create it.
		creationDisposition = OPEN_ALWAYS;
		break;
	case CREATEMODE_CREATE: //Create a new file. Truncates the file if it exists.
		creationDisposition = CREATE_ALWAYS;
		break;
	case CREATEMODE_CREATENEW: //Create a new file. Fails if the file already exists.
		creationDisposition = CREATE_NEW;
		break;
	case CREATEMODE_OVERWRITE: //The file must exist, and will be truncated.
		creationDisposition = TRUNCATE_EXISTING;
		break;
	default:
		return false;
	}

	//Try and open a new file handle
	fileHandle = CreateFile(filename.c_str(), desiredAccess, shareMode, NULL, creationDisposition, 0, NULL);
	if(fileHandle == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	fileOpen = true;
	return true;
}

//----------------------------------------------------------------------------------------
void File::Close()
{
	if(fileOpen)
	{
		fileOpen = false;
		CloseHandle(fileHandle);
	}
}

//----------------------------------------------------------------------------------------
bool File::IsOpen() const
{
	return fileOpen;
}

//----------------------------------------------------------------------------------------
//Internal read/write functions
//----------------------------------------------------------------------------------------
bool File::ReadBinary(void* rawData, unsigned int bytesToRead)
{
	if(!fileOpen)
	{
		return false;
	}

	DWORD bytesRead = 0;
	BOOL readFileReturn = ReadFile(fileHandle, rawData, bytesToRead, &bytesRead, NULL);
	//If we only managed to read part of the requested data, restore the original stream
	//position, and return false.
	if((bytesRead > 0) && (bytesRead != bytesToRead))
	{
		SetStreamPos(GetStreamPos() - (SizeType)bytesRead);
		return false;
	}
	//Return true if the read was successful, and we read the same number of bytes that
	//were requested.
	return (readFileReturn != 0) && (bytesRead == bytesToRead);
}

//----------------------------------------------------------------------------------------
bool File::WriteBinary(const void* rawData, unsigned int bytesToWrite)
{
	if(!fileOpen)
	{
		return false;
	}

	DWORD bytesWritten = 0;
	BOOL writeFileReturn = WriteFile(fileHandle, rawData, bytesToWrite, &bytesWritten, NULL);
	//If we only managed to write part of the supplied data, restore the original stream
	//position, and return false.
	if((bytesWritten > 0) && (bytesWritten != bytesToWrite))
	{
		SetStreamPos(GetStreamPos() - (SizeType)bytesWritten);
		return false;
	}
	//Return true if the write was successful, and we wrote the same number of bytes that
	//were supplied.
	return (writeFileReturn != 0) && (bytesWritten == bytesToWrite);
}

} //Close namespace Stream
