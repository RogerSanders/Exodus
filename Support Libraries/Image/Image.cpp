#include "Image/Image.h"
#include "Stream/Stream.pkg"
#include <climits>

//----------------------------------------------------------------------------------------
//Image format functions
//----------------------------------------------------------------------------------------
void Image::ResizeBuffer(unsigned int newSize)
{
	imageData.resize(newSize);
}

//----------------------------------------------------------------------------------------
unsigned int Image::GetImageWidth() const
{
	return imageWidth;
}

//----------------------------------------------------------------------------------------
unsigned int Image::GetImageHeight() const
{
	return imageHeight;
}

//----------------------------------------------------------------------------------------
Image::PixelFormat Image::GetPixelFormat() const
{
	return pixelFormat;
}

//----------------------------------------------------------------------------------------
Image::DataFormat Image::GetDataFormat() const
{
	return dataFormat;
}

//----------------------------------------------------------------------------------------
unsigned int Image::GetDataPlaneCount() const
{
	return dataPlaneCount;
}

//----------------------------------------------------------------------------------------
void Image::SetImageFormat(unsigned int aimageWidth, unsigned int aimageHeight, PixelFormat apixelFormat, DataFormat adataFormat)
{
	imageWidth = aimageWidth;
	imageHeight = aimageHeight;
	dataFormat = adataFormat;
	pixelFormat = apixelFormat;
	switch(pixelFormat)
	{
	default:
		dataPlaneCount = 0;
		break;
	case PIXELFORMAT_M:
		dataPlaneCount = 1;
		break;
	case PIXELFORMAT_MA:
		dataPlaneCount = 2;
		break;
	case PIXELFORMAT_RGB:
		dataPlaneCount = 3;
		break;
	case PIXELFORMAT_RGBA:
		dataPlaneCount = 4;
		break;
	}
	unsigned int newDataBufferSize = imageWidth * imageHeight * dataPlaneCount;
	ResizeBuffer(newDataBufferSize);
}

//----------------------------------------------------------------------------------------
//Pixel data manipulation
//----------------------------------------------------------------------------------------
void Image::GetRawPixelData(unsigned int posX, unsigned int posY, unsigned int planeNo, PixelData& data) const
{
	GetRawPixelDataInternal(posX, posY, planeNo, data);
}

//----------------------------------------------------------------------------------------
void Image::SetRawPixelData(unsigned int posX, unsigned int posY, unsigned int planeNo, PixelData data)
{
	SetRawPixelDataInternal(posX, posY, planeNo, data);
}

//----------------------------------------------------------------------------------------
void Image::ReadPixelData(unsigned int posX, unsigned int posY, unsigned int planeNo, float& data) const
{
	ReadPixelDataInternal(posX, posY, planeNo, data);
}

//----------------------------------------------------------------------------------------
void Image::ReadPixelData(unsigned int posX, unsigned int posY, unsigned int planeNo, unsigned char& data) const
{
	ReadPixelDataInternal(posX, posY, planeNo, data);
}

//----------------------------------------------------------------------------------------
void Image::ReadPixelData(unsigned int posX, unsigned int posY, unsigned int planeNo, unsigned int& data, unsigned int bitCount) const
{
	ReadPixelDataInternal(posX, posY, planeNo, data, bitCount);
}

//----------------------------------------------------------------------------------------
void Image::WritePixelData(unsigned int posX, unsigned int posY, unsigned int planeNo, float data)
{
	WritePixelDataInternal(posX, posY, planeNo, data);
}

//----------------------------------------------------------------------------------------
void Image::WritePixelData(unsigned int posX, unsigned int posY, unsigned int planeNo, unsigned char data)
{
	WritePixelDataInternal(posX, posY, planeNo, data);
}

//----------------------------------------------------------------------------------------
void Image::WritePixelData(unsigned int posX, unsigned int posY, unsigned int planeNo, unsigned int data, unsigned int bitCount)
{
	WritePixelDataInternal(posX, posY, planeNo, data, bitCount);
}

//----------------------------------------------------------------------------------------
//Generic load functions
//----------------------------------------------------------------------------------------
bool Image::LoadImageFile(Stream::IStream& stream)
{
	//Save the current read position from the stream so that we can restore it when we're
	//done
	Stream::IStream::SizeType initialReadPosition = stream.GetStreamPos();

	//Read a few bytes from the stream, so we can try and detect the image format from
	//the header data.
	std::vector<unsigned char> header;
	if(!stream.ReadData(header, 10))
	{
		return false;
	}

	//Restore the initial stream read position
	stream.SetStreamPos(initialReadPosition);

	//Try and detect the file format
	if((header[0] == 'B') && (header[1] == 'M'))
	{
		//The stream contains a bitmap file marker. Decode the data as a bitmap image.
		return LoadBMPImage(stream);
	}
	if((header[0] == 0x89) && (header[1] == 'P') && (header[2] == 'N') && (header[3] == 'G')
	&& (header[4]== 0x0D) && (header[5]== 0x0A) && (header[6]== 0x1A) && (header[7]== 0x0A))
	{
		//The stream contains a PNG file marker. Decode the data as a PNG image.
		return LoadPNGImage(stream);
	}
	if((header[0] == 'G') && (header[1] == 'I') && (header[2] == 'F'))
	{
		//The GIF file format is not currently supported. Abort further processing.
		return false;
	}
	//##TODO## Find a reliable way to detect jpeg data
	if((header[6] == 'J') && (header[7] == 'F') && (header[8] == 'I') && (header[9] == 'F'))
	{
		//The file appears to be a JPEG file. Decode the data as a JPEG image.
		return LoadJPGImage(stream);
	}
	if(((header[0] == 'I') && (header[1] == 'I') && (header[2] == 42) && (header[3] == 0))
		|| ((header[0] == 'M') && (header[1] == 'M') && (header[2] == 0) && (header[3] == 42)))
	{
		//The header matches the TIFF file format. Decode the data as a TIFF file.
		return LoadTIFFImage(stream);
	}
	if((header[0] == 10) && (header[1] <= 5))
	{
		//The header appears to match the PCX file format. Decode the data as a PCX file.
		return LoadPCXImage(stream);
	}
	if(((header[1] == 0) || (header[1] == 1))
	|| ((header[2] >= 1) && (header[2] <= 3))
	|| ((header[2] >= 9) && (header[2] <= 11)))
	{
		//The file header appears to match the TGA header structure. Decode the data as a
		//TGA image.
		return LoadTGAImage(stream);
	}

	//We've failed to match the data to a known image file format
	return false;
}

