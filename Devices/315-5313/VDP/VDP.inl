//##DEBUG##
#include <iostream>
#include <iomanip>

//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct VDP::Color
{
	unsigned int r;
	unsigned int g;
	unsigned int b;
	bool shadow;
	bool highlight;

	//This is a general conversion function. Takes a number within a given range, and
	//scales it to the corresponding position in a target range. We use it here to do
	//accurate color conversion from Mega Drive 9-bit color codes into any required
	//format.
	template<class S, class T> inline T Convert(S number, S minSource, S maxSource, T minTarget, T maxTarget) const
	{
		return (T)((float)(number - minSource) * ((float)(maxTarget - minTarget) / (float)(maxSource - minSource))) + minTarget;
	}

	inline unsigned char GetR8Bit() const
	{
		unsigned char maxTarget = (!shadow || (highlight && shadow))? 255: 128;
		unsigned char minTarget = (!highlight || (highlight && shadow))? 0: 128;
		return Convert(r, 0u, 7u, minTarget, maxTarget);
	}

	inline unsigned char GetG8Bit() const
	{
		unsigned char maxTarget = (!shadow || (highlight && shadow))? 255: 128;
		unsigned char minTarget = (!highlight || (highlight && shadow))? 0: 128;
		return Convert(g, 0u, 7u, minTarget, maxTarget);
	}

	inline unsigned char GetB8Bit() const
	{
		unsigned char maxTarget = (!shadow || (highlight && shadow))? 255: 128;
		unsigned char minTarget = (!highlight || (highlight && shadow))? 0: 128;
		return Convert(b, 0u, 7u, minTarget, maxTarget);
	}

	inline float GetRFloat() const
	{
		float maxTarget = (!shadow || (highlight && shadow))? 1.0f: 0.5f;
		float minTarget = (!highlight || (highlight && shadow))? 0.0f: 0.5f;
		return Convert(r, 0u, 7u, minTarget, maxTarget);
	}

	inline float GetGFloat() const
	{
		float maxTarget = (!shadow || (highlight && shadow))? 1.0f: 0.5f;
		float minTarget = (!highlight || (highlight && shadow))? 0.0f: 0.5f;
		return Convert(g, 0u, 7u, minTarget, maxTarget);
	}

	inline float GetBFloat() const
	{
		float maxTarget = (!shadow || (highlight && shadow))? 1.0f: 0.5f;
		float minTarget = (!highlight || (highlight && shadow))? 0.0f: 0.5f;
		return Convert(b, 0u, 7u, minTarget, maxTarget);
	}
};

//----------------------------------------------------------------------------------------
struct VDP::BlockMapping
{
	unsigned int blockNumber;
	unsigned int paletteLine;
	bool priority;
	bool vflip;
	bool hflip;
};

//----------------------------------------------------------------------------------------
struct VDP::SpriteMapping
{
	unsigned int blockNumber;
	unsigned int paletteLine;
	unsigned int xpos;
	unsigned int ypos;
	unsigned int width;
	unsigned int height;
	unsigned int link;
	bool priority;
	bool vflip;
	bool hflip;
};

//----------------------------------------------------------------------------------------
struct VDP::FIFOEntry
{
	enum WriteTarget
	{
		FIFO_WRITEDATAPORT,
		FIFO_WRITECONTROLPORT
	};

	bool exists;
	WriteTarget	writeTarget;
	unsigned int data;
	double writeTime;
	IDeviceContext* caller;
};

//----------------------------------------------------------------------------------------
struct VDP::XYPair
{
	unsigned int x;
	unsigned int y;
};

//----------------------------------------------------------------------------------------
enum VDP::MemoryTarget
{
	VRAM,
	CRAM,
	VSRAM
};

//----------------------------------------------------------------------------------------
enum VDP::ScrollModeH
{
	SCROLLH_OVERALL		= 0,
	SCROLLH_CELL		= 2,
	SCROLLH_LINE		= 3,
	SCROLLH_PROHIBITED	= 1
};

//----------------------------------------------------------------------------------------
enum VDP::ScrollModeV
{
	SCROLLV_OVERALL	= 0,
	SCROLLV_2CELL	= 1
};

//----------------------------------------------------------------------------------------
enum VDP::InterlaceMode
{
	INTERLACE_NONE			= 0,
	INTERLACE_ON			= 1,
	INTERLACE_DOUBLE		= 3,
	INTERLACE_PROHIBITED	= 2
};

//----------------------------------------------------------------------------------------
enum VDP::DMAMode
{
	DMA_MEMORY_TO_VRAM,
	DMA_VRAM_FILL,
	DMA_VRAM_COPY
};

//----------------------------------------------------------------------------------------
enum VDP::Code
{
	VRAM_READ		= 0x00, //0000
	CRAM_READ		= 0x08, //1000
	VSRAM_READ		= 0x04, //0100
	VRAM_WRITE		= 0x01, //0001
	CRAM_WRITE		= 0x03, //0011
	VSRAM_WRITE		= 0x05, //0101
};

//----------------------------------------------------------------------------------------
enum VDP::State
{
	STATE_IDLE,
	STATE_RENDER,
	STATE_LINE0,
	STATE_VINT,
	STATE_HINT,
	STATE_EXINT
};

/**********************************Register Functions***********************************\
| Register functions are safe for use by external devices. Reads and writes will be
| synchronized across all devices, and rollbacks will be generated where required.
\***************************************************************************************/
//----------------------------------------------------------------------------------------
//Raw register functions
//----------------------------------------------------------------------------------------
unsigned char VDP::ReadRegister(unsigned int regNumber, double accessTime) const
{
	return reg.Read(regNumber, accessTime);
}

