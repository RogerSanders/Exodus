namespace Stream {

//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum WAVFile::OpenMode
{
	OPENMODE_READONLY,
	OPENMODE_WRITEONLY,
	OPENMODE_READANDWRITE
};

//----------------------------------------------------------------------------------------
enum WAVFile::CreateMode
{
	CREATEMODE_OPEN,       //Open an existing file. Fails if the file doesn't exist.
	CREATEMODE_CREATE,     //Create a new file. Truncates the file if it exists.
	CREATEMODE_CREATENEW,  //Create a new file. Fails if the file already exists.
	CREATEMODE_OVERWRITE   //The file must exist, and will be truncated.
};

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
WAVFile::WAVFile()
:fileBuffer(0), bufferSize(0), fileOpen(false), waveHeaderLoaded(false), savedSampleCount(0)
{
	SetByteOrder(BYTEORDER_LITTLEENDIAN);
}

//----------------------------------------------------------------------------------------
//Data format
//----------------------------------------------------------------------------------------
bool WAVFile::GetDataFormat(unsigned int& channelCount, unsigned int& bitsPerSample, unsigned int& samplesPerSec) const
{
	//Ensure that the wave header has been populated
	if(!waveHeaderLoaded)
	{
		return false;
	}

	//Read the WAVEFORMATEX header structure
	channelCount = (unsigned int)channelCount;
	samplesPerSec = (unsigned int)samplesPerSec;
	bitsPerSample = (unsigned int)bitsPerSample;

	return true;
}

//----------------------------------------------------------------------------------------
void WAVFile::SetDataFormat(unsigned int channelCount, unsigned int bitsPerSample, unsigned int samplesPerSec)
{
	//Build the WAVEFORMATEX header structure
	waveHeader.wFormatTag = WAVE_FORMAT_PCM;
	waveHeader.nChannels = (WORD)channelCount;
	waveHeader.nSamplesPerSec = (DWORD)samplesPerSec;
	waveHeader.nBlockAlign = (WORD)((channelCount * bitsPerSample) / 8);
	waveHeader.nAvgBytesPerSec = (DWORD)(samplesPerSec * waveHeader.nBlockAlign);
	waveHeader.wBitsPerSample = (WORD)bitsPerSample;
	waveHeader.cbSize = 0;

	//Flag that the wave header has been populated
	waveHeaderLoaded = true;
}

//----------------------------------------------------------------------------------------
WAVFile::SizeType WAVFile::GetSavedSampleCount() const
{
	return savedSampleCount;
}

//----------------------------------------------------------------------------------------
//File binding
//----------------------------------------------------------------------------------------
bool WAVFile::Open(const std::string& filename, OpenMode openMode, CreateMode createMode, SizeType abufferSize)
{
	return Open(ConvertStringToWString(filename), openMode, createMode, abufferSize);
}

//----------------------------------------------------------------------------------------
bool WAVFile::Open(const std::wstring& filename, OpenMode openMode, CreateMode createMode, SizeType abufferSize)
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
	bool openExistingFile = false;
	switch(createMode)
	{
	case CREATEMODE_OPEN: //Open an existing file. Fails if the file doesn't exist.
		openExistingFile = true;
		creationDisposition = OPEN_EXISTING;
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

	//Open the target file directly using CreateFile. We do this because the mmioOpen
	//method can only handle paths up to 128 characters, and breaks if any element of the
	//path contains a plus ('+') character, due to the possibility of extra parameters
	//being encoded into the file path.
	HANDLE fileHandle = CreateFile(filename.c_str(), desiredAccess, shareMode, NULL, creationDisposition, 0, NULL);
	if(fileHandle == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	//Create and initialize the MMIOINFO structure, passing in the already open native
	//file handle for the target.
	MMIOINFO mmioInfo;
	ZeroMemory(&mmioInfo, sizeof(mmioInfo));
	mmioInfo.fccIOProc = FOURCC_DOS;
	mmioInfo.adwInfo[0] = (DWORD)HandleToLong(fileHandle);

	//Open an MMIO file handle for the target file we've already loaded
	mmioHandle = mmioOpen(NULL, &mmioInfo, 0);
	if(mmioHandle == NULL)
	{
		return false;
	}

	//Either read or write the management structures in the target file, depending on
	//whether we're opening an existing file or creating a new file.
	if(openExistingFile)
	{
		//Attempt to load the RIFF chunk as a WAVE type
		ZeroMemory(&riffChunk, sizeof(riffChunk));
		riffChunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');
		MMRESULT mmioDescendReturn = mmioDescend(mmioHandle, &riffChunk, NULL, MMIO_FINDRIFF);
		if(mmioDescendReturn != MMSYSERR_NOERROR)
		{
			mmioClose(mmioHandle, 0);
			return false;
		}

		//Attempt to locate the format chunk
		ZeroMemory(&fmtChunk, sizeof(fmtChunk));
		fmtChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
		mmioDescendReturn = mmioDescend(mmioHandle, &fmtChunk, &riffChunk, MMIO_FINDCHUNK);
		if(mmioDescendReturn != MMSYSERR_NOERROR)
		{
			mmioClose(mmioHandle, 0);
			return false;
		}

		//Ensure the format chunk is large enough to hold our required parameters
		unsigned int waveHeaderSize = sizeof(waveHeader.wFormatTag) + sizeof(waveHeader.nChannels) + sizeof(waveHeader.nSamplesPerSec) + sizeof(waveHeader.nBlockAlign) + sizeof(waveHeader.nAvgBytesPerSec) + sizeof(waveHeader.wBitsPerSample);
		if(fmtChunk.cksize < (DWORD)waveHeaderSize)
		{
			mmioClose(mmioHandle, 0);
			return false;
		}

		//Read the parameters from the format chunk
		LONG mmioReadReturn;
		bool mmioReadSucceeded = true;
		mmioReadReturn = mmioRead(mmioHandle, (char*)&waveHeader.wFormatTag, sizeof(waveHeader.wFormatTag));
		mmioReadSucceeded &= (mmioReadReturn != -1);
		mmioReadReturn = mmioRead(mmioHandle, (char*)&waveHeader.nChannels, sizeof(waveHeader.nChannels));
		mmioReadSucceeded &= (mmioReadReturn != -1);
		mmioReadReturn = mmioRead(mmioHandle, (char*)&waveHeader.nSamplesPerSec, sizeof(waveHeader.nSamplesPerSec));
		mmioReadSucceeded &= (mmioReadReturn != -1);
		mmioReadReturn = mmioRead(mmioHandle, (char*)&waveHeader.nAvgBytesPerSec, sizeof(waveHeader.nAvgBytesPerSec));
		mmioReadSucceeded &= (mmioReadReturn != -1);
		mmioReadReturn = mmioRead(mmioHandle, (char*)&waveHeader.nBlockAlign, sizeof(waveHeader.nBlockAlign));
		mmioReadSucceeded &= (mmioReadReturn != -1);
		mmioReadReturn = mmioRead(mmioHandle, (char*)&waveHeader.wBitsPerSample, sizeof(waveHeader.wBitsPerSample));
		mmioReadSucceeded &= (mmioReadReturn != -1);
		waveHeader.cbSize = 0;
		if(!mmioReadSucceeded)
		{
			mmioClose(mmioHandle, 0);
			return false;
		}

		//Since we've now read the contents of the wave header, flag that the wave header
		//has been loaded.
		waveHeaderLoaded = true;

		//Ascend out of the format chunk
		MMRESULT mmioAscendReturn;
		mmioAscendReturn = mmioAscend(mmioHandle, &fmtChunk, 0);
		if(mmioAscendReturn != MMSYSERR_NOERROR)
		{
			mmioClose(mmioHandle, 0);
			return false;
		}

		//Attempt to locate the data chunk
		ZeroMemory(&dataChunk, sizeof(dataChunk));
		dataChunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
		mmioDescendReturn = mmioDescend(mmioHandle, &dataChunk, &riffChunk, MMIO_FINDCHUNK);
		if(mmioDescendReturn != MMSYSERR_NOERROR)
		{
			mmioClose(mmioHandle, 0);
			return false;
		}

		//Calculate the number of whole samples which are recorded as present in the data
		//segment
		savedSampleCount = (dataChunk.cksize / waveHeader.nBlockAlign);
	}
	else
	{
		//Ensure that the wave header has been loaded
		if(!waveHeaderLoaded)
		{
			mmioClose(mmioHandle, 0);
			return false;
		}

		//Write the WAVE chunk header
		ZeroMemory(&riffChunk, sizeof(riffChunk));
		riffChunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');
		MMRESULT mmioCreateChunkReturn;
		mmioCreateChunkReturn = mmioCreateChunk(mmioHandle, &riffChunk, MMIO_CREATERIFF);
		if(mmioCreateChunkReturn != MMSYSERR_NOERROR)
		{
			mmioClose(mmioHandle, 0);
			return false;
		}

		//Write the format chunk header
		ZeroMemory(&fmtChunk, sizeof(fmtChunk));
		fmtChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
		mmioCreateChunkReturn = mmioCreateChunk(mmioHandle, &fmtChunk, 0);
		if(mmioCreateChunkReturn != MMSYSERR_NOERROR)
		{
			mmioClose(mmioHandle, 0);
			return false;
		}

		//Write the WAVEFORMATEX header to the format chunk data. Note that we have to
		//write out the members of the structure separately, since the cbSize member is
		//actually not part of the standard WAV file format chunk, but the WAVEFORMATEX
		//structure seems to be the most suitable structure to represent a WAV file format
		//chunk which has been provided in the API. We strip cbSize off here when writing
		//the members of this structure in order to output the correct format chunk.
		//Writing out the members separately also saves us having to worry about alignment
		//padding issues.
		LONG mmioWriteReturn;
		bool mmioWriteSucceeded = true;
		mmioWriteReturn = mmioWrite(mmioHandle, (const char*)&waveHeader.wFormatTag, sizeof(waveHeader.wFormatTag));
		mmioWriteSucceeded &= (mmioWriteReturn != -1);
		mmioWriteReturn = mmioWrite(mmioHandle, (const char*)&waveHeader.nChannels, sizeof(waveHeader.nChannels));
		mmioWriteSucceeded &= (mmioWriteReturn != -1);
		mmioWriteReturn = mmioWrite(mmioHandle, (const char*)&waveHeader.nSamplesPerSec, sizeof(waveHeader.nSamplesPerSec));
		mmioWriteSucceeded &= (mmioWriteReturn != -1);
		mmioWriteReturn = mmioWrite(mmioHandle, (const char*)&waveHeader.nAvgBytesPerSec, sizeof(waveHeader.nAvgBytesPerSec));
		mmioWriteSucceeded &= (mmioWriteReturn != -1);
		mmioWriteReturn = mmioWrite(mmioHandle, (const char*)&waveHeader.nBlockAlign, sizeof(waveHeader.nBlockAlign));
		mmioWriteSucceeded &= (mmioWriteReturn != -1);
		mmioWriteReturn = mmioWrite(mmioHandle, (const char*)&waveHeader.wBitsPerSample, sizeof(waveHeader.wBitsPerSample));
		mmioWriteSucceeded &= (mmioWriteReturn != -1);
		if(!mmioWriteSucceeded)
		{
			mmioClose(mmioHandle, 0);
			return false;
		}

		//Ascend out of the format chunk
		MMRESULT mmioAscendReturn;
		mmioAscendReturn = mmioAscend(mmioHandle, &fmtChunk, 0);
		if(mmioAscendReturn != MMSYSERR_NOERROR)
		{
			mmioClose(mmioHandle, 0);
			return false;
		}

		//Write the data chunk header
		ZeroMemory(&dataChunk, sizeof(dataChunk));
		dataChunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
		mmioCreateChunkReturn = mmioCreateChunk(mmioHandle, &dataChunk, 0);
		if(mmioCreateChunkReturn != MMSYSERR_NOERROR)
		{
			mmioClose(mmioHandle, 0);
			return false;
		}
	}

	//Initialize the buffer for the file
	if(bufferSize != abufferSize)
	{
		bufferSize = abufferSize;
		delete[] fileBuffer;
		fileBuffer = new unsigned char[(size_t)bufferSize];
	}
	bufferPosOffset = 0;
	bytesRemainingInBuffer = bufferSize;

	//Flag that a file is open, and return true.
	fileOpen = true;
	return true;
}

//----------------------------------------------------------------------------------------
void WAVFile::Close()
{
	if(fileOpen)
	{
		//Write any buffered contents to the file
		EmptyDataBuffer();

		//Flag that a file is no longer open
		fileOpen = false;

		//Ascend out of the data chunk
		mmioAscend(mmioHandle, &dataChunk, 0);

		//Ascend out of the WAVE chunk
		mmioAscend(mmioHandle, &riffChunk, 0);

		//Close the MMIO file handle
		mmioClose(mmioHandle, 0);
	}
}

//----------------------------------------------------------------------------------------
bool WAVFile::IsOpen() const
{
	return fileOpen;
}

//----------------------------------------------------------------------------------------
//Internal read/write functions
//----------------------------------------------------------------------------------------
bool WAVFile::ReadBinary(void* rawData, SizeType bytesToRead)
{
	if(!fileOpen)
	{
		return false;
	}

	//Write any buffered contents to the file
	bool result = true;
	result &= EmptyDataBuffer();

	//Perform the read
	LONG mmioReadReturn = mmioRead(mmioHandle, (char*)rawData, (LONG)bytesToRead);

	//Flag that the read operation was successful if no error was returned, and we read
	//the same number of bytes that were requested.
	result &= (mmioReadReturn != -1) && (mmioReadReturn == (LONG)bytesToRead);

	//Return the result of the operation
	return result;
}

//----------------------------------------------------------------------------------------
bool WAVFile::WriteBinary(const void* rawData, SizeType bytesToWrite)
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
		bytesToWriteToBuffer = (bytesToWrite <= bytesRemainingInBuffer)? bytesToWrite: bytesRemainingInBuffer;
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

			//Write the data to the buffer
			memcpy((void*)(fileBuffer + bufferPosOffset), (void*)(rawDataAsCharArray + bytesToWriteToBuffer), (size_t)bytesRemainingToWrite);
			bufferPosOffset += bytesRemainingToWrite;
			bytesRemainingInBuffer -= bytesRemainingToWrite;
		}
	}

	return result;
}

//----------------------------------------------------------------------------------------
bool WAVFile::WriteBinaryUnbuffered(const void* rawData, SizeType bytesToWrite)
{
	if(!fileOpen)
	{
		return false;
	}

	//Perform the write
	LONG mmioWriteReturn = mmioWrite(mmioHandle, (const char*)rawData, (LONG)bytesToWrite);

	//Return true if the write was successful, and we wrote the same number of bytes that
	//were supplied.
	return (mmioWriteReturn != -1) && (mmioWriteReturn == (LONG)bytesToWrite);
}

//----------------------------------------------------------------------------------------
//Data buffer functions
//----------------------------------------------------------------------------------------
bool WAVFile::EmptyDataBuffer()
{
	bool result = true;

	//If the buffer contains written data, write the buffer contents to the file.
	if(bufferPosOffset > 0)
	{
		result &= WriteBinaryUnbuffered((void*)fileBuffer, bufferPosOffset);
	}

	//Reset the buffer position back to the start
	bufferPosOffset = 0;
	bytesRemainingInBuffer = bufferSize;

	return result;
}

} //Close namespace Stream
