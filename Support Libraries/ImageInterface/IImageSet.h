#ifndef __IIMAGESET_H__
#define __IIMAGESET_H__
#include "IImage.h"

class IImageSet
{
public:
	//Constructors
	virtual ~IImageSet() = 0 {}

	//Contained image functions
	virtual unsigned int GetImageCount() const = 0;
	virtual IImage* GetImageEntry(unsigned int imageNo) const = 0;
	virtual void RemoveImage(unsigned int imageNo) = 0;
	virtual unsigned int AddImageEntry(IImage* image) = 0;

	//Generic load functions
	virtual bool LoadImageSet(Stream::IStream& stream) = 0;

	//ICO functions
	virtual bool LoadICOImageSet(Stream::IStream& stream) = 0;
	virtual bool SaveICOImageSet(Stream::IStream& stream) = 0;

	//CUR functions
	virtual bool LoadCURImageSet(Stream::IStream& stream) = 0;
	virtual bool SaveCURImageSet(Stream::IStream& stream) = 0;
};

#endif
