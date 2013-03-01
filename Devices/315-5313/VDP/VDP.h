/*--------------------------------------------------------------------------------------*\
Description:
This core emulates the Sega video display processor model 315-5313, first used in the
Sega Mega Drive/Genesis console. This processor is an evolution of the VDP used in the
Sega Master System, which is in turn an evolution of the TMS9918 developed by Texas
Instruments.

Timing Points:
-When the control or data ports are read from or written to out of order.
-When a horizontal interrupt, vertical interrupt, or external interrupt is generated
-Raster 0 of each frame

Known Inaccuracies:
-Bit 2 of reg 0 has no effect. When set, this bit should enable the undocumented 8-color
palette mode. This mode is not currently supported.
-Bit 2 of reg 1 has no effect. When cleared, this bit should enable Mode 4. This mode is
not currently supported.
-Make sure you emulate the 128kb boundary wrapping for DMA operations. Apparently, if the
VDP is performing DMA, it will only increment the lower 16 bits of the DMA source address
during the operation. This means if you try and DMA across a 128kb segment, the source
address will wrap back around to the start of that block. See the following thread:
http://www.spritesmind.net/_GenDev/forum/viewtopic.php?t=174&postdays=0&postorder=asc&start=30
-Currently, the sprite layer is being drawn even when the display is disabled. See two
player mode in Sonic 2, with the divider in the centre.

References:
-Genesis Software Manual, Sega Enterprises, 1989, scans by antime
-Sega Genesis VDP documentation (genvdp.txt), Charles MacDonald, v1.5f 08/10/2000
-Sega Master System VDP documentation (msvdp.txt), Charles MacDonald, 11/12/2002
-TMS9918A/TMS9928A/TMS9929A Video Display Processors, Texas Instruments, 1982

*****************************************************************************************
//##TODO## Implement proper support for interlace modes. I suggest two modes of emulation.
//The first mode renders the image at double size. Each field is interlaced in a literal
//fashion, with the first frame occupying all the even rows or pixels, and the second
//frame occupying all the odd rows. The second mode will be a hyper-accurate emulation of
//the interlace modes, and will require the image to be rendered at a minimum of 4x its
//normal size. The purpose of this mode is to account for the fact that while the position
//of the raster beam may be offset by half a pixel on screen, the width of the beam is
//not halved. What this means is that each frame partially overwrites the previous frame,
//at the edges where the two fields meet. This effect creates a noticeable flicker on the
//screen when playing on the original system. We will attempt to emulate this effect by
//drawing a 3x3 pixel block for each pixel output by the VDP. When drawing the odd frame,
//the render position will be offset by 2 pixels. This will add an extra row of pixels
//between each line of an interlaced frame, which will flicker when each field is drawn.
//##NOTE## Alternately, we could attempt to add hyper-accurate support for interlace modes
//by drawing each alternate field to a separate texture, and combining them with alpha
//blending.

//##TODO##

We're planning a change to the way rendering is performed. The plan is to buffer all
register changes, and record the time at which they are made. During the active scan
period, no actual rendering is performed. At the end of the frame, we trigger an update
task, which renders the frame that just passed. At this point, we know the full set of
register changes that occur during the frame, and the times at which they occur, so there
is no need to ever rollback the rendering task itself. Rendering will be performed once
per frame only. We render cell by cell, through the entire frame, until the frame is
complete. During the rendering process, we read the buffered register changes, and apply
them once we catch up with the time they were made. This update model is the same as the
one which we are planning to apply to the sound devices.

There are a few additional points which need to be covered:
One outstanding issue is memory access. We need to allow memory reads and writes during
active scan. We could have a case where a memory address is modified partway through the
frame, then is read back by the device which wrote it. This presents several problems.
The old value stored in that memory location needs to be available to the update task,
otherwise the new data may be used in the frame before it has technically been written.
Additionally, reads need to correctly return the new value after a write has been made,
otherwise external devices could perceive that their writes to memory are being lost.

The only way to solve the old data issue for the update task is to store and timestamp
each and every write.

The second issue is the status register. The status register problem has two parts. One
part relates to the sprite collision and overflow flags. The second part relates to the
FIFO full and empty flags.

Regarding the sprite collision and overflow flags, we now know how these flags work. The
flags are set when drawing the pixel in which the overflow/collision occurs. The flags
are cleared when the control port is read. We can always calculate the collision and
overflow flags by reading memory contents. If we track accesses to the control port as
well as memory, we will be able to calculate these flags accurately by generating the
flags from the point of the last control port read.


Times when we need to generate timing points:
-On the completion of active scan, always.
-When a hint is triggered
-When the hint counter is modified during active scan
-When hints are enabled during active scan, when they were previously disabled
-When two different devices read from the control port in the wrong order
-When one device reads from a memory location, and another one writes to the same memory
location, and the operations happen in the wrong order.
-When one device is reading from the control port, and another device changes the
sprite table address during active scan, and the operations happen in the wrong order.
-When one device is reading from the control port, and another device changes sprite
data during active scan, and the operations happen in the wrong order.

In order to simplify the initial implementation, we won't add any locks to deal with
multiple devices working with the VDP. This leaves us with the vint timing point, and
several timing points relating to the hint, two of which are unpredictable. In order to
simplify the initial implementation, we will only implement the predictable hint timing
point.

7-2-2008:
Further to the timing point generation times above, a simplified list dealing with
multiple devices is as follows:
-Record the last read attempt from a memory buffer. When a write is made to memory which
should have happened before the read, regardless of the address being written, and the
operations happen in the wrong order, trigger a rollback up to the time of the read.
-Record the last read attempt from the control port. When any write is made to any memory
buffer which should have happened before the control port is read, and the operations
happen in the wrong order, trigger a rollback to the time of the read.
-Using the recorded last read attempt from the control port, if ever a control port read
occurs with a time that is earlier than the last read which was performed, trigger a
rollback.

This system deals with all overlapped memory reads/writes, the contents of the control
port being changed by memory reads, and the flag errors that could occur if the control
port sprite flags were calculated in the wrong order. In this model, we can always trust
the contents of the registers and the memory buffers to be complete and accurate when
generating the results of a control port read, and we'll always detect and rollback when
memory reads and writes happen in the wrong order.

There only leaves us with challenges relating to register access. One glaring omission
which immediately comes to light is that any memory access involves a register write.
The read/write location is incremented after each memory access, and before initiating
memory access, several registers are always set. Due to this, we can only allow access
to the control or data ports on the VDP in the correct order. Last access timestamps
will be kept for the data and control ports, and if any form of access occurs in the
wrong order, a rollback will be triggered. The only port which is not controlled is
the HV counter, which can be accessed freely in any order. There will be some handling
which needs to be done here when external interrupts are added, since it seems the HV
counter most likely resumes after a control port read or register write. If it resumes
after a HV counter read however, we're fine, as long as we force HV counter reads to
occur in the correct order (independently of the other ports).

One thing that becomes evident from the register read/write patterns above, is that we
will in actuality be performing a LOT of ad-hoc memory reads on the VDP registers, since
every memory access operation involves two register reads (address, increment), and one
register write (increment). This means our TimedMemoryAccessBuffer needs to be able to
cope with a lot of ad-hoc memory reads. After examining the TimedMemoryAccessBuffer, we
should reverse the direction the buffered memory reads are made on an ad-hoc read, so
that we start with the latest write, and work our way back. Currently, we have to check
every buffered memory write until we reach the end of the list, or pass the time the read
is being performed. If we start from the end and work our way back, we can stop as soon
as we find a write to the same address we are reading from. Performance should basically
be equivalent when no writes have been made to the target address, but much higher when
writes are being repeatedly performed on an address.

We've now optimized the ad-hoc read for TimedMemoryAccessBuffer and implemented the
TimedMemoryAccessValue container, but it's obvious we've operated on a few incorrect
assumptions in these last few steps. The current memory access position isn't stored in
the main VDP register block, it's one of the separate control port registers. We created
the TimedMemoryAccessValue container to allow us to track access to these registers over
time, but this isn't really important at all, since the state of the control port
registers has absolutely no effect on the rendering process, and we have to enforce that
port access happens in the correct order anyway, because partial writes to the control
port can be performed, keeping the existing contents of the non-written portions of
the port, and access to the data port always needs to be in sync with the current
control port register state.

We now need to re-examine our rollback criteria. There are only four kinds of access to
the VDP which we need to track (ignoring the HV counter, which we'll worry about later):
1. Control port read (Read Status Register)
2. Control port write (Write Control Port Register) 
3. Data port read (Memory Read)
4. Data port write (Memory Write)
Within these four kinds of access, there are two kinds of updates going on we have to
guard. One is triggered by reading the status register. Reads of the status register
must occur in the correct order, always. The second type is a control port register
modification. The control port registers are modified by data port reads and writes,
as well as directly writing to the control port. These operations must always occur in
the correct order. In addition to this, the status register must be read in the correct
order in almost all cases, due to flags such as the write FIFO flags, and the possible
impact which memory and register changes could have on the result.

What all of this has told us is that the VDP access must always occur in the correct
order to and between the control and data ports. This greatly simplifies our memory
access model. We will maintain a last access timestamp for these ports, compare the
access time for each new operation, and if the time is less than the last stored time,
trigger a rollback. We will also need to maintain a timestamp for the last control port
read, so we can calculate the sprite collision and overflow flags.

One remaining thing to discuss is the FIFO flags. In order to support these, we need to
track the times of the last memory writes to the VDP, and stick them in a list of our
own, mirroring the FIFO stack. Thanks to our TimedRAM buffer, we can make all memory
writes immediately when they are sent to the VDP, but modify the actual write times that
are sent to the TimedRAM buffer, snapping the time to the next available access slot.
We do however need to store these generated write times in a list, so we know on
subsequent writes what the next available slot is, and we know when the status register
is read what the state of the FIFO flags should be.


18-2-2008
We're at the point of implementation of our new design for the VDP core, and we need to
re-design the way our updates work. It's becoming apparent that with our new separate
rendering task, there are fundamentally two kinds of updates our VDP core needs to do:
1. Interrupt triggers and general flag updates. Happen at the time they occur. Generate
timing points, some of which are predictable, some of which are not. This is the bulk
of our current ExecuteStep method. Should be run as an ExecuteStep process.
2. Rendering process. Occurs one frame behind the actual system. Should be run as a
timer, with the frequency matching the output video frequency (eg, 50hz or 60hz).


16-3-2008
We now finally have our TimedRAM buffers implemented and debugged, and our separate VDP
render thread implemented and working. The main ExecuteStep process seems to be working,
interrupts appear to be generated correctly at the correct times, and register/memory
writes are being read at the correct times by the render thread. We have water working
correctly in Sonic 1 for example.

We're now on to the next phase of the rebuild, which is focused squarely at the status
register currently. Water is broken in Sonic 2 right now, along with 2 player mode, and
after checking the code, it appears that it is most likely related to incorrect handling
of the vblank flag in the status register. The next phase of the project is to get the
status register correct.


References:
-Genesis Software Manual, Sega Enterprises, 1992, scans by antime
-Sega Genesis VDP Documentation (genvdp.txt), Charles MacDonald, 10/8/2000
-Sega Master System VDP Documentation (msvdp.txt), Charles MacDonald, 12/11/2002
-315-5313 Information (vdppin.txt), Charles MacDonald, 2008
\*--------------------------------------------------------------------------------------*/

