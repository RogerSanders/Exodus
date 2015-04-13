/*--------------------------------------------------------------------------------------*\
Things to do:
-Write routines to convert from other colour spaces like CMYK
-Implement TIFF file save support
-Implement our own libjpeg error handler
-Implement gif support
-Implement dds support
-Find out what the image origin parameters mean in the tga header
-Implement a bicubic resampling algorithm
-Add full support for the BMP header V2 format
-Add support for BMP V4/5 header formats, and support loading alpha channel from V4+
bitmap files.
-Go over all our load bitmap code to check for any possible validation problems
-Go back and check our PNG file routines to see if we want to enhance any functionality.
PNG file support was added a long time ago and the objective was simply to get it working
for basic PNG images as quickly as possible.
-Enhance the generic LoadImage function format detection code, as commented within the
function.
-Add support for the netpbm image formats (inline text format images for embedding in
code)
-Add support for the XPM icon file format. This format is very useful for embedding
icon and cursor resources directly in sourcecode, and we should be able to do a one-step
conversion from XPM (or other icon formats) into something ready to create a win32 icon or
cursor from.
-Note that win32 icons have separate AND and XOR bit masks, which allows it to have not
only transparent pixels, but invert pixels, which invert the screen pixels under the
image. This invert mask can be treated just like another layer, but we'll need to
introduce new pixel formats in order to support invert layers.
-Add support for color index images with a palette. The color index data can just appear
in a layer, but we'll need a new set of functions to define the palette, as well as a new
PaletteFormat enumeration defining supported palette colour encodings for automatic
conversion to other formats. We'll also need to introduce pixel formats for color indexed
images.
-Add CUR/ICO file support
-Note that in order to support the "hotspot" concept of cursor files, we should introduce
"image origin" data fields, which will indicate the anchor point of the image. This will
be used for cursor files to set and retrieve the cursor hotspot.
-Consider adding a higher level "wrapper" of sorts, to support multiple paged (animated)
image formats. IE, consider adding animated support for gif and mng. Also consider adding
support for dcx (pcx image archive) format, and ani files for animated cursors. We'll also
need multi page support for even normal cur and ico files, since they can contain a
variety of images with different sizes and colour depths. Note that with gif files,
there's a duration value per image too which specifies how long it should be displayed
for, which should be supported.
\*--------------------------------------------------------------------------------------*/
#ifndef __IMAGE_H__
#define __IMAGE_H__
#include "ImageInterface/ImageInterface.pkg"
#include <vector>
#include "WindowsSupport/WindowsSupport.pkg"
#define PNG_SETJMP_NOT_SUPPORTED
#include <png.h>
#include <jpeglib.h>
#define AVOID_WIN32_FILEIO
#include <tiffio.h>
#include "StreamInterface/StreamInterface.pkg"

//##FIX## Right now the static support libraries we link to for various formats increase
//the size of any module using this library by around 1MB. We need to eliminate that
//overhead for uses which don't require this additional format support.
class Image :public IImage
{
public:
	//Constructors
	inline Image(unsigned int aimageWidth = 0, unsigned int aimageHeight = 0, PixelFormat apixelFormat = PIXELFORMAT_RGB, DataFormat adataFormat = DATAFORMAT_8BIT);

	//Image format functions
	virtual unsigned int GetImageWidth() const;
	virtual unsigned int GetImageHeight() const;
	virtual PixelFormat GetPixelFormat() const;
	virtual DataFormat GetDataFormat() const;
	virtual unsigned int GetDataPlaneCount() const;
	virtual void SetImageFormat(unsigned int aimageWidth, unsigned int aimageHeight, PixelFormat apixelFormat = PIXELFORMAT_RGB, DataFormat adataFormat = DATAFORMAT_8BIT);

	//Pixel data manipulation
	virtual void GetRawPixelData(unsigned int posX, unsigned int posY, unsigned int planeNo, PixelData& data) const;
	virtual void SetRawPixelData(unsigned int posX, unsigned int posY, unsigned int planeNo, PixelData data);
	virtual void ReadPixelData(unsigned int posX, unsigned int posY, unsigned int planeNo, float& data) const;
	virtual void ReadPixelData(unsigned int posX, unsigned int posY, unsigned int planeNo, unsigned char& data) const;
	virtual void ReadPixelData(unsigned int posX, unsigned int posY, unsigned int planeNo, unsigned int& data, unsigned int bitCount) const;
	virtual void WritePixelData(unsigned int posX, unsigned int posY, unsigned int planeNo, float data);
	virtual void WritePixelData(unsigned int posX, unsigned int posY, unsigned int planeNo, unsigned char data);
	virtual void WritePixelData(unsigned int posX, unsigned int posY, unsigned int planeNo, unsigned int data, unsigned int bitCount);

	//Generic load functions
	virtual bool LoadImageFile(Stream::IStream& stream);

	//PCX functions
	virtual bool LoadPCXImage(Stream::IStream& stream);
	virtual bool SavePCXImage(Stream::IStream& stream);

