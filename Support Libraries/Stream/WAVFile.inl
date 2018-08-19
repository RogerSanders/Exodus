namespace Stream {

//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum class WAVFile::OpenMode
{
	ReadOnly,
	WriteOnly,
	ReadAndWrite
};

//----------------------------------------------------------------------------------------
enum class WAVFile::CreateMode
{
	Open,       //Open an existing file. Fails if the file doesn't exist.
	Create,     //Create a new file. Truncates the file if it exists.
	CreateNew,  //Create a new file. Fails if the file already exists.
	Overwrite   //The file must exist, and will be truncated.
};

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
WAVFile::WAVFile()
:_fileBuffer(0), _bufferSize(0), _fileOpen(false), _waveHeaderLoaded(false), _savedSampleCount(0)
{
	SetByteOrder(ByteOrder::LittleEndian);
}

//----------------------------------------------------------------------------------------
//Data format
//----------------------------------------------------------------------------------------
bool WAVFile::GetDataFormat(unsigned int& channelCount, unsigned int& bitsPerSample, unsigned int& samplesPerSec) const
{
	//Ensure that the wave header has been populated
	if(!_waveHeaderLoaded)
	{
		return false;
	}

	//Read the WAVEFORMATEX header structure
	channelCount = (unsigned int)_waveHeader.nChannels;
	samplesPerSec = (unsigned int)_waveHeader.nSamplesPerSec;
	bitsPerSample = (unsigned int)_waveHeader.wBitsPerSample;

	return true;
}

//----------------------------------------------------------------------------------------
void WAVFile::SetDataFormat(unsigned int channelCount, unsigned int bitsPerSample, unsigned int samplesPerSec)
{
	//Build the WAVEFORMATEX header structure
	_waveHeader.wFormatTag = WAVE_FORMAT_PCM;
	_waveHeader.nChannels = (WORD)channelCount;
	_waveHeader.nSamplesPerSec = (DWORD)samplesPerSec;
	_waveHeader.nBlockAlign = (WORD)((channelCount * bitsPerSample) / 8);
	_waveHeader.nAvgBytesPerSec = (DWORD)(samplesPerSec * _waveHeader.nBlockAlign);
	_waveHeader.wBitsPerSample = (WORD)bitsPerSample;
	_waveHeader.cbSize = 0;

	//Flag that the wave header has been populated
	_waveHeaderLoaded = true;
}

//----------------------------------------------------------------------------------------
WAVFile::SizeType WAVFile::GetSavedSampleCount() const
{
	return _savedSampleCount;
}

//----------------------------------------------------------------------------------------
//File binding
//----------------------------------------------------------------------------------------
bool WAVFile::Open(const std::wstring& filename, OpenMode openMode, CreateMode createMode, SizeType bufferSize)
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
	case OpenMode::ReadOnly:
		desiredAccess = GENERIC_READ;
		shareMode = FILE_SHARE_READ;
		break;
	case OpenMode::WriteOnly:
		desiredAccess = GENERIC_WRITE;
		shareMode = 0;
		break;
	case OpenMode::ReadAndWrite:
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
	case CreateMode::Open: //Open an existing file. Fails if the file doesn't exist.
		openExistingFile = true;
		creationDisposition = OPEN_EXISTING;
		break;
	case CreateMode::Create: //Create a new file. Truncates the file if it exists.
		creationDisposition = CREATE_ALWAYS;
		break;
	case CreateMode::CreateNew: //Create a new file. Fails if the file already exists.
		creationDisposition = CREATE_NEW;
		break;
	case CreateMode::Overwrite: //The file must exist, and will be truncated.
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
	_mmioHandle = mmioOpen(NULL, &mmioInfo, 0);
	if(_mmioHandle == NULL)
	{
		return false;
	}

	//Either read or write the management structures in the target file, depending on
	//whether we're opening an existing file or creating a new file.
	if(openExistingFile)
	{
		//Attempt to load the RIFF chunk as a WAVE type
		ZeroMemory(&_riffChunk, sizeof(_riffChunk));
		_riffChunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');
		MMRESULT mmioDescendReturn = mmioDescend(_mmioHandle, &_riffChunk, NULL, MMIO_FINDRIFF);
		if(mmioDescendReturn != MMSYSERR_NOERROR)
		{
			mmioClose(_mmioHandle, 0);
			return false;
		}

		//Attempt to locate the format chunk
		ZeroMemory(&_fmtChunk, sizeof(_fmtChunk));
		_fmtChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
		mmioDescendReturn = mmioDescend(_mmioHandle, &_fmtChunk, &_riffChunk, MMIO_FINDCHUNK);
		if(mmioDescendReturn != MMSYSERR_NOERROR)
		{
			mmioClose(_mmioHandle, 0);
			return false;
		}

		//Ensure the format chunk is large enough to hold our required parameters
		unsigned int waveHeaderSize = sizeof(_waveHeader.wFormatTag) + sizeof(_waveHeader.nChannels) + sizeof(_waveHeader.nSamplesPerSec) + sizeof(_waveHeader.nBlockAlign) + sizeof(_waveHeader.nAvgBytesPerSec) + sizeof(_waveHeader.wBitsPerSample);
		if(_fmtChunk.cksize < (DWORD)waveHeaderSize)
		{
			mmioClose(_mmioHandle, 0);
			return false;
		}

		//Read the parameters from the format chunk
		LONG mmioReadReturn;
		bool mmioReadSucceeded = true;
		mmioReadReturn = mmioRead(_mmioHandle, (char*)&_waveHeader.wFormatTag, sizeof(_waveHeader.wFormatTag));
		mmioReadSucceeded &= (mmioReadReturn != -1);
		mmioReadReturn = mmioRead(_mmioHandle, (char*)&_waveHeader.nChannels, sizeof(_waveHeader.nChannels));
		mmioReadSucceeded &= (mmioReadReturn != -1);
		mmioReadReturn = mmioRead(_mmioHandle, (char*)&_waveHeader.nSamplesPerSec, sizeof(_waveHeader.nSamplesPerSec));
		mmioReadSucceeded &= (mmioReadReturn != -1);
		mmioReadReturn = mmioRead(_mmioHandle, (char*)&_waveHeader.nAvgBytesPerSec, sizeof(_waveHeader.nAvgBytesPerSec));
		mmioReadSucceeded &= (mmioReadReturn != -1);
		mmioReadReturn = mmioRead(_mmioHandle, (char*)&_waveHeader.nBlockAlign, sizeof(_waveHeader.nBlockAlign));
		mmioReadSucceeded &= (mmioReadReturn != -1);
		mmioReadReturn = mmioRead(_mmioHandle, (char*)&_waveHeader.wBitsPerSample, sizeof(_waveHeader.wBitsPerSample));
		mmioReadSucceeded &= (mmioReadReturn != -1);
		_waveHeader.cbSize = 0;
		if(!mmioReadSucceeded)
		{
			mmioClose(_mmioHandle, 0);
			return false;
		}

		//Since we've now read the contents of the wave header, flag that the wave header
		//has been loaded.
		_waveHeaderLoaded = true;

		//Ascend out of the format chunk
		MMRESULT mmioAscendReturn;
		mmioAscendReturn = mmioAscend(_mmioHandle, &_fmtChunk, 0);
		if(mmioAscendReturn != MMSYSERR_NOERROR)
		{
			mmioClose(_mmioHandle, 0);
			return false;
		}

		//Attempt to locate the data chunk
		ZeroMemory(&_dataChunk, sizeof(_dataChunk));
		_dataChunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
		mmioDescendReturn = mmioDescend(_mmioHandle, &_dataChunk, &_riffChunk, MMIO_FINDCHUNK);
		if(mmioDescendReturn != MMSYSERR_NOERROR)
		{
			mmioClose(_mmioHandle, 0);
			return false;
		}

		//Calculate the number of whole samples which are recorded as present in the data
		//segment
		_savedSampleCount = (_dataChunk.cksize / _waveHeader.nBlockAlign);
	}
	else
	{
		//Ensure that the wave header has been loaded
		if(!_waveHeaderLoaded)
		{
			mmioClose(_mmioHandle, 0);
			return false;
		}

		//Write the WAVE chunk header
		ZeroMemory(&_riffChunk, sizeof(_riffChunk));
		_riffChunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');
		MMRESULT mmioCreateChunkReturn;
		mmioCreateChunkReturn = mmioCreateChunk(_mmioHandle, &_riffChunk, MMIO_CREATERIFF);
		if(mmioCreateChunkReturn != MMSYSERR_NOERROR)
		{
			mmioClose(_mmioHandle, 0);
			return false;
		}

		//Write the format chunk header
		ZeroMemory(&_fmtChunk, sizeof(_fmtChunk));
		_fmtChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
		mmioCreateChunkReturn = mmioCreateChunk(_mmioHandle, &_fmtChunk, 0);
		if(mmioCreateChunkReturn != MMSYSERR_NOERROR)
		{
			mmioClose(_mmioHandle, 0);
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
		mmioWriteReturn = mmioWrite(_mmioHandle, (const char*)&_waveHeader.wFormatTag, sizeof(_waveHeader.wFormatTag));
		mmioWriteSucceeded &= (mmioWriteReturn != -1);
		mmioWriteReturn = mmioWrite(_mmioHandle, (const char*)&_waveHeader.nChannels, sizeof(_waveHeader.nChannels));
		mmioWriteSucceeded &= (mmioWriteReturn != -1);
		mmioWriteReturn = mmioWrite(_mmioHandle, (const char*)&_waveHeader.nSamplesPerSec, sizeof(_waveHeader.nSamplesPerSec));
		mmioWriteSucceeded &= (mmioWriteReturn != -1);
		mmioWriteReturn = mmioWrite(_mmioHandle, (const char*)&_waveHeader.nAvgBytesPerSec, sizeof(_waveHeader.nAvgBytesPerSec));
		mmioWriteSucceeded &= (mmioWriteReturn != -1);
		mmioWriteReturn = mmioWrite(_mmioHandle, (const char*)&_waveHeader.nBlockAlign, sizeof(_waveHeader.nBlockAlign));
		mmioWriteSucceeded &= (mmioWriteReturn != -1);
		mmioWriteReturn = mmioWrite(_mmioHandle, (const char*)&_waveHeader.wBitsPerSample, sizeof(_waveHeader.wBitsPerSample));
		mmioWriteSucceeded &= (mmioWriteReturn != -1);
		if(!mmioWriteSucceeded)
		{
			mmioClose(_mmioHandle, 0);
			return false;
		}

		//Ascend out of the format chunk
		MMRESULT mmioAscendReturn;
		mmioAscendReturn = mmioAscend(_mmioHandle, &_fmtChunk, 0);
		if(mmioAscendReturn != MMSYSERR_NOERROR)
		{
			mmioClose(_mmioHandle, 0);
			return false;
		}

		//Write the data chunk header
		ZeroMemory(&_dataChunk, sizeof(_dataChunk));
		_dataChunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
		mmioCreateChunkReturn = mmioCreateChunk(_mmioHandle, &_dataChunk, 0);
		if(mmioCreateChunkReturn != MMSYSERR_NOERROR)
		{
			mmioClose(_mmioHandle, 0);
			return false;
		}
	}

	//Initialize the buffer for the file
	if(_bufferSize != bufferSize)
	{
		_bufferSize = bufferSize;
		delete[] _fileBuffer;
		_fileBuffer = new unsigned char[(size_t)_bufferSize];
	}
	_bufferPosOffset = 0;
	__bytesRemainingInBuffer = _bufferSize;

	//Flag that a file is open, and return true.
	_fileOpen = true;
	return true;
}

//----------------------------------------------------------------------------------------
void WAVFile::Close()
{
	if(_fileOpen)
	{
		//Write any buffered contents to the file
		EmptyDataBuffer();

		//Flag that a file is no longer open
		_fileOpen = false;

		//Ascend out of the data chunk
		mmioAscend(_mmioHandle, &_dataChunk, 0);

		//Ascend out of the WAVE chunk
		mmioAscend(_mmioHandle, &_riffChunk, 0);

		//Close the MMIO file handle
		mmioClose(_mmioHandle, 0);
	}
}

//----------------------------------------------------------------------------------------
bool WAVFile::IsOpen() const
{
	return _fileOpen;
}

//----------------------------------------------------------------------------------------
//Internal read/write functions
//----------------------------------------------------------------------------------------
bool WAVFile::ReadBinary(void* rawData, SizeType bytesToRead)
{
	if(!_fileOpen)
	{
		return false;
	}

	//Write any buffered contents to the file
	bool result = true;
	result &= EmptyDataBuffer();

	//Perform the read
	LONG mmioReadReturn = mmioRead(_mmioHandle, (char*)rawData, (LONG)bytesToRead);

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
	if(!_fileOpen)
	{
		return false;
	}

	//Perform the write operation to the file
	bool result = true;
	if(bytesToWrite > _bufferSize)
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
		bytesToWriteToBuffer = (bytesToWrite <= __bytesRemainingInBuffer)? bytesToWrite: __bytesRemainingInBuffer;
		memcpy((void*)(_fileBuffer + _bufferPosOffset), (void*)rawDataAsCharArray, (size_t)bytesToWriteToBuffer);
		_bufferPosOffset += bytesToWriteToBuffer;
		__bytesRemainingInBuffer -= bytesToWriteToBuffer;

		//Reload the data buffer if necessary, and write any remaining data to be written
		//to the buffer.
		SizeType bytesRemainingToWrite = bytesToWrite - bytesToWriteToBuffer;
		if(bytesRemainingToWrite > 0)
		{
			//Set the data buffer to write mode
			result &= EmptyDataBuffer();

			//Write the data to the buffer
			memcpy((void*)(_fileBuffer + _bufferPosOffset), (void*)(rawDataAsCharArray + bytesToWriteToBuffer), (size_t)bytesRemainingToWrite);
			_bufferPosOffset += bytesRemainingToWrite;
			__bytesRemainingInBuffer -= bytesRemainingToWrite;
		}
	}

	return result;
}

//----------------------------------------------------------------------------------------
bool WAVFile::WriteBinaryUnbuffered(const void* rawData, SizeType bytesToWrite)
{
	if(!_fileOpen)
	{
		return false;
	}

	//Perform the write
	LONG mmioWriteReturn = mmioWrite(_mmioHandle, (const char*)rawData, (LONG)bytesToWrite);

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
	if(_bufferPosOffset > 0)
	{
		result &= WriteBinaryUnbuffered((void*)_fileBuffer, _bufferPosOffset);
	}

	//Reset the buffer position back to the start
	_bufferPosOffset = 0;
	__bytesRemainingInBuffer = _bufferSize;

	return result;
}

} //Close namespace Stream
