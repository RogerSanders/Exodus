#ifndef __AUDIOSTREAM_H__
#define __AUDIOSTREAM_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include <list>
#include <vector>

class AudioStream
{
public:
	//Structures
	struct AudioBuffer;

	//Constructors
	AudioStream();
	~AudioStream();

	//Audio stream binding
	bool Open(unsigned int achannelCount, unsigned int abitsPerSample, unsigned int asamplesPerSec, unsigned int amaxPendingSamples = 0, unsigned int aminPlayingSamples = 0);
	void Close();

	//Buffer management functions
	AudioBuffer* CreateAudioBuffer(unsigned int sampleCount, unsigned int achannelCount);
	void DeleteAudioBuffer(AudioBuffer* buffer);
	void PlayBuffer(AudioBuffer* buffer);

	//Sample rate conversion
	void ConvertSampleRate(std::vector<short>& sourceData, unsigned int sourceSampleCount, unsigned int achannelCount, std::vector<short>& targetData, unsigned int targetSampleCount) const;

private:
	//Worker thread functions
	static DWORD WINAPI WorkerThread(LPVOID lpParameter);
	static void CALLBACK WaveOutCallback(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD dwParam1, DWORD dwParam2);

	//Buffer management functions
	void AddPendingBuffers(HWAVEOUT deviceHandle);
	bool AddPendingBuffer(HWAVEOUT deviceHandle, AudioBuffer* entry);
	void ClearCompletedBuffers(HWAVEOUT deviceHandle);

private:
	//Audio format settings
	unsigned int channelCount;
	unsigned int bitsPerSample;
	unsigned int samplesPerSec;
	unsigned int maxPendingSamples;

	//Worker thread event information
	static const unsigned int EVENT_SHUTDOWN = 0;
	static const unsigned int EVENT_PLAYBUFFER = 1;
	static const unsigned int EVENT_BUFFERDONE = 2;
	HANDLE eventHandles[3];
	HANDLE startupCompleteEventHandle;
	HANDLE shutdownCompleteEventHandle;
	volatile bool workerThreadRunning;

	//Audio buffer data
	CRITICAL_SECTION waveMutex;
	unsigned int minPlayingSamples;
	volatile unsigned int currentPlayingSamples;
	std::list<AudioBuffer*> pendingBuffers;
	std::list<AudioBuffer*> playingBuffers;
	volatile unsigned int completedBufferSlots;
};

#include "AudioStream.inl"
#endif