	//TIFF functions
	virtual bool LoadTIFFImage(Stream::IStream& stream);
	//##TODO##
	virtual bool SaveTIFFImage(Stream::IStream& stream);

	//JPG functions
	virtual bool LoadJPGImage(Stream::IStream& stream);
	virtual bool SaveJPGImage(Stream::IStream& stream);

	//TGA functions
	virtual bool LoadTGAImage(Stream::IStream& stream);
	virtual bool SaveTGAImage(Stream::IStream& stream);

	//PNG functions
	virtual bool LoadPNGImage(Stream::IStream& stream);
	virtual bool SavePNGImage(Stream::IStream& stream);

	//BMP functions
	virtual bool LoadBMPImage(Stream::IStream& stream);
	virtual bool SaveBMPImage(Stream::IStream& stream);

	//Win32 DIB functions (Device-Independent Bitmap)
	virtual bool LoadDIBImage(Stream::IStream& stream, const BITMAPINFOHEADER* bitmapHeader);
	virtual bool SaveDIBImage(Stream::IStream& stream, BITMAPINFOHEADER* bitmapHeader);

	//Resampling functions
	virtual void ResampleNearest(unsigned int newWidth, unsigned int newHeight);
	virtual void ResampleNearest(const IImage& oldImage, unsigned int newWidth, unsigned int newHeight);
	virtual void ResampleBilinear(unsigned int newWidth, unsigned int newHeight);
	virtual void ResampleBilinear(const IImage& oldImage, unsigned int newWidth, unsigned int newHeight);
	//virtual void ResampleBicubic(unsigned int newWidth, unsigned int newHeight);
	//virtual void ResampleBicubic(Image& newImage, unsigned int newWidth, unsigned int newHeight);

public:
	//BMP structures
	struct BITMAPV2INFOHEADER;
	struct BITMAPV3INFOHEADER;

private:
	//PCX structures
	struct PCXFileHeader;
	enum class PCXImageFormat;

	//TIFF structures
	struct TIFFStreamManager;

	//JPG structures
	struct JPGStreamSourceManager;
	struct JPGStreamDestinationManager;

	//TGA structures
	struct TGAFileHeader;
	struct TGAColorMapEntry;
	enum class TGAImageType;

private:
	//Image format functions
	inline void ResizeBuffer(unsigned int newSize);

	//Pixel data manipulation
	inline void GetRawPixelDataInternal(unsigned int posX, unsigned int posY, unsigned int planeNo, PixelData& data) const;
	inline void SetRawPixelDataInternal(unsigned int posX, unsigned int posY, unsigned int planeNo, PixelData data);
	inline void ReadPixelDataInternal(unsigned int posX, unsigned int posY, unsigned int planeNo, float& data) const;
	inline void ReadPixelDataInternal(unsigned int posX, unsigned int posY, unsigned int planeNo, unsigned char& data) const;
	inline void ReadPixelDataInternal(unsigned int posX, unsigned int posY, unsigned int planeNo, unsigned int& data, unsigned int bitCount) const;
	inline void WritePixelDataInternal(unsigned int posX, unsigned int posY, unsigned int planeNo, float data);
	inline void WritePixelDataInternal(unsigned int posX, unsigned int posY, unsigned int planeNo, unsigned char data);
	inline void WritePixelDataInternal(unsigned int posX, unsigned int posY, unsigned int planeNo, unsigned int data, unsigned int bitCount);

	//Bitfield functions
	unsigned int ReadBitfieldData(Stream::IStream& stream, unsigned char& currentBuffer, unsigned int& remainingBitsInBuffer, unsigned int dataBitCount) const;
	void WriteBitfieldData(Stream::IStream& stream, unsigned char& currentBuffer, unsigned int& remainingBitsInBuffer, unsigned int data, unsigned int dataBitCount) const;
	void FlushBitfieldData(Stream::IStream& stream, unsigned char& currentBuffer, unsigned int& remainingBitsInBuffer) const;

	//PNG functions
	static void PNGReadData(png_structp pngStruct, png_bytep data, png_size_t length);
	static void PNGWriteData(png_structp pngStruct, png_bytep data, png_size_t length);
	static void PNGFlushData(png_structp pngStruct);

	//Win32 DIB functions (Device-Independent Bitmap)
	unsigned int GetSetBitCount(unsigned int data) const;
	unsigned int MaskData(unsigned int data, unsigned int bitMask) const;

	//Image verification methods
	bool ImageValid() const;

private:
	PixelFormat pixelFormat;
	DataFormat dataFormat;
	unsigned int imageWidth;
	unsigned int imageHeight;
	unsigned int dataPlaneCount;
	//##TODO## Use a raw array rather than a vector here
	//##TODO## Don't use the PixelData structure internally, only use it for passing pixel
	//data externally in a generic way.
	//##TODO## Add fast methods to get and set sub-image data, to allow copying and
	//modifying regions of an image.
	//##TODO## When we make this a raw array, add a copy constructor which uses memcpy to
	//duplicate the elements of the array.
	std::vector<PixelData> imageData;
};

#include "Image.inl"
#endif