#ifndef __VDP_H__
#define __VDP_H__
#include "SystemInterface/SystemInterface.pkg"
#include <vector>
#include <list>
#include <map>
#include "Device/Device.pkg"
#include "TimedBuffers/TimedBuffers.pkg"
//##FIX##
#include "../../Memory/TimedRAM.h"
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

class VDP :public Device
{
public:
	//Structures
	struct Color;
	struct PaletteIndex
	{
		PaletteIndex()
		:line(0), entry(0), shadow(false), highlight(false)
		{}

		unsigned int line;
		unsigned int entry;
		bool shadow;
		bool highlight;
	};

	struct BlockMapping;
	struct SpriteMapping;
	struct SpriteDataEntry
	{
		bool written;
		bool priority;
		PaletteIndex entry;
	};

	struct RasterPos
	{
		RasterPos(unsigned int aline = 0, unsigned int apixel = 0, bool avblank = false, bool ahblank = false)
		:line(aline), pixel(apixel), vblank(avblank), hblank(ahblank)
		{}

		void SetRasterPos(unsigned int aline, unsigned int apixel, bool avblank, bool ahblank)
		{
			line = aline;
			pixel = apixel;
			vblank = avblank;
			hblank = ahblank;
		}

		bool operator==(const RasterPos& target) const
		{
			if(vblank && target.vblank)
			{
				return true;
			}
			else if((hblank && target.hblank) && (line == target.line))
			{
				return true;
			}
			else if((pixel == target.pixel) && (line == target.line) && (!hblank && !target.hblank) && (!vblank && !target.vblank))
			{
				return true;
			}
			return false;
		}

