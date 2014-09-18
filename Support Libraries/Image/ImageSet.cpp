#include "ImageSet.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ImageSet::~ImageSet()
{
	//Delete any images contained in this image set
	for(size_t i = 0; i < images.size(); ++i)
	{
		delete images[i];
	}
}

//----------------------------------------------------------------------------------------
//Contained image functions
//----------------------------------------------------------------------------------------
unsigned int ImageSet::GetImageCount() const
{
	return (unsigned int)images.size();
}

//----------------------------------------------------------------------------------------
IImage* ImageSet::GetImageEntry(unsigned int imageNo) const
{
	//Ensure the requested image number is valid
	if(imageNo >= (unsigned int)images.size())
	{
		return 0;
	}

	//Return the requested image entry to the caller
	return images[imageNo];
}

//----------------------------------------------------------------------------------------
void ImageSet::RemoveImage(unsigned int imageNo)
{
	images.erase(images.begin() + imageNo);
}

//----------------------------------------------------------------------------------------
unsigned int ImageSet::AddImageEntry(IImage* image)
{
	unsigned int imageIndex = (unsigned int)images.size();
	images.push_back(image);
	return imageIndex;
}

//----------------------------------------------------------------------------------------
//Generic load functions
//----------------------------------------------------------------------------------------
bool ImageSet::LoadImageSet(Stream::IStream& stream)
{
	//##TODO##
	return false;
}

//----------------------------------------------------------------------------------------
//ICO functions
//----------------------------------------------------------------------------------------
bool ImageSet::LoadICOImageSet(Stream::IStream& stream)
{
	//##TODO##
	return false;
}

//----------------------------------------------------------------------------------------
bool ImageSet::SaveICOImageSet(Stream::IStream& stream)
{
	//##TODO##
	return false;
}

//----------------------------------------------------------------------------------------
//CUR functions
//----------------------------------------------------------------------------------------
bool ImageSet::LoadCURImageSet(Stream::IStream& stream)
{
	//##TODO##
	return false;
}

//----------------------------------------------------------------------------------------
bool ImageSet::SaveCURImageSet(Stream::IStream& stream)
{
	//##TODO##
	return false;
}
