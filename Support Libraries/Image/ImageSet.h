#ifndef __IMAGESET_H__
#define __IMAGESET_H__
#include "ImageInterface/ImageInterface.pkg"
#include <vector>

//##TODO## Finish implementing this class
class ImageSet :public IImageSet
{
public:
	//Constructors
	virtual ~ImageSet();

	//Contained image functions
	virtual unsigned int GetImageCount() const;
	virtual IImage* GetImageEntry(unsigned int imageNo) const;
	virtual void RemoveImage(unsigned int imageNo);
	virtual unsigned int AddImageEntry(IImage* image);

	//Generic load functions
	virtual bool LoadImageSet(Stream::IStream& stream);

	//ICO functions
	virtual bool LoadICOImageSet(Stream::IStream& stream);
	virtual bool SaveICOImageSet(Stream::IStream& stream);

	//CUR functions
	virtual bool LoadCURImageSet(Stream::IStream& stream);
	virtual bool SaveCURImageSet(Stream::IStream& stream);

private:
	std::vector<IImage*> images;
};

#endif
