#include "WAVFile.h"
#include <WindowFunctions/WindowFunctions.pkg>

//----------------------------------------------------------------------------------------
WAVFile::~WAVFile()
{
	Close();
}

//----------------------------------------------------------------------------------------
bool WAVFile::Open(const std::wstring& fileName, unsigned int channelCount, unsigned int bitsPerSample, unsigned int samplesPerSec)
{
	////Create the target file
	//HMMIO fileHandle;
	//fileHandle = mmioOpen(fileName.c_str(), NULL, MMIO_WRITE | MMIO_CREATE | MMIO_EXCLUSIVE);
	//if(fileHandle == NULL)
	//{
	//	return false;
	//}

	////Write the RIFF chunk header
	//MMCKINFO riffChunk;
	//riffChunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	//MMRESULT mmioCreateChunkReturn;
	//mmioCreateChunkReturn = mmioCreateChunk(fileHandle, &riffChunk, MMIO_CREATERIFF);
	//if(mmioCreateChunkReturn != MMSYSERR_NOERROR)
	//{
	//	mmioClose(fileHandle, 0);
	//	return false;
	//}

	////Write the format chunk header
	//MMCKINFO fmtChunk;
	//fmtChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
	//mmioCreateChunkReturn = mmioCreateChunk(fileHandle, &fmtChunk, 0);
	//if(mmioCreateChunkReturn != MMSYSERR_NOERROR)
	//{
	//	mmioClose(fileHandle, 0);
	//	return false;
	//}

	////Build the WAVEFORMATEX header structure
	////##TODO## Determine whether this is the most appropriate structure to use for this
	////purpose, and whether other structures are supported.
	//WAVEFORMATEX waveHeader;
	//waveHeader.wFormatTag =	WAVE_FORMAT_PCM;
	//waveHeader.nChannels = (WORD)channelCount;
	//waveHeader.nSamplesPerSec = (DWORD)samplesPerSec;
	//waveHeader.nBlockAlign = (WORD)((channelCount * bitsPerSample) / 8);
	//waveHeader.nAvgBytesPerSec = (DWORD)(samplesPerSec * waveHeader.nBlockAlign);
	//waveHeader.wBitsPerSample = (WORD)bitsPerSample;
	//waveHeader.cbSize = 0;

	////Write the WAVEFORMATEX header to the format chunk data
	////##TODO## Add error handling
	//mmioWrite(fileHandle, (const char*)&waveHeader.wFormatTag,      sizeof(waveHeader.wFormatTag));
	//mmioWrite(fileHandle, (const char*)&waveHeader.nChannels,       sizeof(waveHeader.nChannels));
	//mmioWrite(fileHandle, (const char*)&waveHeader.nSamplesPerSec,  sizeof(waveHeader.nSamplesPerSec));
	//mmioWrite(fileHandle, (const char*)&waveHeader.nAvgBytesPerSec, sizeof(waveHeader.nAvgBytesPerSec));
	//mmioWrite(fileHandle, (const char*)&waveHeader.nBlockAlign,     sizeof(waveHeader.nBlockAlign));
	//mmioWrite(fileHandle, (const char*)&waveHeader.wBitsPerSample,  sizeof(waveHeader.wBitsPerSample));

	////Ascend out of the format chunk
	//MMRESULT mmioAscendReturn;
	//mmioAscendReturn = mmioAscend(fileHandle, &fmtChunk, 0);
	//if(mmioAscendReturn != MMSYSERR_NOERROR)
	//{
	//	mmioClose(fileHandle, 0);
	//	return false;
	//}

	//Create the target file
	if(!wavFile.Open(fileName, Stream::File::OPENMODE_READANDWRITE, Stream::File::CREATEMODE_CREATE))
	{
		return false;
	}
	Stream::ViewBinary wavFileStream(wavFile);

	//Write the RIFF header structure
	wavFileStream << 'R' << 'I' << 'F' << 'F';
	wavFileStream << (unsigned int)0;
	wavFileStream << 'W' << 'A' << 'V' << 'E';

	//Write the WAV header structure
	WAVEFORMATEX waveHeader;
	waveHeader.wFormatTag =	WAVE_FORMAT_PCM;
	waveHeader.nChannels = (WORD)channelCount;
	waveHeader.nSamplesPerSec = (DWORD)samplesPerSec;
	waveHeader.nBlockAlign = (WORD)((channelCount * bitsPerSample) / 8);
	waveHeader.nAvgBytesPerSec = (DWORD)(samplesPerSec * waveHeader.nBlockAlign);
	waveHeader.wBitsPerSample = (WORD)bitsPerSample;
	waveHeader.cbSize = 0;

	wavFileStream << 'f' << 'm' << 't' << ' ';
	wavFileStream << (unsigned int)16;
	wavFileStream << waveHeader.wFormatTag;
	wavFileStream << waveHeader.nChannels;
	wavFileStream << waveHeader.nSamplesPerSec;
	wavFileStream << waveHeader.nAvgBytesPerSec;
	wavFileStream << waveHeader.nBlockAlign;
	wavFileStream << waveHeader.wBitsPerSample;

	//Write the DATA header structure
	wavFileStream << 'd' << 'a' << 't' << 'a';
	wavFileStream << (unsigned int)0;

	return true;
}