		bool operator!=(const RasterPos& target) const
		{
			return !(*this == target);
		}

		unsigned int line;
		unsigned int pixel;
		bool vblank;
		bool hblank;
	};

	struct RasterPosNew
	{
		RasterPosNew(unsigned int arasterCount = 0, unsigned int arefreshRate = 0, unsigned int arasterNo = 0, double arasterProgress = 0.0)
		:rasterCount(arasterCount), refreshRate(arefreshRate), rasterNo(arasterNo), rasterProgress(arasterProgress)
		{}

		void SetDisplaySettings(unsigned int arasterCount, unsigned int arefreshRate)
		{
			rasterCount = arasterCount;
			refreshRate = arefreshRate;
		}

		void SetRasterPos(unsigned int acurrentRaster, double arasterProgress)
		{
			rasterNo = acurrentRaster;
			rasterProgress = arasterProgress;
		}

		double GetFrameRenderTime() const
		{
			return 1000000000.0 / refreshRate;
		}

		double GetRasterRenderTime() const
		{
			return GetFrameRenderTime() / rasterCount;
		}

		void AddRasterNo(unsigned int increment)
		{
			rasterNo = (rasterNo + increment) % rasterCount;
		}

		bool operator==(const RasterPosNew& target) const
		{
			return ((rasterNo == target.rasterNo) && (rasterProgress == target.rasterProgress));
		}

		bool operator!=(const RasterPosNew& target) const
		{
			return !(*this == target);
		}

