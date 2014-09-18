#include "Debug/Debug.pkg"

//----------------------------------------------------------------------------------------
//PCX structures
//----------------------------------------------------------------------------------------
struct Image::PCXFileHeader
{
	unsigned char manufacturer;		//0x00
	unsigned char version;			//0x01
	unsigned char encoding;			//0x02
	unsigned char bitsPerPixel;		//0x03
	unsigned short xmin;			//0x04-0x05
	unsigned short ymin;			//0x06-0x07
	unsigned short xmax;			//0x08-0x09
	unsigned short ymax;			//0x0A-0x0B
	unsigned short hdpi;			//0x0C-0x0D
	unsigned short vdpi;			//0x0E-0x0F
	unsigned char colorMap[48];		//0x10-0x40
	unsigned char reserved1;		//0x41
	unsigned char numPlanes;		//0x42
	unsigned short bytesPerLine;	//0x43-0x44
	unsigned short paletteInfo;		//0x45-0x46
	unsigned short hscreenSize;		//0x47-0x48
	unsigned short vscreenSize;		//0x49-0x4A
	unsigned char reserved2[54];	//0x4B-0x80
};

//----------------------------------------------------------------------------------------
enum Image::PCXImageFormat
{
//	PCXIMAGEFORMAT_MONO,
//	PCXIMAGEFORMAT_CGA,
//	PCXIMAGEFORMAT_EGA,
//	PCXIMAGEFORMAT_EGA_AND_VGA,
//	PCXIMAGEFORMAT_EXTENDED_VGA,
//	PCXIMAGEFORMAT_EXTENDED_VGA_AND_XGA,
	PCXIMAGEFORMAT_PALETTE,
	PCXIMAGEFORMAT_LITERAL
};

//----------------------------------------------------------------------------------------
//TIFF structures
//----------------------------------------------------------------------------------------
struct Image::TIFFStreamManager
{
	//Constructors
	TIFFStreamManager(Stream::IStream& astream)
	:stream(astream)
	{}

	TIFF* Open(std::string imageName, std::string mode)
	{
		return TIFFClientOpen(imageName.c_str(), mode.c_str(), (thandle_t)this,
			ReadProc, WriteProc,
			SeekProc, CloseProc, SizeProc,
			MapProc,
			UnmapProc);
	}

private:
	//Callback handlers
	static tmsize_t ReadProc(thandle_t fd, void* buf, tmsize_t size)
	{
		TIFFStreamManager* obj = (TIFFStreamManager*)fd;
		if(!obj->stream.ReadData((unsigned char*)buf, size))
		{
			size = 0;
		}
		return size;
	}

	static tmsize_t WriteProc(thandle_t fd, void* buf, tmsize_t size)
	{
		TIFFStreamManager* obj = (TIFFStreamManager*)fd;
		obj->stream.WriteData((unsigned char*)buf, size);
		return size;
	}

	static uint64 SeekProc(thandle_t fd, uint64 off, int whence)
	{
		TIFFStreamManager* obj = (TIFFStreamManager*)fd;
		Stream::IStream::SizeType newStreamPos = 0;

		switch(whence)
		{
		default:
		case SEEK_SET:
			newStreamPos = (Stream::IStream::SizeType)off;
			break;
		case SEEK_CUR:
			newStreamPos = obj->stream.GetStreamPos() + (Stream::IStream::SizeType)off;
			break;
		case SEEK_END:
			newStreamPos = obj->stream.Size() + (Stream::IStream::SizeType)off;
			break;
		}

		obj->stream.SetStreamPos(newStreamPos);
		return (uint64)newStreamPos;
	}

	static int CloseProc(thandle_t fd)
	{
		return 0;
	}

	static uint64 SizeProc(thandle_t fd)
	{
		TIFFStreamManager* obj = (TIFFStreamManager*)fd;
		return (uint64)obj->stream.Size();
	}

	static int MapProc(thandle_t fd, void** pbase, toff_t* psize)
	{
		return 0;
	}