//----------------------------------------------------------------------------------------
void VDP::WriteRegister(unsigned int regNumber, double accessTime, unsigned char data)
{
	reg.Write(regNumber, accessTime, data);
}

//----------------------------------------------------------------------------------------
unsigned short VDP::GetStatusRegister(const RasterPosNew& rasterPos) const
{
	//##TODO## Implement write FIFO empty/full flags. Note that it may be possible for
	//the VDP to clear some entries from the FIFO during the active scan period in
	//between VDP access to the VRAM. Run some tests to see if entries are cleared during
	//active scan or not. We also need to confirm whether unlimited access to VRAM is
	//possible during hblank, or just during vblank. Also note that there is unlimited
	//VRAM access when the display is disabled.
	//##NOTE## I suspect the VDP renders in 2-cell segments, based on several independent
	//bits of information, such as the nature of the window distortion bug, and the
	//resolution of the vertical displacement information. In the case of the FIFO, it
	//also appears that the VDP allows one memory write access to go through from the
	//FIFO to VRAM every 2 cells. I doubt this is a coincidence. This means the VDP
	//should generate a timing point after the rendering of every 16 pixels on each
	//scanline. During this period, we have the chance to release data from the FIFO and
	//update the corresponding FIFO status flags in lock-step.
	//##NOTE## For performance reasons, we should NOT render in 2-cell segments on every
	//line. Render entire lines as is done currently, and if a rollback is triggered due
	//to a FIFO entry, switch to an accurate 2-cell render mode for the repeated pass.
	//This will not introduce inaccuracies into the rollback state as the rendering
	//process itself has no external impact.
	////##TODO## Confirm that this flag is set at the end of each frame when the vertical
	////interrupt is generated, and is not cleared unless we reach a vblank where no
	////vertical interrupt is generated. It seems logical that this is the case, however
	////there is no documentation confirming this behaviour.
	//data |= (GetVInterruptOccurred())? 0x0080: 0;
	////##TODO## Implement sprite overflow and collision flags. Determine whether these
	////flags are cleared at the end of each line, or the end of each frame. Also determine
	////whether these flags are set at any point during the rendering of a line, or whether
	////they are set at a fixed point, such as the beginning or end of hblank. I suspect
	////they may be set when the VDP state data is supposedly copied at the beginning of
	////a line. If they are set during a line, we must devise a method whereby we can
	////approximate the time at which they are set. Note that we also need to determine
	////whether these flags are set for sprites outside the visible playfield. We also need
	////to check whether sprite masking modes cause the overflow flag to be set. It may
	////simply be that masking modes cause the VDP to loop on a single sprite, like a
	////self-referencing link field, and the rendering loop terminates when the maximum
	////number of sprites is reached.

	unsigned short data = (unsigned short)sr;
	//##TODO## Accurately implement these flags. We need to determine precisely which
	//point to enable these flags, and when to disable them. Run a series of tests on the
	//hardware to determine the correct timing.
	data |= ((rasterPos.rasterNo < (3 * 8)) || (rasterPos.rasterNo >= ((3 * 8) + 224)))? 0x0008: 0;
	data |= (rasterPos.rasterProgress > 0.8)? 0x0004: 0;

//	data |= (rasterPos.vblank)? 0x0008: 0;
//	data |= (rasterPos.hblank | rasterPos.vblank)? 0x0004: 0;
	//##DEBUG##
	//##FIX## If we re-enable the flag use above, water is broken in S2. Checking status
	//flag for the end of vblank, which in our system doesn't end until active scan. It
	//appears that this is not the case on the actual system. Determine precisely when
	//the hblank and vblank flags should be set in the status register, and when they
	//should be cleared.
	//##NOTE## Further to the above, based on code in the raster effects test rom, and
	//various bits of information from other sources, it is possible that the hblank and
	//vblank flags are cleared when the port is read, as per the sprite flags. This seems
	//a little unusual however. Confirm the behaviour of these flags, either by finding a
	//solid reference, or preferably, running test code on the real hardware. We need to
	//fix up these flags, as currently the rand method we're using below breaks various
	//effects in lots or roms, eg, raster effects test, vectorman title, alien soldier
	//weapon selection background.
//	data |= ((rand() & 0x01) != 0)? 0x0008: 0;
//	data |= ((rand() & 0x01) != 0)? 0x0004: 0;
	//##TODO## Write these flags during rendering
//	data |= (spriteOverflow[rasterPos.line])? 0x0040: 0;
//	data |= (spriteCollision[rasterPos.line])? 0x0020: 0;

	//##TODO## Confirm these and only these unused bits are set on the real system.
	//##NOTE## According to notes in the gens source, these upper bits are not all
	//guaranteed to be fixed at their reported values on all models. Some of them are
	//apparently open, hence the random element applied to these bits in gens. As per
	//the above, perform some tests on various models of the hardware to map out the
	//behaviour of these upper bits. Note that according to gens, at least one game
	//requires values to be returned from at least one of these bits which does not
	//match the reported fixed values.
	//##NOTE## See these notes from SpritesMind about the upper bits of SR:
	//Game no Kanzume o Tokuyou won't run. It wants to write to ROM and also seems to use
	//unused VDP control bits. I have not found *right*(read without hack) way to run
	//this game.
	//this one is indeed weird: Haze discovered that it wants ROM mapped as RAM to copy
	//games programs in there, I thought it could be possible to have cart hardware that
	//would enable ROM area writes (this seems to be a seganet compilation, I wonder if a
	//real cartridge exist for this one)
	//If I remember well, the game also rely on the unused bits when reading the VDP
	//status register to be correctly set (i.e top bits of the next 68k instruction) or
	//it would hang at startup 

	//Toggle the DMA busy flag randomly. We put this workaround in place to get Mega
	//Turrican to boot.
	data |= ((rand() % 2) != 0)? 0x0002: 0;

	//Fix PAL mode on. Needed to fix sound timing issue in Quackshot.
	data |= 0x0001;

	//Fix FIFO empty flag to true
	data |= 0x0200;

	return (data & 0x03FF) | 0x3400;
}

