#include "S315_5313.h"
#include "DebugMenuHandler.h"
//##DEBUG##
#include <iostream>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
S315_5313::S315_5313(const std::wstring& ainstanceName, unsigned int amoduleID)
:Device(L"315-5313", ainstanceName, amoduleID),
reg(registerCount, false, Data(8)),
status(10),
bstatus(10),
hcounter(9),
bhcounter(9),
vcounter(9),
bvcounter(9),
hcounterLatchedData(9),
bhcounterLatchedData(9),
vcounterLatchedData(9),
bvcounterLatchedData(9),
readBuffer(16),
breadBuffer(16),
vsramCachedRead(16),
bvsramCachedRead(16),
originalCommandAddress(16),
boriginalCommandAddress(16),
commandAddress(16),
bcommandAddress(16),
commandCode(16),
bcommandCode(16),
dmaTransferReadCache(16),
bdmaTransferReadCache(16),
regSession(Data(8)),
vramSession(0),
cramSession(0),
vsramSession(0)
{
	//Create the menu handler
	menuHandler = new DebugMenuHandler(this);
	menuHandler->LoadMenuItems();

	fifoBuffer.resize(fifoBufferSize);
	bfifoBuffer.resize(fifoBufferSize);

	memoryBus = 0;
	vram = 0;
	cram = 0;
	vsram = 0;
	psg = 0;

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
	renderThreadActive = false;
	renderTimeslicePending = false;
	drawingImageBufferPlane = 0;

	busGranted = false;
	palModeLineState = false;
}

//----------------------------------------------------------------------------------------
S315_5313::~S315_5313()
{
	//Delete the menu handler
	menuHandler->ClearMenuItems();
	delete menuHandler;
}

