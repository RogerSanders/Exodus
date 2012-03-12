/*--------------------------------------------------------------------------------------*\
Things to do:
-The way this class works is very dodgy. Implement this properly. Look into the
"Resource Interchange File Format" in MSDN.
\*--------------------------------------------------------------------------------------*/
#ifndef __WAVFILE_H__
#define __WAVFILE_H__
#include "Stream/Stream.pkg"

class WAVFile
{
public:
	~WAVFile();

	bool Open(const std::wstring& fileName, unsigned int channelCount, unsigned int bitsPerSample, unsigned int samplesPerSec);
	void Close();
	Stream::ViewBinary GetStream();

private:
	Stream::File wavFile;
};

#endif