	static void UnmapProc(thandle_t fd, void* base, toff_t size)
	{
	}

private:
	Stream::IStream& stream;
};

//----------------------------------------------------------------------------------------
//TGA structures
//----------------------------------------------------------------------------------------
struct Image::TGAFileHeader
{
	//Field 1 - ID Length
	unsigned char idLength;
	//Field 2 - Color Map Type
	unsigned char colorMapType;
	//Field 3 - Image Type
	unsigned char imageType;
	//Field 4 - Color Map Specification
	unsigned short colorMapFirstEntry;
	unsigned short colorMapLength;
	unsigned char colorMapBitsPerEntry;
	//Field 5 - Image Specification
	unsigned short imageOriginX;
	unsigned short imageOriginY;
	unsigned short imageWidth;
	unsigned short imageHeight;
	unsigned char bitsPerPixel;
	unsigned char imageDescriptor;
};

//----------------------------------------------------------------------------------------
struct Image::TGAColorMapEntry
{
	unsigned int r;
	unsigned int g;
	unsigned int b;
	unsigned int a;
};

//----------------------------------------------------------------------------------------
enum Image::TGAImageType
{
	TGAIMAGETYPE_NONE = 0,
	TGAIMAGETYPE_PALETTE = 1,
	TGAIMAGETYPE_RGB = 2,
	TGAIMAGETYPE_MONOCHROME = 3,
	TGAIMAGETYPE_RLEPALETTE = 9,
	TGAIMAGETYPE_RLERGB = 10,
	TGAIMAGETYPE_RLEMONOCHROME = 11
};

//----------------------------------------------------------------------------------------
//JPG structures
//----------------------------------------------------------------------------------------
struct Image::JPGStreamSourceManager
{
public:
	//Constructors
	JPGStreamSourceManager(j_decompress_ptr cinfo, Stream::IStream& astream, unsigned int abufferSize)
	:stream(astream), buffer(abufferSize)
	{
		//Register libjpeg callback handlers
		pub.init_source = init_source;
		pub.fill_input_buffer = fill_input_buffer;
		pub.skip_input_data = skip_input_data;
		pub.resync_to_restart = jpeg_resync_to_restart;
		pub.term_source = term_source;

		cinfo->src = (struct jpeg_source_mgr*)this;
	}

private:
	//Libjpeg callback handlers
	static void init_source(j_decompress_ptr cinfo)
	{
		JPGStreamSourceManager* src = (JPGStreamSourceManager*)cinfo->src;
		src->pub.bytes_in_buffer = 0;
		src->pub.next_input_byte = 0;
	}

	static boolean fill_input_buffer(j_decompress_ptr cinfo)
	{
		JPGStreamSourceManager* src = (JPGStreamSourceManager*)cinfo->src;
		//Note that we have to ignore the current state of bytes_in_buffer and
		//next_input_byte here, and simply refill the buffer, as specified in
		//libjpeg.txt. These values are not always valid when this function is
		//called. This function needs to assume the buffer is empty.
		if(!src->stream.ReadData(src->buffer, src->buffer.size()))
		{
			return FALSE;
		}
		src->pub.next_input_byte = &src->buffer[0];
		src->pub.bytes_in_buffer = src->buffer.size();
		return TRUE;
	}

	static void skip_input_data(j_decompress_ptr cinfo, long num_bytes)
	{
		JPGStreamSourceManager* src = (JPGStreamSourceManager*)cinfo->src;
		//As specified in libjpeg.txt, it seems a negative count may be passed to this
		//function on occasion. In this case, the operation should be ignored.
		if(num_bytes <= 0)
		{
			return;
		}
		if(num_bytes <= (long)src->pub.bytes_in_buffer)
		{
			src->pub.next_input_byte += num_bytes;
			src->pub.bytes_in_buffer -= num_bytes;
		}
		else
		{
			num_bytes -= (long)src->pub.bytes_in_buffer;
			for(unsigned int i = 0; i < (unsigned int)num_bytes; ++i)
			{
				unsigned char temp;
				src->stream.ReadData(temp);
			}
			src->pub.next_input_byte = &src->buffer[0];
			src->pub.bytes_in_buffer = 0;
		}
	}