		unsigned int rasterCount;
		unsigned int refreshRate;

		unsigned int rasterNo;
		double rasterProgress;
	};

	struct PortMonitorEntry
	{
		PortMonitorEntry(const std::wstring& atarget, const std::wstring& asource, unsigned int adata)
		:target(atarget), source(asource), data(adata), committed(false)
		{}

		std::wstring source;
		std::wstring target;
		unsigned int data;
		RasterPos accessPos;
		double accessTime;
		bool committed;
	};

	struct FIFOEntry;

	struct XYPair;
	enum MemoryTarget;
	enum ScrollModeH;
	enum ScrollModeV;
	enum InterlaceMode;
	enum Code;
	enum DMAMode;
	enum State;

	typedef std::list<SpriteMapping> SpriteList;
	typedef SpriteMapping SpriteListEntry;

	//Constructors
	VDP(const std::wstring& ainstanceName, unsigned int amoduleID);
	virtual ~VDP();

	//Line functions
	virtual unsigned int GetLineID(const std::wstring& lineName) const;
	virtual std::wstring GetLineName(unsigned int lineID) const;
	virtual unsigned int GetLineWidth(unsigned int lineID) const;
	virtual void SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void TransparentSetLineState(unsigned int targetLine, const Data& lineData);
	virtual void AssertCurrentOutputLineState() const;
	virtual void NegateCurrentOutputLineState() const;

	//Initialization functions
	virtual bool ValidateDevice();
	virtual void Initialize();
	void Reset();
	virtual void BeginExecution();
	virtual void SuspendExecution();

	//Reference functions
	using Device::AddReference;
	virtual bool AddReference(const std::wstring& referenceName, IDevice* target);
	virtual bool AddReference(const std::wstring& referenceName, IBusInterface* target);
	virtual bool RemoveReference(IDevice* target);
	virtual bool RemoveReference(IBusInterface* target);

	//Execute functions
	virtual UpdateMethod GetUpdateMethod() const;
	virtual bool SendNotifyUpcomingTimeslice() const;
	virtual void NotifyUpcomingTimeslice(double nanoseconds);
	virtual double ExecuteStep();
	virtual double GetNextTimingPointInDeviceTime(unsigned int& accessContext) const;
	virtual void ExecuteRollback();
	virtual void ExecuteCommit();

	//CE line state functions
	virtual unsigned int GetCELineID(const std::wstring& lineName, bool inputLine) const;
	virtual void SetCELineInput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber);
	virtual void SetCELineOutput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber);
	virtual unsigned int CalculateCELineStateMemory(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext, double accessTime) const;
	virtual unsigned int CalculateCELineStateMemoryTransparent(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext) const;
	unsigned int BuildCELine(unsigned int targetAddress, bool vdpIsSource, bool transparentAccess, bool currentLowerDataStrobe, bool currentUpperDataStrobe, bool operationIsWrite, bool rmwCycleInProgress, bool rmwCycleFirstOperation) const;

	//Memory interface functions
	virtual IBusInterface::AccessResult ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual IBusInterface::AccessResult WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	void UpdateStatusRegister(double timesliceProgress);
	void UpdateFIFO(double currentTime);
	double WriteFIFO(const FIFOEntry& fifoEntry);
	double GetFIFOCommitTime(double writeTime) const;

	//Render functions
	void ProcessVInt();
	void ProcessHInt();
	void ProcessEInt();
//	double GetRenderTime(const RasterPos& startPos, const RasterPos& endPos) const;
//	double RenderSegment(const RasterPos& startPos, const RasterPos& endPos);
//	RasterPos GetRasterPos(const RasterPos& startPos, double time) const;
	double GetRenderTimeNew(const RasterPosNew& startPos, const RasterPosNew& endPos) const;
	RasterPosNew GetRasterPosNew(const RasterPosNew& startPos, double time) const;
	void RenderThread();
//	double SetNextState(State anextState);
//	double GetStateExecutionTime(State state);

	//Savestate functions
	virtual bool GetScreenshot(IImage& targetImage) const;
	virtual void LoadState(IHeirarchicalStorageNode& node);
	virtual void SaveState(IHeirarchicalStorageNode& node) const;

	//Port functions
	unsigned short ReadDataPort(double accessTime, IDeviceContext* device);
//	unsigned short ReadControlPort(double accessTime, IDeviceContext* device);
	void WriteDataPort(unsigned short data, double accessTime, IDeviceContext* device);
	void WriteControlPort(unsigned short data, double accessTime, IDeviceContext* device);

	//Memory access functions