//----------------------------------------------------------------------------------------
//PCX functions
//----------------------------------------------------------------------------------------
bool Image::LoadPCXImage(Stream::IStream& stream)
{
	//Load the PCX file header
	PCXFileHeader header;
	stream.ReadData(header.manufacturer); //Done
	stream.ReadData(header.version);
	stream.ReadData(header.encoding); //Done
	stream.ReadData(header.bitsPerPixel); //Done
	stream.ReadData(header.xmin); //Done
	stream.ReadData(header.ymin); //Done
	stream.ReadData(header.xmax); //Done
	stream.ReadData(header.ymax); //Done
	stream.ReadData(header.hdpi); //Ignore
	stream.ReadData(header.vdpi); //Ignore
	for(unsigned int i = 0; i < (sizeof(header.colorMap) / sizeof(header.colorMap[0])); ++i)
	{
		stream.ReadData(header.colorMap[i]);
	}
	stream.ReadData(header.reserved1); //Done
	stream.ReadData(header.numPlanes); //Done
	stream.ReadData(header.bytesPerLine); //Done
	stream.ReadData(header.paletteInfo); //Ignore
	stream.ReadData(header.hscreenSize); //Ignore
	stream.ReadData(header.vscreenSize); //Ignore
	for(unsigned int i = 0; i < (sizeof(header.reserved2) / sizeof(header.reserved2[0])); ++i)
	{
		stream.ReadData(header.reserved2[i]); //Done
	}

	//Validate the header data
	if(header.manufacturer != 10)
	{
		//If the header doesn't contain a valid PCX file marker, abort any further
		//processing.
		return false;
	}
	if(header.encoding != 1)
	{
		//An encoding value of 1, indicating run-length encoding, is the only defined
		//value. If the encoding doesn't match this, abort any further processing.
		return false;
	}

	//##FIX## Just how are we supposed to interpret this data? It seems pretty ambiguous.
	//How do we know when there's a palette? When a palette is present, there should be
	//only a single plane, but it seems that's not the case. "Magic" combinations of
	//plane and and bit counts are being used to encode the concept of image format,
	//as well as flag whether a palette is present, which muddles things. One possible
	//way to go is to assume there's a palette if the number of total bits across all
	//planes is less than or equal to 8, and collapse all planes down to a single index
	//value in this case. In other words, if((numPlanes * bitsPerPixel) > 8), we have a
	//true colour image, otherwise the plane count doesn't really represent planes, and
	//can be used to do our image format detection. This seems to be the best way to go.
	//This opens us up to supporting formats like 8-bit RGBA, 4-bit RGB/RGBA, 8-bit mono,
	//etc, without ambiguity.

	//Determine the format of the pixel data stored in the file
	PixelFormat newPixelFormat;
	PCXImageFormat imageFormat;
	unsigned int bitsPerLine = (unsigned int)header.bitsPerPixel * (unsigned int)header.numPlanes;
	bool paletteUsed = (bitsPerLine > 1) && (bitsPerLine <= 8);
	if(paletteUsed)
	{
		newPixelFormat = PIXELFORMAT_RGB;
		imageFormat = PCXIMAGEFORMAT_PALETTE;
	}
	else if(header.numPlanes == 1)
	{
		newPixelFormat = PIXELFORMAT_M;
		imageFormat = PCXIMAGEFORMAT_LITERAL;
	}
	else if(header.numPlanes == 2)
	{
		newPixelFormat = PIXELFORMAT_MA;
		imageFormat = PCXIMAGEFORMAT_LITERAL;
	}
	else if(header.numPlanes == 3)
	{
		newPixelFormat = PIXELFORMAT_RGB;
		imageFormat = PCXIMAGEFORMAT_LITERAL;
	}
	else if(header.numPlanes == 4)
	{
		newPixelFormat = PIXELFORMAT_RGBA;
		imageFormat = PCXIMAGEFORMAT_LITERAL;
	}
	else
	{
		//The image data uses an unsupported image format. Abort any further processing.
		return false;
	}

	////Determine the video mode used to store this image
	//PixelFormat newPixelFormat;
	//PCXImageFormat imageFormat;
	//if((header.numPlanes == 1) && (header.bitsPerPixel == 1))
	//{
	//	//Monochrome
	//	imageFormat = PCXIMAGEFORMAT_MONO;
	//	newPixelFormat = PIXELFORMAT_M;
	//}
	//else if((header.numPlanes == 1) && (header.bitsPerPixel == 2))
	//{
	//	//CGA - We should have a 4-entry palette in the header. There is 1 colour plane
	//	//with 2-bit entries.
	//	imageFormat = PCXIMAGEFORMAT_CGA;
	//	newPixelFormat = PIXELFORMAT_RGB;
	//}
	//else if((header.numPlanes == 3) && (header.bitsPerPixel == 1))
	//{
	//	//EGA - We should have an 8-entry palette in the header. There are 3 colour
	//	//planes, with 1-bit per plane. The values from each plane are combined to make
	//	//a single 3-bit palette index.
	//	imageFormat = PCXIMAGEFORMAT_EGA;
	//	newPixelFormat = PIXELFORMAT_RGB;
	//}
	//else if((header.numPlanes == 4) && (header.bitsPerPixel == 1))
	//{
	//	//EGA and VGA - We should have a 16-entry palette in the header. There are 4
	//	//colour planes, with 1-bit per plane. The values from each plane are combined
	//	//to make a single 4-bit palette index.
	//	imageFormat = PCXIMAGEFORMAT_EGA_AND_VGA;
	//	newPixelFormat = PIXELFORMAT_RGB;
	//}
	//else if((header.numPlanes == 1) && (header.bitsPerPixel == 8))
	//{
	//	//Extended VGA - We should have a 256-entry palette appended to the end of the
	//	//file. There is 1 colour plane with 8-bit entries, giving a single index into
	//	//the palette.
	//	imageFormat = PCXIMAGEFORMAT_EXTENDED_VGA;
	//	newPixelFormat = PIXELFORMAT_RGB;
	//}
	//else if((header.numPlanes == 3) && (header.bitsPerPixel == 8))
	//{
	//	//Extended VGA and XGA
	//	imageFormat = PCXIMAGEFORMAT_EXTENDED_VGA_AND_XGA;
	//	newPixelFormat = PIXELFORMAT_RGB;
	//}
	//else
	//{
	//	//Unknown image format. Abort any further processing.
	//	return false;
	//}

	//Calculate the image dimensions
	//##TODO## Consider whether we should mirror the image in the case where min is less
	//than max. Irfanview doesn't mirror, but it does use the absolute value to always
	//obtain a positive value, IE, max can be 0, and min contain the size.
	if((header.xmin > header.xmax) || (header.ymin > header.ymax))
	{
		//We have a minimum image dimension which is less than a maximum image dimension.
		//Abort any further processing.
		return false;
	}
	unsigned int newImageWidth = (header.xmax - header.xmin) + 1;
	unsigned int newImageHeight = (header.ymax - header.ymin) + 1;

	//Calculate and validate the size of the raw pixel data
	unsigned int totalPlaneByteCount = header.bytesPerLine;
	unsigned int totalLineByteCount = totalPlaneByteCount * header.numPlanes;
	unsigned int usedPlaneByteCount = ((newImageWidth * header.bitsPerPixel) + 7) / 8;
	unsigned int usedLineByteCount = ((newImageWidth * header.numPlanes * header.bitsPerPixel) + 7) / 8;
	if(usedLineByteCount > totalLineByteCount)
	{
		//The calculated number of bytes in a line is greater than the recorded number of
		//bytes in a line. Abort any further processing.
		return false;
	}

	//Read the raw pixel data. Note that we need to read all the pixel data in before we
	//can properly decode it, since we need to check for a VGA palette at the end of the
	//file, and if we try and guess whether the palette exists without correctly decoding
	//the pixel data which comes before it, we may get it wrong sometimes, since the true
	//length of the pixel data is unknown until we decode it, due to compression.
	unsigned int bufferSize = totalLineByteCount * newImageHeight;
	Stream::Buffer buffer(bufferSize);
	unsigned int readImageByteCount = 0;
	while(readImageByteCount < bufferSize)
	{
		//Decode the RLE data
		unsigned int bytesToWrite = 1;
		unsigned char data;
		stream.ReadData(data);
		if((data & 0xC0) == 0xC0)
		{
			bytesToWrite = data & 0x3F;
			stream.ReadData(data);
		}

		//Write this data block to the pixel buffer
		while((bytesToWrite > 0) && (readImageByteCount < bufferSize))
		{
			buffer[readImageByteCount++] = data;
			--bytesToWrite;
		}
		if(bytesToWrite > 0)
		{
			//We've hit a compressed pair which tried to overflow the pixel buffer. Abort
			//any further processing.
			return false;
		}
	}

	//Create and initialize the palette for the image
	const unsigned int paletteEntries = 256;
	const unsigned int paletteChannels = 3;
	unsigned char palette[paletteEntries][paletteChannels];
	if(paletteUsed)
	{
		for(unsigned int entryNo = 0; entryNo < paletteEntries; ++entryNo)
		{
			for(unsigned int channelNo = 0; channelNo < paletteChannels; ++channelNo)
			{
				palette[entryNo][channelNo] = 0;
			}
		}
	}

	//Check if a palette is appended to the end of the file
	bool appendedPalette = false;
	if(paletteUsed && !stream.IsAtEnd())
	{
		unsigned char temp;
		stream.ReadData(temp);
		appendedPalette = (temp == 0x0C);
	}

	//Load the palette for the image
	if(appendedPalette)
	{
		//If a palette appears to be appended at the end of the file, read the appended
		//palette into memory.
		for(unsigned int entryNo = 0; entryNo < paletteEntries; ++entryNo)
		{
			for(unsigned int channelNo = 0; channelNo < paletteChannels; ++channelNo)
			{
				unsigned char temp;
				stream.ReadData(temp);
				palette[entryNo][channelNo] = temp;
			}
		}
	}
	else if(paletteUsed)
	{
		//Parse the palette info in the file header
		for(unsigned int entryNo = 0; entryNo < paletteEntries; ++entryNo)
		{
			for(unsigned int channelNo = 0; channelNo < paletteChannels; ++channelNo)
			{
				unsigned char entry = header.colorMap[(entryNo * paletteChannels) + channelNo];
				palette[entryNo][channelNo] = entry;
			}
		}
	}

	//Set the image format
	SetImageFormat(newImageWidth, newImageHeight, newPixelFormat, DATAFORMAT_8BIT);

	//Decode the pixel data
	unsigned char currentPixelBuffer = 0;
	unsigned int remainingBitsInPixelBuffer = 0;
	for(unsigned int ypos = 0; ypos < imageHeight; ++ypos)
	{
		if(imageFormat == PCXIMAGEFORMAT_PALETTE)
		{
			//Initialize the index buffer. Sample images have shown that when a
			//palletized image contains multiple planes, the palette index for a pixel is
			//striped across all the planes, with the first plane containing the lowest
			//bits. Eg, theoretically if there was a 16-bit palette index of 0x1234, and
			//it was stored across two planes with 8-bits per plane, the first plane
			//would contain 0x34, while the second plane would contain 0x12. We create a
			//buffer here to store the decoded palette index values across all the data
			//planes.
			std::vector<unsigned int> paletteEntries(imageWidth);
			for(unsigned int xpos = 0; xpos < imageWidth; ++xpos)
			{
				paletteEntries[xpos] = 0;
			}

			//Combine the index data from across all the stored data planes
			for(unsigned int planeNo = 0; planeNo < header.numPlanes; ++planeNo)
			{
				//Read the index data for this plane, this line
				for(unsigned int xpos = 0; xpos < imageWidth; ++xpos)
				{
					unsigned int data = ReadBitfieldData(buffer, currentPixelBuffer, remainingBitsInPixelBuffer, header.bitsPerPixel);
					paletteEntries[xpos] |= (data << (planeNo * header.bitsPerPixel));
				}
				//Strip any padding from the end of the line
				remainingBitsInPixelBuffer = 0;
				for(unsigned int byteNo = usedPlaneByteCount; byteNo < totalPlaneByteCount; ++byteNo)
				{
					ReadBitfieldData(buffer, currentPixelBuffer, remainingBitsInPixelBuffer, 8);
				}
			}

			//Read the combined index data, and write the decoded line
			for(unsigned int xpos = 0; xpos < imageWidth; ++xpos)
			{
				unsigned int paletteIndex = paletteEntries[xpos];
				unsigned int r = palette[paletteIndex][0];
				unsigned int g = palette[paletteIndex][1];
				unsigned int b = palette[paletteIndex][2];
				WritePixelDataInternal(xpos, ypos, 0, r, 8);
				WritePixelDataInternal(xpos, ypos, 1, g, 8);
				WritePixelDataInternal(xpos, ypos, 2, b, 8);
			}
		}
		else if(imageFormat == PCXIMAGEFORMAT_LITERAL)
		{
			for(unsigned int planeNo = 0; planeNo < header.numPlanes; ++planeNo)
			{
				//Read the pixel data for this plane, this line
				for(unsigned int xpos = 0; xpos < imageWidth; ++xpos)
				{
					unsigned int data = ReadBitfieldData(buffer, currentPixelBuffer, remainingBitsInPixelBuffer, header.bitsPerPixel);
					WritePixelDataInternal(xpos, ypos, planeNo, data, header.bitsPerPixel);
				}
				//Strip any padding from the end of the line
				remainingBitsInPixelBuffer = 0;
				for(unsigned int byteNo = usedPlaneByteCount; byteNo < totalPlaneByteCount; ++byteNo)
				{
					ReadBitfieldData(buffer, currentPixelBuffer, remainingBitsInPixelBuffer, 8);
				}
			}
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool Image::SavePCXImage(Stream::IStream& stream)
{
	//Ensure the current image is valid before proceeding
	if(!ImageValid())
	{
		return false;
	}

	//Build the PCX file header
	PCXFileHeader header;
	header.manufacturer = 10;
	header.version = 5;
	header.encoding = 1;
	header.bitsPerPixel = 8;
	header.xmin = 0;
	header.ymin = 0;
	header.xmax = (unsigned short)imageWidth - 1;
	header.ymax = (unsigned short)imageHeight - 1;
	header.hdpi = 0;
	header.vdpi = 0;
	for(unsigned int i = 0; i < (sizeof(header.colorMap) / sizeof(header.colorMap[0])); ++i)
	{
		header.colorMap[i] = 0;
	}
	header.reserved1 = 0;
	header.numPlanes = (unsigned char)GetDataPlaneCount();
	unsigned int usedBytesPerPlane = ((header.bitsPerPixel * imageWidth) + 7) / 8;
	bool addPaddingByteToPlane = (usedBytesPerPlane % 2) != 0;
	header.bytesPerLine = (unsigned short)usedBytesPerPlane;
	if(addPaddingByteToPlane)
	{
		++header.bytesPerLine;
	}
	header.paletteInfo = 1;
	header.hscreenSize = 0;
	header.vscreenSize = 0;
	for(unsigned int i = 0; i < (sizeof(header.reserved2) / sizeof(header.reserved2[0])); ++i)
	{
		header.reserved2[i] = 0;
	}

	//Write the PCX file header
	stream.WriteData(header.manufacturer);
	stream.WriteData(header.version);
	stream.WriteData(header.encoding);
	stream.WriteData(header.bitsPerPixel);
	stream.WriteData(header.xmin);
	stream.WriteData(header.ymin);
	stream.WriteData(header.xmax);
	stream.WriteData(header.ymax);
	stream.WriteData(header.hdpi);
	stream.WriteData(header.vdpi);
	for(unsigned int i = 0; i < (sizeof(header.colorMap) / sizeof(header.colorMap[0])); ++i)
	{
		stream.WriteData(header.colorMap[i]);
	}
	stream.WriteData(header.reserved1);
	stream.WriteData(header.numPlanes);
	stream.WriteData(header.bytesPerLine);
	stream.WriteData(header.paletteInfo);
	stream.WriteData(header.hscreenSize);
	stream.WriteData(header.vscreenSize);
	for(unsigned int i = 0; i < (sizeof(header.reserved2) / sizeof(header.reserved2[0])); ++i)
	{
		stream.WriteData(header.reserved2[i]);
	}

	//Write the pixel data
	for(unsigned int ypos = 0; ypos < imageHeight; ++ypos)
	{
		bool compressionRunActive = false;
		unsigned char lastByte;
		unsigned int runCount = 0;
		const unsigned int maxRunLength = 0x3F;
		for(unsigned int planeNo = 0; planeNo < GetDataPlaneCount(); ++planeNo)
		{
			for(unsigned int xpos = 0; xpos < imageWidth; ++xpos)
			{
				unsigned char data;
				ReadPixelDataInternal(xpos, ypos, planeNo, data);

				if(compressionRunActive)
				{
					//Update the active compression run
					if((data != lastByte) || (runCount >= maxRunLength))
					{
						//Save the compression pair to the stream
						if((runCount > 1) || ((lastByte & 0xC0) != 0))
						{
							unsigned char compressionByte = (unsigned char)runCount | 0xC0;
							stream.WriteData(compressionByte);
						}
						stream.WriteData(lastByte);
						compressionRunActive = false;
					}
					else
					{
						//If we're in the middle of an RLE compression run, and the next byte
						//matches the compressed value, add it to the run count.
						++runCount;
					}
				}

				if(!compressionRunActive)
				{
					//Start a new compression run
					compressionRunActive = true;
					lastByte = data;
					runCount = 1;
				}
			}

			//Pad the plane data to a word boundary
			if(addPaddingByteToPlane)
			{
				++runCount;
			}
		}

		//Flush any remaining compressed data to the stream
		if(compressionRunActive)
		{
			//Save the compression pair to the stream
			if((runCount > 1) || ((lastByte & 0xC0) != 0))
			{
				unsigned char compressionByte = (unsigned char)runCount | 0xC0;
				stream.WriteData(compressionByte);
			}
			stream.WriteData(lastByte);
			compressionRunActive = false;
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------
//TIFF functions
//----------------------------------------------------------------------------------------
bool Image::LoadTIFFImage(Stream::IStream& stream)
{
	bool result = false;
	TIFFStreamManager manager(stream);
	TIFF* tif = manager.Open("Test", "r");
	if(tif != 0)
	{
		uint32 newImageWidth;
		uint32 newImageHeight;
		TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &newImageWidth);
		TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &newImageHeight);

		unsigned int pixelCount = (unsigned int)(newImageWidth * newImageHeight);
		uint32* rawPixelData = new uint32[pixelCount];
		if(TIFFReadRGBAImage(tif, newImageWidth, newImageHeight, rawPixelData, 0) != 0)
		{
			//Apply the new image format
			SetImageFormat(newImageWidth, newImageHeight, PIXELFORMAT_RGBA, DATAFORMAT_8BIT);

			//Decode the image data
			for(unsigned int ypos = 0; ypos < imageHeight; ++ypos)
			{
				unsigned int writeYPos = ((imageHeight - 1) - ypos);
				for(unsigned int xpos = 0; xpos < imageWidth; ++xpos)
				{
					uint32 pixel = rawPixelData[(ypos * imageWidth) + xpos];
					unsigned char r = (unsigned char)TIFFGetR(pixel);
					unsigned char g = (unsigned char)TIFFGetG(pixel);
					unsigned char b = (unsigned char)TIFFGetB(pixel);
					unsigned char a = (unsigned char)TIFFGetA(pixel);
					WritePixelDataInternal(xpos, writeYPos, 0, r);
					WritePixelDataInternal(xpos, writeYPos, 1, g);
					WritePixelDataInternal(xpos, writeYPos, 2, b);
					WritePixelDataInternal(xpos, writeYPos, 3, a);
				}
			}

			result = true;
		}
		delete[] rawPixelData;
	}

	return result;
}

//----------------------------------------------------------------------------------------
bool Image::SaveTIFFImage(Stream::IStream& stream)
{
	//Ensure the current image is valid before proceeding
	if(!ImageValid())
	{
		return false;
	}

	//##TODO##
	TIFFStreamManager manager(stream);

	return false;
}

//----------------------------------------------------------------------------------------
//JPG functions
//----------------------------------------------------------------------------------------
bool Image::LoadJPGImage(Stream::IStream& stream)
{
	//1. Allocate and initialize a JPEG decompression object.
	jpeg_decompress_struct cinfo;
	jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	//2. Specify the source of the compressed data (eg, a file).
	JPGStreamSourceManager sourceManager(&cinfo, stream, 1024);

	//3. Call jpeg_read_header() to obtain image info.
	jpeg_read_header(&cinfo, TRUE);

	//4. Set parameters for decompression.
	//Nothing to do. jpeg_read_header loaded defaults, which is what we want.
	//##TODO## Don't use the libjpeg colour space conversion. Write our own routines to
	//convert colour spaces.
	cinfo.out_color_space = JCS_RGB;

	//5. jpeg_start_decompress(...);
	jpeg_start_decompress(&cinfo);

	//Change our image format to match the decoded data
	unsigned int newImageWidth = (unsigned int)cinfo.output_width;
	unsigned int newImageHeight = (unsigned int)cinfo.output_height;
	DataFormat newDataFormat = DATAFORMAT_8BIT;
	PixelFormat newPixelFormat = PIXELFORMAT_RGB;
	//##FIX##
	//switch(cinfo.jpeg_color_space)
	//{
	//case JCS_UNKNOWN:
	//default:
	//	//Unknown jpeg colour space. Abort any further procesing.
	//	return false;
	//case JCS_YCbCr:
	//case JCS_CMYK:
	//case JCS_YCCK:
	//	//The image uses an unsupported colour space. Abort any further processing.
	//	return false;
	//case JCS_GRAYSCALE:
	//	//We're decoding a monochrome image
	//	newPixelFormat = PIXELFORMAT_M;
	//	break;
	//case JCS_RGB:
	//	//We're decoding an RGB image
	//	newPixelFormat = PIXELFORMAT_RGB;
	//	break;
	//}
	SetImageFormat(newImageWidth, newImageHeight, newPixelFormat, newDataFormat);

	//6. while (scan lines remain to be read)
	//jpeg_read_scanlines(...);
	int rowStride = cinfo.output_width * cinfo.output_components;
	JSAMPROW buffer = new JSAMPLE[rowStride];
	for(unsigned int ypos = 0; ypos < imageHeight; ++ypos)
	{
		unsigned int writeYPos = ((imageHeight - 1) - ypos);
		jpeg_read_scanlines(&cinfo, &buffer, 1);
		for(unsigned int xpos = 0; xpos < imageWidth; ++xpos)
		{
			if(pixelFormat == PIXELFORMAT_M)
			{
				unsigned int m = (unsigned int)buffer[xpos];
				WritePixelDataInternal(xpos, writeYPos, 0, m, 8);
			}
			else if(pixelFormat == PIXELFORMAT_RGB)
			{
				unsigned int r = (unsigned int)buffer[(xpos * 3) + 0];
				unsigned int g = (unsigned int)buffer[(xpos * 3) + 1];
				unsigned int b = (unsigned int)buffer[(xpos * 3) + 2];
				WritePixelDataInternal(xpos, writeYPos, 0, r, 8);
				WritePixelDataInternal(xpos, writeYPos, 1, g, 8);
				WritePixelDataInternal(xpos, writeYPos, 2, b, 8);
			}
		}
	}
	delete[] buffer;

	//7. jpeg_finish_decompress(...);
	jpeg_finish_decompress(&cinfo);

	//8. Release the JPEG decompression object.
	jpeg_destroy_decompress(&cinfo);

	return true;
}

//----------------------------------------------------------------------------------------
bool Image::SaveJPGImage(Stream::IStream& stream)
{
	//Ensure the current image is valid before proceeding
	if(!ImageValid())
	{
		return false;
	}

	//1. Allocate and initialize a JPEG compression object.
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	//2. Specify the destination for the compressed data (eg, a file).
	JPGStreamDestinationManager destinationManager(&cinfo, stream, 1024);

	//3. Set parameters for compression, including image size & colorspace.
	cinfo.image_width = imageWidth;
	cinfo.image_height = imageHeight;
	if((pixelFormat == PIXELFORMAT_M) || (pixelFormat == PIXELFORMAT_MA))
	{
		cinfo.input_components = 1;
		cinfo.in_color_space = JCS_GRAYSCALE;
	}
	else if((pixelFormat == PIXELFORMAT_RGB) || (pixelFormat == PIXELFORMAT_RGBA))
	{
		cinfo.input_components = 3;
		cinfo.in_color_space = JCS_RGB;
	}
	else
	{
		//Unknown pixelformat. Abort any further processing.
		return false;
	}
	jpeg_set_defaults(&cinfo);

	//4. jpeg_start_compress(...);
	jpeg_start_compress(&cinfo, TRUE);

	//5. while (scan lines remain to be written)
	//jpeg_write_scanlines(...);
	int rowStride = cinfo.image_width * cinfo.input_components;
	JSAMPROW buffer = new JSAMPLE[rowStride];
	for(unsigned int ypos = 0; ypos < imageHeight; ++ypos)
	{
		unsigned int readYPos = ((imageHeight - 1) - ypos);
		for(unsigned int xpos = 0; xpos < imageWidth; ++xpos)
		{
			if((pixelFormat == PIXELFORMAT_M) || (pixelFormat == PIXELFORMAT_MA))
			{
				unsigned int m;
				ReadPixelDataInternal(xpos, readYPos, 0, m, 8);
				buffer[xpos] = (JSAMPLE)m;
			}
			else if((pixelFormat == PIXELFORMAT_RGB) || (pixelFormat == PIXELFORMAT_RGBA))
			{
				unsigned int r;
				unsigned int g;
				unsigned int b;
				ReadPixelDataInternal(xpos, readYPos, 0, r, 8);
				ReadPixelDataInternal(xpos, readYPos, 1, g, 8);
				ReadPixelDataInternal(xpos, readYPos, 2, b, 8);
				buffer[(xpos * 3) + 0] = (JSAMPLE)r;
				buffer[(xpos * 3) + 1] = (JSAMPLE)g;
				buffer[(xpos * 3) + 2] = (JSAMPLE)b;
			}
		}
		jpeg_write_scanlines(&cinfo, &buffer, 1);
	}
	delete[] buffer;

	//6. jpeg_finish_compress(...);
	jpeg_finish_compress(&cinfo);

	//7. Release the JPEG compression object.
	jpeg_destroy_compress(&cinfo);

	return true;
}

//----------------------------------------------------------------------------------------
//TGA functions
//----------------------------------------------------------------------------------------
bool Image::LoadTGAImage(Stream::IStream& stream)
{
	//Load the tga file header
	TGAFileHeader fileHeader;
	stream.ReadData(fileHeader.idLength);
	stream.ReadData(fileHeader.colorMapType);
	stream.ReadData(fileHeader.imageType);
	stream.ReadData(fileHeader.colorMapFirstEntry);
	stream.ReadData(fileHeader.colorMapLength);
	stream.ReadData(fileHeader.colorMapBitsPerEntry);
	stream.ReadData(fileHeader.imageOriginX);
	stream.ReadData(fileHeader.imageOriginY);
	stream.ReadData(fileHeader.imageWidth);
	stream.ReadData(fileHeader.imageHeight);
	stream.ReadData(fileHeader.bitsPerPixel);
	stream.ReadData(fileHeader.imageDescriptor);

	//Decode the color map info
	if(fileHeader.colorMapType > 1)
	{
		//The colorMapType field is only allowed to contain either a 0, indicating no
		//color map is present, or 1, indicating a color map is present. Any other values
		//are not recognized.
		return false;
	}
	bool colorMapPresent = (fileHeader.colorMapType > 0);
	unsigned int colorMapFirstEntry = fileHeader.colorMapFirstEntry;
	unsigned int colorMapLength = fileHeader.colorMapLength;
	unsigned int colorMapBitsPerEntry = fileHeader.colorMapBitsPerEntry;

	//We limit the maximum size of the alpha channel in the colour map to 32 bits. If the
	//alpha channel is larger than 32 bits, we abort any further processing.
	unsigned int maxColorMapBitsPerEntry = (8 * 4) + (8 * 3);
	if(colorMapBitsPerEntry > maxColorMapBitsPerEntry)
	{
		return false;
	}

	//Calculate the number of bits per channel in the colour map. According to the TGA
	//file format specification, the colour map stores RGBA values, with the R, G, and B
	//channels being the same length, with a maximum of 8 bits. The alpha channel is made
	//up of whatever bits remain in the color map after dividing the bitcount of a single
	//entry amoung the RGB channels.
	unsigned int colorMapBitsRGB = colorMapBitsPerEntry / 3;
	if(colorMapBitsRGB > 8)
	{
		colorMapBitsRGB = 8;
	}
	unsigned int colorMapBitsAlpha = colorMapBitsPerEntry - (colorMapBitsRGB * 8);
	unsigned int colorMapByteSize = ((colorMapBitsPerEntry * colorMapLength) + 7) / 8;

	//Decode the image type
	unsigned int colorChannelCount = 0;
	TGAImageType imageType;
	bool rleCompressionUsed = false;
	switch(fileHeader.imageType)
	{
	case TGAIMAGETYPE_RLEPALETTE:
		rleCompressionUsed = true;
	case TGAIMAGETYPE_PALETTE:
		imageType = TGAIMAGETYPE_PALETTE;
		colorChannelCount = 3;
		break;
	case TGAIMAGETYPE_RLERGB:
		rleCompressionUsed = true;
	case TGAIMAGETYPE_RGB:
		imageType = TGAIMAGETYPE_RGB;
		colorChannelCount = 3;
		break;
	case TGAIMAGETYPE_RLEMONOCHROME:
		rleCompressionUsed = true;
	case TGAIMAGETYPE_MONOCHROME:
		imageType = TGAIMAGETYPE_MONOCHROME;
		colorChannelCount = 1;
		break;
	case TGAIMAGETYPE_NONE:
		//The file contains no image data. Abort any further processing.
		return false;
	default:
		//An unknown image type has been specified. Abort any further processing.
		return false;
	}

	//Decode the image format
	unsigned int newImageWidth = fileHeader.imageWidth;
	unsigned int newImageHeight = fileHeader.imageHeight;
	unsigned int bitsPerPixel = fileHeader.bitsPerPixel;
	unsigned int alphaChannelBits = fileHeader.imageDescriptor & 0x0F;

	//If more bits have been allocated to the alpha channel than are actually present in
	//the image, abort any further processing.
	if(alphaChannelBits > bitsPerPixel)
	{
		return false;
	}

	//Calculate the number of bits per channel
	//##TODO## There's some ambiguity as to how we should calculate the number of bits
	//per channel for RGB images. Obviously we have to extract the number of bits
	//allocated to the alpha channel. If we divide the number of remaining bits evenly
	//between the three colour channels, what do we do with any bits that are left over?
	//I would say these bits simply get discarded, but we need to verify this behaviour.
	unsigned int bitsPerChannel = (bitsPerPixel - alphaChannelBits) / colorChannelCount;
	unsigned int discardedBitsPerPixel = bitsPerPixel - (alphaChannelBits + (bitsPerChannel * colorChannelCount));

	//Check if we need to mirror the image data either horizontally or vertically. Note
	//that if the vertical mirroring bit is unset, the image is written bottom up. Since
	//we decode top-down by default, we reverse the result of this bit.
	bool mirrorX = (fileHeader.imageDescriptor & 0x10) != 0;
	bool mirrorY = (fileHeader.imageDescriptor & 0x20) == 0;

	//Determine the new image format
	unsigned int mBitCount = 0;
	unsigned int rBitCount = 0;
	unsigned int gBitCount = 0;
	unsigned int bBitCount = 0;
	unsigned int aBitCount = 0;
	DataFormat newDataFormat = DATAFORMAT_8BIT;
	PixelFormat newPixelFormat;
	if(imageType == TGAIMAGETYPE_PALETTE)
	{
		rBitCount = colorMapBitsRGB;
		gBitCount = colorMapBitsRGB;
		bBitCount = colorMapBitsRGB;
		if(colorMapBitsAlpha > 0)
		{
			newPixelFormat = PIXELFORMAT_RGBA;
			aBitCount = colorMapBitsAlpha;
		}
		else
		{
			newPixelFormat = PIXELFORMAT_RGB;
		}
	}
	else if(imageType == TGAIMAGETYPE_RGB)
	{
		rBitCount = bitsPerChannel;
		gBitCount = bitsPerChannel;
		bBitCount = bitsPerChannel;
		if(alphaChannelBits > 0)
		{
			newPixelFormat = PIXELFORMAT_RGBA;
			aBitCount = alphaChannelBits;
		}
		else
		{
			newPixelFormat = PIXELFORMAT_RGB;
		}
	}
	else if(imageType == TGAIMAGETYPE_MONOCHROME)
	{
		mBitCount = bitsPerChannel;
		if(alphaChannelBits > 0)
		{
			newPixelFormat = PIXELFORMAT_MA;
			aBitCount = alphaChannelBits;
		}
		else
		{
			newPixelFormat = PIXELFORMAT_M;
		}
	}
	else
	{
		//We should never end up here. If we do, it's because we messed up when decoding
		//the image type above.
		return false;
	}

	//If an image ID field is included, we skip over this data.
	for(unsigned int i = 0; i < fileHeader.idLength; ++i)
	{
		unsigned char temp;
		stream.ReadData(temp);
	}

	//Check if we need to extract a colour map for this image
	std::vector<TGAColorMapEntry> colorMap(colorMapLength);
	if(imageType == TGAIMAGETYPE_PALETTE)
	{
		//If the image is set to use the colour map, but no colour map is present, abort
		//any further processing.
		if(!colorMapPresent)
		{
			return false;
		}

		//If a colour map is present, we read in the colour map data here.
		unsigned char currentBuffer = 0;
		unsigned int remainingBits = 0;
		for(unsigned int i = 0; i < colorMapLength; ++i)
		{
			//##TODO## Verify the correct order of channels is RGBA in the colour map
			colorMap[i].r = ReadBitfieldData(stream, currentBuffer, remainingBits, colorMapBitsRGB);
			colorMap[i].g = ReadBitfieldData(stream, currentBuffer, remainingBits, colorMapBitsRGB);
			colorMap[i].b = ReadBitfieldData(stream, currentBuffer, remainingBits, colorMapBitsRGB);
			colorMap[i].a = ReadBitfieldData(stream, currentBuffer, remainingBits, colorMapBitsAlpha);
		}
	}
	else if(colorMapPresent)
	{
		//If a colour map is present, but this isn't a colour mapped image, we skip over
		//the colour map data. Note that we don't throw an error here, since some
		//programs may use the colour map area on non-colour mapped images to store
		//custom information. Skipping the colour map when it is not required is
		//recommended by the official TGA file format specification.
		for(unsigned int i = 0; i < fileHeader.idLength; ++i)
		{
			unsigned char temp;
			stream.ReadData(temp);
		}
	}

	//Apply the new image properties
	SetImageFormat(newImageWidth, newImageHeight, newPixelFormat, newDataFormat);

	//Decode the pixel data
	unsigned char currentBuffer = 0;
	unsigned int remainingBits = 0;
	unsigned int m, r, g, b, a;
	int repetitionCount = 0;
	bool currentPacketCompressed = false;
	for(unsigned int ypos = 0; ypos < imageHeight; ++ypos)
	{
		unsigned int writeYPos = mirrorY? ((imageHeight - 1) - ypos): ypos;
		for(unsigned int xpos = 0; xpos < imageWidth; ++xpos)
		{
			unsigned int writeXPos = mirrorX? ((imageWidth - 1) - xpos): xpos;

			//If rle compression is enabled for this image, check if we need to read a
			//new packet from the compressed data stream.
			bool readNewRepetitionData = false;
			if(rleCompressionUsed)
			{
				--repetitionCount;
				if(repetitionCount < 0)
				{
					//If we've finished processing the last packet, read another packet
					//header from the stream. Packet headers are 8-bit fields with the
					//following format:
					//-----------------------------------
					//| 7  | 6 | 5 | 4 | 3 | 2 | 1 | 0  |
					//|---------------------------------|
					//|Type|    Repetition Count        |
					//-----------------------------------
					//Type:	The packet type
					//      0 = raw (uncompressed) packet
					//      1 = run length (compressed) packet
					//Repetition Count:	Repetition counter - 1
					//      For an uncompressed packet, this indicates the number of
					//      literal pixel values which follow. For a compressed packet,
					//      this indicates the number of times to repeat the following
					//      single literal pixel value. Add 1 to this field to obtain the
					//      true pixel count, IE, 0 = 1 pixel, 1 = 2 pixels, etc.
					currentPacketCompressed = ReadBitfieldData(stream, currentBuffer, remainingBits, 1) != 0;
					repetitionCount = (int)ReadBitfieldData(stream, currentBuffer, remainingBits, 7) + 1;
					readNewRepetitionData = true;
				}
			}

			if(!rleCompressionUsed || !currentPacketCompressed || readNewRepetitionData)
			{
				if(imageType == TGAIMAGETYPE_PALETTE)
				{
					unsigned int colorMapIndex = ReadBitfieldData(stream, currentBuffer, remainingBits, bitsPerPixel);

					//Verify that the colour map index for this pixel falls within a valid
					//entry in the colour map. If it does not, abort any further processing.
					if((colorMapFirstEntry > colorMapIndex) || ((colorMapIndex - colorMapFirstEntry) >= colorMapLength))
					{
						return false;
					}

					colorMapIndex -= colorMapFirstEntry;

					r = colorMap[colorMapIndex].r;
					g = colorMap[colorMapIndex].g;
					b = colorMap[colorMapIndex].b;
					a = colorMap[colorMapIndex].a;
				}
				else if(imageType == TGAIMAGETYPE_RGB)
				{
					//Note that the field order is BGRA in the file
					b = ReadBitfieldData(stream, currentBuffer, remainingBits, bBitCount);
					g = ReadBitfieldData(stream, currentBuffer, remainingBits, gBitCount);
					r = ReadBitfieldData(stream, currentBuffer, remainingBits, rBitCount);
					a = ReadBitfieldData(stream, currentBuffer, remainingBits, aBitCount);
				}
				else if(imageType == TGAIMAGETYPE_MONOCHROME)
				{
					m = ReadBitfieldData(stream, currentBuffer, remainingBits, mBitCount);
					a = ReadBitfieldData(stream, currentBuffer, remainingBits, aBitCount);
				}

				//Extract the discarded bits per pixel
				ReadBitfieldData(stream, currentBuffer, remainingBits, discardedBitsPerPixel);
			}

			//Write the extracted pixel data to the image
			unsigned int alphaPlaneNo = 0;
			if(mBitCount > 0)
			{
				WritePixelDataInternal(writeXPos, writeYPos, 0, m, mBitCount);
				alphaPlaneNo = 1;
			}
			else
			{
				WritePixelDataInternal(writeXPos, writeYPos, 0, r, rBitCount);
				WritePixelDataInternal(writeXPos, writeYPos, 1, g, gBitCount);
				WritePixelDataInternal(writeXPos, writeYPos, 2, b, bBitCount);
				alphaPlaneNo = 3;
			}
			if(aBitCount > 0)
			{
				WritePixelDataInternal(writeXPos, writeYPos, alphaPlaneNo, a, aBitCount);
			}
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool Image::SaveTGAImage(Stream::IStream& stream)
{
	//Ensure the current image is valid before proceeding
	if(!ImageValid())
	{
		return false;
	}

	//Determine the TGA file format settings
	bool flipVertically = true;
	bool flipHorizontally = false;
	unsigned int colorChannelCount = 0;
	unsigned int colorChannelBitCount = 8;
	unsigned int alphaChannelBitCount = 8;
	unsigned int mBitCount = 0;
	unsigned int rBitCount = 0;
	unsigned int gBitCount = 0;
	unsigned int bBitCount = 0;
	unsigned int aBitCount = 0;
	TGAImageType tgaImageType;
	switch(GetPixelFormat())
	{
	case PIXELFORMAT_MA:
		aBitCount = 8;
	case PIXELFORMAT_M:
		tgaImageType = TGAIMAGETYPE_MONOCHROME;
		colorChannelCount = 1;
		mBitCount = colorChannelBitCount;
		break;
	case PIXELFORMAT_RGBA:
		aBitCount = 8;
	case PIXELFORMAT_RGB:
		tgaImageType = TGAIMAGETYPE_RGB;
		colorChannelCount = 3;
		rBitCount = colorChannelBitCount;
		gBitCount = colorChannelBitCount;
		bBitCount = colorChannelBitCount;
		break;
	default:
		return false;
	}
	unsigned int bitsPerPixel = mBitCount + rBitCount + gBitCount + bBitCount + aBitCount;
	unsigned int imageDescriptor = aBitCount;
	imageDescriptor |= flipVertically? 0x20: 0;
	imageDescriptor |= flipHorizontally? 0x10: 0;

	//Build the TGA file header
	TGAFileHeader fileHeader;
	fileHeader.idLength = 0;
	fileHeader.colorMapType = 0;
	fileHeader.imageType = (unsigned char)tgaImageType;
	fileHeader.colorMapFirstEntry = 0;
	fileHeader.colorMapLength = 0;
	fileHeader.colorMapBitsPerEntry = 0;
	fileHeader.imageOriginX = 0;
	fileHeader.imageOriginY = 0;
	fileHeader.imageWidth = (unsigned short)imageWidth;
	fileHeader.imageHeight = (unsigned short)imageHeight;
	fileHeader.bitsPerPixel = (unsigned char)bitsPerPixel;
	fileHeader.imageDescriptor = (unsigned char)imageDescriptor;

	//Write the header to the file
	stream.WriteData(fileHeader.idLength);
	stream.WriteData(fileHeader.colorMapType);
	stream.WriteData(fileHeader.imageType);
	stream.WriteData(fileHeader.colorMapFirstEntry);
	stream.WriteData(fileHeader.colorMapLength);
	stream.WriteData(fileHeader.colorMapBitsPerEntry);
	stream.WriteData(fileHeader.imageOriginX);
	stream.WriteData(fileHeader.imageOriginY);
	stream.WriteData(fileHeader.imageWidth);
	stream.WriteData(fileHeader.imageHeight);
	stream.WriteData(fileHeader.bitsPerPixel);
	stream.WriteData(fileHeader.imageDescriptor);

	//Write the pixel data to the file
	unsigned char currentBuffer = 0;
	unsigned int remainingBits = 8;
	for(unsigned int ypos = 0; ypos < imageHeight; ++ypos)
	{
		for(unsigned int xpos = 0; xpos < imageWidth; ++xpos)
		{
			//Note that the field order is BGRA in the file
			if(mBitCount > 0)
			{
				unsigned int m;
				ReadPixelDataInternal(xpos, ypos, 0, m, mBitCount);
				WriteBitfieldData(stream, currentBuffer, remainingBits, m, mBitCount);
			}
			else
			{
				unsigned int r;
				unsigned int g;
				unsigned int b;
				ReadPixelDataInternal(xpos, ypos, 0, r, rBitCount);
				ReadPixelDataInternal(xpos, ypos, 1, g, gBitCount);
				ReadPixelDataInternal(xpos, ypos, 2, b, bBitCount);
				WriteBitfieldData(stream, currentBuffer, remainingBits, b, bBitCount);
				WriteBitfieldData(stream, currentBuffer, remainingBits, g, gBitCount);
				WriteBitfieldData(stream, currentBuffer, remainingBits, r, rBitCount);
			}
			if(aBitCount > 0)
			{
				unsigned int a;
				ReadPixelDataInternal(xpos, ypos, 3, a, aBitCount);
				WriteBitfieldData(stream, currentBuffer, remainingBits, a, aBitCount);
			}
		}
	}
	FlushBitfieldData(stream, currentBuffer, remainingBits);

	return true;
}

//----------------------------------------------------------------------------------------
//Bitfield functions
//----------------------------------------------------------------------------------------
unsigned int Image::ReadBitfieldData(Stream::IStream& stream, unsigned char& currentBuffer, unsigned int& remainingBitsInBuffer, unsigned int dataBitCount) const
{
	unsigned int data = 0;
	unsigned int processedBits = 0;
	while(processedBits < dataBitCount)
	{
		//If the buffer is empty, load more data from the stream.
		if(remainingBitsInBuffer == 0)
		{
			stream.ReadData(currentBuffer);
			remainingBitsInBuffer = 8;
		}

		//Calculate the number of bits belonging to this data block which are currently
		//in the buffer.
		unsigned int remainingBitsInBitfield = dataBitCount - processedBits;
		unsigned int bitsToProcess = (remainingBitsInBitfield < remainingBitsInBuffer)? remainingBitsInBitfield: remainingBitsInBuffer;

		//Extract all the bits for this data block which are in the buffer, and combine
		//them with any bits we've already extracted.
		data <<= bitsToProcess;
		data |= ((unsigned int)currentBuffer >> (remainingBitsInBuffer - bitsToProcess)) & ((1 << bitsToProcess) - 1);

		//Now that we've extracted all the bits from the current buffer that we need,
		//update the total number of bits we've read for this data block, and update the
		//count of bits remaining in the buffer.
		processedBits += bitsToProcess;
		remainingBitsInBuffer -= bitsToProcess;
	}
	return data;
}

//----------------------------------------------------------------------------------------
void Image::WriteBitfieldData(Stream::IStream& stream, unsigned char& currentBuffer, unsigned int& remainingBitsInBuffer, unsigned int data, unsigned int dataBitCount) const
{
	unsigned int processedBits = 0;
	while(processedBits < dataBitCount)
	{
		//If the buffer is empty, load more data from the stream.
		if(remainingBitsInBuffer == 0)
		{
			stream.WriteData(currentBuffer);
			remainingBitsInBuffer = 8;
		}

		//Calculate the number of bits belonging to this bitfield in the buffer
		unsigned int remainingBitsInData = dataBitCount - processedBits;
		unsigned int bitsToProcess = (remainingBitsInData < remainingBitsInBuffer)? remainingBitsInData: remainingBitsInBuffer;

		//Extract the bits for this channel from the buffer, and combine them with any
		//bits we've already extracted.
		currentBuffer <<= bitsToProcess;
		currentBuffer |= (unsigned char)((data >> ((dataBitCount - processedBits) - bitsToProcess)) & ((1 << bitsToProcess) - 1));

		//Now that we've extracted the number of bits from the current buffer that we
		//need, update the count of the number of bits we've extracted from this channel,
		//and update the count of bits remaining in the buffer.
		processedBits += bitsToProcess;
		remainingBitsInBuffer -= bitsToProcess;
	}
}

//----------------------------------------------------------------------------------------
void Image::FlushBitfieldData(Stream::IStream& stream, unsigned char& currentBuffer, unsigned int& remainingBitsInBuffer) const
{
	if(remainingBitsInBuffer < 8)
	{
		currentBuffer <<= remainingBitsInBuffer;
		stream.WriteData(currentBuffer);
		remainingBitsInBuffer = 8;
	}
}

//----------------------------------------------------------------------------------------
//PNG functions
//----------------------------------------------------------------------------------------
bool Image::LoadPNGImage(Stream::IStream& stream)
{
	//Verify the file has a valid PNG file marker
	std::vector<unsigned char> header;
	stream.ReadData(header, 8);
	stream.SetStreamPos(0);
	if(png_sig_cmp(&header[0], 0, header.size()) != 0)
	{
		return false;
	}

	//Allocate png_struct
	png_structp pngStruct = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(pngStruct == NULL)
	{
		return false;
	}

	//Allocate png_info
	png_infop pngInfo = png_create_info_struct(pngStruct);
	if(pngInfo == NULL)
	{
		png_destroy_read_struct(&pngStruct, NULL, NULL);
		return false;
	}

	//Link our custom I/O functions for libpng
	png_set_read_fn(pngStruct, (void*)&stream, PNGReadData);

	//Parse the PNG file
	int pngTransforms = PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_EXPAND;
	png_read_png(pngStruct, pngInfo, pngTransforms, NULL);

	//Extract the decoded PNG pixel data
	png_bytepp pngPixelData = png_get_rows(pngStruct, pngInfo);
	unsigned int pngPixelSize = pngInfo->pixel_depth / 8;

	//Change our image format to match the decoded data
	unsigned int newImageWidth = pngInfo->width;
	unsigned int newImageHeight = pngInfo->height;
	DataFormat newDataFormat = DATAFORMAT_8BIT;
	PixelFormat newPixelFormat = PIXELFORMAT_RGB;
	if(((pngInfo->color_type & PNG_COLOR_MASK_COLOR) == 0) && ((pngInfo->color_type & PNG_COLOR_MASK_ALPHA) == 0))
	{
		newPixelFormat = PIXELFORMAT_M;
	}
	else if(((pngInfo->color_type & PNG_COLOR_MASK_COLOR) == 0) && ((pngInfo->color_type & PNG_COLOR_MASK_ALPHA) != 0))
	{
		newPixelFormat = PIXELFORMAT_MA;
	}
	else if(((pngInfo->color_type & PNG_COLOR_MASK_COLOR) != 0) && ((pngInfo->color_type & PNG_COLOR_MASK_ALPHA) == 0))
	{
		newPixelFormat = PIXELFORMAT_RGB;
	}
	else if(((pngInfo->color_type & PNG_COLOR_MASK_COLOR) != 0) && ((pngInfo->color_type & PNG_COLOR_MASK_ALPHA) != 0))
	{
		newPixelFormat = PIXELFORMAT_RGBA;
	}
	SetImageFormat(newImageWidth, newImageHeight, newPixelFormat, newDataFormat);

	//Build our image data from the decoded PNG pixel data
	for(unsigned int ypos = 0; ypos < imageHeight; ++ypos)
	{
		for(unsigned int xpos = 0; xpos < imageWidth; ++xpos)
		{
			if((pngInfo->color_type & PNG_COLOR_MASK_COLOR) == 0)
			{
				WritePixelDataInternal(xpos, ypos, 0, pngPixelData[ypos][xpos * pngPixelSize]);
			}
			else
			{
				WritePixelDataInternal(xpos, ypos, 0, pngPixelData[ypos][(xpos * pngPixelSize) + 0]);
				WritePixelDataInternal(xpos, ypos, 1, pngPixelData[ypos][(xpos * pngPixelSize) + 1]);
				WritePixelDataInternal(xpos, ypos, 2, pngPixelData[ypos][(xpos * pngPixelSize) + 2]);
			}
			if((pngInfo->color_type & PNG_COLOR_MASK_ALPHA) != 0)
			{
				WritePixelDataInternal(xpos, ypos, (dataPlaneCount - 1), pngPixelData[ypos][(xpos * pngPixelSize) + (pngPixelSize - 1)]);
			}
		}
	}

	//Free any allocated resources
	png_destroy_read_struct(&pngStruct, &pngInfo, NULL);

	return true;
}

//----------------------------------------------------------------------------------------
bool Image::SavePNGImage(Stream::IStream& stream)
{
	//Ensure the current image is valid before proceeding
	if(!ImageValid())
	{
		return false;
	}

	//Allocate png_struct
	png_structp pngStruct = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(pngStruct == NULL)
	{
		return false;
	}

	//Allocate png_info
	png_infop pngInfo = png_create_info_struct(pngStruct);
	if(pngInfo == NULL)
	{
		png_destroy_write_struct(&pngStruct, NULL);
		return false;
	}

	//Link our custom I/O functions for libpng
	png_set_write_fn(pngStruct, (void*)&stream, PNGWriteData, PNGFlushData);

	//Populate the PNG header information
	int pngColorType = 0;
	unsigned int bytesPerPixel;
	switch(pixelFormat)
	{
	case PIXELFORMAT_M:
		pngColorType = PNG_COLOR_TYPE_GRAY;
		bytesPerPixel = 1;
		break;
	case PIXELFORMAT_MA:
		pngColorType = PNG_COLOR_TYPE_GRAY_ALPHA;
		bytesPerPixel = 2;
		break;
	case PIXELFORMAT_RGB:
		pngColorType = PNG_COLOR_TYPE_RGB;
		bytesPerPixel = 3;
		break;
	case PIXELFORMAT_RGBA:
		pngColorType = PNG_COLOR_TYPE_RGB_ALPHA;
		bytesPerPixel = 4;
		break;
	}
	int pngBitDepth = 8;
	png_set_IHDR(pngStruct, pngInfo, imageWidth, imageHeight, pngBitDepth, pngColorType, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	//Write the PNG header
	png_write_info(pngStruct, pngInfo);

	//Write the image data to the file one scanline at a time
	png_bytep rowData = new png_byte[imageWidth * bytesPerPixel];
	for(unsigned int ypos = 0; ypos < imageHeight; ++ypos)
	{
		for(unsigned int xpos = 0; xpos < imageWidth; ++xpos)
		{
			for(unsigned int planeNo = 0; planeNo < GetDataPlaneCount(); ++planeNo)
			{
				unsigned char data;
				ReadPixelDataInternal(xpos, ypos, planeNo, data);
				unsigned int targetBufferPos = (xpos * bytesPerPixel) + planeNo;
				rowData[targetBufferPos] = (png_byte)data;
			}
		}
		png_write_row(pngStruct, rowData);
	}
	delete[] rowData;

	//Write the last data to the PNG file
	png_write_end(pngStruct, pngInfo);

	//Free any allocated resources
	png_destroy_write_struct(&pngStruct, &pngInfo);

	return true;
}

//----------------------------------------------------------------------------------------
void Image::PNGReadData(png_structp pngStruct, png_bytep data, png_size_t length)
{
	Stream::IStream& pngFile = *((Stream::IStream*)png_get_io_ptr(pngStruct));
	for(png_size_t i = 0; i < length; ++i)
	{
		unsigned char byte;
		pngFile.ReadData(byte);
		data[i] = byte;
	}
}

//----------------------------------------------------------------------------------------
void Image::PNGWriteData(png_structp pngStruct, png_bytep data, png_size_t length)
{
	Stream::IStream& pngFile = *((Stream::IStream*)png_get_io_ptr(pngStruct));
	for(png_size_t i = 0; i < length; ++i)
	{
		unsigned char byte = data[i];
		pngFile.WriteData(byte);
	}
}

//----------------------------------------------------------------------------------------
void Image::PNGFlushData(png_structp pngStruct)
{}

//----------------------------------------------------------------------------------------
//BMP functions
//----------------------------------------------------------------------------------------
bool Image::LoadBMPImage(Stream::IStream& stream)
{
	//Read and validate the "BM" file type marker from the file header. If this marker is
	//not present, abort any further processing.
	BITMAPFILEHEADER fileHeader;
	stream.ReadData(fileHeader.bfType);
	char typeByte1 = (char)fileHeader.bfType;
	char typeByte2 = (char)(fileHeader.bfType >> 8);
	if((typeByte1 != 'B') && (typeByte2 != 'M'))
	{
		return false;
	}

	//Load the bitmap file header
	unsigned int bitmapFileOffset = 0;
	stream.ReadData(fileHeader.bfSize);
	stream.ReadData(fileHeader.bfReserved1);
	stream.ReadData(fileHeader.bfReserved2);
	stream.ReadData(fileHeader.bfOffBits);
	bitmapFileOffset += sizeof(fileHeader);

	//Load the bitmap V3 info header
	bool rgbTripleColorTable = false;
	BITMAPINFOHEADER infoHeader;
	stream.ReadData(infoHeader.biSize);
	if(infoHeader.biSize == sizeof(BITMAPCOREHEADER))
	{
		//Load the bitmap V1 info header
		BITMAPCOREHEADER v1InfoHeader;
		stream.ReadData(v1InfoHeader.bcWidth);
		stream.ReadData(v1InfoHeader.bcHeight);
		stream.ReadData(v1InfoHeader.bcPlanes);
		stream.ReadData(v1InfoHeader.bcBitCount);

		//Note that in a V1 header, the width and height are unsigned. You can't specify
		//a top-down bitmap by giving a negative height value. Images are always stored
		//bottom-up.
		infoHeader.biWidth = (unsigned)v1InfoHeader.bcWidth;
		infoHeader.biHeight = (unsigned)v1InfoHeader.bcHeight;
		infoHeader.biPlanes = v1InfoHeader.bcPlanes;
		infoHeader.biBitCount = v1InfoHeader.bcBitCount;

		//Validate the number of bits per pixel
		switch(infoHeader.biBitCount)
		{
		default:
			//This V1 image uses an invalid number of bits per pixel. Abort any further
			//processing.
			return false;
		case 1:
		case 4:
		case 8:
		case 24:
			//The number of bits per pixel in this bitmap is a supported number. Proceed
			//with image decoding.
			break;
		}

		//Fill out the rest of the V3 info header with corresponding information
		infoHeader.biCompression = BI_RGB;
		//We can safely set this to 0, since we never actually use it in our load
		//routines. The BITMAPINFOHEADER spec also allows this member to be set to 0 for
		//BI_RGB images.
		infoHeader.biSizeImage = 0;
		infoHeader.biXPelsPerMeter = 0;
		infoHeader.biYPelsPerMeter = 0;
		infoHeader.biClrUsed = 0;
		infoHeader.biClrImportant = 0;

		//Since this is a V1 header, the colour table is stored using RGBTRIPLE objects
		//rather than RGBQUAD objects. We set a flag here so we know how to decode the
		//colour table later on.
		rgbTripleColorTable = true;
	}
	else if(infoHeader.biSize < sizeof(infoHeader))
	{
		//If the recorded size of the info header doesn't match the V1 header, and is
		//less than the V3 header, this is an unknown and unsupported header format,
		//possibly a V2 header. Abort any further processing.
		//##FIX## The V2 header is actually larger than the V3 header
		return false;
	}
	else
	{
		//If the header size is greater than or equal to the V3 info header, process it
		//as a V3 header.
		stream.ReadData(infoHeader.biWidth);
		stream.ReadData(infoHeader.biHeight);
		stream.ReadData(infoHeader.biPlanes);
		stream.ReadData(infoHeader.biBitCount);
		stream.ReadData(infoHeader.biCompression);
		stream.ReadData(infoHeader.biSizeImage);
		stream.ReadData(infoHeader.biXPelsPerMeter);
		stream.ReadData(infoHeader.biYPelsPerMeter);
		stream.ReadData(infoHeader.biClrUsed);
		stream.ReadData(infoHeader.biClrImportant);
	}
	bitmapFileOffset += infoHeader.biSize;

	//If the info header contains additional data, this is a newer header format, with
	//additional information provided. We attempt to decode this image using the V3
	//header info only. Extract the additional data and move past the info header.
	for(unsigned int i = sizeof(infoHeader); i < infoHeader.biSize; ++i)
	{
		//Note that we don't adjust bitmapFileOffset here, since we've already added the
		//total size of the header above.
		unsigned char temp;
		stream.ReadData(temp);
	}
	infoHeader.biSize = sizeof(infoHeader);

	//Sanity check to protect against bad data
	if(fileHeader.bfOffBits < bitmapFileOffset)
	{
		return false;
	}

	//Build the BITMAPINFO structure
	unsigned int sourceColorTableSize = fileHeader.bfOffBits - bitmapFileOffset;
	unsigned int newColorTableSize = rgbTripleColorTable? sourceColorTableSize + (sourceColorTableSize / 3): sourceColorTableSize;
	//Note that we allocate a bit more memory than we actually need here. By doing things
	//this way, we're not assuming anything about memory alignment, since we allow the
	//structure of the BITMAPINFO structure to determine where the colour table begins.
	//The few extra bytes of memory allocation for the bmiColors member in the BITMAPINFO
	//structure doesn't matter.
	unsigned int bitmapInfoSize = sizeof(BITMAPINFO) + newColorTableSize;
	BITMAPINFO* bitmapInfo = (BITMAPINFO*)new unsigned char[bitmapInfoSize];
	bitmapInfo->bmiHeader.biSize = sizeof(bitmapInfo->bmiHeader);
	bitmapInfo->bmiHeader.biWidth = infoHeader.biWidth;
	bitmapInfo->bmiHeader.biHeight = infoHeader.biHeight;
	bitmapInfo->bmiHeader.biPlanes = infoHeader.biPlanes;
	bitmapInfo->bmiHeader.biBitCount = infoHeader.biBitCount;
	bitmapInfo->bmiHeader.biCompression = infoHeader.biCompression;
	bitmapInfo->bmiHeader.biSizeImage = infoHeader.biSizeImage;
	bitmapInfo->bmiHeader.biXPelsPerMeter = infoHeader.biXPelsPerMeter;
	bitmapInfo->bmiHeader.biYPelsPerMeter = infoHeader.biYPelsPerMeter;
	bitmapInfo->bmiHeader.biClrUsed = infoHeader.biClrUsed;
	bitmapInfo->bmiHeader.biClrImportant = infoHeader.biClrImportant;

	//Copy the colour table into the BITMAPINFO structure
	unsigned char* colorTable = (unsigned char*)bitmapInfo->bmiColors;
	unsigned int sourceColorTableValuesRead = 0;
	unsigned int newColorTableValuesWritten = 0;
	while(sourceColorTableValuesRead < sourceColorTableSize)
	{
		unsigned char temp;
		stream.ReadData(temp);
		++sourceColorTableValuesRead;
		++bitmapFileOffset;
		*(colorTable + newColorTableValuesWritten++) = temp;
		//If the colour table is encoded using RGBTRIPLE objects, and we've just
		//processed the last entry in that structure, we need to write out the extra
		//rgbReserved byte to the new colour table to convert them to RGBQUAD objects.
		if(rgbTripleColorTable && ((sourceColorTableValuesRead % sizeof(RGBTRIPLE)) == 0))
		{
			*(colorTable + newColorTableValuesWritten++) = 0;
		}
	}

	//Pass the decoded BMP data onto the LoadDIBImage function
	bool result = LoadDIBImage(stream, *bitmapInfo);

	//Delete the memory allocated to the BITMAPINFO structure
	delete[] (unsigned char*)bitmapInfo;

	//Return the result
	return result;
}

//----------------------------------------------------------------------------------------
bool Image::SaveBMPImage(Stream::IStream& stream)
{
	//Ensure the current image is valid before proceeding
	if(!ImageValid())
	{
		return false;
	}

	//Estimate the required buffer size
	unsigned int bitsPerPixel = 24;
	unsigned int bytesPerPixel = ((bitsPerPixel + 7) / 8);
	unsigned int lineByteCount = imageWidth * bytesPerPixel;
	unsigned int linePaddingByteCount = 0;
	if((lineByteCount % sizeof(DWORD)) != 0)
	{
		linePaddingByteCount = sizeof(DWORD) - (lineByteCount % sizeof(DWORD));
	}
	unsigned int dataBufferSize = imageWidth * imageHeight * (lineByteCount + linePaddingByteCount);

	//Save the DIB data to a pixel buffer, and obtain the bitmap info header.
	Stream::Buffer dataBuffer(dataBufferSize);
	Stream::IStream::SizeType startPos = dataBuffer.GetStreamPos();
	BITMAPINFO bitmapInfo;
	bool saveDIBDataReturn = SaveDIBImage(dataBuffer, bitmapInfo);
	if(!saveDIBDataReturn)
	{
		return false;
	}
	Stream::IStream::SizeType endPos = dataBuffer.GetStreamPos();

	//Calculate the totals and offsets we need to write to the file
	unsigned int pixelDataOffset = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	unsigned int pixelDataSize = (unsigned int)(endPos - startPos);
	unsigned int fileSize = pixelDataOffset + pixelDataSize;

	//Write the bitmap file header to the file
	BITMAPFILEHEADER fileHeader;
	char typeByte1 = 'B';
	char typeByte2 = 'M';
	fileHeader.bfType = ((WORD)typeByte2 << 8) | (WORD)typeByte1;
	fileHeader.bfSize = fileSize;
	fileHeader.bfReserved1 = 0;
	fileHeader.bfReserved2 = 0;
	fileHeader.bfOffBits = pixelDataOffset;
	stream.WriteData(fileHeader.bfType);
	stream.WriteData(fileHeader.bfSize);
	stream.WriteData(fileHeader.bfReserved1);
	stream.WriteData(fileHeader.bfReserved2);
	stream.WriteData(fileHeader.bfOffBits);

	//Write the bitmap info header to the file. Note that our code here assumes there are
	//no bit masks, and no entries in the colour palette. This is currently true, since
	//we always write trivial 24-bit RGB image data, but we should modify this code to be
	//able to handle any possible bitmap data, so that future enhancements to SaveDIBImage
	//don't require changes to be made here.
	stream.WriteData(bitmapInfo.bmiHeader.biSize);
	stream.WriteData(bitmapInfo.bmiHeader.biWidth);
	stream.WriteData(bitmapInfo.bmiHeader.biHeight);
	stream.WriteData(bitmapInfo.bmiHeader.biPlanes);
	stream.WriteData(bitmapInfo.bmiHeader.biBitCount);
	stream.WriteData(bitmapInfo.bmiHeader.biCompression);
	stream.WriteData(bitmapInfo.bmiHeader.biSizeImage);
	stream.WriteData(bitmapInfo.bmiHeader.biXPelsPerMeter);
	stream.WriteData(bitmapInfo.bmiHeader.biYPelsPerMeter);
	stream.WriteData(bitmapInfo.bmiHeader.biClrUsed);
	stream.WriteData(bitmapInfo.bmiHeader.biClrImportant);

	//Write the pixel data to the file
	for(unsigned int i = 0; i < pixelDataSize; ++i)
	{
		unsigned char temp;
		dataBuffer.ReadData(temp);
		stream.WriteData(temp);
	}

	return true;
}

//----------------------------------------------------------------------------------------
//Win32 DIB functions (Device-Independent Bitmap)
//----------------------------------------------------------------------------------------
bool Image::LoadDIBImage(Stream::IStream& stream, const BITMAPINFO& bitmapInfo)
{
	//Read the image dimensions. Also check if the image is stored as a bottom-up image
	//or a top-down image. A top-down image is indicated by a negative value for height.
	bool imageBottomUp = true;
	unsigned int newImageWidth = (unsigned int)bitmapInfo.bmiHeader.biWidth;
	unsigned int newImageHeight = (unsigned int)bitmapInfo.bmiHeader.biHeight;
	if(bitmapInfo.bmiHeader.biHeight < 0)
	{
		imageBottomUp = false;
		newImageHeight = (unsigned int)(-bitmapInfo.bmiHeader.biHeight);
	}

	//Verify the number of planes in the bitmap
	if(bitmapInfo.bmiHeader.biPlanes != 1)
	{
		//If the header specifies more than 1 plane, we don't know how to decode the
		//image. Abort any further processing.
		return false;
	}

	//Determine the pixel data format, and work out how to decode the pixel data.
	PixelFormat newPixelFormat;
	bool useBitmasks = false;
	bool rle4Used = false;
	bool rle8Used = false;
	switch(bitmapInfo.bmiHeader.biCompression)
	{
	case BI_RGB:
		//The image uses literal RGB values
		newPixelFormat = PIXELFORMAT_RGB;
		break;
	case BI_RLE8:
		//The image uses RLE8 compression
		rle8Used = true;
		break;
	case BI_RLE4:
		//The image uses RLE4 compression
		rle4Used = true;
		break;
	case BI_BITFIELDS:
		//The image uses packed literal RGB values
		newPixelFormat = PIXELFORMAT_RGB;
		useBitmasks = true;
		break;
	case BI_JPEG:
	case BI_PNG:
		//JPEG and PNG compression are not currently supported in a DIB structure. Abort
		//any further processing.
		return false;
	default:
		//We've encountered an unknown compression value. Abort any further processing.
		return false;
	}

	DataFormat newDataFormat;
	unsigned int bitsPerPixel = bitmapInfo.bmiHeader.biBitCount;
	switch(bitsPerPixel)
	{
	default:
		//This image contains an unsupported number of bits per pixel. Abort any further
		//processing.
		return false;
	case 1:
	case 4:
	case 8:
	case 16:
	case 24:
	case 32:
		//The number of bits per pixel in this bitmap is a supported number. Proceed with
		//image decoding.
		break;
	}
	//We decode all bitmap data into an 8-bit format
	newDataFormat = DATAFORMAT_8BIT;

	//Apply the new image properties
	SetImageFormat(newImageWidth, newImageHeight, newPixelFormat, newDataFormat);

	//Decode the pixel data
	if((rle4Used && (bitsPerPixel == 4)) || (rle8Used && (bitsPerPixel == 8)))
	{
		//This is a compressed bitmap. Proceed with image decoding.
		RGBQUAD* colorTable = (RGBQUAD*)((unsigned char*)(&bitmapInfo.bmiHeader) + bitmapInfo.bmiHeader.biSize);
		unsigned int xpos = 0;
		unsigned int ypos = 0;
		unsigned int writeYPos = imageBottomUp? ((imageHeight - 1) - ypos): ypos;

		bool done = false;
		while(!done)
		{
			unsigned char repeatCount;
			stream.ReadData(repeatCount);
			if(repeatCount != 0)
			{
				//Encoded mode:
				//CCCCCCCC IIIIIIII
				//Where C > 0 and the specified colour index value is used the specified
				//number of times
				unsigned char colorIndex;
				stream.ReadData(colorIndex);
				while(repeatCount > 0)
				{
					//Extract the individual index entries from the index block, and
					//decode the pixel data using the palette.
					unsigned int indexBlockBitCount = 8;
					unsigned int indexValuesPerBlock = indexBlockBitCount / bitsPerPixel;
					unsigned int indexValueMask = (1 << bitsPerPixel) - 1;
					for(unsigned int i = 0; (i < indexValuesPerBlock) && (repeatCount > 0); ++i)
					{
						//Extract the palette index for the next literal
						unsigned int indexValue = (colorIndex >> (indexBlockBitCount - ((i + 1) * bitsPerPixel))) & indexValueMask;

						//Read the RGB values from the palette based on the palette
						//index
						unsigned int r = (unsigned int)colorTable[indexValue].rgbRed;
						unsigned int g = (unsigned int)colorTable[indexValue].rgbGreen;
						unsigned int b = (unsigned int)colorTable[indexValue].rgbBlue;

						//Validate the write location. If the compressed image data
						//doesn't conform to the specified width and height of the
						//image, abort further processing.
						if((xpos > (imageWidth - 1)) || (ypos > (imageHeight - 1)))
						{
							return false;
						}

						//Write the decoded pixel data to the image
						WritePixelDataInternal(xpos, writeYPos, 0, r, 8);
						WritePixelDataInternal(xpos, writeYPos, 1, g, 8);
						WritePixelDataInternal(xpos, writeYPos, 2, b, 8);

						//Advance to the next literal
						++xpos;
						--repeatCount;
					}
				}
			}
			else
			{
				unsigned char literalCount;
				stream.ReadData(literalCount);
				if(literalCount == 0)
				{
					//End of line flag:
					//00000000 00000001
					++ypos;
					writeYPos = imageBottomUp? ((imageHeight - 1) - ypos): ypos;
				}
				else if(literalCount == 1)
				{
					//End of bitmap flag:
					//00000000 00000001
					done = true;
				}
				else if(literalCount == 2)
				{
					//Delta flag:
					//00000000 00000010 XXXXXXXX YYYYYYYY
					//Where X is the unsigned horizontal delta, and Y is the unsigned
					//vertical delta.
					unsigned char deltaX;
					unsigned char deltaY;
					stream.ReadData(deltaX);
					stream.ReadData(deltaY);
					xpos += deltaX;
					ypos += deltaY;
					writeYPos = imageBottomUp? ((imageHeight - 1) - ypos): ypos;
				}
				else
				{
					//Absolute mode:
					//00000000 CCCCCCCC IIIIIIII IIIIIIII
					//Where C > 2, and the specified number of colour index values
					//follow, padded to a WORD boundary.
					unsigned char dataBuffer;
					unsigned int indexBlock = 0;

					while(literalCount > 0)
					{
						//Read the next 2-byte index block from the stream
						stream.ReadData(dataBuffer);
						indexBlock = (unsigned int)dataBuffer << 8;
						stream.ReadData(dataBuffer);
						indexBlock |= dataBuffer;

						//Extract the individual index entries from the index block, and
						//decode the pixel data using the palette.
						unsigned int indexBlockBitCount = 16;
						unsigned int indexValuesPerBlock = indexBlockBitCount / bitsPerPixel;
						unsigned int indexValueMask = (1 << bitsPerPixel) - 1;
						for(unsigned int i = 0; (i < indexValuesPerBlock) && (literalCount > 0); ++i)
						{
							//Extract the palette index for the next literal
							unsigned int indexValue = (indexBlock >> (indexBlockBitCount - ((i + 1) * bitsPerPixel))) & indexValueMask;

							//Read the RGB values from the palette based on the palette
							//index
							unsigned int r = (unsigned int)colorTable[indexValue].rgbRed;
							unsigned int g = (unsigned int)colorTable[indexValue].rgbGreen;
							unsigned int b = (unsigned int)colorTable[indexValue].rgbBlue;

							//Validate the write location. If the compressed image data
							//doesn't conform to the specified width and height of the
							//image, abort further processing.
							if((xpos > (imageWidth - 1)) || (ypos > (imageHeight - 1)))
							{
								return false;
							}

							//Write the decoded pixel data to the image
							WritePixelDataInternal(xpos, writeYPos, 0, r, 8);
							WritePixelDataInternal(xpos, writeYPos, 1, g, 8);
							WritePixelDataInternal(xpos, writeYPos, 2, b, 8);

							//Advance to the next literal
							++xpos;
							--literalCount;
						}
					}
				}
			}
		}
	}
	else
	{
		//This is an uncompressed bitmap. Proceed with image decoding.

		//Calculate the amount of padding on each line. Lines are padded out to DWORD
		//boundaries.
		unsigned int bytesPerPixel = ((bitsPerPixel + 7) / 8);
		unsigned int lineByteCount = imageWidth * bytesPerPixel;
		unsigned int linePaddingByteCount = 0;
		if((lineByteCount % sizeof(DWORD)) != 0)
		{
			linePaddingByteCount = sizeof(DWORD) - (lineByteCount % sizeof(DWORD));
		}

		//Decode the pixel data
		for(unsigned int ypos = 0; ypos < imageHeight; ++ypos)
		{
			//Calculate the current ypos to write this row to. If the image is stored bottom
			//up in the stream, we need to read it bottom up as well.
			unsigned int writeYPos = imageBottomUp? ((imageHeight - 1) - ypos): ypos;

			unsigned char dataBuffer;
			for(unsigned int xpos = 0; xpos < imageWidth; ++xpos)
			{
				unsigned int r = 0;
				unsigned int g = 0;
				unsigned int b = 0;
				unsigned int rBitCount = 0;
				unsigned int gBitCount = 0;
				unsigned int bBitCount = 0;

				if((bitsPerPixel == 1) || (bitsPerPixel == 4) || (bitsPerPixel == 8))
				{
					//This is a palletized image. Decode the colour values using the colour
					//table.
					rBitCount = 8;
					gBitCount = 8;
					bBitCount = 8;

					//Extract the palette index for this pixel
					RGBQUAD* colorTable = (RGBQUAD*)((unsigned char*)(&bitmapInfo.bmiHeader) + bitmapInfo.bmiHeader.biSize);
					unsigned int dataBufferBitCount = 8;
					unsigned int entryBitCount = bitsPerPixel;
					unsigned int entryBitMask = (1 << entryBitCount) - 1;
					unsigned int entriesInDataBuffer = dataBufferBitCount / entryBitCount;
					unsigned int currentEntryInDataBuffer = xpos % entriesInDataBuffer;
					if(currentEntryInDataBuffer == 0)
					{
						//If we're decoding the first entry in a new buffer, we need to read
						//a new data block from the stream.
						stream.ReadData(dataBuffer);
					}
					unsigned int dataEntry = (dataBuffer >> (dataBufferBitCount - ((currentEntryInDataBuffer + 1) * entryBitCount))) & entryBitMask;

					//Read the RGB values from the palette based on the palette index
					r = (unsigned int)colorTable[dataEntry].rgbRed;
					g = (unsigned int)colorTable[dataEntry].rgbGreen;
					b = (unsigned int)colorTable[dataEntry].rgbBlue;
				}
				else if(bitsPerPixel == 16)
				{
					//The image data contains literal RGB values, with up to 16 bits per
					//pixel. Default bit masks are given below to decode the pixel data.
					unsigned int rMask = 0x7C00;
					unsigned int gMask = 0x03E0;
					unsigned int bMask = 0x001F;
					if(useBitmasks)
					{
						//If custom bitmasks have been provided to define how to decode the
						//data, read the bitmasks in.
						DWORD* maskTable = (DWORD*)((unsigned char*)(&bitmapInfo.bmiHeader) + bitmapInfo.bmiHeader.biSize);
						rMask = (unsigned int)maskTable[0];
						gMask = (unsigned int)maskTable[1];
						bMask = (unsigned int)maskTable[2];
					}
					//Based on the bitmasks, calculate the actual number of bits per colour
					//channel.
					rBitCount = GetSetBitCount(rMask);
					gBitCount = GetSetBitCount(gMask);
					bBitCount = GetSetBitCount(bMask);

					//Read a data entry in, and extract the individual RGB values based on
					//the bitmasks.
					WORD rawData;
					stream.ReadData(rawData);
					r = (unsigned char)MaskData((unsigned int)rawData, rMask);
					g = (unsigned char)MaskData((unsigned int)rawData, gMask);
					b = (unsigned char)MaskData((unsigned int)rawData, bMask);
				}
				else if(bitsPerPixel == 24)
				{
					//The image data contains literal RGB values, with 24 bits per pixel.
					rBitCount = 8;
					gBitCount = 8;
					bBitCount = 8;

					//Read in the RGB values. Note that the data is actually stored in the
					//order BGR in the stream.
					unsigned char rData;
					unsigned char gData;
					unsigned char bData;
					stream.ReadData(bData);
					stream.ReadData(gData);
					stream.ReadData(rData);
					r = rData;
					g = gData;
					b = bData;
				}
				else if(bitsPerPixel == 32)
				{
					//The image data contains literal RGB values, with up to 32 bits per
					//pixel. Default bit masks are given below to decode the pixel data.
					unsigned int rMask = 0x00FF0000;
					unsigned int gMask = 0x0000FF00;
					unsigned int bMask = 0x000000FF;
					if(useBitmasks)
					{
						//If custom bitmasks have been provided to define how to decode the
						//data, read the bitmasks in.
						DWORD* maskTable = (DWORD*)((unsigned char*)(&bitmapInfo.bmiHeader) + bitmapInfo.bmiHeader.biSize);
						rMask = (unsigned int)maskTable[0];
						gMask = (unsigned int)maskTable[1];
						bMask = (unsigned int)maskTable[2];
					}
					//Based on the bitmasks, calculate the actual number of bits per colour
					//channel.
					rBitCount = GetSetBitCount(rMask);
					gBitCount = GetSetBitCount(gMask);
					bBitCount = GetSetBitCount(bMask);

					//Read a data entry in, and extract the individual RGB values based on
					//the bitmasks.
					DWORD rawData;
					stream.ReadData(rawData);
					r = MaskData((unsigned int)rawData, rMask);
					g = MaskData((unsigned int)rawData, gMask);
					b = MaskData((unsigned int)rawData, bMask);
				}

				//Write the extracted pixel data to the image
				WritePixelDataInternal(xpos, writeYPos, 0, r, rBitCount);
				WritePixelDataInternal(xpos, writeYPos, 1, g, gBitCount);
				WritePixelDataInternal(xpos, writeYPos, 2, b, bBitCount);
			}

			//The data is padded out to DWORD boundaries. We extract the padding here and
			//move on to the next line.
			for(unsigned int i = 0; i < linePaddingByteCount; ++i)
			{
				unsigned char temp;
				stream.ReadData(temp);
			}
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool Image::SaveDIBImage(Stream::IStream& stream, BITMAPINFO& bitmapInfo)
{
	//Ensure the current image is valid before proceeding
	if(!ImageValid())
	{
		return false;
	}

	//Calculate the data format for the DIB image
	unsigned int bitsPerPixel = 24;
	unsigned int bytesPerPixel = ((bitsPerPixel + 7) / 8);
	unsigned int lineByteCount = imageWidth * bytesPerPixel;
	bool imageBottomUp = true;

	//Calculate the amount of padding on each line. Lines are padded out to DWORD
	//boundaries.
	unsigned int linePaddingByteCount = 0;
	if((lineByteCount % sizeof(DWORD)) != 0)
	{
		linePaddingByteCount = sizeof(DWORD) - (lineByteCount % sizeof(DWORD));
	}

	//Populate the header information for the DIB image
	bitmapInfo.bmiHeader.biSize = (DWORD)sizeof(bitmapInfo.bmiHeader);
	bitmapInfo.bmiHeader.biWidth = (LONG)imageWidth;
	bitmapInfo.bmiHeader.biHeight = imageBottomUp? (LONG)imageHeight: -((LONG)imageHeight);
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biBitCount = (WORD)bitsPerPixel;
	bitmapInfo.bmiHeader.biCompression = BI_RGB;
	bitmapInfo.bmiHeader.biSizeImage = (DWORD)(imageHeight * (lineByteCount + linePaddingByteCount));
	bitmapInfo.bmiHeader.biXPelsPerMeter = 0;
	bitmapInfo.bmiHeader.biYPelsPerMeter = 0;
	bitmapInfo.bmiHeader.biClrUsed = 0;
	bitmapInfo.bmiHeader.biClrImportant = 0;

	//Build a DIB bitmap using RGB encoding from the current image data
	for(unsigned int ypos = 0; ypos < imageHeight; ++ypos)
	{
		unsigned int readYPos = imageBottomUp? ((imageHeight - 1) - ypos): ypos;
		for(unsigned int xpos = 0; xpos < imageWidth; ++xpos)
		{
			//Save the pixel data. Note that alpha values are discarded.
			if((pixelFormat == PIXELFORMAT_M) || (pixelFormat == PIXELFORMAT_MA))
			{
				unsigned char data;
				ReadPixelDataInternal(xpos, readYPos, 0, data);
				//We convert the monochrome image to an RGB image here
				stream.WriteData(data);
				stream.WriteData(data);
				stream.WriteData(data);
			}
			else if((pixelFormat == PIXELFORMAT_RGB) || (pixelFormat == PIXELFORMAT_RGBA))
			{
				unsigned char r;
				unsigned char g;
				unsigned char b;
				ReadPixelDataInternal(xpos, readYPos, 0, r);
				ReadPixelDataInternal(xpos, readYPos, 1, g);
				ReadPixelDataInternal(xpos, readYPos, 2, b);
				//Note that the byte order is BGR in memory. This follows the format of
				//the RGBTRIPLE structure.
				stream.WriteData(b);
				stream.WriteData(g);
				stream.WriteData(r);
			}
		}

		//Write the line padding to the end of the line
		for(unsigned int i = 0; i < linePaddingByteCount; ++i)
		{
			stream.WriteData(unsigned char(0));
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------
unsigned int Image::GetSetBitCount(unsigned int data) const
{
	unsigned int setBitCount = 0;
	unsigned int bitCount = sizeof(unsigned int) * CHAR_BIT;
	for(unsigned int i = 0; i < bitCount; ++i)
	{
		if((data & (1 << i)) != 0)
		{
			++setBitCount;
		}
	}
	return setBitCount;
}

//----------------------------------------------------------------------------------------
unsigned int Image::MaskData(unsigned int data, unsigned int bitMask) const
{
	unsigned int maskedData = 0;
	unsigned int setBitCount = 0;
	unsigned int bitCount = sizeof(unsigned int) * CHAR_BIT;
	for(unsigned int i = 0; i < bitCount; ++i)
	{
		unsigned int currentBitMask = (1 << i);
		if((bitMask & currentBitMask) != 0)
		{
			maskedData |= ((data & currentBitMask) >> i) << setBitCount;
			++setBitCount;
		}
	}
	return maskedData;
}

//----------------------------------------------------------------------------------------
//Image verification methods
//----------------------------------------------------------------------------------------
bool Image::ImageValid() const
{
	//Validate the current image width and height
	if((imageWidth <= 0) || (imageHeight <= 0))
	{
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
//Resampling functions
//----------------------------------------------------------------------------------------
void Image::ResampleNearest(unsigned int newWidth, unsigned int newHeight)
{
	Image oldImage(*this);
	ResampleNearest(oldImage, newWidth, newHeight);
}

//----------------------------------------------------------------------------------------
void Image::ResampleNearest(const IImage& oldImage, unsigned int newWidth, unsigned int newHeight)
{
	//Apply new new specified image format
	SetImageFormat(newWidth, newHeight, oldImage.GetPixelFormat(), oldImage.GetDataFormat());

	//Ensure that the old image and this image have valid image dimensions specified
	//before attempting the resampling algorithm.
	if((oldImage.GetImageWidth() <= 0) || (oldImage.GetImageHeight() <= 0) || (imageWidth <= 0) || (imageHeight <= 0))
	{
		return;
	}

	unsigned int maxOldImageXPos = (oldImage.GetImageWidth() - 1);
	unsigned int maxOldImageYPos = (oldImage.GetImageHeight() - 1);
	float imageWidthConversionRatio = (float)maxOldImageXPos / (float)imageWidth;
	float imageHeightConversionRatio = (float)maxOldImageYPos / (float)imageHeight;
	for(unsigned int ypos = 0; ypos < imageHeight; ++ypos)
	{
		float yposNormalized = (float)ypos * imageHeightConversionRatio;
		unsigned int yposOld = (unsigned int)(yposNormalized + 0.5f);
		for(unsigned int xpos = 0; xpos < imageWidth; ++xpos)
		{
			float xposNormalized = (float)xpos * imageWidthConversionRatio;
			unsigned int xposOld = (unsigned int)(xposNormalized + 0.5f);
			for(unsigned int plane = 0; plane < dataPlaneCount; ++plane)
			{
				PixelData pixelData;
				oldImage.GetRawPixelData(xposOld, yposOld, plane, pixelData);
				SetRawPixelDataInternal(xpos, ypos, plane, pixelData);
			}
		}
	}
}

//----------------------------------------------------------------------------------------
void Image::ResampleBilinear(unsigned int newWidth, unsigned int newHeight)
{
	Image oldImage(*this);
	ResampleBilinear(oldImage, newWidth, newHeight);
}

//----------------------------------------------------------------------------------------
void Image::ResampleBilinear(const IImage& oldImage, unsigned int newWidth, unsigned int newHeight)
{
	//Apply new new specified image format
	SetImageFormat(newWidth, newHeight, oldImage.GetPixelFormat(), oldImage.GetDataFormat());

	//Ensure that the old image and this image have valid image dimensions specified
	//before attempting the resampling algorithm.
	if((oldImage.GetImageWidth() <= 0) || (oldImage.GetImageHeight() <= 0) || (imageWidth <= 0) || (imageHeight <= 0))
	{
		return;
	}

	//This is a basic bilinear resampling algorithm. It will not cope well when reducing
	//a dimension of the source image by a factor of more than 1.5x.
	//float imageWidthConversionRatio = (float)oldImage.GetImageWidth() / (float)imageWidth;
	//float imageHeightConversionRatio = (float)oldImage.GetImageHeight() / (float)imageHeight;
	//for(unsigned int ypos = 0; ypos < imageHeight; ++ypos)
	//{
	//	float yposNormalized = (float)ypos * imageHeightConversionRatio;
	//	unsigned int yposOld0 = (unsigned int)(yposNormalized - 0.5);
	//	unsigned int yposOld1 = (unsigned int)(yposNormalized + 0.5);
	//	float yposNew = (yposNormalized * (float)oldImage.GetImageHeight()) - (float)((unsigned int)(yposNormalized * (float)oldImage.GetImageHeight()));
	//	for(unsigned int xpos = 0; xpos < imageWidth; ++xpos)
	//	{
	//		float xposNormalized = (float)xpos * imageWidthConversionRatio;
	//		unsigned int xposOld0 = (unsigned int)(xposNormalized - 0.5);
	//		unsigned int xposOld1 = (unsigned int)(xposNormalized + 0.5);
	//		float xposNew = (xposNormalized * (float)oldImage.GetImageWidth()) - (float)((unsigned int)(xposNormalized * (float)oldImage.GetImageWidth()));
	//		for(unsigned int plane = 0; plane < dataPlaneCount; ++plane)
	//		{
	//			float x0y0;
	//			float x1y0;
	//			float x0y1;
	//			float x1y1;
	//			oldImage.ReadPixelData(xposOld0, yposOld0, plane, x0y0);
	//			oldImage.ReadPixelData(xposOld1, yposOld0, plane, x1y0);
	//			oldImage.ReadPixelData(xposOld0, yposOld1, plane, x0y1);
	//			oldImage.ReadPixelData(xposOld1, yposOld1, plane, x1y1);
	//			float finalData = x0y0 * (1.0f - xposNew) * (1.0f - yposNew);
	//			finalData += x1y0 * xposNew * (1.0f - yposNew);
	//			finalData += x0y1 * (1.0f - xposNew) * yposNew;
	//			finalData += x1y1 * xposNew * yposNew;
	//			WritePixelDataInternal(xpos, ypos, plane, finalData);
	//		}
	//	}
	//}

	//This method is much more sophisticated than the one above, and will give the best
	//result possible from linear resampling when either upsampling or downsampling,
	//regardless of the respective dimensions of the source and target images.
	float imageWidthConversionRatio = (float)oldImage.GetImageWidth() / (float)imageWidth;
	float imageHeightConversionRatio = (float)oldImage.GetImageHeight() / (float)imageHeight;
	for(unsigned int ypos = 0; ypos < imageHeight; ++ypos)
	{
		//Calculate the beginning and end of the sample region on the Y axis in the
		//source image, which is being mapped onto this pixel in the target image. Note
		//that because we're adding 1 to the current pixel location, the second sample
		//point may be past the end of the image, but this is ok, because this sample
		//point is really a limit. We protect against reading past the boundaries of the
		//image further below, and if this is attempted, this dud sample will be assigned
		//a weight of zero.
		float firstSamplePointY = (float)ypos * imageHeightConversionRatio;
		float lastSamplePointY = (float)(ypos + 1) * imageHeightConversionRatio;

		//Calculate the total domain, or length, of this sample region on the Y axis.
		float totalDomainY = lastSamplePointY - firstSamplePointY;

		//Calculate the first and last pixels of interest from the source region.
		unsigned int firstSamplePosY = (unsigned int)firstSamplePointY;
		unsigned int lastSamplePosY = (unsigned int)lastSamplePointY;

		//Generate each pixel in this line
		for(unsigned int xpos = 0; xpos < imageWidth; ++xpos)
		{
			//Calculate the beginning and end of the sample region on the X axis in the
			//source image, which is being mapped onto this pixel in the target image.
			//Note that because we're adding 1 to the current pixel location, the second
			//sample point may be past the end of the image, but this is ok, because this
			//sample point is really a limit. We protect against reading past the
			//boundaries of the image further below, and if this is attempted, this dud
			//sample will be assigned a weight of zero.
			float firstSamplePointX = (float)xpos * imageWidthConversionRatio;
			float lastSamplePointX = (float)(xpos + 1) * imageWidthConversionRatio;

			//Calculate the total domain, or length, of this sample region on the X axis.
			float totalDomainX = lastSamplePointX - firstSamplePointX;

			//Calculate the first and last pixels of interest from the source region
			unsigned int firstSamplePosX = (unsigned int)firstSamplePointX;
			unsigned int lastSamplePosX = (unsigned int)lastSamplePointX;

			//Calculate the total domain, or area, of the sample region in the source
			//image. We use this to normalize the sampled data back to an area of one
			//pixel at the end.
			float totalDomain = totalDomainX * totalDomainY;

			//Calculate the sample value in each plane
			for(unsigned int plane = 0; plane < dataPlaneCount; ++plane)
			{
				//Combine sample values from the source image, on both the X and Y axis,
				//with their respective weightings.
				float finalSample = 0.0f;
				for(unsigned int currentSampleY = firstSamplePosY; currentSampleY <= lastSamplePosY; ++currentSampleY)
				{
					float sampleStartPointY = 0.0f;
					if(currentSampleY == firstSamplePosY)
					{
						sampleStartPointY = firstSamplePointY - (float)firstSamplePosY;
					}
					float sampleEndPointY = 1.0f;
					if(currentSampleY == lastSamplePosY)
					{
						sampleEndPointY = lastSamplePointY - (float)lastSamplePosY;
					}
					float sampleWeightY = sampleEndPointY - sampleStartPointY;

					for(unsigned int currentSampleX = firstSamplePosX; currentSampleX <= lastSamplePosX; ++currentSampleX)
					{
						float sampleStartPointX = 0.0f;
						if(currentSampleX == firstSamplePosX)
						{
							sampleStartPointX = firstSamplePointX - (float)firstSamplePosX;
						}
						float sampleEndPointX = 1.0f;
						if(currentSampleX == lastSamplePosX)
						{
							sampleEndPointX = lastSamplePointX - (float)lastSamplePosX;
						}
						float sampleWeightX = sampleEndPointX - sampleStartPointX;

						float sample;
						oldImage.ReadPixelData(currentSampleX % oldImage.GetImageWidth(), currentSampleY % oldImage.GetImageHeight(), plane, sample);
						finalSample += sample * sampleWeightX * sampleWeightY;
					}
				}

				//Normalize the sample value back to a single pixel value, by dividing it
				//by the total area of the sample region in the source image.
				finalSample /= totalDomain;

				//Write the generated pixel to the image
				WritePixelDataInternal(xpos, ypos, plane, finalSample);
			}
		}
	}
}
