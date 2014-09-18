#include "AudioStream.h"
#include <Mmreg.h>
#include <Ks.h>
#include <Ksmedia.h>
//##DEBUG##
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
bool AudioStream::Open(unsigned int achannelCount, unsigned int abitsPerSample, unsigned int asamplesPerSec, unsigned int amaxPendingSamples, unsigned int aminPlayingSamples)
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
	minPlayingSamples = aminPlayingSamples;
	if(minPlayingSamples == 0)
	{
		minPlayingSamples = samplesPerSec;
	}
	currentPlayingSamples = 0;

	//Create the worker thread, and boost priority.
	ResetEvent(startupCompleteEventHandle);
	DWORD threadID;
	HANDLE renderThreadHandle = CreateThread(NULL, 0, WorkerThread, this, CREATE_SUSPENDED, &threadID);
	if(renderThreadHandle == NULL)
	{
		//##DEBUG##
		std::wcout << "AudioStream Error!:\tFailed to create worker thread!\t" << GetLastError() << '\n';
		return false;
	}
	SetThreadPriority(renderThreadHandle, THREAD_PRIORITY_TIME_CRITICAL);
	if(ResumeThread(renderThreadHandle) == (DWORD)-1)
	{
		TerminateThread(renderThreadHandle, 0);
		//##DEBUG##
		std::wcout << "AudioStream Error!:\tFailed to resume worker thread!\t" << GetLastError() << '\n';
		return false;
	}

	//Verify the worker thread has started correctly, and is ready to process audio data.
	WaitForSingleObject(startupCompleteEventHandle, INFINITE);
	if(!workerThreadRunning)
	{
		//##DEBUG##
		std::wcout << "AudioStream Error!:\tWorker thread initialization failed!" << '\n';
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
	//Ensure that the audio output stream has been opened, and that valid number of
	//samples and channels have been specified for this buffer.
	if(!workerThreadRunning || (sampleCount <= 0) || (achannelCount <= 0))
	{
		return 0;
	}

	//Create a new AudioBuffer object
	AudioBuffer* entry = new AudioBuffer(sampleCount * achannelCount);

	//Take a lock on waveMutex before we alter any internal structures
	EnterCriticalSection(&waveMutex);

	//Add the new buffer object to the pending buffer queue
	pendingBuffers.push_back(entry);

	//Calculate the new total number of samples pending, and if we've exceeded our maximum
	//pending sample count, drop pending buffers until we're back under the limit. Note
	//that buffers are dropped starting with the oldest entry in the queue, and buffers
	//are only dropped after they have been handed back to the AudioStream for playback,
	//but before they are sent to the audio hardware. We drop buffers so that the audio
	//output stream isn't left with an ever-increasing backlog of audio data in the case
	//that the system is running too fast.
	unsigned int pendingSampleCount = currentPlayingSamples;
	for(std::list<AudioBuffer*>::iterator i = pendingBuffers.begin(); i != pendingBuffers.end(); ++i)
	{
		pendingSampleCount += (unsigned int)((*i)->buffer.size() / channelCount);
	}
	std::list<AudioBuffer*>::iterator pendingBufferIterator = pendingBuffers.begin();
	while((pendingSampleCount > maxPendingSamples) && (pendingBufferIterator != pendingBuffers.end()) && (*pendingBufferIterator)->playBuffer)
	{
		//If this buffer entry has already been sent to the audio hardware, skip it.
		AudioBuffer* entryToRemove = *pendingBufferIterator;
		if(entryToRemove->bufferSentToAudioDevice)
		{
			++pendingBufferIterator;
			continue;
		}

		//Remove this buffer entry
		pendingSampleCount -= ((unsigned int)entryToRemove->buffer.size() / channelCount);
		delete entryToRemove;
		pendingBuffers.erase(pendingBufferIterator);
		pendingBufferIterator = pendingBuffers.begin();
	}

	//Release the lock on waveMutex
	LeaveCriticalSection(&waveMutex);

	//Return the new buffer object to the caller
	return entry;
}