	static void term_source(j_decompress_ptr cinfo)
	{}

private:
	jpeg_source_mgr pub;
	Stream::IStream& stream;
	std::vector<unsigned char> buffer;
};

//----------------------------------------------------------------------------------------
struct Image::JPGStreamDestinationManager
{
public:
	//Constructors
	JPGStreamDestinationManager(j_compress_ptr cinfo, Stream::IStream& astream, unsigned int abufferSize)
	:stream(astream), buffer(abufferSize)
	{
		//Register libjpeg callback handlers
		pub.init_destination = init_destination;
		pub.empty_output_buffer = empty_output_buffer;
		pub.term_destination = term_destination;

		cinfo->dest = (struct jpeg_destination_mgr*)this;
	}

private:
	//Libjpeg callback handlers
	static void init_destination(j_compress_ptr cinfo)
	{
		JPGStreamDestinationManager* dest = (JPGStreamDestinationManager*)cinfo->dest;
		dest->pub.next_output_byte = &dest->buffer[0];
		dest->pub.free_in_buffer = dest->buffer.size();
	}

	static boolean empty_output_buffer(j_compress_ptr cinfo)
	{
		JPGStreamDestinationManager* dest = (JPGStreamDestinationManager*)cinfo->dest;
		//Note that as in fill_input_buffer, we have to ignore the current state of
		//free_in_buffer and next_output_byte here, and simply write out the entire
		//buffer, as specified in libjpeg.txt. This function needs to assume the buffer
		//is full.
		if(!dest->stream.WriteData(&dest->buffer[0], dest->buffer.size()))
		{
			return FALSE;
		}
		dest->pub.next_output_byte = &dest->buffer[0];
		dest->pub.free_in_buffer = dest->buffer.size();
		return TRUE;
	}

	static void term_destination(j_compress_ptr cinfo)
	{
		JPGStreamDestinationManager* dest = (JPGStreamDestinationManager*)cinfo->dest;
		//We dump out the remaining data in the buffer here. We can't call
		//empty_output_buffer to do the job, since that function has to assume
		//that the buffer is completely full, which it probably won't be here.
		unsigned int bytesToWrite = (unsigned int)(dest->buffer.size() - dest->pub.free_in_buffer);
		if(bytesToWrite > 0)
		{
			dest->stream.WriteData(&dest->buffer[0], bytesToWrite);
			dest->pub.next_output_byte = &dest->buffer[0];
			dest->pub.free_in_buffer = dest->buffer.size();
		}
	}

private:
	jpeg_destination_mgr pub;
	Stream::IStream& stream;
	std::vector<unsigned char> buffer;
};

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Image::Image(unsigned int aimageWidth, unsigned int aimageHeight, PixelFormat apixelFormat, DataFormat adataFormat)
{
	SetImageFormat(aimageWidth, aimageHeight, apixelFormat, adataFormat);
}

//----------------------------------------------------------------------------------------
//Pixel data manipulation
//----------------------------------------------------------------------------------------
void Image::GetRawPixelDataInternal(unsigned int posX, unsigned int posY, unsigned int planeNo, PixelData& data) const
{
	unsigned int dataPos = ((posX + (posY * imageWidth)) * dataPlaneCount) + planeNo;
	DebugAssert(dataPos < imageData.size());
	data = imageData[dataPos];
}

//----------------------------------------------------------------------------------------
void Image::SetRawPixelDataInternal(unsigned int posX, unsigned int posY, unsigned int planeNo, PixelData data)
{
	unsigned int dataPos = ((posX + (posY * imageWidth)) * dataPlaneCount) + planeNo;
	DebugAssert(dataPos < imageData.size());
	imageData[dataPos] = data;
}