//----------------------------------------------------------------------------------------
void WAVFile::Close()
{
	if(wavFile.IsOpen())
	{
		Stream::ViewBinary wavFileStream(wavFile);

		wavFile.SetStreamPos(4);
		if(wavFile.GetStreamPos() == 4)
		{
			wavFileStream << (unsigned int)wavFile.Size() - 8;
		}
		wavFile.SetStreamPos(0x28);
		if(wavFile.GetStreamPos() == 0x28)
		{
			wavFileStream << (unsigned int)wavFile.Size() - 0x2C;
		}

		wavFile.Close();
	}
}

//----------------------------------------------------------------------------------------
Stream::ViewBinary WAVFile::GetStream()
{
	return Stream::ViewBinary(wavFile);
}

//UINT WINAPI WriteWaveToFile(LPCSTR lpszWaveFilename,
//							LPCPCMWAVEFORMAT lppcmWaveFormat,
//							HPCSTR hpWaveData, DWORD dwWaveDataSize)
//{
//	ASSERT(lpszWaveFilename != NULL);
//	ASSERT(::AfxIsValidString(lpszWaveFilename));
//	ASSERT(lppcmWaveFormat != NULL);
//	ASSERT(::IsValidPCMWaveFormat(lppcmWaveFormat));
//
//	MMCKINFO mmckinfoParent, mmckinfoSubchunk;
//	HMMIO hmmio;
//
//	// Open the given file for writing using buffered I/O.
//	if ((hmmio = ::mmioOpen((LPSTR)lpszWaveFilename, NULL,
//		MMIO_CREATE | MMIO_WRITE | MMIO_EXCLUSIVE))
//		== NULL)
//		// Failed to create wave file.
//		return 1;
//
//	// Create a 'RIFF' chunk with a 'WAVE' form type
//	mmckinfoParent.fccType = mmioFOURCC('W', 'A', 'V', 'E');
//	if (::mmioCreateChunk(hmmio, &mmckinfoParent, MMIO_CREATERIFF) != 0)
//	{
//		// Failed create 'RIFF' chunk.
//		::mmioClose(hmmio, 0);
//		return 2;
//	}
//
//	// Create the format subchunk (form type 'fmt ').
//	mmckinfoSubchunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
//	mmckinfoSubchunk.cksize = sizeof(PCMWAVEFORMAT);
//	if (::mmioCreateChunk(hmmio, &mmckinfoSubchunk, 0) != 0)
//	{
//		// Failed create the format subchunk.
//		::mmioClose(hmmio, 0);
//		return 3;
//	}
//
//	// Write the format subchunk.
//	if (::mmioWrite(hmmio, (HPSTR)lppcmWaveFormat, sizeof(PCMWAVEFORMAT))
//		!= (LONG)sizeof(PCMWAVEFORMAT))
//	{
//		// Failed to write the format subchunk.
//		::mmioClose(hmmio, 0);
//		return 4;
//	}
//
//	// Ascend out of the format subchunk.
//	if (::mmioAscend(hmmio, &mmckinfoSubchunk, 0) != 0)
//	{
//		// Failed to write the format subchunk.
//		::mmioClose(hmmio, 0);
//		return 4;
//	}
//
//	// Create the data subchunk that holds the waveform samples.
//	mmckinfoSubchunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
//	mmckinfoSubchunk.cksize = dwWaveDataSize;
//	if (::mmioCreateChunk(hmmio, &mmckinfoSubchunk, 0) != 0)
//	{
//		// Failed to create the data subchunk.
//		::mmioClose(hmmio, 0);
//		return 5;
//	}
//
//	if (hpWaveData != NULL && dwWaveDataSize != 0)
//		// Write the waveform data subchunk.
//		if (::mmioWrite(hmmio, hpWaveData, (LONG)dwWaveDataSize)
//			!= (LONG)dwWaveDataSize)
//		{
//			// Failed to write the data subchunk.
//			::mmioClose(hmmio, 0);
//			return 6;
//		}
//
//		// Ascend out of the data subchunk.
//		if (::mmioAscend(hmmio, &mmckinfoSubchunk, 0) != 0)
//		{
//			// Failed to write the data subchunk.
//			::mmioClose(hmmio, 0);
//			return 6;
//		}
//
//		// Ascend out of the 'RIFF' chunk.
//		if (::mmioAscend(hmmio, &mmckinfoParent, 0) != 0)
//		{
//			// Failed create 'RIFF' chunk.
//			::mmioClose(hmmio, 0);
//			return 2;
//		}
//
//		// We're done with the file, close it.
//		::mmioClose(hmmio, 0);
//
//		return 0;    // successful
//}