//----------------------------------------------------------------------------------------
void AudioStream::DeleteAudioBuffer(AudioBuffer* buffer)
{
	//Find and delete this buffer from the list of pending buffers
	EnterCriticalSection(&waveMutex);
	std::list<AudioBuffer*>::iterator pendingBufferIterator = pendingBuffers.begin();
	bool done = false;
	while(!done && (pendingBufferIterator != pendingBuffers.end()))
	{
		if(*pendingBufferIterator == buffer)
		{
			pendingBuffers.erase(pendingBufferIterator);
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

	//Send all pending buffers which have been flagged for playback to the audio output
	//stream, until we run out of buffers, or we reach the maximum number of samples which
	//are allowed to be held pending.
	std::list<AudioBuffer*>::iterator pendingBufferIterator = pendingBuffers.begin();
	while((pendingBufferIterator != pendingBuffers.end()) && (*pendingBufferIterator)->playBuffer && (currentPlayingSamples < maxPendingSamples))
	{
		AudioBuffer* entry = *pendingBufferIterator;

		//Flag that this buffer has been sent to the audio device
		entry->bufferSentToAudioDevice = true;

		//Add the count of samples in this buffer to the total count of currently playing
		//samples
		unsigned int samplesInBufferEntry = ((unsigned int)entry->buffer.size() / channelCount);
		currentPlayingSamples += samplesInBufferEntry;

		//Add this buffer to the list of playing buffers
		playingBuffers.push_back(entry);

		//Send this buffer to the audio output device
		LeaveCriticalSection(&waveMutex);
		bool addPendingBufferReturn = AddPendingBuffer(deviceHandle, entry);
		EnterCriticalSection(&waveMutex);

		//If we failed to send this buffer to the audio output device, remove it from the
		//list of playing buffers, and delete the buffer data. Note that we do things this
		//way around as we want to ensure that a buffer is correctly entered into our
		//internal management structures before it is sent for playback. This isn't
		//strictly required right now, but it's good practice, as we didn't hold a lock on
		//waveMutex while the audio buffer was being added, so in theory, the buffer could
		//have started and even finished playing before we reach this point in code. In
		//case we want to do something in our WaveOutCallback function in the future that
		//relies on the relevant buffer being setup in our data structures already, we
		//make sure it is valid before the buffer starts playing, and remove it in the
		//case of a failure.
		if(!addPendingBufferReturn)
		{
			currentPlayingSamples -= samplesInBufferEntry;
			playingBuffers.pop_back();
			delete entry;
		}

		//Erase the buffer we just added from the list of pending buffers, and check if
		//another pending buffer exists. Note that at this point, a new entry could have
		//been added to the back of the pendingBuffers queue while we were outside the
		//critical section.
		pendingBuffers.erase(pendingBuffers.begin());
		pendingBufferIterator = pendingBuffers.begin();
	}

	//If we have less than the maximum number of playing samples waiting in the playback
	//buffer, and at least one buffer is currently being played, add a filler buffer to
	//the queue using the last sample of the last playing buffer.
	if(!playingBuffers.empty() && (currentPlayingSamples < minPlayingSamples))
	{
		//Extract the last sample values from the last buffer in the playback queue
		AudioBuffer* lastPlayingBuffer = *playingBuffers.rbegin();
		std::vector<short> sampleBuffer(channelCount);
		for(unsigned int i = 0; i < channelCount; ++i)
		{
			unsigned int lastPlayingBufferSize = (unsigned int)lastPlayingBuffer->buffer.size();
			unsigned int lastPlayingBufferSampleIndex = ((lastPlayingBufferSize - 1) - ((channelCount - 1) - i));
			sampleBuffer[i] = lastPlayingBuffer->buffer[lastPlayingBufferSampleIndex];
		}

		//While there's still empty space in the playback buffer, insert filler buffers
		//into the playback queue.
		while(currentPlayingSamples < minPlayingSamples)
		{
			unsigned int maxSamplesPerFillerBuffer = minPlayingSamples / 2;
			unsigned int sampleCountToAdd = ((minPlayingSamples - currentPlayingSamples) <= maxSamplesPerFillerBuffer)? (minPlayingSamples - currentPlayingSamples): maxSamplesPerFillerBuffer;

			//##DEBUG##
			//std::wcout << "Adding filler sample buffer with " << sampleCountToAdd << " samples.\n";

			//Create a new audio buffer for this filler block
			AudioBuffer* fillerBuffer = new AudioBuffer(sampleCountToAdd * channelCount);

			//Fill this audio buffer with the captured sample data from the last playing
			//audio buffer
			for(unsigned int sampleNo = 0; sampleNo < sampleCountToAdd; ++sampleNo)
			{
				for(unsigned int channelNo = 0; channelNo < channelCount; ++channelNo)
				{
					fillerBuffer->buffer[(sampleNo * channelCount) + channelNo] = sampleBuffer[channelNo];
				}
			}

			//Flag that playback has been requested for this buffer
			fillerBuffer->playBuffer = true;

			//Flag that this buffer has been sent to the audio device
			fillerBuffer->bufferSentToAudioDevice = true;

			//Add the number of inserted samples in the filler buffer to the total count
			//of playing samples
			currentPlayingSamples += sampleCountToAdd;

			//Add this buffer to the list of playing buffers
			playingBuffers.push_back(fillerBuffer);

			//Send this buffer to the audio output device
			LeaveCriticalSection(&waveMutex);
			bool addPendingBufferReturn = AddPendingBuffer(deviceHandle, fillerBuffer);
			EnterCriticalSection(&waveMutex);

			//If we failed to send this buffer to the audio output device, remove it from
			//the list of playing buffers, and delete the buffer data. Note that we do
			//things this way around as we want to ensure that a buffer is correctly
			//entered into our internal management structures before it is sent for
			//playback. This isn't strictly required right now, but it's good practice, as
			//we didn't hold a lock on waveMutex while the audio buffer was being added,
			//so in theory, the buffer could have started and even finished playing before
			//we reach this point in code. In case we want to do something in our
			//WaveOutCallback function in the future that relies on the relevant buffer
			//being setup in our data structures already, we make sure it is valid before
			//the buffer starts playing, and remove it in the case of a failure.
			if(!addPendingBufferReturn)
			{
				currentPlayingSamples -= sampleCountToAdd;
				playingBuffers.pop_back();
				delete fillerBuffer;
			}
		}
	}

	LeaveCriticalSection(&waveMutex);
}

//----------------------------------------------------------------------------------------
bool AudioStream::AddPendingBuffer(HWAVEOUT deviceHandle, AudioBuffer* entry)
{
	//##DEBUG##
	//std::wcout << L"AddPendingBuffer\n";

	//Populate the header structure for this audio buffer
	entry->header.dwFlags = 0;
	entry->header.lpData = (LPSTR)&entry->buffer[0];
	entry->header.dwBufferLength = (DWORD)entry->buffer.size() * sizeof(entry->buffer[0]);

	//Attempt to correctly prepare this buffer for playback
	MMRESULT waveOutPrepareHeaderReturn;
	waveOutPrepareHeaderReturn = waveOutPrepareHeader(deviceHandle, &entry->header, sizeof(entry->header));
	if(waveOutPrepareHeaderReturn != MMSYSERR_NOERROR)
	{
		//##DEBUG##
		std::wcout << "AudioStream Error!:\twaveOutPrepareHeader failed!\t" << waveOutPrepareHeaderReturn << '\n';
		return false;
	}

	//Write the audio buffer to the audio output device
	MMRESULT waveOutWriteReturn;
	waveOutWriteReturn = waveOutWrite(deviceHandle, &entry->header, sizeof(entry->header));
	if(waveOutWriteReturn != MMSYSERR_NOERROR)
	{
		//##DEBUG##
		std::wcout << "AudioStream Error!:\twaveOutWrite failed!\t" << waveOutWriteReturn << '\n';
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
void AudioStream::ClearCompletedBuffers(HWAVEOUT deviceHandle)
{
	//While there are completed buffers waiting for deletion, clean up the next completed
	//buffer in the queue.
	EnterCriticalSection(&waveMutex);
	std::list<AudioBuffer*>::iterator playingBufferIterator = playingBuffers.begin();
	while((completedBufferSlots > 0) && (playingBufferIterator != playingBuffers.end()))
	{
		AudioBuffer* entry = *playingBufferIterator;

		//##DEBUG##
		//std::wcout << L"ClearBuffer\t" << completedBufferSlots << '\n';

		//Clean up all data associated with this buffer
		LeaveCriticalSection(&waveMutex);
		MMRESULT waveOutUnprepareHeaderReturn;
		waveOutUnprepareHeaderReturn = waveOutUnprepareHeader(deviceHandle, &entry->header, sizeof(entry->header));
		if(waveOutUnprepareHeaderReturn != MMSYSERR_NOERROR)
		{
			//##DEBUG##
			std::wcout << "AudioStream Error!:\twaveOutUnprepareHeader failed!\t" << waveOutUnprepareHeaderReturn << '\n';
			std::wcout << entry->buffer.size() << '\t' << entry->header.dwFlags << '\n';
		}
		EnterCriticalSection(&waveMutex);

		//Subtract one from the number of completed buffers, and adjust the count of the
		//currently playing samples. Note that the number of completed buffers in
		//particular could have increased while we were outside our lock.
		--completedBufferSlots;
		currentPlayingSamples -= ((unsigned int)entry->buffer.size() / channelCount);

		//Delete the completed buffer, and advance to the new oldest entry in the list of
		//playing buffers.
		delete entry;
		playingBuffers.erase(playingBuffers.begin());
		playingBufferIterator = playingBuffers.begin();
	}
	LeaveCriticalSection(&waveMutex);
}

//----------------------------------------------------------------------------------------
//Worker thread functions
//----------------------------------------------------------------------------------------
DWORD WINAPI AudioStream::WorkerThread(LPVOID lpParameter)
{
	AudioStream* stream = (AudioStream*)lpParameter;

	//Reset the count of completed buffers awaiting deletion
	stream->completedBufferSlots = 0;

	//Reset all event handles
	ResetEvent(stream->eventHandles[EVENT_SHUTDOWN]);
	ResetEvent(stream->eventHandles[EVENT_PLAYBUFFER]);
	ResetEvent(stream->eventHandles[EVENT_BUFFERDONE]);

	//##DEBUG## Report on the available audio output devices
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
			//std::wcout << L"BufferDone:\t" << stream->pendingBuffers.size() << '\t' << stream->playingBuffers.size() << '\t' << stream->completedBufferSlots << '\n';

			stream->ClearCompletedBuffers(deviceHandle);
			stream->AddPendingBuffers(deviceHandle);
		break;
		case EVENT_PLAYBUFFER:
			//##DEBUG##
			//std::wcout << L"PlayBuffer:\t" << stream->pendingBuffers.size() << '\t' << stream->playingBuffers.size() << '\t' << stream->completedBufferSlots << '\n';

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

		//Increment the count of completed audio buffers awaiting deletion
		++stream->completedBufferSlots;

		//Raise an event to indicate that there's at least one completed buffer awaiting
		//deletion. Note that we do this inside the lock here so that the worker thread
		//recieves the event as soon as possible.
		SetEvent(stream->eventHandles[EVENT_BUFFERDONE]);

		LeaveCriticalSection(&stream->waveMutex);
	}
}

//----------------------------------------------------------------------------------------
//Sample rate conversion
//----------------------------------------------------------------------------------------
void AudioStream::ConvertSampleRate(std::vector<short>& sourceData, unsigned int sourceSampleCount, unsigned int achannelCount, std::vector<short>& targetData, unsigned int targetSampleCount) const
{
	//Perform a linear resampling of the source sample data to the requested sample rate
	targetData.resize(targetSampleCount * achannelCount);
	for(unsigned int channelNo = 0; channelNo < achannelCount; ++channelNo)
	{
		float sampleConversionRatio = (float)sourceSampleCount / (float)targetSampleCount;
		for(unsigned int targetSampleNo = 0; targetSampleNo < targetSampleCount; ++targetSampleNo)
		{
			//Calculate the beginning and end of the sample region in the source buffer,
			//which is being mapped onto this output sample value in the target buffer.
			//Note that because we're adding 1 to the target sample location, the second
			//sample point may be past the end of the buffer, but this is ok, because this
			//sample point is really a limit. We protect against reading past the
			//boundaries of the buffer further below, and if this is attempted, this dud
			//sample will be assigned a weight of zero.
			float firstSamplePoint = (float)targetSampleNo * sampleConversionRatio;
			float lastSamplePoint = (float)(targetSampleNo + 1) * sampleConversionRatio;

			//Calculate the total domain, or length, of this sample region.
			float totalDomain = lastSamplePoint - firstSamplePoint;

			//Calculate the first and last values of interest from the source region
			unsigned int firstSamplePos = (unsigned int)firstSamplePoint;
			unsigned int lastSamplePos = (unsigned int)lastSamplePoint;

			//Combine sample values from the source buffer, with their respective
			//weightings for each value.
			float finalSample = 0.0f;
			for(unsigned int sourceSampleNo = firstSamplePos; sourceSampleNo <= lastSamplePos; ++sourceSampleNo)
			{
				float sampleStartPoint = 0.0f;
				if(sourceSampleNo == firstSamplePos)
				{
					sampleStartPoint = firstSamplePoint - (float)firstSamplePos;
				}
				float sampleEndPoint = 1.0f;
				if(sourceSampleNo == lastSamplePos)
				{
					sampleEndPoint = lastSamplePoint - (float)lastSamplePos;
				}
				float sampleWeight = sampleEndPoint - sampleStartPoint;

				float sample = (float)sourceData[channelNo + ((sourceSampleNo % sourceSampleCount) * achannelCount)];
				finalSample += sample * sampleWeight;
			}

			//Normalize the sample value back to a single value, by dividing it by the
			//total length of the sample region in the source buffer.
			finalSample /= totalDomain;

			//Write the interpolated value to the output buffer
			short interpolatedSample = (short)finalSample;
			targetData[channelNo + (targetSampleNo * achannelCount)] = interpolatedSample;
		}
	}
}
