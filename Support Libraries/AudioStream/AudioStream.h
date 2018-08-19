#ifndef __AUDIOSTREAM_H__
#define __AUDIOSTREAM_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include <list>
#include <vector>

class AudioStream
{
public:
	// Structures
	struct AudioBuffer;

	// Constructors
	AudioStream();
	~AudioStream();

	// Audio stream binding
	bool Open(unsigned int channelCount, unsigned int bitsPerSample, unsigned int samplesPerSec, unsigned int maxPendingSamples = 0, unsigned int minPlayingSamples = 0);
	void Close();

	// Buffer management functions
	AudioBuffer* CreateAudioBuffer(unsigned int sampleCount, unsigned int channelCount);
	void DeleteAudioBuffer(AudioBuffer* buffer);
	void PlayBuffer(AudioBuffer* buffer);

	// Sample rate conversion
	static void ConvertSampleRate(const std::vector<short>& sourceData, unsigned int sourceSampleCount, unsigned int channelCount, std::vector<short>& targetData, unsigned int targetSampleCount);

private:
	// Constants
	static const unsigned int EventIndexShutdown = 0;
	static const unsigned int EventIndexPlayBuffer = 1;
	static const unsigned int EventIndexBufferDone = 2;

private:
	// Worker thread functions
	static DWORD WINAPI WorkerThread(LPVOID lpParameter);
	static void CALLBACK WaveOutCallback(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD dwParam1, DWORD dwParam2);

	// Buffer management functions
	void AddPendingBuffers(HWAVEOUT deviceHandle);
	bool AddPendingBuffer(HWAVEOUT deviceHandle, AudioBuffer* entry);
	void ClearCompletedBuffers(HWAVEOUT deviceHandle);

private:
	// Audio format settings
	unsigned int _channelCount;
	unsigned int _bitsPerSample;
	unsigned int _samplesPerSec;
	unsigned int _maxPendingSamples;

	// Worker thread event information
	HANDLE _eventHandles[3];
	HANDLE _startupCompleteEventHandle;
	HANDLE _shutdownCompleteEventHandle;
	volatile bool _workerThreadRunning;

	// Audio buffer data
	CRITICAL_SECTION _waveMutex;
	unsigned int _minPlayingSamples;
	volatile unsigned int _currentPlayingSamples;
	std::list<AudioBuffer*> _pendingBuffers;
	std::list<AudioBuffer*> _playingBuffers;
	volatile unsigned int _completedBufferSlots;
};

#include "AudioStream.inl"
#endif
