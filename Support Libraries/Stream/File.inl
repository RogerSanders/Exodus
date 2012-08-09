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
:fileOpen(false), bufferInWriteMode(false)
{}

//----------------------------------------------------------------------------------------
File::File(TextEncoding atextEncoding)
:Stream(atextEncoding), fileOpen(false), bufferInWriteMode(false)
{}

//----------------------------------------------------------------------------------------
File::File(TextEncoding atextEncoding, NewLineEncoding anewLineEncoding)
:Stream(atextEncoding, anewLineEncoding), fileOpen(false), bufferInWriteMode(false)
{}

//----------------------------------------------------------------------------------------
File::File(TextEncoding atextEncoding, NewLineEncoding anewLineEncoding, ByteOrder abyteOrder)
:Stream(atextEncoding, anewLineEncoding, abyteOrder), fileOpen(false), bufferInWriteMode(false)
{}

//----------------------------------------------------------------------------------------
//File binding
//----------------------------------------------------------------------------------------
bool File::Open(const std::string& filename, OpenMode openMode, CreateMode createMode, unsigned int abufferSize)
{
	return Open(ConvertStringToWString(filename), openMode, createMode, abufferSize);
}

//----------------------------------------------------------------------------------------
bool File::Open(const std::wstring& filename, OpenMode openMode, CreateMode createMode, unsigned int abufferSize)
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

	//Initialize the buffer for the file
	bufferSize = abufferSize;
	fileBuffer.resize(bufferSize);
	bufferPosOffset = bufferSize;
	bufferInWriteMode = false;

	fileOpen = true;
	return true;
}

//----------------------------------------------------------------------------------------
void File::Close()
{
	if(fileOpen)
	{
		EmptyDataBuffer();
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
	bool result = true;
	if(!fileOpen)
	{
		return false;
	}

	//Perform the read operation from the file
	if(bytesToRead > bufferSize)
	{
		//If the size of the operation exceeds the maximum data buffer size for the file,
		//empty any current contents from the buffer, and perform the operation
		//unbuffered.
		result &= EmptyDataBuffer();
		result &= ReadBinaryUnbuffered(rawData, bytesToRead);
	}
	else
	{
		//If the size of the operation can fit within the data buffer, perform the
		//operation using the data buffer.
		unsigned char* rawDataAsCharArray = (unsigned char*)rawData;
		unsigned int bytesRemainingInBuffer = (bufferSize - bufferPosOffset);

		//Read the data from the buffer
		unsigned int bytesToReadFromBuffer = ((bytesToRead <= bytesRemainingInBuffer) && !bufferInWriteMode)? bytesToRead: bytesRemainingInBuffer;
		for(unsigned int i = 0; i < bytesToReadFromBuffer; ++i)
		{
			*(rawDataAsCharArray + i) = fileBuffer[(size_t)bufferPosOffset++];
		}

		//Reload the data buffer if necessary, and read any remaining data to be read from
		//the buffer.
		unsigned int bytesRemainingToRead = bytesToRead - bytesToReadFromBuffer;
		if(bytesRemainingToRead > 0)
		{
			//Attempt to load data into the data buffer. This may fail if there isn't
			//enough data left in the file to fill the read buffer.
			result &= EmptyDataBuffer();
			bool readDataCached = PrepareDataBufferForReads();

			//If we successfully managed to fill the read cache, perform the read from the
			//data cache, otherwise perform the read in unbuffered mode.
			if(readDataCached)
			{
				//Read the data from the buffer
				for(unsigned int i = 0; i < bytesRemainingToRead; ++i)
				{
					*(rawDataAsCharArray + bytesToReadFromBuffer + i) = fileBuffer[(size_t)bufferPosOffset++];
				}
			}
			else
			{
				//Read data directly from the file in unbuffered mode
				result &= ReadBinaryUnbuffered((void*)((unsigned char*)rawData + bytesToReadFromBuffer), bytesRemainingToRead);
			}
		}
	}

	return result;
}

//----------------------------------------------------------------------------------------
bool File::WriteBinary(const void* rawData, unsigned int bytesToWrite)
{
	bool result = true;
	if(!fileOpen)
	{
		return false;
	}

	//Perform the write operation to the file
	if(bytesToWrite > bufferSize)
	{
		//If the size of the operation exceeds the maximum data buffer size for the file,
		//empty any current contents from the buffer, and perform the operation
		//unbuffered.
		result &= EmptyDataBuffer();
		result &= WriteBinaryUnbuffered(rawData, bytesToWrite);
	}
	else
	{
		//If the size of the operation can fit within the data buffer, perform the
		//operation using the data buffer.
		unsigned char* rawDataAsCharArray = (unsigned char*)rawData;
		unsigned int bytesRemainingInBuffer = (bufferSize - bufferPosOffset);

		//Write data to the buffer
		unsigned int bytesToWriteToBuffer = ((bytesToWrite <= bytesRemainingInBuffer) && bufferInWriteMode)? bytesToWrite: bytesRemainingInBuffer;
		for(unsigned int i = 0; i < bytesToWriteToBuffer; ++i)
		{
			fileBuffer[(size_t)bufferPosOffset++] = *(rawDataAsCharArray + i);
		}

		//Reload the data buffer if necessary, and write any remaining data to be written
		//to the buffer.
		unsigned int bytesRemainingToWrite = bytesToWrite - bytesToWriteToBuffer;
		if(bytesRemainingToWrite > 0)
		{
			//Set the data buffer to write mode
			result &= EmptyDataBuffer();
			result &= PrepareDataBufferForWrites();

			//Write the data to the buffer
			for(unsigned int i = 0; i < bytesRemainingToWrite; ++i)
			{
				fileBuffer[(size_t)bufferPosOffset++] = *(rawDataAsCharArray + bytesToWriteToBuffer + i);
			}
		}
	}

	return result;
}

//----------------------------------------------------------------------------------------
bool File::ReadBinaryUnbuffered(void* rawData, unsigned int bytesToRead)
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
bool File::WriteBinaryUnbuffered(const void* rawData, unsigned int bytesToWrite)
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

//----------------------------------------------------------------------------------------
//Data buffer functions
//----------------------------------------------------------------------------------------
bool File::EmptyDataBuffer()
{
	bool result = true;

	//If the buffer contains written data, write the buffer contents to the file.
	if(bufferInWriteMode && (bufferPosOffset > 0))
	{
		result = WriteBinaryUnbuffered((void*)(&fileBuffer[0]), bufferPosOffset);
		bufferInWriteMode = false;
	}

	//Reset the buffer to an empty state
	bufferPosOffset = bufferSize;

	return result;
}

//----------------------------------------------------------------------------------------
bool File::PrepareDataBufferForReads()
{
	//Set the data buffer to read mode
	bufferInWriteMode = false;

	//Read a new page into the data buffer. If this fails, there might not be enough data
	//left in the file to fill the buffer. In this case, we empty the cache and return
	//false.
	if(!ReadBinaryUnbuffered((void*)(&fileBuffer[0]), bufferSize))
	{
		return false;
	}

	//Reset the buffer position back to the start. Note that this needs to happen after
	//we attempt to read the buffer data above, since if the buffer fill fails, we need to
	//restore the seek position in the file, which is adjusted based on the current buffer
	//offset.
	bufferPosOffset = 0;

	return true;
}

//----------------------------------------------------------------------------------------
bool File::PrepareDataBufferForWrites()
{
	//Set the data buffer to write mode
	bufferInWriteMode = true;

	//Reset the buffer position back to the start
	bufferPosOffset = 0;

	return true;
}

} //Close namespace Stream
