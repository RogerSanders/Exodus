//----------------------------------------------------------------------------------------
struct AudioStream::AudioBuffer
{
	AudioBuffer(unsigned int asampleCount)
	:buffer(asampleCount), playBuffer(false)
	{}

	std::vector<short> buffer;
	WAVEHDR header;
	bool playBuffer;
};
