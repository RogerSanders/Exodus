#include "AudioStream.h"
#include <Mmreg.h>
#include <Ks.h>
#include <Ksmedia.h>
#include <iostream>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
AudioStream::AudioStream()
:workerThreadRunning(false), completedBufferSlots(0)
{
	//Create our critical section object
	InitializeCriticalSection(&waveMutex);

	//Create our event handles
	eventHandles[0] = CreateEvent(NULL, TRUE, FALSE, NULL);
	eventHandles[1] = CreateEvent(NULL, TRUE, FALSE, NULL);
	eventHandles[2] = CreateEvent(NULL, TRUE, FALSE, NULL);
	startupCompleteEventHandle = CreateEvent(NULL, TRUE, FALSE, NULL);
	shutdownCompleteEventHandle = CreateEvent(NULL, TRUE, FALSE, NULL);
}

//----------------------------------------------------------------------------------------
AudioStream::~AudioStream()
{
	//Terminate the audio output stream, and free all resources
	Close();
	
	//Release our critical section object
	DeleteCriticalSection(&waveMutex);
}

//----------------------------------------------------------------------------------------
//Audio stream binding
//----------------------------------------------------------------------------------------
bool AudioStream::Open(unsigned int achannelCount, unsigned int abitsPerSample, unsigned int asamplesPerSec, unsigned int amaxPendingSamples, unsigned int amaxPlayingSamples)
{
	//If the stream already has an open handle to an audio device, close it.
	Close();

	//Set the properties of the audio output stream
	channelCount = achannelCount;
	bitsPerSample = abitsPerSample;
	samplesPerSec = asamplesPerSec;
	maxPendingSamples = amaxPendingSamples;
	if(maxPendingSamples == 0)
	{
		maxPendingSamples = samplesPerSec * 2;
	}
	maxPlayingSamples = amaxPlayingSamples;
	if(maxPlayingSamples == 0)
	{
		maxPlayingSamples = samplesPerSec;
	}
	currentPlayingSamples = 0;

	//Create the worker thread, and boost priority.
	ResetEvent(startupCompleteEventHandle);
	DWORD threadID;
	HANDLE renderThreadHandle = CreateThread(NULL, 0, WorkerThread, this, CREATE_SUSPENDED, &threadID);
	if(renderThreadHandle == NULL)
	{
		//##DEBUG##
		std::wcout << "AudioStream Error!:\tFailed to create worker thread" << GetLastError() << '\n';
		return false;
	}
	SetThreadPriority(renderThreadHandle, THREAD_PRIORITY_TIME_CRITICAL);
	if(ResumeThread(renderThreadHandle) == (DWORD)-1)
	{
		TerminateThread(renderThreadHandle, 0);
		//##DEBUG##
		std::wcout << "AudioStream Error!:\tFailed to resume worker thread" << GetLastError() << '\n';
		return false;
	}

	//Verify the worker thread has started correctly, and is ready to process audio data.
	WaitForSingleObject(startupCompleteEventHandle, INFINITE);
	if(!workerThreadRunning)
	{
		//##DEBUG##
		std::wcout << "AudioStream Error!:\tWorker thread initialization failed" << '\n';
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
void AudioStream::Close()
{
	//If the worker thread is currently marked as running, send a shutdown event
	//notification, and wait for the worker thread to terminate.
	if(workerThreadRunning)
	{
		SetEvent(eventHandles[EVENT_SHUTDOWN]);
		WaitForSingleObject(shutdownCompleteEventHandle, INFINITE);
		ResetEvent(shutdownCompleteEventHandle);
	}

	//Verify that all playing buffer objects have been deleted. This should always be the
	//case by this point anyway.
	if(playingBuffers.size() > 0)
	{
		//##DEBUG##
		std::wcout << "AudioStream Error!:\tEntries remain in playingBuffers on Close\t" << playingBuffers.size() << '\n';
	}
	for(std::list<AudioBuffer*>::iterator i = playingBuffers.begin(); i != playingBuffers.end(); ++i)
	{
		delete *i;
	}
	playingBuffers.clear();

	//Remove any remaining pending buffer objects
	for(std::list<AudioBuffer*>::iterator i = pendingBuffers.begin(); i != pendingBuffers.end(); ++i)
	{
		delete *i;
	}
	pendingBuffers.clear();
}

//----------------------------------------------------------------------------------------
//Buffer management functions
//----------------------------------------------------------------------------------------
AudioStream::AudioBuffer* AudioStream::CreateAudioBuffer(unsigned int sampleCount, unsigned int achannelCount)
{
	if(!workerThreadRunning || (sampleCount <= 0) || (achannelCount <= 0))
	{
		return 0;
	}
	EnterCriticalSection(&waveMutex);

	//Create a new AudioBuffer object, and add it to the pending buffer queue.
	AudioBuffer* entry = new AudioBuffer(sampleCount * achannelCount);
	pendingBuffers.push_back(entry);

	//Calculate the new total number of samples pending, and if we've exceeded our
	//maximum pending sample count, drop pending buffers until we're back under the
	//limit. Note that buffers are dropped starting with the oldest entry in the queue,
	//and buffers are only dropped after they have been handed back to the AudioStream
	//for playback.
	unsigned int pendingSampleCount = 0;
	for(std::list<AudioBuffer*>::iterator i = pendingBuffers.begin(); i != pendingBuffers.end(); ++i)
	{
		pendingSampleCount += (unsigned int)((*i)->buffer.size() / channelCount);
	}
	std::list<AudioBuffer*>::iterator i = pendingBuffers.begin();
	while((pendingSampleCount > maxPendingSamples) && (i != pendingBuffers.end()) && (*i)->playBuffer)
	{
		pendingSampleCount -= (unsigned int)((*i)->buffer.size() / channelCount);
		pendingBuffers.erase(i);
		i = pendingBuffers.begin();
	}

	LeaveCriticalSection(&waveMutex);
	return entry;
}

//----------------------------------------------------------------------------------------
void AudioStream::DeleteAudioBuffer(AudioBuffer* buffer)
{
	EnterCriticalSection(&waveMutex);
	std::list<AudioBuffer*>::iterator i = pendingBuffers.begin();
	bool done = false;
	while(!done && (i != pendingBuffers.end()));
	{
		if(*i == buffer)
		{
			pendingBuffers.erase(i);
			delete buffer;
			done = true;
		}
	}
	LeaveCriticalSection(&waveMutex);
}

//----------------------------------------------------------------------------------------
void AudioStream::PlayBuffer(AudioBuffer* buffer)
{
	EnterCriticalSection(&waveMutex);
	buffer->playBuffer = true;
	LeaveCriticalSection(&waveMutex);
	SetEvent(eventHandles[EVENT_PLAYBUFFER]);
}

//----------------------------------------------------------------------------------------
void AudioStream::AddPendingBuffers(HWAVEOUT deviceHandle)
{
	EnterCriticalSection(&waveMutex);
	std::list<AudioBuffer*>::iterator i = pendingBuffers.begin();
	while((i != pendingBuffers.end()) && (*i)->playBuffer && (currentPlayingSamples < maxPlayingSamples))	// && (playingBuffers.size() < maxPlayingBufferCount))
	{
		AudioBuffer* entry = *i;
		LeaveCriticalSection(&waveMutex);

		//##DEBUG##
//		if(channelCount > 1)
//		{
//			std::wstringstream message;
//			message << L"AddBuffer\n";
//			std::wcout << message.str();
//		}

		entry->header.dwFlags = 0;
		entry->header.lpData = (LPSTR)&entry->buffer[0];
		entry->header.dwBufferLength = (DWORD)entry->buffer.size() * sizeof(entry->buffer[0]);

		MMRESULT waveOutPrepareHeaderReturn;
		waveOutPrepareHeaderReturn = waveOutPrepareHeader(deviceHandle, &entry->header, sizeof(entry->header));
		if(waveOutPrepareHeaderReturn != MMSYSERR_NOERROR)
		{
			//##DEBUG##
			std::wcout << "AudioStream Error!:\twaveOutPrepareHeader failed!\t" << waveOutPrepareHeaderReturn << '\n';
		}

		MMRESULT waveOutWriteReturn;
		waveOutWriteReturn = waveOutWrite(deviceHandle, &entry->header, sizeof(entry->header));
		if(waveOutWriteReturn != MMSYSERR_NOERROR)
		{
			//##DEBUG##
			std::wcout << "AudioStream Error!:\twaveOutWrite failed!\t" << waveOutWriteReturn << '\n';
		}

		EnterCriticalSection(&waveMutex);
		//Note that at this point, a new entry could have been added to the back of the
		//pendingBuffers queue while we were outside the lock.
		currentPlayingSamples += (unsigned int)entry->buffer.size();
		playingBuffers.push_back(entry);
		pendingBuffers.erase(pendingBuffers.begin());
		i = pendingBuffers.begin();
	}
	LeaveCriticalSection(&waveMutex);
}

//----------------------------------------------------------------------------------------
void AudioStream::ClearCompletedBuffers(HWAVEOUT deviceHandle)
{
	EnterCriticalSection(&waveMutex);
	std::list<AudioBuffer*>::iterator i = playingBuffers.begin();
	while((completedBufferSlots > 0) && (i != playingBuffers.end()))
	{
		AudioBuffer* entry = *i;
		LeaveCriticalSection(&waveMutex);

		//##DEBUG##
//		if(channelCount > 1)
//		{
//			std::wstringstream message;
//			message << L"ClearBuffer\t" << completedBufferSlots << '\n';
//			std::wcout << message.str();
//		}

		MMRESULT waveOutUnprepareHeaderReturn;
		waveOutUnprepareHeaderReturn = waveOutUnprepareHeader(deviceHandle, &entry->header, sizeof(entry->header));
		if(waveOutUnprepareHeaderReturn != MMSYSERR_NOERROR)
		{
			//##DEBUG##
			std::wcout << "AudioStream Error!:\twaveOutUnprepareHeader failed!\t" << waveOutUnprepareHeaderReturn << '\n';
		}

		EnterCriticalSection(&waveMutex);
		//Note that at this point, the value of completedBufferSlots could have increased
		//while we were outside the lock.
		currentPlayingSamples -= (unsigned int)entry->buffer.size();
		delete entry;
		playingBuffers.erase(playingBuffers.begin());
		i = playingBuffers.begin();
		--completedBufferSlots;
	}
	LeaveCriticalSection(&waveMutex);
}

//----------------------------------------------------------------------------------------
//Worker thread functions
//----------------------------------------------------------------------------------------
DWORD WINAPI AudioStream::WorkerThread(LPVOID lpParameter)
{
	AudioStream* stream = (AudioStream*)lpParameter;
	stream->completedBufferSlots = 0;

	//Reset all event handles
	ResetEvent(stream->eventHandles[EVENT_SHUTDOWN]);
	ResetEvent(stream->eventHandles[EVENT_PLAYBUFFER]);
	ResetEvent(stream->eventHandles[EVENT_BUFFERDONE]);

	//##DEBUG##
	unsigned int deviceCount = waveOutGetNumDevs();
	for(unsigned int i = 0; i < deviceCount; ++i)
	{
		WAVEOUTCAPSW waveOutCaps;
		MMRESULT waveOutGetDevCapsReturn;
		waveOutGetDevCapsReturn = waveOutGetDevCaps(i, &waveOutCaps, sizeof(waveOutCaps));
		if(waveOutGetDevCapsReturn != MMSYSERR_NOERROR)
		{
			//##DEBUG##
			std::wcout << "AudioStream Error!:\twaveOutGetDevCaps failed!\t" << i << '\t' << waveOutGetDevCapsReturn << '\n';
		}
		else
		{
			std::wcout << "Audio:\t" << waveOutCaps.szPname << '\t' << std::hex << waveOutCaps.dwFormats << '\n';
		}
	}

	//Calculate the number of bytes per sample, rounded up.
	unsigned int bytesPerSample = (stream->bitsPerSample + 7) / 8;

	//Describe the required format of the audio output stream
	WAVEFORMATEX format;
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nChannels = (WORD)stream->channelCount;
	format.nSamplesPerSec = (DWORD)stream->samplesPerSec;
	format.wBitsPerSample = (WORD)stream->bitsPerSample;
	format.nBlockAlign = format.nChannels * (WORD)bytesPerSample;
	format.nAvgBytesPerSec = format.nSamplesPerSec * (DWORD)format.nBlockAlign;
	format.cbSize = 0;

	//WAVEFORMATEXTENSIBLE formatEx;
	//formatEx.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
	//formatEx.Format.nChannels = (WORD)stream->channelCount;
	//formatEx.Format.nSamplesPerSec = (DWORD)stream->samplesPerSec;
	//formatEx.Format.wBitsPerSample = (WORD)(bytesPerSample * 8);
	//formatEx.Format.nBlockAlign = formatEx.Format.nChannels * bytesPerSample;
	//formatEx.Format.nAvgBytesPerSec = formatEx.Format.nSamplesPerSec * (DWORD)formatEx.Format.nBlockAlign;
	//formatEx.Format.cbSize = 22;
	//formatEx.Samples.wReserved = 0;
	//formatEx.Samples.wValidBitsPerSample = (WORD)stream->bitsPerSample;
	//formatEx.dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;
	//formatEx.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;

	//Open a handle to a wave output device which can play audio data matching the
	//required format.
	HWAVEOUT deviceHandle;
	MMRESULT waveOutOpenReturn;
	waveOutOpenReturn = waveOutOpen(&deviceHandle, WAVE_MAPPER, (WAVEFORMATEX*)&format, (DWORD_PTR)WaveOutCallback, (DWORD_PTR)stream, CALLBACK_FUNCTION);
	if(waveOutOpenReturn != MMSYSERR_NOERROR)
	{
		//##DEBUG##
		std::wcout << "AudioStream Error!:\twaveOutOpen failed!\t" << waveOutOpenReturn << '\n';

		//The worker thread initialization has failed. Signal that the worker thread has
		//processed the startup request, but has not successfully entered the running
		//state, and terminate the worker thread.
		stream->workerThreadRunning = false;
		SetEvent(stream->startupCompleteEventHandle);
		return 0;
	}

	//Signal that the worker thread has entered the running state
	stream->workerThreadRunning = true;
	SetEvent(stream->startupCompleteEventHandle);

	//Begin the message loop
	bool done = false;
	while(!done)
	{
		DWORD waitForMultipleObjectsReturn;
		waitForMultipleObjectsReturn = WaitForMultipleObjects(3, &stream->eventHandles[0], FALSE, INFINITE);
		unsigned int eventID = waitForMultipleObjectsReturn - WAIT_OBJECT_0;
		ResetEvent(stream->eventHandles[eventID]);
		switch(eventID)
		{
		case EVENT_SHUTDOWN:
			done = true;
		break;
		case EVENT_BUFFERDONE:
			//##DEBUG##
//			if(stream->channelCount > 1)
//			{
//				std::wstringstream message;
//				message << L"BufferDone:\t" << stream->pendingBuffers.size() << '\t' << stream->playingBuffers.size() << '\t' << stream->completedBufferSlots << '\n';
//				std::wcout << message.str();
//			}

			stream->ClearCompletedBuffers(deviceHandle);
			stream->AddPendingBuffers(deviceHandle);
		break;
		case EVENT_PLAYBUFFER:
			//##DEBUG##
//			if(stream->channelCount > 1)
//			{
//				std::wstringstream message;
//				message << L"PlayBuffer:\t" << stream->pendingBuffers.size() << '\t' << stream->playingBuffers.size() << '\t' << stream->completedBufferSlots << '\n';
//				std::wcout << message.str();
//			}

			stream->AddPendingBuffers(deviceHandle);
		break;
		}
	}

	//Cancel all currently playing buffers, and close the handle to the wave output
	//device.
	waveOutReset(deviceHandle);
	stream->ClearCompletedBuffers(deviceHandle);
	waveOutClose(deviceHandle);

	//Signal that the worker thread has terminated
	stream->workerThreadRunning = false;
	SetEvent(stream->shutdownCompleteEventHandle);
	return 0;
}

//----------------------------------------------------------------------------------------
void CALLBACK AudioStream::WaveOutCallback(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	AudioStream* stream = (AudioStream*)dwInstance;
	if(uMsg == WOM_DONE)
	{
		EnterCriticalSection(&stream->waveMutex);
		++stream->completedBufferSlots;
		SetEvent(stream->eventHandles[EVENT_BUFFERDONE]);
		LeaveCriticalSection(&stream->waveMutex);
	}
}

//----------------------------------------------------------------------------------------
//Sample rate conversion
//----------------------------------------------------------------------------------------
void AudioStream::ConvertSampleRate(std::vector<short>& sourceData, unsigned int sourceSampleCount, unsigned int achannelCount, std::vector<short>& targetData, unsigned int targetSampleCount) const
{
	targetData.resize(targetSampleCount * achannelCount);
	for(unsigned int channelNo = 0; channelNo < achannelCount; ++channelNo)
	{
		for(unsigned int i = 0; i < targetSampleCount; ++i)
		{
			//Find the sample in the source buffer which comes immediately before the target
			//output time for the next sample.
	//		float sourceSamplePosition = (float)sourceSampleCount * ((float)i / (float)targetSampleCount);
	//		unsigned int sourceSampleIndex = (unsigned int)sourceSamplePosition;
	//		float sourceSampleOffset = (sourceSamplePosition - (float)sourceSampleIndex);

			//Read the next sample from the source buffer, and interpolate the two to produce
			//the output sample. We do a mod of the index for the second sample to deal with
			//cases where sourceSampleOffset = 0 at the end of the source buffer, such as
			//when the source and target sample rates are the same. Without this check, we
			//could trigger an access violation. In this case the value we read is assigned a
			//weighting of zero, so it doesn't matter what value we get.
	//		short sample1 = sourceData[channelNo + (sourceSampleIndex * achannelCount)];
	//		short sample2 = sourceData[(channelNo + ((sourceSampleIndex + 1) * achannelCount) % (sourceSampleCount * achannelCount)];
	//		short interpolatedSample = sample1 + (short)((float)(sample2 - sample1) * sourceSampleOffset);

			//This more sophisticated method is capable of either upsampling or downsampling
			//the data accurately
			float firstSamplePoint = (float)i * ((float)sourceSampleCount / (float)targetSampleCount);
			float lastSamplePoint = (float)((i + 1) % targetSampleCount) * ((float)sourceSampleCount / (float)targetSampleCount);
			unsigned int firstSampleIndex = (unsigned int)firstSamplePoint;
			unsigned int lastSampleIndex = (unsigned int)lastSamplePoint;
			float totalDomain = lastSamplePoint - firstSamplePoint;
			float finalSample = 0.0f;
			for(unsigned int currentSample = firstSampleIndex; currentSample <= lastSampleIndex; ++currentSample)
			{
				float sampleWeight = 1.0f;
				if(currentSample == firstSampleIndex)
				{
					sampleWeight -= firstSamplePoint - (float)firstSampleIndex;
				}
				if(currentSample == lastSampleIndex)
				{
					sampleWeight -= 1.0f - (lastSamplePoint - (float)lastSampleIndex);
				}
				finalSample += (float)sourceData[channelNo + (currentSample * achannelCount)] * sampleWeight;
			}
			finalSample /= totalDomain;
			short interpolatedSample = (short)finalSample;

			//Write the interpolated value to the output buffer
			targetData[channelNo + (i * achannelCount)] = interpolatedSample;
		}
	}
}