//----------------------------------------------------------------------------------------
//DMA registers
//----------------------------------------------------------------------------------------
//##TODO## Add notes to all these functions showing the bitfield mappings
unsigned int VDP::GetDMASourceAddress(double accessTime) const
{
	unsigned int dmaSourceAddress;
	dmaSourceAddress = ReadRegister(21, accessTime);
	dmaSourceAddress |= ((unsigned int)ReadRegister(22, accessTime) << 8);
	dmaSourceAddress |= ((unsigned int)(ReadRegister(23, accessTime) & 0x7F) << 16);
	dmaSourceAddress <<= 1;
	return dmaSourceAddress;
}

//----------------------------------------------------------------------------------------
void VDP::SetDMASourceAddress(double accessTime, unsigned int value)
{
	value >>= 1;
	WriteRegister(21, accessTime, (unsigned char)(value & 0xFF));
	WriteRegister(22, accessTime, (unsigned char)((value >> 8) & 0xFF));
	WriteRegister(23, accessTime, (unsigned char)((value >> 16) & 0x7F));
}

//----------------------------------------------------------------------------------------
unsigned int VDP::GetDMATransferCount(double accessTime) const
{
	unsigned int transferCount;
	transferCount = ReadRegister(19, accessTime);
	transferCount |= ((unsigned int)ReadRegister(20, accessTime) << 8);
	if(transferCount == 0)
	{
		transferCount = 0xFFFF;
	}
	return transferCount;
}

//----------------------------------------------------------------------------------------
void VDP::SetDMATransferCount(double accessTime, unsigned int value)
{
	WriteRegister(19, accessTime, (unsigned char)(value & 0xFF));
	WriteRegister(20, accessTime, (unsigned char)((value >> 8) & 0xFF));
}

//----------------------------------------------------------------------------------------
unsigned short VDP::GetAddressIncrement(double accessTime) const
{
	return (unsigned short)ReadRegister(15, accessTime);
}

//----------------------------------------------------------------------------------------
void VDP::SetAddressIncrement(double accessTime, unsigned int value)
{
	WriteRegister(15, accessTime, (unsigned char)(value));
}

//----------------------------------------------------------------------------------------
bool VDP::DMAEnabled(double accessTime) const
{
	return ((ReadRegister(1, accessTime) & 0x10) != 0);
}

//----------------------------------------------------------------------------------------
void VDP::SetDMAEnabled(double accessTime, bool value)
{
	WriteRegister(1, accessTime, (ReadRegister(1, accessTime) & ~0x10) | ((value == true)? 0x10: 0x00));
}

//----------------------------------------------------------------------------------------
VDP::DMAMode VDP::GetDMAMode(double accessTime) const
{
	DMAMode mode;

	switch((ReadRegister(23, accessTime) & 0xC0) >> 6)
	{
	case 0:
	case 1:
		mode = DMA_MEMORY_TO_VRAM;
		break;
	case 2:
		mode = DMA_VRAM_FILL;
		break;
	case 3:
		mode = DMA_VRAM_COPY;
		break;
	}

	return mode;
}

//----------------------------------------------------------------------------------------
//HV counter functions
//----------------------------------------------------------------------------------------
//##TODO## Add support for freezing the HV counter, in preparation for HL support
//##NOTE## Based on a blocking loop at 0x272 in the window distortion test ROM, it
//seems that during vblank, the vcounter may read 0xFF. We're guessing the same is
//true for the hcounter. We've designed this function to simulate this behaviour.
//##TODO## Run some tests on the real system to confirm this behaviour.
//##TODO## Build a proper test ROM to demonstrate this behaviour, and test the
//behaviour of other emulators.
//unsigned short VDP::GetHVCounter(RasterPos rasterPos) const
//{
//	unsigned short counter;
//	counter = GetHCounter(rasterPos);
//	counter |= (unsigned short)GetVCounter(rasterPos) << 8;
//	return counter;
//}
//
////----------------------------------------------------------------------------------------
//unsigned char VDP::GetHCounter(RasterPos rasterPos) const
//{
//	if(!rasterPos.hblank)
//	{
//		return (rasterPos.pixel) >> 1;
//	}
//	else
//	{
//		return 0xFF;
//	}
//}
//
////----------------------------------------------------------------------------------------
//unsigned char VDP::GetVCounter(const RasterPos& rasterPos) const
//{
//	//##TODO## Fix up support for the vertical counter in interlace mode. The treatment
//	//below is not believed to be complete or accurate. Refer to the Genesis Programming
//	//Manual, page 25.
//	if(!rasterPos.vblank)
//	{
//		unsigned int counter = rasterPos.line;
//		if(GetInterlaceMode() == INTERLACE_DOUBLE)
//		{
//			counter &= ~0x0001;
//			counter |= ((counter & 0x0100) >> 8);
//		}
//		return rasterPos.line;
//	}
//	else
//	{
//		return 0xFF;
//	}
//}

//----------------------------------------------------------------------------------------
unsigned short VDP::GetHVCounter(const RasterPosNew& rasterPos, double accessTime) const
{
	unsigned short counter;
	counter = GetHCounter(rasterPos, accessTime);
	counter |= (unsigned short)GetVCounter(rasterPos, accessTime) << 8;
	return counter;
}

