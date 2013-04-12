#include "VDP.h"
#include "DebugMenuHandler.h"
//##DEBUG##
#include <iostream>
#include <iomanip>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
VDP::VDP(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID)
:Device(aimplementationName, ainstanceName, amoduleID), menuHandler(0), reg(registerCount, false, 0), statusRegister(statusRegisterBitCount), bstatusRegister(statusRegisterBitCount)
{
	memoryBus = 0;
	vram = 0;
	cram = 0;
	vsram = 0;

	//Initialize our CE line state
	ceLineMaskLowerDataStrobeInput = 0;
	ceLineMaskUpperDataStrobeInput = 0;
	ceLineMaskReadHighWriteLowInput = 0;
	ceLineMaskAddressStrobeInput = 0;
	ceLineMaskRMWCycleInProgress = 0;
	ceLineMaskRMWCycleFirstOperation = 0;
	ceLineMaskLowerDataStrobeOutput = 0;
	ceLineMaskUpperDataStrobeOutput = 0;
	ceLineMaskReadHighWriteLowOutput = 0;
	ceLineMaskAddressStrobeOutput = 0;
	ceLineMaskLWR = 0;
	ceLineMaskUWR = 0;
	ceLineMaskCAS0 = 0;
	ceLineMaskRAS0 = 0;
	ceLineMaskOE0 = 0;

	//We need to initialize these variables here since a commit is triggered before
	//initialization the first time the system is booted.
	pendingRenderOperation = false;

	enableLayerAHigh = true;
	enableLayerALow = true;
	enableLayerBHigh = true;
	enableLayerBLow = true;
	enableWindowHigh = true;
	enableWindowLow = true;
	enableSpriteHigh = true;
	enableSpriteLow = true;

	vramSync = false;
	cramSync = false;
	vsramSync = false;
	enableWindowDistortion = true;
	enableCRAMFlicker = false;

	logStatusRegisterRead = false;
	logDataPortRead = false;
	logHVCounterRead = false;
	logControlPortWrite = false;
	logDataPortWrite = false;
	portMonitorListSize = 200;

	renderThreadActive = false;
	drawingImageBufferPlane = 0;
}

//----------------------------------------------------------------------------------------
VDP::~VDP()
{
	//Delete the menu handler
	if(menuHandler != 0)
	{
		menuHandler->ClearMenuItems();
		delete menuHandler;
	}
}

