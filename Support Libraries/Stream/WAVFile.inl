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
	CREATEMODE_CREATE      //Create a new file. Truncates the file if it exists.
};

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
WAVFile::WAVFile()
:fileOpen(false)
{
	SetByteOrder(BYTEORDER_LITTLEENDIAN);
}

//----------------------------------------------------------------------------------------
//File binding
//----------------------------------------------------------------------------------------
bool WAVFile::Open(const std::string& filename, OpenMode openMode, CreateMode createMode, unsigned int channelCount, unsigned int bitsPerSample, unsigned int samplesPerSec)
{
	return Open(ConvertStringToWString(filename), openMode, createMode, channelCount, bitsPerSample, samplesPerSec);
}

//----------------------------------------------------------------------------------------
bool WAVFile::Open(const std::wstring& filename, OpenMode openMode, CreateMode createMode, unsigned int channelCount, unsigned int bitsPerSample, unsigned int samplesPerSec)
{
	//If a file handle is currently open, close it.
	if(IsOpen())
	{
		Close();
	}

	//Build our parameters for CreateFile
	DWORD openFlags = 0;

	//Process the openMode parameter
	switch(openMode)
	{
	case OPENMODE_READONLY:
		openFlags = MMIO_READ | MMIO_DENYWRITE;
		break;
	case OPENMODE_WRITEONLY:
		openFlags = MMIO_WRITE | MMIO_EXCLUSIVE;
		break;
	case OPENMODE_READANDWRITE:
		openFlags = MMIO_READWRITE | MMIO_EXCLUSIVE;
		break;
	default:
		return false;
	}

	//Process the createMode parameter
	switch(createMode)
	{
	case CREATEMODE_OPEN: //Open an existing file. Fails if the file doesn't exist.
		break;
	case CREATEMODE_CREATE: //Create a new file. Truncates the file if it exists.
		openFlags |= MMIO_CREATE;
		break;
	default:
		return false;
	}

	//Create and initialize the MMIOINFO structure
	MMIOINFO mmioInfo;
	ZeroMemory(&mmioInfo, sizeof(mmioInfo));

	//##FIX## Calling mmioOpen in this form will break if the filename or path contains a
	//plus '+' symbol, due to some advanced interpretation of the filename.
	//Create the target file
	mmioHandle = mmioOpen((LPWSTR)&filename[0], &mmioInfo, openFlags);
	if(mmioHandle == NULL)
	{
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

	//Build the WAVEFORMATEX header structure
	//##TODO## Determine whether this is the most appropriate structure to use for this
	//purpose, and whether other structures are supported.
	WAVEFORMATEX waveHeader;
	waveHeader.wFormatTag =	WAVE_FORMAT_PCM;
	waveHeader.nChannels = (WORD)channelCount;
	waveHeader.nSamplesPerSec = (DWORD)samplesPerSec;
	waveHeader.nBlockAlign = (WORD)((channelCount * bitsPerSample) / 8);
	waveHeader.nAvgBytesPerSec = (DWORD)(samplesPerSec * waveHeader.nBlockAlign);
	waveHeader.wBitsPerSample = (WORD)bitsPerSample;
	waveHeader.cbSize = 0;

	//Write the format chunk header
	MMCKINFO fmtChunk;
	ZeroMemory(&fmtChunk, sizeof(fmtChunk));
	fmtChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
	mmioCreateChunkReturn = mmioCreateChunk(mmioHandle, &fmtChunk, 0);
	if(mmioCreateChunkReturn != MMSYSERR_NOERROR)
	{
		mmioClose(mmioHandle, 0);
		return false;
	}

	//Write the WAVEFORMATEX header to the format chunk data. Note that we have to write
	//out the members of the structure separately, since the cbSize member is actually not
	//part of the standard WAV file format chunk, but the WAVEFORMATEX structure seems to
	//be the most suitable structure to represent a WAV file format chunk which has been
	//provided in the API. We strip cbSize off here when writing the members of this
	//structure in order to output the correct format chunk. Writing out the members
	//separately also saves us having to worry about alignment padding issues.
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

	//Flag that a file is open, and return true.
	fileOpen = true;
	return true;
}

//----------------------------------------------------------------------------------------
void WAVFile::Close()
{
	if(fileOpen)
	{
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
bool WAVFile::ReadBinary(void* rawData, unsigned int bytesToRead)
{
	if(!fileOpen)
	{
		return false;
	}

	//Perform the read
	LONG mmioReadReturn = mmioRead(mmioHandle, (char*)rawData, (LONG)bytesToRead);

	//Return true if the read was successful, and we read the same number of bytes that
	//were requested.
	return (mmioReadReturn != -1) && (mmioReadReturn == (LONG)bytesToRead);
}

//----------------------------------------------------------------------------------------
bool WAVFile::WriteBinary(const void* rawData, unsigned int bytesToWrite)
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

} //Close namespace Stream