//----------------------------------------------------------------------------------------
unsigned char VDP::GetHCounter(const RasterPosNew& rasterPos, double accessTime) const
{
	unsigned char hcounter;
	hcounter = (unsigned char)((double)0xD2 * rasterPos.rasterProgress);
	if(hcounter > 0xB6)
	{
		hcounter += 0x2E;
	}
	return hcounter;
}

//----------------------------------------------------------------------------------------
unsigned char VDP::GetVCounter(const RasterPosNew& rasterPos, double accessTime) const
{
	//Assumed constants
	//const unsigned int blockHeight = 8;

	///////////////////////////////////////////////////////
	//      lines   lines   lines   vcells  vsync   hsync
	//      screen  display retrace
	//NTSC	262     224     38      28      60Hz    15720Hz
	//PAL	312     224     88      28      50Hz    15600Hz
	//PAL	312     240     72      30      50Hz    15600Hz
	///////////////////////////////////////////////////////
	//##TODO## Calculate these values based on the current display settings
	//##FIX## We can't calculate the current HV counter based on the current display
	//settings, as we need to know what the settings were earlier on in the frame to
	//know how the counter advanced up until this point.
	//unsigned int visibleCellHeight = GetVisibleCellHeight(accessTime);
	//unsigned int rasterCountScreen = 312;
	//unsigned int refreshRate = 50;
	//unsigned int rasterCountTopBorder = ((34 - visibleCellHeight) / 2) * blockHeight;
	//unsigned int rasterCountFrame = visibleCellHeight * blockHeight;
	//unsigned int rasterCountBottomBorder = ((34 - visibleCellHeight) / 2) * blockHeight;
	//unsigned int rasterCountRetrace = rasterCountScreen - (rasterCountTopBorder + rasterCountFrame + rasterCountBottomBorder);
	//unsigned int rasterPosFrame = rasterCountTopBorder;
	//unsigned int rasterPosBottomBorder = rasterCountTopBorder + rasterCountFrame;
	//unsigned int rasterPosRetrace = rasterCountTopBorder + rasterCountFrame + rasterCountBottomBorder;

	//##TODO## Fix up support for the vertical counter in interlace mode. The treatment
	//below is not believed to be complete or accurate. Refer to the Genesis Programming
	//Manual, page 25.
	//##TODO## Implement proper support for hcounter and vcounter in both PAL and NTSC
	//modes.
	unsigned char vcounter;
	if(rasterPos.rasterNo >= 0x100)
	{
		vcounter = (unsigned char)(0xCA + (rasterPos.rasterNo - 0x100));
	}
	else
	{
		vcounter = (unsigned char)rasterPos.rasterNo;
	}
	return vcounter;
}

//----------------------------------------------------------------------------------------
//Interrupt functions
//----------------------------------------------------------------------------------------
bool VDP::GetHInterruptEnabled(double accessTime) const
{
	return (reg.Read(0, accessTime) & 0x10) != 0;
}

//----------------------------------------------------------------------------------------
bool VDP::GetVInterruptEnabled(double accessTime) const
{
	return (reg.Read(1, accessTime) & 0x20) != 0;
}

//----------------------------------------------------------------------------------------
unsigned int VDP::GetHInterruptCounter(double accessTime) const
{
	return reg.Read(10, accessTime);
}

//----------------------------------------------------------------------------------------
//Control port registers
//----------------------------------------------------------------------------------------
bool VDP::GetDMAPending() const
{
	return dmaPending;
}

//----------------------------------------------------------------------------------------
void VDP::SetDMAPending(bool value)
{
	dmaPending = value;
}

//----------------------------------------------------------------------------------------
bool VDP::GetWritePending() const
{
	return writePending;
}

//----------------------------------------------------------------------------------------
void VDP::SetWritePending(bool value)
{
	writePending = value;
}

//----------------------------------------------------------------------------------------
unsigned int VDP::GetControlAddress() const
{
	return address;
}

//----------------------------------------------------------------------------------------
void VDP::SetControlAddress(unsigned int value)
{
	address = (unsigned short)value;
}

//----------------------------------------------------------------------------------------
unsigned int VDP::GetControlCode() const
{
	return (unsigned int)code;
}

//----------------------------------------------------------------------------------------
void VDP::SetControlCode(unsigned int value)
{
	code = (Code)value;
}

//----------------------------------------------------------------------------------------
//Status register
//----------------------------------------------------------------------------------------
bool VDP::GetWriteFIFOEmpty() const
{
	return (sr & 0x0200) != 0;
}

//----------------------------------------------------------------------------------------
void VDP::SetWriteFIFOEmpty(bool value)
{
	sr = (sr & ~0x0200) | ((value == true)? 0x0200: 0x0000);
}

//----------------------------------------------------------------------------------------
bool VDP::GetWriteFIFOFull() const
{
	return (sr & 0x0100) != 0;
}

//----------------------------------------------------------------------------------------
void VDP::SetWriteFIFOFull(bool value)
{
	sr = (sr & ~0x0100) | ((value == true)? 0x0100: 0x0000);
}

//----------------------------------------------------------------------------------------
bool VDP::GetVInterruptHappened() const
{
	return (sr & 0x0080) != 0;
}

//----------------------------------------------------------------------------------------
void VDP::SetVInterruptHappened(bool value)
{
	sr = (sr & ~0x0080) | ((value == true)? 0x0080: 0x0000);
}

////----------------------------------------------------------------------------------------
//bool VDP::GetSpriteOverflow() const
//{
//	return (sr & 0x0040) != 0;
//}
//
////----------------------------------------------------------------------------------------
//void VDP::SetSpriteOverflow(bool value)
//{
//	sr = (sr & ~0x0040) | ((value == true)? 0x0040: 0x0000);
//}
//
////----------------------------------------------------------------------------------------
//bool VDP::GetSpriteCollision() const
//{
//	return (sr & 0x0020) != 0;
//}
//
////----------------------------------------------------------------------------------------
//void VDP::SetSpriteCollision(bool value)
//{
//	sr = (sr & ~0x0020) | ((value == true)? 0x0020: 0x0000);
//}

