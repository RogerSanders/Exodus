#ifndef __IIMAGE_H__
#define __IIMAGE_H__
#include "StreamInterface/StreamInterface.pkg"
#include "WindowsSupport/WindowsSupport.pkg"

class IImage
{
public:
	//Enumerations
	enum PixelFormat
	{
		PIXELFORMAT_M,     //Monochrome
		PIXELFORMAT_MA,    //Monochrome with alpha
		PIXELFORMAT_RGB,   //Colour
		PIXELFORMAT_RGBA,  //Colour with alpha
	};
	enum DataFormat
	{
		//##TODO## Name these better, perhaps like FLOAT32, INT8, etc.
		//##TODO## Add support for more data formats
		DATAFORMAT_FLOAT,
		DATAFORMAT_8BIT
	};

	//Structures
	union PixelData;

public:
	//Constructors
	virtual ~IImage() = 0 {}

	//Image format functions
	virtual unsigned int GetImageWidth() const = 0;
	virtual unsigned int GetImageHeight() const = 0;
	virtual PixelFormat GetPixelFormat() const = 0;
	virtual DataFormat GetDataFormat() const = 0;
	virtual unsigned int GetDataPlaneCount() const = 0;
	virtual void SetImageFormat(unsigned int aimageWidth, unsigned int aimageHeight, PixelFormat apixelFormat = PIXELFORMAT_RGB, DataFormat adataFormat = DATAFORMAT_8BIT) = 0;

	//Pixel data manipulation
	virtual void GetRawPixelData(unsigned int posX, unsigned int posY, unsigned int planeNo, PixelData& data) const = 0;
	virtual void SetRawPixelData(unsigned int posX, unsigned int posY, unsigned int planeNo, PixelData data) = 0;
	virtual void ReadPixelData(unsigned int posX, unsigned int posY, unsigned int planeNo, float& data) const = 0;
	virtual void ReadPixelData(unsigned int posX, unsigned int posY, unsigned int planeNo, unsigned char& data) const = 0;
	virtual void ReadPixelData(unsigned int posX, unsigned int posY, unsigned int planeNo, unsigned int& data, unsigned int bitCount) const = 0;
	virtual void WritePixelData(unsigned int posX, unsigned int posY, unsigned int planeNo, float data) = 0;
	virtual void WritePixelData(unsigned int posX, unsigned int posY, unsigned int planeNo, unsigned char data) = 0;
	virtual void WritePixelData(unsigned int posX, unsigned int posY, unsigned int planeNo, unsigned int data, unsigned int bitCount) = 0;

	//Generic load functions
	virtual bool LoadImageFile(Stream::IStream& stream) = 0;

	//PCX functions
	virtual bool LoadPCXImage(Stream::IStream& stream) = 0;
	virtual bool SavePCXImage(Stream::IStream& stream) = 0;

	//TIFF functions
	virtual bool LoadTIFFImage(Stream::IStream& stream) = 0;
	virtual bool SaveTIFFImage(Stream::IStream& stream) = 0;

	//JPG functions
	virtual bool LoadJPGImage(Stream::IStream& stream) = 0;
	virtual bool SaveJPGImage(Stream::IStream& stream) = 0;

	//TGA functions
	virtual bool LoadTGAImage(Stream::IStream& stream) = 0;
	virtual bool SaveTGAImage(Stream::IStream& stream) = 0;

	//PNG functions
	virtual bool LoadPNGImage(Stream::IStream& stream) = 0;
	virtual bool SavePNGImage(Stream::IStream& stream) = 0;

	//BMP functions
	virtual bool LoadBMPImage(Stream::IStream& stream) = 0;
	virtual bool SaveBMPImage(Stream::IStream& stream) = 0;

	//Win32 DIB functions (Device-Independent Bitmap)
	virtual bool LoadDIBImage(Stream::IStream& stream, const BITMAPINFO& bitmapInfo) = 0;
	virtual bool SaveDIBImage(Stream::IStream& stream, BITMAPINFO& bitmapInfo) = 0;

	//Resampling functions
	virtual void ResampleNearest(unsigned int newWidth, unsigned int newHeight) = 0;
	virtual void ResampleNearest(const IImage& oldImage, unsigned int newWidth, unsigned int newHeight) = 0;
	virtual void ResampleBilinear(unsigned int newWidth, unsigned int newHeight) = 0;
	virtual void ResampleBilinear(const IImage& oldImage, unsigned int newWidth, unsigned int newHeight) = 0;
	//virtual void ResampleBicubic(unsigned int newWidth, unsigned int newHeight) = 0;
	//virtual void ResampleBicubic(Image& newImage, unsigned int newWidth, unsigned int newHeight) = 0;
};

#include "IImage.inl"
#endif