//	unsigned short ReadCRAM(unsigned int address);
//	unsigned short ReadVRAM(unsigned int address);
//	unsigned short ReadVSRAM(unsigned int address);
//	void WriteCRAM(unsigned int address, unsigned short data);
//	void WriteVRAM(unsigned int address, unsigned short data);
//	void WriteVSRAM(unsigned int address, unsigned short data);
	unsigned short ReadCRAM(unsigned int address, double accessTime, IDeviceContext* device);
	unsigned short ReadVRAM(unsigned int address, double accessTime, IDeviceContext* device);
	unsigned short ReadVSRAM(unsigned int address, double accessTime, IDeviceContext* device);
	void WriteCRAM(unsigned int address, unsigned short data, double accessTime, IDeviceContext* device);
	void WriteVRAM(unsigned int address, unsigned short data, double accessTime, IDeviceContext* device);
	void WriteVSRAM(unsigned int address, unsigned short data, double accessTime, IDeviceContext* device);

	//DMA Functions
	void DMAWrite(double accessTime, IDeviceContext* device, Code targetCode, unsigned int sourceAddress, unsigned int targetAddress, unsigned int transferCount, unsigned int increment);
	void DMACopy(double accessTime, IDeviceContext* device, Code targetCode, unsigned int sourceAddress, unsigned int targetAddress, unsigned int transferCount);
	void DMAFill(double accessTime, IDeviceContext* device, Code targetCode, unsigned int targetAddress, unsigned int transferCount, unsigned int increment, unsigned short data);

	/**********************************Register Functions*******************************\
	| Register functions are safe for use by external devices. Reads and writes will be
	| synchronized across all devices, and rollbacks will be generated where required.
	\***********************************************************************************/
	//Raw register functions
	inline unsigned char ReadRegister(unsigned int regNumber, double accessTime) const;
	inline void WriteRegister(unsigned int regNumber, double accessTime, unsigned char data);
//	inline unsigned short GetStatusRegister(const RasterPos& rasterPos) const;
	inline unsigned short GetStatusRegister(const RasterPosNew& rasterPos) const;

	inline unsigned int GetVisibleCellHeight(double accessTime) const;

	//DMA registers
	inline unsigned int GetDMASourceAddress(double accessTime) const;
	inline void SetDMASourceAddress(double accessTime, unsigned int value);
	inline unsigned int GetDMATransferCount(double accessTime) const;
	inline void SetDMATransferCount(double accessTime, unsigned int value);
	inline unsigned short GetAddressIncrement(double accessTime) const;
	inline void SetAddressIncrement(double accessTime, unsigned int value);
	inline bool DMAEnabled(double accessTime) const;
	inline void SetDMAEnabled(double accessTime, bool value);
	inline DMAMode GetDMAMode(double accessTime) const;

	//HV counter functions
//	inline unsigned short GetHVCounter(const RasterPos& rasterPos) const;
//	inline unsigned char GetHCounter(const RasterPos& rasterPos) const;
//	inline unsigned char GetVCounter(const RasterPos& rasterPos) const;
	inline unsigned short GetHVCounter(const RasterPosNew& rasterPos, double accessTime) const;
	inline unsigned char GetHCounter(const RasterPosNew& rasterPos, double accessTime) const;
	inline unsigned char GetVCounter(const RasterPosNew& rasterPos, double accessTime) const;

	//Interrupt functions
	inline bool GetHInterruptEnabled(double accessTime) const;
	inline bool GetVInterruptEnabled(double accessTime) const;
	inline unsigned int GetHInterruptCounter(double accessTime) const;

	//Control port registers
	inline bool GetDMAPending() const;
	inline void SetDMAPending(bool value);
	inline bool GetWritePending() const;
	inline void SetWritePending(bool value);
	inline unsigned int GetControlAddress() const;
	inline void SetControlAddress(unsigned int value);
	inline unsigned int GetControlCode() const;
	inline void SetControlCode(unsigned int value);

	//Status register
	inline bool GetWriteFIFOEmpty() const;
	inline void SetWriteFIFOEmpty(bool value);
	inline bool GetWriteFIFOFull() const;
	inline void SetWriteFIFOFull(bool value);
	inline bool GetVInterruptHappened() const;
	inline void SetVInterruptHappened(bool value);
//	inline bool GetSpriteOverflow() const;
//	inline void SetSpriteOverflow(bool value);
//	inline bool GetSpriteCollision() const;
//	inline void SetSpriteCollision(bool value);
	inline bool GetOddInterlaceFrame() const;
	inline void SetOddInterlaceFrame(bool value);
	inline bool GetDuringVBlank() const;
	inline void SetDuringVBlank(bool value);
	inline bool GetDuringHBlank() const;
	inline void SetDuringHBlank(bool value);
	inline bool GetDMABusy() const;
	inline void SetDMABusy(bool value);
	inline bool GetPALMode() const;
	inline void SetPALMode(bool value);

protected:
	//Window functions
	virtual void AddDebugMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher);
	virtual void RestoreViewModelState(const std::wstring& menuHandlerName, int viewModelID, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher);
	virtual void OpenViewModel(const std::wstring& menuHandlerName, int viewModelID, IViewModelLauncher& viewModelLauncher);