//----------------------------------------------------------------------------------------
bool VDP::GetOddInterlaceFrame() const
{
	return (sr & 0x0010) != 0;
}

//----------------------------------------------------------------------------------------
void VDP::SetOddInterlaceFrame(bool value)
{
	sr = (sr & ~0x0010) | ((value == true)? 0x0010: 0x0000);
}

//----------------------------------------------------------------------------------------
bool VDP::GetDuringVBlank() const
{
	return (sr & 0x0008) != 0;
}

//----------------------------------------------------------------------------------------
void VDP::SetDuringVBlank(bool value)
{
	sr = (sr & ~0x0008) | ((value == true)? 0x0008: 0x0000);
}

//----------------------------------------------------------------------------------------
bool VDP::GetDuringHBlank() const
{
	return (sr & 0x0004) != 0;
}

//----------------------------------------------------------------------------------------
void VDP::SetDuringHBlank(bool value)
{
	sr = (sr & ~0x0004) | ((value == true)? 0x0004: 0x0000);
}

//----------------------------------------------------------------------------------------
bool VDP::GetDMABusy() const
{
	return (sr & 0x0002) != 0;
}

//----------------------------------------------------------------------------------------
void VDP::SetDMABusy(bool value)
{
	sr = (sr & ~0x0002) | ((value == true)? 0x0002: 0x0000);
}

//----------------------------------------------------------------------------------------
bool VDP::GetPALMode() const
{
	return (sr & 0x0001) != 0;
}

//----------------------------------------------------------------------------------------
void VDP::SetPALMode(bool value)
{
	sr = (sr & ~0x0001) | ((value == true)? 0x0001: 0x0000);
}

/*********************************Synchronized Functions********************************\
| Synchronized register functions operate directly on the committed register/memory
| state, and should only be called internally by the VDP core when the buffers are
| synchronized. Write functions should only be used for debugging purposes, as writing
| directly to the committed state does not generate rollbacks in cases of overlapped
| access.
\***************************************************************************************/
//----------------------------------------------------------------------------------------
//Raw register functions
//----------------------------------------------------------------------------------------
unsigned char VDP::ReadRegister(unsigned int regNumber) const
{
	return reg.ReadCommitted(regNumber);
}

//----------------------------------------------------------------------------------------
void VDP::WriteRegister(unsigned int regNumber, unsigned char data)
{
	reg.WriteLatest(regNumber, data);
}

//----------------------------------------------------------------------------------------
//DMA registers
//----------------------------------------------------------------------------------------
unsigned int VDP::GetDMASourceAddress() const
{
	unsigned int dmaSourceAddress;
	dmaSourceAddress = ReadRegister(21);
	dmaSourceAddress |= ((unsigned int)ReadRegister(22) << 8);
	dmaSourceAddress |= ((unsigned int)(ReadRegister(23) & 0x7F) << 16);
	dmaSourceAddress <<= 1;
	return dmaSourceAddress;
}

//----------------------------------------------------------------------------------------
void VDP::SetDMASourceAddress(unsigned int value)
{
	value >>= 1;
	WriteRegister(21, (unsigned char)(value & 0xFF));
	WriteRegister(22, (unsigned char)((value >> 8) & 0xFF));
	WriteRegister(23, (unsigned char)((value >> 16) & 0x7F));
}

//----------------------------------------------------------------------------------------
unsigned int VDP::GetDMATransferCount() const
{
	unsigned int transferCount;
	transferCount = ReadRegister(19);
	transferCount |= ((unsigned int)ReadRegister(20) << 8);
	if(transferCount == 0)
	{
		transferCount = 0xFFFF;
	}
	return transferCount;
}

//----------------------------------------------------------------------------------------
void VDP::SetDMATransferCount(unsigned int value)
{
	WriteRegister(19, (unsigned char)(value & 0xFF));
	WriteRegister(20, (unsigned char)((value >> 8) & 0xFF));
}

//----------------------------------------------------------------------------------------
unsigned short VDP::GetAddressIncrement() const
{
	return (unsigned short)ReadRegister(15);
}

//----------------------------------------------------------------------------------------
void VDP::SetAddressIncrement(unsigned int value)
{
	WriteRegister(15, (unsigned char)value);
}

//----------------------------------------------------------------------------------------
bool VDP::DMAEnabled() const
{
	return ((ReadRegister(1) & 0x10) != 0);
}

//----------------------------------------------------------------------------------------
void VDP::SetDMAEnabled(bool value)
{
	WriteRegister(1, (ReadRegister(1) & ~0x10) | ((value == true)? 0x10: 0x00));
}

//----------------------------------------------------------------------------------------
VDP::DMAMode VDP::GetDMAMode() const
{
	DMAMode mode;

	switch((ReadRegister(23) & 0xC0) >> 6)
	{
	case 0:
	case 1:
		mode = DMA_MEMORY_TO_VRAM;
		break;
	case 2:
		mode = DMA_VRAM_FILL;
		break;
	case 3:
		mode = DMA_VRAM_COPY;
		break;
	}

	return mode;
}

//----------------------------------------------------------------------------------------
//Data table addresses
//----------------------------------------------------------------------------------------
unsigned int VDP::GetMappingAddressA() const
{
	return (unsigned int)(reg.ReadCommitted(2) & 0x38) << 10;
}