//----------------------------------------------------------------------------------------
//Line functions
//----------------------------------------------------------------------------------------
unsigned int S315_5313::GetLineID(const wchar_t* lineName) const
{
	std::wstring lineNameString = lineName;
	if(lineNameString == L"IPL")		//O
	{
		return LINE_IPL;
	}
	else if(lineNameString == L"INT")	//O
	{
		return LINE_INT;
	}
	else if(lineNameString == L"INTAK")	//O
	{
		return LINE_INTAK;
	}
	else if(lineNameString == L"BR")	//O
	{
		return LINE_BR;
	}
	else if(lineNameString == L"BG")	//I
	{
		return LINE_BG;
	}
	else if(lineNameString == L"PAL")	//I
	{
		return LINE_PAL;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
const wchar_t* S315_5313::GetLineName(unsigned int lineID) const
{
	switch(lineID)
	{
	case LINE_IPL:
		return L"IPL";
	case LINE_INT:
		return L"INT";
	case LINE_INTAK:
		return L"INTAK";
	case LINE_BR:
		return L"BR";
	case LINE_BG:
		return L"BG";
	case LINE_PAL:
		return L"PAL";
	}
	return L"";
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::GetLineWidth(unsigned int lineID) const
{
	switch(lineID)
	{
	case LINE_IPL:
		return 3;
	case LINE_INT:
		return 1;
	case LINE_INTAK:
		return 1;
	case LINE_BR:
		return 1;
	case LINE_BG:
		return 1;
	case LINE_PAL:
		return 1;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void S315_5313::SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime)
{
	if(targetLine == LINE_INTAK)
	{
		//##DEBUG##
//		std::wcout << "SetLineState - VDP_LINE_INTAK:\t" << lineData.LSB() << '\n';

		memoryBus->SetLine(LINE_INTAK, Data(GetLineWidth(LINE_INTAK), 1), GetDeviceContext(), GetDeviceContext(), accessTime);
		memoryBus->SetLine(LINE_IPL, Data(GetLineWidth(LINE_IPL), 0), GetDeviceContext(), GetDeviceContext(), accessTime);
		//Since the interrupt has been acknowledged, clear the VINT occurance flag.
		SetStatusFlagF(false);
	}
	else if(targetLine == LINE_BG)
	{
		//##DEBUG##
//		std::wcout << "SetLineState - VDP_LINE_BG:\t" << lineData.LSB() << '\n';

		busGranted = lineData.LSB();
		busGrantTime = accessTime;
		workerThreadUpdate.notify_all();
	}
	else if(targetLine == LINE_PAL)
	{
		//##DEBUG##
//		std::wcout << "SetLineState - VDP_LINE_PAL:\t" << lineData.LSB() << '\n';

		palModeLineState = lineData.LSB();
	}
}

//----------------------------------------------------------------------------------------
//Initialization functions
//----------------------------------------------------------------------------------------
bool S315_5313::BuildDevice()
{
	//Initialize the layer priority lookup table. We use this table to speed up layer
	//priority selection during rendering.
	layerPriorityLookupTable.resize(layerPriorityLookupTableSize);
	for(unsigned int i = 0; i < layerPriorityLookupTableSize; ++i)
	{
		//Determine the input layer settings for this table index value
		bool shadowHighlightEnabled = (i & (1 << 8)) != 0;
		bool spriteIsShadowOperator = (i & (1 << 7)) != 0;
		bool spriteIsHighlightOperator = (i & (1 << 6)) != 0;
		bool foundSpritePixel = (i & (1 << 5)) != 0;
		bool foundLayerAPixel = (i & (1 << 4)) != 0;
		bool foundLayerBPixel = (i & (1 << 3)) != 0;
		bool prioritySprite = (i & (1 << 2)) != 0;
		bool priorityLayerA = (i & (1 << 1)) != 0;
		bool priorityLayerB = (i & 1) != 0;

		//Resolve the layer priority for this combination of layer settings
		unsigned int layerIndex;
		bool shadow;
		bool highlight;
		CalculateLayerPriorityIndex(layerIndex, shadow, highlight, shadowHighlightEnabled, spriteIsShadowOperator, spriteIsHighlightOperator, foundSpritePixel, foundLayerAPixel, foundLayerBPixel, prioritySprite, priorityLayerA, priorityLayerB);

		//Incorporate the shadow and highlight bits into the layer index value
		layerIndex |= shadow? 1 << 3: 0;
		layerIndex |= highlight? 1 << 2: 0;

		//Write the combined value to the layer priority lookup table
		layerPriorityLookupTable[i] = layerIndex;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool S315_5313::ValidateDevice()
{
	return (memoryBus != 0) && (vram != 0) && (cram != 0) && (vsram != 0);
}

//----------------------------------------------------------------------------------------
void S315_5313::Initialize()
{
	reg.Initialize();
	Reset();

	//Note that further testing has also shown that the VRAM is NOT initialized to all 0.
	//It appears the VRAM is initialized with a logical, repeating pattern, but further
	//tests must be carried out to determine what this pattern is. I would suggest reading
	//the entire contents of VRAM, CRAM, and VSRAM back into RAM for analysis.
	//##TODO## Confirm the default pattern for the VRAM
	vram->Initialize();
	for(unsigned int i = 0; i < vramSize; i += 2)
	{
		//##FIX## This is just a random value, not what the system actually has.
		vram->WriteLatest(i, 0xDE);
		vram->WriteLatest(i+1, 0xAD);
	}

	//The CRAM is initialized to 0x0EEE in all slots, except for the second entry in the
	//second palette line, which is initialized to 0x0CEE. This means byte 0x22 in the
	//CRAM is 0x0C instead of 0x0E.
	//##TODO## Confirm the default pattern for the CRAM
	cram->Initialize();
	for(unsigned int i = 0; i < cramSize; i += 2)
	{
		cram->WriteLatest(i, 0x0E);
		cram->WriteLatest(i+1, 0xEE);
	}
	cram->WriteLatest(0x22, 0x0C);
	cram->WriteLatest(0x23, 0xEE);

	//The VSRAM is initialized to 0x07FF, except for the entry at 0x00, which is 0x07DF,
	//and the entry at 0x22, which is 0x07FB.
	//##TODO## Confirm the default pattern for the VSRAM
	vsram->Initialize();
	for(unsigned int i = 0; i < vsramSize; i += 2)
	{
		vsram->WriteLatest(i, 0x07);
		vsram->WriteLatest(i+1, 0xFF);
	}
	vsram->WriteLatest(0x00, 0x07);
	vsram->WriteLatest(0x01, 0xDF);
	vsram->WriteLatest(0x22, 0x07);
	vsram->WriteLatest(0x23, 0xFB);

	currentTimesliceLength = 0;
	currentTimesliceMclkCyclesRemainingTime = 0;
	stateLastUpdateTime = 0;
	lastTimesliceRemainingTime = 0;
	stateLastUpdateMclk = 0;
	stateLastUpdateMclkUnused = 0;
	stateLastUpdateMclkUnusedFromLastTimeslice = 0;
	eventLastUpdateMclk = 0;

	externalInterruptPending = false;

	//##TODO## Initialize the actual data in the FIFO buffer. We can determine the default
	//values for many bits in the FIFO on power-up by immediately setting up a read target
	//from VSRAM and CRAM, and stepping through each entry in the FIFO, saving the values
	//we read out of the initial bits. Most likely, the FIFO is initialized to 0, but we
	//should perform a test to be certain.
	fifoNextReadEntry = 0;
	fifoNextWriteEntry = 0;
	commandWritePending = false;
	for(unsigned int i = 0; i < fifoBufferSize; ++i)
	{
		fifoBuffer[i].codeRegData = 0;
		fifoBuffer[i].addressRegData = 0;
		fifoBuffer[i].dataPortWriteData = 0;
		fifoBuffer[i].dataWriteHalfWritten = false;
		fifoBuffer[i].pendingDataWrite = false;
	}
	vsramCachedRead = 0;
	readDataAvailable = false;
	readDataHalfCached = false;
	dmaFillOperationRunning = false;

	workerThreadPaused = false;
	dmaTransferActive = false;
	busGranted = false;
	palModeLineState = true;
	interlaceEnabled = false;
	interlaceDouble = false;
	screenModeH40 = false;
	screenModeV30 = false;
	palMode = true;
	eventSnapshotSaved = false;
}

//----------------------------------------------------------------------------------------
void S315_5313::Reset()
{
	//After numerous tests performed on the actual system, it appears that all VDP
	//registers are initialized to 0 on a reset. This is supported by Addendum 4 from
	//SEGA in the Genesis Software Manual. Note that since Mode 4 graphics are enabled
	//when bit 2 of reg 1 is 0, the VDP starts in Mode 4. This has been confirmed on the
	//real hardware.
	for(unsigned int i = 0; i < registerCount; ++i)
	{
		SetRegisterData(0, AccessTarget().AccessLatest(), Data(8, 0));
	}
	vintEnabled = false;
	hintEnabled = false;
	exintEnabled = false;
	hintCounterReloadValue = 0;
	dmaEnabled = false;
	dmd0 = false;
	dmd1 = false;
	dmaLengthCounter = 0;
	dmaSourceAddressByte1 = 0;
	dmaSourceAddressByte2 = 0;
	dmaSourceAddressByte3 = 0;
	autoIncrementData = 0;
	interlaceEnabledCached = false;
	interlaceDoubleCached = false;
	screenModeH40Cached = false;
	screenModeV30Cached = false;
	cachedDMASettingsChanged = false;

	//##TODO## Check if we need to clear these here
	status = 0;
	hcounter = 0;
	vcounter = 0;
	hvCounterLatched = false;
	hintCounter = 0;
	hintCounterReloadValue = 0;
	hintEnabled = false;
	exintEnabled = false;

	//Initialize the command port data
	//##TODO## Test to see if these values are actually cleared on a reset.
	fifoNextReadEntry = 0;
	fifoNextWriteEntry = 0;
	commandWritePending = false;
	for(unsigned int i = 0; i < fifoBufferSize; ++i)
	{
		fifoBuffer[i].codeRegData = 0;
		fifoBuffer[i].addressRegData = 0;
		fifoBuffer[i].dataPortWriteData = 0;
		fifoBuffer[i].dataWriteHalfWritten = false;
		fifoBuffer[i].pendingDataWrite = false;
	}
	vsramCachedRead = 0;
	readDataAvailable = false;
	readDataHalfCached = false;
	dmaFillOperationRunning = false;

	lineLWRSavedStateRMW = false;
	lineUWRSavedStateRMW = false;
	lineCAS0SavedStateRMW = false;
	lineRAS0SavedStateRMW = false;
	lineOE0SavedStateRMW = false;
}

//----------------------------------------------------------------------------------------
void S315_5313::BeginExecution()
{
	//Start the render worker thread
	renderThreadActive = true;
	boost::thread renderThread(boost::bind(boost::mem_fn(&S315_5313::RenderThread), this));

	//Start the DMA worker thread
	workerThreadActive = true;
	boost::thread workerThread(boost::bind(boost::mem_fn(&S315_5313::DMAWorkerThread), this));
}

//----------------------------------------------------------------------------------------
void S315_5313::SuspendExecution()
{
	//Suspend the render worker thread
	boost::mutex::scoped_lock renderLock(renderThreadMutex);
	if(renderThreadActive)
	{
		renderThreadActive = false;
		renderThreadUpdate.notify_all();
		renderThreadStopped.wait(renderLock);
	}

	//Suspend the DMA worker thread
	boost::mutex::scoped_lock workerLock(workerThreadMutex);
	if(workerThreadActive)
	{
		workerThreadActive = false;
		workerThreadUpdate.notify_all();
		workerThreadStopped.wait(workerLock);
	}
}

//----------------------------------------------------------------------------------------
//Reference functions
//----------------------------------------------------------------------------------------
bool S315_5313::AddReference(const wchar_t* referenceName, IBusInterface* target)
{
	std::wstring referenceNameString = referenceName;
	if(referenceNameString == L"BusInterface")
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
bool S315_5313::AddReference(const wchar_t* referenceName, IDevice* target)
{
	std::wstring referenceNameString = referenceName;
	if(referenceNameString == L"VRAM")
	{
		ITimedBufferIntDevice* device = dynamic_cast<ITimedBufferIntDevice*>(target);
		if(device != 0)
		{
			vram = device->GetTimedBuffer();
		}
	}
	else if(referenceNameString == L"CRAM")
	{
		ITimedBufferIntDevice* device = dynamic_cast<ITimedBufferIntDevice*>(target);
		if(device != 0)
		{
			cram = device->GetTimedBuffer();
		}
	}
	else if(referenceNameString == L"VSRAM")
	{
		ITimedBufferIntDevice* device = dynamic_cast<ITimedBufferIntDevice*>(target);
		if(device != 0)
		{
			vsram = device->GetTimedBuffer();
		}
	}
	else if(referenceNameString == L"PSG")
	{
		psg = target;
	}
	else
	{
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------------------
bool S315_5313::RemoveReference(IBusInterface* target)
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
bool S315_5313::RemoveReference(IDevice* target)
{
	if((IDevice*)vram == target)
	{
		vram = 0;
	}
	else if((IDevice*)cram == target)
	{
		cram = 0;
	}
	else if((IDevice*)vsram == target)
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
//Execute functions
//----------------------------------------------------------------------------------------
S315_5313::UpdateMethod S315_5313::GetUpdateMethod() const
{
	return UPDATEMETHOD_TIMESLICE;
}

//----------------------------------------------------------------------------------------
bool S315_5313::SendNotifyUpcomingTimeslice() const
{
	return true;
}

//----------------------------------------------------------------------------------------
void S315_5313::NotifyUpcomingTimeslice(double nanoseconds)
{
	stateLastUpdateTime = 0;
	stateLastUpdateMclk = 0;
	stateLastUpdateMclkUnused = 0;

	currentTimesliceLength = nanoseconds;
	eventsProcessedForTimeslice = false;
	eventLastUpdateMclk = 0;

	lastAccessTime = 0;

	//##DEBUG##
	double currentTimesliceMclkCyclesRemainingTimeOriginal = currentTimesliceMclkCyclesRemainingTime;
	currentTimesliceTotalMclkCycles = ConvertAccessTimeToMclkCount(currentTimesliceLength + currentTimesliceMclkCyclesRemainingTimeOriginal);
	double mclkCyclesToAddInAccessTime = ConvertMclkCountToAccessTime(currentTimesliceTotalMclkCycles);
	currentTimesliceMclkCyclesRemainingTime = (currentTimesliceLength + currentTimesliceMclkCyclesRemainingTimeOriginal) - mclkCyclesToAddInAccessTime;

	//Round off error adjustment code in ConvertAccessTimeToMclkCount can result in
	//currentTimesliceTotalMclkCycles being rounded up. In this case, our
	//currentTimesliceMclkCyclesRemainingTime variable would end up with a negative
	//result, the presence of which would bias us towards over-estimating the number of
	//mclk cycles for the next timeslice too, resulting in a compounding error, with the
	//numbers getting further and further off with each successive timeslice. We
	//compensate for the negative error here when it occurs, by removing one mclk cycle to
	//force a positive result.
	while(currentTimesliceMclkCyclesRemainingTime < 0)
	{
		//##DEBUG##
//		std::wcout << "######################################################\n";
//		std::wcout << "VDP NotifyUpcomingTimeslice resulted in negative mclkCyclesRemainingTime!\n";
//		std::wcout << "nanoseconds:\t" << nanoseconds << "\n";
//		std::wcout << "currentTimesliceTotalMclkCycles:\t" << currentTimesliceTotalMclkCycles << "\n";
//		std::wcout << "mclkCyclesToAddInAccessTime:\t" << mclkCyclesToAddInAccessTime << "\n";
//		std::wcout << "currentTimesliceMclkCyclesRemainingTime:\t" << currentTimesliceMclkCyclesRemainingTime << "\n";
//		std::wcout << "currentTimesliceMclkCyclesRemainingTimeOriginal:\t" << currentTimesliceMclkCyclesRemainingTimeOriginal << "\n";
//		std::wcout << "######################################################\n";
		if(currentTimesliceTotalMclkCycles > 0)
		{
			currentTimesliceTotalMclkCycles -= 1;
			mclkCyclesToAddInAccessTime = ConvertMclkCountToAccessTime(currentTimesliceTotalMclkCycles);
		}
		currentTimesliceMclkCyclesRemainingTime = (currentTimesliceLength + currentTimesliceMclkCyclesRemainingTimeOriginal) - mclkCyclesToAddInAccessTime;
	}

	//##DEBUG##
//	std::wcout << "VDPNotifyUpcomingTimeslice:\t" << currentTimesliceLength << '\t' << currentTimesliceTotalMclkCycles << '\t' << mclkCyclesToAddInAccessTime << '\t' << currentTimesliceMclkCyclesRemainingTime << '\n';

	reg.AddTimeslice(currentTimesliceTotalMclkCycles);
	vram->AddTimeslice(currentTimesliceTotalMclkCycles);
	cram->AddTimeslice(currentTimesliceTotalMclkCycles);
	vsram->AddTimeslice(currentTimesliceTotalMclkCycles);
}

//----------------------------------------------------------------------------------------
void S315_5313::NotifyBeforeExecuteCalled()
{
	//If the DMA worker thread is currently active but paused, resume it here.
	boost::mutex::scoped_lock lock(workerThreadMutex);
	if(workerThreadActive && workerThreadPaused)
	{
		//##DEBUG##
//		std::wcout << L"NotifyBeforeExecuteCalled is resuming DMA worker thread\n";

		workerThreadUpdate.notify_all();
	}
}

//----------------------------------------------------------------------------------------
bool S315_5313::SendNotifyBeforeExecuteCalled() const
{
	return true;
}

//----------------------------------------------------------------------------------------
void S315_5313::NotifyAfterExecuteCalled()
{
	//Ensure that the DMA worker thread has finished executing
	boost::mutex::scoped_lock lock(workerThreadMutex);
	if(workerThreadActive && !workerThreadPaused && busGranted)
	{
		//##DEBUG##
//		std::wcout << L"NotifyAfterExecuteCalled is waiting for DMA worker thread to pause\n";
//		std::wcout << '\t' << workerThreadActive << '\t' << workerThreadPaused << '\t' << busGranted << '\n';

		workerThreadIdle.wait(lock);
	}

	//##DEBUG##
//	std::wcout << "VDP - NotifyAfterExecuteCalled(Before): " << hcounter.GetData() << '\t' << vcounter.GetData() << '\t' << stateLastUpdateMclk << '\t' << stateLastUpdateMclkUnused << '\t' << std::setprecision(16) << stateLastUpdateTime << '\n';

	//Ensure the VDP is advanced up to the end of its timeslice
	double lastTimesliceTotalExecuteTime = currentTimesliceLength + lastTimesliceRemainingTime;
	if((lastTimesliceTotalExecuteTime > 0) && (lastTimesliceTotalExecuteTime > GetProcessorStateTime()))
	{
		UpdateInternalState(ConvertAccessTimeToMclkCount(lastTimesliceTotalExecuteTime), false, false, false, false, false, false, false);
	}
	//##TODO## Remove this old code
	//if((currentTimesliceTotalMclkCycles > 0) && (currentTimesliceTotalMclkCycles > GetProcessorStateMclkCurrent()))
	//{
	//	UpdateInternalState(currentTimesliceTotalMclkCycles, false, false, false, false, false, false);
	//}

	//##DEBUG##
//	std::wcout << "VDP - NotifyAfterExecuteCalled(After): " << hcounter.GetData() << '\t' << vcounter.GetData() << '\t' << stateLastUpdateMclk << '\t' << stateLastUpdateMclkUnused << '\t' << std::setprecision(16) << stateLastUpdateTime << '\n';
//	double lastTimesliceRemainingTimeOriginal = lastTimesliceRemainingTime;

	//Record any unused mclk cycles from this timeslice, so we can pass them over into the
	//next timeslice.
	stateLastUpdateMclkUnusedFromLastTimeslice += stateLastUpdateMclkUnused;

	//Record the unused portion of the last timeslice which needs to be added to the next
	//timeslice, and factored in when predicting the next timing point.
//	lastTimesliceRemainingTime = lastTimesliceTotalExecuteTime - GetProcessorStateTime();
	lastTimesliceRemainingTime = (currentTimesliceLength + lastTimesliceRemainingTime) - GetProcessorStateTime();

	//If a DMA transfer is in progress, update the number of remaining mclk cycles from
	//the end of this timeslice which need to be factored in on the next update step for
	//the DMA transfer operation, and reset the last read time to 0. This is necessary, in
	//order to ensure the next read operation for the DMA transfer operation takes place
	//at the correct time.
	if(dmaTransferActive)
	{
		dmaTransferLastTimesliceUnusedMclkCycles = GetProcessorStateMclkCurrent() - dmaTransferNextReadMclk;
		dmaTransferNextReadMclk = 0;
	}

	//##DEBUG##
	//##NOTE## This isn't actually an error. The currentTimesliceMclkCyclesRemainingTime
	//variable will actually always be less than or equal to 0, since we always advance to
	//at least the end of the current timeslice, but we may advance past it, if for
	//example an accessing device tries to write to the data port when the FIFO is full,
	//and needs to block waiting for a slot to open up.
//	if(lastTimesliceRemainingTime < 0)
//	{
//		std::wcout << "######################################################\n";
//		std::wcout << "VDP lastTimesliceRemainingTime < 0!\n";
//		std::wcout << "lastTimesliceRemainingTimeOriginal:\t" << lastTimesliceRemainingTimeOriginal << "\n";
//		std::wcout << "lastTimesliceRemainingTime:\t" << lastTimesliceRemainingTime << "\n";
//		std::wcout << "currentTimesliceLength:\t" << currentTimesliceLength << "\n";
//		std::wcout << "GetProcessorStateTime:\t" << GetProcessorStateTime() << "\n";
//		std::wcout << "######################################################\n";
//	}
}

//----------------------------------------------------------------------------------------
bool S315_5313::SendNotifyAfterExecuteCalled() const
{
	return true;
}

//----------------------------------------------------------------------------------------
void S315_5313::ExecuteTimeslice(double nanoseconds)
{
	//Obtain a lock on the accessMutex, to prevent port access interfering with this
	//operation.
	boost::mutex::scoped_lock lock(accessMutex);

	//##FIX## This magic condition here is to detect when ExecuteTimeslice has been called
	//in order to step through a timing point, as opposed to a normal execute step. I
	//think we should modify our emulation platform to call a separate function for a
	//timing point event than for a normal execute event.
	if((eventLastUpdateMclk > 0) && (nanoseconds == 0))
	{
		//##DEBUG##
//		std::wcout << "VDP - ExecuteOnTimingPoint: " << hcounter.GetData() << '\t' << vcounter.GetData() << '\t' << stateLastUpdateMclkUnused << '\t' << nextEventToExecute.event << '\t' << nextEventToExecute.mclkCycleCounter << '\t' << std::setprecision(16) << nextEventToExecuteTime << '\n';

		if(eventSnapshotSaved)
		{
			ExecuteEvent(nextEventToExecute, nextEventToExecuteTime, nextEventToExecute.hcounter, nextEventToExecute.vcounter, eventSnapshotScreenModeH40, eventSnapshotScreenModeV30, eventSnapshotPalMode, eventSnapshotInterlaceEnabled);
			eventSnapshotSaved = false;
		}
		else if((hcounter.GetData() == nextEventToExecute.hcounter) && (vcounter.GetData() == nextEventToExecute.vcounter))
		{
			ExecuteEvent(nextEventToExecute, nextEventToExecuteTime, nextEventToExecute.hcounter, nextEventToExecute.vcounter, screenModeH40, screenModeV30, palMode, interlaceEnabled);
		}
		else
		{
			//##FIX## We have a major problem with our timing system. Timing points can be
			//missed, because a calling device may access the VDP just before the end of
			//the current timeslice, with the VDP having a timing point at the end of the
			//timeslice, and the call itself may need to be blocked for a period of time
			//that moves past the timing point. In our current execution model, we will
			//allow the call to proceed, and advance the VDP to the end of that call, even
			//if the call passes the end of the current timeslice. This works fine for
			//devices which can only be accessed by a single device at a time, however, it
			//creates a problem with our timing points, because the VDP is essentially
			//multi-threaded, and while the calling device would be stalled at the time
			//the timing point occurs, that timing point should still execute on time.
			//##TODO## What we need to do to fix our update dilemma, is detect the case
			//where we would skip a timing point due to an access call, and during the
			//update process, stop at that timing point, and snapshot all the state data
			//we need to capture at that point in order to execute the timing point. With
			//that state data captured, we then proceed as normal, but when the timing
			//point is executed here, we should retrieve the saved state information we
			//need for that event, and process it using the saved state information. This
			//will solve our threading problem perfectly. Note that we should still keep a
			//debug check like the one below for the case where no event information was
			//latched and saved. This could occur if we actually failed to reach a timing
			//point, rather than advancing past it.
			//##DEBUG##
			std::wcout << "######################################################\n";
			std::wcout << "VDP Didn't reach timing point!!:\n";
			std::wcout << "           Actual\tTarget\n";
			std::wcout << "HCounter   " << hcounter.GetData() << "\t\t" << nextEventToExecute.hcounter << '\n';
			std::wcout << "VCounter   " << vcounter.GetData() << "\t\t" << nextEventToExecute.vcounter << '\n';
			std::wcout << "######################################################\n";
		}
	}
	else
	{
		//Add in any unused time from the previous timeslice
		//##TODO## This seems to be wrong? Compare how we treat remaining time between
		//this and the nextEventToExecuteTime variable. It seems that the
		//nextEventToExecuteTime variable is in system time, while this is in device time?
		nanoseconds += lastTimesliceRemainingTime;

		//While an interrupt event is pending from this timeslice, process it.
		EventProperties nextEvent;
		GetNextEvent(eventLastUpdateMclk, false, nextEvent);
		double nextEventTime;
		nextEventTime = ConvertMclkCountToAccessTime(nextEvent.mclkCycleCounter);
		while((nextEventTime < nanoseconds) && (nextEvent.mclkCycleCounter < nextEventToExecute.mclkCycleCounter))
		{
			//##DEBUG##
//			std::wcout << "NextEvent: " << eventLastUpdateMclk << '\t' << nanoseconds << '\t' << nextEvent << '\t' << nextEventTime << '\n';
//			std::wcout << "NextEvent: " << nextEventFound << '\t' << eventLastUpdateMclk << '\t' << nextEvent << '\t' << nextEventMclkCounter << '\t' << std::setprecision(16) << nextEventTime << '\t' << std::setprecision(16) << nanoseconds << '\n';

			ExecuteEvent(nextEvent, nextEventTime, nextEvent.hcounter, nextEvent.vcounter, screenModeH40, screenModeV30, palMode, interlaceEnabled);

			eventLastUpdateMclk = nextEvent.mclkCycleCounter;
			GetNextEvent(eventLastUpdateMclk+1, false, nextEvent);
			nextEventTime = ConvertMclkCountToAccessTime(nextEvent.mclkCycleCounter);
		}
		//std::wcout << "NextEvent: " << nextEventFound << '\t' << eventLastUpdateMclk << '\t' << nextEvent << '\t' << nextEventMclkCounter << '\t' << nextEventTime << '\t' << nanoseconds << '\n';

		//##FIX## What time should we record when we stop before the indicated timeslice
		//due to round off error, when the nextEventToExecuteMclkCounter value aborts the
		//search?
		eventLastUpdateMclk = ConvertAccessTimeToMclkCount(nanoseconds);
	}

	//Release the block on port access now that events have been processed. This locking
	//mechanism is in place because we need to make sure that the device state remains
	//fixed at the beginning of the timeslice until all events have been processed, in
	//order to ensure that we can accurately calculate the time when the events occur.
	eventsProcessedForTimeslice = true;
	eventProcessingCompleted.notify_all();
}

//----------------------------------------------------------------------------------------
void S315_5313::GetNextEvent(unsigned int currentMclkCycleCounter, bool timingPointsOnly, EventProperties& nextEvent) const
{
	//Adjust the cycle count to factor in unused state mclk update cycles
	currentMclkCycleCounter += stateLastUpdateMclkUnused;

	//Obtain information on the current processor state
	const HScanSettings& hscanSettings = GetHScanSettings(screenModeH40);
	const VScanSettings& vscanSettings = GetVScanSettings(screenModeV30, palMode, interlaceEnabled);

	//Start a search for the next event
	bool skippedEvent = false;
	unsigned int currentPosHCounter = hcounter.GetData();
	unsigned int currentPosVCounter = vcounter.GetData();
	unsigned int skippedMclkCycles = 0;
	do
	{
		Event nextEventToOccur;
		unsigned int eventPosHCounter;
		unsigned int eventPosVCounter;
		bool eventGeneratesTimingPoint;

		bool oddFlagSet = GetStatusFlagOddInterlaceFrame();

		//Start with hint counter advance as the next event
		unsigned int hintCounterAdvanceEventPosHCounter = hscanSettings.hintTriggerPoint;
		unsigned int hintCounterAdvanceEventPosVCounter;
		if((currentPosHCounter < hscanSettings.hintTriggerPoint) || (currentPosHCounter >= hscanSettings.vcounterIncrementPoint))
		{
			hintCounterAdvanceEventPosVCounter = currentPosVCounter;
		}
		else
		{
			hintCounterAdvanceEventPosVCounter = AddStepsToVCounter(hscanSettings, currentPosHCounter, vscanSettings, interlaceEnabled, oddFlagSet, currentPosVCounter, 1);
		}
		nextEventToOccur = NEXTUPDATESTEP_HINTCOUNTERADVANCE;
		eventPosHCounter = hintCounterAdvanceEventPosHCounter;
		eventPosVCounter = hintCounterAdvanceEventPosVCounter;

		//Only generate a timing point here if hint is enabled, the counter is going to
		//expire if it is advanced one more slot, and the line on which the update will
		//occur is in the active scan area.
		if(hintEnabled && (hintCounter == 0) && (hintCounterAdvanceEventPosVCounter < vscanSettings.vblankSetPoint))
		{
			eventGeneratesTimingPoint = true;
		}
		else
		{
			eventGeneratesTimingPoint = false;
		}

		//Check if VINT is going to occur before the earliest event found
		unsigned int vintEventPosHCounter = hscanSettings.fflagSetPoint;
		unsigned int vintEventPosVCounter = vscanSettings.vblankSetPoint;
		if(((vintEventPosHCounter != currentPosHCounter) || (vintEventPosVCounter != currentPosVCounter))
		&& EventOccursWithinCounterRange(hscanSettings, currentPosHCounter, currentPosVCounter, eventPosHCounter, eventPosVCounter, vintEventPosHCounter, vintEventPosVCounter))
		{
			//VINT is the next event
			nextEventToOccur = NEXTUPDATESTEP_VINT;
			eventPosHCounter = vintEventPosHCounter;
			eventPosVCounter = vintEventPosVCounter;
			eventGeneratesTimingPoint = true;
		}

		//Check if vsync is going to occur before the earliest event found
		unsigned int vsyncEventPosHCounter = hscanSettings.vcounterIncrementPoint;
		unsigned int vsyncEventPosVCounter = vscanSettings.vsyncAssertedPoint;
		if(((vsyncEventPosHCounter != currentPosHCounter) || (vsyncEventPosVCounter != currentPosVCounter))
		&& EventOccursWithinCounterRange(hscanSettings, currentPosHCounter, currentPosVCounter, eventPosHCounter, eventPosVCounter, vsyncEventPosHCounter, vsyncEventPosVCounter))
		{
			//VSYNC is the next event
			nextEventToOccur = NEXTUPDATESTEP_VSYNC;
			eventPosHCounter = vsyncEventPosHCounter;
			eventPosVCounter = vsyncEventPosVCounter;
			eventGeneratesTimingPoint = true;
		}

		//Calculate the number of pixel clock steps between now and the target event
		unsigned int pixelClockStepsUntilNextEvent = 0;
		pixelClockStepsUntilNextEvent = GetPixelClockStepsBetweenHVCounterValues(hscanSettings, currentPosHCounter, eventPosHCounter, vscanSettings, interlaceEnabled, oddFlagSet, currentPosVCounter, eventPosVCounter);

		//Calculate the number of mclk cycles which correspond to the number of pixel
		//clock cycles until the next event.
		unsigned int mclkStepsUntilNextEvent = GetMclkTicksForPixelClockTicks(pixelClockStepsUntilNextEvent, currentPosHCounter, screenModeH40);

		//Calculate the final mclk cycle counter value when the target event is reached
		unsigned int mclkCounterAtNextEvent = mclkStepsUntilNextEvent + skippedMclkCycles;

		//If the next event we've found occurs before the start position for the event
		//search, or we're looking for timing point events and the next event doesn't
		//generate a timing point, skip it and look for the next event after this one.
		if((mclkCounterAtNextEvent < currentMclkCycleCounter) || (timingPointsOnly && !eventGeneratesTimingPoint))
		{
			skippedEvent = true;
			currentPosHCounter = eventPosHCounter;
			currentPosVCounter = eventPosVCounter;
			skippedMclkCycles = mclkCounterAtNextEvent;
			continue;
		}

		//Return the event info
		skippedEvent = false;
		nextEvent.event = nextEventToOccur;
		nextEvent.mclkCycleCounter = mclkCounterAtNextEvent;
		nextEvent.hcounter = eventPosHCounter;
		nextEvent.vcounter = eventPosVCounter;

		//Adjust the cycle count to factor in unused state mclk update cycles
		nextEvent.mclkCycleCounter -= stateLastUpdateMclkUnused;

		//##DEBUG##
//		if(timingPointsOnly && !skippedEvent)
//		{
//			std::wcout << "VDP - GetNextEvent: " << hcounter.GetData() << '\t' << vcounter.GetData() << '\t' << stateLastUpdateMclkUnused << '\t' << eventPosHCounter << '\t' << eventPosVCounter << '\n';
//		}
	}
	while(skippedEvent);
}

//----------------------------------------------------------------------------------------
bool S315_5313::EventOccursWithinCounterRange(const HScanSettings& hscanSettings, unsigned int hcounterStart, unsigned int vcounterStart, unsigned int hcounterEnd, unsigned int vcounterEnd, unsigned int hcounterEventPos, unsigned int vcounterEventPos) const
{
	return (((vcounterStart < vcounterEventPos)
		  || ((vcounterStart == vcounterEventPos)
		   && (((hcounterStart < hscanSettings.vcounterIncrementPoint)
		     && (hcounterEventPos < hscanSettings.vcounterIncrementPoint)
			 && (hcounterStart <= hcounterEventPos))
			|| ((hcounterStart >= hscanSettings.vcounterIncrementPoint)
			 && (hcounterEventPos >= hscanSettings.vcounterIncrementPoint)
			 && (hcounterStart <= hcounterEventPos))
			|| ((hcounterStart >= hscanSettings.vcounterIncrementPoint)
			 && (hcounterEventPos < hscanSettings.vcounterIncrementPoint)
			)))) //The target event occurs at or after the start position
		 && ((vcounterEnd > vcounterEventPos)
		  || ((vcounterEnd == vcounterEventPos)
		   && (((hcounterEnd < hscanSettings.vcounterIncrementPoint)
		     && (hcounterEventPos < hscanSettings.vcounterIncrementPoint))
			 && (hcounterEnd >= hcounterEventPos)
		    || ((hcounterEnd >= hscanSettings.vcounterIncrementPoint)
			 && (hcounterEventPos >= hscanSettings.vcounterIncrementPoint))
			 && (hcounterEnd >= hcounterEventPos)
			|| ((hcounterEventPos >= hscanSettings.vcounterIncrementPoint)
			 && (hcounterEnd < hscanSettings.vcounterIncrementPoint)
		   ))))); //The target event occurs at or after the end position
}

//----------------------------------------------------------------------------------------
void S315_5313::ExecuteEvent(EventProperties eventProperties, double accessTime, unsigned int ahcounter, unsigned int avcounter, bool ascreenModeH40, bool ascreenModeV30, bool apalMode, bool ainterlaceEnabled)
{
	//Assumed constants
	//##TODO## Shift this into HScanSettings or the like
	const unsigned int mclkCyclesPerLine = 3420;

	//Obtain information on the current processor state
	const HScanSettings& hscanSettings = GetHScanSettings(ascreenModeH40);
	const VScanSettings& vscanSettings = GetVScanSettings(ascreenModeV30, apalMode, ainterlaceEnabled);

	//Perform the next update step
	switch(eventProperties.event)
	{
	//Vertical interrupt
	case NEXTUPDATESTEP_VINT:{
		//Set the VINT occurrence flag for the status register. Note that this flag is set
		//even if vertical interrupts are disabled. If vertical interrupts are enabled,
		//this bit is cleared when the interrupt is acknowledged by the M68000, otherwise
		//this bit remains set from this point on, until an actual vertical interrupt is
		//generated and acknowledged by the M68000. This behaviour has been confirmed
		//through hardware tests.
		SetStatusFlagF(true);

		//Trigger the vertical interrupt for the M68000 if vertical interrupts are enabled
		vintPending = true;
		if(vintEnabled && vintPending)
		{
			//##DEBUG##
//			std::wcout << "VDP - VINT: " << hcounter.GetData() << '\t' << vcounter.GetData() << '\n';

			//Trigger VInt for M68000
			memoryBus->SetLine(LINE_IPL, Data(GetLineWidth(LINE_IPL), vintIPLLineState), GetDeviceContext(), GetDeviceContext(), accessTime);
			vintPending = false;
		}

		//Trigger the vertical interrupt line for the Z80. Note that this line is always
		//asserted, regardless of the state of the vertical interrupt enable flag.
		//##NOTE## According to Eke, the Z80 INT line is asserted for one scanline, and is
		//cleared at the same point in the following line.
		double intLineAssertedLength = ConvertMclkCountToAccessTime(mclkCyclesPerLine);
		memoryBus->SetLine(LINE_INT, Data(GetLineWidth(LINE_INT), 1), GetDeviceContext(), GetDeviceContext(), accessTime);
		memoryBus->SetLine(LINE_INT, Data(GetLineWidth(LINE_INT), 0), GetDeviceContext(), GetDeviceContext(), accessTime + intLineAssertedLength);
		break;}
	//External interrupt
	case NEXTUPDATESTEP_EXINT:{
		//Latch the current hcounter and vcounter settings, if HV counter latching has
		//been enabled.
		//##TODO## Determine when the hvCounterLatched bit should be cleared. Is it
		//cleared after the HV counter is read?
		//##TODO## Confirm what happens with the latched HV data when the interlace mode
		//changes. Is it the internal value of the hcounter which is latched, or the
		//external value?
		//##TODO## Read the info from Eke at http://gendev.spritesmind.net/forum/viewtopic.php?t=787
		//It says that setting the HV counter latch bit will always cause the HV counter
		//to return a static value. It will be initialized to an unknown default value,
		//and will be updated when HL is asserted. Confirm whether toggling HV counter
		//latch from disabled to enabled latches the current counter value at the time it
		//is enabled. Also confirm whether latching a value, disabling latching, then
		//re-enabling latching returns the same previously latched value.
		//##TODO## Confirm whether the VDP always latches the HV counter when the HL input
		//line is asserted, but that value is only reported when the latch enable bit is
		//set. Try triggering a HL latch for a known value when HV counter latch is
		//disabled, then enable the HV counter latch, and see if the initial latch value
		//is the same as the masked time when the HL trigger occurred.
		if(hvCounterLatchEnabled)
		{
			hvCounterLatched = true;
			hcounterLatchedData = ahcounter;
			vcounterLatchedData = avcounter;
		}

		//Trigger the external interrupt for the M68000 if external interrupts are
		//enabled.
		exintPending = true;
		if(exintEnabled && exintPending)
		{
			//##DEBUG##
//			std::wcout << "VDP - EXINT: " << hcounter.GetData() << '\t' << vcounter.GetData() << '\n';

			//Trigger EXInt for M68000
			memoryBus->SetLine(LINE_IPL, Data(GetLineWidth(LINE_IPL), exintIPLLineState), GetDeviceContext(), GetDeviceContext(), accessTime);
			exintPending = false;
		}
		break;}
	//VSync
	case NEXTUPDATESTEP_VSYNC:{
		//Since we've reached vsync, we should now have a complete frame worth of data to
		//render. As such, flag a pending render operation to trigger rendering of the
		//completed frame.
		pendingRenderOperation = true;
		break;}
	//Hint counter advance
	case NEXTUPDATESTEP_HINTCOUNTERADVANCE:{
		if(eventProperties.vcounter >= vscanSettings.vblankSetPoint)
		{
			//Latch the initial hintCounter value for the frame
			hintCounter = hintCounterReloadValue;
		}
		else if(hintCounter == 0)
		{
			//Reload the hint counter now that it has expired
			hintCounter = hintCounterReloadValue;

			//Trigger the horizontal interrupt for the M68000 if horizontal interrupts are
			//enabled.
			//##TODO## Clean up the old comment below
			//##FIX## According to tests performed by Eke, this implementation is wrong. What
			//actually happens is that an internal hint pending flag is set, which remains set
			//until an interrupt is both triggered and acknowledged. The interrupt is only
			//triggered when the interrupt enable bit is set, so in other words, as soon as
			//we enable horizontal interrupts on the VDP, a HINT is probably going to be
			//triggered. Note that the VINT implementation is identical, with a vint pending
			//flag held. We would need to test and confirm the behaviour of EXINT, but it most
			//likely works the same way. Also note that changes to the interrupt enable bits,
			//IE, setting it as cleared again when it is already cleared. What actually will
			//be happening in hardware is the pending interrupt flags always get set, and only
			//cleared when an interrupt is acknowledged. The VDP decides whether to assert an
			//interrupt line by taking the pending interrupt flag, and ANDing it with the
			//interrupt enable bit. We should follow this same implementation in our core.
			hintPending = true;
			if(hintEnabled && hintPending)
			{
				//##DEBUG##
//				std::wcout << "VDP - HINT: " << hcounter.GetData() << '\t' << vcounter.GetData() << '\n';

				//Trigger HInt for M68000
				memoryBus->SetLine(LINE_IPL, Data(GetLineWidth(LINE_IPL), hintIPLLineState), GetDeviceContext(), GetDeviceContext(), accessTime);
				hintPending = false;
			}
		}
		else
		{
			--hintCounter;
		}
		break;}
	default:{
		//##TODO## Raise an assert here
		break;}
	}
}

//----------------------------------------------------------------------------------------
double S315_5313::GetNextTimingPointInDeviceTime() const
{
	//Determine the details of the next timing point event
	GetNextEvent(0, true, nextEventToExecute);

	//##FIX## I think this comment is misleading. lastTimesliceRemainingTime gives us the
	//total amount of time our state update function ran over or under on the previous
	//timeslice.
	//Calculate the next timing point. Note that while the GetNextEvent() function has
	//taken care of unused, complete mclk cycle counts which have been passed to the
	//AdvanceProcessorState() function, we still need to factor in unused time from the
	//previous timeslice which wasn't long enough to form a complete mclk cycle. That
	//additional time is time this device still has waiting to be consumed, so we need to
	//subtract this time from our calculation of when the next timing point will occur. We
	//also need to factor in the difference between system time and device time.
	//##TODO## Figure out which way is right, and clean this up.
//	nextEventToExecuteTime = (ConvertMclkCountToAccessTime(nextEventToExecute.mclkCycleCounter) - (currentTimesliceMclkCyclesRemainingTime + lastTimesliceRemainingTime));
	nextEventToExecuteTime = (ConvertMclkCountToAccessTime(nextEventToExecute.mclkCycleCounter) - lastTimesliceRemainingTime);

	//##FIX## If nextEventToExecute.mclkCycleCounter reads 5, with stateLastUpdateMclkUnused
	//reading 3, it means we need to execute 5 steps, not 2. Currently, we seem to be
	//executing 2 in NotifyAfterExecuteCalled.

	//##DEBUG##
//	std::wcout << "VDP - GetTimingPoint: " << hcounter.GetData() << '\t' << vcounter.GetData() << '\t' << stateLastUpdateMclkUnused << '\t' << nextEventToExecute.event << '\t' << nextEventToExecute.mclkCycleCounter << '\t' << std::setprecision(16) << nextEventToExecuteTime << '\n';

	return nextEventToExecuteTime;
}

//----------------------------------------------------------------------------------------
void S315_5313::ExecuteRollback()
{
	//Update state
	currentTimesliceMclkCyclesRemainingTime = bcurrentTimesliceMclkCyclesRemainingTime;
	lastTimesliceRemainingTime = blastTimesliceRemainingTime;
	stateLastUpdateMclkUnusedFromLastTimeslice = bstateLastUpdateMclkUnusedFromLastTimeslice;

	//Bus interface
	busGranted = bbusGranted;
	palModeLineState = bpalModeLineState;

	//Saved CE line state for Read-Modify-Write cycles
	lineLWRSavedStateRMW = blineLWRSavedStateRMW;
	lineUWRSavedStateRMW = blineUWRSavedStateRMW;
	lineCAS0SavedStateRMW = blineCAS0SavedStateRMW;
	lineRAS0SavedStateRMW = blineRAS0SavedStateRMW;
	lineOE0SavedStateRMW = blineOE0SavedStateRMW;

	//Physical registers and memory buffers
	reg.Rollback();
	vram->Rollback();
	cram->Rollback();
	vsram->Rollback();
	status = bstatus;
	hcounter = bhcounter;
	vcounter = bvcounter;
	hcounterLatchedData = bhcounterLatchedData;
	vcounterLatchedData = bvcounterLatchedData;
	hvCounterLatched = bhvCounterLatched;
	hintCounter = bhintCounter;
	vintPending = bvintPending;
	hintPending = bhintPending;
	exintPending = bexintPending;
	vsramCachedRead = bvsramCachedRead;
	readDataAvailable = breadDataAvailable;
	readDataHalfCached = breadDataHalfCached;
	dmaFillOperationRunning = bdmaFillOperationRunning;

	//Cached register settings
	hvCounterLatched = bhvCounterLatchEnabled;
	interlaceEnabled = binterlaceEnabled;
	interlaceDouble = binterlaceDouble;
	screenModeH40 = bscreenModeH40;
	screenModeV30 = bscreenModeV30;
	palMode = bpalMode;
	vintEnabled = bvintEnabled;
	hintEnabled = bhintEnabled;
	exintEnabled = bexintEnabled;
	hintCounterReloadValue = bhintCounterReloadValue;
	dmaEnabled = bdmaEnabled;
	dmd0 = bdmd0;
	dmd1 = bdmd1;
	dmaLengthCounter = bdmaLengthCounter;
	dmaSourceAddressByte1 = bdmaSourceAddressByte1;
	dmaSourceAddressByte2 = bdmaSourceAddressByte2;
	dmaSourceAddressByte3 = bdmaSourceAddressByte3;
	autoIncrementData = bautoIncrementData;
	interlaceEnabledCached = binterlaceEnabledCached;
	interlaceDoubleCached = binterlaceDoubleCached;
	screenModeH40Cached = bscreenModeH40Cached;
	screenModeV30Cached = bscreenModeV30Cached;

	//FIFO buffer registers
	for(unsigned int i = 0; i < fifoBufferSize; ++i)
	{
		fifoBuffer[i] = bfifoBuffer[i];
	}
	readBuffer = breadBuffer;
	fifoNextReadEntry = bfifoNextReadEntry;
	fifoNextWriteEntry = bfifoNextWriteEntry;

	//Control port registers
	commandWritePending = bcommandWritePending;

	//DMA worker thread properties
	workerThreadPaused = bworkerThreadPaused;
	busGrantTime = bbusGrantTime;

	//DMA transfer registers
	dmaTransferActive = bdmaTransferActive;
	dmaTransferReadDataCached = bdmaTransferReadDataCached;
	dmaTransferReadCache = bdmaTransferReadCache;
	dmaTransferNextReadMclk = bdmaTransferNextReadMclk;
	dmaTransferLastTimesliceUnusedMclkCycles = bdmaTransferLastTimesliceUnusedMclkCycles;

	//Event handling
	nextEventToExecute = bnextEventToExecute;
	nextEventToExecuteTime = bnextEventToExecuteTime;
	eventSnapshotSaved = beventSnapshotSaved;
	eventSnapshotScreenModeH40 = beventSnapshotScreenModeH40;
	eventSnapshotScreenModeV30 = beventSnapshotScreenModeV30;
	eventSnapshotPalMode = beventSnapshotPalMode;
	eventSnapshotInterlaceEnabled = beventSnapshotInterlaceEnabled;
}

//----------------------------------------------------------------------------------------
void S315_5313::ExecuteCommit()
{
	//Update state
	bcurrentTimesliceMclkCyclesRemainingTime = currentTimesliceMclkCyclesRemainingTime;
	blastTimesliceRemainingTime = lastTimesliceRemainingTime;
	bstateLastUpdateMclkUnusedFromLastTimeslice = stateLastUpdateMclkUnusedFromLastTimeslice;

	//Bus interface
	bbusGranted = busGranted;
	bpalModeLineState = palModeLineState;

	//Saved CE line state for Read-Modify-Write cycles
	blineLWRSavedStateRMW = lineLWRSavedStateRMW;
	blineUWRSavedStateRMW = lineUWRSavedStateRMW;
	blineCAS0SavedStateRMW = lineCAS0SavedStateRMW;
	blineRAS0SavedStateRMW = lineRAS0SavedStateRMW;
	blineOE0SavedStateRMW = lineOE0SavedStateRMW;

	//Physical registers and memory buffers
	reg.Commit();
	vram->Commit();
	cram->Commit();
	vsram->Commit();
	bstatus = status;
	bhcounter = hcounter;
	bvcounter = vcounter;
	bhcounterLatchedData = hcounterLatchedData;
	bvcounterLatchedData = vcounterLatchedData;
	bhvCounterLatched = hvCounterLatched;
	bhintCounter = hintCounter;
	bvintPending = vintPending;
	bhintPending = hintPending;
	bexintPending = exintPending;
	bvsramCachedRead = vsramCachedRead;
	breadDataAvailable = readDataAvailable;
	breadDataHalfCached = readDataHalfCached;
	bdmaFillOperationRunning = dmaFillOperationRunning;

	//Cached register settings
	bhvCounterLatched = hvCounterLatchEnabled;
	binterlaceEnabled = interlaceEnabled;
	binterlaceDouble = interlaceDouble;
	bscreenModeH40 = screenModeH40;
	bscreenModeV30 = screenModeV30;
	bpalMode = palMode;
	bvintEnabled = vintEnabled;
	bhintEnabled = hintEnabled;
	bexintEnabled = exintEnabled;
	bhintCounterReloadValue = hintCounterReloadValue;
	bdmaEnabled = dmaEnabled;
	bdmd0 = dmd0;
	bdmd1 = dmd1;
	bdmaLengthCounter = dmaLengthCounter;
	bdmaSourceAddressByte1 = dmaSourceAddressByte1;
	bdmaSourceAddressByte2 = dmaSourceAddressByte2;
	bdmaSourceAddressByte3 = dmaSourceAddressByte3;
	bautoIncrementData = autoIncrementData;
	binterlaceEnabledCached = interlaceEnabledCached;
	binterlaceDoubleCached = interlaceDoubleCached;
	bscreenModeH40Cached = screenModeH40Cached;
	bscreenModeV30Cached = screenModeV30Cached;

	//Propagate any changes to the cached DMA settings back into the reg buffer. This
	//makes changes to DMA settings visible to the debugger.
	//##FIX## If changes are made to the DMA settings through the debugger, the settings
	//cache won't be updated currently.
	if(cachedDMASettingsChanged)
	{
		AccessTarget accessTarget;
		accessTarget.AccessLatest();
		M5SetDMALengthCounter(accessTarget, dmaLengthCounter);
		M5SetDMASourceAddressByte1(accessTarget, dmaSourceAddressByte1);
		M5SetDMASourceAddressByte2(accessTarget, dmaSourceAddressByte2);
		cachedDMASettingsChanged = false;
	}

	//FIFO buffer registers
	for(unsigned int i = 0; i < fifoBufferSize; ++i)
	{
		bfifoBuffer[i] = fifoBuffer[i];
	}
	breadBuffer = readBuffer;
	bfifoNextReadEntry = fifoNextReadEntry;
	bfifoNextWriteEntry = fifoNextWriteEntry;

	//Control port registers
	bcommandWritePending = commandWritePending;

	//DMA worker thread properties
	bworkerThreadPaused = workerThreadPaused;
	bbusGrantTime = busGrantTime;

	//DMA transfer registers
	bdmaTransferActive = dmaTransferActive;
	bdmaTransferReadDataCached = dmaTransferReadDataCached;
	bdmaTransferReadCache = dmaTransferReadCache;
	bdmaTransferNextReadMclk = dmaTransferNextReadMclk;
	bdmaTransferLastTimesliceUnusedMclkCycles = dmaTransferLastTimesliceUnusedMclkCycles;

	//Event handling
	bnextEventToExecute = nextEventToExecute;
	bnextEventToExecuteTime = nextEventToExecuteTime;
	beventSnapshotSaved = eventSnapshotSaved;
	beventSnapshotScreenModeH40 = eventSnapshotScreenModeH40;
	beventSnapshotScreenModeV30 = eventSnapshotScreenModeV30;
	beventSnapshotPalMode = eventSnapshotPalMode;
	beventSnapshotInterlaceEnabled = eventSnapshotInterlaceEnabled;

	//If we're committing a timeslice which has triggered a render operation, pass the
	//render operation on to the render thread.
	if(pendingRenderOperation)
	{
		pendingRenderOperation = false;
		boost::mutex::scoped_lock lock(timesliceMutex);
		if(renderTimeslicePending)
		{
			vram->FreeTimesliceReference(vramTimeslice);
			cram->FreeTimesliceReference(cramTimeslice);
			vsram->FreeTimesliceReference(vsramTimeslice);
		}
		regTimeslice = reg.GetLatestTimeslice();
		vramTimeslice = vram->GetLatestTimesliceReference();
		cramTimeslice = cram->GetLatestTimesliceReference();
		vsramTimeslice = vsram->GetLatestTimesliceReference();
		renderTimeslicePending = true;
		renderThreadUpdate.notify_all();
	}
}

//----------------------------------------------------------------------------------------
void S315_5313::RenderThread()
{
	boost::mutex::scoped_lock lock(renderThreadMutex);

	//Start the render loop
	while(renderThreadActive)
	{
		//Obtain a copy of the latest completed timeslice period
		bool renderTimesliceObtained = false;
		{
			boost::mutex::scoped_lock lock(timesliceMutex);
			if(renderTimeslicePending)
			{
				regTimesliceCopy = regTimeslice;
				vramTimesliceCopy = vramTimeslice;
				cramTimesliceCopy = cramTimeslice;
				vsramTimesliceCopy = vsramTimeslice;
				renderTimeslicePending = false;

				//Flag that we managed to obtain a render timeslice
				renderTimesliceObtained = true;
			}
		}

		//If no render timeslice was available, suspend this thread until a timeslice
		//becomes available, or this thread is instructed to stop, then begin the loop
		//again.
		if(!renderTimesliceObtained)
		{
			renderThreadUpdate.wait(lock);
			continue;
		}

		//Render a complete frame to the framebuffer
		//##TODO## Rather than rendering frame by frame, render line by line. What I mean
		//by that is, our render thread should receive updates in 3420 MCLK cycle blocks,
		//and should track which line it is up to in active scan, so that it knows which
		//line to render next. If double buffering is not being used, partially rendered
		//frames will be displayed on the screen. If double buffering is being used, our
		//render thread will switch the buffers after rendering the last line before
		//vblank.
		RenderFrame();

		//##TODO## Re-evaluate this. We want to re-implement the frameskip behaviour we
		//had in the previous VDP core, where only the latest frame which is ready is
		//rendered. We need to consider how we'll handle interlacing with this system
		//though.
		//Advance to the latest target timeslice
		{
			boost::mutex::scoped_lock lock(timesliceMutex);
			reg.AdvancePastTimeslice(regTimesliceCopy);
			vram->AdvancePastTimeslice(vramTimesliceCopy);
			cram->AdvancePastTimeslice(cramTimesliceCopy);
			vsram->AdvancePastTimeslice(vsramTimesliceCopy);
			vram->FreeTimesliceReference(vramTimesliceCopy);
			cram->FreeTimesliceReference(cramTimesliceCopy);
			vsram->FreeTimesliceReference(vsramTimesliceCopy);
		}
	}
	renderThreadStopped.notify_all();
}

//----------------------------------------------------------------------------------------
void S315_5313::RenderFrame()
{
	//Create our reg buffer access target. Since we're working with the committed
	//state, access will be very fast, so there's no need to worry about caching
	//settings.
	AccessTarget accessTarget;
	accessTarget.AccessCommitted();

	//##TODO## We need to decide when we consider a "frame" to start, and when we
	//consider a "line" to start. The VDP doesn't have the concept of a start and end
	//for these things, but we need to make one in order to divide rendering into
	//logical segments.

	//##TODO##
	mclkCycleRenderProgress = 0;
	reg.BeginAdvanceSession(regSession, regTimesliceCopy, false);
	vram->BeginAdvanceSession(vramSession, vramTimesliceCopy, false);
	cram->BeginAdvanceSession(cramSession, cramTimesliceCopy, false);
	vsram->BeginAdvanceSession(vsramSession, vsramTimesliceCopy, false);

	//Obtain the VScan settings for the currently active display mode
	//##TODO## Implement palMode properly. We need to know what the palMode flag was
	//set to when the frame began, not what the current live state of the line is.
	//##FIX## We need to cache the state of the V30 cell mode at vsync
	bool v30CellModeEnabled = M5GetV30CellModeEnabled(accessTarget);
	const VScanSettings& vscanSettings = GetVScanSettings(v30CellModeEnabled, palMode, M5GetLSM0(accessTarget));

	//Calculate the number of lines to render for this frame in the various sections of
	//the display.
	//##TODO## Calculate these somewhere else, perhaps in GetVScanSettings().
	//##TODO## Accurately calculate the video regions of the display
	unsigned int activeScanLinesToRender = v30CellModeEnabled? 30*8: 28*8;
	unsigned int topBorderLinesToRender = (vscanSettings.linesPerFrame - activeScanLinesToRender) / 2;
	unsigned int bottomBorderLinesToRender = ((vscanSettings.linesPerFrame - activeScanLinesToRender) / 2) + ((vscanSettings.linesPerFrame - activeScanLinesToRender) % 2);

	//Perform the VDP render operations
	//##TODO## Render the top and bottom border regions correctly
	//Render the top border region
	for(unsigned int i = 0; i < topBorderLinesToRender; ++i)
	{
		RenderBorderLine(accessTarget, i, i);
	}
	//Render the active display lines
	for(unsigned int i = 0; i < activeScanLinesToRender; ++i)
	{
		RenderActiveLine(accessTarget, topBorderLinesToRender+i, i);
	}
	//Render the bottom border region
	for(unsigned int i = 0; i < bottomBorderLinesToRender; ++i)
	{
		RenderBorderLine(accessTarget, topBorderLinesToRender+activeScanLinesToRender+i, i);
	}

	//Here's what we DO know about the way data is dealt with internally:
	//-Colour values are only decoded when a final pixel is being displayed. We know
	//this because the VDP colour bus outputs the palette line and entry information.
	//-Highlight and shadow flags are output separately on the colour bus.
	//-A flag is output indicating whether the pixel is "transparent". Not clear from
	//documentation exactly what this means, but most likely a pixel is flagged as
	//transparent if it comes from the backdrop.
	//-A flag is output indicating whether the pixel originated from a sprite or a
	//layer.
	//-What all of this tells us is that the VDP almost certainly has each layer fill
	//its own 16-pixel buffer plane, and the layer selection from each plane only
	//occurs as that pixel is being displayed.

	//Pattern processing notes:
	//Patent 4243984, Column 9, row 25:
	//-A pattern name is latched into the "name latch" register, by combining the name
	//table base address, with the current vcounter and hcounter, and loading the
	//mapping entry it references.
	//-Using the current mapping stored in the "name latch" register, a pair of "video
	//colour codes" are latched from the "pattern colour table", and stored in a pair
	//of "pattern colour registers". These are a pair of 2-byte block mappings from
	//the layer mapping table.
	//-Using the video colour codes, the overlay control will load 2 sets of 4-byte
	//pattern data, corresponding with two visible lines of pixels, one from each
	//tile, into "pattern shift registers". These pattern shift registers then provide
	//the pixels for that plane to the output circuit.

	//##NOTE## The VISUAL operation of the VDP runs behind the DIGITAL operation of
	//the VDP. The digital VDP starts rendering as soon as the left border begins. The
	//first pixel is "drawn" when the first pixel of the left border is displayed. The
	//last pixel is drawn before the right border is displayed. The drawn pixels are
	//buffered before they are drawn to the screen, but as far as the digital
	//operation of the VDP is concerned, the status register and hv counter will
	//report that the VDP is ahead of what has actually been drawn to the screen. This
	//is important, since the CRAM flicker bug affects the LIVE video output, not the
	//buffered video output. In other words, if you perform a CRAM write when the
	//digital operation (with active scan being at the start of the line, and the left
	//border being at the end of the line) reports that the right border is starting,
	//the actual flicker on the screen will occur before the right border begins.
}

//----------------------------------------------------------------------------------------
//This handles the single line which runs immediately before the first "active" line which
//shows plane data. This line has the normal access restrictions as an active line, but is
//only used to capture sprite data for the following line.
//----------------------------------------------------------------------------------------
void S315_5313::RenderPrefetchLineForSprites(const AccessTarget& accessTarget)
{
	//##TODO##
}

//----------------------------------------------------------------------------------------
void S315_5313::RenderBorderLine(const AccessTarget& accessTarget, unsigned int renderBufferLineNo, unsigned int sectionLineNo)
{
	//##TODO## Implement this properly
	bool renderWithH40Mode = M5GetRS1(accessTarget);
	const HScanSettings& hscanSettings = GetHScanSettings(renderWithH40Mode);

	//Render the right border
	unsigned int totalLinePixelCount = hscanSettings.leftBorderPixelCount + hscanSettings.activeDisplayPixelCount + hscanSettings.rightBorderPixelCount;
	for(unsigned int i = 0; i < totalLinePixelCount; ++i)
	{
		unsigned int borderColorPaletteLine = M5GetBackgroundColorPalette(accessTarget);
		unsigned int borderColorPaletteEntry = M5GetBackgroundColorIndex(accessTarget);
		WriteColorValueToImageBuffer(borderColorPaletteLine, borderColorPaletteEntry, false, false, i, renderBufferLineNo);
	}

	mclkCycleRenderProgress += 3420;
	reg.AdvanceBySession(mclkCycleRenderProgress, regSession, regTimesliceCopy);
	vram->AdvanceBySession(mclkCycleRenderProgress, vramSession, vramTimesliceCopy);
	vsram->AdvanceBySession(mclkCycleRenderProgress, vsramSession, vsramTimesliceCopy);
	cram->AdvanceBySession(mclkCycleRenderProgress, cramSession, cramTimesliceCopy);
}

//----------------------------------------------------------------------------------------
//This function performs the actual task of rendering graphical data to the VDP
//framebuffer. We have performed precise hardware tests on the physical hardware to
//determine how and when various data is latched for the VDP rendering process. This
//render function maintains cycle accuracy for the rendering process, and reads data from
//the VRAM buffers and register settings at the same time as the real hardware.
//
//Testing notes:
//-We have confirmed that the various base address registers, IE, for layer A mapping
//data, are not cached. Changes to these registers take effect the next time mapping data
//is read.
//-We have confirmed that the field size registers are not cached, and changes take effect
//the next time mapping data is read.
//----------------------------------------------------------------------------------------
void S315_5313::RenderActiveLine(const AccessTarget& accessTarget, unsigned int renderBufferLineNo, unsigned int sectionLineNo)
{
	//##TODO## Implement mode 4 support
	//##FIX## The active interlace mode is latched during vblank. It needs to be cached,
	//and the cached value retrieved here.
	//##FIX## Although on the real hardware H40 mode can be enabled and disabled mid-line,
	//we do not support this for performance reasons, and it is instead latched at hblank.
	//We need to cache the H40 mode setting for each line, and use that cached value here.
	LineRenderSettings renderSettings;
	renderSettings.interlaceMode2Active = M5GetLSM0(accessTarget) && M5GetLSM1(accessTarget);
	renderSettings.renderWithH40Mode = M5GetRS1(accessTarget);
	const HScanSettings& hscanSettings = GetHScanSettings(renderSettings.renderWithH40Mode);

	//Calculate various screen mode settings for this line
	renderSettings.blockPatternSizeY = renderSettings.interlaceMode2Active? 16: 8;
	renderSettings.blockPatternByteSize = renderSettings.blockPatternRowByteSize * renderSettings.blockPatternSizeY;
	renderSettings.cellBlockSetsToRead = renderSettings.renderWithH40Mode? 5: 4;
	renderSettings.mclkCyclesPerSCCycle = renderSettings.renderWithH40Mode? 4: 5;
	renderSettings.mclkCyclesPerLogicStep = renderSettings.mclkCyclesPerSCCycle * renderSettings.scCyclesPerLogicStep;

	//Determine which line of active scan we're rendering
	renderSettings.activePixelY = sectionLineNo;
	renderSettings.activeBlockY = sectionLineNo / renderSettings.blockPixelSizeY;

	//##DEBUG##
	const unsigned int mclkCyclesPerLine = 3420;
	unsigned int initialMclkCycleRenderProgress = mclkCycleRenderProgress;

	//Calculate the address of the hscroll data to read for this line
	unsigned int hscrollDataBase = M5GetHScrollDataBase(accessTarget);
	bool hscrState = M5GetHSCR(accessTarget);
	bool lscrState = M5GetLSCR(accessTarget);
	unsigned int hscrollDataAddress = hscrollDataBase;
	//##TODO## Based on the EA logo for Populous, it appears that the state of LSCR is
	//ignored when HSCR is not set. We should confirm this on hardware.
	if(hscrState)
	{
		hscrollDataAddress += lscrState? (sectionLineNo * 4): ((renderSettings.activeBlockY * renderSettings.blockPixelSizeY) * 4);
	}

	//Read the hscroll data for this line
	//##TODO## Confirm the way scrolling data is interpreted through hardware tests. Eg,
	//does -1 actually scroll to the left by one place, or are 0 and -1 equivalent?
	//##TODO## According to the official documentation, the upper 6 bits of the hscroll
	//data are unused, and are allowed to be used by software to store whatever values
	//they want. Confirm this on hardware.
	unsigned int layerAHscrollOffset = ((unsigned int)vram->ReadCommitted(hscrollDataAddress+0) << 8) | (unsigned int)vram->ReadCommitted(hscrollDataAddress+1);
	unsigned int layerBHscrollOffset = ((unsigned int)vram->ReadCommitted(hscrollDataAddress+2) << 8) | (unsigned int)vram->ReadCommitted(hscrollDataAddress+3);
	mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;

	//##TODO## Read the sprite pattern data
	mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;
	mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;
	mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;
	mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;

	//Break the hscroll data into its two component parts. The lower 4 bits represent a
	//displacement into the 2-cell column, or in other words, the displacement of the
	//starting pixel within each column, while the upper 6 bits represent an offset for
	//the column mapping data itself.
	//-----------------------------------------
	//| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	//----------------------------------------|
	//|  Column Shift Value   | Displacement  |
	//-----------------------------------------
	unsigned int scrollValueDisplacementLayerA = layerAHscrollOffset & 0x00F;
	unsigned int columnShiftValueLayerA = (layerAHscrollOffset & 0x3F0) >> 4;
	unsigned int scrollValueDisplacementLayerB = layerBHscrollOffset & 0x00F;
	unsigned int columnShiftValueLayerB = (layerBHscrollOffset & 0x3F0) >> 4;

	//Create the buffers for our mapping and pattern data
	unsigned int currentRenderPixelLayerA = 0;
	unsigned int currentRenderPixelLayerB = 0;
	std::vector<Data> mappingDataCacheLayerA(2, Data(16));
	std::vector<Data> mappingDataCacheLayerB(2, Data(16));
	std::vector<Data> patternDataCacheLayerA(2, Data(32));
	std::vector<Data> patternDataCacheLayerB(2, Data(32));
	std::vector<unsigned int> paletteIndexSprite(hscanSettings.activeDisplayPixelCount+16);
	std::vector<unsigned int> paletteIndexLayerA(hscanSettings.activeDisplayPixelCount+16);
	std::vector<unsigned int> paletteIndexLayerB(hscanSettings.activeDisplayPixelCount+16);
	std::vector<unsigned int> paletteLineSprite(hscanSettings.activeDisplayPixelCount+16);
	std::vector<unsigned int> paletteLineLayerA(hscanSettings.activeDisplayPixelCount+16);
	std::vector<unsigned int> paletteLineLayerB(hscanSettings.activeDisplayPixelCount+16);
	std::vector<bool> prioritySprite(hscanSettings.activeDisplayPixelCount+16);
	std::vector<bool> priorityLayerA(hscanSettings.activeDisplayPixelCount+16);
	std::vector<bool> priorityLayerB(hscanSettings.activeDisplayPixelCount+16);

	//Determine the playfield dimensions
	//##TODO## Determine when playfield dimensions are actually calculated, either for the
	//entire line like this, or whether these registers are checked mid-line and can be
	//modified mid-line. Most likely, mid-line modifications are possible.
	unsigned int hszState = M5GetHSZ(accessTarget);
	unsigned int vszState = M5GetVSZ(accessTarget);

	//##TODO## Comment this
	//##TODO## If we're in interlace mode 2, and the odd flag is set, we need to add 1 to
	//the virtual vertical screen position when calculating the pattern and mapping row
	//numbers.
	unsigned int columnNumber = 0x3F;
	unsigned int displayRowNumber = (renderSettings.interlaceMode2Active? (sectionLineNo * 2): sectionLineNo);

	//Calculate the address of the vscroll data to read for this block
	//##TODO## Determine how hscroll interacts with vscroll when both are applied at the
	//same time. Read the test notes from Eke: Reportedly, if a 2-cell column is partially
	//visible on the left, that column has vscroll fixed to 0 for H32 mode, however for
	//H40 mode, it's some strange ANDed value of several VSRAM addresses. Check the
	//following thread from SpritesMind for more info:
	//http://gendev.spritesmind.net/forum/viewtopic.php?t=737&start=30
	//This only occurs when column-based vscroll is applied. Overall vscroll values work
	//correctly. This all needs major testing on hardware.
	//##TODO## Test how overall vertical scrolling interacts with line-based and overall
	//horizontal scrolling.
	bool vscrState = M5GetVSCR(accessTarget);
	unsigned int vscrollDataAddress = vscrState? (columnNumber * 4): 0;

	//Read the vscroll data for this line. Note only the lower 10 bits are
	//effective, or the lower 11 bits in the case of interlace mode 2, due to the
	//scrolled address being wrapped to lie within the total field boundaries,
	//which never exceed 128 blocks.
	unsigned int layerAVscrollOffset = ((unsigned int)vsram->ReadCommitted(vscrollDataAddress+0) << 8) | (unsigned int)vsram->ReadCommitted(vscrollDataAddress+1);
	unsigned int layerBVscrollOffset = ((unsigned int)vsram->ReadCommitted(vscrollDataAddress+2) << 8) | (unsigned int)vsram->ReadCommitted(vscrollDataAddress+3);

	//Calculate the target block mapping pair and selected row for layer A
	//##TODO## Confirm through VRAM snooping which actual address is read from for the
	//left scrolled cell block when the horizontal scroll value is aligned to a 2-cell
	//boundary. Even though the data is never displayed in this case, we can and should
	//determine where the VDP reads this data from.
	//##TODO## Confirm through VRAM snooping which actual address is read from when a
	//left-aligned window is present for layer A, both with and without horizontal
	//scrolling. We suspect that the normal block which is read for layer A, without a
	//window being active, may be read in this case.
	//##TODO## Replace the division and modulus operators here with bitshift and bitmask
	//operations.
	unsigned int mappingColumnNumberLayerA = columnNumber - columnShiftValueLayerA;
	unsigned int mappingRowNumberLayerA = (displayRowNumber + layerAVscrollOffset) / renderSettings.blockPatternSizeY;
	unsigned int patternRowNumberLayerA = (displayRowNumber + layerAVscrollOffset) % renderSettings.blockPatternSizeY;

	//##TODO## Implement support for the window layer
//	unsigned int nameTableBaseA = M5GetNameTableBaseWindow(accessTarget);
//	ReadMappingDataPair(mappingDataCacheLayerA[0], mappingDataCacheLayerA[1], nameTableBaseA, displayRowNumber / renderSettings.blockPatternSizeY, columnNumber, 0x0, 0x0);

	//Read layer A left scrolled cell block mapping data
	unsigned int nameTableBaseA = M5GetNameTableBaseScrollA(accessTarget);
	ReadMappingDataPair(mappingDataCacheLayerA[0], mappingDataCacheLayerA[1], nameTableBaseA, mappingRowNumberLayerA, mappingColumnNumberLayerA, hszState, vszState);
	mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;

	//##TODO## Read the sprite pattern data
	mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;

	//Read layer A left scrolled cell block pattern row 1
	ReadPatternDataRow(renderSettings, patternDataCacheLayerA[0], mappingDataCacheLayerA[0], patternRowNumberLayerA);
	mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;

	//Read layer A left scrolled cell block pattern row 2
	ReadPatternDataRow(renderSettings, patternDataCacheLayerA[1], mappingDataCacheLayerA[1], patternRowNumberLayerA);
	mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;

	//Render the layer A column
	RenderColumnBlockPair(columnNumber, scrollValueDisplacementLayerA, mappingDataCacheLayerA[0], mappingDataCacheLayerA[1], patternDataCacheLayerA[0], patternDataCacheLayerA[1], paletteLineLayerA, paletteIndexLayerA, currentRenderPixelLayerA);

	//Calculate the target block mapping pair and selected row for layer B
	unsigned int mappingColumnNumberLayerB = columnNumber - columnShiftValueLayerB;
	unsigned int mappingRowNumberLayerB = (displayRowNumber + layerBVscrollOffset) / renderSettings.blockPatternSizeY;
	unsigned int patternRowNumberLayerB = (displayRowNumber + layerBVscrollOffset) % renderSettings.blockPatternSizeY;

	//Read layer B left scrolled cell block mapping data
	unsigned int nameTableBaseB = M5GetNameTableBaseScrollB(accessTarget);
	ReadMappingDataPair(mappingDataCacheLayerB[0], mappingDataCacheLayerB[1], nameTableBaseB, mappingRowNumberLayerB, mappingColumnNumberLayerB, hszState, vszState);
	mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;

	//##TODO## Read sprite mapping data
	mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;

	//Read layer B left scrolled cell block pattern row 1
	ReadPatternDataRow(renderSettings, patternDataCacheLayerB[0], mappingDataCacheLayerB[0], patternRowNumberLayerB);
	mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;

	//Read layer B left scrolled cell block pattern row 2
	ReadPatternDataRow(renderSettings, patternDataCacheLayerB[1], mappingDataCacheLayerB[1], patternRowNumberLayerB);
	mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;

	//Render the layer B column
	RenderColumnBlockPair(columnNumber, scrollValueDisplacementLayerB, mappingDataCacheLayerB[0], mappingDataCacheLayerB[1], patternDataCacheLayerB[0], patternDataCacheLayerB[1], paletteLineLayerB, paletteIndexLayerB, currentRenderPixelLayerB);

	//Read the mapping and pattern data for all layers
	for(unsigned int cellBlockSet = 0; cellBlockSet < renderSettings.cellBlockSetsToRead; ++cellBlockSet)
	{
		for(unsigned int i = 0; i < 4; ++i)
		{
			columnNumber = (cellBlockSet * 4) + i;

			//Calculate the address of the vscroll data to read for this block
			//##TODO## Determine if the vscroll data is read at the start of each 2-cell
			//block, even for overall scrolling. We can determine this by changing the
			//vscroll data mid-line.
			//##TODO## Determine if mid-line changes to VSCR take effect immediately
			bool vscrState = M5GetVSCR(accessTarget);
			unsigned int vscrollDataAddress = vscrState? (columnNumber * 4): 0;

			//Read the vscroll data for this line. Note only the lower 10 bits are
			//effective, or the lower 11 bits in the case of interlace mode 2, due to the
			//scrolled address being wrapped to lie within the total field boundaries,
			//which never exceed 128 blocks.
			unsigned int layerAVscrollOffset = ((unsigned int)vsram->ReadCommitted(vscrollDataAddress+0) << 8) | (unsigned int)vsram->ReadCommitted(vscrollDataAddress+1);
			unsigned int layerBVscrollOffset = ((unsigned int)vsram->ReadCommitted(vscrollDataAddress+2) << 8) | (unsigned int)vsram->ReadCommitted(vscrollDataAddress+3);

			//Calculate the target block mapping pair and selected row for layer A
			unsigned int mappingColumnNumberLayerA = columnNumber - columnShiftValueLayerA;
			unsigned int mappingRowNumberLayerA = (displayRowNumber + layerAVscrollOffset) / renderSettings.blockPatternSizeY;
			unsigned int patternRowNumberLayerA = (displayRowNumber + layerAVscrollOffset) % renderSettings.blockPatternSizeY;

			//Read layer A mapping data
			unsigned int nameTableBaseA = M5GetNameTableBaseScrollA(accessTarget);
			ReadMappingDataPair(mappingDataCacheLayerA[0], mappingDataCacheLayerA[1], nameTableBaseA, mappingRowNumberLayerA, mappingColumnNumberLayerA, hszState, vszState);
			mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;

			//External access slot, or refresh cycle on 4th block in set.
			if(i < 3)
			{
				reg.AdvanceBySession(mclkCycleRenderProgress, regSession, regTimesliceCopy);
				vram->AdvanceBySession(mclkCycleRenderProgress, vramSession, vramTimesliceCopy);
				vsram->AdvanceBySession(mclkCycleRenderProgress, vsramSession, vsramTimesliceCopy);
			}
			mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;

			//Read layer A pattern row 1
			ReadPatternDataRow(renderSettings, patternDataCacheLayerA[0], mappingDataCacheLayerA[0], patternRowNumberLayerA);
			mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;

			//Read layer A pattern row 2
			ReadPatternDataRow(renderSettings, patternDataCacheLayerA[1], mappingDataCacheLayerA[1], patternRowNumberLayerA);
			mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;

			//Render the layer A column
			RenderColumnBlockPair(columnNumber, scrollValueDisplacementLayerA, mappingDataCacheLayerA[0], mappingDataCacheLayerA[1], patternDataCacheLayerA[0], patternDataCacheLayerA[1], paletteLineLayerA, paletteIndexLayerA, currentRenderPixelLayerA);

			//Calculate the target block mapping pair and selected row for layer B
			unsigned int mappingColumnNumberLayerB = columnNumber - columnShiftValueLayerB;
			unsigned int mappingRowNumberLayerB = (displayRowNumber + layerBVscrollOffset) / renderSettings.blockPatternSizeY;
			unsigned int patternRowNumberLayerB = (displayRowNumber + layerBVscrollOffset) % renderSettings.blockPatternSizeY;

			//Read layer B mapping data
			unsigned int nameTableBaseB = M5GetNameTableBaseScrollB(accessTarget);
			ReadMappingDataPair(mappingDataCacheLayerB[0], mappingDataCacheLayerB[1], nameTableBaseB, mappingRowNumberLayerB, mappingColumnNumberLayerB, hszState, vszState);
			mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;

			//##TODO## Read sprite mapping data
			mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;

			//Read layer B pattern row 1
			ReadPatternDataRow(renderSettings, patternDataCacheLayerB[0], mappingDataCacheLayerB[0], patternRowNumberLayerB);
			mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;

			//Read layer B pattern row 2
			ReadPatternDataRow(renderSettings, patternDataCacheLayerB[1], mappingDataCacheLayerB[1], patternRowNumberLayerB);
			mclkCycleRenderProgress += renderSettings.mclkCyclesPerLogicStep;

			//Render the layer A column
			RenderColumnBlockPair(columnNumber, scrollValueDisplacementLayerB, mappingDataCacheLayerB[0], mappingDataCacheLayerB[1], patternDataCacheLayerB[0], patternDataCacheLayerB[1], paletteLineLayerB, paletteIndexLayerB, currentRenderPixelLayerB);
		}
	}

	//##TODO## Note that we only ever need to read or advance the CRAM data when we're
	//actually rendering, after the line has been built. This means we should be advancing
	//the CRAM data state separately from the rest of the buffers, and it is when we
	//perform this advance that we take CRAM write flicker into account.
	//##TODO## Clean up this old code.
	unsigned int cramWriteFlickerBufferSize = hscanSettings.leftBorderPixelCount + hscanSettings.activeDisplayPixelCount + hscanSettings.rightBorderPixelCount;
	std::vector<unsigned int> cramWriteFlickerPaletteIndex(cramWriteFlickerBufferSize);
	std::vector<unsigned int> cramWriteFlickerPaletteLine(cramWriteFlickerBufferSize);
	std::vector<bool> cramWriteFlickerOccurred(cramWriteFlickerBufferSize, false);

	//##TODO## Don't do this at all. Store CRAM writes into a buffer, like we will for
	//rendered sprite pixels.
	//##TODO## Consider enhancing the session stepping for timed buffers, to allow the
	//next write time to be extracted from the session info. This will avoid the need to
	//call BufferGetWriteInfo here unnecessarily, as we can use the saved information in
	//the session to determine if there is a write in range.
	//unsigned int cramWriteIndex = 0;
	//IManagedBufferInt::WriteInfo cramWriteInfo = cram->BufferGetWriteInfo(cramWriteIndex++, cramTimesliceCopy);
	//while(cramWriteInfo.exists && (cramWriteInfo.writeTime < mclkCyclesPerLine))
	//{
	//	unsigned int pixelIndex = (cramWriteInfo.writeTime / renderSettings.mclkCyclesPerSCCycle) / 2;
	//	unsigned int paletteLine = cramWriteInfo.writeAddress / 0x20;
	//	unsigned int paletteIndex = (cramWriteInfo.writeAddress % 0x20) / 2;
	//	cramWriteFlickerOccurred[pixelIndex] = true;
	//	cramWriteFlickerPaletteLine[pixelIndex] = paletteLine;
	//	cramWriteFlickerPaletteIndex[pixelIndex] = paletteIndex;
	//	cramWriteInfo = cram->BufferGetWriteInfo(cramWriteIndex++, cramTimesliceCopy);
	//}

	//##FIX## The background colour palette and index settings have been advanced to the
	//end of the line by now. We need the live register values. In fact, this issue of
	//live register values affects a lot more than just background changes. It affects the
	//disable video flag for example.
	//Render the left border
	for(unsigned int i = 0; i < hscanSettings.leftBorderPixelCount; ++i)
	{
		unsigned int borderColorPaletteLine = M5GetBackgroundColorPalette(accessTarget);
		unsigned int borderColorPaletteEntry = M5GetBackgroundColorIndex(accessTarget);
		if(cramWriteFlickerOccurred[i])
		{
			borderColorPaletteLine = cramWriteFlickerPaletteLine[i];
			borderColorPaletteEntry = cramWriteFlickerPaletteIndex[i];
		}
		WriteColorValueToImageBuffer(borderColorPaletteLine, borderColorPaletteEntry, false, false, i, renderBufferLineNo);
	}

	//Render the active display
	for(unsigned int i = 0; i < hscanSettings.activeDisplayPixelCount; ++i)
	{
		//Read the background palette settings
		unsigned int paletteLineBackground = M5GetBackgroundColorPalette(accessTarget);
		unsigned int paletteIndexBackground = M5GetBackgroundColorIndex(accessTarget);

		//Load the palette line and index values for each layer into our data table
		unsigned int paletteLineData[4];
		unsigned int paletteIndexData[4];
		paletteLineData[LAYER_SPRITE] = paletteLineSprite[i];
		paletteLineData[LAYER_LAYERA] = paletteLineLayerA[i];
		paletteLineData[LAYER_LAYERB] = paletteLineLayerB[i];
		paletteLineData[LAYER_BACKGROUND] = paletteLineBackground;
		paletteIndexData[LAYER_SPRITE] = paletteIndexSprite[i];
		paletteIndexData[LAYER_LAYERA] = paletteIndexLayerA[i];
		paletteIndexData[LAYER_LAYERB] = paletteIndexLayerB[i];
		paletteIndexData[LAYER_BACKGROUND] = paletteIndexBackground;

		//Determine if any of the palette index values for any of the layers indicate a
		//transparent pixel.
		//##TODO## Consider renaming and reversing the logic of these flags to match the
		//comment above. The name of "found pixel" isn't very descriptive, and in the case
		//of the sprite layer "isPixelOpaque" could be misleading when the sprite pixel is
		//being used as an operator in shadow/highlight mode. A flag with a name like
		//isPixelTransparent would be much more descriptive.
		bool foundSpritePixel = (paletteIndexData[LAYER_SPRITE] != 0);
		bool foundLayerAPixel = (paletteIndexData[LAYER_LAYERA] != 0);
		bool foundLayerBPixel = (paletteIndexData[LAYER_LAYERB] != 0);

		//Read the shadow/highlight mode settings
		//##TODO## Check when changes to the shadow/highlight register take effect. Can
		//this register be toggled mid-line?
		bool shadowHighlightEnabled = M5GetShadowHighlightEnabled(accessTarget);
		bool spriteIsShadowOperator = (paletteLineData[LAYER_SPRITE] == 3) && (paletteIndexData[LAYER_SPRITE] == 15);
		bool spriteIsHighlightOperator = (paletteLineData[LAYER_SPRITE] == 3) && (paletteIndexData[LAYER_SPRITE] == 14);

		//Perform layer priority calculations, and determine the layer to use, as well as
		//the resulting state of the shadow and highlight bits.
		//unsigned int layerIndex;
		//bool shadow;
		//bool highlight;
		//CalculateLayerPriorityIndex(layerIndex, shadow, highlight, shadowHighlightEnabled, spriteIsShadowOperator, spriteIsHighlightOperator, foundSpritePixel, foundLayerAPixel, foundLayerBPixel, prioritySprite, priorityLayerA, priorityLayerB);

		//Encode the parameters for the layer priority calculation into an index value for
		//the priority lookup table.
		unsigned int priorityIndex = 0;
		priorityIndex |= (unsigned int)shadowHighlightEnabled << 8;
		priorityIndex |= (unsigned int)spriteIsShadowOperator << 7;
		priorityIndex |= (unsigned int)spriteIsHighlightOperator << 6;
		priorityIndex |= (unsigned int)foundSpritePixel << 5;
		priorityIndex |= (unsigned int)foundLayerAPixel << 4;
		priorityIndex |= (unsigned int)foundLayerBPixel << 3;
		priorityIndex |= (unsigned int)prioritySprite[i] << 2;
		priorityIndex |= (unsigned int)priorityLayerA[i] << 1;
		priorityIndex |= (unsigned int)priorityLayerB[i];

		//Lookup the pre-calculated layer priority from the lookup table. We use a lookup
		//table to eliminate branching, which should yield a significant performance
		//boost.
		unsigned int layerSelectionResult = layerPriorityLookupTable[priorityIndex];

		//Extract the layer index, shadow, and highlight data from the combined result
		//returned from the layer priority lookup table.
		unsigned int layerIndex = layerSelectionResult & 0x03;
		bool shadow = (layerSelectionResult & 0x08) != 0;
		bool highlight = (layerSelectionResult & 0x04) != 0;

		//Read the palette line and index to use for the selected layer
		unsigned int paletteLine = paletteLineData[layerIndex];
		unsigned int paletteIndex = paletteIndexData[layerIndex];

		//Write the final colour values to the image buffer
		if(cramWriteFlickerOccurred[hscanSettings.leftBorderPixelCount+i])
		{
			paletteLine = cramWriteFlickerPaletteLine[hscanSettings.leftBorderPixelCount+i];
			paletteIndex = cramWriteFlickerPaletteIndex[hscanSettings.leftBorderPixelCount+i];
		}
		WriteColorValueToImageBuffer(paletteLine, paletteIndex, shadow, highlight, hscanSettings.leftBorderPixelCount + i, renderBufferLineNo);
	}

	//Render the right border
	for(unsigned int i = 0; i < hscanSettings.rightBorderPixelCount; ++i)
	{
		unsigned int borderColorPaletteLine = M5GetBackgroundColorPalette(accessTarget);
		unsigned int borderColorPaletteEntry = M5GetBackgroundColorIndex(accessTarget);
		if(cramWriteFlickerOccurred[hscanSettings.leftBorderPixelCount+hscanSettings.activeDisplayPixelCount+i])
		{
			borderColorPaletteLine = cramWriteFlickerPaletteLine[hscanSettings.leftBorderPixelCount+hscanSettings.activeDisplayPixelCount+i];
			borderColorPaletteEntry = cramWriteFlickerPaletteIndex[hscanSettings.leftBorderPixelCount+hscanSettings.activeDisplayPixelCount+i];
		}
		WriteColorValueToImageBuffer(borderColorPaletteLine, borderColorPaletteEntry, false, false, hscanSettings.leftBorderPixelCount+hscanSettings.activeDisplayPixelCount+i, renderBufferLineNo);
	}

	//##DEBUG##
	unsigned int remainingMclkCyclesInLine = mclkCyclesPerLine - (mclkCycleRenderProgress - initialMclkCycleRenderProgress);
	mclkCycleRenderProgress += remainingMclkCyclesInLine;
	reg.AdvanceBySession(mclkCycleRenderProgress, regSession, regTimesliceCopy);
	vram->AdvanceBySession(mclkCycleRenderProgress, vramSession, vramTimesliceCopy);
	cram->AdvanceBySession(mclkCycleRenderProgress, cramSession, cramTimesliceCopy);
	vsram->AdvanceBySession(mclkCycleRenderProgress, vsramSession, vsramTimesliceCopy);
}

//----------------------------------------------------------------------------------------
void S315_5313::CalculateLayerPriorityIndex(unsigned int& layerIndex, bool& shadow, bool& highlight, bool shadowHighlightEnabled, bool spriteIsShadowOperator, bool spriteIsHighlightOperator, bool foundSpritePixel, bool foundLayerAPixel, bool foundLayerBPixel, bool prioritySprite, bool priorityLayerA, bool priorityLayerB) const
{
	//Initialize the shadow/highlight flags
	shadow = false;
	highlight = false;

	//Perform layer priority calculations
	if(!shadowHighlightEnabled)
	{
		//Perform standard layer priority calculations
		if(foundSpritePixel && prioritySprite)
		{
			layerIndex = LAYER_SPRITE;
		}
		else if(foundLayerAPixel && priorityLayerA)
		{
			layerIndex = LAYER_LAYERA;
		}
		else if(foundLayerBPixel && priorityLayerB)
		{
			layerIndex = LAYER_LAYERB;
		}
		else if(foundSpritePixel)
		{
			layerIndex = LAYER_SPRITE;
		}
		else if(foundLayerAPixel)
		{
			layerIndex = LAYER_LAYERA;
		}
		else if(foundLayerBPixel)
		{
			layerIndex = LAYER_LAYERB;
		}
		else
		{
			layerIndex = LAYER_BACKGROUND;
		}
	}
	else
	{
		//Perform shadow/highlight mode layer priority calculations. Note that some
		//illustrations in the official documentation from Sega demonstrating the
		//behaviour of shadow/highlight mode are incorrect. In particular, the third and
		//fifth illustrations on page 64 of the "Genesis Software Manual", showing layers
		//B and A being shadowed when a shadow sprite operator is at a lower priority, are
		//incorrect. If any layer is above an operator sprite pixel, the sprite operator
		//is ignored, and the higher priority pixel is output without the sprite operator
		//being applied. This has been confirmed through hardware tests. All other
		//illustrations describing the operation of shadow/highlight mode in relation to
		//layer priority settings appear to be correct.
		if(foundSpritePixel && prioritySprite && !spriteIsShadowOperator && !spriteIsHighlightOperator)
		{
			layerIndex = LAYER_SPRITE;
		}
		else if(foundLayerAPixel && priorityLayerA)
		{
			layerIndex = LAYER_LAYERA;
			if(prioritySprite && spriteIsShadowOperator)
			{
				shadow = true;
			}
			else if(prioritySprite && spriteIsHighlightOperator)
			{
				highlight = true;
			}
		}
		else if(foundLayerBPixel && priorityLayerB)
		{
			layerIndex = LAYER_LAYERB;
			if(prioritySprite && spriteIsShadowOperator)
			{
				shadow = true;
			}
			else if(prioritySprite && spriteIsHighlightOperator)
			{
				highlight = true;
			}
		}
		else if(foundSpritePixel && !spriteIsShadowOperator && !spriteIsHighlightOperator)
		{
			layerIndex = LAYER_SPRITE;
			if(!priorityLayerA && !priorityLayerB)
			{
				shadow = true;
			}
		}
		else if(foundLayerAPixel)
		{
			layerIndex = LAYER_LAYERA;
			if(!priorityLayerA && !priorityLayerB)
			{
				shadow = true;
			}
			if(spriteIsShadowOperator)
			{
				shadow = true;
			}
			else if(spriteIsHighlightOperator)
			{
				highlight = true;
			}
		}
		else if(foundLayerBPixel)
		{
			layerIndex = LAYER_LAYERB;
			if(!priorityLayerA && !priorityLayerB)
			{
				shadow = true;
			}
			if(spriteIsShadowOperator)
			{
				shadow = true;
			}
			else if(spriteIsHighlightOperator)
			{
				highlight = true;
			}
		}
		else
		{
			layerIndex = LAYER_BACKGROUND;
			if(!priorityLayerA && !priorityLayerB)
			{
				shadow = true;
			}
			if(spriteIsShadowOperator)
			{
				shadow = true;
			}
			else if(spriteIsHighlightOperator)
			{
				highlight = true;
			}
		}

		//If shadow and highlight are both set, they cancel each other out. This is why a
		//sprite acting as a highlight operator is unable to highlight layer A, B, or the
		//background, if layers A and B both have their priority bits unset. This has been
		//confirmed on the hardware.
		if(shadow && highlight)
		{
			shadow = false;
			highlight = false;
		}
	}
}

//----------------------------------------------------------------------------------------
void S315_5313::WriteColorValueToImageBuffer(unsigned int paletteLine, unsigned int paletteEntry, bool shadow, bool highlight, unsigned int xpos, unsigned int ypos)
{
	//##TODO## Emulate CRAM write flicker
	const unsigned int paletteEntriesPerLine = 16;
	const unsigned int paletteEntrySize = 2;
	//##TODO## Our new colour values are basically correct, assuming what is suspected
	//after analysis posted on SpritesMind, that the Mega Drive VDP never actually outputs
	//at full intensity. We haven't taken the apparent "ladder effect" into account
	//however. It is highly recommended that we perform our own tests on the hardware, and
	//make some comparisons between captured video output from the real system, and the
	//output from our emulator, when playing back on the same physical screen. If the
	//ladder effect is real and does have an effect on the way the intensity is perceived
	//on the screen, we should emulate it. We also need to confirm the maximum intensity
	//output by the VDP. A step size of 18 for example would get a max value of 252, which
	//would be more logical.
	//const unsigned char paletteEntryTo8Bit[8] = {0, 36, 73, 109, 146, 182, 219, 255};
	//const unsigned char paletteEntryTo8BitShadow[8] = {0, 18, 37, 55, 73, 91, 110, 128};
	//const unsigned char paletteEntryTo8BitHighlight[8] = {128, 146, 165, 183, 201, 219, 238, 255};
	const unsigned char paletteEntryTo8Bit[8] = {0, 34, 68, 102, 136, 170, 204, 238};
	const unsigned char paletteEntryTo8BitShadow[8] = {0, 17, 34, 51, 68, 85, 102, 119};
	const unsigned char paletteEntryTo8BitHighlight[8] = {119, 136, 153, 170, 187, 204, 221, 238};

	//Calculate the address of the colour value to read from the palette
	unsigned int paletteEntryAddress = (paletteEntry + (paletteLine * paletteEntriesPerLine)) * paletteEntrySize;

	//Read the target palette entry
	Data paletteData(16);
	paletteData = (unsigned int)(cram->ReadCommitted(paletteEntryAddress+0) << 8) | (unsigned int)cram->ReadCommitted(paletteEntryAddress+1);

	//Decode palette color
	unsigned char r;
	unsigned char g;
	unsigned char b;
	if(shadow && !highlight)
	{
		r = paletteEntryTo8BitShadow[(paletteData.GetData() & 0x000E) >> 1];
		g = paletteEntryTo8BitShadow[(paletteData.GetData() & 0x00E0) >> 5];
		b = paletteEntryTo8BitShadow[(paletteData.GetData() & 0x0E00) >> 9];
	}
	else if(highlight && !shadow)
	{
		r = paletteEntryTo8BitHighlight[(paletteData.GetData() & 0x000E) >> 1];
		g = paletteEntryTo8BitHighlight[(paletteData.GetData() & 0x00E0) >> 5];
		b = paletteEntryTo8BitHighlight[(paletteData.GetData() & 0x0E00) >> 9];
	}
	else
	{
		r = paletteEntryTo8Bit[(paletteData.GetData() & 0x000E) >> 1];
		g = paletteEntryTo8Bit[(paletteData.GetData() & 0x00E0) >> 5];
		b = paletteEntryTo8Bit[(paletteData.GetData() & 0x0E00) >> 9];
	}

	//Write the final colour value to the image buffer
	const unsigned int imageBufferEntryByteCount = 4;
	//##TODO## Clean up this entry
	const unsigned int height = 313;
	unsigned int bufferPos = ((xpos + (((height - 1) * imageWidth) - (ypos * imageWidth))) * imageBufferEntryByteCount);
//	unsigned int bufferPos = (xpos + (((imageHeight - 1) - ypos) * imageWidth)) * imageBufferEntryByteCount;
	if(bufferPos >= (imageWidth * imageHeight * imageBufferEntryByteCount))
	{
		//##DEBUG##
		std::wcout << "ERROR! Buffer overflow on image buffer\n";
		std::wcout << ypos << ' ' << xpos << '\n';
	}
	image[drawingImageBufferPlane][bufferPos + 0] = r;
	image[drawingImageBufferPlane][bufferPos + 1] = g;
	image[drawingImageBufferPlane][bufferPos + 2] = b;
	image[drawingImageBufferPlane][bufferPos + 3] = 0xFF;
}

//----------------------------------------------------------------------------------------
//This function performs all the necessary calculations to determine which mapping data to
//read for a given playfield position, and reads the corresponding mapping data pair from
//VRAM. The calculations performed appear to produce the same result as the real VDP
//hardware under all modes and settings, including when invalid scroll size modes are
//used.
//
//The following comments are provided as a supplement to the comments within this
//function, and show how the internally calculated row and column numbers are combined
//with the mapping base address data to produce a final VRAM address for the mapping
//block. All possible combinations of screen mode settings are shown, including invalid
//modes (VSZ="10" or HSZ="10"). Note that invalid combinations of screen mode settings are
//not shown, since invalid combinations never actually occur, due to the vertical screen
//mode being adjusted based on the horizontal screen mode, as outlined in the function
//comments below.
//
//Officially supported screen mode settings. Note that the lower two bits of the resulting
//address are masked before the address is used.
//Mapping data VRAM address (HSZ=00 VSZ=00):
//-----------------------------------------------------------------
//| 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//|---------------------------------------------------------------|
//| Base Addr | 0 | 0 |        Row        |       Column      | 0 | (Row Shift Count = 6)
//-----------------------------------------------------------------
//Mapping data VRAM address (HSZ=01 VSZ=00):
//-----------------------------------------------------------------
//| 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//|---------------------------------------------------------------|
//| Base Addr | 0 |        Row        |         Column        | 0 | (Row Shift Count = 7)
//-----------------------------------------------------------------
//Mapping data VRAM address (HSZ=11 VSZ=00):
//-----------------------------------------------------------------
//| 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//|---------------------------------------------------------------|
//| Base Addr |        Row        |           Column          | 0 | (Row Shift Count = 8)
//-----------------------------------------------------------------
//Mapping data VRAM address (HSZ=00 VSZ=01):
//-----------------------------------------------------------------
//| 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//|---------------------------------------------------------------|
//| Base Addr | 0 |          Row          |       Column      | 0 | (Row Shift Count = 6)
//-----------------------------------------------------------------
//Mapping data VRAM address (HSZ=01 VSZ=01):
//-----------------------------------------------------------------
//| 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//|---------------------------------------------------------------|
//| Base Addr |          Row          |         Column        | 0 | (Row Shift Count = 7)
//-----------------------------------------------------------------
//Mapping data VRAM address (HSZ=00 VSZ=11):
//-----------------------------------------------------------------
//| 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//|---------------------------------------------------------------|
//| Base Addr |            Row            |       Column      | 0 | (Row Shift Count = 6)
//-----------------------------------------------------------------
//
//Officially unsupported screen modes. In this case, the row and column data may be
//interleaved, and the row shift count may be 0, as shown below:
//Mapping data VRAM address (HSZ=00 VSZ=10):
//-----------------------------------------------------------------
//| 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//|---------------------------------------------------------------|
//|           |Row| 0 |        Row        |                       | (Row Shift Count = 6)
//|           -------------------------------------------------   |
//|                                       |       Column      |   |
//|---------------------------------------------------------------|
//| Base Addr |Row| 0 |        Row        |       Column      | 0 |
//-----------------------------------------------------------------
//##FIX## Hardware tests have shown the two cases below to be incorrect. It appears the
//upper bit of the column data is never applied, and the row is never incremented when the
//invalid horizontal mode is active.
//Mapping data VRAM address (HSZ=10 VSZ=00):
//-----------------------------------------------------------------
//| 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//|---------------------------------------------------------------|
//|                                           |        Row        | (Row Shift Count = 0)
//|                                       ------------------------|
//|                                       |       Column      |   |
//|---------------------------------------------------------------|
//| Base Addr | 0 | 0 | 0 | 0 | 0 |Col| 0 |       Column      |Row|
//-----------------------------------------------------------------
//Mapping data VRAM address (HSZ=10 VSZ=10):
//-----------------------------------------------------------------
//| 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//|---------------------------------------------------------------|
//|                                   |Row| 0 |        Row        | (Row Shift Count = 0)
//|                               --------------------------------|
//|                               |Col| 0 |       Column      |   |
//|---------------------------------------------------------------|
//| Base Addr | 0 | 0 | 0 | 0 | 0 |Col|Row|       Column      |Row|
//-----------------------------------------------------------------
//##TODO## Implement the correct mappings, which are as follows:
//Mapping data VRAM address (HSZ=10 VSZ=00):
//-----------------------------------------------------------------
//| 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//|---------------------------------------------------------------|
//| Base Addr | 0 | 0 | 0 | 0 | 0 | 0 | 0 |       Column      | 0 |
//-----------------------------------------------------------------
//Mapping data VRAM address (HSZ=10 VSZ=10):
//-----------------------------------------------------------------
//| 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//|---------------------------------------------------------------|
//| Base Addr | 0 | 0 | 0 | 0 | 0 | 0 | 0 |       Column      | 0 |
//-----------------------------------------------------------------
//----------------------------------------------------------------------------------------
void S315_5313::ReadMappingDataPair(Data& mappingDataEntry1, Data& mappingDataEntry2, unsigned int nameTableBaseAddress, unsigned int mappingDataRowNumber, unsigned int mappingDataColumnNumber, unsigned int screenSizeModeH, unsigned int screenSizeModeV) const
{
	//The existence and contents of this table have been determined through analysis of
	//the behaviour of the VDP when invalid field size settings are selected. In
	//particular, the third table entry of 0 is used when the invalid HSZ mode of "10" is
	//selected. This causes the row and column to overlap when building the final address
	//value. The way the address is built in these circumstances can only be logically
	//explained by a lookup table being used for the row shift count, with the third entry
	//being set to 0, as we have implemented here.
	//##FIX## Hardware tests have shown this is not actually the case
	static const unsigned int rowShiftCountTable[4] = {6, 7, 0, 8};

	//The following calculation limits the vertical playfield size, based on the
	//horizontal playfield size. This calculation is quite simple in hardware, but looks
	//more complicated in code than it really is. Basically, the upper bit of the vertical
	//scroll mode is run through a NAND operation with the lower bit of the horizontal
	//scroll mode, and likewise, the lower bit of the vertical scroll mode is run through
	//a NAND operation with the upper bit of the horizontal scroll mode. This limits the
	//vertical scroll mode in the exact same way the real hardware does, including when
	//invalid scroll modes are being used.
	screenSizeModeV = ((screenSizeModeV & 0x1) & ((~screenSizeModeH & 0x02) >> 1)) | ((screenSizeModeV & 0x02) & ((~screenSizeModeH & 0x01) << 1));

	//Build the playfield block masks. These masks ultimately determine the boundaries of
	//the playfield horizontally and vertically. Hardware testing and analysis on the
	//scrolling behaviour of the VDP indicate that these masks are built by mapping the
	//HSZ and VSZ bits to the upper bits of a 7-bit mask value, as shown below. Note that
	//the invalid mode setting of "10" results in a mask where bit 7 is set, and bit 6 is
	//unset. This has been confirmed through hardware tests.
	//##TODO## Test this on hardware.
	//##TODO## Test and confirm how the window distortion bug interacts with this block
	//mapping selection process.
	unsigned int playfieldBlockMaskX = (screenSizeModeH << 5) | 0x1F;
	unsigned int playfieldBlockMaskY = (screenSizeModeV << 5) | 0x1F;

	//Calculate the row and column numbers for the mapping data. This is simply done by
	//converting the calculated playfield position from a pixel index into a block index,
	//then masking the X and Y coordinates by the horizontal and vertical block masks.
	//This gives us a row and column number, wrapped to the playfield boundaries.
	//##TODO## We want to keep this shift method for calculating the row and column,
	//rather than using division, but we should be using a constant, or at least
	//explaining why the magic number "3" is being used.
	//##TODO## Update these comments
	mappingDataRowNumber &= playfieldBlockMaskY;
	mappingDataColumnNumber <<= 1;
	mappingDataColumnNumber &= playfieldBlockMaskX;

	//Based on the horizontal playfield mode, lookup the row shift count to use when
	//building the final mapping data address value. The column shift count is always
	//fixed to 1.
	unsigned int rowShiftCount = rowShiftCountTable[screenSizeModeH];
	const unsigned int columnShiftCount = 1;

	//Calculate the final mapping data address. Note that the row number is masked with
	//the inverted mask for the column number, so that row data is only allowed to appear
	//where column data is not allowed to appear. This is based on the observed behaviour
	//of the system, as is critical in order to correctly emulate the scrolling behaviour
	//where an invalid horizontal scroll mode of "10" is applied. In this case, the row
	//data can be interleaved with the column data, since the row shift count under this
	//mode is 0.
	unsigned int mappingDataAddress = nameTableBaseAddress | ((mappingDataRowNumber << rowShiftCount) & (~playfieldBlockMaskX << columnShiftCount)) | (mappingDataColumnNumber << columnShiftCount);

	//Mask the lower two bits of the mapping data address, to align the mapping address
	//with a 4-byte boundary. The VDP can only read data from the VRAM in aligned 4-byte
	//blocks, so the lower two bits of the address are ineffective. We read a pair of
	//2-byte block mappings from the masked address.
	mappingDataAddress &= 0xFFFC;

	//##TODO## Clean up the old mapping code below
	////Calculate the target layer mapping data column pair. Note that since we read mapping
	////blocks in pairs, we need to find the next pair of mapping entries, not the next
	////individual mapping entry, which lies at or after the specified playfield position.
	////##TODO## The division operator we use in this function can be avoided. Since the
	////size of a block is always a power of 2, we should be performing a bitshift rather
	////than a full blown division operator. This is certainly what the real hardware would
	////do.
	//unsigned int mappingDataColumnAddress = ((layerScrolledPlayfieldX / (renderSettings.blockPatternSizeX * 2)) * 2) & playfieldBlockMaskX;
	//if((layerScrolledPlayfieldX % (renderSettings.blockPatternSizeX * 2)) != 0)
	//{
	//	//If a block pair is partially visible on the left side of the playfield,
	//	//those blocks are already being displayed using the scrolled block data.
	//	//In this case, advance to the next pair block. Note that this also
	//	//emulates the window distortion bug.
	//	//##TODO## Confirm this emulates the window distortion bug.
	//	mappingDataColumnAddress += 2;
	//}

	////Calculate the target layer mapping data row
	////##TODO## Avoid the division operator here, and use a shift instead.
	//unsigned int mappingDataRowAddress = (layerScrolledPlayfieldY / renderSettings.blockPatternSizeY) & playfieldBlockMaskY;

	////Calculate the final target layer mapping data address
	////##TODO## Continue testing and analysis on the invalid playfield sizes, and determine
	////how the row number is combined into the base address.
	//unsigned int mappingDataAddress = nameTableBaseAddress + ((mappingDataColumnAddress + (mappingDataRowAddress * playfieldBlockSizeX)) * renderSettings.blockMappingEntryByteSize);

	//Read target layer mapping data
	mappingDataEntry1 = ((unsigned int)vram->ReadCommitted(mappingDataAddress+0) << 8) | (unsigned int)vram->ReadCommitted(mappingDataAddress+1);
	mappingDataEntry2 = ((unsigned int)vram->ReadCommitted(mappingDataAddress+2) << 8) | (unsigned int)vram->ReadCommitted(mappingDataAddress+3);
}

//----------------------------------------------------------------------------------------
void S315_5313::ReadPatternDataRow(const LineRenderSettings& renderSettings, Data& patternData, const Data& mappingData, unsigned int patternRowNumber) const
{
	//Calculate the final address for the pattern row, applying vertical flip if it is
	//specified in the block mapping. Note that we grab the entire mapping data block as
	//the block number when calculating the address. This is because the resulting address
	//is wrapped to keep it within the VRAM boundaries. Due to this wrapping, in reality
	//only the lower 11 bits of the mapping data are effective when determining the block
	//number, or the lower 10 bits in the case of interlace mode 2.
	//##TODO## Use low-level bit operations here, and provide a comment showing the
	//structure of the calculated address value.
	unsigned int patternRowNumberWithFlip = mappingData.GetBit(12)? (renderSettings.blockPatternSizeY - 1) - patternRowNumber: patternRowNumber;
	unsigned int patternDataAddress = ((mappingData.GetData() * renderSettings.blockPatternByteSize) + (patternRowNumberWithFlip * renderSettings.blockPatternRowByteSize)) % vramSize;

	//Read the target pattern row
	patternData = ((unsigned int)vram->ReadCommitted(patternDataAddress+0) << 24) | ((unsigned int)vram->ReadCommitted(patternDataAddress+1) << 16) | ((unsigned int)vram->ReadCommitted(patternDataAddress+2) << 8) | (unsigned int)vram->ReadCommitted(patternDataAddress+3);
}

//----------------------------------------------------------------------------------------
void S315_5313::RenderColumnBlockPair(unsigned int columnNumber, unsigned int scrollValueDisplacement, const Data& mappingDataCell1, const Data& mappingDataCell2, const Data& patternDataCell1, const Data& patternDataCell2, std::vector<unsigned int>& outputPaletteLines, std::vector<unsigned int>& outputPaletteEntries, unsigned int& currentRenderPixel) const
{
	//Extract the individual palette index values from the block pattern data, accounting
	//for the horizontal flip flag in the block mappings.
	std::vector<unsigned int> patternEntryArray(16);
	if(mappingDataCell1.GetBit(11))
	{
		patternEntryArray[0] = patternDataCell1.GetDataSegment(0, 4);
		patternEntryArray[1] = patternDataCell1.GetDataSegment(4, 4);
		patternEntryArray[2] = patternDataCell1.GetDataSegment(8, 4);
		patternEntryArray[3] = patternDataCell1.GetDataSegment(12, 4);
		patternEntryArray[4] = patternDataCell1.GetDataSegment(16, 4);
		patternEntryArray[5] = patternDataCell1.GetDataSegment(20, 4);
		patternEntryArray[6] = patternDataCell1.GetDataSegment(24, 4);
		patternEntryArray[7] = patternDataCell1.GetDataSegment(28, 4);
	}
	else
	{
		patternEntryArray[0] = patternDataCell1.GetDataSegment(28, 4);
		patternEntryArray[1] = patternDataCell1.GetDataSegment(24, 4);
		patternEntryArray[2] = patternDataCell1.GetDataSegment(20, 4);
		patternEntryArray[3] = patternDataCell1.GetDataSegment(16, 4);
		patternEntryArray[4] = patternDataCell1.GetDataSegment(12, 4);
		patternEntryArray[5] = patternDataCell1.GetDataSegment(8, 4);
		patternEntryArray[6] = patternDataCell1.GetDataSegment(4, 4);
		patternEntryArray[7] = patternDataCell1.GetDataSegment(0, 4);
	}
	if(mappingDataCell2.GetBit(11))
	{
		patternEntryArray[8] = patternDataCell2.GetDataSegment(0, 4);
		patternEntryArray[9] = patternDataCell2.GetDataSegment(4, 4);
		patternEntryArray[10] = patternDataCell2.GetDataSegment(8, 4);
		patternEntryArray[11] = patternDataCell2.GetDataSegment(12, 4);
		patternEntryArray[12] = patternDataCell2.GetDataSegment(16, 4);
		patternEntryArray[13] = patternDataCell2.GetDataSegment(20, 4);
		patternEntryArray[14] = patternDataCell2.GetDataSegment(24, 4);
		patternEntryArray[15] = patternDataCell2.GetDataSegment(28, 4);
	}
	else
	{
		patternEntryArray[8] = patternDataCell2.GetDataSegment(28, 4);
		patternEntryArray[9] = patternDataCell2.GetDataSegment(24, 4);
		patternEntryArray[10] = patternDataCell2.GetDataSegment(20, 4);
		patternEntryArray[11] = patternDataCell2.GetDataSegment(16, 4);
		patternEntryArray[12] = patternDataCell2.GetDataSegment(12, 4);
		patternEntryArray[13] = patternDataCell2.GetDataSegment(8, 4);
		patternEntryArray[14] = patternDataCell2.GetDataSegment(4, 4);
		patternEntryArray[15] = patternDataCell2.GetDataSegment(0, 4);
	}

	//Extract the palette line numbers from the block mappings
	unsigned int paletteLineCell1 = mappingDataCell1.GetDataSegment(13, 2);
	unsigned int paletteLineCell2 = mappingDataCell2.GetDataSegment(13, 2);

	//##TODO## Make a comment about the window distortion bug
	//Calculate the number of pixels to output for this column, and the initial pixel
	//entry in the pattern buffer to output.
	unsigned int columnPixelEndPosLayerA = (((columnNumber + 1) << 4) + scrollValueDisplacement) & 0x3FF;
	unsigned int pixelsToOutputLayerA = columnPixelEndPosLayerA - currentRenderPixel;
	unsigned int pixelIndexStartValueLayerA = 0x10 - (pixelsToOutputLayerA & 0x0F);

	//Output each pixel for this column
	for(unsigned int i = 0; i < pixelsToOutputLayerA; ++i)
	{
		unsigned int patternEntryBufferIndex = (pixelIndexStartValueLayerA + i) % 0x10;
		outputPaletteEntries[currentRenderPixel] = patternEntryArray[patternEntryBufferIndex];
		outputPaletteLines[currentRenderPixel] = (patternEntryBufferIndex < 8)? paletteLineCell1: paletteLineCell2;
		++currentRenderPixel;
	}
}

//----------------------------------------------------------------------------------------
void S315_5313::DMAWorkerThread()
{
	boost::mutex::scoped_lock lock(workerThreadMutex);

	//##DEBUG##
//	std::wcout << L"DMAWorkerThread running\n";

	//Begin the DMA work loop
	while(workerThreadActive)
	{
		if(!busGranted)
		{
			//##DEBUG##
	//		std::wcout << L"DMAWorkerThread going idle\t" << GetProcessorStateTime() << '\n';
	//		std::wcout << '\t' << workerThreadActive << '\t' << workerThreadPaused << '\t' << busGranted << '\n';

			//If we don't currently have the bus, go idle until a DMA work request comes
			//through.
			workerThreadIdle.notify_all();
			workerThreadUpdate.wait(lock);

			//##DEBUG##
	//		std::wcout << L"DMAWorkerThread going active\t" << GetProcessorStateTime() << '\n';
		}
		else
		{
			//If we currently have the bus, advance the processor state until the DMA
			//operation is complete, or we reach the end of the current timeslice.
			//##FIX## This is incorrect. We know from testing that the FIFO can read as
			//full after a DMA transfer operation has completed, but currently, we advance
			//until the FIFO reads as not full. We need a new kind of parameter for the
			//UpdateInternalState function that will allow us to stop when the DMA
			//transfer is actually completed.
			//##FIX## This is actually seriously breaking stuff. Currently, the FIFO is
			//advanced after DMA operations are advanced. This can mean a new DMA transfer
			//operation, on its first step, can result in no pending data writes in the
			//FIFO after executing the first data transfer. This means we really do need
			//to add that new flag, which is stopWhenNoDMAOperationInProgress.
			double lastTimesliceTotalExecuteTime = currentTimesliceLength + lastTimesliceRemainingTime;
			if((lastTimesliceTotalExecuteTime > 0) && (lastTimesliceTotalExecuteTime > GetProcessorStateTime()))
			{
				//##DEBUG##
//				std::wcout << "VDP - DMAWorkerThread: " << lastTimesliceTotalExecuteTime << '\t' << stateLastUpdateMclk << '\t' << stateLastUpdateMclkUnused << "\n";

				UpdateInternalState(ConvertAccessTimeToMclkCount(lastTimesliceTotalExecuteTime), true, false, false, false, false, true, false);
			}

			//If the DMA transfer has been completed, negate BR to release the bus.
			if(!GetStatusFlagDMA())
			{
				//##DEBUG##
	//			std::wcout << "SetLine - VDP_LINE_BR:\t" << false << '\n';

				//Note that by negating BR, the M68000 should negate BG in response, which
				//will clear the busGranted flag before this call returns.
				memoryBus->SetLine(LINE_BR, Data(GetLineWidth(LINE_BR), 0), GetDeviceContext(), GetDeviceContext(), GetProcessorStateTime());

				//##DEBUG##
	//			std::wcout << "DMA Transfer complete\n";
			}

			//If the VDP still has the bus, but we've reached the end of the current
			//timeslice, suspend the worker thread until the next timeslice is received.
			if(busGranted)
			{
				//##DEBUG##
	//			std::wcout << L"DMAWorkerThread pausing\t" << GetProcessorStateTime() << '\n';
	//			std::wcout << '\t' << workerThreadActive << '\t' << workerThreadPaused << '\t' << busGranted << '\n';

				//Suspend the DMA worker thread until a new timeslice is received.
				workerThreadPaused = true;
				workerThreadIdle.notify_all();
				workerThreadUpdate.wait(lock);
				workerThreadPaused = false;

				//##DEBUG##
	//			std::wcout << L"DMAWorkerThread resuming\t" << GetProcessorStateTime() << '\n';
			}

		}
	}

	//##DEBUG##
//	std::wcout << L"DMAWorkerThread terminating\t" << GetProcessorStateTime() << '\n';
//	std::wcout << '\t' << workerThreadActive << '\t' << busGranted << '\n';

	workerThreadStopped.notify_all();
}

//----------------------------------------------------------------------------------------
//CE line state functions
//----------------------------------------------------------------------------------------
unsigned int S315_5313::GetCELineID(const wchar_t* lineName, bool inputLine) const
{
	std::wstring lineNameString = lineName;
	if(lineNameString == L"LDS")
	{
		return CELINE_LDS;
	}
	else if(lineNameString == L"UDS")
	{
		return CELINE_UDS;
	}
	else if(lineNameString == L"R/W")
	{
		return CELINE_RW;
	}
	else if(lineNameString == L"AS")
	{
		return CELINE_AS;
	}
	else if(lineNameString == L"RMWCycleInProgress")
	{
		return CELINE_RMWCYCLEINPROGRESS;
	}
	else if(lineNameString == L"RMWCycleFirstOperation")
	{
		return CELINE_RMWCYCLEFIRSTOPERATION;
	}
	else if(lineNameString == L"LWR")
	{
		return CELINE_LWR;
	}
	else if(lineNameString == L"UWR")
	{
		return CELINE_UWR;
	}
	else if(lineNameString == L"CAS0")
	{
		return CELINE_CAS0;
	}
	else if(lineNameString == L"RAS0")
	{
		return CELINE_RAS0;
	}
	else if(lineNameString == L"OE0")
	{
		return CELINE_OE0;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void S315_5313::SetCELineInput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber)
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
void S315_5313::SetCELineOutput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber)
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
unsigned int S315_5313::CalculateCELineStateMemory(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, double accessTime) const
{
	bool vdpIsSource = (caller == this->GetDeviceContext());
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
	return BuildCELine(location, vdpIsSource, currentLowerDataStrobe, currentUpperDataStrobe, operationIsWrite, rmwCycleInProgress, rmwCycleFirstOperation);
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::CalculateCELineStateMemoryTransparent(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller) const
{
	bool vdpIsSource = (caller == this->GetDeviceContext());
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
	return BuildCELine(location, vdpIsSource, currentLowerDataStrobe, currentUpperDataStrobe, operationIsWrite, rmwCycleInProgress, rmwCycleFirstOperation);
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::BuildCELine(unsigned int targetAddress, bool vdpIsSource, bool currentLowerDataStrobe, bool currentUpperDataStrobe, bool operationIsWrite, bool rmwCycleInProgress, bool rmwCycleFirstOperation) const
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
	if(rmwCycleInProgress)
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
//The real VDP does its own address line decoding, and doesn't have a dedicated CE line.
//The VDP has 23 address line inputs CA0-CA22, and it will respond to a read or write
//when the address lines match the following state: 110**000 ******** 000****. Note that
//this line decoding is based on how the VDP responds in the Mega Drive. It may be that
//some of the external pins allow the base address of the VDP to be adjusted by grounding
//or lifting certain pins, however if any pins perform this function, they are currently
//undocumented. Target register decoding is based on CA3-CA1:
//3210
//000* - Data Port
//001* - Control Port
//01** - HV Counter
//10** - SN76489 PSG
//110* - Unused
//111* - Test Register
//In our implementation, we allow the system to map the VDP to the bus as if it had a
//dedicated CE line, so this core can be made to respond to access on any address. Target
//register decoding assumes the lower 4 bits of the target address correspond with
//CA3-CA0.
//----------------------------------------------------------------------------------------
IBusInterface::AccessResult S315_5313::ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime)
{
	IBusInterface::AccessResult accessResult(true);
	boost::mutex::scoped_lock lock(accessMutex);
	while(!eventsProcessedForTimeslice)
	{
		eventProcessingCompleted.wait(lock);
	}

	//Convert the access time into a cycle count relative to MCLK, rounding up the result
	//to the nearest MCLK cycle.
	unsigned int accessMclkCycle = ConvertAccessTimeToMclkCount(accessTime + lastTimesliceRemainingTime);
	double accessMclkCycleInAccessTime = ConvertMclkCountToAccessTime(accessMclkCycle);
	if(accessMclkCycleInAccessTime < accessTime)
	{
		accessMclkCycle += 1;
	}

	//##DEBUG##
	if(busGranted)
	{
		std::wcout << "######################################################\n";
		std::wcout << "VDP ReadInterface called when busGranted was set!\n";
		std::wcout << "accessTime:\t" << accessTime << "\n";
		std::wcout << "accessMclkCycle:\t" << accessMclkCycle << "\n";
		std::wcout << "stateLastUpdateMclk:\t" << stateLastUpdateMclk << "\n";
		std::wcout << "stateLastUpdateMclkUnused:\t" << stateLastUpdateMclkUnused << "\n";
		std::wcout << "######################################################\n";
	}

	//##DEBUG##
//	std::wcout << "VDPReadInterface:\t" << std::hex << std::uppercase << location << '\t' << accessMclkCycle << '\t' << GetProcessorStateMclkCurrent() << '\t' << accessTime << '\t' << lastTimesliceRemainingTime << '\n';

	//Check that the access is targeting the VDP, not the embedded PSG.
	if((location & 0xC) != 0x8)
	{
		//Trigger a system rollback if the device has been accessed out of order
		if(lastAccessTime > accessTime)
		{
			GetDeviceContext()->SetSystemRollback(GetDeviceContext(), caller, accessTime);
		}
		lastAccessTime = accessTime;

		//Update the current VDP state
		UpdateInternalState(accessMclkCycle, false, false, false, false, false, false, false);
	}

	//Decode the target address to determine which port was accessed
	switch((location >> 1) & 0x7)
	{
	case 0: //000 - Data Port
		//If the VDP was prepared to latch the next write to the control port as the
		//second half of a command data block, that state is cancelled when the data port
		//is read from or written to, or the control port is read.
		commandWritePending = false;

		//Ensure we have a valid read target loaded
		if(!ValidReadTargetInCommandCode())
		{
			//##TODO## If we reach this point in code, we've attempted to read from an
			//invalid read target. This causes a lockup on the real hardware, as the
			//requested data never becomes available for reading. We should be logging a
			//critical error here.
		}
		else
		{
			//Advance the internal state until we have a data value buffered in the read
			//buffer.
			UpdateInternalState(accessMclkCycle, true, false, false, false, true, false, true);

			//Calculate any delay which occurred in processing this operation, and return
			//the delay time to the caller.
			accessResult.executionTime += (GetProcessorStateTime() - lastTimesliceRemainingTime) - accessTime;
			if(accessResult.executionTime < 0.0)
			{
				//##DEBUG##
				std::wcout << "VDP negative executionTime: " << accessResult.executionTime << '\t' << GetProcessorStateTime() << '\t' << lastTimesliceRemainingTime << '\t' << accessTime << '\n';
			}
		}

		//##DEBUG##
		//##TODO## Keep this check in at runtime, but log a hard error or assertion
		//failure.
		if(!readDataAvailable)
		{
			std::wcout << "VDP Error! Reached M5ReadFIFO when no valid data had been cached!\n";
			std::wcout << "\tCommandCode=0x" << std::hex << std::uppercase << fifoBuffer[fifoNextWriteEntry].codeRegData.GetData() << "\tAddress=0x" << std::hex << std::uppercase << fifoBuffer[fifoNextWriteEntry].addressRegData.GetData() << '\n';
		}

		//Now that we have a data value loaded into the read buffer, return it to the
		//caller.
		data = readBuffer;

		//Now that the cached data has been read from the buffer, the readDataAvailable
		//flag is cleared. This will cause the VDP to read the next word of data from the
		//read target on the next available access slot.
		readDataAvailable = false;
		readDataHalfCached = false;

		break;
	case 1: //001 - Control Port
		//If the VDP was prepared to latch the next write to the control port as the
		//second half of a command data block, that state is cancelled when the data port
		//is read from or written to, or the control port is read.
		commandWritePending = false;

		//Read the current value of the status register. Note that the status register is
		//a 10-bit register, so the call below will only affect the lower 10 bits of the
		//data value.
		data.SetLowerBits(status);

		//##DEBUG##
//		std::wcout << "VDP - ReadSR: 0x" << std::hex << std::uppercase << status.GetData() << '\n';

		//Hardware tests have shown that when reading the status register, only the lower
		//10 lines of the full 16 data lines are touched, corresponding with the 10-bit
		//status register value. The upper 6 lines are left floating. When the M68000
		//reads the status register from the VDP, this has the effect of the upper 6 data
		//lines retaining their previous values from the last bus read operation. We
		//emulate that behaviour here by using data line masking for the result.
		accessResult.accessMaskUsed = true;
		accessResult.accessMask = statusRegisterMask;

		//Since the status register has now been read, clear the sprite overflow and
		//collision flags.
		SetStatusFlagSpriteOverflow(false);
		SetStatusFlagSpriteCollision(false);

		break;
	case 2: //01* - HV Counter
	case 3:
		data = GetHVCounter();
		break;
	case 4: //10* - SN76489 PSG
	case 5:
		if(psg != 0)
		{
			return psg->ReadInterface(0, (location >> 1) & 0x1, data, caller, accessTime);
		}
		break;
	case 6: //110 - Unused
		break;
	case 7: //111 - Test Register
		//##TODO## Perform hardware tests to determine if reads from the test register
		//return valid data.
		break;
	}

	//##DEBUG##
//	std::wcout << "VDPReadInterface:\t" << std::hex << std::uppercase << location << '\t' << data.GetData() << '\t' << accessTime << '\n';

	return accessResult;
}

//----------------------------------------------------------------------------------------
IBusInterface::AccessResult S315_5313::WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime)
{
	IBusInterface::AccessResult accessResult(true);
	boost::mutex::scoped_lock lock(accessMutex);
	while(!eventsProcessedForTimeslice)
	{
		eventProcessingCompleted.wait(lock);
	}

	//Convert the access time into a cycle count relative to MCLK, rounding up the result
	//to the nearest MCLK cycle.
	unsigned int accessMclkCycle = ConvertAccessTimeToMclkCount(accessTime + lastTimesliceRemainingTime);
	double accessMclkCycleInAccessTime = ConvertMclkCountToAccessTime(accessMclkCycle);
	if(accessMclkCycleInAccessTime < accessTime)
	{
		accessMclkCycle += 1;
	}

	//##DEBUG##
	if(busGranted)
	{
		std::wcout << "######################################################\n";
		std::wcout << "VDP ReadInterface called when busGranted was set!\n";
		std::wcout << "accessTime:\t" << accessTime << "\n";
		std::wcout << "accessMclkCycle:\t" << accessMclkCycle << "\n";
		std::wcout << "stateLastUpdateMclk:\t" << stateLastUpdateMclk << "\n";
		std::wcout << "stateLastUpdateMclkUnused:\t" << stateLastUpdateMclkUnused << "\n";
		std::wcout << "######################################################\n";
	}

	//##DEBUG##
//	std::wcout << "VDPWriteInterface: " << std::hex << std::uppercase << location << '\t' << data.GetData() << '\t' << accessMclkCycle << '\t' << GetProcessorStateMclkCurrent() << '\t' << accessTime << '\t' << lastTimesliceRemainingTime << '\n';

	//Check that the access is targeting the VDP, not the embedded PSG.
	//##TODO## Find a neater way to implement this
	if((location & 0xC) != 0x8)
	{
		//Trigger a system rollback if the device has been accessed out of order
		if(lastAccessTime > accessTime)
		{
			GetDeviceContext()->SetSystemRollback(GetDeviceContext(), caller, accessTime);
		}
		lastAccessTime = accessTime;

		//Update the current VDP state to the target access time
		UpdateInternalState(accessMclkCycle, false, false, false, false, false, false, false);
	}

	//Decode the target address to determine which port was accessed
	switch((location >> 1) & 0x7)
	{
	case 0:{ //000 - Data Port
		//If the VDP was prepared to latch the next write to the control port as the
		//second half of a command data block, that state is cancelled when the data port
		//is read from or written to, or the control port is read.
		commandWritePending = false;

		//If the 4-word FIFO is full at the time a data port write is attempted, we
		//advance the device state forward one slot, until there's one space free in the
		//FIFO, and calculate the time during which the accessing device is stalled.
		UpdateInternalState(accessMclkCycle, true, false, false, true, false, false, true);

		//Calculate any delay which occurred in processing this operation, and return the
		//delay time to the caller.
		accessResult.executionTime += (GetProcessorStateTime() - lastTimesliceRemainingTime) - accessTime;
		if(accessResult.executionTime < 0.0)
		{
			//##DEBUG##
			std::wcout << "VDP negative executionTime: " << accessResult.executionTime << '\t' << GetProcessorStateTime() << '\t' << lastTimesliceRemainingTime << '\t' << accessTime << '\n';
		}

		//Save the code and address register data for this data port write
		if(codeAndAddressRegistersModifiedSinceLastWrite)
		{
			fifoBuffer[fifoNextWriteEntry].codeRegData = commandCode;
			fifoBuffer[fifoNextWriteEntry].addressRegData = commandAddress;
			codeAndAddressRegistersModifiedSinceLastWrite = false;
		}
		else
		{
			unsigned int fifoLastWriteEntry = (fifoNextWriteEntry+(fifoBufferSize-1)) % fifoBufferSize;
			fifoBuffer[fifoNextWriteEntry].codeRegData = fifoBuffer[fifoLastWriteEntry].codeRegData;
			fifoBuffer[fifoNextWriteEntry].addressRegData = fifoBuffer[fifoLastWriteEntry].addressRegData + autoIncrementData;
		}

		//Write the new data value to the FIFO
		//##TODO## Check if writes to invalid targets stay in the FIFO until an access
		//slot opens up for each one, at which point they are discarded, but the access
		//slot is consumed, or if they are discarded straight away and the next pending
		//write processed. Most likely, a write to an invalid target consumes an access
		//slot, but we should test by examining the FIFO full/empty flags to be sure.
		fifoBuffer[fifoNextWriteEntry].dataPortWriteData = data;
		fifoBuffer[fifoNextWriteEntry].dataWriteHalfWritten = false;
		fifoBuffer[fifoNextWriteEntry].pendingDataWrite = true;

		//Advance the FIFO to the next slot
		//##FIX## Our tests show that command port writes can be made while the FIFO is
		//full, and the FIFO still reads as full after the write. How is this possible if
		//the command code and address data is written directly to the next available FIFO
		//entry? We should do a test to make sure this case doesn't corrupt or in any way
		//affect the next write in the FIFO, such as sending it to the new address and
		//write target, rather than the address and write target it was intended for, but
		//if this cannot be made to occur, then it seems clear that the command code and
		//address data must be cached separately from the FIFO, and that this data is only
		//written into the FIFO when an actual data port write is made. If this is the
		//case, it supports the possibility that all read processing is actually separate
		//from the write FIFO too.
		fifoNextWriteEntry = (fifoNextWriteEntry+1) % fifoBufferSize;

		//Update the FIFO full and empty flags in the status register
		bool fifoEmpty = IsWriteFIFOEmpty();
		bool fifoFull = IsWriteFIFOFull();
		SetStatusFlagFIFOEmpty(fifoEmpty);
		SetStatusFlagFIFOFull(fifoFull);

		//##TODO## I believe performing a write to a read target prevents subsequent read
		//operations from succeeding. Do more testing to determine if this is actually due
		//to CD4 being set, or rather, some flag or status indicating that data is
		//available for reading being cleared. By performing tests with the write,
		//previous read cache operation, and subsequent read in different states, we
		//should be able to gather more information. If at any point two successive read
		//operations can be made to work after an invalid port write, CD4 cannot be set by
		//a data port write.
		commandCode.SetBit(4, true);

		//##FIX## This comment is incorrect. We didn't just write over it.
		//Now that we've added a new write to the FIFO, we invalidate any cached read data
		//that may have been buffered, since if it existed, we just wrote over it.
		readDataAvailable = false;
		readDataHalfCached = false;
		break;}
	case 1:{ //001 - Control Port
		//It is almost certain that control port writes are not processed until the FIFO
		//is empty, otherwise the changes which are made to the the command code and
		//address register would interfere with pending writes held in the FIFO. See the
		//thread at http://gendev.spritesmind.net/forum/viewtopic.php?t=790 for
		//discussion. We emulate that here by forcing control port writes to wait until
		//the FIFO is empty.
		//##FIX## Hardware tests have now shown this assumption to be incorrect. Data port
		//writes and control port writes can both be made regardless of the current state
		//of the FIFO, so each entry in the FIFO must somehow cache its write address and
		//write target. Also note that control port writes don't seem to delay the
		//progression of the FIFO, and we have specifically tested a FIFO that had half
		//its writes going to one write target, and half the other writes going to another
		//write target, at different addresses, with the FIFO full and a full control port
		//write while the FIFO was not empty, followed by data port writes in order to set
		//all this up. All data was written correctly and was verified by reading it back.
		//This clearly demonstrates that the write FIFO must latch the current target
		//address and code register data, or at the very least, the write target from the
		//code register data. Writing a read target to the code register has no apparent
		//effect, but performing a data port read does fully flush the FIFO before
		//returning the read data. After a data port read, the FIFO is always empty.
//		UpdateInternalState(accessMclkCycle, true, true, false, false, false, true);

		//Calculate any delay which occurred in processing this operation, and return the
		//delay time to the caller.
		accessResult.executionTime += (GetProcessorStateTime() - lastTimesliceRemainingTime) - accessTime;
		if(accessResult.executionTime < 0.0)
		{
			//##DEBUG##
			std::wcout << "VDP negative executionTime: " << accessResult.executionTime << '\t' << GetProcessorStateTime() << '\t' << lastTimesliceRemainingTime << '\t' << accessTime << '\n';
		}

		//Calculate the number of mclk cycles that this operation was delayed
		unsigned int accessMclkCycleDelay = GetProcessorStateMclkCurrent() - accessMclkCycle;

		//##FIX## Testing has indicated that control port writes are ignored while DMA
		//operations are in progress. Note that this does not simply mean when CD5 is set
		//however, as control port writes can be made while a DMA fill operation is setup,
		//but waiting for a data port write to trigger it, and we know from current
		//testing that the DMA busy flag reads as set after a DMA fill has been setup, but
		//before the fill data is written.
		//##TODO## Confirm that control port writes can be made while a DMA fill operation
		//is waiting for a data port write
		//##TODO## Check if the DMA busy flag remains set if a partial control port write
		//is made while a DMA fill operation is waiting for a data port write, in
		//particular when we write the same data over again for the first command word. If
		//it does not, it would appear that CD5 is cleared when a partial control port
		//write is made.
		if(!commandWritePending)
		{
			ProcessCommandDataWriteFirstHalf(data);

			//Once a partial control port write has been made, hardware tests indicate
			//that CD4 is set. This occurs even if the partial control port write is
			//interpreted as a register write. CD4 is most likely set at this time for
			//performance reasons. When CD4 is set, no read targets are valid. If a valid
			//read target could be setup by a partial control port write, read pre-caching
			//could begin between the two halves of a full control port write, which would
			//be a bad thing if the indicated read target after the first control port
			//write was not actually what the caller wanted to access.
			commandCode.SetBit(4, true);

			//Contrary to other documentation, and the behaviour of other emulators,
			//register writes go directly to the code and address registers first as a
			//normal command word write. This is the cause of the apparent "clearing" of
			//the code register when a register write occurs, as reported by Charles
			//MacDonald. The register write is then processed from the new address
			//register data. Hardware tests have shown the upper bits of the command and
			//address registers remain unaffected.
			if(commandCode.GetBit(1) && !commandCode.GetBit(0))
			{
				//Register Write
				//-----------------------------------------------------------------
				//|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
				//|---------------------------------------------------------------|
				//| 1 | 0 | / |    Register No    |             Data              |
				//|---------------------------------------------------------------|
				//|CD1|CD0|A13|A12|A11|A10|A9 |A8 |A7 |A6 |A5 |A4 |A3 |A2 |A1 |A0 |
				//-----------------------------------------------------------------

				//Perform the register write
				unsigned int registerNo = originalCommandAddress.GetDataSegment(8, 5);
				Data registerData(8, originalCommandAddress.GetDataSegment(0, 8));
				if(registerNo < registerCount)
				{
					RegisterSpecialUpdateFunction(accessMclkCycle + accessMclkCycleDelay, accessTime, accessResult.executionTime, caller, registerNo, registerData);
					AccessTarget accessTarget;
					accessTarget.AccessTime(accessMclkCycle + accessMclkCycleDelay);
					SetRegisterData(registerNo, accessTarget, registerData);
				}

				//Since the command word has just been latched as a register write, the
				//VDP is no longer expecting a second command word to be written.
				commandWritePending = false;
			}
		}
		else
		{
			ProcessCommandDataWriteSecondHalf(data);

			//Hardware tests have shown that when the DMA enable bit is cleared, the state
			//of CD5 is also forced to clear. This prevents DMA operations, for example,
			//being triggered at the time the DMA enable bit is set, due to a previous
			//control port write setting CD5.
			//##TODO## Perform more hardware tests to confirm if CD5 is actually forced to
			//0 immediately, or if it happens on the next access slot.
			if(!dmaEnabled)
			{
				commandCode.SetBit(5, false);
			}

			//Keep the state of CD5 and the DMA flag in the status register in sync. On
			//the real hardware, the DMA flag in the status register is the literal
			//realtime state of the CD5 bit in the command code register. Note that this
			//means that the DMA bit in the status register is set when a control port
			//write has been made to setup a DMA fill operation, but the data port write
			//has not yet been made to trigger the fill. This is the way the real VDP
			//behaves, and this has been confirmed though hardware tests. Although the
			//status register returns the realtime state of CD5 for the DMA busy flag, we
			//keep the two separated in our core for maximum efficiency on status register
			//reads, and keep them manually in sync whenever the CD5 bit is modified.
			SetStatusFlagDMA(commandCode.GetBit(5));

			//Check if we need to obtain the bus due to an external DMA transfer operation
			if(dmaEnabled             //DMA is enabled
			&& !dmd1                  //DMA is set to perform an external copy operation
			&& commandCode.GetBit(5)) //The command code has a DMA operation flagged
			{
				//Note that a DMA operation will be triggered regardless of the state of
				//CD4, as this bit isn't tested when deciding whether to trigger DMA, and
				//write target decoding only looks at the state of CD3-CD0.

				//Note that at this point, the VDP will request the bus. If the bus
				//request is granted, the DMA worker thread will go active to process the
				//transfer. When the transfer is completed, the VDP will release the bus.
				//##TODO## Determine what happens if the bus request is not granted
				//straight away. IE, What happens if another control port write occurs
				//before the bus is granted? Does the VDP process the write, or does it
				//lock up until the bus is granted? We can test this by physically
				//preventing the VDP from asserting the BR line, so that the M68000 never
				//grants the bus, and see how the VDP responds if we continue to access
				//it.
				//##TODO## Our M68000 core currently doesn't support granting the bus
				//between the two halves of a long-word memory write, but we need to
				//support this here, as a DMA transfer could be triggered by the first
				//word of a long-word write to the control port. Find a way to implement
				//support for the behaviour we need.
				boost::mutex::scoped_lock lock(workerThreadMutex);
				memoryBus->SetLine(LINE_BR, Data(GetLineWidth(LINE_BR), 1), GetDeviceContext(), GetDeviceContext(), accessTime + accessResult.executionTime);

				//Set the initial DMA transfer register settings
				dmaTransferActive = true;
				dmaTransferReadDataCached = false;
				dmaTransferNextReadMclk = accessMclkCycle;
				dmaTransferLastTimesliceUnusedMclkCycles = 0;

				//##DEBUG##
		//		std::wcout << "SetLine - VDP_LINE_BR:\t" << true << '\n';
			}
		}

		//Hardware tests have shown that the command address is cached internally,
		//separate from the command address register that gets written to on a command
		//port write. When the command address is incremented during read and write
		//operations, it is this internal copy of the command address which gets
		//incremented. Even doing a partial command port write will cause the internal
		//command address register to be fully reloaded from the original command address.
		//Even if the upper 2 bits of the command address have been changed since the
		//original address was setup, they will be reloaded on a partial command port
		//write. We emulate that behaviour here by keeping a separate copy of the command
		//address in its original, unmodified state.
		//##TODO## Recent hardware tests dispute these findings. Do more testing to
		//determine what the correct behaviour is.
		commandAddress = originalCommandAddress;

		//Flag that the command and address registers have been modified since the last
		//write. This is important in order to ensure that subsequent data port writes
		//latch the new data, rather than incrementing the write address from the previous
		//write target.
		codeAndAddressRegistersModifiedSinceLastWrite = true;

		//Now that the command code has changed, any cached read data is invalidated. We
		//clear this flag here now so that new data will be retrieved for the target on
		//the next available access slot, if it is a valid read target.
		readDataAvailable = false;
		readDataHalfCached = false;
		break;}
	case 2: //01* - HV Counter
	case 3:
		break;
	case 4: //10* - SN76489 PSG
	case 5:
		if(psg != 0)
		{
			return psg->WriteInterface(0, (location >> 1) & 0x1, data, caller, accessTime);
		}
		break;
	case 6: //110 - Unused
		break;
	case 7: //111 - Test Register
		//##TODO## Implement test register writes
		break;
	}

	return accessResult;
}

//----------------------------------------------------------------------------------------
//Port functions
//----------------------------------------------------------------------------------------
Data S315_5313::GetHVCounter() const
{
	//Since the status register has already been updated when the device was accessed,
	//build the hv counter based on the current processor state.

	//Build the 8-bit external hcounter
	//The internal hcounter is 9-bit, and the external hcounter is 8-bit. The upper 8 bits
	//of the 9-bit internal hcounter are used to build the 8-bit external hcounter, as
	//shown below:
	//Internal counter: 876543210
	//External counter: 87654321
	Data currentHCounter(9);
	currentHCounter = (hvCounterLatched)? hcounterLatchedData: hcounter;
	Data externalHCounter(8);
	externalHCounter = currentHCounter.GetUpperBits(8);

	//Build the 8-bit external vcounter
	//The internal vcounter is 9-bit, and the external vcounter is 8-bit. The way the
	//internal counter is mapped to the external counter depends on the interlace mode.
	//Given that the internal vcounter is 9-bit, the way the bits are mapped from the
	//internal counter to the external counter is given below:
	//Internal counter:           876543210
	//External, no Interlacing:    76543210
	//External, interlace normal:  76543218
	//External, interlace double:  65432107
	Data currentVCounter(9);
	currentVCounter = (hvCounterLatched)? vcounterLatchedData: vcounter;
	Data externalVCounter(8);
	if(interlaceEnabled && interlaceDouble)
	{
		currentVCounter <<= 1;
	}
	externalVCounter = currentVCounter.GetLowerBits(8);
	if(interlaceEnabled)
	{
		externalVCounter.SetBit(0, currentVCounter.GetBit(8));
	}

	//Build and return the combined 16-bit external hv counter
	//-----------------------------------------------------------------
	//| 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	//|-------------------------------|-------------------------------|
	//|   External V counter (8-bit)  |   External H counter (8-bit)  |
	//|---------------------------------------------------------------|
	//|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	//|---------------------------------------------------------------|
	//|               External HV counter (16-bit)                    |
	//-----------------------------------------------------------------
	Data externalHVCounter(16);
	externalHVCounter.SetUpperHalf(externalVCounter);
	externalHVCounter.SetLowerHalf(externalHCounter);

	//##DEBUG##
	//std::wcout << "VDP - ReadHVCounter: 0x" << std::hex << std::uppercase << externalHVCounter.GetData() << '\n';

	return externalHVCounter;
}

//----------------------------------------------------------------------------------------
void S315_5313::ProcessCommandDataWriteFirstHalf(const Data& data)
{
	//Command data first half
	//-----------------------------------------------------------------
	//|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	//|---------------------------------------------------------------|
	//| Code  |                    Address                            |
	//|-------|-------------------------------------------------------|
	//|CD1|CD0|A13|A12|A11|A10|A9 |A8 |A7 |A6 |A5 |A4 |A3 |A2 |A1 |A0 |
	//-----------------------------------------------------------------
	commandWritePending = true;
	commandCode.SetDataSegment(0, 2, data.GetDataSegment(14, 2));
	originalCommandAddress.SetDataSegment(0, 14, data.GetDataSegment(0, 14));
}

//----------------------------------------------------------------------------------------
void S315_5313::ProcessCommandDataWriteSecondHalf(const Data& data)
{
	//Command data second half
	//-----------------------------------------------------------------
	//|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	//|---------------------------------------------------------------|
	//|                               |     Code      |       |Address|
	//| /   /   /   /   /   /   /   / |---------------| /   / |-------|
	//|                               |CD5|CD4|CD3|CD2|       |A15|A14|
	//-----------------------------------------------------------------
	commandWritePending = false;
	commandCode.SetDataSegment(2, 4, data.GetDataSegment(4, 4));
	originalCommandAddress.SetDataSegment(14, 2, data.GetDataSegment(0, 2));
}

//----------------------------------------------------------------------------------------
void S315_5313::RegisterSpecialUpdateFunction(unsigned int mclkCycle, double accessTime, double accessDelay, IDeviceContext* caller, unsigned int registerNo, const Data& data)
{
	switch(registerNo)
	{
	case 0:
		hvCounterLatchEnabled = data.GetBit(1);
		if(hintEnabled != data.GetBit(4))
		{
			hintEnabled = data.GetBit(4);
			if(hintEnabled)
			{
				if(hintPending)
				{
					//Trigger HInt for M68000
					memoryBus->SetLine(LINE_IPL, Data(GetLineWidth(LINE_IPL), hintIPLLineState), GetDeviceContext(), caller, accessTime + accessDelay);
					hintPending = false;
				}
				else
				{
					//If horizontal interrupts have changed from disabled to enabled, and
					//there wasn't already a horizontal interrupt pending, trigger a
					//rollback, otherwise there's a chance a horizontal interrupt might
					//not be flagged at the correct time, since the GetNextEvent function
					//might not have generated a timing point for the hint counter
					//expiring.
					GetDeviceContext()->SetSystemRollback(GetDeviceContext(), caller, accessTime);
				}
			}
		}
		break;
	case 1:
		screenModeV30Cached = data.GetBit(3);
		dmaEnabled = data.GetBit(4);

		if(vintEnabled != data.GetBit(5))
		{
			vintEnabled = data.GetBit(5);
			if(vintEnabled && vintPending)
			{
				//Trigger VInt for M68000
				memoryBus->SetLine(LINE_IPL, Data(GetLineWidth(LINE_IPL), vintIPLLineState), GetDeviceContext(), GetDeviceContext(), accessTime + accessDelay);
				vintPending = false;
			}
			//Note that we never need to generate a rollback if the VINT pending state is
			//changed. We always generate a timing point for the VINT event, since the Z80
			//INT line always generates a VINT, even when the VINT enable bit isn't set.
		}
		break;
	case 10:
		if(hintCounterReloadValue != data.GetData())
		{
			hintCounterReloadValue = data.GetData();
			//If the HINT counter data has been modified while hint is disabled, trigger a
			//rollback.
			if(!hintEnabled)
			{
				GetDeviceContext()->SetSystemRollback(GetDeviceContext(), caller, accessTime);
			}
		}
		break;
	case 11:
		if(exintEnabled != data.GetBit(3))
		{
			exintEnabled = data.GetBit(3);
			if(exintEnabled && exintPending)
			{
				//Trigger EXInt for M68000
				memoryBus->SetLine(LINE_IPL, Data(GetLineWidth(LINE_IPL), exintIPLLineState), GetDeviceContext(), caller, accessTime + accessDelay);
				exintPending = false;
			}
			else
			{
				//##TODO## Figure out what we really need to do here. Since external
				//interrupts aren't implemented yet, it's a bit hard to be sure.
//				//If external interrupts have changed from disabled to enabled, trigger a rollback.
//				GetDeviceContext()->SetSystemRollback(GetDeviceContext(), caller, accessTime);
			}
		}
		break;
	case 12:
		screenModeH40Cached = data.GetBit(0);
		interlaceEnabledCached = data.GetBit(1);
		interlaceDoubleCached = data.GetBit(2);
		break;
	case 15:
		autoIncrementData = data.GetData();
		break;
	case 19:
		dmaLengthCounter = (dmaLengthCounter & 0xFF00) | data.GetData();
		break;
	case 20:
		dmaLengthCounter = (dmaLengthCounter & 0x00FF) | (data.GetData() << 8);
		break;
	case 21:
		dmaSourceAddressByte1 = data.GetData();
		break;
	case 22:
		dmaSourceAddressByte2 = data.GetData();
		break;
	case 23:
		dmaSourceAddressByte3 = data.GetDataSegment(0, 7);
		dmd0 = data.GetBit(6);
		dmd1 = data.GetBit(7);
		break;
	}
}

//----------------------------------------------------------------------------------------
//HV counter internal/linear conversion
//----------------------------------------------------------------------------------------
unsigned int S315_5313::HCounterValueFromVDPInternalToLinear(const HScanSettings& hscanSettings, unsigned int hcounterCurrent)
{
	return (hcounterCurrent >= hscanSettings.hcounterBlankingInitialValue)? hscanSettings.hcounterActiveScanMaxValue + ((hcounterCurrent - hscanSettings.hcounterBlankingInitialValue) + 1): hcounterCurrent;
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::VCounterValueFromVDPInternalToLinear(const VScanSettings& vscanSettings, unsigned int vcounterCurrent, bool oddFlagSet)
{
	unsigned int blankingInitialValue = oddFlagSet? vscanSettings.vcounterBlankingInitialValueOddFlag: vscanSettings.vcounterBlankingInitialValue;
	return (vcounterCurrent >= blankingInitialValue)? vscanSettings.vcounterActiveScanMaxValue + ((vcounterCurrent - blankingInitialValue) + 1): vcounterCurrent;
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::HCounterValueFromLinearToVDPInternal(const HScanSettings& hscanSettings, unsigned int hcounterCurrent)
{
	return (hcounterCurrent > hscanSettings.hcounterActiveScanMaxValue)? ((hcounterCurrent - hscanSettings.hcounterActiveScanMaxValue) - 1) + hscanSettings.hcounterBlankingInitialValue: hcounterCurrent;
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::VCounterValueFromLinearToVDPInternal(const VScanSettings& vscanSettings, unsigned int vcounterCurrent, bool oddFlagSet)
{
	unsigned int blankingInitialValue = oddFlagSet? vscanSettings.vcounterBlankingInitialValueOddFlag: vscanSettings.vcounterBlankingInitialValue;
	return (vcounterCurrent > vscanSettings.vcounterActiveScanMaxValue)? ((vcounterCurrent - vscanSettings.vcounterActiveScanMaxValue) - 1) + blankingInitialValue: vcounterCurrent;
}

//----------------------------------------------------------------------------------------
//Video scan settings functions
//----------------------------------------------------------------------------------------
const S315_5313::HScanSettings& S315_5313::GetHScanSettings(bool screenModeH40Active)
{
	//----------------------------------------------------
	//|        Video |PAL/NTSC         |PAL/NTSC         |
	//|         Mode |H32     (RSx=00) |H40     (RSx=11) |
	//|              |V28/V30 (M2=*)   |V28/V30 (M2=*)   |
	//| Test         |Int any (LSMx=**)|Int any (LSMx=**)|
	//|--------------------------------------------------|
	//|HCounter      |[1]0x000-0x127   |[1]0x000-0x16C   |
	//|progression   |[2]0x1D2-0x1FF   |[2]0x1C9-0x1FF   |
	//|9-bit internal|                 |                 |
	//|--------------------------------------------------|
	//|VCounter      |HCounter changes |HCounter changes | //##TODO## Confirm these values
	//|increment     |from 0x84 to 0x85|from 0xA4 to 0xA5|
	//|              |in [1].          |in [1].          |
	//|--------------------------------------------------|
	//|HBlank set    |HCounter changes |HCounter changes |
	//|              |from 0x125 to    |from 0x165 to    |
	//|              |0x126 in [1].    |0x166 in [1].    |
	//|--------------------------------------------------|
	//|HBlank cleared|HCounter changes |HCounter changes |
	//|              |from 0x009 to    |from 0x00A to    |
	//|              |0x00A in [1].    |0x00B in [1].    |
	//|--------------------------------------------------|
	//|F flag set    |HCounter changes |HCounter changes | //##TODO## Confirm these values
	//|              |from 0x00 to 0x01|from 0x00 to 0x01|
	//|              |at start of [1]  |at start of [1]  |
	//|--------------------------------------------------|
	//|ODD flag      |HCounter changes |HCounter changes | //##TODO## Confirm these values
	//|toggled       |from 0x00 to 0x01|from 0x00 to 0x01|
	//|              |at start of [1]  |at start of [1]  |
	//|--------------------------------------------------|
	//|HINT flagged  |HCounter changes |HCounter changes | //##TODO## Confirm these values
	//|via IPL lines |from 0x0EE to    |from 0x12E to    |
	//|              |0x0EF in [1].    |0x12F in [1].    |
	//|--------------------------------------------------|
	//|VINT flagged  |                 |                 | //##TODO## Measure these values
	//|via IPL lines |                 |                 |
	//|              |                 |                 |
	//|--------------------------------------------------|
	//|HSYNC asserted|HCounter changes |HCounter changes |
	//|              |from 0x1D8 to    |from 0x1CC to    |
	//|              |0x1D9 in [2].    |0x1CD in [2].    |
	//|--------------------------------------------------|
	//|HSYNC negated |HCounter changes |HCounter changes |
	//|              |from 0x1F2 to    |from 0x1EC to    |
	//|              |0x1F3 in [2].    |0x1ED in [2].    |
	//----------------------------------------------------
	//##TODO## Hcounter values shown in the table above as 8-bit values have only been
	//tested in hardware to 8 bits of accuracy. We've taken a guess as to the 9-bit
	//internal values to populate the settings structures below. Perform analysis on the
	//real hardware to determine the true 9-bit values.
	//##TODO## The values for HINT flagged are based on an educated guess. Confirm them
	//through hardware tests.
	static const HScanSettings h32ScanSettings(0x127, 0x1D2, 0x10A, 0x126, 0x00A, 0x001, 0x002, 0x1FF, 0x0EF, 0x000, 0x1D9, 0x1F3, 13, 256, 14);
	static const HScanSettings h40ScanSettings(0x16C, 0x1C9, 0x14A, 0x166, 0x00B, 0x001, 0x002, 0x1FF, 0x12F, 0x000, 0x1CD, 0x1ED, 13, 320, 14);
	return (screenModeH40Active)? h40ScanSettings: h32ScanSettings; 
}

//----------------------------------------------------------------------------------------
const S315_5313::VScanSettings& S315_5313::GetVScanSettings(bool screenModeV30Active, bool palModeActive, bool interlaceActive)
{
	//----------------------------------------------------------------------------------------
	//|        Video |PAL              |PAL              |PAL              |PAL              |
	//|         Mode |H32/H40(RSx00/11)|H32/H40(RSx00/11)|H32/H40(RSx00/11)|H32/H40(RSx00/11)|
	//|              |V28     (M2=0)   |V28     (M2=0)   |V30     (M2=1)   |V30     (M2=1)   |
	//| Test         |Int none(LSMx=*0)|Int both(LSMx=*1)|Int none(LSMx=*0)|Int both(LSMx=*1)|
	//|--------------------------------------------------------------------------------------|
	//|VCounter      |[1]0x000-0x102   |[1]0x000-0x101   |[1]0x000-0x10A   |[1]0x000-0x109   |
	//|progression   |[2]0x1CA-0x1FF   |[2]0x1C9(#ODD)   |[2]0x1D2-0x1FF   |[2]0x1D1(#ODD)   |
	//|9-bit internal|                 |[3]0x1CA-0x1FF   |                 |[3]0x1D2-0x1FF   |
	//|--------------------------------------------------------------------------------------|
	//|VBlank set    |Vcounter changes |                 |Vcounter changes |                 |
	//|              |from 0x0DF to    |     <Same>      |from 0x0EF to    |     <Same>      |
	//|              |0x0E0 in [1].    |                 |0x0F0 in [1].    |                 |
	//|--------------------------------------------------------------------------------------|
	//|VBlank cleared|Vcounter changes |                 |Vcounter changes |                 |
	//|              |from 0x1FE to    |     <Same>      |from 0x1FE to    |     <Same>      |
	//|              |0x1FF in [2].    |                 |0x1FF in [2].    |                 |
	//|--------------------------------------------------------------------------------------|
	//|F flag set    |while VCounter is|                 |while VCounter is|                 |
	//|              |set to 0x0E0 in  |     <Same>      |set to 0x0F0 in  |     <Same>      |
	//|              |[1].             |                 |[1].             |                 |
	//|--------------------------------------------------------------------------------------|
	//|VSYNC asserted|Vcounter changes |                 |Vcounter changes |                 |
	//|              |from 0x0FF to    |     <Same>      |from 0x107 to    |     <Same>      |
	//|              |0x100 in [1].    |                 |0x108 in [1].    |                 |
	//|--------------------------------------------------------------------------------------|
	//|VSYNC cleared |Vcounter changes |                 |Vcounter changes |                 |
	//|              |from 0x1D9 to    |     <Same>      |from 0x1E1 to    |     <Same>      |
	//|              |0x1DA in [1].    |                 |0x1E2 in [1].    |                 |
	//|--------------------------------------------------------------------------------------|
	//|ODD flag      |while VCounter is|                 |while VCounter is|                 |
	//|toggled       |set to 0x0E0 in  |     <Same>      |set to 0x0F0 in  |     <Same>      |
	//|              |[1].             |                 |[1].             |                 |
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	//|        Video |NTSC             |NTSC             |NTSC             |NTSC             |
	//|         Mode |H32/H40(RSx00/11)|H32/H40(RSx00/11)|H32/H40(RSx00/11)|H32/H40(RSx00/11)|
	//|              |V28     (M2=0)   |V28     (M2=0)   |V30     (M2=1)   |V30     (M2=1)   |
	//| Test         |Int none(LSMx=*0)|Int both(LSMx=*1)|Int none(LSMx=*0)|Int both(LSMx=*1)|
	//|--------------------------------------------------------------------------------------|
	//|VCounter      |[1]0x000-0x0EA   |[1]0x000-0x0EA   |[1]0x000-0x1FF   |[1]0x000-0x1FF   |
	//|progression   |[2]0x1E5-0x1FF   |[2]0x1E4(#ODD)   |                 |                 |
	//|9-bit internal|                 |[3]0x1E5-0x1FF   |                 |                 |
	//|--------------------------------------------------------------------------------------|
	//|VBlank set    |Vcounter changes |                 |Vcounter changes |                 |
	//|              |from 0x0DF to    |     <Same>      |from 0x0EF to    |     <Same>      |
	//|              |0x0E0 in [1].    |                 |0x0F0 in [1].    |                 |
	//|--------------------------------------------------------------------------------------|
	//|VBlank cleared|Vcounter changes |                 |Vcounter changes |                 |
	//|              |from 0x1FE to    |     <Same>      |from 0x1FE to    |     <Same>      |
	//|              |0x1FF in [2].    |                 |0x1FF in [1].    |                 |
	//|--------------------------------------------------------------------------------------|
	//|F flag set    |while VCounter is|                 |while VCounter is|                 |
	//|              |set to 0x0E0 in  |     <Same>      |set to 0x0F0 in  |     <Same>      |
	//|              |[1].             |                 |[1].             |                 |
	//|--------------------------------------------------------------------------------------|
	//|VSYNC asserted|Vcounter changes |                 |      Never      |                 |
	//|              |from 0x0E7 to    |     <Same>      |                 |     <Same>      |
	//|              |0x0E8 in [1].    |                 |                 |                 |
	//|--------------------------------------------------------------------------------------|
	//|VSYNC cleared |Vcounter changes |                 |      Never      |                 |
	//|              |from 0x1F4 to    |     <Same>      |                 |     <Same>      |
	//|              |0x1F5 in [1].    |                 |                 |                 |
	//|--------------------------------------------------------------------------------------|
	//|ODD flag      |while VCounter is|                 |while VCounter is|                 |
	//|toggled       |set to 0x0E0 in  |     <Same>      |set to 0x0F0 in  |     <Same>      |
	//|              |[1].             |                 |[1].             |                 |
	//----------------------------------------------------------------------------------------
	//#ODD - Runs only when the ODD flag is set
	//##TODO## Evaluate the way we're using the first parameter vcounterActiveScanMaxValue
	//in code. Note that we subtract 1 from this value for the interlace values in PAL
	//mode, but we use the same values for both interlace and non-interlace modes in NTSC.
	//Confirm if this is correct.
	static const VScanSettings v28PalNoIntScanSettings(0x102, 0x1CA, 0x1C9, 0x0E0, 0x1FF, 0x1FF, 0x100, 0x1DA, 313);
	static const VScanSettings v28PalIntEnScanSettings(0x101, 0x1CA, 0x1C9, 0x0E0, 0x1FF, 0x1FF, 0x100, 0x1DA, 313);
	static const VScanSettings v30PalNoIntScanSettings(0x10A, 0x1D2, 0x1D1, 0x0F0, 0x1FF, 0x1FF, 0x108, 0x1E2, 313);
	static const VScanSettings v30PalIntEnScanSettings(0x109, 0x1D2, 0x1D1, 0x0F0, 0x1FF, 0x1FF, 0x108, 0x1E2, 313);
	static const VScanSettings v28NtscNoIntScanSettings(0x0EA, 0x1E5, 0x1E4, 0x0E0, 0x1FF, 0x1FF, 0x0E8, 0x1F5, 262);
	static const VScanSettings v28NtscIntEnScanSettings(0x0EA, 0x1E5, 0x1E4, 0x0E0, 0x1FF, 0x1FF, 0x0E8, 0x1F5, 262);
	static const VScanSettings v30NtscNoIntScanSettings(0x1FF, 0x200, 0x200, 0x0F0, 0x1FF, 0x1FF, 0x1FF, 0x1FF, 262);
	static const VScanSettings v30NtscIntEnScanSettings(0x1FF, 0x200, 0x200, 0x0F0, 0x1FF, 0x1FF, 0x1FF, 0x1FF, 262);

	//Select the scan settings which correspond with the current screen mode
	if(palModeActive)
	{
		if(screenModeV30Active)
		{
			return (interlaceActive)? v30PalIntEnScanSettings: v30PalNoIntScanSettings;
		}
		else
		{
			return (interlaceActive)? v28PalIntEnScanSettings: v28PalNoIntScanSettings;
		}
	}
	else
	{
		if(screenModeV30Active)
		{
			return (interlaceActive)? v30NtscIntEnScanSettings: v30NtscNoIntScanSettings;
		}
		else
		{
			return (interlaceActive)? v28NtscIntEnScanSettings: v28NtscNoIntScanSettings;
		}
	}
}

//----------------------------------------------------------------------------------------
//HV counter comparison functions
//----------------------------------------------------------------------------------------
unsigned int S315_5313::GetPixelClockStepsBetweenHVCounterValues(const HScanSettings& hscanSettings, unsigned int hcounterCurrent, unsigned int hcounterTarget, const VScanSettings& vscanSettings, bool interlaceIsEnabled, bool oddFlagSet, unsigned int vcounterCurrent, unsigned int vcounterTarget)
{
	unsigned int totalPixelClockSteps = 0;

	//Check if the hcounter is going to pass the vcounter increment point when advancing
	//it from its current position to the target position
	if(((hcounterCurrent < hcounterTarget) && (hcounterCurrent < hscanSettings.vcounterIncrementPoint) && (hcounterTarget >= hscanSettings.vcounterIncrementPoint))
	|| ((hcounterCurrent > hcounterTarget) && ((hcounterCurrent < hscanSettings.vcounterIncrementPoint) || (hcounterTarget >= hscanSettings.vcounterIncrementPoint))))
	{
		//If the hcounter advancement is going to increment the vcounter, advance the
		//hcounter up to the vcounter increment point, and increment the vcounter.
		totalPixelClockSteps += GetPixelClockStepsBetweenHCounterValues(hscanSettings, hcounterCurrent, hscanSettings.vcounterIncrementPoint);
		hcounterCurrent = hscanSettings.vcounterIncrementPoint;
		vcounterCurrent = AddStepsToVCounter(hscanSettings, hcounterCurrent, vscanSettings, interlaceIsEnabled, oddFlagSet, vcounterCurrent, 1);
	}

	//Calculate the total number of steps required to advance the vcounter to its final
	//position
	if(vcounterCurrent != vcounterTarget)
	{
		//##FIX## This function first advances the hcounter to the vcounter increment point,
		//then leaves it there and advances the vcounter.
		totalPixelClockSteps += GetPixelClockStepsBetweenVCounterValues(hscanSettings, hcounterCurrent, vscanSettings, interlaceIsEnabled, oddFlagSet, vcounterCurrent, vcounterTarget);
		hcounterCurrent = hscanSettings.vcounterIncrementPoint;
	}

	//Calculate the total number of steps required to advance the hcounter to its final
	//position
	totalPixelClockSteps += GetPixelClockStepsBetweenHCounterValues(hscanSettings, hcounterCurrent, hcounterTarget);

	return totalPixelClockSteps;
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::GetPixelClockStepsBetweenHCounterValues(const HScanSettings& hscanSettings, unsigned int hcounterCurrent, unsigned int hcounterTarget)
{
	//Convert the internal hcounter values into linear values
	hcounterCurrent = HCounterValueFromVDPInternalToLinear(hscanSettings, hcounterCurrent);
	hcounterTarget = HCounterValueFromVDPInternalToLinear(hscanSettings, hcounterTarget);

	//Calculate the number of pixel clock steps required to advance the current hcounter
	//to the target value.
	return ((hcounterTarget + hscanSettings.hcounterStepsPerIteration) - hcounterCurrent) % hscanSettings.hcounterStepsPerIteration;
}

//----------------------------------------------------------------------------------------
//##TODO## What happens if the target vcounter is the extra line in an odd interlace frame?
unsigned int S315_5313::GetPixelClockStepsBetweenVCounterValues(const HScanSettings& hscanSettings, unsigned int hcounterCurrent, const VScanSettings& vscanSettings, bool interlaceIsEnabled, bool oddFlagSet, unsigned int vcounterCurrent, unsigned int vcounterTarget)
{
	unsigned int totalPixelClockSteps = 0;
	if(vcounterCurrent != vcounterTarget)
	{
		bool hcounterAdvancedToIncrementPoint = false;
		if(hcounterCurrent != hscanSettings.vcounterIncrementPoint)
		{
			//Add the number of pixel clock steps required to advance the hcounter to the
			//point where the vcounter is incremented.
			totalPixelClockSteps += GetPixelClockStepsBetweenHCounterValues(hscanSettings, hcounterCurrent, hscanSettings.vcounterIncrementPoint);
			hcounterAdvancedToIncrementPoint = true;
		}

		//Convert the internal vcounter values into linear values
		vcounterCurrent = VCounterValueFromVDPInternalToLinear(vscanSettings, vcounterCurrent, oddFlagSet);
		vcounterTarget = VCounterValueFromVDPInternalToLinear(vscanSettings, vcounterTarget, oddFlagSet);

		//If the current horizontal and vertical counters haven't yet passed the point
		//where the odd flag is toggled, and the target vcounter value is passed that
		//point, update the value of the odd flag.
		if(((vcounterCurrent < vscanSettings.vblankSetPoint) || ((vcounterCurrent == vscanSettings.vblankSetPoint) && (hcounterCurrent < hscanSettings.oddFlagTogglePoint))) && (vcounterTarget > vscanSettings.vblankSetPoint))
		{
			oddFlagSet = interlaceIsEnabled && !oddFlagSet;
		}

		//Calculate the number of vcounter increment steps required to reach the target
		//vcounter value.
		unsigned int vcounterStepsPerIteration = oddFlagSet? vscanSettings.vcounterStepsPerIterationOddFlag: vscanSettings.vcounterStepsPerIteration;
		unsigned int totalVCounterStepsBetweenValues = ((vcounterTarget + vcounterStepsPerIteration) - vcounterCurrent) % vcounterStepsPerIteration;

		if(hcounterAdvancedToIncrementPoint)
		{
			//Subtract 1 from the total number of vcounter increment steps, since we know we
			//will already have advanced one vcounter step by advancing the hcounter to the
			//vcounter increment point, which we did above. Note that it is safe to subtract
			//here because we know the current and target vcounter values are different, since
			//we've already filtered for that above.
			totalVCounterStepsBetweenValues -= 1;
		}

		//Add the number of pixel clock steps required to advance the required number of
		//vcounter steps.
		totalPixelClockSteps += totalVCounterStepsBetweenValues * hscanSettings.hcounterStepsPerIteration;
	}
	return totalPixelClockSteps;
}

//----------------------------------------------------------------------------------------
//HV counter advancement functions
//----------------------------------------------------------------------------------------
unsigned int S315_5313::AddStepsToHCounter(const HScanSettings& hscanSettings, unsigned int hcounterCurrent, unsigned int hcounterStepsToAdd)
{
	//Convert the internal hcounter value into a linear value
	hcounterCurrent = HCounterValueFromVDPInternalToLinear(hscanSettings, hcounterCurrent);

	//Calculate the initial value for the target hcounter value
	unsigned int hcounterTarget = hcounterCurrent + hcounterStepsToAdd;

	//Wrap the hcounter value back around to the start if we've passed the total number of
	//steps per iteration.
	hcounterTarget %= hscanSettings.hcounterStepsPerIteration;

	//Convert the linear hcounter value back into an internal value
	hcounterTarget = HCounterValueFromLinearToVDPInternal(hscanSettings, hcounterTarget);

	//Return the incremented hcounter
	return hcounterTarget;
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::AddStepsToVCounter(const HScanSettings& hscanSettings, unsigned int hcounterCurrent, const VScanSettings& vscanSettings, bool interlaceIsEnabled, bool oddFlagSet, unsigned int vcounterCurrent, unsigned int vcounterStepsToAdd)
{
	//Convert the internal vcounter value into a linear value
	vcounterCurrent = VCounterValueFromVDPInternalToLinear(vscanSettings, vcounterCurrent, oddFlagSet);

	//Calculate the initial value for the target vcounter value
	unsigned int vcounterTarget = vcounterCurrent + vcounterStepsToAdd;

	//If the current horizontal and vertical counters haven't yet passed the point
	//where the odd flag is toggled, and the target vcounter value is passed that
	//point, update the value of the odd flag.
	if(((vcounterCurrent < vscanSettings.vblankSetPoint) || ((vcounterCurrent == vscanSettings.vblankSetPoint) && (hcounterCurrent < hscanSettings.oddFlagTogglePoint))) && (vcounterTarget > vscanSettings.vblankSetPoint))
	{
		oddFlagSet = interlaceIsEnabled && !oddFlagSet;
	}

	//Wrap the vcounter value back around to the start if we've passed the total number of
	//steps per iteration.
	unsigned int vcounterStepsPerIteration = oddFlagSet? vscanSettings.vcounterStepsPerIterationOddFlag: vscanSettings.vcounterStepsPerIteration;
	vcounterTarget %= vcounterStepsPerIteration;

	//Convert the linear vcounter value back into an internal value
	vcounterTarget = VCounterValueFromLinearToVDPInternal(vscanSettings, vcounterTarget, oddFlagSet);

	//Return the incremented vcounter
	return vcounterTarget;
}

//----------------------------------------------------------------------------------------
void S315_5313::AdvanceHVCounters(const HScanSettings& hscanSettings, unsigned int& hcounterCurrent, const VScanSettings& vscanSettings, bool interlaceIsEnabled, bool& oddFlagSet, unsigned int& vcounterCurrent, unsigned int pixelClockSteps)
{
	//Convert the internal hcounter and vcounter values into linear values
	hcounterCurrent = HCounterValueFromVDPInternalToLinear(hscanSettings, hcounterCurrent);
	vcounterCurrent = VCounterValueFromVDPInternalToLinear(vscanSettings, vcounterCurrent, oddFlagSet);

	//Calculate the number of times the vcounter needs to be incremented in order to reach
	//the odd flag toggle point.
	unsigned int vcounterStepsPerIteration = oddFlagSet? vscanSettings.vcounterStepsPerIterationOddFlag: vscanSettings.vcounterStepsPerIteration;
	unsigned int vcounterIncrementStepsUntilOddFlagToggle;
	if((vcounterCurrent < vscanSettings.vblankSetPoint) || ((vcounterCurrent == vscanSettings.vblankSetPoint) && (hcounterCurrent < hscanSettings.oddFlagTogglePoint)))
	{
		vcounterIncrementStepsUntilOddFlagToggle = vscanSettings.vblankSetPoint - vcounterCurrent;
	}
	else
	{
		vcounterIncrementStepsUntilOddFlagToggle = (vcounterStepsPerIteration - vcounterCurrent) + vscanSettings.vblankSetPoint;
	}

	//Calculate the number of times the hcounter needs to be incremented in order to reach
	//the odd flag toggle point.
	unsigned int hcounterIncrementStepsUntilOddFlagToggle;
	if(hcounterCurrent < hscanSettings.oddFlagTogglePoint)
	{
		hcounterIncrementStepsUntilOddFlagToggle = hscanSettings.oddFlagTogglePoint - hcounterCurrent;
	}
	else
	{
		hcounterIncrementStepsUntilOddFlagToggle = (hscanSettings.hcounterStepsPerIteration - hcounterCurrent) + hscanSettings.oddFlagTogglePoint;
	}

	//Calculate the number of pixel clock steps until the odd flag needs to be toggled
	unsigned int pixelClockStepsUntilOddFlagToggle = (vcounterIncrementStepsUntilOddFlagToggle * hscanSettings.hcounterStepsPerIteration) + hcounterIncrementStepsUntilOddFlagToggle;

	//While there's enough cycles remaining to reach the odd flag toggle point, advance to
	//that point, and update the odd flag.
	while(pixelClockStepsUntilOddFlagToggle < pixelClockSteps)
	{
		//Advance the hcounter and vcounter to the odd flag toggle point
		hcounterCurrent = (hcounterCurrent + hcounterIncrementStepsUntilOddFlagToggle) % hscanSettings.hcounterStepsPerIteration;
		vcounterCurrent = (vcounterCurrent + vcounterIncrementStepsUntilOddFlagToggle) % vcounterStepsPerIteration;

		//Update the odd flag, now that we've reached the toggle point.
		oddFlagSet = interlaceIsEnabled & !oddFlagSet;

		//Update the remaining pixel clock steps to advance
		pixelClockSteps -= pixelClockStepsUntilOddFlagToggle;

		//Recalculate the number of vcounter steps per iteration, to take into account the
		//updated odd flag setting.
		vcounterStepsPerIteration = oddFlagSet? vscanSettings.vcounterStepsPerIterationOddFlag: vscanSettings.vcounterStepsPerIteration;

		//Calculate the time to the next odd flag toggle point
		pixelClockStepsUntilOddFlagToggle = vcounterStepsPerIteration * hscanSettings.hcounterStepsPerIteration;
	}

	//Calculate the number of times the hcounter needs to be incremented in order to reach
	//the vcounter increment point.
	unsigned int hcounterIncrementStepsUntilVCounterIncrement;
	if(hcounterCurrent < hscanSettings.vcounterIncrementPoint)
	{
		hcounterIncrementStepsUntilVCounterIncrement = hscanSettings.vcounterIncrementPoint - hcounterCurrent;
	}
	else
	{
		hcounterIncrementStepsUntilVCounterIncrement = (hscanSettings.hcounterStepsPerIteration - hcounterCurrent) + hscanSettings.vcounterIncrementPoint;
	}

	//Calculate the number of times the hcounter and vcounter each need to be incremented
	//to reach their final positions.
	unsigned int hcounterIncrementSteps = pixelClockSteps;
	unsigned int vcounterIncrementSteps = 0;
	if(hcounterIncrementStepsUntilVCounterIncrement <= pixelClockSteps)
	{
		vcounterIncrementSteps = ((pixelClockSteps - hcounterIncrementStepsUntilVCounterIncrement) + hscanSettings.hcounterStepsPerIteration) / hscanSettings.hcounterStepsPerIteration;
	}

	//Advance hcounter and vcounter to their final positions
	unsigned int hcounterFinal = (hcounterCurrent + hcounterIncrementSteps) % hscanSettings.hcounterStepsPerIteration;
	unsigned int vcounterFinal = (vcounterCurrent + vcounterIncrementSteps) % vcounterStepsPerIteration;

	//Convert the final hcounter and vcounter values from linear values back to internal
	//values, and return them to the caller.
	hcounterCurrent = HCounterValueFromLinearToVDPInternal(hscanSettings, hcounterFinal);
	vcounterCurrent = VCounterValueFromLinearToVDPInternal(vscanSettings, vcounterFinal, oddFlagSet);
}

//----------------------------------------------------------------------------------------
//Pixel clock functions
//----------------------------------------------------------------------------------------
unsigned int S315_5313::GetPixelClockTicksUntilNextAccessSlot(unsigned int hcounterCurrent, bool screenModeH40Current, unsigned int vcounterCurrent, bool screenModeV30Current, bool palModeCurrent, bool interlaceEnabledCurrent)
{
	//Obtain the current hscan and vscan settings
	const HScanSettings& hscanSettings = GetHScanSettings(screenModeH40Current);
	const VScanSettings& vscanSettings = GetVScanSettings(screenModeV30Current, palModeCurrent, interlaceEnabledCurrent);

	//Check if the current line, and the line following, are within the active display
	//region. Active display lines only allow external VRAM access at specific access
	//slots, while non-active display lines allow free access to VRAM, except where memory
	//refresh cycles occur.
	//##TODO## We need to check all settings which blank or disable the display here, as
	//they affect access slot timing.
	//##TODO## Confirm that the active display region is defined by vblank, which is
	//actually first asserted at vcounter=0x1FF, meaning there is one additional line at
	//the start of the display. This extra line is probably where sprite mapping and
	//pattern data is latched for the first line of the display. Test this on hardware.
	bool currentLineActiveDisplayRegion = (vcounterCurrent == 0x1FF) || (vcounterCurrent < vscanSettings.vblankSetPoint);
	bool nextLineActiveDisplayRegion = (vcounterCurrent == 0x1FF) || (vcounterCurrent+1 == 0x1FF) || (vcounterCurrent+1 < vscanSettings.vblankSetPoint);

	//Determine when the next access slot will occur, based on the hcounter position and
	//the current screen mode settings. Note that we can safely assume the horizontal
	//screen mode doesn't change when making these calculations.
	//##TODO## Consider using table lookups here to eliminate branching.
	unsigned int nextAccessSlotHCounter = 0;
	if(!currentLineActiveDisplayRegion)
	{
		if(!screenModeH40Current)
		{
			//Refresh cycle slots (H32):
			//---------------
			//|Slot|HCounter|
			//|-------------|
			//|  1 | 0x035  |
			//|  2 | 0x075  |
			//|  3 | 0x0B5  |
			//|  4 | 0x0F5  |
			//---------------
			if(hcounterCurrent == 0x34)
			{
				nextAccessSlotHCounter = 0x36;
			}
			else if(hcounterCurrent == 0x74)
			{
				nextAccessSlotHCounter = 0x76;
			}
			else if(hcounterCurrent == 0xB4)
			{
				nextAccessSlotHCounter = 0xB6;
			}
			else if(hcounterCurrent == 0xF4)
			{
				nextAccessSlotHCounter = 0xF6;
			}
			else if(nextLineActiveDisplayRegion && ((hcounterCurrent + 1) == hscanSettings.vcounterIncrementPoint))
			{
				//If we're at the end of a non-display line, and the vcounter is about to
				//be incremented on the next pixel clock cycle and bring us into an active
				//display line, set the next access slot based on the first allowed access
				//slot after vcounter increment in an active display line for this screen
				//mode.
				nextAccessSlotHCounter = 0x121;
			}
			else
			{
				//If none of the above cases apply, our next access slot is on the next
				//hcounter value.
				nextAccessSlotHCounter = AddStepsToHCounter(hscanSettings, hcounterCurrent, 1);
			}
		}
		else
		{
			//Refresh cycle slots (H40):
			//---------------
			//|Slot|HCounter|
			//|-------------|
			//|  1 | 0x032  |
			//|  2 | 0x072  |
			//|  3 | 0x0B2  |
			//|  4 | 0x0F2  |
			//|  5 | 0x132  |
			//---------------
			if(hcounterCurrent == 0x31)
			{
				nextAccessSlotHCounter = 0x33;
			}
			else if(hcounterCurrent == 0x71)
			{
				nextAccessSlotHCounter = 0x73;
			}
			else if(hcounterCurrent == 0xB1)
			{
				nextAccessSlotHCounter = 0xB3;
			}
			else if(hcounterCurrent == 0xF1)
			{
				nextAccessSlotHCounter = 0xF3;
			}
			else if(hcounterCurrent == 0x131)
			{
				nextAccessSlotHCounter = 0x133;
			}
			else if(nextLineActiveDisplayRegion && ((hcounterCurrent + 1) == hscanSettings.vcounterIncrementPoint))
			{
				//If we're at the end of a non-display line, and the vcounter is about to
				//be incremented on the next pixel clock cycle and bring us into an active
				//display line, set the next access slot based on the first allowed access
				//slot after vcounter increment in an active display line for this screen
				//mode.
				nextAccessSlotHCounter = 0x1CE;
			}
			else
			{
				//If none of the above cases apply, our next access slot is on the next
				//hcounter value.
				nextAccessSlotHCounter = AddStepsToHCounter(hscanSettings, hcounterCurrent, 1);
			}
		}
	}
	else
	{
		if(!screenModeH40Current)
		{
			//##TODO## Confirm the point at which vcounter is incremented
			//External access slots (H32):
			//---------------
			//|Slot|HCounter|
			//|-------------|
			//|  1 | 0x005  |
			//|  2 | 0x015  |
			//|  3 | 0x025  |
			//|  4 | 0x045  |
			//|  5 | 0x055  |
			//|  6 | 0x065  |
			//|  7 | 0x085  |
			//|  8 | 0x095  |
			//|  9 | 0x0A5  |
			//| 10 | 0x0C5  |
			//| 11 | 0x0D5  |
			//| 12 | 0x0E5  |
			//| 13 | 0x103  |
			//| 14 | 0x105  | //Vcounter increments at either 0x10A or 0x10B
			//| 15 | 0x121  | //Next line
			//| 16 | 0x1E7  | //Next line
			//---------------
			if(hcounterCurrent < 0x005)
			{
				nextAccessSlotHCounter = 0x005;
			}
			else if(hcounterCurrent < 0x015)
			{
				nextAccessSlotHCounter = 0x015;
			}
			else if(hcounterCurrent < 0x025)
			{
				nextAccessSlotHCounter = 0x025;
			}
			else if(hcounterCurrent < 0x045)
			{
				nextAccessSlotHCounter = 0x045;
			}
			else if(hcounterCurrent < 0x055)
			{
				nextAccessSlotHCounter = 0x055;
			}
			else if(hcounterCurrent < 0x065)
			{
				nextAccessSlotHCounter = 0x065;
			}
			else if(hcounterCurrent < 0x085)
			{
				nextAccessSlotHCounter = 0x085;
			}
			else if(hcounterCurrent < 0x095)
			{
				nextAccessSlotHCounter = 0x095;
			}
			else if(hcounterCurrent < 0x0A5)
			{
				nextAccessSlotHCounter = 0x0A5;
			}
			else if(hcounterCurrent < 0x0C5)
			{
				nextAccessSlotHCounter = 0x0C5;
			}
			else if(hcounterCurrent < 0x0D5)
			{
				nextAccessSlotHCounter = 0x0D5;
			}
			else if(hcounterCurrent < 0x0E5)
			{
				nextAccessSlotHCounter = 0x0E5;
			}
			else if(hcounterCurrent < 0x103)
			{
				nextAccessSlotHCounter = 0x103;
			}
			else if(hcounterCurrent < 0x105)
			{
				nextAccessSlotHCounter = 0x105;
			}
			else if(!nextLineActiveDisplayRegion && (hcounterCurrent < hscanSettings.vcounterIncrementPoint))
			{
				//If we're going to pass from an active line to a non-active line on the
				//next vcounter increment, and there are no remaining access slots on the
				//current line before the vcounter increment, we set the next access slot
				//to occur on the vcounter increment point.
				nextAccessSlotHCounter = hscanSettings.vcounterIncrementPoint;
			}
			else if(hcounterCurrent < 0x121)
			{
				nextAccessSlotHCounter = 0x121;
			}
			else if(hcounterCurrent < 0x1E7)
			{
				nextAccessSlotHCounter = 0x1E7;
			}
			else
			{
				nextAccessSlotHCounter = 0x005;
			}
		}
		else
		{
			//##TODO## Confirm the point at which vcounter is incremented
			//External access slots (H40):
			//---------------
			//|Slot|HCounter|
			//|-------------|
			//|  1 | 0x002  |
			//|  2 | 0x012  |
			//|  3 | 0x022  |
			//|  4 | 0x042  |
			//|  5 | 0x052  |
			//|  6 | 0x062  |
			//|  7 | 0x082  |
			//|  8 | 0x092  |
			//|  9 | 0x0A2  |
			//| 10 | 0x0C2  |
			//| 11 | 0x0D2  |
			//| 12 | 0x0E2  |
			//| 13 | 0x102  |
			//| 14 | 0x112  |
			//| 15 | 0x122  |
			//| 16 | 0x140  |
			//| 17 | 0x142  | //Vcounter increments at either 0x14A or 0x14B
			//| 18 | 0x1CE  | //Next line
			//---------------
			if(hcounterCurrent < 0x002)
			{
				nextAccessSlotHCounter = 0x002;
			}
			else if(hcounterCurrent < 0x012)
			{
				nextAccessSlotHCounter = 0x012;
			}
			else if(hcounterCurrent < 0x022)
			{
				nextAccessSlotHCounter = 0x022;
			}
			else if(hcounterCurrent < 0x042)
			{
				nextAccessSlotHCounter = 0x042;
			}
			else if(hcounterCurrent < 0x052)
			{
				nextAccessSlotHCounter = 0x052;
			}
			else if(hcounterCurrent < 0x062)
			{
				nextAccessSlotHCounter = 0x062;
			}
			else if(hcounterCurrent < 0x082)
			{
				nextAccessSlotHCounter = 0x082;
			}
			else if(hcounterCurrent < 0x092)
			{
				nextAccessSlotHCounter = 0x092;
			}
			else if(hcounterCurrent < 0x0A2)
			{
				nextAccessSlotHCounter = 0x0A2;
			}
			else if(hcounterCurrent < 0x0C2)
			{
				nextAccessSlotHCounter = 0x0C2;
			}
			else if(hcounterCurrent < 0x0D2)
			{
				nextAccessSlotHCounter = 0x0D2;
			}
			else if(hcounterCurrent < 0x0E2)
			{
				nextAccessSlotHCounter = 0x0E2;
			}
			else if(hcounterCurrent < 0x102)
			{
				nextAccessSlotHCounter = 0x102;
			}
			else if(hcounterCurrent < 0x112)
			{
				nextAccessSlotHCounter = 0x112;
			}
			else if(hcounterCurrent < 0x122)
			{
				nextAccessSlotHCounter = 0x122;
			}
			else if(hcounterCurrent < 0x140)
			{
				nextAccessSlotHCounter = 0x140;
			}
			else if(hcounterCurrent < 0x142)
			{
				nextAccessSlotHCounter = 0x142;
			}
			else if(!nextLineActiveDisplayRegion && (hcounterCurrent < hscanSettings.vcounterIncrementPoint))
			{
				//If we're going to pass from an active line to a non-active line on the
				//next vcounter increment, and there are no remaining access slots on the
				//current line before the vcounter increment, we set the next access slot
				//to occur on the vcounter increment point.
				nextAccessSlotHCounter = hscanSettings.vcounterIncrementPoint;
			}
			else if(hcounterCurrent < 0x1CE)
			{
				nextAccessSlotHCounter = 0x1CE;
			}
			else
			{
				nextAccessSlotHCounter = 0x002;
			}
		}
	}

	//Calculate the number of pixel clock steps which are required to reach the next
	//target hcounter value
	unsigned int pixelClockTicksToAccessSlot = GetPixelClockStepsBetweenHCounterValues(hscanSettings, hcounterCurrent, nextAccessSlotHCounter);

	//Return the number of pixel clock steps until the next available access slot
	return pixelClockTicksToAccessSlot;
}

//----------------------------------------------------------------------------------------
//##TODO## Modify this function to support irregular horizontal modes. In particular, we
//need to support the case where H40 mode is enabled, without setting the corresponding
//bit to enable the external dot clock. In this case, the VDP uses an internally generated
//clock signal of MCLK/4 for the entire line, resulting in 3360 MCLK ticks per line
//instead of 3420, which produces a sync rate too fast for a normal TV. We want our VDP
//core to support output of this invalid display mode. Since all VRAM and register writes
//are stored relative to MCLK, it should be possible.
unsigned int S315_5313::GetPixelClockTicksForMclkTicks(unsigned int mclkTicks, unsigned int hcounterCurrent, bool screenModeH40Active, unsigned int& mclkTicksUnused)
{
	//Calculate the total number of pixel clock ticks which will execute after the given
	//number of mclk ticks, based on the current screen mode.
	unsigned int pixelClockTicks;
	if(!screenModeH40Active)
	{
		//In H32 mode, the serial clock is calculated as MCLK/5. The relation of hcounter
		//to the various clock signals, and the alignment of hcounter with the analog
		//video output, is shown below. This information has been determined through
		//extensive and precise hardware tests.
		//-----------------------------------------------------------------
		//| Screen section | HCounter  |Pixel| Pixel |Serial|Serial |MCLK |
		//| (PAL/NTSC H32) |  value    |clock| clock |clock |clock  |ticks|
		//|                |           |ticks|divider|ticks |divider|     |
		//|----------------|-----------|-----|-------|------|-------|-----|
		//|Left border     |0x00B-0x017|  13 |SCLK/2 |   26 |MCLK/5 | 130 |
		//|----------------|-----------|-----|-------|------|-------|-----|
		//|Active display  |0x018-0x117| 256 |SCLK/2 |  512 |MCLK/5 |2560 |
		//|----------------|-----------|-----|-------|------|-------|-----|
		//|Right border    |0x118-0x125|  14 |SCLK/2 |   28 |MCLK/5 | 140 |
		//|----------------|-----------|-----|-------|------|-------|-----|
		//|Back porch      |0x126-0x127|   9 |SCLK/2 |   18 |MCLK/5 |  90 |
		//|(Right Blanking)|0x1D2-0x1D8|     |       |      |       |     |
		//|----------------|-----------|-----|-------|------|-------|-----|
		//|Horizontal sync |0x1D9-0x1F2|  26 |SCLK/2 |   52 |MCLK/5 | 260 |
		//|----------------|-----------|-----|-------|------|-------|-----|
		//|Front porch     |0x1F3-0x00A|  24 |SCLK/2 |   48 |MCLK/5 | 240 |
		//|(Left Blanking) |           |     |       |      |       |     |
		//|----------------|-----------|-----|-------|------|-------|-----|
		//|TOTALS          |           | 342 |       |  684 |       |3420 |
		//-----------------------------------------------------------------
		pixelClockTicks = mclkTicks / 10;
		mclkTicksUnused = mclkTicks % 10;
	}
	else
	{
		//In H40 mode, the serial clock is provided through the external EDCLK input line.
		//The relation of hcounter to the various clock signals, and the alignment of
		//hcounter with the analog video output, is shown below. This information has been
		//determined through extensive and precise hardware tests.
		//##NOTE## Our own testing as well as tests done by Jorge Nuno confirm that in H40
		//mode, the left border starts being displayed 64SC cycles after the end of HSYNC.
		//We know from VRAM snooping that HSYNC ends when the HCounter advances to 0x1ED
		//in H40 mode, and there are 2 SC cycles to every pixel clock cycle, which is what
		//advances the HCounter and outputs a pixel to the screen, so when the left border
		//is about to start being drawn to the screen, our internal 9-bit HCounter has a
		//value of 0x00D, so at 0x00E, one border pixel will have been drawn.
		//-In H40 mode, the pixel clock is calculated as EDCLK/2. EDCLK is the external
		//dot clock, and on the real VDP, this is an input line to the VDP, with EDCLK
		//generated by an external circuit. We emulate EDCLK internally here, assuming
		//that any other system will use the same EDCLK generation pattern. This should be
		//generally true, since the altered EDCLK signal must maintain 3420 MCLK cycles
		//per line, with an input of MCLK/4 during active scan, in order to generate a
		//valid video signal which is compatible with PAL/NTSC standards. If the VDP is
		//used in a system which has an unusual video sync rate, EDCLK could be different
		//however.
		//-EDCLK is a complex signal, it is not a regular clock pulse. When the external
		//HSYNC line has not been asserted, EDCLK is simply MCLK/4. As soon as HSYNC is
		//asserted however, EDCLK starts outputting an alternating pattern, which lasts
		//for 66 EDCLK ticks after hcounter has been asserted. In this pattern, EDCLK
		//switches to MCLK/5 for 15 ticks, then switches back to, MCLK/4 for 2 ticks, then
		//back to MCLK/5 for 15 ticks, and so on. This continues until 4 complete periods
		//of MCLK/5 being output for 15 ticks have been completed, with 2 blocks of MCLK/4
		//separating each one.
		//--------------------------------------------------------------------
		//| Screen section |   HCounter    |Pixel| Pixel |EDCLK| EDCLK |MCLK |
		//| (PAL/NTSC H40) |    value      |clock| clock |ticks|divider|ticks|
		//|                |               |ticks|divider|     |       |     |
		//|----------------|---------------|-----|-------|-----|-------|-----|
		//|Left border     |0x00D-0x019    |  13 |EDCLK/2|  26 |MCLK/4 | 104 |
		//|----------------|---------------|-----|-------|-----|-------|-----|
		//|Active display  |0x01A-0x159    | 320 |EDCLK/2| 640 |MCLK/4 |2560 |
		//|----------------|---------------|-----|-------|-----|-------|-----|
		//|Right border    |0x15A-0x167    |  14 |EDCLK/2|  28 |MCLK/4 | 112 |
		//|----------------|---------------|-----|-------|-----|-------|-----|
		//|Back porch      |0x168-0x16C    |   9 |EDCLK/2|  18 |MCLK/4 |  72 |
		//|(Right Blanking)|0x1C9-0x1CC    |     |       |     |       |     |
		//|----------------|---------------|-----|-------|-----|-------|-----|
		//|Horizontal sync |0x1CD.0-0x1D4.5| 7.5 |EDCLK/2|  15 |MCLK/5 |  75 |
		//|                |0x1D4.5-0x1D5.5|   1 |EDCLK/2|   2 |MCLK/4 |   8 |
		//|                |0x1D5.5-0x1DC.0| 7.5 |EDCLK/2|  15 |MCLK/5 |  75 |
		//|                |0x1DD.0        |   1 |EDCLK/2|   2 |MCLK/4 |   8 |
		//|                |0x1DE.0-0x1E5.5| 7.5 |EDCLK/2|  15 |MCLK/5 |  75 |
		//|                |0x1E5.5-0x1E6.5|   1 |EDCLK/2|   2 |MCLK/4 |   8 |
		//|                |0x1E6.5-0x1EC.0| 6.5 |EDCLK/2|  13 |MCLK/5 |  65 |
		//|                |===============|=====|=======|=====|=======|=====|
		//|        Subtotal|0x1CD-0x1EC    | (32)|       | (64)|       |(314)|
		//|----------------|---------------|-----|-------|-----|-------|-----|
		//|Front porch     |0x1ED          |   1 |EDCLK/2|   2 |MCLK/5 |  10 |
		//|(Left Blanking) |0x1EE-0x00C    |  31 |EDCLK/2|  62 |MCLK/4 | 248 |
		//|                |===============|=====|=======|=====|=======|=====|
		//|        Subtotal|0x1ED-0x00C    | (32)|       | (64)|       |(258)|
		//|----------------|---------------|-----|-------|-----|-------|-----|
		//|TOTALS          |               | 420 |       | 840 |       |3420 |
		//--------------------------------------------------------------------

		//Get the current hscan settings
		const HScanSettings* hscanSettings = &GetHScanSettings(screenModeH40Active);

		//Calculate the number of complete hcounter lines which were advanced over the
		//time period, and the number of pixel clock ticks used to advance that number of
		//lines.
		const unsigned int mclkTicksPerLine = 3420;
		unsigned int completeLinesAdvanced = mclkTicks / mclkTicksPerLine;
		unsigned int mclkTicksUsed = (completeLinesAdvanced * mclkTicksPerLine);
		unsigned int mclkTicksRemaining = mclkTicks - mclkTicksUsed;
		pixelClockTicks = completeLinesAdvanced * hscanSettings->hcounterStepsPerIteration;

		//Using the above information, the table below shows the number of MCLK cycles
		//that hcounter values around the affected blanking area remain set for.
		//--------------------------
		//|Hcounter|Hcounter| MCLK |
		//| value  | linear |cycles|
		//|internal| value  |      |
		//|--------|--------|------|
		//|...     |...     |8     |
		//|0x1CB   |0x16F   |8     |
		//|0x1CC   |0x170   |8     |
		//|0x1CD   |0x171   |10    |
		//|0x1CE   |0x172   |10    |
		//|0x1CF   |0x173   |10    |
		//|0x1D0   |0x174   |10    |
		//|0x1D1   |0x175   |10    |
		//|0x1D2   |0x176   |10    |
		//|0x1D3   |0x177   |10    |
		//|0x1D4   |0x178   |9     |
		//|0x1D5   |0x179   |9     |
		//|0x1D6   |0x17A   |10    |
		//|0x1D7   |0x17B   |10    |
		//|0x1D8   |0x17C   |10    |
		//|0x1D9   |0x17D   |10    |
		//|0x1DA   |0x17E   |10    |
		//|0x1DB   |0x17F   |10    |
		//|0x1DC   |0x180   |10    |
		//|0x1DD   |0x181   |8     |
		//|0x1DE   |0x182   |10    |
		//|0x1DF   |0x183   |10    |
		//|0x1E0   |0x184   |10    |
		//|0x1E1   |0x185   |10    |
		//|0x1E2   |0x186   |10    |
		//|0x1E3   |0x187   |10    |
		//|0x1E4   |0x188   |10    |
		//|0x1E5   |0x189   |9     |
		//|0x1E6   |0x18A   |9     |
		//|0x1E7   |0x18B   |10    |
		//|0x1E8   |0x18C   |10    |
		//|0x1E9   |0x18D   |10    |
		//|0x1EA   |0x18E   |10    |
		//|0x1EB   |0x18F   |10    |
		//|0x1EC   |0x190   |10    |
		//|0x1ED   |0x191   |10    |
		//|0x1EE   |0x192   |8     |
		//|0x1EF   |0x193   |8     |
		//|...     |...     |8     |
		//--------------------------
		//We use this information below to accurately calculate the number of mclk cycles
		//used to advance the hcounter.
		while(mclkTicksRemaining > 0)
		{
			//Determine the current clock rate, and the number of steps we can advance the
			//pixel clock until the clock rate changes.
			unsigned int nextHCounterTargetStep;
			unsigned int currentMclksPerStep;
			if(hcounterCurrent < 0x1CD)
			{
				nextHCounterTargetStep = 0x1CD;
				currentMclksPerStep = 8;
			}
			else if(hcounterCurrent < 0x1D4)
			{
				nextHCounterTargetStep = 0x1D4;
				currentMclksPerStep = 10;
			}
			else if(hcounterCurrent < 0x1D6)
			{
				nextHCounterTargetStep = 0x1D6;
				currentMclksPerStep = 9;
			}
			else if(hcounterCurrent < 0x1DD)
			{
				nextHCounterTargetStep = 0x1DD;
				currentMclksPerStep = 10;
			}
			else if(hcounterCurrent < 0x1DE)
			{
				nextHCounterTargetStep = 0x1DE;
				currentMclksPerStep = 8;
			}
			else if(hcounterCurrent < 0x1E5)
			{
				nextHCounterTargetStep = 0x1E5;
				currentMclksPerStep = 10;
			}
			else if(hcounterCurrent < 0x1E7)
			{
				nextHCounterTargetStep = 0x1E7;
				currentMclksPerStep = 9;
			}
			else if(hcounterCurrent < 0x1EE)
			{
				nextHCounterTargetStep = 0x1EE;
				currentMclksPerStep = 10;
			}
			else
			{
				nextHCounterTargetStep = 0x1CD;
				currentMclksPerStep = 8;
			}

			//Calculate the number of pixel clock steps, and mclk ticks, which are
			//required to reach the next target hcounter value.
			unsigned int hcounterStepsToTarget = GetPixelClockStepsBetweenHCounterValues(*hscanSettings, hcounterCurrent, nextHCounterTargetStep);
			unsigned int mclkTicksToTarget = hcounterStepsToTarget * currentMclksPerStep;

			//Calculate the number of mclk ticks we can actually advance in the time we
			//have.
			unsigned int mclkTicksToAdvance = (mclkTicksRemaining < mclkTicksToTarget)? mclkTicksRemaining: mclkTicksToTarget;

			//Calculate the number of pixel clock ticks we can advance
			unsigned int pixelClockTicksToAdvance = (mclkTicksToAdvance / currentMclksPerStep);
			pixelClockTicks += pixelClockTicksToAdvance;

			//Calculate the number of useful mclk cycles in the step we're advancing. If
			//some mclk ticks are left over, we still allow them to be deducted from
			//mclkTicksRemaining in order to indicate we have consumed all available mclk
			//ticks. The correct number of mclk ticks which were actually used to advance
			//the pixel clock is reflected in mclkTicksUsed.
			mclkTicksUsed += pixelClockTicksToAdvance * currentMclksPerStep;

			//Update hcounter to the position we've just advanced to
			hcounterCurrent = nextHCounterTargetStep;

			//Update the remaining count of mclk cycles
			mclkTicksRemaining -= mclkTicksToAdvance;
		}

		//Calculate the amount of time remaining
		mclkTicksUnused = mclkTicks - mclkTicksUsed;
	}

	return pixelClockTicks;
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::GetMclkTicksForPixelClockTicks(unsigned int pixelClockTicks, unsigned int hcounterCurrent, bool screenModeH40Active)
{
	//Calculate the total number of mclk ticks which will be consumed in order to advance
	//the given number of pixel clock ticks, based on the given position in the current
	//scanline, and the current screen mode settings.
	unsigned int mclkTicks;
	if(!screenModeH40Active)
	{
		//In H32 mode, the serial clock is calculated as MCLK/5. The relation of hcounter
		//to the various clock signals, and the alignment of hcounter with the analog
		//video output, is shown below. This information has been determined through
		//extensive and precise hardware tests.
		//-----------------------------------------------------------------
		//| Screen section | HCounter  |Pixel| Pixel |Serial|Serial |MCLK |
		//| (PAL/NTSC H32) |  value    |clock| clock |clock |clock  |ticks|
		//|                |           |ticks|divider|ticks |divider|     |
		//|----------------|-----------|-----|-------|------|-------|-----|
		//|Left border     |0x00B-0x017|  13 |SCLK/2 |   26 |MCLK/5 | 130 |
		//|----------------|-----------|-----|-------|------|-------|-----|
		//|Active display  |0x018-0x117| 256 |SCLK/2 |  512 |MCLK/5 |2560 |
		//|----------------|-----------|-----|-------|------|-------|-----|
		//|Right border    |0x118-0x125|  14 |SCLK/2 |   28 |MCLK/5 | 140 |
		//|----------------|-----------|-----|-------|------|-------|-----|
		//|Back porch      |0x126-0x127|   9 |SCLK/2 |   18 |MCLK/5 |  90 |
		//|(Right Blanking)|0x1D2-0x1D8|     |       |      |       |     |
		//|----------------|-----------|-----|-------|------|-------|-----|
		//|Horizontal sync |0x1D9-0x1F2|  26 |SCLK/2 |   52 |MCLK/5 | 260 |
		//|----------------|-----------|-----|-------|------|-------|-----|
		//|Front porch     |0x1F3-0x00A|  24 |SCLK/2 |   48 |MCLK/5 | 240 |
		//|(Left Blanking) |           |     |       |      |       |     |
		//|----------------|-----------|-----|-------|------|-------|-----|
		//|TOTALS          |           | 342 |       |  684 |       |3420 |
		//-----------------------------------------------------------------
		mclkTicks = pixelClockTicks * 10;
	}
	else
	{
		//In H40 mode, the serial clock is provided through the external EDCLK input line.
		//The relation of hcounter to the various clock signals, and the alignment of
		//hcounter with the analog video output, is shown below. This information has been
		//determined through extensive and precise hardware tests.
		//##NOTE## Our own testing as well as tests done by Jorge Nuno confirm that in H40
		//mode, the left border starts being displayed 64SC cycles after the end of HSYNC.
		//We know from VRAM snooping that HSYNC ends when the HCounter advances to 0x1ED
		//in H40 mode, and there are 2 SC cycles to every pixel clock cycle, which is what
		//advances the HCounter and outputs a pixel to the screen, so when the left border
		//is about to start being drawn to the screen, our internal 9-bit HCounter has a
		//value of 0x00D, so at 0x00E, one border pixel will have been drawn.
		//-In H40 mode, the pixel clock is calculated as EDCLK/2. EDCLK is the external
		//dot clock, and on the real VDP, this is an input line to the VDP, with EDCLK
		//generated by an external circuit. We emulate EDCLK internally here, assuming
		//that any other system will use the same EDCLK generation pattern. This should be
		//generally true, since the altered EDCLK signal must maintain 3420 MCLK cycles
		//per line, with an input of MCLK/4 during active scan, in order to generate a
		//valid video signal which is compatible with PAL/NTSC standards. If the VDP is
		//used in a system which has an unusual video sync rate, EDCLK could be different
		//however.
		//-EDCLK is a complex signal, it is not a regular clock pulse. When the external
		//HSYNC line has not been asserted, EDCLK is simply MCLK/4. As soon as HSYNC is
		//asserted however, EDCLK starts outputting an alternating pattern, which lasts
		//for 66 EDCLK ticks after hcounter has been asserted. In this pattern, EDCLK
		//switches to MCLK/5 for 15 ticks, then switches back to, MCLK/4 for 2 ticks, then
		//back to MCLK/5 for 15 ticks, and so on. This continues until 4 complete periods
		//of MCLK/5 being output for 15 ticks have been completed, with 2 blocks of MCLK/4
		//separating each one.
		//--------------------------------------------------------------------
		//| Screen section |   HCounter    |Pixel| Pixel |EDCLK| EDCLK |MCLK |
		//| (PAL/NTSC H40) |    value      |clock| clock |ticks|divider|ticks|
		//|                |               |ticks|divider|     |       |     |
		//|----------------|---------------|-----|-------|-----|-------|-----|
		//|Left border     |0x00D-0x019    |  13 |EDCLK/2|  26 |MCLK/4 | 104 |
		//|----------------|---------------|-----|-------|-----|-------|-----|
		//|Active display  |0x01A-0x159    | 320 |EDCLK/2| 640 |MCLK/4 |2560 |
		//|----------------|---------------|-----|-------|-----|-------|-----|
		//|Right border    |0x15A-0x167    |  14 |EDCLK/2|  28 |MCLK/4 | 112 |
		//|----------------|---------------|-----|-------|-----|-------|-----|
		//|Back porch      |0x168-0x16C    |   9 |EDCLK/2|  18 |MCLK/4 |  72 |
		//|(Right Blanking)|0x1C9-0x1CC    |     |       |     |       |     |
		//|----------------|---------------|-----|-------|-----|-------|-----|
		//|Horizontal sync |0x1CD.0-0x1D4.5| 7.5 |EDCLK/2|  15 |MCLK/5 |  75 |
		//|                |0x1D4.5-0x1D5.5|   1 |EDCLK/2|   2 |MCLK/4 |   8 |
		//|                |0x1D5.5-0x1DC.0| 7.5 |EDCLK/2|  15 |MCLK/5 |  75 |
		//|                |0x1DD.0        |   1 |EDCLK/2|   2 |MCLK/4 |   8 |
		//|                |0x1DE.0-0x1E5.5| 7.5 |EDCLK/2|  15 |MCLK/5 |  75 |
		//|                |0x1E5.5-0x1E6.5|   1 |EDCLK/2|   2 |MCLK/4 |   8 |
		//|                |0x1E6.5-0x1EC.0| 6.5 |EDCLK/2|  13 |MCLK/5 |  65 |
		//|                |===============|=====|=======|=====|=======|=====|
		//|        Subtotal|0x1CD-0x1EC    | (32)|       | (64)|       |(314)|
		//|----------------|---------------|-----|-------|-----|-------|-----|
		//|Front porch     |0x1ED          |   1 |EDCLK/2|   2 |MCLK/5 |  10 |
		//|(Left Blanking) |0x1EE-0x00C    |  31 |EDCLK/2|  62 |MCLK/4 | 248 |
		//|                |===============|=====|=======|=====|=======|=====|
		//|        Subtotal|0x1ED-0x00C    | (32)|       | (64)|       |(258)|
		//|----------------|---------------|-----|-------|-----|-------|-----|
		//|TOTALS          |               | 420 |       | 840 |       |3420 |
		//--------------------------------------------------------------------

		//Get the current hscan settings
		const HScanSettings* hscanSettings = &GetHScanSettings(screenModeH40Active);

		//Calculate the number of complete hcounter lines which were advanced over the
		//time period, and the number of mclk ticks used to advance that number of lines.
		const unsigned int mclkTicksPerLine = 3420;
		unsigned int completeLinesAdvanced = pixelClockTicks / hscanSettings->hcounterStepsPerIteration;
		unsigned int pixelClockTicksUsed = (completeLinesAdvanced * hscanSettings->hcounterStepsPerIteration);
		unsigned int pixelClockTicksRemaining = pixelClockTicks - pixelClockTicksUsed;
		mclkTicks = completeLinesAdvanced * mclkTicksPerLine;

		//Using the above information, the table below shows the number of MCLK cycles
		//that hcounter values around the affected blanking area remain set for.
		//--------------------------
		//|Hcounter|Hcounter| MCLK |
		//| value  | linear |cycles|
		//|internal| value  |      |
		//|--------|--------|------|
		//|...     |...     |8     |
		//|0x1CB   |0x16F   |8     |
		//|0x1CC   |0x170   |8     |
		//|0x1CD   |0x171   |10    |
		//|0x1CE   |0x172   |10    |
		//|0x1CF   |0x173   |10    |
		//|0x1D0   |0x174   |10    |
		//|0x1D1   |0x175   |10    |
		//|0x1D2   |0x176   |10    |
		//|0x1D3   |0x177   |10    |
		//|0x1D4   |0x178   |9     |
		//|0x1D5   |0x179   |9     |
		//|0x1D6   |0x17A   |10    |
		//|0x1D7   |0x17B   |10    |
		//|0x1D8   |0x17C   |10    |
		//|0x1D9   |0x17D   |10    |
		//|0x1DA   |0x17E   |10    |
		//|0x1DB   |0x17F   |10    |
		//|0x1DC   |0x180   |10    |
		//|0x1DD   |0x181   |8     |
		//|0x1DE   |0x182   |10    |
		//|0x1DF   |0x183   |10    |
		//|0x1E0   |0x184   |10    |
		//|0x1E1   |0x185   |10    |
		//|0x1E2   |0x186   |10    |
		//|0x1E3   |0x187   |10    |
		//|0x1E4   |0x188   |10    |
		//|0x1E5   |0x189   |9     |
		//|0x1E6   |0x18A   |9     |
		//|0x1E7   |0x18B   |10    |
		//|0x1E8   |0x18C   |10    |
		//|0x1E9   |0x18D   |10    |
		//|0x1EA   |0x18E   |10    |
		//|0x1EB   |0x18F   |10    |
		//|0x1EC   |0x190   |10    |
		//|0x1ED   |0x191   |10    |
		//|0x1EE   |0x192   |8     |
		//|0x1EF   |0x193   |8     |
		//|...     |...     |8     |
		//--------------------------
		//We use this information below to accurately calculate the number of mclk cycles
		//used to advance the hcounter.
		while(pixelClockTicksRemaining > 0)
		{
			//Determine the current clock rate, and the number of steps we can advance the
			//pixel clock until the clock rate changes.
			unsigned int nextHCounterTargetStep;
			unsigned int currentMclksPerStep;
			if(hcounterCurrent < 0x1CD)
			{
				nextHCounterTargetStep = 0x1CD;
				currentMclksPerStep = 8;
			}
			else if(hcounterCurrent < 0x1D4)
			{
				nextHCounterTargetStep = 0x1D4;
				currentMclksPerStep = 10;
			}
			else if(hcounterCurrent < 0x1D6)
			{
				nextHCounterTargetStep = 0x1D6;
				currentMclksPerStep = 9;
			}
			else if(hcounterCurrent < 0x1DD)
			{
				nextHCounterTargetStep = 0x1DD;
				currentMclksPerStep = 10;
			}
			else if(hcounterCurrent < 0x1DE)
			{
				nextHCounterTargetStep = 0x1DE;
				currentMclksPerStep = 8;
			}
			else if(hcounterCurrent < 0x1E5)
			{
				nextHCounterTargetStep = 0x1E5;
				currentMclksPerStep = 10;
			}
			else if(hcounterCurrent < 0x1E7)
			{
				nextHCounterTargetStep = 0x1E7;
				currentMclksPerStep = 9;
			}
			else if(hcounterCurrent < 0x1EE)
			{
				nextHCounterTargetStep = 0x1EE;
				currentMclksPerStep = 10;
			}
			else
			{
				nextHCounterTargetStep = 0x1CD;
				currentMclksPerStep = 8;
			}

			//Calculate the number of pixel clock steps which are required to reach the
			//next target hcounter value.
			unsigned int hcounterStepsToTarget = GetPixelClockStepsBetweenHCounterValues(*hscanSettings, hcounterCurrent, nextHCounterTargetStep);

			//Calculate the number of pixel clock ticks we can advance
			unsigned int pixelClockTicksToAdvance = (hcounterStepsToTarget <= pixelClockTicksRemaining)? hcounterStepsToTarget: pixelClockTicksRemaining;

			//Increment the number of mclk ticks advanced
			mclkTicks += pixelClockTicksToAdvance * currentMclksPerStep;

			//Update hcounter to the position we've just advanced to
			hcounterCurrent = nextHCounterTargetStep;

			//Update the remaining pixel clock ticks
			pixelClockTicksRemaining -= pixelClockTicksToAdvance;
		}
	}
	return mclkTicks;
}

//----------------------------------------------------------------------------------------
//Access time functions
//----------------------------------------------------------------------------------------
unsigned int S315_5313::ConvertAccessTimeToMclkCount(double accessTime) const
{
	//##DEBUG##
	if(accessTime < 0)
	{
		std::wcout << "######################################################\n";
		std::wcout << "VDP ConvertAccessTimeToMclkCount accessTime < 0!\n";
		std::wcout << "accessTime:\t" << accessTime << "\n";
		std::wcout << "lastTimesliceRemainingTime:\t" << lastTimesliceRemainingTime << "\n";
		std::wcout << "currentTimesliceLength:\t" << currentTimesliceLength << "\n";
		std::wcout << "GetProcessorStateTime:\t" << GetProcessorStateTime() << "\n";
		std::wcout << "######################################################\n";
	}

	//Calculate mclk
	//##TODO## Define this properly
	double mclk;
	if(palMode)
	{
		mclk = 53.203400 * 1000000.0;
	}
	else
	{
		mclk = 53.634165 * 1000000.0;
	}

	//Calculate the current mclk cycle counter
	double roundOffAdjustment = 0.1 * (1000000000.0 / mclk);
	unsigned int mclkCyclesTarget = (unsigned int)((accessTime + roundOffAdjustment) * (mclk / 1000000000.0));
	return mclkCyclesTarget;
}

//----------------------------------------------------------------------------------------
double S315_5313::ConvertMclkCountToAccessTime(unsigned int mclkCount) const
{
	//Calculate mclk
	//##TODO## Define this properly
	double mclk;
	if(palMode)
	{
		mclk = 53.203400 * 1000000.0;
	}
	else
	{
		mclk = 53.634165 * 1000000.0;
	}

	//Calculate the access time
	double accessTime = (double)mclkCount * (1000000000.0 / mclk);
	return accessTime;
}

//----------------------------------------------------------------------------------------
//Processor state advancement functions
//----------------------------------------------------------------------------------------
//The purpose of this function is to advance the internal state of the device up to the
//indicated time. This includes updates to the status register, DMA operations, the FIFO
//buffer, and any other internal changes that occur within the VDP over a given period of
//time. This update function is always called before each register change, so it can
//assume that no register changes have occurred since the last update step.
//----------------------------------------------------------------------------------------
//We need to keep a copy of all state which can affect the number of available slots,
//and refresh them as we advance through the execution. In order to support status
//register updates, we need to fully perform sprite calculations. In order to
//calculate the render slots, we need to track the raster position, and take into
//account screen mode settings. Basically, we need to duplicate some of the render
//logic.

//Settings which affect slots:
//-R1B6(DISP): Disables the display. We know there are no access limitations when the
//display is disabled. We also know that the display can be disabled between lines,
//then enabled again before the next line starts, and the next line will still be
//drawn, so it takes effect immediately. We also know as a side-effect of disabling
//the display during hblank that the maximum number of sprites for the next line is
//reduced, which affects mickey mania. Lets start with this taking effect immediately,
//although the effect it has on the sprite rendering will need to be researched in
//order to emulate mickey mania properly, and to get the status register correct in
//this case too.
//-R1B3(M2): Switches between V28 and V30 modes. No idea when it takes effect.
//-R12B0(RS1): Switches between H32/H40 mode. Not sure if this can be done mid-line
//(notes from charles macdonald suggest this may be possible), but we know it can be
//done mid-frame.

//Settings which MAY affect slots, but need to be tested:
//-R0B0(U2): Disables video output. It's possible that access may be allowed freely like
//when the display is disabled.
//-R1B7(U1): Affects the display mode.
//-R1B0(U2): Affects the display mode.
//-R1B2(M5): Switches between M4 and M5. We know this can happen mid-frame, but we
//don't know if it can happen mid-line.

//Settings which affect status register:
//None known

//Settings which MAY affect status register, but need to be tested:
//-R5(SAT): Defines the location of the sprite attribute table. Obviously it affects
//sprites, but we don't know for sure if it can be updated during a frame or a line.
//Same goes for sprite data in VRAM. Some changes are possible, but they're limited I
//believe. Charles has some notes on this IIRC.

//So there really isn't that much we need to cache at all. We just need to keep track
//of the current render position, and use these few state parameters to advance from
//that point to the next time interval. We don't actually need to evaluate any layers
//at all, and for sprites, we only care about the size and position of each sprite, to
//support dot overflow and collision flags.

//Actually, how does the collision flag work again? Doesn't it get set if
//non-transparent pixels overlap in sprites? That would require us to touch the VRAM
//and evaluate the colour value for each pixel in each sprite. If this is the case, we
//now have to worry about VRAM updates affecting the calculation too. We also still
//don't know exactly when the sprites are rendered, but notes from Charles suggest
//that they're done on the line before the line that they're displayed on. Behaviour
//from Mickey Mania also suggests the sprite table is built during hblank. It's likely
//what really happens is that the sprite table is populated in the hblank before the
//line those sprites are used on begins, and that the actual pixel colours for each
//sprite are only evaluated during rendering, in-sync with the raster position. It
//would then be mid-frame that collision and overflow flags are set, at the exact
//pixel location when they are evalulated (although it might actually be a bit before,
//with results being buffered and then output later).

//For now, let's assume that the collision flag is set if two pixels, non-transparent
//or not, overlap during active scan. This is almost certainly wrong, but let's just
//get our new update function working first, and fix up the sprite details later. It's
//not going to radically modify the implementation anyway, and we need to do testing
//to be sure of the correct behaviour.

//bool AdvanceProcessorState(double advanceTime, bool stopAtNextAccessSlot)
//Advances the processor state, assuming no state changes occur between the current
//processor update time and the advanceTime parameter. Returns false if the
//stotAtNextAccessSlot parameter was set and the target access time wasn't reached,
//otherwise returns true. This function should be used by our UpdateInternalState()
//function to do the actual advance. Our UpdateInternalState() function will manage
//advancing the FIFO and DMA. If the FIFO is empty (or becomes empty during the update
//process), the UpdateInternalState() function can just call AdvanceProcessorState()
//with stopAtNextAccessSlot set to false to finish the update step. Since we need to
//know what the current internal state time is in order to process DMA operations, a
//GetProcessorStateTime() function will be provided to give access to the current
//advancement time, in the case that a request for an access slot means the end of the
//timeblock isn't reached.
//----------------------------------------------------------------------------------------
void S315_5313::UpdateInternalState(unsigned int mclkCyclesTarget, bool checkFifoStateBeforeUpdate, bool stopWhenFifoEmpty, bool stopWhenFifoFull, bool stopWhenFifoNotFull, bool stopWhenReadDataAvailable, bool stopWhenNoDMAOperationInProgress, bool allowAdvancePastCycleTarget)
{
	//Gather some info about the current state
	bool dmaOperationWillRun = dmaEnabled && commandCode.GetBit(5) && (!dmd1 || dmd0 || dmaFillOperationRunning);
	bool readOperationWillRun = ValidReadTargetInCommandCode() && !readDataAvailable;
	bool writeOperationWillRun = !IsWriteFIFOEmpty();

	//##DEBUG##
	if(stopWhenNoDMAOperationInProgress && !dmaOperationWillRun)
	{
		std::wcout << "######################################################\n";
		std::wcout << "VDP UpdateInternalState called with stopWhenNoDMAOperationInProgress when no DMA operation was running!\n";
		std::wcout << "mclkCyclesTarget:\t" << mclkCyclesTarget << "\n";
		std::wcout << "stateLastUpdateMclk:\t" << stateLastUpdateMclk << "\n";
		std::wcout << "stateLastUpdateMclkUnused:\t" << stateLastUpdateMclkUnused << "\n";
		std::wcout << "######################################################\n";
	}

	//Check if we're already sitting on one of the target states
	bool targetFifoStateReached = false;
	if(checkFifoStateBeforeUpdate)
	{
		//If we're already at the target state, we exit the function immediately, since
		//there's no more work to do.
		if(TargetProcessorStateReached(stopWhenFifoEmpty, stopWhenFifoFull, stopWhenFifoNotFull, stopWhenReadDataAvailable, stopWhenNoDMAOperationInProgress))
		{
			return;
		}
	}

	//Check if we need to stop at an access slot on the next step
	bool stopAtAccessSlot = writeOperationWillRun || readOperationWillRun || dmaOperationWillRun;

	//Advance the VDP until we reach the target state
	while(!targetFifoStateReached && (!AdvanceProcessorStateNew(mclkCyclesTarget, stopAtAccessSlot, allowAdvancePastCycleTarget) || allowAdvancePastCycleTarget))
	{
		//Advance a DMA transfer operation while the write FIFO or read cache is not full,
		//and there has been enough time since the read cache became empty to fully read
		//another value from external memory.
		//##TODO## We assume here that a read operation is performed immediately, whether
		//there is room in the FIFO or not currently to save it, and that the data then
		//gets held as pending until a FIFO slot opens up, at which time, the pending data
		//write then gets moved into the FIFO, and a new read operation begins
		//immediately. Do some hardware tests to confirm this is the way the real VDP
		//behaves, and confirm the timing of everything.
		while(dmaEnabled && commandCode.GetBit(5) && !dmd1 && (!dmaTransferReadDataCached || !IsWriteFIFOFull())
		  && ((dmaTransferNextReadMclk + dmaTransferReadTimeInMclkCycles) < (GetProcessorStateMclkCurrent() + dmaTransferLastTimesliceUnusedMclkCycles)))
		{
			//If there is space in the DMA transfer read cache, read a new data value into
			//the read cache.
			if(!dmaTransferReadDataCached)
			{
				CacheDMATransferReadData(dmaTransferNextReadMclk);
			}

			//Advance the dmaTransferLastReadMclk counter, taking into account any unused
			//mclk cycles from the last timeslice which were not consumed by the DMA
			//transfer operation.
			if(dmaTransferLastTimesliceUnusedMclkCycles == 0)
			{
				dmaTransferNextReadMclk += dmaTransferReadTimeInMclkCycles;
			}
			else if(dmaTransferLastTimesliceUnusedMclkCycles >= dmaTransferReadTimeInMclkCycles)
			{
				dmaTransferLastTimesliceUnusedMclkCycles -= dmaTransferReadTimeInMclkCycles;
			}
			else
			{
				dmaTransferNextReadMclk += (dmaTransferReadTimeInMclkCycles - dmaTransferLastTimesliceUnusedMclkCycles);
				dmaTransferLastTimesliceUnusedMclkCycles = 0;
			}

			//If there is space in the write FIFO to store another write value, empty the
			//DMA transfer read cache data into the FIFO.
			if(!IsWriteFIFOFull())
			{
				PerformDMATransferOperation();
				AdvanceDMAState();
			}
		}

		//Advance a DMA fill operation if fill data has been latched to trigger the fill,
		//and the write FIFO is empty. If a data port write has been made during an active
		//DMA fill operation, that data port write is performed first, and we carry on the
		//fill once the FIFO returns to an empty state.
		if(dmaEnabled && commandCode.GetBit(5) && dmd1 && !dmd0 && dmaFillOperationRunning && IsWriteFIFOEmpty())
		{
			PerformDMAFillOperation();
			AdvanceDMAState();
		}

		//Advance a DMA copy operation
		if(dmaEnabled && commandCode.GetBit(5) && dmd1 && dmd0)
		{
			PerformDMACopyOperation();
			AdvanceDMAState();
		}

		//Perform a VRAM read cache operation
		bool readOperationPerformed = false;
		if(IsWriteFIFOEmpty() && !readDataAvailable && ValidReadTargetInCommandCode())
		//##TODO## Evaluate this old code, and figure out what to do with it.
		//if(!readDataAvailable && ValidReadTargetInCommandCode())
		{
			PerformReadCacheOperation();
			readOperationPerformed = true;
		}

		//Perform a VRAM write operation
		if(!IsWriteFIFOEmpty() && !readOperationPerformed)
		{
			PerformFIFOWriteOperation();
		}

		//If a DMA transfer operation is in progress, and there's a read value held in the
		//DMA transfer read cache as pending, and the write FIFO now has a slot available,
		//load the cached DMA transfer read data into the write FIFO. This restores the
		//FIFO state back to full at the end of this update step if a DMA transfer has
		//more data pending, which is essential in order to ensure that the FIFO full flag
		//appears as full immediately after a DMA transfer operation has reached the end
		//of the source data.
		if(dmaEnabled && commandCode.GetBit(5) && !dmd1 && dmaTransferReadDataCached && !IsWriteFIFOFull())
		{
			//If there is space in the write FIFO to store another write value, empty the
			//DMA transfer read cache data into the FIFO.
			PerformDMATransferOperation();
			AdvanceDMAState();
		}

		//Update the FIFO full and empty flags in the status register
		SetStatusFlagFIFOEmpty(IsWriteFIFOEmpty());
		SetStatusFlagFIFOFull(IsWriteFIFOFull());

		//Gather some info about the current state
		dmaOperationWillRun = dmaEnabled && commandCode.GetBit(5) && (!dmd1 || dmd0 || dmaFillOperationRunning);
		readOperationWillRun = ValidReadTargetInCommandCode() && !readDataAvailable;
		writeOperationWillRun = !IsWriteFIFOEmpty();

		//Check if we need to stop at an access slot on the next step
		stopAtAccessSlot = writeOperationWillRun || readOperationWillRun || dmaOperationWillRun;

		//Stop the update process if one of the target states has been reached
		targetFifoStateReached = TargetProcessorStateReached(stopWhenFifoEmpty, stopWhenFifoFull, stopWhenFifoNotFull, stopWhenReadDataAvailable, stopWhenNoDMAOperationInProgress);
	}
}

//----------------------------------------------------------------------------------------
void S315_5313::PerformReadCacheOperation()
{
	//Hardware tests have shown that when doing a read from VSRAM or CRAM, not all
	//the bits in the 16-bit result are set as a result of the read. The bits
	//which are not set as a result of the read from VSRAM or CRAM, are set based
	//on the next available entry in the FIFO buffer. There is one interesting and
	//quite special case to note however. If a write is made to the data port,
	//immediately followed by a read, and the write itself has not yet been
	//processed by the time the read is attempted, due to no access slot being
	//available, the read is processed BEFORE the write. The written data
	//overwrites the contents of the next available entry in the FIFO buffer, and
	//uninitialized bits in the read data now get set based on the corresponding
	//bits in the written data. This can be observed by attempting to write to a
	//read target, immediately followed by a read. We emulate this behaviour here
	//by loading the first pending data port write, if one exists, into the FIFO
	//buffer at the next available entry, then using the next available entry in
	//the FIFO as the initial value for the read operation. The data port write
	//will then be processed later, and perform the same write into the FIFO
	//buffer, resulting in no change, and it will then be processed as normal.
	//##TODO## I now suspect the above theory is incorrect. We have found from
	//additional hardware testing that, generally speaking, after writing to a
	//read target, attempting a read locks up the hardware. I suspect the
	//exception to this rule, which we have observed, happens when a read
	//pre-cache is in progress or complete at the time the write occurs. In this
	//case, the next read works, with the returned value possibly being combined
	//with the invalid write data rather than the previous FIFO contents. After
	//this read however, the following read will cause a lock-up. The exact reason
	//for this lockup is unknown, but for our purposes, we will assume CD4 is set
	//after a data port write occurs.
	//if(!IsWriteFIFOEmpty())
	//{
	//	fifoBuffer[fifoNextReadEntry] = *pendingDataPortWrites.begin();
	//}

	//Note that we have confirmed that a VRAM read operation doesn't use the
	//actual write FIFO to store the read value. This has been tested on the
	//hardware by alternating between VRAM, CRAM, and VSRAM read operations, and
	//observing the resulting values of the uninitialized bits from CRAM and VSRAM
	//reads. The results of these tests show that the contents of the FIFO is not
	//modified as a result of a read operation, it is merely the uninitialized
	//bits in the read data which obtain their value from the next entry in the
	//FIFO. The fact that reads are processed before writes, as detailed above,
	//also confirms that the data from the FIFO and the read data is combined at
	//the time the read occurs, IE, it is not the live state of the FIFO that the
	//read value is combined with at the time the data is being output over the
	//data port, as when a write has been combined with a read as described above,
	//the result is the same, no matter how long after the write has occurred the
	//user actually then reads the data port to obtain the latched read value.
	//##TODO## Confirm the above assertion about the live state of the FIFO not
	//being used at the time the read value is output over the data port.
	//Re-running our original test with the NOP operations inserted should give us
	//the answer.
	if(!readDataHalfCached)
	{
		readBuffer = fifoBuffer[fifoNextReadEntry].dataPortWriteData;
	}

	//All possible combinations of the code flags and data port reads have been
	//tested on the hardware. Reads are decoded based on the lower 5 bits of the
	//code data.
	RAMAccessTarget ramAccessTarget;
	ramAccessTarget.AccessTime(GetProcessorStateMclkCurrent());
	switch(commandCode.GetDataSegment(0, 5))
	{
	case 0x00:{ //?00000 VRAM Read
		//Note that hardware tests have shown that the LSB of the address is ignored for
		//16-bit reads from VRAM.
		Data tempAddress(commandAddress);
		tempAddress.SetBit(0, false);

		//##NOTE## Hardware tests have shown that the upper byte of the 16-bit
		//value is read first. This has been observed by examining the results
		//when performing reads from odd VRAM addresses. It appears that when data
		//is read from an odd address, the flag is set indicating that read data
		//is available, even though the data has actually only been half cached.
		//If a data port read is then made before the other byte is cached, the
		//read will return a data value where only the upper byte of the result
		//comes from the target address, and the lower byte of the result retains
		//whatever the previous value was from the last read value to successfully
		//cache a lower byte.
		//##TODO## Implement these results into the way we perform reads.
		//##TODO## Comment what's going on here with the read operations. The
		//M5ReadVRAM8Bit() function inverts the LSB of the address, so this is a
		//bit confusing.
		if(!readDataHalfCached)
		{
			Data tempDataBuffer(8);
			M5ReadVRAM8Bit(tempAddress+1, tempDataBuffer, ramAccessTarget);
			readBuffer.SetByte(1, tempDataBuffer.GetData());
			readDataHalfCached = true;
		}
		else
		{
			M5ReadVRAM8Bit(tempAddress, readBuffer, ramAccessTarget);
			readDataHalfCached = false;
			readDataAvailable = true;
		}
		break;}
	case 0x04: //?00100 VSRAM Read
		M5ReadVSRAM(commandAddress, readBuffer, ramAccessTarget);
		readDataAvailable = true;
		break;
	case 0x08: //?01000 CRAM Read
		M5ReadCRAM(commandAddress, readBuffer, ramAccessTarget);
		readDataAvailable = true;
		break;
	case 0x0C: //?01100 8-bit VRAM Read (undocumented)
		//This undocumented read mode performs a true 8-bit VRAM read. The lower 8 bits
		//return an 8-bit value read from VRAM, while the upper 8 bits are unaffected.
		M5ReadVRAM8Bit(commandAddress, readBuffer, ramAccessTarget);
		readDataAvailable = true;
		break;
	default: //Invalid
		//##TODO## Update these comments, and the way we handle invalid read
		//attempts.
		//Any attempts to read from the data port when the lower five bits don't match one
		//of the above patterns causes the VDP to lock up. A reset is unable to restore
		//normal operation. Only power cycling the device can bring the VDP back from this
		//state.
		//Update cached data for a read operation. Note that on the real VDP,
		//attempting to read from an invalid target causes the system to lock up when
		//reading from the data port. The reason this occurs is that the VDP never
		//successfully fetches a data word for the read request, so the data port read
		//is waiting for the readDataAvailable flag to be set, which never actually
		//occurs in this case. We catch cases where this would occur in our emulator
		//at the time the data port is read, so we can report the error to the user,
		//and avoid the infinite loop that would otherwise occur.
		//##TODO## Raise some kind of hard error when this occurs.
		break;
	}

	//##FIX## Is this correct? We need to sort out how we track incremented
	//address register data for operations such as reads and DMA fill/transfer
	//operations.
	//Increment the target address
	if(!readDataHalfCached)
	{
		commandAddress += autoIncrementData;
	}
}

//----------------------------------------------------------------------------------------
void S315_5313::PerformFIFOWriteOperation()
{
	//##TODO## Update all the comments here
	//##TODO## Completely update this section of code to use our new FIFO
	//structure
	FIFOBufferEntry& fifoBufferEntry = fifoBuffer[fifoNextReadEntry];

	//Move the pending write through the physical 4-word FIFO. The physical FIFO
	//is only maintained to correctly support old data being combined and blended
	//with data being read from CRAM or VSRAM in the uninitialized bits.
	//			fifoBuffer[fifoNextReadEntry] = data;
	//			fifoNextReadEntry = (fifoNextReadEntry+1) % fifoBufferSize;

	//Process the write
	//##FIX## We know from VRAM snooping as well as the official documentation,
	//that access to VRAM is byte-wide, while access to CRAM and VSRAM is
	//word-wide. What this means is it actually takes two access clock cycles to
	//read a word from VRAM, and it takes two access clock cycles to write a word
	//to VRAM. We know from hardware tests that only a single FIFO entry is used
	//to store a full word-wide write to VRAM, so there must be some kind of
	//internal flag which records whether only one byte has been written to VRAM
	//from a pending word-wide write. Note that most likely, the lower byte is
	//written first, owing to the byteswapped layout in VRAM, but this should be
	//tested in hardware by snooping on the VRAM bus during the write operation.

	//All possible combinations of the code flags and data port writes have been tested
	//on the hardware. Writes are decoded based on the lower 4 bits of the code data.
	RAMAccessTarget ramAccessTarget;
	ramAccessTarget.AccessTime(GetProcessorStateMclkCurrent());
	switch(fifoBufferEntry.codeRegData.GetDataSegment(0, 4))
	{
	case 0x01:{ //??0001 VRAM Write
		//-Hardware tests have verified that if the LSB of the address is set, it is ignored
		//when determining the target VRAM address, but it acts as a special flag causing the
		//data to be byteswapped for writes to VRAM. This is true for any write to VRAM,
		//including writes performed as part of a DMA transfer. The LSB of the address is
		//ignored for reads from VRAM, IE, no byteswapping is ever performed on reads.
		//-It should be noted that the real VDP actually stores all VRAM data byteswapped in
		//the physical memory. This has been confirmed by using a logic analyzer to snoop on
		//the VRAM bus during operation. This means that in reality, byteswapping on VRAM
		//writes really occurs when the LSB is unset, while the data is byteswapped if the LSB
		//is set, and all reads are byteswapped. We don't byteswap the actual VRAM data in our
		//emulator, as not only is this byteswapping virtually transparent to the caller
		//except in the case of DMA fills and copies (refer to the implementation for further
		//info), it would be slower for us to byteswap everything on every read and write to
		//VRAM. Since it's faster for us, and more convenient and logical for the user
		//therefore for the data to be stored sequentially, we don't store data as byteswapped
		//in VRAM.
		//-Note that the real VDP also stores the VRAM data in a non-linear fashion, with data
		//within each 0x400 byte block stored in an interleaved format. The byteswapped data
		//is striped in 4-byte groups within each 0x400 byte block, with all the first bytes
		//of each 4-byte set at 0x000-0x100, then the second bytes at 0x100-0x200, and so on
		//within each 0x400 byte block. This is necessary in order to support the serial
		//access mode used to read data from VRAM. We also don't implement this interleaved
		//VRAM in our emulator, as it is an implementation detail that has no effect on the
		//logical operation of the VDP.
		Data tempAddress(fifoBufferEntry.addressRegData);
		//##TODO## Comment this
		unsigned int dataByteToRead = (tempAddress.GetBit(0) ^ fifoBufferEntry.dataWriteHalfWritten)? 1: 0;
		tempAddress.SetBit(0, fifoBufferEntry.dataWriteHalfWritten);
		Data writeData(8, fifoBufferEntry.dataPortWriteData.GetByte(dataByteToRead));
		M5WriteVRAM8Bit(tempAddress, writeData, ramAccessTarget);
		fifoBufferEntry.dataWriteHalfWritten = !fifoBufferEntry.dataWriteHalfWritten;
		fifoBufferEntry.pendingDataWrite = fifoBufferEntry.dataWriteHalfWritten;
		break;}
	case 0x03: //??0011 CRAM Write
		M5WriteCRAM(fifoBufferEntry.addressRegData, fifoBufferEntry.dataPortWriteData, ramAccessTarget);
		fifoBufferEntry.pendingDataWrite = false;
		break;
	case 0x05: //??0101 VSRAM Write
		M5WriteVSRAM(fifoBufferEntry.addressRegData, fifoBufferEntry.dataPortWriteData, ramAccessTarget);
		fifoBufferEntry.pendingDataWrite = false;
		break;
	default: //Invalid
		//Any attempts to write to the data port when the lower four bits don't match
		//one of the above patterns has no effect. The write is discarded, and the
		//VDP is unaffected.
		fifoBufferEntry.pendingDataWrite = false;
		break;
	}

	//If this pending write has been fully processed, advance to the next entry in
	//the FIFO buffer.
	if(!fifoBufferEntry.pendingDataWrite)
	{
		fifoNextReadEntry = (fifoNextReadEntry+1) % fifoBufferSize;

		//##TODO## CD4 is most likely not set by a DMA fill operation. We can test this
		//though. Try doing a DMA fill to a read target, with a non-empty FIFO at the time
		//the control port data is written. This should trigger an immediate DMA fill
		//operation to a read target, which should do nothing at all. After this, we can
		//attempt a read. Actually, this will probably lock up. Bad things happen when you
		//mix read and write operations. Still, if it locks up, that's enough evidence to
		//indicate that CD4 is not set as a result of a DMA fill operation.
		if(dmaEnabled && commandCode.GetBit(5) && dmd1 && !dmd0)
		{
			dmaFillOperationRunning = true;
		}

		//Check if a DMA transfer operation is in progress, and the transfer is stalled
		//waiting for a slot to open up in the write FIFO. This will be the case if there
		//is currently a value held in the DMA transfer read cache. In this case, we could
		//not have performed any more external memory reads before this point. Since we
		//just made a write slot available in the FIFO, we set the dmaTransferNextReadMclk
		//variable to the current processor state time, so that the next external memory
		//read for the DMA transfer operation will start no sooner than this time.
		if(dmaEnabled && commandCode.GetBit(5) && !dmd1 && dmaTransferReadDataCached)
		{
			unsigned int processorStateMclkCurrent = GetProcessorStateMclkCurrent();
			if(dmaTransferLastTimesliceUnusedMclkCycles == 0)
			{
				dmaTransferNextReadMclk = processorStateMclkCurrent;
			}
			else if(dmaTransferLastTimesliceUnusedMclkCycles >= processorStateMclkCurrent)
			{
				dmaTransferNextReadMclk = 0;
				dmaTransferLastTimesliceUnusedMclkCycles -= processorStateMclkCurrent;
			}
			else
			{
				dmaTransferNextReadMclk = (processorStateMclkCurrent - dmaTransferLastTimesliceUnusedMclkCycles);
				dmaTransferLastTimesliceUnusedMclkCycles = 0;
			}
		}
	}
}

//----------------------------------------------------------------------------------------
void S315_5313::PerformDMAFillOperation()
{
	//##FIX## We need to determine how the VDP knows a write has been made to
	//the data port. VSRAM and CRAM fill targets grab the next available entry
	//in the FIFO, after the write has been made and the FIFO has been
	//advanced, so they perform the fill operation with the wrong data. A VRAM
	//fill target uses the correct data, but it appears the write is still
	//processed first, possibly even both halves of the write. Further testing
	//is required.
	//##TODO## Hardware tests have shown that if writes are pending in the
	//FIFO at the time a control port write sets up a DMA fill operation, the
	//DMA fill operation will trigger without waiting for a data port write,
	//and will use an entry from the current FIFO buffer instead. Exactly
	//which entry is selected, and whether the DMA fill triggers on the next
	//write or the last write, still needs to be confirmed however.
	//Only advance a DMA fill operation if the FIFO is not empty
	//##FIX## The write needs to be processed first
	//##FIX## Hardware tests have shown that if the data port is written to
	//while a DMA fill operation is already in progress, the data port write
	//is processed immediately at the next access slot, before the fill
	//operation is advanced. The data port write occurs at the current
	//incremented address that the DMA fill operation was up to, and the DMA
	//fill operation continues from the incremented address after the write
	//has been processed. We need to emulate this behaviour. It is also clear
	//from this that pending write entries in the FIFO need to be processed
	//before DMA fill update steps.

	//Manipulate the source target address for the DMA fill operation.
	//-Note that on the real hardware, the VDP stores its data in VRAM with
	//each 16-bit word byteswapped, so a value of 0x1234 would be stored in
	//the physical memory as 0x3412. This has been confirmed through the use
	//of a logic analyzer snooping on the VRAM bus during operation. For most
	//operations, this byteswapping of VRAM memory is transparent to the user,
	//since the VDP automatically byteswaps all word-wide reads and writes to
	//and from VRAM, so the data is read and written as if it was not
	//byteswapped at all. DMA fill and copy operations are the only time where
	//the byteswapping behaviour of the real hardware is visible to the user,
	//as the byte-wide VRAM access that occurs as part of these operations
	//allow reads and writes to and from odd addresses in VRAM. In the real
	//hardware, these addresses are used directly, without modification, to
	//read and write the byteswapped data, meaning that reads and writes from
	//odd addresses actually access the upper byte of a word, and reads and
	//writes to even addresses actually access to the lower byte of the word.
	//For our emulator, we store data in VRAM without byteswapping, to
	//simplify the implementation and present the data to the user in the form
	//they would expect when using the debugger. In order to correctly
	//implement the behaviour of a DMA fill or copy however, we therefore have
	//to swap odd and even addresses when performing byte-wide access, so that
	//we get the correct result.
	//-Note that the official documentation from Sega listing the order the
	//data is written to VRAM during a DMA fill operation is technically
	//accurate, but is very misleading, since the order and pattern of the
	//writes that they list shows the actual write pattern to the byteswapped
	//memory, with the order of all bytes being reversed to what the reader
	//may expect. At no point in the official documentation is it mentioned
	//that the data in VRAM is byteswapped.
	//Data targetAddressByteswapped(16, commandAddress.GetData());
	//targetAddressByteswapped.SetBit(0, !targetAddressByteswapped.GetBit(0));

	//Check if this DMA fill operation is targeting VRAM. Right now, we have
	//special case handling for VRAM write targets as opposed to CRAM or VSRAM
	//write targets, as there are some implementation quirks in the VDP that
	//result in different behaviour for CRAM or VSRAM targets.
	//##TODO## Find a way to unify this code, and end up with a clean
	//implementation.
	bool fillTargetsVRAM = (commandCode.GetDataSegment(0, 4) == 0x01);

	//Increment the target address for the last entry
	unsigned int fifoLastReadEntry = (fifoNextReadEntry+(fifoBufferSize-1)) % fifoBufferSize;
	fifoBuffer[fifoLastReadEntry].addressRegData += autoIncrementData;

	//##TODO## We need to determine exactly how the VDP latches the DMA fill
	//data. It seems to me, the most likely explanation is that a DMA fill is
	//triggered after the first half of a data port write is written, and then
	//kicks in and repeats the second half of that write repeatedly. Since a
	//write to CRAM or VSRAM is 16-bit, and completes in one step, this
	//results in the FIFO being advanced, and the entire write being repeated.
	Data fillData(16);
	if(fillTargetsVRAM)
	{
		//Extract the upper byte of the written data. This single byte of data is
		//used to perform the fill. Hardware tests have shown no other data gets
		//written to the FIFO using a DMA fill operation other than the normal
		//write that triggers the fill, and the FIFO is not advanced at all during
		//the fill. Also note that the byteswapping behaviour of VRAM writes has
		//no impact on which data is used for the fill operation. The upper byte
		//of the data written to the data port is always used for the fill,
		//regardless of whether the write is performed to an even or odd VRAM
		//address.
		fillData = fifoBuffer[fifoLastReadEntry].dataPortWriteData.GetUpperBits(8);
	}
	else
	{
		//##FIX## Hardware tests have shown that when a DMA fill is being
		//performed to CRAM or VSRAM, the write to the data port isn't used as the
		//fill data for the operation. What happens instead is that the data write
		//is performed as normal, and the FIFO is advanced, then when the DMA fill
		//operation triggers, it is the data in the next available FIFO slot that
		//is used for the fill data. This only occurs for DMA fills to CRAM or
		//VSRAM, and is no doubt due to the fact that VRAM requires two byte-wide
		//writes to commit a single word-wide write to the VRAM, while CRAM and
		//VSRAM perform a single word-wide operation.
		fillData = fifoBuffer[fifoNextReadEntry].dataPortWriteData;
	}

	//##NOTE## Hardware tests have shown that during a DMA fill operation, the
	//FIFO flags in the status register remain with the empty flag set, and
	//the full flag cleared, throughout the fill operation, with of course the
	//exception of when the data port write occurs to trigger the fill, if
	//that write is waiting in the FIFO at the time of the control port read.
	//Note that the DMA busy flag remains set as soon as the control port is
	//written, even if the data port write to trigger the fill hasn't yet been
	//made. The DMA busy flag remains set until a data port write has been
	//made and the DMA fill is complete.
	//##FIX## Hardware tests have shown that DMA fill operations to CRAM and
	//VSRAM are possible, and work correctly, with the exception that the
	//wrong data is used for the fill, as described above. We need to emulate
	//this by actually using the specified write target when performing the
	//write below. Also note that a single DMA fill write is a 16-bit
	//operation to VSRAM and CRAM, with the full 16-bit fill value being used
	//at each target address, and the LSB of the address being masked.
	//Transfer the next byte of the fill operation
	//##TODO## Test on hardware to determine what happens when the data port
	//is written to while a DMA fill operation is in progress.
	RAMAccessTarget ramAccessTarget;
	ramAccessTarget.AccessTime(GetProcessorStateMclkCurrent());
	switch(commandCode.GetDataSegment(0, 4))
	{
	case 0x01: //??0001 VRAM Write
		M5WriteVRAM8Bit(fifoBuffer[fifoLastReadEntry].addressRegData, fillData, ramAccessTarget);
		break;
	case 0x03: //??0011 CRAM Write
		M5WriteCRAM(fifoBuffer[fifoLastReadEntry].addressRegData, fillData, ramAccessTarget);
		break;
	case 0x05: //??0101 VSRAM Write
		M5WriteVSRAM(fifoBuffer[fifoLastReadEntry].addressRegData, fillData, ramAccessTarget);
		break;
	}

	//##FIX## This is incorrect. We know from testing that if a data port
	//write occurs while a DMA fill operation is in progress, that write
	//occurs at the currently incremented address that the next fill write
	//would go to, and the fill operation proceeds at the incremented address
	//after the write.
	//##NOTE## Fixing this could be as simple as setting the
	//codeAndAddressRegistersModifiedSinceLastWrite register to true here,
	//which should trigger the next data port write to obtain its code and
	//address register data directly from commandAddress and commandCode,
	//rather than using the previous FIFO buffer entry contents. This won't
	//work fully actually, since the DMA fill operation then needs to continue
	//at the incremented address after the write. It does therefore seem as if
	//the actual address data that is used and modified actively by the DMA
	//fill update step is the address data stored in the FIFO entry which
	//triggered the fill. I just noticed above that this is actually what we
	//are using. Why are we incrementing commandAddress here then? We don't
	//actually seem to be using it anywhere. We should update our code, and
	//our comments.
	//Increment the target address
	commandAddress += autoIncrementData;
}

//----------------------------------------------------------------------------------------
void S315_5313::PerformDMACopyOperation()
{
	//Get the current source address
	unsigned int sourceAddress = (dmaSourceAddressByte1) | (dmaSourceAddressByte2 << 8);

	//Manipulate the source and target addresses for the DMA copy operation.
	//-Note that on the real hardware, the VDP stores its data in VRAM with
	//each 16-bit word byteswapped, so a value of 0x1234 would be stored in
	//the physical memory as 0x3412. This has been confirmed through the use
	//of a logic analyzer snooping on the VRAM bus during operation. For most
	//operations, this byteswapping of VRAM memory is transparent to the user,
	//since the VDP automatically byteswaps all word-wide reads and writes to
	//and from VRAM, so the data is read and written as if it was not
	//byteswapped at all. DMA fill and copy operations are the only time where
	//the byteswapping behaviour of the real hardware is visible to the user,
	//as the byte-wide VRAM access that occurs as part of these operations
	//allow reads and writes to and from odd addresses in VRAM. In the real
	//hardware, these addresses are used directly, without modification, to
	//read and write the byteswapped data, meaning that reads and writes from
	//odd addresses actually access the upper byte of a word, and reads and
	//writes to even addresses actually access to the lower byte of the word.
	//For our emulator, we store data in VRAM without byteswapping, to
	//simplify the implementation and present the data to the user in the form
	//they would expect when using the debugger. In order to correctly
	//implement the behaviour of a DMA fill or copy however, we therefore have
	//to swap odd and even addresses when performing byte-wide access, so that
	//we get the correct result.
	Data sourceAddressByteswapped(16, sourceAddress);
	sourceAddressByteswapped.SetBit(0, !sourceAddressByteswapped.GetBit(0));
	Data targetAddressByteswapped(16, commandAddress.GetData());
	targetAddressByteswapped.SetBit(0, !targetAddressByteswapped.GetBit(0));

	//Ensure that CD4 is set when performing a DMA copy. Hardware tests have
	//shown that while the state of CD0-CD3 is completely ignored for DMA copy
	//operations, CD4 must be set, otherwise the VDP locks up hard when the
	//DMA operation is triggered.
	if(!commandCode.GetBit(4))
	{
		//##TODO## Log an error or trigger an assert here
	}

	//Perform the copy. Note that hardware tests have shown that DMA copy
	//operations always target VRAM, regardless of the state of CD0-CD3.
	RAMAccessTarget ramAccessTarget;
	ramAccessTarget.AccessTime(GetProcessorStateMclkCurrent());
	unsigned char data;
	data = vram->Read(sourceAddressByteswapped.GetData(), ramAccessTarget);
	vram->Write(targetAddressByteswapped.GetData(), data, ramAccessTarget);

	//Increment the target address
	commandAddress += autoIncrementData;
}

//----------------------------------------------------------------------------------------
void S315_5313::CacheDMATransferReadData(unsigned int mclkTime)
{
	//Get the current source address. Note that for a DMA transfer operation, the source
	//address is stored without the LSB, so we need to shift the complete address data up
	//by one.
	unsigned int sourceAddress = (dmaSourceAddressByte1 << 1) | (dmaSourceAddressByte2 << 9) | (dmaSourceAddressByte3 << 17);

	//Read the next data word to transfer from the source
	memoryBus->ReadMemory(sourceAddress, dmaTransferReadCache, GetDeviceContext(), dmaTransferNextReadMclk);

	//Flag that data has been cached for the DMA transfer operation
	dmaTransferReadDataCached = true;
}

//----------------------------------------------------------------------------------------
void S315_5313::PerformDMATransferOperation()
{
	//Add the data write to the FIFO
	//-Note that we can grab the current working value for the command code
	//and address register data, since we know we obtained exclusive bus
	//access when the DMA transfer command word was written.
	//-Note that a DMA transfer will be triggered regardless of the state of
	//CD4, as this bit isn't tested when deciding whether to trigger an
	//external DMA transfer, and write target decoding only looks at the state
	//of CD3-CD0.
	//-Note that hardware testing has confirmed that DMA transfers move the
	//data through the FIFO in the same manner as normal VRAM write
	//operations, with the same rules for memory wrapping and byte swapping.
	//Note that genvdp.txt by Charles MacDonald reports that DMA transfers to
	//CRAM are aborted when the target address passes 0x7F. This is incorrect.
	//The palette corruption reported in "Batman and Robin" on the Mega Drive
	//is due to the fact that DMA operations actively update the DMA source
	//count registers as the operation is being performed, meaning that no
	//matter what transfer count was used, the DMA length registers should
	//both be 0 after the DMA operation is completed. Batman and Robin only
	//writes to the lower transfer count byte when performing some transfers
	//to CRAM, meaning that unless the upper byte is already 0, too much data
	//will be transferred to CRAM, corrupting the palette. If the DMA source
	//and DMA length registers are correctly updated by DMA operations, as
	//hardware tests have proven does occur, this bug will not occur.
	if(codeAndAddressRegistersModifiedSinceLastWrite)
	{
		fifoBuffer[fifoNextWriteEntry].codeRegData = commandCode;
		fifoBuffer[fifoNextWriteEntry].addressRegData = commandAddress;
		codeAndAddressRegistersModifiedSinceLastWrite = false;
	}
	else
	{
		unsigned int fifoLastWriteEntry = (fifoNextWriteEntry+(fifoBufferSize-1)) % fifoBufferSize;
		fifoBuffer[fifoNextWriteEntry].codeRegData = fifoBuffer[fifoLastWriteEntry].codeRegData;
		fifoBuffer[fifoNextWriteEntry].addressRegData = fifoBuffer[fifoLastWriteEntry].addressRegData + autoIncrementData;
	}
	fifoBuffer[fifoNextWriteEntry].dataPortWriteData = dmaTransferReadCache;
	fifoBuffer[fifoNextWriteEntry].dataWriteHalfWritten = false;
	fifoBuffer[fifoNextWriteEntry].pendingDataWrite = true;

	//Advance the FIFO to the next slot
	fifoNextWriteEntry = (fifoNextWriteEntry+1) % fifoBufferSize;

	//Now that the cached DMA transfer data has been written to the FIFO, clear the flag
	//indicating that DMA transfer data is currently cached.
	dmaTransferReadDataCached = false;
}

//----------------------------------------------------------------------------------------
void S315_5313::AdvanceDMAState()
{
	//Decrement the DMA transfer count registers. Note that the transfer count
	//is decremented before it is tested against 0, and all DMA operations are
	//advanced by one step before the transfer counter is decremented, so a
	//transfer count of 0 is equivalent to a transfer count of 0x10000.
	dmaLengthCounter = (dmaLengthCounter - 1) & 0xFFFF;

	//Increment the DMA source address registers. Note that all DMA operations
	//cause the DMA source address registers to be advanced, including a DMA
	//fill operation, even though a DMA fill doesn't actually use the source
	//address. This has been confirmed through hardware tests. Also note that
	//only the lower two DMA source address registers are modified. Register
	//0x17, which contains the upper 7 bits, of the source address for a DMA
	//transfer operation, is not updated, which prevents a DMA transfer
	//operation from crossing a 0x20000 byte boundary. This behaviour is
	//undocumented but well known, and has been verified through hardware
	//tests.
	unsigned int incrementedDMASourceAddress = dmaSourceAddressByte1 | (dmaSourceAddressByte2 << 8);
	++incrementedDMASourceAddress;
	dmaSourceAddressByte1 = incrementedDMASourceAddress & 0xFF;
	dmaSourceAddressByte2 = (incrementedDMASourceAddress >> 8) & 0xFF;

	//If the DMA length counter is 0 after a DMA operation has been advanced,
	//we've reached the end of the DMA operation. In this case, we clear CD5.
	//This flags the DMA operation as completed. If we're in a DMA transfer
	//operation, this will also cause the bus to be released by the DMA worker
	//thread.
	if(dmaLengthCounter == 0)
	{
		commandCode.SetBit(5, false);
		SetStatusFlagDMA(false);

		//If we were running a DMA fill or DMA transfer operation, flag that it is
		//now completed.
		dmaFillOperationRunning = false;
		dmaTransferActive = false;
	}

	//Flag that the cached DMA settings have been modified
	cachedDMASettingsChanged = true;
}

//----------------------------------------------------------------------------------------
bool S315_5313::TargetProcessorStateReached(bool stopWhenFifoEmpty, bool stopWhenFifoFull, bool stopWhenFifoNotFull, bool stopWhenReadDataAvailable, bool stopWhenNoDMAOperationInProgress)
{
	//Check if a DMA operation is currently running
	bool dmaOperationRunning = dmaEnabled && commandCode.GetBit(5) && (!dmd1 || dmd0 || dmaFillOperationRunning);

	//Check if we've reached one of the target processor states
	bool targetProcessorStateReached = false;
	if(stopWhenFifoEmpty && GetStatusFlagFIFOEmpty())
	{
		targetProcessorStateReached = true;
	}
	else if(stopWhenFifoFull && GetStatusFlagFIFOFull())
	{
		targetProcessorStateReached = true;
	}
	else if(stopWhenFifoNotFull && !GetStatusFlagFIFOFull())
	{
		targetProcessorStateReached = true;
	}
	else if(stopWhenReadDataAvailable && readDataAvailable)
	{
		targetProcessorStateReached = true;
	}
	else if(stopWhenNoDMAOperationInProgress && !dmaOperationRunning)
	{
		targetProcessorStateReached = true;
	}

	//Return the result of the processor state check
	return targetProcessorStateReached;
}

//----------------------------------------------------------------------------------------
//##TODO## Remove this old function
//bool S315_5313::AdvanceProcessorState(unsigned int mclkCyclesTarget, bool stopAtNextAccessSlot, bool allowAdvancePastTargetForAccessSlot)
//{
//	//Ensure that we aren't trying to trigger an update out of order
//	if((!stopAtNextAccessSlot || !allowAdvancePastTargetForAccessSlot) && (mclkCyclesTarget < stateLastUpdateMclk))
//	{
//		//##TODO## Raise an assert if this occurs
//		//##DEBUG##
//		std::wcout << "######################################################\n";
//		std::wcout << "VDP AdvanceProcessorState called out of order!\n";
//		std::wcout << "mclkCyclesTarget:\t" << mclkCyclesTarget << "\n";
//		std::wcout << "stateLastUpdateMclk:\t" << stateLastUpdateMclk << "\n";
//		std::wcout << "stateLastUpdateMclkUnused:\t" << stateLastUpdateMclkUnused << "\n";
//		std::wcout << "######################################################\n";
//		return true;
//	}
//
//	unsigned int hcounterBeforeUpdate = hcounter.GetData();
//
//	//Calculate the total number of mclk cycles which need to be advanced
//	unsigned int mclkCyclesToExecute = (mclkCyclesTarget - stateLastUpdateMclk) + stateLastUpdateMclkUnused;
//
//	//Calculate the total number of pixel clock cycles which will occur if the VDP is
//	//advanced the given number of mclk cycles.
//	//##FIX## This won't work, since the video mode may need to change within the update
//	//step, invalidating our result. Currently, this function may tell us that we need to
//	//execute 20 pixel clock steps, while in reality we only need to execute 16, or vice
//	//versa.
//	//##TODO## In order to solve our screen mode settings change issue, we need to fold
//	//the current logic contained within the AdvanceStatusRegisterAndHVCounter() function
//	//into this function, so that we can perform our calculations within the "safe" blocks
//	//between screen mode changes, and simply combine the totals across all our blocks to
//	//work out overall targets.
//	unsigned int mclkRemainingCycles;
//	unsigned int totalPixelClockCyclesToExecute;
//	totalPixelClockCyclesToExecute = GetPixelClockTicksForMclkTicks(mclkCyclesToExecute, hcounterBeforeUpdate, screenModeH40, mclkRemainingCycles);
//
//	//Locate and stop the update at the next available access slot, if requested.
//	bool result = true;
//	if(stopAtNextAccessSlot)
//	{
//		//Calculate the number of pixel clock cycles until the next available access slot
//		unsigned int totalPixelClockCyclesUntilAccessSlot;
//		totalPixelClockCyclesUntilAccessSlot = GetPixelClockTicksUntilNextAccessSlot(hcounterBeforeUpdate, screenModeH40);
//
//		//If an access slot occurs within the update region, stop the update at the access
//		//slot.
//		//##FIX## Evaluate the way we use this function, and make sure that if an access
//		//slot lies exactly on the target MCLK cycle count, that an access slot is
//		//actually allowed at that location. We return false here in this case, so it's
//		//possible the calling code doesn't actually allow access when false is returned.
//		if((totalPixelClockCyclesUntilAccessSlot < totalPixelClockCyclesToExecute) || allowAdvancePastTargetForAccessSlot)
//		{
//			//Since we are stopping at an access slot, set the number of pixel clock
//			//cycles to execute as the number of cycles until the access slot occurs, and
//			//return false to indicate to the caller that an access slot stopped the
//			//update before the target was reached.
//			totalPixelClockCyclesToExecute = totalPixelClockCyclesUntilAccessSlot;
//			result = false;
//		}
//	}
//
//	//Update the status register and HV counter
//	AdvanceStatusRegisterAndHVCounter(totalPixelClockCyclesToExecute);
//
//	//Update the current state mclk cycle counter and remaining mclk cycles
//	if(!result)
//	{
//		//##FIX## This can't work, because screenModeH40 is a member of the class, which
//		//gets changed during the AdvanceStatusRegisterAndHVCounter() function call above.
//		//Potentially, in a single step, more than one video mode is active. We need the
//		//actual AdvanceStatusRegisterAndHVCounter() function to return the number of mclk
//		//cycles which were advanced.
//		stateLastUpdateMclk = (stateLastUpdateMclk + GetMclkTicksForPixelClockTicks(totalPixelClockCyclesToExecute, hcounterBeforeUpdate, screenModeH40)) - stateLastUpdateMclkUnused;
//		stateLastUpdateMclkUnused = 0;
//	}
//	else
//	{
//		stateLastUpdateMclk = mclkCyclesTarget;
//		stateLastUpdateMclkUnused = mclkRemainingCycles;
//	}
//
//	return result;
//}

//----------------------------------------------------------------------------------------
//##TODO## This function needs to populate the vsramCachedRead variable, where the last
//VSRAM data that would have been read by the render thread is loaded into the variable,
//taking into account the current vertical scroll mode, screen mode settings, and the
//display blanking settings. If the render thread has not passed a VSRAM access point
//since the last update, the current value stored in the vsramCachedRead variable must be
//retained.
bool S315_5313::AdvanceProcessorStateNew(unsigned int mclkCyclesTarget, bool stopAtNextAccessSlot, bool allowAdvancePastTargetForAccessSlot)
{
	//Ensure that we aren't trying to trigger an update out of order
	if((!stopAtNextAccessSlot || !allowAdvancePastTargetForAccessSlot) && (mclkCyclesTarget < (stateLastUpdateMclk + stateLastUpdateMclkUnused)))
	{
		//##TODO## Raise an assert if this occurs
		//##DEBUG##
		std::wcout << "######################################################\n";
		std::wcout << "VDP AdvanceProcessorStateNew called out of order!\n";
		std::wcout << "mclkCyclesTarget:\t" << mclkCyclesTarget << "\n";
		std::wcout << "stateLastUpdateMclk:\t" << stateLastUpdateMclk << "\n";
		std::wcout << "stateLastUpdateMclkUnused:\t" << stateLastUpdateMclkUnused << "\n";
		std::wcout << "######################################################\n";
		return true;
	}

	//Grab the latest settings for any registers which affect the video mode. Note that we
	//only advance the VDP state between register writes, so we know the currently latched
	//video mode settings are valid, and that any video mode register changes which have
	//been made need to be applied the next time those settings are latched.
	bool interlaceEnabledNew = interlaceEnabledCached;
	bool interlaceDoubleNew = interlaceDoubleCached;
	bool screenModeH40New = screenModeH40Cached;
	bool screenModeV30New = screenModeV30Cached;
	bool palModeNew = palModeLineState;

	//Check whether any of the relevant video mode settings have changed since they were
	//latched.
	bool hscanSettingsChanged = (screenModeH40 != screenModeH40New);
	//##TODO## Currently, changes to the palMode flag are applied at vblank. Test how the
	//real hardware deals with changes to this line by toggling the line state at runtime.
	bool vscanSettingsChanged = (screenModeV30 != screenModeV30New) || (palMode != palModeNew) || (interlaceEnabled != interlaceEnabledNew);

	//Calculate the total number of mclk cycles which need to be advanced
	unsigned int mclkCyclesToExecute = (mclkCyclesTarget - (stateLastUpdateMclk + stateLastUpdateMclkUnused));

	//Advance the device until we reach the target position
	bool stoppedAtAccessSlot = false;
	unsigned int mclkCyclesAdvanced = 0;
	while(((mclkCyclesAdvanced < mclkCyclesToExecute) && (!stopAtNextAccessSlot || !stoppedAtAccessSlot)) || (allowAdvancePastTargetForAccessSlot && stopAtNextAccessSlot && !stoppedAtAccessSlot))
	{
		//Obtain the current hscan and vscan settings
		const HScanSettings* hscanSettings;
		const VScanSettings* vscanSettings;
		hscanSettings = &GetHScanSettings(screenModeH40);
		vscanSettings = &GetVScanSettings(screenModeV30, palMode, interlaceEnabled);

		//Calculate the number of mclk cycles to advance in this next step in order to
		//reach the target mclk cycle count.
		unsigned int mclkCyclesToAdvanceThisStep = mclkCyclesToExecute - mclkCyclesAdvanced;
		
		//If any screen mode changes are pending, check if we're going to encounter any
		//update points in this step.
		bool screenModeSettingsChangeRequired = false;
		bool updateStepIsHBlank = false;
		bool updateStepIsVBlank = false;
		if(hscanSettingsChanged || vscanSettingsChanged)
		{
			//Calculate the number of steps before the hblank and vblank events occur, and
			//new settings are latched.
			unsigned int pixelClockStepsBeforeHBlank = 0;
			if(hscanSettingsChanged)
			{
				pixelClockStepsBeforeHBlank = GetPixelClockStepsBetweenHCounterValues(*hscanSettings, hcounter.GetData(), hscanSettings->hblankSetPoint);
			}
			unsigned int pixelClockStepsBeforeVBlank = 0;
			if(vscanSettingsChanged)
			{
				pixelClockStepsBeforeVBlank = GetPixelClockStepsBetweenVCounterValues(*hscanSettings, hcounter.GetData(), *vscanSettings, interlaceEnabled, GetStatusFlagOddInterlaceFrame(), vcounter.GetData(), vscanSettings->vblankSetPoint);
			}

			//Check whether the next update step occurs at hblank or vblank. If both hscan
			//and vscan settings have changed, select the one which occurs first.
			updateStepIsVBlank = (vscanSettingsChanged && !hscanSettingsChanged) || (vscanSettingsChanged && hscanSettingsChanged && (pixelClockStepsBeforeVBlank < pixelClockStepsBeforeHBlank));
			updateStepIsHBlank = !updateStepIsVBlank;

			//Determine how many pixel clock ticks are going to occur before the next
			//screen mode settings change.
			unsigned int pixelClockTicksBeforeScreenModeChange = updateStepIsVBlank? pixelClockStepsBeforeVBlank: pixelClockStepsBeforeHBlank;

			//If a screen mode change is going to occur before the target mclk cycle
			//count, only advance up to the screen mode change in this step.
			unsigned int mclkTicksBeforeScreenModeChange = GetMclkTicksForPixelClockTicks(pixelClockTicksBeforeScreenModeChange, hcounter.GetData(), screenModeH40);
			if((mclkTicksBeforeScreenModeChange < mclkCyclesToAdvanceThisStep) || allowAdvancePastTargetForAccessSlot)
			{
				mclkCyclesToAdvanceThisStep = mclkTicksBeforeScreenModeChange;
				screenModeSettingsChangeRequired = true;
			}
		}

		//If the caller has requested the update to stop at the next access slot, check if
		//we're going to encounter one in this step.
		if(stopAtNextAccessSlot)
		{
			//Calculate the number of pixel clock cycles until the next available access
			//slot
			unsigned int totalPixelClockCyclesUntilAccessSlot;
			totalPixelClockCyclesUntilAccessSlot = GetPixelClockTicksUntilNextAccessSlot(hcounter.GetData(), screenModeH40, vcounter.GetData(), screenModeV30, palMode, interlaceEnabled);

			//If an access slot occurs within the update region, stop the update at the
			//access slot.
			//##TODO## Update the comments below
			//##FIX## Evaluate the way we use this function, and make sure that if an access
			//slot lies exactly on the target MCLK cycle count, that an access slot is
			//actually allowed at that location. We return false here in this case, so it's
			//possible the calling code doesn't actually allow access when false is returned.
			unsigned int mclkTicksBeforeAccessSlot = GetMclkTicksForPixelClockTicks(totalPixelClockCyclesUntilAccessSlot, hcounter.GetData(), screenModeH40);
			if((mclkTicksBeforeAccessSlot < mclkCyclesToAdvanceThisStep) //The access slot occurs before the end of the update time
			|| (allowAdvancePastTargetForAccessSlot && (!screenModeSettingsChangeRequired || (mclkTicksBeforeAccessSlot < mclkCyclesToAdvanceThisStep)))) //The access slot is allowed to pass the end of the update time, and no screen mode settings changes are required, or they occur after the slot.
			{
				//Since we are stopping at an access slot, set the number of mclk cycles
				//to execute as the number of cycles until the access slot occurs, and
				//flag that the update cycle was stopped at an access slot before the
				//target was reached.
				mclkCyclesToAdvanceThisStep = mclkTicksBeforeAccessSlot;
				stoppedAtAccessSlot = true;
				screenModeSettingsChangeRequired = false;
			}
		}

		//Check if we're going to pass the point at which the next event occurs with this
		//next update step. We need this check here, to handle the case where a calling
		//device causes the state of the VDP to be advanced past the end of the current
		//timeslice, but we have a timing point to run at the end of this timeslice to
		//execute an event. We latch the screen mode settings at the time of the event in
		//this case, so we can correctly handle the event later during the timing point.
		//##FIX## What about where changes to screen mode settings, such as the H40 mode,
		//affect the timing of the next event? Are we catching this when the registers are
		//modified?
		if((nextEventToExecute.mclkCycleCounter >= (stateLastUpdateMclk + stateLastUpdateMclkUnused)) //The event occurs at or after the current time
		&& (nextEventToExecute.mclkCycleCounter < (stateLastUpdateMclk + stateLastUpdateMclkUnused + mclkCyclesToAdvanceThisStep))) //The event occurs during the next update step we're about to make
		{
			eventSnapshotScreenModeH40 = screenModeH40;
			eventSnapshotScreenModeV30 = screenModeV30;
			eventSnapshotPalMode = palMode;
			eventSnapshotInterlaceEnabled = interlaceEnabled;
			eventSnapshotSaved = true;
		}

		//Calculate the total number of pixel clock cycles which will occur if the VDP is
		//advanced the target number of mclk cycles.
		unsigned int mclkRemainingCycles;
		unsigned int totalPixelClockCyclesToExecute;
		totalPixelClockCyclesToExecute = GetPixelClockTicksForMclkTicks(stateLastUpdateMclkUnusedFromLastTimeslice + stateLastUpdateMclkUnused + mclkCyclesToAdvanceThisStep, hcounter.GetData(), screenModeH40, mclkRemainingCycles);

		//Advance the required number of pixel clock steps to reach the target
		AdvanceStatusRegisterAndHVCounterWithCurrentSettings(totalPixelClockCyclesToExecute);

		//Update the number of remaining mclk cycles from the previous timeslice
		if((stateLastUpdateMclkUnused + mclkCyclesToAdvanceThisStep) > stateLastUpdateMclkUnusedFromLastTimeslice)
		{
			stateLastUpdateMclkUnusedFromLastTimeslice = 0;
		}
		else
		{
			stateLastUpdateMclkUnusedFromLastTimeslice -= (stateLastUpdateMclkUnused + mclkCyclesToAdvanceThisStep);
		}

		//Update the current state mclk cycle counter and remaining mclk cycles
		stateLastUpdateMclk += ((stateLastUpdateMclkUnused + mclkCyclesToAdvanceThisStep) - mclkRemainingCycles);
		stateLastUpdateMclkUnused = mclkRemainingCycles;

		//If vertical scan information has changed, and we've just advanced to vblank,
		//latch the new screen mode settings.
		if(screenModeSettingsChangeRequired && updateStepIsVBlank)
		{
			//If the interlace mode has changed, the new setting is latched when the
			//vblank set event occurs. This has been verified in all video modes through
			//hardware tests.
			interlaceEnabled = interlaceEnabledNew;
			interlaceDouble = interlaceDoubleNew;
			//##TODO## Verify that changes to the PAL line state are latched at vblank
			palMode = palModeLineState;
			//##TODO## Verify that the V28/V30 mode change is latched at vblank
			screenModeV30 = screenModeV30New;
		}

		//If horizontal scan information has changed, and we've just advanced to hblank,
		//latch the new screen mode settings.
		if(screenModeSettingsChangeRequired && updateStepIsHBlank)
		{
			//##TODO## Verify that H32/H40 screen mode changes are processed at hblank
			screenModeH40 = screenModeH40New;
		}

		//Update the total number of mclk cycles advanced so far
		mclkCyclesAdvanced += mclkCyclesToAdvanceThisStep;
	}

	//Calculate the new processor state time. Note that we don't have to do this within
	//the loop above, as internally, this core always tracks the current time in mclk
	//cycles. We only need to calculate the current processor time as a timeslice progress
	//value for external interaction.
	stateLastUpdateTime = ConvertMclkCountToAccessTime(GetProcessorStateMclkCurrent());

	//If we stopped at an access slot rather than running until the specified time was
	//reached, return false.
	return !stoppedAtAccessSlot;
}

//----------------------------------------------------------------------------------------
//##TODO## Remove this old function
//void S315_5313::AdvanceStatusRegisterAndHVCounter(unsigned int pixelClockSteps)
//{
//	//Grab the latest settings for any registers which affect the video mode. Note that
//	//we only advance the VDP state between register writes, so we know the currently
//	//latched video mode settings are valid, and that any video mode register changes
//	//which have been made need to be applied the next time those settings are latched.
//	bool interlaceEnabledNew = interlaceEnabledCached;
//	bool interlaceDoubleNew = interlaceDoubleCached;
//	bool screenModeH40New = screenModeH40Cached;
//	bool screenModeV30New = screenModeV30Cached;
//
//	//Check whether any of the relevant video mode settings have changed since they were
//	//latched.
//	bool hscanSettingsChanged = (screenModeH40 != screenModeH40New);
//	//##TODO## Currently, changes to the palMode flag are applied at vblank. Test how
//	//the real hardware deals with changes to this line by toggling the line state at
//	//runtime.
//	bool vscanSettingsChanged = (screenModeV30 != screenModeV30New) || (palMode != palModeLineState) || (interlaceEnabled != interlaceEnabledNew);
//
//	//Obtain the current hscan and vscan settings
//	const HScanSettings* hscanSettings;
//	const VScanSettings* vscanSettings;
//	hscanSettings = &GetHScanSettings(screenModeH40);
//	vscanSettings = &GetVScanSettings(screenModeV30, palMode, interlaceEnabled);
//
//	//Calculate the number of steps before the hblank and vblank events occur, and new
//	//settings are latched.
//	unsigned int pixelClockStepsBeforeHBlank;
//	unsigned int pixelClockStepsBeforeVBlank;
//	pixelClockStepsBeforeHBlank = GetPixelClockStepsBetweenHCounterValues(*hscanSettings, hcounter.GetData(), hscanSettings->hblankSetPoint);
//	pixelClockStepsBeforeVBlank = GetPixelClockStepsBetweenVCounterValues(*hscanSettings, hcounter.GetData(), *vscanSettings, interlaceEnabled, GetStatusFlagOddInterlaceFrame(), vcounter.GetData(), vscanSettings->vblankSetPoint);
//
//	//Check if the vblank event is going to occur before the hblank event. We need to know
//	//which one comes first so we know which one to advance to first, if required.
//	bool vblankBeforeHBlank = pixelClockStepsBeforeVBlank < pixelClockStepsBeforeHBlank;
//
//	//Set the current number of pixel clock steps we still need to advance
//	unsigned int pixelClockStepsRemaining = pixelClockSteps;
//
//	//If vertical scan information has changed, and vblank is going to occur before
//	//hblank, we need to advance to the latch point at the start of vblank.
//	if(vscanSettingsChanged && vblankBeforeHBlank && (pixelClockStepsRemaining <= pixelClockStepsBeforeVBlank))
//	{
//		//Advance the required number of pixel clock steps to reach the vblank event
//		AdvanceStatusRegisterAndHVCounterWithCurrentSettings(pixelClockStepsBeforeVBlank);
//		pixelClockStepsRemaining -= pixelClockStepsBeforeVBlank;
//		//If the interlace mode has changed, the new setting is latched when the vblank set
//		//event occurs. This has been verified in all video modes through hardware tests.
//		interlaceEnabled = interlaceEnabledNew;
//		interlaceDouble = interlaceDoubleNew;
//		//##TODO## Verify that changes to the PAL line state are latched at vblank
//		palMode = palModeLineState;
//		//##TODO## Verify that the V28/V30 mode change is latched at vblank
//		screenModeV30 = screenModeV30New;
//	}
//
//	//If the horizontal cell width has changed, we need to advance to the latch point at
//	//the start of hblank.
//	if(hscanSettingsChanged && (pixelClockStepsRemaining <= pixelClockStepsBeforeHBlank))
//	{
//		//Advance the required number of pixel clock steps to reach the hblank event
//		AdvanceStatusRegisterAndHVCounterWithCurrentSettings(pixelClockStepsBeforeHBlank);
//		pixelClockStepsRemaining -= pixelClockStepsBeforeHBlank;
//		//##TODO## Verify that H32/H40 screen mode changes are processed at hblank
//		screenModeH40 = screenModeH40New;
//
//		//Update the latched hscan settings
//		hscanSettings = &GetHScanSettings(screenModeH40);
//
//		//Recalculate pixelClockStepsBeforeVBlank
//		pixelClockStepsBeforeVBlank = GetPixelClockStepsBetweenVCounterValues(*hscanSettings, hscanSettings->hblankSetPoint, *vscanSettings, interlaceEnabled, GetStatusFlagOddInterlaceFrame(), vcounter.GetData(), vscanSettings->vblankSetPoint);
//	}
//
//	//If vertical scan information has changed, and vblank is going to occur after hblank,
//	//we need to advance to the latch point at the start of vblank.
//	if(vscanSettingsChanged && !vblankBeforeHBlank && (pixelClockStepsRemaining <= pixelClockStepsBeforeVBlank))
//	{
//		//Advance the required number of pixel clock steps to reach the vblank event
//		AdvanceStatusRegisterAndHVCounterWithCurrentSettings(pixelClockStepsBeforeVBlank);
//		pixelClockStepsRemaining -= pixelClockStepsBeforeVBlank;
//		//If the interlace mode has changed, the new setting is latched when the vblank set
//		//event occurs. This has been verified in all video modes through hardware tests.
//		interlaceEnabled = interlaceEnabledNew;
//		interlaceDouble = interlaceDoubleNew;
//		//##TODO## Verify that changes to the PAL line state are latched at vblank
//		palMode = palModeLineState;
//		//##TODO## Verify that the V28/V30 mode change is latched at vblank
//		screenModeV30 = screenModeV30New;
//	}
//
//	//Since all external video mode settings changes which can make the progression of the
//	//HV counter change over time have already been processed above, we can now simply
//	//jump the HV counter to its final position using the current screen mode settings,
//	//and the remaining number of pixel clock update steps.
//	AdvanceStatusRegisterAndHVCounterWithCurrentSettings(pixelClockStepsRemaining);
//}

//----------------------------------------------------------------------------------------
void S315_5313::AdvanceStatusRegisterAndHVCounterWithCurrentSettings(unsigned int pixelClockSteps)
{
	//Obtain the current hscan and vscan settings
	const HScanSettings* hscanSettings = &GetHScanSettings(screenModeH40);
	const VScanSettings* vscanSettings = &GetVScanSettings(screenModeV30, palMode, interlaceEnabled);

	//Advance the HV counter
	unsigned int hcounterCurrent = hcounter.GetData();
	unsigned int vcounterCurrent = vcounter.GetData();
	bool oddFlagSet = GetStatusFlagOddInterlaceFrame();
	AdvanceHVCounters(*hscanSettings, hcounterCurrent, *vscanSettings, interlaceEnabled, oddFlagSet, vcounterCurrent, pixelClockSteps);

	//Save the new values for the HV counter
	hcounter = hcounterCurrent;
	vcounter = vcounterCurrent;

	//##TODO## Update the cached read data for the VSRAM buffer

	//Update the status register

	//Note that the FIFO flags are set by the routine which manages the FIFO.

	//We set the F flag when a vint is generated, and clear it when the interrupt is
	//acknowledged.

	//Note that the sprite overflow and collision flags are cleared when the status
	//register is read.
	//##NOTE## As soon as we come across a case where the sprite overflow or collision
	//flags are set, we can stop searching for more. We know the flag will remain set for
	//the rest of the update cycle.
	//##TODO## Update the sprite overflow flag
	//##TODO## Confirm whether the sprite overflow flag is set just for a sprite overflow,
	//or for a dot overflow as well.
	if(!GetStatusFlagSpriteOverflow())
	{
		//##STUB##
		SetStatusFlagSpriteOverflow(false);
	}

	//##TODO## Update the sprite collision flag
	//##TODO## Verify sprite collision behaviour on the hardware. Test opaque pixels
	//overlapping. Test sprites overlapping off the screen, both partially visible and
	//completely hidden. Determine whether this flag is set during active scan at the
	//pixel location where the overlap occurs, or whether it is set during blanking when
	//the sprites are parsed. Check how sprite overflows and sprite masking interacts with
	//the sprite collision flag.
	//##TODO## A comment by Eke in http://gendev.spritesmind.net/forum/viewtopic.php?t=778
	//says that the sprite collision flag is also cleared during vblank, not just when the
	//status register is read. Most likely, this applies to the sprite overflow flag as
	//well. Perform some hardware tests to confirm the exact time at which these flags are
	//set and cleared.
	//##TODO## Some testing on VRAM access timing shows that the sprite collision flag
	//seems to be actively set during the sprite pattern reads in hblank, so it seems that
	//this flag is set as each sprite pattern block is read. Most likely, the sprite
	//overflow flag is set while performing sprite mapping reads, if there are still
	//blocks remaining when the last sprite mapping read slot is used on a scanline.
	if(!GetStatusFlagSpriteCollision())
	{
		//##STUB##
		SetStatusFlagSpriteCollision(false);
	}

	//Save the new value for the ODD flag
	SetStatusFlagOddInterlaceFrame(oddFlagSet);

	//Update the vblank and hblank flags
	bool vblankFlag = (vcounter >= vscanSettings->vblankSetPoint) && (vcounter < vscanSettings->vblankClearedPoint);
	bool hblankFlag = (hcounter >= hscanSettings->hblankSetPoint) || (hcounter < hscanSettings->hblankClearedPoint);
	SetStatusFlagVBlank(vblankFlag);
	SetStatusFlagHBlank(hblankFlag);

	//##DEBUG##
//	std::wcout << "Advance:\t" << pixelClockSteps << '\n';
//	std::wcout << "Hcounter:\t" << hcounterCurrent << '\t' << "Vcounter:\t" << vcounterCurrent << '\n';
//	std::wcout << "Hblank:\t" << hblankFlag << '\t' << "Vblank:\t" << vblankFlag << '\n';

	//Note that the DMA flag is set and cleared by the routine which manages DMA
	//operations.

	//Update the PAL flag
	SetStatusFlagPAL(palMode);
}

//----------------------------------------------------------------------------------------
double S315_5313::GetProcessorStateTime() const
{
	return stateLastUpdateTime;
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::GetProcessorStateMclkCurrent() const
{
	return stateLastUpdateMclk + stateLastUpdateMclkUnused;
}

//----------------------------------------------------------------------------------------
//FIFO functions
//----------------------------------------------------------------------------------------
bool S315_5313::IsWriteFIFOEmpty() const
{
	//The FIFO is empty if the next FIFO entry for read doesn't have a write pending
	return !fifoBuffer[fifoNextReadEntry].pendingDataWrite;
}

//----------------------------------------------------------------------------------------
bool S315_5313::IsWriteFIFOFull() const
{
	//The FIFO is full if the first and last FIFO entries for reading both have a write
	//pending
	unsigned int fifoLastReadEntry = (fifoNextReadEntry+(fifoBufferSize-1)) % fifoBufferSize;
	return fifoBuffer[fifoNextReadEntry].pendingDataWrite && fifoBuffer[fifoLastReadEntry].pendingDataWrite;
}

//----------------------------------------------------------------------------------------
//Mode 5 control functions
//----------------------------------------------------------------------------------------
bool S315_5313::ValidReadTargetInCommandCode() const
{
	//Return true if bits 0, 1, and 4 of the commandCode register are not set. See the
	//mapping of commandCode data to valid read targets in the ReadVideoMemory function to
	//see why this is a valid test.
	return (commandCode & 0x13) == 0;
}

////----------------------------------------------------------------------------------------
//bool S315_5313::ReadVideoMemory(unsigned int mclkCycle, Data& data)
//{
//	RAMAccessTarget ramAccessTarget;
//	ramAccessTarget.AccessTime(mclkCycle);
//
//	//All possible combinations of the code flags and data port reads have been tested on
//	//the hardware. Reads are decoded based on the lower 5 bits of the code data.
//	switch(commandCode.GetDataSegment(0, 5))
//	{
//	case 0x00:	//?00000 VRAM Read
//		M5ReadVRAM(commandAddress, data, ramAccessTarget);
//		break;
//	case 0x04:	//?00100 VSRAM Read
//		M5ReadVSRAM(commandAddress, data, ramAccessTarget);
//		break;
//	case 0x08:	//?01000 CRAM Read
//		M5ReadCRAM(commandAddress, data, ramAccessTarget);
//		break;
//	case 0x0C:	//?01100 8-bit VRAM Read (undocumented)
//		//This undocumented read mode performs a true 8-bit VRAM read. The lower 8 bits
//		//return an 8-bit value read from VRAM, while the upper 8 bits are unaffected.
//		M5ReadVRAM8Bit(commandAddress, data, ramAccessTarget);
//		break;
//	default:	//Invalid
//		//Any attempts to read from the data port when the lower five bits don't match one
//		//of the above patterns causes the VDP to lock up. A reset is unable to restore
//		//normal operation. Only power cycling the device can bring the VDP back from this
//		//state.
//		//##TODO## Raise some kind of hard error when this occurs.
//		return false;
//	}
//
//	//We always increment the command address, regardless of whether the target was valid
//	//or not. This is probably the way the real device would work, but since incremented
//	//addresses are temporary, and are reloaded from the original address register
//	//whenever a command word is written, there's no way to know if this behaviour is
//	//correct. For the same reason, it also doesn't matter, since there's no possible
//	//impact from either incrementing or not incrementing on invalid targets, but this way
//	//is simpler and produces less code.
////	commandAddress += autoIncrementData;
//
//	return true;
//}

////----------------------------------------------------------------------------------------
//void S315_5313::WriteVideoMemory(unsigned int mclkCycle, const Data& data)
//{
//	RAMAccessTarget ramAccessTarget;
//	ramAccessTarget.AccessTime(mclkCycle);
//
//	//All possible combinations of the code flags and data port writes have been tested
//	//on the hardware. Writes are decoded based on the lower 4 bits of the code data.
//	switch(commandCode.GetDataSegment(0, 4))
//	{
//	case 0x01:	//??0001 VRAM Write
//		M5WriteVRAM(commandAddress, data, ramAccessTarget);
//		break;
//	case 0x03:	//??0011 CRAM Write
//		M5WriteCRAM(commandAddress, data, ramAccessTarget);
//		break;
//	case 0x05:	//??0101 VSRAM Write
//		M5WriteVSRAM(commandAddress, data, ramAccessTarget);
//		break;
//	default:	//Invalid
//		//Any attempts to write to the data port when the lower four bits don't match
//		//one of the above patterns has no effect. The write is discarded, and the
//		//VDP is unaffected.
//		break;
//	}
//
//	//We always increment the command address, regardless of whether the target was valid
//	//or not. This is probably the way the real device would work, but since incremented
//	//addresses are temporary, and are reloaded from the original address register
//	//whenever a command word is written, there's no way to know if this behaviour is
//	//correct. For the same reason, it also doesn't matter, since there's no possible
//	//impact from either incrementing or not incrementing on invalid targets, but this way
//	//is simpler and produces less code.
//	commandAddress += autoIncrementData;
//}

//----------------------------------------------------------------------------------------
//void S315_5313::M5ReadVRAM(const Data& address, Data& data, const RAMAccessTarget& accessTarget)
//{
//	//Note that hardware tests have shown that the LSB of the address is ignored for
//	//16-bit reads from VRAM.
//	Data tempAddress(address);
//	tempAddress.SetBit(0, false);
//
//	//Read the data. The full 16-bit data value is retrieved from VRAM.
//	data.SetByte(1, vram->BufferRead(tempAddress.GetData(), accessTarget));
//	data.SetByte(0, vram->BufferRead(tempAddress.GetData()+1, accessTarget));
//}

//----------------------------------------------------------------------------------------
void S315_5313::M5ReadVRAM8Bit(const Data& address, Data& data, const RAMAccessTarget& accessTarget)
{
	//Note that in the case of 8-bit VRAM reads, the data is directly read from VRAM at
	//the literal address referenced by the address register. Note however that since the
	//actual VDP stores data in VRAM as byteswapped 16-bit values, this means that reads
	//from odd addresses return the even byte of a 16-bit value, and reads from an even
	//address return the odd byte. Since we don't byteswap the VRAM data in our emulation
	//core, we need to invert the LSB of the address here to get the same result as the
	//real hardware.
	Data tempAddress(address);
	tempAddress.SetBit(0, !tempAddress.GetBit(0));

	//Read the data. Only a single 8-bit read is performed from VRAM in this case. The
	//upper 8 bits retain their previous value.
	//##TODO## Snoop on the VRAM bus to confirm only a single byte is read for this target
	data.SetByte(0, vram->Read(tempAddress.GetData(), accessTarget));
}

//----------------------------------------------------------------------------------------
void S315_5313::M5ReadCRAM(const Data& address, Data& data, const RAMAccessTarget& accessTarget)
{
	//Note that hardware tests have shown that not only is the LSB of the address ignored
	//for both reads and writes to CRAM and VSRAM, but the upper 9 bits of the address is
	//also masked. The address value wraps to be within the range 0x00-0x7E, where the LSB
	//of the address is always clear.
	unsigned int tempAddress = address.GetData() & 0x7E;

	//Read the data. Note that not all bits in the 16-bit result are affected by the read
	//from CRAM. Hardware tests have shown that CRAM reads return a 9-bit value, with the
	//mask 0x0EEE. The remaining bits retain their previous value, corresponding with the
	//existing value in the FIFO buffer that the read data is being saved into.
	unsigned int dataMask = 0x0EEE;
	Data tempData(16);
	tempData.SetByte(1, cram->Read(tempAddress, accessTarget));
	tempData.SetByte(0, cram->Read(tempAddress+1, accessTarget));
	data = (data & ~dataMask) | (tempData & dataMask);
}

//----------------------------------------------------------------------------------------
void S315_5313::M5ReadVSRAM(const Data& address, Data& data, const RAMAccessTarget& accessTarget)
{
	//Note that hardware tests have shown that not only is the LSB of the address ignored
	//for both reads and writes to CRAM and VSRAM, but the upper 9 bits of the address is
	//also masked. The address value wraps to be within the range 0x00-0x7E, where the LSB
	//of the address is always clear.
	unsigned int tempAddress = address.GetData() & 0x7E;

	//##TODO## Hardware testing has shown that the VSRAM buffer is 0x50 bytes in size,
	//while the target address into the VSRAM buffer wraps at 0x80 bytes. This leaves 0x30
	//bytes where reads and writes can be attempted, which are beyond the end of the VSRAM
	//buffer. Hardware testing has shown that that the last read value from VSRAM is
	//cached in an internal register, and attempts to read from VSRAM at an invalid
	//address simply return the cached value which remains in the VSRAM read buffer. Note
	//that the actual VDP rendering process itself reads values from VSRAM, and these
	//reads update the VSRAM read buffer.
	//##TODO## Perform hardware tests to confirm whether manual reads from VSRAM update
	//the VSRAM read buffer, or whether it is only populated by the VDP itself during
	//rendering. Also confirm whether writes to VSRAM update the VSRAM read buffer, IE,
	//whether there is a single buffer which is used for both reads and writes. Note that
	//we should be able to determine this by disabling VDP video output, possibly through
	//the normal display enable/disable bit in register 1, or if not, perhaps through the
	//undocumented bit in reg 0 which disables all video output. This should get the VDP
	//out of the way, so we can read VSRAM without the rendering process affecting the
	//VSRAM read buffer.
	unsigned int dataMask = 0x07FF;
	if(tempAddress < 0x50)
	{
		//Read the data. Note that not all bits in the 16-bit result are affected by the
		//read from VSRAM. Hardware tests have shown that VSRAM reads return an 11-bit
		//value, with the mask 0x07FF. The remaining bits retain their previous value,
		//corresponding with the existing value in the FIFO buffer that the read data is
		//being saved into.
		Data tempData(16);
		tempData.SetByte(1, vsram->Read(tempAddress, accessTarget));
		tempData.SetByte(0, vsram->Read(tempAddress+1, accessTarget));
		data = (data & ~dataMask) | (tempData & dataMask);

		//##TODO## Determine whether this is correct
		//vsramCachedRead = tempData;
	}
	else
	{
		data = (data & ~dataMask) | (vsramCachedRead & dataMask);
	}
}

//----------------------------------------------------------------------------------------
//This target isn't directly accessible as a write target for the VDP, but we use this
//function to implement the two 8-bit halves of a 16-bit VRAM write performed for the
//16-bit VRAM write target, as well as implement DMA fill and copy functionality.
//----------------------------------------------------------------------------------------
void S315_5313::M5WriteVRAM8Bit(const Data& address, const Data& data, const RAMAccessTarget& accessTarget)
{
	//Note that in the case of 8-bit VRAM writes, the data is directly written to VRAM at
	//the literal address referenced by the address register. Note however that since the
	//actual VDP stores data in VRAM as byteswapped 16-bit values, this means that writes
	//to odd addresses modify the even byte of a 16-bit value, and writes to an even
	//address modify the odd byte. Since we don't byteswap the VRAM data in our emulation
	//core, we need to invert the LSB of the address here to get the same result as the
	//real hardware.
	Data tempAddress(address);
	tempAddress.SetBit(0, !tempAddress.GetBit(0));

	//Write the data
	vram->Write(tempAddress.GetData(), data.GetByte(0), accessTarget);
}

////----------------------------------------------------------------------------------------
//void S315_5313::M5WriteVRAM(const Data& address, const Data& data, const RAMAccessTarget& accessTarget)
//{
//	//-Hardware tests have verified that if the LSB of the address is set, it is ignored
//	//when determining the target VRAM address, but it acts as a special flag causing the
//	//data to be byteswapped for writes to VRAM. This is true for any write to VRAM,
//	//including writes performed as part of a DMA transfer. The LSB of the address is
//	//ignored for reads from VRAM, IE, no byteswapping is ever performed on reads.
//	//-It should be noted that the real VDP actually stores all VRAM data byteswapped in
//	//the physical memory. This has been confirmed by using a logic analyzer to snoop on
//	//the VRAM bus during operation. This means that in reality, byteswapping on VRAM
//	//writes really occurs when the LSB is unset, while the data is byteswapped if the LSB
//	//is set, and all reads are byteswapped. We don't byteswap the actual VRAM data in our
//	//emulator, as not only is this byteswapping virtually transparent to the caller
//	//except in the case of DMA fills and copies (refer to the implementation for further
//	//info), it would be slower for us to byteswap everything on every read and write to
//	//VRAM. Since it's faster for us, and more convenient and logical for the user
//	//therefore for the data to be stored sequentially, we don't store data as byteswapped
//	//in VRAM.
//	//-Note that the real VDP also stores the VRAM data in a non-linear fashion, with data
//	//within each 0x400 byte block stored in an interleaved format. The byteswapped data
//	//is striped in 4-byte groups within each 0x400 byte block, with all the first bytes
//	//of each 4-byte set at 0x000-0x100, then the second bytes at 0x100-0x200, and so on
//	//within each 0x400 byte block. This is necessary in order to support the serial
//	//access mode used to read data from VRAM. We also don't implement this interleaved
//	//VRAM in our emulator, as it is an implementation detail that has no effect on the
//	//logical operation of the VDP.
//	Data tempAddress(address);
//	if(tempAddress.GetBit(0))
//	{
//		tempAddress.SetBit(0, false);
//		vram->BufferWrite(tempAddress.GetData(), data.GetByte(0), accessTarget);
//		vram->BufferWrite(tempAddress.GetData()+1, data.GetByte(1), accessTarget);
//	}
//	else
//	{
//		vram->BufferWrite(tempAddress.GetData(), data.GetByte(1), accessTarget);
//		vram->BufferWrite(tempAddress.GetData()+1, data.GetByte(0), accessTarget);
//	}
//}

//----------------------------------------------------------------------------------------
void S315_5313::M5WriteCRAM(const Data& address, const Data& data, const RAMAccessTarget& accessTarget)
{
	//Note that hardware tests have shown that not only is the LSB of the address ignored
	//for both reads and writes to CRAM and VSRAM, but the upper 9 bits of the address is
	//also masked. The address value wraps to be within the range 0x00-0x7E, where the LSB
	//of the address is always clear.
	unsigned int tempAddress = address.GetData() & 0x7E;

	//Build a masked data value to write to CRAM. Hardware tests have shown that reads
	//back from CRAM only return data from bits corresponding with the mask 0x0EEE. We
	//mask the data when saving to CRAM here, to make things more sensible when using the
	//debugger. Since the masked bits are almost certainly discarded when saving to CRAM
	//on the real system, we discard them here on save as well, even though it's not
	//technically necessary, since we never attempt to use the invalid bits.
	unsigned int dataMask = 0x0EEE;
	Data tempData(16);
	tempData = data & dataMask;

	//Write the masked data to CRAM
	cram->Write(tempAddress, tempData.GetByte(1), accessTarget);
	cram->Write(tempAddress+1, tempData.GetByte(0), accessTarget);
}

//----------------------------------------------------------------------------------------
void S315_5313::M5WriteVSRAM(const Data& address, const Data& data, const RAMAccessTarget& accessTarget)
{
	//Note that hardware tests have shown that not only is the LSB of the address ignored
	//for both reads and writes to CRAM and VSRAM, but the upper 9 bits of the address is
	//also masked. The address value wraps to be within the range 0x00-0x7E, where the LSB
	//of the address is always clear.
	unsigned int tempAddress = address.GetData() & 0x7E;

	//Build a masked data value to write to VSRAM. Hardware tests have shown that reads
	//back from VSRAM only return data from bits corresponding with the mask 0x07FF. We
	//mask the data when saving to VSRAM here, to make things more sensible when using the
	//debugger. Since the masked bits are almost certainly discarded when saving to VSRAM
	//on the real system, we discard them here on save as well, even though it's not
	//technically necessary, since we never attempt to use the invalid bits.
	unsigned int dataMask = 0x07FF;
	Data tempData(16);
	tempData = data & dataMask;

	//##TODO## Comment this, as per the M5ReadVSRAM function, and perform hardware tests
	//to confirm whether VSRAM writes should update the VSRAM read buffer.
	if(tempAddress < 0x50)
	{
		//Write the masked data to VSRAM
		vsram->Write(tempAddress, tempData.GetByte(1), accessTarget);
		vsram->Write(tempAddress+1, tempData.GetByte(0), accessTarget);

		//##TODO## Determine whether this is correct
		//vsramCachedRead = tempData;
	}
}