private:
	/*********************************Synchronized Functions****************************\
	| Synchronized register functions operate directly on the committed register/memory
	| state, and should only be called internally by the VDP core when the buffers are
	| synchronized. Write functions should only be used for debugging purposes, as
	| writing directly to the committed state does not generate rollbacks in cases of
	| overlapped access.
	\***********************************************************************************/
	//Raw register functions
	inline unsigned char ReadRegister(unsigned int regNumber) const;
	inline void WriteRegister(unsigned int regNumber, unsigned char data);

	//DMA registers
	inline unsigned int GetDMASourceAddress() const;
	inline void SetDMASourceAddress(unsigned int value);
	inline unsigned int GetDMATransferCount() const;
	inline void SetDMATransferCount(unsigned int value);
	inline unsigned short GetAddressIncrement() const;
	inline void SetAddressIncrement(unsigned int value);
	inline bool DMAEnabled() const;
	inline void SetDMAEnabled(bool value);
	inline DMAMode GetDMAMode() const;

	//Data table addresses
	inline unsigned int GetMappingAddressA() const;
	inline void SetMappingAddressA(unsigned int value);
	inline unsigned int GetMappingAddressB() const;
	inline void SetMappingAddressB(unsigned int value);
	inline unsigned int GetMappingAddressWindow() const;
	inline void SetMappingAddressWindow(unsigned int value);
	inline unsigned int GetSpriteTableAddress() const;
	inline void SetSpriteTableAddress(unsigned int value);
	inline unsigned int GetHScrollTableA() const;
	inline unsigned int GetHScrollTableB() const;
	inline unsigned int GetVScrollTableA() const;
	inline unsigned int GetVScrollTableB() const;
	inline void SetHScrollTableAddress(unsigned int value);

	//Size and movement
	inline unsigned int GetScrollSizeH() const;
	inline void SetScrollSizeH(unsigned int value);
	inline unsigned int GetScrollSizeV() const;
	inline void SetScrollSizeV(unsigned int value);
	inline unsigned int GetVisibleCellWidth() const;
	inline void SetVisibleCellWidth(unsigned int value);
	inline unsigned int GetVisibleCellHeight() const;
	inline void SetVisibleCellHeight(unsigned int value);
	inline ScrollModeH GetScrollModeH() const;
	inline void SetScrollModeH(unsigned int value);
	inline ScrollModeV GetScrollModeV() const;
	inline void SetScrollModeV(unsigned int value);
	inline bool IsScrollSizeHInvalid() const;

	//Window functions
	inline bool WindowRightAligned() const;
	inline void SetWindowRightAligned(bool value);
	inline bool WindowDownAligned() const;
	inline void SetWindowDownAligned(bool value);
	inline unsigned int WindowBasePointX() const;
	inline void SetWindowBasePointX(unsigned int value);
	inline unsigned int WindowBasePointY() const;
	inline void SetWindowBasePointY(unsigned int value);
	inline unsigned int GetWindowScrollSizeH() const;
	inline unsigned int GetWindowScrollSizeV() const;

	//HV counter functions
	//##TODO## Rename these to something more appropriate. They don't actually set the
	//counter to the specified number, rather they update it for the specified position.
	//##TODO## Ignore the above and phase out these functions. We want to generate the
	//HV counter on the fly based on the current execution time.
	//inline void SetHCounter(unsigned short data);
	//inline void SetVCounter(unsigned short data);
	inline bool GetHVCounterStop() const;
	inline void SetHVCounterStop(bool value);

	//Interrupt functions
	inline bool GetHInterruptEnabled() const;
	inline void SetHInterruptEnabled(bool value);
	inline bool GetVInterruptEnabled() const;
	inline void SetVInterruptEnabled(bool value);
	inline unsigned int GetHInterruptCounter() const;
	inline void SetHInterruptCounter(unsigned int value);

	//Interlace functions
	inline InterlaceMode GetInterlaceMode() const;
	inline void SetInterlaceMode(unsigned int value);

	//Miscellaneous settings
	inline bool GetDisplayEnabled() const;
	inline void SetDisplayEnabled(bool value);
	inline bool GetShadowHighlightEnabled() const;
	inline void SetShadowHighlightEnabled(bool value);
	inline PaletteIndex GetBackgroundPaletteIndex() const;
	inline void SetBackgroundPaletteLine(unsigned int value);
	inline void SetBackgroundPaletteEntry(unsigned int value);

	///////////////////////////////////Render Functions//////////////////////////////////
	//Window functions
	unsigned int WindowStartX() const;
	unsigned int WindowStartY() const;
	unsigned int GetWindowWidth() const;
	unsigned int GetWindowHeight() const;
	bool CoordinatesInWindow(unsigned int x, unsigned int y) const;

	//Block selection functions
	BlockMapping DecodeBlockMapping(unsigned int mappingAddress) const;
	unsigned int GetBlockPaletteEntry(const BlockMapping& mapping, unsigned int xoffset, unsigned int yoffset) const;
	unsigned int GetSpriteMappingSize() const;
	SpriteMapping GetSpriteMapping(unsigned int spriteMappingAddress) const;
	void SetSpriteMapping(unsigned int spriteMappingAddress, const SpriteMapping& mapping);
	PaletteIndex GetSpritePaletteIndex(const SpriteMapping& mapping, unsigned int x, unsigned int y) const;
	Color GetPaletteColor(const PaletteIndex& index) const;

	//Debug functions
	void RecordPortMonitorEntry(const PortMonitorEntry& entry);
	void ClearPortMonitorList();

	//Render functions
	void SelectBlock(unsigned int xoffset, unsigned int yoffset, unsigned int fieldWidth, unsigned int fieldHeight, unsigned int mappingTableAddress, BlockMapping& blockMapping) const;
	int GetScrollOffsetX(unsigned int scrollTableBase, ScrollModeH scrollMode, unsigned int y) const;
	int GetScrollOffsetY(unsigned int scrollTableBase, ScrollModeV scrollMode, unsigned int x) const;
	void BuildSpriteData(unsigned int line);
	void RenderLine(unsigned int line);
	void WriteColorToImageBuffer(const Color& color, unsigned int xpos, unsigned int ypos);
	void WritePixelToImageBuffer(const PaletteIndex& paletteIndex, unsigned int xpos, unsigned int ypos);

	//Window functions
	void OpenSpriteListDetailsView(unsigned int aspriteMappingAddress, unsigned int aspriteIndex);
	void OpenPortMonitorDetailsView(const PortMonitorEntry& aentry);