//----------------------------------------------------------------------------------------
void VDP::SetMappingAddressA(unsigned int value)
{
	reg.WriteCommitted(2, (unsigned char)((reg.ReadCommitted(2) & ~0x38) | ((value >> 10) & 0x38)));
}

//----------------------------------------------------------------------------------------
unsigned int VDP::GetMappingAddressB() const
{
	return (unsigned int)(reg.ReadCommitted(4) & 0x07) << 13;
}

//----------------------------------------------------------------------------------------
void VDP::SetMappingAddressB(unsigned int value)
{
	reg.WriteCommitted(4, (unsigned char)((reg.ReadCommitted(4) & ~0x07) | ((value >> 13) & 0x07)));
}

//----------------------------------------------------------------------------------------
unsigned int VDP::GetMappingAddressWindow() const
{
	unsigned int address;

	//##TODO## Check genvdp regarding address bits being forced to 0 in 40-cell mode, and
	//make sure we're covering them all.
	//##TODO## Clean up this operation.
	address = (unsigned int)(reg.ReadCommitted(3) & 0x3E) << 10;
	if(GetScrollSizeH() == 40)
	{
		address &= 0xF000;
	}

	return address;
}

//----------------------------------------------------------------------------------------
void VDP::SetMappingAddressWindow(unsigned int value)
{
	if(GetScrollSizeH() == 40)
	{
		value &= 0xF000;
	}
	else
	{
		value &= 0xF800;
	}
	reg.WriteCommitted(3, (unsigned char)((reg.ReadCommitted(3) & ~0x3E) | (value >> 10)));
}

//----------------------------------------------------------------------------------------
unsigned int VDP::GetSpriteTableAddress() const
{
	unsigned int address;

	address = (unsigned int)(reg.ReadCommitted(5) & 0x7F) << 9;
	if(GetScrollSizeH() == 40)
	{
		address &= 0xFC00;
	}

	return address;
}

//----------------------------------------------------------------------------------------
void VDP::SetSpriteTableAddress(unsigned int value)
{
	if(GetScrollSizeH() == 40)
	{
		value &= 0xFC00;
	}
	else
	{
		value &= 0xFE00;
	}
	reg.WriteCommitted(5, (unsigned char)((reg.ReadCommitted(5) & ~0x7F) | (value >> 9)));
}

//----------------------------------------------------------------------------------------
unsigned int VDP::GetHScrollTableA() const
{
	unsigned int scrollTableAddress;

	scrollTableAddress = (reg.ReadCommitted(13) & 0x3F);
	scrollTableAddress <<= 10;

	return scrollTableAddress;
}

//----------------------------------------------------------------------------------------
unsigned int VDP::GetHScrollTableB() const
{
	return GetHScrollTableA() + 2;
}

//----------------------------------------------------------------------------------------
unsigned int VDP::GetVScrollTableA() const
{
	return 0;
}

//----------------------------------------------------------------------------------------
unsigned int VDP::GetVScrollTableB() const
{
	return GetVScrollTableA() + 2;
}

//----------------------------------------------------------------------------------------
void VDP::SetHScrollTableAddress(unsigned int value)
{
	reg.WriteCommitted(13, (unsigned char)((reg.ReadCommitted(13) & ~0x3F) | ((value >> 10) & 0x3F)));
}

//----------------------------------------------------------------------------------------
//Size and movement
//----------------------------------------------------------------------------------------
unsigned int VDP::GetScrollSizeH() const
{
	unsigned int size = 0;

	switch(reg.ReadCommitted(16) & 0x03)
	{
	case 0:	//V32 cell
		size = 32;
		break;
	case 1:	//V64 cell
		size = 64;
		break;
	case 3:	//V128 cell
		size = 128;
		break;
	case 2:	//Prohibited
		size = 32;
		break;
	}

	return size;
}

//----------------------------------------------------------------------------------------
void VDP::SetScrollSizeH(unsigned int value)
{
	if(value >= 128)
	{
		reg.WriteCommitted(16, (reg.ReadCommitted(16) & ~0x03) | (0x03));
	}
	else if(value >= 64)
	{
		reg.WriteCommitted(16, (reg.ReadCommitted(16) & ~0x03) | (0x01));
	}
	else
	{
		reg.WriteCommitted(16, (reg.ReadCommitted(16) & ~0x03) | (0x00));
	}
}

//----------------------------------------------------------------------------------------
unsigned int VDP::GetScrollSizeV() const
{
	unsigned int size = 0;

//##NOTE## Based on tests performed with the window distortion test rom, it appears that
//the prohibited hscroll size 10 actually changes the vertical scroll size, limiting it
//to one cell.

//Handle prohibited hscroll mode 10
	if((reg.ReadCommitted(16) & 0x03) == 2)
	{
		return 1;
	}

	switch((reg.ReadCommitted(16) & 0x30) >> 4)
	{
	case 0:	//V32 cell
		size = 32;
		break;
	case 1:	//V64 cell
		size = 64;
		break;
	case 3:	//V128 cell
		size = 128;
		break;
	case 2:	//Prohibited
		//##UNDEFINED##
		//##TODO## Check what the VDP does for this mode
		size = 32;
		break;
	}

	return size;
}

//----------------------------------------------------------------------------------------
void VDP::SetScrollSizeV(unsigned int value)
{
	if(value >= 128)
	{
		reg.WriteCommitted(16, (reg.ReadCommitted(16) & ~0x30) | (0x03 << 4));
	}
	else if(value >= 64)
	{
		reg.WriteCommitted(16, (reg.ReadCommitted(16) & ~0x30) | (0x01 << 4));
	}
	else
	{
		reg.WriteCommitted(16, (reg.ReadCommitted(16) & ~0x30) | (0x00 << 4));
	}
}