//----------------------------------------------------------------------------------------
//Line functions
//----------------------------------------------------------------------------------------
unsigned int VDP::GetLineID(const std::wstring& lineName) const
{
	if(lineName == L"IPL")
	{
		return LINE_IPL;
	}
	else if(lineName == L"INT")
	{
		return LINE_INT;
	}
	else if(lineName == L"INTAK")
	{
		return LINE_INTAK;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
std::wstring VDP::GetLineName(unsigned int lineID) const
{
	switch(lineID)
	{
	case LINE_IPL:
		return L"IPL";
	case LINE_INT:
		return L"INT";
	case LINE_INTAK:
		return L"INTAK";
	}
	return L"";
}

//----------------------------------------------------------------------------------------
unsigned int VDP::GetLineWidth(unsigned int lineID) const
{
	switch(lineID)
	{
	case LINE_IPL:
		return 3;
	case LINE_INT:
		return 1;
	case LINE_INTAK:
		return 1;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void VDP::SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	//##DEBUG##
//	std::wcout << "VDPSetLineState\t" << targetLine << '\n';
	if(targetLine == LINE_INTAK)
	{
		iplLineState = 0;
		memoryBus->SetLineState(LINE_IPL, Data(GetLineWidth(LINE_IPL), iplLineState), GetDeviceContext(), caller, accessTime, accessContext);
	}
}

//----------------------------------------------------------------------------------------
void VDP::TransparentSetLineState(unsigned int targetLine, const Data& lineData)
{
	SetLineState(targetLine, lineData, 0, 0.0, 0);
}

//----------------------------------------------------------------------------------------
void VDP::AssertCurrentOutputLineState() const
{
	if(memoryBus != 0)
	{
		if(intLineState)      memoryBus->SetLineState(LINE_INT, Data(GetLineWidth(LINE_INT), 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), ACCESSCONTEXT_INT);
		if(iplLineState != 0) memoryBus->SetLineState(LINE_IPL, Data(GetLineWidth(LINE_IPL), iplLineState), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
}

//----------------------------------------------------------------------------------------
void VDP::NegateCurrentOutputLineState() const
{
	if(memoryBus != 0)
	{
		if(intLineState)      memoryBus->SetLineState(LINE_INT, Data(GetLineWidth(LINE_INT), 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), ACCESSCONTEXT_INT);
		if(iplLineState != 0) memoryBus->SetLineState(LINE_IPL, Data(GetLineWidth(LINE_IPL), 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
}

//----------------------------------------------------------------------------------------
//Initialization functions
//----------------------------------------------------------------------------------------
bool VDP::ValidateDevice()
{
	return (memoryBus != 0) && (vram != 0) && (cram != 0) && (vsram != 0);
}

//----------------------------------------------------------------------------------------
void VDP::Initialize()
{
	reg.Initialize();
	Reset();
}

//----------------------------------------------------------------------------------------
void VDP::Reset()
{
	code = VRAM_READ;
	address = 0;
	writePending = false;
	dmaPending = false;

	nextState = STATE_LINE0;
//	renderPosCurrent.SetRasterPos(0, 0, false, false);
//	renderPosNext.SetRasterPos(0, 0, false, false);
	renderPosCurrentNew.SetRasterPos(0, 0.0);
	renderPosNextNew.SetRasterPos(0, 0.0);

	lineLWRSavedStateRMW = false;
	lineUWRSavedStateRMW = false;
	lineCAS0SavedStateRMW = false;
	lineRAS0SavedStateRMW = false;
	lineOE0SavedStateRMW = false;

	renderPosUpdated = true;
	lastTimesliceLength = 0;
	statusRegisterLastAccessTime = 0;
	timesliceOffsetFromRenderPos = 0;
	oddInterlaceFrame = false;
	vintHappened = false;
	palMode = true;
	intLineState = false;
	iplLineState = 0;

//After numerous tests performed on the actual system, it appears that all VDP registers
//are initialized to 0 on a reset. This is supported by Addendum 4 from SEGA in the
//Genesis Software Manual. Note that since Mode 4 graphics are enabled when bit 2 of
//reg 1 is 0, the VDP starts in Mode 4. This has been confirmed on the hardware.
	for(unsigned int i = 0; i < registerCount; ++i)
	{
		reg.WriteCommitted(i, 0);
	}
}

//----------------------------------------------------------------------------------------
void VDP::BeginExecution()
{
	//Start the render worker thread
	renderThreadActive = true;
	boost::thread workerThread(boost::bind(boost::mem_fn(&VDP::RenderThread), this));
}

//----------------------------------------------------------------------------------------
void VDP::SuspendExecution()
{
	//We take a lock here to ensure that the render thread definitely stops. Without this
	//lock, there's a chance the render thread could miss the notification if it is
	//currently in the process of rendering. We also now use this lock to wait for a
	//stopped condition to be triggered, so that we ensure this function does not return
	//until this device is completely halted.
	boost::mutex::scoped_lock lock(renderThreadMutex);

	//Suspend the render worker thread
	if(renderThreadActive)
	{
		renderThreadActive = false;
		renderThreadUpdate.notify_all();
		renderThreadStopped.wait(lock);
	}
}

//----------------------------------------------------------------------------------------
//Reference functions
//----------------------------------------------------------------------------------------
bool VDP::AddReference(const std::wstring& referenceName, IDevice* target)
{
	if(referenceName == L"VRAM")
	{
		vram = (TimedRAM*)target;
	}
	else if(referenceName == L"CRAM")
	{
		cram = (TimedRAM*)target;
	}
	else if(referenceName == L"VSRAM")
	{
		vsram = (TimedRAM*)target;
	}
	else
	{
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------------------
bool VDP::AddReference(const std::wstring& referenceName, IBusInterface* target)
{
	if(referenceName == L"BusInterface")
	{
		memoryBus = target;
	}
	else
	{
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------------------
bool VDP::RemoveReference(IDevice* target)
{
	if(vram == target)
	{
		vram = 0;
	}
	else if(cram == target)
	{
		cram = 0;
	}
	else if(vsram == target)
	{
		vsram = 0;
	}
	else
	{
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------------------
bool VDP::RemoveReference(IBusInterface* target)
{
	if(memoryBus == target)
	{
		memoryBus = 0;
	}
	else
	{
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------------------
//Execute functions
//----------------------------------------------------------------------------------------
VDP::UpdateMethod VDP::GetUpdateMethod() const
{
	return UPDATEMETHOD_STEP;
}

//----------------------------------------------------------------------------------------
bool VDP::SendNotifyUpcomingTimeslice() const
{
	return true;
}

//----------------------------------------------------------------------------------------
void VDP::NotifyUpcomingTimeslice(double nanoseconds)
{
	//Calculate the final value for the status register at the end of the last timeslice,
	//and prepare for the next timeslice.
	UpdateStatusRegister(lastTimesliceLength);
	statusRegisterLastAccessTime = 0;
	if(renderPosUpdated)
	{
		renderPosUpdated = false;
		timesliceOffsetFromRenderPos = 0;
	}
	else
	{
		timesliceOffsetFromRenderPos += lastTimesliceLength;
	}
	lastTimesliceLength = nanoseconds;

	//Add new timeslice to timed buffers
	reg.AddTimeslice(nanoseconds);
	vram->AddTimeslice(nanoseconds);
	cram->AddTimeslice(nanoseconds);
	vsram->AddTimeslice(nanoseconds);
}

////----------------------------------------------------------------------------------------
//double VDP::ExecuteStep()
//{
//	//##General logic for operation of VDP device:
//	//The VDP is always sitting on a timing point, be it for a VINT, a HINT, or an EINT.
//	//Because of this, GetNextTimingPoint always returns 0. ExecuteStep must return the
//	//amount of time between the current timing point, and the expected execution time of
//	//the next timing point. VINT is always known. HINT should be predicted based on
//	//current state. EINT is completely unpredictable.
//	//##Implementation specifics
//	//Since we're predicting the next timing point on each step, we set a nextState flag
//	//indicating what that timing point will be when executing each step. We take the
//	//existing value for nextState, and execute the operation it indicates without
//	//question. We then predict what the next interrupt will be, and update the nextState
//	//flag accordingly.
//	//##Unexpected interrupt events
//	//According to genvdp, the hint counter is only reloaded after it expires, in which
//	//case, the only time we have to generate an unexpected interrupt for a hint is when
//	//hint is changed from disabled to enabled. In this case, we would have predicted the
//	//next timing point as vint, but we now need to rollback and predict a new timing
//	//point which includes the hint counter. For external interrupts, there is no way
//	//of predicting the time at which they will occur.
//	//##Procedure for rolling back after unexpected interrupts
//	//We will introduce an externalRollback flag, an externalState interrupt flag, and
//	//externalTimingPoint data. The externalRollback flag is a boolean flag indicating
//	//when a rollback has been triggered as a result of an unexpected timing point. It
//	//is checked in the ExecuteStep method to determine whether the external rollback
//	//info should be used
//
//	//If the externalRollback flag is set, always use the 
//
//	//##Procedure for rolling back after eint triggered
//	//Set remainingTime to GetCurrentTimesliceProgress of calling device, and trigger
//	//a rollback, with no step-through scheduled. When the timeslice is re-issued,
//	//ExecuteStep will NOT be called for the VDP, since we have instructed the rollback
//	//not to trigger a step through (NOTE: we need to implement this behaviour). Since
//	//we've modified the remainingTime value, the VDP will now indicate it has a timing
//	//point at the exact moment the interrupt was triggered, since GetCurrentTimesliceProgress
//	//always returns 0. When we call ExecuteStep on this timing point, we use the updated
//	//nextState flag to determine what interrupt to execute.
//
//
////Assumed constants
//	const unsigned int finalLine = 224;
//
//	//Trigger the next interrupt
//	if(nextState == STATE_LINE0)
//	{
//		//##DEBUG##
////		std::wcout << "Line0\n";
//
//		//Reload the hint counter
//		hintCounter = GetHInterruptCounter(GetCurrentTimesliceProgress());
//	}
//	else if(nextState == STATE_VINT)
//	{
//		//##DEBUG##
////		std::wcout << "VInt\n";
//
//		ProcessVInt();
//
//		//Start the rendering process for the frame we've just passed
//		boost::mutex::scoped_lock lock(timesliceMutex);
//		regTimeslice = reg.GetLatestTimeslice();
//		vramTimeslice = vram->GetLatestTimeslice();
//		cramTimeslice = cram->GetLatestTimeslice();
//		vsramTimeslice = vsram->GetLatestTimeslice();
//		renderThreadUpdate.notify_all();
//	}
//	else if(nextState == STATE_HINT)
//	{
//		//##DEBUG##
////		std::wcout << "HInt: " << renderPosNext.line << '\n';
//
//		ProcessHInt();
//
//		//Reload the hint counter
//		hintCounter = GetHInterruptCounter(GetCurrentTimesliceProgress());
//	}
//	else if(nextState == STATE_EXINT)
//	{
//		//##DEBUG##
////		std::wcout << "EInt\n";
//
//		ProcessEInt();
//
//		//Update the hint counter
//		hintCounter -= renderPosNext.line - renderPosCurrent.line;
//	}
//
//	//Calculate next expected timing point in nanoseconds, from the start of the frame
//	renderPosCurrent = renderPosNext;
//	if(renderPosCurrent.vblank)
//	{
//		nextState = STATE_LINE0;
//		renderPosNext = RasterPos(0, 0, false, false);
//	}
//	else if(GetHInterruptEnabled(GetCurrentTimesliceProgress())	//HInt is enabled
//	    && ((renderPosCurrent.line + hintCounter) < (finalLine - 1))	//HInt is going to trigger in active scan
//		&& ((hintCounter > 0) || (renderPosCurrent.pixel < 200)))	//We haven't already triggered HInt on this line
//	{
//		nextState = STATE_HINT;
//		renderPosNext = RasterPos(renderPosCurrent.line + hintCounter, 200, false, false);
//	}
//	else
//	{
//		nextState = STATE_VINT;
//		renderPosNext = RasterPos(0, 0, true, false);
//	}
//
//	//Calculate the amount of time between the current interrupt, and the next expected
//	//interrupt.
//	double executionTime = 0;
//	executionTime = GetRenderTime(renderPosCurrent, renderPosNext);
//	//##DEBUG##
////	std::wcout.setf(std::ios_base::fixed, std::ios_base::floatfield);
////	std::wcout << resetiosflags(std::ios_base::scientific);
////	std::wcout << "ExecutionTime: " << executionTime << '\n';
////	std::wcout << "RenderTimeCurrent:\t" << renderPosCurrent.line << '\t' << renderPosCurrent.pixel << '\t' << renderPosCurrent.vblank << '\t' << renderPosCurrent.hblank << '\n';
////	std::wcout << "RenderTimeNext:\t\t" << renderPosNext.line << '\t' << renderPosNext.pixel << '\t' << renderPosNext.vblank << '\t' << renderPosNext.hblank << '\n';
//	return executionTime;
//}

//----------------------------------------------------------------------------------------
double VDP::ExecuteStep()
{
	//Assumed constants
	const unsigned int blockHeight = 8;

	///////////////////////////////////////////////////////
	//      lines   lines   lines   vcells  vsync   hsync
	//      screen  display retrace
	//NTSC	262     224     38      28      60Hz    15720Hz
	//PAL	312     224     88      28      50Hz    15600Hz
	//PAL	312     240     72      30      50Hz    15600Hz
	///////////////////////////////////////////////////////
	//##TODO## Calculate these values based on the current display settings
	unsigned int rasterCountScreen = 312;
	unsigned int refreshRate = 50;
	unsigned int rasterCountTopBorder = 3 * blockHeight;
	unsigned int rasterCountFrame = 28 * blockHeight;
	unsigned int rasterCountBottomBorder = 3 * blockHeight;
	unsigned int rasterCountRetrace = rasterCountScreen - (rasterCountTopBorder + rasterCountFrame + rasterCountBottomBorder);
	unsigned int rasterPosFrame = rasterCountTopBorder;
	unsigned int rasterPosBottomBorder = rasterCountTopBorder + rasterCountFrame;
	unsigned int rasterPosRetrace = rasterCountTopBorder + rasterCountFrame + rasterCountBottomBorder;

	//Process the next timing point
	if(nextState == STATE_LINE0)
	{
//##DEBUG##
//		std::wcout << "Line0\n";

		//Set the display mode
		renderPosCurrentNew.SetDisplaySettings(rasterCountScreen, refreshRate);
		renderPosNextNew.SetDisplaySettings(rasterCountScreen, refreshRate);

		//Reload the hint counter
		hintCounter = GetHInterruptCounter(GetCurrentTimesliceProgress());

		//Start the rendering process for the frame we've just passed
		pendingRenderOperation = true;
//		boost::mutex::scoped_lock lock(timesliceMutex);
//		regTimeslice = reg.GetLatestTimeslice();
//		vramTimeslice = vram->GetLatestTimeslice();
//		cramTimeslice = cram->GetLatestTimeslice();
//		vsramTimeslice = vsram->GetLatestTimeslice();
//		renderThreadUpdate.notify_all();
	}
	else if(nextState == STATE_VINT)
	{
//##DEBUG##
//		std::wcout << "VInt\n";

		ProcessVInt();
	}
	else if(nextState == STATE_HINT)
	{
//##DEBUG##
//		std::wcout << "HInt: " << renderPosNextNew.rasterNo << '\n';

		ProcessHInt();

		//Reload the hint counter
		hintCounter = GetHInterruptCounter(GetCurrentTimesliceProgress());
	}
	else if(nextState == STATE_EXINT)
	{
//##DEBUG##
//		std::wcout << "EInt\n";

		ProcessEInt();

		//Update the hint counter
		hintCounter -= renderPosNextNew.rasterNo - renderPosCurrentNew.rasterNo;
	}

	//##NOTE## Notes about hint:
	//-Hint should be triggered when counter=0xDF
	//-Hint should trigger on every line when counter=0
	//-Mathematically, should be 224 hint per frame when counter=0
	//-Must generate hint on line 0
	//Good test cases are CPZ2 water in S2, title screen for Vectorman, and weapon select
	//screen for Alien Soldier.

	//Calculate raster position of the next expected timing point
	renderPosCurrentNew = renderPosNextNew;
	//##FIX## HInt isn't being generated when the counter=0
	//	unsigned int nextHIntLine = (renderPosCurrentNew.rasterNo >= rasterPosFrame)? renderPosCurrentNew.rasterNo + hintCounter: rasterPosFrame + hintCounter;
	unsigned int nextHIntLine = (renderPosCurrentNew.rasterNo >= rasterPosFrame)? renderPosCurrentNew.rasterNo + (hintCounter + 1): rasterPosFrame + hintCounter;
	if(GetHInterruptEnabled(GetCurrentTimesliceProgress())	//HInt is enabled
		&& (nextHIntLine < rasterPosBottomBorder))	//HInt is going to trigger in active scan
//		&& ((hintCounter > 0) || (renderPosCurrentNew.rasterProgress < 0.8)))	//We haven't already triggered HInt on this line
//		&& ((renderPosCurrentNew.rasterNo != nextHIntLine) || (nextState != STATE_HINT)))	//We haven't already triggered HInt on this line
	{
		//If we're going to hit a hint before the end of the active screen area, the next
		//expected timing point is hint.
		nextState = STATE_HINT;
		//##TODO## Sample an accurate point during the scanline to generate a hint. We
		//know the true value is less than 0.8, due to flickering in the Vectorman intro
		//screen.
		renderPosNextNew = RasterPosNew(rasterCountScreen, refreshRate, nextHIntLine, 0.999);
	}
	else if((renderPosCurrentNew.rasterNo <= rasterPosBottomBorder) && (nextState != STATE_VINT))
	{
		//If there's no hint before the end of active scan, the next expected timing
		//point is vint.
		nextState = STATE_VINT;
		renderPosNextNew = RasterPosNew(rasterCountScreen, refreshRate, rasterPosBottomBorder);
	}
	else
	{
		//If the current raster position is past the end of the active scan area, the
		//next expected timing point is line 0.
		nextState = STATE_LINE0;
		renderPosNextNew = RasterPosNew(rasterCountScreen, refreshRate, 0);
	}

	//##DEBUG##
	//std::wcout << L"VDP ExecuteStep:\n";
	//std::wcout << std::hex << std::uppercase << renderPosCurrentNew.rasterNo << '\t' << renderPosCurrentNew.rasterProgress << '\n';
	//std::wcout << std::hex << std::uppercase << renderPosNextNew.rasterNo << '\t' << renderPosNextNew.rasterProgress << '\n';

	renderPosUpdated = true;

	//Calculate the amount of time between the current raster position, and the next
	//expected timing point.
	renderTimeStep = GetRenderTimeNew(renderPosCurrentNew, renderPosNextNew);
	return renderTimeStep;
}

//----------------------------------------------------------------------------------------
//We hop from one timing point to the next with our execution method, so we're always
//sitting on a timing point.
//----------------------------------------------------------------------------------------
double VDP::GetNextTimingPointInDeviceTime(unsigned int& accessContext) const
{
	return 0;
}

//----------------------------------------------------------------------------------------
void VDP::ExecuteRollback()
{
	//If a render operation was flagged by the timeslice we're rolling back, cancel the
	//pending render operation.
	pendingRenderOperation = false;

	//Saved CE line state for Read-Modify-Write cycles
	lineLWRSavedStateRMW = blineLWRSavedStateRMW;
	lineUWRSavedStateRMW = blineUWRSavedStateRMW;
	lineCAS0SavedStateRMW = blineCAS0SavedStateRMW;
	lineRAS0SavedStateRMW = blineRAS0SavedStateRMW;
	lineOE0SavedStateRMW = blineOE0SavedStateRMW;

	//Rollback status register data
	statusRegister = bstatusRegister;
	lastTimesliceLength = blastTimesliceLength;
	statusRegisterLastAccessTime = bstatusRegisterLastAccessTime;
	timesliceOffsetFromRenderPos = btimesliceOffsetFromRenderPos;
	renderPosUpdated = brenderPosUpdated;
	oddInterlaceFrame = boddInterlaceFrame;
	palMode = bpalMode;
	vintHappened = bvintHappened;
	intLineState = bintLineState;
	iplLineState = biplLineState;

	reg.Rollback();
	address = baddress;
	code = bcode;
	writePending = bwritePending;
	dmaPending = bdmaPending;
	if(logStatusRegisterRead || logDataPortRead || logHVCounterRead || logControlPortWrite || logDataPortWrite)
	{
		boost::mutex::scoped_lock lock(portMonitorMutex);
		portMonitorList = bportMonitorList;
	}
}

//----------------------------------------------------------------------------------------
void VDP::ExecuteCommit()
{
	reg.Commit();
	baddress = address;
	bcode = code;
	bwritePending = writePending;
	bdmaPending = dmaPending;
	if(logStatusRegisterRead || logDataPortRead || logHVCounterRead || logControlPortWrite || logDataPortWrite)
	{
		boost::mutex::scoped_lock lock(portMonitorMutex);
		bportMonitorList = portMonitorList;
	}

	//Saved CE line state for Read-Modify-Write cycles
	blineLWRSavedStateRMW = lineLWRSavedStateRMW;
	blineUWRSavedStateRMW = lineUWRSavedStateRMW;
	blineCAS0SavedStateRMW = lineCAS0SavedStateRMW;
	blineRAS0SavedStateRMW = lineRAS0SavedStateRMW;
	blineOE0SavedStateRMW = lineOE0SavedStateRMW;

	//Backup status register data
	bstatusRegister = statusRegister;
	blastTimesliceLength = lastTimesliceLength;
	bstatusRegisterLastAccessTime = statusRegisterLastAccessTime;
	btimesliceOffsetFromRenderPos = timesliceOffsetFromRenderPos;
	brenderPosUpdated = renderPosUpdated;
	boddInterlaceFrame = oddInterlaceFrame;
	bpalMode = palMode;
	bvintHappened = vintHappened;
	bintLineState = intLineState;
	biplLineState = iplLineState;

	//If we're committing a timeslice which has triggered a render operation, pass the
	//render operation on to the render thread.
	if(pendingRenderOperation)
	{
		pendingRenderOperation = false;
		boost::mutex::scoped_lock lock(timesliceMutex);
		regTimeslice = reg.GetLatestTimeslice();
		vramTimeslice = vram->GetLatestTimeslice();
		cramTimeslice = cram->GetLatestTimeslice();
		vsramTimeslice = vsram->GetLatestTimeslice();
		renderThreadUpdate.notify_all();
	}
}

//----------------------------------------------------------------------------------------
//CE line state functions
//----------------------------------------------------------------------------------------
unsigned int VDP::GetCELineID(const std::wstring& lineName, bool inputLine) const
{
	if(lineName == L"LDS")
	{
		return CELINE_LDS;
	}
	else if(lineName == L"UDS")
	{
		return CELINE_UDS;
	}
	else if(lineName == L"R/W")
	{
		return CELINE_RW;
	}
	else if(lineName == L"AS")
	{
		return CELINE_AS;
	}
	else if(lineName == L"RMWCycleInProgress")
	{
		return CELINE_RMWCYCLEINPROGRESS;
	}
	else if(lineName == L"RMWCycleFirstOperation")
	{
		return CELINE_RMWCYCLEFIRSTOPERATION;
	}
	else if(lineName == L"LWR")
	{
		return CELINE_LWR;
	}
	else if(lineName == L"UWR")
	{
		return CELINE_UWR;
	}
	else if(lineName == L"CAS0")
	{
		return CELINE_CAS0;
	}
	else if(lineName == L"RAS0")
	{
		return CELINE_RAS0;
	}
	else if(lineName == L"OE0")
	{
		return CELINE_OE0;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void VDP::SetCELineInput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber)
{
	switch(lineID)
	{
	case CELINE_LDS:
		ceLineMaskLowerDataStrobeInput = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_UDS:
		ceLineMaskUpperDataStrobeInput = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_RW:
		ceLineMaskReadHighWriteLowInput = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_AS:
		ceLineMaskAddressStrobeInput = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_RMWCYCLEINPROGRESS:
		ceLineMaskRMWCycleInProgress = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_RMWCYCLEFIRSTOPERATION:
		ceLineMaskRMWCycleFirstOperation = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	}
}

//----------------------------------------------------------------------------------------
void VDP::SetCELineOutput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber)
{
	switch(lineID)
	{
	case CELINE_LDS:
		ceLineMaskLowerDataStrobeOutput = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_UDS:
		ceLineMaskUpperDataStrobeOutput = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_RW:
		ceLineMaskReadHighWriteLowOutput = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_AS:
		ceLineMaskAddressStrobeOutput = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_LWR:
		ceLineMaskLWR = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_UWR:
		ceLineMaskUWR = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_CAS0:
		ceLineMaskCAS0 = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_RAS0:
		ceLineMaskRAS0 = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_OE0:
		ceLineMaskOE0 = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	}
}

//----------------------------------------------------------------------------------------
unsigned int VDP::CalculateCELineStateMemory(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext, double accessTime) const
{
	bool vdpIsSource = (caller == GetDeviceContext());
	bool currentLowerDataStrobe = (currentCELineState & ceLineMaskLowerDataStrobeInput) != 0;
	bool currentUpperDataStrobe = (currentCELineState & ceLineMaskUpperDataStrobeInput) != 0;
	bool operationIsWrite = (currentCELineState & ceLineMaskReadHighWriteLowInput) == 0;
	bool rmwCycleInProgress = (currentCELineState & ceLineMaskRMWCycleInProgress) != 0;
	bool rmwCycleFirstOperation = (currentCELineState & ceLineMaskRMWCycleFirstOperation) != 0;
	if(vdpIsSource)
	{
		currentLowerDataStrobe = true;
		currentUpperDataStrobe = true;
		operationIsWrite = false;
		rmwCycleInProgress = false;
		rmwCycleFirstOperation = false;
	}
	return BuildCELine(location, vdpIsSource, false, currentLowerDataStrobe, currentUpperDataStrobe, operationIsWrite, rmwCycleInProgress, rmwCycleFirstOperation);
}

//----------------------------------------------------------------------------------------
unsigned int VDP::CalculateCELineStateMemoryTransparent(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext) const
{
	bool vdpIsSource = (caller == GetDeviceContext());
	bool currentLowerDataStrobe = (currentCELineState & ceLineMaskLowerDataStrobeInput) != 0;
	bool currentUpperDataStrobe = (currentCELineState & ceLineMaskUpperDataStrobeInput) != 0;
	bool operationIsWrite = (currentCELineState & ceLineMaskReadHighWriteLowInput) == 0;
	bool rmwCycleInProgress = (currentCELineState & ceLineMaskRMWCycleInProgress) != 0;
	bool rmwCycleFirstOperation = (currentCELineState & ceLineMaskRMWCycleFirstOperation) != 0;
	if(vdpIsSource)
	{
		currentLowerDataStrobe = true;
		currentUpperDataStrobe = true;
		operationIsWrite = false;
		rmwCycleInProgress = false;
		rmwCycleFirstOperation = false;
	}
	return BuildCELine(location, vdpIsSource, true, currentLowerDataStrobe, currentUpperDataStrobe, operationIsWrite, rmwCycleInProgress, rmwCycleFirstOperation);
}

//----------------------------------------------------------------------------------------
unsigned int VDP::BuildCELine(unsigned int targetAddress, bool vdpIsSource, bool transparentAccess, bool currentLowerDataStrobe, bool currentUpperDataStrobe, bool operationIsWrite, bool rmwCycleInProgress, bool rmwCycleFirstOperation) const
{
	//Calculate the state of all the various CE lines
	bool lineLWR = operationIsWrite && currentLowerDataStrobe;
	bool lineUWR = operationIsWrite && currentUpperDataStrobe;
	bool lineCAS0 = !operationIsWrite && ((!vdpIsSource && (targetAddress <= 0xDFFFFF)) || (vdpIsSource && (targetAddress <= 0xFFFFFF)));
	bool lineRAS0 = (!vdpIsSource && (targetAddress >= 0xE00000) && (targetAddress <= 0xFFFFFF)) || (vdpIsSource && (targetAddress >= 0x800000) && (targetAddress <= 0xFFFFFF));
	bool lineOE0 = (!operationIsWrite && !vdpIsSource && (targetAddress >= 0xE00000) && (targetAddress <= 0xFFFFFF)) || (!operationIsWrite && vdpIsSource && (targetAddress <= 0xFFFFFF));

	//If this bus operation is part of a Read-Modify-Write cycle, we need to handle it
	//slightly differently. The VDP asserts its CE lines when the AS line is asserted,
	//indicating that a valid address has been placed on the bus. Based on the state of
	//the address lines and other lines like R/W at that moment, the VDP determines the
	//state of its output CE lines, and asserts them. The VDP does NOT re-evaluate the
	//state of the CE lines over the course of the bus operation however, it simply holds
	//them at the state it determined when AS was first asserted. when AS is negated, the
	//VDP negates its CE lines. When the M68000 is driving the bus and executes a TAS
	//instruction however, this involves an indivisible read-modify-write cycle. A
	//read-modify-write cycle is exactly the same as a read followed by a write, except
	//that AS remains asserted through both the read and write, being asserted at the
	//start of the read, and negated when the write is complete. The effect this has on
	//the VDP is that the CE lines are frozen at the same state they would be for a read
	//throughout both the read and write cycles. In particular, this affects the LWR, UWR,
	//CAS0, and OE0 lines, which use the R/W line state to determine their output. This
	//can result in devices performing a second (ignored) read at the write phase of the
	//TAS instruction, essentially meaning the write phase of the TAS instruction is
	//ignored. This reportedly affects some games on the Sega Mega Drive (Gargoyles and
	//Ex-Mutants), which rely on the write phase of the TAS instruction failing. We
	//emulate this behaviour here, by caching the output CE line state on the read
	//component of a read-modify-write cycle, and retrieving the CE line state on the
	//write component. This is done through the use of two pseudo-CE lines from the
	//M68000, which indicate whether we are performing a read-modify-write cycle, and
	//whether this is the first operation of that cycle, respectively.
	if(rmwCycleInProgress && !transparentAccess)
	{
		if(rmwCycleFirstOperation)
		{
			lineLWRSavedStateRMW = lineLWR;
			lineUWRSavedStateRMW = lineUWR;
			lineCAS0SavedStateRMW = lineCAS0;
			lineRAS0SavedStateRMW = lineRAS0;
			lineOE0SavedStateRMW = lineOE0;
		}
		else
		{
			lineLWR = lineLWRSavedStateRMW;
			lineUWR = lineUWRSavedStateRMW;
			lineCAS0 = lineCAS0SavedStateRMW;
			lineRAS0 = lineRAS0SavedStateRMW;
			lineOE0 = lineOE0SavedStateRMW;
		}
	}

	//Build the actual CE line state based on the asserted CE lines
	unsigned int ceLineState = 0;
	ceLineState |= lineLWR? ceLineMaskLWR: 0x0;
	ceLineState |= lineUWR? ceLineMaskUWR: 0x0;
	ceLineState |= lineCAS0? ceLineMaskCAS0: 0x0;
	ceLineState |= lineRAS0? ceLineMaskRAS0: 0x0;
	ceLineState |= lineOE0? ceLineMaskOE0: 0x0;
	if(vdpIsSource)
	{
		//If the VDP has the bus, we need to drive the M68K CE lines ourselves.
		ceLineState |= ceLineMaskLowerDataStrobeOutput;
		ceLineState |= ceLineMaskUpperDataStrobeOutput;
		ceLineState |= ceLineMaskReadHighWriteLowOutput;
		ceLineState |= ceLineMaskAddressStrobeOutput;
	}

	//Return the generated CE line state
	return ceLineState;
}

//----------------------------------------------------------------------------------------
//Memory interface functions
//----------------------------------------------------------------------------------------
//##TODO## According to the Genesis Technical Bulletin #12, access attempts to the VDP
//may be ignored when the VDP is executing a DMA operation. Trigger a large DMA fill and
//check if writes to the VDP registers or memory are accepted during the operation.
//----------------------------------------------------------------------------------------
IBusInterface::AccessResult VDP::ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	//##NOTE## We've added in the odd address locations to work around issues accessing
	//the VDP from the Z80. Apparently the bus controller turns all 8-bit reads from the
	//Z80 bus into 16-bit reads for the M68000 bus, and selects the upper or lower byte
	//of the result as required. This means that we should be running Z80 access through
	//a proper bus controller and handling the conversion at that point.
	bool odd = (location % 2) != 0;
	location &= ~0x1;
	bool accessMaskUsed = false;
	unsigned int accessMask;
	Data tempData(16);
	if(location == 0x04 || location == 0x06)
	{
		writePending = false;
	//	double initialTime = GetDeviceContext()->GetInitialRemainingTime();
	//	double deviceTime = accessTime;
	//	double currentTime = (renderTimeStep - initialTime) + deviceTime;
	////	RasterPos rasterPos = GetRasterPos(renderPosCurrent, currentTime);
	//	RasterPosNew rasterPos = GetRasterPosNew(renderPosCurrentNew, currentTime);
	//	tempData = GetStatusRegister(rasterPos);

		//Negate the INT line if it's currently asserted
		if(vintHappened)
		{
			intLineState = false;
			memoryBus->SetLineState(LINE_INT, Data(GetLineWidth(LINE_INT), (unsigned int)intLineState), GetDeviceContext(), caller, accessTime, accessContext);
		}

		//New status register read process
		UpdateStatusRegister(accessTime);
		tempData = statusRegister;
		accessMaskUsed = true;
		accessMask = 0x03FF;

		//##DEBUG##
		//std::wcout << L"SR Read:\n";
		//std::wcout << '\t' << "RenderPosCurrent\t" << std::hex << std::uppercase << renderPosCurrentNew.rasterNo << '\t' << renderPosCurrentNew.rasterProgress << '\n';
		//std::wcout << '\t' << "RenderPosNext   \t" << std::hex << std::uppercase << renderPosNextNew.rasterNo << '\t' << renderPosNextNew.rasterProgress << '\n';
		//std::wcout << '\t' << "RasterPos       \t" << std::hex << std::uppercase << rasterPos.rasterNo << '\t' << rasterPos.rasterProgress << '\n';

		//Port monitor logging
		if(logStatusRegisterRead)
		{
			RecordPortMonitorEntry(PortMonitorEntry(L"SR Read", caller->GetTargetDevice()->GetDeviceInstanceName(), tempData.GetData()));
		}
	}
	if(location == 0x00 || location == 0x02)
	{
		tempData = ReadDataPort(accessTime, caller);
	}
	if(location == 0x08 || location == 0x0A || location == 0x0C || location == 0x0E)
	{
//		double initialTime = GetDeviceContext()->GetInitialRemainingTime();
//		double deviceTime = accessTime;
//		double currentTime = (renderTimeStep - initialTime) + deviceTime;
////		RasterPos rasterPos = GetRasterPos(renderPosCurrent, currentTime);
//		RasterPosNew rasterPos = GetRasterPosNew(renderPosCurrentNew, currentTime);
//		tempData = GetHVCounter(rasterPos);
////		std::wcout << "HV Counter Read:\t" << data << '\n';

		//New hcounter read process
		RasterPosNew rasterPos = GetRasterPosNew(renderPosCurrentNew, timesliceOffsetFromRenderPos + accessTime);
		tempData = GetHVCounter(rasterPos, accessTime);

		//Port monitor logging
		if(logHVCounterRead)
		{
			RecordPortMonitorEntry(PortMonitorEntry(L"HV Read", caller->GetTargetDevice()->GetDeviceInstanceName(), tempData.GetData()));
		}
	}
	if((data.GetBitCount() == 8) && !odd)
	{
		data = tempData.GetUpperHalf();
	}
	else
	{
		data = tempData;
	}
	IBusInterface::AccessResult result(true);
	result.accessMaskUsed = accessMaskUsed;
	result.accessMask = accessMask;
	return result;
}

//----------------------------------------------------------------------------------------
IBusInterface::AccessResult VDP::WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	//##TODO## Implement the write FIFO here
	FIFOEntry fifoEntry;
	fifoEntry.data = data.GetData();
	fifoEntry.writeTime = accessTime;
	fifoEntry.caller = caller;
	if(location == 0x04 || location == 0x06)
	{
//##DEBUG##
//		std::wcout << "WriteControlPort:\t" << data << '\t' << accessTime << '\n';
		fifoEntry.writeTarget = fifoEntry.FIFO_WRITECONTROLPORT;
//		WriteControlPort(caller, (unsigned short)data.GetData());
	}
	if(location == 0x00 || location == 0x02)
	{
//##DEBUG##
//		std::wcout << "WriteDataPort:\t" << data << '\t' << accessTime << '\n';
		fifoEntry.writeTarget = fifoEntry.FIFO_WRITEDATAPORT;
//		WriteDataPort(caller, (unsigned short)data.GetData());
	}

	double writeDelay = WriteFIFO(fifoEntry);

	//Notify the calling device of any additional wait time caused by a full FIFO
	return IBusInterface::AccessResult(true, false, 0, false, writeDelay);
//	return true;
}

//----------------------------------------------------------------------------------------
void VDP::UpdateFIFO(double currentTime)
{
	bool done = false;
	while(!fifoBuffer.empty() && !done)
	{
		//Obtain the first buffered write from the front of the FIFO
		FIFOEntry& fifoEntry = fifoBuffer.front();

		double commitTime = GetFIFOCommitTime(fifoEntry.writeTime);

		//If the write would have been committed by now, perform the write.
		if(commitTime < currentTime)
		{
			//##TODO## Modify the port access functions to work in this kind of scenario.
			//We need to be able to pass the current access time independently from the
			//DeviceContext structure.
			if(fifoEntry.writeTarget == fifoEntry.FIFO_WRITECONTROLPORT)
			{
				WriteControlPort((unsigned short)fifoEntry.data, fifoEntry.writeTime, fifoEntry.caller);
			}
			else if(fifoEntry.writeTarget == fifoEntry.FIFO_WRITEDATAPORT)
			{
				WriteDataPort((unsigned short)fifoEntry.data, fifoEntry.writeTime, fifoEntry.caller);
			}

			//Remove the write from the FIFO
			fifoBuffer.pop_front();
		}
		else
		{
			done = true;
		}
	}

	fifoCommittedTime = currentTime;
}

//----------------------------------------------------------------------------------------
double VDP::WriteFIFO(const FIFOEntry& fifoEntry)
{
	double writeDelay = 0;

	//Update the FIFO buffer to ensure that all stored entries in the FIFO which should
	//have been committed by this time, have been committed.
	UpdateFIFO(fifoEntry.writeTime);

	//If the FIFO is already full, calculate the time it will take for the next write to
	//be removed from the buffer. This determines the total additional time the device
	//performing the write will be left waiting for the write to complete.
	if(fifoBuffer.size() >= fifoMaxEntries)
	{
		writeDelay = GetFIFOCommitTime(fifoBuffer.front().writeTime);
	}

	//If this write can be committed immediately, bypass the queue and carry out the
	//operation, otherwise add it to the back of the FIFO buffer.
	if(fifoBuffer.empty() && (GetFIFOCommitTime(fifoEntry.writeTime) == fifoEntry.writeTime))
	{
		//##TODO## Modify the port access functions to work in this kind of scenario.
		//We need to be able to pass the current access time independently from the
		//DeviceContext structure.
		if(fifoEntry.writeTarget == fifoEntry.FIFO_WRITECONTROLPORT)
		{
			WriteControlPort((unsigned short)fifoEntry.data, fifoEntry.writeTime, fifoEntry.caller);
		}
		else if(fifoEntry.writeTarget == fifoEntry.FIFO_WRITEDATAPORT)
		{
			WriteDataPort((unsigned short)fifoEntry.data, fifoEntry.writeTime, fifoEntry.caller);
		}
	}
	else
	{
		fifoBuffer.push_back(fifoEntry);
	}

	return writeDelay;
}

//----------------------------------------------------------------------------------------
//##TODO## Implement this function
double VDP::GetFIFOCommitTime(double writeTime) const
{
	double commitTime = writeTime;

	////Assumed constants
	//const unsigned int blockHeight = 8;
	//const unsigned int blockWidth = 8;

	////##TODO## Read all relevant VDP state data which could affect FIFO timing, at the time
	////this write occurred.
	//bool displayEnabled;
	//unsigned int cellWidth;
	//unsigned int cellHeight;

	/////////////////////////////////////////////////////////////////////////////////////////
	////      lines  lines   lines   bottom bottom   vertical top      top    vsync hsync
	////      screen display retrace border blanking blanking blanking border
	////NTSC  262    224(28) 38      8      3        3        13       11     60Hz  15720Hz
	////PAL   312    224(28) 88      32     3        3        13       38     50Hz  15600Hz
	////PAL   312    240(30) 72      24     3        3        13       30     50Hz  15600Hz
	/////////////////////////////////////////////////////////////////////////////////////////
	////##TODO## Calculate these values based on the current display settings
	//unsigned int visibleCellHeight = cellHeight;
	//unsigned int rasterCountScreen = 312;
	//unsigned int refreshRate = 50;
	//unsigned int rasterCountTopBorder = ((34 - visibleCellHeight) / 2) * blockHeight;
	//unsigned int rasterCountFrame = visibleCellHeight * blockHeight;
	//unsigned int rasterCountBottomBorder = ((34 - visibleCellHeight) / 2) * blockHeight;
	//unsigned int rasterCountRetrace = rasterCountScreen - (rasterCountTopBorder + rasterCountFrame + rasterCountBottomBorder);
	//unsigned int rasterPosFrame = rasterCountTopBorder;
	//unsigned int rasterPosBottomBorder = rasterCountTopBorder + rasterCountFrame;
	//unsigned int rasterPosRetrace = rasterCountTopBorder + rasterCountFrame + rasterCountBottomBorder;

	////##TODO## Check our use of the current raster pos here
	//RasterPosNew writeRasterPos = GetRasterPosNew(renderPosCurrentNew, writeTime);

	////##TODO## Calculate the time at which this buffered write would be written, based on
	////the current VDP state.
	//if(displayEnabled)
	//{
	//}

	return commitTime;
}

//----------------------------------------------------------------------------------------
void VDP::UpdateStatusRegister(double timesliceProgress)
{
	//-------------------------------------------------------------
	//|  9  |  8  |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
	//|-----------------------------------------------------------|
	//|EMPT |FULL |  F  |SOVR | COL | ODD | VB  | HB  | DMA | PAL |
	//-------------------------------------------------------------
	//EMPT - FIFO Empty
	// FIFO empty flag. When there are no pending writes held in the 4-word FIFO, this
	// bit is set, otherwise it is clear.
	//FULL - FIFO Full
	// FIFO full flag. When there are 4 pending writes held in the 4-word FIFO, this bit
	// is set, otherwise it is clear.
	//F - VInt occurred
	//SOVR - Sprite Overflow
	//COL - Sprite Collision
	//VB - During VBlank
	//HB - During HBlank
	//##TODO## Add a longer description of what each bit is called
	//http://www.smspower.org/dev/docs/wiki/VDP

	//##TODO## Implement a write FIFO, and use the saved write times to build the correct
	//contents of these flags.
	statusRegister.SetBit(9, true);
	statusRegister.SetBit(8, false);

	//##TODO## Figure out what this status register bit even means
	//This bit is set when a vint has just occurred, and is cleared when it is read. This
	//flag was created for the SMS, where the main CPU is a Z80 with only one INT line.
	//Since the VDP couldn't drive the more complex vector modes of the Z80, the Z80
	//didn't know whether the INT was due to hint or vint. This bitflag in the status
	//register allowed games which used hint to differentiate between hint and vint.
	statusRegister.SetBit(7, vintHappened);
	//Clear the vint happened flag now that it's been read
	vintHappened = false;

	//##TODO## Implement functions to calculate these flags from the period between the
	//last access time and the time this access is being made.
	//Note that these flags are calculated on the fly based on the current hcounter. The
	//sprite plane takes the xpos of each sprite, and uses it as a counter which is
	//decremented on each hcounter increment. This means the evaluation does occur
	//mid-line, and this is supported by SMS notes.
	statusRegister.SetBit(6, false);
	statusRegister.SetBit(5, false);

	//##TODO## Add a render status flag which indicates when we're on an odd line in
	//interlace mode, and use that flag to build this flag.
	statusRegister.SetBit(4, oddInterlaceFrame);

	//##TODO## Use the timesliceProgress to build a RasterPos here, and calculate the
	//correct values for these bits from that position. Refer to the old implementation
	//of the VDP status register for our definition of these bits.
	//##TODO## Accurately implement these flags. We need to determine precisely which
	//point to enable these flags, and when to disable them. Run a series of tests on the
	//hardware to determine the correct timing.
	RasterPosNew rasterPos = GetRasterPosNew(renderPosCurrentNew, timesliceOffsetFromRenderPos + timesliceProgress);
	bool vblank = (rasterPos.rasterNo < (3 * 8)) || (rasterPos.rasterNo >= ((3 * 8) + 224));
	bool hblank = (rasterPos.rasterProgress > 0.8);
	statusRegister.SetBit(3, vblank);
	statusRegister.SetBit(2, hblank);

	//##TODO## When a DMA operation is initiated, calculate the time it will finish, and
	//save it to a register. Adjust this saved register when each new timeslice is added.
	//Compare the DMA completion time with the timesliceProgress, and use it to determine
	//the correct value for this bit.
	statusRegister.SetBit(1, false);

	//##TODO## Add some kind of dipswitch/setting for the VDP which allows the PAL line
	//to the VDP to be controlled. Right now, we're just fixing the PAL bit here to set.
	statusRegister.SetBit(0, palMode);

	//Update the last access time for the status register
	statusRegisterLastAccessTime = timesliceProgress;
}

//----------------------------------------------------------------------------------------
//Render functions
//----------------------------------------------------------------------------------------
//void VDP::RenderThread()
//{
//	///////////////////////////////////////////////////////
//	//      lines   lines   lines   vcells  vsync   hsync
//	//      screen  display retrace
//	//NTSC	262     224     38      28      60Hz    15720Hz
//	//PAL	312     224     88      28      50Hz    15600Hz
//	//PAL	312     240     72      30      50Hz    15600Hz
//	///////////////////////////////////////////////////////
//	const unsigned int linesScreen = 312;
//	const unsigned int linesRetrace = 88;
//	const unsigned int finalLine = linesScreen - linesRetrace;
//	const unsigned int refreshRate = 50;
//	double frameRenderTime = 1000000000.0 / refreshRate;
//	double lineRenderTime = frameRenderTime / linesScreen;
//	double vblankRenderTime = lineRenderTime * linesRetrace;
//
//	boost::mutex::scoped_lock lock(renderThreadMutex);
//	renderThreadUpdate.wait(lock);
//	while(renderThreadActive)
//	{
//		//Take copies of the target timeslices for rendering. We do this so we don't
//		//have to keep an exclusive lock open on the timeslices as we render, which
//		//prevents the render thread from locking the rest of the system.
//		RandomTimeAccessBuffer<unsigned char, double>::Timeslice regTimesliceCopy;
//		RandomTimeAccessBuffer<unsigned char, double>::Timeslice vramTimesliceCopy;
//		RandomTimeAccessBuffer<unsigned char, double>::Timeslice cramTimesliceCopy;
//		RandomTimeAccessBuffer<unsigned char, double>::Timeslice vsramTimesliceCopy;
//		{
//			boost::mutex::scoped_lock lock(timesliceMutex);
//			regTimesliceCopy = regTimeslice;
//			vramTimesliceCopy = vramTimeslice;
//			cramTimesliceCopy = cramTimeslice;
//			vsramTimesliceCopy = vsramTimeslice;
//		}
//
//		reg.AdvanceByTime(vblankRenderTime, regTimesliceCopy);
//		vram->AdvanceByTime(vblankRenderTime, vramTimesliceCopy);
//		cram->AdvanceByTime(vblankRenderTime, cramTimesliceCopy);
//		vsram->AdvanceByTime(vblankRenderTime, vsramTimesliceCopy);
//
//		//Reset the sprite masking mode 2 flag. Undocumented sprite masking mode. See
//		//genvdp.txt from Charles MacDonald for further info.
//		spriteMaskingMode2Active = false;
//		for(unsigned int i = 0; i < finalLine; ++i)
//		{
//			RenderLine(i);
//			reg.AdvanceByTime(lineRenderTime, regTimesliceCopy);
//			vram->AdvanceByTime(lineRenderTime, vramTimesliceCopy);
//			cram->AdvanceByTime(lineRenderTime, cramTimesliceCopy);
//			vsram->AdvanceByTime(lineRenderTime, vsramTimesliceCopy);
//		}
//
//		//Obtain the latest target timeslices, and advance to them. We use the latest
//		//timeslice values here rather than the copies we took above, so that if the
//		//rendering thread is running behind the rest of the system, and another frame or
//		//two has been sent since we started rendering this one, the next frame we render
//		//will start from the latest data. If we advanced to the copies we took, we would
//		//double the latency between the rendering process and the actual system state in
//		//cases where rendering is running behind.
//		{
//			boost::mutex::scoped_lock lock(timesliceMutex);
//			reg.AdvancePastTimeslice(regTimeslice);
//			vram->AdvancePastTimeslice(vramTimeslice);
//			cram->AdvancePastTimeslice(cramTimeslice);
//			vsram->AdvancePastTimeslice(vsramTimeslice);
//		}
//		if(!renderThreadActive)
//		{
//			continue;
//		}
//		renderThreadUpdate.wait(lock);
//	}
//	renderThreadStopped.notify_all();
//}

//----------------------------------------------------------------------------------------
void VDP::RenderThread()
{
	//Assumed constants
	const unsigned int blockHeight = 8;

	///////////////////////////////////////////////////////
	//      lines   lines   lines   vcells  vsync   hsync
	//      screen  display retrace
	//NTSC	262     224     38      28      60Hz    15720Hz
	//PAL	312     224     88      28      50Hz    15600Hz
	//PAL	312     240     72      30      50Hz    15600Hz
	///////////////////////////////////////////////////////
	//##TODO## Calculate these values based on the current display settings
	unsigned int visibleCellHeight = GetVisibleCellHeight();
	unsigned int rasterCountScreen = 312;
	unsigned int refreshRate = 50;
	unsigned int rasterCountTopBorder = ((34 - visibleCellHeight) / 2) * blockHeight;
	unsigned int rasterCountFrame = visibleCellHeight * blockHeight;
	unsigned int rasterCountBottomBorder = ((34 - visibleCellHeight) / 2) * blockHeight;
	unsigned int rasterCountRetrace = rasterCountScreen - (rasterCountTopBorder + rasterCountFrame + rasterCountBottomBorder);
	unsigned int rasterPosFrame = rasterCountTopBorder;
	unsigned int rasterPosBottomBorder = rasterCountTopBorder + rasterCountFrame;
	unsigned int rasterPosRetrace = rasterCountTopBorder + rasterCountFrame + rasterCountBottomBorder;

	boost::mutex::scoped_lock lock(renderThreadMutex);
	//We need this doublecheck for renderThreadActive here before the wait, as we were
	//getting deadlocks in the case where the system is single-stepped. Without this
	//test, it's possible for the parent thread to signal a system stop before this
	//thread has even started executing. Now that we've obtained the lock above, we are
	//in a consistent state, and we simply need to check that the render thread is still
	//marked as active before we start our wait cycle for updates.
	if(!renderThreadActive)
	{
		renderThreadStopped.notify_all();
		return;
	}

	//Start the render loop
	renderThreadUpdate.wait(lock);
//	clock_t executionTimeStart = clock();
	while(renderThreadActive)
	{
		//Take copies of the target timeslices for rendering. We do this so we don't
		//have to keep an exclusive lock open on the timeslices as we render, which
		//prevents the render thread from locking the rest of the system.
		{
			boost::mutex::scoped_lock lock(timesliceMutex);
			regTimesliceCopy = regTimeslice;
			vramTimesliceCopy = vramTimeslice;
			cramTimesliceCopy = cramTimeslice;
			vsramTimesliceCopy = vsramTimeslice;
		}

		RasterPosNew renderPos(rasterCountScreen, refreshRate, 0);
		double lineRenderTime = renderPos.GetRasterRenderTime();

		//Render the top border
		for(unsigned int i = 0; i < rasterCountTopBorder; ++i)
		{
			RenderLine(renderPos.rasterNo);
			renderPos.AddRasterNo(1);
			reg.AdvanceByTime(lineRenderTime, regTimesliceCopy);
			if(!vramSync)
			{
				vram->AdvanceByTime(lineRenderTime, vramTimesliceCopy);
			}
			if(!cramSync)
			{
				cram->AdvanceByTime(lineRenderTime, cramTimesliceCopy);
			}
			if(!vsramSync)
			{
				vsram->AdvanceByTime(lineRenderTime, vsramTimesliceCopy);
			}
		}

		//Reset the sprite masking mode 2 flag. This sprite masking mode is undocumented.
		//See genvdp.txt by Charles MacDonald for further info.
		spriteMaskingMode2Active = false;

		//Render the active display
		for(unsigned int i = 0; i < rasterCountFrame; ++i)
		{
			RenderLine(renderPos.rasterNo);
			renderPos.AddRasterNo(1);
			reg.AdvanceByTime(lineRenderTime, regTimesliceCopy);
			if(!vramSync)
			{
				vram->AdvanceByTime(lineRenderTime, vramTimesliceCopy);
			}
			if(!cramSync)
			{
				cram->AdvanceByTime(lineRenderTime, cramTimesliceCopy);
			}
			if(!vsramSync)
			{
				vsram->AdvanceByTime(lineRenderTime, vsramTimesliceCopy);
			}
		}

		//Render the bottom border
		for(unsigned int i = 0; i < rasterCountBottomBorder; ++i)
		{
			RenderLine(renderPos.rasterNo);
			renderPos.AddRasterNo(1);
			reg.AdvanceByTime(lineRenderTime, regTimesliceCopy);
			if(!vramSync)
			{
				vram->AdvanceByTime(lineRenderTime, vramTimesliceCopy);
			}
			if(!cramSync)
			{
				cram->AdvanceByTime(lineRenderTime, cramTimesliceCopy);
			}
			if(!vsramSync)
			{
				vsram->AdvanceByTime(lineRenderTime, vsramTimesliceCopy);
			}
		}

		//Advance to a new image buffer
		{
			boost::mutex::scoped_lock imageBufferLock(imageBufferMutex);
			drawingImageBufferPlane = (drawingImageBufferPlane + 1) % imageBufferPlanes;
		}

		//Obtain the latest target timeslices, and advance to them. We use the latest
		//timeslice values here rather than the copies we took above, so that if the
		//rendering thread is running behind the rest of the system, and another frame or
		//two has been sent since we started rendering this one, the next frame we render
		//will start from the latest data. If we advanced to the copies we took, we would
		//double the latency between the rendering process and the actual system state in
		//cases where rendering is running behind.
		{
			boost::mutex::scoped_lock lock(timesliceMutex);
			reg.AdvancePastTimeslice(regTimeslice);
			vram->AdvancePastTimeslice(vramTimeslice);
			cram->AdvancePastTimeslice(cramTimeslice);
			vsram->AdvancePastTimeslice(vsramTimeslice);
		}
		if(!renderThreadActive)
		{
			continue;
		}

//		clock_t executionTimeEnd = clock();
//		clock_t executionTimeRealEnd = executionTimeEnd;
//		double executionTimeInNanoseconds = (double)(executionTimeEnd - executionTimeStart) * ((double)1000000000.0/CLOCKS_PER_SEC);
//		double remainingTimeInNanoseconds = (1000000000.0/(double)refreshRate) - executionTimeInNanoseconds;
//		clock_t targetExecutionTimeInTicks = (clock_t)(((double)CLOCKS_PER_SEC / (double)refreshRate) + 0.5);
//		if(remainingTimeInNanoseconds > 2000000.0)
//		{
//			Sleep((DWORD)((remainingTimeInNanoseconds - 1000000.0) / 1000000.0));
//		}
//		while((executionTimeEnd - executionTimeStart) < targetExecutionTimeInTicks)
//		{
//			executionTimeEnd = clock();
//		}
		//##DEBUG##
//		std::wcout << std::setprecision(16) << '\t' << targetExecutionTimeInTicks << '\t' << executionTimeStart << '\t' << executionTimeEnd << '\t' << executionTimeEnd - executionTimeStart << '\t' << executionTimeRealEnd << '\t' << executionTimeRealEnd - executionTimeStart << '\n';
//		executionTimeStart = executionTimeEnd;

		renderThreadUpdate.wait(lock);
	}
	renderThreadStopped.notify_all();
}

//----------------------------------------------------------------------------------------
void VDP::ProcessVInt()
{
	if(GetVInterruptEnabled(GetCurrentTimesliceProgress()))
	{
		//Set the vint happened flag for the status register
		vintHappened = true;
	//	SetVInterruptHappened(GetVInterruptEnabled(GetCurrentTimesliceProgress()));

		//Trigger VInt for M68000
		iplLineState = vintIPLLineState;
		memoryBus->SetLineState(LINE_IPL, Data(GetLineWidth(LINE_IPL), iplLineState), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), ACCESSCONTEXT_VINT);
	}

	//##TODO## Confirm whether this should be triggered on every frame, or just when VInt
	//is enabled. According to gen-hw.txt, the Z80 always receives an interrupt,
	//regardless of the VINT enable status of the VDP. This is possible, since the Z80
	//has its own INT line from the VDP.
	//##TODO## According to genvdp.txt, in the case of a VINT from the VDP to the Z80, it
	//will be missed rather than being made pending if interrupts are masked at the time
	//it occurs. In other words, the INT line does not remain asserted for very long. We
	//know that a VINT to the M68000 is cleared when the control port is read. Run some
	//tests to determine the conditions under which the Z80 interrupt line is asserted
	//and cleared.

	//Trigger VInt for Z80
	intLineState = true;
	memoryBus->SetLineState(LINE_INT, Data(GetLineWidth(LINE_INT), (unsigned int)intLineState), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), ACCESSCONTEXT_INT);

	//Set the odd interlace frame flag for the status register
	if(GetInterlaceMode() != INTERLACE_NONE)
	{
		oddInterlaceFrame = !oddInterlaceFrame;
//		SetOddInterlaceFrame(!GetOddInterlaceFrame());
	}
	else
	{
		oddInterlaceFrame = false;
//		SetOddInterlaceFrame(false);
	}
}

//----------------------------------------------------------------------------------------
void VDP::ProcessHInt()
{
	if(GetHInterruptEnabled(GetCurrentTimesliceProgress()))
	{
		iplLineState = hintIPLLineState;
		memoryBus->SetLineState(LINE_IPL, Data(GetLineWidth(LINE_IPL), iplLineState), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), ACCESSCONTEXT_HINT);
	}
	//##DEBUG##
	else
	{
		std::wcout << "Hint Disabled\n";
	}
}

//----------------------------------------------------------------------------------------
void VDP::ProcessEInt()
{
	//##TODO## Implement external interrupts
	SetHVCounterStop(true);
}

//----------------------------------------------------------------------------------------
//double VDP::RenderSegment(const RasterPos& startPos, const RasterPos& endPos)
//{
//	//Assumed constants
//	const unsigned int finalLine = 224;
//	const unsigned int blockWidth = 8;
//
//	//##TODO## Calculate proper values for these render times
//	double lineRenderTime = (11.0 / 3430.0) * (1000000000 / 50);
//	double hblankRenderTime = (2.0 / 3430.0) * (1000000000 / 50);
//	double vblankRenderTime = (385.0 / 3430.0) * (1000000000 / 50);
//	double pixelRenderTime = (lineRenderTime - hblankRenderTime) / (GetVisibleCellWidth() * blockWidth);
//
//	unsigned int pixelsToRender = 0;
//	unsigned int linesToRender = 0;
//	unsigned int hblankToRender = 0;
//	unsigned int vblankToRender = 0;
//	while(startPos != endPos)
//	{
//		if(startPos.hblank)
//		{
//			//We're rendering hblank
//			startPos.hblank = false;
//			startPos.pixel = 0;
//			++startPos.line;
//			++hblankToRender;
//		}
//		else if(startPos.vblank)
//		{
//			//We're rendering vblank
//			startPos.vblank = false;
//			startPos.line = 0;
//			startPos.pixel = 0;
//			++vblankToRender;
//		}
//		else if((startPos.line == endPos.line) && (startPos.pixel < endPos.pixel))
//		{
//			//We're rendering a block of cells up to the endPos
//			while(startPos.pixel < endPos.pixel)
//			{
//				if((startPos.pixel % blockWidth) == 0)
//				{
////					RenderCell(startPos.line, startPos.pixel / blockWidth);
//				}
//				++startPos.pixel;
//				++pixelsToRender;
//			}
//			if(startPos.pixel >= (GetVisibleCellWidth() * blockWidth))
//			{
//				//We've hit hblank
//				startPos.hblank = true;
//			}
//		}
//		else if((startPos.line == endPos.line) && (startPos.pixel > endPos.pixel))
//		{
//			//We're rendering a block of cells past the endPos
//			pixelsToRender += GetVisibleCellWidth() - startPos.pixel;
//			++hblankToRender;
//			startPos.pixel = 0;
//		}
//		else
//		{
//			//We're rendering a complete line
////			RenderLine(startPos.line);
//			++linesToRender;
//			++startPos.line;
//			if(startPos.line >= finalLine)
//			{
//				//We've hit vblank
//				startPos.vblank = true;
//			}
//		}
//	}
//
//	//Calculate the amount of time required to render the selected region
//	double nextTimingPoint = 0;
//	nextTimingPoint += ((double)linesToRender * lineRenderTime);
//	nextTimingPoint += ((double)pixelsToRender * pixelRenderTime);
//	nextTimingPoint += ((double)hblankToRender * hblankRenderTime);
//	nextTimingPoint += ((double)vblankToRender * vblankRenderTime);
//	return nextTimingPoint;
//}

////----------------------------------------------------------------------------------------
//double VDP::GetRenderTime(const RasterPos& startPos, const RasterPos& endPos) const
//{
//	//Assumed constants
//	const unsigned int blockWidth = 8;
//
//	///////////////////////////////////////////////////////
//	//      lines   lines   lines   vcells  vsync   hsync
//	//      screen  display retrace
//	//NTSC	262     224     38      28      60Hz    15720Hz
//	//PAL	312     224     88      28      50Hz    15600Hz
//	//PAL	312     240     72      30      50Hz    15600Hz
//	///////////////////////////////////////////////////////
//	const unsigned int linesScreen = 312;
//	const unsigned int linesRetrace = 88;
//	const unsigned int finalLine = linesScreen - linesRetrace;
//	const unsigned int refreshRate = 50;
//	double frameRenderTime = 1000000000.0 / refreshRate;
//	double lineRenderTime = frameRenderTime / linesScreen;
//	double vblankRenderTime = lineRenderTime * linesRetrace;
//	double hblankRenderTime = lineRenderTime / 10;
//	double pixelRenderTime = (lineRenderTime - hblankRenderTime) / (GetVisibleCellWidth() * blockWidth);
//	//##DEBUG##
////	std::wcout << frameRenderTime << '\t' << lineRenderTime << '\t' << vblankRenderTime << '\t' << hblankRenderTime << '\t' << pixelRenderTime << '\n';
//
//	unsigned int pixelsToRender = 0;
//	unsigned int linesToRender = 0;
//	unsigned int hblankToRender = 0;
//	unsigned int vblankToRender = 0;
//	//##DEBUG##
//	unsigned int cycleCount = 0;
//	while(startPos != endPos)
//	{
//		//##DEBUG##
//		++cycleCount;
//		if(cycleCount > 1000)
//		{
//			std::wcout << L"Blocking loop detected in GetRenderTime\n";
//			std::wcout << startPos.line << '\t' << startPos.pixel << '\t' << startPos.vblank << '\t' << startPos.hblank << '\n';
//			std::wcout << endPos.line << '\t' << endPos.pixel << '\t' << endPos.vblank << '\t' << endPos.hblank << '\n';
//		}
//		if(startPos.hblank)
//		{
//			//We're rendering hblank
//			startPos.hblank = false;
//			startPos.pixel = 0;
//			++startPos.line;
//			++hblankToRender;
//		}
//		else if(startPos.vblank)
//		{
//			//We're rendering vblank
//			startPos.vblank = false;
//			startPos.line = 0;
//			startPos.pixel = 0;
//			++vblankToRender;
//		}
//		else if((startPos.line == endPos.line) && (startPos.pixel < endPos.pixel))
//		{
//			//We're rendering a block of cells up to the endPos
//			pixelsToRender += endPos.pixel - startPos.pixel;
//			startPos.pixel = endPos.pixel;
//			startPos.hblank = endPos.hblank;
////			if(startPos.pixel >= (GetVisibleCellWidth() * blockWidth))
////			{
////				//We've hit hblank
////				startPos.hblank = true;
////			}
//		}
//		else if((startPos.line == endPos.line) && (startPos.pixel > endPos.pixel))
//		{
//			//We're rendering a block of cells past the endPos
//			pixelsToRender += (GetVisibleCellWidth() * blockWidth) - startPos.pixel;
//			startPos.hblank = true;
//		}
//		else
//		{
//			//We're rendering a complete line
//			++linesToRender;
//			++startPos.line;
//			if(startPos.line >= finalLine)
//			{
//				//We've hit vblank
//				startPos.vblank = true;
//			}
//		}
//	}
//
//	//##DEBUG##
////	std::wcout << linesToRender << '\t' << pixelsToRender << '\t' << hblankToRender << '\t' << vblankToRender << '\n';
//
//	//Calculate the amount of time required to render the selected region
//	double nextTimingPoint = 0;
//	nextTimingPoint += ((double)linesToRender * lineRenderTime);
//	nextTimingPoint += ((double)pixelsToRender * pixelRenderTime);
//	nextTimingPoint += ((double)hblankToRender * hblankRenderTime);
//	nextTimingPoint += ((double)vblankToRender * vblankRenderTime);
//	return nextTimingPoint;
//}
//
////----------------------------------------------------------------------------------------
//VDP::RasterPos VDP::GetRasterPos(const RasterPos& startPos, double time) const
//{
//	//Assumed constants
//	const unsigned int blockWidth = 8;
//
//	/////////////////////////////////////////////////////////
//	//      lines   lines   lines   vcells  vsync   hsync
//	//      screen  display vblank
//	//NTSC	262     224     38      28      60Hz    15720Hz
//	//PAL	312     224     88      28      50Hz    15600Hz
//	//PAL	312     240     72      30      50Hz    15600Hz
//	///////////////////////////////////////////////////////
//	const unsigned int linesScreen = 312;
//	const unsigned int linesRetrace = 88;
//	const unsigned int finalLine = linesScreen - linesRetrace;
//	const unsigned int refreshRate = 50;
//	double frameRenderTime = 1000000000.0 / refreshRate;
//	double lineRenderTime = frameRenderTime / linesScreen;
//	double vblankRenderTime = lineRenderTime * linesRetrace;
//	double hblankRenderTime = lineRenderTime / 10;
//	double pixelRenderTime = (lineRenderTime - hblankRenderTime) / (GetVisibleCellWidth() * blockWidth);
//
//	RasterPos rasterPos = startPos;
//
//	while(time > 0)
//	{
//		if(rasterPos.hblank)
//		{
//			time -= hblankRenderTime;
//			if(time > 0)
//			{
//				rasterPos.pixel = 0;
//				rasterPos.hblank = false;
//				++rasterPos.line;
//			}
//		}
//		if(rasterPos.vblank)
//		{
//			time -= vblankRenderTime;
//			if(time > 0)
//			{
//				rasterPos.line = 0;
//				rasterPos.vblank = false;
//			}
//		}
//		if((rasterPos.pixel > 0) || (time < lineRenderTime))
//		{
//			//Add partial cells to the rasterPos
//			while((time > 0) && !rasterPos.hblank && !rasterPos.vblank)
//			{
//				if(rasterPos.pixel >= (GetVisibleCellWidth() * blockWidth))
//				{
//					rasterPos.hblank = true;
//				}
//				else
//				{
//					time -= pixelRenderTime;
//					if(time > 0)
//					{
//						++rasterPos.pixel;
//					}
//				}
//			}
//		}
//		else
//		{
//			//Add complete lines to the rasterPos
//			while((time > lineRenderTime) && !rasterPos.hblank && !rasterPos.vblank)
//			{
//				if(rasterPos.line >= finalLine)
//				{
//					rasterPos.vblank = true;
//				}
//				else
//				{
//					time -= lineRenderTime;
//					if(time > 0)
//					{
//						++rasterPos.line;
//					}
//				}
//			}
//		}
//	}
//
//	return rasterPos;
//}

//----------------------------------------------------------------------------------------
double VDP::GetRenderTimeNew(const RasterPosNew& startPos, const RasterPosNew& endPos) const
{
	///////////////////////////////////////////////////////
	//      lines   lines   lines   vcells  vsync   hsync
	//      screen  display retrace
	//NTSC	262     224     38      28      60Hz    15720Hz
	//PAL	312     224     88      28      50Hz    15600Hz
	//PAL	312     240     72      30      50Hz    15600Hz
	///////////////////////////////////////////////////////
	double frameRenderTime = startPos.GetFrameRenderTime();
	double lineRenderTime = startPos.GetRasterRenderTime();

	RasterPosNew currentPos(startPos);
	double linesToRender = 0.0;
	while(currentPos != endPos)
	{
		if((currentPos.rasterNo == endPos.rasterNo) && (currentPos.rasterProgress < endPos.rasterProgress))
		{
			//We're rendering a segment of a raster line up to the endPos
			linesToRender += (endPos.rasterProgress - currentPos.rasterProgress);
			currentPos.rasterProgress = endPos.rasterProgress;
		}
		else if(currentPos.rasterProgress > 0)
		{
			//We're rendering part of a line
			linesToRender += (1.0 - currentPos.rasterProgress);
			currentPos.rasterProgress = 0;
			currentPos.AddRasterNo(1);
		}
		else
		{
			//We're rendering a complete line
			linesToRender += 1.0;
			currentPos.AddRasterNo(1);
		}
	}

	//Calculate the amount of time required to render the selected region
	return linesToRender * lineRenderTime;
}

//----------------------------------------------------------------------------------------
VDP::RasterPosNew VDP::GetRasterPosNew(const RasterPosNew& startPos, double time) const
{
	/////////////////////////////////////////////////////////
	//      lines   lines   lines   vcells  vsync   hsync
	//      screen  display vblank
	//NTSC	262     224     38      28      60Hz    15720Hz
	//PAL	312     224     88      28      50Hz    15600Hz
	//PAL	312     240     72      30      50Hz    15600Hz
	///////////////////////////////////////////////////////
	double frameRenderTime = startPos.GetFrameRenderTime();
	double lineRenderTime = startPos.GetRasterRenderTime();

	RasterPosNew currentPos(startPos);
	while(time > 0)
	{
		if((currentPos.rasterProgress > 0) || (time < lineRenderTime))
		{
			if((lineRenderTime * (1.0 - currentPos.rasterProgress)) > time)
			{
				//If there isn't enough time to complete the line, advance through the
				//line to the new position.
				currentPos.rasterProgress += (time / lineRenderTime);
				time = 0;
			}
			else
			{
				//If we have enough time to finish the line, jump to the end.
				time -= (lineRenderTime * (1.0 - currentPos.rasterProgress));
				currentPos.rasterProgress = 0;
				currentPos.AddRasterNo(1);
			}
		}
		else while(time > lineRenderTime)
		{
			//Add complete lines to the currentPos
			time -= lineRenderTime;
			currentPos.AddRasterNo(1);
		}
	}

	return currentPos;
}

//----------------------------------------------------------------------------------------
//Savestate functions
//----------------------------------------------------------------------------------------
bool VDP::GetScreenshot(IImage& targetImage) const
{
	//Assumed constants
	unsigned int width = 320;
	unsigned int height = 272;

	boost::mutex::scoped_lock lock(imageBufferMutex);
	unsigned int displayingImageBufferPlane = ((drawingImageBufferPlane + imageBufferPlanes) - 1) % imageBufferPlanes;

	targetImage.SetImageFormat(width, height, IImage::PIXELFORMAT_RGB, IImage::DATAFORMAT_8BIT);
	for(unsigned int ypos = 0; ypos < height; ++ypos)
	{
		for(unsigned int xpos = 0; xpos < width; ++xpos)
		{
			unsigned int bufferPos = ((xpos + (((height - 1) * imageWidth) - (ypos * imageWidth))) * 4);
			unsigned char r = image[displayingImageBufferPlane][bufferPos + 0];
			unsigned char g = image[displayingImageBufferPlane][bufferPos + 1];
			unsigned char b = image[displayingImageBufferPlane][bufferPos + 2];
			targetImage.WritePixelData(xpos, ypos, 0, r);
			targetImage.WritePixelData(xpos, ypos, 1, g);
			targetImage.WritePixelData(xpos, ypos, 2, b);
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------
void VDP::LoadState(IHeirarchicalStorageNode& node)
{
	std::list<IHeirarchicalStorageNode*> childList = node.GetChildList();
	for(std::list<IHeirarchicalStorageNode*>::iterator i = childList.begin(); i != childList.end(); ++i)
	{
		if((*i)->GetName() == L"Registers")
		{
			reg.LoadState(*(*i));
		}
		else if((*i)->GetName() == L"Register")
		{
			IHeirarchicalStorageAttribute* nameAttribute = (*i)->GetAttribute(L"name");
			if(nameAttribute != 0)
			{
				std::wstring registerName = nameAttribute->GetValue();
				if(registerName == L"Address")				address = (unsigned short)(*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"Code")			code = (Code)(*i)->ExtractData<unsigned int>();
				else if(registerName == L"WritePending")	writePending = (*i)->ExtractData<bool>();
				else if(registerName == L"DMAPending")		dmaPending = (*i)->ExtractData<bool>();
				else if(registerName == L"NextState")		nextState = (State)(*i)->ExtractData<unsigned int>();
				else if(registerName == L"HIntCounter")		hintCounter = (*i)->ExtractData<unsigned int>();
				//else if(registerName == L"RenderPosCurrent")
				//{
				//	renderPosCurrent.line = (*i)->GetAttribute(L"Line")->ExtractValue<unsigned int>();
				//	renderPosCurrent.pixel = (*i)->GetAttribute(L"Pixel")->ExtractValue<unsigned int>();
				//	renderPosCurrent.hblank = (*i)->GetAttribute(L"HBlank")->ExtractValue<bool>();
				//	renderPosCurrent.vblank = (*i)->GetAttribute(L"VBlank")->ExtractValue<bool>();
				//}
				//else if(registerName == L"RenderPosNext")
				//{
				//	renderPosNext.line = (*i)->GetAttribute(L"Line")->ExtractValue<unsigned int>();
				//	renderPosNext.pixel = (*i)->GetAttribute(L"Pixel")->ExtractValue<unsigned int>();
				//	renderPosNext.hblank = (*i)->GetAttribute(L"HBlank")->ExtractValue<bool>();
				//	renderPosNext.vblank = (*i)->GetAttribute(L"VBlank")->ExtractValue<bool>();
				//}
			}
		}
	}
}

//----------------------------------------------------------------------------------------
void VDP::SaveState(IHeirarchicalStorageNode& node) const
{
	IHeirarchicalStorageNode& regNode = node.CreateChild(L"Registers");
	std::wstring regBufferName = GetFullyQualifiedDeviceInstanceName();
	regBufferName += L".Registers";
	reg.SaveState(regNode, regBufferName, false);

	node.CreateChildHex(L"Register", address, 4).CreateAttribute(L"name", L"Address");
	node.CreateChild(L"Register", (unsigned int)code).CreateAttribute(L"name", L"Code");
	node.CreateChild(L"Register", writePending).CreateAttribute(L"name", L"WritePending");
	node.CreateChild(L"Register", dmaPending).CreateAttribute(L"name", L"DMAPending");

	node.CreateChild(L"Register", (unsigned int)nextState).CreateAttribute(L"name", L"NextState");
	//IHeirarchicalStorageNode& renderPosNextNode = node.CreateChild(L"Register").CreateAttribute(L"name", L"RenderPosNext");
	//renderPosNextNode.CreateAttribute(L"Line", renderPosNext.line);
	//renderPosNextNode.CreateAttribute(L"Pixel", renderPosNext.pixel);
	//renderPosNextNode.CreateAttribute(L"VBlank", renderPosNext.vblank);
	//renderPosNextNode.CreateAttribute(L"HBlank", renderPosNext.hblank);
	//IHeirarchicalStorageNode& renderPosCurrentNode = node.CreateChild(L"Register").CreateAttribute(L"name", L"RenderPosCurrent");
	//renderPosCurrentNode.CreateAttribute(L"Line", renderPosCurrent.line);
	//renderPosCurrentNode.CreateAttribute(L"Pixel", renderPosCurrent.pixel);
	//renderPosCurrentNode.CreateAttribute(L"VBlank", renderPosCurrent.vblank);
	//renderPosCurrentNode.CreateAttribute(L"HBlank", renderPosCurrent.hblank);
	node.CreateChild(L"Register", hintCounter).CreateAttribute(L"name", L"HIntCounter");
}

//----------------------------------------------------------------------------------------
//Port functions
//----------------------------------------------------------------------------------------
unsigned short VDP::ReadDataPort(double accessTime, IDeviceContext* device)
{
	writePending = false;

	unsigned short data = 0;
	switch(code)
	{
	case VRAM_READ:
		data = ReadVRAM(address, accessTime, device);
		address += GetAddressIncrement(accessTime);
		break;
	case CRAM_READ:
		data = ReadCRAM(address, accessTime, device);
		address += GetAddressIncrement(accessTime);
		break;
	case VSRAM_READ:
		data = ReadVSRAM(address, accessTime, device);
		address += GetAddressIncrement(accessTime);
		break;
	default:
		//##DEBUG##
		std::wcout << "Unexpected VDP data read:\t" << code << '\n';
		break;
	}

	//Port monitor logging
	if(logDataPortRead)
	{
		RecordPortMonitorEntry(PortMonitorEntry(L"DP Read", device->GetTargetDevice()->GetDeviceInstanceName(), data));
	}

	return data;
}

////----------------------------------------------------------------------------------------
//unsigned short VDP::ReadControlPort(double accessTime, IDeviceContext* device)
//{
//	writePending = false;
//	double initialTime = GetDeviceContext()->GetInitialRemainingTime();
//	double deviceTime = accessTime;
//	double currentTime = initialTime + deviceTime;
////	RasterPos rasterPos = GetRasterPos(renderPosCurrent, currentTime);
//	RasterPosNew rasterPos = GetRasterPosNew(renderPosCurrentNew, currentTime);
//	unsigned short data = GetStatusRegister(rasterPos);
//
//	//Port monitor logging
//	if(logStatusRegisterRead)
//	{
//		RecordPortMonitorEntry(PortMonitorEntry(L"SR Read", device->GetTargetDevice()->GetDeviceInstanceName(), data));
//	}
//
//	return data;
//}

//----------------------------------------------------------------------------------------
void VDP::WriteDataPort(unsigned short data, double accessTime, IDeviceContext* device)
{
	//Port monitor logging
	if(logDataPortWrite)
	{
		RecordPortMonitorEntry(PortMonitorEntry(L"DP Write", device->GetTargetDevice()->GetDeviceInstanceName(), data));
	}

	writePending = false;
	if(dmaPending)
	{
	//##TODO## Determine how the VDP knows it has to do a fill operation here. We're
	//taking a guess with the dmaPending flag.
	//##TODO## Check what happens if the DMA mode is changed from fill before performing
	//the next data write
	//##DEBUG##
//		std::wcout << "DMA Fill:\t" << code << '\t' << GetDMASourceAddress(accessTime) << '\t' << address << '\t' << GetDMATransferCount(accessTime) << '\t' << GetAddressIncrement(accessTime) << '\t' << data << '\n';

		DMAFill(accessTime, device, code, address, GetDMATransferCount(accessTime), GetAddressIncrement(accessTime), data); 
		dmaPending = false;
	}
	else
	{
		switch(code)
		{
		case VRAM_WRITE:
			WriteVRAM(address, data, accessTime, device);
			address += GetAddressIncrement(accessTime);
			break;
		case CRAM_WRITE:
			WriteCRAM(address, data, accessTime, device);
			address += GetAddressIncrement(accessTime);
			break;
		case VSRAM_WRITE:
			WriteVSRAM(address, data, accessTime, device);
			address += GetAddressIncrement(accessTime);
			break;
		default:
			//##DEBUG##
			std::wcout << "Unexpected VDP data write:\t" << code << '\n';
			break;
		}
	}
}

//----------------------------------------------------------------------------------------
void VDP::WriteControlPort(unsigned short data, double accessTime, IDeviceContext* device)
{
	//Port monitor logging
	if(logControlPortWrite)
	{
		RecordPortMonitorEntry(PortMonitorEntry(L"CP Write", device->GetTargetDevice()->GetDeviceInstanceName(),  data));
	}

	if(!writePending)
	{
		//##TODO## Check what happens if another control port write is made while a DMA
		//fill is pending. Try completing the data port write after writing the first
		//command word over again, and see if the fill operation completes.
		unsigned short partialCode = data >> 14;
		if(partialCode == 2)
		{
			//Register write
			unsigned int regNo = (data & 0x1F00) >> 8;
			unsigned int regData = (data & 0x00FF);
			if(regNo < registerCount)
			{
				////##DEBUG##
				//double initialTime = GetDeviceContext()->GetInitialRemainingTime();
				//double deviceTime = accessTime;
				//double currentTime = initialTime + deviceTime;
				//RasterPos rasterPos = GetRasterPos(renderPosCurrent, currentTime);

				//double acurrentTime = accessTime;

				//RasterPos arasterPos = GetRasterPos(renderPosCurrent, acurrentTime);

				//##DEBUG##
//				if((regNo == 10) || (regNo == 0))
//				{
//					std::wcout << "WriteReg:\t" << std::setw(4) << std::setfill(L'0') << std::hex << std::uppercase << regNo << '\t' << data << '\n';
					//std::wcout << '\t' << rasterPos.line << '\t' << currentTime << '\n';
					//std::wcout << '\t' << arasterPos.line << '\t' << acurrentTime << '\n';
//				}

//				reg[regNo] = regData;
				WriteRegister(regNo, accessTime, (unsigned char)regData);
			}

		//##NOTE##
		//According to genvdp, code is cleared, and address is probably cleared. Check on
		//actual system, and verify code is cleared to 0 (VRAM_READ). We can test if
		//address is cleared by doing a partial write after setting a register. The upper
		//bits of address would normally be preserved by a partial write, so if they are
		//clear, the register has been cleared.
		//##NOTE##
		//Contrary to initial assumption, address register is NOT cleared here. Confirmed
		//by EA logo.
			code = VRAM_READ;
		}
		else
		{
			//Partial control port write
			unsigned short partialAddress = data & 0x3FFF;
			code = (Code)(((unsigned char)code & ~0x03) | partialCode);
			address &= ~0x3FFF;
			address |= partialAddress;
			writePending = true;
		}
	}
	else
	{
		dmaPending = ((data & 0x0080) != 0);
		bool cd4 = ((data & 0x0040) != 0);
		unsigned short partialCode = (data & 0x0030) >> 2;
		unsigned short partialAddress = (data & 0x0003) << 14;
		code = (Code)(((unsigned char)code & 0x03) | partialCode);
		address &= 0x3FFF;
		address |= partialAddress;
		writePending = false;

	//##NOTE## dmaPending is only activated if DMAEnabled is true. Determined by first level in WolfChild.
		if(!DMAEnabled(accessTime))
		{
			dmaPending = false;
		}

		if(dmaPending)
		{
			if(GetDMAMode(accessTime) == DMA_MEMORY_TO_VRAM)
			{
				DMAWrite(accessTime, device, code, GetDMASourceAddress(accessTime), address, GetDMATransferCount(accessTime), GetAddressIncrement(accessTime));
				dmaPending = false;
			}
			else if((GetDMAMode(accessTime) == DMA_VRAM_COPY) && cd4)
			{
				DMACopy(accessTime, device, code, GetDMASourceAddress(accessTime), address, GetDMATransferCount(accessTime));
				dmaPending = false;
			}
			else if(GetDMAMode(accessTime) == DMA_VRAM_COPY)
			{
				//##TODO## Check what happens when CD4 is not set when doing a VRAM copy
				//##DEBUG##
				std::wcout << "Attempted VRAM copy when CD4 = 0:\t" << code << '\n';
			}
		}
	}
}

//----------------------------------------------------------------------------------------
//Memory access functions
//----------------------------------------------------------------------------------------
//unsigned short VDP::ReadCRAM(unsigned int address)
//{
//	bool byteSwap = ((address & 0x01) != 0);
//	address &= ~0x01;
//	address %= cramSize;
//
//	unsigned short data = 0;
//	cram->TransparentReadInterface16(address, data);
//	if(byteSwap)
//	{
//		unsigned short upper = (data & 0xFF00) >> 8;
//		unsigned short lower = (data & 0x00FF);
//		data = upper | (lower << 8);
//	}
//	return data;
//}
//
////----------------------------------------------------------------------------------------
//unsigned short VDP::ReadVRAM(unsigned int address)
//{
//	bool byteSwap = ((address & 0x01) != 0);
//	address &= ~0x01;
//	address %= vramSize;
//
//	unsigned short data = 0;
//	vram->TransparentReadInterface16(address, data);
//	if(byteSwap)
//	{
//		unsigned short upper = (data & 0xFF00) >> 8;
//		unsigned short lower = (data & 0x00FF);
//		data = upper | (lower << 8);
//	}
//	return data;
//}
//
////----------------------------------------------------------------------------------------
//unsigned short VDP::ReadVSRAM(unsigned int address)
//{
//	bool byteSwap = ((address & 0x01) != 0);
//	address &= ~0x01;
//	address %= vsramSize;
//
//	unsigned short data = 0;
//	vsram->TransparentReadInterface16(address, data);
//	if(byteSwap)
//	{
//		unsigned short upper = (data & 0xFF00) >> 8;
//		unsigned short lower = (data & 0x00FF);
//		data = upper | (lower << 8);
//	}
//	return data;
//}
//
////----------------------------------------------------------------------------------------
//void VDP::WriteCRAM(unsigned int address, unsigned short data)
//{
//	if((address & 0x01) != 0)
//	{
//		unsigned short upper = (data & 0xFF00) >> 8;
//		unsigned short lower = (data & 0x00FF);
//		data = upper | (lower << 8);
//		address &= ~0x01;
//	}
//
//	address %= cramSize;
//	cram->TransparentWriteInterface16(address, data);
//}
//
////----------------------------------------------------------------------------------------
//void VDP::WriteVRAM(unsigned int address, unsigned short data)
//{
//	if((address & 0x01) != 0)
//	{
//		unsigned short upper = (data & 0xFF00) >> 8;
//		unsigned short lower = (data & 0x00FF);
//		data = upper | (lower << 8);
//		address &= ~0x01;
//	}
//
//	address %= vramSize;
//	vram->TransparentWriteInterface16(address, data);
//}
//
////----------------------------------------------------------------------------------------
//void VDP::WriteVSRAM(unsigned int address, unsigned short data)
//{
//	if((address & 0x01) != 0)
//	{
//		unsigned short upper = (data & 0xFF00) >> 8;
//		unsigned short lower = (data & 0x00FF);
//		data = upper | (lower << 8);
//		address &= ~0x01;
//	}
//
//	address %= vsramSize;
//	vsram->TransparentWriteInterface16(address, data);
//}

//----------------------------------------------------------------------------------------
unsigned short VDP::ReadCRAM(unsigned int address, double accessTime, IDeviceContext* device)
{
	bool byteSwap = ((address & 0x01) != 0);
	address &= ~0x01;
	address %= cramSize;

	Data data(16);
	cram->ReadInterface(0, address, data, device, accessTime, 0);
	if(byteSwap)
	{
		Data tempData(data);
		data.SetUpperHalf(tempData.GetLowerHalf());
		data.SetLowerHalf(tempData.GetUpperHalf());
	}
	return (unsigned short)data.GetData();
}

//----------------------------------------------------------------------------------------
unsigned short VDP::ReadVRAM(unsigned int address, double accessTime, IDeviceContext* device)
{
	bool byteSwap = ((address & 0x01) != 0);
	address &= ~0x01;
	address %= vramSize;

	Data data(16);
	vram->ReadInterface(0, address, data, device, accessTime, 0);
	if(byteSwap)
	{
		Data tempData(data);
		data.SetUpperHalf(tempData.GetLowerHalf());
		data.SetLowerHalf(tempData.GetUpperHalf());
	}
	return (unsigned short)data.GetData();
}

//----------------------------------------------------------------------------------------
unsigned short VDP::ReadVSRAM(unsigned int address, double accessTime, IDeviceContext* device)
{
	bool byteSwap = ((address & 0x01) != 0);
	address &= ~0x01;
	address %= vsramSize;

	Data data(16);
	vsram->ReadInterface(0, address, data, device, accessTime, 0);
	if(byteSwap)
	{
		Data tempData(data);
		data.SetUpperHalf(tempData.GetLowerHalf());
		data.SetLowerHalf(tempData.GetUpperHalf());
	}
	return (unsigned short)data.GetData();
}

//----------------------------------------------------------------------------------------
void VDP::WriteCRAM(unsigned int address, unsigned short data, double accessTime, IDeviceContext* device)
{
	Data tempData(16, data);

	bool byteSwap = ((address & 0x01) != 0);
	address &= ~0x01;
	//##NOTE## This check is probably incorrect. We haven't tested on hardware at all, but
	//we know that the CRAM at least ignores some writes above 0x80, otherwise we get
	//palette corruption in Batman and Robin on the first level. We also know from testing
	//that VSRAM reads above 0x80 return weird data. We also know there's ambiguity with
	//column VSRAM scrolling in H40 mode with partially visible cells, since there are
	//more columns than we have memory for scroll data, and there are apparently
	//differences with this on different versions of the hardware. Testing is definitely
	//required, to confirm read and write behaviour with CRAM and VSRAM at addresses
	//beyond 0x80.
	if(address < cramSize)
	{
		address %= cramSize;
		if(byteSwap)
		{
			tempData.SetUpperBits(8, (data & 0x00FF));
			tempData.SetLowerBits(8, (data & 0xFF00) >> 8);
		}

		cram->WriteInterface(0, address, tempData, device, accessTime, 0);
	}
}

//----------------------------------------------------------------------------------------
void VDP::WriteVRAM(unsigned int address, unsigned short data, double accessTime, IDeviceContext* device)
{
	//##DEBUG##
//	if(data != 0)
//	if((address >= 0xC000) && (address < 0xC180))
//	{
//		std::wcout << "WriteVRAM:\t" << std::setw(4) << std::setfill(L'0') << std::hex << std::uppercase << address << '\t' << data << '\n';
//		system("pause");
//	}
	Data tempData(16, data);

	bool byteSwap = ((address & 0x01) != 0);
	address &= ~0x01;
	address %= vramSize;
	if(byteSwap)
	{
		tempData.SetUpperBits(8, (data & 0x00FF));
		tempData.SetLowerBits(8, (data & 0xFF00) >> 8);
	}

	vram->WriteInterface(0, address, tempData, device, accessTime, 0);
}

//----------------------------------------------------------------------------------------
void VDP::WriteVSRAM(unsigned int address, unsigned short data, double accessTime, IDeviceContext* device)
{
	Data tempData(16, data);

	bool byteSwap = ((address & 0x01) != 0);
	address &= ~0x01;
	//##NOTE## This check is probably incorrect. We haven't tested on hardware at all, but
	//we know that the CRAM at least ignores some writes above 0x80, otherwise we get
	//palette corruption in Batman and Robin on the first level. We also know from testing
	//that VSRAM reads above 0x80 return weird data. We also know there's ambiguity with
	//column VSRAM scrolling in H40 mode with partially visible cells, since there are
	//more columns than we have memory for scroll data, and there are apparently
	//differences with this on different versions of the hardware. Testing is definitely
	//required, to confirm read and write behaviour with CRAM and VSRAM at addresses
	//beyond 0x80.
	if(address < vsramSize)
	{
		address %= vsramSize;
		if(byteSwap)
		{
			tempData.SetUpperBits(8, (data & 0x00FF));
			tempData.SetLowerBits(8, (data & 0xFF00) >> 8);
		}

		vsram->WriteInterface(0, address, tempData, device, accessTime, 0);
	}
}

//----------------------------------------------------------------------------------------
void VDP::DMAWrite(double accessTime, IDeviceContext* device, VDP::Code targetCode, unsigned int sourceAddress, unsigned int targetAddress, unsigned int transferCount, unsigned int increment)
{
	if(transferCount == 0)
	{
		transferCount = 0xFFFF;
	}
	switch(targetCode)
	{
		case VRAM_WRITE:
		{
			//##DEBUG##
//			std::wcout << "DMA Write: " << sourceAddress << '\t' << targetAddress << '\t' << transferCount << '\t' << increment << '\n';
			for(unsigned int i = 0; i < transferCount; ++i)
			{
				Data data(16);
//##TODO## Change these back to normal memory reads. We've got this in place to prevent
//bad rollbacks until we implement bus locking for DMA transfers.
//				memoryBus->ReadMemory(sourceAddress, data, *this);
				memoryBus->TransparentReadMemory(sourceAddress, data, GetDeviceContext(), 0);
				WriteVRAM(targetAddress, (unsigned short)data.GetData(), accessTime, device);
				sourceAddress += 2;
				targetAddress += increment;
			}
			break;
		}
		case CRAM_WRITE:
		{
			for(unsigned int i = 0; i < transferCount; ++i)
			{
				Data data(16);
//				memoryBus->ReadMemory(sourceAddress, data, *this);
				memoryBus->TransparentReadMemory(sourceAddress, data, GetDeviceContext(), 0);
				WriteCRAM(targetAddress, (unsigned short)data.GetData(), accessTime, device);
				sourceAddress += 2;
				targetAddress += increment;
			}
			break;
		}
		case VSRAM_WRITE:
		{
			for(unsigned int i = 0; i < transferCount; ++i)
			{
				Data data(16);
//				memoryBus->ReadMemory(sourceAddress, data, *this);
				memoryBus->TransparentReadMemory(sourceAddress, data, GetDeviceContext(), 0);
				WriteVSRAM(targetAddress, (unsigned short)data.GetData(), accessTime, device);
				sourceAddress += 2;
				targetAddress += increment;
			}
			break;
		}
	}
}

//----------------------------------------------------------------------------------------
//##FIX## For both fills and copies, the byte order of writes to the VRAM may be
//incorrect for writes to odd addresses. Refer to data tables in Genesis Software Manual,
//page 30. This may be the cause of our speedo glitch in Virtua Racing.
void VDP::DMACopy(double accessTime, IDeviceContext* device, VDP::Code targetCode, unsigned int sourceAddress, unsigned int targetAddress, unsigned int transferCount)
{
	if(transferCount == 0)
	{
		transferCount = 0xFFFF;
	}
	switch(targetCode)
	{
		case VRAM_WRITE:{
			for(unsigned int i = 0; i < transferCount; ++i)
			{
				unsigned short temp;
				temp = ReadVRAM(sourceAddress + i, accessTime, device);
				WriteVRAM(targetAddress + i, temp, accessTime, device);
			}
		break;}
		case CRAM_WRITE:{
			for(unsigned int i = 0; i < transferCount; ++i)
			{
				unsigned short temp;
				temp = ReadCRAM(sourceAddress + i, accessTime, device);
				WriteCRAM(targetAddress + i, temp, accessTime, device);
			}
		break;}
		case VSRAM_WRITE:{
			for(unsigned int i = 0; i < transferCount; ++i)
			{
				unsigned short temp;
				temp = ReadVSRAM(sourceAddress + i, accessTime, device);
				WriteVSRAM(targetAddress + i, temp, accessTime, device);
			}
		break;}
	}
}

//----------------------------------------------------------------------------------------
void VDP::DMAFill(double accessTime, IDeviceContext* device, VDP::Code targetCode, unsigned int targetAddress, unsigned int transferCount, unsigned int increment, unsigned short data)
{
	if(transferCount == 0)
	{
		transferCount = 0xFFFF;
	}
	switch(targetCode)
	{
		case VRAM_WRITE:{
			for(unsigned int i = 0; i < transferCount; ++i)
			{
				WriteVRAM(targetAddress, data, accessTime, device);
				targetAddress += increment;
			}
		break;}
		case CRAM_WRITE:{
			for(unsigned int i = 0; i < transferCount; ++i)
			{
				WriteCRAM(targetAddress, data, accessTime, device);
				targetAddress += increment;
			}
		break;}
		case VSRAM_WRITE:{
			for(unsigned int i = 0; i < transferCount; ++i)
			{
				WriteVSRAM(targetAddress, data, accessTime, device);
				targetAddress += increment;
			}
		break;}
	}
}

/*******************************Render functions**********************************/
//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
unsigned int VDP::WindowStartX() const
{
//Assumed constants
	unsigned int blockPixelSizeX = 8;

	if(!WindowRightAligned())
	{
		return 0;
	}
	else
	{
		return (WindowBasePointX() * blockPixelSizeX) * 2;
	}
}

//----------------------------------------------------------------------------------------
unsigned int VDP::WindowStartY() const
{
//Assumed constants
	unsigned int blockPixelSizeY = 8;

	if(!WindowDownAligned())
	{
		return 0;
	}
	else
	{
		return (WindowBasePointY() * blockPixelSizeY);
	}
}

//----------------------------------------------------------------------------------------
unsigned int VDP::GetWindowWidth() const
{
//Assumed constants
	unsigned int blockPixelSizeX = 8;

	if(!WindowRightAligned())
	{
		return (WindowBasePointX() * 2) * blockPixelSizeX;
	}
	else
	{
		return (GetVisibleCellWidth() - (WindowBasePointX() * 2)) * blockPixelSizeX;
	}
}

//----------------------------------------------------------------------------------------
unsigned int VDP::GetWindowHeight() const
{
//##TODO## Test how the window behaves in interlace mode 2
//Assumed constants
	unsigned int blockPixelSizeY = 8;

	if(!WindowDownAligned())
	{
		return WindowBasePointY() * blockPixelSizeY;
	}
	else
	{
		return (GetVisibleCellHeight() - WindowBasePointY()) * blockPixelSizeY;
	}
}

//----------------------------------------------------------------------------------------
bool VDP::CoordinatesInWindow(unsigned int x, unsigned int y) const
{
//##TODO## Check if both base points have to be zero to disable the window, or just one
	if((x >= WindowStartX()) && (x < (WindowStartX() + GetWindowWidth())))
	{
		return true;
	}
	if((y >= WindowStartY()) && (y < (WindowStartY() + GetWindowHeight())))
	{
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------------------
//Block selection functions
//----------------------------------------------------------------------------------------
VDP::BlockMapping VDP::DecodeBlockMapping(unsigned int mappingAddress) const
{
	BlockMapping mapping;

//Obtain raw mapping
	Data rawMapping(16);
	vram->ReadCommitted(mappingAddress, rawMapping);

//Translate block mapping
	//##TODO## Add some comments dammit! COMMENTS!
	//##TODO## Use GetDataSegment
	mapping.blockNumber = rawMapping.GetData() & 0x07FF;
	mapping.paletteLine = (rawMapping.GetData() & 0x6000) >> 13;
	mapping.priority = (rawMapping.GetData() & 0x8000) != 0;
	mapping.vflip = (rawMapping.GetData() & 0x1000) != 0;
	mapping.hflip = (rawMapping.GetData() & 0x0800) != 0;

	return mapping;
}

//----------------------------------------------------------------------------------------
//##TODO## Optimize this function
unsigned int VDP::GetBlockPaletteEntry(const BlockMapping& mapping, unsigned int xoffset, unsigned int yoffset) const
{
//Assumed constants
	unsigned int blockPixelSizeX = 8;

	unsigned int blockPixelSizeY = (GetInterlaceMode() != INTERLACE_DOUBLE)? 8: 16;
	unsigned int blockSize = (GetInterlaceMode() != INTERLACE_DOUBLE)? 0x20: 0x40;
	unsigned int blockLocation = mapping.blockNumber * blockSize;

	if(GetInterlaceMode() == INTERLACE_DOUBLE)
	{
		yoffset *= 2;
		if(GetOddInterlaceFrame())
		{
			yoffset += 1;
		}
	}

//Perform block mirroring
	if(mapping.hflip)
	{
		xoffset = (blockPixelSizeX - 1) - xoffset;
	}
	if(mapping.vflip)
	{
		yoffset = (blockPixelSizeY - 1) - yoffset;
	}

//Decode block palette index
	Data index(8);
	unsigned int blockOffset = xoffset + (yoffset * blockPixelSizeX);
	vram->ReadCommitted((blockLocation + (blockOffset / 2)), index);
	if((blockOffset % 2) == 0)
	{
		index >>= 4;
	}
	index &= 0x0F;

	return index.GetData();
}

//----------------------------------------------------------------------------------------
unsigned int VDP::GetSpriteMappingSize() const
{
	//Assumed constants
	const unsigned int spriteMappingSize = 8;

	return spriteMappingSize;
}

//----------------------------------------------------------------------------------------
VDP::SpriteMapping VDP::GetSpriteMapping(unsigned int spriteMappingAddress) const
{
//Build sprite mapping structure
	SpriteMapping mapping;

//##TODO## Improve these notes. Use your normal table structure to show the bit mappings.
//Sprite mapping format
//	Index + 0  :   yyyyyyyy yyyyyyyy
//	Index + 2  :   ----hhvv -lllllll
//	Index + 4  :   pccvhnnn nnnnnnnn
//	Index + 6  :   xxxxxxxx xxxxxxxx
//	y = Vertical coordinate of sprite
//	h = Horizontal size in cells (00b=1 cell, 11b=4 cells)
//	v = Vertical size in cells (00b=1 cell, 11b=4 cells)
//	l = Link field
//	p = Priority
//	c = Color palette
//	v = Vertical flip
//	h = Horizontal flip
//	n = Sprite pattern start index
//	x = Horizontal coordinate of sprite
//##NOTE## The x and y position bit mappings originally obtained from genvdp.txt indicate
//that the x and y positions are 10-bit values. This is not the case. The full 16-bit
//values are reserved for the x and y positions, and these values wrap at the boundaries
//of the field size. In normal rendering modes, the field size is 512x512, so the x and y
//positions would be modulus 512. In interlace mode 2 however, the vertical size is 1024.
//##TODO## Even though it shouldn't be, confirm on the real system that in interlace mode
//2, the horizontal field size is not also 1024.
	unsigned int fieldSizeX = 1024;
	unsigned int fieldSizeY = (GetInterlaceMode() != INTERLACE_DOUBLE)? 512: 1024;

	Data spriteData1(16);
	Data spriteData2(16);
	Data spriteData3(16);
	Data spriteData4(16);
	vram->ReadCommitted(spriteMappingAddress + 0, spriteData1);
	vram->ReadCommitted(spriteMappingAddress + 2, spriteData2);
	vram->ReadCommitted(spriteMappingAddress + 4, spriteData3);
	vram->ReadCommitted(spriteMappingAddress + 6, spriteData4);

	//##TODO## Use GetDataSegment
	mapping.ypos = (spriteData1.GetData() % fieldSizeY);
	mapping.width = ((spriteData2.GetData() & 0x0C00) >> 10) + 1;
	mapping.height = ((spriteData2.GetData() & 0x0300) >> 8) + 1;
	mapping.link = (spriteData2.GetData() & 0x007F);
	mapping.priority = ((spriteData3.GetData() & 0x8000) != 0);
	mapping.paletteLine = (spriteData3.GetData() & 0x6000) >> 13;
	mapping.vflip = ((spriteData3.GetData() & 0x1000) != 0);
	mapping.hflip = ((spriteData3.GetData() & 0x0800) != 0);
	mapping.blockNumber = (spriteData3.GetData() & 0x07FF);
	mapping.xpos = (spriteData4.GetData() % fieldSizeX);

	return mapping;
}

//----------------------------------------------------------------------------------------
void VDP::SetSpriteMapping(unsigned int spriteMappingAddress, const SpriteMapping& mapping)
{
	unsigned int fieldSizeX = 1024;
	unsigned int fieldSizeY = (GetInterlaceMode() != INTERLACE_DOUBLE)? 512: 1024;

	Data spriteData1(16);
	Data spriteData2(16);
	Data spriteData3(16);
	Data spriteData4(16);

	//##TODO## Add some notes referring to the sprite byte mappings shown in the
	//GetSpriteMapping function above.
	//##TODO## Use SetDataSegment
	spriteData1 = (mapping.ypos % fieldSizeY);
	spriteData2 = (spriteData2.GetData() & ~0x0C00) | (((mapping.width - 1) << 10) & 0x0C00);
	spriteData2 = (spriteData2.GetData() & ~0x0300) | (((mapping.height - 1) << 8) & 0x0300);
	spriteData2 = (spriteData2.GetData() & ~0x007F) | (mapping.link & 0x007F);
	spriteData3	= (spriteData3.GetData() & ~0x8000) | ((mapping.priority)? 0x8000: 0x0);
	spriteData3 = (spriteData3.GetData() & ~0x6000) | ((mapping.paletteLine << 13) & 0x6000);
	spriteData3	= (spriteData3.GetData() & ~0x1000) | ((mapping.vflip)? 0x1000: 0x0);
	spriteData3	= (spriteData3.GetData() & ~0x0800) | ((mapping.hflip)? 0x0800: 0x0);
	spriteData3 = (spriteData3.GetData() & ~0x07FF) | (mapping.blockNumber & 0x07FF);
	spriteData4 = (mapping.xpos % fieldSizeX);

	vram->TransparentWriteInterface(0, spriteMappingAddress + 0, spriteData1, this->GetDeviceContext(), 0);
	vram->TransparentWriteInterface(0, spriteMappingAddress + 2, spriteData2, this->GetDeviceContext(), 0);
	vram->TransparentWriteInterface(0, spriteMappingAddress + 4, spriteData3, this->GetDeviceContext(), 0);
	vram->TransparentWriteInterface(0, spriteMappingAddress + 6, spriteData4, this->GetDeviceContext(), 0);
}

//----------------------------------------------------------------------------------------
VDP::PaletteIndex VDP::GetSpritePaletteIndex(const SpriteMapping& mapping, unsigned int x, unsigned int y) const
{
//Assumed constants
	unsigned int blockWidth = 8;
	unsigned int blockHeight = 8;

//Calculate block location and offset
	unsigned int blockLocation;
	blockLocation = mapping.blockNumber;
	if(!mapping.hflip)
	{
		blockLocation += (x / blockWidth) * mapping.height;
	}
	else
	{
		blockLocation += ((mapping.width - 1) - (x / blockWidth)) * mapping.height;
	}
	if(!mapping.vflip)
	{
		blockLocation += y / blockHeight;
	}
	else
	{
		blockLocation += (mapping.height - 1) - (y / blockHeight);
	}

	PaletteIndex index;
	BlockMapping blockMapping;
	blockMapping.blockNumber = blockLocation;
	blockMapping.hflip = mapping.hflip;
	blockMapping.vflip = mapping.vflip;
	index.entry = GetBlockPaletteEntry(blockMapping, x % blockWidth, y % blockHeight);
	index.line = mapping.paletteLine;
	return index;
}

//----------------------------------------------------------------------------------------
VDP::Color VDP::GetPaletteColor(const PaletteIndex& index) const
{
	//Assumed constants
	unsigned int paletteEntriesPerLine = 16;

	//Decode palette color
	Data paletteEntry(16);
	cram->ReadCommitted(((index.entry + (index.line * paletteEntriesPerLine)) * 2), paletteEntry);
	Color color;
	//##TODO## Add some notes here showing the bit structure of a palette entry in a
	//pretty table.
	//##TODO## Use GetDataSegment
	color.r = (paletteEntry.GetData() & 0x000E) >> 1;
	color.g = (paletteEntry.GetData() & 0x00E0) >> 5;
	color.b = (paletteEntry.GetData() & 0x0E00) >> 9;

	color.shadow = index.shadow;
	color.highlight = index.highlight;

	return color;
}

//----------------------------------------------------------------------------------------
//Render functions
//----------------------------------------------------------------------------------------
void VDP::SelectBlock(unsigned int xoffset, unsigned int yoffset, unsigned int fieldWidth, unsigned int fieldHeight, unsigned int mappingTableAddress, BlockMapping& blockMapping) const
{
//Assumed constants
	const unsigned int blockWidth = 8;
	const unsigned int blockHeight = 8;
	const unsigned int blockMappingSize = 2;

//Turn playfield pixel positions into playfield block numbers
	xoffset /= blockWidth;
	yoffset /= blockHeight;

//Wrap block numbers to be within playfield boundaries
	xoffset %= fieldWidth;
	yoffset %= fieldHeight;

//Obtain block mapping
	unsigned int mappingAddress = mappingTableAddress + ((xoffset + (yoffset * fieldWidth)) * blockMappingSize);
	blockMapping = DecodeBlockMapping(mappingAddress);
}

//----------------------------------------------------------------------------------------
int VDP::GetScrollOffsetX(unsigned int scrollTableBase, ScrollModeH scrollMode, unsigned int y) const
{
//Assumed constants
	unsigned int scrollDataEntryOffset = 4;
	unsigned int blockPixelSizeX = 8;

//Calculate position of corresponding entry in scroll table
	unsigned int tableOffset = 0;
	switch(scrollMode)
	{
	case SCROLLH_OVERALL:
		tableOffset = 0;
		break;
	case SCROLLH_CELL:
		tableOffset = (y - (y % blockPixelSizeX));
		break;
	case SCROLLH_LINE:
		tableOffset = y;
		break;
	}

//Translate scroll entry
	tableOffset *= scrollDataEntryOffset;

	//##TODO## Clean this up. Use a sign extend or something.
	Data offset(16);
	vram->ReadCommitted(scrollTableBase + tableOffset, offset);
	short result = (short)((unsigned short)offset.GetData());
	return (int)-result;
}

//----------------------------------------------------------------------------------------
int VDP::GetScrollOffsetY(unsigned int scrollTableBase, ScrollModeV scrollMode, unsigned int x) const
{
//##TODO## Add support for interlace mode 2. See segatech docs for interlace mode 2 vscroll handling.
//Assumed constants
	unsigned int scrollDataEntryOffset = 4;
	unsigned int blockPixelSizeYFixed = 8;
//	unsigned int blockPixelSizeY = (GetInterlaceMode() != INTERLACE_DOUBLE)? 8: 16;
	unsigned int blockPixelSizeY = 8;
	unsigned short scrollDataMask = (GetInterlaceMode() != INTERLACE_DOUBLE)? 0x03FF: 0x07FF;

//Calculate position of corresponding entry in scroll table
	unsigned int tableOffset = 0;
	switch(scrollMode)
	{
	case SCROLLV_OVERALL:
		tableOffset = 0;
		break;
	case SCROLLV_2CELL:
		tableOffset = (x / (blockPixelSizeYFixed * 2));
		break;
	}

//Translate scroll entry
	tableOffset *= scrollDataEntryOffset;

	//##TODO## Clean this up. Use a signextend or something.
	Data offset(16);
	vsram->ReadCommitted(scrollTableBase + tableOffset, offset);
	if((offset.GetData() & ((scrollDataMask + 1) >> 1)) != 0)
	{
		offset |= ~scrollDataMask;
	}
	short result = (short)((unsigned short)offset.GetData());

//##FIX## Handle this extra bit of vertical scroll information. I assume this is the
//magic bullet to stop the excessive flickering. We have observed that some tiles seem to
//remain steady and not be affected by the interlace mode, and this is most likely the
//key to that behaviour.
	if(GetInterlaceMode() == INTERLACE_DOUBLE)
	{
//		if(!GetOddInterlaceFrame())
//		{
//			test += ((test & 0x01) != 0)? 0x02: 0x00;
//		}
//		test >>= 1;
		result /= 2;
	}

	return (int)result;
}

//----------------------------------------------------------------------------------------
void VDP::BuildSpriteData(unsigned int line)
{
	//Assumed constants
	unsigned int blockWidth = 8;
	unsigned int blockHeight = 8;
	unsigned int hoffset = 128;
	unsigned int voffset = 128;
	unsigned int interlaceScale = (GetInterlaceMode() != INTERLACE_DOUBLE)? 1: 2;

	//Limitations
	unsigned int maxPixelsPerLine = GetVisibleCellWidth() * blockWidth;
	unsigned int maxSpritesPerLine = GetVisibleCellWidth() / 2;
	unsigned int maxSprites = GetVisibleCellWidth() * 2;

	//Clear previous sprite data
	//##TODO## Fix up the width
	for(unsigned int i = 0; i < 512; ++i)
	{
		spriteData[i].written = false;
	}

	//Locate sprites on line
	line += voffset;
	unsigned int foundSprites = 0;
	unsigned int foundSpritesOnLine = 0;
	unsigned int pixelsDrawn = 0;
	bool spritesMasked = false;
	bool foundSpriteMaskingMode2Trigger = false;

	SpriteMapping currentMapping;
	unsigned int nextSprite = 0;
	do
	{
		//##DEBUG##
//		std::wcout << nextSprite << '\n';
		currentMapping = GetSpriteMapping(GetSpriteTableAddress() + (GetSpriteMappingSize() * nextSprite));
		if(((currentMapping.ypos / interlaceScale) <= line) && (((currentMapping.ypos / interlaceScale) + (currentMapping.height * blockHeight)) > line))
		{
			//Sprite masking
			//##TODO## Do more thorough testing on sprite masking mode 2. It appears to
			//be a real mode, but do more tests to verify its behaviour. Does mode 2 just
			//cause sprites with xpos=1 to replace sprites with xpos=0 as the masking
			//sprite, or is it really the combination of both sprites on one line that
			//activate masking? If not, What happens if we have the sprite with xpos=1
			//after the sprite with xpos=0 on a scanline? Will this cause the mode not to
			//be enabled if the mode has not already been activated, and if the mode has
			//been activated, will this cause the line to miss being masked? Does the
			//behaviour remain the same if the masking sprites are not at the beginning
			//of the priority chain? If sprite masking mode 2 is active and an xpos=0
			//sprite is not matched with an xpos=1 sprite, so masking is not triggered,
			//does that xpos=0 sprite contribute to the normal sprite overflow counts,
			//or is the sprite simply discarded?
			if(currentMapping.xpos == 1)
			{
				spriteMaskingMode2Active = true;
				foundSpriteMaskingMode2Trigger = true;
				nextSprite = currentMapping.link;
				continue;
			}
			if(currentMapping.xpos == 0)
			{
				if(!spriteMaskingMode2Active || foundSpriteMaskingMode2Trigger)
				{
					spritesMasked = true;
					continue;
				}
			}

			for(unsigned int i = 0; i < (currentMapping.width * blockWidth); ++i)
			{
				unsigned int pixelPos = (currentMapping.xpos + i);
				if(pixelPos < hoffset)
				{
					continue;
				}
				pixelPos -= hoffset;
				if(!spriteData[pixelPos].written)
				{
					PaletteIndex index;
					index = GetSpritePaletteIndex(currentMapping, i, line - (currentMapping.ypos / interlaceScale));
//					//##DEBUG##
//					index.line = 0;
//					index.entry = 0x0F;
					if(index.entry != 0)
					{
						spriteData[pixelPos].written = true;
						spriteData[pixelPos].priority = currentMapping.priority;
						spriteData[pixelPos].entry = index;
					}
				}
				++pixelsDrawn;
			}

			++foundSpritesOnLine;
		}
		++foundSprites;
		nextSprite = currentMapping.link;
	} while((nextSprite != 0) && (foundSprites < maxSprites) && (foundSpritesOnLine < maxSpritesPerLine) && !spritesMasked && (pixelsDrawn < maxPixelsPerLine));

	//##DEBUG##
	//	std::wcout << "FoundSprites: " << foundSprites << ' ' << pixelsDrawn << '\n';
}

//----------------------------------------------------------------------------------------
void VDP::RenderLine(unsigned int line)
{
	//Assumed constants
	const unsigned int blockHeight = 8;
	const unsigned int blockWidth = 8;

	///////////////////////////////////////////////////////
	//      lines   lines   lines   vcells  vsync   hsync
	//      screen  display retrace
	//NTSC	262     224     38      28      60Hz    15720Hz
	//PAL	312     224     88      28      50Hz    15600Hz
	//PAL	312     240     72      30      50Hz    15600Hz
	///////////////////////////////////////////////////////
	//##TODO## Calculate these values based on the current display settings
	unsigned int visibleCellHeight = GetVisibleCellHeight();
	unsigned int rasterCountScreen = 312;
	unsigned int refreshRate = 50;
	unsigned int rasterCountTopBorder = ((34 - visibleCellHeight) / 2) * blockHeight;
	unsigned int rasterCountFrame = visibleCellHeight * blockHeight;
	unsigned int rasterCountBottomBorder = ((34 - visibleCellHeight) / 2) * blockHeight;
	unsigned int rasterCountRetrace = rasterCountScreen - (rasterCountTopBorder + rasterCountFrame + rasterCountBottomBorder);
	unsigned int rasterPosFrame = rasterCountTopBorder;
	unsigned int rasterPosBottomBorder = rasterCountTopBorder + rasterCountFrame;
	unsigned int rasterPosRetrace = rasterCountTopBorder + rasterCountFrame + rasterCountBottomBorder;
	RasterPosNew temp(rasterCountScreen, refreshRate);
	double lineRenderTime = temp.GetRasterRenderTime();

	//Read values which are constant for the line
	unsigned int width = GetVisibleCellWidth() * 8;
	unsigned int rasterLine = line;
	line -= rasterPosFrame;

	//Fill the line with the background colour if the display is disabled
	if((rasterLine < rasterPosFrame) || (rasterLine >= rasterPosBottomBorder) || !GetDisplayEnabled())
	{
		unsigned int xposOffset = (320 - width) / 2;
		for(unsigned int xpos = 0; xpos < xposOffset; ++xpos)
		{
			WritePixelToImageBuffer(GetBackgroundPaletteIndex(), xpos, rasterLine);
		}
		for(unsigned int xpos = 0; xpos < width; ++xpos)
		{
			if(enableCRAMFlicker && (cram->GetNextWriteTime(cramTimesliceCopy) < (lineRenderTime / (double)width)))
			{
				TimedRAM::WriteInfo cramWriteInfo1 = cram->GetWriteInfo(0, cramTimesliceCopy);
				TimedRAM::WriteInfo cramWriteInfo2 = cram->GetWriteInfo(1, cramTimesliceCopy);

				//Decode palette color
				unsigned short paletteEntry;
				paletteEntry = cramWriteInfo1.newValue;
				paletteEntry <<= 8;
				paletteEntry |= cramWriteInfo2.newValue;
				Color color;
				color.r = (paletteEntry & 0x000E) >> 1;
				color.g = (paletteEntry & 0x00E0) >> 5;
				color.b = (paletteEntry & 0x0E00) >> 9;

				color.shadow = false;
				color.highlight = false;

				WriteColorToImageBuffer(color, xpos + xposOffset, rasterLine);
			}
			else
			{
				WritePixelToImageBuffer(GetBackgroundPaletteIndex(), xpos + xposOffset, rasterLine);
			}

			if(vramSync)
			{
				vram->AdvanceByTime((lineRenderTime / (double)width), vramTimesliceCopy);
			}
			if(cramSync)
			{
				cram->AdvanceByTime((lineRenderTime / (double)width), cramTimesliceCopy);
			}
			if(vsramSync)
			{
				vsram->AdvanceByTime((lineRenderTime / (double)width), vsramTimesliceCopy);
			}
		}
		for(unsigned int xpos = 0; xpos < xposOffset; ++xpos)
		{
			WritePixelToImageBuffer(GetBackgroundPaletteIndex(), xposOffset + width + xpos, rasterLine);
		}
		return;
	}

	//Read values which are constant for the line
	BuildSpriteData(line);

	//Read initial values which may change throughout the line
	PaletteIndex finalPaletteIndex;
	int playfieldX;
	int playfieldY;

	BlockMapping mappingWindow;
	BlockMapping mappingLayerA;
	BlockMapping mappingLayerB;
	PaletteIndex indexSprite;
	PaletteIndex indexWindow;
	PaletteIndex indexLayerA;
	PaletteIndex indexLayerB;
	bool prioritySprite;
	bool priorityWindow;
	bool priorityLayerA;
	bool priorityLayerB;

	int windowDistortionOffset = 0;
	int layerAOffsetX = GetScrollOffsetX(GetHScrollTableA(), GetScrollModeH(), line);
	int layerBOffsetX = GetScrollOffsetX(GetHScrollTableB(), GetScrollModeH(), line);
	int layerAOffsetY = 0;
	int layerBOffsetY = 0;

	//Build Window
	SelectBlock(0, line, GetWindowScrollSizeH(), GetWindowScrollSizeV(), GetMappingAddressWindow(), mappingWindow);
	indexWindow.line = mappingWindow.paletteLine;
	priorityWindow = mappingWindow.priority;

	//Build layer A
	playfieldX = layerAOffsetX;
	layerAOffsetY = GetScrollOffsetY(GetVScrollTableA(), GetScrollModeV(), 0);
	playfieldY = (int)line + layerAOffsetY;
	SelectBlock(playfieldX, playfieldY, GetScrollSizeH(), GetScrollSizeV(), GetMappingAddressA(), mappingLayerA);
	indexLayerA.line = mappingLayerA.paletteLine;
	priorityLayerA = mappingLayerA.priority;

	//Build layer B
	playfieldX = layerBOffsetX;
	layerBOffsetY = GetScrollOffsetY(GetVScrollTableB(), GetScrollModeV(), 0);
	playfieldY = (int)line + layerBOffsetY;
	SelectBlock(playfieldX, playfieldY, GetScrollSizeH(), GetScrollSizeV(), GetMappingAddressB(), mappingLayerB);
	indexLayerB.line = mappingLayerB.paletteLine;
	priorityLayerB = mappingLayerB.priority;

	//Render the line
	unsigned int xposOffset = (320 - width) / 2;
	for(unsigned int xpos = 0; xpos < xposOffset; ++xpos)
	{
		WritePixelToImageBuffer(GetBackgroundPaletteIndex(), xpos, rasterLine);
	}
	for(unsigned int xpos = 0; xpos < width; ++xpos)
	{
		bool foundSprite = false;
		bool foundWindow = false;
		bool foundLayerA = false;
		bool foundLayerB = false;
		bool shadow = false;
		bool highlight = false;

		//Attempt sprite
		foundSprite = spriteData[xpos].written;
		if(foundSprite)
		{
			indexSprite = spriteData[xpos].entry;
			prioritySprite = spriteData[xpos].priority;
		}

		//Attempt Window
		if((xpos % blockWidth) == 0)
		{
			SelectBlock(xpos, line, GetWindowScrollSizeH(), GetWindowScrollSizeV(), GetMappingAddressWindow(), mappingWindow);
			indexWindow.line = mappingWindow.paletteLine;
			priorityWindow = mappingWindow.priority;
		}
		indexWindow.entry = GetBlockPaletteEntry(mappingWindow, xpos % blockWidth, line % blockHeight);
		foundWindow = (indexWindow.entry != 0);

		//Attempt layer A
		playfieldX = (int)xpos + layerAOffsetX + windowDistortionOffset;
		playfieldY = (int)line + layerAOffsetY;

		if((playfieldX % blockWidth) == 0)
		{
			//Emulate the window distortion bug
			if((enableWindowHigh || enableWindowLow)	//Window layers are enabled
				&& (enableWindowDistortion)	//Window distortion bug is enabled
				&& ((GetWindowWidth() != 0) && (!WindowRightAligned()))	//Window is visible, and aligned to the left
				&& ((layerAOffsetX % (blockWidth * 2)) > 0)	//The current scroll offset isn't aligned to a 2-cell boundary
				&& (xpos < (GetWindowWidth() + (blockWidth * 2) - (layerAOffsetX % (blockWidth * 2)))))	//xpos is before the end of the distorted region
			{
				windowDistortionOffset = blockWidth * 2;
			}
			else
			{
				windowDistortionOffset = 0;
			}

			playfieldX = (int)xpos + layerAOffsetX + windowDistortionOffset;
			layerAOffsetY = GetScrollOffsetY(GetVScrollTableA(), GetScrollModeV(), xpos);
			playfieldY = (int)line + layerAOffsetY;
			SelectBlock(playfieldX, playfieldY, GetScrollSizeH(), GetScrollSizeV(), GetMappingAddressA(), mappingLayerA);
			indexLayerA.line = mappingLayerA.paletteLine;
			priorityLayerA = mappingLayerA.priority;
		}
		indexLayerA.entry = GetBlockPaletteEntry(mappingLayerA, playfieldX % blockWidth, playfieldY % blockHeight);
		foundLayerA = (indexLayerA.entry != 0);

		//Attempt layer B
		playfieldX = (int)xpos + layerBOffsetX;
		playfieldY = (int)line + layerBOffsetY;
		if((playfieldX % blockWidth) == 0)
		{
			layerBOffsetY = GetScrollOffsetY(GetVScrollTableB(), GetScrollModeV(), xpos);
			playfieldY = (int)line + layerBOffsetY;
			SelectBlock(playfieldX, playfieldY, GetScrollSizeH(), GetScrollSizeV(), GetMappingAddressB(), mappingLayerB);
			indexLayerB.line = mappingLayerB.paletteLine;
			priorityLayerB = mappingLayerB.priority;
		}
		indexLayerB.entry = GetBlockPaletteEntry(mappingLayerB, playfieldX % blockWidth, playfieldY % blockHeight);
		foundLayerB = (indexLayerB.entry != 0);

		//Shadow highlight sprite filter checks
		if(GetShadowHighlightEnabled() && foundSprite && prioritySprite && enableSpriteHigh)
		{
			if((indexSprite.line == 3) && (indexSprite.entry == 14))
			{
				highlight = true;
				foundSprite = false;
			}
			else if((indexSprite.line == 3) && (indexSprite.entry == 15))
			{
				shadow = true;
				foundSprite = false;
			}
		}

		//Select visible layer
		if(foundSprite && prioritySprite && enableSpriteHigh)
		{
			finalPaletteIndex = indexSprite;
		}
		else
		{
			//Shadow highlight layer A+B checks
			if(GetShadowHighlightEnabled()	//Shadow highlight mode active
				&& (((CoordinatesInWindow(xpos, line) && !priorityWindow) || (!CoordinatesInWindow(xpos, line) && !priorityLayerA)) && !priorityLayerB)	//Layer A and Layer B are low priority
				&& ((foundWindow && enableWindowLow) || enableLayerALow || enableLayerBLow))	//Either layer A or Layer B low priority layers are enabled
			{
				shadow |= true;
			}
			if(foundWindow && priorityWindow && enableWindowHigh && CoordinatesInWindow(xpos, line))
			{
				finalPaletteIndex = indexWindow;
			}
			else if(foundLayerA && priorityLayerA && enableLayerAHigh && !(CoordinatesInWindow(xpos, line) && (enableWindowLow || enableWindowHigh)))
			{
				finalPaletteIndex = indexLayerA;
			}
			else if(foundLayerB && priorityLayerB && enableLayerBHigh)
			{
				finalPaletteIndex = indexLayerB;
			}
			else
			{
				//Shadow highlight low priority sprite checks
				if(GetShadowHighlightEnabled() && foundSprite && !prioritySprite && enableSpriteLow)
				{
					if((indexSprite.line == 3) && (indexSprite.entry == 14))
					{
						highlight = true;
						foundSprite = false;
					}
					else if((indexSprite.line == 3) && (indexSprite.entry == 15))
					{
						shadow = true;
						foundSprite = false;
					}
				}
				if(foundSprite && !prioritySprite && enableSpriteLow)
				{
					finalPaletteIndex = indexSprite;
				}
				else if(foundWindow && !priorityWindow && enableWindowLow && CoordinatesInWindow(xpos, line))
				{
					finalPaletteIndex = indexWindow;
				}
				else if(foundLayerA && !priorityLayerA && enableLayerALow && !(CoordinatesInWindow(xpos, line) && (enableWindowLow || enableWindowHigh)))
				{
					finalPaletteIndex = indexLayerA;
				}
				else if(foundLayerB && !priorityLayerB && enableLayerBLow)
				{
					finalPaletteIndex = indexLayerB;
				}
				else
				{
					//Default to background color
					finalPaletteIndex = GetBackgroundPaletteIndex();
				}
			}
		}

		finalPaletteIndex.highlight = highlight;
		finalPaletteIndex.shadow = shadow;

		//Draw pixel
		if(enableCRAMFlicker && (cram->GetNextWriteTime(cramTimesliceCopy) < (lineRenderTime / (double)width)))
		{
			TimedRAM::WriteInfo cramWriteInfo1 = cram->GetWriteInfo(0, cramTimesliceCopy);
			TimedRAM::WriteInfo cramWriteInfo2 = cram->GetWriteInfo(1, cramTimesliceCopy);

			//Decode palette color
			unsigned short paletteEntry;
			paletteEntry = cramWriteInfo1.newValue;
			paletteEntry <<= 8;
			paletteEntry |= cramWriteInfo2.newValue;
			Color color;
			color.r = (paletteEntry & 0x000E) >> 1;
			color.g = (paletteEntry & 0x00E0) >> 5;
			color.b = (paletteEntry & 0x0E00) >> 9;

			color.shadow = false;
			color.highlight = false;

			WriteColorToImageBuffer(color, xpos + xposOffset, rasterLine);
		}
		else
		{
			WritePixelToImageBuffer(finalPaletteIndex, xpos + xposOffset, rasterLine);
		}

		if(vramSync)
		{
			vram->AdvanceByTime((lineRenderTime / (double)width), vramTimesliceCopy);
		}
		if(cramSync)
		{
			cram->AdvanceByTime((lineRenderTime / (double)width), cramTimesliceCopy);
		}
		if(vsramSync)
		{
			vsram->AdvanceByTime((lineRenderTime / (double)width), vsramTimesliceCopy);
		}
	}
	for(unsigned int xpos = 0; xpos < xposOffset; ++xpos)
	{
		WritePixelToImageBuffer(GetBackgroundPaletteIndex(), xposOffset + width + xpos, rasterLine);
	}
}

//----------------------------------------------------------------------------------------
void VDP::WritePixelToImageBuffer(const PaletteIndex& paletteIndex, unsigned int xpos, unsigned int ypos)
{
	//Assumed constants
	unsigned int height = 272;

	//Translate the final palette index into a colour value
//	Color color = GetPaletteColor(paletteIndex);
//	unsigned char r = color.GetR8Bit();
//	unsigned char g = color.GetG8Bit();
//	unsigned char b = color.GetB8Bit();

	//Decode palette color
	unsigned int paletteEntriesPerLine = 16;
	Data paletteEntry(16);
	cram->ReadCommitted(((paletteIndex.entry + (paletteIndex.line * paletteEntriesPerLine)) * 2), paletteEntry);
	const unsigned char paletteEntryTo8Bit[8] = {0, 36, 73, 109, 146, 182, 219, 255};
	const unsigned char paletteEntryTo8BitShadow[8] = {0, 18, 37, 55, 73, 91, 110, 128};
	const unsigned char paletteEntryTo8BitHighlight[8] = {128, 146, 165, 183, 201, 219, 238, 255};
	unsigned char r;
	unsigned char g;
	unsigned char b;
	if(paletteIndex.shadow && !paletteIndex.highlight)
	{
		r = paletteEntryTo8BitShadow[(paletteEntry.GetData() & 0x000E) >> 1];
		g = paletteEntryTo8BitShadow[(paletteEntry.GetData() & 0x00E0) >> 5];
		b = paletteEntryTo8BitShadow[(paletteEntry.GetData() & 0x0E00) >> 9];
	}
	else if(paletteIndex.highlight && !paletteIndex.shadow)
	{
		r = paletteEntryTo8BitHighlight[(paletteEntry.GetData() & 0x000E) >> 1];
		g = paletteEntryTo8BitHighlight[(paletteEntry.GetData() & 0x00E0) >> 5];
		b = paletteEntryTo8BitHighlight[(paletteEntry.GetData() & 0x0E00) >> 9];
	}
	else
	{
		r = paletteEntryTo8Bit[(paletteEntry.GetData() & 0x000E) >> 1];
		g = paletteEntryTo8Bit[(paletteEntry.GetData() & 0x00E0) >> 5];
		b = paletteEntryTo8Bit[(paletteEntry.GetData() & 0x0E00) >> 9];
	}

	//Write the final colour value to the image buffer
	unsigned int bufferPos = ((xpos + (((height - 1) * imageWidth) - (ypos * imageWidth))) * 4);
	if(bufferPos >= (imageWidth * imageWidth * 4))
	{
		//##DEBUG##
		std::wcout << "ERROR! Buffer overflow on image buffer\n";
		std::wcout << ypos << ' ' << xpos << '\n';
		system("pause");
	}
	image[drawingImageBufferPlane][bufferPos + 0] = r;
	image[drawingImageBufferPlane][bufferPos + 1] = g;
	image[drawingImageBufferPlane][bufferPos + 2] = b;
	image[drawingImageBufferPlane][bufferPos + 3] = 0xFF;
}

//----------------------------------------------------------------------------------------
void VDP::WriteColorToImageBuffer(const Color& color, unsigned int xpos, unsigned int ypos)
{
	//Assumed constants
	unsigned int height = 272;

	unsigned char r = color.GetR8Bit();
	unsigned char g = color.GetG8Bit();
	unsigned char b = color.GetB8Bit();

	//Write the final colour value to the image buffer
	unsigned int bufferPos = ((xpos + (((height - 1) * imageWidth) - (ypos * imageWidth))) * 4);
	if(bufferPos >= (imageWidth * imageWidth * 4))
	{
		//##DEBUG##
		std::wcout << "ERROR! Buffer overflow on image buffer\n";
		std::wcout << ypos << ' ' << xpos << '\n';
		system("pause");
	}
	image[drawingImageBufferPlane][bufferPos + 0] = r;
	image[drawingImageBufferPlane][bufferPos + 1] = g;
	image[drawingImageBufferPlane][bufferPos + 2] = b;
	image[drawingImageBufferPlane][bufferPos + 3] = 0xFF;
}

//----------------------------------------------------------------------------------------
//##TODO## Build in rollback support for port access. Add a committed flag to each entry.
void VDP::RecordPortMonitorEntry(const PortMonitorEntry& entry)
{
	boost::mutex::scoped_lock lock(portMonitorMutex);
	portMonitorList.push_front(entry);
	while(portMonitorList.size() > portMonitorListSize)
	{
		portMonitorList.pop_back();
	}
}

//----------------------------------------------------------------------------------------
void VDP::ClearPortMonitorList()
{
	boost::mutex::scoped_lock lock(portMonitorMutex);
	portMonitorList.clear();
}
