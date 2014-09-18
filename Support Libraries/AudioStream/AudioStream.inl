//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct AudioStream::AudioBuffer
{
	AudioBuffer(unsigned int asampleCount)
	:buffer(asampleCount), playBuffer(false), bufferSentToAudioDevice(false)
	{}

	std::vector<short> buffer;
	WAVEHDR header;
	bool playBuffer;
	bool bufferSentToAudioDevice;
};