//----------------------------------------------------------------------------------------
unsigned int VDP::GetVisibleCellWidth() const
{
	unsigned int width = 0;

	switch(((reg.ReadCommitted(12) & 0x80) >> 6) | (reg.ReadCommitted(12) & 0x01))
	{
	case 0:
		width = 32;
		break;
	case 1:
		//##UNDEFINED##
		//##TODO## Check what the VDP does if RS0 and RS1 don't match
		break;
	case 2:
		//##UNDEFINED##
		//##TODO## Check what the VDP does if RS0 and RS1 don't match
		break;
	case 3:
		width = 40;
		break;
	}

	return width;
}

//--------------------------------------------------------------------------------------
void VDP::SetVisibleCellWidth(unsigned int value)
{
	if(value >= 40)
	{
		reg.WriteCommitted(12, (reg.ReadCommitted(12) & ~0x81) | (0x81));
	}
	else
	{
		reg.WriteCommitted(12, (reg.ReadCommitted(12) & ~0x81) | (0x00));
	}
}

//----------------------------------------------------------------------------------------
unsigned int VDP::GetVisibleCellHeight() const
{
	unsigned int height = 0;

	if((reg.ReadCommitted(1) & 0x08) != 0)
	{
		//##TODO## This screen height is supposed to be exclusive to PAL mode. Test what
		//happens on the real system when trying to enable this height when running in
		//NTSC mode. Also test if changing back from NTSC to PAL mode after trying to
		//enable a 30 cell height causes the 30 cell display to immediately become
		//active, or whether another register write needs to be sent.
		//##NOTE## Apparently this mode causes a total meltdown in NTSC mode. See various
		//notes by Charles MacDonald.
		height = 30;
	}
	else
	{
		height = 28;
	}

	return height;
}

//----------------------------------------------------------------------------------------
unsigned int VDP::GetVisibleCellHeight(double accessTime) const
{
	unsigned int regData = reg.Read(1, accessTime);
	unsigned int height = 0;

	if((regData & 0x08) != 0)
	{
		//##TODO## This screen height is supposed to be exclusive to PAL mode. Test what
		//happens on the real system when trying to enable this height when running in
		//NTSC mode. Also test if changing back from NTSC to PAL mode after trying to
		//enable a 30 cell height causes the 30 cell display to immediately become
		//active, or whether another register write needs to be sent.
		//##NOTE## Apparently this mode causes a total meltdown in NTSC mode. See various
		//notes by Charles MacDonald.
		height = 30;
	}
	else
	{
		height = 28;
	}

	return height;
}

//----------------------------------------------------------------------------------------
void VDP::SetVisibleCellHeight(unsigned int value)
{
	if(value >= 30)
	{
		reg.WriteCommitted(1, (reg.ReadCommitted(1) & ~0x08) | (0x08));
	}
	else
	{
		reg.WriteCommitted(1, (reg.ReadCommitted(1) & ~0x08) | (0x00));
	}
}

//----------------------------------------------------------------------------------------
VDP::ScrollModeH VDP::GetScrollModeH() const
{
	ScrollModeH mode;

	switch(reg.ReadCommitted(11) & 0x03)
	{
	case 0:	//Overall
		mode = SCROLLH_OVERALL;
		break;
	case 2:	//Cell
		mode = SCROLLH_CELL;
		break;
	case 3:	//Line
		mode = SCROLLH_LINE;
		break;
	case 1:	//Prohibited
		//##UNDEFINED##
		//##NOTE## Based on the EA logo for Populous, we determined when the scroll mode
		//is set to 1, the system behaves the same as mode 0, with overall scrolling.
		//Needs to be tested on real hardware for absolute confirmation.
		mode = SCROLLH_OVERALL;
		break;
	}

	return mode;
}

//----------------------------------------------------------------------------------------
void VDP::SetScrollModeH(unsigned int value)
{
	value %= 4;
	reg.WriteCommitted(11, (unsigned char)((reg.ReadCommitted(11) & ~0x03) | (value)));
}

//----------------------------------------------------------------------------------------
VDP::ScrollModeV VDP::GetScrollModeV() const
{
	ScrollModeV mode;

	switch((reg.ReadCommitted(11) & 0x04) >> 2)
	{
	case 0:	//Overall
		mode = SCROLLV_OVERALL;
		break;
	case 1:	//2 Cell
		mode = SCROLLV_2CELL;
		break;
	}

	return mode;
}

//----------------------------------------------------------------------------------------
void VDP::SetScrollModeV(unsigned int value)
{
	reg.WriteCommitted(11, (reg.ReadCommitted(11) & ~0x04) | ((value != 0)? 0x04: 0x00));
}

//----------------------------------------------------------------------------------------
bool VDP::IsScrollSizeHInvalid() const
{
	return ((reg.ReadCommitted(16) & 0x03) == 2);
}

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
bool VDP::WindowRightAligned() const
{
	return (reg.ReadCommitted(17) & 0x80) != 0;
}

//----------------------------------------------------------------------------------------
void VDP::SetWindowRightAligned(bool value)
{
	reg.WriteCommitted(17, (reg.ReadCommitted(17) & ~0x80) | ((value == true)? 0x80: 0x00));
}

//----------------------------------------------------------------------------------------
bool VDP::WindowDownAligned() const
{
	return (reg.ReadCommitted(18) & 0x80) != 0;
}

//----------------------------------------------------------------------------------------
void VDP::SetWindowDownAligned(bool value)
{
	reg.WriteCommitted(18, (reg.ReadCommitted(18) & ~0x80) | ((value == true)? 0x80: 0x00));
}