private:
	//Enumerations
	enum CELineID
	{
		CELINE_LDS = 1,
		CELINE_UDS,
		CELINE_RW,
		CELINE_AS,
		CELINE_RMWCYCLEINPROGRESS,
		CELINE_RMWCYCLEFIRSTOPERATION,
		CELINE_LWR,
		CELINE_UWR,
		CELINE_CAS0,
		CELINE_RAS0,
		CELINE_OE0
	};
	enum LineID
	{
		LINE_IPL = 1,
		LINE_INT,
		LINE_INTAK
	};
	enum AccessContext
	{
		ACCESSCONTEXT_VINT,
		ACCESSCONTEXT_INT,
		ACCESSCONTEXT_HINT
	};

	//Constants
	static const unsigned int registerCount = 24;
	static const unsigned int vramSize = 0x10000;
	static const unsigned int vsramSize = 0x80;
	static const unsigned int cramSize = 0x80;

	//View and menu classes
	class DebugMenuHandler;
	class VRAMViewModel;
	class PaletteViewModel;
	class ImageViewModel;
	class RegistersViewModel;
	class LayerRemovalViewModel;
	class SettingsViewModel;
	class SpriteListViewModel;
	class SpriteListDetailsViewModel;
	class PortMonitorViewModel;
	class PortMonitorDetailsViewModel;
	class VRAMView;
	class PaletteView;
	class ImageView;
	class RegistersView;
	class LayerRemovalView;
	class SettingsView;
	class SpriteListView;
	class SpriteListDetailsView;
	class PortMonitorView;
	class PortMonitorDetailsView;
	friend class VRAMViewModel;
	friend class PaletteViewModel;
	friend class ImageViewModel;
	friend class RegistersViewModel;
	friend class LayerRemovalViewModel;
	friend class SettingsViewModel;
	friend class SpriteListViewModel;
	friend class SpriteListDetailsViewModel;
	friend class PortMonitorViewModel;
	friend class PortMonitorDetailsViewModel;
	friend class VRAMView;
	friend class PaletteView;
	friend class ImageView;
	friend class RegistersView;
	friend class LayerRemovalView;
	friend class SettingsView;
	friend class SpriteListView;
	friend class SpriteListDetailsView;
	friend class PortMonitorView;
	friend class PortMonitorDetailsView;

