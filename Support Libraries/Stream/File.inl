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
:fileBuffer(0), bufferSize(0), fileOpen(false), bufferInWriteMode(false)
{}

//----------------------------------------------------------------------------------------
File::File(TextEncoding atextEncoding)
:Stream(atextEncoding), fileBuffer(0), bufferSize(0), fileOpen(false), bufferInWriteMode(false)
{}

//----------------------------------------------------------------------------------------
File::File(TextEncoding atextEncoding, NewLineEncoding anewLineEncoding)
:Stream(atextEncoding, anewLineEncoding), fileBuffer(0), bufferSize(0), fileOpen(false), bufferInWriteMode(false)
{}

//----------------------------------------------------------------------------------------
File::File(TextEncoding atextEncoding, NewLineEncoding anewLineEncoding, ByteOrder abyteOrder)
:Stream(atextEncoding, anewLineEncoding, abyteOrder), fileBuffer(0), bufferSize(0), fileOpen(false), bufferInWriteMode(false)
{}

//----------------------------------------------------------------------------------------
//File binding
//----------------------------------------------------------------------------------------
bool File::Open(const std::string& filename, OpenMode openMode, CreateMode createMode, SizeType abufferSize)
{
	return Open(ConvertStringToWString(filename), openMode, createMode, abufferSize);
}

//----------------------------------------------------------------------------------------
bool File::Open(const std::wstring& filename, OpenMode openMode, CreateMode createMode, SizeType abufferSize)
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
	if(bufferSize != abufferSize)
	{
		bufferSize = abufferSize;
		delete[] fileBuffer;
		fileBuffer = new unsigned char[bufferSize];
	}
	bufferPosOffset = bufferSize;
	bytesRemainingInBuffer = 0;
	bufferInWriteMode = false;

	//Flag that a file is open, and return true.
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
bool File::ReadBinary(void* rawData, SizeType bytesToRead)
{
	//Ensure that a file is currently open
	if(!fileOpen)
	{
		return false;
	}

	//Prepare to perform the read operation
	bool result = true;
	unsigned char* rawDataAsCharArray = (unsigned char*)rawData;

	//Read the data from the buffer. Note that profiling has found memcpy to be the most
	//efficient way of doing this kind of data transfer under all circumstances, being
	//roughly as efficient for a single byte transfer, and noticeably more efficient for
	//two bytes or more.
	SizeType bytesToReadFromBuffer = 0;
	if(!bufferInWriteMode)
	{
		bytesToReadFromBuffer = (bytesToRead <= bytesRemainingInBuffer)? bytesToRead: bytesRemainingInBuffer;
	}
	memcpy((void*)rawDataAsCharArray, (void*)(fileBuffer + bufferPosOffset), (size_t)bytesToReadFromBuffer);
	bufferPosOffset += bytesToReadFromBuffer;
	bytesRemainingInBuffer -= bytesToReadFromBuffer;

	//Perform the read operation from the file
	SizeType bytesRemainingToRead = bytesToRead - bytesToReadFromBuffer;
	if(bytesRemainingToRead > 0)
	{
		//Empty the contents of the data buffer. If the buffer is in write mode, this will
		//cause the uncommitted contents to be flushed to disk. If the buffer is in read
		//mode, this won't do anything, since we've already advanced to the end of the
		//current read buffer in the step above.
		result &= EmptyDataBuffer();

		//If the size of the operation can fit within the data buffer, perform the
		//operation using the data buffer, otherwise perform the read operation
		//unbuffered.
		if(bytesRemainingToRead > bufferSize)
		{
			//Perform the remainder of the read operation unbuffered
			result &= ReadBinaryUnbuffered((void*)(rawDataAsCharArray + bytesToReadFromBuffer), bytesRemainingToRead);
		}
		else
		{
			//Reload the data read buffer
			result &= PrepareDataBufferForReads();

			//If there's enough data in the read buffer to perform the read operation,
			//read the data out of the buffer, otherwise set the result to false.
			if(bytesRemainingToRead > bytesRemainingInBuffer)
			{
				//There's not enough data remaining in the file to complete the read
				//operation. In case we read data out of the buffer before reloading the
				//buffer, we seek back to the file position we would have been up to in
				//the file before we cached this new read buffer block. This will allow
				//another read operation to follow this one, which will be performed from
				//the same file data location this attempted read operation was performed
				//from.
				LARGE_INTEGER relativeFilePointer;
				relativeFilePointer.QuadPart = -((LONGLONG)bytesToReadFromBuffer);
				BOOL setFilePointerExReturn = SetFilePointerEx(fileHandle, relativeFilePointer, NULL, FILE_CURRENT);
				result &= (setFilePointerExReturn != 0);

				//Since the read operation failed, set the result to false.
				result = false;
			}
			else
			{
				//Read the data from the buffer
				memcpy((void*)(rawDataAsCharArray + bytesToReadFromBuffer), (void*)(fileBuffer + bufferPosOffset), (size_t)bytesRemainingToRead);
				bufferPosOffset += bytesRemainingToRead;
				bytesRemainingInBuffer -= bytesRemainingToRead;
			}
		}
	}

	return result;
}