//----------------------------------------------------------------------------------------
void Image::ReadPixelDataInternal(unsigned int posX, unsigned int posY, unsigned int planeNo, float& data) const
{
	PixelData pixelData;
	GetRawPixelDataInternal(posX, posY, planeNo, pixelData);
	switch(dataFormat)
	{
	case DATAFORMAT_FLOAT:
		data = pixelData.dataFloat;
		break;
	case DATAFORMAT_8BIT:
		data = ((float)pixelData.data8Bit / 255.0f);
		break;
	default:
		DebugAssert(false);
		return;
	}
}

//----------------------------------------------------------------------------------------
void Image::ReadPixelDataInternal(unsigned int posX, unsigned int posY, unsigned int planeNo, unsigned char& data) const
{
	PixelData pixelData;
	GetRawPixelDataInternal(posX, posY, planeNo, pixelData);
	switch(dataFormat)
	{
	case DATAFORMAT_FLOAT:
		data = (unsigned char)((pixelData.dataFloat * 255.0f) + 0.5f);
		break;
	case DATAFORMAT_8BIT:
		data = pixelData.data8Bit;
		break;
	default:
		DebugAssert(false);
		return;
	}
}

//----------------------------------------------------------------------------------------
void Image::ReadPixelDataInternal(unsigned int posX, unsigned int posY, unsigned int planeNo, unsigned int& data, unsigned int bitCount) const
{
	unsigned int maxValue = (((1 << (bitCount - 1)) - 1) << 1) | 0x01;
	PixelData pixelData;
	GetRawPixelDataInternal(posX, posY, planeNo, pixelData);
	switch(dataFormat)
	{
	case DATAFORMAT_FLOAT:
		data = (unsigned int)(((double)pixelData.dataFloat * ((double)maxValue / 1.0)) + 0.5f);
		break;
	case DATAFORMAT_8BIT:
		data = (unsigned int)(((double)pixelData.data8Bit * ((double)maxValue / 255.0)) + 0.5f);
		break;
	default:
		DebugAssert(false);
		return;
	}
}

//----------------------------------------------------------------------------------------
void Image::WritePixelDataInternal(unsigned int posX, unsigned int posY, unsigned int planeNo, float data)
{
	PixelData pixelData;
	switch(dataFormat)
	{
	case DATAFORMAT_FLOAT:
		pixelData.dataFloat = data;
		break;
	case DATAFORMAT_8BIT:
		pixelData.data8Bit = (unsigned char)((data * 255.0f) + 0.5f);
		break;
	default:
		DebugAssert(false);
		return;
	}
	SetRawPixelDataInternal(posX, posY, planeNo, pixelData);
}

//----------------------------------------------------------------------------------------
void Image::WritePixelDataInternal(unsigned int posX, unsigned int posY, unsigned int planeNo, unsigned char data)
{
	PixelData pixelData;
	switch(dataFormat)
	{
	case DATAFORMAT_FLOAT:
		pixelData.dataFloat = ((float)data / 255.0f);
		break;
	case DATAFORMAT_8BIT:
		pixelData.data8Bit = data;
		break;
	default:
		DebugAssert(false);
		return;
	}
	SetRawPixelDataInternal(posX, posY, planeNo, pixelData);
}

//----------------------------------------------------------------------------------------
void Image::WritePixelDataInternal(unsigned int posX, unsigned int posY, unsigned int planeNo, unsigned int data, unsigned int bitCount)
{
	unsigned int maxValue = (((1 << (bitCount - 1)) - 1) << 1) | 0x01;
	PixelData pixelData;
	switch(dataFormat)
	{
	case DATAFORMAT_FLOAT:
		pixelData.dataFloat = (float)((double)data * (1.0 / (double)maxValue));
		break;
	case DATAFORMAT_8BIT:
		pixelData.data8Bit = (unsigned char)(((double)data * (255.0 / (double)maxValue)) + 0.5f);
		break;
	default:
		DebugAssert(false);
		return;
	}
	SetRawPixelDataInternal(posX, posY, planeNo, pixelData);
}