//----------------------------------------------------------------------------------------
unsigned int VDP::WindowBasePointX() const
{
	return (reg.ReadCommitted(17) & 0x1F);
}

//----------------------------------------------------------------------------------------
void VDP::SetWindowBasePointX(unsigned int value)
{
	reg.WriteCommitted(17, (unsigned char)((reg.ReadCommitted(17) & ~0x1F) | (value & 0x1F)));
}

//----------------------------------------------------------------------------------------
unsigned int VDP::WindowBasePointY() const
{
	return (reg.ReadCommitted(18) & 0x1F);
}

//----------------------------------------------------------------------------------------
void VDP::SetWindowBasePointY(unsigned int value)
{
	reg.WriteCommitted(18, (unsigned char)((reg.ReadCommitted(18) & ~0x1F) | (value & 0x1F)));
}

//----------------------------------------------------------------------------------------
//##NOTE## How the field size of the window is calculated is not documented, however
//tests with several games have shown that the horizontal field size does toggle between
//0x20 and 0x40 based on the cell width of the screen. Compare HUD in QuackShot to HUD in
//Arrow Flash.
unsigned int VDP::GetWindowScrollSizeH() const
{
	if(GetVisibleCellWidth() > 0x20)
	{
		return 0x40;
	}
	else
	{
		return 0x20;
	}
}

//----------------------------------------------------------------------------------------
unsigned int VDP::GetWindowScrollSizeV() const
{
	return 0x20;
}

//----------------------------------------------------------------------------------------
//HV counter functions
//----------------------------------------------------------------------------------------
bool VDP::GetHVCounterStop() const
{
	return (reg.ReadCommitted(0) & 0x02) != 0;
}

//----------------------------------------------------------------------------------------
void VDP::SetHVCounterStop(bool value)
{
	reg.WriteCommitted(0, (reg.ReadCommitted(0) & ~0x02) | ((value == true)? 0x02: 0x00));
}

//----------------------------------------------------------------------------------------
//Interrupt functions
//----------------------------------------------------------------------------------------
bool VDP::GetHInterruptEnabled() const
{
	return (reg.ReadCommitted(0) & 0x10) != 0;
}

//----------------------------------------------------------------------------------------
void VDP::SetHInterruptEnabled(bool value)
{
	reg.WriteCommitted(0, (reg.ReadCommitted(0) & ~0x10) | ((value == true)? 0x10: 0x00));
}

//----------------------------------------------------------------------------------------
bool VDP::GetVInterruptEnabled() const
{
	return (reg.ReadCommitted(1) & 0x20) != 0;
}

//----------------------------------------------------------------------------------------
void VDP::SetVInterruptEnabled(bool value)
{
	reg.WriteCommitted(1, (reg.ReadCommitted(1) & ~0x20) | ((value == true)? 0x20: 0x00));
}

//----------------------------------------------------------------------------------------
unsigned int VDP::GetHInterruptCounter() const
{
	return reg.ReadCommitted(10);
}

//----------------------------------------------------------------------------------------
void VDP::SetHInterruptCounter(unsigned int value)
{
	reg.WriteCommitted(10, (unsigned char)((value & 0xFF)));
}

//----------------------------------------------------------------------------------------
//Interlace functions
//----------------------------------------------------------------------------------------
VDP::InterlaceMode VDP::GetInterlaceMode() const
{
	InterlaceMode mode;

	switch((reg.ReadCommitted(12) & 0x06) >> 1)
	{
	case 0:
		mode = INTERLACE_NONE;
		break;
	case 1:
		mode = INTERLACE_ON;
		break;
	case 2:
		//##UNDEFINED##
		mode = INTERLACE_PROHIBITED;
		break;
	case 3:
		mode = INTERLACE_DOUBLE;
		break;
	}

	return mode;
}

//----------------------------------------------------------------------------------------
void VDP::SetInterlaceMode(unsigned int value)
{
	reg.WriteCommitted(12, (unsigned char)((reg.ReadCommitted(12) & ~0x06) | ((value << 1) & 0x06)));
}

//----------------------------------------------------------------------------------------
//Miscellaneous settings
//----------------------------------------------------------------------------------------
bool VDP::GetDisplayEnabled() const
{
	return (reg.ReadCommitted(1) & 0x40) != 0;
}

//----------------------------------------------------------------------------------------
void VDP::SetDisplayEnabled(bool value)
{
	reg.WriteCommitted(1, (reg.ReadCommitted(1) & ~0x40) | ((value == true)? 0x40: 0x00));
}

//----------------------------------------------------------------------------------------
bool VDP::GetShadowHighlightEnabled() const
{
	return (reg.ReadCommitted(12) & 0x08) != 0;
}

//----------------------------------------------------------------------------------------
void VDP::SetShadowHighlightEnabled(bool value)
{
	reg.WriteCommitted(12, (reg.ReadCommitted(12) & ~0x08) | ((value == true)? 0x08: 0x00));
}

//----------------------------------------------------------------------------------------
VDP::PaletteIndex VDP::GetBackgroundPaletteIndex() const
{
	PaletteIndex index;

	index.entry = (reg.ReadCommitted(7) & 0x0F);
	index.line = (reg.ReadCommitted(7) & 0x30) >> 4;

	return index;
}

//----------------------------------------------------------------------------------------
void VDP::SetBackgroundPaletteLine(unsigned int value)
{
	reg.WriteCommitted(7, (unsigned char)((reg.ReadCommitted(7) & ~0x30) | ((value << 4) & 0x30)));
}

//----------------------------------------------------------------------------------------
void VDP::SetBackgroundPaletteEntry(unsigned int value)
{
	reg.WriteCommitted(7, (unsigned char)((reg.ReadCommitted(7) & ~0x0F) | (value & 0x0F)));
}