//----------------------------------------------------------------------------------------
bool File::WriteBinary(const void* rawData, SizeType bytesToWrite)
{
	//Ensure that a file is currently open
	if(!fileOpen)
	{
		return false;
	}

	//Perform the write operation to the file
	bool result = true;
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

		//Write data to the buffer
		SizeType bytesToWriteToBuffer = 0;
		if(bufferInWriteMode)
		{
			bytesToWriteToBuffer = (bytesToWrite <= bytesRemainingInBuffer)? bytesToWrite: bytesRemainingInBuffer;
		}
		memcpy((void*)(fileBuffer + bufferPosOffset), (void*)rawDataAsCharArray, (size_t)bytesToWriteToBuffer);
		bufferPosOffset += bytesToWriteToBuffer;
		bytesRemainingInBuffer -= bytesToWriteToBuffer;

		//Reload the data buffer if necessary, and write any remaining data to be written
		//to the buffer.
		SizeType bytesRemainingToWrite = bytesToWrite - bytesToWriteToBuffer;
		if(bytesRemainingToWrite > 0)
		{
			//Set the data buffer to write mode
			result &= EmptyDataBuffer();
			result &= PrepareDataBufferForWrites();

			//Write the data to the buffer
			memcpy((void*)(fileBuffer + bufferPosOffset), (void*)(rawDataAsCharArray + bytesToWriteToBuffer), (size_t)bytesRemainingToWrite);
			bufferPosOffset += bytesRemainingToWrite;
			bytesRemainingInBuffer -= bytesRemainingToWrite;
		}
	}

	return result;
}

//----------------------------------------------------------------------------------------
bool File::ReadBinaryUnbuffered(void* rawData, SizeType bytesToRead)
{
	if(!fileOpen)
	{
		return false;
	}

	DWORD bytesToReadAsDWORD = (DWORD)bytesToRead;
	DWORD bytesRead = 0;
	BOOL readFileReturn = ReadFile(fileHandle, rawData, bytesToReadAsDWORD, &bytesRead, NULL);
	//If we only managed to read part of the requested data, restore the original stream
	//position, and return false.
	if((bytesRead > 0) && (bytesRead != bytesToReadAsDWORD))
	{
		SetStreamPos(GetStreamPos() - (SizeType)bytesRead);
		return false;
	}
	//Return true if the read was successful, and we read the same number of bytes that
	//were requested.
	return (readFileReturn != 0) && (bytesRead == bytesToRead);
}

//----------------------------------------------------------------------------------------
bool File::WriteBinaryUnbuffered(const void* rawData, SizeType bytesToWrite)
{
	if(!fileOpen)
	{
		return false;
	}

	DWORD bytesToWriteAsDWORD = (DWORD)bytesToWrite;
	DWORD bytesWritten = 0;
	BOOL writeFileReturn = WriteFile(fileHandle, rawData, bytesToWriteAsDWORD, &bytesWritten, NULL);
	//If we only managed to write part of the supplied data, restore the original stream
	//position, and return false.
	if((bytesWritten > 0) && (bytesWritten != bytesToWriteAsDWORD))
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

	//Empty the contents of the data buffer
	if(bufferInWriteMode)
	{
		//If the buffer contains written data, write the buffer contents to the file.
		if(bufferPosOffset > 0)
		{
			result &= WriteBinaryUnbuffered((void*)fileBuffer, bufferPosOffset);
		}
	}
	else
	{
		//If the buffer contains unused cached read data, seek to the position in the file
		//that we're up to in the buffer.
		if(bytesRemainingInBuffer > 0)
		{
			LARGE_INTEGER relativeFilePointer;
			relativeFilePointer.QuadPart = -((LONGLONG)bytesRemainingInBuffer);
			BOOL setFilePointerExReturn = SetFilePointerEx(fileHandle, relativeFilePointer, NULL, FILE_CURRENT);
			result &= (setFilePointerExReturn != 0);
		}
	}

	//Reset the buffer to an empty state
	bufferInWriteMode = false;
	bufferPosOffset = bufferSize;
	bytesRemainingInBuffer = 0;

	return result;
}

//----------------------------------------------------------------------------------------
bool File::PrepareDataBufferForReads()
{
	//Set the data buffer to read mode
	bufferInWriteMode = false;

	//If a data buffer has been specified, load a new page of data into the data buffer,
	//otherwise set the data buffer as empty.
	if(bufferSize > 0)
	{
		//Read a new page into the data buffer. If there are not enough bytes remaining in
		//the file, the number returned in bytesRead will be less than the amount
		//requested in bytesToRead.
		DWORD bytesToRead = (DWORD)bufferSize;
		DWORD bytesRead = 0;
		BOOL readFileReturn = ReadFile(fileHandle, (void*)fileBuffer, bytesToRead, &bytesRead, NULL);

		//If there was an error performing the read operation, we empty the buffer and
		//return false.
		if(readFileReturn == 0)
		{
			bufferPosOffset = 0;
			bytesRemainingInBuffer = 0;
			return false;
		}

		//Calculate the number of bytes remaining in the buffer. This handles the case
		//where there wasn't enough data remaining in the file to fill the buffer.
		SizeType bytesEmptyInBuffer = (SizeType)(bytesToRead - bytesRead);
		bytesRemainingInBuffer = bufferSize - bytesEmptyInBuffer;
	}
	else
	{
		bytesRemainingInBuffer = 0;
	}

	//Reset the buffer position back to the start
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
	bytesRemainingInBuffer = bufferSize;

	return true;
}

} //Close namespace Stream