private:
	//Menu handling
	DebugMenuHandler* menuHandler;

	//Bus interface
	IBusInterface* memoryBus;

	//Render thread data
	mutable boost::mutex renderThreadMutex;
	mutable boost::mutex timesliceMutex;
	boost::condition renderThreadUpdate;
	boost::condition renderThreadStopped;
	bool renderThreadActive;
	bool pendingRenderOperation;
	RandomTimeAccessBuffer<unsigned char, double>::Timeslice regTimeslice;
	RandomTimeAccessBuffer<unsigned char, double>::Timeslice vramTimeslice;
	RandomTimeAccessBuffer<unsigned char, double>::Timeslice cramTimeslice;
	RandomTimeAccessBuffer<unsigned char, double>::Timeslice vsramTimeslice;

	bool vramSync;
	bool cramSync;
	bool vsramSync;
	bool enableWindowDistortion;
	bool enableCRAMFlicker;
	RandomTimeAccessBuffer<unsigned char, double>::Timeslice regTimesliceCopy;
	RandomTimeAccessBuffer<unsigned char, double>::Timeslice vramTimesliceCopy;
	RandomTimeAccessBuffer<unsigned char, double>::Timeslice cramTimesliceCopy;
	RandomTimeAccessBuffer<unsigned char, double>::Timeslice vsramTimesliceCopy;

	//Layer removal settings
	bool enableLayerAHigh;
	bool enableLayerALow;
	bool enableLayerBHigh;
	bool enableLayerBLow;
	bool enableWindowHigh;
	bool enableWindowLow;
	bool enableSpriteHigh;
	bool enableSpriteLow;

	//Port monitor settings
	mutable boost::mutex portMonitorMutex;
	bool logStatusRegisterRead;
	bool logDataPortRead;
	bool logHVCounterRead;
	bool logControlPortWrite;
	bool logDataPortWrite;
	unsigned int portMonitorListSize;
	std::list<PortMonitorEntry> portMonitorList;
	std::list<PortMonitorEntry> bportMonitorList;

	//Interrupt settings
	static const unsigned int exintIPLLineState = 2;
	static const unsigned int hintIPLLineState = 4;
	static const unsigned int vintIPLLineState = 6;

	//Physical registers and memory buffers
	RandomTimeAccessBuffer<unsigned char, double> reg;
	TimedRAM* vram;
	TimedRAM* cram;
	TimedRAM* vsram;

	//Status register variables
	static const unsigned int statusRegisterBitCount = 10;
	Data statusRegister;
	double lastTimesliceLength;
	double statusRegisterLastAccessTime;
	double timesliceOffsetFromRenderPos;
	bool renderPosUpdated;
	bool oddInterlaceFrame;
	bool palMode;
	bool vintHappened;
	bool intLineState;
	unsigned int iplLineState;

	//Old status register variables to be removed
	unsigned int sr;

	//Internal control port registers
	unsigned short address;
	Code code;
	bool writePending;
	bool dmaPending;

	//Image buffer
	//##TODO## Fix up these dimensions
	mutable boost::mutex imageBufferMutex;
	static const unsigned int imageWidth = 512;
	static const unsigned int imageHeight = 512;
	static const unsigned int imageBufferPlanes = 3;
	unsigned char image[imageBufferPlanes][imageHeight * imageWidth * 4];
	unsigned int drawingImageBufferPlane;

	//##TODO## Fix up this definition
	SpriteDataEntry spriteData[1024];
	bool spriteMaskingMode2Active;

	//Timing variables
	State nextState;
	double renderTimeStep;
	RasterPosNew renderPosNextNew;
	RasterPosNew renderPosCurrentNew;
	unsigned int hintCounter;

	//FIFO variables
	static const unsigned int fifoMaxEntries = 4;
	std::list<FIFOEntry> fifoBuffer;
	double fifoCommittedTime;

	//CE line masks
	unsigned int ceLineMaskLowerDataStrobeInput;
	unsigned int ceLineMaskUpperDataStrobeInput;
	unsigned int ceLineMaskReadHighWriteLowInput;
	unsigned int ceLineMaskAddressStrobeInput;
	unsigned int ceLineMaskLowerDataStrobeOutput;
	unsigned int ceLineMaskUpperDataStrobeOutput;
	unsigned int ceLineMaskReadHighWriteLowOutput;
	unsigned int ceLineMaskAddressStrobeOutput;
	unsigned int ceLineMaskRMWCycleInProgress;
	unsigned int ceLineMaskRMWCycleFirstOperation;
	unsigned int ceLineMaskLWR;
	unsigned int ceLineMaskUWR;
	unsigned int ceLineMaskCAS0;
	unsigned int ceLineMaskRAS0;
	unsigned int ceLineMaskOE0;

	//Saved CE line state for Read-Modify-Write cycles
	mutable bool lineLWRSavedStateRMW;
	mutable bool lineUWRSavedStateRMW;
	mutable bool lineCAS0SavedStateRMW;
	mutable bool lineRAS0SavedStateRMW;
	mutable bool lineOE0SavedStateRMW;
	bool blineLWRSavedStateRMW;
	bool blineUWRSavedStateRMW;
	bool blineCAS0SavedStateRMW;
	bool blineRAS0SavedStateRMW;
	bool blineOE0SavedStateRMW;

	//Rollback state info
	unsigned short bsr;
	unsigned short baddress;
	Code bcode;
	bool bwritePending;
	bool bdmaPending;
	RasterPos brenderPosCurrent;

	Data bstatusRegister;
	double blastTimesliceLength;
	double bstatusRegisterLastAccessTime;
	double btimesliceOffsetFromRenderPos;
	bool brenderPosUpdated;
	bool boddInterlaceFrame;
	bool bpalMode;
	bool bvintHappened;
	bool bintLineState;
	unsigned int biplLineState;
};

#include "VDP.inl"
#endif
